#pragma once
// Il video d'apertura (docs/29_video_apertura.md): «Lo stesso mare», non un filmato ma una regia che il motore recita
// all'avvio. Due partite vere della stessa scena, una dal 688 e una dall'Alfa (data/motore/apertura), con gli ordini dati
// dalla regia (data/motore/apertura.toml) al posto del giocatore; ogni inquadratura sceglie il battello, la stazione e la
// compressione. L'inquadratura doppia affianca meta' tela di ciascuno. Gira nell'Enhanced e non scrive niente: fuori dalla
// carriera, dalle statistiche e dal rilievo del classico. Saltabile: main esce al primo tasto o clic.

#include <array>
#include <condition_variable>
#include <cstdint>
#include <deque>
#include <functional>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <vector>

#include "grafica/effetti.hpp"
#include "grafica/palette.hpp"
#include "grafica/periscopio.hpp"
#include "grafica/tela.hpp"
#include "missioni/missione.hpp"
#include "nucleo/mixer_audio.hpp"
#include "nucleo/mondo.hpp"
#include "nucleo/ordini.hpp"
#include "nucleo/sole.hpp"
#include "risorse.hpp"
#include "scena_prova.hpp"

namespace m688 {

class RegiaAudio;

struct InquadraturaApertura {
    double battute = 2.0;
    std::string battello;         // "688", "alfa", "doppia"; vuoto: nero
    int stazione = 8;             // 4 Manovra, 5 Periscopio, 6 Navigazione, 7 Armi, 8 Sonar
    double compressione = 0.0;    // s di missione per secondo di film; 0: la scena aspetta
    bool pannello = false;        // il pannello sotto la vista; falso: nero, restano solo gli strumenti
    int colonna_688 = 0, colonna_alfa = 0;   // doppia: la prima colonna della meta' di tela di ciascuno
    int guarda = -1;              // al periscopio: l'unita' da tenere nella vista
    std::vector<int> cifre;       // i pulsanti da premere entrando (la stazione li legge: 3 ingrandisce al periscopio, 8 AUTO al sonar)
    bool ping = false;            // nel nero: il ping lontano e il suo eco
    std::string effetto;          // un suono del gioco (effetti.toml) sul primo battere: il tubo che si allaga, il lancio
    double volume_effetto = 0.5;
    double cuffia = 0.3;          // il volume del mare in cuffia: e' la crescita del film, da un battello all'altro lo stesso ascolto
    double bordo = 1.0;           // il volume dei suoni di bordo della partita (il lancio vero, dopo l'effetto sul battere)
    std::array<int, 4> ritaglio{};   // x, y, l, a sulla tela: resta solo questo, al centro del nero; l 0: tutta la tela
    double scala = 1.0;           // il ritaglio ingrandito (pixel interi o no, il piu' vicino)
    double luce = 1.0;            // 1: la tela come viene; meno: piu' scura (il silenzio non abbaglia)
    double entra_s = 0.0;         // s di dissolvenza dal nero, sul primo battere; 0: il taglio netto
    double tramonto = 0.0;        // 0-1: il cielo e l'orizzonte del periscopio al sole basso, solo nella palette del film
    bool titolo = false;          // il titolo esce dal nero
};

struct OrdineApertura {
    bool alfa = false;            // a chi va
    Ordine ordine;                // ordine.tempo: l'istante di missione
};

struct RegiaApertura {
    double bpm = 72.0;            // alla semiminima, quattro per battuta
    double preriscaldo_s = 30.0;  // la scena prima della prima inquadratura: la cascata non parte vuota
    std::uint64_t seme = 1;       // delle due partite: fa il paesaggio sonoro attorno ai battelli
    std::string brano;            // in data/motore/musica/fm, senza .mid
    DataOra inizio;               // il tramonto dei periscopi
    double latitudine = 0.0, longitudine = 0.0;
    std::vector<InquadraturaApertura> inquadrature;
    std::vector<OrdineApertura> ordini;
    double durata_s() const;
};
RegiaApertura carica_regia_apertura(const std::string& percorso);   // lancia std::runtime_error

class VideoApertura {
public:
    // La scena e la palette da main (componi): il periscopio, il pannello, la luce del momento; `con_vista` falso: la stazione copre
    // il periscopio, che costa un terzo del fotogramma e non si disegna
    using Componi = std::function<Palette(Tela&, const Periscopio&, const Mondo&, const VistaPeriscopio&, const StatoPannello&,
                                          int id_proprio, double tempo_s, const Effetti*, bool con_vista)>;
    // Carica le due scene: lancia std::runtime_error se mancano
    VideoApertura(const Risorse& r, const std::string& dati, RegiaApertura regia, Componi componi);
    ~VideoApertura();
    void imposta_audio(RegiaAudio* a);   // nullptr: muto (le istantanee)

