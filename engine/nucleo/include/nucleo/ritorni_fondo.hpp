#pragma once
// Il BQS-15 sul fondo e sulle pareti (quadro del fondale, docs/24 §2.2; docs/08 §3; dossier docs/ricerca/06 §1.1 e
// §7.1). Le mine restano a Missione::spazzata_bqs15; questa e' la stessa spazzata vista dal lato del terreno.
// - Il canale di prua: +-60 gradi in azimut, da -30 a +45 in elevazione, un raggio ogni `passo_az_deg`. Su ogni raggio
//   l'eco e' il punto del terreno piu' vicino dentro il ventaglio d'elevazione.
// - La portata dipende da cosa si illumina (calcolo del dossier 06 §1.1: f = 25 kHz, SL 205 dB): una parete o un
//   rilievo che sale verso il battello si vede a ~3 km, il fondo radente molto meno (~1,5 km, STIMA). Il rumore proprio
//   la degrada: -30% a 15 nodi, -60% oltre i 20 (STIMA).
// - Un raggio senza eco dice «niente fino a X»: lungo il bordo basso del ventaglio il fondo sta piu' giu'.
// - L'errore del sonar: ~1 grado e ~5 m (dossier 06 §7.1).
// Il risultato sono RitornoFondo gia' in posizione stimata, per QuadroFondale::aggiungi. I sovietici (MG-70, MT-70,
// MG-24) usano lo stesso modello con le portate ridotte (`fattore_portata`, STIMA). Puro e deterministico.

#include <optional>
#include <string>
#include <vector>

#include "nucleo/casuale.hpp"
#include "nucleo/fondale.hpp"
#include "nucleo/quadro_fondale.hpp"

namespace m688 {

struct ParametriBqs15Fondo {
    std::string id = "bqs15";
    double settore_az_deg = 60.0;            // +-
    double elevazione_min_deg = -30.0, elevazione_max_deg = 45.0;
    double passo_az_deg = 7.5;               // due raggi per settore di 15 gradi
    double portata_parete_m = 3200.0, portata_fondo_m = 1500.0;
    double pendenza_parete = 0.3;            // il terreno che sale verso il battello piu' di cosi' e' una parete
    double velocita_media_kn = 15.0, fattore_media = 0.7;   // -30% a 15 nodi
    double velocita_alta_kn = 20.0, fattore_alta = 0.4;     // -60% oltre i 20
    double errore_angolo_deg = 1.0, errore_distanza_m = 5.0;
    double passo_m = 25.0;                   // la marcia lungo il raggio
    double margine_ostacolo_m = 30.0;        // un eco piu' alto della chiglia meno questo e' un ostacolo
    double fattore_portata = 1.0;            // i sonar sovietici: meno (STIMA)
    // vede anche le mine (BQS-15, MG-70, MG-24) o solo ghiaccio e ostacoli (il BQS-8 degli Sturgeon, docs/ricerca/34); e
    // l'impulso che emette, per l'intercettazione (emissioni.<emissione> di acustica.toml)
    bool mine = true;
    std::string emissione = "bqs15";
};

// [bqs15_fondo.<id>] di data/motore/acustica.toml; lancia se il file o la voce mancano
ParametriBqs15Fondo carica_bqs15_fondo(const std::string& percorso, const std::string& id = "bqs15");
// il sonar del fondo della classe (la voce che la elenca in `classi`, anche coi suffissi: los_angeles_flight3), o
// nessuno
std::optional<ParametriBqs15Fondo> bqs15_fondo_di_classe(const std::string& percorso, const std::string& classe);

// la portata dopo il rumore proprio (interpolata fra 0 e la velocita' media, poi fino all'alta)
double portata_effettiva(double portata_m, double velocita_kn, const ParametriBqs15Fondo& p);

// Una spazzata all'istante t: i raggi del settore di prua, sul fondale vero dalla posizione vera, messi nella
// posizione stimata
std::vector<RitornoFondo> spazza_fondo(const Fondale& vero, Vec2 pos_vera, Vec2 pos_stimata, double quota_m, double rotta,
                                       double velocita_kn, double t, const ParametriBqs15Fondo& p, Casuale& caso);

}  // namespace m688
