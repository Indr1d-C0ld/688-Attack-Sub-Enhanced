#include "nucleo/sondaggio_ia.hpp"

#include <algorithm>
#include <cmath>

namespace m688 {

namespace {
constexpr double TRATTO_MIN_M = 600.0;    // il tratto davanti, anche da fermi
constexpr double PASSO_PROFILO_M = 200.0; // la cella della carta
constexpr double KN_M_S = 1852.0 / 3600.0;
// l'incertezza della posizione (navigazione incerta, nav_ia_): il fondo piu' basso della carta entro una sigma di posizione
// da ogni punto del tratto (8 direzioni a una sigma e a mezza), al piu' 5 km. Al posto della pendenza per la sigma, che
// con chilometri di sigma sui pendii ripidi (le Curili) andava oltre la profondita' stessa e portava l'IA a 20 m in 1750 m
// d'acqua (segnalazione di trailer-1, I16)
constexpr double SIGMA_POSIZIONE_MAX_M = 5000.0;
constexpr int DIREZIONI = 8;
}  // namespace

const char* nome_livello_emissioni(LivelloEmissioni l) {
    switch (l) {
        case LivelloEmissioni::silenzio: return "SILENZIO";
        case LivelloEmissioni::ridotto: return "RIDOTTO";
        case LivelloEmissioni::navigazione: return "NAVIGAZIONE";
        case LivelloEmissioni::sicurezza: return "SICUREZZA";
    }
    return "";
}

DottrinaFondo dottrina_fondo(bool sovietica, double prudenza) {
    const double p = std::clamp(prudenza, 0.0, 1.0);
    DottrinaFondo d;
    if (sovietica) {   // carte buone in casa: piu' vicini al fondo, sondaggi rari (docs/08 §8)
        d.margine_m = 25.0 + 30.0 * p;
        d.periodo_s = 1200.0 - 600.0 * p;
        d.eta_max_s = 1800.0;
        d.velocita_prudente_kn = 15.0 - 5.0 * p;
    } else {           // la NATO nelle carte scarse rallenta e accetta di emettere
        d.margine_m = 40.0 + 40.0 * p;
        d.periodo_s = 420.0 - 240.0 * p;
        d.eta_max_s = 600.0;
        d.velocita_prudente_kn = 12.0 - 4.0 * p;
    }
    d.sigma_margine = 1.5 + 1.0 * p;
    return d;
}

DecisioneFondo decidi_fondo(const QuadroFondale& quadro, const SituazioneFondo& s, const DottrinaFondo& d) {
    DecisioneFondo out;
    // il fondo prudente lungo la rotta, per il tempo di reazione; la sigma della posizione pesa con la pendenza
    const double tratto = std::max(TRATTO_MIN_M, std::max(0.0, s.velocita_kn) * KN_M_S * d.reazione_s);
    const auto profilo = quadro.profilo(s.pos_stimata, s.rotta, tratto, PASSO_PROFILO_M, s.t);
    double sigma_davanti = 0.0;
    const double sp = std::clamp(s.sigma_posizione_m, 0.0, SIGMA_POSIZIONE_MAX_M);
    const double r = radianti(s.rotta);
    for (std::size_t i = 0; i < profilo.size(); ++i) {
        const auto& p = profilo[i];
        double fondo = p.stima_m;   // il piu' basso entro una sigma di posizione
        if (sp > 0.0) {
            const Vec2 c{s.pos_stimata.x + p.distanza_m * std::sin(r), s.pos_stimata.y + p.distanza_m * std::cos(r)};
            for (const double raggio : {0.5 * sp, sp})
                for (int k = 0; k < DIREZIONI; ++k) {
                    const double a = 2.0 * PI * k / DIREZIONI;
                    fondo = std::min(fondo, quadro.leggi({c.x + raggio * std::sin(a), c.y + raggio * std::cos(a)}, s.t).profondita_m);
                }
        }
        const double prudente = fondo - d.sigma_margine * p.sigma_m;
        if (prudente < out.fondo_prudente_m) out.fondo_prudente_m = prudente, sigma_davanti = std::hypot(p.sigma_m, p.stima_m - fondo);
    }
    if (profilo.empty()) return out;
    out.quota_max_m = out.fondo_prudente_m - d.margine_m;
    const double acqua = out.fondo_prudente_m - s.quota_m;   // sotto la chiglia, prudente
    const bool critico = acqua < d.margine_m;
    const bool vicino = acqua < d.vicino * d.margine_m;
    // sotto la chiglia: quanto e' vecchia o incerta l'ultima misura
    const LetturaCella qui = quadro.leggi(s.pos_stimata, s.t);
    const bool misura_buona = !qui.da_carta && qui.eta_s >= 0.0 && qui.eta_s <= d.eta_max_s;
    const bool presto = s.t - s.ultimo_sondaggio_t < d.periodo_s;
    // l'ecoscandaglio secondo il livello d'emissione
    if (critico || (vicino && !misura_buona && !presto)) {
        switch (s.livello) {
            case LivelloEmissioni::silenzio:
                out.sonda = critico && s.t - s.ultimo_sondaggio_t >= 60.0;
                out.potenza = PotenzaEco::bassa;
                break;
            case LivelloEmissioni::ridotto:
                out.sonda = true;
                out.potenza = PotenzaEco::bassa;
                break;
            case LivelloEmissioni::navigazione:
                out.sonda = true;
                out.potenza = s.ostile_vicino_m < d.minaccia_m ? PotenzaEco::bassa : PotenzaEco::piena;
                break;
            case LivelloEmissioni::sicurezza:
                out.sonda = true;
                out.potenza = PotenzaEco::piena;
                break;
        }
        if (out.sonda) out.motivo = critico ? "fondo prudente sotto il margine" : "vicino al fondo, misura vecchia o assente";
        else out.motivo = critico ? "fondo critico, silenzio: si aspetta un minuto fra due ping" : "vicino al fondo, silenzio";
    }
    // il BQS-15: nei campi minati, sotto la banchisa (gli SSN lo tengono acceso per le chiglie e gli ostacoli: e' il sonar
    // per mine e ghiaccio, 06 §1.1; la I17 della libreria, 30/10/2026), e vicino al fondo in moto da NAVIGAZIONE. Mai in
    // silenzio
    const bool emette = s.livello != LivelloEmissioni::silenzio;
    out.bqs15 = emette && (s.campo_minato || s.sotto_ghiaccio ||
                           (s.livello >= LivelloEmissioni::navigazione && vicino && s.velocita_kn > 3.0));
    // la velocita': quella sicura del quadro, e con la carta incerta vicino al fondo al piu' la prudente
    for (const auto& a : quadro.avvisi(s.pos_stimata, s.quota_m, s.rotta, s.velocita_kn, s.t, d.reazione_s))
        if (a.tipo == AvvisoFondo::Tipo::velocita_eccessiva) out.velocita_max_kn = std::min(out.velocita_max_kn, a.valore);
    if (vicino && sigma_davanti > 0.1 * std::max(out.fondo_prudente_m, 1.0))
        out.velocita_max_kn = std::min(out.velocita_max_kn, d.velocita_prudente_kn);
    return out;
}

}  // namespace m688
