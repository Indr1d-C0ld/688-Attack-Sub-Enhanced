#include <doctest.h>

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "missioni/danni_missione.hpp"
#include "missioni/missione.hpp"
#include "missioni/registro_missione.hpp"
#include "nucleo/classi.hpp"
#include "nucleo/fondale.hpp"
#include "nucleo/intercettazione.hpp"
#include "nucleo/ordini.hpp"

using namespace m688;
namespace fs = std::filesystem;

// Il quadro del fondale del giocatore nell'Enhanced (docs/24 §3; missione_fondale.cpp): l'ecoscandaglio a comando e
// periodico, le due potenze, il BQS-15 sul fondo, gli avvisi dalle sole misure, il ping che il Victor intercetta;
// niente di tutto questo nel classico.

namespace {

const std::string SUB = "modello = { massima_kn = 32, virata_gradi_s = 1, accel_kn_s = 0.5, quota_m_s = 1, quota_max_m = 450, integrita = 200, rumore = 8, sonar = 19 }";

// il 688 a 100 m e 8 nodi su un fondale piatto; un Victor III a 5 km che ascolta
std::string scena() {
    const auto f = fs::temp_directory_path() / "m688_prova_fondale_missione.lua";
    std::ofstream(f) << "return { id = 'PROVA_FONDALE', teatro = 'faroe', lato = 'usa', data = '1989-06-15', ora_utc = 12, paesaggio_sonoro = false,"
                        " script = {}, varianti = { { probabilita = 1, unita = {\n"
                        "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true, posizione = { x = 0, y = 0 },"
                        "  quota_m = 100, rotta = 0, velocita_kn = 8, " << SUB << " },\n"
                        "{ id = 3, classe = 'victor3', categoria = 'sottomarino', flag = 17, posizione = { x = 5000, y = 0 },"
                        "  quota_m = 100, rotta = 180, velocita_kn = 4, " << SUB << " },\n"
                        "}, timer = {} } } }\n";
    return f.string();
}

struct Partita {
    std::map<std::string, ClasseNave> classi = carica_classi(M688_DATI "/classi.toml");
    Missione m;
    DanniMissione dm;
    Partita(const std::string& file, double fondale_m, bool enhanced = true)
        : m(file, 1, false), dm(classi, M688_DATI, 1, enhanced ? RegolaDanni::simulazione : RegolaDanni::classica) {
        m.imposta_fondale(std::make_shared<const Fondale>(200, 200, 1000.0, 1.0, -fondale_m, std::vector<std::uint16_t>(200 * 200, 0)));
    }
    void avanza(double s) { for (long i = 0; i < static_cast<long>(s * 9.0); ++i) { m.avanza(1.0 / 9.0); dm.avanza(m, 1.0 / 9.0); } }
    bool ordina(TipoOrdine t, double a = 0.0, int n = 0) { return m.applica({m.tempo(), t, a, 0.0, n}); }
    bool messaggio(int codice) const {
        return std::any_of(m.messaggi().begin(), m.messaggi().end(), [&](const Messaggio& x) { return x.codice == codice; });
    }
};

bool c_e() { return fs::exists(M688_DATI "/acustica.toml"); }

}  // namespace

TEST_CASE("fondale nella missione: il ping a comando misura il fondo, va nel quadro e il Victor lo intercetta" * doctest::skip(!c_e())) {
    const std::string f = scena();
    Partita p(f, 600.0);
    p.avanza(1.0);
    REQUIRE(p.m.ecoscandaglio(0).has_value());
    CHECK_FALSE(p.m.ecoscandaglio(0)->ultimo.has_value());   // niente ping finche' non lo si ordina
    REQUIRE(p.ordina(TipoOrdine::ecoscandaglio_ping));
    p.avanza(2.0);
    const auto e = p.m.ecoscandaglio(0);
    REQUIRE(e->ultimo.has_value());
    REQUIRE(e->ultimo->eco);
    MESSAGE("fondo misurato ", e->ultimo->fondo_m, " m (vero 600), sotto la chiglia ", e->ultimo->sotto_chiglia_m, " m");
    if (!e->ultimo->falso) CHECK(e->ultimo->fondo_m == doctest::Approx(600.0).epsilon(0.05));
    const QuadroFondale* q = p.m.quadro_fondale(0);
    REQUIRE(q);
    CHECK(q->sondaggi().size() == 1);
    CHECK_FALSE(q->celle_misurate().empty());
    CHECK(p.m.quadro_fondale(3) != q);   // l'IA ha il suo quadro (missione_fondale_ia.cpp), non quello del giocatore
    // la contro-scoperta: l'intercettatore del Victor sente il ping
    const RicevitoreIntercettazione* v = p.m.intercettatore(3);
    REQUIRE(v);
    CHECK_FALSE(v->tracce().empty());
    fs::remove(f);
}

