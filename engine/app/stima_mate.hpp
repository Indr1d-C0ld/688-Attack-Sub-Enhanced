#pragma once
// Quando la soluzione del MATE vale come rotta stimata (obiettivo rotta_stimata con da_tma, LIB-S20): se i soli
// rilevamenti del contatto hanno una soluzione con la distanza determinata (nucleo/tma_ia) e quella del giocatore li
// spiega altrettanto bene. Niente verita': la rotta vera la mette la missione, per il registro.

#include "nucleo/tma.hpp"
#include "nucleo/tma_ia.hpp"

namespace m688 {

inline constexpr double SIGMA_TMA_MATE_DEG = 1.5;   // come il comandante IA (missioni/comandante_ia.cpp)
inline constexpr double SCARTO_MATE_DEG = 1.0;      // STIMA: quanto il MATE puo' spiegare i rilevamenti peggio della soluzione migliore

inline bool stima_mate_valida(const Tma& tma, const SoluzioneTma& mate, double t) {
    const auto s = risolvi_tma(tma, t, SIGMA_TMA_MATE_DEG);
    return s && s->osservabile && tma.scarto_quadratico_medio(mate) <= s->scarto_deg + SCARTO_MATE_DEG;
}

}  // namespace m688
