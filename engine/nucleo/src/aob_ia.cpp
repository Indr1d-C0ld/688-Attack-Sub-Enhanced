#include "nucleo/aob_ia.hpp"

#include <algorithm>
#include <cmath>

namespace m688 {

namespace {

constexpr double SIGMA_ALTO_DEG = 10.0, SIGMA_BASSO_DEG = 15.0;   // come il giocatore (docs/25 §3.4)
constexpr double DISTANZA = 1.0;                // alla portata la sigma raddoppia
constexpr double NOTTE_LUCE = 0.2;              // sotto e' notte (come l'artiglieria e il libretto)
constexpr double NOTTE_LUCI = 1.5, NOTTE_SAGOMA = 2.0, VELA = 1.5;
constexpr double SIGMA_MAX_DEG = 60.0;
constexpr double ASSE_DEG = 20.0;               // entro questo dalla prua o dalla poppa il lato e' ambiguo
constexpr double SPECCHIO_ASSE = 0.3;           // la probabilita' sull'asse, da vicino; raddoppia alla portata
constexpr double PASSO_DEG = 10.0;

double gauss(Casuale& c) {
    const double u1 = std::max(c.uniforme(), 1e-12), u2 = c.uniforme();
    return std::sqrt(-2.0 * std::log(u1)) * std::cos(2.0 * PI * u2);
}

double distanza_relativa(const CondizioniAob& c) {
    return c.portata_m > 0.0 ? std::clamp(c.distanza_m / c.portata_m, 0.0, 1.0) : 1.0;
}

}  // namespace

double aob_vero(double rilevamento, double rotta_bersaglio) { return normalizza_180(rilevamento + 180.0 - rotta_bersaglio); }

double sigma_aob(const CondizioniAob& c) {
    double s = (c.alto_ingrandimento ? SIGMA_ALTO_DEG : SIGMA_BASSO_DEG) * (1.0 + DISTANZA * distanza_relativa(c));
    if (c.luce < NOTTE_LUCE) s *= c.luci ? NOTTE_LUCI : NOTTE_SAGOMA;
    if (c.solo_vela) s *= VELA;
    return std::min(s, SIGMA_MAX_DEG);
}

double probabilita_specchio(double aob_gradi, const CondizioniAob& c) {
    const double a = std::fabs(normalizza_180(aob_gradi));
    const double dall_asse = std::min(a, 180.0 - a);
    if (dall_asse >= ASSE_DEG) return 0.0;
    if (c.luci && c.luce < NOTTE_LUCE) return 0.0;   // il rosso e il verde dicono il lato
    return std::min(0.5, SPECCHIO_ASSE * (1.0 - dall_asse / ASSE_DEG) * (1.0 + distanza_relativa(c)));
}

StimaAob stima_aob(double aob_vero_gradi, double rilevamento, const CondizioniAob& c, Casuale& caso) {
    StimaAob s;
    s.sigma_gradi = sigma_aob(c);
    double a = normalizza_180(aob_vero_gradi);
    if (caso.uniforme() < probabilita_specchio(a, c)) {
        a = normalizza_180(-a);   // dritta per sinistra: 15 diventa -15, 170 diventa -170
        s.specchiato = true;
    }
    a += s.sigma_gradi * gauss(caso);
    s.aob_gradi = normalizza_180(PASSO_DEG * std::round(normalizza_180(a) / PASSO_DEG));
    s.rotta = normalizza_360(rilevamento + 180.0 - s.aob_gradi);
    return s;
}

}  // namespace m688
