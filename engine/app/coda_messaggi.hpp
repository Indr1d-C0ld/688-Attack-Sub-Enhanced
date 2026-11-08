#pragma once
// Coda dei messaggi di bordo dell'Enhanced (roadmap punto 21, docs/11 §4 «Barra dei messaggi»): nessun messaggio va
// perso sotto un altro. Ogni messaggio nuovo entra in coda e resta in barra per un tempo che cresce con la lunghezza
// (2,5-7 s reali); gli avvisi critici (quelli col punto esclamativo) passano davanti e interrompono un messaggio
// normale gia' letto per almeno un secondo. Se la coda si allunga troppo, i normali piu' vecchi si saltano: restano nel
// registro (Partita::storico_messaggi). Il classico non la usa: lui mostra l'ultimo messaggio, come l'originale.

#include <cstddef>
#include <deque>
#include <string>
#include <utility>
#include <vector>

namespace m688 {

class CodaMessaggi {
public:
    // Prende i messaggi nuovi dallo storico (tempo di missione, testo; in ordine di tempo, che cresce soltanto) e
    // decide quello da mostrare all'ora reale `ora` (s)
    void aggiorna(const std::vector<std::pair<double, std::string>>& storico, double ora);
    const std::string& corrente() const { return corrente_; }
    static double durata(const std::string& testo);   // s reali in barra
    static bool critico(const std::string& testo);

private:
    static constexpr std::size_t MASSIMO = 6;   // in coda; oltre si saltano i normali piu' vecchi
    std::size_t visti_ = 0;
    std::deque<std::string> coda_;
    std::string corrente_;
    double inizio_ = 0.0, fino_ = 0.0;
    bool corrente_critico_ = false;
};

}  // namespace m688
