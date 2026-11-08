#include <doctest.h>

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <string>

#include "nucleo/portata_luci.hpp"

#ifndef M688_DATI
#define M688_DATI "../data/motore"
#endif

using namespace m688;

// La portata delle luci di navigazione con la foschia (docs/27): l'Annesso I, Allard, l'orizzonte del fanale, i settori

namespace {

constexpr double MIGLIO = 1852.0;
const double VISIBILITA_ANNESSO = 3.912 * MIGLIO / -std::log(luci::K_ANNESSO);   // ~32 km: la K = 0,8 per miglio

luci::Fanale fanale(const char* ruolo, luci::Colore c, double da, double a, double z, double miglia) {
    luci::Fanale f;
    f.ruolo = ruolo;
    f.colore = c;
    f.settore_da = da;
    f.settore_a = a;
    f.pos_m = {0.0, 0.0, z};
    f.portata_m = miglia * MIGLIO;
    f.stati = 1u << static_cast<int>(luci::Stato::moto);
    return f;
}

// un mercantile di 150 m in moto: testa d'albero, laterali, coronamento
luci::Configurazione mercantile() {
    luci::Configurazione c;
    c.classe = "prova";
    c.tipo = "mercantile";
    c.lunghezza_m = 150.0;
    c.fanali = {fanale("albero", luci::Colore::bianco, -112.5, 112.5, 25.0, 6.0),
                fanale("laterale_sinistro", luci::Colore::rosso, -112.5, 0.0, 15.0, 3.0),
                fanale("laterale_dritto", luci::Colore::verde, 0.0, 112.5, 15.0, 3.0),
                fanale("coronamento", luci::Colore::bianco, 112.5, -112.5, 8.0, 3.0)};
    return c;
}

Osservatore vedetta() {
    Osservatore o;
    o.altezza_occhio_m = 20.0;
    o.allertato = true;
    return o;
}

CondizioniVista notte(double visibilita_m) {
    CondizioniVista c;
    c.luce = 0.0;
    c.visibilita_m = visibilita_m;
    return c;
}

}  // namespace

TEST_CASE("portata delle luci: l'intensita' dell'Annesso I e la portata nominale con la sua visibilita'") {
    // la tavola dell'Annesso I §8: 1, 2, 3, 6 miglia (3,43e6 e' 1852^2 arrotondato: 3e-5 di scarto)
    CHECK(luci::intensita_cd(1.0 * MIGLIO) == doctest::Approx(0.86).epsilon(0.02));
    CHECK(luci::intensita_cd(2.0 * MIGLIO) == doctest::Approx(4.3).epsilon(0.02));
    CHECK(luci::intensita_cd(3.0 * MIGLIO) == doctest::Approx(12.0).epsilon(0.03));
    CHECK(luci::intensita_cd(6.0 * MIGLIO) == doctest::Approx(94.0).epsilon(0.01));
    CHECK(luci::intensita_cd(0.0) == 0.0);
    for (const double m : {1.0, 3.0, 6.0, 10.0})
        CHECK(luci::portata_nominale_m(luci::intensita_cd(m * MIGLIO)) == doctest::Approx(m * MIGLIO).epsilon(1e-4));
    // la stessa con Allard, la visibilita' dell'Annesso e la soglia di notte; e il rapporto della vista vale 1 li'
    CHECK(luci::portata_luminosa_m(94.0, VISIBILITA_ANNESSO, luci::SOGLIA_NOTTE_LUX) == doctest::Approx(6.0 * MIGLIO).epsilon(0.01));
    CHECK(luci::rapporto_soglia(6.0 * MIGLIO, 6.0 * MIGLIO, VISIBILITA_ANNESSO, 0.0) == doctest::Approx(1.0).epsilon(1e-4));
    CHECK(luci::rapporto_soglia(6.0 * MIGLIO, 3000.0, VISIBILITA_ANNESSO, 0.0) > 1.0);
}

