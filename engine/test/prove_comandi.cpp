#include <doctest.h>

#include <cmath>
#include <filesystem>
#include <string>

#include "missioni/missione.hpp"
#include "nucleo/ordini.hpp"

using namespace m688;

// Comandi del giocatore nel livello classico (docs/11_comandi_e_stazioni.md, fase C1): ordini del nucleo -> dispatcher
// dell'originale -> gestori parametrici delle missioni convertite.

namespace {
const std::string SCENA = std::string(M688_MISSIONI) + "/SCENE1A.lua";
bool c_e_scena() { return std::filesystem::exists(SCENA); }
const UnitaClassica& giocatore(const Missione& m) { return *m.giocatore(); }
bool messaggio(const Missione& m, int codice) {
    for (const auto& x : m.messaggi()) if (x.codice == codice) return true;
    return false;
}
}  // namespace

TEST_CASE("comandi: telegrafo, timone, quote e periscopio con i gestori della missione" * doctest::skip(!c_e_scena())) {
    Missione m(SCENA, 1);
    REQUIRE(m.giocatore());
    CHECK(m.gestori_giocatore().count("telegrafo"));
    CHECK(m.applica({0, TipoOrdine::telegrafo, 0.75}));
    CHECK(giocatore(m).velocita_ordinata == doctest::Approx(0.75 * giocatore(m).avanti_tutta));
    CHECK(m.applica({0, TipoOrdine::telegrafo, 0, 0, 1}));   // arresto rapido
    CHECK(giocatore(m).velocita_ordinata == 0.0);
    CHECK(m.applica({0, TipoOrdine::timone, -3.6}));
    CHECK(giocatore(m).rateo_virata == doctest::Approx(-3.6));
    CHECK(m.applica({0, TipoOrdine::quota_150_piedi}));
    CHECK(giocatore(m).quota_ordinata == doctest::Approx(45.72));
    CHECK(m.applica({0, TipoOrdine::quota, 120.0, 0.5}));
    CHECK(giocatore(m).quota_ordinata == doctest::Approx(120.0));
    m.avanza(20.0);   // il battello lascia la quota periscopica
    CHECK(giocatore(m).quota > 6.2);
    CHECK(m.applica({0, TipoOrdine::quota_periscopica}));
    CHECK(giocatore(m).quota_ordinata == doctest::Approx(6.096));
    CHECK(m.applica({0, TipoOrdine::periscopio, 0, 0, 1}));
    CHECK((giocatore(m).flag54 & 2) != 0);
    CHECK(m.applica({0, TipoOrdine::periscopio, 0, 0, 0}));
    CHECK((giocatore(m).flag54 & 2) == 0);
    CHECK_FALSE(m.applica({0, TipoOrdine::spegni_siluro}));   // solo Enhanced
    CHECK(m.errori().empty());
}

TEST_CASE("comandi: punti di rotta e pilota automatico, che il timone disinserisce" * doctest::skip(!c_e_scena())) {
    Missione m(SCENA, 1);
    const Vec2 p0 = giocatore(m).pos;
    CHECK_FALSE(m.applica({0, TipoOrdine::pilota_automatico, 0, 0, 1}));   // senza punti non si inserisce
    const Vec2 a{p0.x + 1500, p0.y + 1500}, b{p0.x + 3000, p0.y};
    CHECK(m.applica({0, TipoOrdine::punto_di_rotta, a.x, a.y}));
    CHECK(m.applica({0, TipoOrdine::punto_di_rotta, b.x, b.y}));
    CHECK(m.applica({0, TipoOrdine::telegrafo, 0.5}));
    CHECK(m.applica({0, TipoOrdine::pilota_automatico, 0, 0, 1}));
    CHECK(m.pilota_automatico());
    double vicino_a = 1e9, vicino_b = 1e9;
    for (int i = 0; i < 9 * 1200 && m.pilota_automatico(); ++i) {
        m.avanza(1.0 / 9.0);
        vicino_a = std::min(vicino_a, distanza(giocatore(m).pos, a));
        vicino_b = std::min(vicino_b, distanza(giocatore(m).pos, b));
    }
    CHECK_FALSE(m.pilota_automatico());          // percorsi tutti: si disinserisce
    CHECK(m.punti_di_rotta().empty());
    CHECK(vicino_a < 400.0);                     // soglia d'arrivo dell'originale: 30 quarti di unita' per asse
    CHECK(vicino_b < 400.0);
    // di nuovo, e il timone lo disinserisce
    CHECK(m.applica({0, TipoOrdine::punto_di_rotta, p0.x, p0.y}));
    CHECK(m.applica({0, TipoOrdine::pilota_automatico, 0, 0, 1}));
    CHECK(m.applica({0, TipoOrdine::timone, 1.8}));
    CHECK_FALSE(m.pilota_automatico());
    CHECK(m.applica({0, TipoOrdine::cancella_punto}));
    CHECK(m.punti_di_rotta().empty());
    CHECK(m.errori().empty());
}

