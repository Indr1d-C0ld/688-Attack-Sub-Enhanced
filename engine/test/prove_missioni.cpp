#include <doctest.h>

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <tuple>
#include <map>
#include <memory>
#include <set>
#include <string>

#include "missioni/missione.hpp"
#include "missioni/stazioni_missione.hpp"
#include "nucleo/carico.hpp"
#include "nucleo/classi.hpp"
#include "nucleo/fondale.hpp"

using namespace m688;
namespace fs = std::filesystem;

namespace {
const std::string DIR = M688_MISSIONI;
bool ci_sono_missioni() { return fs::exists(DIR + "/SCENE1A.lua"); }
std::string elenco(const std::vector<std::string>& v) {
    std::string s;
    for (size_t i = 0; i < v.size() && i < 5; ++i) s += v[i] + "\n";
    return s;
}
}  // namespace

TEST_CASE("missione TORPEX '89: caricamento e 30 minuti di gioco" * doctest::skip(!ci_sono_missioni())) {
    Missione m(DIR + "/SCENE1A.lua", 1);
    CHECK(m.id() == "SCENE1A");
    CHECK(m.titolo() == "TORPEX '89");
    CHECK(m.teatro() == "faroe");
    REQUIRE(m.giocatore() != nullptr);
    CHECK(m.giocatore()->classe == "los_angeles");
    CHECK(m.unita().size() >= 5);
    for (int i = 0; i < 900; ++i) m.avanza(2.0);
    CHECK(m.tempo() == doctest::Approx(1800.0));
    INFO(elenco(m.errori()));
    CHECK(m.errori().empty());
    std::string mancanti;
    for (const auto& n : m.non_implementate()) mancanti += n + " ";
    INFO(mancanti);
    CHECK(m.non_implementate().empty());
}

TEST_CASE("missioni deterministiche con lo stesso seme" * doctest::skip(!ci_sono_missioni())) {
    auto gioca = [](std::uint64_t seme) {
        Missione m(DIR + "/SCENE1A.lua", seme);
        for (int i = 0; i < 300; ++i) m.avanza(2.0);
        std::string s = std::to_string(m.variante()) + "/" + std::to_string(m.messaggi().size());
        for (const auto& [id, u] : m.unita()) s += "/" + std::to_string(id) + ":" + std::to_string(u.pos.x) + "," + std::to_string(u.pos.y);
        return s;
    };
    CHECK(gioca(7) == gioca(7));
}

TEST_CASE("tutte le scene convertite girano senza errori" * doctest::skip(!ci_sono_missioni())) {
    int n = 0;
    for (const auto& e : fs::directory_iterator(DIR)) {
        if (e.path().extension() != ".lua") continue;
        ++n;
        Missione m(e.path().string(), 3);
        for (int i = 0; i < 450; ++i) m.avanza(2.0);   // 15 minuti
        INFO(e.path().filename().string() << "\n" << elenco(m.errori()));
        CHECK(m.errori().empty());
        CHECK(m.non_implementate().empty());
    }
    CHECK(n == 22);
}

TEST_CASE("ambiente isolato: niente io, os, load") {
    const auto f = fs::temp_directory_path() / "m688_prova_isolamento.lua";
    for (const char* codice : {"return io.open('x')", "return os.exit(1)", "return load('return 1')()",
                               "return require('os')", "return math.random()"}) {
        std::ofstream(f) << codice;
        CHECK_THROWS(Missione(f.string(), 1));
    }
    fs::remove(f);
}

// --- fondale ---------------------------------------------------------------------------------------------------

namespace {
// Fondale di prova: 100 x 100 celle da 100 m (10 km di lato, centro in 0,0); profondita' 200 m a ovest di
// x = 0, poi il fondo sale fino alla costa (x = 3000 m) e oltre c'e' terra a +10 m.
std::shared_ptr<const Fondale> fondale_di_prova() {
    std::vector<std::uint16_t> v(100 * 100);
    for (int r = 0; r < 100; ++r)
        for (int c = 0; c < 100; ++c) {
            const double x = (c + 0.5) * 100.0 - 5000.0;
            const double z = x < 0.0 ? -200.0 : std::min(10.0, -200.0 + x * 210.0 / 3000.0);
            v[static_cast<std::size_t>(r) * 100 + c] = static_cast<std::uint16_t>((z + 11000.0) / 0.25);
        }
    return std::make_shared<const Fondale>(100, 100, 100.0, 0.25, -11000.0, std::move(v));
}

// Missione minima con le unita' date (righe Lua della tabella `unita`)
std::string missione_di_prova(const std::string& unita, const std::string& script = {}, const std::string& timer = {}) {
    const auto f = fs::temp_directory_path() / "m688_prova_fondale.lua";
    std::ofstream(f) << "return { id = 'PROVA', teatro = 'prova', script = {" << script
                     << "}, varianti = { { probabilita = 1, unita = {\n" << unita << "\n}, timer = {" << timer << "} } } }\n";
    return f.string();
}

const std::string MODELLO = "modello = { massima_kn = 40, virata_gradi_s = 3, accel_kn_s = 1, quota_m_s = 5, quota_max_m = 450, integrita = 200 }";
}  // namespace

TEST_CASE("fondale: l'IA tiene il franco e aggira la costa, il giocatore si incaglia") {
    const auto file = missione_di_prova(
        "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true,"
        "  posizione = { x = -1000, y = -3000 }, quota_m = 50, rotta = 90, velocita_kn = 30, " + MODELLO + " },"
        "{ id = 1, classe = 'victor3', categoria = 'sottomarino', flag = 17, posizione = { x = -1000, y = 0 },"
        "  quota_m = 150, rotta = 90, velocita_kn = 15, " + MODELLO + " },"
        "{ id = 2, classe = 'krivak2', categoria = 'nave', flag = 18, posizione = { x = -1000, y = 3000 },"
        "  quota_m = 0, rotta = 90, velocita_kn = 20, " + MODELLO + " },");
    Missione m(file, 1, false);
    m.imposta_fondale(fondale_di_prova(), {{"los_angeles", 9.4}, {"victor3", 7.0}, {"krivak2", 7.3}});
    bool ia_incagliata = false;
    double franco_min = 1e9;
    for (int i = 0; i < 9 * 300; ++i) {
        m.avanza(1.0 / 9.0);
        for (const auto& [id, u] : m.unita()) {
            if (id == 0) continue;
            ia_incagliata |= u.incagliata;
            const double fondo = m.profondita(u.pos);
            if (id == 1 && std::isfinite(fondo)) franco_min = std::min(franco_min, fondo - u.quota);
            CHECK(fondo >= std::max(u.quota, m.pescaggio(u)));   // mai sotto il fondo
        }
    }
    CHECK_FALSE(ia_incagliata);
    CHECK(franco_min >= 6.5);   // 25 piedi di franco, meno l'inseguimento della quota
    // il giocatore (30 nodi verso la costa, senza IA) si e' incagliato: messaggio 30 e danni pari al 36%
    // dell'integrita' (15,4 m/s su 20); poi resta fermo
    REQUIRE(m.giocatore() != nullptr);
    const auto& g = *m.giocatore();
    CHECK(g.incagliata);
    CHECK(g.integrita == 200 - static_cast<int>(std::ceil(200 * std::pow(30 * NODO / 20.0, 2))));
    CHECK(m.profondita(g.pos) >= g.quota);
    bool msg30 = false;
    for (const auto& x : m.messaggi()) msg30 |= x.codice == 30;
    CHECK(msg30);
    CHECK_FALSE(m.esito());
    fs::remove(file);
}

