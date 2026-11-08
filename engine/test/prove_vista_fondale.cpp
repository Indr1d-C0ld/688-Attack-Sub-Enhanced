// Le viste del quadro del fondale (app/vista_fondale): sulla carta le celle misurate e il nastro dei sondaggi, la
// striscia dell'ecoscandaglio, la sezione di prua e la prospettiva dei ritorni disegnano solo carta e misure.
#include <doctest.h>

#include <cmath>
#include <cstdlib>
#include <filesystem>
#include <string>

#include "../app/vista_fondale.hpp"
#include "grafica/carta_vga.hpp"
#include "grafica/palette.hpp"
#include "grafica/tema_stazioni.hpp"
#include "nucleo/fondale.hpp"
#include "nucleo/quadro_fondale.hpp"

using namespace m688;
namespace fs = std::filesystem;

namespace {

bool c_e() { return fs::exists(M688_DATI "/teatri/gibilterra.fdl"); }

// una cella d'acqua aperta, fra 300 e 1500 m, con altre celle d'acqua per 5 km verso nord
bool acqua_aperta(const Fondale& f, const QuadroFondale& q, Vec2& p) {
    for (int r = 0; r < q.altezza(); r += 7)
        for (int c = 0; c < q.larghezza(); c += 7) {
            const Vec2 x = q.centro_cella(c, r);
            bool ok = true;
            for (double d = 0.0; d <= 5000.0 && ok; d += 500.0) {
                const double z = f.profondita({x.x, x.y + d});
                ok = std::isfinite(z) && z > 300.0 && z < 1500.0;
            }
            if (ok) {
                p = x;
                return true;
            }
        }
    return false;
}

// dieci sondaggi verso nord, uno ogni 500 m, col fondo misurato 40 m piu' basso della carta (la carta sbaglia)
QuadroFondale quadro_con_misure(const Fondale& f, Vec2 p0) {
    QuadroFondale q(f);
    for (int i = 0; i < 10; ++i) {
        Sondaggio s;
        s.t = 60.0 * i;
        s.pos = {p0.x, p0.y + 500.0 * i};
        s.quota_m = 60.0;
        s.eco = true;
        s.fondo_m = f.profondita(s.pos) + 40.0;
        s.sotto_chiglia_m = s.fondo_m - s.quota_m;
        s.sigma_m = 2.0;
        s.impronta_m = 150.0;
        q.aggiungi(s, 50.0);
    }
    return q;
}

int quanti(const Tela& t, Rettangolo r, int da, int a) {
    int n = 0;
    for (int y = r.y; y < r.y + r.h; ++y)
        for (int x = r.x; x < r.x + r.w; ++x) n += t.at(x, y) >= da && t.at(x, y) <= a;
    return n;
}

}  // namespace

