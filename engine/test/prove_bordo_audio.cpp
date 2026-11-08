// Ambiente sonoro di bordo (R3): scafo, fondo delle stazioni, soglia, eventi; niente aerei.
#include <doctest.h>

#include <algorithm>
#include <cmath>

#include "nucleo/acustica_asw.hpp"
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

double dbfs(const std::vector<float>& v, std::size_t da = 0, std::size_t a = 0) {
    if (a == 0 || a > v.size()) a = v.size();
    double q = 0.0;
    for (std::size_t i = da; i < a; ++i) q += static_cast<double>(v[i]) * v[i];
    return 10.0 * std::log10(std::max(q / static_cast<double>(a - da), 1e-30));
}

std::vector<float> ascolta(const std::vector<Sorgente>& s, StazioneBordo st, RegimeBordo rg, double secondi = 2.0) {
    AmbienteBordo b;
    return b.passo(s, {0.0, 0.0}, 100.0, mare(), st, rg, secondi);
}

Sorgente nave(Vec2 pos, double kn) {
    UnitaAcustica u;
    u.id = 7;
    u.classe = "perry";
    u.categoria = "nave";
    u.pos = pos;
    u.velocita_kn = kn;
    u.rotta = 90.0;
    return sorgente_acustica(catalogo(), u);
}

}  // namespace

TEST_CASE("bordo: lo scafo e' un passa basso, il fondo e la soglia") {
    CHECK(trasferimento_scafo_db(100.0, StazioneBordo::centrale) == doctest::Approx(91.5 - 4.0));
    CHECK(trasferimento_scafo_db(4000.0, StazioneBordo::centrale) > trasferimento_scafo_db(1000.0, StazioneBordo::centrale) + 10.0);
    CHECK(trasferimento_scafo_db(1000.0, StazioneBordo::siluri) < trasferimento_scafo_db(1000.0, StazioneBordo::centrale));
    CHECK(fondo_stazione_dba(StazioneBordo::macchine, RegimeBordo::normale) == doctest::Approx(87.0));
    CHECK(fondo_stazione_dba(StazioneBordo::centrale, RegimeBordo::silenzioso) < fondo_stazione_dba(StazioneBordo::centrale, RegimeBordo::normale));
    CHECK(soglia_udibilita_db(20.0) > soglia_udibilita_db(1000.0) + 60.0);
    CHECK(soglia_udibilita_db(4000.0) < 0.0);
    // il fondo in uscita: la sala macchine ~25 dB sopra la centrale, il silenzio sotto il normale
    const double macchine = dbfs(ascolta({}, StazioneBordo::macchine, RegimeBordo::normale), 4096);
    const double centrale = dbfs(ascolta({}, StazioneBordo::centrale, RegimeBordo::normale), 4096);
    const double zitti = dbfs(ascolta({}, StazioneBordo::centrale, RegimeBordo::silenzioso), 4096);
    CHECK(macchine - centrale == doctest::Approx(25.0).epsilon(0.3));
    CHECK(zitti < centrale - 8.0);
    CHECK(macchine < 0.0);
    CHECK(std::string(nome_stazione_bordo(StazioneBordo::siluri)) == "locale siluri");
}

TEST_CASE("bordo: le eliche di chi passa sopra si sentono, quelle lontane no; gli aerei mai") {
    const double vuoto = dbfs(ascolta({}, StazioneBordo::centrale, RegimeBordo::silenzioso), 4096);
    const double sopra = dbfs(ascolta({nave({30.0, 0.0}, 18.0)}, StazioneBordo::centrale, RegimeBordo::silenzioso), 4096);
    const double lontana = dbfs(ascolta({nave({20000.0, 0.0}, 18.0)}, StazioneBordo::centrale, RegimeBordo::silenzioso), 4096);
    CHECK(sopra > vuoto + 3.0);   // dossier 18 §6.1: 40-60 dB dentro, sopra il fondo della barca silenziosa
    CHECK(std::fabs(lontana - vuoto) < 0.5);
    // in sala macchine la stessa nave si perde nel fondo
    const double macchine = dbfs(ascolta({}, StazioneBordo::macchine, RegimeBordo::normale), 4096);
    const double macchine_sopra = dbfs(ascolta({nave({30.0, 0.0}, 18.0)}, StazioneBordo::macchine, RegimeBordo::normale), 4096);
    CHECK(macchine_sopra - macchine < sopra - vuoto);
    // un P-3 sulla verticale: a bordo niente (scelta dell'utente), campioni identici
    AereoAcustico p3;
    p3.id = 50;
    p3.classe = "p3c";
    p3.quota_m = 91.44;
    const auto s = sorgente_aerea(catalogo(), p3, {0.0, 0.0}, 100.0, 10.0);
    REQUIRE(s.has_value());
    CHECK(ascolta({*s}, StazioneBordo::centrale, RegimeBordo::silenzioso) == ascolta({}, StazioneBordo::centrale, RegimeBordo::silenzioso));
}

