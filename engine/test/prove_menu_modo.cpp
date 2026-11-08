// Il menu con tasti e clic veri (prova a mano dell'utente del 06/11/2026): il modo di gioco, classico o Enhanced, e la marina del
// comandante nuovo. Le schermate ricevono i Comandi come dalla finestra: gli stessi campi, non il loro stato interno
#include <doctest.h>

#include <algorithm>
#include <cstdlib>
#include <filesystem>
#include <string>
#include <vector>

#include <SDL3/SDL.h>

#include "albo.hpp"
#include "grafica/cerimonia.hpp"
#include "grafica/decorazioni.hpp"
#include "grafica/font.hpp"
#include "grafica/palette.hpp"
#include "grafica/tela.hpp"
#include "nucleo/albo.hpp"
#include "nucleo/carriera.hpp"
#include "nucleo/fascicolo.hpp"
#include "nucleo/mappa_tasti.hpp"
#include "scelta_missione.hpp"
#include "schermata_modo.hpp"

using namespace m688;
namespace fs = std::filesystem;

namespace m688 {
std::string comando_del_tasto(const MappaTasti& tasti, int scancode, int tasto, int mod_tasto);   // piattaforma/src/finestra.cpp
std::string forma_tasti_valore();
}

namespace {

struct CartellaProva {   // XDG_DATA_HOME vale solo dentro il blocco, come in prove_albo
    fs::path dati = fs::temp_directory_path() / "m688_prova_menu";
    CartellaProva() {
        fs::remove_all(dati);
        setenv("XDG_DATA_HOME", dati.c_str(), 1);
    }
    ~CartellaProva() {
        unsetenv("XDG_DATA_HOME");
        fs::remove_all(dati);
    }
};

Comandi invio() { Comandi c; c.invio = true; return c; }
Comandi esc() { Comandi c; c.esc = true; return c; }
Comandi tasto_lettera(char l) { Comandi c; c.lettere.push_back(l); return c; }
Comandi clic(int x, int y) { Comandi c; c.clic.push_back({x, y}); return c; }
Comandi freccia(int su_giu, int ruota) { Comandi c; c.su_giu = su_giu; c.ruota = ruota; return c; }
Comandi testo(const std::string& s) { Comandi c; c.testo = s; return c; }
Comandi tab(int d) { Comandi c; c.tab = d; return c; }
Comandi cifra(int n) { Comandi c; c.cifre.push_back(n); return c; }
Comandi clic_su(const Rettangolo& r) { return clic(r.x + r.w / 2, r.y + r.h / 2); }

SchermataScelta menu() { return SchermataScelta(elenco_missioni(M688_MISSIONI), M688_DATI); }
std::optional<SceltaPartita> passo(SchermataScelta& s, const Comandi& c) {
    bool esci = false;
    return s.comandi(c, esci);
}
constexpr int MODO_X = 80, MODO_Y = 38;   // il tasto del modo in alto, il primo della fila

const RegoleCarriera& regole() {
    static const RegoleCarriera r = carica_carriera(M688_DATI "/carriera.toml");
    return r;
}
const TavolaDecorazioni& tavola() {
    static const TavolaDecorazioni t = TavolaDecorazioni::carica(regole(), M688_DATI "/decorazioni_grafica.toml");
    return t;
}
// fino alla casella «nuova carriera» (dopo l'ultimo cartellino) e Invio
void apri_modulo(SchermataAlbo& a, int comandanti = 0) {
    a.scegli(comandanti);
    a.comandi(invio());
    REQUIRE(a.modulo_aperto());
}

}  // namespace

