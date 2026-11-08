#include "sonar_enhanced.hpp"
#include "registro_tempi.hpp"
#include "spettri_fasci.hpp"

#include <algorithm>
#include <array>
#include <cctype>
#include <cmath>
#include <cstdio>

#include <filesystem>
#include <sstream>
#include <fstream>

#include "grafica/etichetta_tasti.hpp"
#include "grafica/consolle.hpp"
#include "grafica/lamiera.hpp"
#include "grafica/periscopio.hpp"
#include "grafica/scala_cascata.hpp"
#include "grafica/tema_stazioni.hpp"
#include "nucleo/ascolto.hpp"
#include "nucleo/fascicolo.hpp"
#include "nucleo/geometria.hpp"
#include "stazione_radio.hpp"

namespace m688 {

namespace {

constexpr double GRANA_LOFAR_DB = 1.5;   // come la grana della cascata (nucleo/analisi_acustica.cpp)
constexpr double F_MIN_BANDA = 10.0, F_MIN_ALTO = 100.0, F_MAX = 410.0, BIN_HZ = 0.1;   // righe strette come nella banca dati
constexpr double SOGLIA_TRACKER_DB = 3.0, SOGLIA_AUTOMATICO_DB = 8.0;
constexpr double C_SUONO = 1500.0;   // m/s, come confronta_rapporti per la velocita' radiale
constexpr double DISTANZA_AUTOMATICI = 20.0;   // gradi: un contatto forte occupa piu' fasci, un tracker solo
constexpr double UNIONE = 3.0;                 // gradi: due tracker cosi' vicini seguono lo stesso picco
constexpr double ALFA = 0.25, BETA = 0.02;     // filtro alfa-beta del tracker, per riga (STIMA di gioco)
constexpr int PERSO_DOPO = 15;           // righe senza picco (30 s)
constexpr int VIA_DOPO = 75;             // il tracker automatico perso sparisce dopo 2 minuti e mezzo
constexpr int LW_AUTO = 316;            // AUTO legge il LOFAR a 316 pixel (la vista intera), in qualunque area


struct Rgb {
    std::uint8_t r, g, b;
};
// Scale dei colori: punti uguali da 0 a 1, interpolati (viridis, turbo e inferno dai loro autori; fosforo P1 e grigio)
constexpr Rgb FOSFORO[] = {{0, 0, 0}, {6, 40, 8}, {18, 100, 22}, {40, 170, 45}, {80, 230, 80}, {190, 255, 180}};
constexpr Rgb GRIGIO_SCALA[] = {{0, 0, 0}, {255, 255, 255}};
constexpr Rgb VIRIDIS[] = {{0x44, 0x01, 0x54}, {0x48, 0x28, 0x78}, {0x3e, 0x49, 0x89}, {0x31, 0x68, 0x8e}, {0x26, 0x82, 0x8e},
                           {0x1f, 0x9e, 0x89}, {0x35, 0xb7, 0x79}, {0x6e, 0xce, 0x58}, {0xb5, 0xde, 0x2b}, {0xfd, 0xe7, 0x25}};
constexpr Rgb TURBO[] = {{0x30, 0x12, 0x3b}, {0x41, 0x45, 0xab}, {0x46, 0x75, 0xed}, {0x39, 0xa2, 0xfc}, {0x1b, 0xcf, 0xd4},
                         {0x24, 0xec, 0xa6}, {0x61, 0xfc, 0x6c}, {0xa4, 0xfc, 0x3b}, {0xd1, 0xe8, 0x34}, {0xf3, 0xc6, 0x3a},
                         {0xfe, 0x9b, 0x2d}, {0xf3, 0x63, 0x15}, {0xd9, 0x38, 0x06}, {0xb1, 0x19, 0x01}, {0x7a, 0x04, 0x03}};
constexpr Rgb CALDO[] = {{0x00, 0x00, 0x04}, {0x1b, 0x0c, 0x41}, {0x4a, 0x0c, 0x6b}, {0x78, 0x1c, 0x6d}, {0xa5, 0x2c, 0x60},
                         {0xcf, 0x44, 0x46}, {0xed, 0x69, 0x25}, {0xfb, 0x9b, 0x06}, {0xf7, 0xd1, 0x3d}, {0xfc, 0xff, 0xa4}};

template <std::size_t N>
Rgb interpola(const Rgb (&a)[N], double t) {
    const double x = std::clamp(t, 0.0, 1.0) * static_cast<double>(N - 1);
    const auto i = std::min(static_cast<std::size_t>(x), N - 2);
    const double u = x - static_cast<double>(i);
    const auto mix = [u](std::uint8_t p, std::uint8_t q) { return static_cast<std::uint8_t>(std::lround(p + (q - p) * u)); };
    return {mix(a[i].r, a[i + 1].r), mix(a[i].g, a[i + 1].g), mix(a[i].b, a[i + 1].b)};
}

// Media delle prime n misure (la piu' recente per prima), bin per bin
template <typename Riga>
Riga media(const std::deque<Riga>& misure, int n, std::vector<float> Riga::*campo) {
    Riga r = misure.front();
    const std::size_t k = std::min<std::size_t>(misure.size(), static_cast<std::size_t>(n));
    auto& v = r.*campo;
    for (std::size_t j = 1; j < k; ++j) {
        const auto& w = misure[j].*campo;
        for (std::size_t i = 0; i < v.size() && i < w.size(); ++i) v[i] += w[i];
    }
    for (auto& x : v) x /= static_cast<float>(k);
    return r;
}

// Massimo bin per bin (decimazione delle scale dei tempi lente: i transitori brevi restano)
template <typename Riga>
void massimo(Riga& acc, const Riga& r, std::vector<float> Riga::*campo) {
    auto& v = acc.*campo;
    const auto& w = r.*campo;
    if (v.size() != w.size()) {
        acc = r;
        return;
    }
    for (std::size_t i = 0; i < v.size(); ++i) v[i] = std::max(v[i], w[i]);
}

// Normalizzatore a finestra divisa (split window): a ogni pixel si toglie la media dei pixel da 4 a 24 di distanza
// da una parte e dall'altra, senza il pixel e i vicini; via il continuo a banda larga, restano le righe strette
void finestra_divisa(std::vector<float>& px) {
    const int n = static_cast<int>(px.size());
    std::vector<float> somma(px.size());
    float acc = 0.0f;
    for (int x = 0; x < n; ++x) somma[static_cast<std::size_t>(x)] = acc += px[static_cast<std::size_t>(x)];
    const auto tra = [&](int a, int b) {   // somma dei pixel in [a, b], ritagliata alla riga
        a = std::max(a, 0);
        b = std::min(b, n - 1);
        if (a > b) return std::pair<float, int>{0.0f, 0};
        return std::pair<float, int>{somma[static_cast<std::size_t>(b)] - (a ? somma[static_cast<std::size_t>(a - 1)] : 0.0f), b - a + 1};
    };
    std::vector<float> out(px.size());
    for (int x = 0; x < n; ++x) {
        const auto [s1, n1] = tra(x - 24, x - 4);
        const auto [s2, n2] = tra(x + 4, x + 24);
        out[static_cast<std::size_t>(x)] = px[static_cast<std::size_t>(x)] - (n1 + n2 ? (s1 + s2) / static_cast<float>(n1 + n2) : 0.0f);
    }
    px = std::move(out);
}

// Lo spettro sui `w` pixel della finestra [f0, f1]: il massimo dei bin nel pixel (o il bin del centro, se il pixel ne
// copre meno di uno), poi il normalizzatore
std::vector<float> pixel_lofar(const Spettro& s, double f0, double f1, int w) {
    std::vector<float> px(static_cast<std::size_t>(w), 0.0f);
    if (s.snr_db.empty()) return px;
    const double n = static_cast<double>(s.snr_db.size());
    const auto bin = [&](double f) { return (f - s.f0_hz) / s.df_hz; };
    for (int x = 0; x < w; ++x) {
        const double fa = f0 + (f1 - f0) * x / w, fb = f0 + (f1 - f0) * (x + 1) / w;
        const double ba = std::clamp(bin(fa), 0.0, n - 1.0), bb = std::clamp(bin(fb), 0.0, n);
        const auto i0 = static_cast<std::size_t>(ba);
        const auto i1 = std::max(i0 + 1, static_cast<std::size_t>(bb));
        float v = 0.0f;
        for (std::size_t i = i0; i < i1 && i < s.snr_db.size(); ++i) v = std::max(v, s.snr_db[i]);
        px[static_cast<std::size_t>(x)] = v;
    }
    finestra_divisa(px);
    return px;
}

std::string maiuscolo(std::string s) {
    for (auto& c : s) c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
    return s;
}

// Linee a puntini (cursori e griglia sopra le cascate)
// gli EGA della stazione che la palette colori:: non nomina: il rosso chiaro dei tracker e del fascio, il blu e il viola
// del colore per rilevamento (chiari e scuri)
constexpr std::uint8_t ROSSO_CHIARO = 12, BLU_CHIARO = 9, VIOLA = 13, VIOLA_SCURO = 5;

void puntini_v(Tela& t, int x, int y0, int y1, std::uint8_t c) {
    for (int y = y0; y <= y1; y += 3) t.pixel(x, y, c);
}
void puntini_o(Tela& t, int x0, int x1, int y, std::uint8_t c) {
    for (int x = x0; x <= x1; x += 4) t.pixel(x, y, c);
}

constexpr const char* SCHEDE[] = {"navale", "biologico", "geofisico", "meteo", "umano"};

// Colore delle righe della libreria sovrapposte, per tipo
std::uint8_t colore_riga(TipoRiga t) {
    switch (t) {
        case TipoRiga::rete: return colori::CIANO_CHIARO;
        case TipoRiga::elica: return colori::GIALLO;
        case TipoRiga::biologico: return colori::VERDE_CHIARO;
        case TipoRiga::geofisico: return colori::MARRONE;
        case TipoRiga::industriale: return colori::ROSSO;
        default: return colori::BIANCO;
    }
}

std::string tempo_indietro(double s) {
    char b[16];
    const int v = static_cast<int>(std::lround(s));
    std::snprintf(b, sizeof b, "-%02d:%02d", v / 60, v % 60);
    return b;
}

}  // namespace

const char* nome_palette(PaletteSonar p) {
    switch (p) {
        case PaletteSonar::fosforo: return "FOSFORO";
        case PaletteSonar::grigio: return "GRIGIO";
        case PaletteSonar::viridis: return "VIRIDIS";
        case PaletteSonar::turbo: return "TURBO";
        case PaletteSonar::caldo: return "CALDO";
    }
    return "";
}

const char* nome_passo(PassoClassifica p) {
    switch (p) {
        case PassoClassifica::nessuno: return "-";
        case PassoClassifica::nonsub: return "NONSUB";
        case PassoClassifica::possub: return "POSSUB";
        case PassoClassifica::probsub: return "PROBSUB";
        case PassoClassifica::certsub: return "CERTSUB";
    }
    return "";
}

double TrackerSonar::rateo() const { return storia.size() < 15 ? 0.0 : rateo_s * 60.0; }

void SonarEnhanced::imposta_modo(ModoSonar m) {
    modo_ = m;
    palette_ = m == ModoSonar::anni_89 ? PaletteSonar::fosforo : PaletteSonar::viridis;
}

std::vector<int> SonarEnhanced::messaggi() {
    std::vector<int> v;
    v.swap(messaggi_);
    return v;
}

double SonarEnhanced::f_min() const { return passa_alto_ ? F_MIN_ALTO : F_MIN_BANDA; }

std::pair<double, double> SonarEnhanced::vista_hz() const {
    const double lo = f_min(), span = (F_MAX - lo) / zoom_;
    if (zoom_ == 1) return {lo, F_MAX};
    const double f0 = std::clamp(cursore_hz_ - span / 2.0, lo, F_MAX - span);
    return {f0, f0 + span};
}

TrackerSonar* SonarEnhanced::tracker_numero(int n) {
    for (auto& t : tracker_)
        if (t.numero == n) return &t;
    return nullptr;
}

void SonarEnhanced::aggiorna(const Missione& m, int bersaglio) {
    const auto* g = m.giocatore();
    // una riga ogni PASSO_S di tempo simulato, a calendario fisso: contare dal passo che la fa scattare la ritardava di un
    // passo della missione quando la somma dei passi arriva un soffio sotto (a 1/9 s, 2,11 s invece di 2 sei righe su
    // dieci), e il tracker e la scala dei tempi contano 2 s per riga. Dopo un salto (ricostruzione) si riparte da ora
    if (!g || !m.con_acustica() || m.tempo() + 1e-6 < prossimo_) return;
    prossimo_ += PASSO_S;
    if (prossimo_ <= m.tempo()) prossimo_ = m.tempo() + PASSO_S;
    tempo_ = m.tempo();
    rotta_ = g->rotta;
    classe_ = g->classe;
    echi_.clear();   // l'elenco della missione si rifa' a ogni ping: qui si tiene finche' non svanisce
    for (const auto& e : m.echi_falsi_attivo())
        if (tempo_ - e.t < ECO_DURATA_S) echi_.push_back({e.t, e.rilevamento, e.distanza_m, e.eccesso_db});
    if (sensori_.empty())
        if (const CatalogoAcustico* c = m.catalogo_acustico())
            if (const auto it = c->suite.find(g->classe); it != c->suite.end()) {
                for (const auto& s : it->second)
                    if (s.find("attivo") == std::string::npos) sensori_.push_back(s);   // il passivo della cascata
                if (!sensori_.empty()) sensore_ = sensori_.front();
            }
    catalogo_ = m.catalogo_acustico();
    if (sensore_.empty()) return;
    Cronometro cr_scena(&RegistroTempi::Riga::sonar_scena);
    const auto scena = m.scena_acustica(g->id, sensore_);
    if (!scena.ricevitore.sensore) return;   // sensore assente o schiera non filata
    btr_misure_.push_front(cascata(scena.sorgenti, scena.ricevitore, scena.ambiente, 72, 0.0, &grana_, &scena.campi));
    cr_scena.ferma();
    while (btr_misure_.size() > 8) btr_misure_.pop_back();
    RigaCascata b = media(btr_misure_, integrazione_, &RigaCascata::db);
    aggiorna_tracker(b);
    // fascio del LOFAR: fissato a mano, un tracker, o il bersaglio (rilevamento misurato del contatto)
    if (fascio_fisso_) fascio_ = *fascio_fisso_;
    else if (const TrackerSonar* t = segue_tracker_ ? tracker_numero(segue_tracker_) : nullptr) fascio_ = t->rilevamento;
    else if (const auto c = g->contatti.find(bersaglio); c != g->contatti.end()) fascio_ = c->second.rilevamento;
    // coerente: le frange d'interferenza di Lloyd e del fondo sul LOFAR (docs/20 §4, blocco 1 del sonar)
    Cronometro cr_spettri(&RegistroTempi::Riga::sonar_spettri);
    // il colore per rilevamento (moderno): i fasci a 10 e 20 gradi dai due lati; un bin la cui riga e' piu' forte di
    // 3 dB in un fascio vicino (e sopra i 3 dB) prende quel lato. Tutti insieme, su piu' fili (app/spettri_fasci.hpp)
    static constexpr int SCARTI[] = {-20, -10, 10, 20};
    std::vector<FascioChiesto> fasci{{fascio_, true}};
    if (colore_rilevamento())
        for (const int sc : SCARTI) fasci.push_back({normalizza_360(fascio_ + sc), false});
    std::vector<Spettro> spettri = spettri_dei_fasci(scena.sorgenti, scena.ricevitore, scena.ambiente, fasci, f_min(), F_MAX, BIN_HZ, fili_per_gli_spettri());
    Spettro sp = std::move(spettri.front());
    std::vector<std::int8_t> lati;
    if (spettri.size() > 1) {
        lati.assign(sp.snr_db.size(), 0);
        for (std::size_t i = 0; i < sp.snr_db.size(); ++i) {
            double meglio = sp.snr_db[i] + 3.0;
            for (std::size_t k = 0; k < std::size(SCARTI); ++k) {
                const Spettro& vicino = spettri[k + 1];
                if (i < vicino.snr_db.size() && vicino.snr_db[i] > meglio && vicino.snr_db[i] > 3.0) {
                    meglio = vicino.snr_db[i];
                    lati[i] = static_cast<std::int8_t>(SCARTI[k] / 10);
                }
            }
        }
        colore_ril_usato_ = true;
    }
    cr_spettri.ferma();
    for (auto& v : sp.snr_db) {   // la grana del rumore in ogni bin, come nella cascata
        const double u1 = std::max(grana_lofar_.uniforme(), 1e-12), u2 = grana_lofar_.uniforme();
        v = static_cast<float>(std::max(0.0, v + GRANA_LOFAR_DB * std::sqrt(-2.0 * std::log(u1)) * std::cos(2.0 * PI * u2)));
    }
    // un altro filtro o un salto del fascio oltre la sua larghezza: le misure vecchie non si sommano alle nuove (il
    // fascio che segue il bersaglio si sposta di poco e continua a integrare)
    if (!lofar_misure_.empty() && (lofar_misure_.front().f0_hz != sp.f0_hz || std::fabs(normalizza_180(fascio_ - fascio_misure_)) > 10.0)) {
        lofar_misure_.clear();
        rilevatore_.azzera();
        storia_lofar_.reset();   // un altro fascio: un'altra storia
    }
    fascio_misure_ = fascio_;
    lofar_misure_.push_front(std::move(sp));
    while (lofar_misure_.size() > 8) lofar_misure_.pop_back();
    Spettro integrato = media(lofar_misure_, integrazione_, &Spettro::snr_db);
    if (modo_ == ModoSonar::moderno) rilevatore_.aggiungi(integrato, tempo_);   // le righe automatiche (ALI/ALT)
    // la storia lunga: il LOFAR del fascio com'e', il BTR riportato su 72 fasci veri fissi (la prua cambia)
    if (!storia_lofar_ || storia_lofar_->bin() != integrato.snr_db.size() || storia_lofar_->f0() != integrato.f0_hz)
        storia_lofar_.emplace(integrato.f0_hz, integrato.df_hz, integrato.snr_db.size());
    storia_lofar_->aggiungi(integrato, tempo_);
    if (!b.db.empty() && b.passo > 0.0) {
        const auto v = normalizza_btr(b);
        std::vector<float> veri(72, 0.0f);
        const int n = static_cast<int>(v.size());
        for (int fa = 0; fa < 72; ++fa) {
            int i = static_cast<int>(std::floor(normalizza_360(fa * 5.0 + 2.5 - b.rilevamento0) / b.passo));
            i = ((i % n) + n) % n;
            const auto k = static_cast<std::size_t>(i);
            if (!(k < b.cieco.size() && b.cieco[k])) veri[static_cast<std::size_t>(fa)] = v[k];
        }
        if (!storia_btr_) storia_btr_.emplace(0.0, 5.0, 72);
        storia_btr_->aggiungi(tempo_, veri);
    }
    nuova_riga(std::move(b), std::move(integrato));
    lati_.push_front(std::move(lati));   // stesso ordine di lofar_[0]
    while (lati_.size() > RIGHE) lati_.pop_back();
    // le righe del proprio battello nel fascio: una che prima non c'era e' un avviso per il sonarista
    std::vector<RigaPropria> proprie = righe_proprie(scena.ricevitore, scena.ambiente, fascio_, BIN_HZ);
    if (!righe_proprie_.empty())
        for (const auto& r : proprie)
            if (std::none_of(righe_proprie_.begin(), righe_proprie_.end(), [&](const RigaPropria& x) { return std::fabs(x.hz - r.hz) < 0.5; })) {
                riga_nuova_hz_ = r.hz;
                riga_nuova_fino_ = tempo_ + 30.0;
            }
    righe_proprie_ = std::move(proprie);
    for (const auto& s : scena.sorgenti)
        if (s.id == bersaglio) {
            demon_ = demon(s, scena.ricevitore, scena.ambiente);
            lettura_ = leggi_demon(demon_);
        }
    // il DEMON a cascata del fascio (tutto quello che cavita nel fascio): si ricomincia a un altro fascio o bersaglio
    if (std::fabs(normalizza_180(fascio_ - fascio_demon_)) > 10.0 || bersaglio != bersaglio_demon_) {
        demon_casc_.svuota();
        fascio_demon_ = fascio_;
        bersaglio_demon_ = bersaglio;
    }
    demon_casc_.aggiungi(demon_fascio(scena.sorgenti, scena.ricevitore, scena.ambiente, fascio_));
    misura(m, scena, bersaglio);
}

// Il contatto del giocatore sul fascio: il bersaglio se il LOFAR lo segue, altrimenti il contatto piu' vicino al
// rilevamento del fascio entro 5 gradi (-1 nessuno)
int SonarEnhanced::contatto_sul_fascio(const Missione& m, int bersaglio) const {
    const auto* g = m.giocatore();
    if (!g) return -1;
    if (!fascio_fisso_ && !segue_tracker_ && g->contatti.count(bersaglio)) return bersaglio;
    int scelto = -1;
    double scarto = 5.0;
    for (const auto& [id, c] : g->contatti) {
        const auto it = m.unita().find(id);
        if (it == m.unita().end() || it->second.distrutta || it->second.persa) continue;
        const double d = std::fabs(normalizza_180(rilevamento(g->pos, it->second.pos) - fascio_));
        if (d <= scarto) scarto = d, scelto = id;
    }
    return scelto;
}

// S5d: il registratore (un campione ogni 5 s del contatto sul fascio) e la riga agganciata (una misura ogni 10 s)
void SonarEnhanced::misura(const Missione& m, const Missione::SceneAcustica& scena, int bersaglio) {
    const auto* g = m.giocatore();
    const double t = m.tempo();
    if (t >= prossima_registrazione_ && !lofar_[0].empty()) {
        prossima_registrazione_ = t + 5.0;
        if (const int id = contatto_sul_fascio(m, bersaglio); id >= 0) {
            CampioneContatto c;
            c.t = t;
            c.rilevamento = fascio_;
            c.righe = righe_per_analisi();
            for (const auto& r : c.righe) c.livello_db = std::max(c.livello_db, r.snr_db);
            if (id == bersaglio) c.demon = lettura_;
            c.sensore = sensore_;
            if (!c.righe.empty()) registratore_.registra(id, c);
        }
    }
    if (!riga_ || t < riga_->prossima) return;
    riga_->prossima = t + 10.0;
    if (riga_->contatto < 0) riga_->contatto = contatto_sul_fascio(m, bersaglio);
    const Spettro sp = spettro_verniero(scena.sorgenti, scena.ricevitore, scena.ambiente, fascio_, riga_->hz, 4.0, 0.01, true);
    const MisuraRiga mr = misura_riga(sp, riga_->hz, 0.5, integrazione_ * PASSO_S);
    riga_->ultima = mr;
    if (!mr.valida) return;
    riga_->hz = mr.hz;
    riga_->serie.push_back({t, mr.hz, mr.sigma_hz});
    riga_->rilevamenti.push_back({t, g->pos, fascio_});
    riga_->rotte.push_back(g->rotta);
    constexpr std::size_t MASSIMO = 360;   // un'ora di misure
    if (riga_->serie.size() > MASSIMO) {
        riga_->serie.erase(riga_->serie.begin());
        riga_->rilevamenti.erase(riga_->rilevamenti.begin());
        riga_->rotte.erase(riga_->rotte.begin());
    }
    if (riga_->serie.size() >= 8) riga_->cpa = misura_cpa(riga_->serie);
    // FRAZ sugli ultimi 5 minuti (30 misure), con la frequenza a riposo dalla curva a S se c'e', o la media
    const std::size_t n = std::min<std::size_t>(riga_->serie.size(), 30), da = riga_->serie.size() - n;
    if (n >= 6) {
        std::vector<PuntoFrequenza> f(riga_->serie.begin() + static_cast<std::ptrdiff_t>(da), riga_->serie.end());
        std::vector<Rilevamento> b(riga_->rilevamenti.begin() + static_cast<std::ptrdiff_t>(da), riga_->rilevamenti.end());
        double f0 = 0.0;
        for (const auto& p : f) f0 += p.hz / static_cast<double>(n);
        if (riga_->cpa.valida) f0 = riga_->cpa.f0_hz;
        bool manovra = false;   // la propria rotta cambiata di oltre 5 gradi nella finestra
        for (std::size_t i = da; i < riga_->rotte.size(); ++i)
            manovra |= std::fabs(normalizza_180(riga_->rotte[i] - riga_->rotte[da])) > 5.0;
        riga_->fraz = distanza_fraz(f0, f, b, manovra);
    }
}

bool SonarEnhanced::storia_cerca() {
    if (!storia_lofar_) return false;
    double hz = cursore_hz_;
    if (const auto r = righe_auto_.riga_scelta(rilevatore_.confermate())) hz = r->hz;   // la riga automatica scelta
    return vista_storia_.cerca_comparsa(*storia_lofar_, hz, tempo_).has_value();
}

bool SonarEnhanced::righe_aggancia() {
    const auto r = righe_auto_.riga_scelta(rilevatore_.confermate());
    if (!r) return false;
    riga_.reset();   // dalla riga che c'era alla scelta
    cursore_hz_ = r->hz;
    aggancia_riga();
    return riga_.has_value();
}

void SonarEnhanced::aggancia_riga() {
    if (riga_) {
        riga_.reset();
        return;
    }
    if (cursore_ == Cursore::spento) return;
    riga_.emplace();
    riga_->hz = cursore_hz_;
}

std::vector<float> SonarEnhanced::ascolta(const Missione& m, double dt_reale) {
    const auto* g = m.giocatore();
    if (!g || !m.con_acustica() || sensore_.empty() || dt_reale <= 0.0) return {};
    // un altro sensore: scena e suono ripartono; il filtro cambia la banda dal passo dopo
    if (sensore_cuffia_ != sensore_) {
        tace();
        sensore_cuffia_ = sensore_;
    }
    scena_cuffia_s_ += dt_reale;
    if (!scena_cuffia_ || scena_cuffia_s_ >= 0.25) {
        scena_cuffia_ = m.scena_acustica(g->id, sensore_);
        scena_cuffia_s_ = 0.0;
    }
    if (!scena_cuffia_->ricevitore.sensore) return {};   // schiera non filata
    ParametriAscolto p;
    p.campionamento_hz = CUFFIA_HZ;
    p.f_min_hz = passa_alto_ ? F_MIN_ALTO : 20.0;
    p.seme = 0xcf1au;
    p.ottava_su = modo_cuffia_ == ModoCuffia::ottava;
    p.accelerazione = modo_cuffia_ == ModoCuffia::x4 ? 4.0 : modo_cuffia_ == ModoCuffia::x16 ? 16.0 : 1.0;
    if (!cuffia_) {
        cuffia_.emplace(p);
        cuffia_->imposta_campioni(campioni_);
    } else if (alto_cuffia_ != passa_alto_ || modo_applicato_ != modo_cuffia_) {
        cuffia_->imposta(p);
    }
    alto_cuffia_ = passa_alto_;
    modo_applicato_ = modo_cuffia_;
    return cuffia_->passo(scena_cuffia_->sorgenti, scena_cuffia_->ricevitore, scena_cuffia_->ambiente, fascio_, dt_reale);
}

void SonarEnhanced::tace() {
    cuffia_.reset();
    scena_cuffia_.reset();
}

std::optional<RapportoAcint> SonarEnhanced::rapporto_acint() const {
    if (!catalogo_ || registratore_.vuoto()) return std::nullopt;
    return ::m688::rapporto_acint(registratore_, *catalogo_);
}

std::string SonarEnhanced::registra_ascolto(const Missione& m) const {
    const auto* g = m.giocatore();
    if (!g || sensore_.empty()) return "";
    const auto scena = m.scena_acustica(g->id, sensore_);
    if (!scena.ricevitore.sensore) return "";
    ParametriAscolto p;
    p.durata_s = 8.0;
    p.seme = static_cast<std::uint64_t>(m.tempo() * 10.0) + 1;
    const Suono s = sintetizza(scena.sorgenti, scena.ricevitore, scena.ambiente, fascio_, p);
    std::error_code e;
    const auto cartella = cartella_fascicoli().parent_path() / "ascolti";
    std::filesystem::create_directories(cartella, e);
    char nome[96];
    std::snprintf(nome, sizeof nome, "ascolto_%06d_%s_%03d.wav", static_cast<int>(m.tempo()), sensore_.c_str(),
                  static_cast<int>(normalizza_360(fascio_)));
    const std::string percorso = (cartella / nome).string();
    return scrivi_wav(percorso, s) ? percorso : "";
}

void SonarEnhanced::nuova_riga(RigaCascata b, Spettro s) {
    static constexpr int PER_SCALA[3] = {1, 4, 16};
    for (std::size_t k = 1; k < 3; ++k) {   // scale lente: il massimo di 4 o 16 righe
        if (acc_n_[k] == 0) {
            acc_btr_[k] = b;
            acc_lofar_[k] = s;
        } else {
            massimo(acc_btr_[k], b, &RigaCascata::db);
            massimo(acc_lofar_[k], s, &Spettro::snr_db);
        }
        if (++acc_n_[k] == PER_SCALA[k]) {
            btr_[k].push_front(acc_btr_[k]);
            lofar_[k].push_front(acc_lofar_[k]);
            acc_n_[k] = 0;
        }
    }
    btr_[0].push_front(std::move(b));
    lofar_[0].push_front(std::move(s));
    for (std::size_t k = 0; k < 3; ++k) {
        while (btr_[k].size() > RIGHE) btr_[k].pop_back();
        while (lofar_[k].size() > RIGHE) lofar_[k].pop_back();
    }
}

std::vector<float> SonarEnhanced::normalizza_btr(const RigaCascata& r) {
    const int n = static_cast<int>(r.db.size());
    std::vector<float> v(r.db.size(), 0.0f);
    const auto cieco = [&](int i) { return static_cast<std::size_t>(i) < r.cieco.size() && r.cieco[static_cast<std::size_t>(i)]; };
    for (int i = 0; i < n; ++i) {
        if (cieco(i)) continue;
        float s = 0.0f;
        int k = 0;
        for (int d = 2; d <= 6; ++d)   // i fasci da 2 a 6 di distanza ai due lati (il giro si chiude), senza i ciechi
            for (const int j : {(i + d) % n, (i - d + n) % n})
                if (!cieco(j)) {
                    s += r.db[static_cast<std::size_t>(j)];
                    ++k;
                }
        v[static_cast<std::size_t>(i)] = r.db[static_cast<std::size_t>(i)] - (k ? s / static_cast<float>(k) : 0.0f);
    }
    return v;
}

void SonarEnhanced::aggiorna_tracker(const RigaCascata& riga) {
    if (riga.db.empty()) return;
    const std::vector<float> v = normalizza_btr(riga);
    const int n = static_cast<int>(v.size());
    const auto cieco = [&](int i) { return static_cast<std::size_t>(i) < riga.cieco.size() && riga.cieco[static_cast<std::size_t>(i)]; };
    const auto at = [&](int i) { return static_cast<double>(v[static_cast<std::size_t>(((i % n) + n) % n)]); };
    const auto fascio_di = [&](double ril) { return static_cast<int>(std::lround(normalizza_360(ril - riga.rilevamento0) / riga.passo)) % n; };
    for (auto& t : tracker_) {
        if (t.perso) {
            ++t.mancati;
            continue;
        }
        // previsione col rateo, poi il picco entro due fasci (10 gradi) e il baricentro dei fasci attorno: su un
        // contatto forte, che occupa piu' fasci, il baricentro e' piu' stabile del fascio piu' alto
        const double previsto = normalizza_360(t.rilevamento + t.rateo_s * PASSO_S);
        const int i0 = fascio_di(previsto);
        int best = -1;
        for (int d = -2; d <= 2; ++d) {
            const int j = ((i0 + d) % n + n) % n;
            if (!cieco(j) && (best < 0 || at(j) > at(best))) best = j;
        }
        if (best >= 0 && at(best) > SOGLIA_TRACKER_DB) {
            double somma = 0.0, peso = 0.0;
            for (int d = -2; d <= 2; ++d) {
                const int j = best + d;
                if (cieco(((j % n) + n) % n)) continue;
                const double w = std::max(0.0, at(j) - SOGLIA_TRACKER_DB);
                somma += w * d;
                peso += w;
            }
            const double misura = normalizza_360(riga.rilevamento0 + (best + (peso > 0.0 ? somma / peso : 0.0)) * riga.passo);
            const double scarto = normalizza_180(misura - previsto);   // filtro alfa-beta
            t.rilevamento = normalizza_360(previsto + ALFA * scarto);
            t.rateo_s += BETA * scarto / PASSO_S;
            t.livello_db = at(best);
            t.mancati = 0;
        } else {
            t.rilevamento = previsto;   // senza misura il tracker va avanti col suo rateo
            t.livello_db = at(i0);
            if (++t.mancati >= PERSO_DOPO) {
                t.perso = true;
                messaggi_.push_back(MSG_TRACKER_PERSO);
            }
        }
        t.storia.push_front(t.rilevamento);
        while (t.storia.size() > 60) t.storia.pop_back();
    }
    // due tracker sullo stesso picco: l'automatico piu' recente si toglie (quelli a mano restano, li toglie chi li ha messi)
    for (std::size_t i = 0; i < tracker_.size(); ++i)
        for (std::size_t j = 0; j < tracker_.size(); ++j)
            if (i != j && !tracker_[i].perso && !tracker_[j].perso && tracker_[j].automatico && tracker_[j].numero > tracker_[i].numero &&
                std::fabs(normalizza_180(tracker_[i].rilevamento - tracker_[j].rilevamento)) < UNIONE) {
                tracker_[j].perso = true;
                tracker_[j].mancati = VIA_DOPO;
            }
    tracker_.erase(std::remove_if(tracker_.begin(), tracker_.end(),
                                  [&](const TrackerSonar& t) {
                                      const bool via = t.automatico && t.perso && t.mancati >= VIA_DOPO;
                                      if (via && t.numero == segue_tracker_) segue_tracker_ = 0;
                                      return via;
                                  }),
                   tracker_.end());
    if (modo_ != ModoSonar::moderno) return;
    // modo moderno: i tracker automatici, nuovi sui picchi forti lontani dagli altri
    for (int i = 0; i < n; ++i) {
        if (cieco(i) || at(i) < SOGLIA_AUTOMATICO_DB || at(i) < at(i - 1) || at(i) < at(i + 1)) continue;
        const double ril = normalizza_360(riga.rilevamento0 + i * riga.passo);
        const bool vicino = std::any_of(tracker_.begin(), tracker_.end(), [&](const TrackerSonar& t) {
            return !t.perso && std::fabs(normalizza_180(t.rilevamento - ril)) < DISTANZA_AUTOMATICI;
        });
        const auto attivi = std::count_if(tracker_.begin(), tracker_.end(), [](const TrackerSonar& t) { return !t.perso; });
        if (vicino || attivi >= MAX_TRACKER) continue;
        TrackerSonar t;
        t.numero = prossimo_numero_++;
        t.rilevamento = ril;
        t.livello_db = at(i);
        t.automatico = true;
        t.storia.push_front(ril);
        tracker_.push_back(std::move(t));
        messaggi_.push_back(MSG_TRACKER_NUOVO);
    }
}

void SonarEnhanced::nuovo_tracker() {
    const auto attivi = std::count_if(tracker_.begin(), tracker_.end(), [](const TrackerSonar& t) { return !t.perso; });
    if (attivi >= MAX_TRACKER) {
        messaggi_.push_back(MSG_TRACKER_PIENI);
        return;
    }
    TrackerSonar t;
    t.numero = prossimo_numero_++;
    t.rilevamento = normalizza_360(fascio_);
    t.storia.push_front(t.rilevamento);
    segue_tracker_ = t.numero;
    fascio_fisso_.reset();
    tracker_.push_back(std::move(t));
    messaggi_.push_back(MSG_TRACKER_NUOVO);
}

void SonarEnhanced::togli_tracker() {
    auto it = std::find_if(tracker_.begin(), tracker_.end(), [&](const TrackerSonar& t) { return t.numero == segue_tracker_; });
    if (it == tracker_.end()) it = std::find_if(tracker_.begin(), tracker_.end(), [](const TrackerSonar& t) { return t.perso; });
    if (it == tracker_.end()) return;
    if (it->numero == segue_tracker_) segue_tracker_ = 0;
    tracker_.erase(it);
}

void SonarEnhanced::fascio_successivo() {
    fascio_fisso_.reset();
    std::vector<int> numeri;
    for (const auto& t : tracker_)
        if (!t.perso) numeri.push_back(t.numero);
    std::sort(numeri.begin(), numeri.end());
    const auto it = std::upper_bound(numeri.begin(), numeri.end(), segue_tracker_);
    segue_tracker_ = it == numeri.end() ? 0 : *it;   // dopo l'ultimo tracker si torna al bersaglio
}

void SonarEnhanced::sensore_successivo() {
    if (sensori_.empty()) return;
    const auto it = std::find(sensori_.begin(), sensori_.end(), sensore_);
    sensore_ = it == sensori_.end() || it + 1 == sensori_.end() ? sensori_.front() : *(it + 1);
    for (std::size_t k = 0; k < 3; ++k) {   // un altro sensore, un'altra cascata
        btr_[k].clear();
        lofar_[k].clear();
        acc_n_[k] = 0;
    }
    btr_misure_.clear();
    lofar_misure_.clear();
    lati_.clear();
}

void SonarEnhanced::sposta_fascio(int verso) {
    fascio_fisso_ = normalizza_360((fascio_fisso_ ? *fascio_fisso_ : fascio_) + 5.0 * verso);
    segue_tracker_ = 0;
}

void SonarEnhanced::campo_successivo(int verso) {
    constexpr int N = static_cast<int>(Campo::maschera) + 1;
    campo_ = static_cast<Campo>((static_cast<int>(campo_) + verso + N) % N);
    // la persistenza c'e' solo sul fosforo del 1989, il colore per rilevamento solo nel moderno: saltati negli altri
    for (int k = 0; k < 2; ++k)
        if ((campo_ == Campo::persistenza && modo_ != ModoSonar::anni_89) || (campo_ == Campo::colore_rilevamento && modo_ != ModoSonar::moderno))
            campo_ = static_cast<Campo>((static_cast<int>(campo_) + verso + N) % N);
}

void SonarEnhanced::cambia_valore(int verso) {
    switch (campo_) {
        case Campo::pavimento: pavimento_ = std::clamp(pavimento_ + verso, -10.0, tetto_ - 3.0); break;
        case Campo::tetto: tetto_ = std::clamp(tetto_ + verso, pavimento_ + 3.0, 40.0); break;
        case Campo::palette: {   // il modo 1989 ha solo il fosforo e il grigio
            const std::vector<PaletteSonar> scelte =
                modo_ == ModoSonar::anni_89 ? std::vector<PaletteSonar>{PaletteSonar::fosforo, PaletteSonar::grigio}
                                            : std::vector<PaletteSonar>{PaletteSonar::viridis, PaletteSonar::turbo, PaletteSonar::caldo,
                                                                        PaletteSonar::grigio, PaletteSonar::fosforo};
            const auto it = std::find(scelte.begin(), scelte.end(), palette_);
            const int i = it == scelte.end() ? 0 : static_cast<int>(it - scelte.begin());
            const int n = static_cast<int>(scelte.size());
            palette_ = scelte[static_cast<std::size_t>(((i + verso) % n + n) % n)];
            break;
        }
        case Campo::scala_tempi: scala_tempi_ = verso > 0 ? std::min(scala_tempi_ * 4, 16) : std::max(scala_tempi_ / 4, 1); break;
        case Campo::doppler: k_ = std::clamp(k_ + 0.0005 * verso, 0.988, 1.012); break;   // circa 1,5 nodi a passo
        case Campo::maschera: maschera_ = !maschera_; break;
        case Campo::colore_rilevamento: colore_ril_ = !colore_ril_; break;
        case Campo::persistenza:   // tre gradi: breve, normale, lunga
            persistenza_ = static_cast<cascate::Persistenza>(std::clamp(static_cast<int>(persistenza_) + verso, 0, 2));
            break;
    }
}

void SonarEnhanced::automatico() {
    if (modo_ != ModoSonar::moderno || btr_[0].empty() || lofar_[0].empty()) return;
    std::vector<float> v = normalizza_btr(btr_[0].front());
    const auto [f0, f1] = vista_hz();
    const auto px = pixel_lofar(lofar_[0].front(), f0, f1, LW_AUTO);
    v.insert(v.end(), px.begin(), px.end());
    std::sort(v.begin(), v.end());
    const auto quantile = [&](double q) { return static_cast<double>(v[static_cast<std::size_t>(q * static_cast<double>(v.size() - 1))]); };
    pavimento_ = std::clamp(std::round(quantile(0.2)), -10.0, 30.0);
    tetto_ = std::clamp(std::round(quantile(0.995) + 2.0), pavimento_ + 8.0, 40.0);
}

void SonarEnhanced::cursore_successivo() {
    constexpr int N = static_cast<int>(Cursore::storia) + 1;   // spento ... righe, storia, e di nuovo spento
    cursore_ = static_cast<Cursore>((static_cast<int>(cursore_) + 1) % N);
    if (cursore_ == Cursore::righe && modo_ != ModoSonar::moderno) cursore_ = static_cast<Cursore>((static_cast<int>(cursore_) + 1) % N);
    if (cursore_ != Cursore::storia) vista_storia_.adesso();   // fuori dal modo storia: di nuovo in diretta
    if (cursore_ == Cursore::delta) {   // il riferimento e' dove sta il cursore
        riferimento_hz_ = cursore_hz_;
        riferimento_riga_ = cursore_riga_;
    }
}

void SonarEnhanced::muovi_cursore(double d_hz, int d_righe) {
    cursore_hz_ = std::clamp(cursore_hz_ + d_hz, f_min(), F_MAX);
    cursore_riga_ = std::clamp(cursore_riga_ + d_righe, 0, RIGHE - 1);
}

void SonarEnhanced::zoom(int verso) { zoom_ = std::clamp(verso > 0 ? zoom_ * 2 : zoom_ / 2, 1, 16); }

namespace {
// Le voci che le righe lette reggono: almeno 3 righe osservate spiegate, almeno 2 righe della voce e il 60% di quelle
// lette; prima chi ne spiega di piu'. La confidenza del confronto e' relativa alle voci: dice un nome anche con una riga
// sola o con nessuna concorde, e piu' righe a caso la alzano (07/11/2026: 6% di classi giuste sui contatti veri, nota
// analizzatore-07-11). Il confronto resta com'e': lo usano il sonarista e l'IA
struct VoceRetta {
    const ConfrontoRapporti* c = nullptr;
    int spiegate = 0;
};
constexpr int RETTA_SPIEGATE = 3, RETTA_DELLA_VOCE = 2;
constexpr double RETTA_QUOTA = 0.6;
template <typename T>
int distinte(const std::vector<std::pair<RigaOsservata, RigaLibreria>>& concordi, T campo) {
    std::vector<double> visti;
    for (const auto& p : concordi)
        if (const double x = campo(p); std::find(visti.begin(), visti.end(), x) == visti.end()) visti.push_back(x);
    return static_cast<int>(visti.size());
}
int righe_spiegate(const ConfrontoRapporti& c) { return distinte(c.concordi, [](const auto& p) { return p.first.hz; }); }
std::vector<VoceRetta> voci_rette(const std::vector<ConfrontoRapporti>& v) {
    std::vector<VoceRetta> out;
    for (const auto& c : v) {
        const int o = righe_spiegate(c), della_voce = distinte(c.concordi, [](const auto& p) { return p.second.hz; });
        const double lette = static_cast<double>(o) + static_cast<double>(c.in_piu.size());
        if (o >= RETTA_SPIEGATE && della_voce >= RETTA_DELLA_VOCE && o >= RETTA_QUOTA * lette) out.push_back({&c, o});
    }
    std::stable_sort(out.begin(), out.end(), [](const VoceRetta& a, const VoceRetta& b) { return a.spiegate > b.spiegate; });
    return out;
}
bool voce_sottomarino(const CatalogoAcustico& cat, const std::string& id) {
    const auto* f = cat.firma(id);
    return f && (f->tipo_unita == "ssn" || f->tipo_unita == "ssgn" || f->tipo_unita == "ssbn" || f->tipo_unita == "ssk");
}
}  // namespace

// Mai NONSUB: nel fascio largo le righe di una nave non dicono che il contatto non sia un sottomarino (a Foxtrot e
// Victor III il fascio prestava le righe di petroliere e pescherecci, 2-4 letture su 100 anche con le soglie). Solo le
// due voci mostrate: un sottomarino fuori di li' era quasi sempre una coincidenza (sfera in moto: allarme 3% sui
// sottomarini, 6% sui civili). PROBSUB e' il massimo che le righe sostengono da sole
PassoClassifica passo_da_voci(const std::vector<ConfrontoRapporti>& v, const CatalogoAcustico& cat) {
    const auto rette = voci_rette(v);
    const std::size_t mostrate = std::min<std::size_t>(rette.size(), 2);
    std::size_t sub = 0;
    for (std::size_t i = 0; i < mostrate; ++i)
        if (voce_sottomarino(cat, rette[i].c->id)) ++sub;
    return sub == 0 ? PassoClassifica::nessuno : sub == mostrate ? PassoClassifica::probsub : PassoClassifica::possub;
}

SonarEnhanced::LetturaAnalisi SonarEnhanced::lettura_analisi(std::size_t lungo, bool una_voce) const {
    if (confronti_.empty()) return {};
    const std::size_t lette = static_cast<std::size_t>(righe_spiegate(confronti_.front())) + confronti_.front().in_piu.size();
    const auto rette = voci_rette(confronti_);
    char b[96];
    if (rette.empty()) {
        std::snprintf(b, sizeof b, lette == 1 ? "NESSUN NOME: 1 RIGA NON BASTA" : "NESSUN NOME: %zu RIGHE NON BASTANO", lette);
        return {b, false};
    }
    const auto nome = [&](const VoceRetta& x) {   // «MERCANTILE (RAVE» no: il nome tagliato finisce prima della parentesi
        std::string n = nome_voce(x.c->id).substr(0, lungo);
        if (const auto k = n.find(" ("); k != std::string::npos) n.resize(k);
        while (!n.empty() && n.back() == ' ') n.pop_back();
        return n;
    };
    std::snprintf(b, sizeof b, "%s %d/%zu RIGHE", nome(rette[0]).c_str(), rette[0].spiegate, lette);
    std::string s = b;
    if (!una_voce && rette.size() > 1) {
        std::snprintf(b, sizeof b, " O %s %d/%zu", nome(rette[1]).c_str(), rette[1].spiegate, lette);
        s += b;
    }
    return {s, true};
}

int SonarEnhanced::analizza(const Missione& m) {
    pagina_analisi_ = !pagina_analisi_;   // la pagina dell'analizzatore: il 4 la apre, di nuovo la chiude; analizza sempre
    ipotesi_.clear();
    confronti_.clear();   // senza righe si torna prima: la pagina non deve restare sulla lettura del fascio di prima
    famiglie_.clear();
    const CatalogoAcustico* c = m.catalogo_acustico();
    if (!c || lofar_[0].empty()) return MSG_ANALISI_INCERTA;
    const auto righe = righe_per_analisi();
    if (righe.empty()) return MSG_ANALISI_INCERTA;
    // le pale dai cursori del DEMON a cascata se ci sono, altrimenti dalla lettura del bersaglio
    const std::optional<LetturaDemon> ld = demon_casc_.lettura() ? demon_casc_.lettura() : lettura_;
    const std::optional<int> pale = ld ? std::optional<int>(ld->pale) : std::nullopt;
    ipotesi_ = confronta_firma(righe, *c, pale, BIN_HZ);
    // S5b: confronto per rapporti (anche col bersaglio in moto) e famiglie armoniche; la voce sovrapposta prende il k
    confronti_ = confronta_rapporti(righe, *c, pale);
    famiglie_ = famiglie_armoniche(righe, F_MAX, c);
    if (sovrapposta_)
        for (const auto& x : confronti_)
            if (x.id == sovrapposta_->id && !x.concordi.empty()) k_ = x.fattore_doppler;
    // il tracker sul fascio: quello seguito, o il piu' vicino entro mezzo fascio
    TrackerSonar* t = segue_tracker_ ? tracker_numero(segue_tracker_) : nullptr;
    if (!t)
        for (auto& x : tracker_)
            if (!x.perso && std::fabs(normalizza_180(x.rilevamento - fascio_)) < 5.0 &&
                (!t || std::fabs(normalizza_180(x.rilevamento - fascio_)) < std::fabs(normalizza_180(t->rilevamento - fascio_))))
                t = &x;
    const auto rette = voci_rette(confronti_);
    if (t && !rette.empty()) {   // sotto le soglie il tracker resta com'era: nessuna classe, nessun passo, nessun messaggio
        const PassoClassifica p = passo_da_voci(confronti_, *c);
        t->classe = rette.front().c->id;
        t->confidenza = static_cast<double>(rette.front().spiegate) / static_cast<double>(righe.size());
        if (p != t->passo) {
            t->passo = p;
            static constexpr int CODICI[] = {0, MSG_NONSUB, MSG_POSSUB, MSG_PROBSUB, MSG_CERTSUB};
            if (const int k = CODICI[static_cast<int>(p)]) messaggi_.push_back(k);
        }
    }
    return rette.empty() ? MSG_ANALISI_INCERTA : MSG_ANALISI_PRONTA;
}

void SonarEnhanced::applica_palette(Palette& p) const {
    if (scala_lunga_) {
        static constexpr cascate::Tinta TINTE[] = {cascate::Tinta::fosforo, cascate::Tinta::grigio, cascate::Tinta::viridis,
                                                   cascate::Tinta::turbo, cascate::Tinta::caldo};
        // l'Alfa: il fosforo verde giallino dei suoi CRT (dossier 19 §8) al posto del verde del BQQ-5
        const bool alfa = classe_ == "alfa" && palette_ == PaletteSonar::fosforo;
        cascate::applica(p, alfa ? cascate::Tinta::fosforo_alfa : TINTE[static_cast<int>(palette_)], cascate::SCALA_LUNGA);
        return;
    }
    for (int i = 0; i < LIVELLI; ++i) {
        const double t = static_cast<double>(i) / (LIVELLI - 1);
        Rgb c{};
        switch (palette_) {
            case PaletteSonar::fosforo: c = interpola(FOSFORO, t); break;
            case PaletteSonar::grigio: c = interpola(GRIGIO_SCALA, t); break;
            case PaletteSonar::viridis: c = interpola(VIRIDIS, t); break;
            case PaletteSonar::turbo: c = interpola(TURBO, t); break;
            case PaletteSonar::caldo: c = interpola(CALDO, t); break;
        }
        if (i == 0) c = {0, 0, 0};   // sotto il pavimento il fondo resta nero
        p.imposta(SCALA0 + i, c.r, c.g, c.b);
    }
}

std::uint8_t SonarEnhanced::colore(double db) const {
    const double u = (db - pavimento_) / (tetto_ - pavimento_);
    return static_cast<std::uint8_t>(SCALA0 + std::clamp(static_cast<int>(std::lround(u * (LIVELLI - 1))), 0, LIVELLI - 1));
}

// Il fosforo del 1989: gli effetti di grafica/scala_cascata, piu' un velo di grana ferma al 7% sotto il pavimento,
// perche' lo schermo acceso non sembri spento (tavola fosforo_fondo del kit, 28/10/2026)
cascate::Fosforo::Effetti SonarEnhanced::effetti_fosforo() {
    cascate::Fosforo::Effetti e;
    e.fondo = 0.07;
    return e;
}

std::vector<RigaOsservata> SonarEnhanced::righe_per_analisi() const {
    std::vector<RigaOsservata> r = righe_osservate(lofar_[0].front());
    if (maschera_) {   // senza maschera il sonarista puo' scambiare una riga propria per un contatto (dossier 09 §3.9)
        std::vector<double> hz;
        for (const auto& p : righe_proprie_) hz.push_back(p.hz);
        togli_righe(r, hz, 0.2);
    }
    return r;
}

void SonarEnhanced::tratto_cascata(Tela& t, int x, int y, int w, int h, double db) const {
    if (!scala_lunga_) {
        t.rettangolo_pieno(x, y, w, h, colore(db));
        return;
    }
    const double u = cascate::frazione(db, pavimento_, tetto_);
    for (int py = y; py < y + h; ++py)
        for (int px = x; px < x + w; ++px) t.pixel(px, py, cascate::indice(u, px, py));
}

// La disposizione nell'area: a sinistra la cascata dei rilevamenti (meta' larghezza), a destra l'A-scan sopra il LOFAR,
// sotto 72 righe di testo. Nella vista intera (640x360) sono i numeri di sempre
// La disposizione del quadro SDR (la bozza A, scelta dell'utente del 29/10/2026 per il moderno; docs/ricerca/30 §4.4):
// sopra il BTR i tracker come segnalibri e la scala a tacche; fra l'A-scan e il LOFAR la striscia del band plan; sotto
// la scala delle frequenze la barra dello zoom
SonarEnhanced::Disposizione SonarEnhanced::disposizione(Rettangolo a) const {
    Disposizione d;
    d.area = a;
    d.bw = a.w / 2;
    d.top = a.y + 40;
    d.fondo = a.y + a.h - 72;
    d.lx = a.x + d.bw + 4;
    d.lw = a.w - d.bw - 4;
    d.ay = a.y + 14;
    d.ah = 34;
    d.ly = a.y + 58;
    // il DEMON a cascata nel terzo basso della colonna del LOFAR (dossier 09 §4.3), se c'e' posto
    d.fl = d.fondo - d.ly >= 200 ? d.fondo - 94 : d.fondo;
    return d;
}

// i colori del testo: nel 1989 il verde del fosforo (sull'Alfa il verde giallino del tema, 40-47), nel moderno il quadro
std::uint8_t SonarEnhanced::c_titolo() const {
    return modo_ == ModoSonar::anni_89 ? (classe_ == "alfa" ? fosforo(7) : std::uint8_t{colori::VERDE_CHIARO}) : std::uint8_t{colori::CIANO_CHIARO};
}
std::uint8_t SonarEnhanced::c_tenue() const {
    return modo_ == ModoSonar::anni_89 ? (classe_ == "alfa" ? fosforo(4) : std::uint8_t{colori::VERDE}) : std::uint8_t{colori::GRIGIO};
}
std::uint8_t SonarEnhanced::c_testo() const {
    return modo_ == ModoSonar::anni_89 ? (classe_ == "alfa" ? fosforo(6) : std::uint8_t{colori::VERDE_CHIARO}) : std::uint8_t{colori::BIANCO};
}

void SonarEnhanced::disegna(Tela& t, const Font& f) const { disegna(t, f, {0, 0, LARGHEZZA_TELA, ALTEZZA_VISTA}); }

void SonarEnhanced::disegna(Tela& t, const Font& f, Rettangolo area) const {
    const Rettangolo prima = t.ritaglio();
    t.imposta_ritaglio(area);
    t.rettangolo_pieno(area.x, area.y, area.w, area.h, colori::NERO);
    if (pagina_analisi_ && !libreria_aperta_) {   // la pagina dell'analizzatore (la bozza C)
        disegna_analisi(t, f, area);
        t.imposta_ritaglio(prima);
        return;
    }
    // il 1989 sulla consolle BQQ-5 (la bozza B): lamiera, due CRT, i tasti; il moderno nel quadro SDR (la bozza A)
    const Disposizione d = modo_ == ModoSonar::anni_89 && !libreria_aperta_ ? disegna_consolle(t, f, area) : disposizione(area);
    if (libreria_aperta_) {
        disegna_libreria(t, f, d);
    } else {
        disegna_btr(t, f, d);
        disegna_lofar(t, f, d);
        disegna_demon(t, f, d);
        disegna_storia(t, f, d, true);
        disegna_fondo(t, f, d);
        if (riga_) disegna_riga_misurata(t, f, d);
    }
    t.imposta_ritaglio(prima);
}

// La consolle BQQ-5 del 1989 (la bozza B, scelta dell'utente del 29/10/2026; docs/ricerca/30 §4.2): la lamiera, due CRT
// incassati (il BTR; il LOFAR sopra il DEMON), una colonna di tasti quadrati a luce che dicono lo stato (non si
// cliccano: i comandi restano quelli della tastiera), il vetro dei contatti e dello stato sotto
SonarEnhanced::Disposizione SonarEnhanced::disegna_consolle(Tela& t, const Font& f, Rettangolo a) const {
    constexpr int TASTI = 40, BASSO = 60;
    lamiera::lamiera(t, a, 7, 31);
    const int meta = (a.w - TASTI) / 2;
    const Rettangolo c1 = crt_rettangolare(t, {a.x + 2, a.y + 2, meta - 4, a.h - BASSO - 4});
    const Rettangolo c2 = crt_rettangolare(t, {a.x + meta + 2, a.y + 2, a.w - TASTI - meta - 4, a.h - BASSO - 4});
    t.rettangolo_pieno(c1.x, c1.y, c1.w, c1.h, colori::NERO);
    t.rettangolo_pieno(c2.x, c2.y, c2.w, c2.h, colori::NERO);
    const struct { const char* nome; bool acceso; } tasti[] = {
        {"BTR", true}, {"LOF", true}, {"DEM", lettura_.has_value()}, {"INT", integrazione_ > 1}, {"CURS", cursore_ != Cursore::spento},
        {"TRK", !tracker_.empty()}, {"ANAL", !ipotesi_.empty() || !confronti_.empty()}, {"MASC", maschera_}};
    const int n = static_cast<int>(std::size(tasti)), th = std::min(28, (a.h - BASSO - 8) / n - 4);
    for (int k = 0; k < n; ++k)
        lamiera::tasto(t, {a.x + a.w - TASTI + 2, a.y + 4 + k * (th + 4), TASTI - 4, th}, tasti[k].nome, ColoreLampada::verde,
                       tasti[k].acceso ? lamiera::Stato::evidenziato : lamiera::Stato::normale, false, f);
    lamiera::vetro(t, {a.x + 2, a.y + a.h - BASSO + 2, a.w - 4, BASSO - 4});
    Disposizione d;
    d.consolle = true;
    d.area = {c1.x, c1.y, c2.x + c2.w - c1.x, c1.h};   // area.x e area.y: il BTR e i titoli nei CRT
    d.bw = c1.w;
    d.top = c1.y + 40;
    d.fondo = c1.y + c1.h;
    d.lx = c2.x + 2;
    d.lw = c2.w - 4;
    d.ay = c2.y + 14;
    d.ah = 34;
    d.ly = c2.y + 58;
    d.fl = d.fondo - 80;   // il DEMON sotto il LOFAR nello stesso CRT, come sulla consolle vera
    return d;
}

// La pagina dell'analizzatore (la bozza C; il tasto 4): il LOFAR del fascio a tutta larghezza con lo zoom, l'A-scan, il
// band plan della voce sovrapposta, i cursori; sotto a sinistra il DEMON, a destra l'analisi e gli otto slot
void SonarEnhanced::disegna_analisi(Tela& t, const Font& f, Rettangolo a) const {
    Disposizione e;
    e.area = a;
    e.lx = a.x + 4;
    e.lw = a.w - 8;
    e.ay = a.y + 14;
    e.ah = 50;
    e.ly = a.y + 74;
    e.fondo = a.y + a.h - 6;
    e.fl = a.y + a.h - 122;
    e.top = e.ly;
    disegna_lofar(t, f, e);
    if (riga_) disegna_riga_misurata(t, f, e);
    Disposizione dd = e;   // il DEMON sotto a sinistra, sotto la scala e la barra dello zoom
    dd.lw = a.w / 2 - 8;
    dd.fl = e.fl + 4;
    disegna_demon(t, f, dd);
    // a destra: l'analisi e gli slot
    const int x = a.x + a.w / 2 + 4, w = a.w / 2 - 8;
    int y = e.fl + 22;
    f.scrivi(t, x, y, "ANALIZZATORE  (4 CHIUDE)", c_titolo(), 1);
    y += 11;
    char riga[160];
    if (!confronti_.empty()) {
        LetturaAnalisi l = lettura_analisi(12);
        if (f.larghezza(l.testo, 1) > w) l = lettura_analisi(12, true);
        f.scrivi(t, x, y, l.testo, l.nome ? colori::GIALLO : c_tenue(), 1);
    } else if (!ipotesi_.empty()) {
        const auto& h = ipotesi_.front();
        std::snprintf(riga, sizeof riga, "%s %s %.0f%% (%d RIGHE)", maiuscolo(h.classe).c_str(), maiuscolo(h.nazionalita).c_str(),
                      h.confidenza * 100.0, h.righe_concordi);
        f.scrivi(t, x, y, riga, colori::GIALLO, 1);
    } else {
        f.scrivi(t, x, y, "NESSUNA IPOTESI: POCHE RIGHE NEL FASCIO", c_tenue(), 1);
    }
    y += 11;
    if (!famiglie_.empty()) {
        const auto& fm = famiglie_.front();
        if (fm.pale_stimate) std::snprintf(riga, sizeof riga, "%s %.1f HZ %d PALE", maiuscolo(fm.indizio).c_str(), fm.pala_hz, fm.pale_stimate);
        else std::snprintf(riga, sizeof riga, "%s %.1f HZ X%zu", maiuscolo(fm.indizio).c_str(), fm.f1_hz, fm.armoniche.size());
        f.scrivi(t, x, y, riga, c_testo(), 1);
    }
    y += 13;
    // gli slot: quattro per riga, con lo spettro salvato in miniatura
    if (!slot_disponibili()) {
        f.scrivi(t, x, y, "NIENTE REGISTRATORE SUL 1989 DELL'ALFA", c_tenue(), 1);
        return;
    }
    const auto [f0, f1] = vista_hz();
    const int sw = w / 4 - 3, sh = std::max(12, (e.fondo - y) / 2 - 3);
    for (int k = 0; k < SLOT; ++k) {
        const Rettangolo r{x + (k % 4) * (sw + 3), y + (k / 4) * (sh + 3), sw, sh};
        const std::uint8_t cb = k == slot_ ? colori::GIALLO : c_tenue();
        t.linea_orizzontale(r.x, r.x + r.w - 1, r.y, cb), t.linea_orizzontale(r.x, r.x + r.w - 1, r.y + r.h - 1, cb);
        t.linea_verticale(r.x, r.y, r.y + r.h - 1, cb), t.linea_verticale(r.x + r.w - 1, r.y, r.y + r.h - 1, cb);
        const Slot& sl = slots_[static_cast<std::size_t>(k)];
        f.scrivi(t, r.x + 2, r.y + 2, std::to_string(k + 1), cb, 1);
        if (!sl.pieno) continue;
        const auto px = pixel_lofar(sl.spettro, f0, f1, r.w - 4);
        int prima = -1;
        for (int i = 0; i < r.w - 4; ++i) {
            const int yy = r.y + r.h - 2 - std::clamp(static_cast<int>((px[static_cast<std::size_t>(i)] - pavimento_) / (tetto_ - pavimento_) * (r.h - 4)), 0, r.h - 4);
            if (prima >= 0) t.linea_verticale(r.x + 2 + i, std::min(prima, yy), std::max(prima, yy), c_testo());
            prima = yy;
        }
    }
}

// Nel modo storia: la storia del LOFAR al posto della sua cascata e, se c'e', quella del BTR al posto della sua
void SonarEnhanced::disegna_storia(Tela& t, const Font& f, const Disposizione& d, bool con_btr) const {
    if (cursore_ != Cursore::storia) return;
    StileStoria st;
    st.modo_1989 = modo_ == ModoSonar::anni_89;
    st.fosforo = con_fosforo();
    st.scala = scala_lunga_ ? cascate::SCALA_LUNGA : cascate::SCALA_CORTA;
    st.pavimento_db = pavimento_;
    st.tetto_db = tetto_;
    const auto [f0, f1] = vista_hz();
    if (storia_lofar_) {
        StileStoria sl = st;
        sl.x0 = f0;
        sl.x1 = f1;
        vista_storia_.disegna(t, f, {d.lx, d.ly, d.lw, d.fl - d.ly}, *storia_lofar_, tempo_, sl);
    }
    if (con_btr && storia_btr_) {
        StileStoria sb = st;
        sb.gradi = true;
        vista_storia_.disegna(t, f, {d.area.x, d.top, d.bw, d.fondo - d.top}, *storia_btr_, tempo_, sb);
    }
}

void SonarEnhanced::disegna_demon(Tela& t, const Font& f, const Disposizione& d) const {
    if (d.fl >= d.fondo) return;
    StileDemon s;
    s.modo_1989 = modo_ == ModoSonar::anni_89;
    s.fosforo = con_fosforo();
    s.scala = scala_lunga_ ? cascate::SCALA_LUNGA : cascate::SCALA_CORTA;
    s.pavimento_db = pavimento_;
    s.tetto_db = tetto_;
    s.scala_tempi = scala_tempi_;
    if (classe_ == "alfa") s.nome = "DEMOD", s.unita = "GC";   // Okean: le scritte traslitterate
    demon_casc_.disegna(t, f, {d.lx, d.fl + 18, d.lw, d.fondo - d.fl - 18}, s);   // sotto la scala e la barra dello zoom
}

// I livelli 0-1 per rilevamento relativo (0 la prua, un valore per grado) dell'ultima riga della cascata alla scala
// scelta, fra pavimento e tetto; i fasci ciechi a 0
std::vector<float> SonarEnhanced::livelli_relativi() const {
    std::vector<float> out(360, 0.0f);
    const auto& storia = btr_[static_cast<std::size_t>(indice_scala(scala_tempi_))];
    if (storia.empty()) return out;
    const auto& c = storia.front();
    const auto v = normalizza_btr(c);
    if (v.empty() || c.passo <= 0.0) return out;
    const int n = static_cast<int>(v.size());
    for (int g = 0; g < 360; ++g) {
        const double vero = normalizza_360(rotta_ + g + 0.5);
        int i = static_cast<int>(std::floor(normalizza_360(vero - c.rilevamento0) / c.passo));
        i = ((i % n) + n) % n;
        const auto k = static_cast<std::size_t>(i);
        if (k < c.cieco.size() && c.cieco[k]) continue;
        out[static_cast<std::size_t>(g)] = static_cast<float>(std::clamp((v[k] - pavimento_) / (tetto_ - pavimento_), 0.0, 1.0));
    }
    return out;
}

// Okean dell'Alfa (dossier 19 §6.6): a sinistra, al posto della cascata dei rilevamenti, lo schermo tondo di Enisej col
// ventaglio dei rilevamenti relativi (l'ultima riga integrata: le «tracce senza suono»), il fascio del LOFAR e i
// tracker sul bordo; a destra il LOFAR a cascata e sotto il testo, come nel 688. La palette del tema dell'Alfa deve
// essere applicata (smalto e fosforo negli indici 16-47)
void SonarEnhanced::disegna_okean(Tela& t, const Font& f, const FontConsolle& fc, Rettangolo area) const {
    const Rettangolo prima = t.ritaglio();
    t.imposta_ritaglio(area);
    t.rettangolo_pieno(area.x, area.y, area.w, area.h, colori::NERO);
    const Disposizione d = disposizione(area);
    if (libreria_aperta_) {
        disegna_libreria(t, f, d);
        t.imposta_ritaglio(prima);
        return;
    }
    // il pulto: lo smalto attorno allo schermo, la targhetta, la lampada di Luch (antimine: nel gioco spenta)
    const Rettangolo pulto{area.x, area.y, d.bw, d.fondo - area.y};
    pannello(t, pulto, BattelloTema::alfa);
    targhetta(t, {pulto.x + 6, pulto.y + 4, 0, 0}, "OKEAN", fc.piccolo);
    lampada_tonda(t, pulto.x + pulto.w - 64, pulto.y + 9, 4, ColoreLampada::verde, false);
    targhetta(t, {pulto.x + pulto.w - 56, pulto.y + 4, 0, 0}, "LUCH", fc.piccolo);
    const int alto = 18, r = std::max(20, std::min(pulto.w, pulto.h - alto) / 2 - 11);
    const int cx = pulto.x + pulto.w / 2, cy = pulto.y + alto + (pulto.h - alto) / 2;
    crt_tondo(t, cx, cy, r);
    disegna_enisej(t, fc, cx, cy, r);
    targhetta(t, {pulto.x + 6, pulto.y + pulto.h - 14, 0, 0}, "ENISEJ  OTN. PELENG", fc.piccolo);
    disegna_griglia(t, f, d, false);
    disegna_lofar(t, f, d);
    disegna_demon(t, f, d);
    disegna_storia(t, f, d, false);
    disegna_fondo(t, f, d);
    if (riga_) disegna_riga_misurata(t, f, d);
    t.imposta_ritaglio(prima);
}

static std::uint8_t colore_tracker(const TrackerSonar& tr, ModoSonar modo);

// Enisej come cascata polare (bozza D; docs/ricerca/30 §3.3): l'angolo e' il rilevamento relativo con la prua in alto,
// il raggio il tempo, l'ultima riga sul bordo e le vecchie verso il centro (una riga per pixel, alla scala dei tempi
// scelta). Le righe vecchie si girano con la prua di adesso (STIMA: in virata le tracce piegano). Il pavimento come la
// cascata (il rumore cade sul nero); il cieco di poppa (oltre 120 gradi) e i fasci ciechi a tratteggio; i minuti a cerchi
// di puntini; sul bordo la scala (tacca ogni 5, lunga ogni 10, numero ogni 30: L a sinistra, P a dritta); il fascio con
// le parentesi sul bordo e i lati a puntini; i tracker con la tacca dentro il bordo e il nome, su due raggi se si toccano
void SonarEnhanced::disegna_enisej(Tela& t, const FontConsolle& fc, int cx, int cy, int r) const {
    constexpr double SETTORE = 120.0;
    const bool fosforo_1989 = modo_ == ModoSonar::anni_89;
    const std::uint8_t tenue = fosforo_1989 ? fosforo(3) : colori::GRIGIO, griglia = fosforo_1989 ? fosforo(2) : colori::GRIGIO_SCURO;
    const std::uint8_t testo = fosforo_1989 ? fosforo(6) : colori::BIANCO, fascio_c = fosforo_1989 ? fosforo(7) : std::uint8_t{12};
    const Font& fp = fc.piccolo;
    const int sp = fp.formato() == 2 ? fp.spazio_predefinito() : 1;
    const auto largo = [&](const std::string& s) { return fp.larghezza(s, 1, sp, false); };
    const auto scrivi = [&](int x, int y, const std::string& s, std::uint8_t c) {
        if (fosforo_1989 && c == testo) testo_fosforo(t, x, y, s, fp);
        else fp.scrivi(t, x, y, s, c, 1, sp, false);
    };
    const int R = r - 1, dati = R - 22, dentro = 10;
    const double xc = cx + 0.5, yc = cy + 0.5;
    // le righe normalizzate come la cascata, dalla piu' recente
    const auto& storia = btr_[static_cast<std::size_t>(indice_scala(scala_tempi_))];
    const int righe = std::min<int>(static_cast<int>(storia.size()), dati - dentro);
    std::vector<std::vector<float>> norm(static_cast<std::size_t>(righe));
    for (int k = 0; k < righe; ++k) norm[static_cast<std::size_t>(k)] = normalizza_btr(storia[static_cast<std::size_t>(k)]);
    for (int y = cy - R; y <= cy + R; ++y)
        for (int x = cx - R; x <= cx + R; ++x) {
            const double dx = x + 0.5 - xc, dy = y + 0.5 - yc, rho = std::hypot(dx, dy);
            if (rho > R) continue;
            t.pixel(x, y, colori::NERO);
            if (rho > dati || rho < dentro) continue;
            const double rel = std::atan2(dx, -dy) * 180.0 / PI;
            const int k = static_cast<int>(dati - rho);
            bool cieco = std::fabs(rel) > SETTORE;
            float v = 0.0f;
            if (!cieco && k < righe) {
                const RigaCascata& c = storia[static_cast<std::size_t>(k)];
                const int n = static_cast<int>(c.db.size());
                if (n > 0) {
                    const int i = static_cast<int>(normalizza_360(rel + rotta_ - c.rilevamento0) / c.passo) % n;
                    cieco = static_cast<std::size_t>(i) < c.cieco.size() && c.cieco[static_cast<std::size_t>(i)];
                    v = norm[static_cast<std::size_t>(k)][static_cast<std::size_t>(i)];
                }
            }
            if (cieco) {   // a tratteggio tenue, non un blocco
                if ((x + y) % 6 == 0) t.pixel(x, y, griglia);
                continue;
            }
            if (k < righe) tratto_cascata(t, x, y, 1, 1, v);
        }
    const auto punto = [&](double rel, double rho) {
        const double a = rel * PI / 180.0;
        return std::pair<int, int>{static_cast<int>(std::lround(xc + rho * std::sin(a) - 0.5)), static_cast<int>(std::lround(yc - rho * std::cos(a) - 0.5))};
    };
    // i minuti: un cerchio di puntini ogni 30 righe di 2 s (alla scala x1)
    const int riga_min = std::max(1, 30 / std::max(1, scala_tempi_));
    for (int m = riga_min; dati - m > dentro; m += riga_min)
        for (int g = -120; g <= 120; g += 3) {
            const auto [x, y] = punto(g, dati - m);
            t.pixel(x, y, griglia);
        }
    for (const double g : {-SETTORE, SETTORE})
        for (int k = dentro; k < dati; k += 3) {
            const auto [x, y] = punto(g, k);
            t.pixel(x, y, tenue);
        }
    for (int g = -180; g < 180; g += 5) {
        const int lun = g % 30 == 0 ? 6 : g % 10 == 0 ? 4 : 2;
        const bool fuori = std::abs(g) > SETTORE;
        for (int k = 1; k <= lun; ++k) {
            const auto [x, y] = punto(g, dati + k);
            t.pixel(x, y, fuori ? griglia : tenue);
        }
        if (g % 30 == 0) {
            const std::string n = g == 0 ? "0" : g == -180 ? "180" : (g < 0 ? "L" : "P") + std::to_string(std::abs(g));
            const auto [x, y] = punto(g, dati + 14);
            scrivi(x - largo(n) / 2, y - 3, n, fuori ? griglia : testo);
        }
    }
    // il fascio puntato
    const double fascio = normalizza_180(fascio_ - rotta_);
    for (const double lato : {-2.5, 2.5}) {
        for (int k = dentro; k < dati; k += 3) {
            const auto [x, y] = punto(fascio + lato, k);
            t.pixel(x, y, fascio_c);
        }
        for (int k = 0; k < 4; ++k) {
            const auto [x, y] = punto(fascio + lato * (1.0 + k * 0.3), dati + 1);
            t.pixel(x, y, fascio_c);
        }
    }
    // i tracker, in ordine di rilevamento relativo: la tacca e il nome con l'ombra nera
    std::vector<std::pair<double, const TrackerSonar*>> tr;
    for (const auto& x : tracker_)
        if (const double rel = normalizza_180(x.rilevamento - rotta_); std::fabs(rel) <= SETTORE) tr.push_back({rel, &x});
    std::sort(tr.begin(), tr.end(), [](const auto& a, const auto& b) { return a.first < b.first; });
    double prima = -999.0;
    int giro = 0;
    for (const auto& [rel, x] : tr) {
        giro = rel - prima < 14.0 ? 1 - giro : 0;
        prima = rel;
        const std::uint8_t c = fosforo_1989 ? testo : colore_tracker(*x, modo_);
        for (int k = 1; k <= 5; ++k) {
            const auto [px, py] = punto(rel, dati - k);
            t.pixel(px, py, c);
        }
        const std::string nome = "S" + std::to_string(x->numero);
        const auto [px, py] = punto(rel, dati - 12 - 10 * giro);
        fp.scrivi(t, px - largo(nome) / 2 + 1, py - 2, nome, colori::NERO, 1, sp, false);
        scrivi(px - largo(nome) / 2, py - 3, nome, c);
    }
    // gli echi falsi dell'ultimo ping: un punto sul bordo dei dati, che si spegne
    for (const EcoVisto& e : echi_) {
        const double rel = normalizza_180(e.rilevamento - rotta_);
        if (std::fabs(rel) > SETTORE || tempo_ - e.t >= ECO_DURATA_S) continue;
        const auto [x, y] = punto(rel, dati - 2);
        t.rettangolo_pieno(x - 1, y - 1, 2, 2, tempo_ - e.t < ECO_DURATA_S / 2 ? testo : tenue);
    }
    // il proprio battello al centro
    for (int k = -3; k <= 3; ++k) t.pixel(cx, cy + k, testo);
    t.pixel(cx - 1, cy - 2, testo), t.pixel(cx + 1, cy - 2, testo);
}

// Il riquadro della riga agganciata, in alto a destra sul LOFAR: la frequenza del verniero, la curva a S (punti misurati e
// la curva del CPA), il CPA e il FRAZ con le incertezze o il motivo per cui non valgono
void SonarEnhanced::disegna_riga_misurata(Tela& t, const Font& f, const Disposizione& d) const {
    const int LX = d.lx, LW = d.lw, LY = d.ly;
    const RigaMisurata& r = *riga_;
    const int w = 190, h = 92, x0 = LX + LW - w - 2, y0 = LY + 2;
    t.rettangolo_pieno(x0, y0, w, h, colori::NERO);
    t.linea_orizzontale(x0, x0 + w - 1, y0, colori::GRIGIO);
    t.linea_orizzontale(x0, x0 + w - 1, y0 + h - 1, colori::GRIGIO);
    t.linea_verticale(x0, y0, y0 + h - 1, colori::GRIGIO);
    t.linea_verticale(x0 + w - 1, y0, y0 + h - 1, colori::GRIGIO);
    char riga[96];
    if (r.ultima.valida)
        std::snprintf(riga, sizeof riga, "VERNIERO %.4f +-%.4f HZ", r.ultima.hz, r.ultima.sigma_hz);
    else
        std::snprintf(riga, sizeof riga, "VERNIERO %.2f HZ: %s", r.hz, r.serie.empty() ? "IN MISURA" : "RIGA PERSA");
    f.scrivi(t, x0 + 4, y0 + 3, riga, colori::GIALLO, 1);
    // la curva a S: frequenza (verticale) contro tempo (orizzontale) nella finestra delle misure
    const int px = x0 + 4, py = y0 + 13, pw = w - 8, ph = 48;
    t.linea_orizzontale(px, px + pw - 1, py + ph, colori::GRIGIO_SCURO);
    if (r.serie.size() >= 2) {
        double fmin = 1e18, fmax = -1e18;
        for (const auto& p : r.serie) fmin = std::min(fmin, p.hz), fmax = std::max(fmax, p.hz);
        if (fmax - fmin < 0.02) fmin -= 0.01, fmax += 0.01;
        const double t0 = r.serie.front().t, t1 = std::max(r.serie.back().t, t0 + 1.0);
        const auto x_di = [&](double tt) { return px + static_cast<int>((tt - t0) / (t1 - t0) * (pw - 1)); };
        const auto y_di = [&](double hz) { return py + ph - static_cast<int>((hz - fmin) / (fmax - fmin) * ph); };
        if (r.cpa.valida)
            for (int x = 0; x < pw; ++x) {
                const double tt = t0 + (t1 - t0) * x / (pw - 1);
                const int y = y_di(frequenza_cpa(r.cpa, tt));
                if (y >= py && y <= py + ph) t.pixel(px + x, y, colori::VERDE);
            }
        for (const auto& p : r.serie) t.pixel(x_di(p.t), y_di(p.hz), colori::BIANCO);
    }
    if (r.cpa.valida) {
        const double dt = r.cpa.t_cpa - tempo_;
        std::snprintf(riga, sizeof riga, "CPA %s%d' %.1f KM %.0f KN F0 %.2f", dt < 0 ? "-" : "+", static_cast<int>(std::fabs(dt) / 60.0),
                      r.cpa.distanza_cpa_m / 1000.0, r.cpa.velocita_relativa_m_s / 0.5144, r.cpa.f0_hz);
        f.scrivi(t, x0 + 4, y0 + 66, riga, colori::VERDE_CHIARO, 1);
    } else {
        std::string mot = r.serie.size() < 8 ? std::string("POCHE MISURE") : r.cpa.motivo;
        for (auto& ch : mot) ch = static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
        f.scrivi(t, x0 + 4, y0 + 66, "CPA: " + mot.substr(0, 30), colori::GRIGIO, 1);
    }
    if (r.fraz.valida) {
        std::snprintf(riga, sizeof riga, "FRAZ %.1f KM +-%.1f", r.fraz.distanza_m / 1000.0, r.fraz.sigma_m / 1000.0);
        f.scrivi(t, x0 + 4, y0 + 78, riga, colori::VERDE_CHIARO, 1);
    } else {
        std::string mot = r.fraz.motivo.empty() ? std::string("POCHE MISURE") : r.fraz.motivo;
        for (auto& ch : mot) ch = static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
        f.scrivi(t, x0 + 4, y0 + 78, "FRAZ: " + mot.substr(0, 29), colori::GRIGIO, 1);
    }
}

// Il colore di un tracker nel modo moderno: per il passo della classificazione (perso grigio scuro); nel 1989 il fosforo
static std::uint8_t colore_tracker(const TrackerSonar& tr, ModoSonar modo) {
    if (tr.perso) return modo == ModoSonar::anni_89 ? colori::VERDE : colori::GRIGIO_SCURO;
    if (modo == ModoSonar::anni_89) return colori::VERDE_CHIARO;
    switch (tr.passo) {
        case PassoClassifica::certsub:
        case PassoClassifica::probsub: return ROSSO_CHIARO;
        case PassoClassifica::possub: return colori::GIALLO;
        case PassoClassifica::nonsub: return colori::GRIGIO;
        default: return colori::BIANCO;
    }
}

void SonarEnhanced::disegna_btr(Tela& t, const Font& f, const Disposizione& d) const {
    const int X0 = d.area.x, BW = d.bw, TOP = d.top, FONDO = d.fondo;
    char riga[160];
    std::string sensore = maiuscolo(sensore_);
    std::replace(sensore.begin(), sensore.end(), '_', ' ');   // bqq5_sfera: il trattino basso sembra un cursore
    std::snprintf(riga, sizeof riga, "CASCATA %s  INT. %d  X%d", sensore.c_str(), integrazione_, scala_tempi_);
    f.scrivi(t, X0 + 6, d.area.y + 3, riga, c_titolo(), 1);
    // la scala dei rilevamenti (docs/ricerca/30 §4.2): una tacca ogni 5 gradi, lunga ogni 10, il numero ogni 30
    for (int g = 0; g <= 360; g += 5) {
        const int x = std::min(X0 + g * BW / 360, X0 + BW - 1);
        const int lun = g % 30 == 0 ? 5 : g % 10 == 0 ? 3 : 1;
        t.linea_verticale(x, TOP - 1 - lun, TOP - 1, c_tenue());
        if (g % 30 == 0 && g < 360) f.scrivi(t, x + (g ? -f.larghezza(std::to_string(g), 1) / 2 : 1), d.area.y + 27, std::to_string(g), c_tenue(), 1);
    }
    const auto& storia = btr_[static_cast<std::size_t>(indice_scala(scala_tempi_))];
    std::optional<cascate::Fosforo> fo;   // modo 1989: la cascata sul fosforo (i fasci ciechi restano spenti)
    if (con_fosforo()) fo.emplace(BW, std::max(1, FONDO - TOP));
    std::vector<Rettangolo> ciechi;   // a tratteggio sopra la cascata, non un blocco pieno (la bozza A)
    for (std::size_t r = 0; r < storia.size(); ++r) {
        const int y = TOP + static_cast<int>(r) * 2;
        if (y + 2 > FONDO) break;
        const auto& c = storia[r];
        const auto v = normalizza_btr(c);
        for (std::size_t i = 0; i < v.size(); ++i) {
            const double ril = normalizza_360(c.rilevamento0 + c.passo * static_cast<double>(i));
            const int x0 = X0 + static_cast<int>(ril / 360.0 * BW), x1 = X0 + static_cast<int>((ril + c.passo) / 360.0 * BW);
            const int w = std::max(1, std::min(x1, X0 + BW) - x0);
            const bool cieco = i < c.cieco.size() && c.cieco[i];
            if (cieco) {
                ciechi.push_back({x0, y, w, 2});
            } else if (fo) {
                for (int px = x0; px < x0 + w; ++px)
                    for (int py = y; py < y + 2; ++py) fo->deponi(px - X0, py - TOP, cascate::frazione(v[i], pavimento_, tetto_));
            } else {
                tratto_cascata(t, x0, y, w, 2, v[i]);
            }
        }
    }
    if (fo) fo->componi(t, X0, TOP, cascate::SCALA_LUNGA, effetti_fosforo());
    for (const auto& c : ciechi)
        for (int py = c.y; py < c.y + c.h; ++py)
            for (int px = c.x; px < c.x + c.w; ++px) t.pixel(px, py, (px + py) % 8 == 0 ? c_tenue() : colori::NERO);
    // gli echi falsi dell'ultimo ping attivo: una tacca larga un fascio nelle righe piu' recenti, che si spegne in
    // ECO_DURATA_S (piu' forte, piu' chiara); niente lettera ne' tracker
    for (const EcoVisto& e : echi_) {
        const double eta = (tempo_ - e.t) / ECO_DURATA_S;
        if (eta >= 1.0) continue;
        const int x = X0 + static_cast<int>(normalizza_360(e.rilevamento) / 360.0 * BW);
        const bool forte = e.eccesso_db > 6.0 && eta < 0.5;
        const std::uint8_t c = modo_ == ModoSonar::anni_89 ? (forte ? colori::VERDE_CHIARO : colori::VERDE) : (forte ? colori::BIANCO : colori::GRIGIO);
        t.rettangolo_pieno(x - 1, TOP, 3, 6 - static_cast<int>(eta * 4.0), c);
    }
    disegna_griglia(t, f, d, true);
    // il fascio del LOFAR: una barretta sulla scala e i bordi a puntini lungo la cascata (la banda passante delle SDR)
    const double largo_fascio = !storia.empty() && storia.front().passo > 0.0 ? storia.front().passo : 5.0;
    const int xf = X0 + static_cast<int>(normalizza_360(fascio_) / 360.0 * BW);
    const int mezzo = std::max(1, static_cast<int>(std::lround(largo_fascio / 2.0 / 360.0 * BW)));
    const std::uint8_t cf = modo_ == ModoSonar::anni_89 ? colori::VERDE_CHIARO : ROSSO_CHIARO;
    t.rettangolo_pieno(xf - mezzo, TOP - 9, 2 * mezzo + 1, 3, cf);
    puntini_v(t, std::max(X0, xf - mezzo - 1), TOP, FONDO - 1, cf);
    puntini_v(t, std::min(X0 + BW - 1, xf + mezzo + 1), TOP, FONDO - 1, cf);
    // i tracker come segnalibri sopra la scala (GQRX): l'etichetta, a due righe se si toccano, e la linea a puntini
    // fino alla cascata; nel moderno il colore del passo, il seguito nel riquadro
    std::vector<const TrackerSonar*> ordinati;
    for (const auto& tr : tracker_) ordinati.push_back(&tr);
    std::sort(ordinati.begin(), ordinati.end(), [](const TrackerSonar* a, const TrackerSonar* b) { return normalizza_360(a->rilevamento) < normalizza_360(b->rilevamento); });
    int ultimo = -100, fila = 0;
    for (const TrackerSonar* tr : ordinati) {
        const int x = X0 + static_cast<int>(normalizza_360(tr->rilevamento) / 360.0 * BW);
        const std::string nome = "S" + std::to_string(tr->numero);
        const int w = f.larghezza(nome, 1);
        fila = x - ultimo < w + 4 ? 1 - fila : 0;
        ultimo = x;
        const int ty = d.area.y + 12 + fila * 7;
        const std::uint8_t col = colore_tracker(*tr, modo_);
        const int lx = std::clamp(x - w / 2, X0, X0 + BW - w);
        if (tr->numero == segue_tracker_) {   // il seguito dal LOFAR: nel riquadro
            t.linea_orizzontale(lx - 2, lx + w + 1, ty - 1, col), t.linea_orizzontale(lx - 2, lx + w + 1, ty + 8, col);
            t.linea_verticale(lx - 2, ty - 1, ty + 8, col), t.linea_verticale(lx + w + 1, ty - 1, ty + 8, col);
        }
        f.scrivi(t, lx, ty, nome, col, 1);
        for (int y = ty + 9; y < TOP + 6; y += 3) t.pixel(x, y, col);
    }
}

// La griglia del tempo: una linea ogni 1, 5 o 15 minuti secondo la scala, sulla cascata dei rilevamenti (se c'e') e
// sul LOFAR, coi minuti a destra della prima
void SonarEnhanced::disegna_griglia(Tela& t, const Font& f, const Disposizione& d, bool con_btr) const {
    const int minuti = scala_tempi_ == 1 ? 1 : scala_tempi_ == 4 ? 5 : 15;
    const int righe_linea = minuti * 60 / static_cast<int>(PASSO_S * scala_tempi_);
    const int X0 = d.area.x, TOP = con_btr ? d.top : d.ly;
    for (int r = righe_linea, k = 1; TOP + r * 2 < d.fondo; r += righe_linea, ++k) {
        const int y = TOP + r * 2;
        if (con_btr) puntini_o(t, X0, X0 + d.bw - 1, y, c_tenue());
        if (y < d.fl) puntini_o(t, d.lx, d.lx + d.lw - 1, y, c_tenue());   // sotto il LOFAR c'e' il DEMON
        if (con_btr || y < d.fl) f.scrivi(t, con_btr ? X0 + d.bw - 26 : d.lx + d.lw - 26, y - 8, std::to_string(k * minuti) + "'", c_tenue(), 1);
    }
}

void SonarEnhanced::disegna_lofar(Tela& t, const Font& f, const Disposizione& d) const {
    const int LX = d.lx, LW = d.lw, AY = d.ay, AH = d.ah, LY = d.ly, FONDO = d.fl;
    char riga[160];
    const auto [f0, f1] = vista_hz();
    const std::string sorgente = fascio_fisso_ ? "FISSO" : segue_tracker_ ? "S" + std::to_string(segue_tracker_) : "BERSAGLIO";
    const std::string zoom = zoom_ > 1 ? "  ZOOM X" + std::to_string(zoom_) : "";
    static constexpr const char* MODI_CUFFIA[] = {"E ASCOLTO", "OTTAVA SU", "CUFFIA X4", "CUFFIA X16"};
    const std::string ascolto = MODI_CUFFIA[static_cast<int>(modo_cuffia_)];
    // sull'Alfa le scritte di Okean in russo traslitterato (scelta della principale del 29/10/2026): SPEKTR, IST., GC
    const bool okean = classe_ == "alfa";
    const char* lofar = okean ? "SPEKTR" : "LOFAR";
    std::snprintf(riga, sizeof riga, "%s %03.0f° %s (%s)  %.0f-%.0f %s%s", lofar, normalizza_360(fascio_), okean ? "IST." : "VERO",
                  sorgente.c_str(), f0, f1, okean ? "GC" : "HZ", zoom.c_str());
    if (f.larghezza(riga, 1) + f.larghezza(ascolto, 1) + 12 > LW)   // nel CRT della consolle: il titolo corto
        std::snprintf(riga, sizeof riga, "%s %03.0f° %s %.0f-%.0f%s", lofar, normalizza_360(fascio_), sorgente.c_str(), f0, f1, zoom.c_str());
    f.scrivi(t, LX + 2, d.area.y + 3, riga, c_titolo(), 1);
    {   // l'ascolto: E registra; Q il modo della cuffia, scritto al posto di E quando non e' quello normale
        const std::string& a = ascolto;
        f.scrivi(t, LX + LW - 2 - f.larghezza(a, 1), d.area.y + 3, a, modo_cuffia_ == ModoCuffia::normale ? c_tenue() : colori::GIALLO, 1);
    }
    const auto x_di = [&, f0 = f0, f1 = f1](double hz) { return LX + static_cast<int>((hz - f0) / (f1 - f0) * LW); };
    // A-scan: l'ultima riga, col massimo (giallo) e il minimo (grigio) delle ultime 8; con uno slot pieno e il
    // confronto acceso, lo slot affiancato (sotto), sovrapposto (ciano) o la differenza dall'ultima riga
    t.linea_orizzontale(LX, LX + LW - 1, AY + AH, colori::GRIGIO_SCURO);
    const Slot& sl = slots_[static_cast<std::size_t>(slot_)];
    const bool con_slot = confronto_ != Confronto::spento && sl.pieno && slot_disponibili();
    if (!lofar_[0].empty()) {
        const std::vector<float> ora = pixel_lofar(lofar_[0].front(), f0, f1, LW);
        std::vector<float> alto = ora, basso = ora;
        for (std::size_t r = 1; r < std::min<std::size_t>(lofar_[0].size(), 8); ++r) {
            const auto px = pixel_lofar(lofar_[0][r], f0, f1, LW);
            for (std::size_t x = 0; x < px.size(); ++x) {
                alto[x] = std::max(alto[x], px[x]);
                basso[x] = std::min(basso[x], px[x]);
            }
        }
        const bool meta = con_slot && confronto_ == Confronto::affiancato;
        const int altezza = meta ? AH / 2 - 1 : AH;
        const auto h = [&](double v, int base) {
            return base - std::clamp(static_cast<int>((v - pavimento_) / (tetto_ - pavimento_) * altezza), 0, altezza);
        };
        const auto traccia = [&](const std::vector<float>& v, int base, std::uint8_t colore) {
            for (int x = 0; x < LW; ++x) {
                const auto i = static_cast<std::size_t>(x);
                const int y = h(v[i], base), yp = x ? h(v[i - 1], base) : y;
                t.linea_verticale(LX + x, std::min(y, yp), std::max(y, yp), colore);
            }
        };
        if (con_slot && confronto_ == Confronto::differenza) {   // ultima riga meno lo slot, attorno alla meta'
            const auto dif = pixel_lofar(differenza(lofar_[0].front(), sl.spettro), f0, f1, LW);
            const int zero = AY + AH / 2;
            t.linea_orizzontale(LX, LX + LW - 1, zero, colori::GRIGIO_SCURO);
            for (int x = 0; x < LW; ++x) {
                const int y = zero - std::clamp(static_cast<int>(dif[static_cast<std::size_t>(x)] / (tetto_ - pavimento_) * AH), -AH / 2, AH / 2);
                t.linea_verticale(LX + x, std::min(y, zero), std::max(y, zero), dif[static_cast<std::size_t>(x)] >= 0 ? colori::VERDE_CHIARO : colori::ROSSO);
            }
        } else {
            const int base = meta ? AY + AH / 2 - 1 : AY + AH;
            if (con_fosforo() && !con_slot) {   // 1989: la traccia sul fosforo, che si spegne piano al posto di massimo e minimo
                if (!fosforo_ascan_ || fosforo_ascan_->larghezza() != LW || fosforo_ascan_->altezza() != AH + 1)
                    fosforo_ascan_.emplace(LW, AH + 1);
                fosforo_ascan_->decadi(cascate::coefficiente(persistenza_));
                for (int x = 1; x < LW; ++x)
                    fosforo_ascan_->linea(x - 1, h(ora[static_cast<std::size_t>(x - 1)], base) - AY, x, h(ora[static_cast<std::size_t>(x)], base) - AY, 0.95);
                fosforo_ascan_->componi(t, LX, AY, cascate::SCALA_LUNGA, effetti_fosforo());
            } else {
                if (!meta)
                    for (int x = 0; x < LW; ++x) {
                        t.pixel(LX + x, h(basso[static_cast<std::size_t>(x)], base), colori::GRIGIO_SCURO);
                        t.pixel(LX + x, h(alto[static_cast<std::size_t>(x)], base), colori::GIALLO);
                    }
                traccia(ora, base, colori::VERDE_CHIARO);
            }
            if (con_slot) traccia(pixel_lofar(sl.spettro, f0, f1, LW), meta ? AY + AH : base, colori::CIANO_CHIARO);
        }
        if (con_slot) {
            static constexpr const char* MODI[] = {"", "AFFIANCATO", "SOVRAPPOSTO", "DIFFERENZA"};
            std::snprintf(riga, sizeof riga, "SLOT %d %s  %s", slot_ + 1, sl.origine.c_str(), MODI[static_cast<int>(confronto_)]);
            f.scrivi(t, LX + LW - 2 - f.larghezza(riga, 1), AY + 10, riga, colori::CIANO_CHIARO, 1);
        }
    }
    // cascata
    const auto& storia = lofar_[static_cast<std::size_t>(indice_scala(scala_tempi_))];
    std::optional<cascate::Fosforo> fo;
    if (con_fosforo()) fo.emplace(LW, std::max(1, FONDO - LY));
    for (std::size_t r = 0; r < storia.size(); ++r) {
        const int y = LY + static_cast<int>(r) * 2;
        if (y + 2 > FONDO) break;
        const auto px = pixel_lofar(storia[r], f0, f1, LW);
        // il colore per rilevamento: i lati della riga misurata (la scala x1), se l'aiuto e' acceso
        const std::vector<std::int8_t>* lati = colore_rilevamento() && scala_tempi_ == 1 && r < lati_.size() && !lati_[r].empty() ? &lati_[r] : nullptr;
        for (int x = 0; x < LW; ++x) {
            const double db = px[static_cast<std::size_t>(x)];
            if (lati && db > pavimento_ + 1.0 && storia[r].df_hz > 0.0) {
                const double hz = f0 + (x + 0.5) * (f1 - f0) / LW;
                const auto bin = static_cast<long>((hz - storia[r].f0_hz) / storia[r].df_hz);
                const int lato = bin >= 0 && static_cast<std::size_t>(bin) < lati->size() ? (*lati)[static_cast<std::size_t>(bin)] : 0;
                if (lato != 0) {
                    const bool forte = cascate::frazione(db, pavimento_, tetto_) > 0.5;
                    t.rettangolo_pieno(LX + x, y, 1, 2, lato < 0 ? (forte ? BLU_CHIARO : colori::BLU) : (forte ? VIOLA : VIOLA_SCURO));
                    continue;
                }
            }
            if (!fo) tratto_cascata(t, LX + x, y, 1, 2, db);
            else for (int py = y; py < y + 2; ++py) fo->deponi(x, py - LY, cascate::frazione(db, pavimento_, tetto_));
        }
    }
    if (fo) fo->componi(t, LX, LY, cascate::SCALA_LUNGA, effetti_fosforo());
    // le righe automatiche del modo moderno: le marche sul bordo alto della cascata, la lista col cursore in modo righe
    if (modo_ == ModoSonar::moderno) {
        const auto conf = rilevatore_.confermate();
        StileRighe sr;
        sr.f0_hz = f0;
        sr.f1_hz = f1;
        righe_auto_.disegna_marche(t, f, {LX, LY, LW, FONDO - LY}, conf, sr);
        if (cursore_ == Cursore::righe) righe_auto_.disegna_lista(t, f, {LX + 2, LY + 2, 200, 80}, conf, tempo_);
    }
    // il band plan fra l'A-scan e il LOFAR (SDR++, SDR#; docs/ricerca/30 §4.3): nel moderno le righe attese della voce
    // sovrapposta, colorate per tipo; nel 1989 solo le tacche fisse di 50, 60 e 400 Hz. Sopra, le righe del proprio
    // battello (ciano; blu se mascherate)
    {
        const int b0 = AY + AH + 2, b1 = LY - 2;
        if (modo_ == ModoSonar::moderno && sovrapposta_) {
            for (const auto& r : sovrapposta_->righe)
                if (r.hz_fine <= 0.0 && r.hz * k_ >= f0 && r.hz * k_ <= f1) t.rettangolo_pieno(x_di(r.hz * k_) - 1, b0 + 1, 3, std::max(1, b1 - b0 - 1), colore_riga(r.tipo));
        } else if (modo_ == ModoSonar::anni_89) {
            for (const double hz : {50.0, 60.0, 400.0})
                if (hz >= f0 && hz <= f1) t.linea_verticale(x_di(hz), b1 - 3, b1, colori::VERDE_CHIARO);
        }
        for (const auto& r : righe_proprie_)
            if (r.hz >= f0 && r.hz <= f1)
                t.linea_verticale(x_di(r.hz), AY + AH + 1, b0 + 2, modo_ == ModoSonar::anni_89 ? colori::VERDE : maschera_ ? colori::BLU : colori::CIANO_CHIARO);
    }
    // i cerchietti sulle righe che l'analisi automatica ha trovato, sull'A-scan (Mark Peaks di SDR#; solo moderno)
    if (modo_ == ModoSonar::moderno)
        for (const auto& r : rilevatore_.confermate())
            if (r.hz >= f0 && r.hz <= f1) {
                const int cx = x_di(r.hz), cy = AY + 3;
                for (int k = 0; k < 12; ++k)
                    t.pixel(cx + static_cast<int>(std::lround(2.0 * std::cos(k * PI / 6))), cy + static_cast<int>(std::lround(2.0 * std::sin(k * PI / 6))), colori::CIANO_CHIARO);
            }
    // scala delle frequenze in fondo: un passo tondo con al piu' 8 etichette
    double passo = 100.0;
    for (const double p : {0.5, 1.0, 2.0, 5.0, 10.0, 20.0, 50.0, 100.0})
        if ((f1 - f0) / p <= 8.0) {
            passo = p;
            break;
        }
    for (double hz = std::ceil(f0 / passo) * passo; hz <= f1; hz += passo) {
        const int x = x_di(hz);
        t.linea_verticale(x, FONDO, FONDO + 3, c_tenue());
        char e[16];
        std::snprintf(e, sizeof e, "%g", hz);
        if (x + 2 + f.larghezza(e, 1) < LX + LW) f.scrivi(t, x + 2, FONDO + 1, e, c_tenue(), 1);
    }
    // la barra dello zoom sotto la scala (GQRX): la finestra nella banda intera e il fattore; quando sotto c'e' il DEMON
    if (FONDO < d.fondo) {
        const int by = FONDO + 12, bw = LW - 30;
        const double b0 = f_min(), b1 = F_MAX;
        puntini_o(t, LX, LX + bw - 1, by + 2, c_tenue());
        const int a = LX + static_cast<int>((f0 - b0) / (b1 - b0) * bw), b = LX + static_cast<int>((f1 - b0) / (b1 - b0) * bw);
        const int w = std::max(3, b - a);
        t.linea_orizzontale(a, a + w - 1, by, c_titolo()), t.linea_orizzontale(a, a + w - 1, by + 4, c_titolo());
        t.linea_verticale(a, by, by + 4, c_titolo()), t.linea_verticale(a + w - 1, by, by + 4, c_titolo());
        f.scrivi(t, LX + bw + 4, by - 2, "X" + std::to_string(zoom_), c_testo(), 1);
    }
    if (colore_rilevamento() && scala_tempi_ == 1) {   // la legenda del colore per rilevamento, in fondo alla cascata
        const char* leg = "RIL: SN BLU  DR VIOLA";
        f.scrivi(t, LX + LW - 2 - f.larghezza(leg, 1), FONDO - 12, leg, colori::GRIGIO, 1);   // staccata dai numeri della scala
    }
    // righe della voce della libreria sovrapposta, spostate per il fattore Doppler k; gli sweep come segmenti in alto
    if (sovrapposta_) {
        for (const auto& r : sovrapposta_->righe) {
            const double a0 = r.hz * k_;
            if (r.hz_fine > 0.0) {
                const double a1 = r.hz_fine * k_;
                const int xa = std::clamp(x_di(std::min(a0, a1)), LX, LX + LW - 1), xb = std::clamp(x_di(std::max(a0, a1)), LX, LX + LW - 1);
                if (xb > xa) t.linea_orizzontale(xa, xb, LY + 1, colore_riga(r.tipo));
            } else if (a0 >= f0 && a0 <= f1) {
                puntini_v(t, x_di(a0), LY, FONDO - 1, colore_riga(r.tipo));
            }
        }
        std::snprintf(riga, sizeof riga, "VOCE %s  K %.4f (%+.1f KN)", nome_voce(sovrapposta_->id).c_str(), k_,
                      (k_ - 1.0) * C_SUONO / NODO);
        f.scrivi(t, LX + 2, AY, riga, colori::CIANO_CHIARO, 1);
    }
    // annotazioni: un segno e il numero, che scendono con la cascata
    for (const auto& a : annotazioni_) {
        const int r = static_cast<int>((tempo_ - a.tempo) / (PASSO_S * scala_tempi_));
        const int y = LY + r * 2;
        if (r < 0 || y > FONDO - 8 || a.hz < f0 || a.hz > f1) continue;
        const int x = x_di(a.hz);
        t.linea_orizzontale(x - 3, x + 3, y, colori::BIANCO);
        t.linea_verticale(x, y - 3, y + 3, colori::BIANCO);
        f.scrivi(t, std::min(x + 4, LX + LW - 16), y - 3, "N" + std::to_string(a.numero), colori::BIANCO, 1);
    }
    // cursori
    if (cursore_ == Cursore::spento) return;
    const int yc = LY + cursore_riga_ * 2;
    if (cursore_hz_ >= f0 && cursore_hz_ <= f1) {
        puntini_v(t, x_di(cursore_hz_), AY, FONDO - 1, colori::BIANCO);
        t.linea_orizzontale(x_di(cursore_hz_) - 4, x_di(cursore_hz_) + 4, yc, colori::BIANCO);
    }
    if (cursore_ == Cursore::delta && riferimento_hz_ >= f0 && riferimento_hz_ <= f1) {
        puntini_v(t, x_di(riferimento_hz_), AY, FONDO - 1, colori::GIALLO);
        t.linea_orizzontale(x_di(riferimento_hz_) - 4, x_di(riferimento_hz_) + 4, LY + riferimento_riga_ * 2, colori::GIALLO);
    }
    if (cursore_ == Cursore::armonico) {   // la famiglia: k volte la frequenza del cursore (i numeri diradati se fitti)
        const int passo_px = std::max(1, x_di(2.0 * cursore_hz_) - x_di(cursore_hz_));
        const int ogni = std::max(1, (14 + passo_px - 1) / passo_px);
        for (int k = 2; cursore_hz_ * k <= f1; ++k)
            if (cursore_hz_ * k >= f0) {
                puntini_v(t, x_di(cursore_hz_ * k), LY, FONDO - 1, colori::GIALLO);
                if (k % ogni == 0) f.scrivi(t, x_di(cursore_hz_ * k) + 2, LY + 2, std::to_string(k), colori::GIALLO, 1);
            }
    }
}

void SonarEnhanced::disegna_fondo(Tela& t, const Font& f, const Disposizione& d) const {
    const int LX = d.lx, LW = d.lw, X0 = d.area.x;
    char riga[200];
    const int y0 = d.fondo + 12;
    // tracker, a sinistra
    f.scrivi(t, X0 + 4, y0, "TRACKER  RIL.   DB  °/MIN  PASSO    CLASSE", c_titolo(), 1);
    int k = 0;
    for (const auto& tr : tracker_) {
        if (k >= 4) break;
        const std::string classe = tr.classe.empty() ? "-" : maiuscolo(tr.classe) + " " + std::to_string(static_cast<int>(tr.confidenza * 100));
        const std::string nome = "S" + std::to_string(tr.numero);
        std::snprintf(riga, sizeof riga, "%-3s %c  %03.0f° %+4.0f  %+5.1f  %-8s %s", nome.c_str(), tr.automatico ? 'A' : ' ',
                      normalizza_360(tr.rilevamento), tr.livello_db, tr.rateo(), tr.perso ? "PERSO" : nome_passo(tr.passo), classe.c_str());
        // il colore del passo (moderno, come i segnalibri del BTR); il seguito dal LOFAR sulla fascia blu nel moderno, con
        // la freccia nel 1989 (il fosforo pieno coprirebbe il testo)
        const bool seguito = tr.numero == segue_tracker_;
        const int ty = y0 + 12 + k * 11;
        if (seguito && modo_ == ModoSonar::moderno) t.rettangolo_pieno(X0 + 2, ty - 1, d.bw - 4, 10, colori::BLU);
        if (seguito && modo_ == ModoSonar::anni_89) f.scrivi(t, X0 - 2, ty, ">", colori::VERDE_CHIARO, 1);
        f.scrivi(t, X0 + 4, ty, riga, colore_tracker(tr, modo_), 1);
        ++k;
    }
    if (tracker_.empty())
        f.scrivi(t, X0 + 4, y0 + 12, modo_ == ModoSonar::moderno ? "NESSUN TRACKER (T SUL FASCIO, O AUTOMATICI)" : "NESSUN TRACKER (T SUL FASCIO)",
                 c_tenue(), 1);
    // a destra: presentazione, cursore, DEMON, analizzatore
    static constexpr const char* NOMI[] = {"PAV", "TETTO", "", "", "K", "PERS", "RIL", "MASCH"};
    char valori[8][24];
    std::snprintf(valori[0], 24, "%+.0f", pavimento_);
    std::snprintf(valori[1], 24, "%+.0f", tetto_);
    std::snprintf(valori[2], 24, "%s", nome_palette(palette_));
    std::snprintf(valori[3], 24, "X%d", scala_tempi_);
    std::snprintf(valori[4], 24, "%.4f", k_);
    std::snprintf(valori[5], 24, "%.2f", cascate::coefficiente(persistenza_));
    std::snprintf(valori[6], 24, "%s", colore_ril_ ? "SI" : "NO");
    std::snprintf(valori[7], 24, "%s", maschera_ ? "SI" : "NO");
    std::string s = modo_ == ModoSonar::moderno ? "MODERNO " : "1989 ";
    for (int i = 0; i < 8; ++i) {
        const bool scelto = static_cast<int>(campo_) == i;
        if (i == 4 && !scelto && !sovrapposta_) continue;   // il fattore Doppler serve solo con una voce sovrapposta
        if (i == 5 && !scelto && !con_fosforo()) continue;   // la persistenza solo sul fosforo
        if (i == 6 && !scelto && modo_ != ModoSonar::moderno) continue;   // il colore per rilevamento solo nel moderno
        if (i == 7 && !scelto && !maschera_) continue;        // la maschera quando e' accesa o scelta
        s += std::string(scelto ? "[" : "") + NOMI[i] + (*NOMI[i] ? " " : "") + valori[i] + (scelto ? "] " : " ");
    }
    // senza il nome del modo se la riga non ci sta (si vede dall'aspetto); sulla consolle il vetro arriva sotto i tasti
    const int spazio = d.area.x + d.area.w + (d.consolle ? 40 : 0) - LX - 4;
    if (f.larghezza(s, 1) > spazio) s = s.substr(s.find(' ') + 1);
    f.scrivi(t, LX + 2, y0, s, c_testo(), 1);
    const double secondi_riga = PASSO_S * scala_tempi_;
    switch (cursore_) {
        case Cursore::spento: std::snprintf(riga, sizeof riga, "CURSORE SPENTO (9)"); break;
        case Cursore::lettura:
            std::snprintf(riga, sizeof riga, "CURSORE %.1f HZ  %s", cursore_hz_, tempo_indietro(cursore_riga_ * secondi_riga).c_str());
            break;
        case Cursore::delta:
            std::snprintf(riga, sizeof riga, "DELTA %+.2f HZ  %s  (RIF. %.1f HZ)", cursore_hz_ - riferimento_hz_,
                          tempo_indietro(std::abs(cursore_riga_ - riferimento_riga_) * secondi_riga).substr(1).c_str(), riferimento_hz_);
            break;
        case Cursore::armonico: std::snprintf(riga, sizeof riga, "ARMONICHE DI %.2f HZ (K F)", cursore_hz_); break;
        case Cursore::righe: std::snprintf(riga, sizeof riga, "RIGHE AUTOMATICHE: SU E GIU' LA RIGA, F IL VERNIERO"); break;
        case Cursore::storia: std::snprintf(riga, sizeof riga, "STORIA: SU E GIU' IL TEMPO, %s LA RISOLUZIONE, F LA COMPARSA", tasti_valore().c_str()); break;
        case Cursore::demon:
            std::snprintf(riga, sizeof riga, "DEMON: FRECCE ASSE E PALE%s", modo_ == ModoSonar::moderno ? ", F AGGANCIA LA SERIE" : "");
            break;
    }
    // la riga del cursore che non ci sta va a capo all'ultima virgola che ci sta (la storia, le righe nella sala sonar):
    // sotto si stringe per restare sul vetro della consolle, e sopra no: le parentesi e gli accenti sconfinano
    std::string cursore_testo = riga, cursore_seguito;
    if (f.larghezza(cursore_testo, 1) > spazio)
        for (auto v = cursore_testo.rfind(", "); v != std::string::npos && v > 0; v = cursore_testo.rfind(", ", v - 1))
            if (f.larghezza(cursore_testo.substr(0, v + 1), 1) <= spazio) {
                cursore_seguito = cursore_testo.substr(v + 2);
                cursore_testo.resize(v + 1);
                break;
            }
    const std::uint8_t c_cursore = cursore_ == Cursore::spento ? c_tenue() : colori::GIALLO;
    const bool a_capo = !cursore_seguito.empty();
    const int passo = a_capo ? 10 : 12, y_sotto = a_capo ? y0 + 33 : y0 + 24;
    f.scrivi(t, LX + 2, y0 + 12, cursore_testo, c_cursore, 1);
    if (a_capo) f.scrivi(t, LX + 2, y0 + 23, cursore_seguito, c_cursore, 1);
    // il verniero si aggancia alla riga del cursore, nei modi dove F fa questo, se la riga gli lascia il posto
    if (cursore_ == Cursore::lettura || cursore_ == Cursore::delta || cursore_ == Cursore::armonico) {
        const char* v = riga_ ? "F SGANCIA" : "F VERNIERO";
        const int xv = LX + LW - 2 - f.larghezza(v, 1);
        if (!a_capo && LX + 2 + f.larghezza(cursore_testo, 1) + 6 <= xv) f.scrivi(t, xv, y0 + 12, v, c_tenue(), 1);
    }
    if (campo_ == Campo::maschera) {   // il catalogo delle righe proprie nel fascio, al posto di DEMON e analizzatore
        std::string cat = std::string("RIGHE PROPRIE") + (maschera_ ? " (MASCHERATE):" : ":");
        if (righe_proprie_.empty()) cat += " NESSUNA NEL FASCIO";
        f.scrivi(t, LX + 2, y_sotto, cat, colori::CIANO_CHIARO, 1);
        std::string elenco;
        for (std::size_t i = 0; i < righe_proprie_.size() && i < 4; ++i) {
            char e[48];
            std::snprintf(e, sizeof e, "%s%.1f %s", i ? "  " : "", righe_proprie_[i].hz, maiuscolo(righe_proprie_[i].macchina).c_str());
            elenco += e;
        }
        f.scrivi(t, LX + 2, y_sotto + passo, elenco, colori::BIANCO, 1);
        return;
    }
    if (tempo_ < riga_nuova_fino_) {   // una riga propria nuova: la segnala al posto del DEMON per mezzo minuto
        std::snprintf(riga, sizeof riga, "RIGA PROPRIA NUOVA A %.1f HZ%s", riga_nuova_hz_, maschera_ ? " (MASCHERATA)" : "");
        f.scrivi(t, LX + 2, y_sotto, riga, colori::GIALLO, 1);
    } else {
        if (lettura_) std::snprintf(riga, sizeof riga, "DEMON: ASSE %.1f HZ  PALA %.1f HZ  %d PALE", lettura_->asse_hz, lettura_->pala_hz, lettura_->pale);
        else std::snprintf(riga, sizeof riga, "DEMON: NESSUNA CAVITAZIONE LEGGIBILE");
        f.scrivi(t, LX + 2, y_sotto, riga, c_testo(), 1);
    }
    // analisi per rapporti: le voci che le righe reggono, con quante ne spiegano; le famiglie armoniche
    if (!confronti_.empty()) {
        LetturaAnalisi l = lettura_analisi(14);
        if (f.larghezza(l.testo, 1) > spazio) l = lettura_analisi(14, true);   // la seconda voce, se ci sta
        f.scrivi(t, LX + 2, y_sotto + passo, l.testo, l.nome ? colori::GIALLO : c_tenue(), 1);
    } else if (!ipotesi_.empty()) {
        const auto& h = ipotesi_.front();
        std::snprintf(riga, sizeof riga, "%s  %s  %.0f PER CENTO  (%d RIGHE)", maiuscolo(h.classe).c_str(), maiuscolo(h.nazionalita).c_str(),
                      h.confidenza * 100.0, h.righe_concordi);
        f.scrivi(t, LX + 2, y_sotto + passo, riga, colori::GIALLO, 1);
    } else {
        f.scrivi(t, LX + 2, y_sotto + passo, "ANALIZZATORE: 4 PER ANALIZZARE IL FASCIO", c_tenue(), 1);
    }
    std::string fam;
    for (std::size_t i = 0; i < std::min<std::size_t>(famiglie_.size(), 2); ++i) {
        const auto& x = famiglie_[i];
        char e[80];
        if (x.pale_stimate) std::snprintf(e, sizeof e, "%s %.1f HZ %d PALE", maiuscolo(x.indizio).c_str(), x.pala_hz, x.pale_stimate);
        else std::snprintf(e, sizeof e, "%s %.1f HZ X%zu", maiuscolo(x.indizio).c_str(), x.f1_hz, x.armoniche.size());
        fam += (fam.empty() ? "FAMIGLIE: " : "  ") + std::string(e);
    }
    if (!fam.empty() && !a_capo) f.scrivi(t, LX + 2, y0 + 47, fam, c_testo(), 1);   // col capo non ci stanno: restano nella pagina dell'analizzatore
}

// --- libreria, annotazioni e slot (S5b) ---

void SonarEnhanced::aggiorna_scelta() {
    elenco_.clear();
    nomi_elenco_.clear();
    scelta_.reset();
    if (!catalogo_) return;
    elenco_ = elenco_libreria(*catalogo_, SCHEDE[scheda_]);
    for (const auto& id : elenco_) nomi_elenco_.push_back(nome_voce(id));
    voce_ = elenco_.empty() ? 0 : std::clamp(voce_, 0, static_cast<int>(elenco_.size()) - 1);
    if (!elenco_.empty()) scelta_ = voce_libreria(*catalogo_, elenco_[static_cast<std::size_t>(voce_)]);
}

void SonarEnhanced::commuta_libreria() {
    libreria_aperta_ = !libreria_aperta_;
    if (libreria_aperta_) aggiorna_scelta();
}

void SonarEnhanced::scheda_successiva(int verso) {
    scheda_ = (scheda_ + verso + 5) % 5;
    voce_ = 0;
    aggiorna_scelta();
}

void SonarEnhanced::voce_successiva(int verso) {
    voce_ += verso;
    aggiorna_scelta();
}

void SonarEnhanced::sovrapponi() {
    if (!scelta_) return;
    sovrapposta_ = scelta_;
    k_ = 1.0;   // dall'ultima analisi per rapporti, se la voce c'era
    for (const auto& x : confronti_)
        if (x.id == sovrapposta_->id && !x.concordi.empty()) k_ = x.fattore_doppler;
    libreria_aperta_ = false;
}

std::string SonarEnhanced::nome_voce(const std::string& id) const {
    if (const auto it = nomi_.find(id); it != nomi_.end()) return it->second;
    if (catalogo_)
        if (const auto v = voce_libreria(*catalogo_, id); v && !v->unita_navale && !v->nome.empty()) return maiuscolo(v->nome);
    return maiuscolo(id);
}

void SonarEnhanced::annota() {
    Annotazione a;
    a.numero = prossima_annotazione_++;
    const auto [f0, f1] = vista_hz();
    a.hz = cursore_ != Cursore::spento ? cursore_hz_ : (f0 + f1) / 2.0;
    a.tempo = tempo_ - (cursore_ != Cursore::spento ? cursore_riga_ * PASSO_S * scala_tempi_ : 0.0);
    annotazioni_.push_back(a);
    if (annotazioni_.size() > static_cast<std::size_t>(MAX_ANNOTAZIONI)) annotazioni_.erase(annotazioni_.begin());
}

void SonarEnhanced::salva_slot() {
    if (lofar_[0].empty() || !slot_disponibili()) return;
    int dove = slot_;   // il primo vuoto da quello scelto, o quello scelto se sono tutti pieni
    for (int i = 0; i < SLOT; ++i)
        if (!slots_[static_cast<std::size_t>((slot_ + i) % SLOT)].pieno) {
            dove = (slot_ + i) % SLOT;
            break;
        }
    char ora[16];
    std::snprintf(ora, sizeof ora, "%02d:%02d", static_cast<int>(tempo_ / 3600.0), static_cast<int>(tempo_ / 60.0) % 60);
    slots_[static_cast<std::size_t>(dove)] = {lofar_[0].front(), tempo_, fascio_, segue_tracker_, true,
                                              (missione_.empty() ? std::string("?") : missione_) + " " + ora};
    slot_ = dove;
}

// Il quaderno: una riga per slot pieno, «slot K f0 df N origine v1 ... vN» (l'origine senza spazi: '_')
std::string SonarEnhanced::percorso_quaderno() { return (cartella_fascicoli().parent_path() / "quaderno_sonar.txt").string(); }

bool SonarEnhanced::salva_quaderno(const std::string& percorso) const {
    std::error_code e;
    std::filesystem::create_directories(std::filesystem::path(percorso).parent_path(), e);
    std::FILE* fp = std::fopen(percorso.c_str(), "w");
    if (!fp) return false;
    std::fprintf(fp, "# quaderno del sonarista di melchior688: gli slot del LOFAR fra le missioni\n");
    for (int k = 0; k < SLOT; ++k) {
        const Slot& s = slots_[static_cast<std::size_t>(k)];
        if (!s.pieno) continue;
        std::string o = s.origine.empty() ? std::string("?") : s.origine;
        for (auto& ch : o) if (ch == ' ') ch = '_';
        std::fprintf(fp, "slot %d %.6f %.6f %zu %s", k, s.spettro.f0_hz, s.spettro.df_hz, s.spettro.snr_db.size(), o.c_str());
        for (const float v : s.spettro.snr_db) std::fprintf(fp, " %.2f", static_cast<double>(v));
        std::fprintf(fp, "\n");
    }
    return std::fclose(fp) == 0;
}

int SonarEnhanced::carica_quaderno(const std::string& percorso) {
    std::ifstream in(percorso);
    if (!in) return 0;
    int letti = 0;
    for (std::string riga; std::getline(in, riga);) {
        std::istringstream campi(riga);
        std::string parola, origine;
        int k = -1;
        std::size_t n = 0;
        Slot s;
        if (!(campi >> parola) || parola != "slot" || !(campi >> k >> s.spettro.f0_hz >> s.spettro.df_hz >> n >> origine)) continue;
        if (k < 0 || k >= SLOT || n > 100000) continue;
        s.spettro.snr_db.resize(n);
        bool ok = true;
        for (auto& v : s.spettro.snr_db) ok = ok && static_cast<bool>(campi >> v);
        if (!ok) continue;
        for (auto& ch : origine) if (ch == '_') ch = ' ';
        s.origine = origine;
        s.pieno = true;
        slots_[static_cast<std::size_t>(k)] = std::move(s);
        ++letti;
    }
    return letti;
}

void SonarEnhanced::disegna_libreria(Tela& t, const Font& f, const Disposizione& d) const {
    const int X0 = d.area.x, Y0 = d.area.y;
    char riga[200];
    int x = X0 + 6;
    x = f.scrivi(t, x, Y0 + 4, "LIBRERIA", colori::CIANO_CHIARO, 1) + 14;
    for (int i = 0; i < 5; ++i) {
        const std::string nome = maiuscolo(SCHEDE[i]);
        x = f.scrivi(t, x, Y0 + 4, i == scheda_ ? "[" + nome + "]" : nome, i == scheda_ ? colori::GIALLO : colori::GRIGIO, 1) + 10;
    }
    // elenco delle voci, la scelta evidenziata
    const int visibili = 26, primo = std::max(0, voce_ - visibili + 1);
    for (int i = primo; i < std::min(static_cast<int>(nomi_elenco_.size()), primo + visibili); ++i) {
        const int y = Y0 + 20 + (i - primo) * 11;
        if (i == voce_) t.rettangolo_pieno(X0 + 2, y - 2, 196, 11, colori::GRIGIO_SCURO);
        f.scrivi(t, X0 + 6, y, nomi_elenco_[static_cast<std::size_t>(i)].substr(0, 30), i == voce_ ? colori::GIALLO : colori::BIANCO, 1);
    }
    if (nomi_elenco_.empty()) f.scrivi(t, X0 + 6, Y0 + 20, catalogo_ ? "NESSUNA VOCE" : "LIBRERIA NON DISPONIBILE", colori::GRIGIO, 1);
    f.scrivi(t, X0 + 6, Y0 + d.area.h - 12, "TAB SCHEDA  FRECCE VOCE  INVIO SOVRAPPONI AL LOFAR  B O ESC CHIUDE", colori::GRIGIO, 1);
    if (!scelta_) return;
    // scheda della voce
    const VoceLibreria& v = *scelta_;
    const int dx = X0 + 210, larghezza = X0 + d.area.w - dx - 8;
    int y = Y0 + 20;
    f.scrivi(t, dx, y, nome_voce(v.id), colori::GIALLO, 1);
    y += 13;
    std::snprintf(riga, sizeof riga, "%s  %s  %s", maiuscolo(v.categoria).c_str(), maiuscolo(v.nazionalita).c_str(), maiuscolo(v.tipo_unita).c_str());
    f.scrivi(t, dx, y, riga, colori::BIANCO, 1);
    y += 11;
    if (v.pale) {
        std::snprintf(riga, sizeof riga, "ELICA A %d PALE  %.1f GIRI AL MINUTO PER NODO", v.pale, v.giri_per_nodo);
        f.scrivi(t, dx, y, riga, colori::BIANCO, 1);
        y += 11;
    }
    const auto paragrafo = [&](const std::string& titolo, const std::string& testo, std::uint8_t colore) {
        if (testo.empty() || y > Y0 + 228) return;
        std::string pulito = maiuscolo(titolo + testo);   // il font non ha il paragrafo
        for (std::size_t k; (k = pulito.find("\xc2\xa7")) != std::string::npos;) pulito.replace(k, 2, "PAR. ");
        for (const auto& l : StazioneRadio::a_capo(f, pulito, larghezza)) {
            if (y > Y0 + 228) break;
            f.scrivi(t, dx, y, l, colore, 1);
            y += 11;
        }
    };
    const auto unisci = [](const std::vector<std::string>& elenco) {
        std::string r;
        for (const auto& e : elenco) r += (r.empty() ? "" : ", ") + e;
        return r;
    };
    std::string righe;
    for (const auto& r : v.righe) righe += (righe.empty() ? "" : "; ") + r.etichetta;
    paragrafo("RIGHE: ", righe, colori::CIANO_CHIARO);
    paragrafo("RITMO: ", v.ritmo, colori::BIANCO);
    paragrafo("INDIZI: ", unisci(v.indizi), colori::BIANCO);
    paragrafo("SI CONFONDE CON: ", unisci(v.confonde_con), colori::GIALLO);
    paragrafo("ALL'ASCOLTO: ", v.aurale, colori::BIANCO);
    paragrafo("FONTE: ", v.fonte + (v.confidenza.empty() ? "" : " (" + v.confidenza + ")"), colori::GRIGIO);
    // spettro di riferimento: righe in altezza secondo il livello relativo, bande in grigio, sweep come segmenti
    const int sx = dx, sw = larghezza, sy = Y0 + 248, sh = 70;
    t.rettangolo_pieno(sx, sy, sw, sh, colori::NERO);
    t.linea_orizzontale(sx, sx + sw - 1, sy + sh, colori::GRIGIO);
    const auto xs = [&](double hz) { return sx + static_cast<int>((hz - F_MIN_BANDA) / (F_MAX - F_MIN_BANDA) * sw); };
    for (const auto& [lo, hi] : v.bande)
        if (hi > F_MIN_BANDA && lo < F_MAX)
            t.rettangolo_pieno(std::max(sx, xs(lo)), sy + sh - 8, std::max(1, std::min(sx + sw, xs(hi)) - std::max(sx, xs(lo))), 8, colori::GRIGIO_SCURO);
    double massimo_db = -1e9;
    for (const auto& r : v.righe) massimo_db = std::max(massimo_db, r.db_relativo);
    for (const auto& r : v.righe) {
        if (r.hz < F_MIN_BANDA || r.hz > F_MAX) continue;
        if (r.hz_fine > 0.0) {
            t.linea_orizzontale(std::max(sx, std::min(xs(r.hz), xs(r.hz_fine))), std::min(sx + sw - 1, std::max(xs(r.hz), xs(r.hz_fine))), sy + 4,
                                colore_riga(r.tipo));
            continue;
        }
        const int h = std::clamp(static_cast<int>(sh * (1.0 - (massimo_db - r.db_relativo) / 30.0)), 3, sh);
        t.linea_verticale(xs(r.hz), sy + sh - h, sy + sh - 1, colore_riga(r.tipo));
    }
    for (const double hz : {10.0, 50.0, 100.0, 200.0, 300.0, 400.0})
        f.scrivi(t, xs(hz), sy + sh + 3, std::to_string(static_cast<int>(hz)), colori::GRIGIO, 1);
}

}  // namespace m688
