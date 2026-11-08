#include <doctest.h>

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <map>
#include <memory>
#include <regex>
#include <set>
#include <string>
#include <tuple>

#include "../app/data_missione.hpp"
#include "missioni/danni_missione.hpp"
#include "missioni/danni_sottomarini.hpp"
#include "missioni/missione.hpp"
#include "missioni/missione_rapida.hpp"
#include "missioni/scenario_missione.hpp"
#include "nucleo/classi.hpp"
#include "nucleo/fondale.hpp"
#include "nucleo/ordini.hpp"
#include "nucleo/radio.hpp"

using namespace m688;
namespace fs = std::filesystem;

// Scenario di missione (missioni/scenario_missione.hpp, editor fasi E1-E3): le rapide dallo scenario con gli stessi
// byte, il blocco `editor` che si rilegge e riscrive uguale, la libreria dei comportamenti 2 nei due livelli, le regole
// «quando... allora...» e la verifica

namespace {

const std::string DATI = M688_DATI;
bool ci_sono_dati() { return fs::exists(DATI + "/missioni_rapide.toml") && fs::exists(DATI + "/teatri/faroe.fdl"); }

std::uint64_t fnv(const std::string& s) {
    std::uint64_t h = 1469598103934665603ull;
    for (const unsigned char c : s) { h ^= c; h *= 1099511628211ull; }
    return h;
}

std::string scrivi(const std::string& id, const std::string& testo) {
    const auto f = fs::temp_directory_path() / (id + ".lua");
    std::ofstream(f, std::ios::binary) << testo;
    return f.string();
}

const DatiScenario& dati() {
    static const DatiScenario d = carica_dati_scenario(DATI);
    return d;
}

// Uno scenario dell'editor da una rapida: libreria 2, niente origine rapida, qualche parametro per unita'
ScenarioMissione da_rapida(std::uint32_t seme, TipoRapida tipo, const std::string& teatro = "") {
    ParametriRapida p;
    p.seme = seme;
    p.tipo = tipo;
    p.teatro = teatro;
    ScenarioMissione s = genera_missione_rapida(p, DATI).scenario;
    s.rapida.reset();
    s.libreria = 2;
    s.id = "EDITOR_PROVA_" + std::to_string(seme);
    s.autore = "prova";
    for (auto& u : s.unita)
        if (u.categoria == "sottomarino" && !u.giocatore) {
            u.parametri.salve = 4;
            u.parametri.paranoia = 3;
        } else if (u.comportamento == "R_nave") {
            u.parametri.portata_nm = 1.5;
        }
    return s;
}

struct Enhanced {
    std::map<std::string, ClasseNave> classi = carica_classi(DATI + "/classi.toml");
    Missione m;
    DanniMissione dm;
    DanniSottomarini ds;
    Enhanced(const std::string& file, std::uint64_t seme)
        : m(file, seme), dm(classi, DATI, seme, RegolaDanni::simulazione), ds(classi, DATI, seme) {
        std::map<std::string, double> pescaggi;
        for (const auto& [id, c] : classi) pescaggi[id] = c.pescaggio_m;
        m.imposta_fondale(std::make_shared<const Fondale>(Fondale::carica(DATI + "/teatri/" + m.teatro() + ".fdl")), pescaggi);
        if (const auto* g = m.giocatore()) ds.prepara(m, g->id);
    }
    void avanza(double s) {
        constexpr double DT = 1.0 / 9.0;
        for (long i = 0; i < static_cast<long>(s * 9.0); ++i) {
            m.avanza(DT);
            dm.avanza(m, DT);
            ds.avanza(m, DT);
            m.raccogli_danni(&ds, &dm);
        }
    }
};

CondizioneScenario cond(CondizioneScenario::Tipo t, double valore = 0.0) {
    CondizioneScenario c;
    c.tipo = t;
    c.valore = valore;
    return c;
}

ObiettivoScenario obiettivo(const std::string& id, const std::string& tipo, std::vector<int> unita = {}) {
    ObiettivoScenario o;
    o.id = id;
    o.tipo = tipo;
    o.unita = std::move(unita);
    return o;
}

bool ha_messaggio(const Missione& m, int codice) {
    for (const auto& x : m.messaggi())
        if (x.codice == codice) return true;
    return false;
}

bool errori(const std::vector<AvvisoScenario>& v, const std::string& codice = "") {
    for (const auto& a : v)
        if (a.gravita == AvvisoScenario::Gravita::errore && (codice.empty() || a.codice == codice)) return true;
    return false;
}

}  // namespace

// Riferimento della versione 1 (25/10/2026): i file delle rapide per 40 codici. Se dati o generatore cambiano di
// proposito l'uscita delle rapide, si alza VERSIONE_RAPIDE (i codici vecchi vengono rifiutati) e si rifa' l'elenco.
// Eccezione, solo testo: il 26/10/2026 «Mare di Ochotsk» e' diventato «Mare di Okhotsk» nei briefing (stessa
// generazione; le otto impronte delle Curili rifatte e verificate da tools/rapide/okhotsk/verifica_impronte.py).
TEST_CASE("scenario: le rapide passano dallo scenario con gli stessi byte della versione 1" * doctest::skip(!ci_sono_dati())) {
    static const std::pair<const char*, std::uint64_t> RIFERIMENTO[] = {
        {"7M01-SEKF-EYG9S", 0xa628e1906861cb71ull}, {"4P9Q-92TE-R7XDS", 0x52869bff62b8decfull},
        {"7MSV-JCS3-B628C", 0xb8e385f102945093ull}, {"6P4T-9Y0P-V9RAB", 0xed31c4ba2bb49582ull},
        {"4RH3-RSM1-3TK0T", 0x299e17049f26b2e2ull}, {"6GGE-VCG0-CSBDF", 0x80dbf2c3b89c72c8ull},
        {"6358-4S5T-ZC3SC", 0xaba69c680c5e6f9aull}, {"5R8Z-X130-9NTJT", 0x553330f9a6cc9589ull},
        {"40GT-GCM5-B4Y3V", 0xd902fb098fed9396ull}, {"54C1-HB6M-ZJES1", 0xfc9173b16f7734e9ull},
        {"6P0R-9GKX-YB4RW", 0xcaf6d7372c0e2a60ull}, {"64M7-SPRR-D0Z30", 0x78b4e08b813fb2feull},
        {"7K5J-9J2V-NB4P4", 0xa940ed1177ab58bdull}, {"7JNX-V282-1BDJK", 0x6a754e497a238df0ull},
        {"6M99-16RJ-HS4EX", 0x1f52bfccc619f7deull}, {"54D8-9VYP-48EZ4", 0x9da970ddf034ec0eull},
        {"765H-KPX4-01Y7E", 0x1b72eb7806d32ef1ull}, {"7408-B97M-NJWD2", 0xc0ebf110c20fe3e4ull},
        {"705H-MVKR-G8DP6", 0x1f85194d9cdd7838ull}, {"56WV-REQ4-PZVS9", 0x98ebf10ceea5c81eull},
        {"534V-031H-5Y1HH", 0x0b094ab4034f5de3ull}, {"6JXE-K4VJ-H3RCR", 0x04766ca47f8edc22ull},
        {"7RSF-981W-9ANMF", 0x93d7641bedde8eabull}, {"700H-CY12-Z21QG", 0x08d479a4a8d58bf8ull},
        {"42G7-8R39-MZJA7", 0x1bfb6e86f16e1280ull}, {"7803-3C2J-2J2KC", 0x70f70ca3d659b443ull},
        {"7PM0-2S7S-2AMNX", 0xd9bdf03ba7bcce57ull}, {"421B-C2MX-DVNK2", 0xb74c9448a2fe3ddaull},
        {"7JSY-B9M3-MBGP4", 0xe3c237e4d7754af4ull}, {"7950-HMRX-Y9DAY", 0x41ef11aaadd89b47ull},
        {"7M5V-8ZWM-EQGHX", 0x82b326cadbdb69daull}, {"6MRJ-2QA7-Q27HY", 0x21fff4fbe13f2177ull},
        {"4S4S-NTZP-YNSMV", 0xbc5dd21eda127aefull}, {"6J8C-TAH6-A3Z07", 0x8df69b159bb6a8bcull},
        {"765R-NZE0-K6BAH", 0x94f3d56638476f7bull}, {"54MK-S3M7-79ZDC", 0xbc1049d55b663858ull},
        {"494Q-58RE-302Q7", 0x8a3f997587830ed4ull}, {"6JWG-42K8-AFASZ", 0x499c88117f3b4cc9ull},
        {"470E-0HV2-06REE", 0x14b9c3257b8913f5ull}, {"5P51-RF2W-R1KCY", 0x1557a08c27ba9db1ull},
    };
    for (const auto& [codice, impronta] : RIFERIMENTO) {
        CAPTURE(codice);
        const auto p = leggi_codice(codice);
        REQUIRE(p);
        const auto m = genera_missione_rapida(*p, DATI);
        CHECK(fnv(m.lua) == impronta);
        // lo scenario riscritto da' lo stesso file, e le rapide non hanno il blocco editor
        CHECK(scrivi_missione_lua(m.scenario, dati()) == m.lua);
        CHECK_FALSE(leggi_scenario(m.lua));
    }
}

TEST_CASE("scenario: il blocco editor si rilegge e riscrive lo stesso file" * doctest::skip(!ci_sono_dati())) {
    for (int k = 1; k <= 7; ++k) {
        ScenarioMissione s = da_rapida(100u + static_cast<std::uint32_t>(k), static_cast<TipoRapida>(k));
        s.zone.push_back({"porto", {"il porto", "the harbour"}, {s.unita[0].pos.x + 5000.0, s.unita[0].pos.y, 3000.0}});
        RegolaScenario r;
        r.id = "avviso";
        r.quando.push_back(cond(CondizioneScenario::Tipo::tempo, 600.0));
        AzioneScenario a;
        a.tipo = AzioneScenario::Tipo::messaggio;
        a.codice = 35;
        r.allora.push_back(a);
        s.regole.push_back(r);
        const std::string testo = scrivi_missione_lua(s, dati());
        const auto riletto = leggi_scenario(testo);
        REQUIRE(riletto);
        CAPTURE(k);
        CHECK(riletto->id == s.id);
        CHECK(riletto->unita.size() == s.unita.size());
        CHECK(riletto->unita[1].pos.x == s.unita[1].pos.x);   // numeri esatti nel blocco
        CHECK(riletto->regole.size() == 1);
        CHECK(riletto->zone.at(0).nome[1] == "the harbour");
        CHECK(scrivi_missione_lua(*riletto, dati()) == testo);
    }
}

TEST_CASE("scenario: le missioni originali non hanno il blocco editor") {
    const std::string f = std::string(M688_MISSIONI) + "/SCENE1A.lua";
    if (!fs::exists(f)) return;
    std::ifstream in(f, std::ios::binary);
    const std::string testo{std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>()};
    CHECK_FALSE(leggi_scenario(testo));
    CHECK_THROWS_AS(leggi_scenario("return 1 +"), std::runtime_error);
    CHECK_THROWS_AS(leggi_scenario("while true do end"), std::runtime_error);   // limite di istruzioni
    CHECK_FALSE(leggi_scenario("return { editor = { formato_editor = 1 } }"));
    CHECK_THROWS_AS(leggi_scenario("return { editor = { formato_editor = 99, scenario = {} } }"), std::runtime_error);
    CHECK_THROWS_AS(leggi_scenario("return io.open('x')"), std::runtime_error);    // niente io nell'ambiente isolato
}

