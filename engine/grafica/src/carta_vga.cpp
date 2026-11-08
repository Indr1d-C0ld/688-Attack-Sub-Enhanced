#include "grafica/carta_vga.hpp"

#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <limits>
#include <string>

#include <toml.hpp>

#include "grafica/font.hpp"

namespace m688 {

namespace {

using namespace colori_carta_vga;

// fasce di profondita' e di quota (le stesse di app/carta.cpp, dossier 12 §5.6); principali a 100, 200, 500, 1000, 2000 m
constexpr double SOGLIE_ACQUA[15] = {10, 20, 30, 50, 75, 100, 150, 200, 300, 400, 500, 750, 1000, 1500, 2000};
constexpr bool PRINCIPALE[15] = {false, false, false, false, false, true, false, true, false, false, true, false, true, false, true};
constexpr double SOGLIE_TERRA[N_TINTE - 1] = {20, 50, 100, 200, 400, 700, 1200};

// la fascia e quanto vi si e' dentro (0-1)
int fascia(double v, const double* soglie, int n, double& dentro) {
    int b = 0;
    while (b < n && v >= soglie[b]) ++b;
    const double lo = b ? soglie[b - 1] : 0.0, hi = b < n ? soglie[b] : lo * 1.5 + 1.0;
    dentro = std::clamp((v - lo) / (hi - lo), 0.0, 1.0);
    return b;
}

}  // namespace

int fascia_acqua_vga(double profondita_m) {
    double dentro = 0.0;
    return fascia(profondita_m, SOGLIE_ACQUA, 15, dentro);
}
int fascia_acqua_vga(double profondita_m, double& dentro) { return fascia(profondita_m, SOGLIE_ACQUA, 15, dentro); }

namespace {

// soglie di Bayer 4x4 in (0, 1)
double bayer(int x, int y) {
    static constexpr int M[4][4] = {{0, 8, 2, 10}, {12, 4, 14, 6}, {3, 11, 1, 9}, {15, 7, 13, 5}};
    return (M[y & 3][x & 3] + 0.5) / 16.0;
}

// Luce dell'ombreggiatura: dal nord-ovest, alta 45 gradi; 1 = terreno piano, sopra la luce, sotto l'ombra. Esagerazione
// verticale: i fondali hanno pendenze di pochi per cento (la scarpata 5-10%), la terra della heightmap qualcosa di piu'
constexpr double ESAGERA_ACQUA = 6.0, ESAGERA_TERRA = 2.0;
double luce(double dzdx, double dzdy_nord, double esagera) {
    const double nx = -dzdx * esagera, ny = -dzdy_nord * esagera, nz = 1.0;
    const double n = std::sqrt(nx * nx + ny * ny + nz * nz);
    constexpr double lx = -0.5, ly = 0.5, lz = 0.70710678;   // dal nord-ovest
    return (nx * lx + ny * ly + nz * lz) / n / lz;
}

struct Rgb {
    double r, g, b;
};

}  // namespace

void applica_palette_rilievo(Palette& p, StileCarta stile) {
    // camoscio, ocra, bruno (le stesse di app/carta.cpp) piu' il bruno scuro delle cime; nella carta nautica piu' chiare
    static constexpr Rgb TERRE[N_TINTE] = {{226, 206, 158}, {212, 184, 124}, {196, 162, 100}, {176, 140, 84},
                                           {156, 120, 72},  {136, 104, 62},  {118, 90, 56},   {100, 78, 52}};
    static constexpr double LUCI[N_LUCI] = {0.62, 0.80, 1.0, 1.14};
    for (int k = 0; k < N_TINTE; ++k)
        for (int l = 0; l < N_LUCI; ++l) {
            Rgb c = TERRE[k];
            if (stile == StileCarta::nautica) c = {c.r + (255 - c.r) * 0.35, c.g + (255 - c.g) * 0.35, c.b + (255 - c.b) * 0.25};
            const auto v = [&](double x) { return static_cast<std::uint8_t>(std::clamp(std::lround(x * LUCI[l]), 0L, 255L)); };
            p.imposta(RILIEVO + k * N_LUCI + l, v(c.r), v(c.g), v(c.b));
        }
}

void testo_con_alone(Tela& t, int x, int y, std::string_view testo, const Font& f, std::uint8_t colore, std::uint8_t alone,
                     int scala) {
    const int sp = f.formato() == 2 ? f.spazio_predefinito() : 1;
    for (int dy = -1; dy <= 1; ++dy)
        for (int dx = -1; dx <= 1; ++dx)
            if (dx || dy) f.scrivi(t, x + dx * scala, y + dy * scala, testo, alone, scala, sp, false);
    f.scrivi(t, x, y, testo, colore, scala, sp, false);
}

// --- mobilia ---

namespace {

void linea_h(Tela& t, int x0, int x1, int y, std::uint8_t c) { t.linea_orizzontale(std::min(x0, x1), std::max(x0, x1), y, c); }

std::string gradi_primi(double v, bool lat) {
    const char e = lat ? (v >= 0 ? 'N' : 'S') : (v >= 0 ? 'E' : 'W');
    const double a = std::fabs(v);
    int gr = static_cast<int>(a);
    int pr = static_cast<int>(std::lround((a - gr) * 60.0));
    if (pr == 60) { ++gr; pr = 0; }
    char s[24];
    std::snprintf(s, sizeof s, "%d°%02d'%c", gr, pr, e);
    return s;
}

int spazio_font(const Font& f) { return f.formato() == 2 ? f.spazio_predefinito() : 1; }

}  // namespace

double declinazione_stima(std::string_view teatro) {
    // STIMA per il 1990 dal modello di campo IGRF/WMM al centro del teatro (valori arrotondati al grado; da verificare
    // col WMM 1990): la declinazione occidentale e' negativa
    static constexpr std::pair<std::string_view, double> T[] = {
        {"baltico", 1.5}, {"barents", 12.0}, {"celtica", -6.0}, {"faroe", -9.0}, {"gibilterra", -4.0}, {"kunashir", -9.0}, {"otranto", 2.0}};
    for (const auto& [n, d] : T)
        if (n == teatro) return d;
    return 0.0;
}

void cornice_minuti(Tela& t, const GeometriaCarta& g, const Fondale& f, const Font& font, int riserva_alto, int riserva_basso) {
    if (!f.georeferenziato()) return;
    constexpr int B = 5;   // spessore della banda
    const auto nero = static_cast<std::uint8_t>(MATITA), bianco = static_cast<std::uint8_t>(MATITA + 5);
    const int x0 = g.x, x1 = g.x + g.w - 1, y0 = g.y + riserva_alto, y1 = g.y + g.h - 1 - riserva_basso;
    if (y1 - y0 < 4 * B) return;
    // il passo in primi per asse: il piu' piccolo con le barrette di almeno 8 pixel. Un primo di latitudine e' un miglio;
    // uno di longitudine un miglio per il coseno della latitudine (a 62 gradi meno della meta')
    const double coslat = std::max(0.1, std::cos(f.geografiche(g.centro).lat * PI / 180.0));
    const auto passo_per = [&](double primi_per_pixel) {
        for (const int p : {1, 2, 5, 10, 15, 30, 60})
            if (p / primi_per_pixel >= 8.0) return p;
        return 60;
    };
    const int passo_lat = passo_per(g.metri_per_pixel / 1852.0), passo_lon = passo_per(g.metri_per_pixel / 1852.0 / coslat);
    // latitudine sui bordi sinistro e destro, longitudine su quelli alto e basso
    for (const bool lat : {true, false}) {
        const int n = lat ? y1 - y0 + 1 : x1 - x0 + 1;
        const int passo = lat ? passo_lat : passo_lon;
        const auto indice = [&](double gradi) { return static_cast<long>(std::floor(gradi * 60.0 / passo)); };
        for (const int lato : {0, 1}) {
            long prima = 0;
            int ultima_etichetta = -1000;   // dove finisce l'ultima etichetta lungo il bordo (pixel)
            for (int k = 0; k < n; ++k) {
                const int x = lat ? (lato ? x1 - B + 1 : x0) : x0 + k, y = lat ? y0 + k : (lato ? y1 - B + 1 : y0);
                const Geografiche geo = f.geografiche(g.a_mondo({(lat ? (lato ? x1 - B / 2 : x0 + B / 2) : x) + 0.5,
                                                                   (lat ? y : (lato ? y1 - B / 2 : y0 + B / 2)) + 0.5}));
                const double v = lat ? geo.lat : geo.lon;
                const long id = indice(v);
                const std::uint8_t c = (id % 2 + 2) % 2 ? bianco : nero;
                if (lat) linea_h(t, x, x + B - 1, y, c);
                else t.linea_verticale(x, y, y + B - 1, c);
                // un'etichetta dove si passa un multiplo di cinque passi, dentro la carta accanto alla banda
                if (k > 0 && id != prima && ((std::max(id, prima)) % 5 == 0)) {
                    const std::string s = gradi_primi(static_cast<double>(std::max(id, prima)) * passo / 60.0, lat);
                    const int w = font.larghezza(s, 1, spazio_font(font), false);
                    const int ingombro = lat ? font.righe() + 2 : w + 6;   // lungo il bordo
                    if (k - ultima_etichetta < ingombro) { prima = id; continue; }
                    ultima_etichetta = k;
                    int tx = 0, ty = 0;
                    if (lat) { tx = lato ? x1 - B - 3 - w : x0 + B + 3; ty = y - font.righe() / 2; }
                    else { tx = x - w / 2; ty = lato ? y1 - B - 2 - font.righe() : y0 + B + 2; }
                    testo_con_alone(t, tx, ty, s, font, nero, bianco);
                    // la tacca lunga verso l'interno
                    if (lat) linea_h(t, lato ? x1 - B - 2 : x0 + B, lato ? x1 - B : x0 + B + 2, y, nero);
                    else t.linea_verticale(x, lato ? y1 - B - 2 : y0 + B, lato ? y1 - B : y0 + B + 2, nero);
                }
                prima = id;
            }
        }
    }
    // il filo nero della cornice dentro e fuori dalla banda, gli angoli pieni
    for (const int d : {0, B}) {
        linea_h(t, x0 + d, x1 - d, y0 + d, nero);
        linea_h(t, x0 + d, x1 - d, y1 - d, nero);
        t.linea_verticale(x0 + d, y0 + d, y1 - d, nero);
        t.linea_verticale(x1 - d, y0 + d, y1 - d, nero);
    }
    for (const int cx : {x0, x1 - B + 1})
        for (const int cy : {y0, y1 - B + 1}) t.rettangolo_pieno(cx, cy, B, B, nero);
}

void rosa_dei_venti(Tela& t, int cx, int cy, int r, double declinazione_gradi, int anno, bool sovietica, const Font& font) {
    const auto mag = static_cast<std::uint8_t>(MAGENTA), bianco = static_cast<std::uint8_t>(MATITA + 5);
    const double xc = cx + 0.5, yc = cy + 0.5;
    const auto punto = [&](double rr, double gradi) { const double a = gradi * PI / 180.0; return std::pair{xc + rr * std::sin(a), yc - rr * std::cos(a)}; };
    const auto segmento = [&](std::pair<double, double> a, std::pair<double, double> b) {
        const int n = static_cast<int>(std::max(std::fabs(b.first - a.first), std::fabs(b.second - a.second))) + 1;
        for (int i = 0; i <= n; ++i) t.pixel(static_cast<int>(a.first + (b.first - a.first) * i / n), static_cast<int>(a.second + (b.second - a.second) * i / n), mag);
    };
    // anello vero: due cerchi, tacche ogni 5 gradi (lunghe ogni 10), i numeri ogni 30
    for (int k = 0; k < 720; ++k) {
        const auto a = punto(r, k * 0.5), b = punto(r - 1, k * 0.5);
        t.pixel(static_cast<int>(a.first), static_cast<int>(a.second), mag);
        t.pixel(static_cast<int>(b.first), static_cast<int>(b.second), mag);
    }
    for (int gr = 0; gr < 360; gr += 5) segmento(punto(r - 1, gr), punto(r - (gr % 10 ? 3 : 5), gr));
    for (int gr = 0; gr < 360; gr += 30) {
        const std::string s = std::to_string(gr);
        const auto [x, y] = punto(r - 11, gr);
        const int w = font.larghezza(s, 1, spazio_font(font), false);
        testo_con_alone(t, static_cast<int>(x) - w / 2, static_cast<int>(y) - font.righe() / 2, s, font, mag, bianco);
    }
    // anello magnetico, ruotato della declinazione: tacche ogni 10 gradi, la freccia del nord magnetico
    const double rm = r * 0.55;
    for (int k = 0; k < 360; ++k) {
        const auto a = punto(rm, k);
        if (k % 2 == 0) t.pixel(static_cast<int>(a.first), static_cast<int>(a.second), mag);
    }
    for (int gr = 0; gr < 360; gr += 10) segmento(punto(rm, gr + declinazione_gradi), punto(rm - (gr % 30 ? 2 : 4), gr + declinazione_gradi));
    segmento(punto(rm - 4, declinazione_gradi), punto(rm + 6, declinazione_gradi));
    // il nord vero: la stella a quattro punte sopra l'anello
    const auto n = punto(r + 7, 0);
    segmento(punto(r + 1, -4), n);
    segmento(punto(r + 1, 4), n);
    // la scritta al centro, su due righe: la declinazione e l'anno
    char s1[24], s2[12];
    const double d = std::fabs(declinazione_gradi);
    if (sovietica) std::snprintf(s1, sizeof s1, "SKL %.0f%s", d, declinazione_gradi >= 0 ? "OST" : "W");
    else std::snprintf(s1, sizeof s1, "VAR %.0f%s", d, declinazione_gradi >= 0 ? "E" : "W");
    std::snprintf(s2, sizeof s2, "%d", anno);
    const int w1 = font.larghezza(s1, 1, spazio_font(font), false), w2 = font.larghezza(s2, 1, spazio_font(font), false);
    testo_con_alone(t, cx - w1 / 2, cy - font.righe(), s1, font, mag, bianco);
    testo_con_alone(t, cx - w2 / 2, cy + 1, s2, font, mag, bianco);
}

int scala_grafica(Tela& t, int x, int y, double metri_per_pixel, bool sovietica, const Font& font) {
    const auto nero = static_cast<std::uint8_t>(MATITA), bianco = static_cast<std::uint8_t>(MATITA + 5);
    // le miglia tonde piu' lunghe che stanno in 140 pixel, in cinque tratti
    double nm = 0.5;
    for (const double n : {0.5, 1.0, 2.0, 5.0, 10.0, 20.0, 50.0, 100.0})
        if (n * 1852.0 / metri_per_pixel <= 140.0) nm = n;
    const int lp = static_cast<int>(std::lround(nm * 1852.0 / metri_per_pixel));
    t.rettangolo_pieno(x - 1, y - 1, lp + 3, 6, nero);
    for (int k = 0; k < 5; ++k) {
        const int a = x + k * lp / 5, b = x + (k + 1) * lp / 5;
        t.rettangolo_pieno(a, y, b - a, 4, k % 2 ? nero : bianco);
    }
    char s[24];
    std::snprintf(s, sizeof s, "%g %s", nm, sovietica ? "MIL" : "NM");
    testo_con_alone(t, x - font.larghezza("0", 1, spazio_font(font), false) / 2, y - font.righe() - 2, "0", font, nero, bianco);
    const int w = font.larghezza(s, 1, spazio_font(font), false);
    testo_con_alone(t, x + lp - w / 2, y - font.righe() - 2, s, font, nero, bianco);
    return lp + 2;
}

Rettangolo cartiglio(Tela& t, int x, int y, const MobiliaCarta& m, const Font& font) {
    const auto nero = static_cast<std::uint8_t>(MATITA), bianco = static_cast<std::uint8_t>(MATITA + 5);
    std::vector<std::string> righe;
    char ed[48];
    if (m.sovietica) {
        righe.push_back("GUNIO MO SSSR");
        if (!m.titolo.empty()) righe.push_back(m.titolo);
        if (m.edizione > 0) { std::snprintf(ed, sizeof ed, "IZDANIE %d", m.edizione); righe.push_back(ed); }
        righe.push_back("GLUBINY V METRAH");
        if (m.zone_non_rilevate) righe.push_back("SHTRIKH.: NE OBSLEDOVANO");
    } else {
        righe.push_back("DMAHTC");
        if (!m.titolo.empty()) righe.push_back(m.titolo);
        if (m.edizione > 0) { std::snprintf(ed, sizeof ed, "EDITION %d", m.edizione); righe.push_back(ed); }
        righe.push_back("SOUNDINGS IN METERS");
        if (m.zone_non_rilevate) righe.push_back("HATCHED: UNSURVEYED");
    }
    int w = 0;
    for (const auto& s : righe) w = std::max(w, font.larghezza(s, 1, spazio_font(font), false));
    const int passo = font.righe() + 1, h = static_cast<int>(righe.size()) * passo + 5;
    const Rettangolo r{x, y - h, w + 10, h};
    t.rettangolo_pieno(r.x, r.y, r.w, r.h, bianco);
    linea_h(t, r.x, r.x + r.w - 1, r.y, nero);
    linea_h(t, r.x, r.x + r.w - 1, r.y + r.h - 1, nero);
    t.linea_verticale(r.x, r.y, r.y + r.h - 1, nero);
    t.linea_verticale(r.x + r.w - 1, r.y, r.y + r.h - 1, nero);
    for (std::size_t i = 0; i < righe.size(); ++i) {
        const int ws = font.larghezza(righe[i], 1, spazio_font(font), false);
        font.scrivi(t, r.x + (r.w - ws) / 2, r.y + 3 + static_cast<int>(i) * passo, righe[i], nero, 1, spazio_font(font), false);
    }
    return r;
}

SpazioMobilia disegna_mobilia(Tela& t, const GeometriaCarta& g, const Fondale& f, const Font& font, const MobiliaCarta& m) {
    const int margine = m.cornice ? 5 + 16 : 6;   // dentro la cornice e le sue etichette
    const int basso = g.y + g.h - m.riserva_basso - margine;
    SpazioMobilia spazio;
    spazio.interno = {g.x, g.y + m.riserva_alto, g.w, g.h - m.riserva_alto - m.riserva_basso};
    if (m.cornice && f.georeferenziato()) {
        // fuori dalla cornice il margine bianco del foglio, come sulle carte nautiche: le fasce di riserva (intestazione e
        // scritte della stazione) non mostrano il fondale
        const auto foglio = static_cast<std::uint8_t>(MATITA + 5);
        if (m.riserva_alto > 0) t.rettangolo_pieno(g.x, g.y, g.w, m.riserva_alto, foglio);
        if (m.riserva_basso > 0) t.rettangolo_pieno(g.x, g.y + g.h - m.riserva_basso, g.w, m.riserva_basso, foglio);
        cornice_minuti(t, g, f, font, m.riserva_alto, m.riserva_basso);
        const int dentro = 5 + font.righe() + 4;   // la banda e le etichette dei minuti
        spazio.interno = {g.x + dentro, g.y + m.riserva_alto + dentro, g.w - 2 * dentro,
                          g.h - m.riserva_alto - m.riserva_basso - 2 * dentro};
    }
    int y_scala = basso - 6;
    if (m.scala) {
        const int w = scala_grafica(t, g.x + margine + 4, y_scala, g.metri_per_pixel, m.sovietica, font);
        spazio.occupati.push_back({g.x + margine, y_scala - font.righe() - 4, w + 10, font.righe() + 12});
        y_scala -= font.righe() + 10;
    }
    if (m.cartiglio) spazio.occupati.push_back(cartiglio(t, g.x + margine, y_scala, m, font));
    if (m.rosa) {
        const int r = std::max(24, std::min(46, g.h / 7));
        const int cx = g.x + g.w - margine - r - 8, cy = basso - r - 6;
        rosa_dei_venti(t, cx, cy, r, m.declinazione_gradi, m.anno_declinazione, m.sovietica, font);
        spazio.occupati.push_back({cx - r - 12, cy - r - 12, 2 * r + 24, 2 * r + 24});
    }
    return spazio;
}

void FondaleVga::disegna(Tela& t, const GeometriaCarta& g, const Fondale& f, const OpzioniCarta& o) {
    ricomposto_ = false;
    // la traslazione dalla vista della cache (pixel della cache = pixel della tela + spostamento)
    const double sx = (g.centro.x - g_.centro.x) / g.metri_per_pixel, sy = -(g.centro.y - g_.centro.y) / g.metri_per_pixel;
    // ricomporre costa 55 ms: non a ogni passo mentre il battello cambia quota (il contorno e' la quota piu' 30 m)
    constexpr double CONTORNO_INDIETRO_M = 10.0, CONTORNO_FERMO_S = 1.0;
    if (o.sicurezza_m != sicurezza_vista_) {
        sicurezza_vista_ = o.sicurezza_m;
        sicurezza_cambiata_s_ = o.tempo_s;
    }
    const double indietro = std::fabs(o.sicurezza_m - o_.sicurezza_m);
    const bool contorno_va = indietro == 0.0 || (o.tempo_s >= 0.0 && indietro < CONTORNO_INDIETRO_M && std::fabs(o.tempo_s - sicurezza_cambiata_s_) < CONTORNO_FERMO_S);
    const bool stessa = valida_ && fondale_ == &f && edizione_ == f.edizione() && g.x == g_.x && g.y == g_.y && g.w == g_.w &&
                        g.h == g_.h && g.metri_per_pixel == g_.metri_per_pixel && o.stile == o_.stile &&
                        contorno_va && o.rilievo == o_.rilievo && o.font_isobate == o_.font_isobate &&
                        o.scandagli == o_.scandagli && o.passo_tratteggio == o_.passo_tratteggio &&
                        std::fabs(sx) < MARGINE - 1 && std::fabs(sy) < MARGINE - 1;
    if (!stessa) ricomponi(g, f, o);
    const int dx = stessa ? static_cast<int>(std::lround(sx)) : 0, dy = stessa ? static_cast<int>(std::lround(sy)) : 0;
    const Rettangolo& rit = t.ritaglio();
    const int x0 = std::max(g.x, rit.x), y0 = std::max(g.y, rit.y);
    const int x1 = std::min(g.x + g.w, rit.x + rit.w), y1 = std::min(g.y + g.h, rit.y + rit.h);
    for (int y = y0; y < y1; ++y) {
        const int j = y - g.y + MARGINE + dy;
        if (j < 0 || j >= ch_) continue;
        std::uint8_t* riga = t.riga(y);
        const std::uint8_t* sorg = pix_.data() + static_cast<std::size_t>(j) * static_cast<std::size_t>(cw_);
        for (int x = x0; x < x1; ++x) {
            const int i = x - g.x + MARGINE + dx;
            if (i >= 0 && i < cw_) riga[x] = sorg[i];
        }
    }
}

void FondaleVga::ricomponi(const GeometriaCarta& g, const Fondale& f, const OpzioniCarta& o) {
    const auto inizio = std::chrono::steady_clock::now();
    g_ = g;
    fondale_ = &f;
    edizione_ = f.edizione();
    o_ = o;
    valida_ = ricomposto_ = true;
    cw_ = g.w + 2 * MARGINE;
    ch_ = g.h + 2 * MARGINE;
    // quote al centro di ogni pixel della cache, con un pixel di bordo per le differenze (positive in su, -inf fuori)
    const int W = cw_ + 2, H = ch_ + 2;
    std::vector<double> z(static_cast<std::size_t>(W) * static_cast<std::size_t>(H));
    const auto Z = [&](int i, int j) -> double& { return z[static_cast<std::size_t>(j + 1) * static_cast<std::size_t>(W) + static_cast<std::size_t>(i + 1)]; };
    const double mpp = g.metri_per_pixel;
    const bool lontano = mpp > 1.2 * f.cella_m();   // piu' celle per pixel: le isole piccole non devono sparire
    for (int j = -1; j <= ch_; ++j)
        for (int i = -1; i <= cw_; ++i) {
            const Vec2 q{g.x - MARGINE + i + 0.5, g.y - MARGINE + j + 0.5};
            const Vec2 m = g.a_mondo(q);
            double v = f.quota(m);
            if (lontano && std::isfinite(v)) {
                // piu' celle per pixel: la quota piu' alta delle celle sotto il pixel (al piu' 4x4 a passo regolare),
                // cosi' un'isola di una cella non sparisce nell'interpolazione col mare attorno
                int c0 = 0, r0 = 0, c1 = 0, r1 = 0;
                if (f.cella({m.x - 0.5 * mpp, m.y + 0.5 * mpp}, c0, r0) && f.cella({m.x + 0.5 * mpp, m.y - 0.5 * mpp}, c1, r1)) {
                    const int pc = std::max(1, (c1 - c0 + 4) / 4), pr = std::max(1, (r1 - r0 + 4) / 4);
                    for (int r = r0; r <= r1; r += pr)
                        for (int c = c0; c <= c1; c += pc) v = std::max(v, -f.profondita_cella(c, r));
                }
            }
            Z(i, j) = v;
        }
    // la profondita' lisciata (media 3x3 dei vicini d'acqua) per fasce e isobate: nei fondali profondi il dato e'
    // rumoroso e al pixel farebbe anelli; la costa e il rilievo restano sul dato vero
    std::vector<double> zl(z.size());
    const auto ZL = [&](int i, int j) -> double& { return zl[static_cast<std::size_t>(j + 1) * static_cast<std::size_t>(W) + static_cast<std::size_t>(i + 1)]; };
    for (int j = -1; j <= ch_; ++j)
        for (int i = -1; i <= cw_; ++i) {
            const double v = Z(i, j);
            if (!std::isfinite(v) || v >= 0.0 || i < 0 || j < 0 || i >= cw_ || j >= ch_) { ZL(i, j) = v; continue; }
            double sm = 0;
            int n = 0;
            for (int b = -1; b <= 1; ++b)
                for (int a = -1; a <= 1; ++a) {
                    const double w = Z(i + a, j + b);
                    if (std::isfinite(w) && w < 0.0) { sm += w; ++n; }
                }
            ZL(i, j) = sm / n;
        }
    // fasce (senza retinatura) per le isobate, terra o acqua
    std::vector<std::int8_t> banda(static_cast<std::size_t>(cw_) * static_cast<std::size_t>(ch_));
    const auto B = [&](int i, int j) -> std::int8_t& { return banda[static_cast<std::size_t>(j) * static_cast<std::size_t>(cw_) + static_cast<std::size_t>(i)]; };
    for (int j = 0; j < ch_; ++j)
        for (int i = 0; i < cw_; ++i) {
            const double v = ZL(i, j);
            double dentro = 0;
            // terra dove la profondita' e' <= 0, come in app/carta (le carte d'epoca portano la terra bassa a quota 0)
            B(i, j) = !std::isfinite(v) ? std::int8_t{100} : v >= 0.0 ? std::int8_t{-1} : static_cast<std::int8_t>(fascia(-v, SOGLIE_ACQUA, 15, dentro));
        }
    pix_.assign(static_cast<std::size_t>(cw_) * static_cast<std::size_t>(ch_), 0);
    const auto P = [&](int i, int j) -> std::uint8_t& { return pix_[static_cast<std::size_t>(j) * static_cast<std::size_t>(cw_) + static_cast<std::size_t>(i)]; };
    const bool epoca = f.carta_epoca();
    for (int j = 0; j < ch_; ++j)
        for (int i = 0; i < cw_; ++i) {
            const double v = Z(i, j);
            const int b = B(i, j);
            const int xg = g.x - MARGINE + i, yg = g.y - MARGINE + j;   // per la retinatura, fissa sulla tela
            if (b == 100) { P(i, j) = static_cast<std::uint8_t>(ACQUA + 15 * 3 + 1); continue; }   // fuori dai dati
            // la luce dal gradiente (le quote fuori dai dati valgono come quelle del pixel)
            const auto zz = [&](int a, int c) { const double w = Z(a, c); return std::isfinite(w) ? w : v; };
            const double dzdx = (zz(i + 1, j) - zz(i - 1, j)) / (2 * mpp), dzdn = (zz(i, j - 1) - zz(i, j + 1)) / (2 * mpp);
            if (b < 0) {   // terra: tinta di quota retinata, luce retinata, costa scura dove tocca l'acqua
                const bool costa = B(std::max(i - 1, 0), j) >= 0 || B(std::min(i + 1, cw_ - 1), j) >= 0 ||
                                   B(i, std::max(j - 1, 0)) >= 0 || B(i, std::min(j + 1, ch_ - 1)) >= 0;
                if (costa) { P(i, j) = static_cast<std::uint8_t>(COSTA); continue; }
                double dentro = 0;
                int k = fascia(v, SOGLIE_TERRA, N_TINTE - 1, dentro);
                if (k < N_TINTE - 1 && dentro > bayer(xg, yg) + 0.25) ++k;
                const double s = o.rilievo ? luce(dzdx, dzdn, ESAGERA_TERRA) : 1.0;
                // il piano esattamente sulla luce media (2), la retinatura verso i vicini
                const int l = std::clamp(static_cast<int>(std::floor(2.0 + (s - 1.0) / 0.22 + bayer(xg + 1, yg + 2))), 0, N_LUCI - 1);
                P(i, j) = static_cast<std::uint8_t>(RILIEVO + k * N_LUCI + l);
                continue;
            }
            // acqua: fascia retinata verso la successiva (sulla profondita' lisciata), ombra dalla luce
            double dentro = 0;
            fascia(-ZL(i, j), SOGLIE_ACQUA, 15, dentro);
            const int bb = b < 15 && dentro > bayer(xg, yg) + 0.35 ? b + 1 : b;   // solo verso la fine della fascia
            const double s = o.rilievo ? luce(dzdx, dzdn, ESAGERA_ACQUA) : 1.0;
            const int ombra = std::clamp(static_cast<int>(std::floor((s - 0.8) / 0.2 + bayer(xg + 2, yg + 1))), 0, 2);
            std::uint8_t c = static_cast<std::uint8_t>(ACQUA + bb * 3 + ombra);
            // la fascia chiara lungo la costa (la risacca, il bassofondo)
            bool riva = false;
            for (int d = 1; d <= 2 && !riva; ++d)
                riva = B(std::max(i - d, 0), j) < 0 || B(std::min(i + d, cw_ - 1), j) < 0 || B(i, std::max(j - d, 0)) < 0 ||
                       B(i, std::min(j + d, ch_ - 1)) < 0;
            if (riva) c = static_cast<std::uint8_t>(ACQUA + 2);
            // carta d'epoca: le zone non rilevate col tratteggio diagonale, tre fasce piu' chiaro dell'acqua sotto, non del
            // colore delle isobate: non si confonde con loro
            if (epoca && (xg + yg) % std::max(2, o.passo_tratteggio) == 0 && !f.rilevato(g.a_mondo({xg + 0.5, yg + 0.5})))
                c = static_cast<std::uint8_t>(ACQUA + std::max(bb - 3, 0) * 3 + ombra);
            P(i, j) = c;
        }
    // isobate: dove la fascia cambia fra due pixel d'acqua; le principali su due pixel. Contorno di sicurezza sopra
    std::vector<std::int8_t> soglia_principale(pix_.size(), -1);
    for (int j = 0; j + 1 < ch_; ++j)
        for (int i = 0; i + 1 < cw_; ++i) {
            const int b = B(i, j);
            if (b < 0 || b == 100) continue;
            for (const auto& [a, c] : {std::pair{i + 1, j}, std::pair{i, j + 1}}) {
                const int bq = B(a, c);
                if (bq < 0 || bq == 100 || bq == b) continue;
                const int confine = std::min(b, bq);
                if (PRINCIPALE[confine]) {   // le principali piene, le altre puntinate (come le carte nautiche)
                    P(i, j) = static_cast<std::uint8_t>(ISOBATA_PRINCIPALE);
                    soglia_principale[static_cast<std::size_t>(j) * static_cast<std::size_t>(cw_) + static_cast<std::size_t>(i)] = static_cast<std::int8_t>(confine);
                } else if (P(i, j) != ISOBATA_PRINCIPALE && (g.x + i + g.y + j) % 2 == 0) {
                    P(i, j) = static_cast<std::uint8_t>(ISOBATA);
                }
            }
            if (o.sicurezza_m > 0.0) {
                const double p0 = -Z(i, j), p1 = -Z(i + 1, j), p2 = -Z(i, j + 1);
                if ((std::isfinite(p1) && (p0 < o.sicurezza_m) != (p1 < o.sicurezza_m)) || (std::isfinite(p2) && (p0 < o.sicurezza_m) != (p2 < o.sicurezza_m)))
                    P(i, j) = static_cast<std::uint8_t>(SICUREZZA);
            }
        }
    // i numeri delle isobate principali: uno per riquadro di 120 pixel, il pixel piu' vicino al centro del riquadro
    std::vector<std::pair<int, int>> etichette;   // dove stanno i numeri delle isobate (gli scandagli li evitano)
    if (o.font_isobate) {
        Tela c(cw_, ch_);
        for (int j = 0; j < ch_; ++j)
            std::copy_n(pix_.data() + static_cast<std::size_t>(j) * static_cast<std::size_t>(cw_), cw_, c.riga(j));
        constexpr int PASSO = 120;
        for (int rj = 0; rj < ch_; rj += PASSO)
            for (int ri = 0; ri < cw_; ri += PASSO) {
                int migliore = -1;
                double dmin = 1e18;
                for (int j = rj; j < std::min(rj + PASSO, ch_); ++j)
                    for (int i = ri; i < std::min(ri + PASSO, cw_); ++i) {
                        const std::size_t k = static_cast<std::size_t>(j) * static_cast<std::size_t>(cw_) + static_cast<std::size_t>(i);
                        if (soglia_principale[k] < 0) continue;
                        const double d = std::hypot(i - (ri + PASSO / 2.0), j - (rj + PASSO / 2.0));
                        if (d < dmin) { dmin = d; migliore = static_cast<int>(k); }
                    }
                if (migliore < 0) continue;
                const int i = migliore % cw_, j = migliore / cw_;
                const std::string s = std::to_string(static_cast<int>(SOGLIE_ACQUA[soglia_principale[static_cast<std::size_t>(migliore)]]));
                const int w = o.font_isobate->larghezza(s, 1, o.font_isobate->formato() == 2 ? o.font_isobate->spazio_predefinito() : 1, false);
                testo_con_alone(c, i - w / 2, j - o.font_isobate->righe() / 2, s, *o.font_isobate, static_cast<std::uint8_t>(ISOBATA_PRINCIPALE),
                                static_cast<std::uint8_t>(ACQUA + 15 * 3 + 1));
                etichette.push_back({i, j});
            }
        // scandagli: uno per riquadro di 44 pixel, in un punto del riquadro scelto da un hash fisso sul mondo (non
        // ballano quando la cache si rifa'), solo in acqua, lontani dalla costa e dai numeri delle isobate
        if (o.scandagli) {
            constexpr int PASSO_S = 44;
            const int sp = o.font_isobate->formato() == 2 ? o.font_isobate->spazio_predefinito() : 1;
            const auto mescola = [](std::uint64_t x) {
                x ^= x >> 33; x *= 0xff51afd7ed558ccdull; x ^= x >> 33; x *= 0xc4ceb9fe1a85ec53ull; x ^= x >> 33;
                return x;
            };
            for (int rj = 6; rj + 6 < ch_; rj += PASSO_S)
                for (int ri = 6; ri + 6 < cw_; ri += PASSO_S) {
                    // il riquadro nel mondo: la griglia dei riquadri e' fissa sulla tela, l'hash sul centro nel mondo
                    const Vec2 m = g.a_mondo({g.x - MARGINE + ri + 0.5, g.y - MARGINE + rj + 0.5});
                    const std::uint64_t h = mescola(static_cast<std::uint64_t>(std::llround(m.x / 997.0)) * 0x9e3779b97f4a7c15ull ^
                                                    static_cast<std::uint64_t>(std::llround(m.y / 991.0)));
                    const int i = ri + static_cast<int>(h % 30), j = rj + static_cast<int>((h >> 16) % 30);
                    if (i >= cw_ - 4 || j >= ch_ - 4) continue;
                    const double v = Z(i, j);
                    if (!std::isfinite(v) || v >= 0.0) continue;
                    bool vicino = false;   // costa entro 6 pixel, o fuori dai dati
                    for (int d = -6; d <= 6 && !vicino; d += 3)
                        for (int e = -6; e <= 6 && !vicino; e += 3) {
                            const int a = std::clamp(i + d, 0, cw_ - 1), b2 = std::clamp(j + e, 0, ch_ - 1);
                            vicino = B(a, b2) < 0 || B(a, b2) == 100;
                        }
                    for (const auto& [ei, ej] : etichette) vicino |= std::abs(ei - i) < 26 && std::abs(ej - j) < 12;
                    // e dalle note della carta d'epoca (ED, PA...), che stanno a destra del loro punto
                    if (epoca)
                        for (const auto& a : f.annotazioni()) {
                            const Vec2 q = g.a_tela(a.pos);
                            const int ai = static_cast<int>(q.x) - g.x + MARGINE, aj = static_cast<int>(q.y) - g.y + MARGINE;
                            vicino |= i > ai - 16 && i < ai + 76 && std::abs(aj - j) < 10;
                        }
                    if (vicino) continue;
                    if (epoca && !f.rilevato(g.a_mondo({g.x - MARGINE + i + 0.5, g.y - MARGINE + j + 0.5}))) continue;
                    const std::string s = std::to_string(static_cast<int>(std::lround(-v)));
                    const int w = o.font_isobate->larghezza(s, 1, sp, false);
                    testo_con_alone(c, i - w / 2, j - o.font_isobate->righe() / 2, s, *o.font_isobate, static_cast<std::uint8_t>(ISOBATA),
                                    static_cast<std::uint8_t>(ACQUA + 15 * 3 + 1));
                }
        }
        std::copy(c.dati(), c.dati() + pix_.size(), pix_.begin());
    }
    ms_ = std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - inizio).count();
}

