#include <doctest.h>

#include <cmath>
#include <cstdint>
#include <filesystem>
#include <limits>
#include <map>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "nucleo/casuale.hpp"
#include "nucleo/fondale.hpp"
#include "nucleo/geometria.hpp"
#include "nucleo/mondo.hpp"
#include "nucleo/orologio.hpp"

using namespace m688;
using doctest::Approx;

TEST_CASE("angoli normalizzati") {
    CHECK(normalizza_360(370.0) == Approx(10.0));
    CHECK(normalizza_360(-10.0) == Approx(350.0));
    CHECK(normalizza_360(360.0) == Approx(0.0));
    CHECK(normalizza_180(190.0) == Approx(-170.0));
    CHECK(normalizza_180(-180.0) == Approx(180.0));
}

TEST_CASE("rilevamenti nautici: 0 nord, 90 est") {
    const Vec2 o{0, 0};
    CHECK(rilevamento(o, {0, 100}) == Approx(0.0));
    CHECK(rilevamento(o, {100, 0}) == Approx(90.0));
    CHECK(rilevamento(o, {0, -100}) == Approx(180.0));
    CHECK(rilevamento(o, {-100, 0}) == Approx(270.0));
    CHECK(distanza(o, {300, 400}) == Approx(500.0));
}

TEST_CASE("angolo sulla prua e lato") {
    // bersaglio a nord dell'osservatore, rotta 270 (verso ovest): l'osservatore e' a sud, cioe' sulla sinistra
    auto a = angolo_sulla_prua({0, 1000}, 270.0, {0, 0});
    CHECK(a.gradi == Approx(90.0));
    CHECK_FALSE(a.dritta);
    // stessa posizione, rotta 90 (verso est): l'osservatore e' sulla dritta
    a = angolo_sulla_prua({0, 1000}, 90.0, {0, 0});
    CHECK(a.gradi == Approx(90.0));
    CHECK(a.dritta);
    // bersaglio che viene incontro (rotta 180): prua verso l'osservatore
    CHECK(angolo_sulla_prua({0, 1000}, 180.0, {0, 0}).gradi == Approx(0.0));
    CHECK(angolo_sulla_prua({0, 1000}, 0.0, {0, 0}).gradi == Approx(180.0));
}

TEST_CASE("orizzonte con rifrazione standard") {
    // occhio a 1,5 m: circa 4,7 km (2,5 miglia) con k = 1/7
    CHECK(distanza_orizzonte(1.5) == Approx(4724.0).epsilon(0.01));
    CHECK(depressione_orizzonte(1.5) * distanza_orizzonte(1.5) == Approx(3.0));   // 2h
}

TEST_CASE("orologio a passo fisso") {
    Orologio o(0.1);
    CHECK(o.avanza(0.25) == 2);
    CHECK(o.avanza(0.05) == 1);          // 0,05 accumulati + 0,05 = un passo
    CHECK(o.avanza(0.1, 8.0) == 8);      // compressione del tempo
    CHECK(o.tempo() == Approx(1.1));
    Orologio lento(0.1, 5);
    CHECK(lento.avanza(0.25, 64.0) == 5);   // limite di passi per fotogramma
}

TEST_CASE("generatore casuale riproducibile") {
    Casuale a(42), b(42), c(43);
    for (int i = 0; i < 100; ++i) {
        const auto x = a.prossimo();
        CHECK(x == b.prossimo());
        if (i == 0) CHECK(x != c.prossimo());
    }
    Casuale u(7);
    for (int i = 0; i < 1000; ++i) {
        const double v = u.uniforme();
        CHECK((v >= 0.0 && v < 1.0));
    }
}

TEST_CASE("cinematica: accostata dal lato piu' breve, velocita' limitata") {
    Mondo m;
    Nave n;
    n.rotta = 350.0; n.rotta_ordinata = 20.0;
    n.velocita = n.velocita_ordinata = 10.0 * NODO;
    n.rateo_accostata = 1.0;
    m.aggiungi(n);
    for (int i = 0; i < 10 * 30; ++i) m.passo(1.0 / 30.0);   // 10 s
    CHECK(m.navi()[0].rotta == Approx(0.0).epsilon(1e-6));    // 350 + 10 gradi in 10 s, passando per il nord
    for (int i = 0; i < 30 * 30; ++i) m.passo(1.0 / 30.0);
    CHECK(m.navi()[0].rotta == Approx(20.0));
    CHECK(m.tempo() == Approx(40.0));
}

TEST_CASE("simulazione deterministica") {
    auto simula = [] {
        Mondo m;
        for (int i = 0; i < 5; ++i) {
            Nave n;
            n.id = i; n.rotta = 30.0 * i; n.rotta_ordinata = 200.0 - 15.0 * i;
            n.velocita_ordinata = (8.0 + i) * NODO;
            m.aggiungi(n);
        }
        for (int i = 0; i < 3000; ++i) m.passo(1.0 / 30.0);
        return m.impronta();
    };
    CHECK(simula() == simula());
}

