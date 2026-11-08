// Statistiche (nucleo/statistiche.hpp; roadmap 6): il report di una missione dalla traccia del replay, le cumulate e
// il file
#include <doctest.h>

#include <cmath>
#include <filesystem>
#include <fstream>
#include <set>
#include <string>

#include "missioni/missione.hpp"
#include "missioni/stazioni_missione.hpp"
#include "missioni/traccia_missione.hpp"
#include "nucleo/acustica.hpp"
#include "nucleo/statistiche.hpp"
#include "nucleo/traccia_replay.hpp"

using namespace m688;
namespace fs = std::filesystem;

namespace {

const CatalogoAcustico& catalogo() {
    static const CatalogoAcustico c = carica_acustica(M688_DATI "/acustica.toml");
    return c;
}

EventoTraccia evento(int id, double t, const char* tipo) {
    EventoTraccia e;
    e.id = id;
    e.tempo = t;
    e.tipo = tipo;
    return e;
}

// 50 minuti di un 688: 10 in superficie con l'antenna, 10 a quota periscopica col periscopio, 30 a 200 m a 25 nodi;
// trasmissioni, un ping, tre lanci, due colpi e un affondamento, la scoperta nemica, due classificazioni
Traccia sintetica() {
    RegistratoreTraccia reg("PROVA", 1);
    reg.metadato("teatro", "faroe");
    reg.metadato("giocatore", "0");
    std::vector<EventoTraccia> eventi;
    auto e = evento(0, 100.0, "trasmissione");
    eventi.push_back(e);
    eventi.push_back(evento(1, 500.0, "trasmissione"));
    eventi.push_back(evento(2, 1250.0, "ping_attivo"));
    for (int k = 0; k < 3; ++k) {
        e = evento(3 + k, 1300.0 + 10.0 * k, "lancio_giocatore");
        e.classe_arma = "mk48";
        e.altra = k < 2 ? 5 : 6;
        eventi.push_back(e);
    }
    for (int k = 0; k < 2; ++k) {
        e = evento(6 + k, 1400.0 + 10.0 * k, "colpo");
        e.unita = 5;
        e.altra = 0;
        e.classe = "krivak2";
        e.classe_arma = "mk48";
        eventi.push_back(e);
    }
    e = evento(8, 1500.0, "affondamento");
    e.unita = 5;
    e.lato = 1;
    e.classe = "krivak2";
    eventi.push_back(e);
    e = evento(9, 1300.0, "avvistato");
    e.classe = "kara";
    e.distanza_m = 12000.0;
    eventi.push_back(e);
    e = evento(10, 2000.0, "ping_subito");
    e.distanza_m = 6000.0;
    eventi.push_back(e);
    for (int u : {5, 6}) {
        e = evento(10 + u, 300.0 + 100.0 * u, "scoperta");
        e.unita = u;
        eventi.push_back(e);
    }
    e = evento(17, 900.0, "classificazione");
    e.unita = 5;
    e.livello = 2;
    e.tempo_scoperta = 300.0;
    eventi.push_back(e);
    e = evento(18, 1000.0, "classificazione");
    e.unita = 6;
    e.livello = 2;
    e.giusta = false;
    e.tempo_scoperta = 800.0;
    eventi.push_back(e);
    // non contano: la sola categoria automatica, e il sonarista dell'equipaggio (che va a parte)
    e = evento(19, 700.0, "classificazione");
    e.unita = 5;
    e.livello = 1;
    e.tempo_scoperta = 300.0;
    eventi.push_back(e);
    for (int k = 0; k < 4; ++k) {
        e = evento(20 + k, 1100.0 + 10.0 * k, "classificazione");
        e.unita = 7;
        e.codice = 411;
        e.livello = 2;
        e.valore = 1.0;
        e.giusta = k != 0;
        eventi.push_back(e);
    }
    // gli id del registro crescono col tempo
    std::stable_sort(eventi.begin(), eventi.end(), [](const auto& a, const auto& b) { return a.tempo < b.tempo; });
    for (std::size_t k = 0; k < eventi.size(); ++k) eventi[k].id = static_cast<int>(k);
    std::size_t scritti = 0;
    for (double t = 0.0; t <= 3000.0 + 1e-9; t += 1.0) {
        std::size_t n = 0;
        for (const auto& x : eventi) n += x.tempo <= t;
        if (!reg.serve(t, n)) continue;
        Fotogramma f;
        f.t = t;
        StatoUnita u;
        u.id = 0;
        u.classe = "los_angeles";
        u.categoria = "sottomarino";
        u.quota = t < 600.0 ? 0.0 : t < 1200.0 ? 5.5 : 200.0;
        u.velocita_kn = t < 600.0 ? 10.0 : t < 1200.0 ? 5.0 : 25.0;
        u.alberi = t < 600.0 ? 0x04 : t < 1200.0 ? 0x02 : 0;
        u.pos = {t * 3.0, 0.0};
        f.unita.push_back(u);
        for (const auto& x : eventi)
            if (x.tempo <= t && x.id > reg.ultimo_evento()) f.eventi.push_back(x);
        reg.aggiungi(f, n);
        scritti = n;
    }
    CHECK(scritti == eventi.size());
    return leggi_traccia(reg.dati());
}

}  // namespace

