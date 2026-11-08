#pragma once

#include <algorithm>
#include <cstdlib>
#include <optional>
#include <utility>

#include "grafica/tela.hpp"
#include "nucleo/geometria.hpp"
#include "piattaforma/finestra.hpp"

namespace m688 {

inline bool dentro_carta(const Comandi& c, Rettangolo r) {
    const auto [mx, my] = c.mouse;
    return mx >= r.x && mx < r.x + r.w && my >= r.y && my < r.y + r.h;
}

// Trascinare la carta la sposta, come nel replay; vero se si e' mossa
inline bool trascina_carta(const Comandi& c, Rettangolo carta, double mpp, std::optional<std::pair<int, int>>& presa, Vec2& centro) {
    const auto [mx, my] = c.mouse;
    bool mossa = false;
    if (c.premuto && (presa || dentro_carta(c, carta))) {
        if (presa && (mx != presa->first || my != presa->second)) {
            centro.x -= (mx - presa->first) * mpp;
            centro.y += (my - presa->second) * mpp;
            mossa = true;
        }
        presa = std::make_pair(mx, my);
    } else {
        presa.reset();
    }
    return mossa;
}

// Passando da mpp_prima a mpp_dopo il punto sotto il cursore resta dov'e'
inline void zoom_sul_cursore(const Comandi& c, Rettangolo carta, double mpp_prima, double mpp_dopo, Vec2& centro) {
    const auto [mx, my] = c.mouse;
    centro.x += (mx - (carta.x + carta.w / 2.0)) * (mpp_prima - mpp_dopo);
    centro.y -= (my - (carta.y + carta.h / 2.0)) * (mpp_prima - mpp_dopo);
}

// La rotella sopra una carta a scala continua (il replay): uno scatto vale I o O, fuori dalla carta non fa nulla
inline void rotella_carta(const Comandi& c, Rettangolo carta, Vec2& centro, double& mpp, double minimo = 10.0, double massimo = 2000.0) {
    if (!c.rotella || !dentro_carta(c, carta)) return;
    for (int k = std::abs(c.rotella); k > 0; --k) {
        const double prima = mpp;
        mpp = std::clamp(c.rotella > 0 ? mpp / 1.5 : mpp * 1.5, minimo, massimo);
        zoom_sul_cursore(c, carta, prima, mpp, centro);
    }
}

}  // namespace m688
