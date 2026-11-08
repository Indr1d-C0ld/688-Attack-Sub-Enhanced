#include <doctest.h>

#include <algorithm>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <string>

#include "carta.hpp"
#include "grafica/font.hpp"
#include "grafica/palette.hpp"
#include "grafica/tela.hpp"
#include "missioni/contatti_giocatore.hpp"
#include "missioni/danni_missione.hpp"
#include "missioni/missione.hpp"
#include "nucleo/classi.hpp"
#include "nucleo/fondale.hpp"

using namespace m688;
namespace fs = std::filesystem;

// Carta dell'Enhanced (app/carta.cpp; docs/11_comandi_e_stazioni.md §11, N1-N2): grafica VGA/SVGA e strumenti

namespace {

bool ci_sono_dati() { return fs::exists(std::string(M688_DATI) + "/teatri/faroe.fdl"); }

std::string scena() {
    const auto f = fs::temp_directory_path() / "m688_prova_carta.lua";
    const std::string modello = "modello = { massima_kn = 32, virata_gradi_s = 1, accel_kn_s = 0.5, quota_m_s = 1, quota_max_m = 450, integrita = 200 }";
    std::ofstream(f) << "return { id = 'PROVA', teatro = 'faroe', script = {}, paesaggio_sonoro = false, varianti = { { probabilita = 1, unita = {\n"
                        "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true,"
                        "  posizione = { x = 0, y = 0 }, quota_m = 150, rotta = 0, velocita_kn = 5, " << modello << " },"
                        "{ id = 2, classe = 'charlie2', categoria = 'sottomarino', flag = 25, posizione = { x = 0, y = 3000 },"
                        "  quota_m = 150, rotta = 90, velocita_kn = 8, " << modello << " },"
                        "\n}, timer = {} } } }\n";
    return f.string();
}

int conta(const Tela& t, int da, int a) {
    int n = 0;
    for (int y = 0; y < 360; ++y)
        for (int x = 0; x < LARGHEZZA_TELA; ++x) n += t.at(x, y) >= da && t.at(x, y) <= a;
    return n;
}

}  // namespace

TEST_CASE("carta enhanced: fondale a fasce con le ombre, rosa, righello, linee, eventi e datum" * doctest::skip(!ci_sono_dati())) {
    const auto f = scena();
    const auto classi = carica_classi(M688_DATI "/classi.toml");
    Missione m(f, 1, false);
    m.imposta_fondale(std::make_shared<const Fondale>(Fondale::carica(M688_DATI "/teatri/faroe.fdl")), {});
    DanniMissione dm(classi, M688_DATI, 1, RegolaDanni::simulazione);
    for (int i = 0; i < 9 * 30; ++i) {
        m.avanza(1.0 / 9.0);
        dm.avanza(m, 1.0 / 9.0);
    }
    const auto* g = m.giocatore();
    REQUIRE(g);
    CartaGiocatore c;
    c.restyle = true;
    c.anelli = 2;
    c.righello.aperto = true;
    c.righello.a = g->pos;
    c.righello.b = {g->pos.x + 3000.0, g->pos.y + 2000.0};
    c.lucido.aggiungi_linea({g->pos, 10.0, 0.0, "A", 1});
    c.lucido.aggiungi_linea({{g->pos.x + 500.0, g->pos.y}, 5.0, 120.0, "S1", 2});
    c.lucido.evento(Evento::Tipo::segno, {g->pos.x - 2000.0, g->pos.y - 1000.0}, 60.0);
    c.lucido.aggiungi_datum({{g->pos.x + 4000.0, g->pos.y + 4000.0}, 0.0, m.metri_al_secondo(30.0), "A"});
    c.lucido.aggiungi_segno({{g->pos.x + 2500.0, g->pos.y - 2500.0}, Segno::Affiliazione::ostile, Segno::Dominio::subacqueo, "VICTOR?", 0.0});
    c.lucido.aggiungi_segno({{g->pos.x - 3000.0, g->pos.y + 2500.0}, Segno::Affiliazione::amico, Segno::Dominio::superficie, "P-3", 0.0});
    c.lucido.aggiungi_zona({{g->pos.x - 4000.0, g->pos.y - 3500.0}, 1500.0, true});
    c.lucido.aggiungi_linea({g->pos, 20.0, 60.0, "A", 1});
    c.ekelund = {true, "A", 3000.0, 90.0, 8.0};
    c.tempo_rilevamento = true;
    c.contorno_sicurezza_m = 100.0;   // la piattaforma delle Faroe attraversa i 100 m
    const Font font = Font::carica(M688_DATI "/font/originale.fnt");
    Tela t;
    disegna_carta(t, font, m, c, segni_contatti(m, false), -1, std::nullopt);
    // l'acqua nelle fasce con le ombre, la terra o la costa in qualche punto, le matite e l'evidenza del righello
    CHECK(conta(t, colori_carta::ACQUA, colori_carta::ACQUA + 47) > 100000);
    CHECK(conta(t, colori_carta::EVIDENZA, colori_carta::EVIDENZA) > 20);
    CHECK(conta(t, colori_carta::MATITA, colori_carta::MATITA + 5) > 200);
    CHECK(conta(t, colori_carta::ZONA + 2, colori_carta::ZONA + 2) > 20);   // il datum
    CHECK(conta(t, colori_carta::ZONA, colori_carta::ZONA) > 20);           // la zona da evitare
    CHECK(conta(t, colori_carta::NTDS, colori_carta::NTDS + 1) > 10);       // i simboli amico e ostile
    CHECK(conta(t, colori_carta::SICUREZZA, colori_carta::SICUREZZA) > 50); // il contorno di sicurezza
    Palette pal = Palette::carica(M688_DATI "/palette/giorno.pal");
    applica_palette_carta(pal, StileCarta::vga);
    CHECK(pal.rgb(colori_carta::ACQUA + 45)[2] < pal.rgb(colori_carta::ACQUA + 1)[2]);   // il profondo e' piu' scuro
    if (const char* d = std::getenv("M688_ISTANTANEE")) t.salva_bmp(std::string(d) + "/carta_strumenti.bmp", pal);
    // con la carta d'epoca della parte: il cartiglio in magenta e il reticolo dei gradi
    const std::string carta = percorso_carta(M688_DATI, "faroe", Parte::nato);
    if (fs::exists(carta)) {
        m.imposta_fondale_carta(Parte::nato, std::make_shared<const Fondale>(Fondale::carica(carta)));
        Tela t2;
        disegna_carta(t2, font, m, c, segni_contatti(m, false), -1, std::nullopt);
        CHECK(conta(t2, colori_carta::MAGENTA, colori_carta::MAGENTA) > 50);
        CHECK(conta(t2, colori_carta::RETICOLO, colori_carta::RETICOLO) > 200);
    }
    fs::remove(f);
}

