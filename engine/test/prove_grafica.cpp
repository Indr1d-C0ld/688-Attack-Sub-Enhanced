#include <doctest.h>

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <utility>

#include "grafica/font.hpp"
#include "grafica/modello.hpp"
#include "grafica/palette.hpp"
#include "grafica/periscopio.hpp"
#include "grafica/sprite.hpp"
#include "grafica/tela.hpp"
#include "nucleo/classi.hpp"

using namespace m688;
using doctest::Approx;

namespace {
Sprite sprite_di_prova() {   // 4x2: riga alta trasparente ai bordi, ancora al centro del bordo inferiore
    Sprite s;
    s.w = 4; s.h = 2; s.ancora_x = 2.0f; s.ancora_y = 2.0f;
    s.pix = {TRASPARENTE, 150, 151, TRASPARENTE, 152, 152, 152, 152};
    return s;
}
}  // namespace

TEST_CASE("sprite: ancora, scala e trasparenza") {
    Tela t(20, 20);
    t.riempi(1);
    t.disegna_sprite(sprite_di_prova(), 10.0, 10.0, 1.0);
    CHECK(t.at(8, 9) == 152);              // riga bassa da x = 8 a 11, sopra la riga dell'ancora
    CHECK(t.at(11, 9) == 152);
    CHECK(t.at(8, 8) == 1);                // trasparente: resta lo sfondo
    CHECK(t.at(9, 8) == 150);
    CHECK(t.at(10, 10) == 1);              // niente sotto il galleggiamento
    Tela g(20, 20);
    g.riempi(1);
    g.disegna_sprite(sprite_di_prova(), 10.0, 10.0, 2.0);   // doppio: 8x4 pixel da x = 6, y = 6
    CHECK(g.at(6, 9) == 152);
    CHECK(g.at(13, 8) == 152);
    CHECK(g.at(8, 6) == 150);
    CHECK(g.at(5, 9) == 1);
}

TEST_CASE("sprite: limite inferiore (nave oltre l'orizzonte)") {
    Tela t(20, 20);
    t.riempi(1);
    t.disegna_sprite(sprite_di_prova(), 10.0, 10.0, 1.0, 9);   // la riga 9 e' nascosta dal mare
    CHECK(t.at(9, 8) == 150);
    CHECK(t.at(9, 9) == 1);
}

TEST_CASE("ritaglio della tela") {
    Tela t(10, 10);
    t.riempi(0);
    t.imposta_ritaglio({2, 2, 4, 4});
    t.rettangolo_pieno(0, 0, 10, 10, 5);
    CHECK(t.at(1, 1) == 0);
    CHECK(t.at(2, 2) == 5);
    CHECK(t.at(5, 5) == 5);
    CHECK(t.at(6, 6) == 0);
}

TEST_CASE("UTF-8") {
    const auto cp = Font::decodifica_utf8("Città°");
    REQUIRE(cp.size() == 6);
    CHECK(cp[4] == U'à');
    CHECK(cp[5] == U'°');
    CHECK(Font::decodifica_utf8("\xff").at(0) == U'?');
}

TEST_CASE("geometria del periscopio") {
    CHECK(Periscopio::pixel_per_radiante(false) * radianti(CAMPO_BASSO_GRADI) == Approx(640.0));
    VistaPeriscopio v;
    CHECK(Periscopio::riga_orizzonte(v) > ALTEZZA_VISTA / 2.0);   // l'orizzonte sta sotto l'orizzontale
}

// Prove sui dati esportati dalla pipeline (saltate se data/motore non e' stato generato)
TEST_CASE("dati esportati" * doctest::skip(!std::filesystem::exists(M688_DATI "/classi.toml"))) {
    const std::string d = M688_DATI;
    const auto pal = Palette::carica(d + "/palette/giorno.pal");
    CHECK(pal.rgb(15)[0] == 255);
    const auto f = Font::carica(d + "/font/originale.fnt");
    CHECK(f.ha(U'à'));
    CHECK(f.ha(U'°'));
    CHECK(f.larghezza("A", 1, 1, false) == 5);   // convenzione dell'originale: maiuscola piccola
    CHECK(f.larghezza("a", 1, 1, false) == 7);   // maiuscola grande nello slot della minuscola
    CHECK(f.larghezza("A", 1) == 7);             // motore: ogni lettera in maiuscola grande
    CHECK(f.larghezza("Città", 1) == f.larghezza("città", 1));
    const auto classi = carica_classi(d + "/classi.toml");
    REQUIRE(classi.count("perry") == 1);
    const auto s = InsiemeSprite::carica(d + "/" + classi.at("perry").sprite);
    CHECK(s.numero() == 74);
    CHECK(s.scala_px_m() == Approx(3.75));
    const auto& fianco = s.per_angolo(90.0, true);
    CHECK(fianco.aob == 90);
    // di fianco la Perry (135,6 m) e' larga circa 135,6 * 3,75 pixel e l'ancora sta a meta'
    CHECK(fianco.w == Approx(135.6 * 3.75).epsilon(0.03));
    CHECK(fianco.ancora_x == Approx(fianco.w / 2.0).epsilon(0.05));
    CHECK(fianco.ancora_y == Approx(fianco.h).epsilon(0.02));
}

