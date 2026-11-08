// I punti di posa degli schemi di boe per la figura di E5 del manuale (kit, 03/11/2026), da nucleo/asw_aereo (posa_schema):
// datum nell'origine, rotta stimata 090, 8 nodi, ritardo 15 minuti, portata della boa 7,4 km, r0 1 miglio.
// Compilare dalla radice:
//   g++ -std=c++20 -O2 -Iengine/nucleo/include tools/manuale/sonda_schemi_boe.cpp engine/build_kit/libnucleo.a -o tools/manuale/sonda_schemi_boe
//   tools/manuale/sonda_schemi_boe > tools/manuale/schemi_boe.txt
#include <cstdio>
#include "nucleo/asw_aereo.hpp"
using namespace m688;
int main() {
    const SchemaBoe schemi[] = {SchemaBoe::barriera, SchemaBoe::chevron, SchemaBoe::cerchio, SchemaBoe::cuneo, SchemaBoe::trappola, SchemaBoe::barriera_circolare};
    for (SchemaBoe s : schemi) {
        ParametriSchema p;
        p.datum = {0, 0};
        p.ritardo_s = 900;
        p.rotta_deg = 90.0;
        const auto v = posa_schema(s, p);
        std::printf("%s", nome_schema(s));
        for (const auto& q : v) std::printf(" %.0f,%.0f", q.x, q.y);
        std::printf("\n");
    }
}
