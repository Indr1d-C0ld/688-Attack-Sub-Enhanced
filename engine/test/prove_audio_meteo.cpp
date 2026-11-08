// Audio del maltempo (M5): fulmini al sonar e in cuffia, tuono e colpi di mare a bordo, scariche in sala radio.
#include <doctest.h>

#include <algorithm>
#include <cmath>

#include "nucleo/audio_meteo.hpp"
#include "nucleo/bordo_audio.hpp"

using namespace m688;

namespace {

const CatalogoAcustico& catalogo() {
    static const CatalogoAcustico c = carica_acustica(M688_DATI "/acustica.toml");
    return c;
}

Ambiente mare(int ss = 3) {
    Ambiente a;
    a.fondale_m = 3000.0;
    a.vento_m_s = vento_per_stato_del_mare(ss);
    return a;
}

double dbfs(const std::vector<float>& v, std::size_t da = 0, std::size_t a = 0) {
    if (a == 0 || a > v.size()) a = v.size();
    double q = 0.0;
    for (std::size_t i = da; i < a; ++i) q += static_cast<double>(v[i]) * v[i];
    return 10.0 * std::log10(std::max(q / static_cast<double>(a - da), 1e-30));
}

// Vd (dB): rms dell'inviluppo sulla sua media, con l'inviluppo dal quadrato mediato su 0,3 ms (sul rumore gaussiano
// da' ~0,5 dB invece di 1,05: si confronta)
double vd(const std::vector<float>& x) {
    const double a = 1.0 - std::exp(-1.0 / (0.0003 * 22050.0));
    double e2 = 0.0, m1 = 0.0, m2 = 0.0;
    std::size_t n = 0;
    for (std::size_t i = 0; i < x.size(); ++i) {
        e2 += a * (static_cast<double>(x[i]) * x[i] - e2);
        if (i < 2205) continue;
        const double e = std::sqrt(std::max(2.0 * e2, 0.0));
        m1 += e;
        m2 += e * e;
        ++n;
    }
    return 20.0 * std::log10(std::sqrt(m2 / static_cast<double>(n)) / (m1 / static_cast<double>(n)));
}

// livello sotto ~200 Hz (passa basso a un polo, due volte)
double grave_db(const std::vector<float>& v, std::size_t da, std::size_t a) {
    const double k = 1.0 - std::exp(-2.0 * 3.14159265358979323846 * 200.0 / 22050.0);
    double f1 = 0.0, f2 = 0.0, q = 0.0;
    for (std::size_t i = 0; i < a; ++i) {
        f1 += k * (v[i] - f1);
        f2 += k * (f1 - f2);
        if (i >= da) q += f2 * f2;
    }
    return 10.0 * std::log10(std::max(q / static_cast<double>(a - da), 1e-30));
}

double ampiezza(const std::vector<float>& x, double hz, int fs, std::size_t da, std::size_t a) {   // Goertzel
    const double w = 2.0 * 3.14159265358979323846 * hz / fs, c = 2.0 * std::cos(w);
    double s1 = 0.0, s2 = 0.0;
    for (std::size_t i = da; i < a; ++i) {
        const double s0 = x[i] + c * s1 - s2;
        s2 = s1;
        s1 = s0;
    }
    return 2.0 * std::hypot(s1 - s2 * std::cos(w), s2 * std::sin(w)) / static_cast<double>(a - da);
}

Lampo lampo_a(double x, int colpi = 4) {
    Lampo l;
    l.pos = {x, 0.0};
    l.t = 10.0;
    l.colpi = colpi;
    l.nube_mare = true;
    return l;
}

// la sorgente del primo colpo di un lampo al tempo t
std::optional<Sorgente> colpo(const Lampo& l, double t, int k = 0) {
    const auto x = transitori_lampo(l, 500, 7);
    return sorgente_transitoria(catalogo(), x.at(static_cast<std::size_t>(k)), t, 0.25);
}

}  // namespace

TEST_CASE("audio meteo: i colpi di un lampo nube-mare come transitori") {
    CHECK(transitori_lampo(Lampo{{0.0, 0.0}, 5.0, 3, false}, 1, 1).empty());   // nella nube: niente in acqua
    const auto x = transitori_lampo(lampo_a(5000.0, 4), 100, 3);
    REQUIRE(x.size() == 4);
    CHECK(x[0].t0 == doctest::Approx(10.0));
    CHECK(x[0].guadagno_db == 0.0);
    for (std::size_t k = 1; k < x.size(); ++k) {
        CHECK(x[k].id == 100 + static_cast<int>(k));
        CHECK(x[k].t0 > x[k - 1].t0);
        CHECK(x[k].guadagno_db <= 0.0);
        CHECK(x[k].tipo == "fulmine_mare");
    }
    CHECK(transitori_lampo(lampo_a(5000.0, 4), 100, 3)[3].t0 == x[3].t0);   // deterministici
    // gli intervalli: mediana ~57 ms
    std::vector<double> d;
    for (int i = 0; i < 300; ++i) {
        const auto y = transitori_lampo(lampo_a(0.0, 2), 1, static_cast<std::uint64_t>(i));
        d.push_back(y[1].t0 - y[0].t0);
    }
    std::nth_element(d.begin(), d.begin() + 150, d.end());
    CHECK(d[150] == doctest::Approx(0.057).epsilon(0.2));
}

