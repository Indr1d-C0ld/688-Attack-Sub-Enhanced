#include <doctest.h>

#include <filesystem>
#include <fstream>
#include <map>
#include <string>

#include "missioni/danni_missione.hpp"
#include "missioni/missione.hpp"
#include "nucleo/classi.hpp"

using namespace m688;
namespace fs = std::filesystem;

// Il sonarista dell'equipaggio nella missione (S5d): rapporti con l'esperienza, classificazioni nel registro

namespace {

std::string scena_sonarista() {
    const auto f = fs::temp_directory_path() / "m688_prova_sonarista.lua";
    std::ofstream o(f);
    const std::string sub = "modello = { massima_kn = 40, virata_gradi_s = 2, accel_kn_s = 0.5, quota_m_s = 2, quota_max_m = 600, integrita = 100, rumore = 12, sonar = 19 }";
    o << "return { id = 'PROVA_SONARISTA', teatro = 'faroe', paesaggio_sonoro = false, lato = 'usa', script = {},\n"
      << "varianti = { { probabilita = 1, unita = {\n"
      << "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true, posizione = { x = 0, y = 0 },"
         " quota_m = 100, rotta = 90, velocita_kn = 5, " << sub << " },\n"
      << "{ id = 1, classe = 'alfa', categoria = 'sottomarino', flag = 17, posizione = { x = 0, y = 8000 }, quota_m = 100,"
         " rotta = 270, velocita_kn = 25, " << sub << ", rotta_punti = { { x = -400000, y = 8000 } } },\n"
      << "} } } }\n";
    return f.string();
}

struct Gioco {
    std::map<std::string, ClasseNave> classi = carica_classi(M688_DATI "/classi.toml");
    Missione m;
    std::unique_ptr<DanniMissione> dm;
    Gioco(std::optional<Esperienza> e, bool enhanced = true) : m(scena_sonarista(), 5) {
        if (e) m.imposta_sonarista(*e);
        if (enhanced) dm = std::make_unique<DanniMissione>(classi, M688_DATI, 5, RegolaDanni::simulazione);
    }
    void avanza(double s) {
        for (long i = 0; i < static_cast<long>(s * 9.0); ++i) {
            m.avanza(1.0 / 9.0);
            if (dm) dm->avanza(m, 1.0 / 9.0);
        }
    }
    int conta(int codice) const {
        int n = 0;
        for (const auto& a : m.avvisi_sonarista()) n += a.codice == codice;
        return n;
    }
};

}  // namespace

TEST_CASE("sonarista missione: rapporti dell'esperto sul contatto rumoroso, classificazione nel registro") {
    Gioco g(Esperienza::esperto);
    g.avanza(600.0);
    REQUIRE(g.m.errori().empty());
    REQUIRE(g.m.sonarista());
    CHECK(g.conta(410) >= 1);
    CHECK(g.conta(411) >= 1);
    const auto& primo = g.m.avvisi_sonarista().front();
    CHECK(primo.codice == 410);
    CHECK(primo.numero == 1);
    CHECK(primo.sorgente == 1);
    CHECK_FALSE(primo.testo_it.empty());
    bool registro = false;
    for (const auto& e : g.m.registro().eventi())
        if (e.tipo == TipoEvento::classificazione && e.codice == 411) {
            registro = true;
            CHECK(e.unita == 1);
        }
    CHECK(registro);
    const auto esito = g.m.registro().esito(g.m);
    CHECK(esito.classificazioni_sonarista >= 1);
    CHECK(esito.classificazioni_giuste + esito.classificazioni_errate == 0);   // non sono del giocatore
    // deterministico
    Gioco h(Esperienza::esperto);
    h.avanza(600.0);
    REQUIRE(h.m.avvisi_sonarista().size() == g.m.avvisi_sonarista().size());
    for (std::size_t i = 0; i < g.m.avvisi_sonarista().size(); ++i) CHECK(h.m.avvisi_sonarista()[i].tempo == g.m.avvisi_sonarista()[i].tempo);
}

TEST_CASE("sonarista missione: la recluta riferisce piu' tardi; senza sonarista o nel classico niente") {
    Gioco esperto(Esperienza::esperto), recluta(Esperienza::recluta);
    esperto.avanza(300.0);
    recluta.avanza(300.0);
    REQUIRE_FALSE(esperto.m.avvisi_sonarista().empty());
    if (!recluta.m.avvisi_sonarista().empty()) CHECK(recluta.m.avvisi_sonarista().front().tempo >= esperto.m.avvisi_sonarista().front().tempo);
    Gioco nessuno(std::nullopt);
    nessuno.avanza(300.0);
    CHECK(nessuno.m.sonarista() == nullptr);
    CHECK(nessuno.m.avvisi_sonarista().empty());
    Gioco classico(Esperienza::esperto, false);
    classico.avanza(300.0);
    CHECK(classico.m.sonarista() == nullptr);
}

TEST_CASE("sonarista missione: col sonarista i rapporti di contatto sono solo i suoi") {
    const auto conta = [](const Missione& m, int codice) {
        int n = 0;
        for (const auto& x : m.messaggi_sonar()) n += x.codice == codice;
        return n;
    };
    Gioco con(Esperienza::medio), senza(std::nullopt);
    con.avanza(600.0);
    senza.avanza(600.0);
    CHECK(conta(senza.m, 310) >= 1);   // nuovo contatto della missione
    CHECK(conta(con.m, 310) == 0);
    CHECK(conta(con.m, 340) + conta(con.m, 341) == 0);
    CHECK(con.conta(410) >= 1);
}

// La verita' delle classificazioni del sonarista (la principale, 29/10/2026: in SCENE1B ogni contatto «Portarinfuse»
// risultava giusto nel registro). Qualunque cosa dica il sonarista, errata confronta la classe detta con quella vera
// della sorgente, e il registro la riporta
TEST_CASE("sonarista missione: errata dice se la classe detta e' quella vera (SCENE1B)") {
    std::map<std::string, ClasseNave> classi = carica_classi(M688_DATI "/classi.toml");
    Missione m(M688_DATI "/../missioni/SCENE1B.lua", 1);
    m.imposta_sonarista(Esperienza::recluta);   // la recluta sbaglia di piu
    DanniMissione dm(classi, M688_DATI, 1, RegolaDanni::simulazione);
    for (long i = 0; i < static_cast<long>(900.0 * 9.0); ++i) {
        m.avanza(1.0 / 9.0);
        dm.avanza(m, 1.0 / 9.0);
    }
    int con_classe = 0, sbagliate = 0;
    for (const auto& a : m.avvisi_sonarista()) {
        if (a.codice != 411 || a.classe.empty() || a.classe == "biologico" || !m.unita().count(a.sorgente)) continue;
        ++con_classe;
        const bool vera = a.classe == m.unita().at(a.sorgente).classe;
        if (!vera) ++sbagliate;
        CAPTURE(a.classe);
        CAPTURE(m.unita().at(a.sorgente).classe);
        CHECK(a.errata == !vera);
    }
    for (const auto& e : m.registro().eventi())
        if (e.tipo == TipoEvento::classificazione && e.codice == 411 && !e.classe_creduta.empty() && e.classe_creduta != "biologico")
            CHECK(e.giusta == (e.classe_creduta == e.classe));
    MESSAGE("SCENE1B: ", con_classe, " classificazioni con la classe, ", sbagliate, " sbagliate");
}
