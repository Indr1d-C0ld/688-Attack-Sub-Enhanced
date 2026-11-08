#include <doctest.h>

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <string>
#include <thread>

#include "grafica/scala_cascata.hpp"
#include "missioni/danni_missione.hpp"
#include "missioni/missione.hpp"
#include "nucleo/analisi_acustica.hpp"
#include "nucleo/classi.hpp"
#include "nucleo/fondale.hpp"
#include "nucleo/geometria.hpp"
#include "grafica/etichetta_tasti.hpp"
#include "grafica/font.hpp"
#include "grafica/palette.hpp"
#include "grafica/sale.hpp"
#include "grafica/tema_stazioni.hpp"
#include "grafica/tela.hpp"
#include "sonar_enhanced.hpp"
#include "spettri_fasci.hpp"

using namespace m688;
namespace fs = std::filesystem;

// Stazione Sonar dell'Enhanced (app/sonar_enhanced.hpp; docs/11_comandi_e_stazioni.md §11, S3): storie della cascata
// per rilevamento e del LOFAR, fascio che segue il bersaglio, analizzatore di firma.

namespace {

constexpr double DT = 1.0 / 9.0;

bool ci_sono_dati() { return fs::exists(std::string(M688_DATI) + "/acustica.toml"); }

// il 688 a 150 m e 5 nodi, un Charlie II a 3 km a nord a 8 nodi (un Victor III a 5 nodi resta quasi nel rumore)
std::string scena(const char* classe = "charlie2", double kn = 8.0, int rotta_propria = 0, const char* giocatore = "los_angeles") {
    const auto f = fs::temp_directory_path() / "m688_prova_sonar_enhanced.lua";
    const std::string modello = "modello = { massima_kn = 32, virata_gradi_s = 1, accel_kn_s = 0.5, quota_m_s = 1, quota_max_m = 450, integrita = 200 }";
    std::ofstream(f) << "return { id = 'PROVA', teatro = 'faroe', script = {}, varianti = { { probabilita = 1, unita = {\n"
                        "{ id = 0, classe = '" << giocatore << "', categoria = 'sottomarino', flag = 9, giocatore = true,"
                        "  posizione = { x = 0, y = 0 }, quota_m = 150, rotta = " << rotta_propria << ", velocita_kn = 5, " << modello << " },"
                        "{ id = 2, classe = '" << classe << "', categoria = 'sottomarino', flag = 25, posizione = { x = 0, y = 3000 },"
                        "  quota_m = 150, rotta = 90, velocita_kn = " << kn << ", " << modello << " },"
                        "\n}, timer = {} } } }\n";
    return f.string();
}

struct Partita {
    std::map<std::string, ClasseNave> classi = carica_classi(M688_DATI "/classi.toml");
    Missione m;
    DanniMissione dm;
    SonarEnhanced s;
    Partita(const std::string& file) : m(file, 1, false), dm(classi, M688_DATI, 1, RegolaDanni::simulazione) {
        m.usa_paesaggio(false);   // la scena con le sole unita' della prova, senza le sorgenti ambientali
        m.usa_variazioni_acustiche(false);   // le firme di catalogo: le prove misurano il sonar, non lo scarto delle unita' (R6)
    }
    void avanza(double sec) {
        for (double t = 0; t < sec; t += DT) {
            m.avanza(DT);
            dm.avanza(m, DT);
            s.aggiorna(m, 2);
        }
    }
};

}  // namespace

TEST_CASE("sonar enhanced: cascata e LOFAR sul bersaglio, una riga ogni 2 s" * doctest::skip(!ci_sono_dati())) {
    const auto f = scena();
    Partita p(f);
    p.avanza(20.0);
    REQUIRE(p.m.con_acustica());
    CHECK(p.s.sensore() == "bqq5_sfera");
    CHECK(p.s.btr().size() >= 9);
    CHECK(p.s.btr().size() <= 11);
    CHECK(p.s.btr().size() == p.s.lofar().size());
    // nella cascata il fascio piu' forte e' quello del Charlie, a nord
    const auto& c = p.s.btr().front();
    const auto it = std::max_element(c.db.begin(), c.db.end());
    const double ril = normalizza_360(c.rilevamento0 + c.passo * static_cast<double>(it - c.db.begin()));
    CHECK(std::fabs(normalizza_180(ril)) <= 10.0);
    CHECK(*it > 3.0);
    // il LOFAR segue il rilevamento del contatto e ne vede le righe
    CHECK(std::fabs(normalizza_180(p.s.fascio())) < 10.0);
    const auto& sp = p.s.lofar().front();
    CHECK(*std::max_element(sp.snr_db.begin(), sp.snr_db.end()) > 6.0);
    // il fascio fissato a mano si sposta di 5 gradi e torna sul bersaglio
    const double prima = p.s.fascio();
    p.s.sposta_fascio(+1);
    p.avanza(2.0);
    CHECK(std::fabs(normalizza_180(p.s.fascio() - prima - 5.0)) < 1e-9);
    p.s.segui_bersaglio();
    p.avanza(2.0);
    CHECK(std::fabs(normalizza_180(p.s.fascio())) < 10.0);
    fs::remove(f);
}

TEST_CASE("sonar enhanced: gli spettri su piu' fili sono quelli su uno solo, bit per bit" * doctest::skip(!ci_sono_dati())) {
    const auto stessi = [](const Spettro& a, const Spettro& b) {
        return a.f0_hz == b.f0_hz && a.df_hz == b.df_hz && a.snr_db.size() == b.snr_db.size() && !a.snr_db.empty() &&
               std::memcmp(a.snr_db.data(), b.snr_db.data(), a.snr_db.size() * sizeof(float)) == 0;
    };
    const int nucleo = fili_per_gli_spettri();
    CHECK(nucleo >= 1);
    CHECK(nucleo <= 5);
    if (const unsigned core = std::thread::hardware_concurrency()) CHECK(nucleo <= static_cast<int>(core));
    const auto f = scena();
    Partita p(f);
    p.m.usa_paesaggio(true);   // le sorgenti ambientali, come in partita: e' li' il costo
    const std::vector<FascioChiesto> fasci{{0.0, true}, {340.0, false}, {350.0, false}, {10.0, false}, {20.0, false}};
    for (int giro = 0; giro < 3; ++giro) {
        p.avanza(4.0);
        const auto sc = p.m.scena_acustica(0, "bqq5_sfera");
        REQUIRE(sc.ricevitore.sensore != nullptr);
        CHECK(sc.sorgenti.size() > 1);
        const auto uno = spettri_dei_fasci(sc.sorgenti, sc.ricevitore, sc.ambiente, fasci, 10.0, 410.0, 0.1, 1);
        REQUIRE(uno.size() == fasci.size());
        for (std::size_t i = 0; i < fasci.size(); ++i)
            CHECK(stessi(uno[i], spettro_fascio(sc.sorgenti, sc.ricevitore, sc.ambiente, fasci[i].rilevamento, 10.0, 410.0, 0.1, fasci[i].coerente)));
        for (const int fili : {2, 3, 5, 8}) {   // 8: piu' dei fasci, ne partono 5
            const auto molti = spettri_dei_fasci(sc.sorgenti, sc.ricevitore, sc.ambiente, fasci, 10.0, 410.0, 0.1, fili);
            REQUIRE(molti.size() == fasci.size());
            for (std::size_t i = 0; i < fasci.size(); ++i) CHECK(stessi(uno[i], molti[i]));
        }
    }
    fs::remove(f);
}

