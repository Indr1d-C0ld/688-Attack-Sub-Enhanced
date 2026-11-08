#include <doctest.h>

#include "coda_messaggi.hpp"

using namespace m688;

TEST_CASE("coda dei messaggi: nessuno si perde, i critici passano davanti, tempo per lunghezza") {
    CodaMessaggi c;
    std::vector<std::pair<double, std::string>> s{{10, "Sonar: nuova riga al LOFAR."}, {10, "Periscopio su."}};
    c.aggiorna(s, 0.0);
    CHECK(c.corrente() == "Sonar: nuova riga al LOFAR.");
    c.aggiorna(s, 1.0);   // non e' ancora scaduto
    CHECK(c.corrente() == "Sonar: nuova riga al LOFAR.");
    c.aggiorna(s, CodaMessaggi::durata("Sonar: nuova riga al LOFAR.") + 0.1);
    CHECK(c.corrente() == "Periscopio su.");
    // un critico interrompe un normale letto da almeno un secondo
    s.push_back({20, "Esplosioni in superficie: ci sparano!"});
    s.push_back({20, "Foto scattata."});
    c.aggiorna(s, 30.0);
    CHECK(c.corrente() == "Esplosioni in superficie: ci sparano!");
    c.aggiorna(s, 30.0 + CodaMessaggi::durata("Esplosioni in superficie: ci sparano!") + 0.1);
    CHECK(c.corrente() == "Foto scattata.");
    // con la coda vuota l'ultimo resta in barra
    c.aggiorna(s, 200.0);
    CHECK(c.corrente() == "Foto scattata.");
    CHECK(CodaMessaggi::durata("x") == doctest::Approx(2.5));
    CHECK(CodaMessaggi::durata(std::string(200, 'x')) == doctest::Approx(7.0));
    // troppi normali: i piu' vecchi saltano, i critici restano
    CodaMessaggi d;
    std::vector<std::pair<double, std::string>> t;
    for (int i = 0; i < 12; ++i) t.push_back({1.0 * i, "Normale " + std::to_string(i)});
    t.push_back({20, "Siluro in acqua!"});
    d.aggiorna(t, 0.0);
    CHECK(d.corrente() == "Siluro in acqua!");
}
