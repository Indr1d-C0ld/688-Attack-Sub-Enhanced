// Variazioni nel modello acustico (R6): individuo, regime delle pompe, ausiliari, aspetto, virate, danni, usura; con le
// variazioni spente la sorgente e' quella della classe.
#include <doctest.h>

#include <algorithm>
#include <cmath>

#include "nucleo/analisi_acustica.hpp"

using namespace m688;

namespace {

const CatalogoAcustico& catalogo() {
    static const CatalogoAcustico c = carica_acustica(M688_DATI "/acustica.toml");
    return c;
}

UnitaAcustica unita(const std::string& classe, double kn, std::uint64_t seme = 0, bool variazioni = true) {
    UnitaAcustica u;
    u.id = 3;
    u.classe = classe;
    u.categoria = classe == "perry" || classe == "mercantile" ? "nave" : "sottomarino";
    u.quota_m = u.categoria == "nave" ? 0.0 : 150.0;
    u.velocita_kn = kn;
    u.rotta = 0.0;
    u.variazioni = variazioni;
    u.seme = seme;
    return u;
}

const RigaEmessa* riga(const std::vector<RigaEmessa>& v, double hz, double tol = 0.04) {
    for (const auto& r : v)
        if (std::fabs(r.hz - hz) <= tol * hz) return &r;
    return nullptr;
}

Ambiente mare() {
    Ambiente a;
    a.fondale_m = 3000.0;
    a.vento_m_s = vento_per_stato_del_mare(3);
    return a;
}

}  // namespace

TEST_CASE("variazioni: spente, la sorgente e' quella della classe") {
    UnitaAcustica u = unita("los_angeles", 12.0, 77, false);
    u.danno_elica = u.danno_macchine = u.usura = 1.0;
    u.urto_db = 12.0;
    u.rateo_virata_deg_s = 3.0;
    u.regime_pompe = RegimePompe::spente;
    const Sorgente a = sorgente_acustica(catalogo(), u), b = sorgente_acustica(catalogo(), unita("los_angeles", 12.0, 0, false));
    CHECK_FALSE(a.var.attive);
    CHECK(livello_banda_larga(a) == livello_banda_larga(b));
    CHECK(frequenza_pala_hz(a) == frequenza_pala_hz(b));
    const auto ra = righe_emesse(a), rb = righe_emesse(b);
    REQUIRE(ra.size() == rb.size());
    for (std::size_t i = 0; i < ra.size(); ++i) {
        CHECK(ra[i].hz == rb[i].hz);
        CHECK(ra[i].db == rb[i].db);
    }
    CHECK(aspetto_db(a, {0.0, 5000.0}, 1000.0) == 0.0);
    // armi e falsi bersagli restano senza variazioni anche accese
    UnitaAcustica arma = unita("mk48", 40.0, 5);
    arma.categoria = "arma";
    CHECK_FALSE(sorgente_acustica(catalogo(), arma).var.attive);
}

TEST_CASE("variazioni: due scafi della stessa classe non suonano uguali") {
    const Sorgente a = sorgente_acustica(catalogo(), unita("victor3", 10.0, 1)), b = sorgente_acustica(catalogo(), unita("victor3", 10.0, 2));
    CHECK(a.var.attive);
    CHECK(livello_banda_larga(a) != livello_banda_larga(b));
    // ripetibile dal seme
    CHECK(livello_banda_larga(a) == livello_banda_larga(sorgente_acustica(catalogo(), unita("victor3", 10.0, 1))));
    const auto ra = righe_emesse(a), rb = righe_emesse(b);
    // la rete e' la stessa per tutta la classe; turbine e riduttori si spostano dell'1-3%, le pompe appena
    const auto* rete_a = riga(ra, 50.0, 1e-9);
    const auto* rete_b = riga(rb, 50.0, 1e-9);
    REQUIRE(rete_a);
    REQUIRE(rete_b);
    CHECK(rete_a->hz == rete_b->hz);
    const auto* turbina = riga(ra, 88.0);
    REQUIRE(turbina);
    CHECK(std::fabs(turbina->hz / 88.0 - 1.0) <= 0.03 + 1e-12);
    const auto* pompa = riga(ra, 36.0);
    REQUIRE(pompa);
    CHECK(std::fabs(pompa->hz / 36.0 - 1.0) <= 0.005 + 1e-12);
    // la dispersione della banda larga su molti scafi: sigma ~2,4 dB (Sponagle)
    const double base = livello_banda_larga(sorgente_acustica(catalogo(), unita("victor3", 10.0, 0, false)));
    double q = 0.0;
    int elica_cantante = 0;
    for (std::uint64_t k = 1; k <= 400; ++k) {
        const Sorgente s = sorgente_acustica(catalogo(), unita("victor3", 10.0, k));
        q += std::pow(livello_banda_larga(s) - base, 2.0);
        elica_cantante += s.var.cantante_hz > 0.0;
        CHECK(std::fabs(s.var.fattore_giri - 1.0) <= 0.03 + 1e-12);
    }
    CHECK(std::sqrt(q / 400.0) == doctest::Approx(2.4).epsilon(0.2));
    CHECK(elica_cantante >= 8);    // ~5%
    CHECK(elica_cantante <= 35);
}

