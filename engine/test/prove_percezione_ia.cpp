#include <doctest.h>

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <map>
#include <string>

#include "missioni/danni_missione.hpp"
#include "missioni/missione.hpp"
#include "missioni/registro_missione.hpp"
#include "nucleo/classi.hpp"
#include "nucleo/geometria.hpp"

using namespace m688;
namespace fs = std::filesystem;

// Fase 2 delle interazioni fra unita' dell'IA (docs/18_interazioni_ia.md): la percezione uguale per tutti. Radar contro
// gli alberi, ESM e vista delle teste anche fra unita' dell'IA; il passivo dell'IA col solo rilevamento e la sua TMA.

namespace {

const std::string SUB = "modello = { massima_kn = 32, virata_gradi_s = 1, accel_kn_s = 0.5, quota_m_s = 1, quota_max_m = 450, integrita = 200, rumore = 8, sonar = 19 }";
const std::string NAVE = "modello = { massima_kn = 32, virata_gradi_s = 2, accel_kn_s = 0.5, quota_m_s = 1, quota_max_m = 0, integrita = 100, rumore = 10, sonar = 20 }";

// il giocatore lontano e profondo; `altre` le unita' della scena; `script` e `timer` per alzare alberi e periscopi
std::string scena(const std::string& nome, const std::string& altre, const std::string& script = "", const std::string& timer = "") {
    const auto f = fs::temp_directory_path() / ("m688_prova_percezione_" + nome + ".lua");
    // febbraio: l'acqua d'inverno delle Faroe, per cui le prove sono tarate (l'acustica segue il mese della data)
    std::ofstream(f) << "return { id = 'PERC_" << nome << "', teatro = 'faroe', lato = 'usa', data = '1989-02-15', ora_utc = 12,"
                        " paesaggio_sonoro = false, script = {" << script << "}, varianti = { { probabilita = 1, unita = {\n"
                        "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true, posizione = { x = 0, y = -60000 },"
                        "  quota_m = 300, rotta = 90, velocita_kn = 3, " << SUB << " },\n" << altre << "\n}, timer = {" << timer << "} } } }\n";
    return f.string();
}

struct Partita {
    std::map<std::string, ClasseNave> classi = carica_classi(M688_DATI "/classi.toml");
    Missione m;
    DanniMissione dm;
    explicit Partita(const std::string& file) : m(file, 1, false), dm(classi, M688_DATI, 1, RegolaDanni::simulazione) {}
    void avanza(double s) { for (long i = 0; i < static_cast<long>(s * 9.0); ++i) { m.avanza(1.0 / 9.0); dm.avanza(m, 1.0 / 9.0); } }
    const Contatto* contatto(int da, int su) const {
        const auto& cc = m.unita().at(da).contatti;
        const auto it = cc.find(su);
        return it == cc.end() ? nullptr : &it->second;
    }
};

}  // namespace

TEST_CASE("percezione dell'IA: il radar di una nave vede l'antenna di un sottomarino dell'IA" * doctest::skip(!fs::exists(M688_DATI "/emettitori.toml"))) {
    // un Krivak sovietico col radar acceso e un 688 dell'IA (non il giocatore) a quota periscopica con l'antenna su, a 6 km
    const auto f = scena("radar",
        "{ id = 2, classe = 'krivak2', categoria = 'nave', flag = 18, posizione = { x = 0, y = 6000 }, quota_m = 0, rotta = 90, velocita_kn = 5, " + NAVE + " },"
        "{ id = 3, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, posizione = { x = 0, y = 0 }, quota_m = 6.1, rotta = 90, velocita_kn = 3, " + SUB + " },",
        "A = function(u) m688.antenna(u, true) m688.salva_modello(u) end", "{ dopo_s = 1, unita = 3, gestore = 'A' }");
    Partita p(f);
    double quando = -1.0;
    for (int s = 0; s < 1800 && quando < 0.0; ++s) {
        p.avanza(1.0);
        if (const auto* c = p.contatto(2, 3); c && c->sensore == "radar") quando = p.m.tempo();
    }
    CHECK(quando > 0.0);
    const auto* c = p.contatto(2, 3);
    REQUIRE(c != nullptr);
    CHECK(c->posizione_nota);
    fs::remove(f);
}

