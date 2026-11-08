#include <doctest.h>

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <map>
#include <set>
#include <string>
#include <tuple>
#include <vector>

#include "missioni/danni_missione.hpp"
#include "missioni/interventi.hpp"
#include "missioni/missione.hpp"
#include "nucleo/ordini.hpp"
#include "missioni/registro_missione.hpp"
#include "nucleo/classi.hpp"
#include "nucleo/geometria.hpp"
#include "nucleo/sonarista.hpp"

using namespace m688;
namespace fs = std::filesystem;

// Fase 3 delle interazioni fra unita' dell'IA (docs/18_interazioni_ia.md; nota condivisa ia-fase3-contratto.md): allerta
// della forza e della formazione, situazione per coppia di parti con le provocazioni, ROE dell'IA, attacca con la portata.

namespace {

const std::string SUB = "modello = { massima_kn = 32, virata_gradi_s = 1, accel_kn_s = 0.5, quota_m_s = 1, quota_max_m = 450, integrita = 200, rumore = 8, sonar = 19 }";
const std::string NAVE = "modello = { massima_kn = 32, virata_gradi_s = 2, accel_kn_s = 0.5, quota_m_s = 1, quota_max_m = 0, integrita = 100, rumore = 10, sonar = 20 }";

// il giocatore lontano e profondo; una nave americana (2) e una sovietica (3) a `distanza_m`, e una seconda americana
// (4) lontana dalla prima; `situazione` nel blocco carriera
std::string scena(const std::string& nome, const std::string& situazione, double distanza_m, const std::string& script = "",
                  const std::string& timer = "") {
    const auto f = fs::temp_directory_path() / ("m688_prova_forze_" + nome + ".lua");
    std::ofstream(f) << "return { id = 'FORZE_" << nome << "', teatro = 'faroe', lato = 'usa', data = '1989-06-15', ora_utc = 12,"
                        " paesaggio_sonoro = false, carriera = { situazione = '" << situazione << "' },"
                        " script = {" << script << "}, varianti = { { probabilita = 1, unita = {\n"
                        "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true, posizione = { x = 0, y = -80000 },"
                        "  quota_m = 300, rotta = 90, velocita_kn = 3, " << SUB << " },\n"
                        "{ id = 2, classe = 'perry', categoria = 'nave', flag = 10, posizione = { x = 0, y = 0 }, quota_m = 0, rotta = 90, velocita_kn = 5, " << NAVE << " },\n"
                        "{ id = 3, classe = 'krivak2', categoria = 'nave', flag = 18, posizione = { x = 0, y = " << distanza_m << " }, quota_m = 0, rotta = 90, velocita_kn = 5, " << NAVE << " },\n"
                        "{ id = 4, classe = 'perry', categoria = 'nave', flag = 10, posizione = { x = 60000, y = 0 }, quota_m = 0, rotta = 90, velocita_kn = 5, " << NAVE << " },\n"
                        "}, timer = {" << timer << "} } } }\n";
    return f.string();
}

struct Partita {
    std::map<std::string, ClasseNave> classi = carica_classi(M688_DATI "/classi.toml");
    Missione m;
    DanniMissione dm;
    explicit Partita(const std::string& file) : m(file, 1, false), dm(classi, M688_DATI, 1, RegolaDanni::simulazione) {}
    void avanza(double s) { for (long i = 0; i < static_cast<long>(s * 9.0); ++i) { m.avanza(1.0 / 9.0); dm.avanza(m, 1.0 / 9.0); } }
    int eventi(TipoEvento t) const {
        return static_cast<int>(std::count_if(m.eventi_missione().begin(), m.eventi_missione().end(), [&](const EventoMissione& e) { return e.tipo == t; }));
    }
};

// la nave americana 2 attacca la sovietica 3 (ATTACK, una carica: con difesa 1 riesce quasi sempre)
const std::string ATTACCA = "A = function(u) m688.bersaglio(u, 3) m688.attacca(u, 1, 1) m688.salva_modello(u) end";
const std::string ATTACCA_DOPO = "{ dopo_s = 5, unita = 2, gestore = 'A' }";

}  // namespace

TEST_CASE("forze: in crisi le ROE dell'IA trattengono le armi, un atto ostile porta la coppia in guerra" * doctest::skip(!fs::exists(M688_DATI "/interazioni.toml"))) {
    const auto f = scena("crisi", "crisi", 2000.0, ATTACCA, ATTACCA_DOPO);
    Partita p(f);
    p.avanza(2.0);
    REQUIRE(p.m.con_forze());
    CHECK(p.m.situazione_fra(0x08, 0x10) == Situazione::crisi);
    CHECK(p.m.allerta(0x08) == 1);   // il minimo della crisi
    CHECK(p.m.armi_libere_ia(3, 2) == false);
    CHECK(p.m.armi_libere_ia(2, 3) == false);
    p.avanza(10.0);
    // la carica della nave americana: provocazione, guerra fra le due parti in tutti e due i versi
    CHECK(p.m.situazione_fra(0x08, 0x10) == Situazione::guerra);
    CHECK(p.m.situazione_fra(0x10, 0x08) == Situazione::guerra);
    REQUIRE(p.m.provocazioni().size() == 1);
    CHECK(p.m.provocazioni()[0].colpito == 0x10);
    CHECK(p.m.provocazioni()[0].provocatore == 0x08);
    CHECK(p.m.armi_libere_ia(3, 2) == true);
    CHECK(p.eventi(TipoEvento::provocazione) == 1);
    // i neutrali restano fuori
    CHECK(p.m.situazione_fra(0, 0x08) == Situazione::pace);
    fs::remove(f);
}

TEST_CASE("forze: l'arma porta la formazione a combattimento subito, il resto della forza sale al FLASH" * doctest::skip(!fs::exists(M688_DATI "/interazioni.toml"))) {
    const auto f = scena("allerta", "crisi", 2000.0, ATTACCA, ATTACCA_DOPO);
    Partita p(f);
    p.avanza(12.0);
    // chi attacca e chi e' colpito: A3; la seconda americana, a 60 km, no
    CHECK(p.m.allerta_unita(2) == 3);
    CHECK(p.m.allerta_unita(3) == 3);
    CHECK(p.m.allerta_unita(4) == 2);   // la guerra: almeno A2
    bool formazione = false;
    for (const auto& c : p.m.cambi_allerta()) formazione = formazione || (c.unita == 2 && c.a == 3 && c.causa == "arma");
    CHECK(formazione);
    CHECK(p.eventi(TipoEvento::allerta_forza) >= 2);
    // ad A3 il sonarista dell'IA prende tre volte piu' spesso un biologico per un sottomarino
    if (const Sonarista* s = p.m.sonarista_ia(2))
        CHECK(s->profilo().biologico_come_sub == doctest::Approx(std::min(0.9, 3.0 * profilo_sonarista(s->profilo().soglia_db <= 0.0 ? Esperienza::esperto : s->profilo().soglia_db >= 3.0 ? Esperienza::recluta : Esperienza::medio).biologico_come_sub)));
    // dopo il FLASH (5-15 minuti) la forza americana sa dell'arma: +1
    p.avanza(1000.0);
    CHECK(p.m.allerta(0x08).value_or(0) >= 3);
    fs::remove(f);
}

TEST_CASE("forze: m688.situazione dichiara la guerra e alza i minimi" * doctest::skip(!fs::exists(M688_DATI "/interazioni.toml"))) {
    const auto f = scena("dichiara", "pace", 20000.0, "G = function(u) m688.situazione(u, 'guerra') end", "{ dopo_s = 5, unita = 2, gestore = 'G' }");
    Partita p(f);
    p.avanza(2.0);
    CHECK(p.m.situazione_fra(0x08, 0x10) == Situazione::pace);
    CHECK(p.m.armi_libere_ia(2) == false);
    p.avanza(10.0);
    CHECK(p.m.situazione_fra(0x08, 0x10) == Situazione::guerra);
    CHECK(p.m.armi_libere_ia(2) == true);
    CHECK(p.m.allerta(0x10) == 2);
    fs::remove(f);
}

TEST_CASE("forze: nel classico non ci sono, e attacca colpisce senza portata") {
    const auto f = scena("classico", "crisi", 20000.0, ATTACCA, ATTACCA_DOPO);
    Missione m(f, 1, false);
    for (int i = 0; i < 9 * 12; ++i) m.avanza(1.0 / 9.0);
    CHECK_FALSE(m.con_forze());
    CHECK_FALSE(m.allerta_unita(2).has_value());
    CHECK_FALSE(m.armi_libere_ia(2, 3).has_value());
    CHECK(m.colpi().size() == 1);   // ATTACK dell'originale: la portata si legge e si scarta
    fs::remove(f);
}

TEST_CASE("forze: nell'Enhanced attacca non colpisce oltre la portata" * doctest::skip(!fs::exists(M688_DATI "/interazioni.toml"))) {
    const auto f = scena("portata", "guerra", 20000.0, ATTACCA, ATTACCA_DOPO);
    Partita p(f);
    p.avanza(12.0);
    CHECK(p.m.colpi().empty());
    fs::remove(f);
}

