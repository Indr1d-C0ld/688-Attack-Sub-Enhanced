#pragma once
// Replay completo (roadmap 16): la traccia di una partita dell'Enhanced (nucleo/traccia_replay.hpp) sulla carta del
// teatro, da scorrere senza risimulare.
//
// - Carta (fondale VGA di grafica/carta_vga, come l'editor) con due letture, V per alternarle:
//   - «verita'»: tutte le unita' vere coi simboli NTDS per parte, le scie degli ultimi 5 minuti, le armi in acqua col
//     filo verso chi le guida, gli alberi alzati (periscopio, antenna, schiera);
//   - «quadro del giocatore»: il battello, i contatti con la lettera e la posizione stimata (o la sola linea di
//     rilevamento), le linee della TMA e la soluzione proposta.
//   I e O ingrandiscono; trascinare sposta la carta, che smette di seguire il battello finche' non si clicca SEGUI.
// - Nel replay (non dal vivo) la variante C della passata estetica (roadmap 33, grafica/quadro): la consolle «SALA ANALISI - REPLAY» con la carta
//   grande in uno schermo a fosforo; sopra la carta la legenda (in basso a sinistra) e la lettura (in alto a destra: gli
//   ultimi eventi e la quota degli ultimi 10 minuti, col fondale e i termoclini); sotto la fascia del tempo e il piede
//   comune coi tasti ESC, V e INVIO (anche col clic).
// - In basso la timeline coi segni degli eventi: P gioca e mette in pausa, + e - (anche del tastierino) e Pag Su e Pag Giu'
//   (la compressione del tempo) cambiano la velocita' da 1x a 64x, le frecce sinistra e destra saltano di 30 s, Tab e Maiuscolo+Tab vanno all'evento successivo e
//   precedente, un clic sulla timeline salta li'.
// - Invio sceglie l'istante per «Riprendi da qui» (riprendi_da) ed esce; Esc esce senza.
//
// Modo dal vivo (sandbox, docs/19 §3): la stessa schermata su una traccia che cresce mentre la partita gira. Il chiamante
// le passa i fotogrammi registrati (aggiungi) e il tempo della partita (adesso); la schermata non fa avanzare il tempo:
// comandi_vivo restituisce le richieste dello spettatore (pausa, compressione, fine) e il chiamante le esegue. Al vivo
// le unita' proseguono a stima dall'ultimo campione; con un clic sulla timeline, le frecce o un clic sul registro si
// guarda indietro (la partita va avanti), Invio o ADESSO tornano al vivo. A destra, al posto della legenda e del profilo,
// la colonna del registro (gli eventi di tutte le unita', filtri per parte e per tipo; un clic porta la carta sul posto,
// con Maiuscolo anche il tempo) e la scheda dell'unita' scelta (Tab, Maiuscolo+Tab; S la segue).
//
// Nella lettura «verita'», con le tracce della versione 2: gli anelli dei ping, il ventaglio del radar, la radio, la
// posizione stimata con l'ellisse della navigazione al 95% e l'allerta accanto al nome. Dal vivo V gira anche le
// letture «parte» (il quadro della parte, record P: posizioni con l'ellisse o linee di rilevamento, eta' e sensore; C
// mette sotto in grigio la verita' degli altri) e «unita'» (i contatti dell'unita' scelta, record K, con la qualita').
//
// Testi in italiano. Il teatro arriva dal chiamante, altrimenti dai metadati della traccia («teatro») o dal file della
// missione nelle cartelle note; senza teatro la carta e' il solo mare.

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "grafica/carta_vga.hpp"
#include "grafica/font.hpp"
#include "grafica/palette.hpp"
#include "grafica/quadro.hpp"
#include "grafica/tela.hpp"
#include "nucleo/fondale.hpp"
#include "missioni/interventi.hpp"
#include "nucleo/traccia_replay.hpp"
#include "pannello_interventi.hpp"
#include "piattaforma/finestra.hpp"

namespace m688 {

class CampoGhiaccio;
struct GrigliaGhiaccio;
struct ProfiloSuono;

// Il teatro di una missione e i suoi termoclini (`ambiente = { termoclini_m = { ... } }`) dal suo file, nelle cartelle
// dove il gioco le cerca (originali, libreria, rapide, editor, importate); vuoto se non la trova
std::string teatro_dal_file(const std::string& dati, const std::string& missione, std::vector<double>& termoclini);
// Le tacche della linea del tempo: gli eventi col colore del registro e il loro interesse, sulla durata `fine`; `solo_noti`:
// quelli che il giocatore sapeva allora (la lettura QUADRO)
std::vector<quadro::TaccaTempo> tacche_eventi(const std::vector<EventoTraccia>& segni, double fine, bool solo_noti);
// Il colore di un intervento del vetrino di Dio: quello della sua famiglia (grafica/simboli_carta del kit)
std::uint8_t colore_intervento(const std::string& tipo);

// Quello che lo spettatore ha chiesto in un fotogramma del modo dal vivo: lo esegue il chiamante, che fa avanzare la partita
struct RichiesteVivo {
    bool pausa = false;        // P: alternare pausa e marcia
    int compressione = 0;      // +1 / -1: raddoppiare o dimezzare la compressione (+ e -, Pag Su e Pag Giu')
    bool fine = false;         // Esc o finestra chiusa: fine della partita
    // il vetrino di Dio (docs/28, D1): G apre il pannello; gli interventi confermati, da passare a Missione::intervieni
    // (tempo -1: subito); Backspace annulla l'ultimo «subito» (si rigioca dall'inizio senza)
    std::vector<m688::Intervento> interventi;
    bool annulla_ultimo = false;
};

class SchermataReplay {
public:
    // verita' e quadro del giocatore nel replay; dal vivo (sandbox) V gira verita', parte NATO, parte Patto, unita'
    enum class Lettura { verita, quadro, parte_nato, parte_patto, unita };

