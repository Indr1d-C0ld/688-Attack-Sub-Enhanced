#include <doctest.h>

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <map>
#include <string>

#include "missioni/danni_missione.hpp"
#include "missioni/danni_sottomarini.hpp"
#include "missioni/missione.hpp"
#include "missioni/registro_missione.hpp"
#include "nucleo/classi.hpp"
#include "nucleo/battello.hpp"
#include "nucleo/geometria.hpp"

using namespace m688;
namespace fs = std::filesystem;

// Collisioni nella missione dell'Enhanced (docs/ricerca/16_collisioni.md, P1, P3, P6): rilevazione, urto valutato col
// contratto del kit, contatto reciproco, scena acustica, registro; quasi collisioni; Crazy Ivan.

namespace {

const std::string SUB = "modello = { massima_kn = 32, virata_gradi_s = 1, accel_kn_s = 0.5, quota_m_s = 1, quota_max_m = 450, integrita = 200, rumore = 8, sonar = 19 }";

// il 688 del giocatore fermo a 100 m con la prua a est; `altre` le righe Lua delle altre unita'
std::string scena(const std::string& nome, const std::string& altre, const std::string& script = "", const std::string& timer = "") {
    const auto f = fs::temp_directory_path() / ("m688_prova_collisioni_" + nome + ".lua");
    std::ofstream(f) << "return { id = 'URTO_" << nome << "', teatro = 'faroe', paesaggio_sonoro = false, script = {" << script << "},"
                        " varianti = { { probabilita = 1, unita = {\n"
                        "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true,"
                        "  posizione = { x = 0, y = 0 }, quota_m = 100, rotta = 90, velocita_kn = 0, " << SUB << " },\n"
                     << altre << "\n}, timer = {" << timer << "} } } }\n";
    return f.string();
}

struct Partita {
    std::map<std::string, ClasseNave> classi = carica_classi(M688_DATI "/classi.toml");
    Missione m;
    DanniMissione dm;
    explicit Partita(const std::string& file) : m(file, 1, false), dm(classi, M688_DATI, 1, RegolaDanni::simulazione) {}
    void avanza(double s) { for (long i = 0; i < static_cast<long>(s * 9.0); ++i) { m.avanza(1.0 / 9.0); dm.avanza(m, 1.0 / 9.0); } }
};

int quanti(const Missione& m, TipoEvento t) {
    return static_cast<int>(std::count_if(m.registro().eventi().begin(), m.registro().eventi().end(), [&](const EventoMissione& e) { return e.tipo == t; }));
}

}  // namespace

TEST_CASE("collisioni: un Victor sperona di prua il 688 fermo; urto, contatto reciproco, suono e registro") {
    const auto f = scena("speronamento", "{ id = 2, classe = 'victor3', categoria = 'sottomarino', flag = 25, posizione = { x = 0, y = -400 },"
                                         "  quota_m = 100, rotta = 0, velocita_kn = 8, " + SUB + " },");
    Partita p(f);
    p.m.usa_difesa(false);   // lo speronamento e' voluto: niente anticollisione dell'IA (docs/18 fase 4 A)
    p.avanza(5.0);
    REQUIRE(p.m.con_collisioni());
    p.m.usa_crazy_ivan(false);
    for (int i = 0; i < 200 && p.m.urti().empty(); ++i) p.avanza(1.0);
    REQUIRE_FALSE(p.m.urti().empty());
    const auto& x = p.m.urti().front();
    CHECK(x.urto.a.unita == 2);   // l'urtante e' il Victor che arriva
    CHECK(x.urto.b.unita == 0);
    CHECK(x.urto.a.sottomarino);
    CHECK(x.urto.a.massa_t > 3000.0);
    CHECK(std::fabs(x.urto.normale[1]) > 0.9);   // lungo il nord: di fianco al 688
    CHECK(x.esito.energia_normale_mj > 0.5);
    CHECK(x.esito.livello_sorgente_db > 150.0);
    // un solo urto all'inizio del contatto (il successivo solo dopo 2 s)
    CHECK(p.m.urti().size() <= 2);
    // dopo l'urto entrambi sanno: il contatto reciproco con la posizione
    REQUIRE(p.m.unita().at(0).contatti.count(2) == 1);
    CHECK(p.m.unita().at(0).contatti.at(2).posizione_nota);
    CHECK(p.m.unita().at(2).contatti.count(0) == 1);
    CHECK(quanti(p.m, TipoEvento::collisione) >= 1);
    bool suono = false;
    for (const auto& s : p.m.scena_acustica(0, "bqq5_sfera").sorgenti) suono = suono || (s.id >= 940000 && s.id <= 949999);
    CHECK(suono);
    fs::remove(f);
}

