#include <doctest.h>

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <map>
#include <memory>
#include <string>
#include <tuple>
#include <vector>

#include "missioni/danni_missione.hpp"
#include "missioni/missione.hpp"
#include "missioni/registro_missione.hpp"
#include "nucleo/classi.hpp"
#include "nucleo/fondale.hpp"
#include "nucleo/geometria.hpp"
#include "nucleo/ordini.hpp"

using namespace m688;
namespace fs = std::filesystem;

// Il BQS-15 del 688 nella missione (docs/ricerca/06 §1.1 e §7; scheda S06 di docs/17): le mine nel settore di prua,
// l'evento del registro, i messaggi e la contro-scoperta dei suoi impulsi.

namespace {

const std::string SUB = "modello = { massima_kn = 32, virata_gradi_s = 1, accel_kn_s = 0.5, quota_m_s = 1, quota_max_m = 450, integrita = 200, rumore = 8, sonar = 19 }";

// 400 m d'acqua a ovest di x = 4000, 60 m a est
std::shared_ptr<const Fondale> fondale_bqs15() {
    std::vector<std::uint16_t> v(100 * 100);
    for (int r = 0; r < 100; ++r)
        for (int c = 0; c < 100; ++c) {
            const double x = (c + 0.5) * 100.0 - 5000.0;
            v[static_cast<std::size_t>(r) * 100 + c] = static_cast<std::uint16_t>(((x < 4000.0 ? -400.0 : -60.0) + 11000.0) / 0.25);
        }
    return std::make_shared<const Fondale>(100, 100, 100.0, 0.25, -11000.0, std::move(v));
}

// Il 688 fermo a 300 m con la prua a est, a 900 m da una CAPTOR ormeggiata (a 250 m), a 300 m da una Mk 67 sul fondo
// e con un'altra CAPTOR alle spalle (disarmate: la CAPTOR armata lancerebbe contro il 688); un Victor 2 km avanti e
// un Kilo (per difetto 10 km dietro), che ascoltano
std::string scena(double xk = -13000.0, double rk = 90.0) {
    const auto f = fs::temp_directory_path() / "m688_prova_bqs15.lua";
    std::ofstream(f) << "return { id = 'BQS15', teatro = 'faroe', paesaggio_sonoro = false, script = {},"
                        " mine = { { tipo = 'mk60', x = -2100, y = 0, lato = 'urss', armata = false }, { tipo = 'mk67', x = -2700, y = 0, lato = 'urss', armata = false },"
                        "          { tipo = 'mk60', x = -3900, y = 0, lato = 'urss', armata = false } },"
                        " varianti = { { probabilita = 1, unita = {\n"
                        "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true,"
                        "  posizione = { x = -3000, y = 0 }, quota_m = 300, rotta = 90, velocita_kn = 0, " << SUB << " },\n"
                        "{ id = 2, classe = 'victor3', categoria = 'sottomarino', flag = 25, posizione = { x = -1000, y = 0 },"
                        "  quota_m = 200, rotta = 270, velocita_kn = 0, " << SUB << " },\n"
                        "{ id = 3, classe = 'kilo', categoria = 'sottomarino', flag = 25, posizione = { x = " << xk << ", y = 0 },"
                        "  quota_m = 200, rotta = " << rk << ", velocita_kn = 0, " << SUB << " },\n"
                        "}, timer = {} } } }\n";
    return f.string();
}

struct Partita {
    std::map<std::string, ClasseNave> classi = carica_classi(M688_DATI "/classi.toml");
    Missione m;
    DanniMissione dm;
    explicit Partita(const std::string& file) : m(file, 1, false), dm(classi, M688_DATI, 1, RegolaDanni::simulazione) {
        m.imposta_fondale(fondale_bqs15());
    }
    void passo() { m.avanza(1.0 / 9.0); dm.avanza(m, 1.0 / 9.0); }
    void avanza(double s) { for (long i = 0; i < static_cast<long>(s * 9.0); ++i) passo(); }
};

bool messaggio(const Missione& m, int codice) {
    return std::any_of(m.messaggi().begin(), m.messaggi().end(), [&](const Messaggio& x) { return x.codice == codice; });
}

bool intercettato(const Missione& m, int ascoltatore) {
    const auto& cc = m.unita().at(ascoltatore).contatti;
    return cc.count(0) && cc.at(0).sensore == "intercettazione_sov";
}

}  // namespace

