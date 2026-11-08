// Immagini del Museo (enciclopedia, roadmap punto 14): cascata dimostrativa della firma (grafica/cascata_firma), ritaglio
// della carta d'epoca (app/carta, disegna_carta_museo), spaccato col sistema evidenziato (grafica/spaccato).
#include <doctest.h>

#include <algorithm>
#include <filesystem>
#include <numeric>

#include "carta.hpp"
#include "grafica/cascata_firma.hpp"
#include "grafica/spaccato.hpp"
#include "nucleo/acustica.hpp"
#include "nucleo/fondale.hpp"

using namespace m688;
namespace fs = std::filesystem;

namespace {
const bool CON_DATI = fs::exists(M688_DATI "/acustica.toml") && fs::exists(M688_DATI "/modelli/los_angeles.mdl");

const CatalogoAcustico& catalogo() {
    static const CatalogoAcustico c = carica_acustica(M688_DATI "/acustica.toml");
    return c;
}

// tutti i pixel fuori da r restano al valore di fondo
bool solo_dentro(const Tela& t, Rettangolo r, std::uint8_t fondo) {
    for (int y = 0; y < ALTEZZA_TELA; ++y)
        for (int x = 0; x < LARGHEZZA_TELA; ++x)
            if ((x < r.x || y < r.y || x >= r.x + r.w || y >= r.y + r.h) && t.at(x, y) != fondo) return false;
    return true;
}
}  // namespace

TEST_CASE("cascata del Museo: sensore usato, velocita' tipica e distanza" * doctest::skip(!CON_DATI)) {
    const auto& c = catalogo();
    const ScenaCascata v = scena_cascata(c, "victor3", 0.0, "bqq5_sfera");
    CHECK(v.sensore == "bqq5_sfera");
    CHECK(v.quota_m == doctest::Approx(60.0));
    CHECK(v.cavitazione);   // appena sopra la cavitazione: si vede anche quella
    CHECK(v.velocita_kn == doctest::Approx(velocita_cavitazione_kn(*c.firma("victor3"), 60.0) + 1.5));
    CHECK(v.velocita_lenta_kn < velocita_cavitazione_kn(*c.firma("victor3"), 60.0));   // la meta' lenta non cava
    CHECK(v.distanza_m >= 500.0);
    CHECK(v.distanza_m <= 40000.0);
    // velocita' data: nessuna cavitazione a 3 nodi, e un battello piu' quieto si sente piu' vicino
    const ScenaCascata lento = scena_cascata(c, "victor3", 3.0, "bqq5_sfera");
    CHECK_FALSE(lento.cavitazione);
    CHECK(lento.distanza_m <= v.distanza_m);
    // navi a 15 nodi in superficie
    const ScenaCascata nave = scena_cascata(c, "perry", 0.0, "sovietico_3gen");
    CHECK(nave.velocita_kn == doctest::Approx(15.0));
    CHECK(nave.quota_m == 0.0);
    // sensori: il passivo corrispondente, o la sfera del BQQ-5
    CHECK(scena_cascata(c, "victor3", 0.0, "vgs2").sensore == "vgs2_passivo");
    CHECK(scena_cascata(c, "victor3", 0.0, "aqs13b").sensore == "aqs13b_passivo");
    CHECK(scena_cascata(c, "victor3", 0.0, "bqq5_attivo").sensore == "bqq5_sfera");
    CHECK(scena_cascata(c, "victor3", 0.0, "").sensore == "bqq5_sfera");
    CHECK(scena_cascata(c, "victor3", 0.0, "inesistente").sensore == "bqq5_sfera");
}

