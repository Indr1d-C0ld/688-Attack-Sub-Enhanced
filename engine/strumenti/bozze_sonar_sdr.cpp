// Le bozze della stazione Sonar (F8) in stile cascate SDR (proposta e del kit; ricerca in docs/ricerca/09 §4 e
// docs/ricerca/30_sonar_grafica_sdr.md §4). Dati sintetici con la forma di quelli del modello (nota del disassemblatore
// sonar-dati-per-la-grafica.md): BTR a 72 fasci da 5 gradi in dB sopra il fondo coi fasci ciechi, LOFAR 10-410 Hz,
// DEMON 0-100 Hz. Sei tavole 640x360 (la parte di sopra della stazione; il pannello di sotto resta):
//   sonar_A_1989, sonar_A_moderno: il quadro SDR (BTR a sinistra con la scala a tacche, i tracker come segnalibri, il
//     fascio evidenziato e i ciechi tratteggiati; a destra A-scan, band plan, LOFAR, barra dello zoom, DEMON piccolo;
//     sotto la tabella dei contatti);
//   sonar_B_1989, sonar_B_moderno: la consolle BQQ-5 (due CRT incassati coi tasti quadrati accanto);
//   sonar_C_1989, sonar_C_moderno: l'analizzatore (il LOFAR del fascio a tutta larghezza con lo zoom, le righe della
//     libreria sovrapposte, il picco, il delta, il DEMON, gli otto slot).
//   sonar_D_1989, sonar_D_moderno: l'Okean dell'Alfa (lo schermo tondo di Enisej come cascata polare, col cieco a
//     tratteggio, la scala L/P sul bordo, il fascio e i tracker; lo spettro e il DEMOD sul piccolo schermo; lo stampino
//     traslitterato; nel 1989 il fosforo verde giallino).
// Uso: bozze_sonar_sdr <dati> <cartella d'uscita>
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdio>
#include <functional>
#include <string>
#include <vector>

#include "grafica/consolle.hpp"
#include "grafica/sale.hpp"
#include "grafica/font.hpp"
#include "grafica/lamiera.hpp"
#include "grafica/palette.hpp"
#include "grafica/scala_cascata.hpp"
#include "grafica/tela.hpp"
#include "grafica/tema_stazioni.hpp"

using namespace m688;

