// Le funzioni dell'Alfa nell'Enhanced (dossier 19 §3.1-3.4 e §6; nota condivisa alfa-boksit-tan-poks-akkord-proposta.md):
// Akkord a tre bersagli e l'avaria che blocca le armi (nucleo/armi_giocatore), Boksit a programma (StazioniMissione)
#include <doctest.h>

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <string>

#include "missioni/missione.hpp"
#include "missioni/stazioni_missione.hpp"
#include "nucleo/armi_giocatore.hpp"
#include "nucleo/carico.hpp"
#include "nucleo/classi.hpp"
#include "nucleo/geometria.hpp"

using namespace m688;

namespace {

const std::string MISSIONE_ALFA = std::string(M688_MISSIONI) + "/libreria/LIB_I04_PORTA_DI_ERCOLE.lua";
bool c_e() { return std::filesystem::exists(MISSIONE_ALFA) && std::filesystem::exists(std::string(M688_DATI) + "/classi.toml"); }

const CatalogoCarico& catalogo() {
    static const CatalogoCarico c = carica_carico(M688_DATI "/carico.toml");
    return c;
}

// la stazione Armi dell'Alfa col carico antisom, Akkord attivo, i tubi carichi
StazioneArmi stazione_alfa() {
    LimitiMissione l;
    l.deposito = {{"type53", 18}, {"esca", 4}};
    l.tipo_missione = "antisom";
    SalaSiluri sala(catalogo(), "alfa", l);
    sala.carico_iniziale();
    StazioneArmi s({"type53", "type53", "type53", "type53", "type53", "esca"}, {}, 6, 6);
    s.imposta_carico(sala.carico(), catalogo(), parametri_ricarica("alfa"));
    s.attiva_akkord();
    return s;
}

bool designa(const EsitoStazione& e, int id) {
    return std::any_of(e.alla_missione.begin(), e.alla_missione.end(),
                       [&](const Ordine& o) { return o.tipo == TipoOrdine::designa_bersaglio && o.n == id; });
}

}  // namespace

TEST_CASE("alfa: Akkord tiene tre bersagli, il quarto prende il posto del piu' vecchio, la scelta va al lancio") {
    StazioneArmi s = stazione_alfa();
    CHECK(s.riguarda({0, TipoOrdine::akkord_designa}));
    auto e = s.consegna({10.0, TipoOrdine::akkord_designa, 0, 0, 21}, {});
    CHECK(e.messaggio == MSG_AKKORD_DESIGNATO);
    CHECK(designa(e, 21));
    s.consegna({20.0, TipoOrdine::akkord_designa, 0, 0, 22}, {});
    s.consegna({30.0, TipoOrdine::designa_bersaglio, 0, 0, 23}, {});   // il TARGET di sempre passa da Akkord
    CHECK(s.bersagli_akkord() == std::array<int, 3>{21, 22, 23});
    CHECK(s.bersaglio_scelto() == 3);
    // lo stesso contatto di nuovo: lo si sceglie, senza spostarlo
    e = s.consegna({35.0, TipoOrdine::akkord_designa, 0, 0, 21}, {});
    CHECK(e.messaggio == 0);
    CHECK(s.bersaglio_scelto() == 1);
    // un quarto: prende il posto del meno recente (il 21, designato a 10 s)
    s.consegna({40.0, TipoOrdine::akkord_designa, 0, 0, 24}, {});
    CHECK(s.bersagli_akkord() == std::array<int, 3>{24, 22, 23});
    // la scelta di uno slot: il suo bersaglio va alla missione (e al lancio, come il TARGET)
    e = s.consegna({50.0, TipoOrdine::akkord_scegli, 0, 0, 2}, {});
    CHECK(designa(e, 22));
    CHECK(s.bersaglio() == 22);
    CHECK(s.consegna({50.0, TipoOrdine::akkord_scegli, 0, 0, 3}, {}).alla_missione.size() == 1);
    // i contatti persi lasciano libero il loro posto
    s.aggiorna_akkord({22, 24});
    CHECK(s.bersagli_akkord() == std::array<int, 3>{24, 22, -1});
    CHECK(s.bersaglio_scelto() == 0);
    CHECK(s.consegna({60.0, TipoOrdine::akkord_scegli, 0, 0, 3}, {}).alla_missione.empty());   // slot vuoto
}

