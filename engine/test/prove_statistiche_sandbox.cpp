// Statistiche tecniche della sandbox (nucleo/statistiche_sandbox.hpp, docs/19 §4): una traccia sintetica coi conti
// noti, la serie, e una partita vera dell'Enhanced registrata con la raccolta.
#include <doctest.h>

#include <cmath>
#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

#include <map>
#include <memory>

#include "missioni/comandante_ia.hpp"
#include "missioni/danni_missione.hpp"
#include "missioni/missione.hpp"
#include "missioni/stazioni_missione.hpp"
#include "missioni/traccia_missione.hpp"
#include "nucleo/classi.hpp"
#include "nucleo/fondale.hpp"
#include "nucleo/statistiche_sandbox.hpp"

using namespace m688;
namespace fs = std::filesystem;

namespace {

constexpr int NATO = 0x08, PATTO = 0x10;

// un'unita' ogni 5 s da t0 a t1 (l'ultimo campione rimosso se `esce`), ferma in `pos`
void unita(Traccia& t, int id, const std::string& classe, const std::string& categoria, int flag, double t0, double t1,
           Vec2 pos, bool esce = false, int lanciatore = -1) {
    for (double x = t0; x <= t1 + 1e-9; x += 5.0) {
        StatoUnita u;
        u.id = id; u.classe = classe; u.categoria = categoria; u.flag = flag;
        u.pos = pos;
        u.pos_stimata = pos;
        u.lanciatore = lanciatore;
        u.rimossa = esce && x + 5.0 > t1 + 1e-9;
        t.unita[id].emplace_back(x, u);
    }
}
EventoTraccia ev(const char* tipo, double tempo, int u, int altra = -1, int arma = -1) {
    EventoTraccia e;
    static int id = 0;
    e.id = id++; e.tipo = tipo; e.tempo = tempo; e.unita = u; e.altra = altra; e.arma = arma;
    return e;
}
EventoIa fatto(const char* tipo, double t, int u, int altra, double a = 0.0, const char* s = "", int n = 0) {
    EventoIa e;
    e.t = t; e.tipo = tipo; e.unita = u; e.altra = altra; e.a = a; e.s = s; e.n = n;
    return e;
}

Traccia sintetica() {
    Traccia t;
    t.versione = 2;
    t.durata = 900.0;
    t.metadati["giocatore"] = "1";
    unita(t, 1, "los_angeles", "sottomarino", 0x09, 0, 900, {0, 0});
    unita(t, 2, "victor3", "sottomarino", 0x11, 0, 700, {0, 20000}, true);
    unita(t, 3, "krivak2", "nave", 0x12, 0, 900, {5000, 25000});
    unita(t, 4, "mercantile", "nave", 0x42, 0, 900, {-8000, 0});
    unita(t, 10, "mk48", "arma", 0x29, 400, 700, {0, 10000}, true, 1);    // il Mk 48 del giocatore: uccide il Victor
    unita(t, 11, "type53", "arma", 0x31, 500, 800, {0, 5000}, true, 2);   // il siluro del Victor: preso dall'esca
    unita(t, 12, "adc_mk2", "arma", 0xC1, 530, 700, {0, 500}, true, 1);   // l'esca del giocatore
    // il giocatore: periscopio alzato nel primo minuto, la stima a 100 m dalla vera, il filo per 60 s
    for (auto& [x, u] : t.unita[1]) {
        u.alberi = x < 60.0 ? 0x02 : 0;
        u.pos_stimata = {u.pos.x + 100.0, u.pos.y};
        u.sigma_nav_m = 150.0;
    }
    for (auto& [x, u] : t.unita[10]) u.collegata = x < 460.0 ? 1 : -1;
    // i contatti: il giocatore sente il Victor a 20 km (possibile da 40 s), il Victor sente il giocatore a 300 s
    t.eventi_ia = {fatto("possibile", 40, 1, 2, 20000, "tb16"), fatto("contatto_nuovo", 100, 1, 2, 20000, "tb16"),
                   fatto("contatto_solido", 160, 1, 2, 20000, "tb16"), fatto("contatto_nuovo", 300, 2, 1, 20000, "bqq5"),
                   fatto("contatto_nuovo", 120, 3, 1, 25500, "esm"),   // contro-scoperta: il periscopio era su? no, a 120 s
                   fatto("contatto_nuovo", 520, 1, 11, 5000, "tb16"), fatto("siluro", 560, 11, 2, 0, "esca_sedotta", 12)};
    t.registro = {ev("classificazione", 200, 2), ev("allerta_forza", 130, -1), ev("comandante_ia", 150, 1, 2),
                  ev("lancio_giocatore", 400, 1, 2, 10), ev("comandante_ia", 525, 1, 11), ev("colpo", 690, 2, 1, 10),
                  ev("affondamento", 700, 2), ev("ping_attivo", 380, -1), ev("punto_nave", 50, 1)};
    t.registro[1].codice = NATO; t.registro[1].valore = 0; t.registro[1].livello = 2;
    t.registro[2].classe_arma = "tma";
    t.registro[4].classe_arma = "difesa";
    t.registro[0].giusta = true;
    t.registro[8].giusta = true;
    std::stable_sort(t.registro.begin(), t.registro.end(), [](const auto& a, const auto& b) { return a.tempo < b.tempo; });
    // la soluzione proposta poco prima del lancio: 19,5 km (vera 20), rotta 10 (vera 0), 4 kn (vero 0)
    Traccia::Quadro q;
    q.t = 395.0;
    q.app.soluzione = true;
    q.app.proposta = {19500.0, 10.0, 4.0 * 0.514444};
    q.bersaglio_tma = 2;
    t.quadri.push_back(q);
    return t;
}

}  // namespace

