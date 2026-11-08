#include <doctest.h>

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <map>
#include <optional>
#include <set>
#include <string>

#include "missioni/danni_missione.hpp"
#include "missioni/interventi.hpp"
#include "missioni/missione.hpp"
#include "missioni/registro_missione.hpp"
#include "missioni/traccia_missione.hpp"
#include "nucleo/classi.hpp"
#include "nucleo/geometria.hpp"
#include "nucleo/ordini.hpp"
#include "nucleo/traccia_replay.hpp"

using namespace m688;
namespace fs = std::filesystem;

// Il vetrino di Dio (docs/28, D1; missione_interventi.cpp): il catalogo, l'ingresso, la coda e il punto del passo, il
// registro dello spettatore; togli, ordine e unita' nuova.

namespace {

const std::string SUB = "modello = { massima_kn = 32, virata_gradi_s = 1, accel_kn_s = 0.5, quota_m_s = 1, quota_max_m = 450, integrita = 200, rumore = 8, sonar = 19 }";
const std::string NAVE = "modello = { massima_kn = 32, virata_gradi_s = 2, accel_kn_s = 0.5, quota_m_s = 1, quota_max_m = 0, integrita = 100, rumore = 10, sonar = 20 }";

std::string scena() {
    const auto f = fs::temp_directory_path() / "m688_prova_interventi.lua";
    std::ofstream(f) << "return { id = 'PROVA_INTERVENTI', teatro = 'faroe', lato = 'usa', data = '1989-06-15', ora_utc = 12, paesaggio_sonoro = false,"
                        " script = {}, varianti = { { probabilita = 1, unita = {\n"
                        "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true, posizione = { x = 0, y = -60000 },"
                        "  quota_m = 200, rotta = 90, velocita_kn = 3, " << SUB << " },\n"
                        "{ id = 2, classe = 'perry', categoria = 'nave', flag = 10, posizione = { x = 0, y = 0 }, quota_m = 0, rotta = 0, velocita_kn = 10, " << NAVE << " },\n"
                        "{ id = 3, classe = 'victor3', categoria = 'sottomarino', flag = 17, posizione = { x = 8000, y = 0 }, quota_m = 150, rotta = 0, velocita_kn = 5, " << SUB << " },\n"
                        "}, timer = {} } } }\n";
    return f.string();
}

struct Vetrino {
    std::map<std::string, ClasseNave> classi = carica_classi(M688_DATI "/classi.toml");
    Missione m;
    DanniMissione dm;
    explicit Vetrino(bool sandbox = true, const std::string& file = scena())
        : m(file, 1, false), dm(classi, M688_DATI, 1, RegolaDanni::simulazione) {
        m.acustica_enhanced(M688_DATI);
        m.senza_giocatore(sandbox);
    }
    void avanza(double s) {
        for (long i = 0; i < static_cast<long>(s * 9.0 + 0.5); ++i) {
            m.avanza(1.0 / 9.0);
            dm.avanza(m, 1.0 / 9.0);
        }
    }
    const EventoMissione* evento(int id, int livello) const {
        for (const auto& e : m.eventi_missione())
            if (e.tipo == TipoEvento::intervento && e.codice == id && e.livello == livello) return &e;
        return nullptr;
    }
};

}  // namespace

TEST_CASE("interventi: il catalogo della D1, coi gruppi") {
    const auto& c = catalogo_interventi();
    for (const char* t : {"unita_nuova", "togli", "ordine", "situazione", "roe", "allerta", "macchine", "sensore", "tubi", "timone", "meteo", "ora"}) {
        CAPTURE(t);
        const auto* v = voce_intervento(t);
        REQUIRE(v != nullptr);
        CHECK_FALSE(v->gruppo.empty());
        CHECK_FALSE(v->testo_it.empty());
        CHECK_FALSE(v->testo_en.empty());
    }
    CHECK(c.size() >= 12);
    CHECK(voce_intervento("nessuno") == nullptr);
}

TEST_CASE("interventi: solo nel vetrino dell'Enhanced; i controlli dell'ingresso") {
    Vetrino giocato(false);
    Intervento x;
    x.tipo = "togli";
    x.unita = 2;
    CHECK_FALSE(giocato.m.intervieni(x).ok);   // col giocatore ai comandi no
    Vetrino v;
    CHECK(v.m.intervieni(x).ok);
    x.unita = 99;
    CHECK_FALSE(v.m.intervieni(x).ok);         // unita' assente
    Intervento o;
    o.tipo = "ordine";
    o.unita = 2;
    o.parametri["velocita_kn"] = 99.0;
    CHECK_FALSE(v.m.intervieni(o).ok);         // fuori dai limiti
    Intervento n;
    n.tipo = "unita_nuova";
    n.testi = {{"classe", "perry"}, {"parte", "usa"}};
    CHECK_FALSE(v.m.intervieni(n).ok);         // senza punto
    n.punto = Vec2{1000.0, 1000.0};
    n.testi["classe"] = "nessuna";
    CHECK_FALSE(v.m.intervieni(n).ok);         // classe sconosciuta
    Intervento t;
    t.tipo = "inventato";
    CHECK_FALSE(t.tipo.empty());
    CHECK_FALSE(v.m.intervieni(t).ok);
}

TEST_CASE("interventi: togli subito e a tempo; il registro dello spettatore; annulla prima dell'applicazione") {
    Vetrino v;
    v.avanza(5.0);
    Intervento x;
    x.tipo = "togli";
    x.unita = 3;
    x.tempo = 30.0;
    const auto e = v.m.intervieni(x);
    REQUIRE(e.ok);
    Intervento y;
    y.tipo = "togli";
    y.unita = 2;
    y.tempo = 40.0;
    const auto ey = v.m.intervieni(y);
    REQUIRE(ey.ok);
    CHECK(v.m.annulla_intervento(ey.id));      // annullato prima: non avviene
    CHECK(v.m.interventi().size() == 1);
    v.avanza(20.0);
    CHECK(v.m.unita().count(3) == 1);          // non ancora
    v.avanza(30.0);
    CHECK(v.m.unita().count(3) == 0);
    CHECK(v.m.unita().count(2) == 1);
    REQUIRE(v.m.stati_interventi().size() == 1);
    const auto& st = v.m.stati_interventi().front();
    CHECK(st.applicato);
    CHECK(st.finito);
    CHECK(st.inizio == doctest::Approx(30.0).epsilon(0.01));
    const auto* ev = v.evento(e.id, 1);
    REQUIRE(ev != nullptr);
    CHECK_FALSE(ev->noto);
    CHECK(ev->classe_arma == "togli");
    CHECK(ev->testo_it.find("Togli") != std::string::npos);
    CHECK_FALSE(v.m.annulla_intervento(e.id));   // gia' applicato
}

TEST_CASE("interventi: l'ordine scavalca la dottrina per la sua durata, poi l'unita' torna com'era") {
    Vetrino v;
    v.avanza(2.0);
    const double prima = v.m.unita().at(2).velocita_ordinata;
    Intervento x;
    x.tipo = "ordine";
    x.unita = 2;
    x.parametri = {{"rotta", 90.0}, {"velocita_kn", 20.0}};
    x.durata_s = 60.0;
    const auto e = v.m.intervieni(x);   // subito
    REQUIRE(e.ok);
    v.avanza(30.0);
    CHECK(v.m.unita().at(2).rotta_ordinata == doctest::Approx(90.0));
    CHECK(v.m.unita().at(2).velocita_ordinata == doctest::Approx(20.0));
    CHECK(v.m.interventi_attivi(v.m.tempo()).size() == 1);
    CHECK(v.m.interventi().front().tempo == doctest::Approx(2.0 + 1.0 / 9.0).epsilon(0.01));   // il «subito» col tempo del passo
    v.avanza(40.0);
    CHECK(v.m.unita().at(2).velocita_ordinata == doctest::Approx(prima));
    CHECK(v.m.interventi_attivi(v.m.tempo()).empty());
    CHECK(v.evento(e.id, 2) != nullptr);   // la fine nel registro
}

TEST_CASE("interventi: l'unita' nuova compare dove si vuole, con la sua parte, e si muove") {
    Vetrino v;
    v.avanza(2.0);
    const std::size_t prima = v.m.unita().size();
    Intervento x;
    x.tipo = "unita_nuova";
    x.punto = Vec2{5000.0, 5000.0};
    x.testi = {{"classe", "krivak2"}, {"parte", "urss"}};
    x.parametri = {{"rotta", 180.0}, {"velocita_kn", 15.0}};
    REQUIRE(v.m.intervieni(x).ok);
    Intervento s;
    s.tipo = "unita_nuova";
    s.punto = Vec2{-5000.0, 5000.0};
    s.testi = {{"classe", "kilo"}, {"parte", "urss"}};
    s.parametri = {{"quota_m", 80.0}, {"velocita_kn", 5.0}};
    REQUIRE(v.m.intervieni(s).ok);
    v.avanza(60.0);
    CHECK(v.m.unita().size() == prima + 2);
    const UnitaClassica* k = nullptr;
    const UnitaClassica* sub = nullptr;
    for (const auto& [id, u] : v.m.unita()) {
        if (u.classe == "krivak2") k = &u;
        if (u.classe == "kilo") sub = &u;
    }
    REQUIRE(k != nullptr);
    REQUIRE(sub != nullptr);
    CHECK(k->categoria == "nave");
    CHECK((k->flag & 0x18) == 0x10);
    CHECK(k->pos.y < 5000.0 - 300.0);   // verso sud a 15 nodi
    CHECK(sub->categoria == "sottomarino");
    CHECK(sub->quota == doctest::Approx(80.0).epsilon(0.05));
}

