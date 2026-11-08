// Bozze del sonar per la rifinitura estetica (roadmap 33; dossier docs/ricerca/25_estetica_simulatori.md §9 e nota
// condivisa sonar-confronto-27-10.md): la scala lunga con il dithering e il fosforo del 1989 (grafica/scala_cascata),
// sulla disposizione del dossier 09 §4.3 (BTR a sinistra; A-scan, LOFAR e DEMON a destra; contatti in basso).
// I dati sono sintetici ma con la forma di quelli veri: pavimento che cala con la frequenza, righe tonali a 50 Hz e
// armoniche, un contatto che deriva sul BTR, un mercantile, il cono cieco di poppa, un transitorio.
// Scrive BMP a 640x480: livelli_cascata (oggi, 128 livelli, 128 con Bayer), sonar_moderno, sonar_1989 e
// fosforo_dettaglio (la persistenza dell'A-scan e il bagliore, ingranditi).
// Con la cartella delle righe vere del disassemblatore (tools/re/prove/cascate_esempio: un 688 con la TB-16 e un
// mercantile al CPA, senza e con le frange d'interferenza) scrive anche frange_vere e frange_fosforo.
// Uso: bozze_sonar <dati> <cartella d'uscita> [cartella delle righe vere]
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <fstream>
#include <random>
#include <string>
#include <vector>

#include "grafica/consolle.hpp"
#include "grafica/font.hpp"
#include "grafica/palette.hpp"
#include "grafica/scala_cascata.hpp"
#include "grafica/tela.hpp"
#include "grafica/tema_stazioni.hpp"

using namespace m688;
namespace cs = m688::cascate;

