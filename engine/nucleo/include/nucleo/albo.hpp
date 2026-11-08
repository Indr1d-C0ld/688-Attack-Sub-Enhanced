#pragma once
// Albo dei comandanti (Fase 4a, docs/ricerca/15 §7.1 e 13 §5.4 C): le carriere salvate (i fascicoli di
// nucleo/fascicolo.hpp), al piu' ALBO_MAX come gli armadietti di F-15 III, e il comandante in carica per marina, quello
// che va in mare con la prossima missione della sua parte. Il comandante in carica sta in albo.toml accanto alla
// cartella dei fascicoli. I caduti, i rimossi e i congedati restano nell'albo da consultare, ma non si caricano (da
// F-19); un fascicolo che non si legge piu' si salta.

#include <filesystem>
#include <optional>
#include <string>
#include <vector>

#include "nucleo/fascicolo.hpp"

namespace m688 {

inline constexpr int ALBO_MAX = 8;

struct VoceAlbo {
    Fascicolo f;
    double punti = 0.0;           // punteggio di carriera: la somma dei punti delle missioni
    bool in_carica = false;       // il comandante in carica della sua marina
};

std::filesystem::path percorso_albo();                    // <cartella dei fascicoli>/../albo.toml
// Le carriere dell'albo, in ordine di id (le prime ALBO_MAX); in_carica segnato da albo.toml
std::vector<VoceAlbo> leggi_albo();
// Il comandante in carica della marina, se c'e' ed e' ancora caricabile
std::optional<std::string> comandante_in_carica(Marina m);
void imposta_in_carica(Marina m, const std::string& id);   // crea la cartella se serve
// Si puo' mandare in mare: in servizio
bool caricabile(const Fascicolo& f);
// Un id libero per un nome nuovo: lo slug del nome, poi _2, _3... se c'e' gia'
std::string id_libero(const std::string& id);
// Toglie il fascicolo dall'albo (cancella il file) e, se era in carica, libera il posto (e i suoi codici)
void elimina_dall_albo(const std::string& id);

// Missioni rapide: ogni codice conta per la carriera una volta sola per comandante (roadmap 15). Il registro sta in
// albo.toml, [codici] <id> = [...]
bool codice_giocato(const std::string& id, const std::string& codice);
void registra_codice(const std::string& id, const std::string& codice);

}  // namespace m688