TEST_CASE("fondale nella missione: periodico ogni minuto, la potenza bassa, e il nastro lungo la rotta" * doctest::skip(!c_e())) {
    const std::string f = scena();
    Partita p(f, 600.0);
    p.avanza(1.0);
    REQUIRE(p.ordina(TipoOrdine::ecoscandaglio_periodo, 1.0));
    REQUIRE(p.ordina(TipoOrdine::ecoscandaglio_potenza, 0.0, 1));
    p.avanza(600.0);
    const auto e = p.m.ecoscandaglio(0);
    CHECK(e->periodo_min == doctest::Approx(1.0));
    CHECK(e->potenza == PotenzaEco::bassa);
    const QuadroFondale* q = p.m.quadro_fondale(0);
    REQUIRE(q);
    MESSAGE(q->sondaggi().size(), " sondaggi in 10 minuti, ", q->celle_misurate().size(), " celle misurate");
    CHECK(q->sondaggi().size() >= 9);
    CHECK(q->sondaggi().size() <= 11);
    CHECK(std::all_of(q->sondaggi().begin(), q->sondaggi().end(), [](const Sondaggio& s) { return s.potenza == PotenzaEco::bassa; }));
    CHECK(q->celle_misurate().size() >= 3);   // 8 nodi per 10 minuti: 2,5 km di nastro, sulle celle da 1 km della prova
    // spento: niente piu' ping
    REQUIRE(p.ordina(TipoOrdine::ecoscandaglio_periodo, 0.0));
    const std::size_t prima = q->sondaggi().size();
    p.avanza(180.0);
    CHECK(q->sondaggi().size() == prima);
    fs::remove(f);
}

TEST_CASE("fondale nella missione: gli avvisi vengono dalle misure, non dal fondo vero" * doctest::skip(!c_e())) {
    const std::string f = scena();
    Partita p(f, 120.0);   // 20 m sotto la chiglia del 688
    p.avanza(60.0);
    CHECK_FALSE(p.messaggio(480));
    CHECK_FALSE(p.messaggio(481));   // senza un ping l'equipaggio non lo sa
    REQUIRE(p.ordina(TipoOrdine::ecoscandaglio_ping));
    p.avanza(5.0);
    CHECK(p.messaggio(481));
    CHECK(std::count_if(p.m.messaggi().begin(), p.m.messaggi().end(), [](const Messaggio& x) { return x.codice == 481; }) == 1);
    fs::remove(f);
}

TEST_CASE("fondale nella missione: il BQS-15 acceso mette nel quadro i ritorni del fondo" * doctest::skip(!c_e())) {
    const std::string f = scena();
    Partita p(f, 200.0);
    p.avanza(1.0);
    REQUIRE(p.ordina(TipoOrdine::bqs15, 0.0, 1));
    p.avanza(30.0);
    const QuadroFondale* q = p.m.quadro_fondale(0);
    REQUIRE(q);
    MESSAGE(q->ritorni().size(), " ritorni del BQS-15 in 30 s");
    CHECK_FALSE(q->ritorni().empty());
    CHECK(q->sondaggi().empty());
    fs::remove(f);
}

TEST_CASE("fondale nella missione: nel classico non c'e' l'ecoscandaglio" * doctest::skip(!c_e())) {
    const std::string f = scena();
    Partita p(f, 600.0, false);
    p.avanza(1.0);
    CHECK_FALSE(p.m.ecoscandaglio(0).has_value());
    CHECK_FALSE(p.ordina(TipoOrdine::ecoscandaglio_ping));
    p.avanza(60.0);
    CHECK(p.m.quadro_fondale(0) == nullptr);
    fs::remove(f);
}

// docs/26: i secondi con meno di 20 m d'acqua sotto la chiglia, il mare massimo e i ping dell'ecoscandaglio nel registro
TEST_CASE("fondale nella missione: i conti di docs/26 (sotto la chiglia, il mare, i ping nel registro)" * doctest::skip(!c_e())) {
    const std::string f = scena();
    Partita p(f, 112.0);   // 12 m sotto la chiglia del 688 a 100 m
    p.avanza(1.0);
    REQUIRE(p.ordina(TipoOrdine::ecoscandaglio_ping));
    p.avanza(60.0);
    const auto& c = p.m.conti_giocatore();
    MESSAGE("sotto la chiglia per ", c.sotto_chiglia_20_s, " s, mare massimo ", c.mare_max);
    CHECK(c.sotto_chiglia_20_s == doctest::Approx(61.0).epsilon(0.05));
    CHECK(c.mare_max >= 0);
    const auto& ev = p.m.eventi_missione();
    CHECK(std::count_if(ev.begin(), ev.end(), [](const EventoMissione& e) { return e.tipo == TipoEvento::ping_ecoscandaglio; }) == 1);
    fs::remove(f);
    // a 600 m di fondo, niente
    const std::string g = scena();
    Partita q(g, 600.0);
    q.avanza(60.0);
    CHECK(q.m.conti_giocatore().sotto_chiglia_20_s == 0.0);
    fs::remove(g);
}
