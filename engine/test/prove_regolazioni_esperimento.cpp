#include <doctest.h>

#include <algorithm>
#include <cstdlib>
#include <filesystem>
#include <string>
#include <vector>

#include "../app/codice_partita.hpp"
#include "../app/esperimento.hpp"
#include "../app/pannello_interventi.hpp"
#include "../app/regolazioni_sandbox.hpp"
#include "grafica/font.hpp"
#include "grafica/palette.hpp"
#include "grafica/tela.hpp"
#include "missioni/interventi.hpp"

using namespace m688;
namespace fs = std::filesystem;

// La pagina «interventi programmati» delle regolazioni del vetrino (app/regolazioni_sandbox, docs/28 §5-6): Tab fra le
// due pagine, un intervento composto col pannello e poi l'ora, togliere, i campi dell'esperimento, salvare e caricare un
// .m688v (dell'utente e pronto), l'uscita per il confronto A/B. La prima pagina resta com'era.

namespace {

std::vector<UnitaEsperimento> unita() {
    return {{{0, "LOS ANGELES (0)", "NATO", "sottomarino"}, {0.0, -60000.0}},
            {{2, "PERRY (2)", "NATO", "nave"}, {0.0, 0.0}},
            {{3, "VICTOR III (3)", "PATTO", "sottomarino"}, {8000.0, 0.0}}};
}

// le unita' della scena S5 del convoglio, quella degli esperimenti pronti (data/missioni/prove/PROVA_S5_CONVOGLIO.lua)
std::vector<UnitaEsperimento> unita_s5() {
    return {{{0, "LOS ANGELES (0)", "NATO", "sottomarino"}, {55130.0, -16898.0}},
            {{2, "MERCANTILE (2)", "NATO", "nave"}, {50960.0, -15469.0}},
            {{3, "CARGO (3)", "NATO", "nave"}, {50543.0, -13106.0}},
            {{4, "PETROLIERA (4)", "NATO", "nave"}, {52437.0, -15209.0}},
            {{8, "PERRY (8)", "NATO", "nave"}, {45827.0, -15156.0}},
            {{9, "PERRY (9)", "NATO", "nave"}, {50057.0, -10348.0}},
            {{10, "PERRY (10)", "NATO", "nave"}, {51446.0, -18227.0}},
            {{11, "SH60B (11)", "NATO", "aereo"}, {50022.0, -10151.0}},
            {{12, "VICTOR3 (12)", "PATTO", "sottomarino"}, {52967.0, -13897.0}}};
}

const std::vector<std::pair<std::string, std::string>> CLASSI = {{"perry", "Oliver H. Perry"}, {"spruance", "Spruance"}};

Comandi tasto(int tab = 0, bool invio = false, int ruota = 0) {
    Comandi c;
    c.tab = tab;
    c.invio = invio;
    c.ruota = ruota;
    return c;
}

Comandi lettera(char l) {
    Comandi c;
    c.lettere.push_back(l);
    return c;
}

Comandi esc() {
    Comandi c;
    c.esc = true;
    return c;
}

Comandi clic(int x, int y) {
    Comandi c;
    c.clic.push_back({x, y});
    return c;
}

// il codice sandbox di una missione finta con l'impronta data
std::function<std::string(const RegolazioniSandbox&)> codice(std::uint32_t impronta) {
    return [impronta](const RegolazioniSandbox& r) {
        CodiceSandbox k;
        k.partita.impronta = impronta;
        k.partita.seme = 5;
        k.regolazioni = r;
        return scrivi_codice_sandbox(k);
    };
}

fs::path cartella_vuota(const std::string& nome) {
    const fs::path p = fs::temp_directory_path() / nome;
    fs::remove_all(p);
    fs::create_directories(p);
    return p;
}

// N, poi dal gruppo `g` al tipo `t` del pannello (Invio sul gruppo, Tab fino al tipo, Invio)
void scegli(SchermataRegolazioni& s, const std::string& g, const std::string& t) {
    const auto cat = catalogo_pannello(catalogo_interventi(), CLASSI);
    std::vector<std::string> gruppi;
    for (const auto& v : cat)
        if (std::find(gruppi.begin(), gruppi.end(), v.gruppo) == gruppi.end()) gruppi.push_back(v.gruppo);
    s.comandi(lettera('N'));
    const int ig = static_cast<int>(std::find(gruppi.begin(), gruppi.end(), g) - gruppi.begin());
    if (ig > 0) s.comandi(tasto(ig));
    s.comandi(tasto(0, true));
    int it = 0;
    for (const auto& v : cat) {
        if (v.gruppo != g) continue;
        if (v.tipo == t) break;
        ++it;
    }
    if (it > 0) s.comandi(tasto(it));
    s.comandi(tasto(0, true));
}

// la schermata sulla seconda pagina, con le unita' e le classi
SchermataRegolazioni seconda_pagina() {
    SchermataRegolazioni s("SCENE1A", RegolazioniSandbox{});
    s.imposta_missione(unita(), CLASSI);
    s.comandi(tasto(1));
    return s;
}

// toglie la Victor (3) col pannello: «fra» a 2 minuti, poi l'ora a 3 minuti
void aggiungi_togli(SchermataRegolazioni& s) {
    scegli(s, "FORZE", "togli");
    s.comandi(tasto(2));                    // la Victor
    s.comandi(tasto(0, true));
    for (int i = 0; i < 2; ++i) s.comandi(tasto(0, false, 1), 0.2);   // fra: 2 minuti
    s.comandi(tasto(1));                    // APPLICA
    s.comandi(tasto(0, true));
    s.comandi(tasto(0, false, 1));          // l'ora: un minuto in piu'
    s.comandi(tasto());
    s.comandi(tasto(0, true));
}

}  // namespace