    // `dati`: data/motore (i teatri); `teatro` vuoto: dai metadati o dal file della missione
    SchermataReplay(std::string dati, Traccia traccia, std::string teatro = {});

    bool comandi(const Comandi& c, double dt);   // vero: si esce (Esc, finestra chiusa, Invio con l'istante scelto)
    void disegna(Tela& t, const Font& f) const;
    void palette(Palette& p) const;

    // l'istante scelto con Invio, per «Riprendi da qui» (nullopt: uscita senza scelta)
    std::optional<double> riprendi_da() const { return riprendi_da_; }

    // --- dal vivo (sandbox) ---
    void dal_vivo(bool si);
    bool vivo() const { return vivo_; }
    bool al_vivo() const { return vivo_ && !indietro_; }   // falso mentre lo spettatore guarda indietro
    // il clima del ghiaccio per la carta dello spettatore (missioni/griglia_ghiaccio.hpp): da file sta nei metadati della traccia,
    // dal vivo lo passa main quando il meteo c'e'. Vuoto o rovinato: niente pack
    void griglia_ghiaccio(const std::string& griglia);
    // il mese dell'acqua per il profilo del suono nella scheda (segni_ambiente.hpp): come la griglia. Fuori da 1-12, o senza il
    // clima del teatro, niente profilo
    void mese_acustica(int mese);
    // dopo ogni RegistratoreTraccia::aggiungi(f, n), lo stesso fotogramma (fotogramma_da con dopo_evento e RaccoltaTraccia)
    void aggiungi(const Fotogramma& f);
    // a ogni fotogramma dello schermo: il tempo della partita (s di missione) e il tempo come lo sta applicando il chiamante
    void adesso(double t, double compressione, bool pausa);
    RichiesteVivo comandi_vivo(const Comandi& c, double dt);
    // il registro filtrato: parte 0 tutte, 1 NATO, 2 Patto, 3 neutrali; tipo 0 tutti, 1 scoperte, 2 armi, 3 manovre,
    // 4 allerta e radio, 5 comandante
    void filtri(int parte, int tipo) { filtro_parte_ = parte; filtro_tipo_ = tipo; }
    std::size_t righe_registro() const;          // quante righe passano i filtri (fino all'istante mostrato)
    int scelta() const { return scelta_; }       // l'unita' scelta (-1 nessuna)
    bool pannello_aperto() const { return pannello_ && pannello_->aperto(); }
    const std::vector<RichiestaIntervento>& richieste() const { return richieste_; }   // quelle date dal pannello
    // un avviso allo spettatore sopra la carta per `durata_s` secondi (un intervento rifiutato e il suo motivo); il
    // tempo scorre coi dt di comandi_vivo e comandi
    void avviso(const std::string& testo, double durata_s = 4.0) {
        avviso_ = testo;
        avviso_resta_ = durata_s;
    }
    const std::string& avviso_attuale() const { static const std::string nessuno; return avviso_resta_ > 0.0 ? avviso_ : nessuno; }

