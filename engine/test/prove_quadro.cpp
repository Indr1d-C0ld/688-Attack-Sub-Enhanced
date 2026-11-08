// I pezzi della variante C (grafica/quadro.hpp): lo schermo resta nel suo rettangolo e restituisce un'area dentro il
// vetro, le linguette non escono dall'area.
#include <doctest.h>

#include "grafica/font.hpp"
#include "grafica/quadro.hpp"
#include "grafica/tela.hpp"
#include "grafica/tema_stazioni.hpp"

#ifndef M688_DATI
#define M688_DATI "../data/motore"
#endif

using namespace m688;

TEST_CASE("quadro: lo schermo e le linguette restano nei loro rettangoli") {
    const Font f = Font::carica(std::string(M688_DATI) + "/font/fosforo688_5x7.fnt");
    Tela t(320, 200);
    t.riempi(255);
    const Rettangolo r{10, 20, 200, 150};
    const Rettangolo a = quadro::schermo(t, r, f, "TITOLO", "0:09:58");
    for (int y = 0; y < 200; ++y)
        for (int x = 0; x < 320; ++x)
            if (x < r.x || y < r.y || x >= r.x + r.w || y >= r.y + r.h) CHECK(t.at(x, y) == 255);
    CHECK(a.x > r.x);
    CHECK(a.y > r.y + f.righe());   // sotto la testata
    CHECK(a.x + a.w < r.x + r.w);
    CHECK(a.y + a.h < r.y + r.h);
    CHECK(t.at(a.x + a.w / 2, a.y + a.h / 2) == fosforo(0));   // il vetro spento
    Tela l(200, 40);
    l.riempi(255);
    const Rettangolo area{10, 5, 120, 30};
    quadro::linguette(l, area, 8, f, {"1 RIEPILOGO", "2 SCOPERTE", "3 ARMI", "4 REAZIONI E TMA"}, 1);
    for (int y = 0; y < 40; ++y)
        for (int x = 0; x < 200; ++x)
            if (x >= area.x + area.w) CHECK(l.at(x, y) == 255);   // le linguette che non ci stanno si lasciano fuori
}

TEST_CASE("quadro: il riquadro e la fascia del tempo restano nei loro rettangoli; la barra sta fra i due testi") {
    const Font f = Font::carica(std::string(M688_DATI) + "/font/fosforo688_5x7.fnt");
    Tela t(320, 200);
    t.riempi(255);
    const Rettangolo r{20, 30, 120, 60};
    const Rettangolo a = quadro::riquadro(t, r, f, "LEGENDA");
    const Rettangolo s{10, 150, 300, f.righe() + 6};
    const Rettangolo bar = quadro::fascia_tempo(t, s, f, "0:05:00 / 0:10:00", "03:44 EVENTO", 0.5, {{0.1, 3, 0}, {0.6, 3, 0}, {0.05, 12, 100}});
    for (int y = 0; y < 200; ++y)
        for (int x = 0; x < 320; ++x) {
            const bool in_r = x >= r.x && y >= r.y && x < r.x + r.w && y < r.y + r.h;
            const bool in_s = x >= s.x && y >= s.y && x < s.x + s.w && y < s.y + s.h;
            if (!in_r && !in_s) CHECK(t.at(x, y) == 255);
        }
    CHECK(a.y > r.y + f.righe());
    CHECK(a.x + a.w < r.x + r.w);
    CHECK(bar.x > s.x + 40);                 // dopo il tempo
    CHECK(bar.x + bar.w < s.x + s.w - 40);   // prima dell'evento
    CHECK(bar.y >= s.y);
    CHECK(bar.y + bar.h <= s.y + s.h);
    CHECK(t.at(bar.x + bar.w / 2, bar.y + bar.h / 2) == fosforo(7));   // il cursore a meta'
    CHECK(t.at(bar.x + static_cast<int>(0.05 * (bar.w - 1) + 0.5), bar.y) == 12);   // la tacca a tutta altezza
}

