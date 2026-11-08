#pragma once
// Sprite delle navi (file .spr di tools/pipeline/esporta_motore.py): per ogni angolo sulla prua (AOB) e
// lato, uno sprite a 16 grigi con l'ancora nel centro nave al galleggiamento.

#include <cstdint>
#include <string>
#include <vector>

namespace m688 {

struct Sprite {
    int aob = 0;              // gradi, 0 = prua verso l'osservatore
    bool dritta = true;       // osservatore sulla dritta della nave
    int elevazione = 0;       // gradi: 0 per le navi; oggetti in volo visti dal basso
    int w = 0, h = 0;
    float ancora_x = 0.0f;    // centro nave al galleggiamento, in pixel dello sprite
    float ancora_y = 0.0f;
    std::vector<std::uint8_t> pix;   // w*h indici di palette, TRASPARENTE dove non c'e' nave
};

class InsiemeSprite {
public:
    static InsiemeSprite carica(const std::string& percorso);   // lancia std::runtime_error

    float scala_px_m() const { return scala_; }   // pixel per metro alla scala degli sprite
    std::size_t numero() const { return sprite_.size(); }
    // Sprite dell'angolo piu' vicino (AOB in gradi, 0-180) sul lato indicato e, per gli oggetti in volo, della
    // elevazione piu' vicina.
    const Sprite& per_angolo(double aob, bool dritta, double elevazione = 0.0) const;

private:
    float scala_ = 3.75f;
    std::vector<Sprite> sprite_;
};

}  // namespace m688
