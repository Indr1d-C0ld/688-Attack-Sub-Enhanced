#include <doctest.h>

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <map>
#include <set>
#include <tuple>
#include <string>

#include "missioni/danni_missione.hpp"
#include "missioni/danni_sottomarini.hpp"
#include "missioni/missione.hpp"
#include "missioni/registro_missione.hpp"
#include "missioni/stazioni_missione.hpp"
#include "nucleo/carico.hpp"
#include "nucleo/classi.hpp"
#include "nucleo/geometria.hpp"
#include "nucleo/missili.hpp"
#include "nucleo/ordini.hpp"

using namespace m688;
namespace fs = std::filesystem;

// Fase 4 B delle interazioni fra unita' dell'IA (docs/18_interazioni_ia.md; nota condivisa ia-fase4-contratto.md): le
// armi dell'IA nella missione. Carico dell'anno e tubi, scelta del siluro al lancio, gambe di TMA fino alla soluzione.

namespace {

const std::string SUB = "modello = { massima_kn = 32, virata_gradi_s = 1, accel_kn_s = 0.5, quota_m_s = 1, quota_max_m = 450, integrita = 200, rumore = 8, sonar = 19 }";
const std::string NAVE = "modello = { massima_kn = 32, virata_gradi_s = 2, accel_kn_s = 0.5, quota_m_s = 1, quota_max_m = 0, integrita = 100, rumore = 10, sonar = 20 }";

// un lancio come quello della libreria (R_siluro_urss), ridotto: un type53 del modello classico sul bersaglio 2
const std::string LANCIA =
    "L = function(u) m688.bersaglio(u, 2) m688.memorizza_bersaglio(u) m688.modello_azzera(u) m688.modello_classe(u, 'type53')"
    " m688.modello_flag(u, 49) m688.lancia(u, '', '') m688.modello_avanti_tutta(u, 45.7) m688.modello_integrita(u, 20)"
    " m688.modello_quota_max(u, 548) m688.telegrafo(u, 1) m688.navigazione(u, 1) m688.collega(u, 1) m688.nuova_unita(u) end";

// `editor`: con il blocco dell'editor la missione non e' un'originale (le gambe di TMA valgono)
std::string scena(const std::string& nome, const std::string& situazione, const std::string& altre, const std::string& script,
                  const std::string& timer, bool editor) {
    const auto f = fs::temp_directory_path() / ("m688_prova_armi_ia_" + nome + ".lua");
    std::ofstream(f) << "return { id = 'ARMI_IA_" << nome << "', teatro = 'faroe', lato = 'usa', data = '1989-06-15', ora_utc = 12,"
                        " paesaggio_sonoro = false, carriera = { situazione = '" << situazione << "' }," << (editor ? " editor = {}," : "")
                     << " script = {" << script << "}, varianti = { { probabilita = 1, unita = {\n"
                        "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true, posizione = { x = -90000, y = -80000 },"
                        "  quota_m = 300, rotta = 90, velocita_kn = 3, " << SUB << " },\n" << altre << "\n}, timer = {" << timer << "} } } }\n";
    return f.string();
}

struct Partita {
    std::map<std::string, ClasseNave> classi = carica_classi(M688_DATI "/classi.toml");
    Missione m;
    DanniMissione dm;
    explicit Partita(const std::string& file) : m(file, 1, false), dm(classi, M688_DATI, 1, RegolaDanni::simulazione) {}
    void avanza(double s) { for (long i = 0; i < static_cast<long>(s * 9.0); ++i) { m.avanza(1.0 / 9.0); dm.avanza(m, 1.0 / 9.0); } }
};

bool c_e() { return fs::exists(M688_DATI "/interazioni.toml") && fs::exists(M688_DATI "/carico.toml"); }

}  // namespace

TEST_CASE("armi dell'IA: il Victor lancia dai suoi tubi i siluri scelti; col quinto lancio i tubi sono vuoti" * doctest::skip(!c_e())) {
    // un Victor III (3) e una Perry (2) a 6 km; lo script lancia cinque volte di fila
    const auto f = scena("tubi", "guerra",
        "{ id = 2, classe = 'perry', categoria = 'nave', flag = 10, posizione = { x = 0, y = 6000 }, quota_m = 0, rotta = 90, velocita_kn = 5, " + NAVE + " },"
        "{ id = 3, classe = 'victor3', categoria = 'sottomarino', flag = 17, posizione = { x = 0, y = 0 }, quota_m = 100, rotta = 90, velocita_kn = 5, " + SUB + " },",
        LANCIA, "{ dopo_s = 5, unita = 3, gestore = 'L' }, { dopo_s = 6, unita = 3, gestore = 'L' }, { dopo_s = 7, unita = 3, gestore = 'L' },"
                "{ dopo_s = 8, unita = 3, gestore = 'L' }, { dopo_s = 9, unita = 3, gestore = 'L' }", false);
    Partita p(f);
    p.avanza(2.0);
    REQUIRE(p.m.con_armi_ia());
    REQUIRE(p.m.carico_ia_di(3) != nullptr);
    const int prima = p.m.armi_rimaste(3).value_or(-1);
    CHECK(prima >= 8);
    p.avanza(12.0);
    int lanciati = 0, mancati = 0;
    std::set<std::string> modelli;
    for (const auto& [id, u] : p.m.unita()) {
        if (u.lanciatore != 3 || !(u.flag & 0x20) || (u.flag & 0x80)) continue;
        if (const Siluro* s = p.m.siluro(id)) {
            ++lanciati;
            modelli.insert(s->dati().classe);
        } else if (u.persa) {
            ++mancati;
        }
    }
    const auto lanci_ia = std::count_if(p.m.eventi_missione().begin(), p.m.eventi_missione().end(),
                                        [](const EventoMissione& e) { return e.tipo == TipoEvento::lancio_ia && e.unita == 3; });
    CHECK(lanciati == 4);   // quattro tubi da 533
    CHECK(lanci_ia == 4);   // il quinto non parte e non e' un lancio
    CHECK(p.m.armi_rimaste(3).value_or(-1) == prima - 4);
    CHECK(std::all_of(p.m.carico_ia_di(3)->tubi.begin(), p.m.carico_ia_di(3)->tubi.end(), [](const std::string& v) { return v.empty(); }));
    CHECK_FALSE(modelli.empty());
    MESSAGE("modelli lanciati dal Victor: ", [&] { std::string x; for (const auto& m : modelli) x += m + " "; return x; }());
    // dopo la ricarica i tubi sono di nuovo pieni, dalla riserva
    p.avanza(320.0);
    CHECK(std::count_if(p.m.carico_ia_di(3)->tubi.begin(), p.m.carico_ia_di(3)->tubi.end(), [](const std::string& v) { return !v.empty(); }) == 4);
    CHECK(p.m.armi_rimaste(3).value_or(-1) == prima - 4);
    fs::remove(f);
}