TEST_CASE("quadro: le tacche hanno il colore dell'evento, l'altezza dell'interesse, le piu' gravi sopra e il cursore sopra tutte") {
    const Font f = Font::carica(std::string(M688_DATI) + "/font/fosforo688_5x7.fnt");
    Tela t(320, 40);
    const auto x_di = [](Rettangolo b, double u) { return b.x + static_cast<int>(u * (b.w - 1) + 0.5); };
    const auto altezza = [&](Rettangolo b, double u, std::uint8_t colore) {
        int n = 0;
        for (int y = b.y; y < b.y + b.h; ++y) n += t.at(x_di(b, u), y) == colore;
        return n;
    };
    t.riempi(255);
    // un colore per tacca, tutti diversi da quelli della barra; nell'ordine sbagliato, la piu' grave prima
    const Rettangolo b = quadro::fascia_tempo(t, {4, 4, 312, f.righe() + 6}, f, "T", "", 0.9,
                                              {{0.1, 11, 100}, {0.2, 12, 70}, {0.3, 13, 69}, {0.4, 14, 50}, {0.5, 15, 49}, {0.6, 9, 30}, {0.7, 10, 29}, {0.8, 3, 0}});
    REQUIRE(b.h >= 7);   // tutta altezza: la barra intera, qualunque sia il font
    CHECK(altezza(b, 0.1, 11) == b.h);
    CHECK(altezza(b, 0.2, 12) == b.h);
    CHECK(altezza(b, 0.3, 13) == 5);
    CHECK(altezza(b, 0.4, 14) == 5);
    CHECK(altezza(b, 0.5, 15) == 3);
    CHECK(altezza(b, 0.6, 9) == 3);
    CHECK(altezza(b, 0.7, 10) == 1);
    CHECK(altezza(b, 0.8, 3) == 1);
    // sullo stesso pixel vince la piu' grave, comunque arrivino
    t.riempi(255);
    const Rettangolo c = quadro::fascia_tempo(t, {4, 4, 312, f.righe() + 6}, f, "T", "", 0.9, {{0.5, 11, 90}, {0.5, 12, 10}, {0.5, 13, 60}});
    CHECK(altezza(c, 0.5, 11) == c.h);
    t.riempi(255);
    const Rettangolo d = quadro::fascia_tempo(t, {4, 4, 312, f.righe() + 6}, f, "T", "", 0.9, {{0.5, 12, 10}, {0.5, 13, 60}, {0.5, 11, 90}});
    CHECK(altezza(d, 0.5, 11) == d.h);
    // il cursore dell'ora sopra una tacca a tutta altezza
    t.riempi(255);
    const Rettangolo e = quadro::fascia_tempo(t, {4, 4, 312, f.righe() + 6}, f, "T", "", 0.5, {{0.5, 11, 100}});
    CHECK(altezza(e, 0.5, fosforo(7)) == e.h);
}

TEST_CASE("quadro: la barra statistica sta nella sua riga, la media bianca e la scatola del colore") {
    Tela t(200, 20);
    t.riempi(255);
    const Rettangolo r{20, 4, 100, 7};
    quadro::barra_statistica(t, r, 10.0, 5.0, 1.0, 2.0, 8.0, 11);
    for (int y = 0; y < 20; ++y)
        for (int x = 0; x < 200; ++x)
            if (x < r.x || y < r.y || x >= r.x + r.w || y >= r.y + r.h) CHECK(t.at(x, y) == 255);
    const int xm = r.x + static_cast<int>(0.5 * (r.w - 1) + 0.5);
    CHECK(t.at(xm, r.y + r.h / 2) == 15);       // la media
    CHECK(t.at(xm - 6, r.y + r.h / 2) == 11);   // dentro media +- scarto
    CHECK(t.at(r.x + r.w - 2, r.y + r.h / 2) != 11);   // oltre il massimo: solo la scala
}