TEST_CASE("sonar enhanced: le righe a calendario fisso, coi passi di 1/9 s della missione" * doctest::skip(!ci_sono_dati())) {
    // contando dal passo che la faceva scattare, sei righe su dieci arrivavano dopo 2,11 s: in 200 s ne uscivano 95
    const auto f = scena();
    Partita p(f);
    p.avanza(200.0);
    REQUIRE(p.m.con_acustica());
    CHECK(p.s.btr().size() >= 100);
    CHECK(p.s.btr().size() <= 101);
    fs::remove(f);
}

TEST_CASE("sonar enhanced: l'analizzatore mette il Charlie II fra le prime ipotesi" * doctest::skip(!ci_sono_dati())) {
    const auto f = scena();
    Partita p(f);
    p.avanza(10.0);
    const int msg = p.s.analizza(p.m);
    CHECK((msg == MSG_ANALISI_PRONTA || msg == MSG_ANALISI_INCERTA));
    REQUIRE_FALSE(p.s.ipotesi().empty());
    bool trovato = false;
    for (std::size_t i = 0; i < std::min<std::size_t>(3, p.s.ipotesi().size()); ++i)
        trovato = trovato || p.s.ipotesi()[i].classe == "charlie2";
    CHECK(trovato);
    CHECK((msg == MSG_ANALISI_PRONTA) == p.s.lettura_analisi().nome);
    fs::remove(f);
}

TEST_CASE("sonar enhanced: l'analizzatore da' un nome solo se le righe lo reggono" * doctest::skip(!ci_sono_dati())) {
    const auto lettura = [](const char* classe) {
        const auto f = scena(classe);
        Partita p(f);
        p.avanza(40.0);
        const int msg = p.s.analizza(p.m);
        REQUIRE_FALSE(p.s.confronti().empty());
        const auto l = p.s.lettura_analisi();
        CHECK((msg == MSG_ANALISI_PRONTA) == l.nome);
        fs::remove(f);
        return l;
    };
    // il Victor III a 3 km da' una riga sola: il confronto mette comunque una voce in testa, lo schermo no
    const auto poche = lettura("victor3");
    CHECK_FALSE(poche.nome);
    CHECK(poche.testo == "NESSUN NOME: 1 RIGA NON BASTA");
    const auto tante = lettura("charlie2");
    CHECK(tante.nome);
    CHECK(tante.testo.rfind("CHARLIE2 5/5 RIGHE", 0) == 0);   // senza percentuale: quante righe spiega su quelle lette
}

TEST_CASE("sonar enhanced: senza righe non resta la lettura del fascio di prima" * doctest::skip(!ci_sono_dati())) {
    const auto f = scena();
    Partita p(f);
    p.avanza(40.0);
    p.s.analizza(p.m);
    REQUIRE_FALSE(p.s.confronti().empty());
    p.s.sensore_successivo();   // la schiera ancora a bordo: nessuno spettro, si torna prima del confronto
    p.s.analizza(p.m);
    CHECK(p.s.confronti().empty());
    CHECK(p.s.famiglie().empty());
    CHECK(p.s.lettura_analisi().testo.empty());
    fs::remove(f);
}

TEST_CASE("sonar enhanced: il tracker non dice mai NONSUB, e sotto le soglie non dice niente" * doctest::skip(!ci_sono_dati())) {
    const auto tracker_su = [](const char* classe, const auto& controlla, double kn = 3.0) {
        const auto f = scena(classe, kn);
        Partita p(f);
        p.avanza(40.0);
        p.s.nuovo_tracker();
        p.avanza(40.0);
        p.s.analizza(p.m);
        const auto it = std::find_if(p.s.tracker().begin(), p.s.tracker().end(), [&](const TrackerSonar& x) { return x.numero == p.s.segue_tracker(); });
        REQUIRE(it != p.s.tracker().end());
        controlla(*it, p.s.messaggi());
        fs::remove(f);
    };
    const auto c_e = [](const std::vector<int>& m, int k) { return std::find(m.begin(), m.end(), k) != m.end(); };
    for (const char* nave : {"mercantile", "petroliera"})   // le sue righe bastano: la classe si', la natura no
        for (const double kn : {3.0, 8.0})
            tracker_su(nave, [&](const TrackerSonar& t, const std::vector<int>& m) {
                CHECK(t.classe == nave);
                CHECK(t.passo != PassoClassifica::nonsub);
                CHECK_FALSE(c_e(m, MSG_NONSUB));
            }, kn);
    tracker_su("victor3", [&](const TrackerSonar& t, const std::vector<int>& m) {   // una riga sola: niente
        CHECK(t.classe.empty());
        CHECK(t.passo == PassoClassifica::nessuno);
        for (const int k : {MSG_NONSUB, MSG_POSSUB, MSG_PROBSUB, MSG_CERTSUB}) CHECK_FALSE(c_e(m, k));
    });
    tracker_su("charlie2", [&](const TrackerSonar& t, const std::vector<int>& m) {   // le sue righe bastano: PROBSUB, non di piu'
        CHECK(t.classe == "charlie2");
        CHECK(t.passo == PassoClassifica::probsub);
        CHECK(c_e(m, MSG_PROBSUB));
    });
}