TEST_CASE("scenario: la libreria 2 coi parametri per unita' gira nei due livelli" * doctest::skip(!ci_sono_dati())) {
    for (int k = 1; k <= 7; ++k) {
        const ScenarioMissione s = da_rapida(200u + static_cast<std::uint32_t>(k), static_cast<TipoRapida>(k));
        const std::string testo = scrivi_missione_lua(s, dati());
        bool sottomarini = false;
        for (const auto& u : s.unita) sottomarini = sottomarini || (u.categoria == "sottomarino" && !u.giocatore);
        CHECK((testo.find("salve = 4") != std::string::npos) == sottomarini);
        const std::string f = scrivi(s.id, testo);
        CAPTURE(k);
        Missione m(f, 7);
        for (int i = 0; i < 300; ++i) m.avanza(2.0);   // 10 minuti
        CHECK(m.errori().empty());
        CHECK(m.non_implementate().empty());
        Enhanced e(f, 7);
        e.avanza(90.0);
        CHECK(e.m.errori().empty());
        CHECK(e.m.non_implementate().empty());
        fs::remove(f);
    }
}

TEST_CASE("scenario: regole, condizioni e azioni" * doctest::skip(!ci_sono_dati())) {
    ScenarioMissione s = da_rapida(31, TipoRapida::pattuglia, "faroe");
    s.situazione = "pace";
    s.aerei_asw.clear();
    const Vec2 g = s.unita[0].pos;
    // un'unita' di riserva: una Krivak II che entra quando il giocatore arriva nella zona «arrivo»
    UnitaScenario riserva = s.unita[1];
    riserva.id = static_cast<int>(s.unita.size());
    riserva.classe = "krivak2";
    riserva.categoria = "nave";
    riserva.parte = "urss";
    riserva.flag = 0x12;
    riserva.comportamento = "R_nave";
    riserva.punti.clear();
    riserva.pos = {g.x + 3000.0, g.y};
    riserva.velocita = 8.0;
    riserva.entra_con = "ingresso";
    s.unita.push_back(riserva);
    s.zone.push_back({"arrivo", {"arrivo", "arrival"}, {g.x + 20000.0, g.y, 2000.0}});
    const auto regola = [&](const std::string& id, std::vector<CondizioneScenario> q, std::vector<AzioneScenario> a, bool una = true) {
        RegolaScenario r;
        r.id = id;
        r.quando = std::move(q);
        r.allora = std::move(a);
        r.una_volta = una;
        s.regole.push_back(r);
    };
    using C = CondizioneScenario::Tipo;
    using A = AzioneScenario::Tipo;
    AzioneScenario msg;
    msg.tipo = A::messaggio;
    msg.codice = 35;
    AzioneScenario var;
    var.tipo = A::variabile;
    var.indice = 3;
    var.valore = 5;
    regola("primo", {cond(C::tempo, 60.0)}, {msg, var});
    CondizioneScenario dopo_primo = cond(C::regola_scattata);
    dopo_primo.regola = "primo";
    CondizioneScenario cinque = cond(C::variabile, 5.0);
    cinque.indice = 3;
    AzioneScenario allarme;
    allarme.tipo = A::allarme;
    AzioneScenario piu;
    piu.tipo = A::variabile;
    piu.indice = 4;
    piu.operazione = "+";
    piu.valore = 2;
    regola("secondo", {dopo_primo, cinque}, {allarme, piu});
    CondizioneScenario dentro = cond(C::giocatore_in_zona);
    dentro.zona = "arrivo";
    AzioneScenario entra;
    entra.tipo = A::entra;
    entra.unita = riserva.id;
    AzioneScenario cue;
    cue.tipo = A::cue_asw;
    cue.punto = g;
    AzioneScenario vai;
    vai.tipo = A::vai_a;
    vai.unita = 1;
    vai.punto = {g.x, g.y - 10000.0};
    AzioneScenario nessuno;
    nessuno.tipo = A::comportamento;
    nessuno.unita = 1;
    regola("ingresso", {dentro}, {entra, cue, vai, nessuno});
    CondizioneScenario fuori = cond(C::giocatore_fuori_zona);
    fuori.zona = "arrivo";
    CondizioneScenario dopo_ingresso = cond(C::regola_scattata);
    dopo_ingresso.regola = "ingresso";
    AzioneScenario fine;
    fine.tipo = A::fine;
    fine.codice = 40;
    fine.categoria = "successo";
    fine.testo = {"Fatto.", "Done."};
    regola("fine", {dopo_ingresso, fuori, cond(C::allarme), cond(C::unita_colpita)}, {fine});   // unita' colpita: mai, qui
    s.regole.back().quando[3].unita = 1;
    regola("fine_vera", {dopo_ingresso, fuori, cond(C::allarme)}, {fine});
    REQUIRE_FALSE(errori(verifica(s, Fondale::carica(DATI + "/teatri/faroe.fdl"), dati())));
    const std::string testo = scrivi_missione_lua(s, dati());
    CHECK(testo.find("categorie_esiti = { [40] = \"successo\" }") != std::string::npos);
    const std::string f = scrivi(s.id, testo);
    Missione m(f, 3);
    // la riserva aspetta fuori dalla carta
    REQUIRE(m.unita().count(riserva.id));
    CHECK(m.unita().at(riserva.id).pos.y > g.y + 1.0e6);
    for (int i = 0; i < 40; ++i) m.avanza(2.0);
    CHECK(ha_messaggio(m, 35));
    CHECK(m.variabili().at(303) == 5);
    for (int i = 0; i < 10; ++i) m.avanza(2.0);
    CHECK(m.variabili().at(11) == 1);
    CHECK(m.variabili().at(304) == 2);   // + 2, una volta sola
    // il giocatore entra nella zona: la riserva entra in scena
    REQUIRE(m.comando("posizione", {g.x + 20000.0, g.y}));
    for (int i = 0; i < 10; ++i) m.avanza(2.0);
    REQUIRE(m.unita().count(riserva.id));
    CHECK(distanza(m.unita().at(riserva.id).pos, riserva.pos) < 3000.0);
    CHECK(m.unita().at(1).g_bersaglio.empty());   // nessun comportamento
    CHECK_FALSE(m.esito());
    // ed esce: esito 40
    REQUIRE(m.comando("posizione", {g.x, g.y}));
    for (int i = 0; i < 10 && !m.esito(); ++i) m.avanza(2.0);
    REQUIRE(m.esito());
    CHECK(*m.esito() == 40);
    CHECK(m.testo_esito(40) == "Fatto.");
    CHECK(m.esito_dettagliato().categoria == EsitoDettagliato::Categoria::successo);
    CHECK(m.errori().empty());
    CHECK(m.non_implementate().empty());
    // nell'Enhanced la riserva resta parcheggiata (anche col fondale vero) finche' la regola non scatta
    Enhanced e(f, 3);
    e.avanza(120.0);
    CHECK(e.m.errori().empty());
    REQUIRE(e.m.unita().count(riserva.id));
    CHECK(e.m.unita().at(riserva.id).pos.y > g.y + 1.0e6);
    REQUIRE(e.m.comando("posizione", {g.x + 20000.0, g.y}));
    e.avanza(30.0);
    CHECK(distanza(e.m.unita().at(riserva.id).pos, riserva.pos) < 3000.0);
    CHECK(e.m.errori().empty());
    fs::remove(f);
}

TEST_CASE("scenario: obiettivo «variabile» e velocita' del «vai a»" * doctest::skip(!ci_sono_dati())) {
    ScenarioMissione s = da_rapida(31, TipoRapida::pattuglia, "faroe");
    s.situazione = "pace";
    s.aerei_asw.clear();
    // una nave dell'IA ferma, senza rotta: a 60 s parte a 10 nodi verso sud e la variabile 2 sale a 3
    auto nave = std::find_if(s.unita.begin(), s.unita.end(), [](const UnitaScenario& u) { return !u.giocatore && u.categoria == "nave"; });
    REQUIRE(nave != s.unita.end());
    nave->velocita = 0.0;
    nave->punti.clear();
    nave->comportamento = "";
    const int id = nave->id;
    const Vec2 p0 = nave->pos;
    RegolaScenario r;
    r.id = "parti";
    r.quando = {cond(CondizioneScenario::Tipo::tempo, 60.0)};
    AzioneScenario vai;
    vai.tipo = AzioneScenario::Tipo::vai_a;
    vai.unita = id;
    vai.punto = {p0.x, p0.y - 20000.0};
    vai.velocita = 10.0;
    AzioneScenario piu;
    piu.tipo = AzioneScenario::Tipo::variabile;
    piu.indice = 2;
    piu.operazione = "+";
    piu.valore = 3;
    r.allora = {vai, piu};
    s.regole.push_back(r);
    ObiettivoScenario o = obiettivo("conto", "variabile", {});
    o.indice = 2;
    o.minimo = 3;
    s.obiettivi.push_back(o);
    const Fondale f = Fondale::carica(DATI + "/teatri/faroe.fdl");
    REQUIRE_FALSE(errori(verifica(s, f, dati())));
    const std::string testo = scrivi_missione_lua(s, dati());
    CHECK(testo.find("indice = 302, almeno = 3") != std::string::npos);
    const auto riletto = leggi_scenario(testo);
    REQUIRE(riletto);
    CHECK(riletto->obiettivi.back().indice == 2);
    CHECK(riletto->regole.back().allora.front().velocita == 10.0);
    CHECK(scrivi_missione_lua(*riletto, dati()) == testo);
    const std::string file = scrivi(s.id + "_variabile", testo);
    Missione m(file, 3);
    for (int i = 0; i < 20; ++i) m.avanza(2.0);
    CHECK(m.unita().at(id).velocita < 0.5);
    const auto conto = [&] {
        for (const auto& x : m.esito_dettagliato().obiettivi)
            if (x.id == "conto") return x.stato;
        return Obiettivo::Stato::fallito;
    };
    CHECK(conto() != Obiettivo::Stato::raggiunto);
    for (int i = 0; i < 150; ++i) m.avanza(2.0);
    CHECK(m.unita().at(id).velocita > 5.0);
    CHECK(m.unita().at(id).pos.y < p0.y - 500.0);
    CHECK(m.variabili().at(302) == 3);
    CHECK(conto() == Obiettivo::Stato::raggiunto);
    CHECK(m.errori().empty());
    fs::remove(file);
    // senza la variabile l'obiettivo non si scrive come si deve: errore
    s.obiettivi.back().indice = -1;
    CHECK(errori(verifica(s, f, dati()), "obiettivo_variabile"));
}