// --- toponimi ---

std::string file_toponimi(const std::string& cartella_dati, std::string_view teatro) {
    return cartella_dati + "/teatri/" + std::string(teatro) + ".toponimi.toml";
}

std::vector<Toponimo> carica_toponimi(const std::string& percorso) {
    std::vector<Toponimo> out;
    toml::table tab;
    try {
        tab = toml::parse_file(percorso);
    } catch (const toml::parse_error&) {
        return out;   // manca o non si legge: la carta resta senza nomi
    }
    if (const auto* nomi = tab["nome"].as_array())
        for (const auto& n : *nomi) {
            const auto* v = n.as_table();
            if (!v) continue;
            Toponimo k;
            k.testo = (*v)["testo"].value_or(std::string{});
            k.en = (*v)["en"].value_or(k.testo);
            k.tipo = (*v)["tipo"].value_or(std::string{});
            k.acqua = (*v)["acqua"].value_or(false);
            k.rango = static_cast<int>((*v)["rango"].value_or(5));
            k.pos = {(*v)["x"].value_or(0.0), (*v)["y"].value_or(0.0)};
            if (!k.testo.empty()) out.push_back(std::move(k));
        }
    return out;
}

int rango_massimo_toponimi(double metri_per_pixel) {
    // tutto il teatro (500-700 m per pixel): i mari, gli stretti, le isole maggiori e le basi; poi un rango in piu' a
    // ogni ingrandimento, fino a tutti i nomi sotto i 25 m per pixel
    static constexpr std::pair<double, int> SOGLIE[] = {{400.0, 4}, {200.0, 5}, {100.0, 6}, {50.0, 7}, {25.0, 8}};
    for (const auto& [mpp, r] : SOGLIE)
        if (metri_per_pixel >= mpp) return r;
    return 10;
}

