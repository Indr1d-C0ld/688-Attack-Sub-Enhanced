#include "nucleo/acustica.hpp"

#include "nucleo/analisi_acustica.hpp"
#include "nucleo/meteo.hpp"
#include "nucleo/navi_civili.hpp"

#include <algorithm>
#include <cmath>
#include <functional>
#include <limits>
#include <stdexcept>

#define TOML_EXCEPTIONS 1
#include <toml.hpp>

namespace m688 {

namespace {

constexpr double C_RIFERIMENTO = 1500.0;        // m/s (dossier 02 §10.7)
constexpr double GRADIENTE_PRESSIONE = 0.0163;  // s^-1, strato isotermo (§3.1)
constexpr double SLD_MAX_M = 400.0;             // il layer si cerca nei primi 400 m (§3.3)
constexpr double SLD_MIN_M = 10.0;
constexpr double GHIACCIO_RIMBALZO_DB = 0.5;    // un rimbalzo sotto la volta: l'energia che entra nel ghiaccio (STIMA, 06 §5.1)              // sotto, nessun layer (gradiente negativo dalla superficie)
constexpr double ECCESSO_CZ_M = 200.0;          // eccesso di profondita' minimo per le CZ (§3.3)
constexpr double MICROSTRATO_MAX_M = 30.0;      // effetto pomeriggio: il microtermoclino sta nei primi 5-15 m
constexpr double LIVELLO_MUTO_DB = -300.0;        // una sorgente muta (solo eco): nessun livello udibile
// le navi civili: la banda dei loro punti e' misurata (McKenna 2012, Veirs 2016), ma lo spettro qui sotto integrato su
// 5-200 Hz le da' 17 dB in piu'; levati, torna al valore di banda (tools/re/prove/portata_sonar/proposta_punto1.md). Le
// navi da guerra e i sottomarini no: le loro portate sono tarate su questo spettro
constexpr double SPETTRO_CIVILI_SOTTO_DB = 17.0;
constexpr double MICROSTRATO_PORTATA = 5.0;    // vale per chi sta entro 5 volte lo spessore (sorgenti superficiali)
constexpr double MICROSTRATO_DB_PER_MS = 2.0;   // perdita del condotto per m/s di salto (circa 9 dB al massimo, STIMA)
constexpr double CANALE_CZ_M = 800.0;           // fra vertice e profondita' critica: sotto e' un semicanale (asse in
                                                // superficie, alte latitudini), non una CZ (STIMA)
constexpr double ACQUA_BASSA_M = 200.0;         // sotto: Marsh-Schulkin (§3.4 i)
constexpr double KYD = 914.4;
constexpr double PIEDE = 0.3048;

// STIME DI GIOCO (dossier 02 §3.4, §10.6: parametri da tarare)
constexpr double CZ_GUADAGNO_DB = 8.0;          // 5-10 dB sulla divergenza sferica
constexpr double CZ_PERDITA_ZONA_DB = 4.0;      // ogni CZ successiva 3-5 dB in meno
constexpr double CZ_SEMILARGHEZZA = 0.05;       // frazione della distanza della CZ (zone larghe 3-5 nm)
constexpr double ANGOLO_CRITICO_SABBIA = 30.0, ANGOLO_CRITICO_ROCCIA = 60.0;   // gradi di radenza (STIMA, c2/c1 1,15 e 2)
constexpr double OMBRA_MAX_DB = 25.0;           // zona d'ombra: 10-30 dB peggio della sferica
constexpr double FONDO_ANGOLO_MIN_GRADI = 10.0; // rimbalzo sul fondo: raggi a 10-40 gradi
constexpr double CAVITAZIONE_SALTO_DB = 10.0;   // all'insorgenza la larga banda sale di 10-20 dB in pochi nodi
constexpr double CAVITAZIONE_PENDENZA = 40.0;   // dB/decade oltre la pendenza della tabella, sopra l'insorgenza
constexpr double SORDO_DB = 30.0;               // sensore oltre la sua velocita' massima
constexpr double DOPPLER_RIVERBERAZIONE_KN = 2.5;
constexpr double DOPPLER_GUADAGNO_DB = 10.0;    // un CW separa dalla riverberazione un bersaglio in moto radiale
constexpr double SV_VOLUME_DB = -80.0;          // forza di diffusione di volume (-90..-70)
constexpr double SB_FONDO_DB = -27.0;           // fondo, legge di Lambert (Mackenzie 1961)
constexpr double POSATO_DB = -10.0;             // battello posato in assetto silenzioso (STIMA, docs/11 §6)
constexpr double CIECO_DB = 30.0;               // schiera in virata o sorgente nell'endfire anteriore
constexpr double SFREGAMENTO_5KN_DB = 95.0;     // la schiera che striscia sul fondo (blocco 1 del sonar, D; STIMA)
constexpr double SCHIERA_DANNEGGIATA_DB = 6.0, SCHIERA_DANNEGGIATA_HZ = 300.0;   // dossier 04 §6.3 (STIMA)
constexpr double RADDRIZZAMENTO_DB = 10.0;      // schiera appena uscita dalla virata, a calare
constexpr double SCALA_TESTA_DB = 18.0;         // banda larga 5-200 Hz -> scala dei siluri (112,5 + 18 = 130 dB)
constexpr double ERRORE_MIN_DEG = 0.2;
// riga di pala (dossier 02 §2.2-2.3, Ross 1976): 60 log V attorno a 10 kn, dove sta 10 dB sopra il continuo dello
// spettro a bassa frequenza (Ross: 10-20 dB sopra il continuo; STIMA a meta')
constexpr double PALA_RIFERIMENTO_KN = 10.0;
constexpr double PALA_SU_CONTINUO_DB = 15.0;
constexpr double PALA_PENDENZA_DB = 60.0;
constexpr double PALA_CAVITAZIONE_DB = 10.0;
// riverberazione di superficie (Chapman e Harris 1962, dossier 02 §1.3) e di fondo (Lambert, mu = -27 dB)
constexpr double SS_MASSIMA_DB = -20.0;         // tetto della forza di diffusione della superficie (STIMA)
constexpr double QUOTA_MIN_RIVERBERAZIONE_M = 3.0;   // trasduttori di scafo alla quota zero delle navi: la chiglia
constexpr double LOBO_VERTICALE_MAX_DB = 30.0;  // per verso, al piu': i lobi laterali delle schiere (STIMA)
// sotto la volta (dossier 06 §5.1, STIMA): la diffusione all'indietro della volta rugosa, -30 dB a 10 gradi radenti per un
// metro di rugosita' (l'ordine di grandezza delle misure sotto la banchisa, Brown e Milne 1967: -40/-30 dB a 1-10 kHz), che
// scende col seno dell'angolo, e molto sopra il mare calmo di Chapman-Harris; il rumore del pack deformato (le creste che si
// spingono: 72 dB attorno ai 100 Hz, 15-20 dB sopra il pack quieto, Milne e Ganton) e delle fessure termiche (+18 dB sopra il
// ghiaccio attorno ai 2 kHz)
constexpr double SS_VOLTA_DB = -30.0, SS_VOLTA_RADENTE_DEG = 10.0, SS_VOLTA_MASSIMA_DB = -15.0;
constexpr double CRESTE_LIVELLO_DB = 72.0, CRESTE_KHZ = 0.1, CRESTE_LARGHEZZA = 0.6, CRESTE_CALO_DB = 12.0;
constexpr double FESSURE_DB = 18.0, FESSURE_KHZ = 2.0, FESSURE_LARGHEZZA = 0.4;

const ProfiloSuono& isotermo() {
    static const ProfiloSuono p = [] {
        ProfiloSuono q;
        q.z_m = {0.0, 6000.0};
        q.c_m_s = {C_RIFERIMENTO, C_RIFERIMENTO + GRADIENTE_PRESSIONE * 6000.0};
        q.analizza();
        return q;
    }();
    return p;
}

double db_somma(double a, double b) { return 10.0 * std::log10(std::pow(10.0, a / 10.0) + std::pow(10.0, b / 10.0)); }
double smoothstep(double a, double b, double x) {
    const double t = std::clamp((x - a) / (b - a), 0.0, 1.0);
    return t * t * (3.0 - 2.0 * t);
}

// frequenza di taglio del condotto superficiale di spessore h (Urick: lambda_max = 8,5e-3 H^1,5 in piedi)
double taglio_condotto_hz(double h_m, double c) {
    if (h_m <= 0.0) return 1e12;
    const double lambda_ft = 8.5e-3 * std::pow(h_m / PIEDE, 1.5);
    return c / (lambda_ft * PIEDE);
}


// distanza orizzontale percorsa da un raggio fra la quota del vertice e la quota z, in un gradiente uniforme g
double arco(double dz, double g, double c) {
    if (dz <= 0.0) return 0.0;
    if (std::fabs(g) < 1e-6) return 1e9;
    return std::sqrt(2.0 * (c / std::fabs(g)) * dz);
}

}  // namespace

double perdita_fondo_db(TipoFondo t, double f_hz, double angolo_gradi) {
    // 2-6 dB per sabbia o roccia, 8-15 per fango (1-4 kHz), crescente con frequenza e angolo (STIMA). Sabbia e roccia
    // sono piu' veloci dell'acqua: sotto l'angolo critico (Rayleigh: arccos c1/c2, circa 30 gradi per la sabbia, 60
    // per la roccia) la riflessione e' quasi totale e resta la sola perdita nel sedimento (0,5 e 0,2 dB), che cresce
    // col quadrato dell'angolo fino al valore di sopra. Il fango (c2 circa c1) non ha angolo critico
    const double base = t == TipoFondo::roccia ? 3.0 : t == TipoFondo::sabbia ? 5.0 : 11.0;
    const double freq = 1.0 + 0.25 * std::log10(std::max(f_hz, 100.0) / 1000.0);
    const double angolo = 1.0 + std::max(0.0, angolo_gradi - 20.0) / 40.0;
    const double sopra = base * std::max(0.5, freq) * angolo;
    if (t == TipoFondo::fango) return sopra;
    const double critico = t == TipoFondo::roccia ? ANGOLO_CRITICO_ROCCIA : ANGOLO_CRITICO_SABBIA;
    if (angolo_gradi >= critico) return sopra;
    // il minimo sotto l'angolo critico: la sola perdita nel sedimento in basso, poi rugosita' e strati (STIMA, Urick 5.x
    // e APL-UW: sabbia circa 0,5 dB a 250 Hz e 1,5-2 dB a 1 kHz; roccia 0,3 e 1); fra 250 Hz e 1 kHz in logaritmo,
    // oltre come la perdita di sopra
    const double basso = t == TipoFondo::roccia ? 0.3 : 0.5, alto = t == TipoFondo::roccia ? 1.0 : 1.8;
    const double x = std::clamp(std::log2(std::max(f_hz, 1.0) / 250.0) / 2.0, 0.0, 1.0);
    const double minimo = std::min(sopra, f_hz > 1000.0 ? alto * std::max(0.5, freq) : basso + (alto - basso) * x);
    const double k = angolo_gradi / critico;
    return minimo + (sopra - minimo) * k * k;
}

// --- formule di base ---

double velocita_suono_mackenzie(double t, double s, double d) {
    return 1448.96 + 4.591 * t - 5.304e-2 * t * t + 2.374e-4 * t * t * t + 1.340 * (s - 35.0) + 1.630e-2 * d +
           1.675e-7 * d * d - 1.025e-2 * t * (s - 35.0) - 7.139e-13 * t * d * d * d;
}

double assorbimento_db_km(double f_hz, double t, double s, double z_m, double ph) {
    const double f = f_hz / 1000.0, f2 = f * f, z = z_m / 1000.0;
    const double f1 = 0.78 * std::sqrt(std::max(s, 0.0) / 35.0) * std::exp(t / 26.0);
    const double fm = 42.0 * std::exp(t / 17.0);
    return 0.106 * (f1 * f2 / (f1 * f1 + f2)) * std::exp((ph - 8.0) / 0.56) +
           0.52 * (1.0 + t / 43.0) * (s / 35.0) * (fm * f2 / (fm * fm + f2)) * std::exp(-z / 6.0) +
           0.00049 * f2 * std::exp(-(t / 27.0 + z / 17.0));
}

double assorbimento_thorp_db_km(double f_hz) {
    const double f2 = (f_hz / 1000.0) * (f_hz / 1000.0);
    return 0.11 * f2 / (1.0 + f2) + 44.0 * f2 / (4100.0 + f2) + 2.75e-4 * f2 + 0.003;
}

int stato_del_mare(double vento_m_s) {
    const double kt = vento_m_s / NODO;
    constexpr double limiti[] = {1.0, 7.0, 11.0, 17.0, 22.0, 28.0, 34.0};
    int ss = 0;
    for (double l : limiti)
        if (kt >= l) ++ss;
    return ss;
}

double vento_per_stato_del_mare(int ss) {
    constexpr double centri_kt[] = {0.5, 3.5, 8.5, 13.5, 19.0, 24.5, 30.5, 40.5};
    return centri_kt[std::clamp(ss, 0, 7)] * NODO;
}

double altezza_onda_m(int ss) { return centro_douglas(std::clamp(ss, 0, 7)); }   // la tabella unica (dossier 21 §8.2)

int mare_di(double vento_m_s, double hs_m) { return hs_m >= 0.0 ? classe_douglas(hs_m) : stato_del_mare(vento_m_s); }

// --- profilo ---

double ProfiloSuono::c(double z) const {
    if (z_m.empty()) return C_RIFERIMENTO + GRADIENTE_PRESSIONE * std::max(z, 0.0);
    if (z <= z_m.front()) return c_m_s.front();
    if (z >= z_m.back()) return c_m_s.back() + GRADIENTE_PRESSIONE * (z - z_m.back());
    const auto it = std::upper_bound(z_m.begin(), z_m.end(), z);
    const std::size_t i = static_cast<std::size_t>(it - z_m.begin());
    const double t = (z - z_m[i - 1]) / (z_m[i] - z_m[i - 1]);
    return c_m_s[i - 1] + t * (c_m_s[i] - c_m_s[i - 1]);
}

double ProfiloSuono::gradiente(double z) const {
    if (z_m.size() < 2 || z >= z_m.back()) return GRADIENTE_PRESSIONE;
    if (z <= z_m.front()) return (c_m_s[1] - c_m_s[0]) / (z_m[1] - z_m[0]);
    const auto it = std::upper_bound(z_m.begin(), z_m.end(), z);
    const std::size_t i = static_cast<std::size_t>(it - z_m.begin());
    return (c_m_s[i] - c_m_s[i - 1]) / (z_m[i] - z_m[i - 1]);
}

void ProfiloSuono::analizza() {
    if (z_m.size() < 2 || z_m.size() != c_m_s.size()) return;
    // layer: il massimo di c nei primi 400 m
    std::size_t imax = 0;
    for (std::size_t i = 0; i < z_m.size() && z_m[i] <= SLD_MAX_M; ++i)
        if (c_m_s[i] > c_m_s[imax]) imax = i;
    sld_m = z_m[imax];
    // uno strato sotto i 10 m non e' un condotto: il taglio sta sopra i 3 kHz (8,6 a 5 m) e non intrappola il passivo.
    // Contarlo toglieva dal campo dei raggi, sotto il taglio, proprio i raggi radenti di chi sta in superficie: col profilo
    // di giugno delle Faroe (5 m quasi isotermi) una nave a 5 km perdeva 13 dB (29/10/2026)
    if (sld_m < SLD_MIN_M) sld_m = 0.0;
    gradiente_strato = sld_m > 0.0 ? (c(sld_m) - c(0.0)) / sld_m : 0.0;
    gradiente_sotto = (c(sld_m + 100.0) - c(sld_m)) / 100.0;
    // microstrato: c cala dalla superficie e riprende a salire entro 30 m (sopra il layer)
    microstrato_m = microstrato_dc = 0.0;
    for (std::size_t i = 0; i + 1 < z_m.size() && z_m[i + 1] <= MICROSTRATO_MAX_M; ++i)
        if (c_m_s[i + 1] > c_m_s[i]) {
            if (i > 0 && c_m_s[0] - c_m_s[i] > 0.3) { microstrato_m = z_m[i]; microstrato_dc = c_m_s[0] - c_m_s[i]; }
            break;
        }
    // asse e profondita' critica
    std::size_t imin = 0;
    for (std::size_t i = 0; i < z_m.size(); ++i)
        if (c_m_s[i] < c_m_s[imin]) imin = i;
    asse_m = z_m[imin];
    c_asse = c_m_s[imin];
    double c_vertice = 0.0, z_vertice = 0.0;
    for (std::size_t i = 0; i <= imin; ++i)
        if (c_m_s[i] > c_vertice) { c_vertice = c_m_s[i]; z_vertice = z_m[i]; }
    profondita_critica_m = -1.0;
    for (std::size_t i = imin + 1; i < z_m.size(); ++i)
        if (c_m_s[i] >= c_vertice) {
            const double t = (c_vertice - c_m_s[i - 1]) / (c_m_s[i] - c_m_s[i - 1]);
            profondita_critica_m = z_m[i - 1] + t * (z_m[i] - z_m[i - 1]);
            break;
        }
    // prima CZ: raggio orizzontale al vertice, x = 2 * integrale di c / sqrt(cv^2 - c^2) fra vertice e profondita'
    // critica (dossier 02 §3.4 d), a passi di 1 m sul punto medio
    distanza_cz_m = 0.0;
    if (profondita_critica_m - z_vertice >= CANALE_CZ_M && c_vertice > c_asse + 0.5) {
        double x = 0.0;
        for (double z = z_vertice; z < profondita_critica_m; z += 1.0) {
            const double cm = c(std::min(z + 0.5, profondita_critica_m - 1e-3));
            const double d = c_vertice * c_vertice - cm * cm;
            if (d <= 0.0) continue;
            x += cm / std::sqrt(d) * std::min(1.0, profondita_critica_m - z);
        }
        distanza_cz_m = 2.0 * x;
    }
}

double intensita_pomeriggio(double ora, double vento_m_s, double nuvolosita) {
    if (ora < 0.0) return 0.0;
    const double h = std::fmod(ora, 24.0);
    const double giorno = h < 10.0 ? 0.0 : h < 14.0 ? (h - 10.0) / 4.0 : h <= 18.0 ? 1.0 : h < 22.0 ? 1.0 - (h - 18.0) / 4.0 : 0.0;
    const double vento = std::clamp((11.0 - vento_m_s / NODO) / 3.0, 0.0, 1.0);   // pieno sotto 8 kn, nullo da 11
    const double nubi = std::clamp((0.5 - nuvolosita) / 0.2, 0.0, 1.0);           // pieno sotto il 30%, nullo dal 50%
    return giorno * vento * nubi;
}

ProfiloSuono applica_pomeriggio(const ProfiloSuono& p, double intensita) {
    if (intensita <= 0.0) return p;
    constexpr double SPESSORE_M = 10.0, DELTA_T = 1.5;   // 5-15 m, 0,5-2 gradi (STIMA)
    const double t = p.temperatura_superficie, s = p.salinita_superficie;
    const double dc_dt = velocita_suono_mackenzie(t + 0.5, s, 0.0) - velocita_suono_mackenzie(t - 0.5, s, 0.0);
    ProfiloSuono q;
    q.temperatura_superficie = t + DELTA_T * intensita;
    q.salinita_superficie = s;
    for (double z = 0.0; z <= SPESSORE_M + 1e-9; z += SPESSORE_M / 4.0) {
        q.z_m.push_back(z);
        q.c_m_s.push_back(p.c(z) + dc_dt * DELTA_T * intensita * (1.0 - z / SPESSORE_M));
    }
    for (std::size_t i = 0; i < p.z_m.size(); ++i)
        if (p.z_m[i] > SPESSORE_M + 1e-9) { q.z_m.push_back(p.z_m[i]); q.c_m_s.push_back(p.c_m_s[i]); }
    q.analizza();
    return q;
}

TipoFondo ClimaTeatro::fondo(double profondita_m) const {
    return profondita_m < FONDO_SCARPATA_M ? fondo_piattaforma : profondita_m < FONDO_PROFONDO_M ? fondo_scarpata : fondo_profondo;
}

bool gamberi_attivi(double fondale_m, double temperatura_superficie_c) {
    return fondale_m < GAMBERI_FONDALE_M && temperatura_superficie_c >= GAMBERI_TEMPERATURA_C;
}

std::string file_clima(const std::string& cartella_dati, const std::string& teatro) {
    return cartella_dati + "/teatri/" + teatro + ".suono.toml";
}

const ProfiloSuono& ClimaTeatro::profilo(int mese) const {
    return mesi[static_cast<std::size_t>(((mese - 1) % 12 + 12) % 12)];
}

ClimaTeatro carica_clima(const std::string& percorso) {
    toml::table t;
    try {
        t = toml::parse_file(percorso);
    } catch (const toml::parse_error& e) {
        throw std::runtime_error(percorso + ": " + std::string(e.description()));
    }
    ClimaTeatro out;
    out.teatro = t["teatro"].value_or(std::string{});
    out.ph = t["ph"].value_or(8.0);
    out.traffico = t["traffico"].value_or(0.5);
    out.mese_predefinito = t["mese_predefinito"].value_or(2);
    const auto tipo_fondo = [&](const char* fascia, TipoFondo riserva) {
        const std::string v = t["fondo"][fascia].value_or(std::string{});
        return v == "roccia" ? TipoFondo::roccia : v == "sabbia" ? TipoFondo::sabbia : v == "fango" ? TipoFondo::fango : riserva;
    };
    out.fondo_piattaforma = tipo_fondo("piattaforma", out.fondo_piattaforma);
    out.fondo_scarpata = tipo_fondo("scarpata", out.fondo_scarpata);
    out.fondo_profondo = tipo_fondo("profondo", out.fondo_profondo);
    const auto* mesi = t["mesi"].as_table();
    if (!mesi) throw std::runtime_error(percorso + ": senza [mesi]");
    for (int m = 1; m <= 12; ++m) {
        const auto* tab = (*mesi)[std::to_string(m)].as_table();
        if (!tab) throw std::runtime_error(percorso + ": manca il mese " + std::to_string(m));
        ProfiloSuono& p = out.mesi[static_cast<std::size_t>(m - 1)];
        p.temperatura_superficie = (*tab)["temperatura_superficie"].value_or(10.0);
        p.salinita_superficie = (*tab)["salinita_superficie"].value_or(35.0);
        const auto leggi = [&](const char* campo, std::vector<double>& v) {
            const auto* a = (*tab)[campo].as_array();
            if (!a) throw std::runtime_error(percorso + ": mese " + std::to_string(m) + " senza " + campo);
            for (const auto& x : *a) v.push_back(x.value_or(0.0));
        };
        leggi("z_m", p.z_m);
        leggi("c_m_s", p.c_m_s);
        if (p.z_m.size() < 2 || p.z_m.size() != p.c_m_s.size() || !std::is_sorted(p.z_m.begin(), p.z_m.end()))
            throw std::runtime_error(percorso + ": profilo del mese " + std::to_string(m) + " non valido");
        p.analizza();
    }
    return out;
}

// --- catalogo ---

const TipoCivile* CatalogoAcustico::tipo_civile(const std::string& id) const {
    const auto it = civili.find(id);
    return it == civili.end() ? nullptr : &it->second;
}

const DatiSchiera* CatalogoAcustico::schiera(const std::string& id) const {
    const auto it = schiere.find(id);
    return it == schiere.end() ? nullptr : &it->second;
}

const FirmaRumore* CatalogoAcustico::firma(const std::string& classe) const {
    const auto it = firme.find(classe);
    return it == firme.end() ? nullptr : &it->second;
}

const SorgenteLibreria* CatalogoAcustico::sorgente(const std::string& id) const {
    const auto it = sorgenti.find(id);
    return it == sorgenti.end() ? nullptr : &it->second;
}

const SorgenteLibreria* CatalogoAcustico::transitorio(const std::string& id) const {
    const auto it = transitori.find(id);
    return it == transitori.end() ? nullptr : &it->second;
}

const Sensore* CatalogoAcustico::sensore(const std::string& id) const {
    const auto it = sensori.find(id);
    return it == sensori.end() ? nullptr : &it->second;
}

std::vector<const Sensore*> CatalogoAcustico::sensori_di(const std::string& classe) const {
    std::vector<const Sensore*> out;
    const auto it = suite.find(classe);
    if (it == suite.end()) return out;
    for (const auto& id : it->second)
        if (const auto* s = sensore(id)) out.push_back(s);
    return out;
}

CatalogoAcustico carica_acustica(const std::string& percorso) {
    toml::table t;
    try {
        t = toml::parse_file(percorso);
    } catch (const toml::parse_error& e) {
        throw std::runtime_error(percorso + ": " + std::string(e.description()));
    }
    CatalogoAcustico out;
    const auto numero = [&](const toml::table& tab, const std::string& chi, const char* campo, std::optional<double> def = {}) {
        const auto v = tab[campo].value<double>();
        if (v) return *v;
        if (def) return *def;
        throw std::runtime_error(percorso + ": '" + chi + "' senza " + campo);
    };
    if (const auto* firme = t["firme"].as_table())
        for (auto&& [chiave, nodo] : *firme) {
            const auto* tab = nodo.as_table();
            if (!tab) continue;
            FirmaRumore f;
            f.classe = std::string(chiave.str());
            if (const auto* pt = (*tab)["punti"].as_array())
                for (const auto& x : *pt)
                    if (const auto* coppia = x.as_array(); coppia && coppia->size() == 2)
                        f.punti.emplace_back((*coppia)[0].value_or(0.0), (*coppia)[1].value_or(0.0));
            if (f.punti.size() < 2) throw std::runtime_error(percorso + ": firma '" + f.classe + "' con meno di due punti");
            f.sl_quieto_db = f.punti.front().second;
            f.v_quieto_kn = f.punti.front().first;
            f.v_cavitazione_kn = numero(*tab, f.classe, "v_cavitazione_kn");
            f.tonale_db = numero(*tab, f.classe, "tonale_db");
            f.tonale_hz = numero(*tab, f.classe, "tonale_hz", 300.0);
            f.ts_prua_db = numero(*tab, f.classe, "ts_prua_db");
            f.ts_traverso_db = numero(*tab, f.classe, "ts_traverso_db");
            f.nave = (*tab)["nave"].value_or(false);
            f.snorkel_db = numero(*tab, f.classe, "snorkel_db", 0.0);
            f.fonte = (*tab)["fonte"].value_or(std::string{});
            if (const auto* righe = (*tab)["righe"].as_array())
                for (const auto& x : *righe) {
                    const auto* a = x.as_array();
                    if (!a || a->size() < 3) throw std::runtime_error(percorso + ": firma '" + f.classe + "' con una riga incompleta");
                    RigaFirma r;
                    r.hz = (*a)[0].value_or(0.0);
                    r.db_relativo = (*a)[1].value_or(0.0);
                    const std::string tipo = (*a)[2].value_or(std::string{});
                    constexpr std::pair<const char*, TipoRiga> tipi[] = {
                        {"rete", TipoRiga::rete}, {"pompa", TipoRiga::pompa}, {"turbina", TipoRiga::turbina},
                        {"riduttore", TipoRiga::riduttore}, {"ausiliario", TipoRiga::ausiliario}, {"diesel", TipoRiga::diesel},
                        {"elica", TipoRiga::elica}, {"biologico", TipoRiga::biologico},
                        {"geofisico", TipoRiga::geofisico}, {"industriale", TipoRiga::industriale}};
                    bool noto = false;
                    for (const auto& [nome, tr] : tipi)
                        if (tipo == nome) { r.tipo = tr; noto = true; }
                    if (!noto) throw std::runtime_error(percorso + ": firma '" + f.classe + "' con il tipo di riga ignoto '" + tipo + "'");
                    if (a->size() > 3) r.v_min_kn = (*a)[3].value_or(0.0);
                    if (a->size() > 4) r.v_max_kn = (*a)[4].value_or(1e9);
                    if (a->size() > 5) r.multiplo_asse = (*a)[5].value_or(0.0);
                    f.righe.push_back(r);
                }
            f.pale = (*tab)["pale"].value_or(0);
            f.assi = (*tab)["assi"].value_or(1);
            f.giri_per_nodo = (*tab)["giri_per_nodo"].value_or(0.0);
            f.nazionalita = (*tab)["nazionalita"].value_or(std::string("ignota"));
            f.tipo_unita = (*tab)["tipo_unita"].value_or(std::string("generica"));
            out.firme.emplace(f.classe, std::move(f));
        }
    if (const auto* sensori = t["sensori"].as_table())
        for (auto&& [chiave, nodo] : *sensori) {
            const auto* tab = nodo.as_table();
            if (!tab) continue;
            Sensore s;
            s.id = std::string(chiave.str());
            const std::string modo = (*tab)["modo"].value_or(std::string("passivo"));
            const std::string antenna = (*tab)["antenna"].value_or(std::string("sfera"));
            s.modo = modo == "attivo" ? ModoSensore::attivo : ModoSensore::passivo;
            s.antenna = antenna == "lineare" ? Antenna::lineare : antenna == "scafo" ? Antenna::scafo : Antenna::sfera;
            s.dimensione_m = numero(*tab, s.id, "dimensione_m");
            if (const auto* b = (*tab)["bande_hz"].as_array())
                for (const auto& x : *b) s.bande_hz.push_back(x.value_or(0.0));
            if (s.bande_hz.empty()) throw std::runtime_error(percorso + ": sensore '" + s.id + "' senza bande_hz");
            s.integrazione_s = numero(*tab, s.id, "integrazione_s", s.integrazione_s);
            s.indice_rilevazione = numero(*tab, s.id, "indice_rilevazione", s.indice_rilevazione);
            s.nl_macchina_db = numero(*tab, s.id, "nl_macchina_db");
            s.nl_flusso_db = numero(*tab, s.id, "nl_flusso_db");
            s.v0_flusso_kn = numero(*tab, s.id, "v0_flusso_kn");
            s.velocita_max_kn = numero(*tab, s.id, "velocita_max_kn");
            s.baffles_gradi = numero(*tab, s.id, "baffles_gradi", 0.0);
            s.baffles_db = numero(*tab, s.id, "baffles_db", s.baffles_db);
            s.sl_attivo_db = numero(*tab, s.id, "sl_attivo_db", s.sl_attivo_db);
            s.impulso_s = numero(*tab, s.id, "impulso_s", s.impulso_s);
            s.virata_max_deg_s = numero(*tab, s.id, "virata_max_deg_s", s.virata_max_deg_s);
            s.raddrizzamento_s = numero(*tab, s.id, "raddrizzamento_s", 0.0);
            s.endfire_gradi = numero(*tab, s.id, "endfire_gradi", 0.0);
            s.conico = (*tab)["conico"].value_or(false);
            s.lofar_bin_hz = numero(*tab, s.id, "lofar_bin_hz", 0.0);
            s.lofar_integrazione_s = numero(*tab, s.id, "lofar_integrazione_s", s.lofar_integrazione_s);
            s.di_fisso_db = numero(*tab, s.id, "di_fisso_db", -1.0);
            s.sigma_rilevamento_deg = numero(*tab, s.id, "sigma_rilevamento_deg", 0.0);
            s.omni = (*tab)["omni"].value_or(false);
            s.sigma_distanza_m = numero(*tab, s.id, "sigma_distanza_m", 0.0);
            s.mare_proprio_db = numero(*tab, s.id, "mare_proprio_db", -1.0);
            s.risoluzione_s = numero(*tab, s.id, "risoluzione_s", 0.0);
            s.cavo_diametro_m = numero(*tab, s.id, "cavo_diametro_m", 0.0);
            s.isolamento_proprio_db = numero(*tab, s.id, "isolamento_proprio_db", -1.0);
            if (const auto* g = (*tab)["gamma_hz"].as_array(); g && g->size() == 2) {
                s.f_min_hz = (*g)[0].value_or(0.0);
                s.f_max_hz = (*g)[1].value_or(0.0);
            }
            out.sensori.emplace(s.id, std::move(s));
        }
    if (const auto* suite = t["suite"].as_table())
        for (auto&& [chiave, nodo] : *suite) {
            std::vector<std::string> ids;
            if (const auto* a = nodo.as_array())
                for (const auto& x : *a) {
                    const std::string id = x.value_or(std::string{});
                    if (!out.sensori.count(id)) throw std::runtime_error(percorso + ": suite '" + std::string(chiave.str()) + "' con il sensore ignoto '" + id + "'");
                    ids.push_back(id);
                }
            out.suite.emplace(std::string(chiave.str()), std::move(ids));
        }
    const auto libreria = [&](const char* tabella, std::map<std::string, SorgenteLibreria>& dove) {
        if (const auto* sorgenti = t[tabella].as_table())
            for (auto&& [chiave, nodo] : *sorgenti) {
                const auto* tab = nodo.as_table();
                if (!tab) continue;
                SorgenteLibreria f;
                f.id = std::string(chiave.str());
                const auto testo = [&](const toml::table& x, const char* campo) { return x[campo].value_or(std::string{}); };
                const auto testi = [&](const char* campo) {
                    std::vector<std::string> v;
                    if (const auto* a = (*tab)[campo].as_array()) for (const auto& e : *a) v.push_back(e.value_or(std::string{}));
                    return v;
                };
                const auto coppia = [&](const toml::table& x, const char* campo, double& a, double& b) {
                    if (const auto* v = x[campo].as_array(); v && v->size() == 2) { a = (*v)[0].value_or(a); b = (*v)[1].value_or(b); }
                };
                f.nome = testo(*tab, "nome"); f.nome_breve = testo(*tab, "nome_breve");
                f.categoria = testo(*tab, "categoria"); f.sottocategoria = testo(*tab, "sottocategoria");
                f.teatri = testi("teatri"); f.ore = testi("ore");
                if (const auto* a = (*tab)["mesi"].as_array()) for (const auto& e : *a) f.mesi.push_back(e.value_or(0));
                f.densita = (*tab)["densita"].value_or(0.0);
                coppia(*tab, "profondita_m", f.quota_min_m, f.quota_max_m);
                coppia(*tab, "velocita_kn", f.v_min_kn, f.v_max_kn);
                f.estensione_m = (*tab)["estensione_m"].value_or(0.0);
                f.livello_db = numero(*tab, f.id, "livello_db");
                if (testo(*tab, "livello_tipo") == "pp") f.livello_db -= 9.0;   // picco-picco -> rms (09 §5)
                f.ts_db = (*tab)["ts_db"].value_or(-10.0);
                f.diurno_db = (*tab)["diurno_db"].value_or(0.0);
                f.diurno_ore = testo(*tab, "diurno_ore");
                f.indizi = testi("indizi"); f.confonde_con = testi("confonde_con");
                f.aurale = testo(*tab, "aurale"); f.fonte = testo(*tab, "fonte"); f.confidenza = testo(*tab, "confidenza");
                f.vita_s = (*tab)["vita_s"].value_or(0.0);
                if (const auto* comp = (*tab)["componenti"].as_array())
                    for (const auto& e : *comp) {
                        const auto* x = e.as_table();
                        if (!x) continue;
                        Componente c;
                        const std::string tipo = testo(*x, "tipo");
                        constexpr std::pair<const char*, TipoComponente> tipi[] = {
                            {"tonale", TipoComponente::tonale}, {"famiglia", TipoComponente::famiglia}, {"banda", TipoComponente::banda},
                            {"sweep", TipoComponente::sweep}, {"impulso", TipoComponente::impulso}};
                        bool noto = false;
                        for (const auto& [nome, tc] : tipi) if (tipo == nome) { c.tipo = tc; noto = true; }
                        if (!noto) throw std::runtime_error(percorso + ": sorgente '" + f.id + "' con la componente ignota '" + tipo + "'");
                        c.livello_rel_db = (*x)["livello_rel_db"].value_or(0.0);
                        c.hz = (*x)["hz"].value_or(0.0);
                        c.armoniche = (*x)["armoniche"].value_or(1);
                        c.decadimento_db = (*x)["decadimento_db"].value_or(0.0);
                        c.f_lo = (*x)["f_lo"].value_or(0.0); c.f_hi = (*x)["f_hi"].value_or(0.0);
                        c.picco_hz = (*x)["picco_hz"].value_or(0.0); c.pendenza_db_ott = (*x)["pendenza_db_ott"].value_or(0.0);
                        c.f_da = (*x)["f_da"].value_or(0.0); c.f_a = (*x)["f_a"].value_or(0.0);
                        c.durata_s = (*x)["durata_s"].value_or(0.0);
                        c.ritmata = (*x)["ritmata"].value_or(false);
                        if (c.tipo == TipoComponente::banda || c.tipo == TipoComponente::impulso) c.integrale_db = integrale_forma_db(c);
                        f.componenti.push_back(c);
                    }
                if (f.componenti.empty()) throw std::runtime_error(percorso + ": sorgente '" + f.id + "' senza componenti");
                if (const auto* r = (*tab)["ritmo"].as_table()) {
                    const std::string p = testo(*r, "processo");
                    f.ritmo.processo = p == "periodico" ? Processo::periodico : p == "poisson" ? Processo::poisson
                                     : p == "treno" ? Processo::treno : p == "singolo" ? Processo::singolo : Processo::continuo;
                    coppia(*r, "intervallo_s", f.ritmo.intervallo_min_s, f.ritmo.intervallo_max_s);
                    f.ritmo.tasso_h = (*r)["tasso_h"].value_or(0.0);
                    double gmin = 1, gmax = 1;
                    coppia(*r, "gruppi", gmin, gmax);
                    f.ritmo.gruppo_min = static_cast<int>(gmin); f.ritmo.gruppo_max = static_cast<int>(gmax);
                    coppia(*r, "intervallo_interno_s", f.ritmo.interno_min_s, f.ritmo.interno_max_s);
                    f.ritmo.serie_min = (*r)["serie_min"].value_or(0.0);
                    f.ritmo.pausa_s = (*r)["pausa_s"].value_or(0.0);
                }
                dove.emplace(f.id, std::move(f));
            }
    };
    libreria("sorgenti", out.sorgenti);
    libreria("transitori", out.transitori);
    // [civili.*] (blocco 1 del sonar): i tipi di navi civili; la firma tipica di ognuno fra le firme, se la classe non
    // ne ha gia' una fatta a mano (mercantile, cargo, petroliera, peschereccio restano quelle di sempre)
    if (const auto* civ = t["civili"].as_table())
        for (const auto& [chiave, nodo] : *civ) {
            const auto* tab = nodo.as_table();
            if (!tab) continue;
            TipoCivile tc;
            tc.id = std::string(chiave.str());
            const auto testo = [&](const char* k) { return (*tab)[k].value_or(std::string{}); };
            const auto intervallo = [&](const char* k) {
                const auto* a = (*tab)[k].as_array();
                if (!a || a->size() != 2) throw std::runtime_error(percorso + ": tipo civile '" + tc.id + "' senza " + k + " = [min, max]");
                return Intervallo{(*a)[0].value_or(0.0), (*a)[1].value_or(0.0)};
            };
            tc.nome = testo("nome");
            tc.categoria = testo("categoria");
            tc.classe = testo("classe");
            tc.lunghezza_m = intervallo("lunghezza_m");
            tc.pescaggio_m = intervallo("pescaggio_m");
            tc.velocita_servizio_kn = intervallo("velocita_servizio_kn");
            tc.cavitazione_kn = intervallo("cavitazione_kn");
            tc.velocita_max_rel = (*tab)["velocita_max_rel"].value_or(1.1);
            tc.larghezza_rel = (*tab)["larghezza_rel"].value_or(0.15);
            tc.echo_vc_kn = numero(*tab, tc.id, "echo_vc_kn");
            tc.echo_d_lf = (*tab)["echo_d_lf"].value_or(0.0);
            tc.epoca_db = (*tab)["epoca_db"].value_or(0.0);
            tc.propulsione = propulsione_da_testo(testo("propulsione"));
            tc.passo_variabile = (*tab)["passo_variabile"].value_or(false);
            tc.assi = intervallo("assi");
            tc.pale = intervallo("pale");
            tc.giri_asse = intervallo("giri_asse");
            tc.motori = intervallo("motori");
            tc.cilindri = intervallo("cilindri");
            tc.giri_motore = intervallo("giri_motore");
            tc.generatori = intervallo("generatori");
            tc.cilindri_generatori = intervallo("cilindri_generatori");
            tc.giri_generatori = intervallo("giri_generatori");
            tc.rete_60 = (*tab)["rete_60"].value_or(0.5);
            tc.strascico = (*tab)["strascico"].value_or(false);
            tc.fonte = testo("fonte");
            tc.confidenza = testo("confidenza");
            if (!out.firme.count(tc.id)) {
                std::uint64_t seme = 0xcbf29ce484222325ull;   // FNV-1a dell'id: la firma tipica e' sempre la stessa
                for (const char ch : tc.id) seme = (seme ^ static_cast<unsigned char>(ch)) * 0x100000001b3ull;
                out.firme[tc.id] = genera_nave_civile(tc, seme).firma;
            }
            out.civili[tc.id] = std::move(tc);
        }
    // [schiere.*] (blocco 1 del sonar, D): i sistemi delle schiere trainate
    if (const auto* sch = t["schiere"].as_table())
        for (const auto& [chiave, nodo] : *sch) {
            const auto* tab = nodo.as_table();
            if (!tab) continue;
            DatiSchiera d;
            d.id = std::string(chiave.str());
            d.schiera_m = numero(*tab, d.id, "schiera_m");
            d.cavo_max_m = numero(*tab, d.id, "cavo_max_m");
            d.cavo_diametro_m = numero(*tab, d.id, "cavo_diametro_m");
            d.peso_cavo_n_m = numero(*tab, d.id, "peso_cavo_n_m");
            d.cn = numero(*tab, d.id, "cn", 1.2);
            d.v_flessione_kn = numero(*tab, d.id, "v_flessione_kn");
            d.filata_m_s = numero(*tab, d.id, "filata_m_s");
            d.recupero_m_s = numero(*tab, d.id, "recupero_m_s");
            d.v_manovra_kn = numero(*tab, d.id, "v_manovra_kn");
            d.v_max_kn = numero(*tab, d.id, "v_max_kn");
            d.quota_max_m = numero(*tab, d.id, "quota_max_m");
            d.fonte = (*tab)["fonte"].value_or(std::string{});
            out.schiere.emplace(d.id, std::move(d));
        }
    if (out.firme.empty() || out.sensori.empty()) throw std::runtime_error(percorso + ": senza firme o senza sensori");
    return out;
}

// --- perdita di trasmissione (dossier 02 §3.4, livello A di §10.2) ---

const char* nome_percorso(Percorso p) {
    switch (p) {
        case Percorso::diretto: return "diretto";
        case Percorso::condotto: return "condotto";
        case Percorso::zona_convergenza: return "zona di convergenza";
        case Percorso::fondo: return "fondo";
        case Percorso::acqua_bassa: return "acqua bassa";
        case Percorso::raggi: return "raggi";
        case Percorso::nessuno: return "nessuno";
    }
    return "?";
}

// specchio di Lloyd, incoerente oltre pi/4 e con la coerenza di Eckart per il mare mosso (§3.4 h)
SpecchioLloyd specchio_lloyd(double f_hz, double r_m, double zs, double zr, const Ambiente& amb) {
    const ProfiloSuono& p = amb.profilo ? *amb.profilo : isotermo();
    const double fondale = std::max(amb.fondale_m, 1.0);
    zs = std::clamp(zs, 0.5, fondale);
    zr = std::clamp(zr, 0.5, fondale);
    const double r = std::max(r_m, 1.0);
    const double r3 = std::hypot(r, zs - zr);
    SpecchioLloyd out;
    out.c0_m_s = p.c(0.0);
    const int ss = mare_di(amb.vento_m_s, amb.hs_m);
    const double k = 2.0 * PI * f_hz / out.c0_m_s;
    const double x = k * zs * zr / r;
    const double sin_radenza = std::min(1.0, (zs + zr) / r3);
    out.sigma_onda_m = (amb.hs_m >= 0.0 ? amb.hs_m : altezza_onda_m(ss)) / 4.0;
    const double coerenza = std::exp(-2.0 * std::pow(k * out.sigma_onda_m * sin_radenza, 2.0));
    out.fattore = x < PI / 4.0 ? 2.0 - 2.0 * coerenza * std::cos(2.0 * x) : 2.0;
    return out;
}

Perdita perdita_trasmissione(double f_hz, double r_m, double zs, double zr, const Ambiente& amb) {
    const ProfiloSuono& p = amb.profilo ? *amb.profilo : isotermo();
    const double fondale = std::max(amb.fondale_m, 1.0);
    zs = std::clamp(zs, 0.5, fondale);
    zr = std::clamp(zr, 0.5, fondale);
    const double r = std::max(r_m, 1.0);
    const double r3 = std::hypot(r, zs - zr);
    const double c0 = p.c(0.0);
    const double alfa = assorbimento_db_km(f_hz, p.temperatura_superficie, p.salinita_superficie, 0.5 * (zs + zr), amb.ph) / 1000.0;
    const int ss = mare_di(amb.vento_m_s, amb.hs_m);
    const double h = p.sld_m;

    struct Contributo { double tl; Percorso p; int zona; };
    std::vector<Contributo> vie;

    const double d_lloyd = -10.0 * std::log10(std::max(specchio_lloyd(f_hz, r, zs, zr, amb).fattore, 1e-3));

    // layer: penalita' d'attraversamento crescente con il gradiente sotto lo strato, minore sotto il taglio (§3.4 c)
    const double f_taglio = taglio_condotto_hz(h, c0);
    double d_layer = 0.0;
    if (h > 0.0 && ((zs < h) != (zr < h))) {
        d_layer = std::clamp(5.0 + 100.0 * std::max(0.0, -p.gradiente_sotto), 5.0, 20.0);
        if (f_hz < f_taglio) d_layer *= std::max(0.25, f_hz / f_taglio);
        d_layer *= std::min(1.0, r / 2000.0);
    }

    if (fondale < ACQUA_BASSA_M) {
        // Marsh-Schulkin (§3.4 i); R in kyd, L e D in piedi
        const double rk = r3 / KYD;
        const double hk = std::max(std::sqrt((h / PIEDE + fondale / PIEDE) / 3.0), 0.05);   // parametro di salto (kyd)
        const double ak = alfa * KYD;
        const double at = (amb.fondo == TipoFondo::roccia ? 1.0 : amb.fondo == TipoFondo::sabbia ? 2.0 : 4.0) + 0.3 * ss;
        constexpr double KL = 2.0;
        double tl;
        if (rk < hk) tl = 20.0 * std::log10(std::max(rk, 1e-3)) + ak * rk + 60.0 - KL;
        else if (rk < 8.0 * hk) tl = 15.0 * std::log10(rk) + ak * rk + at * (rk / hk - 1.0) + 5.0 * std::log10(hk) + 60.0 - KL;
        else tl = 10.0 * std::log10(rk) + ak * rk + at * (rk / hk - 1.0) + 10.0 * std::log10(hk) + 64.5 - KL;
        vie.push_back({tl + d_lloyd * 0.5, Percorso::acqua_bassa, 0});
    } else {
        // diretto, con la zona d'ombra oltre la portata del raggio limite
        double ombra = 0.0;
        if (p.gradiente_sotto < 0.0 || h == 0.0) {
            const auto x_raggio = [&](double z) {
                if (z < h) return arco(h - z, std::max(p.gradiente_strato, 1e-4), c0);
                const double zz = std::max(z, h + 100.0);
                const double g = (p.c(zz) - p.c(h)) / (zz - h);
                return g < 0.0 ? arco(z - h + 1.0, g, c0) : 1e9;
            };
            const double r_dir = x_raggio(zs) + x_raggio(zr);
            if (r > r_dir) ombra = std::min(OMBRA_MAX_DB, 20.0 * (r / r_dir - 1.0));
        }
        // effetto pomeriggio: il microtermoclino sopra il condotto piega i raggi verso il basso (§3.5)
        const double d_pomeriggio = p.microstrato_m > 0.0 && std::min(zs, zr) < MICROSTRATO_PORTATA * p.microstrato_m
                                        ? std::min(10.0, MICROSTRATO_DB_PER_MS * p.microstrato_dc) * std::min(1.0, r / 1000.0) : 0.0;
        vie.push_back({20.0 * std::log10(r3) + alfa * r3 + d_lloyd + d_layer + ombra + d_pomeriggio, Percorso::diretto, 0});
        // condotto superficiale (§3.4 b)
        if (h > 0.0 && p.gradiente_strato > 0.0 && zs < h && zr < h && f_hz >= f_taglio) {
            const double rc = c0 / p.gradiente_strato;
            const double salto = std::sqrt(8.0 * rc * h);
            const double r0 = salto / 6.0;
            if (r3 > r0) {
                const double per_salto = (0.5 + 0.6 * ss) * (1.0 + 2.0 * std::pow(f_taglio / f_hz, 2.0));   // STIMA
                vie.push_back({10.0 * std::log10(r0) + 10.0 * std::log10(r3) + (alfa + per_salto / salto) * r3 + d_pomeriggio,
                               Percorso::condotto, 0});
            }
        }
        // zone di convergenza (§3.4 d)
        if (p.distanza_cz_m > 0.0 && fondale >= p.profondita_critica_m + ECCESSO_CZ_M) {
            const double rz = p.distanza_cz_m;
            for (int z = 1; z <= 3; ++z) {
                const double w = CZ_SEMILARGHEZZA * rz * z;
                const double g = (CZ_GUADAGNO_DB - CZ_PERDITA_ZONA_DB * (z - 1)) * std::exp(-std::pow((r - z * rz) / w, 2.0));
                if (g > 0.5) vie.push_back({20.0 * std::log10(r3) + alfa * r3 - g, Percorso::zona_convergenza, z});
            }
        }
        // rimbalzo sul fondo (§3.4 e)
        if (fondale < 1e8) {
            const double dv = 2.0 * fondale - zs - zr;
            const double angolo = gradi(std::atan2(dv, r));
            if (angolo >= FONDO_ANGOLO_MIN_GRADI) {
                const double l = std::hypot(r, dv);
                vie.push_back({20.0 * std::log10(l) + alfa * l + perdita_fondo_db(amb.fondo, f_hz, angolo), Percorso::fondo, 0});
            }
        }
    }
    Perdita out;
    double somma = 0.0, migliore = 1e9;
    for (const auto& v : vie) {
        somma += std::pow(10.0, -v.tl / 10.0);
        if (v.tl < migliore) { migliore = v.tl; out.percorso = v.p; out.zona = v.zona; }
    }
    out.tl_db = somma > 0.0 ? std::max(0.0, -10.0 * std::log10(somma)) : 1e9;
    return out;
}

// --- rumore (dossier 02 §4) ---

namespace {
// le fessure termiche sopra il termine del ghiaccio (lf = log10 della frequenza in kHz); 0 senza (bit per bit come prima)
double carattere_pack_db(double lf, const Ambiente& amb) {
    const double fe = std::clamp(amb.ghiaccio_fessure, 0.0, 1.0);
    if (fe <= 0.0) return 0.0;
    const double b = (lf - std::log10(FESSURE_KHZ)) / FESSURE_LARGHEZZA;
    return FESSURE_DB * fe * std::exp(-b * b);
}
// le creste che si spingono: un livello proprio attorno ai 100 Hz (piu' forte del traffico lontano), che cala di 12 dB per
// ogni larghezza in decadi; -300 senza
double creste_db(double lf, const Ambiente& amb) {
    const double cr = std::clamp(amb.ghiaccio_creste, 0.0, 1.0) * std::clamp(amb.ghiaccio, 0.0, 1.0);
    if (cr <= 0.0) return -300.0;
    const double a = (lf - std::log10(CRESTE_KHZ)) / CRESTE_LARGHEZZA;
    return CRESTE_LIVELLO_DB + 10.0 * std::log10(cr) - CRESTE_CALO_DB * a * a;
}
}  // namespace

double rumore_ambiente(double f_hz, const Ambiente& amb) {
    const double f = std::max(f_hz, 1.0) / 1000.0;
    const double lf = std::log10(f);
    const double s = std::clamp(amb.traffico, 0.0, 1.0);
    const double w = std::min(amb.vento_m_s, 15.0) * (1.0 - std::clamp(amb.ghiaccio, 0.0, 1.0));   // satura oltre 15 m/s
    const auto vento = [&](double v) { return 50.0 + 7.5 * std::sqrt(v) + 20.0 * lf - 40.0 * std::log10(f + 0.4); };
    double nl = 17.0 - 30.0 * lf;
    nl = db_somma(nl, 40.0 + 20.0 * (s - 0.5) + 26.0 * lf - 60.0 * std::log10(f + 0.03));
    nl = db_somma(nl, vento(w));
    nl = db_somma(nl, -15.0 + 20.0 * lf);
    if (amb.pioggia_mm_h > 0.0) {   // §4.4 (STIMA)
        const double base = 50.0 + 10.0 * std::log10(amb.pioggia_mm_h);
        double r = f < 5.0 ? base + 10.0 * std::log10(f / 5.0) : f <= 25.0 ? base : base - 10.0 * std::log10(f / 25.0);
        if (amb.pioggia_mm_h < 2.0) r += 5.0 * std::exp(-std::pow(std::log10(f / 15.0) / 0.15, 2.0));
        nl = db_somma(nl, r);
    }
    if (amb.gamberi) {   // §4.5: +20-30 dB in 2-20 kHz (STIMA: 68 dB nella banda, -20 dB/decade fuori)
        const double fuori = f < 2.0 ? std::log10(2.0 / f) : f > 20.0 ? std::log10(f / 20.0) : 0.0;
        nl = db_somma(nl, 68.0 - 20.0 * fuori);
    }
    if (amb.ghiaccio > 0.0) {   // §4.6: pack quieto 5 dB sotto SS0, margine del ghiaccio +10-20 dB (STIMA)
        const double g = std::clamp(amb.ghiaccio, 0.0, 1.0);
        nl = db_somma(nl, vento(0.0) - 5.0 + 15.0 * 4.0 * g * (1.0 - g) + carattere_pack_db(lf, amb));
        if (amb.ghiaccio_creste > 0.0) nl = db_somma(nl, creste_db(lf, amb));
    }
    return nl;
}

ComponentiRumore componenti_rumore_ambiente(double f_hz, const Ambiente& amb) {
    ComponentiRumore c;
    const double f = std::max(f_hz, 1.0) / 1000.0;
    const double lf = std::log10(f);
    const double s = std::clamp(amb.traffico, 0.0, 1.0);
    const double w = std::min(amb.vento_m_s, 15.0) * (1.0 - std::clamp(amb.ghiaccio, 0.0, 1.0));
    const auto vento = [&](double v) { return 50.0 + 7.5 * std::sqrt(v) + 20.0 * lf - 40.0 * std::log10(f + 0.4); };
    c.turbolenza = 17.0 - 30.0 * lf;
    c.traffico = 40.0 + 20.0 * (s - 0.5) + 26.0 * lf - 60.0 * std::log10(f + 0.03);
    c.vento = vento(w);
    c.termico = -15.0 + 20.0 * lf;
    if (amb.pioggia_mm_h > 0.0) {
        const double base = 50.0 + 10.0 * std::log10(amb.pioggia_mm_h);
        double r = f < 5.0 ? base + 10.0 * std::log10(f / 5.0) : f <= 25.0 ? base : base - 10.0 * std::log10(f / 25.0);
        if (amb.pioggia_mm_h < 2.0) r += 5.0 * std::exp(-std::pow(std::log10(f / 15.0) / 0.15, 2.0));
        c.pioggia = r;
    }
    if (amb.gamberi) {
        const double fuori = f < 2.0 ? std::log10(2.0 / f) : f > 20.0 ? std::log10(f / 20.0) : 0.0;
        c.gamberi = 68.0 - 20.0 * fuori;
    }
    if (amb.ghiaccio > 0.0) {
        const double g = std::clamp(amb.ghiaccio, 0.0, 1.0);
        c.ghiaccio = vento(0.0) - 5.0 + 15.0 * 4.0 * g * (1.0 - g) + carattere_pack_db(lf, amb);
        if (amb.ghiaccio_creste > 0.0) c.ghiaccio = db_somma(c.ghiaccio, creste_db(lf, amb));
    }
    return c;
}

double velocita_cavitazione_kn(const FirmaRumore& f, double quota_m) {
    return f.v_cavitazione_kn * std::sqrt((10.33 + std::max(quota_m, 0.0)) / 10.33);
}

bool cavita(const FirmaRumore& f, double v_kn, double quota_m) {
    return !f.nave && v_kn > velocita_cavitazione_kn(f, quota_m);
}

namespace {
// R6: un'elica danneggiata cavita prima (STIMA: fino al 40% di velocita' in meno all'insorgenza)
double v_cavitazione_di(const Sorgente& s) {
    const double v = velocita_cavitazione_kn(*s.firma, s.quota_m);
    return s.var.attive && s.var.danno_elica > 0.0 ? v * (1.0 - 0.4 * std::clamp(s.var.danno_elica, 0.0, 1.0)) : v;
}
bool cavitazione_di(const Sorgente& s) {
    if (s.cavitazione) return *s.cavitazione;
    if (!s.firma) return false;
    if (s.var.attive && s.var.danno_elica > 0.0) return !s.firma->nave && s.velocita_kn > v_cavitazione_di(s);
    return cavita(*s.firma, s.velocita_kn, s.quota_m);
}
double banda_piu_vicina(double f_hz) {
    double migliore = BANDE_HZ[0];
    for (double b : BANDE_HZ)
        if (std::fabs(std::log(b / f_hz)) < std::fabs(std::log(migliore / f_hz))) migliore = b;
    return migliore;
}
}  // namespace

double livello_banda_larga(const Sorgente& s) {
    if (s.muta) return LIVELLO_MUTO_DB;
    if (!s.firma) return s.fonte ? livello_totale_fonte(s) : 0.0;
    const FirmaRumore& fr = *s.firma;
    // banda larga 5-200 Hz dalla tabella velocita' -> livello (dossier 01 §3.2), interpolata in log v
    const double v = std::max(s.velocita_kn, 0.0);
    double sl = fr.sl_quieto_db;
    if (!fr.punti.empty()) {
        const auto& pt = fr.punti;
        if (v <= pt.front().first) sl = pt.front().second;
        else {
            std::size_t i = 1;
            while (i + 1 < pt.size() && v > pt[i].first) ++i;
            const double a = std::log10(pt[i - 1].first), b = std::log10(pt[i].first);
            sl = pt[i - 1].second + (pt[i].second - pt[i - 1].second) * (std::log10(v) - a) / (b - a);
        }
    } else if (v > fr.v_quieto_kn) {
        sl += 20.0 * std::log10(v / fr.v_quieto_kn);
    }
    // cavitazione: l'insorgenza cresce con la profondita' dell'elica (§2.3)
    if (cavitazione_di(s) && !fr.nave) {
        const double v_cav = v_cavitazione_di(s);
        sl += CAVITAZIONE_PENDENZA * std::log10(std::max(v, v_cav) / v_cav);
        // R6: l'elica danneggiata cavita anche piu' forte, +8-15 dB all'insorgenza (dossier 18 §3, Spo88)
        if (s.var.attive && s.var.danno_elica > 0.0) sl += 8.0 + 7.0 * std::clamp(s.var.danno_elica, 0.0, 1.0);
    }
    if (s.snorkel) sl += fr.snorkel_db;
    if (s.sul_fondo) sl += POSATO_DB;
    if (s.var.attive) sl += s.var.scarto_db + s.var.usura_db + s.var.urto_db + s.var.virata_db;
    return sl + s.transitorio_db;
}

double livello_sorgente(double f_hz, const Sorgente& s) {
    if (s.muta) return LIVELLO_MUTO_DB;
    if (!s.firma) return s.fonte ? livello_fonte(s, f_hz, true) : 0.0;
    return livello_sorgente_da(f_hz, s, livello_banda_larga(s));
}

double livello_sorgente_da(double f_hz, const Sorgente& s, double banda_larga_db) {
    if (s.muta) return LIVELLO_MUTO_DB;
    if (!s.firma) return s.fonte ? livello_fonte(s, f_hz, true) : 0.0;
    // spettro: livello per Hz a 1 kHz = banda - 20 dB, -20 dB/decade sopra 200 Hz, piatto sotto (01 §3.1)
    double spettro = banda_larga_db - 20.0 - 20.0 * std::log10(std::max(f_hz, 200.0) / 1000.0);
    if (s.firma->nazionalita == "civile") spettro -= SPETTRO_CIVILI_SOTTO_DB;
    if (cavitazione_di(s) && !s.firma->nave && f_hz > 100.0) {
        const double v_cav = v_cavitazione_di(s);
        // con la cavitazione data dalla dinamica il gradino e' intero; dalla firma sale in 2 kn
        spettro += CAVITAZIONE_SALTO_DB * (s.cavitazione ? 1.0 : smoothstep(v_cav, v_cav + 2.0, s.velocita_kn));
    }
    // impulso attivo: livello spettrale nella sua banda (1/durata)
    if (s.ping_db > 0.0 && banda_piu_vicina(s.ping_hz) == banda_piu_vicina(f_hz))
        spettro = db_somma(spettro, s.ping_db + 10.0 * std::log10(s.ping_s));
    return spettro;
}

Sorgente sorgente_acustica(const CatalogoAcustico& c, const UnitaAcustica& u) {
    Sorgente s;
    s.id = u.id; s.pos = u.pos; s.quota_m = u.quota_m; s.rotta = u.rotta; s.velocita_kn = u.velocita_kn;
    s.cavitazione = u.cavitazione; s.transitorio_db = u.transitorio_db; s.snorkel = u.snorkel; s.sul_fondo = u.sul_fondo;
    s.esca = u.falso_bersaglio;
    s.nave = u.categoria == "nave";
    s.fedelta = std::clamp(u.fedelta, 0.0, 1.0);   // il falso contatto (D3)
    if (u.categoria == "aereo") return s;   // fuori dal sonar
    if (u.falso_bersaglio) s.firma = c.firma(u.categoria == "arma" ? "adc_mk2" : "biologico");
    if (!s.firma) s.firma = u.firma;   // l'individuo (navi del traffico, nucleo/navi_civili)
    if (!s.firma) s.firma = c.firma(u.classe == "type53" ? "uset80" : u.classe);
    if (!s.firma) {
        const char* generica = u.categoria == "nave" ? "nave_generica" : u.categoria == "arma" ? "siluro_generico"
                               : u.categoria == "nessuna" ? "biologico" : "sottomarino_generico";
        s.firma = c.firma(generica);
    }
    s.lunghezza_m = u.lunghezza_m > 0.0 ? u.lunghezza_m
                    : u.falso_bersaglio ? (u.categoria == "arma" ? 0.0 : 20.0)
                    : u.categoria == "nave" ? 150.0 : u.categoria == "arma" ? 6.0 : 100.0;
    // R6: solo navi e sottomarini veri (non armi, esche, biologici)
    if (u.variazioni && !u.falso_bersaglio && (u.categoria == "nave" || u.categoria == "sottomarino")) {
        VariazioniSorgente& v = s.var;
        v.attive = true;
        v.seme = u.seme;
        Casuale caso(u.seme ^ 0x5eed688a5eedull);
        const auto normale = [&caso] {
            const double u1 = std::max(caso.uniforme(), 1e-300), u2 = caso.uniforme();
            return std::sqrt(-2.0 * std::log(u1)) * std::cos(2.0 * PI * u2);
        };
        // l'individuo (dossier 18 §3: Sponagle, sigma 2,4 dB fra un giorno e l'altro, 5-6,8 fra navi diverse; le righe
        // dei giri spostate dell'1-3%, STIMA; le pompe sulla rete appena)
        v.scarto_db = std::clamp(2.4 * normale(), -5.0, 5.0);
        v.fattore_giri = 1.0 + std::clamp(0.015 * normale(), -0.03, 0.03);
        v.fattore_pompe = 1.0 + std::clamp(0.0025 * normale(), -0.005, 0.005);
        if (s.firma && s.firma->pale > 0 && caso.uniforme() < 0.05) {   // l'elica «cantante», 100-1000 Hz (Ross 9.5)
            v.cantante_hz = 100.0 * std::pow(10.0, caso.uniforme());
            v.cantante_da_kn = caso.uniforme(6.0, 14.0);
            v.cantante_a_kn = v.cantante_da_kn + caso.uniforme(3.0, 6.0);
        }
        // il momento: virate (Trevorrow 2008: 1,65 dB per grado/s, fino a +18 su una nave; meta' sui sottomarini, STIMA),
        // danni, usura, regime
        v.virata_db = std::min(1.65 * std::fabs(u.rateo_virata_deg_s), u.categoria == "nave" ? 18.0 : 12.0);
        v.usura_db = 5.0 * std::clamp(u.usura, 0.0, 1.0);
        v.urto_db = std::clamp(u.urto_db, 0.0, 12.0);
        v.danno_elica = std::clamp(u.danno_elica, 0.0, 1.0);
        v.danno_macchine = std::clamp(u.danno_macchine, 0.0, 1.0);
        v.regime = u.regime_pompe;
        v.tempo_s = u.tempo_s;
    }
    return s;
}

const char* nome_regime_pompe(RegimePompe r) {
    switch (r) {
        case RegimePompe::automatico: return "automatico";
        case RegimePompe::spente: return "spente";
        case RegimePompe::lente: return "lente";
        case RegimePompe::veloci: return "veloci";
    }
    return "?";
}

double aspetto_db(const Sorgente& s, Vec2 ascoltatore, double f_hz) {
    if (!s.var.attive || !s.firma || distanza(s.pos, ascoltatore) < 1.0) return 0.0;
    // theta dalla prua (0) alla poppa (180); Trevorrow: cos^1,95(theta - 97) + 0,08 rispetto al traverso, poppa +3 dB
    const double theta = std::fabs(normalizza_180(rilevamento(s.pos, ascoltatore) - s.rotta));
    const auto forma = [](double t) { return std::pow(std::fabs(std::cos(radianti(t - 97.0))), 1.95) + 0.08; };
    double db = 10.0 * std::log10(forma(theta) / forma(97.0)) + 3.0 * std::max(0.0, -std::cos(radianti(theta)));
    db = std::min(db, 0.0);
    if (!s.nave) db *= 0.3;   // sottomarino: 30%, prua ~-3 dB (STIMA: nessuna misura aperta; decisione del 25/10/2026)
    const double basse = std::clamp((f_hz - 100.0) / 200.0, 0.0, 1.0);   // 100-300 Hz: da 30% a 100%
    return db * (0.3 + 0.7 * basse);
}

double forza_superficie_db(double f_hz, double vento_m_s, double radente_deg) {
    // Chapman e Harris (1962): Ss = 3,3 b log(theta/30) - 42,4 log b + 2,6, b = 158 (v f^(1/3))^-0,58, v in kn
    const double v = std::max(vento_m_s / NODO, 1.0);
    const double b = 158.0 * std::pow(v * std::cbrt(std::max(f_hz, 1.0)), -0.58);
    const double th = std::clamp(radente_deg, 0.1, 30.0);
    return std::min(SS_MASSIMA_DB, 3.3 * b * std::log10(th / 30.0) - 42.4 * std::log10(b) + 2.6);
}

double frequenza_pala_hz(const Sorgente& s) {
    if (!s.firma || s.firma->pale <= 0 || (s.firma->giri_per_nodo <= 0.0 && s.firma->giri_costanti <= 0.0) || s.velocita_kn < 0.5) return 0.0;
    const double giri = s.firma->giri_costanti > 0.0 ? s.firma->giri_costanti : s.firma->giri_per_nodo * s.velocita_kn;
    const double f = s.firma->pale * giri / 60.0;
    return s.var.attive ? f * s.var.fattore_giri : f;
}

double livello_pala_db(const Sorgente& s) {
    if (frequenza_pala_hz(s) <= 0.0) return -300.0;
    Sorgente rif = s;   // la banda larga a 10 kn senza cavitazione, snorkel, posato e transitori
    rif.velocita_kn = PALA_RIFERIMENTO_KN;
    rif.cavitazione = false;
    rif.snorkel = rif.sul_fondo = false;
    rif.transitorio_db = 0.0;
    return livello_sorgente(100.0, rif) + PALA_SU_CONTINUO_DB + PALA_PENDENZA_DB * std::log10(s.velocita_kn / PALA_RIFERIMENTO_KN) +
           (cavitazione_di(s) ? PALA_CAVITAZIONE_DB : 0.0) + (s.sul_fondo ? POSATO_DB : 0.0);
}

SorgenteAcustica sorgente_per_siluri(const Sorgente& s) {
    SorgenteAcustica a;
    a.id = s.id; a.pos = s.pos; a.quota = s.quota_m; a.rotta = s.rotta; a.velocita = s.velocita_kn * NODO;
    a.livello_db = livello_banda_larga(s) + SCALA_TESTA_DB;
    a.forza_bersaglio_db = s.firma ? 0.5 * (s.firma->ts_prua_db + s.firma->ts_traverso_db) : 0.0;
    a.lunghezza_m = s.lunghezza_m;
    a.esca = s.esca;
    a.nave = s.nave;
    return a;
}

double rumore_proprio(double f_hz, const Sensore& s, double velocita_kn, bool nei_baffles) {
    // §2.5: max(macchinario, flusso + 60 log(V/V0)), stessa forma spettrale della sorgente (STIMA)
    const double v = std::max(velocita_kn, 0.1);
    double nl = std::max(s.nl_macchina_db, s.nl_flusso_db + 60.0 * std::log10(v / s.v0_flusso_kn));
    nl -= 20.0 * std::log10(std::max(f_hz, 100.0) / 1000.0);
    if (nei_baffles) nl += s.baffles_db;
    if (velocita_kn > s.velocita_max_kn) nl += SORDO_DB;
    return nl;
}

double rumore_sfregamento_db(double f_hz, double velocita_kn, double frazione, TipoFondo fondo) {
    if (frazione <= 0.0) return -300.0;
    // STIMA (dossier 04 §2.1 senza numeri): a pieno contatto su sabbia a 5 kn circa 95 dB re 1 uPa^2/Hz fino a 300 Hz,
    // 20-25 dB sopra il rumore proprio della TB-16 a quelle velocita'
    const double duro = fondo == TipoFondo::roccia ? 6.0 : fondo == TipoFondo::fango ? -8.0 : 0.0;
    return SFREGAMENTO_5KN_DB + 30.0 * std::log10(std::max(velocita_kn, 0.5) / 5.0) + 10.0 * std::log10(std::min(frazione, 1.0)) +
           duro - 20.0 * std::log10(std::max(f_hz, 300.0) / 300.0);
}

double rumore_proprio_ricevitore(double f_hz, const Ricevitore& r, double velocita_kn, bool nei_baffles) {
    const double proprio = rumore_proprio(f_hz, *r.sensore, velocita_kn, nei_baffles);
    if (r.sfregamento <= 0.0) return proprio;   // senza contatto col fondo: quello di sempre, bit per bit
    return db_somma(proprio, rumore_sfregamento_db(f_hz, r.velocita_kn, r.sfregamento, r.fondo_sfregamento));
}

double rumore_mare_proprio_db(const Sensore& s, double quota_m, const Ambiente& amb) {
    const int mare = mare_di(amb.vento_m_s, amb.hs_m);
    if (mare < 5) return 0.0;
    const double forza = mare >= 6 ? 1.0 : 0.6;
    const bool schiera = s.antenna == Antenna::lineare;
    const double massimo = s.mare_proprio_db >= 0.0 ? s.mare_proprio_db : schiera ? 6.0 : 10.0;
    const double fino_a = schiera ? 150.0 : 100.0;
    if (massimo <= 0.0) return 0.0;
    return forza * massimo * std::clamp(1.0 - std::max(quota_m, 0.0) / fino_a, 0.0, 1.0);
}

double indice_direttivita(const Sensore& s, double f_hz) {
    if (s.di_fisso_db >= 0.0) return s.di_fisso_db;
    const double lambda = C_RIFERIMENTO / std::max(f_hz, 1.0);
    const double di = s.antenna == Antenna::lineare ? 10.0 * std::log10(2.0 * s.dimensione_m / lambda)
                                                    : 20.0 * std::log10(PI * s.dimensione_m / lambda);
    return std::max(0.0, di);
}

double soglia(const Sensore& s) {
    // §1.2: rivelatore d'energia DT = 5 log(d W / t) riferito a 1 Hz, cioe' S/N in banda 5 log(d / (W t)); la banda
    // e' un'ottava attorno alla frequenza (W = 0,7 f) e qui si da' il valore di riferimento a 1 kHz. Attivo con
    // replica: DT = 10 log(d / 2t) riferito a 1 Hz, cioe' 10 log(d / 2) in banda W = 1/t
    if (s.modo == ModoSensore::attivo) return 10.0 * std::log10(s.indice_rilevazione / 2.0) - s.abilita_db;
    return 5.0 * std::log10(s.indice_rilevazione / (700.0 * s.integrazione_s)) - s.abilita_db;
}

namespace {
double soglia_banda(const Sensore& s, double f_hz) {
    if (s.modo == ModoSensore::attivo) return soglia(s);
    return 5.0 * std::log10(s.indice_rilevazione / (0.7 * f_hz * s.integrazione_s)) - s.abilita_db;
}
}  // namespace

double forza_bersaglio(const FirmaRumore& f, double aspetto) {
    // §1.4: TS(phi) = prua + (traverso - prua) |sin phi|^3, piu' il lobo speculare di +4 dB a +-7 gradi dal traverso
    const double phi = std::fabs(normalizza_180(aspetto));
    const double lobo = 4.0 * std::exp(-std::pow((phi - 90.0) / 7.0, 2.0));
    return f.ts_prua_db + (f.ts_traverso_db - f.ts_prua_db) * std::pow(std::fabs(std::sin(radianti(phi))), 3.0) + lobo;
}

const char* nome_tipo_riga(TipoRiga t) {
    switch (t) {
        case TipoRiga::rete: return "rete";
        case TipoRiga::pompa: return "pompa";
        case TipoRiga::turbina: return "turbina";
        case TipoRiga::riduttore: return "riduttore";
        case TipoRiga::ausiliario: return "ausiliario";
        case TipoRiga::diesel: return "diesel";
        case TipoRiga::elica: return "elica";
        case TipoRiga::biologico: return "biologico";
        case TipoRiga::geofisico: return "geofisico";
        case TipoRiga::industriale: return "industriale";
    }
    return "?";
}

double larghezza_fascio_deg(const Sensore& s, double f_hz, double relativo) {
    const double lambda = C_RIFERIMENTO / std::max(f_hz, 1.0);
    if (s.antenna == Antenna::lineare) {
        // traverso 50,8 gradi lambda/L; verso l'endfire / sin(angolo dall'asse), al piu' 2 sqrt(lambda/L) rad (§1.2)
        const double a = std::min(std::fabs(relativo), 180.0 - std::fabs(relativo));
        const double max_endfire = gradi(2.0 * std::sqrt(lambda / s.dimensione_m));
        return std::min(50.8 * lambda / s.dimensione_m / std::max(std::sin(radianti(a)), 1e-3), max_endfire);
    }
    return std::min(58.4 * lambda / s.dimensione_m, 180.0);   // apertura circolare: 1,02 lambda / D
}

double errore_rilevamento_deg(const Rilevazione& r, const Sensore& s) {
    if (s.sigma_rilevamento_deg > 0.0) return s.sigma_rilevamento_deg;   // misurato (DIFAR, RGB-2)
    const double theta = larghezza_fascio_deg(s, r.banda_hz, r.rilevamento_relativo);
    // SNR dopo l'integrazione: d alla soglia, per 10^(SE/10) sopra (SE limitato a -10 dB)
    const double snr = s.indice_rilevazione * std::pow(10.0, std::max(r.se_db, -10.0) / 10.0);
    return std::max(ERRORE_MIN_DEG, theta / std::sqrt(snr));
}

// --- equazione del sonar ---

namespace {
// `perdita(f, r, zs, zr)` -> Perdita: un template, non std::function (30/10/2026: costruirla e distruggerla a ogni
// valutazione era il 7% della ripresa del replay; la stessa aritmetica, bit per bit)
template <class FunzionePerdita>
Rilevazione valuta_con(const Sorgente& s, const Ricevitore& rc, const Ambiente& amb, const FunzionePerdita& perdita) {
    Rilevazione out;
    if (!rc.sensore || (!s.firma && !s.fonte)) return out;
    const Sensore& sn = *rc.sensore;
    const double r = distanza(rc.pos, s.pos);
    const double relativo = r > 0.0 ? normalizza_180(rilevamento(rc.pos, s.pos) - rc.rotta) : 0.0;
    out.baffles = sn.baffles_gradi > 0.0 && std::fabs(relativo) > 180.0 - sn.baffles_gradi;
    out.sordo = rc.velocita_kn > sn.velocita_max_kn;
    out.cieco = (sn.endfire_gradi > 0.0 && std::fabs(relativo) < sn.endfire_gradi) || rc.rateo_virata_deg_s > sn.virata_max_deg_s;
    out.rilevamento_relativo = relativo;
    out.conico = sn.conico;
    out.cavitazione = cavitazione_di(s);
    out.transitorio = s.transitorio_db > 0.0;
    out.ping = s.ping_db > 0.0;
    double penalita = out.cieco ? CIECO_DB : 0.0;
    if (!out.cieco && sn.raddrizzamento_s > 0.0 && rc.tempo_da_virata_s < sn.raddrizzamento_s)
        penalita += RADDRIZZAMENTO_DB * (1.0 - rc.tempo_da_virata_s / sn.raddrizzamento_s);
    const double v_proprio = rc.sul_fondo ? 0.0 : rc.velocita_kn;   // posato: niente flusso
    const double mare_db = rumore_mare_proprio_db(sn, rc.quota_m, amb);   // il mare sui sensori vicino alla superficie
    const bool attivo = sn.modo == ModoSensore::attivo;
    double ts = 0.0, v_radiale_kn = 0.0;
    if (attivo) {
        ts = s.firma ? forza_bersaglio(*s.firma, r > 0.0 ? rilevamento(s.pos, rc.pos) - s.rotta : 0.0) : s.fonte->ts_db;
        const double rr = radianti(rilevamento(rc.pos, s.pos));
        const Vec2 u{std::sin(rr), std::cos(rr)};
        const double rs = radianti(s.rotta), rp = radianti(rc.rotta);
        v_radiale_kn = (s.velocita_kn * (std::sin(rs) * u.x + std::cos(rs) * u.y)) -
                       (rc.velocita_kn * (std::sin(rp) * u.x + std::cos(rp) * u.y));
    }
    for (double f : sn.bande_hz) {
        if (rc.schiera_danneggiata && f > SCHIERA_DANNEGGIATA_HZ) continue;   // la schiera danneggiata non ha l'alta
        const Perdita p = perdita(f, r, s.quota_m, rc.quota_m);
        const double nl = db_somma(rumore_ambiente(f, amb), rumore_proprio_ricevitore(f, rc, v_proprio, out.baffles) + mare_db);
        // nei baffles i fasci verso poppa sono schermati dallo scafo e dall'elica: niente guadagno d'antenna (STIMA)
        const double di = (out.baffles ? 0.0 : indice_direttivita(sn, f)) - (rc.schiera_danneggiata ? SCHIERA_DANNEGGIATA_DB : 0.0);
        double dt = soglia_banda(sn, f);
        double se, sl, rl = -1e9;
        if (attivo) {
            sl = sn.sl_attivo_db;
            // riverberazione di volume: V = r^2 psi c tau / 2, psi dall'indice di direttivita' (§1.3)
            const double psi = 4.0 * PI * std::pow(10.0, -di / 10.0);
            const double volume = std::max(r, 1.0) * std::max(r, 1.0) * psi * C_RIFERIMENTO * sn.impulso_s / 2.0;
            rl = sl - 2.0 * p.tl_db + SV_VOLUME_DB + 10.0 * std::log10(volume);
            // superficie (Chapman-Harris, col vento) e fondo (Lambert) nell'anello che arriva insieme all'eco: distanza
            // obliqua R dell'eco, area R phi c tau / 2 (phi il fascio orizzontale; omni: tutto il giro), angolo radente
            // dalla quota del trasduttore. Il lobo verticale (gaussiano, largo quanto il fascio: STIMA) attenua andata e
            // ritorno verso la superficie e il fondo ripidi
            const double zr = std::max(rc.quota_m, QUOTA_MIN_RIVERBERAZIONE_M);
            const double obliqua = std::max(std::hypot(r, s.quota_m - rc.quota_m), 1.0);
            const double fascio = larghezza_fascio_deg(sn, f, relativo);
            const double orizzontale = sn.omni ? 2.0 * PI : radianti(fascio);
            const double tau = sn.risoluzione_s > 0.0 ? sn.risoluzione_s : sn.impulso_s;
            const double anello = 10.0 * std::log10(obliqua * orizzontale * C_RIFERIMENTO * tau / 2.0);
            const auto lobo = [&](double elevazione_deg) {
                const double x = elevazione_deg / std::max(0.5 * fascio, 0.5);
                return 2.0 * std::min(LOBO_VERTICALE_MAX_DB, 3.0 * x * x);   // -3 dB a meta' fascio, per verso
            };
            if (obliqua > zr) {
                const double rs = std::sqrt(obliqua * obliqua - zr * zr), radente = gradi(std::asin(zr / obliqua));
                // sotto la volta la diffusione del ghiaccio rugoso, pesata con la copertura (senza ghiaccio il mare di sempre)
                double ss = forza_superficie_db(f, amb.vento_m_s, radente);
                if (const double g = std::clamp(amb.ghiaccio, 0.0, 1.0); g > 0.0) {
                    const double seno = std::sin(radianti(std::max(radente, 0.1))) / std::sin(radianti(SS_VOLTA_RADENTE_DEG));
                    const double volta = std::min(SS_VOLTA_MASSIMA_DB, SS_VOLTA_DB + 10.0 * std::log10(std::max(amb.rugosita_ghiaccio_m, 0.1)) +
                                                                           10.0 * std::log10(seno));
                    ss = 10.0 * std::log10((1.0 - g) * std::pow(10.0, ss / 10.0) + g * std::pow(10.0, volta / 10.0));
                }
                rl = db_somma(rl, sl - 2.0 * perdita_trasmissione(f, rs, zr, 1.0, amb).tl_db + ss + anello - lobo(radente));
            }
            if (const double h = amb.fondale_m - zr; amb.fondale_m < 1e6 && h > 0.0 && obliqua > h) {
                const double rb = std::sqrt(obliqua * obliqua - h * h), radente = gradi(std::asin(h / obliqua));
                rl = db_somma(rl, sl - 2.0 * perdita_trasmissione(f, rb, zr, amb.fondale_m - 1.0, amb).tl_db + SB_FONDO_DB +
                                      20.0 * std::log10(h / obliqua) + anello - lobo(radente));
            }
            if (std::fabs(v_radiale_kn) > DOPPLER_RIVERBERAZIONE_KN) rl -= DOPPLER_GUADAGNO_DB;
            if (s.sul_fondo) {   // posato: l'eco si confonde con la riverberazione del fondo (§1.3, Lambert)
                const double phi = radianti(std::min(58.4 * (C_RIFERIMENTO / f) / sn.dimensione_m, 180.0));
                const double area = std::max(r, 1.0) * phi * C_RIFERIMENTO * sn.impulso_s / 2.0;
                rl = std::max(rl, sl - 2.0 * p.tl_db + SB_FONDO_DB + 10.0 * std::log10(area));
            }
            // l'impulso occupa la banda 1/tau: il rumore spettrale si integra su quella
            const double nl_banda = nl + 10.0 * std::log10(1.0 / sn.impulso_s);
            se = sl - 2.0 * p.tl_db + ts - std::max(nl_banda - di, rl) - dt;
        } else if (s.ping_db > 0.0 && banda_piu_vicina(s.ping_hz) == banda_piu_vicina(f)) {
            // un impulso attivo nella banda (contro-scoperta, WLR-9): il rumore della sorgente come sempre, e l'impulso
            // a un rivelatore d'energia sulla sua durata. Il ricevitore integra la banda B (0,7 f, come la soglia
            // passiva) per tau: segnale SL - TL contro il rumore della banda NL + 10 log B - DI, soglia
            // 5 log(d / (B tau)) (BT >= 1). Il livello spettrale dell'impulso col guadagno 5 log(B T) del rumore a banda
            // larga lo sopravvalutava di 10 log B + 5 log(tau T): a 25 kHz e 10 ms, 32 dB
            Sorgente quieta = s;
            quieta.ping_db = 0.0;
            sl = livello_sorgente(f, quieta);
            if (s.var.attive) sl += aspetto_db(s, rc.pos, f);
            se = sl - p.tl_db - (nl - di) - dt;
            const double banda = 0.7 * f;
            const double dt_impulso = 5.0 * std::log10(sn.indice_rilevazione / std::max(banda * s.ping_s, 1.0)) - sn.abilita_db;
            const double se_impulso = s.ping_db - p.tl_db - (nl + 10.0 * std::log10(banda) - di) - dt_impulso;
            if (se_impulso > se) {
                se = se_impulso;
                sl = s.ping_db;
                dt = dt_impulso;
            }
        } else {
            sl = livello_sorgente(f, s);
            if (s.var.attive) sl += aspetto_db(s, rc.pos, f);
            se = sl - p.tl_db - (nl - di) - dt;
        }
        se -= penalita;
        if (se > out.se_db) {
            out.se_db = se; out.banda_hz = f; out.sl_db = sl; out.tl_db = p.tl_db; out.nl_db = nl; out.di_db = di;
            out.dt_db = dt; out.ts_db = ts; out.rl_db = rl; out.percorso = p.percorso; out.zona = p.zona;
        }
    }
    // LOFAR (§1.2, §2.7): la tonale piu' forte in un bin b, integrata per T; soglia 5 log(d / (b T)) nel bin. Con la
    // tonale dei macchinari, le righe di pala (asse, pala, doppia pala) che cadono nella gamma del sensore
    if (!attivo && sn.lofar_bin_hz > 0.0 && s.firma && !s.muta) {
        const FirmaRumore& fr = *s.firma;
        double lo = sn.f_min_hz, hi = sn.f_max_hz;
        const bool gamma = lo > 0.0 && hi > 0.0;   // data: vale anche per la tonale (boe); dalle bande: solo per le pale
        if (lo <= 0.0 && !sn.bande_hz.empty()) lo = *std::min_element(sn.bande_hz.begin(), sn.bande_hz.end()) / std::sqrt(2.0);
        if (hi <= 0.0 && !sn.bande_hz.empty()) hi = *std::max_element(sn.bande_hz.begin(), sn.bande_hz.end()) * std::sqrt(2.0);
        std::vector<std::pair<double, double>> righe;
        if (s.var.attive) {   // R6: tutte le righe che la firma emette ora (regime, individuo, danni), con l'aspetto
            for (const RigaEmessa& re : righe_emesse(s))
                if (re.tipo == TipoRiga::elica ? (re.hz >= lo && re.hz <= hi) : (!gamma || (re.hz >= lo && re.hz <= hi)))
                    righe.emplace_back(re.hz, re.db + aspetto_db(s, rc.pos, re.hz));
        } else {
            if (!gamma || (fr.tonale_hz >= lo && fr.tonale_hz <= hi))
                righe.emplace_back(fr.tonale_hz, fr.tonale_db + (s.sul_fondo ? POSATO_DB : 0.0));
            if (const double fp = frequenza_pala_hz(s); fp > 0.0) {
                const double pala = livello_pala_db(s);
                for (const auto& [hz, db] : {std::pair{fp / fr.pale, pala - 6.0}, std::pair{fp, pala}, std::pair{2.0 * fp, pala - 6.0}})
                    if (hz >= lo && hz <= hi) righe.emplace_back(hz, db);
            }
        }
        const double dt = 5.0 * std::log10(sn.indice_rilevazione / (sn.lofar_bin_hz * sn.lofar_integrazione_s)) - sn.abilita_db;
        for (const auto& [f, sl] : righe) {
            if (rc.schiera_danneggiata && f > SCHIERA_DANNEGGIATA_HZ) continue;
            const Perdita p = perdita(f, r, s.quota_m, rc.quota_m);
            const double nl = db_somma(rumore_ambiente(f, amb), rumore_proprio_ricevitore(f, rc, v_proprio, out.baffles) + mare_db);
            const double di = (out.baffles ? 0.0 : indice_direttivita(sn, f)) - (rc.schiera_danneggiata ? SCHIERA_DANNEGGIATA_DB : 0.0);
            const double se = sl - p.tl_db - (nl + 10.0 * std::log10(sn.lofar_bin_hz) - di) - dt - penalita;
            out.se_tonale_db = std::max(out.se_tonale_db, se);
            if (se > out.se_db) {
                out.se_db = se; out.banda_hz = f; out.sl_db = sl; out.tl_db = p.tl_db; out.nl_db = nl; out.di_db = di;
                out.dt_db = dt; out.percorso = p.percorso; out.zona = p.zona; out.lofar = true;
            }
        }
    }
    return out;
}
}  // namespace

Rilevazione valuta(const Sorgente& s, const Ricevitore& rc, const Ambiente& amb) {
    return valuta_con(s, rc, amb, [&](double f, double r, double zs, double zr) { return perdita_trasmissione(f, r, zs, zr, amb); });
}

Rilevazione valuta(const Sorgente& s, const Ricevitore& rc, const Ambiente& amb, CacheCampi& campi) {
    if (!rc.sensore || (!s.firma && !s.fonte)) return Rilevazione{};
    const CampoPerdita* c = campi.campo(s.id, s.pos, s.quota_m, amb);
    return valuta_con(s, rc, amb, [&](double f, double r, double zs, double zr) {
        if (const auto tl = c ? c->tl(f, r, zr) : std::nullopt) return Perdita{*tl, Percorso::raggi, 0};
        return perdita_trasmissione(f, r, zs, zr, amb);
    });
}

Rilevazione valuta(const Sorgente& s, const Ricevitore& rc, const Ambiente& amb, CacheCampiAsincrona& campi, double tempo_s) {
    if (!rc.sensore || (!s.firma && !s.fonte)) return Rilevazione{};
    const CampoPerdita* c = campi.campo(s.id, s.pos, s.quota_m, amb, tempo_s);
    return valuta_con(s, rc, amb, [&](double f, double r, double zs, double zr) {
        if (const auto tl = c ? c->tl(f, r, zr) : std::nullopt) return Perdita{*tl, Percorso::raggi, 0};
        return perdita_trasmissione(f, r, zs, zr, amb);
    });
}

// --- tracciamento dei raggi (livello B, dossier 02 §10.2) ---

std::optional<double> CampoPerdita::tl(double f_hz, double r, double zr) const {
    if (nr < 2 || nz < 1 || r > r_max_m || r < 0.0) return std::nullopt;
    const double x = r / dr_m, y = std::clamp(zr / dz_m, 0.0, static_cast<double>(nz - 1));
    const int i = std::min(static_cast<int>(x), nr - 2), j = std::min(static_cast<int>(y), std::max(nz - 2, 0));
    const double tx = x - i, ty = nz > 1 ? y - j : 0.0;
    const bool basse = f_hz < FREQ_BASSE_HZ && !tl_basse_db.empty();
    const bool condotto = f_hz < taglio_condotto_hz && !tl_senza_condotto_db.empty();
    const std::vector<float>& g = condotto ? (basse && !tl_senza_condotto_basse_db.empty() ? tl_senza_condotto_basse_db : tl_senza_condotto_db)
                                           : (basse ? tl_basse_db : tl_db);
    const auto v = [&](int a, int b) { return static_cast<double>(g[static_cast<std::size_t>(a * nz + std::min(b, nz - 1))]); };
    const double t = (1 - tx) * ((1 - ty) * v(i, j) + ty * v(i, j + 1)) + tx * ((1 - ty) * v(i + 1, j) + ty * v(i + 1, j + 1));
    const double slant = std::hypot(r, zr - zs_m);
    return t + assorbimento_db_km(f_hz, temperatura, salinita, 0.5 * (zs_m + zr), ph) * slant / 1000.0;
}

CampoPerdita calcola_campo(double zs, const Ambiente& amb, const ParametriRaggi& par) {
    const ProfiloSuono& p = amb.profilo ? *amb.profilo : isotermo();
    const bool con_fondo = amb.fondale_m < 1e8;
    const double fondale = con_fondo ? std::max(amb.fondale_m, 2.0) : std::numeric_limits<double>::infinity();
    zs = std::clamp(zs, 0.5, con_fondo ? fondale - 0.5 : 1e9);
    CampoPerdita c;
    c.zs_m = zs; c.dr_m = par.dr_m; c.dz_m = par.dz_m; c.r_max_m = par.r_max_m;
    c.nr = static_cast<int>(par.r_max_m / par.dr_m) + 1;
    c.nz = static_cast<int>(std::min(con_fondo ? fondale : 1e9, par.quota_max_m) / par.dz_m) + 1;
    c.temperatura = p.temperatura_superficie; c.salinita = p.salinita_superficie; c.ph = amb.ph;
    // in acqua bassa i raggi ripidi muoiono presto sul fondo e lontano arrivano quelli quasi orizzontali: un ventaglio
    // piu' fitto (con 101 raggi su 30 gradi non ce n'e' nessuno fra 0 e 0,6 gradi)
    const bool bassa = con_fondo && fondale < par.acqua_bassa_m;
    const int n = std::max(bassa ? par.raggi_acqua_bassa : par.raggi, 3), nr = c.nr, nz = c.nz;
    const double th_max = radianti(bassa ? std::min(par.angolo_max_gradi, par.angolo_acqua_bassa_gradi) : par.angolo_max_gradi),
                 dth = 2.0 * th_max / (n - 1);
    const double sigma_onde = (amb.hs_m >= 0.0 ? amb.hs_m : altezza_onda_m(stato_del_mare(amb.vento_m_s))) / 4.0;
    // sotto il ghiaccio (dossier 06 §5.1, STIMA): la rugosita' della volta al posto delle onde, pesata con la copertura, e
    // 0,5 dB per rimbalzo nel ghiaccio; senza ghiaccio la superficie di sempre, bit per bit
    const double copertura = std::clamp(amb.ghiaccio, 0.0, 1.0);
    const double sigma_h = copertura > 0.0 ? std::sqrt((1.0 - copertura) * sigma_onde * sigma_onde +
                                                        copertura * amb.rugosita_ghiaccio_m * amb.rugosita_ghiaccio_m)
                                           : sigma_onde;
    const double perdita_volta = copertura > 0.0 ? std::pow(10.0, -GHIACCIO_RIMBALZO_DB * copertura / 10.0) : 1.0;
    // perdite al contorno a due frequenze: 1 kHz (le bande da FREQ_BASSE_HZ in su) e 250 Hz (sotto). Il fondo sotto
    // l'angolo critico e la superficie mossa perdono molto meno in basso
    const double k_rif = 2.0 * PI * 1000.0 / p.c(0.0), k_basse = 2.0 * PI * 250.0 / p.c(0.0);
    const std::size_t celle = static_cast<std::size_t>(n) * static_cast<std::size_t>(nr);
    const float nan = std::numeric_limits<float>::quiet_NaN();
    std::vector<float> zc(celle, nan), uc(celle, nan), cc(celle, 0.0f), pc(celle, 0.0f), pb(celle, 0.0f);
    // condotto superficiale: lo strato sopra la SLD, se e' piu' sottile del fondale; un raggio che non e' mai sceso
    // sotto e' un raggio del condotto (sotto il taglio non porta energia, e il suo fascio resta nello strato)
    const double sld = p.sld_m > 0.0 && p.gradiente_strato > 0.0 && p.sld_m < 0.8 * fondale ? p.sld_m : 0.0;
    std::vector<std::uint8_t> nel_condotto(celle, 0);
    // equazioni dei raggi sull'arco: dz/ds = sin(th), dr/ds = cos(th), dth/ds = -(g / c) cos(th) (th positivo in giu')
    // c(z) e il gradiente dallo stesso tratto del profilo, con l'indice del tratto portato da un passo all'altro: gli
    // stessi valori di ProfiloSuono::c e ::gradiente (la stessa aritmetica, bit per bit), senza le quattro ricerche binarie
    // per passo che prendevano un terzo del calcolo
    const std::vector<double>& pz = p.z_m;
    const std::vector<double>& pc_ = p.c_m_s;
    std::size_t tratto = 1;
    const auto deriva = [&](double z, double th) {
        double cz, g;
        if (pz.empty()) {
            cz = C_RIFERIMENTO + GRADIENTE_PRESSIONE * std::max(z, 0.0);
            g = GRADIENTE_PRESSIONE;
        } else if (z <= pz.front()) {
            cz = pc_.front();
            g = pz.size() < 2 || z >= pz.back() ? GRADIENTE_PRESSIONE : (pc_[1] - pc_[0]) / (pz[1] - pz[0]);
        } else if (z >= pz.back()) {
            cz = pc_.back() + GRADIENTE_PRESSIONE * (z - pz.back());
            g = GRADIENTE_PRESSIONE;
        } else {   // pz[tratto - 1] <= z < pz[tratto], come upper_bound
            while (tratto + 1 < pz.size() && pz[tratto] <= z) ++tratto;
            while (tratto > 1 && pz[tratto - 1] > z) --tratto;
            const std::size_t i = tratto;
            const double t = (z - pz[i - 1]) / (pz[i] - pz[i - 1]);
            cz = pc_[i - 1] + t * (pc_[i] - pc_[i - 1]);
            g = (pc_[i] - pc_[i - 1]) / (pz[i] - pz[i - 1]);
        }
        return -g / cz * std::cos(th);
    };
    for (int k = 0; k < n; ++k) {
        double th = -th_max + k * dth, z = zs, r = 0.0, u = zs, segno = 1.0, potenza = 1.0, potenza_b = 1.0, piu_giu = zs;
        int prossima = 1;
        const double ds = par.passo_m;
        for (int passi = 0; r < par.r_max_m && prossima < nr && passi < 200000; ++passi) {
            // punto medio (RK2)
            const double k1 = deriva(z, th);
            const double zm = z + 0.5 * ds * std::sin(th), thm = th + 0.5 * ds * k1;
            const double k2 = deriva(zm, thm);
            double z1 = z + ds * std::sin(thm), r1 = r + ds * std::cos(thm), th1 = th + ds * k2;
            const double du = ds * std::sin(thm) * segno;
            if (z1 < 0.0) {   // superficie: riflessione con la perdita di Eckart (mare mosso)
                z1 = -z1; th1 = -th1; segno = -segno;
                const double a_rif = k_rif * sigma_h * std::sin(std::fabs(th1)), coer = std::exp(-2.0 * (a_rif * a_rif));
                potenza *= std::max(coer * coer, 1e-3);
                const double a_basse = k_basse * sigma_h * std::sin(std::fabs(th1)), coer_b = std::exp(-2.0 * (a_basse * a_basse));
                potenza_b *= std::max(coer_b * coer_b, 1e-3);
                if (copertura > 0.0) potenza *= perdita_volta, potenza_b *= perdita_volta;
            }
            if (con_fondo && z1 > fondale) {   // fondo: riflessione con la perdita del tipo di fondo
                z1 = 2.0 * fondale - z1; th1 = -th1; segno = -segno;
                potenza *= std::pow(10.0, -perdita_fondo_db(amb.fondo, 1000.0, gradi(std::fabs(th1))) / 10.0);
                potenza_b *= std::pow(10.0, -perdita_fondo_db(amb.fondo, 250.0, gradi(std::fabs(th1))) / 10.0);
            }
            const double u1 = u + du;
            while (prossima < nr && r1 >= prossima * par.dr_m) {
                const double t = (prossima * par.dr_m - r) / std::max(r1 - r, 1e-9);
                const std::size_t q = static_cast<std::size_t>(k) * static_cast<std::size_t>(nr) + static_cast<std::size_t>(prossima);
                zc[q] = static_cast<float>(std::fabs(z1 - z) < ds ? z + t * (z1 - z) : z1);
                uc[q] = static_cast<float>(u + t * (u1 - u));
                cc[q] = static_cast<float>(std::cos(th + t * (th1 - th)));
                pc[q] = static_cast<float>(potenza);
                pb[q] = static_cast<float>(potenza_b);
                nel_condotto[q] = sld > 0.0 && piu_giu <= sld * 1.05 + 1.0;
                ++prossima;
            }
            z = z1; r = r1; th = th1; u = u1;
            piu_giu = std::max(piu_giu, z);
            if (std::max(potenza, potenza_b) < 1e-5) break;   // 50 dB sotto: oltre il tetto della sferica + 25 dB
        }
    }
    // intensita': fasci gaussiani larghi quanto il tubo fra i raggi vicini (nell'ordine di lancio, quote svolte)
    std::vector<double> intensita(static_cast<std::size_t>(nr) * static_cast<std::size_t>(nz), 0.0);
    std::vector<double> senza(sld > 0.0 ? intensita.size() : 0, 0.0);
    std::vector<double> intensita_b(intensita.size(), 0.0), senza_b(senza.size(), 0.0);   // a 250 Hz
    const auto at = [&](int k, int i) { return static_cast<std::size_t>(k) * static_cast<std::size_t>(nr) + static_cast<std::size_t>(i); };
    for (int i = 1; i < nr; ++i) {
        const double ri = i * par.dr_m;
        for (int k = 0; k < n; ++k) {
            const std::size_t q = at(k, i);
            if (std::isnan(zc[q])) continue;
            const bool prima = k > 0 && !std::isnan(uc[at(k - 1, i)]), dopo = k + 1 < n && !std::isnan(uc[at(k + 1, i)]);
            double h = ri * dth;
            if (prima && dopo) h = std::fabs(uc[at(k + 1, i)] - uc[at(k - 1, i)]) / 2.0;
            else if (prima) h = std::fabs(uc[q] - uc[at(k - 1, i)]);
            else if (dopo) h = std::fabs(uc[at(k + 1, i)] - uc[q]);
            else if (nel_condotto[q]) h = std::min(h, 2.0 * sld);       // isolato nel condotto: resta nello strato
            else if (con_fondo) h = std::min(h, fondale);               // isolato in acqua bassa: non oltre il fondale
            const double sigma = std::max(h / 2.0, par.dz_m);
            const double th0 = -th_max + k * dth;
            const double base = std::cos(th0) * dth / (ri * std::max(static_cast<double>(cc[q]), 0.05)) /
                                (sigma * std::sqrt(2.0 * PI));
            const double a = base * pc[q], a_b = base * pb[q];
            const double zk = zc[q];
            if (con_fondo && 3.0 * sigma > fondale) {   // fascio piu' largo della colonna d'acqua: l'energia la riempie tutta
                const double u = a * sigma * std::sqrt(2.0 * PI) / fondale, u_b = a_b * sigma * std::sqrt(2.0 * PI) / fondale;
                for (int j = 0; j < nz; ++j) {
                    const std::size_t cella = static_cast<std::size_t>(i * nz + j);
                    intensita[cella] += u;
                    intensita_b[cella] += u_b;
                    if (!senza.empty() && !nel_condotto[q]) senza[cella] += u, senza_b[cella] += u_b;
                }
                continue;
            }
            const int j0 = std::max(0, static_cast<int>((zk - 3.0 * sigma) / par.dz_m)),
                      j1 = std::min(nz - 1, static_cast<int>((zk + 3.0 * sigma) / par.dz_m) + 1);
            for (int j = j0; j <= j1; ++j) {
                const double zj = j * par.dz_m;
                const double gj = (zj - zk) / sigma;   // x * x al posto di pow(x, 2): lo stesso arrotondamento, senza la chiamata
                double g = std::exp(-0.5 * (gj * gj));
                if (zk < 3.0 * sigma) {   // immagine in superficie
                    const double gi = (zj + zk) / sigma;
                    g += std::exp(-0.5 * (gi * gi));
                }
                if (con_fondo && zk > fondale - 3.0 * sigma) {   // immagine nel fondo
                    const double gf = (2.0 * fondale - zj - zk) / sigma;
                    g += std::exp(-0.5 * (gf * gf));
                }
                const std::size_t cella = static_cast<std::size_t>(i * nz + j);
                intensita[cella] += a * g;
                intensita_b[cella] += a_b * g;
                if (!senza.empty() && !nel_condotto[q]) senza[cella] += a * g, senza_b[cella] += a_b * g;
            }
        }
    }
    const auto riempi = [&](const std::vector<double>& in, std::vector<float>& out) {
        out.resize(static_cast<std::size_t>(nr) * static_cast<std::size_t>(nz));
        for (int i = 0; i < nr; ++i)
            for (int j = 0; j < nz; ++j) {
                const double zj = j * par.dz_m;
                const double sferica = 20.0 * std::log10(std::max(std::hypot(i * par.dr_m, zj - zs), 1.0));
                const double v = in[static_cast<std::size_t>(i * nz + j)];
                const double tl = i == 0 ? sferica : v > 0.0 ? -10.0 * std::log10(v) : 1e9;
                out[static_cast<std::size_t>(i * nz + j)] = static_cast<float>(std::min(tl, sferica + OMBRA_MAX_DB));
            }
    };
    riempi(intensita, c.tl_db);
    riempi(intensita_b, c.tl_basse_db);
    if (!senza.empty()) {
        riempi(senza, c.tl_senza_condotto_db);
        riempi(senza_b, c.tl_senza_condotto_basse_db);
        c.taglio_condotto_hz = taglio_condotto_hz(sld, p.c(0.0));
    }
    return c;
}

const CampoPerdita* CacheCampi::campo(int id, Vec2 pos, double zs, const Ambiente& amb) {
    auto it = voci_.find(id);
    const bool valida = it != voci_.end() && distanza(it->second.pos, pos) <= soglia_spostamento_m &&
                        std::fabs(it->second.zs - zs) <= soglia_quota_m && it->second.profilo == amb.profilo &&
                        it->second.fondo == amb.fondo &&
                        mare_di(it->second.vento, it->second.hs) == mare_di(amb.vento_m_s, amb.hs_m) &&
                        std::fabs(it->second.fondale - amb.fondale_m) <= 0.1 * std::max(amb.fondale_m, 1.0) &&
                        std::fabs(it->second.ghiaccio - amb.ghiaccio) <= 0.1;
    if (!valida) {
        if (nel_passo_ >= limite_per_passo) return it != voci_.end() ? &it->second.campo : nullptr;   // al prossimo passo
        Voce v{calcola_campo(zs, amb, par_), pos, zs, amb.fondale_m, amb.vento_m_s, amb.profilo, amb.fondo, amb.hs_m, amb.ghiaccio};
        ++calcoli_;
        ++nel_passo_;
        it = voci_.insert_or_assign(id, std::move(v)).first;
    }
    return &it->second.campo;
}

double probabilita_rilevazione(double se_db, double sigma_db) {
    if (sigma_db <= 0.0) return se_db >= 0.0 ? 1.0 : 0.0;
    return 0.5 * std::erfc(-se_db / (sigma_db * std::sqrt(2.0)));
}

void Fluttuazione::avanza(double dt_s, double tau_s, double sigma_db, Casuale& caso) {
    const double rho = std::exp(-dt_s / std::max(tau_s, 1e-3));
    const double u1 = std::max(caso.uniforme(), 1e-12), u2 = caso.uniforme();
    const double n = std::sqrt(-2.0 * std::log(u1)) * std::cos(2.0 * PI * u2);   // Box-Muller
    db = rho * db + std::sqrt(1.0 - rho * rho) * sigma_db * n;
}

}  // namespace m688

