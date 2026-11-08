// Effetti visivi dell'artiglieria (docs/16_artiglieria.md, G4): colonne per calibro, rimbalzi, raffiche, vampe,
// illuminanti, acqua sul vetro; la testa colpita nell'oculare.
#include <doctest.h>

#include <algorithm>
#include <cmath>

#include "grafica/effetti.hpp"
#include "grafica/oculare.hpp"
#include "grafica/tela.hpp"

using namespace m688;

namespace {

EventoArtiglieria evento(TipoEventoArtiglieria tipo, Vec2 pos, double t, double calibro, double carica) {
    EventoArtiglieria e;
    e.tipo = tipo;
    e.pos = pos;
    e.tempo_s = t;
    e.calibro_mm = calibro;
    e.carica_kg = carica;
    e.tiratore = 1;
    return e;
}

Mondo con_tiratore() {
    Mondo m;
    Nave n;
    n.id = 1;
    n.classe = "perry";
    n.pos = {0, -3000};   // a sud: la linea di tiro va verso nord
    n.pos_prec = n.pos;
    m.aggiungi(n);
    return m;
}

double quota_massima(const Effetti& e, Materia m) {
    double z = 0;
    for (const auto& p : e.particelle())
        if (p.materia == m && p.ritardo <= 0) z = std::max(z, p.z);
    return z;
}

}  // namespace

TEST_CASE("artiglieria: colonne per calibro (dossier 22 §6.1)") {
    // dalla tabella: 30 mm 3,5 m, 76 mm 12,5 m, 5" 25 m, 16" 80 m; crescono col calibro; l'inerte e' piu' basso
    CHECK(Effetti::colonna_proietto(30, 0.05).altezza_m == doctest::Approx(3.5));
    CHECK(Effetti::colonna_proietto(76, 0.75).altezza_m == doctest::Approx(12.5));
    CHECK(Effetti::colonna_proietto(130, 3.5).altezza_m == doctest::Approx(25.0));
    CHECK(Effetti::colonna_proietto(406, 70).altezza_m == doctest::Approx(80.0));
    double prima = 0;
    for (double c = 30; c <= 406; c += 10) {
        const double h = Effetti::colonna_proietto(c, 1.0).altezza_m;
        CHECK(h >= prima);
        prima = h;
    }
    CHECK(Effetti::colonna_proietto(127, 0).altezza_m < 0.5 * Effetti::colonna_proietto(127, 3.5).altezza_m);
    // la colonna del 5" sale fino a circa 25 m (+-20%) in circa sqrt(2H/g) e poi ricade; il lampo alla base subito
    const Mondo m = con_tiratore();
    Effetti e(5);
    e.artiglieria(evento(TipoEventoArtiglieria::scoppio_superficie, {0, 500}, 10.0, 127, 3.5), 10.0);
    double alta = 0;
    bool lampo = false;
    for (double t = 0; t < 4.0; t += 0.02) {
        e.avanza(0.02, m);
        alta = std::max(alta, quota_massima(e, Materia::spruzzo));
        if (t < 0.2) for (const auto& p : e.particelle()) lampo |= p.materia == Materia::fuoco;
    }
    CHECK(alta > 15.0);
    CHECK(alta < 32.0);
    CHECK(lampo);
    // il 16" dell'Iowa: acqua tinta; l'inerte: niente lampo
    Effetti iowa(5);
    iowa.artiglieria(evento(TipoEventoArtiglieria::scoppio_superficie, {0, 2000}, 0, 406, 70), 0);
    iowa.avanza(0.5, m);
    CHECK(std::any_of(iowa.particelle().begin(), iowa.particelle().end(), [](const Particella& p) { return p.materia == Materia::tinta; }));
    Effetti inerte(5);
    inerte.artiglieria(evento(TipoEventoArtiglieria::tonfo, {0, 500}, 0, 127, 0), 0);
    inerte.avanza(0.05, m);
    CHECK(std::none_of(inerte.particelle().begin(), inerte.particelle().end(), [](const Particella& p) { return p.materia == Materia::fuoco; }));
}

TEST_CASE("artiglieria: gli eventi partono al loro tempo; deterministico; niente senza eventi") {
    const Mondo m = con_tiratore();
    Effetti e(9);
    e.artiglieria(evento(TipoEventoArtiglieria::scoppio_superficie, {0, 500}, 13.0, 127, 3.5), 10.0);   // fra 3 s
    for (int k = 0; k < 140; ++k) e.avanza(0.02, m);   // 2,8 s
    CHECK(e.numero() == 0);
    for (int k = 0; k < 20; ++k) e.avanza(0.02, m);
    CHECK(e.numero() > 0);
    // stesso seme e stessi eventi: le stesse particelle
    Effetti a(4), b(4);
    for (auto* x : {&a, &b}) {
        x->artiglieria(evento(TipoEventoArtiglieria::scoppio_superficie, {10, 600}, 0, 76, 0.75), 0);
        for (int k = 0; k < 50; ++k) x->avanza(0.02, m);
    }
    REQUIRE(a.numero() == b.numero());
    for (std::size_t i = 0; i < a.numero(); ++i) CHECK(a.particelle()[i].z == b.particelle()[i].z);
    Effetti vuoto(4);
    for (int k = 0; k < 100; ++k) vuoto.avanza(0.02, m);
    CHECK(vuoto.numero() == 0);
}

