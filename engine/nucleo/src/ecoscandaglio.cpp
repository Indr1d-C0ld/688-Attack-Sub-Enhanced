#include "nucleo/ecoscandaglio.hpp"

#include <algorithm>
#include <cmath>
#include <limits>
#include <stdexcept>

#include <toml.hpp>

namespace m688 {

namespace {
constexpr double LOBO_LATERALE_DB = -20.0;   // l'impulso va verso il basso: di lato ne arriva meno (STIMA)
constexpr int ANELLI = 8, RAGGI = 16;        // i punti dell'impronta su cui si cerca l'eco piu' vicino

double gauss(Casuale& c) {
    const double u1 = std::max(c.uniforme(), 1e-12), u2 = c.uniforme();
    return std::sqrt(-2.0 * std::log(u1)) * std::cos(2.0 * PI * u2);
}
}  // namespace

ParametriEcoscandaglio carica_ecoscandaglio(const std::string& percorso, const std::string& id) {
    const toml::table t = toml::parse_file(percorso);
    const auto* x = t["ecoscandagli"][id].as_table();
    if (!x) throw std::runtime_error(percorso + ": manca [ecoscandagli." + id + "]");
    ParametriEcoscandaglio p;
    p.id = id;
    const auto leggi = [&](const char* k, double& v) { v = (*x)[k].value_or(v); };
    leggi("cono_deg", p.cono_deg);
    leggi("portata_piena_m", p.portata_piena_m);
    leggi("portata_bassa_m", p.portata_bassa_m);
    leggi("sl_piena_db", p.sl_piena_db);
    leggi("sl_bassa_db", p.sl_bassa_db);
    leggi("f_hz", p.f_hz);
    leggi("errore_scala", p.errore_scala);
    leggi("rumore_rel", p.rumore_rel);
    leggi("falso_fondo", p.falso_fondo);
    leggi("dsl_min_m", p.dsl_min_m);
    leggi("dsl_max_m", p.dsl_max_m);
    leggi("scia_kn", p.scia_kn);
    leggi("falso_scia", p.falso_scia);
    return p;
}

double scala_ecoscandaglio(const ParametriEcoscandaglio& p, Casuale& caso) { return p.errore_scala * gauss(caso); }

Sondaggio sonda(const Fondale& vero, Vec2 pos_vera, Vec2 pos_stimata, double quota_m, double velocita_kn, double t,
                PotenzaEco potenza, bool giorno, double scala, const ParametriEcoscandaglio& p, Casuale& caso) {
    Sondaggio s;
    s.t = t;
    s.pos = pos_stimata;
    s.pos_vera = pos_vera;
    s.quota_m = quota_m;
    s.potenza = potenza;
    s.fondo_vero_m = vero.profondita(pos_vera);
    const double portata = potenza == PotenzaEco::piena ? p.portata_piena_m : p.portata_bassa_m;
    const double tan_mezzo = std::tan(radianti(0.5 * p.cono_deg));
    // l'eco piu' vicino nel cono: al centro, poi su anelli fino al raggio dell'impronta sotto il centro
    const double h0 = s.fondo_vero_m - quota_m;
    double minima = std::isfinite(h0) && h0 > 0.0 ? h0 : std::numeric_limits<double>::infinity();
    const double raggio = std::isfinite(h0) && h0 > 0.0 ? h0 * tan_mezzo : std::min(portata, 6000.0) * tan_mezzo;
    for (int a = 1; a <= ANELLI; ++a) {
        const double r = raggio * a / ANELLI;
        for (int k = 0; k < RAGGI; ++k) {
            const double ang = 2.0 * PI * (k + 0.5 * (a % 2)) / RAGGI;
            const Vec2 q{pos_vera.x + r * std::sin(ang), pos_vera.y + r * std::cos(ang)};
            const double h = vero.profondita(q) - quota_m;
            if (!std::isfinite(h) || h <= 0.0) continue;
            if (r > h * tan_mezzo + 1e-9) continue;   // fuori dal cono a quella profondita'
            minima = std::min(minima, std::hypot(h, r));
        }
    }
    s.impronta_m = std::isfinite(minima) ? minima * tan_mezzo : 0.0;
    // i falsi fondi: le bolle della scia ad alta velocita', di giorno lo strato diffondente se sta fra il battello e il fondo
    const double p_falso = velocita_kn > p.scia_kn ? p.falso_scia : p.falso_fondo;
    if (caso.uniforme() < p_falso) {
        const double dsl = p.dsl_min_m + (p.dsl_max_m - p.dsl_min_m) * caso.uniforme();
        double d = 0.0;
        if (velocita_kn > p.scia_kn) d = 5.0 + 45.0 * caso.uniforme();
        else if (giorno && dsl > quota_m && dsl - quota_m < minima) d = dsl - quota_m;
        else d = 5.0 + 45.0 * caso.uniforme();   // bolle, pesci, uno strato di salinita'
        if (d < minima && d <= portata) {
            s.eco = true;
            s.falso = true;
            s.sotto_chiglia_m = d;
        }
    }
    if (!s.falso) {
        if (!std::isfinite(minima) || minima > portata) return s;   // niente entro la portata
        s.eco = true;
        s.sotto_chiglia_m = std::max(0.0, minima * (1.0 + scala) + p.rumore_rel * minima * gauss(caso));
    }
    s.fondo_m = quota_m + s.sotto_chiglia_m;
    s.sigma_m = std::hypot(p.errore_scala, p.rumore_rel) * s.sotto_chiglia_m;
    return s;
}

EmissioneAttiva emissione(const Sondaggio& s, int emettitore, double rotta, double velocita_kn, const ParametriEcoscandaglio& p) {
    EmissioneAttiva e;
    e.emettitore = emettitore;
    e.chiave = p.id;
    e.pos = s.pos_vera;
    e.quota_m = s.quota_m;
    e.rotta = rotta;
    e.velocita_kn = velocita_kn;
    e.t = s.t;
    e.sl_db = (s.potenza == PotenzaEco::piena ? p.sl_piena_db : p.sl_bassa_db) + LOBO_LATERALE_DB;
    e.f_hz = p.f_hz;
    return e;
}

void ProgrammaEcoscandaglio::periodico(double minuti) {
    periodo_s_ = std::max(0.0, minuti) * 60.0;
    prossimo_ = -1.0;
}

bool ProgrammaEcoscandaglio::passo(double t) {
    if (singolo_) {
        singolo_ = false;
        if (periodo_s_ > 0.0) prossimo_ = t + periodo_s_;
        return true;
    }
    if (periodo_s_ <= 0.0) return false;
    if (prossimo_ < 0.0 || t >= prossimo_) {
        prossimo_ = (prossimo_ < 0.0 ? t : prossimo_) + periodo_s_;
        if (prossimo_ <= t) prossimo_ = t + periodo_s_;
        return true;
    }
    return false;
}

}  // namespace m688
