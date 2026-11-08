// La navigazione del Museo con tasti e clic veri: gli eventi entrano in SDL (driver video dummy, finestra vera a scala 2),
// passano da Finestra::eventi e arrivano a SchermataMuseo fino al cambio di scheda. Il giro di un fotogramma e' quello di
// app/main.cpp. La prova col gioco intero, sotto X e dal percorso dell'utente, e' in tools/re/prove/museo.
#include <SDL3/SDL.h>
#include <doctest.h>

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <map>
#include <memory>
#include <string>

#include "carta.hpp"
#include "grafica/tela.hpp"
#include "museo.hpp"
#include "nucleo/enciclopedia.hpp"
#include "nucleo/fondale.hpp"
#include "piattaforma/finestra.hpp"
#include "risorse.hpp"

using namespace m688;

namespace {
const bool CON_DATI = std::filesystem::exists(M688_DATI "/enciclopedia.toml") && std::filesystem::exists(M688_DATI "/classi.toml");

constexpr int SCALA = 2;
// i centri dei tasti disegnati nello schedario e di quelli del piede
constexpr int SIN_X = 25, DES_X = 187, FRECCE_Y = 41, NOME_X = 100;
constexpr int PIEDE_TASTI_Y = 463, ESC_X = 100, TAB_X = 320, PAG_X = 530;
constexpr int ELENCO_X = 80, ELENCO_Y0 = 75, PASSO = 14;   // il centro della prima riga del vetro

void premi(SDL_Scancode sc, SDL_Keycode k, bool ripetuto = false) {
    SDL_Event e{};
    e.type = SDL_EVENT_KEY_DOWN;
    e.key.scancode = sc;
    e.key.key = k;
    e.key.down = true;
    e.key.repeat = ripetuto;
    SDL_PushEvent(&e);
    e.type = SDL_EVENT_KEY_UP;
    e.key.down = false;
    SDL_PushEvent(&e);
}
void destra() { premi(SDL_SCANCODE_RIGHT, SDLK_RIGHT); }
void sinistra() { premi(SDL_SCANCODE_LEFT, SDLK_LEFT); }
void su() { premi(SDL_SCANCODE_UP, SDLK_UP); }
void giu() { premi(SDL_SCANCODE_DOWN, SDLK_DOWN); }
void invio() { premi(SDL_SCANCODE_RETURN, SDLK_RETURN); }
void esc() { premi(SDL_SCANCODE_ESCAPE, SDLK_ESCAPE); }

// un clic nei pixel della tela, portato ai pixel della finestra come lo manda il sistema
void clicca(int x, int y) {
    SDL_Event e{};
    e.type = SDL_EVENT_MOUSE_BUTTON_DOWN;
    e.button.button = SDL_BUTTON_LEFT;
    e.button.down = true;
    e.button.x = static_cast<float>(x * SCALA + 1);
    e.button.y = static_cast<float>(y * SCALA + 1);
    SDL_PushEvent(&e);
    e.type = SDL_EVENT_MOUSE_BUTTON_UP;
    e.button.down = false;
    SDL_PushEvent(&e);
}

struct Banco {
    Risorse r = carica_risorse(M688_DATI);
    Enciclopedia enc = carica_enciclopedia(M688_DATI "/enciclopedia.toml");
    Finestra w{"prova del museo", SCALA};
    Tela tela;
};

Banco& banco() {
    SDL_SetHint(SDL_HINT_VIDEO_DRIVER, "dummy");
    static Banco b;
    return b;
}

// un fotogramma come in app/main.cpp: vero se il Museo chiede di uscire
bool giro(Banco& b, SchermataMuseo& m) {
    const Comandi c = b.w.eventi();
    if (m.comandi(c, 1.0 / 60.0)) return true;
    m.disegna(b.tela, b.r.font);
    Palette p = b.r.giorno;
    m.palette(p);
    b.w.presenta(b.tela, p);
    return false;
}
}  // namespace

