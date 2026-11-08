// Misure del sonarista (S5d): verniero, Doppler e CPA dalla curva a S, consegna alla TMA, FRAZ.
#include <doctest.h>

#include <algorithm>
#include <cmath>

#include "nucleo/casuale.hpp"
#include "nucleo/misure_sonar.hpp"

using namespace m688;

namespace {

const CatalogoAcustico& catalogo() {
    static const CatalogoAcustico c = carica_acustica(M688_DATI "/acustica.toml");
    return c;
}

double gauss(Casuale& c) {
    const double u1 = std::max(c.uniforme(), 1e-12), u2 = c.uniforme();
    return std::sqrt(-2.0 * std::log(u1)) * std::cos(2.0 * PI * u2);
}

// passaggio rettilineo: il proprio battello fermo nell'origine (o in moto), il bersaglio da (x0, y0) a velocita' v
struct Passaggio {
    Vec2 proprio0{0, 0}, v_proprio{0, 0};
    Vec2 bersaglio0{-3000.0, 1500.0}, v_bersaglio{5.0, 0.0};
    double f0 = 100.0;
    Vec2 proprio(double t) const { return {proprio0.x + v_proprio.x * t, proprio0.y + v_proprio.y * t}; }
    Vec2 bersaglio(double t) const { return {bersaglio0.x + v_bersaglio.x * t, bersaglio0.y + v_bersaglio.y * t}; }
    double frequenza(double t) const {   // f0 (1 + avvicinamento / c)
        const Vec2 d{bersaglio(t).x - proprio(t).x, bersaglio(t).y - proprio(t).y};
        const Vec2 u{v_bersaglio.x - v_proprio.x, v_bersaglio.y - v_proprio.y};
        const double r = std::hypot(d.x, d.y);
        return f0 * (1.0 - (d.x * u.x + d.y * u.y) / r / C_SUONO_MISURE);
    }
};

}  // namespace

TEST_CASE("misure sonar: verniero fra i bin, incertezza dal tempo e dal S/N") {
    const auto& c = catalogo();
    static const ClimaTeatro faroe = carica_clima(M688_DATI "/teatri/faroe.suono.toml");
    Ambiente amb;
    amb.profilo = &faroe.profilo(2);
    amb.fondale_m = 3000.0;
    amb.vento_m_s = 7.0;
    amb.traffico = faroe.traffico;
    Ricevitore r;
    r.quota_m = 150.0;
    r.velocita_kn = 5.0;
    r.sensore = c.sensore("tb16");
    UnitaAcustica u;
    u.id = 4;
    u.classe = "charlie2";
    u.categoria = "sottomarino";
    u.quota_m = 150.0;
    u.velocita_kn = 12.0;
    u.rotta = 270.0;   // verso il ricevitore: Doppler positivo
    u.pos = {4000.0, 0.0};
    const std::vector<Sorgente> s = {sorgente_acustica(c, u)};
    // la riga dei 250 Hz con il suo Doppler, vera
    double vera = 0.0;
    for (const auto& x : righe_ricevute(s, r, amb, 90.0, 240.0, 260.0, 0.1))
        if (std::fabs(x.hz_emessa - 250.0) < 1e-6) vera = x.hz;
    REQUIRE(vera > 250.5);
    const Spettro z = spettro_verniero(s, r, amb, 90.0, vera, 4.0, 0.02);
    CHECK(z.snr_db.size() == 200);
    const MisuraRiga m = misura_riga(z, vera, 0.5);
    REQUIRE(m.valida);
    CHECK(std::fabs(m.hz - vera) < 0.02);   // meglio del bin
    CHECK(std::fabs(m.hz - vera) < 4.0 * m.sigma_hz + 0.005);
    // integrare piu' a lungo restringe l'incertezza
    CHECK(misura_riga(z, vera, 0.5, 600.0).sigma_hz < m.sigma_hz);
    // lo spettro normale ha la riga in un bin solo; il verniero la sparge (Hann)
    const Spettro p = spettro_fascio(s, r, amb, 90.0, vera - 2.0, vera + 2.0, 0.02);
    const auto accesi = [](const Spettro& x) {   // bin sopra il fondo (il minimo dello spettro) di piu' di 3 dB
        const float fondo = *std::min_element(x.snr_db.begin(), x.snr_db.end());
        int k = 0;
        for (float v : x.snr_db) k += v > fondo + 3.0f;
        return k;
    };
    const int accesi_p = accesi(p), accesi_z = accesi(z);
    CHECK(accesi_z > accesi_p);
    // fuori dalla riga: niente misura valida
    CHECK_FALSE(misura_riga(z, vera + 1.5, 0.2).valida);
}

