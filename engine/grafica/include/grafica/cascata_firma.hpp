#pragma once
// Cascata dimostrativa della firma acustica di una classe, per il Museo (enciclopedia, roadmap punto 14): BTR e LOFAR
// in piccolo come alla stazione sonar (app/sonar_enhanced), ma ferme. La classe passa davanti a un ricevitore tipico
// con le righe dei macchinari, la pala e la cavitazione, contro un rumore ambiente medio; la fisica e' quella della
// stazione (nucleo/analisi_acustica: cascata e spettro_fascio, con la grana del rumore), i colori la scala in fosforo
// verde del sonar del 1989.

#include <cstdint>
#include <string>

#include "grafica/palette.hpp"
#include "grafica/tela.hpp"
#include "nucleo/acustica.hpp"

namespace m688 {

class Font;

inline constexpr int CASCATA_BASE = 200, CASCATA_LIVELLI = 32;   // gli indici della scala alla stazione sonar

// La scala dei colori della cascata (fosforo verde, nero sotto il pavimento) negli indici base..base+livelli-1
void applica_palette_cascata(Palette& p, int base = CASCATA_BASE, int livelli = CASCATA_LIVELLI);

// Riempie r: BTR a sinistra (rilevamento vero 0-360 per tempo, la riga piu' recente in alto), LOFAR a destra (10-410 Hz
// del fascio sul bersaglio, normalizzato come gli schermi veri). Per un quarto d'ora la classe passa al traverso: nella
// meta' piu' vecchia (in basso) piano, sotto la cavitazione, con le righe dei macchinari; poi accelera fino a
// velocita_kn e il continuo della cavitazione le copre e accende la traccia. velocita_kn <= 0: appena sopra la
// cavitazione (navi: 15 kn). sensore vuoto o attivo: il passivo corrispondente, altrimenti la sfera del BQQ-5. Con
// font le diciture. Classe senza firma: solo il fondo.
void disegna_cascata_firma(Tela& t, Rettangolo r, const CatalogoAcustico& c, const std::string& classe, double velocita_kn,
                           std::uint64_t seme, const std::string& sensore = "", const Font* font = nullptr,
                           int base = CASCATA_BASE, int livelli = CASCATA_LIVELLI);

// Per le prove e per le didascalie: la scena scelta (velocita', distanza, sensore usato, cavitazione)
struct ScenaCascata {
    std::string sensore;
    double velocita_kn = 0.0, distanza_m = 0.0, quota_m = 0.0;
    double velocita_lenta_kn = 0.0;   // la meta' piu' vecchia, prima della rampa
    bool cavitazione = false;         // alla velocita' finale
};
ScenaCascata scena_cascata(const CatalogoAcustico& c, const std::string& classe, double velocita_kn, const std::string& sensore = "");

}  // namespace m688
