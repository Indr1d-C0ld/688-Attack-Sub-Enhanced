#include "nucleo/portata_luci.hpp"

#include <algorithm>
#include <cmath>

namespace m688::luci {

namespace {

constexpr double MIGLIO_M = 1852.0;
constexpr double KOSCHMIEDER = 3.912;             // -ln(0,02), come nucleo/meteo
constexpr double ORIZZONTE_VISIVO_M = 3843.0;     // come nucleo/periscopi: 2,075 miglia per radice di metro
constexpr double ALTEZZA_MIN_M = 0.5;             // un fanale sul pelo dell'acqua resta un po' sopra
constexpr double META_PORTATA = 0.8;              // dove il tasso e' la meta' (STIMA)
constexpr double LAMPI = 0.5;                     // il faro a lampi rispetto a una luce fissa (STIMA)

bool acceso_nello_stato(const Fanale& f, Stato s) {
    return s != Stato::spente && (f.stati & (1u << static_cast<int>(s))) != 0;
}

// La distanza dove I exp(-sigma r) / r^2 scende a e: la funzione cala con r, quindi bisezione sul logaritmo fra 1 m e la
// portata senza foschia
double allard(double intensita, double sigma, double e) {
    if (!(intensita > 0.0) || !(e > 0.0)) return 0.0;
    const double ln = std::log(intensita / e);
    if (ln <= 0.0) return 0.0;   // non arriva nemmeno a un metro
    double lo = 0.0, hi = 0.5 * ln;   // log dei metri: senza foschia r = sqrt(I / e)
    if (sigma <= 0.0) return std::exp(hi);
    for (int k = 0; k < 60; ++k) {
        const double m = 0.5 * (lo + hi);
        (ln - 2.0 * m - sigma * std::exp(m) > 0.0 ? lo : hi) = m;
    }
    return std::exp(0.5 * (lo + hi));
}

}  // namespace

double intensita_cd(double portata_m) {
    if (portata_m <= 0.0) return 0.0;
    const double d = portata_m / MIGLIO_M;
    return 3.43e6 * SOGLIA_NOTTE_LUX * d * d * std::pow(K_ANNESSO, -d);
}

double portata_nominale_m(double intensita_cd) {
    return allard(intensita_cd, -std::log(K_ANNESSO) / MIGLIO_M, SOGLIA_NOTTE_LUX);
}

double soglia_lux(double luce, bool allertato, double guadagno) {
    const double giorno = std::clamp((luce - 0.1) / 0.9, 0.0, 1.0);
    return SOGLIA_NOTTE_LUX * (1.0 + 3000.0 * giorno * giorno) * (allertato ? 1.0 : NON_ALLERTATA) / std::max(guadagno, 1e-6);
}

double guadagno_sensore(ModoVista sensore) {
    switch (sensore) {
        case ModoVista::bassa_luminosita:
        case ModoVista::tv: return 3.0;
        case ModoVista::intensificatore: return 10.0;
        case ModoVista::giorno:
        case ModoVista::termico: return 1.0;
    }
    return 1.0;
}

double portata_luminosa_m(double intensita_cd, double visibilita_m, double soglia_lux) {
    return allard(intensita_cd, KOSCHMIEDER / std::max(visibilita_m, 1.0), soglia_lux);
}

double orizzonte_luce_m(double occhio_m, double altezza_m) {
    return ORIZZONTE_VISIVO_M * (std::sqrt(std::max(occhio_m, 0.0)) + std::sqrt(std::max(altezza_m, 0.0)));
}

double portata_fanale_m(const Fanale& f, const Osservatore& o, const CondizioniVista& c, double guadagno_ottico) {
    if (f.portata_m <= 0.0) return 0.0;
    const double e = soglia_lux(c.luce, o.allertato, guadagno_ottico * guadagno_sensore(o.notte));
    const double r = portata_luminosa_m(intensita_cd(f.portata_m), c.visibilita_m, e);
    return std::min(r, orizzonte_luce_m(o.altezza_occhio_m, std::max(f.pos_m[2], ALTEZZA_MIN_M)));
}

VistaLuci vista_luci(const Configurazione& c, Stato s, double relativo, const Osservatore& o, const CondizioniVista& cv,
                     double guadagno_ottico) {
    VistaLuci v;
    for (const auto& f : c.fanali) {
        if (!acceso_nello_stato(f, s) || !nel_settore(f, relativo)) continue;
        if (const double r = portata_fanale_m(f, o, cv, guadagno_ottico); r > v.portata_m) {
            v.portata_m = r;
            v.migliore = &f;
        }
    }
    return v;
}

double probabilita_vista_luci(const Configurazione& c, Stato s, double relativo, const Osservatore& o,
                              const CondizioniVista& cv, double distanza_m, double dt, double guadagno_ottico) {
    if (dt <= 0.0) return 0.0;
    double tasso = 0.0;
    for (const auto& f : c.fanali) {
        if (!acceso_nello_stato(f, s) || !nel_settore(f, relativo)) continue;
        const double r = portata_fanale_m(f, o, cv, guadagno_ottico);
        if (!(r > 0.0) || distanza_m > r) continue;
        const double x = distanza_m / (META_PORTATA * r);
        tasso = std::max(tasso, TASSO_LUCI_S * (f.lampi_7077 ? LAMPI : 1.0) / (1.0 + std::pow(x, 8.0)));
    }
    return 1.0 - std::exp(-tasso * dt);
}

double rapporto_soglia(double portata_m, double distanza_m, double visibilita_m, double luce_ambiente, double guadagno) {
    if (portata_m <= 0.0 || guadagno <= 0.0) return 0.0;
    const double r = std::max(distanza_m, 1.0);
    const double ln = std::log(intensita_cd(portata_m) / (r * r)) - KOSCHMIEDER / std::max(visibilita_m, 1.0) * r -
                      std::log(soglia_lux(luce_ambiente, true, guadagno));
    return std::exp(std::min(ln, 50.0));
}

}  // namespace m688::luci