TEST_CASE("scenario: le condizioni delle rapide v2 e l'obiettivo «contatto»" * doctest::skip(!ci_sono_dati())) {
    ScenarioMissione s = da_rapida(31, TipoRapida::pattuglia, "faroe");
    s.situazione = "pace";
    s.aerei_asw.clear();
    const Vec2 g = s.unita[0].pos;
    auto nave = std::find_if(s.unita.begin(), s.unita.end(), [](const UnitaScenario& u) { return !u.giocatore && u.categoria == "nave"; });
    REQUIRE(nave != s.unita.end());
    const int id = nave->id;
    nave->pos = {g.x + 4000.0, g.y};
    nave->velocita = 0.0;
    nave->punti.clear();
    nave->comportamento = "";
    s.zone.push_back({"qui", {"qui", "here"}, {g.x, g.y, 1500.0}});
    ObiettivoScenario arrivo = obiettivo("arrivo", "raggiungere");
    arrivo.cerchio = CerchioScenario{g.x, g.y, 1500.0};
    arrivo.primario = false;
    s.obiettivi.push_back(arrivo);
    ObiettivoScenario sentita = obiettivo("sentita", "contatto", {id});
    sentita.minuti = 1.0;
    sentita.primario = false;
    s.obiettivi.push_back(sentita);
    using C = CondizioneScenario::Tipo;
    const auto regola = [&](const std::string& rid, std::vector<CondizioneScenario> q, int indice) {
        RegolaScenario r;
        r.id = rid;
        r.quando = std::move(q);
        AzioneScenario a;
        a.tipo = AzioneScenario::Tipo::variabile;
        a.indice = indice;
        a.valore = 1;
        r.allora = {a};
        s.regole.push_back(r);
    };
    CondizioneScenario lento = cond(C::giocatore_sotto_nodi, 40.0);
    CondizioneScenario quota = cond(C::giocatore_a_quota, 0.0);
    quota.fino_a = 1000.0;
    CondizioneScenario compiuto = cond(C::obiettivo_compiuto);
    compiuto.obiettivo = "arrivo";
    CondizioneScenario dentro = cond(C::giocatore_in_zona);
    dentro.zona = "qui";
    CondizioneScenario lancio = cond(C::ostile_ha_lanciato);
    CondizioneScenario ping = cond(C::ping_vicino, 10000.0);
    ping.unita = id;
    CondizioneScenario contatto = cond(C::contatto_su, 0.5);
    contatto.unita = id;
    regola("lento", {lento, quota, dentro}, 0);
    regola("compiuto", {compiuto}, 1);
    regola("lancio", {lancio}, 2);
    regola("ping", {ping}, 3);
    regola("contatto", {contatto}, 4);
    regola("superficie", {cond(C::giocatore_in_superficie)}, 5);
    CondizioneScenario vede = cond(C::unita_vede, 0.5);   // la nave ferma sente il giocatore
    vede.unita = id;
    regola("vede", {vede}, 6);
    CondizioneScenario non_vede = cond(C::unita_non_vede, 0.5);   // e non vede se stessa: mai, qui
    non_vede.unita = id;
    non_vede.bersaglio = s.unita[0].id;
    regola("non_vede", {non_vede, cond(C::tempo, 1e9)}, 7);
    const Fondale f = Fondale::carica(DATI + "/teatri/faroe.fdl");
    for (const auto& a : verifica(s, f, dati())) CHECK_MESSAGE(a.gravita != AvvisoScenario::Gravita::errore, a.testo);
    const std::string testo = scrivi_missione_lua(s, dati());
    CHECK(testo.find("local function su_unita") != std::string::npos);
    CHECK(testo.find("m688.in_cerchio(u, table.unpack(ZONE[\"qui\"]))") != std::string::npos);
    CHECK(testo.find("tipo = \"contatto\", unita = { " + std::to_string(id) + " }, primario = false, minuti = 1") != std::string::npos);
    const auto riletto = leggi_scenario(testo);
    REQUIRE(riletto);
    CHECK(scrivi_missione_lua(*riletto, dati()) == testo);
    CHECK(riletto->regole[0].quando[1].fino_a == 1000.0);
    CHECK(riletto->regole[1].quando[0].obiettivo == "arrivo");
    CHECK(riletto->obiettivi.back().minuti == 1.0);
    const std::string file = scrivi(s.id + "_v2", testo);
    Missione m(file, 3);
    const auto var = [&](int i) { const auto it = m.variabili().find(300 + i); return it == m.variabili().end() ? 0 : it->second; };
    for (int i = 0; i < 30; ++i) m.avanza(2.0);
    CHECK(var(0) == 1);   // lento, a quota, nella zona
    CHECK(var(1) == 1);   // l'obiettivo «arrivo» e' raggiunto subito
    CHECK(var(2) == 0);   // nessun lancio in pace
    CHECK(var(3) == 0);   // nessun ping
    CHECK(var(5) == 0);   // in immersione
    REQUIRE(m.applica(Ordine{m.tempo(), TipoOrdine::impulso_sonar}));
    for (int i = 0; i < 30; ++i) m.avanza(2.0);
    CHECK(var(3) == 1);   // il ping a 4 km dalla nave
    for (int i = 0; i < 60 && var(4) == 0; ++i) m.avanza(2.0);
    CHECK(var(4) == 1);   // mezzo minuto di contatto sulla nave ferma a 4 km
    for (int i = 0; i < 120 && var(6) == 0; ++i) m.avanza(2.0);
    CHECK(var(6) == 1);   // la nave ha sentito il giocatore per mezzo minuto (la domanda passa dalla nave)
    CHECK(m.errori().empty());
    CHECK(m.non_implementate().empty());
    fs::remove(file);
    // gli errori della verifica
    {
        ScenarioMissione t = s;
        t.regole[1].quando[0].obiettivo = "nessuno";
        CHECK(errori(verifica(t, f, dati()), "regola_obiettivo"));
    }
    {
        ScenarioMissione t = s;
        t.obiettivi.back().minuti = 0.0;
        CHECK(errori(verifica(t, f, dati()), "obiettivo_minuti"));
    }
    {   // lo stato del giocatore e la zona di un'altra unita' nella stessa regola
        ScenarioMissione t = s;
        CondizioneScenario altra = cond(C::unita_in_zona);
        altra.unita = id;
        altra.zona = "qui";
        t.regole[0].quando = {lento, altra};
        CHECK(errori(verifica(t, f, dati()), "regola_soggetti"));
    }
}

TEST_CASE("scenario: l'AGI e il peschereccio del kit hanno il modello del classico e girano nei due livelli" * doctest::skip(!ci_sono_dati())) {
    REQUIRE(dati().modelli.count("agi"));
    REQUIRE(dati().modelli.count("peschereccio"));
    CHECK(dati().pescaggi.at("agi") == doctest::Approx(3.7));
    CHECK(dati().pescaggi.at("peschereccio") == doctest::Approx(5.0));
    ScenarioMissione s = da_rapida(5, TipoRapida::pattuglia, "faroe");
    const Vec2 g = s.unita[0].pos;
    int id = 0;
    for (const auto& u : s.unita) id = std::max(id, u.id + 1);
    const auto aggiungi = [&](const std::string& classe, const std::string& parte, int flag, double est) {
        UnitaScenario u;
        u.id = id++;
        u.classe = classe;
        u.categoria = "nave";
        u.parte = parte;
        u.flag = flag;
        u.non_combattente = true;
        u.pos = {g.x + est, g.y + 3000.0};
        u.rotta = 90.0;
        u.velocita = 6.0;
        s.unita.push_back(u);
        return u.id;
    };
    const int agi = aggiungi("agi", "urss", 0x52, 2000.0);
    const int pesca = aggiungi("peschereccio", "neutrale", 0x42, -2000.0);
    const Fondale f = Fondale::carica(DATI + "/teatri/faroe.fdl");
    for (const auto& a : verifica(s, f, dati())) CHECK_MESSAGE(a.gravita != AvvisoScenario::Gravita::errore, a.testo);
    const std::string file = scrivi(s.id + "_kit", scrivi_missione_lua(s, dati()));
    {
        Missione m(file, 1);
        for (int i = 0; i < 300; ++i) m.avanza(2.0);
        CHECK(m.unita().count(agi));
        CHECK(m.unita().count(pesca));
        CHECK(m.errori().empty());
        CHECK(m.non_implementate().empty());
    }
    {
        Enhanced e(file, 1);
        e.avanza(600.0);
        CHECK(e.m.unita().count(agi));
        CHECK(e.m.unita().count(pesca));
        CHECK(e.m.errori().empty());
    }
    fs::remove(file);
}

TEST_CASE("scenario: la verifica trova terra, rotte, riferimenti e soggetti sbagliati" * doctest::skip(!ci_sono_dati())) {
    const Fondale f = Fondale::carica(DATI + "/teatri/faroe.fdl");
    // nessun errore sulle rapide (al piu' attenzioni)
    for (std::uint32_t seme = 1; seme <= 40; ++seme) {
        ParametriRapida p;
        p.seme = seme;
        const auto m = genera_missione_rapida(p, DATI);
        const Fondale ft = Fondale::carica(DATI + "/teatri/" + m.parametri.teatro + ".fdl");
        CAPTURE(m.codice);
        for (const auto& a : verifica(m.scenario, ft, dati())) CHECK_MESSAGE(a.gravita != AvvisoScenario::Gravita::errore, a.testo);
    }
    // un punto di terra sulla carta delle Faer Oer
    Vec2 terra{};
    for (double y = -150000.0; y < 150000.0 && terra.x == 0.0; y += 2000.0)
        for (double x = -150000.0; x < 150000.0; x += 2000.0)
            if (f.profondita({x, y}) < -50.0) { terra = {x, y}; break; }
    REQUIRE(terra.x != 0.0);
    ScenarioMissione s = da_rapida(5, TipoRapida::pattuglia, "faroe");
    CHECK_FALSE(errori(verifica(s, f, dati())));
    {
        ScenarioMissione t = s;
        t.unita[1].pos = terra;
        t.unita[1].categoria = "nave";
        CHECK(errori(verifica(t, f, dati()), "terra"));
    }
    {
        ScenarioMissione t = s;
        t.unita[0].punti = {terra};
        bool rotta = false;
        for (const auto& a : verifica(t, f, dati())) rotta = rotta || a.codice == "rotta_su_terra";
        CHECK(rotta);
    }
    {
        ScenarioMissione t = s;
        t.unita.erase(t.unita.begin());
        CHECK(errori(verifica(t, f, dati()), "senza_giocatore"));
        CHECK_THROWS_AS(scrivi_missione_lua(t, dati()), std::invalid_argument);
    }
    {
        ScenarioMissione t = s;
        t.obiettivi.push_back(obiettivo("x", "affondare", {999}));
        CHECK(errori(verifica(t, f, dati()), "obiettivo_unita"));
        t.obiettivi.back() = obiettivo("y", "raggiungere");
        CHECK(errori(verifica(t, f, dati()), "obiettivo_senza_zona"));
    }
    {
        ScenarioMissione t = s;
        t.zone.push_back({"a", {}, {0, 0, 1000}});
        RegolaScenario r;
        r.id = "mista";
        CondizioneScenario c1 = cond(CondizioneScenario::Tipo::giocatore_in_zona);
        c1.zona = "a";
        CondizioneScenario c2 = cond(CondizioneScenario::Tipo::unita_in_zona);
        c2.zona = "a";
        c2.unita = 1;
        r.quando = {c1, c2};
        AzioneScenario a;
        a.tipo = AzioneScenario::Tipo::allarme;
        r.allora = {a};
        t.regole.push_back(r);
        CHECK(errori(verifica(t, f, dati()), "regola_soggetti"));
        t.regole.back().quando = {c1};
        t.regole.back().quando[0].zona = "b";
        CHECK(errori(verifica(t, f, dati()), "regola_zona"));
        CHECK_THROWS_AS(scrivi_missione_lua(t, dati()), std::invalid_argument);
    }
}

TEST_CASE("scenario: «nuova da una missione originale» per le 22 missioni" * doctest::skip(!ci_sono_dati())) {
    const std::string cartella = M688_MISSIONI;
    if (!fs::exists(cartella + "/SCENE1A.lua")) return;
    int n = 0;
    for (const auto& e : fs::directory_iterator(cartella)) {
        const std::string nome = e.path().filename().string();
        if (e.path().extension() != ".lua" || nome.rfind("SCENE", 0) != 0) continue;
        ++n;
        const std::string id = e.path().stem().string();
        CAPTURE(id);
        std::vector<std::string> note;
        const ScenarioMissione s = scenario_da_originale(cartella, id, DATI, &note);
        CHECK(s.id == "EDITOR_" + id);
        REQUIRE_FALSE(s.unita.empty());
        CHECK(s.unita[0].giocatore);
        CHECK(s.unita[0].classe == (s.alfa ? "alfa" : "los_angeles"));
        CHECK(s.libreria == 2);
        CHECK_FALSE(s.briefing[0].empty());
        REQUIRE_FALSE(note.empty());
        CHECK(note[0].find("logica degli script originali") != std::string::npos);
        const Fondale f = Fondale::carica(DATI + "/teatri/" + s.teatro + ".fdl");
        for (const auto& a : verifica(s, f, dati())) CHECK_MESSAGE(a.gravita != AvvisoScenario::Gravita::errore, a.testo);
        const std::string testo = scrivi_missione_lua(s, dati());
        const auto riletto = leggi_scenario(testo);
        REQUIRE(riletto);
        CHECK(scrivi_missione_lua(*riletto, dati()) == testo);
        const std::string file = scrivi(s.id, testo);
        Missione m(file, 5);
        for (int i = 0; i < 90; ++i) m.avanza(2.0);
        CHECK(m.errori().empty());
        CHECK(m.non_implementate().empty());
        fs::remove(file);
    }
    CHECK(n == 22);
    CHECK_THROWS_AS(scenario_da_originale(cartella, "SCENE_INESISTENTE", DATI), std::runtime_error);
}

