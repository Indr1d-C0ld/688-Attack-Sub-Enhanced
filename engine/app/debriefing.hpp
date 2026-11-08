#pragma once
// Debriefing dopo la missione (Fase 4a, docs/12_schermate_di_missione.md M5; proposta in docs/ricerca/15 §7.7, sul
// modello di F-117A e Falcon 3.0). Prima forma, sul registro degli eventi (M1):
// - carta con la traccia del battello che si disegna fino all'evento corrente, gli eventi gia' visti come segnaposto
//   numerati e quello corrente evidenziato; «Verita'» (V) sovrappone le tracce vere delle altre unita';
// - riquadro dell'evento: numero, ora, descrizione, interesse;
// - elenco compatto degli eventi (i momenti salienti; Tab li mostra tutti);
// - in fondo l'esito: categoria, completezza, obiettivi con lo stato.
// - la minicartolina dell'evento (M4 del kit): la scena dalla fotografia del mondo che la partita ha scattato
//   all'evento, o l'icona d'epoca per quello che non si vede. Le cartoline sono nella palette della vista, che la
//   carta in parte ridefinisce (96-143, 199-247, e 48-79 per il rilievo): i loro colori, al massimo 32 (i piu'
//   frequenti; gli altri sul piu' vicino), vanno negli indici 160-191 (l'oculare e il fuoco della vista, che qui non
//   servono): 16-47 sono della lamiera.
// Gruppo 3 della passata estetica (roadmap 33; docs/ricerca/25_bozze/indice.md §4): la lamiera di A coi documenti di
// B. Consolle «DEBRIEFING» con la testata classificata, la carta incassata nel pannello di sinistra, a destra il
// rapporto dattiloscritto su un foglio bianco (l'evento con la cartolina e l'elenco, poi l'esito col timbro, le voci a
// puntini coi valori a penna e le caselle degli obiettivi, poi il rapporto ACINT); il piede comune con quattro tasti
// (ESC CHIUDI, R RIPRENDI, P REPLAY, E ESITO; scelta dell'utente del 28/10/2026) e gli altri comandi nella riga d'aiuto.

#include <array>
#include <map>
#include <memory>
#include <optional>
#include <utility>
#include <vector>

#include "carta.hpp"
#include "figurina_evento.hpp"
#include "grafica/font.hpp"
#include "grafica/periscopio.hpp"
#include "grafica/palette.hpp"
#include "grafica/tela.hpp"
#include "missioni/missione.hpp"
#include "missioni/registro_missione.hpp"
#include "nucleo/confronto_classificazioni.hpp"
#include "nucleo/acint.hpp"
#include "nucleo/carta_giocatore.hpp"
#include "partita.hpp"
#include "piattaforma/finestra.hpp"

namespace m688 {

class SchermataDebriefing {
public:
    // il codice partita (app/codice_partita.hpp) o della rapida, in fondo alla pagina dell'esito
    void imposta_codice(std::string c) { codice_ = std::move(c); }
    // la cartella dei dati (data/motore): il font inciso della targa; senza, quello del chiamante
    void imposta_dati(const std::string& dati);
    // `periscopio` e `vista` (la palette del giorno): per le minicartoline; senza, il riquadro resta col nome
    SchermataDebriefing(const Partita& p, bool urss, StileCarta stile, const Periscopio* periscopio = nullptr,
                        const Palette* vista = nullptr);
    // vero quando il giocatore chiude il debriefing (Esc)
    bool comandi(const Comandi& c, double dt);
    void disegna(Tela& t, const Font& f) const;
    void palette(Palette& p) const;

