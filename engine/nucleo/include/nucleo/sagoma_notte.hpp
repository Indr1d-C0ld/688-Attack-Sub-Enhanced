#pragma once
// Vale a ogni luce, dal buio al pieno giorno (docs/27 e la vista di giorno, 29/10/2026): il nome dice da dove e' nato.
// La sagoma di una nave a luci spente di notte (docs/27 §2, la simmetria: «una a luci spente solo da vicino»): al posto
// dei 3 km fissi della vedetta notturna, una portata dalla luce del cielo (stelle, Luna, crepuscolo, nubi), dalla
// dimensione e dall'aspetto della sagoma, dal binocolo e dal sensore, con la foschia e l'orizzonte. Solo nell'Enhanced.
// Modulo puro e deterministico; tutti i numeri della vista sono STIME tarate su pochi punti:
// - una nave di 130 m (sagoma alta in media 12 m) al traverso, con le stelle, il sereno e 30 km di visibilita', a occhio
//   nudo da una vedetta allertata: 3 km (i 3 km di oggi);
// - in aria limpida col 7x50 il doppio, con la Luna piena alta il doppio; con la foschia dei 30 km ne resta meno (il
//   7x50 4,5 km, la piena 5,7); col coperto senza Luna 2,5 km; di prora meta'; il 688 in superficie 1,5 km.
// Il modello:
// - la luce del cielo (lux): stelle 0,002 col sereno; la Luna dalla fase e dall'altezza (0,25 la piena allo zenit, con
//   la fase alla Allen: 10^(-0,4 (0,026 |i| + 4e-9 i^4)), i l'angolo di fase); il crepuscolo dall'altezza del Sole
//   (tavole del cielo sereno: 0,008 lux a -12 gradi, 3 a -6, 700 a 0); le nubi tolgono fino all'85% (stelle e Luna) e
//   al 75% (Sole);
// - in aria limpida (senza foschia) la portata va con la dimensione della sagoma (radice dell'area vista), con la luce alla 0,143
//   (la Luna piena alta, 125 volte le stelle, raddoppia) e con l'ingrandimento alla 0,356 (il 7x50 raddoppia);
// - la foschia: la sagoma va trovata con la soglia di contrasto che cresce col quadrato della distanza (Ricco) e il
//   contrasto che cala con Koschmieder: (d / R)^2 exp(k 3,912 d / V) = 1, con k = ln 50 / ln(1 / soglia): la soglia per
//   la sagoma scura e' 0,02 di giorno e sale al buio (0,28 con le stelle, 0,1 con la Luna piena), quindi di notte la
//   sagoma si perde ben prima della visibilita' meteorologica; e mai oltre il limite del contrasto (di giorno la
//   visibilita' stessa, di notte una frazione);
// - la Luna bassa (sotto i 30 gradi) dietro la nave, entro 30 gradi di rilevamento, la stacca sul cielo chiaro: x1,5;
// - mai oltre l'orizzonte della sovrastruttura.
// La posizione della Luna e' quella a bassa precisione (Meeus, Astronomical Algorithms cap. 47 ridotto ai termini
// maggiori): un paio di gradi, che bastano per la luce.

#include <string>

#include "nucleo/periscopi.hpp"
#include "nucleo/sole.hpp"

namespace m688 {

// La Luna per data e luogo (gradi; azimut dal nord in senso orario)
struct PosizioneLuna {
    double elevazione = 0.0, azimut = 0.0;
    double angolo_fase = 0.0;   // 0 la piena, 180 la nuova
    double illuminata = 0.0;    // frazione del disco illuminata, 0-1
    double angolo_lembo = 0.0;  // dove guarda il lembo illuminato (verso il Sole) sul disco visto in cielo: 0 in alto
                                // (lo zenit), 90 a destra per chi guarda la Luna, 180 in basso, 270 a sinistra
};
PosizioneLuna posizione_luna(const DataOra& t, double latitudine, double longitudine);

// La luce del cielo di notte e al crepuscolo per la vista delle sagome
struct CieloNotte {
    double lux = 0.002;                 // l'illuminamento sul mare
    bool luna_su = false;
    double luna_elevazione = 0.0, luna_azimut = 0.0;
    // per disegnare la falce: la frazione illuminata (0 senza data) e l'angolo del lembo come in PosizioneLuna
    double luna_illuminata = 0.0, luna_angolo_lembo = 0.0;
};
// Dalla data, dal luogo e dalle nubi (ottavi)
CieloNotte cielo_notte(const DataOra& t, double latitudine, double longitudine, double nubi_ottavi);
// Senza data: dalla luce ambiente di nucleo/sole (0-1) e dalle nubi, senza Luna
CieloNotte cielo_notte(double luce, double nubi_ottavi);
// L'illuminamento del crepuscolo e del giorno per l'altezza del Sole (lux, cielo sereno)
double lux_sole(double elevazione_sole_gradi);

struct Sagoma {
    double lunghezza_m = 100.0;
    double altezza_media_m = 10.0;      // l'area della fiancata sul mare divisa per la lunghezza (scafo, sovrastruttura,
                                        // alberi); per un sottomarino in superficie lo scafo e la vela, ~2 m
    double altezza_max_m = 25.0;        // la cima (albero, vela) per l'orizzonte
    double larghezza_m = 0.0;           // 0: un ottavo della lunghezza
};
// Una sagoma dalla lunghezza e dal tipo (STIMA per le classi senza dati): mercantile, militare, peschereccio,
// sottomarino (in superficie)
Sagoma sagoma_da_tipo(const std::string& tipo, double lunghezza_m);
// La dimensione equivalente (m): la radice dell'area vista. `aspetto` in gradi dalla prua verso chi guarda
// (0 di prora, 90 al traverso)
double dimensione_sagoma_m(const Sagoma& s, double aspetto);

inline constexpr double INGRANDIMENTO_BINOCOLO = 7.0;   // il 7x50 delle vedette

// La portata della sagoma per l'osservatore: `cielo` la luce, `c` la visibilita' (e le nubi, gia' nel cielo), `aspetto`
// come sopra, `rilevamento` dall'osservatore alla nave (per la Luna dietro), `ingrandimento` 1 l'occhio nudo. Il
// sensore notturno dell'osservatore: la TV e la bassa luminosita' x30 sulla luce, l'intensificatore x1000, il termico
// come un crepuscolo di 1 lux senza la Luna (STIME)
double portata_sagoma_notte_m(const Sagoma& s, double aspetto, const Osservatore& o, const CondizioniVista& c,
                              const CieloNotte& cielo, double rilevamento, double ingrandimento = 1.0);
// La probabilita' di vederla nei dt secondi a `distanza_m`: la meta' del tasso massimo alla portata, come per la testa
// del periscopio (nucleo/periscopi, nave: 0,1 al secondo); 0 oltre l'orizzonte e oltre due volte la portata
double probabilita_sagoma_notte(const Sagoma& s, double aspetto, const Osservatore& o, const CondizioniVista& c,
                                const CieloNotte& cielo, double rilevamento, double distanza_m, double dt,
                                double ingrandimento = 1.0);

}  // namespace m688