TEST_CASE("variazioni: il regime delle pompe e la legge di Ross") {
    // Los Angeles: pompa lenta 42,3 Hz sotto 15 kn, veloce 84,6 da 15 kn
    UnitaAcustica u = unita("los_angeles", 10.0, 9);
    const auto automatico = righe_emesse(sorgente_acustica(catalogo(), u));
    CHECK(riga(automatico, 42.3) != nullptr);
    CHECK(riga(automatico, 84.6, 0.01) == nullptr);
    u.regime_pompe = RegimePompe::veloci;
    const auto veloci = righe_emesse(sorgente_acustica(catalogo(), u));
    REQUIRE(riga(veloci, 84.6, 0.01) != nullptr);
    CHECK(riga(veloci, 42.3, 0.01) == nullptr);
    // +40 log 2 = 12 dB sulla lenta (piu' o meno il ritratto dello scafo, +-2,5 per riga)
    CHECK(riga(veloci, 84.6, 0.01)->db - riga(automatico, 42.3, 0.01)->db == doctest::Approx(12.0).epsilon(0.5));
    u.regime_pompe = RegimePompe::spente;
    const auto spente = righe_emesse(sorgente_acustica(catalogo(), u));
    CHECK(std::none_of(spente.begin(), spente.end(), [](const RigaEmessa& r) { return r.tipo == TipoRiga::pompa; }));
    // la cascata lo vede: la riga della pompa veloce nel fascio, non quella della lenta
    Ricevitore r{{0.0, 1000.0}, 150.0, 90.0, 5.0, catalogo().sensore("tb16")};   // al traverso della schiera
    u.regime_pompe = RegimePompe::veloci;
    const Spettro sp = spettro_fascio({sorgente_acustica(catalogo(), u)}, r, mare(), 180.0, 30.0, 100.0, 0.05);
    const auto snr_a = [&](double hz) {
        double m = 0.0;
        for (std::size_t i = 0; i < sp.snr_db.size(); ++i)
            if (std::fabs(sp.frequenza(i) - hz) < 0.5) m = std::max(m, static_cast<double>(sp.snr_db[i]));
        return m;
    };
    const double veloce = riga(veloci, 84.6, 0.01)->hz;
    CHECK(snr_a(veloce) > 6.0);
    CHECK(snr_a(42.3) < 3.0);
}

TEST_CASE("variazioni: gli ausiliari si accendono e si spengono a turni") {
    int accesi = 0;
    for (int turno = 0; turno < 200; ++turno) {
        UnitaAcustica u = unita("victor3", 8.0, 4);
        u.tempo_s = 1200.0 * turno + 10.0;
        accesi += riga(righe_emesse(sorgente_acustica(catalogo(), u)), 185.0) != nullptr;
    }
    CHECK(accesi == doctest::Approx(140).epsilon(0.15));   // 70%
    // nello stesso turno non cambia
    UnitaAcustica u = unita("victor3", 8.0, 4);
    u.tempo_s = 100.0;
    const bool prima = riga(righe_emesse(sorgente_acustica(catalogo(), u)), 185.0) != nullptr;
    u.tempo_s = 1100.0;
    CHECK((riga(righe_emesse(sorgente_acustica(catalogo(), u)), 185.0) != nullptr) == prima);
}

