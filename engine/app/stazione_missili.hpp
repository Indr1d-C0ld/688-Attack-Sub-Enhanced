#pragma once
// Pagina di tiro dei missili della stazione Armi (F7) dell'Enhanced (docs/21_stazione_missili.md §3-§4, approvata):
// in alto la carta di tiro (la carta della stazione Navigazione con gli strati del tiro), in basso a sinistra la
// consolle di tiro (MK 117 sul Flight I, CCS MK 1 sul Flight II, BSY-1 sul 688I), a destra la Weapons Launch Console
// con le spie PWR, ALN, RDY, DOOR, SLIDE per tubo e il FIRE col coperchio.
//
// Solo letture della missione (Missione::tiro_missile, missili_in_volo, slide_libera): quello che cambia il tiro esce
// come Ordine (imposta_missile, prepara_missile) e l'interfaccia lo manda alla missione, cosi' il replay lo ripete.
// Dentro restano solo cose della vista: la riga scelta, il coperchio, la scala e il centro della carta.
//
// Tasti (docs/21 §3.5): frecce su e giu' la riga, sinistra e destra il valore; Maiusc+L il coperchio, L il lancio
// (StazioneArmi::lancia, solo a coperchio alzato); I e O la scala della carta, C il centro; Tab torna ai siluri.

#include <optional>
#include <string>
#include <vector>

#include "carta.hpp"
#include "grafica/font.hpp"
#include "grafica/tela.hpp"
#include "missioni/contatti_giocatore.hpp"
#include "missioni/missione.hpp"
#include "nucleo/carta_giocatore.hpp"
#include "nucleo/ordini.hpp"
#include "scena_prova.hpp"

namespace m688 {

// Quello che solo l'interfaccia sa, passato a ogni chiamata
struct ContestoMissili {
    enum class Consolle : std::uint8_t { mk117, ccs_mk1, bsy1 };   // Flight I, Flight II, 688I: la testata della consolle
    struct Tubo {
        int n = 0;                  // 1-4, 7
        std::string arma;
        bool porta = false;         // tubo allagato, equalizzato, porta aperta (lo stato 3, armato, della stazione Armi)
    };
    int tubo = 1;                   // il tubo scelto
    std::vector<Tubo> tubi;         // nell'ordine della stazione Armi; la WLC mostra quelli con un missile del catalogo
    int bersaglio = -1;             // il contatto designato (G)
    Consolle consolle = Consolle::mk117;
    bool modo_1989 = false;         // il modo della stazione scelto per partita (come ModoSonar): piu' avanti il fosforo
    std::string messaggio;          // l'ultimo messaggio di bordo
    double ora_s = 0.0, compressione = 1.0;   // l'intestazione, come le altre stazioni
};

class StazioneMissili {
public:
    // Le righe della consolle che si scelgono (le altre si leggono e si saltano). Antinave: FONTE, DISTANZA (solo col
    // periscopio), MODO, RICERCA (RBL) o SRCH e DSTR (BOL), PREPARAZIONE. TLAM: MISSIONE (col passo 3 del runtime),
    // PREPARAZIONE
    enum class Riga : std::uint8_t { fonte, distanza, modo, ricerca, srch, dstr, missione, preparazione };
    std::vector<Riga> righe(const Missione& m, int tubo) const;
    std::optional<Riga> riga(const Missione& m, int tubo) const;   // nullopt: il tubo non ha un missile del catalogo

    // --- comandi; gli ordini sono da mandare alla missione (vuoti se non cambia niente), t = Missione::tempo() ---
    void su_giu(const Missione& m, int tubo, int verso);                          // verso -1 su, +1 giu'
    std::vector<Ordine> cambia(const Missione& m, int tubo, int verso, double t);  // verso -1 sinistra, +1 destra
    void coperchio() { coperchio_ = !coperchio_; }                                 // Maiusc+L
    bool coperchio_alzato() const { return coperchio_; }
    // L: nullopt se si puo' passare a StazioneArmi::lancia, altrimenti il testo per la barra
    std::optional<std::string> rifiuto_lancio() const;
    void lanciato() { coperchio_ = false; }                                        // dopo il lancio si riabbassa
    void zoom(int verso);                                                          // +1 ingrandisce (I), -1 allarga (O)
    void centra();                                                                 // C: bersaglio, area di lancio, ...
    void cambio_tubo();                                                            // riga 0, coperchio giu', carta automatica

    // --- disegno ---
    // la carta di tiro nelle righe 0-357, con gli strati del §4 sopra la carta di F6
    void disegna_carta(Tela& t, const Font& f, const Missione& m, const CartaGiocatore& c,
                       const std::vector<SegnoContatto>& segni, const ContestoMissili& k) const;
    // il pannello nelle righe 360-479: intestazione, consolle, WLC, avvisi e messaggio. Le lampade e il FIRE sono del
    // tema delle consolle (indici 16-47 della palette, grafica/tema_stazioni). Con `aree`: una per riga che si sceglie
    // («missili_riga_<i>», i come in righe()) e il FIRE («missili_fuoco»)
    void disegna_pannello(Tela& t, const Font& f, const Missione& m, const ContestoMissili& k,
                          std::vector<AreaAttiva>* aree = nullptr) const;
    // la vista della carta di tiro: automatica (battello, area di ricerca e missili in volo) o alla scala scelta
    VistaCarta vista(const Missione& m, int tubo) const;

    // La soluzione del MATE come ordini per la fonte TMA: da mandare a ogni cambio dei quadranti, come coi siluri. Un
    // valore < 0 dimentica il campo
    static std::vector<Ordine> ordini_soluzione(double t, int tubo, double distanza_m, double rotta, double velocita_kn,
                                                double sigma_distanza_m, double sigma_trasversale_m);

    // larghezza della carta in km per le scale fisse (I e O)
    static constexpr double SCALE_KM[] = {2.0, 5.0, 10.0, 20.0, 50.0, 100.0, 200.0};
    static constexpr int SCALA_PARTENZA = 3;   // 20 km, la scala automatica senza bersaglio
    static constexpr int ALTEZZA_CARTA = 358;
    static constexpr double PASSO_DISTANZA_M = 500.0;   // la distanza dello stadimetro corretta a mano
    // le spie della WLC: la prima colonna, il passo fra le colonne, la lampada e le righe dei tubi (dall'inizio del pannello)
    static constexpr int WLC_X = 446, WLC_PASSO = 29, WLC_W = 24, WLC_H = 9, WLC_Y = 36, WLC_RIGA = 11;

private:
    enum class Centro : std::uint8_t { automatico, bersaglio, lancio };
    int riga_ = 0;
    bool coperchio_ = false;
    int scala_ = -1;                    // -1: automatica
    mutable int scala_auto_ = 3;        // l'ultima scala automatica, da cui partono I e O
    Centro centro_ = Centro::automatico;
};

}  // namespace m688
