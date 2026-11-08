#include "grafica/moto_navi.hpp"

#include <algorithm>
#include <array>
#include <cmath>

#include "grafica/palette.hpp"
#include "nucleo/geometria.hpp"
#include "nucleo/onde.hpp"

namespace m688 {

namespace {

// Rollio / beccheggio RMS (gradi) per WMO 3, 4, 5, 6, 7, 8 e taglia (nota C §12.4, STIMA calibrata sul DD-963)
constexpr std::array<double, 6> HS_RIGHE{0.875, 1.875, 3.25, 5.0, 7.5, 11.5};   // centri delle classi Douglas 3-8
struct Taglia { double lunghezza; std::array<double, 6> rollio, beccheggio; };
constexpr std::array<Taglia, 5> TAGLIE{{
    {59.0, {2.5, 4.0, 7.0, 10.0, 14.0, 18.0}, {1.5, 2.5, 4.0, 5.5, 7.0, 8.0}},     // Nanuchka ~700 t
    {130.0, {1.0, 2.5, 5.0, 7.0, 10.0, 13.0}, {0.5, 1.2, 2.0, 3.0, 4.0, 5.0}},     // fregata ~3600 t
    {170.0, {0.8, 2.0, 4.5, 6.0, 8.0, 10.0}, {0.4, 0.9, 1.5, 2.3, 3.2, 4.0}},      // incrociatore ~8000 t
    {250.0, {0.5, 1.2, 2.5, 4.0, 6.0, 8.0}, {0.2, 0.5, 0.9, 1.5, 2.3, 3.0}},       // Kirov ~24000 t
    {320.0, {0.2, 0.5, 1.0, 2.0, 3.0, 4.0}, {0.1, 0.3, 0.6, 1.0, 1.6, 2.2}},       // portaerei ~80000 t
}};

double interpola(const std::array<double, 6>& v, double hs) {
    if (hs <= HS_RIGHE[0]) return v[0] * hs / HS_RIGHE[0];   // sotto mare 3: in proporzione a Hs
    for (std::size_t i = 1; i < HS_RIGHE.size(); ++i)
        if (hs <= HS_RIGHE[i]) {
            const double f = (hs - HS_RIGHE[i - 1]) / (HS_RIGHE[i] - HS_RIGHE[i - 1]);
            return v[i - 1] + f * (v[i] - v[i - 1]);
        }
    return v.back();
}

}  // namespace

void ampiezze_rms(double lunghezza_m, double hs_m, double& rollio, double& beccheggio) {
    const double l = std::clamp(lunghezza_m, TAGLIE.front().lunghezza, TAGLIE.back().lunghezza);
    std::size_t i = 1;
    while (i + 1 < TAGLIE.size() && TAGLIE[i].lunghezza < l) ++i;
    const auto& a = TAGLIE[i - 1];
    const auto& b = TAGLIE[i];
    const double f = std::clamp((l - a.lunghezza) / (b.lunghezza - a.lunghezza), 0.0, 1.0);
    rollio = interpola(a.rollio, hs_m) * (1 - f) + interpola(b.rollio, hs_m) * f;
    beccheggio = interpola(a.beccheggio, hs_m) * (1 - f) + interpola(b.beccheggio, hs_m) * f;
}

MotoNave moto_nave(double lunghezza_m, double larghezza_m, double hs_m, double tp_s, double mare_da_gradi,
                   double rotta_gradi, double velocita_m_s, double tempo_s, double fase) {
    MotoNave m;
    if (hs_m <= 0.0 || lunghezza_m <= 0.0) return m;
    double rr, bb;
    ampiezze_rms(lunghezza_m, hs_m, rr, bb);
    const double tp = tp_s > 0.0 ? tp_s : periodo_picco_s(hs_m);
    // mu: 0 col mare di prua (il mare viene da dove punta la prua), 180 di poppa
    const double mu = radianti(normalizza_180(mare_da_gradi - rotta_gradi));
    const double c = 1.56 * tp;   // velocita' di fase in acqua profonda (m/s)
    const double te = std::max(2.0, tp / std::max(0.2, 1.0 + velocita_m_s * std::cos(mu) / c));
    const double tr = std::clamp(0.75 * std::max(larghezza_m, 4.0), 4.0, 22.0);   // periodo proprio di rollio
    // rollio pieno al traverso, ridotto di prua; beccheggio pieno di prua, ridotto al traverso e di poppa (x0,6)
    const double f_rollio = 0.4 + 0.6 * std::fabs(std::sin(mu));
    const double f_becch = (std::cos(mu) >= 0 ? 1.0 : 0.6) * (0.5 + 0.5 * std::fabs(std::cos(mu)));
    // due componenti per non far sembrare il moto un pendolo; ampiezza tale che l'RMS sia quello della tabella
    constexpr double NORMA = 1.4142 / 0.7616;   // sqrt(2) / sqrt(0,7^2 + 0,3^2)
    const double w_r = 2 * PI / tr, w_e = 2 * PI / te;
    m.rollio_gradi = rr * f_rollio * NORMA * (0.7 * std::sin(w_r * tempo_s + fase) + 0.3 * std::sin(1.27 * w_r * tempo_s + 2.1 * fase));
    m.beccheggio_gradi = bb * f_becch * NORMA *
                         (0.7 * std::sin(w_e * tempo_s + 1.3 * fase) + 0.3 * std::sin(0.83 * w_e * tempo_s + 0.7 * fase + 1.0));
    // sussulto: Hs/4 filtrato dalla lunghezza (le navi lunghe rispetto all'onda salgono meno)
    const double lambda = 1.56 * tp * tp;
    const double filtro = 1.0 / (1.0 + std::pow(lunghezza_m / lambda, 2.0));
    m.sussulto_m = (hs_m / 4.0) * filtro * 1.4142 * std::sin(w_e * tempo_s + 1.3 * fase - 0.6);
    // la prua si pianta: beccheggio in giu' oltre il 60% del massimo, da Hs 2,5 m, per le navi sotto i 200 m
    const double massimo = bb * f_becch * NORMA;
    if (hs_m >= 2.5 && lunghezza_m < 200.0 && massimo > 0.0) {
        const double giu = -m.beccheggio_gradi / massimo;
        m.spruzzo = std::clamp((giu - 0.6) / 0.4, 0.0, 1.0) * std::clamp((hs_m - 2.5) / 2.5 + 0.4, 0.0, 1.0) * std::cos(std::min(std::fabs(mu), PI / 2));
    }
    return m;
}

double angolo_apparente(const MotoNave& m, double aob_gradi, bool dritta) {
    // di fianco si vede il beccheggio (prua a destra se l'osservatore e' sulla dritta), di prua il rollio
    const double lato = dritta ? 1.0 : -1.0;
    const double a = radianti(aob_gradi);
    return radianti(lato * m.beccheggio_gradi * std::sin(a) + lato * m.rollio_gradi * std::cos(a));
}

void disegna_sprite_ruotato(Tela& t, const Sprite& s, double x, double y, double scala, double angolo, int y_limite,
                            double foschia) {
    if (std::fabs(angolo) < 1e-4) {
        t.disegna_sprite(s, x, y, scala, y_limite, foschia);
        return;
    }
    if (scala <= 0.0 || s.w == 0 || s.h == 0) return;
    static constexpr int BAYER[4][4] = {{0, 8, 2, 10}, {12, 4, 14, 6}, {3, 11, 1, 9}, {15, 7, 13, 5}};
    const double f = std::clamp(foschia, 0.0, 1.0);
    const double ca = std::cos(angolo), sa = std::sin(angolo);
    // riquadro dello sprite ruotato attorno all'ancora
    double minx = 1e9, maxx = -1e9, miny = 1e9, maxy = -1e9;
    for (const auto& [px, py] : {std::pair{0.0, 0.0}, {double(s.w), 0.0}, {0.0, double(s.h)}, {double(s.w), double(s.h)}}) {
        const double dx = (px - s.ancora_x) * scala, dy = (py - s.ancora_y) * scala;
        const double rx = dx * ca + dy * sa, ry = -dx * sa + dy * ca;   // rotazione antioraria sullo schermo (y in giu')
        minx = std::min(minx, rx); maxx = std::max(maxx, rx); miny = std::min(miny, ry); maxy = std::max(maxy, ry);
    }
    const Rettangolo& r = t.ritaglio();
    const int x0 = std::max(static_cast<int>(std::floor(x + minx)), r.x), x1 = std::min(static_cast<int>(std::ceil(x + maxx)), r.x + r.w);
    const int y0 = std::max(static_cast<int>(std::floor(y + miny)), r.y);
    const int y1 = std::min({static_cast<int>(std::ceil(y + maxy)), r.y + r.h, y_limite});
    for (int ty = y0; ty < y1; ++ty)
        for (int tx = x0; tx < x1; ++tx) {
            const double dx = tx + 0.5 - x, dy = ty + 0.5 - y;
            const double ux = dx * ca - dy * sa, uy = dx * sa + dy * ca;   // rotazione inversa
            const int sx = static_cast<int>(std::floor(ux / scala + s.ancora_x)), sy = static_cast<int>(std::floor(uy / scala + s.ancora_y));
            if (sx < 0 || sy < 0 || sx >= s.w || sy >= s.h) continue;
            const std::uint8_t c = s.pix[static_cast<std::size_t>(sy) * s.w + sx];
            if (c == TRASPARENTE) continue;
            if (f > 0.0 && c >= colori::GRIGI && c < colori::GRIGI + 15) {
                const double v = (c - colori::GRIGI) * (1.0 - f) + 13.0 * f, b = std::floor(v);
                t.pixel(tx, ty, static_cast<std::uint8_t>(colori::GRIGI + std::min(14.0, b + ((v - b) * 16.0 > BAYER[ty & 3][tx & 3] ? 1.0 : 0.0))));
            } else {
                t.pixel(tx, ty, c);
            }
        }
}

}  // namespace m688
