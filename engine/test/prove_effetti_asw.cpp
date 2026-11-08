// Effetti dell'ASW aereo (grafica/effetti.hpp, docs/13 pezzo A4): hover, cavo, luci, MAD, boe, paracadute, marcatori.
#include <doctest.h>

#include <cmath>

#include "grafica/effetti.hpp"

using namespace m688;

namespace {

int conta(const std::vector<Particella>& v, Materia m) {
    int n = 0;
    for (const auto& p : v) n += p.materia == m;
    return n;
}

AereoAsw elicottero(double quota, double rotore) {
    AereoAsw a;
    a.id = 7; a.elicottero = true; a.hover = true; a.quota_m = quota; a.rotore_m = rotore; a.apertura_m = 4; a.luci = false;
    return a;
}

}  // namespace

TEST_CASE("ASW aereo: anello di spruzzi sotto l'hover, piu' forte sotto il Sea King a 12 m che sotto il Ka-27 a 30 m") {
    Mondo m;
    auto spruzzi = [&](const AereoAsw& a) {
        Effetti e(4);
        int n = 0;
        double r_min = 1e9, r_max = 0;
        for (int k = 0; k < 50; ++k) {
            e.aerei_asw({a});
            e.avanza(0.1, m);
        }
        for (const auto& p : e.particelle())
            if (p.materia == Materia::spruzzo) {
                ++n;
                const double r0 = std::hypot(p.x - p.vx * p.eta, p.y - p.vy * p.eta);   // dove e' nata
                r_min = std::min(r_min, r0), r_max = std::max(r_max, r0);
            }
        return std::tuple{n, r_min, r_max};
    };
    const auto [n_sh3, rmin_sh3, rmax_sh3] = spruzzi(elicottero(12, 18.9));
    const auto [n_ka27, rmin_ka27, rmax_ka27] = spruzzi(elicottero(30, 15.9));
    CHECK(n_sh3 > 0);
    CHECK(n_ka27 > 0);
    CHECK(n_sh3 > n_ka27);
    CHECK(rmin_sh3 > 5.0);    // un anello, non una macchia sotto l'elicottero
    CHECK(rmax_sh3 < 25.0);
    AereoAsw fermo = elicottero(12, 18.9);
    fermo.hover = false;
    CHECK(std::get<0>(spruzzi(fermo)) == 0);
}

TEST_CASE("ASW aereo: cavo e trasduttore che scendono, tonfo quando il trasduttore entra in acqua") {
    Mondo m;
    Effetti e(1);
    AereoAsw a = elicottero(30, 15.9);
    a.cavo_m = 10;
    e.aerei_asw({a});
    e.avanza(0.1, m);
    // cavo dalla pancia lungo quanto e' filato, e il trasduttore appeso in fondo
    const Particella* cavo = nullptr;
    const Particella* trasduttore = nullptr;
    for (const auto& p : e.orli()) {
        if (p.materia == Materia::cavo) cavo = &p;
        if (p.materia == Materia::detrito) trasduttore = &p;
    }
    REQUIRE(cavo);
    REQUIRE(trasduttore);
    CHECK(cavo->z == doctest::Approx(29.0));
    CHECK(cavo->H == doctest::Approx(10.0));
    CHECK(trasduttore->z == doctest::Approx(19.0));
    const int prima = conta(e.particelle(), Materia::spruzzo);
    a.cavo_m = 40;   // il trasduttore passa sotto il pelo dell'acqua
    e.aerei_asw({a});
    e.avanza(0.1, m);
    CHECK(conta(e.particelle(), Materia::spruzzo) > prima);
    for (const auto& p : e.orli())
        if (p.materia == Materia::cavo) CHECK(p.H == doctest::Approx(29.0));   // il filo finisce sul mare
    CHECK(conta(e.orli(), Materia::detrito) == 0);   // il trasduttore e' sott'acqua
}

TEST_CASE("ASW aereo: luci rossa a sinistra e verde a dritta, spente in caccia; MAD dietro l'elicottero in volo") {
    Mondo m;
    Effetti e(2);
    AereoAsw a;
    a.id = 1; a.quota_m = 100; a.rotta = 0; a.apertura_m = 30; a.luci = true;   // verso nord
    e.aerei_asw({a});
    e.avanza(0.1, m);
    int rosse_ovest = 0, verdi_est = 0, luci = 0;
    for (const auto& p : e.orli()) {
        if (p.materia != Materia::luce) continue;
        ++luci;
        if (p.tono < 0.25 && p.x < -10) ++rosse_ovest;
        if (p.tono > 0.25 && p.tono < 0.75 && p.x > 10) ++verdi_est;
    }
    CHECK(luci >= 3);
    CHECK(rosse_ovest == 1);
    CHECK(verdi_est == 1);
    a.luci = false;
    e.aerei_asw({a});
    e.avanza(0.1, m);
    CHECK(conta(e.orli(), Materia::luce) == 0);
    // MAD rimorchiato: in volo verso nord il «bird» resta a sud e sotto
    AereoAsw h;
    h.id = 2; h.elicottero = true; h.quota_m = 60; h.velocita = 40; h.rotta = 0; h.mad_m = 55; h.luci = false;
    e.aerei_asw({h});
    e.avanza(0.1, m);
    double y_bird = 1e9, z_bird = 1e9, r_bird = 0;
    for (const auto& p : e.orli())
        if (p.materia == Materia::detrito && p.r > r_bird) r_bird = p.r, y_bird = p.y, z_bird = p.z;
    CHECK(y_bird < -30.0);
    CHECK(z_bird < 59.0);
    CHECK(z_bird > 3.0);
}

