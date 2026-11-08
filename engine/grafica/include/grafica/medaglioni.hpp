#pragma once
// Medaglioni degli achievement (roadmap 15; elenco e id nella proposta del disassemblatore, nucleo/achievement):
// una medaglia tonda in rilievo come le decorazioni di grafica/decorazioni (luce da sinistra in alto, retinatura), col
// bordo del livello (bronzo liscio, argento perlinato, oro perlinato con la stella in alto), lo smalto della categoria
// (tattico rosso, stealth blu notte, tecnico verde, storico azzurro, carriera bianco, editor viola, nascosti nero) e il
// simbolo dell'achievement in metallo. Non preso: tutto in grigio spento; nascosto e non preso: il «?».
//
// L'id dell'icona e' quello dell'achievement (campo `icona` di achievement.toml). Due tavolozze:
// - cerimonia: le rampe di cerimonia.hpp (cerimonia::imposta_palette), per la bacheca e l'albo d'onore;
// - vista: soli indici che la luce della vista non tocca (EGA 0-15, interfaccia 80-95, 160-195, 254), per la striscia
//   del debriefing che tiene la palette della vista.
// Si disegna in qualunque quadrato: 48x48 per la bacheca, 24x24 per la striscia (sotto i 32 pixel niente perline).

#include <string>
#include <string_view>
#include <vector>

#include "grafica/tela.hpp"

namespace m688 {

enum class LivelloMedaglione { bronzo, argento, oro };
enum class TavolozzaMedaglione { cerimonia, vista };

// Il medaglione centrato nel quadrato piu' grande che sta in r. Icona sconosciuta: un'ancora sullo smalto di
// `categoria` (DefAchievement::categoria; vuota o sconosciuta: nero)
void disegna_medaglione(Tela& t, Rettangolo r, std::string_view icona, LivelloMedaglione livello, bool preso,
                        bool nascosto = false, TavolozzaMedaglione tavolozza = TavolozzaMedaglione::cerimonia,
                        std::string_view categoria = {});
// Dal livello del nucleo (nucleo/achievement: 0 nessuno, 1 bronzo, 2 argento, 3 oro) al livello del medaglione e
// al «preso» (0: non preso, disegnato come oro spento)
inline LivelloMedaglione livello_medaglione(int livello) {
    return livello >= 3 ? LivelloMedaglione::oro : livello == 2 ? LivelloMedaglione::argento : LivelloMedaglione::bronzo;
}

// Le icone note, nell'ordine della bacheca (categorie, poi i nascosti), e la categoria di un'icona ("" se sconosciuta)
const std::vector<std::string>& icone_medaglioni();
std::string categoria_medaglione(std::string_view icona);

}  // namespace m688
