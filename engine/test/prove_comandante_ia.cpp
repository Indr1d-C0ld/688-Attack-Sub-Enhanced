#include <doctest.h>

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <map>
#include <set>
#include <memory>
#include <optional>
#include <string>

#include "missioni/comandante_ia.hpp"
#include "missioni/danni_missione.hpp"
#include "missioni/danni_sottomarini.hpp"
#include "missioni/missione.hpp"
#include "missioni/registro_missione.hpp"
#include "missioni/sandbox_originali.hpp"
#include "missioni/stazioni_missione.hpp"
#include "nucleo/classi.hpp"
#include "nucleo/geometria.hpp"
#include "nucleo/ordini.hpp"

using namespace m688;
namespace fs = std::filesystem;

// Il comandante IA del battello del giocatore (sandbox V1, docs/19 §5; missioni/comandante_ia.hpp): comanda con gli
// ordini del giocatore, e gli ordini registrati rifanno la stessa partita senza di lui.

namespace {

const std::string SCENA = std::string(M688_MISSIONI) + "/SCENE1A.lua";   // TORPEX '89: tre Forrest Sherman da affondare
bool c_e() { return fs::exists(SCENA) && fs::exists(M688_DATI "/interazioni.toml"); }

// una partita come quella di app/partita: ordini dal registro, missione, stazioni, danni
struct Sandbox {
    std::map<std::string, ClasseNave> classi = carica_classi(M688_DATI "/classi.toml");
    Missione m;
    DanniMissione dm;
    DanniSottomarini ds;
    StazioniMissione st;
    RegistroOrdini ordini;
    std::unique_ptr<ComandanteIa> comandante;
    Sandbox(std::uint64_t seme, bool con_comandante, const std::string& scena = SCENA, const std::optional<PianoOriginale>& piano = std::nullopt)
        : m(scena, seme), dm(classi, M688_DATI, seme, RegolaDanni::simulazione), ds(classi, M688_DATI, seme), st(m, static_cast<std::uint32_t>(seme)) {
        m.senza_giocatore(true);
        if (const auto* g = m.giocatore()) ds.prepara(m, g->id);
        st.usa_danni(&ds);
        OpzioniComandante oc;
        oc.piano = piano;
        if (con_comandante) comandante = std::make_unique<ComandanteIa>(m, oc, seme);
    }
    void avanza(double s) {
        for (long i = 0; i < static_cast<long>(s * 9.0); ++i) {
            if (comandante)
                for (const auto& o : comandante->passo(m, st)) ordini.ordina(o);
            for (const auto& o : ordini.consegna(m.tempo())) {
                if (ds.consegna(m, o)) continue;
                st.consegna(m, o);
            }
            m.avanza(1.0 / 9.0);
            st.dopo_passo(m);
            dm.avanza(m, 1.0 / 9.0);
            ds.avanza(m, 1.0 / 9.0);
            if (m.esito()) break;
        }
    }
};

}  // namespace

TEST_CASE("comandante IA: nella TORPEX cambia fase, comanda con gli ordini del giocatore e finisce la missione" * doctest::skip(!c_e())) {
    Sandbox p(1, true);
    p.avanza(1800.0);
    REQUIRE(p.comandante);
    const auto& storia = p.comandante->storia();
    REQUIRE_FALSE(storia.empty());
    std::set<StatoComandante::Fase> fasi;
    for (const auto& [t, s] : storia) fasi.insert(s.fase);
    CHECK(fasi.size() >= 2);
    CHECK_FALSE(p.ordini.storico().empty());
    const auto& reg = p.m.eventi_missione();
    CHECK(std::any_of(reg.begin(), reg.end(), [](const EventoMissione& e) { return e.tipo == TipoEvento::comandante_ia; }));
    std::string fasi_testo;
    for (const auto& [t, s] : storia) fasi_testo += std::to_string(static_cast<int>(t)) + " " + nome_fase_comandante(s.fase) + " (" + s.motivo + "); ";
    MESSAGE("comandante: ", fasi_testo);
    // lo spettatore guarda da fuori: i testi del battello in terza persona, col suo nome
    for (const auto& e : reg) {
        CHECK_MESSAGE(e.testo_it.find("ci ha scoperti") == std::string::npos, e.testo_it);
        CHECK_MESSAGE(e.testo_it.find("di noi") == std::string::npos, e.testo_it);
        CHECK_MESSAGE(e.testo_en.find(" us") == std::string::npos, e.testo_en);
        if (e.tipo == TipoEvento::lancio_giocatore) CHECK_MESSAGE(e.testo_it.rfind("Los Angeles lancia", 0) == 0, e.testo_it);
    }
    const auto lanci = std::count_if(reg.begin(), reg.end(), [](const EventoMissione& e) { return e.tipo == TipoEvento::lancio_giocatore; });
    MESSAGE("lanci del giocatore: ", lanci, ", esito ", p.m.esito().value_or(-1));
}

