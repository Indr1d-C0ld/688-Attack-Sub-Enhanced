#include <doctest.h>

#include <cmath>
#include <filesystem>
#include <string>

#include "missioni/stazioni_missione.hpp"
#include "missioni/missione.hpp"
#include "nucleo/armi_giocatore.hpp"
#include "nucleo/ordini.hpp"

using namespace m688;

// Stazione Armi del giocatore (docs/11_comandi_e_stazioni.md, fase C3; docs/originale/sensori_e_navigazione.md §10)

namespace {
const std::vector<std::string> TUBI_688 = {"mk48", "mk48", "mk48", "mk48", "esca", "esca", "harpoon"};
const std::map<std::string, int> DOTAZIONE = {{"mk48", 2}, {"harpoon", 2}, {"esca", 6}};

// arma il tubo con i due clic (il primo se non e' gia' selezionato) e la fine del timer dello script
void arma(StazioneArmi& s, int tubo) {
    if (s.selezionato() != tubo) s.clic_tubo(tubo, 0.0);
    auto e = s.clic_tubo(tubo, 0.0);
    REQUIRE(e.alla_missione.size() == 1);
    s.consegna(e.alla_missione.front(), {});
    s.messaggio_missione(9 + tubo);
}

SituazioneArmi nave_lontana() {
    SituazioneArmi x;
    x.quota = 6.0;
    x.bersaglio_presente = x.bersaglio_nave = x.contatto = x.traccia_solida = true;
    x.distanza_m = 6.0 * 1852.0;
    return x;
}
}  // namespace

TEST_CASE("armi: armamento in due tempi, tubo liberato e scorte") {
    StazioneArmi s(TUBI_688, DOTAZIONE, 7, 4);
    CHECK(s.stato(2) == StatoTubo::vuoto);
    auto e = s.clic_tubo(2, 1.0);             // primo clic: seleziona
    CHECK(e.alla_missione.empty());
    CHECK(e.messaggio == MSG_TUBO_SILURI);
    CHECK(s.selezionato() == 2);
    e = s.clic_tubo(2, 1.0);                  // secondo clic: ordine di armamento
    REQUIRE(e.alla_missione.size() == 1);
    CHECK(e.alla_missione[0].tipo == TipoOrdine::carica_tubo);
    CHECK(e.alla_missione[0].n == 2);
    e = s.consegna(e.alla_missione[0], {});
    CHECK(s.stato(2) == StatoTubo::in_armamento);
    CHECK(s.scorte(Arma::mk48) == 1);
    CHECK(e.messaggio == MSG_IN_ARMAMENTO);
    REQUIRE(e.alla_missione.size() == 1);     // 1Dh + tubo: il gestore carica_tubo_2 della missione
    CHECK(s.messaggio_missione(11) == MSG_ARMATO);
    CHECK(s.stato(2) == StatoTubo::armato);
    // di nuovo: il tubo si libera e l'arma torna nelle scorte
    e = s.consegna({2.0, TipoOrdine::carica_tubo, 0, 0, 2}, {});
    CHECK(s.stato(2) == StatoTubo::vuoto);
    CHECK(s.scorte(Arma::mk48) == 2);
    CHECK(e.messaggio == MSG_TUBO_LIBERO);
    CHECK(e.alla_missione.empty());
    // liberato durante l'armamento: il timer dello script che arriva dopo non lo arma
    s.consegna({3.0, TipoOrdine::carica_tubo, 0, 0, 3}, {});
    s.consegna({4.0, TipoOrdine::carica_tubo, 0, 0, 3}, {});
    CHECK(s.messaggio_missione(12) == 0);
    CHECK(s.stato(3) == StatoTubo::vuoto);
    // esca e missile: messaggio secondo il tipo d'arma, non quello del tubo
    CHECK(s.clic_tubo(5, 5.0).messaggio == MSG_TUBO_AUSILIARIO);
    arma(s, 5);
    CHECK(s.stato(5) == StatoTubo::armato);
    s.consegna({6.0, TipoOrdine::carica_tubo, 0, 0, 7}, {});
    CHECK(s.messaggio_missione(16) == MSG_ARMATO + 4);   // Harpoon: 14, «missile armato»
    // scorte finite
    arma(s, 1);
    arma(s, 4);
    CHECK(s.scorte(Arma::mk48) == 0);
    s.clic_tubo(3, 7.0);
    CHECK(s.clic_tubo(3, 7.0).messaggio == MSG_ARMA_ESAURITA);
}

