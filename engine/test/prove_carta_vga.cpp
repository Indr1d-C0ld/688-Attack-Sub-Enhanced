// Carta di navigazione VGA/SVGA (grafica/carta_vga): fondale al pixel, rilievo, costa, isobate, cache che si trasla.
#include <doctest.h>

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <string>
#include <vector>

#include "grafica/carta_vga.hpp"
#include "grafica/font.hpp"

using namespace m688;
namespace fs = std::filesystem;

namespace {

// 120 x 80 celle da 500 m: mare a ovest (profondita' che cresce verso ovest fino a 2600 m), terra a est con una collina
Fondale fondale_prova(bool isola = false) {
    std::vector<std::uint16_t> val(120 * 80);
    for (int r = 0; r < 80; ++r)
        for (int c = 0; c < 120; ++c) {
            double z = c < 80 ? -(80 - c) * 33.0 : 30.0 + 400.0 * std::exp(-((c - 100) * (c - 100) + (r - 40) * (r - 40)) / 120.0);
            if (isola && c == 30 && r == 40) z = 15.0;   // un'isola di una cella in mezzo al mare
            val[static_cast<std::size_t>(r * 120 + c)] = static_cast<std::uint16_t>(std::lround((z + 11000.0) / 0.25));
        }
    return Fondale(120, 80, 500.0, 0.25, -11000.0, val);
}

int conta(const Tela& t, int da, int a, Rettangolo r) {
    int n = 0;
    for (int y = r.y; y < r.y + r.h; ++y)
        for (int x = r.x; x < r.x + r.w; ++x) n += t.at(x, y) >= da && t.at(x, y) <= a;
    return n;
}

}  // namespace

TEST_CASE("carta VGA: terra nel rilievo 48-79, acqua nelle fasce, costa e isobate, niente fuori dall'area") {
    using namespace colori_carta_vga;
    const Fondale f = fondale_prova();
    GeometriaCarta g;
    g.x = 0; g.y = 0; g.w = 640; g.h = 360;
    g.centro = f.centro_cella(60, 40);
    g.metri_per_pixel = 100.0;
    Tela t;
    t.riempi(7);
    FondaleVga fv;
    fv.disegna(t, g, f, {});
    CHECK(fv.ricomposto());
    for (int y = 360; y < ALTEZZA_TELA; ++y)
        for (int x = 0; x < LARGHEZZA_TELA; ++x) REQUIRE(t.at(x, y) == 7);
    // la costa fra le colonne 79 e 80 (celle da 500 m): a circa 100 pixel a destra del centro
    const int xc = static_cast<int>(g.a_tela(f.centro_cella(80, 40)).x);
    CHECK(xc > 400);
    CHECK(xc < 440);
    const int xe = static_cast<int>(g.a_tela(f.centro_cella(119, 40)).x);   // il bordo est della griglia: oltre, niente dati
    CHECK(conta(t, RILIEVO, RILIEVO + 31, {xc + 4, 0, xe - xc - 8, 360}) > (xe - xc - 8) * 340);
    CHECK(conta(t, ACQUA, ACQUA + 47, {0, 0, xc - 20, 360}) > (xc - 20) * 300);
    CHECK(conta(t, COSTA, COSTA, {xc - 10, 0, 20, 360}) >= 300);   // una linea di costa lungo tutta l'altezza
    CHECK(conta(t, ISOBATA_PRINCIPALE, ISOBATA_PRINCIPALE, {0, 0, 320, 360}) >= 300);   // almeno un'isobata principale
    // il rilievo: la collina ha il versante a nord-ovest piu' chiaro di quello a sud-est
    const auto luce = [&](int x, int y) { return (t.at(x, y) - RILIEVO) % N_LUCI; };
    const Vec2 cima = g.a_tela(f.centro_cella(100, 40));
    double nw = 0, se = 0;
    for (int k = 4; k < 20; ++k) {
        nw += luce(static_cast<int>(cima.x) - k, static_cast<int>(cima.y) - k);
        se += luce(static_cast<int>(cima.x) + k, static_cast<int>(cima.y) + k);
    }
    CHECK(nw > se + 8);
    // senza rilievo, la terra tutta alla luce media
    Tela u;
    u.riempi(7);
    FondaleVga fv2;
    OpzioniCarta o;
    o.rilievo = false;
    fv2.disegna(u, g, f, o);
    int non_media = 0;
    for (int y = 0; y < 360; ++y)
        for (int x = xc + 4; x < xe - 4; ++x)
            if (u.at(x, y) >= RILIEVO && u.at(x, y) <= RILIEVO + 31) non_media += (u.at(x, y) - RILIEVO) % N_LUCI != 2;
    CHECK(non_media == 0);   // il piano sta tutto sulla luce media
}