TEST_CASE("schermata del modo: frecce, cifre, clic e Invio; Esc esce") {
    bool esci = false;
    {   // col cursore sull'Enhanced; freccia a sinistra e Invio: classico
        SchermataModo m;
        CHECK_FALSE(m.cursore_classico());
        CHECK_FALSE(m.comandi(freccia(0, -1), esci));
        CHECK(m.cursore_classico());
        m.comandi({}, esci);
        const auto s = m.comandi(invio(), esci);
        REQUIRE(s);
        CHECK(*s);
    }
    {   // cifra 2 ed Invio: Enhanced
        SchermataModo m;
        m.comandi(cifra(1), esci);
        CHECK(m.cursore_classico());
        m.comandi(cifra(2), esci);
        const auto s = m.comandi(invio(), esci);
        REQUIRE(s);
        CHECK_FALSE(*s);
    }
    {   // un clic sceglie il pannello, il secondo conferma; il tasto del piede conferma subito
        SchermataModo m;
        CHECK_FALSE(m.comandi(clic(100, 200), esci));
        CHECK(m.cursore_classico());
        const auto s = m.comandi(clic(100, 200), esci);
        REQUIRE(s);
        CHECK(*s);
        SchermataModo p;
        const auto d = p.comandi(clic(500, 425), esci);
        REQUIRE(d);
        CHECK_FALSE(*d);
    }
    {
        SchermataModo m;
        CHECK_FALSE(m.comandi(esc(), esci));
        CHECK(esci);
        esci = false;
        m.comandi(clic(100, 425), esci);
        CHECK(esci);
    }
}

TEST_CASE("menu: il modo si cambia col tasto O e col clic, in ogni scheda; il classico lascia solo le originali") {
    SchermataScelta s = menu();
    CHECK_FALSE(s.classico());
    passo(s, tasto_lettera('O'));
    CHECK(s.classico());
    passo(s, clic(MODO_X, MODO_Y));
    CHECK_FALSE(s.classico());

    // dalla scheda rapida: si torna alle originali
    passo(s, tasto_lettera('R'));
    passo(s, tasto_lettera('O'));
    CHECK(s.classico());

    // nel classico R, le schede, V, P e C non fanno niente
    passo(s, tasto_lettera('R'));
    passo(s, clic(8 + 158 * 2 + 40, 38));
    {
        Comandi c;
        c.cerca = true;
        passo(s, c);
        CHECK_FALSE(s.albo_chiesto());
        CHECK_FALSE(s.vuole_testo());
        passo(s, tasto_lettera('P'));
        CHECK_FALSE(s.vuole_testo());
        const auto v = passo(s, tasto_lettera('V'));
        CHECK_FALSE(v);
    }
    // Invio: una partita classica, senza sandbox, su una missione originale
    auto scelta = passo(s, invio());
    REQUIRE(scelta);
    CHECK(scelta->classico);
    CHECK_FALSE(scelta->sandbox);
    CHECK_FALSE(scelta->rapida);
    CHECK(scelta->missione.rfind("SCENE", 0) == 0);

    // tornati all'Enhanced: la partita non e' classica, e il resto risponde
    passo(s, tasto_lettera('O'));
    scelta = passo(s, invio());
    REQUIRE(scelta);
    CHECK_FALSE(scelta->classico);
    Comandi c;
    c.cerca = true;
    passo(s, c);
    CHECK(s.albo_chiesto());
}

TEST_CASE("menu: il battello del comandante scelto vale nel resto del giro") {
    SchermataScelta s = menu();
    CHECK_FALSE(s.alfa());
    s.vai_al_battello(true);
    // l'Alfa resta anche dopo un fotogramma, e la scelta e' una missione sovietica
    const auto scelta = passo(s, invio());
    REQUIRE(scelta);
    CHECK(s.alfa());
    bool sovietica = false;
    for (const auto& m : elenco_missioni(M688_MISSIONI)) sovietica = sovietica || (m.id == scelta->missione && m.lato == "urss");
    CHECK(sovietica);
    // e nella rapida
    passo(s, tasto_lettera('R'));
    Tela t;
    const Font f = Font::carica(M688_DATI "/font/originale.fnt");
    s.disegna(t, f);
    s.vai_al_battello(false);
    CHECK_FALSE(s.alfa());
}

