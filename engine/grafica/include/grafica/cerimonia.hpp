#pragma once
// Grafica delle schermate attorno alla missione (Fase 4a, M4, docs/12_schermate_di_missione.md): rampe di palette
// comuni a ritratto, medaglie, nastrini, cornici e timbri, in stile VGA come lo spaccato.
//
// Le schermate di cerimonia e del fascicolo coprono la vista: le rampe vanno negli indici della vista 16-79, 96-143
// e 199-247 (imposta_palette). Il debriefing con le minicartoline tiene invece la palette della vista: li' cornici e
// timbri usano solo l'interfaccia (0-15, 80-95) e i grigi 144-158 (cornici.hpp, modo «vista»).

#include <array>
#include <cstdint>
#include <string>

#include "grafica/palette.hpp"
#include "nucleo/fascicolo.hpp"   // Marina

namespace m688::cerimonia {

// --- rampe dal buio alla luce ---
inline constexpr int PELLE = 16, N_PELLE = 10;        // 16-25
inline constexpr int CAPELLI = 26, N_CAPELLI = 6;     // 26-31 (colore scelto per ritratto: imposta_capelli)
inline constexpr int BLU = 32, N_BLU = 8;             // 32-39 Service Dress Blue
inline constexpr int NERO = 40, N_NERO = 8;           // 40-47 tuzurka della VMF
inline constexpr int BIANCO = 48, N_BIANCO = 5;       // 48-52 camicia, calotta del berretto
inline constexpr int ORO = 53, N_ORO = 8;             // 53-60 galloni, bottoni, dolphins, medaglie dorate
inline constexpr int ARGENTO = 61, N_ARGENTO = 6;     // 61-66
inline constexpr int BRONZO = 67, N_BRONZO = 6;       // 67-72
inline constexpr int SMALTO = 73, N_SMALTO = 6;       // 73-78 smalto rosso
inline constexpr int OCCHI = 79;                      // iride
// colori piatti dei nastrini (96-111): 96-109 dai colori di carriera.toml (TavolaDecorazioni::imposta_palette),
// poi il nero delle pieghe e il bianco delle stelline
inline constexpr int NASTRO = 96, N_NASTRO = 14, NERO_N = 110, BIANCO_N = 111;
inline constexpr int SFONDO = 112, N_SFONDO = 8;      // 112-119 fondale dello studio fotografico
inline constexpr int KAKI = 120, N_KAKI = 8;          // 120-127
inline constexpr int CARTA = 128, N_CARTA = 6;        // 128-133 carta del fascicolo
inline constexpr int TIMBRO = 134, N_TIMBRO = 4;      // 134-137 inchiostro rosso
inline constexpr int SMALTO_BLU = 138, N_SMALTO_BLU = 6;   // 138-143 smalti blu e azzurri
inline constexpr int VELLUTO = 199, N_VELLUTO = 8;    // 199-206 fodera dell'astuccio (blu USA, rosso URSS)
inline constexpr int LEGNO = 207, N_LEGNO = 8;        // 207-214 astuccio e scrivania
inline constexpr int VIOLA = 215, N_VIOLA = 6;        // 215-220 smalto del Purple Heart
inline constexpr int VERDE = 221, N_VERDE = 6;        // 221-226 alloro smaltato
inline constexpr int BANDIERA = 227, N_BANDIERA = 8;  // 227-234 tessuto del fondale (bandiera)
inline constexpr int LABBRA = 235, N_LABBRA = 4;      // 235-238
inline constexpr int IRIDE = 239, N_IRIDE = 4;        // 239-242 (colore degli occhi scelto per ritratto)
inline constexpr int GUANCE = 243, N_GUANCE = 5;      // 243-247 pelle arrossata (labbra, naso, orecchie in ombra)

// Tutte le rampe; il velluto e la bandiera secondo la marina
void imposta_palette(Palette& p, Marina m);
// Capelli: colore base (r, g, b) e frazione di grigio (eta')
void imposta_capelli(Palette& p, std::array<std::uint8_t, 3> base, double grigio);
// Carnagione (0 chiara - 1 olivastra) e colore degli occhi
void imposta_volto(Palette& p, double carnagione, std::array<std::uint8_t, 3> occhi);

// --- aiuti di disegno condivisi ---
double bayer(int x, int y);                         // soglia 0-1 della matrice 4x4
int retina(double t, int x, int y, int massimo);   // tono continuo -> intero, con retinatura
double caso(int a, int b, int c = 0);              // 0-1 deterministico

}  // namespace m688::cerimonia
