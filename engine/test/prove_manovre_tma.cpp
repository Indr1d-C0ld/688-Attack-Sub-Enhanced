// Prove delle manovre di TMA dell'IA (nucleo/manovre_tma.hpp; fase 4 delle interazioni fra IA, dossier 02 §5.1)
#include <doctest.h>

#include <algorithm>
#include <cmath>
#include <optional>

#include "nucleo/casuale.hpp"
#include "nucleo/manovre_tma.hpp"

using namespace m688;

namespace {

constexpr double KN = 1852.0 / 3600.0;

const CatalogoTma& catalogo() {
    static const CatalogoTma c = carica_tma(M688_DATI "/interazioni.toml");
    return c;
}

double gauss(Casuale& c) {
    const double u = 1.0 - c.uniforme(), v = c.uniforme();
    return std::sqrt(-2.0 * std::log(u)) * std::cos(2.0 * PI * v);
}

// Un ascoltatore che parte a 5 kn per nord con un bersaglio a 15 km per est che va a 8 kn per 300. Un rilevamento ogni
// 20 s con 1 grado d'errore, una soluzione al minuto. Con `manovra` l'ascoltatore segue le gambe (virata a 3 gradi/s),
// senza resta sulla sua rotta. Ritorna l'ultima stima e la decisione
struct Esito {
    std::optional<StimaTma> stima;
    DecisioneTma decisione;
    double errore_relativo = 1.0, t_pronta = -1.0;
    int virate = 0;
    double virata_max = 0.0;
};
Esito insegui(bool manovra, double minuti, std::uint64_t seme, const DottrinaTma& d, double distanza_km = 15.0) {
    Casuale caso(seme);
    ManovreTma m(d, seme);
    Tma tma;
    Vec2 io{0.0, 0.0}, lui{distanza_km * 1000.0, 0.0};
    double rotta = 0.0, velocita = 5.0, rotta_voluta = 0.0;
    const Vec2 v_lui{8.0 * KN * std::sin(radianti(300.0)), 8.0 * KN * std::cos(radianti(300.0))};
    Esito e;
    for (int s = 0; s <= static_cast<int>(minuti * 60.0); ++s) {
        const double t = s;
        if (s % 20 == 0) tma.aggiungi({t, io, normalizza_360(rilevamento(io, lui) + gauss(caso))});
        if (s % 60 == 0 && s > 0) {
            SituazioneTma sit;
            sit.t = t, sit.pos = io, sit.rotta = rotta, sit.velocita_kn = velocita;
            sit.rilevamento = tma.rilevamenti().back().rilevamento;
            sit.stima = risolvi_tma(tma, t, 1.0);
            sit.schiera = true;
            e.decisione = m.passo(sit);
            e.stima = sit.stima;
            if (e.decisione.nuova_gamba && manovra) {
                const double salto = std::fabs(normalizza_180(e.decisione.rotta - rotta_voluta));
                // il limite dei 90 gradi vale da lontano; sotto vicino_m la gamba apre e puo' virare di piu'
                if (e.virate > 0 && distanza(io, lui) > 1.5 * d.vicino_m) e.virata_max = std::max(e.virata_max, salto);
                ++e.virate;
                rotta_voluta = e.decisione.rotta;
                velocita = e.decisione.velocita_kn;
            }
            if (e.decisione.soluzione_pronta && e.t_pronta < 0.0) e.t_pronta = t;
        }
        if (manovra) {   // virata a 3 gradi al secondo verso la rotta voluta
            const double d_rotta = normalizza_180(rotta_voluta - rotta);
            rotta = normalizza_360(rotta + std::clamp(d_rotta, -3.0, 3.0));
        }
        io.x += velocita * KN * std::sin(radianti(rotta));
        io.y += velocita * KN * std::cos(radianti(rotta));
        lui.x += v_lui.x;
        lui.y += v_lui.y;
    }
    if (e.stima) e.errore_relativo = std::fabs(distanza(io, e.stima->posizione) - distanza(io, lui)) / distanza(io, lui);
    return e;
}

}  // namespace