TEST_CASE("statistiche sandbox: una traccia sintetica coi conti noti") {
    const StatisticheSandbox s = statistiche_sandbox(sintetica());
    CHECK(s.giocatore == 1);
    CHECK(s.complete);
    // catene: giocatore -> Victor col ritardo dal possibile; Victor -> giocatore; Krivak -> giocatore via ESM
    const CatenaScoperta* c = nullptr;
    for (const auto& x : s.catene) if (x.osservatore == 1 && x.bersaglio == 2) c = &x;
    REQUIRE(c);
    CHECK(c->sensore == "tb16");
    CHECK(c->t_contatto == doctest::Approx(100.0));
    CHECK(c->t_solido == doctest::Approx(160.0));
    CHECK(c->ritardo_s() == doctest::Approx(60.0));
    CHECK(c->classificato);
    CHECK(c->classe_giusta);
    CHECK(c->t_classificato == doctest::Approx(200.0));
    for (const auto& x : s.catene) CHECK_FALSE(x.bersaglio == 11);   // le armi non fanno catene
    // reazioni del giocatore: allerta +30, ingaggio +50, lancio +300; la minaccia a 520, l'esca a 525
    const ReazioneUnita* r = nullptr;
    for (const auto& x : s.reazioni) if (x.unita == 1) r = &x;
    REQUIRE(r);
    CHECK(r->su == 2);
    CHECK(r->allerta_s == doctest::Approx(30.0));
    CHECK(r->ingaggio_s == doctest::Approx(50.0));
    CHECK(r->lancio_s == doctest::Approx(300.0));
    CHECK(r->minaccia_t == doctest::Approx(520.0));
    CHECK(r->evasione_s == doctest::Approx(5.0));
    // armi: il Mk 48 uccide, col filo per 60 s; il Type 53 va sull'esca; l'esca non e' un'arma lanciata
    REQUIRE(s.armi.size() == 2);
    for (const auto& a : s.armi) {
        if (a.arma == 10) {
            CHECK(a.esito == ArmaLanciata::Esito::uccisione);
            CHECK(a.vittima == 2);
            CHECK(a.bersaglio == 2);
            CHECK(a.filo_s == doctest::Approx(60.0));
        } else {
            CHECK(a.arma == 11);
            CHECK(a.esito == ArmaLanciata::Esito::esca);
            CHECK(std::string(nome_esito(a.esito)) == "esca");
        }
    }
    // l'errore della soluzione al lancio del giocatore
    const TmaUnita* tm = nullptr;
    for (const auto& x : s.tma) if (x.unita == 1 && x.bersaglio == 2) tm = &x;
    REQUIRE(tm);
    REQUIRE(tm->errore_distanza_m);
    CHECK(*tm->errore_distanza_m == doctest::Approx(500.0));
    CHECK(*tm->errore_rotta_gradi == doctest::Approx(10.0));
    CHECK(*tm->errore_velocita_kn == doctest::Approx(4.0));
    // esposizione e navigazione del giocatore
    const EsposizioneUnita* e = nullptr;
    for (const auto& x : s.esposizione) if (x.unita == 1) e = &x;
    REQUIRE(e);
    CHECK(e->periscopio_s == doctest::Approx(60.0));
    CHECK(e->ping == 1);
    CHECK(e->contro_scoperte == 1);   // il Krivak via ESM
    const NavigazioneUnita* n = nullptr;
    for (const auto& x : s.navigazione) if (x.unita == 1) n = &x;
    REQUIRE(n);
    CHECK(n->errore_medio_m == doctest::Approx(100.0));
    CHECK(n->errore_max_m == doctest::Approx(100.0));
    CHECK(n->punti_nave == 1);
    // per parte: un Victor perso dal Patto, inflitto dalla NATO; un'esca efficace della NATO
    REQUIRE(s.parti.count(NATO));
    REQUIRE(s.parti.count(PATTO));
    const StatParte& nato = s.parti.at(NATO);
    const StatParte& patto = s.parti.at(PATTO);
    CHECK(nato.unita == 1);
    CHECK(patto.unita == 2);
    CHECK(s.parti.at(0).unita == 1);   // il mercantile
    CHECK(patto.perse == 1);
    CHECK(nato.inflitte == 1);
    CHECK(nato.perse == 0);
    CHECK(nato.scambio == doctest::Approx(1.0));
    CHECK(patto.scambio == doctest::Approx(0.0));
    CHECK(nato.scoperte == 1);
    CHECK(nato.scoperte_subite == 2);
    CHECK(nato.ritardo_medio_s == doctest::Approx(60.0));
    CHECK(nato.lanci == 1);
    CHECK(nato.a_segno == 1);
    CHECK(nato.pk.at("mk48") == std::pair<int, int>{1, 1});
    CHECK(patto.lanci == 1);
    CHECK(patto.a_segno == 0);
    CHECK(nato.esche == 1);
    CHECK(nato.esche_efficaci == 1);
    CHECK(nato.reazione_media_s == doctest::Approx(30.0));
    CHECK(nato.errore_lancio_medio_m == doctest::Approx(500.0));
}