TEST_CASE("portata delle luci: la foschia, il giorno, l'allerta, il binocolo e l'orizzonte") {
    const double i6 = luci::intensita_cd(6.0 * MIGLIO);
    const double e = luci::SOGLIA_NOTTE_LUX;
    const double sereno = luci::portata_luminosa_m(i6, 30000.0, e);
    const double foschia = luci::portata_luminosa_m(i6, 5000.0, e);
    const double nebbia = luci::portata_luminosa_m(i6, 2000.0, e);
    const double fitta = luci::portata_luminosa_m(i6, 300.0, e);
    MESSAGE("6 miglia: ", sereno, " m col sereno, ", foschia, " a 5 km, ", nebbia, " a 2 km, ", fitta, " a 300 m");
    CHECK(sereno > foschia);
    CHECK(foschia > nebbia);
    CHECK(nebbia > fitta);
    // un fanale forte si vede un po' oltre la visibilita' meteorologica, mai di molto nella nebbia fitta
    CHECK(nebbia > 2000.0);
    CHECK(nebbia < 3500.0);
    CHECK(fitta > 300.0);
    CHECK(fitta < 700.0);
    // il giorno spegne i fanali; il crepuscolo poco; il chiaro di luna niente (come la vista)
    CHECK(luci::soglia_lux(0.1, true) == doctest::Approx(e));
    CHECK(luci::portata_luminosa_m(i6, 30000.0, luci::soglia_lux(1.0, true)) < sereno / 10.0);
    CHECK(luci::portata_luminosa_m(i6, 30000.0, luci::soglia_lux(0.3, true)) < sereno);
    // la vedetta non allertata ha bisogno di piu' luce, il binocolo e l'intensificatore di meno; il termico non aiuta
    CHECK(luci::soglia_lux(0.0, false) == doctest::Approx(3.0 * e));
    CHECK(luci::soglia_lux(0.0, true, luci::GUADAGNO_BINOCOLO) < e);
    CHECK(luci::guadagno_sensore(ModoVista::intensificatore) > luci::guadagno_sensore(ModoVista::tv));
    CHECK(luci::guadagno_sensore(ModoVista::termico) == 1.0);
    // l'orizzonte del fanale: una luce forte e bassa, vista dal periscopio a pelo d'acqua, sparisce sotto l'orizzonte
    const luci::Fanale basso = fanale("prova", luci::Colore::bianco, -180.0, 180.0, 4.0, 20.0);
    Osservatore periscopio;
    periscopio.tipo = TipoOsservatore::periscopio;
    periscopio.altezza_occhio_m = 1.0;
    periscopio.allertato = true;
    const double r = luci::portata_fanale_m(basso, periscopio, notte(50000.0));
    CHECK(r == doctest::Approx(luci::orizzonte_luce_m(1.0, 4.0)));
    CHECK(r < 12000.0);
}

TEST_CASE("portata delle luci: i fanali nel settore, il migliore, lo stato, la probabilita'") {
    const luci::Configurazione m = mercantile();
    const Osservatore o = vedetta();
    const CondizioniVista c = notte(30000.0);
    // di prora la testa d'albero (6 miglia), di poppa il coronamento (3 miglia): la nave che viene incontro si vede prima
    const luci::VistaLuci prora = luci::vista_luci(m, luci::Stato::moto, 0.0, o, c);
    const luci::VistaLuci poppa = luci::vista_luci(m, luci::Stato::moto, 180.0, o, c);
    REQUIRE(prora.migliore);
    REQUIRE(poppa.migliore);
    CHECK(prora.migliore->ruolo == "albero");
    CHECK(poppa.migliore->ruolo == "coronamento");
    CHECK(prora.portata_m > 1.5 * poppa.portata_m);
    // ben oltre i 3 km fissi della vedetta notturna di oggi; col binocolo ancora di piu'
    MESSAGE("mercantile di notte: ", prora.portata_m, " m di prora, ", poppa.portata_m, " di poppa");
    CHECK(prora.portata_m > 8000.0);
    CHECK(luci::vista_luci(m, luci::Stato::moto, 0.0, o, c, luci::GUADAGNO_BINOCOLO).portata_m > prora.portata_m);
    // spente: niente; alla fonda questo mercantile non ha fanali di fonda
    CHECK(luci::vista_luci(m, luci::Stato::spente, 0.0, o, c).portata_m == 0.0);
    CHECK(luci::vista_luci(m, luci::Stato::fonda, 0.0, o, c).migliore == nullptr);
    // la probabilita': sicura vicino, nulla oltre la portata, cala con la distanza
    const double vicino = luci::probabilita_vista_luci(m, luci::Stato::moto, 0.0, o, c, 1000.0, 10.0);
    const double meta = luci::probabilita_vista_luci(m, luci::Stato::moto, 0.0, o, c, 0.8 * prora.portata_m, 10.0);
    const double lontano = luci::probabilita_vista_luci(m, luci::Stato::moto, 0.0, o, c, 0.95 * prora.portata_m, 10.0);
    CHECK(vicino > 0.8);
    CHECK(meta == doctest::Approx(1.0 - std::exp(-0.5 * luci::TASSO_LUCI_S * 10.0)).epsilon(1e-6));
    CHECK(lontano < meta);
    CHECK(luci::probabilita_vista_luci(m, luci::Stato::moto, 0.0, o, c, 1.01 * prora.portata_m, 10.0) == 0.0);
    CHECK(luci::probabilita_vista_luci(m, luci::Stato::moto, 0.0, o, c, 1000.0, 0.0) == 0.0);
}

