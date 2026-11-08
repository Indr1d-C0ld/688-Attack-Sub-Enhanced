#pragma once
// Navi civili generate per individuo (blocco 1 del sonar, docs/20_sonar_blocco1.md §2): dal tipo ([civili.*] di
// acustica.toml) e da un seme, una nave intera con la sua firma, sempre la stessa per lo stesso seme.
//
// Fonti: docs/ricerca/26_navi_civili_traffico.md, che raccoglie gli appunti di 26_note/rumore_navi.md:
// - il livello: il modello JOMOPANS-ECHO (MacGillivray e de Jong 2021, J. Mar. Sci. Eng. 9:369, CC BY), sulla classe,
//   sulla velocita' (60 log V) e sulla lunghezza (20 log l), integrato su 5-200 Hz come le firme del motore; scarto
//   dell'individuo 4 dB (ECHO: 6 dB con la misura, STIMA);
// - sotto la velocita' d'innesco della cavitazione 6 dB in meno (STIMA), con il minimo delle macchine 30 dB sotto
//   il livello di servizio;
// - le righe (ITTC 2014, Arveson e Vendittis 2000): asse e pala dall'elica; le accensioni del diesel lento in presa
//   diretta legate all'asse (cilindri x giri / 60); dei diesel medi con riduttore (cilindri x giri / 120), fisse col
//   passo variabile; i generatori a giri fissi con la loro rete a 50 o 60 Hz e le armoniche; la turbina e la ruota
//   del riduttore legate all'asse; le pompe degli ausiliari.
//
// Il modulo e' puro.

#include <cstdint>

#include "nucleo/acustica.hpp"

namespace m688 {

struct IndividuoCivile {
    FirmaRumore firma;
    double lunghezza_m = 0.0, larghezza_m = 0.0, pescaggio_m = 0.0;
    double velocita_servizio_kn = 0.0, velocita_max_kn = 0.0;
};
// La nave del tipo per quel seme. firma.classe = id del tipo
IndividuoCivile genera_nave_civile(const TipoCivile& t, std::uint64_t seme);

// Il livello a banda larga 5-200 Hz (dB re 1 uPa @ 1 m) del modello ECHO per il tipo, la velocita' e la lunghezza,
// senza scarto ne' innesco della cavitazione
double livello_echo_db(const TipoCivile& t, double velocita_kn, double lunghezza_m);

// la stringa del tipo di propulsione nei dati ("diesel_lento", ...); std::runtime_error se ignota
Propulsione propulsione_da_testo(const std::string& testo);

}  // namespace m688