TEST_CASE("regolazioni del vetrino: la prima pagina come prima, Tab alla seconda e ritorno") {
    SchermataRegolazioni s("SCENE1A", RegolazioniSandbox{});
    CHECK(s.pagina() == SchermataRegolazioni::Pagina::regolazioni);
    CHECK(s.comandi(tasto(0, false, 1)) == SchermataRegolazioni::Uscita::nessuna);   // la situazione della NATO
    CHECK(s.regolazioni().nato.situazione == SituazioneScelta::pace);
    CHECK(s.comandi(tasto(0, true)) == SchermataRegolazioni::Uscita::avvia);
    CHECK(s.comandi(lettera('S')) == SchermataRegolazioni::Uscita::serie);
    CHECK(s.comandi(esc()) == SchermataRegolazioni::Uscita::indietro);
    s.comandi(tasto(1));
    CHECK(s.pagina() == SchermataRegolazioni::Pagina::interventi);
    CHECK(s.comandi(lettera('S')) == SchermataRegolazioni::Uscita::nessuna);   // qui S salva: chiede il nome
    CHECK(s.vuole_testo());
    s.comandi(esc());
    CHECK_FALSE(s.vuole_testo());
    CHECK(s.comandi(esc()) == SchermataRegolazioni::Uscita::nessuna);   // Esc: di nuovo le regolazioni, non indietro
    CHECK(s.pagina() == SchermataRegolazioni::Pagina::regolazioni);
    s.comandi(clic(600, 12));   // la scheda in alto a destra
    CHECK(s.pagina() == SchermataRegolazioni::Pagina::interventi);
    s.comandi(clic(600, 12));
    CHECK(s.pagina() == SchermataRegolazioni::Pagina::regolazioni);
}

