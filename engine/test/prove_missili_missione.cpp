#include <doctest.h>

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <map>
#include <string>
#include <vector>

#include "missioni/danni_missione.hpp"
#include "missioni/missione.hpp"
#include "missioni/registro_missione.hpp"
#include "nucleo/classi.hpp"
#include "nucleo/ordini.hpp"

using namespace m688;
namespace fs = std::filesystem;

// I missili del modello nella missione dell'Enhanced (docs/21 §11.9; missione_missili.cpp): il tiro del tubo per la
// stazione, il lancio col piano del battello, il primo eco agganciato, l'impatto con la difesa della classe, il gestore
// della missione che gira ancora senza il suo ATTACK_RND.

namespace {

const std::string NAVE = "modello = { massima_kn = 30, virata_gradi_s = 1, accel_kn_s = 0.5, quota_m_s = 0, quota_max_m = 0, integrita = 100, rumore = 14, sonar = 0 }";
const std::string SUB = "modello = { massima_kn = 32, virata_gradi_s = 1, accel_kn_s = 0.5, quota_m_s = 1, quota_max_m = 450, integrita = 200, rumore = 8, sonar = 19 }";

// il 688 a quota periscopica; lo Slava a 14 km verso nord, un Krivak sulla linea di tiro a 10 km, un cargo neutrale
// lontano. Il gestore dell'Harpoon e' quello della libreria (MISSILE, messaggio 49, ATTACK_RND dopo 20 s)
// con `gambe` il 688 accosta a 090 e va a 10 nodi dopo 5 minuti (lo script T), poi a 330 dopo 20 (T2): con due gambe la
// TMA dell'equipaggio diventa osservabile
std::string scena(bool gambe = false, const std::string& altre = "", const std::string& chiavi = "") {
    const auto f = fs::temp_directory_path() / (gambe ? "m688_prova_missili_missione_gambe.lua" : "m688_prova_missili_missione.lua");
    std::ofstream(f)
        << "local script = {}\n"
           "script.T = function(u)\n  m688.rotta(u, 90)\n  m688.telegrafo(u, 0.3125)\n  m688.salva_modello(u)\nend\n"
           "script.T2 = function(u)\n  m688.rotta(u, 330)\n  m688.salva_modello(u)\nend\n"
           "script.H = function(u)\n  m688.lancio_missile(u)\n  m688.messaggio_al_giocatore(u, 49)\n  m688.seleziona_collegata(u)\n"
           "  m688.dopo(u, 20, 'H2')\nend\n"
           "script.H2 = function(u)\n  m688.attacco_casuale(u, 75)\nend\n"
           "script.D = function(u, p)\n  m688.bersaglio(u, p ~= nil and p[1] or 0)\n  m688.salva_modello(u)\nend\n"
           "return { id = 'PROVA_MISSILI', teatro = 'faroe', lato = 'usa', data = '1989-06-15', ora_utc = 12, paesaggio_sonoro = false," << chiavi <<
           " tubi = { 'harpoon', 'harpoon', 'mk48', 'mk48', 'esca', 'esca', 'harpoon' }, armi = { harpoon = { gestore = 'H' } }, gestori_giocatore = { designa_bersaglio = 'D' },"
           " script = script, varianti = { { probabilita = 1, unita = {\n"
           "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true, posizione = { x = 0, y = 0 },"
           "  quota_m = 6, rotta = 0, velocita_kn = 4, " << SUB << " },\n"
           "{ id = 2, classe = 'slava', categoria = 'nave', flag = 18, posizione = { x = 0, y = 14000 }, quota_m = 0, rotta = 90,"
           "  velocita_kn = 10, " << NAVE << " },\n"
           "{ id = 3, classe = 'krivak2', categoria = 'nave', flag = 18, posizione = { x = 200, y = 10000 }, quota_m = 0, rotta = 90,"
           "  velocita_kn = 10, " << NAVE << " },\n"
           "{ id = 5, classe = 'cargo', categoria = 'nave', flag = 2, posizione = { x = 60000, y = 60000 }, quota_m = 0, rotta = 90,"
           "  velocita_kn = 0, " << NAVE << " },\n" << altre <<
           "}, timer = {" << (gambe ? " { dopo_s = 300, unita = 0, gestore = 'T' }, { dopo_s = 1200, unita = 0, gestore = 'T2' }" : "")
        << "} } } }\n";
    return f.string();
}

struct Partita {
    std::map<std::string, ClasseNave> classi = carica_classi(M688_DATI "/classi.toml");
    Missione m;
    DanniMissione dm;
    explicit Partita(const std::string& file, std::uint64_t seme = 1) : m(file, seme, false), dm(classi, M688_DATI, seme, RegolaDanni::simulazione) {}
    void avanza(double s) { for (long i = 0; i < static_cast<long>(s * 9.0); ++i) { m.avanza(1.0 / 9.0); dm.avanza(m, 1.0 / 9.0); } }
    void ordina(TipoOrdine t, int n, double a = 0.0, double b = 0.0) { m.applica({m.tempo(), t, a, b, n}); }
};

bool c_e() { return fs::exists(M688_DATI "/missili.toml"); }

// il tiro col MATE «perfetto» sullo Slava: la distanza e il moto veri dall'interfaccia
void prepara_tiro(Partita& p) {
    p.avanza(1.0);
    REQUIRE(p.m.con_missili());
    p.ordina(TipoOrdine::designa_bersaglio, 2);
    p.ordina(TipoOrdine::imposta_missile, 1, 0.0, static_cast<double>(CampoMissile::fonte));
    p.ordina(TipoOrdine::imposta_missile, 1, 14000.0, static_cast<double>(CampoMissile::distanza_m));
    p.ordina(TipoOrdine::imposta_missile, 1, 90.0, static_cast<double>(CampoMissile::rotta));
    p.ordina(TipoOrdine::imposta_missile, 1, 10.0, static_cast<double>(CampoMissile::velocita_kn));
    p.ordina(TipoOrdine::imposta_missile, 1, 1500.0, static_cast<double>(CampoMissile::sigma_distanza_m));
}

}  // namespace

TEST_CASE("missili nella missione: il tiro del tubo, il lancio e il primo eco" * doctest::skip(!c_e())) {
    const std::string f = scena();
    Partita p(f);
    prepara_tiro(p);
    const auto* g = p.m.giocatore();
    REQUIRE(g);
    if (!g->contatti.count(2)) {   // senza il contatto sullo Slava il tiro non ha bersaglio: si aspetta il sonar
        p.avanza(120.0);
        p.ordina(TipoOrdine::designa_bersaglio, 2);
    }
    REQUIRE(p.m.giocatore()->contatti.count(2));
    CHECK_FALSE(p.m.tiro_missile(3).has_value());   // un Mk 48
    const auto tiro = p.m.tiro_missile(1);
    REQUIRE(tiro.has_value());
    CHECK(tiro->dati->id == "harpoon_84c");   // il 1989
    REQUIRE(tiro->bersaglio.has_value());
    CHECK(tiro->bersaglio->distanza_nota);
    CHECK(tiro->aln);                          // il missile caricato prima della missione e' gia' pronto
    CHECK(tiro->piano.valido);
    CHECK(tiro->rdy);
    CHECK(tiro->slide_libera);
    CHECK(tiro->piano.distanza_m == doctest::Approx(14000.0).epsilon(0.05));
    const double t0 = p.m.tempo();
    REQUIRE(p.m.applica({t0, TipoOrdine::lancia, 0, 0, 1}));
    CHECK(p.m.variabili().empty());
    CHECK_FALSE(p.m.slide_libera(3));          // lo stesso lato: 20 s
    p.avanza(5.0);
    CHECK(p.m.slide_libera(2));                // l'altro lato: 4 s
    const auto volo = p.m.missili_in_volo();
    REQUIRE(volo.size() == 1);
    CHECK(volo[0].missile == "harpoon_84c");
    // il lancio per gli effetti (la fiammata al periscopio): una voce sola, coll'arma del modello
    REQUIRE(p.m.lanci_missili().size() == 1);
    CHECK(p.m.lanci_missili()[0].arma == "harpoon_84c");
    CHECK(p.m.lanci_missili()[0].unita == 0);
    CHECK(p.m.lanci_missili()[0].categoria_lanciatore == "sottomarino");
    CHECK(volo[0].tubo == 1);
    // il dopo tubo e' vuoto: il prossimo missile da preparare
    p.avanza(20.0);
    CHECK(p.m.colpi().empty());                // niente ATTACK_RND del gestore a 20 s: il missile vola
    for (int k = 0; k < 60 && !p.m.missili_in_volo().empty(); ++k) p.avanza(5.0);
    CHECK(p.m.missili_in_volo().empty());
    REQUIRE(p.m.colpi().size() == 1);
    MESSAGE("colpita l'unita' ", p.m.colpi()[0].vittima, " a ", p.m.tempo() - t0, " s dal lancio");
    CHECK((p.m.colpi()[0].vittima == 2 || p.m.colpi()[0].vittima == 3));
    // il registro: un lancio del giocatore, del missile del catalogo
    CHECK(p.m.esito_dettagliato().per_arma.at("harpoon_84c").lanci == 1);
    // il tubo dopo il lancio: il missile caricato si prepara da capo
    fs::remove(f);
}

