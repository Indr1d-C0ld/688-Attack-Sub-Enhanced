#include <doctest.h>

#include <cmath>
#include <filesystem>
#include <memory>
#include <string>

#include "missioni/danni_sottomarini.hpp"
#include "missioni/missione.hpp"
#include "missioni/stazioni_missione.hpp"
#include "nucleo/classi.hpp"
#include "nucleo/comando.hpp"
#include "nucleo/dinamica_sottomarino.hpp"
#include "nucleo/fondale.hpp"
#include "nucleo/siluro.hpp"

using namespace m688;

// Comandi del giocatore nell'Enhanced (docs/11 §10, fase C6c): posto di comando e traduzione per la dinamica (C6a)

TEST_CASE("enhanced: posto di comando con barra in gradi, campane e quote") {
    PostoEnhanced p([](double kn) { return kn / 32.0; }, 32.0);
    StatoProprio s{0.0, 40.0};
    p.imposta(PostoEnhanced::STOP, 0, 40.0);
    for (int i = 0; i < 9; ++i) p.azione(Azione::timone_dritta, s);
    CHECK(p.barra() == PostoEnhanced::BARRA_MAX);            // fino a 35 a scatti di 5
    auto o = p.azione(Azione::timone_centro, s);
    REQUIRE(o);
    CHECK(o->tipo == TipoOrdine::barra);
    CHECK(o->a == 0.0);
    o = p.azione(Azione::telegrafo_avanti, s);                // AV. 1/3: 5 nodi
    REQUIRE(o);
    CHECK(o->tipo == TipoOrdine::giri);
    CHECK(o->a == doctest::Approx(5.0 / 32.0));
    for (int i = 0; i < 6; ++i) p.azione(Azione::telegrafo_avanti, s);
    CHECK(PostoEnhanced::nome_campana(p.campana()) == "MASSIMA");
    for (int i = 0; i < 10; ++i) o = p.azione(Azione::telegrafo_indietro, s);
    CHECK(p.campana() == 0);                                  // IND. TUTTA
    o = p.azione(Azione::barra_giu, s);
    REQUIRE(o);
    CHECK(o->tipo == TipoOrdine::quota_con_angolo);
    CHECK(o->a == doctest::Approx(55.0));
    CHECK(o->b == doctest::Approx(PostoEnhanced::ANGOLO_MAX));
    CHECK(p.azione(Azione::quota_periscopica, s)->a == doctest::Approx(PostoEnhanced::QUOTA_PERISCOPICA_M));
    CHECK_FALSE(p.azione(Azione::periscopio, s));             // resta del classico
    p.osserva({0, TipoOrdine::giri, 15.0 / 32.0});            // un replay: la campana piu' vicina
    CHECK(PostoEnhanced::nome_campana(p.campana()) == "STANDARD");
}

namespace {
const std::string SCENA = std::string(M688_MISSIONI) + "/SCENE1A.lua";
bool ci_sono() { return std::filesystem::exists(SCENA) && std::filesystem::exists(std::string(M688_DATI) + "/classi.toml"); }
}  // namespace

TEST_CASE("enhanced: gli ordini arrivano alla dinamica del giocatore, nel classico no" * doctest::skip(!ci_sono())) {
    const auto classi = carica_classi(std::string(M688_DATI) + "/classi.toml");
    {   // classico: nessuna dinamica, ordini dell'Enhanced ignorati
        Missione m(SCENA, 1);
        StazioniMissione st(m);
        CHECK_FALSE(st.consegna(m, {0, TipoOrdine::giri, 0.5}));
        CHECK_FALSE(st.comandi_dinamica());
    }
    Missione m(SCENA, 1);
    m.dinamica_sottomarini(classi, M688_DATI);
    StazioniMissione st(m);
    const int id = m.giocatore()->id;
    m.avanza(1.0 / 9.0);   // il modello nasce al primo passo
    REQUIRE(m.dinamica(id));
    const double q0 = m.dinamica(id)->quota_m();
    CHECK(st.consegna(m, {0, TipoOrdine::giri, 0.6}));
    CHECK(st.consegna(m, {0, TipoOrdine::barra, 20.0}));
    CHECK(st.consegna(m, {0, TipoOrdine::quota_con_angolo, q0 + 40.0, 15.0}));
    REQUIRE(st.comandi_dinamica());
    CHECK(st.comandi_dinamica()->giri == doctest::Approx(0.6));   // lo stato intero, non solo l'ultimo campo
    CHECK(st.comandi_dinamica()->barra_deg == doctest::Approx(20.0));
    const double rotta0 = m.dinamica(id)->rotta();
    for (int i = 0; i < 9 * 300; ++i) m.avanza(1.0 / 9.0);   // un battello di 7000 t accelera piano (~0,08 kn/s)
    const auto* d = m.dinamica(id);
    CHECK(d->velocita_kn() > 5.0);
    CHECK(d->quota_m() > q0 + 20.0);                                 // scende verso la quota ordinata
    CHECK(std::fabs(normalizza_180(d->rotta() - rotta0)) > 30.0);   // accosta a dritta
    // il pilota automatico rimette il timoniere; la barra lo disinserisce e ridà i comandi al giocatore
    const Vec2 p = m.giocatore()->pos;
    CHECK(st.consegna(m, {m.tempo(), TipoOrdine::punto_di_rotta, p.x + 3000.0, p.y + 3000.0}));
    CHECK(st.consegna(m, {m.tempo(), TipoOrdine::pilota_automatico, 0, 0, 1}));
    CHECK(m.pilota_automatico());
    CHECK_FALSE(st.comandi_dinamica());
    CHECK(st.consegna(m, {m.tempo(), TipoOrdine::barra, -10.0}));
    CHECK_FALSE(m.pilota_automatico());
    REQUIRE(st.comandi_dinamica());
    CHECK(st.comandi_dinamica()->barra_deg == doctest::Approx(-10.0));
    CHECK(m.errori().empty());
}

