#include <doctest.h>

#include <cmath>

#include "nucleo/aob_ia.hpp"

using namespace m688;

// L'AOB stimato a vista dall'IA (simmetria col giocatore, docs/25 §3.4-3.5): la geometria, la sigma del giocatore che
// cresce con la distanza, la notte e la vela, i passi di 10 gradi, il lato ambiguo vicino all'asse

TEST_CASE("AOB dell'IA: l'AOB vero e la rotta, la sigma nelle condizioni") {
    // il bersaglio a nord che viene incontro: di prua (0); che va a est: 90 (rotta = 0 + 180 - 90); a ovest: -90
    CHECK(aob_vero(0.0, 180.0) == doctest::Approx(0.0));
    CHECK(aob_vero(0.0, 90.0) == doctest::Approx(90.0));
    CHECK(aob_vero(0.0, 270.0) == doctest::Approx(-90.0));
    CHECK(std::fabs(aob_vero(0.0, 0.0)) == doctest::Approx(180.0));   // di poppa
    CHECK(normalizza_360(45.0 + 180.0 - aob_vero(45.0, 300.0)) == doctest::Approx(300.0));
    CondizioniAob c;
    c.distanza_m = 0.0;
    CHECK(sigma_aob(c) == doctest::Approx(10.0));   // come il giocatore ad alto ingrandimento
    c.alto_ingrandimento = false;
    CHECK(sigma_aob(c) == doctest::Approx(15.0));   // e a basso
    c.alto_ingrandimento = true;
    c.distanza_m = c.portata_m;
    CHECK(sigma_aob(c) == doctest::Approx(20.0));   // alla portata il doppio
    c.luce = 0.0;
    CHECK(sigma_aob(c) == doctest::Approx(40.0));   // di notte sulla sola sagoma
    c.luci = true;
    CHECK(sigma_aob(c) == doctest::Approx(30.0));   // coi fanali meno
    c.luci = false;
    c.solo_vela = true;
    CHECK(sigma_aob(c) == doctest::Approx(60.0));   // la vela di notte alla portata: al massimo
}

TEST_CASE("AOB dell'IA: le stime a passi di 10 gradi, senza deriva, con la sigma; il lato ambiguo vicino all'asse") {
    Casuale caso(7);
    CondizioniAob c;
    c.distanza_m = 5000.0;   // un quarto della portata: sigma 12,5
    const double sigma = sigma_aob(c);
    double somma = 0.0, quadrati = 0.0;
    const int n = 4000;
    for (int k = 0; k < n; ++k) {
        const StimaAob s = stima_aob(60.0, 30.0, c, caso);
        CHECK(std::fmod(std::fabs(s.aob_gradi) + 1e-9, 10.0) < 1e-6);
        CHECK(s.rotta == doctest::Approx(normalizza_360(30.0 + 180.0 - s.aob_gradi)));
        CHECK_FALSE(s.specchiato);   // a 60 gradi il lato si vede
        const double e = normalizza_180(s.aob_gradi - 60.0);
        somma += e;
        quadrati += e * e;
    }
    const double media = somma / n, rms = std::sqrt(quadrati / n);
    MESSAGE("AOB 60 a 5 km: errore medio ", media, ", rms ", rms, " (sigma ", sigma, ", passi di 10)");
    CHECK(std::fabs(media) < 1.0);
    CHECK(rms > sigma * 0.9);
    CHECK(rms < std::hypot(sigma, 10.0 / std::sqrt(12.0)) * 1.1);
    // vicino alla prua il lato si specchia a volte; coi fanali di notte mai; oltre 20 gradi mai
    CHECK(probabilita_specchio(30.0, c) == 0.0);
    CHECK(probabilita_specchio(175.0, c) > 0.0);
    int specchiati = 0;
    for (int k = 0; k < n; ++k) specchiati += stima_aob(5.0, 0.0, c, caso).specchiato;
    MESSAGE("AOB 5: specchiati ", specchiati, " su ", n, " (P ", probabilita_specchio(5.0, c), ")");
    CHECK(static_cast<double>(specchiati) / n == doctest::Approx(probabilita_specchio(5.0, c)).epsilon(0.15));
    CondizioniAob notte = c;
    notte.luce = 0.0;
    notte.luci = true;
    CHECK(probabilita_specchio(5.0, notte) == 0.0);
    // lo stesso seme, le stesse stime
    Casuale a(3), b(3);
    for (int k = 0; k < 20; ++k) CHECK(stima_aob(40.0, 10.0, c, a).aob_gradi == stima_aob(40.0, 10.0, c, b).aob_gradi);
}
