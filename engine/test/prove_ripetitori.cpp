// Le stazioni del gruppo 5 (app/ripetitori.hpp, StazioneRadio::disegna_sala; bozze approvate il 29/10/2026): ogni
// pezzo ridisegna solo la sua meta' (la vista 0-360 o il pannello 360-480) e usa solo il tema delle stazioni (16-47)
// e gli EGA (0-15), cosi' la carta, il rilievo e la vista tengono i loro colori.
#include <doctest.h>

#include <filesystem>
#include <fstream>
#include <string>

#include "grafica/font.hpp"
#include "grafica/palette.hpp"
#include "grafica/periscopio.hpp"
#include "grafica/tela.hpp"
#include "missioni/danni_missione.hpp"
#include "missioni/missione.hpp"
#include "nucleo/classi.hpp"
#include "ripetitori.hpp"
#include "stazione_radio.hpp"

using namespace m688;
namespace fs = std::filesystem;

namespace {

bool ci_sono_font() { return fs::exists(std::string(M688_DATI) + "/font/fosforo688_5x7.fnt"); }

struct FontProva {
    Font orig = Font::carica(std::string(M688_DATI) + "/font/originale.fnt");
    Font incisa = Font::carica(std::string(M688_DATI) + "/font/incisione688_7.fnt");
    Font schermo = Font::carica(std::string(M688_DATI) + "/font/fosforo688_5x7.fnt");
    ripetitori::Fonti fonti() const { return {orig, incisa, schermo}; }
};

// la tela parte tutta a 255: le righe [da, a) devono essere coperte negli indici 0-47, le altre intatte
void controlla_meta(const Tela& t, int da, int a) {
    int fuori = 0, colori_fuori = 0, scoperti = 0;
    for (int y = 0; y < t.altezza(); ++y)
        for (int x = 0; x < t.larghezza(); ++x) {
            const int c = t.at(x, y);
            if (y < da || y >= a) fuori += c != 255;
            else if (c == 255) ++scoperti;
            else colori_fuori += c > 47;
        }
    CHECK(fuori == 0);
    CHECK(scoperti == 0);
    CHECK(colori_fuori == 0);
}

}  // namespace

TEST_CASE("stazioni del gruppo 5: i ripetitori di manovra e armi stanno nella vista, negli indici 0-47" * doctest::skip(!ci_sono_font())) {
    const FontProva f;
    SUBCASE("manovra") {
        Tela t(LARGHEZZA_TELA, ALTEZZA_TELA);
        t.riempi(255);
        ripetitori::DatiManovra d;
        for (int k = 0; k <= 60; ++k) d.profilo.quota_m.push_back(18.0 + k * 0.5), d.profilo.fondo_m.push_back(k % 3 ? 120.0 : 0.0);
        d.profilo.periscopica_m = 18.3, d.profilo.strato_m = 62.0;
        d.righe = {{"ROTTA", "000", "001"}, {"QUOTA", "-", "45 M"}, {"MACCHINA", "AV. 1/3", "GIRI 33"}};
        d.chiglia = "83 M, IN SALITA";
        d.cavitazione = "CAVITAZIONE A 7,5 KN";
        ripetitori::manovra(t, f.fonti(), d);
        controlla_meta(t, 0, ALTEZZA_VISTA);
    }
    SUBCASE("armi, con e senza armi in acqua") {
        for (const bool vuoto : {false, true}) {
            Tela t(LARGHEZZA_TELA, ALTEZZA_TELA);
            t.riempi(255);
            ripetitori::DatiArmiRipetitore d;
            if (!vuoto) {
                d.in_acqua = {{"1", "ADCAP", "4,2 KM", "9,8 KM", "ATTIVO", "AGGANCIO", "A"}};
                d.scelta_titolo = "L'ARMA SCELTA: 1 MK 48 ADCAP";
                d.scelta = {"RILEVAMENTO 012, DISTANZA STIMATA 3,1 KM"};
            }
            d.fili = "1 FILO SU 4";
            d.tubi = {{"1", "VUOTO", "-", true}, {"2", "MK 48 ADCAP", "PRONTO", false}};
            d.carico = {"MK 48 ADCAP  14"};
            d.ricarica = {"NESSUNA"};
            ripetitori::armi(t, f.fonti(), d);
            controlla_meta(t, 0, ALTEZZA_VISTA);
        }
    }
}