TEST_CASE("forze: il rapporto di contatto arriva alla formazione in un paio di minuti e al resto della forza col FLASH" * doctest::skip(!fs::exists(M688_DATI "/interazioni.toml"))) {
    // un Victor a 18 nodi a 6 km dalla Perry 2, che lo sente; la Sacramento 5 senza sonar a 12 km dalla 2, nella sua
    // formazione; una seconda Sacramento (4) a 60 km
    const auto f = fs::temp_directory_path() / "m688_prova_forze_messaggi.lua";
    const std::string SORDA = "modello = { massima_kn = 20, virata_gradi_s = 2, accel_kn_s = 0.5, quota_m_s = 1, quota_max_m = 0, integrita = 100, rumore = 10, sonar = 0 }";
    std::ofstream(f) << "return { id = 'FORZE_MSG', teatro = 'faroe', lato = 'usa', data = '1989-06-15', ora_utc = 12,"
                        " paesaggio_sonoro = false, carriera = { situazione = 'crisi' }, script = {}, varianti = { { probabilita = 1, unita = {\n"
                        "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true, posizione = { x = 0, y = -80000 },"
                        "  quota_m = 300, rotta = 90, velocita_kn = 3, " << SUB << " },\n"
                        "{ id = 2, classe = 'perry', categoria = 'nave', flag = 10, posizione = { x = 0, y = 0 }, quota_m = 0, rotta = 90, velocita_kn = 5, " << NAVE << " },\n"
                        "{ id = 3, classe = 'victor3', categoria = 'sottomarino', flag = 17, posizione = { x = 0, y = 6000 }, quota_m = 40, rotta = 90, velocita_kn = 18, " << SUB << " },\n"
                        "{ id = 4, classe = 'sacramento', categoria = 'nave', flag = 10, posizione = { x = 60000, y = 0 }, quota_m = 0, rotta = 90, velocita_kn = 5, " << SORDA << " },\n"
                        "{ id = 5, classe = 'sacramento', categoria = 'nave', flag = 10, posizione = { x = 0, y = -12000 }, quota_m = 0, rotta = 90, velocita_kn = 5, " << SORDA << " },\n"
                        "}, timer = {} } } }\n";
    Partita p(f.string());
    double formazione = -1.0, teatro = -1.0;
    for (int s = 0; s < 2500 && (formazione < 0.0 || teatro < 0.0); ++s) {
        p.avanza(1.0);
        if (formazione < 0.0 && p.m.contatto_radio(5, 3)) formazione = p.m.tempo();
        if (teatro < 0.0 && p.m.contatto_radio(4, 3)) teatro = p.m.tempo();
    }
    CHECK(formazione > 0.0);
    CHECK(formazione < 300.0);
    CHECK(teatro > formazione);
    const auto c = p.m.contatto_radio(5, 3);
    REQUIRE(c.has_value());
    CHECK(c->r.da == 2);
    CHECK((c->r.categoria == "sottomarino" || c->r.categoria == "sconosciuto"));
    if (c->r.pos) CHECK(distanza(*c->r.pos, p.m.unita().at(3).pos) < 10000.0);
    else {   // col solo rilevamento: verso nord, o il suo specchio sulla schiera (la Perry va per 90 e puo' credere il lato falso)
        const double ril = normalizza_180(c->r.rilevamento);
        CHECK((std::fabs(ril) < 45.0 || std::fabs(normalizza_180(180.0 - ril)) < 45.0));
    }
    CHECK(c->raggio_m >= 1852.0);
    CHECK(p.eventi(TipoEvento::messaggio_ia) >= 2);
    fs::remove(f);
}

TEST_CASE("forze: navi e aerei non usano armi antisom nell'area d'azione di un sottomarino amico" * doctest::skip(!fs::exists(M688_DATI "/interazioni.toml"))) {
    // in guerra una Perry e un Victor; un 688 dell'IA (6) diretto a nord: a 10 km davanti al Victor il Victor e' nella
    // sua area (30 nm dietro, 5 per lato), a 40 km di lato no
    const auto prova = [](double x_amico) {
        const auto f = fs::temp_directory_path() / "m688_prova_forze_zone.lua";
        std::ofstream(f) << "return { id = 'FORZE_ZONE', teatro = 'faroe', lato = 'usa', data = '1989-06-15', ora_utc = 12,"
                            " paesaggio_sonoro = false, carriera = { situazione = 'guerra' }, script = {}, varianti = { { probabilita = 1, unita = {\n"
                            "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true, posizione = { x = -90000, y = -80000 },"
                            "  quota_m = 300, rotta = 90, velocita_kn = 3, " << SUB << " },\n"
                            "{ id = 2, classe = 'perry', categoria = 'nave', flag = 10, posizione = { x = 0, y = -3000 }, quota_m = 0, rotta = 90, velocita_kn = 5, " << NAVE << " },\n"
                            "{ id = 3, classe = 'victor3', categoria = 'sottomarino', flag = 17, posizione = { x = 0, y = 0 }, quota_m = 100, rotta = 90, velocita_kn = 5, " << SUB << " },\n"
                            "{ id = 6, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, posizione = { x = " << x_amico << ", y = 10000 }, quota_m = 150, rotta = 0, velocita_kn = 5, " << SUB << " },\n"
                            "}, timer = {} } } }\n";
        Partita p(f.string());
        p.avanza(2.0);
        const bool zona = p.m.in_zona_amica(2, 3);
        const auto armi = p.m.armi_libere_ia(2, 3);
        // l'ASW aereo (fase 4): lo sgancio sul punto del Victor, e sul punto senza bersaglio
        const bool sgancio = !p.m.sgancio_vietato(2, 3, {0.0, 0.0}) && !p.m.sgancio_vietato(2, -1, {0.0, 0.0});
        fs::remove(f);
        return std::make_tuple(zona, armi.value_or(true), sgancio);
    };
    const auto [dentro, armi_dentro, sgancio_dentro] = prova(0.0);
    CHECK(dentro);
    CHECK_FALSE(armi_dentro);
    CHECK_FALSE(sgancio_dentro);
    const auto [fuori, armi_fuori, sgancio_fuori] = prova(40000.0);
    CHECK_FALSE(fuori);
    CHECK(armi_fuori);
    CHECK(sgancio_fuori);
}

TEST_CASE("forze: il FLASH della missione alla parte del giocatore alza l'allerta della forza (avviso radio)" * doctest::skip(!fs::exists(M688_DATI "/interazioni.toml"))) {
    // il giocatore a 300 m non lo riceve; l'IA della parte si', alla prima messa in onda. Un IMMEDIATE no
    const auto prova = [](const std::string& precedenza) {
        const auto f = fs::temp_directory_path() / "m688_prova_forze_flash.lua";
        std::ofstream(f) << "return { id = 'FORZE_FLASH', teatro = 'faroe', lato = 'usa', data = '1989-06-15', ora_utc = 12,"
                            " paesaggio_sonoro = false, carriera = { situazione = 'crisi' },"
                            " radio = { { id = 'AVVISO', dopo_s = 60, precedenza = '" << precedenza << "', da = 'COMSUBLANT', oggetto = 'THREAT WARNING',"
                            " testo = { it = 'Sottomarino nemico in zona', en = 'ENEMY SUBMARINE IN AREA' } } },"
                            " script = {}, varianti = { { probabilita = 1, unita = {\n"
                            "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true, posizione = { x = 0, y = -80000 },"
                            "  quota_m = 300, rotta = 90, velocita_kn = 3, " << SUB << " },\n"
                            "{ id = 2, classe = 'perry', categoria = 'nave', flag = 10, posizione = { x = 0, y = 0 }, quota_m = 0, rotta = 90, velocita_kn = 5, " << NAVE << " },\n"
                            "{ id = 3, classe = 'krivak2', categoria = 'nave', flag = 18, posizione = { x = 0, y = 90000 }, quota_m = 0, rotta = 90, velocita_kn = 5, " << NAVE << " },\n"
                            "}, timer = {} } } }\n";
        Partita p(f.string());
        p.avanza(1.0);   // le forze nascono al primo passo
        const int prima = p.m.allerta(0x08).value_or(-1), sovietici = p.m.allerta(0x10).value_or(-1);
        p.avanza(1200.0);
        const bool flash = std::any_of(p.m.cambi_allerta().begin(), p.m.cambi_allerta().end(),
                                       [](const Missione::CambioAllerta& c) { return c.causa == "flash" && c.lato == 0x08 && c.unita == -1; });
        CHECK(p.m.allerta(0x10).value_or(-1) == sovietici);   // l'altra parte non lo sa
        fs::remove(f);
        return std::make_tuple(flash, prima, p.m.allerta(0x08).value_or(-1));
    };
    const auto [flash, prima, dopo] = prova("flash");
    CHECK(flash);
    CHECK(dopo == std::min(prima + 1, 3));
    const auto [immediate, prima_i, dopo_i] = prova("immediate");
    CHECK_FALSE(immediate);
    CHECK(dopo_i == prima_i);
}

TEST_CASE("forze: la parte del luogo (M7) in guerra con gli USA e in pace con l'URSS, finche' uno script non cambia" * doctest::skip(!fs::exists(M688_DATI "/interazioni.toml"))) {
    // una Perry (2), una Krivak (3) e una motovedetta del luogo (5); a 30 s lo script fa la pace fra il luogo e gli USA
    const auto f = fs::temp_directory_path() / "m688_prova_forze_luogo.lua";
    std::ofstream(f) << "return { id = 'FORZE_LUOGO', teatro = 'faroe', lato = 'usa', data = '1989-06-15', ora_utc = 12,"
                        " paesaggio_sonoro = false, carriera = { situazione = 'pace' }, situazione_luogo = { usa = 'guerra', urss = 'pace' },"
                        " script = { P = function(u) m688.situazione(u, 'pace', 'luogo', 'usa') end }, varianti = { { probabilita = 1, unita = {\n"
                        "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true, posizione = { x = 0, y = -80000 },"
                        "  quota_m = 300, rotta = 90, velocita_kn = 3, " << SUB << " },\n"
                        "{ id = 2, classe = 'perry', categoria = 'nave', flag = 10, posizione = { x = 0, y = 0 }, quota_m = 0, rotta = 90, velocita_kn = 5, " << NAVE << " },\n"
                        "{ id = 3, classe = 'krivak2', categoria = 'nave', flag = 18, posizione = { x = 0, y = 9000 }, quota_m = 0, rotta = 90, velocita_kn = 5, " << NAVE << " },\n"
                        "{ id = 5, classe = 'grisha3', categoria = 'nave', schieramento = 'luogo', flag = 2, posizione = { x = 5000, y = 4000 }, quota_m = 0, rotta = 90, velocita_kn = 5, " << NAVE << " },\n"
                        "}, timer = { { dopo_s = 30, unita = 5, gestore = 'P' } } } } }\n";
    Partita p(f.string());
    p.avanza(2.0);
    CHECK((p.m.unita().at(5).flag & 0x18) == 0x18);
    CHECK(p.m.situazione_fra(0x18, 0x08) == Situazione::guerra);
    CHECK(p.m.situazione_fra(0x18, 0x10) == Situazione::pace);
    CHECK(p.m.situazione_fra(0x08, 0x10) == Situazione::pace);
    CHECK(p.m.armi_libere_ia(5, 2).value_or(false));
    CHECK_FALSE(p.m.armi_libere_ia(5, 3).value_or(true));
    CHECK(p.m.armi_libere_ia(2, 5).value_or(false));
    CHECK_FALSE(p.m.armi_libere_ia(2, 3).value_or(true));   // fra le due parti resta la pace
    p.avanza(40.0);
    CHECK(p.m.situazione_fra(0x18, 0x08) == Situazione::pace);
    CHECK_FALSE(p.m.armi_libere_ia(5, 2).value_or(true));
    fs::remove(f);
}

