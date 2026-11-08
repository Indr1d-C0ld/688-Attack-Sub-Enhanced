#include "grafica/medaglioni.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <functional>
#include <map>

#include "grafica/cerimonia.hpp"
#include "nucleo/geometria.hpp"

namespace m688 {

namespace {

using namespace cerimonia;

// materiali del medaglione: le rampe dal buio alla luce
enum Mat { BRONZO_M, ARGENTO_M, ORO_M, GRIGIO_M, ROSSO_M, NOTTE_M, VERDE_M, AZZURRO_M, BIANCO_M, VIOLA_M, NERO_M, N_MAT };

std::vector<std::uint8_t> rampa(int primo, int n) {
    std::vector<std::uint8_t> r;
    for (int i = 0; i < n; ++i) r.push_back(static_cast<std::uint8_t>(primo + i));
    return r;
}

const std::array<std::vector<std::uint8_t>, N_MAT>& rampe(TavolozzaMedaglione t) {
    static const std::array<std::vector<std::uint8_t>, N_MAT> cer = {
        rampa(BRONZO, N_BRONZO), rampa(ARGENTO, N_ARGENTO), rampa(ORO, N_ORO), rampa(ARGENTO, N_ARGENTO - 2),
        rampa(SMALTO, N_SMALTO), rampa(BLU, N_BLU - 2), rampa(VERDE, N_VERDE), rampa(SMALTO_BLU, N_SMALTO_BLU),
        rampa(BIANCO, N_BIANCO), rampa(VIOLA, N_VIOLA), rampa(NERO, 4)};
    // indici fissi che la vista non illumina (gli stessi della tavolozza fissa delle sagome delle armi)
    static const std::array<std::vector<std::uint8_t>, N_MAT> vis = {{
        {0, 6, 188, 186, 185},          // bronzo
        {88, 86, 84, 82, 81, 80, 15},   // argento
        {6, 93, 94, 14},                // oro
        {0, 88, 87, 86, 84},            // grigio
        {0, 4, 91, 12},                 // smalto rosso
        {0, 1, 9},                      // blu notte
        {0, 2, 10},                     // verde
        {1, 3, 11},                     // azzurro
        {84, 82, 80, 15},               // bianco
        {0, 5, 13},                     // viola
        {0, 0, 88},                     // nero
    }};
    return t == TavolozzaMedaglione::vista ? vis : cer;
}

struct P {
    double x, y;
};
double sd_cerchio(P p, double r) { return std::hypot(p.x, p.y) - r; }
double sd_capsula(P p, P a, P b, double r) {
    const P pa{p.x - a.x, p.y - a.y}, ba{b.x - a.x, b.y - a.y};
    const double h = std::clamp((pa.x * ba.x + pa.y * ba.y) / std::max(ba.x * ba.x + ba.y * ba.y, 1e-9), 0.0, 1.0);
    return std::hypot(pa.x - ba.x * h, pa.y - ba.y * h) - r;
}
double sd_poligono(const std::vector<P>& v, P p) {
    double d = (p.x - v[0].x) * (p.x - v[0].x) + (p.y - v[0].y) * (p.y - v[0].y), s = 1;
    for (std::size_t i = 0, j = v.size() - 1; i < v.size(); j = i, ++i) {
        const P e{v[j].x - v[i].x, v[j].y - v[i].y}, w{p.x - v[i].x, p.y - v[i].y};
        const double k = std::clamp((w.x * e.x + w.y * e.y) / (e.x * e.x + e.y * e.y), 0.0, 1.0);
        const P b{w.x - e.x * k, w.y - e.y * k};
        d = std::min(d, b.x * b.x + b.y * b.y);
        const bool c1 = p.y >= v[i].y, c2 = p.y < v[j].y, c3 = e.x * w.y > e.y * w.x;
        if ((c1 && c2 && c3) || (!c1 && !c2 && !c3)) s = -s;
    }
    return s * std::sqrt(d);
}

// rilievo: altezza e materiale per pixel, poi la luce (come le medaglie di grafica/decorazioni)
class Rilievo {
public:
    Rilievo(int w, int h) : w_(w), h_(h), alt_(static_cast<std::size_t>(w) * h, 0.0), mat_(static_cast<std::size_t>(w) * h, -1) {}
    void forma(const std::function<double(P)>& sd, double base, double rilievo, double smusso, int mat, bool solo_sopra = false) {
        for (int y = 0; y < h_; ++y)
            for (int x = 0; x < w_; ++x) {
                const double d = sd({x + 0.5, y + 0.5});
                if (d > 0) continue;
                const std::size_t i = static_cast<std::size_t>(y) * w_ + x;
                if (solo_sopra && mat_[i] < 0) continue;
                alt_[i] = base + rilievo * std::min(-d / smusso, 1.0);
                mat_[i] = mat;
            }
    }
    void disegna(Tela& t, int ox, int oy, const std::array<std::vector<std::uint8_t>, N_MAT>& r, double luce_max) const {
        const double lx = -0.55, ly = -0.65, lz = 0.85, ln = std::sqrt(lx * lx + ly * ly + lz * lz);
        for (int y = 0; y < h_; ++y)
            for (int x = 0; x < w_; ++x) {
                const std::size_t i = static_cast<std::size_t>(y) * w_ + x;
                const int m = mat_[i];
                if (m < 0) continue;
                auto a = [&](int xx, int yy) {
                    xx = std::clamp(xx, 0, w_ - 1);
                    yy = std::clamp(yy, 0, h_ - 1);
                    const std::size_t j = static_cast<std::size_t>(yy) * w_ + xx;
                    return mat_[j] < 0 ? alt_[i] - 1.2 : alt_[j];
                };
                const double gx = (a(x + 1, y) - a(x - 1, y)) / 2, gy = (a(x, y + 1) - a(x, y - 1)) / 2;
                const double nn = std::sqrt(gx * gx + gy * gy + 1.0);
                const double l = (-gx * lx - gy * ly + lz) / (nn * ln);
                const auto& rm = r[static_cast<std::size_t>(m)];
                const int n = static_cast<int>(rm.size());
                const double spec = std::pow(std::max(0.0, l), 12.0);
                double tono = std::min((0.15 + 0.7 * std::max(0.0, l)) * (n - 1) + spec * 0.45 * (n - 1), luce_max * (n - 1));
                bool bordo = false;
                for (const auto& [dx, dy] : {std::pair{1, 0}, {-1, 0}, {0, 1}, {0, -1}}) {
                    const int xx = x + dx, yy = y + dy;
                    if (xx < 0 || yy < 0 || xx >= w_ || yy >= h_ || mat_[static_cast<std::size_t>(yy) * w_ + xx] < 0) bordo = true;
                }
                if (bordo) tono = std::min(tono, 0.6);
                const int k = std::clamp(retina(tono, ox + x, oy + y, n - 1), 0, n - 1);
                t.pixel(ox + x, oy + y, rm[static_cast<std::size_t>(k)]);
            }
    }

private:
    int w_, h_;
    std::vector<double> alt_;
    std::vector<int> mat_;
};

// Il simbolo in coordinate del campo: (0, 0) al centro, 1 = raggio utile del simbolo, y in giu'
class Simbolo {
public:
    Simbolo(Rilievo& r, double cx, double cy, double s, int mat, double base) : r_(r), cx_(cx), cy_(cy), s_(s), mat_(mat), base_(base) {}
    void cerchio(double x, double y, double rr) { add([=](P p) { return sd_cerchio({p.x - x, p.y - y}, rr); }); }
    void anello(double x, double y, double rr, double sp) {
        add([=](P p) { return std::fabs(sd_cerchio({p.x - x, p.y - y}, rr)) - sp / 2; });
    }
    void linea(double x0, double y0, double x1, double y1, double sp) { add([=](P p) { return sd_capsula(p, {x0, y0}, {x1, y1}, sp / 2); }); }
    void rett(double x, double y, double w, double h) { poli({{x - w / 2, y - h / 2}, {x + w / 2, y - h / 2}, {x + w / 2, y + h / 2}, {x - w / 2, y + h / 2}}); }
    void poli(std::vector<P> v) { add([v](P p) { return sd_poligono(v, p); }); }
    void ellisse(double x, double y, double a, double b) {
        add([=](P p) { const double dx = (p.x - x) / a, dy = (p.y - y) / b; return (std::hypot(dx, dy) - 1.0) * std::min(a, b); });
    }
    void stella(double x, double y, int n, double re, double ri, double rot = 0.0) {
        std::vector<P> v;
        for (int k = 0; k < 2 * n; ++k) {
            const double a = rot + PI * k / n, rr = k % 2 == 0 ? re : ri;
            v.push_back({x + rr * std::sin(a), y - rr * std::cos(a)});
        }
        poli(v);
    }
    // arco di circonferenza da a0 ad a1 (gradi, 0 in alto, in senso orario), a pezzi di capsula
    void arco(double x, double y, double rr, double a0, double a1, double sp) {
        const int n = std::max(3, static_cast<int>(std::fabs(a1 - a0) / 12.0));
        for (int k = 0; k < n; ++k) {
            const double t0 = radianti(a0 + (a1 - a0) * k / n), t1 = radianti(a0 + (a1 - a0) * (k + 1) / n);
            linea(x + rr * std::sin(t0), y - rr * std::cos(t0), x + rr * std::sin(t1), y - rr * std::cos(t1), sp);
        }
    }
    // parti a rilievo piu' alto (in primo piano) o piu' basso
    void quota(double b) { base_ = b; }