TEST_CASE("stazioni del gruppo 5: il pannello della carta sta sotto la vista, negli indici 0-47" * doctest::skip(!ci_sono_font())) {
    const FontProva f;
    Tela t(LARGHEZZA_TELA, ALTEZZA_TELA);
    t.riempi(255);
    ripetitori::DatiPannelloCarta d;
    d.titolo = "NAVIGAZIONE  TORPEX '89  1,5X";
    d.ora = "ORA 10:01:00Z  TEMPO X1";
    d.bersaglio = "A  CONTATTO DI SUPERFICIE SCONOSCIUTO";
    d.bersaglio_dati = "RIL. 012°   ROTTA 270°   DISTANZA 3,4 NM   VELOCITÀ 12 NODI   QUOTA 0 FT";
    d.comandi = {{"4", "RIGHELLO"}, {"5", "LINEE"}, {"T", "MATE"}};
    d.messaggio = "COMANDANTE, SONAR: NUOVO CONTATTO, RILEVAMENTO 293, DESIGNATO SIERRA 4.";
    ripetitori::pannello_carta(t, f.fonti(), d);
    controlla_meta(t, ALTEZZA_VISTA, ALTEZZA_TELA);
    // il Sozh dell'Alfa: le scritte scure sul beige, nessuna in ciano
    Tela ta(LARGHEZZA_TELA, ALTEZZA_TELA);
    ta.riempi(255);
    ripetitori::Fonti scure = f.fonti();
    scure.scritte = lamiera::Scritte::scure;
    d.titolo = "SOZH  SCROLLATEVELO DI DOSSO  1,5X";
    ripetitori::pannello_carta(ta, scure, d);
    controlla_meta(ta, ALTEZZA_VISTA, ALTEZZA_TELA);
    int ciano = 0;
    for (int x = 0; x < LARGHEZZA_TELA; ++x)
        for (int y = ALTEZZA_VISTA; y < ALTEZZA_VISTA + 18; ++y) ciano += ta.at(x, y) == colori::CIANO_CHIARO;
    CHECK(ciano == 0);
    // il testo lungo si accorcia a carattere intero, anche con le lettere accentate
    CHECK(ripetitori::largo(f.orig, ripetitori::taglia(f.orig, "VELOCITÀ VELOCITÀ VELOCITÀ", 60)) <= 60);
}

TEST_CASE("stazioni del gruppo 5: la sala radio sulla lamiera sta nella vista, negli indici 0-47" *
          doctest::skip(!ci_sono_font() || !fs::exists(std::string(M688_DATI) + "/acustica.toml"))) {
    const auto file = fs::temp_directory_path() / "m688_prova_ripetitori_radio.lua";
    const std::string modello = "modello = { massima_kn = 32, virata_gradi_s = 1, accel_kn_s = 0.5, quota_m_s = 1, quota_max_m = 450, integrita = 200 }";
    std::ofstream(file) << "return { id = 'PROVA', teatro = 'faroe', script = {}, varianti = { { probabilita = 1, unita = {\n"
                           "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true,"
                           "  posizione = { x = 0, y = 0 }, quota_m = 100, rotta = 0, velocita_kn = 5, " << modello << " },"
                           "\n}, timer = {} } } }\n";
    const auto classi = carica_classi(M688_DATI "/classi.toml");
    Missione m(file.string(), 1, false);
    DanniMissione dm(classi, M688_DATI, 1, RegolaDanni::simulazione);
    for (int i = 0; i < 9; ++i) { m.avanza(1.0 / 9.0); dm.avanza(m, 1.0 / 9.0); }
    REQUIRE(m.radio() != nullptr);
    const FontProva f;
    Tela t(LARGHEZZA_TELA, ALTEZZA_TELA);
    t.riempi(255);
    StazioneRadio r;
    r.disegna_sala(t, f.orig, f.incisa, m);
    controlla_meta(t, 0, ALTEZZA_VISTA);
    fs::remove(file);
}
