#pragma once
// Interfaccia del giocatore in missione (docs/11_comandi_e_stazioni.md, fasi C2-C4): traduce i tasti in ordini per
// il registro (posto di comando, stazioni Armi e Sonar, carta), tiene le scelte che fermano il tempo come
// nell'originale (TARGET, punto di rotta, conferma del taglio della schiera) e prepara pannelli, barra del
// bersaglio, carta e segni al periscopio.
//
// Tutto cio' che cambia la simulazione passa per gli ordini: un replay non ha bisogno dell'interfaccia.

#include <array>
#include <deque>
#include <functional>
#include <map>
#include <memory>
#include <optional>
#include <random>
#include <string>
#include <utility>
#include <vector>

#include "grafica/font.hpp"
#include "grafica/libretto.hpp"
#include "nucleo/luci_navigazione.hpp"
#include "grafica/modello.hpp"
#include "grafica/periscopio.hpp"
#include "grafica/spaccato.hpp"
#include "nucleo/mappa_tasti.hpp"
#include "schermata_comandi.hpp"
#include "grafica/tela.hpp"
#include "missioni/contatti_giocatore.hpp"
#include "missioni/missione.hpp"
#include "missioni/stazioni_missione.hpp"
#include "nucleo/bordo_audio.hpp"
#include "nucleo/carta_giocatore.hpp"
#include "nucleo/classi.hpp"
#include "nucleo/comando.hpp"
#include "nucleo/fari.hpp"
#include "nucleo/mixer_audio.hpp"
#include "nucleo/suoni_classici.hpp"
#include "nucleo/ordini.hpp"
#include "nucleo/tma.hpp"
#include "nucleo/traccia_replay.hpp"
#include "piattaforma/finestra.hpp"
#include "scena_prova.hpp"
#include "sonar_enhanced.hpp"
#include "pagina_intercettazione.hpp"
#include "ripetitori.hpp"
#include "vista_fondale.hpp"
#include "stazione_missili.hpp"
#include "stazione_radio.hpp"

