#include <doctest.h>

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <map>
#include <string>

#include "missioni/danni_missione.hpp"
#include "missioni/missione.hpp"
#include "missioni/registro_missione.hpp"
#include "nucleo/classi.hpp"
#include "nucleo/ordini.hpp"

using namespace m688;
namespace fs = std::filesystem;

// Le luci di navigazione nella missione dell'Enhanced (docs/27; missioni/src/missione_luci.cpp): lo stato per unita'
// (chiave dello scenario, danni, ordine del giocatore, stato predefinito con la guerra), l'ordine e il registro, la
// vedetta di notte. Senza luci_enhanced (il classico) nessuna luce.

namespace {

const std::string NAVE = "modello = { massima_kn = 30, virata_gradi_s = 1, accel_kn_s = 0.5, quota_m_s = 0, quota_max_m = 0, integrita = 100, rumore = 14, sonar = 0 }";
const std::string SUB = "modello = { massima_kn = 32, virata_gradi_s = 1, accel_kn_s = 0.5, quota_m_s = 1, quota_max_m = 450, integrita = 200, rumore = 8, sonar = 19 }";

// il 688 (quota_688) a mezzanotte di dicembre; un mercantile a 4 km che va a 8 nodi, uno fermo, una Krivak sovietica a 3 km (o a
// krivak_m), un peschereccio a 3 nodi; la situazione e le chiavi d'unita' a richiesta
std::string scena(double quota_688, const std::string& situazione, const std::string& luci_krivak = "", double krivak_m = 3000.0,
                  const std::string& chiavi_krivak = "") {
    const auto f = fs::temp_directory_path() / "m688_prova_luci.lua";
    std::ofstream(f) << "return { id = 'PROVA_LUCI', teatro = 'faroe', lato = 'usa', data = '1989-12-15', ora_utc = 0,"
                        " paesaggio_sonoro = false, script = {}, carriera = { situazione = '" << situazione << "', difficolta = 3 },"
                        " varianti = { { probabilita = 1, unita = {\n"
                        "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true, posizione = { x = 0, y = 0 },"
                        "  quota_m = " << quota_688 << ", rotta = 0, velocita_kn = 3, " << SUB << " },\n"
                        "{ id = 2, classe = 'mercantile', categoria = 'nave', flag = 2, posizione = { x = 0, y = 4000 }, quota_m = 0,"
                        "  rotta = 90, velocita_kn = 8, " << NAVE << " },\n"
                        "{ id = 3, classe = 'mercantile', categoria = 'nave', flag = 2, posizione = { x = 4000, y = 0 }, quota_m = 0,"
                        "  rotta = 90, velocita_kn = 0, " << NAVE << " },\n"
                        "{ id = 4, classe = 'krivak2', categoria = 'nave', flag = 18, posizione = { x = 0, y = " << -krivak_m << " }, quota_m = 0,"
                        "  rotta = 90, velocita_kn = 10, " << NAVE << (luci_krivak.empty() ? std::string() : ", luci = '" + luci_krivak + "'") << chiavi_krivak << " },\n"
                        "{ id = 5, classe = 'peschereccio', categoria = 'nave', flag = 2, posizione = { x = -4000, y = 0 }, quota_m = 0,"
                        "  rotta = 90, velocita_kn = 3, " << NAVE << " },\n"
                        "}, timer = {} } } }\n";
    return f.string();
}

struct Partita {
    std::map<std::string, ClasseNave> classi = carica_classi(M688_DATI "/classi.toml");
    Missione m;
    DanniMissione dm;
    // le luci le accende DanniMissione con la regola di simulazione (Enhanced); col classico non ci sono
    Partita(const std::string& file, bool enhanced = true)
        : m(file, 1, false), dm(classi, M688_DATI, 1, enhanced ? RegolaDanni::simulazione : RegolaDanni::classica) {
        if (enhanced) m.luci_enhanced(M688_DATI);   // gia' prima del primo passo, per gli ordini (idempotente)
    }
    void avanza(double s) { for (long i = 0; i < static_cast<long>(s * 9.0); ++i) { m.avanza(1.0 / 9.0); dm.avanza(m, 1.0 / 9.0); } }
};

bool c_e() { return fs::exists(M688_DATI "/luci.toml"); }

}  // namespace