TEST_CASE("Museo, frecce: una pressione breve cambia sezione e voce, anche dentro un solo fotogramma" * doctest::skip(!CON_DATI)) {
    Banco& b = banco();
    SchermataMuseo m(b.enc, b.r, M688_DATI);
    giro(b, m);
    REQUIRE(m.quante_sezioni() > 3);
    const VoceEnciclopedia* prima = m.voce();
    REQUIRE(prima);

    destra();   // premuto e rilasciato prima della lettura: lo stato dei tasti non lo vede, l'evento si'
    giro(b, m);
    CHECK(m.sezione() == 1);
    REQUIRE(m.voce());
    CHECK(m.voce()->gruppo != prima->gruppo);

    sinistra();
    giro(b, m);
    CHECK(m.sezione() == 0);
    CHECK(m.voce() == prima);

    sinistra();   // in giro: dalla prima all'ultima
    giro(b, m);
    CHECK(m.sezione() == m.quante_sezioni() - 1);
    destra();
    giro(b, m);
    CHECK(m.sezione() == 0);

    destra();   // due pressioni in un fotogramma contano due
    destra();
    giro(b, m);
    CHECK(m.sezione() == 2);
    sinistra();
    sinistra();
    giro(b, m);
    CHECK(m.sezione() == 0);

    const VoceEnciclopedia* v0 = m.voce();
    giu();
    giro(b, m);
    CHECK(m.voce() != v0);
    const VoceEnciclopedia* v1 = m.voce();
    giu();
    giu();
    giro(b, m);
    CHECK(m.voce() != v1);
    su();
    su();
    su();
    su();   // in cima si ferma
    giro(b, m);
    CHECK(m.voce() == v0);
}

TEST_CASE("Museo, frecce: la ripetizione del sistema non conta, tenere premuto scorre le voci ma non le sezioni" * doctest::skip(!CON_DATI)) {
    Banco& b = banco();
    SchermataMuseo m(b.enc, b.r, M688_DATI);
    giro(b, m);
    premi(SDL_SCANCODE_RIGHT, SDLK_RIGHT, true);
    premi(SDL_SCANCODE_DOWN, SDLK_DOWN, true);
    const VoceEnciclopedia* v0 = m.voce();
    giro(b, m);
    CHECK(m.sezione() == 0);
    CHECK(m.voce() == v0);

    Comandi tenuto;   // lo stato dei tasti: su_giu e ruota, senza nuove pressioni
    tenuto.su_giu = -1;
    tenuto.ruota = 1;
    CHECK_FALSE(m.comandi(tenuto, 0.01));   // il primo fotogramma non scatta
    CHECK(m.voce() == v0);
    for (int i = 0; i < 40; ++i) CHECK_FALSE(m.comandi(tenuto, 0.02));   // 0,8 s: oltre il ritardo, ripete
    CHECK(m.sezione() == 0);
    CHECK(m.voce() != v0);
}

TEST_CASE("Museo, clic: i tasti < e > cambiano sezione, il clic su una riga sceglie la voce" * doctest::skip(!CON_DATI)) {
    Banco& b = banco();
    SchermataMuseo m(b.enc, b.r, M688_DATI);
    giro(b, m);
    clicca(DES_X, FRECCE_Y);
    giro(b, m);
    CHECK(m.sezione() == 1);
    clicca(SIN_X, FRECCE_Y);
    giro(b, m);
    CHECK(m.sezione() == 0);
    clicca(SIN_X, FRECCE_Y);
    giro(b, m);
    CHECK(m.sezione() == m.quante_sezioni() - 1);
    clicca(DES_X, FRECCE_Y);
    giro(b, m);
    CHECK(m.sezione() == 0);

    const Enciclopedia& e = b.enc;
    const auto voci = e.del_gruppo(m.voce()->gruppo);
    REQUIRE(voci.size() > 4);
    const Tela prima = b.tela;
    clicca(ELENCO_X, ELENCO_Y0 + 3 * PASSO);
    giro(b, m);
    CHECK(m.voce() == voci[3]);
    bool cambiata = false;   // la riga accesa e' quella cliccata
    for (int x = 20; x < 190; ++x) cambiata = cambiata || prima.at(x, ELENCO_Y0 + 3 * PASSO) != b.tela.at(x, ELENCO_Y0 + 3 * PASSO);
    CHECK(cambiata);
    clicca(ELENCO_X, ELENCO_Y0);
    giro(b, m);
    CHECK(m.voce() == voci[0]);
    clicca(ELENCO_X, 415);   // sotto l'ultima riga di questa sezione corta: niente
    giro(b, m);
    CHECK(m.voce() == voci[0]);
}

