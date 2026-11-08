#include <doctest.h>

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <map>
#include <string>

#include "missioni/danni_missione.hpp"
#include "missioni/missione.hpp"
#include "missioni/registro_missione.hpp"
#include "nucleo/classi.hpp"
#include "nucleo/geometria.hpp"

using namespace m688;
namespace fs = std::filesystem;

// I transitori dello scafo (missione_scafo.cpp; nota disassemblatore-transitori-scafo.md) e i posti di combattimento
// del battello del giocatore.

namespace {

const std::string SUB = "modello = { massima_kn = 32, virata_gradi_s = 1, accel_kn_s = 0.5, quota_m_s = 3, quota_max_m = 450, integrita = 200, rumore = 8, sonar = 19 }";
const std::string NAVE = "modello = { massima_kn = 32, virata_gradi_s = 2, accel_kn_s = 0.5, quota_m_s = 1, quota_max_m = 0, integrita = 100, rumore = 10, sonar = 20 }";

struct Partita {
    std::map<std::string, ClasseNave> classi = carica_classi(M688_DATI "/classi.toml");
    Missione m;
    DanniMissione dm;
    explicit Partita(const std::string& file) : m(file, 1, false), dm(classi, M688_DATI, 1, RegolaDanni::simulazione) {}
    void avanza(double s) {
        for (long i = 0; i < static_cast<long>(s * 9.0 + 0.5); ++i) {
            m.avanza(1.0 / 9.0);
            dm.avanza(m, 1.0 / 9.0);
        }
    }
};

std::string scena(const std::string& nome, const std::string& altre, const std::string& script = "", const std::string& timer = "") {
    const auto f = fs::temp_directory_path() / ("m688_prova_scafo_" + nome + ".lua");
    std::ofstream(f) << "return { id = 'SCAFO_" << nome << "', teatro = 'faroe', lato = 'usa', data = '1989-02-15', ora_utc = 12,"
                        " paesaggio_sonoro = false, script = {" << script << "}, varianti = { { probabilita = 1, unita = {\n" << altre
                     << "\n}, timer = {" << timer << "} } } }\n";
    return f.string();
}

}  // namespace

TEST_CASE("scafo: il Kilo che scende oltre i 100 m scricchiola, e la Perry a 2 km lo sente") {
    // un Kilo quasi fermo a 2 nodi, da 60 a 420 m, e una Perry ferma a 2 km: dello scafo che scricchiola le arriva il
    // transitorio (128 dB a impulsi fra 1 e 5 kHz: a 6 km, sotto lo strato, non piu')
    const auto f = scena("scricchiolio",
        "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true, posizione = { x = -90000, y = -80000 },"
        "  quota_m = 300, rotta = 90, velocita_kn = 3, " + SUB + " },\n"
        "{ id = 2, classe = 'perry', categoria = 'nave', flag = 10, posizione = { x = 0, y = 2000 }, quota_m = 0, rotta = 90, velocita_kn = 0, " + NAVE + " },\n"
        "{ id = 3, classe = 'kilo', categoria = 'sottomarino', flag = 17, posizione = { x = 0, y = 0 }, quota_m = 60, rotta = 0, velocita_kn = 2, " + SUB + " },",
        "Q = function(u) m688.quota(u, 420, 1) m688.salva_modello(u) end", "{ dopo_s = 5, unita = 3, gestore = 'Q' }");
    Partita p(f);
    p.avanza(1200.0);
    const auto& nati = p.m.scafo_nati();
    const int scricchiolii = nati.count("scafo_scricchiola") ? nati.at("scafo_scricchiola") : 0;
    const bool sentito = std::any_of(p.m.scafo_sentiti().begin(), p.m.scafo_sentiti().end(), [](const Missione::ScafoSentito& x) {
        return x.ascoltatore == 2 && x.origine == 3 && x.tipo == "scafo_scricchiola";
    });
    MESSAGE("Kilo a ", p.m.unita().at(3).quota, " m, scricchiolii ", scricchiolii, ", sentiti ", p.m.scafo_sentiti().size());
    CHECK(p.m.unita().at(3).quota > 200.0);
    CHECK(scricchiolii >= 2);
    CHECK(sentito);
    CHECK(p.m.unita().at(2).contatti.count(3) == 1);
    fs::remove(f);
}

