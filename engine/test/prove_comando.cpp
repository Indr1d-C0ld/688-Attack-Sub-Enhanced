#include <doctest.h>

#include "nucleo/comando.hpp"
#include "nucleo/mappa_tasti.hpp"

using namespace m688;

// Posto di comando (docs/11_comandi_e_stazioni.md, fase C2): posizioni dell'originale, ordini e rifiuti

TEST_CASE("posto di comando: timone a scatti di 0,72 gradi/s fino a +-3,6, e disinserisce il pilota") {
    PostoDiComando p;
    StatoProprio s{10.0, 50.0};
    CHECK(p.azione(Azione::pilota_automatico, s).ordine->n == 1);
    CHECK(p.pilota_automatico());
    auto r = p.azione(Azione::timone_dritta, s);
    REQUIRE(r.ordine);
    CHECK(r.ordine->tipo == TipoOrdine::timone);
    CHECK(r.ordine->a == doctest::Approx(0.72));
    CHECK(r.ordine->tempo == 10.0);
    CHECK_FALSE(p.pilota_automatico());
    for (int i = 0; i < 10; ++i) p.azione(Azione::timone_sinistra, s);
    CHECK(p.timone() == -5);
    CHECK_FALSE(p.azione(Azione::timone_sinistra, s).ordine);   // a fondo corsa: niente ordine
    r = p.azione(Azione::timone_centro, s);
    CHECK(r.ordine->a == 0.0);
}

TEST_CASE("posto di comando: telegrafo a 7 posizioni, niente STD sopra i 100 piedi") {
    PostoDiComando p;
    CHECK(p.telegrafo() == PostoDiComando::TELEGRAFO_STOP);
    CHECK(PostoDiComando::nome_telegrafo(6) == "MAX");
    StatoProprio alto{0.0, 20.0};
    CHECK(p.azione(Azione::telegrafo_avanti, alto).ordine->a == doctest::Approx(0.25));
    CHECK(p.azione(Azione::telegrafo_avanti, alto).ordine->a == doctest::Approx(0.5));
    const auto rifiuto = p.azione(Azione::telegrafo_avanti, alto);   // STD a 20 m: no
    CHECK_FALSE(rifiuto.ordine);
    CHECK(rifiuto.rifiuto == MSG_TROPPO_IN_ALTO);
    CHECK(p.telegrafo() == 3);
    StatoProprio fondo{0.0, 60.0};
    CHECK(p.azione(Azione::telegrafo_avanti, fondo).ordine->a == doctest::Approx(0.75));
    CHECK(p.azione(Azione::telegrafo_avanti, fondo).ordine->a == doctest::Approx(1.0));
    CHECK(p.azione(Azione::telegrafo_avanti, fondo).ordine->a == doctest::Approx(1.09375));
    CHECK_FALSE(p.azione(Azione::telegrafo_avanti, fondo).ordine);
    CHECK(p.azione(Azione::telegrafo_indietro, alto).ordine);   // ridurre si puo' sempre
    for (int i = 0; i < 6; ++i) p.azione(Azione::telegrafo_indietro, alto);
    CHECK(p.telegrafo() == PostoDiComando::TELEGRAFO_INDIETRO);
    CHECK(PostoDiComando::posizione_da_velocita(0.5) == 3);
}

TEST_CASE("posto di comando: barra di immersione, automatismi, periscopio solo a quota periscopica") {
    PostoDiComando p;
    StatoProprio s{0.0, 100.0};
    auto r = p.azione(Azione::barra_giu, s);
    CHECK(r.ordine->tipo == TipoOrdine::quota);
    CHECK(r.ordine->a == doctest::Approx(624.2));
    CHECK(r.ordine->b == doctest::Approx(0.25));
    p.azione(Azione::barra_su, s);
    r = p.azione(Azione::barra_su, s);
    CHECK(r.ordine->a == 0.0);
    CHECK(r.ordine->b == doctest::Approx(0.25));
    r = p.azione(Azione::barra_zero, s);   // rateo 0: tiene la quota
    CHECK(r.ordine->b == 0.0);
    CHECK(p.barra() == 0);
    CHECK(p.azione(Azione::quota_150_piedi, s).ordine->tipo == TipoOrdine::quota_150_piedi);
    CHECK(p.azione(Azione::periscopio, s).rifiuto == MSG_SOLO_PERISCOPICA);
    StatoProprio periscopica{0.0, 6.1};
    CHECK(p.azione(Azione::periscopio, periscopica).ordine->n == 1);
    StatoProprio su{0.0, 100.0, true};
    CHECK(p.azione(Azione::periscopio, su).ordine->n == 0);   // abbassarlo si puo' sempre
}

TEST_CASE("tasti: predefiniti, file che li cambia e modello") {
    MappaTasti m = MappaTasti::predefinita();
    CHECK(m.comando("Tastierino 4") == "timone_sinistra");
    CHECK(m.comando("Ctrl+P") == "quota_periscopica");
    CHECK(m.comando("F7") == "stazione_armi");
    CHECK(m.comando("F10") == "uscita_schermo");   // il giro delle uscite a schermo (E9 §2.1)
    CHECK(m.comando("Ctrl+Y").empty());
    m.applica_testo("[tasti]\nlancia = [\"Ctrl+Y\", \"Ctrl+L\"]\nantenna = \"\"\ntimone_sinistra = \"A\"\n");
    CHECK(m.comando("Ctrl+Y") == "lancia");
    CHECK(m.comando("Ctrl+L") == "lancia");
    CHECK(m.comando("L").empty());              // il vecchio tasto e' tolto
    CHECK(m.tasti("antenna").empty());
    CHECK(m.comando("A") == "timone_sinistra"); // preso al tubo 5
    CHECK(m.tasti("tubo_5").empty());
    CHECK_THROWS(m.applica_testo("[tasti]\nnon_esiste = \"P\"\n"));
    CHECK_THROWS(m.applica_testo("[tasti]\nlancia = 3\n"));
    // il modello si rilegge identico
    MappaTasti n = MappaTasti::predefinita();
    n.applica_testo(m.come_toml());
    for (const auto& v : comandi_rimappabili()) CHECK(n.tasti(v.id) == m.tasti(v.id));
}
