#include <doctest.h>

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <map>
#include <memory>
#include <string>
#include <tuple>

#include "missioni/danni_missione.hpp"
#include "missioni/danni_sottomarini.hpp"
#include "missioni/missione.hpp"
#include "missioni/registro_missione.hpp"
#include "nucleo/asw_aereo.hpp"
#include "nucleo/classi.hpp"
#include "nucleo/fondale.hpp"
#include "nucleo/geometria.hpp"

using namespace m688;
namespace fs = std::filesystem;

// Fase 4 A delle interazioni fra unita' dell'IA (docs/18_interazioni_ia.md; nota condivisa ia-fase4-contratto.md): la
// difesa dell'IA. Evasione ed esche su un siluro ostile qualsiasi, le navi che si allontanano, la ritirata del
// sottomarino colpito, l'anticollisione.

namespace {

const std::string SUB = "modello = { massima_kn = 32, virata_gradi_s = 1, accel_kn_s = 0.5, quota_m_s = 1, quota_max_m = 450, integrita = 200, rumore = 8, sonar = 19 }";
const std::string NAVE = "modello = { massima_kn = 32, virata_gradi_s = 2, accel_kn_s = 0.5, quota_m_s = 1, quota_max_m = 0, integrita = 100, rumore = 10, sonar = 20 }";
const std::string ARMA = "modello = { massima_kn = 45, virata_gradi_s = 5, accel_kn_s = 5, quota_m_s = 5, quota_max_m = 450, integrita = 1, rumore = 30, sonar = 0 }";

std::string scena(const std::string& nome, const std::string& situazione, const std::string& altre, const std::string& script = "",
                  const std::string& timer = "") {
    const auto f = fs::temp_directory_path() / ("m688_prova_difesa_" + nome + ".lua");
    std::ofstream(f) << "return { id = 'DIFESA_" << nome << "', teatro = 'faroe', lato = 'usa', data = '1989-06-15', ora_utc = 12,"
                        " paesaggio_sonoro = false, carriera = { situazione = '" << situazione << "' },"
                        " script = {" << script << "}, varianti = { { probabilita = 1, unita = {\n"
                        "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true, posizione = { x = -90000, y = -80000 },"
                        "  quota_m = 300, rotta = 90, velocita_kn = 3, " << SUB << " },\n" << altre << "\n}, timer = {" << timer << "} } } }\n";
    return f.string();
}

struct Partita {
    std::map<std::string, ClasseNave> classi = carica_classi(M688_DATI "/classi.toml");
    Missione m;
    DanniMissione dm;
    explicit Partita(const std::string& file) : m(file, 1, false), dm(classi, M688_DATI, 1, RegolaDanni::simulazione) {}
    void avanza(double s) { for (long i = 0; i < static_cast<long>(s * 9.0); ++i) { m.avanza(1.0 / 9.0); dm.avanza(m, 1.0 / 9.0); } }
    int ritirate(const std::string& causa, int unita) const {
        return static_cast<int>(std::count_if(m.ritirate_ia().begin(), m.ritirate_ia().end(),
                                              [&](const Missione::RitirataIa& r) { return r.causa == causa && r.unita == unita; }));
    }
};

// un siluro americano (5) che corre verso un Victor (3) a 2,5 km, e una Krivak (4) a 2 km dal siluro
const std::string SILURO_SUL_VICTOR =
    "{ id = 3, classe = 'victor3', categoria = 'sottomarino', flag = 17, posizione = { x = 0, y = 0 }, quota_m = 150, rotta = 90, velocita_kn = 5, " + SUB + " },"
    "{ id = 4, classe = 'krivak2', categoria = 'nave', flag = 18, posizione = { x = 2000, y = -2500 }, quota_m = 0, rotta = 90, velocita_kn = 5, " + NAVE + " },"
    "{ id = 5, classe = 'mk48', categoria = 'arma', flag = 41, posizione = { x = 0, y = -2500 }, quota_m = 150, rotta = 0, velocita_kn = 40, " + ARMA + " },";

}  // namespace

TEST_CASE("difesa: il sottomarino dell'IA evade e lancia esche su un siluro qualsiasi, la nave si allontana" * doctest::skip(!fs::exists(M688_DATI "/asw_aereo.toml"))) {
    const auto f = scena("siluro", "guerra", SILURO_SUL_VICTOR);
    Partita p(f);
    p.avanza(60.0);
    REQUIRE(p.m.con_forze());
    CHECK(p.m.manovre_evasione(3) >= 1);
    CHECK(std::any_of(p.m.esche_ia().begin(), p.m.esche_ia().end(), [](const Missione::EscaIa& e) { return e.unita == 3; }));
    CHECK(p.ritirate("siluro", 4) == 1);
    const auto& reg = p.m.eventi_missione();
    CHECK(std::any_of(reg.begin(), reg.end(), [](const EventoMissione& e) { return e.tipo == TipoEvento::esca_ia && e.unita == 3; }));
    // l'evento dello spettatore (fase 5): la manovra d'evasione nuova, non nota all'equipaggio del giocatore
    CHECK(std::any_of(reg.begin(), reg.end(), [](const EventoMissione& e) { return e.tipo == TipoEvento::evasione_ia && e.unita == 3 && !e.noto; }));
    fs::remove(f);
}

TEST_CASE("difesa: senza, nessuna evasione ne' esca fuori dalla caccia aerea" * doctest::skip(!fs::exists(M688_DATI "/asw_aereo.toml"))) {
    const auto f = scena("senza", "guerra", SILURO_SUL_VICTOR);
    Partita p(f);
    p.m.usa_difesa(false);
    p.avanza(60.0);
    CHECK(p.m.manovre_evasione(3) == 0);
    CHECK(p.m.esche_ia().empty());
    CHECK(p.m.ritirate_ia().empty());
    fs::remove(f);
}