TEST_CASE("armi dell'IA: col solo rilevamento il sottomarino fa le gambe di TMA, ma non nelle missioni originali" * doctest::skip(!c_e())) {
    // un Victor III (3) che sente una Perry (2) a 7 km, in guerra (a 10 km la Perry e' in zona d'ombra: la qualita' del
    // contatto non arriva alla traccia solida, misura del disassemblatore coi profili WOA del 29/10/2026)
    const std::string unita =
        "{ id = 2, classe = 'perry', categoria = 'nave', flag = 10, posizione = { x = 0, y = 7000 }, quota_m = 0, rotta = 90, velocita_kn = 12, " + NAVE + " },"
        "{ id = 3, classe = 'victor3', categoria = 'sottomarino', flag = 17, posizione = { x = 0, y = 0 }, quota_m = 100, rotta = 90, velocita_kn = 5, " + SUB + " },";
    const auto prova = [&](bool editor) {
        const auto f = scena(editor ? "tma_editor" : "tma_originale", "guerra", unita, "", "", editor);
        Partita p(f);
        p.avanza(900.0);
        fs::remove(f);
        const auto& ev = p.m.eventi_tma_ia();
        return std::make_pair(static_cast<int>(std::count_if(ev.begin(), ev.end(), [](const Missione::EventoTmaIa& e) { return e.unita == 3; })),
                              p.m.unita().at(3).contatti.count(2) ? p.m.unita().at(3).contatti.at(2).posizione_nota : false);
    };
    const auto [eventi, soluzione] = prova(true);
    CHECK(eventi >= 1);
    MESSAGE("TMA dell'IA in 15 minuti: eventi ", eventi, ", soluzione ", soluzione);
    const auto [eventi_orig, soluzione_orig] = prova(false);
    CHECK(eventi_orig == 0);
    (void)soluzione_orig;
}

TEST_CASE("armi dell'IA: m688.attacca_unita lancia sulla nave della sua parte, anche in pace (I03, I05)" * doctest::skip(!c_e())) {
    // un Victor III (3) e una Krivak sovietica (2) a 4 km, in pace: senza l'ordine niente; con l'ordine senza soluzione
    // il siluro parte sul rilevamento vero
    const auto prova = [](bool ordine, bool senza_soluzione) {
        const std::string script = std::string("F = function(u) m688.attacca_unita(u, 2") + (senza_soluzione ? ", true" : "") + ") end";
        const auto f = scena("forzato", "pace",
            "{ id = 2, classe = 'krivak2', categoria = 'nave', flag = 18, posizione = { x = 0, y = 4000 }, quota_m = 0, rotta = 90, velocita_kn = 5, " + NAVE + " },"
            "{ id = 3, classe = 'victor3', categoria = 'sottomarino', flag = 17, posizione = { x = 0, y = 0 }, quota_m = 100, rotta = 90, velocita_kn = 5, " + SUB + " },",
            script, ordine ? "{ dopo_s = 5, unita = 3, gestore = 'F' }" : "", true);
        Partita p(f);
        p.avanza(60.0);
        int siluri = 0;
        double rotta = -1.0;
        for (const auto& [id, u] : p.m.unita())
            if (u.categoria == "arma" && u.lanciatore == 3) { ++siluri; rotta = u.rotta; }
        fs::remove(f);
        return std::make_pair(siluri, rotta);
    };
    CHECK(prova(false, false).first == 0);
    const auto [siluri, rotta] = prova(true, true);
    CHECK(siluri == 1);
    CHECK(std::fabs(normalizza_180(rotta)) < 20.0);   // la Krivak e' a nord: il rilevamento vero
}

TEST_CASE("armi dell'IA: nel classico niente carico") {
    const auto f = scena("classico", "guerra",
        "{ id = 3, classe = 'victor3', categoria = 'sottomarino', flag = 17, posizione = { x = 0, y = 0 }, quota_m = 100, rotta = 90, velocita_kn = 5, " + SUB + " },",
        "", "", true);
    Missione m(f, 1, false);
    for (int i = 0; i < 9 * 5; ++i) m.avanza(1.0 / 9.0);
    CHECK_FALSE(m.con_armi_ia());
    CHECK_FALSE(m.armi_rimaste(3).has_value());
    fs::remove(f);
}

// L'ATTACK di uno script dell'IA su un sottomarino (collaudo della libreria, S12): nell'Enhanced la Perry tira il Mk 46
// vero dai Mk 32 col modello del siluro, sul punto del suo contatto; la Krivak II il SET-65. Senza la posizione del
// contatto, o fuori portata, non tira. Il Victor a 30 m: la posizione gliela da' il ping della nave. Contro il Victor
// emerso niente siluro (dal 03/11/2026: la testa cerca sotto; restano i cannoni)
TEST_CASE("armi dell'IA: l'ATTACK della nave tira il siluro antisom vero della classe (S12)" * doctest::skip(!c_e())) {
    const auto prova = [](const std::string& classe, int flag, double distanza_m, double quota_m = 30.0) {
        std::string sub = SUB;   // l'emerso resta in superficie: niente quota per evadere
        if (quota_m <= 0.0) sub.replace(sub.find("quota_max_m = 450"), 17, "quota_max_m = 0");
        const std::string unita =
            "{ id = 2, classe = '" + classe + "', categoria = 'nave', flag = " + std::to_string(flag) + ", posizione = { x = 0, y = 0 }, quota_m = 0,"
            " rotta = 90, velocita_kn = 10, " + NAVE + " },"
            "{ id = 3, classe = 'victor3', categoria = 'sottomarino', flag = " + std::to_string(flag == 10 ? 17 : 9) + ", posizione = { x = " +
            std::to_string(distanza_m) + ", y = 0 }, quota_m = " + std::to_string(quota_m) + ", rotta = 0, velocita_kn = 4, " + sub + " },";
        const auto f = scena("antisom_" + classe, "guerra", unita,
                             "A = function(u) m688.impulso_sonar(u) m688.bersaglio(u, 3) m688.attacca(u, 6, 0) end",
                             "{ dopo_s = 120, unita = 2, gestore = 'A' }", true);
        Partita p(f);
        p.avanza(125.0);
        std::string arma;
        for (const auto& [id, u] : p.m.unita())
            if (u.categoria == "arma" && u.lanciatore == 2) arma = u.classe;
        fs::remove(f);
        return arma;
    };
    const std::string perry = prova("perry", 10, 4000.0);
    MESSAGE("la Perry tira: ", perry.empty() ? std::string("la carica a dado") : perry);
    CHECK(perry == "mk46");
    CHECK(prova("krivak2", 18, 4000.0) == "set65");
    CHECK(prova("type22", 10, 4000.0) == "mk46");   // gli STWS Mk 2 della Type 22 (disassemblatore, 31/10/2026)
    CHECK(prova("perry", 10, 9000.0).empty());   // oltre la corsa utile dai Mk 32: niente (e niente dado di cariche)
    CHECK(prova("perry", 10, 4000.0, 0.0).empty());   // il Victor emerso: nessun siluro (e nessun dado)
}

