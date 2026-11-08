#pragma once
// La TMA che risolve da sola (fase 2 delle interazioni fra IA, docs/18_interazioni_ia.md; nota condivisa
// ia-acustica-fase2-proposta.md, punto A): dal passivo l'IA ha solo i rilevamenti, come il giocatore, e la posizione
// la ricava da questi. Stessa Tma del giocatore (nucleo/tma.hpp: rilevamenti con la posizione propria) e stesso modello
// del bersaglio: moto rettilineo uniforme riferito all'ultimo rilevamento.
//
// risolvi_tma cerca la soluzione che minimizza lo scarto quadratico dei rilevamenti: una griglia di distanza (in log),
// rotta e velocita', poi un raffinamento per coordinate. Dalla forma del minimo dice se la distanza e' osservabile:
// senza una manovra propria (o con troppo poco tempo) lo scarto resta piatto su un largo intervallo di distanze
// (Ekelund, dossier 02 §5.1) e la posizione non si da'. Con la manovra converge in 10-30 minuti dalla geometria.
// Le osservazioni di rotta del periscopio (Tma::rotte, libretto di riconoscimento docs/25 §3.4) entrano come termini in
// piu', pesati con la loro sigma nelle unita' dei rilevamenti: fissano la rotta e, su una gamba sola, anche la distanza.
// Il modulo e' puro e deterministico.

#include <optional>

#include "nucleo/tma.hpp"

namespace m688 {

struct StimaTma {
    SoluzioneTma soluzione;              // riferita all'ultimo rilevamento (distanza lungo quello, rotta, velocita')
    Vec2 posizione;                      // del bersaglio al tempo richiesto
    bool osservabile = false;            // la distanza e' determinata: si puo' dare la posizione
    double sigma_distanza_m = 0.0;       // incertezza lungo il rilevamento (meta' dell'intervallo compatibile)
    double sigma_trasversale_m = 0.0;    // attraverso: distanza x errore del rilevamento
    double scarto_deg = 0.0;             // scarto quadratico medio dei rilevamenti con la soluzione (con le osservazioni di
                                         // rotta pesate nelle unita' dei rilevamenti, se ce ne sono)
};

// Almeno RILEVAMENTI_MIN rilevamenti in almeno ARCO_MIN_S secondi, altrimenti nullopt. `sigma_deg`: l'errore dei
// rilevamenti del sensore (errore_rilevamento_deg). La posizione e' al tempo `t`.
inline constexpr int RILEVAMENTI_MIN_TMA = 6;
inline constexpr double ARCO_MIN_TMA_S = 120.0;
std::optional<StimaTma> risolvi_tma(const Tma& tma, double t, double sigma_deg);

}  // namespace m688
