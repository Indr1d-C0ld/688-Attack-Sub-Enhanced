#include "nucleo/fari.hpp"

#include <cmath>
#include <cstdint>
#include <filesystem>
#include <stdexcept>
#include <toml.hpp>

namespace m688 {

std::vector<Faro> carica_fari(const std::string& percorso, const std::string& teatro) {
    std::vector<Faro> out;
    if (!std::filesystem::exists(percorso)) return out;
    toml::table t;
    try {
        t = toml::parse_file(percorso);
    } catch (const toml::parse_error& e) {
        throw std::runtime_error(percorso + ": " + std::string(e.description()));
    }
    const auto* elenco = t["faro"].as_array();
    if (!elenco) return out;
    for (const auto& nodo : *elenco) {
        const auto* x = nodo.as_table();
        if (!x) continue;
        Faro f;
        f.teatro = (*x)["teatro"].value_or(std::string{});
        if (!teatro.empty() && f.teatro != teatro) continue;
        f.id = (*x)["id"].value_or(std::string{});
        f.nome = (*x)["nome"].value_or(std::string{});
        f.caratteristica = (*x)["caratteristica"].value_or(std::string{});
        f.lat = (*x)["lat"].value_or(0.0);
        f.lon = (*x)["lon"].value_or(0.0);
        f.quota_m = (*x)["quota_m"].value_or(20.0);
        f.portata_m = (*x)["portata_nm"].value_or(15.0) * 1852.0;
        const std::string c = (*x)["colore"].value_or(std::string{"bianco"});
        f.colore = c == "rosso" ? luci::Colore::rosso : c == "verde" ? luci::Colore::verde : c == "giallo" ? luci::Colore::giallo
                                                                                               : luci::Colore::bianco;
        f.periodo_s = (*x)["periodo_s"].value_or(0.0);
        if (const auto* l = (*x)["lampi"].as_array())
            for (const auto& d : *l) f.lampi.push_back(d.value_or(0.0));
        f.anno = static_cast<int>((*x)["anno"].value_or(0));
        out.push_back(std::move(f));
    }
    return out;
}

bool faro_acceso(const Faro& f, double t) {
    if (!(f.periodo_s > 0.0) || f.lampi.empty()) return true;   // luce fissa
    // lo sfasamento: dall'id (FNV-1a), una frazione stabile del periodo
    std::uint32_t h = 2166136261u;
    for (const char c : f.id) h = (h ^ static_cast<std::uint8_t>(c)) * 16777619u;
    double s = std::fmod(t + f.periodo_s * (h % 1000) / 1000.0, f.periodo_s);
    bool acceso = true;
    for (const double d : f.lampi) {
        if (s < d) return acceso;
        s -= d;
        acceso = !acceso;
    }
    return false;
}

bool faro_coperto(const std::function<double(Vec2)>& quota, double passo_m, Vec2 occhio, double quota_occhio_m, Vec2 faro,
                  double quota_faro_m) {
    constexpr double R_EFFICACE = 4.0 / 3.0 * 6371000.0, CAPO_M = 300.0;
    const double D = std::hypot(faro.x - occhio.x, faro.y - occhio.y);
    if (D <= CAPO_M || passo_m <= 0.0) return false;
    const int n = static_cast<int>((D - CAPO_M) / passo_m);
    for (int k = 1; k <= n; ++k) {
        const double s = k * passo_m, t = s / D;
        const double retta = quota_occhio_m + (quota_faro_m - quota_occhio_m) * t;
        const double rigonfio = s * (D - s) / (2.0 * R_EFFICACE);   // la Terra che si alza fra i due
        if (quota({occhio.x + (faro.x - occhio.x) * t, occhio.y + (faro.y - occhio.y) * t}) + rigonfio > retta) return true;
    }
    return false;
}

}  // namespace m688