TEST_CASE("scenario: il pedinamento si porta vicino e resta, la zona si sposta con una regola" * doctest::skip(!ci_sono_dati())) {
    ScenarioMissione s = da_rapida(41, TipoRapida::pattuglia, "faroe");
    s.situazione = "pace";
    s.aerei_asw.clear();
    s.unita.resize(1);   // solo il giocatore, fermo
    s.unita[0].velocita = 0.0;
    s.obiettivi.erase(std::remove_if(s.obiettivi.begin(), s.obiettivi.end(), [](const ObiettivoScenario& o) { return o.tipo != "sopravvivere"; }),
                      s.obiettivi.end());
    s.chiusura = {};
    const Vec2 g = s.unita[0].pos;
    UnitaScenario p;
    p.id = 1;
    p.classe = "victor3";
    p.categoria = "sottomarino";
    p.parte = "urss";
    p.flag = 0x11;
    p.pos = {g.x + 9000.0, g.y};
    p.quota = 80.0;
    p.velocita = 6.0;
    p.rotta = 270.0;
    p.comportamento = "R_pedina";
    p.profonda = 200.0;
    p.parametri.distanza_nm = 2.0;
    p.parametri.paranoia = 3;
    s.unita.push_back(p);
    // la zona «lontano» e' a 50 km; a 60 s una regola la porta sul giocatore, e un'altra chiude quando ci si trova dentro
    s.zone.push_back({"lontano", {"lontano", "far"}, {g.x + 50000.0, g.y, 2000.0}});
    RegolaScenario sposta;
    sposta.id = "sposta";
    sposta.quando.push_back(cond(CondizioneScenario::Tipo::tempo, 60.0));
    AzioneScenario a;
    a.tipo = AzioneScenario::Tipo::sposta_zona;
    a.zona = "lontano";
    a.punto = g;
    sposta.allora.push_back(a);
    s.regole.push_back(sposta);
    RegolaScenario dentro;
    dentro.id = "dentro";
    CondizioneScenario c = cond(CondizioneScenario::Tipo::giocatore_in_zona);
    c.zona = "lontano";
    dentro.quando.push_back(c);
    AzioneScenario fine;
    fine.tipo = AzioneScenario::Tipo::fine;
    fine.codice = 40;
    fine.testo = {"Zona raggiunta.", "Zone reached."};
    dentro.allora.push_back(fine);
    s.regole.push_back(dentro);
    REQUIRE_FALSE(errori(verifica(s, Fondale::carica(DATI + "/teatri/faroe.fdl"), dati())));
    const std::string testo = scrivi_missione_lua(s, dati());
    CHECK(testo.find("script.R_pedina = function(u)") != std::string::npos);
    CHECK(testo.find("local ZONE = {") != std::string::npos);
    CHECK(scrivi_missione_lua(*leggi_scenario(testo), dati()) == testo);
    const std::string f = scrivi(s.id + "_pedina", testo);
    Missione m(f, 2);
    for (int i = 0; i < 20; ++i) m.avanza(2.0);
    CHECK_FALSE(m.esito());   // la zona e' ancora lontana
    for (int i = 0; i < 30 && !m.esito(); ++i) m.avanza(2.0);
    REQUIRE(m.esito());
    CHECK(*m.esito() == 40);
    // il pedinatore, partito a 9 km, si e' avvicinato
    Missione q(f, 2);
    for (int i = 0; i < 900 && q.unita().count(1); ++i) q.avanza(2.0);
    if (q.unita().count(1)) CHECK(distanza(q.unita().at(1).pos, g) < 8000.0);
    CHECK(q.errori().empty());
    fs::remove(f);
}

// R3 (docs/11): i contenuti radio delle 22 originali, data/missioni/radio/<ID>.lua. Nell'Enhanced ogni file si carica e
// i suoi messaggi vanno in onda (anche senza riceverli): quelli programmati entro 20 minuti sono nel debriefing a 35
TEST_CASE("scenario: i file radio delle 22 originali si caricano e vanno in onda" * doctest::skip(!ci_sono_dati())) {
    const auto classi = carica_classi(DATI + "/classi.toml");
    const fs::path cartella = fs::path(M688_MISSIONI) / "radio";
    int file = 0;
    for (const char* n : {"0A", "1A", "1B", "1C", "2A", "2B", "2C", "3B", "4A", "5A", "5B", "5C", "6A", "7A", "7B", "7C", "8A", "8B", "8C",
                          "9A", "9B", "9C"}) {
        const std::string id = std::string("SCENE") + n;
        CAPTURE(id);
        const auto radio = cartella / (id + ".lua");
        REQUIRE(fs::exists(radio));
        ++file;
        // gli id dei messaggi e quando partono, dal testo del file (una voce per riga: { id = "...", dopo_s = N)
        std::map<std::string, double> attesi;
        std::ifstream in(radio);
        const std::regex voce("\\{ id = \"([^\"]+)\", dopo_s = ([0-9]+)");
        for (std::string riga; std::getline(in, riga);) {
            std::smatch x;
            if (std::regex_search(riga, x, voce) && std::stod(x[2]) <= 1200.0) attesi[x[1]] = std::stod(x[2]);
        }
        CHECK(!attesi.empty());
        Missione m(std::string(M688_MISSIONI) + "/" + id + ".lua", 1);
        DanniMissione dm(classi, DATI, 1, RegolaDanni::simulazione);
        double fine = 1e18;   // col giocatore perso la sala radio tace: contano i messaggi di prima
        for (int i = 0; i < 9 * 2100; ++i) {
            m.avanza(1.0 / 9.0);
            dm.avanza(m, 1.0 / 9.0);
            if (m.esito() && fine > 1e17) fine = m.tempo();
        }
        REQUIRE(m.radio() != nullptr);
        std::set<std::string> in_onda;
        for (const auto& v : m.radio()->debriefing().trasmessi) in_onda.insert(v.messaggio.id);
        for (const auto& [mid, dopo] : attesi) if (dopo < fine) CHECK_MESSAGE(in_onda.count(mid), mid);
        CHECK(m.errori().empty());
    }
    CHECK(file == 22);
}

// R3 con E6: l'intestazione della serie 0 e i messaggi legati agli eventi (un colpo su un'unita' li fa partire)
TEST_CASE("scenario: nei file radio delle originali gli ordini hanno l'intestazione e i messaggi a evento partono" * doctest::skip(!ci_sono_dati())) {
    const auto partito = [](const Missione& m, const std::string& id) {
        if (m.radio()->in_coda(id)) return true;
        for (const auto& v : m.radio()->debriefing().trasmessi)
            if (v.messaggio.id == id) return true;
        return false;
    };
    {
        Missione m(std::string(M688_MISSIONI) + "/SCENE4A.lua", 1);
        m.radio_enhanced();
        REQUIRE(m.radio());
        const MessaggioRadio& o = m.radio()->ordini().front()->messaggio;
        CHECK(o.da == "CINCLANTFLT");
        CHECK(o.oggetto == "MUMAR CADAVER");
        CHECK(o.dtg.find("MAY 89") != std::string::npos);
        for (int i = 0; i < 9 * 60; ++i) m.avanza(1.0 / 9.0);
        CHECK_FALSE(partito(m, "KONI"));
        m.distruggi(4);   // una petroliera
        for (int i = 0; i < 9 * 60; ++i) m.avanza(1.0 / 9.0);
        CHECK(partito(m, "KONI"));
        CHECK(m.errori().empty());
    }
    for (const auto& [id, colpita, messaggio] : {std::tuple{"SCENE7B", 2, "PLARB"}, std::tuple{"SCENE9B", 3, "UDAR"}}) {
        CAPTURE(id);
        Missione m(std::string(M688_MISSIONI) + "/" + id + ".lua", 1);
        m.radio_enhanced();
        REQUIRE(m.radio());
        CHECK(m.radio()->ordini().front()->messaggio.a == "PL «LENINSKII KOMSOMOL»");
        for (int i = 0; i < 9 * 30; ++i) m.avanza(1.0 / 9.0);
        CHECK_FALSE(partito(m, messaggio));
        m.distruggi(colpita);
        for (int i = 0; i < 9 * 60; ++i) m.avanza(1.0 / 9.0);
        CHECK(partito(m, messaggio));
        CHECK(m.errori().empty());
    }
}

// E6 nello scrittore: la radio della missione, il meteo fissato, i messaggi di bordo propri e il cambio di situazione
TEST_CASE("scenario: radio, meteo fissato, messaggi di bordo propri e situazione da una regola" * doctest::skip(!ci_sono_dati())) {
    ScenarioMissione s = da_rapida(31, TipoRapida::pattuglia, "faroe");
    s.situazione = "pace";
    s.aerei_asw.clear();
    RadioScenario r;
    r.ordini = OrdiniScenario{"COMSUBLANT", "USS LOS ANGELES (SSN 688)", "PROVA", "SECRET"};
    r.roe = {{"sottomarini", "trattenuto"}};
    MessaggioScenario a;
    a.id = "A";
    a.dopo_s = 0.0;
    a.precedenza = "immediate";
    a.testo = {"primo", "first"};
    MessaggioScenario b = a;
    b.id = "B";
    b.su_richiesta = true;
    b.roe = {{"sottomarini", "libero"}};
    b.testo = {"armi libere", "weapons free"};
    r.messaggi = {a, b};
    s.radio = r;
    s.meteo_fisso = MeteoFissoScenario{4.0, 270.0, 1.0, 20000.0, std::nullopt, std::nullopt, std::nullopt, "nessuna"};
    RegolaScenario regola;
    regola.id = "svolta";
    regola.quando = {cond(CondizioneScenario::Tipo::tempo, 60.0)};
    AzioneScenario radio;
    radio.tipo = AzioneScenario::Tipo::radio;
    radio.zona = "B";
    AzioneScenario guerra;
    guerra.tipo = AzioneScenario::Tipo::situazione;
    guerra.comportamento = "guerra";
    AzioneScenario bordo;
    bordo.tipo = AzioneScenario::Tipo::messaggio;
    bordo.testo = {"Comandante, la guerra e' cominciata.", "Captain, the war has begun."};
    regola.allora = {radio, guerra, bordo};
    s.regole.push_back(regola);
    const Fondale f = Fondale::carica(DATI + "/teatri/faroe.fdl");
    REQUIRE_FALSE(errori(verifica(s, f, dati())));
    const std::string testo = scrivi_missione_lua(s, dati());
    CHECK(testo.find("  radio = {") != std::string::npos);
    CHECK(testo.find("meteo = { fisso = {") != std::string::npos);
    CHECK(testo.find("[1] = { it = \"Comandante, la guerra e' cominciata.\"") != std::string::npos);
    CHECK(testo.find("m688.radio(u, \"B\")") != std::string::npos);
    CHECK(testo.find("ARMI = 2") != std::string::npos);
    CHECK(testo.find("m688.messaggio_al_giocatore(u, 1001)") != std::string::npos);
    const auto riletto = leggi_scenario(testo);
    REQUIRE(riletto);
    REQUIRE(riletto->radio);
    CHECK(riletto->radio->messaggi.size() == 2);
    CHECK(riletto->radio->messaggi[1].su_richiesta);
    REQUIRE(riletto->meteo_fisso);
    CHECK(riletto->meteo_fisso->nebbia == "nessuna");
    CHECK(scrivi_missione_lua(*riletto, dati()) == testo);
    const std::string file = scrivi(s.id + "_e6", testo);
    Missione m(file, 1);
    m.radio_enhanced();
    REQUIRE(m.radio());
    CHECK(m.radio()->ordini().front()->messaggio.oggetto == "PROVA");
    CHECK(m.radio()->in_coda("A"));
    CHECK_FALSE(m.radio()->in_coda("B"));
    for (int i = 0; i < 9 * 90; ++i) m.avanza(1.0 / 9.0);
    CHECK(m.radio()->in_coda("B"));   // mandato dalla regola
    REQUIRE(m.testo_messaggio_missione(1001));
    CHECK(m.testo_messaggio_missione(1001)->it == "Comandante, la guerra e' cominciata.");
    CHECK(m.errori().empty());
    fs::remove(file);
    // la verifica: un messaggio radio sconosciuto, due messaggi con lo stesso id
    {
        ScenarioMissione t = s;
        t.regole.back().allora[0].zona = "NESSUNO";
        CHECK(errori(verifica(t, f, dati()), "regola_radio"));
    }
    {
        ScenarioMissione t = s;
        t.radio->messaggi[1].id = "A";
        CHECK(errori(verifica(t, f, dati()), "radio_id"));
    }
}

