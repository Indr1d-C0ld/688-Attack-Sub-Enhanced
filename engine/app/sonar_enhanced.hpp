#pragma once
// Stazione Sonar completa dell'Enhanced (docs/11_comandi_e_stazioni.md §11, S3 e revisione S5a; punti 19 e 24 della
// roadmap; ricerca in docs/ricerca/09_sonar_sistemi_e_interfacce.md §2-§4 e §6.1). Sui dati del modulo di analisi
// acustica (nucleo/analisi_acustica.hpp) tiene le storie e le disegna nell'area della vista, in stile SDR:
// - cascata per rilevamento a banda larga (BTR) del sensore scelto, una riga ogni 2 s, la piu' recente in alto, con
//   normalizzatore a finestra divisa sui fasci, griglia del tempo e i tracker S1, S2...;
// - A-scan (spettro dell'ultima riga, con massimo e minimo delle ultime) sopra la cascata LOFAR del fascio puntato
//   (il bersaglio, un tracker o un rilevamento fissato a mano), fra i filtri f_min e f_max, con zoom in frequenza;
// - integrazione: ogni riga mostrata e' la media delle ultime N misurate (1, 2, 4, 8), che abbassa la grana di
//   radice di N; scala dei tempi x1, x4, x16 (una riga per 1, 4 o 16 righe, decimate col massimo: i transitori brevi
//   restano visibili);
// - pavimento e tetto della scala dei colori (dB sopra il fondo) regolabili, AUTO nel modo moderno; palette 1989
//   (fosforo verde, grigio) e moderne (viridis, turbo, caldo), negli indici 200-231 della palette (il cielo, che qui
//   non si vede);
// - cursori sul LOFAR: lettura (frequenza e tempo), delta rispetto a un riferimento, famiglia armonica;
// - tracker: assegnati a mano sul fascio puntato, o automatici nel modo moderno; seguono il picco del BTR, si perdono
//   dopo 30 s senza segnale; ANALIZZA sul fascio di un tracker gli da' il passo di classificazione (POSSUB,
//   PROBSUB) e il rapporto al comando;
// - DEMON del bersaglio (pale e giri) e analizzatore di firma con le prime ipotesi;
// - S5b: libreria consultabile (schede navale, biologico, geofisico, meteo, umano) con le righe di una voce sovrapposte
//   al LOFAR e il fattore Doppler regolabile; ANALIZZA col confronto per rapporti (insensibile al Doppler: fattore k
//   e velocita' radiale) e le famiglie armoniche; annotazioni sulla cascata; 8 slot dello spettro col confronto
//   affiancato, sovrapposto o per differenza.
// Il modulo e' deterministico (la grana del rumore ha i suoi semi); non cambia la simulazione: lo stesso replay da' le
// stesse righe.

#include <array>
#include <deque>
#include <map>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "grafica/font.hpp"
#include "grafica/sale.hpp"
#include "grafica/palette.hpp"
#include "grafica/periscopio.hpp"
#include "grafica/scala_cascata.hpp"
#include "cascata_demon.hpp"
#include "nucleo/rilevatore_righe.hpp"
#include "righe_automatiche.hpp"
#include "vista_storia.hpp"
#include "nucleo/storia_cascata.hpp"
#include "grafica/tela.hpp"
#include "missioni/missione.hpp"
#include "nucleo/acint.hpp"
#include "nucleo/analisi_acustica.hpp"
#include "nucleo/ascolto.hpp"
#include "nucleo/misure_sonar.hpp"
#include "nucleo/casuale.hpp"