namespace m688 {

inline constexpr int MSG_CONFERMA_TAGLIO = 213;
inline constexpr int MSG_IMMERSIONE_EMERGENZA = 430;   // collisioni (P4): l'ordine dato; 431-434 li compone Partita
inline constexpr int MSG_FONDO_SULLA_ROTTA = 378, MSG_ZONA_SULLA_ROTTA = 379;   // allarmi della carta (modo moderno)   // domanda del telegrafo con la schiera fuori (messaggi del motore)

// stato del mare per la vista dei periscopi finche' non c'e' il meteo (la missione usa 3 fisso)
inline constexpr int STATO_MARE_PERISCOPI = 3;

class DanniMissione;

// Il nome del periscopio sul pulsante 1 di F5, senza il ruolo («Type 18 (ricerca)»: TYPE 18, il ruolo e' nella testata): in maiuscolo,
// il cirillico traslitterato e senza virgolette, come OKEAN e SARGAN («Сигнал»: SIGNAL)
std::string nome_periscopio_pulsante(const std::string& nome);

class InterfacciaMissione {
public:
    InterfacciaMissione(Missione& m, StazioniMissione& st, PostoDiComando& posto, RegistroOrdini& ordini,
                        std::vector<Messaggio>& equipaggio, const std::map<std::string, ClasseNave>& classi, int stazione);
    void imposta_quota_collaudo(double m) { quota_collaudo_ = m; }
    // Fase 3 (Enhanced): i pannelli delle consolle del kit per Manovra e Armi, coi font del battello del giocatore in
    // <dati>/font; senza i font restano i pannelli di oggi
    void imposta_consolle(const std::string& dati);
    // Le aree cliccabili della vista dell'ultimo fotogramma (Centrale delle consolle: i posti aprono le stazioni)
    const std::vector<AreaAttiva>& aree_vista() const { return aree_vista_; }
    DatiApp dati_app() const;   // per la traccia del replay: sonar, stazione, silenzio, bersaglio, soluzione proposta   // del battello del giocatore (scafi/<classe>.toml)
    bool silenzio() const;      // il silenzio a bordo (stazione armi), per i transitori dello scafo della missione
    // Altezza dell'albero sul galleggiamento per classe (dai modelli 3D), per lo stadimetro
    void imposta_alberi(std::map<std::string, double> m) { alberi_ = std::move(m); }
    // Il libretto di riconoscimento al periscopio (docs/25; grafica/libretto del kit): le tavole di <dati>/libretto.toml,
    // le sagome dai modelli 3D della classe (`modelli` resta di main) con la rampa dei grigi del giorno. Senza, Ctrl+Z tace
    void imposta_libretto(const std::string& dati, const std::map<std::string, Modello>* modelli, const std::array<double, 15>& rampa);
    // Le luci di navigazione al periscopio (docs/27, nucleo/luci_navigazione del kit): <dati>/luci.toml. Senza, niente luci
    void imposta_luci(const std::string& dati);
    // Lo spaccato del battello del giocatore per il quadro danni dell'Enhanced (F3)
    void imposta_spaccato(std::unique_ptr<Spaccato> s) { spaccato_ = std::move(s); }
    // il catalogo del carico (Enhanced, M2): sagome in scala delle armi nei tubi e nello spaccato
    void imposta_catalogo_carico(std::function<const CatalogoCarico*()> c) { catalogo_carico_ = std::move(c); }
    // audio (R1-R3): il mixer di main. A ogni fotogramma `audio` riempie, col tempo vero, la cuffia del sonar alla stazione
    // Sonar e l'ambiente di bordo della stazione in cui si e' (sonar, locale siluri, centrale), col regime del battello;
    // in pausa niente, oltre ×8 di compressione si genera ma non si suona (resta continuo al ritorno)
    void imposta_mixer(Mixer* m) { mixer_ = m; }
    // Con il filo dell'audio: cuffia e bordo tengono `s` secondi in coda. 0: si generano per il tempo vero (senza dispositivo, le prove)
    void imposta_anticipo(double s) { anticipo_s_ = s; }
    // Le navi colpite (la fisica dei danni): al bordo gli scoppi dei depositi, coi loro kg, e la rottura dello scafo (T14)
    void imposta_navi(const DanniMissione* d) { navi_ = d; }
    void audio(double dt_reale, double compressione);

    // I comandi di un fotogramma (dt reale in secondi). Vero se il giocatore vuole uscire.
    bool comandi(const Comandi& c, VistaPeriscopio& vista, double dt);
    // Una scelta in corso ferma il tempo (salvo via modem, che qui non c'e')
    // Tempo fermo: una scelta in corso, la pausa (P), o la Centrale nel livello classico (come il CONN dell'originale)
    bool in_pausa() const { return scelta_ != Scelta::nessuna || pausa_ || (stazione_ == 9 && !enhanced_) || schermata_aperta_; }
    // La schermata dei comandi (F11, app/schermata_comandi): la cartella dei dati, la mappa dei tasti vera e, in finestra, il suggerimento dei primi
    // 20 s dell'Enhanced. Senza imposta_comandi F11 non fa niente. comandi_schermata: vero se il comando e' suo; main svuota allora i Comandi, cosi'
    // Esc non esce dal gioco e le frecce non girano il periscopio
    void imposta_comandi(const std::string& dati, const MappaTasti& tasti, bool suggerimento);
    bool comandi_schermata(const Comandi& c);
    bool schermata_comandi_aperta() const { return schermata_aperta_; }
    const std::vector<AreaComando>& aree_comandi() const { return aree_comandi_; }
    int scheda_comandi() const { return scheda_comandi_; }
    // regola di simulazione: la Centrale non ferma il tempo, e la carta ha la grafica VGA/SVGA (N1)
    void imposta_enhanced(bool si) { enhanced_ = si; carta_.restyle = si; }
    int stazione() const { return stazione_; }
    CartaGiocatore& carta() { return carta_; }   // stato iniziale della carta (opzioni da riga di comando)
    void mostra_mate(bool si) { tma_vista_ = si; }