// il Terne III della Oslo (S16; principale, 30/10/2026): nessun siluro della classe, il colpo a dado dell'ATTACK come
// l'RBU della Koni, ma solo fra 425 e 1600 m e per sei salve. Uno script che chiama l'ATTACK ogni 20 s per 10 minuti su
// un Victor senza motore a 50 m (immerso: i cannoni della Oslo, dal 31/10/2026, non lo prendono). Ogni colpo cade nella
// fascia (la distanza la sposta la deriva del Victor: si guarda quella del colpo, 31/10/2026): a 1 km sei colpi; da
// 300 m, quelli che arrivano solo quando la deriva lo ha portato oltre i 425 m; a 3 km nessuno; la Koni a 3 km tira
TEST_CASE("armi dell'IA: il Terne della Oslo tira sei salve fra 425 e 1600 m" * doctest::skip(!c_e())) {
    std::string fermo = SUB, nave_ferma = NAVE;
    fermo.replace(fermo.find("massima_kn = 32"), 15, "massima_kn = 0");
    nave_ferma.replace(nave_ferma.find("massima_kn = 32"), 15, "massima_kn = 0");
    // i colpi della nave e la distanza nave-bersaglio di ciascuno (la nave resta in (0, 0))
    const auto colpi = [&](const std::string& classe, int flag, double distanza_m) {
        const std::string unita =
            "{ id = 2, classe = '" + classe + "', categoria = 'nave', flag = " + std::to_string(flag) + ", posizione = { x = 0, y = 0 }, quota_m = 0,"
            " rotta = 90, velocita_kn = 0, " + nave_ferma + " },"
            "{ id = 3, classe = 'victor3', categoria = 'sottomarino', flag = " + std::to_string(flag == 10 ? 17 : 9) + ", posizione = { x = " +
            std::to_string(distanza_m) + ", y = 0 }, quota_m = 50, rotta = 0, velocita_kn = 0, " + fermo + " },";
        const auto f = scena("terne_" + classe + "_" + std::to_string(static_cast<int>(distanza_m)), "guerra", unita,
                             "A = function(u) m688.bersaglio(u, 3) m688.attacca(u, 6, 0) m688.dopo(u, 20, 'A') end",
                             "{ dopo_s = 30, unita = 2, gestore = 'A' }", true);
        Partita p(f);
        p.avanza(600.0);
        fs::remove(f);
        REQUIRE(distanza(p.m.unita().at(2).pos, {0.0, 0.0}) < 1.0);
        std::vector<double> d;
        for (const auto& c : p.m.colpi())
            if (c.tipo == Colpo::Tipo::attacco && c.lanciatore == 2) d.push_back(distanza(c.pos, {0.0, 0.0}));
        return d;
    };
    const auto nella_fascia = [](const std::vector<double>& d) {
        return std::all_of(d.begin(), d.end(), [](double x) { return x >= 425.0 - 1.0 && x <= 1600.0 + 1.0; });
    };
    const auto a1000 = colpi("oslo", 10, 1000.0);
    MESSAGE("colpi del Terne a 1 km in 10 minuti: ", a1000.size());
    CHECK(a1000.size() == 6);
    CHECK(nella_fascia(a1000));
    const auto a300 = colpi("oslo", 10, 300.0);
    MESSAGE("da 300 m: ", a300.size(), " colpi, il primo a ", a300.empty() ? 0.0 : a300.front(), " m");
    CHECK(nella_fascia(a300));               // sotto la fascia i razzi passano sopra: si tira solo dopo la deriva
    CHECK(colpi("oslo", 10, 3000.0).empty());   // oltre i 1600 m
    CHECK_FALSE(colpi("koni", 18, 3000.0).empty());   // l'RBU-6000 della Koni arriva alle 2 nm dell'ATTACK
}

// la cadenza (secondo collaudo della libreria, 30/10/2026: 12 Mk 46 in tre minuti da due Perry su un Alfa fermo): uno
// script che chiama l'ATTACK ogni 20 s; la Perry tira un'arma alla volta, la successiva dopo l'esito e la verifica
TEST_CASE("armi dell'IA: la nave tira un siluro antisom alla volta e aspetta l'esito" * doctest::skip(!c_e())) {
    const std::string unita =
        "{ id = 2, classe = 'perry', categoria = 'nave', flag = 10, posizione = { x = 0, y = 0 }, quota_m = 0,"
        " rotta = 90, velocita_kn = 5, " + NAVE + " },"
        "{ id = 3, classe = 'alfa', categoria = 'sottomarino', flag = 17, posizione = { x = 4000, y = 0 }, quota_m = 30, rotta = 0,"
        " velocita_kn = 0, " + SUB + " },";   // fermo a 30 m: la posizione gliela da' il ping della Perry
    const auto f = scena("antisom_cadenza", "guerra", unita,
                         "A = function(u) m688.impulso_sonar(u) m688.bersaglio(u, 3) m688.attacca(u, 6, 0) m688.dopo(u, 20, 'A') end",
                         "{ dopo_s = 30, unita = 2, gestore = 'A' }", true);
    Partita p(f);
    std::set<int> lanciati;
    int insieme = 0;
    for (int k = 0; k < 60; ++k) {   // 10 minuti
        p.avanza(10.0);
        int in_acqua = 0;
        for (const auto& [id, u] : p.m.unita())
            if (u.categoria == "arma" && u.lanciatore == 2 && u.classe == "mk46") {
                lanciati.insert(id);
                if (!u.distrutta && !u.persa) ++in_acqua;
            }
        insieme = std::max(insieme, in_acqua);
    }
    MESSAGE("Mk 46 in 10 minuti: ", lanciati.size(), ", al piu' ", insieme, " insieme");
    CHECK(lanciati.size() >= 1);
    CHECK(lanciati.size() <= 4);   // prima del 30/10/2026: uno ogni 20 s
    CHECK(insieme <= 1);
    fs::remove(f);
}

