// Le scale delle cascate del sonar (grafica/scala_cascata.hpp): la scala lunga, il dithering ordinato e il fosforo.
#include <doctest.h>

#include <algorithm>
#include <cmath>
#include <set>

#include "grafica/palette.hpp"
#include "grafica/scala_cascata.hpp"
#include "grafica/tela.hpp"

using namespace m688;
namespace cs = m688::cascate;

TEST_CASE("scala delle cascate: frazione e indici nei limiti") {
    CHECK(cs::frazione(50.0, 60.0, 86.0) == 0.0);
    CHECK(cs::frazione(99.0, 60.0, 86.0) == 1.0);
    CHECK(cs::frazione(73.0, 60.0, 86.0) == doctest::Approx(0.5));
    CHECK(cs::frazione(70.0, 80.0, 80.0) == 0.0);   // scala vuota
    for (double u = -0.5; u <= 1.5; u += 0.01)
        for (int x = 0; x < 4; ++x) {
            const int i = cs::indice(u, x, x * 3, cs::SCALA_LUNGA);
            CHECK(i >= 96);
            CHECK(i <= 223);
        }
    CHECK(cs::indice(0.0, 1, 2) == 96);    // il fondo resta nero, senza grana
    CHECK(cs::indice(1.0, 3, 1) == 223);
    CHECK(cs::indice(0.5, 0, 0, cs::SCALA_CORTA, false) == 200 + 16);
}

TEST_CASE("scala delle cascate: il dithering Bayer rende la media della frazione") {
    // fra due gradini la media dei 16 pixel di una cella 4x4 rende la parte frazionaria, a 1/16
    for (const double f : {0.0, 0.25, 0.4, 0.75, 0.9}) {
        const double u = (40.0 + f) / 127.0;
        double somma = 0.0;
        for (int y = 0; y < 4; ++y)
            for (int x = 0; x < 4; ++x) somma += cs::indice(u, x, y) - 96;
        CHECK(somma / 16.0 == doctest::Approx(40.0 + f).epsilon(0.002));
    }
    // le soglie sono le 16 frazioni distinte
    std::set<double> s;
    for (int y = 0; y < 4; ++y)
        for (int x = 0; x < 4; ++x) s.insert(cs::soglia_bayer(x, y));
    CHECK(s.size() == 16);
    CHECK(cs::soglia_bayer(5, 9) == cs::soglia_bayer(1, 1));   // il disegno si ripete ogni 4 pixel: grana ferma
}

TEST_CASE("scala delle cascate: la palette lunga e le tinte") {
    Palette p;
    cs::applica(p, cs::Tinta::fosforo, cs::SCALA_LUNGA);
    CHECK(p.rgb(96) == std::array<std::uint8_t, 3>{0, 0, 0});
    const auto alto = p.rgb(223);
    CHECK(alto[1] == 255);                   // il nucleo del fosforo
    CHECK(alto[0] > 150);
    for (int i = 97; i <= 223; ++i) CHECK(p.rgb(i)[1] >= p.rgb(i - 1)[1]);   // il verde non scende mai
    CHECK(p.rgb(95) == std::array<std::uint8_t, 3>{0, 0, 0});               // fuori dalla scala non tocca niente
    const auto v = cs::colore(cs::Tinta::viridis, 1.0);
    CHECK(v == std::array<std::uint8_t, 3>{0xfd, 0xe7, 0x25});
}

TEST_CASE("scala delle cascate: il fosforo decade, traccia senza buchi e resta nel suo rettangolo") {
    cs::Fosforo f(40, 20);
    f.deponi(5, 5, 1.0);
    f.deponi(5, 5, 0.3);   // tiene il massimo
    CHECK(f.at(5, 5) == doctest::Approx(1.0));
    f.decadi(cs::coefficiente(cs::Persistenza::normale));
    CHECK(f.at(5, 5) == doctest::Approx(0.9));
    CHECK(cs::coefficiente(cs::Persistenza::breve) < cs::coefficiente(cs::Persistenza::lunga));
    f.deponi(-1, 3, 1.0), f.deponi(40, 3, 1.0);   // fuori: ignorati
    f.spegni();
    f.linea(0.0, 2.0, 39.0, 17.0, 1.0);
    for (int x = 0; x < 40; ++x) {   // ogni colonna ha la traccia: niente scalini vuoti
        double c = 0.0;
        for (int y = 0; y < 20; ++y) c += f.at(x, y);
        CHECK(c == doctest::Approx(1.0).epsilon(0.02));
    }
    Tela t(100, 60);
    t.riempi(0);
    f.componi(t, 30, 20, cs::SCALA_LUNGA, cs::Fosforo::Effetti{});
    for (int y = 0; y < 60; ++y)
        for (int x = 0; x < 100; ++x)
            if (x < 30 || x >= 70 || y < 20 || y >= 40) CHECK(t.at(x, y) == 0);
    // le righe dispari valgono meno: a parita' di intensita' l'indice non e' mai piu' alto
    cs::Fosforo pieno(8, 2);
    for (int x = 0; x < 8; ++x) pieno.deponi(x, 0, 0.8), pieno.deponi(x, 1, 0.8);
    cs::Fosforo::Effetti e;
    e.bagliore = 0.0, e.grana = 0.0, e.dithering = false;
    Tela r(8, 2);
    pieno.componi(r, 0, 0, cs::SCALA_LUNGA, e);
    CHECK(r.at(3, 1) < r.at(3, 0));
    // il fondo: a fosforo spento lo schermo resta nero senza velo, e col velo si accende appena
    cs::Fosforo vuoto(16, 16);
    Tela nero(16, 16), velo(16, 16);
    cs::Fosforo::Effetti senza, con;
    con.fondo = 0.07;
    vuoto.componi(nero, 0, 0, cs::SCALA_LUNGA, senza);
    vuoto.componi(velo, 0, 0, cs::SCALA_LUNGA, con);
    int accesi = 0, massimo = 0;
    for (int y = 0; y < 16; ++y)
        for (int x = 0; x < 16; ++x) {
            CHECK(nero.at(x, y) == 96);
            accesi += velo.at(x, y) > 96;
            massimo = std::max(massimo, velo.at(x, y) - 96);
        }
    CHECK(accesi > 64);
    CHECK(massimo <= 16);   // un velo: al piu' un ottavo della scala
}