// l'ESM sente il radar a ogni giro: un contatto che esce e torna entro 30 s non e' una contro-scoperta, dopo si'
TEST_CASE("statistiche sandbox: la contro-scoperta non conta i contatti che tornano subito") {
    Traccia t;
    t.versione = 2;
    t.durata = 300.0;
    t.metadati["giocatore"] = "1";
    unita(t, 1, "los_angeles", "sottomarino", 0x09, 0, 300, {0, 0});
    unita(t, 2, "victor3", "sottomarino", 0x11, 0, 300, {20000, 0});
    t.eventi_ia = {fatto("contatto_nuovo", 100.0, 2, 1, 20000, "esm"), fatto("contatto_perso", 101.0, 2, 1, 20000),
                   fatto("contatto_nuovo", 102.5, 2, 1, 20000, "esm"), fatto("contatto_perso", 104.0, 2, 1, 20000),
                   fatto("contatto_nuovo", 105.5, 2, 1, 20000, "esm"), fatto("contatto_perso", 110.0, 2, 1, 20000),
                   fatto("contatto_nuovo", 150.0, 2, 1, 20000, "esm")};
    const StatisticheSandbox s = statistiche_sandbox(t);
    const EsposizioneUnita* e = nullptr;
    for (const auto& x : s.esposizione) if (x.unita == 1) e = &x;
    REQUIRE(e);
    CHECK(e->contro_scoperte == 2);
}

