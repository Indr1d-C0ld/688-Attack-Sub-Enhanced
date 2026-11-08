#include <doctest.h>

#include <cmath>
#include <vector>

#include "nucleo/ecoscandaglio.hpp"

using namespace m688;

// L'ecoscandaglio (quadro del fondale, docs/24 §2.1)

namespace {

// un fondale di 40 x 40 km a celle da 200 m, piatto a `profondita`; `picco`: una cella piu' alta a (x, y)
Fondale piatto(double profondita, std::vector<std::pair<Vec2, double>> picchi = {}) {
    const int n = 200;
    std::vector<std::uint16_t> v(static_cast<std::size_t>(n * n), static_cast<std::uint16_t>(8000.0 - profondita));
    Fondale f(n, n, 200.0, 1.0, -8000.0, v);
    std::vector<std::uint16_t> w = v;
    for (const auto& [p, prof] : picchi) {
        int c = 0, r = 0;
        if (f.cella(p, c, r)) w[static_cast<std::size_t>(r * n + c)] = static_cast<std::uint16_t>(8000.0 - prof);
    }
    return Fondale(n, n, 200.0, 1.0, -8000.0, w);
}

ParametriEcoscandaglio senza_falsi() {
    ParametriEcoscandaglio p;
    p.falso_fondo = 0.0;
    p.falso_scia = 0.0;
    return p;
}

}  // namespace

TEST_CASE("ecoscandaglio: sul fondo piatto misura entro l'1%, la sigma lo dice") {
    const Fondale f = piatto(1500.0);
    Casuale caso(1);
    const auto p = senza_falsi();
    int dentro = 0;
    for (int k = 0; k < 200; ++k) {
        const double scala = scala_ecoscandaglio(p, caso);   // un apparato diverso a ogni ping: la peggiore delle ipotesi
        const Sondaggio s = sonda(f, {100.0, 100.0}, {300.0, 120.0}, 100.0, 5.0, k, PotenzaEco::piena, false, scala, p, caso);
        REQUIRE(s.eco);
        CHECK_FALSE(s.falso);
        CHECK(s.fondo_vero_m == doctest::Approx(1500.0).epsilon(0.001));
        dentro += std::fabs(s.fondo_m - 1500.0) <= 2.0 * s.sigma_m;
        CHECK(s.pos.x == 300.0);   // dove il battello crede di essere
        CHECK(s.impronta_m == doctest::Approx(1400.0 * std::tan(radianti(15.0))).epsilon(0.05));
    }
    CHECK(dentro >= 180);   // ~95% entro due sigma
}

TEST_CASE("ecoscandaglio: un pinnacolo nell'impronta si vede prima di esserci sopra, arrotondato") {
    // il fondo a 2000 m, un pinnacolo a 1200 m a 300 m di lato: il battello a 100 m ha l'impronta di ~500 m di raggio
    const Fondale f = piatto(2000.0, {{{300.0, 0.0}, 1200.0}});
    Casuale caso(2);
    const auto p = senza_falsi();
    const Sondaggio lontano = sonda(f, {-2000.0, 0.0}, {-2000.0, 0.0}, 100.0, 5.0, 0.0, PotenzaEco::piena, false, 0.0, p, caso);
    const Sondaggio vicino = sonda(f, {0.0, 0.0}, {0.0, 0.0}, 100.0, 5.0, 0.0, PotenzaEco::piena, false, 0.0, p, caso);
    const Sondaggio sopra = sonda(f, {300.0, 0.0}, {300.0, 0.0}, 100.0, 5.0, 0.0, PotenzaEco::piena, false, 0.0, p, caso);
    MESSAGE("lontano " << lontano.fondo_m << " m, vicino " << vicino.fondo_m << " m (vero sotto " << vicino.fondo_vero_m
                       << "), sopra " << sopra.fondo_m << " m");
    CHECK(lontano.fondo_m == doctest::Approx(2000.0).epsilon(0.01));
    CHECK(vicino.fondo_m < vicino.fondo_vero_m - 300.0);   // il pinnacolo di lato accorcia l'eco: lo anticipa
    CHECK(vicino.fondo_m > 1200.0);                         // ma obliquo: arrotondato, non la sua cima
    CHECK(sopra.fondo_m < 1400.0);
}

