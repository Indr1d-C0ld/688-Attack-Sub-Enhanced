#include <doctest.h>

#include <algorithm>
#include <cmath>
#include <string>

#include "grafica/font.hpp"
#include "grafica/palette.hpp"
#include "grafica/tela.hpp"
#include "nucleo/rilevatore_righe.hpp"
#include "righe_automatiche.hpp"

using namespace m688;

// Le righe trovate in automatico sul LOFAR (app/righe_automatiche.hpp; docs/23 §3, H): le marche e la lista delle righe
// confermate da un RilevatoreRighe vero, la scelta con su e giu', la riga persa.

namespace {

// una riga del LOFAR 0-200 Hz a 0,25 Hz con un po' di grana ferma e le righe date (Hz, dB)
Spettro lofar(const std::vector<std::pair<double, float>>& righe, int k) {
    Spettro s;
    s.f0_hz = 0.0;
    s.df_hz = 0.25;
    s.snr_db.assign(800, 0.0f);
    for (std::size_t i = 0; i < s.snr_db.size(); ++i) s.snr_db[i] = 0.6f * static_cast<float>(((i * 7919 + static_cast<std::size_t>(k) * 104729) % 97) / 97.0);
    for (const auto& [hz, db] : righe) s.snr_db[static_cast<std::size_t>(hz / s.df_hz)] += db;
    return s;
}

// due minuti di righe ogni 5 s: una ferma a 46,2 Hz e una a 120 Hz che sale di 0,5 Hz al minuto; la terza, a 80 Hz, solo
// dopo `terza_da` secondi
void nutri(RilevatoreRighe& ril, double da, double a, double terza_da = 1e9, bool con_ferma = true) {
    for (double t = da; t < a; t += 5.0) {
        std::vector<std::pair<double, float>> r = {{120.0 + 0.5 * t / 60.0, 10.0f}};
        if (con_ferma) r.push_back({46.2, 9.0f});
        if (t >= terza_da) r.push_back({80.0, 9.0f});
        ril.aggiungi(lofar(r, static_cast<int>(t)), t);
    }
}

const Rettangolo LOFAR{220, 60, 400, 150};

}  // namespace

TEST_CASE("righe automatiche: la lista in ordine, la scelta che segue la riga, la riga persa") {
    RilevatoreRighe ril;
    nutri(ril, 0.0, 120.0);
    const auto v = VistaRigheAutomatiche::in_ordine(ril.righe());
    REQUIRE(v.size() == 2);
    CHECK(v[0].hz == doctest::Approx(46.2).epsilon(0.01));
    CHECK(v[1].hz > 120.5);
    CHECK(v[1].deriva_hz_min > 0.2);
    const std::string s = VistaRigheAutomatiche::riga(v[0], 120.0);
    MESSAGE(s);
    CHECK(s.rfind("R" + std::to_string(v[0].id) + "  46,", 0) == 0);
    CHECK(s.find("HZ/MIN") != std::string::npos);

    VistaRigheAutomatiche vista;
    CHECK_FALSE(vista.riga_scelta(ril.righe()).has_value());
    vista.su_giu(ril.righe(), +1);
    CHECK(vista.scelta() == v[0].id);
    vista.su_giu(ril.righe(), +1);
    CHECK(vista.scelta() == v[1].id);
    vista.su_giu(ril.righe(), +1);   // in fondo resta li'
    CHECK(vista.scelta() == v[1].id);
    // compare una terza riga a 80 Hz, in mezzo: la scelta resta sulla stessa riga
    nutri(ril, 120.0, 180.0, 120.0);
    REQUIRE(VistaRigheAutomatiche::in_ordine(ril.righe()).size() == 3);
    CHECK(vista.scelta() == v[1].id);
    REQUIRE(vista.riga_scelta(ril.righe()).has_value());
    CHECK(vista.riga_scelta(ril.righe())->hz > 120.0);
    // la riga a 46,2 Hz sparisce: oltre perdita_s non c'e' piu', e una scelta su di lei si perde
    vista.scegli(v[0].id);
    nutri(ril, 180.0, 300.0, 120.0, false);
    CHECK_FALSE(vista.riga_scelta(ril.righe()).has_value());
}