    // Il film avanza di `dt` s; le due partite gli vanno dietro per al piu' `limite_s` s di calcolo (0: quanto serve).
    // Se restano indietro il film non le aspetta: ogni fotogramma mostra dove sono arrivate. Falso a film finito
    bool avanza(double dt, double limite_s);
    Palette disegna(Tela& t);
    double tempo() const { return tempo_; }
    double tempo_missione(bool alfa) const;
    const Missione& missione(bool alfa) const;
    int inquadratura() const { return corrente_; }
    double ritardo_massimo() const { return ritardo_max_; }   // s di missione: quanto le partite sono rimaste dietro al film, dopo il nero
    const RegiaApertura& regia() const { return regia_; }

private:
    struct Battello;
    double durata_battute(double b) const { return b * 4.0 * 60.0 / regia_.bpm; }
    void entra_in_inquadratura();
    void comanda(Battello& b, double dt);
    Palette disegna_battello(Battello& b, Tela& t, const InquadraturaApertura& q);
    Palette disegna_strumenti(Tela& t, const InquadraturaApertura& q);

    const Risorse& r_;
    RegiaApertura regia_;
    Componi componi_;
    std::unique_ptr<Battello> usa_, urss_;
    RegiaAudio* audio_ = nullptr;
    double tempo_ = 0.0, obiettivo_ = 0.0, dt_ = 0.0;   // s di film; s di missione a cui devono arrivare le partite
    double inizio_inquadratura_ = 0.0;
    int corrente_ = -1;
    double ritardo_max_ = 0.0;
    Tela altra_;
    double prossima_carta_ = 0.0;   // la carta costa e si muove piano: si ridisegna otto volte al secondo
    Palette ultima_;
    std::optional<Font> font_titolo_;
    std::array<float, CANALI_AUDIO> volumi_prima_{};      // i volumi del mixer fuori dal film
};

// Il film preparato in anticipo: un filo mette fotogrammi e suono, il filo dell'audio consuma il suono, e il fotogramma da mostrare
// e' quello dei campioni gia' dati, cosi' immagine e suono non possono scollarsi. Se la coda si vuota si fermano insieme, col
// silenzio. Parte quando sono pronti `avvio` fotogrammi; chi prepara aspetta quando ce ne sono `massimo`
class CodaFilm {
public:
    struct Fotogramma {
        Tela tela;
        Palette palette;
        int inquadratura = 0;
        std::uint64_t numero = 0;
    };
    CodaFilm(std::size_t campioni_a_fotogramma, std::size_t massimo, std::size_t avvio);
    // chi prepara: falso se il film e' stato fermato
    bool metti(const Tela& t, const Palette& p, int inquadratura, const std::vector<float>& suono);
    void finita();   // non arrivera' altro
    // il filo dell'audio: n campioni, col silenzio che manca
    std::vector<float> campioni(std::size_t n);
    // chi mostra: l'ultimo fotogramma arrivato al campione `fino_a`, se e' nuovo. L'audio avanza a blocchi di 20 ms: chi mostra tiene
    // un orologio suo, agganciato a dati()
    std::optional<Fotogramma> prendi(std::uint64_t fino_a);
    std::optional<Fotogramma> prendi() { return prendi(dati()); }
    std::uint64_t dati() const;
    void ferma();
    bool partita() const;
    bool conclusa() const;   // finita e tutta consumata
    std::size_t anticipo() const;   // i fotogrammi gia' pronti
    std::uint64_t silenzio() const;   // i campioni di silenzio dati a film partito

private:
    mutable std::mutex mutex_;
    std::condition_variable spazio_;
    std::deque<Fotogramma> fotogrammi_;
    std::deque<float> suono_;
    std::size_t per_fotogramma_, massimo_, avvio_;
    std::uint64_t messi_ = 0, dati_ = 0, silenzio_ = 0;
    bool partita_ = false, finita_ = false, fermata_ = false;
};

// L'inquadratura doppia: la meta' sinistra di `a` e di `b`, ciascuna dalla sua colonna, con una palette sola. Gli indici
// che le due usano con colori diversi si spostano su indici liberi (funzione pura, per le prove)
Palette affianca(Tela& uscita, const Tela& a, const Palette& pal_a, int colonna_a, const Tela& b, const Palette& pal_b, int colonna_b);

}  // namespace m688