TEST_CASE("fondale: discesa sul fondo e scafo che cede (esito 1)") {
    // giocatore fermo a 150 m che scende a 400 m in 200 m d'acqua: si appoggia sul fondo a 5 m/s (urto lieve)
    auto file = missione_di_prova(
        "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true,"
        "  posizione = { x = -3000, y = 0 }, quota_m = 150, rotta = 0, velocita_kn = 0, " + MODELLO + " },",
        "S = function(u) m688.quota(u, 400, 1) m688.salva_modello(u) end", "{ dopo_s = 0, unita = 0, gestore = 'S' }");
    {
        Missione m(file, 1, false);
        m.imposta_fondale(fondale_di_prova());
        for (int i = 0; i < 9 * 30; ++i) m.avanza(1.0 / 9.0);
        REQUIRE(m.giocatore() != nullptr);
        CHECK(m.giocatore()->quota == doctest::Approx(200.0));
        CHECK(m.giocatore()->sul_fondo);
        CHECK(m.giocatore()->integrita == 200 - static_cast<int>(std::ceil(200 * 0.0625)));   // (5/20)^2
        REQUIRE(m.messaggi().size() == 1);   // un solo urto
        CHECK(m.messaggi()[0].codice == 30);
    }
    // a 45 nodi contro la costa lo scafo cede: messaggio 9 ed esito 1 ("si e' incagliato")
    file = missione_di_prova(
        "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true,"
        "  posizione = { x = 1000, y = 0 }, quota_m = 60, rotta = 90, velocita_kn = 45, " + MODELLO + " },");
    Missione m(file, 1, false);
    m.imposta_fondale(fondale_di_prova());
    for (int i = 0; i < 9 * 60 && !m.esito(); ++i) m.avanza(1.0 / 9.0);
    CHECK(m.esito() == 1);
    CHECK(m.giocatore() == nullptr);
    std::string codici;
    for (const auto& x : m.messaggi()) codici += std::to_string(x.codice) + " ";
    CHECK(codici == "9 ");
    fs::remove(file);
}

TEST_CASE("cadenza della scoperta: un osservatore per passo, flag 40h e TYPE") {
    // lista dell'originale: 3, 2, 1, 0 (le nuove in testa). La 2 (flag 40h) non osserva mai; la 3 (TYPE 2) osserva
    // un giro si' e uno no: passi 1, 4, 6, 9, ... la 1, 2, 5, 7, 10, ... il giocatore, 3, 8, 13, ... la 3
    const auto file = missione_di_prova(
        "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true,"
        "  posizione = { x = 0, y = 0 }, quota_m = 0, rotta = 0, velocita_kn = 0, " + MODELLO + " },"
        "{ id = 1, classe = 'krivak2', categoria = 'nave', flag = 18, posizione = { x = 300, y = 0 }, quota_m = 0, " + MODELLO + " },"
        "{ id = 2, classe = 'krivak2', categoria = 'nave', flag = 82, posizione = { x = 0, y = 300 }, quota_m = 0, " + MODELLO + " },"
        "{ id = 3, classe = 'krivak2', categoria = 'nave', flag = 18, tipo = 2, posizione = { x = 300, y = 300 }, quota_m = 0, " + MODELLO + " },");
    Missione m(file, 1, false);
    std::map<int, int> osservazioni;
    std::string primi;
    for (int passo = 1; passo <= 90; ++passo) {
        m.avanza(1.0 / 9.0);
        int osservatori = 0;
        for (const int id : {1, 3}) {
            const auto& c = m.unita().at(id).contatti;
            const bool ora = c.count(2) && c.at(2).ultima_scoperta == m.tempo();   // la nave scopre sempre la nave vicina
            osservazioni[id] += ora;
            osservatori += ora;
            if (ora && passo <= 8) primi += std::to_string(passo) + ":" + std::to_string(id) + " ";
        }
        CHECK(osservatori <= 1);
        CHECK(m.unita().at(2).contatti.empty());
    }
    CHECK(primi == "1:1 3:3 4:1 6:1 8:3 ");
    CHECK(osservazioni[1] == 36);
    CHECK(osservazioni[3] == 18);
    fs::remove(file);
}

TEST_CASE("visite: un'unita' per passo nell'ordine della lista, dalla testa dopo un'uscita") {
    // lista 4, 3, 2, 1, 0 (0 e 1 sono i giocatori, che non escono). La 3 e la 2, distrutte al passo 0, sono visitate
    // ai passi 2, 7, ... e 3, 8, ...: la 3 ha la diciottesima visita al passo 87 ed esce, le visite ripartono dalla
    // testa (la 4 al passo 88) e la 2 ha la diciottesima al passo 89 invece che all'88
    std::string unita;
    for (int id = 0; id < 5; ++id)
        unita += "{ id = " + std::to_string(id) + ", classe = 'krivak2', categoria = 'nave', flag = " + (id ? "18" : "9") +
                 (id ? "" : ", giocatore = true") + ", posizione = { x = " + std::to_string(id * 5000) + ", y = 0 }, quota_m = 0, " + MODELLO + " },";
    const auto file = missione_di_prova(unita);
    Missione m(file, 1, false);
    m.distruggi(3);
    m.distruggi(2);
    std::map<int, int> uscita;
    for (int passo = 1; passo <= 100; ++passo) {
        m.avanza(1.0 / 9.0);
        for (const int id : {2, 3})
            if (!m.unita().count(id) && !uscita.count(id)) uscita[id] = passo;
    }
    CHECK(uscita[3] == 87);
    CHECK(uscita[2] == 89);
    fs::remove(file);
}

TEST_CASE("dotazione: le scorte iniziali per tipo d'arma dalla tabella della missione" * doctest::skip(!ci_sono_missioni())) {
    Missione m(DIR + "/SCENE1A.lua", 1);
    const std::map<std::string, int> attesa{{"mk48", 22}, {"harpoon", 2}, {"esca", 6}};
    CHECK(m.dotazione() == attesa);
}

TEST_CASE("contatti: chi esce dal gioco si toglie dalle liste (003C, 127B) e la sua lettera torna libera") {
    // il giocatore e' una nave (scopre in automatico chi e' in superficie) e da' le lettere ai contatti. La 2 viene
    // distrutta ed esce dal gioco: il suo contatto resta nelle liste finche' una valutazione non lo incontra prima del
    // contatto del bersaglio (in fondo alla lista, solo con un bersaglio nuovo). Nasce la 4: le liste si puliscono e
    // la 4 prende la lettera che era della 2
    std::string unita;
    for (int id = 0; id < 4; ++id)
        unita += "{ id = " + std::to_string(id) + ", classe = 'krivak2', categoria = 'nave', flag = " + (id ? "18" : "10") +
                 (id ? "" : ", giocatore = true") + ", posizione = { x = " + std::to_string((id % 2) * 300) + ", y = " +
                 std::to_string((id / 2) * 300) + " }, quota_m = 0, " + MODELLO + " },";
    const auto file = missione_di_prova(unita, "C = function(u) m688.nuova_unita(u) end", "{ dopo_s = 30, unita = 3, gestore = 'C' }");
    Missione m(file, 1, false);
    for (int i = 0; i < 9 * 5; ++i) m.avanza(1.0 / 9.0);
    std::string lettere;
    for (const auto& [id, c] : m.giocatore()->contatti) lettere += c.lettera;
    std::sort(lettere.begin(), lettere.end());
    CHECK(lettere == "ABC");
    const char della_2 = m.giocatore()->contatti.at(2).lettera;
    m.distruggi(2);
    for (int i = 0; i < 9 * 25; ++i) m.avanza(1.0 / 9.0);   // 18 visite a 4 unita' per giro: esce entro 9 s
    CHECK(m.unita().count(2) == 0);
    for (int i = 0; i < 9 * 10; ++i) m.avanza(1.0 / 9.0);   // a 30 s nasce la 4
    for (const auto& [id, u] : m.unita()) CHECK(u.contatti.count(2) == 0);
    REQUIRE(m.unita().count(4) == 1);
    REQUIRE(m.giocatore()->contatti.count(4) == 1);
    CHECK(m.giocatore()->contatti.at(4).lettera == della_2);
    fs::remove(file);
}

