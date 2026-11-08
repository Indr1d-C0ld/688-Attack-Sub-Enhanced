#pragma once
// La portata visiva delle luci di navigazione con la foschia (docs/27_luci_navigazione.md, la parte del
// disassemblatore): per la vedetta dell'IA e per il giocatore illuminato visto, la stessa legge della vista del
// periscopio. Solo nell'Enhanced. Modulo puro e deterministico sopra nucleo/luci_navigazione (i fanali del kit) e
// nucleo/periscopi (Osservatore, CondizioniVista).
// - L'intensita' dalla portata del regolamento (COLREG 1972, Annesso I §8: I = 3,43e6 x T x D^2 x K^-D, con T = 2e-7
//   lux e K = 0,8 per miglio, cioe' circa 13 miglia di visibilita'): 6 miglia sono 94 candele, 3 miglia 12.
// - La legge di Allard: all'occhio arriva E = I x exp(-sigma d) / d^2, con sigma dalla visibilita' del meteo
//   (Koschmieder col contrasto 0,02: sigma = 3,912 / V, come nucleo/meteo). Il fanale si vede finche' E supera la
//   soglia: 2e-7 lux di notte col fondo buio, che sale col giorno come nella vista (grafica/periscopio, luce_fanale);
//   la vedetta non allertata ha bisogno del triplo (STIMA: deve trovare la luce, non solo vederla); il binocolo e i
//   sensori notturni dividono la soglia (STIME).
// - Mai oltre l'orizzonte del fanale: l'occhio e l'altezza della luce, che sta sopra lo scafo (la luce si vede prima
//   della sagoma). Le onde sulla finestra del periscopio restano al chiamante (StatoPeriscopio::finestra_coperta).
// - Solo i fanali dello stato che guardano l'osservatore (accesi, nel_settore). Il faro ambra a lampi conta come una
//   luce accesa un quarto del tempo, ma piu' vistosa (STIMA: meta' del tasso d'una luce fissa).

#include "nucleo/luci_navigazione.hpp"
#include "nucleo/periscopi.hpp"

namespace m688::luci {

inline constexpr double SOGLIA_NOTTE_LUX = 2e-7;    // Annesso I §8, la T
inline constexpr double K_ANNESSO = 0.8;            // trasmissivita' per miglio dell'Annesso I
inline constexpr double NON_ALLERTATA = 3.0;        // la vedetta che non sa dove guardare (STIMA)
// Il guadagno d'un'ottica sui punti luminosi (un fanale e' una sorgente puntiforme: l'ingrandimento non lo fa piu' grande,
// conta la luce raccolta): trasmissione x (apertura utile / pupilla)^2, con l'apertura utile = min(obiettivo, ingrandimento
// x pupilla), perche' una pupilla d'uscita piu' larga di quella dell'occhio spreca la luce in piu'. Di notte la pupilla e'
// di 7 mm. Il 7x50: circa 42; il periscopio d'attacco a 6x (obiettivo 30 mm, trasmissione 0,3: STIMA) circa 5,5, a 1,5x
// circa 0,4. Una sola legge per la vedetta dell'IA (portata_fanale_m, probabilita_vista_luci) e per la vista del giocatore
// (rapporto_soglia, grafica/periscopio luce_fanale)
constexpr double guadagno_ottico(double obiettivo_mm, double trasmissione, double ingrandimento, double pupilla_mm = 7.0) {
    const double utile = obiettivo_mm < ingrandimento * pupilla_mm ? obiettivo_mm : ingrandimento * pupilla_mm;
    return pupilla_mm > 0.0 ? trasmissione * (utile / pupilla_mm) * (utile / pupilla_mm) : 0.0;
}
inline constexpr double BINOCOLO_OBIETTIVO_MM = 50.0, BINOCOLO_TRASMISSIONE = 0.85, BINOCOLO_INGRANDIMENTO = 7.0;
inline constexpr double PONTE_STABILITA = 0.25;    // il 7x50 a mano sul ponte che si muove ne rende un quarto (STIMA)
inline constexpr double GUADAGNO_BINOCOLO =        // il 7x50 delle vedette: circa 10
    guadagno_ottico(BINOCOLO_OBIETTIVO_MM, BINOCOLO_TRASMISSIONE, BINOCOLO_INGRANDIMENTO) * PONTE_STABILITA;
inline constexpr double TASSO_LUCI_S = 0.2;         // scoperte per secondo con la luce ben sopra la soglia (STIMA)

// L'intensita' (candele) per la portata del regolamento (m), e il contrario
double intensita_cd(double portata_m);
double portata_nominale_m(double intensita_cd);

// La soglia all'occhio (lux): di notte 2e-7, fino a 3000 volte di piu' a pieno giorno (come la vista); il triplo per la
// vedetta non allertata; divisa per `guadagno` (binocolo, sensore)
double soglia_lux(double luce, bool allertato, double guadagno = 1.0);
// Il guadagno del sensore notturno sui punti luminosi (STIMA): la bassa luminosita' e la TV 3, l'intensificatore 10; il
// termico non vede le luci, ma l'occhio resta (1)
double guadagno_sensore(ModoVista sensore);

// La legge di Allard: dove la luce scende alla soglia (m); 0 senza luce
double portata_luminosa_m(double intensita_cd, double visibilita_m, double soglia_lux);
// L'orizzonte d'un punto alto `altezza_m` per l'occhio a `occhio_m` (con la rifrazione, come nucleo/periscopi)
double orizzonte_luce_m(double occhio_m, double altezza_m);

// La portata d'un fanale per l'osservatore: Allard con la foschia, la luce, l'allerta, il sensore e `guadagno_ottico`
// (GUADAGNO_BINOCOLO per le vedette, 1 l'occhio nudo), mai oltre l'orizzonte della luce
double portata_fanale_m(const Fanale& f, const Osservatore& o, const CondizioniVista& c, double guadagno_ottico = 1.0);

// Le luci d'un'unita' viste da un osservatore col rilevamento relativo `relativo` (gradi dalla prua dell'unita' verso
// chi guarda, positivi a dritta, come nel_settore): il fanale che arriva piu' lontano. Il faro a lampi conta a ogni
// istante (la portata non dipende dal lampo)
struct VistaLuci {
    double portata_m = 0.0;
    const Fanale* migliore = nullptr;   // nella configurazione; nullptr senza luci
};
VistaLuci vista_luci(const Configurazione& c, Stato s, double relativo, const Osservatore& o, const CondizioniVista& cv,
                     double guadagno_ottico = 1.0);

// La probabilita' che l'osservatore veda le luci nei dt secondi a `distanza_m`: il tasso e' TASSO_LUCI_S ben dentro la
// portata, la meta' a 0,8 della portata, e cade di netto verso la portata (la soglia e' quasi un limite); 0 oltre. Per
// ogni fanale il suo tasso (il faro a lampi la meta'), e conta il migliore
double probabilita_vista_luci(const Configurazione& c, Stato s, double relativo, const Osservatore& o,
                              const CondizioniVista& cv, double distanza_m, double dt, double guadagno_ottico = 1.0);

// Il rapporto fra la luce all'occhio e la soglia di notte, per la vista: lo stesso di grafica/periscopio luce_fanale
// (1 alla portata nominale con la visibilita' dell'Annesso), con la K dell'Annesso
// `guadagno`: quello dell'ottica (guadagno_ottico), che moltiplica la luce; 1 l'occhio nudo
double rapporto_soglia(double portata_m, double distanza_m, double visibilita_m, double luce_ambiente, double guadagno = 1.0);

}  // namespace m688::luci
