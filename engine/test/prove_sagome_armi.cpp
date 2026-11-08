// Sagome delle armi e sezione del tubo (grafica/sagome_armi.hpp): dimensioni e orientamento, non i pixel.
#include <doctest.h>

#include <map>

#include "grafica/periscopio.hpp"
#include "grafica/sagome_armi.hpp"
#include "nucleo/carico.hpp"

#ifndef M688_DATI
#define M688_DATI "../data/motore"
#endif

using namespace m688;

namespace {

struct Impronta {
    int xa = 1 << 30, xb = -(1 << 30), ya = 1 << 30, yb = -(1 << 30);
    std::map<int, std::map<MaterialeArma, int>> per_colonna;   // x -> materiale -> pixel
    int w() const { return xb - xa + 1; }
    int h() const { return yb - ya + 1; }
    // pixel del materiale fra le frazioni a e b della larghezza (0 poppa, 1 prua)
    int conta(MaterialeArma m, double a, double b) const {
        int n = 0;
        for (const auto& [x, mm] : per_colonna) {
            const double f = static_cast<double>(x - xa) / std::max(1, w() - 1);
            if (f < a || f > b) continue;
            const auto it = mm.find(m);
            if (it != mm.end()) n += it->second;
        }
        return n;
    }
};

Impronta impronta(const SagomaArma& s, double px_m) {
    Impronta i;
    traccia_sagoma_arma(s, 100.0, 200.0, px_m, [&](int x, int y, MaterialeArma m, double tono) {
        CHECK(tono >= 0.0);
        CHECK(tono <= 1.0);
        i.xa = std::min(i.xa, x), i.xb = std::max(i.xb, x), i.ya = std::min(i.ya, y), i.yb = std::max(i.yb, y);
        ++i.per_colonna[x][m];
    });
    return i;
}

const CatalogoCarico& catalogo() {
    static const CatalogoCarico c = carica_carico(std::string(M688_DATI) + "/carico.toml");
    return c;
}

}  // namespace

TEST_CASE("sagome delle armi: in scala con lunghezza e diametro del catalogo") {
    const double px_m = 20.0;
    for (const auto& [id, a] : catalogo().armi) {
        CAPTURE(id);
        const SagomaArma s = sagoma_da_scheda(a);
        const double L = lunghezza_disegnata(s);
        CHECK(L >= (a.lunghezza_m > 0 ? a.lunghezza_m : 0.5));
        const Impronta i = impronta(s, px_m);
        REQUIRE(i.w() > 0);
        CHECK(std::abs(i.w() - L * px_m) <= 3.0);
        // tutto entra nel calibro: alette ed eliche comprese (la MOSS da 10" col suo manicotto da 533 mm)
        const double D = std::max(2.0, (id == "moss_mk70" ? 533.0 : a.diametro_mm) / 1000.0 * px_m);
        CHECK(i.h() <= D + 2.0);
        CHECK(i.h() >= D - 2.0);
        CHECK(i.w() > i.h());   // di fianco, sdraiata
    }
}