TEST_CASE("interventi: deterministici, lo stesso elenco da' la stessa partita") {
    const auto gioca = [] {
        Vetrino v;
        Intervento n;
        n.tipo = "unita_nuova";
        n.tempo = 20.0;
        n.punto = Vec2{3000.0, 0.0};
        n.testi = {{"classe", "perry"}, {"parte", "usa"}};
        n.parametri = {{"rotta", 45.0}, {"velocita_kn", 12.0}};
        v.m.intervieni(n);
        Intervento o;
        o.tipo = "ordine";
        o.tempo = 40.0;
        o.unita = 3;
        o.parametri = {{"rotta", 270.0}};
        o.durata_s = 100.0;
        v.m.intervieni(o);
        v.avanza(300.0);
        std::vector<double> x;
        for (const auto& [id, u] : v.m.unita()) x.push_back(u.pos.x), x.push_back(u.pos.y);
        return x;
    };
    CHECK(gioca() == gioca());
}

TEST_CASE("interventi: i guasti; il sonar a meta' portata (-6 dB), spento, la schiera lasciata") {
    // il Victor (3) sente la Perry (2) a 8 km: con lo stesso seme, l'eccesso di segnale con e senza il guasto
    const auto eccesso = [](double fattore) {
        Vetrino v;
        if (fattore >= 0.0) {
            Intervento x;
            x.tipo = "sensore";
            x.unita = 3;
            x.testi["sensore"] = "sonar";
            x.parametri["fattore"] = fattore;
            REQUIRE(v.m.intervieni(x).ok);
        }
        v.avanza(120.0);
        const auto& cc = v.m.unita().at(3).contatti;
        const auto k = cc.find(2);
        return k == cc.end() || v.m.tempo() - k->second.ultima_scoperta > 5.0 ? -1e9 : k->second.eccesso_db;
    };
    const double sano = eccesso(-1.0), meta = eccesso(0.5), spento = eccesso(0.0);
    MESSAGE("eccesso sul Victor: sano ", sano, " dB, a meta' portata ", meta, " dB");
    REQUIRE(sano > 0.0);
    CHECK(meta == doctest::Approx(sano - 6.02).epsilon(0.05));
    CHECK(spento < -1e8);   // nessuna scoperta recente
}

TEST_CASE("interventi: macchine ferme, timone bloccato, poi liberi") {
    // le macchine del Victor (3) ferme; il timone della Perry (2) bloccato a 3 gradi/s per un minuto (la nave col moto
    // classico: il rateo e' la virata; un sottomarino a dinamica prende l'abbrivio dell'accostata piu' piano)
    Vetrino v;
    v.avanza(2.0);
    Intervento m;
    m.tipo = "macchine";
    m.unita = 3;
    m.parametri["velocita_max_frazione"] = 0.0;
    REQUIRE(v.m.intervieni(m).ok);
    Intervento t;
    t.tipo = "timone";
    t.unita = 2;
    t.parametri["timone_gradi_s"] = 3.0;
    t.durata_s = 60.0;
    REQUIRE(v.m.intervieni(t).ok);
    const double rotta0 = v.m.unita().at(2).rotta, velocita0 = v.m.unita().at(3).velocita;
    v.avanza(50.0);
    CHECK(v.m.unita().at(3).velocita_ordinata == doctest::Approx(0.0));
    CHECK(v.m.unita().at(3).velocita < velocita0);   // l'abbrivio si spegne
    CHECK(std::fabs(normalizza_180(v.m.unita().at(2).rotta - rotta0)) > 25.0);   // gira da sola
    v.avanza(120.0);
    const double r1 = v.m.unita().at(2).rotta;
    v.avanza(30.0);
    CHECK(std::fabs(normalizza_180(v.m.unita().at(2).rotta - r1)) < 5.0);   // libera: torna sulla rotta di prima e la tiene
    CHECK_FALSE(v.m.unita().at(2).rotta_libera);
}

TEST_CASE("interventi: i tubi bloccati, il comandante prova e non puo'") {
    const auto f = fs::temp_directory_path() / "m688_prova_interventi_tubi.lua";
    std::ofstream(f) << "return { id = 'PROVA_INTERVENTI_TUBI', teatro = 'faroe', lato = 'usa', data = '1989-06-15', ora_utc = 12, paesaggio_sonoro = false,"
                        " carriera = { situazione = 'guerra' }, editor = {},"
                        " script = { L = function(u) m688.bersaglio(u, 2) m688.memorizza_bersaglio(u) m688.modello_azzera(u) m688.modello_classe(u, 'type53')"
                        " m688.modello_flag(u, 49) m688.lancia(u, '', '') m688.modello_avanti_tutta(u, 45.7) m688.modello_integrita(u, 20)"
                        " m688.modello_quota_max(u, 548) m688.telegrafo(u, 1) m688.navigazione(u, 1) m688.collega(u, 1) m688.nuova_unita(u) end },"
                        " varianti = { { probabilita = 1, unita = {\n"
                        "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true, posizione = { x = -90000, y = -80000 },"
                        "  quota_m = 300, rotta = 90, velocita_kn = 3, " << SUB << " },\n"
                        "{ id = 2, classe = 'perry', categoria = 'nave', flag = 10, posizione = { x = 0, y = 6000 }, quota_m = 0, rotta = 90, velocita_kn = 5, " << NAVE << " },\n"
                        "{ id = 3, classe = 'victor3', categoria = 'sottomarino', flag = 17, posizione = { x = 0, y = 0 }, quota_m = 100, rotta = 90, velocita_kn = 5, " << SUB << " },\n"
                        "}, timer = { { dopo_s = 10, unita = 3, gestore = 'L' } } } } }\n";
    const auto siluri = [&](bool bloccati) {
        Vetrino v(true, f.string());
        if (bloccati) {
            Intervento x;
            x.tipo = "tubi";
            x.unita = 3;
            x.parametri["tubi"] = -1.0;
            REQUIRE(v.m.intervieni(x).ok);
        }
        v.avanza(15.0);
        int n = 0;
        for (const auto& [id, u] : v.m.unita()) n += u.lanciatore == 3 && v.m.siluro(id) != nullptr;
        return n;
    };
    CHECK(siluri(false) == 1);
    CHECK(siluri(true) == 0);
    fs::remove(f);
}

TEST_CASE("interventi: situazione, ROE e allerta di una parte; l'ora; il mare") {
    Vetrino v;
    v.m.forze_enhanced(M688_DATI);
    v.avanza(2.0);
    const auto x = [&](const char* tipo, std::map<std::string, std::string> testi, std::map<std::string, double> par = {}, int unita = -1) {
        Intervento i;
        i.tipo = tipo;
        i.testi = std::move(testi);
        i.parametri = std::move(par);
        i.unita = unita;
        const auto e = v.m.intervieni(i);
        CHECK_MESSAGE(e.ok, tipo, ": ", e.motivo_it);
        return e.id;
    };
    x("situazione", {{"parte_a", "usa"}, {"parte_b", "urss"}, {"situazione", "pace"}});
    x("roe", {{"parte", "urss"}, {"categoria", "navi"}, {"regola", "trattenute"}});
    x("allerta", {}, {{"livello", 3.0}}, 3);
    const double ora0 = v.m.ora_utc_inizio_s();
    x("ora", {}, {{"ore", 6.0}});
    const int meteo = x("meteo", {}, {{"mare", 6.0}});
    v.avanza(2.0);
    CHECK(v.m.situazione_fra(0x08, 0x10) == Situazione::pace);
    CHECK(v.m.roe_parte(0x10, CategoriaRoe::navi) == std::optional<bool>(false));
    CHECK(v.m.allerta(0x10) == std::optional<int>(3));
    CHECK(v.m.ora_utc_inizio_s() == doctest::Approx(ora0 + 6.0 * 3600.0));
    const auto& st = v.m.stati_interventi()[static_cast<std::size_t>(meteo)];
    if (st.applicato) CHECK(v.m.stato_mare() == 6);   // col meteo dell'Enhanced
    else CHECK_FALSE(st.motivo_it.empty());
    for (const auto& s : v.m.stati_interventi()) CHECK((s.applicato || s.id == meteo));
}

TEST_CASE("interventi: nella traccia del replay (record X), ogni fase col suo tempo, anche fra due fotogrammi") {
    Vetrino v;
    RaccoltaTraccia raccolta;
    RegistratoreTraccia reg("PROVA_INTERVENTI", 1);
    Intervento o;
    o.tipo = "ordine";
    o.unita = 2;
    o.tempo = 7.0;
    o.parametri = {{"rotta", 90.0}};
    o.testi = {{"gestore", ""}};
    o.durata_s = 3.0;   // applicato e finito fra due fotogrammi da 5 s
    REQUIRE(v.m.intervieni(o).ok);
    Intervento t;
    t.tipo = "togli";
    t.unita = 3;
    t.tempo = 12.0;
    REQUIRE(v.m.intervieni(t).ok);
    for (int s = 0; s < 30; ++s) {
        v.avanza(1.0);
        if (reg.serve(v.m.tempo(), v.m.eventi_missione().size()))
            reg.aggiungi(fotogramma_da(v.m, nullptr, {}, reg.ultimo_evento(), &raccolta), v.m.eventi_missione().size());
    }
    const Traccia tr = leggi_traccia(reg.dati());
    REQUIRE(tr.interventi.size() == 3);   // ordine applicato e finito, togli applicato (istantaneo)
    CHECK(tr.interventi[0].tipo == "ordine");
    CHECK(tr.interventi[0].fase == RecordIntervento::FASE_APPLICATO);
    CHECK(tr.interventi[0].t == doctest::Approx(7.0).epsilon(0.02));
    CHECK(tr.interventi[1].fase == RecordIntervento::FASE_FINITO);
    CHECK(tr.interventi[1].t == doctest::Approx(10.0).epsilon(0.02));
    CHECK(tr.interventi[2].tipo == "togli");
    CHECK(tr.interventi[2].unita == 3);
    const auto chiavi = leggi_chiavi(tr.interventi[0].chiavi);
    CHECK(chiavi.at("#rotta") == "90");
    CHECK(chiavi.count("gestore") == 1);
    CHECK_FALSE(tr.interventi[0].riga_it.empty());
}

