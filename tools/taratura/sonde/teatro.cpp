// Sonda dei teatri (trailer-1, 28/10/2026): i confini in lat/lon e una griglia di profondita' ogni `passo` gradi, per
// scegliere i punti delle missioni della libreria. Uso: teatro <teatro.fdl> [passo_gradi] [lat_s lat_n lon_o lon_e]
#include <cstdio>
#include <cstdlib>
#include "nucleo/fondale.hpp"
using namespace m688;
int main(int argc, char** argv) {
    const Fondale f = Fondale::carica(argv[1]);
    const double passo = argc > 2 ? std::atof(argv[2]) : 0.5;
    const double w = f.larghezza() * f.cella_m(), h = f.altezza() * f.cella_m();
    Geografiche so = f.geografiche({-w / 2, -h / 2}), ne = f.geografiche({w / 2, h / 2});
    std::printf("centro %.3f %.3f  sud-ovest %.3f %.3f  nord-est %.3f %.3f\n", f.centro().lat, f.centro().lon, so.lat, so.lon, ne.lat, ne.lon);
    if (argc > 6) so = {std::atof(argv[3]), std::atof(argv[5])}, ne = {std::atof(argv[4]), std::atof(argv[6])};
    std::printf("       ");
    for (double lon = so.lon; lon <= ne.lon; lon += passo) std::printf("%5.2f", lon);
    std::printf("\n");
    for (double lat = ne.lat; lat >= so.lat; lat -= passo) {
        std::printf("%6.2f ", lat);
        for (double lon = so.lon; lon <= ne.lon; lon += passo) {
            const Vec2 p = f.proiettata({lat, lon});
            if (!f.dentro(p)) std::printf("   . ");
            else std::printf("%4.0f ", f.profondita(p));
        }
        std::printf("\n");
    }
}
