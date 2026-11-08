#include "grafica/cielo.hpp"
#include "per_righe.hpp"

#include <algorithm>
#include <cmath>

#include "grafica/palette.hpp"
#include "nucleo/geometria.hpp"
#include "nucleo/sagoma_notte.hpp"
#include "nucleo/sole.hpp"

namespace m688 {

double GeometriaVista::elevazione_riga(int y) const { return elevazione_rad + std::atan((cy - (y + 0.5)) / ppr); }
double GeometriaVista::azimut_colonna(int x) const { return rilevamento_gradi + gradi(std::atan((x + 0.5 - cx) / ppr)); }
double GeometriaVista::riga_elevazione(double el) const { return cy - std::tan(el - elevazione_rad) * ppr; }

namespace cielo {

namespace {

constexpr int BAYER[4][4] = {{0, 8, 2, 10}, {12, 4, 14, 6}, {3, 11, 1, 9}, {15, 7, 13, 5}};
double soglia(int x, int y) { return (BAYER[y & 3][x & 3] + 0.5) / 16.0; }

double hash(int x, int y, int f) {
    std::uint32_t h = static_cast<std::uint32_t>(x) * 374761393u + static_cast<std::uint32_t>(y) * 668265263u +
                      static_cast<std::uint32_t>(f) * 2246822519u;
    h = (h ^ (h >> 13)) * 1274126177u;
    return ((h ^ (h >> 16)) & 0xffffff) / 16777216.0;
}

double liscio(double x, double y, int seme) {
    const double fx = std::floor(x), fy = std::floor(y);
    const int xi = static_cast<int>(fx), yi = static_cast<int>(fy);
    const double u = x - fx, v = y - fy, su = u * u * (3 - 2 * u), sv = v * v * (3 - 2 * v);
    const double a = hash(xi, yi, seme), b = hash(xi + 1, yi, seme), c = hash(xi, yi + 1, seme), d = hash(xi + 1, yi + 1, seme);
    return a + (b - a) * su + (c - a) * sv + (a - b - c + d) * su * sv;
}

double frattale(double x, double y, int seme) {
    double v = 0, a = 0.5;
    for (int o = 0; o < 4; ++o, x *= 2.03, y *= 2.03, a *= 0.5) v += a * liscio(x, y, seme + o);
    return v / 0.9375;
}

// soglia del rumore frattale (media ~0,5, scarto ~0,13) superata dalla frazione `copertura` dei punti
double soglia_copertura(double copertura) {
    const double p = std::clamp(copertura, 0.001, 0.999);
    return 0.5 - 0.13 * std::log(p / (1.0 - p)) / 1.7;
}

// pixel della riga che e' cielo (sopra l'orizzonte apparente)
bool cielo_alla_riga(const GeometriaVista& q, int y) {
    return q.elevazione_riga(y) > -depressione_orizzonte(q.quota_occhio_m);
}

// distanza dell'orizzontale di una riga di mare (come il mare della vista)
double distanza_mare(const GeometriaVista& q, int y) {
    const double delta = -q.elevazione_riga(y), dep = depressione_orizzonte(q.quota_occhio_m);
    if (delta <= dep) return distanza_orizzonte(q.quota_occhio_m);
    return std::min(q.quota_occhio_m / std::tan(delta - dep * 0.98), distanza_orizzonte(q.quota_occhio_m));
}

void punto(Tela& t, const GeometriaVista& q, int x, int y, std::uint8_t c) {
    const Rettangolo& a = q.area;
    if (x >= a.x && y >= a.y && x < a.x + a.w && y < a.y + a.h) t.pixel(x, y, c);
}

}  // namespace

void nubi(Tela& t, const GeometriaVista& q, const MeteoVista& m, double tempo_s) {
    if (m.nubi <= 0.01 && m.temporale <= 0.0) return;
    const double cop = std::clamp(std::max(m.nubi, m.temporale), 0.0, 1.0);
    const double s = soglia_copertura(cop);
    const double h = std::max(m.plafond_m, 150.0) - q.quota_occhio_m;
    // le nubi corrono col vento (da cui viene il vento: si spostano verso il lato opposto), a ~1,5 volte il vento al mare
    const double vd = radianti(m.vento_da_gradi + 180.0), v = 1.5 * m.vento_m_s * tempo_s;
    const double dx = v * std::sin(vd), dy = v * std::cos(vd);
    const double scala = m.plafond_m < 600.0 ? 3500.0 : 1800.0;   // strati bassi piu' larghi e lisci (STIMA)
    const double fitte = std::clamp((cop - 0.5) / 0.3, 0.0, 1.0), copertura_velo = cop * fitte * fitte * (3.0 - 2.0 * fitte);
    const Rettangolo& a = q.area;
    per_righe(a.y, a.y + a.h, [&](int y) {
        const double el = q.elevazione_riga(y);
        if (el <= 0.0005) return;
        const double d = h / std::tan(el);
        const double lontano = 1.0 - std::exp(-d / 25000.0);   // verso l'orizzonte le nubi si confondono nella foschia
        // oltre i 60 km il disegno sfuma (fermare d farebbe colonne verticali): radente, il coperto resta un velo uniforme, le
        // nubi sparse spariscono nel cielo (un velo rado sarebbe il retino, una griglia di puntini)
        const double oltre = std::clamp((d - 60000.0) / 120000.0, 0.0, 1.0), velo = oltre * oltre * (3.0 - 2.0 * oltre);
        for (int x = a.x; x < a.x + a.w; ++x) {
            if (!cielo_alla_riga(q, y)) continue;
            const double az = radianti(q.azimut_colonna(x));
            const double px = d * std::sin(az) - dx, py = d * std::cos(az) - dy;
            const double n = frattale(px / scala, py / scala, 211);
            const double densita = std::clamp((n - s) / 0.07, 0.0, 1.0) * (1.0 - velo) + copertura_velo * velo;
            if (densita < soglia(x, y) * 0.999) continue;
            // la base e' scura dove la nube e' spessa e sotto i cumulonembi; lontano schiarisce verso la foschia
            // bordi chiari, cuore spesso piu' scuro: il chiaroscuro dei cumuli
            const double spessa = (n - s) * (1.0 - velo) + 0.15 * velo;
            double c = NUBE + 5.0 * (1.0 - std::clamp(spessa / 0.15, 0.0, 1.0)) - 2.0 * std::clamp(spessa / 0.3, 0.0, 1.0) -
                       5.0 * m.temporale * densita;
            c += lontano * (FOSCHIA - c) * 0.8;
            const double b = std::floor(c);
            t.pixel(x, y, static_cast<std::uint8_t>(std::clamp(b + ((c - b) > soglia(x + 1, y) ? 1.0 : 0.0), 144.0, 158.0)));
        }
    });
    // cortine di pioggia sotto i cumulonembi, a 8-25 km: colonne retinate dalla base all'orizzonte
    if (m.temporale > 0.05 || m.pioggia_mm_h > 8.0) {
        const double forza = std::max(m.temporale, std::min(1.0, m.pioggia_mm_h / 30.0));
        for (int x = a.x; x < a.x + a.w; ++x) {
            const double az = q.azimut_colonna(x);
            const double c = liscio(az / 6.0, 0.5, 223);   // cortine larghe qualche grado, ferme nel mondo
            if (c < 1.0 - 0.45 * forza) continue;
            const double d = 8000.0 + 17000.0 * liscio(az / 3.0, 7.5, 227);
            const int y0 = static_cast<int>(q.riga_elevazione(std::atan(h / d)));
            const int y1 = static_cast<int>(q.riga_elevazione(-depressione_orizzonte(q.quota_occhio_m)));
            for (int y = std::max(y0, a.y); y <= std::min(y1, a.y + a.h - 1); ++y) {
                const double f = 0.55 * (c - (1.0 - 0.45 * forza)) / 0.45 * (0.6 + 0.4 * (y - y0) / std::max(1, y1 - y0));
                if (f > soglia(x, y)) t.pixel(x, y, PIOGGIA);
            }
        }
    }
}

void foschia(Tela& t, const GeometriaVista& q, const MeteoVista& m, double visibilita_m) {
    const bool nebbia = m.nebbia == NebbiaVista::avvezione || m.nebbia == NebbiaVista::radiazione;
    if (visibilita_m >= 40000.0 && !nebbia) return;
    const double v = std::max(visibilita_m, 30.0);
    // spessore dello strato che sbianca il cielo basso: foschia ~1 km, nebbia 50-200 m (STIMA); col cielo coperto la
    // base delle nubi piu' bassa dello strato si vede attraverso la foschia che le sta sotto
    const double strato = nebbia ? 150.0 : (std::max(m.nubi, m.temporale) >= 0.6 ? std::clamp(m.plafond_m, 150.0, 1000.0) : 1000.0);
    const Rettangolo& a = q.area;
    for (int y = a.y; y < a.y + a.h; ++y) {
        const double el = q.elevazione_riga(y);
        double r;
        if (cielo_alla_riga(q, y)) r = std::min(strato / std::max(std::sin(std::max(el, 0.0)), strato / distanza_orizzonte(q.quota_occhio_m) / 3.0), 80000.0);
        else r = distanza_mare(q, y);
        const double f = 1.0 - std::exp(-3.912 * r / v);
        if (f < 0.02) continue;
        for (int x = a.x; x < a.x + a.w; ++x)
            if (f > soglia(x, y)) t.pixel(x, y, FOSCHIA);
    }
}

void fulmini(Tela& t, const GeometriaVista& q, const MeteoVista& m, double tempo_s) {
    for (const auto& l : m.lampi) {
        if (l.distanza_m > LAMPI_BAGLIORE_M || tempo_s < l.t) continue;
        const int k = static_cast<int>(std::floor((tempo_s - l.t) / INTERVALLO_COLPI_S));
        if (k >= l.colpi || tempo_s - (l.t + k * INTERVALLO_COLPI_S) > COLPO_S) continue;   // fra un colpo e l'altro
        const double rel = radianti(normalizza_180(l.rilevamento_gradi - q.rilevamento_gradi));
        if (std::fabs(rel) > radianti(80.0)) continue;
        const double x0 = q.cx + std::tan(rel) * q.ppr;
        const double base = std::atan((std::max(m.plafond_m, 300.0) - q.quota_occhio_m) / l.distanza_m);
        const double mare = -(q.quota_occhio_m / l.distanza_m + l.distanza_m / (2.0 * RAGGIO_TERRA_RIFRAZIONE));
        const double ya = q.riga_elevazione(base);
        if (l.nube_mare && l.distanza_m <= LAMPI_CANALE_M) {
            // canale: spezzata dalla base della nube al mare (sotto l'orizzonte: fino all'orizzonte), con 1-3 rami
            const double yb = std::min(q.riga_elevazione(mare), q.riga_elevazione(-depressione_orizzonte(q.quota_occhio_m)));
            const int passi = std::max(4, static_cast<int>((yb - ya) / 7.0));
            const int spessore = l.distanza_m < 3000.0 ? 2 : 1;
            double x = x0;
            const int rami = 1 + static_cast<int>(3.0 * hash(static_cast<int>(l.seme), 1, 301));
            for (int i = 0; i < passi; ++i) {
                const double y1 = ya + (yb - ya) * i / passi, y2 = ya + (yb - ya) * (i + 1) / passi;
                const double x2 = x + (hash(static_cast<int>(l.seme), i, 303) - 0.5) * 10.0;
                const int n = static_cast<int>(std::max(std::fabs(x2 - x), y2 - y1)) + 1;
                for (int j = 0; j <= n; ++j)
                    for (int s = 0; s < spessore; ++s)
                        punto(t, q, static_cast<int>(x + (x2 - x) * j / n) + s, static_cast<int>(y1 + (y2 - y1) * j / n), colori::BIANCO);
                for (int r = 0; r < rami; ++r)   // un ramo parte da questo passo e scende di lato
                    if (static_cast<int>(hash(static_cast<int>(l.seme), r, 305) * passi) == i) {
                        const double lato = hash(static_cast<int>(l.seme), r, 307) < 0.5 ? -1.0 : 1.0;
                        const int lungo = 6 + static_cast<int>(14.0 * hash(static_cast<int>(l.seme), r, 309));
                        for (int j = 0; j < lungo; ++j)
                            punto(t, q, static_cast<int>(x2 + lato * (j + hash(static_cast<int>(l.seme) + j, r, 311) * 2.0)),
                                  static_cast<int>(y2 + j * 0.8), colori::BIANCO);
                    }
                x = x2;
            }
        } else {
            // bagliore dietro le nubi: chiazza retinata, piu' fitta al centro e piu' debole da lontano
            const double forza = 0.6 / (1.0 + l.distanza_m / 30000.0) * std::pow(0.75, k);
            const int raggio = std::clamp(static_cast<int>(q.ppr * 3000.0 / l.distanza_m), 12, 70);
            for (int y = static_cast<int>(ya) - raggio; y <= static_cast<int>(ya) + raggio / 2; ++y)
                for (int x = static_cast<int>(x0) - raggio; x <= static_cast<int>(x0) + raggio; ++x) {
                    if (!cielo_alla_riga(q, y)) continue;
                    const double d = std::hypot(x - x0, (y - ya) * 1.6) / raggio;
                    if (d < 1.0 && forza * (1.0 - d) > soglia(x, y)) punto(t, q, x, y, FOSCHIA);
                }
        }
    }
}

void fumo_di_mare(Tela& t, const GeometriaVista& q, const MeteoVista& m, double tempo_s) {
    if (m.nebbia != NebbiaVista::fumo_di_mare || m.strato_nebbia_m <= 0.0) return;
    const double h = q.quota_occhio_m, s = m.strato_nebbia_m;
    const double dep = depressione_orizzonte(h), d_or = distanza_orizzonte(h);
    // cima dello strato vista all'orizzonte e fondo un poco sotto l'orizzonte
    const double y_cima = q.riga_elevazione(std::atan((s - h) / d_or));
    const double y_oriz = q.riga_elevazione(-dep);
    const double y_fondo = y_oriz + std::max(4.0, 0.4 * (y_oriz - y_cima));
    const Rettangolo& a = q.area;
    for (int y = std::max(a.y, static_cast<int>(std::floor(y_cima)) - 2); y <= std::min(a.y + a.h - 1, static_cast<int>(y_fondo)); ++y) {
        const double alto = std::clamp((y_oriz - y) / std::max(1.0, y_oriz - y_cima), 0.0, 1.0);   // 0 al mare, 1 in cima
        for (int x = a.x; x < a.x + a.w; ++x) {
            // colonne di vapore ferme nel mondo che salgono e si sfrangiano verso l'alto
            const double az = q.azimut_colonna(x);
            const double c = frattale(az * 2.5, (y - tempo_s * 6.0) * 0.12, 241);
            const double f = (1.0 - alto) * 0.9 + c * 0.5 - 0.35;
            if (f > soglia(x, y)) t.pixel(x, y, FOSCHIA);
        }
    }
}

void precipitazione(Tela& t, const GeometriaVista& q, const MeteoVista& m, double tempo_s) {
    const Rettangolo& a = q.area;
    // vento di traverso rispetto alla vista: la pioggia pende (caduta ~7 m/s), la neve deriva (caduta ~1,2 m/s)
    const double traverso = m.vento_m_s * std::sin(radianti(m.vento_da_gradi + 180.0 - q.rilevamento_gradi));
    if (m.pioggia_mm_h > 0.05) {
        const int n = std::clamp(static_cast<int>(m.pioggia_mm_h * 22.0), 6, 500);
        const double pendenza = std::clamp(traverso / 7.0, -1.2, 1.2);
        const int lungo = m.pioggia_mm_h > 8.0 ? 14 : 9;
        for (int i = 0; i < n; ++i) {
            const double vita = 0.35, fase = hash(i, 0, 401) * vita, giro = (tempo_s + fase) / vita;
            const int nascita = static_cast<int>(std::floor(giro));
            const double eta = giro - nascita;
            const double x0 = a.x + a.w * hash(i, nascita, 403), y0 = a.y - 20 + (a.h + 40) * eta;
            for (int j = 0; j < lungo; j += 1) {
                const int x = static_cast<int>(x0 + pendenza * j), y = static_cast<int>(y0 + j);
                if ((x + y) & 1) punto(t, q, x, y, PIOGGIA);
            }
        }
    }
    if (m.neve_mm_h > 0.05) {
        const int n = std::clamp(static_cast<int>(m.neve_mm_h * 60.0), 10, 600);
        for (int i = 0; i < n; ++i) {
            const double vita = 3.0, fase = hash(i, 0, 411) * vita, giro = (tempo_s + fase) / vita;
            const int nascita = static_cast<int>(std::floor(giro));
            const double eta = giro - nascita;
            const double x = a.x + a.w * hash(i, nascita, 413) + traverso * 18.0 * eta + 4.0 * std::sin(tempo_s * 2.0 + i);
            const double y = a.y + a.h * eta;
            punto(t, q, static_cast<int>(x), static_cast<int>(y), NEVE);
            if (hash(i, nascita, 415) < 0.3) punto(t, q, static_cast<int>(x) + 1, static_cast<int>(y), NEVE);
        }
    }
}

double luce_sotto_le_nubi(double luce, const MeteoVista& m) {
    if (!m.attivo) return luce;
    const double nubi = std::clamp(m.nubi, 0.0, 1.0);
    return luce * std::clamp(1.0 - 0.45 * std::pow(nubi, 1.5) - 0.3 * std::clamp(m.temporale, 0.0, 1.0), 0.2, 1.0);
}

double lampo(const MeteoVista& m, double tempo_s, double luce) {
    double s = 0.0;
    for (const auto& l : m.lampi) {
        if (tempo_s < l.t || l.distanza_m > LAMPI_BAGLIORE_M) continue;
        const int k = static_cast<int>(std::floor((tempo_s - l.t) / INTERVALLO_COLPI_S));
        if (k >= l.colpi) continue;
        const double dentro = tempo_s - (l.t + k * INTERVALLO_COLPI_S);
        if (dentro > COLPO_S) continue;
        // i colpi successivi al 70% del precedente; da vicino illumina tutto, da lontano solo un bagliore
        s += std::pow(0.7, k) * (1.0 / (1.0 + std::pow(l.distanza_m / 8000.0, 2.0)) + 0.12 * std::exp(-l.distanza_m / 60000.0)) *
             (l.nube_mare ? 1.0 : 0.6);
    }
    return std::clamp(s, 0.0, 1.0) * (1.0 - 0.75 * std::clamp(luce, 0.0, 1.0));
}

namespace {
// i colori della luna sono quelli EGA dei fanali, fuori dagli INDICI_VISTA: la palette della notte non li scurisce
constexpr std::uint8_t LUNA = 15, LUNA_BORDO = 7, LUNA_BASSA = 14, LUNA_BASSA_BORDO = 6;
constexpr double DIAMETRO_LUNA_GRADI = 0.52;
constexpr double LUX_LUNA_ALTA = 0.15;   // sopra, la scia e' piena (la luna piena alta da' ~0,25 lux)
constexpr double LUX_ADATTAMENTO_MIN = 0.003, LUX_LUNA_PIENA_VISTA = 0.25, LUCE_LUNA_PIENA = 0.22;
bool del_mare(std::uint8_t c) { return c >= 96 && c <= 143; }
}  // namespace

double luce_della_notte(double lux) {
    if (!(lux > 0.0)) return 0.0;
    // il crepuscolo con lo stesso illuminamento (lux_sole cresce con l'altezza del Sole)
    double lo = -18.0, hi = 90.0;
    double crepuscolo;
    if (lux_sole(hi) <= lux) {
        crepuscolo = luce_ambiente(hi);
    } else {
        for (int k = 0; k < 40; ++k) {
            const double m = 0.5 * (lo + hi);
            (lux_sole(m) < lux ? lo : hi) = m;
        }
        crepuscolo = luce_ambiente(0.5 * (lo + hi));
    }
    // l'occhio adattato al buio: sotto la luna si legge molto piu' di quanto direbbe il crepuscolo equivalente (che da'
    // alla luna piena appena 0,03-0,07). Da LUX_ADATTAMENTO_MIN (stelle, niente) alla luna piena alta, in scala
    // logaritmica fino a LUCE_LUNA_PIENA (STIMA: sotto il crepuscolo col Sole a -4 gradi, 0,3, che si legge bene)
    const double a = std::clamp(std::log10(lux / LUX_ADATTAMENTO_MIN) / std::log10(LUX_LUNA_PIENA_VISTA / LUX_ADATTAMENTO_MIN), 0.0, 1.0);
    return std::max(crepuscolo, LUCE_LUNA_PIENA * a);
}

void luna(Tela& t, const GeometriaVista& q, const CieloNotte& c, double luce) {
    if (!c.luna_su) return;
    const double rel = radianti(normalizza_180(c.luna_azimut - q.rilevamento_gradi));
    if (std::fabs(rel) > radianti(80.0)) return;
    const double xc = q.cx + std::tan(rel) * q.ppr, yc = q.riga_elevazione(radianti(c.luna_elevazione));
    const double r = std::max(1.0, 0.5 * radianti(DIAMETRO_LUNA_GRADI) * q.ppr);
    const bool bassa = c.luna_elevazione < 4.0;   // vicino all'orizzonte e' gialla
    const std::uint8_t nucleo = luce > 0.3 ? LUNA_BORDO : bassa ? LUNA_BASSA : LUNA, bordo = bassa ? LUNA_BASSA_BORDO : LUNA_BORDO;
    const double alone = luce < 0.3 ? 4.0 * r + 3.0 : r;   // di notte un alone rado attorno al disco
    // la fase: il lembo illuminato verso luna_angolo_lembo (0 in alto, 90 a destra, sullo schermo); il terminatore e'
    // un'ellisse col semiasse 2f - 1 lungo quella direzione (f la frazione illuminata)
    const double f = std::clamp(c.luna_illuminata, 0.0, 1.0), k = 2.0 * f - 1.0;
    const double th = radianti(c.luna_angolo_lembo), ux = std::sin(th), uy = -std::cos(th);
    const Rettangolo& a = q.area;
    const int x0 = std::max(a.x, static_cast<int>(std::floor(xc - alone))), x1 = std::min(a.x + a.w - 1, static_cast<int>(std::ceil(xc + alone)));
    const int y0 = std::max(a.y, static_cast<int>(std::floor(yc - alone))), y1 = std::min(a.y + a.h - 1, static_cast<int>(std::ceil(yc + alone)));
    for (int y = y0; y <= y1; ++y) {
        if (!cielo_alla_riga(q, y)) continue;
        for (int x = x0; x <= x1; ++x) {
            const double dx = x + 0.5 - xc, dy = y + 0.5 - yc, d = std::hypot(dx, dy);
            if (d <= r) {
                const double a = (dx * ux + dy * uy) / r, b = (-dx * uy + dy * ux) / r;   // lungo il lembo e di traverso
                if (a >= -k * std::sqrt(std::max(0.0, 1.0 - b * b))) t.pixel(x, y, d > r - 1.0 && r > 1.5 ? bordo : nucleo);
            } else if (d <= alone && hash(x, y, 977) < 0.12 * f * (1.0 - (d - r) / (alone - r))) {
                t.pixel(x, y, LUNA_BORDO);
            }
        }
    }
}

void scia_lunare(Tela& t, const GeometriaVista& q, const CieloNotte& c, double luce, double altezza_onde_m, double tempo_s) {
    if (!c.luna_su || luce > 0.35 || c.luna_elevazione > 70.0) return;
    const double e = radianti(std::max(c.luna_elevazione, 0.0));
    const double forza = std::clamp(c.lux / LUX_LUNA_ALTA, 0.05, 1.0);   // la fase, l'altezza e le nubi
    // pendenza media delle faccette d'onda (STIMA, alla Cox e Munk: piu' larga col mare formato)
    const double pendenza = std::clamp(0.06 + 0.05 * std::max(altezza_onde_m, 0.0), 0.06, 0.3);
    const int f = static_cast<int>(std::floor(tempo_s * 4.0));   // lo scintillio cambia 4 volte al secondo
    const Rettangolo& a = q.area;
    for (int y = a.y; y < a.y + a.h; ++y) {
        if (cielo_alla_riga(q, y)) continue;
        // la faccetta che rimanda la luna all'occhio da questa depressione e' inclinata di (d - e) / 2
        const double sv = (-q.elevazione_riga(y) - e) / (2.0 * pendenza);
        const double wv = std::exp(-0.5 * sv * sv);
        if (wv < 0.01) continue;
        for (int x = a.x; x < a.x + a.w; ++x) {
            if (!del_mare(t.at(x, y))) continue;   // solo il mare: non la terra, il ghiaccio, le navi
            // di lato la scia e' piu' stretta che in profondita' (le faccette inclinate di traverso rimandano meno)
            const double sa = std::sin(radianti(normalizza_180(q.azimut_colonna(x) - c.luna_azimut))) / (1.2 * pendenza);
            const double w = forza * wv * std::exp(-0.5 * sa * sa);
            const double h = hash(x, y, f);
            if (h < 0.18 * w) t.pixel(x, y, h < 0.05 * w ? LUNA : LUNA_BORDO);   // scintille rade, le piu' vive bianche
        }
    }
}

}  // namespace cielo

}  // namespace m688
