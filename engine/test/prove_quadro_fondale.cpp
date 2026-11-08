#include <doctest.h>

#include <cmath>
#include <functional>
#include <sstream>
#include <vector>

#include "nucleo/ecoscandaglio.hpp"
#include "nucleo/quadro_fondale.hpp"

using namespace m688;

// Il quadro del fondale: carta + misure (docs/24 §2.3)

namespace {

constexpr int N = 300;   // 60 x 60 km a celle da 200 m

// un fondale dalla funzione profondita'(x, y)
Fondale fondale(const std::function<double(double, double)>& prof) {
    Fondale base(N, N, 200.0, 1.0, -8000.0, std::vector<std::uint16_t>(static_cast<std::size_t>(N * N), 0));
    std::vector<std::uint16_t> v(static_cast<std::size_t>(N * N));
    for (int r = 0; r < N; ++r)
        for (int c = 0; c < N; ++c) {
            const Vec2 p = base.centro_cella(c, r);
            v[static_cast<std::size_t>(r * N + c)] = static_cast<std::uint16_t>(std::lround(8000.0 - prof(p.x, p.y)));
        }
    return Fondale(N, N, 200.0, 1.0, -8000.0, v);
}

// la carta d'epoca: tutto rilevato da una campagna con linee ogni 2 nm
Fondale carta(const std::function<double(double, double)>& prof) {
    Fondale f = fondale(prof);
    Campagna c;
    c.anno = 1975;
    c.ente = "prova";
    c.spaziatura_m = 3704.0;
    c.spaziatura_bacini_m = 3704.0;
    f.imposta_carta(1988, {c}, std::vector<std::uint8_t>(static_cast<std::size_t>(N * N), 1));
    return f;
}

ParametriEcoscandaglio senza_falsi() {
    ParametriEcoscandaglio p;
    p.falso_fondo = 0.0;
    p.falso_scia = 0.0;
    return p;
}

}  // namespace

TEST_CASE("quadro del fondale: il nastro lungo la rotta, la sigma che cala, il vincolo, il resto da carta") {
    // il fondo vero a 2000 m, la carta a 2200 (sbagliata del 10%)
    const Fondale vero = fondale([](double, double) { return 2000.0; });
    const Fondale c = carta([](double, double) { return 2200.0; });
    QuadroFondale q(c);
    Casuale caso(1);
    const auto p = senza_falsi();
    const std::uint64_t v0 = q.versione();
    for (int k = 0; k < 60; ++k) {   // un sondaggio al minuto a 10 kn verso nord
        const Vec2 pos{0.0, -10000.0 + 10.0 * NODO * 60.0 * k};
        q.aggiungi(sonda(vero, pos, pos, 150.0, 10.0, 60.0 * k, PotenzaEco::piena, false, 0.0, p, caso), 50.0);
    }
    CHECK(q.versione() > v0);
    const LetturaCella misurata = q.leggi(Vec2{0.0, -10000.0 + 10.0 * NODO * 60.0 * 30}, 3600.0);
    CHECK_FALSE(misurata.da_carta);
    CHECK(misurata.fonte == FonteFondo::ecoscandaglio);
    CHECK(misurata.carta_m == doctest::Approx(2200.0));
    CHECK(misurata.profondita_m == doctest::Approx(2000.0).epsilon(0.03));
    CHECK(misurata.sigma_m < 0.5 * misurata.sigma_carta_m);
    // a 2 km di lato: solo la carta
    const LetturaCella lato = q.leggi(Vec2{2000.0, 0.0}, 3600.0);
    CHECK(lato.da_carta);
    CHECK(lato.profondita_m == doctest::Approx(2200.0));
    // nell'impronta (~500 m di raggio) fuori dal centro: il vincolo «almeno»
    const LetturaCella bordo = q.leggi(Vec2{250.0, -10000.0 + 10.0 * NODO * 60.0 * 30}, 3600.0);
    CHECK(bordo.almeno_m > 1900.0);
    CHECK(q.celle_misurate().size() > 60);
    // il profilo di prua: misurato dove c'e' il nastro
    const auto prof = q.profilo({0.0, -10000.0}, 0.0, 15000.0, 500.0, 3600.0);
    REQUIRE(prof.size() == 31);
    CHECK(prof[4].misurato);
    CHECK(prof[4].stima_m == doctest::Approx(2000.0).epsilon(0.03));
    CHECK(prof[30].carta_m == doctest::Approx(2200.0));
    // le misure invecchiano: dopo un giorno la stima e' tornata quasi alla carta
    const LetturaCella vecchia = q.leggi(Vec2{0.0, -10000.0 + 10.0 * NODO * 60.0 * 30}, 3600.0 + 86400.0);
    CHECK(vecchia.sigma_m > 0.9 * vecchia.sigma_carta_m);
    CHECK(vecchia.profondita_m > misurata.profondita_m + 150.0);
}

