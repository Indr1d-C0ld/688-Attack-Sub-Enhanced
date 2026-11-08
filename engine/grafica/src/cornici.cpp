#include "grafica/cornici.hpp"

#include <algorithm>
#include <cmath>
#include <string>

#include "grafica/quadro.hpp"
#include "grafica/tema_stazioni.hpp"
#include "nucleo/geometria.hpp"

namespace m688 {

using namespace cerimonia;

namespace {

constexpr int GRIGI = 144;   // 144-158: dal nero al quasi bianco

int grigio(double t) { return GRIGI + std::clamp(static_cast<int>(std::lround(t)), 0, 14); }

}  // namespace

void cornice(Tela& t, Rettangolo r, StileCornice s, int sp, bool vista) {
    for (int y = r.y; y < r.y + r.h; ++y)
        for (int x = r.x; x < r.x + r.w; ++x) {
            const int dx = std::min(x - r.x, r.x + r.w - 1 - x), dy = std::min(y - r.y, r.y + r.h - 1 - y);
            const int d = std::min(dx, dy);
            if (d >= sp) continue;
            const bool alto_sx = (x - r.x) + (y - r.y) < (r.x + r.w - 1 - x) + (r.y + r.h - 1 - y);   // lato in luce
            const double f = static_cast<double>(d) / std::max(sp - 1, 1);   // 0 fuori, 1 dentro
            int col = 0;
            switch (s) {
                case StileCornice::console: {
                    double g = 8.5 + (alto_sx ? 3.5 : -3.0) * (1 - f) + (d == 0 ? -4 : 0) + (d == sp - 1 ? -5 : 0);
                    col = grigio(g);
                    break;
                }
                case StileCornice::oro:
                case StileCornice::rossa: {
                    // modanatura: filo scuro, toro in luce, gola, filo interno
                    const double prof = std::sin(f * PI * 1.6);
                    double tt = 3.0 + 3.0 * prof * (alto_sx ? 1.0 : 0.55) + (d == 0 || d == sp - 1 ? -2.5 : 0.0);
                    if (vista) col = tt > 4 ? 14 : tt > 2.5 ? 6 : 8;
                    else if (s == StileCornice::rossa && f > 0.35 && f < 0.75) col = SMALTO + retina(tt * 0.8, x, y, N_SMALTO - 1);
                    else col = ORO + retina(tt, x, y, N_ORO - 1);
                    break;
                }
                case StileCornice::legno: {
                    const double vena = 0.6 * std::sin((x + 3 * std::sin(y * 0.15)) * 0.35) + 0.5 * caso(x / 3, y, 4);
                    const double tt = 3.2 + (alto_sx ? 1.6 : -1.2) * (1 - f) + vena + (d == 0 ? -2 : 0);
                    col = vista ? (tt > 3.5 ? 6 : 8) : LEGNO + retina(tt, x, y, N_LEGNO - 1);
                    break;
                }
            }
            t.pixel(x, y, static_cast<std::uint8_t>(col));
        }
    if (s == StileCornice::console && sp >= 5)   // viti agli angoli
        for (const int cx : {r.x + sp / 2, r.x + r.w - 1 - sp / 2})
            for (const int cy : {r.y + sp / 2, r.y + r.h - 1 - sp / 2})
                for (int y = -1; y <= 1; ++y)
                    for (int x = -1; x <= 1; ++x)
                        t.pixel(cx + x, cy + y, static_cast<std::uint8_t>(grigio(x == y ? 2 : (x + y < 0 ? 13 : 5))));
}

void pannello(Tela& t, Rettangolo r, bool incavato, bool vista) {
    (void)vista;
    for (int y = r.y; y < r.y + r.h; ++y)
        for (int x = r.x; x < r.x + r.w; ++x) {
            const bool bordo_alto = y == r.y || x == r.x, bordo_basso = y == r.y + r.h - 1 || x == r.x + r.w - 1;
            double g = incavato ? 4.5 : 8.0;
            if (bordo_alto) g = incavato ? 2 : 12;
            if (bordo_basso) g = incavato ? 11 : 3;
            t.pixel(x, y, static_cast<std::uint8_t>(grigio(g)));
        }
}

void pulsante(Tela& t, const Font& f, Rettangolo r, std::string_view testo, bool acceso) {
    pannello(t, r, acceso);
    const int w = f.larghezza(testo, 1);
    f.scrivi(t, r.x + (r.w - w) / 2 + (acceso ? 1 : 0), r.y + (r.h - f.altezza_riga(1)) / 2 + 2 + (acceso ? 1 : 0), testo,
             static_cast<std::uint8_t>(acceso ? 14 : 15), 1);
}

void carta(Tela& t, Rettangolo r, std::uint32_t seme) {
    const int s = static_cast<int>(seme);
    for (int y = r.y; y < r.y + r.h; ++y)
        for (int x = r.x; x < r.x + r.w; ++x) {
            const double fibra = 0.5 * caso(x, y, s) + 0.35 * caso(x / 7, y, s + 1);
            const int dx = std::min(x - r.x, r.x + r.w - 1 - x), dy = std::min(y - r.y, r.y + r.h - 1 - y);
            const double bordo = std::clamp(std::min(dx, dy) / 6.0, 0.0, 1.0);
            const double piega = std::exp(-std::pow((y - (r.y + r.h * 0.48)) / 2.5, 2.0));
            const double tt = 2.4 + 2.2 * bordo + fibra - 1.6 * piega - 0.6 * (1 - bordo) * caso(x, y / 3, s + 2);
            // bordo strappato
            if (std::min(dx, dy) == 0 && caso(x, y, s + 3) < 0.35) continue;
            t.pixel(x, y, static_cast<std::uint8_t>(CARTA + retina(tt, x, y, N_CARTA - 1)));
        }
}

void intestazione(Tela& t, const Font& f, Rettangolo r, std::string_view titolo, std::string_view sottotitolo, Marina m,
                  bool vista) {
    for (int y = r.y; y < r.y + r.h; ++y)
        for (int x = r.x; x < r.x + r.w; ++x) {
            const double v = static_cast<double>(y - r.y) / std::max(r.h - 1, 1);
            int col;
            if (vista) col = grigio(2.5 + 2.0 * (1 - v) + (y == r.y + r.h - 1 ? 6 : 0));
            else col = (m == Marina::usn ? BLU : NERO) + retina(1.0 + 3.0 * (1 - v), x, y, 7);
            if (y == r.y + r.h - 1 || y == r.y + r.h - 3) col = vista ? 14 : ORO + 5;
            t.pixel(x, y, static_cast<std::uint8_t>(col));
        }
    // emblemi agli angoli: tre stelle bianche (USN) o la stella rossa (VMF)
    auto emblema = [&](int cx, int cy) {
        for (int k = 0; k < (m == Marina::usn ? 3 : 1); ++k) {
            const int ox = cx + (m == Marina::usn ? (k - 1) * 9 : 0);
            for (int y = -4; y <= 4; ++y)
                for (int x = -4; x <= 4; ++x) {
                    const double a = std::atan2(x, -y), rr = std::hypot(x, y);
                    const double ff = std::fmod(a + 2 * PI + PI / 5, 2 * PI / 5) - PI / 5;
                    if (rr <= 4.3 * (0.42 + 0.58 * std::pow(std::fabs(std::cos(ff * 2.5)), 6.0)) || rr < 1.9)
                        t.pixel(ox + x, cy + y, static_cast<std::uint8_t>(m == Marina::usn ? 15 : (vista ? 12 : SMALTO + 4)));
                }
        }
    };
    emblema(r.x + 22, r.y + r.h / 2);
    emblema(r.x + r.w - 23, r.y + r.h / 2);
    // il titolo grande; se fra i due emblemi non ci sta (i nomi lunghi delle medaglie sovietiche), in piccolo
    const int posto = r.w - 2 * 40;
    const int scala = f.larghezza(titolo, 2) <= posto ? 2 : 1;
    std::string tt(titolo);
    while (tt.size() > 1 && f.larghezza(tt, scala) > posto) tt.pop_back();
    const int wt = f.larghezza(tt, scala);
    const int yt = scala == 2 ? r.y + 3 : r.y + 3 + f.altezza_riga(1) / 2;
    f.scrivi(t, r.x + (r.w - wt) / 2, yt, tt, static_cast<std::uint8_t>(vista ? 15 : ORO + 7), scala);
    if (!sottotitolo.empty()) {
        const int ws = f.larghezza(sottotitolo, 1);
        f.scrivi(t, r.x + (r.w - ws) / 2, r.y + r.h - 15, sottotitolo, static_cast<std::uint8_t>(vista ? 7 : ORO + 4), 1);
    }
}

void timbro(Tela& t, const Font& f, int cx, int cy, std::string_view testo, double gradi, int scala, std::uint32_t seme,
            bool vista) {
    // il testo si compone dritto in una tela di appoggio, poi si ruota sulla tela con un bordo doppio e l'usura
    const int tw = f.larghezza(testo, scala) + 8 * scala, th = f.altezza_riga(scala) + 4 * scala;
    Tela m(tw + 8, th + 8);
    m.riempi(0);
    f.scrivi(m, 4 + 4 * scala, 4 + 2 * scala + scala, testo, 1, scala);
    const int bordo = std::max(1, scala);
    for (int y = 0; y < th; ++y)
        for (int x = 0; x < tw; ++x) {
            const bool b1 = x < bordo || y < bordo || x >= tw - bordo || y >= th - bordo;
            const bool b2 = (x >= 2 * bordo && x < 3 * bordo) || (y >= 2 * bordo && y < 3 * bordo) || (x < tw - 2 * bordo && x >= tw - 3 * bordo) ||
                            (y < th - 2 * bordo && y >= th - 3 * bordo);
            const bool dentro2 = x >= 2 * bordo && y >= 2 * bordo && x < tw - 2 * bordo && y < th - 2 * bordo;
            if (b1 || (b2 && dentro2)) m.pixel(4 + x, 4 + y, 1);
        }
    const double a = gradi * PI / 180.0, ca = std::cos(a), sa = std::sin(a);
    const int raggio = static_cast<int>(std::hypot(tw, th) / 2) + 6;
    const int s = static_cast<int>(seme);
    for (int y = -raggio; y <= raggio; ++y)
        for (int x = -raggio; x <= raggio; ++x) {
            const double u = x * ca + y * sa + m.larghezza() / 2.0, v = -x * sa + y * ca + m.altezza() / 2.0;
            const int iu = static_cast<int>(std::floor(u)), iv = static_cast<int>(std::floor(v));
            if (iu < 0 || iv < 0 || iu >= m.larghezza() || iv >= m.altezza() || m.at(iu, iv) == 0) continue;
            // inchiostro: chiazze piu' scariche e puntini mancanti
            const double usura = 0.6 * caso((cx + x) / 4, (cy + y) / 4, s) + 0.4 * caso(cx + x, cy + y, s + 1);
            if (usura < 0.22) continue;
            const int col = vista ? (usura > 0.55 ? 4 : 12) : TIMBRO + std::clamp(static_cast<int>(usura * 4.4) - 1, 0, 3);
            t.pixel(cx + x, cy + y, static_cast<std::uint8_t>(col));
        }
}

namespace {

int spazio(const Font& f) { return f.formato() == 2 ? f.spazio_predefinito() : 1; }
int largo(const Font& f, std::string_view s) { return f.larghezza(s, 1, spazio(f), f.formato() != 2); }
void scrivi(Tela& t, const Font& f, int x, int y, std::string_view s, int c) {
    f.scrivi(t, x, y, s, static_cast<std::uint8_t>(c), 1, spazio(f), f.formato() != 2);
}
void filo(Tela& t, Rettangolo r, int d, int c) {
    const auto k = static_cast<std::uint8_t>(c);
    t.linea_orizzontale(r.x - d, r.x + r.w - 1 + d, r.y - d, k);
    t.linea_orizzontale(r.x - d, r.x + r.w - 1 + d, r.y + r.h - 1 + d, k);
    t.linea_verticale(r.x - d, r.y - d, r.y + r.h - 1 + d, k);
    t.linea_verticale(r.x + r.w - 1 + d, r.y - d, r.y + r.h - 1 + d, k);
}

}  // namespace

void posto_libero(Tela& t, Rettangolo r, const Font& f, int numero) {
    const auto c = static_cast<std::uint8_t>(LEGNO + 6);
    for (int x = r.x; x < r.x + r.w; ++x)
        if ((x - r.x) / 4 % 2 == 0) t.pixel(x, r.y, c), t.pixel(x, r.y + r.h - 1, c);
    for (int y = r.y; y < r.y + r.h; ++y)
        if ((y - r.y) / 4 % 2 == 0) t.pixel(r.x, y, c), t.pixel(r.x + r.w - 1, y, c);
    const std::string s = std::to_string(numero);
    scrivi(t, f, r.x + (r.w - largo(f, s)) / 2, r.y + (r.h - f.righe()) / 2, s, c);
}

void piede_cerimonia(Tela& t, const Font& f, int y, std::string_view aiuto, const std::vector<TastoCerimonia>& tasti,
                     int predefinito) {
    const int L = t.larghezza();
    t.rettangolo_pieno(0, y, L, t.altezza() - y, static_cast<std::uint8_t>(LEGNO + 1));
    t.linea_orizzontale(0, L - 1, y, static_cast<std::uint8_t>(ORO + 4));
    t.linea_orizzontale(0, L - 1, y + 1, static_cast<std::uint8_t>(LEGNO));
    scrivi(t, f, 12, y + 6, aiuto, BIANCO + 4);
    const int n = static_cast<int>(tasti.size());
    if (n == 0) return;
    const int gap = 8, w = (L - 24 - gap * (n - 1)) / n, h = std::min(24, t.altezza() - y - 26);
    for (int i = 0; i < n; ++i) {
        const Rettangolo r{12 + i * (w + gap), y + 20, w, h};
        const auto& k = tasti[static_cast<std::size_t>(i)];
        if (i == predefinito) filo(t, r, 1, ORO + 7), filo(t, r, 2, ORO + 7);
        const int ty = r.y + (r.h - f.righe()) / 2;
        if (k.attivo) {   // l'ottone: luce in alto a sinistra, due ribattini
            t.rettangolo_pieno(r.x, r.y, r.w, r.h, static_cast<std::uint8_t>(ORO + 3));
            t.linea_orizzontale(r.x, r.x + r.w - 1, r.y, static_cast<std::uint8_t>(ORO + 6));
            t.linea_verticale(r.x, r.y, r.y + r.h - 1, static_cast<std::uint8_t>(ORO + 6));
            t.linea_orizzontale(r.x, r.x + r.w - 1, r.y + r.h - 1, static_cast<std::uint8_t>(ORO));
            t.linea_verticale(r.x + r.w - 1, r.y, r.y + r.h - 1, static_cast<std::uint8_t>(ORO));
            t.pixel(r.x + 3, r.y + r.h / 2, static_cast<std::uint8_t>(ORO));
            t.pixel(r.x + r.w - 4, r.y + r.h / 2, static_cast<std::uint8_t>(ORO));
            scrivi(t, f, r.x + (r.w - largo(f, k.testo)) / 2, ty, k.testo, NERO + 1);
        } else {   // spento: il vano nel legno
            t.rettangolo_pieno(r.x, r.y, r.w, r.h, static_cast<std::uint8_t>(LEGNO));
            t.linea_orizzontale(r.x, r.x + r.w - 1, r.y + r.h - 1, static_cast<std::uint8_t>(LEGNO + 3));
            scrivi(t, f, r.x + (r.w - largo(f, k.testo)) / 2, ty, k.testo, LEGNO + 4);
        }
    }
}

int tasto_cerimonia(int x, int y, int y_piede, int n, int larghezza_tela, int altezza_tela) {
    if (n <= 0) return -1;
    const int gap = 8, w = (larghezza_tela - 24 - gap * (n - 1)) / n, h = std::min(24, altezza_tela - y_piede - 26);
    if (y < y_piede + 20 || y >= y_piede + 20 + h || x < 12) return -1;
    const int k = (x - 12) / (w + gap);
    if (k >= n || x >= 12 + k * (w + gap) + w) return -1;   // fra due tasti, nello spazio
    return k;
}

// --- il vetrino di Dio ---

Rettangolo pannello_interventi(Tela& t, const Font& f, Rettangolo r, std::string_view titolo, int scelta) {
    const Rettangolo a = quadro::riquadro(t, r, f, titolo);
    const int fondo = r.y + r.h - 15;   // il filo sopra la riga d'aiuto
    t.linea_orizzontale(r.x + 2, r.x + r.w - 3, fondo, fosforo(2));
    const Rettangolo voci{a.x, a.y, a.w, std::max(0, fondo - 1 - a.y)};
    const int h = f.righe() + 3;
    if (scelta >= 0) {
        const int y = voci.y + 2 + scelta * h;
        if (y + h <= voci.y + voci.h) {
            t.rettangolo_pieno(voci.x, y, voci.w, h, fosforo(2));
            t.rettangolo_pieno(voci.x, y, 2, h, 13);   // il magenta del vetrino: solo gli interventi
        }
    }
    return voci;
}

}  // namespace m688