TEST_CASE("difesa: il sottomarino colpito si ritira da chi ha lanciato" * doctest::skip(!fs::exists(M688_DATI "/interazioni.toml"))) {
    // una Koni (2) che attacca un 688 dell'IA (3) a 1,5 km con una carica (ATTACK, il dado): una classe senza il siluro
    // antisom vero (missione_antisom_navi.cpp; la Type 22 ha il Mk 46 dal 31/10/2026), perche' chi ha il siluro non ha cariche
    const auto f = scena("ritirata", "guerra",
        "{ id = 2, classe = 'koni', categoria = 'nave', flag = 18, posizione = { x = 0, y = -1500 }, quota_m = 0, rotta = 90, velocita_kn = 5, " + NAVE + " },"
        "{ id = 3, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, posizione = { x = 0, y = 0 }, quota_m = 150, rotta = 90, velocita_kn = 5, " + SUB + " },",
        "A = function(u) m688.bersaglio(u, 3) m688.attacca(u, 1, 1) m688.salva_modello(u) end", "{ dopo_s = 5, unita = 2, gestore = 'A' }");
    Partita p(f);
    p.avanza(30.0);
    REQUIRE_FALSE(p.m.colpi().empty());
    CHECK(p.ritirate("colpito", 3) == 1);
    const auto& v = p.m.unita().at(3);
    if (!v.distrutta) CHECK(std::fabs(normalizza_180(v.rotta_ordinata)) < 30.0);   // a nord, lontano dalla Koni
    fs::remove(f);
}

TEST_CASE("difesa: due sottomarini in rotta di collisione, l'IA cambia quota e accosta" * doctest::skip(!fs::exists(M688_DATI "/interazioni.toml"))) {
    // un Victor (3) e un 688 dell'IA (6) a 900 m, prua contro prua, alla stessa quota
    const auto f = scena("collisione", "pace",
        "{ id = 3, classe = 'victor3', categoria = 'sottomarino', flag = 17, posizione = { x = 0, y = 0 }, quota_m = 150, rotta = 0, velocita_kn = 10, " + SUB + " },"
        "{ id = 6, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, posizione = { x = 20, y = 900 }, quota_m = 150, rotta = 180, velocita_kn = 10, " + SUB + " },");
    Partita p(f);
    double quando = -1.0;
    for (int s = 0; s < 60 && quando < 0.0; ++s) {
        p.avanza(1.0);
        if (p.ritirate("collisione", 3) + p.ritirate("collisione", 6) > 0) quando = p.m.tempo();
    }
    CHECK(quando > 0.0);
    const auto& a = p.m.unita().at(3);
    const auto& b = p.m.unita().at(6);
    CHECK(std::max(std::fabs(a.quota_ordinata - 150.0), std::fabs(b.quota_ordinata - 150.0)) >= 30.0);
    fs::remove(f);
}

// le navi non investono il sottomarino vicino alla superficie (S16, 30/10/2026: la fregata in caccia passava sopra l'Alfa
// che emergeva): una Perry a 15 nodi dritta su un Victor emerso e fermo a 1 km, che vede al radar, accosta via e non gli
// passa sopra
TEST_CASE("difesa: la nave dell'IA accosta via dal sottomarino in superficie sulla sua rotta" * doctest::skip(!fs::exists(M688_DATI "/interazioni.toml"))) {
    const auto f = scena("nave_urto", "pace",
        "{ id = 2, classe = 'perry', categoria = 'nave', flag = 10, posizione = { x = 0, y = 0 }, quota_m = 0, rotta = 0, velocita_kn = 15, " + NAVE + " },"
        "{ id = 3, classe = 'victor3', categoria = 'sottomarino', flag = 17, posizione = { x = 10, y = 1000 }, quota_m = 0, rotta = 90, velocita_kn = 0, " + SUB + " },");
    Partita p(f);
    double minima = 1e9;
    for (int s = 0; s < 180; ++s) {
        p.avanza(1.0);
        minima = std::min(minima, distanza(p.m.unita().at(2).pos, p.m.unita().at(3).pos));
    }
    MESSAGE("la Perry passa a ", minima, " m dal Victor");
    CHECK(p.ritirate("collisione", 2) >= 1);
    CHECK(minima > 100.0);
    fs::remove(f);
}