    int corrente() const { return corrente_; }
    int eventi() const { return static_cast<int>(eventi_.size()); }
    Figurina figurina(const EventoMissione& e) const;   // che cosa mostra la minicartolina di un evento
    // La carta: la vista per le prove e per chi la guarda
    Rettangolo carta() const { return mappa_; }
    Vec2 centro_carta() const { return centro_; }
    double metri_per_pixel() const;
    // l'evento i; dopo gli eventi le pagine: l'esito, le classificazioni (sonar contro vista, se ce ne sono) e il
    // rapporto ACINT (se c'e')
    void vai(int i);
    // Il rapporto ACINT del registratore del sonar (S5d): una pagina dopo l'esito
    void imposta_acint(std::optional<RapportoAcint> r) { acint_ = std::move(r); }
    void mostra_tutti(bool si) {     // tutti gli eventi, non i soli momenti salienti (Tab)
        tutti_ = si;
        ricostruisci();
    }
    // «Riprendi da qui» (roadmap 16): R o il pulsante su un evento, poi Invio per confermare; comandi() torna vero e
    // riprendi_da() da' l'istante dell'evento. imposta_ripresa: questa partita e' gia' una ripresa (lo dice sulla carta)
    std::optional<double> riprendi_da() const { return riprendi_; }
    void riprendi(double t) { riprendi_ = t; }   // l'istante scelto nel replay completo
    // Il replay completo (P o il pulsante): comandi() torna vero; vero una volta sola, poi il debriefing riprende
    bool replay_chiesto() { return std::exchange(replay_, false); }
    bool statistiche_chieste() { return std::exchange(statistiche_, false); }   // S: le statistiche della missione
    void imposta_ripresa(double t) { ripresa_ = t; }
    void mostra_verita(bool si) {
        verita_ = si;
        ricostruisci();
    }

private:
    std::string codice_;
    std::optional<Font> targa_;
    // dall'ultimo disegno, per i clic e la carta: la carta nel vetro, l'area del testo sul foglio, l'elenco degli eventi
    mutable Rettangolo mappa_{17, 34, 398, 380}, foglio_{440, 31, 186, 377};
    mutable int lista_y_ = 0, lista_primo_ = 0, lista_visibili_ = 0;
    void disegna_carta_tracce(Tela& t, const Font& f) const;
    void disegna_riquadro(Tela& t, const Font& f) const;
    void disegna_esito(Tela& t, const Font& f) const;
    void disegna_acint(Tela& t, const Font& f) const;
    void disegna_classificazioni(Tela& t, const Font& f) const;
    int pagina_classificazioni() const { return confronto_ ? eventi() + 1 : -1; }
    int ultima_pagina() const { return eventi() + (confronto_ ? 1 : 0) + (acint_ ? 1 : 0); }
    void disegna_cartolina_evento(Tela& t, const Font& f, const EventoMissione& e, Rettangolo r) const;
    void ricostruisci();              // l'elenco con Tab e Verita', tenendo l'evento piu' vicino nel tempo
    void inquadra();                  // zoom e centro che contengono la traccia del battello e gli eventi
    double tempo_corrente() const;

    const Partita& p_;
    bool urss_ = false;
    bool tutti_ = false, verita_ = false;
    EsitoDettagliato esito_;
    std::map<int, Lato> lati_;        // il lato delle unita' dagli eventi (le altre in grigio)
    std::vector<const EventoMissione*> eventi_;
    int corrente_ = 0;                // eventi_.size(): la pagina dell'esito
    CartaGiocatore carta_;
    Vec2 centro_;
    // la carta si trascina fra questi limiti: tutte le tracce e tutti gli eventi, oltre c'e' il vuoto
    Vec2 limite_min_, limite_max_;
    std::optional<std::pair<int, int>> presa_;   // il punto del trascinamento in corso
    int corrente_visto_ = -1;         // l'evento per cui la carta si e' gia' ricentrata
    int lente_ = 1;                   // oltre la scala piu' grande della carta: 1, 2, 4 (S5 parte gia' da li')
    void piu_vicino();
    void piu_lontano();
    int prec_ruota_ = 0;
    const Periscopio* periscopio_ = nullptr;
    std::map<std::string, ClasseNave> classi_;   // la lunghezza, per comporre la scena dell'affondamento
    const Palette* vista_ = nullptr;
    // gli indici dei colori della cartolina: 160-191 (l'oculare e il fuoco, che nel debriefing non servono), perche' 16-47
    // e' la lamiera e 48-79 il rilievo della carta (grafica/palette.hpp)
    static constexpr int CARTOLINA = 160, N_CARTOLINA = 32;
    mutable std::array<std::array<std::uint8_t, 3>, N_CARTOLINA> colori_{};
    mutable int n_colori_ = 0;
    double tempo_ = 0.0;                       // le icone animate
    mutable std::unique_ptr<Tela> cartolina_;  // la scena dell'evento, rifatta solo quando l'evento cambia
    mutable int cartolina_di_ = -1;
    double fine_s_ = 0.0;
    bool conferma_ = false;                     // la domanda di «Riprendi da qui» e' aperta
    std::optional<double> riprendi_, ripresa_;
    bool replay_ = false, statistiche_ = false;
    std::optional<RapportoAcint> acint_;
    // il confronto fra la classificazione acustica e la visiva con la verita' (docs/25; nucleo/confronto_classificazioni),
    // dagli eventi di classificazione del registro; vuoto se nessuno ha classificato (le sole automatiche non contano)
    std::optional<ConfrontoClassificazioni> confronto_;
};

}  // namespace m688