// Il campo minato della sua parte sulla carta (la I17 di trailer-1, 30/10/2026): una Mk 67 dello scenario della parte del
// giocatore si disegna (simbolo e «NOTA»); una nemica, mai vista dal BQS-15, no. Le due carte differiscono solo li'
TEST_CASE("carta enhanced: le mine dello scenario della sua parte si vedono, quelle nemiche no" * doctest::skip(!ci_sono_dati())) {
    const auto carta_con = [](const std::string& lato) {
        const auto f = fs::temp_directory_path() / ("m688_prova_carta_mine_" + lato + ".lua");
        const std::string modello = "modello = { massima_kn = 32, virata_gradi_s = 1, accel_kn_s = 0.5, quota_m_s = 1, quota_max_m = 450, integrita = 200 }";
        std::ofstream(f) << "return { id = 'PROVA_MINE', teatro = 'faroe', script = {}, paesaggio_sonoro = false,"
                            " mine = { { tipo = 'mk67', x = 1500, y = 1500, lato = '" << lato << "', armata = true } },"
                            " varianti = { { probabilita = 1, unita = {\n"
                            "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true,"
                            "  posizione = { x = 0, y = 0 }, quota_m = 150, rotta = 0, velocita_kn = 5, " << modello << " },"
                            "\n}, timer = {} } } }\n";
        const auto classi = carica_classi(M688_DATI "/classi.toml");
        Missione m(f.string(), 1, false);
        m.imposta_fondale(std::make_shared<const Fondale>(Fondale::carica(M688_DATI "/teatri/faroe.fdl")), {});
        DanniMissione dm(classi, M688_DATI, 1, RegolaDanni::simulazione);
        for (int i = 0; i < 9 * 5; ++i) {
            m.avanza(1.0 / 9.0);
            dm.avanza(m, 1.0 / 9.0);
        }
        CartaGiocatore c;
        c.restyle = true;
        const Font font = Font::carica(M688_DATI "/font/originale.fnt");
        Tela t;
        disegna_carta(t, font, m, c, segni_contatti(m, false), -1, std::nullopt);
        fs::remove(f);
        return t;
    };
    const Tela amica = carta_con("usa"), nemica = carta_con("urss");
    int diversi = 0;
    for (int y = 0; y < 360; ++y)
        for (int x = 0; x < LARGHEZZA_TELA; ++x) diversi += amica.at(x, y) != nemica.at(x, y);
    MESSAGE("pixel diversi fra la carta con la mina amica e quella con la nemica: ", diversi);
    CHECK(diversi > 30);   // il simbolo, la scritta MK67 NOTA e il cerchio d'innesco dell'armata
}