TEST_CASE("BQS-15: le mine nel settore di prua, l'evento del registro e chi sente gli impulsi (S06)") {
    const auto f = scena();
    Partita p(f);
    p.avanza(1.0);
    REQUIRE(p.m.con_acustica());
    REQUIRE(p.m.campo_minato().mine().size() == 3);
    CHECK(p.m.mine_scoperte().empty());
    CHECK_FALSE(p.m.bqs15(2, true));   // il Victor non lo ha
    CHECK_FALSE(intercettato(p.m, 2));

    REQUIRE(p.m.applica(Ordine{p.m.tempo(), TipoOrdine::bqs15, 0.0, 0.0, 1}));
    CHECK(p.m.bqs15_acceso(0));
    p.passo();   // la prima spazzata
    CHECK(messaggio(p.m, 450));
    CHECK(messaggio(p.m, 451));
    const auto& s = p.m.mine_scoperte();
    REQUIRE(s.size() == 2);   // la CAPTOR alle spalle resta fuori dal settore
    const auto& mine = p.m.campo_minato().mine();
    for (const auto& x : s) {
        const auto vera = std::find_if(mine.begin(), mine.end(), [&](const Mina& y) { return y.id == x.mina; });
        REQUIRE(vera != mine.end());
        CHECK(distanza(x.pos, vera->pos) < 40.0);
        CHECK(std::fabs(x.quota - vera->quota) < 30.0);
        CHECK(x.ormeggiata == (vera->tipo == "mk60"));
        CHECK(vera->pos.x > -3000.0);
    }
    // la contro-scoperta: il Victor nel fascio a 2 km sente l'impulso
    CHECK(intercettato(p.m, 2));

    // le spazzate seguenti aggiornano la stessa scoperta, senza nuovi eventi
    p.avanza(20.0);
    CHECK(s.size() == 2);
    CHECK(s.front().vista_t > s.front().scoperta_t);
    const auto& ev = p.m.registro().eventi();
    const auto scoperte = std::count_if(ev.begin(), ev.end(), [](const EventoMissione& e) { return e.tipo == TipoEvento::mina_scoperta; });
    CHECK(scoperte == 2);
    CHECK(std::any_of(ev.begin(), ev.end(), [](const EventoMissione& e) { return e.tipo == TipoEvento::mina_scoperta && e.livello == 1 && e.classe_arma == "mk60"; }));

    // spento: niente piu' spazzate
    REQUIRE(p.m.applica(Ordine{p.m.tempo(), TipoOrdine::bqs15, 0.0, 0.0, 0}));
    CHECK_FALSE(p.m.bqs15_acceso(0));
    CHECK(messaggio(p.m, 452));
    const double vista = s.front().vista_t;
    p.avanza(20.0);
    CHECK(s.front().vista_t == vista);
    CHECK(p.m.errori().empty());
    fs::remove(f);
}