TEST_CASE("interventi: il mare per un tempo, poi di nuovo il meteo del teatro") {
    Vetrino v;
    v.m.meteo_enhanced(M688_DATI);
    v.avanza(2.0);
    const int prima = v.m.stato_mare();
    const int voluto = prima >= 6 ? 2 : 7;
    Intervento x;
    x.tipo = "meteo";
    x.parametri["mare"] = voluto;
    x.durata_s = 60.0;
    const auto e = v.m.intervieni(x);
    REQUIRE(e.ok);
    v.avanza(10.0);
    REQUIRE(v.m.stati_interventi().back().applicato);
    CHECK(v.m.stato_mare() == voluto);
    v.avanza(70.0);
    CHECK(v.m.stati_interventi().back().finito);
    CHECK(std::abs(v.m.stato_mare() - prima) <= 1);   // il teatro di nuovo (puo' cambiare di poco in un minuto)
}

TEST_CASE("interventi: il radar spento non scopre e non si fa intercettare") {
    const auto f = fs::temp_directory_path() / "m688_prova_interventi_radar.lua";
    std::ofstream(f) << "return { id = 'PROVA_INTERVENTI_RADAR', teatro = 'faroe', lato = 'usa', data = '1989-06-15', ora_utc = 12, paesaggio_sonoro = false,"
                        " script = { A = function(u) m688.antenna(u, true) m688.salva_modello(u) end }, varianti = { { probabilita = 1, unita = {\n"
                        "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true, posizione = { x = -90000, y = -80000 },"
                        "  quota_m = 300, rotta = 90, velocita_kn = 3, " << SUB << " },\n"
                        "{ id = 2, classe = 'perry', categoria = 'nave', flag = 10, posizione = { x = 0, y = 15000 }, quota_m = 0, rotta = 90, velocita_kn = 5, " << NAVE << " },\n"
                        "{ id = 3, classe = 'victor3', categoria = 'sottomarino', flag = 17, posizione = { x = 0, y = 0 }, quota_m = 6.1, rotta = 90, velocita_kn = 3, " << SUB << " },\n"
                        "}, timer = { { dopo_s = 1, unita = 3, gestore = 'A' } } } } }\n";
    const auto esm = [&](bool spento) {
        Vetrino v(true, f.string());
        if (spento) {
            Intervento x;
            x.tipo = "sensore";
            x.unita = 2;
            x.testi["sensore"] = "radar";
            x.parametri["fattore"] = 0.0;
            REQUIRE(v.m.intervieni(x).ok);
        }
        for (int s = 0; s < 600; ++s) {
            v.avanza(1.0);
            const auto& cc = v.m.unita().at(3).contatti;
            if (const auto k = cc.find(2); k != cc.end() && k->second.sensore == "esm") return true;
        }
        return false;
    };
    CHECK(esm(false));
    CHECK_FALSE(esm(true));
    fs::remove(f);
}

TEST_CASE("interventi: il meteo in una zona, sopra quello del teatro, finche' dura") {
    Vetrino v;
    v.m.meteo_enhanced(M688_DATI);
    v.avanza(2.0);
    const int teatro = v.m.meteo_in({50000.0, 50000.0}).stato_mare();
    const int voluto = teatro >= 6 ? 2 : 8;
    Intervento x;
    x.tipo = "meteo";
    x.punto = Vec2{0.0, 0.0};
    x.raggio_m = 5000.0;
    x.parametri["mare"] = voluto;
    x.durata_s = 60.0;
    REQUIRE(v.m.intervieni(x).ok);
    v.avanza(5.0);
    CHECK(v.m.meteo_in({1000.0, 1000.0}).stato_mare() == voluto);
    CHECK(v.m.meteo_in({50000.0, 50000.0}).stato_mare() != voluto);   // fuori dalla zona il teatro
    v.avanza(70.0);
    CHECK(v.m.meteo_in({1000.0, 1000.0}).stato_mare() != voluto);
}

TEST_CASE("interventi: le ROE nuove valgono per chi le riceve; il sottomarino in profondita' le sa al broadcast") {
    Vetrino v;
    v.m.forze_enhanced(M688_DATI);
    v.avanza(2.0);
    REQUIRE(v.m.armi_libere_ia(3, 2) == std::optional<bool>(true));   // in guerra, sulla Perry
    Intervento x;
    x.tipo = "roe";
    x.testi = {{"parte", "urss"}, {"categoria", "navi"}, {"regola", "trattenute"}};
    REQUIRE(v.m.intervieni(x).ok);
    v.avanza(120.0);
    CHECK(v.m.armi_libere_ia(3, 2) == std::optional<bool>(true));   // a 150 m non ha sentito
    v.m.trova(3)->quota = v.m.trova(3)->quota_ordinata = 5.0;        // a quota periscopica: il broadcast
    v.avanza(2.0);
    CHECK(v.m.armi_libere_ia(3, 2) == std::optional<bool>(false));
}

TEST_CASE("interventi: dati prima del primo passo, anche senza il catalogo acustico; si applicano quando c'e'") {
    Missione m(scena(), 1, false);
    m.senza_giocatore(true);
    Intervento x;
    x.tipo = "togli";
    x.unita = 3;
    x.tempo = 5.0;
    REQUIRE(m.intervieni(x).ok);   // prima del primo passo, senza l'acustica: accettato
    m.acustica_enhanced(M688_DATI);
    for (int i = 0; i < 9 * 10; ++i) m.avanza(1.0 / 9.0);
    CHECK(m.unita().count(3) == 0);
    CHECK(m.stati_interventi().front().applicato);
    // nel classico (senza catalogo) all'applicazione fallisce, col motivo
    Missione c(scena(), 1, false);
    c.senza_giocatore(true);
    REQUIRE(c.intervieni(x).ok);
    for (int i = 0; i < 9 * 10; ++i) c.avanza(1.0 / 9.0);
    CHECK(c.stati_interventi().front().fallito);
    CHECK(c.unita().count(3) == 1);
}

TEST_CASE("interventi a t=0: forze, allerta e mare aspettano che i sistemi ci siano; senza, falliscono col motivo") {
    const auto dati = [](Missione& m) {
        const auto x = [&](const char* tipo, std::map<std::string, std::string> testi, std::map<std::string, double> par = {}, int unita = -1) {
            Intervento i;
            i.tempo = 0.0;
            i.tipo = tipo;
            i.testi = std::move(testi);
            i.parametri = std::move(par);
            i.unita = unita;
            REQUIRE(m.intervieni(i).ok);
        };
        x("situazione", {{"parte_a", "usa"}, {"parte_b", "urss"}, {"situazione", "guerra"}});
        x("allerta", {}, {{"livello", 3.0}}, 3);
        x("meteo", {}, {{"mare", 6.0}});
    };
    Vetrino v;
    dati(v.m);   // prima del primo passo, come il file dell'esperimento: forze e meteo li crea DanniMissione alla fine del passo
    v.avanza(1.0);
    for (const auto& s : v.m.stati_interventi()) {
        CAPTURE(s.id);
        CHECK(s.applicato);
        CHECK_FALSE(s.fallito);
        CHECK(s.inizio <= 0.5);   // al primo passo in cui le forze ci sono
    }
    CHECK(v.m.situazione_fra(0x08, 0x10) == Situazione::guerra);
    CHECK(v.m.allerta(0x10) == std::optional<int>(3));
    v.avanza(14.0);   // il mare di chi sta li' si rilegge ogni tanto (le celle del meteo, 10 s)
    CHECK(v.m.stato_mare() == 6);
    // le forze spente non arrivano mai: fallisce subito, le altre vanno avanti
    Vetrino spente;
    spente.m.usa_forze(false);
    dati(spente.m);
    spente.avanza(1.0);
    REQUIRE(spente.m.stati_interventi().size() == 3);
    CHECK(spente.m.stati_interventi()[0].fallito);
    CHECK(spente.m.stati_interventi()[1].fallito);
    CHECK_FALSE(spente.m.stati_interventi()[0].motivo_it.empty());
    CHECK(spente.m.stati_interventi()[2].applicato);
    // senza danni della missione (e quindi senza forze), passata l'attesa, fallisce col motivo
    Missione sola(scena(), 1, false);
    sola.acustica_enhanced(M688_DATI);
    sola.senza_giocatore(true);
    dati(sola);
    for (int i = 0; i < 9; ++i) sola.avanza(1.0 / 9.0);
    CHECK_FALSE(sola.stati_interventi()[0].fallito);   // nell'attesa
    CHECK_FALSE(sola.stati_interventi()[0].applicato);
    for (int i = 0; i < 2 * 9; ++i) sola.avanza(1.0 / 9.0);
    CHECK(sola.stati_interventi()[0].fallito);
}

