#include <doctest.h>

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "missioni/danni_missione.hpp"
#include "missioni/danni_sottomarini.hpp"
#include "missioni/missione.hpp"
#include "missioni/registro_missione.hpp"
#include "nucleo/classi.hpp"
#include "nucleo/fari.hpp"
#include "nucleo/fondale.hpp"
#include "nucleo/ordini.hpp"

using namespace m688;
namespace fs = std::filesystem;

// La navigazione incerta del giocatore nella missione (roadmap 26; nucleo/navigazione.hpp; nota condivisa
// navigazione-aggancio-contratto.md): spenta di norma; accesa, la stima deriva e il punto nave la corregge alle sue
// condizioni.

namespace {

const std::string SUB = "modello = { massima_kn = 32, virata_gradi_s = 1, accel_kn_s = 0.5, quota_m_s = 1, quota_max_m = 450, integrita = 200, rumore = 8, sonar = 19 }";

// il giocatore in superficie (gli alberi fuori senza il posto dei periscopi); `navigazione` la chiave della missione
std::string scena(const std::string& nome, const std::string& navigazione) {
    const auto f = fs::temp_directory_path() / ("m688_prova_navigazione_" + nome + ".lua");
    std::ofstream(f) << "return { id = 'NAV_" << nome << "', teatro = 'faroe', lato = 'usa', data = '1989-06-15', ora_utc = 12,"
                        " paesaggio_sonoro = false, " << navigazione << " script = {}, varianti = { { probabilita = 1, unita = {\n"
                        "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true, posizione = { x = 0, y = 0 },"
                        "  quota_m = 0, rotta = 90, velocita_kn = 4, " << SUB << " },\n"
                        "}, timer = {} } } }\n";
    return f.string();
}

struct Partita {
    std::map<std::string, ClasseNave> classi = carica_classi(M688_DATI "/classi.toml");
    Missione m;
    DanniMissione dm;
    Partita(const std::string& file, bool navigazione) : m(file, 1, false), dm(classi, M688_DATI, 1, RegolaDanni::simulazione) {
        m.usa_navigazione(navigazione);
    }
    void avanza(double s) { for (long i = 0; i < static_cast<long>(s * 9.0); ++i) { m.avanza(1.0 / 9.0); dm.avanza(m, 1.0 / 9.0); } }
};

bool c_e() { return fs::exists(M688_DATI "/navigazione.toml"); }

}  // namespace

TEST_CASE("navigazione: spenta di norma, la posizione propria e' esatta" * doctest::skip(!c_e())) {
    const auto f = scena("spenta", "");
    Partita p(f, false);
    p.avanza(60.0);
    CHECK_FALSE(p.m.con_navigazione());
    const Vec2 s = p.m.posizione_stimata(0), v = p.m.unita().at(0).pos;
    CHECK(s.x == v.x);
    CHECK(s.y == v.y);
    CHECK_FALSE(p.m.punto_nave_in_corso().has_value());
    fs::remove(f);
}

TEST_CASE("navigazione: a inerziale guasto la stima deriva; il GPS la riporta a posto" * doctest::skip(!c_e())) {
    const auto f = scena("deriva", "navigazione = { inerziale_guasto = true, gps = true },");
    Partita p(f, true);
    p.avanza(2.0);
    REQUIRE(p.m.con_navigazione());
    const double sigma0 = p.m.sigma_navigazione_m(0).value_or(-1.0);
    p.avanza(2.0 * 3600.0);
    const double sigma = p.m.sigma_navigazione_m(0).value_or(-1.0);
    CHECK(sigma > sigma0 * 3.0);
    const Vec2 s = p.m.posizione_stimata(0), v = p.m.unita().at(0).pos;
    CHECK(std::hypot(s.x - v.x, s.y - v.y) > 1.0);
    // il GPS con l'albero fuori: un minuto e la stima e' buona
    bool gps = false;
    for (const auto& [id, val] : p.m.punti_nave_possibili()) {
        if (id == "gps") gps = val.possibile;
        if (id == "gps" && !val.possibile) MESSAGE("gps motivo ", static_cast<int>(val.motivo));
    }
    REQUIRE(gps);
    CHECK(p.m.applica(Ordine{p.m.tempo(), TipoOrdine::punto_nave, 0.0, 0.0, 0, "gps"}));
    CHECK(p.m.punto_nave_in_corso()->in_corso);
    p.avanza(90.0);
    REQUIRE_FALSE(p.m.punti_nave().empty());
    const auto& x = p.m.punti_nave().back();
    CHECK(x.fatto);
    CHECK(x.sigma_dopo_m < x.sigma_prima_m / 3.0);
    const Vec2 s2 = p.m.posizione_stimata(0), v2 = p.m.unita().at(0).pos;
    CHECK(std::hypot(s2.x - v2.x, s2.y - v2.y) < 200.0);
    const auto& reg = p.m.eventi_missione();
    CHECK(std::any_of(reg.begin(), reg.end(), [](const EventoMissione& e) { return e.tipo == TipoEvento::punto_nave && e.giusta; }));
    fs::remove(f);
}

