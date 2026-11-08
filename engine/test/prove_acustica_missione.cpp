#include <doctest.h>

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <map>
#include <memory>
#include <string>
#include <tuple>
#include <vector>

#include "missioni/danni_missione.hpp"
#include "missioni/missione.hpp"
#include "nucleo/classi.hpp"
#include "nucleo/fondale.hpp"
#include "nucleo/geometria.hpp"

using namespace m688;
namespace fs = std::filesystem;

// Scoperta sonar dell'Enhanced nella missione (nucleo/acustica.hpp; piano nelle note della sessione): equazione del
// sonar al posto del modello dell'originale, contatti che integrano l'eccesso di segnale, ping con eco e contro-scoperta.

namespace {

constexpr double DT = 1.0 / 9.0;

bool ci_sono_dati() { return fs::exists(std::string(M688_DATI) + "/acustica.toml"); }

// il 688 a 150 m e un Victor III a `distanza` metri a nord, tutti e due a 5 nodi; `velocita` e `quota` del giocatore
std::string scena(double distanza, double velocita = 5.0, double quota = 150.0, bool paesaggio = false) {
    const auto f = fs::temp_directory_path() / "m688_prova_acustica.lua";
    const std::string modello = "modello = { massima_kn = 32, virata_gradi_s = 1, accel_kn_s = 0.5, quota_m_s = 1, quota_max_m = 450, integrita = 200 }";
    std::ofstream(f) << "return { id = 'PROVA', teatro = 'faroe', paesaggio_sonoro = " << (paesaggio ? "true" : "false") << ", script = {"
                        "P = function(u) m688.impulso_sonar(u) end"
                        "}, varianti = { { probabilita = 1, unita = {\n"
                        "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true,"
                        "  posizione = { x = 0, y = 0 }, quota_m = " << quota << ", rotta = 0, velocita_kn = " << velocita << ", " << modello << " },"
                        "{ id = 2, classe = 'victor3', categoria = 'sottomarino', flag = 25, posizione = { x = 0, y = " << distanza << " },"
                        "  quota_m = 150, rotta = 90, velocita_kn = 5, " << modello << " },"
                        "\n}, timer = { { dopo_s = 600, unita = 2, gestore = 'P' } } } } }\n";
    return f.string();
}

struct Partita {
    std::map<std::string, ClasseNave> classi = carica_classi(M688_DATI "/classi.toml");
    Missione m;
    DanniMissione dm;
    Partita(const std::string& file) : m(file, 1, false), dm(classi, M688_DATI, 1, RegolaDanni::simulazione) {}
    void avanza(double s) { for (double t = 0; t < s; t += DT) { m.avanza(DT); dm.avanza(m, DT); } }
};

bool c_e(const Missione& m, int codice) {
    for (const auto& x : m.messaggi_sonar()) if (x.codice == codice) return true;
    return false;
}

}  // namespace

TEST_CASE("acustica: senza la regola di simulazione resta la scoperta dell'originale" * doctest::skip(!ci_sono_dati())) {
    const auto f = scena(3000.0);
    Missione m(f, 1, false);
    for (int i = 0; i < 9 * 60; ++i) m.avanza(DT);
    CHECK_FALSE(m.con_acustica());
    CHECK(m.messaggi_sonar().empty());
    fs::remove(f);
}

TEST_CASE("acustica: un Victor a 3 km si sente subito e diventa traccia solida; il passivo da' solo il rilevamento" * doctest::skip(!ci_sono_dati())) {
    const auto f = scena(3000.0);
    Partita p(f);
    p.avanza(10.0);
    REQUIRE(p.m.con_acustica());
    const auto& cc = p.m.unita().at(0).contatti;
    REQUIRE(cc.count(2) == 1);
    CHECK(c_e(p.m, 310));   // nuovo contatto
    p.avanza(290.0);
    const Contatto& c = cc.at(2);
    CHECK(c.qualita >= 800.0);
    CHECK(c.visibile());
    CHECK_FALSE(c.posizione_nota);   // solo passivo: la X sulla carta
    const double vero = rilevamento(p.m.unita().at(0).pos, p.m.unita().at(2).pos);
    CHECK(std::fabs(normalizza_180(c.rilevamento - vero)) < 5.0);
    // il ping del giocatore: eco con la posizione, e il Victor scopre il 688
    p.m.impulso_sonar_enhanced(0);
    CHECK(c.posizione_nota);
    CHECK(distanza(c.pos_stimata, p.m.unita().at(2).pos) < 0.05 * 3000.0);
    REQUIRE(p.m.unita().at(2).contatti.count(0) == 1);
    // l'IA che intercetta il ping ha il rilevamento, come il giocatore (docs/18 fase 2): la posizione no
    const auto& k = p.m.unita().at(2).contatti.at(0);
    CHECK(std::fabs(normalizza_180(k.rilevamento - rilevamento(p.m.unita().at(2).pos, p.m.unita().at(0).pos))) < 5.0);
    fs::remove(f);
}