TEST_CASE("comandante nuovo: scritto il nome, Invio non lo crea; la marina si sceglie coi tasti") {
    const CartellaProva c;
    SchermataAlbo a(regole(), tavola());
    apri_modulo(a);
    CHECK(a.vuole_testo());
    a.comandi(testo("Ramius"));
    a.comandi(invio());                      // il nome scritto: si passa alla marina, senza aprire il fascicolo
    CHECK(a.modulo_aperto());
    CHECK(a.campo_modulo() == 1);
    CHECK_FALSE(a.vuole_testo());
    CHECK(leggi_albo().empty());
    a.comandi(freccia(0, 1));                // destra: VMF
    CHECK(a.marina_modulo() == Marina::vmf);
    a.comandi({});
    a.comandi(invio());
    CHECK(a.campo_modulo() == 2);
    a.comandi(invio());
    CHECK(a.campo_modulo() == 3);
    a.comandi(invio());                      // l'ultimo campo: si apre
    CHECK_FALSE(a.modulo_aperto());
    const auto albo = leggi_albo();
    REQUIRE(albo.size() == 1);
    CHECK(albo[0].f.nome == "Ramius");
    CHECK(albo[0].f.marina == Marina::vmf);
    CHECK(comandante_in_carica(Marina::vmf));
    CHECK_FALSE(comandante_in_carica(Marina::usn));
    REQUIRE(a.marina_scelta());
    CHECK(*a.marina_scelta() == Marina::vmf);
}

TEST_CASE("comandante nuovo: tutto col mouse, e Tab e le frecce muovono il cursore") {
    const CartellaProva c;
    SchermataAlbo a(regole(), tavola());
    apri_modulo(a);
    // il clic sulla VMF, dal campo del nome, senza passare dai tasti
    a.comandi(clic_su(SchermataAlbo::casella_modulo(1, 1)));
    CHECK(a.marina_modulo() == Marina::vmf);
    CHECK(a.campo_modulo() == 1);
    a.comandi(clic_su(SchermataAlbo::casella_modulo(1, 0)));
    CHECK(a.marina_modulo() == Marina::usn);
    a.comandi(clic_su(SchermataAlbo::casella_modulo(1, 1)));
    // il clic sul nome torna a scrivere
    a.comandi(clic_su(SchermataAlbo::casella_modulo(0, 0)));
    CHECK(a.vuole_testo());
    // Tab e Maiusc+Tab, con giro
    a.comandi(tab(1));
    CHECK(a.campo_modulo() == 1);
    a.comandi(tab(-1));
    a.comandi(tab(-1));
    CHECK(a.campo_modulo() == 3);
    a.comandi(tab(1));
    CHECK(a.campo_modulo() == 0);
    // la freccia giu', la strada di prima
    a.comandi(freccia(-1, 0));
    CHECK(a.campo_modulo() == 1);
    a.comandi({});
    // Annulla col clic: niente fascicolo
    a.comandi(clic_su(SchermataAlbo::tasto_modulo(false)));
    CHECK_FALSE(a.modulo_aperto());
    CHECK(leggi_albo().empty());
    // di nuovo, VMF col clic, il tasto Apri col clic e senza nome: il nome di serie della sua marina
    apri_modulo(a);
    a.comandi(clic_su(SchermataAlbo::casella_modulo(1, 1)));
    a.comandi(clic_su(SchermataAlbo::tasto_modulo(true)));
    const auto albo = leggi_albo();
    REQUIRE(albo.size() == 1);
    CHECK(albo[0].f.marina == Marina::vmf);
    CHECK(albo[0].f.nome == "V. A. Suchkov");
}