namespace {

constexpr std::uint8_t NERO = 0, BLU = 1, CIANO = 3, GRIGIO = 7, GRIGIO_SCURO = 8, VERDE_CHIARO = 10, CIANO_CHIARO = 11,
                       ROSA = 13, GIALLO = 14, BIANCO = 15;
constexpr double PAV = 60.0, TETTO = 86.0;   // dB della scala

struct Fonti {
    Font orig, fos7, fos5;
};
int largo(const Font& f, const std::string& s) { return f.larghezza(s, 1, f.formato() == 2 ? f.spazio_predefinito() : 1, f.formato() != 2); }
void scrivi(Tela& t, const Font& f, int x, int y, const std::string& s, std::uint8_t c) {
    f.scrivi(t, x, y, s, c, 1, f.formato() == 2 ? f.spazio_predefinito() : 1, f.formato() != 2);
}

// --- i dati: una matrice di dB, la riga 0 e' la piu' recente ---
struct Gram {
    int w, h;
    std::vector<double> db;
    double at(int x, int y) const { return db[static_cast<std::size_t>(y) * w + x]; }
};

double gauss(double d, double s) { return std::exp(-0.5 * d * d / (s * s)); }
// somma in potenza di un livello sopra il pavimento
double somma_db(double a, double b) { return 10.0 * std::log10(std::pow(10.0, a / 10.0) + std::pow(10.0, b / 10.0)); }

Gram btr(int w, int h) {   // rilevamento (0-360 su w pixel) per tempo (1 riga = 1 s)
    Gram g{w, h, std::vector<double>(static_cast<std::size_t>(w) * h)};
    std::mt19937 r(7);
    std::normal_distribution<double> n(0.0, 1.6);
    for (int y = 0; y < h; ++y)
        for (int x = 0; x < w; ++x) {
            const double b = 360.0 * (x + 0.5) / w;
            double v = 64.0 + n(r);
            const auto contatto = [&](double centro, double livello, double largo_g) {
                double d = std::fmod(std::abs(b - centro), 360.0);
                d = std::min(d, 360.0 - d);
                v = somma_db(v, livello + 10.0 * std::log10(gauss(d, largo_g) + 1e-9));
            };
            contatto(100.0 + 0.045 * y, 74.0, 1.3);    // S1: il sottomarino che deriva
            contatto(232.0 - 0.012 * y, 67.5, 1.6);    // S2: debole, ai limiti
            contatto(300.0 + 0.020 * y, 79.0, 2.6);    // il mercantile
            if (y >= 58 && y <= 61) contatto(250.0, 84.0, 4.0);   // un transitorio: il portello di un tubo
            if (b > 165.0 && b < 195.0) v -= 3.5;      // il cono cieco di poppa: piu' sordo, non vuoto
            g.db[static_cast<std::size_t>(y) * w + x] = v;
        }
    return g;
}

Gram lofar(int w, int h, double f0, double f1) {   // frequenza per tempo
    Gram g{w, h, std::vector<double>(static_cast<std::size_t>(w) * h)};
    std::mt19937 r(11);
    std::normal_distribution<double> n(0.0, 1.5);
    for (int y = 0; y < h; ++y)
        for (int x = 0; x < w; ++x) {
            const double f = f0 + (f1 - f0) * (x + 0.5) / w, hz_px = (f1 - f0) / w;
            // il pavimento cala con la frequenza (il DEMON, inviluppo demodulato, e' piatto)
            double v = (f1 < 100.0 ? 66.0 : 71.0 - 7.0 * std::log10(f / f0)) + n(r);
            const auto riga = [&](double fr, double livello) {
                v = somma_db(v, livello + 10.0 * std::log10(gauss(f - fr, 0.7 * hz_px) + 1e-9));
            };
            if (f1 < 100.0) {   // DEMON: albero e pale (7) con le armoniche
                riga(3.1, 73.0), riga(6.2, 69.0), riga(21.7, 72.0), riga(43.4, 67.0);
            } else {
                riga(50.0, 82.0), riga(100.0, 77.0), riga(150.0, 72.0), riga(200.0, 69.0);
                riga(37.5 + 0.6 * std::sin(y / 9.0), 74.0);          // una pompa che varia
                if (y > 18 && y < 70) riga(275.0, 71.0);              // un'ausiliaria che si accende e si spegne
            }
            g.db[static_cast<std::size_t>(y) * w + x] = v;
        }
    return g;
}

// La riga dell'A-scan: la media delle prime n righe del LOFAR
std::vector<double> riga_ascan(const Gram& g, int n, int dalla = 0) {
    std::vector<double> v(static_cast<std::size_t>(g.w), 0.0);
    for (int y = dalla; y < dalla + n; ++y)
        for (int x = 0; x < g.w; ++x) v[static_cast<std::size_t>(x)] += g.at(x, y) / n;
    return v;
}

// --- disegno ---
enum class Resa { oggi, lunga, bayer };

void disegna_gram(Tela& t, const Gram& g, int x, int y, Resa resa) {
    for (int j = 0; j < g.h; ++j)
        for (int i = 0; i < g.w; ++i) {
            const double u = cs::frazione(g.at(i, j), PAV, TETTO);
            std::uint8_t c;
            if (resa == Resa::oggi) {   // 32 gradini, come la stazione oggi (rifatti sulla scala lunga)
                const double q = std::round(u * 31.0) / 31.0;
                c = cs::indice(q, x + i, y + j, cs::SCALA_LUNGA, false);
            } else {
                c = cs::indice(u, x + i, y + j, cs::SCALA_LUNGA, resa == Resa::bayer);
            }
            t.pixel(x + i, y + j, c);
        }
}

void barra_scala(Tela& t, Rettangolo r, Resa resa) {
    for (int i = 0; i < r.w; ++i)
        for (int j = 0; j < r.h; ++j) {
            const double u = static_cast<double>(i) / (r.w - 1);
            const std::uint8_t c = resa == Resa::oggi ? cs::indice(std::round(u * 31.0) / 31.0, 0, 0, cs::SCALA_LUNGA, false)
                                                      : cs::indice(u, r.x + i, r.y + j, cs::SCALA_LUNGA, resa == Resa::bayer);
            t.pixel(r.x + i, r.y + j, c);
        }
}

void cornice(Tela& t, Rettangolo r, std::uint8_t c) {
    t.linea_orizzontale(r.x - 1, r.x + r.w, r.y - 1, c);
    t.linea_orizzontale(r.x - 1, r.x + r.w, r.y + r.h, c);
    t.linea_verticale(r.x - 1, r.y - 1, r.y + r.h, c);
    t.linea_verticale(r.x + r.w, r.y - 1, r.y + r.h, c);
}

// Il segno del tracker sul bordo del BTR, con le convenzioni dei simboli della carta: ottagono giallo l'ignoto,
// rombo rosa l'ostile (qui con gli indici fissi 0-15, perche' la scala lunga copre quelli della carta)
void segno_tracker(Tela& t, const Font& f, int x, int y, const std::string& nome, bool ostile) {
    const std::uint8_t c = ostile ? ROSA : GIALLO;
    if (ostile) {
        for (int k = 0; k <= 3; ++k) {
            t.pixel(x - k, y - 3 + k, c), t.pixel(x + k, y - 3 + k, c);
            t.pixel(x - k, y + 3 - k, c), t.pixel(x + k, y + 3 - k, c);
        }
    } else {
        t.linea_orizzontale(x - 1, x + 1, y - 3, c), t.linea_orizzontale(x - 1, x + 1, y + 3, c);
        t.linea_verticale(x - 3, y - 1, y + 1, c), t.linea_verticale(x + 3, y - 1, y + 1, c);
        t.pixel(x - 2, y - 2, c), t.pixel(x + 2, y - 2, c), t.pixel(x - 2, y + 2, c), t.pixel(x + 2, y + 2, c);
    }
    scrivi(t, f, x + 6, y - 3, nome, c);
}

// La tavola 1: lo stesso LOFAR e lo stesso BTR in tre rese
void livelli_cascata(Tela& t, const Fonti& f) {
    t.riempi(NERO);
    scrivi(t, f.orig, 12, 8, "LE CASCATE: 32 LIVELLI, 128 LIVELLI, 128 CON IL DITHERING BAYER 4X4 (VIRIDIS)", CIANO_CHIARO);
    const Gram l = lofar(196, 104, 10.0, 270.0), b = btr(196, 104);
    const char* titoli[] = {"OGGI: 32 LIVELLI", "128 LIVELLI", "128 + BAYER 4X4"};
    const Resa rese[] = {Resa::oggi, Resa::lunga, Resa::bayer};
    for (int k = 0; k < 3; ++k) {
        const int x = 14 + k * 208;
        scrivi(t, f.orig, x, 30, titoli[k], k == 2 ? GIALLO : GRIGIO);
        disegna_gram(t, l, x, 46, rese[k]);
        cornice(t, {x, 46, l.w, l.h}, GRIGIO_SCURO);
        disegna_gram(t, b, x, 172, rese[k]);
        cornice(t, {x, 172, b.w, b.h}, GRIGIO_SCURO);
        // l'ingrandimento 4x del fianco del mercantile, dove la rampa e' liscia: i gradini si vedono qui
        Tela z(196, 104);
        disegna_gram(z, b, 0, 0, rese[k]);
        const int zx = static_cast<int>(296.0 / 360 * 196) - 30, zy = 40;
        for (int j = 0; j < 22; ++j)
            for (int i = 0; i < 48; ++i) t.rettangolo_pieno(x + 2 + i * 4, 296 + j * 4, 4, 4, z.at(zx + i, zy + j));
        cornice(t, {x + 2, 296, 192, 88}, GRIGIO_SCURO);
        barra_scala(t, {x, 404, 196, 12}, rese[k]);
        cornice(t, {x, 404, 196, 12}, GRIGIO_SCURO);
    }
    scrivi(t, f.orig, 14, 154, "LOFAR 10-270 HZ: 50 HZ E ARMONICHE, LA POMPA A 37,5", GRIGIO);
    scrivi(t, f.orig, 14, 280, "BTR: S1 CHE DERIVA, S2 DEBOLE, IL MERCANTILE, IL TRANSITORIO; SOTTO, IL MERCANTILE X4", GRIGIO);
    scrivi(t, f.orig, 14, 392, "LA STESSA RAMPA DAL PAVIMENTO AL TETTO:", GRIGIO);
    scrivi(t, f.orig, 14, 428, "A 32 GRADINI LE FASCE SI CONTANO. PALETTE: 128 INDICI (96-223) QUANDO LA STAZIONE", GRIGIO);
    scrivi(t, f.orig, 14, 442, "COPRE LA VISTA, COME LA CARTA. IL DITHERING E' FERMO: LA GRANA NON TREMOLA.", GRIGIO);
}

// Il guscio della stazione come oggi (lampade in alto, pannello comandi in basso), per mettere le bozze in contesto
void guscio(Tela& t, const Fonti& f, bool anni89) {
    t.riempi(NERO);
    t.rettangolo_pieno(0, 0, 640, 12, smalto(2));
    scrivi(t, f.orig, 10, 2, "AN/BQQ-5 SONAR", smalto(8));
    for (int k = 0; k < 10; ++k) t.rettangolo_pieno(152 + k * 30, 4, 20, 6, k == 9 ? 12 : (k % 3 == 0 ? VERDE_CHIARO : 2));
    t.rettangolo_pieno(0, 12, 640, 346, smalto(1));
    t.rettangolo_pieno(4, 16, 632, 338, NERO);
    t.rettangolo_pieno(0, 358, 640, 122, smalto(4));
    scrivi(t, f.orig, 12, 364, anni89 ? "SONAR  TORPEX '89  1.5X" : "SONAR  TORPEX '89  1.5X", CIANO_CHIARO);
    scrivi(t, f.orig, 490, 364, "ORA 10:03:20Z  X1", CIANO_CHIARO);
    const char* p[] = {"1 PASSA BANDA", "2 IMPULSO ATTIVO", "3 FILA", "4 ANALIZZA"};
    for (int k = 0; k < 4; ++k) {
        t.rettangolo_pieno(12 + k * 158, 380, 150, 20, NERO);
        const int w = largo(f.orig, p[k]);
        scrivi(t, f.orig, 12 + k * 158 + (150 - w) / 2, 386, p[k], BIANCO);
    }
    t.rettangolo_pieno(8, 408, 624, 26, NERO);
    scrivi(t, f.orig, 14, 416, "S1 SU 108: SOTTOMARINO PROBABILE, 50 HZ. S2 SU 230: DA CLASSIFICARE", CIANO_CHIARO);
    scrivi(t, f.orig, 12, 446, "4 ANALIZZA 6 FASCIO 7 INT 8 AUTO 9 CURS TAB [ ] B LIBRERIA W J NOTE S Y C SLOT T K", GRIGIO);
    scrivi(t, f.orig, 12, 460, "COMANDANTE, SONAR: SIERRA 1 CLASSIFICATO POSSIBILE SOTTOMARINO.", VERDE_CHIARO);
}

// Le scale (rilevamenti, frequenze) e i titoli dei riquadri
void scala_orizzontale(Tela& t, const Font& f, int x, int y, int w, double v0, double v1, double passo, std::uint8_t c,
                       const char* unita) {
    for (double v = std::ceil(v0 / passo) * passo; v <= v1 + 1e-9; v += passo) {
        const int px = x + static_cast<int>(std::lround((v - v0) / (v1 - v0) * (w - 1)));
        t.linea_verticale(px, y, y + 3, c);
        char s[16];
        std::snprintf(s, sizeof s, "%g", v);
        if (px + 2 + largo(f, s) <= x + w) scrivi(t, f, px + 2, y - 1, s, c);   // l'etichetta resta nel riquadro
    }
    scrivi(t, f, x + w - largo(f, unita), y + 6, unita, c);
}

// La disposizione del 09 §4.3 nella vista: BTR a sinistra, A-scan, LOFAR e DEMON a destra, contatti in basso
struct Disposizione {
    Rettangolo btr{10, 40, 300, 224}, ascan{330, 40, 298, 52}, lofar{330, 108, 298, 120}, demon{330, 256, 298, 34},
        contatti{10, 286, 300, 64}, stato{330, 298, 298, 52};
};

void sonar_moderno(Tela& t, const Fonti& f) {
    guscio(t, f, false);
    const Disposizione d;
    const Gram b = btr(d.btr.w, d.btr.h), l = lofar(d.lofar.w, d.lofar.h, 10.0, 410.0), m = lofar(d.demon.w, d.demon.h, 0.0, 40.0);
    // BTR
    scrivi(t, f.orig, d.btr.x, 20, "BTR  SFERA  INT 4", CIANO_CHIARO);
    disegna_gram(t, b, d.btr.x, d.btr.y, Resa::bayer);
    scala_orizzontale(t, f.fos5, d.btr.x, d.btr.y - 9, d.btr.w, 0, 315, 45, CIANO, "");
    for (int k = 1; k <= 3; ++k) {   // i minuti: tacche fuori dal BTR, non righe sopra i dati
        const int y = d.btr.y + k * 60;
        t.linea_orizzontale(d.btr.x + d.btr.w + 1, d.btr.x + d.btr.w + 4, y, GRIGIO);
        scrivi(t, f.fos5, d.btr.x + d.btr.w + 6, y - 3, std::to_string(k) + "'", GRIGIO);
    }
    segno_tracker(t, f.fos5, d.btr.x + static_cast<int>(100.0 / 360 * d.btr.w), d.btr.y + 6, "S1", true);
    segno_tracker(t, f.fos5, d.btr.x + static_cast<int>(232.0 / 360 * d.btr.w), d.btr.y + 6, "S2", false);
    // A-scan: la riga del LOFAR, senza scalini (le colonne vicine unite da un tratto verticale)
    scrivi(t, f.orig, d.ascan.x, 20, "A-SCAN E LOFAR  108 VERO (S1)", CIANO_CHIARO);
    t.rettangolo_pieno(d.ascan.x, d.ascan.y, d.ascan.w, d.ascan.h, NERO);
    for (int k = 1; k < 4; ++k)
        for (int x = d.ascan.x; x < d.ascan.x + d.ascan.w; x += 3) t.pixel(x, d.ascan.y + k * d.ascan.h / 4, GRIGIO_SCURO);
    const auto a = riga_ascan(l, 4);
    int py = -1;
    for (int i = 0; i < d.ascan.w; ++i) {
        const int y = d.ascan.y + d.ascan.h - 1 - static_cast<int>(std::lround(cs::frazione(a[i], PAV, TETTO) * (d.ascan.h - 2)));
        if (py >= 0) t.linea_verticale(d.ascan.x + i, std::min(py, y), std::max(py, y), GIALLO);
        t.pixel(d.ascan.x + i, y, BIANCO);
        py = y;
    }
    cornice(t, d.ascan, GRIGIO_SCURO);
    disegna_gram(t, l, d.lofar.x, d.lofar.y, Resa::bayer);
    cornice(t, d.lofar, GRIGIO_SCURO);
    scala_orizzontale(t, f.fos5, d.lofar.x, d.lofar.y + d.lofar.h + 3, d.lofar.w, 50, 400, 50, CIANO, "HZ");
    scrivi(t, f.orig, d.demon.x, d.demon.y - 12, "DEMON 0-40 HZ  ALBERO 3,1  PALE 7", CIANO_CHIARO);
    disegna_gram(t, m, d.demon.x, d.demon.y, Resa::bayer);
    cornice(t, d.demon, GRIGIO_SCURO);
    // in basso: contatti e stato
    scrivi(t, f.orig, d.contatti.x, d.contatti.y, "TRACKER RIL.  DB  /MIN  CLASSE        CONF", CIANO_CHIARO);
    scrivi(t, f.orig, d.contatti.x, d.contatti.y + 14, "S1      108   +10  +2,7  SOTTOMARINO   0,72", ROSA);
    scrivi(t, f.orig, d.contatti.x, d.contatti.y + 26, "S2      230   +4   -0,7  ?             --", GIALLO);
    scrivi(t, f.orig, d.contatti.x, d.contatti.y + 38, "S3      306   +15  +1,2  MERCANTILE    0,91", CIANO);
    scrivi(t, f.orig, d.stato.x, d.stato.y, "MODERNO  PAV +2  TETTO +16  VIRIDIS", BIANCO);
    barra_scala(t, {d.stato.x, d.stato.y + 14, 160, 8}, Resa::bayer);
    scrivi(t, f.fos5, d.stato.x + 166, d.stato.y + 14, "60-86 DB", GRIGIO);
    scrivi(t, f.orig, d.stato.x, d.stato.y + 28, "SLOT: [S1] [  ] [  ]   ASCOLTO S1", GRIGIO);
}

// Il modo 1989: tutto sul fosforo verde, con persistenza, bagliore, grana e righe
void sonar_1989(Tela& t, const Fonti& f) {
    guscio(t, f, true);
    const Disposizione d;
    const Gram b = btr(d.btr.w, d.btr.h), l = lofar(d.lofar.w, d.lofar.h, 10.0, 410.0), m = lofar(d.demon.w, d.demon.h, 0.0, 40.0);
    const cs::Fosforo::Effetti e;
    const auto gram_fosforo = [&](const Gram& g, Rettangolo r) {
        cs::Fosforo fo(g.w, g.h);
        for (int j = 0; j < g.h; ++j)
            for (int i = 0; i < g.w; ++i) fo.deponi(i, j, cs::frazione(g.at(i, j), PAV, TETTO));
        fo.componi(t, r.x, r.y, cs::SCALA_LUNGA, e);
    };
    testo_fosforo(t, d.btr.x, 17, "BTR SFERA INT 4", f.fos7);
    gram_fosforo(b, d.btr);
    for (int k = 0; k <= 315; k += 45) {
        const int px = d.btr.x + k * d.btr.w / 360;
        t.linea_verticale(px, d.btr.y - 5, d.btr.y - 2, fosforo(4));
        testo_fosforo(t, px + 2, d.btr.y - 11, std::to_string(k), f.fos5);
    }
    testo_fosforo(t, d.btr.x + static_cast<int>(100.0 / 360 * d.btr.w) - 2, d.btr.y + 2, "S1", f.fos5);
    testo_fosforo(t, d.btr.x + static_cast<int>(232.0 / 360 * d.btr.w) - 2, d.btr.y + 2, "S2", f.fos5);
    // A-scan con la persistenza: le ultime sei righe, ciascuna decaduta di un fotogramma in piu'
    testo_fosforo(t, d.ascan.x, 20, "A-SCAN LOFAR 108 VERO", f.fos7);
    cs::Fosforo as(d.ascan.w, d.ascan.h);
    for (int fot = 5; fot >= 0; --fot) {
        as.decadi(0.9);
        const auto a = riga_ascan(l, 2, fot * 2);
        for (int i = 1; i < d.ascan.w; ++i) {
            const auto yy = [&](int k) { return d.ascan.h - 2 - cs::frazione(a[k], PAV, TETTO) * (d.ascan.h - 4); };
            as.linea(i - 1, yy(i - 1), i, yy(i), 0.95);
        }
    }
    as.componi(t, d.ascan.x, d.ascan.y, cs::SCALA_LUNGA, e);
    gram_fosforo(l, d.lofar);
    for (int k = 50; k <= 350; k += 50) {
        const int px = d.lofar.x + static_cast<int>(std::lround((k - 10.0) / 400.0 * (d.lofar.w - 1)));
        t.linea_verticale(px, d.lofar.y + d.lofar.h + 1, d.lofar.y + d.lofar.h + 3, fosforo(4));
        testo_fosforo(t, px + 2, d.lofar.y + d.lofar.h + 2, std::to_string(k), f.fos5);
    }
    testo_fosforo(t, d.demon.x, d.demon.y - 12, "DEMON 0-40 HZ", f.fos7);
    gram_fosforo(m, d.demon);
    testo_fosforo(t, d.contatti.x, d.contatti.y, "TRK RIL  DB  /MIN CLASSE", f.fos7);
    testo_fosforo(t, d.contatti.x, d.contatti.y + 14, "S1  108 +10 +2.7 SSN?", f.fos7);
    testo_fosforo(t, d.contatti.x, d.contatti.y + 26, "S2  230  +4 -0.7", f.fos7);
    testo_fosforo(t, d.contatti.x, d.contatti.y + 38, "S3  306 +15 +1.2 MERC", f.fos7);
    testo_fosforo(t, d.stato.x, d.stato.y, "1989 PAV +2 TETTO +16 FOSFORO", f.fos7);
    testo_fosforo(t, d.stato.x, d.stato.y + 14, "PERSISTENZA 0.90  RIGHE CRT", f.fos5);
}

// Il dettaglio del fosforo ingrandito 4x: senza effetti, con la persistenza, con bagliore e righe
void fosforo_dettaglio(Tela& t, const Fonti& f) {
    t.riempi(NERO);
    scrivi(t, f.orig, 12, 8, "IL FOSFORO DEL 1989, INGRANDITO 4 VOLTE (A-SCAN, 76 PIXEL)", CIANO_CHIARO);
    const Gram l = lofar(298, 12, 10.0, 410.0);
    const char* titoli[] = {"OGGI: UN PUNTO PER COLONNA", "PERSISTENZA E TRACCIA", "+ BAGLIORE, GRANA, RIGHE"};
    for (int k = 0; k < 3; ++k) {
        cs::Fosforo as(76, 40);
        const int fotogrammi = k == 0 ? 1 : 6;
        for (int fot = fotogrammi - 1; fot >= 0; --fot) {
            as.decadi(0.9);
            const auto a = riga_ascan(l, 2, fot * 2);
            for (int i = 1; i < 76; ++i) {
                const auto yy = [&](int q) { return 38.0 - cs::frazione(a[q + 20], PAV, TETTO) * 36.0; };
                if (k == 0) as.deponi(i, static_cast<int>(std::lround(yy(i))), 0.95);
                else as.linea(i - 1, yy(i - 1), i, yy(i), 0.95);
            }
        }
        cs::Fosforo::Effetti e;
        if (k < 2) e.bagliore = 0.0, e.grana = 0.0, e.righe = 1.0;
        Tela piccola(76, 40);
        piccola.riempi(NERO);
        as.componi(piccola, 0, 0, cs::SCALA_LUNGA, e);
        const int x0 = 12 + k * 208, y0 = 40;
        scrivi(t, f.orig, x0, y0, titoli[k], k == 2 ? GIALLO : GRIGIO);
        for (int y = 0; y < 40; ++y)
            for (int x = 0; x < 48; ++x) t.rettangolo_pieno(x0 + x * 4, y0 + 16 + y * 4, 4, 4, piccola.at(x + 14, y));
        cornice(t, {x0, y0 + 16, 192, 160}, GRIGIO_SCURO);
    }
    scrivi(t, f.orig, 12, 236, "LA TRACCIA: XIAOLIN WU SU UN'INTENSITA' PER PIXEL, POI LA RAMPA VERDE A 128 GRADINI.", GRIGIO);
    scrivi(t, f.orig, 12, 252, "LA PERSISTENZA: OGNI FOTOGRAMMA V = MAX(NUOVO, VECCHIO X 0,90).", GRIGIO);
    scrivi(t, f.orig, 12, 268, "IL BAGLIORE: DAI VICINI PIU' ACCESI. LE RIGHE: UNA SU DUE ALL'85%.", GRIGIO);
    scrivi(t, f.orig, 12, 284, "LA GRANA: UN RUMORE FERMO PER PIXEL, SOLO SUL FOSFORO ACCESO.", GRIGIO);
    barra_scala(t, {12, 310, 600, 16}, Resa::bayer);
    scrivi(t, f.orig, 12, 332, "LA RAMPA DEL FOSFORO P1: DAL VETRO SPENTO AL QUASI BIANCO DEL NUCLEO", GRIGIO);
}

// --- le righe vere (float32, riga per riga dalla piu' vecchia) ---
Gram leggi_f32(const std::string& percorso, int bin, int righe) {
    Gram g{bin, righe, std::vector<double>(static_cast<std::size_t>(bin) * righe)};
    std::ifstream in(percorso, std::ios::binary);
    std::vector<float> riga(static_cast<std::size_t>(bin));
    for (int y = righe - 1; y >= 0; --y) {   // la riga 0 della tavola e' la piu' recente
        in.read(reinterpret_cast<char*>(riga.data()), static_cast<std::streamsize>(riga.size() * sizeof(float)));
        for (int x = 0; x < bin; ++x) g.db[static_cast<std::size_t>(y) * bin + x] = riga[static_cast<std::size_t>(x)];
    }
    return g;
}

// Riduce a w x h come la stazione: il massimo dei bin nel pixel e delle righe nella riga (i transitori restano)
Gram riduci(const Gram& g, int w, int h) {
    Gram r{w, h, std::vector<double>(static_cast<std::size_t>(w) * h, -1e9)};
    for (int y = 0; y < g.h; ++y)
        for (int x = 0; x < g.w; ++x) {
            double& v = r.db[static_cast<std::size_t>(y * h / g.h) * w + x * w / g.w];
            v = std::max(v, g.at(x, y));
        }
    return r;
}

void frange(Tela& t, const Fonti& f, const std::string& cartella, bool fosforo) {
    t.riempi(NERO);
    const Gram senza = riduci(leggi_f32(cartella + "/lofar_senza.f32", 2000, 600), 300, 176),
               con = riduci(leggi_f32(cartella + "/lofar_con.f32", 2000, 600), 300, 176);
    scrivi(t, f.orig, 12, 6, fosforo ? "RIGHE VERE NEL MODO 1989: LE FRANGE SUL FOSFORO, 128 LIVELLI"
                                     : "RIGHE VERE: LE FRANGE D'INTERFERENZA SUL LOFAR, 32 E 128 LIVELLI", CIANO_CHIARO);
    scrivi(t, f.orig, 12, 18, "688 CON LA TB-16, MERCANTILE AL CPA A 3 KM, CELTICA. 10-510 HZ, 20 MINUTI, 20-45 DB", GRIGIO);
    const auto pannello = [&](const Gram& g, int x, int y, Resa resa, const char* titolo, bool giallo) {
        scrivi(t, f.orig, x, y - 12, titolo, giallo ? GIALLO : GRIGIO);
        for (int j = 0; j < g.h; ++j)
            for (int i = 0; i < g.w; ++i) {
                const double u = cs::frazione(g.at(i, j), 20.0, 45.0);
                const std::uint8_t c = resa == Resa::oggi ? cs::indice(std::round(u * 31.0) / 31.0, 0, 0, cs::SCALA_LUNGA, false)
                                                          : cs::indice(u, x + i, y + j, cs::SCALA_LUNGA, resa == Resa::bayer);
                t.pixel(x + i, y + j, c);
            }
        cornice(t, {x, y, g.w, g.h}, GRIGIO_SCURO);
    };
    if (fosforo) {
        cs::Fosforo fo(300, 176);
        for (int j = 0; j < 176; ++j)
            for (int i = 0; i < 300; ++i) fo.deponi(i, j, cs::frazione(con.at(i, j), 20.0, 45.0));
        scrivi(t, f.orig, 12, 44, "OGGI: 32 GRADINI, NESSUN EFFETTO", GRIGIO);
        pannello(con, 12, 56, Resa::oggi, "", false);
        scrivi(t, f.orig, 328, 44, "128 + BAYER, BAGLIORE, GRANA, RIGHE", GIALLO);
        fo.componi(t, 328, 56, cs::SCALA_LUNGA, cs::Fosforo::Effetti{});
    } else {
        pannello(senza, 12, 56, Resa::oggi, "SENZA FRANGE, 32 LIVELLI (OGGI)", false);
        pannello(senza, 328, 56, Resa::bayer, "SENZA FRANGE, 128 + BAYER", true);
        pannello(con, 12, 258, Resa::oggi, "CON LE FRANGE, 32 LIVELLI (OGGI)", false);
        pannello(con, 328, 258, Resa::bayer, "CON LE FRANGE, 128 + BAYER", true);
    }
    if (fosforo) {   // l'ingrandimento 4x di un tratto di frange, sulle due rese
        Tela a(300, 176), b(300, 176);
        for (int j = 0; j < 176; ++j)
            for (int i = 0; i < 300; ++i) a.pixel(i, j, t.at(12 + i, 56 + j)), b.pixel(i, j, t.at(328 + i, 56 + j));
        for (int k = 0; k < 2; ++k) {
            const Tela& z = k ? b : a;
            const int x0 = 12 + k * 316;
            for (int j = 0; j < 40; ++j)
                for (int i = 0; i < 75; ++i) t.rettangolo_pieno(x0 + i * 4, 252 + j * 4, 4, 4, z.at(200 + i, 70 + j));
            cornice(t, {x0, 252, 300, 160}, GRIGIO_SCURO);
        }
        scrivi(t, f.orig, 12, 240, "INGRANDITE 4 VOLTE, LE FRANGE A 250-350 HZ:", GRIGIO);
    }
    scrivi(t, f.orig, 12, 440, "LE FRANGE VANNO DI 3-10 DB: A 32 GRADINI DIVENTANO CURVE DI LIVELLO, A 128 CON IL", GRIGIO);
    scrivi(t, f.orig, 12, 452, "DITHERING RESTANO UNA SFUMATURA. DATI GREZZI (S+N)/N, SENZA NORMALIZZATORE.", GRIGIO);
}

// Il fondo del fosforo (proposta per il 1989 scuro): le righe vere normalizzate come la stazione, a finestra divisa, e
// mostrate fra +2 e +16 dB come la riga di stato di oggi; senza e con il velo di grana sotto il pavimento
void finestra_divisa(std::vector<double>& px) {
    const int n = static_cast<int>(px.size());
    std::vector<double> out(px.size());
    for (int x = 0; x < n; ++x) {
        double s = 0.0;
        int k = 0;
        for (int d = 4; d <= 24; ++d) {
            if (x - d >= 0) s += px[static_cast<std::size_t>(x - d)], ++k;
            if (x + d < n) s += px[static_cast<std::size_t>(x + d)], ++k;
        }
        out[static_cast<std::size_t>(x)] = px[static_cast<std::size_t>(x)] - (k ? s / k : 0.0);
    }
    px = std::move(out);
}

void fosforo_fondo(Tela& t, const Fonti& f, const std::string& cartella) {
    t.riempi(NERO);
    Gram l = riduci(leggi_f32(cartella + "/lofar_con.f32", 2000, 600), 300, 300);
    for (int y = 0; y < l.h; ++y) {
        std::vector<double> r(l.db.begin() + y * l.w, l.db.begin() + (y + 1) * l.w);
        finestra_divisa(r);
        std::copy(r.begin(), r.end(), l.db.begin() + y * l.w);
    }
    scrivi(t, f.orig, 12, 6, "IL FONDO DEL FOSFORO: LO SCHERMO DEL 1989 CON I DATI VICINO AL PAVIMENTO", CIANO_CHIARO);
    scrivi(t, f.orig, 12, 18, "RIGHE VERE NORMALIZZATE A FINESTRA DIVISA, COME LA STAZIONE; PAV +2, TETTO +16", GRIGIO);
    for (int k = 0; k < 2; ++k) {
        cs::Fosforo fo(l.w, l.h);
        for (int j = 0; j < l.h; ++j)
            for (int i = 0; i < l.w; ++i) fo.deponi(i, j, cs::frazione(l.at(i, j), 2.0, 16.0));
        cs::Fosforo::Effetti e;
        e.fondo = k ? 0.07 : 0.0;
        const int x0 = 12 + k * 316;
        scrivi(t, f.orig, x0, 42, k ? "DOPO: UN VELO DI GRANA AL 7%" : "OGGI: SOTTO IL PAVIMENTO E' NERO", k ? GIALLO : GRIGIO);
        fo.componi(t, x0, 56, cs::SCALA_LUNGA, e);
        cornice(t, {x0, 56, l.w, l.h}, GRIGIO_SCURO);
    }
    scrivi(t, f.orig, 12, 368, "IL VELO E' UN RUMORE FERMO PER PIXEL CHE SI SOMMA DOVE IL SEGNALE E' DEBOLE; SUI PICCHI", GRIGIO);
    scrivi(t, f.orig, 12, 382, "QUASI NIENTE. LE RIGHE DEL CRT E LA GRANA SI VEDONO ANCHE A SCHERMO VUOTO, COME SU", GRIGIO);
    scrivi(t, f.orig, 12, 396, "UN BQQ-5 ACCESO. NEL CODICE: CASCATE::FOSFORO::EFFETTI::FONDO (0: SPENTO, OGGI).", GRIGIO);
}

}  // namespace

