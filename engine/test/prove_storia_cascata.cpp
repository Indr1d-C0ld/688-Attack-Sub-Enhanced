#include <doctest.h>

#include <algorithm>
#include <cmath>
#include <sstream>
#include <vector>

#include "nucleo/casuale.hpp"
#include "nucleo/storia_cascata.hpp"

using namespace m688;

// La storia lunga delle cascate (blocco 3 del sonar, I; docs/23_sonar_blocco3.md §4)

namespace {

double gauss(Casuale& c) {
    const double u1 = std::max(c.uniforme(), 1e-12), u2 = c.uniforme();
    return std::sqrt(-2.0 * std::log(u1)) * std::cos(2.0 * PI * u2);
}

// una riga del LOFAR: `bin` bin di grana, le righe date (indice, S/N)
std::vector<float> riga(Casuale& caso, std::size_t bin, const std::vector<std::pair<std::size_t, double>>& righe) {
    std::vector<float> v(bin);
    for (float& x : v) x = static_cast<float>(std::max(0.0, 1.5 + 1.5 * gauss(caso)));
    for (const auto& [i, db] : righe) v[i] = static_cast<float>(db);
    return v;
}

}  // namespace

TEST_CASE("storia delle cascate: la decimazione col massimo tiene i picchi, le finestre a ogni livello") {
    Casuale caso(1);
    StoriaCascata s(10.0, 0.1, 400);
    for (int k = 0; k < 3 * 3600; ++k) {   // tre ore a una riga al secondo; un lampo di 20 dB al bin 200 a t = 1234
        const double t = k;
        s.aggiungi(t, riga(caso, 400, k == 1234 ? std::vector<std::pair<std::size_t, double>>{{200, 20.0}} : std::vector<std::pair<std::size_t, double>>{}));
    }
    CHECK(s.fine_s() == doctest::Approx(10799.0));
    CHECK(s.inizio_s() == doctest::Approx(0.0));
    // il livello fine tiene 10 minuti, quello da 10 s due ore, quello da 60 s tutto
    const FinestraStoria fine = s.finestra(10200.0, 10799.0);
    CHECK(fine.passo_s == doctest::Approx(1.0));
    CHECK(fine.righe() >= 599);
    const FinestraStoria media = s.finestra(5000.0, 6000.0);
    CHECK(media.passo_s == doctest::Approx(10.0));
    CHECK(media.righe() == doctest::Approx(100).epsilon(0.02));
    const FinestraStoria grossa = s.finestra(0.0, 3600.0);
    CHECK(grossa.passo_s == doctest::Approx(60.0));
    // il lampo c'e' ancora nella riga da 60 s che lo contiene, a 20 dB (quantizzato a 0,25)
    bool trovato = false;
    for (std::size_t r = 0; r < grossa.righe(); ++r)
        if (grossa.t[r] == 1200.0) {
            CHECK(grossa.valore(r, 200) == doctest::Approx(20.0).epsilon(0.02));
            trovato = true;
        }
    CHECK(trovato);
    // un livello chiesto apposta
    CHECK(s.finestra(10700.0, 10799.0, 2).passo_s == doctest::Approx(60.0));
}

TEST_CASE("storia delle cascate: quando e' comparsa una riga") {
    Casuale caso(2);
    StoriaCascata s(10.0, 0.1, 400);
    for (int k = 0; k < 4 * 3600; ++k) {   // la riga al bin 120 compare a 1 h 30 e resta, con buchi brevi (frange)
        const double t = k;
        std::vector<std::pair<std::size_t, double>> r;
        if (t >= 5400.0 && (k % 300) > 20) r.push_back({120, 9.0});
        s.aggiungi(t, riga(caso, 400, r));
    }
    const auto c = s.comparsa(10.0 + 120.5 * 0.1);
    REQUIRE(c.has_value());
    MESSAGE("comparsa a " << *c << " s (vera 5400)");
    CHECK(*c == doctest::Approx(5400.0).epsilon(0.005));   // nel livello da 60 s: piu' vecchia di due ore
    // dove non c'e' niente: nulla
    CHECK_FALSE(s.comparsa(10.0 + 300.5 * 0.1).has_value());
    // il BTR: una traccia a 47 gradi da 30 minuti
    StoriaCascata btr(0.0, 1.0, 360);
    for (int k = 0; k < 7200; ++k) {
        std::vector<std::pair<std::size_t, double>> r;
        if (k >= 5400) r.push_back({47, 8.0});
        btr.aggiungi(static_cast<double>(k), riga(caso, 360, r));
    }
    const auto b = btr.comparsa(47.5);
    REQUIRE(b.has_value());
    CHECK(*b == doctest::Approx(5400.0).epsilon(0.002));
}

TEST_CASE("storia delle cascate: la memoria, il salvataggio e la ricarica") {
    Casuale caso(3);
    StoriaCascata s(10.0, 0.1, 4000);
    for (int k = 0; k < 13 * 3600; k += 1) s.aggiungi(static_cast<double>(k), riga(caso, 4000, {{1000, 12.0}}));
    MESSAGE("memoria con 4000 bin e 13 ore: " << static_cast<double>(s.memoria_byte()) / 1e6 << " MB");
    CHECK(s.memoria_byte() < 9'000'000u);
    CHECK(s.inizio_s() >= 13 * 3600 - 43200 - 60);   // le 12 ore piu' recenti
    std::stringstream buf;
    s.salva(buf);
    const StoriaCascata r = StoriaCascata::carica(buf);
    CHECK(r.bin() == 4000);
    CHECK(r.fine_s() == s.fine_s());
    CHECK(r.inizio_s() == s.inizio_s());
    for (const auto& [da, a] : std::vector<std::pair<double, double>>{{46000.0, 46799.0}, {40000.0, 41000.0}, {5000.0, 9000.0}}) {
        const FinestraStoria x = s.finestra(da, a), y = r.finestra(da, a);
        CHECK(x.passo_s == y.passo_s);
        CHECK(x.t == y.t);
        CHECK(x.db == y.db);
    }
    CHECK(r.comparsa(10.0 + 1000.5 * 0.1) == s.comparsa(10.0 + 1000.5 * 0.1));
    // un salvataggio rotto non si carica
    std::stringstream rotto(buf.str().substr(0, 100));
    CHECK_THROWS(StoriaCascata::carica(rotto));
    std::stringstream altro("non sono una storia");
    CHECK_THROWS(StoriaCascata::carica(altro));
}
