#include "video_apertura.hpp"

#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <map>
#include <stdexcept>
#include <utility>

#include <toml.hpp>

#include "interfaccia.hpp"
#include "missioni/missione.hpp"
#include "partita.hpp"
#include "regia_audio.hpp"
#include "registro_tempi.hpp"

namespace m688 {

namespace {

constexpr double PASSO_S = 1.0 / 9.0;   // la cadenza delle missioni (missione.cpp)
constexpr int CENTRO = LARGHEZZA_TELA / 2;
constexpr float VOLUME_PING = 0.4f;     // lontanissimo: picco attorno a -18 dB

// Il ping lontanissimo e il suo eco: un tono a 3,5 kHz che si spegne, sintetizzato qui, e lo stesso piu' basso e piu' cupo
// dopo 1,8 s. Solo audio, nel nero della prima inquadratura
std::vector<float> ping_lontano(int hz) {
    std::vector<float> x(static_cast<std::size_t>(hz * 4.5), 0.0f);
    const auto tono = [&](double da, double f, double forza, double spegni) {
        for (std::size_t i = static_cast<std::size_t>(da * hz); i < x.size(); ++i) {
            const double t = static_cast<double>(i) / hz - da;
            const double attacco = std::min(1.0, t / 0.012);
            x[i] += static_cast<float>(forza * attacco * std::exp(-t * spegni) * std::sin(2.0 * M_PI * f * t));
        }
    };
    tono(0.0, 3500.0, 0.35, 5.0);
    tono(1.8, 3480.0, 0.07, 3.5);
    tono(2.9, 3470.0, 0.02, 3.0);
    return x;
}

// Il ritaglio al centro del nero: fuori restano le scritte d'aiuto delle stazioni, il film ha solo gli strumenti (docs/29 §1)
void ritaglia(Tela& t, const std::array<int, 4>& r, double scala) {
    const auto [rx, ry, rl, ra] = r;
    if (rl <= 0 || ra <= 0) return;
    const Tela prima = t;
    t.riempi(0);
    const int l = static_cast<int>(rl * scala), a = static_cast<int>(ra * scala);
    const int x0 = (LARGHEZZA_TELA - l) / 2, y0 = (ALTEZZA_TELA - a) / 2;
    for (int y = std::max(y0, 0); y < std::min(y0 + a, ALTEZZA_TELA); ++y) {
        std::uint8_t* riga = t.riga(y);
        const int sy = ry + static_cast<int>((y - y0) / scala);
        for (int x = std::max(x0, 0); x < std::min(x0 + l, LARGHEZZA_TELA); ++x) riga[x] = prima.at(rx + static_cast<int>((x - x0) / scala), sy);
    }
}

// Il sole basso nella palette del film: la rampa del cielo dal colore del sole all'orizzonte al viola, gia' tutta nei primi
// indici perche' i 4 gradi del campo stretto ne usano solo quelli (periscopio.cpp, indice_fondo); il mare lontano ne prende
// un riflesso, le nubi alte il colore. La luce della vista resta quella di luce_ambiente; niente disco del sole: il periscopio non lo disegna
void tinge_tramonto(Palette& pal, double forza) {
    constexpr int INDICI_DEL_CAMPO = 9;
    const auto mescola = [&](int i, double r, double g, double b, double f) {
        const auto c = pal.rgb(i);
        const auto m = [&](std::uint8_t a, double d) { return static_cast<std::uint8_t>(std::lround(a + std::clamp(f * forza, 0.0, 1.0) * (d - a))); };
        pal.imposta(i, m(c[0], r), m(c[1], g), m(c[2], b));
    };
    for (int i = colori::CIELO_ORIZZONTE; i <= colori::CIELO_ALTO; ++i) {
        const double u = std::min(1.0, static_cast<double>(i - colori::CIELO_ORIZZONTE) / INDICI_DEL_CAMPO);
        mescola(i, 214.0 - 104.0 * u, 118.0 - 48.0 * u, 74.0 + 34.0 * u, 1.0);
    }
    for (int i = 155; i <= 158; ++i) mescola(i, 236.0, 150.0, 118.0, 0.85);   // i grigi chiari delle nubi, accesi dal sole; le navi stanno sotto il 155
    for (int i = colori::MARE_VICINO; i <= colori::MARE_ORIZZONTE; ++i) {
        const double u = static_cast<double>(i - colori::MARE_VICINO) / (colori::MARE_ORIZZONTE - colori::MARE_VICINO);
        mescola(i, 120.0, 72.0, 70.0, 0.55 * u * u);
    }
}

}  // namespace

double RegiaApertura::durata_s() const {
    double b = 0.0;
    for (const auto& q : inquadrature) b += q.battute;
    return b * 4.0 * 60.0 / bpm;
}

RegiaApertura carica_regia_apertura(const std::string& percorso) {
    toml::table t;
    try {
        t = toml::parse_file(percorso);
    } catch (const toml::parse_error& e) {
        throw std::runtime_error(percorso + ": " + std::string(e.description()));
    }
    RegiaApertura r;
    r.bpm = t["bpm"].value_or(72.0);
    r.preriscaldo_s = t["preriscaldo_s"].value_or(30.0);
    r.seme = static_cast<std::uint64_t>(t["seme"].value_or(1));
    r.brano = t["brano"].value_or(std::string{});
    r.latitudine = t["latitudine"].value_or(0.0);
    r.longitudine = t["longitudine"].value_or(0.0);
    r.inizio = DataOra{t["anno"].value_or(1989), t["mese"].value_or(6), t["giorno"].value_or(12), t["ora_utc"].value_or(12.0)};
    if (const auto* a = t["inquadratura"].as_array())
        for (const auto& v : *a) {
            const auto* x = v.as_table();
            if (!x) continue;
            InquadraturaApertura q;
            q.battute = (*x)["battute"].value_or(2.0);
            q.battello = (*x)["battello"].value_or(std::string{});
            q.stazione = (*x)["stazione"].value_or(8);
            q.compressione = (*x)["compressione"].value_or(0.0);
            q.pannello = (*x)["pannello"].value_or(false);
            q.colonna_688 = (*x)["colonna_688"].value_or(0);
            q.colonna_alfa = (*x)["colonna_alfa"].value_or(0);
            q.guarda = (*x)["guarda"].value_or(-1);
            if (const auto* c = (*x)["cifre"].as_array())
                for (const auto& k : *c) q.cifre.push_back(k.value_or(0));
            q.ping = (*x)["ping"].value_or(false);
            q.effetto = (*x)["effetto"].value_or(std::string{});
            q.volume_effetto = (*x)["volume_effetto"].value_or(q.volume_effetto);
            q.cuffia = (*x)["cuffia"].value_or(q.cuffia);
            q.bordo = (*x)["bordo"].value_or(q.bordo);
            if (const auto* c = (*x)["ritaglio"].as_array(); c && c->size() == 4)
                for (std::size_t i = 0; i < 4; ++i) q.ritaglio[i] = (*c)[i].value_or(0);
            q.scala = (*x)["scala"].value_or(q.scala);
            q.luce = (*x)["luce"].value_or(q.luce);
            q.entra_s = (*x)["entra_s"].value_or(q.entra_s);
            q.tramonto = (*x)["tramonto"].value_or(q.tramonto);
            q.titolo = (*x)["titolo"].value_or(false);
            if (!q.battello.empty() && q.battello != "688" && q.battello != "alfa" && q.battello != "doppia")
                throw std::runtime_error(percorso + ": battello sconosciuto " + q.battello);
            r.inquadrature.push_back(q);
        }
    if (const auto* a = t["ordine"].as_array())
        for (const auto& v : *a) {
            const auto* x = v.as_table();
            if (!x) continue;
            const std::string nome = (*x)["tipo"].value_or(std::string{});
            const auto tipo = ordine_da_nome(nome);
            if (!tipo) throw std::runtime_error(percorso + ": ordine sconosciuto " + nome);
            OrdineApertura o;
            const std::string chi = (*x)["battello"].value_or(std::string{});
            if (chi != "688" && chi != "alfa") throw std::runtime_error(percorso + ": l'ordine " + nome + " non dice a chi va");
            o.alfa = chi == "alfa";
            o.ordine.tempo = (*x)["t"].value_or(0.0);
            o.ordine.tipo = *tipo;
            o.ordine.a = (*x)["a"].value_or(0.0);
            o.ordine.b = (*x)["b"].value_or(0.0);
            o.ordine.n = (*x)["n"].value_or(0);
            r.ordini.push_back(o);
        }
    std::stable_sort(r.ordini.begin(), r.ordini.end(), [](const auto& a, const auto& b) { return a.ordine.tempo < b.ordine.tempo; });
    if (r.inquadrature.empty()) throw std::runtime_error(percorso + ": nessuna inquadratura");
    return r;
}

Palette affianca(Tela& uscita, const Tela& a, const Palette& pal_a, int colonna_a, const Tela& b, const Palette& pal_b, int colonna_b) {
    std::array<bool, 256> usati_a{}, usati_b{};
    for (int y = 0; y < ALTEZZA_TELA; ++y)
        for (int x = 0; x < CENTRO; ++x) {
            usati_a[a.at(colonna_a + x, y)] = true;
            usati_b[b.at(colonna_b + x, y)] = true;
        }
    Palette pal = pal_a;
    std::array<std::uint8_t, 256> mappa{};
    std::array<bool, 256> presi = usati_a;
    for (int i = 0; i < 256; ++i) presi[i] = presi[i] || usati_b[i];
    for (int i = 0; i < 256; ++i) {
        mappa[i] = static_cast<std::uint8_t>(i);
        if (!usati_b[i]) continue;
        const auto c = pal_b.rgb(i);
        if (!usati_a[i]) {   // lo usa solo b: prende il colore di b al suo posto
            pal.imposta(i, c[0], c[1], c[2]);
            continue;
        }
        if (pal_a.rgb(i) == c) continue;
        int dove = -1;
        for (int k = 0; k < 256 && dove < 0; ++k)
            if (!presi[k]) dove = k;
        if (dove >= 0) {
            presi[dove] = true;
            pal.imposta(dove, c[0], c[1], c[2]);
            mappa[i] = static_cast<std::uint8_t>(dove);
            continue;
        }
        long meglio = -1;   // indici finiti: il colore piu' vicino fra quelli di a
        for (int k = 0; k < 256; ++k) {
            if (!usati_a[k]) continue;
            const auto d = pal.rgb(k);
            const long e = (long{c[0]} - d[0]) * (long{c[0]} - d[0]) + (long{c[1]} - d[1]) * (long{c[1]} - d[1]) +
                           (long{c[2]} - d[2]) * (long{c[2]} - d[2]);
            if (meglio < 0 || e < meglio) meglio = e, mappa[i] = static_cast<std::uint8_t>(k);
        }
    }
    for (int y = 0; y < ALTEZZA_TELA; ++y) {
        std::uint8_t* riga = uscita.riga(y);
        for (int x = 0; x < CENTRO; ++x) {
            riga[x] = a.at(colonna_a + x, y);
            riga[CENTRO + x] = mappa[b.at(colonna_b + x, y)];
        }
    }
    return pal;
}

struct VideoApertura::Battello {
    Effetti effetti{1};
    std::unique_ptr<Partita> partita;
    std::unique_ptr<InterfacciaMissione> ui;
    Periscopio periscopio;
    VistaPeriscopio vista;
    StatoPannello pannello;
    std::vector<Ordine> ordini;       // ancora da dare
    std::size_t prossimo = 0;
    bool attivo = false;

