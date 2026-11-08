#include <doctest.h>

#include <algorithm>
#include <cmath>
#include <functional>
#include <vector>

#include "nucleo/ecoscandaglio.hpp"
#include "nucleo/quadro_fondale.hpp"
#include "nucleo/sondaggio_ia.hpp"

using namespace m688;

// L'IA che sonda il fondo (docs/24 §3): quando, a che potenza, il BQS-15, la quota e la velocita' prudenti

namespace {

constexpr int N = 200;   // 40 x 40 km a celle da 200 m

Fondale fondale(const std::function<double(double, double)>& prof, bool rilevato) {
    Fondale base(N, N, 200.0, 1.0, -8000.0, std::vector<std::uint16_t>(static_cast<std::size_t>(N * N), 0));
    std::vector<std::uint16_t> v(static_cast<std::size_t>(N * N));
    for (int r = 0; r < N; ++r)
        for (int c = 0; c < N; ++c) {
            const Vec2 p = base.centro_cella(c, r);
            v[static_cast<std::size_t>(r * N + c)] = static_cast<std::uint16_t>(std::lround(8000.0 - prof(p.x, p.y)));
        }
    Fondale f(N, N, 200.0, 1.0, -8000.0, v);
    Campagna k;
    k.anno = 1975;
    k.ente = "prova";
    k.spaziatura_m = 3704.0;
    k.spaziatura_bacini_m = 3704.0;
    f.imposta_carta(1988, {k}, std::vector<std::uint8_t>(static_cast<std::size_t>(N * N), rilevato ? 1 : 0));
    return f;
}

// un monte sottomarino a nord del centro: da 1500 m sale a 300 m in cima
double monte(double x, double y) {
    const Fondale ref(N, N, 200.0, 1.0, -8000.0, std::vector<std::uint16_t>(static_cast<std::size_t>(N * N), 0));
    const Vec2 c = ref.centro_cella(N / 2, N / 2 + 15);
    const double d = std::hypot(x - c.x, y - c.y);
    return 1500.0 - 1200.0 * std::exp(-d * d / (2.0 * 1500.0 * 1500.0));
}

SituazioneFondo situazione(const Fondale& f, double quota, LivelloEmissioni l) {
    SituazioneFondo s;
    s.pos_stimata = f.centro_cella(N / 2, N / 2 + 6);   // 1,8 km a sud della cima
    s.sigma_posizione_m = 300.0;
    s.quota_m = quota;
    s.rotta = 0.0;   // verso il monte
    s.velocita_kn = 12.0;
    s.t = 1000.0;
    s.livello = l;
    return s;
}

}  // namespace

TEST_CASE("sondaggio dell'IA: in acqua profonda niente; verso un monte con la carta scarsa sonda, rallenta, risale") {
    const Fondale piatto = fondale([](double, double) { return 4000.0; }, true);
    const QuadroFondale qp(piatto);
    const DottrinaFondo nato = dottrina_fondo(false);
    const DecisioneFondo lontano = decidi_fondo(qp, situazione(piatto, 200.0, LivelloEmissioni::navigazione), nato);
    CHECK_FALSE(lontano.sonda);
    CHECK_FALSE(lontano.bqs15);
    CHECK(lontano.quota_max_m > 3000.0);

    const Fondale scarsa = fondale(monte, false);   // la carta senza rilievo: sigma del 25%
    const QuadroFondale q(scarsa);
    SituazioneFondo s = situazione(scarsa, 260.0, LivelloEmissioni::navigazione);
    const DecisioneFondo d = decidi_fondo(q, s, nato);
    MESSAGE("verso il monte: fondo prudente ", d.fondo_prudente_m, " m, quota max ", d.quota_max_m, ", velocita' max ", d.velocita_max_kn, ", ", d.motivo);
    CHECK(d.sonda);
    CHECK(d.potenza == PotenzaEco::piena);
    CHECK(d.bqs15);
    CHECK(d.quota_max_m < 400.0);
    CHECK(d.velocita_max_kn <= nato.velocita_prudente_kn + 1e-9);
    // un ostile vicino: la potenza bassa; RIDOTTO sempre bassa; SICUREZZA sempre piena
    s.ostile_vicino_m = 8000.0;
    CHECK(decidi_fondo(q, s, nato).potenza == PotenzaEco::bassa);
    s.livello = LivelloEmissioni::sicurezza;
    CHECK(decidi_fondo(q, s, nato).potenza == PotenzaEco::piena);
    s.livello = LivelloEmissioni::ridotto;
    CHECK(decidi_fondo(q, s, nato).potenza == PotenzaEco::bassa);
    CHECK_FALSE(decidi_fondo(q, s, nato).bqs15);   // in RIDOTTO solo nei campi minati
    s.campo_minato = true;
    CHECK(decidi_fondo(q, s, nato).bqs15);
    // appena sondato: non si risonda prima del periodo, se non e' critico
    s.livello = LivelloEmissioni::navigazione;
    s.campo_minato = false;
    s.ultimo_sondaggio_t = s.t - 30.0;
    s.quota_m = 100.0;
    const DecisioneFondo presto = decidi_fondo(q, s, nato);
    if (s.quota_m < presto.fondo_prudente_m - nato.margine_m) CHECK_FALSE(presto.sonda);
}