TEST_CASE("acustica: a 30 km il Victor silenzioso non da' una traccia; il suo ping si', ma solo come rilevamento" * doctest::skip(!ci_sono_dati())) {
    const auto f = scena(30000.0);
    Partita p(f);
    p.avanza(300.0);
    const auto& cc = p.m.unita().at(0).contatti;
    CHECK((cc.count(2) == 0 || cc.at(2).qualita < 800.0));
    CHECK(p.m.unita().at(2).contatti.count(0) == 0);   // il 688 a 5 nodi non lo sente nessuno
    p.avanza(310.0);                                   // a 600 s il Victor fa un ping (script PING)
    CHECK(c_e(p.m, 312));                              // «ci stanno pingando»
    REQUIRE(cc.count(2) == 1);
    CHECK_FALSE(cc.at(2).posizione_nota);              // il giocatore ha il rilevamento, non la posizione
    const double vero = rilevamento(p.m.unita().at(0).pos, p.m.unita().at(2).pos);
    CHECK(std::fabs(normalizza_180(cc.at(2).rilevamento - vero)) < 5.0);
    fs::remove(f);
}

TEST_CASE("acustica: a quota bassa e a tutta forza il giocatore cavita e viene avvisato" * doctest::skip(!ci_sono_dati())) {
    const auto f = scena(30000.0, 30.0, 20.0);
    Partita p(f);
    p.avanza(5.0);
    REQUIRE(p.m.dinamica(0) != nullptr);
    CHECK(p.m.dinamica(0)->cavita());
    CHECK(c_e(p.m, 313));
    fs::remove(f);
}

TEST_CASE("acustica: scena per la stazione sonar, col sensore della suite" * doctest::skip(!ci_sono_dati())) {
    const auto f = scena(3000.0);
    Partita p(f);
    p.avanza(1.0);
    const auto sc = p.m.scena_acustica(0, "bqq5_sfera");
    REQUIRE(sc.ricevitore.sensore != nullptr);
    CHECK(sc.ricevitore.sensore->id == "bqq5_sfera");
    REQUIRE(sc.sorgenti.size() == 1);   // scena di prova senza paesaggio sonoro
    CHECK(sc.sorgenti.front().id == 2);
    CHECK(sc.ambiente.profilo != nullptr);
    CHECK(p.m.scena_acustica(0, "tb16").ricevitore.sensore == nullptr);   // schiera non filata
    CHECK(p.m.scena_acustica(0, "sovietico_3gen").ricevitore.sensore == nullptr);
    Missione classica(f, 1, false);
    CHECK(classica.scena_acustica(0, "bqq5_sfera").sorgenti.empty());
    fs::remove(f);
}

TEST_CASE("acustica: avvisi del sonar al giocatore, riga LOFAR e classificazione del Victor come sottomarino" * doctest::skip(!ci_sono_dati())) {
    const auto f = scena(3000.0);
    Partita p(f);
    p.avanza(900.0);
    CHECK(c_e(p.m, 347));   // nuova riga al LOFAR
    CHECK(c_e(p.m, 340));   // classificato: sottomarino
    CHECK_FALSE(c_e(p.m, 341));
    int lofar = 0;
    for (const auto& x : p.m.messaggi_sonar()) lofar += x.codice == 347;
    CHECK(lofar == 1);      // una volta per contatto
    CHECK(p.m.unita().at(0).contatti.at(2).durata >= 800.0);
    fs::remove(f);
}

