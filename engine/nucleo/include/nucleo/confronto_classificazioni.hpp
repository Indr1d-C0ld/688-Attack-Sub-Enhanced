#pragma once
// Il confronto nel debriefing fra la classificazione acustica e la visiva, con la verita' (libretto di riconoscimento,
// docs/25). Il modulo e' puro: le voci le riempie chi legge il registro della missione (gli eventi
// TipoEvento::classificazione, missioni/registro_missione.hpp).
// - Acustica: l'ultima classificazione del sonarista dell'equipaggio o del giocatore alla stazione sonar. Visiva:
//   l'ultima identificazione al periscopio. Quelle «per durata» del motore (senza sonarista) sono automatiche: non
//   entrano nei conti, si contano a parte.
// - Giusta: la classe esatta (o, a livello 1, la sola categoria giusta). Della stessa famiglia (victor2 per victor3,
//   los_angeles per los_angeles_flight3): mezza giusta.
// - Prima: chi ha dato per primo una classificazione giusta (o della famiglia).

#include <cstdint>
#include <string>
#include <vector>

namespace m688 {

struct VoceClassificazione {
    enum class Chi : std::uint8_t { visiva, equipaggio, giocatore_sonar, automatica };
    int contatto = -1;                // la sorgente vera
    double t = 0.0;
    Chi chi = Chi::equipaggio;
    int livello = 2;                  // 1: solo la categoria, 2: la classe
    std::string classe_creduta;       // chiave di classi.toml (vuota a livello 1)
    bool giusta = false;              // come la scrive il registro
    std::string classe_vera;
};

enum class EsitoClassificazione : std::uint8_t { assente, giusta, famiglia, solo_categoria, sbagliata };
const char* nome_esito(EsitoClassificazione e);

struct ConfrontoContatto {
    int contatto = -1;
    std::string classe_vera;
    struct Parte {
        EsitoClassificazione esito = EsitoClassificazione::assente;
        std::string classe;           // l'ultima creduta
        double t = -1.0;              // dell'ultima
        double t_prima_giusta = -1.0; // la prima giusta o della famiglia (< 0: mai)
    };
    Parte acustica, visiva;
    enum class Prima : std::uint8_t { nessuna, acustica, visiva, insieme } prima = Prima::nessuna;
};

struct ConfrontoClassificazioni {
    std::vector<ConfrontoContatto> contatti;   // per contatto, in ordine di id
    // conteggi: classificate (presenti), giuste (famiglia e solo categoria contano mezza), percentuali 0..100
    int acustiche = 0, visive = 0, automatiche = 0;
    double acustiche_giuste = 0.0, visive_giuste = 0.0;
    double percento_acustica() const { return acustiche ? 100.0 * acustiche_giuste / acustiche : 0.0; }
    double percento_visiva() const { return visive ? 100.0 * visive_giuste / visive : 0.0; }
    int prima_acustica = 0, prima_visiva = 0;
};

// la famiglia di una classe: la chiave fino alla prima cifra o a «_flight» (victor3 -> victor)
std::string famiglia_classe(const std::string& classe);

ConfrontoClassificazioni confronta_classificazioni(const std::vector<VoceClassificazione>& voci);

}  // namespace m688