TEST_CASE("percezione dell'IA: l'ESM di un sottomarino dell'IA intercetta il radar di una nave" * doctest::skip(!fs::exists(M688_DATI "/emettitori.toml"))) {
    // un Victor III sovietico con l'antenna su e una Perry col radar di navigazione a 15 km: il rilevamento, non la posizione
    const auto f = scena("esm",
        "{ id = 2, classe = 'perry', categoria = 'nave', flag = 10, posizione = { x = 0, y = 15000 }, quota_m = 0, rotta = 90, velocita_kn = 5, " + NAVE + " },"
        "{ id = 3, classe = 'victor3', categoria = 'sottomarino', flag = 17, posizione = { x = 0, y = 0 }, quota_m = 6.1, rotta = 90, velocita_kn = 3, " + SUB + " },",
        "A = function(u) m688.antenna(u, true) m688.salva_modello(u) end", "{ dopo_s = 1, unita = 3, gestore = 'A' }");
    Partita p(f);
    bool esm = false;
    for (int s = 0; s < 600 && !esm; ++s) {
        p.avanza(1.0);
        if (const auto* c = p.contatto(3, 2); c && c->sensore == "esm") esm = true;
    }
    REQUIRE(esm);
    const auto* c = p.contatto(3, 2);
    CHECK(std::fabs(normalizza_180(c->rilevamento - rilevamento(p.m.unita().at(3).pos, p.m.unita().at(2).pos))) < 10.0);
    fs::remove(f);
}

TEST_CASE("percezione dell'IA: le vedette di una nave vedono il periscopio di un sottomarino dell'IA") {
    // un Krivak e un 688 dell'IA col periscopio su (script) a 2 km, di giorno, mare calmo
    const auto f = scena("vista",
        "{ id = 2, classe = 'krivak2', categoria = 'nave', flag = 18, posizione = { x = 0, y = 2000 }, quota_m = 0, rotta = 90, velocita_kn = 3, " + NAVE + " },"
        "{ id = 3, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, posizione = { x = 0, y = 0 }, quota_m = 6.1, rotta = 90, velocita_kn = 3, " + SUB + " },",
        "P = function(u) m688.periscopio(u, true) m688.salva_modello(u) end", "{ dopo_s = 1, unita = 3, gestore = 'P' }");
    Partita p(f);
    p.m.usa_meteo(Missione::ModoMeteo::semplificato);
    p.m.usa_forze(false);   // con l'allerta il Krivak che classifica il 688 accende il radar: la prova e' sulla vedetta
    bool vista = false;
    for (int s = 0; s < 600 && !vista; ++s) {
        p.avanza(1.0);
        if (const auto* c = p.contatto(2, 3); c && c->sensore == "vista") vista = true;
    }
    CHECK(vista);
    REQUIRE(p.m.con_periscopi());
    fs::remove(f);
}

TEST_CASE("percezione dell'IA: dal passivo il rilevamento e la TMA, non una posizione regalata") {
    // un Victor III fermo che sente il 688 dell'IA che passa a 4 km: senza manovra propria la distanza non si osserva
    const auto f = scena("passivo",
        "{ id = 2, classe = 'victor3', categoria = 'sottomarino', flag = 17, posizione = { x = 0, y = 0 }, quota_m = 150, rotta = 0, velocita_kn = 0, " + SUB + " },"
        "{ id = 3, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, posizione = { x = -3000, y = 4000 }, quota_m = 150, rotta = 90, velocita_kn = 12, " + SUB + " },");
    Partita p(f);
    bool sentito = false, posizione = false, rilevamento_giusto = false;
    for (int s = 0; s < 300; ++s) {
        p.avanza(1.0);
        const auto* c = p.contatto(2, 3);
        if (!c || !c->visibile()) continue;
        sentito = true;
        posizione = posizione || c->posizione_nota;   // fermo: la TMA non e' osservabile
        rilevamento_giusto = rilevamento_giusto ||
                             std::fabs(normalizza_180(c->rilevamento - rilevamento(p.m.unita().at(2).pos, p.m.unita().at(3).pos))) < 10.0;
    }
    CHECK(sentito);
    CHECK(rilevamento_giusto);
    CHECK_FALSE(posizione);
    fs::remove(f);
}

