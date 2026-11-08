#include "grafica/tema_stazioni.hpp"

#include <algorithm>
#include <cmath>

namespace m688 {

namespace {

struct Rgb {
    double r, g, b;
};

Rgb mescola(Rgb a, Rgb b, double t) { return {a.r + (b.r - a.r) * t, a.g + (b.g - a.g) * t, a.b + (b.b - a.b) * t}; }

// colore lungo una serie di punti uguali da 0 a 1
template <std::size_t N>
Rgb lungo(const Rgb (&p)[N], double t) {
    const double x = std::clamp(t, 0.0, 1.0) * static_cast<double>(N - 1);
    const auto i = std::min(static_cast<std::size_t>(x), N - 2);
    return mescola(p[i], p[i + 1], x - static_cast<double>(i));
}

// La luce della sala sulle superfici (non sulle lampade ne' sui fosfori): rossa di notte, la luminanza in un rosso
// scuro; blu nella sala sonar, attenuata e virata al blu (STIMA dalle foto della sala sonar del 688, dossier 19 §2.3)
Rgb luce(Rgb c, LuceSala l) {
    switch (l) {
        case LuceSala::bianca: return c;
        case LuceSala::rossa: {
            const double y = 0.30 * c.r + 0.59 * c.g + 0.11 * c.b;
            return {y * 0.95, y * 0.12, y * 0.08};
        }
        case LuceSala::blu: return {c.r * 0.40, c.g * 0.52, std::min(255.0, c.b * 0.80 + 30.0)};
    }
    return c;
}

void metti(Palette& p, int i, Rgb c) {
    const auto v = [](double x) { return static_cast<std::uint8_t>(std::clamp(std::lround(x), 0L, 255L)); };
    p.imposta(i, v(c.r), v(c.g), v(c.b));
}

// smalto del 688: dal nero dei bordi al grigio chiaro degli apparati (FS 26307, circa 135-140) e ai riflessi
constexpr Rgb SMALTO_688[] = {{10, 11, 12}, {38, 41, 43}, {78, 82, 84}, {135, 140, 140}, {196, 200, 200}};
// smalto dell'Alfa: blu-grigio del pavimento, marrone dei cuscini, crema, avorio (dossier 19 §3.5, STIMA RGB)
constexpr Rgb SMALTO_ALFA[] = {{34, 38, 48}, {60, 70, 85}, {104, 74, 52}, {176, 160, 126}, {225, 215, 180}, {244, 238, 214}};
constexpr Rgb ACCENTO_688[] = {{46, 58, 66}, {92, 112, 124}};    // cristalli grigio-azzurri dei CRT
constexpr Rgb ACCENTO_ALFA[] = {{14, 12, 10}, {46, 40, 34}};     // bachelite nera, col suo riflesso
// lampade EGA: accesa e spenta (spenta: la lente colorata senza luce)
constexpr Rgb LUCI_LAMPADE[3][2] = {{{255, 48, 36}, {92, 22, 18}}, {{255, 178, 24}, {100, 66, 10}}, {{56, 232, 72}, {16, 74, 26}}};
// fosfori: vetro spento, poi alone, tratto, nucleo (P1 verde; verde giallino; ambra P3)
constexpr Rgb FOSFORO_VERDE[] = {{6, 12, 8}, {10, 40, 16}, {24, 110, 40}, {60, 200, 80}, {170, 255, 170}};
constexpr Rgb FOSFORO_GIALLINO[] = {{10, 12, 4}, {34, 44, 8}, {100, 130, 20}, {180, 225, 50}, {236, 255, 150}};
constexpr Rgb FOSFORO_AMBRA[] = {{12, 8, 2}, {50, 28, 4}, {150, 86, 10}, {240, 150, 24}, {255, 220, 140}};

}  // namespace

void applica_tema(Palette& p, const TemaStazione& t) {
    using namespace colori_tema;
    const bool alfa = t.battello == BattelloTema::alfa;
    for (int i = 0; i < N_SMALTO; ++i) {
        const double x = static_cast<double>(i) / (N_SMALTO - 1);
        metti(p, SMALTO + i, luce(alfa ? lungo(SMALTO_ALFA, x) : lungo(SMALTO_688, x), t.luce));
    }
    for (int i = 0; i < N_ACCENTO; ++i) metti(p, ACCENTO + i, luce(alfa ? ACCENTO_ALFA[i] : ACCENTO_688[i], t.luce));
    for (int c = 0; c < 3; ++c)
        for (int s = 0; s < 2; ++s) metti(p, LAMPADE + 2 * c + s, LUCI_LAMPADE[c][s]);
    // targhette: 688 bianco su nero (tipo I), Alfa bianco su rosso; il mezzo tono e' la scritta sul fondo a meta'
    const Rgb fondo = alfa ? Rgb{168, 30, 24} : Rgb{22, 22, 24}, scritta = alfa ? Rgb{242, 236, 224} : Rgb{232, 232, 226};
    metti(p, TARGHETTA, luce(fondo, t.luce));
    metti(p, TARGHETTA + 1, luce(mescola(fondo, {0, 0, 0}, 0.45), t.luce));
    metti(p, TARGHETTA + 2, luce(scritta, t.luce));
    metti(p, TARGHETTA + 3, luce(mescola(fondo, scritta, 0.5), t.luce));
    const Fosforo f = t.fosforo != Fosforo::del_battello ? t.fosforo : alfa ? Fosforo::verde_giallino : Fosforo::verde;
    for (int i = 0; i < N_FOSFORO; ++i) {
        const double x = std::pow(static_cast<double>(i) / (N_FOSFORO - 1), 0.85);
        metti(p, FOSFORO + i, f == Fosforo::ambra ? lungo(FOSFORO_AMBRA, x) : f == Fosforo::verde_giallino ? lungo(FOSFORO_GIALLINO, x)
                                                                                               : lungo(FOSFORO_VERDE, x));
    }
}

}  // namespace m688