TEST_CASE("navigazione: senza l'opzione il GPS nel 1989 non c'e', e un punto nave impossibile ha il suo motivo" * doctest::skip(!c_e())) {
    const auto f = scena("motivo", "");
    Partita p(f, true);
    p.avanza(5.0);
    REQUIRE(p.m.con_navigazione());
    CHECK_FALSE(p.m.applica(Ordine{p.m.tempo(), TipoOrdine::punto_nave, 0.0, 0.0, 0, "gps"}));
    REQUIRE_FALSE(p.m.punti_nave().empty());
    CHECK_FALSE(p.m.punti_nave().back().fatto);
    CHECK(p.m.punti_nave().back().motivo != MotivoPuntoNave::nessuno);
    CHECK(p.m.punto_nave_in_corso()->motivo != MotivoPuntoNave::nessuno);
    fs::remove(f);
}

TEST_CASE("navigazione: l'inerziale fuori servizio nei danni fa navigare a stima, col messaggio 474" * doctest::skip(!c_e())) {
    const auto f = scena("danni", "danni_iniziali = { sistemi = { inerziale = 0 } },");
    std::map<std::string, ClasseNave> classi = carica_classi(M688_DATI "/classi.toml");
    Missione m(f, 1, false);
    m.usa_navigazione(true);
    DanniMissione dm(classi, M688_DATI, 1, RegolaDanni::simulazione);
    DanniSottomarini ds(classi, M688_DATI, 1);
    for (int i = 0; i < 9 * 20; ++i) {
        m.avanza(1.0 / 9.0);
        dm.avanza(m, 1.0 / 9.0);
        ds.avanza(m, 1.0 / 9.0);
    }
    REQUIRE(m.con_navigazione());
    CHECK(std::any_of(m.messaggi().begin(), m.messaggi().end(), [](const Messaggio& x) { return x.codice == 474; }));
    REQUIRE(m.navigatore() != nullptr);
    CHECK_FALSE(m.navigatore()->a_inerziale());
    fs::remove(f);
}

TEST_CASE("navigazione: anche i sottomarini dell'IA sanno dove sono solo per stima, ed esposti la correggono" * doctest::skip(!c_e())) {
    // un Victor (3) profondo per 12 ore, poi in superficie (script): l'errore cresce e il punto nave lo riduce
    const auto f = fs::temp_directory_path() / "m688_prova_navigazione_ia.lua";
    std::ofstream(f) << "return { id = 'NAV_IA', teatro = 'faroe', lato = 'usa', data = '1989-06-15', ora_utc = 12, paesaggio_sonoro = false,"
                        " script = { S = function(u) m688.quota(u, 0, 1) m688.salva_modello(u) end }, varianti = { { probabilita = 1, unita = {\n"
                        "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true, posizione = { x = 0, y = 0 },"
                        "  quota_m = 0, rotta = 90, velocita_kn = 4, " << SUB << " },\n"
                        "{ id = 3, classe = 'victor3', categoria = 'sottomarino', flag = 17, posizione = { x = 50000, y = 50000 },"
                        "  quota_m = 150, rotta = 90, velocita_kn = 6, " << SUB << " },\n"
                        "}, timer = { { dopo_s = 43200, unita = 3, gestore = 'S' } } } } }\n";
    Partita p(f.string(), true);
    p.avanza(12.0 * 3600.0);
    REQUIRE(p.m.con_navigazione());
    const auto errore = [&] {
        const Vec2 s = p.m.posizione_stimata(3), v = p.m.unita().at(3).pos;
        return std::hypot(s.x - v.x, s.y - v.y);
    };
    const double profondo = errore();
    CHECK(profondo > 300.0);
    p.avanza(3.0 * 3600.0);   // in superficie: il Parus (o un altro punto nave della VMF) quando passa
    MESSAGE("errore del Victor: 12 h sotto ", profondo, " m, dopo 3 h in superficie ", errore(), " m");
    CHECK(errore() < profondo);
    fs::remove(f);
}

