#pragma once
// Acustica delle collisioni fra unita' (dossier docs/ricerca/16_collisioni.md §2.6, citato «16»; il contratto dei danni
// e' nucleo/collisioni.hpp). Solo Enhanced.
//
// La missione, per ogni Urto valutato, ne mette le sorgenti nella scena (sonar, cascata, cuffia, ascolto dell'IA: dopo
// l'urto entrambi sanno) e AmbienteBordo::urto (nucleo/bordo_audio.hpp) ne fa il suono a bordo.
// - Il colpo: un transitorio metallico forte al punto di contatto, col livello di EsitoUrto (16 §2.6: STIMA 170 dB re
//   1 uPa a 1 m per 1 MJ, +10 per decade). Sordo e grave se si toccano gli scafi; piu' acuto e squillante se c'e' di
//   mezzo una lama (vela, alberi, timoni, eliche, schiera, sovrastrutture).
// - Lo stridio dello strisciamento, per durata_strisciamento_s, a pezzi di 1 s: porta l'energia tangente distribuita
//   sulla sua durata (STIMA: 160 dB per 1 MJ in 1 s, +10 per decade, fra 130 e 180).
// - Un'elica che colpisce: i colpi delle pale per qualche secondo, mentre l'asse si ferma.
// I rumori di danno che restano (elica sbilanciata, piastrelle) sono l'aumento permanente di DanniUrtoUnita::rumore_db.
//
// Il modulo e' puro e deterministico.

#include <cstdint>
#include <vector>

#include "nucleo/acustica_asw.hpp"
#include "nucleo/collisioni.hpp"

namespace m688 {

// Il livello dello stridio (dB re 1 uPa a 1 m) per l'energia tangente e la durata dello strisciamento
double livello_stridio_db(double energia_tangente_mj, double durata_s);
// Vero se la parte colpita e' una lama o un'appendice: il colpo suona metallico
bool parte_squillante(ParteUrto p);

// Le sorgenti della scena di un urto (voci [transitori.urto_*] di acustica.toml), con id da `id_base` (al piu' 22: il
// colpo, i pezzi dello stridio e l'elica)
std::vector<Transitorio> transitori_urto(const Urto& u, const EsitoUrto& e, int id_base, std::uint64_t seme);

}  // namespace m688