TEST_CASE("quadro del fondale: il punto nave fa scorrere il mosaico della correzione giusta") {
    // un canyon nord-sud largo 2 km (3000 m) che la carta non ha (1500 m ovunque). Il battello va verso nord lungo il
    // canyon, ma l'inerziale deriva verso est di 1 m/s: crede di essere sempre piu' a destra
    const Fondale vero = fondale([](double x, double) { return std::fabs(x) < 1000.0 ? 3000.0 : 1500.0; });
    const Fondale c = carta([](double, double) { return 1500.0; });
    QuadroFondale q(c);
    Casuale caso(2);
    const auto p = senza_falsi();
    for (int k = 0; k <= 20; ++k) {
        const double t = 60.0 * k;
        const Vec2 pos{0.0, -6000.0 + 5.0 * t};
        const Vec2 stimata{pos.x + 1.0 * t, pos.y};
        q.aggiungi(sonda(vero, pos, stimata, 100.0, 10.0, t, PotenzaEco::piena, false, 0.0, p, caso), 10.0 + t);
    }
    // prima del punto nave l'ultimo sondaggio sta a 1200 m di lato: il canyon misurato e' fuori posto
    const Vec2 ultimo = q.sondaggi().back().pos;
    CHECK(ultimo.x == doctest::Approx(1200.0));
    CHECK(q.leggi(ultimo, 1200.0).profondita_m > 2000.0);
    q.punto_nave(1200.0, {-1200.0, 0.0});
    for (const Sondaggio& s : q.sondaggi()) CHECK(s.pos.x == doctest::Approx(s.pos_vera.x).epsilon(1e-9));
    CHECK(q.leggi(ultimo, 1200.0).da_carta);   // ripulito
    CHECK(q.leggi(Vec2{0.0, ultimo.y}, 1200.0).profondita_m > 2000.0);   // e il canyon misurato e' tornato nel canyon
}

TEST_CASE("quadro del fondale: gli avvisi solo dalle misure; lo scenario del San Francisco") {
    // un monte sottomarino ripido: il fondo a 2600 m, la cima a 100 m, i fianchi in 5 km; la carta non lo ha
    const auto monte = [](double x, double y) { const double d = std::hypot(x, y); return d >= 5000.0 ? 2600.0 : 100.0 + 2500.0 * d / 5000.0; };
    const Fondale vero = fondale(monte);
    const Fondale c = carta([](double, double) { return 2600.0; });
    // senza misure niente avvisi, anche col monte davanti
    QuadroFondale vuoto(c);
    CHECK(vuoto.avvisi({0.0, -3000.0}, 160.0, 0.0, 33.0, 0.0).empty());
    // a 33 kn e 200 m verso la cima da 14 km: sondaggi ogni 15 minuti o ogni minuto
    const auto corsa = [&](double periodo_min) {
        QuadroFondale q(c);
        Casuale caso(3);
        const auto p = senza_falsi();
        ProgrammaEcoscandaglio g;
        g.periodico(periodo_min);
        const double v = 33.0 * NODO;
        for (double t = 0.0; t < 2000.0; t += 1.0) {
            const Vec2 pos{0.0, -14000.0 + v * t};
            if (vero.profondita(pos) < 200.0 + 5.0) return -1.0;   // l'urto: nessun avviso prima
            if (g.passo(t)) q.aggiungi(sonda(vero, pos, pos, 200.0, 33.0, t, PotenzaEco::piena, false, 0.0, p, caso), 50.0);
            for (const AvvisoFondo& a : q.avvisi(pos, 200.0, 0.0, 33.0, t))
                if (a.tipo == AvvisoFondo::Tipo::fondo_in_salita) return distanza(pos, {0.0, 0.0});
        }
        return 0.0;
    };
    const double ogni_15 = corsa(15.0), ogni_1 = corsa(1.0);
    MESSAGE("fondo in salita: con un sondaggio ogni 15 min " << ogni_15 << ", ogni minuto a " << ogni_1 << " m dalla cima");
    CHECK(ogni_15 < 0.0);      // l'urto prima del secondo sondaggio
    CHECK(ogni_1 > 3000.0);    // l'avviso coi chilometri per accostare
}