TEST_CASE("percezione dell'IA: il radar del giocatore acceso si fa intercettare, e sott'acqua si spegne" * doctest::skip(!fs::exists(M688_DATI "/emettitori.toml"))) {
    // il giocatore emerso col BPS-15 e un Victor III con l'antenna su a 15 km
    const auto f = fs::temp_directory_path() / "m688_prova_percezione_radar_giocatore.lua";
    std::ofstream(f) << "return { id = 'PERC_RADAR', teatro = 'faroe', lato = 'usa', data = '1989-06-15', ora_utc = 12, paesaggio_sonoro = false,"
                        " script = { A = function(u) m688.antenna(u, true) m688.salva_modello(u) end }, varianti = { { probabilita = 1, unita = {\n"
                        "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true, posizione = { x = 0, y = 0 },"
                        "  quota_m = 0, rotta = 90, velocita_kn = 3, " << SUB << " },\n"
                        "{ id = 3, classe = 'victor3', categoria = 'sottomarino', flag = 17, posizione = { x = 0, y = 15000 }, quota_m = 6.1, rotta = 90,"
                        "  velocita_kn = 3, " << SUB << " },\n}, timer = { { dopo_s = 1, unita = 3, gestore = 'A' } } } } }\n";
    Partita p(f.string());
    p.avanza(2.0);
    CHECK_FALSE(p.m.radar_giocatore_acceso());
    REQUIRE(p.m.applica({p.m.tempo(), TipoOrdine::radar, 0.0, 0.0, 1}));
    CHECK(p.m.radar_giocatore_acceso());
    CHECK(std::any_of(p.m.messaggi().begin(), p.m.messaggi().end(), [](const Messaggio& x) { return x.codice == 458; }));
    bool esm = false;
    for (int s = 0; s < 600 && !esm; ++s) {
        p.avanza(1.0);
        if (const auto* c = p.contatto(3, 0); c && c->sensore == "esm") esm = true;
    }
    CHECK(esm);
    // in immersione si spegne da solo
    p.m.trova(0)->quota = p.m.trova(0)->quota_ordinata = 60.0;   // la scena non ha i gestori della manovra
    p.avanza(2.0);
    CHECK_FALSE(p.m.radar_giocatore_acceso());
    CHECK(std::any_of(p.m.messaggi().begin(), p.m.messaggi().end(), [](const Messaggio& x) { return x.codice == 459; }));
    // senza l'elettronica dell'Enhanced l'ordine non vale
    Missione classico(f.string(), 1, false);
    CHECK_FALSE(classico.applica({0.0, TipoOrdine::radar, 0.0, 0.0, 1}));
    fs::remove(f);
}

TEST_CASE("percezione dell'IA: il sonarista di un sottomarino dell'IA riferisce, poi classifica (fase 2, C)") {
    // un Victor III che sente una Perry a 15 nodi a 5 km; sonarista esperto per la chiave d'unita'
    const auto f = scena("sonarista",
        "{ id = 2, classe = 'victor3', categoria = 'sottomarino', flag = 17, sonarista = 'esperto', posizione = { x = 0, y = 0 }, quota_m = 150,"
        "  rotta = 0, velocita_kn = 4, " + SUB + " },"
        "{ id = 3, classe = 'perry', categoria = 'nave', flag = 10, posizione = { x = 0, y = 5000 }, quota_m = 0, rotta = 90,"
        "  velocita_kn = 15, " + NAVE + " },");
    Partita p(f);
    p.avanza(20.0);
    if (const auto* c = p.contatto(2, 3)) CHECK(c->durata < 800.0);   // non ancora riferito: sotto la media
    for (int s = 0; s < 1200 && p.m.classificazioni_ia().empty(); ++s) p.avanza(1.0);
    REQUIRE_FALSE(p.m.classificazioni_ia().empty());
    const auto& k = p.m.classificazioni_ia().front();
    CHECK(k.unita == 2);
    CHECK(k.sorgente == 3);
    const auto& ev = p.m.registro().eventi();
    CHECK(std::any_of(ev.begin(), ev.end(), [](const EventoMissione& e) { return e.tipo == TipoEvento::classificazione_ia && e.unita == 2 && !e.noto; }));
    fs::remove(f);
}