TEST_CASE("audio meteo: il fulmine in acqua, tarato sulle osservazioni") {
    const auto* f = catalogo().transitorio("fulmine_mare");
    REQUIRE(f != nullptr);
    CHECK(f->categoria == "meteo");
    Ricevitore r;
    r.quota_m = 100.0;
    r.sensore = catalogo().sensore("ssq41b");   // un idrofono omnidirezionale
    REQUIRE(r.sensore != nullptr);
    const auto snr = [&](double x) {
        auto l = lampo_a(x);
        const auto s = colpo(l, 10.05);
        REQUIRE(s.has_value());
        return snr_impulso_db(*s, r, mare(3), 90.0, 150.0);
    };
    CHECK(snr(10000.0) == doctest::Approx(15.0).epsilon(0.3));   // [INPEX09]: fino a +15 dB a 5-10 km
    CHECK(snr(1000.0) > snr(10000.0) + 5.0);
    CHECK(snr(60000.0) < 0.0);                                  // oltre 60 km non si sente [MH370]
    // un sonar lo sente come transitorio (il falso allarme), lontano no
    Ricevitore sfera{{0.0, 0.0}, 100.0, 0.0, 5.0, catalogo().sensore("tb16")};
    auto vicino = lampo_a(5000.0), lontano = lampo_a(100000.0);
    CHECK(ascolta_transitorio(*colpo(vicino, 10.2), {sfera}, mare(3)).sentito);
    CHECK_FALSE(ascolta_transitorio(*colpo(lontano, 10.2), {sfera}, mare(3)).sentito);
}

TEST_CASE("audio meteo: lo schiocco in cuffia a campione esatto") {
    Ricevitore r{{0.0, 0.0}, 150.0, 0.0, 5.0, catalogo().sensore("tb16")};
    ParametriAscolto p;
    p.f_min_hz = 20.0;
    p.f_max_hz = 2000.0;
    const auto ascolta = [&](double x) {
        CuffiaSonar c(p);
        std::vector<float> v;
        auto prima = c.passo({}, r, mare(3), 90.0, 0.5);   // la scena quieta
        v.insert(v.end(), prima.begin(), prima.end());
        auto l = lampo_a(x);
        const auto s = colpo(l, 10.05);
        auto dopo = c.passo({*s}, r, mare(3), 90.0, 0.5);
        v.insert(v.end(), dopo.begin(), dopo.end());
        return v;
    };
    const std::size_t inizio = 11025;   // 0,5 s
    const auto vicino = ascolta(3000.0), lontano = ascolta(80000.0);
    const double fondo = dbfs(vicino, 0, inizio);
    CHECK(dbfs(vicino, inizio, inizio + 1100) > fondo + 10.0);    // i primi 50 ms
    CHECK(dbfs(vicino, inizio + 8000) < fondo + 3.0);              // poi torna il fondo
    CHECK(std::fabs(dbfs(lontano, inizio, inizio + 1100) - fondo) < 2.0);
    CHECK(ascolta(3000.0) == vicino);                              // ripetibile
    // il fascio puntato dall'altra parte lo sente meno
    CuffiaSonar c(p);
    c.passo({}, r, mare(3), 30.0, 0.5);   // non 270: la schiera trainata e' ambigua a specchio
    auto l = lampo_a(3000.0);
    const auto altrove = c.passo({*colpo(l, 10.05)}, r, mare(3), 30.0, 0.5);
    CHECK(dbfs(altrove, 0, 1100) < dbfs(vicino, inizio, inizio + 1100));
}

