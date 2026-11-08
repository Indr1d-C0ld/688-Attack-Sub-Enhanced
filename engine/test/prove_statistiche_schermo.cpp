// Le schermate delle statistiche (app/statistiche.hpp; roadmap 6) su una traccia vera, registrata da SCENE4A
#include <doctest.h>

#include <cstdlib>
#include <filesystem>
#include <map>
#include <memory>
#include <string>

#include "../app/statistiche.hpp"
#include "grafica/tema_stazioni.hpp"
#include "missioni/danni_missione.hpp"
#include "missioni/missione.hpp"
#include "missioni/stazioni_missione.hpp"
#include "missioni/traccia_missione.hpp"
#include "nucleo/acustica.hpp"
#include "nucleo/classi.hpp"
#include "nucleo/fondale.hpp"
#include "nucleo/traccia_replay.hpp"

using namespace m688;
namespace fs = std::filesystem;

namespace {

bool c_e() { return fs::exists(M688_MISSIONI "/SCENE4A.lua") && fs::exists(M688_DATI "/teatri/gibilterra.fdl"); }

// 20 minuti di SCENE4A col moto reale, il periscopio alzato a 5 minuti e un ping a 10
const Traccia& traccia() {
    static const Traccia t = [] {
        Missione m(M688_MISSIONI "/SCENE4A.lua", 1, false);
        StazioniMissione st(m, 1);
        RegistratoreTraccia reg("SCENE4A", 1);
        reg.metadato("teatro", "gibilterra");
        bool periscopio = false, ping = false;
        for (int k = 0; k < 1200 * 9; ++k) {
            if (!periscopio && m.tempo() > 300.0) periscopio = m.applica(Ordine{m.tempo(), TipoOrdine::periscopio, 0, 0, 1});
            if (!ping && m.tempo() > 600.0) ping = m.applica(Ordine{m.tempo(), TipoOrdine::impulso_sonar});
            m.avanza(1.0 / 9.0);
            if (reg.serve(m.tempo(), m.eventi_missione().size())) {
                DatiApp app;
                app.stazione = 6;
                reg.aggiungi(fotogramma_da(m, &st, app, reg.ultimo_evento()), m.eventi_missione().size());
            }
        }
        return leggi_traccia(reg.dati());
    }();
    return t;
}

const CatalogoAcustico& acustica() {
    static const CatalogoAcustico a = carica_acustica(M688_DATI "/acustica.toml");
    return a;
}

int quanti_fra(const Tela& t, Rettangolo r, int da, int a) {
    int n = 0;
    for (int y = r.y; y < r.y + r.h; ++y)
        for (int x = r.x; x < r.x + r.w; ++x) {
            const int c = t.dati()[y * LARGHEZZA_TELA + x];
            n += c >= da && c <= a;
        }
    return n;
}

}  // namespace

TEST_CASE("statistiche, schermo: la missione con quota, livello, armi, scoperta e heatmap sulla carta" * doctest::skip(!c_e())) {
    const ReportMissione r = report_missione(traccia(), &acustica());
    REQUIRE(r.durata_s > 1000.0);
    REQUIRE_FALSE(r.heatmap.empty());
    SchermataStatistiche s(M688_DATI, r, traccia());
    CHECK(s.teatro() == "gibilterra");
    Tela t;
    Palette p;
    s.palette(p);
    s.disegna(t, Font::carica(M688_DATI "/font/originale.fnt"));
    CHECK(quanti_fra(t, {322, 32, 312, 206}, 232, 247) > 20);              // le celle della heatmap
    CHECK(quanti_fra(t, {322, 32, 312, 206}, 96, 143) > 1000);             // il fondale sotto
    CHECK(quanti_fra(t, {36, 34, 274, 86}, colori::VERDE_CHIARO, colori::VERDE_CHIARO) > 50);   // la quota
    if (!r.livello_irradiato.empty())
        CHECK(quanti_fra(t, {36, 152, 274, 86}, colori::GIALLO, colori::GIALLO) > 50);          // il livello irradiato
    // la rampa del calore va dal blu al rosso
    CHECK(p.rgb(232)[2] > p.rgb(232)[0]);
    CHECK(p.rgb(247)[0] > p.rgb(247)[2]);
    // Esc e Invio escono; il resto no
    Comandi c;
    CHECK_FALSE(s.comandi(c, 0.1));
    c.esc = true;
    CHECK(s.comandi(c, 0.1));
}