// Le mine gia' posate, la batteria dei diesel, la posa di mine da una regola, le condizioni sul cavo e sulla superficie,
// il contatto d'intelligence nei messaggi radio (S03, S06, S10, I04)
TEST_CASE("scenario: mine, batteria, posa di mine, cavo e superficie si scrivono e si rileggono" * doctest::skip(!ci_sono_dati())) {
    ScenarioMissione s = da_rapida(31, TipoRapida::pattuglia, "faroe");
    s.situazione = "crisi";
    auto nave = std::find_if(s.unita.begin(), s.unita.end(), [](const UnitaScenario& u) { return !u.giocatore && u.categoria == "nave"; });
    REQUIRE(nave != s.unita.end());
    const int id = nave->id;
    UnitaScenario diesel = *nave;
    diesel.id = id + 100;
    diesel.classe = "foxtrot";
    diesel.categoria = "sottomarino";
    diesel.flag = 0x11;
    diesel.quota = 30.0;
    diesel.batteria = 0.4;
    diesel.punti.clear();
    diesel.comportamento = "";
    s.unita.push_back(diesel);
    const Vec2 g = s.unita[0].pos;
    s.mine = {MinaScenario{"mk67", {g.x + 5000.0, g.y}, "urss", true}, MinaScenario{"mk60", {g.x - 5000.0, g.y}, "usa", false}};
    RegolaScenario r;
    r.id = "posa";
    CondizioneScenario cavo = cond(CondizioneScenario::Tipo::vicino_al_cavo, 100.0);
    cavo.unita = id;
    CondizioneScenario su = cond(CondizioneScenario::Tipo::unita_in_superficie);
    su.unita = diesel.id;
    r.quando = {cond(CondizioneScenario::Tipo::giocatore_sopra_nodi, 1.0), cavo, su};
    AzioneScenario posa;
    posa.tipo = AzioneScenario::Tipo::posa_mina;
    posa.unita = id;
    posa.comportamento = "mdt";
    posa.valore = 3;
    posa.codice = 90;
    r.allora = {posa};
    s.regole.push_back(r);
    RadioScenario radio;
    MessaggioScenario m;
    m.id = "INT";
    m.dopo_s = 60.0;
    m.testo = {"contatto", "contact"};
    m.contatto = {diesel.id};
    m.contatto_errore_m = 3000.0;
    m.contatto_sensore = "p3";
    radio.messaggi = {m};
    s.radio = radio;
    const Fondale f = Fondale::carica(DATI + "/teatri/faroe.fdl");
    for (const auto& a : verifica(s, f, dati())) CHECK_MESSAGE(a.gravita != AvvisoScenario::Gravita::errore, a.testo);
    const std::string testo = scrivi_missione_lua(s, dati());
    CHECK(testo.find("  mine = {") != std::string::npos);
    CHECK(testo.find("batteria = 0.4") != std::string::npos);
    CHECK(testo.find("m688.posa_mina(u, \"mdt\", 3, 90)") != std::string::npos);
    CHECK(testo.find("m688.distanza_cavo(u, " + std::to_string(id) + ")") != std::string::npos);
    CHECK(testo.find("contatto = { unita = { " + std::to_string(diesel.id) + ", }, errore_m = 3000, sensore = \"p3\" }") != std::string::npos);
    const auto riletto = leggi_scenario(testo);
    REQUIRE(riletto);
    CHECK(riletto->mine.size() == 2);
    CHECK_FALSE(riletto->mine[1].armata);
    CHECK(riletto->unita.back().batteria == doctest::Approx(0.4));
    CHECK(riletto->radio->messaggi[0].contatto.size() == 1);
    CHECK(scrivi_missione_lua(*riletto, dati()) == testo);
    const std::string file = scrivi(s.id + "_mine", testo);
    {
        Missione mi(file, 1);
        for (int i = 0; i < 60; ++i) mi.avanza(2.0);
        CHECK(mi.errori().empty());
        CHECK(mi.non_implementate().empty());
    }
    {
        Enhanced e(file, 1);
        e.m.siluri_enhanced(DATI);
        e.avanza(120.0);
        CHECK(e.m.errori().empty());
        CHECK(e.m.campo_minato().mine().size() >= 2);   // il campo gia' posato
    }
    fs::remove(file);
    {   // una mina di tipo sconosciuto e una sulla terra
        ScenarioMissione t = s;
        t.mine[0].tipo = "nessuna";
        CHECK(errori(verifica(t, f, dati()), "mina_tipo"));
    }
}

// M6 per l'editor: lo stato iniziale delle unita' (avaria, incagliata), le azioni avaria, rimorchia e molla, le
// condizioni «in avaria» e «a rimorchio» (S11)
TEST_CASE("scenario: stato, avaria e rimorchio si scrivono, si rileggono e si giocano" * doctest::skip(!ci_sono_dati())) {
    ScenarioMissione s = da_rapida(31, TipoRapida::pattuglia, "faroe");
    s.situazione = "crisi";
    auto nave = std::find_if(s.unita.begin(), s.unita.end(), [](const UnitaScenario& u) { return !u.giocatore && u.categoria == "nave"; });
    REQUIRE(nave != s.unita.end());
    const int rimorchiatore = nave->id;
    UnitaScenario diesel = *nave;
    diesel.id = rimorchiatore + 100;
    diesel.classe = "foxtrot";
    diesel.categoria = "sottomarino";
    diesel.flag = 0x11;
    diesel.pos = {nave->pos.x + 600.0, nave->pos.y};
    diesel.quota = 0.0;
    diesel.punti.clear();
    diesel.comportamento = "";
    diesel.stato = "incagliata";
    s.unita.push_back(diesel);
    RegolaScenario r;
    r.id = "traino";
    CondizioneScenario fermo = cond(CondizioneScenario::Tipo::unita_in_avaria);
    fermo.unita = diesel.id;
    r.quando = {cond(CondizioneScenario::Tipo::tempo, 30.0), fermo};
    AzioneScenario traino;
    traino.tipo = AzioneScenario::Tipo::rimorchia;
    traino.unita = rimorchiatore;
    traino.bersaglio = diesel.id;
    traino.velocita = 5.0;
    traino.valore = 250;
    r.allora = {traino};
    s.regole.push_back(r);
    RegolaScenario r2;
    r2.id = "molla";
    CondizioneScenario tirata = cond(CondizioneScenario::Tipo::unita_rimorchiata);
    tirata.unita = diesel.id;
    r2.quando = {cond(CondizioneScenario::Tipo::tempo, 600.0), tirata};
    AzioneScenario molla;
    molla.tipo = AzioneScenario::Tipo::molla_rimorchio;
    molla.unita = rimorchiatore;
    AzioneScenario guasto;
    guasto.tipo = AzioneScenario::Tipo::avaria;
    guasto.unita = rimorchiatore;
    guasto.valore = 1;
    r2.allora = {molla, guasto};
    s.regole.push_back(r2);
    const Fondale f = Fondale::carica(DATI + "/teatri/faroe.fdl");
    for (const auto& a : verifica(s, f, dati())) CHECK_MESSAGE(a.gravita != AvvisoScenario::Gravita::errore, a.testo);
    const std::string testo = scrivi_missione_lua(s, dati());
    CHECK(testo.find("stato = \"incagliata\"") != std::string::npos);
    CHECK(testo.find("m688.rimorchia(u, " + std::to_string(diesel.id) + ", 5, 250)") != std::string::npos);
    CHECK(testo.find("m688.molla_rimorchio(u)") != std::string::npos);
    CHECK(testo.find("su_unita(u, " + std::to_string(diesel.id) + ", m688.in_avaria)") != std::string::npos);
    CHECK(testo.find("su_unita(u, " + std::to_string(diesel.id) + ", m688.rimorchiata)") != std::string::npos);
    const auto riletto = leggi_scenario(testo);
    REQUIRE(riletto);
    CHECK(riletto->unita.back().stato == "incagliata");
    CHECK(riletto->regole[0].allora[0].bersaglio == diesel.id);
    CHECK(scrivi_missione_lua(*riletto, dati()) == testo);
    // le verifiche: uno stato sconosciuto, il giocatore in avaria, un rimorchio di se stessi
    ScenarioMissione cattivo = s;
    cattivo.unita.back().stato = "affondata";
    cattivo.unita[0].stato = "avaria";
    cattivo.regole[0].allora[0].bersaglio = rimorchiatore;
    int errori = 0;
    for (const auto& a : verifica(cattivo, f, dati())) errori += a.gravita == AvvisoScenario::Gravita::errore && (a.codice == "stato" || a.codice == "regola_rimorchio");
    CHECK(errori == 3);
    const std::string file = scrivi(s.id + "_rimorchio", testo);
    Enhanced e(file, 1);
    e.avanza(20.0);
    CHECK(e.m.incagliata_ia(diesel.id));
    e.avanza(60.0);
    CHECK(e.m.rimorchiatore_di(diesel.id) == rimorchiatore);
    CHECK(e.m.errori().empty());
    fs::remove(file);
}