TEST_CASE("ecoscandaglio: i falsi fondi, il 2% e di piu' sulla scia; lo strato diffondente di giorno") {
    const Fondale f = piatto(3000.0);
    Casuale caso(3);
    const ParametriEcoscandaglio p;
    int falsi = 0, dsl = 0, scia = 0;
    for (int k = 0; k < 5000; ++k) {
        const Sondaggio s = sonda(f, {0.0, 0.0}, {0.0, 0.0}, 150.0, 8.0, k, PotenzaEco::piena, true, 0.0, p, caso);
        falsi += s.falso;
        dsl += s.falso && s.fondo_m >= p.dsl_min_m && s.fondo_m <= p.dsl_max_m;
        const Sondaggio v = sonda(f, {0.0, 0.0}, {0.0, 0.0}, 150.0, 28.0, k, PotenzaEco::piena, true, 0.0, p, caso);
        scia += v.falso && v.sotto_chiglia_m < 60.0;
    }
    MESSAGE("falsi fondi su 5000: " << falsi << " (strato diffondente " << dsl << "), sulla scia a 28 kn " << scia);
    CHECK(falsi == doctest::Approx(100).epsilon(0.3));
    CHECK(dsl == falsi);   // di giorno, sopra uno strato che sta fra il battello e il fondo
    CHECK(scia == doctest::Approx(500).epsilon(0.2));
}

TEST_CASE("ecoscandaglio: le portate delle due potenze e l'impulso per l'intercettazione") {
    const Fondale f = piatto(4000.0);
    Casuale caso(4);
    const auto p = senza_falsi();
    const Sondaggio piena = sonda(f, {0.0, 0.0}, {0.0, 0.0}, 200.0, 5.0, 10.0, PotenzaEco::piena, false, 0.0, p, caso);
    const Sondaggio bassa = sonda(f, {0.0, 0.0}, {0.0, 0.0}, 200.0, 5.0, 10.0, PotenzaEco::bassa, false, 0.0, p, caso);
    CHECK(piena.eco);
    CHECK_FALSE(bassa.eco);   // 3800 m sotto la chiglia: oltre i 2000 della bassa potenza
    const Fondale basso = piatto(1500.0);
    CHECK(sonda(basso, {0.0, 0.0}, {0.0, 0.0}, 200.0, 5.0, 10.0, PotenzaEco::bassa, false, 0.0, p, caso).eco);
    const EmissioneAttiva e1 = emissione(piena, 0, 90.0, 5.0, p), e2 = emissione(bassa, 0, 90.0, 5.0, p);
    CHECK(e1.chiave == "fathometer");
    CHECK(e1.f_hz == 12000.0);
    CHECK(e1.sl_db - e2.sl_db == doctest::Approx(20.0));
    CHECK(e1.t == 10.0);
    // fuori dalla griglia niente fondo: niente eco
    CHECK_FALSE(sonda(f, {90000.0, 0.0}, {90000.0, 0.0}, 200.0, 5.0, 0.0, PotenzaEco::piena, false, 0.0, p, caso).eco);
}

TEST_CASE("ecoscandaglio: ripetibile dal seme; il programma a comando e periodico") {
    const Fondale f = piatto(2500.0, {{{400.0, 200.0}, 1800.0}});
    const auto prova = [&](std::uint64_t seme) {
        Casuale caso(seme);
        const ParametriEcoscandaglio p;
        const double scala = scala_ecoscandaglio(p, caso);
        std::vector<double> v;
        for (int k = 0; k < 50; ++k) v.push_back(sonda(f, {10.0 * k, 0.0}, {10.0 * k, 0.0}, 100.0, 6.0, k, PotenzaEco::piena, true, scala, p, caso).fondo_m);
        return v;
    };
    CHECK(prova(9) == prova(9));
    CHECK(prova(9) != prova(10));
    ProgrammaEcoscandaglio g;
    CHECK_FALSE(g.passo(0.0));
    g.singolo();
    CHECK(g.passo(1.0));
    CHECK_FALSE(g.passo(2.0));
    g.periodico(15.0);   // il San Francisco: un sondaggio ogni 15 minuti
    int n = 0;
    for (double t = 10.0; t < 10.0 + 3600.0; t += 1.0) n += g.passo(t);
    CHECK(n == 4);
    g.periodico(0.0);
    CHECK_FALSE(g.passo(5000.0));
}

TEST_CASE("ecoscandaglio: i dati di acustica.toml, e l'emissione nel catalogo dell'intercettazione") {
    const ParametriEcoscandaglio p = carica_ecoscandaglio(M688_DATI "/acustica.toml");
    const ParametriEcoscandaglio d;
    CHECK(p.id == "fathometer");
    CHECK(p.cono_deg == d.cono_deg);
    CHECK(p.portata_piena_m == d.portata_piena_m);
    CHECK(p.sl_bassa_db == d.sl_bassa_db);
    CHECK(p.falso_fondo == d.falso_fondo);
    CHECK_THROWS(carica_ecoscandaglio(M688_DATI "/acustica.toml", "nessuno"));
    const CatalogoIntercettazione c = carica_intercettazione(M688_DATI "/acustica.toml");
    REQUIRE(c.emissione("fathometer") != nullptr);
    CHECK(c.emissione("fathometer")->f_hz == doctest::Approx(12000.0));
}