TEST_CASE("righe automatiche: le marche sul LOFAR, la scelta in bianco, la lista e le aree") {
    RilevatoreRighe ril;
    nutri(ril, 0.0, 120.0);
    const auto v = VistaRigheAutomatiche::in_ordine(ril.righe());
    REQUIRE(v.size() == 2);
    VistaRigheAutomatiche vista;
    vista.scegli(v[1].id);
    const Font f = Font::carica(M688_DATI "/font/originale.fnt");
    Tela t;
    std::vector<AreaAttiva> aree;
    StileRighe s{0.0, 150.0};   // la finestra fino a 150 Hz: tutte e due dentro
    vista.disegna_marche(t, f, LOFAR, ril.righe(), s, &aree);
    CHECK(t.at(VistaRigheAutomatiche::x_di(LOFAR, s, v[0].hz), LOFAR.y) == colori::VERDE_CHIARO);
    CHECK(t.at(VistaRigheAutomatiche::x_di(LOFAR, s, v[1].hz), LOFAR.y) == colori::BIANCO);
    CHECK(std::count_if(aree.begin(), aree.end(), [](const AreaAttiva& a) { return a.comando.rfind("riga_auto_marca_", 0) == 0; }) == 2);
    // con la finestra 0-100 Hz la riga a 120 Hz e' una freccetta sul bordo destro
    Tela t2;
    const StileRighe stretta{0.0, 100.0};
    vista.disegna_marche(t2, f, LOFAR, ril.righe(), stretta);
    CHECK(t2.at(LOFAR.x + LOFAR.w - 2, LOFAR.y + 2) == colori::BIANCO);
    // la lista: il titolo, due righe, la scelta evidenziata
    const Rettangolo r{LOFAR.x + 2, LOFAR.y + 2, 230, 60};
    std::vector<AreaAttiva> aree2;
    vista.disegna_lista(t, f, r, ril.righe(), 120.0, &aree2);
    CHECK(std::count_if(aree2.begin(), aree2.end(), [](const AreaAttiva& a) { return a.comando.rfind("riga_auto_", 0) == 0; }) == 2);
    bool bianco = false;
    for (int x = r.x; x < r.x + r.w; ++x)
        for (int y = r.y + 14 + VistaRigheAutomatiche::RIGA_PX; y < r.y + 14 + 2 * VistaRigheAutomatiche::RIGA_PX; ++y) bianco = bianco || t.at(x, y) == colori::BIANCO;
    CHECK(bianco);   // la seconda riga della lista e' la scelta
}

TEST_CASE("righe automatiche: le etichette fitte si impilano con la linea a puntini, oltre quattro righe un +n") {
    const Font f = Font::carica(M688_DATI "/font/originale.fnt");
    const StileRighe s{0.0, 400.0};   // un pixel per hertz
    const auto riga = [](int id, double hz) {
        RigaAutomatica r;
        r.id = id;
        r.hz = hz;
        r.snr_db = 10.0;
        r.confermata = true;
        return r;
    };
    // le due del gioco: «36 38» e «8 7 9 10», a pochi hertz l'una dall'altra; una lontana resta sulla prima riga
    std::vector<RigaAutomatica> righe = {riga(36, 160.0), riga(38, 163.0), riga(8, 40.0), riga(7, 41.0), riga(9, 42.0), riga(10, 43.0), riga(60, 300.0)};
    VistaRigheAutomatiche vista;
    Tela t;
    std::vector<AreaAttiva> aree;
    vista.disegna_marche(t, f, LOFAR, righe, s, &aree);
    const auto y_area = [&](int id) {
        for (const auto& a : aree)
            if (a.comando == "riga_auto_marca_" + std::to_string(id)) return a.y;
        return -1;
    };
    CHECK(y_area(8) == LOFAR.y);
    CHECK(y_area(7) == LOFAR.y + VistaRigheAutomatiche::ETICHETTA_PASSO_PX);
    CHECK(y_area(9) == LOFAR.y + 2 * VistaRigheAutomatiche::ETICHETTA_PASSO_PX);
    CHECK(y_area(10) == LOFAR.y + 3 * VistaRigheAutomatiche::ETICHETTA_PASSO_PX);
    CHECK(y_area(36) == LOFAR.y);
    CHECK(y_area(38) == LOFAR.y + VistaRigheAutomatiche::ETICHETTA_PASSO_PX);
    CHECK(y_area(60) == LOFAR.y);
    // la linea a puntini sotto il triangolino della 10, fino alla sua riga: un pixel si', uno no
    const int x10 = VistaRigheAutomatiche::x_di(LOFAR, s, 43.0);
    CHECK(t.at(x10, LOFAR.y + 5) == colori::VERDE_CHIARO);
    CHECK(t.at(x10, LOFAR.y + 6) != colori::VERDE_CHIARO);
    // le etichette non si toccano: sulla stessa riga il testo dell'una (da x+5 a fine area-3) finisce prima dell'altro
    for (const auto& a : aree)
        for (const auto& b : aree)
            if (&a != &b && a.y == b.y && a.w > 8 && b.w > 8) CHECK((a.x + a.w <= b.x + 9 || b.x + b.w <= a.x + 9));
    // una quinta a 44 Hz non trova riga: e' un «+1» sull'ultima della riga piu' bassa, e il suo triangolino resta cliccabile
    righe.push_back(riga(11, 44.0));
    std::vector<AreaAttiva> aree2;
    Tela t2;
    vista.disegna_marche(t2, f, LOFAR, righe, s, &aree2);
    CHECK(std::count_if(aree2.begin(), aree2.end(), [](const AreaAttiva& a) { return a.comando == "riga_auto_marca_11" && a.y == LOFAR.y; }) == 1);
    bool piu = false;   // il «+1» dopo il «10», sulla quarta riga
    const int y4 = LOFAR.y + 3 * VistaRigheAutomatiche::ETICHETTA_PASSO_PX;
    for (int x = x10 + 5 + f.larghezza("10", 1); x < x10 + 5 + f.larghezza("10 +1", 1); ++x)
        for (int y = y4; y < y4 + 8; ++y) piu = piu || t2.at(x, y) == colori::VERDE_CHIARO;
    CHECK(piu);
}