TEST_CASE("ASW aereo: boa che scende, tonfo, colorante solo per le boe USA, galleggiante fino alla fine") {
    Mondo m;
    Effetti e(3);
    std::vector<Sonoboa> boe(2);
    boe[0].id = 1001; boe[0].tipo = "ssq53b"; boe[0].pos = {0, 0};
    boe[1].id = 1002; boe[1].tipo = "rgb2"; boe[1].pos = {100, 0};
    for (auto& b : boe) { b.t_posa = 10; b.t_impatto = 13; b.t_attiva = 100; b.t_fine = 200; }
    auto passo = [&](double t) {
        e.boe_asw(boe, t);
        e.avanza(0.1, m);
    };
    passo(11.0);
    double z1 = 0;
    for (const auto& p : e.orli()) z1 = std::max(z1, p.z);
    passo(12.5);
    double z2 = 0;
    for (const auto& p : e.orli()) z2 = std::max(z2, p.z);
    CHECK(z1 > z2);   // scende
    CHECK(conta(e.particelle(), Materia::colorante) == 0);
    passo(13.05);
    CHECK(conta(e.particelle(), Materia::colorante) == 1);   // solo la SSQ-53B
    CHECK(conta(e.particelle(), Materia::spruzzo) > 0);
    passo(50.0);
    CHECK(conta(e.orli(), Materia::cavo) == 2);   // due antenne
    passo(250.0);
    CHECK(conta(e.orli(), Materia::cavo) == 0);   // affondate a fine vita
}

TEST_CASE("ASW aereo: siluro col paracadute, il tonfo solo all'impatto") {
    Mondo m;
    Effetti e(5);
    const double t = e.arma_paracadute({0, 0}, 45, 90, 50, false);
    CHECK(t == doctest::Approx(3.0));
    CHECK(e.arma_paracadute({0, 0}, 50, 90, 50, true) > t);   // il paracadute sovietico e' piu' lento
    Effetti f(5);
    f.arma_paracadute({0, 0}, 45, 90, 50, false);
    auto spruzzi_visibili = [&] {
        int n = 0;
        for (const auto& p : f.particelle()) n += p.materia == Materia::spruzzo && p.ritardo <= 0;
        return n;
    };
    for (int k = 0; k < 25; ++k) f.avanza(0.1, m);
    CHECK(spruzzi_visibili() == 0);
    double z_min = 1e9;
    for (const auto& p : f.particelle())
        if (p.materia == Materia::detrito) z_min = std::min(z_min, p.z);
    CHECK(z_min > 0.0);
    CHECK(z_min < 10.0);
    for (int k = 0; k < 10; ++k) f.avanza(0.1, m);
    CHECK(spruzzi_visibili() > 0);
}

TEST_CASE("ASW aereo: marcatori Mk 25, Mk 58 e OMAB, accesi per la loro durata") {
    Mondo m;
    Effetti e(6);
    e.marcatore_asw(MarcatoreAsw::mk58, {0, 0}, 60);
    e.marcatore_asw(MarcatoreAsw::omab_giorno, {50, 0}, 60);
    e.avanza(0.1, m);
    CHECK(conta(e.orli(), Materia::fuoco) == 2);   // le due candele
    CHECK(conta(e.particelle(), Materia::colorante) == 1);
    for (int k = 0; k < 300; ++k) e.avanza(0.1, m);
    CHECK(conta(e.particelle(), Materia::fumo) > 0);
    for (int k = 0; k < 400; ++k) e.avanza(0.1, m);
    CHECK(conta(e.orli(), Materia::fuoco) == 0);
    CHECK(conta(e.particelle(), Materia::colorante) == 0);
}

TEST_CASE("ASW aereo: deterministico") {
    Mondo m;
    auto gira = [&] {
        Effetti e(9);
        AereoAsw a = elicottero(12, 18.9);
        a.cavo_m = 50;
        e.marcatore_asw(MarcatoreAsw::mk25, {10, 0}, 600);
        for (int k = 0; k < 40; ++k) {
            e.aerei_asw({a});
            e.avanza(0.1, m);
        }
        double s = 0;
        for (const auto& p : e.particelle()) s += p.x + 2 * p.y + 3 * p.z;
        return std::pair{e.numero(), s};
    };
    CHECK(gira() == gira());
}