TEST_CASE("sonar enhanced: il passo conta solo le due voci mostrate" * doctest::skip(!ci_sono_dati())) {
    const CatalogoAcustico cat = carica_acustica(std::string(M688_DATI) + "/acustica.toml");
    const auto voce = [](const char* id, int righe) {
        ConfrontoRapporti c;
        c.id = id;
        for (int i = 0; i < righe; ++i) {
            RigaOsservata o{};
            RigaLibreria l{};
            o.hz = l.hz = 50.0 + i;
            c.concordi.push_back({o, l});
        }
        return c;
    };
    using P = PassoClassifica;
    CHECK(passo_da_voci({}, cat) == P::nessuno);
    CHECK(passo_da_voci({voce("charlie2", 3)}, cat) == P::probsub);
    CHECK(passo_da_voci({voce("charlie2", 4), voce("victor3", 3)}, cat) == P::probsub);
    CHECK(passo_da_voci({voce("mercantile", 4), voce("charlie2", 3)}, cat) == P::possub);   // uno dei due, in seconda
    CHECK(passo_da_voci({voce("charlie2", 4), voce("mercantile", 3)}, cat) == P::possub);   // e in prima
    CHECK(passo_da_voci({voce("mercantile", 4), voce("petroliera", 3)}, cat) == P::nessuno);
    // lo stesso sottomarino fuori dalle due mostrate, o sotto le soglie: niente allarme
    CHECK(passo_da_voci({voce("charlie2", 3), voce("mercantile", 5), voce("petroliera", 4)}, cat) == P::nessuno);
    CHECK(passo_da_voci({voce("mercantile", 3), voce("charlie2", 2)}, cat) == P::nessuno);
}

TEST_CASE("sonar enhanced: il sensore cambia e la storia riparte; la schiera non filata non da' righe" * doctest::skip(!ci_sono_dati())) {
    const auto f = scena();
    Partita p(f);
    p.avanza(6.0);
    REQUIRE(p.s.sensori().size() >= 2);
    REQUIRE_FALSE(p.s.btr().empty());
    p.s.sensore_successivo();
    CHECK(p.s.btr().empty());
    CHECK(p.s.sensore() != "bqq5_sfera");
    p.avanza(6.0);
    if (p.s.sensore().find("tb") == 0) CHECK(p.s.btr().empty());   // la schiera rimorchiata e' ancora a bordo
    fs::remove(f);
}

TEST_CASE("sonar enhanced: l'integrazione fa uscire il Victor III silenzioso dalla grana" * doctest::skip(!ci_sono_dati())) {
    const auto f = scena("victor3", 5.0);
    const auto picco_a_nord = [](const RigaCascata& c) {
        const auto it = std::max_element(c.db.begin(), c.db.end());
        return std::fabs(normalizza_180(c.rilevamento0 + c.passo * static_cast<double>(it - c.db.begin()))) <= 10.0;
    };
    int senza = 0, con = 0;
    {
        Partita p(f);
        p.s.commuta_integrazione();   // 4 -> 8
        p.s.commuta_integrazione();   // 8 -> 1
        REQUIRE(p.s.integrazione() == 1);
        p.avanza(80.0);
        for (const auto& c : p.s.btr()) senza += picco_a_nord(c);
    }
    {
        Partita p(f);
        p.s.commuta_integrazione();   // 4 -> 8
        REQUIRE(p.s.integrazione() == 8);
        p.avanza(80.0);
        for (std::size_t i = 0; i + 7 < p.s.btr().size(); ++i) con += picco_a_nord(p.s.btr()[i]);   // righe a regime
    }
    CHECK(con >= 25);          // a regime il Victor e' il fascio piu' forte quasi sempre
    CHECK(con > senza);
    fs::remove(f);
}

TEST_CASE("sonar enhanced: nel modo moderno un tracker automatico prende il Charlie e lo segue" * doctest::skip(!ci_sono_dati())) {
    const auto f = scena();
    Partita p(f);
    p.avanza(40.0);
    REQUIRE_FALSE(p.s.tracker().empty());
    const auto vero = [&] { return rilevamento(p.m.unita().at(0).pos, p.m.unita().at(2).pos); };
    const auto vicino = [&](const TrackerSonar& t) { return std::fabs(normalizza_180(t.rilevamento - vero())) < 6.0; };
    REQUIRE(std::any_of(p.s.tracker().begin(), p.s.tracker().end(), vicino));
    const int numero = std::find_if(p.s.tracker().begin(), p.s.tracker().end(), vicino)->numero;
    const auto annunci = p.s.messaggi();
    CHECK(std::find(annunci.begin(), annunci.end(), MSG_TRACKER_NUOVO) != annunci.end());
    p.avanza(240.0);   // il Charlie va a est: il rilevamento cambia e il tracker lo segue
    const TrackerSonar* t = nullptr;
    for (const auto& x : p.s.tracker()) if (x.numero == numero) t = &x;
    REQUIRE(t != nullptr);
    CHECK_FALSE(t->perso);
    CHECK(std::fabs(normalizza_180(t->rilevamento - vero())) < 6.0);
    CHECK(t->rateo() > 0.0);   // verso est: il rilevamento cresce
    // il rateo del filtro e' quello vero (~4,7 gradi al minuto) entro due gradi al minuto (rilevamento vero un minuto fa
    // e ora); la scena ha le firme di catalogo, senza le variazioni R6 (vedi Partita)
    double prima = 0.0;
    {
        Partita q(f);
        q.avanza(220.0);
        prima = rilevamento(q.m.unita().at(0).pos, q.m.unita().at(2).pos);
    }
    CHECK(std::fabs(t->rateo() - normalizza_180(vero() - prima)) < 2.0);
    CHECK(std::fabs(normalizza_180(t->rilevamento - vero())) < 3.0);
    // un solo tracker sul Charlie: i doppioni dei fasci vicini si uniscono
    CHECK(std::count_if(p.s.tracker().begin(), p.s.tracker().end(), [&](const TrackerSonar& x) { return !x.perso && vicino(x); }) == 1);
    fs::remove(f);
}

TEST_CASE("sonar enhanced: nel modo 1989 niente automatici; T mette un tracker sul fascio e ANALIZZA lo classifica" * doctest::skip(!ci_sono_dati())) {
    const auto f = scena();
    Partita p(f);
    p.s.imposta_modo(ModoSonar::anni_89);
    CHECK(p.s.palette() == PaletteSonar::fosforo);
    p.avanza(20.0);
    CHECK(p.s.tracker().empty());
    p.s.nuovo_tracker();   // sul fascio del LOFAR, che segue il bersaglio
    REQUIRE(p.s.tracker().size() == 1);
    CHECK(p.s.segue_tracker() == p.s.tracker().front().numero);
    const auto m1 = p.s.messaggi();
    CHECK(std::find(m1.begin(), m1.end(), MSG_TRACKER_NUOVO) != m1.end());
    p.avanza(20.0);
    p.s.analizza(p.m);
    const TrackerSonar& t = p.s.tracker().front();
    CHECK((t.passo == PassoClassifica::possub || t.passo == PassoClassifica::probsub));   // il Charlie II e' un sottomarino: il rapporto al comando
    CHECK(t.classe == "charlie2");
    const auto m2 = p.s.messaggi();
    CHECK(std::any_of(m2.begin(), m2.end(), [](int k) { return k == MSG_POSSUB || k == MSG_PROBSUB; }));
    p.s.togli_tracker();
    CHECK(p.s.tracker().empty());
    CHECK(p.s.segue_tracker() == 0);
    fs::remove(f);
}