// L'IA con l'errore di posizione (docs/18): i gestori d'esito delle 22 originali, letti dal testo dello script. Con
// GESTORI_ESITO=file l'elenco per missione (i gestori d'esito e i vai_a lasciati veri) si scrive nel file, da guardare a
// mano: una funzione d'esito che sfugge si vede li'
TEST_CASE("navigazione: i gestori d'esito delle originali" * doctest::skip(!fs::exists(std::string(M688_MISSIONI) + "/SCENE0A.lua"))) {
    std::ofstream elenco;
    if (const char* f = std::getenv("GESTORI_ESITO")) elenco.open(f);
    int originali = 0;
    std::vector<fs::path> file;
    for (const auto& e : fs::directory_iterator(M688_MISSIONI))
        if (e.path().filename().string().rfind("SCENE", 0) == 0 && e.path().extension() == ".lua") file.push_back(e.path());
    std::sort(file.begin(), file.end());
    for (const auto& p : file) {
        std::ifstream in(p, std::ios::binary);
        const std::string testo{std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>()};
        const GestoriEsito g = leggi_gestori_esito(testo);
        ++originali;
        CHECK_MESSAGE(g.leggibile, p.filename().string());
        CHECK_MESSAGE(!g.esito.empty(), p.filename().string());
        if (elenco) {
            elenco << p.stem().string() << (g.leggibile ? "" : " (illeggibile: tutto vero)") << "\n  d'esito:";
            for (const auto& x : g.esito) elenco << ' ' << x;
            elenco << "\n  vai_a veri:";
            for (const auto& [f, a] : g.vai_a_veri) elenco << ' ' << f << "->" << a;
            elenco << "\n";
        }
    }
    CHECK(originali == 22);
    // la 0A: L_02fb (i Tomahawk a 300 s, esito 3) e L_0311 (Copenaghen, esiti 4-6) sono d'esito, il lancio L_0724 no
    std::ifstream in(std::string(M688_MISSIONI) + "/SCENE0A.lua", std::ios::binary);
    const GestoriEsito g = leggi_gestori_esito({std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>()});
    CHECK(g.esito.count("L_02fb"));
    CHECK(g.esito.count("L_0311"));
    CHECK_FALSE(g.esito.count("L_0724"));
    // le chiamate: chi chiama un gestore d'esito lo e'; un codice non letterale rende lo script illeggibile
    const GestoriEsito h = leggi_gestori_esito("script.A = function(u)\n  script.B(u)\nend\nscript.B = function(u)\n"
                                               "  m688.messaggio(u, 104)\nend\nscript.C = function(u)\n  m688.vai_a(u, 1, 2, 30, \"B\")\n"
                                               "  m688.messaggio(u, 1003)\nend\n");
    CHECK(h.leggibile);
    CHECK(h.esito == std::set<std::string>{"A", "B"});
    REQUIRE(h.vai_a_veri.size() == 1);
    CHECK(h.vai_a_veri[0] == std::pair<std::string, std::string>{"C", "B"});
    CHECK_FALSE(leggi_gestori_esito("script.A = function(u)\n  m688.messaggio(u, 100 + n)\nend\n").leggibile);
}

