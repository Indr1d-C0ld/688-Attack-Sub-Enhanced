#pragma once
// Temi delle stazioni della Fase 3 (dossier docs/ricerca/19_consolle_688_alfa.md §2.6, §3.5 e §5.2): i colori delle
// consolle del 688 Flight I e dell'Alfa (Pr. 705) negli indici 16-47 della palette, che le stazioni dell'Enhanced non
// usano (verificato sulle istantanee di stazioni, periscopio, sala siluri, quadro danni e Museo; li usano solo ritratti,
// cerimonie e cartoline, in schermate a se').
//
// - smalto delle consolle: grigio degli apparati del 688 (dal nero dei bordi al grigio chiaro FS 26307) o crema e
//   avorio dell'Alfa (dal blu-grigio del pavimento al marrone dei cuscini all'avorio);
// - due toni d'accento: i cristalli grigio-azzurri dei CRT del 688, la bachelite nera dell'Alfa;
// - lampade rosse, ambra e verdi, accese e spente (le EGA del dossier: di notte non si spengono);
// - targhette: bianco su nero (688, MIL-M-18012 tipo I) o bianco su rosso (Alfa), col mezzo tono dell'incisione;
// - fosforo dei CRT: verde (688, STIMA) o verde giallino (Alfa, scelta dell'utente del 25/10/2026); ambra a richiesta.
//
// La luce della sala tinge smalto e targhette (bianca; rossa per la notte e il periscopio notturno del 688; blu nella
// sala sonar); lampade e fosfori sono emissivi e non cambiano. Va applicato dopo Palette::illuminata della vista: 16-47
// sta dentro INDICI_VISTA, che di notte lo scurirebbe. Solo nell'Enhanced, dietro il flag delle stazioni nuove.

#include <cstdint>

#include "grafica/palette.hpp"

namespace m688 {

namespace colori_tema {
inline constexpr int SMALTO = 16, N_SMALTO = 12;    // 16-27: dal piu' scuro al piu' chiaro
inline constexpr int ACCENTO = 28, N_ACCENTO = 2;   // 28-29: cristalli dei CRT (688), bachelite (Alfa)
inline constexpr int LAMPADE = 30;                  // 30-35: rossa, ambra, verde; accesa = pari, spenta = dispari
inline constexpr int TARGHETTA = 36;                // 36 fondo, 37 fondo in ombra, 38 scritta, 39 mezzo tono della scritta
inline constexpr int FOSFORO = 40, N_FOSFORO = 8;   // 40-47: vetro spento, alone, tratto, nucleo
inline constexpr int PRIMO = 16, ULTIMO = 47;
}  // namespace colori_tema

enum class BattelloTema : std::uint8_t { usa688, alfa };
enum class LuceSala : std::uint8_t { bianca, rossa, blu };
enum class ColoreLampada : std::uint8_t { rossa, ambra, verde };
enum class Fosforo : std::uint8_t { del_battello, verde, verde_giallino, ambra };

struct TemaStazione {
    BattelloTema battello = BattelloTema::usa688;
    LuceSala luce = LuceSala::bianca;
    Fosforo fosforo = Fosforo::del_battello;   // del battello: verde sul 688, verde giallino sull'Alfa
};

// Scrive gli indici 16-47 della palette per il tema
void applica_tema(Palette& p, const TemaStazione& t);

// Indici di comodo
inline std::uint8_t smalto(int tono) {   // 0 (scuro) .. N_SMALTO-1 (chiaro), fuori dai limiti tagliato
    return static_cast<std::uint8_t>(colori_tema::SMALTO + (tono < 0 ? 0 : tono >= colori_tema::N_SMALTO ? colori_tema::N_SMALTO - 1 : tono));
}
inline std::uint8_t lampada(ColoreLampada c, bool accesa) {
    return static_cast<std::uint8_t>(colori_tema::LAMPADE + 2 * static_cast<int>(c) + (accesa ? 0 : 1));
}
inline std::uint8_t fosforo(int tono) {   // 0 vetro spento .. N_FOSFORO-1 nucleo
    return static_cast<std::uint8_t>(colori_tema::FOSFORO + (tono < 0 ? 0 : tono >= colori_tema::N_FOSFORO ? colori_tema::N_FOSFORO - 1 : tono));
}

}  // namespace m688
