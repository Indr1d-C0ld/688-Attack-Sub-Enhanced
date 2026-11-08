#include "nucleo/evasione.hpp"

#include <algorithm>
#include <cmath>

namespace m688 {

namespace {

// tempi delle manovre (STIMA): la corsa di un siluro aviolanciato e' di 6-8 min (§6.7); dopo un tonfo o un ping
// vicino il comandante resta in silenzio un quarto d'ora; un sorvolo dura secondi, la prudenza dieci minuti
double durata_s(StimoloEvasione s) {
    switch (s) {
        case StimoloEvasione::ping_lontano: return 600.0;
        case StimoloEvasione::tonfo: return 900.0;
        case StimoloEvasione::sorvolo: return 600.0;
        case StimoloEvasione::ping_vicino: return 900.0;
        case StimoloEvasione::siluro: return 480.0;
        case StimoloEvasione::siluro_vicino: return 480.0;
    }
    return 600.0;
}

// sul fondo si resta piu' a lungo: il San Luis resse 20 h di caccia fermo a ~70 m (§9)
constexpr double DURATA_FONDO_S = 1800.0;
// acque basse per posarsi: entro la quota massima, fra 50 e 150 m (STIMA dal San Luis a ~70 m; piu' in alto lo scafo e'
// troppo vicino alla superficie per nascondersi)
constexpr double FONDO_MAX_M = 150.0, FONDO_MIN_M = 50.0;
// velocita' silenziosa (STIMA): un SSN a 5 kn come nel riferimento del §7.2, un SSK a 3 kn sulle batterie; dosata,
// per allontanarsi senza farsi sentire sotto lo strato: 12 kn e 6 kn
constexpr double SILENZIO_SSN_KN = 5.0, SILENZIO_SSK_KN = 3.0, DOSATA_SSN_KN = 12.0, DOSATA_SSK_KN = 6.0;
// sotto lo strato: 50 m oltre la SLD; senza strato 150 m, oltre la portata del MAD dalla superficie (§6.3)
constexpr double SOTTO_STRATO_M = 50.0, SENZA_STRATO_M = 150.0;
// franco dal fondo e dalla quota massima
constexpr double FRANCO_FONDO_M = 15.0, FRANCO_MAX_M = 20.0;
// sotto il pavimento del siluro: 50 m oltre la sua quota massima (STIMA)
constexpr double OLTRE_PAVIMENTO_M = 50.0;

double sotto_lo_strato(const BattelloEvasione& b) {
    const double voluta = b.strato_m > 0.0 ? b.strato_m + SOTTO_STRATO_M : SENZA_STRATO_M;
    return std::max(b.quota_m, voluta);
}

}  // namespace

const char* nome_stimolo(StimoloEvasione s) {
    switch (s) {
        case StimoloEvasione::ping_lontano: return "ping lontano";
        case StimoloEvasione::tonfo: return "tonfo";
        case StimoloEvasione::sorvolo: return "sorvolo";
        case StimoloEvasione::ping_vicino: return "ping vicino";
        case StimoloEvasione::siluro: return "siluro";
        case StimoloEvasione::siluro_vicino: return "siluro vicino";
    }
    return "?";
}

bool battello_diesel(const std::string& classe) {
    return classe == "kilo" || classe == "foxtrot";
}

void Evasione::stimolo(StimoloEvasione s, double rilevamento_deg, double t, const BattelloEvasione& b, Casuale& caso) {
    const bool in_corso = attiva(t);
    if (in_corso && s < stimolo_) return;
    if (in_corso && s == stimolo_) {   // lo stesso stimolo: la manovra continua
        fine_ = std::max(fine_, t + (fondo_ ? DURATA_FONDO_S : durata_s(s)));
        return;
    }
    const double u = caso.uniforme(), lato = caso.uniforme() < 0.5 ? -1.0 : 1.0;
    const double silenzio = std::min(b.velocita_max_kn, b.diesel ? SILENZIO_SSK_KN : SILENZIO_SSN_KN);
    const double dosata = std::min(b.velocita_max_kn, b.diesel ? DOSATA_SSK_KN : DOSATA_SSN_KN);
    const double via = normalizza_360(rilevamento_deg + 180.0);   // dalla parte opposta alla sorgente
    stimolo_ = s;
    fondo_ = false;
    rotta_libera_ = false;
    ++manovre_;
    switch (s) {
        case StimoloEvasione::ping_lontano:   // via dal sonar a velocita' dosata, sotto lo strato
            rotta_ = normalizza_360(via + lato * 30.0 * u);
            velocita_ = dosata;
            quota_ = sotto_lo_strato(b);
            break;
        case StimoloEvasione::tonfo:   // qualcuno posa boe: silenzio, sotto lo strato, via di 60-90° dal tonfo
            rotta_ = normalizza_360(b.rotta_deg + lato * (60.0 + 30.0 * u));
            if (std::abs(normalizza_180(rotta_ - rilevamento_deg)) < 60.0) rotta_ = normalizza_360(b.rotta_deg - lato * (60.0 + 30.0 * u));
            velocita_ = std::min(b.velocita_kn, silenzio);
            quota_ = sotto_lo_strato(b);
            break;
        case StimoloEvasione::sorvolo:   // l'aereo e' sopra: giu' (oltre il MAD) e di traverso alla sua rotta
            rotta_ = normalizza_360(b.rotta_deg + lato * (70.0 + 40.0 * u));
            velocita_ = std::min(b.velocita_kn, silenzio);
            quota_ = std::max(sotto_lo_strato(b), SENZA_STRATO_M);
            break;
        case StimoloEvasione::ping_vicino:   // l'eco e' vicina: via dal datum su una rotta sola, dosata, sotto lo strato
            rotta_ = normalizza_360(via + lato * 20.0 * u);
            velocita_ = dosata;
            quota_ = sotto_lo_strato(b);
            break;
        case StimoloEvasione::siluro:
        case StimoloEvasione::siluro_vicino:   // poppa al siluro, tutta forza, giu' lontano da lui
            rotta_ = normalizza_360(via + lato * 10.0 * u);
            velocita_ = b.velocita_max_kn;
            // il siluro aviolanciato scende dalla superficie: giu', sotto il suo pavimento se si arriva, se no al massimo
            quota_ = b.pavimento_siluro_m > 0.0 && b.quota_max_m - FRANCO_MAX_M >= b.pavimento_siluro_m + OLTRE_PAVIMENTO_M
                         ? b.pavimento_siluro_m + OLTRE_PAVIMENTO_M : b.quota_max_m;
            break;
    }
    // in acque basse, contro il sonar vicino e i siluri, ci si posa sul fondo: un siluro contro un battello fermo sul
    // fondo ha Pk <= 0,1 (§6.7); STIMA: due SSK su tre, un SSN su quattro
    const bool basso = b.fondale_m >= FONDO_MIN_M && b.fondale_m <= std::min(FONDO_MAX_M, b.quota_max_m);
    if (basso && (s == StimoloEvasione::ping_vicino || s == StimoloEvasione::siluro || s == StimoloEvasione::siluro_vicino ||
                  s == StimoloEvasione::sorvolo) &&
        caso.uniforme() < (b.diesel ? 0.67 : 0.25)) {
        fondo_ = true;
        velocita_ = 0.0;
    }
    fine_ = t + (fondo_ ? DURATA_FONDO_S : durata_s(s));
}

std::optional<ComandoEvasione> Evasione::comando(double t, const BattelloEvasione& b) const {
    if (!attiva(t)) return std::nullopt;
    ComandoEvasione c;
    c.stimolo = stimolo_;
    c.rotta_deg = rotta_libera_ ? b.rotta_deg : rotta_;
    c.velocita_kn = velocita_;
    c.sul_fondo = fondo_;
    const double massima = std::min(b.quota_max_m - FRANCO_MAX_M, b.fondale_m - FRANCO_FONDO_M);
    c.quota_m = fondo_ ? std::max(0.0, b.fondale_m - 0.5) : std::clamp(quota_, 30.0, std::max(30.0, massima));
    return c;
}

}  // namespace m688