TEST_CASE("sondaggio dell'IA: in SILENZIO solo per un margine critico, a bassa potenza; i sovietici sondano di rado") {
    const Fondale scarsa = fondale(monte, false);
    const QuadroFondale q(scarsa);
    const DottrinaFondo nato = dottrina_fondo(false), urss = dottrina_fondo(true);
    SituazioneFondo s = situazione(scarsa, 150.0, LivelloEmissioni::silenzio);
    const DecisioneFondo d = decidi_fondo(q, s, nato);
    // vicino ma non critico: niente
    if (s.quota_m < d.fondo_prudente_m - nato.margine_m) CHECK_FALSE(d.sonda);
    CHECK_FALSE(d.bqs15);
    // critico: un ping a bassa potenza, non piu' di uno al minuto
    s.quota_m = d.fondo_prudente_m - 0.5 * nato.margine_m;
    const DecisioneFondo c = decidi_fondo(q, s, nato);
    CHECK(c.sonda);
    CHECK(c.potenza == PotenzaEco::bassa);
    s.ultimo_sondaggio_t = s.t - 20.0;
    CHECK_FALSE(decidi_fondo(q, s, nato).sonda);
    // la dottrina: i sovietici piu' vicini al fondo e con sondaggi piu' rari; la prudenza alza i margini
    CHECK(urss.margine_m < nato.margine_m);
    CHECK(urss.periodo_s > nato.periodo_s);
    CHECK(dottrina_fondo(false, 1.0).margine_m > dottrina_fondo(false, 0.0).margine_m);
    CHECK(std::string(nome_livello_emissioni(LivelloEmissioni::ridotto)) == "RIDOTTO");
}

TEST_CASE("sondaggio dell'IA: una misura recente sotto la chiglia basta, finche' non invecchia") {
    const Fondale vero = fondale(monte, true);
    const Fondale scarsa = fondale(monte, false);
    QuadroFondale q(scarsa);
    const DottrinaFondo nato = dottrina_fondo(false);
    SituazioneFondo s = situazione(scarsa, 300.0, LivelloEmissioni::navigazione);
    s.t = 100.0;
    Casuale caso(3);
    ParametriEcoscandaglio p;
    p.falso_fondo = 0.0;
    p.falso_scia = 0.0;
    q.aggiungi(sonda(vero, s.pos_stimata, s.pos_stimata, s.quota_m, s.velocita_kn, s.t, PotenzaEco::piena, true, 0.0, p, caso), 50.0);
    s.ultimo_sondaggio_t = s.t;
    s.t += nato.periodo_s + 1.0;   // dopo il periodo, ma la misura e' ancora buona
    const DecisioneFondo d = decidi_fondo(q, s, nato);
    if (d.fondo_prudente_m - s.quota_m >= nato.margine_m) CHECK_FALSE(d.sonda);
    s.t += nato.eta_max_s;          // la misura invecchia: si risonda
    CHECK(decidi_fondo(q, s, nato).sonda);
}

// Navigazione incerta su un pendio ripido (segnalazione di trailer-1, I16 alle Curili): la sigma di posizione di qualche km
// non deve portare il fondo prudente sopra il fondo piu' basso della carta vicino. Il pendio sale da 3000 a 200 m in 10 km
// verso est; il battello e' dove la carta da' 1750 m
TEST_CASE("sondaggio dell'IA: con la posizione incerta su un pendio ripido il fondo prudente resta quello della carta vicina") {
    const Vec2 centro = fondale([](double, double) { return 1000.0; }, true).centro_cella(N / 2, N / 2);
    const auto pendio = [centro](double x, double) { return std::clamp(1750.0 - 0.28 * (x - centro.x), 200.0, 3000.0); };
    const Fondale f = fondale(pendio, true);
    const QuadroFondale q(f);
    SituazioneFondo s;
    s.pos_stimata = centro;
    s.quota_m = 150.0;
    s.rotta = 0.0;   // lungo l'isobata
    s.velocita_kn = 8.0;
    s.t = 1000.0;
    s.livello = LivelloEmissioni::navigazione;
    const DottrinaFondo sov = dottrina_fondo(true);
    for (const double rotta : {0.0, 90.0}) {   // lungo l'isobata, e su per il pendio verso la costa
        s.rotta = rotta;
        double prima = 1e9;
        for (const double sigma : {0.0, 1000.0, 3000.0}) {
            CAPTURE(rotta);
            CAPTURE(sigma);
            s.sigma_posizione_m = sigma;
            const DecisioneFondo d = decidi_fondo(q, s, sov);
            MESSAGE("rotta ", rotta, ", sigma ", sigma, " m: fondo prudente ", d.fondo_prudente_m, ", quota max ", d.quota_max_m);
            CHECK(d.quota_max_m > 300.0);   // mai la risalita a 20 m in acqua profonda
            CHECK(d.fondo_prudente_m <= prima + 1e-9);   // piu' incerta la posizione, piu' prudente
            prima = d.fondo_prudente_m;
        }
    }
}
