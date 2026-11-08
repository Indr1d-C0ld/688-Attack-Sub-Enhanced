#pragma once
// Stazione Radio (F2, come la RADIO ROOM dell'originale; docs/11_comandi_e_stazioni.md §11, pezzo S4).
// - Classico: periscopio e antenna su o giu', rilettura dei messaggi di bordo gia' passati (PREC. e SUCC.).
// - Enhanced (Missione::radio(), la sala radio della sessione sul runtime): antenne e ricezione per banda, broadcast
//   in onda e in programma, buchi nella serie, telescrivente del messaggio in arrivo, archivio dei ricevuti con gli
//   ordini di missione, trasmissioni (tipo e invio) e scarico via satellite. L'esito della trasmissione (fix della
//   radiogoniometria nemica) non si mostra: il comandante non lo sa.
// - R1: regole d'ingaggio in vigore secondo i messaggi ricevuti, con la serie che le ha stabilite; boe SLOT rimaste,
//   il ritardo scelto ([ e ], 5-30 minuti, i limiti predefiniti delle missioni) e la trasmissione in attesa.
// Disegna nell'area della vista; i comandi passano per gli ordini radio_* (l'interfaccia li manda al registro).

#include <algorithm>
#include <string>
#include <utility>
#include <vector>

#include "grafica/font.hpp"
#include "grafica/lamiera.hpp"
#include "grafica/tela.hpp"
#include "missioni/missione.hpp"
#include "nucleo/radio.hpp"

namespace m688 {

// Una riga dell'archivio dei messaggi di bordo del classico: istante (s di missione) e testo gia' composto
using RigaStorico = std::pair<double, std::string>;

class StazioneRadio {
public:
    // Scelta nell'archivio: 0 e' il piu' recente; PREC. va indietro nel tempo, SUCC. in avanti
    void precedente() { ++scelto_; }
    void successivo() { if (scelto_ > 0) --scelto_; }
    // Enhanced: solo gli ordini di missione (serie 0 e i messaggi che li cambiano) o tutto
    void commuta_ordini() { solo_ordini_ = !solo_ordini_; scelto_ = 0; }
    bool solo_ordini() const { return solo_ordini_; }
    // Enhanced: tipo della prossima trasmissione, a giro
    void tipo_successivo() { tipo_ = static_cast<TipoTrasmissione>((static_cast<int>(tipo_) + 1) % 6); }
    TipoTrasmissione tipo() const { return tipo_; }
    // Enhanced: ritardo della prossima boa SLOT, a passi di 5 minuti
    void cambia_ritardo(int verso) { ritardo_s_ = std::clamp(ritardo_s_ + 300.0 * verso, 300.0, 1800.0); }
    double ritardo_s() const { return ritardo_s_; }

    void disegna(Tela& t, const Font& f, const Missione& m) const;   // Enhanced
    // Enhanced sul 688 con le consolle (gruppo 5, bozza approvata il 29/10/2026): gli stessi dati sulla lamiera, la
    // fascia di stato con antenne e ROE in spie, il messaggio sulla telescrivente, a destra in onda, in programma e
    // l'elenco dei ricevuti. `incisa`: il font delle scritte incise (incisione688_7; sull'Alfa lo stampino); `sc`: le
    // scritte sulla lamiera, chiare sul 688 e scure sul beige dell'Alfa (lamiera::scritte_per); `sovietica`: la sala
    // dell'Alfa, RADIORUBKA
    void disegna_sala(Tela& t, const Font& f, const Font& incisa, const Missione& m,
                      lamiera::Scritte sc = lamiera::Scritte::chiare, bool sovietica = false) const;
    void disegna_classico(Tela& t, const Font& f, const Missione& m, const std::vector<RigaStorico>& storico) const;

    // La serie del messaggio scelto nell'archivio (per la verifica dell'autenticazione), -1 senza messaggi
    int serie_scelta(const SalaRadio& r) const {
        const auto l = archivio(r);
        return l.empty() ? -1 : l[static_cast<std::size_t>(std::clamp(scelto_, 0, static_cast<int>(l.size()) - 1))]->messaggio.serie;
    }
    // I messaggi dell'archivio, il piu' recente per primo (prove)
    std::vector<const Ricevuto*> archivio(const SalaRadio& r) const;
    int scelto() const { return scelto_; }

    // Il testo spezzato in righe che stanno in `larghezza` pixel alla scala 1 (a capo anche sui '\n')
    static std::vector<std::string> a_capo(const Font& f, const std::string& testo, int larghezza);

private:
    mutable int scelto_ = 0;   // ritagliato all'archivio quando si disegna
    bool solo_ordini_ = false;
    TipoTrasmissione tipo_ = TipoTrasmissione::contatto;
    double ritardo_s_ = 600.0;
};

}  // namespace m688
