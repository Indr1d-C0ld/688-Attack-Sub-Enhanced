#include <doctest.h>

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <map>
#include <set>
#include <string>
#include <vector>

#include "grafica/font.hpp"
#include "grafica/palette.hpp"
#include "grafica/tela.hpp"
#include "interfaccia.hpp"
#include "missioni/danni_missione.hpp"
#include "missioni/danni_sottomarini.hpp"
#include "missioni/missione.hpp"
#include "missioni/stazioni_missione.hpp"
#include "nucleo/classi.hpp"
#include "nucleo/comando.hpp"
#include "nucleo/ordini.hpp"
#include "piattaforma/finestra.hpp"
#include "scena_prova.hpp"

using namespace m688;
namespace fs = std::filesystem;

// I comandi di manovra alla stazione F4, dal vero: si disegna il pannello, si clicca dove il comando e' stato disegnato, l'ordine esce.

namespace {

std::string scena(const std::string& classe, double quota = 100.0) {
    const auto f = fs::temp_directory_path() / ("m688_prova_manovra_" + classe + ".lua");
    std::ofstream(f) << "return { id = 'PROVA', teatro = 'prova', script = {}, varianti = { { probabilita = 1, unita = {\n"
                     << "{ id = 0, classe = '" << classe << "', categoria = 'sottomarino', flag = 9, giocatore = true,"
                     << "  posizione = { x = 0, y = 0 }, quota_m = " << quota << ", rotta = 0, velocita_kn = 0,"
                     << "  modello = { massima_kn = 30, virata_gradi_s = 1.44, accel_kn_s = 0.05, quota_m_s = 1, quota_max_m = 450, integrita = 160 } }"
                     << "\n}, timer = {} } } }\n";
    return f.string();
}

const std::map<std::string, Azione> AZIONI = {
    {"timone_sinistra", Azione::timone_sinistra}, {"timone_dritta", Azione::timone_dritta}, {"timone_centro", Azione::timone_centro},
    {"barra_su", Azione::barra_su}, {"barra_giu", Azione::barra_giu}, {"barra_zero", Azione::barra_zero},
    {"telegrafo_avanti", Azione::telegrafo_avanti}, {"telegrafo_indietro", Azione::telegrafo_indietro}};

struct Banco {
    std::map<std::string, ClasseNave> classi = carica_classi(std::string(M688_DATI) + "/classi.toml");
    Missione m;
    StazioniMissione st;
    PostoDiComando posto;
    RegistroOrdini ordini;
    std::vector<Messaggio> equipaggio;
    InterfacciaMissione ui;
    Font f = Font::carica(std::string(M688_DATI) + "/font/originale.fnt");
    VistaPeriscopio v;
    Tela t;
    std::vector<AreaAttiva> aree;

    Banco(const std::string& classe, bool enhanced, double quota = 100.0) : m(scena(classe, quota), 1, false), st(m, 1u), ui(m, st, posto, ordini, equipaggio, classi, 4) {
        ui.imposta_enhanced(enhanced);
        if (enhanced) {
            m.dinamica_sottomarini(classi, M688_DATI);
            ui.imposta_consolle(M688_DATI);
        }
        m.avanza(1.0 / 9.0);
        Comandi c;
        c.stazione = 4;
        ui.comandi(c, v, 1.0 / 9.0);
        ui.imposta_comandi(M688_DATI, MappaTasti::predefinita(), false);
    }
    bool detto(int codice) const {
        return std::any_of(equipaggio.begin(), equipaggio.end(), [&](const Messaggio& x) { return x.codice == codice; });
    }
    void azione(Azione a) {
        Comandi c;
        c.azioni.push_back(a);
        ui.comandi(c, v, 1.0 / 9.0);
    }
    void nome(const std::string& k) {
        Comandi c;
        c.nominati.push_back(k);
        ui.comandi(c, v, 1.0 / 9.0);
    }
    double quota_a_schermo() {
        StatoManovra q;
        q.quota_m = m.giocatore()->quota;
        q.quota_ordinata_m = m.giocatore()->quota_ordinata;
        ui.manovra_enhanced(q);
        return q.quota_m;
    }
    std::set<std::string> comandi_a_schermo() {
        StatoPannello s;
        ui.pannello(s);
        StatoManovra q;
        ui.manovra_enhanced(q);
        s.manovra = q;
        Nave proprio;
        disegna_pannello(t, f, proprio, v, s, &aree);
        std::set<std::string> c;
        for (const auto& a : aree) c.insert(a.comando);
        return c;
    }
    std::vector<Ordine> clic(const std::string& comando) {
        comandi_a_schermo();
        std::vector<Ordine> fatti;
        for (const auto& a : aree)
            if (a.comando == comando) {
                Comandi c;
                c.azioni.push_back(AZIONI.at(comando));   // applica() di piattaforma/finestra.cpp, che qui non si collega
                ui.comandi(c, v, 1.0 / 9.0);
                fatti = ordini.consegna(m.tempo() + 1.0);
                break;
            }
        return fatti;
    }
};

}  // namespace