TEST_CASE("missili nella missione: l'aggancio riferito da un'amica che vede il bersaglio, e il missile esaurito nel registro" * doctest::skip(!c_e())) {
    // una Perry (6) a 3 km dallo Slava: lo ha in contatto e riferisce l'aggancio (messaggio 479)
    const std::string f = scena(false, "{ id = 6, classe = 'perry', categoria = 'nave', flag = 10, posizione = { x = 3000, y = 14000 }, quota_m = 0,"
                                       " rotta = 90, velocita_kn = 10, " + NAVE + " },\n");
    const auto ha = [](const Partita& p, int codice) {
        return std::any_of(p.m.messaggi().begin(), p.m.messaggi().end(), [&](const Messaggio& x) { return x.codice == codice; });
    };
    {
        Partita p(f);
        p.avanza(120.0);   // la Perry prende il contatto
        prepara_tiro(p);
        if (!p.m.giocatore()->contatti.count(2)) { p.avanza(120.0); p.ordina(TipoOrdine::designa_bersaglio, 2); }
        REQUIRE(p.m.applica({p.m.tempo(), TipoOrdine::lancia, 0, 0, 1}));
        for (int k = 0; k < 60 && !p.m.missili_in_volo().empty(); ++k) p.avanza(5.0);
        CHECK(p.m.unita().at(6).contatti.count(2));
        CHECK(ha(p, 479));
        CHECK(std::count_if(p.m.messaggi().begin(), p.m.messaggi().end(), [](const Messaggio& x) { return x.codice == 479; }) == 1);
    }
    {   // il piano a 60 km sulla stessa linea: il seeker si accende oltre lo Slava e non trova niente
        Partita p(f);
        prepara_tiro(p);
        if (!p.m.giocatore()->contatti.count(2)) { p.avanza(120.0); p.ordina(TipoOrdine::designa_bersaglio, 2); }
        p.ordina(TipoOrdine::imposta_missile, 1, 60000.0, static_cast<double>(CampoMissile::distanza_m));
        REQUIRE(p.m.applica({p.m.tempo(), TipoOrdine::lancia, 0, 0, 1}));
        for (int k = 0; k < 200 && !p.m.missili_in_volo().empty(); ++k) p.avanza(5.0);
        CHECK(p.m.missili_in_volo().empty());
        const auto& ev = p.m.eventi_missione();
        const auto fine = std::find_if(ev.begin(), ev.end(), [](const EventoMissione& e) { return e.tipo == TipoEvento::fine_missile; });
        MESSAGE("colpi ", p.m.colpi().size(), ", fine nel registro ", fine != ev.end());
        if (p.m.colpi().empty()) {
            REQUIRE(fine != ev.end());
            CHECK(fine->classe_arma == "harpoon_84c");
            CHECK(fine->unita == 0);
            CHECK_FALSE(fine->noto);
            CHECK(fine->valore > 60.0);
            MESSAGE(fine->testo_it);
        }
    }
    fs::remove(f);
}

TEST_CASE("missili nella missione: in esercitazione il missile e' inerte, col colpo d'esercitazione e senza danni (S13)" * doctest::skip(!c_e())) {
    const std::string f = scena(false, "", " esercitazione = true,");
    Partita p(f);
    prepara_tiro(p);
    if (!p.m.giocatore()->contatti.count(2)) { p.avanza(120.0); p.ordina(TipoOrdine::designa_bersaglio, 2); }
    const int prima2 = p.m.unita().at(2).integrita, prima3 = p.m.unita().at(3).integrita;
    REQUIRE(p.m.applica({p.m.tempo(), TipoOrdine::lancia, 0, 0, 1}));
    for (int k = 0; k < 60 && !p.m.missili_in_volo().empty(); ++k) p.avanza(5.0);
    CHECK(p.m.missili_in_volo().empty());
    CHECK(p.m.colpi().empty());   // niente danno
    CHECK(p.m.unita().at(2).integrita == prima2);
    CHECK(p.m.unita().at(3).integrita == prima3);
    REQUIRE(p.m.colpi_esercitazione().size() == 1);
    const auto& c = p.m.colpi_esercitazione().front();
    CHECK((c.vittima == 2 || c.vittima == 3));
    CHECK(c.lanciatore == 0);
    CHECK(c.classe_arma == "harpoon_84c");
    CHECK(std::any_of(p.m.messaggi().begin(), p.m.messaggi().end(), [](const Messaggio& x) { return x.codice == 456; }));
    fs::remove(f);
}

TEST_CASE("missili nella missione: il lancio registra la scatola e il modo, e l'obiettivo della scatola (I11)" * doctest::skip(!c_e())) {
    const std::string f = scena(false, "", " obiettivi = { { id = 'stretta', tipo = 'scatola', massima = 'piccola' },"
                                           " { id = 'larga', tipo = 'scatola', massima = 'grande' } },");
    Partita p(f);
    prepara_tiro(p);
    if (!p.m.giocatore()->contatti.count(2)) { p.avanza(120.0); p.ordina(TipoOrdine::designa_bersaglio, 2); }
    REQUIRE(p.m.applica({p.m.tempo(), TipoOrdine::lancia, 0, 0, 1}));
    p.avanza(5.0);
    const auto& ev = p.m.eventi_missione();
    const auto l = std::find_if(ev.begin(), ev.end(), [](const EventoMissione& e) { return e.tipo == TipoEvento::lancio_giocatore; });
    REQUIRE(l != ev.end());
    CHECK(l->livello == 2);   // la scatola media, di serie
    CHECK(l->codice == 0);    // RBL
    const auto es = p.m.esito_dettagliato();
    REQUIRE(es.obiettivi.size() >= 2);   // con la radio anche quello delle ROE
    CHECK(es.obiettivi[0].stato == Obiettivo::Stato::fallito);    // oltre la piccola
    CHECK(es.obiettivi[1].stato == Obiettivo::Stato::in_corso);   // la grande va bene, fino alla fine
    fs::remove(f);
}

TEST_CASE("missili nella missione: lo stesso seme, lo stesso volo" * doctest::skip(!c_e())) {
    const std::string f = scena();
    const auto gioca = [&] {
        Partita p(f, 7);
        prepara_tiro(p);
        p.avanza(120.0);
        p.ordina(TipoOrdine::designa_bersaglio, 2);
        p.m.applica({p.m.tempo(), TipoOrdine::lancia, 0, 0, 1});
        for (int k = 0; k < 80 && (k < 2 || !p.m.missili_in_volo().empty()); ++k) p.avanza(5.0);
        std::vector<int> v;
        for (const auto& c : p.m.colpi()) v.push_back(c.vittima);
        return v;
    };
    CHECK(gioca() == gioca());
    fs::remove(f);
}

TEST_CASE("missili nella missione: senza piano valido il lancio non parte, e il BOL va col solo rilevamento" * doctest::skip(!c_e())) {
    const std::string f = scena();
    Partita p(f);
    p.avanza(121.0);
    p.ordina(TipoOrdine::designa_bersaglio, 2);
    REQUIRE(p.m.giocatore()->contatti.count(2));
    p.ordina(TipoOrdine::imposta_missile, 1, 3000.0, static_cast<double>(CampoMissile::distanza_m));   // troppo vicino
    auto tiro = p.m.tiro_missile(1);
    REQUIRE(tiro.has_value());
    CHECK_FALSE(tiro->piano.valido);
    CHECK_FALSE(tiro->rdy);
    CHECK_FALSE(p.m.applica({p.m.tempo(), TipoOrdine::lancia, 0, 0, 1}));
    // la fonte radio senza un rapporto del quadro: nessun bersaglio, quindi nessun piano
    p.ordina(TipoOrdine::imposta_missile, 1, static_cast<double>(FonteDati::radio), static_cast<double>(CampoMissile::fonte));
    tiro = p.m.tiro_missile(1);
    REQUIRE(tiro.has_value());
    CHECK_FALSE(tiro->bersaglio.has_value());
    CHECK_FALSE(tiro->piano.valido);
    // la fonte ESM: solo il rilevamento, quindi solo BOL
    p.ordina(TipoOrdine::imposta_missile, 1, -1.0, static_cast<double>(CampoMissile::distanza_m));
    p.ordina(TipoOrdine::imposta_missile, 1, static_cast<double>(FonteDati::esm), static_cast<double>(CampoMissile::fonte));
    tiro = p.m.tiro_missile(1);
    REQUIRE(tiro.has_value());
    if (tiro->bersaglio) {
        CHECK_FALSE(tiro->bersaglio->distanza_nota);
        p.ordina(TipoOrdine::imposta_missile, 1, 1.0, static_cast<double>(CampoMissile::modo));
        CHECK(p.m.tiro_missile(1)->imp.modo == ModoLancio::bol);
    }
    fs::remove(f);
}

// La TMA dell'equipaggio (il fire control party, solo per i missili): senza i quadranti del MATE e senza posizioni da
// vista o radar, dopo due gambe il tiro ha la distanza e i contatti noti entrano nell'area
TEST_CASE("missili nella missione: la TMA dell'equipaggio con le gambe da' la distanza e i contatti noti" * doctest::skip(!c_e())) {
    const std::string f = scena(true);
    Partita p(f);
    p.avanza(35.0 * 60.0);
    p.ordina(TipoOrdine::designa_bersaglio, 3);   // il Krivak a 10 km
    REQUIRE(p.m.giocatore()->contatti.count(3));
    CHECK_FALSE(p.m.giocatore()->contatti.at(3).posizione_nota);   // solo il passivo: il giocatore non ha la posizione
    const auto tiro = p.m.tiro_missile(1);
    REQUIRE(tiro.has_value());
    REQUIRE(tiro->bersaglio.has_value());
    CHECK(tiro->bersaglio->distanza_nota);   // dalla TMA dell'equipaggio
    const Vec2 v = p.m.unita().at(3).pos;
    MESSAGE("TMA dell'equipaggio: il Krivak stimato a ", distanza(tiro->bersaglio->pos, v), " m dal vero; ellisse ",
            tiro->bersaglio->ellisse.maggiore_m, " x ", tiro->bersaglio->ellisse.minore_m, " m; ", tiro->piano.in_area.size(), " contatti nell'area");
    CHECK(tiro->bersaglio->relativo);
    CHECK_FALSE(tiro->piano.in_area.empty());   // i contatti noti dalla TMA: l'area non e' piu' vuota in immersione
    // la bonta' della soluzione e' del solutore (nucleo/tma_ia): qui dopo una sola gamba la distanza puo' essere ancora
    // lontana dalla vera, con un intervallo stretto (un equipaggio «sicuro e sbagliato»); non la si controlla
    fs::remove(f);
}