namespace m688 {

inline constexpr int MSG_ANALISI_PRONTA = 342, MSG_ANALISI_INCERTA = 343;
// rapporti dei tracker (messaggi del motore, fascia della sessione principale 370-399)
inline constexpr int MSG_TRACKER_NUOVO = 370, MSG_TRACKER_PERSO = 371, MSG_POSSUB = 372, MSG_PROBSUB = 373,
                     MSG_CERTSUB = 374, MSG_NONSUB = 375, MSG_TRACKER_PIENI = 376;

// Modo della stazione, scelto per partita: 1989 (fosforo verde, tutto a mano, come un BQQ-5) o moderno (colori,
// AUTO, tracker automatici, come un A-RCI)
enum class ModoSonar : std::uint8_t { anni_89, moderno };
enum class PaletteSonar : std::uint8_t { fosforo, grigio, viridis, turbo, caldo };
const char* nome_palette(PaletteSonar p);
// Passi della classificazione di un contatto (docs/ricerca/09 §2.9)
enum class PassoClassifica : std::uint8_t { nessuno, nonsub, possub, probsub, certsub };
const char* nome_passo(PassoClassifica p);
// Il passo dalle due voci che la stazione mostra: PROBSUB se sono sottomarini (ssn, ssgn, ssbn, ssk), POSSUB se lo e'
// una delle due, altrimenti nessuno. Mai NONSUB
PassoClassifica passo_da_voci(const std::vector<ConfrontoRapporti>& v, const CatalogoAcustico& cat);

struct TrackerSonar {
    int numero = 0;                 // S1, S2, ...
    double rilevamento = 0.0;       // vero, gradi
    double livello_db = 0.0;        // sopra il fondo, nell'ultima riga
    int mancati = 0;                // righe di fila senza il picco
    bool perso = false, automatico = false;
    PassoClassifica passo = PassoClassifica::nessuno;
    std::string classe;             // prima ipotesi dell'ultima analisi
    double confidenza = 0.0;
    std::deque<double> storia;      // rilevamenti delle ultime righe, il piu' recente prima
    double rateo_s = 0.0;           // gradi al secondo, dal filtro alfa-beta
    double rateo() const;           // gradi al minuto (0 finche' il filtro non si e' assestato)
};

// messaggi delle misure e dell'ascolto (S5d, fascia del principale)
inline constexpr int MSG_VERNIERO_AGGANCIATO = 390, MSG_VERNIERO_SGANCIATO = 391, MSG_VERNIERO_CURSORE = 392;
inline constexpr int MSG_ASCOLTO_REGISTRATO = 393, MSG_ASCOLTO_FALLITO = 394;

class SonarEnhanced {
public:
    static constexpr int RIGHE = 150;            // storia per ogni scala dei tempi
    static constexpr double PASSO_S = 2.0;
    static constexpr int SCALA0 = 200, LIVELLI = 32;
    static constexpr int MAX_TRACKER = 8;
    // persistenza: solo 1989; maschera: le righe del proprio battello tolte dall'analisi (blocco 2 del sonar, F);
    // colore_rilevamento: solo moderno, il colore per rilevamento sul LOFAR (scelta dell'utente del 29/10/2026)
    enum class Campo : std::uint8_t { pavimento, tetto, palette, scala_tempi, doppler, persistenza, colore_rilevamento, maschera };
    enum class Confronto : std::uint8_t { spento, affiancato, sovrapposto, differenza };
    static constexpr int SLOT = 8, MAX_ANNOTAZIONI = 20;
    struct Annotazione {
        int numero = 0;
        double tempo = 0.0, hz = 0.0;   // s di missione e frequenza del punto annotato
    };
    struct Slot {
        Spettro spettro;
        double tempo = 0.0, fascio = 0.0;
        int tracker = 0;
        bool pieno = false;
        std::string origine;   // missione e ora dello spettro («SCENE1A 00:12»): nel quaderno vale fra le missioni
    };
    // demon: i cursori armonici del DEMON; righe: le righe automatiche del modo moderno (blocco 3 del sonar, H)
    // storia: la storia lunga del LOFAR e del BTR (blocco 3, I), in entrambi i modi
    enum class Cursore : std::uint8_t { spento, lettura, delta, armonico, demon, righe, storia };

    explicit SonarEnhanced(ModoSonar modo = ModoSonar::moderno) { imposta_modo(modo); }
    void imposta_modo(ModoSonar m);
    ModoSonar modo() const { return modo_; }