TEST_CASE("regolazioni del vetrino: un intervento col pannello, poi l'ora; togliere; il confronto A/B") {
    SchermataRegolazioni s = seconda_pagina();
    CHECK(s.comandi(lettera('A')) == SchermataRegolazioni::Uscita::nessuna);   // niente interventi: niente confronto
    CHECK(s.avviso().find("NESSUN INTERVENTO") != std::string::npos);
    aggiungi_togli(s);
    Esperimento e = s.esperimento();
    REQUIRE(e.interventi.size() == 1);
    CHECK(e.interventi[0].tipo == "togli");
    CHECK(e.interventi[0].unita == 3);
    CHECK(e.interventi[0].tempo == doctest::Approx(180.0));
    CHECK(e.interventi[0].id == -1);
    CHECK(e.semi == 10);
    CHECK(s.avviso().find("03:00") != std::string::npos);
    // un secondo prima: in testa alla lista, e la riga resta su di lui
    scegli(s, "FORZE", "togli");
    s.comandi(tasto(1));                    // la Perry
    s.comandi(tasto(0, true));
    s.comandi(tasto(1));
    s.comandi(tasto(0, true));              // fra 0: l'ora parte da 00:00
    s.comandi(tasto(0, true));
    e = s.esperimento();
    REQUIRE(e.interventi.size() == 2);
    CHECK(e.interventi[0].unita == 2);
    CHECK(e.interventi[0].tempo == 0.0);
    CHECK(e.interventi[1].unita == 3);
    // destra sulla riga scelta: un minuto in piu', l'ordine si rifa'
    s.comandi(tasto(0, false, 1));
    s.comandi(tasto());
    s.comandi(tasto(0, false, 1));
    s.comandi(tasto());
    s.comandi(tasto(0, false, 1));
    s.comandi(tasto());
    s.comandi(tasto(0, false, 1));
    s.comandi(tasto());
    e = s.esperimento();
    CHECK(e.interventi[0].unita == 3);
    CHECK(e.interventi[1].unita == 2);
    CHECK(e.interventi[1].tempo == doctest::Approx(240.0));
    CHECK(s.comandi(lettera('T')) == SchermataRegolazioni::Uscita::nessuna);   // toglie la Perry, la scelta
    e = s.esperimento();
    REQUIRE(e.interventi.size() == 1);
    CHECK(e.interventi[0].unita == 3);
    CHECK(s.comandi(lettera('A')) == SchermataRegolazioni::Uscita::esperimento);
    // Esc nell'ora: l'intervento composto non entra
    scegli(s, "FORZE", "togli");
    s.comandi(tasto(0, true));
    s.comandi(tasto(1));
    s.comandi(tasto(0, true));
    s.comandi(esc());
    CHECK(s.esperimento().interventi.size() == 1);
}

TEST_CASE("regolazioni del vetrino: un'unita' nuova col punto sulla cartina delle unita'") {
    SchermataRegolazioni s = seconda_pagina();
    scegli(s, "FORZE", "unita_nuova");
    // la cartina: tutte le unita' col margine, il centro a meta' fra (0, -60000) e (8000, 0); il clic al centro
    s.comandi(clic(16 + 176, 156 + 24 + 100));
    s.comandi(tasto(-1));   // dall'inizio dei parametri all'ultima voce: APPLICA
    s.comandi(tasto(0, true));
    s.comandi(tasto(0, true));   // l'ora: 00:00
    const Esperimento e = s.esperimento();
    REQUIRE(e.interventi.size() == 1);
    CHECK(e.interventi[0].tipo == "unita_nuova");
    REQUIRE(e.interventi[0].punto);
    CHECK(e.interventi[0].punto->x == doctest::Approx(4000.0));
    CHECK(e.interventi[0].punto->y == doctest::Approx(-30000.0));
    CHECK(e.interventi[0].testi.at("classe") == "perry");
}

