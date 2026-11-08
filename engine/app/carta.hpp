#pragma once
// Carta della stazione Navigazione (F6) nell'area della vista (docs/11_comandi_e_stazioni.md §4, fase C4; regole di
// docs/originale/numeri_eseguibile.md §3.2): fondale del teatro, proprio battello al centro, contatti del giocatore
// come X o lettere, punti di rotta numerati, proiezione a N minuti, cursore per scegliere un punto.
//
// Un pixel della carta originale (320x200) sono due pixel della tela: la scala S resta quella dell'originale, ma
// l'area (640x360) mostra piu' mare del riquadro di 168x92 dell'originale.

#include <optional>
#include <vector>

#include "grafica/carta_vga.hpp"
#include "grafica/font.hpp"
#include "grafica/palette.hpp"
#include "grafica/tela.hpp"
#include "missioni/contatti_giocatore.hpp"
#include "missioni/missione.hpp"
#include "nucleo/carta_giocatore.hpp"

namespace m688 {

// Indici della palette ridefiniti mentre la carta dell'Enhanced copre la vista (le rampe del mare e del cielo, che
// qui non si vedono; docs/ricerca/12 §5.6)
namespace colori_carta {
inline constexpr int ACQUA = 96;            // 16 fasce di profondita' x 3 ombre (scura, media, chiara)
inline constexpr int COSTA = 199, TERRA = 200;   // linea di costa; 7 fasce di quota
inline constexpr int ISOBATA = 207, ISOBATA_PRINCIPALE = 208, SICUREZZA = 209;
inline constexpr int RETICOLO = 210, GRIGLIA = 211;
inline constexpr int MATITA = 212;          // 6 matite: nero, rosso, blu, verde, giallo, bianco
inline constexpr int NTDS = 218;            // amico, ostile, ignoto, neutrale; da 222 gli stessi attenuati
inline constexpr int ZONA = 226;            // esclusione, azione, intelligence
inline constexpr int MAGENTA = 229, CURSORE = 230, EVIDENZA = 231;
}  // namespace colori_carta

// Le scale di colore della carta per lo stile scelto
void applica_palette_carta(Palette& p, StileCarta stile);
// Il colore della carta per una profondita' (negativa sulla terra), senza ombre: le anteprime dei teatri
std::uint8_t colore_carta(double profondita);

struct VistaCarta {
    int x = 0, y = 0, w = LARGHEZZA_TELA, h = 360;   // area della tela
    Vec2 centro;                                     // metri, il proprio battello
    double metri_per_pixel = 1.0;                    // S unita' di gioco per pixel originale, su due pixel della tela
    Vec2 a_tela(Vec2 p) const;
    Vec2 a_mondo(Vec2 q) const;
};

VistaCarta vista_carta(const Missione& m, const CartaGiocatore& c);

class Meteo;
// Strati meteo dell'Enhanced (docs/15_meteo_e_mare.md, M3; grafica/strati_meteo), con quello che il battello sa:
// - vento e mare veri entro 5 nm dal battello (li misura), piu' lontano quelli dell'ultimo bollettino (00, 06, 12, 18 UTC);
// - celle temporalesche solo con la testa fuori (quota periscopica o superficie), entro 100 km (i lampi);
// - nebbia vera vicino col la testa fuori, lontano se il bollettino la da' («possibili banchi»);
// - ghiaccio dalla climatologia (lo danno le carte del ghiaccio).
// Sotto il livello CartaGiocatore::livelli.meteo, spento nel classico
// - i fulmini sentiti al sonar negli ultimi 10 minuti, come rilevamenti dal battello (anche in profondita').
void disegna_meteo_carta(Tela& t, const Font& f, const VistaCarta& v, const Missione& m, const Meteo& meteo, Vec2 battello,
                         bool testa_fuori);

// La carta d'epoca per il Museo (enciclopedia): con `punto` un ritaglio largo larghezza_km centrato sul punto, con la
// base segnata; senza (nullptr, o carta non georeferenziata) il teatro intero. Disegno VGA della carta (fondale, zone
// non rilevate, isobate) e una scala in miglia. Palette: applica_palette_carta(p, StileCarta::vga).
void disegna_carta_museo(Tela& t, const Font& f, Rettangolo r, const Fondale& carta, const Geografiche* punto,
                         double larghezza_km = 150.0);

// Fase 3 (kit, grafica/carta_vga): nell'Enhanced, stili vga e nautica, il fondale al pixel col rilievo, le coste
// lisce e le isobate coi numeri, in cache (una sola, per la stazione, il briefing e il debriefing: non si vedono mai
// insieme). I numeri delle isobate col font della carta: incisione NATO e stampino sovietico (nullptr: senza numeri).
// La palette aggiunge il rilievo della terra (48-79) con applica_palette_rilievo, dopo applica_palette_carta.
void imposta_font_isobate(const Font* nato, const Font* urss);
// Il testo col contorno a croce dell'originale (1000:57CC, numeri_eseguibile.md §3.2): quattro copie nel colore del
// contorno spostate di un pixel a sinistra, a destra, sopra e sotto, poi il testo nel suo colore. Sulla carta e al
// periscopio: lettere, cifre dei punti di rotta e asterischi con contorno nero, le X col rosso scuro (EGA 4)
void scrivi_contornato(Tela& t, const Font& f, int x, int y, const std::string& s, std::uint8_t colore, std::uint8_t contorno);
// La cartella dei dati per i toponimi dei teatri (grafica/carta_vga: data/motore/teatri/<teatro>.toponimi.toml); vuota,
// la carta resta senza nomi
void imposta_cartella_dati_carta(std::string cartella);

// `bersaglio`: evidenziato; `cursore`: punto in scelta (SET), in metri
// `area`: un'altra porzione della tela e un altro centro (la carta del briefing); senza, la vista della stazione
void disegna_carta(Tela& t, const Font& f, const Missione& m, const CartaGiocatore& c, const std::vector<SegnoContatto>& segni,
                   int bersaglio, std::optional<Vec2> cursore, const VistaCarta* area = nullptr);

// Segni visibili sulla carta con la loro posizione sulla tela (per TARGET): le X a 25 pixel originali dal battello
struct SegnoSuCarta {
    SegnoContatto segno;
    Vec2 q;   // pixel della tela
};
std::vector<SegnoSuCarta> segni_su_carta(const VistaCarta& v, const Missione& m, const std::vector<SegnoContatto>& segni);

}  // namespace m688