TEST_CASE("alfa: Akkord guasto blocca tutte le armi, la ricarica meccanica continua") {
    StazioneArmi s = stazione_alfa();
    REQUIRE(s.stato(1) == StatoTubo::carico);
    s.consegna({0.0, TipoOrdine::carica_tubo, 0, 0, 1}, {});
    REQUIRE(s.messaggio_missione(10) != 0);
    REQUIRE(s.stato(1) == StatoTubo::armato);
    CHECK(s.imposta_guasto_akkord(false) == 0);
    CHECK(s.imposta_guasto_akkord(true) == MSG_AKKORD_GUASTO);
    CHECK(s.akkord_guasto());
    CHECK(s.stato(1) == StatoTubo::carico);   // i dati di tiro non arrivano piu'
    CHECK(s.consegna({1.0, TipoOrdine::lancia, 0, 0, 1}, {}).messaggio == MSG_AKKORD_GUASTO);
    CHECK(s.consegna({1.0, TipoOrdine::carica_tubo, 0, 0, 1}, {}).messaggio == MSG_AKKORD_GUASTO);   // niente armamento
    CHECK(s.consegna({1.0, TipoOrdine::akkord_designa, 0, 0, 5}, {}).messaggio == MSG_AKKORD_GUASTO);
    CHECK(s.consegna({1.0, TipoOrdine::imposta_siluro, 1.0, 0.0, 2}, {}).messaggio == MSG_AKKORD_GUASTO);
    // la ricarica (УБЗ) si', fino al tubo carico; l'armamento automatico no
    StazioneArmi r = stazione_alfa();
    r.consegna({0.0, TipoOrdine::carica_tubo, 0, 0, 1}, {});
    r.messaggio_missione(10);
    r.consegna({1.0, TipoOrdine::lancia, 0, 0, 1}, {});   // parte, il tubo si ricarica da solo
    REQUIRE(r.stato(1) == StatoTubo::in_ricarica);
    r.imposta_guasto_akkord(true);
    const EsitoRicarica fine = r.avanza(1.0 + 75.0);
    CHECK(r.stato(1) == StatoTubo::carico);
    CHECK(fine.alla_missione.empty());
    // di nuovo in servizio: si arma come sempre
    CHECK(r.imposta_guasto_akkord(false) == MSG_AKKORD_RIPRISTINATO);
    CHECK(r.consegna({80.0, TipoOrdine::carica_tubo, 0, 0, 1}, {}).messaggio == MSG_IN_ARMAMENTO);
    // un 688 (senza Akkord) non lo conosce
    StazioneArmi a({"mk48", "mk48", "mk48", "mk48", "esca", "esca", "harpoon"}, {{"mk48", 20}}, 7, 4);
    CHECK(a.imposta_guasto_akkord(true) == 0);
    CHECK_FALSE(a.akkord_guasto());
    CHECK(a.consegna({0, TipoOrdine::akkord_designa, 0, 0, 5}, {}).alla_missione.empty());
}