TEST_CASE("audio meteo: a bordo il tuono si sente solo in superficie, e dopo distanza / c") {
    const auto ascolta = [&](double quota, double km, bool tuona) {
        AmbienteBordo b;
        b.passo({}, {0.0, 0.0}, quota, mare(3), StazioneBordo::centrale, RegimeBordo::silenzioso, 0.5);
        if (tuona) b.tuono(km * 1000.0, true, 10.0);
        return b.passo({}, {0.0, 0.0}, quota, mare(3), StazioneBordo::centrale, RegimeBordo::silenzioso, 8.0);
    };
    const int fs = 22050;
    const auto quiete = ascolta(0.0, 1.0, false), vicino = ascolta(0.0, 1.0, true);
    const double fondo = dbfs(quiete);
    // a 1 km il suono arriva dopo ~2,9 s: prima niente, poi lo schianto
    CHECK(dbfs(vicino, 0, static_cast<std::size_t>(2.8 * fs)) == doctest::Approx(dbfs(quiete, 0, static_cast<std::size_t>(2.8 * fs))));
    CHECK(dbfs(vicino, static_cast<std::size_t>(3.0 * fs), static_cast<std::size_t>(4.0 * fs)) > fondo + 10.0);
    // in profondita' niente, oltre 20 km niente
    CHECK(ascolta(100.0, 1.0, true) == ascolta(100.0, 1.0, false));
    CHECK(ascolta(0.0, 25.0, true) == quiete);
    // a 10 km piu' debole che a 1 km
    const auto lontano = ascolta(0.0, 10.0, true);
    CHECK(dbfs(lontano, static_cast<std::size_t>(0.2 * fs)) < dbfs(vicino, static_cast<std::size_t>(2.9 * fs)));
}

TEST_CASE("audio meteo: colpi di mare e acqua sul ponte, solo vicino alla superficie") {
    StatoMeteo grosso;
    grosso.hs_vento_m = 5.0;   // mare 6
    grosso.tp_vento_s = 9.0;
    REQUIRE(grosso.stato_mare() == 6);
    StatoMeteo calmo;
    calmo.hs_vento_m = 0.3;
    const auto ascolta = [&](double quota, const StatoMeteo* m) {
        AmbienteBordo b;
        if (m) b.meteo(*m);
        return b.passo({}, {0.0, 0.0}, quota, mare(m ? m->stato_mare() : 3), StazioneBordo::centrale, RegimeBordo::silenzioso, 30.0);
    };
    const auto sopra = ascolta(0.0, &grosso), sotto = ascolta(0.0, nullptr);
    CHECK(dbfs(sopra) > dbfs(sotto) + 6.0);
    // in profondita' e col mare calmo come senza meteo
    const auto fondo = [&](double q, int ss) {
        AmbienteBordo b;
        return b.passo({}, {0.0, 0.0}, q, mare(ss), StazioneBordo::centrale, RegimeBordo::silenzioso, 30.0);
    };
    CHECK(ascolta(200.0, &grosso) == fondo(200.0, 6));
    CHECK(ascolta(0.0, &calmo) == fondo(0.0, calmo.stato_mare()));
    // a quota periscopica si sente meno che in superficie, ma si sente
    const double periscopica = dbfs(ascolta(18.0, &grosso));
    CHECK(periscopica < dbfs(sopra));
    CHECK(periscopica > dbfs(fondo(18.0, 6)) + 0.5);
    CHECK(ascolta(0.0, &grosso) == sopra);   // ripetibile
}

TEST_CASE("audio meteo: il fulmine caduto vicino si sente a bordo come un tonfo") {
    const auto ascolta = [&](double x) {
        AmbienteBordo b;
        auto l = lampo_a(x);
        b.passo({}, {0.0, 0.0}, 30.0, mare(3), StazioneBordo::siluri, RegimeBordo::silenzioso, 0.5);
        return b.passo({*colpo(l, 10.05)}, {0.0, 0.0}, 30.0, mare(3), StazioneBordo::siluri, RegimeBordo::silenzioso, 0.5);
    };
    // a 50 m ~75 dB a 150 Hz dentro il locale siluri: un tonfo grave sopra il fondo della barca silenziosa (a 200 m ~52
    // dB, appena sopra: lo scafo isola ~87 dB)
    const auto vicino = ascolta(50.0), lontano = ascolta(20000.0);
    CHECK(grave_db(vicino, 0, 2205) > grave_db(lontano, 0, 2205) + 6.0);
    CHECK(grave_db(vicino, 8820, 11025) == doctest::Approx(grave_db(lontano, 8820, 11025)).epsilon(0.05));   // poi passa
}

