// La figura della firma per il capitolo C4 del manuale (kit, 03/11/2026): la cascata del Museo (BTR e LOFAR) di un
// Victor III che passa al traverso e accelera fino a cavitare, sulla sfera del BQQ-5. Stesso codice del Museo
// (grafica/cascata_firma). Compilare dalla radice:
//   g++ -std=c++20 -O2 -Iengine/grafica/include -Iengine/nucleo/include tools/manuale/figura_firma.cpp \
//       engine/build_kit/libgrafica.a engine/build_kit/libnucleo.a -o tools/manuale/figura_firma
//   tools/manuale/figura_firma data/motore uscita.bmp
#include <string>
#include "grafica/cascata_firma.hpp"
#include "grafica/font.hpp"
#include "grafica/palette.hpp"
#include "grafica/tela.hpp"
#include "nucleo/acustica.hpp"
using namespace m688;
int main(int, char** argv) {
    const std::string dati = argv[1];
    Palette p = Palette::carica(dati + "/palette/giorno.pal");
    applica_palette_cascata(p);
    const Font f = Font::carica(dati + "/font/originale.fnt");
    const CatalogoAcustico c = carica_acustica(dati + "/acustica.toml");
    Tela t(640, 400);
    t.riempi(0);
    disegna_cascata_firma(t, {0, 0, 640, 400}, c, "victor3", 18.0, 0x688u, "bqq5_sfera", &f);
    t.salva_bmp(argv[2], p);
}