TEST_CASE("sagome delle armi: prua a destra") {
    const auto& cat = catalogo();
    const double px_m = 30.0;
    // Mk 48: ogiva nera a prua, propulsore intubato a poppa
    for (const char* id : {"mk48", "mk48_adcap"}) {
        CAPTURE(id);
        const Impronta i = impronta(sagoma_da_scheda(*cat.arma(id)), px_m);
        CHECK(i.conta(MaterialeArma::nero, 0.9, 1.0) > 20);
        CHECK(i.conta(MaterialeArma::grigio, 0.0, 0.06) > 0);
        CHECK(i.conta(MaterialeArma::grigio, 0.9, 1.0) == 0);
    }
    // siluri a eliche e Mk 67: le eliche in bronzo a poppa, nessuna a prua
    for (const char* id : {"set65", "type53_65k", "mk67"}) {
        CAPTURE(id);
        const Impronta i = impronta(sagoma_da_scheda(*cat.arma(id)), px_m);
        CHECK(i.conta(MaterialeArma::bronzo, 0.0, 0.05) > 0);
        CHECK(i.conta(MaterialeArma::bronzo, 0.5, 1.0) == 0);
    }
    // Mk 67: la sezione mina con la fascia gialla davanti
    {
        const Impronta i = impronta(sagoma_da_scheda(*cat.arma("mk67")), px_m);
        CHECK(i.conta(MaterialeArma::giallo, 0.7, 0.9) > 0);
        CHECK(i.conta(MaterialeArma::oliva, 0.75, 1.0) > i.conta(MaterialeArma::oliva, 0.0, 0.6));
    }
    // CAPTOR: calotta dorata a prua (da li' esce il Mk 46), ancora a poppa
    {
        const Impronta i = impronta(sagoma_da_scheda(*cat.arma("mk60")), px_m);
        CHECK(i.conta(MaterialeArma::bronzo, 0.92, 1.0) > 0);
        CHECK(i.conta(MaterialeArma::bronzo, 0.10, 0.25) == 0);
        CHECK(i.conta(MaterialeArma::verde, 0.3, 0.9) > 0);   // il Mk 46 nella finestra
    }
}

TEST_CASE("sagome delle armi: colori d'esercitazione") {
    const auto& cat = catalogo();
    const Impronta mk48 = impronta(sagoma_da_scheda(*cat.arma("mk48"), true), 20.0);
    CHECK(mk48.conta(MaterialeArma::arancione, 0.1, 0.6) > 0);
    CHECK(impronta(sagoma_da_scheda(*cat.arma("mk48")), 20.0).conta(MaterialeArma::arancione, 0.0, 1.0) == 0);
    const Impronta set65 = impronta(sagoma_da_scheda(*cat.arma("set65"), true), 20.0);
    CHECK(set65.conta(MaterialeArma::rosso, 0.8, 1.0) > 0);
    CHECK(set65.conta(MaterialeArma::rosso, 0.0, 0.7) == 0);
}

TEST_CASE("sagome delle armi: piccole restano visibili, la tavolozza fissa non tocca la vista") {
    const auto& cat = catalogo();
    const Impronta i = impronta(sagoma_da_scheda(*cat.arma("mk48")), 3.0);   // spaccato a scala piccola
    CHECK(i.h() >= 2);
    CHECK(std::abs(i.w() - 5.8 * 3.0) <= 3.0);
    const auto& tav = TavolozzaSagome::fissa();
    for (const auto& r : tav.rampe) {
        REQUIRE(!r.empty());
        for (const int k : r)
            for (const auto& [a, b] : INDICI_VISTA) {
                CAPTURE(k);
                CHECK_FALSE((k >= a && k <= b));
            }
    }
    Palette p;
    const auto ricca = TavolozzaSagome::ricca(p, 96);
    int n = 0;
    for (const auto& r : ricca.rampe)
        for (const int k : r) {
            CHECK((k >= 96 && k < 144));
            ++n;
        }
    CHECK(n == 48);
}

