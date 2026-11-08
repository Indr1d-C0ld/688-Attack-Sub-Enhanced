#include <doctest.h>

#include <cmath>

#include "nucleo/strumenti_carta.hpp"

using namespace m688;

// Strumenti di plottaggio della carta (nucleo/strumenti_carta.hpp; docs/ricerca/12 §2.4 e §5.2)

TEST_CASE("carta: misura del righello, con il tempo alla velocita' data") {
    const Misura m = misura({0, 0}, {MIGLIO * 3.0, MIGLIO * 4.0}, 10.0 * NODO);
    CHECK(m.distanza_m == doctest::Approx(5.0 * MIGLIO));
    CHECK(m.rilevamento == doctest::Approx(36.87).epsilon(0.001));
    CHECK(m.tempo_s == doctest::Approx(1800.0).epsilon(0.001));   // 5 nm a 10 nodi: mezz'ora
    CHECK(testo_misura(m, 10.0, false, 6) == "037°  5,0 NM  30 MIN A 10 KN");
    CHECK(testo_misura(misura({0, 0}, {0, 1234.0}), 0.0, false, 6) == "000,0°  0,67 NM");
}

TEST_CASE("carta: letture del modo 1989 arrotondate alla tacca della scala") {
    CHECK(distanza_1989(1.234 * MIGLIO, 6) == doctest::Approx(1.2 * MIGLIO));
    CHECK(distanza_1989(1.3 * MIGLIO, 24) == doctest::Approx(1.5 * MIGLIO));
    CHECK(rilevamento_1989(359.6) == doctest::Approx(0.0));
    CHECK(testo_misura(misura({0, 0}, {0, 1234.0}), 0.0, true, 6) == "000°  0,7 NM");
}

TEST_CASE("carta: CPA e TCPA di due moti rettilinei") {
    // noi fermi nell'origine; il bersaglio a 2 km a est e 10 km a nord va verso ovest a 5 m/s: ci passa a nord
    const Cpa c = cpa({0, 0}, {0, 0}, {2000.0, 10000.0}, vettore(270.0, 5.0));
    CHECK(c.tempo_s == doctest::Approx(400.0));
    CHECK(c.distanza_m == doctest::Approx(10000.0));
    CHECK(c.rilevamento == doctest::Approx(0.0).epsilon(1e-6));
    // rotte di collisione: CPA zero
    const Cpa urto = cpa({0, 0}, vettore(90.0, 5.0), {1000.0, 1000.0}, vettore(180.0, 5.0));
    CHECK(urto.distanza_m == doctest::Approx(0.0).epsilon(1e-6));
    CHECK(urto.tempo_s == doctest::Approx(200.0));
    // gia' passato: tempo negativo
    CHECK(cpa({0, 0}, {0, 0}, {0, 1000.0}, vettore(0.0, 5.0)).tempo_s < 0.0);
}

TEST_CASE("carta: il lucido tiene le ultime linee per fonte, gli eventi numerati e i datum") {
    LucidoCarta l;
    for (int i = 0; i < 20; ++i) l.aggiungi_linea({{0, 0}, 10.0 * i, 30.0 * i, "S1", 0});
    l.aggiungi_linea({{0, 0}, 90.0, 5.0, "A", 1});
    CHECK(l.linee().size() == static_cast<std::size_t>(LucidoCarta::MAX_LINEE_PER_FONTE + 1));
    REQUIRE(l.ultima_linea("S1"));
    CHECK(l.ultima_linea("S1")->tempo == doctest::Approx(570.0));
    CHECK(l.ultima_linea("S9") == nullptr);
    CHECK(l.evento(Evento::Tipo::segno, {1, 2}, 10.0) == 1);
    CHECK(l.evento(Evento::Tipo::datum, {1, 2}, 20.0, "A") == 2);
    l.togli_evento();
    CHECK(l.eventi().size() == 1);
    l.aggiungi_datum({{0, 0}, 0.0, 15.0 * NODO, "A"});
    CHECK(raggio_datum(600.0, 15.0 * NODO) == doctest::Approx(600.0 * 15.0 * NODO));
    l.pulisci();
    CHECK(l.linee().empty());
    CHECK(l.eventi().empty());
    CHECK(l.datum().empty());
}

TEST_CASE("carta: regolo di Ekelund, nullo sulla soluzione vera e grande fuori") {
    // il bersaglio parte da (0, 5000) e va verso est a 5 m/s; noi fermi nell'origine prendiamo tre linee
    std::vector<LineaRilevamento> linee;
    for (const double t : {0.0, 300.0, 600.0}) {
        const Vec2 b{5.0 * t, 5000.0};
        linee.push_back({{0, 0}, rilevamento({0, 0}, b), t, "S1", 2});
    }
    const double d = distanza({0, 0}, {3000.0, 5000.0});
    const Ekelund giusto = regolo_ekelund(linee, d, 90.0, 5.0);
    REQUIRE(giusto.tacche.size() == 3);
    CHECK(giusto.scarto_gradi < 1e-6);
    CHECK(distanza(giusto.tacche.front(), {0.0, 5000.0}) < 1e-3);
    CHECK(regolo_ekelund(linee, d, 90.0, 10.0).scarto_gradi > 5.0);   // velocita' sbagliata
    CHECK(regolo_ekelund(linee, d * 0.5, 90.0, 5.0).scarto_gradi > 5.0);   // distanza sbagliata
}

TEST_CASE("carta: retta del tempo-rilevamento e rateo ai minimi quadrati, anche attraverso il nord") {
    std::vector<LineaRilevamento> linee;
    for (int k = 0; k < 5; ++k) linee.push_back({{0, 0}, normalizza_360(356.0 + 2.0 * k), 60.0 * k, "A", 1});   // +2 gradi al minuto
    const auto r = retta_rilevamento(linee, 240.0);
    REQUIRE(r);
    CHECK(r->rateo_gradi_min == doctest::Approx(2.0));
    CHECK(r->rilevamento == doctest::Approx(4.0));
    CHECK_FALSE(retta_rilevamento({linee.front()}, 0.0));
}

TEST_CASE("carta: simboli con la scritta limitata, zone e pulizia del lucido") {
    LucidoCarta l;
    l.aggiungi_segno({{0, 0}, Segno::Affiliazione::ostile, Segno::Dominio::subacqueo, std::string(40, 'A'), 0.0});
    CHECK(l.segni().back().testo.size() == static_cast<std::size_t>(LucidoCarta::MAX_TESTO));
    l.aggiungi_zona({{0, 0}, MIGLIO, true});
    REQUIRE(l.ultimo_segno());
    l.pulisci();
    CHECK(l.segni().empty());
    CHECK(l.zone().empty());
}

TEST_CASE("carta: punto dentro un poligono (zone dello spazio d'acqua)") {
    const std::vector<Vec2> q{{0, 0}, {10, 0}, {10, 10}, {0, 10}};
    CHECK(dentro_poligono(q, {5, 5}));
    CHECK_FALSE(dentro_poligono(q, {15, 5}));
    const std::vector<Vec2> elle{{0, 0}, {10, 0}, {10, 3}, {3, 3}, {3, 10}, {0, 10}};   // a L: il vuoto dell'angolo
    CHECK(dentro_poligono(elle, {1, 8}));
    CHECK_FALSE(dentro_poligono(elle, {8, 8}));
}