TEST_CASE("luci nella missione: lo stato predefinito, la guerra, la chiave dello scenario" * doctest::skip(!c_e())) {
    {   // in pace: il mercantile in moto, quello fermo alla fonda, il peschereccio in pesca, la Krivak in moto
        Partita p(scena(18.0, "pace"));
        p.avanza(1.0);
        CHECK(p.m.stato_luci(2) == luci::Stato::moto);
        CHECK(p.m.stato_luci(3) == luci::Stato::fonda);
        CHECK(p.m.stato_luci(5) == luci::Stato::pesca);
        CHECK(p.m.stato_luci(4) == luci::Stato::moto);
    }
    {   // in guerra la Krivak spegne; il mercantile neutrale no
        Partita p(scena(18.0, "guerra"));
        p.avanza(1.0);
        CHECK(p.m.stato_luci(4) == luci::Stato::spente);
        CHECK(p.m.stato_luci(2) == luci::Stato::moto);
    }
    {   // la chiave dello scenario vince sullo stato predefinito, anche in guerra
        Partita p(scena(18.0, "guerra", "manovra_limitata"));
        p.avanza(1.0);
        CHECK(p.m.stato_luci(4) == luci::Stato::manovra_limitata);
    }
    {   // una nave in avaria: senza governo
        Partita p(scena(18.0, "pace"));
        REQUIRE(p.m.avaria(2));
        p.avanza(1.0);
        CHECK(p.m.stato_luci(2) == luci::Stato::non_governa);
    }
    {   // il classico (regola classica, senza luci_enhanced): nessuna luce
        Partita p(scena(18.0, "pace"), false);
        p.avanza(1.0);
        CHECK(p.m.stato_luci(2) == luci::Stato::spente);
        CHECK_FALSE(p.m.luci_giocatore_accese());
    }
}

TEST_CASE("luci nella missione: l'ordine del giocatore va nel registro" * doctest::skip(!c_e())) {
    Partita p(scena(0.0, "pace"));   // il 688 in superficie, in pace: accese di serie
    p.avanza(1.0);
    CHECK(p.m.luci_giocatore_accese());
    REQUIRE(p.m.applica(Ordine{p.m.tempo(), TipoOrdine::luci_giocatore, 0.0}));
    CHECK_FALSE(p.m.luci_giocatore_accese());
    REQUIRE(p.m.applica(Ordine{p.m.tempo(), TipoOrdine::luci_giocatore, 1.0}));
    CHECK(p.m.luci_giocatore_accese());
    p.avanza(1.0);
    REQUIRE(p.m.cambi_luci().size() == 2u);
    CHECK(p.m.cambi_luci()[0].stato == luci::Stato::spente);
    const auto& ev = p.m.eventi_missione();
    CHECK(std::count_if(ev.begin(), ev.end(), [](const EventoMissione& e) { return e.tipo == TipoEvento::luci; }) == 2);
}

// la scoperta automatica a 9 miglia di Missione::valuta di notte vale solo entro la portata notturna (vista_automatica_di_notte)
TEST_CASE("luci nella missione: di notte la Krivak vede il 688 illuminato in superficie, non quello a luci spente" * doctest::skip(!c_e())) {
    for (const bool accese : {true, false}) {
        CAPTURE(accese);
        // a 7 km: la sagoma del 688 emerso di notte arriva a circa 1,5 km, i fanali molto piu' in la'. La Krivak senza radar
        // (EMCON silenzio), se no lo scopre col radar. I periscopi li accende DanniMissione al primo passo, dopo il meteo
        Partita p(scena(0.0, "crisi", "", 7000.0, ", emcon = 'silenzio'"));
        REQUIRE(p.m.applica(Ordine{p.m.tempo(), TipoOrdine::luci_giocatore, accese ? 1.0 : 0.0}));
        p.avanza(1.0);   // la luce si calcola al primo passo dei periscopi
        REQUIRE(p.m.luce_periscopi() < 0.2);   // mezzanotte
        p.avanza(119.0);
        const auto& k = p.m.unita().at(4).contatti;
        const bool vista = k.count(0) && k.at(0).sensore == "vista";
        CHECK(vista == accese);
    }
}