TEST_CASE("scafo: la soffiata d'emergenza del giocatore si sente lontano") {
    const auto f = scena("soffiata",
        "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true, posizione = { x = 0, y = 0 },"
        "  quota_m = 150, rotta = 90, velocita_kn = 3, " + SUB + " },\n"
        "{ id = 3, classe = 'victor3', categoria = 'sottomarino', flag = 17, sonarista = 'esperto', posizione = { x = 0, y = 25000 }, quota_m = 100, rotta = 0,"
        " velocita_kn = 3, " + SUB + " },");
    Partita p(f);
    p.avanza(3.0);
    p.m.applica(Ordine{p.m.tempo(), TipoOrdine::emersione_emergenza});   // la soffiata parte anche senza i gestori della manovra
    p.avanza(10.0);
    CHECK(p.m.scafo_nati().count("soffiata_emergenza") == 1);
    CHECK(std::any_of(p.m.scafo_sentiti().begin(), p.m.scafo_sentiti().end(), [](const Missione::ScafoSentito& x) {
        return x.ascoltatore == 3 && x.origine == 0 && x.tipo == "soffiata_emergenza";
    }));   // 175 dB a 25 km
    CHECK(p.m.unita().at(3).contatti.count(0) == 1);
    // il sonarista del Victor ne fa un sottomarino (nucleo/sonarista, transitorio_scafo)
    p.avanza(60.0);
    const Sonarista* so = p.m.sonarista_ia(3);
    REQUIRE(so != nullptr);
    const TracciaSonar* t = so->traccia_di(0);
    REQUIRE(t != nullptr);
    MESSAGE("la traccia del Victor sul 688: confidenza ", static_cast<int>(t->confidenza));
    CHECK(static_cast<int>(t->confidenza) >= static_cast<int>(Confidenza::possub));
    fs::remove(f);
}

TEST_CASE("scafo: i posti di combattimento del giocatore con un ostile vicino, poi la condizione normale") {
    const auto f = scena("posti",
        "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true, posizione = { x = 0, y = 0 },"
        "  quota_m = 150, rotta = 90, velocita_kn = 3, " + SUB + " },\n"
        "{ id = 2, classe = 'krivak2', categoria = 'nave', flag = 18, posizione = { x = 0, y = 5000 }, quota_m = 0, rotta = 90, velocita_kn = 5, " + NAVE + " },");
    Partita p(f);
    p.avanza(2.0);
    CHECK_FALSE(p.m.posti_di_combattimento());
    auto& c = p.m.trova(0)->contatti[2];   // classificata (categoria) dal giocatore
    c.durata = 900.0;
    c.qualita = 1000.0;
    p.avanza(2.0);
    CHECK(p.m.posti_di_combattimento());
    p.m.trova(2)->pos = {0.0, 60000.0};    // se ne va
    p.avanza(20.0 * 60.0);
    CHECK_FALSE(p.m.posti_di_combattimento());
    const auto& ev = p.m.eventi_missione();
    const auto n = [&](int livello) {
        return std::count_if(ev.begin(), ev.end(), [&](const EventoMissione& e) { return e.tipo == TipoEvento::posti_di_combattimento && e.livello == livello; });
    };
    CHECK(n(1) == 1);
    CHECK(n(0) == 1);
    for (const auto& e : ev)
        if (e.tipo == TipoEvento::posti_di_combattimento && e.livello == 1) CHECK(e.testo_it.find("Posti di combattimento") != std::string::npos);
    fs::remove(f);
}