TEST_CASE("misure sonar: curva a S, velocita' relativa, istante e distanza del CPA") {
    Passaggio g;   // CPA a 1500 m dopo 600 s, 5 m/s
    Casuale caso(11);
    std::vector<PuntoFrequenza> punti;
    for (double t = 0.0; t <= 1200.0; t += 10.0) punti.push_back({t, g.frequenza(t) + 0.003 * gauss(caso), 0.003});
    const MisuraCpa m = misura_cpa(punti);
    REQUIRE(m.valida);
    CHECK(m.cpa_osservato);
    CHECK(m.f0_hz == doctest::Approx(100.0).epsilon(0.001));
    CHECK(m.velocita_relativa_m_s == doctest::Approx(5.0).epsilon(0.1));
    CHECK(m.t_cpa == doctest::Approx(600.0).epsilon(0.05));
    CHECK(m.distanza_cpa_m == doctest::Approx(1500.0).epsilon(0.15));
    CHECK(m.residuo_hz < 0.01);
    CHECK(m.sigma_distanza_m > 0.0);
    CHECK(frequenza_cpa(m, 600.0) == doctest::Approx(m.f0_hz));
    // solo prima del CPA: distanza e velocita' non si separano
    std::vector<PuntoFrequenza> prima(punti.begin(), punti.begin() + 40);
    const MisuraCpa n = misura_cpa(prima);
    CHECK_FALSE(n.valida);
    CHECK_FALSE(n.motivo.empty());
    CHECK_FALSE(misura_cpa({{0, 100, 0}, {10, 100, 0}}).valida);
}

TEST_CASE("misure sonar: il CPA misurato controlla una soluzione della TMA") {
    Passaggio g;
    g.proprio0 = {0.0, -2000.0};
    g.v_proprio = {0.0, 2.0};   // il battello va a nord a 2 m/s
    Casuale caso(5);
    std::vector<PuntoFrequenza> punti;
    Tma tma;
    for (double t = 0.0; t <= 1500.0; t += 10.0) {
        punti.push_back({t, g.frequenza(t) + 0.002 * gauss(caso), 0.002});
        tma.aggiungi({t, g.proprio(t), rilevamento(g.proprio(t), g.bersaglio(t))});
    }
    const MisuraCpa m = misura_cpa(punti);
    REQUIRE(m.valida);
    // la soluzione vera, riferita all'ultimo rilevamento
    SoluzioneTma vera;
    vera.distanza_m = distanza(g.proprio(1500.0), g.bersaglio(1500.0));
    vera.rotta = 90.0;
    vera.velocita_m_s = 5.0;
    const auto s = confronta_cpa(tma, vera, m);
    REQUIRE(s.has_value());
    CHECK(s->coerente);
    CHECK(std::fabs(s->scarto_t_s) < 60.0);
    // una soluzione sbagliata (meta' distanza) non torna col CPA
    SoluzioneTma sbagliata = vera;
    sbagliata.distanza_m *= 0.5;
    CHECK_FALSE(confronta_cpa(tma, sbagliata, m)->coerente);
    CHECK_FALSE(confronta_cpa(Tma{}, vera, m).has_value());
}

