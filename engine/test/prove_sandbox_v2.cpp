#include <doctest.h>

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <map>
#include <memory>
#include <set>
#include <string>

#include "missioni/comandante_ia.hpp"
#include "missioni/danni_missione.hpp"
#include "missioni/danni_sottomarini.hpp"
#include "missioni/missione.hpp"
#include "missioni/registro_missione.hpp"
#include "missioni/stazioni_missione.hpp"
#include "nucleo/classi.hpp"
#include "nucleo/armi_giocatore.hpp"
#include "nucleo/artiglieria.hpp"
#include "nucleo/carico.hpp"
#include "nucleo/radio.hpp"
#include "nucleo/sonarista.hpp"

using namespace m688;
namespace fs = std::filesystem;

// Sandbox V2, la parte del runtime (docs/19_sandbox.md; nota condivisa sandbox-v2-runtime-contratto.md): le regolazioni
// per parte complete (livello, allerta) e le liste per le statistiche tecniche (R1-R6).

namespace {

const std::string SUB = "modello = { massima_kn = 32, virata_gradi_s = 1, accel_kn_s = 0.5, quota_m_s = 1, quota_max_m = 450, integrita = 200, rumore = 8, sonar = 19 }";
const std::string NAVE = "modello = { massima_kn = 32, virata_gradi_s = 2, accel_kn_s = 0.5, quota_m_s = 1, quota_max_m = 0, integrita = 100, rumore = 10, sonar = 20 }";

// un Victor III (sonarista esperto per la chiave d'unita') che sente una Perry a 5 km; il giocatore lontano
std::string scena_victor() {
    const auto f = fs::temp_directory_path() / "m688_prova_sandbox_v2.lua";
    std::ofstream(f) << "return { id = 'SANDBOX_V2', teatro = 'faroe', lato = 'usa', data = '1989-02-15', ora_utc = 12, paesaggio_sonoro = false,"
                        " script = {}, varianti = { { probabilita = 1, unita = {\n"
                        "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true, posizione = { x = 0, y = -60000 },"
                        "  quota_m = 300, rotta = 90, velocita_kn = 3, " << SUB << " },\n"
                        "{ id = 2, classe = 'victor3', categoria = 'sottomarino', flag = 17, sonarista = 'esperto', posizione = { x = 0, y = 0 },"
                        "  quota_m = 150, rotta = 0, velocita_kn = 4, " << SUB << " },\n"
                        "{ id = 3, classe = 'perry', categoria = 'nave', flag = 10, posizione = { x = 0, y = 5000 }, quota_m = 0, rotta = 90,"
                        "  velocita_kn = 15, " << NAVE << " },\n}, timer = {} } } }\n";
    return f.string();
}

struct Partita {
    std::map<std::string, ClasseNave> classi = carica_classi(M688_DATI "/classi.toml");
    Missione m;
    DanniMissione dm;
    explicit Partita(const std::string& file) : m(file, 1, false), dm(classi, M688_DATI, 1, RegolaDanni::simulazione) {}
    void avanza(double s) { for (long i = 0; i < static_cast<long>(s * 9.0); ++i) { m.avanza(1.0 / 9.0); dm.avanza(m, 1.0 / 9.0); } }
};

const std::string TORPEX = std::string(M688_MISSIONI) + "/SCENE1A.lua";   // TORPEX '89: tre Forrest Sherman da affondare

// la partita sandbox col comandante, come in prove_comandante_ia
struct Sandbox {
    std::map<std::string, ClasseNave> classi = carica_classi(M688_DATI "/classi.toml");
    Missione m;
    DanniMissione dm;
    DanniSottomarini ds;
    StazioniMissione st;
    RegistroOrdini ordini;
    std::unique_ptr<ComandanteIa> comandante;
    Sandbox(std::uint64_t seme, bool statistiche)
        : m(TORPEX, seme), dm(classi, M688_DATI, seme, RegolaDanni::simulazione), ds(classi, M688_DATI, seme), st(m, static_cast<std::uint32_t>(seme)) {
        m.senza_giocatore(true);
        m.registra_statistiche(statistiche);
        if (const auto* g = m.giocatore()) ds.prepara(m, g->id);
        st.usa_danni(&ds);
        comandante = std::make_unique<ComandanteIa>(m, OpzioniComandante{}, seme);
    }
    void avanza(double s) {
        for (long i = 0; i < static_cast<long>(s * 9.0); ++i) {
            for (const auto& o : comandante->passo(m, st)) ordini.ordina(o);
            for (const auto& o : ordini.consegna(m.tempo())) {
                if (ds.consegna(m, o)) continue;
                st.consegna(m, o);
            }
            m.avanza(1.0 / 9.0);
            st.dopo_passo(m);
            dm.avanza(m, 1.0 / 9.0);
            ds.avanza(m, 1.0 / 9.0);
            if (m.esito()) break;
        }
    }
};

}  // namespace

