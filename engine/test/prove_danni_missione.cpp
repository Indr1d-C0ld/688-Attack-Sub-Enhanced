#include <doctest.h>

#include <cmath>
#include <filesystem>
#include <fstream>
#include <map>
#include <string>

#include "interfaccia.hpp"
#include "missioni/danni_missione.hpp"
#include "missioni/missione.hpp"
#include "missioni/stazioni_missione.hpp"
#include "nucleo/classi.hpp"
#include "nucleo/comando.hpp"
#include "nucleo/mixer_audio.hpp"
#include "nucleo/ordini.hpp"

using namespace m688;
namespace fs = std::filesystem;

// Regola d'impatto dell'originale (docs/originale/sensori_e_navigazione.md §9.8), relitti e reazioni (fase D5).

namespace {

std::string missione(const std::string& unita, const std::string& nome) {
    const auto f = fs::temp_directory_path() / ("m688_prova_" + nome + ".lua");
    std::ofstream(f) << "return { id = 'PROVA', teatro = 'prova', script = {}, varianti = { { probabilita = 1, unita = {\n"
                     << unita << "\n}, timer = {} } } }\n";
    return f.string();
}

const std::string MODELLO = "modello = { massima_kn = 30, virata_gradi_s = 1.44, accel_kn_s = 0.05, quota_m_s = 1, quota_max_m = 450, integrita = ";

// giocatore lontano, una Perry ferma a (0, 3000) con l'integrita' data, un siluro che arriva da sud alla quota data;
// facoltativi altri bersagli (righe Lua)
std::string scena(int integrita_nave, double quota_siluro, const std::string& classe_siluro = "mk48", const std::string& altro = {}) {
    return "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true,"
           "  posizione = { x = -20000, y = -20000 }, quota_m = 50, rotta = 0, velocita_kn = 0, " + MODELLO + "160 } },"
           "{ id = 1, classe = 'biologico', categoria = 'nessuna', flag = 192, posizione = { x = 0, y = 2000 },"
           "  quota_m = " + std::to_string(quota_siluro) + ", rotta = 0, velocita_kn = 0, " + MODELLO + "10 } },"
           "{ id = 2, classe = 'perry', categoria = 'nave', flag = 18, posizione = { x = 0, y = 3000 },"
           "  quota_m = 0, rotta = 90, velocita_kn = 0, " + MODELLO + std::to_string(integrita_nave) + " } },"
           "{ id = 3, classe = '" + classe_siluro + "', categoria = 'arma', flag = 41, posizione = { x = 0, y = 0 },"
           "  quota_m = " + std::to_string(quota_siluro) + ", rotta = 0, velocita_kn = 50, "
           "  modello = { massima_kn = 50, virata_gradi_s = 9, accel_kn_s = 5, quota_m_s = 5, quota_max_m = 500, integrita = 2, difesa = 75 } },"
           + altro;
}

bool c_e_messaggio(const Missione& m, int codice) {
    for (const auto& x : m.messaggi()) if (x.codice == codice) return true;
    return false;
}

}  // namespace

TEST_CASE("impatto: il siluro colpisce entro 5 unita' per asse e 100 piedi, poi sparisce") {
    const auto f = missione(scena(100, 5.0), "impatto");
    Missione m(f, 1, false);
    for (int i = 0; i < 9 * 200 && m.colpi().empty(); ++i) m.avanza(1.0 / 9.0);
    REQUIRE(m.colpi().size() == 1);
    const Colpo& c = m.colpi().front();
    CHECK(c.vittima == 2);                  // non il biologico (falso bersaglio) che sta sulla rotta
    CHECK(c.arma == 3);
    CHECK((c.danno == 75 || c.danno == 37));   // dimezzato in 7 passi su 18
    // colpita entro la finestra quadrata: 5 unita' (5 x 37,04 m) davanti alla nave
    CHECK(std::fabs(c.pos.y - 3000.0) < 5 * 37.04 + 15.0);
    CHECK(m.unita().at(2).integrita == 100 - c.danno);
    CHECK(m.unita().at(2).colpo == 1);
    CHECK(c_e_messaggio(m, 7));             // "un siluro ha raggiunto il bersaglio"
    m.avanza(1.0);
    CHECK(m.unita().count(3) == 0);         // l'arma e' uscita dal gioco
    fs::remove(f);
}