// le salve della dotazione contano solo per l'attacco a dado (disassemblatore, E6 §7, 03/11/2026): con l'ATTACK ogni 2 s
// prima le 12 salve della Perry finivano in 24 s mentre il Mk 46 vero aspettava, e la nave non tirava piu' nemmeno quello
TEST_CASE("armi dell'IA: l'ATTACK frequente non consuma le salve della nave che ha il siluro vero" * doctest::skip(!c_e())) {
    std::string duro = SUB;
    duro.replace(duro.find("integrita = 200"), 15, "integrita = 9000");   // il primo Mk 46 non lo affonda
    const std::string unita =
        "{ id = 2, classe = 'perry', categoria = 'nave', flag = 10, posizione = { x = 0, y = 0 }, quota_m = 0,"
        " rotta = 90, velocita_kn = 5, " + NAVE + " },"
        "{ id = 3, classe = 'alfa', categoria = 'sottomarino', flag = 17, posizione = { x = 4000, y = 0 }, quota_m = 30, rotta = 0,"
        " velocita_kn = 0, " + duro + " },";
    const auto f = scena("antisom_salve", "guerra", unita,
                         "A = function(u) m688.impulso_sonar(u) m688.bersaglio(u, 3) m688.attacca(u, 6, 0) m688.dopo(u, 2, 'A') end",
                         "{ dopo_s = 30, unita = 2, gestore = 'A' }", true);
    Partita p(f);
    const auto mk46 = [&] {
        std::set<int> v;
        for (const auto& [id, u] : p.m.unita())
            if (u.categoria == "arma" && u.lanciatore == 2 && u.classe == "mk46") v.insert(id);
        return v;
    };
    p.avanza(90.0);   // il primo ATTACK a 30 s; a 54 s la vecchia regola aveva gia' speso le 12 salve
    const auto primo = mk46();
    REQUIRE(primo.size() == 1);
    p.m.trova(*primo.begin())->persa = true;   // il primo Mk 46 finisce (qui a mano): poi la verifica, 90 s
    p.avanza(240.0);
    auto lanciati = mk46();   // il primo, perso, non e' piu' fra le unita'
    lanciati.insert(*primo.begin());
    MESSAGE("Mk 46 in 5 minuti e mezzo con l'ATTACK ogni 2 s: ", lanciati.size());
    CHECK(lanciati.size() >= 2);
    fs::remove(f);
}

// l'elicottero imbarcato oltre la portata dei tubi (principale, 30/10/2026; missione_antisom_navi.cpp): la Perry col
// contatto su un Alfa in superficie a 20 km fa decollare l'SH-60B, che da li' e' un aereo ASW come gli altri (boe, Mk 46);
// col mare grosso no, e se la missione ha gia' un elicottero sulla nave non ne fa un altro
TEST_CASE("armi dell'IA: oltre i tubi la nave fa decollare il suo elicottero ASW" * doctest::skip(!c_e() || !fs::exists(M688_DATI "/asw_aereo.toml"))) {
    const auto gioca = [](const std::string& nome, const std::string& meteo, bool gia_suo, const std::string& nave = "perry",
                          const std::string& mezzo = "sh60b", bool passato = false, double quota_alfa = 0.0,
                          const std::string& classe_sub = "alfa", int minuti = 20) {
        const auto f = fs::temp_directory_path() / ("m688_prova_armi_ia_elicottero_" + nome + ".lua");
        std::ofstream(f) << "return { id = 'ARMI_IA_ELI_" << nome << "', teatro = 'faroe', lato = 'usa', data = '1989-06-15', ora_utc = 12,"
                            " paesaggio_sonoro = false, carriera = { situazione = 'guerra' }, editor = {}," << meteo
                         << " script = { A = function(u) m688.bersaglio(u, 3) m688.attacca(u, 20, 0) m688.dopo(u, 20, 'A') end },"
                            " varianti = { { probabilita = 1, unita = {\n"
                            "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true, posizione = { x = -90000, y = -80000 },"
                            "  quota_m = 300, rotta = 90, velocita_kn = 3, " << SUB << " },\n"
                            "{ id = 2, classe = '" << nave << "', categoria = 'nave', flag = 10, posizione = { x = 0, y = 0 }, quota_m = 0,"
                            " rotta = 90, velocita_kn = 5, " << NAVE << " },\n"
                            "{ id = 3, classe = '" << classe_sub << "', categoria = 'sottomarino', flag = 17, posizione = { x = 20000, y = 0 }, quota_m = " << quota_alfa << ", rotta = 0,"
                            " velocita_kn = 4, " << SUB << " },\n"
                         << (gia_suo ? "{ id = 4, classe = 'sh60b', categoria = 'aereo', flag = 11, posizione = { x = 100, y = 0 }, quota_m = 0, rotta = 90,"
                                       " velocita_kn = 0, " + NAVE + " },\n" : std::string())
                         << "}, timer = { { dopo_s = 30, unita = 2, gestore = 'A' } } } } }\n";
        Partita p(f.string());
        std::set<int> elicotteri, controllati, iniziali;
        for (const auto& [id, u] : p.m.unita()) iniziali.insert(id);
        std::size_t boe = 0, lanci = 0;
        for (int k = 0; k < minuti * 6; ++k) {
            if (passato) {   // il contatto passato dalla formazione (la Knox non ha ancora il radar nei dati dell'elettronica)
                UnitaClassica* n = p.m.trova(2);
                Contatto& c = n->contatti[3];
                c.qualita = 1000.0;
                c.posizione_nota = true;
                c.pos_stimata = p.m.unita().at(3).pos;
                c.rilevamento = rilevamento(n->pos, c.pos_stimata);
                c.ultima_scoperta = p.m.tempo();
                c.sensore = "radio";
            }
            p.avanza(10.0);
            boe = std::max(boe, p.m.boe().size());
            lanci = std::max(lanci, p.m.lanci_aerei().size());
            for (const auto& [id, u] : p.m.unita())
                if (u.classe == mezzo && !iniziali.count(id)) {
                    elicotteri.insert(id);
                    if (p.m.controllore_asw(id)) controllati.insert(id);
                }
        }
        fs::remove(f);
        MESSAGE(nome, ": elicotteri della nave ", elicotteri.size(), " (col controllore ", controllati.size(), "), boe ", boe, ", lanci ", lanci);
        return std::tuple<int, int, std::size_t, std::size_t>{static_cast<int>(elicotteri.size()), static_cast<int>(controllati.size()), boe, lanci};
    };
    const auto [n, c, boe, lanci] = gioca("calmo", "", false);
    CHECK(n == 1);
    CHECK(c == 1);
    CHECK(boe + lanci >= 1);   // caccia: boe o il Mk 46
    CHECK(std::get<0>(gioca("mare7", " meteo = { fisso = { stato_mare = 7 } },", false)) == 0);   // dal ponte col mare 7 no
    CHECK(std::get<0>(gioca("suo", "", true)) == 0);   // la missione ne ha gia' uno sulla nave
    // la Knox: l'SH-2F Seasprite (LAMPS I), senza sonar a immersione
    const auto [nk, ck, bk, lk] = gioca("knox", "", false, "knox", "sh2f", true);
    CHECK(nk == 1);
    CHECK(ck == 1);
    CHECK(bk + lk >= 1);
    // la Type 22: il Lynx col solo MAD, diretto dalla nave col suo contatto ogni 45 s (MATCH), attacca un Victor III
    // immerso che si sposta (in superficie un siluro antisom non si lancia; l'Alfa di titanio il MAD lo sente appena)
    {
        // 30 minuti: 7 di transito, poi i passaggi a quadrifoglio del MAD fino al fix (disassemblatore, 31/10/2026)
        const auto [nl, cl, bl, ll] = gioca("type22", "", false, "type22", "lynx", true, 100.0, "victor3", 30);
        CHECK(nl == 1);
        CHECK(cl == 1);
        CHECK(ll >= 1);
    }
}