// l'allerta non sale per una nave classificata (la principale, 30/10/2026: «Perry classifica Perry» portava la formazione
// ad A3 al secondo 0 in 14 missioni della libreria): due Perry a 2 km e un mercantile, in pace, per 5 minuti. Le
// classificazioni ci sono, ma nessuna sale l'allerta per possub o probsub
TEST_CASE("forze: la classificazione d'una nave, amica o neutrale, non alza l'allerta" * doctest::skip(!fs::exists(M688_DATI "/interazioni.toml"))) {
    const auto f = fs::temp_directory_path() / "m688_prova_forze_classifica_amico.lua";
    std::ofstream(f) << "return { id = 'FORZE_AMICO', teatro = 'faroe', lato = 'usa', data = '1989-06-15', ora_utc = 12,"
                        " paesaggio_sonoro = false, carriera = { situazione = 'pace' }, script = {}, varianti = { { probabilita = 1, unita = {\n"
                        "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true, posizione = { x = 0, y = -80000 },"
                        "  quota_m = 300, rotta = 90, velocita_kn = 3, " << SUB << " },\n"
                        "{ id = 2, classe = 'perry', categoria = 'nave', flag = 10, posizione = { x = 0, y = 0 }, quota_m = 0, rotta = 90, velocita_kn = 12, " << NAVE << " },\n"
                        "{ id = 3, classe = 'perry', categoria = 'nave', flag = 10, posizione = { x = 0, y = 2000 }, quota_m = 0, rotta = 90, velocita_kn = 12, " << NAVE << " },\n"
                        "{ id = 4, classe = 'mercantile', categoria = 'nave', flag = 2, posizione = { x = 3000, y = 1000 }, quota_m = 0, rotta = 270, velocita_kn = 14, " << NAVE << " },\n"
                        "}, timer = {} } } }\n";
    Partita p(f.string());
    p.avanza(300.0);
    const int classificazioni = p.eventi(TipoEvento::classificazione_ia);
    const bool sale = std::any_of(p.m.cambi_allerta().begin(), p.m.cambi_allerta().end(),
                                  [](const Missione::CambioAllerta& c) { return c.causa == "possub" || c.causa == "probsub"; });
    MESSAGE("classificazioni fra navi in 5 minuti: ", classificazioni);
    CHECK_FALSE(sale);
    fs::remove(f);
}

// in pace un battello visto non e' una minaccia (la principale, 31/10/2026: l'Orion della S23 in A3 «albero» dopo 150 s):
// una Krivak in pace col 688 del giocatore in superficie a 3 km, di notte coi fanali accesi, per 5 minuti: la vedetta lo
// vede (l'evento «albero»), e l'allerta resta alla vigilanza
TEST_CASE("forze: in pace l'albero o il battello visto porta al piu' alla vigilanza" * doctest::skip(!fs::exists(M688_DATI "/interazioni.toml") || !fs::exists(M688_DATI "/luci.toml"))) {
    const auto f = fs::temp_directory_path() / "m688_prova_forze_albero_pace.lua";
    std::ofstream(f) << "return { id = 'FORZE_ALBERO', teatro = 'faroe', lato = 'usa', data = '1989-12-15', ora_utc = 0,"
                        " paesaggio_sonoro = false, carriera = { situazione = 'pace' }, script = {}, varianti = { { probabilita = 1, unita = {\n"
                        "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true, posizione = { x = 0, y = 0 },"
                        "  quota_m = 0, rotta = 90, velocita_kn = 0, " << SUB << " },\n"
                        "{ id = 2, classe = 'krivak2', categoria = 'nave', flag = 18, posizione = { x = 0, y = 3000 }, quota_m = 0, rotta = 90, velocita_kn = 5, " << NAVE << " },\n"
                        "}, timer = {} } } }\n";
    Partita p(f.string());
    p.m.luci_enhanced(M688_DATI);
    REQUIRE(p.m.applica(Ordine{p.m.tempo(), TipoOrdine::luci_giocatore, 1.0}));
    bool visto = false;
    int massima = 0;
    for (int k = 0; k < 60; ++k) {
        p.avanza(5.0);
        const auto& u = p.m.unita().at(2);
        if (const auto c = u.contatti.find(0); c != u.contatti.end() && c->second.sensore == "vista") visto = true;
        massima = std::max(massima, p.m.allerta_unita(2).value_or(0));
    }
    MESSAGE("la vedetta della Krivak ha visto il battello: ", visto, "; allerta massima in pace: ", massima);
    CHECK(visto);
    CHECK(massima <= 1);
    fs::remove(f);
}

// Il quadro radio ai sottomarini dell'IA (tools/re/prove/quadro_radio/proposta.md, passo_radio_sottomarini): a quota d'antenna, al
// ciclo, con la copia del messaggio; la salita alle finestre in crisi e in guerra; la caccia sul dato con le armi libere

namespace {

struct BattelloQuadro {
    int id;
    double x, y, quota, kn;
};

// una Perry (2) ferma al centro, i Victor richiesti, il giocatore lontano e profondo e della loro parte (la spia non lo riporta);
// `altre`: unita' in piu'
// il giocatore fuori scena oltre la griglia dei raggi (300 km): a 100 km sente e si fa sentire, e sposta il percorso della prova
constexpr double Y_OLTRE_GRIGLIA = -350000.0;

std::string scena_quadro(const std::string& nome, const std::string& situazione, const std::vector<BattelloQuadro>& battelli, const std::string& altre = "",
                         const std::string& script = "", const std::string& timer = "", bool editor = true, bool perry = true, double y_giocatore = -100000.0) {
    const auto f = fs::temp_directory_path() / ("m688_prova_quadro_" + nome + ".lua");
    std::ofstream o(f);
    o << "return { id = 'QUADRO_" << nome << "', teatro = 'faroe', lato = 'urss', data = '1989-06-15', ora_utc = 12, paesaggio_sonoro = false,"
      << (editor ? " editor = {}," : "") << " carriera = { situazione = '" << situazione << "' }, script = {" << script << "}, varianti = { { probabilita = 1, unita = {\n"
      << "{ id = 0, classe = 'victor3', categoria = 'sottomarino', flag = 17, giocatore = true, posizione = { x = 0, y = " << y_giocatore << " }, quota_m = 300, rotta = 90, velocita_kn = 3, " << SUB << " },\n";
    if (perry) o << "{ id = 2, classe = 'perry', categoria = 'nave', flag = 10, posizione = { x = 0, y = 0 }, quota_m = 0, rotta = 0, velocita_kn = 0, " << NAVE << " },\n";
    for (const auto& b : battelli)
        o << "{ id = " << b.id << ", classe = 'victor3', categoria = 'sottomarino', flag = 17, posizione = { x = " << b.x << ", y = " << b.y << " }, quota_m = " << b.quota
          << ", rotta = 0, velocita_kn = " << b.kn << ", " << SUB << " },\n";
    o << altre << "}, timer = {" << timer << "} } } }\n";
    return f.string();
}

struct ProvaQuadro {
    std::map<std::string, ClasseNave> classi = carica_classi(M688_DATI "/classi.toml");
    Missione m;
    DanniMissione dm;
    explicit ProvaQuadro(const std::string& file, std::uint64_t seme = 1) : m(file, seme, false), dm(classi, M688_DATI, 1, RegolaDanni::simulazione) {
        m.acustica_enhanced(M688_DATI);
        m.senza_giocatore(true);
    }
    void avanza(double s) {
        for (long i = 0; i < static_cast<long>(s * 9.0 + 0.5); ++i) {
            m.avanza(1.0 / 9.0);
            dm.avanza(m, 1.0 / 9.0);
        }
    }
    void livello(Esperienza e, std::optional<int> allerta = std::nullopt) {
        Missione::RegolazioniParte r;
        r.livello = e;
        r.allerta = allerta;
        m.regolazioni({{0x10, r}});
    }
    void spia(double durata_s = 0.0, double tempo = -1.0) {
        Intervento x;
        x.tipo = "spia";
        x.testi = {{"parte", "urss"}, {"altra", "usa"}};
        x.durata_s = durata_s;
        x.tempo = tempo;
        REQUIRE(m.intervieni(x).ok);
    }
    void cieco(int id, double durata_s = 0.0) {   // senza sonar: niente contatto proprio, il dato radio resta l'unico
        Intervento x;
        x.tipo = "sensore";
        x.unita = id;
        x.testi["sensore"] = "sonar";
        x.parametri["fattore"] = 0.0;
        x.durata_s = durata_s;
        REQUIRE(m.intervieni(x).ok);
    }
    std::vector<Missione::ReazioneIa> reazioni(const std::string& nome, int id = 3) const {
        std::vector<Missione::ReazioneIa> v;
        for (const auto& r : m.reazioni_ia())
            if (r.reazione == nome && r.unita == id) v.push_back(r);
        return v;
    }
    double arrivo(int ricevente, int bersaglio) const {
        const auto c = m.contatto_radio(ricevente, bersaglio);
        return c ? c->t_arrivo : -1.0;
    }
};

// il primo ciclo del broadcast del Patto, col seme 1: lo dice un battello a quota d'antenna
double primo_ciclo() {
    const auto f = scena_quadro("ciclo", "pace", {{5, 20000, 60000, 6, 3}});
    ProvaQuadro p(f);
    p.spia();
    p.avanza(905.0);
    fs::remove(f);
    return p.arrivo(5, 2);
}

}  // namespace

