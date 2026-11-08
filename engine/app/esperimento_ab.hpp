#pragma once
// L'esperimento A/B del vetrino di Dio dal menu (docs/28 §5, D2; nota condivisa trailer1-d2-forma-m688v.md): giocarlo
// con strumenti/serie_ab (le due serie, il confronto, il rapporto) mentre la finestra mostra quante partite sono fatte,
// e mostrarne il confronto (app/schermata_confronto) coi replay affiancati di una coppia (app/replay_affiancati).
// La finestra la da' chi chiama (app/main.cpp), con le sue funzioni per gli eventi e per presentare la tela col brano.

#include <filesystem>
#include <functional>
#include <optional>
#include <string>

#include "grafica/font.hpp"
#include "grafica/palette.hpp"
#include "grafica/tela.hpp"
#include "piattaforma/finestra.hpp"

namespace m688 {

struct AnelloFinestra {
    std::function<Comandi()> eventi;
    std::function<void(const Tela&, const Palette&, const std::string& brano)> presenta;
    std::function<double()> secondi;
};

// Gioca l'esperimento `m688v` con serie_ab (accanto a `motore`, l'eseguibile) in `uscita`; il confronto.toml, o vuoto se
// non e' riuscito. `chiusa` vero se la finestra si chiude intanto (le partite vanno avanti fino alla fine)
std::optional<std::filesystem::path> gioca_esperimento(AnelloFinestra& w, Tela& tela, const Font& f, const Palette& pal,
                                                       const std::filesystem::path& motore, const std::filesystem::path& m688v,
                                                       const std::filesystem::path& uscita, bool& chiusa);

// La schermata del confronto; un clic su una coppia apre i due replay affiancati, Esc torna al confronto. `base`: la
// palette del gioco (i colori fondamentali), sotto quelle delle schermate. Falso se la finestra si chiude
bool mostra_confronto(AnelloFinestra& w, Tela& tela, const Font& f, const Palette& base, const std::string& dati,
                      const std::filesystem::path& confronto);

}  // namespace m688