TEST_CASE("manovra, dal vero: nell'Enhanced si clicca su timone, piani e telegrafo" * doctest::skip(!fs::exists(std::string(M688_DATI) + "/classi.toml"))) {
    Banco k("los_angeles", true);
    const auto c = k.comandi_a_schermo();
    for (const auto& [nome, azione] : AZIONI) {
        if (nome == "telegrafo_indietro") continue;   // da STOP il telegrafo va avanti e indietro: l'area c'e' sulle altre campane
        CAPTURE(nome);
        CHECK(c.count(nome) == 1);
        const auto o = k.clic(nome);
        CHECK_FALSE(o.empty());
    }
}

TEST_CASE("manovra, dal vero: nel classico non si clicca sui comandi di F4, valgono i tasti" * doctest::skip(!fs::exists(std::string(M688_DATI) + "/classi.toml"))) {
    Banco k("los_angeles", false);
    const auto c = k.comandi_a_schermo();
    for (const auto& [nome, azione] : AZIONI) CHECK(c.count(nome) == 0);
}

TEST_CASE("periscopio: la quota a schermo non inganna" * doctest::skip(!fs::exists(std::string(M688_DATI) + "/classi.toml"))) {
    SUBCASE("a quota periscopica si alza") {
        Banco k("los_angeles", true, 6.096);
        CHECK(k.quota_a_schermo() == doctest::Approx(18.3).epsilon(0.01));
        k.azione(Azione::periscopio);
        CHECK_FALSE(k.detto(MSG_SOLO_PERISCOPICA));
        CHECK_FALSE(k.ordini.vuota());
    }
    SUBCASE("a 18 m della missione la chiglia a schermo non dice ancora 18: si legge che manca, e il rifiuto torna") {
        Banco k("los_angeles", true, 18.0);
        CHECK(k.quota_a_schermo() > 25.0);
        CHECK(k.quota_a_schermo() < 30.0);
        k.azione(Azione::periscopio);
        CHECK(k.detto(500));   // l'Enhanced dice come fare
        CHECK(k.ordini.vuota());
    }
    SUBCASE("la spia e il rifiuto dicono la stessa cosa") {
        for (const double q : {6.096, 6.5, 6.7, 7.0}) {
            CAPTURE(q);
            Banco k("los_angeles", true, q);
            StatoManovra s;
            s.quota_m = q;
            s.quota_ordinata_m = q;
            k.ui.manovra_enhanced(s);
            k.azione(Azione::periscopio);
            CHECK((s.quota_periscopica == 1) == !k.detto(500));
            CHECK((s.quota_periscopica == 1) == !k.ordini.vuota());
        }
    }
    SUBCASE("nel classico il rifiuto resta quello del 1989") {
        Banco k("los_angeles", false, 30.0);
        k.azione(Azione::periscopio);
        CHECK(k.detto(MSG_SOLO_PERISCOPICA));
        CHECK_FALSE(k.detto(500));
    }
    SUBCASE("in profondita' la quota e' quella di sempre") {
        Banco k("los_angeles", true, 100.0);
        CHECK(k.quota_a_schermo() == doctest::Approx(100.0).epsilon(0.001));
    }
    SUBCASE("Ctrl+P da fermo: l'ordine parte, e il timoniere dice che serve velocita'") {
        Banco k("los_angeles", true, 30.0);
        k.azione(Azione::quota_periscopica);
        CHECK(k.detto(469));
        CHECK_FALSE(k.ordini.vuota());
    }
    SUBCASE("Ctrl+P gia' a quota periscopica: niente messaggio") {
        Banco k("los_angeles", true, 6.096);
        k.azione(Azione::quota_periscopica);
        CHECK_FALSE(k.detto(469));
    }
}

