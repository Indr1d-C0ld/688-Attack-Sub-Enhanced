// Le frecce nelle schermate di contorno: una pressione e' un passo, anche se dura meno di un fotogramma e anche se dura molti
// fotogrammi. Gli eventi entrano in SDL (driver video dummy) e passano da Finestra::eventi come nel gioco. SDL_PushEvent non
// cambia lo stato dei tasti: per il tasto tenuto i Comandi si riempiono come li riempirebbe la finestra.
#include <SDL3/SDL.h>
#include <doctest.h>

#include <cstdlib>
#include <filesystem>
#include <map>
#include <optional>
#include <string>

#include "albo.hpp"
#include "bacheca.hpp"
#include "briefing.hpp"
#include "grafica/decorazioni.hpp"
#include "grafica/font.hpp"
#include "grafica/tela.hpp"
#include "missioni/missione.hpp"
#include "nucleo/achievement.hpp"
#include "nucleo/carico.hpp"
#include "nucleo/carriera.hpp"
#include "nucleo/compiti_carico.hpp"
#include "piattaforma/finestra.hpp"
#include "scelta_missione.hpp"
#include "schermata_modo.hpp"
#include "schermata_siluri.hpp"

using namespace m688;
namespace fs = std::filesystem;

namespace {
const bool CON_DATI = fs::exists(M688_DATI "/carico.toml") && fs::exists(M688_MISSIONI "/SCENE1A.lua");

Finestra& finestra() {
    SDL_SetHint(SDL_HINT_VIDEO_DRIVER, "dummy");
    static Finestra w("prova delle frecce");
    return w;
}

void premi(SDL_Scancode sc, SDL_Keycode k) {
    SDL_Event e{};
    e.type = SDL_EVENT_KEY_DOWN;
    e.key.scancode = sc;
    e.key.key = k;
    e.key.down = true;
    SDL_PushEvent(&e);
    e.type = SDL_EVENT_KEY_UP;
    e.key.down = false;
    SDL_PushEvent(&e);
}
void destra() { premi(SDL_SCANCODE_RIGHT, SDLK_RIGHT); }
void sinistra() { premi(SDL_SCANCODE_LEFT, SDLK_LEFT); }
void su() { premi(SDL_SCANCODE_UP, SDLK_UP); }
void giu() { premi(SDL_SCANCODE_DOWN, SDLK_DOWN); }

// i fotogrammi dopo una pressione tenuta a lungo: lo stato del tasto, senza pressioni nuove
Comandi tenuto(int su_giu, int ruota) {
    Comandi c;
    c.su_giu = su_giu;
    c.ruota = ruota;
    return c;
}
// il primo fotogramma della stessa pressione
Comandi premuto(int su_giu, int ruota) {
    Comandi c = tenuto(su_giu, ruota);
    c.passi_su_giu = su_giu;
    c.passi_ruota = ruota;
    return c;
}

struct CartellaProva {   // i fascicoli e le preferenze di una prova non sono quelli dell'utente
    fs::path dati = fs::temp_directory_path() / "melchior_prove_frecce";
    CartellaProva() {
        fs::remove_all(dati);
        setenv("XDG_DATA_HOME", dati.c_str(), 1);
    }
    ~CartellaProva() {
        unsetenv("XDG_DATA_HOME");
        fs::remove_all(dati);
    }
};

const RegoleCarriera& regole() {
    static const RegoleCarriera r = carica_carriera(M688_DATI "/carriera.toml");
    return r;
}
const TavolaDecorazioni& tavola() {
    static const TavolaDecorazioni t = TavolaDecorazioni::carica(regole(), M688_DATI "/decorazioni_grafica.toml");
    return t;
}
}  // namespace