TEST_CASE("cascata del Museo: nel riquadro, nella scala, con la traccia e le righe" * doctest::skip(!CON_DATI)) {
    const auto& c = catalogo();
    const Rettangolo r{100, 50, 236, 128};
    Tela t;
    t.riempi(7);
    disegna_cascata_firma(t, r, c, "victor3", 0.0, 42, "bqq5_sfera");
    CHECK(solo_dentro(t, r, 7));
    for (int y = r.y; y < r.y + r.h; ++y)
        for (int x = r.x; x < r.x + r.w; ++x) {
            const int k = t.at(x, y);
            REQUIRE((k >= CASCATA_BASE && k < CASCATA_BASE + CASCATA_LIVELLI));
        }
    // BTR (i primi 2/5): la colonna piu' luminosa e' la traccia, a destra di 035 nella riga piu' vecchia
    const int bw = r.w * 2 / 5, lw = r.w - bw - 2, xl = r.x + bw + 2;
    int migliore = 0;
    for (int y = r.y + r.h - 1, x = r.x; x < r.x + bw; ++x)
        if (t.at(x, y) > t.at(r.x + migliore, y)) migliore = x - r.x;
    CHECK(360.0 * migliore / bw == doctest::Approx(35.0).epsilon(0.25));
    // LOFAR, meta' lenta (in basso): la colonna piu' luminosa e' la tonale del Victor III a 250 Hz, molto sopra la mediana
    std::vector<double> col(static_cast<std::size_t>(lw));
    for (int x = 0; x < lw; ++x) {
        double sm = 0;
        for (int y = r.y + r.h * 3 / 4; y < r.y + r.h; ++y) sm += t.at(xl + x, y) - CASCATA_BASE;
        col[static_cast<std::size_t>(x)] = sm / (r.h / 4);
    }
    const auto piu = std::max_element(col.begin(), col.end());
    const double hz = 10.0 + 400.0 * (static_cast<double>(piu - col.begin()) + 0.5) / lw;
    CHECK(hz == doctest::Approx(250.0).epsilon(0.02));
    std::vector<double> ord = col;
    std::nth_element(ord.begin(), ord.begin() + lw / 2, ord.end());
    CHECK(*piu > ord[static_cast<std::size_t>(lw / 2)] + 10.0);
    // lo stesso seme, la stessa cascata; un altro seme, un'altra grana
    Tela u, w;
    u.riempi(7);
    w.riempi(7);
    disegna_cascata_firma(u, r, c, "victor3", 0.0, 42, "bqq5_sfera");
    disegna_cascata_firma(w, r, c, "victor3", 0.0, 43, "bqq5_sfera");
    const std::size_t n = static_cast<std::size_t>(LARGHEZZA_TELA) * ALTEZZA_TELA;
    CHECK(std::equal(t.dati(), t.dati() + n, u.dati()));
    CHECK_FALSE(std::equal(t.dati(), t.dati() + n, w.dati()));
    // classe senza firma: il solo fondo nero
    Tela v;
    v.riempi(7);
    disegna_cascata_firma(v, r, c, "nessuna", 0.0, 42);
    CHECK(solo_dentro(v, r, 7));
    CHECK(v.at(r.x + 10, r.y + 10) == CASCATA_BASE);
}

TEST_CASE("cascata del Museo: palette in fosforo verde") {
    Palette p;
    applica_palette_cascata(p);
    CHECK(p.rgb(CASCATA_BASE) == std::array<std::uint8_t, 3>{0, 0, 0});
    CHECK(p.rgb(CASCATA_BASE + CASCATA_LIVELLI - 1) == std::array<std::uint8_t, 3>{190, 255, 180});
    for (int i = 1; i < CASCATA_LIVELLI; ++i) {
        CHECK(p.rgb(CASCATA_BASE + i)[1] >= p.rgb(CASCATA_BASE + i - 1)[1]);   // il verde sale
        CHECK(p.rgb(CASCATA_BASE + i)[1] >= p.rgb(CASCATA_BASE + i)[0]);
    }
}