TEST_CASE("statistiche, schermo: le cumulate con totali, primati e la heatmap di un teatro alla volta" * doctest::skip(!c_e())) {
    const ReportMissione r = report_missione(traccia(), &acustica());
    StatisticheCumulate cum;
    accumula(cum, r);
    accumula(cum, r);
    ReportMissione altrove = r;   // la stessa missione in un altro teatro: due heatmap
    altrove.teatro = "faroe";
    accumula(cum, altrove);
    REQUIRE(cum.missioni == 3);
    SchermataStatisticheCumulate s(M688_DATI, cum, "globali");
    CHECK(s.teatro() == "gibilterra");   // dove si e' passato piu' tempo
    Tela t;
    const Font f = Font::carica(M688_DATI "/font/originale.fnt");
    s.disegna(t, f);
    CHECK(quanti_fra(t, {322, 32, 312, 246}, 232, 247) > 20);
    Comandi tab;
    tab.tab = 1;
    CHECK_FALSE(s.comandi(tab, 0.1));
    CHECK(s.teatro() == "faroe");
    s.comandi(tab, 0.1);
    CHECK(s.teatro() == "gibilterra");
    // senza dati: niente heatmap, e la schermata si disegna lo stesso
    SchermataStatisticheCumulate vuota(M688_DATI, StatisticheCumulate{}, "comandante nuovo");
    CHECK(vuota.teatro().empty());
    vuota.disegna(t, f);
    Comandi invio;
    invio.invio = true;
    CHECK(vuota.comandi(invio, 0.1));
}

// La pagina «IMPRESE» (docs/26 §3 e il secondo giro del 31/10/2026): Tab dal report, 1 e 2 nelle cumulate; con
// M688_ISTANTANEE=cartella le due pagine in BMP
TEST_CASE("statistiche, schermo: la pagina delle imprese nel report e nelle cumulate" * doctest::skip(!c_e())) {
    ReportMissione r = report_missione(traccia(), &acustica());
    r.sotto_banchisa_s = 1830.0, r.urti_ghiaccio = 1, r.avvisi_chiglia = 4;
    r.visive_notte_fanali = 2, r.visive_luci_spente = 1, r.luci_accese_s = 300.0;
    r.in_coda_s = 2700.0, r.distanza_min_sottomarino_m = 640.0, r.classificati_da_sottomarini = 1;
    r.mine_per_sensore = {{"bqs15", 7}, {"mg70", 2}}, r.mine_ormeggiate = 6, r.mine_di_fondo = 3, r.mine_aggirate = 5;
    r.sorvoli_elicotteri = 3, r.sorvoli_aerei = 2, r.siluri_aerei_evitati = 1, r.salvati = 40;
    r.classi_giuste = {"kidd", "spruance", "kara"};
    SchermataStatistiche s(M688_DATI, r, traccia());
    const Font f = Font::carica(M688_DATI "/font/originale.fnt");
    Palette p;
    s.palette(p);
    Tela prima, imprese;
    s.disegna(prima, f);
    Comandi tab;
    tab.tab = 1;
    CHECK_FALSE(s.comandi(tab, 0.1));
    s.disegna(imprese, f);
    CHECK(quanti_fra(imprese, {0, 0, 640, 420}, 40, 47) > 2000);   // i due schermi a fosforo
    CHECK(quanti_fra(imprese, {0, 0, 640, 420}, 232, 247) == 0);   // niente heatmap: e' l'altra pagina
    CHECK(quanti_fra(prima, {322, 32, 312, 206}, 232, 247) > 20);
    CHECK_FALSE(s.comandi(tab, 0.1));                              // Tab torna al report
    Tela di_nuovo;
    s.disegna(di_nuovo, f);
    CHECK(quanti_fra(di_nuovo, {322, 32, 312, 206}, 232, 247) > 20);
    // le cumulate: 2 le imprese, Tab torna alle statistiche (non cambia teatro)
    StatisticheCumulate cum;
    accumula(cum, r);
    accumula(cum, r);
    CHECK(cum.salvati == 80);
    CHECK(cum.classi_giuste.size() == 3);
    CHECK(cum.in_coda_max_s == doctest::Approx(2700.0));
    SchermataStatisticheCumulate c(M688_DATI, cum, "globali");
    Comandi due;
    due.cifre = {2};
    CHECK_FALSE(c.comandi(due, 0.1));
    Tela cumulate;
    c.disegna(cumulate, f);
    CHECK(quanti_fra(cumulate, {0, 0, 640, 420}, 232, 247) == 0);
    const std::string teatro = c.teatro();
    CHECK_FALSE(c.comandi(tab, 0.1));
    CHECK(c.teatro() == teatro);
    if (const char* d = std::getenv("M688_ISTANTANEE")) {   // sulla palette di base del gioco, coi colori fondamentali
        Palette pr = Palette::carica(M688_DATI "/immagini/MAIN.pal"), pc = pr;
        s.palette(pr);
        c.palette(pc);
        imprese.salva_bmp(std::string(d) + "/statistiche_imprese.bmp", pr);
        cumulate.salva_bmp(std::string(d) + "/statistiche_cumulate_imprese.bmp", pc);
    }
}

