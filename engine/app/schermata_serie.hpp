#pragma once
// La schermata delle serie della sandbox (docs/19 §6, V3): il riassunto di strumenti/serie_sandbox (serie.toml,
// app/serie_dati.hpp) in tre pagine, Tab o un clic sulle linguette:
// 1. esiti: una barra per esito con le partite;
// 2. per parte: ogni numero di numeri_parte (nucleo/statistiche_sandbox) con media +- dev, min-max, la fascia p10-p90 e la
//    mediana, NATO e Patto affiancati sulla stessa scala;
// 3. istogrammi: il numero scelto con le frecce su e giu', un istogramma per parte e sotto una barra per partita (in
//    ordine di seme); un clic su una barra sceglie quella partita: traccia_scelta() per aprirne il replay.
// I grafici sono del kit (grafica/grafici_serie), i colori delle parti quelli dei simboli della carta. Esc o Invio escono.
// Gruppo 3 della passata estetica (roadmap 33, grafica/quadro): la consolle «SALA ANALISI» con uno schermo a fosforo e
// le linguette; nella pagina 2 la barra statistica del kit (media +- scarto, baffi min-max), NATO in ciano e Patto in
// rosso sulla stessa scala, e in fondo la sintesi della pagina 1 (scelta dell'utente del 28/10/2026); il piede comune.

#include <filesystem>
#include <optional>
#include <utility>
#include <string>
#include <vector>

#include "grafica/font.hpp"
#include "grafica/palette.hpp"
#include "grafica/tela.hpp"
#include "piattaforma/finestra.hpp"
#include "serie_dati.hpp"

namespace m688 {

class SchermataSerie {
public:
    SchermataSerie(std::string dati, const std::filesystem::path& riassunto);   // serie.toml; std::runtime_error se non si legge
    SchermataSerie(std::string dati, DatiSerie serie, std::filesystem::path cartella);   // gia' letto (prove)

    bool comandi(const Comandi& c, double dt);   // vero: si esce (Esc, Invio, finestra chiusa, o una partita scelta)
    void disegna(Tela& t, const Font& f) const;
    void palette(Palette& p) const;
    // la partita cliccata nella pagina degli istogrammi: la sua traccia, per il replay
    std::optional<std::filesystem::path> traccia_scelta() const { return traccia_scelta_; }

    static constexpr int PAGINE = 3;
    void pagina(int k) { pagina_ = k; }          // per le istantanee e le prove (0-2)
    int pagina() const { return pagina_; }
    void numero(int k);                          // il numero degli istogrammi (indice in numeri())
    int numero() const { return numero_; }
    const std::vector<std::string>& numeri() const { return numeri_; }
    const DatiSerie& serie() const { return s_; }
    const std::vector<Rettangolo>& barre() const { return barre_; }   // le barre delle partite dell'ultimo disegno

private:
    std::vector<double> valori(int parte, const std::string& nome) const;   // per partita, nell'unita' mostrata; NaN i mancanti

    std::string dati_;
    DatiSerie s_;
    std::filesystem::path cartella_;
    std::vector<std::string> numeri_;            // quelli che hanno almeno un valore
    std::vector<int> parti_;                     // le parti presenti (NATO, Patto, neutrali)
    int pagina_ = 0, numero_ = 0, scorri_ = 0, prec_su_giu_ = 0;
    std::optional<std::filesystem::path> traccia_scelta_;
    mutable std::vector<Rettangolo> barre_;      // le barre delle partite disegnate (per i clic)
    mutable std::vector<Rettangolo> linguette_;  // le linguette disegnate (per i clic)
    mutable int righe_tabella_ = 20;             // le righe della pagina 2 che entrano nello schermo
    std::optional<Font> targa_, fosforo_;        // incisione688_7 e fosforo688_5x7 (senza: il font del chiamante)
};

}  // namespace m688