TEST_CASE("bordo: ping altrui come ticchettio, tonfi, ping proprio, scoppi") {
    const auto& c = catalogo();
    const auto prova = [&](double distanza) {
        Transitorio t;
        t.id = 900;
        t.tipo = "ping_aqs13f";   // il sonar a immersione sopra il battello
        t.pos = {distanza, 0.0};
        t.quota_m = 60.0;
        t.t0 = 0.0;
        AmbienteBordo b;
        std::vector<float> v;
        for (int k = 0; k < 20; ++k) {   // passi da 0,05 s: il ping comincia a t0
            std::vector<Sorgente> s;
            if (const auto x = sorgente_transitoria(c, t, 0.05 * (k + 1), 0.05)) s.push_back(*x);
            const auto w = b.passo(s, {0.0, 0.0}, 100.0, mare(), StazioneBordo::siluri, RegimeBordo::silenzioso, 0.05);
            v.insert(v.end(), w.begin(), w.end());
        }
        return v;
    };
    const auto vicino = prova(150.0), lontano = prova(40000.0);
    const double fondo = dbfs(lontano, 4096);
    CHECK(dbfs(vicino, 0, 441) > fondo + 10.0);    // il colpetto sullo scafo nei primi 20 ms
    CHECK(dbfs(lontano, 0, 441) < fondo + 3.0);

    // scoppio: 100 kg a 300 m forte e con lo scossone, a 30 km appena
    AmbienteBordo b;
    b.passo({}, {0.0, 0.0}, 100.0, mare(), StazioneBordo::centrale, RegimeBordo::normale, 1.0);
    b.scoppio(300.0, 100.0, 50.0);
    CHECK(b.scossone() > 0.5);
    const auto colpo = b.passo({}, {0.0, 0.0}, 100.0, mare(), StazioneBordo::centrale, RegimeBordo::normale, 1.0);
    AmbienteBordo d;
    d.passo({}, {0.0, 0.0}, 100.0, mare(), StazioneBordo::centrale, RegimeBordo::normale, 1.0);
    d.scoppio(30000.0, 1.0, 50.0);
    CHECK(d.scossone() < 0.1);
    const auto eco = d.passo({}, {0.0, 0.0}, 100.0, mare(), StazioneBordo::centrale, RegimeBordo::normale, 1.0);
    float picco_vicino = 0.0f, picco_lontano = 0.0f;
    for (float x : colpo) picco_vicino = std::max(picco_vicino, std::fabs(x));
    for (float x : eco) picco_lontano = std::max(picco_lontano, std::fabs(x));
    CHECK(picco_vicino > 0.7f);                    // saturato (tanh): oltre 130 dB dentro
    CHECK(picco_lontano < 0.3f);

    // il ping proprio: forte nel locale siluri, debole sopra il fondo della sala macchine
    const auto ping = [&](StazioneBordo st, RegimeBordo rg) {
        AmbienteBordo a;
        a.passo({}, {0.0, 0.0}, 100.0, mare(), st, rg, 0.5);
        const auto base = a.passo({}, {0.0, 0.0}, 100.0, mare(), st, rg, 0.5);
        a.ping_proprio(225.0, 3500.0, 0.5);
        const auto con = a.passo({}, {0.0, 0.0}, 100.0, mare(), st, rg, 0.5);
        return dbfs(con) - dbfs(base);
    };
    CHECK(ping(StazioneBordo::siluri, RegimeBordo::normale) > ping(StazioneBordo::macchine, RegimeBordo::normale) + 10.0);
    CHECK(ping(StazioneBordo::siluri, RegimeBordo::normale) > 20.0);
    // ripetibile
    CHECK(ascolta({nave({30.0, 0.0}, 18.0)}, StazioneBordo::centrale, RegimeBordo::normale) ==
          ascolta({nave({30.0, 0.0}, 18.0)}, StazioneBordo::centrale, RegimeBordo::normale));
}