TEST_CASE("schermata dei comandi: F11 apre la scheda della stazione, Esc chiude, intanto nessun ordine" * doctest::skip(!fs::exists(std::string(M688_DATI) + "/comandi_schermata.toml"))) {
    Banco k("los_angeles", true, 6.096);
    CHECK_FALSE(k.ui.schermata_comandi_aperta());
    k.nome("aiuto_comandi");
    REQUIRE(k.ui.schermata_comandi_aperta());
    CHECK(k.ui.in_pausa());
    k.azione(Azione::telegrafo_avanti);   // assorbito
    CHECK(k.ordini.vuota());
    Comandi esc;
    esc.esc = true;
    CHECK(k.ui.comandi_schermata(esc));
    CHECK_FALSE(k.ui.schermata_comandi_aperta());
    CHECK_FALSE(k.ui.in_pausa());
    k.azione(Azione::telegrafo_avanti);
    CHECK_FALSE(k.ordini.vuota());
    // disegnata e cliccata: la linguetta cambia scheda, il piede chiude, e il clic non passa alla partita
    k.nome("aiuto_comandi");
    k.ui.disegna(k.t, k.f, k.v);
    REQUIRE_FALSE(k.ui.aree_comandi().empty());
    const auto clic = [&](const AreaComando& a) {
        Comandi c;
        c.clic.push_back({a.r.x + a.r.w / 2, a.r.y + a.r.h / 2});
        CHECK(k.ui.comandi_schermata(c));
    };
    for (const auto& a : k.ui.aree_comandi())
        if (a.scheda == 0) { clic(a); break; }
    CHECK(k.ui.scheda_comandi() == 0);
    k.ui.disegna(k.t, k.f, k.v);
    const auto aree = k.ui.aree_comandi();
    for (const auto& a : aree)
        if (a.chiudi) { clic(a); break; }
    CHECK_FALSE(k.ui.schermata_comandi_aperta());
    // F2-F8 cambiano scheda e non stazione
    k.nome("aiuto_comandi");
    Comandi f5;
    f5.stazione = 5;
    CHECK(k.ui.comandi_schermata(f5));
    CHECK(k.ui.scheda_comandi() == 3);
    k.nome("aiuto_comandi");
    CHECK(k.ui.stazione() == 4);
}

TEST_CASE("schermata dei comandi: i dati seguono il modo, il battello e i tasti rimappati" * doctest::skip(!fs::exists(std::string(M688_DATI) + "/comandi_schermata.toml"))) {
    const TestoComandi testo(M688_DATI);
    const MappaTasti tasti = MappaTasti::predefinita();
    const auto riga = [](const DatiSchermataComandi& d, const std::string& pezzo) {
        for (const auto& s : d.schede)
            for (const auto& g : s.gruppi)
                for (const auto& r : g.righe)
                    if (r.cosa.find(pezzo) != std::string::npos) return r;
        return RigaComando{};
    };
    const auto enh = testo.dati(true, false, tasti), cla = testo.dati(false, false, tasti), alfa = testo.dati(true, true, tasti);
    CHECK(testo.scheda_di(5) == 3);
    CHECK(enh.schede[static_cast<std::size_t>(testo.scheda_di(5))].etichetta == "F5 PERISCOPIO");
    CHECK(enh.schede[static_cast<std::size_t>(testo.scheda_di(9))].etichetta == "OVUNQUE");
    // il periscopio dice che cosa serve prima, col tasto vero
    const auto& cons = enh.schede[3].consigli;
    REQUIRE_FALSE(cons.empty());
    CHECK(cons.front().find("Ctrl+P") != std::string::npos);
    CHECK(cons.front().find("6 NODI") != std::string::npos);
    CHECK(cons.front().find("{comando") == std::string::npos);
    // i tasti di manovra, solo nell'Enhanced le righe dell'Enhanced, l'Alfa solo sull'Alfa
    CHECK(riga(enh, "il timone").tasti == "Tastierino 4, Tastierino 6, Tastierino 5");
    CHECK_FALSE(riga(enh, "immersione d'emergenza").cosa.empty());
    CHECK(riga(cla, "immersione d'emergenza").cosa.empty());
    CHECK(riga(enh, "Akkord").cosa.empty());
    CHECK_FALSE(riga(alfa, "Akkord").cosa.empty());
    // le quadre, sulla tastiera italiana
    CHECK(riga(enh, "i minuti della proiezione").tasti.find("AltGr+è") != std::string::npos);
    CHECK(riga(enh, "i minuti della proiezione").tasti.find("AltGr++") != std::string::npos);
    // rimappato
    MappaTasti altri = MappaTasti::predefinita();
    altri.applica_testo("[tasti]\ntimone_sinistra = \"Ctrl+Y\"\n");
    CHECK(riga(testo.dati(true, false, altri), "il timone").tasti.rfind("Ctrl+Y", 0) == 0);
    // nessuna riga senza tasti o senza testo
    for (const auto& s : enh.schede)
        for (const auto& g : s.gruppi)
            for (const auto& r : g.righe) {
                CAPTURE(r.cosa);
                CHECK_FALSE(r.tasti.empty());
                CHECK_FALSE(r.cosa.empty());
                CHECK(r.tasti.find("NESSUN TASTO") == std::string::npos);
            }
}
