#include "nucleo/mondo.hpp"

#include <algorithm>
#include <cmath>
#include <cstring>

namespace m688 {

Nave& Mondo::aggiungi(Nave n) {
    n.pos_prec = n.pos;
    n.rotta_prec = n.rotta;
    navi_.push_back(std::move(n));
    return navi_.back();
}

const Nave* Mondo::trova(int id) const {
    for (const auto& n : navi_)
        if (n.id == id) return &n;
    return nullptr;
}

void Mondo::passo(double dt) {
    for (auto& n : navi_) {
        n.pos_prec = n.pos;
        n.rotta_prec = n.rotta;
        // velocita': si avvicina a quella ordinata con accelerazione limitata
        const double dv = n.velocita_ordinata - n.velocita;
        const double max_dv = n.accelerazione * dt;
        n.velocita += std::clamp(dv, -max_dv, max_dv);
        // rotta: accosta dal lato piu' breve; il rateo cala sotto i 3 nodi (timone poco efficace)
        const double errore = normalizza_180(n.rotta_ordinata - n.rotta);
        const double efficacia = std::clamp(n.velocita / (3.0 * NODO), 0.0, 1.0);
        const double max_da = n.rateo_accostata * efficacia * dt;
        n.rotta = normalizza_360(n.rotta + std::clamp(errore, -max_da, max_da));
        const double r = radianti(n.rotta);
        n.pos.x += n.velocita * std::sin(r) * dt;
        n.pos.y += n.velocita * std::cos(r) * dt;
    }
    tempo_ += dt;
}

Mondo Mondo::interpolato(double alfa) const {
    Mondo m = *this;
    const double a = std::clamp(alfa, 0.0, 1.0);
    for (auto& n : m.navi_) {
        n.pos.x = n.pos_prec.x + (n.pos.x - n.pos_prec.x) * a;
        n.pos.y = n.pos_prec.y + (n.pos.y - n.pos_prec.y) * a;
        n.rotta = normalizza_360(n.rotta_prec + normalizza_180(n.rotta - n.rotta_prec) * a);
    }
    return m;
}

std::uint64_t Mondo::impronta() const {
    std::uint64_t h = 1469598103934665603ull;   // FNV-1a sui bit dei valori
    auto mescola = [&h](double v) {
        std::uint64_t b;
        std::memcpy(&b, &v, sizeof b);
        for (int i = 0; i < 8; ++i) {
            h ^= (b >> (8 * i)) & 0xffu;
            h *= 1099511628211ull;
        }
    };
    for (const auto& n : navi_) {
        mescola(n.pos.x); mescola(n.pos.y); mescola(n.rotta); mescola(n.velocita);
    }
    mescola(tempo_);
    return h;
}

}  // namespace m688
