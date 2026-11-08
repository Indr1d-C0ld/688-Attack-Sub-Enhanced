#include "grafica/modello.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <limits>
#include <stdexcept>

#include "grafica/tela.hpp"
#include "nucleo/inflate.hpp"
#include "nucleo/geometria.hpp"

namespace m688 {

namespace {

struct V3 {
    double x = 0, y = 0, z = 0;
};
V3 operator+(V3 a, V3 b) { return {a.x + b.x, a.y + b.y, a.z + b.z}; }
V3 operator-(V3 a, V3 b) { return {a.x - b.x, a.y - b.y, a.z - b.z}; }
V3 operator*(double k, V3 a) { return {k * a.x, k * a.y, k * a.z}; }
double punto(V3 a, V3 b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
V3 croce(V3 a, V3 b) { return {a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x}; }
V3 normalizza(V3 a) {
    const double n = std::sqrt(punto(a, a));
    return n > 1e-12 ? (1.0 / n) * a : a;
}

template <typename T>
T leggi(std::ifstream& f) {
    T v{};
    f.read(reinterpret_cast<char*>(&v), sizeof v);
    return v;
}

std::vector<std::uint8_t> leggi_blocco(std::ifstream& f, std::size_t atteso, const std::string& percorso) {
    const auto n = leggi<std::uint32_t>(f);
    std::vector<std::uint8_t> z(n);
    f.read(reinterpret_cast<char*>(z.data()), static_cast<std::streamsize>(n));
    if (!f) throw std::runtime_error(percorso + ": modello troncato");
    try {
        return interno::inflate(z.data(), z.size(), atteso);
    } catch (const std::runtime_error& e) {
        throw std::runtime_error(percorso + ": " + e.what());
    }
}

// Assetto di un corpo rigido (mondo.hpp): rotazioni attorno al perno, poi spostamento e immersione.
V3 posa_punto(const AssettoScafo& a, V3 p) {
    p.x -= a.x_perno;
    const double t = radianti(a.assetto), s = radianti(a.sbandamento);
    const V3 q{p.x * std::cos(t) - p.z * std::sin(t), p.y, p.x * std::sin(t) + p.z * std::cos(t)};     // prua alta
    const V3 w{q.x, q.y * std::cos(s) - q.z * std::sin(s), q.y * std::sin(s) + q.z * std::cos(s)};     // dritta giu'
    return {w.x + a.x_perno + a.dx, w.y + a.dy, w.z - a.immersione};
}

// Matrice di Bayer 4x4 della pipeline (costruisci.BAYER4), gia' scalata: (M / 16 - 0,47) * 0,7
constexpr int BAYER[4][4] = {{0, 8, 2, 10}, {12, 4, 14, 6}, {3, 11, 1, 9}, {15, 7, 13, 5}};
double soglia_bayer(int x, int y) { return (BAYER[y & 3][x & 3] / 16.0 - 0.47) * 0.7; }

int div_inf(int a, int b) { return a >= 0 ? a / b : -((-a + b - 1) / b); }   // divisione per difetto

// rumore di valore 3D (0..1) su un reticolo di 1 m: bordi frastagliati delle falle e chiazze di fuliggine
double rumore3(double x, double y, double z) {
    auto h = [](int i, int j, int k) {
        std::uint32_t a = static_cast<std::uint32_t>(i) * 0x8da6b343u ^ static_cast<std::uint32_t>(j) * 0xd8163841u ^
                          static_cast<std::uint32_t>(k) * 0xcb1ab31fu;
        a ^= a >> 15; a *= 0x2c1b3c6du; a ^= a >> 12;
        return (a & 0xffffu) / 65535.0;
    };
    const double fx = std::floor(x), fy = std::floor(y), fz = std::floor(z);
    const int i = static_cast<int>(fx), j = static_cast<int>(fy), k = static_cast<int>(fz);
    const double u = x - fx, v = y - fy, w = z - fz;
    auto l = [](double a, double b, double t) { return a + (b - a) * t; };
    return l(l(l(h(i, j, k), h(i + 1, j, k), u), l(h(i, j + 1, k), h(i + 1, j + 1, k), u), v),
             l(l(h(i, j, k + 1), h(i + 1, j, k + 1), u), l(h(i, j + 1, k + 1), h(i + 1, j + 1, k + 1), u), v), w);
}

// Segni dello scafo sul valore di un punto del modello intatto: falle nere dal bordo frastagliato con un alone
// bruciato, fuliggine a chiazze attorno ai locali che hanno bruciato.
double segna(double valore, const std::vector<SegnoScafo>& segni, double x, double y, double z) {
    for (const auto& s : segni) {
        const double dx = (x - s.x) / s.rx, dy = (y - s.y) / s.ry, dz = (z - s.z) / s.rz;
        const double d2 = dx * dx + dy * dy + dz * dz;
        if (s.tipo == SegnoScafo::Tipo::falla) {
            if (d2 > 2.5) continue;
            const double bordo = 0.7 + 0.6 * rumore3(x / 0.7, y / 0.7, z / 0.7);
            const double d = std::sqrt(d2);
            if (d < bordo) return 0.03;                                           // buco
            if (d < 1.4 * bordo) valore *= 0.25 + 0.75 * (d - bordo) / (0.4 * bordo);   // lamiere annerite
        } else {
            if (d2 > 4.0) continue;
            const double chiazze = 0.55 + 0.45 * rumore3(x / 2.5, y / 2.5, z / 1.5);
            valore *= 1.0 - 0.8 * s.intensita * chiazze * std::exp(-d2);
        }
    }
    return valore;
}

// Rotazione di p attorno all'asse orizzontale (ax, ay, 0) passante per il perno (Rodrigues)
V3 ruota_parte(const ParteMossa& m, V3 p) {
    const V3 k = normalizza({m.ax, m.ay, 0.0}), o{m.px, m.py, m.pz};
    const V3 v = p - o;
    const double c = std::cos(m.angolo), s = std::sin(m.angolo);
    return o + (c * v + s * croce(k, v) + (punto(k, v) * (1 - c)) * k);
}

}  // namespace

Modello Modello::carica(const std::string& percorso) {
    std::ifstream f(percorso, std::ios::binary);
    if (!f) throw std::runtime_error("impossibile aprire " + percorso);
    char firma[4];
    f.read(firma, 4);
    if (!f || std::memcmp(firma, "MDL2", 4) != 0)
        throw std::runtime_error(percorso + ": non e' un modello MDL2 (riesportare con tools/navi/esporta_modelli.sh)");
    Modello m;
    const auto n = leggi<std::uint32_t>(f);
    m.tw_ = static_cast<int>(leggi<std::uint32_t>(f));
    m.th_ = static_cast<int>(leggi<std::uint32_t>(f));
    leggi<float>(f);   // lato del texel (informativo)
    m.tono_ = leggi<float>(f);
    m.ambiente_ = leggi<float>(f);
    m.lunghezza_ = leggi<float>(f);
    m.quota_max_ = leggi<float>(f);
    const auto np = leggi<std::uint16_t>(f);
    for (int i = 0; i < np; ++i) {
        const auto l = leggi<std::uint8_t>(f);
        std::string s(l, '\0');
        f.read(s.data(), l);
        m.parti_.push_back(std::move(s));
    }
    if (!f || m.tw_ < 2 || m.th_ < 2) throw std::runtime_error(percorso + ": modello troncato");

    // record dei triangoli per piani di byte (il byte j del record i sta in j * n + i)
    constexpr std::size_t RECORD = 91;
    const auto piani = leggi_blocco(f, RECORD * n, percorso);
    m.tri_.resize(n);
    std::array<std::uint8_t, RECORD> r{};
    for (std::size_t i = 0; i < n; ++i) {
        for (std::size_t j = 0; j < RECORD; ++j) r[j] = piani[j * n + i];
        const std::uint8_t* p = r.data();
        auto f32 = [&p] { float v; std::memcpy(&v, p, 4); p += 4; return v; };
        auto& t = m.tri_[i];
        for (auto& v : t.v)
            for (auto& c : v) c = f32();
        for (auto& faccia : t.uv)
            for (auto& uv : faccia)
                for (auto& c : uv) c = f32();
        t.albedo = f32();
        t.retinato = (p[0] & 1) != 0;
        t.parte = static_cast<std::uint16_t>(p[1] | (p[2] << 8));
    }

    // quota delle foto; poi il valore dei texel con quota > 0, come scarto dalla predizione MED
    // (esporta_modello.predizione)
    const auto W = static_cast<std::size_t>(m.tw_), texel = W * static_cast<std::size_t>(m.th_);
    m.quota_foto_ = leggi_blocco(f, texel, percorso);
    const auto& q = m.quota_foto_;
    const auto viste = static_cast<std::size_t>(std::count_if(q.begin(), q.end(), [](std::uint8_t c) { return c != 0; }));
    const auto scarti = leggi_blocco(f, viste, percorso);
    m.valore_.assign(texel, 0);
    auto& v = m.valore_;
    std::size_t k = 0;
    for (std::size_t i = 0; i < texel; ++i) {
        if (!q[i]) continue;
        const std::size_t x = i % W;
        const bool va = x > 0 && q[i - 1], vb = i >= W && q[i - W], vc = x > 0 && i >= W && q[i - W - 1];
        int pred = 128;
        if (va && vb && vc) {
            const int a = v[i - 1], b = v[i - W], c = v[i - W - 1];
            pred = c >= std::max(a, b) ? std::min(a, b) : c <= std::min(a, b) ? std::max(a, b) : a + b - c;
        } else if (va) {
            pred = v[i - 1];
        } else if (vb) {
            pred = v[i - W];
        }
        v[i] = static_cast<std::uint8_t>(pred + scarti[k++]);
    }
    return m;
}

void Modello::campiona(float u, float v, float& valore, float& quota_foto) const {
    // bilineare fra i centri dei texel (come esporta_modello.rendi_cotto)
    u -= 0.5f;
    v -= 0.5f;
    const int u0 = std::clamp(static_cast<int>(std::floor(u)), 0, tw_ - 2), v0 = std::clamp(static_cast<int>(std::floor(v)), 0, th_ - 2);
    const float fu = std::clamp(u - static_cast<float>(u0), 0.0f, 1.0f), fv = std::clamp(v - static_cast<float>(v0), 0.0f, 1.0f);
    auto bil = [&](const std::vector<std::uint8_t>& t) {
        const std::size_t i = static_cast<std::size_t>(v0) * static_cast<std::size_t>(tw_) + static_cast<std::size_t>(u0);
        const std::size_t j = i + static_cast<std::size_t>(tw_);
        return ((t[i] * (1 - fu) + t[i + 1] * fu) * (1 - fv) + (t[j] * (1 - fu) + t[j + 1] * fu) * fv) / 255.0f;
    };
    valore = bil(valore_);
    quota_foto = bil(quota_foto_);
}

std::array<double, 15> rampa_grigi(const Palette& giorno) {
    std::array<double, 15> r{};
    for (int i = 0; i < 15; ++i) r[static_cast<std::size_t>(i)] = giorno.rgb(colori::GRIGI + i)[0];
    return r;
}

Sprite genera_sprite(const Modello& m, const PosaNave& posa, double aob, bool dritta, const std::array<double, 15>& rampa,
                     double scala_px_m, int sovracampiona) {
    // camera degli sprite (costruisci.camera_vista): da 4 km, occhio a 2 m, mirata a 0,4 volte l'altezza della vista
    const int SS = std::clamp(sovracampiona, 1, 4);
    constexpr double D = 4000.0, OCCHIO = 2.0;
    const double L = m.lunghezza_m(), alto = std::max(46.0, m.quota_max_m() * 1.08 + 2.0);
    const double a = radianti(aob), lato = dritta ? 1.0 : -1.0;
    const V3 C{D * std::cos(a), -lato * D * std::sin(a), OCCHIO};
    const V3 bersaglio{0.0, 0.0, alto * 0.40};
    const V3 d = bersaglio - C;
    const double yaw = std::atan2(d.y, d.x), pitch = std::atan2(d.z, std::hypot(d.x, d.y));
    const V3 fw{std::cos(pitch) * std::cos(yaw), std::cos(pitch) * std::sin(yaw), std::sin(pitch)};
    const V3 dx = normalizza(croce(fw, {0, 0, 1})), dy = croce(dx, fw);
    const double f = scala_px_m * SS * D;
    const int W0 = static_cast<int>(L * 1.12 * scala_px_m * SS) / 2 * 2, H0 = static_cast<int>(alto * scala_px_m * SS);
    auto proietta = [&](V3 p, double& px, double& py) {
        const V3 v = p - C;
        const double z = punto(v, fw);
        px = W0 / 2.0 + f * punto(v, dx) / z;
        py = H0 / 2.0 - f * punto(v, dy) / z;
        return z;
    };

    // vertici nella posa: i triangoli a prua della rottura seguono il troncone prodiero
    const auto& tri = m.triangoli();
    std::vector<std::array<V3, 3>> P(tri.size());
    for (std::size_t k = 0; k < tri.size(); ++k) {
        const auto& t = tri[k];
        const double cx = (t.v[0][0] + t.v[1][0] + t.v[2][0]) / 3.0;
        const AssettoScafo& as = posa.spezzata && cx < posa.x_rottura ? posa.poppiero : posa.prodiero;
        const ParteMossa* mossa = nullptr;   // alberi caduti: prima la rotazione della parte, poi l'assetto
        for (const auto& pm : posa.parti) if (pm.parte == t.parte) mossa = &pm;
        for (int i = 0; i < 3; ++i) {
            V3 v{t.v[i][0], t.v[i][1], t.v[i][2]};
            if (mossa) v = ruota_parte(*mossa, v);
            if (posa.flessione_m != 0.0) {
                const double u = (v.x - posa.flessione_x) / posa.flessione_larghezza;
                v.z += posa.flessione_m * std::exp(-u * u);
            }
            P[k][static_cast<std::size_t>(i)] = posa_punto(as, v);
        }
    }

    // tela: quella degli sprite, allargata a multipli di 4 pixel (12 sottopixel) dove il modello in posa esce,
    // cosi' la retinatura resta in fase con gli sprite precalcolati
    const int w2 = W0 / SS, h2 = H0 / SS;
    double xmin = 0, xmax = w2 * SS, ymin = 0, ymax = h2 * SS;
    for (const auto& p : P) {
        if (p[0].z < 0 && p[1].z < 0 && p[2].z < 0) continue;   // tutto sott'acqua
        for (const auto& q : p) {
            double px, py;
            if (proietta(q, px, py) <= 0.1) continue;
            xmin = std::min(xmin, px); xmax = std::max(xmax, px); ymin = std::min(ymin, py); ymax = std::max(ymax, py);
        }
    }
    const int X0 = div_inf(static_cast<int>(std::floor(xmin)), 4 * SS) * 4 * SS, Y0 = div_inf(static_cast<int>(std::floor(ymin)), 4 * SS) * 4 * SS;
    const int X1 = std::max(w2 * SS, (static_cast<int>(std::ceil(xmax)) / SS + 1) * SS);
    const int Y1 = std::max(h2 * SS, (static_cast<int>(std::ceil(ymax)) / SS + 1) * SS);
    const int Wc = X1 - X0, Hc = Y1 - Y0;
    const std::size_t nsp = static_cast<std::size_t>(Wc) * static_cast<std::size_t>(Hc);
    std::vector<float> zbuf(nsp, std::numeric_limits<float>::infinity()), b0buf(nsp), b1buf(nsp);
    std::vector<std::int32_t> tid(nsp, -1);

    // rasterizzazione (raster.gbuffer): baricentriche corrette in prospettiva, niente sotto il livello del mare
    for (std::size_t k = 0; k < tri.size(); ++k) {
        const auto& p = P[k];
        if (p[0].z < 0 && p[1].z < 0 && p[2].z < 0) continue;
        double xs[3], ys[3], zs[3];
        bool dietro = false;
        for (int i = 0; i < 3; ++i) {
            zs[i] = proietta(p[static_cast<std::size_t>(i)], xs[i], ys[i]);
            xs[i] -= X0; ys[i] -= Y0;
            dietro |= zs[i] <= 0.1;
        }
        if (dietro) continue;
        const int x0 = std::max(static_cast<int>(std::floor(std::min({xs[0], xs[1], xs[2]}))), 0);
        const int x1 = std::min(static_cast<int>(std::ceil(std::max({xs[0], xs[1], xs[2]}))), Wc - 1);
        const int y0 = std::max(static_cast<int>(std::floor(std::min({ys[0], ys[1], ys[2]}))), 0);
        const int y1 = std::min(static_cast<int>(std::ceil(std::max({ys[0], ys[1], ys[2]}))), Hc - 1);
        if (x0 > x1 || y0 > y1) continue;
        const double den = (ys[1] - ys[2]) * (xs[0] - xs[2]) + (xs[2] - xs[1]) * (ys[0] - ys[2]);
        if (std::fabs(den) < 1e-12) continue;
        const bool retinato = tri[k].retinato;
        for (int y = y0; y <= y1; ++y) {
            const double gy = y + 0.5;
            for (int x = x0; x <= x1; ++x) {
                const double gx = x + 0.5;
                const double l0 = ((ys[1] - ys[2]) * (gx - xs[2]) + (xs[2] - xs[1]) * (gy - ys[2])) / den;
                const double l1 = ((ys[2] - ys[0]) * (gx - xs[2]) + (xs[0] - xs[2]) * (gy - ys[2])) / den;
                const double l2 = 1.0 - l0 - l1;
                if (l0 < 0 || l1 < 0 || l2 < 0) continue;
                if (retinato && ((div_inf(x + X0, SS) + div_inf(y + Y0, SS)) & 1)) continue;   // dischi a scacchiera
                const double zz = 1.0 / (l0 / zs[0] + l1 / zs[1] + l2 / zs[2]);
                const double b0 = l0 / zs[0] * zz, b1 = l1 / zs[1] * zz, b2 = 1.0 - b0 - b1;
                if (b0 * p[0].z + b1 * p[1].z + b2 * p[2].z < 0.0) continue;   // sotto il livello del mare
                const std::size_t i = static_cast<std::size_t>(y) * static_cast<std::size_t>(Wc) + static_cast<std::size_t>(x);
                if (zz >= zbuf[i]) continue;
                zbuf[i] = static_cast<float>(zz);
                tid[i] = static_cast<std::int32_t>(k);
                b0buf[i] = static_cast<float>(b0);
                b1buf[i] = static_cast<float>(b1);
            }
        }
    }

    // colore dei sottopixel (costruisci._rendi): foto cotte e ombreggiatura di ripiego con il Sole alle spalle
    const V3 sole = normalizza(0.55 * (-1.0 * fw) + 0.35 * dx + 0.75 * V3{0, 0, 1});
    const double amb = m.luce_ambiente(), tono = m.tono();
    std::vector<V3> normali(tri.size());
    for (std::size_t k = 0; k < tri.size(); ++k) normali[k] = normalizza(croce(P[k][1] - P[k][0], P[k][2] - P[k][0]));
    const int ws = Wc / SS, hs = Hc / SS;
    std::vector<double> valori(static_cast<std::size_t>(ws) * static_cast<std::size_t>(hs), std::nan(""));
    for (int by = 0; by < hs; ++by)
        for (int bx = 0; bx < ws; ++bx) {
            double somma = 0;
            int coperti = 0;
            for (int sy = 0; sy < SS; ++sy)
                for (int sx = 0; sx < SS; ++sx) {
                    const std::size_t i = static_cast<std::size_t>(by * SS + sy) * static_cast<std::size_t>(Wc) +
                                          static_cast<std::size_t>(bx * SS + sx);
                    const int k = tid[i];
                    if (k < 0) continue;
                    const auto& t = tri[static_cast<std::size_t>(k)];
                    const auto& p = P[static_cast<std::size_t>(k)];
                    const double b0 = b0buf[i], b1 = b1buf[i], b2 = 1.0 - b0 - b1;
                    const V3 pos = b0 * p[0] + b1 * p[1] + b2 * p[2];
                    V3 n = normali[static_cast<std::size_t>(k)];
                    const bool opposta = punto(n, C - pos) < 0;
                    if (opposta) n = -1.0 * n;
                    const auto& uv = t.uv[opposta ? 1 : 0];
                    const float u = static_cast<float>(b0 * uv[0][0] + b1 * uv[1][0] + b2 * uv[2][0]);
                    const float v = static_cast<float>(b0 * uv[0][1] + b1 * uv[1][1] + b2 * uv[2][1]);
                    float val, mix;
                    m.campiona(u, v, val, mix);
                    const double zmod = b0 * t.v[0][2] + b1 * t.v[1][2] + b2 * t.v[2][2];
                    const double ombra = std::clamp(t.albedo * (amb + (1 - amb) * std::clamp(punto(n, sole), 0.0, 1.0)) *
                                                        (0.85 + 0.15 * std::clamp(zmod / 15.0, 0.0, 1.0)) / 0.62 * tono, 0.0, 1.0);
                    double g = (1.0 - mix) * ombra + mix * val;
                    if (!posa.segni.empty())
                        g = segna(g, posa.segni, b0 * t.v[0][0] + b1 * t.v[1][0] + b2 * t.v[2][0],
                                  b0 * t.v[0][1] + b1 * t.v[1][1] + b2 * t.v[2][1], zmod);
                    somma += g;
                    ++coperti;
                }
            if (coperti >= 4) valori[static_cast<std::size_t>(by) * static_cast<std::size_t>(ws) + static_cast<std::size_t>(bx)] = somma / coperti;
        }

    // 16 grigi con retinatura (costruisci.sprite_ancorato), in fase con la tela degli sprite
    const double passo = (rampa[14] - rampa[0]) / 14.0;
    std::vector<std::uint8_t> indici(valori.size(), TRASPARENTE);
    int xa = ws, xb = -1, ya = hs, yb = -1;
    for (int y = 0; y < hs; ++y)
        for (int x = 0; x < ws; ++x) {
            const double g = valori[static_cast<std::size_t>(y) * static_cast<std::size_t>(ws) + static_cast<std::size_t>(x)];
            if (std::isnan(g)) continue;
            const double q = (g * 255.0 - rampa[0]) / passo + soglia_bayer(x + X0 / SS, y + Y0 / SS);
            indici[static_cast<std::size_t>(y) * static_cast<std::size_t>(ws) + static_cast<std::size_t>(x)] =
                static_cast<std::uint8_t>(colori::GRIGI + std::clamp(static_cast<int>(std::nearbyint(q)), 0, 14));
            xa = std::min(xa, x); xb = std::max(xb, x); ya = std::min(ya, y); yb = std::max(yb, y);
        }
    Sprite s;
    s.aob = static_cast<int>(std::lround(aob));
    s.dritta = dritta;
    if (xb < 0) return s;
    s.w = xb - xa + 1;
    s.h = yb - ya + 1;
    s.pix.resize(static_cast<std::size_t>(s.w) * static_cast<std::size_t>(s.h));
    for (int y = 0; y < s.h; ++y)
        std::memcpy(s.pix.data() + static_cast<std::size_t>(y) * static_cast<std::size_t>(s.w),
                    indici.data() + static_cast<std::size_t>(y + ya) * static_cast<std::size_t>(ws) + static_cast<std::size_t>(xa),
                    static_cast<std::size_t>(s.w));
    // ancora: centro nave sul livello del mare (colonna del centro della tela degli sprite)
    double ox, oy;
    proietta({0, 0, 0}, ox, oy);
    s.ancora_x = static_cast<float>(w2 / 2.0 - X0 / SS - xa);
    s.ancora_y = static_cast<float>((oy - Y0) / SS - ya);
    return s;
}

}  // namespace m688
