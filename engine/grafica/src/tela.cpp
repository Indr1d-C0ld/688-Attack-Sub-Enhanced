#include "grafica/tela.hpp"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <stdexcept>

#include "grafica/palette.hpp"
#include "grafica/sprite.hpp"

namespace m688 {

Tela::Tela(int w, int h) : w_(w), h_(h), rit_{0, 0, w, h}, pix_(static_cast<std::size_t>(w) * h, 0) {}

void Tela::imposta_ritaglio(Rettangolo r) {
    const int x0 = std::max(r.x, 0), y0 = std::max(r.y, 0);
    const int x1 = std::min(r.x + r.w, w_), y1 = std::min(r.y + r.h, h_);
    rit_ = {x0, y0, std::max(x1 - x0, 0), std::max(y1 - y0, 0)};
}

void Tela::togli_ritaglio() { rit_ = {0, 0, w_, h_}; }

void Tela::riempi(std::uint8_t c) { std::fill(pix_.begin(), pix_.end(), c); }

void Tela::pixel(int x, int y, std::uint8_t c) {
    if (x >= rit_.x && x < rit_.x + rit_.w && y >= rit_.y && y < rit_.y + rit_.h)
        pix_[static_cast<std::size_t>(y) * w_ + x] = c;
}

void Tela::rettangolo_pieno(int x, int y, int w, int h, std::uint8_t c) {
    const int x0 = std::max(x, rit_.x), x1 = std::min(x + w, rit_.x + rit_.w);
    const int y0 = std::max(y, rit_.y), y1 = std::min(y + h, rit_.y + rit_.h);
    for (int yy = y0; yy < y1; ++yy)
        std::fill_n(pix_.begin() + static_cast<std::ptrdiff_t>(yy) * w_ + x0, std::max(x1 - x0, 0), c);
}

void Tela::linea_orizzontale(int x0, int x1, int y, std::uint8_t c) {
    if (x1 < x0) std::swap(x0, x1);
    rettangolo_pieno(x0, y, x1 - x0 + 1, 1, c);
}

void Tela::linea_verticale(int x, int y0, int y1, std::uint8_t c) {
    if (y1 < y0) std::swap(y0, y1);
    rettangolo_pieno(x, y0, 1, y1 - y0 + 1, c);
}

void Tela::disegna_sprite(const Sprite& s, double x, double y, double scala, int y_limite, double foschia,
                          double grigio_foschia) {
    static constexpr int BAYER[4][4] = {{0, 8, 2, 10}, {12, 4, 14, 6}, {3, 11, 1, 9}, {15, 7, 13, 5}};
    const double f = std::clamp(foschia, 0.0, 1.0);
    // tabella dei grigi schiariti: parte intera e frazione (in sedicesimi) per la retinatura
    int base[15], frazione[15];
    for (int g = 0; g < 15; ++g) {
        const double v = g * (1.0 - f) + grigio_foschia * f;
        base[g] = static_cast<int>(std::floor(v));
        frazione[g] = static_cast<int>(std::lround((v - base[g]) * 16.0));
    }
    if (scala <= 0.0 || s.w == 0 || s.h == 0) return;
    // rettangolo di destinazione (coordinate reali), poi intersezione con ritaglio e limite inferiore
    const double dx0 = x - s.ancora_x * scala, dy0 = y - s.ancora_y * scala;
    const int x0 = std::max(static_cast<int>(std::floor(dx0)), rit_.x);
    const int y0 = std::max(static_cast<int>(std::floor(dy0)), rit_.y);
    const int x1 = std::min(static_cast<int>(std::ceil(dx0 + s.w * scala)), rit_.x + rit_.w);
    const int y1 = std::min({static_cast<int>(std::ceil(dy0 + s.h * scala)), rit_.y + rit_.h, y_limite});
    for (int ty = y0; ty < y1; ++ty) {
        const int sy = static_cast<int>(std::floor((ty + 0.5 - dy0) / scala));
        if (sy < 0 || sy >= s.h) continue;
        const std::uint8_t* riga = s.pix.data() + static_cast<std::size_t>(sy) * s.w;
        std::uint8_t* dst = pix_.data() + static_cast<std::size_t>(ty) * w_;
        for (int tx = x0; tx < x1; ++tx) {
            const int sx = static_cast<int>(std::floor((tx + 0.5 - dx0) / scala));
            if (sx < 0 || sx >= s.w) continue;
            const std::uint8_t c = riga[sx];
            if (c == TRASPARENTE) continue;
            if (f > 0.0 && c >= colori::GRIGI && c < colori::GRIGI + 15) {
                const int g = c - colori::GRIGI;
                dst[tx] = static_cast<std::uint8_t>(colori::GRIGI + std::min(14, base[g] + (BAYER[ty & 3][tx & 3] < frazione[g] ? 1 : 0)));
            } else {
                dst[tx] = c;
            }
        }
    }
}

void Tela::salva_bmp(const std::string& percorso, const Palette& pal) const {
    std::ofstream f(percorso, std::ios::binary);
    if (!f) throw std::runtime_error("impossibile scrivere " + percorso);
    const int riga = (w_ + 3) & ~3;
    const std::uint32_t dati = static_cast<std::uint32_t>(riga) * static_cast<std::uint32_t>(h_);
    const std::uint32_t inizio = 14 + 40 + 1024;
    auto u16 = [&f](std::uint16_t v) { f.put(static_cast<char>(v & 0xff)); f.put(static_cast<char>(v >> 8)); };
    auto u32 = [&u16](std::uint32_t v) { u16(static_cast<std::uint16_t>(v & 0xffff)); u16(static_cast<std::uint16_t>(v >> 16)); };
    f.write("BM", 2); u32(inizio + dati); u32(0); u32(inizio);
    u32(40); u32(static_cast<std::uint32_t>(w_)); u32(static_cast<std::uint32_t>(h_)); u16(1); u16(8);
    u32(0); u32(dati); u32(2835); u32(2835); u32(256); u32(0);
    for (int i = 0; i < 256; ++i) {
        const auto c = pal.rgb(i);
        f.put(static_cast<char>(c[2])); f.put(static_cast<char>(c[1])); f.put(static_cast<char>(c[0])); f.put(0);
    }
    std::vector<char> buf(static_cast<std::size_t>(riga), 0);
    for (int y = h_ - 1; y >= 0; --y) {   // il BMP parte dalla riga in basso
        std::copy_n(pix_.begin() + static_cast<std::ptrdiff_t>(y) * w_, w_, buf.begin());
        f.write(buf.data(), riga);
    }
}

}  // namespace m688