TEST_CASE("reazioni: un alleato attaccato in guerra, la nave antisom va sul datum, il sottomarino amico ascolta" * doctest::skip(!fs::exists(M688_DATI "/interazioni.toml"))) {
    // una Krivak (3) colpisce con una carica la Perry 4; la Perry 2 a 10 km e' l'antisom piu' vicina; un 688 dell'IA (6)
    // a 12 km. Col blocco dell'editor: non e' un'originale
    const auto f = fs::temp_directory_path() / "m688_prova_reazioni.lua";
    std::ofstream(f) << "return { id = 'REAZIONI', teatro = 'faroe', lato = 'usa', data = '1989-06-15', ora_utc = 12, editor = {},"
                        " paesaggio_sonoro = false, carriera = { situazione = 'guerra' },"
                        " script = { A = function(u) m688.bersaglio(u, 4) m688.attacca(u, 1, 1) m688.salva_modello(u) end },"
                        " varianti = { { probabilita = 1, unita = {\n"
                        "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true, posizione = { x = -90000, y = -80000 },"
                        "  quota_m = 300, rotta = 90, velocita_kn = 3, " << SUB << " },\n"
                        "{ id = 2, classe = 'perry', categoria = 'nave', flag = 10, posizione = { x = 10000, y = 0 }, quota_m = 0, rotta = 90, velocita_kn = 5, " << NAVE << " },\n"
                        "{ id = 3, classe = 'krivak2', categoria = 'nave', flag = 18, posizione = { x = 0, y = 1500 }, quota_m = 0, rotta = 90, velocita_kn = 5, " << NAVE << " },\n"
                        "{ id = 4, classe = 'perry', categoria = 'nave', flag = 10, posizione = { x = 0, y = 0 }, quota_m = 0, rotta = 90, velocita_kn = 5, " << NAVE << " },\n"
                        "{ id = 6, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, posizione = { x = -12000, y = 0 }, quota_m = 150, rotta = 90, velocita_kn = 8, " << SUB << " },\n"
                        "}, timer = { { dopo_s = 5, unita = 3, gestore = 'A' } } } } }\n";
    Partita p(f.string());
    p.avanza(30.0);
    const auto& r = p.m.reazioni_ia();
    const auto c_e = [&](int u, const char* reazione) {
        return std::any_of(r.begin(), r.end(), [&](const Missione::ReazioneIa& x) { return x.unita == u && x.reazione == reazione && x.causa == 4; });
    };
    CHECK(c_e(2, "datum"));
    CHECK(c_e(6, "ascolta"));
    const auto& reg = p.m.eventi_missione();
    CHECK(std::any_of(reg.begin(), reg.end(), [](const EventoMissione& e) { return e.tipo == TipoEvento::reazione_ia; }));
    // dopo il ritardo della voce il 688 rallenta ad ascoltare
    p.avanza(150.0);
    CHECK(p.m.unita().at(6).velocita_ordinata <= 3.0 + 1e-6);
    fs::remove(f);
}

TEST_CASE("pace: la nave pedina il sottomarino che ha classificato, e due navi di parti diverse tengono i 1000 yd" * doctest::skip(!fs::exists(M688_DATI "/interazioni.toml"))) {
    // in pace una Perry (2) che sente un Victor rumoroso (3) a 6 km; una Krivak (4) a 700 m dalla Perry
    const auto f = fs::temp_directory_path() / "m688_prova_pace.lua";
    std::ofstream(f) << "return { id = 'PACE', teatro = 'faroe', lato = 'usa', data = '1989-06-15', ora_utc = 12, editor = {},"
                        " paesaggio_sonoro = false, carriera = { situazione = 'pace' }, script = {}, varianti = { { probabilita = 1, unita = {\n"
                        "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true, posizione = { x = -90000, y = -80000 },"
                        "  quota_m = 300, rotta = 90, velocita_kn = 3, " << SUB << " },\n"
                        "{ id = 2, classe = 'perry', categoria = 'nave', flag = 10, posizione = { x = 0, y = 0 }, quota_m = 0, rotta = 90, velocita_kn = 5, " << NAVE << " },\n"
                        "{ id = 3, classe = 'victor3', categoria = 'sottomarino', flag = 17, posizione = { x = 0, y = 6000 }, quota_m = 60, rotta = 90, velocita_kn = 15, " << SUB << " },\n"
                        "{ id = 4, classe = 'krivak2', categoria = 'nave', flag = 18, posizione = { x = 700, y = 0 }, quota_m = 0, rotta = 90, velocita_kn = 5, " << NAVE << " },\n"
                        "}, timer = {} } } }\n";
    Partita p(f.string());
    p.avanza(20.0);
    CHECK_FALSE(p.m.incsea().empty());   // a 700 m: violazione, e la Krivak e la Perry si aprono
    p.avanza(900.0);
    const auto& r = p.m.reazioni_ia();
    const bool pedina = std::any_of(r.begin(), r.end(), [](const Missione::ReazioneIa& x) { return x.unita == 2 && x.reazione == "pedina" && x.causa == 3; });
    MESSAGE("Perry sul Victor: pedina ", pedina, ", distanza ", distanza(p.m.unita().at(2).pos, p.m.unita().at(3).pos));
    CHECK(pedina);
    fs::remove(f);
}