TEST_CASE("percezione dell'IA: i radar vedono le navi in superficie, quello del giocatore e quelli dell'IA" * doctest::skip(!fs::exists(M688_DATI "/emettitori.toml"))) {
    // il giocatore emerso col BPS-15, un Krivak a 12 km e una Perry a 15 km dal Krivak
    const auto f = fs::temp_directory_path() / "m688_prova_percezione_radar_navi.lua";
    std::ofstream(f) << "return { id = 'PERC_NAVI', teatro = 'faroe', lato = 'usa', data = '1989-06-15', ora_utc = 12, paesaggio_sonoro = false,"
                        " script = {}, varianti = { { probabilita = 1, unita = {\n"
                        "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true, posizione = { x = 0, y = 0 },"
                        "  quota_m = 0, rotta = 90, velocita_kn = 3, " << SUB << " },\n"
                        "{ id = 2, classe = 'krivak2', categoria = 'nave', flag = 18, posizione = { x = 0, y = 12000 }, quota_m = 0, rotta = 90, velocita_kn = 5, " << NAVE << " },\n"
                        "{ id = 3, classe = 'perry', categoria = 'nave', flag = 10, posizione = { x = 15000, y = 12000 }, quota_m = 0, rotta = 90, velocita_kn = 5, " << NAVE << " },\n"
                        "}, timer = {} } } }\n";
    Partita p(f.string());
    p.avanza(2.0);
    REQUIRE(p.m.applica({p.m.tempo(), TipoOrdine::radar, 0.0, 0.0, 1}));
    bool giocatore = false, perry = false;
    for (int s = 0; s < 120 && !(giocatore && perry); ++s) {
        p.avanza(1.0);
        if (const auto* c = p.contatto(0, 2); c && c->sensore == "radar") giocatore = true;
        if (const auto* c = p.contatto(3, 2); c && c->sensore == "radar") perry = true;
    }
    CHECK(giocatore);
    CHECK(perry);
    REQUIRE(p.contatto(0, 2) != nullptr);
    CHECK(p.contatto(0, 2)->posizione_nota);
    fs::remove(f);
}

TEST_CASE("percezione dell'IA: un sottomarino dell'IA vicino al giocatore sente anche il paesaggio sonoro (fase 2, F)") {
    // il giocatore e un Victor III a 3 km, col paesaggio sonoro acceso: il sonarista del Victor traccia anche le istanze
    const auto f = fs::temp_directory_path() / "m688_prova_percezione_paesaggio.lua";
    std::ofstream(f) << "return { id = 'PERC_PAESAGGIO', teatro = 'faroe', lato = 'usa', data = '1989-02-15', ora_utc = 12, paesaggio_sonoro = true,"
                        " script = {}, varianti = { { probabilita = 1, unita = {\n"
                        "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true, posizione = { x = 0, y = 0 },"
                        "  quota_m = 150, rotta = 90, velocita_kn = 3, " << SUB << " },\n"
                        "{ id = 2, classe = 'victor3', categoria = 'sottomarino', flag = 17, sonarista = 'esperto', posizione = { x = 0, y = 3000 },"
                        "  quota_m = 150, rotta = 90, velocita_kn = 3, " << SUB << " },\n}, timer = {} } } }\n";
    Partita p(f.string());
    p.avanza(1800.0);
    REQUIRE(p.m.sonarista_ia(2) != nullptr);
    const auto& tracce = p.m.sonarista_ia(2)->tracce();
    CHECK(std::any_of(tracce.begin(), tracce.end(), [](const TracciaSonar& t) { return t.sorgente >= 100000; }));
    fs::remove(f);
}

