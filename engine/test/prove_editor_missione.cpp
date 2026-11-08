#include <doctest.h>

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <string>

#include "editor_missione.hpp"
#include "missioni/missione.hpp"
#include "missioni/missione_rapida.hpp"
#include "missioni/pacchetto_missione.hpp"
#include "missioni/scenario_missione.hpp"

using namespace m688;
namespace fs = std::filesystem;

// Schermata dell'editor di missione (app/editor_missione, fase E4) senza finestra: missione nuova, schede, unita'
// aggiunte e trascinate col mouse, testi, salvataggio e rilettura, prova, una rapida aperta nell'editor

namespace {

const std::string DATI = M688_DATI;
bool ci_sono_dati() { return fs::exists(DATI + "/missioni_rapide.toml") && fs::exists(DATI + "/font/originale.fnt"); }

const Risorse& risorse() {   // solo palette, font e classi: l'editor non usa sprite e modelli
    static const Risorse r{Palette::carica(DATI + "/palette/giorno.pal"), Font::carica(DATI + "/font/originale.fnt"),
                           carica_classi(DATI + "/classi.toml"), {}, {}};
    return r;
}

Comandi clic(int x, int y, bool destro = false) {
    Comandi c;
    (destro ? c.clic_destro : c.clic).push_back({x, y});
    c.mouse = {x, y};
    return c;
}

bool errori(const std::vector<AvvisoScenario>& v) {
    for (const auto& a : v)
        if (a.gravita == AvvisoScenario::Gravita::errore) return true;
    return false;
}

}  // namespace