TEST_CASE("quadro ai sottomarini: a 100 m non riceve, a quota d'antenna riceve al ciclo; la nave subito" * doctest::skip(!fs::exists(M688_DATI "/classi.toml"))) {
    const auto f = scena_quadro("riceve", "pace", {{3, 0, 60000, 100, 3}, {5, 20000, 60000, 6, 3}},
                                "{ id = 4, classe = 'krivak2', categoria = 'nave', flag = 18, posizione = { x = 0, y = 90000 }, quota_m = 0, rotta = 90, velocita_kn = 3, " + NAVE + " },\n"
                                "{ id = 6, classe = 'slava', categoria = 'nave', flag = 18, posizione = { x = 20000, y = 90000 }, quota_m = 0, rotta = 90, velocita_kn = 3, " + NAVE + " },\n");
    ProvaQuadro p(f);
    p.spia();
    p.avanza(5.0);
    CHECK(p.m.contatto_radio(4, 2).has_value());      // le navi: la spia diretta, come prima
    CHECK(p.m.contatto_radio(6, 2).has_value());
    CHECK_FALSE(p.m.contatto_radio(5, 2).has_value()); // il battello: al ciclo
    p.avanza(900.0);
    CHECK_FALSE(p.m.contatto_radio(3, 2).has_value());
    const auto c = p.m.contatto_radio(5, 2);
    REQUIRE(c.has_value());
    CHECK(c->t_arrivo >= 600.0);
    CHECK(c->t_arrivo <= 902.0);
    CHECK(c->r.t_fix >= c->t_arrivo - 61.0);   // l'ultimo quadro prima del ciclo
    fs::remove(f);
}

TEST_CASE("quadro ai sottomarini: a 15 m senza albero non riceve; a interruttore spento o nelle originali resta la regola di prima" * doctest::skip(!fs::exists(M688_DATI "/classi.toml"))) {
    const auto f = scena_quadro("quindici", "pace", {{3, 0, 60000, 15, 3}});
    {
        ProvaQuadro p(f);
        p.spia();
        p.avanza(905.0);
        CHECK_FALSE(p.m.contatto_radio(3, 2).has_value());
    }
    {
        ProvaQuadro p(f);
        p.m.usa_quadro_sottomarini(false);
        p.spia();
        p.avanza(5.0);
        CHECK(p.m.contatto_radio(3, 2).has_value());   // la spia diretta
    }
    fs::remove(f);
    const auto g = scena_quadro("originale", "guerra", {{3, 0, 60000, 100, 3}}, "", "", "", false);
    ProvaQuadro p(g);
    p.spia();
    p.avanza(2500.0);
    CHECK(p.m.contatto_radio(3, 2).has_value());
    CHECK(p.reazioni("ascolto_broadcast").empty());   // nelle originali non sale
    CHECK(p.reazioni("dato_radio_usato").empty());
    fs::remove(g);
}

TEST_CASE("quadro ai sottomarini: salito 10 s prima del ciclo non copia il messaggio, al ciclo dopo si'; salito 100 s prima lo copia" * doctest::skip(!fs::exists(M688_DATI "/classi.toml"))) {
    const double t1 = primo_ciclo();
    REQUIRE(t1 > 100.0);
    const auto f = scena_quadro("copia", "pace", {{3, 0, 60000, 12, 3}, {5, 20000, 60000, 12, 3}});
    ProvaQuadro p(f);
    p.spia();
    const auto a_quota = [&](int id) { p.m.trova(id)->quota = p.m.trova(id)->quota_ordinata = 5.5; };
    p.avanza(t1 - 100.0);
    a_quota(5);
    p.avanza(90.0);
    a_quota(3);
    p.avanza(15.0);
    CHECK_FALSE(p.m.contatto_radio(3, 2).has_value());
    REQUIRE(p.m.contatto_radio(5, 2).has_value());
    CHECK(p.m.contatto_radio(5, 2)->t_arrivo <= t1 + 2.0);
    p.avanza(t1 + 5.0);
    const auto c = p.m.contatto_radio(3, 2);
    REQUIRE(c.has_value());
    CHECK(c->t_arrivo > 1.9 * t1);
    fs::remove(f);
}

namespace {

// otto navi e la spia, un ciclo solo (dura fra 600 e 900 s): i messaggi copiati a quella velocita', a quota d'antenna
int copiati_quadro(double kn, int semi) {
    std::string navi;
    for (int i = 0; i < 8; ++i)
        navi += "{ id = " + std::to_string(10 + i) + ", classe = 'perry', categoria = 'nave', flag = 10, posizione = { x = " + std::to_string(2000 * i) + ", y = 0 }, quota_m = 0, rotta = 0, velocita_kn = 0, " + NAVE + " },\n";
    const auto f = scena_quadro("veloce", "pace", {{3, 0, 80000, 6, kn}}, navi, "", "", true, false);
    int ricevuti = 0;
    for (int s = 1; s <= semi; ++s) {
        ProvaQuadro p(f, static_cast<std::uint64_t>(s));
        p.spia();
        p.avanza(905.0);
        ricevuti += static_cast<int>(p.m.quadro_radio(3).size());
    }
    fs::remove(f);
    return ricevuti;
}

}  // namespace

TEST_CASE("quadro ai sottomarini: oltre 10 nodi a quota d'antenna copia circa la meta' dei messaggi" * doctest::skip(!fs::exists(M688_DATI "/classi.toml"))) {
    CHECK(copiati_quadro(8.0, 1) == 8);
    const int veloci = copiati_quadro(15.0, 6);
    MESSAGE("copiati a 15 nodi: ", veloci, " su 48");
    CHECK(veloci >= 14);
    CHECK(veloci <= 34);
}

// la misura lunga, da lanciare a mano (--no-skip): 1600 messaggi in 200 semi, circa 20 minuti
TEST_CASE("quadro ai sottomarini: 200 semi oltre 10 nodi" * doctest::skip()) {
    const int veloci = copiati_quadro(15.0, 200);
    MESSAGE("copiati a 15 nodi: ", veloci, " su 1600");
    CHECK(veloci >= 720);
    CHECK(veloci <= 880);
}

TEST_CASE("quadro ai sottomarini: sotto la banchisa non riceve e non sale" * doctest::skip(!fs::exists(M688_DATI "/classi.toml"))) {
    const auto f = fs::temp_directory_path() / "m688_prova_quadro_banchisa.lua";
    std::ofstream(f) << "return { id = 'QUADRO_BANCHISA', teatro = 'barents', lato = 'usa', data = '1989-01-15', ora_utc = 12, paesaggio_sonoro = false, editor = {},"
                        " meteo = { fisso = { ghiaccio = 1 } }, carriera = { situazione = 'guerra' }, script = {}, varianti = { { probabilita = 1, unita = {\n"
                        "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true, posizione = { x = 50000, y = 50000 }, quota_m = 150, rotta = 0, velocita_kn = 3, " << SUB << " },\n"
                        "{ id = 2, classe = 'perry', categoria = 'nave', flag = 10, posizione = { x = 90000, y = 90000 }, quota_m = 0, rotta = 0, velocita_kn = 0, " << NAVE << " },\n"
                        "{ id = 3, classe = 'victor3', categoria = 'sottomarino', flag = 17, posizione = { x = 80000, y = 50000 }, quota_m = 150, rotta = 0, velocita_kn = 3, " << SUB << " },\n"
                        "}, timer = {} } } }\n";
    ProvaQuadro p(f.string());
    p.cieco(3);   // col sonar farebbe la TMA sul giocatore, e allora non salirebbe comunque
    p.spia();
    p.avanza(2500.0);
    REQUIRE(p.m.sotto_banchisa(3));
    CHECK(p.reazioni("ascolto_broadcast").empty());
    CHECK_FALSE(p.m.contatto_radio(3, 2).has_value());
    // lo stesso predicato per gli ordini radio: a 5 m ma sotto il ghiaccio le ROE nuove non arrivano
    REQUIRE(p.m.armi_libere_ia(3, 2) == std::optional<bool>(true));
    Intervento x;
    x.tipo = "roe";
    x.testi = {{"parte", "urss"}, {"categoria", "navi"}, {"regola", "trattenute"}};
    REQUIRE(p.m.intervieni(x).ok);
    Intervento o;
    o.tipo = "ordine_falso";
    o.testi = {{"parte", "urss"}, {"ordine", "rientro"}};
    o.parametri = {{"autenticazione", 1.0}};
    REQUIRE(p.m.intervieni(o).ok);
    for (int k = 0; k < 905 * 9; ++k) {   // e neanche il quadro, a un ciclo intero. Tenuto a 5 m a mano: sotto il ghiaccio l'IA scenderebbe
        p.m.trova(3)->quota = p.m.trova(3)->quota_ordinata = 5.0;
        p.m.trova(3)->flag54 |= 4;
        p.avanza(1.0 / 9.0);
    }
    CHECK(p.m.armi_libere_ia(3, 2) == std::optional<bool>(true));
    CHECK_FALSE(p.m.contatto_radio(3, 2).has_value());
    CHECK(std::none_of(p.m.reazioni_ia().begin(), p.m.reazioni_ia().end(), [](const Missione::ReazioneIa& r) { return r.evento == "ordine_falso" && r.unita == 3; }));
    fs::remove(f);
}