TEST_CASE("manovre tma: dottrine per marina e per classe") {
    const auto& c = catalogo();
    const DottrinaTma nato = dottrina_tma(c, "usa");
    CHECK(nato.marina == "nato");
    CHECK(nato.gamba_min_s == doctest::Approx(180.0));
    CHECK(nato.gamba_max_s == doctest::Approx(360.0));
    CHECK(nato.mascone_deg == doctest::Approx(45.0));
    CHECK(dottrina_tma(c, "uk").marina == "nato");
    const DottrinaTma vmf = dottrina_tma(c, "urss");
    CHECK(vmf.marina == "vmf");
    CHECK(vmf.gamba_max_s > nato.gamba_max_s);
    CHECK(vmf.sigma_relativa > nato.sigma_relativa);
    CHECK(dottrina_tma(c, "sconosciuta").marina == "nato");
    // la classe cambia solo i suoi campi, e la fonte lo dice
    const DottrinaTma alfa = dottrina_tma(c, "urss", "alfa");
    CHECK(alfa.velocita_kn == doctest::Approx(12.0));
    CHECK(alfa.gamba_max_s == doctest::Approx(vmf.gamba_max_s));
    CHECK(alfa.fonte.find("alfa") != std::string::npos);
    CHECK_FALSE(nato.fonte.empty());
}

TEST_CASE("manovre tma: le gambe alternano i masconi, durano 3-6 minuti, e da vicino si apre") {
    const DottrinaTma d = dottrina_tma(catalogo(), "usa");
    ManovreTma m(d, 1);
    SituazioneTma s;
    s.rilevamento = 90.0, s.rotta = 0.0, s.velocita_kn = 5.0;
    DecisioneTma x = m.passo(s);
    REQUIRE(x.nuova_gamba);
    CHECK(x.rotta == doctest::Approx(45.0));        // il bersaglio sul mascone di dritta: virata piu' corta da nord
    CHECK(x.fase == DecisioneTma::Fase::virata);
    CHECK(x.velocita_kn == doctest::Approx(6.0));
    // virata finita: con la schiera si aspetta il raddrizzamento, poi conta la gamba
    s.t = 30.0, s.rotta = 45.0, s.schiera = true;
    x = m.passo(s);
    CHECK(x.fase == DecisioneTma::Fase::raddrizzamento);
    CHECK(x.prossima_s == doctest::Approx(150.0));
    s.t = 150.0;
    x = m.passo(s);
    CHECK(x.fase == DecisioneTma::Fase::gamba);
    // 10 km senza stima: a meta' fra 3 e 6 minuti, piu' o meno la variazione del seme
    const double durata = x.prossima_s - 150.0;
    CHECK(durata >= 270.0 - 0.15 * 180.0 - 1e-9);
    CHECK(durata <= 270.0 + 0.15 * 180.0 + 1e-9);
    s.t = 180.0;
    CHECK_FALSE(m.passo(s).nuova_gamba);
    s.t = 150.0 + durata;
    x = m.passo(s);
    REQUIRE(x.nuova_gamba);
    CHECK(x.rotta == doctest::Approx(135.0));       // l'altro mascone: 90 gradi di virata
    CHECK(x.gambe == 1);
    // vicino al bersaglio la gamba nuova lo mette oltre il traverso
    ManovreTma v(d, 2);
    SituazioneTma w;
    w.rilevamento = 90.0;
    w.stima = StimaTma{};
    w.stima->osservabile = true;   // la distanza vale, ma non basta per la soluzione
    w.stima->soluzione.distanza_m = 3000.0;
    w.stima->sigma_distanza_m = 3000.0;
    const DecisioneTma y = v.passo(w);
    CHECK(std::fabs(normalizza_180(y.rotta - w.rilevamento)) == doctest::Approx(100.0));
}

TEST_CASE("manovre tma: con le gambe la distanza diventa osservabile, senza no") {
    const DottrinaTma d = dottrina_tma(catalogo(), "usa");
    int pronte = 0, ferme = 0;
    double errore = 0.0, t_medio = 0.0;
    const int n = 10;
    for (int k = 0; k < n; ++k) {
        const Esito con = insegui(true, 30.0, 100u + static_cast<std::uint64_t>(k), d);
        const Esito senza = insegui(false, 30.0, 100u + static_cast<std::uint64_t>(k), d);
        if (con.t_pronta >= 0.0) {
            ++pronte;
            errore += con.errore_relativo;
            t_medio += con.t_pronta;
            CHECK(con.virata_max <= 90.0 + 1e-6);   // fra due gambe lontane al piu' 90 gradi (dossier: 30-90)
        }
        if (senza.stima && senza.stima->osservabile) ++ferme;
    }
    MESSAGE("con le gambe pronta in " << pronte << " su " << n << ", a " << (pronte ? t_medio / pronte / 60.0 : 0.0)
                                      << " min, errore " << (pronte ? errore / pronte * 100.0 : 0.0) << "%; senza "
                                      << ferme << " osservabili");
    CHECK(pronte >= 8);
    CHECK(errore / std::max(pronte, 1) < 0.2);
    CHECK(t_medio / std::max(pronte, 1) <= 30.0 * 60.0);
    CHECK(ferme <= 2);
}