TEST_CASE("interventi su evento (D2): l'ingresso in una zona e un evento del registro; l'innesco sconosciuto no") {
    Vetrino v;
    v.avanza(2.0);
    // la Perry (2) va a nord a 10 nodi da (0, 0): quando entra nel cerchio di 300 m attorno a (0, 1500) il Victor si toglie
    Intervento z;
    z.tipo = "togli";
    z.unita = 3;
    z.innesco = Innesco{"zona", 2, -1, Vec2{0.0, 1500.0}, 300.0};
    const auto ez = v.m.intervieni(z);
    REQUIRE(ez.ok);
    CHECK(v.m.interventi().back().riga_it.find("entra in una zona") != std::string::npos);
    v.avanza(60.0);
    CHECK(v.m.unita().count(3) == 1);   // ancora lontana
    v.avanza(240.0);
    CHECK(v.m.unita().count(3) == 0);
    // la soffiata d'emergenza del giocatore: l'evento nel registro (anche per l'audio) e l'innesco che lo aspetta
    Intervento o;
    o.tipo = "ordine";
    o.unita = 2;
    o.parametri["rotta"] = 270.0;
    o.innesco = Innesco{"emersione_emergenza", 0, -1, std::nullopt, 0.0};
    REQUIRE(v.m.intervieni(o).ok);
    v.avanza(5.0);
    CHECK_FALSE(v.m.stati_interventi().back().applicato);
    v.m.applica(Ordine{v.m.tempo(), TipoOrdine::emersione_emergenza});
    v.avanza(3.0);
    CHECK(v.m.stati_interventi().back().applicato);
    CHECK(v.m.unita().at(2).rotta_ordinata == doctest::Approx(270.0));
    const auto& ev = v.m.eventi_missione();
    CHECK(std::any_of(ev.begin(), ev.end(), [](const EventoMissione& e) {
        return e.tipo == TipoEvento::emersione_emergenza && e.unita == 0 && e.testo_it.find("aria alle casse") != std::string::npos;
    }));
    // l'innesco sconosciuto, o la zona senza raggio
    Intervento x;
    x.tipo = "togli";
    x.unita = 2;
    x.innesco = Innesco{"inventato", -1, -1, std::nullopt, 0.0};
    CHECK_FALSE(v.m.intervieni(x).ok);
    x.innesco = Innesco{"zona", 2, -1, Vec2{0.0, 0.0}, 0.0};
    CHECK_FALSE(v.m.intervieni(x).ok);
    CHECK(catalogo_inneschi().size() >= 10);
}

// D3, il falso contatto acustico (docs/28 §3.5; nota d3-contatto-falso-contratto.md): un 688 dell'IA sente un «Victor III»
// che non c'e' e il suo sonarista lo prende per vero (errata); con la firma rovinata prima o poi lo smaschera
namespace {
std::string scena_falso() {
    const auto f = fs::temp_directory_path() / "m688_prova_falso_contatto.lua";
    std::ofstream(f) << "return { id = 'PROVA_FALSO', teatro = 'faroe', lato = 'usa', data = '1989-06-15', ora_utc = 12, paesaggio_sonoro = false,"
                        " carriera = { situazione = 'crisi' }, script = {}, varianti = { { probabilita = 1, unita = {\n"
                        "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true, posizione = { x = -90000, y = -80000 },"
                        "  quota_m = 300, rotta = 90, velocita_kn = 3, " << SUB << " },\n"
                        "{ id = 2, classe = 'perry', categoria = 'nave', flag = 10, posizione = { x = 0, y = 0 }, quota_m = 0, rotta = 0, velocita_kn = 5, " << NAVE << " },\n"
                        "{ id = 4, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, posizione = { x = 30000, y = 0 }, quota_m = 150, rotta = 0, velocita_kn = 4, " << SUB << " },\n"
                        "}, timer = {} } } }\n";
    return f.string();
}
std::optional<Sorgente> sorgente_di(const Missione& m, int ascoltatore, int id) {   // come la sente chi ascolta
    for (const auto& s : m.scena_acustica(ascoltatore, "").sorgenti)
        if (s.id == id) return s;
    return std::nullopt;
}
int falso_di(const Missione& m) {
    for (const auto& [id, u] : m.unita())
        if (m.contatto_falso(id)) return id;
    return -1;
}
}  // namespace

TEST_CASE("interventi D3: il falso contatto, preso per vero; la firma perfetta non si smaschera ascoltando; l'eco del ping si'") {
    const auto f = scena_falso();
    for (const std::string classe : {"victor3", "alfa"}) {
        CAPTURE(classe);
        Vetrino v(true, f);
        REQUIRE(voce_intervento("contatto_falso") != nullptr);
        CHECK(voce_intervento("contatto_falso")->gruppo == "INGANNO");
        Intervento x;
        x.tipo = "contatto_falso";
        x.punto = Vec2{30000.0, 4000.0};   // davanti al 688 dell'IA (4), lontano dalla Perry
        x.testi = {{"classe", classe}, {"parte", "urss"}};
        x.parametri = {{"rotta", 90.0}, {"velocita_kn", classe == "alfa" ? 20.0 : 8.0}, {"quota_m", 100.0}, {"fedelta", 1.0}};
        x.durata_s = 1800.0;
        REQUIRE(v.m.intervieni(x).ok);
        v.avanza(1.0);
        const int id = falso_di(v.m);
        REQUIRE(id >= 990000);
        CHECK(id <= 994999);
        const auto& u = v.m.unita().at(id);
        CHECK(u.classe == classe);
        CHECK((u.flag & 0x80) != 0);      // falso bersaglio: nessun colpo
        CHECK((u.flag & 0x18) == 0x10);   // la parte creduta
        // la sorgente ha la firma della classe, la sua categoria e l'eco puntiforme di un'esca
        const auto s = sorgente_di(v.m, 4, id);
        REQUIRE(s.has_value());
        CHECK(s->esca);
        CHECK(s->fedelta == doctest::Approx(1.0));
        CHECK_FALSE(s->nave);
        REQUIRE(s->firma != nullptr);
        CHECK(s->firma->tipo_unita == "ssn");
        // il sonarista del 688 lo classifica come un contatto vero, sbagliando (come un Victor vero, a 3,7 km dal 03/11/2026
        // non piu' NONSUB: le righe del cavo della schiera sono mascherate); ascoltando, la firma perfetta non lo tradisce
        const Missione::ClassificazioneIa* presa = nullptr;
        for (int k = 0; k < 120 && !presa; ++k) {
            v.avanza(5.0);
            for (const auto& c : v.m.classificazioni_ia())
                if (c.unita == 4 && c.sorgente == id) presa = &c;
        }
        REQUIRE(presa != nullptr);
        CHECK(presa->errata);
        const double t_presa = presa->tempo_s;
        MESSAGE(classe, ": preso per vero a ", t_presa, " s, '", presa->classe, "', confidenza ", static_cast<int>(presa->confidenza));
        v.avanza(600.0);
        bool smascherato = false;
        for (const auto& c : v.m.classificazioni_ia())
            if (c.unita == 4 && c.sorgente == id && !c.errata) smascherato = true;
        CHECK_FALSE(smascherato);
        // il ping del 688: l'eco e' quella di un'esca, troppo piccola per qualunque battello; il sonarista lo smaschera
        // subito, comunque l'avesse classificato (nucleo/sonarista, dal 03/11/2026 anche i NONSUB senza classe)
        v.m.impulso_sonar_enhanced(4);
        v.avanza(3.0);
        for (const auto& c : v.m.classificazioni_ia())
            if (c.unita == 4 && c.sorgente == id && c.tempo_s > t_presa && c.classe.empty() && c.confidenza == Confidenza::nonsub && !c.errata)
                smascherato = true;
        CHECK(smascherato);
        // alla fine della durata sparisce
        v.avanza(1200.0);
        CHECK(falso_di(v.m) == -1);
        CHECK((v.m.unita().count(id) == 0 || v.m.unita().at(id).persa));
    }
    fs::remove(f);
}

TEST_CASE("interventi D3: il falso contatto muto o che pinga; la fedelta' passa alla sorgente; un falso mercantile resta nave") {
    const auto f = scena_falso();
    Vetrino v(true, f);
    Intervento x;
    x.tipo = "contatto_falso";
    x.punto = Vec2{0.0, 6000.0};
    x.testi = {{"classe", "mercantile"}};
    x.parametri = {{"velocita_kn", 12.0}, {"fedelta", 0.5}, {"ping_s", 0.0}};
    REQUIRE(v.m.intervieni(x).ok);
    x.testi = {{"classe", "victor3"}, {"parte", "urss"}};
    x.punto = Vec2{3000.0, 3000.0};
    x.parametri = {{"velocita_kn", 6.0}, {"quota_m", 150.0}, {"ping_s", 30.0}};
    REQUIRE(v.m.intervieni(x).ok);
    v.avanza(1.0);
    std::vector<int> falsi;
    for (const auto& [id, u] : v.m.unita()) if (v.m.contatto_falso(id)) falsi.push_back(id);
    REQUIRE(falsi.size() == 2);
    const auto& mercantile = v.m.unita().at(falsi[0]).classe == "mercantile" ? v.m.unita().at(falsi[0]) : v.m.unita().at(falsi[1]);
    const auto s = sorgente_di(v.m, 2, mercantile.id);
    REQUIRE(s.has_value());
    CHECK(s->nave);
    CHECK(s->fedelta == doctest::Approx(0.5));
    CHECK(mercantile.quota >= 2.0);   // mai in superficie: vista e radar non lo colgono
    CHECK((mercantile.flag & 0x18) == 0);   // senza parte: neutrale
    // il «Victor» pinga ogni 30 s: la Perry lo sente col rilevamento (l'impulso di sola andata)
    const auto& victor = v.m.unita().at(falsi[0]).classe == "victor3" ? v.m.unita().at(falsi[0]) : v.m.unita().at(falsi[1]);
    v.avanza(40.0);
    const auto& perry = v.m.unita().at(2);
    CHECK(perry.contatti.count(victor.id) == 1);
    // la classe sconosciuta si rifiuta all'ingresso
    x.testi = {{"classe", "nessuna"}};
    CHECK_FALSE(v.m.intervieni(x).ok);
    // la fascia d'id non si sovrappone ai transitori dell'artiglieria (995000-999990)
    for (const int id : falsi) CHECK(id < 995000);
    fs::remove(f);
}