TEST_CASE("alfa: Boksit a programma segue i punti di rotta con la quota e la velocita' di ogni tappa" * doctest::skip(!c_e())) {
    const auto classi = carica_classi(std::string(M688_DATI) + "/classi.toml");
    Missione m(MISSIONE_ALFA, 1, false);
    m.dinamica_sottomarini(classi, M688_DATI);
    StazioniMissione st(m);
    m.avanza(1.0 / 9.0);
    REQUIRE(m.giocatore());
    REQUIRE(m.giocatore()->classe == "alfa");
    const int id = m.giocatore()->id;
    REQUIRE(m.dinamica(id));
    CHECK(st.modo_boksit(0) == ModoBoksit::automatico);
    // senza punti il programma non parte
    CHECK_FALSE(st.consegna(m, {m.tempo(), TipoOrdine::modo_boksit, 2.0, 0.0, 0}));
    const Vec2 p = m.giocatore()->pos;
    const double r = m.giocatore()->rotta * PI / 180.0;
    const Vec2 a{p.x + 1500.0 * std::sin(r), p.y + 1500.0 * std::cos(r)};   // davanti, poi a dritta
    const Vec2 b{a.x + 1500.0 * std::cos(r), a.y - 1500.0 * std::sin(r)};
    CHECK(st.consegna(m, {m.tempo(), TipoOrdine::punto_di_rotta, a.x, a.y}));
    CHECK(st.consegna(m, {m.tempo(), TipoOrdine::punto_di_rotta, b.x, b.y}));
    CHECK(st.consegna(m, {m.tempo(), TipoOrdine::programma_tappa, 140.0, -1.0, 1}));
    CHECK(st.consegna(m, {m.tempo(), TipoOrdine::programma_tappa, 60.0, 12.0, 2}));
    CHECK_FALSE(st.consegna(m, {m.tempo(), TipoOrdine::programma_tappa, 60.0, 12.0, 7}));   // non c'e' il punto 7
    CHECK(st.programma_boksit().size() == 2);
    CHECK(st.consegna(m, {m.tempo(), TipoOrdine::modo_boksit, 2.0, 0.0, 1}));   // la quota a programma
    CHECK(st.consegna(m, {m.tempo(), TipoOrdine::modo_boksit, 2.0, 0.0, 0}));   // la rotta a programma: il pilota
    CHECK(m.pilota_automatico());
    CHECK(st.modo_boksit(0) == ModoBoksit::programma);
    CHECK(st.tappa_boksit(m) == 1);
    CHECK(st.tappe_boksit() == 2);
    // la prima tappa: la sua quota
    st.dopo_passo(m);
    CHECK(m.giocatore()->quota_ordinata == doctest::Approx(140.0));
    // fino al primo punto: la seconda tappa, la sua quota e la sua velocita'
    bool tappa = false;
    for (int k = 0; k < 9 * 1200 && !tappa; ++k) {
        m.avanza(1.0 / 9.0);
        st.dopo_passo(m);
        for (const auto& x : st.messaggi()) tappa = tappa || x.codice == MSG_TAPPA_BOKSIT;
    }
    REQUIRE(tappa);
    CHECK(st.tappa_boksit(m) == 2);
    CHECK(m.giocatore()->quota_ordinata == doctest::Approx(60.0));
    // toccare il timone lascia il programma
    CHECK(st.consegna(m, {m.tempo(), TipoOrdine::barra, 10.0}));
    st.dopo_passo(m);
    CHECK_FALSE(m.pilota_automatico());
    CHECK(st.modo_boksit(0) == ModoBoksit::automatico);
    CHECK(st.tappe_boksit() == 0);
    CHECK(m.errori().empty());
}

TEST_CASE("alfa: Boksit non vale per il 688 ne' nel classico" * doctest::skip(!c_e())) {
    Missione classico(MISSIONE_ALFA, 1, false);
    StazioniMissione st(classico);
    CHECK_FALSE(st.consegna(classico, {0, TipoOrdine::modo_boksit, 2.0, 0.0, 1}));
    Missione m(std::string(M688_MISSIONI) + "/SCENE1A.lua", 1, false);
    const auto classi = carica_classi(std::string(M688_DATI) + "/classi.toml");
    m.dinamica_sottomarini(classi, M688_DATI);
    StazioniMissione s688(m);
    m.avanza(1.0 / 9.0);
    CHECK_FALSE(s688.consegna(m, {0, TipoOrdine::modo_boksit, 2.0, 0.0, 1}));
    s688.dopo_passo(m);
    CHECK_FALSE(s688.armi().akkord());
}
