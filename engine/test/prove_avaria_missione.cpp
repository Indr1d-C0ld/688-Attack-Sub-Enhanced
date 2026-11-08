#include <doctest.h>

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <string>

#include "missioni/missione.hpp"
#include "missioni/registro_missione.hpp"
#include "nucleo/geometria.hpp"

using namespace m688;
namespace fs = std::filesystem;

// M6 del dossier 23 (schede S04 e S11 di docs/17): unita' dell'IA in avaria, incagliate e rimorchiate.

namespace {

const std::string MODELLO = "modello = { massima_kn = 20, virata_gradi_s = 2, accel_kn_s = 0.5, quota_m_s = 2, quota_max_m = 300, integrita = 200 }";

// Il Foxtrot sugli scogli (2), un Koni che lo prende a rimorchio a 10 s (3) e va a est a 15 nodi, un Kilo in avaria a
// 5 nodi (4); il 688 lontano
std::string scena(const std::string& stato_foxtrot = "incagliata") {
    const auto f = fs::temp_directory_path() / "m688_prova_avaria.lua";
    std::ofstream(f) << "return { id = 'AVARIA', teatro = 'prova', script = {"
                        " R = function(u) m688.variabile(u, 1, m688.rimorchia(u, 2, 8, 300) and 1 or 0)"
                        "   m688.variabile(u, 2, m688.rimorchia(u, 2) and 1 or 0) end,"
                        " Q = function(u) m688.variabile(u, 3, m688.rimorchiata(u) and 1 or 0)"
                        "   m688.variabile(u, 4, m688.in_avaria(u) and 1 or 0) end,"
                        " A = function(u) m688.avaria(u) end,"
                        " M = function(u) m688.variabile(u, 5, m688.molla_rimorchio(u) and 1 or 0) end },"
                        " varianti = { { probabilita = 1, unita = {\n"
                        "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true, posizione = { x = -30000, y = 0 },"
                        "  quota_m = 100, rotta = 0, velocita_kn = 0, " << MODELLO << " },"
                        "{ id = 2, classe = 'foxtrot', categoria = 'sottomarino', flag = 18, posizione = { x = 0, y = 0 },"
                        "  quota_m = 20, rotta = 0, velocita_kn = 0, stato = '" << stato_foxtrot << "', " << MODELLO << " },"
                        "{ id = 3, classe = 'koni', categoria = 'nave', flag = 10, posizione = { x = 400, y = 0 },"
                        "  quota_m = 0, rotta = 90, velocita_kn = 15, " << MODELLO << " },"
                        "{ id = 4, classe = 'kilo', categoria = 'sottomarino', flag = 18, posizione = { x = 0, y = 5000 },"
                        "  quota_m = 100, rotta = 0, velocita_kn = 5, stato = 'avaria', " << MODELLO << " },"
                        "\n}, timer = { { dopo_s = 1, unita = 3, gestore = 'R' }, { dopo_s = 120, unita = 2, gestore = 'Q' },"
                        "  { dopo_s = 200, unita = 3, gestore = 'A' }, { dopo_s = 400, unita = 2, gestore = 'M' } } } } }\n";
    return f.string();
}

int var(const Missione& m, int i) { const auto it = m.variabili().find(i); return it == m.variabili().end() ? -1 : it->second; }

int quanti(const Missione& m, TipoEvento t, int livello = -1) {
    const auto& ev = m.registro().eventi();
    return static_cast<int>(std::count_if(ev.begin(), ev.end(), [&](const EventoMissione& e) { return e.tipo == t && (livello < 0 || e.livello == livello); }));
}

void avanza(Missione& m, double s) { for (long i = 0; i < static_cast<long>(s * 9.0); ++i) m.avanza(1.0 / 9.0); }

}  // namespace

TEST_CASE("M6: il Foxtrot sugli scogli preso a rimorchio dal Koni; il Kilo in avaria si ferma") {
    const auto f = scena();
    Missione m(f, 1, false);
    REQUIRE(m.errori().empty());
    CHECK(m.incagliata_ia(2));
    CHECK(m.in_avaria(4));
    CHECK_FALSE(m.avaria(0));      // il giocatore no: le sue avarie sono dei danni
    CHECK_FALSE(m.incaglia(0));
    const Vec2 scogli = m.unita().at(2).pos;
    avanza(m, 0.5);
    CHECK(distanza(m.unita().at(2).pos, scogli) < 0.01);   // fermo dove si trova
    avanza(m, 1.0);
    CHECK(var(m, 1) == 1);         // il rimorchio parte
    CHECK(var(m, 2) == 0);         // gia' rimorchiato
    CHECK(m.rimorchiatore_di(2) == 3);
    CHECK_FALSE(m.incagliata_ia(2));

    avanza(m, 150.0);
    const auto& fox = m.unita().at(2);
    const auto& koni = m.unita().at(3);
    CHECK(koni.velocita <= 8.0 + 1e-9);   // il rimorchiatore non supera la velocita' del rimorchio
    CHECK(distanza(fox.pos, koni.pos) == doctest::Approx(300.0).epsilon(0.02));   // il cavo teso
    CHECK(fox.pos.x > 100.0);             // tirato via verso est, sulla scia
    CHECK(std::fabs(fox.pos.y - koni.pos.y) < 30.0);
    CHECK(fox.quota == doctest::Approx(0.0));   // in superficie
    CHECK(fox.velocita == doctest::Approx(koni.velocita));
    CHECK(var(m, 3) == 1);                // m688.rimorchiata
    CHECK(var(m, 4) == 0);                // incagliata non piu', e non in avaria
    // il Kilo in avaria: fermo, qualunque fosse la sua velocita'
    CHECK(m.unita().at(4).velocita == doctest::Approx(0.0));

    // il Koni in avaria a 200 s: rallenta e si ferma, il cavo va lento e il Foxtrot si ferma
    avanza(m, 150.0);
    CHECK(m.in_avaria(3));
    CHECK(koni.velocita == doctest::Approx(0.0));
    CHECK(fox.velocita == doctest::Approx(0.0));
    CHECK(quanti(m, TipoEvento::avaria_ia) == 1);   // il Kilo era in avaria dall'inizio: non e' un evento
    // molla il rimorchio a 400 s
    avanza(m, 150.0);
    CHECK(var(m, 5) == 1);
    CHECK(m.rimorchiatore_di(2) == -1);
    CHECK(quanti(m, TipoEvento::rimorchio, 0) == 1);
    CHECK(quanti(m, TipoEvento::rimorchio, 1) == 1);
    CHECK(m.errori().empty());
    fs::remove(f);
}

TEST_CASE("M6: lo stato sconosciuto e' un errore della missione; avaria(id, false) la ripara") {
    const auto f = scena("rotto");
    Missione m(f, 1, false);
    CHECK_FALSE(m.errori().empty());
    REQUIRE(m.in_avaria(4));
    CHECK(m.avaria(4, false));
    CHECK_FALSE(m.in_avaria(4));
    fs::remove(f);
}