namespace {
// un 688 dell'IA con la sola schiera (la cupola guasta dai danni) e un Victor III rumoroso al traverso a dritta, a 8 km
std::string scena_schiera(const std::string& nome) {
    const std::string SUB_SCHIERA = "modello = { massima_kn = 32, virata_gradi_s = 1, accel_kn_s = 0.5, quota_m_s = 1, quota_max_m = 450, integrita = 200,"
                                    " rumore = 8, sonar = 19, schiera_trainata = true }";
    return scena(nome,
        "{ id = 2, classe = 'victor3', categoria = 'sottomarino', flag = 17, posizione = { x = 8000, y = 1500 }, quota_m = 150, rotta = 0, velocita_kn = 14, " + SUB + " },"
        "{ id = 3, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, posizione = { x = 0, y = 0 }, quota_m = 150, rotta = 0, velocita_kn = 5, " +
            SUB_SCHIERA + " },");
}
}  // namespace

TEST_CASE("percezione dell'IA: la schiera dell'IA crede un lato a caso, e un'accostata lo risolve (simmetria, punto 4)") {
    const auto f = scena_schiera("schiera_lato");
    int veri = 0, specchi = 0, risolti = 0, prove = 0;
    for (std::uint64_t seme = 1; seme <= 12; ++seme) {
        Missione m(f, seme, false);
        m.acustica_enhanced(M688_DATI);
        m.sonar_ia(3, false, true);   // solo la schiera: nessun sensore senza specchio
        const auto vero = [&] { return rilevamento(m.unita().at(3).pos, m.unita().at(2).pos); };
        const auto sul = [&](double a) {
            const auto it = m.unita().at(3).contatti.find(2);
            return it != m.unita().at(3).contatti.end() && it->second.sensore == "tb16" && std::fabs(normalizza_180(it->second.rilevamento - a)) < 12.0;
        };
        for (int i = 0; i < 300 * 9; ++i) m.avanza(1.0 / 9.0);
        if (sul(vero())) ++veri;
        else if (sul(normalizza_360(2.0 * m.unita().at(3).rotta - vero()))) ++specchi;
        else continue;
        ++prove;
        // l'accostata di 60 gradi a sinistra (a dritta il Victor finirebbe nel cono sordo di prua della schiera): il
        // rilevamento falso salta, il lato si sa
        m.trova(3)->rotta = m.trova(3)->rotta_ordinata = 300.0;
        for (int i = 0; i < 120 * 9; ++i) m.avanza(1.0 / 9.0);
        if (sul(vero())) ++risolti;

    }
    CHECK(prove >= 10);
    CHECK(veri >= 2);
    CHECK(specchi >= 2);   // senza il punto 4 l'IA prendeva sempre il lato vero
    CHECK(risolti == prove);
    fs::remove(f);
}

TEST_CASE("percezione dell'IA: la cupola e la schiera guaste dai danni tacciono anche per l'IA (simmetria, punto 5)") {
    const auto f = scena_schiera("schiera_guasta");
    Missione m(f, 1, false);
    m.acustica_enhanced(M688_DATI);
    m.sonar_ia(3, false, false);   // cupola e schiera guaste
    for (int i = 0; i < 300 * 9; ++i) m.avanza(1.0 / 9.0);
    const auto& cc = m.unita().at(3).contatti;
    const auto it = cc.find(2);
    CHECK((it == cc.end() || (it->second.sensore != "tb16" && it->second.sensore != "bqq5_sfera")));
    // riparate: la schiera torna ad ascoltare
    m.sonar_ia(3, true, true);
    bool sentito = false;
    for (int i = 0; i < 300 * 9 && !sentito; ++i) {
        m.avanza(1.0 / 9.0);
        const auto k = m.unita().at(3).contatti.find(2);
        sentito = k != m.unita().at(3).contatti.end() && (k->second.sensore == "tb16" || k->second.sensore == "bqq5_sfera");
    }
    CHECK(sentito);
    fs::remove(f);
}