TEST_CASE("comandante nuovo: il modulo si disegna, e le caselle stanno dentro lo schermo senza coprirsi") {
    const CartellaProva c;
    SchermataAlbo a(regole(), tavola());
    apri_modulo(a);
    a.comandi(testo("Ramius"));
    const Font f = Font::carica(M688_DATI "/font/originale.fnt");
    Tela t;
    a.disegna(t, f);
    if (const char* d = std::getenv("M688_ISTANTANEE")) {
        Palette p = Palette::carica(M688_DATI "/immagini/MAIN.pal");
        a.palette(p);
        t.salva_bmp(std::string(d) + "/menu_modulo.bmp", p);
    }
    std::vector<Rettangolo> tutte{SchermataAlbo::casella_modulo(0, 0), SchermataAlbo::tasto_modulo(false), SchermataAlbo::tasto_modulo(true)};
    for (int k = 0; k < 2; ++k) tutte.push_back(SchermataAlbo::casella_modulo(1, k)), tutte.push_back(SchermataAlbo::casella_modulo(2, k));
    for (int k = 0; k < 3; ++k) tutte.push_back(SchermataAlbo::casella_modulo(3, k));
    for (std::size_t i = 0; i < tutte.size(); ++i) {
        CHECK(tutte[i].x >= 0);
        CHECK(tutte[i].y >= 0);
        CHECK(tutte[i].x + tutte[i].w <= LARGHEZZA_TELA);
        CHECK(tutte[i].y + tutte[i].h <= ALTEZZA_TELA);
        for (std::size_t j = i + 1; j < tutte.size(); ++j) {
            const Rettangolo &p = tutte[i], &q = tutte[j];
            CHECK_FALSE((p.x < q.x + q.w && q.x < p.x + p.w && p.y < q.y + q.h && q.y < p.y + p.h));
        }
    }
}

TEST_CASE("menu e modo: le schermate si disegnano nei due modi") {
    const Font f = Font::carica(M688_DATI "/font/originale.fnt");
    const char* d = std::getenv("M688_ISTANTANEE");
    Tela t;
    SchermataModo m;
    m.disegna(t, f);
    if (d) {
        Palette p = Palette::carica(M688_DATI "/immagini/MAIN.pal");
        m.palette(p);
        t.salva_bmp(std::string(d) + "/menu_modo.bmp", p);
    }
    SchermataScelta s = menu();
    Tela e, cl;
    s.disegna(e, f);
    s.imposta_classico(true);
    s.disegna(cl, f);
    CHECK_FALSE(std::equal(e.dati(), e.dati() + LARGHEZZA_TELA * ALTEZZA_TELA, cl.dati()));
    if (d) {
        Palette p = Palette::carica(M688_DATI "/immagini/MAIN.pal");
        s.palette(p);
        e.salva_bmp(std::string(d) + "/menu_enhanced.bmp", p);
        cl.salva_bmp(std::string(d) + "/menu_classico.bmp", p);
    }
}

