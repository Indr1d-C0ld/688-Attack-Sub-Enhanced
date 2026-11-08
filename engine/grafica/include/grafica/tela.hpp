#pragma once
// Tela unica 640x480 a 256 colori indicizzati (docs/00_ROADMAP.md, "Grafica"). Tutto il disegno avviene qui;
// la piattaforma converte gli indici con la palette della scena solo al momento di presentare.

#include <cstdint>
#include <string>
#include <vector>

namespace m688 {

inline constexpr int LARGHEZZA_TELA = 640;
inline constexpr int ALTEZZA_TELA = 480;
inline constexpr std::uint8_t TRASPARENTE = 159;   // indice trasparente degli sprite (come nell'originale)

struct Rettangolo {
    int x = 0, y = 0, w = 0, h = 0;
};

struct Sprite;
class Palette;

class Tela {
public:
    Tela(int w = LARGHEZZA_TELA, int h = ALTEZZA_TELA);

    int larghezza() const { return w_; }
    int altezza() const { return h_; }
    const std::uint8_t* dati() const { return pix_.data(); }
    std::uint8_t at(int x, int y) const { return pix_[static_cast<std::size_t>(y) * w_ + x]; }

    void imposta_ritaglio(Rettangolo r);   // il disegno e' limitato a questo rettangolo
    void togli_ritaglio();
    const Rettangolo& ritaglio() const { return rit_; }
    // Riga y per i cicli di disegno veloci: chi scrive resta dentro ritaglio() da solo.
    std::uint8_t* riga(int y) { return pix_.data() + static_cast<std::size_t>(y) * w_; }

    void riempi(std::uint8_t c);
    void pixel(int x, int y, std::uint8_t c);
    void rettangolo_pieno(int x, int y, int w, int h, std::uint8_t c);
    void linea_orizzontale(int x0, int x1, int y, std::uint8_t c);
    void linea_verticale(int x, int y0, int y1, std::uint8_t c);

    // Sprite con ancora in (x, y) della tela, ingrandito di `scala` (campionamento al pixel piu' vicino).
    // Le righe della tela oltre `y_limite` non vengono disegnate (nave in parte sotto l'orizzonte).
    // Con `foschia` (0-1) i grigi degli sprite si schiariscono, retinati, verso il grigio `grigio_foschia` (0-14,
    // prospettiva aerea: v' = v (1 - f) + g f).
    void disegna_sprite(const Sprite& s, double x, double y, double scala, int y_limite = 1 << 30, double foschia = 0.0,
                        double grigio_foschia = 13.0);

    // Istantanea BMP a 8 bit con la palette indicata (verifiche senza finestra).
    void salva_bmp(const std::string& percorso, const Palette& pal) const;

private:
    int w_, h_;
    Rettangolo rit_;
    std::vector<std::uint8_t> pix_;
};

}  // namespace m688
