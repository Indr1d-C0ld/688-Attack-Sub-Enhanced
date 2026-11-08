#include <doctest.h>

#include <algorithm>
#include <cmath>
#include <cstdint>

#include "nucleo/tma.hpp"
#include "nucleo/tma_ia.hpp"

using namespace m688;

// TMA dai soli rilevamenti (fase C6c; dossier 02 §5.1)

namespace {
// Il battello fa due gambe di 5 minuti (rotta 0 poi 90, 5 m/s); il bersaglio a 9 km per 045 va a 270 a 4 m/s.
struct Scena {
    Tma tma;
    Vec2 bersaglio0{9000.0 * std::sin(radianti(45.0)), 9000.0 * std::cos(radianti(45.0))};
    Vec2 bersaglio(double t) const { return {bersaglio0.x - 4.0 * t, bersaglio0.y}; }
    Vec2 proprio(double t) const {
        return t <= 300.0 ? Vec2{0.0, 5.0 * t} : Vec2{5.0 * (t - 300.0), 1500.0};
    }
    Scena() {
        for (double t = 0; t <= 600.0; t += 5.0) tma.aggiungi({t, proprio(t), rilevamento(proprio(t), bersaglio(t))});
    }
};
}  // namespace

TEST_CASE("tma: la soluzione vera ha scarti nulli, una sbagliata no") {
    Scena s;
    const Rilevamento& u = s.tma.rilevamenti().back();
    const SoluzioneTma vera{distanza(u.proprio, s.bersaglio(600.0)), 270.0, 4.0};
    CHECK(s.tma.scarto_quadratico_medio(vera) < 1e-6);
    CHECK(distanza(*s.tma.posizione(vera, 0.0), s.bersaglio(0.0)) < 1e-6);
    SoluzioneTma sbagliata = vera;
    sbagliata.distanza_m *= 0.7;
    CHECK(s.tma.scarto_quadratico_medio(sbagliata) > 0.5);   // con due gambe la distanza si vede
}

TEST_CASE("tma: su una gamba sola la distanza non si vede (inosservabile)") {
    Tma tma;
    const Vec2 b0{6000.0, 6000.0};
    for (double t = 0; t <= 300.0; t += 5.0) {
        const Vec2 p{0.0, 5.0 * t}, b{b0.x - 4.0 * t, b0.y};
        tma.aggiungi({t, p, rilevamento(p, b)});
    }
    // bersaglio e moto relativo scalati del 50%: stessi rilevamenti finche' il battello non manovra
    const Vec2 p = tma.rilevamenti().back().proprio, b{b0.x - 4.0 * 300.0, b0.y};
    const Vec2 v_rel{-4.0, -5.0};   // velocita' del bersaglio meno quella propria
    const Vec2 v_scalata{0.5 * v_rel.x, 0.5 * v_rel.y + 5.0};   // velocita' vera della soluzione scalata
    const SoluzioneTma scalata{0.5 * distanza(p, b), normalizza_360(gradi(std::atan2(v_scalata.x, v_scalata.y))),
                               std::hypot(v_scalata.x, v_scalata.y)};
    CHECK(tma.scarto_quadratico_medio(scalata) < 1e-6);
}

TEST_CASE("tma: rateo di rilevamento, regola del 1934 e distanza di Ekelund") {
    Scena s;
    // ratei nei due minuti prima e dopo la manovra: Ekelund ignora l'accelerazione del rilevamento, quindi le gambe
    // vanno prese corte attorno alla virata (dossier 02 §5.1)
    const auto b1 = s.tma.rateo(180.0, 300.0), b2 = s.tma.rateo(300.0, 420.0);
    REQUIRE(b1);
    REQUIRE(b2);
    const double r0 = s.tma.rilevamenti()[60].rilevamento;   // alla manovra (t = 300 s)
    const double osa1 = Tma::attraverso({0.0, 5.0}, r0), osa2 = Tma::attraverso({5.0, 0.0}, r0);
    const auto r = Tma::distanza_ekelund(osa1, osa2, *b1, *b2);
    REQUIRE(r);
    const double vera = distanza(s.proprio(300.0), s.bersaglio(300.0));
    CHECK(*r == doctest::Approx(vera).epsilon(0.10));   // entro il 10%
    // 1934: a 1000 yd (914,4 m) un nodo attraverso la linea di vista da' 1,934 gradi/min
    CHECK(Tma::rateo_1934(1852.0 / 3600.0, 914.4) == doctest::Approx(1.934).epsilon(0.001));
    CHECK_FALSE(Tma::distanza_ekelund(1.0, 2.0, 0.5, 0.5));
}