TEST_CASE("carta del Museo: ritaglio sulla base e teatro intero") {
    // 100 x 80 celle da 1 km: mare profondo a ovest, terra a est
    std::vector<std::uint16_t> val(100 * 80);
    for (int r = 0; r < 80; ++r)
        for (int c = 0; c < 100; ++c) val[static_cast<std::size_t>(r * 100 + c)] = static_cast<std::uint16_t>(c < 60 ? 40000 : 44100);
    Fondale fondo(100, 80, 1000.0, 0.25, -11000.0, val);
    fondo.imposta_centro({40.0, 10.0});
    const Font f = Font::carica(std::string(M688_DATI) + "/font/originale.fnt");
    const Rettangolo r{50, 40, 236, 128};
    Tela t;
    t.riempi(3);
    const Geografiche base{40.0, 10.0};
    disegna_carta_museo(t, f, r, fondo, &base, 60.0);
    CHECK(solo_dentro(t, r, 3));
    // il segno della base al centro: croce rossa, cerchi rosso e bianco
    const int cx = r.x + r.w / 2, cy = r.y + r.h / 2;
    CHECK(t.at(cx, cy) == colori_carta::MATITA + 1);
    int rossi = 0, bianchi = 0;
    for (int y = cy - 8; y <= cy + 8; ++y)
        for (int x = cx - 8; x <= cx + 8; ++x) {
            rossi += t.at(x, y) == colori_carta::MATITA + 1;
            bianchi += t.at(x, y) == colori_carta::MATITA + 5;
        }
    CHECK(rossi > 20);
    CHECK(bianchi > 40);
    // acqua a ovest e terra a est del centro (la costa a 10 km dal centro, 39 pixel con 60 km su 236)
    CHECK(t.at(r.x + 20, cy) >= colori_carta::ACQUA);
    CHECK(t.at(r.x + 20, cy) < colori_carta::ACQUA + 48);
    CHECK(t.at(r.x + r.w - 20, cy) >= colori_carta::TERRA);
    CHECK(t.at(r.x + r.w - 20, cy) < colori_carta::TERRA + 7);
    // teatro intero: tutta la terra a est e il mare a ovest, senza la base
    Tela u;
    u.riempi(3);
    disegna_carta_museo(u, f, r, fondo, nullptr);
    CHECK(solo_dentro(u, r, 3));
    int segni = 0;
    for (int y = r.y; y < r.y + r.h - 10; ++y)
        for (int x = r.x; x < r.x + r.w; ++x) segni += u.at(x, y) == colori_carta::MATITA + 1;
    CHECK(segni == 0);
    CHECK(u.at(r.x + r.w / 2 - 40, r.y + r.h / 2) < colori_carta::ACQUA + 48);
    CHECK(u.at(r.x + r.w / 2 + 40, r.y + r.h / 2) >= colori_carta::TERRA);
}

TEST_CASE("spaccato del Museo: i nomi dei sistemi") {
    CHECK(sistemi_spaccato("sonar_prua") == std::vector<Sistema>{Sistema::sonar_prua});
    CHECK(sistemi_spaccato("calcolatore_sonar") == std::vector<Sistema>{Sistema::calcolatore_sonar});
    CHECK(sistemi_spaccato("reattore") == std::vector<Sistema>{Sistema::propulsione});
    CHECK(sistemi_spaccato("centrale") == std::vector<Sistema>{Sistema::periscopio, Sistema::idroplani});
    CHECK(sistemi_spaccato("filo_galleggiante") == std::vector<Sistema>{Sistema::antenna});
    CHECK(sistemi_spaccato("boa_radio") == std::vector<Sistema>{Sistema::antenna});
    CHECK(sistemi_spaccato("direzione_tiro") == std::vector<Sistema>{Sistema::sonar_tiro});
    CHECK(sistemi_spaccato("ignoto").empty());
}

TEST_CASE("spaccato del Museo: il sistema col contorno rosso" * doctest::skip(!CON_DATI)) {
    const auto modello = Modello::carica(M688_DATI "/modelli/los_angeles.mdl");
    const auto scafo = DatiScafo::carica(M688_DATI "/scafi/los_angeles.toml");
    const Spaccato sp("los_angeles", modello, scafo);
    const Rettangolo r{40, 60, 236, 128};
    const auto rossi = [&](const Tela& t, int xa, int xb) {
        int n = 0;
        for (int y = r.y; y < r.y + r.h; ++y)
            for (int x = std::max(r.x, xa); x < std::min(r.x + r.w, xb); ++x) n += t.at(x, y) >= 235 && t.at(x, y) <= 238;
        return n;
    };
    Tela nessuno, reattore, prua;
    disegna_spaccato_sistema(nessuno, r, sp, "ignoto");
    disegna_spaccato_sistema(reattore, r, sp, "reattore");
    disegna_spaccato_sistema(prua, r, sp, "sonar_prua");
    CHECK(rossi(nessuno, 0, 1 << 20) == 0);
    double x0 = 0, x1 = 0;
    REQUIRE(sp.estensione(1, x0, x1));   // il compartimento del reattore
    CHECK(rossi(reattore, sp.x_tela(r, x0), sp.x_tela(r, x1)) > 10);
    // la sfera a prua: rossi solo nella parte prodiera
    const int meta = r.x + r.w / 2;
    CHECK(rossi(prua, meta, 1 << 20) > 10);
    CHECK(rossi(prua, 0, meta) == 0);
}