    // Dopo ogni passo della missione: una riga nuova ogni 2 s di simulazione (con l'acustica dell'Enhanced)
    void aggiorna(const Missione& m, int bersaglio);
    // Rapporti al comando nati dall'ultima chiamata (codici dei messaggi del motore)
    std::vector<int> messaggi();

    // Sensori della suite del battello del giocatore (bqq5_sfera, tb16, ...) e quello scelto
    const std::vector<std::string>& sensori() const { return sensori_; }
    const std::string& sensore() const { return sensore_; }
    void sensore_successivo();
    // Fascio del LOFAR: il bersaglio, poi i tracker a giro; Z e X lo fissano a mano a passi di 5 gradi
    void fascio_successivo();
    void sposta_fascio(int verso);
    void segui_bersaglio() { fascio_fisso_.reset(); segue_tracker_ = 0; }
    int segue_tracker() const { return segue_tracker_; }   // 0: il bersaglio o il fascio fisso
    // Filtri del punto 24: banda passante del LOFAR (passa banda 10-410 Hz o passa alto 100-410 Hz)
    void commuta_filtro() { passa_alto_ = !passa_alto_; }
    bool passa_alto() const { return passa_alto_; }
    // Integrazione: 1, 2, 4, 8 righe, a giro; vale per le righe nuove
    void commuta_integrazione() { integrazione_ = integrazione_ >= 8 ? 1 : integrazione_ * 2; }
    int integrazione() const { return integrazione_; }
    // Presentazione: campo scelto con Tab, valore con [ e ]; AUTO (modo moderno) dall'ultima riga
    void campo_successivo(int verso);
    void cambia_valore(int verso);
    void automatico();
    Campo campo() const { return campo_; }
    double pavimento() const { return pavimento_; }
    double tetto() const { return tetto_; }
    PaletteSonar palette() const { return palette_; }
    int scala_tempi() const { return scala_tempi_; }
    // Cursori sul LOFAR: spento, lettura, delta (il riferimento e' dove era il cursore), armonico; frecce per muoverli
    void cursore_successivo();
    void muovi_cursore(double d_hz, int d_righe);
    Cursore cursore() const { return cursore_; }
    double cursore_hz() const { return cursore_hz_; }
    // Zoom in frequenza del LOFAR attorno al cursore (o al centro), 1-16
    void zoom(int verso);
    int zoom() const { return zoom_; }
    // Tracker: uno nuovo sul fascio puntato; via quello seguito dal LOFAR (o l'ultimo perso)
    void nuovo_tracker();
    void togli_tracker();
    const std::vector<TrackerSonar>& tracker() const { return tracker_; }
    // ANALIZZA: righe osservate nell'ultimo spettro, DEMON per le pale, confronto con la banca dati delle classi; il
    // tracker sul fascio prende il passo di classificazione (e il rapporto, se cambia). Apre la pagina
    // dell'analizzatore (la bozza C, scelta dell'utente del 29/10/2026), o la richiude: l'analisi si fa sempre
    int analizza(const Missione& m);
    bool pagina_analisi() const { return pagina_analisi_; }
    void chiudi_pagina_analisi() { pagina_analisi_ = false; }   // per Esc
    // Il colore per rilevamento sul LOFAR (modo moderno, aiuto acceso di serie; campo «RIL» della presentazione): le
    // righe piu' forti in un fascio vicino (10 o 20 gradi a sinistra o a dritta) che in quello puntato prendono il
    // colore del loro lato, blu a sinistra e viola a dritta (lo studio DSTO del 1997, docs/ricerca/30 §3). Vale sulla
    // scala dei tempi x1, dove le righe sono quelle misurate
    bool colore_rilevamento() const { return modo_ == ModoSonar::moderno && colore_ril_; }
    // Vero se l'aiuto ha colorato almeno una riga del LOFAR nella partita: il realismo del punteggio (-0,05)
    bool colore_rilevamento_usato() const { return colore_ril_usato_; }
    // Gli echi falsi delle chiglie di ghiaccio dell'ultimo ping attivo (Missione::echi_falsi_attivo, del disassemblatore):
    // sulla cascata dei rilevamenti come tacche che svaniscono in ECO_DURATA_S, senza lettera e senza tracker. Il
    // giocatore li distingue dai contatti guardandoli nel tempo: sono fermi e tornano a ogni ping
    struct EcoVisto { double t = 0.0, rilevamento = 0.0, distanza_m = 0.0, eccesso_db = 0.0; };
    static constexpr double ECO_DURATA_S = 10.0;
    const std::vector<EcoVisto>& echi_falsi() const { return echi_; }
    const std::vector<Ipotesi>& ipotesi() const { return ipotesi_; }