// Le statistiche tecniche della sandbox (docs/19 §4) su S5 nell'Enhanced: la traccia v2 con la raccolta dei fatti dell'IA
namespace {
bool c_e_s5() { return fs::exists(M688_MISSIONI "/prove/PROVA_S5_CONVOGLIO.lua") && fs::exists(M688_DATI "/teatri/celtica.fdl"); }
const Traccia& traccia_s5() {
    static const Traccia t = [] {
        const auto classi = carica_classi(M688_DATI "/classi.toml");
        Missione m(M688_MISSIONI "/prove/PROVA_S5_CONVOGLIO.lua", 1);
        std::map<std::string, double> pescaggi;
        for (const auto& [k, c] : classi) pescaggi[k] = c.pescaggio_m;
        m.imposta_fondale(std::make_shared<const Fondale>(Fondale::carica(M688_DATI "/teatri/celtica.fdl")), pescaggi);
        m.radio_enhanced();
        DanniMissione dm(classi, M688_DATI, 1, RegolaDanni::simulazione);
        StazioniMissione st(m, 1);
        RegistratoreTraccia reg("PROVA_S5_CONVOGLIO", 1);
        RaccoltaTraccia raccolta;
        // il Victor lancia verso 760 s, dopo due gambe di TMA (la TMA dell'IA corretta il 28/10: prima lanciava a 333 s
        // su una soluzione sbagliata di 2 km), il colpo verso 870-950 s
        for (int k = 0; k < 1000 * 9; ++k) {
            m.avanza(1.0 / 9.0);
            dm.avanza(m, 1.0 / 9.0);
            m.raccogli_danni(nullptr, &dm);
            if (reg.serve(m.tempo(), m.eventi_missione().size()))
                reg.aggiungi(fotogramma_da(m, &st, DatiApp{}, reg.ultimo_evento(), &raccolta), m.eventi_missione().size());
        }
        return leggi_traccia(reg.dati());
    }();
    return t;
}
}  // namespace

TEST_CASE("statistiche, schermo: la sandbox in cinque pagine, Tab, frecce e il replay da una riga" * doctest::skip(!c_e_s5())) {
    SchermataStatisticheSandbox s(M688_DATI, traccia_s5());
    CHECK(s.statistiche().complete);
    CHECK(s.pagina() == 0);
    const Font f = Font::carica(M688_DATI "/font/originale.fnt");
    Tela t;
    Palette p;
    s.palette(p);
    for (int k = 0; k < SchermataStatisticheSandbox::PAGINE; ++k) {
        CAPTURE(k);
        CHECK(s.pagina() == k);
        t.riempi(0);
        s.disegna(t, f);
        CHECK(quanti_fra(t, {0, 36, 640, 390}, fosforo(4), fosforo(7)) > 200);   // la tabella ha del testo, sul fosforo
        Comandi tab;
        tab.tab = 1;
        CHECK_FALSE(s.comandi(tab, 0.1));
    }
    CHECK(s.pagina() == 0);
    s.pagina(1);   // le catene di scoperta: ce ne sono, e un clic su una riga da' l'istante del replay
    CHECK(s.righe() > 3);
    s.pagina(2);   // le armi: almeno i due siluri del Victor
    CHECK(s.righe() >= 2);
    s.disegna(t, f);
    REQUIRE_FALSE(s.zone().empty());
    const Rettangolo r = s.zone().front().first;   // la prima riga sotto l'intestazione
    Comandi clic;
    clic.clic = {{r.x + 60, r.y + r.h / 2}};
    CHECK(s.comandi(clic, 0.1));
    REQUIRE(s.replay_da());
    CHECK(*s.replay_da() > 0.0);
    Comandi esc;
    esc.esc = true;
    CHECK(SchermataStatisticheSandbox(M688_DATI, traccia_s5()).comandi(esc, 0.1));
    // i clic: la terza linguetta, il tasto TAB del piede (la pagina seguente), il tasto ESC
    SchermataStatisticheSandbox s2(M688_DATI, traccia_s5());
    s2.disegna(t, f);
    Comandi c;
    c.clic = {{170, 52}};
    for (int x = 20; x < 380 && s2.pagina() == 0; x += 4) {
        c.clic = {{x, 52}};
        s2.comandi(c, 0.1);
    }
    CHECK(s2.pagina() == 1);
    c.clic = {{160, 460}};
    CHECK_FALSE(s2.comandi(c, 0.1));
    CHECK(s2.pagina() == 2);
    c.clic = {{480, 460}};
    CHECK(s2.comandi(c, 0.1));
}
