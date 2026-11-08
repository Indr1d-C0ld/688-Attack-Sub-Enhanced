#pragma once
// Bacheca dei riconoscimenti (achievement, roadmap 15; nucleo/achievement, medaglioni del kit in grafica/medaglioni).
//
// Sette schede, una per categoria (tattico, stealth, tecnico, storico, carriera, editor, nascosti), con la griglia dei
// medaglioni 48x48: il bordo di bronzo, argento od oro del livello raggiunto, in grigio quelli non presi, «?» i nascosti
// non presi. Sotto, il riquadro del medaglione scelto: nome, condizione, livello, data e missione dello sblocco e, per
// quelli che si sommano sulle missioni, l'avanzamento verso la soglia dopo («12 SU 20»).
// Si guarda per un comandante (dall'albo, col tasto B) o per il giocatore, cioe' i riconoscimenti globali: G li alterna.
// Comandi: 1-7 o Tab le schede, frecce il medaglione, G comandante o globali, Esc indietro; il mouse sceglie.

#include <memory>
#include <optional>
#include <string>

#include "grafica/decorazioni.hpp"
#include "grafica/font.hpp"
#include "grafica/palette.hpp"
#include "grafica/tela.hpp"
#include "nucleo/achievement.hpp"
#include "nucleo/carriera.hpp"
#include "piattaforma/finestra.hpp"

namespace m688 {

class SchermataBacheca {
public:
    // `comandante`: il fascicolo di cui mostrare i riconoscimenti; senza, solo i globali
    SchermataBacheca(const CatalogoAchievement& cat, const TavolaDecorazioni& tav, std::optional<Fascicolo> comandante);
    bool comandi(const Comandi& c);   // vero: si esce
    int voce_scelta() const { return voce_; }
    void disegna(Tela& t, const Font& f) const;
    void palette(Palette& p) const;
    void scegli(int scheda, int voce);   // per le istantanee
    void globali(bool g) { globali_ = g || !comandante_; sporca_ = true; }

private:
    std::vector<const DefAchievement*> voci() const;   // della scheda
    const StatoAchievement& stato() const;
    Rettangolo casella(int i) const;
    void componi(Tela& t, const Font& f) const;

    const CatalogoAchievement& cat_;
    const TavolaDecorazioni& tav_;
    std::optional<Fascicolo> comandante_;
    FileAchievement file_;
    StatoAchievement vuoto_;
    int scheda_ = 0, voce_ = 0, prec_su_giu_ = 0, prec_ruota_ = 0;
    bool globali_ = false;
    mutable std::unique_ptr<Tela> cache_;
    mutable bool sporca_ = true;
};

}  // namespace m688