// La fonte radio: lo Slava a 60 km; una Spruance amica a 8 km da lui lo vede e lo riferisce alla forza, e il rapporto
// arriva alla sala radio del 688 come contatto d'intelligence. Il tiro col rapporto: geografico (l'ellisse di navigazione
// si somma)
TEST_CASE("missili nella missione: la fonte radio col rapporto di un'unita' amica" * doctest::skip(!c_e())) {
    const auto f = fs::temp_directory_path() / "m688_prova_missili_radio.lua";
    std::ofstream(f)
        << "local script = {}\n"
           "script.D = function(u, p)\n  m688.bersaglio(u, p ~= nil and p[1] or 0)\n  m688.salva_modello(u)\nend\n"
           "script.A = function(u)\n  m688.antenna(u, 1)\n  m688.salva_modello(u)\nend\n"
           "return { id = 'PROVA_MISSILI_RADIO', teatro = 'faroe', lato = 'usa', data = '1989-06-15', ora_utc = 12, paesaggio_sonoro = false,"
           " carriera = { situazione = 'guerra' },"
           " tubi = { 'harpoon', 'harpoon', 'mk48', 'mk48', 'esca', 'esca', 'harpoon' }, armi = { harpoon = { gestore = 'D' } },"
           " gestori_giocatore = { designa_bersaglio = 'D' }, script = script, varianti = { { probabilita = 1, unita = {\n"
           "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true, posizione = { x = 0, y = 0 },"
           "  quota_m = 6, rotta = 0, velocita_kn = 4, " << SUB << " },\n"
           "{ id = 2, classe = 'slava', categoria = 'nave', flag = 18, posizione = { x = 0, y = 60000 }, quota_m = 0, rotta = 90,"
           "  velocita_kn = 10, " << NAVE << " },\n"
           "{ id = 4, classe = 'spruance', categoria = 'nave', flag = 10, posizione = { x = 0, y = 52000 }, quota_m = 0, rotta = 90,"
           "  velocita_kn = 10, " << NAVE << " },\n"
           "}, timer = { { dopo_s = 1, unita = 0, gestore = 'A' } } } } }\n";
    Partita p(f.string());
    p.m.radio_enhanced();   // la sala radio: il giocatore riceve i rapporti della forza come contatti d'intelligence
    bool rapporto = false;
    for (int k = 0; k < 120 && !rapporto; ++k) {
        p.avanza(30.0);
        for (const auto& ci : p.m.radio()->intelligence(p.m.tempo()))
            rapporto = rapporto || std::find(ci.dati.unita.begin(), ci.dati.unita.end(), 2) != ci.dati.unita.end();
    }
    MESSAGE("contatto d'intelligence sullo Slava dopo ", p.m.tempo() / 60.0, " minuti");
    REQUIRE(rapporto);
    p.ordina(TipoOrdine::designa_bersaglio, 2);
    p.ordina(TipoOrdine::imposta_missile, 1, static_cast<double>(FonteDati::radio), static_cast<double>(CampoMissile::fonte));
    const auto tiro = p.m.tiro_missile(1);
    REQUIRE(tiro.has_value());
    REQUIRE(tiro->bersaglio.has_value());
    CHECK_FALSE(tiro->bersaglio->relativo);   // geografico
    CHECK(tiro->bersaglio->fonte == FonteDati::radio);
    const Vec2 v = p.m.unita().at(2).pos;
    MESSAGE("rapporto radio: lo Slava a ", distanza(tiro->bersaglio->pos, v), " m dal vero, eta' ", p.m.tempo() - tiro->bersaglio->t_pos,
            " s; piano ", std::string(tiro->piano.valido ? "valido" : "non valido"), ", distanza ", tiro->piano.distanza_m);
    // il rapporto della forza non da' il moto: la posizione e' quella dell'osservazione, e lo Slava a 10 nodi in mezz'ora
    // ne fa 9 km; l'ellisse del piano lo copre (cresce come una nave a 30 nodi)
    CHECK(distanza(tiro->bersaglio->pos, v) < 15000.0);
    CHECK(tiro->piano.valido);
    fs::remove(f);
}

// Il TLAM (docs/21 §11.8): la libreria di bordo della missione, l'ordine d'esecuzione dalla radio (`esecuzione_tlam`),
// I12: l'ordine d'esecuzione col gruppo che non torna (autentico = false) non rende eseguibile la missione
TEST_CASE("missili nella missione: l'ordine d'esecuzione TLAM non autentico non arma la missione (I12)" * doctest::skip(!c_e())) {
    const auto prova = [](bool autentico) {
        const auto f = fs::temp_directory_path() / "m688_prova_missili_tlam_falso.lua";
        std::ofstream(f)
            << "local script = {}\n"
               "script.A = function(u)\n  m688.antenna(u, 1)\n  m688.salva_modello(u)\nend\n"
               "return { id = 'PROVA_MISSILI_TLAM_FALSO', teatro = 'faroe', lato = 'usa', data = '1989-06-15', ora_utc = 12, paesaggio_sonoro = false,"
               " tubi = { 'tlam_c', 'tlam_c', 'mk48', 'mk48', 'esca', 'esca', 'tlam_c' },"
               " tlam = { { id = 'T1', bersaglio = 'PROVA', plp = { x = 0, y = 0 }, landfall = { x = 0, y = 100000 }, tot_s = 3000,"
               " finestra_s = 100000, tratta_terra_m = 20000 } },"
               " radio = { { id = 'ESEGUIRE', dopo_s = 60, precedenza = 'immediate', da = 'COMSUBLANT', oggetto = 'TLAM EXECUTE',"
               " classifica = 'SECRET', esecuzione_tlam = 'T1', autentico = " << (autentico ? "true" : "false")
            << ", testo = { it = 'ESEGUIRE T1', en = 'EXECUTE T1' } } },"
               " script = script, varianti = { { probabilita = 1, unita = {\n"
               "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true, posizione = { x = 0, y = 0 },"
               "  quota_m = 6, rotta = 0, velocita_kn = 4, " << SUB << " },\n"
               "}, timer = { { dopo_s = 1, unita = 0, gestore = 'A' } } } } }\n";
        Partita p(f.string());
        p.m.radio_enhanced();
        p.avanza(5.0);
        p.ordina(TipoOrdine::imposta_missile, 1, 0.0, static_cast<double>(CampoMissile::missione_tlam));
        p.avanza(1200.0);
        const auto tiro = p.m.tiro_missile(1);
        fs::remove(f);
        return std::make_pair(tiro && tiro->ordine_esecuzione, tiro && tiro->ordine_non_autentico);
    };
    CHECK(prova(true) == std::make_pair(true, false));
    CHECK(prova(false) == std::make_pair(false, true));   // la consolle dice «non autenticato», RDY spento
}

