#pragma once
// Elevazione del mare in un punto nel tempo, per la testa del periscopio (docs/14_periscopi.md, P3): quando un'onda
// copre la finestra, la vista si riempie d'acqua e il periscopio non vede. Somma di cinque componenti attorno al
// periodo di picco di Pierson-Moskowitz per mare formato (Hs = 0,0246 U^2, Tp = 0,785 U: Tp = 5 sqrt(Hs)), con varianza
// (Hs / 4)^2. Pura e deterministica: la vista e il modello del posto (nucleo/periscopi) possono usare la stessa.

#include <algorithm>
#include <array>
#include <cmath>

namespace m688 {

// Periodo di picco (s) del mare formato con altezza significativa hs_m
inline double periodo_picco_s(double hs_m) { return 5.0 * std::sqrt(std::max(hs_m, 0.05)); }

// Elevazione della superficie (m, positiva in su) al tempo t; `fase` distingue i punti del mare (0 la testa propria)
inline double elevazione_onda_m(double hs_m, double t, double fase = 0.0) {
    if (hs_m <= 0.0) return 0.0;
    constexpr std::array<double, 5> RAPPORTI{0.8, 0.9, 1.0, 1.15, 1.35};   // frequenze sul picco
    constexpr std::array<double, 5> FASI{0.0, 2.1, 4.4, 1.3, 5.6};
    const double w0 = 2.0 * 3.14159265358979323846 / periodo_picco_s(hs_m);
    const double a = (hs_m / 4.0) * std::sqrt(2.0 / static_cast<double>(RAPPORTI.size()));   // varianza (Hs/4)^2
    double e = 0.0;
    for (std::size_t k = 0; k < RAPPORTI.size(); ++k) e += a * std::sin(RAPPORTI[k] * w0 * t + FASI[k] + fase * static_cast<double>(k + 1));
    return e;
}

}  // namespace m688