    // Libreria (S5b): apri e chiudi, scheda (Tab), voce (frecce), sovrapponi la voce al LOFAR (Invio), via (Invio
    // fuori dalla libreria); il fattore Doppler della sovrapposizione e' il campo «K» della presentazione
    void imposta_nomi(std::map<std::string, std::string> nomi) { nomi_ = std::move(nomi); }   // id della classe -> nome
    void commuta_libreria();
    bool libreria_aperta() const { return libreria_aperta_; }
    void scheda_successiva(int verso);
    void voce_successiva(int verso);
    void sovrapponi();
    const std::optional<VoceLibreria>& voce_scelta() const { return scelta_; }
    void togli_sovrapposizione() { sovrapposta_.reset(); }
    const std::optional<VoceLibreria>& sovrapposta() const { return sovrapposta_; }
    double fattore_doppler() const { return k_; }
    const std::vector<ConfrontoRapporti>& confronti() const { return confronti_; }
    const std::vector<FamigliaArmonica>& famiglie() const { return famiglie_; }
    // La scritta dell'analizzatore: le voci che le righe reggono, con quante ne spiegano, o «NESSUN NOME»; vuota prima di analizzare
    struct LetturaAnalisi {
        std::string testo;
        bool nome = false;
    };
    LetturaAnalisi lettura_analisi(std::size_t lungo = 14, bool una_voce = false) const;
    // Annotazioni sulla cascata LOFAR, al cursore (o al centro): N1, N2...; via l'ultima
    void annota();
    void togli_annotazione() { if (!annotazioni_.empty()) annotazioni_.pop_back(); }
    const std::vector<Annotazione>& annotazioni() const { return annotazioni_; }
    // Slot: salva l'ultimo spettro nello slot scelto (e passa al successivo), scegli lo slot, modo del confronto
    // L'Alfa nel modo 1989 non ha registratore (dossier 19 §6.6, scelta dell'utente del 25/10/2026): niente slot,
    // salvarli, sceglierli e confrontarli non fa nulla. Il 688 e l'Alfa nel modo moderno li hanno
    bool slot_disponibili() const { return !(modo_ == ModoSonar::anni_89 && classe_ == "alfa"); }
    void salva_slot();
    void slot_successivo() { if (slot_disponibili()) slot_ = (slot_ + 1) % SLOT; }
    void confronto_successivo() {
        if (slot_disponibili()) confronto_ = static_cast<Confronto>((static_cast<int>(confronto_) + 1) % 4);
    }
    int slot() const { return slot_; }
    const std::array<Slot, SLOT>& slots() const { return slots_; }
    Confronto confronto() const { return confronto_; }
    // Quaderno del sonarista (requisito 27): gli slot pieni si salvano su file a fine missione e si ricaricano alla
    // successiva, cosi' uno spettro registrato si confronta con un contatto di un'altra missione (riconoscere un
    // battello gia' incontrato). `missione` va nell'origine degli slot nuovi
    void imposta_missione(std::string id) { missione_ = std::move(id); }
    // R5 del disassembler: i campioni reali per la cuffia (la libreria vive quanto la stazione; nullptr: solo il modello)
    void imposta_campioni(const LibreriaCampioni* l) {
        campioni_ = l;
        if (cuffia_) cuffia_->imposta_campioni(l);
    }
    bool salva_quaderno(const std::string& percorso) const;
    int carica_quaderno(const std::string& percorso);   // slot letti (0 se il file non c'e')
    static std::string percorso_quaderno();             // accanto alla cartella dei fascicoli