TEST_CASE("statistiche sandbox: la serie fa media e dispersione, i mancanti restano fuori") {
    StatisticheSandbox a = statistiche_sandbox(sintetica());
    StatisticheSandbox b = a;
    b.parti[NATO].scambio = 3.0;
    b.parti[NATO].ritardo_medio_s = -1.0;   // mancante
    const auto r = riassunto_serie({a, b});
    const ValoreSerie& sc = r.at(NATO).at("scambio");
    CHECK(sc.n == 2);
    CHECK(sc.media == doctest::Approx(2.0));
    CHECK(sc.min == doctest::Approx(1.0));
    CHECK(sc.max == doctest::Approx(3.0));
    CHECK(sc.dev == doctest::Approx(std::sqrt(2.0)));
    CHECK(r.at(NATO).at("ritardo_medio_s").n == 1);
    CHECK(r.at(PATTO).at("perse").media == doctest::Approx(1.0));
    // una traccia vecchia, senza la raccolta: le scoperte dal registro, «non complete»
    Traccia v1;
    v1.metadati["giocatore"] = "1";
    unita(v1, 1, "los_angeles", "sottomarino", 0x09, 0, 100, {0, 0});
    unita(v1, 2, "victor3", "sottomarino", 0x11, 0, 100, {0, 9000});
    v1.registro = {ev("scoperta", 30, 2)};
    v1.registro[0].distanza_m = 9000.0;
    const StatisticheSandbox s1 = statistiche_sandbox(v1);
    CHECK_FALSE(s1.complete);
    REQUIRE(s1.catene.size() == 1);
    CHECK(s1.catene[0].osservatore == 1);
    CHECK(s1.catene[0].t_contatto == doctest::Approx(30.0));
    CHECK(s1.catene[0].ritardo_s() < 0.0);
}