// il pannello sulla stima, il lancio con l'esito sulla posizione vera
TEST_CASE("missili nella missione: il TLAM, dalla libreria all'ordine d'esecuzione al lancio" * doctest::skip(!c_e())) {
    const auto f = fs::temp_directory_path() / "m688_prova_missili_tlam.lua";
    std::ofstream(f)
        << "local script = {}\n"
           "script.A = function(u)\n  m688.antenna(u, 1)\n  m688.salva_modello(u)\nend\n"
           "return { id = 'PROVA_MISSILI_TLAM', teatro = 'faroe', lato = 'usa', data = '1989-06-15', ora_utc = 12, paesaggio_sonoro = false,"
           " obiettivi = { { id = 'attacco', tipo = 'tlam', missione = 'T1' }, { id = 'altra', tipo = 'tlam', missione = 'T9' } },"
           " tubi = { 'tlam_c', 'tlam_c', 'mk48', 'mk48', 'esca', 'esca', 'tlam_c' },"
           " tlam = { { id = 'T1', bersaglio = 'PROVA', plp = { x = 0, y = 0 }, landfall = { x = 0, y = 100000 }, tot_s = 3000,"
           " finestra_s = 100000, tratta_terra_m = 20000 } },"
           " radio = { { id = 'ESEGUIRE', dopo_s = 60, precedenza = 'immediate', da = 'COMSUBLANT', oggetto = 'TLAM EXECUTE',"
           " classifica = 'SECRET', esecuzione_tlam = 'T1', testo = { it = 'ESEGUIRE T1', en = 'EXECUTE T1' } } },"
           " script = script, varianti = { { probabilita = 1, unita = {\n"
           "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true, posizione = { x = 0, y = 0 },"
           "  quota_m = 6, rotta = 0, velocita_kn = 4, " << SUB << " },\n"
           "}, timer = { { dopo_s = 1, unita = 0, gestore = 'A' } } } } }\n";
    Partita p(f.string());
    p.m.radio_enhanced();
    p.avanza(5.0);
    REQUIRE(p.m.libreria_tlam().size() == 1);
    CHECK(p.m.libreria_tlam()[0].id == "T1");
    CHECK(p.m.libreria_tlam()[0].landfall.y == doctest::Approx(100000.0));
    auto tiro = p.m.tiro_missile(1);
    REQUIRE(tiro.has_value());
    CHECK(tiro->dati->id == "tlam_c");
    CHECK(tiro->tlam == nullptr);   // nessuna missione scelta
    CHECK_FALSE(tiro->rdy);
    CHECK_FALSE(p.m.applica({p.m.tempo(), TipoOrdine::lancia, 0, 0, 1}));   // senza missione non parte
    p.ordina(TipoOrdine::imposta_missile, 1, 0.0, static_cast<double>(CampoMissile::missione_tlam));
    tiro = p.m.tiro_missile(1);
    REQUIRE(tiro->tlam);
    CHECK_FALSE(tiro->aln);   // il TLAM non parte gia' pronto: l'ora di preparazione resta vera (decisione 3 di docs/21 §10)
    p.ordina(TipoOrdine::prepara_missile, 1, 1.0);
    p.ordina(TipoOrdine::prepara_missile, 2, 1.0);   // il secondo in parallelo: i minuti degli altri, non il quarto d'ora
    const double prep_primo = p.m.tiro_missile(1)->residuo_s, prep_secondo = p.m.tiro_missile(2)->residuo_s;
    MESSAGE("preparazione TLAM: il primo ", prep_primo, " s, il secondo ", prep_secondo, " s (della fase in corso)");
    tiro = p.m.tiro_missile(1);
    REQUIRE(tiro->stato_tlam.has_value());
    CHECK(tiro->stato_tlam->validita == ValiditaTlam::valida);   // sul punto di lancio
    CHECK_FALSE(tiro->ordine_esecuzione);
    CHECK_FALSE(tiro->rdy);                                       // l'ordine non c'e' ancora
    bool ordine = false;
    for (int k = 0; k < 60 && !ordine; ++k) {
        p.avanza(30.0);
        ordine = p.m.tlam_eseguibile("T1");
    }
    REQUIRE(ordine);
    for (int k = 0; k < 40 && !p.m.tiro_missile(1)->aln; ++k) p.avanza(30.0);   // il quarto d'ora del primo
    CHECK(p.m.tiro_missile(1)->aln);
    MESSAGE("TLAM pronto a ", p.m.tempo(), " s");
    // a quota periscopica con la dinamica l'antenna resta su (la tolleranza di 1 m sui 20 ft): la radio dei TLAM
    MESSAGE("688 a ", p.m.giocatore()->quota, " m, antenna ", std::string((p.m.giocatore()->flag54 & 4) ? "su" : "giu'"));
    CHECK((p.m.giocatore()->flag54 & 4) != 0);
    tiro = p.m.tiro_missile(1);
    CHECK(tiro->ordine_esecuzione);
    CHECK(tiro->rdy);
    REQUIRE(p.m.applica({p.m.tempo(), TipoOrdine::lancia, 0, 0, 1}));
    REQUIRE(p.m.lanci_tlam().size() == 1);
    const auto& l = p.m.lanci_tlam().front();
    REQUIRE_FALSE(p.m.lanci_missili().empty());
    CHECK(p.m.lanci_missili().back().arma == "tlam_c");
    MESSAGE("TLAM T1: pannello ", std::string(nome_validita(l.pannello.validita)), " P(landfall) ", l.pannello.p_landfall, "; vero ",
            std::string(nome_validita(l.vero.validita)),
            " P ", l.vero.p_landfall, "; esito ", std::string(l.esito.arrivato ? "arrivato" : "perso"), " a ", l.esito.t_arrivo, " s");
    CHECK(l.missione == "T1");
    CHECK(l.esito.t_arrivo > l.t_lancio);
    CHECK(p.m.esito_dettagliato().per_arma.at("tlam_c").lanci == 1);
    // all'ora d'arrivo la verita' nel registro, per il debriefing
    p.avanza(l.esito.t_arrivo - p.m.tempo() + 2.0);
    const auto& reg = p.m.eventi_missione();
    const auto e = std::find_if(reg.begin(), reg.end(), [](const EventoMissione& x) { return x.tipo == TipoEvento::esito_tlam; });
    REQUIRE(e != reg.end());
    CHECK(e->classe_arma == "T1");
    CHECK(e->classe_creduta == "PROVA");
    CHECK(e->giusta == l.esito.arrivato);
    CHECK(std::string(nome_evento(TipoEvento::esito_tlam)) == "esito_tlam");
    // I12: l'obiettivo sull'esito, la missione T1 nella finestra (tot 3000 s, finestra larga); un'altra missione no
    CHECK(e->codice == 1);
    const auto es = p.m.esito_dettagliato();
    REQUIRE(es.obiettivi.size() >= 2);   // con la radio anche quello delle ROE
    if (l.esito.arrivato) CHECK(es.obiettivi[0].stato == Obiettivo::Stato::raggiunto);
    CHECK(es.obiettivi[1].stato == Obiettivo::Stato::in_corso);
    fs::remove(f);
}

// I missili dell'IA (docs/21 §7, simmetria): il MISSILE dello script di una Spruance sul Krivak a 25 km diventa un
// Harpoon del modello, col piano sulla posizione che la Spruance ha dal radar; l'ATTACK_RND che lo script programma
// 20 s dopo non fa niente
TEST_CASE("missili nella missione: il MISSILE dello script dell'IA vola col modello" * doctest::skip(!c_e())) {
    const auto f = fs::temp_directory_path() / "m688_prova_missili_ia.lua";
    std::ofstream(f)
        << "local script = {}\n"
           "script.M = function(u)\n  m688.bersaglio(u, 3)\n  m688.salva_modello(u)\n  m688.lancio_missile(u)\n  m688.dopo(u, 20, 'R')\nend\n"
           "script.R = function(u)\n  m688.attacco_casuale(u, 75)\nend\n"
           "return { id = 'PROVA_MISSILI_IA', teatro = 'faroe', lato = 'usa', data = '1989-06-15', ora_utc = 12, paesaggio_sonoro = false,"
           " carriera = { situazione = 'guerra' }, script = script, varianti = { { probabilita = 1, unita = {\n"
           "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true, posizione = { x = -40000, y = 0 },"
           "  quota_m = 100, rotta = 0, velocita_kn = 4, " << SUB << " },\n"
           "{ id = 2, classe = 'spruance', categoria = 'nave', flag = 10, posizione = { x = 0, y = 0 }, quota_m = 0, rotta = 0,"
           "  velocita_kn = 12, " << NAVE << " },\n"
           "{ id = 3, classe = 'krivak2', categoria = 'nave', flag = 18, posizione = { x = 0, y = 25000 }, quota_m = 0, rotta = 90,"
           "  velocita_kn = 12, " << NAVE << " },\n"
           "}, timer = { { dopo_s = 120, unita = 2, gestore = 'M' } } } } }\n";
    Partita p(f.string());
    p.avanza(121.0);
    REQUIRE(p.m.con_missili());
    int missile = -1;
    for (const auto& [id, u] : p.m.unita())
        if (u.categoria == "arma" && u.lanciatore == 2 && u.classe.rfind("harpoon", 0) == 0) missile = id;
    REQUIRE(missile >= 0);
    MESSAGE("Harpoon dell'IA: ", p.m.unita().at(missile).classe);
    // lo script MISSILE e il modello: una voce sola per gli effetti, della nave, coll'arma del modello
    const auto n = std::count_if(p.m.lanci_missili().begin(), p.m.lanci_missili().end(), [](const LancioMissile& x) { return x.unita == 2; });
    CHECK(n == 1);
    CHECK(p.m.lanci_missili().back().arma == p.m.unita().at(missile).classe);
    CHECK(p.m.lanci_missili().back().categoria_lanciatore == "nave");
    CHECK(p.m.missili_in_volo().empty());   // solo i missili del giocatore
    p.avanza(25.0);
    CHECK(p.m.colpi().empty());             // niente ATTACK_RND dello script a 20 s
    for (int k = 0; k < 40 && p.m.unita().count(missile) && !p.m.unita().at(missile).persa; ++k) p.avanza(5.0);
    MESSAGE("colpi dopo il volo: ", p.m.colpi().size(), p.m.colpi().empty() ? "" : (" sull'unita' " + std::to_string(p.m.colpi()[0].vittima)));
    for (const auto& c : p.m.colpi()) CHECK(c.vittima == 3);   // il Krivak, se lo prende
    fs::remove(f);
}

// I11 «Il primo eco»: lo Slava che ha il 688 in superficie al radar lo attacca col P-500 del modello, dal MISSILE della
// libreria. Un antinave vede solo la superficie: il 688 immerso non e' un bersaglio (lo Slava usa le armi antisom)
TEST_CASE("missili nella missione: lo Slava tira il P-500 al 688 in superficie (I11)" * doctest::skip(!c_e())) {
    const auto prova = [](double quota_688) {
        const auto f = fs::temp_directory_path() / "m688_prova_missili_slava.lua";
        std::ofstream(f)
            << "local script = {}\n"
               "script.M = function(u)\n  m688.bersaglio(u, 0)\n  m688.salva_modello(u)\n  m688.lancio_missile(u)\nend\n"
               "return { id = 'PROVA_MISSILI_SLAVA', teatro = 'faroe', lato = 'usa', data = '1989-06-15', ora_utc = 12, paesaggio_sonoro = false,"
               " carriera = { situazione = 'guerra' }, script = script, varianti = { { probabilita = 1, unita = {\n"
               "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true, posizione = { x = 0, y = 30000 },"
               "  quota_m = " << quota_688 << ", rotta = 90, velocita_kn = 4, " << SUB << " },\n"
               "{ id = 2, classe = 'slava', categoria = 'nave', flag = 18, posizione = { x = 0, y = 0 }, quota_m = 0, rotta = 0,"
               "  velocita_kn = 12, " << NAVE << " },\n"
               "}, timer = { { dopo_s = 120, unita = 2, gestore = 'M' } } } } }\n";
        Partita p(f.string());
        p.avanza(121.0);
        std::string arma;
        for (const auto& [id, u] : p.m.unita())
            if (u.categoria == "arma" && u.lanciatore == 2) arma = u.classe;
        fs::remove(f);
        return arma;
    };
    const std::string in_superficie = prova(0.0);
    MESSAGE("lo Slava sul 688 in superficie: ", in_superficie.empty() ? std::string("niente") : in_superficie);
    CHECK(in_superficie.rfind("p500", 0) == 0);
}