// Un SSN dell'IA (occidentale: senza i Crazy Ivan, che allungano il viaggio di ore) con 5 nm di sigma iniziale va con
// vai_a a un punto: arriva dove crede (la prova di zona del suo script,
// fuori dai gestori d'esito, lo trova dentro), cioe' spostato del suo errore; il gestore d'esito, sulla stessa zona,
// guarda la posizione vera e non da' l'esito. Un vai_a col gestore d'arrivo d'esito resta vero: arriva davvero
TEST_CASE("navigazione: il sottomarino dell'IA arriva spostato del suo errore, gli obiettivi restano veri" * doctest::skip(!c_e())) {
    const auto scena_ia = [](const std::string& arrivo) {
        const auto f = fs::temp_directory_path() / ("m688_prova_navigazione_ia_meta_" + arrivo + ".lua");
        std::ofstream(f)
            << "local script = {}\n"
               "script.V = function(u)\n  m688.vai_a(u, 80000, 50000, 30, '" << arrivo << "')\n  m688.salva_modello(u)\nend\n"
               "script.A = function(u)\n  m688.variabile(u, 1, 1)\nend\n"
               "script.E2 = function(u)\n  m688.variabile(u, 1, 1)\n  m688.messaggio(u, 106)\nend\n"
               "script.Z = function(u)\n  if m688.in_rettangolo(u, 79700, 50300, 80300, 49700) then m688.variabile(u, 2, 1) end\n"
               "  m688.dopo(u, 10, 'Z')\nend\n"
               "script.E = function(u)\n  if m688.in_rettangolo(u, 79700, 50300, 80300, 49700) then m688.messaggio(u, 105) end\n"
               "  m688.dopo(u, 10, 'E')\nend\n"
               "return { id = 'NAV_IA_META', teatro = 'faroe', lato = 'usa', data = '1989-06-15', ora_utc = 12, paesaggio_sonoro = false,"
               " navigazione = { sigma_iniziale_nm = 5 }, script = script, varianti = { { probabilita = 1, unita = {\n"
               "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true, posizione = { x = 0, y = 0 },"
               "  quota_m = 0, rotta = 90, velocita_kn = 4, " << SUB << " },\n"
               "{ id = 3, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, posizione = { x = 70000, y = 50000 },"
               "  quota_m = 150, rotta = 90, velocita_kn = 8, " << SUB << " },\n"
               "}, timer = { { dopo_s = 1, unita = 3, gestore = 'V' }, { dopo_s = 5, unita = 3, gestore = 'Z' },"
               " { dopo_s = 5, unita = 3, gestore = 'E' } } } } }\n";
        return f.string();
    };
    const auto arrivato = [](Partita& p) {
        CHECK(p.m.gestori_esito().leggibile);
        CHECK(p.m.gestori_esito().esito == std::set<std::string>{"E", "E2"});
        for (int k = 0; k < 6 * 180 && !(p.m.variabili().count(1) && p.m.variabili().at(1) == 1); ++k) p.avanza(10.0);   // al piu' 3 ore
        return p.m.variabili().count(1) && p.m.variabili().at(1) == 1;
    };
    SUBCASE("arrivo senza esito: dalla stima") {
        const std::string f = scena_ia("A");
        Partita p(f, true);
        REQUIRE(arrivato(p));
        const Vec2 v = p.m.unita().at(3).pos, s = p.m.posizione_stimata(3);
        const double vera = std::hypot(v.x - 80000.0, v.y - 50000.0), stimata = std::hypot(s.x - 80000.0, s.y - 50000.0);
        MESSAGE("SSN all'arrivo: vero a ", vera, " m dal punto, stimato a ", stimata, " m");
        CHECK(stimata < 450.0);   // la soglia d'arrivo e' per asse (278 m), piu' l'abbrivio
        CHECK(vera > 1500.0);
        p.avanza(30.0);
        CHECK(p.m.variabili().count(2));      // la zona dello script: sulla stima, dentro
        CHECK_FALSE(p.m.esito().has_value());   // l'esito sulla stessa zona: sul vero, fuori
        fs::remove(f);
    }
    SUBCASE("arrivo d'esito: vero") {
        const std::string f = scena_ia("E2");
        Partita p(f, true);
        REQUIRE(arrivato(p));
        const Vec2 v = p.m.unita().at(3).pos;
        CHECK(std::hypot(v.x - 80000.0, v.y - 50000.0) < 450.0);
        CHECK(p.m.esito() == std::optional<int>(6));
        fs::remove(f);
    }
    SUBCASE("navigazione spenta: tutto vero") {
        const std::string f = scena_ia("A");
        Partita p(f, false);
        REQUIRE(arrivato(p));
        const Vec2 v = p.m.unita().at(3).pos;
        CHECK(std::hypot(v.x - 80000.0, v.y - 50000.0) < 450.0);
        p.avanza(30.0);
        CHECK(p.m.esito() == std::optional<int>(5));
        fs::remove(f);
    }
}

// la navigazione incerta dell'IA (principale, 29/10/2026): la rotta a punti della missione, come vai_a, si governa dalla
// stima (l'SSN arriva spostato del suo errore); i punti nave dell'IA vanno nel registro (punto_nave_ia, verita'); senza
// la navigazione incerta tutto vero
TEST_CASE("navigazione: la rotta a punti dell'IA dalla stima, i suoi punti nave nel registro" * doctest::skip(!c_e())) {
    const auto f = fs::temp_directory_path() / "m688_prova_navigazione_ia_rotta_punti.lua";
    std::ofstream(f) << "local script = {}\nscript.A = function(u)\n  m688.variabile(u, 1, 1)\nend\n"
                        "return { id = 'NAV_IA_ROTTA', teatro = 'faroe', lato = 'usa', data = '1989-06-15', ora_utc = 12, paesaggio_sonoro = false,"
                        " navigazione = { sigma_iniziale_nm = 5 }, script = script, varianti = { { probabilita = 1, unita = {\n"
                        "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true, posizione = { x = 0, y = 0 },"
                        "  quota_m = 0, rotta = 90, velocita_kn = 4, " << SUB << " },\n"
                        "{ id = 3, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, posizione = { x = 70000, y = 50000 },"
                        "  quota_m = 150, rotta = 90, velocita_kn = 8, rotta_punti = { { x = 80000, y = 50000 } }, gestori = { arrivo = 'A' }, "
                     << SUB << " },\n"
                        "{ id = 4, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, posizione = { x = -60000, y = 50000 },"
                        "  quota_m = 15, rotta = 90, velocita_kn = 4, " << SUB << " },\n"
                        "}, timer = {} } } }\n";
    const auto gioca = [&](bool navigazione) {
        Partita p(f.string(), navigazione);
        for (int k = 0; k < 6 * 180 && !(p.m.variabili().count(1) && p.m.variabili().at(1) == 1); ++k) p.avanza(10.0);
        REQUIRE(p.m.variabili().count(1));
        const Vec2 v = p.m.unita().at(3).pos;
        const auto& ev = p.m.eventi_missione();
        const auto fix = std::count_if(ev.begin(), ev.end(), [](const EventoMissione& e) { return e.tipo == TipoEvento::punto_nave_ia && e.unita == 4; });
        return std::pair<double, long>{std::hypot(v.x - 80000.0, v.y - 50000.0), fix};
    };
    const auto [vera, fix] = gioca(true);
    MESSAGE("SSN all'arrivo della rotta a punti: ", vera, " m dal punto vero; punti nave dell'altro nel registro: ", fix);
    CHECK(vera > 1500.0);   // la stima governa la meta: arriva spostato
    CHECK(fix >= 1);
    const auto [vera_spenta, fix_spenta] = gioca(false);
    CHECK(vera_spenta < 450.0);
    CHECK(fix_spenta == 0);
    fs::remove(f);
}

