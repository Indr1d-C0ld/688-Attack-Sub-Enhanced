// I grafici della schermata delle serie (grafica/grafici_serie.hpp): riassunto, numeri, e che il disegno stia nei
// rettangoli.
#include <doctest.h>

#include <cmath>
#include <filesystem>
#include <vector>

#include "grafica/font.hpp"
#include "grafica/grafici_serie.hpp"
#include "grafica/tela.hpp"

#ifndef M688_DATI
#define M688_DATI "../data/motore"
#endif

using namespace m688;
namespace g = m688::grafici;

namespace {

// vero se niente e' stato disegnato fuori da r
bool tutto_dentro(const Tela& t, Rettangolo r) {
    for (int y = 0; y < t.altezza(); ++y)
        for (int x = 0; x < t.larghezza(); ++x)
            if (t.at(x, y) && (x < r.x || y < r.y || x >= r.x + r.w || y >= r.y + r.h)) return false;
    return true;
}

}  // namespace

TEST_CASE("grafici delle serie: il riassunto") {
    const std::vector<double> v = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, std::nan("")};
    const g::Riassunto s = g::riassumi(v);
    CHECK(s.n == 10);
    CHECK(s.media == doctest::Approx(5.5));
    CHECK(s.dev == doctest::Approx(3.02765).epsilon(1e-4));
    CHECK(s.min == 1.0);
    CHECK(s.max == 10.0);
    CHECK(s.mediana == doctest::Approx(5.5));
    CHECK(s.p10 == doctest::Approx(1.9));
    CHECK(s.p90 == doctest::Approx(9.1));
    CHECK(g::riassumi(std::vector<double>{}).n == 0);
    CHECK(g::riassumi(std::vector<double>{4.0}).dev == 0.0);
}

TEST_CASE("grafici delle serie: i numeri all'italiana") {
    CHECK(g::numero(3.0) == "3");
    CHECK(g::numero(1.4) == "1,4");
    CHECK(g::numero(0.25) == "0,25");
    CHECK(g::numero(12.34) == "12,3");
    CHECK(g::numero(1234.6, "m") == "1235 m");
    CHECK(g::numero(-0.0) == "0");
}

TEST_CASE("grafici delle serie: i disegni restano nei loro rettangoli" * doctest::skip(!std::filesystem::exists(M688_DATI "/font/originale.fnt"))) {
    const Font f = Font::carica(M688_DATI "/font/originale.fnt");
    std::vector<double> v;
    for (int k = 0; k < 20; ++k) v.push_back(100.0 + 37.0 * ((k * 7) % 11));
    v[3] = std::nan("");
    {
        Tela t(400, 300);
        t.riempi(0);
        const Rettangolo r{50, 40, 300, 200};
        g::OpzioniIstogramma o;
        o.titolo = "RITARDO";
        o.unita = "s";
        g::istogramma(t, f, r, v, o);
        CHECK(tutto_dentro(t, r));
    }
    {
        Tela t(700, 200);
        t.riempi(0);
        const Rettangolo r{10, 10, 616, 100};
        const std::vector<g::VoceEsito> e = {{21, "Tempo scaduto", 14}, {2, "Battello affondato", 6}};
        g::barre_esiti(t, f, r, e, 11, 7, 3);
        CHECK(tutto_dentro(t, r));
        // un codice negativo (la partita non finita) non si scrive: il testo comincia al bordo come quello col codice
        Tela a(700, 40), b(700, 40);
        a.riempi(0), b.riempi(0);
        const std::vector<g::VoceEsito> col = {{-1, "NON FINITA", 3}}, senza = {{0, "NON FINITA", 3}};
        g::barre_esiti(a, f, r, col, 11, 7, 3);
        g::barre_esiti(b, f, r, senza, 11, 7, 3);
        int pa = 0, pb = 0;   // i pixel del testo nella colonna dell'etichetta
        for (int y = 0; y < 40; ++y)
            for (int x = r.x; x < r.x + 200; ++x) pa += a.at(x, y) == 7, pb += b.at(x, y) == 7;
        CHECK(pa > 0);
        CHECK(pa < pb);   // senza il «-1 » davanti ci sono meno pixel che con lo «0 »
    }
    {
        Tela t(400, 40);
        t.riempi(0);
        const Rettangolo r{20, 10, 250, 10};
        g::riga_dispersione(t, f, r, g::riassumi(v), "s", 11, 15);
        CHECK(tutto_dentro(t, r));
    }
    {
        Tela t(700, 200);
        t.riempi(0);
        const Rettangolo r{10, 20, 616, 100};
        const auto zone = g::barre_partite(t, r, v, 11, 5);
        CHECK(zone.size() == v.size());
        for (std::size_t i = 1; i < zone.size(); ++i) CHECK(zone[i].x >= zone[i - 1].x + zone[i - 1].w);   // in ordine, senza sovrapporsi
        CHECK(zone.back().x + zone.back().w <= r.x + r.w);
        // la scelta e' bordata di bianco appena fuori dalla barra: resta dentro r salvo la colonna di sinistra della prima
        CHECK(tutto_dentro(t, {r.x - 1, r.y - 1, r.w + 2, r.h + 1}));
    }
}
