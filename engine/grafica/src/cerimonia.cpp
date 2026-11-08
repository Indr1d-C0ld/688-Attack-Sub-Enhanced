#include "grafica/cerimonia.hpp"

#include <algorithm>
#include <cmath>
#include <vector>

namespace m688::cerimonia {

namespace {

using C = std::array<std::uint8_t, 3>;

void rampa(Palette& p, int primo, const std::vector<C>& c) {
    for (std::size_t i = 0; i < c.size(); ++i) p.imposta(primo + static_cast<int>(i), c[i][0], c[i][1], c[i][2]);
}

constexpr int BAYER[16] = {0, 8, 2, 10, 12, 4, 14, 6, 3, 11, 1, 9, 15, 7, 13, 5};

}  // namespace

void imposta_palette(Palette& p, Marina m) {
    imposta_capelli(p, {70, 45, 30}, 0.0);
    imposta_volto(p, 0.2, {90, 120, 150});
    rampa(p, BLU, {{6, 8, 16}, {10, 13, 26}, {14, 19, 38}, {20, 27, 52}, {28, 37, 68}, {38, 50, 88}, {52, 66, 110}, {74, 90, 136}});
    rampa(p, NERO, {{5, 5, 8}, {10, 10, 14}, {16, 16, 22}, {23, 23, 31}, {32, 32, 42}, {44, 44, 56}, {60, 60, 74}, {82, 82, 98}});
    rampa(p, BIANCO, {{150, 152, 156}, {184, 186, 190}, {212, 214, 216}, {234, 235, 236}, {252, 252, 250}});
    rampa(p, ORO, {{60, 36, 8}, {96, 62, 14}, {134, 92, 22}, {172, 124, 34}, {204, 156, 50}, {228, 186, 74}, {244, 214, 112}, {255, 240, 170}});
    rampa(p, ARGENTO, {{56, 60, 66}, {92, 96, 104}, {130, 134, 142}, {168, 172, 178}, {204, 208, 212}, {240, 242, 244}});
    rampa(p, BRONZO, {{52, 30, 14}, {86, 52, 24}, {122, 78, 38}, {158, 106, 56}, {192, 138, 80}, {226, 176, 116}});
    rampa(p, SMALTO, {{70, 4, 4}, {110, 8, 8}, {152, 14, 12}, {192, 24, 20}, {226, 48, 40}, {250, 110, 96}});
    p.imposta(OCCHI, 64, 82, 98);
    for (int i = 0; i < N_NASTRO; ++i) p.imposta(NASTRO + i, 128, 128, 128);   // poi i colori della carriera
    p.imposta(NERO_N, 16, 16, 16);
    p.imposta(BIANCO_N, 240, 240, 240);
    rampa(p, SFONDO, {{20, 26, 36}, {28, 36, 50}, {36, 46, 64}, {46, 58, 80}, {58, 72, 98}, {72, 88, 116}, {88, 106, 136}, {108, 128, 158}});
    rampa(p, KAKI, {{52, 44, 24}, {78, 66, 38}, {104, 90, 54}, {130, 114, 72}, {154, 138, 92}, {178, 162, 114}, {200, 186, 140}, {222, 210, 170}});
    rampa(p, CARTA, {{120, 100, 60}, {156, 134, 84}, {186, 164, 110}, {210, 190, 138}, {228, 212, 166}, {242, 232, 198}});
    rampa(p, TIMBRO, {{110, 14, 18}, {156, 26, 30}, {196, 40, 44}, {226, 80, 80}});
    rampa(p, SMALTO_BLU, {{8, 16, 60}, {14, 28, 100}, {24, 46, 140}, {40, 70, 180}, {70, 110, 214}, {130, 170, 240}});
    if (m == Marina::usn) {
        rampa(p, VELLUTO, {{4, 6, 22}, {8, 12, 38}, {12, 18, 54}, {18, 26, 72}, {26, 36, 92}, {36, 48, 112}, {50, 64, 134}, {70, 86, 158}});
        rampa(p, BANDIERA, {{6, 8, 20}, {10, 14, 34}, {16, 22, 50}, {22, 30, 66}, {30, 40, 84}, {40, 52, 102}, {54, 68, 122}, {72, 88, 144}});
    } else {
        rampa(p, VELLUTO, {{30, 2, 6}, {50, 4, 10}, {72, 8, 14}, {96, 12, 20}, {120, 18, 26}, {144, 28, 34}, {166, 42, 46}, {188, 64, 64}});
        rampa(p, BANDIERA, {{40, 4, 6}, {62, 6, 10}, {86, 10, 14}, {110, 14, 18}, {136, 20, 24}, {160, 30, 32}, {184, 46, 44}, {206, 70, 64}});
    }
    rampa(p, LEGNO, {{40, 20, 10}, {62, 34, 16}, {86, 50, 24}, {112, 68, 34}, {138, 88, 46}, {162, 110, 62}, {186, 134, 82}, {208, 160, 108}});
    rampa(p, VIOLA, {{40, 10, 50}, {64, 18, 80}, {90, 28, 110}, {118, 42, 140}, {148, 64, 170}, {184, 108, 204}});
    rampa(p, VERDE, {{10, 40, 16}, {18, 64, 26}, {28, 90, 38}, {42, 118, 54}, {64, 148, 74}, {100, 180, 106}});
}

void imposta_volto(Palette& p, double carnagione, std::array<std::uint8_t, 3> occhi) {
    const double k = std::clamp(carnagione, 0.0, 1.0);
    // pelle: dall'ombra calda alla luce, piu' olivastra con k
    for (int i = 0; i < N_PELLE; ++i) {
        const double t = i / (N_PELLE - 1.0);
        const double r = 52 + 196 * std::pow(t, 0.8) - 16 * k * t, g = 30 + 182 * std::pow(t, 0.95) - 8 * k * t,
                     b = 26 + 160 * std::pow(t, 1.12) - 22 * k * t;
        p.imposta(PELLE + i, static_cast<std::uint8_t>(std::clamp(r, 0.0, 255.0)), static_cast<std::uint8_t>(std::clamp(g, 0.0, 255.0)),
                  static_cast<std::uint8_t>(std::clamp(b, 0.0, 255.0)));
    }
    for (int i = 0; i < N_GUANCE; ++i) {
        const double t = (i + 1.5) / (N_GUANCE + 1.5);
        p.imposta(GUANCE + i, static_cast<std::uint8_t>(std::clamp(70 + 180 * t - 10 * k, 0.0, 255.0)),
                  static_cast<std::uint8_t>(std::clamp(26 + 120 * t * t, 0.0, 255.0)), static_cast<std::uint8_t>(std::clamp(22 + 100 * t * t, 0.0, 255.0)));
    }
    rampa(p, LABBRA, {{84, 38, 32}, {124, 62, 52}, {162, 92, 78}, {196, 126, 108}});
    for (int i = 0; i < N_IRIDE; ++i) {
        const double t = 0.35 + 0.65 * i / (N_IRIDE - 1.0);
        p.imposta(IRIDE + i, static_cast<std::uint8_t>(occhi[0] * t), static_cast<std::uint8_t>(occhi[1] * t), static_cast<std::uint8_t>(occhi[2] * t));
    }
}

void imposta_capelli(Palette& p, std::array<std::uint8_t, 3> base, double grigio) {
    const double g = std::clamp(grigio, 0.0, 1.0);
    for (int i = 0; i < N_CAPELLI; ++i) {
        const double k = 0.3 + 1.5 * i / (N_CAPELLI - 1.0);
        std::array<std::uint8_t, 3> c{};
        for (int j = 0; j < 3; ++j) {
            const double v = std::min(255.0, base[static_cast<std::size_t>(j)] * k);
            const double grig = 40 + 150 * i / (N_CAPELLI - 1.0);
            c[static_cast<std::size_t>(j)] = static_cast<std::uint8_t>(std::lround(v * (1 - g) + grig * g));
        }
        p.imposta(CAPELLI + i, c[0], c[1], c[2]);
    }
}

double bayer(int x, int y) { return (BAYER[(y & 3) * 4 + (x & 3)] + 0.5) / 16.0; }

int retina(double t, int x, int y, int massimo) {
    return std::clamp(static_cast<int>(std::floor(t + bayer(x, y))), 0, massimo);
}

double caso(int a, int b, int c) {
    std::uint32_t h = static_cast<std::uint32_t>(a) * 73856093U ^ static_cast<std::uint32_t>(b) * 19349663U ^
                      static_cast<std::uint32_t>(c) * 83492791U;
    h ^= h >> 16;
    h *= 0x7feb352dU;
    h ^= h >> 15;
    h *= 0x846ca68bU;
    h ^= h >> 16;
    return h / 4294967296.0;
}

}  // namespace m688::cerimonia
