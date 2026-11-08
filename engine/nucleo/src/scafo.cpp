#include "nucleo/scafo.hpp"

#include <cmath>
#include <stdexcept>

#define TOML_EXCEPTIONS 1
#include <toml.hpp>

namespace m688 {

double area_poligono(const Poligono& p, Punto2* baricentro) {
    double a = 0, cy = 0, cz = 0;
    const std::size_t n = p.size();
    for (std::size_t i = 0; i < n; ++i) {
        const auto& u = p[i];
        const auto& v = p[(i + 1) % n];
        const double k = u.y * v.z - v.y * u.z;
        a += k; cy += (u.y + v.y) * k; cz += (u.z + v.z) * k;
    }
    a *= 0.5;
    if (baricentro) {
        if (std::fabs(a) > 1e-12) *baricentro = {cy / (6.0 * a), cz / (6.0 * a)};
        else *baricentro = {};
    }
    return std::fabs(a);
}

Poligono taglia(const Poligono& p, double a, double b, double c) {
    Poligono out;
    const std::size_t n = p.size();
    if (n == 0) return out;
    out.reserve(n + 2);
    for (std::size_t i = 0; i < n; ++i) {
        const auto& u = p[i];
        const auto& v = p[(i + 1) % n];
        const double du = a * u.y + b * u.z - c, dv = a * v.y + b * v.z - c;
        if (du <= 0) out.push_back(u);
        if ((du < 0) != (dv < 0) && std::fabs(du - dv) > 1e-15) {
            const double t = du / (du - dv);
            out.push_back({u.y + (v.y - u.y) * t, u.z + (v.z - u.z) * t});
        }
    }
    return out;
}

Poligono Stazione::sezione() const {
    Poligono p;
    const std::size_t n = semilarghezze.size();
    if (n < 2) return p;
    for (std::size_t k = 0; k < n; ++k) p.push_back({-semilarghezze[k], z_chiglia + dz * static_cast<double>(k)});
    for (std::size_t k = n; k-- > 0;) p.push_back({semilarghezze[k], z_chiglia + dz * static_cast<double>(k)});
    return p;
}

DatiScafo DatiScafo::carica(const std::string& percorso) {
    toml::table t;
    try {
        t = toml::parse_file(percorso);
    } catch (const toml::parse_error& e) {
        throw std::runtime_error(percorso + ": " + std::string(e.description()));
    }
    DatiScafo d;
    const auto& nave = t["nave"];
    d.tipo = nave["tipo"].value_or(std::string{"militare"});
    d.GM = nave["GM_m"].value_or(1.0);
    d.dislocamento_pubblicato_t = nave["dislocamento_pubblicato_t"].value_or(0.0);
    d.squadre = nave["squadre"].value_or(2);
    d.qualita_controllo_danni = nave["qualita_controllo_danni"].value_or(1.0);
    d.resistenza_trave = nave["resistenza_trave"].value_or(1.0);
    d.lunghezza = nave["lunghezza_m"].value_or(0.0);
    d.larghezza = nave["larghezza_m"].value_or(0.0);
    const auto& st = t["stazioni"];
    d.passo = st["passo_m"].value_or(1.0);
    const double dz = st["dz_m"].value_or(0.25);
    const auto* xs = st["x"].as_array();
    const auto* zs = st["z_chiglia"].as_array();
    const auto* hs = st["semilarghezze"].as_array();
    if (!xs || !zs || !hs || xs->size() != zs->size() || xs->size() != hs->size())
        throw std::runtime_error(percorso + ": stazioni incomplete");
    // scafo resistente (sottomarini): tre liste facoltative, una voce per stazione
    const auto* rr = st["raggio_resistente"].as_array();
    const auto* zr = st["z_resistente"].as_array();
    const auto* yr = st["y_resistente"].as_array();
    if ((rr || zr || yr) && !(rr && zr && yr && rr->size() == xs->size() && zr->size() == xs->size() && yr->size() == xs->size()))
        throw std::runtime_error(percorso + ": scafo resistente incompleto");
    for (std::size_t i = 0; i < xs->size(); ++i) {
        Stazione s;
        s.x = (*xs)[i].value_or(0.0);
        s.z_chiglia = (*zs)[i].value_or(0.0);
        s.dz = dz;
        if (rr) {
            s.r_resistente = (*rr)[i].value_or(0.0);
            s.z_resistente = (*zr)[i].value_or(0.0);
            s.y_resistente = (*yr)[i].value_or(0.0);
        }
        if (const auto* h = (*hs)[i].as_array())
            for (const auto& v : *h) s.semilarghezze.push_back(v.value_or(0.0));
        d.stazioni.push_back(std::move(s));
    }
    if (const auto* celle = t["cella"].as_array())
        for (const auto& n : *celle) {
            const auto* c = n.as_table();
            if (!c) continue;
            CellaScafo cs;
            cs.nome = (*c)["nome"].value_or(std::string{});
            cs.uso = (*c)["uso"].value_or(std::string{"locali"});
            cs.deposito = (*c)["deposito"].value_or(std::string{});
            cs.x0 = (*c)["x0"].value_or(0.0); cs.x1 = (*c)["x1"].value_or(0.0);
            cs.y0 = (*c)["y0"].value_or(-1e3); cs.y1 = (*c)["y1"].value_or(1e3);
            cs.permeabilita = (*c)["permeabilita"].value_or(0.95);
            cs.acqua_iniziale = (*c)["acqua_iniziale_m3"].value_or(0.0);
            cs.carica_kg = (*c)["carica_kg"].value_or(0.0);
            cs.protezione = (*c)["protezione"].value_or(0.0);
            cs.propellente_kg = (*c)["propellente_kg"].value_or(0.0);
            cs.carburante_t = (*c)["carburante_t"].value_or(0.0);
            cs.carburante = (*c)["carburante"].value_or(std::string{});
            d.celle.push_back(std::move(cs));
        }
    if (const auto* zone = t["sopra_coperta"].as_array())
        for (const auto& n : *zone) {
            const auto* c = n.as_table();
            if (!c) continue;
            d.sopra_coperta.push_back({(*c)["nome"].value_or(std::string{}), (*c)["carico"].value_or(std::string{}),
                                       (*c)["x0"].value_or(0.0), (*c)["x1"].value_or(0.0), (*c)["carica_kg"].value_or(0.0)});
        }
    if (const auto* zone = t["fuori_resistente"].as_array())
        for (const auto& n : *zone) {
            const auto* c = n.as_table();
            if (!c) continue;
            d.fuori_resistente.push_back({(*c)["nome"].value_or(std::string{}), (*c)["carico"].value_or(std::string{}),
                                          (*c)["x0"].value_or(0.0), (*c)["x1"].value_or(0.0), (*c)["carica_kg"].value_or(0.0)});
        }
    if (const auto* sm = t["sottomarino"].as_table()) {
        auto& s = d.sottomarino;
        s.presente = true;
        s.scafo = (*sm)["scafo"].value_or(std::string{"doppio"});
        s.dislocamento_emersione_t = (*sm)["dislocamento_emersione_t"].value_or(0.0);
        s.dislocamento_immersione_t = (*sm)["dislocamento_immersione_t"].value_or(0.0);
        s.riserva_spinta = (*sm)["riserva_spinta"].value_or(0.0);
        s.quota_collaudo_m = (*sm)["quota_collaudo_m"].value_or(0.0);
        s.quota_collasso_m = (*sm)["quota_collasso_m"].value_or(0.0);
        s.intercapedine_m = (*sm)["intercapedine_m"].value_or(0.0);
        s.volume_resistente_m3 = (*sm)["volume_resistente_m3"].value_or(0.0);
        s.volume_esterno_m3 = (*sm)["volume_esterno_m3"].value_or(0.0);
    }
    if (d.stazioni.empty()) throw std::runtime_error(percorso + ": nessuna stazione");
    return d;
}

void Assetto::coefficienti(double x, double& a, double& b, double& c) const {
    // trim (X, z) -> (X ct - z st, X st + z ct), poi sbandamento (y, Z) -> (y ch - Z sh, y sh + Z ch)
    const double ct = std::cos(appoppamento), st = std::sin(appoppamento), ch = std::cos(sbandamento), sh = std::sin(sbandamento);
    const double X = x - x_o;
    a = ch * st * X - immersione;
    b = sh;
    c = ch * ct;
}

double Assetto::quota(double x, double y, double z) const {
    double a, b, c;
    coefficienti(x, a, b, c);
    return a + b * y + c * z;
}

void Assetto::mondo(double x, double y, double z, double& X, double& Y, double& Z) const {
    const double ct = std::cos(appoppamento), st = std::sin(appoppamento), ch = std::cos(sbandamento), sh = std::sin(sbandamento);
    const double Xr = x - x_o;
    const double x1 = Xr * ct - z * st, z1 = Xr * st + z * ct;
    X = x1 + x_o + dx;
    Y = y * ch - z1 * sh + dy;
    Z = y * sh + z1 * ch - immersione;
}

}  // namespace m688