namespace {

constexpr double PI_ = 3.14159265358979323846;
constexpr double PAVIMENTO = 2.0, TETTO = 16.0;   // dB sopra il fondo, come la stazione (PAV +2, TETTO +16): il
                                                  // rumore cade sul nero (GQRX taglia a nero il primo 8%)

// --- i dati sintetici ---

double hash01(int a, int b, int c) {
    std::uint32_t h = static_cast<std::uint32_t>(a) * 374761393u + static_cast<std::uint32_t>(b) * 668265263u +
                      static_cast<std::uint32_t>(c) * 2246822519u;
    h = (h ^ (h >> 13)) * 1274126177u;
    h ^= h >> 16;
    return (h & 0xffffff) / 16777216.0;
}
// la grana del fondo: circa 1,5 dB (come il modello), sempre positiva sopra il pavimento solo a volte
double grana(int a, int b, int c) {
    double s = 0;
    for (int k = 0; k < 4; ++k) s += hash01(a, b, c * 4 + k);
    return (s - 2.0) * 1.5;
}
double scarto(double a, double b) { return std::remainder(a - b, 360.0); }

constexpr int FASCI = 72;
constexpr int GRADI_FASCIO = 5;
constexpr double PRUA = 270.0;   // il battello per 270: la poppa (fasci ciechi della sfera) a 090
bool cieco(int fascio) { return std::abs(scarto(fascio * GRADI_FASCIO + 2.5, PRUA + 180.0)) < 22.5; }

struct Contatto {
    const char* nome;
    double ril_ora, ril_prima, db;   // rilevamento adesso e 4 minuti fa
    const char* classe;
    int colore;                      // EGA nel modo moderno (per classificazione)
};
const Contatto CONTATTI[] = {{"S1", 32.0, 41.0, 9.0, "PROBSUB", 12}, {"S2", 250.0, 249.0, 4.5, "NONSUB", 7},
                             {"S3", 300.0, 322.0, 6.5, "POSSUB", 14}};

// il BTR: dB del fascio `f` alla riga `r` (0 la piu' recente, `righe` le ultime righe), NaN se cieco
double btr(int f, int r, int righe) {
    if (cieco(f)) return std::nan("");
    const double centro = f * GRADI_FASCIO + 2.5, s = static_cast<double>(r) / righe;
    double db = grana(f, r, 1);
    for (const auto& c : CONTATTI) {
        const double b = c.ril_ora + (c.ril_prima - c.ril_ora) * s;
        const double d = scarto(centro, b) / 4.0;
        db = std::max(db, c.db * std::exp(-0.5 * d * d) + grana(f, r, 2) * 0.5);
    }
    if (r > 150 && r < 168 && centro > 190 && centro < 240) db += 3.0 * hash01(f, r, 3);   // un banco di gamberi
    return db;
}

// le righe del LOFAR del fascio di S1 (e quelle del proprio battello: 60 e 400 Hz)
struct Riga {
    double hz, db;
    int tipo;   // 0 rete, 1 pompa, 2 elica, 3 turbina, 4 propria
};
const Riga RIGHE[] = {{50.0, 8.0, 0}, {100.0, 6.0, 0}, {150.0, 4.0, 0}, {82.5, 5.0, 1}, {165.0, 3.0, 1},
                      {29.4, 4.5, 2}, {58.8, 3.0, 2}, {245.0, 3.5, 3}, {60.0, 6.0, 4}, {400.0, 5.0, 4}};
double lofar(double hz, double hz_px, int r) {
    double db = grana(static_cast<int>(hz * 10), r, 5);
    for (const auto& rr : RIGHE) {
        const double d = (hz - rr.hz) / std::max(hz_px * 0.7, 0.05);
        const double a = rr.db * (0.75 + 0.25 * std::sin(r * 0.05 + rr.hz));
        db = std::max(db, a * std::exp(-0.5 * d * d) + grana(static_cast<int>(hz * 10), r, 6) * 0.4);
    }
    return db;
}
// il DEMON: asse 4,2 Hz e armoniche, pale 7 x 4,2 = 29,4 Hz
double demon(double hz, double hz_px, int r) {
    double db = grana(static_cast<int>(hz * 20), r, 7);
    for (const double f : {4.2, 8.4, 12.6, 29.4, 58.8}) {
        const double d = (hz - f) / std::max(hz_px * 0.7, 0.05);
        db = std::max(db, (f == 29.4 ? 7.0 : f == 4.2 ? 6.0 : 3.5) * std::exp(-0.5 * d * d));
    }
    return db;
}

// --- la resa ---

enum class Modo { m1989, moderno };

struct Stile {
    Modo modo;
    std::uint8_t testo, tenue, accento, griglia, cursore, rosso;
};
Stile stile(Modo m) {
    if (m == Modo::m1989) return {m, 10, 2, 10, 2, 10, 10};
    return {m, 15, 7, 11, 8, 15, 12};
}

const Font* F5 = nullptr;   // fosforo688_5x7: il testo degli schermi
int largo(const std::string& s) { return F5->larghezza(s, 1, F5->formato() == 2 ? F5->spazio_predefinito() : 1, F5->formato() != 2); }
void scrivi(Tela& t, int x, int y, const std::string& s, std::uint8_t c) {
    F5->scrivi(t, x, y, s, c, 1, F5->formato() == 2 ? F5->spazio_predefinito() : 1, F5->formato() != 2);
}
// il testo con l'ombra nera, per stare sopra la cascata (GQRX)
void scrivi_ombra(Tela& t, int x, int y, const std::string& s, std::uint8_t c) {
    scrivi(t, x + 1, y + 1, s, 0);
    scrivi(t, x, y, s, c);
}

// il pixel di cascata per un valore: nel 1989 16 livelli di fosforo senza dithering, nel moderno 128 col dithering;
// `piu` alza il valore (il fascio evidenziato: niente trasparenza, un gradino in piu')
void pixel_cascata(Tela& t, int x, int y, double db, const Stile& s, double piu = 0.0) {
    double u = cascate::frazione(db, PAVIMENTO, TETTO);
    u = std::clamp(u + piu, 0.0, 1.0);
    if (s.modo == Modo::m1989) {
        u = std::floor(u * 15.0 + 0.5) / 15.0;
        t.pixel(x, y, cascate::indice(u, x, y, cascate::SCALA_LUNGA, false));
    } else {
        t.pixel(x, y, cascate::indice(u, x, y, cascate::SCALA_LUNGA, true));
    }
}

void linea_punti(Tela& t, int x0, int y0, int x1, int y1, std::uint8_t c, int passo = 3) {
    const int n = std::max(std::abs(x1 - x0), std::abs(y1 - y0));
    for (int k = 0; k <= n; k += passo) t.pixel(x0 + (x1 - x0) * k / std::max(n, 1), y0 + (y1 - y0) * k / std::max(n, 1), c);
}

// il BTR: scala a tacche in alto (tacca ogni 5, lunga ogni 10, numero ogni 30), i tracker come segnalibri sopra la
// scala, il fascio puntato evidenziato (parentesi sulla scala, bordi a puntini, un gradino in piu'), i ciechi a
// tratteggio, l'ora a sinistra. r: tutto il riquadro; restituisce l'area della cascata
Rettangolo disegna_btr(Tela& t, Rettangolo r, const Stile& s, int fascio_scelto) {
    const int sinistra = 18, x0 = r.x + sinistra, larghezza = FASCI * 4;
    const int y_tracker = r.y, y_scala = r.y + 14, y0 = r.y + 30, h = r.h - 30;
    // le etichette dei tracker, a righe se si toccano, con la linea tratteggiata in giu'
    int ultimo_x = -100, riga = 0;
    for (const auto& c : CONTATTI) {
        const int x = x0 + static_cast<int>(c.ril_ora / 5.0 * 4.0);
        riga = x - ultimo_x < 18 ? 1 - riga : 0;
        ultimo_x = x;
        const int ty = y_tracker + riga * 7;
        const std::uint8_t col = s.modo == Modo::moderno ? static_cast<std::uint8_t>(c.colore) : s.testo;
        if (s.modo == Modo::moderno) {
            t.rettangolo_pieno(x - 7, ty, 15, 8, 0);
            t.linea_orizzontale(x - 7, x + 7, ty + 9, col);
        }
        scrivi(t, x - largo(c.nome) / 2, ty + 1, c.nome, col);
    }
    // la scala
    for (int g = 0; g <= 360; g += 5) {
        const int x = x0 + g * 4 / 5;
        const int lun = g % 30 == 0 ? 5 : g % 10 == 0 ? 3 : 1;
        t.linea_verticale(x, y0 - 1 - lun, y0 - 1, s.griglia == 2 ? s.testo : s.tenue);
        if (g % 30 == 0 && g < 360) {
            const std::string n = std::to_string(g);
            scrivi(t, x - largo(n) / 2 + (g == 0 ? largo(n) / 2 : 0), y_scala, n, s.tenue);
        }
    }
    // il fascio puntato: una barretta piena sotto la scala, larga quanto il fascio
    const int fx = x0 + fascio_scelto * 4;
    t.rettangolo_pieno(fx, y0 - 9, 4, 3, s.accento);
    // la cascata
    for (int y = 0; y < h; ++y)
        for (int x = 0; x < larghezza; ++x) {
            const int f = x / 4;
            const double db = btr(f, y, h);
            if (std::isnan(db)) {   // cieco: tratteggio diagonale tenue, non un blocco
                if ((x + y) % 8 == 0) t.pixel(x0 + x, y0 + y, s.griglia);
                continue;
            }
            const bool nel_fascio = f == fascio_scelto;
            pixel_cascata(t, x0 + x, y0 + y, db, s, nel_fascio ? (s.modo == Modo::moderno ? 0.12 : 1.0 / 15.0) : 0.0);
        }
    // i bordi a puntini del fascio
    linea_punti(t, fx - 1, y0, fx - 1, y0 + h - 1, s.modo == Modo::moderno ? s.rosso : s.accento, 2);
    linea_punti(t, fx + 4, y0, fx + 4, y0 + h - 1, s.modo == Modo::moderno ? s.rosso : s.accento, 2);
    // le linee dei segnalibri al 40% (un punto ogni 3) fino alla traccia
    for (const auto& c : CONTATTI) {
        const int x = x0 + static_cast<int>(c.ril_ora / 5.0 * 4.0);
        linea_punti(t, x, y_scala + 8, x, y0 + 6, s.modo == Modo::moderno ? static_cast<std::uint8_t>(c.colore) : s.tenue, 3);
    }
    // l'ora: una tacca ogni minuto (60 righe), il numero ogni minuto a sinistra
    for (int y = 0; y < h; y += 60) {
        t.linea_orizzontale(x0 - 4, x0 - 1, y0 + y, s.tenue);
        const std::string m = y == 0 ? "0" : "-" + std::to_string(y / 60);
        scrivi(t, r.x, y0 + y - 3, m, s.tenue);
    }
    return {x0, y0, larghezza, h};
}

// l'A-scan: lo spettro istantaneo (media delle ultime righe), la traccia del massimo piu' tenue, i cerchietti sui picchi
void disegna_ascan(Tela& t, Rettangolo r, const Stile& s, double f0, double f1, bool picco) {
    const double hz_px = (f1 - f0) / r.w;
    int prima_y = -1;
    std::vector<double> media(static_cast<std::size_t>(r.w)), massimo(static_cast<std::size_t>(r.w));
    for (int x = 0; x < r.w; ++x) {
        const double hz = f0 + x * hz_px;
        double m = 0, mx = -9;
        for (int k = 0; k < 8; ++k) {
            const double v = lofar(hz, hz_px, k);
            m += v / 8.0;
            mx = std::max(mx, v);
        }
        media[static_cast<std::size_t>(x)] = m;
        massimo[static_cast<std::size_t>(x)] = mx;
    }
    const auto y_di = [&](double db) { return r.y + r.h - 1 - static_cast<int>(std::clamp(db / TETTO, 0.0, 1.0) * (r.h - 2)); };
    if (picco)
        for (int x = 0; x < r.w; ++x) t.pixel(r.x + x, y_di(massimo[static_cast<std::size_t>(x)]), s.tenue);
    for (int x = 0; x < r.w; ++x) {
        const int y = y_di(media[static_cast<std::size_t>(x)]);
        if (prima_y >= 0) t.linea_verticale(r.x + x, std::min(prima_y, y), std::max(prima_y, y), s.testo);
        prima_y = y;
    }
    if (picco)   // i cerchietti sui picchi che l'analisi ha trovato
        for (const auto& rr : RIGHE) {
            if (rr.hz < f0 || rr.hz > f1 || rr.tipo == 4) continue;
            const int x = r.x + static_cast<int>((rr.hz - f0) / hz_px);
            const int y = y_di(rr.db * 0.9) - 4;
            for (int a = 0; a < 12; ++a)
                t.pixel(x + static_cast<int>(std::lround(2 * std::cos(a * PI_ / 6))), y + static_cast<int>(std::lround(2 * std::sin(a * PI_ / 6))), s.accento);
        }
}

// il band plan: la striscia delle righe attese della classe sovrapposta, colorate per tipo (moderno); nel 1989 solo le
// tacche fisse di 50, 60 e 400 Hz
void disegna_band_plan(Tela& t, Rettangolo r, const Stile& s, double f0, double f1) {
    const double hz_px = (f1 - f0) / r.w;
    if (s.modo == Modo::m1989) {
        for (const double hz : {50.0, 60.0, 400.0}) {
            if (hz < f0 || hz > f1) continue;
            const int x = r.x + static_cast<int>((hz - f0) / hz_px);
            t.linea_verticale(x, r.y + r.h - 4, r.y + r.h - 1, s.testo);
        }
        return;
    }
    static constexpr std::uint8_t COLORI[] = {14, 11, 13, 10, 9};   // rete, pompa, elica, turbina, propria
    t.rettangolo_pieno(r.x, r.y, r.w, r.h, 0);
    for (const auto& rr : RIGHE) {
        if (rr.hz < f0 || rr.hz > f1) continue;
        const int x = r.x + static_cast<int>((rr.hz - f0) / hz_px);
        t.rettangolo_pieno(x - 1, r.y + 1, 3, r.h - 2, COLORI[rr.tipo]);
    }
}

// la cascata del LOFAR (o del DEMON) fra f0 e f1
void disegna_gram(Tela& t, Rettangolo r, const Stile& s, double f0, double f1, bool demon_) {
    const double hz_px = (f1 - f0) / r.w;
    for (int y = 0; y < r.h; ++y)
        for (int x = 0; x < r.w; ++x) {
            const double hz = f0 + x * hz_px;
            pixel_cascata(t, r.x + x, r.y + y, demon_ ? demon(hz, hz_px, y) : lofar(hz, hz_px, y), s);
        }
}

// la scala delle frequenze sotto un gram: tacca ogni 10 Hz, numero ogni 50 (ogni 10 sotto i 100 Hz di larghezza)
void scala_hz(Tela& t, Rettangolo r, const Stile& s, double f0, double f1) {
    const double hz_px = (f1 - f0) / r.w;
    const double passo = f1 - f0 <= 120 ? 10.0 : 50.0, tacca = f1 - f0 <= 120 ? 2.0 : 10.0;
    for (double hz = std::ceil(f0 / tacca) * tacca; hz <= f1; hz += tacca) {
        const int x = r.x + static_cast<int>((hz - f0) / hz_px);
        const bool lunga = std::fmod(hz, passo) < 0.01;
        t.linea_verticale(x, r.y, r.y + (lunga ? 3 : 1), s.tenue);
        if (lunga) {
            const std::string n = std::to_string(static_cast<int>(hz));
            scrivi(t, x - largo(n) / 2, r.y + 5, n, s.tenue);
        }
    }
}

// la barra dello zoom: la finestra nella banda intera, il fattore, i tasti R C D (moderno)
void barra_zoom(Tela& t, Rettangolo r, const Stile& s, double f0, double f1, double b0, double b1) {
    t.linea_orizzontale(r.x, r.x + r.w - 46, r.y + 3, s.griglia);
    const int a = r.x + static_cast<int>((f0 - b0) / (b1 - b0) * (r.w - 46)), b = r.x + static_cast<int>((f1 - b0) / (b1 - b0) * (r.w - 46));
    const int w = std::max(3, b - a);   // la finestra: il contorno, non il pieno
    t.linea_orizzontale(a, a + w - 1, r.y + 1, s.accento), t.linea_orizzontale(a, a + w - 1, r.y + 5, s.accento);
    t.linea_verticale(a, r.y + 1, r.y + 5, s.accento), t.linea_verticale(a + w - 1, r.y + 1, r.y + 5, s.accento);
    char z[16];
    std::snprintf(z, sizeof z, "X%.0f", (b1 - b0) / (f1 - f0));
    scrivi(t, r.x + r.w - 42, r.y, z, s.testo);
    if (s.modo == Modo::moderno) scrivi(t, r.x + r.w - 20, r.y, "RCD", s.tenue);
}

// la tabella dei contatti: S, rilevamento, dB, deriva, classe, schiera
void tabella_contatti(Tela& t, Rettangolo r, const Stile& s) {
    const int col[] = {0, 22, 58, 86, 130, 190};
    const char* testa[] = {"TR.", "RIL.", "DB", "*/MIN", "CLASSE", "SCHIERA"};
    for (int k = 0; k < 6; ++k) scrivi(t, r.x + col[k], r.y, testa[k], s.accento);
    t.linea_orizzontale(r.x, r.x + r.w - 1, r.y + 9, s.griglia);
    int y = r.y + 12;
    const char* schiere[] = {"SFERA", "SFERA", "TB-16"};
    int i = 0;
    for (const auto& c : CONTATTI) {
        char ril[8], db[8], der[8];
        std::snprintf(ril, sizeof ril, "%03.0f", c.ril_ora);
        std::snprintf(db, sizeof db, "%.0f", c.db);
        std::snprintf(der, sizeof der, "%+.1f", (c.ril_ora - c.ril_prima) / 4.0);
        const std::uint8_t tinta = s.modo == Modo::moderno ? static_cast<std::uint8_t>(c.colore) : s.testo;
        if (i == 0) {   // il designato: la fascia blu nel moderno, la freccia nel 1989 (il verde pieno coprirebbe il testo)
            if (s.modo == Modo::moderno) t.rettangolo_pieno(r.x - 2, y - 1, r.w + 4, 9, 1);
            else scrivi(t, r.x - 8, y, ">", s.testo);
        }
        const std::string v[] = {c.nome, ril, db, der, c.classe, schiere[i]};
        for (int k = 0; k < 6; ++k) scrivi(t, r.x + col[k], y, v[k], k == 0 || k == 4 ? tinta : s.testo);
        y += 10;
        ++i;
    }
}

// --- le tre bozze ---

// A: il quadro SDR, tutto in un vetro
void bozza_A(Tela& t, Modo m) {
    const Stile s = stile(m);
    t.rettangolo_pieno(0, 0, 640, 360, 0);
    // la cornice della sala: il blu solo fuori dagli schermi
    t.rettangolo_pieno(0, 0, 640, 360, m == Modo::m1989 ? 1 : 8);
    t.rettangolo_pieno(4, 4, 632, 352, 0);
    scrivi(t, 10, 7, m == Modo::m1989 ? "BTR  SFERA  INT 4  X1" : "BTR  SFERA  INT 4  X1  VIRIDIS  PAV +2  TETTO +16", s.testo);
    const Rettangolo btr_area = disegna_btr(t, {8, 18, 310, 252}, s, 6);   // il fascio di S1 (32 gradi)
    (void)btr_area;
    // a destra: A-scan, band plan, LOFAR, scala, zoom, DEMON
    const int x = 330, w = 300;
    const double f0 = 10, f1 = 410;
    scrivi(t, x, 7, "LOFAR 032  S1  10-410 HZ", s.testo);
    const Rettangolo ascan{x, 18, w, 44};
    t.linea_orizzontale(x, x + w - 1, ascan.y + ascan.h - 1, s.griglia);
    disegna_ascan(t, ascan, s, f0, f1, m == Modo::moderno);
    disegna_band_plan(t, {x, 63, w, 7}, s, f0, f1);
    disegna_gram(t, {x, 71, w, 130}, s, f0, f1, false);
    scala_hz(t, {x, 202, w, 12}, s, f0, f1);
    barra_zoom(t, {x, 216, w, 8}, s, f0, f1, 10, 410);
    scrivi(t, x, 228, "DEMON 0-100 HZ  ASSE 4,2  PALE 29,4 (7)", s.testo);
    disegna_gram(t, {x, 238, w, 30}, s, 0, 100, true);
    scala_hz(t, {x, 269, w, 12}, s, 0, 100);
    // sotto: i contatti e la riga di stato
    t.linea_orizzontale(8, 631, 284, s.griglia);
    tabella_contatti(t, {12, 288, 300, 60}, s);
    scrivi(t, 330, 290, "CURSORE 50,0 HZ  +8 DB", s.testo);
    scrivi(t, 330, 300, m == Modo::moderno ? "DELTA 50,0 HZ: ARMONICA 2 DI 50" : "DEMON: CAVITAZIONE LEGGIBILE", s.tenue);
    scrivi(t, 330, 312, "S1 PROBSUB: RETE 50 HZ, POMPA 82,5", s.testo);
    scrivi(t, 330, 324, m == Modo::moderno ? "ALLA TMA: S1 032 -2,2*/MIN" : "ANALIZZATORE: 4 SUL FASCIO", s.tenue);
}

// B: la consolle BQQ-5, due CRT incassati e i tasti quadrati
void bozza_B(Tela& t, Modo m) {
    const Stile s = stile(m);
    lamiera::lamiera(t, {0, 0, 640, 360}, 7, 31);
    const Rettangolo a = crt_rettangolare(t, {4, 4, 330, 300});
    t.rettangolo_pieno(a.x, a.y, a.w, a.h, 0);
    disegna_btr(t, {a.x + 2, a.y + 4, a.w - 4, a.h - 8}, s, 6);
    const Rettangolo b = crt_rettangolare(t, {338, 4, 252, 300});
    t.rettangolo_pieno(b.x, b.y, b.w, b.h, 0);
    const double f0 = 10, f1 = 210;
    scrivi(t, b.x + 4, b.y + 3, "LOFAR 032  10-210 HZ", s.testo);
    disegna_ascan(t, {b.x + 4, b.y + 14, b.w - 8, 36}, s, f0, f1, m == Modo::moderno);
    disegna_band_plan(t, {b.x + 4, b.y + 51, b.w - 8, 6}, s, f0, f1);
    disegna_gram(t, {b.x + 4, b.y + 58, b.w - 8, b.h - 140}, s, f0, f1, false);
    scala_hz(t, {b.x + 4, b.y + b.h - 81, b.w - 8, 12}, s, f0, f1);
    scrivi(t, b.x + 4, b.y + b.h - 66, "DEMON 0-100 HZ", s.testo);
    disegna_gram(t, {b.x + 4, b.y + b.h - 56, b.w - 8, 38}, s, 0, 100, true);
    scala_hz(t, {b.x + 4, b.y + b.h - 17, b.w - 8, 12}, s, 0, 100);
    // i tasti quadrati a luce accanto ai CRT
    const char* tasti[] = {"BTR", "LOF", "DEM", "INT", "AUTO", "CURS", "TRK", "ANAL", "TMA"};
    for (int k = 0; k < 9; ++k) {
        const Rettangolo r{596, 8 + k * 32, 38, 28};
        lamiera::tasto(t, r, tasti[k], k == 1 || k == 6 ? ColoreLampada::verde : ColoreLampada::ambra,
                       k == 1 ? lamiera::Stato::evidenziato : lamiera::Stato::normale, false, *F5);
    }
    // sotto i CRT la targhetta e la tabella dei contatti in un vetro
    const Rettangolo v{8, 310, 580, 46};
    lamiera::vetro(t, v);
    tabella_contatti(t, {v.x + 6, v.y + 4, 300, 40}, s);
    scrivi(t, v.x + 320, v.y + 6, "AN/BQQ-5  SALA SONAR", s.tenue);
    scrivi(t, v.x + 320, v.y + 18, "S1 PROBSUB 032  -2,2*/MIN", s.testo);
}

// C: l'analizzatore, il LOFAR del fascio a tutta larghezza con lo zoom e la libreria
void bozza_C(Tela& t, Modo m) {
    const Stile s = stile(m);
    t.rettangolo_pieno(0, 0, 640, 360, m == Modo::m1989 ? 1 : 8);
    t.rettangolo_pieno(4, 4, 632, 352, 0);
    const double f0 = 20, f1 = 120;   // zoom x4 sulla banda delle righe di S1
    scrivi(t, 10, 7, "ANALIZZATORE  FASCIO 032  S1  20-120 HZ  X4", s.testo);
    scrivi(t, 420, 7, m == Modo::moderno ? "LIBRERIA: VICTOR III" : "LIBRERIA: CONFRONTO", s.accento);
    const Rettangolo ascan{10, 18, 616, 50};
    t.linea_orizzontale(ascan.x, ascan.x + ascan.w - 1, ascan.y + ascan.h - 1, s.griglia);
    disegna_ascan(t, ascan, s, f0, f1, m == Modo::moderno);
    disegna_band_plan(t, {10, 69, 616, 7}, s, f0, f1);
    const Rettangolo g{10, 77, 616, 150};
    disegna_gram(t, g, s, f0, f1, false);
    // le righe della libreria sovrapposte: tratteggiate, colorate per tipo (moderno) o a puntini verdi (1989)
    const double hz_px = (f1 - f0) / g.w;
    static constexpr std::uint8_t COLORI[] = {14, 11, 13, 10, 9};
    for (const auto& rr : RIGHE) {
        if (rr.hz < f0 || rr.hz > f1 || rr.tipo == 4) continue;
        const int x = g.x + static_cast<int>((rr.hz - f0) / hz_px) + 3;
        linea_punti(t, x, g.y, x, g.y + g.h - 1, m == Modo::moderno ? COLORI[rr.tipo] : s.tenue, 4);
    }
    // il cursore e il delta (due righe di rete: 50 e 100 Hz)
    const int c1 = g.x + static_cast<int>((50 - f0) / hz_px), c2 = g.x + static_cast<int>((100 - f0) / hz_px);
    t.linea_verticale(c1, g.y, g.y + g.h - 1, s.cursore);
    linea_punti(t, c2, g.y, c2, g.y + g.h - 1, s.cursore, 2);
    scrivi_ombra(t, c1 + 3, g.y + 3, "50,0 HZ +8 DB", s.cursore);
    scrivi_ombra(t, c2 + 3, g.y + 3, "DELTA 50,0: ARM. 2", s.cursore);
    scala_hz(t, {10, 228, 616, 12}, s, f0, f1);
    barra_zoom(t, {10, 242, 616, 8}, s, f0, f1, 10, 410);
    // sotto: il DEMON a sinistra, gli slot a destra
    scrivi(t, 10, 256, "DEMON 0-60 HZ  ASSE 4,2  PALE 29,4 (7)", s.testo);
    disegna_gram(t, {10, 266, 300, 60}, s, 0, 60, true);
    scala_hz(t, {10, 327, 300, 12}, s, 0, 60);
    scrivi(t, 330, 256, "SLOT  (S SALVA, Y CONFRONTA, C DIFFERENZA)", s.testo);
    for (int k = 0; k < 8; ++k) {
        const Rettangolo sl{330 + (k % 4) * 74, 268 + (k / 4) * 40, 70, 34};
        t.linea_orizzontale(sl.x, sl.x + sl.w - 1, sl.y, s.griglia), t.linea_orizzontale(sl.x, sl.x + sl.w - 1, sl.y + sl.h - 1, s.griglia);
        t.linea_verticale(sl.x, sl.y, sl.y + sl.h - 1, s.griglia), t.linea_verticale(sl.x + sl.w - 1, sl.y, sl.y + sl.h - 1, s.griglia);
        if (k < 3) {
            disegna_gram(t, {sl.x + 2, sl.y + 9, sl.w - 4, sl.h - 11}, s, f0, f1, false);
            scrivi(t, sl.x + 3, sl.y + 2, std::string(k == 0 ? "S1 1004" : k == 1 ? "S1 1012" : "S3 1015"), s.testo);
        } else {
            scrivi(t, sl.x + 3, sl.y + 2, std::to_string(k + 1) + " LIBERO", s.tenue);
        }
    }
}

// --- D: l'Okean dell'Alfa (dossier 19 §6.6), con le regole di leggibilita' della ricerca 30 ---

// i contatti nei rilevamenti relativi (gradi dalla prua, positivi a dritta): adesso e 3 minuti fa
const Contatto CONTATTI_OKEAN[] = {{"S1", -35.0, -48.0, 9.0, "PROBSUB", 12}, {"S2", 72.0, 70.0, 4.5, "NONSUB", 7},
                                   {"S3", 104.0, 88.0, 6.5, "POSSUB", 14}};
constexpr double SETTORE_OKEAN = 120.0;   // il ventaglio di Enisej: da -120 a +120, oltre il cieco di poppa

double btr_okean(double rel, int r, int righe) {
    const double s = static_cast<double>(r) / righe;
    const int k = static_cast<int>(std::floor((rel + 180.0) / 2.5));
    double db = grana(k, r, 11);
    for (const auto& c : CONTATTI_OKEAN) {
        const double d = (rel - (c.ril_ora + (c.ril_prima - c.ril_ora) * s)) / 4.0;
        db = std::max(db, c.db * std::exp(-0.5 * d * d) + grana(k, r, 12) * 0.5);
    }
    return db;
}

const Font* STAMPINO = nullptr;   // stampino_urss_7: il latino traslitterato degli schermi dell'Alfa
void scrivi_s(Tela& t, int x, int y, const std::string& testo, std::uint8_t c) {
    STAMPINO->scrivi(t, x, y, testo, c, 1, STAMPINO->formato() == 2 ? STAMPINO->spazio_predefinito() : 1, STAMPINO->formato() != 2);
}
int largo_s(const std::string& testo) {
    return STAMPINO->larghezza(testo, 1, STAMPINO->formato() == 2 ? STAMPINO->spazio_predefinito() : 1, STAMPINO->formato() != 2);
}

// lo schermo tondo di Enisej come cascata polare: l'angolo e' il rilevamento relativo (la prua in alto), il raggio il
// tempo, l'ultima riga sul bordo e le vecchie verso il centro (la storia lunga che l'occhio integra, ricerca 30 §3.3);
// il pavimento a +2 dB (il rumore cade sul nero, non piu' il disco del ventaglio); il cieco di poppa a tratteggio; la
// scala sul bordo (tacca ogni 5, lunga ogni 10, numero ogni 30, L a sinistra e P a dritta); il fascio con le parentesi
// sul bordo e i lati a puntini; i tracker come segnalibri sul bordo, su due raggi se si toccano; i minuti a cerchi
void schermo_enisej(Tela& t, int cx, int cy, int raggio, const Stile& s, double fascio) {
    crt_tondo(t, cx, cy, raggio);
    const int R = raggio - 1, dati = R - 24, dentro = 12;
    const double xc = cx + 0.5, yc = cy + 0.5;
    for (int y = cy - R; y <= cy + R; ++y)
        for (int x = cx - R; x <= cx + R; ++x) {
            const double dx = x + 0.5 - xc, dy = y + 0.5 - yc, rho = std::hypot(dx, dy);
            if (rho > R) continue;
            t.pixel(x, y, 0);
            if (rho > dati || rho < dentro) continue;
            const double rel = std::atan2(dx, -dy) * 180.0 / PI_;
            if (std::fabs(rel) > SETTORE_OKEAN) {   // il cieco: tratteggio tenue, non un blocco
                if ((x + y) % 6 == 0) t.pixel(x, y, s.griglia);
                continue;
            }
            const int r = static_cast<int>(dati - rho);
            pixel_cascata(t, x, y, btr_okean(rel, r, dati - dentro), s, std::fabs(rel - fascio) < 2.5 ? 1.0 / 15.0 : 0.0);
        }
    const auto punto = [&](double rel, double rho) {
        const double a = rel * PI_ / 180.0;
        return std::pair<int, int>{static_cast<int>(std::lround(xc + rho * std::sin(a) - 0.5)), static_cast<int>(std::lround(yc - rho * std::cos(a) - 0.5))};
    };
    // i minuti: cerchi a puntini ogni 30 righe (una riga ogni 2 s)
    for (int m = 30; dati - m > dentro; m += 30)
        for (int g = -120; g <= 120; g += 3) {
            const auto [x, y] = punto(g, dati - m);
            t.pixel(x, y, s.griglia);
        }
    // i bordi del settore e la scala
    for (const double g : {-SETTORE_OKEAN, SETTORE_OKEAN})
        for (int k = dentro; k < dati; k += 3) {
            const auto [x, y] = punto(g, k);
            t.pixel(x, y, s.tenue);
        }
    for (int g = -180; g < 180; g += 5) {
        const int lun = g % 30 == 0 ? 6 : g % 10 == 0 ? 4 : 2;
        for (int k = 1; k <= lun; ++k) {
            const auto [x, y] = punto(g, dati + k);
            t.pixel(x, y, std::abs(g) > SETTORE_OKEAN ? s.griglia : s.tenue);
        }
        if (g % 30 == 0) {
            const std::string n = g == 0 ? "0" : g == -180 ? "180" : (g < 0 ? "L" : "P") + std::to_string(std::abs(g));
            const auto [x, y] = punto(g, dati + 14);
            scrivi_s(t, x - largo_s(n) / 2, y - 3, n, std::abs(g) > SETTORE_OKEAN ? s.griglia : s.testo);
        }
    }
    // il fascio puntato: le parentesi sul bordo e i lati a puntini fino al centro
    for (const double lato : {-2.5, 2.5}) {
        for (int k = dentro; k < dati; k += 3) {
            const auto [x, y] = punto(fascio + lato, k);
            t.pixel(x, y, s.cursore);
        }
        for (int k = 0; k < 4; ++k) {
            const auto [x, y] = punto(fascio + lato * (1.0 + k * 0.3), dati + 1);
            t.pixel(x, y, s.cursore);
        }
    }
    // i tracker: il segnalibro (una tacca) dentro il bordo e il nome, su due raggi se due nomi si toccano
    double prima = -999.0;
    int giro = 0;
    for (const auto& c : CONTATTI_OKEAN) {
        giro = std::fabs(c.ril_ora - prima) < 14.0 ? 1 - giro : 0;
        prima = c.ril_ora;
        const std::uint8_t tinta = s.modo == Modo::moderno ? static_cast<std::uint8_t>(c.colore) : s.testo;
        for (int k = 1; k <= 5; ++k) {
            const auto [x, y] = punto(c.ril_ora, dati - k);
            t.pixel(x, y, tinta);
        }
        const auto [x, y] = punto(c.ril_ora, dati - 12 - 10 * giro);
        scrivi_s(t, x - largo_s(c.nome) / 2 + 1, y - 2, c.nome, 0);   // l'ombra nera sopra la cascata
        scrivi_s(t, x - largo_s(c.nome) / 2, y - 3, c.nome, tinta);
    }
    // il proprio battello al centro
    for (int k = -3; k <= 3; ++k) t.pixel(cx, cy + k, s.testo);
    t.pixel(cx - 1, cy - 2, s.testo), t.pixel(cx + 1, cy - 2, s.testo);
}

void bozza_D(Tela& t, Modo m) {
    Stile s = stile(m);
    if (m == Modo::m1989) s = {m, fosforo(7), fosforo(4), fosforo(7), fosforo(2), fosforo(7), fosforo(7)};
    const Font* f5 = F5;
    F5 = STAMPINO;   // gli schermi dell'Alfa: lo stampino traslitterato anche nei pezzi del LOFAR
    t.rettangolo_pieno(0, 0, 640, 360, 0);
    // il pulto: lo smalto, le targhette, la lampada di Luch
    const Rettangolo pulto{0, 0, 316, 300};
    pannello(t, pulto, BattelloTema::alfa);
    targhetta(t, {6, 4, 0, 0}, "OKEAN", *STAMPINO);
    lampada_tonda(t, pulto.w - 64, 9, 4, ColoreLampada::verde, false);
    targhetta(t, {pulto.w - 56, 4, 0, 0}, "LUCH", *STAMPINO);
    targhetta(t, {6, pulto.h - 16, 0, 0}, "ENISEJ  OTN. PELENG", *STAMPINO);
    schermo_enisej(t, 158, 154, 122, s, -35.0);
    // a destra il piccolo schermo rettangolare: spettro, band plan, cascata, scala, DEMOD
    pannello(t, {316, 0, 324, 300}, BattelloTema::alfa);
    // la cappa nera dell'Alfa (come quella dello schermo tondo), non la cornice grigia col cappuccio del 688
    t.rettangolo_pieno(322, 4, 312, 292, smalto(1));
    t.rettangolo_pieno(324, 6, 308, 288, 0);
    const Rettangolo b{330, 12, 296, 276};
    t.linea_orizzontale(b.x - 1, b.x + b.w, b.y - 1, smalto(3)), t.linea_orizzontale(b.x - 1, b.x + b.w, b.y + b.h, smalto(3));
    t.linea_verticale(b.x - 1, b.y - 1, b.y + b.h, smalto(3)), t.linea_verticale(b.x + b.w, b.y - 1, b.y + b.h, smalto(3));
    const double f0 = 10, f1 = 410;
    scrivi_s(t, b.x + 4, b.y + 3, "SPEKTR  L35  S1  10-410 GC", s.testo);
    disegna_ascan(t, {b.x + 4, b.y + 14, b.w - 8, 34}, s, f0, f1, m == Modo::moderno);
    disegna_band_plan(t, {b.x + 4, b.y + 49, b.w - 8, 6}, s, f0, f1);
    disegna_gram(t, {b.x + 4, b.y + 56, b.w - 8, b.h - 136}, s, f0, f1, false);
    scala_hz(t, {b.x + 4, b.y + b.h - 79, b.w - 8, 12}, s, f0, f1);
    scrivi_s(t, b.x + 4, b.y + b.h - 64, "DEMOD 0-100 GC  VAL 4,2  LOP 29,4 (7)", s.testo);
    disegna_gram(t, {b.x + 4, b.y + b.h - 54, b.w - 8, 36}, s, 0, 100, true);
    scala_hz(t, {b.x + 4, b.y + b.h - 17, b.w - 8, 12}, s, 0, 100);
    // sotto: i contatti nei rilevamenti relativi, in una striscia nera
    t.rettangolo_pieno(0, 300, 640, 60, 0);
    const int col[] = {0, 24, 64, 90, 136};
    const char* testa[] = {"TR.", "PEL.", "DB", "*/MIN", "KLASS"};
    for (int k = 0; k < 5; ++k) scrivi_s(t, 12 + col[k], 304, testa[k], s.accento);
    t.linea_orizzontale(12, 300, 313, s.griglia);
    int y = 316;
    for (const auto& c : CONTATTI_OKEAN) {
        char ril[8], db[8], der[8];
        std::snprintf(ril, sizeof ril, "%c%03.0f", c.ril_ora < 0 ? 'L' : 'P', std::fabs(c.ril_ora));
        std::snprintf(db, sizeof db, "%.0f", c.db);
        std::snprintf(der, sizeof der, "%+.1f", (c.ril_ora - c.ril_prima) / 3.0);
        const std::uint8_t tinta = s.modo == Modo::moderno ? static_cast<std::uint8_t>(c.colore) : s.testo;
        if (y == 316 && s.modo == Modo::moderno) t.rettangolo_pieno(10, y - 1, 292, 9, 1);
        if (y == 316 && s.modo == Modo::m1989) scrivi_s(t, 4, y, ">", s.testo);
        const std::string v[] = {c.nome, ril, db, der, c.classe};
        for (int k = 0; k < 5; ++k) scrivi_s(t, 12 + col[k], y, v[k], k == 0 || k == 4 ? tinta : s.testo);
        y += 10;
    }
    scrivi_s(t, 330, 304, "OKEAN  SHUMOPELENGOVANIE", s.tenue);
    scrivi_s(t, 330, 316, "S1 PROBSUB L35  +4,3*/MIN", s.testo);
    scrivi_s(t, 330, 328, m == Modo::moderno ? "ANALIZ: 4 NA LUCHE" : "BEZ REGISTRATORA", s.tenue);
    F5 = f5;
}

}  // namespace

