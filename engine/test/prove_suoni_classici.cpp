// Suoni in partita del classico (docs/originale/suoni.md): campioni, regole dei messaggi, periscopio, modi.
#include <doctest.h>

#include <string>
#include <vector>

#include "nucleo/suoni_classici.hpp"

using namespace m688;

namespace {
std::vector<std::string> nomi(const std::vector<SuonoClassico>& v) {
    std::vector<std::string> out;
    for (const auto& s : v)
        out.push_back(s.tipo == SuonoClassico::Tipo::bip ? "bip:" + s.causa : s.campione + "@" + std::to_string(s.frequenza_hz));
    return out;
}
using V = std::vector<std::string>;
}  // namespace

TEST_CASE("suoni classici: i sei .T del disco 2 con la loro provenienza") {
    const DatiSuoniClassici d = carica_suoni_classici(M688_DATI "/suoni_classici.toml", M688_DATI "/..");
    REQUIRE(d.campioni.size() == 6);
    const CampioneClassico* e = d.campione("EXP1");
    REQUIRE(e);
    CHECK(e->nibble.size() == 2 * 5696);
    CHECK(e->origine == "DISK2/EXP1.T");
    CHECK(d.campione("WHALE2")->nibble.size() == 2 * 4424);
    CHECK_FALSE(d.campione("NIENTE"));
    const auto pcm = SuoniClassici::pcm(*e, 2);
    CHECK(pcm.size() == 4 * 5696);
    for (const float x : pcm) CHECK((x >= -1.0f && x <= 1.0f));
    CHECK(carica_suoni_classici("/non/esiste.toml", ".").campioni.empty());
}

TEST_CASE("suoni classici: i messaggi del gestore nell'ordine dell'originale") {
    SuoniClassici s;
    s.messaggio(7);    // a segno: testo, poi EXP1 a 5000
    s.messaggio(26);   // colpiti: testo, poi EXP1 a 4000
    s.messaggio(8);    // ping altrui: sonar1 a 6200, poi testo
    CHECK_FALSE(s.testo_soppresso());
    s.messaggio(8);    // ripetuto: solo il bip grave, niente testo
    CHECK(s.testo_soppresso());
    s.messaggio(3);    // un altro: solo il testo
    s.messaggio(8);    // dopo un altro messaggio torna il campione
    CHECK(nomi(s.prendi()) == V{"bip:testo", "EXP1@5000", "bip:testo", "EXP1@4000", "SONAR1@6200", "bip:testo",
                                "bip:ping_ripetuto", "bip:testo", "SONAR1@6200", "bip:testo"});
    CHECK(s.prendi().empty());
    s.messaggio(50);
    s.messaggio(8);
    s.messaggio(19);
    s.messaggio(8);   // dopo 50 e 19 DS:03C8 e' 0: il secondo 8 non e' un ripetuto
    CHECK(nomi(s.prendi()) == V{"bip:testo", "EXP1@5000", "SONAR1@6200", "bip:testo", "bip:testo", "EXP1@4000",
                                "SONAR1@6200", "bip:testo"});
}

