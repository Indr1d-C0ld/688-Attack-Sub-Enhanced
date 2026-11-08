#include <doctest.h>

#include "nucleo/ordini.hpp"

using namespace m688;

// Ordini del giocatore (docs/11_comandi_e_stazioni.md §2, fase C1)

TEST_CASE("ordini: consegna per istante e per ordine di arrivo, poi nello storico") {
    RegistroOrdini r;
    r.ordina({2.0, TipoOrdine::timone, -3.6});
    r.ordina({1.0, TipoOrdine::telegrafo, 0.5});
    r.ordina({2.0, TipoOrdine::quota, 45.72, 1.0});   // stesso istante del timone: dopo, perche' arrivato dopo
    r.ordina({5.0, TipoOrdine::periscopio, 0, 0, 1});
    CHECK(r.consegna(0.5).empty());
    const auto a = r.consegna(2.0);
    REQUIRE(a.size() == 3);
    CHECK(a[0].tipo == TipoOrdine::telegrafo);
    CHECK(a[1].tipo == TipoOrdine::timone);
    CHECK(a[2].tipo == TipoOrdine::quota);
    CHECK_FALSE(r.vuota());
    CHECK(r.consegna(5.0).size() == 1);
    CHECK(r.vuota());
    CHECK(r.storico().size() == 4);
}

TEST_CASE("ordini: un ordine in ritardo vale al passo in cui arriva, e lo storico lo ricorda cosi'") {
    RegistroOrdini r;
    r.consegna(10.0);
    r.ordina({3.0, TipoOrdine::antenna, 0, 0, 1});    // istante gia' passato
    const auto a = r.consegna(10.1);
    REQUIRE(a.size() == 1);
    CHECK(a[0].tempo == doctest::Approx(10.1));
    CHECK(r.storico().front().tempo == doctest::Approx(10.1));
}

TEST_CASE("ordini: lo storico salvato si rilegge identico e ridà la stessa sequenza di consegne") {
    RegistroOrdini r;
    const double passo = 1.0 / 30.0;
    for (int i = 0; i < 300; ++i) {
        if (i % 37 == 0) r.ordina({i * passo + 0.01, TipoOrdine::timone, 0.36 * (i % 9 - 4)});
        if (i % 53 == 0) r.ordina({i * passo, TipoOrdine::punto_di_rotta, -114475.2 + i, 64925.28 - i / 3.0});
        if (i == 100) r.ordina({0.0, TipoOrdine::designa_bersaglio, 0, 0, 7});   // in ritardo
        r.consegna(i * passo);
    }
    const std::string testo = r.serializza();
    RegistroOrdini replay = RegistroOrdini::da_testo(testo);
    std::vector<Ordine> rigiocati;
    for (int i = 0; i < 300; ++i)
        for (const auto& o : replay.consegna(i * passo)) rigiocati.push_back(o);
    CHECK(rigiocati == r.storico());
    CHECK(replay.serializza() == testo);
    CHECK_THROWS(RegistroOrdini::da_testo("1.0 decolla 0 0 0\n"));
    CHECK_THROWS(RegistroOrdini::da_testo("1.0 timone\n"));
    CHECK(ordine_da_nome("guida_detona") == TipoOrdine::guida_detona);
    CHECK(nome_ordine(TipoOrdine::compressione_tempo) == "compressione_tempo");
}

TEST_CASE("ordini: la classificazione del giocatore nello storico, con la classe come testo") {
    RegistroOrdini r;
    r.ordina(ordine_classifica(12.5, 3, "victor3"));
    r.ordina(ordine_classifica(20.0, 4, "nave mercantile"));
    r.ordina({30.0, TipoOrdine::timone, 1.0});
    r.consegna(100.0);
    const std::string testo = r.serializza();
    CHECK(testo.find("classifica_contatto 0 0 3 victor3\n") != std::string::npos);
    const RegistroOrdini replay = RegistroOrdini::da_testo(testo);
    REQUIRE(replay.storico().empty());
    RegistroOrdini rr = RegistroOrdini::da_testo(testo);
    const auto o = rr.consegna(100.0);
    REQUIRE(o.size() == 3);
    CHECK(o[0].tipo == TipoOrdine::classifica_contatto);
    CHECK(o[0].n == 3);
    CHECK(o[0].testo == "victor3");
    CHECK(o[1].testo == "nave_mercantile");   // niente spazi nella riga
    CHECK(o[2].testo.empty());
    CHECK(o == r.storico());
    CHECK(ordine_da_nome("classifica_contatto") == TipoOrdine::classifica_contatto);
}