TEST_CASE("lampeggio dei contatti deboli: fasi nascoste che si accorciano e visibili che si allungano (2011:1106)") {
    // segnale 480: n = 70/8 + 1 = 9. Nascosto 9, visibile 1, nascosto 9, visibile 2, nascosto 8, ... nascosto 2,
    // poi visibile per sempre
    Contatto c;
    c.lampeggio_da_capo(480.0);
    std::string fasi;
    for (int i = 0; i < 120; ++i) {
        c.passo_lampeggio();
        fasi += c.visibile() ? 'V' : 'N';
    }
    std::string attese;
    for (int k = 0; k < 8; ++k) attese += std::string(k ? 9 - k + 1 : 9, 'N') + std::string(k + 1, 'V');
    attese += std::string(2, 'N');
    attese += std::string(120 - attese.size(), 'V');
    CHECK(fasi == attese);
    CHECK(c.lampeggio == 0);
    c.qualita = 800.0;
    c.lampeggio_da_capo(480.0);
    CHECK(c.visibile());   // la traccia solida si vede anche col bit 80h
}

TEST_CASE("lampeggio nella missione: contatto debole e fermo nascosto, poi visibile; niente bersaglio sotto 500") {
    // nave ferma con sonar 12 e un sottomarino fermo (rumore 1) a 1100 m di prua: T = 3 + (1 + 40) x 12 = 495,
    // segnale ~480. Il contatto nasce nascosto con qualita' 0 e, a segnale costante, lampeggia per 89 valutazioni
    const std::string debole = "modello = { massima_kn = 30, virata_gradi_s = 3, accel_kn_s = 1, quota_m_s = 5, quota_max_m = 450,"
                               " integrita = 200, sonar = 12, rumore = 1 }";
    const auto file = missione_di_prova(
        "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true,"
        "  posizione = { x = -30000, y = -30000 }, quota_m = 50, rotta = 0, velocita_kn = 0, " + MODELLO + " },"
        "{ id = 1, classe = 'krivak2', categoria = 'nave', flag = 18, posizione = { x = 0, y = 0 }, quota_m = 0, rotta = 0,"
        "  velocita_kn = 0, " + debole + ", gestori = { bersaglio = 'B' } },"
        "{ id = 2, classe = 'victor3', categoria = 'sottomarino', flag = 9, posizione = { x = 0, y = 1100 }, quota_m = 100,"
        "  rotta = 0, velocita_kn = 0, " + debole + " },",
        "B = function(u) end");
    Missione m(file, 1, false);
    int cambi = 0;
    bool visto = false, primo = true;
    for (int i = 0; i < 9 * 120; ++i) {
        m.avanza(1.0 / 9.0);
        const auto& cc = m.unita().at(1).contatti;
        if (!cc.count(2)) continue;
        const Contatto& c = cc.at(2);
        if (primo) { CHECK_FALSE(c.visibile()); CHECK(c.segnale < 500.0); CHECK(c.segnale > 470.0); primo = false; }
        cambi += c.visibile() != visto;
        visto = c.visibile();
        CHECK(c.qualita == 0.0);
    }
    CHECK_FALSE(primo);
    CHECK(visto);                // dopo 360 osservazioni il lampeggio e' finito
    CHECK(cambi >= 5);           // al piu' 9 accensioni e 8 spegnimenti: le fasi brevi possono cadere in un passo solo
    CHECK(cambi % 2 == 1);
    CHECK(m.unita().at(1).bersaglio < 0);   // con segnale <= 500 la scelta del bersaglio non avanza
    fs::remove(file);
}

TEST_CASE("livello ESM (DS:476C): ultima intercettazione sul giocatore, a 0 coi sensori giu' e quando un'unita' esce") {
    // una nave a 1000 m dal giocatore: 108 quarti, livello 85 - 108/32 = 82. Periscopio su a 0 s, giu' a 10 s, su a
    // 20 s; a 25 s la nave si porta fuori portata (il livello resta), a 30 s la 2 (lontana) esce dal gioco
    const auto file = missione_di_prova(
        "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true,"
        "  posizione = { x = 0, y = 0 }, quota_m = 5, rotta = 0, velocita_kn = 0, " + MODELLO + " },"
        "{ id = 1, classe = 'krivak2', categoria = 'nave', flag = 18, posizione = { x = 1000, y = 0 }, quota_m = 0, " + MODELLO + " },"
        "{ id = 2, classe = 'krivak2', categoria = 'nave', flag = 18, posizione = { x = 60000, y = 60000 }, quota_m = 0, " + MODELLO + " },",
        "SU = function(u) m688.periscopio(u, true) m688.salva_modello(u) end,"
        "GIU = function(u) m688.periscopio(u, false) m688.salva_modello(u) end,"
        "VIA = function(u) m688.togli_unita(u) end,"
        "LONTANO = function(u) m688.posizione(u, 30000, 0) m688.salva_modello(u) end",
        "{ dopo_s = 0, unita = 0, gestore = 'SU' }, { dopo_s = 10, unita = 0, gestore = 'GIU' },"
        "{ dopo_s = 20, unita = 0, gestore = 'SU' }, { dopo_s = 25, unita = 1, gestore = 'LONTANO' },"
        "{ dopo_s = 30, unita = 2, gestore = 'VIA' }");
    Missione m(file, 1, false);
    CHECK(m.livello_esm() == 0);
    m.avanza(5.0);
    CHECK(m.livello_esm() == 82);
    m.avanza(10.0);
    CHECK(m.livello_esm() == 0);    // periscopio giu' a 10 s
    m.avanza(10.0);
    CHECK(m.livello_esm() == 82);
    m.avanza(4.0);
    CHECK(m.livello_esm() == 82);   // a 29 s la nave e' lontana: nessun decadimento
    for (int i = 0; i < 9 * 10 && m.unita().count(2); ++i) m.avanza(1.0 / 9.0);
    REQUIRE(m.unita().count(2) == 0);
    CHECK(m.livello_esm() == 0);    // 003C: azzerato all'uscita, anche se il giocatore ha ancora il periscopio su
    fs::remove(file);
}

TEST_CASE("scia della carta (12EE): il giocatore ogni 30 suoi turni e, in quel turno, i contatti solidi") {
    // lista 1, 0: il giocatore osserva un passo si' e uno no e valuta due bersagli, cioe' la nave due volte. Col
    // periscopio alzato la vede in automatico (traccia solida subito): al suo trentesimo turno (passo 60) la scia
    // ha il giocatore e due volte la nave, nella posizione vera
    const auto file = missione_di_prova(
        "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true,"
        "  posizione = { x = 0, y = 0 }, quota_m = 5, rotta = 0, velocita_kn = 0, " + MODELLO + " },"
        "{ id = 1, classe = 'krivak2', categoria = 'nave', flag = 18, posizione = { x = 3000, y = 2000 }, quota_m = 0, " + MODELLO + " },",
        "SU = function(u) m688.periscopio(u, true) m688.salva_modello(u) end", "{ dopo_s = 0, unita = 0, gestore = 'SU' }");
    Missione m(file, 1, false);
    int passo = 0;
    while (m.scia().empty() && passo < 200) { m.avanza(1.0 / 9.0); ++passo; }
    REQUIRE(m.scia().size() == 3);
    CHECK(passo >= 59);
    CHECK(passo <= 61);
    CHECK(m.scia()[0].id == 0);
    CHECK(m.scia()[1].id == 1);
    CHECK(m.scia()[2].id == 1);
    CHECK(m.scia()[1].pos.x == doctest::Approx(3000.0));
    CHECK(m.scia()[1].pos.y == doctest::Approx(2000.0));
    for (int i = 0; i < 59; ++i) m.avanza(1.0 / 9.0);
    CHECK(m.scia().size() == 3);    // fino al prossimo trentesimo turno niente
    m.avanza(400.0);                // 60 giri da tre punti: l'anello si ferma a 150
    CHECK(m.scia().size() == 150);
    fs::remove(file);
}

