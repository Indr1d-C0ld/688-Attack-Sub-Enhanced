#include <doctest.h>

#include <filesystem>
#include <map>
#include <memory>
#include <string>

#include "missioni/danni_missione.hpp"
#include "missioni/danni_sottomarini.hpp"
#include "missioni/missione.hpp"
#include "missioni/registro_missione.hpp"
#include "missioni/stazioni_missione.hpp"
#include "nucleo/carico.hpp"
#include "nucleo/classi.hpp"
#include "nucleo/fondale.hpp"
#include "nucleo/ordini.hpp"

using namespace m688;
namespace fs = std::filesystem;

// Replay deterministico fino a un evento del registro (minicartoline del debriefing, M1/M4/M5): lo stesso seme, lo
// stesso storico degli ordini col carico della Sala siluri e lo stesso passo fisso ridanno gli stessi eventi, con gli
// stessi id e le stesse pose. La partita di prova fa i passi di app/partita.cpp (Partita::avanza) con la regola di
// simulazione: ordini alle stazioni, missione, danni delle navi e dei sottomarini, registro.

namespace {

constexpr double PASSO = 1.0 / 9.0;
const std::string SCENA = std::string(M688_MISSIONI) + "/SCENE1A.lua";
bool ci_sono() { return fs::exists(SCENA) && fs::exists(std::string(M688_DATI) + "/carico.toml"); }

struct PartitaDiProva {
    std::map<std::string, ClasseNave> classi = carica_classi(M688_DATI "/classi.toml");
    CatalogoCarico catalogo = carica_carico(M688_DATI "/carico.toml");
    Missione m;
    DanniMissione dm;
    DanniSottomarini ds;
    StazioniMissione st;
    RegistroOrdini ordini;
    std::uint64_t seme;
    explicit PartitaDiProva(std::uint64_t s, RegistroOrdini o = {}, bool radio_al_briefing = true)
        : m(SCENA, s), dm(classi, M688_DATI, s, RegolaDanni::simulazione), ds(classi, M688_DATI, s),
          st(m, static_cast<std::uint32_t>(s)), ordini(std::move(o)), seme(s) {
        std::map<std::string, double> pescaggi;
        for (const auto& [id, c] : classi) pescaggi[id] = c.pescaggio_m;
        m.imposta_fondale(std::make_shared<const Fondale>(Fondale::carica(std::string(M688_DATI) + "/teatri/" + m.teatro() + ".fdl")),
                          std::move(pescaggi));
        m.imposta_data_ora(24, 10, 1989, 6.0);
        // come app/main.cpp col briefing: la sala radio prima del primo passo. Partita giocata e replay devono farlo
        // allo stesso modo (senza, la regola di simulazione la crea dopo il primo passo, che allora non ha la radio)
        if (radio_al_briefing) m.radio_enhanced();
        ds.prepara(m, m.giocatore()->id);
        st.usa_danni(&ds);
        st.raccogli_tma(true);
        if (ordini.carico()) st.imposta_carico(m, *ordini.carico(), catalogo);
    }
    void avanza() {
        for (const auto& o : ordini.consegna(m.tempo())) st.consegna(m, o);
        m.avanza(PASSO);
        st.dopo_passo(m);
        dm.avanza(m, PASSO);
        ds.avanza(m, PASSO);
        m.raccogli_danni(&ds, &dm);
    }
};

void stesso_evento(const EventoMissione& a, const EventoMissione& b) {
    CHECK(a.id == b.id);
    CHECK(a.tipo == b.tipo);
    CHECK(a.tempo == b.tempo);
    CHECK(a.unita == b.unita);
    CHECK(a.altra == b.altra);
    CHECK(a.arma == b.arma);
    CHECK(a.pos.x == b.pos.x);
    CHECK(a.pos.y == b.pos.y);
    CHECK(a.testo_it == b.testo_it);
    REQUIRE(a.pose.size() == b.pose.size());
    for (std::size_t i = 0; i < a.pose.size(); ++i) {
        CHECK(a.pose[i].unita == b.pose[i].unita);
        CHECK(a.pose[i].pos.x == b.pose[i].pos.x);
        CHECK(a.pose[i].pos.y == b.pose[i].pos.y);
        CHECK(a.pose[i].quota == b.pose[i].quota);
        CHECK(a.pose[i].rotta == b.pose[i].rotta);
    }
}

}  // namespace

TEST_CASE("ordini: il carico della Sala siluri nello storico, e i file senza restano validi") {
    RegistroOrdini r;
    CaricoPartenza c;
    c.posizioni = {"mk48", "mk48_adcap", "", "harpoon"};
    c.tipi = {"tubo", "tubo", "tubo", "pozzi"};
    c.rastrelliera = {{"mk48", 12}, {"harpoon", 2}};
    c.pozzi = {{"tlam_c", 12}};
    c.esche = {{"adc_mk2", 6}};
    r.imposta_carico(c);
    r.ordina({3.0, TipoOrdine::telegrafo, 0.5, 0, 0});
    r.consegna(5.0);
    const std::string testo = "# commento\n" + r.serializza();
    const RegistroOrdini letto = RegistroOrdini::da_testo(testo);
    REQUIRE(letto.carico());
    CHECK(*letto.carico() == c);
    CHECK(letto.serializza().find("carico posizioni 4 mk48 mk48_adcap - harpoon") != std::string::npos);
    RegistroOrdini replay = RegistroOrdini::da_testo(testo);
    const auto o = replay.consegna(10.0);
    REQUIRE(o.size() == 1);
    CHECK(o[0].tipo == TipoOrdine::telegrafo);
    // vuoti e senza carico
    CaricoPartenza vuoto;
    RegistroOrdini rv;
    rv.imposta_carico(vuoto);
    CHECK(*RegistroOrdini::da_testo(rv.serializza()).carico() == vuoto);
    CHECK_FALSE(RegistroOrdini::da_testo("1 telegrafo 0.5 0 0\n").carico());
    CHECK_THROWS(RegistroOrdini::da_testo("carico posizioni 3 mk48\n"));
    CHECK_THROWS(RegistroOrdini::da_testo("carico rastrelliera mk48\n"));
    CHECK_THROWS(RegistroOrdini::da_testo("carico ignoto 1\n"));
}