TEST_CASE("quadro ai sottomarini: il disturbo della parte vale come per il giocatore, al teatro; e per la spia" * doctest::skip(!fs::exists(M688_DATI "/classi.toml"))) {
    const auto krivak = "{ id = 4, classe = 'krivak2', categoria = 'nave', flag = 18, posizione = { x = 0, y = 4000 }, quota_m = 0, rotta = 90, velocita_kn = 3, " + NAVE + " },\n";
    const auto f = scena_quadro("disturbo", "pace", {{3, 0, 60000, 6, 3}}, krivak);
    struct Esito { double arrivo, viaggio; };   // il primo rapporto al battello, il suo viaggio fino al teatro
    const auto prova = [&](double perdita, double ritardo) {
        ProvaQuadro p(f);
        if (perdita > 0.0 || ritardo > 1.0) {
            Intervento x;
            x.tipo = "disturbo_radio";
            x.testi = {{"parte", "urss"}};
            x.parametri = {{"perdita", perdita}, {"ritardo", ritardo}};
            REQUIRE(p.m.intervieni(x).ok);
        }
        Esito e{-1.0, -1.0};
        for (int k = 0; k < 800 && e.arrivo < 0.0; ++k) {
            p.avanza(10.0);
            e.arrivo = p.arrivo(3, 2);
        }
        for (const auto& m : p.m.messaggi_ia())
            if (m.canale == Missione::MessaggioIa::Canale::teatro && m.da == 4 && m.su == 2 && !m.perso) {
                e.viaggio = m.t_arrivo - m.t_partenza;
                break;
            }
        return e;
    };
    const auto libero = prova(0.0, 1.0);
    MESSAGE("il rapporto al battello senza disturbo a ", libero.arrivo, " s, viaggio ", libero.viaggio, " s");
    REQUIRE(libero.arrivo > 0.0);
    REQUIRE(libero.viaggio > 0.0);
    const auto triplo = prova(0.0, 3.0);
    CHECK(triplo.viaggio == doctest::Approx(3.0 * libero.viaggio).epsilon(0.001));
    CHECK(triplo.arrivo > libero.arrivo);
    CHECK(prova(1.0, 1.0).arrivo < 0.0);
    // la spia verso i sottomarini: perdita piena, non arriva niente; la nave la riceve lo stesso
    {
        const auto g = scena_quadro("disturbo_spia", "pace", {{3, 0, 60000, 6, 3}}, "{ id = 4, classe = 'krivak2', categoria = 'nave', flag = 18, posizione = { x = 0, y = 90000 }, quota_m = 0, rotta = 90, velocita_kn = 3, " + NAVE + " },\n");
        ProvaQuadro p(g);
        Intervento x;
        x.tipo = "disturbo_radio";
        x.testi = {{"parte", "urss"}};
        x.parametri = {{"perdita", 1.0}};
        REQUIRE(p.m.intervieni(x).ok);
        p.spia();
        p.avanza(905.0);
        CHECK_FALSE(p.m.contatto_radio(3, 2).has_value());
        CHECK(p.m.contatto_radio(4, 2).has_value());
        fs::remove(g);
    }
    fs::remove(f);
}

TEST_CASE("quadro ai sottomarini: sale alle finestre in guerra e in crisi, mai in pace; in A3 di meno" * doctest::skip(!fs::exists(M688_DATI "/classi.toml"))) {
    const auto f = scena_quadro("guerra", "guerra", {{3, 0, 60000, 100, 3}});
    ProvaQuadro p(f);
    p.cieco(3);
    double salita = -1.0, in_alto = 1e9, albero = -1.0;
    for (int k = 0; k < 3000 && p.m.tempo() < 3000.0; ++k) {
        p.avanza(1.0);
        const auto& u = p.m.unita().at(3);
        if (salita < 0.0 && !p.reazioni("ascolto_broadcast").empty()) salita = p.reazioni("ascolto_broadcast").front().t;
        in_alto = std::min(in_alto, u.quota);
        if (albero < 0.0 && (u.flag54 & 4)) albero = p.m.tempo();
    }
    MESSAGE("salita a ", salita, " s, albero a ", albero, " s, quota minima ", in_alto);
    REQUIRE(salita > 0.0);
    CHECK(salita >= 1480.0);   // la prima finestra non prima di 1800 s, meno l'anticipo da 100 m
    CHECK(salita <= 2700.0);
    CHECK(in_alto <= 6.6);
    CHECK(albero > salita);
    p.avanza(1200.0);   // tornato giu', alla quota e alla velocita' di prima, con l'albero ammainato
    const auto& u = p.m.unita().at(3);
    CHECK(u.quota > 90.0);
    CHECK(u.quota_ordinata == doctest::Approx(100.0));
    CHECK(u.velocita_ordinata == doctest::Approx(3.0).epsilon(0.05));
    CHECK((u.flag54 & 4) == 0);
    fs::remove(f);

    const auto g = scena_quadro("pace_mai", "pace", {{3, 0, 60000, 100, 3}});
    ProvaQuadro q(g);
    q.cieco(3);
    q.avanza(2800.0);
    CHECK(q.reazioni("ascolto_broadcast").empty());
    fs::remove(g);

    const auto h = scena_quadro("a3", "guerra", {{3, 0, 60000, 100, 3}});
    ProvaQuadro a3(h);
    a3.livello(Esperienza::medio, 3);
    a3.cieco(3);
    a3.avanza(3000.0);
    CHECK(a3.reazioni("ascolto_broadcast").empty());   // la prima non prima di 3600 s
    a3.avanza(1500.0);
    CHECK(a3.reazioni("ascolto_broadcast").size() == 1);
    fs::remove(h);

    // in crisi sale, ma col dato non caccia: le armi non sono libere
    const auto c = scena_quadro("crisi", "crisi", {{3, 0, 60000, 100, 3}});
    ProvaQuadro cr(c);
    cr.cieco(3);
    cr.spia();
    for (int k = 0; k < 3000 && cr.reazioni("ascolto_broadcast").empty(); ++k) cr.avanza(1.0);
    REQUIRE_FALSE(cr.reazioni("ascolto_broadcast").empty());
    CHECK_FALSE(cr.m.contatto_radio(3, 2).has_value());   // la spia gira da un pezzo: finche' non sale, niente
    cr.avanza(3200.0 - cr.m.tempo());
    CHECK(cr.m.contatto_radio(3, 2).has_value());
    CHECK(cr.reazioni("dato_radio_usato").empty());
    fs::remove(c);
}

TEST_CASE("quadro ai sottomarini: una salita disturbata non si ritenta prima del ciclo dopo" * doctest::skip(!fs::exists(M688_DATI "/classi.toml"))) {
    const auto f = scena_quadro("disturbata", "guerra", {{3, 0, 60000, 100, 3}});
    ProvaQuadro p(f);
    p.cieco(3);
    // un altro comandante riprende il battello a ogni secondo: la salita si chiude al passo dopo
    for (int k = 0; k < 4000 && p.m.tempo() < 4000.0; ++k) {
        p.avanza(1.0);
        p.m.trova(3)->quota_ordinata = 100.0;
    }
    const auto salite = p.reazioni("ascolto_broadcast");
    REQUIRE_FALSE(salite.empty());
    const double ciclo = primo_ciclo();
    MESSAGE("salite disturbate: ", salite.size(), " in 4000 s, il primo ciclo a ", ciclo, " s");
    CHECK(salite.size() >= 2);   // al ciclo dopo ci riprova
    for (std::size_t i = 1; i < salite.size(); ++i) CHECK(salite[i].t - salite[i - 1].t > 0.5 * ciclo);
    CHECK(p.m.unita().at(3).quota > 90.0);
    fs::remove(f);
}

TEST_CASE("quadro ai sottomarini: col contatto ostile l'esperto rimanda, la recluta sale" * doctest::skip(!fs::exists(M688_DATI "/classi.toml"))) {
    const auto f = scena_quadro("rimanda", "crisi", {{3, 0, 2000, 100, 1}}, "{ id = 2, classe = 'perry', categoria = 'nave', flag = 10, posizione = { x = -12000, y = 0 }, quota_m = 0, rotta = 90, velocita_kn = 12, " + NAVE + " },\n", "", "", true, false, Y_OLTRE_GRIGLIA);
    const auto salite = [&](Esperienza e, double dopo_s = 0.0) {
        ProvaQuadro p(f);
        p.livello(e);
        p.avanza(1950.0);   // la prima finestra si apre a circa 1940 s
        const auto& c = p.m.unita().at(3).contatti;
        const auto k = c.find(2);
        REQUIRE(k != c.end());
        MESSAGE("contatto sulla Perry: qualita' ", k->second.qualita, ", posizione ", k->second.posizione_nota);
        CHECK(k->second.qualita >= 800.0);
        CHECK_FALSE(k->second.posizione_nota);
        p.avanza(dopo_s);
        return p.reazioni("ascolto_broadcast").size();
    };
    CHECK(salite(Esperienza::recluta) == 1);
    CHECK(salite(Esperienza::esperto) == 0);
    CHECK(salite(Esperienza::esperto, 2500.0) >= 1);   // rimanda, non rinuncia: passata la Perry sale
    fs::remove(f);
}

TEST_CASE("quadro ai sottomarini: il dato fresco da' la rotta, alla velocita' del livello, e nessun lancio" * doctest::skip(!fs::exists(M688_DATI "/classi.toml"))) {
    const auto f = scena_quadro("caccia", "guerra", {{3, 0, 30000, 6, 3}});
    const auto prova = [&](Esperienza e, double kn) {
        ProvaQuadro p(f);
        p.livello(e);
        p.cieco(3);
        p.spia();
        double t = -1.0;
        for (int k = 0; k < 1000 && t < 0.0; ++k) {
            p.avanza(1.0);
            if (!p.reazioni("dato_radio_usato").empty()) t = p.m.tempo();
        }
        REQUIRE(t > 0.0);
        CHECK(p.reazioni("dato_radio_usato").front().causa == 2);
        const double prima = distanza(p.m.unita().at(3).pos, p.m.unita().at(2).pos);
        p.avanza(1500.0);   // prima l'accostata, dalla rotta opposta: a poca velocita' circa 500 s
        const auto& u = p.m.unita().at(3);
        CHECK(u.velocita_ordinata == doctest::Approx(kn));
        CHECK(std::fabs(u.rotta - 180.0) < 10.0);
        CHECK(distanza(u.pos, p.m.unita().at(2).pos) < prima - 0.4 * kn * 0.514 * 1500.0);
        CHECK(p.m.eventi_missione().end() == std::find_if(p.m.eventi_missione().begin(), p.m.eventi_missione().end(), [](const EventoMissione& x) { return x.tipo == TipoEvento::lancio_ia && x.unita == 3; }));
    };
    prova(Esperienza::recluta, 15.0);
    prova(Esperienza::medio, 10.0);
    prova(Esperienza::esperto, 8.0);
    fs::remove(f);
}