    // S5d, misure del sonarista: F aggancia la riga sotto il cursore (e la sgancia). Ogni 10 s il verniero la misura
    // nello spettro di zoom e la segue; la serie frequenza-tempo da' la curva a S (CPA: frequenza a riposo, velocita'
    // relativa, istante e distanza) e, con i rilevamenti, la distanza FRAZ. Il contatto e' quello del fascio.
    struct RigaMisurata {
        double hz = 0.0;                          // centro attuale: segue la riga
        std::vector<PuntoFrequenza> serie;
        std::vector<Rilevamento> rilevamenti;     // stessi istanti della serie
        std::vector<double> rotte;                // rotta propria agli stessi istanti (manovra per il FRAZ)
        MisuraRiga ultima;
        MisuraCpa cpa;
        StimaFraz fraz;
        double prossima = 0.0;
        int contatto = -1;
    };
    void aggancia_riga();
    const std::optional<RigaMisurata>& riga_misurata() const { return riga_; }
    // Registratore (ACINT): il contatto del fascio, un campione ogni 5 s; il rapporto di fine missione
    const Registratore& registratore() const { return registratore_; }
    std::optional<RapportoAcint> rapporto_acint() const;
    // Ascolto: il fascio puntato per 8 s in un WAV (il gioco non ha ancora l'uscita audio), nella cartella degli ascolti
    // accanto a quella dei fascicoli; il percorso, o "" se non si e' potuto
    std::string registra_ascolto(const Missione& m) const;
    // Cuffia del sonarista in tempo reale (R2, nucleo/ascolto.hpp): i campioni del fascio puntato per `dt_reale` secondi
    // di tempo vero (22.050 Hz), con la banda del filtro scelto; la scena si rilegge ogni 0,25 s. tace() la azzera
    std::vector<float> ascolta(const Missione& m, double dt_reale);
    void tace();
    static constexpr int CUFFIA_HZ = 22050;
    // Q: la cuffia a giro normale, ottava su (le righe sotto 50 Hz salgono), accelerata ×4 e ×16 (un segmento suona piu'
    // acuto, per sentire i ritmi lenti delle pale)
    enum class ModoCuffia : std::uint8_t { normale, ottava, x4, x16 };
    void modo_cuffia_successivo() { modo_cuffia_ = static_cast<ModoCuffia>((static_cast<int>(modo_cuffia_) + 1) % 4); }
    ModoCuffia modo_cuffia() const { return modo_cuffia_; }

    // Storie alla scala x1, la riga piu' recente per prima (prove)
    const std::deque<RigaCascata>& btr() const { return btr_[0]; }
    const std::deque<Spettro>& lofar() const { return lofar_[0]; }
    const std::deque<RigaCascata>& btr(int scala) const { return btr_[indice_scala(scala)]; }
    double fascio() const { return fascio_; }
    const std::optional<LetturaDemon>& lettura_demon() const { return lettura_; }