TEST_CASE("fondale reale: l'IA delle scene originali non urta il fondo" *
          doctest::skip(!ci_sono_missioni() || !fs::exists(M688_DATI "/teatri/faroe.fdl"))) {
    const auto classi = carica_classi(M688_DATI "/classi.toml");
    std::map<std::string, double> pescaggi;
    for (const auto& [id, c] : classi) pescaggi[id] = c.pescaggio_m;
    std::map<std::string, std::shared_ptr<const Fondale>> fondali;
    for (const auto& e : fs::directory_iterator(DIR)) {
        if (e.path().extension() != ".lua") continue;
        Missione m(e.path().string(), 3);
        auto& f = fondali[m.teatro()];
        if (!f) f = std::make_shared<const Fondale>(Fondale::carica(M688_DATI "/teatri/" + m.teatro() + ".fdl"));
        m.imposta_fondale(f, pescaggi);
        // urti col fondo con danni (sopra 1 m/s): l'IA puo' solo sfiorarlo a macchine ferme
        std::set<int> incagliate, sotto;
        std::map<int, int> integrita;
        for (int i = 0; i < 9 * 900; ++i) {   // 15 minuti
            m.avanza(1.0 / 9.0);
            for (const auto& [id, u] : m.unita()) {
                if (u.giocatore || (u.categoria != "nave" && u.categoria != "sottomarino")) continue;
                const auto it = integrita.find(id);
                if ((u.incagliata || u.sul_fondo) && it != integrita.end() && u.integrita < it->second) incagliate.insert(id);
                integrita[id] = u.integrita;
                if (m.profondita(u.pos) < u.quota - 0.01) sotto.insert(id);
            }
        }
        std::string s;
        for (int id : incagliate) s += std::to_string(id) + " ";
        INFO(e.path().filename().string() << ": incagliate " << s);
        CHECK(incagliate.empty());
        CHECK(sotto.empty());
        CHECK(m.errori().empty());
    }
}

// --- rapide v2 e libreria (docs/17 §3): condizioni, obiettivo «contatto», limiti degli script ---------------------

namespace {
// il 688 fermo a 30 m e un Victor III a 3 km a nord; `script` e `timer` come nelle missioni, `extra` nella tabella
std::string scena_v2(const std::string& nome, const std::string& script, const std::string& timer, const std::string& extra = "") {
    const auto f = fs::temp_directory_path() / ("m688_prova_v2_" + nome + ".lua");
    const std::string modello = "modello = { massima_kn = 32, virata_gradi_s = 1, accel_kn_s = 0.5, quota_m_s = 1, quota_max_m = 450, integrita = 200, rumore = 8, sonar = 19 }";
    std::ofstream(f) << "return { id = 'V2', teatro = 'faroe', " << extra << " script = {" << script << "}, varianti = { { probabilita = 1, unita = {\n"
                        "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true,"
                        "  posizione = { x = 0, y = 0 }, quota_m = 30, rotta = 0, velocita_kn = 0, " << modello << " },"
                        "{ id = 2, classe = 'victor3', categoria = 'sottomarino', flag = 25, posizione = { x = 0, y = 3000 },"
                        "  quota_m = 150, rotta = 90, velocita_kn = 5, " << modello << " },"
                        "\n}, timer = { " << timer << " } } } }\n";
    return f.string();
}
int var(const Missione& m, int i) { const auto it = m.variabili().find(i); return it == m.variabili().end() ? -1 : it->second; }
}  // namespace

TEST_CASE("rapide v2: condizioni sull'unita' (cerchio, velocita', quota, superficie, alberi)") {
    const auto f = scena_v2("condizioni",
        "C = function(u) local function b(x) return x and 1 or 0 end "
        "m688.variabile(u, 1, b(m688.in_cerchio(u, 0, 0, 100))) m688.variabile(u, 2, b(m688.in_cerchio(u, 5000, 0, 100))) "
        "m688.variabile(u, 3, b(m688.velocita_sotto(u, 3))) m688.variabile(u, 4, b(m688.quota_entro(u, 10, 50))) "
        "m688.variabile(u, 5, b(m688.in_superficie(u))) m688.variabile(u, 6, b(m688.alberi_alzati(u))) "
        "m688.periscopio(u, true) m688.variabile(u, 7, b(m688.alberi_alzati(u))) end",
        "{ dopo_s = 5, unita = 0, gestore = 'C' }");
    Missione m(f, 1);
    for (int i = 0; i < 9 * 12; ++i) m.avanza(1.0 / 9.0);
    CHECK(m.errori().empty());
    CHECK(var(m, 1) == 1);
    CHECK(var(m, 2) == 0);
    CHECK(var(m, 3) == 1);
    CHECK(var(m, 4) == 1);
    CHECK(var(m, 5) == 0);
    CHECK(var(m, 6) == 0);
    CHECK(var(m, 7) == 1);
    fs::remove(f);
}

TEST_CASE("rapide v2: obiettivo «contatto», obiettivo_compiuto, contatto_da e senza_contatto_da") {
    const auto f = scena_v2("contatto",
        "C = function(u) m688.variabile(u, 1, m688.obiettivo_compiuto(u, 'ascolta') and 1 or 0) "
        "m688.variabile(u, 2, m688.contatto_da(u, 2, 1) and 1 or 0) m688.variabile(u, 3, m688.senza_contatto_da(u, 2, 1) and 1 or 0) "
        "m688.variabile(u, 4, m688.obiettivo_compiuto(u, 'inesistente') and 1 or 0) end",
        "{ dopo_s = 600, unita = 0, gestore = 'C' }",
        "obiettivi = { { id = 'ascolta', tipo = 'contatto', unita = { 2 }, minuti = 1, testo = 'Un minuto di contatto' } },");
    Missione m(f, 1);
    for (int i = 0; i < 9 * 605; ++i) m.avanza(1.0 / 9.0);
    REQUIRE(m.errori().empty());
    REQUIRE(m.contatto_cumulato_s(0, 2) >= 60.0);   // il Victor a 3 km si sente per tutti i 10 minuti
    const auto e = m.esito_dettagliato();
    const auto o = std::find_if(e.obiettivi.begin(), e.obiettivi.end(), [](const Obiettivo& x) { return x.id == "ascolta"; });
    REQUIRE(o != e.obiettivi.end());
    CHECK(o->tipo == Obiettivo::Tipo::contatto);
    CHECK(o->stato == Obiettivo::Stato::raggiunto);
    CHECK(var(m, 1) == 1);
    CHECK(var(m, 2) == 1);
    CHECK(var(m, 3) == (m.senza_contatto_s(0, 2) >= 60.0 ? 1 : 0));
    CHECK(var(m, 4) == 0);
    fs::remove(f);
}

TEST_CASE("rapide v2: ping del giocatore vicino a un'unita', lancio di un'unita' ostile") {
    const auto f = scena_v2("ping",
        "P = function(u) m688.impulso_sonar(u) end, "
        "Q = function(u) m688.variabile(u, 1, m688.ping_giocatore_entro(u, 5000) and 1 or 0) "
        "m688.variabile(u, 2, m688.ping_giocatore_entro(u, 1000) and 1 or 0) m688.variabile(u, 3, m688.ha_lanciato(u) and 1 or 0) end",
        "{ dopo_s = 2, unita = 0, gestore = 'P' }, { dopo_s = 60, unita = 2, gestore = 'Q' }");
    Missione m(f, 1);
    for (int i = 0; i < 9 * 62; ++i) m.avanza(1.0 / 9.0);
    CHECK(m.errori().empty());
    CHECK(var(m, 1) == 1);   // il Victor e' a 3 km dal ping
    CHECK(var(m, 2) == 0);
    CHECK(var(m, 3) == 0);   // nessuno ha lanciato
    fs::remove(f);
}