TEST_CASE("acustica: paesaggio sonoro attorno al giocatore, nella scena della stazione e nei suoi contatti" * doctest::skip(!ci_sono_dati())) {
    const auto f = scena(30000.0, 5.0, 150.0, true);
    Partita p(f);
    p.avanza(300.0);
    const auto& ps = p.m.paesaggio();
    REQUIRE_FALSE(ps.empty());                         // alle Faroe a febbraio alcune decine
    for (const auto& i : ps) {
        CHECK(i.id >= 100000);
        CHECK(distanza(i.pos, p.m.unita().at(0).pos) < 110000.0);
    }
    const auto sc = p.m.scena_acustica(0, "bqq5_sfera");
    CHECK(sc.sorgenti.size() == 1 + ps.size());       // il Victor piu' il paesaggio
    for (const auto& [id, c] : p.m.contatti_paesaggio()) {
        CHECK(p.m.istanza_paesaggio(id) != nullptr);
        CHECK_FALSE(c.posizione_nota);
        CHECK(c.durata < 1400.0);
    }
    Missione classica(f, 1, false);
    classica.avanza(10.0);
    CHECK(classica.paesaggio().empty());
    fs::remove(f);
}

TEST_CASE("acustica: le voci registrate si collegano ai loro campioni (R5), una libreria sola per processo" * doctest::skip(!ci_sono_dati() || !fs::exists(std::string(M688_DATI) + "/campioni.toml"))) {
    const auto f = scena(3000.0);
    Partita p(f);
    p.avanza(1.0);
    REQUIRE(p.m.con_acustica());
    const LibreriaCampioni* l = p.m.campioni();
    REQUIRE(l != nullptr);
    CHECK(l == libreria_campioni(M688_DATI).get());   // la stessa che va alla cuffia
    const SorgenteLibreria* balena = p.m.catalogo_acustico()->sorgente("balenottera_comune");
    REQUIRE(balena != nullptr);
    REQUIRE(balena->campioni.size() == 1);
    CHECK(balena->campioni[0]->voce == "balenottera_comune");
    CHECK(balena->attivita_media.size() == balena->componenti.size());
    // una seconda missione riusa la libreria; con R5 spento il catalogo resta quello del modello
    Partita q(f);
    q.m.usa_campioni(false);
    q.avanza(1.0);
    REQUIRE(q.m.con_acustica());
    CHECK(q.m.campioni() == nullptr);
    CHECK(q.m.catalogo_acustico()->sorgente("balenottera_comune")->campioni.empty());
    fs::remove(f);
}

TEST_CASE("acustica: il sonar attivo speciale dello scenario vede l'eco piu' lontano e pinga da solo (scheda I09)" * doctest::skip(!ci_sono_dati())) {
    // il 688 a 150 m e uno Slava a 40 km: col sonar della classe l'eco non torna; con uno «speciale» (antenna di 15 m,
    // FM compresso a 5 ms) si', e la nave ha il 688 con la posizione. Pinga da sola ogni minuto
    const auto partita = [](bool speciale) {
        const auto f = fs::temp_directory_path() / "m688_prova_sonar_speciale.lua";
        const std::string modello = "modello = { massima_kn = 32, virata_gradi_s = 1, accel_kn_s = 0.5, quota_m_s = 1, quota_max_m = 450, integrita = 200 }";
        std::ofstream(f) << "return { id = 'PROVA', teatro = 'faroe', paesaggio_sonoro = false, script = { P = function(u) m688.impulso_sonar(u) end },"
                            " varianti = { { probabilita = 1, unita = {\n"
                            "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true, posizione = { x = 0, y = 0 },"
                            "  quota_m = 150, rotta = 0, velocita_kn = 5, " << modello << " },"
                            "{ id = 2, classe = 'slava', categoria = 'nave', flag = 18, posizione = { x = 0, y = 40000 }, quota_m = 0, rotta = 90,"
                            "  velocita_kn = 5, " << modello
                         << (speciale ? ", sonar_speciale = { sl_db = 230, frequenza_hz = 3500, impulso_s = 1, risoluzione_s = 0.005, dimensione_m = 15, ogni_s = 60 }" : "")
                         << " },\n}, timer = { " << (speciale ? "" : "{ dopo_s = 60, unita = 2, gestore = 'P' }") << " } } } }\n";
        auto p = std::make_unique<Partita>(f.string());
        p->avanza(65.0);
        fs::remove(f);
        return p;
    };
    const auto classe = partita(false);
    const auto& cc = classe->m.unita().at(2).contatti;
    CHECK((cc.count(0) == 0 || !cc.at(0).posizione_nota));
    const auto speciale = partita(true);
    REQUIRE(speciale->m.sonar_speciale(2) != nullptr);
    CHECK(speciale->m.sonar_speciale(0) == nullptr);
    const auto& cs = speciale->m.unita().at(2).contatti;
    REQUIRE(cs.count(0) == 1);
    CHECK(cs.at(0).posizione_nota);
    CHECK(cs.at(0).sensore == "sonar_speciale");
    CHECK(c_e(speciale->m, 312));   // e il 688 sente l'impulso
    CHECK(speciale->m.sonar_speciale(2)->prossimo > 100.0);   // il prossimo ping, da solo
}

