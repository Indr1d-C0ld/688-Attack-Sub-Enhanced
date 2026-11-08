#include "grafica/oculare.hpp"
#include "per_righe.hpp"

#include <algorithm>
#include <array>
#include <climits>
#include <cmath>
#include <cstdint>
#include <unordered_map>
#include <vector>

#include "grafica/periscopio.hpp"
#include "nucleo/geometria.hpp"
#include "nucleo/onde.hpp"
#include "nucleo/periscopi.hpp"
#include "nucleo/portata_luci.hpp"

namespace m688 {

ParametriOculare parametri_oculare(const DatiPeriscopio& d, const StatoPeriscopio& s) {
    ParametriOculare p;
    p.attivo = true;
    const int n = static_cast<int>(d.campi_gradi.size());
    const int i = std::clamp(s.ingrandimento, 0, std::max(n - 1, 0));
    auto voce = [i](const std::vector<double>& v, double riserva) {
        return v.empty() ? riserva : v[static_cast<std::size_t>(std::min<int>(i, static_cast<int>(v.size()) - 1))];
    };
    p.campo_gradi = voce(d.campi_gradi, 32.0);
    p.guadagno_luci = luci::guadagno_ottico(d.obiettivo_mm, d.trasmissione, voce(d.ingrandimenti, 1.5));
    p.passo_gradi = voce(d.reticolo_gradi, 4.0);
    p.passo_fine_gradi = voce(d.reticolo_fine_gradi, 1.0);
    p.reticolo = d.reticolo == "sovietico" ? Reticolo::sovietico : d.reticolo == "britannico" ? Reticolo::britannico : Reticolo::americano;
    p.elevazione_gradi = s.elevazione;
    p.modo = static_cast<ModoOculare>(s.modo);   // stessi valori nello stesso ordine
    p.tremolio = s.tremolio;
    p.sott_acqua = !s.testa_fuori;
    p.altezza_testa_m = s.altezza_testa_m;
    if (s.filtro >= 0 && s.filtro < static_cast<int>(d.filtri.size())) p.filtro = filtro_da_nome(d.filtri[static_cast<std::size_t>(s.filtro)]);
    p.sdoppiamento_gradi = s.sdoppiamento_gradi;
    p.reticolo_misura = d.telemetro == "reticolo";
    p.telecamera_verticale = s.telecamera_verticale;
    return p;
}

Filtro filtro_da_nome(const std::string& nome) {
    if (nome == "polaroid") return Filtro::polaroid;
    if (nome == "rosso") return Filtro::rosso;
    if (nome == "verde") return Filtro::verde;
    if (nome == "giallo") return Filtro::giallo;
    if (nome == "affumicato") return Filtro::affumicato;
    return Filtro::nessuno;
}

static_assert(static_cast<int>(ModoVista::termico) == static_cast<int>(ModoOculare::termico) &&
              static_cast<int>(ModoVista::tv) == static_cast<int>(ModoOculare::tv) &&
              static_cast<int>(ModoVista::intensificatore) == static_cast<int>(ModoOculare::intensificatore) &&
              static_cast<int>(ModoVista::bassa_luminosita) == static_cast<int>(ModoOculare::bassa_luminosita));

}  // namespace m688

