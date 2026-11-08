#pragma once
// I ripetitori delle stazioni dell'Enhanced, 688 e Alfa (gruppo 5 della rifinitura estetica, roadmap 33; bozze
// approvate dall'utente il 29/10/2026 in docs/ricerca/25_bozze/stazioni, misure in strumenti/bozze_strumenti.cpp; sull'Alfa
// il tema beige e le scritte scure, Fonti::scritte):
// - Manovra (F4) col periscopio giu': al posto della vista sott'acqua il profilo di quota degli ultimi minuti e gli
//   ordini contro lo stato (variante C, schermi a fosforo);
// - Armi (F7) col periscopio giu': le armi in acqua, l'arma scelta, i tubi, il carico e la ricarica (variante C);
// - Carta (F6): il pannello di sotto con lo stato in spie e vetri, i comandi come tasti della tastiera (variante A).
// La radio (F2) e' in stazione_radio (StazioneRadio::disegna_sala). Qui solo il disegno: i dati arrivano gia' composti
// dall'interfaccia (nessuna dipendenza dalla missione). Indici: il tema delle stazioni (16-47) e gli EGA.

#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "grafica/font.hpp"
#include "grafica/lamiera.hpp"
#include "grafica/quadro.hpp"
#include "grafica/tela.hpp"

namespace m688::ripetitori {

// I tre font del gruppo 5: l'originale (lamiera, vetri), l'incisione (scritte incise; lo stampino sull'Alfa), il
// fosforo 5x7 (schermi, anche sull'Alfa); e le scritte sulla lamiera: chiare sul grigio del 688, scure sul beige
// dell'Alfa (lamiera::scritte_per)
struct Fonti {
    const Font& orig;
    const Font& incisa;
    const Font& schermo;
    lamiera::Scritte scritte = lamiera::Scritte::chiare;
};

// La larghezza e la scrittura alla scala 1 con lo spazio del font (i FNT2 hanno il loro), come i pezzi di grafica;
// scrivi restituisce la x dopo il testo
int largo(const Font& f, std::string_view s);
int scrivi(Tela& t, const Font& f, int x, int y, std::string_view s, std::uint8_t c);
// Il testo accorciato a destra finche' sta in w pixel
std::string taglia(const Font& f, std::string s, int w);

// --- Manovra ---
struct RigaOrdinato {
    std::string nome, ordinato, attuale;   // «-» se non c'e' un ordine
};
struct DatiManovra {
    quadro::ProfiloQuota profilo;
    std::string titolo = "PROFILO DI QUOTA, ULTIMI 10 MIN";
    bool alfa = false;                     // le testate traslitterate dei pulti dell'Alfa (kit, 31/10/2026)
    std::vector<RigaOrdinato> righe;       // ROTTA, VELOCITA', QUOTA, TIMONE, PIANI, MACCHINA, ZAVORRA
    std::string chiglia;                   // «83 M, IN SALITA»
    std::string cavitazione;               // «CAVITAZIONE A 7,5 KN»
    bool chiglia_misurata = true;          // falso: la riga in un fosforo spento
};
void manovra(Tela& t, const Fonti& f, const DatiManovra& d);

// --- Armi ---
struct ArmaInAcqua {
    std::string numero, arma, corsa, filo, modo, stato, bersaglio;
};
struct TuboRipetitore {
    std::string numero, arma, stato;
    bool scelto = false;
};
struct DatiArmiRipetitore {
    std::vector<ArmaInAcqua> in_acqua;
    std::string fili;                      // a destra nella testata: «2 FILI SU 4»
    bool alfa = false;                     // le testate traslitterate dei pulti dell'Alfa (kit, 31/10/2026)
    std::string scelta_titolo;             // «L'ARMA SCELTA: 1 MK48»; vuoto: nessuna
    std::vector<std::string> scelta;       // quattro righe al piu'
    std::vector<TuboRipetitore> tubi;
    std::vector<std::string> carico, ricarica;
};
void armi(Tela& t, const Fonti& f, const DatiArmiRipetitore& d);

// --- Carta (il pannello di sotto, 360-480) ---
struct DatiPannelloCarta {
    std::string titolo, ora;               // la riga incisa in alto: a sinistra e a destra
    bool pilota = false, proiezione = false, scie = false;
    int minuti = 10, punti = 0, punti_max = 9, scala = 3;
    std::string bersaglio, bersaglio_dati; // vuoto: «NESSUNO»
    std::vector<std::pair<std::string, std::string>> comandi;   // il tasto e la parola
    std::vector<std::pair<std::string, std::uint8_t>> modo;     // MATE o fondale: righe al posto dei tasti
    std::string messaggio;                 // la riga dell'equipaggio
};
void pannello_carta(Tela& t, const Fonti& f, const DatiPannelloCarta& d);

}  // namespace m688::ripetitori