TEST_CASE("suoni classici: ping, lancio, ANALIZZA e i bip") {
    SuoniClassici s(1);
    s.ping_proprio();
    s.lancio_proprio();
    s.analizza(ContattoAnalizzato::da_flag(0x05, 0x12));   // Kara, nave sovietica: prop2 a 2500 + 50 t, 4 volte
    s.analizza(ContattoAnalizzato::da_flag(0x16, 0x29));   // Mk 48, arma: propv2 a 5500 + 10 t, 5 volte
    s.analizza(ContattoAnalizzato::da_flag(0x21, 0x11));   // Foxtrot, sottomarino: propv2 a 2000 + 30 t
    s.analizza(ContattoAnalizzato::da_flag(0x19, 0xC0));   // biologico: whale2 a caso, 2 volte
    const auto v = s.prendi();
    CHECK(nomi(v) == V{"bip:testo", "SONAR1@5500", "TORPRAMP@6500", "bip:testo", "PROP2@2750", "bip:testo",
                       "PROPV2@5720", "bip:testo", "PROPV2@2990", "bip:testo", v.back().campione + "@" +
                       std::to_string(v.back().frequenza_hz)});
    CHECK(v[4].ripetizioni == 4);
    CHECK(v[6].ripetizioni == 5);
    CHECK(v.back().campione == "WHALE2");
    CHECK(v.back().ripetizioni == 2);
    CHECK(v.back().frequenza_hz >= 2500);
    CHECK(v.back().frequenza_hz <= 6499);
    CHECK(v[0].frequenza_hz == 398);   // 1193182 / 3000
    CHECK(v[0].durata_s == doctest::Approx(0.0146).epsilon(0.01));
    const auto onda = SuoniClassici::pcm_bip(v[0], 48000);
    CHECK(onda.size() == 701);
    CHECK(onda.front() == doctest::Approx(SuoniClassici::VOLUME_BIP));
    SuoniClassici t(1);   // lo stesso seme, la stessa balena
    t.analizza(ContattoAnalizzato::da_flag(0x19, 0xC0));
    CHECK(t.prendi().back().frequenza_hz == v.back().frequenza_hz);
}

TEST_CASE("suoni classici: Ctrl-N toglie i campioni, Ctrl-Q anche i bip; il tasto suona sempre") {
    SuoniClassici s;
    s.modo(ModoSuoniClassici::senza_campioni);
    s.tasto_modo();
    s.messaggio(7);
    s.lancio_proprio();
    CHECK(nomi(s.prendi()) == V{"bip:tasto", "bip:testo"});
    s.modo(ModoSuoniClassici::muti);
    s.tasto_modo();
    s.messaggio(26);
    s.ping_proprio();
    CHECK(nomi(s.prendi()) == V{"bip:tasto"});
}

TEST_CASE("suoni classici: al periscopio lo scoppio arriva al quarto ridisegno dell'unita' nel campo") {
    SuoniClassici s;
    s.vista_periscopio(true);
    s.colpo(5, 1);       // primo colpo sopravvissuto
    s.colpo(6, 0xD3);    // distrutta
    s.messaggio(7);      // con la vista a schermo: solo il testo
    CHECK(nomi(s.prendi()) == V{"bip:testo"});
    s.fotogramma_periscopio({5, 6});
    s.fotogramma_periscopio({5});   // la 6 esce dal campo: la sua animazione si ferma
    s.fotogramma_periscopio({5, 6});
    CHECK(s.prendi().empty());
    s.fotogramma_periscopio({5, 6});   // la 5 al quarto ridisegno (stato 4): uno scoppio
    CHECK(nomi(s.prendi()) == V{"EXP1@5000"});
    CHECK(s.stato_colpo(5) == 5);
    CHECK(s.stato_colpo(6) == 0xD6);
    s.fotogramma_periscopio({5, 6});   // la 6 al suo quarto (D6h)
    CHECK(nomi(s.prendi()) == V{"EXP1@5000"});
    for (int i = 0; i < 300; ++i) s.fotogramma_periscopio({5, 6});
    CHECK(s.prendi().empty());           // una volta per colpo
    CHECK(s.stato_colpo(6) == 0xFF);     // affonda e resta FFh
    CHECK(s.stato_colpo(5) >= 0xA0);     // gira fra A0h e C7h
    CHECK(s.stato_colpo(5) <= 0xC7);
    s.colpo(5, 0xC9);                    // colpita di nuovo: l'animazione riparte
    s.vista_periscopio(false);
    for (int i = 0; i < 4; ++i) s.fotogramma_periscopio({5});   // senza la vista non conta
    s.vista_periscopio(true);
    for (int i = 0; i < 4; ++i) s.fotogramma_periscopio({5});
    CHECK(nomi(s.prendi()) == V{"EXP1@5000"});
    s.vista_periscopio(false);
    s.messaggio(50);
    CHECK(nomi(s.prendi()) == V{"bip:testo", "EXP1@5000"});
}