TEST_CASE("carta VGA: la cache si trasla e si rifa' quando serve") {
    const Fondale f = fondale_prova();
    GeometriaCarta g;
    g.centro = f.centro_cella(60, 40);
    g.metri_per_pixel = 100.0;
    FondaleVga fv;
    Tela a;
    fv.disegna(a, g, f, {});
    CHECK(fv.ricomposto());
    // il centro 10 pixel a est: niente ricomposizione, la tela e' la stessa spostata di 10 pixel
    GeometriaCarta g2 = g;
    g2.centro.x += 10 * g.metri_per_pixel;
    Tela b;
    fv.disegna(b, g2, f, {});
    CHECK_FALSE(fv.ricomposto());
    bool uguale = true;
    for (int y = 0; y < 360; ++y)
        for (int x = 0; x < 600; ++x) uguale &= b.at(x, y) == a.at(x + 10, y);
    CHECK(uguale);
    // oltre il margine, cambio di scala o di stile: si ricompone
    GeometriaCarta g3 = g;
    g3.centro.x += (FondaleVga::MARGINE + 5) * g.metri_per_pixel;
    fv.disegna(b, g3, f, {});
    CHECK(fv.ricomposto());
    GeometriaCarta g4 = g;
    g4.metri_per_pixel = 150.0;
    fv.disegna(b, g4, f, {});
    CHECK(fv.ricomposto());
    OpzioniCarta naut;
    naut.stile = StileCarta::nautica;
    fv.disegna(b, g4, f, naut);
    CHECK(fv.ricomposto());
    fv.disegna(b, g4, f, naut);
    CHECK_FALSE(fv.ricomposto());
}

TEST_CASE("carta VGA: il contorno di sicurezza a quota ferma e' esatto, mentre la quota cambia non resta indietro oltre 10 m") {
    const Fondale f = fondale_prova();
    GeometriaCarta g;
    g.centro = f.centro_cella(60, 40);
    g.metri_per_pixel = 100.0;
    const auto con = [](double sicurezza_m, double tempo_s) {
        OpzioniCarta o;
        o.sicurezza_m = sicurezza_m;
        o.tempo_s = tempo_s;
        return o;
    };
    const auto esatta = [&](double sicurezza_m) {   // una cache nuova: il contorno giusto per quella quota
        FondaleVga nuova;
        Tela t;
        nuova.disegna(t, g, f, con(sicurezza_m, -1.0));
        return t;
    };
    const auto uguali = [](const Tela& a, const Tela& b) {
        return std::equal(a.dati(), a.dati() + LARGHEZZA_TELA * ALTEZZA_TELA, b.dati());
    };
    CHECK_FALSE(uguali(esatta(100.0), esatta(400.0)));   // il contorno si vede
    FondaleVga fv;
    Tela t;
    fv.disegna(t, g, f, con(100.0, 0.0));
    CHECK(fv.ricomposto());
    // la quota scende di 3 m a ogni passo da 1/9 s: si ricompone almeno ogni 10 m e mai a ogni passo
    double sicurezza = 100.0, tempo = 0.0, composta = 100.0;
    int ricomposizioni = 0;
    for (int passo = 0; passo < 90; ++passo) {
        sicurezza += 3.0, tempo += 1.0 / 9.0;
        for (int fotogramma = 0; fotogramma < 3; ++fotogramma) {   // piu' disegni per passo: il contorno e' fermo solo in apparenza
            fv.disegna(t, g, f, con(sicurezza, tempo));
            if (fv.ricomposto()) composta = sicurezza, ++ricomposizioni;
            CHECK(std::fabs(sicurezza - composta) < 10.0);
        }
    }
    CHECK(ricomposizioni == 22);   // 270 m a salti di 12
    // si ferma con il contorno indietro: un secondo dopo, esatto al byte; prima no
    sicurezza += 3.0, tempo += 1.0 / 9.0;
    fv.disegna(t, g, f, con(sicurezza, tempo));
    CHECK_FALSE(fv.ricomposto());
    fv.disegna(t, g, f, con(sicurezza, tempo + 0.9));
    CHECK_FALSE(fv.ricomposto());
    fv.disegna(t, g, f, con(sicurezza, tempo + 1.1));
    CHECK(fv.ricomposto());
    CHECK(uguali(t, esatta(sicurezza)));
    fv.disegna(t, g, f, con(sicurezza, tempo + 5.0));   // poi resta com'e'
    CHECK_FALSE(fv.ricomposto());
    // senza il tempo della missione: sempre esatto
    fv.disegna(t, g, f, con(sicurezza + 1.0, -1.0));
    CHECK(fv.ricomposto());
    CHECK(uguali(t, esatta(sicurezza + 1.0)));
}