TEST_CASE("BQS-15: la portata cala con la velocita' e con il fondo; niente senza l'Enhanced") {
    const auto f = scena();
    Missione m(f, 1, false);
    CHECK(m.portata_bqs15(0.0, true) == doctest::Approx(1100.0));
    CHECK(m.portata_bqs15(15.0, true) == doctest::Approx(770.0));
    CHECK(m.portata_bqs15(25.0, true) == doctest::Approx(440.0));
    CHECK(m.portata_bqs15(0.0, false) == doctest::Approx(450.0));
    CHECK(Missione::ha_bqs15("los_angeles"));
    CHECK_FALSE(Missione::ha_bqs15("victor3"));
    CHECK_FALSE(m.applica(Ordine{0.0, TipoOrdine::bqs15, 0.0, 0.0, 1}));   // il classico non lo ha
    CHECK(nome_ordine(TipoOrdine::bqs15) == "bqs15");
    CHECK(ordine_da_nome("bqs15") == TipoOrdine::bqs15);
    fs::remove(f);
}


TEST_CASE("BQS-15: fuori dal fascio di prua l'impulso arriva coi lobi laterali, 25 dB sotto") {
    const auto se_kilo = [](double x, double rotta) {
        const auto f = scena(x, rotta);
        Partita p(f);
        p.avanza(1.0);
        REQUIRE(p.m.applica(Ordine{p.m.tempo(), TipoOrdine::bqs15, 0.0, 0.0, 1}));
        p.passo();
        REQUIRE(intercettato(p.m, 3));
        const double se = p.m.unita().at(3).contatti.at(0).eccesso_db;
        fs::remove(f);
        return se;
    };
    const double avanti = se_kilo(-3000.0 + 4000.0, 270.0), dietro = se_kilo(-3000.0 - 4000.0, 90.0);
    CHECK(avanti - dietro == doctest::Approx(25.0).epsilon(0.1));
}

// La simmetria (runtime, 30/10/2026): un SSN dell'IA col BQS-15 tiene le mine che vede e il pilota passa al largo; uno
// Sturgeon, che non ha sonar da mine, ci passa sopra. In 80 m d'acqua, una Mk 67 disarmata sul fondo sulla rotta, del suo
// campo minato (noto: il BQS-15 si accende)
TEST_CASE("BQS-15: il sottomarino dell'IA vede la mina e la aggira; senza sonar da mine ci passa sopra") {
    const auto gioca = [](const std::string& classe) {
        const auto f = fs::temp_directory_path() / ("m688_prova_bqs15_ia_" + classe + ".lua");
        std::ofstream(f) << "local script = { V = function(u) m688.vai_a(u, 4000, 0, 6, '') m688.salva_modello(u) end }\n"
                            "return { id = 'BQS15_IA', teatro = 'faroe', paesaggio_sonoro = false, script = script,"
                            " mine = { { tipo = 'mk67', x = 1500, y = 0, lato = 'usa', armata = false } },"
                            " varianti = { { probabilita = 1, unita = {\n"
                            "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true,"
                            "  posizione = { x = -8000, y = -8000 }, quota_m = 30, rotta = 90, velocita_kn = 0, " << SUB << " },\n"
                            "{ id = 3, classe = '" << classe << "', categoria = 'sottomarino', flag = 9, posizione = { x = -1000, y = 0 },"
                            "  quota_m = 35, rotta = 90, velocita_kn = 6, " << SUB << " },\n"
                            "}, timer = { { dopo_s = 1, unita = 3, gestore = 'V' } } } } }\n";
        std::map<std::string, ClasseNave> classi = carica_classi(M688_DATI "/classi.toml");
        Missione m(f.string(), 1, false);
        DanniMissione dm(classi, M688_DATI, 1, RegolaDanni::simulazione);
        m.imposta_fondale(std::make_shared<const Fondale>(200, 200, 100.0, 1.0, -80.0, std::vector<std::uint16_t>(200 * 200, 0)));
        double vicina = 1e9;
        for (long i = 0; i < 900L * 9; ++i) {   // un quarto d'ora
            m.avanza(1.0 / 9.0);
            dm.avanza(m, 1.0 / 9.0);
            const auto& mine = m.campo_minato().mine();
            if (!mine.empty()) vicina = std::min(vicina, distanza(m.unita().at(3).pos, mine.front().pos));
        }
        const auto* s = m.mine_scoperte_ia(3);
        fs::remove(f);
        MESSAGE(classe, ": piu' vicino alla mina ", vicina, " m, mine viste ", s ? s->size() : 0u);
        return std::pair<double, std::size_t>{vicina, s ? s->size() : 0u};
    };
    const auto [la, viste] = gioca("los_angeles");
    CHECK(viste == 1);
    CHECK(la > 200.0);   // al largo (MARGINE_MINA_M 300, con la virata)
    const auto [st, viste_st] = gioca("sturgeon");
    CHECK(viste_st == 0);
    CHECK(st < 100.0);   // dritto sopra
}