TEST_CASE("misure sonar: FRAZ, distanza da df/dt e dal rateo, con i suoi limiti") {
    Passaggio g;
    g.bersaglio0 = {-2000.0, 1500.0};
    Casuale caso(3);
    const auto finestra = [&](double t0, double t1, double rumore_hz, double rumore_deg) {
        std::vector<PuntoFrequenza> f;
        std::vector<Rilevamento> r;
        for (double t = t0; t <= t1; t += 5.0) {
            f.push_back({t, g.frequenza(t) + rumore_hz * gauss(caso), rumore_hz});
            r.push_back({t, g.proprio(t), rilevamento(g.proprio(t), g.bersaglio(t)) + rumore_deg * gauss(caso)});
        }
        return std::pair{f, r};
    };
    // vicino al CPA (a 400 s il bersaglio e' sulla verticale, a 1500 m)
    auto [f, r] = finestra(340.0, 460.0, 0.0005, 0.05);
    const StimaFraz s = distanza_fraz(g.f0, f, r);
    REQUIRE(s.valida);
    CHECK(s.distanza_m == doctest::Approx(1500.0).epsilon(0.15));
    CHECK(s.sigma_m > 0.0);
    CHECK_FALSE(distanza_fraz(g.f0, f, r, true).valida);   // manovra nella finestra
    // lontano (bersaglio a ~40 km): rateo minuscolo, niente stima
    g.bersaglio0 = {-40000.0, 1500.0};
    auto [f2, r2] = finestra(0.0, 120.0, 0.0005, 0.05);
    const StimaFraz lontano = distanza_fraz(g.f0, f2, r2);
    CHECK_FALSE(lontano.valida);
    CHECK_FALSE(lontano.motivo.empty());
    // la formula: R = -c df/dt / (f0 (dB/dt)^2)
    const StimaFraz x = distanza_fraz(100.0, {-0.001, 1e-6, 10}, {gradi(0.01), 1e-6, 10});
    CHECK(x.distanza_m == doctest::Approx(1500.0 * 0.001 / (100.0 * 1e-4)));
}

TEST_CASE("misure sonar: la soluzione dal CPA per il MATE (blocco 3 del sonar)") {
    // il battello a nord a 2 m/s, il bersaglio a est a 5 m/s: la soluzione dal CPA misurato torna con la vera
    Passaggio g;
    g.proprio0 = {0.0, -2000.0};
    g.v_proprio = {0.0, 2.0};
    Casuale caso(7);
    std::vector<PuntoFrequenza> punti;
    Tma tma;
    for (double t = 0.0; t <= 1500.0; t += 10.0) {
        punti.push_back({t, g.frequenza(t) + 0.002 * gauss(caso), 0.002});
        tma.aggiungi({t, g.proprio(t), normalizza_360(rilevamento(g.proprio(t), g.bersaglio(t)) + 0.3 * gauss(caso))});
    }
    const MisuraCpa m = misura_cpa(punti);
    REQUIRE(m.valida);
    REQUIRE(m.cpa_osservato);
    const auto s = soluzione_da_cpa(tma, m);
    REQUIRE(s.has_value());
    const double vera = distanza(g.proprio(1500.0), g.bersaglio(1500.0));
    MESSAGE("dal CPA: " << s->soluzione.distanza_m << " m (vera " << vera << "), rotta " << s->soluzione.rotta << ", "
                        << s->soluzione.velocita_m_s << " m/s; sigma " << s->sigma_distanza_m << " m");
    CHECK(s->soluzione.distanza_m == doctest::Approx(vera).epsilon(0.08));
    CHECK(std::fabs(normalizza_180(s->soluzione.rotta - 90.0)) < 8.0);
    CHECK(s->soluzione.velocita_m_s == doctest::Approx(5.0).epsilon(0.1));
    CHECK(s->sigma_distanza_m > 0.0);
    // coerente con il controllo di sempre
    CHECK(confronta_cpa(tma, s->soluzione, m)->coerente);
    // senza il CPA osservato (solo prima) niente proposta
    std::vector<PuntoFrequenza> prima;
    Tma solo_prima;
    for (double t = 0.0; t <= 400.0; t += 10.0) {
        prima.push_back({t, g.frequenza(t), 0.002});
        solo_prima.aggiungi({t, g.proprio(t), rilevamento(g.proprio(t), g.bersaglio(t))});
    }
    const MisuraCpa mp = misura_cpa(prima);
    CHECK_FALSE(soluzione_da_cpa(solo_prima, mp).has_value());
    CHECK_FALSE(soluzione_da_cpa(Tma{}, m).has_value());
}