// Il tiro antinave dell'IA senza la regola (missione_missili.cpp, passo_missili_ia), sulla simmetria: il bersaglio
// classificato, la portata del missile, la salva della classe (missili.toml). Il blocco `editor` dice che non e' una
// missione originale; le altre unita' sono il traffico di prova
namespace {

struct LanciIa { int n = 0, bersaglio = -1; std::string arma; double t = -1.0, quota = 0.0; };

LanciIa lanci_di(const Missione& m, int unita) {
    LanciIa r;
    for (const auto& l : m.lanci_missili())
        if (l.unita == unita) {
            if (r.n++ == 0) { r.t = l.tempo; r.quota = l.quota; }
            r.bersaglio = l.bersaglio;
            r.arma = l.arma;
        }
    return r;
}

std::string scena_ia(const std::string& situazione, const std::string& unita, double quota_688 = 150.0, const std::string& chiavi = "") {
    const auto f = fs::temp_directory_path() / "m688_prova_missili_ia_da_se.lua";
    std::ofstream(f) << "return { id = 'PROVA_MISSILI_IA_DA_SE', teatro = 'faroe', lato = 'usa', data = '1989-06-15', ora_utc = 12,"
                        " paesaggio_sonoro = false, editor = {}, carriera = { situazione = '" << situazione << "' }, script = {}," << chiavi <<
                        " varianti = { { probabilita = 1, unita = {\n"
                        "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true, posizione = { x = -90000, y = -90000 },"
                        "  quota_m = " << quota_688 << ", rotta = 90, velocita_kn = 4, " << SUB << " },\n" << unita << "}, timer = {} } } }\n";
    return f.string();
}

}  // namespace

// la Perry vede il Victor emerso a 8 km al radar, la vedetta lo identifica a vista e dopo un minuto gli tira un Harpoon
// (uno: e' un sottomarino), se intanto il Victor non si e' immerso. Se lo prende per un Los Angeles (l'altra classe di sottomarino in mare) non tira: l'errore
// del libretto vale anche per l'IA. Mai in pace, mai sul Victor immerso
TEST_CASE("missili nella missione: la Perry tira da se' l'Harpoon al Victor emerso che ha identificato" * doctest::skip(!c_e())) {
    const auto prova = [](const std::string& situazione, double quota_victor, std::uint64_t seme) {
        const std::string f = scena_ia(situazione,
            "{ id = 4, classe = 'perry', categoria = 'nave', flag = 9, posizione = { x = 0, y = 0 }, quota_m = 0, rotta = 0, velocita_kn = 12, " + NAVE + " },\n"
            "{ id = 6, classe = 'victor3', categoria = 'sottomarino', flag = 17, posizione = { x = 8000, y = 0 }, quota_m = " + std::to_string(quota_victor) +
            ", rotta = 0, velocita_kn = 6, " + SUB + " },\n");
        Partita p(f, seme);
        p.avanza(420.0);   // anche un'identificazione giusta arrivata dopo una sbagliata (si rivede da piu' vicino o dopo 5 minuti)
        const LanciIa l = lanci_di(p.m, 4);
        const auto* v = p.m.identificazione_visiva(4, 6);
        const std::string classe = v ? v->classe : std::string();
        fs::remove(f);
        return std::pair<LanciIa, std::string>{l, classe};
    };
    int giuste = 0, tirate = 0;
    for (std::uint64_t seme = 1; seme <= 8; ++seme) {
        const auto [l, classe] = prova("guerra", 0.0, seme);
        CHECK_FALSE(classe.empty());   // la vedetta guarda dove dice il radar
        if (classe != "victor3") { CHECK(l.n == 0); continue; }   // preso per un amico
        ++giuste;
        if (l.n == 0) continue;
        ++tirate;
        CHECK(l.n == 1);
        CHECK(l.bersaglio == 6);
        CHECK(l.arma.rfind("harpoon", 0) == 0);
        CHECK(l.t >= 60.0);   // la reazione
    }
    MESSAGE("la Perry sul Victor emerso: identificato giusto in ", giuste, " semi su 8, Harpoon in ", tirate);
    CHECK(giuste >= 5);
    CHECK(tirate >= 4);   // non sempre: il Victor che si vede scoperto puo' immergersi prima del lancio, e allora non e' piu' un bersaglio
    CHECK(prova("pace", 0.0, 1).first.n == 0);
    CHECK(prova("guerra", 120.0, 1).first.n == 0);   // immerso: non e' un bersaglio dell'antinave
}

// lo Slava ha il 688 emerso a 30 km solo al radar, troppo lontano per la vedetta e senza ESM (le navi non l'hanno nei
// dati): un'eco senza classe. La traccia tenuta 5 minuti non basta per un'eco lenta a rotta costante, che potrebbe
// essere un mercantile; basta nella zona di guerra dichiarata dalla missione, e allora il P-500 (uno, sul sottomarino)
TEST_CASE("missili nella missione: lo Slava non tira su un'eco radar finche' non e' classificata" * doctest::skip(!c_e())) {
    const auto prova = [](const std::string& chiavi) {
        const std::string f = scena_ia("guerra",
            "{ id = 2, classe = 'slava', categoria = 'nave', flag = 18, posizione = { x = -90000, y = -60000 }, quota_m = 0, rotta = 0, velocita_kn = 12, " + NAVE + " },\n",
            0.0, chiavi);
        Partita p(f);
        p.avanza(240.0);
        CHECK(p.m.unita().at(2).contatti.count(0));   // al radar
        CHECK(lanci_di(p.m, 2).n == 0);
        p.avanza(360.0);
        const LanciIa l = lanci_di(p.m, 2);
        fs::remove(f);
        return l;
    };
    CHECK(prova("").n == 0);   // un'eco lenta a rotta costante: non si sa che cosa sia
    const LanciIa l = prova(" zona_di_guerra = true,");
    MESSAGE("lo Slava nella zona di guerra: ", l.n, " ", l.arma, ", il primo a ", l.t, " s");
    CHECK(l.n >= 1);   // uno per salva (e' un sottomarino); dopo l'esito e la valutazione, di nuovo
    CHECK(l.t >= 300.0);
    CHECK(l.bersaglio == 0);
}

// la traccia tenuta di un mercantile sovietico a 12 nodi a rotta costante non e' classificata: la Perry non tira
// (decisione dell'utente, 29/10/2026: niente colpi sui mercantili non identificati)
TEST_CASE("missili nella missione: la Perry non tira sul mercantile non identificato" * doctest::skip(!c_e())) {
    const std::string f = scena_ia("guerra",
        "{ id = 4, classe = 'perry', categoria = 'nave', flag = 9, posizione = { x = 0, y = 0 }, quota_m = 0, rotta = 0, velocita_kn = 12, " + NAVE + " },\n"
        "{ id = 8, classe = 'cargo', categoria = 'nave', flag = 18, posizione = { x = 30000, y = 0 }, quota_m = 0, rotta = 0, velocita_kn = 12, " + NAVE + " },\n");
    Partita p(f);
    p.avanza(600.0);
    CHECK(p.m.unita().at(4).contatti.count(8));
    CHECK(lanci_di(p.m, 4).n == 0);
    fs::remove(f);
}

// la salva della classe: lo Slava segue la Perry a 30 km al radar; a 20 nodi si comporta da nave militare, e dopo 5
// minuti e' classificata. Le tira 4 P-500 (missili.toml, salva = 4), poi aspetta l'esito
TEST_CASE("missili nella missione: lo Slava tira sulla Perry la salva della classe" * doctest::skip(!c_e())) {
    const std::string f = scena_ia("guerra",
        "{ id = 2, classe = 'slava', categoria = 'nave', flag = 18, posizione = { x = 0, y = 0 }, quota_m = 0, rotta = 0, velocita_kn = 12, " + NAVE + " },\n"
        "{ id = 4, classe = 'perry', categoria = 'nave', flag = 9, posizione = { x = 30000, y = 0 }, quota_m = 0, rotta = 0, velocita_kn = 20, " + NAVE + " },\n");
    Partita p(f);
    p.avanza(420.0);
    const LanciIa l = lanci_di(p.m, 2);
    MESSAGE("lo Slava sulla Perry: ", l.n, " ", l.arma, ", il primo a ", l.t, " s");
    CHECK(l.n == 4);
    CHECK(l.bersaglio == 4);
    CHECK(l.arma.rfind("p500", 0) == 0);
    fs::remove(f);
}

