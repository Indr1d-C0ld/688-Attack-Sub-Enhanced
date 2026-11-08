#include <doctest.h>

#include <cmath>
#include <sstream>
#include <stdexcept>

#include "nucleo/volta_ghiaccio.hpp"

using namespace m688;

// La volta del ghiaccio: il sondatore verso l'alto (il liscio scambiato per acqua) e il quadro della volta

TEST_CASE("volta del ghiaccio: il sondatore misura il pescaggio, scambia a volte il liscio per acqua, vede le polynye") {
    const CampoGhiaccio g(5);
    const ParametriSondatoreAlto p;
    Casuale caso(9);
    QuadroVolta q;
    int misure = 0, sul_ghiaccio = 0, buone = 0, libere_vere = 0, viste_libere = 0;
    for (int k = 0; k < 4000; ++k) {
        const Vec2 pos{k * 50.0, 1000.0};   // 200 km a 100 m di quota
        const SondaggioAlto s = sonda_alto(g, pos, pos, 100.0, 0.0, p, caso);
        REQUIRE(s.eco);
        ++misure;
        const PuntoGhiaccio x = g.in(pos, 0.0);
        if (!s.libera) {
            ++sul_ghiaccio;
            buone += std::fabs(s.pescaggio_m - s.pescaggio_vero_m) < 5.0 * s.sigma_m + 1e-6;
        }
        if (x.aperta) {
            ++libere_vere;
            viste_libere += s.libera;
        }
        CHECK(s.pescaggio_vero_m >= x.pescaggio_m - 1e-9);   // il cono vede almeno il punto sotto
        q.aggiungi(s);
    }
    CHECK(misure == 4000);
    REQUIRE(sul_ghiaccio > 0);
    CHECK(static_cast<double>(buone) / sul_ghiaccio > 0.99);
    REQUIRE(libere_vere > 50);
    // l'acqua libera sotto la verticale si vede libera, salvo una chiglia vicina nel cono
    CHECK(static_cast<double>(viste_libere) / libere_vere > 0.7);
    // il quadro: le polynye candidate vicine, la piu' recente prima; il pescaggio massimo lungo il tratto
    const auto c = q.polynye({100000.0, 1000.0}, 100000.0, 10.0, 3600.0);
    CHECK_FALSE(c.empty());
    CHECK(q.pescaggio_max({0.0, 1000.0}, {200000.0, 1000.0}) > 5.0);
    // fuori portata o in superficie niente
    CHECK_FALSE(sonda_alto(g, {0.0, 0.0}, {0.0, 0.0}, 1500.0, 0.0, p, caso).eco);
    CHECK_FALSE(sonda_alto(g, {0.0, 0.0}, {0.0, 0.0}, 0.0, 0.0, p, caso).eco);
    // il ghiaccio liscio sottile: con p_scambio 1 e' sempre acqua; con 0 mai
    ParametriGhiaccio sottile;
    sottile.spessore_livellato_m = 0.4;
    sottile.creste_per_km = 0.0;
    sottile.frazione_aperta = 0.0;
    const CampoGhiaccio liscio(5, sottile);
    ParametriSondatoreAlto sempre = p, mai = p;
    sempre.p_scambio = 1.0;
    mai.p_scambio = 0.0;
    const SondaggioAlto a = sonda_alto(liscio, {500.0, 500.0}, {500.0, 500.0}, 80.0, 0.0, sempre, caso);
    CHECK(a.libera);
    CHECK(a.scambio);
    const SondaggioAlto b = sonda_alto(liscio, {500.0, 500.0}, {500.0, 500.0}, 80.0, 0.0, mai, caso);
    CHECK_FALSE(b.libera);
    CHECK(b.pescaggio_m == doctest::Approx(0.36).epsilon(5.0));
}

