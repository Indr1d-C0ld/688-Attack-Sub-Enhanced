#include <doctest.h>

#include <filesystem>
#include <fstream>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "missioni/danni_missione.hpp"
#include "missioni/missione.hpp"
#include "nucleo/classi.hpp"
#include "nucleo/fondale.hpp"
#include "nucleo/ordini.hpp"

using namespace m688;
namespace fs = std::filesystem;

// La schiera trainata dei sottomarini nella missione dell'Enhanced (docs/20 §7, D; missione_schiera.cpp): la quota
// vera sotto il battello a bassa velocita', il fondo che la tocca in acqua bassa, i danni e la perdita, la persa che non
// si fila piu'; niente di tutto questo senza l'acustica.

namespace {

const std::string SUB = "modello = { massima_kn = 32, virata_gradi_s = 1, accel_kn_s = 0.5, quota_m_s = 1, quota_max_m = 450, integrita = 200, rumore = 8, sonar = 19, schiera_trainata = true }";

// il 688 a 60 m e 3 nodi con la schiera filata dall'inizio, su un fondale piatto
std::string scena() {
    const auto f = fs::temp_directory_path() / "m688_prova_schiera_missione.lua";
    std::ofstream(f) << "return { id = 'PROVA_SCHIERA', teatro = 'faroe', lato = 'usa', data = '1989-01-15', ora_utc = 12, paesaggio_sonoro = false,"
                        " script = {}, varianti = { { probabilita = 1, unita = {\n"
                        "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true, posizione = { x = 0, y = 0 },"
                        "  quota_m = 60, rotta = 0, velocita_kn = 3, " << SUB << " },\n"
                        "{ id = 3, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, posizione = { x = 40000, y = 0 },"
                        "  quota_m = 60, rotta = 0, velocita_kn = 3, " << SUB << " },\n"
                        "}, timer = {} } } }\n";
    return f.string();
}

struct Partita {
    std::map<std::string, ClasseNave> classi = carica_classi(M688_DATI "/classi.toml");
    Missione m;
    DanniMissione dm;
    Partita(const std::string& file, double fondale_m, std::uint64_t seme = 1, bool enhanced = true)
        : m(file, seme, false), dm(classi, M688_DATI, seme, enhanced ? RegolaDanni::simulazione : RegolaDanni::classica) {
        // 200 x 200 km piatti
        m.imposta_fondale(std::make_shared<const Fondale>(200, 200, 1000.0, 1.0, -fondale_m, std::vector<std::uint16_t>(200 * 200, 0)));
    }
    void avanza(double s) { for (long i = 0; i < static_cast<long>(s * 9.0); ++i) { m.avanza(1.0 / 9.0); dm.avanza(m, 1.0 / 9.0); } }
};

bool c_e() { return fs::exists(M688_DATI "/acustica.toml"); }

}  // namespace

TEST_CASE("schiera nella missione: in acqua profonda a 3 nodi la schiera scende sotto il battello e non tocca" * doctest::skip(!c_e())) {
    const std::string f = scena();
    Partita p(f, 3000.0);
    p.avanza(600.0);
    const auto s = p.m.stato_schiera(0);
    REQUIRE(s.has_value());
    MESSAGE("a 3 nodi: cavo ", s->cavo_filato_m, " m, schiera a ", s->quota_centro_m, " m, ", s->dietro_m, " m dietro");
    CHECK(s->cavo_filato_m == doctest::Approx(p.m.catalogo_acustico()->schiera("tb16")->cavo_max_m));   // filata dall'inizio
    CHECK(s->quota_centro_m > p.m.giocatore()->quota + 50.0);   // sotto lo strato, dove il battello non sente
    CHECK_FALSE(s->fondo);
    CHECK_FALSE(s->danneggiata);
    CHECK_FALSE(s->persa);
    fs::remove(f);
}

TEST_CASE("schiera nella missione: in 200 m d'acqua tocca il fondo, si danneggia o si perde, e la persa non si fila piu'" * doctest::skip(!c_e())) {
    const std::string f = scena();
    Partita p(f, 200.0);
    bool fondo = false;
    for (int k = 0; k < 60 && !p.m.schiera_persa(0); ++k) {
        p.avanza(60.0);
        if (const auto s = p.m.stato_schiera(0)) fondo = fondo || s->fondo;
    }
    CHECK(fondo);
    // l'IA con la stessa regola (simmetria), ma il suo comandante accorcia il cavo in acqua bassa (cavo_sicuro_m): la
    // schiera dell'SSN dell'IA non tocca e resta sana
    const auto ia = p.m.stato_schiera(3);
    REQUIRE(ia.has_value());
    MESSAGE("SSN dell'IA in 200 m: cavo ", ia->cavo_filato_m, " m, schiera a ", ia->quota_centro_m, " m");
    CHECK(ia->cavo_filato_m < p.m.catalogo_acustico()->schiera("tb16")->cavo_max_m);
    CHECK_FALSE(ia->fondo);
    CHECK_FALSE(ia->persa);
    const auto s = p.m.stato_schiera(0);
    REQUIRE(s.has_value());
    CHECK((s->danneggiata || s->persa));
    if (s->persa) {
        CHECK((p.m.giocatore()->flag54 & 1) == 0);
        CHECK_FALSE(p.m.applica({p.m.tempo(), TipoOrdine::schiera_cala, 0, 0, 0}));
    }
    MESSAGE("dopo ", p.m.tempo() / 60.0, " minuti: ", std::string(s->persa ? "persa" : "danneggiata"));
    fs::remove(f);
}

TEST_CASE("schiera nella missione: senza l'acustica (il classico) niente schiera del modello" * doctest::skip(!c_e())) {
    const std::string f = scena();
    Partita p(f, 200.0, 1, false);
    p.avanza(300.0);
    CHECK_FALSE(p.m.stato_schiera(0).has_value());
    fs::remove(f);
}

TEST_CASE("schiera nella missione: lo stesso seme, la stessa storia" * doctest::skip(!c_e())) {
    const std::string f = scena();
    const auto gioca = [&] {
        Partita p(f, 200.0, 5);
        p.avanza(1800.0);
        const auto s = p.m.stato_schiera(0);
        return std::vector<double>{s ? s->quota_centro_m : -1.0, s && s->danneggiata ? 1.0 : 0.0, s && s->persa ? 1.0 : 0.0};
    };
    CHECK(gioca() == gioca());
    fs::remove(f);
}
