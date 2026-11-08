// Sonda per la scena S6 (trailer-1, 28/10/2026): la quota della TB-16 per velocita' e quota del battello, col cavo
// tutto filato, contro il fondale del punto d'attesa (circa 220 m). Uso: schiera_s6 <acustica.toml>
#include <cstdio>
#include "nucleo/acustica.hpp"
#include "nucleo/schiera.hpp"
using namespace m688;
int main(int argc, char** argv) {
    const auto c = carica_acustica(argc > 1 ? argv[1] : "data/motore/acustica.toml");
    const DatiSchiera& d = *c.schiera("tb16");
    for (double q : {40.0, 60.0, 90.0})
        for (double v : {3.0, 4.0, 5.0, 6.0, 8.0}) {
            const FormaSchiera f = forma_schiera(d, v, q, d.cavo_max_m);
            std::printf("quota %3.0f m  %3.0f kn: centro %5.0f m, coda %5.0f m\n", q, v, f.quota_centro_m, f.quota_bassa_m);
        }
}