TEST_CASE("volta del ghiaccio: il BQS-15 vede le chiglie a prua, ostacoli solo quelle che arrivano alla quota") {
    const CampoGhiaccio g(5);
    const ParametriBqs15Fondo p;
    Casuale caso(4);
    QuadroVolta q;
    // a 15 m di quota, sotto un pack con chiglie fino a 20 m: echi entro la portata, qualche ostacolo (da 5 m in giu')
    int echi = 0, ostacoli = 0;
    for (int k = 0; k < 50; ++k) {
        const Vec2 pos{k * 2000.0, 0.0};
        for (const auto& r : spazza_ghiaccio(g, pos, pos, 15.0, 0.0, 5.0, 0.0, p, caso)) {
            ++echi;
            ostacoli += r.ostacolo;
            CHECK(r.distanza_m <= 2500.0 + 30.0);
            CHECK(r.pescaggio_m <= 15.0 + 10.0);
            q.aggiungi(r);
        }
    }
    MESSAGE("a 15 m: ", echi, " echi, ", ostacoli, " ostacoli");
    CHECK(echi > 100);
    CHECK(ostacoli > 0);
    CHECK(ostacoli < echi);
    CHECK(q.celle() > 0);
    // a 150 m le chiglie restano sopra: echi ma nessun ostacolo; piu' veloci, portata minore
    int ostacoli_fondi = 0;
    double piu_lontano_piano = 0.0, piu_lontano_veloce = 0.0;
    for (int k = 0; k < 20; ++k) {
        const Vec2 pos{k * 3000.0, 5000.0};
        for (const auto& r : spazza_ghiaccio(g, pos, pos, 150.0, 0.0, 5.0, 0.0, p, caso)) {
            ostacoli_fondi += r.ostacolo;
            piu_lontano_piano = std::max(piu_lontano_piano, r.distanza_m);
        }
        for (const auto& r : spazza_ghiaccio(g, pos, pos, 150.0, 0.0, 22.0, 0.0, p, caso)) piu_lontano_veloce = std::max(piu_lontano_veloce, r.distanza_m);
    }
    CHECK(ostacoli_fondi == 0);
    CHECK(piu_lontano_veloce < piu_lontano_piano);
}

TEST_CASE("volta del ghiaccio: la portata sulle chiglie, gli avvisi dalle misure, il quadro salvato e ricaricato") {
    // la portata: 2,5 km il BQS-15, +20% sul 688I, il fattore dei sonar sovietici
    ParametriBqs15Fondo bqs, mg70;
    mg70.fattore_portata = 0.7;
    CHECK(portata_chiglia_m(bqs) == doctest::Approx(2500.0));
    CHECK(portata_chiglia_m(bqs, true) == doctest::Approx(3000.0));
    CHECK(portata_chiglia_m(mg70) == doctest::Approx(1750.0));
    // il sondatore: la volta a 40 m, poi a 20 m (una chiglia di 30 m sopra il battello a 50 m): bassa e in discesa
    QuadroVolta q;
    SondaggioAlto a;
    a.eco = true;
    a.quota_m = 50.0;
    a.distanza_m = 40.0;
    a.pescaggio_m = 10.0;
    SondaggioAlto b = a;
    b.t = 30.0;
    b.distanza_m = 20.0;
    b.pescaggio_m = 30.0;
    q.aggiungi(a);
    q.aggiungi(b);
    const auto tipo = [](const std::vector<AvvisoVolta>& v, AvvisoVolta::Tipo t) {
        for (const auto& x : v)
            if (x.tipo == t) return x.valore;
        return -1.0;
    };
    auto v = q.avvisi({0.0, 0.0}, 50.0, 0.0, 5.0, 30.0);
    CHECK(tipo(v, AvvisoVolta::Tipo::volta_bassa) == doctest::Approx(20.0));
    CHECK(tipo(v, AvvisoVolta::Tipo::volta_in_discesa) == doctest::Approx(20.0));
    // sceso a 80 m la volta non e' piu' bassa (la distanza alla quota di adesso)
    CHECK(tipo(q.avvisi({0.0, 0.0}, 80.0, 0.0, 5.0, 30.0), AvvisoVolta::Tipo::volta_bassa) < 0.0);
    // l'acqua libera sopra non e' una volta bassa
    SondaggioAlto libera = b;
    libera.t = 31.0;
    libera.libera = true;
    libera.pescaggio_m = 0.0;
    libera.distanza_m = 50.0;
    q.aggiungi(libera);
    CHECK(tipo(q.avvisi({0.0, 0.0}, 50.0, 0.0, 5.0, 31.0), AvvisoVolta::Tipo::volta_bassa) < 0.0);
    // il BQS-15: una chiglia-ostacolo a 800 m di prua
    RitornoChiglia r;
    r.t = 31.0;
    r.rilevamento = 0.0;
    r.distanza_m = 800.0;
    r.punto = {0.0, 800.0};
    r.pescaggio_m = 45.0;
    r.ostacolo = true;
    q.aggiungi_spazzata(31.0, 1500.0, {r});
    v = q.avvisi({0.0, 0.0}, 50.0, 0.0, 10.0, 32.0);   // 10 nodi x 180 s = 926 m: dentro
    CHECK(tipo(v, AvvisoVolta::Tipo::chiglia_a_prua) == doctest::Approx(800.0));
    CHECK(tipo(v, AvvisoVolta::Tipo::velocita_eccessiva) == doctest::Approx(800.0 / 180.0 / NODO));
    CHECK(tipo(q.avvisi({0.0, 0.0}, 50.0, 0.0, 2.0, 32.0), AvvisoVolta::Tipo::chiglia_a_prua) < 0.0);    // lontana per 2 nodi
    CHECK(tipo(q.avvisi({0.0, 0.0}, 50.0, 0.0, 5.0, 32.0), AvvisoVolta::Tipo::velocita_eccessiva) < 0.0);
    CHECK(tipo(q.avvisi({0.0, 0.0}, 50.0, 90.0, 10.0, 32.0), AvvisoVolta::Tipo::chiglia_a_prua) < 0.0);  // non a prua
    CHECK(tipo(q.avvisi({0.0, 0.0}, 50.0, 0.0, 10.0, 70.0), AvvisoVolta::Tipo::chiglia_a_prua) < 0.0);   // vecchia
    // salvato e ricaricato: le stesse celle e gli stessi avvisi
    std::stringstream ss;
    q.salva(ss);
    const QuadroVolta c = QuadroVolta::carica(ss);
    CHECK(c.celle() == q.celle());
    CHECK(c.cella_m() == q.cella_m());
    const auto v1 = q.avvisi({0.0, 0.0}, 50.0, 0.0, 10.0, 32.0), v2 = c.avvisi({0.0, 0.0}, 50.0, 0.0, 10.0, 32.0);
    REQUIRE(v1.size() == v2.size());
    for (std::size_t i = 0; i < v1.size(); ++i) {
        CHECK(v1[i].tipo == v2[i].tipo);
        CHECK(v1[i].valore == v2[i].valore);
    }
    CHECK(c.pescaggio_max({0.0, 0.0}, {0.0, 1000.0}) == q.pescaggio_max({0.0, 0.0}, {0.0, 1000.0}));
    std::stringstream rotto("non e' un quadro");
    CHECK_THROWS_AS(QuadroVolta::carica(rotto), std::runtime_error);
}