TEST_CASE("Museo, clic in un elenco lungo: la riga scelta e' quella sotto il puntatore anche a elenco scorso" * doctest::skip(!CON_DATI)) {
    Banco& b = banco();
    SchermataMuseo m(b.enc, b.r, M688_DATI);
    giro(b, m);
    int lunga = -1;
    for (int s = 0; s < m.quante_sezioni() && lunga < 0; ++s) {
        if (b.enc.del_gruppo(m.voce()->gruppo).size() > 30) {
            lunga = s;
        } else {
            destra();
            giro(b, m);
        }
    }
    REQUIRE(lunga >= 0);
    const auto voci = b.enc.del_gruppo(m.voce()->gruppo);
    for (int i = 0; i < 20; ++i) giu();
    giro(b, m);
    REQUIRE(m.voce() == voci[20]);   // in vista dalla 8 alla 31: la 20 sta a meta' del vetro
    clicca(ELENCO_X, ELENCO_Y0 + 2 * PASSO);
    giro(b, m);
    CHECK(m.voce() == voci[10]);
}

TEST_CASE("Museo, indice delle sezioni: da tastiera e col clic, Esc lo chiude senza uscire" * doctest::skip(!CON_DATI)) {
    Banco& b = banco();
    SchermataMuseo m(b.enc, b.r, M688_DATI);
    giro(b, m);
    const VoceEnciclopedia* v0 = m.voce();
    const Tela senza = b.tela;

    invio();
    giro(b, m);
    REQUIRE(m.indice_aperto());
    bool diverso = false;
    for (int y = 60; y < 420; ++y) diverso = diverso || senza.at(ELENCO_X, y) != b.tela.at(ELENCO_X, y);
    CHECK(diverso);
    CHECK(m.voce() == v0);
    giu();
    giu();
    giu();
    giro(b, m);
    CHECK(m.sezione() == 0);   // l'indice si scorre senza cambiare scheda
    CHECK(m.voce() == v0);
    invio();
    giro(b, m);
    CHECK_FALSE(m.indice_aperto());
    CHECK(m.sezione() == 3);

    clicca(NOME_X, FRECCE_Y);
    giro(b, m);
    REQUIRE(m.indice_aperto());
    clicca(ELENCO_X, ELENCO_Y0 + 5 * PASSO);
    giro(b, m);
    CHECK_FALSE(m.indice_aperto());
    CHECK(m.sezione() == 5);

    invio();
    giro(b, m);
    REQUIRE(m.indice_aperto());
    esc();
    CHECK_FALSE(giro(b, m));   // Esc chiude l'indice: la scheda resta
    CHECK_FALSE(m.indice_aperto());
    CHECK(m.sezione() == 5);
    clicca(NOME_X, FRECCE_Y);
    giro(b, m);
    REQUIRE(m.indice_aperto());
    clicca(ESC_X, PIEDE_TASTI_Y);
    CHECK_FALSE(giro(b, m));
    CHECK_FALSE(m.indice_aperto());

    esc();
    CHECK(giro(b, m));   // Esc senza indice e senza riferimenti: fuori
}

TEST_CASE("Museo: ogni sezione si raggiunge con >, con la freccia e dall'indice, e si disegna" * doctest::skip(!CON_DATI)) {
    Banco& b = banco();
    SchermataMuseo m(b.enc, b.r, M688_DATI);
    giro(b, m);
    const int n = m.quante_sezioni();
    std::vector<std::string> gruppi;
    for (int s = 0; s < n; ++s) {
        REQUIRE(m.voce());
        CHECK(m.sezione() == s);
        gruppi.push_back(m.voce()->gruppo);
        clicca(DES_X, FRECCE_Y);
        giro(b, m);
    }
    CHECK(m.sezione() == 0);
    std::sort(gruppi.begin(), gruppi.end());
    CHECK(std::unique(gruppi.begin(), gruppi.end()) == gruppi.end());   // tutte diverse
    CHECK(gruppi.size() == b.enc.gruppi.size());                        // e nessuna manca

    for (int s = 1; s <= n; ++s) {
        destra();
        giro(b, m);
        CHECK(m.sezione() == s % n);
    }
    REQUIRE(n <= 24);   // le sezioni stanno tutte nel vetro
    for (int s = 0; s < n; ++s) {
        invio();
        giro(b, m);
        REQUIRE(m.indice_aperto());
        clicca(ELENCO_X, ELENCO_Y0 + s * PASSO);
        giro(b, m);
        CHECK(m.sezione() == s);
    }
}