TEST_CASE("manovre tma: soluzione pronta, poi se si guasta le gambe riprendono") {
    const DottrinaTma d = dottrina_tma(catalogo(), "usa");
    ManovreTma m(d, 3);
    SituazioneTma s;
    s.rilevamento = 90.0;
    m.passo(s);
    s.stima = StimaTma{};
    s.stima->osservabile = true;
    s.stima->soluzione.distanza_m = 10000.0;
    s.stima->sigma_distanza_m = 1000.0;   // 10%: dentro il 15%
    s.t = 400.0;
    DecisioneTma x = m.passo(s);
    CHECK(x.soluzione_pronta);
    CHECK(x.fase == DecisioneTma::Fase::soluzione);
    s.stima->sigma_distanza_m = 3000.0;   // 30%: non basta piu'
    s.t = 460.0;
    x = m.passo(s);
    CHECK_FALSE(x.soluzione_pronta);
    CHECK(x.fase == DecisioneTma::Fase::gamba);
    CHECK(std::string(nome_fase_tma(x.fase)) == "gamba");
}

TEST_CASE("manovre tma: il fondale scarta un lato, la rinuncia, e lo stesso seme da' le stesse gambe") {
    const DottrinaTma d = dottrina_tma(catalogo(), "usa");
    // a nord dell'unita' il fondo e' a 60 m: con l'unita' a 100 m il mascone di dritta (rotta 45, verso nord-est) si
    // scarta e la gamba va sull'altro (rotta 135, verso sud-est)
    ManovreTma m(d, 4);
    SituazioneTma s;
    s.rilevamento = 90.0, s.rotta = 90.0, s.quota_m = 100.0;
    s.fondale = [](Vec2 p) { return p.y > 0.0 ? 60.0 : 1000.0; };
    DecisioneTma x = m.passo(s);
    CHECK(x.fondo_rifiutato);
    CHECK(x.rotta == doctest::Approx(135.0));
    // tutti e due i lati sul basso: niente gamba nuova, si tiene la rotta
    ManovreTma basso(d, 4);
    s.fondale = [](Vec2) { return 80.0; };
    x = basso.passo(s);
    CHECK_FALSE(x.nuova_gamba);
    CHECK(x.rotta == doctest::Approx(90.0));
    // senza la carta vale il fondale di adesso
    ManovreTma senza(d, 4);
    s.fondale = nullptr;
    s.fondale_m = 120.0;
    CHECK_FALSE(senza.passo(s).nuova_gamba);
    // rinuncia: niente rilevamenti da oltre la tenuta
    ManovreTma r(d, 5);
    SituazioneTma q;
    q.rilevamento = 90.0;
    r.passo(q);
    q.t = 1000.0;
    q.t_ultimo_rilevamento = 300.0;
    CHECK(r.passo(q).rinuncia == DecisioneTma::Rinuncia::contatto_perso);
    // rinuncia: lo specchio irrisolto dopo gambe_max_specchio gambe, e troppe gambe senza soluzione
    for (const bool specchio : {true, false}) {
        ManovreTma g(d, 6);
        SituazioneTma z;
        z.rilevamento = 90.0;
        z.specchio_irrisolto = specchio;
        DecisioneTma y = g.passo(z);
        for (int k = 0; k < 400 && y.rinuncia == DecisioneTma::Rinuncia::no; ++k) {
            z.t += 60.0;
            z.rotta = y.rotta;   // l'unita' segue subito la gamba
            y = g.passo(z);
        }
        CHECK(y.rinuncia == (specchio ? DecisioneTma::Rinuncia::specchio : DecisioneTma::Rinuncia::gambe));
        CHECK(y.gambe == (specchio ? d.gambe_max_specchio : d.gambe_max));
    }
    // lo stesso seme: le stesse gambe; un altro seme: durate diverse
    const auto gambe = [&](std::uint64_t seme) {
        ManovreTma g(d, seme);
        SituazioneTma z;
        z.rilevamento = 90.0;
        std::vector<double> tempi;
        DecisioneTma y = g.passo(z);
        for (int k = 0; k < 120; ++k) {
            z.t += 15.0;
            z.rotta = y.rotta;
            y = g.passo(z);
            if (y.nuova_gamba) tempi.push_back(z.t);
        }
        return tempi;
    };
    CHECK(gambe(7) == gambe(7));
    CHECK(gambe(7) != gambe(8));
}
