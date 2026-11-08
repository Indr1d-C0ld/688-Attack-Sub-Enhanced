#include <doctest.h>

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <string>

#include "grafica/font.hpp"
#include "grafica/palette.hpp"
#include "grafica/scala_cascata.hpp"
#include "grafica/tela.hpp"
#include "nucleo/storia_cascata.hpp"
#include "vista_storia.hpp"

using namespace m688;

// La storia lunga delle cascate (app/vista_storia.hpp; docs/23 §4, I): la cascata della finestra alla risoluzione scelta,
// il cursore del tempo che trascina la finestra, «quando e' comparsa» sul LOFAR e sul BTR in gradi, la diretta.

namespace {

// tre ore di LOFAR 0-200 Hz a 0,5 Hz, una riga al secondo: una riga a 50 Hz che compare a 5000 s, un transitorio di un
// secondo a 120 Hz a 3000 s
StoriaCascata tre_ore() {
    StoriaCascata s(0.0, 0.5, 400);
    std::vector<float> riga(400);
    for (int t = 0; t < 10800; ++t) {
        std::fill(riga.begin(), riga.end(), 0.0f);
        if (t >= 5000) riga[100] = 12.0f;   // 50 Hz
        if (t == 3000) riga[240] = 15.0f;   // 120 Hz
        s.aggiungi(static_cast<double>(t), riga);
    }
    return s;
}

const Rettangolo LOFAR{220, 60, 400, 150};

bool acceso_vicino(const Tela& t, int x, int y) {
    for (int dy = -1; dy <= 1; ++dy)
        for (int dx = -1; dx <= 1; ++dx)
            if (t.at(x + dx, y + dy) != 0) return true;
    return false;
}

}  // namespace

TEST_CASE("vista storia: la diretta, la risoluzione, il cursore che trascina la finestra") {
    const StoriaCascata s = tre_ore();
    const double ora = 10799.0;
    VistaStoria v;
    CHECK(v.in_diretta());
    const Font f = Font::carica(M688_DATI "/font/originale.fnt");
    Tela t;
    v.disegna(t, f, LOFAR, s, ora, {});
    // in diretta la riga a 50 Hz c'e' in tutta la cascata (l'ultima ora e mezza), il transitorio di 3000 s no
    const int x50 = LOFAR.x + static_cast<int>(50.25 / 200.0 * LOFAR.w);
    CHECK(acceso_vicino(t, x50, LOFAR.y + VistaStoria::ALTO_TESTO + 5));
    CHECK(acceso_vicino(t, x50, LOFAR.y + LOFAR.h - 5));
    // a 60 s per riga la finestra di 140 righe arriva a 8400 s indietro: il transitorio a 120 Hz resta, col massimo
    v.risoluzione(+1);
    v.risoluzione(+1);
    CHECK(v.livello() == 2);
    Tela t2;
    v.disegna(t2, f, LOFAR, s, ora, {});
    const int x120 = LOFAR.x + static_cast<int>(120.25 / 200.0 * LOFAR.w);
    CHECK(acceso_vicino(t2, x120, v.y_di(LOFAR, s, 3000.0, ora)));
    CHECK(v.y_di(LOFAR, s, 5000.0, ora) > LOFAR.y + VistaStoria::ALTO_TESTO);   // la comparsa della riga dentro la finestra
    // il cursore: giu' di dieci righe alla volta; quando esce dal fondo la finestra scorre con lui
    for (int i = 0; i < 20; ++i) v.cursore(+1, true);
    CHECK_FALSE(v.in_diretta());
    CHECK(v.t_cursore(ora) == doctest::Approx(ora - 20 * 10 * 60.0));
    Tela t3;
    v.disegna(t3, f, LOFAR, s, ora, {});
    const int yc = v.y_di(LOFAR, s, v.t_cursore(ora), ora);
    CHECK(yc >= LOFAR.y + VistaStoria::ALTO_TESTO);
    CHECK(yc < LOFAR.y + LOFAR.h);
    bool giallo = false;
    for (int x = LOFAR.x + 2; x < LOFAR.x + 40; ++x) giallo = giallo || t3.at(x, yc) == colori::GIALLO;
    CHECK(giallo);
    // il clic: l'istante della riga torna
    CHECK(v.t_a(LOFAR, s, yc, ora) == doctest::Approx(v.t_cursore(ora)).epsilon(0.01));
    v.adesso();
    CHECK(v.in_diretta());
    CHECK(VistaStoria::relativo(4320.0) == "-1 H 12 MIN");
    CHECK(VistaStoria::relativo(270.0) == "-4 MIN 30 S");
}