TEST_CASE("armi: lancio, ricarica automatica e limite delle armi in acqua") {
    StazioneArmi s({"mk48", "mk48", "mk48", "mk48", "mk48", "esca", "esca"}, {{"mk48", 8}, {"esca", 2}}, 6, 4);
    CHECK(s.consegna({0, TipoOrdine::lancia, 0, 0, 1}, {}).messaggio == MSG_NON_ARMATA);
    for (int t = 1; t <= 5; ++t) arma(s, t);
    CHECK(s.scorte(Arma::mk48) == 3);
    s.consegna({0, TipoOrdine::designa_bersaglio, 0, 0, 42}, {});
    auto e = s.consegna({1, TipoOrdine::lancia, 0, 0, 1}, {});
    REQUIRE(e.alla_missione.size() == 3);   // 38h con il bersaglio, 24h + tipo, 1Dh + tubo (ricarica)
    CHECK(e.alla_missione[0].tipo == TipoOrdine::designa_bersaglio);
    CHECK(e.alla_missione[0].n == 42);
    CHECK(e.alla_missione[1].tipo == TipoOrdine::lancia);
    CHECK(e.alla_missione[2].tipo == TipoOrdine::carica_tubo);
    CHECK(s.stato(1) == StatoTubo::in_armamento);
    CHECK(s.scorte(Arma::mk48) == 2);
    CHECK(s.selezionato() == 2);            // il prossimo tubo armato con la stessa arma
    CHECK(s.in_acqua() == 1);
    for (int t = 2; t <= 4; ++t) CHECK(s.consegna({2, TipoOrdine::lancia, 0, 0, t}, {}).messaggio == 0);
    CHECK(s.in_acqua() == 4);
    CHECK(s.consegna({3, TipoOrdine::lancia, 0, 0, 5}, {}).messaggio == MSG_NON_ANCORA);   // nessun intervallo, ma il limite
    CHECK(s.stato(5) == StatoTubo::armato);
    s.unita_lanciate({100, 101, 102, 103});
    s.unita_lanciate({100, 102, 103});      // un siluro esce dal gioco: si libera un posto
    CHECK(s.in_acqua() == 3);
    CHECK(s.consegna({4, TipoOrdine::lancia, 0, 0, 5}, {}).messaggio == 0);
    CHECK(s.stato(5) == StatoTubo::vuoto);  // scorte finite: niente ricarica
}

TEST_CASE("armi: tubo selezionato dopo armamento e lancio (7AF1)") {
    StazioneArmi s(TUBI_688, DOTAZIONE, 7, 4);
    // seleziona il 5 (esca) e arma il 2: il selezionato non e' armato, ma non c'e' un'esca armata -> torna sul 5
    s.consegna({0, TipoOrdine::carica_tubo, 0, 0, 2}, {});
    s.clic_tubo(5, 0);
    s.messaggio_missione(11);
    CHECK(s.selezionato() == 5);
    // arma l'esca del 6 con il 5 selezionato e vuoto: passa al 6, il primo armato con l'arma del selezionato
    s.consegna({0, TipoOrdine::carica_tubo, 0, 0, 6}, {});
    s.messaggio_missione(15);
    CHECK(s.selezionato() == 6);
    // lancio dal 2, unico Mk 48 armato: in armamento per la ricarica, dopo N passi la selezione torna sul 2
    s.consegna({0, TipoOrdine::lancia, 0, 0, 2}, {});
    CHECK(s.stato(2) == StatoTubo::in_armamento);
    CHECK(s.selezionato() == 2);
}

TEST_CASE("armi: controlli dei missili del tubo 7 nell'ordine dell'originale") {
    StazioneArmi s(TUBI_688, DOTAZIONE, 7, 4);
    arma(s, 7);
    auto lancia = [&](const SituazioneArmi& x) { return s.consegna({1, TipoOrdine::lancia, 0, 0, 7}, x); };
    CHECK(lancia(nave_lontana()).messaggio == MSG_SERVE_BERSAGLIO);   // nessun bersaglio selezionato
    s.consegna({0, TipoOrdine::designa_bersaglio, 0, 0, 9}, {});
    auto x = nave_lontana();
    x.bersaglio_nave = false;
    CHECK(lancia(x).messaggio == MSG_SOLO_SUPERFICIE);
    x = nave_lontana();
    x.contatto = x.traccia_solida = false;
    auto e = lancia(x);
    CHECK(e.messaggio == 0);                 // senza contatto: rifiuto in silenzio
    CHECK(e.alla_missione.empty());
    x.contatto = true;
    CHECK(lancia(x).messaggio == MSG_SERVE_DISTANZA);
    x = nave_lontana();
    x.distanza_m = 4.95 * 1852.0;
    CHECK(lancia(x).messaggio == MSG_TROPPO_VICINO);
    x = nave_lontana();
    x.quota = 9.0;
    CHECK(lancia(x).messaggio == MSG_QUOTA_PERISCOPICA);
    e = lancia(nave_lontana());
    CHECK(e.messaggio == 0);
    CHECK(e.alla_missione.size() == 3);
    CHECK(s.in_acqua() == 0);                // i missili del tubo 7 non contano
}