TEST_CASE("pace: l'elicottero dell'ASW aereo non passa sotto i 500 ft entro 1 nm dalla nave dell'altra parte (INCSEA)" * doctest::skip(!fs::exists(M688_DATI "/asw_aereo.toml") || !fs::exists(M688_DATI "/interazioni.toml"))) {
    // in pace un SH-60B (1) caccia un Alfa (3) a 20 nodi, rumoroso; una Krivak (4) e' ferma sul punto della segnalazione,
    // dove l'elicottero arriva comunque (la prova e' dell'INCSEA, non della scoperta). Con l'accordo
    // l'elicottero sale da 2 nm, senza (`incsea = false`) passa basso e il registro lo dice
    const auto prova = [](bool incsea, bool sposta) {
        bool spostata = false;
        const auto f = fs::temp_directory_path() / "m688_prova_incsea_aereo.lua";
        const std::string volo = "modello = { massima_kn = 330, virata_gradi_s = 3, accel_kn_s = 2, quota_m_s = 1, quota_max_m = 500, integrita = 10, rumore = 0, sonar = 20 }";
        std::ofstream(f) << "return { id = 'INCSEA_AEREO', teatro = 'faroe', lato = 'usa', data = '1989-06-15', ora_utc = 12, editor = {},"
                            " paesaggio_sonoro = false, carriera = { situazione = 'pace' }," << (incsea ? "" : " incsea = false,")
                         << " aereo_asw = { { unita = 1, missione = 'caccia', situazione = 'pace', zona = { x = 0, y = 0, raggio_m = 20000 },"
                            " cue = { tipo = 'sosus', x = 0, y = 1000, raggio_m = 3000, ritardo_s = 30, rotta = 0, velocita_kn = 20 } } },"
                            " script = {}, varianti = { { probabilita = 1, unita = {\n"
                            "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true, posizione = { x = -90000, y = -80000 },"
                            "  quota_m = 300, rotta = 90, velocita_kn = 3, " << SUB << " },\n"
                            "{ id = 1, classe = 'sh60b', categoria = 'aereo', flag = 11, posizione = { x = 0, y = -15000 }, quota_m = 0, rotta = 0, velocita_kn = 100, " << volo << " },\n"
                            "{ id = 3, classe = 'alfa', categoria = 'sottomarino', flag = 17, posizione = { x = 0, y = 0 }, quota_m = 100, rotta = 0, velocita_kn = 20, " << SUB << " },\n"
                            "{ id = 4, classe = 'krivak2', categoria = 'nave', flag = 18, posizione = { x = 300, y = 1000 }, quota_m = 0, rotta = 0, velocita_kn = 0, " << NAVE << " },\n"
                            "}, timer = {} } } }\n";
        Partita p(f.string());
        double minima = 1e9;   // la quota piu' bassa entro 1 nm dalla Krivak, fuori dall'hover
        bool vicino = false;
        for (int k = 0; k < 2400; ++k) {
            p.avanza(1.0);
            const auto& u = p.m.unita();
            if (!u.count(1) || !u.count(4)) break;
            const auto* c = p.m.controllore_asw(1);
            // la Krivak che compare a 200 m dall'elicottero gia' basso (un passaggio che l'aereo non ha potuto evitare)
            if (sposta && !spostata && p.m.quota_aerea(u.at(1)) < 100.0 && distanza(u.at(1).pos, u.at(4).pos) > 4000.0) {
                p.m.trova(4)->pos = {u.at(1).pos.x + 200.0, u.at(1).pos.y};
                spostata = true;
            }
            if (distanza(u.at(1).pos, u.at(4).pos) < 1852.0 && c && !c->volo().hover) {
                vicino = true;
                minima = std::min(minima, p.m.quota_aerea(u.at(1)));
            }
        }
        const bool violazione = std::any_of(p.m.incsea().begin(), p.m.incsea().end(), [](const Missione::ViolazioneIncsea& v) { return v.unita == 1; });
        if (violazione)   // il registro la scrive da aereo
            CHECK(std::any_of(p.m.eventi_missione().begin(), p.m.eventi_missione().end(), [](const EventoMissione& e) {
                return e.tipo == TipoEvento::incsea && e.testo_it.find("sotto i 500 piedi") != std::string::npos;
            }));
        fs::remove(f);
        MESSAGE("incsea ", incsea, ": vicino ", vicino, ", quota minima ", minima, " m, violazione ", violazione);
        return std::make_tuple(vicino, minima, violazione);
    };
    const auto [vicino, minima, violazione] = prova(true, false);
    REQUIRE(vicino);
    CHECK(minima >= 152.4);
    (void)violazione;   // l'hover col sonar a immersione vicino alla Krivak puo' darne una: e' giusto
    const auto [vicino_senza, minima_senza, violazione_senza] = prova(false, false);
    REQUIRE(vicino_senza);
    CHECK(minima_senza < 152.4);
    CHECK_FALSE(violazione_senza);   // l'accordo tolto: niente registro
    // la Krivak che compare accanto all'elicottero basso: la violazione nel registro, poi l'elicottero sale
    const auto [vicino_dentro, minima_dentro, violazione_dentro] = prova(true, true);
    CHECK(violazione_dentro);
    (void)vicino_dentro;
    (void)minima_dentro;
}

TEST_CASE("pace: il pedinamento del giocatore fatto dallo script e' un reazione_ia «pedina»" * doctest::skip(!fs::exists(M688_DATI "/interazioni.toml"))) {
    // il 688 in superficie (la Krivak lo vede) e una Krivak a 5 km che lo tiene come bersaglio per script
    const auto f = fs::temp_directory_path() / "m688_prova_pedina_script.lua";
    std::ofstream(f) << "return { id = 'PEDINA_SCRIPT', teatro = 'faroe', lato = 'usa', data = '1989-06-15', ora_utc = 12, editor = {},"
                        " paesaggio_sonoro = false, carriera = { situazione = 'pace' },"
                        " script = { B = function(u) m688.bersaglio(u, 0) m688.salva_modello(u) end },"
                        " varianti = { { probabilita = 1, unita = {\n"
                        "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true, posizione = { x = 0, y = 0 },"
                        "  quota_m = 0, rotta = 90, velocita_kn = 0, " << SUB << " },\n"
                        "{ id = 3, classe = 'krivak2', categoria = 'nave', flag = 18, posizione = { x = 0, y = 5000 }, quota_m = 0, rotta = 90, velocita_kn = 0, " << NAVE << " },\n"
                        "}, timer = { { dopo_s = 2, unita = 3, gestore = 'B' }, { dopo_s = 100, unita = 3, gestore = 'B' }, { dopo_s = 200, unita = 3, gestore = 'B' },"
                        " { dopo_s = 300, unita = 3, gestore = 'B' } } } } }\n";
    Partita p(f.string());
    p.avanza(400.0);
    const auto& reg = p.m.eventi_missione();
    CHECK(std::any_of(reg.begin(), reg.end(), [](const EventoMissione& e) {
        return e.tipo == TipoEvento::reazione_ia && e.unita == 3 && e.altra == 0 && e.classe_arma == "pedina";
    }));
    fs::remove(f);
}