namespace {
// la Perry (2) con l'Alfa (3) in superficie oltre i tubi, che resiste, e il contatto dato di continuo: quanti elicotteri fa
// decollare in `ore`, con l'hangar in fiamme prima del decollo o mentre l'elicottero e' in volo. A facile di notte l'aereo
// rientra: la scena comincia prima di sera, cosi' l'elicottero decolla col giorno e torna a bordo col buio
enum class HangarBruciato { no, prima, in_volo };
struct DecolliHangar {
    int decolli = 0;
    bool appontato = false, hangar_perso = false, nave_viva = false;
};
DecolliHangar decolli_con_hangar(const std::string& nome, HangarBruciato quando, double ore) {
    const auto f = fs::temp_directory_path() / ("m688_prova_armi_ia_hangar_" + nome + ".lua");
    std::ofstream(f) << "return { id = 'ARMI_IA_HANGAR_" << nome << "', teatro = 'faroe', lato = 'usa', data = '1989-06-15', ora_utc = 18.8,"
                        " paesaggio_sonoro = false, carriera = { situazione = 'guerra' }, editor = {},"
                        " script = { A = function(u) m688.bersaglio(u, 3) m688.attacca(u, 20, 0) m688.dopo(u, 20, 'A') end },"
                        " varianti = { { probabilita = 1, unita = {\n"
                        "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true, posizione = { x = -90000, y = -80000 },"
                        "  quota_m = 300, rotta = 90, velocita_kn = 3, " << SUB << " },\n"
                        "{ id = 2, classe = 'perry', categoria = 'nave', flag = 10, posizione = { x = 0, y = 0 }, quota_m = 0,"
                        " rotta = 90, velocita_kn = 5, " << NAVE << " },\n"
                        "{ id = 3, classe = 'alfa', categoria = 'sottomarino', flag = 17, posizione = { x = 20000, y = 0 }, quota_m = 0, rotta = 0,"
                        " velocita_kn = 4, modello = { massima_kn = 32, virata_gradi_s = 1, accel_kn_s = 0.5, quota_m_s = 1, quota_max_m = 450,"
                        " integrita = 30000, rumore = 8, sonar = 19 } },\n"
                        "}, timer = { { dopo_s = 30, unita = 2, gestore = 'A' } } } } }\n";
    Partita p(f.string());
    p.m.imposta_difficolta_asw(DifficoltaAsw::facile);
    // il colpo fa nascere la fisica della nave; lo scoppio sull'hangar (da x0 a x1 in perry.toml) gli da' fuoco di sicuro
    const auto brucia = [&] {
        Colpo c;
        c.tipo = Colpo::Tipo::attacco_casuale;
        c.tempo = p.m.tempo();
        c.vittima = 2;
        c.classe_vittima = "perry";
        c.classe_arma = "harpoon";
        p.m.registra_colpo(c);
        p.dm.avanza(p.m, 0.0);
        Esplosione e;
        e.tipo = Esplosione::Tipo::missile;
        e.x = -50.0; e.y = 5.0; e.z = 3.0; e.tnt_kg = 100.0;
        p.dm.nave(2)->danni->colpo(e);
    };
    p.avanza(1.0);
    if (quando == HangarBruciato::prima) brucia();
    DecolliHangar r;
    bool bruciato = quando == HangarBruciato::prima;
    for (long k = 0; k < static_cast<long>(ore * 1800.0); ++k) {
        UnitaClassica* n = p.m.trova(2);
        const UnitaClassica* a = p.m.trova(3);
        if (!a || a->distrutta || a->persa) break;
        if (n) {
            Contatto& c = n->contatti[3];
            c.qualita = 1000.0;
            c.posizione_nota = true;
            c.pos_stimata = a->pos;
            c.rilevamento = rilevamento(n->pos, c.pos_stimata);
            c.ultima_scoperta = p.m.tempo();
            c.sensore = "radio";
        }
        p.m.avanza(2.0);
        p.dm.avanza(p.m, 2.0);
        // ogni decollo e' una configurazione nuova; appontato, l'elicottero esce dalle unita'
        std::vector<int> voli;
        for (const auto& v : p.m.aerei_asw_previsti(M688_DATI))
            if (v.base_unita == 2) voli.push_back(v.unita);
        r.decolli = static_cast<int>(voli.size());
        if (!voli.empty()) {
            const UnitaClassica* h = p.m.trova(voli.front());
            r.appontato = !h || h->persa;
        }
        if (!bruciato && quando == HangarBruciato::in_volo && !voli.empty()) {
            brucia();
            bruciato = true;
        }
    }
    if (const NaveColpita* d = p.dm.nave(2))
        for (const auto& z : d->danni->zone()) r.hangar_perso = r.hangar_perso || z.elicottero_perso;
    const UnitaClassica* n = p.m.trova(2);
    r.nave_viva = n && !n->distrutta && !n->persa;
    fs::remove(f);
    MESSAGE(nome, ": decolli ", r.decolli, ", appontato ", r.appontato, ", hangar perso ", r.hangar_perso, ", nave viva ", r.nave_viva);
    return r;
}
}  // namespace

TEST_CASE("armi dell'IA: dall'hangar bruciato non decolla piu' nessun elicottero" * doctest::skip(!c_e() || !fs::exists(M688_DATI "/asw_aereo.toml"))) {
    // la stessa scena senza incendio: decolla, e dopo il rifornimento decolla di nuovo
    const DecolliHangar illeso = decolli_con_hangar("illeso", HangarBruciato::no, 1.0);
    CHECK(illeso.decolli >= 2);
    CHECK(illeso.appontato);
    // in fiamme prima del decollo: non parte
    const DecolliHangar prima = decolli_con_hangar("prima", HangarBruciato::prima, 0.5);
    CHECK(prima.hangar_perso);
    CHECK(prima.nave_viva);
    CHECK(prima.decolli == 0);
    // in fiamme con l'elicottero in volo: quello torna, ma non riparte
    const DecolliHangar in_volo = decolli_con_hangar("in_volo", HangarBruciato::in_volo, 1.0);
    CHECK(in_volo.hangar_perso);
    CHECK(in_volo.nave_viva);
    CHECK(in_volo.appontato);
    CHECK(in_volo.decolli == 1);
}

