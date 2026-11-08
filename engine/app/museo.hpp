#pragma once
// Museo dell'Enhanced (requisito 14 di docs/00_ROADMAP.md): l'enciclopedia di tutto cio' che c'e' nella simulazione
// (nucleo/enciclopedia.hpp, dati e testi del kit), sfogliabile per sezioni. A sinistra le voci della sezione, a destra
// la voce: nome, un'immagine alla volta (sprite da piu' angolazioni, modello 3D in posa da museo, sagoma dell'arma,
// oculare del periscopio, boa o marcatore in mare, cascata della firma acustica, carta d'epoca, spaccato col sistema,
// foto «alla 1989»), la scheda tecnica con la confidenza, il testo
// storico, il manuale di bordo e i riferimenti, da seguire con le cifre. Italiano o inglese.
//
// Comandi: frecce su e giu' la voce, sinistra e destra la sezione, Invio l'indice delle sezioni, Tab l'immagine, Pag su e
// giu' il testo, 1-9 il riferimento, L la lingua, Esc indietro (dall'indice, dopo un riferimento) o fuori. Col clic: i
// tasti < e > la sezione, il suo nome l'indice, una riga la voce o la sezione, i tre tasti del piede.

#include <map>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "grafica/font.hpp"
#include "grafica/oculare.hpp"
#include "grafica/palette.hpp"
#include "grafica/tela.hpp"
#include "grafica/testo_font.hpp"
#include "nucleo/enciclopedia.hpp"
#include "piattaforma/finestra.hpp"
#include "risorse.hpp"

namespace m688 {

class SchermataMuseo {
public:
    // `dati`: la cartella del motore (immagini, carico, periscopi); `voce`: quella da aprire ("" la prima)
    SchermataMuseo(const Enciclopedia& e, const Risorse& r, std::string dati, const std::string& voce = "");
    bool comandi(const Comandi& c, double dt);   // vero: si esce
    void disegna(Tela& t, const Font& f) const;
    void palette(Palette& p) const;               // quella dell'immagine in vista: oculare, cascata, carta o spaccato
    void vai(const std::string& id);              // apre una voce (e la sua sezione)
    const VoceEnciclopedia* voce() const;
    void lingua_inglese(bool si) { inglese_ = si; }
    void immagine(int k) { immagine_ = k; }
    int sezione() const { return sezione_; }
    int quante_sezioni() const { return static_cast<int>(sezioni_.size()); }
    bool indice_aperto() const { return indice_; }

private:
    void apri_sezione(int s);                     // la sezione s (in giro), dalla prima voce
    std::string nome_sezione(int s) const;
    void disegna_elenco(Tela& t, const Font& f) const;
    void disegna_voce(Tela& t, const Font& f, const VoceEnciclopedia& v) const;
    void disegna_immagine(Tela& t, const Font& f, const ImmagineVoce& im, Rettangolo r) const;
    const std::vector<const VoceEnciclopedia*>& voci_sezione() const;

    const Enciclopedia& e_;
    const Risorse& r_;
    std::string dati_;
    std::vector<std::string> sezioni_;        // nell'ordine del Museo
    int sezione_ = 0, riga_ = 0, immagine_ = 0, scorri_ = 0;
    bool inglese_ = false;
    std::vector<std::string> storia_;         // le voci da cui si e' arrivati coi riferimenti (Esc torna)
    bool indice_ = false;                     // nel vetro l'indice delle sezioni al posto delle voci
    int riga_indice_ = 0;
    int prec_su_giu_ = 0;
    double ripeti_ = 0.35;
    mutable std::vector<const VoceEnciclopedia*> elenco_;
    mutable int elenco_di_ = -1;
    mutable std::unique_ptr<Tela> cache_;     // l'immagine disegnata (voce e indice)
    mutable std::string cache_di_;
    mutable ParametriOculare oculare_;        // dell'immagine in vista, per la palette
    mutable std::string tipo_mostrato_;       // cascata, carta e spaccato hanno le loro scale di colore
    // gruppo 4 della passata estetica (scelta dell'utente del 29/10/2026): lo schedario e i documenti sulla lamiera,
    // portata nei colori della vista (lamiera::in_colori_vista) per non toccare le immagini
    std::optional<Font> targa_;               // incisione688_7, per i nomi incisi
    mutable std::optional<Palette> col_tema_; // il giorno col tema delle stazioni, per la rimappatura
    mutable int pagine_testo_ = 1;            // le pagine del testo della voce (il piede: PAG SEGUE k/n)
    mutable Rettangolo riquadro_immagine_{218, 61, 236, 128};
};

}  // namespace m688
