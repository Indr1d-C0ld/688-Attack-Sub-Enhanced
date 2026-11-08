#pragma once
// I pezzi della variante C «Il quadro di comando» per gli strumenti (roadmap 33; docs/ricerca/
// 25_estetica_simulatori.md §8.5, scelta dell'utente del 28/10/2026: gli strumenti come schermi di consolle). Replay,
// statistiche, serie e sandbox stanno in una consolle di lamiera con la targa del posto («SALA ANALISI»); dentro, uno
// o piu' schermi a fosforo con la testata, le linguette delle pagine, le righe di valori. Il piede e' quello comune di
// grafica/lamiera. Indici: il tema delle stazioni (16-47, fosforo 40-47), come le consolle.

#include <string>
#include <string_view>
#include <vector>

#include "grafica/font.hpp"
#include "grafica/tela.hpp"

namespace m688::quadro {

// La consolle a tutto schermo: lamiera e la targa del posto in alto; restituisce l'area utile (sotto la targa)
Rettangolo consolle(Tela& t, const Font& targa, std::string_view posto);

// Uno schermo d'analisi incassato, senza il cappuccio dei CRT delle stazioni: cornice smussata, vetro a fosforo con
// gli angoli arrotondati, la testata (titolo a sinistra, un valore a destra). Restituisce l'area sotto la testata
Rettangolo schermo(Tela& t, Rettangolo r, const Font& f, std::string_view titolo, std::string_view destra = {});

// Le linguette delle pagine sulla riga y dello schermo: la scelta in negativo; restituisce l'altezza usata
int linguette(Tela& t, Rettangolo area, int y, const Font& f, const std::vector<std::string>& pagine, int scelta);

// Una riga di valori: l'etichetta a sinistra e i valori nelle colonne x date (relative all'area)
void riga(Tela& t, Rettangolo area, int y, const Font& f, std::string_view etichetta, const std::vector<int>& colonne,
          const std::vector<std::string>& valori, bool evidente = false);
// Un'intestazione di gruppo: testo pieno e la riga sotto
int intestazione(Tela& t, Rettangolo area, int y, const Font& f, std::string_view testo, const std::vector<int>& colonne = {},
                 const std::vector<std::string>& nomi = {});

// Un riquadro sopra una carta o una figura (la legenda, la lettura del replay): vetro spento, filo di fosforo, il titolo
// in negativo nella prima riga. Restituisce l'area sotto il titolo
Rettangolo riquadro(Tela& t, Rettangolo r, const Font& f, std::string_view titolo);

// La linea del tempo come fascia sottile, alta una riga di testo (il replay; scelta dell'utente del 28/10/2026): a
// sinistra il tempo e lo stato, a destra l'evento corrente, in mezzo la barra. `ora` e le tacche sono frazioni 0-1
// della durata. Una tacca ha il colore del suo evento e l'altezza del suo interesse (0-100, il dato del registro): da 70 a
// tutta altezza (affondamenti, colpi subiti), da 50 piu' corta (lanci), da 30 bassa (scoperte), sotto un punto sulla
// linea. Le piu' interessanti si disegnano per ultime, il cursore sopra tutte. Restituisce la barra, per i clic
struct TaccaTempo {
    double u = 0.0;
    std::uint8_t colore = 0;
    int interesse = 0;
};
Rettangolo fascia_tempo(Tela& t, Rettangolo r, const Font& f, std::string_view sinistra, std::string_view destra, double ora,
                        const std::vector<TaccaTempo>& tacche);

// La barra di una grandezza della serie (scelta dell'utente del 28/10/2026): nella riga r la scala da 0 a `scala`, i
// baffi dal minimo al massimo, la scatola media +- scarto del colore della parte, la media in bianco
void barra_statistica(Tela& t, Rettangolo r, double scala, double media, double scarto, double minimo, double massimo,
                      std::uint8_t colore);

// Lo stato vuoto di uno schermo (le statistiche globali senza missioni; scelta dell'utente del 29/10/2026): un riquadro
// a filo tratteggiato al centro di r, il titolo acceso e le righe che dicono come si riempie
void stato_vuoto(Tela& t, Rettangolo r, const Font& f, std::string_view titolo, const std::vector<std::string>& righe);

// Il profilo di quota del ripetitore di manovra (gruppo 5, scelta dell'utente del 29/10/2026: la manovra e le armi a
// periscopio giu'), nell'area di uno schermo: la scala delle quote a sinistra (0 in alto), i minuti sotto («-10» a
// sinistra, «-0» a destra), lo strato a tratti in ciano, il fondo misurato a retino in basso, la quota periscopica a
// tratti in verde e la traccia del battello. I campioni vanno dal piu' vecchio al piu' recente, a passo regolare sulla
// durata; il fondo ha lo stesso passo, con 0 o meno dove non si e' misurato
struct ProfiloQuota {
    std::vector<double> quota_m;     // la quota del battello
    std::vector<double> fondo_m;     // il fondo dall'ecoscandaglio (vuoto: nessuno)
    double durata_s = 600.0;         // quanto tempo coprono i campioni
    double periscopica_m = 0.0;      // 0: non si disegna
    double strato_m = 0.0;           // 0: nessuno strato
    double scala_m = 0.0;            // il fondo della scala; 0: la sceglie a multipli di 50 m sopra il piu' profondo
};
void profilo_quota(Tela& t, Rettangolo area, const Font& f, const ProfiloQuota& p);

// La vista dal vivo della sandbox in variante C (gruppo 6, scelta dell'utente del 03/11/2026; bozza
// docs/ricerca/25_bozze/strumenti/vivo_C.png): la consolle «SALA ANALISI - DAL VIVO», lo schermo della carta a sinistra
// con la testata (`titolo` a sinistra, `destra` a destra: la lettura, l'ora dal vivo, la velocita'), lo schermo del
// registro a destra, largo `colonna` pixel, con la testata «REGISTRO» e il filtro. Restituisce le aree utili dei due
// schermi e il rettangolo della fascia del tempo, da disegnare con fascia_tempo («ADESSO» a destra). Il piede comune
// (grafica/lamiera::piede) lo mette chi chiama, a y = `piede_y`
struct DisposizioneVivo {
    Rettangolo carta, registro, fascia;
};
DisposizioneVivo consolle_vivo(Tela& t, const Font& targa, const Font& f, std::string_view titolo, std::string_view destra,
                               std::string_view filtro, int colonna = 168, int piede_y = 428);
// Una riga del registro dal vivo, all'altezza y dell'area: l'ora in fosforo spento, il testo nel suo colore, tagliato
// alla larghezza; `scelta` la mette in negativo (l'evento sotto il cursore)
void riga_registro(Tela& t, Rettangolo area, int y, const Font& f, std::string_view ora, std::string_view testo,
                   std::uint8_t colore, bool scelta = false);

}  // namespace m688::quadro