// Gli obiettivi foto, zona inviolata e raggiungere un'unita' in moto, i danni iniziali e il sonar speciale (runtime,
// docs/09 §4 e §5.2)
TEST_CASE("scenario: foto, zona inviolata, unita' da raggiungere, danni iniziali e sonar speciale" * doctest::skip(!ci_sono_dati())) {
    ScenarioMissione s = da_rapida(31, TipoRapida::pattuglia, "faroe");
    auto nave = std::find_if(s.unita.begin(), s.unita.end(), [](const UnitaScenario& u) { return !u.giocatore && u.categoria == "nave"; });
    REQUIRE(nave != s.unita.end());
    nave->sonar_speciale = SonarSpecialeScenario{230.0, 3500.0, 1.0, 0.005, 60.0, 15.0};
    const int id = nave->id;
    const Vec2 g = s.unita[0].pos;
    ObiettivoScenario foto;
    foto.id = "foto";
    foto.tipo = "foto";
    foto.unita = {id};
    foto.distanza_m = 8000.0;
    foto.testo = {"Fotografare la nave", "Photograph the ship"};
    ObiettivoScenario zona;
    zona.id = "zona";
    zona.tipo = "zona_inviolata";
    zona.cerchio = CerchioScenario{g.x, g.y, 3000.0};
    zona.parte = "neutrale";
    zona.minuti = 2.0;
    zona.primario = false;
    zona.testo = {"Nessun neutrale vicino", "No neutrals close"};
    ObiettivoScenario incontro;
    incontro.id = "incontro";
    incontro.tipo = "raggiungere";
    incontro.unita = {id};
    incontro.distanza_m = 2000.0;
    incontro.testo = {"Raggiungere la nave", "Reach the ship"};
    s.obiettivi.insert(s.obiettivi.begin(), {foto, zona, incontro});
    s.danni_iniziali = DanniInizialiScenario{UrtoIniziale{150.0, 60.0, 90.0}, {{"propulsione", 0.4}, {"schiera", 0.0}}, {{"macchine", 10.0}},
                                             {{"macchine", 0.02}}, {{"macchine", 0.3}}};
    const Fondale f = Fondale::carica(DATI + "/teatri/faroe.fdl");
    for (const auto& a : verifica(s, f, dati())) CHECK_MESSAGE(a.gravita != AvvisoScenario::Gravita::errore, a.testo);
    const std::string testo = scrivi_missione_lua(s, dati());
    CHECK(testo.find("tipo = \"foto\", unita = { " + std::to_string(id) + " }") != std::string::npos);
    CHECK(testo.find("distanza_m = 8000") != std::string::npos);
    CHECK(testo.find("parte = \"neutrale\"") != std::string::npos);
    CHECK(testo.find("danni_iniziali = { urto = { tnt_kg = 150") != std::string::npos);
    CHECK(testo.find("sistemi = { propulsione = 0.4, schiera = 0, }") != std::string::npos);
    CHECK(testo.find("incendi = { { compartimento = \"macchine\", intensita = 0.3 }, }") != std::string::npos);
    CHECK(testo.find("sonar_speciale = { sl_db = 230, frequenza_hz = 3500") != std::string::npos);
    const auto riletto = leggi_scenario(testo);
    REQUIRE(riletto);
    REQUIRE(riletto->danni_iniziali);
    CHECK(riletto->danni_iniziali->sistemi.at("propulsione") == doctest::Approx(0.4));
    CHECK(riletto->danni_iniziali->falle.size() == 1);
    CHECK(riletto->obiettivi[1].parte == "neutrale");
    CHECK(riletto->obiettivi[2].distanza_m == doctest::Approx(2000.0));
    CHECK(scrivi_missione_lua(*riletto, dati()) == testo);
    // le verifiche: un sistema sconosciuto, una zona senza cerchio, una parte sconosciuta, raggiungere senza niente
    ScenarioMissione cattivo = s;
    cattivo.danni_iniziali->sistemi["motore_a_vapore"] = 0.5;
    cattivo.obiettivi[1].cerchio.reset();
    cattivo.obiettivi[1].parte = "alleata";
    cattivo.obiettivi[2].distanza_m = 0.0;
    std::set<std::string> codici;
    for (const auto& a : verifica(cattivo, f, dati()))
        if (a.gravita == AvvisoScenario::Gravita::errore) codici.insert(a.codice);
    CHECK(codici.count("danni_sistema"));
    CHECK(codici.count("obiettivo_senza_zona"));
    CHECK(codici.count("obiettivo_parte"));
    // nell'Enhanced: si gioca senza errori, e il battello parte danneggiato
    const std::string file = scrivi(s.id + "_nuovi", testo);
    Enhanced e(file, 1);
    e.avanza(120.0);
    CHECK(e.m.errori().empty());
    CHECK(e.m.non_implementate().empty());
    CHECK(e.m.danni_iniziali().sistemi.size() == 2);
    fs::remove(file);
}

TEST_CASE("scenario: armi da esercitazione, obiettivo colpire, relitto e antenna del sonar speciale" * doctest::skip(!ci_sono_dati())) {
    ScenarioMissione s = da_rapida(32, TipoRapida::pattuglia, "faroe");
    auto nave = std::find_if(s.unita.begin(), s.unita.end(), [](const UnitaScenario& u) { return !u.giocatore && u.categoria == "nave"; });
    REQUIRE(nave != s.unita.end());
    nave->sonar_speciale = SonarSpecialeScenario{230.0, 3500.0, 1.0, 0.005, 0.0, 15.0, "scafo"};
    nave->armi_esercitazione = true;
    const int id = nave->id;
    auto altra = std::find_if(s.unita.begin(), s.unita.end(), [id](const UnitaScenario& u) { return !u.giocatore && u.id != id && u.categoria != "aereo"; });
    REQUIRE(altra != s.unita.end());
    altra->stato = "relitto";
    const int relitto = altra->id;
    s.esercitazione = true;
    ObiettivoScenario colpire;
    colpire.id = "colpire";
    colpire.tipo = "colpire";
    colpire.unita = {id};
    colpire.entro_s = 3600.0;
    colpire.testo = {"Colpire la nave con un'arma da esercitazione", "Hit the ship with a practice weapon"};
    s.obiettivi.insert(s.obiettivi.begin(), colpire);
    RegolaScenario r;
    r.id = "armi_vere";
    CondizioneScenario c;
    c.tipo = CondizioneScenario::Tipo::tempo;
    c.valore = 10.0;
    r.quando = {c};
    AzioneScenario a;
    a.tipo = AzioneScenario::Tipo::esercitazione;
    a.unita = id;
    a.valore = 0;   // di nuovo armi vere
    r.allora = {a};
    s.regole.push_back(r);
    const Fondale f = Fondale::carica(DATI + "/teatri/faroe.fdl");
    for (const auto& v : verifica(s, f, dati())) CHECK_MESSAGE(v.gravita != AvvisoScenario::Gravita::errore, v.testo);
    const std::string testo = scrivi_missione_lua(s, dati());
    CHECK(testo.find("tipo = \"colpire\", unita = { " + std::to_string(id) + " }") != std::string::npos);
    CHECK(testo.find("  esercitazione = true,") != std::string::npos);
    CHECK(testo.find("armi_esercitazione = true") != std::string::npos);
    CHECK(testo.find("stato = \"relitto\"") != std::string::npos);
    CHECK(testo.find("antenna = \"scafo\"") != std::string::npos);
    CHECK(testo.find("m688.esercitazione(u, false)") != std::string::npos);
    const auto riletto = leggi_scenario(testo);
    REQUIRE(riletto);
    CHECK(riletto->esercitazione);
    CHECK(riletto->obiettivi[0].tipo == "colpire");
    CHECK(scrivi_missione_lua(*riletto, dati()) == testo);
    // le verifiche: un'antenna e uno stato sconosciuti, colpire senza unita'
    ScenarioMissione cattivo = s;
    for (auto& u : cattivo.unita) {
        if (u.id == id) u.sonar_speciale->antenna = "parabola";
        if (u.id == relitto) u.stato = "affondata";
    }
    cattivo.obiettivi[0].unita.clear();
    std::set<std::string> codici;
    for (const auto& v : verifica(cattivo, f, dati()))
        if (v.gravita == AvvisoScenario::Gravita::errore) codici.insert(v.codice);
    CHECK(codici.count("sonar_speciale"));
    CHECK(codici.count("stato"));
    CHECK(codici.count("obiettivo_senza_unita"));
    // nell'Enhanced: si gioca senza errori, il relitto e' tale e la regola gira
    const std::string file = scrivi(s.id + "_esercitazione", testo);
    Enhanced e(file, 1);
    e.avanza(60.0);
    CHECK(e.m.errori().empty());
    CHECK(e.m.non_implementate().empty());
    CHECK(e.m.relitto(relitto));
    fs::remove(file);
}

TEST_CASE("scenario: la navigazione e la libreria TLAM con l'ordine d'esecuzione nello scrittore, riletti e caricati" *
          doctest::skip(!ci_sono_dati())) {
    ScenarioMissione s = da_rapida(211u, static_cast<TipoRapida>(1));
    s.navigazione = NavigazioneScenario{1.5};
    TlamScenario x;
    x.id = "0417";
    x.bersaglio = "Severomorsk";
    x.plp = {s.unita[0].pos.x + 30000.0, s.unita[0].pos.y};
    x.landfall = {s.unita[0].pos.x + 30000.0, s.unita[0].pos.y - 200000.0};
    x.tot_s = 12600.0;
    s.tlam.push_back(x);
    RadioScenario r;
    MessaggioScenario m;
    m.id = "ESEGUIRE";
    m.dopo_s = 1800.0;
    m.testo = {"ESEGUIRE 0417", "EXECUTE 0417"};
    m.esecuzione_tlam = "0417";
    r.messaggi.push_back(m);
    s.radio = r;
    const std::string testo = scrivi_missione_lua(s, dati());
    CHECK(testo.find("navigazione = { sigma_iniziale_nm = 1.5 }") != std::string::npos);
    CHECK(testo.find("esecuzione_tlam = \"0417\"") != std::string::npos);
    const auto riletto = leggi_scenario(testo);
    REQUIRE(riletto);
    REQUIRE(riletto->navigazione);
    CHECK(riletto->navigazione->sigma_iniziale_nm == 1.5);
    REQUIRE(riletto->tlam.size() == 1);
    CHECK(riletto->tlam[0].id == "0417");
    CHECK(riletto->tlam[0].bersaglio == "Severomorsk");
    CHECK(riletto->tlam[0].plp.x == x.plp.x);
    CHECK(riletto->tlam[0].tot_s == 12600.0);
    CHECK(riletto->tlam[0].finestra_s == 600.0);
    REQUIRE(riletto->radio);
    CHECK(riletto->radio->messaggi.at(0).esecuzione_tlam == "0417");
    CHECK(scrivi_missione_lua(*riletto, dati()) == testo);
    // la missione la legge come la libreria di bordo
    const auto f = fs::temp_directory_path() / "m688_prova_scenario_tlam.lua";
    std::ofstream(f, std::ios::binary) << testo;
    Missione mis(f.string(), 1, false);
    REQUIRE(mis.libreria_tlam().size() == 1);
    CHECK(mis.libreria_tlam()[0].id == "0417");
    CHECK(std::abs(mis.libreria_tlam()[0].plp.x - x.plp.x) < 0.1);   // la missione ha le posizioni a 0,1 m
    fs::remove(f);
}

TEST_CASE("scenario: la condizione «il giocatore ha trasmesso» e l'EMCON di partenza si scrivono, si rileggono e si verificano" *
          doctest::skip(!ci_sono_dati())) {
    ScenarioMissione s = da_rapida(212u, static_cast<TipoRapida>(1));
    REQUIRE(s.unita.size() >= 2);
    s.unita[1].emcon = "ricerca";
    RegolaScenario r;
    r.id = "rapporto";
    CondizioneScenario c;
    c.tipo = CondizioneScenario::Tipo::giocatore_ha_trasmesso;
    c.valore = 600.0;
    c.trasmissione = "contatto";
    r.quando.push_back(c);
    AzioneScenario a;
    a.tipo = AzioneScenario::Tipo::variabile;
    a.indice = 4;
    a.valore = 1;
    r.allora.push_back(a);
    s.regole.push_back(r);
    const std::string testo = scrivi_missione_lua(s, dati());
    CHECK(testo.find("m688.trasmesso(u, \"contatto\", 600)") != std::string::npos);
    CHECK(testo.find("emcon = \"ricerca\"") != std::string::npos);
    const auto riletto = leggi_scenario(testo);
    REQUIRE(riletto);
    CHECK(riletto->unita[1].emcon == "ricerca");
    REQUIRE(riletto->regole.size() == s.regole.size());
    CHECK(riletto->regole.back().quando.at(0).trasmissione == "contatto");
    CHECK(scrivi_missione_lua(*riletto, dati()) == testo);
    // valori sconosciuti: errori della verifica
    ScenarioMissione cattivo = s;
    cattivo.unita[1].emcon = "a_volte";
    cattivo.regole.back().quando[0].trasmissione = "cartolina";
    const auto avvisi = verifica(cattivo, Fondale::carica(std::string(M688_DATI) + "/teatri/" + cattivo.teatro + ".fdl"), dati());
    int errori = 0;
    for (const auto& x : avvisi) errori += x.gravita == AvvisoScenario::Gravita::errore && (x.codice == "emcon" || x.codice == "regola_trasmissione");
    CHECK(errori == 2);
}

