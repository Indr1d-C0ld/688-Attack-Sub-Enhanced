// Acustica delle collisioni (nucleo/acustica_urto.hpp; dossier docs/ricerca/16_collisioni.md §2.6): le sorgenti della
// scena (colpo, stridio, elica), un urto vero di valuta_urto al sonar di un altro battello, i suoni a bordo.
#include <doctest.h>

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <optional>
#include <string>

#include "nucleo/acustica_urto.hpp"
#include "nucleo/bordo_audio.hpp"

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
    a = std::min(a, v.size());
    double q = 0.0;
    for (std::size_t i = da; i < a; ++i) q += static_cast<double>(v[i]) * v[i];
    return 10.0 * std::log10(std::max(q / static_cast<double>(std::max<std::size_t>(a - da, 1)), 1e-30));
}

Urto urto_semplice() {
    Urto u;
    u.tempo_s = 100.0;
    u.a.unita = 2;
    u.b.unita = 7;
    u.punto = {1000.0, 0.0};
    u.quota_punto_m = 95.0;
    return u;
}

EsitoUrto esito(double livello_db, double tangente_mj, double durata_s, ParteUrto pa = ParteUrto::prua,
                ParteUrto pb = ParteUrto::fianco) {
    EsitoUrto e;
    e.livello_sorgente_db = livello_db;
    e.energia_tangente_mj = tangente_mj;
    e.durata_strisciamento_s = durata_s;
    e.a.parte = pa;
    e.b.parte = pb;
    return e;
}

}  // namespace

TEST_CASE("urto: le sorgenti della scena") {
    CHECK(livello_stridio_db(1.0, 1.0) == doctest::Approx(160.0));
    CHECK(livello_stridio_db(10.0, 1.0) == doctest::Approx(170.0));
    CHECK(livello_stridio_db(1e-9, 20.0) == doctest::Approx(130.0));
    CHECK(livello_stridio_db(1e6, 0.3) == doctest::Approx(180.0));
    // scafo contro scafo: il colpo sordo col guadagno sul livello di 1 MJ, poi lo stridio a pezzi di 1 s
    const Urto u = urto_semplice();
    const auto s = transitori_urto(u, esito(180.0, 2.0, 3.4), 940000, 3);
    REQUIRE(s.size() == 5);
    CHECK(s[0].tipo == "urto_colpo");
    CHECK(s[0].guadagno_db == doctest::Approx(10.0));
    CHECK(s[0].t0 == doctest::Approx(100.0));
    CHECK(s[0].origine == 2);
    CHECK(s[0].quota_m == doctest::Approx(95.0));
    for (int k = 1; k < 5; ++k) {
        CHECK(s[k].tipo == "urto_stridio");
        CHECK(s[k].t0 == doctest::Approx(100.0 + (k - 1)));
        CHECK(s[k].guadagno_db == doctest::Approx(livello_stridio_db(2.0, 3.4) - 160.0));
    }
    for (std::size_t k = 0; k < s.size(); ++k) {
        CHECK(s[k].id == 940000 + static_cast<int>(k));
        CHECK_MESSAGE(catalogo().transitorio(s[k].tipo) != nullptr, s[k].tipo);
    }
    // la vela che colpisce: la lamiera; lo strisciamento lungo si ferma a 20 pezzi; senza strisciamento solo il colpo
    const auto vela = transitori_urto(u, esito(175.0, 5.0, 25.0, ParteUrto::vela), 1, 3);
    CHECK(vela[0].tipo == "urto_lamiera");
    CHECK(vela.size() == 21);
    CHECK(transitori_urto(u, esito(175.0, 0.0, 0.3), 1, 3).size() == 1);
    // l'elica: i colpi delle pale, una voce in piu'
    auto e = esito(175.0, 0.0, 0.3, ParteUrto::prua, ParteUrto::eliche);
    const auto elica = transitori_urto(u, e, 1, 3);
    REQUIRE(elica.size() == 2);
    CHECK(elica[0].tipo == "urto_lamiera");
    CHECK(elica[1].tipo == "urto_elica");
    CHECK_MESSAGE(catalogo().transitorio("urto_elica") != nullptr, "urto_elica");
    // un punto sopra il galleggiamento suona appena sotto
    Urto alto = u;
    alto.quota_punto_m = -3.0;
    CHECK(transitori_urto(alto, esito(170.0, 0.0, 0.0), 1, 3)[0].quota_m == doctest::Approx(0.5));
}

