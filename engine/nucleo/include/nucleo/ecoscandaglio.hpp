#pragma once
// L'ecoscandaglio (quadro del fondale, docs/24 §2.1; docs/08 §3; dossier docs/ricerca/06 §1.2 e §7.1): un ping verso
// il basso che misura la distanza dalla chiglia al fondo, solo sotto il battello e solo quando lo si usa.
// - Un cono di 30 gradi: l'eco arriva dal punto del fondo piu' vicino dentro il cono (la distanza obliqua), e lo
//   strumento la legge come verticale. I fianchi dei rilievi si arrotondano, i picchi stretti si anticipano o si
//   sottostimano (Bowditch §2608).
// - L'errore: la scala della velocita' del suono, fissa per l'apparato e la massa d'acqua (~1%), piu' un rumore.
// - I falsi fondi (~2%): lo strato diffondente profondo di giorno (300-500 m sotto la superficie), le bolle e la scia
//   sopra i 20 nodi (eco a pochi metri).
// - Due potenze: piena (fino a 6000 m sotto la chiglia, si sente a 15-20 km) e bassa (-20 dB, fino a 2000 m, 5-8 km).
// - Ogni ping e' un impulso che gli altri sentono: emissione() lo da' ai ricevitori d'intercettazione (blocco 2).
// Quasi tutti i valori sono STIMA DI GIOCO (le caratteristiche del fathometer dei 688 non sono pubbliche). Il modulo e'
// puro e deterministico dal generatore; la simulazione passa la posizione vera (per il fondo) e quella stimata (per
// dove il battello crede di aver misurato).

#include <optional>
#include <string>

#include "nucleo/casuale.hpp"
#include "nucleo/fondale.hpp"
#include "nucleo/geometria.hpp"
#include "nucleo/intercettazione.hpp"

namespace m688 {

enum class PotenzaEco { piena, bassa };

struct ParametriEcoscandaglio {
    std::string id = "fathometer";            // la chiave dell'emissione nel catalogo dell'intercettazione
    double cono_deg = 30.0;                   // apertura totale
    double portata_piena_m = 6000.0, portata_bassa_m = 2000.0;   // sotto la chiglia
    double sl_piena_db = 210.0, sl_bassa_db = 190.0;
    double f_hz = 12000.0;
    double errore_scala = 0.01;               // sigma della scala della velocita' del suono (per apparato)
    double rumore_rel = 0.002;                // rumore per ping, relativo alla distanza
    double falso_fondo = 0.02;                // probabilita' di un falso fondo per ping
    double dsl_min_m = 300.0, dsl_max_m = 500.0;   // lo strato diffondente di giorno (profondita' dalla superficie)
    double scia_kn = 20.0;                    // sopra, i falsi fondi diventano bolle della scia
    double falso_scia = 0.10;                 // probabilita' sopra scia_kn
};

struct Sondaggio {
    double t = 0.0;
    Vec2 pos;                                 // dove il battello crede di essere (stimata)
    double quota_m = 0.0;                     // della chiglia
    bool eco = false;                         // falso: niente entro la portata
    double sotto_chiglia_m = 0.0;             // misurata (letta come verticale)
    double fondo_m = 0.0;                     // profondita' del fondo misurata: quota + sotto la chiglia
    double sigma_m = 0.0;                     // quanto il battello si fida della misura (la scala e il rumore)
    double impronta_m = 0.0;                  // raggio dell'impronta sul fondo
    PotenzaEco potenza = PotenzaEco::piena;
    // verita', per le prove e il debriefing (chi naviga non la vede)
    bool falso = false;
    Vec2 pos_vera;
    double fondo_vero_m = 0.0;                // sotto la posizione vera, al centro
};

// [ecoscandagli.<id>] di data/motore/acustica.toml (tools/pipeline/acustica.py); lancia se il file o la voce mancano
ParametriEcoscandaglio carica_ecoscandaglio(const std::string& percorso, const std::string& id = "fathometer");

// Lo scarto di scala dell'apparato (velocita' del suono non corretta), una volta per battello: N(0, errore_scala)
double scala_ecoscandaglio(const ParametriEcoscandaglio& p, Casuale& caso);

// Un ping all'istante t. `scala`: da scala_ecoscandaglio. `giorno`: lo strato diffondente c'e' solo di giorno
Sondaggio sonda(const Fondale& vero, Vec2 pos_vera, Vec2 pos_stimata, double quota_m, double velocita_kn, double t,
                PotenzaEco potenza, bool giorno, double scala, const ParametriEcoscandaglio& p, Casuale& caso);

// L'impulso per i ricevitori d'intercettazione (verso il basso: chi e' di lato sente il lobo laterale, -20 dB, STIMA)
EmissioneAttiva emissione(const Sondaggio& s, int emettitore, double rotta, double velocita_kn, const ParametriEcoscandaglio& p);

// Il modo d'uso: a comando o periodico ogni N minuti (1-30). passo() dice se all'istante t va fatto un ping
class ProgrammaEcoscandaglio {
public:
    void singolo() { singolo_ = true; }
    void periodico(double minuti);            // 0: spento
    double periodo_min() const { return periodo_s_ / 60.0; }
    bool passo(double t);
private:
    bool singolo_ = false;
    double periodo_s_ = 0.0, prossimo_ = -1.0;
};

}  // namespace m688
