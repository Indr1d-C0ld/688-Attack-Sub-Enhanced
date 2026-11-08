#pragma once
// I due replay affiancati di una coppia del confronto A/B (docs/28 §5, D2): a sinistra la partita A (senza gli
// interventi), a destra la B (con), la stessa scala, lo stesso centro e lo stesso istante, fermi all'apertura sul punto di
// divergenza. Sotto ogni carta gli ultimi fatti del suo registro fino all'istante; in fondo la linea del tempo con il
// punto di divergenza (giallo) e gli interventi di B (i triangoli del vetrino). Nello stile C del replay (grafica/quadro).
//
// Comandi: P marcia o pausa, frecce 30 s indietro o avanti, + e - (o Pag) la velocita', I e O lo zoom, trascinare
// sposta le due carte insieme, un clic sulla linea va a quell'istante, D torna al punto di divergenza, Esc esce.

#include <memory>
#include <optional>
#include <string>

#include "grafica/font.hpp"
#include "grafica/palette.hpp"
#include "grafica/tela.hpp"
#include "nucleo/traccia_replay.hpp"
#include "piattaforma/finestra.hpp"
#include "replay.hpp"

namespace m688 {

class ReplayAffiancati {
public:
    // `t_divergenza` < 0: dall'inizio; `seme` per la testata
    ReplayAffiancati(std::string dati, Traccia a, Traccia b, double t_divergenza, std::uint64_t seme = 0);

    bool comandi(const Comandi& c, double dt);   // vero: si esce
    void disegna(Tela& t, const Font& f) const;
    void palette(Palette& p) const;

    double tempo() const { return t_; }
    void vai(double t);
    const SchermataReplay& a() const { return *a_; }
    const SchermataReplay& b() const { return *b_; }
    Rettangolo carta_a() const { return carte_[0]; }   // dell'ultimo disegno (per i clic e le prove)
    Rettangolo carta_b() const { return carte_[1]; }
    Rettangolo linea() const { return barra_; }

private:
    void centra();   // le due carte sul punto della divergenza (o sul battello)
    std::string dati_;
    std::unique_ptr<SchermataReplay> a_, b_;
    double t_ = 0.0, t_div_ = -1.0, durata_ = 0.0, velocita_ = 4.0, salto_attesa_ = 0.0, mpp_ = 120.0;
    bool pausa_ = true;
    int salto_verso_ = 0;
    std::uint64_t seme_ = 0;
    Vec2 centro_;
    std::optional<std::pair<int, int>> trascina_;
    std::optional<Font> targa_, fosforo_;
    mutable Rettangolo carte_[2]{{12, 40, 302, 290}, {326, 40, 302, 290}}, barra_{120, 403, 400, 7};
};

}  // namespace m688