TEST_CASE("vista del fondale: le misure sulla carta, l'ecoscandaglio, la sezione di prua e la prospettiva" * doctest::skip(!c_e())) {
    const Fondale f = Fondale::carica(M688_DATI "/teatri/gibilterra.fdl");
    QuadroFondale vuoto(f);
    Vec2 p0;
    REQUIRE(acqua_aperta(f, vuoto, p0));
    const QuadroFondale q = quadro_con_misure(f, p0);
    REQUIRE(!q.celle_misurate().empty());
    const double ora = 600.0;

    // la carta: una tela d'acqua di una fascia; le misure la cambiano lungo il nastro, il resto e' tratteggiato
    VistaCarta v;
    v.x = 0, v.y = 0, v.w = 320, v.h = 240;
    v.centro = {p0.x, p0.y + 2500.0};
    v.metri_per_pixel = 50.0;
    Tela t;
    t.riempi(indice_acqua(0, 1));
    disegna_misure_carta(t, v, q, ora);
    CHECK(quanti(t, {0, 0, 320, 240}, colori_carta_vga::MAGENTA, colori_carta_vga::MAGENTA) >= 10);   // il nastro dei sondaggi
    CHECK(quanti(t, {0, 0, 320, 240}, colori_carta_vga::MATITA, colori_carta_vga::MATITA) > 1000);  // «da carta», puntinata
    const int misurate = quanti(t, {0, 0, 320, 240}, colori_carta_vga::ACQUA + 3, colori_carta_vga::ACQUA + 47);
    CHECK(misurate > 20);   // le celle misurate, nella fascia della profondita' misurata
    // lontano dal nastro, niente misure: solo acqua e tratti
    CHECK(quanti(t, {0, 0, 40, 40}, colori_carta_vga::ACQUA + 3, colori_carta_vga::ACQUA + 47) == 0);

    // la striscia dell'ecoscandaglio: i segni neri sulla carta termica, uno per sondaggio
    Tela e;
    e.riempi(255);
    disegna_ecoscandaglio(e, Font::carica(M688_DATI "/font/originale.fnt"), {0, 0, 300, 120}, q, ora, {5.0, true});
    CHECK(quanti(e, {34, 14, 262, 92}, 0, 0) >= 10 * 4);
    CHECK(quanti(e, {0, 0, 300, 120}, 255, 255) == 0);   // tutta la striscia coperta

    // la sezione di prua lungo il nastro: la stima misurata (fosforo pieno) e la carta a tratti
    Tela s;
    s.riempi(255);
    disegna_sezione_prua(s, Font::carica(M688_DATI "/font/originale.fnt"), {0, 0, 320, 120}, q, p0, 0.0, 60.0, 30.0, ora, 6000.0);
    CHECK(quanti(s, {34, 14, 282, 102}, fosforo(6), fosforo(6)) > 50);
    CHECK(quanti(s, {34, 14, 282, 102}, fosforo(3), fosforo(3)) > 30);
    CHECK(quanti(s, {34, 14, 282, 102}, colori::VERDE_CHIARO, colori::VERDE_CHIARO) > 100);   // la quota propria

    // la prospettiva: la griglia, piena dove misurata e a tratti altrove
    Tela pr;
    pr.riempi(255);
    disegna_ritorni_prospettiva(pr, Font::carica(M688_DATI "/font/originale.fnt"), {0, 0, 320, 200}, q, p0, 0.0, 60.0, ora, 5000.0);
    CHECK(quanti(pr, {0, 0, 320, 200}, fosforo(6), fosforo(6)) > 30);
    CHECK(quanti(pr, {0, 0, 320, 200}, fosforo(2), fosforo(2)) > 200);
}

TEST_CASE("vista della volta: la pagina sotto il ghiaccio e la striscia della carta disegnano solo le misure della volta") {
    // il battello verso nord a 60 m (la vela a 44): mezz'ora di sondatore alto lungo la scia, con una polynya passata
    // 3 minuti fa (a 700 m di poppa), e una spazzata del BQS-15 con una chiglia di 38 m a 2 km (sotto la vela) e una di 12 m
    QuadroVolta q;
    BattelloVolta b;
    b.pos = {0.0, 0.0}, b.rotta = 0.0, b.velocita_kn = 9.0, b.chiglia_m = 60.0, b.cima_vela_m = 44.0;
    const double ora = 1800.0;
    for (int k = 0; k <= 1800; k += 2) {
        SondaggioAlto s;
        s.t = k;
        s.pos = {0.0, -4.6 * (1800 - k)};
        s.quota_m = 60.0;
        s.eco = true;
        s.libera = k > 1620 && k < 1660;
        s.pescaggio_m = s.libera ? 0.0 : 3.0 + 2.0 * std::sin(k / 37.0);
        s.distanza_m = s.quota_m - s.pescaggio_m;
        q.aggiungi(s);
    }
    RitornoChiglia grossa, piccola;
    grossa.t = piccola.t = ora;
    grossa.punto = {0.0, 2000.0}, grossa.distanza_m = 2000.0, grossa.pescaggio_m = 38.0, grossa.ostacolo = true;
    piccola.punto = {300.0, 900.0}, piccola.distanza_m = 950.0, piccola.pescaggio_m = 12.0;
    q.aggiungi_spazzata(ora, 2500.0, {grossa, piccola});
    REQUIRE_FALSE(q.polynye(b.pos, 4000.0, ora, 3600.0).empty());

    const Font f = Font::carica(M688_DATI "/font/fosforo688_5x7.fnt");
    Tela t;
    t.riempi(255);
    disegna_pagina_volta(t, f, q, nullptr, b, ora);
    if (const char* d = std::getenv("M688_SALVA")) {   // per guardarla: M688_SALVA=<cartella>
        Palette p = Palette::carica(M688_DATI "/palette/giorno.pal");
        applica_tema(p, TemaStazione{});
        t.salva_bmp(std::string(d) + "/volta.bmp", p);
    }
    CHECK(quanti(t, {0, 0, 640, 360}, 255, 255) == 0);                       // tutta la parte di sopra
    CHECK(quanti(t, {4, 4, 400, 214}, 12, 12) > 5);                          // la chiglia sotto la vela, in rosso
    CHECK(quanti(t, {4, 4, 400, 214}, colori::CIANO_CHIARO, colori::CIANO_CHIARO) > 8);   // la polynya candidata
    CHECK(quanti(t, {412, 6, 222, 214}, colori::BLU, colori::BLU) > 0);      // l'acqua libera sulla carta termica
    CHECK(quanti(t, {4, 222, 632, 136}, colori::GIALLO, colori::GIALLO) > 50);   // la vela meno il margine
    CHECK(quanti(t, {4, 222, 632, 136}, colori::ROSSO, colori::ROSSO) > 0);      // la chiglia a prua, a tacche rosse

    // la carta: le celle lungo la scia, il cerchio della polynya, la croce rossa; fuori dalle misure niente
    VistaCarta v;
    v.x = 0, v.y = 0, v.w = 320, v.h = 240;
    v.centro = {0.0, -2000.0};
    v.metri_per_pixel = 50.0;
    Tela c;
    c.riempi(indice_acqua(0, 1));
    disegna_volta_carta(c, v, f, q, b, ora);
    CHECK(quanti(c, {0, 0, 320, 240}, colori::BIANCO, colori::BIANCO) > 20);
    CHECK(quanti(c, {0, 0, 320, 240}, colori::CIANO_CHIARO, colori::CIANO_CHIARO) > 20);
    CHECK(quanti(c, {0, 0, 320, 240}, colori::ROSSO, colori::ROSSO) > 5);
    CHECK(quanti(c, {260, 180, 60, 60}, 0, 15) == 0);   // lontano dalla scia la carta resta com'e'

    // senza misure: la carta non cambia fuori dalla legenda
    Tela vuota;
    vuota.riempi(indice_acqua(0, 1));
    disegna_volta_carta(vuota, v, f, QuadroVolta{}, b, ora);
    CHECK(quanti(vuota, {0, 70, 320, 170}, 0, 15) == 0);
}