TEST_CASE("tma dell'IA: con due gambe e rilevamenti rumorosi trova il bersaglio; su una gamba sola no") {
    // rilevamenti con l'errore di un sensore (1 grado, gaussiano deterministico)
    std::uint64_t z = 12345;
    const auto rumore = [&]() {
        z = z * 6364136223846793005ull + 1442695040888963407ull;
        const double u1 = std::max(static_cast<double>(z >> 11) / 9007199254740992.0, 1e-12);
        z = z * 6364136223846793005ull + 1442695040888963407ull;
        const double u2 = static_cast<double>(z >> 11) / 9007199254740992.0;
        return std::sqrt(-2.0 * std::log(u1)) * std::cos(2.0 * PI * u2);
    };
    const double sigma = 1.0;
    Scena vera;   // il bersaglio a 9 km, due gambe di 5 minuti
    Tma tma;
    for (const auto& r : vera.tma.rilevamenti()) tma.aggiungi({r.tempo, r.proprio, normalizza_360(r.rilevamento + sigma * rumore())});
    const auto s = risolvi_tma(tma, 600.0, sigma);
    REQUIRE(s);
    CHECK(s->osservabile);
    const double r_vera = distanza(tma.rilevamenti().back().proprio, vera.bersaglio(600.0));
    CHECK(std::fabs(s->soluzione.distanza_m - r_vera) < 0.2 * r_vera);
    CHECK(distanza(s->posizione, vera.bersaglio(600.0)) < 0.2 * r_vera);
    CHECK(s->scarto_deg < 2.0 * sigma);
    CHECK(s->sigma_distanza_m < 0.4 * r_vera);
    // la posizione a un tempo dopo l'ultimo rilevamento: col moto stimato
    CHECK(distanza(s->posizione, *tma.posizione(s->soluzione, 600.0)) < 1e-6);
    // una gamba sola: la distanza non e' osservabile
    Tma gamba;
    for (const auto& r : vera.tma.rilevamenti())
        if (r.tempo <= 300.0) gamba.aggiungi({r.tempo, r.proprio, normalizza_360(r.rilevamento + sigma * rumore())});
    const auto g = risolvi_tma(gamba, 300.0, sigma);
    REQUIRE(g);
    CHECK_FALSE(g->osservabile);
    // troppo pochi rilevamenti o troppo poco tempo: niente
    Tma corta;
    for (double t = 0; t <= 60.0; t += 10.0) corta.aggiungi({t, vera.proprio(t), rilevamento(vera.proprio(t), vera.bersaglio(t))});
    CHECK_FALSE(risolvi_tma(corta, 60.0, sigma));
}

TEST_CASE("tma dell'IA: su una gamba sola mai una distanza osservabile, con qualsiasi geometria e rumore") {
    // il caso del runtime: un Krivak a 10 km a 15 kn, il battello dritto a 6 kn, 3-8 minuti di rilevamenti. Il profilo
    // della distanza e' piatto (le soluzioni in scala danno gli stessi rilevamenti): un ottimizzatore che si ferma in un
    // minimo locale ci faceva un dente, e la distanza sembrava osservabile a 3-5 km
    int osservabili = 0, risolte = 0;
    for (std::uint64_t seme = 1; seme <= 30; ++seme) {
        std::uint64_t z = 1000 + seme;
        const auto rumore = [&]() {
            z = z * 6364136223846793005ull + 1442695040888963407ull;
            const double u1 = std::max(static_cast<double>(z >> 11) / 9007199254740992.0, 1e-12);
            z = z * 6364136223846793005ull + 1442695040888963407ull;
            const double u2 = static_cast<double>(z >> 11) / 9007199254740992.0;
            return std::sqrt(-2.0 * std::log(u1)) * std::cos(2.0 * PI * u2);
        };
        const double s = static_cast<double>(seme);
        const double brg = 20.0 + 11.0 * s, rot = std::fmod(37.0 * s, 360.0), sigma = 0.5 + 0.5 * static_cast<double>(seme % 3);
        const double durata = 180.0 + 60.0 * static_cast<double>(seme % 6);
        const Vec2 b0{10000.0 * std::sin(radianti(brg)), 10000.0 * std::cos(radianti(brg))};
        Tma tma;
        for (double t = 0.0; t <= durata; t += 10.0) {
            const Vec2 p{0.0, 3.0 * t}, b{b0.x + 7.7 * std::sin(radianti(rot)) * t, b0.y + 7.7 * std::cos(radianti(rot)) * t};
            tma.aggiungi({t, p, normalizza_360(rilevamento(p, b) + sigma * rumore())});
        }
        if (const auto x = risolvi_tma(tma, durata, sigma)) {
            ++risolte;
            osservabili += x->osservabile;
        }
    }
    CHECK(risolte == 30);
    CHECK(osservabili == 0);
}