// D3, il disturbo della radio di una parte (docs/28 §3.4): la Perry 2 sente un Victor e lo riferisce alla Sacramento 5
// della sua formazione (12 km). Col disturbo pieno sulla parte americana il rapporto non arriva (e il messaggio e' perso
// per lo spettatore); coi ritardi per 5 arriva piu' tardi; senza intervento come prima
namespace {
std::string scena_radio() {
    const auto f = fs::temp_directory_path() / "m688_prova_disturbo_radio.lua";
    const std::string SORDA = "modello = { massima_kn = 20, virata_gradi_s = 2, accel_kn_s = 0.5, quota_m_s = 1, quota_max_m = 0, integrita = 100, rumore = 10, sonar = 0 }";
    std::ofstream(f) << "return { id = 'PROVA_DISTURBO', teatro = 'faroe', lato = 'usa', data = '1989-06-15', ora_utc = 12, paesaggio_sonoro = false,"
                        " carriera = { situazione = 'crisi' }, script = {}, varianti = { { probabilita = 1, unita = {\n"
                        "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true, posizione = { x = 0, y = -80000 },"
                        "  quota_m = 300, rotta = 90, velocita_kn = 3, " << SUB << " },\n"
                        "{ id = 2, classe = 'perry', categoria = 'nave', flag = 10, posizione = { x = 0, y = 0 }, quota_m = 0, rotta = 90, velocita_kn = 5, " << NAVE << " },\n"
                        "{ id = 3, classe = 'victor3', categoria = 'sottomarino', flag = 17, posizione = { x = 0, y = 6000 }, quota_m = 40, rotta = 90, velocita_kn = 18, " << SUB << " },\n"
                        "{ id = 5, classe = 'sacramento', categoria = 'nave', flag = 10, posizione = { x = 0, y = -12000 }, quota_m = 0, rotta = 90, velocita_kn = 5, " << SORDA << " },\n"
                        "}, timer = {} } } }\n";
    return f.string();
}
}  // namespace

TEST_CASE("interventi D3: il disturbo radio di una parte perde e ritarda i rapporti della formazione") {
    REQUIRE(voce_intervento("disturbo_radio") != nullptr);
    CHECK(voce_intervento("disturbo_radio")->gruppo == "COMUNICAZIONI");
    const auto f = scena_radio();
    // il primo istante in cui la Sacramento ha il rapporto sul Victor (-1: mai in `secondi`), e i messaggi persi
    const auto arrivo = [&](std::optional<Intervento> x, double secondi, int* persi = nullptr) {
        Vetrino v(true, f);
        if (x) REQUIRE(v.m.intervieni(*x).ok);
        double t = -1.0;
        for (int s = 0; s < static_cast<int>(secondi) && t < 0.0; ++s) {
            v.avanza(1.0);
            if (v.m.contatto_radio(5, 3)) t = v.m.tempo();
        }
        if (persi) {
            *persi = 0;
            for (const auto& m : v.m.messaggi_ia()) if (m.perso && m.da == 2) ++*persi;
        }
        return t;
    };
    const double libera = arrivo(std::nullopt, 900.0);
    REQUIRE(libera > 0.0);
    Intervento x;
    x.tipo = "disturbo_radio";
    x.testi = {{"parte", "usa"}};
    x.parametri = {{"perdita", 1.0}, {"ritardo", 1.0}};
    int persi = 0;
    CHECK(arrivo(x, 900.0, &persi) < 0.0);   // nessun rapporto passa
    CHECK(persi >= 1);
    {   // lo spettatore li vede: un evento per ogni rapporto perso nel disturbo; senza disturbo nessuno
        const auto persi_nel_registro = [&](std::optional<Intervento> i) {
            Vetrino v(true, f);
            if (i) REQUIRE(v.m.intervieni(*i).ok);
            v.avanza(600.0);
            int n = 0;
            for (const auto& e : v.m.eventi_missione())
                if (e.tipo == TipoEvento::reazione_ia && e.classe_arma == "messaggio_perso") {
                    ++n;
                    CHECK(e.testo_it.find("disturbo radio") != std::string::npos);
                    CHECK_FALSE(e.noto);
                }
            return n;
        };
        CHECK(persi_nel_registro(x) >= 1);
        CHECK(persi_nel_registro(std::nullopt) == 0);
    }
    x.parametri = {{"perdita", 0.0}, {"ritardo", 5.0}};
    const double lenta = arrivo(x, 2400.0);
    MESSAGE("il rapporto alla Sacramento: libero a ", libera, " s, coi ritardi per 5 a ", lenta, " s");
    CHECK(lenta > libera);
    x.testi = {{"parte", "urss"}};   // il disturbo sull'altra parte non tocca la radio americana
    x.parametri = {{"perdita", 1.0}};
    CHECK(arrivo(x, 900.0) == doctest::Approx(libera));
    // fuori dalla zona del disturbo nemmeno
    x.testi = {{"parte", "usa"}};
    x.punto = Vec2{100000.0, 100000.0};
    x.raggio_m = 5000.0;
    CHECK(arrivo(x, 900.0) == doctest::Approx(libera));
    fs::remove(f);
}

// D3, il falso ordine radio (docs/28 §3.4; nota runtime-d3-falsi-ordini-proposta.md): otto Perry americane in linea
// verso nord e una Krivak 20 km avanti. Il falso «rientro» lo credono quasi tutte le reclute e meno degli esperti; chi
// dubita chiede conferma, trasmette, e la Krivak la localizza (radiogoniometria); chi ci crede torna indietro
namespace {
std::string scena_ordini(const std::string& situazione) {
    const auto f = fs::temp_directory_path() / ("m688_prova_ordini_falsi_" + situazione + ".lua");
    std::ofstream o(f);
    o << "return { id = 'PROVA_ORDINI', teatro = 'faroe', lato = 'usa', data = '1989-06-15', ora_utc = 12, paesaggio_sonoro = false,"
         " carriera = { situazione = '" << situazione << "' }, script = {}, varianti = { { probabilita = 1, unita = {\n"
         "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true, posizione = { x = 0, y = -80000 },"
         "  quota_m = 300, rotta = 90, velocita_kn = 3, " << SUB << " },\n";
    for (int i = 0; i < 8; ++i)
        o << "{ id = " << 10 + i << ", classe = 'perry', categoria = 'nave', flag = 10, posizione = { x = " << 2000 * i
          << ", y = 0 }, quota_m = 0, rotta = 0, velocita_kn = 5, " << NAVE << " },\n";
    o << "{ id = 3, classe = 'krivak2', categoria = 'nave', flag = 18, posizione = { x = 7000, y = 20000 }, quota_m = 0, rotta = 90, velocita_kn = 3, " << NAVE << " },\n"
         "}, timer = {} } } }\n";
    return f.string();
}
struct EsitoOrdini {
    int creduti = 0, ignorati = 0, verifiche = 0, smascherati = 0;
    bool localizzati = false;
    double via_creduti = 0.0, via_altri = 0.0;   // di quanto si sono allontanate dalla Krivak, in media
};
EsitoOrdini ordine_falso(Esperienza livello, const std::string& ordine, const std::string& situazione, double autenticazione, double secondi) {
    const auto f = scena_ordini(situazione);
    Vetrino v(true, f);
    Missione::RegolazioniParte r;
    r.livello = livello;
    v.m.regolazioni({{0x08, r}});
    Intervento x;
    x.tipo = "ordine_falso";
    x.testi = {{"parte", "usa"}, {"ordine", ordine}};
    x.parametri = {{"autenticazione", autenticazione}};
    x.durata_s = 3600.0;
    REQUIRE(v.m.intervieni(x).ok);
    std::map<int, double> prima;
    for (int i = 0; i < 8; ++i) prima[10 + i] = distanza(v.m.unita().at(10 + i).pos, v.m.unita().at(3).pos);
    EsitoOrdini e;
    v.avanza(secondi);
    std::set<int> creduti;
    for (const auto& q : v.m.reazioni_ia()) {
        if (q.evento == "radiogoniometria" && q.unita == 3 && q.causa >= 10 && q.causa < 18) e.localizzati = true;
        if (q.evento != "ordine_falso") continue;
        if (q.reazione == "ordine_creduto") ++e.creduti, creduti.insert(q.unita);
        if (q.reazione == "ordine_ignorato") ++e.ignorati;
        if (q.reazione == "ordine_verifica") ++e.verifiche;
        if (q.reazione == "ordine_smascherato") ++e.smascherati;
    }
    int nc = 0, na = 0;
    for (int i = 0; i < 8; ++i) {
        const double d = distanza(v.m.unita().at(10 + i).pos, v.m.unita().at(3).pos) - prima[10 + i];
        if (creduti.count(10 + i)) e.via_creduti += d, ++nc;
        else e.via_altri += d, ++na;
    }
    if (nc) e.via_creduti /= nc;
    if (na) e.via_altri /= na;
    fs::remove(f);
    return e;
}
}  // namespace

TEST_CASE("interventi D3: il falso ordine di rientro, creduto secondo il livello; la verifica si fa localizzare") {
    REQUIRE(voce_intervento("ordine_falso") != nullptr);
    CHECK(voce_intervento("ordine_falso")->gruppo == "COMUNICAZIONI");
    const EsitoOrdini reclute = ordine_falso(Esperienza::recluta, "rientro", "crisi", 0.5, 900.0);
    const EsitoOrdini esperti = ordine_falso(Esperienza::esperto, "rientro", "crisi", 0.5, 900.0);
    MESSAGE("reclute: credono ", reclute.creduti, ", ignorano ", reclute.ignorati, ", verificano ", reclute.verifiche,
            "; esperti: credono ", esperti.creduti, ", ignorano ", esperti.ignorati, ", verificano ", esperti.verifiche,
            ", smascherati ", esperti.smascherati, ", localizzati ", esperti.localizzati);
    CHECK(reclute.creduti + reclute.ignorati + reclute.verifiche >= 8);   // tutte le navi l'hanno ricevuto
    CHECK(reclute.creduti > esperti.creduti);
    CHECK(esperti.verifiche >= 1);
    CHECK(esperti.smascherati >= 1);   // la risposta del comando arriva in 30-120 s
    CHECK(esperti.localizzati);        // la Krivak e' l'ESM nemico vicino: la richiesta in UHF la tradisce
    MESSAGE("allontanamento dalla Krivak: chi crede ", reclute.via_creduti, " m, gli altri ", reclute.via_altri, " m");
    CHECK(reclute.via_creduti > 2000.0);   // tornano indietro a 15 nodi
    CHECK(reclute.via_altri < reclute.via_creduti);
}

