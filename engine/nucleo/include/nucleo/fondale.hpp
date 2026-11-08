#pragma once
// Fondale di un teatro (data/motore/teatri/<teatro>.fdl, esportato da tools/pipeline/esporta_motore.py dalle
// heightmap di data/teatri/): griglia regolare in proiezione azimutale equidistante centrata sul teatro, la
// stessa origine delle posizioni delle missioni (x verso est, y verso nord, metri).
//
// Lo stesso formato porta le carte d'epoca (data/motore/teatri/<teatro>_carta_<nato|urss>.fdl, da
// tools/pipeline/carta_epoca.py, docs/08 §5): la carta nautica di una parte, derivata dal fondale vero e degradata,
// con la stessa griglia, le campagne di rilievo per cella e le annotazioni. La simulazione usa solo il fondale vero.

#include <cstdint>
#include <string>
#include <vector>

#include "nucleo/geometria.hpp"

namespace m688 {

enum class Parte { nato, urss };
const char* nome_parte(Parte p);   // "nato", "urss" (come nei nomi dei file)

struct Geografiche {
    double lat = 0.0, lon = 0.0;   // gradi, WGS84 (lon negative a ovest)
};

// Campagna di rilievo di una carta d'epoca. Lo spostamento e' quello vero della campagna: per le prove e il
// debug, la stazione non lo mostra.
struct Campagna {
    int anno = 0;
    std::string ente;
    double spaziatura_m = 0.0;          // fra le linee di rilievo, sulle soglie e in piattaforma
    double spaziatura_bacini_m = 0.0;   // nei bacini profondi
    double spostamento_m = 0.0;
};

// Nota di una carta d'epoca su un rilievo che la carta non mostra (docs/08 §5 punto 5)
struct Annotazione {
    enum class Tipo { ed, pa, acqua_scolorita, profondita_minore };   // esistenza dubbia, posizione approssimata, ...
    Tipo tipo = Tipo::ed;
    Vec2 pos;                    // come la riporta la carta (non dove sta il rilievo vero)
    double profondita_m = 0.0;   // riportata (0 per l'acqua scolorita)
    int anno = 0;
};

// data/motore/teatri/<teatro>_carta_<nato|urss>.fdl sotto la cartella dei dati
std::string percorso_carta(const std::string& dati, const std::string& teatro, Parte p);

class Fondale {
public:
    // Lancia std::runtime_error se il file manca o non e' valido.
    static Fondale carica(const std::string& percorso);
    // Griglia data (riga 0 = nord): quota z in metri = valore * scala + offset, positiva verso l'alto.
    Fondale(int larghezza, int altezza, double cella_m, double scala, double offset, std::vector<std::uint16_t> valori);

    int larghezza() const { return w_; }
    int altezza() const { return h_; }
    double cella_m() const { return cella_; }
    bool dentro(Vec2 p) const;
    // Quota del terreno in metri (positiva sopra il livello del mare), interpolata fra i centri delle celle.
    // Fuori dalla griglia non ci sono dati: restituisce -infinito (acqua senza fondo).
    double quota(Vec2 p) const;
    // Profondita' dell'acqua in metri (negativa sulla terra), +infinito fuori dalla griglia.
    double profondita(Vec2 p) const { return -quota(p); }

    // Celle della griglia (colonna verso est, riga verso sud, entrambe da 0)
    bool cella(Vec2 p, int& col, int& riga) const;   // falso fuori dalla griglia
    Vec2 centro_cella(int col, int riga) const;
    double profondita_cella(int col, int riga) const { return -z(col, riga); }

    // Georeferenziazione (blocco GEO1): centro della proiezione azimutale equidistante sull'ellissoide WGS84, dove
    // le distanze e gli azimut geodetici dal centro sono quelli del piano. Problemi diretto e inverso di Vincenty
    // (errore sotto il millimetro alle distanze dei teatri). Senza centro geografiche e proiettata lanciano
    // std::logic_error.
    bool georeferenziato() const { return geo_; }
    Geografiche centro() const { return centro_; }
    void imposta_centro(Geografiche c);
    Geografiche geografiche(Vec2 p) const;
    Vec2 proiettata(Geografiche g) const;

    // Carta d'epoca (blocchi RIL1 e ANN1): tutto vuoto sul fondale vero
    bool carta_epoca() const { return !campagna_cella_.empty(); }
    int edizione() const { return edizione_; }       // anno dell'edizione della carta, per il cartiglio
    // Campagna di rilievo della cella: indice in campagne(), -1 se la zona non e' rilevata, sulla terra o fuori.
    int campagna(Vec2 p) const;
    int campagna_cella(int col, int riga) const;
    bool rilevato(Vec2 p) const { return campagna(p) >= 0; }
    const std::vector<Campagna>& campagne() const { return campagne_; }
    const std::vector<Annotazione>& annotazioni() const { return annotazioni_; }
    // Per le prove: per_cella ha larghezza*altezza valori (0 = non rilevato, k = campagne[k-1])
    void imposta_carta(int edizione, std::vector<Campagna> campagne, std::vector<std::uint8_t> per_cella,
                       std::vector<Annotazione> annotazioni = {});

private:
    double z(int col, int riga) const { return valori_[static_cast<std::size_t>(riga) * w_ + col] * scala_ + offset_; }

    int w_ = 0, h_ = 0;
    double cella_ = 200.0, scala_ = 0.25, offset_ = -11000.0;
    std::vector<std::uint16_t> valori_;
    bool geo_ = false;
    Geografiche centro_;
    int edizione_ = 0;
    std::vector<Campagna> campagne_;
    std::vector<std::uint8_t> campagna_cella_;
    std::vector<Annotazione> annotazioni_;
};

}  // namespace m688
