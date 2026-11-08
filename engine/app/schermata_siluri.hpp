#pragma once
// Sala siluri prima della missione (Fase 4a, docs/12_schermate_di_missione.md M5; proposta in docs/ricerca/15 §7.5 e
// 14 §5.3), sulla SalaSiluri della M2 (nucleo/carico.hpp): a sinistra il deposito con le quantita' e i divieti, al
// centro la pianta del locale (tubi, rastrelliera, pozzi verticali, esche) con i totali, in basso la scheda dell'arma.
// I carichi standard per tipo di missione (da A-10), uno «consigliato»; «Svuota» e la dotazione dell'originale (da
// Falcon 3.0); la conferma prima di partire con gli avvisi (da F-117A). Solo nell'Enhanced.
//
// Comandi: frecce su e giu' l'arma del deposito, destra la mette (in rastrelliera, nei pozzi o fra le esche secondo
// l'arma), sinistra la toglie; 1-6 la mette nel tubo (o la toglie se c'e' gia'); Tab il carico standard successivo, S
// lo applica, X svuota, D la dotazione dell'originale; Esc torna al briefing, Invio in mare. C, o il clic sul pulsante del
// carico, apre l'elenco dei carichi per compito: per ognuno che cosa salirebbe davvero a bordo. Il carico consigliato dalla
// missione vi e' solo evidenziato: non si applica da solo, e dopo la scelta il carico resta modificabile a mano.

#include <map>
#include <optional>
#include <string>
#include <vector>

#include "grafica/font.hpp"
#include "grafica/palette.hpp"
#include "grafica/sagome_armi.hpp"
#include "grafica/tela.hpp"
#include "nucleo/carico.hpp"
#include "nucleo/compiti_carico.hpp"
#include "piattaforma/finestra.hpp"

namespace m688 {

class SchermataSiluri {
public:
    enum class Uscita { nessuna, briefing, in_mare };
    // Una riga dell'elenco: il carico cosi' come salirebbe a bordo con questo deposito e quest'anno
    struct VoceElenco {
        std::string tipo;
        CaricoPartenza carico;
        bool adattato = false;     // il deposito o l'anno non consentono quello standard
        double scarto = 0.0;       // dallo standard della missione, quello che pesa in carriera (0..1)
        std::string mancano;       // che cosa non sale rispetto allo standard, in una riga
        bool doppione = false;     // con questo deposito sale lo stesso di una riga sopra
    };
    // `battello`: nome per la testata; `consigliato`: il tipo della missione (vuoto: il predefinito del locale);
    // `compiti`: i nomi per l'elenco (senza, le chiavi)
    SchermataSiluri(SalaSiluri& sala, std::string battello, int anno, std::string consigliato, std::map<std::string, CompitoCarico> compiti = {});
    Uscita comandi(const Comandi& c, double dt);
    void disegna(Tela& t, const Font& f) const;
    // le rampe delle sagome nei posti 96-143 (dopo aver preso la palette del giorno); senza, la tavolozza fissa
    void palette(Palette& p) const;

    int scelta() const { return riga_; }
    void scegli(const std::string& arma);   // per le istantanee
    const std::vector<std::string>& tipi() const { return tipi_; }
    const std::vector<VoceElenco>& voci() const { return voci_; }
    const std::string& consigliato() const { return consigliato_; }
    bool elenco_aperto() const { return elenco_; }
    int tipo_scelto() const { return tipo_; }
    void apri_elenco() { elenco_ = true; }   // anche per le istantanee
    int y_riga(int i) const;                  // dove sta la riga i dell'elenco (la x e' quella della pianta)

private:
    void disegna_deposito(Tela& t, const Font& f) const;
    void disegna_pianta(Tela& t, const Font& f) const;
    void disegna_scheda(Tela& t, const Font& f) const;
    void disegna_elenco(Tela& t, const Font& f) const;
    void comandi_elenco(const Comandi& c);
    void applica_tipo();
    const std::string& arma_scelta() const;
    void esito(const EsitoCarico& e);
    void metti();
    void togli();
    void tubo(int n);
    const TavolozzaSagome& tav() const;

    SalaSiluri& sala_;
    std::string battello_, consigliato_;
    int anno_ = 1989;
    std::vector<ArmaDisponibile> armi_;      // aggiornate a ogni cambio
    std::vector<std::string> tipi_;          // i carichi standard del locale, nell'ordine dei compiti
    std::map<std::string, CompitoCarico> compiti_;
    std::vector<VoceElenco> voci_;           // parallele a tipi_
    bool elenco_ = false;                    // l'elenco dei carichi, sopra la pianta
    int riga_ = 0, tipo_ = 0;
    int prec_su_giu_ = 0, prec_ruota_ = 0;
    std::string avviso_;                     // l'ultimo rifiuto, per qualche secondo
    double avviso_s_ = 0.0;
    mutable std::optional<TavolozzaSagome> tav_;   // dalla palette()
    bool conferma_ = false;                  // Invio una volta con gli avvisi: chiede di confermare
};

}  // namespace m688