TEST_CASE("impatto su ogni coppia (regola di simulazione): stessa vittima della coppia dei sensori") {
    // senza impatti_dell_originale il colpo si controlla su ogni coppia a ogni passo
    const auto f = missione(scena(100, 5.0), "impatto_ogni_coppia");
    Missione m(f, 1, false);
    m.impatti_dell_originale(false);
    for (int i = 0; i < 9 * 200 && m.colpi().empty(); ++i) m.avanza(1.0 / 9.0);
    REQUIRE(m.colpi().size() == 1);
    CHECK(m.colpi().front().vittima == 2);
    CHECK(m.colpi().front().arma == 3);
    CHECK(std::fabs(m.colpi().front().pos.y - 3000.0) < 5 * 37.04 + 15.0);
    fs::remove(f);
}

TEST_CASE("impatto: sotto la nave a piu' di 100 piedi il siluro passa") {
    const auto f = missione(scena(100, 40.0), "profondo");
    Missione m(f, 1, false);
    for (int i = 0; i < 9 * 200; ++i) m.avanza(1.0 / 9.0);
    CHECK(m.colpi().empty());
    fs::remove(f);
}

TEST_CASE("distruzione: conto delle visite, l'unita' continua a muoversi e poi esce dal gioco") {
    const auto f = missione(scena(30, 5.0), "distruzione");   // 30: distrutta anche dal colpo dimezzato
    Missione m(f, 1, false);
    for (int i = 0; i < 9 * 200 && m.colpi().empty(); ++i) m.avanza(1.0 / 9.0);
    REQUIRE(m.colpi().size() == 1);
    CHECK(m.colpi().front().distrutta);
    const auto& v = m.unita().at(2);
    CHECK(v.affondamento == 18);
    CHECK(v.colpo == 0xD3);
    double t = 0;
    while (m.unita().count(2) && t < 120) { m.avanza(1.0 / 9.0); t += 1.0 / 9.0; }
    // 18 visite a un'unita' per passo con 3 unita' rimaste (il siluro e' uscito): 54 passi, 6 s
    CHECK(m.unita().count(2) == 0);
    CHECK(t == doctest::Approx(6.0).epsilon(0.2));
    fs::remove(f);
}

TEST_CASE("livello classico: la nave distrutta diventa un relitto che affonda") {
    const auto classi = carica_classi(M688_DATI "/classi.toml");
    const auto f = missione(scena(30, 5.0), "relitto");
    Missione m(f, 1, false);
    DanniMissione dm(classi, M688_DATI, 1, RegolaDanni::classica);
    Mondo mondo;
    for (int i = 0; i < 9 * 3600 * 2; ++i) {
        m.avanza(1.0 / 9.0);
        dm.avanza(m, 1.0 / 9.0);
        if (!dm.navi().empty() && dm.navi().front()->danni->affondata()) break;
    }
    REQUIRE(dm.navi().size() == 1);
    const auto& n = *dm.navi().front();
    CHECK(n.relitto);
    CHECK(n.danni->condannata());             // spezzata dal Mk 48 o abbandonata col colpo di grazia
    CHECK(n.danni->affondata());              // in ogni caso affonda entro due ore
    m.sincronizza(mondo);
    dm.sincronizza(mondo);
    bool immagine = false, unita = false;
    for (const auto& x : mondo.navi()) { immagine |= x.id == DanniMissione::ID_IMMAGINE + 2; unita |= x.id == 2; }
    CHECK(immagine);
    CHECK_FALSE(unita);
    fs::remove(f);
}