// la simmetria del ping (inventario della simmetria dell'IA, §2): le condizioni sui ping di qualunque unita', anche nel
// classico (il PING degli script)
TEST_CASE("script: m688.ha_pingato e m688.ping_entro valgono per i ping di qualunque unita'") {
    const auto f = scena_v2("ping_ia",
        "P = function(u) m688.impulso_sonar(u) end, "
        "Q = function(u) m688.variabile(u, 1, m688.ha_pingato(u, 2) and 1 or 0) m688.variabile(u, 2, m688.ha_pingato(u, -1) and 1 or 0) "
        "m688.variabile(u, 3, m688.ha_pingato(u, 0) and 1 or 0) m688.variabile(u, 4, m688.ha_pingato(u, 2, 10) and 1 or 0) "
        "m688.variabile(u, 5, m688.ping_entro(u, 5000) and 1 or 0) m688.variabile(u, 6, m688.ping_entro(u, 1000) and 1 or 0) "
        "m688.variabile(u, 7, m688.ping_giocatore_entro(u, 5000) and 1 or 0) end",
        "{ dopo_s = 2, unita = 2, gestore = 'P' }, { dopo_s = 60, unita = 0, gestore = 'Q' }");
    Missione m(f, 1);
    for (int i = 0; i < 9 * 62; ++i) m.avanza(1.0 / 9.0);
    CHECK(m.errori().empty());
    CHECK(var(m, 1) == 1);   // il Victor ha pingato
    CHECK(var(m, 2) == 1);   // qualcuno ha pingato
    CHECK(var(m, 3) == 0);   // il giocatore no
    CHECK(var(m, 4) == 0);   // non negli ultimi 10 s
    CHECK(var(m, 5) == 1);   // a 3 km dal giocatore
    CHECK(var(m, 6) == 0);
    CHECK(var(m, 7) == 0);   // ping_giocatore_entro resta del giocatore
    fs::remove(f);
}

// m688.distanza_da (trailer-1, la S14): i metri veri fra l'unita' e un'altra, nil se non c'e'
TEST_CASE("script: m688.distanza_da la distanza vera fra due unita'") {
    const auto f = scena_v2("distanza_da",
        "Q = function(u) local d = m688.distanza_da(u, 2) m688.variabile(u, 1, d and math.floor(d) or -1)"
        " m688.variabile(u, 2, m688.distanza_da(u, 999) == nil and 1 or 0) end",
        "{ dopo_s = 2, unita = 0, gestore = 'Q' }");
    Missione m(f, 1);
    for (int i = 0; i < 9 * 3; ++i) m.avanza(1.0 / 9.0);
    CHECK(m.errori().empty());
    const double vera = distanza(m.unita().at(0).pos, m.unita().at(2).pos);
    CHECK(std::abs(var(m, 1) - vera) < 20.0);   // al passo dello script (pochi metri di moto)
    CHECK(var(m, 2) == 1);                       // un'unita' che non c'e': nil
    fs::remove(f);
}

TEST_CASE("rapide v2: i limiti degli script fermano la chiamata, la missione continua") {
    const auto f = scena_v2("limiti",
        "G = function(u) while true do end end, "
        "H = function(u) local t = {} for i = 1, 10000000 do t[i] = i end end, "
        "K = function(u) m688.variabile(u, 1, 1) end",
        "{ dopo_s = 2, unita = 0, gestore = 'G' }, { dopo_s = 3, unita = 0, gestore = 'H' }, { dopo_s = 4, unita = 0, gestore = 'K' }");
    LimitiScript l;
    l.istruzioni_per_chiamata = 1000000;
    l.memoria_byte = 8u << 20;
    Missione m(f, 1, l);
    for (int i = 0; i < 9 * 6; ++i) m.avanza(1.0 / 9.0);
    REQUIRE(m.errori().size() == 2);
    CHECK(m.errori()[0].find("troppe istruzioni") != std::string::npos);
    CHECK(m.errori()[1].find("memory") != std::string::npos);
    CHECK(var(m, 1) == 1);   // il gestore dopo gli errori gira
    // senza limiti la stessa missione si carica come prima
    Missione libera(f, 1);
    CHECK(libera.errori().empty());
    fs::remove(f);
}

TEST_CASE("esche semoventi: il tubo del giocatore lancia la MOSS con la rotta e la quota impostate") {
    const auto f = scena_v2("moss", "", "");
    Missione m(f, 1);
    m.siluri_enhanced(M688_DATI);
    REQUIRE(m.con_siluri_enhanced());
    m.imposta_tubi({"mk48", "mk48", "mk48", "mk48", "esca", "esca", "harpoon"});
    m.variante_tubo(1, "moss_mk70");
    ImpostazioniSiluro imp;
    imp.rotta = 135.0;
    imp.quota_corsa_m = 80.0;
    m.imposta_siluro_giocatore(imp);
    REQUIRE(m.applica({m.tempo(), TipoOrdine::lancia, 0, 0, 1}));
    int esca = -1;
    for (const auto& [id, u] : m.unita()) if (m.esca_mobile(id)) esca = id;
    REQUIRE(esca >= 0);
    const UnitaClassica& e = m.unita().at(esca);
    CHECK(e.lanciatore == 0);
    CHECK(e.rotta == doctest::Approx(135.0));
    CHECK(e.quota_ordinata == doctest::Approx(80.0));
    CHECK(m.esito_dettagliato().per_arma.at("moss_mk70").lanci == 1);
    for (const auto& [id, u] : m.unita()) CHECK(m.siluro(id) == nullptr);   // nessun siluro: l'esca e' sua
    // senza impostazioni: la rotta e la quota del battello
    m.variante_tubo(2, "moss_mk70");
    REQUIRE(m.applica({m.tempo(), TipoOrdine::lancia, 0, 0, 2}));
    int seconda = -1;
    for (const auto& [id, u] : m.unita()) if (m.esca_mobile(id) && id != esca) seconda = id;
    REQUIRE(seconda >= 0);
    CHECK(m.unita().at(seconda).rotta == doctest::Approx(m.giocatore()->rotta));
    CHECK(m.unita().at(seconda).quota_ordinata == doctest::Approx(m.giocatore()->quota));
    fs::remove(f);
}

