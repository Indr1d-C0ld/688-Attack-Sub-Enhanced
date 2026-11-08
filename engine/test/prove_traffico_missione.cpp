#include <doctest.h>

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <cstdlib>
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

using namespace m688;
namespace fs = std::filesystem;

// Il traffico civile nella missione dell'Enhanced (docs/20 §3; missione_traffico.cpp): spento di norma, acceso solo
// con l'acustica; unita' vere, neutrali e non combattenti, entro il disco e sotto il tetto, mai osservatori e mai
// bersagli degli script; la chiave `traffico = false`; ripetibile dal seme.

namespace {

bool c_e() {
    return fs::exists(M688_DATI "/teatri/baltico.traffico.toml") && fs::exists(std::string(M688_MISSIONI) + "/SCENE0A.lua");
}

struct Partita {
    std::map<std::string, ClasseNave> classi = carica_classi(M688_DATI "/classi.toml");
    Missione m;
    DanniMissione dm;
    Partita(const std::string& file, std::uint64_t seme, bool traffico) : m(file, seme), dm(classi, M688_DATI, seme, RegolaDanni::simulazione) {
        std::map<std::string, double> pescaggi;
        for (const auto& [k, c] : classi) pescaggi[k] = c.pescaggio_m;
        if (const std::string f = std::string(M688_DATI) + "/teatri/" + m.teatro() + ".fdl"; fs::exists(f))
            m.imposta_fondale(std::make_shared<const Fondale>(Fondale::carica(f)), pescaggi);
        m.senza_giocatore(true);
        m.usa_traffico(traffico);
    }
    void avanza(double s) {
        for (long i = 0; i < static_cast<long>(s * 9.0); ++i) {
            m.avanza(1.0 / 9.0);
            dm.avanza(m, 1.0 / 9.0);
            if (m.esito()) break;
        }
    }
    std::vector<int> navi() const {
        std::vector<int> v;
        for (const auto& [id, u] : m.unita()) if (m.del_traffico(id)) v.push_back(id);
        return v;
    }
};

const std::string ZEROA = std::string(M688_MISSIONI) + "/SCENE0A.lua";

}  // namespace

TEST_CASE("traffico nella missione: spento di norma, acceso con le navi vere sotto il tetto" * doctest::skip(!c_e())) {
    {
        Partita p(ZEROA, 1, false);
        p.avanza(30.0);
        CHECK_FALSE(p.m.con_traffico());
        CHECK(p.navi().empty());
    }
    Partita p(ZEROA, 1, true);
    p.avanza(30.0);
    REQUIRE(p.m.con_traffico());
    const auto navi = p.navi();
    MESSAGE("SCENE0A (Baltico): ", navi.size(), " navi del traffico");
    CHECK(!navi.empty());
    CHECK(navi.size() <= 40);
    const Vec2 g = p.m.giocatore()->pos;
    for (const int id : navi) {
        const UnitaClassica& u = p.m.unita().at(id);
        CHECK(id >= 10000);
        CHECK(u.categoria == "nave");
        CHECK((u.flag & 0x18) == 0);    // neutrale
        CHECK((u.flag & 0x40) != 0);    // non combattente: mai osservatore
        CHECK(distanza(u.pos, g) <= 110000.0 + 1.0);
        CHECK(p.m.categoria_roe(u) == CategoriaRoe::mercantili);
    }
    // solo sorgenti: dopo dieci minuti nessuna nave del traffico ha contatti, e nessuna unita' dell'IA ne ha preso una
    // per bersaglio dagli script
    p.avanza(600.0);
    int sentite = 0;
    for (const auto& [id, u] : p.m.unita()) {
        if (p.m.del_traffico(id)) {
            for (const auto& [cid, c] : u.contatti) MESSAGE("contatto della nave ", id, " su ", cid, " dal sensore '", c.sensore, "'");
            CHECK(u.contatti.empty());
            continue;
        }
        if (!u.giocatore) CHECK_FALSE(p.m.del_traffico(u.bersaglio));
        else for (const auto& [cid, c] : u.contatti) sentite += p.m.del_traffico(cid);
    }
    double vicina = 1e18;
    for (const int id : p.navi()) vicina = std::min(vicina, distanza(p.m.unita().at(id).pos, p.m.giocatore()->pos));
    MESSAGE("il 688 ha ", sentite, " contatti sul traffico dopo 10 minuti; la nave piu' vicina a ", vicina / 1000.0, " km");
}