namespace m688::oculare {

namespace {

constexpr int BAYER[4][4] = {{0, 8, 2, 10}, {12, 4, 14, 6}, {3, 11, 1, 9}, {15, 7, 13, 5}};
double soglia(int x, int y) { return (BAYER[y & 3][x & 3] + 0.5) / 16.0; }

// valore 0-1 sulla rampa, retinato
std::uint8_t rampa(double v, int x, int y) {
    const double i = std::clamp(v, 0.0, 1.0) * (N_RAMPA - 1);
    const double b = std::floor(i);
    return static_cast<std::uint8_t>(RAMPA + static_cast<int>(b) + ((i - b) > soglia(x, y) ? 1 : 0));
}

// rumore deterministico in [0, 1) per pixel e fotogramma
double hash(int x, int y, int f) {
    std::uint32_t h = static_cast<std::uint32_t>(x) * 374761393u + static_cast<std::uint32_t>(y) * 668265263u +
                      static_cast<std::uint32_t>(f) * 2246822519u;
    h = (h ^ (h >> 13)) * 1274126177u;
    return ((h ^ (h >> 16)) & 0xffffff) / 16777216.0;
}

// rumore di valore liscio (telecamera verticale)
double liscio(double x, double y, int seme) {
    const int xi = static_cast<int>(std::floor(x)), yi = static_cast<int>(std::floor(y));
    const double fx = x - xi, fy = y - yi;
    const double sx = fx * fx * (3 - 2 * fx), sy = fy * fy * (3 - 2 * fy);
    const double a = hash(xi, yi, seme), b = hash(xi + 1, yi, seme), c = hash(xi, yi + 1, seme), d = hash(xi + 1, yi + 1, seme);
    return a + (b - a) * sx + (c - a) * sy + (a - b - c + d) * sx * sy;
}

double frattale(double x, double y, int seme) {
    double v = 0, a = 0.5;
    for (int o = 0; o < 4; ++o, x *= 2.03, y *= 2.03, a *= 0.5) v += a * liscio(x, y, seme + o);
    return v / 0.9375;
}

bool di_vista(int i) {
    for (const auto& [a, b] : INDICI_VISTA)
        if (i >= a && i <= b) return true;
    return false;
}

// colori della rampa per modo: dal nero al chiaro del fosforo
struct Fosforo { std::array<double, 3> scuro, chiaro; };
Fosforo fosforo(ModoOculare m) {
    switch (m) {
    case ModoOculare::bassa_luminosita: return {{0, 10, 2}, {150, 255, 140}};   // verde del Type 18 (STIMA)
    case ModoOculare::intensificatore: return {{10, 14, 0}, {222, 246, 96}};    // verde giallino sovietico (dossier 19)
    case ModoOculare::termico: return {{4, 4, 4}, {250, 250, 246}};             // bianco caldo
    default: return {{8, 8, 10}, {236, 236, 226}};                              // TV in bianco e nero
    }
}

bool monocromo(ModoOculare m) { return m != ModoOculare::giorno; }

}  // namespace

double pixel_per_radiante(const ParametriOculare& p) {
    const double ampiezza = p.modo == ModoOculare::tv ? 2.0 * TV_MEZZA_L : 2.0 * RAGGIO;
    return ampiezza / radianti(p.campo_gradi);
}

Rettangolo area(const ParametriOculare& p) {
    if (p.modo == ModoOculare::tv) return {CENTRO_X - TV_MEZZA_L, CENTRO_Y - RAGGIO, 2 * TV_MEZZA_L, 2 * RAGGIO};
    return {CENTRO_X - RAGGIO, CENTRO_Y - RAGGIO, 2 * RAGGIO, 2 * RAGGIO};
}

namespace {
// distanza dal bordo in frazione (0 al centro, 1 sul bordo; oltre, fuori)
double bordo(const ParametriOculare& p, int x, int y) {
    const double dx = x + 0.5 - CENTRO_X, dy = y + 0.5 - CENTRO_Y;
    if (p.modo != ModoOculare::tv) return std::hypot(dx, dy) / RAGGIO;
    constexpr double R = 28.0;   // angoli tondi del tubo
    const double ax = std::fabs(dx) - (TV_MEZZA_L - R), ay = std::fabs(dy) - (RAGGIO - R);
    const double b = std::max(std::fabs(dx) / TV_MEZZA_L, std::fabs(dy) / RAGGIO);
    if (ax > 0 && ay > 0) return std::hypot(ax, ay) > R ? 1.01 : std::max(b, 1.0 - (R - std::hypot(ax, ay)) / TV_MEZZA_L);
    return b;
}
}  // namespace

bool dentro(const ParametriOculare& p, int x, int y) { return bordo(p, x, y) <= 1.0; }

void tremolio(const ParametriOculare& p, double t, double& d_ril, double& d_el) {
    const double a = std::clamp(p.tremolio, 0.0, 1.0) * TREMOLIO_GRADI;
    d_ril = a * (0.7 * std::sin(2 * PI * 1.7 * t) + 0.3 * std::sin(2 * PI * 4.3 * t + 1.0));
    d_el = a * (0.6 * std::sin(2 * PI * 2.3 * t + 2.0) + 0.4 * std::sin(2 * PI * 5.1 * t + 0.4));
}

void palette(Palette& pal, const ParametriOculare& p) {
    if (monocromo(p.modo)) {
        const Fosforo f = fosforo(p.modo);
        const double fumo = p.filtro == Filtro::affumicato ? 0.45 : 1.0;
        for (int i = 0; i < N_RAMPA; ++i) {
            const double t = std::pow(i / (N_RAMPA - 1.0), 0.9);
            std::array<std::uint8_t, 3> c{};
            for (int k = 0; k < 3; ++k) c[k] = static_cast<std::uint8_t>(std::lround((f.scuro[k] + t * (f.chiaro[k] - f.scuro[k])) * fumo));
            pal.imposta(RAMPA + i, c[0], c[1], c[2]);
        }
        return;
    }
    std::array<double, 3> m{1, 1, 1};
    switch (p.filtro) {
    case Filtro::nessuno: return;
    case Filtro::polaroid: m = {0.82, 0.85, 0.8}; break;   // toglie i riflessi del mare: tutto un po' piu' scuro
    case Filtro::rosso: m = {1.0, 0.3, 0.25}; break;
    case Filtro::verde: m = {0.35, 1.0, 0.4}; break;
    case Filtro::giallo: m = {1.0, 0.95, 0.45}; break;
    case Filtro::affumicato: m = {0.42, 0.42, 0.42}; break;
    }
    for (const auto& [a, b] : INDICI_VISTA)
        for (int i = a; i <= b; ++i) {
            const auto c = pal.rgb(i);
            pal.imposta(i, static_cast<std::uint8_t>(c[0] * m[0]), static_cast<std::uint8_t>(c[1] * m[1]), static_cast<std::uint8_t>(c[2] * m[2]));
        }
}

void sdoppia(Tela& t, const ParametriOculare& p) {
    if (p.reticolo_misura) return;   // col reticolo niente immagine fantasma: il cursore lo disegna reticolo()
    const int d = static_cast<int>(std::lround(std::tan(radianti(p.sdoppiamento_gradi)) * pixel_per_radiante(p)));
    if (d == 0) return;
    const Rettangolo a = area(p);
    std::vector<std::uint8_t> copia(static_cast<std::size_t>(a.w) * a.h);
    for (int y = 0; y < a.h; ++y)
        for (int x = 0; x < a.w; ++x) copia[static_cast<std::size_t>(y) * a.w + x] = t.at(a.x + x, a.y + y);
    for (int y = 0; y < a.h; ++y)
        for (int x = 0; x < a.w; ++x) {
            if (((a.x + x) + (a.y + y)) & 1) continue;   // a scacchiera: meta' dei pixel all'immagine fantasma
            // fantasma piu' in alto della stessa quantita' (o a sinistra, con la lente ruotata)
            const int sx = p.sdoppiamento_orizzontale ? x + d : x, sy = p.sdoppiamento_orizzontale ? y : y + d;
            if (sx < 0 || sy < 0 || sx >= a.w || sy >= a.h) continue;
            t.pixel(a.x + x, a.y + y, copia[static_cast<std::size_t>(sy) * a.w + sx]);
        }
}

void converti(Tela& t, const Palette& giorno, const ParametriOculare& p, double luce, double tempo_s) {
    if (!monocromo(p.modo)) return;
    // amplificazione della luce ambiente (STIMA): l'intensificatore vede con la luce delle stelle, la TV a bassa
    // luminosita' con la luna, la TV comune solo di giorno; il termico non dipende dalla luce
    double guadagno = 1.5, grana = 0.04;
    switch (p.modo) {
    case ModoOculare::bassa_luminosita: guadagno = 8.0; grana = 0.05 + 0.10 * (1.0 - std::min(1.0, 4.0 * luce)); break;
    case ModoOculare::intensificatore: guadagno = 14.0; grana = 0.06 + 0.13 * (1.0 - std::min(1.0, 4.0 * luce)); break;
    case ModoOculare::termico: grana = 0.03; break;
    default: grana = 0.05 + 0.08 * (1.0 - std::min(1.0, 2.0 * luce)); break;
    }
    const double amplificata = std::min(1.0, (std::max(luce, 0.0) + 0.02) * guadagno);
    std::array<double, 256> lum{};
    for (int i = 0; i < 256; ++i) {
        const auto c = giorno.rgb(i);
        const double y = (0.299 * c[0] + 0.587 * c[1] + 0.114 * c[2]) / 255.0;
        if (p.modo == ModoOculare::termico) {   // mare e cielo freddi, scafi e fumo caldi, fuoco caldissimo
            lum[i] = (i >= 199 && i <= 247) ? 0.1 + 0.08 * y : (i >= 96 && i <= 143) ? 0.22 + 0.1 * y : 0.75 + 0.15 * y;
            if (i == 0) lum[i] = 0.05;
        } else {
            lum[i] = di_vista(i) ? y * amplificata : std::min(1.0, y * std::max(1.0, 0.3 * guadagno));
        }
    }
    for (const int f : {colori::FUOCO[0], colori::FUOCO[1], colori::FUOCO[2], colori::FUOCO[3], colori::FUOCO[4], colori::FUOCO[5],
                        colori::FUMO_ACCESO, colori::FUMO_ACCESO_CHIARO})
        if (p.modo == ModoOculare::termico) lum[static_cast<std::size_t>(f)] = 1.0;
    const int fotogramma = static_cast<int>(std::floor(tempo_s * 15.0));
    const Rettangolo a = area(p);
    for (int y = a.y; y < a.y + a.h; ++y)
        for (int x = a.x; x < a.x + a.w; ++x) {
            double v = lum[t.at(x, y)] + grana * (hash(x, y, fotogramma) + hash(x + 7, y + 3, fotogramma) - 1.0) * 1.7;
            if (p.modo == ModoOculare::tv && (y & 1)) v *= 0.8;   // righe del tubo
            t.pixel(x, y, rampa(v, x, y));
        }
}

void disegna_reticolo(Tela& t, const ParametriOculare& p) {
    const double ppr = pixel_per_radiante(p);
    // nero sull'ottica; la TV ha il reticolo elettronico chiaro, i modi notte il piu' scuro della rampa
    const std::uint8_t c = p.modo == ModoOculare::tv ? static_cast<std::uint8_t>(RAMPA + N_RAMPA - 2)
                           : monocromo(p.modo)       ? static_cast<std::uint8_t>(RAMPA)
                                                     : static_cast<std::uint8_t>(colori::NERO);
    auto px = [&](int x, int y) { if (dentro(p, x, y)) t.pixel(x, y, c); };
    const Rettangolo a = area(p);
    const double fine = std::max(p.passo_fine_gradi, 1e-3), grande = std::max(p.passo_gradi, fine);
    const int n = static_cast<int>(std::ceil(p.campo_gradi / fine));
    auto tacche = [&](bool orizzontale, int lato_lungo, int lato_corto, bool solo_un_lato) {
        for (int k = -n; k <= n; ++k) {
            if (k == 0) continue;
            const double g = k * fine;
            const bool lunga = std::fabs(std::remainder(g, grande)) < 1e-6;
            const int o = static_cast<int>(std::lround(std::tan(radianti(g)) * ppr));
            const int l = lunga ? lato_lungo : lato_corto;
            for (int s = solo_un_lato ? 0 : -l; s <= l; ++s)
                orizzontale ? px(CENTRO_X + o, CENTRO_Y - s) : px(CENTRO_X + s, CENTRO_Y + o);
        }
    };
    switch (p.reticolo) {
    case Reticolo::americano:
    case Reticolo::britannico:
        // croce sottile a tutto campo con le tacche (foto dei 688: Houston, Tucson; dossier 20 §6.1)
        for (int x = a.x; x < a.x + a.w; ++x) px(x, CENTRO_Y);
        for (int y = a.y; y < a.y + a.h; ++y) px(CENTRO_X, y);
        tacche(true, 6, 3, false);
        tacche(false, 6, 3, false);
        if (p.reticolo == Reticolo::britannico)   // pali spessi dal bordo verso il centro
            for (int s = -1; s <= 1; ++s) {
                for (int x = a.x; x < CENTRO_X - RAGGIO / 3; ++x) { px(x, CENTRO_Y + s); px(2 * CENTRO_X - x, CENTRO_Y + s); }
                for (int y = CENTRO_Y + RAGGIO / 3; y < a.y + a.h; ++y) px(CENTRO_X + s, y);
            }
        break;
    case Reticolo::sovietico: {
        // griglia goniometrica a due scale (dossier 20 §4.3): scala orizzontale con le tacche in su, verticale con le
        // tacche a destra, vuoto al centro e un vertice a punta sul bersaglio
        const int vuoto = 8;
        for (int x = a.x; x < a.x + a.w; ++x)
            if (std::abs(x - CENTRO_X) > vuoto) px(x, CENTRO_Y);
        for (int y = a.y; y < a.y + a.h; ++y)
            if (std::abs(y - CENTRO_Y) > vuoto) px(CENTRO_X, y);
        tacche(true, 8, 4, true);
        tacche(false, 8, 4, true);
        for (int s = 0; s <= 5; ++s) { px(CENTRO_X - s, CENTRO_Y + vuoto + s); px(CENTRO_X + s, CENTRO_Y + vuoto + s); }
        // il telemetro a reticolo: il galleggiamento del bersaglio sulla linea di mezzo, il cursore sulla testa d'albero;
        // le divisioni fra i due le conta il giocatore (una barra a sinistra della scala, col dente verso la scala)
        if (p.reticolo_misura && p.sdoppiamento_gradi > 0.0) {
            const int yc = CENTRO_Y - static_cast<int>(std::lround(std::tan(radianti(p.sdoppiamento_gradi)) * ppr));
            for (int x = CENTRO_X - 16; x <= CENTRO_X - 2; ++x) { px(x, yc); px(x, yc - 1); }
            for (int s = 1; s <= 3; ++s) { px(CENTRO_X - 16, yc - 1 - s); px(CENTRO_X - 16, yc + s); }
        }
        break;
    }
    }
}

void maschera(Tela& t, const ParametriOculare& p) {
    const Rettangolo rit = t.ritaglio();
    const double inizio = p.modo == ModoOculare::tv ? 0.9 : 0.8;   // vignettatura: dal bordo del campo verso il nero
    per_righe(rit.y, rit.y + rit.h, [&](int y) {
        for (int x = rit.x; x < rit.x + rit.w; ++x) {
            if (y < 30) continue;   // la scala dei rilevamenti in alto resta
            const double b = bordo(p, x, y);
            if (b > 1.0) { t.pixel(x, y, colori::NERO); continue; }
            if (b <= inizio) continue;
            const double f = std::pow((b - inizio) / (1.0 - inizio), 1.6);
            if (f > soglia(x, y)) t.pixel(x, y, colori::NERO);
        }
    });
}

namespace {
// un pixel d'acqua torbida: dal basso scuro al chiaro verso la superficie, con le bolle che salgono
std::uint8_t pixel_acqua(const Rettangolo& a, int x, int y, double luce, double tempo_s) {
    const double l = 0.25 + 0.75 * std::clamp(luce, 0.0, 1.0);
    const double f = static_cast<double>(y - a.y) / a.h;
    double i = colori::MARE_VICINO + (1.0 - f) * l * (colori::MARE_ORIZZONTE - colori::MARE_VICINO) +
               6.0 * (frattale(x * 0.02, y * 0.02 + tempo_s * 0.8, 81) - 0.5);
    if (hash(x / 3, (y + static_cast<int>(tempo_s * 90.0)) / 3, 83) > 0.996) i = colori::MARE_ORIZZONTE + 8;   // bolle
    const double b = std::floor(i);
    return static_cast<std::uint8_t>(std::clamp(b + ((i - b) > soglia(x, y) ? 1.0 : 0.0), 96.0, 143.0));
}

constexpr int SCHIUMA = colori::GRIGI + 14;   // il grigio piu' chiaro degli sprite: schiuma e spruzzi (presi dalla luce)

// Una goccia sulla finestra: una piccola lente che mostra la scena capovolta e ingrandita, col riflesso in alto
void goccia(Tela& t, const Rettangolo& a, const std::vector<std::uint8_t>& copia, double cx, double cy, double r) {
    const int x0 = static_cast<int>(std::floor(cx - r)), x1 = static_cast<int>(std::ceil(cx + r));
    const int y0 = static_cast<int>(std::floor(cy - r)), y1 = static_cast<int>(std::ceil(cy + r * 1.2));
    for (int y = y0; y <= y1; ++y)
        for (int x = x0; x <= x1; ++x) {
            if (x < a.x || y < a.y || x >= a.x + a.w || y >= a.y + a.h) continue;
            const double dx = (x + 0.5 - cx) / r, dy = (y + 0.5 - cy) / (r * 1.2);   // un po' allungata in giu'
            if (dx * dx + dy * dy > 1.0) continue;
            const int sx = std::clamp(static_cast<int>(std::lround(cx - dx * r * 2.2)), a.x, a.x + a.w - 1);
            const int sy = std::clamp(static_cast<int>(std::lround(cy - dy * r * 2.2)), a.y, a.y + a.h - 1);
            t.pixel(x, y, copia[static_cast<std::size_t>(sy - a.y) * a.w + (sx - a.x)]);
        }
    if (r >= 2.5) t.pixel(static_cast<int>(cx - r * 0.35), static_cast<int>(cy - r * 0.45), SCHIUMA);
}
}  // namespace

void acqua(Tela& t, const ParametriOculare& p, double luce, double tempo_s) {
    const Rettangolo a = area(p);
    for (int y = a.y; y < a.y + a.h; ++y)
        for (int x = a.x; x < a.x + a.w; ++x) t.pixel(x, y, pixel_acqua(a, x, y, luce, tempo_s));
}

namespace {
// L'acqua di una colonna caduta vicino (artiglieria, 22 §6.2): con molta acqua il velo che scola dall'alto e ondeggia la
// scena, sempre le gocce grosse che scivolano, piu' numerose quanto piu' la colonna era vicina
void spruzzi_colonne(Tela& t, const Rettangolo& a, const std::vector<std::uint8_t>& copia, double quanta, double tempo_s) {
    const double q = std::clamp(quanta, 0.0, 1.0);
    if (q > 0.5) {   // il velo: dall'alto fin dove arriva l'acqua
        const double yf = a.y + a.h * std::min(1.0, (q - 0.5) * 2.0);
        for (int y = a.y; y < static_cast<int>(yf); ++y)
            for (int x = a.x; x < a.x + a.w; ++x) {
                const int sx = std::clamp(x + static_cast<int>(std::lround(4.0 * std::sin(y * 0.12 + tempo_s * 14.0))), a.x, a.x + a.w - 1);
                const int sy = std::clamp(y - 3, a.y, a.y + a.h - 1);
                t.pixel(x, y, copia[static_cast<std::size_t>(sy - a.y) * a.w + (sx - a.x)]);
            }
    }
    const int n = static_cast<int>(std::lround(60.0 * q)), ondata = static_cast<int>(std::floor(tempo_s / 3.0));
    for (int i = 0; i < n; ++i) {
        const double eta = std::fmod(tempo_s, 3.0) + 3.0 * hash(i, ondata, 141);
        const double r = 3.0 + 7.0 * hash(i, ondata, 143);
        goccia(t, a, copia, a.x + a.w * hash(i, ondata, 145), a.y + a.h * hash(i, ondata, 147) + 8.0 * eta * eta * r / 6.0, r);
    }
}
}  // namespace

double acqua_sulla_finestra(Tela& t, const ParametriOculare& p, double hs_m, double luce, double tempo_s) {
    if (p.spruzzi_proietti > 0.0 && (p.altezza_testa_m < 0.0 || hs_m <= 0.0)) {
        // senza mare noto o col mare calmo: solo l'acqua delle colonne vicine (sotto)
        const Rettangolo a = area(p);
        std::vector<std::uint8_t> copia(static_cast<std::size_t>(a.w) * a.h);
        for (int y = 0; y < a.h; ++y)
            for (int x = 0; x < a.w; ++x) copia[static_cast<std::size_t>(y) * a.w + x] = t.at(a.x + x, a.y + y);
        spruzzi_colonne(t, a, copia, p.spruzzi_proietti, tempo_s);
        return 0.0;
    }
    if (p.altezza_testa_m < 0.0 || (hs_m <= 0.0 && p.pioggia <= 0.0)) return 0.0;
    const Rettangolo a = area(p);
    // acqua sopra il centro della finestra; copertura 0 (sotto il bordo basso) - 1 (sopra il bordo alto)
    auto livello = [&](double tt) { return elevazione_onda_m(hs_m, tt) - p.altezza_testa_m; };
    auto copertura = [&](double tt) { return std::clamp((livello(tt) + FINESTRA_M) / (2.0 * FINESTRA_M), 0.0, 1.0); };
    const double c = copertura(tempo_s);
    if (c >= 1.0) {
        acqua(t, p, luce, tempo_s);
        return 1.0;
    }
    std::vector<std::uint8_t> copia(static_cast<std::size_t>(a.w) * a.h);
    for (int y = 0; y < a.h; ++y)
        for (int x = 0; x < a.w; ++x) copia[static_cast<std::size_t>(y) * a.w + x] = t.at(a.x + x, a.y + y);

    if (c > 0.0) {
        // l'acqua sale dal basso: profilo mosso che corre, orlo di schiuma, sotto l'acqua torbida
        const double y0 = a.y + a.h * (1.0 - c);
        for (int x = a.x; x < a.x + a.w; ++x) {
            const double yl = y0 + 7.0 * std::sin(x * 0.043 + tempo_s * 3.1) + 3.0 * std::sin(x * 0.11 - tempo_s * 5.3) +
                              2.0 * (frattale(x * 0.05, tempo_s * 2.0, 91) - 0.5);
            // il fianco dell'onda vicina: chiaro in cresta, piu' scuro in basso, con le strisce di schiuma; sotto il
            // fianco (gia' sul vetro) l'acqua torbida
            const double l = 0.35 + 0.65 * std::clamp(luce, 0.0, 1.0);
            for (int y = std::max(a.y, static_cast<int>(std::floor(yl)) - 2); y < a.y + a.h; ++y) {
                if (y < yl) { if (hash(x, y, static_cast<int>(tempo_s * 15)) < 0.6) t.pixel(x, y, SCHIUMA); continue; }
                const double giu = y - yl;
                if (giu < 2.5 && hash(x, y, 97) < 0.6) { t.pixel(x, y, SCHIUMA); continue; }
                if (giu > 0.45 * a.h * c + 10) { t.pixel(x, y, pixel_acqua(a, x, y, luce, tempo_s)); continue; }
                const double striscia = frattale(x * 0.03 + giu * 0.01, giu * 0.12 - tempo_s * 1.5, 99);
                if (striscia > 0.72 && hash(x, y, 131) < 0.5) { t.pixel(x, y, SCHIUMA); continue; }
                const double i = colori::MARE_VICINO + l * (colori::MARE_ORIZZONTE + 6 - colori::MARE_VICINO) * std::exp(-giu / 40.0) +
                                 4.0 * (striscia - 0.5);
                const double b = std::floor(i);
                t.pixel(x, y, static_cast<std::uint8_t>(std::clamp(b + ((i - b) > soglia(x, y) ? 1.0 : 0.0), 96.0, 143.0)));
            }
        }
        return c;
    }

    // dall'ultima volta che l'acqua ha lasciato la finestra (fino a 8 s fa)
    double asciutta = 1e9, uscita = 0.0;
    for (int k = 1; k <= 160; ++k) {
        const double tt = tempo_s - 0.05 * k;
        if (copertura(tt) > 0.0) { asciutta = 0.05 * k; uscita = tt; break; }
    }
    if (asciutta < 0.7) {   // il velo d'acqua scola verso il basso e ondeggia la scena sotto il suo orlo
        const double yf = a.y + a.h * (asciutta / 0.7);
        for (int y = static_cast<int>(yf); y < a.y + a.h; ++y)
            for (int x = a.x; x < a.x + a.w; ++x) {
                const int sx = std::clamp(x + static_cast<int>(std::lround(3.0 * std::sin(y * 0.15 + tempo_s * 20.0))), a.x, a.x + a.w - 1);
                const int sy = std::clamp(y - 2, a.y, a.y + a.h - 1);
                t.pixel(x, y, copia[static_cast<std::size_t>(sy - a.y) * a.w + (sx - a.x)]);
            }
    }
    if (asciutta < 8.0) {   // gocce lasciate dall'onda: scivolano in giu' e spariscono (asciugano o colano via)
        const int evento = static_cast<int>(std::floor(uscita * 20.0));
        const int n = static_cast<int>(std::lround(40.0 * std::exp(-asciutta / 2.5)));
        for (int i = 0; i < n; ++i) {
            const double r = 2.5 + 5.5 * hash(i, evento, 101);
            const double v = (4.0 + 22.0 * hash(i, evento, 103)) * r / 7.0;   // le grosse scivolano piu' in fretta
            goccia(t, a, copia, a.x + a.w * hash(i, evento, 105), a.y + a.h * hash(i, evento, 107) + v * asciutta, r);
        }
    }
    if (p.spruzzi_proietti > 0.0) spruzzi_colonne(t, a, copia, p.spruzzi_proietti, tempo_s);
    if (p.pioggia > 0.0) {   // pioggia: ogni posto ha una goccia che vive 2 s
        const int n = static_cast<int>(std::lround(40.0 * std::clamp(p.pioggia, 0.0, 1.0)));
        for (int i = 0; i < n; ++i) {
            const double fase = 2.0 * hash(i, 0, 111), vita = (tempo_s + fase) / 2.0;
            const int nascita = static_cast<int>(std::floor(vita));
            const double eta = (vita - nascita) * 2.0;
            const double r = 1.5 + 3.0 * hash(i, nascita, 113);
            goccia(t, a, copia, a.x + a.w * hash(i, nascita, 115), a.y + a.h * hash(i, nascita, 117) + 6.0 * eta * eta, r);
        }
    }
    // spruzzi: la cresta vicina sotto la finestra, col mare da 4-5 in su (Hs oltre 1,25 m)
    const double vicino = std::clamp(1.0 - (-livello(tempo_s) - FINESTRA_M) / 0.6, 0.0, 1.0);
    if (hs_m >= 1.25 && vicino > 0.0) {
        const int raffica = static_cast<int>(std::floor(tempo_s * 4.0));
        const double f = tempo_s * 4.0 - raffica;
        const int n = static_cast<int>(std::lround(70.0 * vicino * std::min(1.0, hs_m / 3.0)));
        for (int i = 0; i < n; ++i) {
            const double x = a.x + a.w * hash(i, raffica, 121) + 120.0 * f * (hash(i, raffica, 123) - 0.3);
            const double y = a.y + a.h * (0.55 + 0.45 * hash(i, raffica, 125)) - 160.0 * f * hash(i, raffica, 127);
            const int l = 1 + static_cast<int>(3.0 * hash(i, raffica, 129));
            for (int k = 0; k < l; ++k) {
                const int px = static_cast<int>(x) + k, py = static_cast<int>(y) + k;
                if (px >= a.x && py >= a.y && px < a.x + a.w && py < a.y + a.h) t.pixel(px, py, SCHIUMA);
            }
        }
    }
    return 0.0;
}

double frazione_schiuma(double vento_m_s) {
    return std::min(0.2, 3.84e-6 * std::pow(std::max(vento_m_s, 0.0), 3.41));
}

double frazione_strisce(double vento_m_s) {
    return vento_m_s < 13.9 ? 0.0 : std::min(0.7, 0.04 + 0.022 * (vento_m_s - 13.9));
}

int ProfiloMare::riga(int x, double r) const {
    const int i = x - x0;
    if (i < 0 || i >= static_cast<int>(colonne.size())) return INT_MAX;
    int y = INT_MAX;
    for (const auto& [d, riga] : colonne[static_cast<std::size_t>(i)]) {   // poche voci, in ordine di distanza
        if (d >= r) break;
        y = riga;
    }
    return y;
}

namespace {
// soglia di un rumore frattale (media 0,5, scarto ~0,12) superata dalla frazione f della superficie
double soglia_frazione(double f) { return 0.5 + 0.12 * std::max(0.0, std::sqrt(-2.0 * std::log(std::max(f, 1e-9))) - 0.8); }
// frattale a due ottave (le maschere della schiuma, calcolate per pixel: la meta' del costo)
double frattale2(double x, double y, int seme) { return (0.5 * liscio(x, y, seme) + 0.25 * liscio(x * 2.03, y * 2.03, seme + 1)) / 0.75; }
}  // namespace

void mare(Tela& t, const ParametriOculare& p, double rilevamento_gradi, double ox, double oy, double h, double hs_m,
          double visibilita_m, double tempo_s, ProfiloMare* profilo) {
    if (profilo) {
        profilo->x0 = 0;
        profilo->colonne.clear();
    }
    if (hs_m <= 0.0) return;
    constexpr double G = 9.81;
    h = std::max(h, 0.15);
    const double ppr = pixel_per_radiante(p);
    const double dep = depressione_orizzonte(h);
    const double d_or = distanza_orizzonte(h);
    // vento: quello del mare formato (Hs = 0,0246 U^2) se non dato
    const double u = p.vento_kn >= 0.0 ? p.vento_kn * 0.5144 : std::sqrt(hs_m / 0.0246);
    const double tp = periodo_picco_s(hs_m);
    // componenti: quattro lunghe attorno al picco (direzioni sparse di +-35 gradi), quattro corte del vento
    struct Componente { double kx, ky, w, a, lambda, fase; };
    std::vector<Componente> comp;
    const double sigma = hs_m / 4.0;
    const double da = radianti(p.direzione_mare_gradi + 180.0);   // verso cui corrono
    const double rapporti[4] = {0.85, 1.0, 1.2, 1.5}, sparsi[4] = {-0.35, 0.1, 0.45, -0.15};
    for (int k = 0; k < 4; ++k) {
        const double w = rapporti[k] * 2.0 * PI / tp, kk = w * w / G, dir = da + sparsi[k];
        comp.push_back({kk * std::sin(dir), kk * std::cos(dir), w, sigma * std::sqrt(2.0 / 4.0) * (k == 1 ? 1.2 : 0.9),
                        2.0 * PI / kk, 1.7 * k});
    }
    const double corte[4] = {14.0, 8.0, 4.5, 2.5};   // m: onde del vento e increspature (STIMA)
    for (int k = 0; k < 4; ++k) {
        const double kk = 2.0 * PI / corte[k], w = std::sqrt(G * kk), dir = da + (k % 2 ? 0.5 : -0.4);
        comp.push_back({kk * std::sin(dir), kk * std::cos(dir), w, 0.02 * corte[k] * std::min(1.0, u / 8.0), corte[k], 2.3 * k + 0.5});
    }
    const double schiuma = frazione_schiuma(u), strisce = frazione_strisce(u);
    const double soglia_frange = soglia_frazione(schiuma), soglia_strisce = soglia_frazione(strisce);
    const double spruzzaglia = std::clamp((u - 17.2) / 10.0, 0.0, 1.0);   // da forza 8
    const double r_ril = radianti(rilevamento_gradi), el0 = radianti(p.elevazione_gradi);
    const double lungo_x = std::sin(da), lungo_y = std::cos(da);   // direzione del vento (le onde corrono con lui)
    const Rettangolo a = area(p);
    const int fotogramma = static_cast<int>(std::floor(tempo_s * 8.0));
    // il colore di un punto della superficie: cielo riflesso dalle facce verso l'occhio, foschia, schiuma
    auto nebbia = [&](double r) { return 1.0 - std::exp(-3.912 * r / std::max(visibilita_m, 1.0)); };
    auto colore = [&](int x, int y, double wx, double wy, double r, double foschia, double delta, double eta, double pend,
                      double base) {
        // faccia verso l'occhio (pendenza negativa lungo la vista): riflette il cielo piu' alto, chiara; le altre
        // mostrano l'acqua, scura. A pelo d'acqua bastano pendenze piccole; sopra l'occhio (muro d'acqua) niente cielo
        const double luce_faccia = delta > 0.0 ? std::clamp(-2.0 * pend / std::max(delta, 0.01), -1.0, 1.0)
                                               : std::clamp(-0.5 - pend, -1.0, -0.2);
        double i = base + 9.0 * luce_faccia;
        // da forza 10 la schiuma vecchia e la spruzzaglia sbiancano tutto il mare (il «mare bianco»)
        i += 22.0 * std::max(0.0, strisce - 0.25);
        i += foschia * (colori::MARE_ORIZZONTE - i);
        const double b = std::floor(i);
        std::uint8_t c = static_cast<std::uint8_t>(std::clamp(b + ((i - b) > soglia(x, y) ? 1.0 : 0.0), 96.0, 143.0));
        // la trama della schiuma, ferma nel mondo: un merletto fine da vicino; da lontano, sotto il pixel, un retino che
        // si spegne nella foschia
        auto trama = [&](double soglia_trama) {
            if (r < 150.0) return frattale2(wx * 1.3, wy * 1.3 - tempo_s * 0.05, 137) > soglia_trama;
            return hash(x / 2, y, fotogramma) > 0.3 + 0.6 * foschia;
        };
        // strisce di schiuma lungo il vento (da forza 7): lunghe, strette, ferme nel mondo
        if (strisce > 0.0) {
            const double lungo = wx * lungo_x + wy * lungo_y, attraverso = -wx * lungo_y + wy * lungo_x;
            if (frattale2(lungo * 0.012 - tempo_s * 0.01, attraverso * 0.35, 151) > soglia_strisce && trama(0.52 - 0.4 * strisce) &&
                hash(x, y, fotogramma / 4) > 0.3 * foschia)
                c = static_cast<std::uint8_t>(SCHIUMA - 2);
        }
        // schiuma sulle creste che frangono (chiazze ferme nel mondo che coprono la frazione W, piu' sulle creste, mai nei
        // cavi)
        if (schiuma > 1e-3 && eta > -0.3 * sigma &&
            frattale2(wx * 0.3, wy * 0.3 - tempo_s * 0.1, 133) + 0.02 * eta / sigma > soglia_frange && trama(0.45 - 0.5 * schiuma))
            // da vicino marmorizzata: schiuma fresca bianca e venature piu' grigie
            c = static_cast<std::uint8_t>(r < 150.0 && frattale2(wx * 3.1, wy * 3.1, 139) < 0.42 ? SCHIUMA - 3 : SCHIUMA);
        return c;
    };
    // 1. il mare vicino come campo d'altezze: colonna per colonna dal vicino al lontano, finche' l'onda e' piu' grande
    // di mezzo pixel; ogni campione copre le righe fra la sua e quella piu' alta gia' coperta (le creste nascondono il
    // mare dietro e, sopra l'occhio, l'orizzonte)
    // px_m: pixel per metro di traverso; scorcio: lungo la vista l'onda si accorcia del seno della depressione
    auto superficie = [&](double wx, double wy, double vx, double vy, double px_m, double scorcio, double& pend) {
        double eta = 0.0;
        pend = 0.0;
        for (const auto& c : comp) {
            const double peso = std::clamp((c.lambda * px_m * scorcio - 2.0) / 4.0, 0.0, 1.0);
            if (peso <= 0.0) continue;
            const double fase = c.kx * wx + c.ky * wy - c.w * tempo_s + c.fase;
            eta += peso * c.a * std::sin(fase);
            pend += peso * c.a * std::cos(fase) * (c.kx * vx + c.ky * vy);
        }
        return eta;
    };
    double pend0 = 0.0;
    const double occhio = h + superficie(ox, oy, 0.0, 1.0, 1e9, 1.0, pend0);   // quota assoluta dell'occhio sul mare medio
    const double r_lontano = std::min(d_or, ppr * hs_m * 0.5);   // oltre, l'onda e' sotto i due pixel
    if (profilo) {
        profilo->x0 = a.x;
        profilo->colonne.assign(static_cast<std::size_t>(a.w), {});
    }
    struct Spruzzo { int y0, y1; double r; };
    std::vector<int> coperto(static_cast<std::size_t>(a.w), INT_MAX);   // per colonna: da qui in giu' e' mare vicino
    per_righe(a.x, a.x + a.w, [&](int x) {
        int y_basso = -1, y_alto = INT_MAX;
        for (int y = a.y + a.h - 1; y >= a.y; --y)
            if (dentro(p, x, y)) { y_basso = y; break; }
        for (int y = a.y; y < a.y + a.h; ++y)
            if (dentro(p, x, y)) { y_alto = y; break; }
        if (y_basso < 0) return;
        const double alfa = std::atan((x + 0.5 - CENTRO_X) / ppr), ca = std::cos(alfa);
        const double az = r_ril + alfa, vx = std::sin(az), vy = std::cos(az);
        // da dove si comincia: a meta' strada dal punto del mare che l'occhio, alto h sul mare che ha davanti, vede sul
        // bordo basso dell'oculare
        const double el_basso = el0 + std::atan((CENTRO_Y - (y_basso + 1.0)) / ppr);
        double r = el_basso < -0.01 ? 0.5 * h / std::tan(-el_basso) / ca : 5.0;
        r = std::max(r, 1.0);
        int y_coperto = y_basso + 1;   // le righe da qui in giu' sono gia' mare
        std::vector<Spruzzo> spruzzi;
        auto* col = profilo ? &profilo->colonne[static_cast<std::size_t>(x - a.x)] : nullptr;
        double r_prec = r, y_prec = y_basso + 1.0, eta_prec = 0.0, pend_prec = 0.0, base_prec = 0.0, delta_prec = 0.0, fos_prec = 0.0;
        bool primo = true;
        while (r < r_lontano && y_coperto > y_alto) {
            const double r_asse = r * ca;
            const double wx = ox + r * vx, wy = oy + r * vy;
            const double px_m = ppr / r_asse;
            double pend = 0.0;
            const double eta = superficie(wx, wy, vx, vy, px_m, std::clamp(h / r_asse, 0.004, 1.0), pend);
            const double el = std::atan2(eta - occhio, r_asse) - r / (2.0 * RAGGIO_TERRA_RIFRAZIONE);
            const double yf = CENTRO_Y - ppr * std::tan(el - el0);
            const int y = static_cast<int>(std::floor(yf));
            const double f = std::clamp(-gradi(el) / 16.0, 0.0, 1.0);
            const double base = colori::MARE_ORIZZONTE - std::sqrt(f) * (colori::MARE_ORIZZONTE - colori::MARE_VICINO);
            const double delta = std::atan2(occhio - eta, r_asse), fos = nebbia(r);
            if (y < y_coperto) {
                // le righe fra questo campione e il precedente: il punto della superficie interpolato, non un blocco
                for (int yy = std::max(y, y_alto); yy < y_coperto; ++yy) {
                    if (!dentro(p, x, yy)) continue;
                    const double q = primo || y_prec - yf < 1e-6 ? 1.0 : std::clamp((y_prec - (yy + 0.5)) / (y_prec - yf), 0.0, 1.0);
                    auto mix = [q](double a0, double a1) { return a0 + q * (a1 - a0); };
                    const double ri = mix(r_prec, r);
                    t.pixel(x, yy, colore(x, yy, ox + ri * vx, oy + ri * vy, ri, mix(fos_prec, fos), mix(delta_prec, delta),
                                          mix(eta_prec, eta), mix(pend_prec, pend), mix(base_prec, base)));
                }
                // spruzzaglia strappata dalla cresta (da forza 8): il vento la porta su di uno-due metri
                if (spruzzaglia > 0.0 && eta > 0.8 * sigma && r > 30.0 && r < 3000.0) {
                    const int alto = std::min(12, static_cast<int>(std::ceil(px_m * (0.6 + 1.6 * spruzzaglia) * std::min(1.0, eta / hs_m + 0.3))));
                    if (alto > 0) spruzzi.push_back({std::max(y - alto, y_alto), y, r});
                }
                y_coperto = std::max(y, y_alto);
                if (col) col->emplace_back(static_cast<float>(r), static_cast<short>(y_coperto));
            }
            r_prec = r;
            y_prec = yf;
            eta_prec = eta;
            pend_prec = pend;
            base_prec = base;
            delta_prec = delta;
            fos_prec = fos;
            primo = false;
            // passo: mezzo pixel di quota alla distanza r, almeno l'1% della distanza
            r += std::max(0.7 * r * r_asse / (ppr * std::max(occhio + 0.5 * hs_m, 0.5)), 0.03 * r) + 0.02;
        }
        coperto[static_cast<std::size_t>(x - a.x)] = y_coperto;
        // la spruzzaglia sta davanti al mare piu' lontano: retinata sopra le creste, che corre col vento
        for (const auto& s : spruzzi) {
            const double foschia = nebbia(s.r);
            for (int yy = s.y0; yy < s.y1; ++yy) {
                const double densita = spruzzaglia * 0.35 * (1.0 - foschia) * (yy - s.y0 + 1.0) / (s.y1 - s.y0 + 1.0);
                if (dentro(p, x, yy) && hash(x + fotogramma * 3, yy, fotogramma) < densita) t.pixel(x, yy, static_cast<std::uint8_t>(SCHIUMA - 1));
            }
        }
    });
    // 2. il mare lontano per righe (onde sotto il pixel in verticale), solo sopra il mare vicino gia' disegnato
    per_righe(a.y, a.y + a.h, [&](int y) {
        const double el = el0 + std::atan((CENTRO_Y - (y + 0.5)) / ppr);
        const double delta = -el;
        if (delta <= dep * 1.02) return;
        const double r_asse = std::min(h / std::tan(delta - dep * 0.98), d_or);
        // dimensione verticale di un metro d'onda a quella distanza (pixel), per spegnere le onde sotto il pixel
        const double px_m_v = ppr * delta / r_asse, px_m_o = ppr / r_asse;
        const double f = std::clamp(-gradi(el) / 16.0, 0.0, 1.0);
        const double base = colori::MARE_ORIZZONTE - std::sqrt(f) * (colori::MARE_ORIZZONTE - colori::MARE_VICINO);
        double peso[8];
        for (std::size_t k = 0; k < comp.size(); ++k) {
            const double dim = std::min(comp[k].lambda * px_m_v, comp[k].lambda * px_m_o);
            peso[k] = std::clamp((dim - 2.0) / 4.0, 0.0, 1.0);
        }
        for (int x = a.x; x < a.x + a.w; ++x) {
            if (y >= coperto[static_cast<std::size_t>(x - a.x)] || !dentro(p, x, y)) continue;
            const double alfa = std::atan((x + 0.5 - CENTRO_X) / ppr);
            const double r = r_asse / std::cos(alfa);
            const double az = r_ril + alfa;
            const double wx = ox + r * std::sin(az), wy = oy + r * std::cos(az);
            const double vx = std::sin(az), vy = std::cos(az);
            double pend = 0.0, eta = 0.0;
            for (std::size_t k = 0; k < comp.size(); ++k) {
                if (peso[k] <= 0.0) continue;
                const double fase = comp[k].kx * wx + comp[k].ky * wy - comp[k].w * tempo_s + comp[k].fase;
                eta += peso[k] * comp[k].a * std::sin(fase);
                pend += peso[k] * comp[k].a * std::cos(fase) * (comp[k].kx * vx + comp[k].ky * vy);
            }
            t.pixel(x, y, colore(x, y, wx, wy, r, nebbia(r), delta, eta, pend, base));
        }
    });
}

namespace {
// la banchisa: la luminosita' del ghiaccio (0 ombra fonda, 1 neve al sole) retinata sui grigi degli sprite 148-158
// (presi dalla luce: la notte li scurisce); il nilas sui grigi scuri
constexpr int GHIACCIO_SCURO = colori::GRIGI + 4, GHIACCIO_CHIARO = colori::GRIGI + 14;

double u01_cella(std::int64_t i, std::int64_t j, std::uint64_t sale) {
    std::uint64_t z = static_cast<std::uint64_t>(i) * 0x9E3779B97F4A7C15ull ^ static_cast<std::uint64_t>(j) * 0xC2B2AE3D27D4EB4Full ^ sale;
    z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ull;
    z = (z ^ (z >> 27)) * 0x94D049BB133111EBull;
    return ((z ^ (z >> 31)) >> 11) * (1.0 / 9007199254740992.0);
}

// le creste di pressione: una linea per cella, lunga fino a 1,2 celle (passa nelle vicine, con le punte che calano),
// a blocchi, con le falde a ~27 gradi; alta quanto la vela del campo nel suo centro. Vale la piu' alta delle nove celle
// attorno al punto (q nel ghiaccio, che deriva); `vela_in(x, y)` la vela nel punto del mondo. lume < 0: nessuna cresta.
struct Cresta { double z = 0.0, lume = -1.0; };
template <class F>
Cresta creste(const GhiaccioVista& g, double qx, double qy, double vela_qui, F vela_in, double sole_x, double sole_y, bool blocchi) {
    const double cel = g.cella_m;
    const auto ci0 = static_cast<std::int64_t>(std::floor(qx / cel)), cj0 = static_cast<std::int64_t>(std::floor(qy / cel));
    Cresta out;
    for (std::int64_t ci = ci0 - 1; ci <= ci0 + 1; ++ci)
        for (std::int64_t cj = cj0 - 1; cj <= cj0 + 1; ++cj) {
            const double cx = (ci + 0.3 + 0.4 * u01_cella(ci, cj, 2)) * cel, cy = (cj + 0.3 + 0.4 * u01_cella(ci, cj, 3)) * cel;
            const double ang = PI * u01_cella(ci, cj, 1), ux = std::cos(ang), uy = std::sin(ang);
            const double lungo = (qx - cx) * ux + (qy - cy) * uy, attraverso = -(qx - cx) * uy + (qy - cy) * ux;
            const double meta = 0.6 * cel;
            if (std::abs(lungo) >= meta || std::abs(attraverso) > 14.0) continue;   // le falde: 2 x vela + 2 m, vele fino a 5 m
            const double vela = (ci == ci0 && cj == cj0) ? vela_qui : vela_in(cx + g.deriva_m.x, cy + g.deriva_m.y);
            if (vela <= 0.0) continue;
            const double punta = 1.0 - std::pow(std::abs(lungo) / meta, 4.0);
            const double alta = vela * punta * (0.55 + 0.45 * frattale2(lungo * 0.08, static_cast<double>(ci * 7 + cj * 3), 177));
            const double falda = 2.0 * alta + 2.0;
            const double d = std::abs(attraverso) + 1.2 * (frattale2(lungo * 0.4, attraverso * 0.4, 179) - 0.5);
            if (d >= falda || alta * (1.0 - d / falda) <= out.z) continue;
            out.z = alta * (1.0 - d / falda);
            // la falda verso il sole e' chiara, l'altra in ombra; i blocchi rotti a chiazze, ognuno col suo lume
            const double verso = (attraverso >= 0.0 ? 1.0 : -1.0) * (-uy * sole_x + ux * sole_y);
            out.lume = 0.55 + 0.45 * verso;
            const double blocco = frattale2(lungo * 0.9, attraverso * 0.9, 183);
            if (blocchi && blocco > 0.6) out.lume = 0.15 + 0.3 * blocco;
        }
    return out;
}
}  // namespace

double quota_cresta(const GhiaccioVista& g, Vec2 p) {
    if (!g.campione) return 0.0;
    auto vela = [&](double x, double y) { return g.campione({x, y}).cresta_m; };
    return creste(g, p.x - g.deriva_m.x, p.y - g.deriva_m.y, vela(p.x, p.y), vela, 0.0, 1.0, false).z;
}

void banchisa(Tela& t, const ParametriOculare& p, double rilevamento_gradi, double ox, double oy, double h,
              double visibilita_m, double tempo_s, const GhiaccioVista& g, ProfiloMare* profilo) {
    (void)tempo_s;   // il pack e' fermo nella vista: la deriva arriva gia' nel campione e in `deriva_m`
    if (!g.attivo || !g.campione) return;
    h = std::max(h, 0.15);
    const double ppr = pixel_per_radiante(p);
    const double r_ril = radianti(rilevamento_gradi), el0 = radianti(p.elevazione_gradi);
    const bool nel_pack = g.copertura >= 0.3;
    // fin dove: l'orizzonte delle creste piu' alte (3 m), entro tre volte la visibilita'
    const double r_max = std::min(distanza_orizzonte(h) + distanza_orizzonte(3.0), 3.0 * std::max(visibilita_m, 500.0));
    const Rettangolo a = area(p);
    // il campione del campo, una volta per quadrato di 10 m: il campo ha celle di 200 m
    std::unordered_map<std::uint64_t, CampioneGhiaccio> memo;
    auto campione = [&](double wx, double wy) -> const CampioneGhiaccio& {
        const auto i = static_cast<std::int64_t>(std::floor(wx / 10.0)), j = static_cast<std::int64_t>(std::floor(wy / 10.0));
        const std::uint64_t k = (static_cast<std::uint64_t>(i) << 32) ^ (static_cast<std::uint64_t>(j) & 0xffffffffull);
        if (auto it = memo.find(k); it != memo.end()) return it->second;
        return memo.emplace(k, g.campione({(i + 0.5) * 10.0, (j + 0.5) * 10.0})).first->second;
    };
    // il sole basso da sud-sudovest (STIMA: la vista non sa l'ora), per le falde delle creste
    const double sole_x = std::sin(radianti(200.0)), sole_y = std::cos(radianti(200.0));
    struct Punto { double z = 0.0, lume = -1.0; };   // lume < 0: acqua
    // la quota e la luminosita' della superficie nel punto del mondo
    // r: la distanza, per spegnere i dettagli piu' piccoli di due pixel (le gobbe di scorcio, i blocchi di traverso)
    auto superficie = [&](double wx, double wy, double r) {
        const CampioneGhiaccio& c = campione(wx, wy);
        Punto s;
        const double qx = wx - g.deriva_m.x, qy = wy - g.deriva_m.y;   // nel ghiaccio, che deriva
        if (!c.ghiaccio) {   // nell'acqua del pack, rari pezzi di lastra alla deriva (qualche metro)
            if (nel_pack && liscio(qx * 0.12, qy * 0.12, 189) > 0.86) return Punto{0.08, 0.7 + 0.2 * liscio(qx * 0.5, qy * 0.5, 191)};
            return s;
        }
        if (c.nilas) return Punto{0.01, 0.12 + 0.08 * liscio(qx * 0.02, qy * 0.02, 185)};
        // le fratture fra le lastre: linee di livello di un rumore, larghe ~8 m, a tratti
        if (std::abs(frattale2(qx * 0.003, qy * 0.003, 171) - 0.5) < 0.003 && liscio(qx * 0.004, qy * 0.004, 181) > 0.45) return s;
        // la lastra: bordo libero di un decimo dello spessore, la neve a gobbe ombreggiate dal sole; ogni lastra ha il suo
        // tono (neve fresca, vecchia, ghiaccio nudo)
        const double bordo_libero = std::clamp(0.1 * c.spessore_m, 0.05, 0.4);
        const double n0 = frattale2(qx * 0.08, qy * 0.08, 173), n1 = frattale2((qx + 2.0 * sole_x) * 0.08, (qy + 2.0 * sole_y) * 0.08, 173);
        const double gobbe = std::clamp((12.0 * ppr * h / (r * r) - 2.0) / 4.0, 0.0, 1.0);
        s.z = bordo_libero + 0.25 * (n0 - 0.5);
        s.lume = 0.62 + 0.3 * liscio(qx * 0.012, qy * 0.012, 187) + gobbe * 4.0 * (n1 - n0);
        // le creste delle nove celle attorno (la piu' alta), con la falda al sole e i blocchi
        const Cresta cr = creste(g, qx, qy, c.cresta_m, [&](double x, double y) { return campione(x, y).cresta_m; }, sole_x, sole_y,
                                 1.1 * ppr / r > 2.0);
        s.z += cr.z;
        if (cr.lume >= 0.0) s.lume = cr.lume;
        return s;
    };
    auto nebbia = [&](double r) { return 1.0 - std::exp(-3.912 * r / std::max(visibilita_m, 1.0)); };
    // il colore di un pixel: il ghiaccio lontano schiarisce nella foschia (retinato); l'acqua calma dei canali e' scura
    // vicino e riflette il cielo verso l'orizzonte, come il mare senza onde
    auto colore = [&](int x, int y, const Punto& s, double r, double el, bool bordo) -> std::uint8_t {
        const double fos = nebbia(r);
        if (s.lume < 0.0) {
            const double f = std::clamp(-gradi(el) / 16.0, 0.0, 1.0);
            double i = colori::MARE_ORIZZONTE - std::sqrt(f) * (colori::MARE_ORIZZONTE - colori::MARE_VICINO) - 2.0;
            i += fos * (colori::MARE_ORIZZONTE - i);
            const double b = std::floor(i);
            return static_cast<std::uint8_t>(std::clamp(b + ((i - b) > soglia(x, y) ? 1.0 : 0.0), 96.0, 143.0));
        }
        double l = bordo ? std::min(s.lume, 0.45) : s.lume;   // il bordo della lastra verso l'occhio, in ombra
        l += fos * (0.85 - l);                                  // la foschia: il grigio chiaro lontano
        const double i = GHIACCIO_SCURO + std::clamp(l, 0.0, 1.0) * (GHIACCIO_CHIARO - GHIACCIO_SCURO);
        const double b = std::floor(i);
        return static_cast<std::uint8_t>(std::min<double>(GHIACCIO_CHIARO, b + ((i - b) > soglia(x, y) ? 1.0 : 0.0)));
    };
    // la superficie sotto le nubi per il riflesso: la frazione di ghiaccio (il nilas conta poco) su una griglia di
    // un chilometro (quattro campioni per nodo), interpolata
    const double nubi = std::clamp(g.nubi, 0.0, 1.0), plafond = std::max(g.plafond_m, 150.0);
    std::unordered_map<std::uint64_t, double> nodi;
    auto nodo = [&](std::int64_t i, std::int64_t j) {
        const std::uint64_t k = (static_cast<std::uint64_t>(i) << 32) ^ (static_cast<std::uint64_t>(j) & 0xffffffffull);
        if (auto it = nodi.find(k); it != nodi.end()) return it->second;
        double somma = 0.0;
        for (const double u : {-250.0, 250.0})
            for (const double v : {-250.0, 250.0}) {
                const CampioneGhiaccio c = g.campione({i * 1000.0 + u, j * 1000.0 + v});
                somma += c.ghiaccio ? (c.nilas ? 0.3 : 1.0) : 0.0;
            }
        return nodi.emplace(k, somma / 4.0).first->second;
    };
    auto albedo = [&](double wx, double wy) {
        const double fx = wx / 1000.0, fy = wy / 1000.0;
        const auto i = static_cast<std::int64_t>(std::floor(fx)), j = static_cast<std::int64_t>(std::floor(fy));
        const double tx = fx - i, ty = fy - j;
        return (1 - ty) * ((1 - tx) * nodo(i, j) + tx * nodo(i + 1, j)) + ty * ((1 - tx) * nodo(i, j + 1) + tx * nodo(i + 1, j + 1));
    };
    // per non disegnare sopra le creste del mare mosso piu' vicine (fuori dal pack)
    const ProfiloMare mare_vicino = profilo ? *profilo : ProfiloMare{};
    std::vector<std::vector<std::pair<float, short>>> coperture(static_cast<std::size_t>(a.w));
    for (int x = a.x; x < a.x + a.w; ++x) {
        int y_basso = -1, y_alto = INT_MAX;
        for (int y = a.y + a.h - 1; y >= a.y; --y)
            if (dentro(p, x, y)) { y_basso = y; break; }
        for (int y = a.y; y < a.y + a.h; ++y)
            if (dentro(p, x, y)) { y_alto = y; break; }
        if (y_basso < 0) continue;
        const double alfa = std::atan((x + 0.5 - CENTRO_X) / ppr), ca = std::cos(alfa);
        const double az = r_ril + alfa, vx = std::sin(az), vy = std::cos(az);
        const double el_basso = el0 + std::atan((CENTRO_Y - (y_basso + 1.0)) / ppr);
        double r = std::max(el_basso < -0.01 ? 0.5 * h / std::tan(-el_basso) / ca : 5.0, 1.0);
        int y_coperto = y_basso + 1;
        double r_prec = r, y_prec = y_basso + 1.0;
        bool primo = true, acqua_prec = true;
        auto& cop = coperture[static_cast<std::size_t>(x - a.x)];
        int lontani = 0, lontani_ghiaccio = 0;   // i campioni oltre il chilometro: il riflesso del ghiaccio nel cielo
        while (r < r_max && y_coperto > y_alto) {
            const double r_asse = r * ca;
            const Punto s = superficie(ox + r * vx, oy + r * vy, r);
            const double el = std::atan2(s.z - h, r_asse) - r / (2.0 * RAGGIO_TERRA_RIFRAZIONE);
            const double yf = CENTRO_Y - ppr * std::tan(el - el0);
            const int y = static_cast<int>(std::floor(yf));
            const bool acqua = s.lume < 0.0;
            if (r > 1000.0) ++lontani, lontani_ghiaccio += acqua ? 0 : 1;
            // l'acqua fuori dal pack resta il mare mosso; dentro, calma
            if (y < y_coperto && (!acqua || nel_pack)) {
                const bool bordo = !acqua && acqua_prec && !primo;   // il bordo della lastra verso l'occhio
                for (int yy = std::max(y, y_alto); yy < y_coperto; ++yy) {
                    if (!dentro(p, x, yy)) continue;
                    const double q = primo || y_prec - yf < 1e-6 ? 1.0 : std::clamp((y_prec - (yy + 0.5)) / (y_prec - yf), 0.0, 1.0);
                    const double ri = r_prec + q * (r - r_prec);
                    if (!nel_pack && yy >= mare_vicino.riga(x, ri)) continue;   // dietro una cresta d'onda piu' vicina
                    t.pixel(x, yy, colore(x, yy, s, ri, el, bordo && yy == y_coperto - 1));
                }
                if (!acqua) cop.emplace_back(static_cast<float>(r), static_cast<short>(std::max(y, y_alto)));
            }
            if (y < y_coperto) y_coperto = std::max(y, y_alto);   // anche l'acqua lasciata al mare mosso prende le sue righe
            r_prec = r;
            y_prec = yf;
            acqua_prec = acqua;
            primo = false;
            // passo: mezzo pixel di quota alla distanza r (per le creste di 3 m), almeno l'1,5% della distanza
            r += std::max(0.7 * r * r_asse / (ppr * std::max(h + 1.5, 0.5)), 0.015 * r) + 0.05;
        }
        // il riflesso del ghiaccio (iceblink) col cielo sereno: sopra la banchisa lontana, visto dal mare aperto, il
        // cielo basso si schiarisce di un bianco retinato per un grado e mezzo; debole senza nubi (STIMA), dentro il
        // pack uniforme e non si disegna
        if (!nel_pack && lontani > 0 && lontani_ghiaccio * 3 > lontani) {
            const double forza = (0.15 + 0.3 * nubi) * lontani_ghiaccio / lontani;
            const int n = std::max(2, static_cast<int>(ppr * radianti(1.5)));
            for (int k = 1; k <= n; ++k) {
                const int yy = y_coperto - k;
                if (yy < y_alto || !dentro(p, x, yy)) break;
                if (soglia(x, yy) < forza * (1.0 - static_cast<double>(k) / n)) t.pixel(x, yy, static_cast<std::uint8_t>(GHIACCIO_CHIARO - 1));
            }
        }
        // con le nubi: la loro base riflette la superficie che le sta sotto, chiara sopra il ghiaccio (il riflesso) e
        // scura sopra l'acqua (il «cielo d'acqua» dei canali, delle polynye e del mare aperto oltre il margine), anche
        // oltre l'orizzonte; i grigi delle nubi gia' disegnate salgono fino a cinque passi sopra il ghiaccio e scendono di
        // uno sopra l'acqua (STIMA)
        if (nubi > 0.05)
            for (int yy = y_alto; yy < y_coperto; ++yy) {
                if (!dentro(p, x, yy)) continue;
                const int c0 = t.at(x, yy);
                if (c0 < colori::GRIGI || c0 >= GHIACCIO_CHIARO) continue;   // solo le nubi, nel cielo
                const double el = el0 + std::atan((CENTRO_Y - (yy + 0.5)) / ppr);
                if (el <= 0.0005) continue;
                const double d = std::min((plafond - h) / std::tan(el), 60000.0);
                const double i = c0 + nubi * std::exp(-d / 40000.0) * (6.0 * albedo(ox + d * vx, oy + d * vy) - 1.0);
                const double b = std::floor(i);
                t.pixel(x, yy, static_cast<std::uint8_t>(std::clamp<double>(b + ((i - b) > soglia(x, yy) ? 1.0 : 0.0), colori::GRIGI, GHIACCIO_CHIARO)));
            }
    }
    // il profilo: per ogni distanza la riga piu' alta coperta dal mare o dal ghiaccio piu' vicini
    if (!profilo) return;
    if (profilo->colonne.empty()) {
        profilo->x0 = a.x;
        profilo->colonne.assign(static_cast<std::size_t>(a.w), {});
    }
    for (std::size_t i = 0; i < coperture.size() && i < profilo->colonne.size(); ++i) {
        auto& col = profilo->colonne[i];
        if (coperture[i].empty()) continue;
        col.insert(col.end(), coperture[i].begin(), coperture[i].end());
        std::stable_sort(col.begin(), col.end(), [](const auto& u, const auto& v) { return u.first < v.first; });
        short minimo = SHRT_MAX;
        std::vector<std::pair<float, short>> unite;
        for (const auto& [d, y] : col)
            if (y < minimo) { minimo = y; unite.emplace_back(d, y); }
        col.swap(unite);
    }
}

namespace {
// la terraferma: bosco coi bruni del fango e della fuliggine degli effetti (196-198, presi dalla luce), roccia e neve sui
// grigi degli sprite; il chiaro della foschia lontana e' quello di cielo::foschia (157), cosi' la terra dentro la
// foschia del meteo ci sparisce
constexpr int BOSCO_SCURO = 198, BOSCO_CHIARO = 196, ROCCIA_SCURA = colori::GRIGI + 5, ROCCIA_CHIARA = colori::GRIGI + 10;
constexpr int NEVE_SCURA = colori::GRIGI + 12, NEVE_CHIARA = colori::GRIGI + 14, LONTANANZA = colori::GRIGI + 13;
constexpr double QUOTA_NEVE_M = 450.0;   // sopra, a chiazze, la neve (STIMA per i teatri del nord)
}  // namespace

void costa(Tela& t, const ParametriOculare& p, double rilevamento_gradi, double ox, double oy, double h, double visibilita_m,
           double luce, const CostaVista& c, ProfiloMare* profilo) {
    (void)luce;   // la notte la fa la palette (i bruni e i grigi sono in INDICI_VISTA)
    if (!c.attiva || !c.quota) return;
    h = std::max(h, 0.15);
    const double ppr = pixel_per_radiante(p);
    const double r_ril = radianti(rilevamento_gradi), el0 = radianti(p.elevazione_gradi);
    // fin dove: tre volte la visibilita', al piu' 80 km (le montagne oltre l'orizzonte del mare)
    const double r_max = std::min(3.0 * std::max(visibilita_m, 500.0), 80000.0);
    const double passo_min = std::max(c.cella_m * 0.25, 10.0);
    const double sole_x = std::sin(radianti(200.0)), sole_y = std::cos(radianti(200.0));
    auto nebbia = [&](double r) { return 1.0 - std::exp(-3.912 * r / std::max(visibilita_m, 1.0)); };
    auto riga = [&](double el) { return CENTRO_Y - ppr * std::tan(el - el0); };
    const Rettangolo a = area(p);
    std::vector<std::vector<std::pair<float, short>>> coperture(static_cast<std::size_t>(a.w));
    for (int x = a.x; x < a.x + a.w; ++x) {
        int y_basso = -1, y_alto = INT_MAX;
        for (int y = a.y + a.h - 1; y >= a.y; --y)
            if (dentro(p, x, y)) { y_basso = y; break; }
        for (int y = a.y; y < a.y + a.h; ++y)
            if (dentro(p, x, y)) { y_alto = y; break; }
        if (y_basso < 0) continue;
        const double alfa = std::atan((x + 0.5 - CENTRO_X) / ppr), ca = std::cos(alfa);
        const double az = r_ril + alfa, vx = std::sin(az), vy = std::cos(az);
        int y_cima = INT_MAX;   // la riga piu' alta coperta dalla terra piu' vicina (INT_MAX: ancora niente terra)
        bool in_terra = false;
        auto& cop = coperture[static_cast<std::size_t>(x - a.x)];
        for (double r = 20.0; r < r_max; r += std::max(passo_min, 0.006 * r)) {
            const double wx = ox + r * vx, wy = oy + r * vy, r_asse = r * ca;
            const double q = c.quota({wx, wy});
            if (!(q > 0.0)) { in_terra = false; continue; }
            const double curva = r / (2.0 * RAGGIO_TERRA_RIFRAZIONE);
            if (!in_terra) {   // la linea di costa: la terra comincia al livello del mare, con la risacca chiara
                const int y_mare = static_cast<int>(std::floor(riga(std::atan2(-h, r_asse) - curva)));
                if (y_cima == INT_MAX) y_cima = std::min(y_mare + 1, y_basso + 1);
                if (r < 8000.0 && y_mare < y_cima && y_mare >= y_alto && dentro(p, x, y_mare) &&
                    soglia(x, y_mare) < 0.7 * (1.0 - nebbia(r)))
                    t.pixel(x, y_mare, static_cast<std::uint8_t>(NEVE_CHIARA));
                in_terra = true;
            }
            const int y = static_cast<int>(std::floor(riga(std::atan2(q - h, r_asse) - curva)));
            if (y >= y_cima) continue;
            // il colore del pendio: l'ombreggiatura dal sole, il materiale dalla quota e da un rumore, la foschia
            const double d = std::max(c.cella_m * 0.5, 30.0);
            const double gx = (c.quota({wx + d, wy}) - c.quota({wx - d, wy})) / (2.0 * d);
            const double gy = (c.quota({wx, wy + d}) - c.quota({wx, wy - d})) / (2.0 * d);
            const double lume = std::clamp(0.55 - 1.2 * (gx * sole_x + gy * sole_y), 0.0, 1.0);
            const double n = frattale2(wx * 0.0015, wy * 0.0015, 241);
            int scuro = BOSCO_SCURO, chiaro = BOSCO_CHIARO;
            if (q > QUOTA_NEVE_M && n > 0.42) scuro = NEVE_SCURA, chiaro = NEVE_CHIARA;
            else if (n > 0.6 || q > 0.8 * QUOTA_NEVE_M) scuro = ROCCIA_SCURA, chiaro = ROCCIA_CHIARA;
            const double fos = std::min(1.0, 0.85 * nebbia(r) + 0.15 * std::min(1.0, r / 30000.0));
            for (int yy = std::max(y, y_alto); yy < y_cima; ++yy) {
                if (!dentro(p, x, yy)) continue;
                const double s0 = soglia(x, yy);
                int col = s0 < lume ? chiaro : scuro;
                if (soglia(x + 2, yy + 1) < fos) col = LONTANANZA;
                t.pixel(x, yy, static_cast<std::uint8_t>(col));
            }
            y_cima = std::max(y, y_alto);
            cop.emplace_back(static_cast<float>(r), static_cast<short>(y_cima));
        }
    }
    // il profilo: la terra copre le navi che le stanno dietro (come le creste del mare e del ghiaccio)
    if (!profilo) return;
    if (profilo->colonne.empty()) {
        profilo->x0 = a.x;
        profilo->colonne.assign(static_cast<std::size_t>(a.w), {});
    }
    for (std::size_t i = 0; i < coperture.size() && i < profilo->colonne.size(); ++i) {
        auto& col = profilo->colonne[i];
        if (coperture[i].empty()) continue;
        col.insert(col.end(), coperture[i].begin(), coperture[i].end());
        std::stable_sort(col.begin(), col.end(), [](const auto& u, const auto& v) { return u.first < v.first; });
        short minimo = SHRT_MAX;
        std::vector<std::pair<float, short>> unite;
        for (const auto& [dd, yy] : col)
            if (yy < minimo) { minimo = yy; unite.emplace_back(dd, yy); }
        col.swap(unite);
    }
}

void telecamera_verticale(Tela& t, const Rettangolo& r, double quota_m, double luce, double ghiaccio, double tempo_s) {
    // luce che arriva dall'alto, attenuata dall'acqua (STIMA: un fattore e ogni 40 m); il guadagno automatico della
    // telecamera ne recupera una parte e alza la grana
    const double l0 = std::min(1.0, 1.3 * std::max(luce, 0.0) + 0.02) * std::exp(-std::max(quota_m, 0.0) / 40.0);
    const double guadagno = std::pow(l0, 0.4), grana = 0.05 + 0.1 * (1.0 - l0);
    const double scala = 18.0 / std::max(quota_m, 3.0);   // piu' vicini, dettagli piu' grandi
    const int fotogramma = static_cast<int>(std::floor(tempo_s * 15.0));
    for (int y = 0; y < r.h; ++y)
        for (int x = 0; x < r.w; ++x) {
            const double u = x * scala * 0.02, w = y * scala * 0.02;
            double v;
            if (ghiaccio > 0.0) {
                // lastre (scure, spesse), canali e polinie (chiari), lastre sottili (grigie), creste di pressione
                const double n = frattale(u * 0.6, w * 0.6, 31);
                const double soglia_canale = 1.0 - 0.55 * ghiaccio;
                if (n > soglia_canale) v = 0.85;                                     // acqua libera
                else if (n > soglia_canale - 0.08) v = 0.5;                          // ghiaccio giovane
                else v = 0.12 + 0.1 * frattale(u * 3.0, w * 3.0, 47);                // pack
                const double cresta = std::fabs(frattale(u * 1.4, w * 1.4, 53) - 0.5);
                if (cresta < 0.012 && n <= soglia_canale) v *= 0.5;
            } else {
                // superficie mossa: la rete delle caustiche, dove due campi d'onda si incrociano, che scorre
                const double c = std::fabs(frattale(u * 1.5, w * 1.5 + tempo_s * 0.15, 61) -
                                           frattale(u * 1.5 + 7.3, w * 1.5 - tempo_s * 0.12, 67));
                v = 0.5 + 0.4 * std::pow(std::max(0.0, 1.0 - 9.0 * c), 3.0) + 0.1 * frattale(u * 3, w * 3, 71);
            }
            v *= guadagno;
            v += grana * (hash(x, y, fotogramma) + hash(x + 5, y + 9, fotogramma) - 1.0) * 1.7;
            if (y & 1) v *= 0.8;
            t.pixel(r.x + x, r.y + y, rampa(v, r.x + x, r.y + y));
        }
    // croce elettronica al centro, corta
    const std::uint8_t c = static_cast<std::uint8_t>(RAMPA + N_RAMPA - 2);
    const int cx = r.x + r.w / 2, cy = r.y + r.h / 2;
    for (int s = 6; s <= 16; ++s) { t.pixel(cx - s, cy, c); t.pixel(cx + s, cy, c); t.pixel(cx, cy - s, c); t.pixel(cx, cy + s, c); }
}

}  // namespace m688::oculare