TEST_CASE("sonar enhanced: scale dei tempi decimate col massimo" * doctest::skip(!ci_sono_dati())) {
    const auto f = scena();
    Partita p(f);
    p.avanza(130.0);   // 65 righe
    const auto n1 = p.s.btr(1).size(), n4 = p.s.btr(4).size(), n16 = p.s.btr(16).size();
    CHECK(n1 >= 64);
    CHECK(n4 == n1 / 4);
    CHECK(n16 == n1 / 16);
    // ogni riga x4 e' il massimo delle sue quattro righe x1 (le piu' vecchie, gia' complete)
    const auto& r4 = p.s.btr(4).back();
    const auto& b1 = p.s.btr(1);
    for (std::size_t i = 0; i < r4.db.size(); ++i) {
        float m = 0.0f;
        for (std::size_t j = b1.size() - 4; j < b1.size(); ++j) m = std::max(m, b1[j].db[i]);
        CHECK(r4.db[i] == doctest::Approx(m));
    }
    fs::remove(f);
}

TEST_CASE("sonar enhanced: presentazione (pavimento, tetto, palette, scala, cursori, zoom)") {
    SonarEnhanced s;
    CHECK(s.palette() == PaletteSonar::viridis);
    Palette pal{};
    s.applica_palette(pal);
    CHECK(pal.rgb(SonarEnhanced::SCALA0) == std::array<std::uint8_t, 3>{0, 0, 0});        // sotto il pavimento: nero
    CHECK(pal.rgb(SonarEnhanced::SCALA0 + SonarEnhanced::LIVELLI - 1)[1] > 200);          // in cima il giallo di viridis
    CHECK(s.colore(-50.0) == SonarEnhanced::SCALA0);
    CHECK(s.colore(+50.0) == SonarEnhanced::SCALA0 + SonarEnhanced::LIVELLI - 1);
    for (int i = 0; i < 30; ++i) s.cambia_valore(+1);   // pavimento: non supera tetto - 3
    CHECK(s.pavimento() == doctest::Approx(s.tetto() - 3.0));
    s.campo_successivo(+1);
    s.campo_successivo(+1);
    CHECK(s.campo() == SonarEnhanced::Campo::palette);
    s.cambia_valore(+1);
    CHECK(s.palette() == PaletteSonar::turbo);
    s.imposta_modo(ModoSonar::anni_89);
    s.cambia_valore(+1);
    CHECK(s.palette() == PaletteSonar::grigio);   // nel 1989 solo fosforo e grigio
    s.campo_successivo(+1);
    s.cambia_valore(+1);
    s.cambia_valore(+1);
    s.cambia_valore(+1);
    CHECK(s.scala_tempi() == 16);
    s.cursore_successivo();
    CHECK(s.cursore() == SonarEnhanced::Cursore::lettura);
    s.muovi_cursore(1000.0, 0);
    CHECK(s.cursore_hz() == doctest::Approx(410.0));
    s.muovi_cursore(-10000.0, 0);
    CHECK(s.cursore_hz() == doctest::Approx(10.0));
    for (int i = 0; i < 6; ++i) s.zoom(+1);
    CHECK(s.zoom() == 16);
}

TEST_CASE("sonar enhanced: libreria a schede, voce sovrapposta col fattore Doppler dell'analisi per rapporti" * doctest::skip(!ci_sono_dati())) {
    const auto f = scena();
    Partita p(f);
    p.avanza(20.0);
    p.s.analizza(p.m);
    REQUIRE_FALSE(p.s.confronti().empty());
    CHECK(p.s.confronti().front().id == "charlie2");
    p.s.commuta_libreria();
    REQUIRE(p.s.libreria_aperta());
    REQUIRE(p.s.voce_scelta());
    CHECK(p.s.voce_scelta()->categoria_scheda == "navale");
    for (int i = 0; i < 60 && p.s.voce_scelta() && p.s.voce_scelta()->id != "charlie2"; ++i) p.s.voce_successiva(+1);
    REQUIRE(p.s.voce_scelta()->id == "charlie2");
    p.s.sovrapponi();
    CHECK_FALSE(p.s.libreria_aperta());
    REQUIRE(p.s.sovrapposta());
    CHECK(p.s.fattore_doppler() == doctest::Approx(p.s.confronti().front().fattore_doppler));
    // con M688_ISTANTANEE=cartella: le immagini della stazione per guardarle (libreria e voce sovrapposta)
    const auto salva = [&](const char* nome) {
        const char* d = std::getenv("M688_ISTANTANEE");
        if (!d) return;
        Tela t;
        const Font fo = Font::carica(M688_DATI "/font/originale.fnt");
        Palette pal = Palette::carica(M688_DATI "/palette/giorno.pal");
        p.s.applica_palette(pal);
        p.s.disegna(t, fo);
        t.salva_bmp(std::string(d) + "/" + nome, pal);
    };
    p.avanza(60.0);
    p.s.cursore_successivo();
    p.s.cursore_successivo();
    p.s.cursore_successivo();   // armonico
    p.s.muovi_cursore(-50.0 + 0.0, 0);
    p.s.annota();
    p.s.salva_slot();
    p.s.confronto_successivo();
    p.s.confronto_successivo();   // sovrapposto
    salva("sonar_sovrapposta.bmp");
    p.s.commuta_libreria();
    salva("sonar_libreria.bmp");
    p.s.commuta_libreria();
    // le altre schede hanno le sorgenti della libreria
    p.s.commuta_libreria();
    p.s.scheda_successiva(+1);
    REQUIRE(p.s.voce_scelta());
    CHECK(p.s.voce_scelta()->categoria_scheda == "biologico");
    CHECK_FALSE(p.s.voce_scelta()->unita_navale);
    p.s.commuta_libreria();
    p.s.togli_sovrapposizione();
    CHECK_FALSE(p.s.sovrapposta());
    fs::remove(f);
}