TEST_CASE("statistiche sandbox: una partita sandbox vera (S4, due SSN, il comandante IA al posto del giocatore)" *
          doctest::skip(!fs::exists(M688_DATI "/acustica.toml") || !fs::exists(M688_MISSIONI "/prove/PROVA_S4_DUE_SSN.lua"))) {
    // come --sandbox dell'app: la regola di simulazione, nessun giocatore, il comandante che da' gli ordini ogni secondo
    auto classi = carica_classi(M688_DATI "/classi.toml");
    Missione m(std::string(M688_MISSIONI) + "/prove/PROVA_S4_DUE_SSN.lua", 1);
    std::map<std::string, double> pescaggi;
    for (const auto& [id, k] : classi) pescaggi[id] = k.pescaggio_m;
    m.imposta_fondale(std::make_shared<const Fondale>(Fondale::carica(std::string(M688_DATI) + "/teatri/" + m.teatro() + ".fdl")),
                      pescaggi);
    DanniMissione dm(classi, M688_DATI, 1, RegolaDanni::simulazione);
    m.senza_giocatore(true);
    m.registra_statistiche(true);   // R1-R6 del runtime
    StazioniMissione st(m, 1);
    ComandanteIa comandante(m, OpzioniComandante{}, 1);
    RegistratoreTraccia reg("PROVA_S4_DUE_SSN", 1);
    REQUIRE(m.giocatore());
    reg.metadato("giocatore", std::to_string(m.giocatore()->id));
    RaccoltaTraccia raccolta;
    double prossimo = 0.0;
    for (int k = 0; k < 1800 * 9 && !m.esito(); ++k) {
        if (m.tempo() >= prossimo) {
            for (const Ordine& o : comandante.passo(m, st)) st.consegna(m, o);
            prossimo += 1.0;
        }
        m.avanza(1.0 / 9.0);
        dm.avanza(m, 1.0 / 9.0);
        m.raccogli_danni(nullptr, &dm);
        if (reg.serve(m.tempo(), m.eventi_missione().size()))
            reg.aggiungi(fotogramma_da(m, &st, {}, reg.ultimo_evento(), &raccolta), m.eventi_missione().size());
    }
    REQUIRE(m.con_acustica());
    const Traccia t = leggi_traccia(reg.dati());
    const StatisticheSandbox s = statistiche_sandbox(t);
    CHECK(s.complete);
    CHECK(s.giocatore == m.giocatore()->id);
    CHECK(s.durata_s >= m.tempo() - RegistratoreTraccia::PASSO_S - 0.5);   // l'ultimo fotogramma (senza unita' veloci, ogni 5 s)
    REQUIRE_FALSE(s.catene.empty());
    int con_sensore = 0;   // i contatti dati dallo script o da un rapporto non hanno un sensore
    for (const auto& c : s.catene) {
        CHECK(c.osservatore != c.bersaglio);
        CHECK(parte_da_flag(t.unita.at(c.osservatore).front().second.flag) != parte_da_flag(t.unita.at(c.bersaglio).front().second.flag));
        if (c.t_contatto >= 0.0) CHECK(c.distanza_m > 0.0);
        con_sensore += !c.sensore.empty();
    }
    CHECK(con_sensore >= 1);
    // con R2 le scoperte hanno il primo istante possibile, prima o insieme al contatto
    int con_ritardo = 0;
    for (const auto& c : s.catene)
        if (c.ritardo_s() >= 0.0) ++con_ritardo;
    CHECK(con_ritardo >= 1);
    // le righe K: il quadro di chi scopre, all'istante della scoperta e dopo, con il bersaglio. Un contatto debole e
    // lontano lampeggia: con la S4 del 28/10/2026 (SSN vicini al confine) la prima catena e' il 688 sul Victor con la TB-16
    // a 37 km, nel quadro alla scoperta, fuori da +3 a +30 s e di nuovo dentro a +60 s. Si guarda il primo minuto
    REQUIRE_FALSE(t.contatti_unita.empty());
    const CatenaScoperta& prima = s.catene.front();
    if (prima.t_contatto >= 0.0) {
        bool c_e = false;
        for (double dt = 0.0; dt <= 60.0 && !c_e; dt += 3.0)
            for (const auto& k : t.quadro_unita(prima.osservatore, prima.t_contatto + dt)) c_e = c_e || k.bersaglio == prima.bersaglio;
        CHECK(c_e);
    }
    // il quadro di parte (record P): la NATO ha nel suo quadro chi ha scoperto
    REQUIRE_FALSE(t.quadri_parte.empty());
    if (prima.t_contatto >= 0.0 && parte_da_flag(t.unita.at(prima.osservatore).front().second.flag) == 0x08) {
        const QuadroParte* q = t.quadro_parte(0x08, t.durata);
        REQUIRE(q);
        bool c_e = false;
        for (const VoceParte& v : q->voci) c_e = c_e || v.bersaglio == prima.bersaglio;
        CHECK(c_e);
    }
    // la TMA in corso nei campioni di chi la fa (record V)
    if (!s.tma.empty()) {
        bool in_corso = false;
        for (const auto& [x, u] : t.unita.at(s.tma.front().unita)) in_corso = in_corso || u.tma_bersaglio == s.tma.front().bersaglio;
        CHECK(in_corso);
    }
    REQUIRE(s.parti.count(0x08));
    CHECK(s.parti.at(0x08).scoperte >= 1);
    CHECK(s.parti.at(0x08).distanza_media_m > 1000.0);
    for (const auto& [p, x] : s.parti) {
        CHECK(x.scambio >= 0.0);
        CHECK(x.a_segno <= x.lanci);
    }
    for (const auto& r : s.reazioni) CHECK((r.t_scoperta >= 0.0 || r.minaccia_t >= 0.0));
    for (const auto& e : s.esposizione) CHECK(e.schiera_s <= s.durata_s + 1e-6);
    MESSAGE("S4 sandbox 1800 s: " << s.catene.size() << " catene, " << s.armi.size() << " armi, " << s.reazioni.size()
                                   << " reazioni, " << s.tma.size() << " TMA, traccia " << reg.dati().size() << " byte");
}