int main(int argc, char** argv) {
    if (argc < 3) {
        std::fprintf(stderr, "uso: %s <dati> <cartella d'uscita>\n", argv[0]);
        return 2;
    }
    const std::string dati = argv[1], fuori = argv[2];
    const Font f5 = Font::carica(dati + "/font/fosforo688_5x7.fnt");
    F5 = &f5;
    const Font stampino = Font::carica(dati + "/font/stampino_urss_7.fnt");
    STAMPINO = &stampino;
    for (const Modo m : {Modo::m1989, Modo::moderno}) {
        Palette p = Palette::carica(dati + "/palette/giorno.pal");
        applica_tema(p, TemaStazione{});
        cascate::applica(p, m == Modo::m1989 ? cascate::Tinta::fosforo : cascate::Tinta::viridis, cascate::SCALA_LUNGA);
        const std::string suf = m == Modo::m1989 ? "_1989" : "_moderno";
        const struct { const char* nome; void (*disegna)(Tela&, Modo); } bozze[] = {{"sonar_A", bozza_A}, {"sonar_B", bozza_B}, {"sonar_C", bozza_C}};
        for (const auto& b : bozze) {
            Tela t(640, 480);
            t.riempi(0);
            b.disegna(t, m);
            t.salva_bmp(fuori + "/" + b.nome + suf + ".bmp", p);
            std::printf("%s%s\n", b.nome, suf.c_str());
        }
        // D, l'Okean: il tema dell'Alfa; nel 1989 la cascata sul fosforo verde giallino (dossier 19 §8; STIMA: la
        // rampa dal nero a 550 nm circa), nel moderno viridis
        Palette pa = Palette::carica(dati + "/palette/giorno.pal");
        TemaStazione alfa;
        alfa.battello = BattelloTema::alfa;
        applica_tema(pa, alfa);
        cascate::applica(pa, m == Modo::m1989 ? cascate::Tinta::fosforo : cascate::Tinta::viridis, cascate::SCALA_LUNGA);
        if (m == Modo::m1989)
            for (int i = 1; i < cascate::SCALA_LUNGA.livelli; ++i) {
                const double u = static_cast<double>(i) / (cascate::SCALA_LUNGA.livelli - 1);
                const double v = std::pow(u, 0.9);
                pa.imposta(cascate::SCALA_LUNGA.primo + i, static_cast<std::uint8_t>(std::min(255.0, 30 + 205 * v + 30 * u * u)),
                           static_cast<std::uint8_t>(std::min(255.0, 40 + 215 * v)), static_cast<std::uint8_t>(std::min(255.0, 10 + 60 * v * v)));
            }
        Tela t(640, 480);
        t.riempi(0);
        bozza_D(t, m);
        t.salva_bmp(fuori + "/sonar_D" + suf + ".bmp", pa);
        std::printf("sonar_D%s\n", suf.c_str());
    }
    return 0;
}
