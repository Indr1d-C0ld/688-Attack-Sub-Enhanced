// Il file .m688v del vetrino di Dio (app/esperimento, docs/28 D2): scritto e riletto uguale, e gli errori.
#include <doctest.h>

#include <filesystem>
#include <fstream>

#include "esperimento.hpp"

using namespace m688;

TEST_CASE("esperimento: il file .m688v si scrive e si rilegge uguale") {
    const auto dir = std::filesystem::temp_directory_path() / "m688_prova_esperimento";
    std::filesystem::create_directories(dir);
    Esperimento e;
    e.titolo = "Il sonar della scorta guasto";
    e.mostra = "Che cosa cambia se la Perry perde il sonar";
    e.codice = "8FZD3Q-4G0000-08K000-0000HZ-G0008J";
    e.semi = 12;
    e.da = 5;
    Intervento a;
    a.tempo = 2400.0;
    a.tipo = "sensore";
    a.unita = 3;
    a.durata_s = 1200.0;
    a.parametri["fattore"] = 0.0;
    a.testi["sensore"] = "sonar";
    Intervento b;
    b.tempo = 600.0;
    b.tipo = "meteo";
    b.punto = Vec2{1000.0, -2000.0};
    b.raggio_m = 15000.0;
    b.parametri["mare"] = 6.0;
    b.innesco = Innesco{"classificazione_ia", 2, 5, std::nullopt, 0.0};
    e.interventi = {a, b};
    scrivi_esperimento(dir / "e.m688v", e);
    const Esperimento r = leggi_esperimento(dir / "e.m688v");
    CHECK(r.titolo == e.titolo);
    CHECK(r.mostra == e.mostra);
    CHECK(r.ipotesi.empty());
    CHECK(r.codice == e.codice);
    CHECK(r.semi == 12);
    CHECK(r.da == 5);
    REQUIRE(r.interventi.size() == 2);
    CHECK(r.interventi[0].id == -1);
    CHECK(r.interventi[0].tipo == "sensore");
    CHECK(r.interventi[0].unita == 3);
    CHECK(r.interventi[0].tempo == doctest::Approx(2400.0));
    CHECK(r.interventi[0].durata_s == doctest::Approx(1200.0));
    CHECK(r.interventi[0].parametri.at("fattore") == doctest::Approx(0.0));
    CHECK(r.interventi[0].testi.at("sensore") == "sonar");
    CHECK_FALSE(r.interventi[0].punto.has_value());
    CHECK(r.interventi[1].tipo == "meteo");
    CHECK(r.interventi[1].unita == -1);
    REQUIRE(r.interventi[1].punto.has_value());
    CHECK(r.interventi[1].punto->x == doctest::Approx(1000.0));
    CHECK(r.interventi[1].punto->y == doctest::Approx(-2000.0));
    CHECK(r.interventi[1].raggio_m == doctest::Approx(15000.0));
    CHECK(r.interventi[1].parametri.at("mare") == doctest::Approx(6.0));
    CHECK_FALSE(r.interventi[0].innesco.has_value());
    REQUIRE(r.interventi[1].innesco.has_value());
    CHECK(r.interventi[1].innesco->evento == "classificazione_ia");
    CHECK(r.interventi[1].innesco->unita == 2);
    CHECK(r.interventi[1].innesco->altra == 5);
    CHECK_FALSE(r.interventi[1].innesco->punto.has_value());
    std::filesystem::remove_all(dir);
}

TEST_CASE("esperimento: senza [esperimento] o senza codice il file e' rifiutato") {
    const auto dir = std::filesystem::temp_directory_path() / "m688_prova_esperimento_errori";
    std::filesystem::create_directories(dir);
    { std::ofstream(dir / "a.m688v") << "[[intervento]]\ntipo = \"togli\"\n"; }
    CHECK_THROWS(leggi_esperimento(dir / "a.m688v"));
    { std::ofstream(dir / "b.m688v") << "[esperimento]\ntitolo = \"x\"\n"; }
    CHECK_THROWS(leggi_esperimento(dir / "b.m688v"));
    CHECK_THROWS(leggi_esperimento(dir / "non_c_e.m688v"));
    std::filesystem::remove_all(dir);
}