TEST_CASE("quadro ai sottomarini: sul punto senza contatto lascia il dato e non lo ritenta; il dato scaduto si lascia" * doctest::skip(!fs::exists(M688_DATI "/classi.toml"))) {
    {   // la spia per due minuti, il punto a pochi km: un sottomarino che se ne va, e che sul punto nessuno sente. Lo riapre solo un rapporto piu' nuovo
        const auto f = scena_quadro("a_vuoto", "guerra", {{3, 0, 2000, 6, 3}}, "{ id = 2, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, posizione = { x = 0, y = 0 }, quota_m = 200, rotta = 180, velocita_kn = 18, " + SUB + " },\n", "", "", true, false);
        ProvaQuadro p(f);
        p.cieco(3);
        p.spia(120.0);
        p.avanza(3000.0);
        const auto l = p.reazioni("dato_radio_lasciato");
        REQUIRE_FALSE(l.empty());
        CHECK(l.front().evento == "a_vuoto");
        CHECK(p.reazioni("dato_radio_usato").size() == 1);
        CHECK(p.m.unita().at(3).velocita_ordinata == doctest::Approx(3.0).epsilon(0.05));
        p.spia(120.0, p.m.tempo() + 10.0);
        p.avanza(1100.0);
        CHECK(p.reazioni("dato_radio_usato").size() == 2);
        fs::remove(f);
    }
    {   // lontano: l'ellisse passa i 30 km prima che arrivi (a 30 nodi per una nave, 1824 s dal fix)
        const auto f = scena_quadro("scaduto", "guerra", {{3, 0, 45000, 6, 3}});
        ProvaQuadro p(f);
        p.cieco(3);
        p.spia(120.0);
        p.avanza(1800.0);
        REQUIRE(p.reazioni("dato_radio_usato").size() == 1);
        CHECK(p.reazioni("dato_radio_lasciato").empty());
        p.avanza(400.0);
        const auto l = p.reazioni("dato_radio_lasciato");
        REQUIRE(l.size() == 1);
        CHECK(l.front().evento == "scaduto");
        CHECK(l.front().t <= 120.0 + 1824.0 + 2.0);
        const auto& u = p.m.unita().at(3);
        CHECK(u.velocita_ordinata == doctest::Approx(3.0).epsilon(0.05));
        CHECK(u.rotta_ordinata == doctest::Approx(0.0).epsilon(0.01));
        fs::remove(f);
    }
}

TEST_CASE("quadro ai sottomarini: in A3 il dato vecchio non basta, in A2 si'" * doctest::skip(!fs::exists(M688_DATI "/classi.toml"))) {
    const double t1 = primo_ciclo();
    REQUIRE(t1 >= 600.0);
    const auto f = scena_quadro("vecchio", "guerra", {{3, 0, 45000, 6, 3}});
    const auto usati = [&](int allerta) {
        ProvaQuadro p(f);
        p.livello(Esperienza::medio, allerta);
        p.cieco(3);
        p.spia(1.0, t1 - 600.0);   // un quadro solo, di 600 s prima del ciclo: un'ellisse sopra i 10 km
        p.avanza(t1 + 10.0);
        REQUIRE(p.m.contatto_radio(3, 2).has_value());
        CHECK_FALSE(p.m.contatto_radio(3, 2)->posizione_valida);
        return p.reazioni("dato_radio_usato").size();
    };
    CHECK(usati(2) == 1);
    CHECK(usati(3) == 0);
    fs::remove(f);
}

TEST_CASE("quadro ai sottomarini: il contatto proprio e l'ordine dello script chiudono la caccia" * doctest::skip(!fs::exists(M688_DATI "/classi.toml"))) {
    const double t1 = primo_ciclo();
    REQUIRE(t1 > 0.0);
    {   // la spia ha dato un punto vecchio; la Perry viene incontro e il Victor la sente prima di arrivarci
        const auto f = scena_quadro("contatto", "guerra", {{3, 0, 15000, 6, 3}}, "{ id = 2, classe = 'perry', categoria = 'nave', flag = 10, posizione = { x = 0, y = 0 }, quota_m = 0, rotta = 0, velocita_kn = 12, " + NAVE + " },\n", "", "", true, false, Y_OLTRE_GRIGLIA);
        ProvaQuadro p(f);
        p.cieco(3, t1 + 30.0);
        p.spia(120.0);
        p.avanza(t1 + 1500.0);
        REQUIRE_FALSE(p.reazioni("dato_radio_usato").empty());
        const auto l = p.reazioni("dato_radio_lasciato");
        REQUIRE_FALSE(l.empty());
        CHECK(l.front().evento == "contatto");
        CHECK(p.reazioni("dato_radio_usato").size() == 1);
        fs::remove(f);
    }
    {
        const auto gestore = "V = function(u) m688.telegrafo(u, 1) m688.salva_modello(u) end";
        const auto timer = "{ dopo_s = " + std::to_string(static_cast<int>(t1) + 60) + ", unita = 3, gestore = 'V' }";
        const auto f = scena_quadro("script", "guerra", {{3, 0, 30000, 6, 3}}, "", gestore, timer);
        ProvaQuadro p(f);
        p.cieco(3);
        p.spia();
        p.avanza(t1 + 70.0);
        const auto l = p.reazioni("dato_radio_lasciato");
        REQUIRE_FALSE(l.empty());
        CHECK(l.front().evento == "script");
        const double kn = p.m.unita().at(3).velocita_ordinata;
        CHECK(std::fabs(kn - 10.0) > 0.5);   // la velocita' dello script resta
        CHECK(kn != doctest::Approx(3.0));
        fs::remove(f);
    }
}

TEST_CASE("quadro ai sottomarini: un rapporto su un aereo non fa cacciare" * doctest::skip(!fs::exists(M688_DATI "/classi.toml"))) {
    const auto aereo = "{ id = 7, classe = 'p3c', categoria = 'aereo', flag = 11, posizione = { x = 0, y = 0 }, quota_m = 0, rotta = 0, velocita_kn = 200, " + NAVE + " },\n";
    const auto f = scena_quadro("aereo", "guerra", {{3, 0, 30000, 6, 3}}, aereo, "", "", true, false);
    ProvaQuadro p(f);
    p.cieco(3);
    p.spia();
    p.avanza(1000.0);
    const auto c = p.m.contatto_radio(3, 7);
    REQUIRE(c.has_value());
    CHECK(c->r.categoria == "aereo");
    CHECK(p.reazioni("dato_radio_usato").empty());
    fs::remove(f);
}

TEST_CASE("quadro ai sottomarini: nessun siluro col solo dato radio; col contatto proprio lo stesso gestore lancia" * doctest::skip(!fs::exists(M688_DATI "/classi.toml"))) {
    // il gestore di bersaglio lancia appena ha la traccia e la posizione, come R_sottomarino; la Perry scappa a 14 nodi, entro la portata del siluro, e il dato
    // della spia resta fresco
    const auto lanci = [](ProvaQuadro& p, double s) {   // guardando ogni 10 s: l'arma a fine corsa esce dalle unita'
        std::set<int> armi;
        for (int k = 0; k < static_cast<int>(s / 10.0); ++k) {
            p.avanza(10.0);
            for (const auto& [id, u] : p.m.unita())
                if (u.lanciatore == 3 && (u.flag & 0x20)) armi.insert(id);
        }
        return armi.size();
    };
    const std::string gestore = "T = function(u) if not (m688.traccia_solida(u) and m688.soluzione(u)) then m688.comportamento(u, 'T') m688.salva_modello(u) return end"
                                " m688.bersaglio(u, 2) m688.memorizza_bersaglio(u) m688.modello_azzera(u) m688.modello_classe(u, 'type53')"
                                " m688.modello_flag(u, 49) m688.lancia(u, '', '') m688.modello_avanti_tutta(u, 45.7) m688.modello_integrita(u, 20)"
                                " m688.modello_quota_max(u, 548) m688.telegrafo(u, 1) m688.navigazione(u, 1) m688.collega(u, 1) m688.nuova_unita(u) end";
    const auto scena_t = [&](const std::string& nome, double y, double kn) {
        return scena_quadro(nome, "guerra", {}, "{ id = 2, classe = 'perry', categoria = 'nave', flag = 10, posizione = { x = 0, y = 0 }, quota_m = 0, rotta = 180, velocita_kn = " + std::to_string(kn) + ", " + NAVE + " },\n"
                            "{ id = 3, classe = 'victor3', categoria = 'sottomarino', flag = 17, posizione = { x = 0, y = " + std::to_string(y) + " }, quota_m = 6, rotta = 180, velocita_kn = 3, gestori = { bersaglio = 'T' }, " + SUB + " },\n", gestore, "", true, false);
    };
    {
        const auto f = scena_t("solo_radio", 5000.0, 14.0);
        ProvaQuadro p(f);
        p.cieco(3);
        p.spia();
        const auto n = lanci(p, 1800.0);
        REQUIRE_FALSE(p.reazioni("dato_radio_usato").empty());
        REQUIRE(p.m.contatto_radio(3, 2).has_value());
        const auto& c = p.m.unita().at(3).contatti;
        CHECK((c.count(2) == 0 || !c.at(2).posizione_nota));   // al piu' l'ESM, quando a quota d'antenna la Perry irradia
        CHECK(n == 0);
        fs::remove(f);
    }
    {   // il controllo: il gestore e' vivo, col contatto proprio lancia
        const auto f = scena_t("col_contatto", 3000.0, 12.0);
        ProvaQuadro p(f);
        CHECK(lanci(p, 1800.0) >= 1);
        fs::remove(f);
    }
}