// il Charlie II a 120 m e 10 nodi tiene la traccia della Perry: la classifica, ne fa la TMA (la soluzione onesta arriva
// dopo una ventina di minuti, con la Perry che si allontana a 25 nodi), sale alla quota di lancio del P-120 (sotto i
// 50 m) rallentando sotto i 5 nodi, e tira la salva della classe (2)
TEST_CASE("missili nella missione: il Charlie II sale alla quota di lancio e tira la salva di P-120" * doctest::skip(!c_e())) {
    const std::string f = scena_ia("guerra",
        "{ id = 4, classe = 'perry', categoria = 'nave', flag = 9, posizione = { x = 0, y = 11500 }, quota_m = 0, rotta = 90, velocita_kn = 25, " + NAVE + " },\n"
        "{ id = 7, classe = 'charlie2', categoria = 'sottomarino', flag = 17, posizione = { x = 0, y = 0 }, quota_m = 120, rotta = 0, velocita_kn = 10, " + SUB + " },\n");
    Partita p(f);
    LanciIa l;
    double velocita_al_lancio = -1.0;
    for (int k = 0; k < 360 && l.n < 2; ++k) {   // 30 minuti
        p.avanza(5.0);
        l = lanci_di(p.m, 7);
        if (l.n >= 1 && velocita_al_lancio < 0.0) velocita_al_lancio = std::fabs(p.m.unita().at(7).velocita);
    }
    MESSAGE("il Charlie II: ", l.n, " ", l.arma, ", il primo a ", l.t, " s da ", l.quota, " m, a ", velocita_al_lancio, " nodi");
    REQUIRE(l.n == 2);
    CHECK(l.arma.rfind("p120", 0) == 0);
    CHECK(l.bersaglio == 4);
    CHECK(l.quota <= 50.0);
    CHECK(velocita_al_lancio <= 5.0);
    fs::remove(f);
}

// Il MISSILE di uno script dell'IA col piano non valido (qui senza contatto sul 688 immerso): nell'Enhanced il missile
// del modello non parte, e con lui niente voce dei lanci, niente avviso al giocatore e niente ATTACK_RND dopo. Una
// classe senza missile del modello (il Krivak) tiene il lancio dello script di prima
TEST_CASE("missili nella missione: il MISSILE dello script col piano non valido non parte" * doctest::skip(!c_e())) {
    const auto prova = [](const std::string& classe) {
        const auto f = fs::temp_directory_path() / "m688_prova_missili_negato.lua";
        std::ofstream(f)
            << "local script = {}\n"
               "script.M = function(u)\n  m688.bersaglio(u, 0)\n  m688.salva_modello(u)\n  m688.lancio_missile(u)\n"
               "  m688.seleziona_bersaglio(u)\n  m688.attacco_casuale(u, 75)\nend\n"
               "return { id = 'PROVA_MISSILI_NEGATO', teatro = 'faroe', lato = 'usa', data = '1989-06-15', ora_utc = 12, paesaggio_sonoro = false,"
               " carriera = { situazione = 'guerra' }, script = script, varianti = { { probabilita = 1, unita = {\n"
               "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true, posizione = { x = 0, y = 30000 },"
               "  quota_m = 150, rotta = 90, velocita_kn = 4, " << SUB << " },\n"
               "{ id = 2, classe = '" << classe << "', categoria = 'nave', flag = 18, posizione = { x = 0, y = 0 }, quota_m = 0, rotta = 0,"
               "  velocita_kn = 12, " << NAVE << " },\n"
               "}, timer = { { dopo_s = 120, unita = 2, gestore = 'M' } } } } }\n";
        Partita p(f.string());
        p.avanza(125.0);
        const auto lanci = std::count_if(p.m.lanci_missili().begin(), p.m.lanci_missili().end(), [](const LancioMissile& l) { return l.unita == 2; });
        const bool avviso = std::any_of(p.m.messaggi().begin(), p.m.messaggi().end(), [](const Messaggio& m) { return m.codice == 0x30; });
        fs::remove(f);
        return std::pair<long, bool>{lanci, avviso};
    };
    const auto slava = prova("slava");
    CHECK(slava.first == 0);
    CHECK_FALSE(slava.second);
    const auto krivak = prova("krivak2");
    CHECK(krivak.first == 1);   // senza missile del modello: il lancio dello script di prima
}

// T13 del kit: il missile fermato dalla difesa della nave non sparisce piu' in silenzio. Il registro ha fine_missile col
// codice 2: la nave, lo scoppio in aria prima di lei, la quota e l'esplosivo (Effetti::esplosione_aerea nell'app)
TEST_CASE("missili nella missione: la difesa piena di una classe fuori tabella ferma tutti i P-500, e ogni missile fermato e' un evento con lo scoppio" * doctest::skip(!c_e())) {
    const std::string difesa = "modello = { massima_kn = 30, virata_gradi_s = 1, accel_kn_s = 0.5, quota_m_s = 0, quota_max_m = 0, integrita = 100, rumore = 14, sonar = 0, difesa = 255 }";
    const std::string f = scena_ia("guerra",
        "{ id = 2, classe = 'slava', categoria = 'nave', flag = 18, posizione = { x = 0, y = 0 }, quota_m = 0, rotta = 0, velocita_kn = 12, " + NAVE + " },\n"
        "{ id = 4, classe = 'ugra', categoria = 'nave', flag = 9, posizione = { x = 30000, y = 0 }, quota_m = 0, rotta = 0, velocita_kn = 20, " + difesa + " },\n");
    Partita p(f);
    std::map<int, double> distanza_allora;   // id dell'evento -> distanza dal bersaglio nel passo in cui arriva
    for (int k = 0; k < 900; ++k) {
        p.avanza(1.0);
        for (const auto& e : p.m.eventi_missione())
            if (e.tipo == TipoEvento::fine_missile && e.codice == 2 && !distanza_allora.count(e.id))
                distanza_allora[e.id] = distanza(e.pos, p.m.unita().at(4).pos);
    }
    const LanciIa l = lanci_di(p.m, 2);
    REQUIRE(l.n >= 1);
    const DatiMissile* dm = p.m.catalogo_missili()->missile("p500");
    REQUIRE(dm != nullptr);
    int fermati = 0;
    for (const auto& e : p.m.eventi_missione()) {
        if (e.tipo != TipoEvento::fine_missile || e.codice != 2) continue;
        ++fermati;
        CHECK(e.unita == 2);
        CHECK(e.altra == 4);
        CHECK(e.quota >= 3.0);
        CHECK(e.valore == doctest::Approx(dm->esplosivo_kg));
        CHECK_FALSE(e.noto);
        CHECK(e.testo_it.find("abbattuto") != std::string::npos);
        const double d = distanza_allora.at(e.id);
        CHECK(d > 200.0);   // prima della nave, non sopra (fa 20 nodi: in un secondo 10 m)
        CHECK(d < 1000.0);
    }
    MESSAGE("lo Slava tira ", l.n, " ", l.arma, ", la nave ne ferma ", fermati);
    CHECK(fermati >= 1);
    for (const auto& c : p.m.colpi()) CHECK(c.vittima != 4);   // con la difesa piena nessuno passa
    fs::remove(f);
}

// Le difese delle navi nella missione (nucleo/difese_navi, le frequenze sono in prove_difese_navi.cpp). Una Koni ha un canale e
// nessun Phalanx: col byte 255 il missile con il canale libero si ferma (tranne il tiro 255), gli altri passano.
namespace {

// la scena del tiro con un solo bersaglio: l'unita' 2 al posto dello Slava, col suo byte, e senza il Krivak
std::string scena_bersaglio(const std::string& classe, const std::string& categoria, int difesa, const std::string& chiavi = "") {
    const std::string f = scena(false, "", chiavi);
    std::string s;
    {
        std::ifstream in(f);
        s.assign(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>());
    }
    const auto id3 = s.find("{ id = 3,");
    s.erase(id3, s.find("{ id = 5,") - id3);
    const auto id2 = s.find("{ id = 2,");
    const std::string vecchia = "classe = 'slava', categoria = 'nave'", modello = "sonar = 0 }";
    s.replace(s.find(vecchia, id2), vecchia.size(), "classe = '" + classe + "', categoria = '" + categoria + "'");
    s.replace(s.find(modello, id2), modello.size(), "sonar = 0, difesa = " + std::to_string(difesa) + " }");
    std::ofstream(f) << s;
    return f;
}

struct Esiti { int fermati = 0, colpi = 0; };
Esiti esiti_su(const Missione& m, int nave) {
    Esiti e;
    for (const auto& x : m.eventi_missione())
        if (x.tipo == TipoEvento::fine_missile && x.codice == 2 && x.altra == nave) ++e.fermati;
    for (const auto& c : m.colpi())
        if (c.vittima == nave) ++e.colpi;
    return e;
}

// il 688 tira i tubi dati, sul bersaglio 2, e aspetta che i missili finiscano
void tira(Partita& p, const std::vector<int>& tubi) {
    prepara_tiro(p);
    if (!p.m.giocatore()->contatti.count(2)) { p.avanza(120.0); p.ordina(TipoOrdine::designa_bersaglio, 2); }
    for (const int t : tubi) {
        if (t != 1) {
            p.ordina(TipoOrdine::imposta_missile, t, 0.0, static_cast<double>(CampoMissile::fonte));
            p.ordina(TipoOrdine::imposta_missile, t, 14000.0, static_cast<double>(CampoMissile::distanza_m));
            p.ordina(TipoOrdine::imposta_missile, t, 90.0, static_cast<double>(CampoMissile::rotta));
            p.ordina(TipoOrdine::imposta_missile, t, 10.0, static_cast<double>(CampoMissile::velocita_kn));
            p.ordina(TipoOrdine::imposta_missile, t, 1500.0, static_cast<double>(CampoMissile::sigma_distanza_m));
        }
        REQUIRE(p.m.applica({p.m.tempo(), TipoOrdine::lancia, 0, 0, t}));
        p.avanza(5.0);
    }
}
void fine_volo(Partita& p) {
    for (int k = 0; k < 60 && !p.m.missili_in_volo().empty(); ++k) p.avanza(2.0);
    CHECK(p.m.missili_in_volo().empty());
}

const std::string CRISI = " editor = {}, carriera = { situazione = 'crisi' },", GUERRA = " editor = {}, carriera = { situazione = 'guerra' },";

}  // namespace