namespace m688::oculare {

void guasto(Tela& t, const ParametriOculare& p, double luce, double tempo_s) {
    if (p.spento <= 0.0 && p.incrinato <= 0.0) return;
    const Rettangolo a = area(p);
    const double cx = a.x + a.w / 2.0, cy = a.y + a.h / 2.0, rr = std::min(a.w, a.h) / 2.0;
    const auto dentro_area = [&](int x, int y) { return (x - cx) * (x - cx) + (y - cy) * (y - cy) <= rr * rr; };
    // il vetro incrinato: dal punto colpito (dal seme) 7-11 crepe a zigzag e qualche anello spezzato; sotto, l'acqua
    // torbida che sale dal basso quanto piu' il vetro e' andato
    if (p.incrinato > 0.0) {
        const double ix = cx + 0.5 * rr * (hash(static_cast<int>(p.seme_colpo), 1, 151) - 0.5);
        const double iy = cy + 0.5 * rr * (hash(static_cast<int>(p.seme_colpo), 2, 153) - 0.5);
        const double livello = a.y + a.h * (1.0 - 0.6 * std::clamp(p.incrinato, 0.0, 1.0));
        for (int y = static_cast<int>(livello); y < a.y + a.h; ++y)
            for (int x = a.x; x < a.x + a.w; ++x)
                if (dentro_area(x, y) && y > livello + 3.0 * std::sin(x * 0.07 + tempo_s * 2.0)) t.pixel(x, y, pixel_acqua(a, x, y, luce, tempo_s));
        const std::uint8_t crepa = static_cast<std::uint8_t>(colori::GRIGI + 13), ombra = static_cast<std::uint8_t>(colori::GRIGI + 2);
        const int n = 7 + static_cast<int>(4.0 * hash(static_cast<int>(p.seme_colpo), 3, 155));
        const double lungh = rr * (0.4 + 0.7 * std::clamp(p.incrinato, 0.0, 1.0));
        for (int k = 0; k < n; ++k) {
            double ang = 2.0 * PI * (k + hash(k, static_cast<int>(p.seme_colpo), 157)) / n, x = ix, y = iy;
            for (double s = 0; s < lungh; s += 1.0) {
                ang += 0.25 * (hash(k, static_cast<int>(s), static_cast<int>(p.seme_colpo) + 159) - 0.5);
                x += std::cos(ang);
                y += std::sin(ang);
                const int px = static_cast<int>(x), py = static_cast<int>(y);
                if (!dentro_area(px, py)) break;
                t.pixel(px, py, crepa);
                if (dentro_area(px + 1, py + 1)) t.pixel(px + 1, py + 1, ombra);
            }
        }
        for (int anello = 1; anello <= 3; ++anello)   // gli anelli spezzati attorno all'impatto
            for (int g = 0; g < 360; g += 2) {
                if (hash(anello, g / 20, static_cast<int>(p.seme_colpo) + 161) < 0.45) continue;
                const double r = anello * lungh / 4.0;
                const int px = static_cast<int>(ix + r * std::cos(g * PI / 180.0)), py = static_cast<int>(iy + r * std::sin(g * PI / 180.0));
                if (dentro_area(px, py)) t.pixel(px, py, crepa);
            }
    }
    // l'immagine che si spegne: il rumore copre la scena e la luce cala fino al nero (spento 1: nero con un filo di
    // rumore)
    if (p.spento > 0.0) {
        const double s = std::clamp(p.spento, 0.0, 1.0);
        const int fotogramma = static_cast<int>(tempo_s * 25.0);
        for (int y = a.y; y < a.y + a.h; ++y)
            for (int x = a.x; x < a.x + a.w; ++x) {
                if (!dentro_area(x, y)) continue;
                const double h = hash(x, y, fotogramma);
                if (s >= 1.0) t.pixel(x, y, h < 0.03 ? static_cast<std::uint8_t>(colori::GRIGI + 3) : static_cast<std::uint8_t>(colori::NERO));
                else if (h < s) t.pixel(x, y, h < 0.5 * s ? static_cast<std::uint8_t>(colori::NERO)
                                                         : static_cast<std::uint8_t>(colori::GRIGI + static_cast<int>(14.0 * hash(y, x, fotogramma))));
            }
    }
}

}  // namespace m688::oculare