TEST_CASE("enhanced: posarsi sul fondo, con i rifiuti dell'equipaggio" * doctest::skip(!ci_sono())) {
    const auto classi = carica_classi(std::string(M688_DATI) + "/classi.toml");
    Missione m(SCENA, 1);
    m.imposta_fondale(std::make_shared<const Fondale>(Fondale::carica(std::string(M688_DATI) + "/teatri/" + m.teatro() + ".fdl")));
    m.dinamica_sottomarini(classi, M688_DATI);
    StazioniMissione st(m);
    m.avanza(1.0 / 9.0);
    const auto& g = *m.giocatore();
    // un punto pianeggiante e non troppo profondo vicino al battello (pendenza sotto i 5 gradi, fondo fra 60 e 250 m)
    bool trovato = false;
    for (int i = -20; i <= 20 && !trovato; ++i)
        for (int j = -20; j <= 20 && !trovato; ++j) {
            const Vec2 p{g.pos.x + 500.0 * i, g.pos.y + 500.0 * j};
            const double f = m.profondita(p);
            if (!(f > 60.0 && f < 250.0)) continue;
            const double gx = (m.profondita({p.x + 50, p.y}) - m.profondita({p.x - 50, p.y})) / 100.0;
            const double gy = (m.profondita({p.x, p.y + 50}) - m.profondita({p.x, p.y - 50})) / 100.0;
            if (std::hypot(gx, gy) > 0.03) continue;
            m.trova(g.id)->pos = p;
            trovato = true;
        }
    REQUIRE(trovato);
    m.avanza(1.0 / 9.0);
    if (m.profondita(g.pos) > 100.0) CHECK(st.posarsi(m, 100.0).messaggio == MSG_POSARSI_PROFONDO);   // oltre il collaudo
    st.consegna(m, {m.tempo(), TipoOrdine::schiera_cala});   // schiera in filatura: rifiuto
    CHECK(st.posarsi(m, 290.0).messaggio == MSG_POSARSI_SCHIERA);
    StazioniMissione libera(m);
    const EsitoStazione e = libera.posarsi(m, 290.0);
    REQUIRE(e.messaggio == MSG_POSATI);
    REQUIRE(e.alla_missione.size() == 2);
    for (const auto& o : e.alla_missione) CHECK(libera.consegna(m, o));
    const double fondo = m.profondita(g.pos);
    for (int i = 0; i < 9 * 900; ++i) {
        m.avanza(1.0 / 9.0);
        libera.dopo_passo(m);
    }
    const auto* d = m.dinamica(g.id);
    REQUIRE(d);
    CHECK(d->quota_m() > m.profondita(m.giocatore()->pos) - 6.0);   // a un passo dal fondo, dove si e' fermato
    CHECK(d->giri() == 0.0);                                        // macchine ferme
    (void)fondo;
    CHECK(d->velocita_kn() < 2.0);       // fermo
    CHECK(m.errori().empty());
}

