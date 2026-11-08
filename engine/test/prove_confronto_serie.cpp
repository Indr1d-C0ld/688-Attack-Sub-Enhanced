#include <doctest.h>

#include <cmath>
#include <vector>

#include "nucleo/casuale.hpp"
#include "nucleo/confronto_serie.hpp"

using namespace m688;

// Il confronto statistico delle serie A/B del vetrino di Dio (docs/28 §5, D2)

TEST_CASE("confronto: l'intervallo di Wilson") {
    const IntervalloWilson m = wilson(5, 10);
    CHECK(m.p == doctest::Approx(0.5));
    CHECK(m.basso == doctest::Approx(0.2366).epsilon(1e-3));
    CHECK(m.alto == doctest::Approx(0.7634).epsilon(1e-3));
    const IntervalloWilson z = wilson(0, 10);   // anche a zero l'intervallo ha una larghezza
    CHECK(z.basso == doctest::Approx(0.0));
    CHECK(z.alto == doctest::Approx(0.2775).epsilon(1e-3));
    const IntervalloWilson t = wilson(20, 20);
    CHECK(t.alto == doctest::Approx(1.0));
    CHECK(t.basso == doctest::Approx(0.8389).epsilon(1e-3));
    CHECK(wilson(0, 0).n == 0);
}

TEST_CASE("confronto: la differenza degli esiti a coppie (Newcombe), piu' stretta delle serie indipendenti") {
    // nessuna differenza: zero, intervallo attorno a zero
    std::vector<bool> a(20), b(20);
    for (int i = 0; i < 20; ++i) a[i] = b[i] = i % 2 == 0;
    const ConfrontoEsito pari = confronta_esito(a, b);
    CHECK(pari.differenza == doctest::Approx(0.0));
    CHECK(pari.basso < 0.0);
    CHECK(pari.alto > 0.0);
    CHECK_FALSE(pari.significativo());
    CHECK(pari.entrambe == 10);
    CHECK(pari.nessuna == 10);
    // l'intervento rovescia l'esito in 8 coppie su 20 e nessuna nell'altro senso: +40 punti, significativo
    std::vector<bool> c = a;
    for (int i = 1; i < 16; i += 2) c[i] = true;
    const ConfrontoEsito su = confronta_esito(a, c);
    CHECK(su.solo_b == 8);
    CHECK(su.solo_a == 0);
    CHECK(su.differenza == doctest::Approx(0.4));
    CHECK(su.significativo());
    CHECK(su.basso > 0.1);
    CHECK(su.alto < 0.65);
    CHECK(su.a.p == doctest::Approx(0.5));
    CHECK(su.b.p == doctest::Approx(0.9));
    // il disegno a coppie: con le stesse frequenze ma coppie scorrelate l'intervallo e' piu' largo
    std::vector<bool> d(20), e(20);
    for (int i = 0; i < 20; ++i) {
        d[i] = i < 10;
        e[i] = i < 18;   // 18 su 20: le 10 di A restano, 8 coppie passano, nessuna torna
    }
    const ConfrontoEsito apparato = confronta_esito(d, e);
    std::vector<bool> f(20);
    for (int i = 0; i < 20; ++i) f[i] = i % 10 != 9;   // ancora 18 su 20 (+40 punti), ma una coppia torna indietro
    const ConfrontoEsito sciolto = confronta_esito(d, f);
    CHECK(apparato.differenza == doctest::Approx(sciolto.differenza));
    CHECK(apparato.alto - apparato.basso < sciolto.alto - sciolto.basso);
    // l'esempio pubblicato (Armitage e Berry 1994, p. 138; StatsDirect, «Paired Proportions»): crescita col mezzo A e col
    // mezzo B, 20 entrambi, 12 solo A, 2 solo B, 16 nessuno; A - B = 0,2 con l'intervallo 0,056156-0,329207
    std::vector<bool> ma, mb;
    const auto metti = [&](int k, bool x, bool y) { for (int i = 0; i < k; ++i) { ma.push_back(x); mb.push_back(y); } };
    metti(20, true, true);
    metti(12, true, false);
    metti(2, false, true);
    metti(16, false, false);
    const ConfrontoEsito ab = confronta_esito(mb, ma);   // B - A con B = il mezzo A: +0,2
    CHECK(ab.differenza == doctest::Approx(0.2));
    CHECK(ab.basso == doctest::Approx(0.056156).epsilon(1e-4));
    CHECK(ab.alto == doctest::Approx(0.329207).epsilon(1e-4));
    // tutto rovesciato: +1, l'intervallo resta dentro [-1, 1]
    const ConfrontoEsito tutto = confronta_esito(std::vector<bool>(20, false), std::vector<bool>(20, true));
    CHECK(tutto.differenza == doctest::Approx(1.0));
    CHECK(tutto.alto <= 1.0);
    CHECK(tutto.basso > 0.75);
}