TEST_CASE("volta del ghiaccio: in transito sotto il pack a 15 m il BQS-15 avvisa delle chiglie prima di arrivarci") {
    const CampoGhiaccio g(5);
    const ParametriBqs15Fondo p;
    Casuale caso(11);
    QuadroVolta q;
    const double portata = portata_chiglia_m(p);
    int avvisati = 0, urti_senza_avviso = 0, urti = 0;
    bool avviso_attivo = false;
    for (int k = 0; k < 2000; ++k) {   // 100 km verso nord a 10 nodi, una spazzata ogni 5 s
        const double t = k * 5.0;
        const Vec2 pos{3000.0, t * 10.0 * NODO};
        q.aggiungi_spazzata(t, portata_effettiva(portata, 10.0, p), spazza_ghiaccio(g, pos, pos, 15.0, 0.0, 10.0, t, p, caso, portata));
        bool chiglia = false;
        for (const auto& a : q.avvisi(pos, 15.0, 0.0, 10.0, t)) chiglia = chiglia || a.tipo == AvvisoVolta::Tipo::chiglia_a_prua;
        avvisati += chiglia;
        // la verita': una chiglia alla quota del battello (meno 2 m) nei prossimi 5 s di strada
        const bool urto = g.pescaggio_max(pos, {pos.x, pos.y + 5.0 * 10.0 * NODO}, t) >= 13.0;
        urti += urto;
        urti_senza_avviso += urto && !avviso_attivo;
        avviso_attivo = chiglia;
    }
    MESSAGE(avvisati, " passi con la chiglia a prua, ", urti, " passi con una chiglia sulla rotta, ", urti_senza_avviso, " senza avviso");
    CHECK(avvisati > 0);
    REQUIRE(urti > 0);
    CHECK(static_cast<double>(urti_senza_avviso) / urti < 0.2);
}