// la sua TMA vale per gli obiettivi con da_tma, come il MATE del giocatore (Missione::stima_tma_giocatore): la SCENE5B col
// piano. Nella S20 no: senza piano non fa la TMA sull'SSBN, che non ha armi libere.
// Quasi tutte le stime sono sui biologici vicini, non sul 688 che pedina: in coda a un biologico a 1,8 nodi una soluzione
// «osservabile» puo' sbagliare di 100 gradi per qualche minuto, quindi la prova guarda piu' semi e la quota, non ogni stima.
// Limite noto: sul Los Angeles, l'obiettivo vero, 3 stime su 4 superano i 20 gradi, su 40 semi
// (tools/re/prove/portata_sonar/punto1/esiti/tma/riassunto_40.txt)
TEST_CASE("comandante IA: le soluzioni osservabili della sua TMA diventano stime di rotta" * doctest::skip(!c_e())) {
    int stime = 0, entro = 0;
    for (std::uint64_t seme = 1; seme <= 6; ++seme) {
        Sandbox p(seme, true, std::string(M688_MISSIONI) + "/SCENE5B.lua", piano_originale(M688_DATI, "SCENE5B"));
        p.avanza(1800.0);
        for (const auto& x : p.m.stime_tma_giocatore()) {
            ++stime;
            entro += std::fabs(normalizza_180(x.rotta - x.rotta_vera)) < 20.0;
        }
    }
    MESSAGE("stime entro 20 gradi: ", entro, " su ", stime);
    REQUIRE(stime >= 10);
    CHECK(entro >= 0.8 * stime);
}

// l'esca lanciata ha la classe del battello: nel registro non e' un primo contatto col battello stesso
TEST_CASE("comandante IA: la sua esca non e' un contatto nuovo nel registro" * doctest::skip(!c_e())) {
    Sandbox p(1, false);
    p.m.acustica_enhanced(M688_DATI);
    p.avanza(1.0);
    p.ordini.ordina({p.m.tempo(), TipoOrdine::carica_tubo, 0.0, 0.0, 5});
    p.avanza(40.0);
    p.ordini.ordina({p.m.tempo(), TipoOrdine::lancia, 0.0, 0.0, 5});
    p.avanza(20.0);
    REQUIRE(p.m.giocatore());
    REQUIRE(std::any_of(p.m.unita().begin(), p.m.unita().end(), [&](const auto& k) { return (k.second.flag & 0x80) && k.second.lanciatore == p.m.giocatore()->id; }));
    p.avanza(180.0);
    for (const auto& e : p.m.eventi_missione())
        if (e.tipo == TipoEvento::scoperta) CHECK_MESSAGE(e.testo_it.find("primo contatto con Los Angeles") == std::string::npos, e.testo_it);
}

