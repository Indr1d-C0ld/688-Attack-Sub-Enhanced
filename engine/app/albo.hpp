#pragma once
// Albo dei comandanti e fascicolo personale (Fase 4a, docs/ricerca/15 §7.1 e 13 §5.4 C; nucleo/albo.hpp).
//
// L'albo: fino a ALBO_MAX carriere come cartellini sulla scrivania, con la foto ufficiale, il nome, il grado, la
// marina, le missioni, il punteggio di carriera e lo stato. Chi e' in carica va in mare con la prossima missione della
// sua parte (688 la US Navy, Alfa la VMF). I caduti («ON ETERNAL PATROL», «POGIB PRI ISPOLNENII»), i rimossi e i
// congedati si consultano ma non si caricano. Il cartellino vuoto apre una carriera nuova: nome, marina, carriera breve
// (dal comando) o completa, indulgenza. Cancellare chiede conferma due volte.
// Comandi: frecce, Invio in carica (o carriera nuova), F il fascicolo, B la bacheca dei riconoscimenti, O l'albo
// d'onore, Backspace elimina, Esc al menu; il mouse sceglie.
//
// Il fascicolo («OFFICER SERVICE RECORD», «LICHNOE DELO»): copertina timbrata con la foto e l'anagrafica, stato di
// servizio (incarichi e qualifiche), note caratteristiche o attestazioni (una riga per missione col giudizio e la
// graduatoria fra i pari grado), decorazioni (la barretta e ognuna con data e motivazione), commissioni e
// provvedimenti. Frecce sinistra e destra le pagine, su e giu' il testo, Esc indietro.

#include <filesystem>
#include <utility>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "grafica/decorazioni.hpp"
#include "grafica/font.hpp"
#include "grafica/palette.hpp"
#include "grafica/tela.hpp"
#include "nucleo/albo.hpp"
#include "nucleo/carriera.hpp"
#include "piattaforma/finestra.hpp"

namespace m688 {

class SchermataFascicolo {
public:
    SchermataFascicolo(const RegoleCarriera& r, const TavolaDecorazioni& tav, Fascicolo f);
    bool comandi(const Comandi& c);   // vero: si esce
    void disegna(Tela& t, const Font& f) const;
    void palette(Palette& p) const;
    int pagine() const { return static_cast<int>(pagine_.size()); }
    void vai(int i);
    // Nelle note la missione scelta (frecce su e giu'); P chiede la sua traccia del replay, se c'e': il percorso una
    // volta sola, poi niente
    std::optional<std::filesystem::path> replay_chiesto() { return std::exchange(replay_, std::nullopt); }

private:
    enum class Pagina { copertina, servizio, note, decorazioni, provvedimenti };
    void componi(Tela& t, const Font& f) const;
    void copertina(Tela& t, const Font& f) const;
    void servizio(Tela& t, const Font& f) const;
    void note(Tela& t, const Font& f) const;
    void decorazioni(Tela& t, const Font& f) const;
    void provvedimenti(Tela& t, const Font& f) const;
    int foglio(Tela& t, const Font& f, const std::string& titolo) const;   // la pagina vuota; restituisce la y del testo

    const RegoleCarriera& r_;
    const TavolaDecorazioni& tav_;
    Fascicolo f_;
    std::vector<Pagina> pagine_;
    int pagina_ = 0, scorri_ = 0, prec_ruota_ = 0, prec_su_giu_ = 0;
    int scelta_ = 0;                                   // nelle note: la missione, 0 la piu' recente
    std::optional<std::filesystem::path> replay_;
    mutable std::unique_ptr<Tela> cache_;
    mutable int cache_di_ = -1, cache_scorri_ = -1, cache_scelta_ = -1;
};

class SchermataAlbo {
public:
    enum class Uscita { nessuna, menu, fascicolo, bacheca, onore, statistiche };   // B riconoscimenti, O albo d'onore, S statistiche
    SchermataAlbo(const RegoleCarriera& r, const TavolaDecorazioni& tav);
    Uscita comandi(const Comandi& c);
    void disegna(Tela& t, const Font& f) const;
    void palette(Palette& p) const;
    bool vuole_testo() const { return modulo_ && modulo_->campo == 0; }   // il nome della carriera nuova
    // La marina dell'ultimo comandante aperto o messo in carica qui: il menu va al suo battello
    std::optional<Marina> marina_scelta() const { return marina_scelta_; }
    // Il modulo della carriera nuova, per le prove: aperto, il campo col cursore, la marina; le caselle si cliccano
    bool modulo_aperto() const { return modulo_.has_value(); }
    int campo_modulo() const { return modulo_ ? modulo_->campo : -1; }
    Marina marina_modulo() const { return modulo_ ? modulo_->marina : Marina::usn; }
    static Rettangolo casella_modulo(int campo, int k);   // k-esima scelta del campo 1-3; campo 0: la riga del nome
    static Rettangolo tasto_modulo(bool apri);            // i due tasti in fondo: apri il fascicolo o annulla
    // Il fascicolo da aprire dopo Uscita::fascicolo
    const Fascicolo* scelto() const;
    void scegli(int i) { cursore_ = i; }
    void ricarica();   // rilegge i fascicoli dal disco (dopo una missione)

private:
    struct Modulo {   // carriera nuova
        int campo = 0;                 // 0 nome, 1 marina, 2 carriera, 3 indulgenza
        std::string nome;
        Marina marina = Marina::usn;
        bool breve = true;
        Indulgenza indulgenza = Indulgenza::realistica;
    };
    Rettangolo cartellino(int i) const;
    void componi(Tela& t, const Font& f) const;
    void disegna_modulo(Tela& t, const Font& f) const;
    void crea();

    const RegoleCarriera& r_;
    const TavolaDecorazioni& tav_;
    std::vector<VoceAlbo> voci_;
    int cursore_ = 0, conferma_ = 0, prec_ruota_ = 0, prec_su_giu_ = 0;
    std::optional<Modulo> modulo_;
    std::optional<Marina> marina_scelta_;
    std::string avviso_;
    mutable std::unique_ptr<Tela> cache_;   // la scrivania coi cartellini: le foto costano, si rifanno coi dati
    mutable bool sporca_ = true;
};

}  // namespace m688