TEST_CASE("quadro: lo stato vuoto sta nel suo rettangolo, col titolo acceso") {
    const Font g = Font::carica(std::string(M688_DATI) + "/font/fosforo688_5x7.fnt");
    Tela t(320, 200);
    t.riempi(0);
    const Rettangolo r{20, 20, 260, 150};
    quadro::stato_vuoto(t, r, g, "NESSUNA MISSIONE ANCORA", {"LA CARTA SI COLORA", "DOVE PORTI IL BATTELLO"});
    bool fuori = false, acceso = false;
    for (int y = 0; y < 200; ++y)
        for (int x = 0; x < 320; ++x) {
            const bool dentro = x >= r.x && y >= r.y && x < r.x + r.w && y < r.y + r.h;
            fuori = fuori || (t.at(x, y) && !dentro);
            acceso = acceso || t.at(x, y) == fosforo(6);
        }
    CHECK_FALSE(fuori);
    CHECK(acceso);
}

TEST_CASE("quadro: il profilo di quota sta nella sua area; la traccia, lo strato e la quota periscopica alle loro quote") {
    const Font g = Font::carica(std::string(M688_DATI) + "/font/fosforo688_5x7.fnt");
    Tela t(640, 400);
    t.riempi(0);
    const Rettangolo r{10, 10, 400, 300};
    quadro::ProfiloQuota p;
    p.periscopica_m = 18.0, p.strato_m = 62.0, p.scala_m = 150.0;
    for (int k = 0; k <= 60; ++k) p.quota_m.push_back(45.0), p.fondo_m.push_back(k < 30 ? 0.0 : 120.0);   // il fondo a meta'
    quadro::profilo_quota(t, r, g, p);
    bool fuori = false;
    int traccia = 0, strato = 0, periscopica = 0, retino_sinistra = 0, retino_destra = 0;
    for (int y = 0; y < 400; ++y)
        for (int x = 0; x < 640; ++x) {
            const int c = t.at(x, y);
            if (!c) continue;
            fuori = fuori || x < r.x || y < r.y || x >= r.x + r.w || y >= r.y + r.h;
            traccia += c == fosforo(7);
            strato += c == 11;
            periscopica += c == 10;
            if (c == fosforo(2) && y > r.y + r.h / 2) (x < r.x + r.w / 2 - 20 ? retino_sinistra : retino_destra)++;
        }
    CHECK_FALSE(fuori);
    CHECK(traccia > 300);   // una riga orizzontale continua lungo tutta la larghezza
    CHECK(strato > 100);
    CHECK(periscopica > 100);
    CHECK(retino_destra > 1000);   // il fondo solo dove e' misurato
    CHECK(retino_sinistra < retino_destra / 10);
    // la traccia a 45 m sta fra la quota periscopica (18) e lo strato (62): la riga di fosforo(7) piu' frequente
    int y_traccia = -1, y_strato = -1, y_peri = -1, migliore = 0;
    for (int y = r.y; y < r.y + r.h; ++y) {
        int n7 = 0, n11 = 0, n10 = 0;
        for (int x = r.x; x < r.x + r.w; ++x) n7 += t.at(x, y) == fosforo(7), n11 += t.at(x, y) == 11, n10 += t.at(x, y) == 10;
        if (n7 > migliore) migliore = n7, y_traccia = y;
        if (n11 > 100) y_strato = y;
        if (n10 > 100) y_peri = y;
    }
    CHECK(y_peri < y_traccia);
    CHECK(y_traccia < y_strato);
    // vuoto: niente traccia, niente fondo, ma la scala si', senza uscire
    Tela v(640, 400);
    v.riempi(0);
    quadro::profilo_quota(v, r, g, {});
    int scritto = 0;
    for (int y = 0; y < 400; ++y)
        for (int x = 0; x < 640; ++x) scritto += v.at(x, y) != 0;
    CHECK(scritto > 0);
}
