#include <doctest.h>

#include <cmath>
#include <functional>
#include <vector>

#include "nucleo/quadro_fondale.hpp"
#include "nucleo/ritorni_fondo.hpp"

using namespace m688;

// Il BQS-15 sul fondo e sulle pareti (quadro del fondale, docs/24 §2.2)

namespace {

constexpr int N = 200;   // 40 x 40 km a celle da 200 m

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

// una parete che sale da 2000 m a 50 m fra y = 2000 e y = 2200 (a nord del battello)
const Fondale& parete() {
    static const Fondale f = fondale([](double, double y) { return y < 2000.0 ? 2000.0 : y > 2200.0 ? 50.0 : 2000.0 - 1950.0 * (y - 2000.0) / 200.0; });
    return f;
}

int echi_a_prua(const std::vector<RitornoFondo>& v) {
    int n = 0;
    for (const auto& r : v) n += r.eco && std::fabs(normalizza_180(r.rilevamento)) < 10.0;
    return n;
}

}  // namespace

TEST_CASE("bqs15 sul fondo: una parete a 2,1 km si vede a 10 kn e non a 25; la base e l'ostacolo davanti alla chiglia") {
    const ParametriBqs15Fondo p;
    Casuale caso(1);
    const auto lento = spazza_fondo(parete(), {0.0, 0.0}, {0.0, 0.0}, 200.0, 0.0, 10.0, 5.0, p, caso);
    const auto veloce = spazza_fondo(parete(), {0.0, 0.0}, {0.0, 0.0}, 200.0, 0.0, 25.0, 5.0, p, caso);
    CHECK(lento.size() >= 17);   // da -60 a +60 ogni 7,5 gradi, uno o due echi per raggio
    CHECK(echi_a_prua(lento) >= 2);
    CHECK(echi_a_prua(veloce) == 0);
    bool base = false, davanti = false;
    for (const auto& r : lento)
        if (r.eco && std::fabs(normalizza_180(r.rilevamento)) < 3.0) {
            if (!r.ostacolo) base = base || (r.distanza_m > 2200.0 && r.distanza_m < 2500.0 && r.profondita_m > 1000.0);
            else davanti = davanti || (r.punto.y > 2050.0 && r.punto.y < 2300.0 && r.profondita_m < 260.0);
        }
    CHECK(base);      // la base della parete, in basso: l'eco piu' vicino
    CHECK(davanti);   // e la parete davanti alla chiglia, a ~2,2 km
    CHECK(portata_effettiva(3200.0, 0.0, p) == doctest::Approx(3200.0));
    CHECK(portata_effettiva(3200.0, 15.0, p) == doctest::Approx(2240.0));
    CHECK(portata_effettiva(3200.0, 25.0, p) == doctest::Approx(1280.0));
}

TEST_CASE("bqs15 sul fondo: il fondo radente vicino torna, lontano no (i raggi liberi); la posizione stimata") {
    const ParametriBqs15Fondo p;
    Casuale caso(2);
    // il fondo a 500 m, la chiglia a 200: il bordo basso del ventaglio lo tocca a ~520 m, obliqua ~600 m
    const Fondale basso = fondale([](double, double) { return 500.0; });
    const auto v = spazza_fondo(basso, {0.0, 0.0}, {1000.0, 0.0}, 200.0, 90.0, 5.0, 0.0, p, caso);
    int echi = 0;
    for (const auto& r : v) {
        if (!r.eco) continue;
        ++echi;
        CHECK_FALSE(r.ostacolo);
        CHECK(r.profondita_m == doctest::Approx(500.0).epsilon(0.08));
        CHECK(r.origine.x == 1000.0);   // dove il battello crede di essere
    }
    CHECK(echi == 17);
    // il fondo a 1500 m: tocca il ventaglio a ~2250 m, obliqua ~2600: oltre la portata del fondo radente
    const Fondale profondo = fondale([](double, double) { return 1500.0; });
    const auto w = spazza_fondo(profondo, {0.0, 0.0}, {0.0, 0.0}, 200.0, 0.0, 5.0, 0.0, p, caso);
    for (const auto& r : w) {
        CHECK_FALSE(r.eco);
        CHECK(r.elevazione == p.elevazione_min_deg);
        CHECK(r.distanza_m == doctest::Approx(portata_effettiva(p.portata_fondo_m, 5.0, p)));
    }
}

TEST_CASE("bqs15 sul fondo: nel quadro l'ostacolo a prua e la velocita' sicura; i dati e le classi; ripetibile") {
    const Fondale carta = fondale([](double, double) { return 2000.0; });   // la carta non ha la parete
    QuadroFondale q(carta);
    Casuale caso(3);
    const ParametriBqs15Fondo p = carica_bqs15_fondo(M688_DATI "/acustica.toml");
    for (const auto& r : spazza_fondo(parete(), {0.0, 0.0}, {0.0, 0.0}, 200.0, 0.0, 12.0, 10.0, p, caso)) q.aggiungi(r, 20.0);
    bool ostacolo = false, veloce = false;
    for (const auto& a : q.avvisi({0.0, 0.0}, 200.0, 0.0, 12.0, 12.0, 600.0)) {
        ostacolo = ostacolo || a.tipo == AvvisoFondo::Tipo::ostacolo_a_prua;
        veloce = veloce || a.tipo == AvvisoFondo::Tipo::velocita_eccessiva;
    }
    CHECK(ostacolo);   // 2,2 km a 12 kn: dentro i 600 s di reazione (3,7 km)
    CHECK(veloce);     // la parete a 2,2 km per 600 s: ~7 kn sicuri
    // i dati
    CHECK(p.portata_parete_m == ParametriBqs15Fondo{}.portata_parete_m);
    REQUIRE(bqs15_fondo_di_classe(M688_DATI "/acustica.toml", "los_angeles_flight3"));
    CHECK(bqs15_fondo_di_classe(M688_DATI "/acustica.toml", "los_angeles_flight3")->id == "bqs15");
    CHECK(bqs15_fondo_di_classe(M688_DATI "/acustica.toml", "akula")->fattore_portata < 1.0);
    CHECK_FALSE(bqs15_fondo_di_classe(M688_DATI "/acustica.toml", "perry"));
    // ripetibile
    Casuale a(7), b(7);
    const auto x = spazza_fondo(parete(), {0.0, 0.0}, {0.0, 0.0}, 200.0, 0.0, 10.0, 0.0, p, a);
    const auto y = spazza_fondo(parete(), {0.0, 0.0}, {0.0, 0.0}, 200.0, 0.0, 10.0, 0.0, p, b);
    REQUIRE(x.size() == y.size());
    for (std::size_t i = 0; i < x.size(); ++i) CHECK(x[i].distanza_m == y[i].distanza_m);
}