TEST_CASE("tubo lanciasiluri: la ricarica fa scorrere l'arma dentro, il disegno resta nel riquadro") {
    const auto& cat = catalogo();
    const Rettangolo r{40, 30, 300, 80};
    SagomaInTubo s;
    s.arma = sagoma_da_scheda(*cat.arma("mk48"));
    s.stato = FaseTubo::ricarica;
    const double L = lunghezza_disegnata(s.arma);
    s.ricarica = 0.0;
    GeometriaTubo g = geometria_tubo(r, s);
    CHECK(g.x_volata > g.x_culatta);
    CHECK(g.x_poppa_arma + L * g.px_m <= g.x_culatta + 0.5);   // ancora tutta sulla rastrelliera
    s.ricarica = 0.5;
    const double meta = geometria_tubo(r, s).x_poppa_arma;
    s.ricarica = 1.0;
    g = geometria_tubo(r, s);
    CHECK(meta > geometria_tubo(r, {s.arma, FaseTubo::ricarica, 0.0}).x_poppa_arma);
    CHECK(g.x_poppa_arma >= g.x_culatta);
    CHECK(g.x_poppa_arma + L * g.px_m <= g.x_volata);         // dentro, porta esterna libera
    s.stato = FaseTubo::carico;
    s.ricarica = 0.0;   // ignorata fuori dalla ricarica
    CHECK(geometria_tubo(r, s).x_poppa_arma == doctest::Approx(g.x_poppa_arma));
    // anche le armi sovietiche lunghe e il PMR-1 entrano nel tubo
    for (const char* id : {"uset80", "pmr1", "shkval"}) {
        CAPTURE(id);
        SagomaInTubo q;
        q.arma = sagoma_da_scheda(*cat.arma(id));
        q.stato = FaseTubo::carico;
        const auto gq = geometria_tubo(r, q);
        CHECK(gq.x_poppa_arma + lunghezza_disegnata(q.arma) * gq.px_m <= gq.x_volata);
    }
    // niente fuori dal riquadro, in ogni stato
    for (const auto st : {FaseTubo::vuoto, FaseTubo::ricarica, FaseTubo::carico, FaseTubo::armamento, FaseTubo::armato}) {
        Tela t(400, 160);
        t.riempi(255);
        SagomaInTubo q = s;
        q.stato = st;
        q.ricarica = 0.3;
        q.allagato = q.porta_aperta = st == FaseTubo::armato;
        disegna_tubo(t, r, q);
        int fuori = 0, dentro = 0;
        for (int y = 0; y < t.altezza(); ++y)
            for (int x = 0; x < t.larghezza(); ++x) {
                const bool in = x >= r.x && y >= r.y && x < r.x + r.w && y < r.y + r.h;
                if (!in && t.at(x, y) != 255) ++fuori;
                if (in && t.at(x, y) != 255) ++dentro;
            }
        CHECK(fuori == 0);
        CHECK(dentro == r.w * r.h);
    }
}

TEST_CASE("tubo lanciasiluri: la mina armata ha il segno della posa") {
    const auto& cat = catalogo();
    const Rettangolo r{0, 0, 300, 80};
    auto disegna = [&](const char* id) {
        Tela t(300, 80);
        SagomaInTubo s;
        s.arma = sagoma_da_scheda(*cat.arma(id));
        s.stato = FaseTubo::armato;
        disegna_tubo(t, r, s);
        return t;
    };
    const Tela mina = disegna("mk67"), siluro = disegna("mk48");
    int diversi = 0;
    for (int y = 0; y < 14; ++y)
        for (int x = 8; x < 30; ++x) diversi += mina.at(x, y) != siluro.at(x, y);
    CHECK(diversi > 4);
}

TEST_CASE("sagome delle armi: le esche semoventi da tubo (MG-74, MOSS Mk 70)") {
    const auto& cat = catalogo();
    if (!cat.arma("mg74") || !cat.arma("moss_mk70")) return;
    const double px_m = 40.0;
    // MG-74: siluro elettrico corto da 533 mm, 3,9 m, con la fascia rossa e le eliche controrotanti a poppa
    const Impronta mg = impronta(sagoma_da_scheda(*cat.arma("mg74")), px_m);
    CHECK(std::abs(mg.w() - 3.9 * px_m) <= 3.0);
    CHECK(std::abs(mg.h() - 0.533 * px_m) <= 2.0);
    CHECK(mg.conta(MaterialeArma::rosso, 0.4, 0.8) > 20);
    CHECK(mg.conta(MaterialeArma::bronzo, 0.0, 0.05) > 0);
    CHECK(mg.conta(MaterialeArma::nero, 0.92, 1.0) > 0);
    // MOSS: il corpo del Mk 37 da 10" dentro il manicotto da 533 mm (guide e anelli), 4,0 m
    const Impronta moss = impronta(sagoma_da_scheda(*cat.arma("moss_mk70")), px_m);
    CHECK(std::abs(moss.w() - 4.0 * px_m) <= 3.0);
    CHECK(std::abs(moss.h() - 0.533 * px_m) <= 2.0);
    CHECK(moss.conta(MaterialeArma::giallo, 0.0, 1.0) > 0);   // anelli del manicotto
    // a mezza lunghezza il corpo e' meno della meta' del manicotto: grigio pieno solo vicino all'asse
    const int x_mezzo = (moss.xa + moss.xb) / 2;
    const auto& colonna = moss.per_colonna.at(x_mezzo);
    int pieni = 0;
    for (const auto& [m, n] : colonna) pieni += n;
    CHECK(pieni < 0.75 * 0.533 * px_m);
    // nel tubo e nei riquadri piccoli restano dentro il riquadro
    for (const char* id : {"mg74", "moss_mk70"}) {
        Tela t;
        t.riempi(0);
        SagomaInTubo q;
        q.arma = sagoma_da_scheda(*cat.arma(id));
        q.stato = FaseTubo::armato;
        q.allagato = q.porta_aperta = true;
        const Rettangolo r{100, 100, 82, 21};
        disegna_tubo(t, r, q);
        int fuori = 0;
        for (int y = 0; y < ALTEZZA_VISTA; ++y)
            for (int x = 0; x < LARGHEZZA_TELA; ++x)
                if (t.at(x, y) != 0 && (x < r.x || y < r.y || x >= r.x + r.w || y >= r.y + r.h)) ++fuori;
        CHECK(fuori == 0);
    }
}

