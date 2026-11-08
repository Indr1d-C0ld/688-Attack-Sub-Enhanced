#pragma once
// L'IA che sonda il fondo (docs/24 §3; docs/08 §3 e §8; dossier docs/ricerca/06 §103): per simmetria col giocatore, un
// battello dell'IA sa del fondo solo la sua carta e le sue misure (il suo QuadroFondale), e per misurare deve emettere.
// Il modulo decide, a ogni passo, se sondare con l'ecoscandaglio e a che potenza, se tenere acceso il BQS-15, e la quota
// e la velocita' prudenti; il runtime esegue (gli stessi ordini del giocatore) e mette i sondaggi nel quadro.
// - Davanti alla prua, per il tempo di reazione alla velocita', il fondo «prudente» e' la stima del quadro meno
//   `sigma_margine` volte la sua sigma (con l'incertezza della posizione per la pendenza); la quota massima e' quel
//   fondo meno `margine_m`.
// - Si sonda quando il fondo prudente e' vicino (entro `vicino` volte il margine) e il quadro sotto la chiglia e'
//   vecchio o incerto, al piu' una volta ogni `periodo_s`; con il fondo prudente sotto il margine si sonda subito.
// - I livelli d'emissione (docs/08 §3, dossier 06 §103): SILENZIO solo per un margine critico, a bassa potenza;
//   RIDOTTO a bassa potenza; NAVIGAZIONE a piena, bassa con un ostile vicino; SICUREZZA sempre a piena.
// - Il BQS-15 nei campi minati (da RIDOTTO in su) e, da NAVIGAZIONE, vicino al fondo e in moto.
// - La dottrina (docs/08 §8): la NATO nelle carte scarse rallenta e accetta di emettere; i sovietici con le carte buone
//   in casa sondano di rado, a bassa potenza, e stanno piu' vicini al fondo. `prudenza` (0..1) e' la difficolta'.
// Il modulo e' puro e deterministico.

#include <string>

#include "nucleo/ecoscandaglio.hpp"
#include "nucleo/quadro_fondale.hpp"

namespace m688 {

enum class LivelloEmissioni : std::uint8_t { silenzio, ridotto, navigazione, sicurezza };
const char* nome_livello_emissioni(LivelloEmissioni l);

struct DottrinaFondo {
    double margine_m = 50.0;          // acqua sotto la chiglia voluta oltre l'incertezza
    double sigma_margine = 2.0;       // quante sigma del quadro tolte alla stima
    double vicino = 3.0;              // si sonda col fondo prudente entro vicino x margine
    double periodo_s = 300.0;         // al piu' un sondaggio ogni tanto, fuori dai casi critici
    double eta_max_s = 600.0;         // una misura sotto la chiglia piu' vecchia di cosi' non basta
    double reazione_s = 180.0;        // il tratto davanti: la velocita' per questo tempo
    double minaccia_m = 20000.0;      // un ostile entro: in NAVIGAZIONE la potenza bassa
    double velocita_prudente_kn = 10.0;   // con la carta incerta vicino al fondo, al piu'
};
// la dottrina della marina (sovietica o NATO) per la prudenza 0..1 (STIMA di gioco)
DottrinaFondo dottrina_fondo(bool sovietica, double prudenza = 0.5);

struct SituazioneFondo {
    Vec2 pos_stimata;
    double sigma_posizione_m = 0.0;
    double quota_m = 0.0, rotta = 0.0, velocita_kn = 0.0;
    double t = 0.0;
    LivelloEmissioni livello = LivelloEmissioni::navigazione;
    double ostile_vicino_m = 1e9;     // la distanza dall'ostile piu' vicino noto
    bool campo_minato = false;        // dentro o vicino a un campo minato noto
    bool sotto_ghiaccio = false;      // sotto la banchisa: il sonar da mine e ghiaccio resta acceso (chiglie, mine)
    double ultimo_sondaggio_t = -1e9;
};

struct DecisioneFondo {
    bool sonda = false;
    PotenzaEco potenza = PotenzaEco::piena;
    bool bqs15 = false;
    double velocita_max_kn = 1e9;     // nessun limite
    double quota_max_m = 1e9;         // la quota piu' profonda prudente (nessun limite)
    double fondo_prudente_m = 1e9;    // il fondo prudente davanti
    std::string motivo;               // per il registro e le prove
};

DecisioneFondo decidi_fondo(const QuadroFondale& quadro, const SituazioneFondo& s, const DottrinaFondo& d);

}  // namespace m688