TEST_CASE("statistiche sandbox: le cariche di profondita' (senza un'unita' d'arma) contano nei lanci, nel Pk e nell'uccisione") {
    Traccia t;
    t.versione = 2;
    t.durata = 600.0;
    unita(t, 8, "perry", "nave", 0x0a, 0, 600, {0, 0});
    unita(t, 12, "victor3", "sottomarino", 0x11, 0, 400, {0, 800}, true);
    t.eventi_ia = {fatto("contatto_nuovo", 10, 8, 12, 800, "sqs56")};
    t.registro = {ev("colpo", 300, 12, 8), ev("colpo", 330, 12, 8), ev("colpo", 360, 12, 8), ev("affondamento", 360, 12, 8)};
    for (int k = 0; k < 3; ++k) t.registro[k].classe_arma = "carica";
    t.registro[0].valore = 0.0;    // la prima fallisce
    t.registro[1].valore = 30.0;
    t.registro[2].valore = 30.0;   // la terza affonda
    const StatisticheSandbox s = statistiche_sandbox(t);
    REQUIRE(s.armi.size() == 3);
    int uccisioni = 0, mancate = 0, a_segno = 0;
    for (const ArmaLanciata& a : s.armi) {
        CHECK(a.arma == -1);
        CHECK(a.lanciatore == 8);
        CHECK(a.classe == "carica");
        uccisioni += a.esito == ArmaLanciata::Esito::uccisione;
        mancate += a.esito == ArmaLanciata::Esito::mancata;
        a_segno += a.esito == ArmaLanciata::Esito::a_segno;
    }
    CHECK(uccisioni == 1);
    CHECK(mancate == 1);
    CHECK(a_segno == 1);
    const StatParte& nato = s.parti.at(0x08);
    CHECK(nato.inflitte == 1);
    CHECK(nato.lanci == 3);
    CHECK(nato.a_segno == 2);
    CHECK(nato.pk.at("carica") == std::pair<int, int>{2, 3});
    CHECK(s.parti.at(0x10).perse == 1);
}

TEST_CASE("statistiche sandbox: l'aggregazione di piu' partite (quantili, esiti, Pk sommato, ordine intatto)") {
    std::vector<StatisticheSandbox> v(3);
    const double scambi[3] = {1.0, 4.0, 2.0};
    for (int k = 0; k < 3; ++k) {
        StatParte& n = v[static_cast<std::size_t>(k)].parti[0x08];
        n.parte = 0x08;
        n.scambio = scambi[k];
        n.ritardo_medio_s = k == 1 ? -1.0 : 30.0 * (k + 1);   // mancante nella seconda
        n.pk["mk48"] = {k, 2};
        v[static_cast<std::size_t>(k)].esito = k < 2 ? 21 : -1;
        v[static_cast<std::size_t>(k)].esito_testo = k < 2 ? "Tempo scaduto" : "";
    }
    const RiassuntoSerie r = riassunto_partite(v);
    CHECK(r.partite == 3);
    const ValoreSerie& s = r.parti.at(0x08).at("scambio");
    CHECK(s.n == 3);
    CHECK(s.media == doctest::Approx(7.0 / 3.0));
    CHECK(s.dev == doctest::Approx(std::sqrt(((1 - 7.0 / 3) * (1 - 7.0 / 3) + (4 - 7.0 / 3) * (4 - 7.0 / 3) + (2 - 7.0 / 3) * (2 - 7.0 / 3)) / 2.0)));
    CHECK(s.min == doctest::Approx(1.0));
    CHECK(s.max == doctest::Approx(4.0));
    CHECK(s.mediana == doctest::Approx(2.0));
    CHECK(s.q10 == doctest::Approx(1.2));   // 1 + 0,2 (2 - 1)
    CHECK(s.q90 == doctest::Approx(3.6));   // 2 + 0,8 (4 - 2)
    const ValoreSerie& rit = r.parti.at(0x08).at("ritardo_medio_s");
    CHECK(rit.n == 2);
    CHECK(rit.media == doctest::Approx(60.0));   // 30 e 90
    CHECK(r.pk.at(0x08).at("mk48") == std::pair<int, int>{3, 6});
    REQUIRE(r.esiti.count(21));
    CHECK(r.esiti.at(21).volte == 2);
    CHECK(r.esiti.at(21).testo == "Tempo scaduto");
    CHECK(r.esiti.at(21).frequenza == doctest::Approx(2.0 / 3.0));
    CHECK(r.esiti.at(-1).volte == 1);
    CHECK(r.esiti.at(-1).testo == "non finita");
    CHECK(v[1].parti.at(0x08).scambio == doctest::Approx(4.0));   // l'ordine delle partite non cambia
    CHECK(riassunto_serie(v).at(0x08).at("scambio").mediana == doctest::Approx(2.0));
}