TEST_CASE("variazioni: aspetto, virate, danni e usura") {
    // nave: dal traverso alla prua -10 dB a 1 kHz, ~-3 sotto 100 Hz; poppa meno attenuata; sottomarino meta'
    const Sorgente nave = sorgente_acustica(catalogo(), unita("perry", 15.0, 3));
    const double prua = aspetto_db(nave, {0.0, 5000.0}, 1000.0), poppa = aspetto_db(nave, {0.0, -5000.0}, 1000.0);
    CHECK(std::fabs(aspetto_db(nave, {5000.0, 0.0}, 1000.0)) < 0.2);   // traverso (il massimo e' a 97 gradi)
    CHECK(prua == doctest::Approx(-10.4).epsilon(0.1));
    CHECK(poppa > prua + 2.0);
    CHECK(aspetto_db(nave, {0.0, 5000.0}, 50.0) == doctest::Approx(0.3 * prua));
    const Sorgente sub = sorgente_acustica(catalogo(), unita("victor3", 10.0, 3));
    CHECK(aspetto_db(sub, {0.0, 5000.0}, 1000.0) == doctest::Approx(0.3 * prua).epsilon(0.05));   // sottomarino: 30%
    // la scoperta lo vede: di prua si sente meno che al traverso
    Ricevitore di_prua{{0.0, 8000.0}, 150.0, 180.0, 5.0, catalogo().sensore("bqq5_sfera")};
    Ricevitore al_traverso{{8000.0, 0.0}, 150.0, 270.0, 5.0, catalogo().sensore("bqq5_sfera")};
    REQUIRE(di_prua.sensore != nullptr);
    CHECK(valuta(nave, di_prua, mare()).se_db < valuta(nave, al_traverso, mare()).se_db - 2.0);
    // virata di 3 gradi/s: +4,95 dB
    UnitaAcustica u = unita("victor3", 10.0, 3);
    const double dritto = livello_banda_larga(sorgente_acustica(catalogo(), u));
    u.rateo_virata_deg_s = 3.0;
    CHECK(livello_banda_larga(sorgente_acustica(catalogo(), u)) - dritto == doctest::Approx(4.95));
    u.rateo_virata_deg_s = 0.0;
    // usura piena: +5 dB
    u.usura = 1.0;
    CHECK(livello_banda_larga(sorgente_acustica(catalogo(), u)) - dritto == doctest::Approx(5.0));
    u.usura = 0.0;
    // dopo un urto (DanniSottomarini::rumore_urto_db): i dB in piu' sulla banda larga, al piu' 12
    u.urto_db = 6.0;
    CHECK(livello_banda_larga(sorgente_acustica(catalogo(), u)) - dritto == doctest::Approx(6.0));
    u.urto_db = 30.0;
    CHECK(livello_banda_larga(sorgente_acustica(catalogo(), u)) - dritto == doctest::Approx(12.0));
    u.urto_db = 0.0;
    // elica danneggiata: cavita prima e piu' forte, con le bande laterali
    const double v_cav = velocita_cavitazione_kn(*catalogo().firma("victor3"), 150.0);
    u.velocita_kn = 0.8 * v_cav;
    CHECK_FALSE(cavita(*catalogo().firma("victor3"), u.velocita_kn, 150.0));
    const double sano = livello_banda_larga(sorgente_acustica(catalogo(), u));
    u.danno_elica = 1.0;
    const Sorgente rotta = sorgente_acustica(catalogo(), u);
    CHECK(livello_banda_larga(rotta) > sano + 8.0);
    const double fp = frequenza_pala_hz(rotta), fs = frequenza_asse_hz(rotta);
    REQUIRE(fp > 0.0);
    const auto rr = righe_emesse(rotta);
    CHECK(riga(rr, fp - fs, 1e-6) != nullptr);
    CHECK(riga(rr, fp + fs, 1e-6) != nullptr);
    // macchine danneggiate a meta': +13 dB sulle righe dei macchinari, la rete no
    u.danno_elica = 0.0;
    u.velocita_kn = 8.0;
    const auto sane = righe_emesse(sorgente_acustica(catalogo(), u));
    u.danno_macchine = 0.5;
    const auto guaste = righe_emesse(sorgente_acustica(catalogo(), u));
    CHECK(riga(guaste, 88.0)->db - riga(sane, 88.0)->db == doctest::Approx(13.0));
    CHECK(riga(guaste, 50.0, 1e-9)->db == riga(sane, 50.0, 1e-9)->db);
    // e la scoperta lo vede nel LOFAR: con le macchine guaste la riga piu' forte non e' piu' la rete
    Ricevitore r{{0.0, 20000.0}, 150.0, 180.0, 5.0, catalogo().sensore("tb16")};
    u.danno_macchine = 0.0;
    const double prima = valuta(sorgente_acustica(catalogo(), u), r, mare()).se_tonale_db;
    u.danno_macchine = 1.0;
    CHECK(valuta(sorgente_acustica(catalogo(), u), r, mare()).se_tonale_db > prima + 3.0);
}

TEST_CASE("variazioni: l'ACINT riconosce la classe anche con lo scafo diverso") {
    int giusti = 0;
    for (std::uint64_t k = 1; k <= 20; ++k) {
        std::vector<RigaOsservata> osservate;
        for (const auto& r : righe_emesse(sorgente_acustica(catalogo(), unita("victor3", 8.0, k))))
            if (r.tipo != TipoRiga::elica) osservate.push_back({r.hz, 10.0});
        const auto ipotesi = confronta_rapporti(osservate, catalogo());
        giusti += !ipotesi.empty() && ipotesi.front().id == "victor3";
    }
    CHECK(giusti >= 18);
}
