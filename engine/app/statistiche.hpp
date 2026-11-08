#pragma once
// Le schermate delle statistiche (roadmap punto 6; nucleo/statistiche.hpp e la nota condivisa statistiche-proposta.md):
//
// - SchermataStatistiche: il report di una missione, ricalcolato dalla traccia del replay. A sinistra la quota nel tempo
//   (le fasce di superficie e periscopica, i termoclini), il livello irradiato coi segni dei ping e delle trasmissioni,
//   le armi per classe e per bersaglio; a destra la heatmap sulla carta del teatro (il fondale del replay), la prima
//   scoperta nemica e i tempi. Tab passa alla pagina «IMPRESE» (ghiaccio, notte, coda, mine, ASW aereo, soccorso,
//   riconoscimento, armi e navigazione: docs/26 §3 e il secondo giro del 31/10/2026), nella variante C.
// - SchermataStatisticheCumulate: le cumulate globali o di un comandante. Totali, medie per missione, rilevamenti,
//   primati, armi; a destra la heatmap di un teatro alla volta (Tab, o un clic sulla carta, per cambiarlo). Gruppo 4
//   della passata estetica (variante C): la consolle «SALA ANALISI», due schermi a fosforo; senza missioni i trattini
//   spenti al posto degli zeri e lo stato vuoto sulla carta (scelta dell'utente del 29/10/2026). 1 e 2 scelgono la
//   pagina: la seconda e' «IMPRESE», come nel report di una missione, coi totali e i primati.
//
// - SchermataStatisticheSandbox: le statistiche tecniche di una partita sandbox (docs/19 §4; nucleo/statistiche_sandbox),
//   in cinque pagine (Tab, o un clic sulle linguette): riepilogo per parte, catene di scoperta, armi, reazioni e TMA,
//   esposizione e navigazione. Le frecce su e giu' scorrono le tabelle; un clic su una riga sceglie l'istante per aprire
//   il replay li' (replay_da) ed esce. Variante C della passata estetica (roadmap 33, grafica/quadro): la consolle
//   «SALA ANALISI», schermi a fosforo (nel riepilogo due: la tabella e i numeri in figura), il piede comune coi tasti
//   TAB e ESC (anche col clic).
//
// Si esce con Esc o Invio. Testi in italiano (senza percentuali: il font del gioco non ha il segno), lo stile del
// debriefing e del replay; la palette e' quella della carta VGA piu' la rampa del calore (232-247).

#include <map>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "grafica/carta_vga.hpp"
#include "grafica/font.hpp"
#include "grafica/palette.hpp"
#include "grafica/tela.hpp"
#include "nucleo/fondale.hpp"
#include "nucleo/statistiche.hpp"
#include "nucleo/statistiche_sandbox.hpp"
#include "nucleo/traccia_replay.hpp"
#include "piattaforma/finestra.hpp"

namespace m688 {

class SchermataStatistiche {
public:
    // `dati`: data/motore (i teatri); il teatro dal report, altrimenti dai metadati della traccia o dal file della missione
    SchermataStatistiche(std::string dati, ReportMissione report, Traccia traccia);

    bool comandi(const Comandi& c, double dt);   // vero: si esce (Esc, Invio, finestra chiusa)
    void disegna(Tela& t, const Font& f) const;
    void palette(Palette& p) const;

    const ReportMissione& report() const { return r_; }
    const std::string& teatro() const { return teatro_; }
    void scegli_pagina(int p) { pagina_ = p; }    // 0 il report, 1 le imprese (per le istantanee e le prove)

private:
    void disegna_quota(Tela& t, const Font& f, Rettangolo r) const;
    void disegna_livello(Tela& t, const Font& f, Rettangolo r) const;
    void disegna_armi(Tela& t, const Font& f, Rettangolo r) const;
    void disegna_scoperta(Tela& t, const Font& f, Rettangolo r) const;
    void disegna_tempi(Tela& t, const Font& f, Rettangolo r) const;