TEST_CASE("albo: una carriera si elimina dal piede, con la conferma; Esc la salva") {
    const CartellaProva c;
    Fascicolo a = Carriera::nuovo(regole(), Marina::vmf, "Ramius", true, Indulgenza::realistica);
    Fascicolo b = Carriera::nuovo(regole(), Marina::usn, "Mancuso", true, Indulgenza::realistica);
    salva_fascicolo(a, percorso_fascicolo(a.id));
    salva_fascicolo(b, percorso_fascicolo(b.id));
    imposta_in_carica(a.marina, a.id);
    imposta_in_carica(b.marina, b.id);
    REQUIRE(leggi_albo().size() == 2);
    SchermataAlbo albo(regole(), tavola());
    const auto rimasti = [] { return leggi_albo().size(); };
    const int y = 428 + 30;   // i tasti del piede
    const auto scegli_vmf = [&] {
        const auto v = leggi_albo();
        for (std::size_t i = 0; i < v.size(); ++i)
            if (v[i].f.id == a.id) albo.scegli(static_cast<int>(i));
        REQUIRE(albo.scelto());
        REQUIRE(albo.scelto()->id == a.id);
    };

    // E: chiede conferma e non cancella; Esc annulla, senza uscire
    scegli_vmf();
    albo.comandi(tasto_lettera('E'));
    CHECK(rimasti() == 2);
    CHECK(albo.comandi(esc()) == SchermataAlbo::Uscita::nessuna);
    CHECK(rimasti() == 2);
    // dopo l'annullamento la prima E torna a chiedere; nella conferma Invio e' il no, e non cambia chi e' in carica
    albo.comandi(tasto_lettera('E'));
    CHECK(rimasti() == 2);
    albo.comandi(invio());
    CHECK(rimasti() == 2);
    albo.comandi(tasto_lettera('E'));
    CHECK(rimasti() == 2);               // ha chiesto di nuovo: l'Invio aveva annullato
    albo.comandi(esc());

    // col mouse: il quarto tasto chiede, nella conferma il tasto di sinistra salva
    const int w5 = (LARGHEZZA_TELA - 24 - 8 * 4) / 5;
    albo.comandi(clic(12 + 3 * (w5 + 8) + w5 / 2, y));
    CHECK(rimasti() == 2);
    CHECK(albo.comandi(clic(100, y)) == SchermataAlbo::Uscita::nessuna);
    CHECK(rimasti() == 2);
    // il disegno della conferma, da guardare
    albo.comandi(clic(12 + 3 * (w5 + 8) + w5 / 2, y));
    const Font f = Font::carica(M688_DATI "/font/originale.fnt");
    if (const char* d = std::getenv("M688_ISTANTANEE")) {
        Tela t;
        albo.disegna(t, f);
        Palette p = Palette::carica(M688_DATI "/immagini/MAIN.pal");
        albo.palette(p);
        t.salva_bmp(std::string(d) + "/menu_elimina.bmp", p);
    }
    // il tasto di destra elimina: via il file e il posto in carica, l'altro comandante resta
    albo.comandi(clic(500, y));
    CHECK(rimasti() == 1);
    CHECK_FALSE(fs::exists(percorso_fascicolo(a.id)));
    CHECK_FALSE(comandante_in_carica(Marina::vmf));
    CHECK(comandante_in_carica(Marina::usn));
    REQUIRE(leggi_albo()[0].f.id == b.id);

    // anche E due volte e Backspace due volte, che c'era gia'
    salva_fascicolo(a, percorso_fascicolo(a.id));
    albo.ricarica();
    scegli_vmf();
    albo.comandi(tasto_lettera('E'));
    albo.comandi(tasto_lettera('E'));
    CHECK(rimasti() == 1);
    salva_fascicolo(a, percorso_fascicolo(a.id));
    albo.ricarica();
    scegli_vmf();
    Comandi indietro;
    indietro.cancella = true;
    albo.comandi(indietro);
    CHECK(rimasti() == 2);
    albo.comandi(indietro);
    CHECK(rimasti() == 1);

    // sul cartellino vuoto E non fa niente, e il tasto di destra e' ancora «nuova carriera»
    albo.scegli(1);
    REQUIRE_FALSE(albo.scelto());
    albo.comandi(tasto_lettera('E'));
    CHECK(rimasti() == 1);
    albo.comandi(clic(600, y));
    CHECK(albo.modulo_aperto());
}