TEST_CASE("statistiche: il report di una missione dalla traccia") {
    const Traccia t = sintetica();
    const ReportMissione r = report_missione(t, &catalogo());
    CHECK(r.missione == "PROVA");
    CHECK(r.teatro == "faroe");
    CHECK(r.classe == "los_angeles");
    CHECK(r.durata_s == doctest::Approx(3000.0).epsilon(0.01));
    CHECK(r.superficie_s == doctest::Approx(600.0).epsilon(0.02));
    CHECK(r.periscopica_s == doctest::Approx(600.0).epsilon(0.02));
    CHECK(r.antenna_s == doctest::Approx(600.0).epsilon(0.02));
    CHECK(r.periscopio_s == doctest::Approx(600.0).epsilon(0.02));
    CHECK(r.schiera_s == 0.0);
    CHECK(r.quota_max_m == doctest::Approx(200.0));
    CHECK(r.velocita_max_kn == doctest::Approx(25.0));
    CHECK(r.quota_media_m == doctest::Approx((600.0 * 5.5 + 1800.0 * 200.0) / 3000.0).epsilon(0.02));
    CHECK(r.distanza_nm == doctest::Approx(3000.0 * 3.0 / 1852.0).epsilon(0.01));
    CHECK(r.trasmissioni == 2);
    CHECK(r.ping == 1);
    CHECK(r.lanci == 3);
    CHECK(r.colpi == 2);
    CHECK(r.per_arma.at("mk48").lanci == 3);
    CHECK(r.per_arma.at("mk48").colpi == 2);
    REQUIRE(r.per_bersaglio.count(5));
    CHECK(r.per_bersaglio.at(5).classe == "krivak2");
    CHECK(r.per_bersaglio.at(5).affondato);
    CHECK(r.affondati_ostili == 1);
    REQUIRE(r.prima_scoperta);
    CHECK(r.prima_scoperta->come == "avvistato");
    CHECK(r.prima_scoperta->classe == "kara");
    CHECK(r.prima_scoperta->distanza_m == doctest::Approx(12000.0));
    CHECK(r.scoperte_nemiche == 2);
    CHECK(r.contatti == 2);
    CHECK(r.classificati == 2);
    CHECK(r.giuste == 1);
    CHECK(r.errate == 1);
    CHECK(r.tempo_classificazione_s == doctest::Approx(400.0));
    CHECK(r.classificazioni_sonarista == 4);
    // la firma: il livello irradiato al minuto, piu' alto a 25 nodi; la cavitazione a 25 nodi
    CHECK(r.livello_irradiato.size() >= 45);
    CHECK(r.livello_max_db > r.livello_irradiato.front().second + 5.0);   // in superficie a 10 kn cavita gia'
    CHECK(r.livello_medio_db > r.livello_irradiato.front().second);
    CHECK(r.livello_medio_db <= r.livello_max_db);
    CHECK(r.cavitazione_s > 0.0);
    double celle = 0.0;
    for (const auto& [xy, s] : r.heatmap) celle += s;
    CHECK(celle == doctest::Approx(r.durata_s));
    CHECK(r.heatmap.size() >= 9);   // 9 km percorsi in celle di 1 km
    // senza la firma: niente livello ne' cavitazione, il resto uguale
    const ReportMissione s = report_missione(t);
    CHECK(s.livello_irradiato.empty());
    CHECK(s.cavitazione_s == 0.0);
    CHECK(s.lanci == 3);
}