TEST_CASE("confronto: la differenza delle mediane col bootstrap delle coppie, ripetibile; i mancanti tolgono la coppia") {
    std::vector<double> a, b;
    Casuale caso(11);
    for (int i = 0; i < 30; ++i) {
        const double x = 600.0 + 300.0 * caso.uniforme();
        a.push_back(x);
        b.push_back(x - 120.0 + 20.0 * (caso.uniforme() - 0.5));   // l'intervento anticipa la scoperta di 2 minuti
    }
    const ConfrontoNumero r = confronta_numero(a, b);
    CHECK(r.coppie == 30);
    CHECK(r.differenza == doctest::Approx(-120.0).epsilon(0.1));
    CHECK(r.basso < -100.0);
    CHECK(r.alto > -140.0);
    CHECK(r.alto < 0.0);
    CHECK(r.significativo());
    CHECK(r.scende == 30);
    // ripetibile col seme; la differenza non dipende dal seme
    const ConfrontoNumero s = confronta_numero(a, b);
    CHECK(s.basso == r.basso);
    CHECK(s.alto == r.alto);
    ParametriBootstrap altro;
    altro.seme = 99;
    CHECK(confronta_numero(a, b, altro).differenza == r.differenza);
    // un mancante (< 0) toglie la coppia; uguali: nessuna differenza
    std::vector<double> m = a;
    m[3] = -1.0;
    CHECK(confronta_numero(m, b).coppie == 29);
    const ConfrontoNumero u = confronta_numero(a, a);
    CHECK(u.differenza == doctest::Approx(0.0));
    CHECK(u.uguale == 30);
    CHECK_FALSE(u.significativo());
    CHECK(confronta_numero({}, {}).coppie == 0);
}

namespace {

// due tracce in memoria: la stessa partita fino a t = 25 s, poi in B la nave 2 accosta e c'e' un lancio in piu'
Traccia traccia(bool con_intervento) {
    Traccia t;
    int id_evento = 0;
    for (int k = 0; k <= 10; ++k) {
        Fotogramma f;
        f.t = 5.0 * k;
        StatoUnita u;
        u.id = 2;
        u.classe = "perry";
        u.categoria = "nave";
        u.flag = 10;
        u.pos = {100.0 * k, 0.0};
        u.rotta = 90.0;
        u.velocita_kn = 10.0;
        if (con_intervento && f.t >= 25.0) {
            u.pos = {100.0 * k, 40.0 * (k - 4)};
            u.rotta = 70.0;
        }
        f.unita.push_back(u);
        const auto evento = [&](double tempo, const char* tipo, int unita) {
            EventoTraccia e;
            e.id = id_evento++;
            e.tempo = tempo;
            e.tipo = tipo;
            e.unita = unita;
            f.eventi.push_back(e);
            f.evento = e.id;
        };
        if (k == 2) evento(9.0, "scoperta_ia", 2);
        if (k == 4 && con_intervento) evento(18.0, "intervento", 2);   // dello spettatore: il confronto lo salta
        if (k == 6 && con_intervento) evento(28.0, "lancio_ia", 2);
        if (k == 8) evento(39.0, "scoperta_ia", 3);
        if (k == 4 && con_intervento) {
            RecordIntervento x;
            x.id = 1;
            x.t = 18.0;
            x.fase = RecordIntervento::FASE_APPLICATO;
            x.tipo = "sensore";
            x.unita = 2;
            f.interventi.push_back(x);
        }
        aggiungi_fotogramma(t, f);
    }
    return t;
}

}  // namespace

TEST_CASE("confronto: il punto di divergenza di una coppia, nella verita' e nel registro") {
    const Traccia a = traccia(false), b = traccia(true);
    const Divergenza d = punto_divergenza(a, b);
    CHECK(d.divergono);
    CHECK(d.t_s == doctest::Approx(25.0));
    CHECK(d.unita == 2);
    CHECK(d.come == "posizione");
    CHECK(d.t_intervento_s == doctest::Approx(18.0));
    CHECK(d.dopo_intervento_s == doctest::Approx(7.0));
    REQUIRE(d.indice_evento == 1);   // lo stesso primo evento, poi il lancio che c'e' solo in B
    REQUIRE(d.evento_b);
    CHECK(d.evento_b->tipo == "lancio_ia");
    REQUIRE(d.evento_a);
    CHECK(d.evento_a->tipo == "scoperta_ia");
    // la stessa partita due volte: nessuna divergenza
    const Divergenza nessuna = punto_divergenza(a, traccia(false));
    CHECK_FALSE(nessuna.divergono);
    CHECK(nessuna.t_s < 0.0);
    CHECK(nessuna.indice_evento == -1);
}