TEST_CASE("bordo: le macchine proprie per stazione, i colpi del battello, la ventilazione (R3b)") {
    const auto proprio = [](double kn, RegimePompe regime = RegimePompe::automatico) {
        UnitaAcustica u;
        u.id = 0;
        u.classe = "los_angeles";
        u.categoria = "sottomarino";
        u.quota_m = 100.0;
        u.velocita_kn = kn;
        u.variazioni = true;
        u.seme = 3;
        u.regime_pompe = regime;
        return sorgente_acustica(catalogo(), u);
    };
    const auto ascolta = [&](StazioneBordo st, const Sorgente* s, std::vector<Sorgente> scena = {}) {
        AmbienteBordo b;
        b.passo({}, {0.0, 0.0}, 100.0, mare(), st, RegimeBordo::normale, 0.5, s);
        const auto v = b.passo(scena, {0.0, 0.0}, 100.0, mare(), st, RegimeBordo::normale, 2.0, s);
        CHECK(b.stazione() == st);
        CHECK(b.regime() == RegimeBordo::normale);
        return v;
    };
    const auto goertzel = [](const std::vector<float>& x, double hz) {
        const double w = 2.0 * 3.14159265358979323846 * hz / 22050.0, c = 2.0 * std::cos(w);
        double s1 = 0.0, s2 = 0.0;
        for (std::size_t i = 4096; i < x.size(); ++i) {
            const double s0 = x[i] + c * s1 - s2;
            s2 = s1;
            s1 = s0;
        }
        return 2.0 * std::hypot(s1 - s2 * std::cos(w), s2 * std::sin(w)) / static_cast<double>(x.size() - 4096);
    };
    const Sorgente lento = proprio(10.0), veloce = proprio(25.0, RegimePompe::veloci);
    // senza la sorgente propria: come prima; con: diverso
    CHECK(ascolta(StazioneBordo::macchine, nullptr) == ascolta(StazioneBordo::macchine, nullptr));
    CHECK(ascolta(StazioneBordo::macchine, &lento) != ascolta(StazioneBordo::macchine, nullptr));
    // la pompa lenta del Los Angeles (42,3 Hz, con lo scarto dell'individuo) forte in sala macchine, molto meno in centrale
    double pompa = 0.0;
    for (const auto& r : righe_emesse(lento)) if (r.tipo == TipoRiga::pompa) pompa = r.hz;
    REQUIRE(pompa > 0.0);
    const double in_macchine = goertzel(ascolta(StazioneBordo::macchine, &lento), pompa);
    CHECK(in_macchine > 5.0 * goertzel(ascolta(StazioneBordo::centrale, &lento), pompa));
    // pompe spente (circolazione naturale): la riga sparisce
    const Sorgente spente = proprio(10.0, RegimePompe::spente);
    CHECK(goertzel(ascolta(StazioneBordo::macchine, &spente), pompa) < 0.2 * in_macchine);
    // piu' veloce, piu' forte (il fischio della turbina e le pompe veloci)
    CHECK(dbfs(ascolta(StazioneBordo::macchine, &veloce), 4096) > dbfs(ascolta(StazioneBordo::macchine, &lento), 4096));
    // un portello che sbatte a bordo (voce R7 del battello, a due passi): forte dentro anche in centrale
    Transitorio t;
    t.id = 960001;
    t.tipo = "portello_sbatte";
    t.pos = {2.0, 0.0};
    t.quota_m = 100.0;
    t.t0 = 0.0;
    const auto s = sorgente_transitoria(catalogo(), t, 0.05, 0.25);
    REQUIRE(s.has_value());
    const auto quiete = ascolta(StazioneBordo::centrale, &lento);
    const auto colpo = ascolta(StazioneBordo::centrale, &lento, {*s});
    CHECK(dbfs(colpo, 0, 441) > dbfs(quiete, 0, 441) + 6.0);   // i primi 20 ms: il colpo dura pochi millisecondi
}