TEST_CASE("statistiche: le cumulate, il comandante e i globali, le riprese, il file") {
    const ReportMissione r = report_missione(sintetica(), &catalogo());
    const fs::path d = fs::temp_directory_path() / "m688_prova_statistiche";
    fs::remove_all(d);
    const fs::path p = d / "statistiche.toml";
    CHECK_FALSE(registra_statistiche(r, "halloran", true, true, p));   // ripresa: niente
    CHECK_FALSE(fs::exists(p));
    CHECK(registra_statistiche(r, "halloran", true, false, p));
    CHECK(registra_statistiche(r, "halloran", false, false, p));       // fuori carriera: solo i globali
    const FileStatistiche f = leggi_statistiche(p);
    CHECK(f.globali.missioni == 2);
    REQUIRE(f.comandanti.count("halloran"));
    const StatisticheCumulate& c = f.comandanti.at("halloran");
    CHECK(c.missioni == 1);
    CHECK(f.globali.durata_s == doctest::Approx(2.0 * r.durata_s));
    CHECK(f.globali.quota_media_m() == doctest::Approx(r.quota_media_m).epsilon(1e-6));
    CHECK(f.globali.lanci == 6);
    CHECK(f.globali.per_arma.at("mk48").colpi == 4);
    CHECK(f.globali.affondati_per_classe.at("krivak2") == 2);
    CHECK(f.globali.missioni_scoperti == 2);
    CHECK(f.globali.scoperte_per_modo.at("avvistato").first == 2);
    CHECK(f.globali.scoperta_piu_lontana_m == doctest::Approx(12000.0));
    CHECK(f.globali.scoperta_piu_vicina_m == doctest::Approx(12000.0));
    CHECK(f.globali.tempo_classificazione_s() == doctest::Approx(400.0));
    CHECK(f.globali.quota_max_m == doctest::Approx(200.0));
    double celle = 0.0;
    for (const auto& [xy, s] : f.globali.heatmap.at("faroe")) celle += s;
    CHECK(celle == doctest::Approx(2.0 * r.durata_s));
    CHECK(f.globali.heatmap.at("faroe").size() < r.heatmap.size());   // celle di 2 km
    // un file rovinato riparte da zero
    std::ofstream(p) << "[[[ non e' toml";
    CHECK(leggi_statistiche(p).globali.missioni == 0);
    fs::remove_all(d);
}

TEST_CASE("statistiche: una missione vera, dalla traccia che si registra mentre si gioca") {
    Missione m(std::string(M688_MISSIONI) + "/SCENE1A.lua", 1, false);
    StazioniMissione st(m);
    RegistratoreTraccia reg("SCENE1A", 1);
    m.avanza(1.0 / 9.0);   // il giocatore c'e' dopo il primo passo
    REQUIRE(m.giocatore());
    m.applica({m.tempo(), TipoOrdine::telegrafo, 0.75});   // in moto: la distanza e le velocita'
    reg.metadato("giocatore", std::to_string(m.giocatore()->id));
    reg.metadato("teatro", m.teatro());
    for (int k = 0; k < 9 * 600; ++k) {
        m.avanza(1.0 / 9.0);
        if (reg.serve(m.tempo(), m.eventi_missione().size()))
            reg.aggiungi(fotogramma_da(m, &st, {}, reg.ultimo_evento()), m.eventi_missione().size());
    }
    const ReportMissione r = report_missione(leggi_traccia(reg.dati()), &catalogo());
    CHECK(r.classe == m.giocatore()->classe);
    CHECK(r.teatro == m.teatro());
    CHECK(r.durata_s == doctest::Approx(600.0).epsilon(0.03));
    CHECK(r.distanza_nm > 0.5);
    CHECK(r.velocita_max_kn > 5.0);
    CHECK(r.velocita_media_kn > 0.0);
    CHECK(r.classificati <= r.contatti);
    CHECK(r.giuste + r.errate <= 2 * r.classificati);
    CHECK_FALSE(r.livello_irradiato.empty());
    CHECK(r.contatti >= 0);
}