TEST_CASE("comandi: lancio dal tubo 1, selezione del siluro e DETONATE dell'originale" * doctest::skip(!c_e_scena())) {
    Missione m(SCENA, 1);
    int bersaglio = -1;
    for (const auto& [id, u] : m.unita()) if (u.categoria == "nave") { bersaglio = id; break; }
    REQUIRE(bersaglio >= 0);
    REQUIRE_FALSE(m.tubi().empty());
    CHECK(m.tubi().front() == "mk48");
    CHECK(m.applica({0, TipoOrdine::designa_bersaglio, 0, 0, bersaglio}));
    CHECK(giocatore(m).bersaglio == bersaglio);
    const auto prima = m.unita().size();
    CHECK(m.applica({0, TipoOrdine::lancia, 0, 0, 1}));
    REQUIRE(m.unita().size() == prima + 1);
    int siluro = -1;
    for (const auto& [id, u] : m.unita()) if (u.classe == "mk48") siluro = id;
    REQUIRE(siluro >= 0);
    CHECK(m.unita().at(siluro).lanciatore == giocatore(m).id);
    CHECK(messaggio(m, 2));                   // "siluro lanciato"
    m.avanza(5.0);
    CHECK(m.applica({0, TipoOrdine::seleziona_siluro, 0, 0, siluro}));
    CHECK(m.applica({0, TipoOrdine::guida_detona}));
    CHECK(messaggio(m, 19));                  // "esplosione subacquea"
    m.avanza(0.2);
    CHECK(m.unita().count(siluro) == 0);      // tolto dal gioco, senza danni
    CHECK(m.colpi().empty());
    CHECK(m.errori().empty());
}

TEST_CASE("comandi: lo storico degli ordini ridà la stessa partita" * doctest::skip(!c_e_scena())) {
    auto gioca = [](RegistroOrdini& r, bool registra) {
        Missione m(SCENA, 4);
        const Vec2 p0 = m.giocatore()->pos;
        if (registra) {
            r.ordina({1.0, TipoOrdine::telegrafo, 0.75});
            r.ordina({1.0, TipoOrdine::quota_150_piedi});
            r.ordina({20.0, TipoOrdine::timone, 2.16});
            r.ordina({60.0, TipoOrdine::punto_di_rotta, p0.x - 2000, p0.y + 1000});
            r.ordina({61.0, TipoOrdine::pilota_automatico, 0, 0, 1});
            r.ordina({140.0, TipoOrdine::periscopio, 0, 0, 1});
        }
        for (int i = 0; i < 9 * 200; ++i) {
            for (const auto& o : r.consegna(m.tempo())) m.applica(o);
            m.avanza(1.0 / 9.0);
        }
        const auto& g = *m.giocatore();
        return std::make_tuple(g.pos.x, g.pos.y, g.rotta, g.quota, m.unita().size(), m.messaggi().size());
    };
    RegistroOrdini r;
    const auto a = gioca(r, true);
    REQUIRE(r.storico().size() == 6);
    RegistroOrdini replay = RegistroOrdini::da_testo(r.serializza());
    const auto b = gioca(replay, false);
    CHECK(a == b);
    RegistroOrdini vuoto;
    CHECK(gioca(vuoto, false) != a);   // senza ordini la partita e' un'altra
}

TEST_CASE("comandi: controlli dell'originale sul giocatore e accostate rallentate" * doctest::skip(!c_e_scena())) {
    Missione m(SCENA, 1);
    const double vmax = giocatore(m).avanti_tutta;
    // a quota periscopica, alla massima: il motore la riduce a meta' di avanti tutta (messaggio 45)
    CHECK(m.applica({0, TipoOrdine::telegrafo, 1.09375}));
    m.avanza(5.0);
    CHECK(giocatore(m).velocita_ordinata <= vmax / 2.0 + 1e-9);
    CHECK(messaggio(m, 45));
    // periscopio su e poi sotto i 20 ft: si abbassa da solo (messaggio 3)
    CHECK(m.applica({0, TipoOrdine::periscopio, 0, 0, 1}));
    m.avanza(2.0);
    CHECK(giocatore(m).velocita_ordinata <= vmax / 4.0 + 1e-9);   // col periscopio su, un quarto
    CHECK(m.applica({0, TipoOrdine::quota_150_piedi}));
    for (int i = 0; i < 9 * 120 && (giocatore(m).flag54 & 2); ++i) m.avanza(1.0 / 9.0);
    CHECK((giocatore(m).flag54 & 2) == 0);
    CHECK(messaggio(m, 3));
    // barra a zero: rateo 0, il battello tiene la quota (DEPTH_TO dell'originale)
    m.avanza(120.0);
    CHECK(m.applica({0, TipoOrdine::quota, 624.2, 0.0}));
    const double q = giocatore(m).quota;
    m.avanza(30.0);
    CHECK(giocatore(m).quota == doctest::Approx(q));
    // punto di rotta dietro le spalle: rallenta a un quarto per accostare (messaggio 47), poi riprende
    const Vec2 p = giocatore(m).pos;
    const double r = radianti(giocatore(m).rotta + 150.0);
    CHECK(m.applica({0, TipoOrdine::telegrafo, 1.0}));
    m.avanza(60.0);
    CHECK(m.applica({0, TipoOrdine::punto_di_rotta, p.x + 6000 * std::sin(r), p.y + 6000 * std::cos(r)}));
    CHECK(m.applica({0, TipoOrdine::pilota_automatico, 0, 0, 1}));
    m.avanza(1.0);
    CHECK(giocatore(m).velocita_ordinata == doctest::Approx(vmax / 4.0));
    CHECK(giocatore(m).velocita_salvata == doctest::Approx(vmax));
    CHECK(messaggio(m, 47));
    m.avanza(120.0);   // virata finita: riprende avanti tutta
    CHECK(giocatore(m).velocita_salvata == 0.0);
    CHECK(giocatore(m).velocita_ordinata == doctest::Approx(vmax));
    CHECK(m.errori().empty());
}