TEST_CASE("Museo, basi: ogni base ha la carta della zona, col punto dentro e la terra vicina" * doctest::skip(!CON_DATI)) {
    Banco& b = banco();
    int basi = 0;
    for (const auto& [id, v] : b.enc.voci) {
        if (v.categoria != "base") continue;
        ++basi;
        CAPTURE(id);
        REQUIRE(v.immagini.size() >= 1);
        const ImmagineVoce& im = v.immagini.front();
        REQUIRE(im.tipo == "carta");
        CHECK(im.testo("file").rfind("basi/", 0) == 0);   // la sua, non quella del teatro che puo' non coprirla
        const Fondale carta = Fondale::carica(std::string(M688_DATI "/") + im.testo("file"));
        REQUIRE(carta.georeferenziato());
        const Geografiche g{im.numero("lat"), im.numero("lon")};
        const Vec2 p = carta.proiettata(g);
        // la vista del Museo, 150 km di larghezza nel riquadro 236x128: tutta dentro la carta
        const double mezza_x = 75000.0, mezza_y = 75000.0 * 128.0 / 236.0;
        const Vec2 a = carta.centro_cella(0, 0), z = carta.centro_cella(carta.larghezza() - 1, carta.altezza() - 1);
        CHECK(p.x - mezza_x >= std::min(a.x, z.x));
        CHECK(p.x + mezza_x <= std::max(a.x, z.x));
        CHECK(p.y - mezza_y >= std::min(a.y, z.y));
        CHECK(p.y + mezza_y <= std::max(a.y, z.y));
        // niente base in mare aperto: terra entro 4 km dal punto
        bool terra = false;
        for (double dy = -4000.0; dy <= 4000.0 && !terra; dy += 400.0)
            for (double dx = -4000.0; dx <= 4000.0 && !terra; dx += 400.0)
                terra = std::hypot(dx, dy) <= 4000.0 && carta.profondita({p.x + dx, p.y + dy}) <= 0.0;
        CHECK(terra);
        // e si vede: un'immagine vera, non il riquadro quasi tutto di un colore
        Tela t(236, 128);
        disegna_carta_museo(t, b.r.font, {0, 0, 236, 128}, carta, &g);
        std::map<std::uint8_t, int> colori;
        for (int y = 0; y < 128; ++y)
            for (int x = 0; x < 236; ++x) ++colori[t.at(x, y)];
        int massimo = 0;
        for (const auto& [c, n] : colori) massimo = std::max(massimo, n);
        CHECK(colori.size() > 4);
        CHECK(massimo < 236 * 128 * 9 / 10);
    }
    CHECK(basi == 31);
}

TEST_CASE("Museo, piede: Tab, Pag ed Esc col clic; Pag non conta oltre l'ultima pagina" * doctest::skip(!CON_DATI)) {
    Banco& b = banco();
    SchermataMuseo m(b.enc, b.r, M688_DATI);
    giro(b, m);
    REQUIRE(m.voce());
    REQUIRE(m.voce()->immagini.size() > 1);
    Tela prima = b.tela;
    clicca(TAB_X, PIEDE_TASTI_Y);
    giro(b, m);
    bool diversa = false;
    for (int y = 60; y < 190 && !diversa; ++y)
        for (int x = 220; x < 450 && !diversa; ++x) diversa = prima.at(x, y) != b.tela.at(x, y);
    CHECK(diversa);

    prima = b.tela;
    clicca(PAG_X, PIEDE_TASTI_Y);
    giro(b, m);
    diversa = false;
    for (int y = 240; y < 410 && !diversa; ++y)
        for (int x = 220; x < 620 && !diversa; ++x) diversa = prima.at(x, y) != b.tela.at(x, y);
    CHECK(diversa);

    // molte pressioni di Pag Giu' oltre la fine: basta una Pag Su per tornare indietro di una pagina
    for (int i = 0; i < 30; ++i) {
        premi(SDL_SCANCODE_PAGEDOWN, SDLK_PAGEDOWN);
        giro(b, m);
    }
    const Tela ultima = b.tela;
    premi(SDL_SCANCODE_PAGEUP, SDLK_PAGEUP);
    giro(b, m);
    diversa = false;
    for (int y = 240; y < 410 && !diversa; ++y)
        for (int x = 220; x < 620 && !diversa; ++x) diversa = ultima.at(x, y) != b.tela.at(x, y);
    CHECK(diversa);

    clicca(ESC_X, PIEDE_TASTI_Y);
    CHECK(giro(b, m));
}