TEST_CASE("menu: ogni opzione risponde al clic; la carta anche a D, e i vecchi [ ] restano") {
    const Font f = Font::carica(M688_DATI "/font/originale.fnt");
    SchermataScelta s = menu();
    Tela t;
    s.disegna(t, f);
    const auto clic_opzione = [&](const char* tasto) {
        const auto r = s.zona_opzione(tasto);
        REQUIRE(r);
        CHECK(r->x + r->w <= LARGHEZZA_TELA);
        passo(s, clic_su(*r));
        s.disegna(t, f);
    };
    CHECK_FALSE(s.zona_opzione("[ ]"));   // a schermo non c'e' piu' un tasto che manca su mezze tastiere
    SceltaPartita o = s.opzioni_partita();
    const bool carta = o.carta_epoca, stazioni = o.anni_89, meteo = o.meteo_realistico;
    const int asw = o.asw, sonarista = o.sonarista;

    const int riga = s.missione_scelta();
    clic_opzione("D");
    CHECK(s.opzioni_partita().carta_epoca != carta);
    CHECK(s.missione_scelta() == riga);   // il clic sulle opzioni non sceglie la missione alla stessa altezza
    passo(s, tasto_lettera('D'));
    CHECK(s.opzioni_partita().carta_epoca == carta);
    {   // [ e ]: il campo di sempre, anche uno solo
        Comandi meno;
        meno.minuti = -1;
        passo(s, meno);
        CHECK(s.opzioni_partita().carta_epoca != carta);
        Comandi piu;
        piu.minuti = 1;
        passo(s, piu);
        CHECK(s.opzioni_partita().carta_epoca == carta);
    }
    clic_opzione("TAB");
    CHECK(s.opzioni_partita().anni_89 != stazioni);
    clic_opzione("W");
    CHECK(s.opzioni_partita().meteo_realistico != meteo);
    clic_opzione("K");
    CHECK(s.opzioni_partita().asw == (asw + 1) % 4);
    clic_opzione("S");
    CHECK(s.opzioni_partita().sonarista == (sonarista + 1) % 4);
    const bool nav = s.opzioni_partita().navigazione_realistica;
    clic_opzione("N");
    CHECK(s.opzioni_partita().navigazione_realistica != nav);
    const int musica = s.musica(), video = s.video();
    clic_opzione("M");
    CHECK(s.musica() == (musica + 1) % 6);
    clic_opzione("A");
    CHECK(s.video() == (video + 1) % 3);
    CHECK(s.missione_scelta() == riga);
    // nell'elenco il clic sceglie la missione, come prima
    passo(s, clic(100, 76 + 2 * 30 + 5));
    CHECK(s.missione_scelta() == 2);
    // le scelte arrivano alla partita
    const auto scelta = passo(s, invio());
    REQUIRE(scelta);
    CHECK(scelta->carta_epoca == carta);
    CHECK(scelta->anni_89 != stazioni);

    // anche nella scheda rapida, e un clic fuori dalle righe non cambia niente
    passo(s, tasto_lettera('R'));
    s.disegna(t, f);
    const SceltaPartita prima = s.opzioni_partita();
    clic_opzione("D");
    CHECK(s.opzioni_partita().carta_epoca != prima.carta_epoca);
    CHECK_FALSE(s.zona_opzione("W"));     // il meteo della rapida e' fra i suoi parametri
    passo(s, clic(LARGHEZZA_TELA - 2, 300));
    CHECK(s.opzioni_partita().sonarista == prima.sonarista);

    // nel classico restano musica e video, cliccabili; la carta non c'e', e D non fa niente
    SchermataScelta cl = menu();
    cl.imposta_classico(true);
    Tela tc;
    cl.disegna(tc, f);
    CHECK_FALSE(cl.zona_opzione("D"));
    const auto rm = cl.zona_opzione("M");
    REQUIRE(rm);
    const int m0 = cl.musica();
    passo(cl, clic_su(*rm));
    CHECK(cl.musica() == (m0 + 1) % 6);
    const bool c0 = cl.opzioni_partita().carta_epoca;
    passo(cl, tasto_lettera('D'));
    CHECK(cl.opzioni_partita().carta_epoca == c0);
}

// SDL senza finestra, con la mappa dei tasti del driver scelto (dummy: quella di serie, americana)
struct VideoSdl {
    bool ok;
    explicit VideoSdl(const char* driver) {
        SDL_SetHint(SDL_HINT_VIDEO_DRIVER, driver);
        ok = SDL_Init(SDL_INIT_VIDEO);
    }
    ~VideoSdl() { SDL_Quit(); }
};