TEST_CASE("passi_freccia: le pressioni contate, o il fronte per chi da' solo lo stato") {
    int prec = 0;
    CHECK(passi_freccia(0, 1, prec) == 1);    // premuto e rilasciato dentro la lettura
    CHECK(prec == 0);
    CHECK(passi_freccia(1, 1, prec) == 1);    // premuto e ancora giu': una volta sola
    CHECK(passi_freccia(1, 0, prec) == 0);
    CHECK(passi_freccia(1, 0, prec) == 0);
    CHECK(passi_freccia(0, 0, prec) == 0);
    CHECK(passi_freccia(0, -2, prec) == -2);  // due pressioni in una lettura
    prec = 0;
    CHECK(passi_freccia(-1, 0, prec) == -1);  // i Comandi riempiti a mano: il fronte, come prima
    CHECK(passi_freccia(-1, 0, prec) == 0);
    CHECK(passi_freccia(0, 0, prec) == 0);
    CHECK(verso_freccia(1, 0) == 1);
    CHECK(verso_freccia(0, 3) == 1);
    CHECK(verso_freccia(0, -1) == -1);
    CHECK(verso_freccia(0, 0) == 0);
}

TEST_CASE("schermata del modo: una pressione breve sceglie" * doctest::skip(!CON_DATI)) {
    Finestra& w = finestra();
    SchermataModo m;
    bool esci = false;
    const bool all_inizio = m.cursore_classico();
    all_inizio ? destra() : sinistra();
    m.comandi(w.eventi(), esci);
    CHECK(m.cursore_classico() != all_inizio);
    all_inizio ? sinistra() : destra();
    m.comandi(w.eventi(), esci);
    CHECK(m.cursore_classico() == all_inizio);
}

TEST_CASE("scelta della missione: le pressioni brevi non si perdono, due in un fotogramma contano due" * doctest::skip(!CON_DATI)) {
    CartellaProva cartella;
    Finestra& w = finestra();
    SchermataScelta s(elenco_missioni(M688_MISSIONI), M688_DATI);
    bool esci = false;
    REQUIRE(s.missione_scelta() == 0);
    giu();
    s.comandi(w.eventi(), esci);
    CHECK(s.missione_scelta() == 1);
    giu();
    giu();
    s.comandi(w.eventi(), esci);
    CHECK(s.missione_scelta() == 3);
    su();
    s.comandi(w.eventi(), esci);
    CHECK(s.missione_scelta() == 2);
    // tenuto: un passo solo, per quanto duri
    s.comandi(premuto(-1, 0), esci);
    CHECK(s.missione_scelta() == 3);
    for (int i = 0; i < 20; ++i) s.comandi(tenuto(-1, 0), esci);
    CHECK(s.missione_scelta() == 3);
    s.comandi(tenuto(0, 0), esci);
    // il battello: sulla prima missione che ha tutti e due i lati (le altre lo riportano al loro)
    bool cambiato = false;
    for (int i = 0; i < 30 && !cambiato; ++i) {
        const bool alfa = s.alfa();
        alfa ? sinistra() : destra();
        s.comandi(w.eventi(), esci);
        cambiato = s.alfa() != alfa;
        if (!cambiato) {
            giu();
            s.comandi(w.eventi(), esci);
        }
    }
    CHECK(cambiato);
}

TEST_CASE("sala siluri: la pressione breve sceglie l'arma, la mette e la toglie" * doctest::skip(!CON_DATI)) {
    Finestra& w = finestra();
    const CatalogoCarico cat = carica_carico(M688_DATI "/carico.toml");
    const auto compiti = carica_compiti(M688_DATI "/carico.toml");
    LimitiMissione l;
    l.deposito = {{"mk48", 25}, {"harpoon", 10}};
    SalaSiluri sala(cat, "los_angeles", l);
    sala.carico_standard("antisom");
    SchermataSiluri s(sala, "Los Angeles", 1989, l.tipo_missione, compiti);
    const int r0 = s.scelta();
    giu();
    s.comandi(w.eventi(), 0.02);
    CHECK(s.scelta() == r0 + 1);
    su();
    s.comandi(w.eventi(), 0.02);
    CHECK(s.scelta() == r0);

    // la prima arma a bordo: Sinistra la toglie, Destra la rimette
    const CaricoPartenza prima = sala.carico();
    for (int i = 0; i < 40 && sala.carico() == prima; ++i) {
        sinistra();
        s.comandi(w.eventi(), 0.02);
        if (sala.carico() == prima) {
            giu();
            s.comandi(w.eventi(), 0.02);
        }
    }
    REQUIRE_FALSE(sala.carico() == prima);
    const CaricoPartenza senza = sala.carico();
    destra();
    s.comandi(w.eventi(), 0.02);
    CHECK_FALSE(sala.carico() == senza);
}