// S08 «Valona» (secondo collaudo della libreria, 30/10/2026): i quattro Foxtrot neutrali ai moli, fermi e senza
// comportamento, partivano a 10 nodi e uno si incagliava. Due cause: il pedinamento in pace (i neutrali non pedinano) e
// l'anticollisione che, finita, rimetteva la velocita' del pedinamento (e un battello fermo in superficie che si
// immergeva per evitare chi passa). Mezz'ora come nel collaudo: restano ai moli
TEST_CASE("S08: i Foxtrot neutrali ai moli restano fermi (pedinamento e anticollisione)" *
          doctest::skip(!fs::exists(std::string(M688_MISSIONI) + "/libreria/LIB_S08_VALONA.lua"))) {
    std::map<std::string, ClasseNave> classi = carica_classi(M688_DATI "/classi.toml");
    Missione m(std::string(M688_MISSIONI) + "/libreria/LIB_S08_VALONA.lua", 1);
    DanniMissione dm(classi, M688_DATI, 1, RegolaDanni::simulazione);
    DanniSottomarini ds(classi, M688_DATI, 1);
    std::map<std::string, double> pescaggi;
    for (const auto& [id, c] : classi) pescaggi[id] = c.pescaggio_m;
    m.imposta_fondale(std::make_shared<const Fondale>(Fondale::carica(M688_DATI "/teatri/" + m.teatro() + ".fdl")), pescaggi);
    m.radio_enhanced();
    if (const auto* g = m.giocatore()) {
        ds.prepara(m, g->id);
        auto* u = m.trova(g->id);   // come lo strumento del collaudo: il giocatore fermo che ascolta
        u->velocita_ordinata = 0.0;
        u->velocita_salvata = 0.0;
    }
    std::map<int, Vec2> partenza;
    for (const auto& [id, u] : m.unita())
        if (u.classe == "foxtrot" && (u.flag & 0x18) == 0) partenza[id] = u.pos;
    REQUIRE(partenza.size() == 4);
    double lontano = 0.0;
    bool incagliato = false;
    for (int k = 0; k < 180; ++k) {
        for (int i = 0; i < 90; ++i) { m.avanza(1.0 / 9.0); dm.avanza(m, 1.0 / 9.0); ds.avanza(m, 1.0 / 9.0); m.raccogli_danni(&ds, &dm); }
        for (const auto& [id, p] : partenza) {
            const UnitaClassica& u = m.unita().at(id);
            lontano = std::max(lontano, distanza(u.pos, p));
            incagliato = incagliato || u.incagliata;
        }
    }
    MESSAGE("i Foxtrot ai moli: al piu' a ", lontano, " m dal posto");
    CHECK(lontano < 100.0);
    CHECK_FALSE(incagliato);
    for (const auto& r : m.reazioni_ia()) CHECK_FALSE((partenza.count(r.unita) && r.reazione == "pedina"));
}

// l'urto fra due unita' dell'IA nel registro (fase 5, lo spettatore): un Victor e un 688 dell'IA prua contro prua alla
// stessa quota, senza la difesa (niente anticollisione): collisione_ia con l'energia
TEST_CASE("difesa: senza l'anticollisione due sottomarini dell'IA si urtano, e il registro ha collisione_ia" * doctest::skip(!fs::exists(M688_DATI "/interazioni.toml"))) {
    const auto f = scena("urto_ia", "pace",
        "{ id = 3, classe = 'victor3', categoria = 'sottomarino', flag = 17, posizione = { x = 0, y = 0 }, quota_m = 150, rotta = 0, velocita_kn = 10, " + SUB + " },"
        "{ id = 6, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, posizione = { x = 0, y = 900 }, quota_m = 150, rotta = 180, velocita_kn = 10, " + SUB + " },");
    Partita p(f);
    p.m.usa_difesa(false);
    p.avanza(180.0);
    const auto& reg = p.m.eventi_missione();
    const auto e = std::find_if(reg.begin(), reg.end(), [](const EventoMissione& x) { return x.tipo == TipoEvento::collisione_ia; });
    REQUIRE(e != reg.end());
    CHECK(((e->unita == 3 && e->altra == 6) || (e->unita == 6 && e->altra == 3)));
    CHECK(e->valore > 0.0);
    // dopo l'urto si separano (la principale, 30/10/2026: nella S15 l'Ugra urtava il Charlie 86 volte): la strisciata del
    // passaggio (gli urti ogni 1-2 s a contatto continuo) finisce, e poi non si riurtano
    const auto urti = std::count_if(reg.begin(), reg.end(), [](const EventoMissione& x) { return x.tipo == TipoEvento::collisione_ia; });
    std::string quando;
    for (const auto& u : p.m.urti()) quando += " " + std::to_string(static_cast<int>(u.urto.tempo_s));
    MESSAGE("urti fra il Victor e il 688 dell'IA in 3 minuti: ", urti, ", a", quando, " s");
    REQUIRE_FALSE(p.m.urti().empty());
    CHECK(p.m.urti().back().urto.tempo_s - p.m.urti().front().urto.tempo_s < 15.0);
    fs::remove(f);
}


// «Ci stanno pingando» per episodio (principale, 30/10/2026: 3393 righe in due ore nella S15): una Krivak a 3 km dal 688 pinga ogni
// 20 s per 10 minuti, tace 5 minuti, poi riprende: due eventi, col numero dei ping; le statistiche contano i ping
TEST_CASE("registro: i ping subiti in un evento per episodio, col conteggio" * doctest::skip(!fs::exists(M688_DATI "/interazioni.toml"))) {
    const auto f = scena("ping_episodi", "crisi",
        "{ id = 2, classe = 'krivak2', categoria = 'nave', flag = 18, posizione = { x = -90000, y = -77000 }, quota_m = 0, rotta = 90, velocita_kn = 5, " + NAVE + " },",
        "P = function(u) if not m688.variabile_uguale(u, 1, 1) then m688.impulso_sonar(u) end m688.dopo(u, 20, 'P') end,"
        " S = function(u) m688.variabile(u, 1, 1) end, R = function(u) m688.variabile(u, 1, 0) end",
        "{ dopo_s = 10, unita = 2, gestore = 'P' }, { dopo_s = 600, unita = 2, gestore = 'S' }, { dopo_s = 900, unita = 2, gestore = 'R' }");
    Partita p(f);
    p.avanza(1200.0);
    std::vector<const EventoMissione*> ping;
    for (const auto& e : p.m.eventi_missione())
        if (e.tipo == TipoEvento::ping_subito) ping.push_back(&e);
    MESSAGE("episodi di ping: ", ping.size(), ping.empty() ? std::string() : ", il primo con " + std::to_string(static_cast<int>(ping.front()->valore)) + " ping");
    REQUIRE(ping.size() == 2);
    CHECK(ping.front()->valore >= 20.0);
    CHECK(ping.front()->testo_it.find(" ping)") != std::string::npos);
    CHECK(ping.back()->tempo > 900.0);
    fs::remove(f);
}

