#include <doctest.h>

#include <filesystem>
#include <string>

#include "missioni/danni_sottomarini.hpp"
#include "missioni/missione.hpp"
#include "missioni/stazioni_missione.hpp"
#include "nucleo/ordini.hpp"
#include "nucleo/classi.hpp"
#include "nucleo/guasti_giocatore.hpp"
#include "nucleo/sonar_giocatore.hpp"

using namespace m688;

// Stazione Sonar del giocatore (docs/11_comandi_e_stazioni.md, fase C4; docs/originale/sensori_e_navigazione.md §10.4)

TEST_CASE("sonar: schiera filata solo sotto i 3/4, manovre e taglio") {
    StazioneSonar s;
    SituazioneSonar lento{10.0, 30.0}, veloce{22.5, 30.0};   // posizione di velocita' 1 e 3
    CHECK(StazioneSonar::posizione_velocita(15.0, 30.0) == 2);
    CHECK(s.schiera() == StatoSchiera::dentro);
    auto e = s.consegna({0, TipoOrdine::schiera_cala}, veloce);
    CHECK(e.messaggio == MSG_NON_SI_FILA);
    CHECK(e.alla_missione.empty());
    e = s.consegna({0, TipoOrdine::schiera_cala}, lento);
    REQUIRE(e.alla_missione.size() == 1);     // 34h: il gestore della missione
    CHECK(s.manovra() == ManovraSchiera::filando);
    CHECK(s.schiera_esposta());
    CHECK(s.consegna({1, TipoOrdine::schiera_cala}, lento).messaggio == MSG_ANCORA_IN_FILATURA);
    CHECK(s.consegna({1, TipoOrdine::schiera_recupera}, lento).messaggio == MSG_ANCORA_IN_FILATURA);
    s.messaggio_missione(MSG_SCHIERA_FILATA);
    CHECK(s.schiera() == StatoSchiera::filata);
    CHECK(s.manovra() == ManovraSchiera::nessuna);
    // telegrafo: 2/3 passa, 3/4, TUTTA, MAX e l'arresto rapido chiedono conferma
    CHECK_FALSE(s.chiede_conferma({2, TipoOrdine::telegrafo, 0.5}));
    CHECK(s.chiede_conferma({2, TipoOrdine::telegrafo, 0.75}));
    CHECK(s.chiede_conferma({2, TipoOrdine::telegrafo, 1.09375}));
    CHECK(s.chiede_conferma({2, TipoOrdine::telegrafo, 0, 0, 1}));
    e = s.consegna(StazioneSonar::taglio(3), lento);
    CHECK(e.messaggio == MSG_SCHIERA_TAGLIATA);
    REQUIRE(e.alla_missione.size() == 1);
    CHECK(s.schiera() == StatoSchiera::persa);
    CHECK_FALSE(s.chiede_conferma({4, TipoOrdine::telegrafo, 1.0}));
    CHECK(s.consegna({5, TipoOrdine::schiera_cala}, lento).messaggio == MSG_SCHIERA_GIA_TAGLIATA);
    CHECK(s.consegna({5, TipoOrdine::schiera_taglia}, lento).alla_missione.empty());
}

TEST_CASE("sonar: filtro, impulso attivo e ANALYZE") {
    StazioneSonar s;
    CHECK(s.consegna({0, TipoOrdine::filtro_sonar, 0, 0, 1}, {}).messaggio == MSG_PASSA_ALTO);
    CHECK(s.filtro());
    s.guasto_calcolatore();
    CHECK_FALSE(s.filtro());
    CHECK(s.consegna({0, TipoOrdine::filtro_sonar, 0, 0, 0}, {}).messaggio == MSG_PASSA_BANDA);
    auto e = s.consegna({0, TipoOrdine::impulso_sonar}, {});
    CHECK(e.messaggio == MSG_PING);
    CHECK(e.alla_missione.size() == 1);
    SituazioneSonar x;
    CHECK(s.analizza(x) == MSG_SONAR_SERVE_BERSAGLIO);
    x.bersaglio = true;
    CHECK(s.analizza(x) == MSG_CONTATTO_PERSO);
    x.contatto = true;
    CHECK(s.analizza(x) == MSG_ANALISI);
    x.aereo = true;
    CHECK(s.analizza(x) == MSG_NO_ANALISI_AEREI);
}

namespace {
const std::string SCENA = std::string(M688_MISSIONI) + "/SCENE1A.lua";
bool c_e_scena() { return std::filesystem::exists(SCENA); }
}  // namespace

