#include <doctest.h>

#include <algorithm>
#include <cmath>
#include <vector>

#include "nucleo/casuale.hpp"
#include "nucleo/rilevatore_righe.hpp"

using namespace m688;

// Le righe trovate in automatico, ALI/ALT (blocco 3 del sonar, H; docs/23_sonar_blocco3.md §3)

namespace {

double gauss(Casuale& c) {
    const double u1 = std::max(c.uniforme(), 1e-12), u2 = c.uniforme();
    return std::sqrt(-2.0 * std::log(u1)) * std::cos(2.0 * PI * u2);
}

// una riga del LOFAR come la mostra la stazione: 10-210 Hz a 0,1 Hz, la grana di 1,5 dB, le righe date (hz, S/N)
Spettro riga_lofar(Casuale& caso, const std::vector<std::pair<double, double>>& righe) {
    Spettro s;
    s.f0_hz = 10.0;
    s.df_hz = 0.1;
    s.snr_db.assign(2000, 0.0f);
    for (float& x : s.snr_db) x = static_cast<float>(std::max(0.0, 1.5 + 1.5 * gauss(caso)));
    for (const auto& [hz, db] : righe) {
        const std::size_t i = static_cast<std::size_t>((hz - s.f0_hz) / s.df_hz);
        if (i < s.snr_db.size()) s.snr_db[i] = static_cast<float>(std::max(static_cast<double>(s.snr_db[i]), db + 1.5 * gauss(caso)));
    }
    return s;
}

const RigaAutomatica* vicina(const std::vector<RigaAutomatica>& v, double hz, double tol = 0.3) {
    for (const RigaAutomatica& r : v)
        if (std::fabs(r.hz - hz) <= tol) return &r;
    return nullptr;
}

}  // namespace

TEST_CASE("righe automatiche: una riga vera confermata presto, con l'id stabile e la deriva") {
    Casuale caso(1);
    RilevatoreRighe ril;
    int id = -1;
    double confermata_a = -1.0;
    for (int k = 0; k < 120; ++k) {   // una riga ogni 5 s per 10 minuti, che sale di 0,1 Hz al minuto (Doppler)
        const double t = 5.0 * k, hz = 100.0 + 0.1 * t / 60.0;
        ril.aggiungi(riga_lofar(caso, {{hz, 10.0}}), t);
        const auto c = ril.confermate();
        if (const RigaAutomatica* r = vicina(c, hz)) {
            if (confermata_a < 0.0) { confermata_a = t; id = r->id; }
            CHECK(r->id == id);   // la stessa riga per tutto il tempo
        }
    }
    CHECK(confermata_a >= 0.0);
    CHECK(confermata_a <= 25.0);   // M = 3 su N = 5: alla terza riga
    const RigaAutomatica* r = vicina(ril.confermate(), 101.0);
    REQUIRE(r != nullptr);
    CHECK(r->deriva_hz_min == doctest::Approx(0.1).epsilon(0.4));
    CHECK(r->eta_s(595.0) > 500.0);
}

TEST_CASE("righe automatiche: la grana fa poche righe false") {
    Casuale caso(2);
    RilevatoreRighe ril;
    int false_confermate = 0;
    std::vector<int> viste;
    for (int k = 0; k < 720; ++k) {   // un'ora di solo rumore, una riga ogni 5 s
        ril.aggiungi(riga_lofar(caso, {}), 5.0 * k);
        for (const RigaAutomatica& r : ril.confermate())
            if (std::find(viste.begin(), viste.end(), r.id) == viste.end()) { viste.push_back(r.id); ++false_confermate; }
    }
    MESSAGE("righe false confermate in un'ora di rumore su 200 Hz: " << false_confermate);
    CHECK(false_confermate < 60);   // poche, ma non zero: l'operatore le deve scartare
}

TEST_CASE("righe automatiche: una riga che svanisce e torna; due vicine che si fondono") {
    Casuale caso(3);
    RilevatoreRighe ril;
    int id_prima = -1;
    for (int k = 0; k < 40; ++k) {   // 200 s con la riga, poi 40 s senza (una frangia), poi di nuovo
        const double t = 5.0 * k;
        ril.aggiungi(riga_lofar(caso, {{150.0, 10.0}}), t);
    }
    REQUIRE(vicina(ril.confermate(), 150.0));
    id_prima = vicina(ril.confermate(), 150.0)->id;
    for (int k = 40; k < 48; ++k) ril.aggiungi(riga_lofar(caso, {}), 5.0 * k);
    for (int k = 48; k < 60; ++k) ril.aggiungi(riga_lofar(caso, {{150.0, 10.0}}), 5.0 * k);
    REQUIRE(vicina(ril.confermate(), 150.0));
    CHECK(vicina(ril.confermate(), 150.0)->id == id_prima);   // entro perdita_s: la stessa riga
    // assente per due minuti: persa, e torna con un altro id
    for (int k = 60; k < 90; ++k) ril.aggiungi(riga_lofar(caso, {}), 5.0 * k);
    CHECK_FALSE(vicina(ril.righe(), 150.0, 0.2));
    for (int k = 90; k < 100; ++k) ril.aggiungi(riga_lofar(caso, {{150.0, 10.0}}), 5.0 * k);
    REQUIRE(vicina(ril.confermate(), 150.0));
    CHECK(vicina(ril.confermate(), 150.0)->id != id_prima);
    // due righe a 0,15 Hz: una sola marca
    RilevatoreRighe due;
    for (int k = 0; k < 20; ++k) due.aggiungi(riga_lofar(caso, {{80.0, 10.0}, {80.15, 9.0}}), 5.0 * k);
    int vicino_80 = 0;
    for (const RigaAutomatica& r : due.confermate()) vicino_80 += std::fabs(r.hz - 80.07) < 0.3;
    CHECK(vicino_80 == 1);
}

TEST_CASE("righe automatiche: ripetibile") {
    const auto prova = [] {
        Casuale caso(7);
        RilevatoreRighe ril;
        for (int k = 0; k < 60; ++k) ril.aggiungi(riga_lofar(caso, {{60.0, 8.0}, {120.3, 6.0}}), 5.0 * k);
        return ril.righe();
    };
    const auto a = prova(), b = prova();
    REQUIRE(a.size() == b.size());
    for (std::size_t i = 0; i < a.size(); ++i) {
        CHECK(a[i].id == b[i].id);
        CHECK(a[i].hz == b[i].hz);
        CHECK(a[i].snr_db == b[i].snr_db);
    }
}

TEST_CASE("righe automatiche: una riga debole trovata dall'integratore; una che corre al CPA seguita") {
    Casuale caso(4);
    RilevatoreRighe ril;
    for (int k = 0; k < 60; ++k) {
        const double t = 5.0 * k;
        ril.aggiungi(riga_lofar(caso, {{70.0, 5.0}, {180.0 - 1.0 * t / 60.0, 10.0}}), t);   // 1 Hz al minuto: un CPA vicino
    }
    CHECK(vicina(ril.confermate(), 70.0) != nullptr);
    const RigaAutomatica* r = vicina(ril.confermate(), 175.0, 0.5);
    REQUIRE(r != nullptr);
    MESSAGE("deriva misurata " << r->deriva_hz_min << " Hz/min, eta' " << r->eta_s(295.0) << " s");
    CHECK(r->deriva_hz_min == doctest::Approx(-1.0).epsilon(0.3));
}
