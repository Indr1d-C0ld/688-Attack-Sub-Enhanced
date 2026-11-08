// La schermata dei comandi (grafica/schermata_comandi): il disegno, senza la partita.
#include <doctest.h>

#include <algorithm>
#include <filesystem>
#include <string>
#include <vector>

#include "grafica/font.hpp"
#include "grafica/schermata_comandi.hpp"
#include "grafica/tela.hpp"
#include "grafica/testo_font.hpp"
#include "grafica/palette.hpp"

using namespace m688;

namespace {
const bool CON_DATI = std::filesystem::exists(M688_DATI "/font/originale.fnt");

struct Font3 {
    Font f = Font::carica(M688_DATI "/font/originale.fnt"), piccolo = Font::carica(M688_DATI "/font/fosforo688_5x7.fnt"),
         targa = Font::carica(M688_DATI "/font/incisione688_7.fnt");
};
const Font3& font() {
    static const Font3 x;
    return x;
}

SchedaComandi scheda(const std::string& etichetta, int righe, int lungo = 40) {
    SchedaComandi s;
    s.etichetta = etichetta;
    GruppoComandi g;
    g.titolo = "Gruppo «uno»";
    for (int i = 0; i < righe; ++i) g.righe.push_back({"Ctrl+P, Maiusc+K", std::string(static_cast<std::size_t>(lungo), 'x') + " " + std::to_string(i)});
    s.gruppi.push_back(g);
    return s;
}

int disegna(Tela& t, const DatiSchermataComandi& d, std::vector<AreaComando>* a = nullptr) {
    return disegna_schermata_comandi(t, font().f, font().piccolo, font().targa, d, a);
}
}  // namespace

TEST_CASE("schermata comandi: una scheda corta sta in una pagina, una lunga si divide e nessuna riga si perde" * doctest::skip(!CON_DATI)) {
    Tela t;
    DatiSchermataComandi d;
    d.schede = {scheda("F3 STATO", 5), scheda("F6 NAVIGAZIONE", 30, 200)};
    CHECK(disegna(t, d) == 1);
    d.scheda = 1;
    const int pagine = disegna(t, d);
    CHECK(pagine >= 2);
    CHECK(pagine <= 8);
    // pagine diverse disegnano cose diverse; fuori misura si riporta all'ultima
    Tela a, b, ultima;
    d.pagina = 0;
    disegna(a, d);
    d.pagina = 1;
    disegna(b, d);
    bool diverse = false;
    for (int y = 60; y < 420 && !diverse; ++y)
        for (int x = 8; x < 632 && !diverse; ++x) diverse = a.at(x, y) != b.at(x, y);
    CHECK(diverse);
    d.pagina = pagine - 1;
    disegna(ultima, d);
    Tela oltre;
    d.pagina = pagine + 5;
    CHECK(disegna(oltre, d) == pagine);
    bool uguali = true;
    for (int y = 0; y < 480 && uguali; ++y)
        for (int x = 0; x < 640 && uguali; ++x) uguali = oltre.at(x, y) == ultima.at(x, y);
    CHECK(uguali);
}

TEST_CASE("schermata comandi: le aree per il clic: le linguette, chiudi, la scheda e la pagina dopo" * doctest::skip(!CON_DATI)) {
    Tela t;
    DatiSchermataComandi d;
    for (const char* e : {"F2 RADIO", "F3 STATO", "F4 MANOVRA", "F5 PERISCOPIO", "F6 NAVIGAZIONE", "F7 ARMI", "F8 SONAR", "F9 CENTRALE", "OVUNQUE"})
        d.schede.push_back(scheda(e, 30, 120));
    d.scheda = 8;
    std::vector<AreaComando> a;
    disegna(t, d, &a);
    int linguette = 0, chiudi = 0, dopo_scheda = -1, dopo_pagina = -1;
    for (const auto& x : a) {
        CHECK(x.r.x >= 0);
        CHECK(x.r.y >= 0);
        CHECK(x.r.x + x.r.w <= 640);
        CHECK(x.r.y + x.r.h <= 480);
        if (x.chiudi) ++chiudi;
        else if (x.pagina >= 0) dopo_pagina = x.pagina;
        else if (x.r.y > 400) dopo_scheda = x.scheda;
        else ++linguette;
    }
    CHECK(linguette == 9);
    CHECK(chiudi == 1);
    CHECK(dopo_scheda == 0);   // dopo l'ultima, in giro
    CHECK(dopo_pagina == 1);
    // le linguette non si sovrappongono
    for (std::size_t i = 0; i + 1 < 9; ++i) CHECK(a[i].r.x + a[i].r.w <= a[i + 1].r.x);
}

TEST_CASE("schermata comandi: senza schede o senza righe non cade; i consigli stanno nella prima pagina" * doctest::skip(!CON_DATI)) {
    Tela t;
    DatiSchermataComandi vuota;
    CHECK(disegna(t, vuota) == 1);
    vuota.schede.push_back({"OVUNQUE", {}, {}});
    CHECK(disegna(t, vuota) == 1);

    DatiSchermataComandi d;
    d.schede = {scheda("F5 PERISCOPIO", 3)};
    Tela senza, con;
    disegna(senza, d);
    d.schede[0].consigli = {"Prima di alzarlo: quota periscopica"};
    disegna(con, d);
    int rossi_senza = 0, rossi_con = 0;
    for (int y = 60; y < 110; ++y)
        for (int x = 8; x < 632; ++x) {
            rossi_senza += senza.at(x, y) == colori::ROSSO;
            rossi_con += con.at(x, y) == colori::ROSSO;
        }
    CHECK(rossi_con > rossi_senza);
}

TEST_CASE("testo_per_font: virgolette e accenti nel font del gioco") {
    CHECK(testo_per_font("«più» di") == "\"PIÙ\" DI");
    CHECK(testo_per_font("a–b") == "A-B");
}