TEST_CASE("editor: missione nuova, unita' col mouse, testi, salvataggio e rilettura" * doctest::skip(!ci_sono_dati())) {
    SchermataEditor e(risorse(), DATI);
    const auto cartella = fs::temp_directory_path() / "m688_prova_editor";
    fs::remove_all(cartella);
    e.cartella_di_salvataggio(cartella);
    REQUIRE(e.scenario().unita.size() == 1);
    CHECK(e.scenario().unita[0].giocatore);
    CHECK_FALSE(errori(e.avvisi()));
    // il 688 del 1989 parte senza esche semoventi (la MOSS e' what-if, del modo moderno)
    CHECK(std::none_of(e.scenario().carico.deposito.begin(), e.scenario().carico.deposito.end(),
                       [](const VoceCarico& v) { return v.arma == "esca_mobile"; }));
    Tela t;
    e.disegna(t, risorse().font);
    // scheda 2 (unita'), clic destro sulla carta vicino al giocatore: una Krivak II
    Comandi c;
    c.cifre = {2};
    e.comandi(c, 0.05);
    e.comandi(clic(250, 200, true), 0.05);
    REQUIRE(e.scenario().unita.size() == 2);
    CHECK(e.scenario().unita[1].classe == "krivak2");
    CHECK(e.scenario().unita[1].parte == "urss");
    CHECK(e.scenario().unita[1].comportamento == "R_nave");
    // trascinata col tasto sinistro
    const Vec2 prima = e.scenario().unita[1].pos;
    Comandi giu = clic(250, 200);
    giu.premuto = true;   // come la finestra: nel fotogramma del clic il tasto e' ancora giu'
    e.comandi(giu, 0.05);
    Comandi tieni;
    tieni.premuto = true;
    tieni.mouse = {270, 210};
    e.comandi(tieni, 0.05);
    Comandi lascia;
    lascia.rilascio.push_back({270, 210});
    lascia.mouse = {270, 210};
    e.comandi(lascia, 0.05);
    CHECK(e.scenario().unita[1].pos.x > prima.x);
    CHECK(e.scenario().unita[1].pos.y < prima.y);
    // scheda 3 (rotte): due punti per la Krivak
    c.cifre = {3};
    e.comandi(c, 0.05);
    e.comandi(clic(300, 150, true), 0.05);
    e.comandi(clic(320, 260, true), 0.05);
    CHECK(e.scenario().unita[1].punti.size() == 2);
    // scheda 1: il titolo si scrive (la seconda riga del pannello)
    c.cifre = {1};
    e.comandi(c, 0.05);
    e.comandi(clic(470, 72 + 10 + 3), 0.05);   // la riga del titolo, sotto l'intestazione LA MISSIONE
    REQUIRE(e.vuole_testo());
    Comandi testo;
    for (int i = 0; i < 20; ++i) {
        Comandi cancella;
        cancella.cancella = true;
        e.comandi(cancella, 0.05);
    }
    testo.testo = "Guardia a Suðuroy";
    e.comandi(testo, 0.05);
    Comandi invio;
    invio.invio = true;
    e.comandi(invio, 0.05);
    CHECK_FALSE(e.vuole_testo());
    CHECK(e.scenario().titolo[0] == "Guardia a Suðuroy");
    e.disegna(t, risorse().font);
    // salvataggio: id dal titolo, file giocabile col blocco editor
    REQUIRE(e.salva());
    CHECK(e.id() == "EDITOR_GUARDIA_A_SU_UROY");
    const auto f = cartella / (e.id() + ".lua");
    REQUIRE(fs::exists(f));
    std::ifstream in(f, std::ios::binary);
    const std::string file{std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>()};
    const auto s = leggi_scenario(file);
    REQUIRE(s);
    CHECK(s->unita.size() == 2);
    CHECK(s->unita[1].punti.size() == 2);
    const auto elenco = elenco_scenari(cartella);
    REQUIRE(elenco.size() == 1);
    CHECK(elenco[0].id == e.id());
    Missione m(f.string(), e.seme());
    for (int i = 0; i < 60; ++i) m.avanza(2.0);
    CHECK(m.errori().empty());
    CHECK(m.non_implementate().empty());
    CHECK(e.simula(300.0).find("NESSUN ERRORE") != std::string::npos);
    // E: il pacchetto da scambiare, che si importa
    e.cartella_di_esportazione(cartella / "esportate");
    Comandi esporta;
    esporta.lettere = {'E'};
    e.comandi(esporta, 0.05);
    REQUIRE(fs::exists(e.pacchetto()));
    std::ifstream pin(e.pacchetto(), std::ios::binary);
    const std::string pacchetto{std::istreambuf_iterator<char>(pin), std::istreambuf_iterator<char>()};
    const auto imp = importa_pacchetto(pacchetto, carica_dati_scenario(DATI), DATI, cartella / "importate");
    CHECK(imp.importato);
    CHECK(imp.id == e.id());
    // riaperta: lo stesso scenario
    SchermataEditor r(risorse(), DATI, *s);
    CHECK(r.id() == e.id());
    CHECK(r.scenario().unita.size() == 2);
    // G: salva e prova in mare
    Comandi g;
    g.lettere = {'G'};
    CHECK(r.comandi(g, 0.05) == SchermataEditor::Uscita::gioca);
    // Esc con modifiche: il primo avvisa, il secondo esce
    Comandi x;
    x.lettere = {'X'};
    r.scheda(SchermataEditor::Scheda::rotte);
    r.scegli_unita(1);
    r.comandi(x, 0.05);   // toglie l'ultimo punto della rotta
    CHECK(r.scenario().unita[1].punti.size() == 1);
    Comandi esc;
    esc.esc = true;
    CHECK(r.comandi(esc, 0.05) == SchermataEditor::Uscita::nessuna);
    CHECK(r.comandi(esc, 0.05) == SchermataEditor::Uscita::menu);
    fs::remove_all(cartella);
}

TEST_CASE("editor: una rapida aperta nell'editor e tutte le schede disegnate" * doctest::skip(!ci_sono_dati())) {
    ParametriRapida p;
    p.seme = 12;
    p.tipo = TipoRapida::antinave;
    const auto rapida = genera_missione_rapida(p, DATI);
    SchermataEditor e(risorse(), DATI, rapida.scenario);
    CHECK(e.scenario().libreria == 2);
    CHECK_FALSE(e.scenario().rapida);
    CHECK(e.id().rfind("EDITOR_RAPIDA_", 0) == 0);
    CHECK(e.scenario().unita.size() == rapida.scenario.unita.size());
    CHECK_FALSE(errori(e.avvisi()));
    Tela t;
    Palette pal = risorse().giorno;
    e.palette(pal);
    for (int k = 0; k < 8; ++k) {
        e.scheda(static_cast<SchermataEditor::Scheda>(k));
        e.scegli_unita(1);
        e.disegna(t, risorse().font);
    }
    CHECK(e.simula(120.0).find("NESSUN ERRORE") != std::string::npos);
}