TEST_CASE("fondale: griglia, interpolazione e bordi") {
    // 4 x 2 celle da 100 m (riga 0 = nord): quote -100, -50, 0, +20 da ovest a est, uguali nelle due righe
    auto v = [](double z) { return static_cast<std::uint16_t>((z + 11000.0) / 0.25); };
    Fondale f(4, 2, 100.0, 0.25, -11000.0, {v(-100), v(-50), v(0), v(20), v(-100), v(-50), v(0), v(20)});
    CHECK(f.dentro({0, 0}));
    CHECK_FALSE(f.dentro({201, 0}));
    CHECK(f.quota({-150, 50}) == Approx(-100.0));   // centro della prima cella
    CHECK(f.quota({-100, 0}) == Approx(-75.0));     // a meta' fra i centri delle prime due
    CHECK(f.profondita({50, -50}) == Approx(0.0));
    CHECK(f.quota({190, 0}) == Approx(20.0));       // oltre l'ultimo centro: il valore del bordo
    CHECK(f.profondita({0, 500}) == std::numeric_limits<double>::infinity());   // fuori: acqua senza fondo
}

TEST_CASE("fondali esportati dei teatri" * doctest::skip(!std::filesystem::exists(M688_DATI "/teatri/faroe.fdl"))) {
    const auto f = Fondale::carica(M688_DATI "/teatri/faroe.fdl");
    CHECK(f.larghezza() == 1571);
    CHECK(f.cella_m() == Approx(200.0));
    // centro della proiezione 62 N 7 O; Streymoy (62,15 N 7,05 O) e' alta piu' di 300 m, a 61 N 7 O (a sud di
    // Suðuroy) il fondo e' sotto i 200 m (coordinate calcolate con pyproj sulla proiezione del teatro)
    CHECK(f.quota({-2607.0, 16718.0}) > 300.0);
    CHECK(f.profondita({0.0, -111437.0}) > 200.0);
}

TEST_CASE("fondale: georeferenziazione azimutale equidistante sull'ellissoide WGS84") {
    Fondale f(2, 2, 100.0, 1.0, -100.0, std::vector<std::uint16_t>(4, 0));
    CHECK_FALSE(f.georeferenziato());
    CHECK_THROWS_AS(f.geografiche({0, 0}), std::logic_error);
    // punti di riferimento calcolati con pyproj (+proj=aeqd +datum=WGS84) sui centri dei teatri
    struct Rif { double lat0, lon0, lat, lon, x, y; };
    const Rif rif[] = {
        {62.0, -7.0, 62.15, -7.05, -2607.025, 16718.024},
        {62.0, -7.0, 63.5, -10.0, -149375.695, 170656.168},
        {62.0, -7.0, 60.5, -4.0, 164828.468, -163356.557},
        {44.45, 146.65, 43.3, 144.8, -150112.321, -126090.587},
        {44.45, 146.65, 45.6, 148.5, 144333.344, 129445.159},
        {35.9, -5.5, 35.0, -7.5, -182571.339, -97998.829},
    };
    for (const auto& r : rif) {
        CAPTURE(r.lat);
        f.imposta_centro({r.lat0, r.lon0});
        REQUIRE(f.georeferenziato());
        const Vec2 p = f.proiettata({r.lat, r.lon});
        CHECK(std::fabs(p.x - r.x) < 0.01);
        CHECK(std::fabs(p.y - r.y) < 0.01);
        const auto g = f.geografiche({r.x, r.y});
        CHECK(std::fabs(g.lat - r.lat) < 1e-7);   // ~1 cm
        CHECK(std::fabs(g.lon - r.lon) < 1e-7);
        const auto c = f.geografiche({0, 0});
        CHECK(c.lat == r.lat0);
        CHECK(c.lon == r.lon0);
    }
}

