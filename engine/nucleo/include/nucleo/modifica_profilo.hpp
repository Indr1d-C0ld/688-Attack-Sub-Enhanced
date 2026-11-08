#pragma once
// Lo strato termico che cambia: il vetrino di Dio, D4 (docs/28 §3.3; nota disassemblatore-d4-strato-ghiaccio-proposta.md).
// Dal profilo del teatro un profilo modificato, gia' analizzato, come applica_pomeriggio. Chi lo usa lo tiene da parte con
// un puntatore stabile: il puntatore entra nella chiave delle cache dei raggi, quindi un profilo nuovo fa ricalcolare i
// campi. Il modulo e' puro. STIMA di gioco: nessun dato dice come si sposta uno strato vero in pochi minuti, e qui conta
// che la geometria acustica sia quella di uno strato a quella quota.

#include "nucleo/acustica.hpp"

namespace m688 {

struct ModificaStrato {
    enum class Tipo {
        sposta,     // lo strato rimescolato a `valore` metri (al piu' 400: la SLD si cerca li'), il termoclino sotto di lui
        togli,      // niente condotto: il gradiente negativo dalla superficie
        rinforza,   // il termoclino `valore` volte piu' ripido (sotto 1: piu' dolce)
        canale      // l'asse del canale sonoro a `valore` metri
    };
    Tipo tipo = Tipo::togli;
    double valore = 0.0;
};

// Le acque polari non hanno un termoclino sotto la SLD (c sale dalla superficie: un condotto che e' tutta la colonna, e la
// SLD e' solo il tetto della ricerca): lo strato non si sposta e non si rinforza, e senza l'asse sotto lo strato il canale
// non si muove. Togliere il condotto si puo' sempre, se c'e'. Chi chiede l'intervento rifiuta con questa
bool modifica_applicabile(const ProfiloSuono& p, ModificaStrato::Tipo t);

// A che punto e' un cambiamento a `t`: 0 spento, 1 pieno, in mezzo la transizione, all'andata e al ritorno. Con `durata_s`
// > 0, dopo la transizione e la durata torna indietro nello stesso tempo; con 0 resta per sempre (strato e ghiaccio)
double fase_cambiamento(double t, double t0, double transizione_s, double durata_s);

// `frazione` 0..1: la transizione, una media dei due profili livello per livello. Con 0, o se la modifica non e'
// applicabile, il profilo di partenza
ProfiloSuono modifica_profilo(const ProfiloSuono& p, const ModificaStrato& m, double frazione = 1.0);

}  // namespace m688
