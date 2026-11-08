#include "grafica/ritratto.hpp"

#include <algorithm>
#include <cmath>
#include <functional>

#include "nucleo/geometria.hpp"

namespace m688 {

using namespace cerimonia;

namespace {

// Coordinate di riferimento: tela 176 x 232, centro della testa in (88, 84), 40 pixel per unita' della testa
// (la testa e' larga circa 1,5 unita'). Il pennello riempie i pixel veri scalando all'indietro.
constexpr double RW = 176.0, RH = 232.0, CX = 88.0, HY = 90.0, U = 41.0;

struct Q {
    double x, y;
};

bool nel_poligono(const std::vector<Q>& v, double x, double y) {
    bool d = false;
    for (std::size_t i = 0, j = v.size() - 1; i < v.size(); j = i++)
        if ((v[i].y > y) != (v[j].y > y) && x < (v[j].x - v[i].x) * (y - v[i].y) / (v[j].y - v[i].y) + v[i].x) d = !d;
    return d;
}

double rumore(double x, double y, int seme) {
    const int ix = static_cast<int>(std::floor(x)), iy = static_cast<int>(std::floor(y));
    const double fx = x - ix, fy = y - iy, sx = fx * fx * (3 - 2 * fx), sy = fy * fy * (3 - 2 * fy);
    const double a = caso(ix, iy, seme), b = caso(ix + 1, iy, seme), c = caso(ix, iy + 1, seme), d = caso(ix + 1, iy + 1, seme);
    return (a * (1 - sx) + b * sx) * (1 - sy) + (c * (1 - sx) + d * sx) * sy;
}

class Pennello {
public:
    Pennello(Tela& t, Rettangolo r) : t_(t), r_(r), s_(std::min(r.w / RW, r.h / RH)) {
        ox_ = r.x + (r.w - RW * s_) / 2;
        oy_ = r.y + (r.h - RH * s_) / 2;
    }
    double scala() const { return s_; }
    int px(double x) const { return static_cast<int>(std::floor(ox_ + x * s_)); }
    int py(double y) const { return static_cast<int>(std::floor(oy_ + y * s_)); }
    double rx(int px) const { return (px + 0.5 - ox_) / s_; }
    double ry(int py) const { return (py + 0.5 - oy_) / s_; }
    const Rettangolo& area() const { return r_; }
    void riempi(double x0, double y0, double x1, double y1, const std::function<int(double, double, int, int)>& f) const {
        const int a = std::max(r_.x, static_cast<int>(std::floor(ox_ + x0 * s_))),
                  b = std::min(r_.x + r_.w, static_cast<int>(std::ceil(ox_ + x1 * s_)));
        const int c = std::max(r_.y, static_cast<int>(std::floor(oy_ + y0 * s_))),
                  d = std::min(r_.y + r_.h, static_cast<int>(std::ceil(oy_ + y1 * s_)));
        for (int y = c; y < d; ++y)
            for (int x = a; x < b; ++x) {
                const int col = f((x + 0.5 - ox_) / s_, (y + 0.5 - oy_) / s_, x, y);
                if (col >= 0) t_.pixel(x, y, static_cast<std::uint8_t>(col));
            }
    }
    void poligono(const std::vector<Q>& v, const std::function<int(double, double, int, int)>& f) const {
        double x0 = 1e9, y0 = 1e9, x1 = -1e9, y1 = -1e9;
        for (const auto& q : v) { x0 = std::min(x0, q.x); y0 = std::min(y0, q.y); x1 = std::max(x1, q.x); y1 = std::max(y1, q.y); }
        riempi(x0, y0, x1, y1, [&](double x, double y, int px_, int py_) { return nel_poligono(v, x, y) ? f(x, y, px_, py_) : -1; });
    }
    void ellisse(double cx, double cy, double rx_, double ry_, const std::function<int(double, double, int, int)>& f) const {
        riempi(cx - rx_, cy - ry_, cx + rx_, cy + ry_, [&](double x, double y, int px_, int py_) {
            const double u = (x - cx) / rx_, v = (y - cy) / ry_;
            return u * u + v * v <= 1 ? f(x, y, px_, py_) : -1;
        });
    }
    void tratto(Q a, Q b, double sp, int col) const {
        riempi(std::min(a.x, b.x) - sp, std::min(a.y, b.y) - sp, std::max(a.x, b.x) + sp, std::max(a.y, b.y) + sp,
               [&](double x, double y, int, int) {
                   const double ex = b.x - a.x, ey = b.y - a.y, l2 = std::max(ex * ex + ey * ey, 1e-9);
                   const double k = std::clamp(((x - a.x) * ex + (y - a.y) * ey) / l2, 0.0, 1.0);
                   return std::hypot(x - a.x - k * ex, y - a.y - k * ey) <= sp / 2 ? col : -1;
               });
    }

private:
    Tela& t_;
    Rettangolo r_;
    double s_, ox_ = 0, oy_ = 0;
};

int stella_in(double x, double y, double cx, double cy, double r, int col) {
    const double dx = x - cx, dy = y - cy, rr = std::hypot(dx, dy);
    if (rr > r) return -1;
    const double a = std::atan2(dx, -dy);
    const double f = std::fmod(a + 2 * PI + PI / 5, 2 * PI / 5) - PI / 5;
    const double lim = r * (0.42 + 0.58 * std::pow(std::fabs(std::cos(f * 2.5)), 6.0));
    return rr <= std::max(lim, r * 0.42) ? col : -1;
}

std::vector<double> galloni_usn(int g) {   // dal polsino in su: 2 medio, 1 sottile, 5 largo
    switch (std::clamp(g, 1, 10)) {
        case 1: return {2};
        case 2: return {2, 1};
        case 3: return {2, 2};
        case 4: return {2, 1, 2};
        case 5: return {2, 2, 2};
        case 6: return {2, 2, 2, 2};
        case 7: return {5};
        case 8: return {5, 2};
        case 9: return {5, 2, 2};
        default: return {5, 2, 2, 2};
    }
}

// ================================ testa in 3D a distanze con segno ================================

struct V {
    double x = 0, y = 0, z = 0;
};
V operator+(V a, V b) { return {a.x + b.x, a.y + b.y, a.z + b.z}; }
V operator-(V a, V b) { return {a.x - b.x, a.y - b.y, a.z - b.z}; }
V operator*(V a, double k) { return {a.x * k, a.y * k, a.z * k}; }
double dot(V a, V b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
double len(V a) { return std::sqrt(dot(a, a)); }
V norm(V a) { return a * (1.0 / std::max(len(a), 1e-12)); }

double smin(double a, double b, double k) {
    const double h = std::clamp(0.5 + 0.5 * (b - a) / k, 0.0, 1.0);
    return b * (1 - h) + a * h - k * h * (1 - h);
}
double smax(double a, double b, double k) { return -smin(-a, -b, k); }
double ell(V p, V r) {
    const V a{p.x / r.x, p.y / r.y, p.z / r.z}, b{p.x / (r.x * r.x), p.y / (r.y * r.y), p.z / (r.z * r.z)};
    const double k0 = len(a), k1 = len(b);
    return k1 < 1e-12 ? -std::min({r.x, r.y, r.z}) : k0 * (k0 - 1) / k1;
}
double sfera(V p, double r) { return len(p) - r; }
double cono_tondo(V p, V a, V b, double r1, double r2) {   // iq, sdRoundCone fra due punti
    const V ba = b - a;
    const double l2 = dot(ba, ba), rr = r1 - r2, a2 = l2 - rr * rr, il2 = 1.0 / l2;
    const V pa = p - a;
    const double y = dot(pa, ba), z = y - l2;
    const V xv = pa * l2 - ba * y;
    const double x2 = dot(xv, xv), y2 = y * y * l2, z2 = z * z * l2;
    const double k = (rr >= 0 ? 1 : -1) * rr * rr * x2;
    if ((z >= 0 ? 1 : -1) * a2 * z2 > k) return std::sqrt(x2 + z2) * il2 - r2;
    if ((y >= 0 ? 1 : -1) * a2 * y2 < k) return std::sqrt(x2 + y2) * il2 - r1;
    return (std::sqrt(x2 * a2 * il2) + y * rr) * il2 - r1;
}
double scatola(V p, V b) {
    const V q{std::fabs(p.x) - b.x, std::fabs(p.y) - b.y, std::fabs(p.z) - b.z};
    return len({std::max(q.x, 0.0), std::max(q.y, 0.0), std::max(q.z, 0.0)}) + std::min(std::max({q.x, q.y, q.z}), 0.0);
}

enum Materiale { PELLE_M, OCCHIO_M, CAPELLI_M, CALOTTA_M, FASCIA_M, VISIERA_M, NESSUNO_M };

struct Testa {
    // lineamenti (moltiplicatori attorno a 1)
    double jw = 1, fl = 1, cw = 1, cb = 1, nl = 1, nw = 1, es = 0.27, lab = 1, orecchio = 1, arcata = 1;
    double imbardata = 0, beccheggio = 0;
    int pettinatura = 0;   // 0 riga di lato, 1 a spazzola, 2 pettinati indietro
    double volume = 1, stempiatura = 0;
    bool berretto = false, usn = true;
    double ym() const { return -0.27 * nl - 0.15; }   // bocca
    V occhio(int s) const { return {s * es, 0.055, 0.58}; }

    double testa(V q) const {
        // simmetria: i lineamenti doppi si calcolano sul lato della x positiva; ogni gruppo si salta quando il suo volume
        // di contenimento e' piu' lontano della distanza gia' trovata (piu' il raccordo)
        const V m{std::fabs(q.x), q.y, q.z};
        double d = ell(q - V{0, 0.16, -0.04}, {0.74, 0.86, 0.84});
        d = smin(d, ell(q - V{0, -0.17 * fl, 0.16}, {0.58 * jw, 0.56 * fl, 0.68}), 0.22);
        d = smin(d, ell(q - V{0, 0.42, 0.28}, {0.62, 0.36, 0.5}), 0.15);   // fronte
        d = smin(d, ell(m - V{0.32 * jw, -0.4 * fl, -0.04}, {0.22, 0.24, 0.4}), 0.2);   // mascella
        if (len(m - V{0.36, -0.06, 0.48}) - 0.25 < d + 0.12) d = smin(d, ell(m - V{0.36, -0.06, 0.48}, V{0.19, 0.11, 0.2} * cb), 0.12);
        if (len(q - V{0, -0.64 * fl, 0.6}) - 0.22 < d + 0.16) d = smin(d, ell(q - V{0, -0.64 * fl, 0.6}, {0.19 * cw, 0.13, 0.2}), 0.16);   // mento
        if (len(q - V{0, 0.21, 0.6}) - 0.52 < d + 0.1) d = smin(d, ell(q - V{0, 0.21, 0.6}, {0.5, 0.08 + 0.03 * arcata, 0.14}), 0.1);   // arcata
        if (len(m - V{es, 0.07, 0.77}) - 0.135 < 0.09) d = smax(d, -sfera(m - V{es, 0.07, 0.77}, 0.135), 0.09);   // orbite
        // naso: dorso, punta, ali
        if (len(q - V{0, -0.08, 0.86}) - 0.36 < d + 0.06) {
            d = smin(d, cono_tondo(q, {0, 0.13, 0.7}, {0, -0.24 * nl, 0.97}, 0.05, 0.075 * nw), 0.06);
            d = smin(d, sfera(q - V{0, -0.25 * nl, 0.92}, 0.085 * nw), 0.04);
            d = smin(d, sfera(m - V{0.1 * nw, -0.31 * nl, 0.83}, 0.068), 0.04);
        }
        // labbra e fessura della bocca
        const double y = ym();
        if (len(q - V{0, y - 0.02, 0.8}) - 0.24 < d + 0.04) {
            d = smin(d, ell(q - V{0, y + 0.02, 0.78}, {0.17 * lab, 0.04, 0.06}), 0.04);
            d = smin(d, ell(q - V{0, y - 0.06, 0.765}, {0.14 * lab, 0.048, 0.065}), 0.04);
            d = smax(d, -scatola(q - V{0, y - 0.02, 0.84}, {0.15 * lab, 0.006, 0.12}), 0.012);
        }
        // orecchie con la conca
        if (len(m - V{0.76, 0.0, -0.06}) - 0.28 < d + 0.04) {
            double o = ell(m - V{0.74, 0.0, -0.06}, V{0.07, 0.2 * orecchio, 0.13});
            o = smax(o, -ell(m - V{0.79, 0.0, -0.04}, {0.05, 0.12 * orecchio, 0.08}), 0.02);
            d = smin(d, o, 0.04);
        }
        d = smin(d, cono_tondo(q, {0, -0.62, -0.24}, {0, -2.6, -0.1}, 0.36, 0.46), 0.07);   // collo
        return d;
    }
    double capelli(V q) const {
        const double v = pettinatura == 1 ? 0.4 : volume;
        double sh = ell(q - V{0, 0.17 + 0.02 * v, -0.05}, {0.755 + 0.015 * v, 0.88 + 0.035 * v, 0.865 + 0.015 * v});
        if (sh > 0.06) return sh;   // lontano dai capelli: niente attaccatura ne' ciocche
        const double a = std::atan2(q.x, q.z);   // 0 davanti
        double linea = 0.66 - 0.52 * (1 - std::cos(a)) + 0.03 * (rumore(a * 14.0, 0.5, 19) - 0.5);
        linea -= 0.2 * std::exp(-std::pow((std::fabs(a) - 1.32) / 0.14, 2.0));   // basette
        linea += stempiatura * (0.16 * std::pow(std::sin(2 * a), 2.0) * (std::cos(a) > 0 ? 1 : 0) + 0.08 * std::max(0.0, std::cos(a)));
        sh -= 0.012 * (rumore(a * 9.0, q.y * 12.0, 17) - 0.5);   // ciocche
        if (pettinatura == 0) sh -= 0.02 * v * std::clamp(-q.x * 3.0, 0.0, 1.0) * std::clamp((q.y - 0.3) * 4.0, 0.0, 1.0);
        double h = std::max(sh, linea - q.y);
        if (pettinatura == 0) h = smax(h, -scatola(q - V{0.24, 0.7, 0.1}, {0.012, 0.3, 0.6}), 0.01);   // riga
        return h;
    }
    double berretto_sdf(V q, int& m) const {
        // fascia (cilindro un po' ellittico), calotta schiacciata (rialzata davanti nella VMF), visiera inclinata
        const double fr = std::sqrt(q.x * q.x / (0.84 * 0.84) + q.z * q.z / (0.88 * 0.88)) - 1.0;
        const double fascia = std::max(fr * 0.84, std::fabs(q.y - 0.52) - 0.1);
        const double alto = usn ? 0.0 : 0.05, avanti = usn ? 0.04 : 0.12;
        double calotta = ell(q - V{0, 0.7 + alto, avanti}, {1.0, 0.2 + alto * 1.4, 1.04});
        calotta = std::max(calotta, 0.56 - q.y);
        const V vq{q.x, q.y - 0.42 + 0.3 * (q.z - 0.7), q.z - 0.7};
        const double visiera = std::max(ell(vq, {0.6, 0.022, 0.42}), 0.55 - q.z);
        double d = fascia;
        m = FASCIA_M;
        if (calotta < d) { d = calotta; m = CALOTTA_M; }
        if (visiera < d) { d = visiera; m = VISIERA_M; }
        return d;
    }
    double mappa(V q, int& m) const {
        m = PELLE_M;
        // lontano dalla testa basta un volume che la contiene (il collo scende sotto y = -0,7)
        if (q.y > -0.7) {
            const double b = len(q - V{0, 0.12, 0.05}) - 1.42;
            if (b > 0.2) return b;
        } else {
            const double b = std::hypot(q.x, q.z + 0.1) - 0.6;
            if (b > 0.2) return b;
        }
        double d = testa(q);
        for (const int s : {-1, 1}) {
            const V c = occhio(s);
            if (s * q.x < 0 && std::fabs(q.x) > 0.05) continue;   // solo l'occhio dal proprio lato
            if (len(q - c) - 0.14 > d) continue;
            const double e = sfera(q - c, 0.112);
            if (e < d) { d = e; m = OCCHIO_M; }
            // palpebre: gusci sopra e sotto il bulbo
            const double dx2 = (q.x - c.x) * (q.x - c.x);
            const double sopra = std::max(sfera(q - c, 0.124), -(q.y - (c.y + 0.026 - 0.01 * s * (q.x - c.x) * 0 + 4.5 * dx2)));
            const double sotto = std::max(sfera(q - c, 0.119), q.y - (c.y - 0.028 + 3.0 * dx2));
            const double p = std::min(sopra, sotto);
            if (p < d) { d = p; m = PELLE_M; }
        }
        const double h = capelli(q);
        if (h < d) { d = h; m = CAPELLI_M; }
        if (berretto) {
            int mb = 0;
            const double b = berretto_sdf(q, mb);
            if (b < d) { d = b; m = mb; }
        }
        return d;
    }
    double mappa(V q) const {
        int m = 0;
        return mappa(q, m);
    }
    V normale(V p) const {
        const double e = 0.0015;   // quattro valutazioni ai vertici di un tetraedro
        const V a{1, -1, -1}, b{-1, -1, 1}, c{-1, 1, -1}, d{1, 1, 1};
        return norm(a * mappa(p + a * e) + b * mappa(p + b * e) + c * mappa(p + c * e) + d * mappa(p + d * e));
    }
    double ombra(V ro, V rd) const {   // ombra morbida (iq)
        double r = 1.0, t = 0.02;
        for (int i = 0; i < 20 && t < 1.4; ++i) {
            const double h = mappa(ro + rd * t);
            if (h < 0.0005) return 0.0;
            r = std::min(r, 10.0 * h / t);
            t += std::clamp(h, 0.02, 0.25);
        }
        return std::clamp(r, 0.0, 1.0);
    }
    double occlusione(V p, V n) const {
        double o = 0, k = 1;
        for (int i = 1; i <= 3; ++i) {
            const double h = 0.05 * i;
            o += k * (h - mappa(p + n * h));
            k *= 0.6;
        }
        return std::clamp(1.0 - 2.4 * o, 0.0, 1.0);
    }
    // dal riferimento del mondo (x a destra, y in su, z verso chi guarda) a quello della testa, e ritorno
    V in_testa(V p) const {
        const double cy = std::cos(imbardata), sy = std::sin(imbardata), cp = std::cos(beccheggio), sp = std::sin(beccheggio);
        const V a{cy * p.x - sy * p.z, p.y, sy * p.x + cy * p.z};
        return {a.x, cp * a.y - sp * a.z, sp * a.y + cp * a.z};
    }
};

Testa testa_da(const ParametriRitratto& par) {
    const auto& v = par.volto;
    const auto k = [&](int i) { return caso(static_cast<int>(v.seme), i, 77); };
    Testa t;
    t.jw = 0.82 + 0.16 * k(1);
    t.fl = 0.94 + 0.12 * k(2);
    t.cw = 0.85 + 0.35 * k(3);
    t.cb = 0.85 + 0.3 * k(4);
    t.nl = 0.88 + 0.24 * k(5);
    t.nw = 0.85 + 0.35 * k(6);
    t.es = 0.25 + 0.04 * k(7);
    t.lab = 0.85 + 0.3 * k(8);
    t.orecchio = 0.9 + 0.25 * k(9);
    t.arcata = 0.6 + 0.8 * k(10);
    t.imbardata = (k(11) < 0.5 ? -1 : 1) * radianti(3.0 + 7.0 * k(12));
    t.beccheggio = radianti(-2.0 + 4.0 * k(13));
    t.pettinatura = static_cast<int>(k(14) * 3);
    t.volume = 0.4 + 0.7 * k(15);
    t.stempiatura = std::clamp((v.eta - 38) / 20.0, 0.0, 1.0) * (0.3 + 0.7 * k(16));
    if (par.girata_gradi > -900) t.imbardata = radianti(par.girata_gradi);
    t.berretto = par.berretto;
    t.usn = par.marina == Marina::usn;
    return t;
}

// pixel della testa: indice di palette o -1 (nessuna superficie)
struct Campione {
    int mat = NESSUNO_M;
    double luce = 0;
    V q;   // punto sulla superficie, in coordinate della testa
    V n;
};

Campione campiona(const Testa& h, double X, double Y) {
    Campione c;
    // raggio ortografico da davanti
    V ro = h.in_testa({X, Y, 3.0}), rd = h.in_testa({0, 0, -1});
    // scatola che contiene testa, collo e berretto
    double t = 1.6;   // niente davanti a z = 1,4 (punta del naso 1,0, visiera 1,15)
    for (int i = 0; i < 90 && t < 4.6; ++i) {
        const V p = ro + rd * t;
        int m = 0;
        const double d = h.mappa(p, m);
        if (d < 0.0008) {
            c.mat = m;
            c.q = p;
            break;
        }
        t += d * 0.92;
    }
    if (c.mat == NESSUNO_M) return c;
    const V p = c.q, n = h.normale(p);
    c.n = n;
    const V vista = rd * -1.0;
    const V L1 = norm(h.in_testa({-0.7, 0.55, 0.55})), L2 = norm(h.in_testa({0.75, -0.05, 0.45})), L3 = norm(h.in_testa({0.7, 0.35, -0.6}));
    const double sh = h.ombra(p + n * 0.006, L1);
    const double ao = h.occlusione(p, n);
    const double dif = std::max(0.0, dot(n, L1)) * (0.12 + 0.88 * sh);
    const double fill = std::max(0.0, dot(n, L2));
    const double rim = std::pow(1.0 - std::max(0.0, dot(n, vista)), 3.0) * std::max(0.0, dot(n, L3) + 0.3);
    const V rif = n * (2.0 * dot(n, L1)) - L1;
    double lucido = 0.12, esp = 14;
    if (c.mat == VISIERA_M) { lucido = 0.9; esp = 40; }
    if (c.mat == OCCHIO_M) { lucido = 0.8; esp = 60; }
    if (c.mat == CAPELLI_M) { lucido = 0.25; esp = 10; }
    if (c.mat == PELLE_M) { lucido = 0.22; esp = 22; }
    const double spec = lucido * std::pow(std::max(0.0, dot(rif, vista)), esp) * sh;
    c.luce = (0.1 + 1.0 * dif + 0.16 * fill + 0.3 * rim) * (0.4 + 0.6 * ao) + spec;
    return c;
}

// colore di un pixel della testa: materiale m, luce L, punto q sulla superficie (coordinate della testa)
int colore_testa(const Testa& h, const ParametriRitratto& par, int m, double L, V q, int px, int py) {
    const bool usn = par.marina == Marina::usn;
    const int eta = par.volto.eta;
    const int seme = static_cast<int>(par.volto.seme);
    const double ax = std::fabs(q.x);
    int col = -1;
    auto tono = [&](int primo, int n, double k) { return primo + retina(std::pow(std::clamp(L * k, 0.0, 1.15), 0.8) * (n - 1), px, py, n - 1); };
    switch (m) {
        case PELLE_M: {
            col = tono(PELLE, N_PELLE, 0.98);
            const double ym = h.ym();
            // linea delle ciglia sul bordo della palpebra e piega sopra
            bool ciglia = false;
            for (const int s : {-1, 1}) {
                const V c = h.occhio(s);
                const double dx = q.x - c.x, bordo = c.y + 0.026 + 4.5 * dx * dx;
                if (std::fabs(dx) < 0.105 && q.z > c.z - 0.02) {
                    if (q.y > bordo - 0.004 && q.y < bordo + 0.016) { col = NERO + (std::fabs(dx) > 0.075 ? 3 : 1); ciglia = true; }
                    else if (std::fabs(q.y - (bordo + 0.06 - 1.0 * dx * dx)) < 0.007 && std::fabs(dx) < 0.09) col = std::max(PELLE, col - 2);
                }
            }
            if (ciglia) break;
            // sopracciglia
            const double yb = 0.235 + 0.035 * std::sin(std::clamp((ax - 0.08) / 0.36, 0.0, 1.0) * PI) - 0.02 * (ax > 0.3);
            const double spess = 0.024 + 0.01 * h.arcata;
            if (ax > 0.07 && ax < 0.46 && std::fabs(q.y - yb) < spess * (1.0 - 0.6 * std::clamp((ax - 0.3) / 0.16, 0.0, 1.0)) && q.z > 0.4)
                col = CAPELLI + retina(std::clamp(L * 0.55, 0.0, 1.0) * (N_CAPELLI - 1) + 0.5 * (rumore(q.x * 60, q.y * 20, 3) - 0.5), px, py, N_CAPELLI - 1);
            // labbra
            else if (ax < 0.15 * h.lab * (1.0 - 0.3 * std::fabs(q.y - ym + 0.02) / 0.06) && q.y < ym + 0.035 && q.y > ym - 0.085 && q.z > 0.66)
                col = std::fabs(q.y - (ym - 0.02)) < 0.008 ? LABBRA + 0 : tono(LABBRA, N_LABBRA, q.y > ym - 0.02 ? 0.8 : 1.05);
            // baffi
            else if (par.volto.baffi && q.y > ym + 0.04 && q.y < ym + 0.12 - 0.2 * ax && ax < 0.24 && q.z > 0.72)
                col = CAPELLI + retina(std::clamp(L * 0.7, 0.0, 1.0) * (N_CAPELLI - 1) + 0.8 * (rumore(q.x * 50, q.y * 12, 4) - 0.5), px, py, N_CAPELLI - 1);
            else {
                // guance e punta del naso un po' rosate, orecchie in ombra calde
                const double rosa = std::max(0.0, 1.0 - std::hypot(ax - 0.34, q.y + 0.2) / 0.13) * (q.z > 0.4 ? 1 : 0);
                if (bayer(px, py) < 0.0 * rosa) col = GUANCE + retina(std::clamp(L, 0.0, 1.0) * (N_GUANCE - 1), px, py, N_GUANCE - 1);
                // rughe con l'eta': fronte, zampe di gallina, solchi naso-labiali
                double ruga = 0;
                if (eta >= 45 && ax < 0.32 && q.z > 0.5)
                    for (const double yr : {0.4, 0.47}) ruga = std::max(ruga, 1.0 - std::fabs(q.y - yr - 0.02 * std::cos(q.x * 8)) / 0.008);
                if (eta >= 42) {
                    const double xs = ax - (0.12 * h.nw + 0.08 * std::clamp((-0.31 * h.nl - q.y) / 0.2, 0.0, 1.0));
                    if (q.y < -0.3 * h.nl && q.y > ym - 0.04 && q.z > 0.6) ruga = std::max(ruga, 0.8 * (1.0 - std::fabs(xs) / 0.012));
                }
                if (eta >= 48 && ax > 0.36 && ax < 0.46 && std::fabs(q.y - 0.06 + 0.3 * (ax - 0.36)) < 0.008) ruga = 0.7;
                if (ruga > 0.3) col = PELLE + retina(std::clamp(L * 0.7, 0.0, 1.0) * (N_PELLE - 1), px, py, N_PELLE - 1);
                // barba appena rasata sulle guance e sul mento
                if ((caso(seme, 31) < 0.5 || !usn) && q.y < ym + 0.08 && q.z > 0.1 && ax > 0.05 && bayer(px, py) < 0.35 &&
                    !(ax < 0.2 && q.y > ym - 0.12))
                    col = std::max(PELLE, col - 1);
            }
            break;
        }
        case OCCHIO_M: {
            // iride rivolta verso chi guarda, pupilla, riflesso; il bianco in ombra sotto la palpebra
            const int s = q.x < 0 ? -1 : 1;
            const V c = h.occhio(s);
            const V dir = norm(q - c), sguardo = norm(h.in_testa({0, 0, 1}));
            const double ang = std::acos(std::clamp(dot(dir, sguardo), -1.0, 1.0));
            if (ang < 0.17) col = NERO + 0;
            else if (ang < 0.42) col = IRIDE + retina((ang < 0.36 ? 2.6 : 1.0) * std::clamp(L, 0.3, 1.0), px, py, N_IRIDE - 1);
            else col = BIANCO + retina(std::clamp(L * 0.5 - 0.15, 0.0, 1.0) * (N_BIANCO - 1), px, py, N_BIANCO - 1);
            if (L > 1.0 && ang < 0.45) col = BIANCO + 4;   // riflesso della luce
            break;
        }
        case CAPELLI_M: {
            const double fili = rumore(q.x * 26 + q.z * 26 + q.y * 3, q.y * 6 - q.x * 2, 13) - 0.5;
            const double ciocche = rumore(q.x * 7 + q.z * 5, q.y * 3, 14) - 0.5;
            const double a = std::atan2(q.x, q.z);
            const double radici = std::clamp((q.y - (0.66 - 0.52 * (1 - std::cos(a)))) / 0.06, 0.0, 1.0);   // vicino all'attaccatura
            col = CAPELLI + retina(std::clamp(L, 0.0, 1.25) * (N_CAPELLI - 1) * (0.55 + 0.45 * radici) + 1.4 * fili + 0.9 * ciocche, px, py, N_CAPELLI - 1);
            break;
        }
        case CALOTTA_M:
            col = usn ? tono(BIANCO, N_BIANCO, 1.05) : tono(NERO, N_NERO, 0.9);
            // fregio sul davanti
            if (q.z > 0.5 && std::fabs(q.x) < 0.2 && q.y > 0.56 && q.y < 0.8) {
                const double fx = q.x / 0.2, fy = (q.y - 0.68) / 0.12;
                const double r2 = fx * fx + fy * fy;
                if (usn && r2 < 1.0) col = r2 < 0.3 ? ARGENTO + retina(std::clamp(L, 0.0, 1.0) * 5, px, py, 5) : ORO + retina(std::clamp(L, 0.0, 1.0) * 7, px, py, 7);
                if (!usn && r2 < 1.0) col = r2 < 0.25 ? SMALTO + 4 : (r2 > 0.55 ? ORO + retina(std::clamp(L, 0.0, 1.0) * 7 + ((px + py) % 2 ? 0.8 : -0.8), px, py, 7) : NERO + 1);
            }
            break;
        case FASCIA_M:
            col = tono(NERO, N_NERO, 0.8);
            if (!usn && q.y > 0.6 && q.z > 0.0) col = tono(ORO, N_ORO, 1.0);   // cordoncino dorato
            if (!usn && q.z > 0.55 && std::fabs(q.x) < 0.24 && q.y > 0.44) {   // il «granchio» sulla fascia
                const double fx = q.x / 0.24, fy = (q.y - 0.52) / 0.09;
                if (fx * fx + fy * fy < 1.0) col = fx * fx + fy * fy < 0.3 ? SMALTO + 4 : ORO + retina(std::clamp(L, 0.0, 1.0) * 7, px, py, 7);
            }
            break;
        case VISIERA_M: {
            col = tono(NERO, N_NERO, 1.0);
            // foglie di quercia sul bordo della visiera (USN dal capitano di fregata, VMF ufficiali superiori)
            const bool foglie = par.grado >= 5;
            if (foglie && q.z > 0.82 && std::fabs(q.x) > 0.05 && std::fmod(std::fabs(q.x) + 10, 0.1) < 0.06)
                col = ORO + retina(std::clamp(L * 1.2, 0.0, 1.0) * 7, px, py, 7);
            break;
        }
        default: break;
    }
    return col;
}

}  // namespace

void prepara_palette_ritratto(Palette& p, const ParametriRitratto& r, const TavolaDecorazioni* tavola) {
    imposta_palette(p, r.marina);
    if (tavola) tavola->imposta_palette(p);
    imposta_capelli(p, r.volto.capelli, std::clamp((r.volto.eta - 38) / 22.0, 0.0, 0.85));
    imposta_volto(p, r.volto.carnagione, r.volto.occhi);
}

void disegna_ritratto(Tela& t, Rettangolo r, const ParametriRitratto& par, const TavolaDecorazioni* tavola, bool fondale) {
    Pennello pn(t, r);
    const bool usn = par.marina == Marina::usn;
    const bool kaki = usn && par.uniforme == Uniforme::kaki;
    const int STOFFA = kaki ? KAKI : usn ? BLU : NERO;
    const Testa h = testa_da(par);

    // fondale dello studio: chiazze morbide, piu' chiaro dietro la testa
    if (fondale)
        pn.riempi(0, 0, RW, RH, [&](double x, double y, int px, int py) {
            const double v = 1.0 - 0.95 * std::hypot((x - 64) / RW, (y - 70) / RH);
            const double n = rumore(x / 26.0, y / 26.0, 5) * 0.6 + rumore(x / 9.0, y / 9.0, 6) * 0.25;
            return SFONDO + retina(0.3 + 6.4 * v * (0.5 + n), px, py, N_SFONDO - 1);
        });

    // --- testa e collo: un campione al centro di ogni pixel, poi quattro dove il materiale cambia (bordi, occhi, labbra)
    const auto& ar = pn.area();
    const int x0 = std::max(ar.x, pn.px(CX - 1.25 * U)), x1 = std::min(ar.x + ar.w, pn.px(CX + 1.25 * U) + 1);
    const int y0 = std::max(ar.y, pn.py(HY - 1.35 * U)), y1 = std::min(ar.y + ar.h, pn.py(156) + 1);   // sotto c'e' l'uniforme
    const int gw = x1 - x0, gh = y1 - y0;
    std::vector<Campione> griglia(static_cast<std::size_t>(std::max(gw, 0) * std::max(gh, 0)));
    auto in_mondo = [&](double px, double py) {
        return std::pair{(pn.rx(0) + px / pn.scala() - 0.5 / pn.scala() - CX) / U, (HY - (pn.ry(0) + py / pn.scala() - 0.5 / pn.scala())) / U};
    };
    for (int py = y0; py < y1; ++py)
        for (int px = x0; px < x1; ++px) {
            const auto [X, Y] = in_mondo(px + 0.5, py + 0.5);
            griglia[static_cast<std::size_t>((py - y0) * gw + (px - x0))] = campiona(h, X, Y);
        }
    auto mat_a = [&](int px, int py) {
        if (px < x0 || py < y0 || px >= x1 || py >= y1) return static_cast<int>(NESSUNO_M);
        return griglia[static_cast<std::size_t>((py - y0) * gw + (px - x0))].mat;
    };
    for (int py = y0; py < y1; ++py)
        for (int px = x0; px < x1; ++px) {
            int conta[NESSUNO_M + 1] = {};
            double luce[NESSUNO_M + 1] = {};
            Campione ultimo[NESSUNO_M + 1];
            const Campione& centro = griglia[static_cast<std::size_t>((py - y0) * gw + (px - x0))];
            const int mc = centro.mat;
            const bool sul_bordo = mat_a(px - 1, py) != mc || mat_a(px + 1, py) != mc || mat_a(px, py - 1) != mc || mat_a(px, py + 1) != mc;
            const int ss = sul_bordo && pn.scala() < 2.5 ? 2 : 1;
            if (ss == 1) {
                conta[mc] = 1;
                luce[mc] = centro.luce;
                ultimo[mc] = centro;
            } else {
                for (int sy = 0; sy < 2; ++sy)
                    for (int sx = 0; sx < 2; ++sx) {
                        const auto [X, Y] = in_mondo(px + (sx + 0.5) / 2, py + (sy + 0.5) / 2);
                        const Campione c = campiona(h, X, Y);
                        ++conta[c.mat];
                        luce[c.mat] += c.luce;
                        ultimo[c.mat] = c;
                    }
            }
            int m = NESSUNO_M, best = 0;
            for (int k = 0; k < NESSUNO_M; ++k)
                if (conta[k] > best) { best = conta[k]; m = k; }
            if (m == NESSUNO_M || conta[m] * 2 < ss * ss) continue;
            const double L = luce[m] / conta[m];
            const int col = colore_testa(h, par, m, L, ultimo[m].q, px, py);
            if (col >= 0) t.pixel(px, py, static_cast<std::uint8_t>(col));
        }

    // contorno della testa: un filo scuro dove la testa confina con il fondale, come negli sprite d'epoca
    {
        std::vector<std::uint8_t> testa_px(static_cast<std::size_t>((x1 - x0) * (y1 - y0)), 0);
        for (int py = y0; py < y1; ++py)
            for (int px = x0; px < x1; ++px) {
                const int c = t.at(px, py);
                testa_px[static_cast<std::size_t>((py - y0) * (x1 - x0) + (px - x0))] = !(c >= SFONDO && c < SFONDO + N_SFONDO);
            }
        auto dentro = [&](int px, int py) {
            if (px < x0 || py < y0 || px >= x1 || py >= y1) return false;
            return testa_px[static_cast<std::size_t>((py - y0) * (x1 - x0) + (px - x0))] != 0;
        };
        for (int py = y0; py < y1; ++py)
            for (int px = x0; px < x1; ++px)
                if (dentro(px, py) && (!dentro(px - 1, py) || !dentro(px + 1, py) || !dentro(px, py - 1))) {
                    const int c = t.at(px, py);
                    int scuro = c;
                    for (const auto& [primo, n] : {std::pair{PELLE, N_PELLE}, {CAPELLI, N_CAPELLI}, {NERO, N_NERO}, {BIANCO, N_BIANCO}, {ORO, N_ORO}})
                        if (c >= primo && c < primo + n) scuro = std::max(primo, c - (primo == BIANCO ? 2 : 3));
                    t.pixel(px, py, static_cast<std::uint8_t>(scuro));
                }
    }

    if (par.solo_testa) return;
    // --- uniforme (sopra il collo) ---
    const std::vector<Q> giacca = {{0, 232}, {0, 170}, {12, 158}, {38, 147}, {64, 139}, {112, 139}, {138, 147}, {164, 158}, {176, 170}, {176, 232}};
    pn.poligono(giacca, [&](double x, double y, int px, int py) {
        const double u = (x - CX) / 88.0;
        double tt = 4.4 - 2.0 * u - 0.008 * (y - 150);
        // spalle in luce, ombra sotto le spalle e nei fianchi
        if (y < 175) tt += 1.2 * (1 - (y - 147) / 28.0) * (1 - std::fabs(u));
        tt += 0.5 * (rumore(x / 14.0, y / 30.0, 21) - 0.5);
        if (std::fabs(u) > 0.84) tt -= 1.4;
        return STOFFA + retina(tt, px, py, 7);
    });
    // contorno delle spalle
    pn.tratto({12, 158}, {38, 147}, 1.0, STOFFA + 7);
    pn.tratto({38, 147}, {64, 139}, 1.0, STOFFA + 6);
    pn.tratto({112, 139}, {138, 147}, 1.0, STOFFA + 3);
    pn.tratto({138, 147}, {164, 158}, 1.0, STOFFA + 2);

    if (!kaki) {
        // camicia bianca nella V, cravatta nera col nodo
        const double fondo_v = 236;
        pn.poligono({{64, 139}, {112, 139}, {CX, fondo_v}}, [&](double x, double y, int px, int py) {
            return BIANCO + retina(3.7 - 1.1 * (x - CX) / 24.0 - 0.006 * (y - 147), px, py, N_BIANCO - 1);
        });
        pn.poligono({{CX - 6, 150}, {CX + 6, 150}, {CX + 5, 162}, {CX + 8, 222}, {CX, 228}, {CX - 8, 222}, {CX - 5, 162}},
                    [&](double x, double y, int px, int py) {
                        return NERO + retina(2.4 - 0.25 * (x - CX) + (y < 170 ? 1.0 : 0.0) - 0.004 * (y - 170), px, py, 7);
                    });
        // colletto della camicia
        for (const int s : {-1, 1})
            pn.poligono({{CX + s * 20, 135}, {CX + s * 5, 152}, {CX + s * 10, 158}, {CX + s * 25, 144}},
                        [&](double x, double y, int px, int py) { return BIANCO + retina(4.2 - 0.7 * (x - CX) / 20.0 - 0.05 * (y - 143), px, py, 4); });
        // risvolti (a lancia, USN; a punta piu' bassa, VMF) con le cuciture
        for (const int s : {-1, 1}) {
            const double p = usn ? 0 : 6;
            const std::vector<Q> risvolto = {{CX + s * 24, 139}, {CX + s * 40, 152}, {CX + s * 58, 168 + p}, {CX + s * 62, 160 + p},
                                             {CX + s * 54, 182 + p}, {CX + s * 46, 184 + p}, {CX + s * 6, 232}, {CX + s * 2, 232}};
            pn.poligono(risvolto, [&](double x, double y, int px, int py) {
                const double u = (x - CX) / 60.0;
                return STOFFA + retina(5.0 - 2.2 * u - 0.008 * (y - 150), px, py, 7);
            });
            pn.tratto({CX + s * 24, 139}, {CX + s * 2, 232}, 1.4, STOFFA + (s < 0 ? 7 : 1));
            pn.tratto({CX + s * 62, 160 + p}, {CX + s * 54, 182 + p}, 1.0, STOFFA + (s < 0 ? 7 : 2));
            pn.tratto({CX + s * 46, 184 + p}, {CX + s * 6, 232}, 1.2, STOFFA + 0);
            pn.tratto({CX + s * 54, 182 + p}, {CX + s * 46, 184 + p}, 1.0, STOFFA + 0);
        }
        // primi bottoni in fondo all'inquadratura
        for (const int s : {-1, 1})
            pn.ellisse(CX + s * 22, 226, 3.2, 3.2, [&](double x, double y, int px, int py) {
                const double l = -(x - CX - s * 22) * 0.35 - (y - 226) * 0.4;
                return ORO + retina(4.0 + 2.5 * l, px, py, 7);
            });
    } else {
        // camicia kaki col colletto aperto e i gradi metallici sulle punte
        pn.poligono({{70, 139}, {106, 139}, {CX, 168}}, [&](double, double, int px, int py) { return PELLE + retina(4.0, px, py, 9); });
        for (const int s : {-1, 1}) {
            pn.poligono({{CX + s * 18, 137}, {CX + s * 42, 148}, {CX + s * 30, 168}, {CX + s * 2, 176}},
                        [&](double x, double, int px, int py) { return KAKI + retina(5.4 - 1.4 * (x - CX) / 42.0, px, py, 7); });
            pn.tratto({CX + s * 2, 176}, {CX + s * 30, 168}, 1.0, KAKI + 1);
            const double gx = CX + s * 26, gy = 168;
            const int g = par.grado;
            const int met = (g == 1 || g == 4) ? ORO : ARGENTO, nm = met == ORO ? N_ORO : N_ARGENTO;
            if (g <= 3) {
                for (int k = 0; k < (g == 3 ? 2 : 1); ++k)
                    pn.riempi(gx - 2 + k * 4 * s, gy - 5, gx + 0.5 + k * 4 * s, gy + 5,
                              [&](double, double yy, int px, int py) { return met + retina((nm - 1) * (0.9 - 0.06 * (yy - gy + 5)), px, py, nm - 1); });
            } else if (g <= 5) {
                pn.ellisse(gx, gy, 4.0, 5.5, [&](double x, double yy, int px, int py) {
                    return met + retina((nm - 1) * (0.8 - 0.08 * (x - gx) - 0.04 * (yy - gy)) + ((px ^ py) & 1 ? 0.7 : -0.7), px, py, nm - 1);
                });
            } else {
                pn.ellisse(gx, gy, 6.0, 4.0, [&](double x, double, int px, int py) { return ARGENTO + retina(4.0 - 0.4 * (x - gx), px, py, 5); });
            }
        }
        for (const int s : {-1, 1})   // taschini con la patta
            pn.riempi(CX + s * 40 - 15, 196, CX + s * 40 + 15, 232, [&](double x, double y, int px, int py) {
                const bool bordo = std::fabs(x - (CX + s * 40)) > 14 || std::fabs(y - 204) < 0.7;
                return KAKI + retina(bordo ? 2.2 : 4.4 - 1.2 * s, px, py, 7);
            });
    }

    // --- controspalline (VMF) ---
    if (!usn)
        for (const int s : {-1, 1}) {
            const Q a{CX + s * 30, 143}, b{CX + s * 78, 160};
            const double ux = b.x - a.x, uy = b.y - a.y, lu = std::hypot(ux, uy);
            const double nxv = -uy / lu, nyv = ux / lu, mezza = 6.5;
            const std::vector<Q> v = {{a.x + nxv * mezza, a.y + nyv * mezza}, {b.x + nxv * mezza, b.y + nyv * mezza},
                                      {b.x - nxv * mezza, b.y - nyv * mezza}, {a.x - nxv * mezza, a.y - nyv * mezza}};
            const int g = par.grado;
            const int strisce = g <= 4 ? 1 : g <= 7 ? 2 : 0;
            const int stelle = g <= 4 ? g : g <= 7 ? g - 4 : g <= 10 ? g - 7 : 1;
            const bool grandi = g >= 5;
            pn.poligono(v, [&](double x, double y, int px, int py) {
                const double along = ((x - a.x) * ux + (y - a.y) * uy) / lu / lu, across = ((x - a.x) * nxv + (y - a.y) * nyv) / mezza;
                if (along < 0.08) return ORO + retina(3.0 - across, px, py, 7);   // bottone
                bool scura = false;
                if (strisce == 1) scura = std::fabs(across) < 0.16;
                if (strisce == 2) scura = std::fabs(std::fabs(across) - 0.44) < 0.13;
                if (scura) return NERO + 1;
                if (std::fabs(across) > 0.9) return ORO + 1;
                const double fili = ((px + py) % 2) ? 0.6 : -0.6;
                return ORO + retina(5.2 - 1.6 * across * s - 1.2 * along + fili, px, py, 7);
            });
            for (int k = 0; k < stelle; ++k) {
                const double f = (grandi ? 0.36 : 0.3) + (grandi ? 0.2 : 0.14) * k;
                const double sx = a.x + ux * f, sy = a.y + uy * f;
                const double rr = grandi ? 3.6 : 2.4;
                pn.riempi(sx - rr, sy - rr, sx + rr, sy + rr, [&](double x, double y, int px, int py) {
                    return stella_in(x, y, sx, sy, rr, ARGENTO) >= 0 ? ARGENTO + retina(4.6 - 0.4 * (x - sx), px, py, 5) : -1;
                });
            }
        }

    // --- petto sinistro (a destra nell'immagine): dolphins e barretta ---
    const double bx = CX + 44;
    const int nw = std::max(4, static_cast<int>(std::lround(14 * pn.scala()))), nh = std::max(2, static_cast<int>(std::lround(4 * pn.scala())));
    const double y_barretta = 214;
    Rettangolo ingombro{};
    if (tavola && !par.nastrini.empty() && !kaki)
        ingombro = tavola->disegna_barretta(t, pn.px(bx), pn.py(y_barretta), par.nastrini, Carriera::per_fila(par.marina), nw, nh);
    const double cima = kaki ? 192 : y_barretta - ingombro.h / pn.scala();
    if (usn && par.qualificato) {
        // dolphins d'oro: il sommergibile di prua con la vela, fra due delfini, sulle onde
        const double dy = cima - 7, w = 1.0;
        pn.ellisse(bx, dy, 7.0 * w, 3.0, [&](double x, double y, int px, int py) { return ORO + retina(6.2 - 0.5 * (x - bx) - 0.8 * (y - dy), px, py, 7); });
        pn.riempi(bx - 2, dy - 6.5, bx + 2, dy - 1, [&](double x, double, int px, int py) { return ORO + retina(6.6 - 0.8 * (x - bx), px, py, 7); });
        for (const int s : {-1, 1}) {
            pn.tratto({bx + s * 6, dy + 1}, {bx + s * 13, dy - 1.5}, 2.6, ORO + (s < 0 ? 6 : 4));
            pn.tratto({bx + s * 13, dy - 1.5}, {bx + s * 18, dy + 2}, 1.8, ORO + (s < 0 ? 5 : 3));
            pn.tratto({bx + s * 18, dy + 2}, {bx + s * 19, dy + 5}, 1.2, ORO + 3);
            pn.tratto({bx + s * 5, dy + 4}, {bx + s * 14, dy + 4.5}, 1.0, ORO + 2);   // onde
        }
    }
    // --- petto destro (a sinistra nell'immagine) ---
    const double dx = CX - 44;
    if (usn && par.comando && !kaki) {
        pn.ellisse(dx, 204, 6.5, 4.4, [&](double x, double y, int px, int py) {
            const bool st = stella_in(x, y, dx, 204, 3.4, ORO) >= 0;
            return st ? ARGENTO + 5 : ORO + retina(5.0 - 0.6 * (x - dx) - 0.6 * (y - 204), px, py, 7);
        });
    }
    if (!usn) {
        double y = 190;
        if (par.diploma) {   // rombo del diploma
            pn.poligono({{dx, y - 8}, {dx + 6, y}, {dx, y + 8}, {dx - 6, y}}, [&](double x, double yy, int px, int py) {
                const bool bordo = std::fabs(x - dx) / 6.0 + std::fabs(yy - y) / 8.0 > 0.72;
                return bordo ? ARGENTO + retina(4.2 - 0.5 * (x - dx), px, py, 5) : SMALTO_BLU + retina(3.6, px, py, 5);
            });
            y += 18;
        }
        if (par.comando) {   // «Comandante di sottomarino»: sagoma d'argento con la stella rossa
            pn.ellisse(dx, y, 15, 3.2, [&](double x, double, int px, int py) { return ARGENTO + retina(4.8 - 0.2 * (x - dx), px, py, 5); });
            pn.riempi(dx - 5, y - 8, dx + 2, y - 2, [&](double x, double, int px, int py) { return ARGENTO + retina(4.2 - 0.3 * (x - dx), px, py, 5); });
            pn.riempi(dx + 5, y - 6.5, dx + 13, y + 1.5, [&](double x, double yy, int, int) { return stella_in(x, yy, dx + 9, y - 2.5, 3.6, SMALTO + 4); });
        }
    }
}


// ================================ ritratto di gruppo ================================

namespace {

// testa piccola: un campione per pixel, i pixel della testa sopra il mento (il corpo copre il collo)
void testa_piccola(Tela& t, Rettangolo clip, double cx, double cy, double u, const Testa& h, const ParametriRitratto& par) {
    const int x0 = std::max(clip.x, static_cast<int>(cx - 1.3 * u)), x1 = std::min(clip.x + clip.w, static_cast<int>(cx + 1.3 * u) + 1);
    const int y0 = std::max(clip.y, static_cast<int>(cy - 1.5 * u)), y1 = std::min(clip.y + clip.h, static_cast<int>(cy + 1.05 * u) + 1);
    for (int py = y0; py < y1; ++py)
        for (int px = x0; px < x1; ++px) {
            const Campione c = campiona(h, (px + 0.5 - cx) / u, (cy - (py + 0.5)) / u);
            if (c.mat == NESSUNO_M) continue;
            const int col = colore_testa(h, par, c.mat, c.luce, c.q, px, py);
            if (col >= 0) t.pixel(px, py, static_cast<std::uint8_t>(col));
        }
}

// riempimento di un'ellisse e di un poligono in pixel della tela, con ritaglio
void ell_px(Tela& t, Rettangolo clip, double cx, double cy, double rx, double ry, const std::function<int(double, double, int, int)>& f) {
    for (int y = std::max(clip.y, static_cast<int>(cy - ry)); y < std::min(clip.y + clip.h, static_cast<int>(cy + ry) + 1); ++y)
        for (int x = std::max(clip.x, static_cast<int>(cx - rx)); x < std::min(clip.x + clip.w, static_cast<int>(cx + rx) + 1); ++x) {
            const double u = (x + 0.5 - cx) / rx, v = (y + 0.5 - cy) / ry;
            if (u * u + v * v > 1) continue;
            const int c = f(u, v, x, y);
            if (c >= 0) t.pixel(x, y, static_cast<std::uint8_t>(c));
        }
}
void pol_px(Tela& t, Rettangolo clip, const std::vector<Q>& v, const std::function<int(double, double, int, int)>& f) {
    double a = 1e9, b = 1e9, c = -1e9, d = -1e9;
    for (const auto& q : v) { a = std::min(a, q.x); b = std::min(b, q.y); c = std::max(c, q.x); d = std::max(d, q.y); }
    for (int y = std::max(clip.y, static_cast<int>(b)); y < std::min(clip.y + clip.h, static_cast<int>(d) + 1); ++y)
        for (int x = std::max(clip.x, static_cast<int>(a)); x < std::min(clip.x + clip.w, static_cast<int>(c) + 1); ++x)
            if (nel_poligono(v, x + 0.5, y + 0.5)) {
                const int col = f(x + 0.5, y + 0.5, x, y);
                if (col >= 0) t.pixel(x, y, static_cast<std::uint8_t>(col));
            }
}

enum class Ruolo { marinaio, ufficiale, comandante };

// una figura: testa in (cx, cy) con u pixel per unita' della testa; il busto scende fino al bordo
void figura(Tela& t, Rettangolo clip, double cx, double cy, double u, Ruolo ruolo, const ParametriEquipaggio& pe, std::uint32_t seme,
            bool ferito) {
    const bool usn = pe.marina == Marina::usn;
    const bool uff = ruolo != Ruolo::marinaio;
    ParametriRitratto par;
    par.marina = pe.marina;
    par.grado = ruolo == Ruolo::comandante ? pe.grado_comandante : 3;
    par.berretto = uff;
    par.volto.seme = seme;
    par.volto.eta = uff ? 30 + static_cast<int>(caso(static_cast<int>(seme), 5) * 15) : 19 + static_cast<int>(caso(static_cast<int>(seme), 5) * 12);
    par.volto.baffi = !usn && caso(static_cast<int>(seme), 6) < 0.25;
    par.girata_gradi = (caso(static_cast<int>(seme), 7) - 0.5) * 24;
    Testa h = testa_da(par);
    const int STOFFA = usn ? BLU : NERO;
    // busto: spalle, uniforme fino al bordo inferiore
    const double sp = 1.25 * u, collo = cy + 0.9 * u, fondo = clip.y + clip.h + 4;
    pol_px(t, clip, {{cx - 0.38 * u, collo - 0.2 * u}, {cx + 0.38 * u, collo - 0.2 * u}, {cx + 0.42 * u, collo + 0.5 * u}, {cx - 0.42 * u, collo + 0.5 * u}},
           [&](double x, double, int px, int py) { return PELLE + retina(4.6 - 1.8 * (x - cx) / (0.4 * u), px, py, 9); });
    pol_px(t, clip, {{cx - 0.4 * u, collo + 0.25 * u}, {cx + 0.4 * u, collo + 0.25 * u}, {cx + sp, collo + 0.55 * u}, {cx + sp + 0.15 * u, fondo},
                     {cx - sp - 0.15 * u, fondo}, {cx - sp, collo + 0.55 * u}},
           [&](double x, double y, int px, int py) {
               const double v = (x - cx) / sp;
               double tt = 4.2 - 2.0 * v - 0.02 * (y - collo) / u;
               if (std::fabs(v) > 0.9) tt -= 1.4;
               return STOFFA + retina(tt, px, py, 7);
           });
    if (uff) {
        // camicia bianca e cravatta nella V, bottoni dorati; controspalline VMF
        pol_px(t, clip, {{cx - 0.36 * u, collo + 0.25 * u}, {cx + 0.36 * u, collo + 0.25 * u}, {cx, collo + 1.6 * u}},
               [&](double x, double, int px, int py) { return BIANCO + retina(3.6 - 0.8 * (x - cx) / (0.36 * u), px, py, 4); });
        pol_px(t, clip, {{cx - 0.07 * u, collo + 0.3 * u}, {cx + 0.07 * u, collo + 0.3 * u}, {cx + 0.1 * u, collo + 1.5 * u}, {cx - 0.1 * u, collo + 1.5 * u}},
               [&](double, double, int px, int py) { return NERO + retina(1.5, px, py, 7); });
        for (const double s : {-1.0, 1.0}) {
            ell_px(t, clip, cx + s * 0.32 * u, collo + 2.0 * u, 0.09 * u + 0.5, 0.09 * u + 0.5, [](double, double, int, int) { return ORO + 6; });
            if (!usn)
                pol_px(t, clip, {{cx + s * 0.45 * u, collo + 0.32 * u}, {cx + s * 1.05 * u, collo + 0.5 * u}, {cx + s * 1.05 * u, collo + 0.68 * u}, {cx + s * 0.45 * u, collo + 0.5 * u}},
                       [&](double, double, int px, int py) { return ORO + retina(4.5 + ((px + py) % 2 ? 0.6 : -0.6), px, py, 7); });
        }
    } else if (usn) {
        // colletto alla marinara blu con tre righe bianche, fazzoletto nero annodato
        pol_px(t, clip, {{cx - 0.42 * u, collo + 0.25 * u}, {cx + 0.42 * u, collo + 0.25 * u}, {cx, collo + 1.3 * u}},
               [&](double x, double y, int px, int py) {
                   const double bordo = std::fabs(std::fabs(x - cx) - (0.42 * u) * (1 - (y - collo - 0.25 * u) / (1.05 * u)));
                   return bordo < 0.08 * u + 0.4 ? BIANCO + 4 : BIANCO + retina(2.6, px, py, 4);
               });
        pol_px(t, clip, {{cx - 0.14 * u, collo + 0.55 * u}, {cx + 0.14 * u, collo + 0.55 * u}, {cx + 0.05 * u, collo + 1.3 * u}, {cx - 0.05 * u, collo + 1.3 * u}},
               [&](double, double, int px, int py) { return NERO + retina(1.2, px, py, 7); });
        for (const double s : {-1.0, 1.0})   // il colletto sulle spalle con le righe
            pol_px(t, clip, {{cx + s * 0.4 * u, collo + 0.28 * u}, {cx + s * 1.1 * u, collo + 0.5 * u}, {cx + s * 0.95 * u, collo + 0.85 * u}, {cx + s * 0.32 * u, collo + 0.6 * u}},
                   [&](double x, double y, int px, int py) {
                       const double d = std::fabs(y - (collo + 0.32 * u + 0.3 * std::fabs(x - cx)));
                       return std::fmod(d, 0.12 * u + 1) < 0.6 ? BIANCO + 4 : BLU + retina(3.5, px, py, 7);
                   });
    } else {
        // VMF: guis blu con le righe bianche, maglia a righe nella V
        pol_px(t, clip, {{cx - 0.4 * u, collo + 0.25 * u}, {cx + 0.4 * u, collo + 0.25 * u}, {cx, collo + 1.25 * u}},
               [&](double, double y, int px, int py) { return static_cast<int>((y - collo) / std::max(1.0, 0.13 * u)) % 2 ? BIANCO + 4 : SMALTO_BLU + retina(2.2, px, py, 5); });
        for (const double s : {-1.0, 1.0})
            pol_px(t, clip, {{cx + s * 0.4 * u, collo + 0.28 * u}, {cx + s * 1.1 * u, collo + 0.5 * u}, {cx + s * 0.95 * u, collo + 0.85 * u}, {cx + s * 0.32 * u, collo + 0.6 * u}},
                   [&](double x, double y, int px, int py) {
                       const double d = std::fabs(y - (collo + 0.32 * u + 0.3 * std::fabs(x - cx)));
                       return std::fmod(d, 0.12 * u + 1) < 0.6 ? BIANCO + 4 : SMALTO_BLU + retina(3.0, px, py, 5);
                   });
    }
    if (ferito) {   // braccio al collo: la fascia bianca di traverso
        pol_px(t, clip, {{cx - 0.35 * u, collo + 0.3 * u}, {cx - 0.15 * u, collo + 0.3 * u}, {cx + 0.9 * u, collo + 1.9 * u}, {cx + 0.65 * u, collo + 2.0 * u}},
               [&](double, double, int px, int py) { return BIANCO + retina(3.8, px, py, 4); });
    }
    testa_piccola(t, clip, cx, cy, u, h, par);
    // berretti dei marinai: bianco USN (dixie cup), bustina nera VMF col nastro
    if (!uff) {
        if (usn) {
            ell_px(t, clip, cx, cy - 0.78 * u, 0.85 * u, 0.32 * u, [&](double uu, double v, int px, int py) { return BIANCO + retina(3.8 - 1.2 * uu - 0.6 * v, px, py, 4); });
            ell_px(t, clip, cx, cy - 0.62 * u, 0.95 * u, 0.13 * u + 0.5, [&](double uu, double, int px, int py) { return BIANCO + retina(3.0 - 1.2 * uu, px, py, 4); });
        } else {
            ell_px(t, clip, cx, cy - 0.8 * u, 1.0 * u, 0.26 * u, [&](double uu, double v, int px, int py) { return NERO + retina(2.6 - 1.0 * uu - 0.8 * v, px, py, 7); });
            ell_px(t, clip, cx, cy - 0.62 * u, 0.82 * u, 0.13 * u + 0.5, [&](double, double, int px, int py) {
                return (px + py) % 3 == 0 ? ORO + 4 : NERO + retina(1.0, px, py, 7);   // la scritta dorata sul nastro
            });
        }
    }
    if (ferito && caso(static_cast<int>(seme), 9) < 0.5)   // benda sulla fronte
        ell_px(t, clip, cx, cy - 0.38 * u, 0.82 * u, 0.12 * u + 0.5, [&](double uu, double, int px, int py) { return BIANCO + retina(3.8 - uu, px, py, 4); });
}

}  // namespace

void disegna_equipaggio(Tela& t, const Font& f, Rettangolo r, const ParametriEquipaggio& pe) {
    const bool usn = pe.marina == Marina::usn;
    const int W = r.w, H = r.h;
    // cielo coperto e acqua del porto
    for (int y = r.y; y < r.y + r.h; ++y)
        for (int x = r.x; x < r.x + r.w; ++x) {
            const double v = static_cast<double>(y - r.y) / H;
            const double n = rumore(x / 30.0, y / 14.0, 41);
            t.pixel(x, y, static_cast<std::uint8_t>(SFONDO + retina(v < 0.38 ? 6.2 - 3.0 * v + 1.2 * n : 2.0 + 1.5 * n, x, y, N_SFONDO - 1)));
        }
    // la vela del battello, scura, con i timoni (USN) e il numero; lo scafo sull'acqua
    const double vx = r.x + W * 0.5, vt = r.y + H * 0.08, vb = r.y + H * 0.52, vw = std::min(W * 0.34, H * 0.3);
    pol_px(t, r, {{vx - vw * 0.5, vb}, {vx - vw * 0.42, vt + vw * 0.08}, {vx - vw * 0.3, vt}, {vx + vw * 0.42, vt}, {vx + vw * 0.5, vt + vw * 0.15}, {vx + vw * 0.55, vb}},
           [&](double x, double, int px, int py) { return NERO + retina(3.4 - 2.4 * (x - vx) / vw, px, py, 7); });
    if (usn)
        pol_px(t, r, {{vx - vw * 1.05, vt + (vb - vt) * 0.42}, {vx + vw * 1.05, vt + (vb - vt) * 0.42}, {vx + vw * 1.0, vt + (vb - vt) * 0.48}, {vx - vw * 1.0, vt + (vb - vt) * 0.48}},
               [&](double x, double, int px, int py) { return NERO + retina(3.0 - 1.5 * (x - vx) / vw, px, py, 7); });
    pol_px(t, r, {{static_cast<double>(r.x), vb}, {static_cast<double>(r.x + W), vb}, {static_cast<double>(r.x + W), vb + H * 0.06}, {static_cast<double>(r.x), vb + H * 0.06}},
           [&](double, double y, int px, int py) { return NERO + retina(4.0 - 3.0 * (y - vb) / (H * 0.06), px, py, 7); });
    if (!pe.battello.empty()) {
        const int sc = std::max(1, static_cast<int>(vw / 36));
        const int lw = f.larghezza(pe.battello, sc);
        f.scrivi(t, static_cast<int>(vx) - lw / 2, static_cast<int>(vt + (vb - vt) * 0.15), pe.battello, static_cast<std::uint8_t>(BIANCO + 3), sc);
    }
    // banchina di legno
    const double banchina = r.y + H * 0.7;
    for (int y = static_cast<int>(banchina); y < r.y + r.h; ++y)
        for (int x = r.x; x < r.x + r.w; ++x) {
            const double v = (y - banchina) / (r.y + H - banchina);
            const bool giunto = static_cast<int>((x - r.x + 7 * static_cast<int>(v * 9)) * (1 + v)) % 23 == 0;
            t.pixel(x, y, static_cast<std::uint8_t>(LEGNO + retina(giunto ? 1.0 : 2.4 + 2.5 * v + 0.6 * caso(x / 6, y, 3), x, y, 7)));
        }
    // tre file sovrapposte come nelle foto di bordo: dietro sul ponte, poi a meta', in prima fila sulla banchina con
    // gli ufficiali; ogni fila copre il busto di quella dietro
    const int presenti = std::max(3, pe.uomini - pe.caduti);
    const int n_avanti = (presenti + 2) / 3, n_mezzo = (presenti - n_avanti + 1) / 2, n_dietro = presenti - n_avanti - n_mezzo;
    const int largo = std::max({n_avanti, n_mezzo, n_dietro, 1});
    const double u0 = std::min(H / 17.0, W / (largo * 2.35));
    struct Fila { double y, u; int n; };
    std::vector<Fila> file = {{r.y + H * 0.44, u0 * 0.88, n_dietro}, {r.y + H * 0.57, u0 * 0.94, n_mezzo}, {r.y + H * 0.71, u0, n_avanti}};
    int feriti = pe.feriti;
    std::uint32_t s = pe.seme * 7919u;
    for (int k = 0; k < 3; ++k) {   // da dietro in avanti
        const auto& fl = file[static_cast<std::size_t>(k)];
        const double passo = std::min(W / std::max(1.0, static_cast<double>(fl.n)), 2.6 * fl.u);
        const double x0 = r.x + (W - passo * fl.n) / 2;
        for (int i = 0; i < fl.n; ++i) {
            const double cx = x0 + passo * (i + 0.5) + (k == 1 ? 0.12 * passo : 0.0);
            Ruolo ruolo = Ruolo::marinaio;
            if (k == 2) {
                const int centro = fl.n / 2;
                if (i == centro) ruolo = Ruolo::comandante;
                else if (std::abs(i - centro) <= (fl.n >= 9 ? 2 : 1)) ruolo = Ruolo::ufficiale;
            }
            const bool ferito = feriti > 0 && ruolo == Ruolo::marinaio && caso(static_cast<int>(s), i, k) < 0.5 + 0.2 * k;
            if (ferito) --feriti;
            const double alto = (caso(static_cast<int>(s), i, 31) - 0.5) * 0.25 * fl.u;   // statura diversa
            figura(t, r, cx, fl.y + alto, fl.u, ruolo, pe, s++, ferito);
        }
    }
    // lutto: un nastro nero di traverso nell'angolo in alto a destra
    if (pe.caduti > 0)
        pol_px(t, r, {{static_cast<double>(r.x + W) - W * 0.22, static_cast<double>(r.y)}, {static_cast<double>(r.x + W) - W * 0.12, static_cast<double>(r.y)},
                      {static_cast<double>(r.x + W), r.y + W * 0.12}, {static_cast<double>(r.x + W), r.y + W * 0.22}},
               [&](double, double, int px, int py) { return NERO + retina(1.0, px, py, 7); });
}

void disegna_insegna_grado(Tela& t, Rettangolo r, Marina m, int grado) {
    // in un riquadro di riferimento 176 x 100: il polsino della manica o la controspallina
    auto pnr = [&](double x0, double y0, double x1, double y1, const std::function<int(double, double, int, int)>& f) {
        for (int y = r.y; y < r.y + r.h; ++y)
            for (int x = r.x; x < r.x + r.w; ++x) {
                const double u = (x + 0.5 - r.x) * 176.0 / r.w, v = (y + 0.5 - r.y) * 100.0 / r.h;
                if (u < x0 || u >= x1 || v < y0 || v >= y1) continue;
                const int c = f(u, v, x, y);
                if (c >= 0) t.pixel(x, y, static_cast<std::uint8_t>(c));
            }
    };
    if (m == Marina::usn) {
        // fondo della manica blu, polsino in basso, galloni d'oro e la stella sopra
        pnr(0, 0, 176, 100, [&](double u, double v, int px, int py) {
            return BLU + retina(3.8 - 1.6 * (u - 88) / 88.0 - 0.01 * v + 0.6 * (rumore(u / 12, v / 24, 3) - 0.5), px, py, 7);
        });
        double y = 88;
        for (const double g : galloni_usn(grado)) {
            const double h = g * 3.2, y0 = y - h;
            pnr(0, y0, 176, y, [&](double u, double v, int px, int py) {
                const double fili = ((px + py) % 2) ? 0.5 : -0.5;
                return ORO + retina(5.6 - 2.4 * (u - 88) / 88.0 + (v - y0 < 1.5 ? 1.2 : 0.0) - (y - v < 1.2 ? 2.0 : 0.0) + fili, px, py, 7);
            });
            y = y0 - 4.5;
        }
        pnr(70, y - 26, 106, y - 2, [&](double u, double v, int px, int py) {
            return stella_in(u, v, 88, y - 14, 12, ORO) >= 0 ? ORO + retina(6.0 - 0.12 * (u - 88) - 0.1 * (v - y + 14), px, py, 7) : -1;
        });
        pnr(0, 92, 176, 100, [&](double u, double, int px, int py) { return BIANCO + retina(3.5 - 0.8 * (u - 88) / 88.0, px, py, 4); });
    } else {
        // controspallina vista dall'alto: gallone d'oro, strisce e stelle, bottone
        const int strisce = grado <= 4 ? 1 : grado <= 7 ? 2 : 0;
        const int stelle = grado <= 4 ? grado : grado <= 7 ? grado - 4 : grado <= 10 ? grado - 7 : 1;
        const bool grandi = grado >= 5;
        pnr(6, 22, 170, 78, [&](double u, double v, int px, int py) {
            const double across = (v - 50) / 28.0;
            if (u > 150) {   // punta arrotondata verso il collo con il bottone
                const double d = std::hypot((u - 150) / 20.0, across);
                if (d > 1) return -1;
                if (std::hypot(u - 156, v - 50) < 8) return ORO + retina(5.5 - 0.3 * (u - 156) - 0.3 * (v - 50), px, py, 7);
            }
            bool scura = false;
            if (strisce == 1) scura = std::fabs(across) < 0.12;
            if (strisce == 2) scura = std::fabs(std::fabs(across) - 0.42) < 0.1;
            if (scura) return NERO + 1;
            if (std::fabs(across) > 0.92 || u < 8) return ORO + 1;
            const double fili = ((px / 2 + py) % 2) ? 0.7 : -0.7;
            return ORO + retina(5.0 - 1.4 * across + fili, px, py, 7);
        });
        for (int k = 0; k < stelle; ++k) {
            const double cu = 40 + k * (grandi ? 34 : 26), cv = 50;
            const double rr = grandi ? 12 : 8;
            pnr(cu - rr, cv - rr, cu + rr, cv + rr, [&](double u, double v, int px, int py) {
                return stella_in(u, v, cu, cv, rr, ARGENTO) >= 0 ? ARGENTO + retina(4.8 - 0.15 * (u - cu) - 0.1 * (v - cv), px, py, 5) : -1;
            });
        }
    }
}

}  // namespace m688
