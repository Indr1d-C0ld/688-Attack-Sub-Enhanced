// Sonda per la scena S6 (strumenti/scene_interazioni): punti del Mare di Barents con almeno 220 m d'acqua (scarpata di
// fango, nucleo/acustica.hpp) a 30-60 km dalla rotta del Typhoon, uscita 69.40N 33.55E -> fuori 70.1N 33.7E, a ovest.
// Uso: fango_s6 <barents.fdl> [fondale minimo, m] [distanza minima, km] [distanza massima, km]
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include "nucleo/fondale.hpp"
using namespace m688;
int main(int argc, char** argv) {
    const Fondale f = Fondale::carica(argv[1]);
    const double minimo = argc > 2 ? std::atof(argv[2]) : 220.0, dmin = argc > 3 ? std::atof(argv[3]) * 1000.0 : 30000.0,
                 dmax = argc > 4 ? std::atof(argv[4]) * 1000.0 : 60000.0;
    const Vec2 a = f.proiettata({69.40, 33.55}), b = f.proiettata({70.1, 33.7});
    const double lx = b.x - a.x, ly = b.y - a.y, l = std::hypot(lx, ly);
    for (double lat = 69.5; lat <= 70.6; lat += 0.05)
        for (double lon = 30.5; lon <= 33.6; lon += 0.1) {
            const Vec2 p = f.proiettata({lat, lon});
            if (!f.dentro(p) || f.profondita(p) < minimo) continue;
            const double t = std::clamp(((p.x - a.x) * lx + (p.y - a.y) * ly) / (l * l), 0.0, 1.0);
            const double d = std::hypot(p.x - a.x - t * lx, p.y - a.y - t * ly);
            if (d < dmin || d > dmax || p.x > a.x) continue;
            std::printf("%.2fN %.1fE  %4.0f m  %4.1f km dalla rotta\n", lat, lon, f.profondita(p), d / 1000.0);
        }
}