TEST_CASE("quadro del fondale: il BQS-15 vede l'ostacolo e da' la velocita' sicura; i raggi liberi") {
    const Fondale c = carta([](double, double) { return 1000.0; });
    QuadroFondale q(c);
    // una spazzata: un eco a 1500 m a prua, piu' alto della chiglia (una parete); a dritta niente fino a 3000 m
    RitornoFondo parete;
    parete.t = 100.0;
    parete.origine = {0.0, 0.0};
    parete.quota_m = 200.0;
    parete.rilevamento = 5.0;
    parete.distanza_m = 1500.0;
    parete.punto = {1500.0 * std::sin(radianti(5.0)), 1500.0 * std::cos(radianti(5.0))};
    parete.profondita_m = 180.0;
    parete.ostacolo = true;
    q.aggiungi(parete, 50.0);
    RitornoFondo libero;
    libero.t = 100.0;
    libero.quota_m = 200.0;
    libero.rilevamento = 20.0;
    libero.elevazione = -5.0;
    libero.distanza_m = 3000.0;
    libero.eco = false;
    q.aggiungi(libero, 50.0);
    const auto av = q.avvisi({0.0, 0.0}, 200.0, 0.0, 20.0, 105.0);
    bool ostacolo = false, troppo_veloce = false;
    for (const AvvisoFondo& a : av) {
        if (a.tipo == AvvisoFondo::Tipo::ostacolo_a_prua) ostacolo = a.valore == doctest::Approx(1500.0).epsilon(0.01);
        if (a.tipo == AvvisoFondo::Tipo::velocita_eccessiva) troppo_veloce = a.valore < 20.0;
    }
    CHECK(ostacolo);
    CHECK(troppo_veloce);   // 1500 m in 180 s: ~16 kn
    CHECK(q.leggi(parete.punto, 105.0).ostacolo);
    CHECK(q.leggi(parete.punto, 105.0).fonte == FonteFondo::bqs15);
    // il raggio libero scende di 5 gradi: a 2 km il fondo sta almeno ~370 m (meno il 10%)
    const Vec2 a2km{2000.0 * std::cos(radianti(5.0)) * std::sin(radianti(20.0)), 2000.0 * std::cos(radianti(5.0)) * std::cos(radianti(20.0))};
    CHECK(q.leggi(a2km, 105.0).almeno_m > 300.0);
    CHECK(q.ritorni_recenti(105.0, 30.0).size() == 2);
    CHECK(q.ritorni_recenti(200.0, 30.0).empty());
}

TEST_CASE("quadro del fondale: salvataggio e ricarica uguali") {
    const Fondale vero = fondale([](double x, double) { return 1500.0 + 0.1 * x; });
    const Fondale c = carta([](double, double) { return 1500.0; });
    QuadroFondale q(c);
    Casuale caso(5);
    const ParametriEcoscandaglio p;
    for (int k = 0; k < 30; ++k) {
        const Vec2 pos{100.0 * k, 50.0 * k};
        q.aggiungi(sonda(vero, pos, {pos.x + 3.0 * k, pos.y}, 120.0, 8.0, 30.0 * k, PotenzaEco::piena, true, 0.004, p, caso), 30.0);
    }
    q.punto_nave(450.0, {-40.0, 0.0});
    std::stringstream buf;
    q.salva(buf);
    const QuadroFondale r = QuadroFondale::carica(buf, c);
    REQUIRE(r.sondaggi().size() == q.sondaggi().size());
    for (std::size_t i = 0; i < r.sondaggi().size(); ++i) {
        CHECK(r.sondaggi()[i].pos.x == q.sondaggi()[i].pos.x);
        CHECK(r.sondaggi()[i].fondo_m == q.sondaggi()[i].fondo_m);
        CHECK(r.sondaggi()[i].falso == q.sondaggi()[i].falso);
    }
    CHECK(r.celle_misurate().size() == q.celle_misurate().size());
    for (const auto& [col, riga] : q.celle_misurate()) {
        CHECK(r.leggi(col, riga, 900.0).profondita_m == doctest::Approx(q.leggi(col, riga, 900.0).profondita_m));
        CHECK(r.leggi(col, riga, 900.0).almeno_m == doctest::Approx(q.leggi(col, riga, 900.0).almeno_m));
    }
    std::stringstream rotto(buf.str().substr(0, 40));
    CHECK_THROWS(QuadroFondale::carica(rotto, c));
}
