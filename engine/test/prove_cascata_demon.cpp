#include <doctest.h>

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <string>

#include "cascata_demon.hpp"
#include "grafica/font.hpp"
#include "grafica/palette.hpp"
#include "grafica/scala_cascata.hpp"
#include "grafica/tela.hpp"

using namespace m688;

// Il DEMON a cascata della stazione sonar (app/cascata_demon.hpp; docs/23 §2, G): la storia con le scale dei tempi, i
// cursori armonici a mano e agganciati alle serie trovate, la lettura per l'analizzatore, il disegno nel rettangolo.

namespace {

// uno Spettro DEMON sintetico 0-100 Hz a 0,1 Hz: due serie nello stesso fascio, (asse 2 Hz, 7 pale) e (asse 3 Hz, 5 pale)
// `giri`: il fattore dei giri (le righe si spostano tutte nella stessa proporzione)
Spettro due_serie(double giri = 1.0) {
    Spettro s;
    s.f0_hz = 0.0;
    s.df_hz = 0.1;
    s.snr_db.assign(1000, 0.0f);
    const auto riga = [&](double hz, float db) { s.snr_db[static_cast<std::size_t>(std::floor(hz * giri / s.df_hz))] = db; };
    riga(2.0, 9.0f);   // la prima: asse, il doppio, la pala e la sua armonica
    riga(4.0, 7.0f);
    riga(14.0, 18.0f);
    riga(28.0, 10.0f);
    riga(3.0, 8.0f);   // la seconda, piu' debole
    riga(15.0, 14.0f);
    riga(30.0, 8.0f);
    return s;
}

const Rettangolo R{324, 270, 316, 90};

bool con_colore_colonna(const Tela& t, int x, int y0, int y1, std::uint8_t c) {
    for (int dx = -1; dx <= 1; ++dx)
        for (int y = y0; y < y1; ++y)
            if (t.at(x + dx, y) == c) return true;
    return false;
}

}  // namespace

TEST_CASE("cascata demon: la storia per scala dei tempi, col massimo delle righe lente") {
    CascataDemon d;
    for (int k = 0; k < 16; ++k) {
        Spettro s = due_serie();
        if (k == 5) s.snr_db[500] = 25.0f;   // un transitorio in una riga sola
        d.aggiungi(s);
    }
    CHECK(d.righe(1).size() == 16);
    CHECK(d.righe(4).size() == 4);
    CHECK(d.righe(16).size() == 1);
    CHECK(d.righe(16).front().snr_db[500] == 25.0f);   // la scala lenta lo tiene
    CHECK(d.righe(4)[2].snr_db[500] == 25.0f);          // la riga x4 delle righe 4-7 (la piu' recente per prima)
    for (int k = 0; k < CascataDemon::RIGHE_MAX + 20; ++k) d.aggiungi(due_serie());
    CHECK(static_cast<int>(d.righe(1).size()) == CascataDemon::RIGHE_MAX);
    d.svuota();
    CHECK(d.righe(1).empty());
    CHECK(d.righe(4).empty());
}

TEST_CASE("cascata demon: i cursori a mano, l'aggancio alle due serie e la lettura") {
    CascataDemon d;
    CHECK_FALSE(d.lettura().has_value());
    CHECK_FALSE(d.aggancia());   // senza righe nessuna serie
    d.aggiungi(due_serie());
    const auto v = d.serie();
    REQUIRE(v.size() == 2);
    CHECK(v[0].asse_hz == doctest::Approx(2.05).epsilon(0.05));
    CHECK(v[0].pale == 7);
    CHECK(v[1].pale == 5);
    // a mano: il primo tocco mette l'asse a 1 Hz, poi passi fini e grossi; le pale fra 3 e 9
    d.sposta_asse(+1, true);
    CHECK(d.asse_hz() == doctest::Approx(1.0));
    d.sposta_asse(+1, true);
    CHECK(d.asse_hz() == doctest::Approx(1.1));
    d.sposta_asse(+1, false);
    CHECK(d.asse_hz() == doctest::Approx(2.1));
    d.sposta_asse(-1, true);
    CHECK(d.asse_hz() == doctest::Approx(2.0));
    for (int i = 0; i < 10; ++i) d.pale(+1);
    CHECK(d.pale_n() == CascataDemon::PALE_MAX);
    for (int i = 0; i < 10; ++i) d.pale(-1);
    CHECK(d.pale_n() == CascataDemon::PALE_MIN);
    CHECK_FALSE(d.agganciato());
    // F: la prima serie, poi l'altra, poi di nuovo la prima
    REQUIRE(d.aggancia());
    CHECK(d.agganciato());
    CHECK(d.pale_n() == 7);
    CHECK(d.asse_hz() == doctest::Approx(v[0].pala_hz / 7));   // l'asse dalla pala, piu' precisa
    CHECK(d.asse_hz() == doctest::Approx(2.0).epsilon(0.01));
    REQUIRE(d.aggancia());
    CHECK(d.pale_n() == 5);
    REQUIRE(d.aggancia());
    CHECK(d.pale_n() == 7);
    const auto l = d.lettura();
    REQUIRE(l.has_value());
    CHECK(l->pala_hz == doctest::Approx(l->asse_hz * 7));
    // un tocco a mano stacca l'aggancio
    d.pale(+1);
    CHECK_FALSE(d.agganciato());
}