TEST_CASE("replay: stesso seme, ordini e carico ridanno gli eventi del registro fino all'istante scelto" * doctest::skip(!ci_sono())) {
    constexpr std::uint64_t SEME = 11;
    // la partita giocata: carico fuori standard (il tubo 4 senza l'Harpoon), poi bersaglio, lancio e manovre
    PartitaDiProva gioco(SEME);
    {
        SalaSiluri sala(gioco.catalogo, gioco.m.giocatore()->classe, gioco.m.limiti_carico());
        sala.carico_iniziale();
        sala.togli(Posto::tubo, "harpoon", 4);
        gioco.ordini.imposta_carico(sala.carico());
        gioco.st.imposta_carico(gioco.m, sala.carico(), gioco.catalogo);
    }
    int bersaglio = -1;
    for (const auto& [id, u] : gioco.m.unita()) if (u.categoria == "nave") { bersaglio = id; break; }
    REQUIRE(bersaglio >= 0);
    std::map<int, std::vector<Ordine>> da_dare = {
        {9 * 10, {{0, TipoOrdine::carica_tubo, 0, 0, 1}}},
        {9 * 20, {{0, TipoOrdine::designa_bersaglio, 0, 0, bersaglio}}},
        {9 * 150, {{0, TipoOrdine::lancia, 0, 0, 1}}},
        {9 * 60, {{0, TipoOrdine::telegrafo, 0.5, 0, 0}, {0, TipoOrdine::timone, 1.8, 0, 0}}},
        {9 * 200, {{0, TipoOrdine::timone, 0.0, 0, 0}, {0, TipoOrdine::quota, 100.0, 0.5, 0}}},
    };
    constexpr int PASSI = 9 * 900;
    for (int i = 0; i < PASSI; ++i) {
        if (const auto it = da_dare.find(i); it != da_dare.end())
            for (Ordine o : it->second) { o.tempo = gioco.m.tempo(); gioco.ordini.ordina(o); }
        gioco.avanza();
    }
    REQUIRE(gioco.m.errori().empty());
    const auto& eventi = gioco.m.eventi_missione();
    REQUIRE(eventi.size() > 3);
    const EventoMissione* lancio = nullptr;
    for (const auto& e : eventi) if (e.tipo == TipoEvento::lancio_giocatore) lancio = &e;
    REQUIRE(lancio);
    CHECK(gioco.ordini.storico().size() == 7);

    // lo storico salvato come testo e rigiocato da capo: la partita nuova riprende il carico dallo storico
    const std::string testo = gioco.ordini.serializza();
    REQUIRE(RegistroOrdini::da_testo(testo).carico());
    CHECK(*RegistroOrdini::da_testo(testo).carico() == *gioco.ordini.carico());
    for (const EventoMissione* scelto : {lancio, &eventi[eventi.size() / 2], &eventi.back()}) {
        PartitaDiProva replay(SEME, RegistroOrdini::da_testo(testo));
        const long massimo = passi_per_evento(*scelto, PASSO);
        long passi = 0;
        while (!replay.m.registro().evento(scelto->id) && passi < massimo) { replay.avanza(); ++passi; }
        CAPTURE(nome_evento(scelto->tipo));
        CAPTURE(scelto->tempo);
        REQUIRE(replay.m.registro().evento(scelto->id));
        CHECK(passi <= massimo);
        // tutti gli eventi fino a quello scelto, uguali; le unita' coinvolte nella stessa posa
        for (const auto& e : replay.m.eventi_missione()) {
            const EventoMissione* x = gioco.m.registro().evento(e.id);
            REQUIRE(x);
            stesso_evento(*x, e);
        }
        stesso_evento(*scelto, *replay.m.registro().evento(scelto->id));
        CHECK(replay.m.tempo() <= scelto->tempo + 2.0 * PASSO);
    }
    // un altro seme da' un'altra partita (la prova distingue davvero)
    PartitaDiProva altro(SEME + 1, RegistroOrdini::da_testo(testo));
    for (int i = 0; i < PASSI; ++i) altro.avanza();
    bool diversa = altro.m.eventi_missione().size() != eventi.size();
    for (std::size_t i = 0; !diversa && i < eventi.size(); ++i)
        diversa = altro.m.eventi_missione()[i].tempo != eventi[i].tempo || altro.m.eventi_missione()[i].tipo != eventi[i].tipo;
    CHECK(diversa);
}
