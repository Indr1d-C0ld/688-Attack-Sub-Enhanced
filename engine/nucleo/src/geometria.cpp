#include "nucleo/geometria.hpp"

#include <algorithm>
#include <cmath>

namespace m688 {

double normalizza_360(double g) {
    // fmod e' esatto; per |g| < 720 lo e' anche g -+ 360 (Sterbenz): lo stesso risultato senza la chiamata, che nel TMA
    // dell'IA e nei rilevamenti costava parecchio (-360 resta a fmod, che da' -0)
    double r;
    if (g >= 0.0 && g < 360.0) r = g;
    else if (g >= 360.0 && g < 720.0) r = g - 360.0;
    else if (g < 0.0 && g > -360.0) r = g;
    else if (g < -360.0 && g > -720.0) r = g + 360.0;
    else r = std::fmod(g, 360.0);
    if (r < 0.0) r += 360.0;
    return r >= 360.0 ? 0.0 : r;
}

double normalizza_180(double g) {
    double r = normalizza_360(g);
    return r > 180.0 ? r - 360.0 : r;
}

double distanza(Vec2 a, Vec2 b) { return std::hypot(b.x - a.x, b.y - a.y); }

double rilevamento(Vec2 da, Vec2 a) { return normalizza_360(gradi(std::atan2(a.x - da.x, a.y - da.y))); }

AngoloSullaPrua angolo_sulla_prua(Vec2 bersaglio, double rotta_bersaglio, Vec2 osservatore) {
    const double rel = normalizza_180(rilevamento(bersaglio, osservatore) - rotta_bersaglio);
    return {std::fabs(rel), rel >= 0.0};
}

double depressione_orizzonte(double quota_occhio) {
    return std::sqrt(2.0 * std::max(quota_occhio, 0.0) / RAGGIO_TERRA_RIFRAZIONE);
}

double distanza_orizzonte(double quota_occhio) {
    return std::sqrt(2.0 * std::max(quota_occhio, 0.0) * RAGGIO_TERRA_RIFRAZIONE);
}

}  // namespace m688