TEST_CASE("tma: l'osservazione di rotta dal periscopio (libretto, docs/25 §3.4) fissa la rotta e su una gamba anche la distanza") {
    // il bersaglio della scena (rotta 270, 4 m/s), una gamba sola (fino a 300 s): coi soli rilevamenti non si osserva
    Scena vera;
    Tma gamba;
    std::uint64_t z = 777;
    const auto rumore = [&]() {
        z = z * 6364136223846793005ull + 1442695040888963407ull;
        const double u1 = std::max(static_cast<double>(z >> 11) / 9007199254740992.0, 1e-12);
        z = z * 6364136223846793005ull + 1442695040888963407ull;
        const double u2 = static_cast<double>(z >> 11) / 9007199254740992.0;
        return std::sqrt(-2.0 * std::log(u1)) * std::cos(2.0 * PI * u2);
    };
    for (const auto& r : vera.tma.rilevamenti())
        if (r.tempo <= 300.0) gamba.aggiungi({r.tempo, r.proprio, normalizza_360(r.rilevamento + 0.5 * rumore())});
    const auto senza = risolvi_tma(gamba, 300.0, 0.5);
    REQUIRE(senza);
    CHECK_FALSE(senza->osservabile);
    // l'AOB al periscopio: rotta 270 vista come 280 (+-15 gradi)
    Tma con = gamba;
    con.aggiungi_rotta({300.0, 280.0, 15.0});
    REQUIRE(con.rotte().size() == 1);
    const auto s = risolvi_tma(con, 300.0, 0.5);
    REQUIRE(s);
    MESSAGE("una gamba e una rotta: distanza " << s->soluzione.distanza_m << " (vera " << distanza(con.rilevamenti().back().proprio, vera.bersaglio(300.0))
            << "), rotta " << s->soluzione.rotta << ", velocita' " << s->soluzione.velocita_m_s << ", osservabile " << s->osservabile);
    CHECK(std::fabs(normalizza_180(s->soluzione.rotta - 270.0)) < 20.0);
    const double r_vera = distanza(con.rilevamenti().back().proprio, vera.bersaglio(300.0));
    // una rotta a +-15 gradi contro 61 rilevamenti a 0,5 lascia la distanza larga, e su una gamba il rumore sposta gia'
    // la rotta dei soli rilevamenti: l'intervallo compatibile (a 1 sigma) contiene la distanza vera solo in media. Su 20
    // rumori diversi: la rotta torna e la distanza vera sta entro due semi-intervalli quasi sempre
    int dentro = 0;
    double err_con = 0.0, err_senza = 0.0;
    for (std::uint64_t seme = 1; seme <= 20; ++seme) {
        z = seme * 7919;
        Tma x;
        for (const auto& r : vera.tma.rilevamenti())
            if (r.tempo <= 300.0) x.aggiungi({r.tempo, r.proprio, normalizza_360(r.rilevamento + 0.5 * rumore())});
        const auto y0 = risolvi_tma(x, 300.0, 0.5);
        x.aggiungi_rotta({300.0, 270.0 + 10.0 * rumore(), 15.0});
        const auto y = risolvi_tma(x, 300.0, 0.5);
        REQUIRE(y);
        REQUIRE(y0);
        dentro += std::fabs(y->soluzione.distanza_m - r_vera) <= 2.0 * y->sigma_distanza_m ? 1 : 0;
        err_con += std::fabs(normalizza_180(y->soluzione.rotta - 270.0)) / 20.0;
        err_senza += std::fabs(normalizza_180(y0->soluzione.rotta - 270.0)) / 20.0;
    }
    MESSAGE("20 rumori: distanza vera entro 2 semi-intervalli " << dentro << "; errore medio di rotta " << err_senza << " gradi coi soli "
            "rilevamenti, " << err_con << " con l'osservazione");
    CHECK(dentro >= 16);
    CHECK(err_con < 0.6 * err_senza);
    // senza rumore la rotta sceglie la soluzione della famiglia di una gamba (le scale della velocita' relativa): con 280
    // gradi la scala e' 0,876 della vera, 6214 m
    Tma pulita;
    for (const auto& r : vera.tma.rilevamenti())
        if (r.tempo <= 300.0) pulita.aggiungi(r);
    pulita.aggiungi_rotta({300.0, 280.0, 15.0});
    const auto p = risolvi_tma(pulita, 300.0, 0.5);
    REQUIRE(p);
    CHECK(p->soluzione.distanza_m == doctest::Approx(0.876 * r_vera).epsilon(0.03));
    CHECK(p->soluzione.rotta == doctest::Approx(280.0).epsilon(0.005));
    // gli scarti di rotta, e quelli dei rilevamenti che restano dei soli rilevamenti
    const SoluzioneTma giusta{r_vera, 270.0, 4.0};
    REQUIRE(con.scarti_rotta(giusta).size() == 1);
    CHECK(con.scarti_rotta(giusta)[0] == doctest::Approx(-10.0));
    CHECK(con.scarti(giusta).size() == gamba.scarti(giusta).size());
    // tenute al piu' 20, le piu' recenti; azzera le toglie
    for (int i = 0; i < 30; ++i) con.aggiungi_rotta({300.0 + i, 270.0, 20.0});
    CHECK(con.rotte().size() == Tma::MASSIMO_ROTTE);
    CHECK(con.rotte().front().tempo == doctest::Approx(310.0));
    con.azzera();
    CHECK(con.rotte().empty());
}
