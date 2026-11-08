#pragma once
// L'angolo sulla prora (AOB) stimato a vista dall'IA, per la simmetria col giocatore (docs/25 §3.4-3.5; docs/27 per le
// luci). Il giocatore confronta la sagoma con le viste del libretto e imposta l'AOB a passi di 10 gradi, con ±10 gradi ad
// alto ingrandimento e ±15 a basso, e lo manda alla TMA come rotta = rilevamento + 180 - AOB. L'IA fa lo stesso:
// - l'AOB vero piu' un errore gaussiano, arrotondato ai 10 gradi;
// - la sigma parte da quella del giocatore (10 o 15 gradi) e cresce con la distanza sulla portata della vista (che ha
//   gia' la foschia dentro: fino al doppio alla portata), di notte (x1,5 coi fanali, che danno l'aspetto dai colori e
//   dall'allineamento; x2 sulla sola sagoma) e sulla sola vela d'un sottomarino in superficie (x1,5); al piu' 60 gradi;
// - vicino alla prua e alla poppa (entro 20 gradi) la sagoma non dice da che lato si guarda: con una probabilita' che
//   cresce verso l'asse e con la distanza l'AOB si specchia (dritta per sinistra), e la sigma non lo sa;
// - la rotta per la TMA, con la sigma, come osservazione di rotta (Tma::aggiungi_rotta; il peso e' quello di osserva_aob).
// Tutte le costanti sono STIME di gioco. Modulo puro: l'unica estrazione e' dal Casuale passato.

#include "nucleo/casuale.hpp"
#include "nucleo/geometria.hpp"

namespace m688 {

struct CondizioniAob {
    double distanza_m = 0.0;
    double portata_m = 20000.0;         // la portata della vista su quel bersaglio, con la foschia (e la notte)
    double luce = 1.0;                  // 0 notte, 1 giorno (nucleo/sole)
    bool alto_ingrandimento = true;     // il periscopio a 6x o simili; il basso come il giocatore a ±15
    bool solo_vela = false;             // un sottomarino in superficie: si vede la vela
    bool luci = false;                  // di notte coi fanali accesi visti: l'AOB dai colori e dagli alberi
};

struct StimaAob {
    double aob_gradi = 0.0;             // in (-180, 180], a passi di 10: positivo a dritta del bersaglio
    double sigma_gradi = 10.0;
    double rotta = 0.0;                 // rilevamento + 180 - AOB
    bool specchiato = false;            // verita', per le prove e il debriefing: il lato sbagliato vicino all'asse
};

// L'AOB vero di un bersaglio con rotta `rotta_bersaglio` visto col rilevamento `rilevamento` (gradi veri): rilevamento +
// 180 - rotta, in (-180, 180]
double aob_vero(double rilevamento, double rotta_bersaglio);
// La sigma dell'osservazione nelle condizioni date (gradi)
double sigma_aob(const CondizioniAob& c);
// La probabilita' di specchiare l'AOB vicino all'asse (0 oltre 20 gradi dalla prua o dalla poppa)
double probabilita_specchio(double aob_gradi, const CondizioniAob& c);
// La stima: l'AOB vero, il rilevamento a vista e le condizioni
StimaAob stima_aob(double aob_vero_gradi, double rilevamento, const CondizioniAob& c, Casuale& caso);

}  // namespace m688