namespace {
// l'elicottero della missione (aereo_asw, base_unita): la Perry (2) va a 5 nodi e ha sempre il contatto dell'Alfa (3) oltre i
// tubi. A facile di notte l'aereo rientra: la scena comincia prima di sera, cosi' decolla col giorno e rientra col buio
struct VoloMissione {
    Partita p;
    explicit VoloMissione(const std::string& file) : p(file) { p.m.imposta_difficolta_asw(DifficoltaAsw::facile); }
    void contatto() {
        UnitaClassica* n = p.m.trova(2);
        const UnitaClassica* a = p.m.trova(3);
        if (!n || !a) return;
        Contatto& c = n->contatti[3];
        c.qualita = 1000.0;
        c.posizione_nota = true;
        c.pos_stimata = a->pos;
        c.rilevamento = rilevamento(n->pos, c.pos_stimata);
        c.ultima_scoperta = p.m.tempo();
        c.sensore = "radio";
    }
    void passo() {
        contatto();
        p.m.avanza(2.0);
        p.dm.avanza(p.m, 2.0);
    }
    void brucia_hangar() {
        Colpo c;
        c.tipo = Colpo::Tipo::attacco_casuale;
        c.tempo = p.m.tempo();
        c.vittima = 2;
        c.classe_vittima = "perry";
        c.classe_arma = "harpoon";
        p.m.registra_colpo(c);
        p.dm.avanza(p.m, 0.0);
        Esplosione e;
        e.tipo = Esplosione::Tipo::missile;
        e.x = -50.0; e.y = 5.0; e.z = 3.0; e.tnt_kg = 100.0;
        p.dm.nave(2)->danni->colpo(e);
    }
    int voli() const {   // ogni decollo dalla nave e' una configurazione nuova
        int n = 0;
        for (const auto& v : p.m.aerei_asw_previsti(M688_DATI)) n += v.base_unita == 2;
        return n;
    }
    bool reazione(const std::string& r) const {
        for (const auto& x : p.m.reazioni_ia()) if (x.reazione == r) return true;
        return false;
    }
    bool nel_registro(const std::string& testo) const {
        for (const auto& e : p.m.eventi_missione()) if (e.tipo == TipoEvento::reazione_ia && e.testo_it.find(testo) != std::string::npos) return true;
        return false;
    }
};
std::string scena_voli(const std::string& nome, const std::string& aerei_config, const std::string& aerei, const std::string& script = "", const std::string& timer = "") {
    const auto f = fs::temp_directory_path() / ("m688_prova_armi_ia_voli_" + nome + ".lua");
    std::ofstream(f) << "return { id = 'ARMI_IA_VOLI_" << nome << "', teatro = 'faroe', lato = 'usa', data = '1989-06-15', ora_utc = 18.8,"
                        " paesaggio_sonoro = false, carriera = { situazione = 'guerra' }, editor = {}, aereo_asw = { " << aerei_config << " },"
                        " script = { A = function(u) m688.bersaglio(u, 3) m688.attacca(u, 20, 0) m688.dopo(u, 20, 'A') end, " << script << " },"
                        " varianti = { { probabilita = 1, unita = {\n"
                        "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true, posizione = { x = -90000, y = -80000 },"
                        "  quota_m = 300, rotta = 90, velocita_kn = 3, " << SUB << " },\n"
                        "{ id = 2, classe = 'perry', categoria = 'nave', flag = 10, posizione = { x = 0, y = 0 }, quota_m = 0, rotta = 90, velocita_kn = 5, " << NAVE << " },\n"
                        "{ id = 3, classe = 'alfa', categoria = 'sottomarino', flag = 17, posizione = { x = 20000, y = 0 }, quota_m = 0, rotta = 0, velocita_kn = 4,"
                        " modello = { massima_kn = 32, virata_gradi_s = 1, accel_kn_s = 0.5, quota_m_s = 1, quota_max_m = 450, integrita = 30000, rumore = 8, sonar = 19 } },\n"
                     << aerei << "}, timer = { { dopo_s = 30, unita = 2, gestore = 'A' }, " << timer << " } } } }\n";
    return f.string();
}
bool fuori(const UnitaClassica* u) { return !u || u->distrutta || u->persa; }   // uscita dal gioco, l'unita' sparisce
const std::string SH60_4 = "{ id = 4, classe = 'sh60b', categoria = 'aereo', flag = 11, posizione = { x = 0, y = 0 }, quota_m = 0, rotta = 90, velocita_kn = 0 },\n";
const std::string CONF_4 = "{ unita = 4, missione = 'caccia', situazione = 'guerra', bersagli = { 3 }, base_unita = 2, zona = { x = 20000, y = 0, raggio_m = 15000 } }";
}  // namespace

TEST_CASE("armi dell'IA: l'elicottero della missione torna sulla nave che si e' mossa, apponta e poi riparte" * doctest::skip(!c_e() || !fs::exists(M688_DATI "/asw_aereo.toml"))) {
    const auto f = scena_voli("illeso", CONF_4, SH60_4);
    VoloMissione v(f);
    double t_appontato = -1.0, t_ripartito = -1.0;
    Vec2 dove;
    for (int k = 0; k < 1800 && t_ripartito < 0.0; ++k) {
        v.passo();
        const UnitaClassica* h = v.p.m.trova(4);
        if (h && !h->persa) dove = h->pos;   // uscita dal gioco, l'unita' sparisce: resta il punto dove stava
        else if (t_appontato < 0.0) {
            t_appontato = v.p.m.tempo();
            const UnitaClassica* n = v.p.m.trova(2);
            CHECK(distanza(dove, n->pos) < 1800.0);   // sulla nave (1500 m e l'ultimo passo)
            CHECK(distanza(dove, Vec2{0.0, 0.0}) > 1500.0);   // non dove la nave stava all'inizio
        }
        if (v.voli() >= 2) t_ripartito = v.p.m.tempo();
    }
    fs::remove(f);
    MESSAGE("appontato a ", t_appontato, " s, ripartito a ", t_ripartito, " s");
    REQUIRE(t_appontato > 0.0);
    REQUIRE(t_ripartito > 0.0);
    CHECK(t_ripartito - t_appontato >= 1800.0);   // il rifornimento
}

TEST_CASE("armi dell'IA: l'elicottero della missione con l'hangar bruciato apponta e non riparte; con la nave persa ammara" * doctest::skip(!c_e() || !fs::exists(M688_DATI "/asw_aereo.toml"))) {
    SUBCASE("hangar bruciato in volo") {
        const auto f = scena_voli("bruciato", CONF_4, SH60_4);
        VoloMissione v(f);
        for (int k = 0; k < 1800; ++k) {
            if (k == 100) v.brucia_hangar();
            v.passo();
        }
        fs::remove(f);
        CHECK(fuori(v.p.m.trova(4)));   // a bordo
        CHECK_FALSE(v.reazione("ammaraggio"));
        CHECK(v.voli() == 1);   // non ne riparte un altro
    }
    SUBCASE("nave persa in volo") {
        const auto f = scena_voli("nave_persa", CONF_4, SH60_4);
        VoloMissione v(f);
        Vec2 ultima, dove;
        for (int k = 0; k < 900; ++k) {
            if (k == 100) {
                UnitaClassica* n = v.p.m.trova(2);
                ultima = n->pos;
                n->persa = true;
            }
            v.passo();
            if (const UnitaClassica* h = v.p.m.trova(4); !fuori(h)) dove = h->pos;
        }
        fs::remove(f);
        CHECK(fuori(v.p.m.trova(4)));
        CHECK(distanza(dove, ultima) < 1800.0);
        CHECK(v.reazione("ammaraggio"));
        CHECK(v.nel_registro("ammara"));
    }
}