// E della principale (29/10/2026): le navi dell'IA navigano a stima fra un punto nave e l'altro (LORAN-C, Transit, Cajka;
// il GPS col 1990+), con l'antenna sempre fuori: partite con 5 nm di sigma come le altre, dopo un quarto d'ora hanno un
// errore piccolo ma non nullo; i punti nave nel registro. Senza la navigazione incerta l'errore e' zero
TEST_CASE("navigazione: le navi dell'IA con l'errore piccolo dei loro punti nave" * doctest::skip(!c_e())) {
    const auto f = fs::temp_directory_path() / "m688_prova_navigazione_navi_ia.lua";
    const std::string nave = "modello = { massima_kn = 30, virata_gradi_s = 2, accel_kn_s = 0.5, quota_m_s = 0, quota_max_m = 0, integrita = 100, rumore = 10, sonar = 0 }";
    std::ofstream(f) << "return { id = 'NAV_NAVI_IA', teatro = 'faroe', lato = 'usa', data = '1989-06-15', ora_utc = 12, paesaggio_sonoro = false,"
                        " navigazione = { sigma_iniziale_nm = 5 }, script = {}, varianti = { { probabilita = 1, unita = {\n"
                        "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true, posizione = { x = 0, y = 0 },"
                        "  quota_m = 150, rotta = 90, velocita_kn = 4, " << SUB << " },\n"
                        "{ id = 2, classe = 'perry', categoria = 'nave', flag = 10, posizione = { x = 20000, y = 0 }, quota_m = 0, rotta = 90,"
                        "  velocita_kn = 15, " << nave << " },\n"
                        "{ id = 3, classe = 'krivak2', categoria = 'nave', flag = 18, posizione = { x = -20000, y = 0 }, quota_m = 0, rotta = 270,"
                        "  velocita_kn = 15, " << nave << " },\n"
                        "}, timer = {} } } }\n";
    const auto errore = [](const Partita& p, int id) {
        const Vec2 s = p.m.posizione_stimata(id), v = p.m.unita().at(id).pos;
        return std::hypot(s.x - v.x, s.y - v.y);
    };
    {
        Partita p(f.string(), true);
        p.avanza(900.0);
        const double perry = errore(p, 2), krivak = errore(p, 3);
        MESSAGE("errore dopo 15 minuti: Perry ", perry, " m, Krivak II ", krivak, " m");
        CHECK(perry > 0.0);
        CHECK(perry < 1500.0);   // LORAN-C al largo, non 5 nm
        CHECK(krivak > 0.0);
        CHECK(krivak < 3000.0);
        const auto& ev = p.m.eventi_missione();
        CHECK(std::count_if(ev.begin(), ev.end(), [](const EventoMissione& e) { return e.tipo == TipoEvento::punto_nave_ia && e.unita == 2; }) >= 1);
    }
    {
        Partita p(f.string(), false);
        p.avanza(60.0);
        CHECK(errore(p, 2) == 0.0);
    }
    fs::remove(f);
}

