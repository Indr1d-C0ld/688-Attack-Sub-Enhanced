#pragma once
// Medaglie e nastrini della US Navy e della VMF (dossier docs/ricerca/13 §3, M4 della Fase 4a). Una sola fonte per
// id, nomi, precedenza e colori dei nastrini: carriera.toml della M3 (RegoleCarriera, nucleo/carriera.hpp); la parte
// solo grafica (forma della medaglia, metallo, classe, ordini a vite) e' in data/motore/decorazioni_grafica.toml con
// gli stessi id. La tavola disegna il nastrino, la barretta in ordine di precedenza e la medaglia grande per il
// riquadro della cerimonia, nelle rampe di cerimonia.hpp; i colori dei nastrini vanno nei posti 96-109.

#include <string>
#include <string_view>
#include <vector>

#include "grafica/cerimonia.hpp"
#include "grafica/palette.hpp"
#include "grafica/tela.hpp"
#include "nucleo/carriera.hpp"

namespace m688 {

struct Striscia {
    int colore;          // indice di palette (96-109)
    double larghezza;    // trentaseiesimi del nastrino
};

enum class FormaMedaglia {
    nessuna,            // solo nastrino (citazioni di reparto, nastrini di servizio)
    distintivo,         // dolphins, Command at Sea, «Comandante di sottomarino»: sull'uniforme (ritratto.hpp)
    attestato,          // citazione senza nastro: un foglio incorniciato col sigillo
    croce_patente,      // Navy Cross
    stella_capovolta,   // Medal of Honor
    medaglione,         // Navy Distinguished Service Medal
    stella_con_corona,  // Silver Star
    stella,             // Bronze Star, «Per distinzione»
    legione,            // Legion of Merit
    ottagono,           // Navy and Marine Corps Medal
    cuore,              // Purple Heart
    stella_raggiata,    // Meritorious Service Medal
    esagono,            // Navy Commendation Medal
    quadrato,           // Navy Achievement Medal
    tondo,              // medaglie di servizio e medaglie sovietiche
    stella_oro,         // Eroe dell'Unione Sovietica
    ordine_lenin,
    bandiera_rossa,
    stella_rossa,
    servizio_patria,    // «Per il servizio alla Patria nelle Forze armate dell'URSS»
    stella_otto,        // «Per il coraggio personale»
    ordine_usakov,
};

struct DisegnoDecorazione {
    std::string id, nome, tipo;       // dalla carriera (tipo: personale, reparto, servizio, distintivo)
    Marina marina = Marina::usn;
    int precedenza = 99;
    std::vector<Striscia> nastro;     // da sinistra a destra (dall'alto in basso se orizzontale)
    bool orizzontale = false;
    bool stelle = false;              // motivo «stelle_bianche» (Medal of Honor)
    char lettera = 0;                 // dispositivo fisso sul nastrino ('E' del Navy E Ribbon)
    FormaMedaglia forma = FormaMedaglia::nessuna;
    int metallo = cerimonia::BRONZO;  // rampa del metallo
    int classe = 0;
    bool a_vite = false;              // ordini sovietici portati a vite, senza nastro
};

class TavolaDecorazioni {
public:
    // std::runtime_error se il file grafico manca o ha forme e metalli sconosciuti
    static TavolaDecorazioni carica(const RegoleCarriera& regole, const std::string& percorso_grafica);

    void imposta_palette(Palette& p) const;   // colori dei nastrini nei posti 96-109
    const DisegnoDecorazione* decorazione(std::string_view id) const;
    const std::vector<DisegnoDecorazione>& tutte() const { return d_; }   // in ordine di precedenza per marina
    // id presenti in uno solo dei due file (le prove li vogliono vuoti)
    const std::vector<std::string>& solo_in_carriera() const { return solo_carriera_; }
    const std::vector<std::string>& solo_in_grafica() const { return solo_grafica_; }

    // Nastrino w x h pixel in (x, y), con i dispositivi: stelline dal secondo conferimento (d'argento ogni cinque),
    // «V» di combattimento, lettera fissa
    void disegna_nastrino(Tela& t, int x, int y, int w, int h, const VoceBarretta& v) const;
    // Barretta in ordine di precedenza (la prima voce e' la piu' alta): file di `per_fila` (Carriera::per_fila), la
    // fila incompleta in alto e centrata. (cx, y_basso): centro del bordo inferiore. Restituisce l'ingombro.
    Rettangolo disegna_barretta(Tela& t, int cx, int y_basso, const std::vector<VoceBarretta>& v, int per_fila, int w, int h) const;
    // Medaglia grande col suo nastro (o il solo nastrino ingrandito, o l'attestato), centrata in r
    void disegna_medaglia(Tela& t, Rettangolo r, std::string_view id) const;

private:
    std::vector<DisegnoDecorazione> d_;
    std::vector<std::array<std::uint8_t, 3>> colori_;   // posto 96 + i
    std::vector<std::string> solo_carriera_, solo_grafica_;
};

Rettangolo misura_barretta(std::size_t n, int per_fila, int w, int h);

}  // namespace m688