TEST_CASE("esche semoventi: l'Alfa del giocatore carica una MG-74 dalla rastrelliera e la lancia dal tubo") {
    const auto f = fs::temp_directory_path() / "m688_prova_mg74_alfa.lua";
    const std::string modello = "modello = { massima_kn = 42, virata_gradi_s = 1, accel_kn_s = 0.5, quota_m_s = 1, quota_max_m = 700, integrita = 200, rumore = 8, sonar = 19 }";
    std::ofstream(f.string()) << "return { id = 'MG74', teatro = 'barents', script = {}, varianti = { { probabilita = 1, unita = {\n"
                                 "{ id = 0, classe = 'alfa', categoria = 'sottomarino', flag = 17, giocatore = true,"
                                 "  posizione = { x = 0, y = 0 }, quota_m = 120, rotta = 45, velocita_kn = 6, " << modello << " },"
                                 "\n}, timer = {} } } }\n";
    const CatalogoCarico cat = carica_carico(M688_DATI "/carico.toml");
    Missione m(f.string(), 1);
    m.siluri_enhanced(M688_DATI);
    StazioniMissione st(m);
    LimitiMissione la;
    la.deposito = {{"type53", 18}, {"esca_mobile", 2}, {"esca", 4}};
    la.tipo_missione = "antisom";
    SalaSiluri sala(cat, "alfa", la);
    sala.carico_iniziale();
    REQUIRE(sala.carico().rastrelliera.at("mg74") == 2);
    st.imposta_carico(m, sala.carico(), cat);
    REQUIRE(st.consegna(m, st.armi().scegli_arma(1, "mg74", m.tempo())));
    for (int i = 0; i < 9 * 120 && st.armi().stato(1) != StatoTubo::in_armamento; ++i) {
        m.avanza(1.0 / 9.0);
        st.dopo_passo(m);
    }
    REQUIRE(st.armi().stato(1) == StatoTubo::in_armamento);
    st.armi().messaggio_missione(10);   // il tubo 1 armato (9 + tubo: lo script dell'originale)
    REQUIRE(st.armi().stato(1) == StatoTubo::armato);
    REQUIRE(st.consegna(m, {m.tempo(), TipoOrdine::lancia, 0, 0, 1}));
    int esca = -1;
    for (const auto& [id, u] : m.unita()) if (m.esca_mobile(id)) esca = id;
    REQUIRE(esca >= 0);
    CHECK(m.unita().at(esca).classe == "mg74");
    CHECK(m.unita().at(esca).lanciatore == 0);
    CHECK(m.unita().at(esca).rotta == doctest::Approx(45.0));   // tubo non impostato: la rotta del battello
    CHECK(m.esito_dettagliato().per_arma.at("mg74").lanci == 1);
    for (const auto& [id, u] : m.unita()) CHECK(m.siluro(id) == nullptr);
    fs::remove(f);
}

TEST_CASE("reti a strascico: dietro il peschereccio lento, sul fondo, nella scena acustica") {
    const auto file = missione_di_prova(
        "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true, posizione = { x = -4000, y = 3000 },"
        "  quota_m = 60, rotta = 0, velocita_kn = 0, " + MODELLO + " },"
        "{ id = 2, classe = 'peschereccio', categoria = 'nave', flag = 2, posizione = { x = -3000, y = 0 }, quota_m = 0, rotta = 90,"
        "  velocita_kn = 3, " + MODELLO + " },"
        "{ id = 3, classe = 'peschereccio', categoria = 'nave', flag = 2, posizione = { x = -3000, y = -2000 }, quota_m = 0, rotta = 90,"
        "  velocita_kn = 9, " + MODELLO + " }");
    Missione m(file, 1, false);
    m.imposta_fondale(fondale_di_prova());
    m.acustica_enhanced(M688_DATI);
    REQUIRE(m.con_acustica());
    REQUIRE(m.catalogo_acustico()->sorgente("rete_strascico") != nullptr);
    m.avanza(1.0 / 9.0);
    REQUIRE(m.reti_strascico().size() == 1);   // il peschereccio a 9 nodi non strascica
    const IstanzaSorgente& r = m.reti_strascico().front();
    CHECK(r.id == 930002);
    const Vec2 nave = m.unita().at(2).pos;
    CHECK(distanza(r.pos, nave) == doctest::Approx(600.0).epsilon(0.01));   // 3 volte i 200 m di fondale
    CHECK(r.pos.x < nave.x);                                                 // a poppa (rotta est)
    CHECK(r.quota_m == doctest::Approx(198.0).epsilon(0.01));
    CHECK(m.istanza_paesaggio(930002) == &r);
    bool in_scena = false;
    for (const auto& s : m.scena_acustica(0, "bqq5_sfera").sorgenti) in_scena = in_scena || s.id == 930002;
    CHECK(in_scena);
    fs::remove(file);
}

TEST_CASE("radio E6 e R3: la chiave radio della missione, ordini, messaggi su condizione, su richiesta e annullati") {
    const auto f = scena_v2("radio",
        "V = function(u) m688.variabile(u, 5, 2) end, "
        "R = function(u) m688.variabile(u, 6, m688.radio(u, 'C') and 1 or 0) m688.variabile(u, 7, m688.radio(u, 'C') and 1 or 0) "
        "m688.variabile(u, 8, m688.radio(u, 'NESSUNO') and 1 or 0) end",
        "{ dopo_s = 30, unita = 0, gestore = 'V' }, { dopo_s = 60, unita = 0, gestore = 'R' }",
        "radio = { ordini = { da = 'COMSUBLANT', a = 'USS PROVA', oggetto = 'ORDINI', classifica = 'TOP SECRET',"
        "  data = { giorno = 3, mese = 11, anno = 1989, ora_utc = 6 } },"
        "  { id = 'A', dopo_s = 0, testo = { it = 'primo', en = 'first' } },"
        "  { id = 'C', su_richiesta = true, testo = { it = 'da script', en = 'by script' } },"
        "  { id = 'D', se = { variabile = { 5, '>=', 2 } }, testo = { it = 'variabile', en = 'variable' } },"
        "  { id = 'E', dopo_s = 0, annulla = 'F', testo = { it = 'annulla F', en = 'disregard F' } },"
        "  { id = 'F', dopo_s = 3000, testo = { it = 'annullato', en = 'cancelled' } },"
        "  { id = 'G', dopo_s = 0, ripetizioni = 1, ricevuta = true, testo = { it = 'conferma', en = 'acknowledge' } },"
        "},");
    Missione m(f, 1);
    m.radio_enhanced();
    REQUIRE(m.radio());
    const auto ordini = m.radio()->ordini();
    REQUIRE_FALSE(ordini.empty());
    const MessaggioRadio& o = ordini.front()->messaggio;
    CHECK(o.da == "COMSUBLANT");
    CHECK(o.a == "USS PROVA");
    CHECK(o.oggetto == "ORDINI");
    CHECK(o.classifica == "TOP SECRET");
    CHECK(o.dtg.find("NOV 89") != std::string::npos);
    CHECK(o.dtg.rfind("03", 0) == 0);   // il 3 del mese
    CHECK(m.radio()->in_coda("A"));
    CHECK_FALSE(m.radio()->in_coda("C"));   // solo da script
    CHECK_FALSE(m.radio()->in_coda("D"));   // aspetta la variabile
    for (int i = 0; i < 9 * 20; ++i) m.avanza(1.0 / 9.0);
    CHECK_FALSE(m.radio()->in_coda("D"));
    for (int i = 0; i < 9 * 50; ++i) m.avanza(1.0 / 9.0);
    CHECK(m.radio()->in_coda("D"));         // la variabile 5 e' arrivata a 2 a 30 s
    CHECK(var(m, 6) == 1);                  // m688.radio: in onda
    CHECK(var(m, 7) == 0);                  // gia' in coda: non si ripete
    CHECK(var(m, 8) == 0);                  // id sconosciuto
    CHECK(m.radio()->in_coda("C"));
    for (int i = 0; i < 9 * 900; ++i) m.avanza(1.0 / 9.0);
    CHECK_FALSE(m.radio()->in_coda("F"));   // annullato dalla messa in onda di E
    CHECK(m.errori().empty());
    fs::remove(f);
}

TEST_CASE("E6: il meteo fissato dall'autore e i messaggi di bordo propri della missione (1000 + n)") {
    const auto f = scena_v2("e6",
        "M = function(u) m688.messaggio(u, 1003) end",
        "{ dopo_s = 5, unita = 0, gestore = 'M' }",
        "meteo = { fisso = { stato_mare = 5, vento_m_s = 15, vento_da_gradi = 200, nebbia = 'avvezione', visibilita_m = 500 } },"
        "messaggi = { [3] = { it = 'Ciao dal comando', en = 'Hello from command' } },");
    Missione m(f, 1);
    m.meteo_enhanced(M688_DATI);
    REQUIRE(m.meteo());
    for (int i = 0; i < 9 * 10; ++i) m.avanza(1.0 / 9.0);
    CHECK(m.meteo()->fisso());
    CHECK(m.stato_mare() == 5);
    CHECK(m.meteo_qui().vento_m_s == doctest::Approx(15.0));
    CHECK(m.meteo_qui().nebbia == Nebbia::avvezione);
    CHECK(m.meteo_qui().visibilita_m == doctest::Approx(500.0));
    // il messaggio proprio: nella lista, non un esito, col testo nelle due lingue
    CHECK_FALSE(m.esito());
    CHECK(std::any_of(m.messaggi().begin(), m.messaggi().end(), [](const Messaggio& x) { return x.codice == 1003; }));
    const TestoRadio* t = m.testo_messaggio_missione(1003);
    REQUIRE(t != nullptr);
    CHECK(t->it == "Ciao dal comando");
    CHECK(t->en == "Hello from command");
    CHECK(m.testo_messaggio_missione(204) == nullptr);   // i codici globali restano della tabella
    CHECK(m.errori().empty());
    fs::remove(f);
}

