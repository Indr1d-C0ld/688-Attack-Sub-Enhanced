#pragma once
// Posizione del Sole (algoritmo semplificato della NOAA, errore di qualche decimo di grado): serve per la luce
// della scena, il ciclo giorno/notte e, in seguito, per il punto nave astronomico al periscopio.

namespace m688 {

struct DataOra {
    int anno = 1989, mese = 10, giorno = 15;
    double ore_utc = 12.0;   // ore decimali, tempo universale
};

// Elevazione del Sole sull'orizzonte (gradi) per latitudine e longitudine in gradi (est positiva).
double elevazione_sole(const DataOra& t, double latitudine, double longitudine);

// Luce ambiente da 0 (notte senza Luna) a 1 (pieno giorno) in funzione dell'elevazione del Sole:
// crepuscolo nautico (-12 gradi) come buio, pieno giorno da +10 gradi in su.
double luce_ambiente(double elevazione_sole_gradi);

}  // namespace m688