TEST_CASE("sandbox V2: il livello della parte vince sulla chiave del sonarista dell'unita'") {
    const auto f = scena_victor();
    Partita base(f);
    base.avanza(30.0);
    REQUIRE(base.m.sonarista_ia(2) != nullptr);
    CHECK(base.m.sonarista_ia(2)->profilo().ritardo_s == doctest::Approx(Sonarista(Esperienza::esperto).profilo().ritardo_s));
    Partita r(f);
    Missione::RegolazioniParte patto;
    patto.livello = Esperienza::recluta;
    r.m.regolazioni({{0x10, patto}});
    r.avanza(30.0);
    REQUIRE(r.m.sonarista_ia(2) != nullptr);
    CHECK(r.m.sonarista_ia(2)->profilo().ritardo_s == doctest::Approx(Sonarista(Esperienza::recluta).profilo().ritardo_s));
    fs::remove(f);
}

TEST_CASE("sandbox V2: l'allerta di partenza della parte, anche sotto il minimo della guerra") {
    // la scena senza blocco carriera ne' editor e' di guerra: di norma almeno A2
    const auto f = scena_victor();
    Partita base(f);
    base.avanza(2.0);
    REQUIRE(base.m.allerta(0x10).has_value());
    CHECK(*base.m.allerta(0x10) >= 2);
    Partita r(f);
    Missione::RegolazioniParte patto;
    patto.allerta = 0;
    r.m.regolazioni({{0x10, patto}});
    r.avanza(2.0);
    CHECK(r.m.allerta(0x10) == 0);   // la forza sorpresa
    CHECK(r.m.allerta_unita(2) == 0);
    CHECK(r.m.allerta(0x08) == base.m.allerta(0x08));
    fs::remove(f);
}