TEST_CASE("sonar enhanced: annotazioni al cursore e slot col confronto" * doctest::skip(!ci_sono_dati())) {
    const auto f = scena();
    Partita p(f);
    p.avanza(20.0);
    p.s.cursore_successivo();
    p.s.muovi_cursore(100.0, 5);   // 160 Hz, cinque righe fa
    p.s.annota();
    p.s.annota();
    REQUIRE(p.s.annotazioni().size() == 2);
    CHECK(p.s.annotazioni()[0].numero == 1);
    CHECK(p.s.annotazioni()[0].hz == doctest::Approx(160.0));
    CHECK(std::fabs(p.s.annotazioni()[0].tempo - (p.m.tempo() - 10.0)) <= 2.5);   // dall'ultima riga, che ha al piu' 2 s
    p.s.togli_annotazione();
    CHECK(p.s.annotazioni().size() == 1);
    // slot: il primo vuoto, poi il successivo; la differenza con se stesso e' zero
    p.s.salva_slot();
    CHECK(p.s.slot() == 0);
    CHECK(p.s.slots()[0].pieno);
    p.s.salva_slot();
    CHECK(p.s.slot() == 1);
    const auto d = differenza(p.s.slots()[0].spettro, p.s.slots()[0].spettro);
    CHECK(std::all_of(d.snr_db.begin(), d.snr_db.end(), [](float x) { return std::fabs(x) < 1e-5f; }));
    for (int i = 0; i < 3; ++i) p.s.confronto_successivo();
    CHECK(p.s.confronto() == SonarEnhanced::Confronto::differenza);
    p.s.slot_successivo();
    CHECK(p.s.slot() == 2);
    fs::remove(f);
}

// S5d: il verniero agganciato alla riga piu' forte del Charlie II la segue mentre il Doppler la sposta, la curva a S
// trova il CPA (geometria della scena: circa 5,5 minuti dopo l'inizio, a circa 2,5 km), il registratore tiene la storia
// del contatto per il rapporto ACINT e l'ascolto del fascio finisce in un WAV
TEST_CASE("sonar enhanced: verniero, curva a S e CPA, registratore ACINT, ascolto in WAV" * doctest::skip(!ci_sono_dati())) {
    const auto f = scena();
    Partita p(f);
    p.avanza(30.0);
    REQUIRE(!p.s.lofar().empty());
    auto righe = righe_osservate(p.s.lofar().front());
    std::vector<double> proprie;   // la riga del contatto, non una del nostro battello (docs/22, F)
    for (const auto& x : p.s.righe_proprie_nel_fascio()) proprie.push_back(x.hz);
    togli_righe(righe, proprie, 0.2);
    REQUIRE(!righe.empty());
    const auto forte = *std::max_element(righe.begin(), righe.end(), [](const auto& a, const auto& b) { return a.snr_db < b.snr_db; });
    p.s.aggancia_riga();   // senza cursore non si aggancia
    CHECK_FALSE(p.s.riga_misurata());
    p.s.cursore_successivo();
    p.s.muovi_cursore(forte.hz - p.s.cursore_hz(), 0);
    p.s.aggancia_riga();
    REQUIRE(p.s.riga_misurata());
    p.avanza(20.0 * 60.0);
    const auto& r = *p.s.riga_misurata();
    CHECK(r.serie.size() >= 60);
    CHECK(r.ultima.valida);
    CHECK(r.ultima.sigma_hz < 0.05);
    CHECK(r.contatto == 2);
    REQUIRE(r.cpa.valida);
    CHECK(r.cpa.t_cpa == doctest::Approx(30.0 + 330.0).epsilon(0.25));
    CHECK(r.cpa.distanza_cpa_m == doctest::Approx(2540.0).epsilon(0.35));
    if (const char* d = std::getenv("M688_ISTANTANEE")) {   // l'immagine della stazione col riquadro del verniero
        Tela t;
        const Font fo = Font::carica(M688_DATI "/font/originale.fnt");
        Palette pal = Palette::carica(M688_DATI "/palette/giorno.pal");
        p.s.applica_palette(pal);
        p.s.disegna(t, fo);
        t.salva_bmp(std::string(d) + "/sonar_verniero.bmp", pal);
    }
    // il registratore: il Charlie, il contatto del fascio che segue il bersaglio
    REQUIRE(p.s.registratore().storia(2));
    CHECK(p.s.registratore().storia(2)->size() >= 150);   // uno ogni 5 s, se il fascio ha righe
    const auto rapporto = p.s.rapporto_acint();
    REQUIRE(rapporto);
    CHECK(std::any_of(rapporto->voci.begin(), rapporto->voci.end(), [](const VoceAcint& v) { return v.contatto == 2; }));
    // ascolto: il WAV nella cartella degli ascolti accanto ai fascicoli
    const auto dati = fs::temp_directory_path() / "m688_prova_ascolto";
    setenv("XDG_DATA_HOME", dati.c_str(), 1);
    const std::string wav = p.s.registra_ascolto(p.m);
    unsetenv("XDG_DATA_HOME");
    REQUIRE(!wav.empty());
    CHECK(fs::file_size(wav) > 44 + 2 * 22050 * 7);   // 8 s a 16 bit, piu' l'intestazione
    p.s.aggancia_riga();   // F di nuovo: sganciata
    CHECK_FALSE(p.s.riga_misurata());
    fs::remove_all(dati);
    fs::remove(f);
}

// R2: la cuffia della stazione riempie il tempo vero chiesto (22.050 campioni al secondo), suona sopra il silenzio e
// riparte da capo quando tace
TEST_CASE("sonar enhanced: la cuffia in tempo reale riempie il tempo vero e tace a comando" * doctest::skip(!ci_sono_dati())) {
    const auto f = scena();
    Partita p(f);
    p.avanza(10.0);
    std::size_t n = 0;
    double somma = 0.0;
    for (int k = 0; k < 60; ++k) {   // un secondo a 60 fotogrammi
        const auto v = p.s.ascolta(p.m, 1.0 / 60.0);
        n += v.size();
        for (float x : v) somma += static_cast<double>(x) * x;
    }
    CHECK(static_cast<double>(n) == doctest::Approx(SonarEnhanced::CUFFIA_HZ).epsilon(0.05));
    CHECK(somma / static_cast<double>(std::max<std::size_t>(n, 1)) > 1e-5);   // non e' silenzio
    p.s.tace();
    CHECK(p.s.ascolta(p.m, 0.1).size() == doctest::Approx(SonarEnhanced::CUFFIA_HZ * 0.1).epsilon(0.1));
    fs::remove(f);
}