namespace {

bool si_toccano(const Rettangolo& a, const Rettangolo& b) {
    return a.x < b.x + b.w && b.x < a.x + a.w && a.y < b.y + b.h && b.y < a.y + a.h;
}

bool contiene(const Rettangolo& fuori, const Rettangolo& r) {
    return r.x >= fuori.x && r.y >= fuori.y && r.x + r.w <= fuori.x + fuori.w && r.y + r.h <= fuori.y + fuori.h;
}

// La scritta con l'alone, diritta o inclinata (le righe alte spostate a destra di un pixel ogni tre, come il corsivo
// delle acque sulle carte nautiche)
void scritta(Tela& t, int x, int y, const std::string& s, const Font& f, int spazio, bool inclinata, std::uint8_t colore,
             std::uint8_t alone) {
    if (!inclinata) {
        for (int dy = -1; dy <= 1; ++dy)
            for (int dx = -1; dx <= 1; ++dx)
                if (dx || dy) f.scrivi(t, x + dx, y + dy, s, alone, 1, spazio, false);
        f.scrivi(t, x, y, s, colore, 1, spazio, false);
        return;
    }
    const int w = f.larghezza(s, 1, spazio, false) + 2, h = f.righe() + 2, pend = (h - 1) / 3;
    Tela tmp(w + pend + 1, h);
    tmp.riempi(0);
    for (int dy = -1; dy <= 1; ++dy)
        for (int dx = -1; dx <= 1; ++dx)
            if (dx || dy) f.scrivi(tmp, 1 + dx, 1 + dy, s, 2, 1, spazio, false);
    f.scrivi(tmp, 1, 1, s, 1, 1, spazio, false);
    const Rettangolo& rit = t.ritaglio();
    for (int r = 0; r < h; ++r) {
        const int sp = (h - 1 - r) / 3, ty = y - 1 + r;
        if (ty < rit.y || ty >= rit.y + rit.h) continue;
        for (int c = 0; c < tmp.larghezza(); ++c) {
            const std::uint8_t v = tmp.at(c, r);
            const int tx = x - 1 + c + sp;
            if (v && tx >= rit.x && tx < rit.x + rit.w) t.pixel(tx, ty, v == 1 ? colore : alone);
        }
    }
}

}  // namespace