TEST_CASE("scenario: gli obiettivi tlam e scatola e l'azione «attacca l'unita'» si scrivono, si rileggono e si verificano" *
          doctest::skip(!ci_sono_dati())) {
    ScenarioMissione s = da_rapida(213u, static_cast<TipoRapida>(1));
    REQUIRE(s.unita.size() >= 3);
    TlamScenario x;
    x.id = "0521";
    x.plp = s.unita[0].pos;
    x.landfall = {s.unita[0].pos.x, s.unita[0].pos.y - 100000.0};
    x.tot_s = 7200.0;
    s.tlam.push_back(x);
    ObiettivoScenario t;
    t.id = "arrivati";
    t.tipo = "tlam";
    t.missione = "0521";
    t.minimo = 2;
    t.testo = {"Due TLAM sul bersaglio", "Two TLAMs on target"};
    s.obiettivi.push_back(t);
    ObiettivoScenario sc;
    sc.id = "piccola";
    sc.tipo = "scatola";
    sc.massima = "media";
    sc.primario = false;
    sc.testo = {"Niente scatola grande", "No large box"};
    s.obiettivi.push_back(sc);
    RegolaScenario r;
    r.id = "attacco";
    CondizioneScenario c;
    c.tipo = CondizioneScenario::Tipo::tempo;
    c.valore = 600.0;
    r.quando.push_back(c);
    AzioneScenario a;
    a.tipo = AzioneScenario::Tipo::attacca_unita;
    a.unita = s.unita[1].id;
    a.bersaglio = s.unita[2].id;
    a.valore = 1;
    r.allora.push_back(a);
    s.regole.push_back(r);
    const std::string testo = scrivi_missione_lua(s, dati());
    CHECK(testo.find("tipo = \"tlam\", minimo = 2, missione = \"0521\"") != std::string::npos);
    CHECK(testo.find("massima = \"media\"") != std::string::npos);
    CHECK(testo.find("m688.attacca_unita(u, " + std::to_string(s.unita[2].id) + ", true)") != std::string::npos);
    const auto riletto = leggi_scenario(testo);
    REQUIRE(riletto);
    CHECK(scrivi_missione_lua(*riletto, dati()) == testo);
    const auto& o = riletto->obiettivi;
    CHECK(std::any_of(o.begin(), o.end(), [](const ObiettivoScenario& v) { return v.tipo == "tlam" && v.missione == "0521"; }));
    CHECK(std::any_of(o.begin(), o.end(), [](const ObiettivoScenario& v) { return v.tipo == "scatola" && v.massima == "media"; }));
    const Fondale f = Fondale::carica(std::string(M688_DATI) + "/teatri/" + s.teatro + ".fdl");
    for (const auto& v : verifica(s, f, dati())) CHECK_MESSAGE(v.gravita != AvvisoScenario::Gravita::errore, v.testo);
    ScenarioMissione cattivo = s;
    cattivo.obiettivi[cattivo.obiettivi.size() - 2].missione = "9999";
    cattivo.obiettivi.back().massima = "enorme";
    cattivo.regole.back().allora[0].bersaglio = cattivo.regole.back().allora[0].unita;
    int errori = 0;
    for (const auto& v : verifica(cattivo, f, dati()))
        errori += v.codice == "obiettivo_tlam" || v.codice == "obiettivo_scatola" || v.codice == "regola_attacco";
    CHECK(errori == 3);
}

TEST_CASE("scenario: la parte del luogo (M7), la situazione di partenza col luogo e l'azione «situazione» fra due parti" *
          doctest::skip(!ci_sono_dati())) {
    ScenarioMissione s = da_rapida(214u, static_cast<TipoRapida>(1));
    REQUIRE(s.unita.size() >= 2);
    s.unita[1].parte = "luogo";
    s.situazione_luogo = SituazioneLuogo{"guerra", "pace"};
    RegolaScenario r;
    r.id = "tregua";
    CondizioneScenario c;
    c.tipo = CondizioneScenario::Tipo::tempo;
    c.valore = 60.0;
    r.quando.push_back(c);
    AzioneScenario a;
    a.tipo = AzioneScenario::Tipo::situazione;
    a.comportamento = "pace";
    a.fra_a = "luogo";
    a.fra_b = "usa";
    r.allora.push_back(a);
    s.regole.push_back(r);
    const std::string testo = scrivi_missione_lua(s, dati());
    CHECK(testo.find("schieramento = \"luogo\"") != std::string::npos);
    CHECK(testo.find("situazione_luogo = { usa = \"guerra\", urss = \"pace\" }") != std::string::npos);
    CHECK(testo.find("m688.situazione(u, \"pace\", \"luogo\", \"usa\")") != std::string::npos);
    CHECK(testo.find("script.R_siluro_luogo") != std::string::npos);
    const auto riletto = leggi_scenario(testo);
    REQUIRE(riletto);
    CHECK(riletto->unita[1].parte == "luogo");
    REQUIRE(riletto->situazione_luogo);
    CHECK(riletto->situazione_luogo->usa == "guerra");
    CHECK(riletto->regole.back().allora.at(0).fra_a == "luogo");
    CHECK(scrivi_missione_lua(*riletto, dati()) == testo);
    const Fondale f = Fondale::carica(std::string(M688_DATI) + "/teatri/" + s.teatro + ".fdl");
    for (const auto& v : verifica(s, f, dati())) CHECK_MESSAGE(v.gravita != AvvisoScenario::Gravita::errore, v.testo);
    // il motore: il luogo in guerra con gli USA e in pace con l'URSS, poi la tregua della regola
    const auto file = fs::temp_directory_path() / "m688_prova_scenario_luogo.lua";
    std::ofstream(file, std::ios::binary) << testo;
    {
        Missione mis(file.string(), 1, false);
        mis.forze_enhanced(M688_DATI);   // le parti e le coppie sono dell'Enhanced
        mis.avanza(1.0);
        CHECK((mis.unita().at(s.unita[1].id).flag & 0x18) == 0x18);
        CHECK(mis.situazione_fra(0x18, 0x08) == Situazione::guerra);
        CHECK(mis.situazione_fra(0x18, 0x10) == Situazione::pace);
        for (int i = 0; i < 90; ++i) mis.avanza(1.0);
        CHECK(mis.situazione_fra(0x18, 0x08) == Situazione::pace);
    }
    fs::remove(file);
    ScenarioMissione cattivo = s;
    cattivo.situazione_luogo->urss = "tregua";
    cattivo.regole.back().allora[0].fra_b = "luogo";
    int errori = 0;
    for (const auto& v : verifica(cattivo, f, dati())) errori += v.codice == "situazione_luogo" || v.codice == "regola_situazione";
    CHECK(errori == 2);
}

TEST_CASE("scenario: la condizione «evento del registro» si scrive, si rilegge e si verifica" * doctest::skip(!ci_sono_dati())) {
    ScenarioMissione s = da_rapida(215u, static_cast<TipoRapida>(1));
    RegolaScenario r;
    r.id = "fondo";
    CondizioneScenario c;
    c.tipo = CondizioneScenario::Tipo::evento;
    c.evento = "incaglio";
    c.valore = 300.0;
    r.quando.push_back(c);
    AzioneScenario a;
    a.tipo = AzioneScenario::Tipo::variabile;
    a.indice = 6;
    a.valore = 1;
    r.allora.push_back(a);
    s.regole.push_back(r);
    const std::string testo = scrivi_missione_lua(s, dati());
    CHECK(testo.find("m688.evento(u, \"incaglio\", 300)") != std::string::npos);
    const auto riletto = leggi_scenario(testo);
    REQUIRE(riletto);
    CHECK(riletto->regole.back().quando.at(0).evento == "incaglio");
    CHECK(scrivi_missione_lua(*riletto, dati()) == testo);
    const Fondale f = Fondale::carica(std::string(M688_DATI) + "/teatri/" + s.teatro + ".fdl");
    for (const auto& v : verifica(s, f, dati())) CHECK_MESSAGE(v.gravita != AvvisoScenario::Gravita::errore, v.testo);
    ScenarioMissione cattivo = s;
    cattivo.regole.back().quando[0].evento = "terremoto";
    int errori = 0;
    for (const auto& v : verifica(cattivo, f, dati())) errori += v.codice == "regola_evento";
    CHECK(errori == 1);
}

TEST_CASE("scenario: l'azione «lancia_missile» si scrive, si rilegge e si verifica (I11)" * doctest::skip(!ci_sono_dati())) {
    ScenarioMissione s = da_rapida(215u, static_cast<TipoRapida>(1));
    const auto ia = std::find_if(s.unita.begin(), s.unita.end(), [](const UnitaScenario& u) { return !u.giocatore; });
    REQUIRE(ia != s.unita.end());
    const int id = ia->id;
    RegolaScenario r;
    r.id = "salva";
    CondizioneScenario c;
    c.tipo = CondizioneScenario::Tipo::giocatore_in_superficie;
    r.quando.push_back(c);
    AzioneScenario a;
    a.tipo = AzioneScenario::Tipo::lancia_missile;
    a.unita = id;
    a.valore = 2;   // bersaglio -1: il giocatore
    r.allora.push_back(a);
    s.regole.push_back(r);
    const std::string testo = scrivi_missione_lua(s, dati());
    const std::string atteso = "m688.seleziona(u, " + std::to_string(id) + ")\n      m688.bersaglio(u, 0)\n      m688.salva_modello(u)\n"
                               "      m688.lancio_missile(u)\n      m688.lancio_missile(u)\n";
    CHECK(testo.find(atteso) != std::string::npos);
    const auto riletto = leggi_scenario(testo);
    REQUIRE(riletto);
    CHECK(riletto->regole.back().allora.at(0).tipo == AzioneScenario::Tipo::lancia_missile);
    CHECK(riletto->regole.back().allora.at(0).valore == 2);
    CHECK(scrivi_missione_lua(*riletto, dati()) == testo);
    const Fondale f = Fondale::carica(std::string(M688_DATI) + "/teatri/" + s.teatro + ".fdl");
    for (const auto& v : verifica(s, f, dati())) CHECK_MESSAGE(v.gravita != AvvisoScenario::Gravita::errore, v.testo);
    ScenarioMissione cattivo = s;
    cattivo.regole.back().allora[0].bersaglio = id;   // su se stessa
    int errori = 0;
    for (const auto& v : verifica(cattivo, f, dati())) errori += v.codice == "regola_attacco";
    CHECK(errori == 1);
}