namespace m688 {

const CampoPerdita* CacheCampiAsincrona::campo(int id, Vec2 pos, double zs, const Ambiente& amb, double tempo_s) {
    if (!condivisi_.empty() && condivisi_.count(id)) return condiviso(id, pos, zs, amb, tempo_s);
    Voce& v = voci_[id];
    // il campo chiesto entra in vigore al suo istante: se il thread non ha finito, lo si aspetta (ripetibilita')
    if (v.attesa && tempo_s >= v.pronto_al) {
        v.in_vigore = v.in_arrivo.get();
        v.in_arrivo = {};
        v.attesa = false;
    }
    const Chiesto& c = v.chiesto;
    const bool prima = !v.in_vigore && !v.attesa;
    const bool cambiato = prima || distanza(c.pos, pos) > soglia_spostamento_m || std::fabs(c.zs - zs) > soglia_quota_m ||
                          c.profilo != amb.profilo || c.fondo != amb.fondo ||
                          mare_di(c.vento, c.hs) != mare_di(amb.vento_m_s, amb.hs_m) ||
                          std::fabs(c.fondale - amb.fondale_m) > 0.1 * std::max(amb.fondale_m, 1.0) || std::fabs(c.ghiaccio - amb.ghiaccio) > 0.1;
    if (cambiato && !v.attesa) lancia(v, pos, zs, amb, tempo_s);
    return v.in_vigore.get();
}

void CacheCampiAsincrona::lancia(Voce& v, Vec2 pos, double zs, const Ambiente& amb, double tempo_s) {
    // copie per il thread: il profilo dell'ambiente puo' cambiare o sparire prima che il calcolo finisca
    auto profilo = amb.profilo ? std::make_shared<const ProfiloSuono>(*amb.profilo) : nullptr;
    Ambiente copia = amb;
    const ParametriRaggi par = par_;
    v.in_arrivo = std::async(std::launch::async, [profilo, copia, zs, par]() mutable {
                      copia.profilo = profilo.get();
                      return std::shared_ptr<const CampoPerdita>(std::make_shared<CampoPerdita>(calcola_campo(zs, copia, par)));
                  }).share();
    v.pronto_al = tempo_s + ritardo_s_;
    v.attesa = true;
    v.chiesto = {pos, zs, amb.fondale_m, amb.vento_m_s, amb.profilo, amb.fondo, amb.hs_m, amb.ghiaccio};
    ++richieste_;
}

const CampoPerdita* CacheCampiAsincrona::condiviso(int id, Vec2 pos, double zs, const Ambiente& amb, double tempo_s) {
    // le chiavi che nessuno usa da un pezzo, una volta al minuto di gioco (dal tempo: ripetibile)
    if (tempo_s >= prossima_pulizia_) {
        std::erase_if(insieme_, [&](const auto& x) { return !x.second.v.attesa && x.second.ultimo_uso < tempo_s - scadenza_s; });
        prossima_pulizia_ = tempo_s + 60.0;
    }
    const auto fascia = [](double f) { return static_cast<long>(std::floor(std::log(std::max(f, 1.0)) / std::log(1.1))); };
    const Chiave k{std::lround(zs / passo_quota_m), amb.profilo, static_cast<int>(amb.fondo), mare_di(amb.vento_m_s, amb.hs_m),
                   fascia(amb.fondale_m), std::lround(amb.ghiaccio * 10.0)};
    Condivisa& c = insieme_[k];
    Voce& v = c.v;
    if (v.attesa && tempo_s >= v.pronto_al) {   // come per una nave sola: in vigore al suo istante
        v.in_vigore = v.in_arrivo.get();
        v.in_arrivo = {};
        v.attesa = false;
    }
    if (!v.in_vigore && !v.attesa) lancia(v, pos, zs, amb, tempo_s);
    c.ultimo_uso = tempo_s;
    auto& mio = ultimi_[id];   // finche' il campo della chiave nuova non c'e', resta quello di prima
    if (v.in_vigore) mio = v.in_vigore;
    return mio.get();
}

}  // namespace m688
