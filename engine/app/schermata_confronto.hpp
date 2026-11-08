#pragma once
// La schermata del confronto di un esperimento A/B del vetrino di Dio (docs/28 §5, D2; app/confronto_ab.hpp): nello
// stile C delle serie (grafica/quadro: la consolle «SALA ANALISI», uno schermo a fosforo, le linguette, il piede), tre
// pagine, Tab o un clic sulle linguette:
// 1. esiti: per esito la frequenza in A e in B con l'intervallo di Wilson, la differenza B - A con l'intervallo appaiato
//    (in evidenza se non comprende lo zero) disegnata su una scala comune, le coppie che cambiano; sotto gli interventi;
// 2. per parte: le mediane di A e di B, la differenza con l'intervallo bootstrap, quante coppie salgono o scendono;
// 3. coppie: il punto di divergenza, quanto dopo il primo intervento, come, il primo evento che cambia. Un clic su una
//    coppia la sceglie: coppia_scelta() per i due replay affiancati, fermi al punto di divergenza.
// E scrive il rapporto (rapporto.md) nella cartella del confronto. Esc o Invio escono.

#include <filesystem>
#include <optional>
#include <string>
#include <vector>

#include "confronto_ab.hpp"
#include "grafica/font.hpp"
#include "grafica/palette.hpp"
#include "grafica/tela.hpp"
#include "piattaforma/finestra.hpp"

namespace m688 {

class SchermataConfronto {
public:
    SchermataConfronto(std::string dati, const std::filesystem::path& confronto);   // confronto.toml; std::runtime_error
    SchermataConfronto(std::string dati, DatiConfronto d, std::filesystem::path cartella);

    bool comandi(const Comandi& c, double dt);   // vero: si esce (Esc, Invio, finestra chiusa, o una coppia scelta)
    void disegna(Tela& t, const Font& f) const;
    void palette(Palette& p) const;

    // la coppia cliccata nella pagina delle coppie (l'indice in dati().coppie) e le sue due tracce
    std::optional<int> coppia_scelta() const { return coppia_; }
    std::filesystem::path traccia_a(int k) const;
    std::filesystem::path traccia_b(int k) const;
    const DatiConfronto& dati() const { return d_; }
    const std::filesystem::path& cartella() const { return cartella_; }

    void pagina(int k) { pagina_ = k; }          // per le istantanee e le prove (0-2)
    int pagina() const { return pagina_; }
    // il rapporto in <cartella>/rapporto.md (con le immagini indicate, relative); vuoto se non si e' scritto
    std::filesystem::path scrivi_rapporto(const std::vector<std::string>& immagini = {}) const;
    const std::vector<Rettangolo>& righe_coppie() const { return righe_coppie_; }   // dell'ultimo disegno (per i clic)

private:
    std::string dati_;
    DatiConfronto d_;
    std::filesystem::path cartella_;
    int pagina_ = 0, scorri_ = 0, prec_su_giu_ = 0;
    std::optional<int> coppia_;
    std::string avviso_;                         // «RAPPORTO SCRITTO IN ...»
    mutable std::vector<Rettangolo> linguette_, righe_coppie_;
    mutable std::vector<int> indici_coppie_;     // la coppia di ogni riga disegnata
    mutable int righe_visibili_ = 20;
    std::optional<Font> targa_, fosforo_;
};

}  // namespace m688