// una riserva parcheggiata fuori dalla carta (scenario_missione.cpp, parcheggio) non e' ancora in scena: l'ordine falso le
// arriva quando entra, se dura ancora. Nella S5 l'SH-60B di riserva «chiedeva conferma» a 120 s, da 2200 km (trailer-1, 04/11/2026)
TEST_CASE("interventi D3: una riserva non ancora in scena non riceve l'ordine falso") {
    const auto f = fs::temp_directory_path() / "m688_prova_ordine_riserva.lua";
    {
        std::ofstream o(f);
        o << "return { id = 'PROVA_RISERVA', teatro = 'faroe', lato = 'usa', data = '1989-06-15', ora_utc = 12, paesaggio_sonoro = false,"
             " carriera = { situazione = 'crisi' },"
             " script = { ENTRA = function(u) m688.posizione(u, 2000, 0) m688.salva_modello(u) end }, varianti = { { probabilita = 1, unita = {\n"
             "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true, posizione = { x = 0, y = -80000 },"
             "  quota_m = 300, rotta = 90, velocita_kn = 3, " << SUB << " },\n"
             "{ id = 10, classe = 'perry', categoria = 'nave', flag = 10, posizione = { x = 0, y = 0 }, quota_m = 0, rotta = 0, velocita_kn = 5, " << NAVE << " },\n"
             "{ id = 11, classe = 'perry', categoria = 'nave', flag = 10, posizione = { x = 0, y = 2220000 }, quota_m = 0, rotta = 0, velocita_kn = 0, " << NAVE << " },\n"
             "}, timer = { { dopo_s = 300, unita = 11, gestore = 'ENTRA' } } } } }\n";
    }
    Vetrino v(true, f.string());
    Intervento x;
    x.tipo = "ordine_falso";
    x.testi = {{"parte", "usa"}, {"ordine", "rientro"}};
    x.parametri = {{"autenticazione", 1.0}};
    REQUIRE(v.m.intervieni(x).ok);
    const auto ricevuto = [&](int id) {
        for (const auto& q : v.m.reazioni_ia())
            if (q.evento == "ordine_falso" && q.unita == id) return true;
        return false;
    };
    v.avanza(200.0);
    CHECK(ricevuto(10));
    CHECK_FALSE(ricevuto(11));
    v.avanza(200.0);   // entrata a 300 s: l'ordine dura ancora
    CHECK(distanza(v.m.unita().at(11).pos, {2000.0, 0.0}) < 2000.0);
    CHECK(ricevuto(11));
    fs::remove(f);
}

TEST_CASE("interventi D3: la rete strategica localizza chi chiede conferma: nel registro, senza unita'; i testi dell'ordine falso") {
    // otto Krivak sovietiche esperte: la raffica HF della verifica la sente la rete nemica, non un'unita' (unita' -1)
    const auto f = fs::temp_directory_path() / "m688_prova_rete_df.lua";
    {
        std::ofstream o(f);
        o << "return { id = 'PROVA_RETE', teatro = 'faroe', lato = 'usa', data = '1989-06-15', ora_utc = 12, paesaggio_sonoro = false,"
             " carriera = { situazione = 'crisi' }, script = {}, varianti = { { probabilita = 1, unita = {\n"
             "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true, posizione = { x = 0, y = -80000 },"
             "  quota_m = 300, rotta = 90, velocita_kn = 3, " << SUB << " },\n";
        for (int i = 0; i < 8; ++i)
            o << "{ id = " << 10 + i << ", classe = 'krivak2', categoria = 'nave', flag = 18, posizione = { x = " << 2000 * i
              << ", y = 0 }, quota_m = 0, rotta = 0, velocita_kn = 5, " << NAVE << " },\n";
        o << "}, timer = {} } } }\n";
    }
    Vetrino v(true, f.string());
    Missione::RegolazioniParte r;
    r.livello = Esperienza::esperto;
    v.m.regolazioni({{0x10, r}});
    Intervento x;
    x.tipo = "ordine_falso";
    x.testi = {{"parte", "urss"}, {"ordine", "rientro"}};
    x.parametri = {{"autenticazione", 0.0}};
    x.durata_s = 3600.0;
    REQUIRE(v.m.intervieni(x).ok);
    v.avanza(900.0);
    int rete = 0, ordini = 0;
    for (const auto& e : v.m.eventi_missione()) {
        if (e.tipo != TipoEvento::reazione_ia) continue;
        if (e.classe_arma == "localizza" && e.unita < 0) {
            ++rete;
            CHECK(e.testo_it.find("rete strategica") != std::string::npos);
            CHECK(e.altra >= 10);
            CHECK_FALSE(e.noto);
        }
        if (e.classe_arma.rfind("ordine_", 0) == 0) {
            ++ordini;
            CHECK(e.testo_it.find("ordine falso") != std::string::npos);
        }
    }
    MESSAGE("localizzazioni della rete: ", rete, ", reazioni all'ordine: ", ordini);
    CHECK(rete >= 1);
    CHECK(ordini >= 8);
    fs::remove(f);
}

TEST_CASE("interventi D3: il falso ordine di armi libere in pace, a chi ci crede; l'ordine perfetto e quello scadente") {
    const auto f = scena_ordini("pace");
    Vetrino v(true, f);
    Missione::RegolazioniParte r;
    r.livello = Esperienza::recluta;
    v.m.regolazioni({{0x08, r}});
    Intervento x;
    x.tipo = "ordine_falso";
    x.testi = {{"parte", "usa"}, {"ordine", "roe_libere"}};
    x.parametri = {{"autenticazione", 1.0}};
    REQUIRE(v.m.intervieni(x).ok);
    v.avanza(120.0);
    std::set<int> creduti;
    for (const auto& q : v.m.reazioni_ia())
        if (q.evento == "ordine_falso" && q.reazione == "ordine_creduto") creduti.insert(q.unita);
    int libere = 0;
    for (int i = 0; i < 8; ++i)
        if (v.m.armi_libere_ia(10 + i, 3).value_or(false)) {
            ++libere;
            CHECK(creduti.count(10 + i) == 1);   // libera solo chi ci crede
        }
    MESSAGE("in pace, col falso ordine di armi libere: ", creduti.size(), " Perry ci credono, ", libere,
            " libere sulla Krivak (le altre non sanno ancora che e' sovietica)");
    CHECK(creduti.size() >= 5);   // reclute, ordine perfetto: dubita il 10% (x 0,5 per l'autenticazione, x 2 contro la pace)
    CHECK(libere >= 1);
    // senza ordine nessuna
    Vetrino w(true, f);
    w.m.regolazioni({{0x08, r}});
    w.avanza(120.0);
    for (int i = 0; i < 8; ++i) CHECK_FALSE(w.m.armi_libere_ia(10 + i, 3).value_or(false));
    fs::remove(f);
}

// D3, gli incidenti provocati e la spia (docs/28 §3.5), nella scena delle otto Perry e della Krivak, in crisi
TEST_CASE("interventi D3: lo scoppio senza arma sveglia la Krivak; il siluro di nessuno, a segno, si attribuisce dalla firma") {
    const auto f = scena_ordini("crisi");
    {   // lo scoppio a 2 km dalla Krivak: si sente come un'arma, la formazione va ad A3, nessuno e' in guerra
        Vetrino v(true, f);
        v.avanza(5.0);
        const int prima = v.m.allerta_unita(3).value_or(-1);
        Intervento x;
        x.tipo = "incidente";
        x.punto = Vec2{7000.0, 18000.0};
        x.testi = {{"incidente", "scoppio"}};
        x.parametri = {{"kg", 300.0}, {"quota_m", 40.0}};
        REQUIRE(v.m.intervieni(x).ok);
        v.avanza(2.0);
        CHECK(v.m.stati_interventi().back().applicato);
        bool sentito = false;
        for (const auto& s : v.m.scena_acustica(3, "").sorgenti) sentito = sentito || (s.id >= 980000 && s.id <= 989999);
        CHECK(sentito);
        MESSAGE("allerta della Krivak: prima ", prima, ", dopo lo scoppio ", v.m.allerta_unita(3).value_or(-1));
        CHECK(v.m.allerta_unita(3).value_or(-1) == 3);
        CHECK(v.m.situazione_fra(0x08, 0x10) == Situazione::crisi);
    }
    {   // il siluro «sovietico» di nessuno sulla Perry 10, da 3 km: corre col modello, la Perry scappa o e' colpita. Non c'e'
        // chi l'ha lanciato: se colpisce, gli americani deducono il colpevole dalla firma dell'arma (D4) e la crisi con
        // l'URSS diventa guerra; se no resta crisi
        Vetrino v(true, f);
        v.avanza(5.0);
        Intervento x;
        x.tipo = "incidente";
        x.unita = 10;
        x.punto = Vec2{3000.0, 0.0};
        x.testi = {{"incidente", "siluro"}, {"parte", "urss"}};
        x.parametri = {{"quota_m", 20.0}};
        REQUIRE(v.m.intervieni(x).ok);
        v.avanza(1.0);
        int siluro = -1;
        for (const auto& [id, u] : v.m.unita())
            if (u.categoria == "arma" && u.lanciatore == -1 && (u.flag & 0x20)) siluro = id;
        REQUIRE(siluro >= 0);
        CHECK(v.m.siluro(siluro) != nullptr);
        CHECK((v.m.unita().at(siluro).flag & 0x18) == 0x10);
        v.avanza(300.0);
        bool colpita = false, scappa = false;
        for (const auto& c : v.m.colpi()) colpita = colpita || c.vittima == 10;
        for (const auto& r : v.m.ritirate_ia()) scappa = scappa || r.unita == 10;
        MESSAGE("la Perry 10 col siluro di nessuno: colpita ", colpita, ", scappa ", scappa);
        CHECK((colpita || scappa));
        CHECK(v.m.situazione_fra(0x08, 0x10) == (colpita ? Situazione::guerra : Situazione::crisi));
    }
    fs::remove(f);
}

