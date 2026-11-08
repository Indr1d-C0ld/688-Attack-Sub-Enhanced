#pragma once

#include <cstddef>
#include <map>

#include "missioni/registro_missione.hpp"

namespace m688 {

// Quali eventi hanno una fotografia per le minicartoline del debriefing (Partita::avanza). Ogni proiettile a segno e' un
// artiglieria_colpo, e una fotografia costa circa 1,7 MB: nella S07 5581 colpi facevano 10 GB. I colpi di un tiratore
// dividono la sua prima fotografia, e sopra LIMITE passano solo la fine e gli affondamenti, che sono pochi per natura. Gli
// eventi di routine dell'IA non ne hanno: prendevano piu' di meta' del tetto (68 su 127 in S5, 07/11/2026) a danno di quelli
// veri, e la loro cartolina si fa dalle pose dell'evento (app/figurina_evento.hpp).
class SceltaFotografie {
public:
    static constexpr std::size_t TETTO = 120;    // oltre, solo gli eventi con interesse >= 60
    static constexpr std::size_t LIMITE = 300;   // oltre, solo la fine e gli affondamenti

    static bool routine(TipoEvento t) {
        switch (t) {
            case TipoEvento::classificazione_ia:
            case TipoEvento::comandante_ia:
            case TipoEvento::reazione_ia:
            case TipoEvento::tma_ia:
            case TipoEvento::scoperta_ia:
            case TipoEvento::ingaggio_ia:
            case TipoEvento::ritirata_ia:
            case TipoEvento::evasione_ia: return true;
            default: return false;
        }
    }

    // vero se l'evento va fotografato; `quante`: le fotografie fatte e quelle in attesa
    bool scatta(const EventoMissione& e, std::size_t quante) {
        if (routine(e.tipo)) return false;
        const bool colpo = e.tipo == TipoEvento::artiglieria_colpo;
        if (colpo)
            if (const auto it = prima_del_tiratore_.find(e.unita); it != prima_del_tiratore_.end()) {
                rimandi_[e.id] = it->second;
                return false;
            }
        if (quante >= LIMITE && e.tipo != TipoEvento::fine && e.tipo != TipoEvento::affondamento) return false;
        if (quante >= TETTO && e.interesse < 60) return false;
        if (colpo) prima_del_tiratore_[e.unita] = e.id;
        return true;
    }

    // l'evento di cui mostrare la fotografia
    int foto_di(int evento) const {
        const auto it = rimandi_.find(evento);
        return it != rimandi_.end() ? it->second : evento;
    }

private:
    std::map<int, int> prima_del_tiratore_;   // tiratore -> il suo primo colpo fotografato
    std::map<int, int> rimandi_;              // colpo -> il colpo che ha la fotografia
};

}  // namespace m688