TEST_CASE("armi dell'IA: l'elicottero che entra per evento non parte dalla nave con l'hangar bruciato" * doctest::skip(!c_e() || !fs::exists(M688_DATI "/asw_aereo.toml"))) {
    const auto gioca = [](const std::string& nome, bool bruciato) {
        const auto f = scena_voli(nome, "{ classe = 'sh60b', missione = 'caccia', situazione = 'guerra', bersagli = { 3 }, base_unita = 2, zona = { x = 20000, y = 0, raggio_m = 15000 } }", "",
                                  "E = function(u) m688.modello_azzera(u) m688.modello_classe(u, 'sh60b') m688.modello_flag(u, 11) m688.nuova_unita(u) end",
                                  "{ dopo_s = 60, unita = 2, gestore = 'E' }");
        VoloMissione v(f);
        if (bruciato) {
            v.p.avanza(1.0);
            v.brucia_hangar();
        }
        for (int k = 0; k < 60; ++k) v.passo();   // l'hangar brucia prima dell'evento
        fs::remove(f);
        int entrati = 0;
        for (const auto& [id, u] : v.p.m.unita())
            if (u.classe == "sh60b" && !u.persa) ++entrati;
        return std::pair<int, bool>{entrati, v.reazione("hangar_perso") && v.nel_registro("non decolla")};
    };
    CHECK(gioca("evento_illeso", false) == std::pair<int, bool>{1, false});
    CHECK(gioca("evento_bruciato", true) == std::pair<int, bool>{0, true});
}

TEST_CASE("armi dell'IA: l'aereo con la base fissa, o senza base, esce di scena all'arrivo" * doctest::skip(!c_e() || !fs::exists(M688_DATI "/asw_aereo.toml"))) {
    const auto f = scena_voli("base_fissa",
                              "{ unita = 4, missione = 'caccia', situazione = 'guerra', bersagli = { 3 }, base = { x = -30000, y = 0 }, zona = { x = 20000, y = 0, raggio_m = 15000 } },"
                              "{ unita = 5, missione = 'caccia', situazione = 'guerra', bersagli = { 3 }, zona = { x = 20000, y = 0, raggio_m = 15000 } }",
                              "{ id = 4, classe = 'p3c', categoria = 'aereo', flag = 11, posizione = { x = -30000, y = 0 }, quota_m = 300, rotta = 90, velocita_kn = 200 },\n"
                              "{ id = 5, classe = 'sh60b', categoria = 'aereo', flag = 11, posizione = { x = -5000, y = 0 }, quota_m = 0, rotta = 90, velocita_kn = 0 },\n");
    VoloMissione v(f);
    for (int k = 0; k < 1200; ++k) v.passo();
    fs::remove(f);
    for (const int id : {4, 5}) {
        CAPTURE(id);
        CHECK(fuori(v.p.m.trova(id)));
    }
    CHECK_FALSE(v.reazione("ammaraggio"));
}

namespace {
// il Victor (3) sordo (cupola guasta) con la posizione della Perry (2) data a mano, vecchia di `eta_s`; poi lancia una volta
struct LancioAMano {
    int partiti = 0, persi = 0, tubi_pieni = 0;
};
LancioAMano lancio_a_mano(const std::string& nome, double distanza_m, double eta_s, bool lo_sente = false) {
    const auto f = scena(nome, "guerra",
        "{ id = 2, classe = 'perry', categoria = 'nave', flag = 10, posizione = { x = 0, y = " + std::to_string(distanza_m) + " }, quota_m = 0, rotta = 90,"
        " velocita_kn = 0, " + NAVE + " },"
        "{ id = 3, classe = 'victor3', categoria = 'sottomarino', flag = 17, posizione = { x = 0, y = 0 }, quota_m = 100, rotta = 90, velocita_kn = 5, " + SUB + " },",
        LANCIA, "{ dopo_s = 5, unita = 3, gestore = 'L' }", false);
    Partita p(f);
    p.m.acustica_enhanced(M688_DATI);
    p.m.sonar_ia(3, false, false);
    p.avanza(4.0);
    for (int i = 0; i < 27; ++i) {   // il contatto rimesso a ogni passo: la valutazione lo rifarebbe
        auto& k = p.m.trova(3)->contatti[2];
        k.posizione_nota = true;
        k.pos_stimata = p.m.unita().at(2).pos;
        k.qualita = 1000.0;
        k.t_posizione = p.m.tempo() - eta_s;
        k.ultima_scoperta = lo_sente ? p.m.tempo() : k.t_posizione;   // l'ascolto passivo rinnova la scoperta, non la posizione
        p.avanza(1.0 / 9.0);
    }
    LancioAMano r;
    for (const auto& [id, u] : p.m.unita()) {
        if (u.lanciatore != 3 || !(u.flag & 0x20) || (u.flag & 0x80)) continue;
        if (p.m.siluro(id)) ++r.partiti;
        else if (u.persa) ++r.persi;
    }
    if (const auto* c = p.m.carico_ia_di(3))
        r.tubi_pieni = static_cast<int>(std::count_if(c->tubi.begin(), c->tubi.end(), [](const std::string& v) { return !v.empty(); }));
    fs::remove(f);
    return r;
}
}  // namespace

TEST_CASE("armi dell'IA: niente siluro oltre la portata ne' su una posizione vecchia; il tubo resta carico (Oscar di SCENE9C)" * doctest::skip(!c_e())) {
    const auto buono = lancio_a_mano("tiro_buono", 6000.0, 0.0);
    CHECK(buono.partiti == 1);
    CHECK(buono.tubi_pieni == 3);
    const auto lontano = lancio_a_mano("tiro_lontano", 40000.0, 0.0);   // oltre ogni siluro del Victor
    CHECK(lontano.partiti == 0);
    CHECK(lontano.tubi_pieni == 4);
    const auto vecchio = lancio_a_mano("tiro_vecchio", 6000.0, 600.0);   // 10 minuti: a 15 nodi 4,6 km
    CHECK(vecchio.partiti == 0);
    CHECK(vecchio.tubi_pieni == 4);
    // la stessa posizione vecchia, col bersaglio che si sente ancora: sapere dov'era 10 minuti fa non basta
    const auto sentito = lancio_a_mano("tiro_sentito", 6000.0, 600.0, true);
    CHECK(sentito.partiti == 0);
    CHECK(sentito.tubi_pieni == 4);
}

// Il carico vivo dei depositi (danni critici, fase 3; BattelloDanni::carico_deposito): la dotazione dell'IA e la stazione
// Armi del giocatore danno i kg d'esplosivo del locale siluri, che scendono coi lanci e non con le ricariche

namespace {
double deposito(const DanniSottomarini& ds, int unita) {
    double kg = 0.0;
    if (const auto* c = ds.compartimenti(unita))
        for (const auto& x : *c) if (x.uso == "deposito") kg += x.carica_kg;
    return kg;
}
}  // namespace

