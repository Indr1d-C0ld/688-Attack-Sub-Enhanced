#include "grafica/palette.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>

#include "lettore.hpp"

namespace m688 {

Palette Palette::carica(const std::string& percorso) {
    const auto d = interno::leggi_file(percorso);
    if (d.size() != 768) throw std::runtime_error(percorso + ": una palette deve avere 768 byte");
    Palette p;
    std::copy(d.begin(), d.end(), p.dati_.begin());
    return p;
}

Palette Palette::mescola(const Palette& a, const Palette& b, double t) {
    Palette p;
    for (std::size_t i = 0; i < p.dati_.size(); ++i)
        p.dati_[i] = static_cast<std::uint8_t>(std::lround(a.dati_[i] + (b.dati_[i] - a.dati_[i]) * t));
    return p;
}

Palette Palette::illuminata(double luce, const std::vector<std::pair<int, int>>& intervalli) const {
    Palette p = *this;
    const double l = std::clamp(luce, 0.0, 1.0);
    const double f = 0.07 + 0.93 * l;                                     // luce stellare / pieno giorno
    const double tinta[3] = {0.65 + 0.35 * l, 0.80 + 0.20 * l, 1.0};      // al buio prevale il blu
    for (const auto& [da, a] : intervalli)
        for (int i = std::max(da, 0); i <= std::min(a, 255); ++i)
            for (int k = 0; k < 3; ++k)
                p.dati_[3 * i + k] = static_cast<std::uint8_t>(
                    std::clamp(std::lround(dati_[3 * i + k] * f * tinta[k]), 0L, 255L));
    return p;
}

Palette Palette::schiarita(double f, const std::vector<std::pair<int, int>>& intervalli) const {
    Palette p = *this;
    const double k = std::clamp(f, 0.0, 1.0);
    for (const auto& [da, a] : intervalli)
        for (int i = std::max(da, 0); i <= std::min(a, 255); ++i)
            for (int c = 0; c < 3; ++c)
                p.dati_[3 * i + c] = static_cast<std::uint8_t>(std::lround(dati_[3 * i + c] + (255 - dati_[3 * i + c]) * k));
    return p;
}

Palette Palette::scurita(double f, const std::vector<std::pair<int, int>>& intervalli) const {
    Palette p = *this;
    const double k = 1.0 - std::clamp(f, 0.0, 1.0);
    for (const auto& [da, a] : intervalli)
        for (int i = std::max(da, 0); i <= std::min(a, 255); ++i)
            for (int c = 0; c < 3; ++c) p.dati_[3 * i + c] = static_cast<std::uint8_t>(std::lround(dati_[3 * i + c] * k));
    return p;
}

}  // namespace m688
