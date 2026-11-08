#include "nucleo/sole.hpp"

#include <algorithm>
#include <cmath>

#include "nucleo/geometria.hpp"

namespace m688 {

namespace {
int giorno_dell_anno(int anno, int mese, int giorno) {
    static constexpr int cumulati[] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};
    const bool bisestile = (anno % 4 == 0 && anno % 100 != 0) || anno % 400 == 0;
    return cumulati[mese - 1] + giorno + (bisestile && mese > 2 ? 1 : 0);
}
}  // namespace

double elevazione_sole(const DataOra& t, double latitudine, double longitudine) {
    // anno frazionario (radianti) e da esso equazione del tempo e declinazione (serie di Spencer, NOAA)
    const double g = 2.0 * PI / 365.0 * (giorno_dell_anno(t.anno, t.mese, t.giorno) - 1 + (t.ore_utc - 12.0) / 24.0);
    const double eqt = 229.18 * (0.000075 + 0.001868 * std::cos(g) - 0.032077 * std::sin(g) -
                                 0.014615 * std::cos(2 * g) - 0.040849 * std::sin(2 * g));   // minuti
    const double decl = 0.006918 - 0.399912 * std::cos(g) + 0.070257 * std::sin(g) - 0.006758 * std::cos(2 * g) +
                        0.000907 * std::sin(2 * g) - 0.002697 * std::cos(3 * g) + 0.00148 * std::sin(3 * g);
    const double minuti_veri = t.ore_utc * 60.0 + eqt + 4.0 * longitudine;   // tempo solare vero
    const double angolo_orario = radianti(minuti_veri / 4.0 - 180.0);
    const double lat = radianti(latitudine);
    const double s = std::sin(lat) * std::sin(decl) + std::cos(lat) * std::cos(decl) * std::cos(angolo_orario);
    return gradi(std::asin(std::clamp(s, -1.0, 1.0)));
}

double luce_ambiente(double e) {
    const double t = std::clamp((e + 12.0) / 22.0, 0.0, 1.0);
    return t * t * (3.0 - 2.0 * t);   // raccordo dolce tra buio e pieno giorno
}

}  // namespace m688