TEST_CASE("audio meteo: scariche in sala radio") {
    ParametriScariche p;
    // fondo al livello chiesto, ripetibile e indipendente da come si spezzano i passi
    ScaricheRadio a(p), b(p);
    const auto tutto = a.passo(2.0);
    std::vector<float> pezzi;
    for (int i = 0; i < 20; ++i) {
        const auto x = b.passo(0.1);
        pezzi.insert(pezzi.end(), x.begin(), x.end());
    }
    CHECK(tutto == pezzi);
    CHECK(std::fabs(dbfs(tutto, 2205) - p.fondo_dbfs) < 1.5);   // di giorno il crepitio pesa poco
    // di notte il crepitio e' impulsivo (Vd 5-9 dB, CCIR 322); il soppressore lo toglie
    const auto rumore = [&](bool notte, bool soppressore) {
        ParametriScariche q = p;
        q.soppressore = soppressore;
        ScaricheRadio s(q);
        s.ambiente({}, {0.0, 0.0}, notte);
        return s.passo(10.0);
    };
    const double giorno = vd(rumore(false, false)), notte = vd(rumore(true, false));
    CHECK(notte > 4.0);   // ~+0,6 dB sotto il Vd vero
    CHECK(notte < 8.5);
    CHECK(giorno < notte - 3.0);
    CHECK(vd(rumore(true, true)) < notte - 3.0);
    // una cella vicina alza il fondo di 10 dB, a 175 km di 5
    ScaricheRadio s(p);
    s.ambiente({CellaTemporale{{20000.0, 0.0}, 10000.0, 0.0, 3600.0, 3.0}}, {0.0, 0.0}, false);
    CHECK(s.aumento_fondo_db() == doctest::Approx(10.0));
    s.ambiente({CellaTemporale{{185000.0, 0.0}, 10000.0, 0.0, 3600.0, 3.0}}, {0.0, 0.0}, false);
    CHECK(s.aumento_fondo_db() == doctest::Approx(5.0));
    s.ambiente({}, {0.0, 0.0}, false);
    CHECK(s.aumento_fondo_db() == 0.0);
}

TEST_CASE("audio meteo: il crash di un lampo vicino e l'AGC che si mangia il segnale") {
    ParametriScariche p;
    const int fs = p.campionamento_hz;
    std::vector<float> segnale(static_cast<std::size_t>(6 * fs));
    for (std::size_t i = 0; i < segnale.size(); ++i)
        segnale[i] = static_cast<float>(0.25 * std::sin(2.0 * 3.14159265358979323846 * 1000.0 * static_cast<double>(i) / fs));
    ScaricheRadio s(p);
    auto prima = s.passo(1.0, &segnale);
    const double nota = ampiezza(prima, 1000.0, fs, 2205, prima.size());
    CHECK(nota == doctest::Approx(0.25).epsilon(0.1));
    s.lampo(lampo_a(10000.0), {0.0, 0.0});
    CHECK(s.crash_in_corso() == 1);
    std::vector<float> resto(segnale.begin() + fs, segnale.end());
    const auto dopo = s.passo(5.0, &resto);
    float picco = 0.0f;
    for (std::size_t i = 0; i < static_cast<std::size_t>(fs / 2); ++i) picco = std::max(picco, std::fabs(dopo[i]));
    CHECK(picco > 0.4f);                                                                  // il crash, +35 dB sul fondo
    CHECK(ampiezza(dopo, 1000.0, fs, 2205, 8820) < 0.7 * nota);                            // il segnale sparisce
    CHECK(ampiezza(dopo, 1000.0, fs, 3 * fs, 4 * fs) == doctest::Approx(nota).epsilon(0.1)); // e torna
    // un lampo lontano: crash debole, l'AGC non si muove
    ScaricheRadio t(p);
    t.passo(1.0, &segnale);
    t.lampo(lampo_a(2.0e6), {0.0, 0.0});
    const auto debole = t.passo(5.0, &resto);
    CHECK(ampiezza(debole, 1000.0, fs, 2205, 8820) == doctest::Approx(nota).epsilon(0.15));
}

TEST_CASE("audio meteo: in VLF di notte i tweek") {
    ParametriScariche p;
    p.banda = BandaRadio::vlf;
    const int fs = p.campionamento_hz;
    double energia_notte = 0.0, energia_giorno = 0.0;
    for (int i = 0; i < 12; ++i) {
        for (const bool notte : {true, false}) {
            ParametriScariche q = p;
            q.seme = static_cast<std::uint64_t>(i + 1);
            ScaricheRadio s(q);
            s.ambiente({}, {0.0, 0.0}, notte);
            s.passo(0.2);
            s.lampo(lampo_a(500000.0), {0.0, 0.0});
            const auto v = s.passo(0.2);
            (notte ? energia_notte : energia_giorno) += ampiezza(v, 1700.0, fs, 600, 2400);
        }
    }
    CHECK(energia_notte > 1.5 * energia_giorno);   // la coda a 1,7 kHz dei tweek
    CHECK(std::string(nome_banda_radio(BandaRadio::vlf)) == "VLF");
}