TEST_CASE("simulazione: la fisica decide, la nave colpita si ferma e le amiche vanno in soccorso") {
    const auto classi = carica_classi(M688_DATI "/classi.toml");
    // una Perry con integrita' 200 (sopravvivrebbe al livello classico) e una Spruance amica a 5 km
    const auto f = missione(scena(200, 5.0, "mk48",
                                  "{ id = 4, classe = 'spruance', categoria = 'nave', flag = 18, posizione = { x = 5000, y = 3000 },"
                                  "  quota_m = 0, rotta = 0, velocita_kn = 10, " + MODELLO + "100 } },"),
                            "simulazione");
    Missione m(f, 2, false);
    DanniMissione dm(classi, M688_DATI, 2, RegolaDanni::simulazione);
    bool soccorso = false;
    for (int i = 0; i < 9 * 1800; ++i) {
        m.avanza(1.0 / 9.0);
        dm.avanza(m, 1.0 / 9.0);
        if (m.unita().count(4)) soccorso |= m.unita().at(4).ordine.meta;
        if (m.unita().count(2) && m.unita().at(2).affondamento > 0) break;
    }
    REQUIRE(dm.navi().size() == 1);
    const auto& n = *dm.navi().front();
    CHECK(n.danni->spezzata());                       // Mk 48 sotto la chiglia: la fisica la spezza
    REQUIRE(m.unita().count(2));
    CHECK(m.unita().at(2).affondamento > 0);          // e la missione la da' per distrutta
    CHECK_FALSE(m.colpi().front().distrutta);         // non per l'integrita' dell'originale (200 - 75)
    CHECK(m.unita().at(2).ordine.velocita_max_kn == 0.0);   // ferma
    CHECK(soccorso);                                  // la Spruance va sul punto
    fs::remove(f);
}

TEST_CASE("il deposito in fiamme e il deposito allagato: una volta per nave, e non al livello classico (danni critici, T15)") {
    const auto classi = carica_classi(M688_DATI "/classi.toml");
    const auto conta = [&](RegolaDanni regola) {
        // col Mk 46 la nave resta intera: col Mk 48 si spezza e il deposito lo allaga il mare, senza allagamento d'emergenza
        const auto f = missione(scena(1000, 5.0, "mk46"), "t15");
        Missione m(f, 2, false);
        DanniMissione dm(classi, M688_DATI, 2, regola);
        std::map<int, int> visti;
        bool acceso = false;
        for (int i = 0; i < 9 * 600; ++i) {
            m.avanza(1.0 / 9.0);
            dm.avanza(m, 1.0 / 9.0);
            if (!acceso && !dm.navi().empty()) {   // subito dopo il colpo: il fuoco nel primo deposito
                auto& d = *dm.navi().front()->danni;
                const auto& celle = d.corpi().front().celle;
                for (std::size_t j = 0; j < celle.size() && !acceso; ++j)
                    if (celle[j].dati.uso == "deposito") { d.imposta_fuoco(j, 1.0); acceso = true; }
            }
        }
        REQUIRE(acceso);
        for (const auto& x : dm.messaggi()) ++visti[x.codice];
        fs::remove(f);
        return visti;
    };
    auto sim = conta(RegolaDanni::simulazione);
    CHECK(sim[207] == 1);
    CHECK(sim[208] == 1);
    auto cla = conta(RegolaDanni::classica);
    CHECK(cla[207] == 0);
    CHECK(cla[208] == 0);
}

TEST_CASE("a bordo: lo scoppio del deposito di una nave si sente una volta, e non dopo una ricostruzione (danni critici, T14)") {
    const auto classi = carica_classi(M688_DATI "/classi.toml");
    // col Mk 46 la nave resta intera; il giocatore a 6 km
    std::string unita = scena(1000, 5.0, "mk46");
    const std::string lontano = "posizione = { x = -20000, y = -20000 }";
    REQUIRE(unita.find(lontano) != std::string::npos);
    unita.replace(unita.find(lontano), lontano.size(), "posizione = { x = -6000, y = 3000 }");
    const auto f = missione(unita, "t14");
    Missione m(f, 2, false);
    DanniMissione dm(classi, M688_DATI, 2, RegolaDanni::simulazione);
    dm.avanza(m, 1.0 / 9.0);
    StazioniMissione st(m, 2u);
    PostoDiComando posto;
    RegistroOrdini ordini;
    std::vector<Messaggio> equipaggio;
    InterfacciaMissione ui(m, st, posto, ordini, equipaggio, classi, 1);
    ParametriMixer pm;
    pm.campionamento_hz = 22050;
    Mixer mixer(pm);   // senza dispositivo, come --audio-wav
    ui.imposta_mixer(&mixer);
    ui.imposta_navi(&dm);
    const auto picco = [&](double secondi) {   // il massimo in uscita in tanti secondi di partita
        float p = 0.0f;
        for (long i = 0; i < static_cast<long>(secondi * 9.0); ++i) {
            m.avanza(1.0 / 9.0);
            dm.avanza(m, 1.0 / 9.0);
            ui.audio(1.0 / 9.0, 1.0);
            for (const float x : mixer.produci(22050 / 9)) p = std::max(p, std::fabs(x));
        }
        return p;
    };
    for (int i = 0; i < 300 && dm.navi().empty(); ++i) picco(1.0);
    REQUIRE_FALSE(dm.navi().empty());
    picco(30.0);   // si spegne lo scoppio del siluro
    const float prima = std::max(picco(10.0), 1e-4f);
    DanniNave& nave = *dm.navi().front()->danni;
    const auto scoppi = nave.scoppi().size();
    REQUIRE(nave.esplosione_di_prova(false));
    REQUIRE(nave.scoppi().size() == scoppi + 1);
    const float scoppio = picco(4.0);
    const float dopo = picco(20.0);
    MESSAGE("picco prima ", prima, ", allo scoppio ", scoppio, ", dopo ", dopo);
    CHECK(scoppio > 3.0f * prima);
    CHECK(dopo < 0.5f * scoppio);   // non si ripete
    // quello che e' successo durante una ricostruzione non suona quando l'audio riparte
    ui.ricostruzione(true);
    REQUIRE(nave.esplosione_di_prova(false));
    ui.ricostruzione(false);
    const float riparte = picco(4.0);
    MESSAGE("picco dopo la ricostruzione ", riparte);
    CHECK(riparte < 0.5f * scoppio);
    fs::remove(f);
}

