#pragma once
// La prima schermata al primo avvio: classico o Enhanced (prova a mano dell'utente del 06/11/2026). Dopo la prima volta la
// scelta sta nelle preferenze ([avvio] modo) e si cambia dal menu col tasto O. Da riga di comando vince --regola

#include <optional>

#include "grafica/font.hpp"
#include "grafica/palette.hpp"
#include "grafica/tela.hpp"
#include "piattaforma/finestra.hpp"

namespace m688 {

class SchermataModo {
public:
    // la scelta fatta, vuota finche' si sceglie: vero classico, falso Enhanced. `esci` vero con Esc
    std::optional<bool> comandi(const Comandi& c, bool& esci);
    void disegna(Tela& t, const Font& f) const;
    void palette(Palette& p) const;
    bool cursore_classico() const { return classico_; }

private:
    bool classico_ = false;   // col cursore all'inizio sull'Enhanced, il gioco che il menu presenta per primo
    int prec_ruota_ = 0;
};

}  // namespace m688