TEST_CASE("acustica: l'antenna del sonar speciale si legge e un nome sconosciuto e' un errore" * doctest::skip(!ci_sono_dati())) {
    const auto scena = [](const std::string& antenna) {
        const auto f = fs::temp_directory_path() / "m688_prova_antenna.lua";
        const std::string modello = "modello = { massima_kn = 32, virata_gradi_s = 1, accel_kn_s = 0.5, quota_m_s = 1, quota_max_m = 450, integrita = 200 }";
        std::ofstream(f) << "return { id = 'PROVA', teatro = 'faroe', paesaggio_sonoro = false, script = {}, varianti = { { probabilita = 1, unita = {\n"
                            "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true, posizione = { x = 0, y = 0 }, quota_m = 150, rotta = 0, velocita_kn = 5, " << modello << " },"
                            "{ id = 2, classe = 'slava', categoria = 'nave', flag = 18, posizione = { x = 0, y = 20000 }, quota_m = 0, rotta = 90, velocita_kn = 5, " << modello
                         << ", sonar_speciale = { sl_db = 230, frequenza_hz = 1000, dimensione_m = 150, antenna = '" << antenna << "' } },\n}, timer = {} } } }\n";
        return f.string();
    };
    {
        Missione m(scena("lineare"), 1, false);
        CHECK(m.errori().empty());
        REQUIRE(m.sonar_speciale(2) != nullptr);
        CHECK(m.sonar_speciale(2)->antenna == "lineare");
    }
    Missione m(scena("parabolica"), 1, false);
    CHECK_FALSE(m.errori().empty());
}

TEST_CASE("acustica: fondo per teatro e profondita', gamberi e effetto pomeriggio nella missione" * doctest::skip(!ci_sono_dati())) {
    // Otranto in luglio: su 40 m d'acqua i gamberi e la sabbia; su 1000 m niente gamberi e il fango. Alle Faroe in marzo,
    // col condotto superficiale, a mezzogiorno col vento debole e il cielo sereno il microtermoclino del pomeriggio; a
    // mezzanotte no
    const auto prova = [](double fondo_m, double ora_utc, const char* teatro = "otranto", int mese = 7) {
        const auto f = fs::temp_directory_path() / "m688_prova_acustica_otranto.lua";
        const std::string modello = "modello = { massima_kn = 32, virata_gradi_s = 1, accel_kn_s = 0.5, quota_m_s = 1, quota_max_m = 450, integrita = 200 }";
        std::ofstream(f) << "return { id = 'PROVA', teatro = '" << teatro << "', paesaggio_sonoro = false,"
                            " meteo = { fisso = { vento_m_s = 2, nubi_ottavi = 0 } }, script = {}, varianti = { { probabilita = 1, unita = {\n"
                            "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true, posizione = { x = 0, y = 0 },"
                            "  quota_m = 30, rotta = 0, velocita_kn = 3, " << modello << " },"
                            "{ id = 2, classe = 'victor3', categoria = 'sottomarino', flag = 25, posizione = { x = 0, y = 3000 }, quota_m = 30, rotta = 90,"
                            "  velocita_kn = 5, " << modello << " },\n}, timer = {} } } }\n";
        Partita p(f.string());
        std::vector<std::uint16_t> v(100 * 100, static_cast<std::uint16_t>((-fondo_m + 11000.0) / 0.25));
        p.m.imposta_fondale(std::make_shared<const Fondale>(100, 100, 100.0, 0.25, -11000.0, std::move(v)));
        p.m.imposta_mese(mese);
        p.m.imposta_data_ora(15, mese, 1989, ora_utc);
        p.avanza(2.0);
        const auto sc = p.m.scena_acustica(0, "bqq5_sfera");
        fs::remove(f);
        return std::tuple{sc.ambiente.fondo, sc.ambiente.gamberi, sc.ambiente.profilo ? sc.ambiente.profilo->microstrato_m : -1.0};
    };
    const auto [fondo_basso, gamberi_basso, m1] = prova(40.0, 12.0);
    CHECK(fondo_basso == TipoFondo::sabbia);
    CHECK(gamberi_basso);
    const auto [fondo_alto, gamberi_alto, m2] = prova(1000.0, 0.0);
    CHECK(fondo_alto == TipoFondo::fango);
    CHECK_FALSE(gamberi_alto);
    const auto [f1, g1, micro_giorno] = prova(1000.0, 14.0, "faroe", 3);
    const auto [f2, g2, micro_notte] = prova(1000.0, 1.0, "faroe", 3);
    MESSAGE("pomeriggio: giorno " << micro_giorno << " m, notte " << micro_notte << " m");
    CHECK(micro_giorno > 0.0);
    CHECK(micro_notte == doctest::Approx(0.0));
}

