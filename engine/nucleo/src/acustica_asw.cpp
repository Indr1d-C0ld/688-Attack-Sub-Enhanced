#include "nucleo/acustica_asw.hpp"

#include <algorithm>
#include <cmath>

namespace m688 {

namespace {

constexpr double QUOTA_RIFERIMENTO_AEREO_M = 91.44;   // 300 ft: il livello degli aerei in [transitori]
constexpr double QUOTA_SORGENTE_AEREA_M = 1.0;        // il suono dell'aereo entra in acqua appena sotto la superficie
constexpr double MARE_FINESTRA_M = 250.0;             // allungamento della finestra per stato del mare (STIMA)
constexpr double MARE_PIENO_M = 30.0, MARE_NULLO_M = 60.0;   // quote fra cui l'allungamento sparisce (STIMA)

double gauss(Casuale& caso) {   // Box-Muller: due estrazioni sempre, per la ripetibilita'
    const double u1 = std::max(caso.uniforme(), 1e-12), u2 = caso.uniforme();
    return std::sqrt(-2.0 * std::log(u1)) * std::cos(2.0 * PI * u2);
}

std::uint64_t seme_di(int id) {   // splitmix64 dell'id: il seme dei transitori senza seme proprio
    std::uint64_t x = static_cast<std::uint64_t>(static_cast<std::int64_t>(id)) + 0x9e3779b97f4a7c15ull;
    x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ull;
    x = (x ^ (x >> 27)) * 0x94d049bb133111ebull;
    return x ^ (x >> 31);
}

Rilevazione valuta_con(const Sorgente& s, const Ricevitore& r, const Ambiente& amb, double t, CacheCampiAsincrona* campi) {
    return campi ? valuta(s, r, amb, *campi, t) : valuta(s, r, amb);
}

}  // namespace

// --- ricevitori ---

const Sensore* sensore_boa(const CatalogoAcustico& c, const DatiBoa& d) {
    return d.sensore.empty() ? nullptr : c.sensore(d.sensore);
}

Ricevitore ricevitore_boa(const Sonoboa& b, const DatiBoa& d, const CatalogoAcustico& c, double t) {
    Ricevitore r;
    r.pos = posizione_boa(b, t);
    r.quota_m = b.quota_m;
    const double v = std::hypot(b.deriva.x, b.deriva.y);
    r.rotta = v > 0.0 ? normalizza_360(gradi(std::atan2(b.deriva.x, b.deriva.y))) : 0.0;
    r.velocita_kn = v / NODO;
    r.sensore = sensore_boa(c, d);
    return r;
}

double quota_trasduttore_m(double cavo_m, double hover_m) { return std::max(0.0, cavo_m - hover_m); }

Ricevitore ricevitore_dip(Vec2 pos, double cavo_m, const DatiSonarImmersione& d, const CatalogoAcustico& c, bool attivo) {
    Ricevitore r;
    r.pos = pos;
    r.quota_m = quota_trasduttore_m(cavo_m, d.hover_m);
    r.sensore = attivo ? c.sensore(d.sensore) : c.sensore(d.sensore + "_passivo");
    if (!r.sensore) r.sensore = c.sensore(d.sensore);
    return r;
}

// --- rivelazioni ---

std::optional<Rivelazione> rivelazione(const Rilevazione& x, const Sensore& s, const Ricevitore& r, const Sorgente& sorgente,
                                       int ricevitore_id, double t, bool sottomarino, Casuale& caso) {
    if (x.se_db < 0.0) return std::nullopt;
    Rivelazione v;
    v.boa_o_aereo = ricevitore_id;
    v.t = t;
    v.livello_db = x.se_db;
    v.sorgente = sorgente.id;
    v.sottomarino = sottomarino;
    if (!s.omni) v.rilevamento = normalizza_360(rilevamento(r.pos, sorgente.pos) + errore_rilevamento_deg(x, s) * gauss(caso));
    if (s.modo == ModoSensore::attivo)
        v.distanza_m = std::max(0.0, distanza(r.pos, sorgente.pos) + s.sigma_distanza_m * gauss(caso));
    return v;
}

bool SogliaBoa::passa(double t, bool sopra, double soglia_s, double passo_s) {
    if (!sopra) {
        da = -1.0;
        return false;
    }
    if (da < 0.0) da = t - passo_s;   // il segnale vale per tutto il passo che finisce qui
    return t - da + 1e-9 >= soglia_s;
}

std::vector<Rivelazione> ascolta(const Ricevitore& r, int id, const std::vector<SorgenteAsw>& sorgenti,
                                 const AmbienteFra& ambiente, double t, Casuale& caso, CacheCampiAsincrona* campi,
                                 double attenuazione_db) {
    std::vector<Rivelazione> out;
    if (!r.sensore || attenuazione_db <= -1e8) return out;
    for (const auto& x : sorgenti) {
        const Ambiente amb = ambiente ? ambiente(r.pos, x.s.pos) : Ambiente{};
        Rilevazione v = valuta_con(x.s, r, amb, t, campi);
        v.se_db += attenuazione_db;
        if (auto rv = rivelazione(v, *r.sensore, r, x.s, id, t, x.sottomarino, caso)) out.push_back(*rv);
    }
    return out;
}

std::vector<Rivelazione> ascolta_boe(const std::vector<Sonoboa>& boe, const DatiAsw& d, const CatalogoAcustico& c,
                                     const std::vector<SorgenteAsw>& sorgenti, const AmbienteFra& ambiente, double t,
                                     int mare, Casuale& caso, StatoAscoltoBoe& stato, CacheCampiAsincrona* campi,
                                     double attenuazione_db) {
    std::vector<Rivelazione> out;
    if (attenuazione_db <= -1e8) return out;
    const auto amb_di = [&](Vec2 a, Vec2 b) { return ambiente ? ambiente(a, b) : Ambiente{}; };
    for (const auto& b : boe) {
        const DatiBoa* db = d.boa(b.tipo);
        if (!db || !boa_in_ascolto(b, t) || mare > db->mare_max) continue;
        const Ricevitore r = ricevitore_boa(b, *db, c, t);
        if (!r.sensore || r.sensore->modo != ModoSensore::passivo) continue;
        for (const auto& x : sorgenti) {
            const bool lontana = stato.distanza_max_m > 0.0 && distanza(r.pos, x.s.pos) > stato.distanza_max_m;
            Rilevazione v;
            if (!lontana) {
                v = valuta_con(x.s, r, amb_di(r.pos, x.s.pos), t, campi);
                v.se_db += attenuazione_db;
            }
            const bool sopra = v.se_db >= 0.0;
            if (db->soglia) {   // sovietiche: solo un segnale che dura
                const auto chiave = std::make_pair(b.id, x.s.id);
                auto it = stato.soglie.find(chiave);
                if (it == stato.soglie.end()) {
                    if (!sopra) continue;
                    it = stato.soglie.emplace(chiave, SogliaBoa{}).first;
                }
                const bool passa = it->second.passa(t, sopra, db->soglia_s, stato.passo_s);
                if (!sopra) stato.soglie.erase(it);
                if (!passa) continue;
            }
            if (auto rv = rivelazione(v, *r.sensore, r, x.s, b.id, t, x.sottomarino, caso)) out.push_back(*rv);
        }
    }
    return out;
}

// --- aerei ed elicotteri ---

double finestra_snell_m(double quota_aereo_m, double quota_ricevitore_m, int mare) {
    const double h = std::max(0.0, quota_aereo_m), d = std::max(0.0, quota_ricevitore_m);
    const double vicino = std::clamp((MARE_NULLO_M - d) / (MARE_NULLO_M - MARE_PIENO_M), 0.0, 1.0);
    return 0.18 * h + 1.2 * d + MARE_FINESTRA_M * std::max(1, mare) * vicino;
}

double probabilita_sorvolo(double quota_ricevitore_m) {
    const double k = std::clamp((quota_ricevitore_m - MARE_PIENO_M) / (MARE_NULLO_M - MARE_PIENO_M), 0.0, 1.0);
    return 0.95 + (0.8 - 0.95) * k;
}

std::string transitorio_aereo(const std::string& classe) {
    static const std::map<std::string, std::string> voci = {
        {"p3c", "aereo_p3c"},       {"il38", "aereo_il38"},       {"be12", "aereo_be12"},      {"tu142", "aereo_tu142"},
        {"s3a", "aereo_s3"},        {"s3b", "aereo_s3"},          {"sh60b", "elicottero_sh60"}, {"sh60f", "elicottero_sh60"},
        {"sh3h", "elicottero_sh3"}, {"ka25", "elicottero_ka25"},  {"ka27", "elicottero_ka27"}, {"sh2f", "elicottero_sh2"},
        {"hkp4", "elicottero_hkp4"}, {"lynx", "elicottero_lynx"}};
    const auto it = voci.find(classe);
    return it == voci.end() ? std::string{} : it->second;
}

std::optional<Sorgente> sorgente_aerea(const CatalogoAcustico& c, const AereoAcustico& a, Vec2 ricevitore,
                                       double quota_ricevitore_m, double tempo_s, double finestra_s, int mare) {
    const SorgenteLibreria* f = c.transitorio(transitorio_aereo(a.classe));
    if (!f || distanza(a.pos, ricevitore) > finestra_snell_m(a.quota_m, quota_ricevitore_m, mare)) return std::nullopt;
    Sorgente s;
    s.id = a.id;
    s.pos = a.pos;
    s.quota_m = QUOTA_SORGENTE_AEREA_M;
    s.rotta = a.rotta;
    s.velocita_kn = a.velocita_kn;
    s.fonte = f;
    s.tempo_s = tempo_s;
    s.finestra_s = finestra_s;
    s.seme = a.seme ? a.seme : seme_di(a.id);
    s.guadagno_db = -20.0 * std::log10(std::max(a.quota_m, 10.0) / QUOTA_RIFERIMENTO_AEREO_M);
    s.lunghezza_m = 20.0;
    return s;
}

std::optional<Sorgente> sorgente_hover(const CatalogoAcustico& c, const AereoAcustico& a, double tempo_s, double finestra_s) {
    const SorgenteLibreria* f = a.hover ? c.transitorio("hover_spruzzi") : nullptr;
    if (!f) return std::nullopt;
    Sorgente s;
    s.id = a.id;
    s.pos = a.pos;
    s.quota_m = QUOTA_SORGENTE_AEREA_M;
    s.fonte = f;
    s.tempo_s = tempo_s;
    s.finestra_s = finestra_s;
    s.seme = a.seme ? a.seme : seme_di(a.id);
    s.estensione_m = 30.0;   // l'anello di spruzzi sotto il rotore
    s.lunghezza_m = 30.0;
    return s;
}

// --- transitori ---

std::optional<Sorgente> sorgente_transitoria(const CatalogoAcustico& c, const Transitorio& x, double tempo_s, double finestra_s) {
    const SorgenteLibreria* f = c.transitorio(x.tipo);
    if (!f || tempo_s < x.t0 || transitorio_finito(c, x, tempo_s - finestra_s)) return std::nullopt;
    Sorgente s;
    s.id = x.id;
    s.pos = x.pos;
    s.quota_m = x.quota_m;
    s.fonte = f;
    s.tempo_s = tempo_s - x.t0;   // il ritmo del transitorio parte dal suo istante 0
    s.finestra_s = finestra_s;
    s.seme = x.seme ? x.seme : seme_di(x.id);
    s.lunghezza_m = 1.0;
    s.guadagno_db = x.guadagno_db;
    return s;
}

bool transitorio_finito(const CatalogoAcustico& c, const Transitorio& x, double tempo_s) {
    const SorgenteLibreria* f = c.transitorio(x.tipo);
    return !f || (f->vita_s > 0.0 && tempo_s - x.t0 > f->vita_s);
}

std::string transitorio_ping(const std::string& sensore) {
    static const std::map<std::string, std::string> voci = {
        {"aqs13b", "ping_aqs13b"}, {"aqs13f", "ping_aqs13f"}, {"vgs3", "ping_vgs3"},    {"vgs2", "ping_vgs2"},
        {"ssq62b", "ping_dicass"}, {"ssq47b", "ping_ssq47b"}, {"rgb55a", "ping_rgb55a"}, {"hs73", "ping_hs73"}};
    const auto it = voci.find(sensore);
    return it == voci.end() ? std::string{} : it->second;
}

AscoltoTransitorio ascolta_transitorio(const Sorgente& s, const std::vector<Ricevitore>& ricevitori, const Ambiente& amb) {
    AscoltoTransitorio out;
    for (const auto& r : ricevitori) {
        if (!r.sensore || r.sensore->modo != ModoSensore::passivo) continue;
        const Rilevazione x = valuta(s, r, amb);
        if (x.se_db <= out.se_db) continue;
        out.se_db = x.se_db;
        out.sentito = x.se_db >= 0.0;
        out.rilevamento = rilevamento(r.pos, s.pos);
        out.errore_deg = errore_rilevamento_deg(x, *r.sensore);
        out.sensore = r.sensore->id;
    }
    return out;
}

}  // namespace m688