TEST_CASE("urto: due sottomarini che si toccano a 10 nodi si sentono a 10 km") {
    if (!std::filesystem::exists(std::string(M688_DATI) + "/classi.toml")) return;
    const auto classi = carica_classi(std::string(M688_DATI) + "/classi.toml");
    // un 688 di prua sul fianco sinistro di un Victor III fermo, come nella prova delle collisioni
    CorpoUrto b;
    b.unita = 7;
    b.classe = "victor3";
    b.sottomarino = true;
    b.massa_t = 7250;
    b.quota_m = 100;
    const double meta_b = classi.at("victor3").larghezza_m / 2;
    const Vec2 p{-meta_b, 0.0};   // rotta 0: la sinistra e' a ovest
    CorpoUrto a;
    a.unita = 2;
    a.classe = "los_angeles";
    a.sottomarino = true;
    a.massa_t = 6927;
    a.quota_m = 100;
    a.rotta_gradi = 90;
    a.velocita_m_s = 10.0 * NODO;
    a.pos = {p.x - 110.3 / 2, p.y};
    Urto u;
    u.tempo_s = 50.0;
    u.a = a;
    u.b = b;
    u.punto = p;
    u.quota_punto_m = 95;
    u.normale[0] = 1;
    u.normale[1] = 0;
    u.normale[2] = 0;
    const EsitoUrto e = valuta_urto(u, classi, M688_DATI, 7);
    REQUIRE(e.livello_sorgente_db > 150.0);
    const auto s = transitori_urto(u, e, 940000, 7);
    const auto colpo = sorgente_transitoria(catalogo(), s[0], u.tempo_s + 0.5, 2.0);
    REQUIRE(colpo.has_value());
    const Ricevitore lontano{{p.x + 10000.0, p.y}, 150.0, 0.0, 5.0, catalogo().sensore("bqq5_sfera")};
    CHECK(ascolta_transitorio(*colpo, {lontano}, mare()).sentito);
    // dopo la vita della voce non c'e' piu'
    CHECK_FALSE(sorgente_transitoria(catalogo(), s[0], u.tempo_s + 10.0, 2.0).has_value());
}

TEST_CASE("urto: a bordo, il nostro scafo e quello degli altri") {
    const int fs = 22050;
    const auto ascolta = [&](StazioneBordo st, const std::optional<EsitoUrto>& e, const DanniUrtoUnita* proprio,
                             double distanza, double secondi, double* scossone = nullptr) {
        AmbienteBordo b;
        b.passo({}, {0.0, 0.0}, 100.0, mare(), st, RegimeBordo::silenzioso, 0.5);
        if (e) b.urto(*e, proprio, distanza);
        if (scossone) *scossone = b.scossone();   // all'urto: poi cala in mezzo secondo
        return b.passo({}, {0.0, 0.0}, 100.0, mare(), st, RegimeBordo::silenzioso, secondi);
    };
    const auto quiete = ascolta(StazioneBordo::centrale, std::nullopt, nullptr, 0.0, 4.0);
    // il nostro scafo: lo schianto fortissimo, lo scossone, poi lo stridio per tutta la sua durata
    EsitoUrto e = esito(180.0, 2.0, 3.0);
    DanniUrtoUnita nostro;
    nostro.energia_mj = 5.0;
    nostro.scossa_g = 0.4;
    double scossone = 0.0;
    const auto colpito = ascolta(StazioneBordo::centrale, e, &nostro, 0.0, 4.0, &scossone);
    CHECK(dbfs(colpito, 0, fs / 5) > dbfs(quiete, 0, fs / 5) + 30.0);
    CHECK(dbfs(colpito, 2 * fs, 3 * fs) > dbfs(quiete, 2 * fs, 3 * fs) + 10.0);   // lo stridio, dopo il rimbombo
    CHECK(scossone > 0.3);
    // si sente anche in sala macchine e in plancia
    CHECK(dbfs(ascolta(StazioneBordo::macchine, e, &nostro, 0.0, 1.0), 0, fs / 5) >
          dbfs(ascolta(StazioneBordo::macchine, std::nullopt, nullptr, 0.0, 1.0), 0, fs / 5) + 10.0);
    // l'urto di altri, dall'acqua attraverso lo scafo: a 50 m un colpo sordo (a 200 m e' gia' al livello del fondo), a
    // 20 km niente; in plancia dall'acqua niente
    const auto vicino = ascolta(StazioneBordo::centrale, e, nullptr, 50.0, 1.0);
    CHECK(dbfs(vicino, 0, fs / 5) > dbfs(quiete, 0, fs / 5) + 6.0);
    CHECK(ascolta(StazioneBordo::centrale, e, nullptr, 20000.0, 1.0) == ascolta(StazioneBordo::centrale, std::nullopt, nullptr, 0.0, 1.0));
    CHECK(ascolta(StazioneBordo::plancia, e, nullptr, 200.0, 1.0) == ascolta(StazioneBordo::plancia, std::nullopt, nullptr, 0.0, 1.0));
}