int main(int argc, char** argv) {
    if (argc < 3) {
        std::fprintf(stderr, "uso: %s <dati> <cartella d'uscita>\n", argv[0]);
        return 2;
    }
    const std::string dati = argv[1], fuori = argv[2];
    const Fonti f{Font::carica(dati + "/font/originale.fnt"), Font::carica(dati + "/font/fosforo688_7x9.fnt"),
                  Font::carica(dati + "/font/fosforo688_5x7.fnt")};
    const struct {
        const char* nome;
        void (*disegna)(Tela&, const Fonti&);
        cs::Tinta tinta;
    } tavole[] = {
        {"livelli_cascata", livelli_cascata, cs::Tinta::viridis},
        {"sonar_moderno", sonar_moderno, cs::Tinta::viridis},
        {"sonar_1989", sonar_1989, cs::Tinta::fosforo},
        {"fosforo_dettaglio", fosforo_dettaglio, cs::Tinta::fosforo},
    };
    for (const auto& x : tavole) {
        Palette p = Palette::carica(dati + "/palette/giorno.pal");
        applica_tema(p, TemaStazione{});
        cs::applica(p, x.tinta, cs::SCALA_LUNGA);
        Tela t(640, 480);
        x.disegna(t, f);
        t.salva_bmp(fuori + "/" + x.nome + ".bmp", p);
    }
    if (argc >= 4) {
        for (const bool fo : {false, true}) {
            Palette p = Palette::carica(dati + "/palette/giorno.pal");
            applica_tema(p, TemaStazione{});
            cs::applica(p, fo ? cs::Tinta::fosforo : cs::Tinta::viridis, cs::SCALA_LUNGA);
            Tela t(640, 480);
            frange(t, f, argv[3], fo);
            t.salva_bmp(fuori + (fo ? "/frange_fosforo.bmp" : "/frange_vere.bmp"), p);
        }
        Palette p = Palette::carica(dati + "/palette/giorno.pal");
        applica_tema(p, TemaStazione{});
        cs::applica(p, cs::Tinta::fosforo, cs::SCALA_LUNGA);
        Tela t(640, 480);
        fosforo_fondo(t, f, argv[3]);
        t.salva_bmp(fuori + "/fosforo_fondo.bmp", p);
    }
    return 0;
}
