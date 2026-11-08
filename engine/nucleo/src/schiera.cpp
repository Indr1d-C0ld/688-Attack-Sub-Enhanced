#include "nucleo/schiera.hpp"

#include <algorithm>
#include <cmath>

namespace m688 {

namespace {

constexpr double RHO = 1025.0;                 // kg/m^3
constexpr double MARGINE_FONDO_M = 5.0;        // dossier 04 §6.3: contatto entro 5 m dal fondale
constexpr double TAU_MIN_S = 60.0, TAU_MAX_S = 600.0;   // STIMA
constexpr double STROUHAL = 0.2;
constexpr double VIBRAZIONE_MIN_KN = 2.0;

}  // namespace

double angolo_critico_deg(const DatiSchiera& d, double v_kn) {
    const double v = std::max(v_kn, 0.0) * NODO;
    const double r = 0.5 * RHO * d.cn * d.cavo_diametro_m * v * v;
    if (r <= 1e-9) return 90.0;
    const double w = d.peso_cavo_n_m;
    const double c = (-w + std::sqrt(w * w + 4.0 * r * r)) / (2.0 * r);   // cos(phi), Pode
    return gradi(std::acos(std::clamp(c, 0.0, 1.0)));
}

double flessione_m(const DatiSchiera& d, double v_kn) {
    return d.schiera_m * std::min(1.0, d.v_flessione_kn / std::max(v_kn, 1e-3));
}

namespace {

// la forma con una discesa del cavo data (quella d'equilibrio o quella raggiunta)
FormaSchiera forma_con(const DatiSchiera& d, double v_kn, double quota_battello, double cavo, double discesa_cavo) {
    FormaSchiera f;
    const double phi = radianti(angolo_critico_deg(d, v_kn));
    f.angolo_cavo_deg = gradi(phi);
    const double fless = flessione_m(d, v_kn);
    // il cavo: dritto all'angolo critico, con la discesa data (il ritardo la scala), lungo `cavo`
    const double dz = std::clamp(discesa_cavo, 0.0, cavo);
    const double dx = std::sqrt(std::max(cavo * cavo - dz * dz, 0.0));
    for (int i = 0; i < PUNTI_CAVO; ++i) {
        const double u = static_cast<double>(i) / (PUNTI_CAVO - 1);
        f.profilo.emplace_back(dx * u, quota_battello + dz * u);
    }
    // la schiera: dalla testa alla coda, flette di `fless` (parabola), lungo l'orizzontale quanto la sua lunghezza
    const double xs = std::sqrt(std::max(d.schiera_m * d.schiera_m - fless * fless, 0.0));
    for (int i = 1; i <= PUNTI_SCHIERA; ++i) {
        const double u = static_cast<double>(i) / PUNTI_SCHIERA;
        f.profilo.emplace_back(dx + xs * u, quota_battello + dz + fless * u * u);
    }
    f.quota_centro_m = quota_battello + dz + 0.25 * fless;   // a meta' schiera la parabola vale un quarto
    f.quota_bassa_m = quota_battello + dz + fless;
    f.dietro_m = dx + 0.5 * xs;
    // oltre la quota massima la schiera e' tenuta li' (il runtime la danneggia)
    if (f.quota_centro_m > d.quota_max_m) {
        const double giu = f.quota_centro_m - d.quota_max_m;
        f.oltre_quota_max = true;
        f.quota_centro_m -= giu;
        f.quota_bassa_m -= giu;
        for (auto& [x, z] : f.profilo) z = std::max(quota_battello, z - giu * std::clamp((z - quota_battello) / std::max(dz + fless, 1.0), 0.0, 1.0));
    }
    return f;
}

double cavo_in_acqua(const DatiSchiera& d, double cavo_filato_m) { return std::clamp(cavo_filato_m, 0.0, d.cavo_max_m); }

}  // namespace

FormaSchiera forma_schiera(const DatiSchiera& d, double v_kn, double quota_battello_m, double cavo_filato_m) {
    const double cavo = cavo_in_acqua(d, cavo_filato_m);
    return forma_con(d, v_kn, quota_battello_m, cavo, cavo * std::sin(radianti(angolo_critico_deg(d, v_kn))));
}

double tempo_schiera_s(const DatiSchiera& d, double v_kn, double cavo_filato_m) {
    const double v = std::max(v_kn, 0.1) * NODO;
    return std::clamp((cavo_in_acqua(d, cavo_filato_m) + d.schiera_m) / v, TAU_MIN_S, TAU_MAX_S);
}

FormaSchiera CavoSchiera::avanza(const DatiSchiera& d, double dt, double v_kn, double quota_battello_m, double cavo_filato_m) {
    const double cavo = cavo_in_acqua(d, cavo_filato_m);
    const double equilibrio = cavo * std::sin(radianti(angolo_critico_deg(d, v_kn)));
    if (discesa_m < 0.0) discesa_m = equilibrio;   // la prima volta: gia' in equilibrio
    const double k = std::exp(-std::max(dt, 0.0) / tempo_schiera_s(d, v_kn, cavo_filato_m));
    discesa_m = equilibrio + (std::min(discesa_m, cavo) - equilibrio) * k;   // il cavo recuperato la accorcia subito
    return forma_con(d, v_kn, quota_battello_m, cavo, discesa_m);
}

double cavo_sicuro_m(const DatiSchiera& d, double v_kn, double quota_battello_m, double fondale_m, double margine_m) {
    const auto ci_sta = [&](double cavo) { return forma_schiera(d, v_kn, quota_battello_m, cavo).quota_bassa_m <= fondale_m - margine_m; };
    if (ci_sta(d.cavo_max_m)) return d.cavo_max_m;
    if (!ci_sta(0.0)) return 0.0;
    double lo = 0.0, hi = d.cavo_max_m;   // la coda scende col cavo: bisezione al metro
    while (hi - lo > 1.0) {
        const double m = 0.5 * (lo + hi);
        (ci_sta(m) ? lo : hi) = m;
    }
    return lo;
}

double cavo_per_quota_m(const DatiSchiera& d, double v_kn, double quota_battello_m, double quota_voluta_m) {
    const auto quota = [&](double cavo) { return forma_schiera(d, v_kn, quota_battello_m, cavo).quota_centro_m; };
    const double voluta = std::min(quota_voluta_m, d.quota_max_m);
    if (quota(d.cavo_max_m) <= voluta) return d.cavo_max_m;
    if (quota(0.0) >= voluta) return 0.0;
    double lo = 0.0, hi = d.cavo_max_m;   // la quota scende col cavo: bisezione al metro
    while (hi - lo > 1.0) {
        const double m = 0.5 * (lo + hi);
        (quota(m) <= voluta ? lo : hi) = m;
    }
    return lo;
}

ContattoFondo contatto_fondo(const FormaSchiera& f, const std::vector<double>& profondita) {
    ContattoFondo c;
    const std::size_t n = std::min(f.profilo.size(), profondita.size());
    int schiera = 0, sul_fondo = 0;
    for (std::size_t i = 0; i < n; ++i) {
        const double fondale = profondita[i];
        const bool tocca = std::isfinite(fondale) && fondale > 0.0 && f.profilo[i].second >= fondale - MARGINE_FONDO_M;
        const bool di_schiera = i >= static_cast<std::size_t>(PUNTI_CAVO);
        if (di_schiera) {
            ++schiera;
            sul_fondo += tocca;
        } else if (tocca && i > 0) {
            c.cavo_m += distanza(Vec2{f.profilo[i - 1].first, f.profilo[i - 1].second}, Vec2{f.profilo[i].first, f.profilo[i].second});
        }
        c.tocca = c.tocca || tocca;
    }
    c.frazione_schiera = schiera > 0 ? static_cast<double>(sul_fondo) / schiera : 0.0;
    return c;
}

std::vector<double> frequenze_vibrazione(double d, double v_kn) {
    if (d <= 0.0 || v_kn < VIBRAZIONE_MIN_KN) return {};
    const double f = STROUHAL * v_kn * NODO / d;
    return {f, 2.0 * f};
}

}  // namespace m688