TEST_CASE("armi: guida a filo solo per i propri siluri collegati") {
    StazioneArmi s(TUBI_688, DOTAZIONE, 7, 4);
    SituazioneArmi x;
    CHECK(s.consegna({0, TipoOrdine::guida_cerca}, x).messaggio == MSG_NESSUNA_ARMA);
    s.consegna({0, TipoOrdine::seleziona_siluro, 0, 0, 77}, x);
    x.arma_presente = true;
    CHECK(s.consegna({0, TipoOrdine::guida_cerca}, x).messaggio == MSG_NON_GUIDATA);
    x.arma_guidata = true;
    auto e = s.consegna({0, TipoOrdine::guida_punto, 100, 200}, x);
    CHECK(e.messaggio == MSG_ARMA_DIRETTA);
    REQUIRE(e.alla_missione.size() == 2);   // 38h con l'arma, poi il comando
    CHECK(e.alla_missione[0].tipo == TipoOrdine::seleziona_siluro);
    CHECK(e.alla_missione[0].n == 77);
    CHECK(e.alla_missione[1].a == 100);
}

namespace {
const std::string SCENA = std::string(M688_MISSIONI) + "/SCENE1A.lua";
bool c_e_scena() { return std::filesystem::exists(SCENA); }

struct Partita {
    Missione m{SCENA, 1};
    StazioniMissione st{m};
    RegistroOrdini ordini;
    void avanza(double s) {
        for (int i = 0; i < static_cast<int>(std::lround(s * 9.0)); ++i) {
            for (const auto& o : ordini.consegna(m.tempo())) st.consegna(m, o);
            m.avanza(1.0 / 9.0);
            st.dopo_passo(m);
        }
    }
    void clic(int tubo) {
        for (const auto& o : st.armi().clic_tubo(tubo, m.tempo()).alla_missione) ordini.ordina(o);
    }
    void arma(int tubo) {   // selezione (se serve) e secondo clic
        if (st.armi().selezionato() != tubo) clic(tubo);
        clic(tubo);
    }
    bool messaggio(int codice) const {
        for (const auto& x : m.messaggi()) if (x.codice == codice) return true;
        for (const auto& x : st.messaggi()) if (x.codice == codice) return true;
        return false;
    }
    int siluro() const {
        for (const auto& [id, u] : m.unita()) if (u.classe == "mk48" && u.lanciatore == m.giocatore()->id) return id;
        return -1;
    }
};
}  // namespace

TEST_CASE("armi: un Mk 48 del giocatore armato in 20 s colpisce una nave con la regola dell'originale" * doctest::skip(!c_e_scena())) {
    Partita p;
    CHECK(p.st.armi().scorte(Arma::mk48) == 22);   // dotazione della missione
    CHECK(p.st.armi().scorte(Arma::harpoon) == 2);
    CHECK(p.st.armi().numero_tubi() == 7);
    const auto& g = *p.m.giocatore();
    int bersaglio = -1;
    for (const auto& [id, u] : p.m.unita()) if (u.categoria == "nave") { bersaglio = id; break; }
    REQUIRE(bersaglio >= 0);
    // la nave a 2 km di prua, ferma
    auto* b = p.m.trova(bersaglio);
    const double r = radianti(g.rotta);
    b->pos = {g.pos.x + 2000.0 * std::sin(r), g.pos.y + 2000.0 * std::cos(r)};
    b->velocita = b->velocita_ordinata = 0.0;
    p.ordini.ordina({0, TipoOrdine::designa_bersaglio, 0, 0, bersaglio});
    p.arma(1);
    p.avanza(19.0);
    CHECK(p.st.armi().stato(1) == StatoTubo::in_armamento);
    p.avanza(2.0);
    CHECK(p.st.armi().stato(1) == StatoTubo::armato);
    CHECK(p.messaggio(MSG_ARMATO));
    p.ordini.ordina(p.st.armi().lancio(p.m.tempo()));
    p.avanza(0.5);
    const int s = p.siluro();
    REQUIRE(s >= 0);
    CHECK(p.messaggio(2));                                         // siluro lanciato
    CHECK(p.st.armi().in_acqua() == 1);
    CHECK(p.st.armi().stato(1) == StatoTubo::in_armamento);   // ricarica automatica
    bool colpita = false;
    for (int i = 0; i < 120 && !colpita; ++i) {
        p.avanza(1.0);
        for (const auto& c : p.m.colpi()) colpita = colpita || (c.arma == s && c.vittima == bersaglio && c.lanciatore == g.id);
    }
    CHECK(colpita);
    CHECK(p.m.unita().count(s) == 0);
    CHECK(p.st.armi().in_acqua() == 0);                     // l'arma uscita dal gioco libera il posto
    CHECK(p.m.errori().empty());
}