TEST_CASE("mine dell'IA: il campo gia' posato fa saltare la petroliera; il posamine ne posa tre (S06, I04)") {
    const auto f = fs::temp_directory_path() / "m688_prova_mine_ia.lua";
    std::ofstream(f) << "return { id = 'MINE_IA', teatro = 'prova', script = {"
                        "P = function(u) m688.variabile(u, 1, m688.posa_mina(u, 'mdt', 3, 30) and 1 or 0)"
                        " m688.variabile(u, 2, m688.posa_mina(u, 'sconosciuta') and 1 or 0) end },"
                         " mine = { { tipo = 'mk67', x = 2400, y = 0, lato = 'urss' } },"
                        " varianti = { { probabilita = 1, unita = {\n"
                        "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true, posizione = { x = -4000, y = 0 },"
                        "  quota_m = 60, rotta = 0, velocita_kn = 0, " << MODELLO << " },"
                        "{ id = 2, classe = 'petroliera', categoria = 'nave', flag = 10, posizione = { x = 2400, y = -1500 },"
                        "  quota_m = 0, rotta = 0, velocita_kn = 10, " << MODELLO << " },"
                        "{ id = 3, classe = 'mercantile', categoria = 'nave', flag = 18, posizione = { x = 2700, y = -3000 },"
                        "  quota_m = 0, rotta = 0, velocita_kn = 6, " << MODELLO << " },"
                        "\n}, timer = { { dopo_s = 5, unita = 3, gestore = 'P' } } } } }\n";
    Missione m(f.string(), 1, false);
    m.imposta_fondale(fondale_di_prova());
    m.siluri_enhanced(M688_DATI);
    REQUIRE(m.con_siluri_enhanced());
    REQUIRE(m.campo_minato().mine().size() == 1);   // il campo della missione, gia' sul posto
    for (int i = 0; i < 9 * 400; ++i) m.avanza(1.0 / 9.0);
    CHECK(m.errori().empty());
    const auto& ev = m.registro().eventi();
    // la petroliera passa sulla Mk 67 armata, su 32 m di fondale (il raggio e' 60 m): scoppia sotto di lei
    CHECK(std::any_of(ev.begin(), ev.end(), [](const EventoMissione& e) { return e.tipo == TipoEvento::mina_esplosa && e.unita == 2; }));
    // il posamine: tre MDT sotto di se', una ogni 30 s, posate e note solo a lui
    CHECK(var(m, 1) == 1);
    CHECK(var(m, 2) == 0);   // tipo sconosciuto
    const auto& mine = m.campo_minato().mine();
    CHECK(std::count_if(mine.begin(), mine.end(), [](const Mina& x) { return x.posatore == 3; }) == 3);
    const auto posate = std::count_if(ev.begin(), ev.end(), [](const EventoMissione& e) { return e.tipo == TipoEvento::mina_posata; });
    CHECK(posate == 3);   // il campo iniziale non e' un evento
    CHECK(std::none_of(ev.begin(), ev.end(), [](const EventoMissione& e) { return e.tipo == TipoEvento::mina_posata && e.noto; }));
    fs::remove(f);
}

TEST_CASE("obiettivi «zona_inviolata» e «raggiungere» un'unita' in moto") {
    const auto f = fs::temp_directory_path() / "m688_prova_obiettivi_zona.lua";
    std::ofstream(f) << "return { id = 'ZONA', teatro = 'prova', script = {},"
                        " obiettivi = {"
                        "  { id = 'vicino', tipo = 'raggiungere', unita = { 2 }, distanza_m = 3500, testo = 'Vicino al Victor' },"
                        "  { id = 'lontano', tipo = 'raggiungere', classi = { 'victor3' }, distanza_m = 500, entro_s = 60, testo = 'Sotto il Victor' },"
                        "  { id = 'zona', tipo = 'zona_inviolata', cerchio = { x = 1000, y = 3000, r = 600 }, minuti = 1, testo = 'Nessun ostile' },"
                        "  { id = 'amici', tipo = 'zona_inviolata', cerchio = { x = 1000, y = 3000, r = 600 }, parte = 'amica', testo = 'Nessun amico' },"
                        "  { id = 'presto', tipo = 'zona_inviolata', cerchio = { x = 6000, y = 3000, r = 300 }, entro_s = 100, testo = 'Fino a 100 s' } },"
                        " varianti = { { probabilita = 1, unita = {\n"
                        "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true, posizione = { x = 0, y = 0 },"
                        "  quota_m = 60, rotta = 0, velocita_kn = 0, " << MODELLO << " },"
                        "{ id = 2, classe = 'victor3', categoria = 'sottomarino', flag = 17, posizione = { x = 0, y = 3000 },"
                        "  quota_m = 100, rotta = 90, velocita_kn = 5, " << MODELLO << " },"
                        "\n}, timer = {} } } }\n";
    Missione m(f.string(), 1, false);
    REQUIRE(m.errori().empty());
    const auto stato = [&](const char* id) {
        const auto e = m.esito_dettagliato();
        const auto o = std::find_if(e.obiettivi.begin(), e.obiettivi.end(), [&](const Obiettivo& x) { return x.id == id; });
        REQUIRE(o != e.obiettivi.end());
        return o->stato;
    };
    for (int i = 0; i < 9 * 5; ++i) m.avanza(1.0 / 9.0);
    CHECK(stato("vicino") == Obiettivo::Stato::raggiunto);   // a 3 km dal Victor che passa
    CHECK(stato("lontano") == Obiettivo::Stato::in_corso);
    CHECK(stato("zona") == Obiettivo::Stato::in_corso);
    for (int i = 0; i < 9 * 120; ++i) m.avanza(1.0 / 9.0);
    CHECK(stato("lontano") == Obiettivo::Stato::fallito);    // scaduto il tempo
    CHECK(stato("presto") == Obiettivo::Stato::raggiunto);   // nessuno entro i 100 s
    // il Victor (ostile) entra nel cerchio e ci resta piu' di un minuto
    for (int i = 0; i < 9 * 400; ++i) m.avanza(1.0 / 9.0);
    CHECK(stato("zona") == Obiettivo::Stato::fallito);
    CHECK(stato("amici") == Obiettivo::Stato::in_corso);     // nessun amico: ancora intatta, raggiunta a fine missione
    fs::remove(f);
}

