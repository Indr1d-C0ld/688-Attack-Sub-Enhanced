#include <doctest.h>

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <memory>

#include "missioni/carriera_missione.hpp"
#include "missioni/missione.hpp"
#include "missioni/registro_missione.hpp"
#include "missioni/stazioni_missione.hpp"
#include "nucleo/carico.hpp"
#include "nucleo/fondale.hpp"
#include "nucleo/mine.hpp"
#include "nucleo/ordini.hpp"

using namespace m688;

// Mine dell'Enhanced (Fase 4a, M2b; nucleo/mine.hpp, dossier docs/ricerca/14_carico_armi.md §2.6-2.7, §2.12)

namespace {

CampoMinato campo() {
    CampoMinato c;
    c.imposta_dati(carica_mine(M688_DATI "/carico.toml"));
    return c;
}

BersaglioMina nave(int id, double x, double y) { return {id, {x, y}, 0.0, true, false}; }
BersaglioMina sub(int id, double x, double y, double quota) { return {id, {x, y}, quota, false, true}; }

}  // namespace

TEST_CASE("mine: dati") {
    const auto d = carica_mine(M688_DATI "/carico.toml");
    REQUIRE(d.size() == 5);
    CHECK(d.at("mk67").tipo == TipoMina::influenza);
    CHECK(d.at("mk67").corsa_max_m == doctest::Approx(13000.0));
    CHECK(d.at("mk60").tipo == TipoMina::captor);
    CHECK(d.at("mk60").siluro == "mk46");
    CHECK_FALSE(d.at("mk60").navi);
    CHECK(d.at("pmr1").tipo == TipoMina::mina_razzo);
    CHECK_FALSE(d.at("pmr2").navi);
    CHECK(d.at("pmr2").raggio_m == doctest::Approx(2.0 * d.at("pmr1").raggio_m));
    CHECK(d.at("mdt").tipo == TipoMina::influenza);
    CHECK(d.at("mdt").profondita_max_m == doctest::Approx(50.0));
}

TEST_CASE("mine: Mk 67, corsa, posa, armamento e scoppio sotto una nave") {
    CampoMinato c = campo();
    const auto basso = [](Vec2) { return 50.0; };
    const int id = c.posa("mk67", {0, 0}, 90.0, 5000.0, 0.0, 0, 8);
    REQUIRE(id > 0);
    CHECK(c.posa("ignota", {0, 0}, 0, 0, 0, 0, 8) == -1);
    // 20 kn: 5000 m in circa 486 s
    double t = 0.0;
    CampoMinato::Esito e;
    for (; t < 480.0; t += 1.0) e = c.avanza(t, 1.0, {}, basso);
    CHECK(c.mine()[0].stato == Mina::Stato::in_corsa);
    for (; t < 500.0 && c.mine()[0].stato == Mina::Stato::in_corsa; t += 1.0) e = c.avanza(t, 1.0, {}, basso);
    REQUIRE(c.mine()[0].stato == Mina::Stato::armandosi);
    CHECK(e.posate == std::vector<int>{id});
    CHECK(c.mine()[0].pos.x == doctest::Approx(5000.0));
    CHECK(c.mine()[0].pos.y == doctest::Approx(0.0).epsilon(1e-6));
    CHECK(c.mine()[0].quota == doctest::Approx(50.0));
    // non armata: una nave sopra non la fa scoppiare
    e = c.avanza(t, 1.0, {nave(5, 5000, 0)}, basso);
    CHECK(e.inneschi.empty());
    t += 600.0;
    e = c.avanza(t, 1.0, {nave(5, 5100, 0)}, basso);   // armata; la nave e' a 112 m: fuori dal raggio di 60
    CHECK(e.armate == std::vector<int>{id});
    e = c.avanza(t + 1, 1.0, {nave(5, 5100, 0)}, basso);
    CHECK(e.inneschi.empty());
    e = c.avanza(t + 2, 1.0, {nave(5, 5020, 0), sub(6, 5000, 10, 30)}, basso);
    REQUIRE(e.inneschi.size() == 1);
    const InnescoMina& x = e.inneschi[0];
    CHECK(x.bersaglio == 6);   // il piu' vicino: il sottomarino a 22 m
    CHECK_FALSE(x.siluro);
    CHECK(x.carica_kg == doctest::Approx(150.0));
    CHECK(x.dz_m == doctest::Approx(20.0));
    CHECK(x.posatore == 0);
    CHECK(c.mine()[0].stato == Mina::Stato::scoppiata);
    CHECK(c.avanza(t + 3, 1.0, {nave(5, 5000, 0)}, basso).inneschi.empty());   // una volta sola
}