namespace {

// un elicottero in virata a 5 gradi al secondo e 100 nodi, campionato agli istanti dati. B ha il fotogramma dell'intervento a 18 s, e da li' la
// cadenza si sposta (prossimo_ = f.t + PASSO_S): come nelle tracce vere, le due partite non hanno piu' istanti in comune.
// `scarto_da`: da quell'istante B sta `scarto_m` a nord, cioe' la divergenza vera
Traccia elicottero(const std::vector<double>& tempi, double scarto_da = 1e18, double scarto_m = 0.0) {
    Traccia t;
    for (const double tf : tempi) {
        Fotogramma f;
        f.t = tf;
        StatoUnita u;
        u.id = 4;
        u.classe = "sh60b";
        u.categoria = "aereo";
        u.flag = 11;
        const double a = radianti(5.0 * tf);
        u.pos = {589.0 * std::sin(a), 589.0 * std::cos(a) + (tf >= scarto_da ? scarto_m * (tf - scarto_da + 1.0) : 0.0)};
        u.rotta = normalizza_360(5.0 * tf + 90.0);
        u.velocita_kn = tf < 1.0 ? 0.0 : 100.0;
        u.quota = tf < 1.0 ? 0.0 : 30.0;
        f.unita.push_back(u);
        if (tf == 18.0) {
            RecordIntervento x;
            x.id = 1;
            x.t = 18.0;
            x.fase = RecordIntervento::FASE_APPLICATO;
            x.tipo = "sensore";
            x.unita = 4;
            f.interventi.push_back(x);
        }
        aggiungi_fotogramma(t, f);
    }
    return t;
}

}  // namespace

TEST_CASE("confronto: la stessa partita con un fotogramma in piu' non diverge, e la divergenza vera si trova lo stesso") {
    const std::vector<double> ta = {0.0, 5.0, 10.0, 15.0, 20.0, 25.0, 30.0, 35.0, 40.0, 45.0, 50.0, 55.0, 60.0};
    const std::vector<double> tb = {0.0, 0.222, 5.0, 10.0, 15.0, 18.0, 23.0, 28.0, 33.0, 38.0, 43.0, 48.0, 53.0, 58.0};
    const Divergenza uguali = punto_divergenza(elicottero(ta), elicottero(tb));
    CAPTURE(uguali.t_s);
    CAPTURE(uguali.come);
    CHECK_FALSE(uguali.divergono);
    // la divergenza vera dopo che la cadenza si e' spostata: con i soli istanti comuni non si vedrebbe mai
    const Divergenza vera = punto_divergenza(elicottero(ta), elicottero(tb, 30.0, 200.0));
    CHECK(vera.divergono);
    CHECK(vera.unita == 4);
    CHECK(vera.t_s >= 30.0);
    CHECK(vera.t_s <= 40.0);
    // l'unita' che esce dal gioco: nello stesso fatto a fotogrammi diversi non diverge, tolta solo in B si' e subito
    const auto fino_a = [](const std::vector<double>& t, double fine) {
        Traccia x;
        for (const double tf : t) {
            Fotogramma f;
            f.t = tf;
            StatoUnita ferma;
            ferma.id = 2;
            f.unita.push_back(ferma);
            if (tf < fine) {
                StatoUnita u;
                u.id = 4;
                f.unita.push_back(u);
            }
            aggiungi_fotogramma(x, f);
        }
        return x;
    };
    CHECK_FALSE(punto_divergenza(fino_a(ta, 22.0), fino_a(tb, 22.0)).divergono);
    const Divergenza uscita = punto_divergenza(fino_a(ta, 1e18), fino_a(ta, 20.0));
    CHECK(uscita.divergono);
    CHECK(uscita.t_s == doctest::Approx(20.0));
    CHECK(uscita.come == "manca");
    // a cadenza uguale resta esatta: un solo campione diverso
    const Divergenza esatta = punto_divergenza(elicottero(ta), elicottero(ta, 30.0, 2.0));
    CHECK(esatta.divergono);
    CHECK(esatta.t_s == doctest::Approx(30.0));
}

TEST_CASE("confronto: due serie di statistiche, esiti e numeri per parte") {
    std::vector<StatisticheSandbox> a(10), b(10);
    for (int i = 0; i < 10; ++i) {
        a[i].esito = i < 3 ? 101 : 102;
        b[i].esito = i < 8 ? 101 : 102;
        a[i].durata_s = 3600.0;
        b[i].durata_s = 3000.0;
        StatParte p;
        p.parte = 8;
        p.perse = 1;
        p.lanci = 2;
        a[i].parti[8] = p;
        p.perse = 0;
        p.lanci = 4;
        b[i].parti[8] = p;
    }
    const ConfrontoSerie c = confronta_serie(a, b);
    CHECK(c.coppie == 10);
    REQUIRE(c.esiti.count(101));
    CHECK(c.esiti.at(101).differenza == doctest::Approx(0.5));
    CHECK(c.esiti.at(102).differenza == doctest::Approx(-0.5));
    REQUIRE(c.parti.count(8));
    CHECK(c.parti.at(8).at("perse").differenza == doctest::Approx(-1.0));
    CHECK(c.parti.at(8).at("lanci").differenza == doctest::Approx(2.0));
    CHECK(c.generali.at("durata_s").differenza == doctest::Approx(-600.0));
}
