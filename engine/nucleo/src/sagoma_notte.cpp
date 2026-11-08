#include "nucleo/sagoma_notte.hpp"

#include <algorithm>
#include <array>
#include <cmath>

#include "nucleo/geometria.hpp"

namespace m688 {

namespace {

constexpr double LUX_STELLE = 0.002;              // cielo sereno senza Luna
constexpr double LUX_LUNA_PIENA = 0.25;           // la piena allo zenit
constexpr double NUBI_LUNA = 0.85, NUBI_SOLE = 0.75;   // quanto toglie il coperto (STIMA)
constexpr double PORTATA_RIF_M = 3000.0;          // la sagoma di riferimento con le stelle e 30 km di visibilita'
constexpr double VISIBILITA_RIF_M = 30000.0;
constexpr double LUCE_ESP = 0.143;                // la Luna piena alta (125 volte le stelle) raddoppia
constexpr double INGRANDIMENTO_ESP = 0.356;       // il 7x50 raddoppia
constexpr double NON_ALLERTATA = 0.7;             // come la testa del periscopio (nucleo/periscopi)
constexpr double CONTROLUCE = 1.5;                // la Luna bassa dietro la nave
constexpr double LUNA_BASSA = 30.0, LUNA_DIETRO = 30.0;   // gradi
constexpr double KOSCHMIEDER = 3.912;
constexpr double ORIZZONTE_VISIVO_M = 3843.0;     // come nucleo/periscopi
constexpr double TASSO_NAVE_S = 0.1;              // come la vedetta d'una nave in nucleo/periscopi

double seno(double g) { return std::sin(radianti(g)); }
double coseno(double g) { return std::cos(radianti(g)); }

// i giorni dal J2000.0 (12 TT del 1/1/2000; la differenza fra TT e UT, un minuto, non conta qui)
double giorni_j2000(const DataOra& t) {
    int a = t.anno, m = t.mese;
    if (m <= 2) { --a; m += 12; }
    const int b = 2 - a / 100 + a / 400;   // gregoriano
    const double jd = std::floor(365.25 * (a + 4716)) + std::floor(30.6001 * (m + 1)) + t.giorno + b - 1524.5 + t.ore_utc / 24.0;
    return jd - 2451545.0;
}

const Sagoma& riferimento() {
    static const Sagoma s{130.0, 12.0, 30.0, 0.0};
    return s;
}

// la soglia di contrasto per la sagoma scura sul cielo: 0,02 di giorno (Koschmieder), che sale al buio fino a 0,3
// (STIMA: 0,28 con le stelle, 0,1 con la piena); la foschia pesa ln(1 / 0,02) / ln(1 / soglia) volte di piu'
double peso_foschia(double lux) {
    const double soglia = std::min(0.3, 0.02 * std::pow(1000.0 / std::max(lux, 1e-6), 0.2));
    return KOSCHMIEDER / std::log(1.0 / std::max(soglia, 0.02));
}

// la soluzione di (d / r)^2 exp(sigma d) = 1 in (0, r]: la funzione cresce con d, bisezione sul logaritmo. Mai oltre
// il limite del contrasto, 3,912 / sigma (con sigma gia' pesato dalla soglia): una sagoma grande non si vede oltre la
// distanza a cui il suo contrasto scende sotto la soglia (di giorno la visibilita' meteorologica)
double con_foschia(double r, double sigma) {
    if (!(r > 0.0)) return 0.0;
    if (sigma <= 0.0) return r;
    const double limite = KOSCHMIEDER / sigma;
    double lo = std::log(r) - 30.0, hi = std::log(r);
    for (int k = 0; k < 60; ++k) {
        const double m = 0.5 * (lo + hi);
        (2.0 * (m - std::log(r)) + sigma * std::exp(m) < 0.0 ? lo : hi) = m;
    }
    return std::min(std::exp(0.5 * (lo + hi)), limite);
}

}  // namespace

PosizioneLuna posizione_luna(const DataOra& t, double latitudine, double longitudine) {
    const double d = giorni_j2000(t);
    // la Luna: longitudine media, anomalia media, argomento della latitudine; i termini maggiori di longitudine e latitudine
    const double l = 218.316 + 13.176396 * d, mm = 134.963 + 13.064993 * d, f = 93.272 + 13.229350 * d;
    const double lambda = l + 6.289 * seno(mm), beta = 5.128 * seno(f);
    // il Sole, per la fase
    const double ms = 357.529 + 0.98560028 * d;
    const double lambda_s = 280.459 + 0.98564736 * d + 1.915 * seno(ms) + 0.020 * seno(2.0 * ms);
    // all'equatore, poi all'orizzonte
    const double eps = 23.439 - 0.0000004 * d;
    const double ra = gradi(std::atan2(seno(lambda) * coseno(eps) - std::tan(radianti(beta)) * seno(eps), coseno(lambda)));
    const double dec = gradi(std::asin(std::clamp(seno(beta) * coseno(eps) + coseno(beta) * seno(eps) * seno(lambda), -1.0, 1.0)));
    const double gmst = 280.46061837 + 360.98564736629 * d;
    const double h = gmst + longitudine - ra;   // angolo orario
    PosizioneLuna p;
    p.elevazione = gradi(std::asin(std::clamp(seno(latitudine) * seno(dec) + coseno(latitudine) * coseno(dec) * coseno(h), -1.0, 1.0)));
    p.azimut = normalizza_360(gradi(std::atan2(-coseno(dec) * seno(h), seno(dec) * coseno(latitudine) - coseno(dec) * coseno(h) * seno(latitudine))));
    const double elongazione = gradi(std::acos(std::clamp(coseno(beta) * coseno(lambda - lambda_s), -1.0, 1.0)));
    p.angolo_fase = 180.0 - elongazione;
    p.illuminata = 0.5 * (1.0 + coseno(p.angolo_fase));
    // il lembo illuminato: la direzione del Sole sul disco, nel piano tangente alla sfera celeste nella Luna (x est,
    // y nord, z zenit); «a destra» e' M x Z, «in alto» Z meno la sua parte lungo M (stessa norma, cos dell'altezza)
    const double ra_s = gradi(std::atan2(seno(lambda_s) * coseno(eps), coseno(lambda_s)));
    const double dec_s = gradi(std::asin(std::clamp(seno(eps) * seno(lambda_s), -1.0, 1.0)));
    const double h_s = gmst + longitudine - ra_s;
    const double el_s = gradi(std::asin(std::clamp(seno(latitudine) * seno(dec_s) + coseno(latitudine) * coseno(dec_s) * coseno(h_s), -1.0, 1.0)));
    const double az_s = gradi(std::atan2(-coseno(dec_s) * seno(h_s), seno(dec_s) * coseno(latitudine) - coseno(dec_s) * coseno(h_s) * seno(latitudine)));
    const auto verso = [](double el, double az) {
        return std::array<double, 3>{coseno(el) * seno(az), coseno(el) * coseno(az), seno(el)};
    };
    const auto m = verso(p.elevazione, p.azimut), s = verso(el_s, az_s);
    const double sm = s[0] * m[0] + s[1] * m[1] + s[2] * m[2];
    const double tg[3] = {s[0] - sm * m[0], s[1] - sm * m[1], s[2] - sm * m[2]};
    const double destra = tg[0] * m[1] - tg[1] * m[0];      // tg . (M x Z), con M x Z = (My, -Mx, 0)
    const double alto = tg[2];                               // tg . (Z - Mz M), e tg . M = 0
    p.angolo_lembo = normalizza_360(gradi(std::atan2(destra, alto)));
    return p;
}

double lux_sole(double e) {
    // log10 dei lux col cielo sereno per l'altezza del Sole (tavole d'illuminamento naturale, arrotondate)
    static constexpr double H[] = {-18.0, -12.0, -6.0, 0.0, 10.0, 30.0, 90.0};
    static constexpr double LG[] = {-4.0, -2.1, 0.5, 2.85, 4.2, 4.8, 5.0};
    if (e <= H[0]) return 0.0;
    for (int k = 1; k < 7; ++k)
        if (e <= H[k]) return std::pow(10.0, LG[k - 1] + (LG[k] - LG[k - 1]) * (e - H[k - 1]) / (H[k] - H[k - 1]));
    return std::pow(10.0, LG[6]);
}

CieloNotte cielo_notte(const DataOra& t, double latitudine, double longitudine, double nubi_ottavi) {
    const double c = std::clamp(nubi_ottavi / 8.0, 0.0, 1.0);
    CieloNotte cielo;
    double luna = 0.0;
    const PosizioneLuna p = posizione_luna(t, latitudine, longitudine);
    if (p.elevazione > 0.0) {
        const double i = std::abs(p.angolo_fase);
        const double fase = std::pow(10.0, -0.4 * (0.026 * i + 4e-9 * i * i * i * i));
        const double s = std::max(seno(p.elevazione), 0.05);
        luna = LUX_LUNA_PIENA * fase * s * std::pow(10.0, -0.4 * 0.25 * (1.0 / s - 1.0));   // l'estinzione in aria
        cielo.luna_su = true;
        cielo.luna_elevazione = p.elevazione;
        cielo.luna_azimut = p.azimut;
    }
    cielo.luna_illuminata = p.illuminata;
    cielo.luna_angolo_lembo = p.angolo_lembo;
    cielo.lux = (LUX_STELLE + luna) * (1.0 - NUBI_LUNA * c) +
                lux_sole(elevazione_sole(t, latitudine, longitudine)) * (1.0 - NUBI_SOLE * c);
    return cielo;
}

CieloNotte cielo_notte(double luce, double nubi_ottavi) {
    const double c = std::clamp(nubi_ottavi / 8.0, 0.0, 1.0);
    CieloNotte cielo;
    double sole = 0.0;
    if (luce > 0.0) {   // il contrario del raccordo di luce_ambiente: t^2 (3 - 2t) = luce, e l'altezza 22 t - 12
        double lo = 0.0, hi = 1.0;
        for (int k = 0; k < 40; ++k) {
            const double m = 0.5 * (lo + hi);
            (m * m * (3.0 - 2.0 * m) < luce ? lo : hi) = m;
        }
        sole = lux_sole(22.0 * 0.5 * (lo + hi) - 12.0);
    }
    cielo.lux = LUX_STELLE * (1.0 - NUBI_LUNA * c) + sole * (1.0 - NUBI_SOLE * c);
    return cielo;
}

Sagoma sagoma_da_tipo(const std::string& tipo, double lunghezza_m) {
    const double l = std::max(lunghezza_m, 5.0);
    if (tipo == "sottomarino") return {l, 1.5 + 70.0 / l, 9.0, 0.0};   // scafo a 1,5 m e una vela di 10 x 7 m
    if (tipo == "peschereccio") return {l, 0.2 * l, std::min(0.4 * l, 20.0), 0.0};
    if (tipo == "militare") return {l, 0.09 * l, std::min(0.23 * l, 50.0), 0.0};
    return {l, 0.08 * l + 2.0, std::min(0.2 * l + 10.0, 45.0), 0.0};   // mercantile
}

double dimensione_sagoma_m(const Sagoma& s, double aspetto) {
    const double b = s.larghezza_m > 0.0 ? s.larghezza_m : s.lunghezza_m / 8.0;
    const double vista = s.lunghezza_m * std::abs(seno(aspetto)) + b * std::abs(coseno(aspetto));
    return std::sqrt(std::max(s.altezza_media_m, 0.0) * vista);
}

double portata_sagoma_notte_m(const Sagoma& s, double aspetto, const Osservatore& o, const CondizioniVista& c,
                              const CieloNotte& cielo, double rilevamento, double ingrandimento) {
    static const double dim_rif = dimensione_sagoma_m(riferimento(), 90.0);
    const double dim = dimensione_sagoma_m(s, aspetto);
    if (!(dim > 0.0)) return 0.0;
    double lux = cielo.lux;
    bool luna = cielo.luna_su;
    switch (o.notte) {
        case ModoVista::bassa_luminosita:
        case ModoVista::tv: lux *= 30.0; break;
        case ModoVista::intensificatore: lux *= 1000.0; break;
        case ModoVista::termico: lux = std::max(lux, 1.0); luna = false; break;
        case ModoVista::giorno: break;
    }
    // in aria limpida: la taratura toglie la foschia di riferimento (le stelle e 30 km)
    static const double limpida = PORTATA_RIF_M * std::exp(0.5 * peso_foschia(LUX_STELLE) * KOSCHMIEDER / VISIBILITA_RIF_M * PORTATA_RIF_M);
    double r = limpida * dim / dim_rif * std::pow(std::max(lux, 1e-6) / LUX_STELLE, LUCE_ESP) *
               std::pow(std::max(ingrandimento, 1.0), INGRANDIMENTO_ESP) * (o.allertato ? 1.0 : NON_ALLERTATA);
    if (luna && cielo.luna_elevazione < LUNA_BASSA && std::abs(normalizza_180(cielo.luna_azimut - rilevamento)) < LUNA_DIETRO)
        r *= CONTROLUCE;
    r = con_foschia(r, peso_foschia(lux) * KOSCHMIEDER / std::max(c.visibilita_m, 1.0));
    return std::min(r, ORIZZONTE_VISIVO_M * (std::sqrt(std::max(o.altezza_occhio_m, 0.0)) + std::sqrt(std::max(s.altezza_max_m, 0.0))));
}

double probabilita_sagoma_notte(const Sagoma& s, double aspetto, const Osservatore& o, const CondizioniVista& c,
                                const CieloNotte& cielo, double rilevamento, double distanza_m, double dt,
                                double ingrandimento) {
    if (dt <= 0.0) return 0.0;
    const double r = portata_sagoma_notte_m(s, aspetto, o, c, cielo, rilevamento, ingrandimento);
    const double orizzonte = ORIZZONTE_VISIVO_M * (std::sqrt(std::max(o.altezza_occhio_m, 0.0)) + std::sqrt(std::max(s.altezza_max_m, 0.0)));
    if (!(r > 0.0) || distanza_m > 2.0 * r || distanza_m > orizzonte) return 0.0;
    const double tasso = TASSO_NAVE_S / (1.0 + std::pow(distanza_m / r, 4.0));
    return 1.0 - std::exp(-tasso * dt);
}

}  // namespace m688
