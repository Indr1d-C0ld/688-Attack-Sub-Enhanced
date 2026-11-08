#include <doctest.h>

#include <algorithm>
#include <chrono>
#include <cmath>
#include <numeric>
#include <vector>

#include "nucleo/analisi_acustica.hpp"
#include "nucleo/interferenza.hpp"

using namespace m688;

// Frange d'interferenza sul LOFAR (blocco 1 del sonar, docs/20_sonar_blocco1.md §4)

namespace {

Ambiente calmo(double fondale_m, TipoFondo fondo = TipoFondo::sabbia) {
    Ambiente a;
    a.fondale_m = fondale_m;
    a.fondo = fondo;
    a.vento_m_s = 0.0;
    a.hs_m = 0.0;   // superficie liscia: lo specchio e' perfetto
    return a;
}

// media in potenza dei dB (il livello medio che vede la larga banda)
double media_db(const std::vector<float>& v) {
    double s = 0.0;
    for (float x : v) s += std::pow(10.0, x / 10.0);
    return 10.0 * std::log10(s / static_cast<double>(v.size()));
}

double escursione(const std::vector<float>& v) {
    const auto [lo, hi] = std::minmax_element(v.begin(), v.end());
    return *hi - *lo;
}

double correlazione(const std::vector<double>& a, const std::vector<double>& b) {
    const double n = static_cast<double>(a.size());
    const double ma = std::accumulate(a.begin(), a.end(), 0.0) / n, mb = std::accumulate(b.begin(), b.end(), 0.0) / n;
    double sab = 0.0, saa = 0.0, sbb = 0.0;
    for (std::size_t i = 0; i < a.size(); ++i) {
        sab += (a[i] - ma) * (b[i] - mb);
        saa += (a[i] - ma) * (a[i] - ma);
        sbb += (b[i] - mb) * (b[i] - mb);
    }
    return sab / std::sqrt(saa * sbb);
}

}  // namespace

TEST_CASE("interferenza: le immagini della guida d'onda") {
    // acqua profonda (fondale ignoto): diretto e superficie
    const auto profonda = immagini_cammini(2000.0, 50.0, 100.0, 1e9, 4);
    REQUIRE(profonda.size() == 2);
    CHECK(profonda[0].lunghezza_m == doctest::Approx(std::hypot(2000.0, 50.0)));
    CHECK(profonda[1].lunghezza_m == doctest::Approx(std::hypot(2000.0, 150.0)));
    CHECK(profonda[1].superficie == 1);
    // fondale di 100 m: quattro per famiglia, con le riflessioni giuste
    const auto bassa = immagini_cammini(5000.0, 10.0, 50.0, 100.0, 3);
    REQUIRE(bassa.size() == 12);
    CHECK(bassa[1].lunghezza_m == doctest::Approx(std::hypot(5000.0, 200.0 - 60.0)));   // il fondo
    CHECK(bassa[1].fondo == 1);
    CHECK(bassa[1].superficie == 0);
    CHECK(bassa[7].superficie == 2);
    CHECK(bassa[7].fondo == 2);
    for (const auto& im : bassa) CHECK(im.lunghezza_m >= 5000.0);
}

TEST_CASE("interferenza: lo specchio di Lloyd, zeri e massimi dove dice la formula") {
    const Ambiente amb = calmo(1e9);
    const double r = 2000.0, zs = 50.0, zr = 100.0;
    const Interferenza in(r, zs, zr, amb);
    const auto im = in.immagini();
    const double c0 = specchio_lloyd(100.0, r, zs, zr, amb).c0_m_s;
    const double dl = im[1].lunghezza_m - im[0].lunghezza_m;   // circa 2 zs zr / r = 5 m
    CHECK(dl == doctest::Approx(2.0 * zs * zr / r).epsilon(0.01));
    // la superficie inverte il segno: zeri a n c / dl, massimi (+3 dB sul livello incoerente) a meta'
    for (int n = 1; n <= 3; ++n) {
        const double zero = n * c0 / dl, massimo = (n + 0.5) * c0 / dl;
        CAPTURE(n);
        CHECK(in.db(zero) < -20.0);
        CHECK(in.db(massimo) == doctest::Approx(10.0 * std::log10(2.0)).epsilon(0.05));
    }
    // il passo delle frange e' c / dl: circa 300 Hz
    CHECK(c0 / dl == doctest::Approx(c0 * r / (2.0 * zs * zr)).epsilon(0.01));
}

