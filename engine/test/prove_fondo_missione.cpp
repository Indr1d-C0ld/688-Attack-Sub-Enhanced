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
#include "nucleo/battello.hpp"
#include "nucleo/classi.hpp"
#include "nucleo/fondale.hpp"
#include "nucleo/ordini.hpp"

using namespace m688;
namespace fs = std::filesystem;

// Posarsi sul fondo nell'Enhanced (roadmap 30, docs/11 §6; missione_fondo.cpp): il fango del Baltico che trattiene e
// intasa le prese a mare, le rocce della piattaforma delle Faroe che danneggiano timone e cupola strisciando; l'IA con
// la stessa regola; niente di tutto questo senza l'acustica.

namespace {

const std::string SUB = "modello = { massima_kn = 32, virata_gradi_s = 1, accel_kn_s = 0.5, quota_m_s = 1, quota_max_m = 450, integrita = 200, rumore = 8, sonar = 19 }";

// il 688 e un altro sottomarino posati su un fondale piatto (la quota iniziale sotto il fondo: la chiglia vi si appoggia)
std::string scena(const std::string& teatro, double quota_m, double kn) {
    const auto f = fs::temp_directory_path() / ("m688_prova_fondo_" + teatro + ".lua");
    std::ofstream(f) << "return { id = 'PROVA_FONDO', teatro = '" << teatro << "', lato = 'usa', data = '1989-06-15', ora_utc = 12, paesaggio_sonoro = false,"
                        " script = {}, varianti = { { probabilita = 1, unita = {\n"
                        "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true, posizione = { x = 0, y = 0 },"
                        "  quota_m = " << quota_m << ", rotta = 0, velocita_kn = " << kn << ", " << SUB << " },\n"
                        "{ id = 3, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, posizione = { x = 40000, y = 0 },"
                        "  quota_m = " << quota_m << ", rotta = 0, velocita_kn = " << kn << ", " << SUB << " },\n"
                        "}, timer = {} } } }\n";
    return f.string();
}

struct Partita {
    std::map<std::string, ClasseNave> classi = carica_classi(M688_DATI "/classi.toml");
    Missione m;
    DanniMissione dm;
    double profondita_fondo = 0.0;
    // fermo: a battello appoggiato (la dinamica non lo fa scendere da fermo, con l'assetto neutro: ci arriva in moto)
    void ferma() {
        auto* u = m.trova(0);
        u->velocita = u->velocita_ordinata = 0.0;
    }
    // la quota ordinata del giocatore direttamente (le scene di prova non hanno i gestori dell'originale)
    void quota(double q) {
        auto* u = m.trova(0);
        u->quota_ordinata = q;
        u->rateo_quota = u->rateo_quota_modello;
    }
    Partita(const std::string& file, double fondale_m, std::uint64_t seme = 1, bool enhanced = true)
        : m(file, seme, false), dm(classi, M688_DATI, seme, enhanced ? RegolaDanni::simulazione : RegolaDanni::classica), profondita_fondo(fondale_m) {
        m.imposta_fondale(std::make_shared<const Fondale>(200, 200, 1000.0, 1.0, -fondale_m, std::vector<std::uint16_t>(200 * 200, 0)));
        quota(fondale_m + 10.0);   // il giocatore si posa (l'unita' parte 25 piedi sopra il fondo)
    }
    // ia_sul_fondo: il comandante dell'IA tiene 25 piedi sotto la chiglia; la prova la spinge sul fondo a ogni passo
    // (come il battello danneggiato che affonda) per vedere la stessa regola del giocatore
    bool ia_sul_fondo = false;
    void avanza(double s) {
        for (long i = 0; i < static_cast<long>(s * 9.0); ++i) {
            if (ia_sul_fondo) if (auto* u = m.trova(3)) u->quota = profondita_fondo + 5.0;
            m.avanza(1.0 / 9.0);
            dm.avanza(m, 1.0 / 9.0);
        }
    }
    bool messaggio(int codice) const {
        return std::any_of(m.messaggi().begin(), m.messaggi().end(), [&](const Messaggio& x) { return x.codice == codice; });
    }
    bool guasto(int unita, Sistema s) const {
        return std::any_of(m.guasti_richiesti().begin(), m.guasti_richiesti().end(),
                           [&](const GuastoRichiesto& g) { return g.unita == unita && g.sistema == static_cast<int>(s); });
    }
};

bool c_e() { return fs::exists(M688_DATI "/acustica.toml"); }

}  // namespace