TEST_CASE("quadro ai sottomarini: nelle originali e con l'interruttore spento non cambia nulla, stesso registro" * doctest::skip(!fs::exists(M688_DATI "/classi.toml"))) {
    const auto impronta = [](const std::string& file, bool acceso, bool sale) {
        ProvaQuadro p(file);
        p.m.usa_quadro_sottomarini(acceso);
        p.spia();
        p.avanza(2600.0);
        std::string s;
        for (const auto& e : p.m.eventi_missione()) s += std::to_string(static_cast<int>(e.tipo)) + "," + std::to_string(e.unita) + ";";
        for (const auto& r : p.m.reazioni_ia()) s += r.reazione + "," + std::to_string(r.t) + ";";
        for (const auto& [id, u] : p.m.unita()) s += std::to_string(u.pos.x) + "," + std::to_string(u.pos.y) + "," + std::to_string(u.quota) + ";";
        CHECK(p.reazioni("ascolto_broadcast").empty() != sale);
        return s;
    };
    const auto o = scena_quadro("uguali_o", "guerra", {{3, 0, 60000, 100, 3}}, "", "", "", false);
    CHECK(impronta(o, true, false) == impronta(o, false, false));
    fs::remove(o);
    // fuori dalle originali: spento non sale e la partita e' un'altra da quella accesa
    const auto e = scena_quadro("uguali_e", "guerra", {{3, 0, 60000, 100, 3}});
    CHECK(impronta(e, false, false) != impronta(e, true, true));
    fs::remove(e);
}

namespace {

const std::string SORDA = "modello = { massima_kn = 20, virata_gradi_s = 2, accel_kn_s = 0.5, quota_m_s = 1, quota_max_m = 0, integrita = 100, rumore = 10, sonar = 0 }";

// il battello 3 alla quota data sente la Perry 2 a 5 km; le navi americane non hanno sonar, quindi il rapporto e' solo suo e sul battello arriva solo il fix
std::string scena_riferisce(const std::string& nome, double quota, const std::string& situazione = "pace", bool editor = true) {
    return scena_quadro(nome, situazione, {},
                        "{ id = 2, classe = 'perry', categoria = 'nave', flag = 10, posizione = { x = 0, y = 0 }, quota_m = 0, rotta = 180, velocita_kn = 14, " + SORDA + " },\n"
                        "{ id = 3, classe = 'victor3', categoria = 'sottomarino', flag = 17, posizione = { x = 0, y = 5000 }, quota_m = " + std::to_string(quota) + ", rotta = 180, velocita_kn = 3, " + SUB + " },\n"
                        "{ id = 4, classe = 'sacramento', categoria = 'nave', flag = 10, posizione = { x = 90000, y = 0 }, quota_m = 0, rotta = 90, velocita_kn = 5, " + SORDA + " },\n",
                        "", "", editor, false);
}

// i rapporti che il battello 3 manda al teatro
std::vector<Missione::MessaggioIa> rapporti_al_teatro(const Missione& m) {
    std::vector<Missione::MessaggioIa> v;
    for (const auto& x : m.messaggi_ia())
        if (x.canale == Missione::MessaggioIa::Canale::teatro && x.da == 3) v.push_back(x);
    return v;
}

bool fix_su(const Missione& m, int id) {
    for (const auto& r : m.reazioni_ia())
        if (r.unita == -1 && r.causa == id && r.reazione == "localizza") return true;
    return false;
}

}  // namespace

TEST_CASE("rapporti dei sottomarini: riferisce a quota d'antenna, non a 15 m senza albero; a interruttore spento resta la regola di prima" * doctest::skip(!fs::exists(M688_DATI "/classi.toml"))) {
    const auto f = scena_riferisce("rif_quindici", 15.0);
    {
        ProvaQuadro p(f);
        p.avanza(400.0);
        CHECK(rapporti_al_teatro(p.m).empty());
    }
    {
        ProvaQuadro p(f);
        p.m.usa_quadro_sottomarini(false);
        p.avanza(400.0);
        CHECK_FALSE(rapporti_al_teatro(p.m).empty());
    }
    fs::remove(f);
    const auto g = scena_riferisce("rif_sei", 6.0);
    ProvaQuadro p(g);
    p.avanza(400.0);
    CHECK_FALSE(rapporti_al_teatro(p.m).empty());
    fs::remove(g);
}

TEST_CASE("rapporti dei sottomarini: la trasmissione non sposta il caso delle forze" * doctest::skip(!fs::exists(M688_DATI "/classi.toml"))) {
    // il primo rapporto dal battello a quota d'antenna: col quadro acceso passa dalla radiogoniometria, spento no; il ritardo viene dal caso delle forze
    const auto f = scena_riferisce("rif_caso", 6.0);
    const auto viaggio = [&](bool acceso, std::uint64_t seme) {
        ProvaQuadro p(f, seme);
        p.m.usa_quadro_sottomarini(acceso);
        p.avanza(400.0);
        const auto v = rapporti_al_teatro(p.m);
        REQUIRE_FALSE(v.empty());
        return std::pair{v.front().t_partenza, v.front().t_arrivo - v.front().t_partenza};
    };
    for (const std::uint64_t seme : {1u, 2u, 3u}) {
        const auto a = viaggio(true, seme), b = viaggio(false, seme);
        CHECK(a.first == doctest::Approx(b.first));
        CHECK(a.second == doctest::Approx(b.second));
    }
    fs::remove(f);
}

TEST_CASE("rapporti dei sottomarini: il fix arriva al teatro nemico col suo errore e il suo ritardo; non basta alla caccia" * doctest::skip(!fs::exists(M688_DATI "/classi.toml"))) {
    const auto f = scena_riferisce("rif_fix", 6.0);
    bool visto = false;
    for (std::uint64_t seme = 1; seme <= 40 && !visto; ++seme) {
        ProvaQuadro p(f, seme);
        p.avanza(300.0);
        if (!fix_su(p.m, 3)) continue;
        visto = true;
        p.avanza(11000.0);
        const auto c = p.m.contatto_radio(4, 3);
        REQUIRE(c.has_value());
        CHECK(c->r.da == -1);
        CHECK(c->r.categoria == "sconosciuto");
        CHECK(c->r.errore_m == doctest::Approx(65000.0));
        CHECK(c->t_arrivo - c->r.t_fix >= 3599.0);
        CHECK(c->t_arrivo - c->r.t_fix <= 10801.0);
        CHECK(c->raggio_m == doctest::Approx(65000.0 + 20.0 * 0.5144 * (p.m.tempo() - c->r.t_fix)).epsilon(0.01));   // sconosciuto: 20 nodi
        CHECK_FALSE(c->posizione_valida);
    }
    CHECK(visto);
    fs::remove(f);
}

TEST_CASE("rapporti dei sottomarini: il giocatore della parte riceve il fix con l'errore del fix" * doctest::skip(!fs::exists(M688_DATI "/classi.toml"))) {
    const auto f = fs::temp_directory_path() / "m688_prova_rif_giocatore.lua";
    std::ofstream(f) << "return { id = 'RIF_GIOCATORE', teatro = 'faroe', lato = 'usa', data = '1989-06-15', ora_utc = 12, editor = {},"
                        " paesaggio_sonoro = false, carriera = { situazione = 'pace' }, script = {}, varianti = { { probabilita = 1, unita = {\n"
                        "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true, posizione = { x = 0, y = -100000 }, quota_m = 300, rotta = 90, velocita_kn = 3, " << SUB << " },\n"
                        "{ id = 2, classe = 'perry', categoria = 'nave', flag = 10, posizione = { x = 0, y = 0 }, quota_m = 0, rotta = 180, velocita_kn = 14, " << SORDA << " },\n"
                        "{ id = 3, classe = 'victor3', categoria = 'sottomarino', flag = 17, posizione = { x = 0, y = 5000 }, quota_m = 6, rotta = 180, velocita_kn = 3, " << SUB << " },\n"
                        "}, timer = {} } } }\n";
    bool visto = false;
    for (std::uint64_t seme = 1; seme <= 40 && !visto; ++seme) {
        ProvaQuadro p(f.string(), seme);
        p.avanza(300.0);
        if (!fix_su(p.m, 3)) continue;
        visto = true;
        p.avanza(11000.0);
        REQUIRE(p.m.radio() != nullptr);
        bool arrivato = false;
        for (const auto& v : p.m.radio()->debriefing().trasmessi)
            if (v.messaggio.oggetto == "CONTACT REPORT" && v.messaggio.richiesta) {
                arrivato = true;
                CHECK(v.messaggio.richiesta->errore_m == doctest::Approx(65000.0));
            }
        CHECK(arrivato);
    }
    CHECK(visto);
    fs::remove(f);
}

TEST_CASE("rapporti dei sottomarini: nelle originali un battello a 15 m riferisce come prima, acceso o spento" * doctest::skip(!fs::exists(M688_DATI "/classi.toml"))) {
    const auto f = scena_riferisce("rif_originale", 15.0, "pace", false);
    const auto impronta = [&](bool acceso) {
        ProvaQuadro p(f);
        p.m.usa_quadro_sottomarini(acceso);
        p.avanza(600.0);
        std::string s;
        for (const auto& x : p.m.messaggi_ia()) s += std::to_string(x.t_arrivo) + "," + std::to_string(x.da) + ";";
        for (const auto& r : p.m.reazioni_ia()) s += r.reazione + "," + std::to_string(r.t) + ";";
        CHECK_FALSE(rapporti_al_teatro(p.m).empty());
        return s;
    };
    CHECK(impronta(true) == impronta(false));
    // nelle originali il rapporto non e' una trasmissione: su 12 semi, con il 34% a rapporto, nessun fix
    for (std::uint64_t seme = 1; seme <= 12; ++seme) {
        ProvaQuadro p(f, seme);
        p.avanza(600.0);
        CHECK_FALSE(rapporti_al_teatro(p.m).empty());
        CHECK_FALSE(fix_su(p.m, 3));
    }
    fs::remove(f);
}