TEST_CASE("tasti: su tastiera americana niente cambia, e i comandi che c'erano restano") {
    VideoSdl sdl("dummy");
    REQUIRE(sdl.ok);
    MappaTasti t = MappaTasti::predefinita();
    const auto com = [&](SDL_Scancode s, SDL_Keymod m) { return comando_del_tasto(t, s, SDL_GetKeyFromScancode(s, SDL_KMOD_NONE, true), m); };
    CHECK(forma_tasti_valore() == "[ ]");   // la scritta resta quella di sempre
    CHECK(com(SDL_SCANCODE_LEFTBRACKET, SDL_KMOD_NONE) == "valore_meno");
    CHECK(com(SDL_SCANCODE_RIGHTBRACKET, SDL_KMOD_NONE) == "valore_piu");
    CHECK(com(SDL_SCANCODE_LEFTBRACKET, SDL_KMOD_LSHIFT) == "valore_meno");   // il passo lungo, come prima
    CHECK(com(SDL_SCANCODE_EQUALS, SDL_KMOD_NONE) == "telegrafo_avanti");
    CHECK(com(SDL_SCANCODE_MINUS, SDL_KMOD_NONE) == "telegrafo_indietro");
    CHECK(com(SDL_SCANCODE_PERIOD, SDL_KMOD_NONE) == "pilota_automatico");
    // i tasti senza comando restano senza, anche con Maiusc e Ctrl
    CHECK(com(SDL_SCANCODE_SEMICOLON, SDL_KMOD_NONE).empty());
    CHECK(com(SDL_SCANCODE_SEMICOLON, SDL_KMOD_LSHIFT).empty());
    CHECK(com(SDL_SCANCODE_SLASH, SDL_KMOD_LCTRL).empty());
    // le lettere e le cifre non passano dal carattere prodotto: Maiusc+0 (`)`) non e' un tasto nuovo
    CHECK(com(SDL_SCANCODE_F, SDL_KMOD_NONE) == t.comando("F"));
    CHECK(com(SDL_SCANCODE_1, SDL_KMOD_LSHIFT) == t.comando("1"));
}

// Col layout vero: M688_PROVA_X11_IT=1 sulla macchina con layout it, senza aprire finestre (la mappa del server X)
TEST_CASE("tasti: su tastiera italiana AltGr+è e AltGr++ sono [ e ]" * doctest::skip(std::getenv("M688_PROVA_X11_IT") == nullptr)) {
    VideoSdl sdl("x11");
    REQUIRE(sdl.ok);
    const SDL_Keycode sopra = SDL_GetKeyFromScancode(SDL_SCANCODE_LEFTBRACKET, SDL_KMOD_NONE, true), piu = SDL_GetKeyFromScancode(SDL_SCANCODE_RIGHTBRACKET, SDL_KMOD_NONE, true);
    REQUIRE(sopra == 0xE8);   // è
    REQUIRE(piu == '+');
    CHECK(forma_tasti_valore() == "ALTGR+È/+");
    MappaTasti t = MappaTasti::predefinita();
    const auto com = [&](SDL_Scancode s, SDL_Keycode k, SDL_Keymod m) { return comando_del_tasto(t, s, static_cast<int>(k), m); };
    // senza AltGr: è niente, + il telegrafo, come oggi
    CHECK(com(SDL_SCANCODE_LEFTBRACKET, sopra, SDL_KMOD_NONE).empty());
    CHECK(com(SDL_SCANCODE_RIGHTBRACKET, piu, SDL_KMOD_NONE) == "telegrafo_avanti");
    // AltGr, come arriva su X11 (MODE) e altrove (RALT, e Ctrl+RALT su Windows)
    for (const SDL_Keymod m : {static_cast<SDL_Keymod>(SDL_KMOD_MODE), static_cast<SDL_Keymod>(SDL_KMOD_RALT), static_cast<SDL_Keymod>(SDL_KMOD_LCTRL | SDL_KMOD_RALT)}) {
        CHECK(com(SDL_SCANCODE_LEFTBRACKET, sopra, m) == "valore_meno");
        CHECK(com(SDL_SCANCODE_RIGHTBRACKET, piu, m) == "valore_piu");
        // con Maiusc: { e } non hanno comando, resta il passo lungo di [ e ]
        CHECK(com(SDL_SCANCODE_LEFTBRACKET, sopra, static_cast<SDL_Keymod>(m | SDL_KMOD_LSHIFT)) == "valore_meno");
        CHECK(com(SDL_SCANCODE_RIGHTBRACKET, piu, static_cast<SDL_Keymod>(m | SDL_KMOD_LSHIFT)) == "valore_piu");
    }
    // il + senza AltGr, con Maiusc (*), non diventa altro; - e . come sono
    CHECK(com(SDL_SCANCODE_RIGHTBRACKET, piu, SDL_KMOD_LSHIFT) == "telegrafo_avanti");
    CHECK(com(SDL_SCANCODE_SLASH, '-', SDL_KMOD_NONE) == "telegrafo_indietro");
    CHECK(com(SDL_SCANCODE_PERIOD, '.', SDL_KMOD_NONE) == "pilota_automatico");
    // Maiusc+0 e' `=`, il telegrafo avanti negli alias: se lo 0 non ha un suo comando, ora si raggiunge anche cosi'
    CHECK(com(SDL_SCANCODE_0, '0', SDL_KMOD_LSHIFT) == (t.comando("0").empty() ? std::string("telegrafo_avanti") : t.comando("0")));
}

