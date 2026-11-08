#pragma once
// Le misure degli achievement (nucleo/achievement.hpp) di una missione chiusa: dall'esito dettagliato e dagli eventi del
// registro M1, dal contesto della carriera e, se c'e', dal fascicolo del comandante. Legge soltanto: la missione non
// cambia. I nomi delle misure sono quelli di misure_achievement_note() (e di tools/pipeline/achievement.py).

#include "missioni/missione.hpp"
#include "nucleo/achievement.hpp"
#include "nucleo/carriera.hpp"

namespace m688 {

struct OpzioniMisure {
    int armi_a_bordo = -1;                      // armi rimaste a bordo alla fine (stazione Armi); -1 non noto
    const Fascicolo* fascicolo = nullptr;       // il fascicolo dopo Carriera::chiudi_missione (achievement di carriera)
    const Fascicolo* fascicolo_prima = nullptr; // e prima della missione: di carriera conta solo quello che cambia (il
                                                // grado, le qualifiche e le decorazioni d'ingresso della carriera breve
                                                // non si guadagnano in gioco)
    const RegoleCarriera* regole = nullptr;     // per gradi e tipi delle decorazioni
};

Misure misure_achievement(const Missione& m, const ContestoMissione& c, const OpzioniMisure& o = {});

}  // namespace m688
