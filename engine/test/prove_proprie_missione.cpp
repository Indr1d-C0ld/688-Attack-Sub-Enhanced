#include <doctest.h>

#include <filesystem>
#include <map>
#include <memory>
#include <string>

#include "missioni/danni_missione.hpp"
#include "missioni/missione.hpp"
#include "nucleo/classi.hpp"

using namespace m688;
namespace fs = std::filesystem;

// Le righe del proprio battello nel sonarista della missione (blocco 2 del sonar, F): la maschera vale a ogni secondo,
// non solo in quello in cui si rifanno le righe del fascio. Prima, nei 4 secondi su 5 fra un ricalcolo e l'altro, il
// sonarista riceveva le righe del 688 senza le proprie e in PROVA_M1_ANTINAVE classificava «los_angeles» lo Slava, il
// Krivak, il cargo e il mercantile

namespace {

// M1 sta in prove/ (prima in prove/missili/): si cerca nei due posti
std::string percorso_m1() {
    for (const char* p : {M688_MISSIONI "/prove/PROVA_M1_ANTINAVE.lua", M688_MISSIONI "/prove/missili/PROVA_M1_ANTINAVE.lua"})
        if (fs::exists(p)) return p;
    return {};
}
bool c_e_m1() { return !percorso_m1().empty(); }

// le classificazioni «los_angeles» di contatti che non sono un los_angeles, in 300 s di M1
int los_angeles_falsi(Esperienza e) {
    const auto classi = carica_classi(M688_DATI "/classi.toml");
    Missione m(percorso_m1(), 1);
    m.imposta_sonarista(e);
    DanniMissione dm(classi, M688_DATI, 1, RegolaDanni::simulazione);
    for (int k = 0; k < 300 * 9; ++k) {
        m.avanza(1.0 / 9.0);
        dm.avanza(m, 1.0 / 9.0);
    }
    REQUIRE(m.errori().empty());
    int n = 0;
    for (const auto& a : m.avvisi_sonarista())
        if (a.codice == 411 && a.classe == "los_angeles") {
            const auto it = m.unita().find(a.sorgente);
            n += it == m.unita().end() || it->second.classe.rfind("los_angeles", 0) != 0;
        }
    return n;
}

}  // namespace

TEST_CASE("righe proprie nella missione: in M1 il sonarista medio e l'esperto non scambiano le navi per un 688" * doctest::skip(!c_e_m1())) {
    CHECK(los_angeles_falsi(Esperienza::medio) == 0);
    CHECK(los_angeles_falsi(Esperienza::esperto) == 0);
}