// A2 della principale (29/10/2026): il sottomarino dell'IA con la navigazione incerta governa sulla stima e sul suo quadro
// del fondale (carta della sua parte e misure), con gli incagli sul vero. Un'isola di 3 km di raggio sulla rotta di un
// SSN dell'IA (un Los Angeles: senza i Crazy Ivan dei sovietici, che spostano la rotta): con la posizione incerta il comandante la aggira largo (le rotte sulla carta con le secche allargate di una
// sigma e mezza) e non ci finisce sopra; senza la navigazione incerta tutto vero come prima
TEST_CASE("navigazione: il sottomarino dell'IA aggira la secca sulla stima, senza incagliarsi" * doctest::skip(!c_e())) {
    const auto f = fs::temp_directory_path() / "m688_prova_navigazione_ia_secca.lua";
    std::ofstream(f) << "local script = {}\n"
                        "script.V = function(u)\n  m688.vai_a(u, 0, 20000, 12, 'A')\n  m688.salva_modello(u)\nend\n"
                        "script.A = function(u)\n  m688.variabile(u, 1, 1)\nend\n"
                        "return { id = 'NAV_IA_SECCA', teatro = 'faroe', lato = 'usa', data = '1989-06-15', ora_utc = 12, paesaggio_sonoro = false,"
                        " navigazione = { sigma_iniziale_nm = 1 }, script = script, varianti = { { probabilita = 1, unita = {\n"
                        "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true, posizione = { x = -40000, y = -40000 },"
                        "  quota_m = 0, rotta = 90, velocita_kn = 4, " << SUB << " },\n"
                        "{ id = 3, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, posizione = { x = 0, y = -20000 },"
                        "  quota_m = 100, rotta = 0, velocita_kn = 12, " << SUB << " },\n"
                        "}, timer = { { dopo_s = 1, unita = 3, gestore = 'V' } } } } }\n";
    // 100 km di lato, celle di 500 m, 400 m d'acqua; l'isola (10 m sul mare) entro 3 km dall'origine
    std::vector<std::uint16_t> valori(200 * 200, 0);
    const Fondale piatto(200, 200, 500.0, 1.0, -400.0, valori);
    for (int r = 0; r < 200; ++r)
        for (int c = 0; c < 200; ++c) {
            const Vec2 q = piatto.centro_cella(c, r);
            if (std::hypot(q.x, q.y) < 3000.0) valori[static_cast<std::size_t>(r) * 200 + c] = 410;
        }
    const auto fondale = std::make_shared<const Fondale>(200, 200, 500.0, 1.0, -400.0, valori);
    const auto gioca = [&](bool navigazione) {
        Partita p(f.string(), navigazione);
        p.m.imposta_fondale(fondale);
        double vicino = 1e9, errore = 0.0;
        for (int k = 0; k < 6 * 240 && !p.m.variabili().count(1); ++k) {
            p.avanza(10.0);
            const UnitaClassica& v = p.m.unita().at(3);
            if (v.distrutta) break;
            // finche' la posizione e' incerta (oltre 1 km di sigma; un punto nave batimetrico sull'isola la riduce)
            if (p.m.sigma_navigazione_m(3).value_or(0.0) > 1000.0 || !navigazione) vicino = std::min(vicino, std::hypot(v.pos.x, v.pos.y));
            const Vec2 s = p.m.posizione_stimata(3);
            errore = std::max(errore, std::hypot(s.x - v.pos.x, s.y - v.pos.y));
        }
        const UnitaClassica& v = p.m.unita().at(3);
        MESSAGE(std::string(navigazione ? "con" : "senza"), " la navigazione incerta: piu' vicino all'isola ", vicino,
                " m dal centro, errore fino a ", errore, " m, integrita' ", v.integrita, "/", v.integrita_max);
        CHECK(p.m.variabili().count(1));   // arriva (sulla stima)
        CHECK_FALSE(v.distrutta);
        CHECK(v.integrita == v.integrita_max);   // nessun urto
        return std::pair<double, bool>{vicino, p.m.quadro_fondale(3) != nullptr};
    };
    const auto [vicino_incerto, quadro] = gioca(true);
    CHECK(quadro);   // l'Enhanced: l'SSN ha il suo quadro, e il pilota lo usa
    const auto vicino_vero = gioca(false).first;
    CHECK(vicino_incerto > vicino_vero + 1000.0);   // con la posizione incerta si tiene piu' al largo
    fs::remove(f);
}