TEST_CASE("sandbox V2: R1-R6 nella TORPEX col comandante; senza registra_statistiche niente" * doctest::skip(!fs::exists(TORPEX))) {
    Sandbox s(1, true);
    s.avanza(1500.0);
    const auto& sc = s.m.scoperte_contatti();
    REQUIRE_FALSE(sc.empty());
    const auto conta = [&](Missione::ScopertaContatto::Tipo t) {
        return std::count_if(sc.begin(), sc.end(), [&](const auto& x) { return x.tipo == t; });
    };
    CHECK(conta(Missione::ScopertaContatto::Tipo::nuovo) > 0);
    CHECK(conta(Missione::ScopertaContatto::Tipo::solido) > 0);
    CHECK(std::is_sorted(sc.begin(), sc.end(), [](const auto& a, const auto& b) { return a.t < b.t; }));
    // il primo istante possibile viene prima del primo contatto della stessa coppia (o insieme)
    const auto& sp = s.m.scoperte_possibili();
    REQUIRE_FALSE(sp.empty());
    int coppie = 0;
    for (const auto& x : sc) {
        // senza sensore: il contatto dato dalla missione o dallo script, non dalla fisica
        if (x.tipo != Missione::ScopertaContatto::Tipo::nuovo || x.sensore.empty()) continue;
        double primo = 1e18;
        for (const auto& p : sp)
            if (p.osservatore == x.osservatore && p.bersaglio == x.bersaglio) primo = std::min(primo, p.t);
        if (primo > 1e17) continue;   // un contatto senza la fisica (armi proprie, script)
        ++coppie;
        CHECK_MESSAGE(primo <= x.t + 1e-9, x.osservatore, " su ", x.bersaglio, " col sensore '", x.sensore, "' a ", x.t, " s");
    }
    CHECK(coppie > 0);
    // i lanci: quelli del battello del giocatore, comandato dal comandante, con la stima del contatto
    const auto& l = s.m.lanci();
    const auto* g = s.m.giocatore();
    REQUIRE(g);
    const auto del_giocatore = std::count_if(l.begin(), l.end(), [&](const auto& x) { return x.lanciatore == g->id; });
    MESSAGE("lanci ", l.size(), " (del giocatore ", del_giocatore, "), emissioni ", s.m.emissioni().size(), ", evasioni ",
            s.m.evasioni().size(), ", scoperte ", sc.size(), ", possibili ", sp.size());
    CHECK(del_giocatore > 0);
    CHECK(std::any_of(l.begin(), l.end(), [&](const auto& x) { return x.lanciatore == g->id && x.stima.has_value(); }));
    for (const auto& x : l)   // la stima del comandante: la sua soluzione o il contatto
        if (x.lanciatore == g->id && x.stima) CHECK((x.stima->da == "giocatore" || x.stima->da == "contatto"));
    // le emissioni sono in ordine e con un tipo noto
    for (const auto& e : s.m.emissioni()) CHECK((e.tipo == "ping" || e.tipo == "radar" || e.tipo == "radio"));
    // senza la registrazione: liste vuote
    Sandbox n(1, false);
    n.avanza(300.0);
    CHECK(n.m.scoperte_contatti().empty());
    CHECK(n.m.scoperte_possibili().empty());
    CHECK(n.m.lanci().empty());
    CHECK(n.m.emissioni().empty());
}

TEST_CASE("sandbox V2: il quadro di una parte, una voce per bersaglio dell'altra parte col dato migliore" * doctest::skip(!fs::exists(TORPEX))) {
    Sandbox s(1, false);
    s.avanza(900.0);
    for (const int lato : {0x08, 0x10}) {
        const auto q = s.m.quadro_parte(lato);
        std::set<int> visti;
        for (const auto& v : q) {
            CHECK(visti.insert(v.bersaglio).second);   // una voce sola per bersaglio
            const auto u = s.m.unita().find(v.bersaglio);
            REQUIRE(u != s.m.unita().end());
            CHECK((u->second.flag & 0x20) == 0);        // niente armi in acqua
            const int l = u->second.flag & 0x18;
            CHECK((l == 0x18 ? 0 : l) != lato);          // niente unita' della parte
            CHECK(v.t_dato <= s.m.tempo() + 1e-9);
            CHECK(std::find(v.altre_fonti.begin(), v.altre_fonti.end(), v.fonte) == v.altre_fonti.end());
            if (v.posizione && v.sensore == "radio") CHECK(v.semiasse_maggiore_m > 0.0);
        }
        MESSAGE("quadro della parte ", lato, ": ", q.size(), " voci");
    }
    // il battello del giocatore della TORPEX ha contatti sui bersagli: il quadro NATO non e' vuoto
    CHECK_FALSE(s.m.quadro_parte(0x08).empty());
}

TEST_CASE("sandbox V2: tubi carichi e TMA corrente di un'unita' dell'IA") {
    const auto f = scena_victor();
    Partita p(f);
    p.avanza(60.0);
    REQUIRE(p.m.tubi_carichi(2).has_value());
    CHECK(*p.m.tubi_carichi(2) > 0);
    CHECK(*p.m.tubi_carichi(2) <= static_cast<int>(p.m.carico_ia_di(2)->tubi.size()));
    CHECK_FALSE(p.m.tubi_carichi(0).has_value());   // il giocatore: lo sa la sua stazione
    if (const auto t = p.m.tma_ia(2)) {
        CHECK(t->bersaglio != 2);
        CHECK(t->gambe >= 0);
        if (t->soluzione) CHECK(t->soluzione->osservabile);
    }
    CHECK_FALSE(p.m.tma_ia(3).has_value());   // la Perry non fa TMA a gambe
    fs::remove(f);
}

