#include "nucleo/guasti_giocatore.hpp"

namespace m688 {

int GuastiGiocatore::messaggio(int codice) {
    if (codice != MSG_BOMBA && codice != MSG_COLPITI && codice != MSG_FONDO) return -1;
    if (rand_() % 2 == 0) return -1;
    const int i = rand_() % SISTEMI;
    if (guasti_[static_cast<std::size_t>(i)]) return -1;   // gia' guasto: niente, senza riprovare
    guasti_[static_cast<std::size_t>(i)] = true;
    return i;
}

}  // namespace m688