// Quaderno del sonarista: gli slot pieni si salvano con la loro origine e si ricaricano in un'altra partita, uguali
TEST_CASE("sonar enhanced: il quaderno del sonarista porta gli slot da una missione all'altra" * doctest::skip(!ci_sono_dati())) {
    const auto f = scena();
    const auto quaderno = fs::temp_directory_path() / "m688_prova_quaderno.txt";
    {
        Partita p(f);
        p.s.imposta_missione("PROVA");
        p.avanza(20.0);
        p.s.salva_slot();
        p.s.salva_slot();
        REQUIRE(p.s.slots()[0].pieno);
        CHECK(p.s.slots()[0].origine.rfind("PROVA ", 0) == 0);
        REQUIRE(p.s.salva_quaderno(quaderno.string()));
    }
    Partita q(f);
    CHECK(q.s.carica_quaderno(quaderno.string()) == 2);
    REQUIRE(q.s.slots()[0].pieno);
    CHECK(q.s.slots()[0].origine.rfind("PROVA ", 0) == 0);
    CHECK(q.s.slots()[0].spettro.snr_db.size() > 100);
    CHECK_FALSE(q.s.slots()[2].pieno);
    CHECK(q.s.carica_quaderno((fs::temp_directory_path() / "m688_nessun_quaderno.txt").string()) == 0);
    fs::remove(quaderno);
    fs::remove(f);
}

TEST_CASE("sonar enhanced: la maschera delle righe proprie e' un campo del giro, in entrambi i modi (docs/22, F)") {
    for (const ModoSonar modo : {ModoSonar::moderno, ModoSonar::anni_89}) {
        SonarEnhanced s(modo);
        CHECK(s.maschera());   // di base accesa
        int passi = 0;
        while (s.campo() != SonarEnhanced::Campo::maschera && passi < 10) s.campo_successivo(+1), ++passi;
        REQUIRE(s.campo() == SonarEnhanced::Campo::maschera);
        s.cambia_valore(+1);
        CHECK_FALSE(s.maschera());
        s.cambia_valore(-1);
        CHECK(s.maschera());
        s.campo_successivo(+1);   // dopo la maschera il giro torna al pavimento
        CHECK(s.campo() == SonarEnhanced::Campo::pavimento);
    }
}

TEST_CASE("sonar enhanced: la scala lunga col dithering quando la stazione copre la vista (roadmap 33)" * doctest::skip(!ci_sono_dati())) {
    Partita p(scena("charlie2", 8.0, 90));
    p.avanza(60.0);
    const Font fo = Font::carica(M688_DATI "/font/originale.fnt");
    // la palette: 128 indici da 96, il primo nero, in cima il giallo di viridis; la scala corta non si tocca
    Palette pal{};
    p.s.usa_scala_lunga(true);
    p.s.applica_palette(pal);
    CHECK(pal.rgb(cascate::SCALA_LUNGA.primo) == std::array<std::uint8_t, 3>{0, 0, 0});
    CHECK(pal.rgb(cascate::SCALA_LUNGA.primo + cascate::SCALA_LUNGA.livelli - 1)[1] > 200);
    // le cascate prendono gli indici della scala lunga, e nessuno di quelli della corta fuori dalla lunga
    Tela t;
    p.s.disegna(t, fo);
    int lunga = 0, corta = 0;
    for (int y = 0; y < ALTEZZA_VISTA; ++y)
        for (int x = 0; x < LARGHEZZA_TELA; ++x) {
            const int i = t.at(x, y);
            lunga += i > cascate::SCALA_LUNGA.primo && i < cascate::SCALA_LUNGA.primo + cascate::SCALA_LUNGA.livelli;
            corta += i >= cascate::SCALA_LUNGA.primo + cascate::SCALA_LUNGA.livelli && i < SonarEnhanced::SCALA0 + SonarEnhanced::LIVELLI;
        }
    CHECK(lunga > 1000);
    CHECK(corta == 0);
}

TEST_CASE("sonar enhanced: dentro la sala sonar del 688 e nell'Okean dell'Alfa, solo nell'area" * doctest::skip(!ci_sono_dati())) {
    // il 688 con la prua a est: il Charlie II a nord e' a 90 gradi a sinistra (relativo 270)
    Partita p(scena("charlie2", 8.0, 90));
    p.avanza(60.0);
    p.s.nuovo_tracker();
    const Font fo = Font::carica(M688_DATI "/font/originale.fnt");
    // la vista intera e' la stessa di sempre
    Tela a, b;
    p.s.disegna(a, fo);
    p.s.disegna(b, fo, {0, 0, LARGHEZZA_TELA, ALTEZZA_VISTA});
    CHECK(std::equal(a.dati(), a.dati() + LARGHEZZA_TELA * ALTEZZA_TELA, b.dati()));
    const auto fuori_intatto = [](const Tela& t, Rettangolo r) {
        for (int y = 0; y < ALTEZZA_TELA; ++y)
            for (int x = 0; x < LARGHEZZA_TELA; ++x)
                if ((x < r.x || x >= r.x + r.w || y < r.y || y >= r.y + r.h) && t.at(x, y) != 255) return false;
        return true;
    };
    const auto con_cascata = [](const Tela& t, Rettangolo r) {   // i colori della scala della cascata nell'area
        int n = 0;
        for (int y = r.y; y < r.y + r.h; ++y)
            for (int x = r.x; x < r.x + r.w; ++x)
                n += t.at(x, y) >= SonarEnhanced::SCALA0 && t.at(x, y) < SonarEnhanced::SCALA0 + SonarEnhanced::LIVELLI;
        return n;
    };
    // nel rettangolo libero della sala sonar: niente fuori, le cascate dentro
    const Rettangolo sala{6, 20, 628, 334};
    Tela c;
    c.riempi(255);
    p.s.disegna(c, fo, sala);
    CHECK(fuori_intatto(c, sala));
    CHECK(con_cascata(c, sala) > 25000);   // 30 righe in 60 s sulle due cascate
    // i livelli relativi: il picco verso il Charlie, a sinistra
    const auto l = p.s.livelli_relativi();
    REQUIRE(l.size() == 360);
    const int picco = static_cast<int>(std::max_element(l.begin(), l.end()) - l.begin());
    CHECK(std::abs(picco - 270) <= 10);
    // Okean: il pulto a sinistra con Enisej come cascata polare (bozza D, scelta dell'utente del 29/10/2026), lo spettro a
    // destra, niente fuori dalla vista
    const FontConsolle fc = FontConsolle::carica(M688_DATI, BattelloTema::alfa);
    const Rettangolo vista{0, 0, LARGHEZZA_TELA, ALTEZZA_VISTA};
    Tela o;
    o.riempi(255);
    p.s.disegna_okean(o, fo, fc, vista);
    CHECK(fuori_intatto(o, vista));
    CHECK(con_cascata(o, {320, 0, 320, 288}) > 15000);
    // la cascata polare: i colori della scala nel cerchio, il picco verso il Charlie (a sinistra della prua), il cieco di
    // poppa a tratteggio (niente scala sotto il centro dello schermo)
    CHECK(con_cascata(o, {0, 0, 320, 288}) > 3000);
    const auto accesi = [&](Rettangolo r) {   // sopra meta' scala: il contatto, non la grana del rumore
        int n = 0;
        for (int y = r.y; y < r.y + r.h; ++y)
            for (int x = r.x; x < r.x + r.w; ++x)
                n += o.at(x, y) >= SonarEnhanced::SCALA0 + SonarEnhanced::LIVELLI / 2 && o.at(x, y) < SonarEnhanced::SCALA0 + SonarEnhanced::LIVELLI;
        return n;
    };
    CHECK(accesi({20, 40, 140, 140}) > 2 * accesi({160, 40, 140, 140}));
    CHECK(con_cascata(o, {130, 230, 60, 40}) == 0);
    // con M688_ISTANTANEE=cartella: la sala sonar del 688 e l'Okean dell'Alfa, coi temi applicati
    if (const char* d = std::getenv("M688_ISTANTANEE")) {
        const Palette base = Palette::carica(M688_DATI "/palette/giorno.pal");
        const TemaStazione t688{BattelloTema::usa688, LuceSala::blu, Fosforo::del_battello};
        Palette p688 = base;
        applica_tema(p688, t688);
        p.s.applica_palette(p688);
        Tela s688;
        s688.riempi(colori::NERO);
        const Rettangolo libero = cornice_sala_sonar(s688, t688, FontConsolle::carica(M688_DATI, BattelloTema::usa688));
        p.s.disegna(s688, fo, libero);
        s688.salva_bmp(std::string(d) + "/sala_sonar_688.bmp", p688);
        Palette palfa = base;
        applica_tema(palfa, {BattelloTema::alfa, LuceSala::bianca, Fosforo::del_battello});
        p.s.applica_palette(palfa);
        o.salva_bmp(std::string(d) + "/okean_alfa.bmp", palfa);
    }
}