TEST_CASE("regolazioni del vetrino: i campi, salvare e caricare un .m688v, l'altra missione rifiutata") {
    const fs::path utente = cartella_vuota("m688_prova_regolazioni_esperimento");
    SchermataRegolazioni s = seconda_pagina();
    s.imposta_cartelle(utente, {});
    s.imposta_codice(codice(0xABCD));
    s.comandi(tasto(0, true));   // il titolo
    REQUIRE(s.vuole_testo());
    Comandi t;
    t.testo = "Scorta senza Victor";
    s.comandi(t);
    s.comandi(tasto(0, true));
    CHECK_FALSE(s.vuole_testo());
    CHECK(s.esperimento().titolo == "Scorta senza Victor");
    Comandi giu;
    giu.su_giu = -1;
    for (int i = 0; i < 3; ++i) {   // fino ai semi
        s.comandi(giu);
        s.comandi(tasto());
    }
    s.comandi(tasto(0, false, 1));
    CHECK(s.esperimento().semi == 11);
    aggiungi_togli(s);
    // una regolazione cambiata nella prima pagina entra nel codice
    s.comandi(tasto(1));
    for (int i = 0; i < 19; ++i) {   // alla durata
        s.comandi(giu);
        s.comandi(tasto());
    }
    s.comandi(tasto(0, false, -1));
    CHECK(s.regolazioni().durata_min == 105);
    s.comandi(tasto(1));
    s.comandi(lettera('S'));   // il nome dal titolo
    s.comandi(tasto(0, true));
    const fs::path file = utente / "scorta_senza_victor.m688v";
    REQUIRE(fs::exists(file));
    const Esperimento letto = leggi_esperimento(file);
    CHECK(letto.titolo == "Scorta senza Victor");
    CHECK(letto.semi == 11);
    REQUIRE(letto.interventi.size() == 1);
    CHECK(letto.interventi[0].tempo == doctest::Approx(180.0));
    const auto k = leggi_codice_sandbox(letto.codice);
    REQUIRE(k);
    CHECK(k->partita.impronta == 0xABCD);
    CHECK(k->regolazioni.durata_min == 105);

    // caricato in una schermata nuova: interventi, campi e le regolazioni del codice anche nella prima pagina
    SchermataRegolazioni r = seconda_pagina();
    r.imposta_cartelle(utente, {});
    r.imposta_codice(codice(0xABCD));
    r.comandi(lettera('C'));
    r.comandi(tasto(0, true));
    const Esperimento e = r.esperimento();
    CHECK(e.titolo == "Scorta senza Victor");
    CHECK(e.semi == 11);
    REQUIRE(e.interventi.size() == 1);
    CHECK(e.interventi[0].unita == 3);
    CHECK(e.codice == letto.codice);
    CHECK(r.regolazioni().durata_min == 105);

    // la stessa missione ma un'altra impronta: rifiutato
    SchermataRegolazioni a = seconda_pagina();
    a.imposta_cartelle(utente, {});
    a.imposta_codice(codice(0x1234));
    a.comandi(lettera('C'));
    a.comandi(tasto(0, true));
    CHECK(a.esperimento().interventi.empty());
    CHECK(a.avviso().find("ALTRA MISSIONE") != std::string::npos);
    fs::remove_all(utente);
}

TEST_CASE("regolazioni del vetrino: gli esperimenti pronti si caricano; il disegno in tutti i passi") {
    const fs::path utente = cartella_vuota("m688_prova_regolazioni_pronti");
    const fs::path pronti = fs::path(M688_DATI) / "esperimenti";
    SchermataRegolazioni s("PROVA_S5_CONVOGLIO", RegolazioniSandbox{});   // la missione dei pronti, con le sue unita'
    s.imposta_missione(unita_s5(), CLASSI);
    s.comandi(tasto(1));
    s.imposta_cartelle(utente, pronti);
    const Font f = Font::carica(M688_DATI "/font/originale.fnt");
    Tela t;
    int n_istantanea = 0;
    const auto istantanea = [&](const char* nome) {   // con M688_ISTANTANEE: le schermate da guardare
        const char* d = std::getenv("M688_ISTANTANEE");
        if (!d) return;
        Palette p = Palette::carica(M688_DATI "/palette/giorno.pal");
        s.palette(p);
        t.salva_bmp(std::string(d) + "/regolazioni_" + std::to_string(++n_istantanea) + "_" + nome + ".bmp", p);
    };
    s.disegna(t, f);
    istantanea("vuota");
    s.comandi(lettera('C'));
    s.disegna(t, f);
    istantanea("carica");
    const auto file = elenco_esperimenti(pronti);
    if (!file.empty()) {   // li scrive trailer-1: la cartella puo' mancare
        s.comandi(tasto(0, true));
        const Esperimento e = s.esperimento();
        const Esperimento atteso = leggi_esperimento(file.front());
        CHECK(e.titolo == atteso.titolo);
        CHECK(e.interventi.size() == atteso.interventi.size());
        CHECK(e.codice == atteso.codice);
        for (const auto& x : e.interventi) CHECK(x.tempo >= 0.0);
    } else {
        CHECK(s.avviso().find("NESSUN ESPERIMENTO") != std::string::npos);
    }
    s.disegna(t, f);
    istantanea("caricato");
    scegli(s, "FORZE", "unita_nuova");   // la cartina e il pannello
    s.disegna(t, f);
    istantanea("cartina");
    s.comandi(esc());
    s.comandi(esc());
    s.comandi(esc());
    s.comandi(tasto(0, true));   // il campo o l'ora, secondo la riga
    s.disegna(t, f);
    istantanea("scrittura");
    s.comandi(esc());
    s.comandi(tasto(1));
    s.disegna(t, f);   // la prima pagina
    istantanea("prima_pagina");
    fs::remove_all(utente);
}