// Le mine «in sicurezza» (la I17 di trailer-1, in pace; chiave `sicura` del campo della missione): al loro posto e alla loro
// quota, inerti, e il BQS-15 le vede. Il 688 fermo a 300 m con una PMR-2 sicura 600 m avanti a 300 m d'acqua sotto di lui
TEST_CASE("BQS-15: la mina in sicurezza resta inerte ma il sonar la vede") {
    const auto f = fs::temp_directory_path() / "m688_prova_bqs15_sicura.lua";
    std::ofstream(f) << "return { id = 'BQS15_SICURA', teatro = 'faroe', paesaggio_sonoro = false, script = {},"
                        " mine = { { tipo = 'pmr2', x = -2400, y = 0, lato = 'urss', armata = true, sicura = true } },"
                        " varianti = { { probabilita = 1, unita = {\n"
                        "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true,"
                        "  posizione = { x = -3000, y = 0 }, quota_m = 300, rotta = 90, velocita_kn = 0, " << SUB << " },\n"
                        "}, timer = {} } } }\n";
    Partita p(f.string());
    p.avanza(1.0);
    REQUIRE(p.m.applica(Ordine{p.m.tempo(), TipoOrdine::bqs15, 0.0, 0.0, 1}));
    p.avanza(700.0);   // oltre l'armamento (600 s)
    REQUIRE(p.m.campo_minato().mine().size() == 1);
    const Mina& m = p.m.campo_minato().mine().front();
    CHECK(m.sicura);
    CHECK(m.stato == Mina::Stato::inerte);
    CHECK(p.m.colpi().empty());
    CHECK(p.m.mine_scoperte().size() == 1);   // il BQS-15 la vede
    fs::remove(f);
}