// docs/26 §3: le voci nuove dagli eventi del registro nella traccia (libretto, telemetro, AOB, missili, ecoscandaglio,
// navigazione, intercettazione, mine, radio, urti), e la loro somma nelle cumulate e nel file
TEST_CASE("statistiche: le voci di docs/26 dagli eventi della traccia") {
    Traccia t = sintetica();
    int id = 1000;
    const auto con = [&](double tempo, const char* tipo) { t.registro.push_back(evento(id++, tempo, tipo)); return &t.registro.back(); };
    auto* v = con(100.0, "classificazione"); v->codice = 2; v->livello = 2; v->valore = 1.0; v->giusta = true; v->unita = 5;
    v = con(110.0, "classificazione"); v->codice = 2; v->livello = 2; v->valore = 1.0; v->giusta = false; v->unita = 6;
    v = con(120.0, "misura_telemetro"); v->valore = 4400.0; v->distanza_m = 4000.0;   // +10%
    v = con(130.0, "misura_telemetro"); v->valore = 3800.0; v->distanza_m = 4000.0;   // -5%
    v = con(140.0, "osservazione_aob"); v->valore = 8.0;
    v = con(150.0, "osservazione_aob"); v->valore = 22.0;
    v = con(160.0, "lancio_giocatore"); v->livello = 2; v->arma = 77; v->valore = 1.0; v->classe_arma = "harpoon_84c";
    v = con(260.0, "colpo"); v->arma = 77; v->altra = 0; v->unita = 6; v->classe_arma = "harpoon_84c";
    con(300.0, "ping_ecoscandaglio");
    con(360.0, "ping_ecoscandaglio");
    v = con(400.0, "punto_nave"); v->giusta = true; v->valore = 900.0;
    v = con(500.0, "punto_nave"); v->giusta = true; v->valore = 300.0;
    v = con(600.0, "allarme_siluro"); v->codice = 1;
    con(700.0, "siluro_evitato");      // dopo l'allarme
    con(2000.0, "siluro_evitato");     // troppo dopo
    con(800.0, "mina_scoperta");
    con(900.0, "attraversamento_mine");
    v = con(1000.0, "verifica_radio"); v->giusta = true;
    v = con(1010.0, "verifica_radio"); v->giusta = false;
    con(1100.0, "collisione");
    con(1110.0, "quasi_collisione");
    con(1120.0, "artiglieria_colpo");
    std::stable_sort(t.registro.begin(), t.registro.end(), [](const EventoTraccia& a, const EventoTraccia& b) { return a.tempo < b.tempo; });
    const ReportMissione r = report_missione(t, &catalogo());
    CHECK(r.visive_giuste == 1);
    CHECK(r.visive_errate == 1);
    CHECK(r.misure_telemetro == 2);
    CHECK(r.errore_telemetro_pct == doctest::Approx(7.5));
    CHECK(r.osservazioni_aob == 2);
    CHECK(r.errore_aob_deg == doctest::Approx(15.0));
    CHECK(r.missili_lanciati == 1);
    CHECK(r.missili_a_segno == 1);
    CHECK(r.neutrali_in_scatola == 1);
    CHECK(r.ping_ecoscandaglio == 2);
    CHECK(r.punti_nave == 2);
    CHECK(r.errore_navigazione_medio_m == doctest::Approx(600.0));
    CHECK(r.errore_navigazione_max_m == doctest::Approx(900.0));
    CHECK(r.allarmi_siluro == 1);
    CHECK(r.siluri_evitati_allarme == 1);
    CHECK(r.mine_scoperte == 1);
    CHECK(r.attraversamenti_mine == 1);
    CHECK(r.verifiche_radio == 2);
    CHECK(r.non_autentici == 1);
    CHECK(r.collisioni == 1);
    CHECK(r.quasi_collisioni == 1);
    CHECK(r.colpi_artiglieria == 1);
    StatisticheCumulate c;
    accumula(c, r);
    accumula(c, r);
    CHECK(c.misure_telemetro == 4);
    CHECK(c.errore_telemetro_pct() == doctest::Approx(7.5));
    CHECK(c.errore_aob_deg() == doctest::Approx(15.0));
    CHECK(c.errore_navigazione_m() == doctest::Approx(600.0));
    CHECK(c.non_autentici == 2);
    const auto f = fs::temp_directory_path() / "m688_prova_statistiche_docs26.toml";
    FileStatistiche fs_;
    fs_.globali = c;
    salva_statistiche(fs_, f);
    const FileStatistiche l = leggi_statistiche(f);
    CHECK(l.globali.misure_telemetro == 4);
    CHECK(l.globali.errore_telemetro_pct() == doctest::Approx(7.5));
    CHECK(l.globali.attraversamenti_mine == 2);
    CHECK(l.globali.colpi_artiglieria == 2);
    fs::remove(f);
}

