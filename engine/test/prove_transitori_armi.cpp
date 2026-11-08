// Transitori delle armi e degli scafi (R7): voci del catalogo, sequenze di lancio, esplosioni con le bolle,
// implosioni, affondamento, teste attive, righe dei siluri, cuffia e bordo.
#include <doctest.h>

#include <algorithm>
#include <cmath>

#include "nucleo/analisi_acustica.hpp"
#include "nucleo/bordo_audio.hpp"
#include "nucleo/transitori_armi.hpp"

using namespace m688;

namespace {

const CatalogoAcustico& catalogo() {
    static const CatalogoAcustico c = carica_acustica(M688_DATI "/acustica.toml");
    return c;
}

Ambiente mare() {
    Ambiente a;
    a.fondale_m = 3000.0;
    a.vento_m_s = vento_per_stato_del_mare(3);
    return a;
}

double dbfs(const std::vector<float>& v, std::size_t da, std::size_t a) {
    double q = 0.0;
    for (std::size_t i = da; i < a; ++i) q += static_cast<double>(v[i]) * v[i];
    return 10.0 * std::log10(std::max(q / static_cast<double>(a - da), 1e-30));
}

}  // namespace

TEST_CASE("transitori armi: tutte le voci delle sequenze sono nel catalogo") {
    std::vector<Transitorio> tutte;
    for (TipoLancio t : {TipoLancio::siluro, TipoLancio::harpoon, TipoLancio::tomahawk_tubo, TipoLancio::tomahawk_vls}) {
        const auto x = transitori_lancio(t, {0.0, 0.0}, 60.0, 100.0, true, 1, 7);
        tutte.insert(tutte.end(), x.begin(), x.end());
    }
    for (const auto& x : transitori_esplosione(250.0, {0.0, 0.0}, 100.0, 0.0, 100)) tutte.push_back(x);
    for (const auto& x : transitori_implosione(470.0, 2100.0, {0.0, 0.0}, 0.0, 200, 3)) tutte.push_back(x);
    for (const auto& x : transitori_affondamento({0.0, 0.0}, 0.0, 300.0, 3000.0, 300, 4)) tutte.push_back(x);
    const auto voce = [](const std::string& tipo) { Transitorio x; x.tipo = tipo; return x; };
    for (const char* c : {"mk48", "mk48_adcap", "mk46", "uset80", "test71m", "at1"}) tutte.push_back(voce(transitorio_testa(c)));
    for (const char* v : {"portello_sbatte", "attrezzo_cade", "piani_colpo", "pompa_avvio", "tdu", "soffiata_emergenza", "scafo_scricchiola"})
        tutte.push_back(voce(v));
    for (const auto& x : tutte) CHECK_MESSAGE(catalogo().transitorio(x.tipo) != nullptr, x.tipo);
    CHECK(transitorio_testa("type53_65k").empty());   // il vapore-gas segue la scia, nessun ping diretto
}

TEST_CASE("transitori armi: il lancio dal tubo in sequenza") {
    const auto x = transitori_lancio(TipoLancio::siluro, {0.0, 0.0}, 60.0, 100.0, true, 10, 1);
    REQUIRE(x.size() == 6);
    CHECK(x[0].tipo == "tubo_allaga");
    CHECK(x[0].t0 == doctest::Approx(65.0));
    CHECK(x[3].tipo == "sparo_ram");
    CHECK(x[3].t0 == 100.0);
    CHECK(x[4].t0 >= 103.0);
    CHECK(x[4].t0 <= 108.0);
    CHECK(x[5].tipo == "avvio_siluro");
    for (std::size_t k = 0; k < x.size(); ++k) CHECK(x[k].id == 10 + static_cast<int>(k));
    CHECK(transitori_lancio(TipoLancio::siluro, {0.0, 0.0}, 60.0, 100.0, false, 1, 1).size() == 4);
    CHECK(transitori_lancio(TipoLancio::tomahawk_vls, {0.0, 0.0}, 60.0, 100.0, true, 1, 1).size() == 2);
    // lo sparo si sente a decine di km (r7: 10-40 km), la porta molto meno
    Ricevitore r{{20000.0, 0.0}, 100.0, 0.0, 5.0, catalogo().sensore("bqq5_sfera")};
    const auto sparo = sorgente_transitoria(catalogo(), x[3], 100.2, 2.0);
    const auto porta = sorgente_transitoria(catalogo(), x[2], 95.2, 2.0);
    REQUIRE(sparo);
    REQUIRE(porta);
    CHECK(ascolta_transitorio(*sparo, {r}, mare()).se_db > ascolta_transitorio(*porta, {r}, mare()).se_db + 15.0);
    CHECK(ascolta_transitorio(*sparo, {r}, mare()).sentito);
}