// Il varco nella fila (principale, 30/10/2026: il pilota passava fra due mine a 200 m): una fila di CAPTOR in sicurezza della
// sua parte attraverso la rotta, a 400 m l'una dall'altra, con un varco di 1 km; il Los Angeles dell'IA passa dal varco,
// a piu' di 250 m da ognuna
TEST_CASE("BQS-15: il sottomarino dell'IA passa dal varco della fila di mine viste") {
    std::string mine;
    for (const double y : {-2000.0, -1600.0, -1200.0, -800.0, -400.0, 0.0, 1000.0, 1400.0, 1800.0, 2200.0})
        mine += "{ tipo = 'mk60', x = 1500, y = " + std::to_string(y) + ", lato = 'usa', armata = false, sicura = true }, ";
    const auto f = fs::temp_directory_path() / "m688_prova_bqs15_varco.lua";
    std::ofstream(f) << "local script = { V = function(u) m688.vai_a(u, 5000, 0, 6, '') m688.salva_modello(u) end }\n"
                        "return { id = 'BQS15_VARCO', teatro = 'faroe', paesaggio_sonoro = false, script = script, mine = { " << mine << "},"
                        " varianti = { { probabilita = 1, unita = {\n"
                        "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true,"
                        "  posizione = { x = -8000, y = -8000 }, quota_m = 30, rotta = 90, velocita_kn = 0, " << SUB << " },\n"
                        "{ id = 3, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, posizione = { x = -1500, y = 0 },"
                        "  quota_m = 150, rotta = 90, velocita_kn = 6, " << SUB << " },\n"
                        "}, timer = { { dopo_s = 1, unita = 3, gestore = 'V' } } } } }\n";
    std::map<std::string, ClasseNave> classi = carica_classi(M688_DATI "/classi.toml");
    Missione m(f.string(), 1, false);
    DanniMissione dm(classi, M688_DATI, 1, RegolaDanni::simulazione);
    m.imposta_fondale(std::make_shared<const Fondale>(200, 200, 100.0, 1.0, -300.0, std::vector<std::uint16_t>(200 * 200, 0)));
    double vicina = 1e9, y_passaggio = 1e9;
    double x_prima = m.unita().at(3).pos.x;
    for (long i = 0; i < 1500L * 9; ++i) {   // 25 minuti
        m.avanza(1.0 / 9.0);
        dm.avanza(m, 1.0 / 9.0);
        const Vec2 pos = m.unita().at(3).pos;
        for (const Mina& x : m.campo_minato().mine()) vicina = std::min(vicina, distanza(pos, x.pos));
        if (x_prima < 1500.0 && pos.x >= 1500.0) y_passaggio = pos.y;
        x_prima = pos.x;
    }
    const auto* s = m.mine_scoperte_ia(3);
    fs::remove(f);
    MESSAGE("mine viste ", s ? s->size() : 0u, ", piu' vicino ", vicina, " m, passa la fila a y = ", y_passaggio);
    CHECK((s && s->size() >= 3));
    CHECK(vicina > 250.0);
    CHECK(y_passaggio > 200.0);   // nel varco fra y = 0 e y = 1000
    CHECK(y_passaggio < 800.0);
}

// La mina comandata da terra (la S18 di trailer-1): una Mk 67 comandata del gruppo «nord» sulla rotta d'uno Sturgeon
// dell'IA in 40 m d'acqua. Senza l'ordine resta in sicurezza e lo Sturgeon ci passa sopra; passata alla stazione
// (m688.comanda_mine) scoppia quando e' sopra di lei, dopo il ritardo dell'operatore
TEST_CASE("mine: la mina comandata da terra scoppia solo dopo comanda_mine, col sottomarino sopra") {
    const auto gioca = [](bool comanda) {
        const auto f = fs::temp_directory_path() / "m688_prova_mina_comandata.lua";
        std::ofstream(f) << "local script = { V = function(u) m688.vai_a(u, 4000, 0, 6, '') m688.salva_modello(u) end,"
                            " C = function(u) m688.variabile(u, 1, m688.comanda_mine(u, 'nord') and 1 or 0) end }\n"
                            "return { id = 'MINA_COMANDATA', teatro = 'faroe', paesaggio_sonoro = false, script = script,"
                            " mine = { { tipo = 'mk67', x = 1500, y = 0, lato = 'urss', comandata = true, gruppo = 'nord' } },"
                            " varianti = { { probabilita = 1, unita = {\n"
                            "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true,"
                            "  posizione = { x = -8000, y = -8000 }, quota_m = 20, rotta = 90, velocita_kn = 0, " << SUB << " },\n"
                            "{ id = 3, classe = 'sturgeon', categoria = 'sottomarino', flag = 9, posizione = { x = -500, y = 0 },"
                            "  quota_m = 20, rotta = 90, velocita_kn = 6, " << SUB << " },\n"
                            "}, timer = { { dopo_s = 1, unita = 3, gestore = 'V' }"
                         << (comanda ? ", { dopo_s = 60, unita = 0, gestore = 'C' }" : "") << " } } } }\n";
        std::map<std::string, ClasseNave> classi = carica_classi(M688_DATI "/classi.toml");
        Missione m(f.string(), 1, false);
        DanniMissione dm(classi, M688_DATI, 1, RegolaDanni::simulazione);
        m.imposta_fondale(std::make_shared<const Fondale>(200, 200, 100.0, 1.0, -40.0, std::vector<std::uint16_t>(200 * 200, 0)));
        double sopra_da = -1.0, scoppio = -1.0;
        for (long i = 0; i < 1200L * 9 && scoppio < 0.0; ++i) {
            m.avanza(1.0 / 9.0);
            dm.avanza(m, 1.0 / 9.0);
            const Mina& x = m.campo_minato().mine().front();
            // l'operatore lo vede entro tre raggi della Mk 67 (60 m): da li' il suo ritardo
            if (sopra_da < 0.0 && distanza(m.unita().at(3).pos, x.pos) <= 3.0 * 60.0) sopra_da = m.tempo();
            if (x.stato == Mina::Stato::scoppiata) scoppio = m.tempo();
        }
        const bool comandate = m.variabili().count(1) && m.variabili().at(1) == 1;
        fs::remove(f);
        MESSAGE(std::string(comanda ? "con" : "senza"), " l'ordine: sopra la mina a ", sopra_da, " s, scoppio a ", scoppio, " s");
        return std::tuple<double, double, bool>{sopra_da, scoppio, comandate};
    };
    const auto [sopra, scoppio, comandate] = gioca(true);
    CHECK(comandate);
    REQUIRE(sopra > 0.0);
    REQUIRE(scoppio > 0.0);
    CHECK(scoppio - sopra >= 10.0);   // il ritardo dell'operatore
    CHECK(scoppio - sopra <= 31.0);
    const auto [sopra2, scoppio2, comandate2] = gioca(false);
    CHECK(sopra2 > 0.0);
    CHECK(scoppio2 < 0.0);   // in sicurezza: niente
    CHECK_FALSE(comandate2);
}