    // pezzi composti
    void siluro(double x, double y, double l, double ang) {   // corpo, ogiva, alette ed elica; prua verso ang (gradi, 0 = destra)
        const double a = radianti(ang), c = std::cos(a), s = std::sin(a), d = l * 0.085;
        auto pt = [&](double u, double v) { return P{x + u * c - v * s, y + u * s + v * c}; };
        const P p0 = pt(-l / 2, 0), p1 = pt(l / 2 - d, 0);
        linea(p0.x, p0.y, p1.x, p1.y, 2 * d);
        poli({pt(-l / 2 + 0.02, -2.3 * d), pt(-l / 2 + 0.16 * l, -d), pt(-l / 2 + 0.16 * l, d), pt(-l / 2 + 0.02, 2.3 * d)});
        const P e = pt(-l / 2 - 0.04, 0);
        linea(e.x, e.y - 1.6 * d * c, e.x, e.y + 1.6 * d * c, 0.05);
    }
    void sottomarino(double x, double y, double l) {   // di fianco, prua a destra
        ellisse(x, y, l / 2, l * 0.09);
        rett(x + l * 0.16, y - l * 0.12, l * 0.14, l * 0.12);
        poli({{x - l * 0.46, y}, {x - l * 0.54, y - l * 0.1}, {x - l * 0.5, y}, {x - l * 0.54, y + l * 0.08}});
    }
    void nave(double x, double y, double l) {   // di fianco, prua a destra
        poli({{x - l / 2, y - l * 0.04}, {x + l / 2, y - l * 0.08}, {x + l * 0.4, y + l * 0.1}, {x - l * 0.46, y + l * 0.1}});
        rett(x - l * 0.05, y - l * 0.11, l * 0.32, l * 0.1);
        rett(x + l * 0.02, y - l * 0.19, l * 0.12, l * 0.08);
        linea(x - l * 0.02, y - l * 0.36, x - l * 0.02, y - l * 0.18, 0.05);
    }
    void mirino(double x, double y, double rr) {
        anello(x, y, rr, 0.08);
        for (const auto& [dx, dy] : {std::pair{1.0, 0.0}, {-1.0, 0.0}, {0.0, 1.0}, {0.0, -1.0}})
            linea(x + dx * rr * 0.55, y + dy * rr * 0.55, x + dx * rr * 1.25, y + dy * rr * 1.25, 0.07);
    }
    void freccia(double x0, double y0, double x1, double y1, double sp) {
        linea(x0, y0, x1, y1, sp);
        const double a = std::atan2(y1 - y0, x1 - x0), k = sp * 3.0;
        poli({{x1 + std::cos(a) * k, y1 + std::sin(a) * k}, {x1 + std::cos(a + 2.4) * k, y1 + std::sin(a + 2.4) * k},
              {x1 + std::cos(a - 2.4) * k, y1 + std::sin(a - 2.4) * k}});
    }

private:
    void add(const std::function<double(P)>& sd_unitario) {
        const double cx = cx_, cy = cy_, s = s_;
        r_.forma([&, cx, cy, s](P p) { return sd_unitario({(p.x - cx) / s, (p.y - cy) / s}) * s; }, base_, 0.9, std::max(0.8, s * 0.07), mat_);
    }
    Rilievo& r_;
    double cx_, cy_, s_;
    int mat_;
    double base_;
};

using Disegno = void (*)(Simbolo&);

struct Voce {
    const char* id;
    const char* categoria;
    Disegno disegno;
};

// --- i simboli (y in giu', 1 = raggio utile) ---
const std::vector<Voce>& voci() {
    static const std::vector<Voce> v = {
        // tattico
        {"primo_sangue", "tattico", [](Simbolo& s) { s.siluro(-0.15, 0.15, 1.3, -35); s.stella(0.5, -0.45, 8, 0.38, 0.14); }},
        {"cacciatore", "tattico", [](Simbolo& s) { s.sottomarino(0.0, 0.1, 1.6); s.quota(1.9); s.mirino(0.0, 0.05, 0.5); }},
        {"flotta", "tattico", [](Simbolo& s) { s.nave(0.0, 0.15, 1.6); s.quota(1.9); s.mirino(0.05, 0.05, 0.5); }},
        {"un_colpo", "tattico", [](Simbolo& s) { s.anello(0.35, 0.0, 0.45, 0.12); s.cerchio(0.35, 0.0, 0.14); s.siluro(-0.45, 0.0, 0.9, 0); }},
        {"doppietta", "tattico", [](Simbolo& s) { s.siluro(0.0, -0.32, 1.5, 0); s.siluro(0.0, 0.32, 1.5, 0); }},
        {"elusione", "tattico", [](Simbolo& s) {
             s.siluro(-0.2, 0.45, 1.1, 0);
             s.arco(0.05, 0.25, 0.55, -80, 80, 0.12);
             s.freccia(0.55, 0.25, 0.6, 0.55, 0.12);
         }},
        {"completa", "tattico", [](Simbolo& s) { s.linea(-0.6, 0.0, -0.15, 0.5, 0.26); s.linea(-0.15, 0.5, 0.7, -0.55, 0.26); }},
        {"posa", "tattico", [](Simbolo& s) {
             s.cerchio(0.0, 0.0, 0.5);
             for (int k = 0; k < 8; ++k) {
                 const double a = PI * k / 4;
                 s.linea(0.0, 0.0, 0.78 * std::sin(a), -0.78 * std::cos(a), 0.12);
                 s.cerchio(0.78 * std::sin(a), -0.78 * std::cos(a), 0.1);
             }
         }},
        // tattico, l'aggiornamento di docs/26 (29/10/2026)
        {"oltre_orizzonte", "tattico", [](Simbolo& s) {   // la traiettoria del missile che scavalca l'orizzonte
             s.linea(-0.85, 0.42, 0.85, 0.42, 0.08);
             s.arco(0.0, 0.62, 0.72, -78, 62, 0.11);
             s.freccia(0.58, 0.2, 0.7, 0.36, 0.1);
             s.quota(0.9);
             s.nave(0.62, 0.62, 0.5);   // sotto l'orizzonte, piu' bassa
         }},
        {"scatola_pulita", "tattico", [](Simbolo& s) {   // la scatola di ricerca col neutrale dentro, intatto
             s.linea(-0.7, -0.55, 0.7, -0.55, 0.09);
             s.linea(0.7, -0.55, 0.7, 0.55, 0.09);
             s.linea(0.7, 0.55, -0.7, 0.55, 0.09);
             s.linea(-0.7, 0.55, -0.7, -0.55, 0.09);
             s.nave(0.0, 0.12, 1.0);
         }},
        {"allarme_siluro", "tattico", [](Simbolo& s) {   // il triangolo dell'allarme col punto esclamativo, il siluro sotto
             s.linea(0.0, -0.82, -0.62, 0.2, 0.1);
             s.linea(-0.62, 0.2, 0.62, 0.2, 0.1);
             s.linea(0.62, 0.2, 0.0, -0.82, 0.1);
             s.linea(0.0, -0.48, 0.0, -0.14, 0.13);
             s.cerchio(0.0, 0.03, 0.07);
             s.siluro(0.0, 0.55, 1.4, 0);
         }},
        {"campo_minato", "tattico", [](Simbolo& s) {   // il battello sopra tre mine all'ormeggio
             s.sottomarino(0.0, -0.5, 1.4);
             for (const double x : {-0.55, 0.0, 0.55}) {
                 s.cerchio(x, 0.3, 0.16);
                 for (const auto& [dx, dy] : {std::pair{0.0, -1.0}, {1.0, 0.0}, {-1.0, 0.0}}) s.linea(x, 0.3, x + dx * 0.24, 0.3 + dy * 0.24, 0.05);
                 s.linea(x, 0.46, x, 0.8, 0.04);
             }
         }},
        {"tempesta", "tattico", [](Simbolo& s) {   // le onde alte e la pioggia di traverso
             for (const double x : {-0.6, 0.0, 0.6}) s.arco(x, 0.15, 0.3, -90, 70, 0.11);
             for (const double x : {-0.3, 0.3}) s.arco(x, 0.62, 0.3, -90, 70, 0.11);
             for (const double x : {-0.55, -0.15, 0.25, 0.65}) s.linea(x, -0.8, x - 0.18, -0.38, 0.06);
         }},
        {"sotto_il_fuoco", "tattico", [](Simbolo& s) {   // le colonne d'acqua dei colpi attorno al battello
             for (const auto& [x, h] : {std::pair{-0.62, 0.75}, {0.08, 0.95}, {0.66, 0.6}})
                 s.poli({{x - 0.13, 0.32}, {x - 0.04, 0.32 - h}, {x + 0.04, 0.32 - h}, {x + 0.13, 0.32}});
             s.quota(1.9);
             s.sottomarino(0.0, 0.5, 1.5);
         }},
        // stealth
        {"fantasma", "stealth", [](Simbolo& s) {
             s.quota(0.6);   // basso, quasi nello smalto
             s.sottomarino(0.0, 0.0, 1.7);
             s.quota(1.2);
             for (int k = -1; k <= 1; ++k) s.arco(k * 0.42, 0.7, 0.2, -70, 70, 0.07);
         }},
        {"silenzio_radio", "stealth", [](Simbolo& s) {
             s.linea(0.0, -0.7, 0.0, 0.75, 0.14);
             s.arco(0.0, -0.4, 0.35, 45, 135, 0.09);
             s.arco(0.0, -0.4, 0.35, -135, -45, 0.09);
             s.quota(1.9);
             s.linea(-0.7, 0.7, 0.7, -0.7, 0.14);
         }},
        {"fra_le_boe", "stealth", [](Simbolo& s) {
             for (int k = -1; k <= 1; ++k) { s.rett(k * 0.6, -0.5, 0.16, 0.38); s.linea(k * 0.6, -0.31, k * 0.6, -0.12, 0.04); }
             s.sottomarino(0.0, 0.45, 1.5);
         }},
        {"solo_orecchie", "stealth", [](Simbolo& s) {
             s.arco(0.05, -0.15, 0.55, -100, 170, 0.18);
             s.arco(0.05, -0.12, 0.25, -60, 160, 0.12);
             s.linea(-0.1, 0.4, -0.3, 0.7, 0.16);
         }},
        {"sovereign", "stealth", [](Simbolo& s) { s.sottomarino(-0.15, -0.38, 1.3); s.quota(1.9); s.sottomarino(0.2, 0.38, 1.3); }},
        {"testa_bassa", "stealth", [](Simbolo& s) {
             s.linea(-0.2, -0.7, -0.2, 0.45, 0.16);
             s.linea(-0.2, -0.7, 0.1, -0.7, 0.16);
             s.rett(0.12, -0.7, 0.14, 0.22);
             s.linea(-0.8, 0.45, 0.8, 0.45, 0.07);
             s.freccia(0.5, -0.4, 0.5, 0.15, 0.1);
         }},
        {"sotto_chiglia", "stealth", [](Simbolo& s) {   // il battello appena sopra il fondo
             s.sottomarino(0.0, -0.2, 1.5);
             s.poli({{-0.9, 0.38}, {-0.5, 0.28}, {-0.1, 0.4}, {0.3, 0.26}, {0.7, 0.36}, {0.9, 0.32}, {0.9, 0.85}, {-0.9, 0.85}});
             s.quota(1.6);
             s.freccia(0.62, -0.05, 0.62, 0.12, 0.05);
         }},
        // tecnico
        {"orecchio_fino", "tecnico", [](Simbolo& s) {
             s.arco(0.0, 0.0, 0.58, -100, 100, 0.13);
             s.rett(-0.6, 0.15, 0.24, 0.44);
             s.rett(0.6, 0.15, 0.24, 0.44);
         }},
        {"anticipo", "tecnico", [](Simbolo& s) {
             s.anello(0.0, 0.1, 0.6, 0.12);
             s.rett(0.0, -0.62, 0.2, 0.14);
             s.linea(0.0, 0.1, 0.0, -0.3, 0.09);
             s.linea(0.0, 0.1, 0.3, 0.25, 0.09);
         }},
        {"controllo_danni", "tecnico", [](Simbolo& s) {
             s.linea(-0.5, 0.55, 0.25, -0.2, 0.2);
             s.anello(0.4, -0.35, 0.28, 0.16);
             s.quota(0.4);
             s.rett(0.55, -0.5, 0.2, 0.2);   // la bocca della chiave, scavata
         }},
        {"quota_limite", "tecnico", [](Simbolo& s) {
             s.anello(0.0, 0.0, 0.65, 0.12);
             for (int k = 0; k < 7; ++k) {
                 const double a = radianti(-120.0 + 40.0 * k);
                 s.linea(0.48 * std::sin(a), -0.48 * std::cos(a), 0.6 * std::sin(a), -0.6 * std::cos(a), 0.06);
             }
             s.linea(0.0, 0.0, 0.42 * std::sin(radianti(110.0)), -0.42 * std::cos(radianti(110.0)), 0.1);
             s.cerchio(0.0, 0.0, 0.09);
         }},
        {"fotografo", "tecnico", [](Simbolo& s) {
             s.rett(0.0, 0.1, 1.3, 0.8);
             s.rett(-0.35, -0.38, 0.3, 0.18);
             s.quota(1.9);
             s.anello(0.08, 0.12, 0.25, 0.1);
         }},
        {"telegrafista", "tecnico", [](Simbolo& s) {
             s.poli({{0.1, -0.8}, {-0.45, 0.1}, {-0.05, 0.1}, {-0.2, 0.8}, {0.45, -0.15}, {0.05, -0.15}, {0.25, -0.8}});
         }},
        {"alla_lettera", "tecnico", [](Simbolo& s) {
             s.rett(0.0, -0.05, 0.9, 1.1);
             s.cerchio(-0.45, -0.6, 0.12);
             s.cerchio(0.45, 0.5, 0.12);
             s.quota(1.9);
             s.cerchio(0.3, 0.25, 0.2);   // il sigillo
         }},
        {"occhio_libretto", "tecnico", [](Simbolo& s) {   // il libretto aperto con la sagoma sopra
             s.poli({{-0.82, -0.12}, {-0.06, -0.02}, {-0.06, 0.7}, {-0.82, 0.6}});
             s.poli({{0.06, -0.02}, {0.82, -0.12}, {0.82, 0.6}, {0.06, 0.7}});
             s.quota(1.9);
             s.nave(0.0, -0.45, 1.1);
         }},
        {"telemetrista", "tecnico", [](Simbolo& s) {   // l'immagine sdoppiata dello stadimetro e la scala
             s.quota(0.9);
             s.nave(-0.15, -0.3, 1.2);   // la copia fantasma, piu' bassa
             s.quota(1.6);
             s.nave(-0.15, 0.38, 1.2);
             s.linea(0.75, -0.7, 0.75, 0.7, 0.07);
             for (int k = 0; k < 5; ++k) s.linea(0.62, -0.6 + 0.3 * k, 0.75, -0.6 + 0.3 * k, 0.05);
         }},
        {"angolo_giusto", "tecnico", [](Simbolo& s) {   // il rilevamento, la rotta e l'angolo fra i due
             s.linea(-0.65, 0.5, 0.75, 0.5, 0.1);
             s.freccia(-0.65, 0.5, 0.2, -0.38, 0.1);
             s.arco(-0.65, 0.5, 0.55, 44, 90, 0.08);
             s.cerchio(-0.65, 0.5, 0.12);
         }},
        {"prima_la_vista", "tecnico", [](Simbolo& s) {   // l'occhio
             s.arco(0.0, 0.62, 0.95, -52, 52, 0.12);
             s.arco(0.0, -0.62, 0.95, 128, 232, 0.12);
             s.anello(0.0, 0.0, 0.27, 0.1);
             s.cerchio(0.0, 0.0, 0.12);
         }},
        {"punto_nave", "tecnico", [](Simbolo& s) {   // tre rette di posizione che si chiudono nel punto
             s.linea(-0.7, 0.09, 0.7, -0.03, 0.08);
             s.linea(-0.33, -0.6, 0.3, 0.66, 0.08);
             s.linea(0.38, -0.62, -0.3, 0.64, 0.08);
             s.quota(1.9);
             s.anello(0.0, 0.03, 0.2, 0.08);
         }},
        {"non_autentico", "tecnico", [](Simbolo& s) {   // la busta del messaggio, sbarrata
             s.rett(0.0, 0.05, 1.3, 0.86);
             s.quota(0.4);
             s.linea(-0.6, -0.33, 0.0, 0.12, 0.07);
             s.linea(0.0, 0.12, 0.6, -0.33, 0.07);
             s.quota(1.9);
             s.linea(-0.72, -0.72, 0.72, 0.72, 0.15);
             s.linea(-0.72, 0.72, 0.72, -0.72, 0.15);
         }},
        {"novita_acint", "tecnico", [](Simbolo& s) {   // le righe dello spettro e quella nuova, con la stella
             s.linea(-0.8, 0.55, 0.8, 0.55, 0.08);
             for (const auto& [x, h] : {std::pair{-0.6, 0.3}, {-0.35, 0.55}, {-0.1, 0.22}, {0.15, 0.4}, {0.65, 0.2}})
                 s.linea(x, 0.55, x, 0.55 - h, 0.08);
             s.linea(0.4, 0.55, 0.4, -0.25, 0.12);
             s.stella(0.4, -0.55, 5, 0.26, 0.1);
         }},
        // storico
        {"originali", "storico", [](Simbolo& s) {   // il dischetto da 5,25"
             s.rett(0.0, 0.0, 1.2, 1.2);
             s.quota(1.8);
             s.anello(0.0, 0.0, 0.16, 0.08);
             s.rett(0.0, 0.45, 0.14, 0.3);
         }},
        {"barents", "storico", [](Simbolo& s) { s.stella(0.0, 0.05, 4, 0.75, 0.16); s.cerchio(0.0, 0.05, 0.14); }},
        {"ghiaccio", "storico", [](Simbolo& s) {
             s.poli({{-0.8, 0.1}, {-0.2, 0.0}, {-0.25, 0.25}, {-0.8, 0.3}});
             s.poli({{0.25, 0.0}, {0.8, 0.12}, {0.8, 0.32}, {0.2, 0.25}});
             s.rett(0.0, -0.15, 0.3, 0.5);   // la vela che rompe il ghiaccio
             s.linea(-0.8, 0.55, 0.8, 0.55, 0.07);
         }},
        {"crisi", "storico", [](Simbolo& s) {   // il termometro basso
             s.linea(0.0, -0.7, 0.0, 0.3, 0.26);
             s.cerchio(0.0, 0.5, 0.28);
             for (int k = 0; k < 4; ++k) s.linea(0.2, -0.5 + 0.22 * k, 0.4, -0.5 + 0.22 * k, 0.06);
         }},
        {"guerra", "storico", [](Simbolo& s) { s.siluro(0.0, 0.0, 1.6, 40); s.siluro(0.0, 0.0, 1.6, 140); }},
        {"due_marine", "storico", [](Simbolo& s) { s.stella(-0.4, 0.0, 5, 0.42, 0.17); s.stella(0.42, 0.0, 5, 0.42, 0.17); }},
        // carriera
        {"dolphins", "carriera", [](Simbolo& s) {
             s.sottomarino(0.0, 0.2, 1.0);
             s.arco(-0.55, 0.0, 0.42, -40, 110, 0.13);
             s.arco(0.55, 0.0, 0.42, -110, 40, 0.13);
         }},
        {"al_comando", "carriera", [](Simbolo& s) {   // la foglia di quercia del CDR
             s.ellisse(0.0, -0.05, 0.42, 0.7);
             s.linea(0.0, -0.6, 0.0, 0.8, 0.07);
             for (int k = -1; k <= 1; ++k) { s.cerchio(-0.42, k * 0.32, 0.14); s.cerchio(0.42, k * 0.32, 0.14); }
         }},
        {"quattro_galloni", "carriera", [](Simbolo& s) {
             for (int k = 0; k < 4; ++k) s.rett(0.0, -0.5 + 0.32 * k, 1.4, 0.16);
         }},
        {"decorato", "carriera", [](Simbolo& s) {
             s.poli({{-0.35, -0.8}, {0.35, -0.8}, {0.25, -0.15}, {-0.25, -0.15}});
             s.quota(1.8);
             s.stella(0.0, 0.35, 5, 0.45, 0.18);
         }},
        {"massima", "carriera", [](Simbolo& s) { s.stella(0.0, 0.05, 5, 0.8, 0.32); }},
        {"veterano", "carriera", [](Simbolo& s) {
             for (int k = 0; k < 3; ++k) {
                 const double y = -0.35 + 0.35 * k;
                 s.linea(-0.6, y + 0.25, 0.0, y - 0.15, 0.16);
                 s.linea(0.0, y - 0.15, 0.6, y + 0.25, 0.16);
             }
         }},
        {"pensione", "carriera", [](Simbolo& s) {   // l'ancora
             s.linea(0.0, -0.55, 0.0, 0.6, 0.15);
             s.anello(0.0, -0.68, 0.14, 0.08);
             s.linea(-0.4, -0.3, 0.4, -0.3, 0.12);
             s.arco(0.0, 0.05, 0.55, 110, 250, 0.14);
         }},
        // editor
        {"autore", "editor", [](Simbolo& s) {   // la penna d'oca
             s.poli({{0.65, -0.75}, {0.2, 0.0}, {-0.45, 0.6}, {-0.3, 0.2}, {0.1, -0.35}});
             s.linea(-0.45, 0.6, -0.6, 0.8, 0.06);
         }},
        {"codici", "editor", [](Simbolo& s) {
             for (int i = -1; i <= 1; ++i)
                 for (int j = -1; j <= 1; ++j)
                     if ((i + j + 3) % 2 == 0 || (i == 0 && j == 0)) s.rett(i * 0.45, j * 0.45, 0.32, 0.32);
         }},
        {"biblioteca", "editor", [](Simbolo& s) {
             for (int k = 0; k < 4; ++k) s.rett(-0.48 + 0.3 * k, 0.05 - 0.04 * (k % 2), 0.22, 1.0 - 0.1 * (k % 3));
             s.linea(-0.75, 0.6, 0.75, 0.6, 0.08);
         }},
        // nascosti
        {"sasso", "nascosto", [](Simbolo& s) { s.poli({{-0.7, 0.45}, {-0.5, -0.15}, {-0.1, -0.45}, {0.45, -0.3}, {0.75, 0.2}, {0.5, 0.45}}); }},
        {"un_pelo", "nascosto", [](Simbolo& s) {
             s.sottomarino(0.0, 0.3, 1.4);
             s.quota(1.9);
             s.siluro(0.0, -0.2, 1.1, 180);
         }},
        {"ultimo_siluro", "nascosto", [](Simbolo& s) {
             for (int k = 0; k < 3; ++k) s.anello(-0.5 + 0.5 * k, -0.35, 0.18, 0.06);   // tubi vuoti
             s.siluro(0.0, 0.4, 1.2, 0);
         }},
        {"pendolare", "nascosto", [](Simbolo& s) {
             s.freccia(-0.75, -0.25, 0.55, -0.25, 0.12);
             s.freccia(0.75, 0.25, -0.55, 0.25, 0.12);
         }},
        // il secondo giro (disassemblatore, 31/10/2026): ghiaccio, fanali, soccorso, pedinamento, mine, elicotteri, schiere
        {"polynya", "tecnico", [](Simbolo& s) {   // la vela che sbuca nell'acqua libera fra due lastre
             s.rett(-0.55, -0.15, 0.62, 0.2);
             s.rett(0.55, -0.15, 0.62, 0.2);
             s.quota(1.9);
             s.rett(0.0, -0.3, 0.2, 0.5);
             s.sottomarino(0.05, 0.45, 1.5);
         }},
        {"banchisa", "storico", [](Simbolo& s) {   // il tetto di ghiaccio con le chiglie, il battello sotto
             s.poli({{-0.85, -0.75}, {0.85, -0.75}, {0.85, -0.35}, {0.5, -0.3}, {0.3, -0.05}, {0.1, -0.3}, {-0.3, -0.35},
                     {-0.45, -0.12}, {-0.6, -0.35}, {-0.85, -0.3}});
             s.quota(1.9);
             s.sottomarino(0.0, 0.42, 1.5);
         }},
        {"fanali", "tecnico", [](Simbolo& s) {   // la nave di notte: due bianchi d'albero e un laterale
             s.quota(1.0);
             s.nave(0.0, 0.42, 1.6);
             s.quota(1.9);
             s.stella(-0.25, -0.5, 8, 0.24, 0.07);
             s.stella(0.3, -0.3, 8, 0.24, 0.07);
             s.stella(0.55, 0.0, 8, 0.16, 0.05);
         }},
        {"luci_spente", "tecnico", [](Simbolo& s) {   // la nave, la luce sbarrata
             s.quota(1.0);
             s.nave(0.0, 0.45, 1.6);
             s.quota(1.9);
             s.stella(0.0, -0.4, 8, 0.28, 0.08);
             s.linea(-0.45, -0.75, 0.45, -0.05, 0.12);
         }},
        {"soccorso", "storico", [](Simbolo& s) {   // il salvagente, sobrio: nessuna arma
             s.anello(0.0, 0.0, 0.55, 0.24);
             for (const double a : {45.0, 135.0, 225.0, 315.0}) {
                 const double r = radianti(a);
                 s.linea(0.34 * std::cos(r), 0.34 * std::sin(r), 0.76 * std::cos(r), 0.76 * std::sin(r), 0.16);
             }
         }},
        {"in_coda", "stealth", [](Simbolo& s) {   // il battello nel cono di poppa dell'altro
             s.sottomarino(0.3, -0.45, 1.1);
             s.linea(-0.25, -0.45, -0.85, -0.85, 0.06);
             s.linea(-0.25, -0.45, -0.85, -0.05, 0.06);
             s.quota(1.9);
             s.sottomarino(-0.15, 0.45, 1.0);
             s.freccia(-0.15, 0.15, -0.45, -0.3, 0.07);
         }},
        {"cinquecento", "nascosto", [](Simbolo& s) {   // due battelli affiancati, la quota della distanza
             s.sottomarino(-0.08, -0.35, 1.3);
             s.sottomarino(0.08, 0.35, 1.3);
             s.linea(0.8, -0.3, 0.8, 0.3, 0.05);
             s.linea(0.7, -0.3, 0.9, -0.3, 0.05);
             s.linea(0.7, 0.3, 0.9, 0.3, 0.05);
         }},
        {"occhio_mine", "tattico", [](Simbolo& s) {   // l'occhio con la mina per pupilla
             s.arco(0.0, 0.62, 1.0, -142, -38, 0.13);
             s.arco(0.0, -0.62, 1.0, 38, 142, 0.13);
             s.anello(0.0, 0.0, 0.34, 0.07);
             s.cerchio(0.0, 0.0, 0.17);
             for (int k = 0; k < 6; ++k) {
                 const double r = radianti(60.0 * k);
                 s.linea(0.0, 0.0, 0.32 * std::cos(r), 0.32 * std::sin(r), 0.05);
             }
         }},
        {"aggirate", "tattico", [](Simbolo& s) {   // la rotta che gira attorno alla mina
             s.cerchio(0.0, 0.4, 0.18);
             for (int k = 0; k < 6; ++k) {
                 const double r = radianti(60.0 * k + 30.0);
                 s.linea(0.0, 0.4, 0.3 * std::cos(r), 0.4 + 0.3 * std::sin(r), 0.05);
             }
             s.linea(-0.75, 0.65, -0.5, -0.3, 0.1);
             s.linea(-0.5, -0.3, 0.5, -0.3, 0.1);
             s.freccia(0.5, -0.3, 0.75, 0.65, 0.1);
         }},
        {"sotto_rotore", "stealth", [](Simbolo& s) {   // l'elicottero col rotore, il battello sotto
             s.linea(-0.65, -0.72, 0.65, -0.72, 0.07);
             s.linea(0.0, -0.72, 0.0, -0.6, 0.06);
             s.ellisse(-0.05, -0.5, 0.3, 0.13);
             s.linea(0.2, -0.5, 0.6, -0.5, 0.06);
             s.quota(1.9);
             s.sottomarino(0.0, 0.42, 1.5);
         }},
        {"fra_le_code", "stealth", [](Simbolo& s) {   // due navi con le schiere filate, il battello che passa in mezzo
             s.quota(1.0);
             s.nave(0.4, -0.6, 0.8);
             s.nave(-0.35, 0.6, 0.8);
             s.linea(0.0, -0.55, -0.85, -0.35, 0.04);
             s.linea(-0.75, 0.65, -0.95, 0.75, 0.04);
             s.quota(1.9);
             s.sottomarino(0.05, 0.02, 1.1);
         }},
        {"catalogo", "tecnico", [](Simbolo& s) {   // tre sagome una sull'altra, come le tavole del libretto
             s.nave(0.0, -0.5, 1.1);
             s.sottomarino(0.0, 0.05, 1.1);
             s.nave(0.0, 0.6, 1.1);
         }},
        {"storie_vere", "storico", [](Simbolo& s) {   // il libro aperto con la stella
             s.poli({{-0.8, -0.2}, {-0.05, -0.05}, {-0.05, 0.7}, {-0.8, 0.55}});
             s.poli({{0.05, -0.05}, {0.8, -0.2}, {0.8, 0.55}, {0.05, 0.7}});
             s.quota(1.9);
             s.stella(0.0, -0.55, 5, 0.3, 0.12);
         }},
    };
    return v;
}

const Voce* voce(std::string_view id) {
    for (const auto& v : voci())
        if (id == v.id) return &v;
    return nullptr;
}

Mat smalto_di(std::string_view categoria) {
    if (categoria == "tattico") return ROSSO_M;
    if (categoria == "stealth") return NOTTE_M;
    if (categoria == "tecnico") return VERDE_M;
    if (categoria == "storico") return AZZURRO_M;
    if (categoria == "carriera") return BIANCO_M;
    if (categoria == "editor") return VIOLA_M;
    return NERO_M;
}

void punto_interrogativo(Simbolo& s) {
    s.arco(0.0, -0.3, 0.38, -100, 160, 0.2);
    s.linea(0.0, 0.1, 0.0, 0.32, 0.2);
    s.cerchio(0.0, 0.62, 0.13);
}

void ancora(Simbolo& s) {
    s.linea(0.0, -0.55, 0.0, 0.6, 0.15);
    s.anello(0.0, -0.68, 0.14, 0.08);
    s.linea(-0.4, -0.3, 0.4, -0.3, 0.12);
    s.arco(0.0, 0.05, 0.55, 110, 250, 0.14);
}

}  // namespace

const std::vector<std::string>& icone_medaglioni() {
    static const std::vector<std::string> v = [] {
        std::vector<std::string> r;
        for (const auto& x : voci()) r.emplace_back(x.id);
        return r;
    }();
    return v;
}

std::string categoria_medaglione(std::string_view icona) {
    const Voce* v = voce(icona);
    return v ? v->categoria : "";
}

void disegna_medaglione(Tela& t, Rettangolo r, std::string_view icona, LivelloMedaglione livello, bool preso, bool nascosto,
                        TavolozzaMedaglione tavolozza, std::string_view categoria) {
    const int lato = std::min(r.w, r.h);
    if (lato < 8) return;
    const int ox = r.x + (r.w - lato) / 2, oy = r.y + (r.h - lato) / 2;
    const double c = lato / 2.0, R = lato / 2.0 - 0.5, F = R * 0.78;   // bordo e campo
    const Voce* v = voce(icona);
    const Mat metallo = !preso ? GRIGIO_M : livello == LivelloMedaglione::oro ? ORO_M : livello == LivelloMedaglione::argento ? ARGENTO_M : BRONZO_M;
    const Mat smalto = !preso ? NERO_M : smalto_di(v ? std::string_view(v->categoria) : categoria);
    Rilievo ril(lato, lato);
    // il campo smaltato, quasi piatto, e il bordo in rilievo
    ril.forma([&](P p) { return sd_cerchio({p.x - c, p.y - c}, F + 0.5); }, 0.2, 0.15, 3.0, smalto);
    ril.forma([&](P p) { const double d = std::hypot(p.x - c, p.y - c); return std::max(d - R, F - d); }, 1.0, 1.0, std::max(1.0, (R - F) * 0.5), metallo);
    // argento e oro: le perline del bordo; oro: la stellina in alto
    if (preso && livello != LivelloMedaglione::bronzo && lato >= 32) {
        const int n = 24;
        const double rp = (R + F) / 2, rr = (R - F) * 0.32;
        for (int k = 0; k < n; ++k) {
            const double a = 2 * PI * k / n;
            const double px = c + rp * std::sin(a), py = c - rp * std::cos(a);
            ril.forma([=](P p) { return sd_cerchio({p.x - px, p.y - py}, rr); }, 1.6, 0.6, rr, metallo);
        }
    }
    // il simbolo (o il «?» dei nascosti non presi, o l'ancora se l'icona e' sconosciuta)
    Simbolo s(ril, c, c, F * 0.84, metallo, preso ? 1.3 : 0.7);
    if (nascosto && !preso) punto_interrogativo(s);
    else if (v) v->disegno(s);
    else ancora(s);
    if (preso && livello == LivelloMedaglione::oro) {
        Simbolo top(ril, c, c - (R + F) / 2, (R - F) * 0.9, ORO_M, 2.2);
        top.stella(0.0, 0.0, 5, 1.0, 0.42);
    }
    ril.disegna(t, ox, oy, rampe(tavolozza), preso ? 1.0 : 0.6);   // non preso: metallo spento, senza riflessi
}

}  // namespace m688