TEST_CASE("mine: fondali inadatti e terra") {
    CampoMinato c = campo();
    c.posa("mk67", {0, 0}, 0.0, 1000.0, 0.0, 0, 8);   // corre su un fondo di 500 m: troppo per l'influenza
    c.posa("mk60", {0, 0}, 0.0, 0.0, 0.0, 0, 8);      // la CAPTOR a 500 m va bene
    c.posa("mk67", {0, 0}, 0.0, 13000.0, 0.0, 0, 8);
    const auto f = [](Vec2 p) { return p.y > 5000.0 ? -10.0 : 500.0; };   // terra a nord
    CampoMinato::Esito e;
    for (double t = 0; t < 900.0; t += 1.0) {
        const auto x = c.avanza(t, 1.0, {}, f);
        e.inerti.insert(e.inerti.end(), x.inerti.begin(), x.inerti.end());
        e.posate.insert(e.posate.end(), x.posate.begin(), x.posate.end());
    }
    CHECK(c.mine()[0].stato == Mina::Stato::inerte);
    CHECK(c.mine()[1].stato == Mina::Stato::armata);
    CHECK(c.mine()[1].quota == doctest::Approx(350.0));   // ormeggiata 150 m sopra il fondo
    CHECK(c.mine()[2].stato == Mina::Stato::inerte);      // arrivata a terra
    CHECK(c.mine()[2].pos.y <= 5001.0);
    CHECK(e.inerti.size() == 2);
    CHECK(e.posate.size() == 1);
}

TEST_CASE("mine: CAPTOR e PMR-2 contro i sottomarini") {
    CampoMinato c = campo();
    const auto f = [](Vec2) { return 400.0; };
    c.posa("mk60", {0, 0}, 0.0, 0.0, 0.0, 0, 8);
    c.posa("pmr2", {10000, 0}, 0.0, 0.0, 0.0, 1, 16);
    c.avanza(0.0, 1.0, {}, f);
    c.avanza(600.0, 1.0, {}, f);
    CHECK(c.mine()[1].quota == doctest::Approx(300.0));   // a quota fissa sopra il fondo di 400 m
    // la CAPTOR ignora la nave sopra di lei e il sottomarino in superficie
    auto e = c.avanza(601.0, 1.0, {nave(5, 0, 0), sub(6, 200, 0, 10)}, f);
    CHECK(e.inneschi.empty());
    e = c.avanza(602.0, 1.0, {sub(6, 600, 0, 150)}, f);
    REQUIRE(e.inneschi.size() == 1);
    CHECK(e.inneschi[0].siluro);
    CHECK(e.inneschi[0].classe_siluro == "mk46");
    CHECK(e.inneschi[0].rotta == doctest::Approx(90.0));
    CHECK(e.inneschi[0].quota == doctest::Approx(250.0));
    // la PMR-2 ignora la nave e il sottomarino sotto di lei, poi lancia il razzo contro quello sopra
    e = c.avanza(603.0, 1.0, {nave(7, 10000, 50), sub(8, 10000, 100, 350)}, f);
    CHECK(e.inneschi.empty());
    e = c.avanza(604.0, 1.0, {sub(9, 10100, 0, 150)}, f);
    REQUIRE(e.inneschi.size() == 1);
    CHECK_FALSE(e.inneschi[0].siluro);                    // il razzo scoppia accanto al bersaglio
    CHECK(e.inneschi[0].pos.x == doctest::Approx(10100.0));
    CHECK(e.inneschi[0].quota == doctest::Approx(150.0));
    CHECK(e.inneschi[0].distanza_m == doctest::Approx(10.0));
    CHECK(e.inneschi[0].carica_kg == doctest::Approx(175.0));
    CHECK(e.inneschi[0].posatore == 1);
}

namespace {

int quanti(const Missione& m, TipoEvento t) {
    return static_cast<int>(std::count_if(m.eventi_missione().begin(), m.eventi_missione().end(), [&](const EventoMissione& e) { return e.tipo == t; }));
}

}  // namespace

