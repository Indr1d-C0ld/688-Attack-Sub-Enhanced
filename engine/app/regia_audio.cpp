#include "regia_audio.hpp"

#include <toml.hpp>

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <filesystem>

#include "nucleo/campioni.hpp"

namespace m688 {

namespace {

// il canale degli effetti sommati, suo (Canale::effetti, PCM: passa dalla resa); ci va qualcosa solo mentre un effetto
// suona
constexpr Canale CANALE_EFFETTI = Canale::effetti;
constexpr double SFUMA_S = 1.0;          // la dissolvenza della musica al cambio di brano
constexpr double CODA_EFFETTO_S = 0.3;   // la dissolvenza in coda di un effetto tagliato

// ricampiona a `a` hz (lineare: gli effetti sono gia' a 22.050, e' solo una sicurezza)
std::vector<float> ricampiona(const std::vector<float>& x, int da, int a) {
    if (da == a || da <= 0 || x.empty()) return x;
    const std::size_t n = static_cast<std::size_t>(static_cast<double>(x.size()) * a / da);
    std::vector<float> y(n);
    for (std::size_t i = 0; i < n; ++i) {
        const double p = static_cast<double>(i) * da / a;
        const std::size_t k = std::min(static_cast<std::size_t>(p), x.size() - 1);
        const double f = p - static_cast<double>(k);
        y[i] = static_cast<float>(x[k] * (1.0 - f) + x[std::min(k + 1, x.size() - 1)] * f);
    }
    return y;
}

// quanti campioni a `hz` servono per n d'uscita a `fs`, col resto che si porta avanti (niente deriva)
std::size_t quanti(std::size_t n, int hz, int fs, double& resto) {
    resto += static_cast<double>(n) * hz / fs;
    const auto k = static_cast<std::size_t>(resto);
    resto -= static_cast<double>(k);
    return k;
}

}  // namespace

RegiaAudio::RegiaAudio(std::string dati, int campionamento_hz) : dati_(std::move(dati)), campionamento_hz_(campionamento_hz) {
    ParametriMixer pm;
    pm.campionamento_hz = campionamento_hz;
    mixer_.imposta(pm);
    try {
        fm_ = std::make_unique<SuonatoreFm>(carica_banco_fm(dati_ + "/timbri_fm.toml"));
    } catch (const std::exception& e) {
        std::fprintf(stderr, "musica FM non disponibile: %s\n", e.what());
    }
    try {
        const toml::table t = toml::parse_file(dati_ + "/effetti.toml");
        if (const toml::table* e = t["effetti"].as_table())
            for (const auto& [id, v] : *e)
                if (const auto f = v.as_table() ? (*v.as_table())["file"].value<std::string>() : std::nullopt) voci_effetti_[std::string(id.str())] = *f;
    } catch (const std::exception& e) {
        std::fprintf(stderr, "effetti non disponibili: %s\n", e.what());
    }
    aggiorna_volumi();
}

RegiaAudio::~RegiaAudio() = default;

void RegiaAudio::resa(Resa r) {
    ParametriMixer p = mixer_.parametri();
    p.resa = r;
    mixer_.imposta(p);
}

void RegiaAudio::aggiorna_volumi() {
    ParametriMixer p = mixer_.parametri();
    p.volume[static_cast<std::size_t>(Canale::musica)] = volume_musica_;
    p.generale = muto_ ? 0.0f : 1.0f;
    mixer_.imposta(p);
}

void RegiaAudio::volume_musica(float v) {
    volume_musica_ = std::clamp(v, 0.0f, 1.0f);
    aggiorna_volumi();
}

void RegiaAudio::muto(bool si) {
    muto_ = si;
    aggiorna_volumi();
}

bool RegiaAudio::musica_in_corso() const {
    std::lock_guard blocco(mutex_);
    return (pcm_ && (pcm_->loop || pcm_->letti < pcm_->campioni.size())) || (fm_ && fm_->suona_ancora());
}

std::string RegiaAudio::cartella_mt32_predefinita() {   // la cache dell'utente, mai il progetto
    if (const char* x = std::getenv("XDG_CACHE_HOME"); x && *x) return std::string(x) + "/melchior688/mt32";
    if (const char* h = std::getenv("HOME"); h && *h) return std::string(h) + "/.cache/melchior688/mt32";
    return {};
}

std::string RegiaAudio::cartella_mt32_per(const std::string& dati) {   // i WAV resi del pacchetto, accanto ai dati
    std::error_code ec;
    const auto p = std::filesystem::path(dati).parent_path() / "mt32";
    if (!dati.empty() && std::filesystem::is_directory(p, ec)) return p.string();
    return cartella_mt32_predefinita();
}

void RegiaAudio::sfuma(double s) {   // la musica in corso sfuma in s secondi; poi tace
    std::lock_guard blocco(mutex_);
    if (pcm_) {
        pcm_->sfuma_tot = pcm_->sfuma_resta = std::max(1.0, s * pcm_->hz);
    } else if (fm_) {
        fm_->ferma(s);
    }
}

void RegiaAudio::musica(const std::string& brano, bool in_loop) {
    if (brano == voluto_) return;   // lo stesso brano continua
    voluto_ = brano;
    loop_voluto_ = in_loop;
    if (musica_in_corso() && !in_corso_.empty()) sfuma(SFUMA_S);   // il nuovo parte quando tace
    else parti(brano, in_loop);
}

void RegiaAudio::musica_pcm(const std::string& nome, std::vector<float> campioni, int campionamento_hz) {
    if (nome == voluto_) return;
    voluto_ = nome;
    in_corso_ = nome;
    std::lock_guard blocco(mutex_);
    if (fm_) fm_->ferma(0.0);
    pcm_ = BranoPcm{nome, std::move(campioni), campionamento_hz};
}

void RegiaAudio::parti(const std::string& brano, bool in_loop) {
    in_corso_ = brano;
    // i file si leggono fuori dal lucchetto: il filo dell'audio non deve aspettare il disco
    std::optional<BranoPcm> pcm;
    const BranoMidi* midi = nullptr;
    if (!brano.empty()) {
        // l'MT-32: il WAV reso dalle ROM dell'utente, se c'e' (mono, alzato a un picco di 0,8: i file sono bassi)
        if (uscita_musica_ == UscitaMusica::mt32 && !cartella_mt32_.empty()) {
            auto it = wav_mt32_.find(brano);
            if (it == wav_mt32_.end()) {
                BranoPcm b;
                b.nome = brano;
                if (leggi_wav(cartella_mt32_ + "/" + brano + ".wav", b.campioni, b.hz) && b.hz > 0 && !b.campioni.empty()) {
                    float picco = 0.0f;
                    for (const float v : b.campioni) picco = std::max(picco, std::fabs(v));
                    const float g = picco > 1e-4f ? std::min(8.0f, 0.8f / picco) : 1.0f;
                    for (float& v : b.campioni) v *= g;
                } else {
                    b.campioni.clear();
                }
                it = wav_mt32_.emplace(brano, std::move(b)).first;
            }
            if (!it->second.campioni.empty()) {
                pcm = it->second;
                pcm->loop = in_loop;
            }
        }
        if (!pcm && fm_) {
            auto it = brani_.find(brano);
            if (it == brani_.end()) {
                try {
                    it = brani_.emplace(brano, leggi_midi_file(dati_ + "/musica/fm/" + brano + ".mid")).first;
                } catch (const std::exception& e) {
                    std::fprintf(stderr, "brano %s non disponibile: %s\n", brano.c_str(), e.what());
                    it = brani_.end();
                }
            }
            if (it != brani_.end()) midi = &it->second;
        }
    }
    std::lock_guard blocco(mutex_);
    pcm_ = std::move(pcm);
    if (fm_) {
        if (midi) fm_->suona(*midi, in_loop);
        else fm_->ferma(0.0);
    }
}

void RegiaAudio::uscita_musica(UscitaMusica u, std::string cartella_mt32) {
    if (u != uscita_musica_ && !in_corso_.empty() && musica_in_corso()) {   // lo stesso brano, con l'altra uscita: sfuma e riparte
        sfuma(SFUMA_S);
        in_corso_.clear();
    }
    uscita_musica_ = u;
    cartella_mt32_ = cartella_mt32.empty() ? cartella_mt32_per(dati_) : std::move(cartella_mt32);
}

const std::vector<float>* RegiaAudio::campioni_effetto(const std::string& id) {
    if (const auto it = effetti_.find(id); it != effetti_.end()) return &it->second;
    const auto v = voci_effetti_.find(id);
    if (v == voci_effetti_.end()) return nullptr;
    std::vector<float> x;
    int hz = 0;   // i file sono relativi a data/, sopra data/motore
    if (!leggi_wav((std::filesystem::path(dati_).parent_path() / v->second).string(), x, hz) || hz <= 0) {
        effetti_[id] = {};
        return &effetti_[id];
    }
    return &(effetti_[id] = ricampiona(x, hz, EFFETTI_HZ));
}

bool RegiaAudio::effetto(const std::string& id, float volume, double massimo_s) {
    const std::vector<float>* x = campioni_effetto(id);
    if (!x || x->empty()) return false;
    Voce v;
    v.volume = volume;
    v.campioni = *x;
    const auto massimo = static_cast<std::size_t>(massimo_s * EFFETTI_HZ);
    if (massimo_s > 0.0 && v.campioni.size() > massimo) {   // tagliato, con la coda che sfuma
        v.campioni.resize(massimo);
        const auto coda = std::min(massimo, static_cast<std::size_t>(CODA_EFFETTO_S * EFFETTI_HZ));
        for (std::size_t i = 0; i < coda; ++i) v.campioni[massimo - 1 - i] *= static_cast<float>(i) / static_cast<float>(coda);
    }
    std::lock_guard blocco(mutex_);
    attive_.push_back(std::move(v));
    return true;
}

void RegiaAudio::suono(std::vector<float> campioni, int campionamento_hz, float volume) {
    if (campioni.empty()) return;
    Voce v;
    v.volume = volume;
    v.campioni = ricampiona(campioni, campionamento_hz, EFFETTI_HZ);
    std::lock_guard blocco(mutex_);
    attive_.push_back(std::move(v));
}

std::vector<float> RegiaAudio::produci(std::size_t n) {
    aggiorna_brano();
    return produci_dal_filo(n);
}

void RegiaAudio::aggiorna_brano() {
    if (!musica_in_corso() && in_corso_ != voluto_) parti(voluto_, loop_voluto_);
}

std::vector<float> RegiaAudio::produci_dal_filo(std::size_t n) {
    const int fs = campionamento_hz_;
    std::lock_guard blocco(mutex_);
    if (pcm_) {   // in loop dall'inizio; la dissolvenza lineare, poi tace
        const std::size_t k = quanti(n, pcm_->hz, fs, resto_pcm_);
        std::vector<float> b;
        b.reserve(k);
        for (std::size_t i = 0; i < k; ++i) {
            if (pcm_->letti >= pcm_->campioni.size()) {
                if (!pcm_->loop) break;
                pcm_->letti = 0;
            }
            float v = pcm_->campioni[pcm_->letti++];
            if (pcm_->sfuma_tot > 0.0) {
                v *= static_cast<float>(pcm_->sfuma_resta / pcm_->sfuma_tot);
                if ((pcm_->sfuma_resta -= 1.0) <= 0.0) {
                    pcm_->loop = false;
                    pcm_->letti = pcm_->campioni.size();
                }
            }
            b.push_back(v);
        }
        if (!b.empty()) mixer_.aggiungi(Canale::musica, b, pcm_->hz);
    } else if (fm_) {
        if (const std::size_t k = quanti(n, fm_->campionamento_hz(), fs, resto_fm_)) mixer_.aggiungi(Canale::musica, fm_->genera(k), fm_->campionamento_hz());
    }
    // gli effetti: la somma delle voci, solo mentre ce n'e' una
    if (!attive_.empty()) {
        const std::size_t k = quanti(n, EFFETTI_HZ, fs, resto_eff_);
        std::vector<float> somma(k, 0.0f);
        for (Voce& v : attive_)
            for (std::size_t i = 0; i < k && v.letti < v.campioni.size(); ++i, ++v.letti) somma[i] += v.campioni[v.letti] * v.volume;
        for (float& s : somma) s = std::clamp(s, -1.0f, 1.0f);
        attive_.erase(std::remove_if(attive_.begin(), attive_.end(), [](const Voce& v) { return v.letti >= v.campioni.size(); }),
                      attive_.end());
        if (k) mixer_.aggiungi(CANALE_EFFETTI, somma, EFFETTI_HZ);
    } else {
        resto_eff_ = 0.0;
    }
    return mixer_.produci(n);
}

}  // namespace m688