TEST_CASE("carta VGA: l'isola piu' piccola di un pixel resta visibile") {
    const Fondale f = fondale_prova(true);
    GeometriaCarta g;
    g.centro = f.centro_cella(30, 40);
    g.metri_per_pixel = 900.0;   // quasi due celle per pixel
    FondaleVga fv;
    Tela t;
    t.riempi(7);
    fv.disegna(t, g, f, {});
    const int terra = conta(t, colori_carta_vga::RILIEVO, colori_carta_vga::RILIEVO + 31, {300, 160, 40, 40}) +
                      conta(t, colori_carta_vga::COSTA, colori_carta_vga::COSTA, {300, 160, 40, 40});
    CHECK(terra >= 1);
}

TEST_CASE("carta VGA: palette del rilievo e testo con l'alone") {
    const Palette base = Palette::carica(std::string(M688_DATI) + "/palette/giorno.pal");
    Palette p = base;
    applica_palette_rilievo(p, StileCarta::vga);
    for (int i = 0; i < 256; ++i)
        if (i < 48 || i > 79) REQUIRE(p.rgb(i) == base.rgb(i));
    // per tinta: dall'ombra alla luce sempre piu' chiaro; la carta nautica piu' chiara della VGA
    const auto lum = [](std::array<std::uint8_t, 3> c) { return 0.30 * c[0] + 0.59 * c[1] + 0.11 * c[2]; };
    for (int k = 0; k < 8; ++k)
        for (int l = 1; l < 4; ++l) CHECK(lum(p.rgb(48 + k * 4 + l)) > lum(p.rgb(48 + k * 4 + l - 1)));
    Palette q = base;
    applica_palette_rilievo(q, StileCarta::nautica);
    CHECK(lum(q.rgb(48 + 2)) > lum(p.rgb(48 + 2)));
    if (!fs::exists(std::string(M688_DATI) + "/font/incisione688_5.fnt")) return;
    const Font f = Font::carica(std::string(M688_DATI) + "/font/incisione688_5.fnt");
    Tela t;
    t.riempi(0);
    testo_con_alone(t, 20, 20, "200", f, 9, 5);
    int col = 0, alone = 0;
    for (int y = 10; y < 40; ++y)
        for (int x = 10; x < 60; ++x) {
            col += t.at(x, y) == 9;
            alone += t.at(x, y) == 5;
        }
    CHECK(col > 10);
    CHECK(alone > col);
}

