#pragma once
// Spaccato del battello del giocatore per il quadro danni dell'Enhanced (F3), in tributo allo spaccato del classico
// (SUB1 dell'originale: prua a destra, interni ciano, macchinari bianchi, elica rossa). Profilo vero dal modello 3D
// (scafo, vela, timoni, elica, alberi), scafo resistente e compartimenti da data/motore/scafi/<classe>.toml, interni
// disegnati per classe (ponti, locali, macchinari riconoscibili). Sopra, lo stato dei danni del kit (BattelloDanni):
// acqua per compartimento (con l'assetto), falle con i getti, fuoco con il bagliore, fumo dal soffitto, compartimenti
// sventrati o implosi, paratie cedute, casse di zavorra con l'aria e l'esaurimento, sistemi guasti col contorno che
// pulsa, squadre di controllo danni al lavoro.
//
// Il quadro copre la vista, quindi usa le rampe 96-143 e 199-247 della palette (imposta_palette, come le cascate del
// sonar). Il disegno e' tutto a indici, con retinatura di Bayer fra i toni.

#include <memory>
#include <string>
#include <vector>

#include "grafica/modello.hpp"
#include "grafica/palette.hpp"
#include "grafica/sagome_armi.hpp"
#include "grafica/tela.hpp"
#include "nucleo/battello.hpp"
#include "nucleo/scafo.hpp"

namespace m688 {

class Font;

// Stato dei danni da disegnare (puntatori a dati del kit; nullptr = battello intatto)
struct StatoSpaccato {
    const std::vector<CompartimentoBattello>* compartimenti = nullptr;
    const std::vector<FallaBattello>* falle = nullptr;
    const std::vector<ParatiaBattello>* paratie = nullptr;
    const std::vector<CassaBattello>* casse = nullptr;
    const std::vector<StatoSistema>* sistemi = nullptr;
    const std::vector<SquadraBattello>* squadre = nullptr;   // controllo danni: figurine al lavoro
    // carico delle rastrelliere, un'arma per posto (sagoma_da_scheda; id vuoto = posto vuoto). nullptr: siluri della
    // classe (Mk 48, 53-65K). Se lo spaccato ha meno posti disegnati dell'elenco, li prende in proporzione.
    const std::vector<SagomaArma>* rastrelliera = nullptr;
    double quota_m = 0.0;       // per l'aria nelle casse
    double assetto_rad = 0.0;   // positivo = prua in su: l'acqua imbarcata si inclina verso poppa
    double tempo_s = 0.0;       // animazioni (fuoco, fumo, getti, lampeggio dei sistemi guasti)
    bool esaurimento = false;   // casse in esaurimento: aria e bolle dagli sfoghi
    bool scram = false;         // reattore fermo: luci d'emergenza nel compartimento del reattore

    static StatoSpaccato da(const BattelloDanni& b, double quota_m, double assetto_rad, double tempo_s);
};

class Spaccato {
public:
    // classe come in classi.toml (los_angeles, alfa, sturgeon, trafalgar; le altre con un allestimento generico)
    Spaccato(const std::string& classe, const Modello& modello, const DatiScafo& scafo);
    ~Spaccato();
    Spaccato(Spaccato&&) noexcept;

    // Rampe dello spaccato negli indici 96-143 e 199-247
    static void imposta_palette(Palette& p);

    // Disegna nel rettangolo r della tela, prua a destra, con il profilo alla scala piu' grande che ci sta
    void disegna(Tela& t, Rettangolo r, const StatoSpaccato& s) const;
    // Sotto lo spaccato disegnato in r: per compartimento nome, uso e stato (acqua, fuoco, fumo, ossigeno, perso),
    // allineati ai compartimenti, nel rettangolo d
    void disegna_didascalie(Tela& t, const Font& f, Rettangolo r, Rettangolo d, const StatoSpaccato& s) const;
    // Dettaglio ingrandito: dalla chiglia alla base della vela, centrato fra x0 e x1 (m, coordinate del kit) e
    // allargato fino a riempire r (per esempio il compartimento scelto dal giocatore, estensione)
    void disegna_dettaglio(Tela& t, Rettangolo r, const StatoSpaccato& s, double x0_m, double x1_m) const;
    // Ascissa sulla tela di una x del kit, per lo spaccato disegnato in r
    int x_tela(Rettangolo r, double x_m) const;
    // Compartimenti nell'ordine di BattelloDanni::compartimenti() e loro estensione lungo lo scafo
    int compartimenti() const;
    bool estensione(int compartimento, double& x0, double& x1) const;

    const std::string& classe() const { return classe_; }

private:
    struct Dati;
    struct Vista;   // porzione del battello da disegnare
    void disegna_vista(Tela& t, Rettangolo r, const StatoSpaccato& s, const Vista& v) const;
    std::string classe_;
    std::unique_ptr<Dati> d_;
};

// Per il Museo (enciclopedia): i sistemi dello spaccato per un nome del descrittore dell'immagine. I 12 di Sistema per
// nome (calcolatore_sonar, schiera, ..., sonar_prua), piu' reattore -> propulsione, centrale -> periscopio e idroplani,
// direzione_tiro -> sonar_tiro, boa_radio e filo_galleggiante -> antenna. Vuoto se il nome non e' noto.
std::vector<Sistema> sistemi_spaccato(const std::string& nome);
// Lo spaccato intatto in r col contorno dei sistemi del nome in rosso, come un guasto nel quadro danni. tempo_s fa
// pulsare il contorno (0: un'immagine ferma, contorno acceso).
void disegna_spaccato_sistema(Tela& t, Rettangolo r, const Spaccato& s, const std::string& sistema, double tempo_s = 0.0);

}  // namespace m688
