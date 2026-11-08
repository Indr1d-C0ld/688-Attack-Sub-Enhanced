#pragma once
// Pagina dell'intercettatore della stazione sonar dell'Enhanced (blocco 2 del sonar, E; docs/22_sonar_blocco2.md §2,
// «contratto con la vista»): gli impulsi attivi altrui sentiti dal ricevitore d'intercettazione (WLR-9, Skat, Rubin).
// - in alto l'allarme siluro: lampada gialla SILURO IN ACQUA, rossa SILURO IN ACQUISIZIONE;
// - a sinistra la rosa dei rilevamenti delle tracce vive, prua in alto: il rilevamento fermo col livello che sale e'
//   l'indizio dell'acquisizione;
// - a destra la cascata frequenza-tempo: frequenza in log da 1 a 100 kHz con le bande del ricevitore, il tempo che
//   scorre verso sinistra per la finestra scelta (2, 5, 10, 30 minuti);
// - in basso le tracce per emettitore: rilevamento, frequenza, durata, cadenza, deriva, livello, CW o FM, Doppler,
//   classe; «--» per quello che il ricevitore non misura.
//
// Solo vista: legge il RicevitoreIntercettazione del giocatore (nucleo/intercettazione) e non manda ordini. I colori sono
// quelli della stazione sonar: la scala lunga di grafica/scala_cascata, e nel modo 1989 il fosforo, ricomposto a ogni
// fotogramma dall'eta' degli intercetti (il lampo dell'impulso nuovo che si spegne con la persistenza della stazione).
//
// Tasti (la principale, interfaccia): Ctrl+W apre e chiude la pagina; frecce su e giu' la traccia, sinistra e destra la
// finestra; Esc torna alle cascate.

#include <string>
#include <vector>

#include "grafica/font.hpp"
#include "grafica/scala_cascata.hpp"
#include "grafica/tela.hpp"
#include "nucleo/intercettazione.hpp"
#include "scena_prova.hpp"

namespace m688 {

// Lo stile della stazione sonar, che sceglie e applica la palette (SonarEnhanced::applica_palette), e la rotta del
// battello per la prua della rosa
struct StileIntercettazione {
    bool modo_1989 = false;                      // testo al fosforo, come il sonar
    bool fosforo = false;                        // SonarEnhanced::con_fosforo(): persistenza, bagliore e grana
    cascate::Scala scala = cascate::SCALA_LUNGA; // la scala della palette in uso
    cascate::Persistenza persistenza = cascate::Persistenza::normale;
    double rotta = 0.0;                          // del battello, gradi veri
};

class PaginaIntercettazione {
public:
    // --- comandi ---
    void su_giu(const RicevitoreIntercettazione& r, int verso);   // la traccia scelta, nella lista e sulla cascata
    void finestra(int verso);                                     // +1 piu' lunga, -1 piu' corta
    int traccia_scelta() const { return scelta_; }                // id della TracciaIntercetto, -1 nessuna
    int finestra_min() const { return FINESTRE_MIN[finestra_]; }

    // Nell'area della vista (640x360 di norma); r == nullptr: nessun intercettatore a bordo. Con `aree`: una per riga
    // della lista («intercetto_riga_<i>», i come in tracce()) e i due tasti della finestra («intercetto_finestra_meno»,
    // «intercetto_finestra_piu»)
    void disegna(Tela& t, const Font& f, Rettangolo area, const RicevitoreIntercettazione* r, double ora,
                 const StileIntercettazione& s, std::vector<AreaAttiva>* aree = nullptr) const;

    // La riga a fosforo di una traccia (dossier 28): «BRG 047  3,3 KHZ  1,5 S  PRI 25 S  +0,4°/MIN  +18 DB  FM  SCAFO
    // DOPPLER +», con «--» per quello che il ricevitore non misura
    static std::string riga(const TracciaIntercetto& tr);
    // Una traccia e' viva se l'ultimo intercetto e' entro tre cadenze (almeno un minuto)
    static bool viva(const TracciaIntercetto& tr, double ora);

    static constexpr int FINESTRE_MIN[] = {2, 5, 10, 30};
    // la geometria, dall'angolo dell'area (per le prove)
    static constexpr int ALTO_ALLARME = 20, ALTO_CENTRO = 200, LARGO_ROSA = 150, MARGINE_FREQ = 34, RIGA_LISTA = 10;
    static constexpr double F_MIN_HZ = 1000.0, F_MAX_HZ = 100000.0;
    // la cascata dentro l'area
    static Rettangolo cascata(Rettangolo area);
    // la y della frequenza e la x dell'istante nella cascata
    static double y_frequenza(Rettangolo c, double f_hz);
    double x_tempo(Rettangolo c, double t, double ora) const;

private:
    int scelta_ = -1;
    int finestra_ = 1;   // 5 minuti
};

}  // namespace m688