    // La riga del BTR normalizzata sui fasci (dB sopra il fondo dei fasci vicini; i ciechi a 0)
    static std::vector<float> normalizza_btr(const RigaCascata& r);
    // La scala dei colori della palette scelta negli indici SCALA0..SCALA0+LIVELLI-1; con la scala lunga i 128 indici
    // di cascate::SCALA_LUNGA (grafica/scala_cascata)
    void applica_palette(Palette& p) const;
    // L'indice della palette per un valore in dB sopra il fondo (la scala corta)
    std::uint8_t colore(double db) const;
    // La scala lunga col dithering Bayer (roadmap 33): quando la stazione occupa la vista, anche nella sala sonar (la
    // cornice usa il tema, 16-47); prende indici che altrove usano mare, sprite e cielo, quindi nel Museo resta la corta
    void usa_scala_lunga(bool s) { scala_lunga_ = s; }
    bool scala_lunga() const { return scala_lunga_; }
    cascate::Persistenza persistenza() const { return persistenza_; }
    void imposta_persistenza(cascate::Persistenza p) { persistenza_ = p; }   // dalle preferenze dell'utente
    bool maschera() const { return maschera_; }
    // il DEMON a cascata (blocco 3 del sonar, G; app/cascata_demon di trailer-1), sotto il LOFAR: i cursori armonici col
    // cursore in modo demon
    void demon_asse(int verso, bool fine) { demon_casc_.sposta_asse(verso, fine); }
    void demon_pale(int verso) { demon_casc_.pale(verso); }
    bool demon_aggancia() { return modo_ == ModoSonar::moderno && demon_casc_.aggancia(); }   // nel 1989 a mano
    const CascataDemon& cascata_demon() const { return demon_casc_; }
    // le righe automatiche (ALI/ALT, solo moderno; app/righe_automatiche di trailer-1): su e giu' la scelta, F il verniero
    void righe_su_giu(int verso) { righe_auto_.su_giu(rilevatore_.confermate(), verso); }
    bool righe_aggancia();   // il verniero sulla riga scelta; falso se non ce n'e'
    const RilevatoreRighe& rilevatore() const { return rilevatore_; }
    // la storia lunga (app/vista_storia di trailer-1): su e giu' il cursore del tempo, [ e ] la risoluzione, F «quando
    // e' comparsa» la riga del cursore (o la riga automatica scelta)
    void storia_cursore(int verso, bool grosso) { vista_storia_.cursore(verso, grosso); }
    void storia_risoluzione(int verso) { vista_storia_.risoluzione(verso); }
    bool storia_cerca();
    const std::vector<RigaPropria>& righe_proprie_nel_fascio() const { return righe_proprie_; }
    // il fosforo con persistenza, bagliore, grana e righe: modo 1989, palette fosforo, scala lunga
    bool con_fosforo() const { return scala_lunga_ && modo_ == ModoSonar::anni_89 && palette_ == PaletteSonar::fosforo; }