// Sprite generati dal modello 3D (docs/10_danni_affondamento_esplosioni.md §5.1): a nave intatta devono coincidere
// con quelli precalcolati, cosi' il passaggio dall'uno all'altro non si vede.
TEST_CASE("modello: sprite generato contro sprite precalcolato" *
          doctest::skip(!std::filesystem::exists(M688_DATI "/modelli/perry.mdl"))) {
    const std::string d = M688_DATI;
    const auto m = Modello::carica(d + "/modelli/perry.mdl");
    const auto spr = InsiemeSprite::carica(d + "/navi/perry.spr");
    const auto rampa = rampa_grigi(Palette::carica(d + "/palette/giorno.pal"));
    for (const auto& [aob, dritta] : {std::pair{90, true}, std::pair{30, false}, std::pair{150, true}}) {
        const Sprite g = genera_sprite(m, PosaNave{}, aob, dritta, rampa);
        const Sprite& r = spr.per_angolo(aob, dritta);
        INFO("aob " << aob << (dritta ? " dritta" : " sinistra"));
        REQUIRE(g.w == r.w);
        REQUIRE(g.h == r.h);
        CHECK(g.ancora_x == Approx(r.ancora_x).epsilon(0.001));
        CHECK(std::fabs(g.ancora_y - r.ancora_y) <= 1.0f);   // proiezione del galleggiamento contro ultima riga
        int entrambi = 0, entro1 = 0, sagoma = 0, unione = 0;
        double scarto = 0;
        for (std::size_t i = 0; i < g.pix.size(); ++i) {
            const bool a = g.pix[i] != TRASPARENTE, b = r.pix[i] != TRASPARENTE;
            sagoma += a && b;
            unione += a || b;
            if (!(a && b)) continue;
            const int dd = std::abs(int{g.pix[i]} - int{r.pix[i]});
            ++entrambi; scarto += dd; entro1 += dd <= 1;
        }
        CHECK(static_cast<double>(sagoma) / unione > 0.995);
        CHECK(scarto / entrambi < 0.35);
        CHECK(static_cast<double>(entro1) / entrambi > 0.96);
    }
}

TEST_CASE("modello: posa, affondamento e rottura" * doctest::skip(!std::filesystem::exists(M688_DATI "/modelli/perry.mdl"))) {
    const std::string d = M688_DATI;
    const auto m = Modello::carica(d + "/modelli/perry.mdl");
    const auto rampa = rampa_grigi(Palette::carica(d + "/palette/giorno.pal"));
    auto pixel = [](const Sprite& s) { return std::count_if(s.pix.begin(), s.pix.end(), [](auto c) { return c != TRASPARENTE; }); };
    const Sprite intatta = genera_sprite(m, PosaNave{}, 90, true, rampa);
    PosaNave p;
    p.danneggiata = true;
    p.prodiero.immersione = 3.0;                    // piu' bassa di 3 m: meno nave sopra il mare
    const Sprite bassa = genera_sprite(m, p, 90, true, rampa);
    CHECK(pixel(bassa) < pixel(intatta));
    CHECK(bassa.h < intatta.h);
    p.prodiero = {};
    p.prodiero.assetto = 30.0;                      // prua alta di 30 gradi: la prua sale a ~42 m, la poppa e' sotto
    const Sprite alta = genera_sprite(m, p, 90, true, rampa);
    CHECK(alta.h / 3.75 == Approx(42.0).epsilon(0.05));   // 42 m (prua) contro i 35,8 m dell'albero
    CHECK(alta.w < intatta.w * 0.7);                // la poppa e' sott'acqua, la prua scorciata (cos 30 gradi)
    p.prodiero = {};
    p.spezzata = true;                              // spezzata al centro, la poppa scende di 2 m e si separa
    p.x_rottura = 0.0;
    p.poppiero.immersione = 2.0;
    p.poppiero.dx = -20.0;
    const Sprite rotta = genera_sprite(m, p, 90, true, rampa);
    CHECK(rotta.w > intatta.w);
    // colonne vuote fra i due tronconi
    int vuote = 0;
    for (int x = 0; x < rotta.w; ++x) {
        bool c = false;
        for (int y = 0; y < rotta.h; ++y) c |= rotta.pix[static_cast<std::size_t>(y * rotta.w + x)] != TRASPARENTE;
        vuote += !c;
    }
    CHECK(vuote >= 40);
}
