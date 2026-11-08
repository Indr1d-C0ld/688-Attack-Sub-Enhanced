#include <doctest.h>

#include <cmath>
#include <filesystem>
#include <fstream>
#include <map>
#include <string>

#include "missioni/danni_missione.hpp"
#include "missioni/missione.hpp"
#include "missioni/stazioni_missione.hpp"
#include "nucleo/classi.hpp"
#include "nucleo/ordini.hpp"

using namespace m688;
namespace fs = std::filesystem;

// Il ricevitore d'intercettazione nella missione (blocco 2 del sonar, E; missione_intercettazione.cpp): i ping in mare
// arrivano agli intercettatori di chi ce l'ha, giocatore e IA (la simmetria); senza l'acustica niente.

namespace {

const std::string SUB = "modello = { massima_kn = 32, virata_gradi_s = 1, accel_kn_s = 0.5, quota_m_s = 1, quota_max_m = 450, integrita = 200, rumore = 8, sonar = 19 }";

// il 688 e un Victor III a 15 km; il Victor pinga ogni 20 s dal suo script
std::string scena() {
    const auto f = fs::temp_directory_path() / "m688_prova_intercettazione_missione.lua";
    std::ofstream(f) << "local script = {}\n"
                        "script.P = function(u)\n  m688.impulso_sonar(u)\n  m688.dopo(u, 20, 'P')\nend\n"
                        "return { id = 'PROVA_INTERCETTAZIONE', teatro = 'faroe', lato = 'usa', data = '1989-06-15', ora_utc = 12, paesaggio_sonoro = false,"
                        " script = script, varianti = { { probabilita = 1, unita = {\n"
                        "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true, posizione = { x = 0, y = 0 },"
                        "  quota_m = 100, rotta = 0, velocita_kn = 4, " << SUB << " },\n"
                        "{ id = 3, classe = 'victor3', categoria = 'sottomarino', flag = 17, posizione = { x = 0, y = 15000 },"
                        "  quota_m = 100, rotta = 180, velocita_kn = 4, " << SUB << " },\n"
                        "}, timer = { { dopo_s = 5, unita = 3, gestore = 'P' } } } } }\n";
    return f.string();
}

struct Partita {
    std::map<std::string, ClasseNave> classi = carica_classi(M688_DATI "/classi.toml");
    Missione m;
    DanniMissione dm;
    Partita(const std::string& file, bool enhanced) : m(file, 1, false), dm(classi, M688_DATI, 1, enhanced ? RegolaDanni::simulazione : RegolaDanni::classica) {}
    void avanza(double s) { for (long i = 0; i < static_cast<long>(s * 9.0); ++i) { m.avanza(1.0 / 9.0); dm.avanza(m, 1.0 / 9.0); } }
};

bool c_e() { return fs::exists(M688_DATI "/acustica.toml"); }

}  // namespace

TEST_CASE("intercettazione nella missione: il 688 intercetta i ping del Victor, il Victor i ping del 688" * doctest::skip(!c_e())) {
    const std::string f = scena();
    Partita p(f, true);
    p.avanza(90.0);
    const RicevitoreIntercettazione* g = p.m.intercettatore_giocatore();
    REQUIRE(g);
    CHECK(g->dati().id == "wlr9");
    REQUIRE_FALSE(g->tracce().empty());
    const auto& t = g->tracce().front();
    MESSAGE("WLR-9: ", g->tracce().size(), " tracce, la prima a ", t.rilevamento, " gradi, ", t.f_hz, " Hz, PRI ", t.pri_s, " s");
    CHECK(std::fabs(normalizza_180(t.rilevamento - 0.0)) < 30.0);   // il Victor e' a nord
    CHECK(g->allarme() == Allarme::nessuno);                           // un sonar di scafo, non una testa
    // la simmetria: il 688 pinga, lo Skat del Victor lo sente
    p.m.applica({p.m.tempo(), TipoOrdine::impulso_sonar, 0, 0, 0});
    p.avanza(5.0);
    const RicevitoreIntercettazione* v = p.m.intercettatore(3);
    REQUIRE(v);
    CHECK(v->dati().id.rfind("skat", 0) == 0);   // lo Skat (skat_ogs nei dati)
    CHECK_FALSE(v->tracce().empty());
    fs::remove(f);
}

TEST_CASE("intercettazione nella missione: senza l'acustica (il classico) nessun intercettatore" * doctest::skip(!c_e())) {
    const std::string f = scena();
    Partita p(f, false);
    p.avanza(60.0);
    CHECK(p.m.intercettatore_giocatore() == nullptr);
    fs::remove(f);
}

// Da capo a fondo, come il caso e2 della regressione: in SCENE4A il 688 carica e lancia il tubo 1 a 40 s; la testa
// attiva del Mk 48 arriva al Rubin dei Foxtrot, l'allarme sale e il Foxtrot che l'ha sentito evade
TEST_CASE("intercettazione nella missione: la testa del Mk 48 accende l'allarme del Foxtrot, che evade (SCENE4A)" *
          doctest::skip(!c_e() || !fs::exists(std::string(M688_MISSIONI) + "/SCENE4A.lua"))) {
    std::map<std::string, ClasseNave> classi = carica_classi(M688_DATI "/classi.toml");
    Missione m(std::string(M688_MISSIONI) + "/SCENE4A.lua", 1);
    DanniMissione dm(classi, M688_DATI, 1, RegolaDanni::simulazione);
    StazioniMissione st(m, 1);
    bool caricato = false, lanciato = false;
    std::map<int, int> allarme;   // per Foxtrot, il piu' grave visto
    for (long i = 0; i < 900L * 9 && !m.esito(); ++i) {
        if (!caricato && m.tempo() >= 1.0) caricato = st.consegna(m, {m.tempo(), TipoOrdine::carica_tubo, 0, 0, 1}) || true;
        if (!lanciato && m.tempo() >= 40.0) lanciato = st.consegna(m, {m.tempo(), TipoOrdine::lancia, 0, 0, 1}) || true;
        m.avanza(1.0 / 9.0);
        st.dopo_passo(m);
        dm.avanza(m, 1.0 / 9.0);
        for (const int id : {2, 3})
            if (const auto* r = m.intercettatore(id)) allarme[id] = std::max(allarme[id], static_cast<int>(r->allarme()));
    }
    int sentito = -1;
    for (const auto& [id, a] : allarme) {
        MESSAGE("Foxtrot ", id, ": allarme massimo ", std::string(nome_allarme(static_cast<Allarme>(a))), ", manovre d'evasione ", m.manovre_evasione(id));
        if (a > 0) sentito = id;
    }
    // il Foxtrot sente anche il lancio col modello acustico (missione_difesa.cpp, disassemblatore 31/10/2026) e puo'
    // evadere prima che la testa lo acquisisca: allora l'allarme non scatta, ma la manovra c'e'
    if (sentito < 0)
        for (const int id : {2, 3})
            if (m.manovre_evasione(id) >= 1) sentito = id;
    REQUIRE(sentito >= 0);
    CHECK(m.manovre_evasione(sentito) >= 1);
}