// un sonar di scafo dell'IA non ripinga prima del ritorno dell'eco (giro della libreria, 30/10/2026: 2,4 ping al secondo
// sul giocatore nella S12): uno script che chiama il ping ogni 2 s per un minuto fa al piu' un ping ogni 10 s
TEST_CASE("difesa: la nave dell'IA non pinga piu' spesso dell'eco della sua scala" * doctest::skip(!fs::exists(M688_DATI "/interazioni.toml"))) {
    const auto f = scena("ping_cadenza", "crisi",
        "{ id = 2, classe = 'perry', categoria = 'nave', flag = 10, posizione = { x = -90000, y = -77000 }, quota_m = 0, rotta = 90, velocita_kn = 5, " + NAVE + " },",
        "P = function(u) m688.impulso_sonar(u) m688.dopo(u, 2, 'P') end", "{ dopo_s = 5, unita = 2, gestore = 'P' }");
    Partita p(f);
    p.avanza(65.0);
    const auto n = std::count_if(p.m.ping_unita().begin(), p.m.ping_unita().end(), [](const Missione::PingUnita& x) { return x.unita == 2; });
    MESSAGE("ping della Perry in un minuto con lo script ogni 2 s: ", n);
    CHECK(n >= 5);
    CHECK(n <= 7);
    fs::remove(f);
}

// gli SSBN non pedinano (trailer-1, la S14: l'Ohio scopriva l'Alfa e lo seguiva invece d'immergersi): un Ohio dell'IA in
// pace col Victor rumoroso a 6 km tiene la sua rotta; uno Sturgeon nello stesso posto lo pedina
TEST_CASE("pace: l'SSBN non pedina il sottomarino che sente, un SSN si'" * doctest::skip(!fs::exists(M688_DATI "/interazioni.toml"))) {
    const auto pedina = [](const std::string& classe) {
        const auto f = fs::temp_directory_path() / ("m688_prova_pace_ssbn_" + classe + ".lua");
        std::ofstream(f) << "return { id = 'PACE_SSBN', teatro = 'faroe', lato = 'usa', data = '1989-06-15', ora_utc = 12, editor = {},"
                            " paesaggio_sonoro = false, carriera = { situazione = 'pace' }, script = {}, varianti = { { probabilita = 1, unita = {\n"
                            "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true, posizione = { x = -90000, y = -80000 },"
                            "  quota_m = 300, rotta = 90, velocita_kn = 3, " << SUB << " },\n"
                            "{ id = 2, classe = '" << classe << "', categoria = 'sottomarino', flag = 9, posizione = { x = 0, y = 0 }, quota_m = 150, rotta = 90, velocita_kn = 5, " << SUB << " },\n"
                            "{ id = 3, classe = 'victor3', categoria = 'sottomarino', flag = 17, posizione = { x = 0, y = 6000 }, quota_m = 60, rotta = 90, velocita_kn = 15, " << SUB << " },\n"
                            "}, timer = {} } } }\n";
        Partita p(f.string());
        p.avanza(900.0);
        const auto& r = p.m.reazioni_ia();
        fs::remove(f);
        return std::any_of(r.begin(), r.end(), [](const Missione::ReazioneIa& x) { return x.unita == 2 && x.reazione == "pedina"; });
    };
    CHECK_FALSE(pedina("ohio"));
    CHECK(pedina("sturgeon"));
}

// pedina solo chi ha il ruolo e il sensore (la principale, 30/10/2026: «Nimitz pedina Trafalgar» nella S01 e nella I08):
// in pace la Nimitz e la sua Spruance col Victor rumoroso a 6 km; la Spruance lo pedina, la portaerei no
TEST_CASE("pace: la portaerei non pedina, la sua scorta col sonar si'" * doctest::skip(!fs::exists(M688_DATI "/interazioni.toml"))) {
    const auto f = fs::temp_directory_path() / "m688_prova_pace_portaerei.lua";
    std::ofstream(f) << "return { id = 'PACE_CV', teatro = 'faroe', lato = 'usa', data = '1989-06-15', ora_utc = 12, editor = {},"
                        " paesaggio_sonoro = false, carriera = { situazione = 'pace' }, script = {}, varianti = { { probabilita = 1, unita = {\n"
                        "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true, posizione = { x = -90000, y = -80000 },"
                        "  quota_m = 300, rotta = 90, velocita_kn = 3, " << SUB << " },\n"
                        "{ id = 2, classe = 'nimitz', categoria = 'nave', flag = 10, posizione = { x = 0, y = 0 }, quota_m = 0, rotta = 90, velocita_kn = 5, " << NAVE << " },\n"
                        "{ id = 4, classe = 'spruance', categoria = 'nave', flag = 10, posizione = { x = 1500, y = 0 }, quota_m = 0, rotta = 90, velocita_kn = 5, " << NAVE << " },\n"
                        "{ id = 3, classe = 'victor3', categoria = 'sottomarino', flag = 17, posizione = { x = 0, y = 6000 }, quota_m = 60, rotta = 90, velocita_kn = 15, " << SUB << " },\n"
                        "}, timer = {} } } }\n";
    Partita p(f.string());
    p.avanza(900.0);
    const auto& r = p.m.reazioni_ia();
    const auto pedina = [&](int id) { return std::any_of(r.begin(), r.end(), [&](const Missione::ReazioneIa& x) { return x.unita == id && x.reazione == "pedina"; }); };
    CHECK_FALSE(pedina(2));   // la Nimitz
    CHECK(pedina(4));         // la Spruance
    fs::remove(f);
}

