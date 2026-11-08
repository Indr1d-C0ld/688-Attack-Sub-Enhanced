// Sonda per la scena S6 (trailer-1, 28/10/2026): il fondale lungo una rotta dal punto d'attesa, ogni 2 km, e la
// distanza dalla rotta d'uscita del Typhoon. Uso: rotta_s6 <barents.fdl> <lat> <lon> <rotta> <km>
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include "nucleo/fondale.hpp"
using namespace m688;
int main(int, char** argv) {
    const Fondale f = Fondale::carica(argv[1]);
    const Vec2 p0 = f.proiettata({std::atof(argv[2]), std::atof(argv[3])});
    const double rotta = std::atof(argv[4]) * 3.14159265358979 / 180.0, km = std::atof(argv[5]);
    const Vec2 a = f.proiettata({69.40, 33.55}), b = f.proiettata({70.1, 33.7});
    const double lx = b.x - a.x, ly = b.y - a.y, l = std::hypot(lx, ly);
    for (double d = 0.0; d <= km * 1000.0; d += 2000.0) {
        const Vec2 p{p0.x + d * std::sin(rotta), p0.y + d * std::cos(rotta)};
        const double t = std::clamp(((p.x - a.x) * lx + (p.y - a.y) * ly) / (l * l), 0.0, 1.0);
        std::printf("%5.0f km: fondale %4.0f m, dalla rotta %4.1f km\n", d / 1000.0, f.profondita(p),
                    std::hypot(p.x - a.x - t * lx, p.y - a.y - t * ly) / 1000.0);
    }
}
