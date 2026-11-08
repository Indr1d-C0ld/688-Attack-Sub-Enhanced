#pragma once
// La schiera trainata dei sottomarini (blocco 1 del sonar, D; docs/20_sonar_blocco1.md §7): la geometria del cavo,
// la quota della schiera che l'acustica usa per ascoltare, il ritardo con cui ci arriva, il contatto col fondo.
//
// Fonti: docs/ricerca/04_towed_array.md
// - §1.5: il cavo lungo e sottile sta all'angolo critico, dove la componente normale del peso eguaglia la resistenza
//   normale (legge del seno quadrato di Pode): w cos(phi) = R sin^2(phi), R = 1/2 rho C_n d v^2. La TB-16 (cavo da
//   9,4 mm, 2 N/m in acqua) sta 165 m sotto il battello a 5 kn e 85 m a 10 kn, +-30%;
// - §6.2: la flessione della schiera L min(1, vD / v) e i parametri per sistema, in [schiere.*] di acustica.toml;
// - §2.1 e §6.3: il fondo (contatto quando il punto piu' basso e' entro 5 m dal fondale), i danni (al runtime).
//
// Il modulo e' puro: la missione da' la velocita', la quota del battello, il cavo filato e le profondita' lungo la scia.

#include <utility>
#include <vector>

#include "nucleo/acustica.hpp"

namespace m688 {

inline constexpr int PUNTI_CAVO = 10, PUNTI_SCHIERA = 6;   // il profilo: 10 punti sul cavo e 6 sulla schiera

struct FormaSchiera {
    double angolo_cavo_deg = 90.0;          // dall'orizzontale (90: appeso, da fermo)
    double quota_centro_m = 0.0;            // il centro della schiera: la quota del Ricevitore
    double quota_bassa_m = 0.0;             // il punto piu' basso, la coda
    double dietro_m = 0.0;                  // distanza orizzontale dalla poppa al centro della schiera
    bool oltre_quota_max = false;           // la schiera andrebbe oltre la sua quota massima: e' tenuta li'
    // dalla poppa alla coda: (distanza orizzontale dietro la poppa, quota), PUNTI_CAVO sul cavo e PUNTI_SCHIERA sulla
    // schiera; le profondita' del fondo lungo la scia vanno date a queste distanze
    std::vector<std::pair<double, double>> profilo;
};

// angolo critico del cavo alla velocita' (gradi dall'orizzontale; 90 da fermo)
double angolo_critico_deg(const DatiSchiera& d, double velocita_kn);
// la flessione della schiera (m, fra la testa e la coda)
double flessione_m(const DatiSchiera& d, double velocita_kn);
// La forma d'equilibrio: il cavo filato (fino al massimo del sistema) all'angolo critico, poi la schiera che flette
FormaSchiera forma_schiera(const DatiSchiera& d, double velocita_kn, double quota_battello_m, double cavo_filato_m);

// La quota non cambia di colpo: la discesa del cavo sotto il battello va verso quella d'equilibrio col tempo del cavo,
// tau = (cavo + schiera) / v, da 60 a 600 s (STIMA, dossier 04 §1.5: il tempo di raddrizzamento). Rallentando la
// schiera affonda in qualche minuto; accelerando di colpo risale piano. Il battello che cambia quota se la porta dietro
struct CavoSchiera {
    double discesa_m = -1.0;                // quota del centro meno quota del battello; < 0: da inizializzare
    // un passo: la forma alla discesa raggiunta (con la quota massima)
    FormaSchiera avanza(const DatiSchiera& d, double dt_s, double velocita_kn, double quota_battello_m, double cavo_filato_m);
};
double tempo_schiera_s(const DatiSchiera& d, double velocita_kn, double cavo_filato_m);

// Il cavo piu' lungo che si puo' filare senza che la coda arrivi entro `margine_m` dal fondo (a quella velocita' e
// quota del battello, col fondale dietro la poppa): per il comandante, giocatore o IA, che accorcia il cavo in acqua
// bassa. 0 se nemmeno il cavo corto ci sta; al piu' il cavo del sistema
double cavo_sicuro_m(const DatiSchiera& d, double velocita_kn, double quota_battello_m, double fondale_m, double margine_m = 20.0);

// Il cavo da filare perche' il centro arrivi a `quota_voluta_m` all'equilibrio (il pesce del VDS delle navi, docs/ricerca/33
// §2: una schiera corta al cavo, con la quota ordinata): al piu' il cavo del sistema, cioe' la quota d'equilibrio col cavo
// intero (e mai oltre la quota massima); 0 se la quota voluta e' sopra il punto di traino
double cavo_per_quota_m(const DatiSchiera& d, double velocita_kn, double quota_battello_m, double quota_voluta_m);
// Il contatto col fondo: `profondita` sono i fondali ai punti del profilo (stessa lunghezza; NaN o <= 0 = ignoto).
// Un punto tocca se sta entro 5 m dal fondale (dossier 04 §6.3)
struct ContattoFondo {
    double frazione_schiera = 0.0;          // dei punti della schiera: lo sfregamento del Ricevitore
    double cavo_m = 0.0;                    // il cavo che striscia
    bool tocca = false;
};
ContattoFondo contatto_fondo(const FormaSchiera& f, const std::vector<double>& profondita);

// La vibrazione del cavo (strum): il distacco dei vortici di Karman a f = St v / d, St = 0,2. La fondamentale e la
// seconda armonica, in Hz; vuoto sotto 2 kn o senza diametro
std::vector<double> frequenze_vibrazione(double cavo_diametro_m, double velocita_kn);

}  // namespace m688
