#include "nucleo/acustica.hpp"

#include "nucleo/campioni.hpp"

#include <algorithm>
#include <cmath>
#include <limits>
#include <vector>

// Paesaggio sonoro: sorgenti non navali della libreria (Fase 3, S5c; dossier 09 §5 e §6.2). Gli istanti degli eventi
// si ricavano da (seme, indice dell'evento) con una funzione di mescolamento, senza stato: lo stesso tempo di gioco da'
// sempre la stessa risposta, in qualunque ordine si chieda.

namespace m688 {

namespace {

constexpr double SLOT_POISSON_S = 10.0;   // gli eventi di Poisson si estraggono per finestre di 10 s
constexpr double FUORI_BANDA_DB_OTT = 24.0;   // le bande calano di 24 dB per ottava fuori dai loro estremi (STIMA)
constexpr double CAMBIO_ROTTA_S = 600.0;

std::uint64_t mescola(std::uint64_t x) {   // splitmix64
    x += 0x9e3779b97f4a7c15ull;
    x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ull;
    x = (x ^ (x >> 27)) * 0x94d049bb133111ebull;
    return x ^ (x >> 31);
}
double u01(std::uint64_t seme, std::int64_t k, std::uint64_t sale) {
    const std::uint64_t h = mescola(seme ^ mescola(static_cast<std::uint64_t>(k) * 0x632be59bd9b4e019ull + sale));
    return static_cast<double>(h >> 11) * 0x1.0p-53;
}
std::uint64_t hash_testo(const std::string& s) {
    std::uint64_t h = 1469598103934665603ull;   // FNV-1a
    for (unsigned char ch : s) h = (h ^ ch) * 1099511628211ull;
    return h;
}

double potenza(double db) { return std::pow(10.0, db / 10.0); }

// forma della banda in dB (relativa), dentro e fuori dagli estremi
double forma(const Componente& c, double f) {
    const double lo = std::max(c.f_lo, 0.1), hi = std::max(c.f_hi, lo * 1.01);
    const double rif = c.picco_hz > 0.0 ? c.picco_hz : lo;
    const double ff = std::clamp(f, lo, hi);
    double db = -std::fabs(c.pendenza_db_ott) * std::fabs(std::log2(ff / rif));
    if (f < lo) db -= FUORI_BANDA_DB_OTT * std::log2(lo / f);
    if (f > hi) db -= FUORI_BANDA_DB_OTT * std::log2(f / hi);
    return db;
}
}  // namespace

// 10 log dell'integrale della forma fra gli estremi (per normalizzare la potenza della banda)
double integrale_forma_db(const Componente& c) {
    const double lo = std::max(c.f_lo, 0.1), hi = std::max(c.f_hi, lo * 1.01);
    constexpr int N = 32;
    double somma = 0.0;
    for (int i = 0; i < N; ++i) {
        const double a = lo * std::pow(hi / lo, static_cast<double>(i) / N), b = lo * std::pow(hi / lo, static_cast<double>(i + 1) / N);
        somma += potenza(forma(c, std::sqrt(a * b))) * (b - a);
    }
    return 10.0 * std::log10(std::max(somma, 1e-30));
}

namespace {
double integrale_di(const Componente& c) { return std::isnan(c.integrale_db) ? integrale_forma_db(c) : c.integrale_db; }

double livello_base(const Sorgente& s) {
    const SorgenteLibreria& f = *s.fonte;
    double l = f.livello_db + s.guadagno_db;
    if (f.diurno_db != 0.0 && s.ora_locale >= 0.0 && f.diurno_ore == fascia_oraria(s.ora_locale)) l += f.diurno_db;
    return l;
}

TipoRiga tipo_riga(const SorgenteLibreria& f) {
    if (f.categoria == "biologico") return TipoRiga::biologico;
    if (f.categoria == "geofisico" || f.categoria == "meteo") return TipoRiga::geofisico;
    return TipoRiga::industriale;
}

}  // namespace

const char* fascia_oraria(double ora) {
    const double h = std::fmod(std::fmod(ora, 24.0) + 24.0, 24.0);
    if (h >= 7.0 && h < 18.0) return "giorno";
    if ((h >= 5.0 && h < 7.0) || (h >= 18.0 && h < 21.0)) return "crepuscolo";
    return "notte";
}

double attivita(const SorgenteLibreria& f, const Componente& c, std::uint64_t seme, double t, double w) {
    const Ritmo& r = f.ritmo;
    const bool ritmata = c.ritmata || c.tipo == TipoComponente::impulso || c.tipo == TipoComponente::sweep;
    if (!ritmata || r.processo == Processo::continuo) return 1.0;
    w = std::max(w, 1e-3);
    const double durata = std::max(c.durata_s, 0.01);
    const double inizio = t - w, fine = t;
    const double gruppo_s = (r.gruppo_max - 1) * r.interno_max_s;
    double occupato = 0.0;
    const auto evento = [&](std::int64_t k, double base) {
        if (r.processo == Processo::treno && r.serie_min > 0.0) {   // serie di serie_min minuti e pause
            const double ciclo = r.serie_min * 60.0 + r.pausa_s;
            if (std::fmod(base + u01(seme, 0, 9) * ciclo, ciclo) >= r.serie_min * 60.0) return;
        }
        const int n = r.gruppo_min + static_cast<int>(u01(seme, k, 4) * (r.gruppo_max - r.gruppo_min + 1));
        double s = base;
        for (int j = 0; j < std::max(n, 1); ++j) {
            if (j > 0) s += r.interno_min_s + u01(seme, k * 16 + j, 5) * (r.interno_max_s - r.interno_min_s);
            occupato += std::max(0.0, std::min(s + durata, fine) - std::max(s, inizio));
        }
    };
    if (r.processo == Processo::singolo) {   // transitori: un evento (o un gruppo) all'istante 0
        evento(0, 0.0);
    } else if (r.processo == Processo::poisson) {
        const double p = std::min(1.0, r.tasso_h * SLOT_POISSON_S / 3600.0);
        const auto k0 = static_cast<std::int64_t>(std::floor((inizio - durata - gruppo_s) / SLOT_POISSON_S)) - 1;
        const auto k1 = static_cast<std::int64_t>(std::floor(fine / SLOT_POISSON_S)) + 1;
        for (std::int64_t k = k0; k <= k1 && k - k0 < 100000; ++k)
            if (u01(seme, k, 2) < p) evento(k, static_cast<double>(k) * SLOT_POISSON_S + u01(seme, k, 3) * SLOT_POISSON_S);
    } else {   // periodico e treno: un evento per periodo medio, spostato a caso entro l'intervallo
        const double m = std::max(0.5 * (r.intervallo_min_s + r.intervallo_max_s), 0.05);
        const double spread = r.intervallo_max_s - r.intervallo_min_s;
        const double fase = u01(seme, 0, 1) * m;
        const auto k0 = static_cast<std::int64_t>(std::floor((inizio - durata - gruppo_s - spread - fase) / m)) - 1;
        const auto k1 = static_cast<std::int64_t>(std::floor((fine + spread) / m)) + 1;
        for (std::int64_t k = k0; k <= k1 && k - k0 < 100000; ++k)
            evento(k, fase + static_cast<double>(k) * m + (u01(seme, k, 6) - 0.5) * spread);
    }
    return std::min(1.0, occupato / w);
}

namespace {
// modulazione_campione dipende dalla frequenza solo per la banda dell'inviluppo che sceglie (il primo ciclo): per lo stesso
// campione, seme, istante e finestra la si tiene per banda. Lo spettro chiede la stessa sorgente bin per bin (30/10/2026:
// 40 milioni di chiamate, il 10% della ripresa del replay); il valore e' quello della funzione, bit per bit
double modulazione_per_banda(const Campione& c, std::uint64_t seme, double tempo_s, double finestra_s, double f_hz) {
    if (c.inviluppo_tempo.empty()) return 1.0;
    std::size_t b = 0;   // lo stesso ciclo di modulazione_campione
    for (std::size_t k = 0; k < c.bande_hz.size() && k < c.inviluppo_tempo.size(); ++k)
        if (f_hz >= c.bande_hz[k].first) b = k;
    struct Memoria {
        const Campione* campione = nullptr;
        std::uint64_t seme = 0;
        double tempo_s = 0.0, finestra_s = 0.0;
        std::vector<double> per_banda;   // NaN: non ancora chiesta
    };
    thread_local Memoria m;
    if (m.campione != &c || m.seme != seme || m.tempo_s != tempo_s || m.finestra_s != finestra_s ||
        m.per_banda.size() != c.inviluppo_tempo.size()) {
        m.campione = &c;
        m.seme = seme;
        m.tempo_s = tempo_s;
        m.finestra_s = finestra_s;
        m.per_banda.assign(c.inviluppo_tempo.size(), std::numeric_limits<double>::quiet_NaN());
    }
    double& v = m.per_banda[b];
    if (std::isnan(v)) v = modulazione_campione(c, seme, tempo_s, finestra_s, f_hz);
    return v;
}

// R5: con un campione collegato l'attivita' della componente e' la sua media di lungo periodo per l'inviluppo del
// campione nel tratto che la cuffia sta leggendo; senza, quella del ritmo
double attivita_di(const Sorgente& s, const Componente& c, double f) {
    const SorgenteLibreria& fo = *s.fonte;
    if (fo.campioni.empty() || fo.attivita_media.size() != fo.componenti.size())
        return attivita(fo, c, s.seme, s.tempo_s, s.finestra_s);
    const std::uint64_t seme = seme_istanza(s);
    const Campione& cp = *fo.campioni[indice_campione(seme, fo.campioni.size())];
    const auto i = static_cast<std::size_t>(&c - fo.componenti.data());
    return std::min(1.0, fo.attivita_media[i] * modulazione_per_banda(cp, seme, s.tempo_s, s.finestra_s, f));
}
double centro(const Componente& c) {
    if (c.tipo == TipoComponente::tonale || c.tipo == TipoComponente::famiglia) return c.hz;
    if (c.tipo == TipoComponente::sweep) return std::sqrt(std::max(c.f_da, 1.0) * std::max(c.f_a, 1.0));
    return c.picco_hz > 0.0 ? c.picco_hz : std::sqrt(std::max(c.f_lo, 1.0) * std::max(c.f_hi, 1.0));
}
}  // namespace

std::vector<double> livelli_componenti(const Sorgente& s) {
    std::vector<double> out;
    if (!s.fonte || !s.fonte->campioni.empty()) return out;
    const SorgenteLibreria& fo = *s.fonte;
    const double base = livello_base(s);
    out.reserve(fo.componenti.size());
    for (const Componente& c : fo.componenti) {
        const double duty = attivita(fo, c, s.seme, s.tempo_s, s.finestra_s);
        out.push_back(duty <= 0.0 ? std::numeric_limits<double>::quiet_NaN() : base + c.livello_rel_db + 10.0 * std::log10(duty));
    }
    return out;
}

double livello_fonte_da(const Sorgente& s, double f, bool righe, const std::vector<double>& livelli) {
    if (!s.fonte) return -300.0;
    const SorgenteLibreria& fo = *s.fonte;
    if (livelli.size() != fo.componenti.size()) return livello_fonte(s, f, righe);
    double p = 0.0;
    const auto riga = [&](double hz, double db) {   // una riga spalmata sull'ottava attorno a f
        if (righe && hz >= f / std::sqrt(2.0) && hz < f * std::sqrt(2.0)) p += potenza(db) / (0.707 * f);
    };
    for (std::size_t i = 0; i < fo.componenti.size(); ++i) {
        const Componente& c = fo.componenti[i];
        const double l = livelli[i];
        if (std::isnan(l)) continue;
        switch (c.tipo) {
            case TipoComponente::banda:
            case TipoComponente::impulso:
                p += potenza(l + forma(c, f) - integrale_di(c));
                break;
            case TipoComponente::sweep: {
                const double lo = std::min(c.f_da, c.f_a), hi = std::max(c.f_da, c.f_a);
                if (f >= lo && f <= hi && hi > lo) p += potenza(l - 10.0 * std::log10(hi - lo));
                break;
            }
            case TipoComponente::tonale: riga(c.hz, l); break;
            case TipoComponente::famiglia:
                for (int k = 1; k <= std::max(c.armoniche, 1); ++k) riga(k * c.hz, l - c.decadimento_db * (k - 1));
                break;
        }
    }
    return p > 0.0 ? 10.0 * std::log10(p) : -300.0;
}

double livello_fonte(const Sorgente& s, double f, bool righe) {
    if (!s.fonte) return -300.0;
    const SorgenteLibreria& fo = *s.fonte;
    if (fo.campioni.empty()) return livello_fonte_da(s, f, righe, livelli_componenti(s));
    const double base = livello_base(s);
    double p = 0.0;
    const auto riga = [&](double hz, double db) {   // una riga spalmata sull'ottava attorno a f
        if (righe && hz >= f / std::sqrt(2.0) && hz < f * std::sqrt(2.0)) p += potenza(db) / (0.707 * f);
    };
    for (const Componente& c : fo.componenti) {
        const double duty = attivita_di(s, c, c.tipo == TipoComponente::banda || c.tipo == TipoComponente::impulso ? f : centro(c));
        if (duty <= 0.0) continue;
        const double l = base + c.livello_rel_db + 10.0 * std::log10(duty);
        switch (c.tipo) {
            case TipoComponente::banda:
            case TipoComponente::impulso:
                p += potenza(l + forma(c, f) - integrale_di(c));
                break;
            case TipoComponente::sweep: {   // l'energia si distribuisce sulla banda percorsa
                const double lo = std::min(c.f_da, c.f_a), hi = std::max(c.f_da, c.f_a);
                if (f >= lo && f <= hi && hi > lo) p += potenza(l - 10.0 * std::log10(hi - lo));
                break;
            }
            case TipoComponente::tonale: riga(c.hz, l); break;
            case TipoComponente::famiglia:
                for (int k = 1; k <= std::max(c.armoniche, 1); ++k) riga(k * c.hz, l - c.decadimento_db * (k - 1));
                break;
        }
    }
    return p > 0.0 ? 10.0 * std::log10(p) : -300.0;
}

double livello_totale_fonte(const Sorgente& s) {
    if (!s.fonte) return 0.0;
    const double base = livello_base(s);
    double p = 0.0;
    for (const Componente& c : s.fonte->componenti) {
        const double duty = attivita_di(s, c, centro(c));
        if (duty <= 0.0) continue;
        double l = base + c.livello_rel_db + 10.0 * std::log10(duty);
        if (c.tipo == TipoComponente::famiglia)   // somma delle armoniche
            l += 10.0 * std::log10(std::max(1, c.armoniche));
        p += potenza(l);
    }
    return p > 0.0 ? 10.0 * std::log10(p) : 0.0;
}

std::vector<std::pair<double, double>> righe_fonte(const Sorgente& s, TipoRiga* tipo) {
    std::vector<std::pair<double, double>> out;
    if (!s.fonte) return out;
    const SorgenteLibreria& fo = *s.fonte;
    if (tipo) *tipo = tipo_riga(fo);
    const double base = livello_base(s);
    for (const Componente& c : fo.componenti) {
        if (c.tipo != TipoComponente::tonale && c.tipo != TipoComponente::famiglia) continue;
        const double duty = attivita_di(s, c, centro(c));
        if (duty <= 0.0) continue;
        const double l = base + c.livello_rel_db + 10.0 * std::log10(duty);
        if (c.tipo == TipoComponente::tonale) out.emplace_back(c.hz, l);
        else for (int k = 1; k <= std::max(c.armoniche, 1); ++k) out.emplace_back(k * c.hz, l - c.decadimento_db * (k - 1));
    }
    return out;
}

// --- popolazione ---

std::vector<IstanzaSorgente> popola_sorgenti(const CatalogoAcustico& c, const std::string& teatro, int mese, double ora,
                                             Vec2 centro, double raggio_m, std::uint64_t seme, int id_base) {
    std::vector<IstanzaSorgente> out;
    const double area_nm2 = PI * std::pow(raggio_m / 1852.0, 2.0);
    const std::string fascia = ora >= 0.0 ? fascia_oraria(ora) : "";
    for (const auto& [id, f] : c.sorgenti) {   // in ordine di chiave: ripetibile
        if (!f.teatri.empty() && std::find(f.teatri.begin(), f.teatri.end(), teatro) == f.teatri.end()) continue;
        if (!f.mesi.empty() && std::find(f.mesi.begin(), f.mesi.end(), mese) == f.mesi.end()) continue;
        if (!fascia.empty() && !f.ore.empty() && std::find(f.ore.begin(), f.ore.end(), fascia) == f.ore.end()) continue;
        Casuale caso(seme ^ hash_testo(id));
        const double lambda = f.densita * area_nm2 / 1000.0;
        int n = 0;   // Poisson (Knuth; con lambda grande la normale)
        if (lambda > 30.0) {
            const double u1 = std::max(caso.uniforme(), 1e-12), u2 = caso.uniforme();
            n = std::max(0, static_cast<int>(std::lround(lambda + std::sqrt(lambda) * std::sqrt(-2.0 * std::log(u1)) * std::cos(2.0 * PI * u2))));
        } else {
            const double soglia = std::exp(-lambda);
            for (double p = caso.uniforme(); p > soglia; p *= caso.uniforme()) ++n;
        }
        for (int k = 0; k < n; ++k) {
            IstanzaSorgente i;
            i.id = id_base + static_cast<int>(out.size());
            i.sorgente = id;
            const double r = raggio_m * std::sqrt(caso.uniforme()), a = 2.0 * PI * caso.uniforme();
            i.pos = {centro.x + r * std::sin(a), centro.y + r * std::cos(a)};
            i.quota_m = f.quota_min_m + caso.uniforme() * (f.quota_max_m - f.quota_min_m);
            i.rotta = 360.0 * caso.uniforme();
            i.velocita_kn = f.v_min_kn + caso.uniforme() * (f.v_max_kn - f.v_min_kn);
            i.seme = caso.prossimo();
            out.push_back(i);
        }
    }
    return out;
}

void muovi_sorgente(IstanzaSorgente& i, double dt) {
    const auto prima = static_cast<std::int64_t>(std::floor(i.eta_s / CAMBIO_ROTTA_S));
    i.eta_s += dt;
    const auto dopo = static_cast<std::int64_t>(std::floor(i.eta_s / CAMBIO_ROTTA_S));
    for (std::int64_t k = prima + 1; k <= dopo; ++k) i.rotta = normalizza_360(i.rotta + (u01(i.seme, k, 7) - 0.5) * 90.0);
    if (i.velocita_kn <= 0.0) return;
    const double r = radianti(i.rotta), v = i.velocita_kn * NODO;
    i.pos.x += v * std::sin(r) * dt;
    i.pos.y += v * std::cos(r) * dt;
}

Sorgente sorgente_da_istanza(const CatalogoAcustico& c, const IstanzaSorgente& i, double tempo_s, double finestra_s,
                             double ora_locale) {
    Sorgente s;
    s.id = i.id; s.pos = i.pos; s.quota_m = i.quota_m; s.rotta = i.rotta; s.velocita_kn = i.velocita_kn;
    s.fonte = c.sorgente(i.sorgente);
    s.tempo_s = tempo_s; s.finestra_s = finestra_s; s.ora_locale = ora_locale; s.seme = i.seme;
    if (s.fonte) {
        s.estensione_m = s.fonte->estensione_m;
        s.esca = s.fonte->categoria == "biologico";   // per le teste dei siluri: falsi bersagli
        s.lunghezza_m = s.fonte->estensione_m > 0.0 ? s.fonte->estensione_m : 20.0;
    }
    return s;
}

}  // namespace m688