// Le scritte con la forma lunga stanno nello schermo? Sono copie delle righe di scena_prova.cpp, sonar_enhanced.cpp e carta.cpp, che con
// un messaggio di bordo in corso non si vedono: la larghezza si misura qui
TEST_CASE("scritte: con ALTGR+È/+ le righe d'aiuto dell'Enhanced stanno nello schermo, e il font ha la È") {
    const Font f = Font::carica(M688_DATI "/font/originale.fnt");
    const std::string e = "ALTGR+È/+";
    CHECK(f.larghezza("È", 1) > 0);
    CHECK(f.larghezza(e, 1) > f.larghezza("ALTGR+E/+", 1) - 2);
    const std::vector<std::pair<std::string, int>> righe = {
        {"CLIC TUBO: RICARICA/ARMA/DISARMA   CTRL+V ARMA   TAB " + e + " IMPOSTA   L LANCIA   S C D Z X FILO", 12},
        {"CLIC TUBO  CTRL+V ARMA  TAB " + e + " IMPOSTA  L LANCIA  S C D Z X 8 9 Y FILO  CTRL+S SILENZIO", 12},
        {"4 ANALIZZA 6 FASCIO 7 INT 8 AUTO 9 CURS TAB " + e + " B LIBRERIA W J NOTE S Y C SLOT T K TRACKER I O", 12},
        {"9 BOA SLOT  " + e + " RITARDO DELLA BOA   TRASMETTERE ESPONE ALLA RADIOGONIOMETRIA NEMICA", 12},
        {"1-9 PULSANTI   " + e + " RITARDO SLOT   INVIO AUTENTICA   TRASMETTERE ESPONE AL DF", 12},
    };
    for (const auto& [r, x] : righe) {
        CAPTURE(r);
        MESSAGE("larghezza ", f.larghezza(r, 1) + x, " su ", LARGHEZZA_TELA, ": ", r);
        CHECK(f.larghezza(r, 1) + x <= LARGHEZZA_TELA);
    }
}

TEST_CASE("finestra: la rotella del mouse arriva nei comandi, uno scatto per evento, col verso giusto") {
    VideoSdl video("dummy");
    REQUIRE(video.ok);
    Finestra w("prova", 1);
    const auto scatto = [](float y, SDL_MouseWheelDirection d) {
        SDL_Event e{};
        e.type = SDL_EVENT_MOUSE_WHEEL;
        e.wheel.y = y;
        e.wheel.direction = d;
        SDL_PushEvent(&e);
    };
    scatto(1.0f, SDL_MOUSEWHEEL_NORMAL);
    scatto(1.0f, SDL_MOUSEWHEEL_NORMAL);
    CHECK(w.eventi().rotella == 2);
    CHECK(w.eventi().rotella == 0);   // si consuma alla lettura
    scatto(-1.0f, SDL_MOUSEWHEEL_NORMAL);
    CHECK(w.eventi().rotella == -1);
    scatto(1.0f, SDL_MOUSEWHEEL_FLIPPED);
    CHECK(w.eventi().rotella == -1);
}

