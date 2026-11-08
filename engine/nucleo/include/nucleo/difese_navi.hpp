#pragma once
// Le difese delle navi contro i missili del modello, solo Enhanced (dossier docs/ricerca/05 §4; manuale D3 §2.3 e §2.6).
// Tre regole allo stesso punto, l'arrivo di un missile su una nave:
// - l'allerta: sotto i posti di combattimento la nave usa solo una parte del byte `difesa` e del Phalanx;
// - la saturazione: ogni missile impegna un canale di guida; con tutti i canali impegnati il byte non conta;
// - il Phalanx: fra il byte e il colpo, un ingaggio per missile, se c'e' un affusto libero.
// Valgono allo stesso modo per il giocatore e per l'IA. Il modulo e' puro: non conosce la missione, che gli passa l'ora,
// il byte della nave e il fattore dell'allerta. I dati delle classi sono in data/motore/missili.toml, [[difesa_nave]]
// (tools/pipeline/missili.py), con fonte e confidenza; una classe fuori tabella resta come prima.
//
// Numeri casuali: il primo tiro (il byte) si estrae sempre, come prima di queste regole; il secondo solo se c'e' un
// affusto libero e il byte non ha fermato il missile. Unita': secondi, m/s.

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include "nucleo/casuale.hpp"

namespace m688 {

struct DifesaClasse {
    std::string classe, fonte, confidenza, nota;
    int phalanx = 0;      // affusti (nessun arco di tiro: vedono tutto)
    int canali = 1;       // canali di guida: quanti missili insieme il byte puo' fermare
    int block1_dal = 0;   // l'anno del Phalanx Block 1; 0 = mai
};

// costanti delle regole (dossier 05 §4.1-§4.4; STIMA dove il manuale D3 §2.6 lo dice)
inline constexpr double FATTORE_A0 = 0.5, FATTORE_A1 = 0.75;   // STIMA: A0 dal rapporto 0,48 di §4.3, A1 a meta' con A2
inline constexpr double CANALE_IMPEGNATO_S = 30.0;             // STIMA: Osa-M, 2 colpi al minuto (§4.2)
inline constexpr double PK_PHALANX_BLOCK0 = 0.35, PK_PHALANX_BLOCK1 = 0.45;   // §4.4, STIMA del dossier
inline constexpr double SUPERSONICO_M_S = 340.0;               // STIMA: la soglia; P-700 e P-500 sopra, P-120 e Harpoon sotto
inline constexpr double FATTORE_SUPERSONICO = 0.5;             // §4.4
inline constexpr double PORTATA_PHALANX_M = 1490.0;            // §4.1: portata efficace del Block 0

// Il fattore su byte e Phalanx per un livello d'allerta (A0-A3); nullopt a forze spente: 1, come prima
double fattore_allerta(std::optional<int> livello);
int byte_in_allerta(int byte, double fattore);    // arrotondato in giu'
// Pk di un ingaggio contro un missile a quella velocita', per la classe nell'anno e col fattore dell'allerta
double pk_phalanx(const DifesaClasse& d, int anno, double velocita_m_s, double fattore);

enum class EsitoDifesa : std::uint8_t { colpo, fermato_byte, fermato_phalanx };

// Canali e affusti impegnati di una nave. Si tira all'arrivo di ogni missile, nell'ordine d'arrivo
class ImpegnoDifese {
public:
    // `classe` nullptr: fuori tabella, solo il byte intero, senza limiti. `t`: l'ora dell'arrivo
    EsitoDifesa arrivo(const DifesaClasse* classe, int byte, double fattore, double t, double velocita_m_s, int anno, Casuale& caso);

private:
    std::vector<double> canali_, affusti_;   // fino a quando e' impegnato ciascuno; vuoti: tutti liberi
};

}  // namespace m688
