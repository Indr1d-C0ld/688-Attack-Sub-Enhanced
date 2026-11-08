#pragma once
// Sala briefing prima della missione (Fase 4a, docs/12_schermate_di_missione.md M5; proposta in
// docs/ricerca/15_schermate_simulatori.md §7.4, sul modello di F-19 e F-117A). Tutta la tela:
// - testata col timbro (SECRET o SEKRETNO), il titolo e il gruppo data-ora;
// - carta del teatro nella grafica della stazione Navigazione, con la partenza «P», pan e zoom;
// - menu dei livelli a destra (1-8, 9 tutto o niente): ordini e obiettivi, zone dello spazio d'acqua, intelligence,
//   basi e radar costieri, portate ASW, SOSUS, fondale, eventi speciali; quelli senza dati della missione in grigio;
// - riquadro delle informazioni (battello, teatro, finestre radio, regole d'ingaggio);
// - pannello degli ordini a telescrivente (Tab): il messaggio della serie 0 e le sezioni fisse (regole d'ingaggio,
//   comunicazioni);
// - piede: Tab ordini, F7 sala siluri, Esc rifiuta, Invio in mare.
// Gruppo 3 della passata estetica (roadmap 33; docs/ricerca/25_bozze/indice.md §4): la lamiera di A coi documenti di B.
// Consolle «SALA BRIEFING» con la testata classificata; a sinistra la carta nel pannello e, sopra (Tab li chiude, come
// ha deciso l'utente), gli ordini sul modulo continuo della telescrivente, a pagine (frecce su e giu'); a destra i
// livelli come spie e la scheda della missione nei vetri; il piede comune coi tasti anche al clic.
// E' solo interfaccia: non cambia la missione.

#include <array>
#include <vector>
#include <functional>
#include <optional>
#include <string>

#include "carta.hpp"
#include "grafica/font.hpp"
#include "grafica/palette.hpp"
#include "grafica/tela.hpp"
#include "missioni/missione.hpp"
#include "nucleo/carta_giocatore.hpp"
#include "piattaforma/finestra.hpp"

namespace m688 {

class SchermataBriefing {
public:
    enum class Uscita { nessuna, in_mare, armi, rifiuta };
    static constexpr int LIVELLI = 8;

    // `urss`: il giocatore e' sovietico (timbro, carta della sua parte); `anni_89`: le stazioni nel modo d'epoca
    // `cartella_dati`: per gli aerei ASW previsti (asw_aereo.toml); vuota, senza il livello delle portate ASW
    SchermataBriefing(const Missione& m, bool urss, bool anni_89, StileCarta stile, const std::string& cartella_dati = "");

    // il codice partita (app/codice_partita.hpp) o della rapida, in fondo alla colonna di destra
    void imposta_codice(std::string c) { codice_ = std::move(c); }
    Uscita comandi(const Comandi& c, double dt);
    void disegna(Tela& t, const Font& f, const Missione& m) const;
    void palette(Palette& p) const;

    bool livello(int i) const { return livelli_[static_cast<std::size_t>(i)]; }   // 0-7
    bool disponibile(const Missione& m, int i) const;   // la missione ha dati per quel livello
    bool ordini_aperti() const { return ordini_; }
    int pagina_ordini() const { return pagina_; }
    int pagine_ordini() const { return pagine_; }   // aggiornate dal disegno
    void completa() { tempo_ = 1e6; }   // la telescrivente ha finito (istantanee)
    void mostra_ordini(bool si) { ordini_ = si; }

private:
    std::string codice_;
    std::optional<Font> targa_;     // incisione688_7 (senza la cartella dei dati: il font del chiamante)
    int pagina_ = 0;                // la pagina degli ordini
    int prec_su_giu_ = 0;
    mutable int pagine_ = 1;        // quante ne ha (dall'ultimo disegno)
    mutable Rettangolo mappa_{15, 37, 402, 382}, livelli_r_{432, 28, 202, 122};   // dall'ultimo disegno, per i clic
    void applica_livelli();
    void disegna_ordini(Tela& t, const Font& f, const Missione& m) const;

    CartaGiocatore carta_;
    std::array<bool, LIVELLI> livelli_{};
    bool ordini_ = true;            // all'inizio si leggono gli ordini, come sulla telescrivente
    bool urss_ = false;
    double tempo_ = 0.0;            // secondi veri dall'apertura (la telescrivente)
    Vec2 centro_;
    std::optional<std::pair<int, int>> presa_;   // il punto del trascinamento in corso
    std::vector<Missione::AereoAswPrevisto> asw_nemici_;   // le portate ASW nemiche (livello 5)
    void disegna_asw(Tela& t, const Font& f, const Missione& m, const VistaCarta& v) const;
    void scrivi_meteo(const Missione& m, const std::function<void(const std::string&, std::uint8_t)>& scrivi) const;
};

}  // namespace m688