TEST_CASE("sonar enhanced: l'Alfa nel modo 1989 non ha gli slot (nessun registratore, dossier 19 §6.6)" * doctest::skip(!ci_sono_dati())) {
    for (const ModoSonar modo : {ModoSonar::anni_89, ModoSonar::moderno}) {
        Partita p(scena("los_angeles", 5.0, 0, "alfa"));
        p.s.imposta_modo(modo);
        p.avanza(30.0);
        REQUIRE_FALSE(p.s.lofar().empty());
        const bool con = modo == ModoSonar::moderno;
        CHECK(p.s.slot_disponibili() == con);
        p.s.salva_slot();
        CHECK(p.s.slots()[0].pieno == con);
        p.s.confronto_successivo();
        CHECK((p.s.confronto() != SonarEnhanced::Confronto::spento) == con);
    }
    // il 688 li ha in tutti e due i modi
    Partita q(scena());
    q.s.imposta_modo(ModoSonar::anni_89);
    q.avanza(30.0);
    CHECK(q.s.slot_disponibili());
    q.s.salva_slot();
    CHECK(q.s.slots()[0].pieno);
}

TEST_CASE("sonar enhanced: il pulsante 9 arriva alla storia lunga in entrambi i modi") {
    using C = SonarEnhanced::Cursore;
    for (const ModoSonar modo : {ModoSonar::moderno, ModoSonar::anni_89}) {
        SonarEnhanced s(modo);
        std::vector<C> giro;
        for (int i = 0; i < 8; ++i) {
            s.cursore_successivo();
            giro.push_back(s.cursore());
        }
        const std::vector<C> atteso = modo == ModoSonar::moderno
            ? std::vector<C>{C::lettura, C::delta, C::armonico, C::demon, C::righe, C::storia, C::spento, C::lettura}
            : std::vector<C>{C::lettura, C::delta, C::armonico, C::demon, C::storia, C::spento, C::lettura, C::delta};
        CHECK(giro == atteso);   // le righe proprie solo nel moderno
    }
}

namespace {
// una missione vera col suo fondale: le prove qui sotto chiedono la perdita in acqua bassa
struct PartitaVera : Partita {
    PartitaVera(const std::string& nome, bool traffico) : Partita(std::string(M688_MISSIONI) + "/" + nome + ".lua") {
        std::map<std::string, double> pescaggi;
        for (const auto& [k, c] : classi) pescaggi[k] = c.pescaggio_m;
        m.imposta_fondale(std::make_shared<const Fondale>(Fondale::carica(std::string(M688_DATI) + "/teatri/" + m.teatro() + ".fdl")), pescaggi);
        m.usa_traffico(traffico);
    }
};
std::size_t fascio_verso(const RigaCascata& r, double ril) {
    return static_cast<std::size_t>(std::lround(normalizza_360(ril - r.rilevamento0) / r.passo)) % r.db.size();
}
}  // namespace

// 07/11/2026: la cascata calcolava la perdita col livello A e il sensore coi raggi, 28 dB di differenza a 9 km in acqua
// bassa: il contatto c'era, solido, e sullo schermo non si vedeva niente.
TEST_CASE("sonar enhanced: la cascata disegna con la perdita del sensore: ciò che tiene, si vede" * doctest::skip(!ci_sono_dati())) {
    PartitaVera p("SCENE2B", false);
    p.avanza(120.0);
    const auto* g = p.m.giocatore();
    REQUIRE(g);
    const auto sc = p.m.scena_acustica(g->id, p.s.sensore());
    REQUIRE(sc.campi.size() == sc.sorgenti.size());
    const RigaCascata col = cascata(sc.sorgenti, sc.ricevitore, sc.ambiente, 72, 0.0, nullptr, &sc.campi);
    const RigaCascata senza = cascata(sc.sorgenti, sc.ricevitore, sc.ambiente, 72);
    REQUIRE_FALSE(p.s.btr().empty());
    const RigaCascata& schermo = p.s.btr().front();
    int visti = 0;
    for (std::size_t i = 0; i < sc.sorgenti.size(); ++i) {
        const Sorgente& s = sc.sorgenti[i];
        if (s.id < 3 || s.id > 5) continue;   // i tre mercantili della missione
        ++visti;
        CAPTURE(s.id);
        REQUIRE(sc.campi[i] != nullptr);
        const auto c = g->contatti.find(s.id);
        REQUIRE(c != g->contatti.end());   // il sensore li tiene
        CHECK(c->second.qualita >= 1000.0);
        const double ril = rilevamento(sc.ricevitore.pos, s.pos);
        CHECK(col.db[fascio_verso(col, ril)] > senza.db[fascio_verso(senza, ril)] + 6.0);   // il nucleo usa il campo
        CHECK(senza.db[fascio_verso(senza, ril)] < 2.0);   // il livello A non li disegnerebbe
        CHECK(schermo.db[fascio_verso(schermo, ril)] > 6.0);   // la stazione passa i campi: sullo schermo ci sono
    }
    CHECK(visti == 3);
}