TEST_CASE("sonar: la schiera nella missione, filata in 60 s e recuperata in 40 s dagli script" * doctest::skip(!c_e_scena())) {
    Missione m(SCENA, 1);
    StazioniMissione st(m);
    RegistroOrdini r;
    auto avanza = [&](double secondi) {
        for (int i = 0; i < static_cast<int>(secondi * 9.0 + 0.5); ++i) {
            for (const auto& o : r.consegna(m.tempo())) st.consegna(m, o);
            m.avanza(1.0 / 9.0);
            st.dopo_passo(m);
        }
    };
    const auto& g = *m.giocatore();
    const double sonar0 = g.sonar;
    r.ordina({0, TipoOrdine::telegrafo, 0.25});
    r.ordina({0, TipoOrdine::quota_150_piedi});   // sotto i 100 ft, dove TUTTA e' permessa
    r.ordina({0.5, TipoOrdine::schiera_cala});
    avanza(59.0);
    CHECK(st.sonar().manovra() == ManovraSchiera::filando);
    CHECK((g.flag54 & 1) == 0);
    avanza(2.0);
    CHECK(st.sonar().schiera() == StatoSchiera::filata);
    CHECK((g.flag54 & 1) != 0);              // ascolto a +-90 gradi
    CHECK(g.sonar == doctest::Approx(sonar0 + 2));
    r.ordina({m.tempo(), TipoOrdine::schiera_recupera});
    avanza(41.0);
    CHECK(st.sonar().schiera() == StatoSchiera::dentro);
    CHECK((g.flag54 & 1) == 0);
    CHECK(g.sonar == doctest::Approx(sonar0));
    // filata di nuovo e tagliata per andare a TUTTA: 36h, la missione la toglie
    r.ordina({m.tempo(), TipoOrdine::schiera_cala});
    avanza(61.0);
    REQUIRE(st.sonar().schiera() == StatoSchiera::filata);
    const Ordine tutta{m.tempo(), TipoOrdine::telegrafo, 1.0};
    REQUIRE(st.sonar().chiede_conferma(tutta));
    r.ordina(StazioneSonar::taglio(m.tempo()));
    r.ordina(tutta);
    avanza(1.0);
    CHECK(st.sonar().schiera() == StatoSchiera::persa);
    CHECK((g.flag54 & 1) == 0);
    CHECK(g.velocita_ordinata > 0.9 * g.avanti_tutta);
    CHECK(m.errori().empty());
}

TEST_CASE("guasti: rand() di Borland C e un tiro per messaggio 20, 26 o 30") {
    RandBorland r(1);   // srand(1): la sequenza nota del Borland C
    CHECK(r() == 346);
    CHECK(r() == 130);
    CHECK(r() == 10982);
    GuastiGiocatore g(7);
    CHECK(g.messaggio(19) == -1);           // altri messaggi: nessun tiro
    int guasti = 0;
    for (int i = 0; i < 400; ++i)
        if (g.messaggio(MSG_COLPITI) >= 0) ++guasti;
    CHECK(guasti == SISTEMI);               // ognuno una volta sola; poi niente, senza riprovare
    for (int i = 0; i < SISTEMI; ++i) CHECK(g.guasto(static_cast<SistemaClassico>(i)));
}

TEST_CASE("guasti: effetti e rifiuti nella missione" * doctest::skip(!c_e_scena())) {
    Missione m(SCENA, 1);
    StazioniMissione st(m, 3);
    const auto& g = *m.giocatore();
    REQUIRE(m.applica({0, TipoOrdine::periscopio, 0, 0, 1}));
    REQUIRE((g.flag54 & 2) != 0);
    for (int i = 0; i < 400 && !st.guasti().guasto(SistemaClassico::periscopio); ++i) {   // colpi finche' il periscopio si guasta
        m.messaggio(g.id, MSG_COLPITI);
        st.dopo_passo(m);
    }
    REQUIRE(st.guasti().guasto(SistemaClassico::periscopio));
    CHECK((g.flag54 & 2) == 0);             // abbassato (32h)
    CHECK_FALSE(st.consegna(m, {0, TipoOrdine::periscopio, 0, 0, 1}));
    CHECK(st.messaggi().back().codice == GuastiGiocatore::annuncio(SistemaClassico::periscopio));
    for (int i = 0; i < 400; ++i) { m.messaggio(g.id, MSG_FONDO); st.dopo_passo(m); }
    CHECK(st.rifiuto_per_guasto(m, {0, TipoOrdine::impulso_sonar}) == GuastiGiocatore::annuncio(SistemaClassico::sonar_di_tiro));
    CHECK(st.rifiuto_per_guasto(m, {0, TipoOrdine::telegrafo, 1.09375}) == MSG_MOTORI_NON_REGGONO);
    CHECK(st.rifiuto_per_guasto(m, {0, TipoOrdine::telegrafo, 0, 0, 1}) == MSG_MOTORI_NON_REGGONO);
    CHECK(st.rifiuto_per_guasto(m, {0, TipoOrdine::telegrafo, 1.0}) == 0);   // TUTTA resta
    CHECK(st.analizza(m) == GuastiGiocatore::annuncio(SistemaClassico::calcolatore_sonar));
    CHECK(st.percento_operativo(m) == 100);
}

TEST_CASE("guasti: nell'Enhanced vengono dal battello danneggiato, non dal tiro dell'originale" * doctest::skip(!c_e_scena())) {
    const std::string dati = M688_DATI;
    if (!std::filesystem::exists(dati + "/classi.toml")) return;
    const auto classi = carica_classi(dati + "/classi.toml");
    Missione m(SCENA, 1);
    DanniSottomarini ds(classi, dati, 1);
    StazioniMissione st(m, 3);
    st.usa_danni(&ds);
    const auto& g = *m.giocatore();
    for (int i = 0; i < 200; ++i) { m.messaggio(g.id, MSG_COLPITI); st.dopo_passo(m); }   // nessun tiro 1/2
    for (int i = 0; i < SISTEMI; ++i) CHECK_FALSE(st.fuori_uso(m, static_cast<SistemaClassico>(i)));
    CHECK(st.rifiuto_per_guasto(m, {0, TipoOrdine::telegrafo, 1.09375}) == 0);   // battello intatto: tutto concesso
    CHECK(st.rifiuto_per_guasto(m, {0, TipoOrdine::timone, 3.6}) == 0);
    CHECK(st.rifiuto_per_guasto(m, {0, TipoOrdine::periscopio, 0, 0, 1}) == 0);
}