TEST_CASE("carta VGA: mobilia (cornice coi minuti, rosa, scala, cartiglio) e righe riservate") {
    if (!fs::exists(std::string(M688_DATI) + "/font/incisione688_5.fnt")) return;
    using namespace colori_carta_vga;
    const Font font = Font::carica(std::string(M688_DATI) + "/font/incisione688_5.fnt");
    Fondale f = fondale_prova();
    f.imposta_centro({62.0, -7.0});
    GeometriaCarta g;
    g.centro = f.centro_cella(60, 40);
    g.metri_per_pixel = 100.0;
    // la cornice: le righe riservate restano libere, le bande hanno barrette bianche e nere, niente fuori dall'area
    Tela t;
    t.riempi(3);
    cornice_minuti(t, g, f, font, 30, 12);
    for (int x = 0; x < 640; ++x) {
        for (int y = 0; y < 30; ++y) REQUIRE(t.at(x, y) == 3);
        for (int y = 348; y < ALTEZZA_TELA; ++y) REQUIRE(t.at(x, y) == 3);
    }
    int nere = 0, bianche = 0;
    for (int y = 40; y < 330; ++y) {
        nere += t.at(2, y) == MATITA;
        bianche += t.at(2, y) == MATITA + 5;
    }
    CHECK(nere > 20);
    CHECK(bianche > 20);
    // la longitudine a 62 gradi: le barrette in alto non sono piu' corte di 8 pixel
    int corsa = 0, piu_corta = 1000;
    bool prima = true;   // il primo tratto e' tagliato dal punto di partenza: non conta
    for (int x = 8; x < 632; ++x) {
        if (t.at(x, 32) == t.at(x - 1, 32)) ++corsa;
        else {
            if (!prima) piu_corta = std::min(piu_corta, corsa + 1);
            prima = false;
            corsa = 0;
        }
    }
    CHECK(piu_corta >= 7);
    // la rosa: magenta, nel suo quadrato; la scala: tratti bianchi e neri lunghi al piu' 140 pixel; il cartiglio
    Tela r;
    r.riempi(3);
    rosa_dei_venti(r, 200, 200, 40, -9.0, 1990, false, font);
    CHECK(conta(r, MAGENTA, MAGENTA, {150, 150, 101, 101}) > 150);
    CHECK(conta(r, MAGENTA, MAGENTA, {0, 0, 640, 480}) == conta(r, MAGENTA, MAGENTA, {150, 150, 101, 101}));
    Tela s;
    s.riempi(3);
    const int w = scala_grafica(s, 20, 100, 100.0, false, font);
    CHECK(w <= 142);
    CHECK(w > 60);
    CHECK(conta(s, MATITA + 5, MATITA + 5, {20, 100, w, 4}) > 50);
    Tela c;
    c.riempi(3);
    MobiliaCarta m;
    m.titolo = "FAROE";
    m.edizione = 1988;
    const Rettangolo rc = cartiglio(c, 20, 200, m, font);
    CHECK(rc.y + rc.h == 200);
    CHECK(conta(c, MATITA, MATITA, rc) > 100);
    // la declinazione dei teatri (STIMA 1990): occidentale nelle Faroe, orientale nel Barents
    CHECK(declinazione_stima("faroe") < 0.0);
    CHECK(declinazione_stima("barents") > 0.0);
    CHECK(declinazione_stima("nessuno") == 0.0);
    // tutto insieme: con la cornice le righe riservate sono il margine bianco del foglio, senza altro sopra
    Tela tt;
    tt.riempi(3);
    MobiliaCarta tutto;
    tutto.cornice = tutto.rosa = tutto.scala = tutto.cartiglio = true;
    tutto.riserva_alto = 30;
    tutto.riserva_basso = 12;
    disegna_mobilia(tt, g, f, font, tutto);
    for (int x = 0; x < 640; ++x) {
        for (int y = 0; y < 30; ++y) REQUIRE(tt.at(x, y) == MATITA + 5);
        for (int y = 360 - 12; y < 360; ++y) REQUIRE(tt.at(x, y) == MATITA + 5);
    }
    // senza cornice le righe riservate restano libere
    Tela tl;
    tl.riempi(3);
    tutto.cornice = false;
    disegna_mobilia(tl, g, f, font, tutto);
    for (int x = 0; x < 640; ++x)
        for (int y = 0; y < 30; ++y) REQUIRE(tl.at(x, y) == 3);
}

TEST_CASE("carta VGA: scandagli in acqua, sulla carta d'epoca solo nelle zone rilevate") {
    if (!fs::exists(std::string(M688_DATI) + "/font/incisione688_5.fnt")) return;
    const Font font = Font::carica(std::string(M688_DATI) + "/font/incisione688_5.fnt");
    GeometriaCarta g;
    Fondale f = fondale_prova();
    g.centro = f.centro_cella(60, 40);
    g.metri_per_pixel = 100.0;
    OpzioniCarta o;
    o.font_isobate = &font;
    const auto disegna = [&](const Fondale& fo, bool scandagli) {
        Tela t;
        t.riempi(3);
        FondaleVga fv;
        OpzioniCarta oo = o;
        oo.scandagli = scandagli;
        fv.disegna(t, g, fo, oo);
        return conta(t, colori_carta_vga::ISOBATA, colori_carta_vga::ISOBATA, {0, 0, 400, 360});
    };
    const int senza = disegna(f, false), con = disegna(f, true);
    CHECK(con > senza + 200);   // le cifre degli scandagli nel mare a ovest
    // carta d'epoca senza zone rilevate: nessuno scandaglio (solo il tratteggio delle zone non rilevate)
    Fondale e = fondale_prova();
    e.imposta_carta(1988, {Campagna{1960, "prova", 1000, 2000, 0}}, std::vector<std::uint8_t>(120 * 80, 0));
    CHECK(disegna(e, true) == disegna(e, false));
}