TEST_CASE("interferenza: il livello medio non cambia e lontano (il dipolo del livello A) il rapporto vale 1") {
    // vicino: frange forti, ma in media sulle frequenze il livello e' quello del livello A
    const Interferenza vicino(2000.0, 50.0, 100.0, calmo(1e9));
    const auto v = vicino.db_griglia(300.0, 0.5, 3600);   // 300-2100 Hz: sei frange
    CHECK(escursione(v) > 20.0);
    CHECK(std::fabs(media_db(v)) < 0.5);
    // acqua bassa su sabbia: tante immagini, la media resta
    const Interferenza bassa(5000.0, 10.0, 50.0, calmo(100.0));
    const auto b = bassa.db_griglia(50.0, 0.1, 9500);
    CHECK(escursione(b) > 10.0);
    CHECK(std::fabs(media_db(b)) < 1.0);
    // lontano, sotto pi/4: il dipolo e' gia' nel livello A
    const Interferenza lontano(20000.0, 6.0, 60.0, calmo(1e9));
    for (double f : {20.0, 50.0, 100.0}) CHECK(std::fabs(lontano.db(f)) < 0.5);
}

TEST_CASE("interferenza: la griglia e il calcolo puntuale coincidono") {
    const Interferenza in(5000.0, 10.0, 50.0, calmo(100.0));
    const double f0 = 100.0, df = 0.1;
    const auto g = in.db_griglia(f0, df, 2000);
    int confrontati = 0, lontani = 0;
    for (std::size_t i = 0; i < g.size(); i += 37) {
        const double puntuale = in.db(f0 + (static_cast<double>(i) + 0.5) * df);
        if (std::fabs(puntuale) > 15.0) continue;   // vicino a uno zero la pendenza e' ripida
        ++confrontati;
        if (std::fabs(g[i] - puntuale) > 0.1) ++lontani;
    }
    CHECK(confrontati > 40);
    CHECK(lontani <= confrontati / 20);
}

TEST_CASE("interferenza: in acqua bassa le frange scalano con la distanza (invariante di guida circa 1)") {
    // f tau resta costante con tau proporzionale a 1 / r: la figura a 6 km e' quella a 5 km a frequenze 1,2 volte piu'
    // alte. E' la «vasca da bagno»: le frange salgono quando il contatto si allontana
    const Ambiente amb = calmo(100.0);
    const Interferenza a(5000.0, 10.0, 50.0, amb), b(6000.0, 10.0, 50.0, amb);
    std::vector<double> va, vb_scalata, vb_ferma;
    for (double f = 100.0; f < 300.0; f += 0.2) {
        va.push_back(a.db(f));
        vb_scalata.push_back(b.db(f * 1.2));
        vb_ferma.push_back(b.db(f));
    }
    const double scalata = correlazione(va, vb_scalata), ferma = correlazione(va, vb_ferma);
    MESSAGE("correlazione con la frequenza scalata " << scalata << ", senza " << ferma);
    CHECK(scalata > 0.8);
    CHECK(ferma < scalata - 0.3);
}

