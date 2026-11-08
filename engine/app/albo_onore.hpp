#pragma once
// Albo d'onore (roadmap 15; dossier docs/ricerca/15 §7.10; nucleo/albo_onore.hpp): un registro d'epoca sulla scrivania
// con tre schede.
// - «Migliori missioni»: le 20 migliori, una riga ciascuna col punteggio, il comandante col grado e la marina, la
//   missione e le date (della carriera e del giorno in cui si e' giocata). Il codice in un riquadro (della rapida, 13
//   caratteri, o della partita, 18), da ricopiare per rigiocarla.
// - «Migliori carriere»: le 10 migliori carriere chiuse, coi punti, le missioni, la decorazione piu' alta, lo stato e
//   la data della chiusura. I caduti con la croce (US Navy, «ON ETERNAL PATROL») o il nastro nero (VMF, «POGIB PRI
//   ISPOLNENII»).
// - «Comandanti storici»: le schede fisse di data/motore/albo_onore_storico.toml (anno, battello, comandante, impresa e
//   decorazione, la fonte in piccolo), senza punteggi.
// Comandi: 1-3 o Tab le schede, frecce su e giu' scorrono, Esc torna al menu; il mouse sceglie le schede.

#include <filesystem>
#include <memory>
#include <string>
#include <vector>

#include "grafica/font.hpp"
#include "grafica/palette.hpp"
#include "grafica/tela.hpp"
#include "nucleo/albo_onore.hpp"
#include "nucleo/carriera.hpp"
#include "piattaforma/finestra.hpp"

namespace m688 {

class SchermataAlboOnore {
public:
    enum class Uscita { nessuna, menu };
    // `dati`: data/motore (gli storici); `albo`: il file dell'albo d'onore. Un file che manca lascia la scheda vuota
    SchermataAlboOnore(const RegoleCarriera& r, const std::string& dati,
                       std::filesystem::path albo = percorso_albo_onore());
    Uscita comandi(const Comandi& c);
    void disegna(Tela& t, const Font& f) const;
    void palette(Palette& p) const;
    void ricarica();                  // rilegge l'albo d'onore dal disco (dopo una missione)
    void scheda(int i);               // 0 missioni, 1 carriere, 2 storici
    int scheda() const { return scheda_; }

private:
    Rettangolo linguetta(int i) const;
    void componi(Tela& t, const Font& f) const;
    void missioni(Tela& t, const Font& f, Rettangolo r) const;
    void carriere(Tela& t, const Font& f, Rettangolo r) const;
    void storici(Tela& t, const Font& f, Rettangolo r) const;

    const RegoleCarriera& r_;
    std::string dati_;
    std::filesystem::path percorso_;
    AlboOnore albo_;
    std::vector<ImpresaStorica> storici_;
    int scheda_ = 0, scorri_ = 0, prec_su_giu_ = 0, prec_tab_ = 0;
    mutable std::unique_ptr<Tela> cache_;
    mutable int cache_scheda_ = -1, cache_scorri_ = -1;
};

}  // namespace m688
