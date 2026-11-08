// Il confronto nel debriefing fra la classificazione acustica e la visiva (docs/25): l'ultima vale, la famiglia conta
// mezza, le automatiche a parte, chi e' arrivato prima
#include <doctest.h>

#include "nucleo/confronto_classificazioni.hpp"

using namespace m688;
using Chi = VoceClassificazione::Chi;

TEST_CASE("confronto classificazioni: famiglie di classi") {
    CHECK(famiglia_classe("victor3") == "victor");
    CHECK(famiglia_classe("victor2") == "victor");
    CHECK(famiglia_classe("los_angeles_flight3") == "los_angeles");
    CHECK(famiglia_classe("los_angeles") == "los_angeles");
    CHECK(famiglia_classe("kirov") == "kirov");
    CHECK(famiglia_classe("type53") == "type");
    CHECK(famiglia_classe("").empty());
}

TEST_CASE("confronto classificazioni: l'ultima vale, la famiglia mezza, le automatiche a parte, chi prima") {
    const std::vector<VoceClassificazione> voci = {
        // contatto 3, un Victor III: il sonarista dice victor2 (famiglia) a 100 s, poi victor3 a 400; al periscopio a 250
        {3, 100.0, Chi::equipaggio, 2, "victor2", false, "victor3"},
        {3, 400.0, Chi::equipaggio, 2, "victor3", true, "victor3"},
        {3, 250.0, Chi::visiva, 2, "victor3", true, "victor3"},
        // contatto 5, un mercantile: il sonarista solo la categoria (giusta), al periscopio sbagliata
        {5, 50.0, Chi::equipaggio, 1, "", true, "mercantile"},
        {5, 60.0, Chi::visiva, 2, "petroliera", false, "mercantile"},
        // contatto 8: solo il giocatore alla stazione sonar, sbagliato; e una automatica del motore
        {8, 30.0, Chi::giocatore_sonar, 2, "kilo", false, "akula"},
        {8, 10.0, Chi::automatica, 2, "akula", true, "akula"},
    };
    const ConfrontoClassificazioni c = confronta_classificazioni(voci);
    REQUIRE(c.contatti.size() == 3);
    const ConfrontoContatto& v3 = c.contatti[0];
    CHECK(v3.contatto == 3);
    CHECK(v3.acustica.esito == EsitoClassificazione::giusta);   // l'ultima (400 s)
    CHECK(v3.acustica.classe == "victor3");
    CHECK(v3.acustica.t_prima_giusta == doctest::Approx(100.0));   // la famiglia gia' a 100 s
    CHECK(v3.visiva.esito == EsitoClassificazione::giusta);
    CHECK(v3.prima == ConfrontoContatto::Prima::acustica);
    const ConfrontoContatto& m5 = c.contatti[1];
    CHECK(m5.acustica.esito == EsitoClassificazione::solo_categoria);
    CHECK(m5.visiva.esito == EsitoClassificazione::sbagliata);
    CHECK(m5.prima == ConfrontoContatto::Prima::acustica);
    const ConfrontoContatto& a8 = c.contatti[2];
    CHECK(a8.acustica.esito == EsitoClassificazione::sbagliata);
    CHECK(a8.visiva.esito == EsitoClassificazione::assente);
    CHECK(a8.prima == ConfrontoContatto::Prima::nessuna);
    CHECK(c.automatiche == 1);
    CHECK(c.acustiche == 3);
    CHECK(c.acustiche_giuste == doctest::Approx(1.5));   // giusta + solo categoria (mezza) + sbagliata
    CHECK(c.visive == 2);
    CHECK(c.visive_giuste == doctest::Approx(1.0));
    CHECK(c.percento_acustica() == doctest::Approx(50.0));
    CHECK(c.percento_visiva() == doctest::Approx(50.0));
    CHECK(c.prima_acustica == 2);
    CHECK(c.prima_visiva == 0);
    CHECK(std::string(nome_esito(EsitoClassificazione::famiglia)) == "della famiglia");
    // vuoto: niente
    const ConfrontoClassificazioni z = confronta_classificazioni({});
    CHECK(z.contatti.empty());
    CHECK(z.percento_visiva() == 0.0);
}