TEST_CASE("statistiche sandbox: una serie vera di tre semi (S4 in sandbox, 600 s)" *
          doctest::skip(!fs::exists(M688_DATI "/acustica.toml") || !fs::exists(M688_MISSIONI "/prove/PROVA_S4_DUE_SSN.lua"))) {
    auto classi = carica_classi(M688_DATI "/classi.toml");
    std::map<std::string, double> pescaggi;
    for (const auto& [id, k] : classi) pescaggi[id] = k.pescaggio_m;
    std::vector<StatisticheSandbox> serie;
    for (std::uint64_t seme = 1; seme <= 3; ++seme) {
        Missione m(std::string(M688_MISSIONI) + "/prove/PROVA_S4_DUE_SSN.lua", seme);
        m.imposta_fondale(std::make_shared<const Fondale>(Fondale::carica(std::string(M688_DATI) + "/teatri/" + m.teatro() + ".fdl")),
                          pescaggi);
        DanniMissione dm(classi, M688_DATI, seme, RegolaDanni::simulazione);
        m.senza_giocatore(true);
        m.registra_statistiche(true);
        StazioniMissione st(m, static_cast<std::uint32_t>(seme));
        ComandanteIa comandante(m, OpzioniComandante{}, seme);
        RegistratoreTraccia reg("PROVA_S4_DUE_SSN", seme);
        reg.metadato("giocatore", std::to_string(m.giocatore()->id));
        RaccoltaTraccia raccolta;
        double prossimo = 0.0;
        for (int k = 0; k < 600 * 9 && !m.esito(); ++k) {
            if (m.tempo() >= prossimo) {
                for (const Ordine& o : comandante.passo(m, st)) st.consegna(m, o);
                prossimo += 1.0;
            }
            m.avanza(1.0 / 9.0);
            dm.avanza(m, 1.0 / 9.0);
            m.raccogli_danni(nullptr, &dm);
            if (reg.serve(m.tempo(), m.eventi_missione().size()))
                reg.aggiungi(fotogramma_da(m, &st, {}, reg.ultimo_evento(), &raccolta), m.eventi_missione().size());
        }
        serie.push_back(statistiche_sandbox(leggi_traccia(reg.dati())));
    }
    const RiassuntoSerie r = riassunto_partite(serie);
    CHECK(r.partite == 3);
    int volte = 0;
    for (const auto& [codice, e] : r.esiti) {
        volte += e.volte;
        CHECK(e.frequenza == doctest::Approx(e.volte / 3.0));
    }
    CHECK(volte == 3);
    REQUIRE(r.parti.count(0x08));
    for (const auto& [nome, x] : r.parti.at(0x08)) {
        CAPTURE(nome);
        CHECK(x.n >= 1);
        CHECK(x.n <= 3);
        CHECK(x.min <= x.q10 + 1e-9);
        CHECK(x.q10 <= x.mediana + 1e-9);
        CHECK(x.mediana <= x.q90 + 1e-9);
        CHECK(x.q90 <= x.max + 1e-9);
        CHECK(x.media >= x.min - 1e-9);
        CHECK(x.media <= x.max + 1e-9);
    }
    CHECK(r.parti.at(0x08).at("unita").n == 3);
}
