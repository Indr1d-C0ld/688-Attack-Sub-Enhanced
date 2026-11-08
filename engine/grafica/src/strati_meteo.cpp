#include "grafica/strati_meteo.hpp"

#include <algorithm>
#include <cmath>
#include <string>

namespace m688 {

namespace {

void segmento(Tela& t, double x0, double y0, double x1, double y1, std::uint8_t c, int tratteggio = 0) {
    if (!std::isfinite(x0) || !std::isfinite(y0) || !std::isfinite(x1) || !std::isfinite(y1)) return;
    const int n = static_cast<int>(std::min(std::max(std::fabs(x1 - x0), std::fabs(y1 - y0)), 4096.0)) + 1;
    const Rettangolo& r = t.ritaglio();
    for (int i = 0; i <= n; ++i) {
        if (tratteggio > 0 && (i / tratteggio) % 2) continue;
        const int x = static_cast<int>(std::lround(x0 + (x1 - x0) * i / n)), y = static_cast<int>(std::lround(y0 + (y1 - y0) * i / n));
        if (x >= r.x && y >= r.y && x < r.x + r.w && y < r.y + r.h) t.pixel(x, y, c);
    }
}

}  // namespace

void disegna_barba(Tela& t, double x, double y, double vento_kn, double da_gradi, int lunghezza, std::uint8_t colore) {
    if (!std::isfinite(x) || !std::isfinite(y) || !std::isfinite(vento_kn) || !std::isfinite(da_gradi)) return;
    const double a = radianti(da_gradi);
    const double ux = std::sin(a), uy = -std::cos(a);           // verso da dove viene (y dello schermo in giu')
    const double px = -uy, py = ux;                             // perpendicolare (barbe sulla destra dell'asta)
    if (vento_kn < 2.5) {                                       // calma: un cerchietto
        for (int k = 0; k < 12; ++k) {
            const double q = 2 * PI * k / 12;
            t.pixel(static_cast<int>(std::lround(x + 3 * std::cos(q))), static_cast<int>(std::lround(y + 3 * std::sin(q))), colore);
        }
        return;
    }
    const double ex = x + ux * lunghezza, ey = y + uy * lunghezza;
    segmento(t, x, y, ex, ey, colore);
    int resto = static_cast<int>(std::lround(vento_kn / 5.0)) * 5;
    double d = 0.0;                                             // dalla punta verso il piede
    const double barba = 0.45 * lunghezza;
    while (resto >= 50) {                                       // bandiera: triangolo pieno
        for (double k = 0; k <= 4; k += 0.5) {
            const double bx = ex - ux * (d + k), by = ey - uy * (d + k);
            segmento(t, bx, by, bx + px * barba * (1 - k / 4), by + py * barba * (1 - k / 4), colore);
        }
        d += 6;
        resto -= 50;
    }
    while (resto >= 10) {
        const double bx = ex - ux * d, by = ey - uy * d;
        segmento(t, bx, by, bx + px * barba + ux * 2, by + py * barba + uy * 2, colore);
        d += 4;
        resto -= 10;
    }
    if (resto >= 5) {
        if (d == 0) d = 4;                                      // la mezza barba sola non sta in punta
        const double bx = ex - ux * d, by = ey - uy * d;
        segmento(t, bx, by, bx + px * barba * 0.5 + ux, by + py * barba * 0.5 + uy, colore);
    }
}

void disegna_strati_meteo(Tela& t, const Font& font, const StratiMeteo& s) {
    if (!s.a_tela || !s.a_mondo) return;
    const Rettangolo& a = s.area;
    t.imposta_ritaglio(a);
    // ghiaccio e nebbia: campionati a celle di 4 pixel, tratteggio obliquo e puntinato
    if (s.ghiaccio || s.nebbia) {
        const int P = std::max(2, s.campioni_px);
        for (int y = a.y; y < a.y + a.h; y += P)
            for (int x = a.x; x < a.x + a.w; x += P) {
                const Vec2 m = s.a_mondo(Vec2{x + P / 2.0, y + P / 2.0});
                const bool gh = s.ghiaccio && s.ghiaccio(m) >= 0.5;
                const bool ne = s.nebbia && s.nebbia(m);
                for (int dy = 0; dy < P; ++dy)
                    for (int dx = 0; dx < P; ++dx) {
                        const int px = x + dx, py = y + dy;
                        if (gh && (px + py) % 6 == 0) t.pixel(px, py, s.colori.ghiaccio);
                        else if (ne && px % 4 == 0 && py % 4 == 2) t.pixel(px, py, s.colori.nebbia);
                    }
                if (gh) {   // bordo: dove la cella accanto non e' ghiaccio
                    const bool destra = s.ghiaccio(s.a_mondo(Vec2{x + P * 1.5, y + P / 2.0})) < 0.5;
                    const bool sotto = s.ghiaccio(s.a_mondo(Vec2{x + P / 2.0, y + P * 1.5})) < 0.5;
                    if (destra) segmento(t, x + P, y, x + P, y + P, s.colori.ghiaccio);
                    if (sotto) segmento(t, x, y + P, x + P, y + P, s.colori.ghiaccio);
                }
            }
    }
    // celle temporalesche
    for (const auto& c : s.celle) {
        const Vec2 q = s.a_tela(c.pos);
        const Vec2 bordo = s.a_tela(Vec2{c.pos.x + c.raggio_m, c.pos.y});
        const double r = std::hypot(bordo.x - q.x, bordo.y - q.y);
        if (!std::isfinite(q.x) || !std::isfinite(q.y) || !std::isfinite(r)) continue;
        const int n = static_cast<int>(std::clamp(2 * PI * r / 3, 24.0, 4000.0));
        for (int k = 0; k < n; k += 2) {
            const double a0 = 2 * PI * k / n, a1 = 2 * PI * (k + 1) / n;
            segmento(t, q.x + r * std::cos(a0), q.y + r * std::sin(a0), q.x + r * std::cos(a1), q.y + r * std::sin(a1), s.colori.celle);
        }
        // fulmine al centro
        segmento(t, q.x + 2, q.y - 6, q.x - 2, q.y, s.colori.celle);
        segmento(t, q.x - 2, q.y, q.x + 2, q.y, s.colori.celle);
        segmento(t, q.x + 2, q.y, q.x - 2, q.y + 6, s.colori.celle);
    }
    // fulmini sentiti al sonar: un raggio tratteggiato di 60 pixel lungo il rilevamento, col fulmine in punta
    for (const double r : s.rilevamenti_fulmini) {
        const Vec2 q = s.a_tela(s.battello);
        const double ang = radianti(r), ux = std::sin(ang), uy = -std::cos(ang);
        segmento(t, q.x + 8 * ux, q.y + 8 * uy, q.x + 60 * ux, q.y + 60 * uy, s.colori.celle, 3);
        const double ex = q.x + 66 * ux, ey = q.y + 66 * uy;
        segmento(t, ex + 2, ey - 5, ex - 2, ey, s.colori.celle);
        segmento(t, ex - 2, ey, ex + 2, ey, s.colori.celle);
        segmento(t, ex + 2, ey, ex - 2, ey + 5, s.colori.celle);
    }
    // barbe del vento e stato del mare sulla griglia
    if (s.vento || s.stato_mare)
        for (int y = a.y + s.passo_px / 2; y < a.y + a.h; y += s.passo_px)
            for (int x = a.x + s.passo_px / 2; x < a.x + a.w; x += s.passo_px) {
                const Vec2 m = s.a_mondo(Vec2{static_cast<double>(x), static_cast<double>(y)});
                if (s.vento) {
                    double v = 0, da = 0;
                    s.vento(m, v, da);
                    disegna_barba(t, x, y, v / 0.5144, da, std::min(s.passo_px * 3 / 8, 24), s.colori.vento);   // una griglia rada non la allunga
                }
                if (s.stato_mare) {
                    const std::string txt = std::to_string(std::clamp(s.stato_mare(m), 0, 9));
                    font.scrivi(t, x + 3, y + 3, txt, s.colori.mare, 1);
                }
            }
    t.togli_ritaglio();
}

}  // namespace m688
