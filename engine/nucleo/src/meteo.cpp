#include "nucleo/meteo.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>

#include <toml.hpp>

namespace m688 {

namespace {

// --- lettura del file ---

double num(const toml::table& t, const char* k, double d) {
    if (const auto v = t[k].value<double>()) return *v;
    if (const auto v = t[k].value<std::int64_t>()) return static_cast<double>(*v);
    return d;
}
std::string str(const toml::table& t, const char* k, const std::string& d = {}) { return t[k].value_or(d); }
bool vero(const toml::table& t, const char* k, bool d) { return t[k].value_or(d); }
std::vector<double> numeri(const toml::node* n) {
    std::vector<double> v;
    if (const auto* a = n ? n->as_array() : nullptr)
        for (const auto& x : *a) {
            if (const auto d = x.value<double>()) v.push_back(*d);
            else if (const auto i = x.value<std::int64_t>()) v.push_back(static_cast<double>(*i));
        }
    return v;
}
std::vector<double> numeri(const toml::table& t, const char* k) { return numeri(t.get(k)); }
std::vector<std::string> parole(const toml::table& t, const char* k) {
    std::vector<std::string> v;
    if (const auto* a = t[k].as_array())
        for (const auto& x : *a)
            if (const auto s = x.value<std::string>()) v.push_back(*s);
    return v;
}
template <std::size_t N>
void riempi(std::array<double, N>& a, const std::vector<double>& v) {
    for (std::size_t i = 0; i < N && i < v.size(); ++i) a[i] = v[i];
}
template <std::size_t N>
void riempi(std::array<double, N>& a, const toml::table& t, const char* k) { if (t.contains(k)) riempi(a, numeri(t, k)); }

std::optional<Nebbia> nebbia_da_nome(const std::string& s) {
    for (const Nebbia n : {Nebbia::nessuna, Nebbia::avvezione, Nebbia::fumo_di_mare, Nebbia::radiazione, Nebbia::precipitazione})
        if (s == nome_nebbia(n)) return n;
    if (s.empty()) return Nebbia::nessuna;
    return std::nullopt;
}

const char* const STAGIONI[] = {"inverno", "primavera", "estate", "autunno"};

}  // namespace

// --- scala unica ---

int classe_douglas(double hs_m) {
    static constexpr double LIMITI[] = {0.0001, 0.1, 0.5, 1.25, 2.5, 4.0, 6.0, 9.0, 14.0};
    int c = 0;
    for (const double l : LIMITI) if (hs_m >= l) ++c;
    return c;
}

double centro_douglas(int stato_mare) {
    static constexpr double CENTRI[] = {0.0, 0.05, 0.3, 0.875, 1.875, 3.25, 5.0, 7.5, 11.5, 14.0};
    return CENTRI[std::clamp(stato_mare, 0, 9)];
}

int beaufort(double vento_m_s) {
    static constexpr double LIMITI[] = {0.3, 1.6, 3.4, 5.5, 8.0, 10.8, 13.9, 17.2, 20.8, 24.5, 28.5, 32.7};
    int b = 0;
    for (const double l : LIMITI) if (vento_m_s >= l) ++b;
    return b;
}

// --- nomi ---

const char* nome_regime(Regime r) {
    static const char* const NOMI[] = {"anticiclonico", "prefrontale", "settore_caldo", "fronte_freddo", "postfrontale",
                                       "bloccato", "levante", "poniente", "vendaval", "cut_off", "bora_chiara", "bora_scura",
                                       "scirocco", "maestrale", "etesio", "temporale_autunnale", "monsone",
                                       "ciclone_esplosivo", "flusso_nebbioso", "calma"};
    const auto i = static_cast<std::size_t>(r);
    return i < std::size(NOMI) ? NOMI[i] : "";
}

std::optional<Regime> regime_da_nome(const std::string& s) {
    for (int i = 0; i <= static_cast<int>(Regime::calma); ++i)
        if (s == nome_regime(static_cast<Regime>(i))) return static_cast<Regime>(i);
    return std::nullopt;
}

const char* nome_nebbia(Nebbia n) {
    switch (n) {
        case Nebbia::nessuna: return "";
        case Nebbia::avvezione: return "avvezione";
        case Nebbia::fumo_di_mare: return "fumo_di_mare";
        case Nebbia::radiazione: return "radiazione";
        case Nebbia::precipitazione: return "precipitazione";
    }
    return "";
}

// --- stato ---

double StatoMeteo::hs_m() const { return std::hypot(hs_vento_m, hs_swell_m); }
int StatoMeteo::stato_mare() const { return classe_douglas(hs_m()); }
double StatoMeteo::schiuma() const { return std::min(1.0, 3.84e-6 * std::pow(std::max(vento_m_s, 0.0), 3.41)); }

StatoMeteo meteo_semplificato() {
    StatoMeteo s;
    s.regime = Regime::anticiclonico;
    s.vento_m_s = 5.0;
    s.raffica_m_s = 7.0;
    s.hs_vento_m = centro_douglas(3);
    s.tp_vento_s = 5.0 * std::sqrt(s.hs_vento_m);
    s.nubi_ottavi = 1.0;
    s.plafond_m = 3000.0;
    s.visibilita_m = 30000.0;
    return s;
}

// --- dati ---

const TeatroMeteo* DatiMeteo::teatro(const std::string& id) const {
    const auto it = teatri.find(id);
    return it == teatri.end() ? nullptr : &it->second;
}

DatiMeteo carica_meteo(const std::string& percorso) {
    toml::table t;
    try {
        t = toml::parse_file(percorso);
    } catch (const toml::parse_error& e) {
        throw std::runtime_error(percorso + ": " + std::string(e.description()));
    }
    const auto* g = t["generale"].as_table();
    const auto* tt = t["teatri"].as_table();
    if (!g || !tt) throw std::runtime_error(percorso + ": mancano [generale] o [teatri]");
    DatiMeteo d;
    auto& G = d.generale;
    G.douglas_limiti_m = numeri(*g, "douglas_limiti_m");
    G.douglas_centri_m = numeri(*g, "douglas_centri_m");
    G.beaufort_limiti_m_s = numeri(*g, "beaufort_limiti_m_s");
    G.hs_pm_coeff = num(*g, "hs_pm_coeff", G.hs_pm_coeff);
    G.tp_pm_coeff = num(*g, "tp_pm_coeff", G.tp_pm_coeff);
    riempi(G.tau_crescita_h, *g, "tau_crescita_h");
    riempi(G.tau_decadimento_h, *g, "tau_decadimento_h");
    riempi(G.raffica_fattore, *g, "raffica_fattore");
    riempi(G.cem_cd, *g, "cem_cd");
    riempi(G.cem_fetch, *g, "cem_fetch");
    riempi(G.cem_durata, *g, "cem_durata");
    G.koschmieder = num(*g, "koschmieder", G.koschmieder);
    riempi(G.estinzione_nebbia, *g, "estinzione_nebbia");
    riempi(G.estinzione_pioggia, *g, "estinzione_pioggia");
    riempi(G.estinzione_neve, *g, "estinzione_neve");
    riempi(G.acqua_pioggia, *g, "acqua_pioggia");
    G.spruzzi_da_beaufort = static_cast<int>(num(*g, "spruzzi_da_beaufort", G.spruzzi_da_beaufort));
    riempi(G.schiuma_monahan, *g, "schiuma_monahan");
    G.overland_tf_c = num(*g, "overland_tf_c", G.overland_tf_c);
    G.overland_classi_ppr = numeri(*g, "overland_classi_ppr");
    G.overland_classi_cm_h = numeri(*g, "overland_classi_cm_h");
    G.previsione_orizzonti_h = numeri(*g, "previsione_orizzonti_h");
    G.previsione_errore_fronti_h = numeri(*g, "previsione_errore_fronti_h");
    G.previsione_sigma_vento_m_s = numeri(*g, "previsione_sigma_vento_m_s");
    G.previsione_sigma_hs_m = numeri(*g, "previsione_sigma_hs_m");
    G.previsione_burrasche_in_ritardo = num(*g, "previsione_burrasche_in_ritardo", G.previsione_burrasche_in_ritardo);
    G.previsione_mai = parole(*g, "previsione_mai");
    G.bollettini_ore_utc = numeri(*g, "bollettini_ore_utc");
    G.lampi_colpi_media = num(*g, "lampi_colpi_media", G.lampi_colpi_media);
    G.lampi_singolo = num(*g, "lampi_singolo", G.lampi_singolo);
    G.lampi_intervallo_ms = num(*g, "lampi_intervallo_ms", G.lampi_intervallo_ms);
    riempi(G.lampi_al_minuto, *g, "lampi_al_minuto");
    G.lampi_canale_km = num(*g, "lampi_canale_km", G.lampi_canale_km);
    G.lampi_bagliore_km = num(*g, "lampi_bagliore_km", G.lampi_bagliore_km);
    G.tuono_km = num(*g, "tuono_km", G.tuono_km);
    G.tuono_ritardo_s_km = num(*g, "tuono_ritardo_s_km", G.tuono_ritardo_s_km);
    riempi(G.celle_raggio_km, *g, "celle_raggio_km");
    riempi(G.celle_vita_h, *g, "celle_vita_h");
    G.celle_radar_nm = num(*g, "celle_radar_nm", G.celle_radar_nm);
    G.nebbia_vento_max_m_s = num(*g, "nebbia_vento_max_m_s", G.nebbia_vento_max_m_s);
    riempi(G.nebbia_avvezione_vento_m_s, *g, "nebbia_avvezione_vento_m_s");
    G.fumo_di_mare_delta_c = num(*g, "fumo_di_mare_delta_c", G.fumo_di_mare_delta_c);
    riempi(G.fumo_di_mare_strato_m, *g, "fumo_di_mare_strato_m");
    riempi(G.nebbia_visibilita_km, *g, "nebbia_visibilita_km");
    G.nebbia_notte = num(*g, "nebbia_notte", G.nebbia_notte);
    G.nebbia_pomeriggio = num(*g, "nebbia_pomeriggio", G.nebbia_pomeriggio);
    G.neve_sotto_c = num(*g, "neve_sotto_c", G.neve_sotto_c);
    riempi(G.costa_nm, *g, "costa_nm");

    for (auto&& [k, nodo] : *tt) {
        const auto* r = nodo.as_table();
        if (!r) continue;
        const std::string id(k.str());
        TeatroMeteo T;
        T.correzione_burrasche = num(*r, "correzione_burrasche", 1.0);
        T.incanalamento = num(*r, "incanalamento", 1.0);
        riempi(T.fetch_km, *r, "fetch_km");
        riempi(T.foschia_km, *r, "foschia_km");
        if (const auto* sw = (*r)["swell"].as_table()) {
            riempi(T.swell_hs_m, *sw, "hs_m");
            riempi(T.swell_tp_s, *sw, "tp_s");
            riempi(T.swell_da_gradi, *sw, "da_gradi");
        }
        if (const auto* a = (*r)["temporali_giorni"].as_array())
            for (std::size_t i = 0; i < 4 && i < a->size(); ++i) riempi(T.temporali_giorni[i], numeri(a->get(i)));
        riempi(T.brezza_m_s, *r, "brezza_m_s");
        if (const auto* c = (*r)["centro"].as_table()) { T.lat = num(*c, "lat", 0.0); T.lon = num(*c, "lon", 0.0); }
        for (const double a : numeri(*r, "inverni_rigidi")) T.inverni_rigidi.push_back(static_cast<int>(a));
        const auto* mesi = (*r)["mesi"].as_table();
        if (!mesi) throw std::runtime_error(percorso + ": " + id + ": mancano i mesi");
        for (int m = 1; m <= 12; ++m) {
            const auto* x = (*mesi)[std::to_string(m)].as_table();
            if (!x) throw std::runtime_error(percorso + ": " + id + ": manca il mese " + std::to_string(m));
            MeseMeteo& M = T.mesi[static_cast<std::size_t>(m - 1)];
            M.vento_media_m_s = num(*x, "vento_media_m_s", M.vento_media_m_s);
            M.vento_p10_m_s = num(*x, "vento_p10_m_s", M.vento_p10_m_s);
            M.vento_p50_m_s = num(*x, "vento_p50_m_s", M.vento_p50_m_s);
            M.vento_p90_m_s = num(*x, "vento_p90_m_s", M.vento_p90_m_s);
            M.weibull_k = num(*x, "weibull_k", M.weibull_k);
            M.weibull_c_m_s = num(*x, "weibull_c_m_s", M.weibull_c_m_s);
            riempi(M.rosa_pct, *x, "rosa_pct");
            M.bft7_pct = num(*x, "bft7_pct", 0.0);
            M.bft8_pct = num(*x, "bft8_pct", 0.0);
            M.raffica_fattore = num(*x, "raffica_fattore", M.raffica_fattore);
            M.hs_media_m = num(*x, "hs_media_m", M.hs_media_m);
            M.hs_p90_m = num(*x, "hs_p90_m", M.hs_p90_m);
            M.tp_media_s = num(*x, "tp_media_s", M.tp_media_s);
            riempi(M.douglas_pct, *x, "douglas_pct");
            M.ghiaccio_pct = num(*x, "ghiaccio_pct", 0.0);
            M.t_aria_c = num(*x, "t_aria_c", M.t_aria_c);
            M.t_mare_c = num(*x, "t_mare_c", M.t_mare_c);
            M.nubi_pct = num(*x, "nubi_pct", M.nubi_pct);
            M.coperto_pct = num(*x, "coperto_pct", M.coperto_pct);
            M.sereno_pct = num(*x, "sereno_pct", M.sereno_pct);
            M.nubi_basse_pct = num(*x, "nubi_basse_pct", M.nubi_basse_pct);
            M.precipitazione_pct = num(*x, "precipitazione_pct", M.precipitazione_pct);
            M.intensita_mm_h = num(*x, "intensita_mm_h", M.intensita_mm_h);
            M.precipitazione_forte_pct = num(*x, "precipitazione_forte_pct", M.precipitazione_forte_pct);
            M.neve_pct = num(*x, "neve_pct", 0.0);
            M.ur97_pct = num(*x, "ur97_pct", 0.0);
            M.fumo_di_mare_pct = num(*x, "fumo_di_mare_pct", 0.0);
            M.spruzzo_moderato_pct = num(*x, "spruzzo_moderato_pct", 0.0);
            M.spruzzo_forte_pct = num(*x, "spruzzo_forte_pct", 0.0);
            M.pressione_hpa = num(*x, "pressione_hpa", M.pressione_hpa);
            for (double& v : M.douglas_pct) if (std::isnan(v)) v = 0.0;
        }
        if (const auto* st = (*r)["stagioni"].as_table())
            for (const char* nome : STAGIONI)
                if (const auto* x = (*st)[nome].as_table()) {
                    StagioneMeteo S;
                    S.tau_vento_h = num(*x, "tau_vento_h", S.tau_vento_h);
                    S.tau_onda_h = num(*x, "tau_onda_h", S.tau_onda_h);
                    S.ritardo_onda_h = num(*x, "ritardo_onda_h", S.ritardo_onda_h);
                    if (const auto* a = (*x)["markov6h_pct"].as_array())
                        for (std::size_t i = 0; i < 4 && i < a->size(); ++i) riempi(S.markov6h_pct[i], numeri(a->get(i)));
                    riempi(S.quote_classi_pct, *x, "quote_classi_pct");
                    S.burrasche_bft8_per_stagione = num(*x, "burrasche_bft8_per_stagione", 0.0);
                    S.durata_bft8_h = num(*x, "durata_bft8_h", 0.0);
                    S.episodi_bft7_per_stagione = num(*x, "episodi_bft7_per_stagione", 0.0);
                    S.durata_bft7_h = num(*x, "durata_bft7_h", 0.0);
                    S.intervallo_minimi_giorni = num(*x, "intervallo_minimi_giorni", S.intervallo_minimi_giorni);
                    S.diurno_ampiezza_m_s = num(*x, "diurno_ampiezza_m_s", S.diurno_ampiezza_m_s);
                    S.diurno_ora_massimo = static_cast<int>(num(*x, "diurno_ora_massimo", S.diurno_ora_massimo));
                    if (const auto* q = (*x)["regimi"].as_table())
                        for (auto&& [rk, rv] : *q) S.regimi[std::string(rk.str())] = rv.value_or(0.0);
                    T.stagioni[nome] = std::move(S);
                }
        if (const auto* rg = (*r)["regimi"].as_table())
            for (auto&& [rk, rn] : *rg) {
                const auto* x = rn.as_table();
                if (!x) continue;
                const std::string nome(rk.str());
                if (!regime_da_nome(nome)) throw std::runtime_error(percorso + ": " + id + ": regime sconosciuto '" + nome + "'");
                RegimeMeteo R;
                R.vento_fattore = num(*x, "vento_fattore", 1.0);
                R.da_gradi = num(*x, "da_gradi", -1.0);
                R.nubi_pct = num(*x, "nubi_pct", R.nubi_pct);
                R.pioggia_ora = num(*x, "pioggia_ora", R.pioggia_ora);
                R.intensita_mm_h = num(*x, "intensita_mm_h", R.intensita_mm_h);
                R.visibilita_km = num(*x, "visibilita_km", R.visibilita_km);
                riempi(R.anomalia_t_c, *x, "anomalia_t_c");
                R.durata_h = num(*x, "durata_h", R.durata_h);
                R.temporale_ora = num(*x, "temporale_ora", 0.0);
                const auto nb = nebbia_da_nome(str(*x, "nebbia"));
                if (!nb) throw std::runtime_error(percorso + ": " + id + "." + nome + ": nebbia sconosciuta");
                R.nebbia = *nb;
                R.prossimi = parole(*x, "prossimi");
                R.incanalamento = num(*x, "incanalamento", 1.0);
                R.esordio_h = num(*x, "esordio_h", 0.0);
                R.polvere = vero(*x, "polvere", false);
                R.sequenza_da_gradi = numeri(*x, "sequenza_da_gradi");
                T.regimi[nome] = std::move(R);
            }
        for (const auto& [nome, R] : T.regimi)
            for (const auto& p : R.prossimi)
                if (!T.regimi.count(p)) throw std::runtime_error(percorso + ": " + id + "." + nome + ": prossimo sconosciuto '" + p + "'");
        if (const auto* ev = (*r)["eventi"].as_table())
            for (auto&& [ek, en] : *ev) {
                const auto* x = en.as_table();
                if (!x) continue;
                EventoMeteo E;
                for (const double m : numeri(*x, "mesi")) E.mesi.push_back(static_cast<int>(m));
                E.tasso_al_mese = num(*x, "tasso_al_mese", 0.0);
                riempi(E.durata_h, *x, "durata_h");
                E.vento_m_s = numeri(*x, "vento_m_s");
                E.hs_m = numeri(*x, "hs_m");
                E.raggio_km = num(*x, "raggio_km", 0.0);
                E.traslazione_m_s = num(*x, "traslazione_m_s", 0.0);
                E.esordio_h = numeri(*x, "esordio_h");
                E.prevista = vero(*x, "prevista", true);
                E.aria_max_c = num(*x, "aria_max_c", 99.0);
                E.per_giorno_di_temporale = num(*x, "per_giorno_di_temporale", 0.0);
                E.mese_massimo = static_cast<int>(num(*x, "mese_massimo", 0.0));
                T.eventi[std::string(ek.str())] = std::move(E);
            }
        if (const auto* pa = (*r)["punti"].as_array())
            for (const auto& pn : *pa)
                if (const auto* x = pn.as_table()) {
                    PuntoMeteo P;
                    P.nome = str(*x, "nome");
                    P.lat = num(*x, "lat", 0.0);
                    P.lon = num(*x, "lon", 0.0);
                    riempi(P.vento_media_m_s, *x, "vento_media_m_s");
                    riempi(P.hs_media_m, *x, "hs_media_m");
                    riempi(P.ghiaccio_pct, *x, "ghiaccio_pct");
                    riempi(P.ur97_pct, *x, "ur97_pct");
                    riempi(P.t_aria_c, *x, "t_aria_c");
                    T.punti.push_back(std::move(P));
                }
        d.teatri[id] = std::move(T);
    }
    return d;
}

// --- il generatore (dossier 21 §4) ---

namespace {

constexpr double G = 9.81;
constexpr long FINESTRA_ORE = 200;           // ore tenute dietro all'ultima chiesta (tendenza, lampi, previsione)
constexpr double BETA_REGIME = 0.6;
constexpr double SALTO_REGIME = 0.1;
constexpr double TAU_VELOCE_H = 4.0;         // stadio veloce del vento (STIMA)         // transizioni fuori dai `prossimi`: tutti i regimi raggiungibili (STIMA)          // quota del regime nell'anomalia del vento (STIMA)
constexpr double SIGMA_DIREZIONE = 25.0;     // gradi, AR(1) della direzione attorno a quella del regime (STIMA)
constexpr double TAU_SWELL_H = 48.0, TAU_ARIA_H = 72.0, TAU_MARE_H = 240.0, TAU_PRESSIONE_H = 24.0;
constexpr double SIGMA_ARIA_C = 1.5, SIGMA_MARE_C = 0.5, SIGMA_PRESSIONE_HPA = 4.0;
constexpr double PERSISTENZA_PIOGGIA = 0.6;  // autocorrelazione oraria della catena asciutto/bagnato (STIMA)
constexpr double FRAZIONE_NUBE_MARE = 0.25;  // dei lampi che toccano il mare (STIMA)
constexpr double RIQUADRO_M = 100000.0;      // mezzo lato del riquadro delle celle (STIMA)

Vec2 operator+(Vec2 a, Vec2 b) { return {a.x + b.x, a.y + b.y}; }
Vec2 operator-(Vec2 a, Vec2 b) { return {a.x - b.x, a.y - b.y}; }
Vec2 operator*(Vec2 a, double k) { return {a.x * k, a.y * k}; }

std::uint64_t mescola(std::uint64_t x) {
    x += 0x9e3779b97f4a7c15ull;
    x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ull;
    x = (x ^ (x >> 27)) * 0x94d049bb133111ebull;
    return x ^ (x >> 31);
}

double gauss(Casuale& c) {
    const double u1 = std::max(c.uniforme(), 1e-300), u2 = c.uniforme();
    return std::sqrt(-2.0 * std::log(u1)) * std::cos(2.0 * PI * u2);
}

double phi(double z) { return 0.5 * std::erfc(-z / std::sqrt(2.0)); }

// gamma(k, theta) di Marsaglia e Tsang
double gamma_casuale(Casuale& c, double k, double theta) {
    if (k < 1.0) return gamma_casuale(c, k + 1.0, theta) * std::pow(std::max(c.uniforme(), 1e-300), 1.0 / k);
    const double d = k - 1.0 / 3.0, cc = 1.0 / std::sqrt(9.0 * d);
    for (;;) {
        double x, v;
        do { x = gauss(c); v = 1.0 + cc * x; } while (v <= 0.0);
        v = v * v * v;
        const double u = c.uniforme();
        if (u < 1.0 - 0.0331 * x * x * x * x || std::log(std::max(u, 1e-300)) < 0.5 * x * x + d * (1.0 - v + std::log(v))) return d * v * theta;
    }
}

int poisson(Casuale& c, double media) {
    if (media <= 0.0) return 0;
    const double l = std::exp(-media);
    int k = 0;
    for (double p = c.uniforme(); p > l; p *= c.uniforme()) ++k;
    return k;
}

int stagione_di(int mese) { return mese == 12 || mese <= 2 ? 0 : mese <= 5 ? 1 : mese <= 8 ? 2 : 3; }

double angolo(double a) { return std::fmod(std::fmod(a, 360.0) + 360.0, 360.0); }
double differenza(double a, double b) { return std::fmod(a - b + 540.0, 360.0) - 180.0; }   // a - b in (-180, 180]

// pressione rispetto alla media del mese per regime (STIMA, dossier 21 §2: scende prima del fronte)
double scarto_pressione(Regime r) {
    switch (r) {
        case Regime::anticiclonico: return 12.0;
        case Regime::prefrontale: return -4.0;
        case Regime::settore_caldo: return -8.0;
        case Regime::fronte_freddo: return -12.0;
        case Regime::postfrontale: return -2.0;
        case Regime::bloccato: return 10.0;
        case Regime::levante: return 4.0;
        case Regime::poniente: return 2.0;
        case Regime::vendaval: return -8.0;
        case Regime::cut_off: return -6.0;
        case Regime::bora_chiara: return 8.0;
        case Regime::bora_scura: return -2.0;
        case Regime::scirocco: return -8.0;
        case Regime::maestrale: return 4.0;
        case Regime::etesio: return 3.0;
        case Regime::temporale_autunnale: return -4.0;
        case Regime::monsone: return 10.0;
        case Regime::ciclone_esplosivo: return -25.0;
        case Regime::flusso_nebbioso: return 2.0;
        case Regime::calma: return 5.0;
    }
    return 0.0;
}

}  // namespace

struct Meteo::Processi {
    // un generatore per processo: aggiungerne uno non cambia gli altri
    Casuale regime, vento, direzione, swell, nubi, pioggia, temperatura, pressione, nebbia, celle, eventi;
    explicit Processi(std::uint64_t s)
        : regime(mescola(s ^ 0x01)), vento(mescola(s ^ 0x02)), direzione(mescola(s ^ 0x03)), swell(mescola(s ^ 0x04)),
          nubi(mescola(s ^ 0x05)), pioggia(mescola(s ^ 0x06)), temperatura(mescola(s ^ 0x07)), pressione(mescola(s ^ 0x08)),
          nebbia(mescola(s ^ 0x09)), celle(mescola(s ^ 0x0A)), eventi(mescola(s ^ 0x0B)) {}
    // regime corrente
    std::string regime_id, regime_prima;
    double regime_fine_h = 0.0, regime_inizio_h = 0.0;
    double z_vento = 0.0, z_veloce = 0.0, z_direzione = 0.0, z_swell = 0.0, z_nubi = 0.0, z_aria = 0.0, z_mare = 0.0, z_pressione = 0.0;
    double hs_vento = 0.5, da_regime = 270.0;
    bool bagnato = false, in_nebbia = false;
    double v_nebbia_km = 0.3;
    // evento raro corrente
    std::string evento;
    double evento_inizio_h = 0.0, evento_fine_h = -1.0, evento_vento = 0.0, evento_hs = 0.0;
    long ora = -1;                       // l'ultima generata
    // tarati all'avvio per il mese: coda del vento (probabilita' di superare, ogni 0,05 m/s), mare, nebbia
    std::vector<double> supera;
    double scala_onde = 1.0, swell_media = 0.0;
    double scala_nebbia = 1.0, scala_bagnato = 1.0;
    std::map<std::string, double> pesi;  // dei regimi nelle transizioni: le quote nel tempo come quelle della stagione
    std::map<std::string, double> quote; // quote nel tempo che ne vengono (stazionaria del semi-Markov)
    std::vector<double> z_ordinati;      // l'anomalia del vento della taratura, ordinata: il rango da' la probabilita'
    // i conti della corsa di taratura
    long n_ore = 0, ore_nebbia = 0, ore_bagnate = 0;
    std::vector<double> z_visti;
};

Meteo::~Meteo() = default;

Meteo::Meteo(const DatiMeteo& dati, const std::string& teatro, int mese, double ora_locale, std::uint64_t seme, bool taratura)
    : Meteo(dati, teatro, mese, ora_locale, seme, taratura, 0) {}

Meteo::Meteo(const DatiMeteo& dati, const std::string& teatro, int mese, double ora_locale, std::uint64_t seme)
    : Meteo(dati, teatro, mese, ora_locale, seme, false, 0) {
    avanza(0.0);
}

Meteo::Meteo(const DatiMeteo& dati, const std::string& teatro, int mese, double ora_locale, std::uint64_t seme, bool taratura, int)
    : dati_(&dati), teatro_(dati.teatro(teatro)), id_teatro_(teatro), mese_(std::clamp(mese, 1, 12)),
      ora_locale_(ora_locale), seme_(seme), taratura_(taratura) {
    if (teatro_) { lat0_ = teatro_->lat; lon0_ = teatro_->lon; }
    if (!teatro_) fisso_ = meteo_semplificato();
}

void Meteo::imposta_origine(double lat, double lon) { lat0_ = lat; lon0_ = lon; }

void Meteo::imposta_anno(int anno) {
    anno_ = anno;
    processi_.reset();   // il ghiaccio cambia: si rigenera da capo
    ore_.clear();
    celle_.clear();
    prima_ora_ = 0;
    avanza(t_);
}

void Meteo::fissa(const StatoMeteo& s) {
    fisso_ = s;
    stato_ = s;
    ghiaccio_dei_punti_ = false;
}

void Meteo::fissa_sopra(const StatoMeteo& s) {
    const bool tieni = !fisso_ || ghiaccio_dei_punti_;   // sopra un meteo fisso dell'autore o del semplificato: uniforme com'era
    fissa(s);
    ghiaccio_dei_punti_ = tieni;
}

const Meteo::Ora& Meteo::ora(long h) const {
    h = std::max(h, prima_ora_);
    genera_fino(h);
    return ore_[static_cast<std::size_t>(h - prima_ora_)];
}

// Una nuova ora dello stato sinottico: regime, vento, onde, nubi, pioggia, temperature, pressione, nebbia, eventi, celle
void Meteo::genera_fino(long fino) const {
    if (!teatro_) return;
    const TeatroMeteo& T = *teatro_;
    const GeneraleMeteo& Gm = dati_->generale;
    const MeseMeteo& M = T.mesi[static_cast<std::size_t>(mese_ - 1)];
    const int si = stagione_di(mese_);
    static const char* const NOMI_STAGIONI[] = {"inverno", "primavera", "estate", "autunno"};
    const auto st_it = T.stagioni.find(NOMI_STAGIONI[si]);
    static const StagioneMeteo nessuna;
    const StagioneMeteo& S = st_it != T.stagioni.end() ? st_it->second : nessuna;
    if (!processi_) {
        processi_ = std::make_unique<Processi>(seme_);
        auto& P = *processi_;
        // partenza: un regime dalle quote della stagione, gli AR(1) dalla loro distribuzione stazionaria
        P.z_vento = gauss(P.vento);
        P.z_direzione = gauss(P.direzione);
        P.z_swell = gauss(P.swell);
        P.z_nubi = gauss(P.nubi);
        P.z_aria = gauss(P.temperatura);
        P.z_mare = gauss(P.temperatura);
        P.z_pressione = gauss(P.pressione);
        P.ora = -1;
    }
    auto& P = *processi_;
    if (P.supera.empty()) {
        // regimi: i pesi delle transizioni (fra i `prossimi`) tali che la quota stazionaria del semi-Markov
        // (frequenza di visita per durata media) sia quella della stagione
        for (const auto& [n, q] : S.regimi) if (T.regimi.count(n)) P.pesi[n] = q;
        for (int it = 0; it < 60; ++it) {
            std::map<std::string, double> nu, prossimo;
            for (const auto& [n, w] : P.pesi) nu[n] = 1.0 / static_cast<double>(P.pesi.size());
            for (int k = 0; k < 200; ++k) {   // stazionaria della catena incorporata
                for (auto& [n, x] : prossimo) x = 0.0;
                double tutti = 0.0;
                for (const auto& [m, w] : P.pesi) tutti += w;
                for (const auto& [n, x] : nu) {
                    const auto& pr = T.regimi.at(n).prossimi;
                    double tot = 0.0;
                    for (const auto& m : pr) if (P.pesi.count(m)) tot += P.pesi.at(m);
                    const double salto = tot > 0.0 ? SALTO_REGIME : 1.0;
                    for (const auto& m : pr) if (P.pesi.count(m) && tot > 0.0) prossimo[m] += x * (1.0 - salto) * P.pesi.at(m) / tot;
                    for (const auto& [m, w] : P.pesi) if (tutti > 0.0) prossimo[m] += x * salto * w / tutti;
                }
                nu = prossimo;
            }
            double tot = 0.0;
            std::map<std::string, double> pi;
            for (const auto& [n, x] : nu) { pi[n] = x * T.regimi.at(n).durata_h; tot += pi[n]; }
            if (tot <= 0.0) break;
            for (auto& [n, x] : pi) P.quote[n] = x / tot;
            for (auto& [n, w] : P.pesi) {
                const double q = S.regimi.count(n) ? S.regimi.at(n) : 0.0, p = pi[n] / tot;
                if (q <= 0.0) w = 0.0;
                else if (p > 1e-9) w *= std::pow(q / p, 0.5);
            }
        }
        // vento: Weibull del mese con la coda tarata su Bft 7 (dati) e Bft 8 (dati per la correzione delle burrasche,
        // che ERA5 sottostima, §3.3); monotona per costruzione
        const double k = std::max(M.weibull_k, 0.5), c = std::max(M.weibull_c_m_s, 0.5);
        const auto sw = [&](double u) { return std::exp(-std::pow(std::max(u, 0.0) / c, k)); };
        const double f7 = M.bft7_pct > 0.0 ? (M.bft7_pct / 100.0) / std::max(sw(13.9), 1e-12) : 1.0;
        const double f8 = std::max(std::max(T.correzione_burrasche, 1.0) * (M.bft8_pct / 100.0), 1e-6) / std::max(sw(17.2), 1e-12);
        const auto f = [&](double u) {
            if (u <= 10.8) return 1.0;
            if (u <= 13.9) return std::exp(std::log(f7) * (u - 10.8) / (13.9 - 10.8));
            if (u <= 17.2) return std::exp(std::log(f7) + (std::log(f8) - std::log(f7)) * (u - 13.9) / (17.2 - 13.9));
            return f8;
        };
        P.supera.resize(1201);
        double minimo = 1.0;
        for (std::size_t i = 0; i < P.supera.size(); ++i) {
            const double u = 0.05 * static_cast<double>(i);
            minimo = std::min(minimo, std::min(1.0, sw(u) * f(u)));
            P.supera[i] = minimo;
        }
        // il mare: una corsa breve senza taratura da' il mare da vento del mese; scala e swell perche' la Hs media sia
        // quella di ERA5 (§2: Hs = sqrt(vento^2 + swell^2))
        if (!taratura_) {
            Meteo prova(*dati_, id_teatro_, mese_, ora_locale_, mescola(seme_ ^ 0xCA11B4A7Eull), true);
            // l'anno non si copia: si tara il mare senza il ghiaccio degli inverni rigidi
            prova.genera_fino(20000);
            std::vector<double> hw;
            for (std::size_t i = 200; i < prova.ore_.size(); ++i) hw.push_back(prova.ore_[i].vero.hs_vento_m);
            double media = 0.0;
            for (const double x : hw) media += x;
            media /= static_cast<double>(std::max<std::size_t>(hw.size(), 1));
            const double obiettivo = M.hs_media_m * (1.0 - M.ghiaccio_pct / 100.0);
            const double minimo_swell = T.swell_hs_m[0] * 0.8;   // lo swell oceanico tiene il mare sopra il calmo (§3.2)
            static constexpr double LOGNORMALE[] = {0.62, 0.74, 0.83, 0.91, 0.98, 1.06, 1.15, 1.27, 1.48};   // decili, sigma 0,3
            const auto con_swell = [&](double a, double sm) {
                double e = 0.0;
                for (const double x : hw)
                    for (const double q : LOGNORMALE) e += std::hypot(a * x, sm * q);
                return e / static_cast<double>(std::max<std::size_t>(hw.size(), 1) * std::size(LOGNORMALE));
            };
            if (media > 0.0 && con_swell(1.0, minimo_swell) > obiettivo) {
                double lo = 0.0, hi = 1.0;   // il mare da vento troppo alto (fetch): si scala
                for (int i = 0; i < 40; ++i) { const double m = 0.5 * (lo + hi); (con_swell(m, minimo_swell) > obiettivo ? hi : lo) = m; }
                P.scala_onde = 0.5 * (lo + hi);
                P.swell_media = minimo_swell;
            } else {
                double lo = minimo_swell, hi = std::max(obiettivo, minimo_swell);
                for (int i = 0; i < 40; ++i) { const double m = 0.5 * (lo + hi); (con_swell(1.0, m) > obiettivo ? hi : lo) = m; }
                P.swell_media = 0.5 * (lo + hi);
            }
            auto& Q = *prova.processi_;
            const double n = static_cast<double>(std::max<long>(Q.n_ore, 1));
            P.z_ordinati = std::move(Q.z_visti);
            std::sort(P.z_ordinati.begin(), P.z_ordinati.end());
            // nebbia (UR >= 97% e fumo di mare) e ore bagnate come nei dati
            const double nebbia_dati = (M.ur97_pct + M.fumo_di_mare_pct) / 100.0, bagnate_dati = M.precipitazione_pct / 100.0;
            P.scala_nebbia = Q.ore_nebbia > 0 ? std::clamp(nebbia_dati * n / static_cast<double>(Q.ore_nebbia), 0.05, 5.0) : 1.0;
            P.scala_bagnato = Q.ore_bagnate > 0 ? std::clamp(bagnate_dati * n / static_cast<double>(Q.ore_bagnate), 0.2, 5.0) : 1.0;
        }
    }
    // statistiche dei regimi nella stagione: media e sigma di ln(vento_fattore), media della pioggia e dei temporali
    double mu = 0.0, m2 = 0.0, pioggia_media = 0.0, intensita_media = 0.0, temporale_medio = 0.0, peso = 0.0;
    // con le quote che vengono davvero dal semi-Markov (P.quote), non quelle nominali
    for (const auto& [nome, q] : P.quote) {
        const auto r = T.regimi.find(nome);
        if (r == T.regimi.end() || q <= 0.0) continue;
        const double l = std::log(std::max(r->second.vento_fattore, 0.05));
        mu += q * l; m2 += q * l * l;
        pioggia_media += q * r->second.pioggia_ora;
        intensita_media += q * r->second.pioggia_ora * r->second.intensita_mm_h;
        temporale_medio += q * r->second.temporale_ora;
        peso += q;
    }
    double quota_nebbia = 0.0;
    for (const auto& [nome, q] : P.quote)
        if (const auto r = T.regimi.find(nome); r != T.regimi.end() && r->second.nebbia != Nebbia::nessuna) quota_nebbia += q;
    if (peso > 0.0) quota_nebbia /= peso;
    if (peso > 0.0) { mu /= peso; m2 /= peso; pioggia_media /= peso; intensita_media /= peso; temporale_medio /= peso; }
    const double sigma = std::sqrt(std::max(m2 - mu * mu, 1e-6));
    const double scala_pioggia = pioggia_media > 0.0 ? (M.precipitazione_pct / 100.0) / pioggia_media : 0.0;
    const double scala_intensita = intensita_media > 0.0 ? M.intensita_mm_h * pioggia_media / intensita_media : 1.0;
    // temporali: giorni medi della stagione su ~91 giorni, due celle per giorno di temporale (STIMA)
    const double giorni_temporale = 0.5 * (T.temporali_giorni[static_cast<std::size_t>(si)][0] + T.temporali_giorni[static_cast<std::size_t>(si)][1]);
    const double celle_ora = giorni_temporale * 2.0 / (91.0 * 24.0);
    const double scala_temporali = temporale_medio > 0.0 ? celle_ora / temporale_medio : 0.0;
    const auto quantile = [&](double p) {   // la tabella al contrario: la prima velocita' con superamento <= 1 - p
        const double q = 1.0 - p;
        const auto it = std::lower_bound(P.supera.begin(), P.supera.end(), q, [](double x, double y) { return x > y; });
        if (it == P.supera.begin()) return 0.0;
        if (it == P.supera.end()) return 0.05 * static_cast<double>(P.supera.size() - 1);
        const std::size_t i = static_cast<std::size_t>(it - P.supera.begin());
        const double a = P.supera[i - 1], b = P.supera[i];
        const double f = a > b ? (a - q) / (a - b) : 0.0;
        return 0.05 * (static_cast<double>(i - 1) + f);
    };
    const double swell_media = P.swell_media;
    const auto scegli_regime = [&](const std::vector<std::string>& candidati) {
        double tot = 0.0;
        const bool primo = P.regime_id.empty();   // il primo dalle quote, poi coi pesi delle transizioni
        const auto peso_di = [&](const std::string& n) {
            if (primo) { const auto q = S.regimi.find(n); return q != S.regimi.end() ? q->second : 0.0; }
            const auto q = P.pesi.find(n);
            return q != P.pesi.end() ? q->second : 0.0;
        };
        for (const auto& n : candidati) tot += peso_di(n);
        double x = P.regime.uniforme() * tot;
        for (const auto& n : candidati) {
            const double q = peso_di(n);
            x -= q;
            if (x <= 0.0 && q > 0.0) return n;
        }
        // nessuna quota: il primo con quota, altrimenti il primo dei candidati
        for (const auto& [n, q] : S.regimi) if (q > 0.0) return n;
        return candidati.empty() ? std::string(T.regimi.begin()->first) : candidati.front();
    };

    while (P.ora < fino) {
        const long h = ++P.ora;
        const double hd = static_cast<double>(h);
        // 1. regime sinottico, semi-Markov con durate gamma (forma 3, STIMA)
        if (P.regime_id.empty() || hd >= P.regime_fine_h) {
            std::vector<std::string> candidati;
            if (P.regime_id.empty()) for (const auto& [n, q] : S.regimi) candidati.push_back(n);
            else candidati = T.regimi.at(P.regime_id).prossimi;
            if (!P.regime_id.empty() && P.regime.uniforme() < SALTO_REGIME) {   // un salto verso un regime qualunque
                candidati.clear();
                for (const auto& [n, r] : T.regimi) candidati.push_back(n);
            }
            if (candidati.empty()) for (const auto& [n, r] : T.regimi) candidati.push_back(n);
            P.regime_prima = P.regime_id.empty() ? scegli_regime(candidati) : P.regime_id;
            P.regime_id = scegli_regime(candidati);
            const RegimeMeteo& R = T.regimi.at(P.regime_id);
            P.regime_inizio_h = hd;
            P.regime_fine_h = hd + std::max(2.0, gamma_casuale(P.regime, 3.0, R.durata_h / 3.0));
            if (R.da_gradi >= 0.0) P.da_regime = R.da_gradi;
            else {   // dalla rosa del mese
                double tot = 0.0;
                for (const double x : M.rosa_pct) tot += x;
                double x = P.regime.uniforme() * tot;
                int settore = 0;
                for (; settore < 7; ++settore) { x -= M.rosa_pct[static_cast<std::size_t>(settore)]; if (x <= 0.0) break; }
                P.da_regime = 45.0 * settore + P.regime.uniforme(-22.5, 22.5);
            }
        }
        const RegimeMeteo& R = T.regimi.at(P.regime_id);
        const RegimeMeteo& Rp = T.regimi.count(P.regime_prima) ? T.regimi.at(P.regime_prima) : R;
        const double esordio = R.esordio_h > 0.0 ? R.esordio_h : 3.0;
        const double w = std::clamp((hd - P.regime_inizio_h) / esordio, 0.0, 1.0);   // passaggio dal regime di prima
        const double l_regime = (1.0 - w) * std::log(std::max(Rp.vento_fattore, 0.05)) + w * std::log(std::max(R.vento_fattore, 0.05));

        // 2. vento: AR(1) nello spazio gaussiano, regime, quantile mapping
        // due stadi (un AR(1) lento alimentato da uno veloce di TAU_VELOCE_H): liscio alle brevi distanze, con la
        // persistenza misurata; la varianza la rimette a posto la mappa per ranghi
        const double fr = std::exp(-1.0 / TAU_VELOCE_H), fv = std::exp(-1.0 / std::max(S.tau_vento_h - TAU_VELOCE_H, 2.0));
        P.z_veloce = fr * P.z_veloce + std::sqrt(1.0 - fr * fr) * gauss(P.vento);
        P.z_vento = fv * P.z_vento + std::sqrt(1.0 - fv * fv) * P.z_veloce * std::sqrt((1.0 - fv * fr) / (1.0 + fv * fr));
        const double z = std::sqrt(1.0 - BETA_REGIME * BETA_REGIME) * P.z_vento + BETA_REGIME * (l_regime - mu) / sigma;
        if (h >= 200) ++P.n_ore;
        if (taratura_ && h >= 200) P.z_visti.push_back(z);
        double pz = phi(z);
        if (P.z_ordinati.size() > 100) {   // la distribuzione vera dell'anomalia (regimi compresi): il vento del mese
            const auto it = std::lower_bound(P.z_ordinati.begin(), P.z_ordinati.end(), z);
            const double n = static_cast<double>(P.z_ordinati.size());
            pz = std::clamp((static_cast<double>(it - P.z_ordinati.begin()) + 0.5) / n, 0.5 / n, 1.0 - 0.5 / n);
        }
        double u = quantile(pz);
        // ciclo diurno (debole in mare aperto)
        const double ora_loc = std::fmod(ora_locale_ + hd, 24.0);
        u = std::max(0.0, u + S.diurno_ampiezza_m_s * std::cos(2.0 * PI * (ora_loc - S.diurno_ora_massimo) / 24.0));
        const double fd = std::exp(-1.0 / 6.0);
        P.z_direzione = fd * P.z_direzione + std::sqrt(1.0 - fd * fd) * gauss(P.direzione);
        double da = P.da_regime;
        if (R.sequenza_da_gradi.size() >= 2) da = (hd - P.regime_inizio_h) < 0.5 * (P.regime_fine_h - P.regime_inizio_h) ? R.sequenza_da_gradi[0] : R.sequenza_da_gradi[1];
        da = angolo(da + SIGMA_DIREZIONE * P.z_direzione);

        // 3. eventi rari: Poisson orari nei loro mesi; sovrascrivono il regime per la loro durata
        StatoMeteo base;
        if (hd >= P.evento_fine_h) {
            P.evento.clear();
            for (const auto& [nome, E] : T.eventi) {
                if (std::find(E.mesi.begin(), E.mesi.end(), mese_) == E.mesi.end()) continue;
                double tasso = E.tasso_al_mese / (30.0 * 24.0);
                if (E.mese_massimo > 0) tasso *= mese_ == E.mese_massimo ? 1.5 : 0.9;
                if (P.eventi.uniforme() >= tasso) continue;
                P.evento = nome;
                P.evento_inizio_h = hd;
                P.evento_fine_h = hd + P.eventi.uniforme(E.durata_h[0], std::max(E.durata_h[0], E.durata_h[1]));
                P.evento_vento = E.vento_m_s.size() >= 2 ? (E.vento_m_s[1] < E.vento_m_s[0] ? E.vento_m_s[0] + E.vento_m_s[1] * gauss(P.eventi)
                                                                                           : P.eventi.uniforme(E.vento_m_s[0], E.vento_m_s[1]))
                                                        : 0.0;
                P.evento_hs = E.hs_m.size() >= 2 ? P.eventi.uniforme(E.hs_m[0], E.hs_m[1]) : 0.0;
                break;
            }
        }
        const std::string& ev = P.evento;
        const double rampa_evento = ev.empty() ? 0.0 : [&] {
            const auto& E = T.eventi.at(ev);
            const double es = E.esordio_h.empty() ? 1.0 : E.esordio_h[0];
            return std::clamp((hd - P.evento_inizio_h + 1.0) / std::max(es, 0.5), 0.0, 1.0);
        }();

        // 4. onde: mare da vento che rilassa verso l'equilibrio col fetch (CEM); swell AR(1) lognormale
        const auto mare_da = [&](double vento, double da_gradi, double hs_prec) {
            const double cd = Gm.cem_cd[0] * (Gm.cem_cd[1] + Gm.cem_cd[2] * vento);
            const double ustar = vento * std::sqrt(std::max(cd, 1e-6));
            // sopra i 10 m/s il mare non fa in tempo a svilupparsi: Hs cresce come U^1,6 (STIMA dalle Hs mediane per
            // classe di vento della nota A §6.2)
            const double hs_pm = Gm.hs_pm_coeff * vento * vento / G * (vento > 10.0 ? std::pow(10.0 / vento, 0.4) : 1.0);
            const double fetch = T.fetch_km[static_cast<std::size_t>(static_cast<int>(std::floor(angolo(da_gradi + 22.5) / 45.0)) % 8)];
            const double hs_fetch = fetch > 0.0 ? Gm.cem_fetch[0] * ustar * std::sqrt(fetch * 1000.0 / G) : hs_pm;
            const double eq = std::min(hs_pm, hs_fetch) * P.scala_onde;
            // crescita coi suoi tempi; decadimento lento come la persistenza misurata del mare (ERA5, tau_onda_h)
            const double tau = eq > hs_prec ? 0.5 * (Gm.tau_crescita_h[0] + Gm.tau_crescita_h[1])
                                            : std::max(0.5 * (Gm.tau_decadimento_h[0] + Gm.tau_decadimento_h[1]), 0.6 * S.tau_onda_h);
            return eq + (hs_prec - eq) * std::exp(-1.0 / tau);
        };
        const double fs = std::exp(-1.0 / TAU_SWELL_H);
        P.z_swell = fs * P.z_swell + std::sqrt(1.0 - fs * fs) * gauss(P.swell);
        const double hs_swell = swell_media * std::exp(0.3 * P.z_swell - 0.5 * 0.3 * 0.3);
        // 5. nubi e precipitazione: catena asciutto/bagnato col regime, intensita' gamma
        const double fn = std::exp(-1.0 / 6.0);
        P.z_nubi = fn * P.z_nubi + std::sqrt(1.0 - fn * fn) * gauss(P.nubi);
        double nubi = std::clamp((R.nubi_pct + 20.0 * P.z_nubi) / 12.5, 0.0, 8.0);
        const double p_bagnato = std::clamp(R.pioggia_ora * scala_pioggia * P.scala_bagnato, 0.0, 0.98);
        // catena con la stazionaria p e la persistenza: p(b|b) = p + r (1 - p), p(b|a) = p (1 - r)
        const double pb = P.bagnato ? p_bagnato + PERSISTENZA_PIOGGIA * (1.0 - p_bagnato) : p_bagnato * (1.0 - PERSISTENZA_PIOGGIA);
        P.bagnato = P.pioggia.uniforme() < pb;
        if (P.bagnato && h >= 200) ++P.ore_bagnate;
        // la soglia di ERA5 e' 0,1 mm/h: le ore bagnate la superano
        double precip = P.bagnato ? 0.1 + gamma_casuale(P.pioggia, 0.8, std::max(R.intensita_mm_h * scala_intensita - 0.1, 0.05) / 0.8) : 0.0;
        if (precip > 0.0) nubi = std::max(nubi, 6.0);
        if (nubi < 2.0) precip = 0.0;   // cielo sereno: niente precipitazione
        // 6. temperature e pressione
        const double fa = std::exp(-1.0 / TAU_ARIA_H), fm = std::exp(-1.0 / TAU_MARE_H), fp = std::exp(-1.0 / TAU_PRESSIONE_H);
        P.z_aria = fa * P.z_aria + std::sqrt(1.0 - fa * fa) * gauss(P.temperatura);
        P.z_mare = fm * P.z_mare + std::sqrt(1.0 - fm * fm) * gauss(P.temperatura);
        P.z_pressione = fp * P.z_pressione + std::sqrt(1.0 - fp * fp) * gauss(P.pressione);
        const double estate = si == 2 ? 1.0 : si == 0 ? 0.0 : 0.5;
        const auto anomalia = [&](const RegimeMeteo& x) { return (1.0 - estate) * x.anomalia_t_c[0] + estate * x.anomalia_t_c[1]; };
        const double t_aria = M.t_aria_c + (1.0 - w) * anomalia(Rp) + w * anomalia(R) + SIGMA_ARIA_C * P.z_aria;
        const double t_mare = M.t_mare_c + SIGMA_MARE_C * P.z_mare;
        const auto rp = regime_da_nome(P.regime_prima), rn = regime_da_nome(P.regime_id);
        const double pressione = M.pressione_hpa + (1.0 - w) * scarto_pressione(rp ? *rp : Regime::calma) +
                                 w * scarto_pressione(rn ? *rn : Regime::calma) + SIGMA_PRESSIONE_HPA * P.z_pressione;

        // lo stato di base (senza evento) e quello vero
        base.regime = rn ? *rn : Regime::calma;
        base.vento_m_s = u;
        base.vento_da_gradi = da;
        base.raffica_m_s = u * (M.raffica_fattore > 1.0 ? M.raffica_fattore : 0.5 * (Gm.raffica_fattore[0] + Gm.raffica_fattore[1])) *
                           (precip > 2.0 ? 1.1 : 1.0);
        base.hs_swell_m = hs_swell;
        base.tp_swell_s = 0.5 * (T.swell_tp_s[0] + T.swell_tp_s[1]) + 0.25 * (T.swell_tp_s[1] - T.swell_tp_s[0]) * P.z_swell;
        base.swell_da_gradi = 0.5 * (T.swell_da_gradi[0] + T.swell_da_gradi[1]);
        base.nubi_ottavi = nubi;
        base.plafond_m = nubi >= 6.0 ? (precip > 0.0 ? 300.0 : 600.0) : nubi >= 3.0 ? 1200.0 : 3000.0;
        base.t_aria_c = t_aria;
        base.t_mare_c = t_mare;
        base.pressione_hpa = pressione;
        if (t_aria < Gm.neve_sotto_c) base.neve_mm_h = precip; else base.pioggia_mm_h = precip;
        StatoMeteo vero = base;
        if (!ev.empty()) {
            const auto& E = T.eventi.at(ev);
            vero.evento = ev;
            if (E.vento_m_s.size() >= 2) {
                vero.vento_m_s = std::max(vero.vento_m_s, base.vento_m_s + rampa_evento * (P.evento_vento - base.vento_m_s));
                vero.raffica_m_s = vero.vento_m_s * 1.4;
            }
            if (ev == "polar_low") { vero.neve_mm_h = std::max(vero.neve_mm_h, 2.0); vero.nubi_ottavi = 8.0; vero.plafond_m = 300.0; }
            if (ev == "medicane") { vero.pioggia_mm_h = std::max(vero.pioggia_mm_h, 5.0); vero.nubi_ottavi = 8.0; }
            if (ev == "cut_off" || ev == "ciclone_esplosivo") {
                if (const auto rr = regime_da_nome(ev)) vero.regime = *rr;
                vero.vento_m_s = std::max(vero.vento_m_s, base.vento_m_s * 1.3);
                vero.pioggia_mm_h = std::max(vero.pioggia_mm_h, 1.5);
            }
            if (ev == "spruzzo_gelante_forte") vero.t_aria_c = std::min(vero.t_aria_c, E.aria_max_c - 2.0);
            if (ev == "fumo_di_mare_denso") vero.t_aria_c = std::min(vero.t_aria_c, E.aria_max_c);
            if (ev == "tempesta_bft10" || ev == "polar_low" || ev == "medicane" || ev == "ciclone_esplosivo")
                vero.pressione_hpa -= 20.0 * rampa_evento;
        }
        // onde col vento vero (lo stato di base tiene il suo mare, per la previsione)
        P.hs_vento = mare_da(vero.vento_m_s, vero.vento_da_gradi, P.hs_vento);
        vero.hs_vento_m = P.hs_vento;
        if (!ev.empty() && P.evento_hs > 0.0) vero.hs_vento_m = std::max(vero.hs_vento_m, rampa_evento * P.evento_hs);
        base.hs_vento_m = ev.empty() ? vero.hs_vento_m : std::min(vero.hs_vento_m, Gm.hs_pm_coeff * base.vento_m_s * base.vento_m_s / G);
        // tp del mare da vento (Pierson-Moskowitz formato)
        for (StatoMeteo* x : {&base, &vero}) x->tp_vento_s = Gm.tp_pm_coeff * std::sqrt(std::max(x->hs_vento_m, 0.01));
        // 7. ghiaccio (frazione del mese; Baltico negli inverni rigidi) e spruzzo gelante (Overland)
        double ghiaccio = M.ghiaccio_pct / 100.0;
        if (std::find(T.inverni_rigidi.begin(), T.inverni_rigidi.end(), anno_) != T.inverni_rigidi.end() && (mese_ == 2 || mese_ == 3))
            ghiaccio = std::max(ghiaccio, 0.3);
        for (StatoMeteo* x : {&base, &vero}) {
            x->ghiaccio = ghiaccio;
            x->hs_vento_m *= 1.0 - ghiaccio;
            x->hs_swell_m *= 1.0 - ghiaccio;
            if (x->t_aria_c < Gm.overland_tf_c && x->vento_m_s > 0.0) {
                const double ppr = x->vento_m_s * (Gm.overland_tf_c - x->t_aria_c) / (1.0 + 0.3 * (x->t_mare_c - Gm.overland_tf_c));
                x->spruzzo_gelante_cm_h = 0.0;
                for (std::size_t i = 0; i < Gm.overland_classi_ppr.size() && i < Gm.overland_classi_cm_h.size(); ++i)
                    if (ppr >= Gm.overland_classi_ppr[i]) x->spruzzo_gelante_cm_h = Gm.overland_classi_cm_h[i];
            }
        }
        // 8. nebbia (avvezione, fumo di mare, radiazione, precipitazione) e visibilita' di Koschmieder
        const double fumo_possibile = vero.t_mare_c - vero.t_aria_c >= Gm.fumo_di_mare_delta_c;
        double p_nebbia = 0.0;
        Nebbia tipo = Nebbia::nessuna;
        if (ev == "nebbia_persistente") { p_nebbia = 1.0; tipo = Nebbia::avvezione; }
        else if (ev == "fumo_di_mare_denso" || (fumo_possibile && M.fumo_di_mare_pct > 0.0)) {
            p_nebbia = ev == "fumo_di_mare_denso" ? 1.0 : std::min(1.0, 2.0 * M.fumo_di_mare_pct / 100.0);
            tipo = Nebbia::fumo_di_mare;
        } else if (R.nebbia != Nebbia::nessuna && vero.vento_m_s <= Gm.nebbia_vento_max_m_s) {
            tipo = R.nebbia;
            // la quota del mese (UR >= 97%) concentrata nei regimi con la nebbia, col vento giusto (STIMA 0,7 del tempo)
            p_nebbia = std::min(1.0, (M.ur97_pct / 100.0) / std::max(quota_nebbia * 0.7, 0.02));
            if (tipo == Nebbia::avvezione && (vero.vento_m_s < Gm.nebbia_avvezione_vento_m_s[0] || vero.vento_m_s > Gm.nebbia_avvezione_vento_m_s[1])) p_nebbia *= 0.3;
            if (tipo == Nebbia::radiazione && !(ora_loc < 9.0 || ora_loc > 22.0)) p_nebbia = 0.0;
            if (tipo == Nebbia::precipitazione && precip <= 0.0) p_nebbia = 0.0;
        }
        // persistenza della nebbia: cambia stato con la probabilita' del 25% l'ora (STIMA)
        if (P.nebbia.uniforme() < 0.25) {
            P.in_nebbia = P.nebbia.uniforme() < p_nebbia;
            P.v_nebbia_km = P.nebbia.uniforme(Gm.nebbia_visibilita_km[0], Gm.nebbia_visibilita_km[1]);
        }
        if (ev != "nebbia_persistente" && ev != "fumo_di_mare_denso") p_nebbia = std::min(1.0, p_nebbia * P.scala_nebbia);
        if (p_nebbia <= 0.0) P.in_nebbia = false;
        const bool c_e_nebbia = P.in_nebbia;
        if (c_e_nebbia && h >= 200) ++P.ore_nebbia;
        const double diurno_nebbia = ora_loc >= 1.0 && ora_loc < 7.0 ? Gm.nebbia_notte : ora_loc >= 13.0 && ora_loc < 17.0 ? Gm.nebbia_pomeriggio : 1.0;
        const double foschia_km = std::min(R.visibilita_km, T.foschia_km[static_cast<std::size_t>(si)]);
        for (StatoMeteo* x : {&base, &vero}) {
            double sig = Gm.koschmieder / std::max(foschia_km, 0.05);
            if (x->pioggia_mm_h > 0.0) {
                const double cc = Gm.acqua_pioggia[0] * std::pow(x->pioggia_mm_h, Gm.acqua_pioggia[1]);
                sig += Gm.estinzione_pioggia[0] * std::pow(cc, Gm.estinzione_pioggia[1]);
            }
            if (x->neve_mm_h > 0.0) sig += Gm.estinzione_neve[0] * std::pow(x->neve_mm_h, Gm.estinzione_neve[1]) * 0.1;
            if (beaufort(x->vento_m_s) >= Gm.spruzzi_da_beaufort) sig += 0.4 * (beaufort(x->vento_m_s) - Gm.spruzzi_da_beaufort + 1);
            x->nebbia = Nebbia::nessuna;
            x->strato_nebbia_m = 0.0;
            if (c_e_nebbia && !(x == &base && !ev.empty() && tipo == Nebbia::avvezione && ev == "nebbia_persistente")) {
                x->nebbia = tipo;
                sig += diurno_nebbia * Gm.koschmieder / P.v_nebbia_km;
                if (tipo == Nebbia::fumo_di_mare)
                    x->strato_nebbia_m = Gm.fumo_di_mare_strato_m[0] + (Gm.fumo_di_mare_strato_m[1] - Gm.fumo_di_mare_strato_m[0]) *
                                                                         std::clamp((x->t_mare_c - x->t_aria_c - Gm.fumo_di_mare_delta_c) / 10.0, 0.0, 1.0);
            }
            x->visibilita_m = std::min(1000.0 * Gm.koschmieder / sig, 60000.0);
            if (x->evento == "polar_low") x->visibilita_m = std::min(x->visibilita_m, 1500.0);
        }
        // 9. celle temporalesche: nascono nei regimi col temporale (e col temporale estivo), si muovono col vento
        double p_cella = R.temporale_ora * scala_temporali;
        if (ev == "temporale_estivo") p_cella = std::max(p_cella, 0.3);
        if (ev == "medicane") p_cella = std::max(p_cella, 0.2);
        if (P.celle.uniforme() < p_cella) {
            CellaTemporale cl;
            cl.pos = {P.celle.uniforme(-RIQUADRO_M, RIQUADRO_M), P.celle.uniforme(-RIQUADRO_M, RIQUADRO_M)};
            cl.raggio_m = 1000.0 * P.celle.uniforme(Gm.celle_raggio_km[0], Gm.celle_raggio_km[1]);
            cl.inizio = hd * 3600.0 + P.celle.uniforme(0.0, 3600.0);
            cl.fine = cl.inizio + 3600.0 * P.celle.uniforme(Gm.celle_vita_h[0], Gm.celle_vita_h[1]);
            cl.lampi_al_minuto = P.celle.uniforme(Gm.lampi_al_minuto[0], Gm.lampi_al_minuto[1]);
            // la cella va col vento (0,6 U, STIMA): posizione al tempo 0 e velocita' in velocita_celle_
            const double verso = (vero.vento_da_gradi + 180.0) * PI / 180.0, v = 0.6 * vero.vento_m_s;
            cl.pos = cl.pos - Vec2{v * std::sin(verso), v * std::cos(verso)} * cl.inizio;
            celle_.push_back(cl);
            velocita_celle_.push_back(Vec2{v * std::sin(verso), v * std::cos(verso)});
        }
        ore_.push_back({vero, base});
        // la finestra: le ore troppo vecchie si buttano
        while (static_cast<long>(ore_.size()) > FINESTRA_ORE + (P.ora - static_cast<long>(t_ / 3600.0)) + 2 && prima_ora_ < static_cast<long>(t_ / 3600.0) - FINESTRA_ORE) {
            ore_.pop_front();
            ++prima_ora_;
        }
    }
}

StatoMeteo Meteo::interpola(double t, bool base) const {
    if (fisso_) return *fisso_;
    if (!teatro_) return meteo_semplificato();
    const double th = std::max(t, 0.0) / 3600.0;
    const long h = static_cast<long>(std::floor(th));
    const double f = th - static_cast<double>(h);
    const StatoMeteo a = base ? ora(h).base : ora(h).vero;
    const StatoMeteo& b = base ? ora(h + 1).base : ora(h + 1).vero;
    StatoMeteo s = a;
    const auto mix = [f](double x, double y) { return x + (y - x) * f; };
    s.vento_m_s = mix(a.vento_m_s, b.vento_m_s);
    s.vento_da_gradi = angolo(a.vento_da_gradi + f * differenza(b.vento_da_gradi, a.vento_da_gradi));
    s.raffica_m_s = mix(a.raffica_m_s, b.raffica_m_s);
    s.hs_vento_m = mix(a.hs_vento_m, b.hs_vento_m);
    s.tp_vento_s = mix(a.tp_vento_s, b.tp_vento_s);
    s.hs_swell_m = mix(a.hs_swell_m, b.hs_swell_m);
    s.tp_swell_s = mix(a.tp_swell_s, b.tp_swell_s);
    s.nubi_ottavi = mix(a.nubi_ottavi, b.nubi_ottavi);
    s.plafond_m = mix(a.plafond_m, b.plafond_m);
    s.pioggia_mm_h = mix(a.pioggia_mm_h, b.pioggia_mm_h);
    s.neve_mm_h = mix(a.neve_mm_h, b.neve_mm_h);
    s.visibilita_m = std::exp(mix(std::log(a.visibilita_m), std::log(b.visibilita_m)));
    s.t_aria_c = mix(a.t_aria_c, b.t_aria_c);
    s.t_mare_c = mix(a.t_mare_c, b.t_mare_c);
    s.pressione_hpa = mix(a.pressione_hpa, b.pressione_hpa);
    s.strato_nebbia_m = mix(a.strato_nebbia_m, b.strato_nebbia_m);
    s.spruzzo_gelante_cm_h = mix(a.spruzzo_gelante_cm_h, b.spruzzo_gelante_cm_h);
    // tendenza barometrica: le ultime 3 ore
    const double t3 = std::max(t - 3.0 * 3600.0, static_cast<double>(prima_ora_) * 3600.0);
    if (t - t3 > 1.0) {
        const long h3 = static_cast<long>(std::floor(t3 / 3600.0));
        const double f3 = t3 / 3600.0 - static_cast<double>(h3);
        const double p3 = (base ? ora(h3).base : ora(h3).vero).pressione_hpa * (1.0 - f3) + (base ? ora(h3 + 1).base : ora(h3 + 1).vero).pressione_hpa * f3;
        s.tendenza_hpa_3h = (s.pressione_hpa - p3) * 3.0 * 3600.0 / (t - t3);
    }
    if (!base) {   // il temporale nel centro del riquadro: le celle entro il loro raggio
        s.lampi_al_minuto = 0.0;
        for (std::size_t i = 0; i < celle_.size(); ++i) {
            const auto& c = celle_[i];
            if (t < c.inizio || t > c.fine) continue;
            const Vec2 p = c.pos + velocita_celle_[i] * t;
            if (std::hypot(p.x, p.y) < c.raggio_m) s.lampi_al_minuto = std::max(s.lampi_al_minuto, c.lampi_al_minuto);
        }
    }
    return s;
}

void Meteo::avanza(double t) {
    t_ = t;
    stato_ = interpola(t, false);
}

StatoMeteo Meteo::in(Vec2 pos) const {
    StatoMeteo s = stato_;
    if ((fisso_ && !ghiaccio_dei_punti_) || !teatro_) return s;
    const TeatroMeteo& T = *teatro_;
    const auto mi = static_cast<std::size_t>(mese_ - 1);
    // i punti ERA5: rapporti col centro, pesati con l'inverso della distanza al quadrato
    const double lat = lat0_ + pos.y / 111320.0;
    const double lon = lon0_ + pos.x / (111320.0 * std::cos(lat0_ * PI / 180.0));
    if (fisso_) {   // fissa_sopra: tutto fermo com'e', salvo il ghiaccio del punto, con la stessa regola di sotto
        double pg = 0.0, w = 0.0;
        for (const auto& p : T.punti) {
            const double dy = (p.lat - lat) * 111.32, dx = (p.lon - lon) * 111.32 * std::cos(lat * PI / 180.0);
            const double wi = 1.0 / (dx * dx + dy * dy + 25.0);
            pg += wi * p.ghiaccio_pct[mi];
            w += wi;
        }
        if (w > 0.0) {
            const double inverno = stato_.ghiaccio > T.mesi[mi].ghiaccio_pct / 100.0 + 1e-9 ? stato_.ghiaccio : 0.0;
            s.ghiaccio = std::clamp(std::max(pg / w / 100.0, inverno), 0.0, 1.0);
        }
        return s;
    }
    if (!T.punti.empty()) {
        double pv = 0.0, ph = 0.0, pg = 0.0, pt = 0.0, w = 0.0;
        for (const auto& p : T.punti) {
            const double dy = (p.lat - lat) * 111.32, dx = (p.lon - lon) * 111.32 * std::cos(lat * PI / 180.0);
            const double wi = 1.0 / (dx * dx + dy * dy + 25.0);
            pv += wi * p.vento_media_m_s[mi]; ph += wi * p.hs_media_m[mi]; pg += wi * p.ghiaccio_pct[mi]; pt += wi * p.t_aria_c[mi];
            w += wi;
        }
        const MeseMeteo& M = T.mesi[mi];
        if (w > 0.0) {
            const double rv = M.vento_media_m_s > 0.1 ? (pv / w) / M.vento_media_m_s : 1.0;
            const double rh = M.hs_media_m > 0.05 ? (ph / w) / M.hs_media_m : 1.0;
            s.vento_m_s *= rv; s.raffica_m_s *= rv;
            s.hs_vento_m *= rh; s.hs_swell_m *= rh;
            s.t_aria_c += pt / w - M.t_aria_c;
            // il ghiaccio del punto comanda (il margine del pack sta fra i punti: Okhotsk coperto, il Pacifico libero); il
            // pavimento degli inverni rigidi del Baltico, quando il teatro lo mette, resta come minimo
            const double inverno = stato_.ghiaccio > M.ghiaccio_pct / 100.0 + 1e-9 ? stato_.ghiaccio : 0.0;
            s.ghiaccio = std::clamp(std::max(pg / w / 100.0, inverno), 0.0, 1.0);
        }
    }
    // stretti: il vento incanalato vicino al centro del teatro (entro 30 km, STIMA)
    const auto r = T.regimi.find(nome_regime(s.regime));
    const double incanalamento = T.incanalamento * (r != T.regimi.end() ? r->second.incanalamento : 1.0);
    if (incanalamento > 1.0) {
        const double f = 1.0 + (incanalamento - 1.0) * std::clamp(1.0 - std::hypot(pos.x, pos.y) / 30000.0, 0.0, 1.0);
        s.vento_m_s *= f; s.raffica_m_s *= f;
    }
    // le celle sopra il punto: rovesci, raffiche, lampi
    s.lampi_al_minuto = 0.0;
    for (std::size_t i = 0; i < celle_.size(); ++i) {
        const auto& c = celle_[i];
        if (t_ < c.inizio || t_ > c.fine) continue;
        const Vec2 p = c.pos + velocita_celle_[i] * t_;
        const double d = std::hypot(p.x - pos.x, p.y - pos.y);
        if (d > c.raggio_m) continue;
        const double f = 1.0 - d / c.raggio_m;
        s.lampi_al_minuto = std::max(s.lampi_al_minuto, c.lampi_al_minuto);
        s.pioggia_mm_h = std::max(s.pioggia_mm_h, 20.0 * f);
        s.raffica_m_s = std::max(s.raffica_m_s, s.vento_m_s * (1.4 + 0.4 * f));
        s.nubi_ottavi = 8.0;
        s.plafond_m = std::min(s.plafond_m, 500.0);
        const double cc = dati_->generale.acqua_pioggia[0] * std::pow(s.pioggia_mm_h, dati_->generale.acqua_pioggia[1]);
        const double sig = dati_->generale.koschmieder / std::max(s.visibilita_m / 1000.0, 0.01) +
                           dati_->generale.estinzione_pioggia[0] * std::pow(cc, dati_->generale.estinzione_pioggia[1]);
        s.visibilita_m = 1000.0 * dati_->generale.koschmieder / sig;
    }
    // il ghiaccio del punto: lo stato del teatro e' gia' smorzato dal suo, qui solo la differenza (in piu' o in meno: dove il
    // punto ha meno ghiaccio del teatro il mare torna su)
    if (s.ghiaccio != stato_.ghiaccio && stato_.ghiaccio < 1.0) {
        const double f = (1.0 - s.ghiaccio) / (1.0 - stato_.ghiaccio);
        s.hs_vento_m *= f;
        s.hs_swell_m *= f;
    }
    return s;
}

std::vector<CellaTemporale> Meteo::celle(double t) const {
    std::vector<CellaTemporale> v;
    if (fisso_ || !teatro_) return v;
    ora(static_cast<long>(std::floor(t / 3600.0)) + 1);   // le celle nate fino a quell'ora
    for (std::size_t i = 0; i < celle_.size(); ++i)
        if (t >= celle_[i].inizio && t <= celle_[i].fine) {
            CellaTemporale c = celle_[i];
            c.pos = c.pos + velocita_celle_[i] * t;
            v.push_back(c);
        }
    return v;
}

std::vector<Lampo> Meteo::lampi(double t0, double t1) const {
    std::vector<Lampo> v;
    if (fisso_ || !teatro_ || t1 <= t0) return v;
    ora(static_cast<long>(std::floor(t1 / 3600.0)) + 1);
    const GeneraleMeteo& Gm = dati_->generale;
    for (std::size_t i = 0; i < celle_.size(); ++i) {
        const auto& c = celle_[i];
        const double a = std::max(t0, c.inizio), b = std::min(t1, c.fine);
        if (b <= a) continue;
        // per minuto di vita della cella: lo stesso minuto da' gli stessi lampi, comunque si chieda l'intervallo
        for (long m = static_cast<long>(std::floor(a / 60.0)); static_cast<double>(m) * 60.0 < b; ++m) {
            Casuale r(mescola(seme_ ^ mescola(static_cast<std::uint64_t>(i) * 0x51ED27ull + static_cast<std::uint64_t>(m))));
            const int n = poisson(r, c.lampi_al_minuto);
            for (int k = 0; k < n; ++k) {
                Lampo l;
                l.t = static_cast<double>(m) * 60.0 + r.uniforme(0.0, 60.0);
                const double ang = r.uniforme(0.0, 2.0 * PI), d = c.raggio_m * std::sqrt(r.uniforme());
                l.pos = c.pos + velocita_celle_[i] * l.t + Vec2{d * std::sin(ang), d * std::cos(ang)};
                l.colpi = r.uniforme() < Gm.lampi_singolo ? 1
                                                         : 2 + poisson(r, std::max(0.0, (Gm.lampi_colpi_media - Gm.lampi_singolo) / (1.0 - Gm.lampi_singolo) - 2.0));
                l.nube_mare = r.uniforme() < FRAZIONE_NUBE_MARE;
                if (l.t >= t0 && l.t < t1 && l.t >= c.inizio && l.t <= c.fine) v.push_back(l);
            }
        }
    }
    std::sort(v.begin(), v.end(), [](const Lampo& x, const Lampo& y) { return x.t < y.t; });
    return v;
}

StatoMeteo Meteo::previsione(double emessa_t, double valida_t) const {
    if (fisso_ || !teatro_) return interpola(valida_t, true);
    const GeneraleMeteo& Gm = dati_->generale;
    const double orizzonte = std::max(0.0, (valida_t - emessa_t) / 3600.0);
    const auto per_orizzonte = [&](const std::vector<double>& y) {
        const auto& x = Gm.previsione_orizzonti_h;
        if (x.empty() || y.empty()) return 0.0;
        if (orizzonte <= x.front()) return y.front() * orizzonte / std::max(x.front(), 1.0);
        for (std::size_t i = 1; i < x.size() && i < y.size(); ++i)
            if (orizzonte <= x[i]) return y[i - 1] + (y[i] - y[i - 1]) * (orizzonte - x[i - 1]) / (x[i] - x[i - 1]);
        return y[std::min(x.size(), y.size()) - 1] * orizzonte / x[std::min(x.size(), y.size()) - 1];
    };
    // l'errore e' del bollettino (emessa, valida): deterministico
    Casuale r(mescola(seme_ ^ mescola(static_cast<std::uint64_t>(std::llround(emessa_t)) * 0x9E37ull) ^
                      static_cast<std::uint64_t>(std::llround(valida_t / 60.0))));
    Casuale fronti(mescola(seme_ ^ static_cast<std::uint64_t>(std::llround(emessa_t)) * 0xC0FFEEull));   // lo stesso per tutto il bollettino
    const double spostamento_h = gauss(fronti) * per_orizzonte(Gm.previsione_errore_fronti_h) +
                                 (fronti.uniforme() < Gm.previsione_burrasche_in_ritardo && orizzonte >= 48.0 ? -fronti.uniforme(6.0, 12.0) : 0.0);
    StatoMeteo s = interpola(std::max(emessa_t, valida_t - spostamento_h * 3600.0), true);
    s.vento_m_s = std::max(0.0, s.vento_m_s + gauss(r) * per_orizzonte(Gm.previsione_sigma_vento_m_s));
    s.raffica_m_s = std::max(s.vento_m_s, s.raffica_m_s);
    s.hs_vento_m = std::max(0.0, s.hs_vento_m + gauss(r) * per_orizzonte(Gm.previsione_sigma_hs_m));
    s.lampi_al_minuto = 0.0;   // i singoli temporali non si prevedono: «possibili rovesci»
    // gli eventi che si prevedevano nel 1989 (non polar low, getti, temporali, trombe marine)
    const StatoMeteo vero = interpola(valida_t, false);
    if (!vero.evento.empty() && teatro_->eventi.count(vero.evento) && teatro_->eventi.at(vero.evento).prevista &&
        std::find(Gm.previsione_mai.begin(), Gm.previsione_mai.end(), vero.evento) == Gm.previsione_mai.end()) {
        s.evento = vero.evento;
        s.vento_m_s = std::max(s.vento_m_s, vero.vento_m_s - per_orizzonte(Gm.previsione_sigma_vento_m_s));
    }
    if (s.nebbia != Nebbia::nessuna) s.visibilita_m = std::max(s.visibilita_m, 1000.0);   // «possibili banchi di nebbia»
    return s;
}

}  // namespace m688