// L'IA sotto il pack (runtime, 29/10/2026; la simmetria del ghiaccio, docs/18): il diesel che deve fare snorkel sotto
// la banchisa resta sotto e va all'acqua libera piu' vicina, li' sale e ricarica; sotto il ghiaccio non ricarica. Un
// Kilo col 25% di batteria nel pack delle Curili a marzo (copertura 0,5-0,9)
TEST_CASE("sotto il pack: il Kilo scarico va all'acqua libera e fa snorkel li', non sotto il ghiaccio" * doctest::skip(!fs::exists(M688_DATI "/meteo.toml"))) {
    const auto scena = [](double x) {
        const auto f = fs::temp_directory_path() / "m688_prova_pack_kilo.lua";
        std::ofstream(f) << "return { id = 'PACK_KILO', teatro = 'kunashir', lato = 'usa', data = '1989-03-15', ora_utc = 0, paesaggio_sonoro = false,"
                            " posa = { centro = { lat = 44.0, lon = 146.5 } }, script = {}, varianti = { { probabilita = 1, unita = {\n"
                            "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true, posizione = { x = 100000, y = -100000 },"
                            "  quota_m = 100, rotta = 90, velocita_kn = 4, " << SUB << " },\n"
                            "{ id = 3, classe = 'kilo', categoria = 'sottomarino', flag = 17, posizione = { x = " << x << ", y = 0 }, batteria = 0.25,"
                            "  quota_m = 100, rotta = 90, velocita_kn = 3, " << SUB << " },\n"
                            "}, timer = {} } } }\n";
        return f.string();
    };
    std::map<std::string, ClasseNave> classi = carica_classi(M688_DATI "/classi.toml");
    struct Gioco {
        Missione m;
        DanniMissione dm;
        Gioco(const std::string& f, const std::map<std::string, ClasseNave>& c) : m(f, 1, false), dm(c, M688_DATI, 1, RegolaDanni::simulazione) {
            m.usa_meteo(Missione::ModoMeteo::realistico);
        }
        void avanza(double s) { for (long i = 0; i < static_cast<long>(s * 9.0); ++i) { m.avanza(1.0 / 9.0); dm.avanza(m, 1.0 / 9.0); } }
    };
    // il punto di partenza: sotto il ghiaccio dove la copertura e' poco sotto la meta' (Okhotsk verso il margine)
    double x0 = 0.0;
    {
        Gioco sonda(scena(-60000.0), classi);
        sonda.avanza(1.0);
        REQUIRE(sonda.m.ghiaccio());
        for (double x = -60000.0; x <= -30000.0 && x0 == 0.0; x += 500.0) {
            const PuntoGhiaccio g = sonda.m.ghiaccio()->in({x, 0.0}, sonda.m.tempo());
            if (g.ghiaccio && !g.aperta && sonda.m.meteo_in({x, 0.0}).ghiaccio < 0.5) x0 = x;
        }
    }
    REQUIRE(x0 != 0.0);
    const std::string f = scena(x0);
    Gioco p(f, classi);
    p.avanza(1.0);
    REQUIRE(p.m.sotto_banchisa(3));
    double minima_sotto = 1e9, carica_min = 1.0, carica_fine = 0.0, ricarica_sotto = 0.0, prima = p.m.batteria(3);
    bool salito = false, sotto_prima = true;
    for (int k = 0; k < 6 * 360; ++k) {
        p.avanza(10.0);
        const UnitaClassica& u = p.m.unita().at(3);
        const double c = p.m.batteria(3);
        const bool sotto = p.m.sotto_banchisa(3);
        if (sotto) {
            if (!salito) minima_sotto = std::min(minima_sotto, u.quota);   // finche' non e' salito la prima volta
            if (sotto_prima && c > prima + 1e-9) ricarica_sotto += c - prima;   // tutti i 10 s sotto (al campione)
        } else if (u.quota <= 20.0) {
            salito = true;
        }
        sotto_prima = sotto;
        carica_min = std::min(carica_min, c);
        carica_fine = c;
        prima = c;
    }
    const Vec2 v = p.m.unita().at(3).pos;
    MESSAGE("Kilo da x = ", x0 / 1000.0, " km (copertura ", p.m.meteo_in({x0, 0.0}).ghiaccio, "): sotto il pack mai sopra ", minima_sotto,
            " m prima di salire; salito ", salito, "; batteria da 0,25 a ", carica_min, " poi ", carica_fine, "; a ",
            std::hypot(v.x - x0, v.y) / 1000.0, " km dalla partenza");
    CHECK(minima_sotto >= 40.0);      // non sale sotto il ghiaccio
    CHECK(ricarica_sotto == 0.0);     // e li' non ricarica
    CHECK(salito);                    // ha trovato l'acqua libera
    CHECK(carica_fine > carica_min);  // e ha ricaricato
    fs::remove(f);
}