    Battello(const Risorse& r, const std::string& dati, const RegiaApertura& regia, const std::string& missione, bool alfa)
        : periscopio(r.font, r.classi, r.sprite) {
        periscopio.usa_modelli(r.modelli, rampa_grigi(r.giorno));
        periscopio.usa_palette(r.giorno);
        partita = std::make_unique<Partita>(r, ImpostazioniPartita{dati, dati + "/apertura", missione, regia.seme, RegolaDanni::simulazione, "fondale", regia.inizio, {}, {}},
                                            effetti);
        Missione& m = partita->missione();
        m.usa_meteo(Missione::ModoMeteo::semplificato);
        m.usa_navigazione(false);
        m.luogo_del_sole(regia.latitudine, regia.longitudine);
        m.usa_traffico(false);
        m.meteo_enhanced(dati);
        ui = std::make_unique<InterfacciaMissione>(m, partita->stazioni(), partita->posto(), partita->ordini(), partita->equipaggio(), r.classi, 8);
        partita->osserva = [this](const Ordine& o) { ui->osserva(o); };
        partita->dopo_passo = [this]() { ui->dopo_passo(); };
        partita->dati_app = [this]() { return ui->dati_app(); };
        partita->effetti_per_fotogramma = true;
        ui->imposta_enhanced(true);
        ui->imposta_consolle(dati);
        ui->sonar().imposta_modo(ModoSonar::moderno);
        ui->sonar().imposta_missione(missione);
        ui->carta().ora_inizio_s = regia.inizio.ore_utc * 3600.0;
        ui->carta().urss = alfa;
        ui->carta().livelli.meteo = false;
        ui->carta().livelli.mobilia = false;
        ui->imposta_navi(&partita->danni());
        ui->imposta_catalogo_carico([this] { return partita->catalogo_carico(); });
        std::map<std::string, double> alberi;
        for (const auto& [id, mod] : r.modelli) alberi[id] = mod.quota_max_m();
        ui->imposta_alberi(std::move(alberi));
        ui->imposta_luci(dati);
        if (const LibreriaCampioni* c = m.campioni()) ui->sonar().imposta_campioni(c);
        for (const auto& o : regia.ordini)
            if (o.alfa == alfa) ordini.push_back(o.ordine);
    }