    std::string dati_, teatro_;
    ReportMissione r_;
    Traccia tr_;
    int giocatore_id_ = 0;
    std::vector<std::pair<double, double>> quote_;   // (s, m) del battello
    std::vector<double> ping_, trasmissioni_;        // istanti
    std::vector<double> termoclini_;
    std::unique_ptr<Fondale> fondale_;
    mutable FondaleVga vga_;
    int pagina_ = 0;
    std::optional<Font> targa_, fosforo_;   // la pagina delle imprese (variante C)
};

class SchermataStatisticheSandbox {
public:
    SchermataStatisticheSandbox(std::string dati, const Traccia& traccia);   // calcola statistiche_sandbox(traccia)

    bool comandi(const Comandi& c, double dt);   // vero: si esce (Esc, Invio, finestra chiusa, una riga cliccata)
    void disegna(Tela& t, const Font& f) const;
    void palette(Palette& p) const;
    std::optional<double> replay_da() const { return replay_da_; }   // la riga cliccata: il replay da quell'istante

    static constexpr int PAGINE = 5;
    void pagina(int k) { pagina_ = k; scorri_ = 0; }   // per le istantanee e le prove (0-4)
    int pagina() const { return pagina_; }
    const StatisticheSandbox& statistiche() const { return s_; }
    std::size_t righe() const;                        // le righe di dati della pagina mostrata
    // le righe disegnate con un istante (dall'ultimo disegno), per le prove
    const std::vector<std::pair<Rettangolo, double>>& zone() const { return zone_; }

private:
    struct Riga {
        std::vector<std::string> celle;
        std::uint8_t colore = 0;
        double t = -1.0;                              // l'istante per il replay (-1 nessuno)
        bool titolo = false;                          // l'intestazione di una tabella (o una riga di testo)
    };
    void prepara(const Traccia& traccia);
    std::string nome(int id) const;
    void disegna_figura(Tela& t, const Font& g, Rettangolo b) const;   // il riepilogo in figura (pagina 1, a destra)

    std::string dati_;
    StatisticheSandbox s_;
    std::string missione_;
    std::map<int, std::string> nomi_;
    std::vector<std::vector<Riga>> pagine_;
    std::vector<std::vector<int>> colonne_;           // per pagina: l'ascissa di ogni colonna
    int pagina_ = 0, scorri_ = 0;
    std::optional<double> replay_da_;
    mutable std::vector<std::pair<Rettangolo, double>> zone_;   // le righe disegnate con un istante
    mutable std::vector<Rettangolo> linguette_;       // le linguette disegnate, per i clic
    mutable int visibili_ = 30;                       // le righe che entrano nello schermo
    std::optional<Font> targa_, fosforo_;             // incisione688_7 e fosforo688_5x7 (senza: il font del chiamante)
};

class SchermataStatisticheCumulate {
public:
    // `chi`: «GLOBALI» o il nome del comandante, per la testata
    SchermataStatisticheCumulate(std::string dati, StatisticheCumulate statistiche, std::string chi);

    bool comandi(const Comandi& c, double dt);   // vero: si esce
    void disegna(Tela& t, const Font& f) const;
    void palette(Palette& p) const;

    const std::string& teatro() const;            // quello della heatmap mostrata (vuoto: nessuna heatmap)
    void scegli_teatro(int k);                     // per le istantanee e le prove
    void scegli_pagina(int p) { pagina_ = p; }    // 0 le statistiche, 1 le imprese

private:
    void carica_fondale();

    std::string dati_, chi_;
    StatisticheCumulate s_;
    std::vector<std::string> teatri_;              // quelli con una heatmap
    int scelto_ = 0;
    std::optional<Font> targa_, fosforo_;          // gruppo 4 (variante C): incisione688_7 e fosforo688_5x7
    mutable Rettangolo carta_{319, 46, 306, 230};  // la carta dell'ultimo disegno, per il clic
    std::unique_ptr<Fondale> fondale_;
    mutable FondaleVga vga_;
    int pagina_ = 0;
};

}  // namespace m688
