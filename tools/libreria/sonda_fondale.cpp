// Sonda dei fondali dei teatri, per piazzare le unita' delle missioni della libreria (docs/17).
//   g++ -std=c++20 -O1 -Iengine/nucleo/include -DM688_DATI_SONDA=\"$PWD/data/motore\" tools/libreria/sonda_fondale.cpp <build>/libnucleo.a -o <dove>/sonda_fondale
//   sonda_fondale punti TEATRO LAT LON [LAT LON ...]        posizione proiettata e profondita' di ogni punto
//   sonda_fondale mappa TEATRO LAT0 LAT1 LON0 LON1 PASSO      carta ASCII (# terra . <20 : <50 - <100 = <200 ~ <1000 @ oltre)
//   sonda_fondale tratti TEATRO LAT LON LAT LON [...]         profondita' minima lungo ogni tratto di una rotta
#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>

#include "nucleo/fondale.hpp"

using namespace m688;

int main(int argc, char** argv) {
    if (argc < 3) {
        std::fprintf(stderr, "uso: sonda_fondale punti|mappa|tratti TEATRO ...\n");
        return 2;
    }
    const std::string modo = argv[1];
    const Fondale f = Fondale::carica(std::string(M688_DATI_SONDA) + "/teatri/" + argv[2] + ".fdl");
    const auto num = [&](int i) { return std::atof(argv[i]); };
    if (modo == "punti") {
        const auto g0 = f.geografiche(f.centro_cella(0, 0)), g1 = f.geografiche(f.centro_cella(f.larghezza() - 1, f.altezza() - 1));
        std::printf("%s: %dx%d celle da %.0f m, NW %.2f %.2f, SE %.2f %.2f\n", argv[2], f.larghezza(), f.altezza(), f.cella_m(), g0.lat, g0.lon,
                    g1.lat, g1.lon);
        for (int i = 3; i + 1 < argc; i += 2) {
            const Vec2 p = f.proiettata({num(i), num(i + 1)});
            std::printf("  %.3f %.3f -> (%.0f, %.0f) dentro %d profondita' %.0f\n", num(i), num(i + 1), p.x, p.y, f.dentro(p), f.profondita(p));
        }
    } else if (modo == "mappa" && argc >= 8) {
        for (double la = num(4); la >= num(3) - 1e-9; la -= num(7)) {
            std::printf("%6.2f ", la);
            for (double lo = num(5); lo <= num(6) + 1e-9; lo += 2.0 * num(7)) {
                const Vec2 p = f.proiettata({la, lo});
                const double d = f.profondita(p);
                std::putchar(!f.dentro(p) ? ' ' : d < 0 ? '#' : d < 20 ? '.' : d < 50 ? ':' : d < 100 ? '-' : d < 200 ? '=' : d < 1000 ? '~' : '@');
            }
            std::printf("\n");
        }
    } else if (modo == "tratti") {
        std::vector<Vec2> p;
        for (int i = 3; i + 1 < argc; i += 2) p.push_back(f.proiettata({num(i), num(i + 1)}));
        for (std::size_t i = 0; i + 1 < p.size(); ++i) {
            double minima = 1e9;
            Vec2 dove{};
            for (int k = 0; k <= 200; ++k) {
                const Vec2 q{p[i].x + (p[i + 1].x - p[i].x) * k / 200.0, p[i].y + (p[i + 1].y - p[i].y) * k / 200.0};
                if (f.profondita(q) < minima) { minima = f.profondita(q); dove = q; }
            }
            const auto g = f.geografiche(dove);
            std::printf("tratto %zu: minima %.0f m a %.3f %.3f\n", i, minima, g.lat, g.lon);
        }
    } else {
        std::fprintf(stderr, "modo sconosciuto: %s\n", modo.c_str());
        return 2;
    }
    return 0;
}