TEST_CASE("collisioni: il Victor passa a 90 m dalla prua: quasi collisione, nessun urto; il classico si attraversa") {
    const auto f = scena("passaggio", "{ id = 2, classe = 'victor3', categoria = 'sottomarino', flag = 25, posizione = { x = 150, y = -1500 },"
                                      "  quota_m = 100, rotta = 0, velocita_kn = 10, " + SUB + " },");
    Partita p(f);
    p.avanza(1.0);
    p.m.usa_crazy_ivan(false);
    p.avanza(400.0);
    CHECK(p.m.urti().empty());
    REQUIRE(p.m.quasi_collisioni().size() == 1);
    const auto& q = p.m.quasi_collisioni().front();
    CHECK(q.a == 0);
    CHECK(q.b == 2);
    // il 688 ha la prua a est: lo scafo arriva a x = 55 m, il Victor (10 m di diametro) passa a x = 150
    CHECK(q.distanza_m == doctest::Approx(90.0).epsilon(0.05));
    CHECK(quanti(p.m, TipoEvento::quasi_collisione) == 1);
    // nel classico le unita' si attraversano: niente urti
    const auto g = scena("classico", "{ id = 2, classe = 'victor3', categoria = 'sottomarino', flag = 25, posizione = { x = 0, y = -400 },"
                                     "  quota_m = 100, rotta = 0, velocita_kn = 8, " + SUB + " },");
    Missione classico(g, 1, false);
    for (int i = 0; i < 9 * 200; ++i) classico.avanza(1.0 / 9.0);
    CHECK_FALSE(classico.con_collisioni());
    CHECK(classico.urti().empty());
    fs::remove(f);
    fs::remove(g);
}

TEST_CASE("collisioni: il Crazy Ivan di un Alfa sovietico (virata di 120-180 gradi), e spento no") {
    const auto f = scena("ivan", "{ id = 2, classe = 'alfa', categoria = 'sottomarino', flag = 17, posizione = { x = 20000, y = 0 },"
                                 "  quota_m = 150, rotta = 0, velocita_kn = 10, " + SUB + " },");
    Partita p(f);
    double massima = 0.0;
    for (int i = 0; i < 3 * 3600 / 10; ++i) {
        p.avanza(10.0);
        massima = std::max(massima, std::fabs(normalizza_180(p.m.unita().at(2).rotta)));
    }
    CHECK(p.m.manovre_crazy_ivan(2) >= 1);
    CHECK(massima > 100.0);   // la prua si e' girata
    CHECK(p.m.manovre_crazy_ivan(0) == 0);   // mai il giocatore
    Partita q(f);
    q.avanza(1.0);
    q.m.usa_crazy_ivan(false);
    q.avanza(3 * 3600.0);
    CHECK(q.m.manovre_crazy_ivan(2) == 0);
    fs::remove(f);
}