TEST_CASE("sonar enhanced: la cascata segue la regola del sensore: il traffico oltre i 30 km resta col livello A" * doctest::skip(!ci_sono_dati())) {
    PartitaVera p("SCENE2A", true);
    p.avanza(120.0);
    const auto* g = p.m.giocatore();
    REQUIRE(g);
    const auto sc = p.m.scena_acustica(g->id, p.s.sensore());
    REQUIRE(sc.campi.size() == sc.sorgenti.size());
    int vicine = 0, lontane = 0;
    for (std::size_t i = 0; i < sc.sorgenti.size(); ++i) {
        const Sorgente& s = sc.sorgenti[i];
        if (s.id < 10000 || s.id >= 100000) continue;   // il traffico
        const double d = distanza(g->pos, s.pos);
        CAPTURE(s.id);
        CAPTURE(d);
        if (d > 30000.0) { ++lontane; CHECK(sc.campi[i] == nullptr); }
        else if (d < 29000.0) { ++vicine; CHECK(sc.campi[i] != nullptr); }
    }
    CHECK(vicine > 0);
    CHECK(lontane > 0);
}

// La riga del cursore (07/11/2026): in F VERNIERO e in F AGGANCIA due scritte si coprivano, e la STORIA usciva dallo schermo
TEST_CASE("sonar enhanced: la riga del cursore non si copre e non si taglia, in ogni modo, vista e tastiera" * doctest::skip(!ci_sono_dati())) {
    const Font fo = Font::carica(M688_DATI "/font/originale.fnt");
    const std::string tasti_prima = tasti_valore();
    const Rettangolo piena{0, 0, LARGHEZZA_TELA, ALTEZZA_VISTA}, sala{6, 20, 628, 334};
    // l'ultima x col colore di sinistra e la prima col colore di destra, nella fascia y0-y1
    const auto estremi = [](const Tela& t, Rettangolo a, int y0, int y1, std::uint8_t cs, std::uint8_t cd) {
        std::pair<int, int> r{-1, 100000};
        for (int y = y0; y < y1; ++y)
            for (int x = a.x + a.w / 2; x < a.x + a.w; ++x) {
                if (t.at(x, y) == cs) r.first = std::max(r.first, x);
                if (t.at(x, y) == cd) r.second = std::min(r.second, x);
            }
        return r;
    };
    for (const char* tasti : {"[ ]", "ALTGR+\u00c8/+"}) {
        tasti_valore() = tasti;
        for (const char* batt : {"los_angeles", "alfa"})
            for (const ModoSonar modo : {ModoSonar::moderno, ModoSonar::anni_89}) {
                const auto file = scena("charlie2", 8.0, 0, batt);
                Partita p(file);
                p.s.imposta_modo(modo);
                p.avanza(40.0);
                p.s.nuovo_tracker();
                p.avanza(40.0);
                p.s.analizza(p.m);
                p.s.chiudi_pagina_analisi();
                const bool alfa = std::string(batt) == "alfa", moderno = modo == ModoSonar::moderno;
                const std::uint8_t tenue = !moderno ? (alfa ? fosforo(4) : std::uint8_t{colori::VERDE}) : std::uint8_t{colori::GRIGIO};
                const FontConsolle fc = FontConsolle::carica(M688_DATI, alfa ? BattelloTema::alfa : BattelloTema::usa688);
                for (const int vista : {0, 1}) {   // a tutta vista e nella sala sonar; sull'Alfa nell'Okean
                    const Rettangolo area = vista == 0 || alfa ? piena : sala;
                    for (int giro = 0; giro < 7; ++giro) {
                        for (const bool verniero : {false, true}) {
                            const auto c = p.s.cursore();
                            if (c != SonarEnhanced::Cursore::spento && verniero != p.s.riga_misurata().has_value()) p.s.aggancia_riga();
                            Tela t;
                            t.riempi(colori::NERO);
                            if (alfa && vista == 1) p.s.disegna_okean(t, fo, fc, piena);
                            else p.s.disegna(t, fo, area);
                            if (c == SonarEnhanced::Cursore::spento) continue;
                            const bool consolle = modo == ModoSonar::anni_89 && !(alfa && vista == 1);
                            int y = -1;
                            for (int yy = area.y + area.h - (consolle ? 60 : 71); yy < area.y + area.h && y < 0; ++yy)
                                for (int x = area.x + area.w / 2; x < area.x + area.w; ++x)
                                    if (t.at(x, yy) == colori::GIALLO) { y = yy; break; }
                            REQUIRE(y >= 0);
                            const auto [testo, suggerimento] = estremi(t, area, y, y + 8, colori::GIALLO, tenue);
                            CAPTURE(batt);
                            CAPTURE(tasti);
                            CAPTURE(vista);
                            CAPTURE(moderno);
                            CAPTURE(static_cast<int>(c));
                            CHECK(testo < suggerimento);
                            CHECK(testo < area.x + area.w - 1);
                            // il suggerimento del verniero solo dove F fa quello
                            const bool f_verniero = c == SonarEnhanced::Cursore::lettura || c == SonarEnhanced::Cursore::delta || c == SonarEnhanced::Cursore::armonico;
                            if (!f_verniero) CHECK(suggerimento == 100000);
                            // l'analizzatore, in giallo sotto: non esce dallo schermo
                            CHECK(estremi(t, area, y + 20, area.y + area.h, colori::GIALLO, tenue).first < area.x + area.w - 1);
                        }
                        p.s.cursore_successivo();
                    }
                }
                fs::remove(file);
            }
    }
    tasti_valore() = tasti_prima;
}

