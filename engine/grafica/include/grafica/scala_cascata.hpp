#pragma once
// Le scale di colore delle cascate del sonar (roadmap 33, rifinitura estetica; docs/ricerca/25_estetica_simulatori.md
// §9 e la nota sonar-confronto-27-10.md del disassemblatore). Tre pezzi:
// - le tinte (fosforo P1, grigio, viridis, turbo, caldo), le stesse della stazione, interpolate su quanti gradini si
//   vuole;
// - la scala lunga: quando la stazione copre la vista prende 128 indici (96-223, mare, grigi degli sprite, fuoco e
//   cielo, che sul sonar non si vedono) invece dei 32 di oggi (200-231), e il dithering ordinato Bayer 4x4 fra due
//   gradini vicini: circa 16 sotto-livelli percepiti per gradino, con una grana ferma che non tremola;
// - il fosforo del modo 1989: un'intensita' per pixel che decade (persistenza), le tracce con l'antialias, il
//   bagliore attorno ai punti accesi, la grana e una riga su due piu' scura, come un CRT verde.
// Il dato resta del sonar (dB per pixel, pavimento e tetto); qui c'e' solo il passaggio dal dB al colore.

#include <array>
#include <cstdint>
#include <vector>

#include "grafica/tela.hpp"

namespace m688 {

class Palette;

namespace cascate {

// fosforo_alfa: il verde giallino dei CRT dell'Alfa (dossier 19 §8; STIMA: dal nero a circa 550 nm), per Okean nel 1989
enum class Tinta : std::uint8_t { fosforo, grigio, viridis, turbo, caldo, fosforo_alfa };

// Gli indici della palette di una scala: primo .. primo+livelli-1
struct Scala {
    int primo = 96, livelli = 128;
};
inline constexpr Scala SCALA_LUNGA{96, 128};   // la stazione copre la vista
inline constexpr Scala SCALA_CORTA{200, 32};   // la scala di oggi (Museo, stazione in un riquadro)

// Il colore della tinta a t in [0,1]
std::array<std::uint8_t, 3> colore(Tinta tinta, double t);
// Scrive la scala nella palette; l'indice 0 della scala resta nero (sotto il pavimento il fondo e' nero)
void applica(Palette& p, Tinta tinta, Scala s = SCALA_LUNGA);

// La frazione fra pavimento e tetto, in [0,1]
double frazione(double db, double pavimento, double tetto);
// L'indice del pixel (x, y) per la frazione u: con il dithering Bayer 4x4 fra i due gradini vicini
std::uint8_t indice(double u, int x, int y, Scala s = SCALA_LUNGA, bool dithering = true);
// La soglia di Bayer 4x4 del pixel, in (0,1)
double soglia_bayer(int x, int y);

// I tre gradi della persistenza (un tasto della stazione, salvato nelle preferenze; scelta dell'utente del 28/10/2026)
enum class Persistenza : std::uint8_t { breve, normale, lunga };
// Il coefficiente a fotogramma del grado: 0,80, 0,90, 0,95
double coefficiente(Persistenza p);

// Il fosforo: un'intensita' 0-1 per pixel, che si compone sulla tela con la scala. La stazione ne tiene uno per
// riquadro (BTR, A-scan, LOFAR, DEMON) e a ogni fotogramma fa: decadi(coefficiente(grado)), poi deponi o linea coi dati
// nuovi, poi componi
class Fosforo {
public:
    Fosforo(int larghezza, int altezza);

    int larghezza() const { return w_; }
    int altezza() const { return h_; }
    void spegni();
    // Persistenza: ogni pixel perde la frazione 1-k (k fra 0,85 e 0,95 a fotogramma, STIMA)
    void decadi(double k);
    // Accende un punto: tiene il massimo fra il vecchio e il nuovo
    void deponi(int x, int y, double v);
    // Una traccia senza scalini (Xiaolin Wu): ogni pixel prende v per la sua copertura
    void linea(double x0, double y0, double x1, double y1, double v);
    double at(int x, int y) const;

    struct Effetti {
        double bagliore = 0.35;   // quanto dei vicini piu' accesi si somma (alone 3x3)
        double righe = 0.85;      // le righe dispari valgono tanto: il reticolo del CRT
        double grana = 0.035;     // rumore fermo sommato, prima della scala
        // il fondo: un velo di grana sotto il pavimento (0-1, 0 spento), come il rumore sempre acceso dello schermo di
        // un BQQ-5; senza, i dati vicini al pavimento lasciano lo schermo nero. Proposta del 28/10/2026, 0,07 nelle bozze
        double fondo = 0.0;
        bool dithering = true;
    };
    // Disegna tutto il fosforo con l'angolo in (x, y)
    void componi(Tela& t, int x, int y, Scala s, const Effetti& e) const;

private:
    int w_, h_;
    std::vector<float> v_;
};

}  // namespace cascate
}  // namespace m688