TEST_CASE("cascata demon: le righe sulla cascata, i cursori alle armoniche, la lettura e le aree") {
    CascataDemon d;
    for (int k = 0; k < 60; ++k) d.aggiungi(due_serie(k < 30 ? 1.0 : 1.04));   // i giri salgono del 4%: le righe si spostano
    REQUIRE(d.aggancia());
    CHECK(d.pale_n() == 7);   // sulle righe nuove: il rapporto resta 7, e il cursore della pala sta sulla riga
    CHECK(d.asse_hz() * 7 == doctest::Approx(14.0 * 1.04).epsilon(0.01));
    const Font f = Font::carica(M688_DATI "/font/originale.fnt");
    Tela t;
    std::vector<AreaAttiva> aree;
    StileDemon s;
    d.disegna(t, f, R, s, &aree);
    const int y_casc = R.y + CascataDemon::ALTO_ASCAN + 1, y_fine = R.y + R.h - CascataDemon::ALTO_TESTO;
    // la pala a 14,6 Hz delle righe nuove accesa nella cascata in alto, quella a 14 Hz delle vecchie piu' in basso
    const int x_nuova = d.x_di(R, 14.0 * 1.04 + 0.05), x_vecchia = d.x_di(R, 14.05);
    bool nuova = false, vecchia = false;
    for (int dx = -1; dx <= 1; ++dx) {
        nuova = nuova || t.at(x_nuova + dx, y_casc + 2) != 0;
        vecchia = vecchia || t.at(x_vecchia + dx, y_casc + 45) != 0;
    }
    CHECK(nuova);
    CHECK(vecchia);
    // i cursori: l'asse giallo e la pala ciano
    CHECK(con_colore_colonna(t, d.x_di(R, d.asse_hz()), y_casc, y_fine, colori::GIALLO));
    CHECK(con_colore_colonna(t, d.x_di(R, d.asse_hz() * d.pale_n()), y_casc, y_fine, colori::CIANO_CHIARO));
    CHECK(con_colore_colonna(t, d.x_di(R, 2 * d.asse_hz() * d.pale_n()), y_casc, y_fine, colori::CIANO_CHIARO));
    // la lettura in giallo nella riga in basso
    bool lettura = false;
    for (int y = y_fine; y < R.y + R.h; ++y)
        for (int x = R.x; x < R.x + 200; ++x) lettura = lettura || t.at(x, y) == colori::GIALLO;
    CHECK(lettura);
    // niente fuori dal rettangolo
    for (int x = 0; x < LARGHEZZA_TELA; ++x) CHECK(t.at(x, R.y - 1) == 0);
    // le aree e il clic: la frequenza alla x torna
    CHECK(std::count_if(aree.begin(), aree.end(), [](const AreaAttiva& a) { return a.comando == "demon_asse"; }) == 1);
    CHECK(std::count_if(aree.begin(), aree.end(), [](const AreaAttiva& a) { return a.comando == "demon_serie"; }) == 1);
    CHECK(d.frequenza_a(R, d.x_di(R, 14.0)) == doctest::Approx(14.0).epsilon(0.03));
    // il modo 1989 col fosforo: si disegna, senza le tacche delle serie
    StileDemon vecchio;
    vecchio.modo_1989 = true;
    vecchio.fosforo = true;
    Tela t2;
    d.disegna(t2, f, R, vecchio);
    CHECK(t2.at(d.x_di(R, d.serie()[1].asse_hz), R.y) != colori::GRIGIO);
    if (const char* dir = std::getenv("M688_ISTANTANEE")) {
        Palette pal = Palette::carica(M688_DATI "/palette/giorno.pal");
        cascate::applica(pal, cascate::Tinta::turbo);
        t.salva_bmp(std::string(dir) + "/cascata_demon.bmp", pal);
    }
}

TEST_CASE("cascata demon: le scritte dello stile, DEMON e HZ di base, DEMOD e GC sull'Okean dell'Alfa") {
    CascataDemon d;
    for (int k = 0; k < 10; ++k) d.aggiungi(due_serie());
    const Font f = Font::carica(M688_DATI "/font/originale.fnt");
    const auto riga_testo = [&](const StileDemon& s) {   // i pixel della riga in basso, senza cursori: la scritta del titolo
        Tela t;
        d.disegna(t, f, R, s);
        std::vector<std::uint8_t> v;
        for (int y = R.y + R.h - CascataDemon::ALTO_TESTO; y < R.y + R.h; ++y)
            for (int x = R.x; x < R.x + R.w; ++x) v.push_back(t.at(x, y));
        return v;
    };
    StileDemon base, okean;
    okean.nome = "DEMOD";
    okean.unita = "GC";
    StileDemon uguale;
    uguale.nome = "DEMON";
    uguale.unita = "HZ";
    CHECK(riga_testo(base) == riga_testo(uguale));   // di base come prima
    CHECK(riga_testo(base) != riga_testo(okean));
}