TEST_CASE("mine dell'IA: il posamine posa anche senza un campo gia' in mare (S06)") {
    const auto f = fs::temp_directory_path() / "m688_prova_mine_ia_vuoto.lua";
    std::ofstream(f) << "return { id = 'MINE_IA_VUOTO', teatro = 'prova', script = {"
                        "P = function(u) m688.posa_mina(u, 'mdt', 2, 20) end },"
                        " varianti = { { probabilita = 1, unita = {\n"
                        "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true, posizione = { x = -4000, y = 0 },"
                        "  quota_m = 60, rotta = 0, velocita_kn = 0, " << MODELLO << " },"
                        "{ id = 3, classe = 'mercantile', categoria = 'nave', flag = 18, posizione = { x = 2700, y = -3000 },"
                        "  quota_m = 0, rotta = 0, velocita_kn = 6, " << MODELLO << " },"
                        "\n}, timer = { { dopo_s = 5, unita = 3, gestore = 'P' } } } } }\n";
    Missione m(f.string(), 1, false);
    m.imposta_fondale(fondale_di_prova());
    m.siluri_enhanced(M688_DATI);
    REQUIRE(m.con_siluri_enhanced());
    REQUIRE(m.campo_minato().mine().empty());   // niente in mare all'inizio
    for (int i = 0; i < 9 * 120; ++i) m.avanza(1.0 / 9.0);
    CHECK(m.errori().empty());
    CHECK(m.campo_minato().mine().size() == 2);
    fs::remove(f);
}

TEST_CASE("armi da esercitazione: il colpo si registra senza danni (schede I03 e S01)") {
    const auto scena = [](const std::string& chiavi, const std::string& arma, const std::string& nome) {
        const auto f = fs::temp_directory_path() / ("m688_prova_esercitazione_" + nome + ".lua");
        std::ofstream(f) << "return { id = 'ESERCITAZIONE', teatro = 'prova', " << chiavi << " script = {}, varianti = { { probabilita = 1, unita = {\n"
                            "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true, posizione = { x = 0, y = 0 },"
                            "  quota_m = 100, rotta = 90, velocita_kn = 0, " << MODELLO << " },"
                            "{ id = 3, classe = 'mk48', categoria = 'arma', flag = 41, posizione = { x = 0, y = -1500 }, quota_m = 100, rotta = 0,"
                            "  velocita_kn = 50, " << arma << " modello = { massima_kn = 50, virata_gradi_s = 9, accel_kn_s = 5, quota_m_s = 5, quota_max_m = 500, integrita = 2, difesa = 75 } },"
                            "\n}, timer = {} } } }\n";
        return f.string();
    };
    for (const auto& [chiavi, arma, nome] : {std::tuple{std::string(""), std::string("esercitazione = true,"), std::string("arma")},
                                             std::tuple{std::string("esercitazione = true,"), std::string(""), std::string("missione")}}) {
        const auto f = scena(chiavi, arma, nome);
        Missione m(f, 1, false);
        REQUIRE(m.errori().empty());
        CHECK(m.arma_esercitazione(3));
        const int prima = m.unita().at(0).integrita;
        for (int i = 0; i < 9 * 120 && m.colpi_esercitazione().empty(); ++i) m.avanza(1.0 / 9.0);
        REQUIRE(m.colpi_esercitazione().size() == 1);
        CHECK(m.colpi_esercitazione().front().vittima == 0);
        CHECK(m.unita().at(0).integrita == prima);   // nessun danno
        CHECK(m.colpi().empty());
        CHECK(std::any_of(m.messaggi().begin(), m.messaggi().end(), [](const Messaggio& x) { return x.codice == 457; }));
        const auto& ev = m.registro().eventi();
        CHECK(std::any_of(ev.begin(), ev.end(), [](const EventoMissione& e) { return e.tipo == TipoEvento::colpo_esercitazione && e.unita == 0; }));
        fs::remove(f);
    }
    // senza le chiavi il siluro e' vero
    const auto f = scena("", "", "vera");
    Missione m(f, 1, false);
    CHECK_FALSE(m.arma_esercitazione(3));
    for (int i = 0; i < 9 * 120 && m.colpi().empty(); ++i) m.avanza(1.0 / 9.0);
    CHECK_FALSE(m.colpi().empty());
    CHECK(m.colpi_esercitazione().empty());
    fs::remove(f);
}

TEST_CASE("relitto: fermo sul fondo, muto e sordo (scheda I10)") {
    const auto f = fs::temp_directory_path() / "m688_prova_relitto.lua";
    std::ofstream(f) << "return { id = 'RELITTO', teatro = 'prova', script = {}, varianti = { { probabilita = 1, unita = {\n"
                        "{ id = 0, classe = 'alfa', categoria = 'sottomarino', flag = 17, giocatore = true, posizione = { x = -4000, y = 0 },"
                        "  quota_m = 100, rotta = 90, velocita_kn = 0, " << MODELLO << " },"
                        "{ id = 2, classe = 'charlie2', categoria = 'sottomarino', flag = 17, stato = 'relitto', posizione = { x = -2000, y = 0 },"
                        "  quota_m = 50, rotta = 0, velocita_kn = 5, " << MODELLO << " },"
                        "\n}, timer = {} } } }\n";
    Missione m(f.string(), 1, false);
    m.imposta_fondale(fondale_di_prova());
    REQUIRE(m.errori().empty());
    CHECK(m.relitto(2));
    CHECK_FALSE(m.relitto(0));
    for (int i = 0; i < 9 * 120; ++i) m.avanza(1.0 / 9.0);
    const auto& r = m.unita().at(2);
    CHECK(r.pos.x == doctest::Approx(-2000.0));
    CHECK(r.pos.y == doctest::Approx(0.0));
    CHECK(r.velocita == doctest::Approx(0.0));
    CHECK(r.quota == doctest::Approx(m.profondita(r.pos)));   // sul fondo, a 200 m
    CHECK(r.sul_fondo);
    CHECK(r.contatti.empty());
    CHECK(r.rumore == 0);
    CHECK_FALSE(r.distrutta);
    fs::remove(f);
}

TEST_CASE("obiettivo «colpire»: un colpo d'esercitazione del giocatore sull'arancione (S01)") {
    const auto f = fs::temp_directory_path() / "m688_prova_colpire.lua";
    std::ofstream(f) << "return { id = 'COLPIRE', teatro = 'prova', script = {},"
                        " obiettivi = { { id = 'arancione', tipo = 'colpire', unita = { 2 }, testo = 'Colpire l arancione' } },"
                        " varianti = { { probabilita = 1, unita = {\n"
                        "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true, posizione = { x = -5000, y = 0 },"
                        "  quota_m = 100, rotta = 90, velocita_kn = 0, " << MODELLO << " },"
                        "{ id = 2, classe = 'victor3', categoria = 'sottomarino', flag = 17, posizione = { x = 0, y = 0 }, quota_m = 100, rotta = 90,"
                        "  velocita_kn = 0, " << MODELLO << " },"
                        "{ id = 3, classe = 'mk48', categoria = 'arma', flag = 41, esercitazione = true, posizione = { x = 0, y = -1500 }, quota_m = 100,"
                        "  rotta = 0, velocita_kn = 50, modello = { massima_kn = 50, virata_gradi_s = 9, accel_kn_s = 5, quota_m_s = 5, quota_max_m = 500, integrita = 2, difesa = 75 } },"
                        "\n}, timer = {} } } }\n";
    Missione m(f.string(), 1, false);
    REQUIRE(m.errori().empty());
    m.trova(3)->lanciatore = 0;   // l'arma l'ha lanciata il giocatore
    const auto stato = [&] {
        const auto e = m.esito_dettagliato();
        const auto o = std::find_if(e.obiettivi.begin(), e.obiettivi.end(), [](const Obiettivo& x) { return x.id == "arancione"; });
        REQUIRE(o != e.obiettivi.end());
        return o->stato;
    };
    CHECK(stato() == Obiettivo::Stato::in_corso);
    for (int i = 0; i < 9 * 120 && m.colpi_esercitazione().empty(); ++i) m.avanza(1.0 / 9.0);
    m.avanza(1.0);
    REQUIRE(m.colpi_esercitazione().size() == 1);
    CHECK(stato() == Obiettivo::Stato::raggiunto);
    CHECK_FALSE(m.unita().at(2).distrutta);   // colpito, non affondato
    CHECK(std::any_of(m.messaggi().begin(), m.messaggi().end(), [](const Messaggio& x) { return x.codice == 456; }));
    fs::remove(f);
}