int disegna_toponimi(Tela& t, const GeometriaCarta& g, const std::vector<Toponimo>& nomi, const Font& font,
                     const OpzioniToponimi& o) {
    const auto nero = static_cast<std::uint8_t>(MATITA), bianco = static_cast<std::uint8_t>(MATITA + 5);
    const auto blu = static_cast<std::uint8_t>(MATITA + 2);
    const Rettangolo area = o.area.w > 0 ? o.area : Rettangolo{g.x, g.y, g.w, g.h};
    const int massimo = rango_massimo_toponimi(g.metri_per_pixel);
    std::vector<const Toponimo*> ordine;
    for (const auto& n : nomi)
        if (n.rango <= massimo) ordine.push_back(&n);
    std::stable_sort(ordine.begin(), ordine.end(), [](const Toponimo* a, const Toponimo* b) { return a->rango < b->rango; });
    std::vector<Rettangolo> presi = o.occupati;
    const int sp = spazio_font(font), h = font.righe() + 2;
    int scritti = 0;
    for (const Toponimo* n : ordine) {
        const std::string& s = o.inglese ? n->en : n->testo;   // le lettere che il font non ha: Font::caratteri
        if (s.empty()) continue;
        const bool mare = n->acqua && n->rango <= 2 && (n->tipo == "mare" || n->tipo == "oceano");
        const int spazio = mare ? sp + 3 : sp;   // i mari maggiori in maiuscolo spaziato
        const int w = font.larghezza(s, 1, spazio, false) + (n->acqua ? h / 3 : 0) + 2;
        const Vec2 q = g.a_tela(n->pos);
        const int px = static_cast<int>(std::lround(q.x)), py = static_cast<int>(std::lround(q.y));
        const bool punto = n->tipo == "citta" || n->tipo == "base";
        if (punto && (px < area.x + 2 || px >= area.x + area.w - 2 || py < area.y + 2 || py >= area.y + area.h - 2)) continue;
        // le posizioni da provare: centrata per le aree, a destra, a sinistra, sopra e sotto il segno per i punti
        std::vector<Rettangolo> prove;
        if (punto) {
            prove = {{px + 5, py - h / 2, w, h}, {px - 5 - w, py - h / 2, w, h}, {px - w / 2, py - 4 - h, w, h},
                     {px - w / 2, py + 5, w, h}};
        } else {
            prove = {{px - w / 2, py - h / 2, w, h}};
        }
        const Rettangolo segno{px - 3, py - 3, 7, 7};
        for (const Rettangolo& r : prove) {
            if (!contiene(area, r)) continue;
            bool libero = !punto || std::none_of(presi.begin(), presi.end(), [&](const Rettangolo& p) { return si_toccano(p, segno); });
            for (const Rettangolo& p : presi)
                if (libero && si_toccano(p, {r.x - 2, r.y - 1, r.w + 4, r.h + 2})) libero = false;
            if (!libero) continue;
            if (punto) {
                if (n->tipo == "base") {   // la base: un quadrato vuoto
                    t.rettangolo_pieno(px - 3, py - 3, 7, 7, bianco);
                    t.rettangolo_pieno(px - 2, py - 2, 5, 5, nero);
                    t.rettangolo_pieno(px - 1, py - 1, 3, 3, bianco);
                } else {                   // la citta': un punto
                    t.rettangolo_pieno(px - 2, py - 2, 5, 5, bianco);
                    t.rettangolo_pieno(px - 1, py - 1, 3, 3, nero);
                }
                presi.push_back(segno);
            }
            scritta(t, r.x + 1, r.y + 1, s, font, spazio, n->acqua, n->acqua ? blu : nero, bianco);
            presi.push_back(r);
            ++scritti;
            break;
        }
    }
    return scritti;
}

}  // namespace m688