    void pannello(StatoPannello& p) const;
    // La carta al posto della vista nella stazione Navigazione; altrove i segni dei contatti sopra il periscopio
    void disegna(Tela& t, const Font& f, const VistaPeriscopio& v) const;
    // Pannello Manovra dell'Enhanced: posizioni del posto e letture della dinamica (vuoto senza dinamica)
    void manovra_enhanced(StatoManovra& s) const;
    // Un ordine consegnato (anche da un replay): il posto dell'Enhanced segue le posizioni
    void osserva(const Ordine& o);
    // Dopo ogni passo della missione: storie della stazione Sonar dell'Enhanced
    void dopo_passo();
    // «Riprendi da qui» (debriefing): mentre la partita si rigioca fino all'istante scelto, la cascata e i tracker della
    // stazione Sonar (solo interfaccia, il costo maggiore) restano fermi; alla fine l'audio di bordo riparte da adesso,
    // senza gli scoppi e i tuoni gia' passati
    void ricostruzione(bool si);
    // La palette del fotogramma: la scala dei colori delle cascate quando la stazione Sonar dell'Enhanced copre la vista
    void palette(Palette& p) const;
    SonarEnhanced& sonar() { return sonar_enh_; }
    // il LOFAR colorato per rilevamento (RIL) e' stato sulla stazione Sonar almeno una volta: un aiuto del realismo.
    // Acceso di serie nel moderno, conta solo se il giocatore l'ha davvero guardato
    bool ril_visto() const { return ril_visto_; }
    // la scheda del filtro del libretto e' aperta: le frecce sono sue, non ruotano ne' alzano il periscopio
    bool frecce_al_libretto() const { return stazione_ == 5 && libretto_aperto_ && scheda_filtro_; }
    // La carta sta scrivendo una nota: la finestra passa i caratteri (Comandi::testo)
    bool vuole_testo() const { return stazione_ == 6 && carta_.segni.scrivendo; }
    // la pagina di tiro dei missili copre il pannello Armi: le sue aree attive non valgono (docs/21 §3)
    bool copre_pannello() const { return stazione_ == 7 && pagina_missili_; }
    // La stazione copre tutta la vista: il periscopio sotto non si vede, e costa la maggior parte del fotogramma. Solo disegno: niente
    // della partita lo legge
    bool copre_la_vista() const;
    void messaggio_barra(std::string m) { messaggio_barra_ = std::move(m); }   // dal pannello: per le pagine che lo coprono
    // Stazione Radio del classico: l'archivio dei messaggi di bordo gia' composti (da main, che ha i testi)
    void storico(std::vector<RigaStorico> s) { storico_ = std::move(s); }
    // Il registro dei messaggi (punto 21, Enhanced): Ctrl+L lo apre sopra la vista da ogni stazione, Esc lo chiude
    bool registro_aperto() const { return registro_aperto_; }
    void registro(std::vector<std::pair<double, std::string>> r) { registro_ = std::move(r); }
    // Enhanced, dopo una scossa (D6d): quanto scurire la palette in questo fotogramma (luci che tremolano), 0-1
    double tremolio() const;
    // il tempo del disegno, interpolato fra due passi della missione (1/9 s): tremolio e scossa scattano a ogni
    // fotogramma; negativo, il tempo della missione (istantanee senza finestra)
    void imposta_tempo_disegno(double t) { tempo_disegno_ = t; }

private:
    enum class Scelta { nessuna, bersaglio, punto, conferma_taglio, direct };   // direct: il punto per il siluro (C4)
    void ordina(const Ordine& o) { ordini_.ordina(o); }
    bool comandi_stazioni(const Comandi& c, VistaPeriscopio& vista, double dt);
    void messaggio(int codice);
    void azione_posto(Azione a);
    void comando_alfa(const std::string& k);   // i pulti dell'Alfa: Boksit, Tan, ПОКС, Akkord
    bool ha_schiera() const;                   // Enhanced: la classe del giocatore ha l'antenna rimorchiata
    bool tan_ = false;                         // Tan inserito (l'ultimo ordine dato)
    bool registro_aperto_ = false;
    std::vector<std::pair<double, std::string>> registro_;
    void disegna_registro(Tela& t, const Font& f) const;
    void cifra(int n);
    void inizia_bersaglio(const VistaPeriscopio& v);
    void chiudi_bersaglio(bool annulla);
    std::vector<SegnoContatto> segni() const;
    std::string nome_classe(const std::string& classe) const;
    void disegna_tma(Tela& t, const Font& f) const;
    PostoEnhanced* posto_enhanced();   // creato alla prima occasione con la dinamica del giocatore
    void disegna_danni(Tela& t, const Font& f) const;   // quadro danni dell'Enhanced al posto della vista (F3)
    double scossa_attuale() const;                       // fattore d'urto che si spegne in 1,5 s
    double rollio_attuale() const;                       // gradi: il rollio dopo un urto del battello, smorzato