// la frequenza (runtime, 29/10/2026): ogni 30-120 minuti in rotta, non ogni 5-15. Prima la rotta vista diventava quella
// dell'Ivan, e il ritorno in rotta contava come un'accostata, che ne chiama un altro entro 5-15 minuti
TEST_CASE("collisioni: dopo un Crazy Ivan il ritorno in rotta non ne chiama un altro") {
    const auto f = scena("ivan_frequenza", "{ id = 2, classe = 'alfa', categoria = 'sottomarino', flag = 17, posizione = { x = 20000, y = 0 },"
                                           "  quota_m = 150, rotta = 0, velocita_kn = 10, " + SUB + " },");
    Partita p(f);
    p.avanza(6.0 * 3600.0);
    const int n = p.m.manovre_crazy_ivan(2);
    MESSAGE("Crazy Ivan in 6 ore: ", n);
    CHECK(n >= 2);
    CHECK(n <= 12);   // al piu' uno ogni 30 minuti (5; col difetto 17)
    fs::remove(f);
}

#include "nucleo/ordini.hpp"

TEST_CASE("collisioni: l'immersione d'emergenza porta la vela sotto la chiglia delle navi vicine") {
    CHECK(ordine_da_nome("immersione_emergenza") == TipoOrdine::immersione_emergenza);
    CHECK(nome_ordine(TipoOrdine::immersione_emergenza) == "immersione_emergenza");
    // il 688 a quota periscopica con un Krivak a 1 km: almeno 150 ft, e la vela 10 m sotto la sua chiglia
    const auto f = scena("emergenza", "{ id = 2, classe = 'krivak2', categoria = 'nave', flag = 18, posizione = { x = 0, y = 1000 },"
                                      "  quota_m = 0, rotta = 0, velocita_kn = 5, " + SUB + " },");
    Partita p(f);
    p.avanza(1.0);
    const auto& classi = p.classi;
    const double q = p.m.quota_immersione_emergenza();
    CHECK(q >= 45.72);
    CHECK(q >= classi.at("krivak2").pescaggio_m + 10.0 + classi.at("los_angeles").larghezza_m + classi.at("los_angeles").altezza_vela_m - 1e-9);
    // senza i gestori dell'originale (la scena di prova): quota e velocita' ordinate direttamente
    REQUIRE(p.m.applica({p.m.tempo(), TipoOrdine::immersione_emergenza}));
    CHECK(p.m.giocatore()->quota_ordinata == doctest::Approx(q));
    CHECK(p.m.giocatore()->velocita_ordinata == doctest::Approx(p.m.giocatore()->avanti_tutta));
    // nel classico, con i gestori dell'originale: quota rapida e avanti tutta
    Missione m(std::string(M688_MISSIONI) + "/SCENE1A.lua", 1);
    m.avanza(1.0);
    REQUIRE(m.applica({m.tempo(), TipoOrdine::immersione_emergenza}));
    CHECK(m.giocatore()->quota_ordinata == doctest::Approx(m.quota_immersione_emergenza()).epsilon(0.02));
    fs::remove(f);
}

TEST_CASE("M6: la batteria del Foxtrot dell'IA; lo snorkel la ricarica, scarica emerge per forza; il classico no") {
    // un Foxtrot al 32% a 8 kn: in pochi minuti va a snorkel e ricarica
    const auto f = scena("batteria", "{ id = 2, classe = 'foxtrot', categoria = 'sottomarino', flag = 17, posizione = { x = 20000, y = 0 },"
                                     "  quota_m = 100, rotta = 0, velocita_kn = 8, batteria = 0.32, " + SUB + " },");
    Partita p(f);
    p.avanza(1.0);
    p.m.usa_crazy_ivan(false);
    REQUIRE(p.m.batteria(2) == doctest::Approx(0.32).epsilon(0.01));
    CHECK(p.m.batteria(0) < 0.0);   // il giocatore no
    p.avanza(15.0 * 60.0);
    CHECK(p.m.snorkel(2));
    CHECK(p.m.unita().at(2).quota <= 8.0);
    const double prima = p.m.batteria(2);
    p.avanza(30.0 * 60.0);
    CHECK(p.m.batteria(2) > prima);   // i diesel ricaricano
    // al 5,1%, in caccia (il giocatore vicino lo pinga e ne ha la posizione): niente snorkel, si trascina a 2 kn per
    // risparmiare e poi emerge per forza
    const auto g = scena("scarica", "{ id = 2, classe = 'foxtrot', categoria = 'sottomarino', flag = 17, posizione = { x = 0, y = 2500 },"
                                    "  quota_m = 100, rotta = 90, velocita_kn = 12, batteria = 0.051, " + SUB + " },",
                         "P = function(u) m688.impulso_sonar(u) m688.dopo(u, 40, 'P') end", "{ dopo_s = 2, unita = 0, gestore = 'P' }");
    Partita q(g);
    q.avanza(1.0);
    q.m.usa_crazy_ivan(false);
    q.avanza(30.0 * 60.0);
    CAPTURE(q.m.batteria(2));
    CAPTURE(q.m.snorkel(2));
    CHECK(q.m.emersione_forzata(2));
    CHECK(q.m.unita().at(2).quota <= 1.0);
    CHECK(quanti(q.m, TipoEvento::emersione_forzata) == 1);
    // nel classico non c'e' batteria
    Missione classico(f, 1, false);
    classico.avanza(1.0);
    CHECK(classico.batteria(2) < 0.0);
    fs::remove(f);
    fs::remove(g);
}