// l'SSN che aspetta e' sul fango della scarpata a ~49 km dalla rotta d'uscita (scene_interazioni, 28/10): prima stava sulla
// sabbia della piattaforma a 61 km dal Typhoon, al limite della TB-16 (SE fra -3 e +7 dB dopo la correzione del 27/10)
TEST_CASE("acustica: S6, Barents a settembre sul fango della scarpata, il Typhoon in superficie a 100 km non si sente" *
          doctest::skip(!ci_sono_dati() || !fs::exists(std::string(M688_MISSIONI) + "/prove/PROVA_S6_USCITA.lua"))) {
    auto classi = carica_classi(M688_DATI "/classi.toml");
    Missione m(std::string(M688_MISSIONI) + "/prove/PROVA_S6_USCITA.lua", 1);
    CHECK(m.mese_acustica() == 9);   // la scena e' del 4 settembre: l'acqua di settembre, non il marzo del teatro
    std::map<std::string, double> pescaggi;
    for (const auto& [id, k] : classi) pescaggi[id] = k.pescaggio_m;
    const std::string fdl = std::string(M688_DATI) + "/teatri/" + m.teatro() + ".fdl";
    REQUIRE(fs::exists(fdl));
    m.imposta_fondale(std::make_shared<const Fondale>(Fondale::carica(fdl)), pescaggi);
    DanniMissione dm(classi, M688_DATI, 1, RegolaDanni::simulazione);
    while (m.tempo() < 120.0) {
        m.avanza(DT);
        dm.avanza(m, DT);
        m.raccogli_danni(nullptr, &dm);
    }
    REQUIRE(m.con_acustica());
    const auto sc = m.scena_acustica(4, "tb16");   // il Los Angeles a 90 m, la schiera col cavo sicuro
    REQUIRE(sc.ricevitore.sensore);
    CHECK(sc.ambiente.fondale_m > 200.0);   // oltre FONDO_SCARPATA_M: fango, non la sabbia che riflette
    CHECK(sc.ambiente.fondo == TipoFondo::fango);
    CHECK(sc.ricevitore.sfregamento == 0.0);   // la prova misura il fango, non la schiera che striscia (scena del 28/10)
    CacheCampi campi;
    campi.limite_per_passo = 100;
    std::map<int, Rilevazione> r;
    std::map<int, double> d;
    for (const Sorgente& s : sc.sorgenti)
        if (s.id >= 2 && s.id <= 8) {
            r[s.id] = valuta(s, sc.ricevitore, sc.ambiente, campi);
            d[s.id] = distanza(sc.ricevitore.pos, s.pos);
        }
    REQUIRE(r.count(2));
    CHECK(d[2] > 80000.0);
    MESSAGE("Typhoon a " << d[2] << " m: SE " << r[2].se_db << " dB, TL " << r[2].tl_db << "; schiera a " << sc.ricevitore.quota_m
            << " m, sul fondo " << sc.ricevitore.sfregamento);
    // la scena vuole che lo trovi solo dopo il messaggio dei 30 minuti. Il 28/10, con la schiera agganciata (D) e il cavo
    // sicuro, la TB-16 ascolta a ~180 m, sopra il fango: a 89 km (-21,7 dB) lo sentiva in 8-13 minuti, quindi l'SSN aspetta
    // a 101 km, appena oltre il salto della propagazione (-52,8 dB), e lo trova a 5560 e 6580 s nei semi 1 e 2
    CHECK(r[2].se_db <= -10.0);
    CHECK(r[2].se_db >= -60.0);
    for (const int p : {5, 6, 7, 8}) {   // i pescherecci al lavoro entro 35 km restano forti (dal 28/10 sono a 43-56 km)
        REQUIRE(r.count(p));
        if (d[p] < 35000.0) CHECK(r[p].se_db > 15.0);
    }
}
