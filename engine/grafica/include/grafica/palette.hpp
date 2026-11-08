#pragma once
// Palette a 256 colori (file .pal: 768 byte RGB a 8 bit, esportati da DAY.PAL e NIGHT.PAL dell'originale).

#include <array>
#include <cstdint>
#include <string>
#include <utility>
#include <vector>

namespace m688 {

class Palette {
public:
    static Palette carica(const std::string& percorso);   // lancia std::runtime_error se il file non va bene

    // Miscela lineare indice per indice (t = 0: a, t = 1: b).
    static Palette mescola(const Palette& a, const Palette& b, double t);
    // Copia con gli intervalli di indici [da, a] scuriti secondo la luce ambiente (0 notte, 1 giorno) e
    // virati verso il blu al buio; gli altri indici (interfaccia) restano invariati.
    Palette illuminata(double luce, const std::vector<std::pair<int, int>>& intervalli) const;
    // Copia con gli intervalli schiariti verso il bianco della frazione f (lampo di un'esplosione).
    Palette schiarita(double f, const std::vector<std::pair<int, int>>& intervalli) const;
    // Verso il nero di una frazione f (0-1): luci che tremolano dopo una scossa
    Palette scurita(double f, const std::vector<std::pair<int, int>>& intervalli) const;

    // Un indice ridefinito (scale di colori delle stazioni che coprono la vista: cascate del sonar)
    void imposta(int i, std::uint8_t r, std::uint8_t g, std::uint8_t b) {
        dati_[3 * i] = r;
        dati_[3 * i + 1] = g;
        dati_[3 * i + 2] = b;
    }
    std::array<std::uint8_t, 3> rgb(int i) const { return {dati_[3 * i], dati_[3 * i + 1], dati_[3 * i + 2]}; }
    std::uint32_t argb(int i) const {
        return 0xff000000u | (std::uint32_t{dati_[3 * i]} << 16) | (std::uint32_t{dati_[3 * i + 1]} << 8) |
               dati_[3 * i + 2];
    }

private:
    std::array<std::uint8_t, 768> dati_{};
};

// Fasce della palette (verificate il 25/10/2026 sul codice e su 56 istantanee di stazioni, periscopio, sala siluri,
// quadro danni e Museo). Chi aggiunge una rampa ne prende una libera e la scrive qui.
//   0-15     EGA dell'interfaccia (colori:: qui sotto)
//   16-47    tema delle stazioni della Fase 3 (grafica/tema_stazioni.hpp): smalto, accenti, lampade, targhette, fosforo
//   48-79    rilievo della terra della carta (grafica/carta_vga: 8 tinte x 4 luci); nel debriefing la cartolina sta in
//            16-47. 16-79 e' anche di ritratti, cerimonie e decorazioni, in schermate a se'
//   80-95    interfaccia dell'originale
//   96-143   mare della vista; ridefinita da carta (colori_carta) e spaccato a tutto schermo
//   144-158  grigi degli sprite (foschia, nubi, pioggia, neve); 159 trasparente
//   160-174  rampa monocroma dell'oculare (grafica/oculare.hpp: termico, intensificatore, TV)
//   175-198  fuoco, fumo acceso, fango, fuliggine (colori:: qui sotto); 185-191 anche nelle sagome delle armi
//   199-247  cielo della vista; ridefinita da carta (199-231), cascata del sonar e del Museo (200-231), spaccato
//   248-253  liberi (pochi); 254 verde scuro delle sagome; 255 bianco
// Le fasce 16-79, 96-143, 144-158, 196-198 e 199-247 sono illuminate dalla luce della vista (INDICI_VISTA): il tema
// delle stazioni si applica dopo Palette::illuminata.

// Indici della palette del giorno usati dalla grafica (dalla palette dell'originale).
namespace colori {
inline constexpr int NERO = 0, BLU = 1, VERDE = 2, CIANO = 3, ROSSO = 4, MARRONE = 6, GRIGIO = 7;
inline constexpr int GRIGIO_SCURO = 8, VERDE_CHIARO = 10, CIANO_CHIARO = 11, GIALLO = 14, BIANCO = 15;
inline constexpr int CIELO_ALTO = 240, CIELO_ORIZZONTE = 199;   // rampa 199-247: celeste -> blu
inline constexpr int MARE_ORIZZONTE = 128, MARE_VICINO = 98;    // rampa 96-143: blu chiaro -> blu notte
inline constexpr int GRIGI = 144;                               // 144-158: grigi degli sprite
// effetti (docs/10_danni_affondamento_esplosioni.md §5.7): fuoco emissivo dalla brace al bianco-giallo (175-178
// dell'originale e posti liberi 190, 192), fumo illuminato dal fuoco di notte (193-194, emissivi), fango (196-197)
// e fuliggine (198); fumo, spruzzi e schiuma usano i grigi degli sprite
inline constexpr int FUOCO[6] = {190, 175, 176, 177, 178, 192};
inline constexpr int FUMO_ACCESO = 193, FUMO_ACCESO_CHIARO = 194;
inline constexpr int FANGO = 196, FANGO_CHIARO = 197, FULIGGINE = 198;
}  // namespace colori

}  // namespace m688