TEST_CASE("difese delle navi nella missione: la salva satura il canale allo stesso modo per il giocatore e per l'IA" * doctest::skip(!c_e())) {
    {   // il giocatore: due Harpoon a 5 s
        const std::string f = scena_bersaglio("koni", "nave", 255, GUERRA);
        Partita p(f);
        tira(p, {1, 2});
        fine_volo(p);
        const Esiti e = esiti_su(p.m, 2);
        MESSAGE("il giocatore sulla Koni: ", e.fermati, " fermati, ", e.colpi, " a segno");
        CHECK(e.fermati == 1);
        CHECK(e.colpi == 1);
        fs::remove(f);
    }
    {   // l'IA: la Perry tira la sua salva di 2 Harpoon sulla stessa Koni
        const std::string difesa = "modello = { massima_kn = 30, virata_gradi_s = 1, accel_kn_s = 0.5, quota_m_s = 0, quota_max_m = 0, integrita = 100, rumore = 14, sonar = 0, difesa = 255 }";
        const std::string f = scena_ia("guerra",
            "{ id = 2, classe = 'koni', categoria = 'nave', flag = 18, posizione = { x = 0, y = 0 }, quota_m = 0, rotta = 0, velocita_kn = 20, " + difesa + " },\n"
            "{ id = 4, classe = 'perry', categoria = 'nave', flag = 9, posizione = { x = 30000, y = 0 }, quota_m = 0, rotta = 0, velocita_kn = 12, " + NAVE + " },\n");
        Partita p(f);
        for (int k = 0; k < 900 && lanci_di(p.m, 4).n < 2; ++k) p.avanza(1.0);
        REQUIRE(lanci_di(p.m, 4).n == 2);
        p.avanza(180.0);   // i 125 s di volo; missili_in_volo e' del giocatore. La salva dopo arriverebbe a 245 s
        const Esiti e = esiti_su(p.m, 2);
        MESSAGE("l'IA sulla Koni: ", e.fermati, " fermati, ", e.colpi, " a segno");
        CHECK(e.fermati == 1);
        CHECK(e.colpi == 1);
        fs::remove(f);
    }
}

TEST_CASE("difese delle navi nella missione: una salva piu' grande dei canali della Perry passa anche col byte 255" * doctest::skip(!c_e())) {
    const std::string difesa = "modello = { massima_kn = 30, virata_gradi_s = 1, accel_kn_s = 0.5, quota_m_s = 0, quota_max_m = 0, integrita = 100, rumore = 14, sonar = 0, difesa = 255 }";
    const std::string f = scena_ia("guerra",
        "{ id = 2, classe = 'slava', categoria = 'nave', flag = 18, posizione = { x = 0, y = 0 }, quota_m = 0, rotta = 0, velocita_kn = 12, " + NAVE + " },\n"
        "{ id = 4, classe = 'perry', categoria = 'nave', flag = 9, posizione = { x = 30000, y = 0 }, quota_m = 0, rotta = 0, velocita_kn = 20, " + difesa + " },\n");
    Partita p(f);
    for (int k = 0; k < 900 && lanci_di(p.m, 2).n < 4; ++k) p.avanza(1.0);
    REQUIRE(lanci_di(p.m, 2).n == 4);
    p.avanza(150.0);   // i 70 s di volo; la salva dopo arriverebbe a 190 s
    const Esiti e = esiti_su(p.m, 4);
    MESSAGE("quattro P-500 sulla Perry: ", e.fermati, " fermati, ", e.colpi, " a segno");
    CHECK(e.fermati >= 1);
    CHECK(e.colpi >= 1);
    fs::remove(f);
}

// la trappola: il lancio porta la forza del bersaglio ad A2 appena parte, ma all'impatto conta il livello che aveva prima (A1,
// fattore 0,75: col byte 255 il 25% passa). Letta all'impatto, l'allerta fermerebbe tutto
TEST_CASE("difese delle navi nella missione: la provocazione del lancio non sveglia il bersaglio, il missile si'" * doctest::skip(!c_e())) {
    int colpi = 0, fermati = 0;
    for (std::uint64_t seme = 1; seme <= 12; ++seme) {
        const std::string f = scena_bersaglio("koni", "nave", 255, CRISI);
        Partita p(f, seme);
        tira(p, {1});
        CAPTURE(seme);
        CHECK(*p.m.allerta(0x10) >= 2);   // la coppia e' gia' in guerra
        fine_volo(p);
        p.avanza(1.0);
        const Esiti e = esiti_su(p.m, 2);
        CHECK(e.fermati + e.colpi <= 1);
        colpi += e.colpi;
        fermati += e.fermati;
        if (e.fermati + e.colpi) CHECK(*p.m.allerta_unita(2) == 3);   // il prossimo missile la trova sveglia
        fs::remove(f);
    }
    MESSAGE("in crisi, 12 Harpoon sulla Koni col byte 255: ", fermati, " fermati, ", colpi, " a segno");
    CHECK(colpi >= 1);
    CHECK(fermati >= colpi);
}

TEST_CASE("difese delle navi nella missione: il missile fermato alza l'allerta, tranne quello d'esercitazione" * doctest::skip(!c_e())) {
    for (const bool esercitazione : {false, true}) {
        const std::string f = scena_bersaglio("koni", "nave", 255, GUERRA + (esercitazione ? " esercitazione = true," : ""));
        Partita p(f);
        tira(p, {1});
        fine_volo(p);
        p.avanza(1.0);
        CAPTURE(esercitazione);
        REQUIRE(esiti_su(p.m, 2).fermati == 1);
        if (esercitazione) CHECK(*p.m.allerta_unita(2) < 3);
        else CHECK(*p.m.allerta_unita(2) == 3);
        fs::remove(f);
    }
}

TEST_CASE("difese delle navi nella missione: un sottomarino in superficie tiene il byte intero, anche in crisi" * doctest::skip(!c_e())) {
    int fermati = 0;
    for (std::uint64_t seme = 1; seme <= 12; ++seme) {
        const std::string f = scena_bersaglio("victor3", "sottomarino", 255, CRISI);
        Partita p(f, seme);
        tira(p, {1});
        fine_volo(p);
        const Esiti e = esiti_su(p.m, 2);
        CAPTURE(seme);
        CHECK(e.colpi == 0);
        fermati += e.fermati;
        fs::remove(f);
    }
    MESSAGE("in crisi, 12 Harpoon sul Victor in superficie col byte 255: ", fermati, " fermati");
    CHECK(fermati >= 6);
}

TEST_CASE("difese delle navi nella missione: a forze spente il byte resta intero" * doctest::skip(!c_e())) {
    for (std::uint64_t seme = 1; seme <= 12; ++seme) {
        const std::string f = scena_bersaglio("koni", "nave", 255, CRISI);
        Partita p(f, seme);
        p.m.usa_forze(false);
        tira(p, {1});
        fine_volo(p);
        CAPTURE(seme);
        CHECK_FALSE(p.m.allerta_unita(2).has_value());
        CHECK(esiti_su(p.m, 2).colpi == 0);
        fs::remove(f);
    }
}

// 0A, seme 2: il comandante a galla sul poco fondo e' stato colpito dalla sua TASM. Gli echi sono la verita' delle navi in
// superficie, e fra loro c'era il lanciatore se e' a 2 m o meno. Qui il 688 sta a galla nell'area di ricerca e le navi
// sono altrove: senza altri echi, il missile finisce il volo senza colpire nessuno
TEST_CASE("missili nella missione: la testa non prende il proprio lanciatore" * doctest::skip(!c_e())) {
    const std::string f = scena();
    Partita p(f);
    prepara_tiro(p);
    if (!p.m.giocatore()->contatti.count(2)) {
        p.avanza(120.0);
        p.ordina(TipoOrdine::designa_bersaglio, 2);
    }
    const auto tiro = p.m.tiro_missile(1);
    REQUIRE(tiro.has_value());
    REQUIRE(tiro->piano.valido);
    const Vec2 dentro = tiro->piano.previsto;
    REQUIRE(p.m.applica({p.m.tempo(), TipoOrdine::lancia, 0, 0, 1}));
    bool partito = false;
    for (int k = 0; k < 600 && (!partito || !p.m.missili_in_volo().empty()); ++k) {
        p.avanza(1.0);
        partito |= !p.m.missili_in_volo().empty();
        p.m.trova(2)->pos = p.m.trova(3)->pos = Vec2{-90000.0, 90000.0};
        auto* g = p.m.trova(0);
        g->pos = dentro;
        g->quota = g->quota_ordinata = 1.5;
    }
    CHECK(partito);
    CHECK(p.m.missili_in_volo().empty());
    for (const auto& c : p.m.colpi()) CHECK(c.vittima != 0);
    CHECK(p.m.trova(0)->integrita > 0);
    fs::remove(f);
}