TEST_CASE("mine: una Mk 67 posata dal tubo scoppia sotto una nave (Enhanced)") {
    const auto f = std::filesystem::temp_directory_path() / "m688_prova_mine.lua";
    {
        std::ofstream o(f);
        o << "return { id = 'PROVA_MINE', teatro = 'faroe', paesaggio_sonoro = false,\n"
             "dotazione = { mk48 = 10, esca = 6 }, tubi = { 'mk48', 'mk48', 'mk48', 'mk48', 'esca', 'esca' },\n"
             "deposito = { mk48 = 10, mk67 = 2, esca = 6 }, tipo_missione = 'minamento',\n"
             "obiettivi = { { id = 'campo', tipo = 'minare', minimo = 1, cerchio = { x = 0, y = 3000, r = 500 } } },\n"
             "script = {}, varianti = { { probabilita = 1, unita = {\n"
             "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true,"
             "  posizione = { x = 0, y = 0 }, quota_m = 30, rotta = 0, velocita_kn = 0 },\n"
             "{ id = 2, classe = 'krivak2', categoria = 'nave', flag = 18, posizione = { x = 0, y = 3000 },"
             "  quota_m = 0, rotta = 90, velocita_kn = 0 },\n"
             "} } } }\n";
    }
    Missione m(f.string(), 1, false);
    REQUIRE(m.errori().empty());
    // fondale uniforme di 50 m
    m.imposta_fondale(std::make_shared<const Fondale>(200, 200, 1000.0, 1.0, -1000.0, std::vector<std::uint16_t>(200 * 200, 950)), {});
    REQUIRE(m.profondita({0, 3000}) == doctest::Approx(50.0));
    m.siluri_enhanced(M688_DATI);
    REQUIRE(m.errori().empty());
    const CatalogoCarico cat = carica_carico(M688_DATI "/carico.toml");
    StazioniMissione st(m);
    SalaSiluri sala(cat, "los_angeles", m.limiti_carico());
    REQUIRE(sala.metti(Posto::tubo, "mk67", 1).fatto);
    st.imposta_carico(m, sala.carico(), cat);
    RegistroOrdini ordini;
    const auto avanza = [&](double s) {
        for (int i = 0; i < static_cast<int>(std::lround(s * 9.0)); ++i) {
            for (const auto& o : ordini.consegna(m.tempo())) st.consegna(m, o);
            m.avanza(1.0 / 9.0);
            st.dopo_passo(m);
        }
    };
    avanza(1.0 / 9.0);
    // rotta 000 e 3000 m di corsa, poi armamento (lo script dell'originale qui non c'e': il messaggio 10 a mano)
    ordini.ordina({m.tempo(), TipoOrdine::imposta_siluro, 0.0, static_cast<double>(CampoSiluro::rotta), 1});
    ordini.ordina({m.tempo(), TipoOrdine::imposta_siluro, 3000.0, static_cast<double>(CampoSiluro::rte), 1});
    ordini.ordina({m.tempo(), TipoOrdine::carica_tubo, 0, 0, 1});
    avanza(1.0);
    REQUIRE(st.armi().stato(1) == StatoTubo::in_armamento);
    st.armi().messaggio_missione(10);
    REQUIRE(st.armi().stato(1) == StatoTubo::armato);
    ordini.ordina(st.armi().lancio(m.tempo()));
    avanza(1.0);
    REQUIRE(m.campo_minato().mine().size() == 1);
    CHECK(m.campo_minato().mine()[0].stato == Mina::Stato::in_corsa);
    CHECK(st.armi().in_acqua() == 0);
    avanza(320.0);   // 3000 m a 20 kn
    const Mina& mina = m.campo_minato().mine()[0];
    REQUIRE(mina.stato == Mina::Stato::armandosi);
    CHECK(mina.pos.y == doctest::Approx(3000.0).epsilon(0.01));
    CHECK(quanti(m, TipoEvento::mina_posata) == 1);
    avanza(610.0);
    CHECK(m.campo_minato().mine()[0].stato == Mina::Stato::scoppiata);
    REQUIRE(quanti(m, TipoEvento::mina_esplosa) == 1);
    const auto& colpi = m.colpi();
    REQUIRE_FALSE(colpi.empty());
    const Colpo& c = colpi.back();
    CHECK(c.vittima == 2);
    CHECK(c.classe_arma == "mk67");
    CHECK(c.lanciatore == 0);
    CHECK(c.tnt_kg == doctest::Approx(150.0));
    CHECK(c.influenza);
    CHECK(c.geometria_vera);
    CHECK(c.distanza_m == doctest::Approx(50.0).epsilon(0.05));
    const EsitoDettagliato x = m.esito_dettagliato();
    CHECK(x.mine_posate == 1);
    CHECK(x.mine_esplose == 1);
    CHECK(x.per_arma.at("mk67").colpi == 1);
    CHECK(x.lanci == 0);   // la posa e lo scoppio non sono lanci
    REQUIRE(!x.obiettivi.empty());
    CHECK(x.obiettivi[0].tipo == Obiettivo::Tipo::minare);
    CHECK(x.obiettivi[0].stato == Obiettivo::Stato::raggiunto);
    const EsitoPerCarriera e = riassumi(m);
    CHECK(e.colpi == 1);
    CHECK(e.lanci == 0);
    CHECK(m.errori().empty());
    std::filesystem::remove(f);
}