namespace {
// un Alfa giocatore in superficie a 3 km da una Perry, nella situazione data
std::string scena_perry(const std::string& situazione) {
    const auto f = fs::temp_directory_path() / ("m688_prova_sandbox_roe_" + situazione + ".lua");
    std::ofstream(f) << "return { id = 'SANDBOX_ROE', teatro = 'faroe', paesaggio_sonoro = false, lato = 'urss', data = '1989-06-15', ora_utc = 12,"
                        " carriera = { situazione = '" << situazione << "', difficolta = 3 }, script = {}, varianti = { { probabilita = 1, unita = {\n"
                        "{ id = 0, classe = 'alfa', categoria = 'sottomarino', flag = 17, giocatore = true, posizione = { x = 0, y = 0 }, quota_m = 0,"
                        "  rotta = 90, velocita_kn = 3, " << SUB << " },\n"
                        "{ id = 1, classe = 'perry', categoria = 'nave', flag = 10, posizione = { x = 0, y = 3000 }, quota_m = 0, rotta = 270,"
                        "  velocita_kn = 5, " << NAVE << " },\n}, timer = {} } } }\n";
    return f.string();
}
std::size_t partenze(const Missione& m) {
    return static_cast<std::size_t>(std::count_if(m.eventi_artiglieria().begin(), m.eventi_artiglieria().end(),
                                                  [](const auto& e) { return e.tipo == TipoEventoArtiglieria::partenza; }));
}
}  // namespace

TEST_CASE("sandbox V2: le ROE della parte valgono anche per l'artiglieria, sopra la situazione") {
    const auto guerra = scena_perry("guerra");
    Partita base(guerra);
    base.avanza(300.0);
    CHECK(partenze(base.m) > 0);   // in guerra la Perry spara all'Alfa in superficie
    Partita trattenute(guerra);
    Missione::RegolazioniParte nato;
    nato.roe = std::map<CategoriaRoe, bool>{{CategoriaRoe::sottomarini, false}};
    trattenute.m.regolazioni({{0x08, nato}});
    trattenute.avanza(300.0);
    CHECK(partenze(trattenute.m) == 0);
    const auto pace = scena_perry("pace");
    Partita libere(pace);
    nato.roe = std::map<CategoriaRoe, bool>{{CategoriaRoe::sottomarini, true}};
    libere.m.regolazioni({{0x08, nato}});
    libere.avanza(300.0);
    CHECK(partenze(libere.m) > 0);   // in pace, con le armi libere sui sottomarini
    fs::remove(guerra);
    fs::remove(pace);
}

TEST_CASE("sandbox V2: il comandante guida a filo i suoi siluri coi comandi del giocatore" * doctest::skip(!fs::exists(TORPEX))) {
    Sandbox s(1, false);
    s.avanza(1500.0);
    const auto& o = s.ordini.storico();
    const auto filo = std::count_if(o.begin(), o.end(), [](const Ordine& x) {
        return x.tipo == TipoOrdine::comando_filo || x.tipo == TipoOrdine::taglia_filo || x.tipo == TipoOrdine::spegni_siluro;
    });
    const auto lanci = std::count_if(o.begin(), o.end(), [](const Ordine& x) { return x.tipo == TipoOrdine::lancia; });
    MESSAGE("ordini via filo ", filo, " su ", lanci, " lanci");
    REQUIRE(lanci > 0);
    CHECK(filo > 0);
    // mentre guida, dal primo lancio al primo taglio del filo, i giri ordinati non superano i 12 nodi del filo
    const auto* g = s.m.giocatore();
    REQUIRE(g);
    const auto lancio = std::find_if(o.begin(), o.end(), [](const Ordine& x) { return x.tipo == TipoOrdine::lancia; });
    const auto taglio = std::find_if(lancio, o.end(), [](const Ordine& x) { return x.tipo == TipoOrdine::taglia_filo; });
    for (auto it = lancio; it != taglio; ++it)
        if (it->tipo == TipoOrdine::giri && it->tempo > lancio->tempo) CHECK(it->a * g->avanti_tutta <= 12.0 + 0.5);
}

