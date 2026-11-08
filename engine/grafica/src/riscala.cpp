#include "grafica/riscala.hpp"

#include <algorithm>
#include <stdexcept>

#include "lettore.hpp"

namespace m688 {

ImmagineIndicizzata ImmagineIndicizzata::carica(const std::string& percorso) {
    const auto d = interno::leggi_file(percorso);
    interno::Lettore l(d, percorso);
    l.firma("IMG1");
    ImmagineIndicizzata im;
    im.w = l.leggi<std::uint16_t>();
    im.h = l.leggi<std::uint16_t>();
    const auto* p = l.blocco(static_cast<std::size_t>(im.w) * im.h);
    im.pix.assign(p, p + static_cast<std::size_t>(im.w) * im.h);
    return im;
}

ImmagineIndicizzata scale2x(const ImmagineIndicizzata& s) {
    ImmagineIndicizzata d{2 * s.w, 2 * s.h, std::vector<std::uint8_t>(static_cast<std::size_t>(4) * s.w * s.h)};
    auto at = [&s](int x, int y) {   // bordi replicati
        x = x < 0 ? 0 : (x >= s.w ? s.w - 1 : x);
        y = y < 0 ? 0 : (y >= s.h ? s.h - 1 : y);
        return s.pix[static_cast<std::size_t>(y) * s.w + x];
    };
    auto put = [&d](int x, int y, std::uint8_t c) { d.pix[static_cast<std::size_t>(y) * d.w + x] = c; };
    for (int y = 0; y < s.h; ++y)
        for (int x = 0; x < s.w; ++x) {
            const std::uint8_t p = at(x, y), a = at(x, y - 1), b = at(x + 1, y), c = at(x - 1, y), e = at(x, y + 1);
            std::uint8_t r1 = p, r2 = p, r3 = p, r4 = p;
            if (a != e && c != b) {   // regole di EPX/Scale2x
                if (c == a) r1 = a;
                if (a == b) r2 = b;
                if (c == e) r3 = c;
                if (e == b) r4 = b;
            }
            put(2 * x, 2 * y, r1); put(2 * x + 1, 2 * y, r2);
            put(2 * x, 2 * y + 1, r3); put(2 * x + 1, 2 * y + 1, r4);
        }
    return d;
}

ImmagineIndicizzata raddoppia_nitido(const ImmagineIndicizzata& s) {
    ImmagineIndicizzata d{2 * s.w, 2 * s.h, std::vector<std::uint8_t>(static_cast<std::size_t>(4) * s.w * s.h)};
    for (int y = 0; y < d.h; ++y)
        for (int x = 0; x < d.w; ++x)
            d.pix[static_cast<std::size_t>(y) * d.w + x] = s.pix[static_cast<std::size_t>(y / 2) * s.w + x / 2];
    return d;
}

ImmagineIndicizzata stira_verticale(const ImmagineIndicizzata& s, int nuova_altezza) {
    ImmagineIndicizzata d{s.w, nuova_altezza, std::vector<std::uint8_t>(static_cast<std::size_t>(s.w) * nuova_altezza)};
    for (int y = 0; y < nuova_altezza; ++y) {
        const int sy = static_cast<int>((static_cast<long long>(y) * s.h) / nuova_altezza);
        std::copy_n(s.pix.begin() + static_cast<std::ptrdiff_t>(sy) * s.w, s.w,
                    d.pix.begin() + static_cast<std::ptrdiff_t>(y) * d.w);
    }
    return d;
}

ImmagineIndicizzata porta_a_640x480(const ImmagineIndicizzata& s, bool nitido) {
    if (s.w != 320 || s.h != 200) throw std::runtime_error("porta_a_640x480: serve un'immagine 320x200");
    return stira_verticale(nitido ? raddoppia_nitido(s) : scale2x(s), 480);
}

}  // namespace m688
