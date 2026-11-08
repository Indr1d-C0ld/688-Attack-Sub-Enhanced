#pragma once
// Le righe trovate in automatico sul LOFAR nel modo moderno (blocco 3 del sonar, H; docs/23_sonar_blocco3.md §3): le marche
// delle righe confermate dal rilevatore (ALI/ALT, nucleo/rilevatore_righe) e la lista con frequenza, deriva, S/N ed eta'.
// Come nell'A-RCI: un aiuto, non la verita' (righe false, perse, fuse).
// - Le marche, sempre nel moderno: un triangolino sul bordo alto della cascata del LOFAR alla frequenza della riga, col
//   numero e la freccia della deriva; la scelta in bianco, le altre in verde chiaro (il ciano e' delle righe proprie, il
//   giallo dei cursori). Le righe fuori dalla finestra del LOFAR sono una freccetta sul bordo. Le etichette che si
//   toccano si impilano su piu' righe con la linea a puntini, come i segnalibri delle SDR; oltre ETICHETTE_RIGHE un «+n».
// - La lista, in sovrimpressione in alto a sinistra sul LOFAR, solo col cursore della stazione in modo RIGHE.
//
// Solo vista: il RilevatoreRighe lo tiene la stazione (uno per il fascio del LOFAR, svuotato al cambio di fascio); qui si
// leggono le confermate e si tiene la riga scelta. Tasti della stazione (principale): nel modo RIGHE del cursore su e giu'
// scelgono la riga, F aggancia il verniero alla riga scelta.

#include <optional>
#include <vector>

#include "grafica/font.hpp"
#include "grafica/tela.hpp"
#include "nucleo/rilevatore_righe.hpp"
#include "scena_prova.hpp"

namespace m688 {

struct StileRighe {
    double f0_hz = 0.0, f1_hz = 0.0;   // la finestra del LOFAR (SonarEnhanced::vista_hz(), con lo zoom)
};

class VistaRigheAutomatiche {
public:
    // --- la scelta ---
    void su_giu(const std::vector<RigaAutomatica>& righe, int verso);   // nell'ordine della lista (per frequenza)
    void scegli(int id) { scelta_ = id; }                               // dal clic su una marca
    int scelta() const { return scelta_; }                              // id, -1 nessuna
    // la riga scelta se e' ancora viva (per il verniero), altrimenti nullopt
    std::optional<RigaAutomatica> riga_scelta(const std::vector<RigaAutomatica>& righe) const;

    // --- disegno ---
    // le marche sul bordo alto del rettangolo della cascata del LOFAR. Con `aree`: «riga_auto_marca_<id>»
    void disegna_marche(Tela& t, const Font& f, Rettangolo lofar, const std::vector<RigaAutomatica>& righe,
                        const StileRighe& s, std::vector<AreaAttiva>* aree = nullptr) const;
    // la lista nel rettangolo: «R3  46,2 HZ  +0,08 HZ/MIN  12 DB  4:20», per frequenza, la scelta evidenziata, le righe di
    // meno di 30 s in grigio. Con `aree`: «riga_auto_<i>», i nell'ordine della lista
    void disegna_lista(Tela& t, const Font& f, Rettangolo r, const std::vector<RigaAutomatica>& righe, double ora,
                       std::vector<AreaAttiva>* aree = nullptr) const;

    // l'ordine della lista (per frequenza), la riga di testo e la x di una frequenza nel LOFAR
    static std::vector<RigaAutomatica> in_ordine(const std::vector<RigaAutomatica>& righe);
    static std::string riga(const RigaAutomatica& r, double ora);
    static int x_di(Rettangolo lofar, const StileRighe& s, double hz);

    static constexpr double DERIVA_FRECCIA_HZ_MIN = 0.05;   // oltre: la freccia della deriva sulla marca
    static constexpr double GIOVANE_S = 30.0;               // le righe piu' giovani in grigio nella lista
    static constexpr int RIGA_PX = 10;
    static constexpr int ETICHETTE_RIGHE = 4;       // le righe delle etichette impilate sopra la cascata
    static constexpr int ETICHETTA_PASSO_PX = 9;    // da una riga d'etichette all'altra
    static constexpr int ETICHETTA_STACCO_PX = 3;   // lo spazio minimo fra due etichette sulla stessa riga

private:
    int scelta_ = -1;
};

}  // namespace m688