TEST_CASE("armi: filo tagliato a 190 s e fine corsa a 280 s, come negli script" * doctest::skip(!c_e_scena())) {
    Partita p;
    for (const auto& [id, u] : p.m.unita())   // nessuno a tiro: il siluro corre fino alla fine
        if (id != p.m.giocatore()->id) p.m.trova(id)->pos.x += 400000.0;
    p.arma(1);
    p.avanza(21.0);
    REQUIRE(p.st.armi().stato(1) == StatoTubo::armato);
    const double t0 = p.m.tempo();
    p.ordini.ordina(p.st.armi().lancio(t0));
    p.avanza(0.5);
    const int s = p.siluro();
    REQUIRE(s >= 0);
    CHECK(p.m.unita().at(s).collegata == p.m.giocatore()->id);
    // DIRECT verso un punto 10 km piu' avanti, lontano dal battello: senza filo il siluro che cerca e gira in tondo
    // non distingue piu' il lanciatore e potrebbe tornare su di noi (lo fa, con i messaggi 67 e 26)
    const auto& g = *p.m.giocatore();
    p.ordini.ordina({p.m.tempo(), TipoOrdine::seleziona_siluro, 0, 0, s});
    p.ordini.ordina({p.m.tempo(), TipoOrdine::guida_punto, g.pos.x + 10000.0 * std::sin(radianti(g.rotta + 40.0)),
                     g.pos.y + 10000.0 * std::cos(radianti(g.rotta + 40.0))});
    p.avanza(1.0);
    CHECK(p.st.messaggi().back().codice == MSG_ARMA_DIRETTA);
    p.avanza(184.0);
    REQUIRE(p.m.unita().count(s) == 1);
    CHECK(p.m.unita().at(s).collegata == p.m.giocatore()->id);
    p.avanza(10.0);                                                // 190 s dal lancio
    CHECK(p.messaggio(23));                                        // filo tranciato
    CHECK(p.m.unita().at(s).collegata < 0);
    p.ordini.ordina({p.m.tempo(), TipoOrdine::guida_cerca});
    p.avanza(0.5);
    CHECK(p.st.messaggi().back().codice == MSG_NON_GUIDATA);
    for (int i = 0; i < 120 && p.m.unita().count(s); ++i) p.avanza(1.0);
    CHECK(p.m.unita().count(s) == 0);
    CHECK(p.m.tempo() - t0 == doctest::Approx(280.0).epsilon(0.02));
    CHECK(p.messaggio(24));                                        // a fine corsa
    CHECK(p.st.armi().in_acqua() == 0);
    CHECK(p.m.errori().empty());
}

TEST_CASE("armi: impostazioni dei siluri per tubo (Enhanced)") {
    StazioneArmi s(TUBI_688, DOTAZIONE, 7, 4);
    const auto imposta = [&](int tubo, CampoSiluro c, double v) {
        CHECK(s.riguarda({0, TipoOrdine::imposta_siluro, v, static_cast<double>(c), tubo}));
        s.consegna({0, TipoOrdine::imposta_siluro, v, static_cast<double>(c), tubo}, {});
    };
    imposta(2, CampoSiluro::rotta, 250.0);
    imposta(2, CampoSiluro::modo, 1);
    imposta(2, CampoSiluro::velocita, 0);
    imposta(2, CampoSiluro::rte, 4500.0);
    imposta(2, CampoSiluro::soffitto, 30.0);
    imposta(2, CampoSiluro::schema, 2);
    imposta(2, CampoSiluro::anti_circolare, 0);
    imposta(2, CampoSiluro::modo, 9);           // non ammesso (applica di siluro.hpp): resta attivo
    const auto& i = s.impostazioni(2);
    CHECK(i.rotta == doctest::Approx(250.0));
    CHECK(i.modo == ModoTesta::attivo);
    CHECK(i.velocita == VelocitaSiluro::bassa);
    CHECK(i.rte_m == 4500.0);
    CHECK(i.soffitto_m == 30.0);
    CHECK(i.schema == SchemaRicerca::dritto);
    CHECK_FALSE(i.anti_circolare);
    CHECK(s.impostazioni(1).modo == ModoTesta::passivo);   // gli altri tubi non cambiano
}