    // --- per le istantanee e le prove ---
    void vai(double t);                          // salta all'istante (limitato alla durata)
    void vista(Vec2 centro, double metri_per_pixel) {   // la carta ferma su un punto e una scala (non segue piu' il battello)
        centro_ = centro;
        mpp_ = metri_per_pixel;
        segue_ = false;
    }
    double tempo() const { return t_; }
    Rettangolo carta() const;                    // l'area della carta (nel replay: quella dell'ultimo disegno)
    // solo la carta all'istante mostrato, nel rettangolo r (i replay affiancati del confronto A/B: app/replay_affiancati)
    void disegna_carta_in(Tela& t, const Font& f, Rettangolo r) const;
    Rettangolo linea_tempo() const;              // la barra della timeline, per i clic (idem)
    Rettangolo registro() const { return registro_c_; }   // dal vivo: lo schermo del registro, sotto la sua testata
    void lettura(Lettura l) { lettura_ = l; }
    Lettura lettura() const { return lettura_; }
    double velocita() const { return velocita_; }
    bool in_pausa() const { return pausa_; }
    const std::string& teatro() const { return teatro_; }
    const Traccia& traccia() const { return tr_; }

private:
    // una riga della colonna del registro: un evento del registro M1 o un fatto dei record I
    struct RigaRegistro {
        double t = 0.0;
        std::string testo;
        std::uint8_t colore = 0;
        int unita = -1, altra = -1;
        int gruppo = 0;                          // il tipo del filtro (1-5; 0 nessuno)
        int volte = 1;                           // lo stesso evento ripetuto di seguito (i ping subiti...): una riga sola
        std::optional<Vec2> dove;                // un punto proprio (il siluro), altrimenti l'unita' in quel momento
    };
    void aggiorna_registro();                    // le righe nuove dalla traccia
    bool passa(const RigaRegistro& r) const;
    IstanteTraccia istante_vista() const;        // l'istante mostrato, dal vivo con le unita' proseguite a stima
    std::string nome_unita(int id) const;
    int flag_unita(int id) const;
    std::string categoria_imitata(const std::string& classe) const;   // del falso contatto: sottomarino o nave
    void disegna_colonna(Tela& t, const Font& f, const IstanteTraccia& i) const;
    double fine_linea() const;                   // la fine della timeline: la durata, o adesso dal vivo
    void disegna_carta(Tela& t, const Font& f, const IstanteTraccia& i) const;
    void disegna_lettura(Tela& t, const Font& g, Rettangolo d) const;   // il riquadro della lettura (fosforo)
    void disegna_timeline(Tela& t, const Font& f) const;
    // dal vivo in variante C: la consolle, lo schermo della carta e quello del registro (aggiorna carta_c_, registro_c_ e
    // fascia_c_); disegnata anche su una tela di prova all'accensione del vivo, perche' i clic abbiano i rettangoli
    void cornice_vivo(Tela& t, const Font& f) const;
    GeometriaCarta geometria() const;
    int lato(const StatoUnita& u) const;         // 0 amico, 1 ostile, 3 neutrale (gli indici NTDS)
    int lato_flag(int flag) const;               // lo stesso da un flag dell'originale
    const StatoUnita* giocatore(const IstanteTraccia& i) const;
    void segui();                                // la carta torna sul battello
    double x_di(double t) const;                 // sulla timeline
    double t_di(double x) const;

    std::string dati_, teatro_;
    Traccia tr_;
    std::unique_ptr<Fondale> fondale_;
    std::vector<EventoTraccia> segni_;           // i segnalibri della timeline (tutti; il quadro mostra solo i noti)
    mutable FondaleVga vga_;
    const CampoGhiaccio* campo_ghiaccio() const;   // il campo dalla griglia e dai record X; nullptr senza griglia
    std::shared_ptr<const GrigliaGhiaccio> griglia_;
    std::shared_ptr<const ProfiloSuono> suono_base_;   // il profilo del mese; nullptr fuori dal vetrino
    mutable std::shared_ptr<const CampoGhiaccio> campo_;
    mutable std::size_t campo_record_ = 0;       // i record X con cui e' stato fatto
    std::optional<Font> targa_, fosforo_;        // incisione688_7 e fosforo688_5x7 (senza: il font del chiamante)
    mutable Rettangolo carta_c_{11, 44, 618, 358}, barra_{170, 415, 300, 7};   // dall'ultimo disegno del replay
    mutable Rettangolo registro_c_{467, 64, 162, 338}, fascia_c_{8, 412, 624, 13};   // dal vivo, idem
    mutable int scheda_y_ = 370;                 // dal vivo: la prima riga della scheda (il titolo: segui)
    double t_ = 0.0, velocita_ = 1.0;
    bool pausa_ = true, segue_ = true;
    Lettura lettura_ = Lettura::quadro;
    Vec2 centro_;
    double mpp_ = 120.0;                         // metri per pixel della carta
    std::optional<std::pair<int, int>> trascina_;   // dove e' iniziato il trascinamento sulla carta
    double salto_attesa_ = 0.0;                  // le frecce tenute: un salto ogni 0,2 s
    int salto_verso_ = 0;
    std::optional<double> riprendi_da_;
    int giocatore_id_ = 0;
    std::vector<double> termoclini_;            // m, dal file della missione (vuoto: nessuno noto)
    // dal vivo
    bool vivo_ = false, indietro_ = false;
    double adesso_ = 0.0;
    int scelta_ = -1;                            // l'unita' scelta con Tab
    bool confronto_ = false;                     // C: nella lettura di parte, la verita' degli altri in grigio
    int filtro_parte_ = 0, filtro_tipo_ = 0;
    std::vector<RigaRegistro> righe_;            // in ordine di tempo
    std::size_t registro_visti_ = 0, ia_visti_ = 0;
    struct Zona {
        Rettangolo r;
        std::size_t riga;
    };
    mutable std::vector<Zona> zone_righe_;       // dove sono le righe disegnate (per i clic)
    void apri_pannello();
    mutable std::optional<std::vector<std::string>> sottomarini_;   // le classi con la vela (classi.toml), alla prima richiesta
    std::optional<PannelloInterventi> pannello_;   // creato alla prima apertura (il catalogo e le classi)
    std::vector<RichiestaIntervento> richieste_;
    std::string avviso_;
    double avviso_resta_ = 0.0;
    void disegna_avviso(Tela& t, const Font& f) const;
};

}  // namespace m688