// sotto la banchisa niente punto nave dell'IA: ne' albero ne' filo (come il giocatore). Un Victor a 15 m sotto il pack
// pieno della missione, poi col mare libero
TEST_CASE("sotto il pack: il sottomarino dell'IA a quota periscopica non prende il punto nave" * doctest::skip(!c_e() || !fs::exists(M688_DATI "/meteo.toml"))) {
    const auto conta = [](double ghiaccio) {
        const auto f = fs::temp_directory_path() / "m688_prova_pack_punto_nave.lua";
        std::ofstream(f) << "return { id = 'PACK_PN', teatro = 'barents', lato = 'usa', data = '1989-01-15', ora_utc = 12, paesaggio_sonoro = false,"
                            " meteo = { fisso = { ghiaccio = " << ghiaccio << " } }, navigazione = { sigma_iniziale_nm = 2 },"
                            " script = {}, varianti = { { probabilita = 1, unita = {\n"
                            "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true, posizione = { x = 50000, y = 50000 },"
                            "  quota_m = 100, rotta = 0, velocita_kn = 4, " << SUB << " },\n"
                            "{ id = 3, classe = 'victor3', categoria = 'sottomarino', flag = 17, posizione = { x = 80000, y = 50000 },"
                            "  quota_m = 15, rotta = 0, velocita_kn = 4, " << SUB << " },\n"
                            "}, timer = {} } } }\n";
        Partita p(f.string(), true);
        p.avanza(1.0);
        const bool sotto = p.m.sotto_banchisa(3);
        p.avanza(120.0);
        const auto& ev = p.m.eventi_missione();
        fs::remove(f);
        return std::pair<bool, long>{sotto, std::count_if(ev.begin(), ev.end(), [](const EventoMissione& e) { return e.tipo == TipoEvento::punto_nave_ia && e.unita == 3; })};
    };
    const auto [sotto, fix_pack] = conta(1.0);
    REQUIRE(sotto);
    CHECK(fix_pack == 0);
    const auto [libero, fix_libero] = conta(0.0);
    CHECK_FALSE(libero);
    CHECK(fix_libero >= 1);
}

TEST_CASE("navigazione: il punto nave dai fari, di notte, con due luci in vista" * doctest::skip(!c_e() || !fs::exists(M688_DATI "/fari.toml"))) {
    // due fari del teatro a meno di 15 km fra loro (fari.toml del kit); il battello in superficie in acqua fra i due, di
    // notte a dicembre; poi di giorno
    const auto fondale = std::make_shared<const Fondale>(Fondale::carica(M688_DATI "/teatri/faroe.fdl"));
    const auto fari = carica_fari(M688_DATI "/fari.toml", "faroe");
    std::optional<Vec2> punto;
    for (std::size_t i = 0; i < fari.size() && !punto; ++i)
        for (std::size_t j = i + 1; j < fari.size() && !punto; ++j) {
            const Vec2 a = fondale->proiettata({fari[i].lat, fari[i].lon}), b = fondale->proiettata({fari[j].lat, fari[j].lon});
            if (distanza(a, b) > 15000.0 || std::min(fari[i].portata_m, fari[j].portata_m) < 15000.0) continue;
            const Vec2 m{(a.x + b.x) / 2.0, (a.y + b.y) / 2.0};
            for (double r = 0.0; r < 6000.0 && !punto; r += 250.0)
                for (int k = 0; k < 16 && !punto; ++k) {
                    const Vec2 q{m.x + r * std::sin(k * 0.3927), m.y + r * std::cos(k * 0.3927)};
                    if (fondale->profondita(q) > 30.0 && distanza(q, a) < 9000.0 && distanza(q, b) < 9000.0) punto = q;
                }
        }
    REQUIRE(punto.has_value());
    const auto gioca = [&](int ora_utc) {
        const auto f = fs::temp_directory_path() / "m688_prova_navigazione_fari.lua";
        std::ofstream(f) << "return { id = 'NAV_FARI', teatro = 'faroe', lato = 'usa', data = '1989-12-15', ora_utc = " << ora_utc << ","
                            " paesaggio_sonoro = false, script = {}, varianti = { { probabilita = 1, unita = {\n"
                            "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true, posizione = { x = " << punto->x
                         << ", y = " << punto->y << " }, quota_m = 0, rotta = 90, velocita_kn = 0, " << SUB << " },\n}, timer = {} } } }\n";
        Partita p(f.string(), true);
        std::map<std::string, double> pescaggi;
        for (const auto& [id, c] : p.classi) pescaggi[id] = c.pescaggio_m;
        p.m.imposta_fondale(fondale, pescaggi);
        p.avanza(2.0);
        std::optional<ValutazionePuntoNave> v;
        for (const auto& [id, val] : p.m.punti_nave_possibili())
            if (id == "fari") v = val;
        fs::remove(f);
        return v;
    };
    const auto notte = gioca(0), giorno = gioca(12);
    REQUIRE(notte.has_value());
    REQUIRE(giorno.has_value());
    CHECK_MESSAGE(notte->possibile, "motivo ", static_cast<int>(notte->motivo));
    CHECK(notte->sigma_nm <= 0.3 + 1e-9);
    CHECK(giorno->motivo == MotivoPuntoNave::notte);
}