TEST_CASE("vista storia: lo spettro normalizzato come il LOFAR dal vivo, senza buchi fra le righe della stazione") {
    // come la stazione: una riga ogni 2 s, l'SNR grezzo con un fondo di 20 dB e una riga a 50 Hz di 12 dB sopra
    StoriaCascata s(0.0, 0.5, 400);
    std::vector<float> riga(400, 20.0f);
    riga[100] = 32.0f;
    for (int t = 0; t <= 600; t += 2) s.aggiungi(static_cast<double>(t), riga);
    const double ora = 600.0;
    VistaStoria v;
    const Font f = Font::carica(M688_DATI "/font/originale.fnt");
    StileStoria st;
    st.pavimento_db = 2.0;
    st.tetto_db = 16.0;
    Tela t;
    v.disegna(t, f, LOFAR, s, ora, st);
    const int x50 = LOFAR.x + static_cast<int>(50.25 / 200.0 * LOFAR.w);
    const int x150 = LOFAR.x + static_cast<int>(150.25 / 200.0 * LOFAR.w);
    // il fondo resta spento (prima, grezzo sopra il tetto, era tutto saturo); la riga c'e', in ogni riga di pixel
    int spenti = 0, accesi = 0;
    for (int y = LOFAR.y + VistaStoria::ALTO_TESTO + 1; y < LOFAR.y + LOFAR.h - 1; ++y) {
        spenti += t.at(x150, y) == 0;
        accesi += t.at(x50, y) != 0;
    }
    const int righe = LOFAR.h - VistaStoria::ALTO_TESTO - 2;
    CHECK(spenti == righe);
    CHECK(accesi == righe);
    // il BTR arriva gia' normalizzato: lo stesso fondo a 20 dB, in gradi, si disegna com'e' (sopra il tetto)
    StoriaCascata b(0.0, 5.0, 72);
    std::vector<float> btr(72, 20.0f);
    for (int k = 0; k <= 600; k += 2) b.aggiungi(static_cast<double>(k), btr);
    st.gradi = true;
    Tela tb;
    VistaStoria().disegna(tb, f, LOFAR, b, ora, st);
    CHECK(tb.at(LOFAR.x + LOFAR.w / 2, LOFAR.y + LOFAR.h / 2) != 0);
}

TEST_CASE("vista storia: quando e' comparsa, sul LOFAR e sul BTR a cavallo di 0 gradi") {
    const StoriaCascata s = tre_ore();
    const double ora = 10799.0;
    VistaStoria v;
    CHECK_FALSE(v.cerca_comparsa(s, 120.0, ora).has_value());   // il transitorio adesso non c'e'
    const auto t = v.cerca_comparsa(s, 50.0, ora);
    REQUIRE(t.has_value());
    CHECK(*t == doctest::Approx(5000.0).epsilon(0.01));
    CHECK(v.livello() >= 1);   // 5800 s indietro: oltre i 10 minuti del livello fine
    CHECK(v.t_cursore(ora) == doctest::Approx(*t));
    const Font f = Font::carica(M688_DATI "/font/originale.fnt");
    Tela tt;
    std::vector<AreaAttiva> aree;
    v.disegna(tt, f, LOFAR, s, ora, {}, &aree);
    const int yc = v.y_di(LOFAR, s, *t, ora);
    CHECK(yc >= LOFAR.y + VistaStoria::ALTO_TESTO);
    CHECK(yc < LOFAR.y + LOFAR.h);
    CHECK(std::count_if(aree.begin(), aree.end(), [](const AreaAttiva& a) { return a.comando == "storia_tempo"; }) == 1);
    CHECK(std::count_if(aree.begin(), aree.end(), [](const AreaAttiva& a) { return a.comando == "storia_adesso"; }) == 1);
    // il BTR: 72 fasci da 5 gradi, il primo centrato su 0 (da -2,5); una traccia nel fascio di prua da 1000 s, cercata a
    // 359 gradi: fuori dalla storia (che arriva a 357,5), dentro a -1
    StoriaCascata btr(-2.5, 5.0, 72);
    std::vector<float> r(72);
    for (int k = 0; k < 2000; ++k) {
        std::fill(r.begin(), r.end(), 0.0f);
        if (k >= 1000) r[0] = 10.0f;
        btr.aggiungi(static_cast<double>(k), r);
    }
    VistaStoria b;
    const auto tb = b.cerca_comparsa(btr, 359.0, 1999.0, true);
    REQUIRE(tb.has_value());
    CHECK(*tb == doctest::Approx(1000.0).epsilon(0.02));
    if (const char* d = std::getenv("M688_ISTANTANEE")) {
        Palette pal = Palette::carica(M688_DATI "/palette/giorno.pal");
        cascate::applica(pal, cascate::Tinta::turbo);
        tt.salva_bmp(std::string(d) + "/vista_storia.bmp", pal);
    }
}