// il ramo generico del missile legge il pannello come il giocatore: con un neutrale creduto nella scatola non tira, si
// avvicina per il siluro. I contatti sono messi a mano: la prova e' sul piano, non su come li trova
TEST_CASE("comandante IA: con un neutrale nella scatola non tira il missile" * doctest::skip(!c_e())) {
    const auto prova = [](double x_cargo) {
        const auto f = fs::temp_directory_path() / "m688_prova_comandante_scatola.lua";
        const std::string nave = "modello = { massima_kn = 30, virata_gradi_s = 1, accel_kn_s = 0.5, quota_m_s = 0, quota_max_m = 0, integrita = 100, rumore = 14, sonar = 0 }";
        const std::string sub = "modello = { massima_kn = 32, virata_gradi_s = 1, accel_kn_s = 0.5, quota_m_s = 1, quota_max_m = 450, integrita = 200, rumore = 8, sonar = 19 }";
        std::ofstream(f)
            << "local script = {}\n"
               "script.H = function(u)\n  m688.lancio_missile(u)\nend\n"
               "script.D = function(u, p)\n  m688.bersaglio(u, p ~= nil and p[1] or 0)\n  m688.salva_modello(u)\nend\n"
               "return { id = 'PROVA_COMANDANTE_SCATOLA', teatro = 'faroe', lato = 'usa', data = '1989-06-15', ora_utc = 12, paesaggio_sonoro = false,"
               " carriera = { situazione = 'guerra' }, dotazione = { harpoon = 4, mk48 = 6 }, tubi = { 'harpoon', 'harpoon', 'mk48', 'mk48' }, armi = { harpoon = { gestore = 'H' } }, gestori_giocatore = { designa_bersaglio = 'D' }, script = script,"
               " varianti = { { probabilita = 1, unita = {\n"
               "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true, posizione = { x = 0, y = 0 },"
               "  quota_m = 15, rotta = 0, velocita_kn = 4, " << sub << " },\n"
               "{ id = 2, classe = 'slava', categoria = 'nave', flag = 18, posizione = { x = 0, y = 30000 }, quota_m = 0, rotta = 0,"
               "  velocita_kn = 0, " << nave << " },\n"
               "{ id = 5, classe = 'cargo', categoria = 'nave', flag = 2, posizione = { x = " << x_cargo << ", y = 29000 }, quota_m = 0, rotta = 0,"
               "  velocita_kn = 0, " << nave << " },\n"
               "}, timer = {} } } }\n";
        Sandbox p(1, true, f.string());
        p.avanza(1.0);
        p.m.classifica_contatto(0, 5, "cargo");
        int lanci = 0;
        for (int k = 0; k < 1500 && lanci == 0; ++k) {
            for (const int id : {2, 5}) {
                auto& c = p.m.trova(0)->contatti[id];
                const auto& u = p.m.unita().at(id);
                c.qualita = 1500.0;
                c.posizione_nota = true;
                c.pos_stimata = u.pos;
                c.t_posizione = c.ultima_scoperta = p.m.tempo();
                c.rilevamento = rilevamento(p.m.giocatore()->pos, u.pos);
            }
            for (int tubo = 1; tubo <= 4; ++tubo) p.st.armi().messaggio_missione(9 + tubo);   // il timer del gestore della missione: qui non c'e'
            p.avanza(1.0);
            for (const auto& l : p.m.lanci_missili()) lanci += l.unita == 0;
        }
        fs::remove(f);
        return lanci;
    };
    CHECK(prova(-40000.0) > 0);   // il cargo fuori dall'area: tira
    CHECK(prova(500.0) == 0);
}

TEST_CASE("comandante IA: stesso seme stessi ordini, e gli ordini registrati rifanno la partita senza di lui" * doctest::skip(!c_e())) {
    Sandbox a(3, true), b(3, true);
    a.avanza(900.0);
    b.avanza(900.0);
    CHECK(a.ordini.serializza() == b.ordini.serializza());
    // il replay: solo lo storico degli ordini, niente comandante
    Sandbox r(3, false);
    for (const auto& o : a.ordini.storico()) r.ordini.ordina(o);
    r.avanza(900.0);
    const auto& ea = a.m.eventi_missione();
    const auto& er = r.m.eventi_missione();
    // gli eventi della partita sono gli stessi (tolte le note del comandante, che il replay non ha)
    const auto senza_note = [](const std::vector<EventoMissione>& v) {
        std::vector<std::pair<int, double>> x;
        for (const auto& e : v) if (e.tipo != TipoEvento::comandante_ia) x.emplace_back(static_cast<int>(e.tipo), e.tempo);
        return x;
    };
    CHECK(senza_note(ea) == senza_note(er));
    const auto* ga = a.m.giocatore();
    const auto* gr = r.m.giocatore();
    REQUIRE(ga);
    REQUIRE(gr);
    CHECK(ga->pos.x == doctest::Approx(gr->pos.x));
    CHECK(ga->pos.y == doctest::Approx(gr->pos.y));
}