TEST_CASE("carta VGA: toponimi dei teatri, a scala, senza sovrapposizioni ne' fuori dall'area") {
    using namespace colori_carta_vga;
    const std::string dati = M688_DATI;
    // i file dei sette teatri (tools/pipeline/toponimi.py): nomi, ranghi, le basi della Flotta del Nord
    for (const char* teatro : {"baltico", "barents", "celtica", "faroe", "gibilterra", "kunashir", "otranto"}) {
        const auto nomi = carica_toponimi(file_toponimi(dati, teatro));
        CAPTURE(teatro);
        CHECK(nomi.size() >= 5);
        for (const auto& n : nomi) {
            CHECK(!n.testo.empty());
            CHECK(n.rango >= 1);
            CHECK(n.rango <= 10);
        }
    }
    const auto kola = carica_toponimi(file_toponimi(dati, "barents"));
    CHECK(std::any_of(kola.begin(), kola.end(), [](const Toponimo& n) { return n.testo == "Severomorsk" && n.tipo == "base"; }));
    CHECK(carica_toponimi(file_toponimi(dati, "nessuno")).empty());
    // piu' si ingrandisce, piu' nomi
    CHECK(rango_massimo_toponimi(600.0) < rango_massimo_toponimi(100.0));
    CHECK(rango_massimo_toponimi(10.0) == 10);

    if (!fs::exists(dati + "/font/incisione688_5.fnt")) return;
    const Font font = Font::carica(dati + "/font/incisione688_5.fnt");
    GeometriaCarta g;
    g.x = 0, g.y = 0, g.w = 640, g.h = 360;
    g.metri_per_pixel = 100.0;
    const auto nome = [](std::string testo, std::string tipo, bool acqua, int rango, Vec2 pos) {
        Toponimo n;
        n.testo = std::move(testo), n.en = n.testo, n.tipo = std::move(tipo), n.acqua = acqua, n.rango = rango, n.pos = pos;
        return n;
    };
    const auto nuova = [] { Tela t; t.riempi(3); return t; };
    // un nome d'acqua in blu, uno di terra in nero col segno della citta'; le lettere accentate che il font non ha
    Tela t = nuova();
    const Rettangolo area{40, 40, 560, 280};
    int n = disegna_toponimi(t, g, {nome("Mare di Prova", "mare", true, 2, {0, 0}), nome("Gadžievo", "citta", false, 4, {0, -8000})},
                             font, {area, {}});
    CHECK(n == 2);
    CHECK(conta(t, MATITA + 2, MATITA + 2, {0, 0, 640, 360}) > 20);
    CHECK(conta(t, MATITA, MATITA, {0, 0, 640, 360}) > 20);
    // niente fuori dall'area
    for (int y = 0; y < 360; ++y)
        for (int x = 0; x < 640; ++x)
            if (x < area.x || x >= area.x + area.w || y < area.y || y >= area.y + area.h) REQUIRE(t.at(x, y) == 3);
    // due nomi nello stesso punto: il secondo (di rango minore) non si scrive
    t = nuova();
    n = disegna_toponimi(t, g, {nome("Secondo", "isola", false, 5, {0, 0}), nome("Primo", "isola", false, 3, {0, 0})}, font, {});
    CHECK(n == 1);
    // un nome sopra un pezzo della mobilia non si scrive, ne' uno troppo piccolo per la scala
    t = nuova();
    n = disegna_toponimi(t, g, {nome("Coperto", "isola", false, 3, {0, 0})}, font, {{}, {{300, 160, 40, 40}}});
    CHECK(n == 0);
    g.metri_per_pixel = 600.0;
    n = disegna_toponimi(t, g, {nome("Dettaglio", "capo", false, 9, {0, 0})}, font, {});
    CHECK(n == 0);
    CHECK(conta(t, MATITA, MATITA + 5, {0, 0, 640, 360}) == 0);
}