// --- il pedinamento ravvicinato (S24 «Quota zero», principale, 31/10/2026) -------------------------------------------

namespace {

// un Alfa dell'IA (2) a 150 m e 8 nodi verso est; uno Sturgeon dell'IA (3) 500 m a poppa a 160 m, che a 5 s comincia a
// pedinarlo da vicino a 400 m e 10 m sotto (sopra_m = -10, come il pedinatore americano del 1970). `script` e `timer` in piu' per l'Alfa
std::string scena_pedina(const std::string& nome, const std::string& script = "", const std::string& timer = "", double distanza_m = 400.0,
                         double virata_alfa = 1.0) {
    std::string alfa = SUB;
    alfa.replace(alfa.find("virata_gradi_s = 1"), 18, "virata_gradi_s = " + std::to_string(virata_alfa));
    return scena(nome, "pace",
                 "{ id = 2, classe = 'alfa', categoria = 'sottomarino', flag = 17, posizione = { x = 0, y = 0 }, quota_m = 150, rotta = 90, velocita_kn = 8, " + alfa + " },"
                 "{ id = 3, classe = 'sturgeon', categoria = 'sottomarino', flag = 9, posizione = { x = -500, y = 0 }, quota_m = 160, rotta = 90, velocita_kn = 8, " + SUB + " },",
                 "V = function(u) m688.pedina_vicino(u, 2, " + std::to_string(distanza_m) + ", -10) end" + script,
                 "{ dopo_s = 5, unita = 3, gestore = 'V' }" + timer);
}

struct PartitaSeme {
    std::map<std::string, ClasseNave> classi = carica_classi(M688_DATI "/classi.toml");
    Missione m;
    DanniMissione dm;
    PartitaSeme(const std::string& file, std::uint64_t seme) : m(file, seme, false), dm(classi, M688_DATI, seme, RegolaDanni::simulazione) {}
    void avanza(double s) { for (long i = 0; i < static_cast<long>(s * 9.0); ++i) { m.avanza(1.0 / 9.0); dm.avanza(m, 1.0 / 9.0); } }
};

}  // namespace

TEST_CASE("pedinamento ravvicinato: lo Sturgeon tiene il cono sordo dell'Alfa a poche centinaia di metri" * doctest::skip(!fs::exists(M688_DATI "/interazioni.toml"))) {
    const auto f = scena_pedina("pedina_vicino_cono");
    Partita p(f);
    p.avanza(300.0);
    REQUIRE(p.m.in_pedinamento_vicino(3, 2));
    const double cono = p.m.cono_sordo_gradi(2);
    int dentro = 0, prove = 0;
    double dmin = 1e9, dmax = 0.0;
    for (int k = 0; k < 120; ++k) {   // 20 minuti
        p.avanza(10.0);
        const auto& a = p.m.unita().at(2);
        const auto& s = p.m.unita().at(3);
        const double d = distanza(a.pos, s.pos);
        dmin = std::min(dmin, d);
        dmax = std::max(dmax, d);
        const double da_poppa = std::fabs(normalizza_180(rilevamento(a.pos, s.pos) - (a.rotta + 180.0)));
        dentro += da_poppa <= cono + 10.0;
        ++prove;
    }
    MESSAGE("cono sordo dell'Alfa ", cono, " gradi; distanza ", dmin, "-", dmax, " m; nel cono ", dentro, " volte su ", prove);
    CHECK(cono > 0.0);
    CHECK(dmin > 150.0);
    CHECK(dmax < 800.0);
    CHECK(dentro >= prove * 9 / 10);
    CHECK(std::any_of(p.m.reazioni_ia().begin(), p.m.reazioni_ia().end(), [](const Missione::ReazioneIa& r) { return r.unita == 3 && r.reazione == "pedina_vicino"; }));
    fs::remove(f);
}

TEST_CASE("pedinamento ravvicinato: lo Sturgeon segue il cambio di quota dell'Alfa col ritardo" * doctest::skip(!fs::exists(M688_DATI "/interazioni.toml"))) {
    // a 600 s l'Alfa sale da 150 a 100 m
    const auto f = scena_pedina("pedina_vicino_quota", ", Q = function(u) m688.quota(u, 100, 1) m688.salva_modello(u) end", ", { dopo_s = 600, unita = 2, gestore = 'Q' }");
    Partita p(f);
    p.avanza(605.0);
    const double prima = p.m.unita().at(3).quota_ordinata;
    p.avanza(10.0);   // a 615 s: il cambio non l'ha ancora visto (ritardo 20-60 s)
    const double subito = p.m.unita().at(3).quota_ordinata;
    p.avanza(300.0);
    const double dopo = p.m.unita().at(3).quota_ordinata;
    MESSAGE("quota ordinata dello Sturgeon: ", prima, " prima, ", subito, " a 615 s, ", dopo, " a 915 s (l'Alfa a ", p.m.unita().at(2).quota, ")");
    CHECK(std::fabs(subito - prima) < 5.0);
    CHECK(dopo < prima - 20.0);
    CHECK(std::fabs(dopo - 110.0) < 30.0);
    fs::remove(f);
}

