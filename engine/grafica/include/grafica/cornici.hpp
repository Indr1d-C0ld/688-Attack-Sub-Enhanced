#pragma once
// Cornici, intestazioni e timbri delle schermate attorno alla missione (M4, docs/12): cornice da console nei grigi
// delle stazioni ridisegnate (144-158), cornici d'oro o rosse per la medaglia, carta del fascicolo, pulsanti dei menu
// e timbri «CLASSIFIED» / «SEGRETO» ruotati e consumati. Con `vista` (debriefing con le minicartoline, palette della
// vista) usano solo l'interfaccia (0-15, 80-95) e i grigi; senza, anche le rampe di cerimonia.hpp.

#include <cstdint>
#include <string>
#include <vector>
#include <string_view>

#include "grafica/cerimonia.hpp"
#include "grafica/font.hpp"
#include "grafica/tela.hpp"

namespace m688 {

enum class StileCornice {
    console,   // smusso grigio con le viti agli angoli, come le stazioni
    oro,       // cornice dorata del riquadro della medaglia (USN)
    rossa,     // cornice rossa e oro (VMF)
    legno,     // astuccio di legno con la fodera di velluto
};

// Cornice spessa `spessore` pixel attorno (dentro) a r; il campo interno non viene toccato
void cornice(Tela& t, Rettangolo r, StileCornice s, int spessore = 6, bool vista = false);
// Pannello in rilievo (o incavato) con il fondo pieno: menu, pulsanti, riquadri di testo
void pannello(Tela& t, Rettangolo r, bool incavato = false, bool vista = true);
// Pulsante con la scritta centrata; acceso = selezionato
void pulsante(Tela& t, const Font& f, Rettangolo r, std::string_view testo, bool acceso);
// Carta del fascicolo: fibra, bordi consumati, ombra della piegatura (rampe di cerimonia)
void carta(Tela& t, Rettangolo r, std::uint32_t seme);
// Intestazione: fascia con titolo e sottotitolo, stelle (USN) o stella rossa (VMF) agli angoli
void intestazione(Tela& t, const Font& f, Rettangolo r, std::string_view titolo, std::string_view sottotitolo,
                  Marina m, bool vista = true);
// Timbro d'inchiostro: testo in maiuscolo con doppio bordo, ruotato di `gradi`, consumato secondo il seme.
// «SEGRETO» sovietico: СЕКРЕТНО si scrive con le lettere latine uguali (CEKPETHO).
void timbro(Tela& t, const Font& f, int cx, int cy, std::string_view testo, double gradi, int scala, std::uint32_t seme,
            bool vista = false);

// --- il vetrino di Dio (docs/28 §6) ---
// Il pannello degli interventi: il riquadro a fosforo del quadro col titolo, la riga `scelta` (0 la prima, -1
// nessuna) nella fascia accesa col segno magenta a sinistra, il filo sopra la riga d'aiuto. Le righe sono alte
// f.righe()+3; restituisce l'area delle voci. La riga d'aiuto, fra r.y+r.h-14 e r.y+r.h-2, la scrive chi chiama
Rettangolo pannello_interventi(Tela& t, const Font& f, Rettangolo r, std::string_view titolo, int scelta);

// --- l'albo vuoto e il piede di cerimonia (gruppo 4 della roadmap 33, scelte dell'utente del 29/10/2026) ---
// Solo le rampe di cerimonia: albo, fascicolo, bacheca ed esito hanno i ritratti negli indici 16-47 del tema delle
// stazioni, dove la lamiera e il piede comune ridipingerebbero i volti.

// Il posto libero dell'albo: il filo tratteggiato nel legno chiaro e il numero del posto al centro
void posto_libero(Tela& t, Rettangolo r, const Font& f, int numero);

// Il piede delle schermate di cerimonia: la geometria del piede comune (grafica/lamiera: la riga d'aiuto, poi al piu'
// quattro tasti su tutta la larghezza, il predefinito col filo d'oro), ma di legno, coi tasti come targhette d'ottone.
// Un tasto spento e' il vano nel legno con la scritta appena incisa. Occupa da y al fondo della tela
struct TastoCerimonia {
    std::string testo;   // col tasto davanti: «INVIO  NUOVA CARRIERA»
    bool attivo = true;
};
void piede_cerimonia(Tela& t, const Font& f, int y, std::string_view aiuto, const std::vector<TastoCerimonia>& tasti,
                     int predefinito);
// Il tasto del piede di cerimonia sotto il clic (x, y), con `n` tasti e il piede a `y_piede`; -1 fuori dai tasti
int tasto_cerimonia(int x, int y, int y_piede, int n, int larghezza_tela = 640, int altezza_tela = 480);

}  // namespace m688