TEST_CASE("scenario: la condizione «unita_classifica», le azioni «quota», «rumore» e «comanda_mine» e le mine comandate si "
          "scrivono, si rileggono e si verificano (S10, S14, S15, S18)" * doctest::skip(!ci_sono_dati())) {
    ScenarioMissione s = da_rapida(215u, static_cast<TipoRapida>(1));
    const auto ia = std::find_if(s.unita.begin(), s.unita.end(), [](const UnitaScenario& u) { return !u.giocatore; });
    REQUIRE(ia != s.unita.end());
    const int id = ia->id;
    RegolaScenario r;
    r.id = "classificati";
    CondizioneScenario c;
    c.tipo = CondizioneScenario::Tipo::unita_classifica;
    c.unita = id;
    c.valore = 1.0;   // da POSSUB; bersaglio -1: il giocatore
    r.quando.push_back(c);
    AzioneScenario q;
    q.tipo = AzioneScenario::Tipo::quota;
    q.unita = id;
    q.valore = 60;
    r.allora.push_back(q);
    AzioneScenario n;
    n.tipo = AzioneScenario::Tipo::rumore;
    n.unita = id;
    r.allora.push_back(n);
    AzioneScenario cm;   // le mine comandate del gruppo «nord» (S18)
    cm.tipo = AzioneScenario::Tipo::comanda_mine;
    cm.zona = "nord";
    r.allora.push_back(cm);
    MinaScenario mn;
    mn.tipo = "mk60";
    mn.pos = s.unita[0].pos;
    mn.comandata = true;
    mn.gruppo = "nord";
    s.mine.push_back(mn);
    s.regole.push_back(r);
    const std::string testo = scrivi_missione_lua(s, dati());
    const std::string u = std::to_string(id), g = std::to_string(s.unita[0].id);
    CHECK(testo.find("su_unita(u, " + u + ", m688.classificato, " + g + ", 1)") != std::string::npos);
    CHECK(testo.find("m688.seleziona(u, " + u + ")\n      m688.quota(u, 60, 1)\n      m688.salva_modello(u)\n") != std::string::npos);
    CHECK(testo.find("m688.seleziona(u, " + u + ")\n      m688.rumore_transitorio(u)\n") != std::string::npos);
    const auto riletto = leggi_scenario(testo);
    REQUIRE(riletto);
    CHECK(riletto->regole.back().quando.at(0).tipo == CondizioneScenario::Tipo::unita_classifica);
    CHECK(riletto->regole.back().allora.at(0).tipo == AzioneScenario::Tipo::quota);
    CHECK(riletto->regole.back().allora.at(0).valore == 60);
    CHECK(riletto->regole.back().allora.at(1).tipo == AzioneScenario::Tipo::rumore);
    CHECK(testo.find("m688.comanda_mine(u, \"nord\")") != std::string::npos);
    CHECK(riletto->regole.back().allora.at(2).tipo == AzioneScenario::Tipo::comanda_mine);
    REQUIRE(!riletto->mine.empty());
    CHECK(riletto->mine.back().comandata);
    CHECK(riletto->mine.back().gruppo == "nord");
    CHECK(scrivi_missione_lua(*riletto, dati()) == testo);
    const Fondale f = Fondale::carica(std::string(M688_DATI) + "/teatri/" + s.teatro + ".fdl");
    for (const auto& v : verifica(s, f, dati())) CHECK_MESSAGE(v.gravita != AvvisoScenario::Gravita::errore, v.testo);
    ScenarioMissione cattivo = s;
    cattivo.regole.back().allora[0].unita = 999;
    cattivo.regole.back().quando[0].unita = 999;
    cattivo.regole.back().allora[2].zona = "sud";   // un gruppo senza mine
    int errori = 0, mine = 0;
    for (const auto& v : verifica(cattivo, f, dati())) errori += v.codice == "regola_unita", mine += v.codice == "regola_mine";
    CHECK(errori == 2);
    CHECK(mine == 1);
}

TEST_CASE("scenario: gli obiettivi «identificare», «telemetro» e «rotta_stimata» si scrivono, si rileggono e si verificano (I13)" *
          doctest::skip(!ci_sono_dati())) {
    ScenarioMissione s = da_rapida(215u, static_cast<TipoRapida>(1));
    std::vector<int> ia;
    for (const auto& u : s.unita)
        if (!u.giocatore) ia.push_back(u.id);
    REQUIRE(!ia.empty());
    ObiettivoScenario id;
    id.id = "identifica";
    id.tipo = "identificare";
    id.unita = ia;
    id.minimo = 1;
    ObiettivoScenario tel;
    tel.id = "telemetro";
    tel.tipo = "telemetro";
    tel.minimo = 3;
    tel.errore = 0.1;
    ObiettivoScenario aob;
    aob.id = "aob";
    aob.tipo = "rotta_stimata";
    aob.minimo = 3;
    aob.errore = 20.0;
    aob.primario = false;
    s.obiettivi.insert(s.obiettivi.end(), {id, tel, aob});
    const std::string testo = scrivi_missione_lua(s, dati());
    // la tabella degli obiettivi del motore (il registro M1 del runtime): tipo, unita', minimo, errore
    CHECK(testo.find("{ id = \"identifica\", tipo = \"identificare\", unita = { ") != std::string::npos);
    CHECK(testo.find("{ id = \"telemetro\", tipo = \"telemetro\", minimo = 3, errore = 0.1,") != std::string::npos);
    CHECK(testo.find("{ id = \"aob\", tipo = \"rotta_stimata\", minimo = 3, primario = false, errore = 20,") != std::string::npos);
    const auto riletto = leggi_scenario(testo);
    REQUIRE(riletto);
    const auto& o = riletto->obiettivi;
    REQUIRE(o.size() >= 3);
    CHECK(o[o.size() - 2].errore == doctest::Approx(0.1));
    CHECK(o.back().errore == doctest::Approx(20.0));
    CHECK(scrivi_missione_lua(*riletto, dati()) == testo);
    CHECK_FALSE(o.back().da_tma);
    CHECK(testo.find("da_tma") == std::string::npos);   // senza il campo le missioni non cambiano
    {   // le rotte dalla TMA al MATE (LIB-S20): il campo si scrive e si rilegge, nella tabella e nel blocco dell'editor
        ScenarioMissione t = s;
        t.obiettivi.back().da_tma = true;
        const std::string con = scrivi_missione_lua(t, dati());
        CHECK(con.find("{ id = \"aob\", tipo = \"rotta_stimata\", minimo = 3, primario = false, errore = 20, da_tma = true,") != std::string::npos);
        const auto r = leggi_scenario(con);
        REQUIRE(r);
        CHECK(r->obiettivi.back().da_tma);
        CHECK(scrivi_missione_lua(*r, dati()) == con);
    }
    const Fondale f = Fondale::carica(std::string(M688_DATI) + "/teatri/" + s.teatro + ".fdl");
    for (const auto& v : verifica(s, f, dati())) CHECK_MESSAGE(v.gravita != AvvisoScenario::Gravita::errore, v.testo);
    // senza errore massimo o senza unita' da identificare: errori della verifica
    ScenarioMissione cattivo = s;
    cattivo.obiettivi[cattivo.obiettivi.size() - 2].errore = 0.0;
    cattivo.obiettivi[cattivo.obiettivi.size() - 3].unita.clear();
    int errore = 0, senza_unita = 0;
    for (const auto& v : verifica(cattivo, f, dati())) {
        errore += v.codice == "obiettivo_errore";
        senza_unita += v.codice == "obiettivo_senza_unita";
    }
    CHECK(errore == 1);
    CHECK(senza_unita == 1);
}

TEST_CASE("scenario: la chiave d'unita' «cisterne» si scrive, si rilegge, si verifica e arriva all'unita' (danni critici, T7)" * doctest::skip(!ci_sono_dati())) {
    ScenarioMissione s = da_rapida(215u, static_cast<TipoRapida>(1));
    const auto ia = std::find_if(s.unita.begin(), s.unita.end(), [](const UnitaScenario& u) { return !u.giocatore; });
    REQUIRE(ia != s.unita.end());
    CHECK(scrivi_missione_lua(s, dati()).find("cisterne") == std::string::npos);   // senza la chiave il file non cambia
    ia->classe = "petroliera";
    ia->cisterne = "zavorra_inerte";
    const std::string testo = scrivi_missione_lua(s, dati());
    CHECK(testo.find("cisterne = \"zavorra_inerte\"") != std::string::npos);
    const auto riletto = leggi_scenario(testo);
    REQUIRE(riletto);
    const auto ri = std::find_if(riletto->unita.begin(), riletto->unita.end(), [&](const UnitaScenario& u) { return u.id == ia->id; });
    REQUIRE(ri != riletto->unita.end());
    CHECK(ri->cisterne == "zavorra_inerte");
    CHECK(scrivi_missione_lua(*riletto, dati()) == testo);
    const Fondale f = Fondale::carica(std::string(M688_DATI) + "/teatri/" + s.teatro + ".fdl");
    const auto avvisi = [&](AvvisoScenario::Gravita g) {
        int n = 0;
        for (const auto& v : verifica(s, f, dati())) n += v.codice == "cisterne" && v.gravita == g;
        return n;
    };
    CHECK(avvisi(AvvisoScenario::Gravita::errore) == 0);
    CHECK(avvisi(AvvisoScenario::Gravita::attenzione) == 0);
    ia->cisterne = "piene";
    CHECK(avvisi(AvvisoScenario::Gravita::errore) == 1);
    ia->cisterne = "zavorra";
    ia->classe = "cargo";
    CHECK(avvisi(AvvisoScenario::Gravita::attenzione) == 1);
}

TEST_CASE("scenario: la chiave d'unita' «luci» si scrive, si rilegge e si verifica (docs/27)" * doctest::skip(!ci_sono_dati())) {
    ScenarioMissione s = da_rapida(215u, static_cast<TipoRapida>(1));
    const auto ia = std::find_if(s.unita.begin(), s.unita.end(), [](const UnitaScenario& u) { return !u.giocatore; });
    REQUIRE(ia != s.unita.end());
    ia->luci = "fonda";
    const std::string testo = scrivi_missione_lua(s, dati());
    CHECK(testo.find("luci = \"fonda\"") != std::string::npos);
    const auto riletto = leggi_scenario(testo);
    REQUIRE(riletto);
    const auto ri = std::find_if(riletto->unita.begin(), riletto->unita.end(), [&](const UnitaScenario& u) { return u.id == ia->id; });
    REQUIRE(ri != riletto->unita.end());
    CHECK(ri->luci == "fonda");
    CHECK(scrivi_missione_lua(*riletto, dati()) == testo);
    const Fondale f = Fondale::carica(std::string(M688_DATI) + "/teatri/" + s.teatro + ".fdl");
    for (const auto& v : verifica(s, f, dati())) CHECK_MESSAGE(v.gravita != AvvisoScenario::Gravita::errore, v.testo);
    ia->luci = "lanterne";
    int errori = 0;
    for (const auto& v : verifica(s, f, dati())) errori += v.codice == "luci";
    CHECK(errori == 1);
}

// Le partite da riga di comando e dai codici di origine «originale» (le serie, gli esperimenti): la data che main passa alla
// Partita e' quella della missione, non il predefinito del 15 ottobre (06/11/2026: la M4 di giugno girava in ottobre)
TEST_CASE("scenario: la data d'inizio e' quella del file per scene di prova e libreria" * doctest::skip(!ci_sono_dati())) {
    const fs::path radice = M688_MISSIONI;
    int viste = 0;
    for (const char* cartella : {"prove", "libreria"})
        for (const auto& e : fs::directory_iterator(radice / cartella)) {
            if (e.path().extension() != ".lua") continue;
            std::ifstream f(e.path(), std::ios::binary);
            const auto sc = leggi_scenario(std::string{std::istreambuf_iterator<char>(f), std::istreambuf_iterator<char>()});
            REQUIRE(sc.has_value());
            CAPTURE(e.path().string());
            // dalla cartella della scena (i codici) e dalla radice con la sottocartella (--missione prove/X): la stessa data
            for (const auto& d : {data_della_missione(e.path().parent_path(), e.path().stem().string()),
                                  data_della_missione(radice, std::string(cartella) + "/" + e.path().stem().string())}) {
                REQUIRE(d.has_value());
                CHECK(d->anno == sc->anno);
                CHECK(d->mese == sc->mese);
                CHECK(d->giorno == sc->giorno);
                CHECK(std::fabs(d->ore_utc - sc->ora_utc) < 0.001);   // la riga del file ha tre decimali, lo scenario tutti
            }
            ++viste;
        }
    CHECK(viste >= 64);
}

TEST_CASE("scenario: le originali partono dalla data degli ordini e non hanno una data nel file" * doctest::skip(!ci_sono_dati())) {
    const fs::path radice = M688_MISSIONI;
    int viste = 0;
    for (const auto& e : fs::directory_iterator(radice)) {
        if (e.path().extension() != ".lua") continue;
        // senza la cartella radio accanto: dal solo file nessuna data
        const fs::path sola = fs::temp_directory_path() / "m688_prova_data_missione";
        fs::create_directories(sola);
        fs::copy_file(e.path(), sola / e.path().filename(), fs::copy_options::overwrite_existing);
        CAPTURE(e.path().string());
        CHECK_FALSE(data_della_missione(sola, e.path().stem().string()).has_value());
        fs::remove(sola / e.path().filename());
        ++viste;
    }
    CHECK(viste >= 22);
    const auto d = data_della_missione(radice, "SCENE1A");
    REQUIRE(d.has_value());
    CHECK(d->anno == 1989);
    CHECK_FALSE(data_della_missione(radice, "NON_ESISTE").has_value());
}