TEST_CASE("interventi D3: la spia da' alla Krivak il quadro vero delle Perry, finche' dura") {
    const auto f = scena_ordini("crisi");
    Vetrino v(true, f);
    Intervento x;
    x.tipo = "spia";
    x.testi = {{"parte", "urss"}, {"altra", "usa"}};
    x.durata_s = 300.0;
    REQUIRE(v.m.intervieni(x).ok);
    v.avanza(5.0);
    const auto c = v.m.contatto_radio(3, 17);   // la Perry piu' lontana, a 20 km
    REQUIRE(c.has_value());
    REQUIRE(c->r.pos.has_value());
    CHECK(distanza(*c->r.pos, v.m.unita().at(17).pos) < 100.0);
    CHECK(c->r.classe == "perry");
    v.avanza(900.0);   // finita la spia, il quadro invecchia: l'ultimo rapporto e' di quando finiva
    const auto d = v.m.contatto_radio(3, 17);
    if (d) CHECK(d->r.t_fix <= 310.0);
    // senza la spia la Krivak, sola, non ha rapporti di nessuno
    Vetrino w(true, f);
    w.avanza(5.0);
    CHECK_FALSE(w.m.contatto_radio(3, 17).has_value());
    fs::remove(f);
}

// D4, la false flag (docs/28 §3.5): un siluro con la parte sovietica, lanciato in realta' da un'unita' americana, colpisce
// la Perry. Se nessuno ha sentito o visto chi l'ha lanciato, gli americani credono la firma dell'arma: la crisi con l'URSS
// diventa guerra, con la parte sbagliata. Se l'attaccante e' una nave in vista, lo sanno dei loro: fuoco amico, nessuna guerra
namespace {
std::string scena_false_flag(bool attaccante_in_vista) {
    const auto f = fs::temp_directory_path() / (attaccante_in_vista ? "m688_prova_ff_vista.lua" : "m688_prova_ff.lua");
    std::ofstream o(f);
    o << "return { id = 'PROVA_FF', teatro = 'faroe', lato = 'usa', data = '1989-06-15', ora_utc = 12, paesaggio_sonoro = false,"
         " carriera = { situazione = 'crisi' }, script = {}, varianti = { { probabilita = 1, unita = {\n"
         "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true, posizione = { x = 0, y = -80000 },"
         "  quota_m = 300, rotta = 90, velocita_kn = 3, " << SUB << " },\n"
         "{ id = 10, classe = 'perry', categoria = 'nave', flag = 10, posizione = { x = 0, y = 0 }, quota_m = 0, rotta = 90, velocita_kn = 8, " << NAVE << " },\n"
         "{ id = 3, classe = 'krivak2', categoria = 'nave', flag = 18, posizione = { x = 0, y = 20000 }, quota_m = 0, rotta = 90, velocita_kn = 3, " << NAVE << " },\n";
    if (attaccante_in_vista)
        o << "{ id = 5, classe = 'spruance', categoria = 'nave', flag = 10, posizione = { x = 0, y = -3000 }, quota_m = 0, rotta = 90, velocita_kn = 8, " << NAVE << " },\n";
    else
        // con la prua sulla Perry: il rilevamento della schiera e il suo specchio quasi coincidono, e il siluro va giusto
        o << "{ id = 5, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, posizione = { x = 0, y = -6000 }, quota_m = 150, rotta = 0, velocita_kn = 3, " << SUB << " },\n";
    o << "}, timer = {} } } }\n";
    return f.string();
}
struct EsitoFf { bool colpita = false; std::string attribuzione; Situazione situazione = Situazione::pace; };
EsitoFf false_flag(bool in_vista) {
    const auto f = scena_false_flag(in_vista);
    Vetrino v(true, f);
    v.avanza(30.0);   // il tempo di vedersi e di sentirsi
    Intervento x;
    x.tipo = "false_flag";
    x.unita = 5;
    x.testi = {{"parte", "urss"}, {"arma", "mk48"}};   // la parte dell'arma e' quella che si vede; il Mk 48 colpisce sicuro
    x.parametri = {{"bersaglio", 10.0}};
    REQUIRE(v.m.intervieni(x).ok);
    EsitoFf e;
    for (int s = 0; s < 600 && !e.colpita; ++s) {
        v.avanza(1.0);
        for (const auto& c : v.m.colpi()) e.colpita = e.colpita || c.vittima == 10;
    }
    v.avanza(5.0);
    for (const auto& q : v.m.reazioni_ia())
        if (q.evento == "false_flag") e.attribuzione = q.reazione;
    e.situazione = v.m.situazione_fra(0x08, 0x10);
    fs::remove(f);
    return e;
}
}  // namespace

TEST_CASE("interventi D4: la false flag; chi non ha visto l'attaccante dichiara la guerra alla parte sbagliata") {
    REQUIRE(voce_intervento("false_flag") != nullptr);
    const EsitoFf nascosto = false_flag(false);
    MESSAGE("attaccante non visto: colpita ", nascosto.colpita, ", ", nascosto.attribuzione, ", guerra ", nascosto.situazione == Situazione::guerra);
    REQUIRE(nascosto.colpita);
    CHECK(nascosto.attribuzione == "attribuisce_urss");
    CHECK(nascosto.situazione == Situazione::guerra);   // con la parte sbagliata
    const EsitoFf visto = false_flag(true);
    MESSAGE("attaccante in vista: colpita ", visto.colpita, ", ", visto.attribuzione, ", guerra ", visto.situazione == Situazione::guerra);
    REQUIRE(visto.colpita);
    CHECK(visto.attribuzione == "attribuisce_usa");    // sanno che e' dei loro
    CHECK(visto.situazione == Situazione::crisi);
}

// D4, il cambio di parte (docs/28 §3.1): la Spruance 5 passa ai sovietici. Lei e i sovietici lo sanno, la Perry 10 e la Perry 11
// no: per loro resta un'amica finche' non attacca; allora la vittima lo sa subito, la sua parte col rapporto della formazione
namespace {
std::string scena_cambio() {
    const auto f = fs::temp_directory_path() / "m688_prova_cambio_parte.lua";
    std::ofstream o(f);
    o << "return { id = 'PROVA_CAMBIO', teatro = 'faroe', lato = 'usa', data = '1989-06-15', ora_utc = 12, paesaggio_sonoro = false,"
         " carriera = { situazione = 'guerra' }, script = {}, varianti = { { probabilita = 1, unita = {\n"
         "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true, posizione = { x = 0, y = -80000 },"
         "  quota_m = 300, rotta = 90, velocita_kn = 3, " << SUB << " },\n"
         "{ id = 10, classe = 'perry', categoria = 'nave', flag = 10, posizione = { x = 0, y = 0 }, quota_m = 0, rotta = 90, velocita_kn = 8, " << NAVE << " },\n"
         "{ id = 11, classe = 'perry', categoria = 'nave', flag = 10, posizione = { x = 4000, y = 0 }, quota_m = 0, rotta = 90, velocita_kn = 8, " << NAVE << " },\n"
         "{ id = 5, classe = 'spruance', categoria = 'nave', flag = 10, posizione = { x = 0, y = -3000 }, quota_m = 0, rotta = 90, velocita_kn = 8, " << NAVE << " },\n"
         "{ id = 3, classe = 'krivak2', categoria = 'nave', flag = 18, posizione = { x = 0, y = 60000 }, quota_m = 0, rotta = 90, velocita_kn = 3, " << NAVE << " },\n"
         "}, timer = {} } } }\n";
    return f.string();
}
}  // namespace

