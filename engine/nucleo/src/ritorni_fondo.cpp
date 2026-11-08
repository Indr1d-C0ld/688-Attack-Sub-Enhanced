#include "nucleo/ritorni_fondo.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>

#include <toml.hpp>

namespace m688 {

namespace {
double gauss(Casuale& c) {
    const double u1 = std::max(c.uniforme(), 1e-12), u2 = c.uniforme();
    return std::sqrt(-2.0 * std::log(u1)) * std::cos(2.0 * PI * u2);
}
}  // namespace

ParametriBqs15Fondo carica_bqs15_fondo(const std::string& percorso, const std::string& id) {
    const toml::table t = toml::parse_file(percorso);
    const auto* x = t["bqs15_fondo"][id].as_table();
    if (!x) throw std::runtime_error(percorso + ": manca [bqs15_fondo." + id + "]");
    ParametriBqs15Fondo p;
    p.id = id;
    const auto leggi = [&](const char* k, double& v) { v = (*x)[k].value_or(v); };
    leggi("settore_az_deg", p.settore_az_deg);
    leggi("elevazione_min_deg", p.elevazione_min_deg);
    leggi("elevazione_max_deg", p.elevazione_max_deg);
    leggi("passo_az_deg", p.passo_az_deg);
    leggi("portata_parete_m", p.portata_parete_m);
    leggi("portata_fondo_m", p.portata_fondo_m);
    leggi("pendenza_parete", p.pendenza_parete);
    leggi("velocita_media_kn", p.velocita_media_kn);
    leggi("fattore_media", p.fattore_media);
    leggi("velocita_alta_kn", p.velocita_alta_kn);
    leggi("fattore_alta", p.fattore_alta);
    leggi("errore_angolo_deg", p.errore_angolo_deg);
    leggi("errore_distanza_m", p.errore_distanza_m);
    leggi("passo_m", p.passo_m);
    leggi("margine_ostacolo_m", p.margine_ostacolo_m);
    leggi("fattore_portata", p.fattore_portata);
    p.mine = (*x)["mine"].value_or(p.mine);
    p.emissione = (*x)["emissione"].value_or(p.emissione);
    return p;
}

std::optional<ParametriBqs15Fondo> bqs15_fondo_di_classe(const std::string& percorso, const std::string& classe) {
    const toml::table t = toml::parse_file(percorso);
    const auto* voci = t["bqs15_fondo"].as_table();
    if (!voci) return std::nullopt;
    for (const auto& [id, v] : *voci) {
        const auto* classi = v.as_table() ? (*v.as_table())["classi"].as_array() : nullptr;
        if (!classi) continue;
        for (const auto& c : *classi) {
            const std::string nome = c.value_or(std::string{});
            if (!nome.empty() && (classe == nome || classe.rfind(nome + "_", 0) == 0))
                return carica_bqs15_fondo(percorso, std::string(id.str()));
        }
    }
    return std::nullopt;
}

double portata_effettiva(double portata_m, double velocita_kn, const ParametriBqs15Fondo& p) {
    double f = 1.0;
    if (velocita_kn >= p.velocita_alta_kn) f = p.fattore_alta;
    else if (velocita_kn >= p.velocita_media_kn)
        f = p.fattore_media + (p.fattore_alta - p.fattore_media) * (velocita_kn - p.velocita_media_kn) / (p.velocita_alta_kn - p.velocita_media_kn);
    else f = 1.0 + (p.fattore_media - 1.0) * std::max(0.0, velocita_kn) / p.velocita_media_kn;
    return portata_m * f * p.fattore_portata;
}

std::vector<RitornoFondo> spazza_fondo(const Fondale& vero, Vec2 pos_vera, Vec2 pos_stimata, double quota_m, double rotta,
                                       double velocita_kn, double t, const ParametriBqs15Fondo& p, Casuale& caso) {
    std::vector<RitornoFondo> out;
    const double parete = portata_effettiva(p.portata_parete_m, velocita_kn, p);
    const double fondo = portata_effettiva(p.portata_fondo_m, velocita_kn, p);
    const double tan_min = std::tan(radianti(p.elevazione_min_deg));   // negativo: il bordo basso del ventaglio
    const double tan_max = std::tan(radianti(p.elevazione_max_deg));
    for (double az = -p.settore_az_deg; az <= p.settore_az_deg + 1e-9; az += p.passo_az_deg) {
        const double b = radianti(rotta + az);
        const double sb = std::sin(b), cb = std::cos(b);
        // l'eco piu' vicino nel ventaglio e, se e' piu' in basso, il primo all'altezza del battello: il ventaglio verticale
        // vede la base di una parete e anche la parte che sta davanti alla chiglia, quella che conta
        const auto eco = [&](double h, double sopra, double obliqua) {
            RitornoFondo r;
            r.t = t;
            r.origine = pos_stimata;
            r.quota_m = quota_m;
            const double el = gradi(std::atan2(sopra, h)) + p.errore_angolo_deg * gauss(caso);
            const double az_mis = rotta + az + p.errore_angolo_deg * gauss(caso);
            const double dist = std::max(1.0, obliqua + p.errore_distanza_m * gauss(caso));
            r.rilevamento = normalizza_360(az_mis);
            r.elevazione = el;
            r.distanza_m = dist;
            const double orizz = dist * std::cos(radianti(el));
            r.punto = {pos_stimata.x + orizz * std::sin(radianti(az_mis)), pos_stimata.y + orizz * std::cos(radianti(az_mis))};
            r.profondita_m = quota_m - dist * std::sin(radianti(el));
            r.sigma_m = p.errore_distanza_m + dist * radianti(p.errore_angolo_deg);
            r.ostacolo = r.profondita_m < quota_m + p.margine_ostacolo_m;
            r.eco = true;
            return r;
        };
        double d_prima = vero.profondita(pos_vera);
        bool primo = false, ostacolo = false;
        for (double h = p.passo_m; h <= parete && !ostacolo; h += p.passo_m) {
            const double d = vero.profondita({pos_vera.x + h * sb, pos_vera.y + h * cb});
            const double pendenza = (d_prima - d) / p.passo_m;   // positiva se il fondo sale verso chi avanza
            d_prima = d;
            if (!std::isfinite(d)) break;                          // fuori dalla griglia
            const double sopra = quota_m - d;                      // il terreno sopra (+) o sotto (-) la chiglia
            if (sopra < h * tan_min || sopra > h * tan_max) continue;   // fuori dal ventaglio d'elevazione
            const double obliqua = std::hypot(h, sopra);
            const bool davanti = sopra > -p.margine_ostacolo_m;   // all'altezza della chiglia o sopra
            const bool e_parete = pendenza > p.pendenza_parete || davanti;
            if (obliqua > (e_parete ? parete : fondo)) {
                if (!e_parete) continue;   // il fondo radente non torna: ma una parete piu' in la' si'
                break;
            }
            if (!primo || davanti) {
                out.push_back(eco(h, sopra, obliqua));
                primo = true;
                ostacolo = davanti;
            }
        }
        if (!primo) {   // niente fino alla portata del fondo lungo il bordo basso del ventaglio
            RitornoFondo r;
            r.t = t;
            r.origine = pos_stimata;
            r.quota_m = quota_m;
            r.rilevamento = normalizza_360(rotta + az);
            r.eco = false;
            r.elevazione = p.elevazione_min_deg;
            r.distanza_m = fondo;
            out.push_back(r);
        }
    }
    return out;
}

}  // namespace m688