TEST_CASE("enhanced: lancio con le impostazioni del tubo, rotta via filo e spegnimento" * doctest::skip(!ci_sono())) {
    Missione m(SCENA, 1);
    m.siluri_enhanced(M688_DATI);
    StazioniMissione st(m);
    RegistroOrdini r;
    auto avanza = [&](double s) {
        for (int i = 0; i < static_cast<int>(s * 9.0 + 0.5); ++i) {
            for (const auto& o : r.consegna(m.tempo())) st.consegna(m, o);
            m.avanza(1.0 / 9.0);
            st.dopo_passo(m);
        }
    };
    for (const auto& o : st.armi().clic_tubo(1, 0).alla_missione) r.ordina(o);   // il tubo 1 e' gia' selezionato
    r.ordina({0, TipoOrdine::imposta_siluro, 90.0, static_cast<double>(CampoSiluro::rotta), 1});
    r.ordina({0, TipoOrdine::imposta_siluro, 0.0, static_cast<double>(CampoSiluro::modo), 1});
    r.ordina({0, TipoOrdine::imposta_siluro, 8000.0, static_cast<double>(CampoSiluro::rte), 1});
    avanza(21.0);
    REQUIRE(st.armi().stato(1) == StatoTubo::armato);
    REQUIRE(st.armi().impostato(1));
    r.ordina(st.armi().lancio(m.tempo()));
    avanza(1.0);
    int s = -1;
    for (const auto& [id, u] : m.unita())
        if (u.lanciatore == m.giocatore()->id && m.siluro(id)) s = id;
    REQUIRE(s >= 0);
    CHECK(m.siluro(s)->impostazioni().rotta == doctest::Approx(90.0));   // le impostazioni del tubo
    CHECK(m.siluro(s)->impostazioni().rte_m == doctest::Approx(8000.0));
    avanza(15.0);
    r.ordina({m.tempo(), TipoOrdine::comando_filo, 130.0, static_cast<double>(ComandoFiloTipo::rotta), s});
    avanza(15.0);
    CHECK(m.unita().at(s).rotta == doctest::Approx(130.0).epsilon(0.02));
    r.ordina({m.tempo(), TipoOrdine::comando_filo, 120.0, static_cast<double>(ComandoFiloTipo::quota), s});
    avanza(30.0);
    CHECK(m.siluro(s)->quota() == doctest::Approx(120.0).epsilon(0.05));   // quota via filo
    r.ordina({m.tempo(), TipoOrdine::spegni_siluro, 0, 0, s});
    avanza(1.0);
    CHECK(m.unita().count(s) == 0);   // spento: affonda senza esplodere
    bool spento = false;
    for (const auto& x : st.messaggi()) spento = spento || x.codice == MSG_SILURO_SPENTO;
    CHECK(spento);
    r.ordina({m.tempo(), TipoOrdine::taglia_filo, 0, 0, s});   // il siluro non c'e' piu': rifiuto
    avanza(0.5);
    CHECK(st.messaggi().back().codice == MSG_NESSUNA_ARMA);
    CHECK(m.errori().empty());
}

TEST_CASE("enhanced: posato con le casse d'assetto, il battello si appoggia e poi si alleggerisce" * doctest::skip(!ci_sono())) {
    const auto classi = carica_classi(std::string(M688_DATI) + "/classi.toml");
    Missione m(SCENA, 1);
    m.imposta_fondale(std::make_shared<const Fondale>(Fondale::carica(std::string(M688_DATI) + "/teatri/" + m.teatro() + ".fdl")));
    m.dinamica_sottomarini(classi, M688_DATI);
    DanniSottomarini ds(classi, M688_DATI, 1);
    StazioniMissione st(m);
    st.usa_danni(&ds);
    m.avanza(1.0 / 9.0);
    const auto& g = *m.giocatore();
    bool trovato = false;   // un fondo pianeggiante fra 60 e 200 m vicino al battello
    for (int i = -20; i <= 20 && !trovato; ++i)
        for (int j = -20; j <= 20 && !trovato; ++j) {
            const Vec2 p{g.pos.x + 500.0 * i, g.pos.y + 500.0 * j};
            const double f = m.profondita(p);
            if (!(f > 60.0 && f < 200.0)) continue;
            const double gx = (m.profondita({p.x + 50, p.y}) - m.profondita({p.x - 50, p.y})) / 100.0;
            const double gy = (m.profondita({p.x, p.y + 50}) - m.profondita({p.x, p.y - 50})) / 100.0;
            if (std::hypot(gx, gy) > 0.02) continue;
            m.trova(g.id)->pos = p;
            trovato = true;
        }
    REQUIRE(trovato);
    auto avanza = [&](double s) {
        for (int i = 0; i < static_cast<int>(s * 9.0); ++i) {
            m.avanza(1.0 / 9.0);
            ds.avanza(m, 1.0 / 9.0);
            st.dopo_passo(m);
        }
    };
    avanza(1.0);
    const EsitoStazione e = st.posarsi(m, 290.0);
    REQUIRE(e.messaggio == MSG_POSATI);
    for (const auto& o : e.alla_missione) st.consegna(m, o);
    avanza(1200.0);
    const double fondo = m.profondita(g.pos);
    CHECK(m.dinamica(g.id)->quota_m() > fondo - 1.5);   // appoggiato (il fondo lo ferma)
    CHECK(m.dinamica(g.id)->giri() == 0.0);
    // un ordine di quota: si scaricano le casse e il battello risale
    st.consegna(m, {m.tempo(), TipoOrdine::giri, 0.2});
    st.consegna(m, {m.tempo(), TipoOrdine::quota_con_angolo, fondo - 30.0, 10.0});
    avanza(600.0);
    CHECK(m.dinamica(g.id)->quota_m() < fondo - 15.0);
    CHECK(m.errori().empty());
}