    Missione& m_;
    StazioniMissione& st_;
    PostoDiComando& posto_;
    RegistroOrdini& ordini_;
    std::vector<Messaggio>& equipaggio_;
    const std::map<std::string, ClasseNave>& classi_;
    int stazione_;
    CartaGiocatore carta_;
    Scelta scelta_ = Scelta::nessuna;
    std::vector<std::pair<char, int>> candidati_;   // TARGET: zone nell'ordine della lista (lettera, o 0 per le X)
    std::vector<Vec2> posizioni_;                  // e la loro posizione sulla tela (per il mouse)
    int scelto_ = -1, zona_ = -1;
    Vec2 cursore_;                                 // SET: punto in metri
    double tenuto_ = 0.0;                          // s con le frecce premute (il cursore accelera)
    std::optional<Ordine> telegrafo_sospeso_;      // in attesa della conferma del taglio
    int telegrafo_prima_ = 1;
    int deselezionato_ = -1;                       // bersaglio sparito gia' deselezionato
    bool tma_vista_ = false;                       // alla carta, lo strumento MATE (T)
    double prossima_stima_tma_ = 0.0;              // quando il MATE riprova a dare la rotta stimata alla missione
    double tempo_disegno_ = -1.0;                  // imposta_tempo_disegno
    bool menu_punto_nave_ = false;                 // navigazione realistica: il menu dei punti nave alla carta (Ctrl+O)
    void disegna_menu_punto_nave(Tela& t, const Font& f) const;
    SoluzioneTma soluzione_{9260.0, 0.0, 2.5};     // proposta del giocatore: 5 nm, rotta 000, 5 nodi
    double quota_collaudo_ = 0.0;
    std::unique_ptr<FontConsolle> consolle_;         // Fase 3: i font delle consolle (nullptr: pannelli di oggi)
    mutable std::vector<AreaAttiva> aree_vista_;
    bool alfa() const { return m_.giocatore() && m_.giocatore()->classe == "alfa"; }
    bool pausa_ = false, enhanced_ = false;
    std::map<std::string, double> alberi_;
    struct Stadimetro {
        int bersaglio = -1;
        double distanza_m = 0.0, altezza_m = 0.0;   // misura e altezza d'albero usata
        bool classe_nota = false;
        std::string punto = "ALBERO";
    };
    std::optional<Stadimetro> stadimetro_;
    std::mt19937 lettura_{1989};                    // errore di lettura dello stadimetro
    void misura_stadimetro(const VistaPeriscopio& v);
    // il libretto alla stazione Periscopio (Enhanced coi periscopi veri, docs/25 §3): Ctrl+Z lo apre nel terzo destro,
    // Tab e Maiuscolo+Tab sfogliano, Invio identifica il bersaglio con la tavola (l'altezza d'albero va al quadrante),
    // [ ] l'altezza a mano (1 m, Maiuscolo 5), Z X l'angolo sulla prora a passi di 10 gradi, Spazio lo manda alla TMA
    std::vector<libretto::Voce> libretto_;
    struct SagomeTavola { Sprite profilo, prua, tre_quarti, poppa; };
    mutable std::map<std::string, SagomeTavola> sagome_libretto_;   // per classe, alla prima apertura della tavola
    const std::map<std::string, Modello>* modelli_ = nullptr;
    std::array<double, 15> rampa_{};
    std::optional<Font> piccolo_;                  // fosforo688_5x7: il testo della pagina
    std::map<std::string, luci::Configurazione> luci_;
    std::vector<Faro> fari_;   // i fari del teatro della missione (nucleo/fari, kit): con le luci, da <dati>/fari.toml
    mutable bool ril_visto_ = false;               // ril_visto()   // le configurazioni dei fanali per classe (imposta_luci)
    bool libretto_aperto_ = false;
    int tavola_ = 0, aob_ = 0;                     // la tavola aperta (indice in libretto_); l'AOB impostato, gradi (+ dritta)
    // il filtro delle tavole (grafica/libretto del kit): F apre la scheda, le frecce scelgono la riga e il valore, Invio o
    // Spazio il valore successivo, Backspace TUTTE; chiusa la scheda, Tab sfoglia solo le tavole che restano
    libretto::Filtro filtro_;
    bool scheda_filtro_ = false;
    int riga_filtro_ = 0, prec_su_giu_lib_ = 0, prec_ruota_lib_ = 0;
    std::vector<int> tavole_filtrate() const;      // indici in libretto_ (tutte senza filtro)
    struct Quadrante {                             // dal libretto o a mano
        double altezza_m = 30.0;
        bool tavola = false;
        std::string punto = "ALBERO";              // a cosa si mira: la testa d'albero o il riferimento della tavola
    };
    std::map<int, Quadrante> quadrante_;           // per bersaglio: l'altezza d'albero impostata sul telemetro
    Quadrante quadrante(int bersaglio) const;      // con l'aiuto della difficolta' facile e i limiti del periscopio
    const libretto::Voce* voce_libretto(const std::string& classe) const;
    // l'altezza della tavola sul quadrante: l'albero, o il secondo riferimento se l'albero supera il periscopio
    Quadrante quadrante_tavola(const libretto::Voce& v) const;
    bool libretto_possibile() const { return enhanced_ && !libretto_.empty() && m_.con_periscopi() && m_.posto_periscopi(); }
    void comandi_libretto(const Comandi& c, double t);
    void disegna_libretto(Tela& t, const Font& f) const;
    int campo_siluro_ = 0;                         // Armi dell'Enhanced: campo delle impostazioni scelto con Tab
    std::optional<PostoEnhanced> posto_enh_;
    SonarEnhanced sonar_enh_;                      // stazione Sonar completa (S3)
    // la pagina di tiro dei missili della stazione Armi (docs/21 §3-4; app/stazione_missili di trailer-1): si apre da
    // sola quando il tubo scelto ha un missile del catalogo, Tab torna ai siluri
    StazioneMissili missili_;
    bool pagina_missili_ = false;
    int tubo_missili_ = 0, prec_su_giu_missili_ = 0, prec_ruota_missili_ = 0;
    std::string testo_missili_;                    // il rifiuto del lancio (coperchio chiuso), per qualche secondo
    double testo_missili_fino_ = -1.0;
    mutable std::vector<AreaAttiva> aree_missili_;
    std::string messaggio_barra_;
    // la pagina dell'intercettazione del sonar (docs/22, E; app/pagina_intercettazione di trailer-1): Ctrl+W da ogni
    // stazione; l'allarme siluro si vede e si sente in tutte le stazioni
    PaginaIntercettazione intercetti_;
    bool pagina_intercetti_ = false;
    // il quadro del fondale (docs/24; app/vista_fondale): Ctrl+H apre la pagina alla Navigazione da ogni stazione
    // (ecoscandaglio, sezione di prua, prospettiva dei ritorni; 1 ping, 2 periodo, 3 potenza, 4 misure sulla carta)
    bool pagina_fondale_ = false, misure_fondale_ = false;
    void disegna_pagina_fondale(Tela& t, const Font& f) const;
    int prec_su_giu_int_ = 0, prec_ruota_int_ = 0;
    int prec_su_giu_demon_ = 0, prec_ruota_demon_ = 0;   // i cursori armonici del DEMON, un passo per pressione
    Allarme ultimo_allarme_ = Allarme::nessuno;
    StileIntercettazione stile_intercetti() const;
    void suona_allarme(Allarme a);
    std::array<double, 5> soluzione_missili_{};   // l'ultima soluzione del MATE mandata ai tubi coi missili
    double prossima_soluzione_missili_ = 0.0;
    ContestoMissili contesto_missili() const;
    bool tubo_con_missile(int tubo) const { return m_.con_missili() && m_.tiro_missile(tubo).has_value(); }
    StazioneRadio radio_;                          // stazione Radio (S4)
    // gruppo 5 (roadmap 33; bozze approvate il 29/10/2026, dell'Alfa la sera stessa), solo nell'Enhanced con le
    // consolle: la sala radio sulla lamiera, i ripetitori di Manovra e Armi al posto della vista col periscopio giu', il
    // pannello della carta. Sull'Alfa il tema beige, lo stampino e le scritte scure; gli schermi col fosforo 5x7
    bool stazioni_lamiera() const { return enhanced_ && consolle_ != nullptr; }
    bool ripetitore() const;                       // stazione 4 o 7 col periscopio giu'
    lamiera::Scritte scritte() const { return lamiera::scritte_per(alfa() ? BattelloTema::alfa : BattelloTema::usa688); }
    ripetitori::Fonti fonti_ripetitori(const Font& f) const {
        return {f, consolle_->targa, alfa() && schermo_ripetitori_ ? *schermo_ripetitori_ : consolle_->schermo, scritte()};
    }
    std::optional<Font> schermo_ripetitori_;       // fosforo688_5x7 per l'Alfa (le sue consolle hanno lo stampino)
    void disegna_ripetitore_manovra(Tela& t, const Font& f) const;
    BattelloVolta battello_volta() const;          // per le viste della volta del ghiaccio (app/vista_fondale)
    void disegna_ripetitore_armi(Tela& t, const Font& f) const;
    // la storia della quota per il profilo del ripetitore: la chiglia ogni 10 s di missione, gli ultimi 10 minuti
    // (61 campioni), e la quota dello strato (SLD del profilo del suono) letta con l'ultimo campione
    static constexpr double PASSO_QUOTA_S = 10.0;
    static constexpr std::size_t CAMPIONI_QUOTA = 61;
    std::deque<std::pair<double, double>> storia_quota_;   // (s di missione, m)
    double prossimo_campione_quota_ = 0.0, strato_m_ = 0.0;
    double cursore_sonar_ = 0.0;                   // frazione di riga del cursore del LOFAR
    double voce_sonar_ = 0.0;                      // frazione di voce della libreria (frecce tenute)
    struct UltimoBersaglio {                       // per il datum della carta quando il contatto si perde
        int id = -1;
        Vec2 pos;
        double tempo = 0.0;
        std::string fonte;
    };
    std::optional<UltimoBersaglio> ultimo_bersaglio_;
    double prossimo_allarme_carta_ = 0.0;          // allarmi del modo moderno: fondo e zone sulla rotta
    std::size_t asw_sulla_carta_ = 0;
    std::size_t impatti_art_visti_ = 0;            // artiglieria (docs/16, G6): impatti gia' segnati sulla carta              // avvisi ASW gia' riportati sul lucido come linee di rilevamento
    std::function<const CatalogoCarico*()> catalogo_carico_;
    Mixer* mixer_ = nullptr;
    // i suoni del classico come nel 1989 (nucleo/suoni_classici): solo senza l'acustica dell'Enhanced
    const DatiSuoniClassici* dati_classici_ = nullptr;
    SuoniClassici suoni_classici_;
    std::size_t msg_classici_visti_ = 0, testi_classici_visti_ = 0, colpi_classici_visti_ = 0;
    mutable double ril_vista_ = 0.0, semicampo_vista_ = 0.0;   // da prepara_vista: le unita' nel campo del periscopio
    void audio_classico();
    // ambiente sonoro di bordo (R3): la scena riletta ogni 0,25 s di tempo vero, i colpi gia' sentiti
    AmbienteBordo bordo_;
    std::optional<Missione::SceneAcustica> scena_bordo_;
    double scena_bordo_s_ = 0.0;
    std::optional<Sorgente> proprio_bordo_;          // il battello stesso per il bordo (R3b), riletto con la scena
    std::size_t colpi_sentiti_ = 0, art_sentiti_ = 0;   // scoppi ed eventi d'artiglieria gia' passati al bordo
    std::size_t urti_sentiti_ = 0, urti_visti_ = 0;      // urti (Missione::urti) gia' passati al bordo e alla vista
    const DanniMissione* navi_ = nullptr;
    std::map<int, std::pair<std::size_t, bool>> navi_sentite_;   // per nave: scoppi gia' passati al bordo, rottura gia' sentita
    double rollio_ampiezza_ = 0.0, rollio_da_ = -1.0;    // l'ultimo urto del battello: rollio_gradi e istante
    double tuoni_fino_ = 0.0;                        // s di missione: i lampi gia' passati al bordo (tuono)
    double anticipo_s_ = 0.0;
    bool ricostruzione_ = false;                     // la partita si rigioca per «Riprendi da qui»
    mutable std::vector<SagomaArma> rastrelliera_;   // per lo spaccato, rifatta a ogni disegno
    SagomaArma sagoma_arma(const std::string& variante, Arma famiglia) const;
    void classifica_dai_tracker(double t);
    // periscopi dell'Enhanced (docs/14 P4): comandi della stazione, vista dal modello, lista NWP della guardata
    void cifra_periscopi(int n, double t);
    void periscopi_vista(const Comandi& c, VistaPeriscopio& vista, double dt);
public:
    // La vista dal posto dei periscopi (oculare del modello, onde della simulazione, ingrandimento): prima di disegnare
    // la scena, anche senza finestra
    // i campioni originali del classico (data/motore/suoni_classici), caricati una volta da main; nullptr: muto
    void imposta_suoni_classici(const DatiSuoniClassici* d) { dati_classici_ = d; }
    void prepara_vista(VistaPeriscopio& vista) const;
    // F1 in missione (Enhanced): le voci dell'enciclopedia pertinenti alla stazione, la piu' specifica prima (l'arma del
    // tubo scelto, il periscopio, il bersaglio, il proprio battello); vuoto se F1 non e' stato premuto
    std::vector<std::string> enciclopedia_chiesta();
private:
    double ultimo_rilevamento_ = 0.0;      // della vista, per la foto
    bool enciclopedia_ = false;            // F1 premuto: main apre il Museo sulla voce pertinente
    double elevazione_voluta_ = 0.0, prossima_elevazione_ = 0.0;
    double giro_gradi_ = 0.0, giro_prec_ = -1.0;   // rotazione nella guardata in corso (lista NWP)
    bool aria_ = false;
    std::map<int, std::string> classificati_;   // contatto -> classe gia' data al comando
    std::unique_ptr<Spaccato> spaccato_;           // quadro danni dell'Enhanced
    int dettaglio_danni_ = -1;                     // compartimento ingrandito: -1 il piu' in pericolo, o scelto con Tab
    // il controllo danni (docs/10 §10.19): 1 isola, chiude subito o riapre il compartimento mostrato, 5 la pagina delle squadre,
    // e li' 2 e 3 scelgono un sistema, 4 lo porta in cima alle riparazioni
    bool pagina_squadre_ = false;
    int sistema_scelto_ = 0;
    int compartimento_danni() const;               // quello mostrato: dettaglio_danni_ o il piu' in pericolo
    std::unique_ptr<TestoComandi> testo_comandi_;  // la schermata dei comandi (F11)
    std::optional<Font> font_comandi_, targa_comandi_;
    MappaTasti tasti_comandi_ = MappaTasti::predefinita();
    bool schermata_aperta_ = false, suggerimento_comandi_ = false;
    int scheda_comandi_ = 0, pagina_comandi_ = 0;
    mutable int pagine_comandi_ = 1;
    mutable std::vector<AreaComando> aree_comandi_;
    void pulsante_squadre(int n, double t);
    std::vector<RigaStorico> storico_;
    // barra del bersaglio: gli ultimi valori restano senza traccia; rumore estratto solo quando il valore cambia
    struct ValoriBarra {
        int bersaglio = -1;
        bool validi = false;
        int distanza_decimi = 0;
        double velocita_vera = -1.0, velocita = 0.0, quota_vera = -1e9, quota = 0.0;
    };
    mutable ValoriBarra barra_;
    mutable std::mt19937 rumore_{688};
};

}  // namespace m688
