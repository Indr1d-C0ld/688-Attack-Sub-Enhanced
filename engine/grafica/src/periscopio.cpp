#include "grafica/periscopio.hpp"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <vector>

#include "grafica/moto_navi.hpp"
#include "grafica/palette.hpp"
#include "nucleo/geometria.hpp"
#include "nucleo/meteo.hpp"
#include "nucleo/onde.hpp"
#include "nucleo/portata_luci.hpp"

namespace m688 {

namespace {

constexpr int CENTRO_X = LARGHEZZA_TELA / 2;
constexpr int RIGA_OCCHIO = ALTEZZA_VISTA / 2;     // riga dell'orizzontale ottico (centro del reticolo)
constexpr double DISTANZA_MASSIMA = 40000.0;       // m: oltre, nessuna nave e' visibile a quota periscopica
// battelli immersi (E2): sopra IMMERSO_M come in superficie, fino a QUOTA_SCAFO_M lo scafo affonda gradualmente,
// fino a QUOTA_ALBERI_M escono solo periscopio e antenna alzati, ALBERI_FUORI_M sul mare (STIMA)
constexpr double IMMERSO_M = 0.5, QUOTA_SCAFO_M = 3.0, QUOTA_ALBERI_M = 20.0, ALBERI_FUORI_M = 2.5;
// le navi intatte vicine dal modello (E9 §2.3): da 0,5 pixel per metro in su (una nave di 100 m larga 50 pixel), la
// scala a quarti d'ottava (si rigenera quando cambia di un passo), l'angolo a passi di 1 grado, campioni 2 x 2
constexpr double INTATTE_PX_M_MIN = 0.5;
constexpr int INTATTE_SOVRACAMPIONA = 2;

// retinatura ordinata 4x4 (stessa tecnica degli sprite della pipeline)
constexpr int BAYER[4][4] = {{0, 8, 2, 10}, {12, 4, 14, 6}, {3, 11, 1, 9}, {15, 7, 13, 5}};

std::uint8_t sfumatura(double indice, int x, int y) {
    const double soglia = (BAYER[y & 3][x & 3] + 0.5) / 16.0;
    const double base = std::floor(indice);
    return static_cast<std::uint8_t>(base + ((indice - base) > soglia ? 1.0 : 0.0));
}

// indice continuo dello sfondo nella riga y (orizzonte nella riga yo)
double indice_fondo(int y, int yo) {
    if (y < yo) {   // cielo: celeste all'orizzonte, piu' blu in alto
        const double f = 1.0 - static_cast<double>(y) / yo;
        return colori::CIELO_ORIZZONTE + f * (colori::CIELO_ALTO - colori::CIELO_ORIZZONTE);
    }
    // mare: piu' chiaro all'orizzonte, piu' scuro vicino
    const double f = static_cast<double>(y - yo) / std::max(ALTEZZA_VISTA - yo, 1);
    return colori::MARE_ORIZZONTE - std::sqrt(f) * (colori::MARE_ORIZZONTE - colori::MARE_VICINO);
}

// Spruzzo a prua (M3): la prua si pianta e solleva una nube bianca retinata alta fino a ~10% della lunghezza
void spruzzo_a_prua(Tela& t, double x, double y, double lunghezza_m, double aob_gradi, bool dritta, double px_m, double forza,
                    double tempo_s, int id, int limite) {
    const double lato = dritta ? 1.0 : -1.0;
    const double xp = x + lato * 0.5 * lunghezza_m * std::sin(radianti(aob_gradi)) * px_m;
    const double alto = 0.1 * lunghezza_m * forza * px_m, largo = std::max(2.0, 0.06 * lunghezza_m * px_m);
    const int f = static_cast<int>(tempo_s * 12.0);
    for (int py = static_cast<int>(y - alto); py <= static_cast<int>(y + 1); ++py)
        for (int px = static_cast<int>(xp - largo); px <= static_cast<int>(xp + largo); ++px) {
            if (py >= limite) continue;
            const double dx = (px - xp) / largo, dy = (y - py) / std::max(alto, 1.0);
            const double d = dx * dx + dy * dy * 0.8;
            const std::uint32_t h = static_cast<std::uint32_t>(px * 73856093) ^ static_cast<std::uint32_t>(py * 19349663) ^
                                    static_cast<std::uint32_t>((f + id) * 83492791);
            if (d < 1.0 && ((h >> 7) & 0xff) / 255.0 < forza * (1.0 - d)) t.pixel(px, py, static_cast<std::uint8_t>(colori::GRIGI + 14));
        }
}

// Con l'oculare (P3) il fondo segue l'elevazione della riga: il cielo si fa blu salendo, il mare scuro scendendo
double indice_fondo(const VistaPeriscopio& v, int y, int yo) {
    if (!v.oculare.attivo) return indice_fondo(y, yo);
    const double el = radianti(v.oculare.elevazione_gradi) + std::atan((RIGA_OCCHIO - (y + 0.5)) / Periscopio::pixel_per_radiante(v));
    if (y < yo) {
        const double f = std::pow(std::clamp(gradi(el) / 35.0, 0.0, 1.0), 0.8);
        return colori::CIELO_ORIZZONTE + f * (colori::CIELO_ALTO - colori::CIELO_ORIZZONTE);
    }
    const double f = std::clamp(-gradi(el) / 16.0, 0.0, 1.0);
    return colori::MARE_ORIZZONTE - std::sqrt(f) * (colori::MARE_ORIZZONTE - colori::MARE_VICINO);
}

// Creste delle onde davanti a un oggetto a distanza r: nelle colonne [x0, x1] ridipinge di mare le righe fra la cresta
// e il pelo dell'acqua (y_mare). Onde di lunghezza ~ 15 Hs + 10 m che corrono alla velocita' di fase sqrt(g L / 2 pi);
// `fase` lega l'onda al punto del mondo, cosi' resta ferma rispetto all'oggetto.
void copri_con_onde(Tela& t, int x0, int x1, double y_mare, double px_m, int yo, const VistaPeriscopio& v, double fase) {
    const double hs = v.altezza_onde_m;
    if (hs <= 0.0 || px_m * hs < 0.6) return;   // creste sotto il pixel
    const double L = 15.0 * hs + 10.0, c = std::sqrt(9.81 * L / (2 * PI));
    const double k = 2 * PI / (L * px_m), w = 2 * PI * c / L;
    const Rettangolo& rit = t.ritaglio();
    x0 = std::max(x0, rit.x); x1 = std::min(x1, rit.x + rit.w - 1);
    for (int x = x0; x <= x1; ++x) {
        const double h = 0.5 * hs * px_m *
                         (1.0 + 0.6 * std::sin(k * x + fase + w * v.tempo_s) + 0.3 * std::sin(2.7 * k * x + 1.9 * fase - 1.6 * w * v.tempo_s));
        const int ya = static_cast<int>(std::lround(y_mare - h)), yb = static_cast<int>(std::lround(y_mare));
        for (int y = std::max(ya, rit.y); y <= std::min(yb, rit.y + rit.h - 1); ++y)
            t.pixel(x, y, sfumatura(indice_fondo(v, std::max(y, yo), yo), x, y));   // anche sopra l'orizzonte: e' mare
    }
}

// I colori di un fanale nella vista: il nucleo e l'alone, negli EGA che la luce della notte non scurisce
std::pair<std::uint8_t, std::uint8_t> colori_fanale(luci::Colore c) {
    switch (c) {
        case luci::Colore::rosso: return {12, 4};
        case luci::Colore::verde: return {10, 2};
        case luci::Colore::giallo:
        case luci::Colore::ambra: return {14, 6};
        case luci::Colore::luci_coperta: return {7, 8};
        default: return {15, 7};
    }
}

void punto_fanale(Tela& t, int x, int y, luci::Colore c, double k, bool foschia, int limite);
void riflesso_fanale(Tela& t, double x_luce, double distanza_m, double quota_luce_m, double quota_occhio_m, double k,
                     luci::Colore colore, double vento_m_s, double ppr, bool oculare, double elevazione_ottica, double tempo_s,
                     std::uint32_t seme, int riga_min);

// I fari (C9 §2.15): come i fanali, ma fermi sulla costa e coi loro lampi. Oltre l'orizzonte si vedono finche' la
// luce sta sopra il calo della curvatura; la portata e la foschia vengono da luce_fanale. La terra fra il faro e chi
// guarda non li copre (STIMA: i fari stanno sui capi e sulle isole, rivolti al mare)
void disegna_fari(Tela& t, const VistaPeriscopio& v, Vec2 occhio) {
    const double ppr = Periscopio::pixel_per_radiante(v), h = v.quota_occhio_m, re = RAGGIO_TERRA_RIFRAZIONE;
    const double d_orizzonte = distanza_orizzonte(h), yo = Periscopio::riga_orizzonte(v);
    const double vento = v.meteo.attivo ? v.meteo.vento_m_s : std::sqrt(std::max(v.altezza_onde_m, 0.0) / 0.0246);
    const double elevazione_ottica = v.oculare.attivo ? radianti(v.oculare.elevazione_gradi) : 0.0;
    for (const auto& f : v.fari) {
        if (!f.acceso) continue;
        const double rl = std::max(distanza(occhio, f.pos), 1.0);
        const double rel = radianti(normalizza_180(rilevamento(occhio, f.pos) - v.rilevamento));
        if (std::cos(rel) < 0.05) continue;
        const double xl = CENTRO_X + std::tan(rel) * ppr;
        if (xl < 0.0 || xl >= LARGHEZZA_TELA) continue;
        double yl = Periscopio::riga_elevazione(v, std::atan2(f.quota_m - h, rl) - rl / (2.0 * re));
        int lim = ALTEZZA_VISTA;
        if (rl > d_orizzonte) {
            const double oltre = rl - d_orizzonte;
            if (f.quota_m <= oltre * oltre / (2.0 * re)) continue;
            lim = static_cast<int>(std::lround(yo)) + 1;
            yl = std::min(yl, static_cast<double>(lim - 1));
        }
        const double k = luce_fanale(f.portata_m, rl, v.visibilita_m, v.luce, v.oculare.attivo ? v.oculare.guadagno_luci : 1.0);
        const double riga_mare = Periscopio::riga_elevazione(v, -std::atan2(h, std::min(rl, d_orizzonte)) - std::min(rl, d_orizzonte) / (2.0 * re));
        riflesso_fanale(t, xl, rl, f.quota_m, h, k, f.colore, vento, ppr, v.oculare.attivo, elevazione_ottica, v.tempo_s,
                        static_cast<std::uint32_t>(f.seme), static_cast<int>(std::ceil(std::max(riga_mare, yo))) + 1);
        punto_fanale(t, static_cast<int>(std::lround(xl)), std::min(static_cast<int>(std::lround(yl)), lim - 1), f.colore, k,
                     v.visibilita_m < 4000.0, lim);
    }
}

// Il punto di un fanale con la luce `k` (multipli della soglia): appena sopra la soglia un pixel d'alone, da 2 il
// nucleo, da 15 la croce dell'alone, da 150 il nucleo a croce con l'alone attorno. Nella foschia (visibilita' sotto i
// 4 km) l'alone si allarga di un pixel. Niente sotto la riga `limite` (l'orizzonte, oltre la sua distanza)
void punto_fanale(Tela& t, int x, int y, luci::Colore c, double k, bool foschia, int limite) {
    if (k < 1.0 || y >= limite) return;
    const auto [nucleo, alone] = colori_fanale(c);
    const auto metti = [&](int px, int py, std::uint8_t col) {
        if (py < limite) t.pixel(px, py, col);
    };
    if (k < 2.0) {
        metti(x, y, alone);
        return;
    }
    const int raggio_alone = (k >= 150.0 ? 2 : k >= 15.0 ? 1 : 0) + (foschia && k >= 5.0 ? 1 : 0);
    for (int d = 1; d <= raggio_alone; ++d)
        for (const auto& [dx, dy] : {std::pair{d, 0}, {-d, 0}, {0, d}, {0, -d}}) metti(x + dx, y + dy, alone);
    if (raggio_alone >= 2)
        for (const auto& [dx, dy] : {std::pair{1, 1}, {-1, 1}, {1, -1}, {-1, -1}}) metti(x + dx, y + dy, alone);
    if (k >= 150.0)
        for (const auto& [dx, dy] : {std::pair{1, 0}, {-1, 0}, {0, 1}, {0, -1}}) metti(x + dx, y + dy, nucleo);
    metti(x, y, nucleo);
}

// Un numero fra 0 e 1 fermo per (a, b, c): la scintilla di un riflesso in una riga e in un istante
double scintilla(std::uint32_t a, std::uint32_t b, std::uint32_t c) {
    std::uint32_t x = a * 0x9E3779B1u ^ (b + 0x7F4A7C15u) * 0x85EBCA77u ^ (c + 0x165667B1u) * 0xC2B2AE3Du;
    x ^= x >> 15, x *= 0x2C1B3C6Du, x ^= x >> 12, x *= 0x297A2D39u, x ^= x >> 15;
    return (x & 0xFFFFFF) / 16777216.0;
}

// Il riflesso di un fanale sul mare (la scia luccicante, «glitter path»; Cox e Munk 1954): fra l'osservatore e la luce
// ogni riga di mare vista con la depressione a riflette la luce se una faccetta d'onda ha la pendenza giusta, cioe'
// inclinata di b = (a - a_luce) / 2 verso l'osservatore. Le pendenze sono gaussiane, con la varianza totale di Cox e Munk
// 0,003 + 0,00512 U (U il vento a 12,5 m, m/s; meta' per ognuna delle due componenti). La frazione della luce che una
// riga rimanda e' la densita' della pendenza per il suo intervallo di pendenze, per la riflettanza di Fresnel (Schlick,
// R0 0,02) all'incidenza radente: la luce si conserva riga per riga. La frazione per la luce del fanale `k` (multipli
// della soglia) e' la probabilita' che la riga scintilli in quel decimo di secondo; da 3 volte la soglia il nucleo.
// Le componenti laterali delle pendenze spostano la scintilla di 2 c sin(a medio) radianti. STIMA: senza l'ombra delle
// creste sulle faccette alla radente, e senza la schiuma
void riflesso_fanale(Tela& t, double x_luce, double distanza_m, double quota_luce_m, double quota_occhio_m, double k,
                     luci::Colore colore, double vento_m_s, double ppr, bool oculare, double elevazione_ottica, double tempo_s,
                     std::uint32_t seme, int riga_min) {
    if (k < 1.0 || distanza_m < 1.0) return;
    const double h = quota_occhio_m, re = RAGGIO_TERRA_RIFRAZIONE;
    const double sigma2 = 0.003 + 0.00512 * std::max(vento_m_s, 0.0);
    const auto [nucleo, alone] = colori_fanale(colore);
    const auto istante = static_cast<std::uint32_t>(std::max(0.0, tempo_s) * 10.0);
    // la pendenza richiesta nella riga y (depressione a), e il mezzo angolo medio per Fresnel e lo spostamento laterale
    struct Pendenza { bool ok; double s, medio; };
    const auto pendenza = [&](double y) -> Pendenza {
        // la depressione sotto l'orizzontale, come la inverte riga_elevazione (oculare) o riga_mare (vista del classico)
        const double a = oculare ? std::atan((y - RIGA_OCCHIO) / ppr) - elevazione_ottica : (y - RIGA_OCCHIO) / ppr;
        const double disc = a * a - 2.0 * h / re;
        if (a <= 0.0 || disc < 0.0) return {false, 0.0, 0.0};
        const double d = (a - std::sqrt(disc)) * re;   // il punto di mare piu' vicino con quella depressione
        if (d >= distanza_m) return {false, 0.0, 0.0};
        const double resto = distanza_m - d;
        const double a_luce = (quota_luce_m - resto * resto / (2.0 * re)) / resto;
        return {true, std::tan(0.5 * (a - a_luce)), 0.5 * (a + a_luce)};
    };
    const int y0 = std::max(riga_min, 0);
    for (int y = y0; y < ALTEZZA_VISTA; ++y) {
        const Pendenza p = pendenza(y), q = pendenza(y + 1.0);
        if (!p.ok || !q.ok) continue;
        const double densita = std::exp(-p.s * p.s / sigma2) / std::sqrt(PI * sigma2);
        const double coseno = std::clamp(std::sin(p.medio), 0.0, 1.0);
        const double fresnel = 0.02 + 0.98 * std::pow(1.0 - coseno, 5.0);
        const double m = k * fresnel * densita * std::fabs(q.s - p.s);
        if (m < 1e-4 || scintilla(seme, static_cast<std::uint32_t>(y), istante) >= m) continue;
        // lo spostamento laterale: una gaussiana approssimata con la somma di tre uniformi
        const double g = (scintilla(seme ^ 0x51u, static_cast<std::uint32_t>(y), istante) + scintilla(seme ^ 0xA3u, static_cast<std::uint32_t>(y), istante) +
                          scintilla(seme ^ 0xF5u, static_cast<std::uint32_t>(y), istante) - 1.5) * 2.0;
        const double laterale = 2.0 * g * std::sqrt(0.5 * sigma2) * std::max(p.medio, 0.0) * ppr;
        t.pixel(static_cast<int>(std::lround(x_luce + laterale)), y, m >= 3.0 ? nucleo : alone);
    }
}

}  // namespace

double Periscopio::pixel_per_radiante(bool alto) {
    return LARGHEZZA_TELA / radianti(alto ? CAMPO_ALTO_GRADI : CAMPO_BASSO_GRADI);
}

double Periscopio::pixel_per_radiante(const VistaPeriscopio& v) {
    return v.oculare.attivo ? oculare::pixel_per_radiante(v.oculare) : pixel_per_radiante(v.alto_ingrandimento);
}

double Periscopio::riga_orizzonte(const VistaPeriscopio& v) {
    if (v.oculare.attivo) return riga_elevazione(v, -depressione_orizzonte(v.quota_occhio_m));
    return RIGA_OCCHIO + depressione_orizzonte(v.quota_occhio_m) * pixel_per_radiante(v.alto_ingrandimento);
}

double Periscopio::riga_elevazione(const VistaPeriscopio& v, double elevazione) {
    const double e = v.oculare.attivo ? radianti(v.oculare.elevazione_gradi) : 0.0;
    return RIGA_OCCHIO - std::tan(elevazione - e) * pixel_per_radiante(v);
}

void applica_meteo(VistaPeriscopio& v, const StatoMeteo& s, const std::vector<Lampo>& lampi, Vec2 occhio) {
    MeteoVista& m = v.meteo;
    m.attivo = true;
    m.nubi = std::clamp(s.nubi_ottavi / 8.0, 0.0, 1.0);
    m.plafond_m = s.plafond_m;
    m.temporale = s.lampi_al_minuto > 0.0 ? std::clamp(0.5 + s.lampi_al_minuto / 8.0, 0.0, 1.0) : 0.0;
    m.pioggia_mm_h = s.pioggia_mm_h;
    m.neve_mm_h = s.neve_mm_h;
    m.vento_m_s = s.vento_m_s;
    m.vento_da_gradi = s.vento_da_gradi;
    const bool swell = s.hs_swell_m > s.hs_vento_m;   // il moto delle navi segue il mare dominante
    m.mare_da_gradi = swell ? s.swell_da_gradi : s.vento_da_gradi;
    m.tp_s = swell ? s.tp_swell_s : s.tp_vento_s;
    m.nebbia = s.nebbia == Nebbia::avvezione      ? NebbiaVista::avvezione
               : s.nebbia == Nebbia::fumo_di_mare ? NebbiaVista::fumo_di_mare
               : s.nebbia == Nebbia::radiazione   ? NebbiaVista::radiazione
                                                  : NebbiaVista::nessuna;   // nebbia da precipitazione: basta la visibilita'
    m.strato_nebbia_m = s.strato_nebbia_m;
    m.lampi.clear();
    for (const auto& l : lampi) {
        const std::uint32_t seme = static_cast<std::uint32_t>(l.t * 1000.0) ^ static_cast<std::uint32_t>(l.pos.x * 7.0 + l.pos.y * 13.0);
        m.lampi.push_back({l.t, rilevamento(occhio, l.pos), distanza(occhio, l.pos), l.colpi, l.nube_mare, seme | 1u});
    }
    v.altezza_onde_m = std::sqrt(s.hs_vento_m * s.hs_vento_m + s.hs_swell_m * s.hs_swell_m);
    v.visibilita_m = s.visibilita_m;
    v.oculare.direzione_mare_gradi = m.mare_da_gradi;
    v.oculare.vento_kn = s.vento_m_s / 0.5144;
    v.oculare.pioggia = std::max(v.oculare.pioggia, std::clamp(s.pioggia_mm_h / 8.0, 0.0, 1.0));
}

GeometriaVista Periscopio::geometria(const VistaPeriscopio& v) {
    GeometriaVista q;
    q.cx = CENTRO_X;
    q.cy = RIGA_OCCHIO;
    q.ppr = pixel_per_radiante(v);
    q.elevazione_rad = v.oculare.attivo ? radianti(v.oculare.elevazione_gradi) : 0.0;
    q.rilevamento_gradi = v.rilevamento;
    q.area = v.oculare.attivo ? oculare::area(v.oculare) : Rettangolo{0, 0, LARGHEZZA_TELA, ALTEZZA_VISTA};
    q.quota_occhio_m = v.quota_occhio_m;
    return q;
}

void Periscopio::disegna(Tela& t, const Mondo& mondo, const Nave& proprio, const VistaPeriscopio& v, const Effetti* effetti) const {
    t.imposta_ritaglio({0, 0, LARGHEZZA_TELA, ALTEZZA_VISTA});
    if (!v.oculare.attivo) {
        disegna_fondo(t, v);
        if (v.meteo.attivo) {   // meteo (M3) anche nella vista rettangolare
            const GeometriaVista q = geometria(v);
            cielo::nubi(t, q, v.meteo, v.tempo_s);
            cielo::foschia(t, q, v.meteo, v.visibilita_m);
            cielo::fulmini(t, q, v.meteo, v.tempo_s);
        }
        disegna_navi(t, mondo, proprio, v, effetti);
        if (v.meteo.attivo) {
            const GeometriaVista q = geometria(v);
            cielo::fumo_di_mare(t, q, v.meteo, v.tempo_s);
            cielo::precipitazione(t, q, v.meteo, v.tempo_s);
        }
        if (v.reticolo) disegna_reticolo(t, v);
        t.togli_ritaglio();
        return;
    }
    if (v.oculare.telecamera_verticale) {   // Alfa: la telecamera della vela, anche col periscopio giu'
        ParametriOculare tv = v.oculare;
        tv.modo = ModoOculare::tv;
        oculare::telecamera_verticale(t, oculare::area(tv), v.oculare.quota_m, v.luce, v.oculare.ghiaccio, v.tempo_s);
        oculare::maschera(t, tv);
        t.togli_ritaglio();
        return;
    }
    // oculare (P3): la linea di mira trema, poi immagine fantasma, modo monocromo, reticolo, cerchio e scala
    VistaPeriscopio w = v;
    double d_ril = 0, d_el = 0;
    oculare::tremolio(v.oculare, v.tempo_s, d_ril, d_el);
    w.rilevamento += d_ril;
    w.oculare.elevazione_gradi += d_el;
    // dentro il pack le lastre smorzano le onde: nelle polynye e nei canali il mare e' quasi calmo (STIMA)
    if (w.ghiaccio.attivo && w.ghiaccio.copertura >= 0.3)
        w.altezza_onde_m = std::min(w.altezza_onde_m, 0.3 * (1.0 - w.ghiaccio.copertura) + 0.05);
    // le nubi del meteo, che riflettono il ghiaccio e l'acqua sotto di loro
    if (w.ghiaccio.attivo && w.meteo.attivo) {
        w.ghiaccio.nubi = std::max(w.meteo.nubi, w.meteo.temporale);
        w.ghiaccio.plafond_m = w.meteo.plafond_m;
    }
    // col posto noto l'occhio sale e scende rispetto al mare che gli passa davanti: l'orizzonte e le creste vicine
    // si muovono con l'onda (nucleo/onde.hpp)
    if (w.oculare.altezza_testa_m >= 0.0 && w.altezza_onde_m > 0.0)
        w.quota_occhio_m = std::max(0.15, w.oculare.altezza_testa_m - elevazione_onda_m(w.altezza_onde_m, w.tempo_s));
    if (w.oculare.sott_acqua) {
        oculare::acqua(t, w.oculare, w.luce, w.tempo_s);
    } else {
        const GeometriaVista q = geometria(w);
        disegna_fondo(t, w);
        if (w.cielo_lunare) cielo::luna(t, q, w.cielo, w.luce);   // la luna (Enhanced) sotto le nubi
        if (w.meteo.attivo) cielo::nubi(t, q, w.meteo, w.tempo_s);
        oculare::ProfiloMare profilo;
        oculare::mare(t, w.oculare, w.rilevamento, proprio.pos.x, proprio.pos.y, w.quota_occhio_m, w.altezza_onde_m,
                      w.visibilita_m, w.tempo_s, &profilo);
        if (w.ghiaccio.attivo)   // la banchisa sopra il mare, che aggiunge al profilo le creste che coprono le navi
            oculare::banchisa(t, w.oculare, w.rilevamento, proprio.pos.x, proprio.pos.y, w.quota_occhio_m, w.visibilita_m,
                              w.tempo_s, w.ghiaccio, &profilo);
        if (w.meteo.attivo) cielo::foschia(t, q, w.meteo, w.visibilita_m);
        // la terraferma sopra il mare e il ghiaccio, dopo la foschia: la foschia del cielo, alla riga della terra, la
        // coprirebbe come se dietro ci fosse cielo per decine di chilometri; la costa ha la sua, con la distanza vera.
        // Copre le navi dietro le isole
        if (w.costa.attiva)
            oculare::costa(t, w.oculare, w.rilevamento, proprio.pos.x, proprio.pos.y, w.quota_occhio_m, w.visibilita_m, w.luce,
                           w.costa, &profilo);
        // i fari (Enhanced): sulla costa, sotto le navi che passano davanti
        if (!w.fari.empty()) disegna_fari(t, w, proprio.pos);
        // la scia della luna sul mare (Enhanced): sopra il mare, la banchisa e la costa, sotto le navi
        if (w.cielo_lunare) cielo::scia_lunare(t, q, w.cielo, w.luce, w.altezza_onde_m, w.tempo_s);
        if (w.meteo.attivo) cielo::fulmini(t, q, w.meteo, w.tempo_s);
        if (profilo.colonne.empty()) {
            disegna_navi(t, mondo, proprio, w, effetti);
        } else {
            const Tela fondo = t;   // il mare vicino, per ridipingere le creste davanti a navi e particelle
            disegna_navi(t, mondo, proprio, w, effetti, &profilo, &fondo);
        }
        if (w.meteo.attivo) cielo::fumo_di_mare(t, q, w.meteo, w.tempo_s);
        oculare::sdoppia(t, w.oculare);
        if (w.meteo.attivo) {
            cielo::precipitazione(t, q, w.meteo, w.tempo_s);
            // la pioggia bagna anche la finestra della testa
            w.oculare.pioggia = std::max(w.oculare.pioggia, std::clamp(w.meteo.pioggia_mm_h / 8.0, 0.0, 1.0));
        }
        oculare::acqua_sulla_finestra(t, w.oculare, w.altezza_onde_m, w.luce, w.tempo_s);
    }
    if (giorno_) oculare::converti(t, *giorno_, w.oculare, w.luce, w.tempo_s);
    oculare::guasto(t, w.oculare, w.luce, w.tempo_s);   // la testa colpita dall'artiglieria (G4): nulla se intatta
    oculare::disegna_reticolo(t, w.oculare);
    oculare::maschera(t, w.oculare);
    t.rettangolo_pieno(0, 0, LARGHEZZA_TELA, 30, colori::NERO);
    disegna_scala(t, v);
    t.togli_ritaglio();
}

void Periscopio::disegna_fondo(Tela& t, const VistaPeriscopio& v) const {
    const int yo = static_cast<int>(std::lround(riga_orizzonte(v)));
    for (int y = 0; y < ALTEZZA_VISTA; ++y) {
        const double indice = indice_fondo(v, y, yo);
        for (int x = 0; x < LARGHEZZA_TELA; ++x) t.pixel(x, y, sfumatura(indice, x, y));
    }
}

void Periscopio::disegna_navi(Tela& t, const Mondo& mondo, const Nave& proprio, const VistaPeriscopio& v,
                              const Effetti* effetti, const oculare::ProfiloMare* profilo, const Tela* mare_vicino) const {
    // le creste del mare vicino davanti a un oggetto a distanza r: nelle colonne [x0, x1] le righe da quella della cresta
    // fino a `y_basso` tornano mare
    auto dietro_le_creste = [&](double x0, double x1, double y_basso, double r) {
        if (!profilo || !mare_vicino) return;
        const Rettangolo& rit = t.ritaglio();
        const int xa = std::max(static_cast<int>(std::floor(x0)), rit.x), xb = std::min(static_cast<int>(std::ceil(x1)), rit.x + rit.w - 1);
        const int yb = std::min(static_cast<int>(std::ceil(y_basso)) + 2, rit.y + rit.h - 1);
        for (int x = xa; x <= xb; ++x)
            for (int y = std::max(profilo->riga(x, r), rit.y); y <= yb; ++y) t.pixel(x, y, mare_vicino->at(x, y));
    };
    const double ppr = pixel_per_radiante(v);
    const double semicampo = v.oculare.attivo ? radianti(v.oculare.campo_gradi) / 2.0
                                              : radianti(v.alto_ingrandimento ? CAMPO_ALTO_GRADI : CAMPO_BASSO_GRADI) / 2.0;
    const double h = v.quota_occhio_m;
    // riga del pelo dell'acqua a distanza r (quota dell'occhio e curvatura terrestre)
    auto riga_mare = [&](double r) {
        const double d = h / r + r / (2.0 * RAGGIO_TERRA_RIFRAZIONE);
        return v.oculare.attivo ? riga_elevazione(v, -d) : RIGA_OCCHIO + d * ppr;
    };
    const double yo = riga_orizzonte(v);
    const double d_orizzonte = distanza_orizzonte(h);

    // navi e particelle in un solo elenco, dalla piu' lontana alla piu' vicina: le vicine coprono le lontane
    struct Visibile { const Nave* nave; const Particella* particella; double distanza; };
    std::vector<Visibile> vis;
    for (const auto& n : mondo.navi()) {
        if (n.id == proprio.id) continue;
        const double r = distanza(proprio.pos, n.pos);
        if (r < 30.0 || r > DISTANZA_MASSIMA) continue;
        vis.push_back({&n, nullptr, r});
    }
    if (effetti) {
        auto aggiungi = [&](const Particella& p) {
            if (p.ritardo > 0) return;
            const double r = std::hypot(p.x - proprio.pos.x, p.y - proprio.pos.y);
            if (r < 5.0 || r > 2 * DISTANZA_MASSIMA) return;
            const double rel = radianti(normalizza_180(rilevamento(proprio.pos, {p.x, p.y}) - v.rilevamento));
            if (std::fabs(rel) - std::atan2(p.r, r) > semicampo) return;
            vis.push_back({nullptr, &p, r});
        };
        for (const auto& p : effetti->particelle()) aggiungi(p);
        for (const auto& p : effetti->orli()) aggiungi(p);
    }
    std::stable_sort(vis.begin(), vis.end(), [](const Visibile& a, const Visibile& b) { return a.distanza > b.distanza; });

    for (const auto& [n, p, r] : vis) {
        if (p) {   // particella: proiezione come le navi, con la sua quota sul mare
            const double rel = radianti(normalizza_180(rilevamento(proprio.pos, {p->x, p->y}) - v.rilevamento));
            if (std::cos(rel) < 0.05) continue;
            const double x = CENTRO_X + std::tan(rel) * ppr;
            const double elevazione = std::atan2(p->z - h, r) - r / (2.0 * RAGGIO_TERRA_RIFRAZIONE);
            const double y = riga_elevazione(v, elevazione);
            int limite = r > d_orizzonte ? static_cast<int>(std::lround(yo)) : ALTEZZA_VISTA;
            if (!p->piatta) {   // niente sotto il pelo dell'acqua: fumo e spruzzi finiscono sul mare
                const double mare = riga_mare(r);
                limite = std::min(limite, static_cast<int>(std::ceil(mare)) + 1);
            }
            const double foschia = 1.0 - std::exp(-3.912 * r / std::max(v.visibilita_m, 1.0));
            effetti->disegna(t, *p, x, y, ppr / r, limite, v.luce, h, r, foschia);
            if (!p->piatta) dietro_le_creste(x - p->r * ppr / r, x + p->r * ppr / r, std::min<double>(limite, riga_mare(r)), r);
            if (!p->piatta && r <= d_orizzonte) {   // le creste nascondono la base delle colonne
                const double mare = riga_mare(r);
                const double rp = p->r * ppr / r;
                if (y + rp > mare - v.altezza_onde_m * ppr / r)
                    copri_con_onde(t, static_cast<int>(std::floor(x - rp)), static_cast<int>(std::ceil(x + rp)), mare, ppr / r,
                                   static_cast<int>(std::lround(yo)), v, std::floor(p->x / 50.0) * 1.3 + std::floor(p->y / 50.0) * 2.1);
            }
            continue;
        }
        const auto ic = classi_.find(n->classe);
        const auto is = sprite_.find(n->classe);
        if (ic == classi_.end() || is == sprite_.end()) continue;
        const double rel = radianti(normalizza_180(rilevamento(proprio.pos, n->pos) - v.rilevamento));
        const double semiampiezza = ic->second.lunghezza_m / 2.0 / r;
        if (std::fabs(rel) - semiampiezza > semicampo) continue;

        const AngoloSullaPrua aob = angolo_sulla_prua(n->pos, n->rotta, proprio.pos);
        if (n->quota < 0.0) {   // in volo (A4): all'altezza vera, lo sprite visto dal basso con l'elevazione piu' vicina
            const double el = std::atan2(-n->quota - h, r);
            const Sprite& s = is->second.per_angolo(aob.gradi, aob.dritta, std::max(0.0, el * 180.0 / PI));
            const double x = CENTRO_X + std::tan(rel) * ppr;
            const double y = riga_elevazione(v, el - r / (2.0 * RAGGIO_TERRA_RIFRAZIONE));
            const int limite = r > d_orizzonte ? static_cast<int>(std::lround(yo)) : ALTEZZA_VISTA;
            const double foschia = 1.0 - std::exp(-3.912 * r / std::max(v.visibilita_m, 1.0));
            if (v.meteo.attivo && n->velocita < 3.0) {
                // in hover col vento (M3): muso al vento e un po' basso, oscillazioni con le raffiche (STIMA)
                const double vento = std::clamp(v.meteo.vento_m_s / 15.0, 0.0, 1.0);
                const double muso = 3.0 * vento + 1.5 * vento * std::sin(v.tempo_s * 1.9 + n->id);
                const double rollio = 1.5 * vento * std::sin(v.tempo_s * 2.6 + 0.5 * n->id);
                MotoNave mh;
                mh.beccheggio_gradi = -muso;
                mh.rollio_gradi = rollio;
                disegna_sprite_ruotato(t, s, x, y, ppr / (r * is->second.scala_px_m()), angolo_apparente(mh, aob.gradi, aob.dritta),
                                       limite, foschia);
            } else {
                t.disegna_sprite(s, x, y, ppr / (r * is->second.scala_px_m()), limite, foschia);
            }
            continue;
        }
        const Sprite* sp = &is->second.per_angolo(aob.gradi, aob.dritta);
        double scala = ppr / (r * is->second.scala_px_m());
        // immerso (E2): fra 0,5 e 3 m lo scafo affonda gradualmente; piu' sotto, fino a 20 m, escono solo gli alberi
        // alzati (STIMA: 2,5 m sul mare); altrimenti non si vede niente (siluri, battelli profondi)
        const auto im = modelli_ ? modelli_->find(n->classe) : decltype(modelli_->end()){};
        Nave immerso;
        if (n->quota > IMMERSO_M) {
            if (!modelli_ || im == modelli_->end() || n->quota > QUOTA_ALBERI_M || (!n->alberi_alzati && n->quota >= QUOTA_SCAFO_M))
                continue;
            const double zmax = im->second.quota_max_m();
            const double fondo = n->alberi_alzati ? zmax - ALBERI_FUORI_M : zmax + 0.5;
            const double affonda = fondo * std::min(n->quota / QUOTA_SCAFO_M, 1.0);
            if (affonda >= zmax) continue;
            immerso = *n;
            immerso.posa.danneggiata = true;
            immerso.posa.prodiero.immersione += affonda;
            immerso.posa.poppiero.immersione += affonda;
        }
        const Nave& nd = n->quota > IMMERSO_M ? immerso : *n;
        if (nd.posa.danneggiata && modelli_ && im != modelli_->end()) {   // danneggiata o immersa: sprite dal modello
            const double px_m = std::min<double>(is->second.scala_px_m(), ppr / r);
            sp = &sprite_generato(nd, im->second, aob.gradi, aob.dritta, px_m);
            scala = ppr / (r * px_m);
        } else if (v.oculare.attivo && modelli_ && im != modelli_->end() && ppr / r >= INTATTE_PX_M_MIN) {
            // intatta e vicina (Enhanced): dal modello a passi di 1 grado invece degli sprite ogni 5
            const double px_m = std::min<double>(is->second.scala_px_m(),
                                                 std::exp2(std::floor(std::log2(ppr / r) * 4.0) / 4.0));
            if (const Sprite* gi = sprite_intatto(nd, im->second, aob.gradi, aob.dritta, px_m)) {
                sp = gi;
                scala = ppr / (r * intatti_[nd.id].scala);   // quella dello sprite che c'e' (col filo puo' essere la vecchia)
            }
        }
        const Sprite& s = *sp;
        const double x = CENTRO_X + std::tan(rel) * ppr;
        // galleggiamento: sotto l'orizzontale per la quota dell'occhio e per la curvatura terrestre
        double y = riga_mare(r);
        // oltre l'orizzonte geometrico la parte bassa della nave e' nascosta dal mare
        const int limite = r > d_orizzonte ? static_cast<int>(std::lround(yo)) : ALTEZZA_VISTA;
        const double foschia = 1.0 - std::exp(-3.912 * r / std::max(v.visibilita_m, 1.0));
        if (v.meteo.attivo && v.altezza_onde_m > 0.0) {
            // moto col mare (M3): sussulto, rollio e beccheggio col periodo d'incontro, spruzzo quando la prua si pianta
            const MotoNave mn = moto_nave(ic->second.lunghezza_m, ic->second.larghezza_m, v.altezza_onde_m, v.meteo.tp_s,
                                          v.meteo.mare_da_gradi, n->rotta, n->velocita, v.tempo_s, 1.7 * n->id);
            y -= mn.sussulto_m * ppr / r;
            disegna_sprite_ruotato(t, s, x, y, scala, angolo_apparente(mn, aob.gradi, aob.dritta), limite, foschia);
            if (mn.spruzzo > 0.0) spruzzo_a_prua(t, x, y, ic->second.lunghezza_m, aob.gradi, aob.dritta, ppr / r, mn.spruzzo,
                                                 v.tempo_s, n->id, limite);
        } else {
            t.disegna_sprite(s, x, y, scala, limite, foschia);
        }
        {   // le creste del mare vicino davanti alla nave
            const double mezza = std::max(static_cast<double>(s.w) - s.ancora_x, static_cast<double>(s.ancora_x)) * scala;
            dietro_le_creste(x - mezza, x + mezza, std::min<double>(limite, y), r);
        }
        if (r <= d_orizzonte) {   // onde davanti allo scafo
            const double mezza = std::max(static_cast<double>(s.w) - s.ancora_x, static_cast<double>(s.ancora_x)) * scala;
            copri_con_onde(t, static_cast<int>(std::floor(x - mezza)), static_cast<int>(std::ceil(x + mezza)), y, ppr / r,
                           static_cast<int>(std::lround(yo)), v, n->pos.x * 0.013 + n->pos.y * 0.029);
        }

        // le luci di navigazione (Enhanced, docs/27): i fanali accesi nello stato della nave, visti dal loro settore
        if (v.luci.configurazioni && n->quota <= IMMERSO_M)
            if (const auto ist = v.luci.stati.find(n->id); ist != v.luci.stati.end())
                if (const auto icf = v.luci.configurazioni->find(n->classe); icf != v.luci.configurazioni->end()) {
                    const double rotta = radianti(n->rotta);
                    const Vec2 avanti{std::sin(rotta), std::cos(rotta)}, sinistra{-std::cos(rotta), std::sin(rotta)};
                    const double rel_osservatore = normalizza_180(rilevamento(n->pos, proprio.pos) - n->rotta);
                    const bool nella_foschia = v.visibilita_m < 4000.0;
                    const double tp = std::max(v.meteo.tp_s, 6.0);
                    // il vento per le pendenze delle onde: quello del meteo, o dal mare formato (Hs = 0,0246 U^2, STIMA)
                    const double vento = v.meteo.attivo ? v.meteo.vento_m_s : std::sqrt(std::max(v.altezza_onde_m, 0.0) / 0.0246);
                    const double elevazione_ottica = v.oculare.attivo ? radianti(v.oculare.elevazione_gradi) : 0.0;
                    for (const luci::Fanale* f : luci::accesi(icf->second, ist->second, v.tempo_s)) {
                        if (!luci::nel_settore(*f, rel_osservatore)) continue;
                        const Vec2 pl{n->pos.x + avanti.x * f->pos_m[0] + sinistra.x * f->pos_m[1],
                                      n->pos.y + avanti.y * f->pos_m[0] + sinistra.y * f->pos_m[1]};
                        const double rl = std::max(distanza(proprio.pos, pl), 1.0);
                        const double rel_l = radianti(normalizza_180(rilevamento(proprio.pos, pl) - v.rilevamento));
                        if (std::cos(rel_l) < 0.05) continue;
                        const double z = f->pos_m[2];
                        // le creste vicine coprono le luci basse a intermittenza: una cresta a 40 m, alta fino all'altezza
                        // delle onde, col periodo del mare (STIMA)
                        if (v.altezza_onde_m > 0.0) {
                            const double cresta = 0.5 * v.altezza_onde_m *
                                                  (1.0 + std::sin(v.tempo_s * 2.0 * PI / tp + 0.37 * (CENTRO_X + std::tan(rel_l) * ppr) + n->id));
                            if (std::atan2(z - h, rl) < std::atan2(cresta - h, 40.0)) continue;
                        }
                        const double xl = CENTRO_X + std::tan(rel_l) * ppr;
                        double yl = riga_elevazione(v, std::atan2(z - h, rl) - rl / (2.0 * RAGGIO_TERRA_RIFRAZIONE));
                        int lim = ALTEZZA_VISTA;
                        if (rl > d_orizzonte) {
                            // oltre l'orizzonte: nascosto solo se il fanale sta sotto l'orizzonte geometrico (piu' basso
                            // del calo della curvatura dopo l'orizzonte di chi guarda); se si vede, almeno sulla riga
                            // dell'orizzonte. Prima la riga dell'orizzonte tagliava tutto: i fanali bassi lontani, a
                            // meno di un pixel sopra, sparivano verso 4-5 km (I15, trailer-1, 29/10/2026)
                            const double oltre = rl - d_orizzonte;
                            if (z <= oltre * oltre / (2.0 * RAGGIO_TERRA_RIFRAZIONE)) continue;
                            lim = static_cast<int>(std::lround(yo)) + 1;
                            yl = std::min(yl, static_cast<double>(lim - 1));
                        }
                        // nell'oculare dell'Enhanced l'ottica raccoglie piu' luce a 6x (guadagno 5,5) che a 1,5x (0,68)
                        const double k = luce_fanale(f->portata_m, rl, v.visibilita_m, v.luce, v.oculare.attivo ? v.oculare.guadagno_luci : 1.0);
                        // il riflesso sul mare, sotto il pelo dell'acqua della luce (e sotto l'orizzonte se e' oltre)
                        riflesso_fanale(t, xl, rl, z, h, k, f->colore, vento, ppr, v.oculare.attivo, elevazione_ottica, v.tempo_s,
                                        static_cast<std::uint32_t>(n->id * 131 + (f - icf->second.fanali.data())),
                                        static_cast<int>(std::ceil(std::max(riga_mare(rl), yo))) + 1);
                        punto_fanale(t, static_cast<int>(std::lround(xl)), std::min(static_cast<int>(std::lround(yl)), lim - 1), f->colore,
                                     k, nella_foschia, lim);
                    }
                }

        if (v.mostra_verita) {
            char testo[96];
            std::snprintf(testo, sizeof testo, "%s %.1f MN AOB %.0f%s", ic->second.id.c_str(), r / MIGLIO,
                          aob.gradi, aob.dritta ? "D" : "S");
            font_.scrivi(t, static_cast<int>(x) - font_.larghezza(testo, 1, 1, false) / 2, static_cast<int>(yo) + 8, testo,
                         colori::GIALLO, 1, 1, false);
        }
    }
}

namespace {
// falle, fuliggine (a passi di 0,05) e parti mosse (a passi di 0,02 rad) uguali: lo sprite in cache vale ancora
bool segni_uguali(const PosaNave& a, const PosaNave& b) {
    if (a.segni.size() != b.segni.size() || a.parti.size() != b.parti.size()) return false;
    if (std::fabs(a.flessione_m - b.flessione_m) > 0.05) return false;
    for (std::size_t i = 0; i < a.segni.size(); ++i)
        if (a.segni[i].x != b.segni[i].x || a.segni[i].rx != b.segni[i].rx || std::fabs(a.segni[i].intensita - b.segni[i].intensita) > 0.05)
            return false;
    for (std::size_t i = 0; i < a.parti.size(); ++i)
        if (a.parti[i].parte != b.parti[i].parte || std::fabs(a.parti[i].angolo - b.parti[i].angolo) > 0.02) return false;
    return true;
}
}  // namespace

const Sprite& Periscopio::sprite_generato(const Nave& n, const Modello& m, double aob, bool dritta, double scala) const {
    auto& g = generati_[n.id];
    auto uguali = [](const AssettoScafo& a, const AssettoScafo& b) {
        return std::fabs(a.sbandamento - b.sbandamento) < 0.05 && std::fabs(a.assetto - b.assetto) < 0.05 &&
               std::fabs(a.immersione - b.immersione) < 0.02 && std::fabs(a.dx - b.dx) < 0.05 && std::fabs(a.dy - b.dy) < 0.05 &&
               a.x_perno == b.x_perno;
    };
    const bool valido = std::fabs(g.aob - aob) < 0.5 && g.dritta == dritta && std::fabs(g.scala - scala) < 0.05 * scala &&
                        g.posa.spezzata == n.posa.spezzata && g.posa.x_rottura == n.posa.x_rottura &&
                        uguali(g.posa.prodiero, n.posa.prodiero) && uguali(g.posa.poppiero, n.posa.poppiero) &&
                        segni_uguali(g.posa, n.posa);
    if (!valido) {
        g.sprite = genera_sprite(m, n.posa, aob, dritta, rampa_, scala);
        g.aob = aob; g.dritta = dritta; g.scala = scala; g.posa = n.posa;
    }
    return g.sprite;
}

const Sprite* Periscopio::sprite_intatto(const Nave& n, const Modello& m, double aob, bool dritta, double scala) const {
    auto& g = intatti_[n.id];
    const double passo = std::round(aob);
    // il lavoro finito diventa lo sprite della nave
    if (g.lavoro && g.lavoro->wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
        g.sprite = g.lavoro->get();
        g.aob = g.lavoro_aob; g.dritta = g.lavoro_dritta; g.scala = g.lavoro_scala;
        g.lavoro.reset();
    }
    auto uguale = [&](double a, bool d, double sc) { return a == passo && d == dritta && std::fabs(sc - scala) < 1e-9; };
    if (g.aob >= 0.0 && uguale(g.aob, g.dritta, g.scala)) return &g.sprite;
    if (!filo_di_lavoro_) {   // istantanee e prove: subito, deterministico
        g.sprite = genera_sprite(m, n.posa, passo, dritta, rampa_, scala, INTATTE_SOVRACAMPIONA);
        g.aob = passo; g.dritta = dritta; g.scala = scala;
        return &g.sprite;
    }
    // un lavoro alla volta per tutta la vista; intanto resta lo sprite vecchio (o quello precalcolato)
    const bool occupato = std::any_of(intatti_.begin(), intatti_.end(), [](const auto& e) { return e.second.lavoro != nullptr; });
    if (!occupato) {
        g.lavoro = std::make_shared<std::future<Sprite>>(std::async(
            std::launch::async, [&m, posa = n.posa, passo, dritta, rampa = rampa_, scala] {
                return genera_sprite(m, posa, passo, dritta, rampa, scala, INTATTE_SOVRACAMPIONA);
            }));
        g.lavoro_aob = passo; g.lavoro_dritta = dritta; g.lavoro_scala = scala;
    }
    return g.aob >= 0.0 && g.dritta == dritta ? &g.sprite : nullptr;
}

void Periscopio::disegna_reticolo(Tela& t, const VistaPeriscopio& v) const {
    const double ppr = pixel_per_radiante(v.alto_ingrandimento);
    const std::uint8_t c = colori::NERO;
    // croce del reticolo con un vuoto al centro, per non coprire il bersaglio
    t.linea_verticale(CENTRO_X, 40, RIGA_OCCHIO - 10, c);
    t.linea_verticale(CENTRO_X, RIGA_OCCHIO + 10, ALTEZZA_VISTA - 30, c);
    t.linea_orizzontale(40, CENTRO_X - 10, RIGA_OCCHIO, c);
    t.linea_orizzontale(CENTRO_X + 10, LARGHEZZA_TELA - 40, RIGA_OCCHIO, c);
    // tacche del reticolo: ogni grado, piu' lunghe ogni 5
    const double semicampo = (v.alto_ingrandimento ? CAMPO_ALTO_GRADI : CAMPO_BASSO_GRADI) / 2.0;
    for (int g = -static_cast<int>(semicampo); g <= static_cast<int>(semicampo); ++g) {
        if (g == 0) continue;
        const int x = CENTRO_X + static_cast<int>(std::lround(std::tan(radianti(g)) * ppr));
        if (x < 40 || x > LARGHEZZA_TELA - 40) continue;
        const int l = g % 5 == 0 ? 6 : 3;
        t.linea_verticale(x, RIGA_OCCHIO - l, RIGA_OCCHIO + l, c);
    }
    disegna_scala(t, v);
}

void Periscopio::disegna_scala(Tela& t, const VistaPeriscopio& v) const {
    const double ppr = pixel_per_radiante(v);
    // scala dei rilevamenti in alto: numeri ogni 10 gradi (basso ingrandimento) o 2 gradi (alto); con l'oculare la
    // scala copre tutta la larghezza, oltre il cerchio
    const bool fine = v.oculare.attivo ? v.oculare.campo_gradi <= 10.0 : v.alto_ingrandimento;
    const double semicampo = v.oculare.attivo ? gradi(std::atan(CENTRO_X / ppr))
                                              : (v.alto_ingrandimento ? CAMPO_ALTO_GRADI : CAMPO_BASSO_GRADI) / 2.0;
    t.rettangolo_pieno(0, 0, LARGHEZZA_TELA, 30, colori::NERO);
    const int passo = fine ? 2 : 10;
    const int primo = static_cast<int>(std::floor((v.rilevamento - semicampo) / passo)) * passo;
    for (int g = primo; g <= v.rilevamento + semicampo + passo; ++g) {
        const double rel = radianti(normalizza_180(g - v.rilevamento));
        const int x = CENTRO_X + static_cast<int>(std::lround(std::tan(rel) * ppr));
        if (x < 0 || x >= LARGHEZZA_TELA) continue;
        const bool numero = g % passo == 0;
        const bool tacca = fine || g % 5 == 0 || numero;
        if (!tacca) continue;
        t.linea_verticale(x, 24, 29, colori::GRIGIO);
        if (numero) {
            char testo[8];
            std::snprintf(testo, sizeof testo, "%03d", static_cast<int>(normalizza_360(g)));
            font_.scrivi(t, x - font_.larghezza(testo, 2) / 2, 0, testo, colori::GRIGIO, 2);
        }
    }
    t.linea_verticale(CENTRO_X, 22, 29, colori::GIALLO);   // indice del rilevamento corrente
}

double luce_fanale(double portata_m, double distanza_m, double visibilita_m, double luce_ambiente, double guadagno) {
    // la stessa luce che vede la vedetta dell'IA (nucleo/portata_luci): la K dell'Annesso I, la soglia del giorno e il
    // guadagno dell'ottica
    return luci::rapporto_soglia(portata_m, distanza_m, visibilita_m, luce_ambiente, guadagno);
}

}  // namespace m688