TEST_CASE("transitori armi: esplosione, bolla di Willis, implosione, affondamento") {
    CHECK(periodo_bolla_s(250.0, 100.0) == doctest::Approx(2.11 * std::cbrt(250.0) / std::pow(110.1, 5.0 / 6.0)));
    CHECK(guadagno_esplosione_db(1.0) == doctest::Approx(0.0));
    CHECK(guadagno_esplosione_db(1000.0) == doctest::Approx(22.6));
    const auto e = transitori_esplosione(250.0, {0.0, 0.0}, 100.0, 50.0, 1);
    REQUIRE(e.size() == 4);
    const double t1 = periodo_bolla_s(250.0, 100.0);
    CHECK(e[1].t0 - e[0].t0 == doctest::Approx(t1));
    CHECK(e[2].t0 - e[1].t0 == doctest::Approx(0.74 * t1));
    CHECK(e[1].guadagno_db == doctest::Approx(e[0].guadagno_db));   // la voce della bolla e' gia' 14 dB sotto
    CHECK(transitori_esplosione(250.0, {0.0, 0.0}, 5.0, 0.0, 1).size() == 1);   // in superficie la bolla sfiata
    // si sente lontano: 250 kg a 50 km con un idrofono
    Ricevitore r{{50000.0, 0.0}, 100.0, 0.0, 5.0, catalogo().sensore("ssq41b")};
    CHECK(ascolta_transitorio(*sorgente_transitoria(catalogo(), e[0], 50.5, 2.0), {r}, mare()).sentito);
    // l'implosione dello Scorpion: ~6 t a 470 m (Rule) con un volume di ~2100 m^3; 10-20 cedimenti dopo, in discesa
    CHECK(tnt_implosione_kg(470.0, 2100.0) == doctest::Approx(5900.0).epsilon(0.05));
    CHECK(volume_implosione_m3(76.7, 9.6) == doctest::Approx(2100.0).epsilon(0.05));   // il volume dalle dimensioni
    CHECK(volume_implosione_m3(110.3, 10.06) == doctest::Approx(3240.0).epsilon(0.02));
    CHECK(volume_implosione_m3(10.0, 1.0) == doctest::Approx(300.0));
    const auto im = transitori_implosione(470.0, 2100.0, {0.0, 0.0}, 0.0, 1, 9);
    const auto cedimenti = std::count_if(im.begin(), im.end(), [](const Transitorio& x) { return x.tipo == "cedimento_minore"; });
    CHECK(cedimenti >= 10);
    CHECK(cedimenti <= 20);
    CHECK(im[0].guadagno_db == doctest::Approx(guadagno_esplosione_db(tnt_implosione_kg(470.0, 2100.0)) - 12.0));
    CHECK(im.back().quota_m > im[4].quota_m);
    const auto af = transitori_affondamento({0.0, 0.0}, 0.0, 600.0, 3000.0, 1, 2);
    CHECK(af.size() == 4);
    CHECK(af.back().tipo == "urto_fondo");
    CHECK(transitori_affondamento({0.0, 0.0}, 0.0, -1.0, 3000.0, 1, 2).size() == 3);
}

TEST_CASE("transitori armi: teste attive e righe dei siluri") {
    CHECK(intervallo_ping_testa_s(3600.0) == doctest::Approx(5.76));
    CHECK(intervallo_ping_testa_s(100.0) == doctest::Approx(0.16));
    CHECK(intervallo_ping_testa_s(10000.0) == doctest::Approx(5.76));   // fuori portata: la cadenza di ricerca
    CHECK(transitorio_testa("mk48") == "ping_testa_mk48");
    // il Mk 48 a 55 kn: la riga della pompa a getto a 907,5 Hz; a 40 kn quella a 660 Hz
    const auto righe = [](double kn) {
        UnitaAcustica u;
        u.classe = "mk48";
        u.categoria = "arma";
        u.velocita_kn = kn;
        return righe_emesse(sorgente_acustica(catalogo(), u));
    };
    const auto vicina = [](const std::vector<RigaEmessa>& v, double hz) {
        return std::any_of(v.begin(), v.end(), [&](const RigaEmessa& r) { return std::fabs(r.hz - hz) < 0.1; });
    };
    CHECK(vicina(righe(55.0), 907.5));
    CHECK_FALSE(vicina(righe(55.0), 660.0));
    CHECK(vicina(righe(40.0), 660.0));
}

TEST_CASE("transitori armi: in cuffia lo sparo a campione esatto, a bordo la porta e non l'esplosione") {
    Ricevitore r{{0.0, 0.0}, 150.0, 0.0, 5.0, catalogo().sensore("tb16")};
    ParametriAscolto p;
    p.f_min_hz = 20.0;
    p.f_max_hz = 2000.0;
    const auto x = transitori_lancio(TipoLancio::siluro, {5000.0, 0.0}, 60.0, 10.0, false, 1, 1);
    CuffiaSonar c(p);
    const auto prima = c.passo({}, r, mare(), 90.0, 0.5);
    const auto dopo = c.passo({*sorgente_transitoria(catalogo(), x[1], 10.05, 2.0)}, r, mare(), 90.0, 0.5);
    CHECK(dbfs(dopo, 0, 2205) > dbfs(prima, 0, prima.size()) + 6.0);   // i primi 100 ms
    CHECK(impulsivo(*sorgente_transitoria(catalogo(), x[1], 10.05, 2.0)));
    CHECK_FALSE(impulsivo(*sorgente_transitoria(catalogo(), transitori_lancio(TipoLancio::siluro, {}, 60.0, 100.0, true, 1, 1)[0], 70.0, 2.0)));
    // a bordo: la porta di un tubo a 100 m si sente; un'esplosione nella scena no (la porta scoppio())
    const auto bordo = [&](const std::optional<Sorgente>& s) {
        AmbienteBordo b;
        b.passo({}, {0.0, 0.0}, 60.0, mare(), StazioneBordo::siluri, RegimeBordo::silenzioso, 0.5);
        return b.passo(s ? std::vector<Sorgente>{*s} : std::vector<Sorgente>{}, {0.0, 0.0}, 60.0, mare(), StazioneBordo::siluri,
                       RegimeBordo::silenzioso, 0.5);
    };
    const auto porta = transitori_lancio(TipoLancio::siluro, {100.0, 0.0}, 60.0, 10.0, false, 1, 1)[0];
    const auto quiete = bordo(std::nullopt);
    CHECK(bordo(sorgente_transitoria(catalogo(), porta, 5.05, 2.0)) != quiete);
    const auto boom = transitori_esplosione(250.0, {3000.0, 0.0}, 100.0, 0.0, 50)[0];
    CHECK(bordo(sorgente_transitoria(catalogo(), boom, 0.05, 2.0)) == quiete);
}