TEST_CASE("livello classico: colpo di grazia quando la regola dell'originale distrugge una nave che la fisica tiene a galla") {
    const auto classi = carica_classi(M688_DATI "/classi.toml");
    // siluro sovietico a contatto sul fianco (300 kg): la Perry resta a galla, ma per l'originale e' distrutta
    const auto f = missione(scena(30, 5.0, "type53"), "grazia");
    Missione m(f, 1, false);
    DanniMissione dm(classi, M688_DATI, 1, RegolaDanni::classica);
    for (int i = 0; i < 9 * 7200; ++i) {
        m.avanza(1.0 / 9.0);
        dm.avanza(m, 1.0 / 9.0);
        if (!dm.navi().empty() && dm.navi().front()->danni->affondata()) break;
    }
    REQUIRE(dm.navi().size() == 1);
    const auto& n = *dm.navi().front();
    CHECK(n.relitto);
    CHECK(n.colpo_di_grazia);
    CHECK(n.danni->affondata());
    fs::remove(f);
}

TEST_CASE("mine dell'Enhanced (M2b): carica vera e scoppio a influenza sotto la chiglia, alla distanza vera") {
    const auto classi = carica_classi(M688_DATI "/classi.toml");
    const auto f = missione(scena(200, 5.0, "mk48"), "mina");
    Missione m(f, 2, false);
    DanniMissione dm(classi, M688_DATI, 2, RegolaDanni::simulazione);
    m.avanza(1.0 / 9.0);
    Colpo c;
    c.tipo = Colpo::Tipo::arma;
    c.tempo = m.tempo();
    c.vittima = 2;
    c.classe_vittima = "perry";
    c.classe_arma = "mk67";
    c.tnt_kg = 150.0;
    c.influenza = true;
    c.geometria_vera = true;
    c.distanza_m = 12.0;
    c.dz_m = 10.0;   // 10 m sotto il mare, 6,6 m di lato
    m.registra_colpo(c);
    dm.avanza(m, 1.0 / 9.0);
    const auto* n = dm.nave(2);
    REQUIRE(n != nullptr);
    REQUIRE_FALSE(n->danni->scoppi().empty());
    const auto& s = n->danni->scoppi().back();
    CHECK(s.tipo == Esplosione::Tipo::mina);
    CHECK(s.tnt_kg == doctest::Approx(150.0));
    CHECK(s.z < -5.0);                                      // sotto la chiglia, non a contatto
    CHECK(std::fabs(s.y) == doctest::Approx(std::sqrt(144.0 - 100.0)).epsilon(0.05));
    // la carica vera vale anche per un siluro (PMR-1: 80 kg a contatto)
    Colpo p = c;
    p.influenza = false;
    p.geometria_vera = false;
    p.classe_arma = "pmr1";
    p.tnt_kg = 80.0;
    m.registra_colpo(p);
    dm.avanza(m, 1.0 / 9.0);
    CHECK(n->danni->scoppi().back().tnt_kg == doctest::Approx(80.0));
    CHECK(n->danni->scoppi().back().tipo == Esplosione::Tipo::siluro);
    fs::remove(f);
}
