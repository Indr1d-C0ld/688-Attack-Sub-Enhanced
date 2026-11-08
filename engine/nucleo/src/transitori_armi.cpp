#include "nucleo/transitori_armi.hpp"

#include <algorithm>
#include <cmath>
#include <map>

#include "nucleo/casuale.hpp"

namespace m688 {

namespace {

Transitorio t_(int& id, const char* tipo, Vec2 pos, double quota_m, double t0, int origine = -1, double guadagno_db = 0.0) {
    Transitorio x;
    x.id = id++;
    x.tipo = tipo;
    x.pos = pos;
    x.quota_m = quota_m;
    x.t0 = t0;
    x.origine = origine;
    x.guadagno_db = guadagno_db;
    x.seme = static_cast<std::uint64_t>(x.id) * 0x9e3779b97f4a7c15ull + 1;
    return x;
}

constexpr double PULSAZIONI_DB[] = {-14.0, -20.0, -26.0};   // r7 §3.2
constexpr double RAPPORTI_PERIODO[] = {1.0, 0.74, 0.63};
constexpr double IMPLOSIONE_DB = -12.0;                      // l'urto dell'implosione sotto quello dell'esplosione

}  // namespace

std::vector<Transitorio> transitori_lancio(TipoLancio tipo, Vec2 pos, double quota_m, double t0, bool preparazione,
                                           int id_base, std::uint64_t seme) {
    std::vector<Transitorio> out;
    Casuale c(seme ^ 0x1a5c1a5cull);
    int id = id_base;
    if (tipo == TipoLancio::tomahawk_vls) {
        out.push_back(t_(id, "vls_portello", pos, quota_m, t0 - 3.0));
        out.push_back(t_(id, "booster_sub", pos, quota_m, t0));
        return out;
    }
    if (preparazione) {
        out.push_back(t_(id, "tubo_allaga", pos, quota_m, t0 - 35.0));
        out.push_back(t_(id, "tubo_equalizza", pos, quota_m, t0 - 10.0));
    }
    out.push_back(t_(id, "porta_tubo", pos, quota_m, t0 - 5.0));
    out.push_back(t_(id, "sparo_ram", pos, quota_m, t0));
    out.push_back(t_(id, "ram_ritorno", pos, quota_m, t0 + c.uniforme(3.0, 8.0)));
    switch (tipo) {
        case TipoLancio::siluro: out.push_back(t_(id, "avvio_siluro", pos, quota_m, t0 + c.uniforme(0.5, 2.0))); break;
        case TipoLancio::harpoon: out.push_back(t_(id, "capsula_harpoon", pos, 0.0, t0 + c.uniforme(20.0, 60.0))); break;
        case TipoLancio::tomahawk_tubo: out.push_back(t_(id, "booster_sub", pos, quota_m, t0 + c.uniforme(0.5, 1.5))); break;
        case TipoLancio::tomahawk_vls: break;
    }
    return out;
}

double periodo_bolla_s(double tnt_kg, double quota_m) {
    return 2.11 * std::cbrt(std::max(tnt_kg, 1e-3)) / std::pow(std::max(quota_m, 0.0) + 10.1, 5.0 / 6.0);
}

double guadagno_esplosione_db(double tnt_kg) { return 20.0 * 1.13 * std::log10(std::cbrt(std::max(tnt_kg, 1e-3))); }

std::vector<Transitorio> transitori_esplosione(double tnt_kg, Vec2 pos, double quota_m, double t0, int id_base, int origine) {
    std::vector<Transitorio> out;
    int id = id_base;
    const double g = guadagno_esplosione_db(tnt_kg);
    out.push_back(t_(id, "esplosione_urto", pos, quota_m, t0, origine, g));
    if (quota_m < 3.4 * std::cbrt(std::max(tnt_kg, 1e-3))) return out;   // la bolla sfiata in superficie
    const double t1 = periodo_bolla_s(tnt_kg, quota_m);
    double t = t0;
    for (int k = 0; k < 3; ++k) {
        t += RAPPORTI_PERIODO[k] * t1;
        // la voce della bolla sta gia' 14 dB sotto l'urto
        out.push_back(t_(id, "esplosione_bolla", pos, quota_m, t, origine, g + 14.0 + PULSAZIONI_DB[k]));
    }
    return out;
}

double tnt_implosione_kg(double quota_m, double volume_m3) {
    const double p = 1025.0 * 9.81 * std::max(quota_m, 0.0);   // Pa
    return 2.5 * p * std::max(volume_m3, 0.0) / 4.184e6;        // r7 §4.2: (2-3) p V / 4,184 MJ per kg di TNT
}

double volume_implosione_m3(double lunghezza_m, double larghezza_m) {
    return std::clamp(0.29 * std::max(lunghezza_m, 0.0) * larghezza_m * larghezza_m, 300.0, 15000.0);
}

std::vector<Transitorio> transitori_implosione(double quota_m, double volume_m3, Vec2 pos, double t0, int id_base,
                                               std::uint64_t seme, int origine) {
    std::vector<Transitorio> out = transitori_esplosione(tnt_implosione_kg(quota_m, volume_m3), pos, quota_m, t0, id_base, origine);
    for (auto& x : out) x.guadagno_db += IMPLOSIONE_DB;
    int id = id_base + static_cast<int>(out.size());
    Casuale c(seme ^ 0x1b9105e5ull);
    const int n = 10 + static_cast<int>(c.uniforme() * 11.0);   // 10-20 in 200 s (Scorpion: 17)
    std::vector<double> tempi;
    for (int k = 0; k < n; ++k) tempi.push_back(c.uniforme(1.0, 200.0));
    std::sort(tempi.begin(), tempi.end());
    for (double t : tempi)   // in discesa: sempre piu' profondi (~5 m/s, STIMA)
        out.push_back(t_(id, "cedimento_minore", pos, quota_m + 5.0 * t, t0 + t, origine, c.uniforme(-6.0, 6.0)));
    return out;
}

std::vector<Transitorio> transitori_affondamento(Vec2 pos, double t0, double t_fondo, double fondale_m, int id_base,
                                                 std::uint64_t seme, int origine) {
    std::vector<Transitorio> out;
    int id = id_base;
    Casuale c(seme ^ 0xaffa1dull);
    out.push_back(t_(id, "allagamento", pos, 5.0, t0, origine));
    out.push_back(t_(id, "bolle_aria", pos, 5.0, t0 + c.uniforme(0.0, 30.0), origine));
    out.push_back(t_(id, "rottura_nave", pos, 10.0, t0 + c.uniforme(10.0, 120.0), origine));
    if (t_fondo >= t0) out.push_back(t_(id, "urto_fondo", pos, std::max(fondale_m - 1.0, 0.0), t_fondo, origine));
    return out;
}

std::string transitorio_testa(const std::string& classe_arma) {
    static const std::map<std::string, std::string> voci = {
        {"mk48", "ping_testa_mk48"},   {"mk48_adcap", "ping_testa_adcap"}, {"mk46", "ping_testa_mk46"},
        {"uset80", "ping_testa_uset80"}, {"type53", "ping_testa_uset80"}, {"test71m", "ping_testa_set65"},
        {"set65", "ping_testa_set65"}, {"at1", "ping_testa_at1"},       {"tp42", "ping_testa_tp42"},
        {"stingray", "ping_testa_stingray"}};
    const auto it = voci.find(classe_arma);
    return it == voci.end() ? std::string{} : it->second;
}

double intervallo_ping_testa_s(double distanza_m, double portata_m) {
    const double r = std::clamp(distanza_m, 0.0, std::max(portata_m, 1.0));
    return std::max(1.2 * 2.0 * r / 1500.0, 0.16);
}

}  // namespace m688