TEST_CASE("sagome delle armi: i siluri senza voce nel carico e le classi delle missioni (sagoma_da_id)") {
    // le misure dalla ricerca del kit (grafica/sagome_armi.hpp): lunghezza e diametro
    struct Caso { const char* id; double l, d; };
    for (const Caso& c : {Caso{"tigerfish", 6.46, 533}, Caso{"spearfish", 6.1, 533}, Caso{"mk46", 2.59, 324}, Caso{"at1", 4.0, 450},
                          Caso{"at2", 5.0, 533}, Caso{"umgt1", 3.6, 400}, Caso{"apr2", 3.7, 350}, Caso{"type40", 4.5, 400},
                          Caso{"type53", 7.2, 533}, Caso{"mk48", 5.8, 533}, Caso{"biologico", 21.0, 2600}}) {
        CAPTURE(c.id);
        CHECK(sagoma_nota(c.id));
        const SagomaArma s = sagoma_da_id(c.id);
        CHECK(lunghezza_disegnata(s) == doctest::Approx(c.l));
        CHECK(s.diametro_mm == doctest::Approx(c.d));
        const Impronta i = impronta(s, 20.0);
        CHECK(i.w() == doctest::Approx(c.l * 20.0).epsilon(0.05));
        CHECK(i.h() <= static_cast<int>(c.d / 1000.0 * 20.0 * (c.d > 1000 ? 1.4 : 1.1)) + 4);   // la balena con la dorsale
    }
    CHECK_FALSE(sagoma_nota("arma_inventata"));
    CHECK(sagoma_da_id("sea_lance").tipo == TipoArmaCarico::missile);
    CHECK(sagoma_da_id("type40").sovietica);
    // d'esercitazione: teste arancioni per gli occidentali, rosse per i sovietici
    CHECK(impronta(sagoma_da_id("mk46", true), 20.0).conta(MaterialeArma::arancione, 0.6, 1.0) > 0);
    CHECK(impronta(sagoma_da_id("umgt1", true), 20.0).conta(MaterialeArma::rosso, 0.75, 1.0) > 0);
    CHECK(impronta(sagoma_da_id("umgt1"), 20.0).conta(MaterialeArma::rosso, 0.0, 1.0) == 0);
    // la balena: ventre bianco, dorso grigio, niente colori d'arma
    const Impronta b = impronta(sagoma_da_id("biologico"), 10.0);
    CHECK(b.conta(MaterialeArma::bianco, 0.3, 0.8) > 0);
    CHECK(b.conta(MaterialeArma::grigio, 0.3, 0.8) > 0);
    CHECK(b.conta(MaterialeArma::verde, 0.0, 1.0) == 0);
}