TEST_CASE("sandbox V2: il comandante posa le mine nel campo dell'obiettivo (Tappo, carico di minamento)" * doctest::skip(!fs::exists(std::string(M688_MISSIONI) + "/libreria/LIB_S07_TAPPO.lua"))) {
    const std::string f = std::string(M688_MISSIONI) + "/libreria/LIB_S07_TAPPO.lua";
    std::map<std::string, ClasseNave> classi = carica_classi(M688_DATI "/classi.toml");
    Missione m(f, 1);
    m.senza_giocatore(true);
    // il battello gia' a 10 km a nord del campo (centro -34089, -81978): la prova e' la posa, non il transito
    m.trova(0)->pos = {-34089.0, -71978.0};
    DanniMissione dm(classi, M688_DATI, 1, RegolaDanni::simulazione);
    DanniSottomarini ds(classi, M688_DATI, 1);
    StazioniMissione st(m, 1);
    // il carico come lo sceglie l'app: lo standard del tipo di missione (minamento), adattato al deposito
    const CatalogoCarico cat = carica_carico(M688_DATI "/carico.toml");
    SalaSiluri sala(cat, m.giocatore()->classe, m.limiti_carico());
    sala.carico_iniziale();
    st.imposta_carico(m, sala.carico(), cat);
    if (const auto* g = m.giocatore()) ds.prepara(m, g->id);
    st.usa_danni(&ds);
    ComandanteIa comandante(m, OpzioniComandante{}, 1);
    RegistroOrdini ordini;
    for (long i = 0; i < 9L * 1800 && !m.esito(); ++i) {
        for (const auto& o : comandante.passo(m, st)) ordini.ordina(o);
        for (const auto& o : ordini.consegna(m.tempo())) {
            if (ds.consegna(m, o)) continue;
            st.consegna(m, o);
        }
        m.avanza(1.0 / 9.0);
        st.dopo_passo(m);
        dm.avanza(m, 1.0 / 9.0);
        ds.avanza(m, 1.0 / 9.0);
    }
    // le mine lanciate dai tubi con le mine (la posa nel registro arriva a fine corsa, fino a 12 km dopo)
    const auto& o = ordini.storico();
    const auto mine = std::count_if(o.begin(), o.end(), [&](const Ordine& x) {
        return x.tipo == TipoOrdine::lancia && st.armi().arma(x.n) == Arma::mina;
    });
    std::string fasi;
    for (const auto& [t, s] : comandante.storia()) fasi += std::to_string(static_cast<int>(t)) + " " + nome_fase_comandante(s.fase) + "; ";
    MESSAGE("mine lanciate ", mine, ", esito ", m.esito().value_or(-1), " | ", fasi);
    CHECK(mine >= 4);   // i due tubi, poi le ricariche dalla rastrelliera (300 s)
    CHECK(std::any_of(comandante.storia().begin(), comandante.storia().end(),
                      [](const auto& x) { return x.second.fase == StatoComandante::Fase::mine; }));
}

