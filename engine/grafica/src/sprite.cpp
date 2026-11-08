#include "grafica/sprite.hpp"

#include <cmath>
#include <stdexcept>

#include "nucleo/inflate.hpp"
#include "lettore.hpp"

namespace m688 {

InsiemeSprite InsiemeSprite::carica(const std::string& percorso) {
    const auto d = interno::leggi_file(percorso);
    interno::Lettore l(d, percorso);
    l.firma("SPR2");   // SPR1 (indici in chiaro) non si legge piu': si riesporta con tools/pipeline/esporta_motore.py
    InsiemeSprite r;
    const auto n = l.leggi<std::uint16_t>();
    r.scala_ = l.leggi<float>();
    r.sprite_.reserve(n);
    for (int i = 0; i < n; ++i) {
        Sprite s;
        s.aob = l.leggi<std::uint16_t>();
        s.dritta = l.leggi<std::uint8_t>() == 0;
        s.elevazione = l.leggi<std::int8_t>();
        s.w = l.leggi<std::uint16_t>();
        s.h = l.leggi<std::uint16_t>();
        s.ancora_x = l.leggi<float>();
        s.ancora_y = l.leggi<float>();
        const auto nz = l.leggi<std::uint32_t>();
        const auto* z = l.blocco(nz);
        try {
            s.pix = interno::inflate(z, nz, static_cast<std::size_t>(s.w) * s.h);   // indici compressi con DEFLATE
        } catch (const std::runtime_error& e) {
            throw std::runtime_error(percorso + ": " + e.what());
        }
        r.sprite_.push_back(std::move(s));
    }
    if (r.sprite_.empty()) throw std::runtime_error(percorso + ": nessuno sprite");
    return r;
}

const Sprite& InsiemeSprite::per_angolo(double aob, bool dritta, double elevazione) const {
    const Sprite* migliore = nullptr;
    double scarto = 1e9;
    for (const auto& s : sprite_) {
        // a 0 e 180 gradi i due lati coincidono: si accetta anche l'altro lato
        const bool lato_ok = s.dritta == dritta || s.aob == 0 || s.aob == 180;
        // l'elevazione pesa piu' dell'AOB: prima la fascia di elevazione giusta, poi l'angolo
        const double e = std::fabs(s.aob - aob) + 10.0 * std::fabs(s.elevazione - elevazione) + (lato_ok ? 0.0 : 1000.0);
        if (e < scarto) { scarto = e; migliore = &s; }
    }
    return *migliore;
}

}  // namespace m688