TEST_CASE("statistiche: una raffica vale un colpo d'artiglieria; una traccia di prima del 05/11/2026, senza gli spari, conta ogni evento") {
    const auto conta = [](const std::vector<int>& spari) {
        Traccia t = sintetica();
        int id = 1000;
        for (const int sparo : spari) {
            t.registro.push_back(evento(id++, 10.0, "artiglieria_colpo"));
            t.registro.back().codice = sparo;
        }
        return report_missione(t, &catalogo()).colpi_artiglieria;
    };
    CHECK(conta({0, 0, 0, 0}) == 4);
    CHECK(conta({7, 7, 7, 8}) == 2);
    CHECK(conta({}) == 0);
}

TEST_CASE("statistiche: il secondo giro (ghiaccio, notte, coda, mine, ASW aereo, soccorso, classi), dai conti e dalla traccia") {
    Traccia t;
    t.missione = "PROVA2";
    t.metadati = {{"giocatore", "0"}, {"teatro", "barents"}, {"conti.sotto_banchisa_s", "900"}, {"conti.emersioni_ghiaccio", "1"},
                  {"conti.urti_ghiaccio", "2"}, {"conti.avvisi_chiglia", "3"}, {"conti.visive_fanali_giuste", "1"},
                  {"conti.visive_luci_spente", "2"}, {"conti.luci_accese_s", "600"}, {"conti.in_coda_s", "1200"},
                  {"conti.classificati_da_sottomarini", "1"}, {"conti.mine_aggirate", "1"}, {"conti.salvati", "12"},
                  {"conti.sensore_mine", "bqs15"}};
    // il 688 fermo nell'origine; un Victor che passa a 300 m; un Mk 46 lanciato da un P-3C; una Krivak (nave) a 100 m
    const auto campione = [](int id, const char* classe, const char* categoria, int flag, Vec2 p, int lanciatore = -1) {
        StatoUnita u;
        u.id = id;
        u.classe = classe;
        u.categoria = categoria;
        u.flag = flag;
        u.pos = p;
        u.lanciatore = lanciatore;
        return u;
    };
    for (double s = 0.0; s <= 600.0; s += 5.0) {
        t.unita[0].emplace_back(s, campione(0, "los_angeles", "sottomarino", 9, {0.0, 0.0}));
        t.unita[2].emplace_back(s, campione(2, "victor3", "sottomarino", 17, {s * 10.0 - 3000.0, 300.0}));
        t.unita[3].emplace_back(s, campione(3, "p3c", "aereo", 9 | 3, {1000.0, 1000.0}));
        t.unita[4].emplace_back(s, campione(4, "mk46", "arma", 0x20, {500.0, 0.0}, 3));
        t.unita[5].emplace_back(s, campione(5, "krivak2", "nave", 18, {100.0, 0.0}));
    }
    const auto ev = [&](const char* tipo, int unita) {
        EventoTraccia e;
        e.id = static_cast<int>(t.registro.size());
        e.tipo = tipo;
        e.unita = unita;
        t.registro.push_back(e);
        return &t.registro.back();
    };
    ev("mina_scoperta", -1)->livello = 1;
    ev("mina_scoperta", -1)->livello = 0;
    ev("aereo_in_sorvolo", 3)->classe = "p3c";
    ev("aereo_in_sorvolo", 6)->classe = "sh60b";
    ev("siluro_evitato", 4);
    auto* c = ev("classificazione", 2);
    c->classe = "victor3";
    c->livello = 2;
    c->valore = 1.0;
    c = ev("classificazione", 5);   // errata: non conta
    c->classe = "krivak2";
    c->livello = 2;
    c->valore = 1.0;
    c->giusta = false;
    const ReportMissione r = report_missione(t);
    CHECK(r.sotto_banchisa_s == doctest::Approx(900.0));
    CHECK(r.emersioni_ghiaccio == 1);
    CHECK(r.urti_ghiaccio == 2);
    CHECK(r.avvisi_chiglia == 3);
    CHECK(r.visive_notte_fanali == 1);
    CHECK(r.visive_luci_spente == 2);
    CHECK(r.luci_accese_s == doctest::Approx(600.0));
    CHECK(r.in_coda_s == doctest::Approx(1200.0));
    CHECK(r.classificati_da_sottomarini == 1);
    CHECK(r.mine_aggirate == 1);
    CHECK(r.salvati == 12);
    CHECK(r.mine_per_sensore.at("bqs15") == 2);
    CHECK(r.mine_ormeggiate == 1);
    CHECK(r.mine_di_fondo == 1);
    CHECK(r.sorvoli_aerei == 1);
    CHECK(r.sorvoli_elicotteri == 1);
    CHECK(r.siluri_aerei_evitati == 1);
    CHECK(r.classi_giuste == std::set<std::string>{"victor3"});
    CHECK(r.distanza_min_sottomarino_m == doctest::Approx(300.0));   // la Krivak a 100 m e' una nave
    // le cumulate e il file
    StatisticheCumulate k;
    accumula(k, r);
    ReportMissione r2 = r;
    r2.classi_giuste = {"krivak2"};
    r2.in_coda_s = 300.0;
    r2.distanza_min_sottomarino_m = 800.0;
    accumula(k, r2);
    CHECK(k.salvati == 24);
    CHECK(k.in_coda_max_s == doctest::Approx(1200.0));
    CHECK(k.distanza_min_sottomarino_m == doctest::Approx(300.0));
    CHECK(k.classi_giuste.size() == 2);
    CHECK(k.mine_per_sensore.at("bqs15") == 4);
    const auto p = fs::temp_directory_path() / "m688_prova_statistiche_secondo_giro.toml";
    FileStatistiche f;
    f.globali = k;
    salva_statistiche(f, p);
    const FileStatistiche g = leggi_statistiche(p);
    CHECK(g.globali.salvati == 24);
    CHECK(g.globali.classi_giuste == k.classi_giuste);
    CHECK(g.globali.mine_per_sensore.at("bqs15") == 4);
    CHECK(g.globali.sotto_banchisa_s == doctest::Approx(1800.0));
    CHECK(g.globali.distanza_min_sottomarino_m == doctest::Approx(300.0));
    fs::remove(p);
}