TEST_CASE("vista della volta: gli echi falsi dell'ultimo ping sulla carta, che svaniscono") {
    VistaCarta v;
    v.x = 0, v.y = 0, v.w = 320, v.h = 240;
    v.centro = {0.0, 0.0};
    v.metri_per_pixel = 50.0;
    // due echi a nord e a est del battello, a 3 km, dal ping di 2 s fa; uno vecchio di 12 s
    const std::vector<EcoCarta> echi = {{98.0, 0.0, 3000.0, 9.0}, {98.0, 90.0, 3000.0, 4.0}, {88.0, 180.0, 3000.0, 9.0}};
    Tela t;
    t.riempi(indice_acqua(0, 1));
    disegna_echi_carta(t, v, echi, {0.0, 0.0}, 100.0, 10.0);
    const Vec2 nord = v.a_tela({0.0, 3000.0}), est = v.a_tela({3000.0, 0.0}), sud = v.a_tela({0.0, -3000.0});
    CHECK(t.at(static_cast<int>(nord.x), static_cast<int>(nord.y)) == colori::GIALLO);
    CHECK(t.at(static_cast<int>(est.x), static_cast<int>(est.y)) == colori::GIALLO);
    CHECK(t.at(static_cast<int>(sud.x), static_cast<int>(sud.y)) == indice_acqua(0, 1));   // svanito
    // a 8 s dal ping resta un pixel scuro, a 11 s niente
    Tela tardi;
    tardi.riempi(indice_acqua(0, 1));
    disegna_echi_carta(tardi, v, echi, {0.0, 0.0}, 106.0, 10.0);
    CHECK(tardi.at(static_cast<int>(nord.x), static_cast<int>(nord.y)) == colori::NERO);
    CHECK(quanti(tardi, {0, 0, 320, 240}, colori::GIALLO, colori::GIALLO) == 0);
    Tela spento;
    spento.riempi(indice_acqua(0, 1));
    disegna_echi_carta(spento, v, echi, {0.0, 0.0}, 109.0, 10.0);
    CHECK(quanti(spento, {0, 0, 320, 240}, 0, 15) == 0);
}