TEST_CASE("albo: la pressione breve sposta il campo del modulo" * doctest::skip(!CON_DATI)) {
    CartellaProva cartella;
    Finestra& w = finestra();
    SchermataAlbo a(regole(), tavola());
    a.scegli(0);
    Comandi inv;
    inv.invio = true;
    a.comandi(inv);
    REQUIRE(a.modulo_aperto());
    const auto campo = [&] { return a.campo_modulo(); };
    REQUIRE(campo() == 0);
    giu();
    a.comandi(w.eventi());
    CHECK(campo() == 1);
    giu();
    giu();
    a.comandi(w.eventi());
    CHECK(campo() == 3);
    su();
    a.comandi(w.eventi());
    CHECK(campo() == 2);
}

TEST_CASE("briefing: una pressione gira una pagina sola, per quanto duri" * doctest::skip(!CON_DATI)) {
    Finestra& w = finestra();
    const Font f = Font::carica(M688_DATI "/font/originale.fnt");
    Tela t;
    // la prima missione della libreria con almeno tre pagine di ordini (la radio c'e' con le regole dell'Enhanced)
    std::optional<Missione> m;
    std::optional<SchermataBriefing> b;
    for (const auto& file : fs::recursive_directory_iterator(M688_MISSIONI "/libreria")) {
        if (file.path().extension() != ".lua") continue;
        b.reset();
        m.reset();
        try {
            m.emplace(file.path().string(), 1, false);
        } catch (const std::exception&) {
            continue;
        }
        m->radio_enhanced();
        b.emplace(*m, false, false, StileCarta::vga, M688_DATI);
        b->disegna(t, f, *m);
        if (b->pagine_ordini() >= 3) break;
    }
    REQUIRE(b);
    REQUIRE(b->pagine_ordini() >= 3);
    REQUIRE(b->ordini_aperti());
    b->comandi(premuto(-1, 0), 0.02);
    CHECK(b->pagina_ordini() == 1);
    for (int i = 0; i < 20; ++i) b->comandi(tenuto(-1, 0), 0.02);
    CHECK(b->pagina_ordini() == 1);   // prima arrivava all'ultima
    b->comandi(tenuto(0, 0), 0.02);
    giu();                            // breve, dentro il fotogramma
    b->comandi(w.eventi(), 0.02);
    CHECK(b->pagina_ordini() == 2);
    su();
    b->comandi(w.eventi(), 0.02);
    CHECK(b->pagina_ordini() == 1);
}

TEST_CASE("bacheca: una pressione sposta di un medaglione solo, per quanto duri" * doctest::skip(!CON_DATI)) {
    CartellaProva cartella;
    Finestra& w = finestra();
    const CatalogoAchievement cat = carica_achievement(M688_DATI "/achievement.toml");
    SchermataBacheca b(cat, tavola(), std::nullopt);
    REQUIRE(b.voce_scelta() == 0);
    b.comandi(premuto(0, 1));
    CHECK(b.voce_scelta() == 1);
    for (int i = 0; i < 20; ++i) b.comandi(tenuto(0, 1));
    CHECK(b.voce_scelta() == 1);      // prima correva fino all'ultimo
    b.comandi(tenuto(0, 0));
    destra();
    b.comandi(w.eventi());
    CHECK(b.voce_scelta() == 2);
    sinistra();
    b.comandi(w.eventi());
    CHECK(b.voce_scelta() == 1);
}