TEST_CASE("mine: la CAPTOR libera un Mk 46 contro un sottomarino nemico (Enhanced)") {
    const auto f = std::filesystem::temp_directory_path() / "m688_prova_captor.lua";
    {
        std::ofstream o(f);
        o << "return { id = 'PROVA_CAPTOR', teatro = 'faroe', paesaggio_sonoro = false,\n"
             "dotazione = { mk48 = 10, esca = 6 }, tubi = { 'mk48', 'mk48', 'mk48', 'mk48', 'esca', 'esca' },\n"
             "deposito = { mk48 = 10, mk60 = 2, esca = 6 },\n"
             "script = {}, varianti = { { probabilita = 1, unita = {\n"
             "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true,"
             "  posizione = { x = 0, y = 0 }, quota_m = 100, rotta = 0, velocita_kn = 0 },\n"
             "{ id = 2, classe = 'victor3', categoria = 'sottomarino', flag = 18, posizione = { x = 20000, y = 0 },"
             "  quota_m = 150, rotta = 90, velocita_kn = 0 },\n"
             "} } } }\n";
    }
    Missione m(f.string(), 1, false);
    m.imposta_fondale(std::make_shared<const Fondale>(200, 200, 1000.0, 1.0, -1000.0, std::vector<std::uint16_t>(200 * 200, 500)), {});
    m.siluri_enhanced(M688_DATI);
    const CatalogoCarico cat = carica_carico(M688_DATI "/carico.toml");
    StazioniMissione st(m);
    SalaSiluri sala(cat, "los_angeles", m.limiti_carico());
    REQUIRE(sala.metti(Posto::tubo, "mk60", 1).fatto);
    st.imposta_carico(m, sala.carico(), cat);
    RegistroOrdini ordini;
    const auto avanza = [&](double s) {
        for (int i = 0; i < static_cast<int>(std::lround(s * 9.0)); ++i) {
            for (const auto& o : ordini.consegna(m.tempo())) st.consegna(m, o);
            m.avanza(1.0 / 9.0);
            st.dopo_passo(m);
        }
    };
    avanza(1.0 / 9.0);
    ordini.ordina({m.tempo(), TipoOrdine::carica_tubo, 0, 0, 1});
    avanza(1.0);
    st.armi().messaggio_missione(10);
    ordini.ordina(st.armi().lancio(m.tempo()));
    avanza(2.0);
    REQUIRE(m.campo_minato().mine().size() == 1);
    CHECK(m.campo_minato().mine()[0].stato == Mina::Stato::armandosi);   // sotto il battello, a 350 m
    CHECK(m.campo_minato().mine()[0].quota == doctest::Approx(350.0));
    m.trova(0)->pos = {-30000, 0};   // il posatore se ne va; il sottomarino nemico arriva sopra la mina
    m.trova(2)->pos = {500, 0};
    avanza(610.0);
    REQUIRE(quanti(m, TipoEvento::mina_esplosa) == 1);
    int siluro = -1;
    for (const auto& [id, u] : m.unita())
        if (m.da_mina(id) && m.siluro(id)) siluro = id;
    REQUIRE(siluro >= 0);
    CHECK(m.siluro(siluro)->dati().classe == "mk46");
    CHECK(m.unita().at(siluro).lanciatore == 0);
    CHECK(m.unita().at(siluro).classe == "mk60");
    CHECK(quanti(m, TipoEvento::lancio_giocatore) == 0);   // non e' un lancio del giocatore
    CHECK(m.errori().empty());
    std::filesystem::remove(f);
}