TEST_CASE("S10: il cavo della schiera della Perry si impiglia nell'elica del 688 fermo sulla sua scia") {
    // la Perry fila la SQR-19 a 8 nodi verso nord; a 8 nodi il cavo scende fino ai 360 m della schiera, e il 688 fermo
    // a 365 m di chiglia (asse a 360) ha l'elica a y = 1.000 sulla rotta della Perry
    const std::string nave = "modello = { massima_kn = 29, virata_gradi_s = 2, accel_kn_s = 0.5, quota_m_s = 1, quota_max_m = 0, integrita = 100, rumore = 10, sonar = 20, schiera_trainata = true }";
    const auto f = fs::temp_directory_path() / "m688_prova_cavo.lua";
    std::ofstream(f.string()) << "return { id = 'CAVO', teatro = 'faroe', paesaggio_sonoro = false, script = {}, varianti = { { probabilita = 1, unita = {\n"
                                 "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true,"
                                 "  posizione = { x = 0, y = 1055 }, quota_m = 365, rotta = 0, velocita_kn = 0, " << SUB << " },"
                                 "{ id = 2, classe = 'perry', categoria = 'nave', flag = 10, posizione = { x = 0, y = 0 },"
                                 "  quota_m = 0, rotta = 0, velocita_kn = 8, " << nave << " },"
                                 "\n}, timer = {} } } }\n";
    Partita p(f.string());
    p.avanza(1.0);
    p.m.usa_crazy_ivan(false);
    REQUIRE((p.m.unita().at(2).flag54 & 1) != 0);
    for (int i = 0; i < 900 && p.m.incidenti_cavo().empty(); ++i) p.avanza(1.0);
    REQUIRE_FALSE(p.m.incidenti_cavo().empty());
    const auto& x = p.m.incidenti_cavo().front();
    CHECK(x.impigliato);
    CHECK(x.sottomarino == 0);
    CHECK(x.nave == 2);
    CHECK((p.m.unita().at(2).flag54 & 1) == 0);   // la schiera e' persa
    CHECK(p.m.unita().at(2).contatti.count(0) == 1);   // e la Perry sente il battello
    CHECK(std::any_of(p.m.guasti_richiesti().begin(), p.m.guasti_richiesti().end(),
                      [](const GuastoRichiesto& g) { return g.unita == 0 && g.sistema == static_cast<int>(Sistema::propulsione); }));
    CHECK(quanti(p.m, TipoEvento::cavo_impigliato) == 1);
    CHECK(p.m.distanza_dal_cavo(0, 2) < 0.0);   // senza schiera, niente cavo
    fs::remove(f);
}