TEST_CASE("volta del ghiaccio: la storia del sondatore (30 minuti) e i ritorni recenti delle chiglie, per la vista") {
    QuadroVolta q;
    SondaggioAlto s;
    s.eco = true;
    s.quota_m = 60.0;
    s.distanza_m = 50.0;
    s.pescaggio_m = 10.0;
    RitornoChiglia r;
    r.punto = {0.0, 500.0};
    r.distanza_m = 500.0;
    r.pescaggio_m = 8.0;
    for (int k = 0; k <= 2400; k += 2) {   // 40 minuti, un sondaggio ogni 2 s e una spazzata ogni 10 s
        s.t = k;
        q.aggiungi(s);
        if (k % 10 == 0) {
            r.t = k;
            q.aggiungi_spazzata(k, 2000.0, {r});
        }
    }
    REQUIRE_FALSE(q.storia().empty());
    CHECK(q.storia().front().t >= 2400.0 - QuadroVolta::STORIA_S);
    CHECK(q.storia().back().t == doctest::Approx(2400.0));
    CHECK(q.storia().size() == 901u);   // 30 minuti a passi di 2 s, estremi compresi
    const auto cinque = q.ritorni_recenti(2400.0, 300.0);
    CHECK(cinque.size() == 31u);   // 5 minuti a passi di 10 s
    CHECK(q.ritorni_recenti(2400.0, 1200.0).size() == 61u);   // l'archivio tiene 10 minuti
    // gli avvisi guardano ancora solo l'ultimo minuto: la volta a 50 m, niente di basso
    CHECK(q.avvisi({0.0, 0.0}, 60.0, 0.0, 5.0, 2400.0, 180.0, 30.0).empty());
    // la storia torna col quadro salvato
    std::stringstream ss;
    q.salva(ss);
    const QuadroVolta c = QuadroVolta::carica(ss);
    CHECK(c.storia().size() == q.storia().size());
    CHECK(c.ritorni_recenti(2400.0, 300.0).size() == cinque.size());
}

TEST_CASE("volta del ghiaccio: i falsi echi delle chiglie per la testa d'un siluro vicino alla volta, e non in profondita'") {
    CHECK(forza_bersaglio_chiglia_db(10.0) == doctest::Approx(0.0));
    CHECK(forza_bersaglio_chiglia_db(20.0) == doctest::Approx(6.0));
    CHECK(forza_bersaglio_chiglia_db(1.0) == doctest::Approx(-15.0));
    CHECK(forza_bersaglio_chiglia_db(200.0) == doctest::Approx(10.0));
    const CampoGhiaccio g(5);
    int vicino = 0, fondo = 0;
    double piu_vicina = 1e9;
    for (int k = 0; k < 40; ++k) {   // una testa con un cono di 45 gradi, portata 1,5 km, elevazione ±15 gradi
        const Vec2 pos{k * 3000.0, 0.0};
        const auto e = echi_chiglie(g, pos, 20.0, 0.0, 45.0, 1500.0, 15.0, 0.0);
        vicino += static_cast<int>(e.size());
        for (const auto& x : e) {
            piu_vicina = std::min(piu_vicina, x.distanza_m);
            CHECK(x.ts_db >= -15.0);
            CHECK(x.ts_db <= 10.0);
            CHECK(x.distanza_m <= 1500.0 + 1e-6);
        }
        fondo += static_cast<int>(echi_chiglie(g, pos, 500.0, 0.0, 45.0, 1500.0, 15.0, 0.0).size());
    }
    MESSAGE("a 20 m: ", vicino, " falsi echi (il piu' vicino a ", piu_vicina, " m); a 500 m: ", fondo);
    CHECK(vicino > 20);
    CHECK(fondo == 0);   // a 500 m il ventaglio di 15 gradi tocca la volta solo oltre 1,8 km, fuori portata
    // in acqua libera niente
    ParametriGhiaccio libera;
    libera.frazione_aperta = 1.0;
    libera.frazione_nilas = 0.0;
    libera.creste_per_km = 0.0;
    CHECK(echi_chiglie(CampoGhiaccio(5, libera), {0.0, 0.0}, 20.0, 0.0, 45.0, 1500.0, 15.0, 0.0).empty());
}
