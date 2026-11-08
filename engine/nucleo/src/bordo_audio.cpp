#include "nucleo/bordo_audio.hpp"

#include <algorithm>
#include <array>
#include <cmath>

namespace m688 {

namespace {

constexpr double AGGIORNAMENTO_BORDO_S = 0.25;   // la scena si rilegge 4 volte al secondo, come la cuffia
constexpr double RIVERBERO_DB = 4.0;             // il locale accumula la potenza dello scafo: 0-9 dB (STIMA, §6.2)
constexpr double F_MAX_BORDO = 8000.0;
constexpr double BANDA_CRITICA = 0.23;           // la banda critica come frazione della frequenza (terzo d'ottava)
constexpr double TICCHETTIO_DB = 10.0;           // il ping altrui arriva come ticchettio 10 dB sotto (STIMA, §6.1)
constexpr double BATTITO_BORDO = 0.5;            // il «rombo» delle eliche di chi passa sopra (navi o chi cava, STIMA)
constexpr double BATTITO_PORTATA_M = 3000.0;
constexpr double PI_B = 3.14159265358979323846;

double potenza_db(double db) { return std::pow(10.0, db / 10.0); }
double decibel(double p) { return 10.0 * std::log10(std::max(p, 1e-300)); }

// A-ponderazione (IEC 61672), dB
double ponderazione_a(double f) {
    const double f2 = f * f;
    const double ra = 12194.0 * 12194.0 * f2 * f2 /
                      ((f2 + 20.6 * 20.6) * std::sqrt((f2 + 107.7 * 107.7) * (f2 + 737.9 * 737.9)) * (f2 + 12194.0 * 12194.0));
    return 20.0 * std::log10(std::max(ra, 1e-30)) + 2.0;
}

// interpolazione lineare in log f su una tabella (f crescenti); fuori, il primo o l'ultimo valore piu' la pendenza
double tabella_log(double f, const double* fx, const double* v, std::size_t n, double pendenza_oltre_db_ott = 0.0) {
    if (f <= fx[0]) return v[0];
    if (f >= fx[n - 1]) return v[n - 1] + pendenza_oltre_db_ott * std::log2(f / fx[n - 1]);
    std::size_t i = 1;
    while (f > fx[i]) ++i;
    const double t = std::log(f / fx[i - 1]) / std::log(fx[i] / fx[i - 1]);
    return v[i - 1] + t * (v[i] - v[i - 1]);
}

bool aereo(const Sorgente& s) {
    return s.fonte && s.fonte->categoria == "asw" && (s.fonte->sottocategoria == "aereo" || s.fonte->sottocategoria == "elicottero");
}
bool fulmine(const Sorgente& s) { return s.fonte && s.fonte->categoria == "meteo" && s.fonte->sottocategoria == "fulmine"; }
bool evento_della_scena(const Sorgente& s) {
    if (fulmine(s)) return true;
    if (!s.fonte) return false;
    const SorgenteLibreria& f = *s.fonte;
    if (f.categoria == "asw") return f.sottocategoria == "ping" || f.sottocategoria == "tonfo";
    // R7: le teste dei siluri, i lanci e i colpi degli scafi; le esplosioni le porta scoppio() dai colpi della missione,
    // i transitori lunghi (allagamento, soffiata) passano dallo spettro come le sorgenti continue
    if (f.categoria == "arma" && f.sottocategoria == "ping") return true;
    return impulsivo(s) && f.sottocategoria != "esplosione";
}

// --- il maltempo (21e) ---
constexpr double SUPERFICIE_M = 3.0, SOTTO_M = 8.0;   // il ponte all'aria fino a 3 m, sotto 8 m del tutto sommerso (STIMA)
double in_superficie(double quota_m) { return std::clamp((SOTTO_M - quota_m) / (SOTTO_M - SUPERFICIE_M), 0.0, 1.0); }

// il tuono in aria, picco in dB SPL e passa basso secondo la distanza (21e §5.3, STIMA da [JOH13] e [RUSZ21])
double tuono_db(double km) {
    static const double d[] = {0.3, 1.0, 3.0, 5.0, 10.0, 15.0, 20.0};
    static const double l[] = {125, 116, 106, 102, 94, 88, 84};
    return tabella_log(km, d, l, 7, -6.0);
}
double tuono_taglio_hz(double km) {
    static const double d[] = {0.3, 1.0, 3.0, 5.0, 10.0, 15.0};
    static const double f[] = {4000, 2000, 800, 500, 250, 150};
    return tabella_log(km, d, f, 6);
}
constexpr double ISOLAMENTO_TUONO_DB = 45.0, TAGLIO_SCAFO_HZ = 200.0, TUONO_MAX_KM = 20.0;

// R3b, le macchine proprie (STIMA da [Cla92] e [Har71], dossier 18 §6.3): la riga piu' forte della firma a 84 dB in sala
// macchine, e sotto per stazione (centrale, sonar, siluri, macchine, alloggi, plancia)
constexpr double MACCHINE_RIGA_MAX_DB = 84.0;
constexpr double PROPRIO_SCARTO_DB[6] = {25.0, 28.0, 20.0, 0.0, 30.0, 40.0};
// i transitori del battello stesso: dal livello in acqua a 1 m a quello dentro per via aerea (STIMA)
constexpr double AEREO_INTERNO_DB = 60.0, VICINO_PROPRIO_M = 50.0;

// perdita interpolata su 7 frequenze (il costo: una scena con molte sorgenti e centinaia di bin)
struct PerditaNodi {
    std::array<double, 7> f{}, tl{};
    PerditaNodi(double r, double zs, double zr, const Ambiente& amb) {
        for (std::size_t k = 0; k < f.size(); ++k) {
            f[k] = 20.0 * std::pow(F_MAX_BORDO / 20.0, static_cast<double>(k) / 6.0);
            tl[k] = perdita_trasmissione(f[k], std::max(r, 1.0), zs, zr, amb).tl_db;
        }
    }
    double operator()(double x) const { return tabella_log(x, f.data(), tl.data(), f.size()); }
};

}  // namespace

const char* nome_stazione_bordo(StazioneBordo s) {
    switch (s) {
        case StazioneBordo::centrale: return "centrale";
        case StazioneBordo::sonar: return "sonar";
        case StazioneBordo::siluri: return "locale siluri";
        case StazioneBordo::macchine: return "sala macchine";
        case StazioneBordo::alloggi: return "alloggi";
        case StazioneBordo::plancia: return "plancia";
    }
    return "?";
}

double fondo_stazione_dba(StazioneBordo s, RegimeBordo r) {
    // silenzioso, normale, avanti tutta (STIMA: barca silenziosa 35-45 dBA, centrale 60-65 da progetto, macchine 87
    // misurati, alloggi sotto 70; dossier 18 §6.3)
    // plancia: vento e mare all'aperto, il riferimento IMO per le plance e' 65 dBA (STIMA)
    static const double t[6][3] = {{40, 62, 66}, {38, 58, 62}, {40, 60, 64}, {78, 87, 92}, {38, 60, 63}, {60, 62, 65}};
    return t[static_cast<int>(s)][static_cast<int>(r)];
}

double trasferimento_scafo_db(double f, StazioneBordo s) {
    if (s == StazioneBordo::plancia) return 200.0;   // all'aperto: dall'acqua non arriva niente
    // lastra d'acciaio di 4 cm a sola massa, dall'acqua all'aria (§6.2), meno il riverbero del locale; il locale siluri
    // sta contro lo scafo e i tubi, la sala sonar accanto al trasduttore (STIMA)
    static const double fx[] = {50, 250, 500, 1000, 2000, 4000, 8000};
    static const double k[] = {91.5, 91.5, 93, 96, 101, 107, 113};
    double scarto = 0.0;
    if (s == StazioneBordo::siluri) scarto = -5.0;
    else if (s == StazioneBordo::sonar) scarto = -3.0;
    else if (s == StazioneBordo::alloggi) scarto = 2.0;
    return tabella_log(f, fx, k, 7, 6.0) - RIVERBERO_DB + scarto;
}

double soglia_udibilita_db(double f) {
    // ISO 226:2003, soglia binaurale in campo libero (approssimata, da verificare)
    static const double fx[] = {20, 25, 31.5, 40, 50, 63, 80, 100, 125, 160, 200, 250, 315, 400, 500, 630, 800, 1000,
                                1250, 1600, 2000, 2500, 3150, 4000, 5000, 6300, 8000, 10000, 12500};
    static const double t[] = {78.5, 68.7, 59.5, 51.1, 44.0, 37.5, 31.5, 26.5, 22.1, 17.9, 14.4, 11.4, 8.6, 6.2, 4.4, 3.0,
                               2.2, 2.4, 3.5, 1.7, -1.3, -4.2, -6.0, -5.4, -1.5, 6.0, 12.6, 13.9, 12.3};
    return tabella_log(f, fx, t, sizeof fx / sizeof fx[0], 12.0);
}

AmbienteBordo::AmbienteBordo(const ParametriBordo& p) : p_(p), g_(p.campionamento_hz, p.seme) {}

void AmbienteBordo::azzera() {
    g_.azzera();
    resto_ = dal_aggiornamento_ = 0.0;
    pronto_ = false;
    campione_ = 0;
    eventi_.clear();
    attivi_.clear();
    scossone_ = 0.0;
    prossimo_seme_ = 1;
    prossima_onda_ = 0;
    onde_ = Casuale(0x0dde);
}

void AmbienteBordo::prepara(const std::vector<Sorgente>& sorgenti, Vec2 pos, double quota_m, const Ambiente& amb,
                            StazioneBordo st, RegimeBordo rg, const Sorgente* proprio) {
    pronto_ = true;
    stazione_ = st;
    regime_ = rg;
    quota_m_ = quota_m;
    const double fs = p_.campionamento_hz, df = g_.df_hz(), rif = p_.riferimento_db_spl;
    const double f_alto = std::min(F_MAX_BORDO, 0.49 * fs);
    // il fondo della stazione: 1/f sopra 50 Hz, piatto sotto, scalato al livello A del regime (STIMA)
    std::vector<double> fondo(g_.bin(), 0.0), esterno(g_.bin(), 0.0);
    double a_tot = 0.0;
    for (std::size_t j = 1; j < g_.bin(); ++j) {
        const double f = static_cast<double>(j) * df;
        if (f > f_alto) break;
        fondo[j] = 50.0 / std::max(f, 50.0);
        a_tot += fondo[j] * df * potenza_db(ponderazione_a(f));
    }
    // R3b: gli alloggi ovattati (le cuccette e le tende: il fondo piu' cupo, a parita' di dBA; STIMA)
    if (proprio && st == StazioneBordo::alloggi)
        for (std::size_t j = 1; j < g_.bin(); ++j) {
            const double f = static_cast<double>(j) * df;
            if (f > f_alto) break;
            a_tot -= fondo[j] * df * potenza_db(ponderazione_a(f));
            fondo[j] /= 1.0 + std::pow(f / 1500.0, 2.0);
            a_tot += fondo[j] * df * potenza_db(ponderazione_a(f));
        }
    const double la = fondo_stazione_dba(st, rg);
    const double k_fondo = potenza_db(la) / std::max(a_tot, 1e-30);   // in (20 uPa)^2 / Hz
    // in superficie col mare grosso l'acqua che corre sul ponte: un fruscio grave, 50 dBA a mare 3 e +4 per classe
    // (21e §5.4, STIMA), passa basso a 150 Hz
    std::vector<double> acqua(g_.bin(), 0.0);
    if (meteo_ && meteo_->stato_mare() >= 3 && in_superficie(quota_m) > 0.0) {
        double a_acqua = 0.0;
        for (std::size_t j = 1; j < g_.bin(); ++j) {
            const double f = static_cast<double>(j) * df;
            if (f > f_alto) break;
            acqua[j] = 1.0 / (1.0 + std::pow(f / 150.0, 2.0));
            a_acqua += acqua[j] * df * potenza_db(ponderazione_a(f));
        }
        const double livello = 50.0 + 4.0 * (meteo_->stato_mare() - 3) + 10.0 * std::log10(in_superficie(quota_m));
        const double k = potenza_db(livello) / std::max(a_acqua, 1e-30);
        for (double& x : acqua) x *= k;
    }
    // le sorgenti allo scafo
    std::vector<GeneratoreSpettrale::RigaSintesi> righe;
    double battito_hz = 0.0, battito_livello = -1e9;
    for (const Sorgente& s : sorgenti) {
        if (aereo(s) || (!s.firma && !s.fonte)) continue;   // gli aerei non si sentono a bordo (scelta dell'utente)
        if (s.fonte && s.fonte->categoria == "arma" && s.fonte->sottocategoria == "esplosione") continue;   // scoppio()
        const double r = distanza(pos, s.pos);
        if (evento_della_scena(s)) {   // ping e tonfi: al loro inizio un evento a campione esatto
            const bool attivo = livello_totale_fonte(s) > 0.0;
            bool& prima = attivi_[s.id];
            if (attivo && !prima) {
                const bool ping = s.fonte->sottocategoria == "ping", lampo = fulmine(s);
                const double f_rif = ping ? 2000.0 : lampo ? 150.0 : 500.0;
                const double esterno_db = s.fonte->livello_db + s.guadagno_db -
                                          perdita_trasmissione(ping ? 8000.0 : lampo ? 150.0 : 1000.0, std::max(r, 1.0), s.quota_m, quota_m, amb).tl_db;
                double interno = esterno_db - trasferimento_scafo_db(f_rif, st) - (ping ? TICCHETTIO_DB : 0.0);
                // R3b: i colpi del battello stesso (voci «scafo» a due passi) arrivano dentro per via aerea, forti
                const bool nostro = proprio && r < VICINO_PROPRIO_M && s.fonte->categoria == "scafo";
                if (nostro) interno = s.fonte->livello_db + s.guadagno_db - AEREO_INTERNO_DB - (st == StazioneBordo::plancia ? 20.0 : 0.0);
                const bool metallico = nostro && (s.fonte->id == "portello_sbatte" || s.fonte->id == "attrezzo_cade");
                if (interno >= soglia_udibilita_db(f_rif)) {
                    Evento e;
                    e.tipo = ping || metallico ? Evento::Tipo::ticchettio : Evento::Tipo::tonfo;
                    e.inizio = campione_;
                    e.ampiezza = std::sqrt(2.0) * std::pow(10.0, (interno - rif) / 20.0);
                    e.durata_s = ping ? 0.08 : 0.4;
                    e.caso = Casuale(p_.seme ^ (0x9e3779b97f4a7c15ull * prossimo_seme_++));
                    eventi_.push_back(e);
                }
            }
            prima = attivo;
            continue;
        }
        const PerditaNodi tl(r, s.quota_m, quota_m, amb);
        const std::vector<double> livelli = s.fonte ? livelli_componenti(s) : std::vector<double>{};
        const double banda_larga = s.firma ? livello_banda_larga(s) : 0.0;
        for (std::size_t j = 1; j < g_.bin(); ++j) {
            const double f = static_cast<double>(j) * df;
            if (f > f_alto) break;
            const double ls = s.firma ? livello_sorgente_da(f, s, banda_larga) + aspetto_db(s, pos, f) : livello_fonte_da(s, f, false, livelli);
            esterno[j] += potenza_db(ls - tl(f));
        }
        // le righe, col Doppler della sola sorgente (il battello che ascolta e' lo scafo, quasi fermo rispetto al suono)
        const double rr = r > 1.0 ? radianti(rilevamento(s.pos, pos)) : 0.0;
        const double vs = s.velocita_kn * NODO * (std::sin(radianti(s.rotta)) * std::sin(rr) + std::cos(radianti(s.rotta)) * std::cos(rr));
        const double doppler = 1.0 + vs / 1500.0;
        std::vector<std::pair<double, double>> lr;
        if (s.firma)
            for (const RigaEmessa& re : righe_emesse(s)) lr.emplace_back(re.hz, re.db + aspetto_db(s, pos, re.hz));
        else
            lr = righe_fonte(s);
        for (const auto& [hz0, db] : lr) {
            const double hz = hz0 * doppler;
            if (hz < 20.0 || hz > f_alto) continue;
            const double interno = db - tl(hz) - trasferimento_scafo_db(hz, st);
            if (interno < soglia_udibilita_db(hz)) continue;
            righe.push_back({{s.id, std::lround(hz0 * 1000.0)}, hz, std::sqrt(2.0) * std::pow(10.0, (interno - rif) / 20.0)});
        }
        // il rombo delle eliche: la pala di chi cava vicino
        if (s.firma && r < BATTITO_PORTATA_M && (s.nave || cavita(*s.firma, s.velocita_kn, s.quota_m))) {
            const double livello = livello_banda_larga(s) - tl(100.0);
            if (livello > battito_livello && frequenza_pala_hz(s) > 0.0) {
                battito_livello = livello;
                battito_hz = frequenza_pala_hz(s);
            }
        }
    }
    // dentro lo scafo, sopra la soglia d'udibilita' (per banda critica), piu' il fondo: unita' d'uscita
    std::vector<double> densita(g_.bin(), 0.0);
    for (std::size_t j = 1; j < g_.bin(); ++j) {
        const double f = static_cast<double>(j) * df;
        if (f > f_alto) break;
        double interno = esterno[j] > 0.0 ? decibel(esterno[j]) - trasferimento_scafo_db(f, st) : -300.0;
        if (interno + 10.0 * std::log10(BANDA_CRITICA * f) < soglia_udibilita_db(f)) interno = -300.0;
        densita[j] = (potenza_db(interno) + fondo[j] * k_fondo + acqua[j]) * potenza_db(-rif);
    }
    // il ronzio dei macchinari a 60 Hz e armoniche (STIMA: 15 dB sotto il fondo in sala macchine, 22 altrove)
    const double ronzio = la - (st == StazioneBordo::macchine ? 15.0 : 22.0);
    for (int k = 1; k <= 3; ++k)
        righe.push_back({{-1, 60000L * k}, 60.0 * k, std::sqrt(2.0) * std::pow(10.0, (ronzio - 3.0 * (k - 1) - rif) / 20.0)});
    if (proprio) {
        const auto riga = [&](long chiave, double hz, double db) {
            if (hz < 20.0 || hz > f_alto || db < soglia_udibilita_db(hz)) return;
            righe.push_back({{-2, chiave}, hz, std::sqrt(2.0) * std::pow(10.0, (db - rif) / 20.0)});
        };
        // la ventilazione (centrale, alloggi, radio) e le ventole dell'elettronica (sonar): STIMA
        if (st == StazioneBordo::centrale || st == StazioneBordo::alloggi) { riga(120, 120.0, la - 18.0); riga(240, 240.0, la - 24.0); }
        if (st == StazioneBordo::sonar) { riga(400, 400.0, la - 20.0); riga(800, 800.0, la - 26.0); }
        // le macchine: le righe della firma propria come le emette ora (regime, individuo, danni: R6), alla stazione
        if (proprio->firma) {
            const double scarto = PROPRIO_SCARTO_DB[static_cast<int>(st)];
            const double rif_tonale = proprio->firma->tonale_db - MACCHINE_RIGA_MAX_DB;
            for (const RigaEmessa& re : righe_emesse(*proprio))
                if (re.tipo != TipoRiga::elica) riga(std::lround(re.hz * 100.0), re.hz, re.db - rif_tonale - scarto);
            // il fischio della turbina: i giri dell'asse per il riduttore e le palette (STIMA: 800 volte l'asse),
            // 75 dB in sala macchine a 10 nodi e +20 log v
            if (const double fa = frequenza_asse_hz(*proprio); fa > 0.0)
                riga(1, fa * 800.0, 75.0 + 20.0 * std::log10(std::max(proprio->velocita_kn, 1.0) / 10.0) - scarto);
        }
    }
    g_.imposta_rumore(densita, 1.0);
    g_.imposta_righe(righe);
    g_.imposta_battito(battito_hz, battito_hz > 0.0 ? BATTITO_BORDO : 0.0);
}

void AmbienteBordo::scoppio(double distanza_m, double tnt_kg, double quota_m) {
    // picco dell'onda d'urto (Cole: P = 52,4 MPa (W^1/3 / R)^1,13), periodo della bolla (Rayleigh-Willis:
    // T = 2,11 W^1/3 / (d + 10,1)^5/6); dentro lo scafo meno K alle basse frequenze (§6.2)
    const double w3 = std::cbrt(std::max(tnt_kg, 1e-3)), r = std::max(distanza_m, 5.0);
    const double picco_db = 20.0 * std::log10(52.4e6 * std::pow(w3 / r, 1.13) / 1e-6);
    const double interno = picco_db - trasferimento_scafo_db(100.0, stazione_);
    Evento e;
    e.tipo = Evento::Tipo::scoppio;
    e.inizio = campione_;
    e.ampiezza = std::pow(10.0, (interno - p_.riferimento_db_spl) / 20.0);
    e.bolla_s = 2.11 * w3 / std::pow(std::max(quota_m, 0.0) + 10.1, 5.0 / 6.0);
    e.durata_s = 3.0;
    e.caso = Casuale(p_.seme ^ (0x9e3779b97f4a7c15ull * prossimo_seme_++));
    eventi_.push_back(e);
    scossone_ = std::max(scossone_, std::clamp((interno - 120.0) / 40.0, 0.0, 1.0));
}

void AmbienteBordo::meteo(const StatoMeteo& s) {
    if (!meteo_) prossima_onda_ = campione_;
    meteo_ = s;
}

void AmbienteBordo::tuono(double distanza_m, bool nube_mare, double t_aria_c) {
    const double km = std::max(distanza_m, 100.0) / 1000.0;
    if (km > TUONO_MAX_KM) return;
    const double fs = p_.campionamento_hz;
    const double c = 331.3 + 0.606 * t_aria_c;
    Evento e;
    e.tipo = Evento::Tipo::tuono;
    e.inizio = campione_ + static_cast<std::uint64_t>(std::llround(distanza_m / c * fs));
    e.caso = Casuale(p_.seme ^ (0x9e3779b97f4a7c15ull * prossimo_seme_++));
    // durata: la geometria del canale piu' i rami nella nube, fra 2 e 35 s ([BOD08]); i rimbombi secondari ogni ~2 s
    e.durata_s = std::clamp(3.0 + 1.2 * km + e.caso.uniforme(0.0, 8.0) + (nube_mare ? 0.0 : 5.0), 2.0, 35.0);
    for (double t = e.caso.uniforme(0.5, 2.5); t < e.durata_s; t += e.caso.uniforme(1.0, 3.0))
        e.colpi.emplace_back(t, e.caso.uniforme(0.3, 1.0));
    e.vicino = km < 3.0;
    const bool aperto = stazione_ == StazioneBordo::plancia;
    e.taglio_hz = aperto ? tuono_taglio_hz(km) : std::min(TAGLIO_SCAFO_HZ, tuono_taglio_hz(km));
    // il rumore filtrato ha meno potenza del bianco: la sua ampiezza si riporta all'unita' con lo stesso filtro
    Casuale prova(7);
    double f1 = 0.0, f2 = 0.0, q = 0.0;
    const double a = 1.0 - std::exp(-2.0 * PI_B * e.taglio_hz / fs);
    for (int i = 0; i < 8192; ++i) {
        f1 += a * ((2.0 * prova.uniforme() - 1.0) - f1);
        f2 += a * (f1 - f2);
        if (i >= 1024) q += f2 * f2;
    }
    e.hz = 1.0 / std::sqrt(std::max(q / 7168.0, 1e-30));
    const double interno = tuono_db(km) - (nube_mare ? 0.0 : 10.0) - (aperto ? 0.0 : ISOLAMENTO_TUONO_DB);
    e.ampiezza = std::pow(10.0, (interno - 6.0 - p_.riferimento_db_spl) / 20.0);   // il picco ~6 dB sopra l'rms
    eventi_.push_back(e);
}

void AmbienteBordo::artiglieria(const EventoArtiglieria& e, Vec2 pos, double quota_m, double t_aria_c) {
    using T = TipoEventoArtiglieria;
    const double fs = p_.campionamento_hz, rif = p_.riferimento_db_spl;
    const double r = std::max(distanza(pos, e.pos), 1.0), km = r / 1000.0;
    const double c_aria = 331.3 + 0.606 * t_aria_c;
    const bool aperto = stazione_ == StazioneBordo::plancia;
    const bool sopra = in_superficie(quota_m) > 0.0;
    const auto ampiezza = [&](double db) { return std::pow(10.0, (db - rif) / 20.0); };
    const auto nuovo = [&](Evento::Tipo tipo, double ritardo_s, double db, double durata_s) {
        Evento x;
        x.tipo = tipo;
        x.inizio = campione_ + static_cast<std::uint64_t>(std::llround(std::max(ritardo_s, 0.0) * fs));
        x.ampiezza = ampiezza(db);
        x.durata_s = durata_s;
        x.caso = Casuale(p_.seme ^ (0x9e3779b97f4a7c15ull * prossimo_seme_++));
        return x;
    };
    // il boato in aria (partenza, scoppio): come un tuono breve; in plancia pieno, dentro 45 dB sotto e cupo
    const auto boato = [&](double db_10m, double durata_s) {
        if (!sopra) return;
        double l = db_10m - 20.0 * std::log10(std::max(r, 10.0) / 10.0) - 0.5 * km;   // sferico e l'aria (STIMA)
        if (!aperto) l -= ISOLAMENTO_TUONO_DB;
        Evento x = nuovo(Evento::Tipo::tuono, r / c_aria, l - 6.0, durata_s);
        x.vicino = true;
        x.taglio_hz = std::clamp(4000.0 * std::sqrt(1000.0 / r), 150.0, 4000.0);
        if (!aperto) x.taglio_hz = std::min(x.taglio_hz, TAGLIO_SCAFO_HZ);
        Casuale prova(7);   // il rumore filtrato riportato a rms 1, come nel tuono
        double f1 = 0.0, f2 = 0.0, q = 0.0;
        const double a = 1.0 - std::exp(-2.0 * PI_B * x.taglio_hz / fs);
        for (int i = 0; i < 8192; ++i) {
            f1 += a * ((2.0 * prova.uniforme() - 1.0) - f1);
            f2 += a * (f1 - f2);
            if (i >= 1024) q += f2 * f2;
        }
        x.hz = 1.0 / std::sqrt(std::max(q / 7168.0, 1e-30));
        eventi_.push_back(x);
    };
    // lo schiocco sullo scafo dall'acqua (immerso, o in superficie la parte di scafo ancora sotto)
    const auto schiocco = [&](double picco_esterno_db) {
        if (aperto) return;
        const double interno = picco_esterno_db - trasferimento_scafo_db(2000.0, stazione_);
        if (interno < soglia_udibilita_db(2000.0) + 10.0) return;
        Evento x = nuovo(Evento::Tipo::ticchettio, 0.0, interno, 0.08);
        eventi_.push_back(x);
    };
    const double w = std::max(e.carica_kg, 0.0);
    const double calibro = 30.0 * std::log10(std::max(e.calibro_mm, 5.0) / 127.0);   // massa ~ calibro^3 (STIMA)
    const double quota_acqua = sopra ? 5.0 : quota_m;   // in superficie lo scafo e' ancora sotto per ~5 m
    const double colonna_m = 25.0 * std::max(e.calibro_mm, 20.0) / 127.0;            // 22 §6.1 (STIMA)
    const auto spruzzi = [&]() {   // la colonna che ricade entro ~100 m
        if (!sopra || r > 100.0) return;
        const double l = 100.0 - 20.0 * std::log10(std::max(r, 10.0) / 10.0) - (aperto ? 0.0 : 20.0);
        eventi_.push_back(nuovo(Evento::Tipo::spruzzi, std::sqrt(2.0 * colonna_m / 9.81), l, 3.0));
    };
    switch (e.tipo) {
        case T::partenza: boato(177.0 + calibro, 1.2); break;   // 22 §7.4: 110-120 dB a 5 km per un 5"
        case T::illuminante: boato(140.0, 0.5); break;            // la carica d'espulsione: un colpo sordo (STIMA)
        case T::scoppio_superficie:
        case T::scoppio_aria: {
            const double aria = 185.0 + 6.7 * std::log10(std::max(w, 1e-3) / 3.5);   // STIMA: 185 dB a 10 m per 3,5 kg
            boato(aria, 0.6);
            if (r < 3000.0) schiocco(picco_scoppio_db(std::max(w, 1e-3), r, quota_acqua) - (e.tipo == T::scoppio_aria ? 20.0 : 0.0));
            spruzzi();
            break;
        }
        case T::tonfo:
        case T::rimbalzo:
            if (r < 1000.0) schiocco(picco_scoppio_db(3.5, r, quota_acqua) - 40.0 + calibro);
            spruzzi();
            break;
        case T::raffica: {
            const double durata = std::min(std::max(1, e.colpi) * std::max(e.intervallo_s, 0.001), 5.0);
            const double kg = w > 0.0 ? w : 0.05;
            Evento x;
            if (sopra) {
                double l = 185.0 + 6.7 * std::log10(kg / 3.5) - 20.0 * std::log10(std::max(r, 10.0) / 10.0) - 0.5 * km;
                if (!aperto) l -= ISOLAMENTO_TUONO_DB;
                x = nuovo(Evento::Tipo::raffica, r / c_aria, l, durata);
                x.taglio_hz = aperto ? 4000.0 : TAGLIO_SCAFO_HZ;
            } else {
                if (r > 3000.0) break;
                const double interno = picco_scoppio_db(kg, r, quota_m) - trasferimento_scafo_db(2000.0, stazione_);
                if (interno < soglia_udibilita_db(2000.0) + 10.0) break;
                x = nuovo(Evento::Tipo::raffica, 0.0, interno, durata);
                x.taglio_hz = 4000.0;
            }
            x.intervallo_s = std::max(e.intervallo_s, 0.001);
            eventi_.push_back(x);
            break;
        }
        case T::colpo_testa: {   // 22 §7.4: «si sente certamente in centrale» (STIMA: ~95 dB dentro, il clangore in plancia)
            Evento x = nuovo(Evento::Tipo::urto_testa, 0.0, aperto ? 125.0 : 95.0, 4.0);
            eventi_.push_back(x);
            break;
        }
        case T::passaggio: {
            if (!sopra || r > 500.0) break;
            const double l = 150.0 - 15.0 * std::log10(std::max(r, 10.0) / 10.0) - (aperto ? 0.0 : ISOLAMENTO_TUONO_DB);   // STIMA
            Evento x = nuovo(Evento::Tipo::passaggio, r / c_aria, l, 0.8);
            x.vicino = e.velocita_m_s > c_aria;   // supersonico: l'onda a N
            eventi_.push_back(x);
            break;
        }
    }
}

void AmbienteBordo::urto(const EsitoUrto& e, const DanniUrtoUnita* proprio, double distanza_m) {
    const double stridio_acqua = livello_stridio_db(e.energia_tangente_mj, e.durata_strisciamento_s);
    double colpo = 0.0, stridio = 0.0;
    if (proprio) {
        colpo = std::clamp(115.0 + 10.0 * std::log10(std::max(proprio->energia_mj + proprio->strisciamento_mj, 1e-3)), 90.0, 135.0);
        stridio = std::clamp(stridio_acqua - 60.0, 70.0, 120.0);
        scossone_ = std::max(scossone_, std::clamp(proprio->scossa_g / 0.5, 0.0, 1.0));
    } else {
        if (stazione_ == StazioneBordo::plancia) return;   // dall'acqua: in plancia non arriva
        const double perdita = 20.0 * std::log10(std::max(distanza_m, 10.0));
        colpo = e.livello_sorgente_db - perdita - trasferimento_scafo_db(200.0, stazione_);
        stridio = stridio_acqua - perdita - trasferimento_scafo_db(1000.0, stazione_);
    }
    const auto nuovo = [&](Evento::Tipo tipo, double db, double durata_s) {
        Evento x;
        x.tipo = tipo;
        x.inizio = campione_;
        x.ampiezza = std::pow(10.0, (db - p_.riferimento_db_spl) / 20.0);
        x.durata_s = durata_s;
        x.caso = Casuale(p_.seme ^ (0x9e3779b97f4a7c15ull * prossimo_seme_++));
        return x;
    };
    if (colpo >= soglia_udibilita_db(200.0) + 10.0) {
        Evento x = nuovo(Evento::Tipo::urto, colpo, 3.0);
        x.vicino = parte_squillante(e.a.parte) || parte_squillante(e.b.parte);
        eventi_.push_back(x);
    }
    if (e.durata_strisciamento_s > 0.0 && e.energia_tangente_mj > 0.0 && stridio >= soglia_udibilita_db(1000.0) + 10.0)
        eventi_.push_back(nuovo(Evento::Tipo::stridio, stridio, e.durata_strisciamento_s));
}

void AmbienteBordo::colpo_di_mare(std::uint64_t k) {
    // un colpo di mare sullo scafo al periodo d'onda (21e §5.4, STIMA): in superficie 75-85 dB col mare 4-8, piu' la
    // campana dello scafo; sotto, come il moto orbitale exp(-k z), coi tonfi e gli scricchiolii a quota periscopica
    const StatoMeteo& m = *meteo_;
    const double fs = p_.campionamento_hz;
    const double tp = std::max(m.hs_swell_m > m.hs_vento_m ? m.tp_swell_s : m.tp_vento_s, 2.0);
    prossima_onda_ = k + static_cast<std::uint64_t>(tp * onde_.uniforme(0.6, 1.4) * fs);
    const int ss = m.stato_mare();
    if (onde_.uniforme() >= std::clamp((ss - 3) / 4.0, 0.0, 1.0)) return;
    const double lunghezza = 9.81 * tp * tp / (2.0 * PI_B);
    const double attenuazione = -8.686 * 2.0 * PI_B * std::max(quota_m_, 0.0) / lunghezza;
    const double livello = 75.0 + 2.5 * std::min(ss - 4, 4) + attenuazione;
    if (livello < fondo_stazione_dba(stazione_, RegimeBordo::silenzioso) - 10.0) return;
    Evento e;
    e.tipo = Evento::Tipo::onda;
    e.inizio = k;
    e.ampiezza = std::pow(10.0, (livello - p_.riferimento_db_spl) / 20.0);
    e.durata_s = 1.5;
    e.vicino = in_superficie(quota_m_) > 0.5;
    e.hz = onde_.uniforme(150.0, 300.0);   // la campana dello scafo
    e.caso = Casuale(p_.seme ^ (0x9e3779b97f4a7c15ull * prossimo_seme_++));
    eventi_.push_back(e);
}

void AmbienteBordo::ping_proprio(double sl_db, double hz, double durata_s) {
    // il proprio sonar dentro lo scafo: 93-108 dB nei locali prodieri per 220-230 dB di sorgente (Tigrone, §6.1)
    double scarto = 128.0;
    if (stazione_ == StazioneBordo::sonar || stazione_ == StazioneBordo::siluri) scarto = 120.0;
    else if (stazione_ == StazioneBordo::alloggi) scarto = 133.0;
    else if (stazione_ == StazioneBordo::macchine) scarto = 138.0;
    Evento e;
    e.tipo = Evento::Tipo::ping;
    e.inizio = campione_;
    e.ampiezza = std::sqrt(2.0) * std::pow(10.0, (sl_db - scarto - p_.riferimento_db_spl) / 20.0);
    e.hz = std::min(hz, 0.45 * p_.campionamento_hz);
    e.durata_s = durata_s;
    e.caso = Casuale(p_.seme ^ (0x9e3779b97f4a7c15ull * prossimo_seme_++));
    eventi_.push_back(e);
}

double AmbienteBordo::campione_evento(Evento& e, std::uint64_t k) {
    const double fs = p_.campionamento_hz;
    const double t = static_cast<double>(k - e.inizio) / fs;
    const double bianco = 2.0 * e.caso.uniforme() - 1.0;
    const auto passa_basso = [&](double& stato, double x, double fc) {
        stato += (1.0 - std::exp(-2.0 * PI_B * fc / fs)) * (x - stato);
        return stato;
    };
    switch (e.tipo) {
        case Evento::Tipo::ticchettio: {   // colpetto metallico: rumore breve e la risonanza dello scafo
            const double inviluppo = std::exp(-t / 0.008);
            return e.ampiezza * (0.7 * bianco * inviluppo + 0.6 * std::sin(2.0 * PI_B * 2200.0 * t) * std::exp(-t / 0.03));
        }
        case Evento::Tipo::tonfo:
            return e.ampiezza * 2.0 * passa_basso(e.filtro, bianco, 400.0) * std::exp(-t / 0.08);
        case Evento::Tipo::ping: {
            const double attacco = std::min(1.0, t / 0.002);
            const double coda = t > e.durata_s ? std::exp(-(t - e.durata_s) / 0.05) : 1.0;
            return e.ampiezza * attacco * coda * (std::sin(2.0 * PI_B * e.hz * t) + 0.1 * std::sin(2.0 * PI_B * 1800.0 * t));
        }
        case Evento::Tipo::scoppio: {   // urto, bolla e rombo, tutto passa basso (lo scafo)
            const double grave = passa_basso(e.filtro, bianco, 300.0);
            const double cupo = passa_basso(e.filtro2, bianco, 80.0);
            double v = 2.5 * grave * std::exp(-t / 0.03);
            for (const auto& [quando, quanto] : {std::pair{1.0, 0.35}, std::pair{1.8, 0.15}}) {
                const double tb = t - quando * e.bolla_s;
                if (tb > 0.0) v += quanto * 2.5 * grave * std::exp(-tb / 0.02);
            }
            v += 0.4 * 3.0 * cupo * std::exp(-t / 0.8);
            return e.ampiezza * v;
        }
        case Evento::Tipo::tuono: {   // rimbombo: rumore passa basso a inviluppo lento coi rimbombi, lo schianto vicino
            const double a = 1.0 - std::exp(-2.0 * PI_B * e.taglio_hz / fs);
            e.filtro += a * (bianco - e.filtro);
            e.filtro2 += a * (e.filtro - e.filtro2);
            e.filtro3 += (1.0 - std::exp(-2.0 * PI_B * 25.0 / fs)) * (e.filtro2 - e.filtro3);   // via il sotto 25 Hz
            double inviluppo = std::min(1.0, t / 0.05) * std::exp(-t / (0.35 * e.durata_s));
            for (const auto& [quando, quanto] : e.colpi) inviluppo += quanto * std::exp(-std::pow((t - quando) / 0.3, 2.0));
            if (e.vicino) inviluppo += 2.0 * std::exp(-t / 0.4);
            return e.ampiezza * e.hz * (e.filtro2 - e.filtro3) * inviluppo * in_superficie(quota_m_) *
                   (t < e.durata_s ? 1.0 : std::exp(-(t - e.durata_s) / 0.3));
        }
        case Evento::Tipo::onda: {    // tonfo grave 20-60 Hz, poi la campana (in superficie) o lo scricchiolio (sotto)
            const double tonfo = passa_basso(e.filtro, bianco, 60.0);
            double v = 6.0 * tonfo * std::min(1.0, t / 0.03) * std::exp(-t / 0.5);
            if (e.vicino) v += 0.3 * std::sin(2.0 * PI_B * e.hz * t) * std::exp(-t / 1.0);
            else v += 0.8 * (passa_basso(e.filtro2, bianco, 600.0) - passa_basso(e.filtro3, bianco, 200.0)) * std::exp(-t / 0.4);
            return e.ampiezza * v;
        }
        case Evento::Tipo::urto_testa: {   // la testa del periscopio colpita: clangore del tubo, poi l'acqua che entra
            double v = 0.0;
            for (const auto& [hz, quanto] : {std::pair{730.0, 1.0}, std::pair{1310.0, 0.7}, std::pair{2390.0, 0.45}})
                v += quanto * std::sin(2.0 * PI_B * hz * t) * std::exp(-t / 0.35);
            v += 1.5 * bianco * std::exp(-t / 0.004);
            const double gorgoglio = passa_basso(e.filtro, bianco, 500.0) * (t > 0.3 ? std::min(1.0, (t - 0.3) / 0.3) : 0.0) *
                                     std::exp(-std::max(t - 0.6, 0.0) / 2.5) * (0.6 + 0.4 * std::sin(2.0 * PI_B * 7.0 * t));
            return e.ampiezza * (v + 2.0 * gorgoglio);
        }
        case Evento::Tipo::passaggio: {    // onda a N dello schiocco supersonico (10 ms), poi il sibilo del proietto
            double v = 0.0;
            if (e.vicino && t < 0.01) v += 1.5 * (1.0 - 2.0 * t / 0.01);
            const double fruscio = passa_basso(e.filtro, bianco, 4000.0) - passa_basso(e.filtro2, bianco, 1000.0);
            v += 0.3 * 3.0 * fruscio * std::min(1.0, t / 0.05) * std::exp(-t / 0.25);
            return e.ampiezza * v;
        }
        case Evento::Tipo::spruzzi: {      // l'acqua della colonna che ricade: rumore acuto che sale e si spegne
            const double fruscio = bianco - passa_basso(e.filtro, bianco, 1000.0);
            return e.ampiezza * 1.5 * fruscio * std::min(1.0, t / 0.2) * std::exp(-t / 0.8);
        }
        case Evento::Tipo::urto: {         // lo schianto, lo scafo che rimbomba, la lamiera che squilla se c'e' una lama
            double v = 3.0 * passa_basso(e.filtro, bianco, 250.0) * std::exp(-t / 0.06);
            for (const auto& [hz, quanto] : {std::pair{38.0, 0.8}, std::pair{71.0, 0.5}, std::pair{124.0, 0.3}})
                v += quanto * std::sin(2.0 * PI_B * hz * t) * std::min(1.0, t / 0.01) * std::exp(-t / 0.9);
            if (e.vicino)
                for (const auto& [hz, quanto] : {std::pair{640.0, 0.5}, std::pair{1170.0, 0.35}, std::pair{2110.0, 0.2}})
                    v += quanto * std::sin(2.0 * PI_B * hz * t) * std::exp(-t / 0.4);
            return e.ampiezza * v;
        }
        case Evento::Tipo::stridio: {      // lamiera su lamiera: rumore a 400-2500 Hz che s'impunta e riparte
            const double banda = passa_basso(e.filtro, bianco, 2500.0) - passa_basso(e.filtro2, bianco, 400.0);
            e.filtro3 += (1.0 - std::exp(-2.0 * PI_B * 3.0 / fs)) * (bianco - e.filtro3);
            const double impunta = 0.6 + 0.4 * std::sin(2.0 * PI_B * 11.0 * t + 40.0 * e.filtro3);
            const double inviluppo = std::min(1.0, t / 0.1) * (t < e.durata_s ? 1.0 : std::exp(-(t - e.durata_s) / 0.2));
            return e.ampiezza * 3.0 * banda * impunta * inviluppo;
        }
        case Evento::Tipo::raffica: {      // colpi brevi a intervallo fisso (AK-630): crepitio o ronzio
            const double tau = e.intervallo_s > 0.0 ? std::fmod(t, e.intervallo_s) : t;
            const double colpo = (bianco + 0.5 * std::sin(2.0 * PI_B * 1800.0 * tau)) * std::exp(-tau / 0.002);
            return t < e.durata_s ? e.ampiezza * passa_basso(e.filtro, colpo, e.taglio_hz) * 2.0 : 0.0;
        }
    }
    return 0.0;
}

std::vector<float> AmbienteBordo::passo(const std::vector<Sorgente>& sorgenti, Vec2 pos, double quota_m, const Ambiente& amb,
                                        StazioneBordo stazione, RegimeBordo regime, double dt_s, const Sorgente* proprio) {
    const double fs = p_.campionamento_hz;
    if (!pronto_ || stazione != stazione_ || dal_aggiornamento_ >= AGGIORNAMENTO_BORDO_S * fs) {
        prepara(sorgenti, pos, quota_m, amb, stazione, regime, proprio);
        dal_aggiornamento_ = 0.0;
    }
    const double esatti = std::max(0.0, dt_s) * fs + resto_;
    const std::size_t n = static_cast<std::size_t>(std::floor(esatti));
    resto_ = esatti - static_cast<double>(n);
    dal_aggiornamento_ += static_cast<double>(n);
    std::vector<float> out = g_.genera(n);
    for (std::size_t i = 0; i < n; ++i) {
        const std::uint64_t k = campione_ + i;
        if (meteo_ && k >= prossima_onda_) colpo_di_mare(k);
        double v = 0.0;
        for (Evento& e : eventi_)
            if (k >= e.inizio) v += campione_evento(e, k);
        out[i] = static_cast<float>(std::tanh(static_cast<double>(out[i]) + v));   // saturazione morbida: gli scoppi
    }
    campione_ += n;
    eventi_.erase(std::remove_if(eventi_.begin(), eventi_.end(), [&](const Evento& e) {
        return campione_ > e.inizio && static_cast<double>(campione_ - e.inizio) / fs > e.durata_s + 0.5;
    }), eventi_.end());
    scossone_ *= std::exp(-dt_s / 0.5);
    return out;
}

}  // namespace m688