    // La stazione nella vista intera (640x360), o dentro un rettangolo (la sala sonar del 688: il rettangolo libero di
    // cornice_sala_sonar); disegna e ritaglia solo nell'area
    void disegna(Tela& t, const Font& f) const;
    void disegna(Tela& t, const Font& f, Rettangolo area) const;
    // Okean dell'Alfa (dossier 19 §6.6): lo schermo tondo di Enisej al posto della cascata dei rilevamenti, il LOFAR
    // accanto; con la palette del tema dell'Alfa applicata
    void disegna_okean(Tela& t, const Font& f, const FontConsolle& fc, Rettangolo area = {0, 0, LARGHEZZA_TELA, ALTEZZA_VISTA}) const;
    // I livelli 0-1 dell'ultima riga per rilevamento relativo alla prua (360 valori, 0 la prua): lo schermo di Okean
    std::vector<float> livelli_relativi() const;
    // Enisej come cascata polare (bozza D, scelta dell'utente del 29/10/2026): nel cerchio di raggio r in (cx, cy)
    void disegna_enisej(Tela& t, const FontConsolle& fc, int cx, int cy, int r) const;

private:
    static int indice_scala(int s) { return s >= 16 ? 2 : s >= 4 ? 1 : 0; }
    void nuova_riga(RigaCascata b, Spettro s);
    void aggiorna_tracker(const RigaCascata& riga);
    TrackerSonar* tracker_numero(int n);
    double f_min() const;
    std::pair<double, double> vista_hz() const;   // finestra del LOFAR in frequenza, con lo zoom
    // la disposizione delle parti nell'area (nella vista intera i numeri di sempre)
    struct Disposizione {
        bool consolle = false;                     // la consolle BQQ-5 del 1989 (la bozza B): due CRT e i tasti
        Rettangolo area;
        int bw = 0, top = 0, fondo = 0;            // cascata dei rilevamenti: larghezza, prima riga, fine delle cascate
        int lx = 0, lw = 0, ay = 0, ah = 0, ly = 0;  // LOFAR: x, larghezza, A-scan (y e altezza), prima riga
        int fl = 0;                                // fine del LOFAR: sotto, fino a fondo, il DEMON a cascata
    };
    Disposizione disposizione(Rettangolo a) const;
    // la consolle del 1989: lamiera, i due CRT, la colonna dei tasti a luce, il vetro dei contatti; la disposizione
    // dentro i CRT
    Disposizione disegna_consolle(Tela& t, const Font& f, Rettangolo a) const;
    void disegna_analisi(Tela& t, const Font& f, Rettangolo a) const;
    // i colori del testo: verde fosforo nel 1989, ciano, grigio e bianco nel moderno
    std::uint8_t c_titolo() const;
    std::uint8_t c_tenue() const;
    std::uint8_t c_testo() const;
    void disegna_btr(Tela& t, const Font& f, const Disposizione& d) const;
    void disegna_demon(Tela& t, const Font& f, const Disposizione& d) const;
    void disegna_storia(Tela& t, const Font& f, const Disposizione& d, bool con_btr) const;
    // un tratto di cascata (w x h pixel) per il valore db: un colore solo con la scala corta, col dithering con la lunga
    void tratto_cascata(Tela& t, int x, int y, int w, int h, double db) const;
    // le righe osservate dell'ultima riga del LOFAR, senza le proprie se la maschera e' accesa
    std::vector<RigaOsservata> righe_per_analisi() const;
    static cascate::Fosforo::Effetti effetti_fosforo();
    void disegna_griglia(Tela& t, const Font& f, const Disposizione& d, bool con_btr) const;
    void disegna_lofar(Tela& t, const Font& f, const Disposizione& d) const;
    void disegna_fondo(Tela& t, const Font& f, const Disposizione& d) const;
    void disegna_libreria(Tela& t, const Font& f, const Disposizione& d) const;
    void disegna_riga_misurata(Tela& t, const Font& f, const Disposizione& d) const;
    int contatto_sul_fascio(const Missione& m, int bersaglio) const;
    void misura(const Missione& m, const Missione::SceneAcustica& scena, int bersaglio);
    void aggiorna_scelta();
    std::string nome_voce(const std::string& id) const;