    double tempo() const { return partita->missione().tempo(); }

    void un_passo(const RegiaApertura& regia) {
        const double t = tempo();
        for (; prossimo < ordini.size() && ordini[prossimo].tempo <= t + PASSO_S; ++prossimo) partita->ordini().ordina(ordini[prossimo]);
        DataOra ora = regia.inizio;
        ora.ore_utc = std::fmod(regia.inizio.ore_utc + t / 3600.0, 24.0);
        partita->imposta_luce(luce_ambiente(elevazione_sole(ora, regia.latitudine, regia.longitudine)));
        partita->avanza(PASSO_S);
    }
};

VideoApertura::VideoApertura(const Risorse& r, const std::string& dati, RegiaApertura regia, Componi componi)
    : r_(r), regia_(std::move(regia)), componi_(std::move(componi)), ultima_(r.giorno) {
    usa_ = std::make_unique<Battello>(r, dati, regia_, "APERTURA_688", false);
    urss_ = std::make_unique<Battello>(r, dati, regia_, "APERTURA_ALFA", true);
    obiettivo_ = regia_.preriscaldo_s;
    try {
        font_titolo_ = Font::carica(dati + "/font/fosforo688_7x9.fnt");
    } catch (const std::exception&) {   // il titolo col font del gioco
    }
}

VideoApertura::~VideoApertura() { imposta_audio(nullptr); }

void VideoApertura::imposta_audio(RegiaAudio* a) {
    if (audio_) {
        audio_->mixer().svuota();
        ParametriMixer p = audio_->mixer().parametri();
        p.volume = volumi_prima_;
        audio_->mixer().imposta(p);
    }
    audio_ = a;
    if (a) volumi_prima_ = a->mixer().parametri().volume;
    Mixer* m = a ? &a->mixer() : nullptr;
    usa_->ui->imposta_mixer(m);
    urss_->ui->imposta_mixer(m);
}

double VideoApertura::tempo_missione(bool alfa) const { return (alfa ? urss_ : usa_)->tempo(); }

const Missione& VideoApertura::missione(bool alfa) const { return (alfa ? urss_ : usa_)->partita->missione(); }

void VideoApertura::entra_in_inquadratura() {
    const InquadraturaApertura& q = regia_.inquadrature[static_cast<std::size_t>(corrente_)];
    if (audio_) {
        const bool nero = q.battello.empty() && !q.titolo;
        if (q.ping) audio_->suono(ping_lontano(48000), 48000, VOLUME_PING);
        if (!q.effetto.empty()) audio_->effetto(q.effetto, static_cast<float>(q.volume_effetto));
        if (q.battello.empty() || q.titolo) audio_->mixer().svuota();   // il nero e' silenzio: niente cuffia che resta in coda
        ParametriMixer p = audio_->mixer().parametri();
        p.volume[static_cast<std::size_t>(Canale::cuffia)] = static_cast<float>(q.cuffia);
        p.volume[static_cast<std::size_t>(Canale::bordo)] = static_cast<float>(q.bordo);
        // nero: nemmeno le code delle note, tace di colpo
        p.volume[static_cast<std::size_t>(Canale::musica)] = nero ? 0.0f : volumi_prima_[static_cast<std::size_t>(Canale::musica)];
        audio_->mixer().imposta(p);
    }
    for (Battello* b : {usa_.get(), urss_.get()}) {
        b->attivo = (q.battello == "688" && b == usa_.get()) || (q.battello == "alfa" && b == urss_.get()) || q.battello == "doppia";
        if (!b->attivo) continue;
        Comandi c;
        c.stazione = q.stazione;
        b->ui->comandi(c, b->vista, 0.0);
        Comandi p;
        p.cifre = q.cifre;
        if (!p.cifre.empty()) b->ui->comandi(p, b->vista, 0.0);
    }
}

void VideoApertura::comanda(Battello& b, double dt) {
    const InquadraturaApertura& q = regia_.inquadrature[static_cast<std::size_t>(corrente_)];
    b.ui->comandi(Comandi{}, b.vista, dt);
    if (q.guarda >= 0) {
        const Nave* p = b.partita->mondo().trova(b.partita->id_proprio());
        const Nave* n = b.partita->mondo().trova(q.guarda);
        if (p && n) b.vista.rilevamento = normalizza_360(rilevamento(p->pos, n->pos));
    }
}

bool VideoApertura::avanza(double dt, double limite_s) {
    dt_ = dt;
    tempo_ += dt;
    double fine = 0.0;
    int qui = -1;
    for (std::size_t i = 0; i < regia_.inquadrature.size(); ++i) {
        fine += durata_battute(regia_.inquadrature[i].battute);
        if (tempo_ < fine) {
            qui = static_cast<int>(i);
            break;
        }
    }
    if (qui < 0) return false;
    if (qui != corrente_) {
        corrente_ = qui;
        inizio_inquadratura_ = fine - durata_battute(regia_.inquadrature[static_cast<std::size_t>(qui)].battute);
        entra_in_inquadratura();
    }
    obiettivo_ += dt * regia_.inquadrature[static_cast<std::size_t>(qui)].compressione;
    // un passo a testa, il piu' indietro per primo, finche' c'e' tempo: lo schermo non si ferma per aspettarle
    using Orologio = std::chrono::steady_clock;
    const auto parte = Orologio::now();
    for (;;) {
        Battello* b = usa_->tempo() <= urss_->tempo() ? usa_.get() : urss_.get();
        if (b->tempo() + PASSO_S > obiettivo_ + 1e-9) break;
        {
            CronometroPasso cp;
            b->un_passo(regia_);
        }
        if (limite_s > 0.0 && std::chrono::duration<double>(Orologio::now() - parte).count() > limite_s) break;
    }
    if (qui > 0) ritardo_max_ = std::max(ritardo_max_, obiettivo_ - std::min(usa_->tempo(), urss_->tempo()));
    for (Battello* b : {usa_.get(), urss_.get()}) b->partita->anima_effetti(std::clamp(dt, 0.0, 0.25) * regia_.inquadrature[static_cast<std::size_t>(qui)].compressione);
    return true;
}

Palette VideoApertura::disegna_battello(Battello& b, Tela& t, const InquadraturaApertura& q) {
    comanda(b, dt_);
    Missione& m = b.partita->missione();
    b.pannello.compressione = 1.0;
    b.pannello.missione.clear();   // nessuna parola, nessun nome: docs/29 §1
    b.pannello.messaggio.clear();
    b.pannello.due_righe = true;
    if (const auto* g = m.giocatore()) {   // il pannello di Manovra, come in missione
        StatoManovra sm;
        const PostoDiComando& posto = b.partita->posto();
        sm.telegrafo = posto.telegrafo(), sm.timone = posto.timone(), sm.barra = posto.barra();
        sm.pilota = posto.pilota_automatico(), sm.punti = static_cast<int>(m.punti_di_rotta().size());
        sm.periscopio = (g->flag54 & 2) != 0, sm.antenna = (g->flag54 & 4) != 0;
        sm.radar = m.radar_giocatore_acceso();
        sm.rotta = g->rotta, sm.velocita_kn = g->velocita, sm.quota_m = g->quota, sm.quota_ordinata_m = g->quota_ordinata;
        sm.cambia_quota = g->rateo_quota > 0.0 && std::fabs(g->quota_ordinata - g->quota) > 0.3;
        b.ui->manovra_enhanced(sm);
        b.pannello.manovra = sm;
    }
    b.ui->pannello(b.pannello);
    b.ui->messaggio_barra({});
    const double alfa = std::clamp((obiettivo_ - m.tempo()) / PASSO_S, 0.0, 1.0);
    b.ui->imposta_tempo_disegno(m.tempo() + alfa * PASSO_S);
    b.ui->prepara_vista(b.vista);
    b.vista.costa.attiva = false;   // la scena e' in mare aperto: 9 ms a fotogramma per non trovare terra
    const Mondo vista = b.partita->mondo().interpolato(alfa);
    Palette pal = componi_(t, b.periscopio, vista, b.vista, b.pannello, b.partita->id_proprio(), m.tempo(), &b.effetti, q.stazione == 5);
    b.ui->disegna(t, r_.font, b.vista);
    b.ui->palette(pal);
    if (q.tramonto > 0.0) tinge_tramonto(pal, q.tramonto);
    if (!q.pannello) t.rettangolo_pieno(0, ALTEZZA_VISTA, LARGHEZZA_TELA, ALTEZZA_TELA - ALTEZZA_VISTA, 0);
    if (audio_ && (q.battello != "doppia" || &b == usa_.get())) {   // nella doppia, una cuffia sola
        Cronometro cr(&RegistroTempi::Riga::cuffia);
        b.ui->audio(dt_, std::max(1.0, q.compressione));
    }
    return pal;
}

Palette VideoApertura::disegna(Tela& t) {
    Palette nero = r_.giorno;
    if (corrente_ < 0) {
        t.riempi(0);
        return nero;
    }
    const InquadraturaApertura& q = regia_.inquadrature[static_cast<std::size_t>(corrente_)];
    if (q.titolo) {
        t.riempi(0);
        // il titolo del gioco e sotto quello del remake, nel font del gioco: esce dal nero nella prima meta' dell'inquadratura
        const std::string gioco = "688 ATTACK SUB", remake = "ENHANCED";
        const Font& f = font_titolo_ ? *font_titolo_ : r_.font;
        const int sg = 4, sr = 2;
        f.scrivi(t, CENTRO - f.larghezza(gioco, sg) / 2, 180, gioco, colori::BIANCO, sg);
        f.scrivi(t, CENTRO - f.larghezza(remake, sr) / 2, 240, remake, colori::CIANO_CHIARO, sr);
        const double esce = (tempo_ - inizio_inquadratura_) / (durata_battute(q.battute) * 0.5);
        return r_.giorno.scurita(std::clamp(1.0 - esce, 0.0, 1.0), {{0, 255}});
    }
    if (q.battello.empty()) {
        t.riempi(0);
        return nero;
    }
    const Palette pal = disegna_strumenti(t, q);
    const double entra = q.entra_s > 0.0 ? std::clamp((tempo_ - inizio_inquadratura_) / q.entra_s, 0.0, 1.0) : 1.0;
    const double luce = q.luce * entra;
    return luce < 1.0 ? pal.scurita(1.0 - luce, {{0, 255}}) : pal;
}

Palette VideoApertura::disegna_strumenti(Tela& t, const InquadraturaApertura& q) {
    if (q.stazione == 6 && tempo_ < prossima_carta_ && tempo_ >= inizio_inquadratura_ + 0.1) {   // la tela e' ancora quella, il suono no
        if (audio_) (q.battello == "alfa" ? urss_ : usa_)->ui->audio(dt_, std::max(1.0, q.compressione));
        return ultima_;
    }
    prossima_carta_ = tempo_ + 0.125;
    if (q.battello == "doppia") {
        const Palette pa = disegna_battello(*usa_, t, q);
        const Tela prima = t;
        const Palette pb = disegna_battello(*urss_, altra_, q);
        ultima_ = affianca(t, prima, pa, q.colonna_688, altra_, pb, q.colonna_alfa);
    } else {
        ultima_ = disegna_battello(q.battello == "alfa" ? *urss_ : *usa_, t, q);
    }
    ritaglia(t, q.ritaglio, q.scala);
    return ultima_;
}

CodaFilm::CodaFilm(std::size_t campioni_a_fotogramma, std::size_t massimo, std::size_t avvio)
    : per_fotogramma_(campioni_a_fotogramma), massimo_(massimo), avvio_(avvio) {}

bool CodaFilm::metti(const Tela& t, const Palette& p, int inquadratura, const std::vector<float>& suono) {
    std::unique_lock blocco(mutex_);
    spazio_.wait(blocco, [&] { return fermata_ || fotogrammi_.size() < massimo_; });
    if (fermata_) return false;
    fotogrammi_.push_back({t, p, inquadratura, messi_++});
    suono_.insert(suono_.end(), suono.begin(), suono.end());
    if (fotogrammi_.size() >= avvio_) partita_ = true;
    return true;
}

void CodaFilm::finita() {
    std::lock_guard blocco(mutex_);
    finita_ = partita_ = true;
}

std::vector<float> CodaFilm::campioni(std::size_t n) {
    std::vector<float> x(n, 0.0f);
    std::lock_guard blocco(mutex_);
    if (!partita_) return x;
    const std::size_t m = std::min(n, suono_.size());
    std::copy_n(suono_.begin(), m, x.begin());
    suono_.erase(suono_.begin(), suono_.begin() + static_cast<std::ptrdiff_t>(m));
    dati_ += m;
    if (!finita_) silenzio_ += n - m;
    return x;
}

std::uint64_t CodaFilm::dati() const {
    std::lock_guard blocco(mutex_);
    return dati_;
}

std::optional<CodaFilm::Fotogramma> CodaFilm::prendi(std::uint64_t fino_a) {
    std::optional<Fotogramma> f;
    {
        std::lock_guard blocco(mutex_);
        if (!partita_) return f;
        while (!fotogrammi_.empty() && fotogrammi_.front().numero * per_fotogramma_ <= fino_a) {
            f = std::move(fotogrammi_.front());
            fotogrammi_.pop_front();
        }
    }
    if (f) spazio_.notify_one();
    return f;
}

void CodaFilm::ferma() {
    {
        std::lock_guard blocco(mutex_);
        fermata_ = true;
    }
    spazio_.notify_all();
}

bool CodaFilm::partita() const {
    std::lock_guard blocco(mutex_);
    return partita_;
}

bool CodaFilm::conclusa() const {
    std::lock_guard blocco(mutex_);
    return finita_ && fotogrammi_.empty() && suono_.empty();
}

std::size_t CodaFilm::anticipo() const {
    std::lock_guard blocco(mutex_);
    return fotogrammi_.size();
}

std::uint64_t CodaFilm::silenzio() const {
    std::lock_guard blocco(mutex_);
    return silenzio_;
}

}  // namespace m688