TEST_CASE("interferenza: il mare mosso, il fango, la distanza e il moto spengono le frange") {
    // a 1,5-2,5 kHz con radenza alta: con Hs 4 m lo specchio non e' piu' coerente
    Ambiente mosso = calmo(1e9);
    mosso.hs_m = 4.0;
    const Interferenza liscio(500.0, 50.0, 100.0, calmo(1e9)), agitato(500.0, 50.0, 100.0, mosso);
    CHECK(escursione(liscio.db_griglia(1500.0, 1.0, 1000)) > 20.0);
    CHECK(escursione(agitato.db_griglia(1500.0, 1.0, 1000)) < 1.0);
    // fango: i rimbalzi sul fondo pesano poco, resta quasi solo lo specchio
    const Interferenza sabbia(5000.0, 10.0, 50.0, calmo(100.0, TipoFondo::sabbia));
    const Interferenza fango(5000.0, 10.0, 50.0, calmo(100.0, TipoFondo::fango));
    CHECK(escursione(fango.db_griglia(100.0, 0.1, 2000)) < escursione(sabbia.db_griglia(100.0, 0.1, 2000)));
    // i cammini col fondo perdono coerenza con la distanza
    ParametriInterferenza corta;
    corta.coerenza_fondo_m = 1000.0;
    const Interferenza spenta(5000.0, 10.0, 50.0, calmo(100.0), 0.0, corta);
    CHECK(escursione(spenta.db_griglia(100.0, 0.1, 2000)) < 0.5 * escursione(sabbia.db_griglia(100.0, 0.1, 2000)));
    // in moto: 200 m nel tempo d'integrazione spianano le frange fitte
    const Interferenza fermo(2000.0, 50.0, 100.0, calmo(1e9)), in_moto(2000.0, 50.0, 100.0, calmo(1e9), 400.0);
    CHECK(escursione(in_moto.db_griglia(1500.0, 0.5, 1000)) < escursione(fermo.db_griglia(1500.0, 0.5, 1000)));
}

TEST_CASE("interferenza: le frange sullo spettro del fascio, con il livello medio del livello A") {
    static const CatalogoAcustico c = carica_acustica(M688_DATI "/acustica.toml");
    Ambiente amb = calmo(80.0);
    amb.traffico = 0.3;
    Ricevitore r;
    r.quota_m = 40.0;
    r.velocita_kn = 4.0;
    r.sensore = c.sensore("tb16");
    Sorgente kilo;
    kilo.firma = c.firma("kilo");
    kilo.velocita_kn = 3.0;
    kilo.quota_m = 50.0;
    kilo.pos = {3000.0, 0.0};
    kilo.rotta = 0.0;
    kilo.snorkel = true;   // i diesel: righe forti e continuo alto, il S/N si legge bene
    const auto t0 = std::chrono::steady_clock::now();
    const Spettro con = spettro_fascio({kilo}, r, amb, 90.0, 20.0, 420.0, 0.1, true);
    const double ms = std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - t0).count();
    const Spettro senza = spettro_fascio({kilo}, r, amb, 90.0, 20.0, 420.0, 0.1, false);
    const Spettro prima = spettro_fascio({kilo}, r, amb, 90.0, 20.0, 420.0, 0.1);
    MESSAGE("spettro di 4000 bin con le frange: " << ms << " ms");
    REQUIRE(con.snr_db.size() == senza.snr_db.size());
    CHECK(senza.snr_db == prima.snr_db);   // senza frange e' lo spettro di sempre
    // frange: lo spettro scende e sale di parecchi dB attorno a quello di prima, con lo stesso segnale medio
    double piu = 0.0, meno = 0.0, s_con = 0.0, s_senza = 0.0;
    for (std::size_t i = 0; i < con.snr_db.size(); ++i) {
        piu = std::max(piu, static_cast<double>(con.snr_db[i] - senza.snr_db[i]));
        meno = std::min(meno, static_cast<double>(con.snr_db[i] - senza.snr_db[i]));
        s_con += std::pow(10.0, con.snr_db[i] / 10.0) - 1.0;
        s_senza += std::pow(10.0, senza.snr_db[i] / 10.0) - 1.0;
    }
    MESSAGE("frange da " << meno << " a +" << piu << " dB; segnale medio " << 10.0 * std::log10(s_con / s_senza) << " dB");
    CHECK(piu > 2.0);
    CHECK(meno < -5.0);
    CHECK(std::fabs(10.0 * std::log10(s_con / s_senza)) < 1.5);
    // le righe sentono le frange: qualcuna sale e qualcuna scende
    const auto rc = righe_ricevute({kilo}, r, amb, 90.0, 20.0, 420.0, 0.1, true);
    const auto rs = righe_ricevute({kilo}, r, amb, 90.0, 20.0, 420.0, 0.1);
    REQUIRE(rc.size() == rs.size());
    int su = 0, giu = 0;
    for (std::size_t i = 0; i < rc.size(); ++i) {
        su += rc[i].snr_db > rs[i].snr_db + 1.0;
        giu += rc[i].snr_db < rs[i].snr_db - 1.0;
    }
    CHECK(su > 0);
    CHECK(giu > 0);
}