TEST_CASE("collisioni: dopo lo speronamento il Victor fa piu' rumore (urto_db nella sorgente, coi danni dei sottomarini)") {
    const auto f = scena("speronamento_rumore", "{ id = 2, classe = 'victor3', categoria = 'sottomarino', flag = 25, posizione = { x = 0, y = -400 },"
                                                "  quota_m = 100, rotta = 0, velocita_kn = 8, " + SUB + " },");
    std::map<std::string, ClasseNave> classi = carica_classi(M688_DATI "/classi.toml");
    Missione m(f, 1, false);
    DanniMissione dm(classi, M688_DATI, 1, RegolaDanni::simulazione);
    DanniSottomarini ds(classi, M688_DATI, 1);
    m.usa_difesa(false);
    const auto avanza = [&](double s) {
        for (long i = 0; i < static_cast<long>(s * 9.0); ++i) {
            m.avanza(1.0 / 9.0);
            dm.avanza(m, 1.0 / 9.0);
            ds.avanza(m, 1.0 / 9.0);
            m.raccogli_danni(&ds, &dm);
        }
    };
    avanza(5.0);
    m.usa_crazy_ivan(false);
    for (int i = 0; i < 200 && m.urti().empty(); ++i) avanza(1.0);
    REQUIRE_FALSE(m.urti().empty());
    avanza(5.0);
    const double urto = ds.rumore_urto_db(2);
    MESSAGE("rumore d'urto del Victor: ", urto, " dB");
    double nella_sorgente = -1.0;
    for (const auto& s : m.scena_acustica(0, "bqq5_sfera").sorgenti)
        if (s.id == 2) nella_sorgente = s.var.urto_db;
    if (urto > 0.0 && m.variazioni_acustiche()) CHECK(nella_sorgente == doctest::Approx(std::min(urto, 12.0)));
    fs::remove(f);
}

TEST_CASE("collisioni: la nave col duomo perso non sente piu', col timone bloccato non accosta (valuta_urto)") {
    // una Perry (2) a 5 km da un Victor rumoroso (3); il 688 lontano
    const auto prova = [](bool duomo, bool timone) {
        const auto f = scena("duomo_timone",
            "{ id = 2, classe = 'perry', categoria = 'nave', flag = 10, posizione = { x = 30000, y = 0 }, quota_m = 0, rotta = 90, velocita_kn = 10,"
            "  modello = { massima_kn = 30, virata_gradi_s = 2, accel_kn_s = 0.5, quota_m_s = 0, quota_max_m = 0, integrita = 100, rumore = 10, sonar = 20 } },"
            "{ id = 3, classe = 'victor3', categoria = 'sottomarino', flag = 17, posizione = { x = 30000, y = 5000 }, quota_m = 60, rotta = 0, velocita_kn = 20, " + SUB + " },");
        Partita p(f);
        p.avanza(1.0);
        if (duomo) p.m.perdi_sonar_scafo(2);
        OrdineMotore o;
        o.meta = true;
        o.verso = {30000.0, -20000.0};   // verso sud: un'accostata di 90 gradi
        o.velocita_kn = 10.0;
        o.timone = !timone;
        p.m.ordina(2, o);
        p.avanza(180.0);
        const auto& v = p.m.unita().at(2);
        // il sonar che lo tiene: col duomo perso resta solo la SQR-19 filata (docs/ricerca/33)
        const std::string sente = v.contatti.count(3) ? v.contatti.at(3).sensore : "";
        const double rotta = v.rotta;
        fs::remove(f);
        return std::make_pair(sente, rotta);
    };
    const auto [sente, rotta] = prova(false, false);
    CHECK((!sente.empty() && sente != "radar"));
    CHECK(std::fabs(normalizza_180(rotta - 180.0)) < 30.0);   // ha accostato a sud
    const auto [sente_duomo, rotta_duomo] = prova(true, false);
    CHECK((sente_duomo.empty() || sente_duomo == "radar" || sente_duomo == "sqr19"));   // niente sonar di scafo
    (void)rotta_duomo;
    const auto [sente_timone, rotta_timone] = prova(false, true);
    CHECK(std::fabs(normalizza_180(rotta_timone - 90.0)) < 1.0);   // la prua resta a est
    (void)sente_timone;
}