TEST_CASE("traffico nella missione: la chiave traffico = false lo spegne" * doctest::skip(!c_e())) {
    std::ifstream in(ZEROA, std::ios::binary);
    std::string testo{std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>()};
    const std::string chiave = "  id = \"SCENE0A\",\n";
    const auto k = testo.find(chiave);
    REQUIRE(k != std::string::npos);
    testo.insert(k + chiave.size(), "  traffico = false,\n");
    const auto f = fs::temp_directory_path() / "m688_prova_traffico_spento_SCENE0A.lua";
    std::ofstream(f, std::ios::binary) << testo;
    Partita p(f.string(), 1, true);
    p.avanza(30.0);
    CHECK_FALSE(p.m.con_traffico());
    CHECK(p.navi().empty());
    fs::remove(f);
}

TEST_CASE("traffico nella missione: lo stesso seme, le stesse navi" * doctest::skip(!c_e())) {
    const auto gioca = [] {
        Partita p(ZEROA, 3, true);
        p.avanza(120.0);
        std::vector<std::pair<int, Vec2>> v;
        for (const int id : p.navi()) v.emplace_back(id, p.m.unita().at(id).pos);
        return v;
    };
    const auto a = gioca(), b = gioca();
    REQUIRE(a.size() == b.size());
    for (std::size_t i = 0; i < a.size(); ++i) {
        CHECK(a[i].first == b[i].first);
        CHECK(a[i].second.x == b[i].second.x);
        CHECK(a[i].second.y == b[i].second.y);
    }
}

// Il costo (docs/20 §3, la richiesta della principale): DIAG_COSTO_TRAFFICO=<missione> gioca DIAG_SEMI semi (10) da
// DIAG_S secondi (7200) col giocatore fermo, senza e con il traffico, fino in fondo anche dopo l'esito, e stampa i
// tempi. Il traffico non deve raddoppiare il tempo
TEST_CASE("DIAG costo traffico" * doctest::skip(!std::getenv("DIAG_COSTO_TRAFFICO"))) {
    const std::string id = std::getenv("DIAG_COSTO_TRAFFICO");
    const int semi = std::getenv("DIAG_SEMI") ? std::atoi(std::getenv("DIAG_SEMI")) : 10;
    const double durata = std::getenv("DIAG_S") ? std::atof(std::getenv("DIAG_S")) : 7200.0;
    double tot[2] = {0.0, 0.0};
    for (int s = 1; s <= semi; ++s)
        for (int con = 0; con < 2; ++con) {
            Partita p(std::string(M688_MISSIONI) + "/" + id + ".lua", static_cast<std::uint64_t>(s), con == 1);
            const auto t0 = std::chrono::steady_clock::now();
            std::size_t massimo = 0;
            for (long i = 0; i < static_cast<long>(durata * 9.0); ++i) {
                p.m.avanza(1.0 / 9.0);
                p.dm.avanza(p.m, 1.0 / 9.0);
                if (i % 900 == 0) massimo = std::max(massimo, p.navi().size());
            }
            const double sec = std::chrono::duration<double>(std::chrono::steady_clock::now() - t0).count();
            tot[con] += sec;
            std::printf("%s seme %d %s: %.1f s, navi al piu' %zu\n", id.c_str(), s, con ? "col traffico" : "senza", sec, massimo);
            std::fflush(stdout);
        }
    std::printf("%s, %d semi da %.0f s: senza %.1f s, col traffico %.1f s, rapporto %.2f\n", id.c_str(), semi, durata, tot[0], tot[1],
                tot[0] > 0.0 ? tot[1] / tot[0] : 0.0);
}