TEST_CASE("carte d'epoca dei teatri: stessa griglia, maschera del rilievo e qualita' per parte" *
          doctest::skip(!std::filesystem::exists(M688_DATI "/teatri/faroe_carta_nato.fdl"))) {
    struct Statistiche { double non_rilevato = 0, errore = 0; };
    std::map<std::string, Statistiche> st;
    const std::pair<const char*, Geografiche> teatri[] = {
        {"baltico", {55.15, 13.35}}, {"barents", {70.15, 35.0}}, {"celtica", {48.25, -9.75}}, {"faroe", {62.0, -7.0}},
        {"gibilterra", {35.9, -5.5}}, {"kunashir", {44.45, 146.65}}, {"otranto", {41.0, 18.75}}};
    for (const auto& [t, centro] : teatri) {
        CAPTURE(t);
        const auto vero = Fondale::carica(std::string(M688_DATI) + "/teatri/" + t + ".fdl");
        REQUIRE(vero.georeferenziato());   // anche il fondale vero porta il centro (reticolo con il «fondale vero»)
        CHECK(vero.centro().lat == centro.lat);
        CHECK(vero.centro().lon == centro.lon);
        CHECK_FALSE(vero.carta_epoca());
        CHECK(vero.campagna({0, 0}) == -1);
        for (const Parte parte : {Parte::nato, Parte::urss}) {
            CAPTURE(nome_parte(parte));
            const auto carta = Fondale::carica(percorso_carta(M688_DATI, t, parte));
            REQUIRE(carta.carta_epoca());
            REQUIRE(carta.larghezza() == vero.larghezza());
            REQUIRE(carta.altezza() == vero.altezza());
            CHECK(carta.cella_m() == vero.cella_m());
            CHECK(carta.centro().lat == centro.lat);
            CHECK(carta.edizione() == (parte == Parte::nato ? 1988 : 1987));
            CHECK_FALSE(carta.campagne().empty());
            CHECK(carta.annotazioni().size() <= 60);
            for (const auto& c : carta.campagne()) {
                CHECK(c.anno >= 1930);
                CHECK(c.anno <= 1988);
                CHECK_FALSE(c.ente.empty());
                CHECK(c.spaziatura_m <= c.spaziatura_bacini_m);
                CHECK(c.spostamento_m <= 1.0 * MIGLIO + 1.0);
            }
            for (const auto& a : carta.annotazioni()) CHECK(carta.dentro(a.pos));
            // terra vera a passi di 10 m, il mare non diventa terra; maschera solo in mare
            long mare = 0, non_ril = 0, terra_sbagliata = 0, mare_in_terra = 0, campagne_fuori = 0, maschera_in_terra = 0;
            double errore = 0;
            for (int r = 0; r < vero.altezza(); ++r)
                for (int c = 0; c < vero.larghezza(); ++c) {
                    const double pv = vero.profondita_cella(c, r), pc = carta.profondita_cella(c, r);
                    const int k = carta.campagna_cella(c, r);
                    if (k >= static_cast<int>(carta.campagne().size())) ++campagne_fuori;
                    if (pv <= 0) {
                        if (std::fabs(pc - pv) > 5.0 + 1e-9) ++terra_sbagliata;
                        if (k >= 0) ++maschera_in_terra;
                    } else {
                        ++mare;
                        if (pc <= 0) ++mare_in_terra;
                        if (k < 0) ++non_ril;
                        errore += std::fabs(pc - pv);
                    }
                }
            CHECK(terra_sbagliata == 0);
            CHECK(mare_in_terra == 0);
            CHECK(campagne_fuori == 0);
            CHECK(maschera_in_terra == 0);
            errore /= static_cast<double>(mare);
            CHECK(errore > 0.5);     // la carta non e' il territorio...
            CHECK(errore < 150.0);   // ...ma ci somiglia
            st[std::string(t) + "_" + nome_parte(parte)] = {static_cast<double>(non_ril) / static_cast<double>(mare), errore};
        }
    }
    // la tabella si inverte nelle acque di casa sovietiche (docs/08 §5)
    CHECK(st["barents_urss"].non_rilevato < 0.01);
    CHECK(st["barents_nato"].non_rilevato > 0.15);
    CHECK(st["barents_urss"].errore < st["barents_nato"].errore);
    CHECK(st["kunashir_urss"].non_rilevato < st["kunashir_nato"].non_rilevato);
    CHECK(st["baltico_urss"].non_rilevato < st["baltico_nato"].non_rilevato);
    CHECK(st["faroe_nato"].non_rilevato < st["faroe_urss"].non_rilevato);
    CHECK(st["gibilterra_nato"].non_rilevato < st["gibilterra_urss"].non_rilevato);
    CHECK(st["celtica_nato"].non_rilevato < st["celtica_urss"].non_rilevato);
    // Gibilterra e' la carta NATO migliore, Kunashir la peggiore (per la NATO)
    CHECK(st["gibilterra_nato"].non_rilevato < 0.01);
    CHECK(st["kunashir_nato"].non_rilevato > 0.2);
}

TEST_CASE("carte d'epoca: un rilievo stretto fra le linee sparisce, i campi letti dal file") {
    // carta a mano: 3x1 celle, campagna 1 sulla prima, non rilevate le altre
    Fondale f(3, 1, 200.0, 1.0, -100.0, {50, 50, 50});
    f.imposta_carta(1988, {Campagna{1970, "prova", 1000.0, 5000.0, 200.0}}, {1, 0, 0},
                    {Annotazione{Annotazione::Tipo::pa, {10, 20}, 35.0, 1975}});
    CHECK(f.carta_epoca());
    CHECK(f.campagna({-200, 0}) == 0);
    CHECK(f.campagna({0, 0}) == -1);
    CHECK(f.rilevato({-200, 0}));
    CHECK_FALSE(f.rilevato({200, 0}));
    CHECK(f.campagna({0, 1000}) == -1);   // fuori dalla griglia
    REQUIRE(f.annotazioni().size() == 1);
    CHECK(f.annotazioni()[0].tipo == Annotazione::Tipo::pa);
    CHECK_THROWS_AS(f.imposta_carta(1988, {}, {1, 0}), std::invalid_argument);
    CHECK(std::string(nome_parte(Parte::urss)) == "urss");
    CHECK(percorso_carta("dati", "faroe", Parte::nato) == "dati/teatri/faroe_carta_nato.fdl");
}
