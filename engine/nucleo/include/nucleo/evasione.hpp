#pragma once
// Evasione dei sottomarini dell'IA dalla caccia aerea (Enhanced, docs/13_asw_aereo.md, taratura A6; dottrina e numeri
// dal dossier docs/ricerca/17_asw_aereo.md §6.7, §8.3 e §9): il battello che sente la caccia dosa la velocita', scende
// sotto lo strato, cambia rotta e, in acque basse, si posa sul fondo. Gli stimoli sono quelli che un battello sente
// davvero di un aereo (§8.3). Le manovre (STIMA sulla dottrina):
// - ping lontano: via dal sonar a velocita' dosata (il «flaming datum» del §9: si allarga il cerchio senza farsi
//   sentire), sotto lo strato;
// - tonfo: silenzio, sotto lo strato, via; sorvolo: giu' oltre il MAD e di traverso;
// - ping vicino: la tattica classica contro l'elicottero, sotto lo strato e via dal datum su una rotta sola a velocita'
//   dosata, senza manovrare vicino al sonar (contro l'eco attiva il silenzio non serve: la taratura A6.1 da' piu'
//   scoperte a 8 kn che a 16);
// - siluro, lontano o vicino: virata stretta per mettere la poppa al siluro (il fianco ha la forza di bersaglio piu'
//   alta) e tutta forza per uscire dal cono della testa; giu', lontano dal siluro che scende dalla superficie: oltre il
//   suo pavimento se lo scafo ci arriva (l'Alfa contro il Mk 46), altrimenti alla quota massima. Le esche le lancia la missione, nel punto della virata
//   (il «knuckle»: la falsa scia in se' la testa non la modella, la fa l'esca).
// Gli stimoli:
// - il tonfo di una boa, del trasduttore o di un siluro in acqua;
// - il ping di un sonar a immersione o di una boa attiva, lontano (lo si intercetta 7-12 volte prima dell'eco) o vicino;
// - un siluro aviolanciato che cerca, lontano o vicino;
// - l'aereo in sorvolo nella finestra di Snell (i passaggi bassi e quelli del MAD).
//
// Il modulo e' puro e deterministico: la missione decide che cosa il battello sente (portate e probabilita' del §8.3)
// e passa lo stimolo con il Casuale& del suo flusso; qui si sceglie la manovra e la si tiene per un tempo. Tutti i
// numeri sono STIMA salvo dove il dossier li da'. La missione applica il comando col pilota dell'IA (le secche e il
// franco restano suoi) e, finita l'evasione, il battello riprende la sua navigazione.

#include <cstdint>
#include <optional>
#include <string>

#include "nucleo/casuale.hpp"
#include "nucleo/geometria.hpp"

namespace m688 {

enum class StimoloEvasione : std::uint8_t {
    ping_lontano,   // sonar attivo intercettato lontano: si sa di essere cercati
    tonfo,          // boa, trasduttore o siluro che entra in acqua vicino (2,8 km, §8.3)
    sorvolo,        // aereo sulla verticale: passaggio basso o MAD
    ping_vicino,    // sonar attivo vicino: contatto o sospetto
    siluro,         // un siluro aviolanciato in acqua o in ricerca, lontano: lo si distanzia
    siluro_vicino   // la testa attiva a meno di 1,5 km: lenti, a poppa e attraverso lo strato
};
const char* nome_stimolo(StimoloEvasione s);

// Il battello come lo vede la manovra
struct BattelloEvasione {
    Vec2 pos;
    double rotta_deg = 0.0, velocita_kn = 0.0, quota_m = 100.0;
    double quota_max_m = 400.0;
    double fondale_m = 1e9;          // sotto il battello
    double strato_m = 0.0;           // SLD del profilo (0: nessuno strato)
    double velocita_max_kn = 25.0;
    bool diesel = false;             // SSK: piu' lento in silenzio, si posa sul fondo piu' volentieri
    double pavimento_siluro_m = 0.0; // quota massima del siluro sentito (0: ignota)
};

struct ComandoEvasione {
    double rotta_deg = 0.0, velocita_kn = 0.0, quota_m = 0.0;
    bool sul_fondo = false;          // fermo sul fondo (la quota e' quella del fondo)
    StimoloEvasione stimolo = StimoloEvasione::ping_lontano;
};

// Le classi diesel-elettriche fra quelle del gioco
bool battello_diesel(const std::string& classe);

class Evasione {
public:
    // Uno stimolo sentito da `rilevamento_deg` (verso la sorgente). Uno piu' forte di quello in corso, o uno qualunque
    // a manovra finita, sceglie una manovra nuova; uno uguale la prolunga; uno piu' debole non cambia nulla.
    void stimolo(StimoloEvasione s, double rilevamento_deg, double t, const BattelloEvasione& b, Casuale& caso);
    // La manovra in corso (nullopt: nessuna, il battello naviga come vuole lo script)
    std::optional<ComandoEvasione> comando(double t, const BattelloEvasione& b) const;
    bool attiva(double t) const { return t < fine_; }
    int manovre() const { return manovre_; }   // quante ne ha scelte (per la taratura)

private:
    StimoloEvasione stimolo_ = StimoloEvasione::ping_lontano;
    double fine_ = -1e18, rotta_ = 0.0, velocita_ = 0.0, quota_ = 0.0;
    bool fondo_ = false, rotta_libera_ = false;
    int manovre_ = 0;
};

}  // namespace m688
