#include "grafica/scala_cascata.hpp"

#include <algorithm>
#include <cmath>

#include "grafica/palette.hpp"

namespace m688::cascate {

namespace {

using Rgb = std::array<std::uint8_t, 3>;
// Punti uguali da 0 a 1, interpolati: viridis, turbo e inferno («caldo») dai loro autori; fosforo P1 e grigio. Sono
// quelli della stazione (app/sonar_enhanced.cpp)
constexpr Rgb FOSFORO[] = {{0, 0, 0}, {6, 40, 8}, {18, 100, 22}, {40, 170, 45}, {80, 230, 80}, {190, 255, 180}};
constexpr Rgb FOSFORO_ALFA[] = {{0, 0, 0}, {26, 36, 6}, {72, 96, 16}, {124, 156, 30}, {178, 210, 52}, {232, 252, 120}};
constexpr Rgb GRIGIO[] = {{0, 0, 0}, {255, 255, 255}};
constexpr Rgb VIRIDIS[] = {{0x44, 0x01, 0x54}, {0x48, 0x28, 0x78}, {0x3e, 0x49, 0x89}, {0x31, 0x68, 0x8e}, {0x26, 0x82, 0x8e},
                           {0x1f, 0x9e, 0x89}, {0x35, 0xb7, 0x79}, {0x6e, 0xce, 0x58}, {0xb5, 0xde, 0x2b}, {0xfd, 0xe7, 0x25}};
constexpr Rgb TURBO[] = {{0x30, 0x12, 0x3b}, {0x41, 0x45, 0xab}, {0x46, 0x75, 0xed}, {0x39, 0xa2, 0xfc}, {0x1b, 0xcf, 0xd4},
                         {0x24, 0xec, 0xa6}, {0x61, 0xfc, 0x6c}, {0xa4, 0xfc, 0x3b}, {0xd1, 0xe8, 0x34}, {0xf3, 0xc6, 0x3a},
                         {0xfe, 0x9b, 0x2d}, {0xf3, 0x63, 0x15}, {0xd9, 0x38, 0x06}, {0xb1, 0x19, 0x01}, {0x7a, 0x04, 0x03}};
constexpr Rgb CALDO[] = {{0x00, 0x00, 0x04}, {0x1b, 0x0c, 0x41}, {0x4a, 0x0c, 0x6b}, {0x78, 0x1c, 0x6d}, {0xa5, 0x2c, 0x60},
                         {0xcf, 0x44, 0x46}, {0xed, 0x69, 0x25}, {0xfb, 0x9b, 0x06}, {0xf7, 0xd1, 0x3d}, {0xfc, 0xff, 0xa4}};

template <std::size_t N>
Rgb interpola(const Rgb (&a)[N], double t) {
    const double x = std::clamp(t, 0.0, 1.0) * static_cast<double>(N - 1);
    const auto i = std::min(static_cast<std::size_t>(x), N - 2);
    const double u = x - static_cast<double>(i);
    const auto mix = [u](std::uint8_t p, std::uint8_t q) { return static_cast<std::uint8_t>(std::lround(p + (q - p) * u)); };
    return {mix(a[i][0], a[i + 1][0]), mix(a[i][1], a[i + 1][1]), mix(a[i][2], a[i + 1][2])};
}

constexpr int BAYER4[4][4] = {{0, 8, 2, 10}, {12, 4, 14, 6}, {3, 11, 1, 9}, {15, 7, 13, 5}};

// Rumore fermo del pixel, in [-0,5, 0,5]: un hash, cosi' la grana non cambia da un fotogramma all'altro
double rumore(int x, int y) {
    std::uint32_t h = static_cast<std::uint32_t>(x) * 374761393u + static_cast<std::uint32_t>(y) * 668265263u;
    h = (h ^ (h >> 13)) * 1274126177u;
    h ^= h >> 16;
    return static_cast<double>(h & 0xffff) / 65535.0 - 0.5;
}

}  // namespace

Rgb colore(Tinta tinta, double t) {
    switch (tinta) {
        case Tinta::fosforo: return interpola(FOSFORO, t);
        case Tinta::grigio: return interpola(GRIGIO, t);
        case Tinta::viridis: return interpola(VIRIDIS, t);
        case Tinta::turbo: return interpola(TURBO, t);
        case Tinta::caldo: return interpola(CALDO, t);
        case Tinta::fosforo_alfa: return interpola(FOSFORO_ALFA, t);
    }
    return {0, 0, 0};
}

void applica(Palette& p, Tinta tinta, Scala s) {
    for (int i = 0; i < s.livelli; ++i) {
        const Rgb c = i == 0 ? Rgb{0, 0, 0} : colore(tinta, static_cast<double>(i) / (s.livelli - 1));
        p.imposta(s.primo + i, c[0], c[1], c[2]);
    }
}

double frazione(double db, double pavimento, double tetto) {
    if (!(tetto > pavimento)) return 0.0;
    return std::clamp((db - pavimento) / (tetto - pavimento), 0.0, 1.0);
}

double soglia_bayer(int x, int y) { return (BAYER4[y & 3][x & 3] + 0.5) / 16.0; }

std::uint8_t indice(double u, int x, int y, Scala s, bool dithering) {
    const double l = std::clamp(u, 0.0, 1.0) * (s.livelli - 1);
    int k = dithering ? static_cast<int>(std::floor(l)) : static_cast<int>(std::lround(l));
    if (dithering && l - k > soglia_bayer(x, y)) ++k;
    // sotto il primo gradino resta il nero: il dithering non sporca il fondo vuoto
    if (u <= 0.0) k = 0;
    return static_cast<std::uint8_t>(s.primo + std::clamp(k, 0, s.livelli - 1));
}

double coefficiente(Persistenza p) {
    switch (p) {
        case Persistenza::breve: return 0.80;
        case Persistenza::normale: return 0.90;
        case Persistenza::lunga: return 0.95;
    }
    return 0.90;
}

Fosforo::Fosforo(int larghezza, int altezza)
    : w_(larghezza), h_(altezza), v_(static_cast<std::size_t>(larghezza) * altezza, 0.0f) {}

void Fosforo::spegni() { std::fill(v_.begin(), v_.end(), 0.0f); }

void Fosforo::decadi(double k) {
    const float f = static_cast<float>(std::clamp(k, 0.0, 1.0));
    for (auto& x : v_) x *= f;
}

void Fosforo::deponi(int x, int y, double v) {
    if (x < 0 || y < 0 || x >= w_ || y >= h_) return;
    float& p = v_[static_cast<std::size_t>(y) * w_ + x];
    p = std::max(p, static_cast<float>(std::clamp(v, 0.0, 1.0)));
}

double Fosforo::at(int x, int y) const {
    if (x < 0 || y < 0 || x >= w_ || y >= h_) return 0.0;
    return v_[static_cast<std::size_t>(y) * w_ + x];
}

void Fosforo::linea(double x0, double y0, double x1, double y1, double v) {
    const bool ripida = std::abs(y1 - y0) > std::abs(x1 - x0);
    if (ripida) {
        std::swap(x0, y0);
        std::swap(x1, y1);
    }
    if (x0 > x1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }
    const double dx = x1 - x0, pend = dx == 0.0 ? 0.0 : (y1 - y0) / dx;
    const auto metti = [&](int a, int b, double c) {
        if (ripida) deponi(b, a, v * c);
        else deponi(a, b, v * c);
    };
    const int xa = static_cast<int>(std::lround(x0)), xb = static_cast<int>(std::lround(x1));
    for (int x = xa; x <= xb; ++x) {
        const double y = y0 + pend * (x - x0);
        const int yi = static_cast<int>(std::floor(y));
        const double f = y - yi;
        metti(x, yi, 1.0 - f);
        metti(x, yi + 1, f);
    }
}

void Fosforo::componi(Tela& t, int x, int y, Scala s, const Effetti& e) const {
    const Rettangolo& r = t.ritaglio();
    for (int j = 0; j < h_; ++j) {
        const int ty = y + j;
        if (ty < r.y || ty >= r.y + r.h) continue;
        std::uint8_t* riga = t.riga(ty);
        for (int i = 0; i < w_; ++i) {
            const int tx = x + i;
            if (tx < r.x || tx >= r.x + r.w) continue;
            double v = at(i, j);
            if (e.bagliore > 0.0) {   // l'alone: solo dai vicini piu' accesi, cosi' i campi pieni non saturano
                double somma = 0.0;
                for (int dj = -1; dj <= 1; ++dj)
                    for (int di = -1; di <= 1; ++di)
                        if (di || dj) somma += at(i + di, j + dj) * (di && dj ? 0.7 : 1.0);
                v += e.bagliore * std::max(0.0, somma / 6.8 - v);
            }
            if (e.fondo > 0.0) v += e.fondo * (0.5 + rumore(tx, ty)) * (1.0 - v);   // il velo, solo dove c'e' poco
            if (v > 0.004) v += e.grana * rumore(tx, ty);
            if (ty & 1) v *= e.righe;
            riga[tx] = indice(std::clamp(v, 0.0, 1.0), tx, ty, s, e.dithering);
        }
    }
}

}  // namespace m688::cascate
