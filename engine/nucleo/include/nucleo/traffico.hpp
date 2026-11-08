#pragma once
// Traffico civile come contatti veri (blocco 1 del sonar, docs/20_sonar_blocco1.md §3): navi mercantili, traghetti e
// pescherecci che percorrono le rotte del teatro attorno al giocatore. Solo nell'Enhanced.
//
// Il modulo decide, il runtime applica (contratto del 28/10 col runtime):
// - Traffico::avanza popola il disco attorno al centro alla prima chiamata, poi muove le navi e da' le entrate e le
//   uscite (oltre il disco piu' il margine);
// - il runtime crea un'unita' per ogni entrata (id suoi, neutrale, categoria nave, classe = NaveTraffico::classe),
//   la toglie a ogni uscita (non e' un affondamento) e copia ogni passo pos, rotta e velocita' dalle navi;
// - la firma acustica dell'individuo e' Traffico::firma(indice), da passare in UnitaAcustica::firma;
// - le navi sono solo sorgenti: niente sensori, niente IA.
// Tutto dipende dal seme della partita e dal tempo di gioco: le partite si ripetono identiche.
//
// Dati: data/motore/teatri/<teatro>.traffico.toml (tools/pipeline/traffico.py), sul piano del teatro (metri, come il
// fondale e i toponimi). Fonti: docs/ricerca/26_navi_civili_traffico.md.

#include <cstdint>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "nucleo/acustica.hpp"
#include "nucleo/geometria.hpp"
#include "nucleo/navi_civili.hpp"