// S5, seme 7: nel registro l'Harpoon di una Perry finiva sul Los Angeles, dal +62 della vittima. Il colpo e' di chi ha
// lanciato quel missile, con l'arma; il +62 resta com'era per gli script
TEST_CASE("missili nella missione: il colpo e' di chi ha lanciato quel missile" * doctest::skip(!c_e())) {
    const std::string f = scena_ia("guerra",
        "{ id = 2, classe = 'slava', categoria = 'nave', flag = 18, posizione = { x = 0, y = 0 }, quota_m = 0, rotta = 0, velocita_kn = 12, " + NAVE + " },\n"
        "{ id = 4, classe = 'perry', categoria = 'nave', flag = 9, posizione = { x = 30000, y = 0 }, quota_m = 0, rotta = 0, velocita_kn = 20, " + NAVE + " },\n");
    Partita p(f);
    const int collegata = p.m.unita().at(4).lanciatore;
    const auto colpo = [&]() -> const Colpo* {
        for (const auto& c : p.m.colpi())
            if (c.vittima == 4) return &c;
        return nullptr;
    };
    for (int k = 0; k < 600 && !colpo(); ++k) p.avanza(1.0);
    const Colpo* c = colpo();
    REQUIRE(c != nullptr);
    CHECK(c->lanciatore == 2);
    CHECK(c->classe_lanciatore == "slava");
    CHECK(c->arma >= 0);
    CHECK(c->classe_arma.rfind("p500", 0) == 0);
    if (const auto* v = p.m.trova(4)) CHECK(v->lanciatore == collegata);
    fs::remove(f);
}

// dentro il convoglio non si tira: un'amica nella scatola ferma il tiro da se' dell'IA, anche se non e' fra i suoi contatti
// (il radar dell'IA non mette le navi amiche fra i contatti: radar_contro_nave)
TEST_CASE("missili nella missione: con un'amica nella scatola lo Slava non tira" * doctest::skip(!c_e())) {
    const std::string difesa = "modello = { massima_kn = 30, virata_gradi_s = 1, accel_kn_s = 0.5, quota_m_s = 0, quota_max_m = 0, integrita = 100, rumore = 14, sonar = 0, difesa = 255 }";
    const auto prova = [&](double x_amica) {
        const std::string f = scena_ia("guerra",
            "{ id = 2, classe = 'slava', categoria = 'nave', flag = 18, posizione = { x = 0, y = 0 }, quota_m = 0, rotta = 0, velocita_kn = 12, " + difesa + " },\n"
            "{ id = 3, classe = 'krivak2', categoria = 'nave', flag = 18, posizione = { x = " + std::to_string(x_amica) + ", y = 1500 }, quota_m = 0, rotta = 0, velocita_kn = 12, " + difesa + " },\n"
            "{ id = 4, classe = 'perry', categoria = 'nave', flag = 9, posizione = { x = 30000, y = 0 }, quota_m = 0, rotta = 0, velocita_kn = 12, " + NAVE + " },\n");
        Partita p(f);
        p.avanza(420.0);
        const int n = lanci_di(p.m, 2).n;
        fs::remove(f);
        return n;
    };
    CHECK(prova(26000.0) == 0);
    CHECK(prova(-20000.0) > 0);   // la stessa amica fuori dall'area: tira
}

// S5, seme 7: il MISSILE dello script della Perry passava da lancia_missile_ia senza guardare la scatola. L'amica nell'area
// nega il lancio, e niente ATTACK_RND dopo
TEST_CASE("missili nella missione: il MISSILE dello script non parte con un'amica nella scatola" * doctest::skip(!c_e())) {
    const auto prova = [](double x_amica) {
        const auto f = fs::temp_directory_path() / "m688_prova_missili_convoglio.lua";
        std::ofstream(f)
            << "local script = {}\n"
               "script.M = function(u)\n  m688.bersaglio(u, 3)\n  m688.salva_modello(u)\n  m688.lancio_missile(u)\nend\n"
               "return { id = 'PROVA_MISSILI_CONVOGLIO', teatro = 'faroe', lato = 'usa', data = '1989-06-15', ora_utc = 12, paesaggio_sonoro = false,"
               " carriera = { situazione = 'guerra' }, script = script, varianti = { { probabilita = 1, unita = {\n"
               "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true, posizione = { x = -60000, y = 0 },"
               "  quota_m = 100, rotta = 0, velocita_kn = 4, " << SUB << " },\n"
               "{ id = 2, classe = 'spruance', categoria = 'nave', flag = 10, posizione = { x = 0, y = 0 }, quota_m = 0, rotta = 0,"
               "  velocita_kn = 12, " << NAVE << " },\n"
               "{ id = 3, classe = 'krivak2', categoria = 'nave', flag = 18, posizione = { x = 0, y = 25000 }, quota_m = 0, rotta = 90,"
               "  velocita_kn = 12, " << NAVE << " },\n"
               "{ id = 6, classe = 'perry', categoria = 'nave', flag = 10, posizione = { x = " << x_amica << ", y = 21000 }, quota_m = 0, rotta = 0,"
               "  velocita_kn = 12, " << NAVE << " },\n"
               "}, timer = { { dopo_s = 120, unita = 2, gestore = 'M' } } } } }\n";
        Partita p(f.string());
        p.avanza(125.0);
        const int n = lanci_di(p.m, 2).n;
        fs::remove(f);
        return n;
    };
    CHECK(prova(1500.0) == 0);
    CHECK(prova(-20000.0) == 1);
}

// SCENE0A: il bersaglio della missione ha il bit del falso bersaglio (0x80) e non era mai un eco: i TASM lo sorvolavano e
// prendevano il traghetto 28 km dopo. E' un eco solo per il missile che l'ha designato
TEST_CASE("missili nella missione: il falso bersaglio designato e' un eco per il suo missile, e per gli altri no" * doctest::skip(!c_e())) {
    const auto prova = [](int designato) {
        const auto f = fs::temp_directory_path() / "m688_prova_missili_falso.lua";
        std::ofstream(f)
            << "local script = {}\n"
               "script.M = function(u)\n  m688.bersaglio(u, " << designato << ")\n  m688.salva_modello(u)\n  m688.lancio_missile(u)\nend\n"
               "return { id = 'PROVA_MISSILI_FALSO', teatro = 'faroe', lato = 'usa', data = '1989-06-15', ora_utc = 12, paesaggio_sonoro = false,"
               " carriera = { situazione = 'guerra' }, script = script, varianti = { { probabilita = 1, unita = {\n"
               "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true, posizione = { x = -60000, y = 0 },"
               "  quota_m = 100, rotta = 0, velocita_kn = 4, " << SUB << " },\n"
               "{ id = 2, classe = 'spruance', categoria = 'nave', flag = 10, posizione = { x = 0, y = 0 }, quota_m = 0, rotta = 0,"
               "  velocita_kn = 12, " << NAVE << " },\n"
               "{ id = 3, classe = 'krivak2', categoria = 'nave', flag = 18, posizione = { x = 500, y = 25000 }, quota_m = 0, rotta = 0,"
               "  velocita_kn = 0, " << NAVE << " },\n"
               "{ id = 6, classe = 'bqm74', categoria = 'nave', flag = 146, posizione = { x = 0, y = 22000 }, quota_m = 0, rotta = 0,"
               "  velocita_kn = 0, " << NAVE << " },\n"
               "}, timer = { { dopo_s = 300, unita = 2, gestore = 'M' } } } } }\n";
        Partita p(f.string());
        for (int k = 0; k < 900 && p.m.colpi().empty(); ++k) p.avanza(1.0);
        const int n = lanci_di(p.m, 2).n;
        const int vittima = p.m.colpi().empty() ? -1 : p.m.colpi()[0].vittima;
        fs::remove(f);
        return std::pair<int, int>{n, vittima};
    };
    const auto sul_falso = prova(6);
    CHECK(sul_falso.first == 1);
    CHECK(sul_falso.second == 6);
    const auto sulla_nave = prova(3);   // lo stesso falso bersaglio sulla via: il missile lo ignora
    CHECK(sulla_nave.first == 1);
    CHECK(sulla_nave.second == 3);
}

// il giocatore: l'avviso sul pannello per un'amica che ha classificato, e il lancio resta suo. Senza classe l'avviso non
// gli dice di chi e' il contatto
TEST_CASE("missili nella missione: l'amica classificata nell'area e' un avviso, che non blocca il lancio" * doctest::skip(!c_e())) {
    const std::string f = scena(false, "{ id = 6, classe = 'perry', categoria = 'nave', flag = 9, posizione = { x = 500, y = 13000 }, quota_m = 0, rotta = 90,"
                                       "  velocita_kn = 10, " + NAVE + " },\n");
    Partita p(f);
    prepara_tiro(p);
    if (!p.m.giocatore()->contatti.count(2)) { p.avanza(120.0); p.ordina(TipoOrdine::designa_bersaglio, 2); }
    const auto amica = [&] {
        auto& c = p.m.trova(0)->contatti[6];
        c.qualita = 1500.0;
        c.posizione_nota = true;
        c.pos_stimata = p.m.unita().at(6).pos;
        c.t_posizione = c.ultima_scoperta = p.m.tempo();
    };
    const auto ha_avviso = [&](const Missione::TiroMissile& t) {
        return std::find(t.piano.avvisi.begin(), t.piano.avvisi.end(), Avviso::amici_in_area) != t.piano.avvisi.end();
    };
    amica();
    auto tiro = p.m.tiro_missile(1);
    REQUIRE(tiro.has_value());
    REQUIRE(std::find(tiro->piano.in_area.begin(), tiro->piano.in_area.end(), 6) != tiro->piano.in_area.end());
    CHECK_FALSE(ha_avviso(*tiro));
    p.m.classifica_contatto(0, 6, "perry");
    p.avanza(1.0);
    amica();
    tiro = p.m.tiro_missile(1);
    REQUIRE(tiro.has_value());
    CHECK(ha_avviso(*tiro));
    CHECK(tiro->piano.amici_in_area == std::vector<int>{6});
    CHECK(tiro->piano.valido);
    CHECK(p.m.applica({p.m.tempo(), TipoOrdine::lancia, 0, 0, 1}));
    fs::remove(f);
}