    ModoSonar modo_ = ModoSonar::moderno;
    std::vector<std::string> sensori_;
    std::string sensore_;
    std::optional<double> fascio_fisso_;
    int segue_tracker_ = 0;
    double fascio_ = 0.0;                         // rilevamento vero del fascio dell'ultima riga
    double fascio_misure_ = 0.0;                  // il fascio delle misure da integrare
    bool passa_alto_ = false;
    int integrazione_ = 4;
    double prossimo_ = 0.0, tempo_ = 0.0;
    double rotta_ = 0.0;                          // rotta propria all'ultima riga (Okean: rilevamenti relativi)
    std::string classe_;                          // classe del battello (l'Alfa del 1989 senza slot)
    // storie per scala dei tempi (x1, x4, x16) e accumulatori del massimo per le scale lente
    std::array<std::deque<RigaCascata>, 3> btr_;
    std::array<std::deque<Spettro>, 3> lofar_;
    std::array<RigaCascata, 3> acc_btr_;
    std::array<Spettro, 3> acc_lofar_;
    std::array<int, 3> acc_n_{};
    std::deque<RigaCascata> btr_misure_;          // le ultime righe misurate, per l'integrazione
    std::deque<Spettro> lofar_misure_;
    Spettro demon_;
    std::optional<LetturaDemon> lettura_;
    std::vector<Ipotesi> ipotesi_;
    Casuale grana_{0x50a4u};
    Casuale grana_lofar_{0x10fau};
    // presentazione
    Campo campo_ = Campo::pavimento;
    double pavimento_ = 2.0, tetto_ = 16.0;   // dB sopra il fondo: sotto i 2 dB la sola grana del rumore
    PaletteSonar palette_ = PaletteSonar::viridis;
    bool scala_lunga_ = false;
    cascate::Persistenza persistenza_ = cascate::Persistenza::normale;   // il fosforo del 1989 (roadmap 33)
    mutable std::optional<cascate::Fosforo> fosforo_ascan_;   // la traccia dell'A-scan che si spegne piano
    // le righe del proprio battello nel fascio (docs/22, F): il catalogo del momento, la maschera, l'avviso di una
    // riga nuova (un regime delle pompe, un generatore che parte)
    std::vector<RigaPropria> righe_proprie_;
    CascataDemon demon_casc_;
    RilevatoreRighe rilevatore_;
    std::optional<StoriaCascata> storia_lofar_, storia_btr_;   // il LOFAR del fascio; il BTR su 72 fasci veri da 0
    VistaStoria vista_storia_;
    VistaRigheAutomatiche righe_auto_;
    double fascio_demon_ = 1e9;
    int bersaglio_demon_ = -1;
    bool colore_ril_ = true, colore_ril_usato_ = false, pagina_analisi_ = false;
    // il lato di ogni bin delle righe del LOFAR alla scala x1 (stesso ordine di lofar_[0]): 0 il fascio puntato, -1 e -2
    // i fasci a 10 e 20 gradi a sinistra, +1 e +2 a dritta
    std::deque<std::vector<std::int8_t>> lati_;
    std::vector<EcoVisto> echi_;   // gli echi falsi dell'ultimo ping
    bool maschera_ = true;   // di base accesa: l'equipaggio ha il catalogo del proprio rumore; spenta, gli errori del dossier 09 §3.9
    double riga_nuova_hz_ = 0.0, riga_nuova_fino_ = -1.0;
    int scala_tempi_ = 1;
    Cursore cursore_ = Cursore::spento;
    double cursore_hz_ = 60.0, riferimento_hz_ = 60.0;
    int cursore_riga_ = 0, riferimento_riga_ = 0;
    int zoom_ = 1;
    // libreria, analisi per rapporti, annotazioni e slot (S5b)
    std::map<std::string, std::string> nomi_;
    const CatalogoAcustico* catalogo_ = nullptr;      // della missione, dall'ultimo aggiornamento
    bool libreria_aperta_ = false;
    int scheda_ = 0, voce_ = 0;
    std::vector<std::string> elenco_, nomi_elenco_;   // le voci della scheda aperta e i loro nomi
    std::optional<VoceLibreria> scelta_, sovrapposta_;
    double k_ = 1.0;
    std::vector<ConfrontoRapporti> confronti_;
    std::vector<FamigliaArmonica> famiglie_;
    std::vector<Annotazione> annotazioni_;
    int prossima_annotazione_ = 1;
    std::array<Slot, SLOT> slots_{};
    int slot_ = 0;
    std::string missione_;
    Confronto confronto_ = Confronto::spento;
    // tracker
    std::vector<TrackerSonar> tracker_;
    int prossimo_numero_ = 1;
    std::vector<int> messaggi_;
    // misure e registratore (S5d)
    std::optional<RigaMisurata> riga_;
    Registratore registratore_;
    double prossima_registrazione_ = 0.0;
    // cuffia (R2)
    std::optional<CuffiaSonar> cuffia_;
    const LibreriaCampioni* campioni_ = nullptr;
    std::optional<Missione::SceneAcustica> scena_cuffia_;
    double scena_cuffia_s_ = 0.0;                // tempo vero dall'ultima lettura della scena
    std::string sensore_cuffia_;
    bool alto_cuffia_ = false;
    ModoCuffia modo_cuffia_ = ModoCuffia::normale, modo_applicato_ = ModoCuffia::normale;
};

}  // namespace m688
