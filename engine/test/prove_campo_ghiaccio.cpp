#include <doctest.h>

#include <algorithm>
#include <cmath>
#include <vector>

#include "nucleo/campo_ghiaccio.hpp"

using namespace m688;

// Il campo del ghiaccio (opzione A sotto il ghiaccio): chiglie di Ervik, acqua libera, nilas, deriva, deterministico

TEST_CASE("campo del ghiaccio: le chiglie di Ervik 2023, l'acqua libera del 10%, il nilas, la copertura") {
    const CampoGhiaccio g(7);
    std::vector<double> chiglie;
    int aperte = 0, nilas = 0, n = 0;
    for (int i = 0; i < 400; ++i)
        for (int j = 0; j < 400; ++j) {
            const PuntoGhiaccio x = g.in({i * 200.0 + 100.0, j * 200.0 + 100.0}, 0.0);
            ++n;
            aperte += x.aperta;
            nilas += x.nilas;
            if (x.chiglia) chiglie.push_back(x.pescaggio_m);
            if (x.nilas) {
                CHECK(x.spessore_m >= 0.05);
                CHECK(x.spessore_m <= 0.15);
            }
            if (x.ghiaccio && !x.nilas) CHECK(x.spessore_m == doctest::Approx(1.0).epsilon(0.31));
        }
    // a copertura piena il 10% fra aperta e nilas, meta' e meta' (a blocchi di 1,5 km: tolleranza larga)
    CHECK(static_cast<double>(aperte + nilas) / n == doctest::Approx(0.10).epsilon(0.35));
    CHECK(static_cast<double>(nilas) / std::max(1, aperte + nilas) == doctest::Approx(0.5).epsilon(0.3));
    // le chiglie: media 3,8, 99° percentile 9,4, mai oltre 20 (le piu' basse dello spessore non contano come chiglia)
    REQUIRE(chiglie.size() > 10000);
    std::sort(chiglie.begin(), chiglie.end());
    double somma = 0.0;
    for (double c : chiglie) somma += c;
    const double nc = static_cast<double>(chiglie.size());
    MESSAGE("chiglie: ", chiglie.size(), ", media ", somma / nc, " m, 99% ", chiglie[chiglie.size() * 99 / 100], " m, massima ", chiglie.back());
    CHECK(somma / nc == doctest::Approx(3.8).epsilon(0.08));
    CHECK(chiglie[chiglie.size() * 99 / 100] == doctest::Approx(9.4).epsilon(0.1));
    CHECK(chiglie.back() <= 20.0);
    // senza copertura niente; a mezza copertura piu' acqua libera
    const CampoGhiaccio vuoto(7, {}, [](Vec2) { return 0.0; });
    CHECK_FALSE(vuoto.in({1000.0, 1000.0}, 0.0).ghiaccio);
    const CampoGhiaccio mezzo(7, {}, [](Vec2) { return 0.5; });
    int aperte_mezzo = 0;   // l'acqua libera attesa e' il 55% (1 - 0,5 + 0,5 x 0,1), col nilas
    for (int i = 0; i < 200; ++i)
        for (int j = 0; j < 200; ++j) {
            const PuntoGhiaccio x = mezzo.in({i * 400.0, j * 400.0}, 0.0);
            aperte_mezzo += x.aperta || x.nilas;
        }
    CHECK(static_cast<double>(aperte_mezzo) / 40000.0 == doctest::Approx(0.55).epsilon(0.15));
    // il pluriennale: piu' spesso, creste piu' fonde
    const CampoGhiaccio vecchio(7, ghiaccio_pluriennale());
    double massimo = 0.0;
    for (int i = 0; i < 300; ++i)
        for (int j = 0; j < 300; ++j) massimo = std::max(massimo, vecchio.in({i * 200.0 + 100.0, j * 200.0 + 100.0}, 0.0).pescaggio_m);
    CHECK(massimo > 20.0);
    CHECK(massimo <= 50.0);
}

TEST_CASE("campo del ghiaccio: deterministico dal seme, deriva rigida, il pescaggio massimo lungo un tratto") {
    const CampoGhiaccio a(11), b(11), c(12);
    int diversi = 0;
    for (int k = 0; k < 500; ++k) {
        const Vec2 p{k * 137.0, k * 59.0};
        CHECK(a.in(p, 0.0).pescaggio_m == b.in(p, 0.0).pescaggio_m);
        diversi += a.in(p, 0.0).pescaggio_m != c.in(p, 0.0).pescaggio_m;
    }
    CHECK(diversi > 100);
    // la deriva: dopo un'ora il punto p vede quello che a t = 0 stava a p - deriva x 3600
    const Vec2 v = a.parametri().deriva_m_s;
    for (int k = 0; k < 50; ++k) {
        const Vec2 p{3000.0 + k * 211.0, 5000.0 - k * 97.0};
        CHECK(a.in(p, 3600.0).pescaggio_m == a.in({p.x - v.x * 3600.0, p.y - v.y * 3600.0}, 0.0).pescaggio_m);
    }
    CHECK(std::hypot(v.x, v.y) < 0.5144);   // sotto 1 nodo
    // il pescaggio massimo lungo un tratto non e' mai sotto quello dei suoi punti
    const double m = a.pescaggio_max({0.0, 0.0}, {5000.0, 0.0}, 0.0);
    for (double x = 0.0; x <= 5000.0; x += 100.0) CHECK(a.in({x, 0.0}, 0.0).pescaggio_m <= m + 1e-9);
}