TEST_CASE("portata delle luci: le configurazioni di luci.toml, il faro ambra del 688" *
          doctest::skip(!std::filesystem::exists(std::string(M688_DATI) + "/luci.toml"))) {
    const auto tutte = luci::carica(std::string(M688_DATI) + "/luci.toml");
    const auto la = tutte.find("los_angeles");
    REQUIRE(la != tutte.end());
    const Osservatore o = vedetta();
    const CondizioniVista c = notte(30000.0);
    // il 688 in superficie in pace: fanali e faro; da poppa il faro (tutt'intorno) arriva almeno quanto il coronamento
    const luci::VistaLuci v = luci::vista_luci(la->second, luci::Stato::moto, 180.0, o, c);
    REQUIRE(v.migliore);
    CHECK(v.portata_m > 3000.0);
    // il faro da solo: meta' del tasso d'una luce fissa della stessa portata
    luci::Configurazione solo_faro = la->second;
    std::erase_if(solo_faro.fanali, [](const luci::Fanale& f) { return !f.lampi_7077; });
    REQUIRE(!solo_faro.fanali.empty());
    luci::Configurazione fisso = solo_faro;
    for (auto& f : fisso.fanali) f.lampi_7077 = false;
    const double d = 1000.0;
    const double pl = luci::probabilita_vista_luci(solo_faro, luci::Stato::moto, 90.0, o, c, d, 1.0);
    const double pf = luci::probabilita_vista_luci(fisso, luci::Stato::moto, 90.0, o, c, d, 1.0);
    CHECK(pl > 0.0);
    CHECK(pl < pf);
    // ogni classe con le luci accese in moto si vede di notte da qualche lato oltre i 3 km d'oggi
    int classi = 0, oltre = 0;
    for (const auto& [id, conf] : tutte) {
        double migliore = 0.0;
        for (double rel = -180.0; rel < 180.0; rel += 15.0)
            migliore = std::max(migliore, luci::vista_luci(conf, luci::Stato::moto, rel, o, c).portata_m);
        if (migliore <= 0.0) continue;   // nessun fanale in moto (non dovrebbe capitare)
        ++classi;
        oltre += migliore > 3000.0;
    }
    MESSAGE(oltre, " classi su ", classi, " oltre i 3 km di notte");
    CHECK(classi > 20);
    CHECK(oltre == classi);
}

TEST_CASE("portata delle luci: il guadagno ottico, una legge per la vedetta e per il periscopio") {
    // il 7x50 di notte: la pupilla d'uscita di 7,1 mm spreca un po'; il periscopio a 6x (obiettivo 30, trasmissione 0,3)
    CHECK(luci::guadagno_ottico(50.0, 0.85, 7.0) == doctest::Approx(0.85 * 49.0 * 49.0 / 49.0).epsilon(1e-9));
    CHECK(luci::guadagno_ottico(30.0, 0.3, 6.0) == doctest::Approx(0.3 * (30.0 / 7.0) * (30.0 / 7.0)));
    CHECK(luci::guadagno_ottico(30.0, 0.3, 1.5) == doctest::Approx(0.3 * 1.5 * 1.5));   // l'apertura utile: 1,5 x 7 mm
    CHECK(luci::guadagno_ottico(30.0, 0.3, 6.0) > 5.0);
    CHECK(luci::guadagno_ottico(30.0, 0.3, 1.5) < 1.0);   // a basso ingrandimento peggio dell'occhio nudo
    CHECK(luci::GUADAGNO_BINOCOLO == doctest::Approx(10.4).epsilon(0.02));   // il ponte che si muove: un quarto dei 42
    // nella foschia di 15 km un fanale da 3 miglia: a 6x si vede piu' lontano che a occhio nudo (I15 del kit)
    const double tre = 3.0 * 1852.0, nudo_4400 = luci::rapporto_soglia(tre, 4400.0, 15000.0, 0.0);
    const double g6 = luci::guadagno_ottico(30.0, 0.3, 6.0);
    CHECK(luci::rapporto_soglia(tre, 4400.0, 15000.0, 0.0, g6) == doctest::Approx(g6 * nudo_4400).epsilon(1e-9));
    double lontano_nudo = 0.0, lontano_6x = 0.0;
    for (double d = 100.0; d < 30000.0; d += 50.0) {
        if (luci::rapporto_soglia(tre, d, 15000.0, 0.0) >= 1.0) lontano_nudo = d;
        if (luci::rapporto_soglia(tre, d, 15000.0, 0.0, g6) >= 1.0) lontano_6x = d;
    }
    MESSAGE("fanale da 3 miglia con 15 km di foschia: occhio nudo ", lontano_nudo, " m, periscopio a 6x ", lontano_6x, " m");
    CHECK(lontano_6x > lontano_nudo + 1000.0);
}