TEST_CASE("fondo nella missione: il fango del Baltico prende col tempo e trattiene il battello che vuole salire" * doctest::skip(!c_e())) {
    const std::string f = scena("baltico", 110.0, 3.0);
    Partita p(f, 100.0);
    p.avanza(900.0);
    REQUIRE(p.m.giocatore()->sul_fondo);
    const auto s = p.m.stato_fondo(0);
    REQUIRE(s.has_value());
    CHECK(s->tipo == TipoFondo::fango);
    CHECK(s->presa == doctest::Approx(1.0));
    p.quota(50.0);
    p.avanza(60.0);
    CHECK(p.messaggio(476));
    CHECK(p.m.giocatore()->quota > 99.0);   // trattenuto: la chiglia resta sul fondo
    CHECK(p.m.stato_fondo(0)->trattenuto_s > 0.0);
    p.avanza(240.0);   // presa piena: 180 s, poi si libera
    MESSAGE("dopo 5 minuti dall'ordine: quota ", p.m.giocatore()->quota, " m");
    CHECK(p.m.giocatore()->quota < 95.0);
    CHECK_FALSE(p.m.giocatore()->sul_fondo);
    fs::remove(f);
}

TEST_CASE("fondo nella missione: posato a lungo sul fango, le prese a mare si intasano (giocatore e IA)" * doctest::skip(!c_e())) {
    const std::string f = scena("baltico", 110.0, 3.0);
    Partita p(f, 100.0);
    p.ia_sul_fondo = true;
    p.avanza(1500.0);
    CHECK_FALSE(p.m.stato_fondo(0)->prese_intasate);   // prima di mezz'ora no
    p.avanza(4500.0);
    CHECK(p.m.stato_fondo(0)->prese_intasate);
    CHECK(p.guasto(0, Sistema::propulsione));
    CHECK(p.messaggio(478));
    CHECK(p.guasto(3, Sistema::propulsione));   // l'IA con la stessa regola (simmetria)
    fs::remove(f);
}

TEST_CASE("fondo nella missione: strisciare sulle rocce delle Faroe rompe timone o cupola e graffia lo scafo, anche all'IA" * doctest::skip(!c_e())) {
    const std::string f = scena("faroe", 160.0, 4.0);
    Partita p(f, 150.0);   // sulla piattaforma: roccia
    p.ia_sul_fondo = true;
    const int integrita = p.m.giocatore()->integrita;
    p.avanza(1800.0);
    const auto s = p.m.stato_fondo(0);
    REQUIRE(s.has_value());
    CHECK(s->tipo == TipoFondo::roccia);
    CHECK((p.guasto(0, Sistema::timone) || p.guasto(0, Sistema::sonar_prua)));
    CHECK(p.messaggio(477));
    CHECK(p.m.giocatore()->integrita < integrita);
    CHECK_FALSE(s->presa > 0.0);   // la roccia non trattiene
    CHECK((p.guasto(3, Sistema::timone) || p.guasto(3, Sistema::sonar_prua)));
    fs::remove(f);
}

TEST_CASE("fondo nella missione: fermo sulle rocce non si danneggia" * doctest::skip(!c_e())) {
    const std::string f = scena("faroe", 160.0, 3.0);
    Partita p(f, 150.0);
    p.avanza(120.0);
    REQUIRE(p.m.giocatore()->sul_fondo);
    p.ferma();
    p.avanza(120.0);
    const std::size_t guasti = p.m.guasti_richiesti().size();
    p.avanza(1800.0);
    CHECK(p.m.giocatore()->sul_fondo);
    CHECK(std::fabs(p.m.giocatore()->velocita) < 0.1);
    CHECK(p.m.guasti_richiesti().size() == guasti);
    fs::remove(f);
}

TEST_CASE("fondo nella missione: senza l'acustica (classico) il fondo non trattiene e non rompe niente" * doctest::skip(!c_e())) {
    const std::string f = scena("baltico", 110.0, 3.0);
    Partita p(f, 100.0, 1, false);
    p.avanza(4000.0);
    CHECK(p.m.giocatore()->sul_fondo);
    CHECK_FALSE(p.m.stato_fondo(0).has_value());
    CHECK_FALSE(p.messaggio(478));
    fs::remove(f);
}