TEST_CASE("artiglieria: rimbalzi lungo la linea di tiro, raffica coi traccianti, vampa sulla nave") {
    const Mondo m = con_tiratore();
    // rimbalzo a (0, 500) col tiratore a sud: gli spruzzi successivi piu' a nord
    Effetti r(3);
    r.artiglieria(evento(TipoEventoArtiglieria::rimbalzo, {0, 500}, 0, 127, 0), 0);
    double nord = 0;
    for (int k = 0; k < 150; ++k) {
        r.avanza(0.02, m);
        for (const auto& p : r.particelle())
            if (p.ritardo <= 0) nord = std::max(nord, p.y);
    }
    CHECK(nord > 560.0);
    // raffica: colonnine attorno al punto e traccianti in volo dal tiratore
    Effetti q(3);
    auto raffica = evento(TipoEventoArtiglieria::raffica, {0, 600}, 0, 30, 0.05);
    raffica.colpi = 40;
    raffica.intervallo_s = 0.013;
    q.artiglieria(raffica, 0);
    q.avanza(0.02, m);
    int traccianti = 0;
    for (int k = 0; k < 10; ++k) {
        q.avanza(0.02, m);
        for (const auto& p : q.particelle()) traccianti += p.materia == Materia::tracciante && p.ritardo <= 0;
    }
    CHECK(traccianti > 0);
    // vampa: fuoco alla nave che spara, e il fumo che resta
    Effetti v(3);
    v.artiglieria(evento(TipoEventoArtiglieria::partenza, {0, -3000}, 0, 127, 0), 0);
    v.avanza(0.02, m);
    v.avanza(0.02, m);
    CHECK(std::any_of(v.particelle().begin(), v.particelle().end(), [](const Particella& p) { return p.materia == Materia::fuoco && std::fabs(p.y + 3000) < 20; }));
    for (int k = 0; k < 100; ++k) v.avanza(0.02, m);
    CHECK(std::any_of(v.particelle().begin(), v.particelle().end(), [](const Particella& p) { return p.materia == Materia::fumo; }));
}

TEST_CASE("artiglieria: illuminante e la sua luce, acqua sul vetro vicino alle colonne") {
    const Mondo m = con_tiratore();
    Effetti e(2);
    EventoArtiglieria il;
    il.tipo = TipoEventoArtiglieria::illuminante;
    il.pos = {0, 1000};
    il.quota_m = 500;
    il.calibro_mm = 127;
    e.artiglieria(il, 0);
    for (int k = 0; k < 250; ++k) e.avanza(0.02, m);   // 5 s
    const double vicino = e.luce_illuminanti({0, 1000}), lontano = e.luce_illuminanti({0, 6000});
    CHECK(vicino > 0.2);
    CHECK(lontano < vicino * 0.3);
    for (int k = 0; k < 2600; ++k) e.avanza(0.02, m);   // oltre i 50 s
    CHECK(e.luce_illuminanti({0, 1000}) == 0.0);
    // una colonna del 5" a 40 m dall'occhio bagna il vetro; a 400 m no
    Effetti c(2);
    c.artiglieria(evento(TipoEventoArtiglieria::scoppio_superficie, {0, 40}, 0, 127, 3.5), 0);
    double acqua = 0;
    for (int k = 0; k < 150; ++k) {
        c.avanza(0.02, m);
        acqua = std::max(acqua, c.acqua_sul_vetro({0, 0}));
    }
    CHECK(acqua > 0.4);
    CHECK(c.acqua_sul_vetro({0, -400}) == 0.0);
}

TEST_CASE("oculare: la testa colpita (spenta, incrinata) e niente se intatta") {
    ParametriOculare p;
    p.attivo = true;
    const Rettangolo a = oculare::area(p);
    const auto conta = [&](const Tela& t, std::uint8_t c) {
        int n = 0;
        for (int y = a.y; y < a.y + a.h; ++y)
            for (int x = a.x; x < a.x + a.w; ++x) n += t.at(x, y) == c;
        return n;
    };
    Tela intatta;
    intatta.riempi(110);
    oculare::guasto(intatta, p, 1.0, 5.0);
    CHECK(conta(intatta, 110) == a.w * a.h);
    ParametriOculare s = p;
    s.spento = 1.0;
    Tela spenta;
    spenta.riempi(110);
    oculare::guasto(spenta, s, 1.0, 5.0);
    CHECK(conta(spenta, colori::NERO) > a.w * a.h / 2);
    ParametriOculare c = p;
    c.incrinato = 0.6;
    c.seme_colpo = 5;
    Tela crepata;
    crepata.riempi(110);
    oculare::guasto(crepata, c, 1.0, 5.0);
    CHECK(conta(crepata, static_cast<std::uint8_t>(colori::GRIGI + 13)) > 100);
}
