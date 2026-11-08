#include "nucleo/modifica_profilo.hpp"

#include <algorithm>
#include <cmath>
#include <functional>
#include <vector>

namespace m688 {

namespace {
constexpr double SLD_MAX_M = 390.0;            // ProfiloSuono::analizza cerca la SLD nei primi 400 m
constexpr double GRADIENTE_PRESSIONE = 0.016;  // dc/dz in acqua isoterma (s^-1): lo strato rimescolato
constexpr double GRADIENTE_TOGLI_MIN = 0.05;   // il gradiente negativo minimo senza strato (s^-1; STIMA)
constexpr double SOPRA_ASSE_MIN_M = 50.0;      // l'asse resta almeno tanto sotto lo strato
}  // namespace

double fase_cambiamento(double t, double t0, double transizione_s, double durata_s) {
    if (t < t0) return 0.0;
    const double tr = std::max(transizione_s, 0.0);
    if (tr > 0.0 && t < t0 + tr) return (t - t0) / tr;
    if (durata_s <= 0.0) return 1.0;
    const double fine = t0 + tr + durata_s;
    if (t < fine) return 1.0;
    return tr > 0.0 && t < fine + tr ? 1.0 - (t - fine) / tr : 0.0;
}

bool modifica_applicabile(const ProfiloSuono& p, ModificaStrato::Tipo t) {
    switch (t) {
        case ModificaStrato::Tipo::togli: return p.sld_m > 0.0;
        case ModificaStrato::Tipo::sposta:
        case ModificaStrato::Tipo::rinforza: return p.gradiente_sotto < 0.0;
        case ModificaStrato::Tipo::canale: return p.asse_m > p.sld_m;
    }
    return false;
}

ProfiloSuono modifica_profilo(const ProfiloSuono& p, const ModificaStrato& m, double frazione) {
    const double f = std::clamp(frazione, 0.0, 1.0);
    if (f <= 0.0 || p.z_m.size() < 2 || !modifica_applicabile(p, m.tipo)) return p;
    // i livelli di partenza, piu' quelli della geometria nuova
    std::vector<double> z = p.z_m;
    std::function<double(double)> nuovo;
    const double sld = p.sld_m, c_sld = p.c(sld);
    switch (m.tipo) {
        case ModificaStrato::Tipo::togli: {
            const double g = std::max(GRADIENTE_TOGLI_MIN, -p.gradiente_sotto);
            nuovo = [&p, sld, c_sld, g](double q) { return q < sld ? c_sld + g * (sld - q) : p.c(q); };
            break;
        }
        case ModificaStrato::Tipo::sposta: {
            const double v = std::clamp(m.valore, 0.0, std::min(SLD_MAX_M, p.z_m.back())), d = v - sld;
            z.push_back(v);
            for (double q : p.z_m) if (q >= sld && q + d >= 0.0) z.push_back(q + d);
            // sopra lo strato nuovo la sola pressione; sotto, il termoclino traslato finche' raggiunge l'acqua di
            // sempre: scendendo e' piu' veloce di quella (il massimo), salendo e' piu' lento (il minimo). Salendo, fino alla
            // vecchia SLD c'era lo strato, che non conta: e' tutto termoclino
            nuovo = [&p, v, d, c_sld, sld](double q) {
                if (q < v) return c_sld + GRADIENTE_PRESSIONE * (q - v);
                if (d >= 0.0) return std::max(p.c(q - d), p.c(q));
                return q < sld ? p.c(q - d) : std::min(p.c(q - d), p.c(q));
            };
            break;
        }
        case ModificaStrato::Tipo::rinforza: {
            const double k = std::max(m.valore, 0.01);
            for (double q : p.z_m) if (q > sld) z.push_back(sld + (q - sld) / k);
            // il termoclino schiacciato (o allungato) verso lo strato, finche' raggiunge l'acqua di sempre
            nuovo = [&p, sld, k](double q) {
                if (q <= sld) return p.c(q);
                const double r = p.c(sld + k * (q - sld));
                return k >= 1.0 ? std::min(r, p.c(q)) : std::max(r, p.c(q));
            };
            break;
        }
        case ModificaStrato::Tipo::canale: {
            const double fondo = p.z_m.back(), asse = p.asse_m;
            const double v = std::clamp(m.valore, sld + SOPRA_ASSE_MIN_M, fondo - SOPRA_ASSE_MIN_M);
            z.push_back(v);
            for (double q : p.z_m) {
                if (q > sld && q < asse) z.push_back(sld + (q - sld) * (v - sld) / (asse - sld));
                if (q > asse) z.push_back(v + (q - asse) * (fondo - v) / std::max(fondo - asse, 1.0));
            }
            // la colonna d'acqua fra lo strato e il fondo dei dati, stirata con l'asse nel punto nuovo
            nuovo = [&p, sld, asse, v, fondo](double q) {
                if (q <= sld) return p.c(q);
                if (q <= v) return p.c(sld + (q - sld) * (asse - sld) / (v - sld));
                return p.c(asse + (q - v) * (fondo - asse) / std::max(fondo - v, 1.0));
            };
            break;
        }
    }
    std::sort(z.begin(), z.end());
    z.erase(std::upper_bound(z.begin(), z.end(), p.z_m.back()), z.end());   // i livelli nuovi restano nei dati
    z.erase(std::unique(z.begin(), z.end(), [](double a, double b) { return std::fabs(a - b) < 0.5; }), z.end());
    ProfiloSuono q;
    q.temperatura_superficie = p.temperatura_superficie;
    q.salinita_superficie = p.salinita_superficie;
    q.z_m = z;
    q.c_m_s.reserve(z.size());
    for (double x : z) q.c_m_s.push_back((1.0 - f) * p.c(x) + f * nuovo(x));
    q.analizza();
    return q;
}

}  // namespace m688
