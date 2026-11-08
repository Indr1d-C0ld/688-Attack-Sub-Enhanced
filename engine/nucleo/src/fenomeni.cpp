#include "nucleo/fenomeni.hpp"

#include <algorithm>
#include <cmath>
#include <iterator>

#include "nucleo/geometria.hpp"

namespace m688 {

namespace {

constexpr double G = 9.81;

// interpolazione lineare in una tabella a passo qualunque (estremi tenuti fermi)
template <std::size_t N>
double interpola(const double (&x)[N], const double (&y)[N], double v) {
    if (v <= x[0]) return y[0];
    for (std::size_t i = 1; i < N; ++i)
        if (v <= x[i]) return y[i - 1] + (y[i] - y[i - 1]) * (v - x[i - 1]) / (x[i] - x[i - 1]);
    return y[N - 1];
}

// oscillazioni della bolla prima di arrivare in superficie (dossier 07 §1.4, Tab. 12 di [Sw78]): x = d/W^(1/3)
constexpr double OSC_X[] = {0.0, 0.5, 1.0, 1.5, 2.0, 2.5, 3.0, 3.5, 4.0, 4.5, 5.0, 5.5, 6.0};
constexpr double OSC_N[] = {0.0, 0.37, 0.72, 1.08, 1.45, 1.83, 2.22, 2.65, 3.07, 3.55, 4.07, 4.68, 5.37};

// pennacchi di [Sw78] cap. 14 (dossier 07 §2.4 a): y = d/W^(1/4), altezza e raggio massimi / W^(1/3). La cifra
// illeggibile a y = 0,5 ("34-39") e' presa a meta'.
constexpr double PEN_Y[] = {0.0, 0.5, 1.0, 1.5, 2.0, 2.5, 3.0, 3.5, 4.0, 4.5, 5.0, 5.5, 6.0, 6.5, 7.0, 8.0, 9.0};
constexpr double PEN_H[] = {29.7, 36.5, 38.6, 29.0, 20.6, 19.7, 18.9, 18.0, 17.1, 16.2, 15.4, 13.6, 9.5, 5.9, 5.9, 5.9, 5.9};
constexpr double PEN_R[] = {8.3, 8.3, 8.3, 10.0, 12.8, 12.8, 12.8, 12.8, 12.8, 11.6, 10.6, 9.5, 8.5, 7.4, 6.4, 4.3, 2.3};

}  // namespace

const char* nome_regime(RegimeSubacqueo r) {
    switch (r) {
        case RegimeSubacqueo::sfiato: return "sfiato";
        case RegimeSubacqueo::getto: return "getto";
        case RegimeSubacqueo::pennacchi: return "pennacchi";
        case RegimeSubacqueo::cupola_bassa: return "cupola bassa";
        case RegimeSubacqueo::profonda: return "profonda";
    }
    return "?";
}

FenomenoSubacqueo fenomeno_subacqueo(double W, double d, double fondale) {
    FenomenoSubacqueo f;
    W = std::max(W, 0.01);
    d = std::max(d, 0.1);
    f.W = W;
    f.d = d;
    const double W3 = std::cbrt(W), W4 = std::pow(W, 0.25), Z = d + 10.0;
    // bolla (Willis; costanti del TNT), periodi successivi di Swift e Decius
    f.A_max = 3.50 * W3 / std::cbrt(Z);
    f.T1 = 2.11 * W3 / std::pow(Z, 5.0 / 6.0);
    f.T2 = 0.74 * f.T1;
    f.T3 = 0.63 * f.T1;
    f.d_su_A = d / f.A_max;
    if (f.d_su_A < 1.0) f.regime = RegimeSubacqueo::sfiato;
    else if (f.d_su_A < 1.5) f.regime = RegimeSubacqueo::getto;
    else if (f.d_su_A < 3.0) f.regime = RegimeSubacqueo::pennacchi;
    else if (f.d_su_A < 5.0) f.regime = RegimeSubacqueo::cupola_bassa;
    else f.regime = RegimeSubacqueo::profonda;

    // anello scuro: finche' l'urto in superficie supera ~2 MPa
    f.R_slick = 18.0 * W3;
    f.t_slick = std::hypot(f.R_slick, d) / 1500.0;

    // cupola di Kolsky (valida per 0,23 < W^(1/3)/d < 3,1; sopra e' un'esplosione di superficie)
    const double k = W3 / d;
    if (k > 0.23) {
        f.v0_cupola = 86.0 * std::min(k, 3.1) - 6.6;
        f.H_cupola = 0.45 * f.v0_cupola * f.v0_cupola / (2 * G);
        f.t_apice_cupola = 2.0 * f.H_cupola / f.v0_cupola;
        f.R_cupola = std::max(1.25 * d, f.A_max);   // diametro visibile 2-3 volte la profondita'
    }

    // pennacchi: tabella di Swisdak fino a y = 9; oltre resta una gobba (2-5 m per 135-270 kg a 50-100 m)
    const double y = d / W4;
    const double n_osc = interpola(OSC_X, OSC_N, d / W3);
    if (y <= 9.0 && W >= 5.0) {   // le cariche sotto ~5 kg non fanno pennacchi vistosi
        f.H_pennacchio = interpola(PEN_Y, PEN_H, y) * W3;
        f.R_pennacchio = interpola(PEN_Y, PEN_R, y) * W3;
        f.t_pennacchio = f.T1;   // all'impulso della bolla
        f.t_salita = 0.8 * std::sqrt(2.0 * f.H_pennacchio / G);
        f.durata_nebbia = 4.0 + 0.15 * f.H_pennacchio;   // 20-40 s per 150-200 m, 5-15 s per 30-60 m
        if (f.regime == RegimeSubacqueo::sfiato || f.regime == RegimeSubacqueo::getto || f.regime == RegimeSubacqueo::pennacchi)
            f.R_base = 1.5 * f.R_pennacchio;
    } else {
        f.H_gobba = 0.8 * W3 * std::sqrt(9.0 / y);
        if (f.H_gobba < 0.5) f.H_gobba = 0.0;
    }
    // la cupola resta sotto i pennacchi che la bucano (per le cariche molto basse Kolsky darebbe centinaia di metri:
    // li' domina la colonna, dossier 07 §2.1)
    if (f.H_pennacchio > 0 && f.H_cupola > 0.5 * f.H_pennacchio) {
        f.H_cupola = 0.5 * f.H_pennacchio;
        f.t_apice_cupola = 2.0 * f.H_cupola / f.v0_cupola;
    }
    // le cariche minuscole senza pennacchio (sotto ~5 kg) non hanno la cupola di Kolsky delle grandi: un getto di
    // qualche metro (STIMA: 6 W^(1/3) m, 6 m per 1 kg a pelo d'acqua)
    if (f.H_pennacchio <= 0 && W < 5.0 && f.H_cupola > 6.0 * W3) {
        f.H_cupola = 6.0 * W3;
        f.t_apice_cupola = 2.0 * f.H_cupola / f.v0_cupola;
    }
    f.corona_scura = f.regime == RegimeSubacqueo::sfiato;
    if (f.regime == RegimeSubacqueo::pennacchi || f.regime == RegimeSubacqueo::cupola_bassa)
        f.pulsazioni = std::clamp(static_cast<int>(std::floor(n_osc)) - 1, 0, 2);

    // chiazza di schiuma (o di fango se la bolla tocca il fondo)
    f.fango = fondale - d < 2.0 * f.A_max;
    f.R_chiazza = std::max(4.0 * W3, 1.2 * f.R_pennacchio);
    f.durata_chiazza = (f.fango ? 300.0 : 60.0) + 10.0 * W3;
    return f;
}

FenomenoAereo fenomeno_aereo(double W, bool deposito) {
    FenomenoAereo f;
    W = std::max(W, 0.1);
    f.W = W;
    const double W3 = std::cbrt(W);
    // palla di fuoco degli alti esplosivi: ~3,5 W^(1/3) m (10-20 m per i 165 kg di un Exocet, dossier 07 §11.2)
    f.D_palla = 3.5 * W3;
    f.durata_palla = 0.2 + 0.08 * W3;
    f.v_schegge = (deposito ? 32.0 : 25.0) * std::pow(W, 1.0 / 6.0);
    // nube: sbuffo del missile; fungo dei depositi alto centinaia di metri (Hood, Yamato)
    f.H_nube = (deposito ? 45.0 : 10.0) * W3;
    f.lampo = std::clamp(W / 2000.0, 0.25, 1.0);
    return f;
}

double flusso_galleggiamento(double Q) {
    // F = g Qc / (pi rho cp T), con il 70% del calore portato dalla convezione; aria a 15 gradi
    return G * 0.7 * std::max(Q, 0.0) / (PI * 1.2 * 1005.0 * 288.0);
}

double risalita_finale(double F, double u) {
    if (F <= 0) return 0.0;
    u = std::max(u, 1.5);
    const double xf = F < 55.0 ? 49.0 * std::pow(F, 5.0 / 8.0) : 119.0 * std::pow(F, 0.4);
    return 1.6 * std::cbrt(F) * std::pow(xf, 2.0 / 3.0) / u;
}

double risalita_fumo(double F, double u, double x) {
    if (F <= 0 || x <= 0) return 0.0;
    u = std::max(u, 1.5);
    return std::min(1.6 * std::cbrt(F) * std::pow(x, 2.0 / 3.0) / u, risalita_finale(F, u));
}

double altezza_fiamma(double Q, double D) {
    return std::max(0.0, 0.235 * std::pow(std::max(Q, 0.0) / 1000.0, 0.4) - 1.02 * D);
}

}  // namespace m688