TEST_CASE("sandbox V2: m688.tempo_contatto e m688.classificato per le regole delle missioni") {
    // il Victor sente la Perry: a 300 s la regola accosta a 123 se il contatto ha piu' di 60 s, a 77 se e' classificato
    const auto f = fs::temp_directory_path() / "m688_prova_sandbox_lua.lua";
    std::ofstream(f) << "return { id = 'SANDBOX_LUA', teatro = 'faroe', lato = 'usa', data = '1989-02-15', ora_utc = 12, paesaggio_sonoro = false,"
                        " script = { V = function(u) local s = m688.tempo_contatto(u, 3) or -1"
                        "   if m688.classificato(u, 3) then m688.rotta(u, 77) elseif s > 60 then m688.rotta(u, 123) end m688.salva_modello(u) end },"
                        " varianti = { { probabilita = 1, unita = {\n"
                        "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true, posizione = { x = 0, y = -60000 },"
                        "  quota_m = 300, rotta = 90, velocita_kn = 3, " << SUB << " },\n"
                        "{ id = 2, classe = 'victor3', categoria = 'sottomarino', flag = 17, posizione = { x = 0, y = 0 },"
                        "  quota_m = 150, rotta = 0, velocita_kn = 4, " << SUB << " },\n"
                        "{ id = 3, classe = 'perry', categoria = 'nave', flag = 10, posizione = { x = 0, y = 5000 }, quota_m = 0, rotta = 90,"
                        "  velocita_kn = 15, " << NAVE << " },\n}, timer = { { dopo_s = 300, unita = 2, gestore = 'V' } } } } }\n";
    Partita p(f.string());
    p.avanza(310.0);
    REQUIRE(p.m.unita().count(2));
    const auto& c = p.m.unita().at(2).contatti;
    REQUIRE(c.count(3));
    MESSAGE("contatto nato a ", c.at(3).prima_scoperta, " s, durata ", c.at(3).durata, ", rotta ordinata ", p.m.unita().at(2).rotta_ordinata);
    CHECK(c.at(3).prima_scoperta < 240.0);
    const double r = p.m.unita().at(2).rotta_ordinata;
    CHECK((r == doctest::Approx(123.0) || r == doctest::Approx(77.0)));
    fs::remove(f);
}

// la confidenza minima, il terzo argomento (trailer-1, S10: 1 = POSSUB; prima l'argomento si ignorava e chiudeva anche
// su NONSUB): con ciascuna soglia, vero solo con una classificazione almeno di quella confidenza (o col contatto lungo)
TEST_CASE("sandbox V2: m688.classificato con la confidenza minima") {
    const auto f = fs::temp_directory_path() / "m688_prova_sandbox_confidenza.lua";
    std::ofstream(f) << "return { id = 'SANDBOX_CONF', teatro = 'faroe', lato = 'usa', data = '1989-02-15', ora_utc = 12, paesaggio_sonoro = false,"
                        " script = { V = function(u) for k = 0, 3 do m688.variabile(u, 10 + k, m688.classificato(u, 3, k) and 1 or 0) end"
                        "   m688.dopo(u, 30, 'V') end },"
                        " varianti = { { probabilita = 1, unita = {\n"
                        "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true, posizione = { x = 0, y = -60000 },"
                        "  quota_m = 300, rotta = 90, velocita_kn = 3, " << SUB << " },\n"
                        "{ id = 2, classe = 'victor3', categoria = 'sottomarino', flag = 17, posizione = { x = 0, y = 0 },"
                        "  quota_m = 150, rotta = 0, velocita_kn = 4, " << SUB << " },\n"
                        "{ id = 3, classe = 'perry', categoria = 'nave', flag = 10, posizione = { x = 0, y = 5000 }, quota_m = 0, rotta = 90,"
                        "  velocita_kn = 15, " << NAVE << " },\n}, timer = { { dopo_s = 30, unita = 2, gestore = 'V' } } } } }\n";
    Partita p(f.string());
    p.avanza(1200.0);
    // l'ultima chiamata dello script a 1200 s (ogni 30 s): le classificazioni fino ad allora, la piu' confidente
    const double ultima = 30.0 * std::floor(p.m.tempo() / 30.0);
    int massima = -1;
    for (const auto& c : p.m.classificazioni_ia())
        if (c.unita == 2 && c.sorgente == 3 && c.tempo_s < ultima) massima = std::max(massima, static_cast<int>(c.confidenza));
    const auto& cc = p.m.unita().at(2).contatti;
    const bool lungo = cc.count(3) && cc.at(3).durata >= 1400.0;
    MESSAGE("classificazione piu' confidente del Victor sulla Perry: ", massima, ", contatto lungo ", lungo);
    for (int soglia = 0; soglia <= 3; ++soglia) {
        CAPTURE(soglia);
        const auto v = p.m.variabili().find(10 + soglia);
        const int letto = v == p.m.variabili().end() ? 0 : v->second;
        if (!lungo) CHECK(letto == (massima >= soglia ? 1 : 0));
        else CHECK(letto == 1);
    }
    fs::remove(f);
}