// il vetrino di Dio, D4 (docs/28 §3.3): la deriva a tratti e le zone forzate; senza, il campo di prima
TEST_CASE("campo del ghiaccio: la deriva a tratti non fa saltare il campo, e il bordo del pack le va dietro") {
    CampoGhiaccio a(11), b(11);
    CHECK(b.spostamento(1000.0).x == doctest::Approx(a.parametri().deriva_m_s.x * 1000.0));
    b.cambia_deriva(1000.0, {1.0, 0.0});
    // fino al cambio identici; subito dopo, lo stesso punto del campo
    for (double x = -3000.0; x < 3000.0; x += 137.0) {
        const PuntoGhiaccio pa = a.in({x, 500.0}, 1000.0), pb = b.in({x, 500.0}, 1000.0);
        CHECK(pa.pescaggio_m == pb.pescaggio_m);
        CHECK(pa.aperta == pb.aperta);
    }
    CHECK(b.spostamento(1100.0).x == doctest::Approx(a.parametri().deriva_m_s.x * 1000.0 + 100.0));
    CHECK(b.spostamento(1100.0).y == doctest::Approx(a.parametri().deriva_m_s.y * 1000.0));
    // lo stesso punto del campo 100 s dopo sta 100 m piu' a est
    const Vec2 s0 = b.spostamento(1000.0), s1 = b.spostamento(1100.0);
    for (double x = -3000.0; x < 3000.0; x += 137.0)
        CHECK(b.in({x + (s1.x - s0.x), 500.0 + (s1.y - s0.y)}, 1100.0).pescaggio_m == b.in({x, 500.0}, 1000.0).pescaggio_m);
    // un istante piu' vecchio si ignora
    b.cambia_deriva(500.0, {9.0, 9.0});
    CHECK(b.spostamento(1100.0).x == doctest::Approx(s1.x));
    // il bordo: la copertura piena a est di x = 0. La deriva dei parametri non lo muove; quella cambiata si'
    const auto bordo = [](Vec2 p) { return p.x > 0.0 ? 1.0 : 0.0; };
    CampoGhiaccio c(11, ParametriGhiaccio{}, bordo);
    CHECK(c.copertura_in({-500.0, 0.0}, 2000.0) == 0.0);
    c.cambia_deriva(0.0, {-1.0, 0.0});   // il pack avanza verso ovest a 1 m/s
    CHECK(c.copertura_in({-500.0, 0.0}, 100.0) == 0.0);
    CHECK(c.copertura_in({-500.0, 0.0}, 1000.0) == 1.0);
}

TEST_CASE("campo del ghiaccio: la polynya che si apre dal centro e torna; quella che si chiude prima in nilas") {
    CampoGhiaccio g(5);
    const Vec2 centro{20000.0, 20000.0};
    int gia_ghiaccio = 0;
    for (double x = -900.0; x <= 900.0; x += 100.0) gia_ghiaccio += g.in({centro.x + x, centro.y}, 0.0).ghiaccio;
    REQUIRE(gia_ghiaccio > 5);
    ZonaGhiaccio z;
    z.centro = centro;
    z.raggio_m = 1000.0;
    z.t0 = 100.0;
    z.transizione_s = 100.0;
    z.durata_s = 200.0;
    g.forza_zona(z);
    const auto aperto = [&](double r, double t) { return g.in({centro.x + r, centro.y}, t).aperta && !g.in({centro.x + r, centro.y}, t).ghiaccio; };
    CHECK(g.copertura_in(centro, 50.0) == 1.0);
    CHECK(aperto(0.0, 150.0));                 // a meta' transizione il raggio e' 500 m
    CHECK(aperto(400.0, 150.0));
    CHECK(g.copertura_in({centro.x + 400.0, centro.y}, 150.0) == 0.0);
    CHECK(g.copertura_in({centro.x + 700.0, centro.y}, 150.0) == 1.0);
    for (double r = 0.0; r < 1000.0; r += 100.0) CHECK(aperto(r, 300.0));
    CHECK(g.copertura_in({centro.x + 1100.0, centro.y}, 300.0) == 1.0);
    CHECK(g.copertura_in({centro.x + 700.0, centro.y}, 450.0) == 1.0);   // al ritorno il raggio cala
    CHECK(g.copertura_in({centro.x + 100.0, centro.y}, 450.0) == 0.0);
    CHECK(g.copertura_in(centro, 600.0) == 1.0);                          // e' finita: il campo di prima
    CampoGhiaccio pulito(5);
    CHECK(g.in(centro, 600.0).pescaggio_m == pulito.in(centro, 600.0).pescaggio_m);
    // la polynya si chiude: sul mare libero prima il nilas che ingrossa, poi il ghiaccio livellato; il ghiaccio che c'era resta
    const auto libero = [](Vec2) { return 0.0; };
    CampoGhiaccio h(5, ParametriGhiaccio{}, libero);
    ZonaGhiaccio c;
    c.centro = centro;
    c.raggio_m = 1000.0;
    c.apri = false;
    c.t0 = 100.0;
    c.transizione_s = 600.0;
    h.forza_zona(c);
    CHECK_FALSE(h.in(centro, 50.0).ghiaccio);
    const PuntoGhiaccio inizio = h.in(centro, 160.0), fine = h.in(centro, 690.0), dopo = h.in(centro, 800.0);
    CHECK(inizio.nilas);
    CHECK(inizio.spessore_m < fine.spessore_m);
    CHECK(fine.nilas);
    CHECK(fine.spessore_m <= h.parametri().nilas_max_m);
    CHECK(dopo.ghiaccio);
    CHECK_FALSE(dopo.nilas);
    CHECK(dopo.spessore_m > 0.5);
    CHECK(h.copertura_in(centro, 400.0) == doctest::Approx(0.5));
    CHECK(h.copertura_in(centro, 800.0) == 1.0);
    CHECK_FALSE(h.in({centro.x + 1500.0, centro.y}, 800.0).ghiaccio);    // fuori dal cerchio niente
}