TEST_CASE("pedinamento ravvicinato: un arresto brusco del pedinato porta a volte all'urto" * doctest::skip(!fs::exists(M688_DATI "/interazioni.toml"))) {
    // a 600 s l'Alfa si ferma di colpo e scende alla quota dello Sturgeon, che lo segue a 250 m e 8 nodi (il K-108 e il
    // Tautog): se ne accorge dopo 20-60 s, e in 60 s copre i 250 m; coi diversi semi a volte accosta in tempo, a volte no
    const auto f = scena_pedina("pedina_vicino_urto", ", I = function(u) m688.velocita_ora(u, 0) m688.quota(u, 160, 2) m688.salva_modello(u) end",
                                ", { dopo_s = 600, unita = 2, gestore = 'I' }", 250.0);
    int con_urto = 0, senza = 0;
    for (std::uint64_t seme = 1; seme <= 8; ++seme) {
        PartitaSeme p(f, seme);
        p.avanza(900.0);
        const bool urto = std::any_of(p.m.urti().begin(), p.m.urti().end(), [](const Missione::UrtoMissione& u) {
            return (u.urto.a.unita == 2 && u.urto.b.unita == 3) || (u.urto.a.unita == 3 && u.urto.b.unita == 2);
        });
        (urto ? con_urto : senza) += 1;
    }
    MESSAGE("arresto dell'Alfa con lo Sturgeon a 250 m: urto in ", con_urto, " partite su 8");
    CHECK(con_urto >= 1);
    CHECK(senza >= 1);
    fs::remove(f);
}

TEST_CASE("pedinamento ravvicinato: il cono sordo viene dai sensori, la schiera filata lo restringe" * doctest::skip(!fs::exists(M688_DATI "/interazioni.toml"))) {
    const auto f = scena("cono_sordo", "pace",
                         "{ id = 2, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, posizione = { x = 0, y = 0 }, quota_m = 150, rotta = 90, velocita_kn = 8, " + SUB + " },",
                         "S = function(u) m688.schiera_trainata(u, 1) m688.salva_modello(u) end", "{ dopo_s = 5, unita = 2, gestore = 'S' }");
    Partita p(f);
    p.avanza(2.0);
    const double senza = p.m.cono_sordo_gradi(2);
    p.avanza(600.0);
    const double con = p.m.cono_sordo_gradi(2);
    MESSAGE("cono sordo del 688 dell'IA: ", senza, " gradi senza schiera, ", con, " con la schiera filata");
    CHECK(senza > 0.0);
    CHECK(con < senza);
    fs::remove(f);
}

// la simmetria (l'utente, 31/10/2026: «Sì, stesso limite»): anche il sonar del giocatore, nell'Enhanced, non ripinga prima
// di 10 s, e il giocatore lo sa; nel classico come prima
TEST_CASE("difesa: anche il giocatore non pinga piu' spesso dell'eco, e riceve l'avviso" * doctest::skip(!fs::exists(M688_DATI "/interazioni.toml"))) {
    const auto f = scena("ping_giocatore", "pace", "");
    Partita p(f);
    p.avanza(1.0);
    REQUIRE(p.m.applica(Ordine{p.m.tempo(), TipoOrdine::impulso_sonar}));
    p.avanza(3.0);
    const std::size_t prima = p.m.messaggi_sonar().size();
    CHECK_FALSE(p.m.applica(Ordine{p.m.tempo(), TipoOrdine::impulso_sonar}));   // dopo 3 s: non parte
    REQUIRE(p.m.messaggi_sonar().size() > prima);
    CHECK(p.m.messaggi_sonar().back().codice == 414);
    p.avanza(8.0);
    CHECK(p.m.applica(Ordine{p.m.tempo(), TipoOrdine::impulso_sonar}));         // dopo 11 s: si'
    fs::remove(f);
}

// la minaccia credibile (disassemblatore, la principale, 31/10/2026: nella S22 i Victor evadevano per ogni ping lontano
// della Perry, 33-40 volte in 4 ore): il ping lontano non fa evadere, quello vicino al piu' una volta ogni 15 minuti
TEST_CASE("difesa: il ping lontano non fa evadere, quello vicino una volta per quarto d'ora" * doctest::skip(!fs::exists(M688_DATI "/interazioni.toml") || !fs::exists(M688_DATI "/asw_aereo.toml"))) {
    const auto f = scena("ping_evasione", "crisi",
        "{ id = 2, classe = 'perry', categoria = 'nave', flag = 10, posizione = { x = 0, y = 0 }, quota_m = 0, rotta = 90, velocita_kn = 0, " + NAVE + " },"
        "{ id = 3, classe = 'victor3', categoria = 'sottomarino', flag = 17, posizione = { x = 8000, y = 0 }, quota_m = 150, rotta = 0, velocita_kn = 5, " + SUB + " },"
        "{ id = 4, classe = 'victor3', categoria = 'sottomarino', flag = 17, posizione = { x = 0, y = 3000 }, quota_m = 150, rotta = 90, velocita_kn = 5, " + SUB + " },",
        "P = function(u) m688.impulso_sonar(u) m688.dopo(u, 10, 'P') end", "{ dopo_s = 5, unita = 2, gestore = 'P' }");
    Partita p(f);
    p.avanza(3600.0);
    const int lontano = p.m.manovre_evasione(3), vicino = p.m.manovre_evasione(4);
    MESSAGE("un'ora di ping della Perry ogni 10 s: Victor a 8 km ", lontano, " evasioni, a 3 km ", vicino);
    CHECK(lontano == 0);
    CHECK(vicino >= 1);
    CHECK(vicino <= 4);
    fs::remove(f);
}