// la simmetria (disassemblatore e principale, 31/10/2026): un Alfa del giocatore ha il suo sonar delle mine, l'MG-24, con
// lo stesso ordine del BQS-15; i suoi messaggi (496 acceso, 497 mina, 498 spento) e la portata ridotta della sua voce
TEST_CASE("BQS-15: l'Alfa del giocatore accende l'MG-24 e vede le mine coi suoi messaggi") {
    const auto f = fs::temp_directory_path() / "m688_prova_mg24.lua";
    std::ofstream(f) << "return { id = 'MG24', teatro = 'faroe', paesaggio_sonoro = false, script = {},"
                        " mine = { { tipo = 'mk60', x = -2600, y = 0, lato = 'usa', armata = false }, { tipo = 'mk67', x = -2800, y = 0, lato = 'usa', armata = false } },"
                        " varianti = { { probabilita = 1, unita = {\n"
                        "{ id = 0, classe = 'alfa', categoria = 'sottomarino', flag = 17, giocatore = true,"
                        "  posizione = { x = -3000, y = 0 }, quota_m = 300, rotta = 90, velocita_kn = 0, " << SUB << " },\n"
                        "}, timer = {} } } }\n";
    Partita p(f.string());
    p.avanza(1.0);
    REQUIRE(p.m.con_acustica());
    REQUIRE(p.m.applica(Ordine{p.m.tempo(), TipoOrdine::bqs15, 0.0, 0.0, 1}));
    CHECK(p.m.bqs15_acceso(0));
    p.passo();
    CHECK(messaggio(p.m, 496));
    CHECK_FALSE(messaggio(p.m, 450));
    p.avanza(20.0);
    MESSAGE("mine viste dall'MG-24: ", p.m.mine_scoperte().size());
    CHECK_FALSE(p.m.mine_scoperte().empty());
    CHECK(messaggio(p.m, 497));
    REQUIRE(p.m.applica(Ordine{p.m.tempo(), TipoOrdine::bqs15, 0.0, 0.0, 0}));
    CHECK(messaggio(p.m, 498));
    CHECK(p.m.errori().empty());
    fs::remove(f);
}
