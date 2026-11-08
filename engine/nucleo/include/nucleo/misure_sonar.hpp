#pragma once
// Misure del sonarista sulle righe (S5d; dossier docs/ricerca/09_sonar_sistemi_e_interfacce.md §2.2, §3.6, §3.10):
// - verniero: frequenza fine di una riga dallo spettro di zoom (nucleo/analisi_acustica.hpp, spettro_verniero), con
//   l'incertezza dal tempo d'integrazione e dal S/N;
// - Doppler e CPA: dalla curva frequenza-tempo di una riga (la curva a S) frequenza a riposo, velocita' relativa,
//   istante e distanza del punto di minima distanza, da passare alla TMA;
// - FRAZ: distanza dalla pendenza della frequenza e dal rateo di rilevamento, a moto relativo rettilineo.
//
// Il modulo e' puro e deterministico. Unita': Hz, s, m, m/s, gradi bussola.

#include <optional>
#include <string>
#include <vector>

#include "nucleo/analisi_acustica.hpp"
#include "nucleo/tma.hpp"

namespace m688 {

inline constexpr double C_SUONO_MISURE = 1500.0;   // m/s, come il riferimento del motore acustico

// --- verniero ---

struct MisuraRiga {
    bool valida = false;
    double hz = 0.0;            // picco interpolato fra i bin (parabola sui dB dei tre bin attorno al massimo)
    double snr_db = 0.0;        // (S + N) / N del bin di picco
    double sigma_hz = 0.0;      // incertezza (1 sigma)
};
// Misura il picco piu' alto entro `finestra_hz` attorno a `attorno_hz`. L'incertezza e' il limite di Cramer-Rao per un
// tono (Rife e Boorstyn 1974): sigma = sqrt(3) / (pi T sqrt(SNR)), con T = 1 / bin la durata della FFT e SNR il
// rapporto lineare fra il picco e il fondo locale (la mediana della finestra: rumore piu' continuo della sorgente),
// moltiplicato per integrazione_s / T se si sommano piu' spettri (ALI). Non valida se il picco sta meno di `soglia_db`
// sopra il fondo locale.
MisuraRiga misura_riga(const Spettro& s, double attorno_hz, double finestra_hz = 0.5, double integrazione_s = 0.0,
                       double soglia_db = 3.0);

// --- Doppler e CPA (curva a S) ---

struct PuntoFrequenza {
    double t = 0.0, hz = 0.0;
    double sigma_hz = 0.0;      // 0: tutti i punti pesano uguale
};
// f(t) = f0 (1 - v^2 (t - t_cpa) / (c R(t))), R(t) = sqrt(R_cpa^2 + v^2 (t - t_cpa)^2): moto relativo rettilineo
// uniforme, dossier 09 §3.6. Stima iniziale dalla curva (f0 a meta' fra gli estremi, v dal salto, t_cpa alla pendenza
// massima, R_cpa = f0 v^2 / (c |df/dt|)), poi minimi quadrati non lineari (Levenberg-Marquardt).
struct MisuraCpa {
    bool valida = false;
    std::string motivo;                  // perche' non e' valida (o "")
    double f0_hz = 0.0, velocita_relativa_m_s = 0.0, t_cpa = 0.0, distanza_cpa_m = 0.0;
    double sigma_f0_hz = 0.0, sigma_velocita_m_s = 0.0, sigma_t_cpa = 0.0, sigma_distanza_m = 0.0;
    double residuo_hz = 0.0;             // scarto quadratico medio dalla curva
    bool cpa_osservato = false;          // i punti coprono prima e dopo il CPA
};
MisuraCpa misura_cpa(const std::vector<PuntoFrequenza>& punti);
// la frequenza prevista dal modello all'istante t
double frequenza_cpa(const MisuraCpa& m, double t);

// Consegna alla TMA (dossier 09 §3.10): il CPA che la soluzione proposta prevede (il moto proprio dai rilevamenti
// registrati, il bersaglio in moto rettilineo) contro quello misurato. Scarti di tempo, distanza e velocita' relativa;
// nullopt se la TMA ha meno di due rilevamenti.
struct ScartoCpa {
    double t_cpa_previsto = 0.0, distanza_prevista_m = 0.0, velocita_prevista_m_s = 0.0;
    double scarto_t_s = 0.0, scarto_distanza_m = 0.0, scarto_velocita_m_s = 0.0;
    bool coerente = false;               // tutti e tre entro due sigma della misura
};
std::optional<ScartoCpa> confronta_cpa(const Tma& tma, const SoluzioneTma& soluzione, const MisuraCpa& misura);

// Il passaggio inverso (blocco 3 del sonar, L; docs/23 §5): la soluzione che il CPA misurato propone al MATE. Al CPA il
// rilevamento e' perpendicolare alla velocita' relativa: il bersaglio sta lungo il rilevamento di quell'istante (la retta
// sui rilevamenti attorno) alla distanza del CPA, e la velocita' relativa e' perpendicolare, nel verso del rateo di
// rilevamento. Il moto proprio come in confronta_cpa (dal primo all'ultimo rilevamento). La soluzione e' riferita
// all'ultimo rilevamento, come quelle del MATE. nullopt con la misura non valida, col CPA non osservato o fuori dai
// rilevamenti, con meno di 3 rilevamenti
struct SoluzioneDaCpa {
    SoluzioneTma soluzione;
    double sigma_distanza_m = 0.0, sigma_rotta_deg = 0.0, sigma_velocita_m_s = 0.0;
};
std::optional<SoluzioneDaCpa> soluzione_da_cpa(const Tma& tma, const MisuraCpa& misura);

// --- pendenze e FRAZ ---

struct Pendenza {
    double valore = 0.0, sigma = 0.0;    // unita' per secondo
    int punti = 0;
};
// retta ai minimi quadrati di (t, y), con la sigma della pendenza dai residui; nullopt con meno di 3 punti o tempi uguali
std::optional<Pendenza> pendenza(const std::vector<std::pair<double, double>>& tv);

// Distanza dalla frequenza e dal rilevamento (dossier 09 §2.2, «FRAZ»): con il moto relativo rettilineo uniforme
// d2R/dt2 = v_t^2 / R e v_t = R dB/dt, quindi df/dt = -f0 R (dB/dt)^2 / c e R = -c (df/dt) / (f0 (dB/dt)^2).
// Limite di validita': niente manovra propria ne' del bersaglio nella finestra, df/dt negativa e il rateo misurati
// almeno a 3 sigma, errore relativo entro il 50%. Lontano dal CPA il rateo cala e la stima si perde.
struct StimaFraz {
    bool valida = false;
    std::string motivo;
    double distanza_m = 0.0, sigma_m = 0.0;
};
StimaFraz distanza_fraz(double f0_hz, const Pendenza& df_dt_hz_s, const Pendenza& rateo_gradi_s, bool manovra = false);
// Dalle serie della stessa finestra: frequenze della riga e rilevamenti (gradi, srotolati)
StimaFraz distanza_fraz(double f0_hz, const std::vector<PuntoFrequenza>& frequenze, const std::vector<Rilevamento>& rilevamenti,
                        bool manovra = false);

}  // namespace m688
