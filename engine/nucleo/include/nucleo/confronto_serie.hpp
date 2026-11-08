#pragma once
// Il confronto statistico delle serie A/B del vetrino di Dio (docs/28 §5, D2; le statistiche di docs/19 §4): due serie
// sugli stessi semi, A senza e B con l'intervento, partita i di A in coppia con la partita i di B. Puro e deterministico
// (il bootstrap col suo seme): lo usano lo strumento delle serie e la schermata del confronto di trailer-1.
//
// - Gli esiti: la frequenza in A e in B con l'intervallo di Wilson, e la differenza B - A con l'intervallo di Newcombe
//   per dati appaiati (Newcombe 1998, Statistics in Medicine 17:2635, metodo 10: dai due intervalli di Wilson e dalla
//   correlazione phi delle coppie), che col disegno a coppie e' piu' stretto di quello per campioni indipendenti.
//   Verificata sull'esempio di Armitage e Berry (1994, p. 138; celle 20, 12, 2, 16): StatsDirect da' 0,056156-0,329207,
//   qui 0,0562-0,3292 (la prova). Il testo dell'articolo non l'ho potuto rileggere il 31/10/2026 (non rispondeva)
// - I numeri (minuti alla prima scoperta, lanci, perdite...): la differenza delle mediane B - A con l'intervallo
//   bootstrap a percentili ricampionando le coppie, e quante coppie salgono, scendono o restano uguali. Un valore
//   mancante (< 0, come in numeri_parte) toglie la coppia.
// - Il punto di divergenza di una coppia: il primo istante in cui la verita' delle due tracce si separa (un'unita' in piu'
//   o in meno, o spostata, girata, salita) e il primo evento del registro che cambia.
#include <cstdint>
#include <map>
#include <optional>
#include <string>
#include <vector>

#include "nucleo/statistiche_sandbox.hpp"
#include "nucleo/traccia_replay.hpp"

namespace m688 {

inline constexpr double Z_95 = 1.959963984540054;

struct IntervalloWilson {
    int k = 0, n = 0;
    double p = 0.0, basso = 0.0, alto = 0.0;
};
IntervalloWilson wilson(int k, int n, double z = Z_95);

// Un esito confrontato: vero se la partita l'ha avuto
struct ConfrontoEsito {
    IntervalloWilson a, b;
    double differenza = 0.0, basso = 0.0, alto = 0.0;   // B - A, con l'intervallo di Newcombe appaiato
    int solo_a = 0, solo_b = 0, entrambe = 0, nessuna = 0;   // le coppie: l'esito in A e non in B, in B e non in A...
    bool significativo() const { return basso > 0.0 || alto < 0.0; }
};
ConfrontoEsito confronta_esito(const std::vector<bool>& a, const std::vector<bool>& b, double z = Z_95);

struct ParametriBootstrap {
    int ricampionamenti = 2000;
    double livello = 0.95;
    std::uint64_t seme = 0x688B0075u;
};
struct ConfrontoNumero {
    int coppie = 0;                                     // con entrambi i valori
    double mediana_a = 0.0, mediana_b = 0.0;
    double differenza = 0.0, basso = 0.0, alto = 0.0;   // mediana di B - mediana di A, intervallo bootstrap
    int sale = 0, scende = 0, uguale = 0;               // le coppie: B maggiore, minore, uguale ad A
    bool significativo() const { return coppie > 1 && (basso > 0.0 || alto < 0.0); }
};
ConfrontoNumero confronta_numero(const std::vector<double>& a, const std::vector<double>& b, const ParametriBootstrap& par = {});

// Il punto di divergenza di una coppia
struct Divergenza {
    bool divergono = false;
    double t_s = -1.0;                  // il primo istante in cui la verita' e' diversa (-1: mai)
    int unita = -1;                     // la prima unita' diversa a quell'istante
    std::string come;                   // «compare», «manca», «posizione», «rotta», «quota», «velocita'», «stato»
    double t_intervento_s = -1.0;       // il primo intervento applicato in B (-1: nessuno)
    double dopo_intervento_s = -1.0;    // t_s - t_intervento_s, se ci sono entrambi
    // il primo evento del registro che cambia, senza gli interventi dello spettatore: l'indice nella sequenza dei soli
    // fatti e i due eventi (uno dei due puo' mancare)
    int indice_evento = -1;
    std::optional<EventoTraccia> evento_a, evento_b;
};
struct SoglieDivergenza {
    double posizione_m = 1.0, rotta_gradi = 0.5, quota_m = 0.5, velocita_kn = 0.1, tempo_evento_s = 0.05;
};
Divergenza punto_divergenza(const Traccia& a, const Traccia& b, const SoglieDivergenza& s = {});

// Il confronto di due serie (lo stesso numero di partite, in coppia per indice)
struct ConfrontoSerie {
    int coppie = 0;
    std::map<int, ConfrontoEsito> esiti;                                  // codice d'esito (-1: non finita)
    std::map<int, std::map<std::string, ConfrontoNumero>> parti;          // parte -> nome di numeri_parte
    std::map<std::string, ConfrontoNumero> generali;                      // durata_s
};
ConfrontoSerie confronta_serie(const std::vector<StatisticheSandbox>& a, const std::vector<StatisticheSandbox>& b,
                               const ParametriBootstrap& par = {});

}  // namespace m688