TEST_CASE("interventi D4: il cambio di parte; chi non lo sa la crede amica finche' non attacca") {
    REQUIRE(voce_intervento("cambia_parte") != nullptr);
    CHECK(voce_intervento("cambia_parte")->gruppo == "FORZE");
    const auto f = scena_cambio();
    Vetrino v(true, f);
    Intervento x;
    x.tipo = "cambia_parte";
    x.unita = 5;
    x.testi = {{"parte", "urss"}};
    REQUIRE(v.m.intervieni(x).ok);
    v.avanza(2.0);
    REQUIRE(v.m.stati_interventi().back().applicato);
    CHECK((v.m.unita().at(5).flag & 0x18) == 0x10);   // la verita' e lo spettatore
    CHECK(v.m.parte_creduta(10, 5) == 0x08);          // la Perry la crede ancora americana
    CHECK(v.m.parte_creduta(3, 5) == 0x10);           // la parte nuova lo sa
    CHECK(v.m.parte_creduta(5, 10) == 0x08);          // lei sa chi e' la Perry
    CHECK_FALSE(v.m.armi_libere_ia(10, 5).value_or(true));   // in guerra, ma per la Perry e' un'amica
    CHECK(v.m.armi_libere_ia(5, 10).value_or(false));        // lei e' libera
    // attacca: la vittima lo sa subito, l'altra Perry dopo il rapporto
    Intervento y;
    y.tipo = "false_flag";
    y.unita = 5;
    y.testi = {{"parte", "urss"}, {"arma", "mk48"}};
    y.parametri = {{"bersaglio", 10.0}};
    REQUIRE(v.m.intervieni(y).ok);
    bool saputo = false;
    for (int s = 0; s < 600 && !saputo; ++s) {
        v.avanza(1.0);
        for (const auto& e : v.m.eventi_missione())
            saputo = saputo || (e.tipo == TipoEvento::reazione_ia && e.classe_arma == "cambio_saputo" && e.unita == 10 && e.altra == 5);
    }
    REQUIRE(saputo);
    CHECK(v.m.parte_creduta(10, 5) == 0x10);
    CHECK(v.m.parte_creduta(11, 5) == 0x08);   // non ancora: il rapporto non e' arrivato
    v.avanza(125.0);                            // la formazione: al piu' 120 s
    CHECK(v.m.parte_creduta(11, 5) == 0x10);
    // l'istante in cui lo sa tutta la parte, per lo spettatore: una volta sola, la nuova dal cambio, la vecchia dal rapporto
    int sa_usa = 0, sa_urss = 0;
    for (const auto& e : v.m.eventi_missione()) {
        if (e.tipo != TipoEvento::reazione_ia || e.unita != 5) continue;
        sa_usa += e.classe_arma == "cambio_saputo_usa";
        sa_urss += e.classe_arma == "cambio_saputo_urss";
    }
    CHECK(sa_usa == 1);
    CHECK(sa_urss == 1);
    fs::remove(f);
    // annunciato a tutti: lo sanno subito. Il battello del comandante e la parte che ha gia' non si cambiano; a t=0 aspetta le forze
    const auto g = scena_cambio();
    Vetrino w(true, g);
    x.tempo = 0.0;
    x.testi = {{"parte", "urss"}, {"annuncio", "tutti"}};
    REQUIRE(w.m.intervieni(x).ok);
    Intervento z = x;
    z.unita = 3;
    REQUIRE(w.m.intervieni(z).ok);
    z.unita = 0;
    REQUIRE(w.m.intervieni(z).ok);
    w.avanza(2.0);
    CHECK(w.m.stati_interventi()[0].applicato);
    CHECK(w.m.parte_creduta(10, 5) == 0x10);
    CHECK(w.m.stati_interventi()[1].fallito);
    CHECK(w.m.stati_interventi()[2].fallito);
    fs::remove(g);
}

namespace {
std::string scena_d4(const std::string& teatro, const std::string& data) {
    const auto f = fs::temp_directory_path() / "m688_prova_interventi_d4.lua";
    std::ofstream(f) << "return { id = 'PROVA_INTERVENTI_D4', teatro = '" << teatro << "', lato = 'usa', data = '" << data << "', ora_utc = 2, paesaggio_sonoro = false,"
                        " script = {}, varianti = { { probabilita = 1, unita = {\n"
                        "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true, posizione = { x = 0, y = 0 },"
                        "  quota_m = 5, rotta = 0, velocita_kn = 0, " << SUB << " },\n"
                        "{ id = 3, classe = 'victor3', categoria = 'sottomarino', flag = 17, posizione = { x = 500, y = 0 }, quota_m = 300, rotta = 0,"
                        "  velocita_kn = 0, " << SUB << " },\n"
                        "}, timer = {} } } }\n";
    return f.string();
}
Intervento ambiente(const std::string& tipo, double transizione_s, double durata_s) {
    Intervento x;
    x.tipo = tipo;
    x.parametri["transizione_s"] = transizione_s;
    x.durata_s = durata_s;
    return x;
}
}  // namespace

TEST_CASE("interventi D4: lo strato che si sposta nella zona e torna; le acque polari lo rifiutano") {
    Vetrino v(true, scena_d4("faroe", "1989-02-15"));
    v.m.meteo_enhanced(M688_DATI);
    v.avanza(2.0);
    const double prima = v.m.sld_in({0.0, 0.0});
    REQUIRE(prima > 150.0);
    Intervento x = ambiente("strato", 120.0, 600.0);
    x.testi["modifica"] = "sposta";
    CHECK_FALSE(v.m.intervieni(x).ok);   // senza la quota
    x.parametri["quota_m"] = 100.0;
    x.raggio_m = 2000.0;
    CHECK_FALSE(v.m.intervieni(x).ok);   // una zona senza il punto
    x.punto = Vec2{0.0, 0.0};
    REQUIRE(v.m.intervieni(x).ok);
    v.avanza(130.0);
    CHECK(v.m.sld_in({0.0, 0.0}) == doctest::Approx(100.0).epsilon(0.05));
    CHECK(v.m.sld_in({5000.0, 0.0}) == prima);   // fuori dalla zona
    v.avanza(480.0);
    CHECK(v.m.stati_interventi().back().finito);
    v.avanza(125.0);                     // il ritorno comincia allo scadere, con la stessa transizione
    CHECK(v.m.sld_in({0.0, 0.0}) == prima);
    // togliere il condotto non chiede altro
    Intervento t = ambiente("strato", 0.0, 0.0);
    t.testi["modifica"] = "togli";
    REQUIRE(v.m.intervieni(t).ok);
    v.avanza(1.0);
    CHECK(v.m.sld_in({50000.0, 0.0}) == 0.0);

    Vetrino polare(true, scena_d4("barents", "1989-01-15"));
    polare.m.meteo_enhanced(M688_DATI);
    polare.avanza(2.0);
    REQUIRE_FALSE(polare.m.strato_applicabile(ModificaStrato::Tipo::sposta));
    Intervento y = ambiente("strato", 60.0, 0.0);
    y.testi["modifica"] = "sposta";
    y.parametri["quota_m"] = 100.0;
    REQUIRE(polare.m.intervieni(y).ok);   // il teatro si guarda all'applicazione
    polare.avanza(1.0);
    CHECK(polare.m.stati_interventi().back().fallito);
    CHECK_FALSE(polare.m.stati_interventi().back().motivo_it.empty());
}

TEST_CASE("interventi D4: la deriva del pack cambia e torna; la polynya si chiude su chi e' a galla con un avviso, senza danni") {
    Vetrino v(true, scena_d4("faroe", "1989-02-15"));
    v.m.meteo_enhanced(M688_DATI);
    v.avanza(2.0);
    const Vec2 qui{0.0, 0.0};
    REQUIRE(v.m.copertura_ghiaccio(qui) == 0.0);
    const int integrita = v.m.trova(0)->integrita;

    Intervento d;
    d.tipo = "ghiaccio_deriva";
    d.parametri = {{"rotta", 90.0}, {"velocita_kn", 1.0}};
    d.durata_s = 100.0;
    REQUIRE(v.m.intervieni(d).ok);
    v.avanza(50.0);
    const CampoGhiaccio* g = v.m.ghiaccio();
    REQUIRE(g != nullptr);
    const double t = v.m.tempo();
    CHECK((g->spostamento(t + 10.0).x - g->spostamento(t).x) / 10.0 == doctest::Approx(NODO));
    CHECK(g->spostamento(t + 10.0).y == doctest::Approx(g->spostamento(t).y));
    v.avanza(60.0);                      // finito: la deriva dei parametri
    CHECK(v.m.stati_interventi().back().finito);
    const double t2 = v.m.tempo();
    CHECK((g->spostamento(t2 + 10.0).x - g->spostamento(t2).x) / 10.0 == doctest::Approx(g->parametri().deriva_m_s.x));

    Intervento senza = ambiente("polynya", 100.0, 0.0);
    senza.testi["azione"] = "chiudi";
    senza.punto = qui;
    CHECK_FALSE(v.m.intervieni(senza).ok);   // senza raggio
    Intervento c = senza;
    c.raggio_m = 2000.0;
    c.durata_s = 400.0;
    REQUIRE(v.m.intervieni(c).ok);
    const auto avvisi = [&] {
        return std::count_if(v.m.messaggi().begin(), v.m.messaggi().end(), [](const Messaggio& k) { return k.codice == 499 && k.unita == 0; });
    };
    v.avanza(50.0);
    CHECK(v.m.copertura_ghiaccio(qui) == doctest::Approx(0.5).epsilon(0.05));
    CHECK(v.m.sotto_banchisa(0));
    CHECK(avvisi() == 1);
    v.avanza(200.0);
    CHECK(avvisi() == 1);                // una volta sola
    CHECK(v.m.copertura_ghiaccio(qui) == 1.0);
    CHECK(v.m.trova(0)->integrita == integrita);
    const auto voci = std::count_if(v.m.reazioni_ia().begin(), v.m.reazioni_ia().end(), [](const Missione::ReazioneIa& r) { return r.reazione == "ghiaccio_nuovo"; });
    CHECK(voci == 1);                    // il Victor a 300 m non ha niente sopra di se' che lo riguardi
    bool nel_registro = false;
    for (const auto& e : v.m.eventi_missione()) nel_registro |= e.tipo == TipoEvento::reazione_ia && e.testo_it.find("Ghiaccio nuovo") != std::string::npos;
    CHECK(nel_registro);
    v.avanza(300.0);                     // scaduto: si riapre con la stessa transizione
    CHECK(v.m.copertura_ghiaccio(qui) == 0.0);
    CHECK_FALSE(v.m.sotto_banchisa(0));

    Intervento a = ambiente("polynya", 60.0, 0.0);
    a.testi["azione"] = "apri";
    a.punto = qui;
    a.raggio_m = 1000.0;
    REQUIRE(v.m.intervieni(a).ok);
    v.avanza(70.0);
    CHECK(v.m.ghiaccio()->in(qui, v.m.tempo()).aperta);
}
