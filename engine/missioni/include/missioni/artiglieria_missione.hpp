#pragma once
// Stato interno dell'artiglieria navale dell'Enhanced nella missione (docs/16_artiglieria.md, G2): lo usano soltanto
// missione.cpp e missione_artiglieria.cpp. L'interfaccia pubblica e' in missione.hpp (artiglieria_enhanced,
// impatti_artiglieria, eventi_artiglieria, sagoma_esposta).

#include <map>
#include <vector>

#include "missioni/missione.hpp"
#include "nucleo/artiglieria.hpp"
#include "nucleo/carriera.hpp"
#include "nucleo/casuale.hpp"

namespace m688 {

struct Missione::StatoArtiglieriaMissione {
    TabellaArtiglieria dati;
    Situazione situazione = Situazione::pace;
    Casuale caso{0xA27151E7u};
    struct Pezzo {
        const DatiPezzo* dati = nullptr;
        int pronti = 0;                      // colpi prima della pausa di ricarica
        double prossimo = 0.0, pausa_fino = -1.0;
        int sparati = 0;                     // colpi sul bersaglio: i primi senza la correzione sul punto di caduta (22 §3.6)
        int consumati = 0;                   // proietti tirati in tutta la partita: finito il deposito il pezzo tace
    };
    struct Nave {
        std::vector<Pezzo> pezzi;            // vuoto: disarmata (contro la superficie)
        bool combattimento = false;          // posto di combattimento; altrimenti condizione III
        double allarme_da = -1.0;            // il primo contatto in guerra: ai pezzi dopo la reazione di condizione III
        bool acquisito = false;              // il bersaglio e' stato designato (reazione partita)
        double ultimo_visto = -1e18;
        Vec2 punto;                          // dove mira: la stima del bersaglio esposto
        bool punto_finale = true;            // l'ultimo colpo sul punto, dopo il cessate il fuoco
        double avvertimento_da = -1e18;      // crisi: inizio della salva d'avvertimento in corso
        int avvertimenti = 0;
        double prossimo_illuminante = 0.0;
        int bersaglio = -1;                  // il giocatore, un sottomarino dell'IA o una nave ostile con fra_navi
        double prossima_bomba = 0.0;         // acque territoriali: la prossima coppia di bombe d'avvertimento
    };
    std::map<int, Nave> navi;
    int spari = 0;                            // le uscite delle canne: ImpattoArtiglieria::sparo
    std::vector<ImpattoArtiglieria> in_volo;  // sparati, non ancora arrivati
    std::vector<EventoArtiglieria> eventi_in_volo;
    std::size_t lanci_visti = 0;              // eventi del registro gia' guardati per la provocazione
    bool provocati = false;                   // il giocatore ha lanciato: la crisi diventa guerra
    bool fra_navi = false;                    // chiave `artiglieria = { fra_navi = true }`: anche nave contro nave
    double ultimo_illuminante = -1e18;
    Vec2 punto_illuminante;
    int prossimo_transitorio = 0;             // id 995000 + 10 (n % 500), seme da 990000 + 10 n
    int acque_dentro = -1;                    // le acque territoriali straniere dove sta il giocatore
    double acque_da = 0.0;
};

}  // namespace m688