TEST_CASE("rapporti dei sottomarini: 200 semi, le probabilita' della trasmissione del giocatore" * doctest::skip()) {
    // URSS, raffica di 0,72 s: fix al 34%. USA, UHF di 1,5 s: 2% di fix, 24% col nemico con l'ESM entro 50 km (regola_df, nucleo/radio.hpp)
    const auto urss = scena_riferisce("rif_200_urss", 6.0);
    int fix = 0;
    for (std::uint64_t s = 1; s <= 200; ++s) {
        ProvaQuadro p(urss, s);
        p.avanza(400.0);
        REQUIRE_FALSE(rapporti_al_teatro(p.m).empty());
        fix += fix_su(p.m, 3);
    }
    MESSAGE("URSS: ", fix, " fix su 200");
    CHECK(fix >= 50);
    CHECK(fix <= 86);
    fs::remove(urss);
    // gli USA: il battello 3 sente il Victor 2, che in guerra e' ostile; con la nave 4 entro 50 km c'e' l'ESM, a 90 km no
    const auto usa = [](const std::string& nome, double x_nave) {
        return scena_quadro(nome, "guerra", {},
                            "{ id = 2, classe = 'victor3', categoria = 'sottomarino', flag = 17, posizione = { x = 0, y = 0 }, quota_m = 200, rotta = 180, velocita_kn = 18, " + SUB + " },\n"
                            "{ id = 3, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, posizione = { x = 0, y = 5000 }, quota_m = 6, rotta = 180, velocita_kn = 3, " + SUB + " },\n"
                            "{ id = 4, classe = 'krivak2', categoria = 'nave', flag = 18, posizione = { x = " + std::to_string(x_nave) + ", y = 0 }, quota_m = 0, rotta = 90, velocita_kn = 3, " + SORDA + " },\n",
                            "", "", true, false);
    };
    const auto conta = [](const std::string& f, const std::string& motivo) {
        int n = 0, riferito = 0;   // il battello riferisce se sente il Victor in tempo: non in tutti i semi
        for (std::uint64_t s = 1; s <= 200; ++s) {
            ProvaQuadro p(f, s);
            p.avanza(400.0);
            if (rapporti_al_teatro(p.m).empty()) continue;
            ++riferito;
            for (const auto& r : p.m.reazioni_ia())
                if (r.causa == 3 && r.reazione == "localizza") { ++n; break; }
        }
        MESSAGE(motivo, ": ", n, " su ", riferito);
        CHECK(riferito >= 150);
        return n;
    };
    const auto lontana = usa("rif_200_usa", 90000.0), vicina = usa("rif_200_esm", 30000.0);
    CHECK(conta(lontana, "USA senza ESM vicino") <= 12);
    const int esm = conta(vicina, "USA con l'ESM vicino");
    CHECK(esm >= 30);
    CHECK(esm <= 66);
    fs::remove(lontana);
    fs::remove(vicina);
}

namespace {

// il battello 3 riferisce sulla Perry 2; un P-3C americano (1) pattuglia a 150 km: lontano, un contatto suo non lo sostituisce
std::string scena_fix_aereo() {
    const auto f = fs::temp_directory_path() / "m688_prova_rif_aereo.lua";
    const std::string VOLO = "modello = { massima_kn = 330, virata_gradi_s = 3, accel_kn_s = 2, quota_m_s = 1, quota_max_m = 500, integrita = 10, rumore = 0, sonar = 20 }";
    std::ofstream(f) << "return { id = 'RIF_AEREO', teatro = 'faroe', lato = 'urss', data = '1989-06-15', ora_utc = 12, paesaggio_sonoro = false, editor = {}, carriera = { situazione = 'pace' },\n"
                        "aereo_asw = { { unita = 1, situazione = 'pace', zona = { x = 150000, y = 0, raggio_m = 20000 } } }, script = {}, varianti = { { probabilita = 1, unita = {\n"
                        "{ id = 0, classe = 'victor3', categoria = 'sottomarino', flag = 17, giocatore = true, posizione = { x = 0, y = -100000 }, quota_m = 300, rotta = 90, velocita_kn = 3, " << SUB << " },\n"
                        "{ id = 1, classe = 'p3c', categoria = 'aereo', flag = 11, posizione = { x = 150000, y = 0 }, quota_m = 0, rotta = 0, velocita_kn = 100, " << VOLO << " },\n"
                        "{ id = 2, classe = 'perry', categoria = 'nave', flag = 10, posizione = { x = 0, y = 0 }, quota_m = 0, rotta = 180, velocita_kn = 14, " << SORDA << " },\n"
                        "{ id = 3, classe = 'victor3', categoria = 'sottomarino', flag = 17, posizione = { x = 0, y = 5000 }, quota_m = 6, rotta = 180, velocita_kn = 3, " << SUB << " },\n"
                        "}, timer = {} } } }\n";
    return f.string();
}

}  // namespace

TEST_CASE("rapporti dei sottomarini: il fix arriva all'aereo ASW della parte come datum, con l'errore del fix, e l'aereo ci va" * doctest::skip(!fs::exists(M688_DATI "/asw_aereo.toml"))) {
    const auto f = scena_fix_aereo();
    bool visto = false;
    for (std::uint64_t seme = 1; seme <= 40 && !visto; ++seme) {
        ProvaQuadro p(f, seme);
        p.avanza(300.0);
        if (!fix_su(p.m, 3)) continue;
        visto = true;
        REQUIRE(p.m.controllato_asw(1));
        CHECK(p.m.contatto_radio(1, 3) == std::nullopt);   // prima del ritardo non sa niente
        double t_fix = 0.0;
        for (const auto& r : p.m.reazioni_ia())
            if (r.unita == -1 && r.causa == 3 && r.reazione == "localizza") { t_fix = r.t; break; }
        std::optional<Missione::ContattoRadio> c;
        for (int s = 0; s < 11000 && !c; s += 10) {
            p.avanza(10.0);
            c = p.m.contatto_radio(1, 3);
        }
        REQUIRE(c.has_value());
        CHECK(c->r.da == -1);
        CHECK(c->r.t_fix == doctest::Approx(t_fix));
        CHECK(c->t_arrivo - t_fix >= 3599.0);
        CHECK(c->t_arrivo - t_fix <= 10801.0);
        REQUIRE(c->r.pos.has_value());
        p.avanza(30.0);   // il controllore prende il cue al suo prossimo pensiero
        const auto* a = p.m.controllore_asw(1);
        REQUIRE(a != nullptr);
        REQUIRE(a->traccia().valida);
        CHECK(a->traccia().errore_m == doctest::Approx(65000.0));
        CHECK(a->traccia().t == doctest::Approx(t_fix));
        CHECK(a->traccia().pos.x == doctest::Approx(c->r.pos->x));
        CHECK(a->traccia().pos.y == doctest::Approx(c->r.pos->y));
        CHECK(a->traccia().sorgente == 3);
        CHECK(a->stato() == StatoAsw::datum);
        const Vec2 datum = a->traccia().pos;
        const auto* aereo = p.m.trova(1);
        REQUIRE(aereo != nullptr);
        const double prima = distanza(aereo->pos, datum);
        p.avanza(300.0);
        const double dopo = distanza(aereo->pos, datum);
        MESSAGE("seme ", seme, ": fix a t = ", t_fix, ", arriva dopo ", c->t_arrivo - t_fix, " s, datum a ", prima, " m, dopo 300 s a ", dopo, " m");
        CHECK(dopo < prima - 5000.0);
        std::string storia;
        p.avanza(1800.0);
        for (const auto& x : p.m.controllore_asw(1)->storia()) storia += std::to_string(static_cast<int>(x.t)) + " " + nome_stato(x.a) + "; ";
        MESSAGE("il controllore: ", storia);
    }
    CHECK(visto);
    fs::remove(f);
}

TEST_CASE("rapporti dei sottomarini: piu' rapporti nello stesso minuto valgono una trasmissione sola" * doctest::skip(!fs::exists(M688_DATI "/classi.toml"))) {
    // due navi vicine, due contatti insieme: due rapporti al teatro. Col 34% a trasmissione, senza il limite su 40 semi due fix ravvicinati si vedrebbero
    const auto f = scena_quadro("rif_minuto", "pace", {},
                                "{ id = 2, classe = 'perry', categoria = 'nave', flag = 10, posizione = { x = -1000, y = 0 }, quota_m = 0, rotta = 180, velocita_kn = 14, " + SORDA + " },\n"
                                "{ id = 4, classe = 'perry', categoria = 'nave', flag = 10, posizione = { x = 1000, y = 0 }, quota_m = 0, rotta = 180, velocita_kn = 14, " + SORDA + " },\n"
                                "{ id = 3, classe = 'victor3', categoria = 'sottomarino', flag = 17, posizione = { x = 0, y = 5000 }, quota_m = 6, rotta = 180, velocita_kn = 3, " + SUB + " },\n",
                                "", "", true, false);
    int doppi = 0, fix = 0;
    for (std::uint64_t seme = 1; seme <= 40; ++seme) {
        ProvaQuadro p(f, seme);
        p.avanza(300.0);
        const auto v = rapporti_al_teatro(p.m);
        for (std::size_t i = 1; i < v.size(); ++i) doppi += v[i].t_partenza - v[i - 1].t_partenza < 60.0;
        std::vector<double> t;
        for (const auto& r : p.m.reazioni_ia())
            if (r.unita == -1 && r.causa == 3 && r.reazione == "localizza") t.push_back(r.t);
        fix += static_cast<int>(t.size());
        for (std::size_t i = 1; i < t.size(); ++i) CHECK(t[i] - t[i - 1] >= 60.0);
    }
    MESSAGE(doppi, " coppie di rapporti nello stesso minuto, ", fix, " fix su 40 semi");
    CHECK(doppi >= 10);   // la scena fa davvero due rapporti insieme
    CHECK(fix >= 3);      // e i fix ci sono
    fs::remove(f);
}