TEST_CASE("carico vivo dei depositi: il Victor dell'IA si vuota coi lanci, non con le ricariche; il deposito saltato resta vuoto" * doctest::skip(!c_e())) {
    const auto f = scena("deposito", "guerra",
        "{ id = 2, classe = 'perry', categoria = 'nave', flag = 10, posizione = { x = 0, y = 6000 }, quota_m = 0, rotta = 90, velocita_kn = 5, " + NAVE + " },"
        "{ id = 3, classe = 'victor3', categoria = 'sottomarino', flag = 17, posizione = { x = 0, y = 0 }, quota_m = 100, rotta = 90, velocita_kn = 5, " + SUB + " },",
        LANCIA, "{ dopo_s = 5, unita = 3, gestore = 'L' }, { dopo_s = 6, unita = 3, gestore = 'L' }, { dopo_s = 7, unita = 3, gestore = 'L' },"
                "{ dopo_s = 8, unita = 3, gestore = 'L' }", false);
    Partita p(f);
    DanniSottomarini ds(p.classi, M688_DATI, 1);
    const auto passo = [&](double s) {
        for (long i = 0; i < static_cast<long>(s * 9.0); ++i) {
            p.m.avanza(1.0 / 9.0);
            p.dm.avanza(p.m, 1.0 / 9.0);
            ds.avanza(p.m, 1.0 / 9.0);
        }
    };
    passo(2.0);
    REQUIRE(p.m.carico_ia_di(3) != nullptr);
    ds.prepara(p.m, 3);   // il modello del battello prima del primo colpo
    passo(1.0);
    const auto kg0 = p.m.esplosivo_ia(3);
    REQUIRE(kg0.has_value());
    CHECK(*kg0 > 18 * 200.0);   // 18 siluri da 533 mm, 205-300 kg d'esplosivo
    CHECK(*kg0 < 18 * 310.0);
    CHECK(deposito(ds, 3) == doctest::Approx(*kg0));
    passo(12.0);   // quattro lanci
    const auto kg1 = p.m.esplosivo_ia(3);
    REQUIRE(kg1.has_value());
    CHECK(*kg1 < *kg0 - 4 * 200.0);
    CHECK(deposito(ds, 3) == doctest::Approx(*kg1));
    passo(320.0);   // i tubi ricaricati dalla riserva: le armi restano nel locale
    CHECK(p.m.esplosivo_ia(3).value_or(-1.0) == doctest::Approx(*kg1));
    CHECK(deposito(ds, 3) == doctest::Approx(*kg1));
    // il deposito saltato (qui a mano: carica e propellente a zero) resta vuoto anche se la dotazione dice altro
    const auto& sub = *ds.sottomarino(3);
    for (std::size_t k = 0; k < sub.danni->compartimenti().size(); ++k)
        if (sub.danni->compartimenti()[k].uso == "deposito") sub.danni->carico_deposito(static_cast<int>(k), 0.0);
    REQUIRE(deposito(ds, 3) == 0.0);
    passo(1.0);
    ds.carico_vivo(3, *kg1);
    CHECK(deposito(ds, 3) == 0.0);
    CHECK_FALSE(p.m.esplosivo_ia(2).has_value());   // la Perry non ha il carico dei sottomarini
    fs::remove(f);
}

TEST_CASE("carico vivo dei depositi: il 688 del giocatore, dalla stazione Armi (carica, lancio, ricarica)" *
          doctest::skip(!fs::exists(std::string(M688_MISSIONI) + "/SCENE1A.lua") || !c_e())) {
    const auto cat = carica_carico(M688_DATI "/carico.toml");
    const auto classi = carica_classi(M688_DATI "/classi.toml");
    const auto mis = carica_missili(M688_DATI "/missili.toml");
    // le testate: siluri e mine intere, i missili l'esplosivo_kg di missili.toml (quello del colpo), le esche niente; senza
    // il catalogo dei missili la frazione 0,45 della testata
    CHECK(esplosivo_arma(cat, "mk48") == doctest::Approx(cat.arma("mk48")->testata_kg));
    CHECK(esplosivo_arma(cat, "harpoon", &mis, 1989) == doctest::Approx(mis.per_arma("harpoon", 1989)->esplosivo_kg));
    CHECK(esplosivo_arma(cat, "tasm", &mis, 1989) == doctest::Approx(205.0));
    CHECK(esplosivo_arma(cat, "harpoon") == doctest::Approx(0.45 * 221.0));
    CHECK(esplosivo_arma(cat, "adc_mk2") == 0.0);
    CHECK(esplosivo_arma(cat, "nessuna") == 0.0);
    Missione m(std::string(M688_MISSIONI) + "/SCENE1A.lua", 1);
    StazioniMissione st(m);
    m.siluri_enhanced(M688_DATI);
    SalaSiluri sala(cat, "los_angeles", m.limiti_carico());
    sala.carico_iniziale();
    st.imposta_carico(m, sala.carico(), cat);
    const int g = m.giocatore()->id;
    DanniSottomarini ds(classi, M688_DATI, 1);
    ds.prepara(m, g);
    RegistroOrdini ordini;
    st.usa_danni(&ds);   // come la partita: il carico vivo lo da' StazioniMissione::dopo_passo
    const auto avanza = [&](double s) {
        for (int i = 0; i < static_cast<int>(std::lround(s * 9.0)); ++i) {
            for (const auto& o : ordini.consegna(m.tempo())) st.consegna(m, o);
            m.avanza(1.0 / 9.0);
            st.dopo_passo(m);
        }
    };
    avanza(1.0 / 9.0);
    // SCENE1A: 22 Mk 48 e 2 Harpoon (sul Flight I nei tubi da siluri e in rastrelliera), le esche a parte
    const double kg0 = esplosivo_a_bordo(st.armi(), cat, m.catalogo_missili(), m.limiti_carico().anno);
    const double harpoon = m.catalogo_missili() ? m.catalogo_missili()->per_arma("harpoon", m.limiti_carico().anno)->esplosivo_kg : 0.45 * 221.0;
    CHECK(kg0 == doctest::Approx(22 * cat.arma("mk48")->testata_kg + 2 * harpoon));
    CHECK(deposito(ds, g) == doctest::Approx(kg0));
    ordini.ordina({m.tempo(), TipoOrdine::carica_tubo, 0, 0, 1});
    avanza(21.0);
    REQUIRE(st.armi().stato(1) == StatoTubo::armato);
    CHECK(deposito(ds, g) == doctest::Approx(kg0));   // armare non cambia niente
    for (const auto& [id, u] : m.unita())   // nessuno a tiro
        if (id != g) m.trova(id)->pos.x += 400000.0;
    ordini.ordina(st.armi().lancio(m.tempo()));
    avanza(2.0);
    const double kg1 = kg0 - cat.arma(st.armi().lanciata(1))->testata_kg;
    CHECK(deposito(ds, g) == doctest::Approx(kg1));
    avanza(400.0);   // la ricarica del tubo dalla rastrelliera
    CHECK(deposito(ds, g) == doctest::Approx(kg1));
    CHECK(m.errori().empty());
}