TEST_CASE("percezione dell'IA: il sonarista dell'IA classifica anche una nave della sua parte (simmetria, punto 1)") {
    // un Victor III con sonarista esperto e un Krivak sovietico a 15 nodi a 5 km: senza il filtro sulla parte vera
    // (come il sonarista del giocatore, che le righe le ha per tutti) il Krivak arriva alla classe
    const auto f = scena("sonarista_amico",
        "{ id = 2, classe = 'victor3', categoria = 'sottomarino', flag = 17, sonarista = 'esperto', posizione = { x = 0, y = 0 }, quota_m = 150,"
        "  rotta = 0, velocita_kn = 4, " + SUB + " },"
        "{ id = 3, classe = 'krivak2', categoria = 'nave', flag = 18, posizione = { x = 0, y = 5000 }, quota_m = 0, rotta = 90,"
        "  velocita_kn = 15, " + NAVE + " },");
    Partita p(f);
    for (int s = 0; s < 1200 && p.m.classificazioni_ia().empty(); ++s) p.avanza(1.0);
    REQUIRE_FALSE(p.m.classificazioni_ia().empty());
    CHECK(p.m.classificazioni_ia().front().unita == 2);
    CHECK(p.m.classificazioni_ia().front().sorgente == 3);
    fs::remove(f);
}

TEST_CASE("percezione dell'IA: un Victor III vero a 3,7 km non e' una nave per nessuno; il cavo della schiera e' una riga propria") {
    // nota del runtime del 03/11/2026 (§4.1): il 688 dell'IA e la Perry lo dicevano NONSUB senza classe. Le righe della
    // vibrazione del cavo (sorgente -2, la TB-16 e la SQR-19) passavano per righe del contatto e votavano «nave»; ora le
    // maschera l'operatore come le proprie, per il giocatore e per l'IA. Giugno, come la scena del falso contatto
    const auto f = fs::temp_directory_path() / "m688_prova_percezione_victor_vero.lua";
    std::ofstream(f) << "return { id = 'PERC_VICTOR_VERO', teatro = 'faroe', lato = 'usa', data = '1989-06-15', ora_utc = 12,"
                        " paesaggio_sonoro = false, script = {}, varianti = { { probabilita = 1, unita = {\n"
                        "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true, posizione = { x = 3700, y = 0 },"
                        "  quota_m = 150, rotta = 0, velocita_kn = 4, " << SUB << " },\n"
                        "{ id = 2, classe = 'perry', categoria = 'nave', flag = 10, posizione = { x = 0, y = 3700 }, quota_m = 0, rotta = 0,"
                        "  velocita_kn = 5, " << NAVE << " },\n"
                        "{ id = 4, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, posizione = { x = 0, y = -3700 }, quota_m = 150,"
                        "  rotta = 0, velocita_kn = 4, " << SUB << " },\n"
                        "{ id = 6, classe = 'victor3', categoria = 'sottomarino', flag = 17, posizione = { x = 0, y = 0 }, quota_m = 100,"
                        "  rotta = 90, velocita_kn = 8, " << SUB << " },\n"
                        "}, timer = {} } } }\n";
    Partita p(f.string());
    p.m.imposta_sonarista(Esperienza::medio);
    p.avanza(900.0);
    int dal_688 = 0, dalla_perry = 0, dal_giocatore = 0;
    for (const auto& c : p.m.classificazioni_ia()) {
        if (c.sorgente != 6 || c.visiva) continue;
        CAPTURE(c.unita);
        CAPTURE(c.classe);
        CAPTURE(static_cast<int>(c.confidenza));
        CHECK_FALSE((c.classe.empty() && c.confidenza == Confidenza::nonsub));
        dal_688 += c.unita == 4;
        dalla_perry += c.unita == 2;
    }
    for (const auto& a : p.m.avvisi_sonarista()) {
        if (a.codice != 411 || a.sorgente != 6) continue;
        CAPTURE(a.testo_it);
        CHECK(a.testo_it.find("non sottomarino") == std::string::npos);
        ++dal_giocatore;
    }
    MESSAGE("classificazioni del Victor: 688 dell'IA ", dal_688, ", Perry ", dalla_perry, ", giocatore ", dal_giocatore);
    CHECK(dal_688 + dalla_perry + dal_giocatore >= 1);
    fs::remove(f);
}