namespace m688 {

// --- dati del teatro ---
struct CorsiaTraffico {
    std::string id, nome;
    std::vector<Vec2> punti;                 // spezzata, dal primo all'ultimo punto (il senso diretto)
    double larghezza_m = 2000.0;             // le navi si spargono di qua e di la' dall'asse
    bool doppio_senso = true;                // falso: solo nel senso diretto (schemi di separazione)
    double navi_giorno = 0.0;                // per senso
    std::vector<std::pair<std::string, double>> miscela;   // tipo civile -> peso
    std::vector<double> partenze_h;          // traghetti di linea: ore locali delle partenze da ogni capo (vuota: a caso)
    std::vector<int> mesi;                   // vuoti: tutto l'anno
};
struct ZonaPesca {
    std::string id, nome;
    std::vector<Vec2> punti;                 // punti d'acqua buona dentro la zona (validati sul fondale dal pipeline):
                                             // i pescherecci rimorchiano dall'uno a un vicino
    double navi = 0.0;                       // pescherecci al lavoro nella zona
    double velocita_kn = 3.5;                // al lavoro (strascico 2,5-4,5; palangari e reti piu' lenti)
    std::vector<std::pair<std::string, double>> miscela;
    std::vector<int> mesi;
};
struct RotteTraffico {
    std::string teatro;
    std::vector<CorsiaTraffico> corsie;
    std::vector<ZonaPesca> zone;
};
RotteTraffico carica_traffico(const std::string& percorso);   // std::runtime_error se manca o e' malformato
// <cartella_dati>/teatri/<teatro>.traffico.toml; un teatro senza file non ha traffico
std::string file_traffico(const std::string& cartella_dati, const std::string& teatro);

// --- le navi ---
enum class AttivitaNave : std::uint8_t { in_rotta, pesca, trasferimento, fuga };
struct NaveTraffico {
    int indice = -1;                         // stabile nella partita, mai riusato: il runtime lo lega al suo id
    std::string tipo;                        // tipo civile ([civili.*] di acustica.toml)
    std::string classe;                      // classe di classi.toml (modello, dimensioni, ROE)
    double lunghezza_m = 0.0, pescaggio_m = 0.0;
    Vec2 pos;
    double rotta = 0.0, velocita_kn = 0.0;
    AttivitaNave attivita = AttivitaNave::in_rotta;
    // reazione a un'esplosione (la accende il runtime con Traffico::fuggi): accosta via da `fuga_da`, accelera e dopo
    // `fuga_fino_s` torna nella sua corsia o zona
    Vec2 fuga_da;
    double fuga_fino_s = -1.0;
};
struct Ricambio {
    std::vector<int> entrate;                // indici delle navi nuove (gia' in navi())
    std::vector<int> uscite;                 // indici delle navi da togliere (gia' fuori da navi())
};

class Traffico {
public:
    // raggio: il disco attorno al centro (il giocatore); margine: oltre, la nave esce. tetto: navi al piu', le piu'
    // vicine (le altre restano nel rumore del traffico lontano). Rotte e catalogo devono vivere quanto il Traffico.
    // Una nave che esce (oltre il margine o scavalcata da una piu' vicina) puo' rientrare piu' tardi col suo indice
    Traffico(const RotteTraffico& rotte, const CatalogoAcustico& catalogo, int mese, std::uint64_t seme_partita,
             double raggio_m = 100000.0, double margine_m = 110000.0, int tetto = 40);
    // Un passo: alla prima chiamata popola il disco allo stato stazionario; poi muove le navi di dt_s (il tempo del
    // mondo, gia' scalato dal moto classico se serve), toglie quelle oltre il margine e fa entrare le nuove dal bordo.
    // ora_locale per i traghetti e per la pesca di notte
    Ricambio avanza(double tempo_s, double dt_s, Vec2 centro, double ora_locale);
    const std::vector<NaveTraffico>& navi() const { return navi_; }
    const NaveTraffico* nave(int indice) const;
    // la firma dell'individuo: vive quanto il Traffico, anche dopo l'uscita della nave (per il replay e i log)
    const FirmaRumore* firma(int indice) const;
    void fuggi(int indice, Vec2 da, double fino_s);
    // La nave e' stata colpita, affondata o incagliata nella missione: esce da navi() senza passare dalle uscite e non
    // si muove piu' (il resto lo fa la missione come per ogni nave). La firma resta; il posto sotto il tetto si libera
    void perduta(int indice);

private:
    // Il traffico e' una funzione del tempo: ogni corsia e' un nastro (arrivi di Poisson a ogni capo, da semi fissi per
    // fasce di 10 minuti), ogni peschereccio rimorchia da un punto della sua zona a un vicino. Una nave e' una chiave
    // (corsia, senso, fascia, numero o zona, numero); il resto e' il suo stato: indice, firma, fuga
    struct Candidata {
        std::uint64_t chiave = 0;
        std::string tipo;
        std::uint64_t seme = 0;
        Vec2 pos;
        double rotta = 0.0, velocita_kn = 0.0;
        AttivitaNave attivita = AttivitaNave::in_rotta;
    };
    struct Rimorchio {   // un peschereccio: il rimorchio in corso, fatto avanzare dall'inizio della partita
        int da = 0, a = 0, numero = 0;
        double inizio_s = 0.0, fine_s = 0.0;
    };
    struct Stato {   // una nave che e' stata in navi()
        int indice = -1;
        IndividuoCivile individuo;
        Vec2 fuga_da, pos_fuga;   // in fuga: si allontana da fuga_da fino a fuga_fino_s
        double fuga_fino_s = -1.0;
        Vec2 scarto;              // dopo la fuga: la distanza dalla sua rotta, che si richiude in 10 minuti
        double scarto_da_s = -1.0;
    };
    struct Geometria {   // di una corsia: lunghezze cumulate e la velocita' piu' bassa dei suoi tipi (m/s)
        std::vector<double> cumulata;
        double v_min = 1.0;
    };
    std::vector<Candidata> candidate(double tempo_s, Vec2 centro, double ora_locale);
    void da_corsie(std::vector<Candidata>& out, double tempo_s, Vec2 centro, double ora_locale) const;
    void da_zone(std::vector<Candidata>& out, double tempo_s, Vec2 centro);
    Stato& stato(const Candidata& c);
    NaveTraffico nave_da(const Candidata& c, Stato& st, double tempo_s, double dt_s);

    const RotteTraffico& rotte_;
    const CatalogoAcustico& catalogo_;
    int mese_;
    std::uint64_t seme_;
    double raggio_m_, margine_m_;
    int tetto_;
    int prossimo_ = 0;
    std::vector<Geometria> geometrie_;
    std::vector<NaveTraffico> navi_;
    std::map<std::uint64_t, Stato> stati_;           // per chiave
    std::map<int, std::uint64_t> chiavi_;            // indice -> chiave
    std::map<int, std::vector<Rimorchio>> rimorchi_; // zona -> pescherecci
    std::vector<std::uint64_t> perdute_;
};

}  // namespace m688
