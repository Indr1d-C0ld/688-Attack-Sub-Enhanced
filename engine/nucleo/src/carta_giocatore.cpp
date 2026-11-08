#include "nucleo/carta_giocatore.hpp"

namespace m688 {

// I: S -= passo; sotto 2 si ferma a 2 con il rifiuto; poi il passo vale 2 se S < 11, altrimenti scende di 2
int CartaGiocatore::ingrandisci() {
    scala_ -= passo_;
    int rifiuto = 0;
    if (scala_ < 2) {
        scala_ = 2;
        rifiuto = MSG_MASSIMO_INGRANDIMENTO;
    }
    passo_ = scala_ < 11 ? 2 : passo_ - 2;
    if (passo_ < 2) passo_ = 2;
    return rifiuto;
}

// O: oltre 40 il rifiuto; altrimenti il passo vale 2 se S < 11, altrimenti sale di 2, e S += passo
int CartaGiocatore::riduci() {
    if (scala_ > 40) return MSG_MINIMO_INGRANDIMENTO;
    passo_ = scala_ < 11 ? 2 : passo_ + 2;
    scala_ += passo_;
    return 0;
}

}  // namespace m688
