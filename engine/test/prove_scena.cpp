#include <doctest.h>

#include <filesystem>

#include "grafica/palette.hpp"
#include "grafica/riscala.hpp"
#include "nucleo/mondo.hpp"
#include "nucleo/sole.hpp"

using namespace m688;
using doctest::Approx;

TEST_CASE("elevazione del Sole") {
    // equinozio di primavera, mezzogiorno UTC sul meridiano di Greenwich all'equatore: Sole quasi allo zenit
    CHECK(elevazione_sole({1989, 3, 20, 12.0}, 0.0, 0.0) == Approx(88.0).epsilon(0.03));
    // solstizio d'estate al Circolo polare artico (66,56 N): a mezzanotte locale il Sole sfiora l'orizzonte
    CHECK(elevazione_sole({1989, 6, 21, 0.0}, 66.56, 0.0) == Approx(0.0).epsilon(1.0));
    // meta' ottobre alle Faroe: notte fonda a mezzanotte, giorno a mezzogiorno locale (~12:30 UTC)
    CHECK(elevazione_sole({1989, 10, 15, 0.5}, 61.5, -7.0) < -30.0);
    const double mezzogiorno = elevazione_sole({1989, 10, 15, 12.5}, 61.5, -7.0);
    CHECK(mezzogiorno == Approx(90.0 - 61.5 - 8.5).epsilon(0.05));   // declinazione a meta' ottobre ~ -8,5
}

TEST_CASE("luce ambiente") {
    CHECK(luce_ambiente(-30.0) == Approx(0.0));
    CHECK(luce_ambiente(30.0) == Approx(1.0));
    CHECK(luce_ambiente(-1.0) == Approx(0.5));
    CHECK(luce_ambiente(-5.0) < luce_ambiente(0.0));
}

TEST_CASE("Scale2x: le diagonali restano nitide, i blocchi pieni non cambiano") {
    // 2x2: diagonale 1 su sfondo 0
    ImmagineIndicizzata s{2, 2, {1, 0, 0, 1}};
    const auto d = scale2x(s);
    REQUIRE(d.w == 4);
    // regole EPX calcolate a mano: la diagonale diventa la classica scaletta raddoppiata
    CHECK(d.pix == std::vector<std::uint8_t>{1, 1, 0, 0,
                                            1, 0, 1, 0,
                                            0, 1, 0, 1,
                                            0, 0, 1, 1});
    ImmagineIndicizzata pieno{3, 3, std::vector<std::uint8_t>(9, 7)};
    for (auto c : scale2x(pieno).pix) CHECK(c == 7);
    // raddoppio nitido: ogni pixel diventa un blocco 2x2
    const auto n = raddoppia_nitido(s);
    CHECK(n.pix[0] == 1);
    CHECK(n.pix[1] == 1);
    CHECK(n.pix[2] == 0);
}

TEST_CASE("stiramento 5:6") {
    ImmagineIndicizzata s{1, 5, {0, 1, 2, 3, 4}};
    const auto d = stira_verticale(s, 6);
    REQUIRE(d.pix.size() == 6);
    CHECK(d.pix == std::vector<std::uint8_t>{0, 0, 1, 2, 3, 4});   // una riga su cinque viene duplicata
}

TEST_CASE("mondo interpolato tra due passi") {
    Mondo m;
    Nave n;
    n.rotta = n.rotta_ordinata = 90.0;
    n.velocita = n.velocita_ordinata = 10.0;
    m.aggiungi(n);
    m.passo(1.0);
    const auto meta = m.interpolato(0.5);
    CHECK(meta.navi()[0].pos.x == Approx(5.0));
    CHECK(m.interpolato(1.0).navi()[0].pos.x == Approx(10.0));
    CHECK(m.interpolato(0.0).navi()[0].pos.x == Approx(0.0));
}

TEST_CASE("palette illuminata: la vista si scurisce, l'interfaccia no" *
          doctest::skip(!std::filesystem::exists(M688_DATI "/palette/giorno.pal"))) {
    const auto giorno = Palette::carica(M688_DATI "/palette/giorno.pal");
    const auto notte = giorno.illuminata(0.0, {{144, 158}, {199, 247}});
    CHECK(notte.rgb(150)[0] < giorno.rgb(150)[0] / 5);     // grigio degli sprite molto scurito
    CHECK(notte.rgb(220)[2] < giorno.rgb(220)[2] / 5);     // cielo
    CHECK(notte.rgb(15) == giorno.rgb(15));                // bianco dell'interfaccia invariato
    CHECK(notte.rgb(150)[2] >= notte.rgb(150)[0]);         // al buio prevale il blu
    const auto pieno = giorno.illuminata(1.0, {{144, 158}});
    CHECK(pieno.rgb(150) == giorno.rgb(150));
    const auto meta = Palette::mescola(giorno, notte, 0.5);
    CHECK(meta.rgb(150)[0] < giorno.rgb(150)[0]);
}
