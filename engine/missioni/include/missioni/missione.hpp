#pragma once
// Missioni in Lua 5.4 (docs/09_schema_missioni.md) e runtime del "livello classico": le scene originali
// convertite da tools/re/missioni_lua.py girano con la semantica della VM del 1989 (docs/04_vm_missioni.md):
// un buffer del modello corrente che gli script modificano e che `salva_modello` riscrive nell'unita' selezionata,
// contatti con qualita' e durata della traccia, valutazione delle unita' a ogni tick di 2 s, timer, variabili di
// scenario, messaggi di bordo (codici >= 100: esito della missione).
//
// Lo script gira in un ambiente isolato: niente io, os, require, load, dofile; vede solo `m688` e le librerie
// math, string e table.

#include <array>
#include <cstdint>
#include <cmath>
#include <deque>
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <tuple>
#include <string>
#include <variant>
#include <vector>

#include "missioni/interventi.hpp"
#include "missioni/registro_missione.hpp"
#include "nucleo/acustica.hpp"
#include "nucleo/acustica_asw.hpp"
#include "nucleo/artiglieria.hpp"
#include "nucleo/campioni.hpp"
#include "nucleo/asw_aereo.hpp"
#include "nucleo/meteo.hpp"
#include "nucleo/periscopi.hpp"
#include "nucleo/sonarista.hpp"
#include "nucleo/armi_ia.hpp"
#include "nucleo/carico.hpp"
#include "nucleo/carriera.hpp"
#include "nucleo/collisioni.hpp"
#include "nucleo/casuale.hpp"
#include "nucleo/radio.hpp"
#include "nucleo/classi.hpp"
#include "nucleo/difese_navi.hpp"
#include "nucleo/dinamica_sottomarino.hpp"
#include "nucleo/elettronica.hpp"
#include "nucleo/evasione.hpp"
#include "nucleo/manovre_tma.hpp"
#include "nucleo/missili.hpp"
#include "nucleo/traffico.hpp"
#include "nucleo/schiera.hpp"
#include "nucleo/intercettazione.hpp"
#include "nucleo/tma_ia.hpp"
#include "nucleo/fondale.hpp"
#include "nucleo/geometria.hpp"
#include "nucleo/mine.hpp"
#include "nucleo/navigazione.hpp"
#include "nucleo/quadro_fondale.hpp"
#include "nucleo/luci_navigazione.hpp"
#include "nucleo/sagoma_notte.hpp"
#include "nucleo/ritorni_fondo.hpp"
#include "nucleo/campo_ghiaccio.hpp"
#include "nucleo/modifica_profilo.hpp"
#include "nucleo/sondaggio_ia.hpp"
#include "nucleo/volta_ghiaccio.hpp"
#include "nucleo/mondo.hpp"
#include "nucleo/ordini.hpp"
#include "nucleo/rotte.hpp"
#include "nucleo/siluro.hpp"

struct lua_State;

namespace m688 {

// Record contatto (pool DS:2C52, docs/originale/sensori_e_navigazione.md §3.4).
struct Contatto {
    double qualita = 0.0;     // +0E: >= 800 traccia solida (IF_TRACK_GOOD); nell'originale il limite a 1200 non funziona
    double durata = 0.0;      // +14: valutazioni con segnale > 500; >= 800 media, >= 1400 lunga
    double segnale = 0.0;     // +10: segnale dell'ultima valutazione (unita' di gioco)
    // lampeggio dei contatti deboli (2011:10A5-112E): +06 = conto alla rovescia (basso) e in salita (alto), +12 =
    // 80h nascosto e 7 bit di conto. Con segnale <= 500 le fasi nascoste si accorciano (n, n, n-1, ..., 2 valutazioni)
    // e quelle visibili si allungano (1, 2, 3, ...), poi il contatto resta visibile; con segnale > 500 non cambia
    int conto_giu = 0, conto_su = 0;   // +06, +07
    int lampeggio = 0;                 // +12
    double ultima_scoperta = 0.0;   // s di missione
    double prima_scoperta = 0.0;    // s di missione: quando e' nato il contatto (m688.tempo_contatto)
    int conto = 1;            // +16: valutazioni prima che il contatto diventi il bersaglio (ENGAGE n lo porta a n)
    Vec2 pos_stimata;         // +08/+0A: posizione stimata (copia di quella vera con qualita' >= 800)
    double quota_stimata = 0.0;
    bool marcato = false;
    std::uint64_t ordine = 0;   // posto nella lista dell'osservatore (+08): i nuovi in coda (2011:10CB)
    char lettera = 0;           // +13: lettera del contatto per i giocatori (id 0 e 1), la piu' bassa libera da 'A'

    double rilevamento = 0.0;   // +04: rilevamento vero dall'osservatore (Enhanced: misurato, con l'errore del sensore)
    // posizione stimata valida: nel classico con la traccia solida; nell'Enhanced solo da eco, vista, radar (e per
    // l'IA dalla traccia solida, con un errore)
    bool posizione_nota = false;
    double t_posizione = 0.0;   // s di missione: quando quella posizione e' stata presa. L'ultima scoperta la rinnova anche
                                // l'ascolto passivo, che la posizione non la da': la freschezza di una posizione e' questa
    std::optional<double> rilevamento_specchio;   // Enhanced, schiera: l'altro rilevamento dell'ambiguita' conica
    // Enhanced: chi ha dato l'ultima scoperta (id del sensore di acustica.toml, oppure "vista" per armi proprie,
    // radar e periscopio, "esm" per l'intercettazione) e l'eccesso di segnale in dB, fluttuazione compresa
    std::string sensore;
    double eccesso_db = 0.0;
    // indizi dell'ultima valutazione (Enhanced), per avvisare il giocatore solo quando compaiono
    bool indizio_cavitazione = false, indizio_transitorio = false, indizio_lofar = false;

    // 1000:996F: l'interfaccia (990C) salta i contatti con traccia debole e nascosti dal lampeggio
    bool visibile() const { return qualita >= 800.0 || !(lampeggio & 0x80); }
    void lampeggio_da_capo(double s);        // nascosto per (550 - segnale)/8 + 1 valutazioni
    void passo_lampeggio();                   // una valutazione con segnale <= 500
};

// Punto della scia della carta (DS:47BE, 2011:12EE): il giocatore locale ogni 30 suoi turni di osservazione e, in quel
// turno, la posizione vera dei contatti con traccia solida
struct PuntoScia {
    int id;
    Vec2 pos;
};

// Evento acustico (lista DS:4772, al massimo 12): impulso del sonar attivo, lancio di missile, rumore transitorio.
struct EventoAcustico {
    int unita;
    double fine;   // s di missione
};

// Ordine del motore che prevale sugli script (versione Enhanced: reazioni dell'IA ai danni, docs/10 §7)
struct OrdineMotore {
    double velocita_max_kn = -1.0;   // < 0: nessun limite (propulsione danneggiata: 0)
    bool meta = false;               // naviga verso `verso` (soccorso, ritirata)
    Vec2 verso;
    double velocita_kn = 0.0;
    double fino_a = 1e18;            // s di missione
    // sottomarini danneggiati (missioni/danni_sottomarini.hpp, docs/10 §10.8): la quota segue il peso netto dell'acqua
    // imbarcata e dell'aria nelle casse, al rateo della fisica (m/s, positivo verso il basso), non la quota ordinata
    bool quota_fisica = false;
    double rateo_quota_fisica = 0.0;
    // per la dinamica dell'Enhanced (fase C6a): forze del battello danneggiato e governi in avaria
    double peso_netto_N = 0.0, momento_assetto_Nm = 0.0;
    bool timone = true, idroplani = true;
};

// Record d'unita' del livello classico (campi della VM, grandezze convertite).
struct UnitaClassica {
    int id = 0;
    std::string classe;
    std::string categoria;        // "sottomarino", "nave", "aereo", "arma", "nessuna" (dalla missione o dal flag)
    int tipo_originale = -1;      // tipo d'unita' dell'originale (0x00-0x2D: nomi DS:019A, firme di ANALYZE); -1 ignoto
    int flag = 0;                 // +06: 0x18 schieramento, 0x20 arma, ...
    bool giocatore = false;
    std::string cisterne;         // petroliere (danni critici, T7): vuoto cariche, "zavorra" o "zavorra_inerte" (dalla missione)
    Vec2 pos;
    double quota = 0.0, quota_ordinata = 0.0, rateo_quota = 0.0, quota_max = 500.0;   // m, m/s
    double rotta = 0.0, rotta_ordinata = 0.0, rateo_virata = 0.0;                    // gradi bussola, gradi/s (+ a dritta)
    bool rotta_libera = false;    // +34 = FFFFh (HEAD_TO -1): la virata con il rateo non si ferma sulla rotta ordinata
    double velocita = 0.0, velocita_ordinata = 0.0;                                  // nodi
    double avanti_tutta = 20.0, accelerazione = 0.25, virata = 1.44;                 // nodi, nodi/s, gradi/s
    double rateo_quota_modello = 1.0;                                                // m/s
    int rumore = 10, sonar = 18, difesa = 0, integrita = 100, p3c = 0, p4c = 0;
    int tipo = 1, conto_tipo = 1;   // +04 (opcode TYPE): osserva una volta ogni `tipo` giri della rotazione; conto dei giri
    int integrita_max = 100;      // integrita' del modello: misura dei danni da urto col fondo
    int flag54 = 0;               // +54: 01h schiera trainata filata, 02h periscopio alzato, 04h antenna alzata
    int flag_scena = 0;
    std::string g_bersaglio, g_reazione, g_arrivo;   // +10 al bersaglio (una volta), +56 bersaglio perso, +0E arrivo
    int agganciato = -1;          // +60: contatto agganciato da ENGAGE (conta solo quello)
    int inseguito = -1;           // +14: contatto inseguito (PURSUE)
    int prossimo_bersaglio = -1;  // +12: prossimo bersaglio da valutare quando osserva (-1 o tolta: la testa della lista)
    bool navigazione = false;     // naviga verso `meta` (o verso il bersaglio se `insegui`)
    bool insegui = false;
    Vec2 meta;
    int soglia_arrivo = 30;       // +50: soglia d'arrivo in quarti di unita' di gioco per asse (0: mai)
    int bersaglio = -1, collegata = -1;
    std::map<int, Contatto> contatti;
    int colpo = 0;                // +5E: 0 integra, 1 colpita una volta, 0xC9 piu' volte o da ATTACK_RND, 0xD3 distrutta
    int affondamento = 0;         // +55: visite che restano prima di togliere l'unita' distrutta (0: non affonda)
    int lanciatore = -1;          // +62: chi ha lanciato l'arma (resta dopo il taglio del filo, che azzera `collegata`)
    bool distrutta = false, persa = false;   // tolta dal gioco: a fine affondamento, o arma consumata o persa
    // fondale (solo con Missione::imposta_fondale): la quota e' quella della chiglia
    bool incagliata = false;      // fermata da un fondale piu' alto della chiglia (fino a 50 m da `punto_incaglio`)
    Vec2 punto_incaglio;
    bool sul_fondo = false;       // chiglia appoggiata sul fondo
    int lato_evasione = 0;        // IA: lato su cui aggira una secca (+1 dritta, -1 sinistra, 0 nessuna)
    std::vector<Vec2> percorso;   // IA: punti della rotta per acque navigabili verso la meta (vuota: via diretta)
    Vec2 meta_percorso;
    bool percorso_parziale = false;   // la meta non e' raggiungibile: il percorso finisce nel punto piu' vicino
    double pianificato = -1e9;    // s di missione dell'ultima pianificazione
    OrdineMotore ordine;          // reazioni ai danni (solo con la regola di simulazione)
    double velocita_salvata = 0.0;   // +5C: velocita' da riprendere dopo l'accostata (nodi; 0 = nessuna)
};

// Evento di un siluro del modello dell'Enhanced (fase C6b), con l'unita' e chi l'ha lanciato
struct EventoSiluroMissione {
    int siluro = -1, lanciatore = -1;
    EventoSiluro evento;   // tempo: s di missione
};

struct Timer {
    double tempo;       // s di missione
    int unita;
    std::string gestore;
    std::uint64_t ordine;   // a parita' di tempo, l'ordine di inserimento (determinismo)
    bool da_missile = false;   // nato dal gestore di lancio di un missile del modello (Enhanced): niente ATTACK_RND
};

// I campi della stazione dei missili (ordine imposta_missile, b = campo): le impostazioni del piano e i dati del
// bersaglio che ha solo l'interfaccia (i quadranti del MATE, la distanza dello stadimetro)
enum class CampoMissile : std::uint8_t { fonte, modo, taglia, srch_m, dstr_m, missione_tlam, distanza_m, rotta, velocita_kn,
                                         sigma_distanza_m, sigma_trasversale_m };

struct Messaggio {
    double tempo;
    int unita;
    int codice;
};

// Colpo andato a segno secondo la regola dell'originale (docs/originale/sensori_e_navigazione.md §9.8), per il motore:
// fisica dei danni, effetti e relitti. Posizione e quota sono quelle dell'arma (collisione) o della vittima.
struct Colpo {
    enum class Tipo { arma, attacco, attacco_casuale };   // collisione di un'arma, ATTACK (cariche), ATTACK_RND (missili)
    Tipo tipo = Tipo::arma;
    double tempo = 0.0;
    int vittima = -1, arma = -1, lanciatore = -1;
    std::string classe_vittima, classe_arma;
    Vec2 pos;
    double quota = 0.0;
    int danno = 0;
    bool riuscito = true;     // ATTACK fallito: la carica esplode lo stesso, vicino alla vittima, senza danni
    bool distrutta = false;   // la regola classica avvia l'affondamento (+55)
    std::string classe_lanciatore;   // per scegliere la testata (missili: il +62 della vittima)
    // geometria vera dello scoppio (siluri del modello dell'Enhanced, fase C6b): distanza dal centro della vittima e
    // quota dello scoppio meno quota della vittima (positiva: sotto lo scafo), nello stesso passo
    bool geometria_vera = false;
    double distanza_m = 0.0, dz_m = 0.0;
    // mine dell'Enhanced (M2b): la carica vera (0 = quella dell'arma secondo la classe) e lo scoppio a distanza
    double tnt_kg = 0.0;
    bool influenza = false;
};


// Lancio di un missile (MISSILE, opcode 5Ah) per gli effetti visivi (E2, docs/10 §10.15): chi lancia, da dove e
// verso dove. `arma` e' la voce di `armi` della missione il cui gestore sta girando (i lanci del giocatore:
// "harpoon", "tomahawk"); vuota per l'IA, che lancia dal proprio script.
struct LancioMissile {
    double tempo = 0.0;
    int unita = -1, bersaglio = -1;
    Vec2 pos;
    double quota = 0.0, rotta = 0.0;   // m; gradi bussola, verso il bersaglio se c'e'
    std::string classe_lanciatore, categoria_lanciatore, arma;
    int missile = -1;                  // l'unita' del missile del modello (-1: lo script senza modello, il TLAM)
    double distanza_bersaglio_m = 0.0; // vera, al lancio (docs/26: oltre l'orizzonte)
    int neutrali_scatola = 0;          // del giocatore: i neutrali nella scatola di ricerca del piano (docs/26)
};

// ASW aereo dell'Enhanced (docs/13_asw_aereo.md, A3): un marcatore lanciato da un aereo (Mk 25, Mk 58, OMAB) e un'arma
// aviolanciata (siluro col paracadute o bomba di profondita'), per gli effetti del kit, la carta e il debriefing
struct MarcatoreAereo {
    int aereo = -1;
    std::string tipo;                       // voce di DatiAsw::marcatori
    Vec2 pos;
    double inizio = 0.0, fine = 0.0;        // s di missione
};
struct LancioAereo {
    double tempo = 0.0;
    int aereo = -1, arma = -1;              // l'unita' dell'arma (-1 per le bombe: niente unita')
    std::string tipo;                       // voce di siluri.toml o bomba (plab250, plab50)
    Vec2 pos;
    double quota_sgancio_m = 0.0, rotta = 0.0;
};
// Avviso ASW al giocatore (380-389): chi l'ha causato (l'aereo) e, se c'e', il rilevamento vero misurato
struct AvvisoAsw {
    double tempo = 0.0;
    int unita = -1, codice = 0;
    std::optional<double> rilevamento;
};
// Lo stato del volo di un aereo controllato, per la vista e la carta
struct VoloAsw {
    int unita = -1;
    std::string classe;
    double quota_m = 0.0;                   // vera, sopra il mare
    ComandoVolo comando;                    // hover, cavo, luci, radar
    StatoAsw stato = StatoAsw::transito;
    FaseDip fase_dip = FaseDip::nessuna;
    double quota_trasduttore_m = 0.0;
    bool libero = false;                    // segue la rotta dello script
};

// Periscopi dell'Enhanced (docs/14_periscopi.md, P2)
struct FotoPeriscopio {                   // foto_periscopio: la vista del Type 18 per la minicartolina (registro M1)
    double tempo = 0.0;
    double rilevamento = 0.0;             // vero, gradi
    int ingrandimento = 0;                // indice nel modello
    double campo_gradi = 32.0;
    std::string periscopio;
    std::vector<int> in_vista;            // unita' nel campo, la piu' vicina prima
};
struct TestaIa {                          // un periscopio dell'IA sull'acqua (vista, radar e periscopio del giocatore)
    int unita = -1;
    std::string periscopio;
    Vec2 pos;
    double altezza_m = 0.0;
    bool da_script = false;               // alzato dallo script dell'originale (flag54 & 2)
};
struct GuastoRichiesto {                  // un sistema del battello rotto dalla missione (DanniSottomarini lo applica)
    int unita = -1;
    int sistema = 0;                      // indice di Sistema (nucleo/battello.hpp)
    double perdita = 0.0;                 // efficienza persa, 0..1
    std::string testo;
};

// R5: la libreria dei campioni registrati di `cartella_dati` (campioni.toml, i WAV sotto la cartella madre), caricata
// una volta per processo e condivisa fra le missioni e la cuffia; vuota se i dati mancano o un WAV non si legge
std::shared_ptr<const LibreriaCampioni> libreria_campioni(const std::string& cartella_dati);

// Parametro di un comando del giocatore: numero, o testo per i valori simbolici dell'originale ("arresto_rapido")
using ParametroComando = std::variant<double, std::string>;

// Limiti degli script di una missione di terzi (editor, pacchetti): istruzioni Lua per chiamata di un gestore (e per
// il caricamento del file) e memoria dello stato Lua. 0: senza limite. Superati, la chiamata finisce con un errore in
// errori() e la missione continua
struct LimitiScript {
    std::size_t istruzioni_per_chiamata = 0, memoria_byte = 0;
};

// I gestori d'esito di uno script (docs/18, l'IA con l'errore di posizione): le funzioni `script.X` che chiamano
// `m688.messaggio` (o `messaggio_al_giocatore`) con un codice d'esito, anche attraverso le chiamate `script.Y(u)`.
// Lettura euristica del testo convertito: un messaggio con un codice non letterale, o fuori da una funzione dello
// script, rende lo script illeggibile (e allora tutto resta vero). `vai_a_veri`: le funzioni con un vai_a il cui
// gestore d'arrivo e' d'esito (chiamante, gestore d'arrivo)
struct GestoriEsito {
    bool leggibile = true;
    std::set<std::string> esito;
    std::vector<std::pair<std::string, std::string>> vai_a_veri;
};
GestoriEsito leggi_gestori_esito(const std::string& testo);

class Missione {
public:
    // Carica il file (che restituisce la tabella della missione), sceglie la variante iniziale con il seme dato
    // e crea le unita'. Lancia std::runtime_error con il messaggio di Lua in caso di errore.
    // `moto_classico`: le velocita' in nodi si convertono con posa.moto_classico_m_s_per_nodo (il moto
    // dell'originale e' circa 3,46 volte il vero in scala di gioco), cosi' timer e distanze delle missioni tornano;
    // false = velocita' reali.
    Missione(const std::string& file, std::uint64_t seme, bool moto_classico = true)
        : Missione(file, seme, LimitiScript{}, moto_classico) {}
    Missione(const std::string& file, std::uint64_t seme, const LimitiScript& limiti, bool moto_classico = true);
    ~Missione();
    Missione(const Missione&) = delete;
    Missione& operator=(const Missione&) = delete;

    const std::string& id() const { return id_; }
    const std::string& titolo() const { return titolo_; }
    const std::string& teatro() const { return teatro_; }
    int variante() const { return variante_; }
    double tempo() const { return tempo_; }
    const std::map<int, UnitaClassica>& unita() const { return unita_; }
    const UnitaClassica* giocatore() const;
    // per i casi di regressione: FNV-1a sul numero dei contatti del giocatore e, in ordine di id, id, rilevamento (0,1
    // gradi) e posizione nota; 0 senza giocatore. Accanto a Mondo::impronta: cambia quello che sente, non il mondo
    std::uint64_t impronta_contatti() const;
    const std::vector<Messaggio>& messaggi() const { return messaggi_; }
    std::optional<int> esito() const { return esito_; }
    const std::vector<Colpo>& colpi() const { return colpi_; }
    // il giocatore 0 o 1 ha colpito un'unita' amica o neutrale (DS:1F66/1F68: riga in piu' nel rapporto finale)
    bool fuoco_amico(int giocatore) const { return giocatore >= 0 && giocatore < 2 && fuoco_amico_[giocatore]; }
    // Testo italiano dell'esito n (rapporto di fine missione del briefing originale), vuoto se assente.
    std::string testo_esito(int n) const;
    // E6: i messaggi di bordo propri della missione (chiave `messaggi = { [n] = { it = ..., en = ... } }`), coi codici
    // 1000 + n, fuori dalla tabella globale e dagli esiti: nullptr se il codice non e' della missione
    const TestoRadio* testo_messaggio_missione(int codice) const;
    // Registro degli eventi ed esito dettagliato (M1, missioni/registro_missione.hpp): attivo anche nel classico, che
    // non cambia (il registro osserva soltanto). raccogli_danni dopo DanniSottomarini::avanza e DanniMissione::avanza;
    // classifica_contatto quando il giocatore da' una classe a un contatto dalla stazione.
    const RegistroMissione& registro() const { return registro_; }
    const std::vector<EventoMissione>& eventi_missione() const { return registro_.eventi(); }
    std::vector<const EventoMissione*> momenti_salienti(int n = 12, double spazio_s = 60.0) const {
        return registro_.momenti_salienti(n, spazio_s);
    }
    EsitoDettagliato esito_dettagliato() const { return registro_.esito(*this); }
    // rapide v2 e libreria (docs/17 §3): l'obiettivo `id` e' raggiunto ora; secondi cumulativi di contatto visibile
    // dell'unita' `osservatore` su `id`, e secondi dall'ultima volta che l'ha visto (dall'inizio se mai). Si contano
    // per il giocatore da subito, per le altre unita' dalla prima domanda su di loro
    bool obiettivo_compiuto(const std::string& id) const;
    double contatto_cumulato_s(int osservatore, int id) const;
    double senza_contatto_s(int osservatore, int id) const;
    void raccogli_danni(const DanniSottomarini* sub, const DanniMissione* navi) {
        registro_.danni(*this, sub, navi);
        if (peri_) danni_periscopio(sub);
        if (variazioni_acustiche() && sub) danni_acustici(*sub);
        if (sub && artiglieria_) casse_squarciate_ia(*sub);
    }
    void classifica_contatto(int giocatore, int contatto, const std::string& classe) {
        registro_.classifica(*this, giocatore, contatto, classe);
    }
    const std::map<int, int>& variabili() const { return variabili_; }
    // Funzioni m688 chiamate dagli script ma non ancora implementate (dovrebbe restare vuoto).
    const std::set<std::string>& non_implementate() const { return non_implementate_; }
    // Errori di esecuzione degli script (Lua): la missione prosegue, ma la lista dovrebbe restare vuota.
    const std::vector<std::string>& errori() const { return errori_; }

    // Avanza la missione di dt secondi: moto, sensori e script a ogni tick di 2 s, timer.
    void avanza(double dt);

    // --- comandi del giocatore (docs/11_comandi_e_stazioni.md, fase C1) ---
    // Dispatcher dell'originale (docs/originale/sensori_e_navigazione.md §7): esegue sul giocatore il gestore
    // `gestori_giocatore[nome]` della missione con i parametri. Falso se la missione non ha quel gestore.
    bool comando(const std::string& nome, const std::vector<ParametroComando>& parametri = {});
    // Ordine del nucleo tradotto nei comandi del livello classico (telegrafo, timone, quote, punti di rotta e pilota
    // automatico, periscopio, antenna, schiera, bersaglio, tubi, lancio, guida a filo, sonar attivo). Falso se il
    // livello classico non lo prevede (spegnimento e taglio del filo dell'Enhanced, filtro, compressione del tempo).
    bool applica(const Ordine& o);
    const std::map<std::string, std::string>& gestori_giocatore() const { return gestori_giocatore_; }
    const std::vector<std::string>& tubi() const { return tubi_; }              // tipo d'arma per tubo (1-7)
    const std::map<std::string, int>& dotazione() const { return dotazione_; }  // scorte iniziali per tipo d'arma
    // Enhanced, carico M2 (dossier 14 §5.2): deposito, tipi negati, tipo di missione, variante del battello, ospiti e
    // what-if della missione, con la dotazione, i tubi dell'originale e l'anno della missione
    const LimitiMissione& limiti_carico() const { return limiti_carico_; }
    // Enhanced: le famiglie d'arma per tubo (il gestore di lancio segue il carico della sala siluri) e il modello del
    // siluro (voce di siluri.toml) del prossimo lancio da un tubo; "" = quello della famiglia
    void imposta_tubi(const std::vector<std::string>& famiglie) { tubi_ = famiglie; }
    void variante_tubo(int n, const std::string& siluro);
    // Blocco `carriera` della missione (M3, docs/09_schema_missioni.md): rapide ed editor; le originali non l'hanno
    struct BloccoCarriera {
        std::string situazione = "pace", avversario = "regolare";
        int difficolta = 3;
        bool operazione_speciale = false, artico = false;
    };
    const std::optional<BloccoCarriera>& blocco_carriera() const { return blocco_carriera_; }
    // DS:476C: livello dell'ultima intercettazione radar/ESM sul giocatore (85 vicinissimo, 1 al limite, 0 nessuna)
    // nell'Enhanced con l'ESM ripensato: l'emettitore piu' vicino fra quelli intercettati (17 per ampiezza, 0-85)
    int livello_esm() const { return esm_giocatore_ ? esm_giocatore_->livello_originale() : esm_; }
    // Emettitori radar, EMCON ed ESM dell'Enhanced (punto 23; nucleo/elettronica.hpp, data/motore/emettitori.toml).
    // Ogni unita' con radar ha la sua EMCON; un radar acceso vede l'albero o il battello emerso del giocatore con la
    // probabilita' per giro del dossier 11 §6.4 (contatto con la posizione, EMCON in ingaggio), al posto della regola
    // dell'originale (4,5-6,75 nm, sempre). L'ESM del giocatore intercetta i radar accesi: intercettazioni(), contatti
    // col sensore «esm» (solo il rilevamento), avvisi 334-338 in messaggi_esm().
    void elettronica_enhanced(const std::string& cartella_dati);
    bool con_elettronica() const { return catalogo_el_ != nullptr; }
    const std::vector<Intercettazione>& intercettazioni() const;
    std::optional<StatoEmcon> emcon(int id) const;
    const std::vector<Messaggio>& messaggi_esm() const { return messaggi_esm_; }
    // scia della carta: anello di 150 punti (solo quelli scritti); pieno, il piu' vecchio e' quello che segue l'ultimo
    const std::vector<PuntoScia>& scia() const { return scia_; }
    const std::vector<Vec2>& punti_di_rotta() const { return punti_di_rotta_; }
    bool pilota_automatico() const { return pilota_automatico_; }
    // Copia lo stato delle unita' in un Mondo (per il disegno): stessi id, posizione, rotta, velocita'.
    void sincronizza(Mondo& mondo) const;

    // Fondale del teatro e pescaggi delle classi in metri (engine/README.md, "Fondale e incagli"): le quote di
    // navi e sottomarini sono limitate dal fondo, l'IA aggira le secche e tiene un franco sotto la chiglia, chi
    // tocca il fondo si ferma e subisce danni secondo la velocita' d'urto. Senza fondale il mare non ha fondo.
    void imposta_fondale(std::shared_ptr<const Fondale> fondale, std::map<std::string, double> pescaggi = {});
    const Fondale* fondale() const { return fondale_.get(); }
    double profondita(Vec2 p) const;              // +infinito senza fondale o fuori dalla griglia
    double pescaggio(const UnitaClassica& u) const;
    // la chiglia vera di un'unita' (missione_collisioni.cpp): per i sottomarini fra la superficie e la quota periscopica
    // del classico interpolata dal pescaggio alla quota periscopica della classe (688: 18,3 m)
    double chiglia_vera(const UnitaClassica& u) const;
    // unita' spostate all'avvio da imposta_fondale perche' in acqua troppo bassa: (id, metri)
    const std::vector<std::pair<int, double>>& spostamenti() const { return spostamenti_; }
    // Carte d'epoca delle due parti (docs/08 §5, tools/pipeline/carta_epoca.py, percorso_carta): servono solo alle
    // stazioni del giocatore, la simulazione resta sul fondale vero. Senza carta impostata fondale_carta restituisce
    // il fondale vero (partita con il «fondale vero»).
    void imposta_fondale_carta(Parte p, std::shared_ptr<const Fondale> carta) {
        carte_[static_cast<int>(p)] = std::move(carta);
        acque_carta_.clear();
        carte_prudenti_.clear();
    }
    const Fondale* fondale_carta(Parte p) const {
        const auto& c = carte_[static_cast<int>(p)];
        return c ? c.get() : fondale();
    }

    // --- usate dalle funzioni m688 (pubbliche per i binding C) ---
    struct Contesto {
        int esecutore;
        int selezionata;
        UnitaClassica buffer;
        std::optional<std::pair<int, Contatto>> contatto_lancio = {};   // LAUNCH: il contatto che l'arma eredita
        std::string gestore = {};   // script in esecuzione (per riconoscere l'arma di un lancio)
    };
    Contesto& contesto();
    UnitaClassica* trova(int id);
    void carica_buffer(int id);
    void salva_buffer();
    int nuova_unita_da_buffer();
    void programma(double dopo_s, int unita, const std::string& gestore, bool assoluto);
    void annulla_timer(int unita, const std::string* gestore);
    void messaggio(int unita, int codice);
    // un messaggio di bordo dell'Enhanced (mine 364-369...): nella lista, ma non chiude la missione come i codici
    // da 100 in su degli script dell'originale
    void messaggio_di_bordo(int unita, int codice) { messaggi_.push_back({tempo_, unita, codice}); }
    void evento_acustico(int unita, double durata_s);
    void registra_lancio_missile(int unita);   // MISSILE: per gli effetti (lanci_missili())
    // ATTACK e ATTACK_RND (2011:1CDE): danno sulla vittima, affondamento, contatore delle uccisioni e messaggi
    void applica_attacco(UnitaClassica& vittima, int danno, Colpo::Tipo tipo);
    void togli(UnitaClassica& u);                              // l'unita' esce dal gioco (2011:003C)
    int conto_passi() const { return conto_5100_; }         // DS:5100: da 18 a 1, uno per passo
    void registra_colpo(const Colpo& c) { colpi_.push_back(c); }
    // Enhanced: una salva antisom (ATTACK) dalla dotazione dell'unita' (RBU, tubi, ASROC: per classe, STIMA); falso se finite
    bool salva_asw(const UnitaClassica& u);

    // --- regola dei danni (docs/10_danni_affondamento_esplosioni.md §7) ---
    // Livello classico (predefinito): decide l'integrita' dell'originale. Con la regola di simulazione le unita' indicate
    // (navi con la fisica dei danni) ricevono i colpi ma la distruzione la decide il motore con distruggi().
    void affida_alla_fisica(int id) { fisica_.insert(id); }
    // Enhanced (regola di simulazione, fase C6a): i sottomarini si muovono con la dinamica della loro classe
    // (nucleo/dinamica_sottomarino.hpp) invece che col moto del livello classico. Le unita' con comandi propri (il
    // giocatore) li seguono; le altre hanno un timoniere che insegue rotta, velocita' e quota ordinate dagli script
    void dinamica_sottomarini(const std::map<std::string, ClasseNave>& classi, const std::string& cartella_dati);
    bool con_dinamica() const { return dinamica_attiva_; }
    void comandi_sottomarino(int id, const ComandiSottomarino& c) { comandi_sub_[id] = c; }
    void comandi_del_timoniere(int id) { comandi_sub_.erase(id); }
    const DinamicaSottomarino* dinamica(int id) const;
    // Enhanced (regola di simulazione): la scoperta sonar con l'equazione del sonar di nucleo/acustica.hpp
    // (data/motore/acustica.toml e teatri/<teatro>.suono.toml); radar, vista, ESM e armi proprie restano come nel
    // classico. Il contatto integra l'eccesso di segnale nel tempo; il ping e' un impulso con eco e contro-scoperta.
    // Messaggi 310-319 per il giocatore in messaggi_sonar() (fuori da messaggi(): i codici >= 100 sono esiti).
    void acustica_enhanced(const std::string& cartella_dati);
    // Sala radio dell'Enhanced (pezzo S4, nucleo/radio.hpp) per il giocatore: broadcast, filo galleggiante,
    // trasmissioni e radiogoniometria. Ordini di missione dal briefing (serie 0), traffico di fondo, messaggi
    // facoltativi di data/missioni/radio/<ID>.lua (`{ { dopo_s, precedenza, da, a, oggetto, classifica,
    // testo = { it, en }, gestore } }`): ricevuto, il gestore cambia gli ordini. Un fix della radiogoniometria nemica
    // da' alle unita' avversarie un contatto sul giocatore (subito il rilevamento a chi ha l'ESM vicino; dopo il
    // ritardo la posizione del fix a tutte). Avvisi 315-329 in messaggi_radio().
    void radio_enhanced();
    // R3 ed E6: un messaggio radio della missione (file radio o chiave `radio`) per id, messo in onda subito
    // (m688.radio dagli script e dalle regole); falso senza la sala radio, senza quel messaggio o se e' gia' in coda
    bool manda_radio(const std::string& id);
    // data e ora UTC d'inizio della missione per il DTG e le finestre (le missioni non le hanno: 24/10/1989, 00Z); da
    // chiamare prima del primo passo con la regola di simulazione, cioe' prima che nasca la sala radio
    // la data e l'ora della partita: prevalgono anche sulla data degli `ordini` del file radio (R3)
    void imposta_data_ora(int giorno, int mese, int anno, double ora_utc_h) {
        giorno_ = giorno; mese_data_ = mese; anno_ = anno; ora_utc_s_ = ora_utc_h * 3600.0;
        data_imposta_ = true;
    }
    SalaRadio* radio() { return radio_.get(); }
    const SalaRadio* radio() const { return radio_.get(); }
    void radio_filo(bool cala);                              // cala (1) o recupera (0) il filo galleggiante
    EsitoTrasmissione radio_trasmetti(TipoTrasmissione tipo);
    EsitoTrasmissione radio_ssixs() { return radio_trasmetti(TipoTrasmissione::richiesta); }
    // boa SLOT (fase R1, dossier 10 §4.4): trasmette il rapporto di contatto dal punto di lancio dopo il ritardo, solo
    // verso un aereo amico entro la portata VHF; il fix nemico cade sulla boa. Falso senza boe, con il ritardo fuori dai
    // limiti del file radio o senza la sala radio
    bool radio_slot(double ritardo_s);
    int boe_slot() const { return slot_.boe; }
    std::optional<double> slot_in_attesa() const;
    // R2: zone dello spazio d'acqua che conosce il giocatore, con la forma calcolata adesso (le mobili seguono il loro
    // battello); verifica del gruppo d'autenticazione di un messaggio ricevuto (falso: avviso 339)
    struct ZonaCarta {
        Zona zona;
        std::vector<Vec2> vertici;          // poligono, o il rettangolo della mobile (vuoto per un cerchio)
        std::optional<Vec2> centro;         // cerchio, o il battello della mobile
        double raggio_m = 0.0, orientamento = 0.0;   // orientamento: la rotta del battello della mobile
    };
    std::vector<ZonaCarta> zone_ricevute() const;
    bool radio_verifica(int serie);
    // categoria delle regole d'ingaggio di un'unita' (neutrali dal file radio, mercantili per classe)
    CategoriaRoe categoria_roe(const UnitaClassica& u) const;
    const std::vector<Messaggio>& messaggi_radio() const { return messaggi_radio_; }
    // ASW aereo dell'Enhanced (docs/13_asw_aereo.md, A3; nucleo/asw_aereo.hpp e nucleo/acustica_asw.hpp): con la regola
    // di simulazione ogni aereo la cui classe e' in <cartella_dati>/asw_aereo.toml passa a un ControlloreAsw. La
    // configurazione viene dalla chiave `aereo_asw` della missione o da data/missioni/asw/<ID>.lua (le originali):
    // missione (caccia, ricognizione, pattuglia), situazione, bersagli, armi, carico, base, zona, cue (docs/09 §5.4).
    // Lo script decide il quando (la rotta di pattuglia, vai_al_bersaglio diventa un datum), il controllore il come:
    // fuori dalla pattuglia guida rotta e velocita' dell'aereo. La traccia del controllore diventa il contatto
    // dell'aereo per gli script (traccia solida col fix). Boe, dip, transitori, armi col paracadute e marcatori; avvisi
    // 380-389 in messaggi_asw(). Senza il file dei dati resta il classico.
    void asw_enhanced(const std::string& cartella_dati, DifficoltaAsw difficolta = DifficoltaAsw::normale);
    bool con_asw() const { return asw_ != nullptr; }
    // Artiglieria navale dell'Enhanced (docs/16_artiglieria.md, G2; nucleo/artiglieria.hpp). Le navi armate sparano al
    // giocatore esposto, in superficie o con una testa o un albero fuori, visto da radar o vedetta con traccia solida.
    // Contano situazione (pace: mai; crisi: avvertimenti e illuminanti; guerra: al bersaglio; un lancio del giocatore fa
    // della crisi una guerra), prontezza e reazione, portata, fuoco amico, munizioni e la difficolta' dell'ASW. Senza
    // data/motore/artiglieria.toml resta spenta
    void artiglieria_enhanced(const std::string& cartella_dati);
    bool con_artiglieria() const { return artiglieria_ != nullptr; }
    void usa_artiglieria(bool si) { artiglieria_spenta_ = !si; }
    bool artiglieria_avvertimento() const;   // le navi sparano solo d'avvertimento (crisi senza provocazione)
    // la nave va al posto di combattimento: la reazione del pezzo e' la piu' breve (22 §4.2; m688.posto_di_combattimento)
    void posto_di_combattimento(int nave);
    // con la chiave `artiglieria = { fra_navi = true }`, in guerra le navi sparano anche alle navi ostili che hanno con
    // traccia solida e posizione (le battaglie di superficie lontane del dossier 22 §9.1): la sagoma della nave
    SagomaEsposta sagoma_nave(const UnitaClassica& u) const;
    // Crescenti come colpi(): gli impatti all'istante dell'impatto (gli avvertimenti compresi, avvertimento = true) e gli
    // eventi per audio e grafica al loro istante (la partenza allo sparo, gli illuminanti all'apertura)
    const std::vector<ImpattoArtiglieria>& impatti_artiglieria() const { return impatti_artiglieria_; }
    const std::vector<EventoArtiglieria>& eventi_artiglieria() const { return eventi_artiglieria_; }
    // Cio' che del sottomarino sta fuori dall'acqua (vuota se niente): scafo e falsatorre in superficie, teste e alberi
    // alzati, la vela che affiora col mare grosso
    SagomaEsposta sagoma_esposta(const UnitaClassica& u) const;
    // Forze dell'Enhanced (fase 3 delle interazioni fra IA, docs/18; dossier 24 §8.1, §4.6, §8.7; nota condivisa
    // ia-fase3-contratto.md). Una forza e' una parte (lato: 0x08 usa, 0x10 urss, 0 neutrali). Allerta A0-A3 della forza
    // e della formazione (le unita' della parte entro 20 km da chi vive l'evento), mossa dagli eventi e in discesa col
    // tempo; situazione per coppia di parti, che in crisi un atto ostile (lancio, colpo, salva) porta in guerra; regole
    // d'ingaggio dell'IA sulla parte creduta del bersaglio. Senza: allerta e armi_libere_ia non hanno valore
    void forze_enhanced(const std::string& cartella_dati);
    bool con_forze() const { return forze_ != nullptr; }
    void usa_forze(bool si) { forze_spente_ = !si; }   // prima del primo passo: per i confronti
    // La sandbox (docs/19, V1; nota condivisa sandbox-v1-runtime-contratto.md), tutto prima del primo passo.
    // Gli interruttori di confronto dell'IA in una volta (quelli della taratura)
    struct InterruttoriIa {
        bool forze = true, difesa = true, armi = true, reazioni = true, esm = true, sonaristi = true, evasione = true,
             esche_mobili = true;
    };
    void interruttori_ia(const InterruttoriIa& i);
    // Le regolazioni di una parte (lato: 0x08 usa, 0x10 urss, 0 neutrali). La situazione di partenza vale per le
    // decisioni di quella parte finche' in partita la coppia non cambia (dichiarazione, provocazione): le due parti
    // possono vederla diversa. Le ROE per categoria vengono prima della situazione (una categoria trattenuta non si
    // attacca nemmeno in guerra, una libera si attacca anche in pace) e per la parte del giocatore sono anche quelle
    // di partenza della sua sala radio. Livello, allerta e carico: V2
    struct RegolazioniParte {
        std::optional<Situazione> situazione;
        std::optional<std::map<CategoriaRoe, bool>> roe;   // vero: armi libere sulla categoria
        std::optional<Esperienza> livello;   // V2: sonaristi e filo di tutte le unita' dell'IA della parte
        std::optional<int> allerta;          // V2, A0-A3 di partenza della forza e delle formazioni (sopra il minimo)
        bool carico_per_classe = false;   // V2: il battello del giocatore col carico standard (main, SalaSiluri::carico_standard)
    };
    void regolazioni(const std::map<int, RegolazioniParte>& per_lato) { regolazioni_ = per_lato; }
    // la ROE regolata di una parte per la categoria (nullopt senza regolazione): artiglieria e ASW aereo la seguono
    std::optional<bool> roe_parte(int lato, CategoriaRoe c) const;
    const std::map<int, RegolazioniParte>& regolazioni() const { return regolazioni_; }
    // La partita senza giocatore: il battello del giocatore lo comanda il comandante IA (missioni/comandante_ia.hpp);
    // niente attese di input, e fuori carriera (sandbox())
    void senza_giocatore(bool si) { senza_giocatore_ = si; }
    bool sandbox() const { return senza_giocatore_; }
    // Il vetrino di Dio (docs/28, D1; missioni/interventi.hpp, missione_interventi.cpp): lo spettatore interviene. Solo
    // nell'Enhanced e nella sandbox. intervieni valida, assegna l'id e mette in coda: si applica all'inizio del passo
    // (subito: il prossimo; a tempo: il primo passo con tempo_ >= x.tempo)
    EsitoIntervento intervieni(const Intervento& x);
    bool annulla_intervento(int id);                       // solo prima che sia applicato
    const std::vector<Intervento>& interventi() const { return interventi_; }       // gli accettati, in ordine d'id
    const std::vector<StatoIntervento>& stati_interventi() const { return stati_interventi_; }   // stesso ordine
    std::vector<const Intervento*> interventi_attivi(double t) const;   // applicati e non finiti a t (i segni sulla carta)
    // il falso contatto della D3 (docs/28 §3.5, intervento contatto_falso): una sorgente con la firma di una classe e senza
    // unita' vera, id 990000-994999. Per lo spettatore (il segno «falso»); le unita' dell'IA non lo sanno
    bool contatto_falso(int id) const;
    // D4, il cambio di parte (intervento cambia_parte): la parte di `b` che `o` conosce. Chi non sa ancora del cambio (ne' lui
    // ne' la sua parte) la crede quella di prima; senza cambi e' la parte vera (flag & 0x18)
    int parte_nota(const UnitaClassica& o, const UnitaClassica& b) const;
    // la parte che l'unita' `osservatore` crede di `unita`, con la classificazione del suo sonarista (lato_creduto), per la
    // doppia freccia dello spettatore; -1 di nessuno o ignota, -2 senza una delle due unita'
    int parte_creduta(int osservatore, int unita) const;
    // Le fasi del comandante IA (missioni/comandante_ia.hpp), per il registro (evento comandante_ia): non cambiano la partita
    struct NotaComandante {
        double t;
        std::string fase, motivo;
        int bersaglio;
    };
    void nota_comandante(double t, const std::string& fase, const std::string& motivo, int bersaglio) {
        note_comandante_.push_back({t, fase, motivo, bersaglio});
    }
    const std::vector<NotaComandante>& note_comandante() const { return note_comandante_; }
    // Sandbox V2 (nota sandbox-v2-runtime-contratto.md, R1-R6): i fatti di tutte le unita' per le statistiche tecniche
    // (nucleo/statistiche_sandbox), liste crescenti col loro tempo. Si registrano solo se accese prima del primo passo
    void registra_statistiche(bool si) { statistiche_ = si; }
    bool con_statistiche() const { return statistiche_; }
    struct ScopertaContatto {      // R1: al passo esatto
        double t;
        int osservatore, bersaglio;
        enum class Tipo : std::uint8_t { nuovo, solido, perso } tipo;
        std::string sensore;       // del contatto (perso: l'ultimo)
        double distanza_m, eccesso_db;   // vera
    };
    const std::vector<ScopertaContatto>& scoperte_contatti() const { return scoperte_contatti_; }
    struct ScopertaPossibile {     // R2: una volta per (osservatore, bersaglio, sensore)
        double t;
        int osservatore, bersaglio;
        std::string sensore;       // id del sensore acustico, "radar", "vista", "esm"
        double distanza_m, eccesso_db;   // l'eccesso dell'istante (0 per radar, vista ed ESM)
    };
    const std::vector<ScopertaPossibile>& scoperte_possibili() const { return scoperte_possibili_; }
    struct StimaLancio {           // quello che il lanciatore sapeva del bersaglio al lancio
        Vec2 pos;
        std::optional<double> rotta, velocita_kn;   // dalla soluzione della TMA
        double sigma_distanza_m = 0.0, sigma_trasversale_m = 0.0;
        std::string da;            // "tma", "contatto", "giocatore"
    };
    struct LancioArma {            // R3: siluri del modello e missili, di tutti
        double t;
        int lanciatore, arma, bersaglio;
        std::string classe_arma;
        std::optional<StimaLancio> stima;
    };
    const std::vector<LancioArma>& lanci() const { return lanci_; }
    // la soluzione della TMA del giocatore sul bersaglio, per la stima del suo prossimo lancio (main, prima dell'ordine)
    void soluzione_giocatore(int bersaglio, const StimaTma& s) { soluzione_giocatore_ = {bersaglio, s}; }
    struct ManovraEvasione {       // R4: l'inizio di ogni manovra d'evasione di un sottomarino dell'IA
        double t;
        int unita, minaccia;       // minaccia: l'arma, l'aereo o il pinger (-1 se ignota)
        std::string causa;         // ping_lontano, ping_vicino, tonfo, sorvolo, siluro, siluro_vicino
    };
    const std::vector<ManovraEvasione>& evasioni() const { return evasioni_; }
    struct Emissione {             // R5
        double t;
        int unita;
        std::string tipo;          // ping, radar, radio
        double durata_s;           // ping: l'impulso; radar: fino allo spegnimento (-1 se ancora acceso); radio: 0
        std::string dettaglio;     // ping: il sensore; radar: il radar; radio: il canale
    };
    const std::vector<Emissione>& emissioni() const { return emissioni_; }
    // Il quadro di una parte (docs/19 §3, lettura «parte»): quello che la parte sa nell'insieme, una voce per bersaglio
    // col dato migliore fra i contatti delle sue unita' e i rapporti radio ricevuti. Nell'ordine: la posizione da un
    // sensore, la soluzione della TMA dell'IA, il rapporto radio con la posizione, il solo rilevamento piu' fresco. Senza
    // armi in acqua ne' unita' della parte stessa. Lato: 0x08, 0x10, 0 neutrali (0x18 vale 0)
    struct VoceQuadro {
        int bersaglio = -1;          // l'unita' vera (per unire le fonti nella vista: la parte non lo sa)
        bool posizione = false;      // con la posizione stimata; se no un rilevamento da `origine`
        Vec2 pos, origine;
        double rilevamento = 0.0;
        double semiasse_maggiore_m = 0.0, semiasse_minore_m = 0.0, orientamento_deg = 0.0;   // a 1 sigma, 0 se ignota
        double t_dato = 0.0;         // eta' = adesso - t_dato
        std::string sensore;         // del contatto, "tma" per la soluzione, "radio" per il rapporto
        int fonte = -1;              // l'unita' della parte col dato migliore (o che l'ha riferito)
        std::vector<int> altre_fonti;
        std::string categoria, classe;   // credute; classe vuota se non classificato
        double qualita = 0.0;        // del contatto (0 per la radio)
    };
    std::vector<VoceQuadro> quadro_parte(int lato) const;
    // Navigazione incerta (roadmap 26, docs/08 §6; nucleo/navigazione.hpp; nota navigazione-aggancio-contratto.md).
    // Spenta di norma; accesa (prima del primo passo, con la regola di simulazione) il giocatore sa dov'e' solo per
    // stima: l'inerziale (o la stima a inerziale guasto) deriva, i punti nave (ordine punto_nave, testo = id del
    // punto) la riportano a posto alle loro condizioni e coi loro costi; il pilota automatico naviga sulla stima. La
    // chiave di missione `navigazione = { sigma_iniziale_nm, inerziale_guasto, gps }` vale con l'interruttore acceso
    void usa_navigazione(bool si) { navigazione_attiva_ = si; }
    bool con_navigazione() const { return nav_.has_value(); }
    Vec2 posizione_stimata(int id) const;   // la stima del giocatore e dei sottomarini dell'IA; delle altre unita' la vera
    // la posizione su cui lo script dell'unita' corrente prova le zone (in_rettangolo, in_cerchio, x_maggiore): la
    // stima per un sottomarino dell'IA col navigatore, fuori dai gestori d'esito; altrimenti la vera
    Vec2 posizione_per_script(const UnitaClassica& u) const;
    const GestoriEsito& gestori_esito() const { return gestori_esito_; }
    void meta_assoluta(int id, Vec2 meta) { mete_assolute_[id] = meta; }   // vai_a: un punto fisso, non un'unita'
    std::optional<EllisseErrore> ellisse_navigazione(int id, double k = 1.0) const;   // a 1 sigma per k = 1
    std::optional<double> sigma_navigazione_m(int id) const;
    // Quadro del fondale del giocatore nell'Enhanced (docs/24 §3; missione_fondale.cpp): la carta d'epoca della sua parte
    // (o il fondale vero, con la carta «vera») e le misure dell'ecoscandaglio (ordini ecoscandaglio_ping, _periodo,
    // _potenza) e del BQS-15 acceso, messe sulla posizione stimata; i punti nave fanno scorrere il mosaico. Ogni ping va
    // ai ricevitori d'intercettazione; gli avvisi dell'equipaggio (messaggi 480-485) nascono solo dalle misure. L'IA
    // non lo usa ancora: il suo pilota sta sul fondale vero
    const QuadroFondale* quadro_fondale(int id) const;
    // Sotto il ghiaccio (opzione A di sotto-ghiaccio-piano.md; missione_ghiaccio.cpp): il campo vero dal seme sulla
    // copertura del meteo; il quadro della volta del giocatore e dei sottomarini dell'IA col sonar del fondo (sondatore
    // alto e BQS-15 sulle chiglie); sotto_banchisa: ghiaccio sopra e non acqua libera (le regole di bordo: niente alberi
    // ne' periscopio, radio solo ELF, niente punto nave con l'antenna)
    const CampoGhiaccio* ghiaccio() const;
    // Il vetrino di Dio, D4 (docs/28 §3.3; missione_ghiaccio.cpp). Il campo da cambiare, fatto se non c'era (nullptr: senza
    // acustica o senza meteo): CampoGhiaccio::cambia_deriva e forza_zona. La copertura nel punto per chi deve sapere se c'e'
    // ghiaccio: senza cambi e' quella del meteo, bit per bit
    CampoGhiaccio* ghiaccio_da_cambiare();
    double copertura_ghiaccio(Vec2 p) const;
    // Lo strato che cambia, in tutto il teatro o in un cerchio (raggio_m 0: tutto), a transizione a 4 passi: un passo nuovo e'
    // un profilo nuovo, quindi le cache dei raggi ricalcolano a un istante fisso. Un percorso sta nella zona se ci sta il suo
    // punto medio. Senza zone il profilo di sempre, bit per bit. `applicabile` prima di aggiungerla (modifica_applicabile:
    // le acque polari non hanno un termoclino da spostare)
    struct ZonaStrato {
        ModificaStrato modifica;
        Vec2 centro;
        double raggio_m = 0.0;
        double t0 = 0.0, transizione_s = 0.0, durata_s = 0.0;
    };
    bool strato_applicabile(ModificaStrato::Tipo t) const;
    void aggiungi_strato(const ZonaStrato& z) { zone_strato_.push_back(z); }
    const ProfiloSuono* profilo_in(Vec2 a, Vec2 b) const;   // quello che usa ambiente_acustico (nullptr senza clima)
    double sld_in(Vec2 p) const;                            // la SLD per chi sceglie una quota (ASW, VDS)
    bool sotto_banchisa(int id) const;
    const QuadroVolta* quadro_volta(int id) const;
    // i falsi echi delle chiglie dell'ultimo ping attivo del giocatore sotto la banchisa (missione_ghiaccio.cpp): fuori dai
    // contatti veri, per la vista (la stazione Sonar e la carta li mostrano come echi che svaniscono al ping dopo). Vuoto
    // senza ghiaccio. `punto` e `pescaggio_m` sono la verita' (per il debriefing), non per la vista
    struct EcoFalso {
        double t = 0.0, rilevamento = 0.0, distanza_m = 0.0, eccesso_db = 0.0;
        Vec2 punto;
        double pescaggio_m = 0.0;
    };
    const std::vector<EcoFalso>& echi_falsi_attivo() const { return echi_falsi_; }
    struct StatoEcoscandaglio {
        double periodo_min = 0.0;            // 0: a comando
        PotenzaEco potenza = PotenzaEco::piena;
        std::optional<Sondaggio> ultimo;
    };
    std::optional<StatoEcoscandaglio> ecoscandaglio(int id) const;   // nullopt senza ecoscandaglio (classico, IA)
    const NavigazioneStimata* navigatore() const { return nav_ ? &*nav_ : nullptr; }
    struct PuntoNaveInCorso {
        std::string tipo;
        double attesa_s = 0.0;       // al passaggio del satellite (0: non e' un satellite, o e' passato)
        double rimasto_s = 0.0;      // dell'esposizione, dal passaggio
        MotivoPuntoNave motivo = MotivoPuntoNave::nessuno;   // l'ultimo punto nave non possibile o interrotto
        bool in_corso = false;
    };
    std::optional<PuntoNaveInCorso> punto_nave_in_corso() const;   // nullopt senza navigazione
    // i punti nave della marina del giocatore con la valutazione di adesso (possibile, o il primo motivo che manca)
    std::vector<std::pair<std::string, ValutazionePuntoNave>> punti_nave_possibili() const;
    struct EsitoPuntoNave {
        double t;
        std::string tipo;
        bool fatto;
        MotivoPuntoNave motivo;     // non fatto: perche'
        double spostamento_m, sigma_prima_m, sigma_dopo_m;
        int unita = -1;             // R6: chi l'ha fatto (punti_nave_ia)
    };
    const std::vector<EsitoPuntoNave>& punti_nave() const { return punti_nave_; }
    const std::vector<EsitoPuntoNave>& punti_nave_ia() const { return punti_nave_ia_; }   // R6, con registra_statistiche
    // lo stato dell'inerziale del giocatore dai danni (DanniSottomarini, Sistema::inerziale del kit): giu', si naviga a stima
    void inerziale_giocatore(bool ok) { inerziale_danni_ok_ = ok; }
    // lo stesso per i sottomarini dell'IA col navigatore: giu', il loro navigatore va a stima (simmetria)
    void inerziale_ia(int id, bool ok) { if (ok) inerziale_ia_giu_.erase(id); else inerziale_ia_giu_.insert(id); }
    // il sonar di scafo e la schiera dell'IA dai danni (LimitiBattello::sonar e ::schiera), come per il giocatore
    void sonar_ia(int id, bool sonar_ok, bool schiera_ok) {
        if (sonar_ok) sonar_ia_giu_.erase(id); else sonar_ia_giu_.insert(id);
        if (schiera_ok) schiera_ia_giu_.erase(id); else schiera_ia_giu_.insert(id);
    }
    // l'opzione di partita «1990+» (menu dell'Enhanced): il GPS c'e', come con la chiave di missione `navigazione.gps`
    void usa_gps(bool si) { gps_opzione_ = si; }
    // la schiera del giocatore dai danni (LimitiBattello::schiera): guasta, anche filata non ascolta
    void schiera_giocatore(bool ok) { schiera_danni_ok_ = ok; }
    // Difesa dell'IA (fase 4 A, docs/18; nota condivisa ia-fase4-contratto.md): i sottomarini dell'IA evadono (nucleo/
    // evasione) e lanciano esche su qualsiasi siluro ostile sentito, ping ostile intercettato o transitorio di lancio
    // (con P 0,5 l'occhiata lungo il rilevamento); le navi si allontanano a tutta forza da un siluro sentito; il
    // sottomarino colpito si ritira da chi ha lanciato; anticollisione dei sottomarini. Comanda con l'ordine del motore
    void usa_difesa(bool si) { difesa_spenta_ = !si; }
    struct EscaIa {
        double t;
        int unita, esca;
        std::string tipo;   // adc_mk2 (esca fissa), mg74 (mobile)
    };
    const std::vector<EscaIa>& esche_ia() const { return esche_ia_; }
    struct RitirataIa {
        double t;
        int unita, da;        // da chi (il lanciatore, il siluro, l'altra unita'), -1 se ignoto
        std::string causa;    // colpito, siluro, collisione
    };
    const std::vector<RitirataIa>& ritirate_ia() const { return ritirate_ia_; }
    // Il pedinamento ravvicinato (S24, missione_pedinamento.cpp): il sottomarino dell'IA u si tiene nel cono sordo del
    // sottomarino b, a distanza_m, alla sua quota vista piu' sopra_m (negativo: sotto), e ne segue i cambi col ritardo
    // con cui li vede; b < 0 smette. Qualunque parte, Enhanced. Il cono sordo: la semiapertura a poppa dai sensori in
    // ascolto di b (baffles_gradi; la schiera filata lo restringe al suo endfire)
    bool pedina_vicino(int u, int b, double distanza_m = 400.0, double sopra_m = 0.0);
    bool in_pedinamento_vicino(int u, int b = -1) const;
    double cono_sordo_gradi(int b) const;
    // Armi dell'IA (fase 4 B): i sottomarini dell'IA con i dati (carico.toml per 688 e Alfa, [armi_ia.*] per gli altri)
    // hanno il loro carico dell'anno; un lancio dello script prende dai tubi il siluro con piu' stelle contro il
    // bersaglio (nucleo/armi_ia, scegli_arma_ia), il tubo si ricarica in 5 minuti dalla riserva, senza siluri in tubo
    // il lancio non parte. Su un contatto ostile col solo rilevamento, con le armi libere o l'ingaggio dello script, il
    // sottomarino fa le gambe di TMA (nucleo/manovre_tma) fino alla soluzione (m688.soluzione); non nelle missioni
    // originali, e mai nello stesso istante di un ordine del suo script. I filoguidati dell'IA li guida GuidaFiloIa
    // verso il contatto del lanciatore, che intanto tiene i vincoli del filo
    void usa_armi_ia(bool si) { armi_ia_spente_ = !si; }   // prima del primo passo: per i confronti
    // Il quadro radio ai sottomarini dell'IA (missione_forze.cpp, passo_radio_sottomarini): spento, restano le regole di prima
    void usa_quadro_sottomarini(bool si) { quadro_sottomarini_spento_ = !si; }   // prima del primo passo: per i confronti
    bool con_armi_ia() const { return armi_ia_ != nullptr; }
    std::optional<int> armi_rimaste(int id, const std::string& famiglia = {}) const;   // nei tubi e di riserva
    std::optional<int> tubi_carichi(int id) const;   // i tubi con un'arma, dell'IA col carico (nullopt senza)
    // L'attacco forzato di un sottomarino dell'IA su un'unita' precisa (m688.attacca_unita, libreria I03 e I05): ignora
    // parte, ROE e non combattenti. Lancia dai tubi quando ha la posizione del bersaglio (la soluzione), o subito con
    // `senza_soluzione` (il tiro pianificato d'esercitazione: sul rilevamento e la distanza veri); un siluro solo, poi
    // l'ordine finisce. Falso senza armi dell'IA o senza carico
    bool attacca_unita(int id, int bersaglio, bool senza_soluzione);
    // la TMA a gambe in corso di un sottomarino dell'IA (sandbox V2, lettura «unita'»): il bersaglio, le gambe fatte e la
    // soluzione se c'e'; senza gambe in corso, la soluzione sul bersaglio dello script se la sua TMA ce l'ha
    struct TmaCorrente {
        int bersaglio = -1;
        int gambe = 0;
        std::optional<StimaTma> soluzione;
    };
    std::optional<TmaCorrente> tma_ia(int id) const;
    // la dottrina della TMA a gambe della marina dell'unita' (interazioni.toml [tma]); quella di base senza le armi dell'IA
    DottrinaTma dottrina_tma_ia(int id) const;
    // la dottrina del filo dell'IA per il livello (interazioni.toml [armi_ia]); quella di base senza le armi dell'IA
    DottrinaFilo dottrina_filo_ia(Esperienza e) const;
    const CaricoIa* carico_ia_di(int id) const;
    // i kg d'esplosivo che l'unita' ha ancora nel locale siluri (esplosivo_a_bordo di danni_sottomarini.hpp): il carico
    // vivo del deposito per DanniSottomarini. Nullopt senza il carico dell'IA
    std::optional<double> esplosivo_ia(int id) const;
    struct EventoTmaIa {
        double t;
        int unita, bersaglio, gambe;
        enum class Tipo : std::uint8_t { gamba, soluzione, rinuncia } tipo;
        std::optional<StimaTma> stima;   // R3: la soluzione della TMA dell'IA sul bersaglio, nel tipo soluzione
    };
    const std::vector<EventoTmaIa>& eventi_tma_ia() const;
    // Reazioni per situazione (fase 4 C, dossier 24 §8.4): su un alleato attaccato o affondato e su un sottomarino
    // nemico classificato, la formazione della parte (entro 20 km) reagisce dopo il ritardo della voce, per ruolo: la
    // nave antisom piu' vicina va sul datum o tiene sotto il contatto con l'attivo, le altre navi si allontanano, i
    // sottomarini amici si fermano ad ascoltare o coprono, gli aerei dell'ASW aereo hanno il datum. Non nelle originali;
    // nell'istante di un ordine dello script vince lo script
    void usa_reazioni(bool si) { reazioni_spente_ = !si; }
    struct ReazioneIa {
        double t;
        int unita, causa;            // causa: l'unita' attaccata, affondata o il contatto
        std::string ruolo;           // asw, nave, sottomarino, aereo
        std::string reazione;        // datum, tiene_sotto, allontana, ascolta, copre, cue, pedina (D)
        std::string evento;          // attaccato, affondato, scoperto; script (il pedinamento del giocatore per script)
    };
    const std::vector<ReazioneIa>& reazioni_ia() const { return reazioni_ia_; }
    // Pace e INCSEA (fase 4 D, dossier 24 §8.5): in pace la nave antisom e il sottomarino dell'IA che hanno un
    // sottomarino ostile classificato lo pedinano a distanza (nave 2-5 km; sottomarino 2-10 km con la schiera, 500-2000
    // m senza), la nave con il ping «ti tengo» (P del `ping` dell'allerta a ogni minuto); un sottomarino dell'IA che
    // riceve un ping ostile entro 2 nm in pace si allontana con P 0,7; le navi di parti diverse tengono un CPA di almeno
    // 1000 yd (INCSEA 1972), e gli aerei non passano sotto i 500 ft entro 1 nm da una nave dell'altra parte (l'ASW
    // aereo dell'IA tiene la quota), salvo la chiave della missione `incsea = false` (lo scenario d'urto voluto)
    struct ViolazioneIncsea {
        double t;
        int unita, altra;
        double distanza_m;
    };
    const std::vector<ViolazioneIncsea>& incsea() const { return incsea_; }
    // la situazione fra due parti se le forze la sanno davvero: dichiarata dalla missione (carriera, editor, originali)
    // o cambiata in partita; senza, chi la usa tiene la sua (l'ASW aereo e l'artiglieria hanno i loro predefiniti)
    std::optional<Situazione> situazione_nota(int lato_a, int lato_b) const;
    std::optional<int> allerta(int lato) const;          // della forza
    std::optional<int> allerta_unita(int id) const;      // il maggiore fra quello della sua formazione e della forza
    Situazione situazione_fra(int lato_a, int lato_b) const;   // senza forze: la base della missione
    // la missione (m688.situazione) dichiara la situazione fra due parti, o fra tutte con -1; alza i minimi d'allerta
    void dichiara_situazione(Situazione s, int lato_a = -1, int lato_b = -1);
    // l'IA id puo' usare le armi sul bersaglio (-1: sul suo bersaglio corrente; senza: sulla parte ostile); nullopt
    // senza forze. Non const: la P dei neutrali ad A3 si estrae una volta per coppia
    std::optional<bool> armi_libere_ia(int id, int bersaglio = -1);
    struct CambioAllerta {
        double t;
        int lato, unita;   // unita': chi ha vissuto l'evento (la sua formazione); -1: la forza intera
        int da, a;
        std::string causa;  // possub, probsub, albero, ping, arma, affondato, flash, situazione, discesa
    };
    struct Provocazione {
        double t;
        int colpito, provocatore;   // lati
        int unita, bersaglio;       // chi ha fatto l'atto e su chi
        std::string causa;          // lancio, colpo, salva
    };
    // Messaggi di contatto fra unita' dell'IA (fase 3 B, dossier 24 §8.2): un'IA riferisce un contatto ostile quando lo
    // ha (traccia solida o posizione), quando lo classifica di piu' e ogni 10 minuti con la posizione. Canali: la
    // formazione (voce, Link 11; dall'aereo alla nave), il teatro (FLASH o IMMEDIATE), i sottomarini al broadcast con
    // l'antenna o a quota periscopica, il giocatore della stessa parte come contatto d'intelligence dalla sua radio.
    // Chi riceve ha il rapporto nel suo quadro radio, con l'ellisse r0 + u t: e' quello che usa per decidere
    struct RapportoContatto {
        int da = -1, su = -1;          // chi riferisce, su chi (verita', per il registro)
        double t_fix = 0.0;
        std::optional<Vec2> pos;       // la posizione stimata, se chi riferisce l'aveva; se no il rilevamento
        Vec2 origine;                  // da dove e' stato preso il rilevamento
        double rilevamento = 0.0;
        std::string categoria;         // creduta: sottomarino, nave, aereo, sconosciuto
        std::string classe;            // creduta, vuota se non classificato
        int livello = 0;               // 0 contatto, 1 categoria, 2 classe
        double errore_m = 0.0;         // l'errore di un fix della rete; 0: quello di ogni rapporto (r0_m)
    };
    struct ContattoRadio {
        RapportoContatto r;
        double t_arrivo = 0.0;
        double raggio_m = 0.0;         // dell'ellisse adesso
        bool posizione_valida = false; // con la posizione e l'ellisse sotto la soglia
    };
    std::optional<ContattoRadio> contatto_radio(int ricevente, int bersaglio) const;
    std::vector<std::pair<int, ContattoRadio>> quadro_radio(int ricevente) const;   // (bersaglio, contatto)
    struct MessaggioIa {
        double t_partenza = 0.0, t_arrivo = 0.0;
        int da = -1, su = -1;
        int verso = -1;                // -1: la formazione o la forza; >= 0 un'unita'
        enum class Canale : std::uint8_t { formazione, aereo_nave, teatro, broadcast, giocatore } canale = Canale::formazione;
        bool perso = false;
    };
    const std::vector<MessaggioIa>& messaggi_ia() const;
    // Zone e fuoco amico (fase 3 C, dossier 24 §8.3): ogni sottomarino amico ha un'area d'azione mobile (15 nm avanti,
    // 30 indietro, 5 per lato sulla sua rotta); il giocatore la stessa, o le aree d'azione che la missione gli dichiara
    // (radio R2). Navi e aerei della parte non usano armi antisom su un contatto subacqueo che sta dentro, salvo un
    // contatto classificato ostile per classe (anche per errore del sonarista: il fuoco amico fra IA e sul
    // giocatore) e, nella prima ora dopo un attacco nemico alla parte, con la P del 24 (0,05 USA, 0,15 URSS). Una zona
    // e' ignota a chi ha perso il suo messaggio (P della perdita al teatro)
    bool in_zona_amica(int id, int bersaglio = -1);   // la posizione creduta del bersaglio in una zona amica di id
    bool zona_amica_in(int id, Vec2 punto);          // il punto in una zona amica di id
    // l'aereo dell'ASW aereo non sgancia sul punto dentro una zona amica, con le stesse eccezioni (fase 4)
    bool sgancio_vietato(int aereo, int bersaglio, Vec2 punto);
    struct FuocoAmicoIa {
        double t;
        int da, su;
        std::string causa;   // lancio, colpo, salva
    };
    const std::vector<FuocoAmicoIa>& fuoco_amico_ia() const;
    const std::vector<CambioAllerta>& cambi_allerta() const;   // crescente, per il registro
    const std::vector<Provocazione>& provocazioni() const;
    void evento_allerta(int unita, const std::string& evento);   // un evento della tabella vissuto da un'unita'
    // Collisioni fra unita' dell'Enhanced (docs/ricerca/16_collisioni.md, P1, P3 e P6; il contratto e i danni sono in
    // nucleo/collisioni.hpp, del kit). Forme vere da classi.toml: capsula e vela dei sottomarini (la chiglia vera, la
    // quota periscopica tradotta con quella della classe), prisma delle navi dalle sovrastrutture al pescaggio. Un Urto
    // per contatto (un altro dopo 2 s di contatto continuo), valutato con valuta_urto: i danni li applicano
    // DanniSottomarini e DanniMissione leggendo urti() col loro indice; qui la scena acustica (transitori_urto, id
    // 940000-949999), il contatto reciproco garantito e il registro. Le coppie con almeno un sottomarino (le navi fra
    // loro no, per ora). Quasi collisioni del giocatore: entro 200 m fra immersi, 500 m a quota periscopica con una nave.
    // Crazy Ivan dei sottomarini sovietici (03 §C4): virata di 120-180 gradi ogni 30-120 min, piu' spesso dopo un cambio
    // di rotta, a volte con un cambio di quota, poi qualche minuto d'ascolto e il ritorno alla navigazione
    void collisioni_enhanced(const std::map<std::string, ClasseNave>& classi, const std::string& cartella_dati);
    bool con_collisioni() const { return collisioni_ != nullptr; }
    void usa_collisioni(bool si) { collisioni_spente_ = !si; }
    void usa_crazy_ivan(bool si) { crazy_ivan_spento_ = !si; }
    struct UrtoMissione {
        Urto urto;
        EsitoUrto esito;
    };
    const std::vector<UrtoMissione>& urti() const { return urti_; }   // crescente
    struct QuasiCollisione {
        double tempo_s = 0.0;                 // al minimo della distanza
        int a = -1, b = -1;                   // a: il giocatore
        double distanza_m = 0.0;              // fra gli scafi
        double quota_a_m = 0.0, quota_b_m = 0.0;
        Vec2 pos;
        bool a_sapeva = false, b_sapeva = false;   // contatto presente (visibile) dell'uno sull'altro
        double qualita_a = 0.0, qualita_b = 0.0;
    };
    const std::vector<QuasiCollisione>& quasi_collisioni() const { return quasi_collisioni_; }
    int manovre_crazy_ivan(int id) const;
    // Il cavo della schiera trainata di una nave che fila (S10; dossier 04 §2.2 e §3: SQR-19 della Perry e dello
    // Spruance, cavo di 1.700 m all'angolo critico e schiera di 242 m fino a 360 m di quota). Segue la scia della poppa,
    // quindi in un'accostata spazza l'acqua. La poppa all'altezza dell'elica (entro D/2 + 1 m dal mozzo): impigliato (propulsione persa, rumore,
    // la nave perde la schiera e sente il battello: K-324, 1983); lo scafo che lo tocca: toccato. Registro:
    // cavo_impigliato e cavo_toccato. distanza_dal_cavo: m fra lo scafo del sottomarino e il cavo della nave, -1 senza
    struct IncidenteCavo { double tempo_s = 0.0; int sottomarino = -1, nave = -1; bool impigliato = false; Vec2 pos; };
    const std::vector<IncidenteCavo>& incidenti_cavo() const { return incidenti_cavo_; }
    double distanza_dal_cavo(int sottomarino, int nave) const;
    // L'immersione d'emergenza (TipoOrdine::immersione_emergenza): la chiglia sotto la nave piu' profonda entro 5 km,
    // con la vela e un margine (almeno 150 ft), entro il fondale e la quota massima
    double quota_immersione_emergenza() const;
    // Batteria dei diesel dell'IA (Kilo, Foxtrot; M6 del dossier 23, scheda S03 della libreria). La carica 0-1 si
    // consuma col carico di bordo e col cubo della velocita' (tarata su autonomia economica e a tutta forza, STIMA);
    // sotto 0,3 lo snorkel a quota periscopica la ricarica (rumore di diesel per i sonar), se il battello non e' in
    // caccia; a 0,05 l'emersione forzata (registro: emersione_forzata). Chiave `batteria = 0..1` nell'unita' della
    // missione per la carica iniziale (predefinita 0,8). Solo con la regola di simulazione
    void batterie_enhanced();
    double batteria(int id) const;      // -1 se l'unita' non ha batteria seguita
    bool snorkel(int id) const;
    bool emersione_forzata(int id) const;
    const std::vector<std::pair<double, int>>& emersioni_forzate() const { return emersioni_forzate_; }   // tempo, unita'
    // Unita' dell'IA in avaria, incagliate, rimorchiate (M6 del dossier 23: E07, E14, E19, E23; schede S04 e S11).
    // - In avaria: la propulsione e' persa, l'unita' si ferma e ogni 1,5-5 minuti fa un rumore di riparazione (un
    //   transitorio per i sonar). avaria(id, false) la ripara.
    // - Incagliata: ferma dove si trova finche' non la rimorchiano; la propulsione resta.
    // - Rimorchio: il rimorchiato segue il rimorchiatore a `cavo_m` (300 m), sulla sua scia; il rimorchiatore non supera
    //   `velocita_kn` (6). Un sottomarino rimorchiato sale in superficie. Il rimorchio stacca l'incagliata; finisce con
    //   molla_rimorchio o se una delle due esce di gioco.
    // Chiave `stato = "avaria"` o `"incagliata"` nell'unita' della missione; script: m688.avaria, m688.incaglia,
    // m688.rimorchia, m688.molla_rimorchio, m688.in_avaria, m688.rimorchiata. Registro: avaria_ia e rimorchio. Non
    // per il giocatore (le sue avarie sono dei danni)
    // Relitto (scheda I10): `stato = "relitto"`, un'unita' affondata ferma sul fondo, senza sensori ne' script che
    // contino, muta per il passivo (rumore 0 nel classico); la si trova con l'eco del sonar attivo (sul fondo: l'eco si
    // confonde con la riverberazione del fondo) o col MAD
    bool relitto(int id) const { return relitti_.count(id) != 0; }
    bool avaria(int id, bool si = true);
    bool incaglia(int id);
    bool rimorchia(int rimorchiatore, int rimorchiato, double velocita_kn = 6.0, double cavo_m = 300.0);
    bool molla_rimorchio(int id);   // l'una o l'altra unita' del rimorchio
    bool in_avaria(int id) const;
    bool incagliata_ia(int id) const;
    int rimorchiatore_di(int id) const;   // -1 se non e' rimorchiata
    struct EventoRimorchio { double tempo_s = 0.0; int rimorchiatore = -1, rimorchiato = -1; bool inizio = true; };
    const std::vector<EventoRimorchio>& eventi_rimorchio() const { return eventi_rimorchio_; }
    const std::vector<std::pair<double, int>>& avarie_iniziate() const { return avarie_iniziate_; }   // tempo, unita'
    // Armi da esercitazione (schede I03 e S01 di docs/17): inerti, senza testata. Il colpo si registra (registro:
    // colpo_esercitazione, obiettivo `colpire`) ma non fa danni ne' scoppi: l'arma esce dal gioco come all'impatto. Quali
    // sono: chiave della missione `esercitazione = true` (tutte), chiave d'unita' `armi_esercitazione = true` (le armi
    // che lancia), chiave `esercitazione = true` su un'arma della missione, m688.esercitazione(u [, si]) su un'arma o
    // su un lanciatore. Messaggi: 456 a segno (del giocatore), 457 colpiti (il giocatore)
    bool arma_esercitazione(int id) const { return armi_esercitazione_.count(id) != 0; }
    bool esercitazione(int id, bool si = true);   // un'arma: lei; un'altra unita': le armi che lancera'
    struct ColpoEsercitazione { double tempo_s = 0.0; int arma = -1, lanciatore = -1, vittima = -1; std::string classe_arma; Vec2 pos; };
    const std::vector<ColpoEsercitazione>& colpi_esercitazione() const { return colpi_esercitazione_; }
    // Danni iniziali del giocatore (scheda I07 di docs/17; docs/09 §4): chiave `danni_iniziali = { urto = { tnt_kg,
    // distanza_m, angolo_gradi }, sistemi = { propulsione = 0.4, schiera = 0, ... }, acqua = { { compartimento, m3 } },
    // falle = { { compartimento, area_m2 } }, incendi = { { compartimento, intensita } } }`. I sistemi coi nomi di
    // nucleo/battello (calcolatore_sonar, schiera, periscopio, sonar_tiro, motori, antenna, idroplani, timone,
    // propulsione, aria, pompe, sonar_prua) e l'efficienza di partenza 0-1; il compartimento col nome o l'uso dello
    // scafo (C3 o "macchine"). Li applica DanniSottomarini al primo passo: solo con la regola di simulazione
    struct DanniIniziali {
        struct Locale { std::string compartimento; double valore = 0.0; };
        bool urto = false;
        double urto_tnt_kg = 0.0, urto_distanza_m = 0.0, urto_angolo_gradi = 0.0;
        std::vector<std::pair<std::string, double>> sistemi;   // nome, efficienza
        std::vector<Locale> acqua, falle, incendi;
        bool vuoti() const { return !urto && sistemi.empty() && acqua.empty() && falle.empty() && incendi.empty(); }
    };
    const DanniIniziali& danni_iniziali() const { return danni_iniziali_; }
    // Acque territoriali dichiarate dalla missione (dossier 22 §2.4 e §9.2; docs/16): chiave `acque_territoriali = {
    // { nome, lato = "usa" | "urss" | "neutrale", cerchio = { x, y, r } o vertici = { {x, y}, ... } }, ... }`. In pace,
    // col giocatore straniero dentro, le navi del paese costiero: colpi d'avvertimento (300-500 m) se lo vedono
    // esposto; bombe d'avvertimento se e' immerso e ne hanno la posizione, due ogni 2 minuti a 500-1000 m (dopo 20
    // minuti dentro a 200-400 m). Registro: acque_territoriali (livello 0 entrata, 1 uscita); messaggi di bordo 453
    // entrata, 454 uscita, 455 bombe. Solo con l'artiglieria dell'Enhanced; Lua: m688.in_acque_territoriali(u)
    struct AcqueTerritoriali { std::string nome; Zona zona; int lato = 0x18; };   // lato: flag & 0x18 (0x18 neutrali)
    const std::vector<AcqueTerritoriali>& acque_territoriali() const { return acque_territoriali_; }
    int acque_territoriali_di(const UnitaClassica& u) const;   // indice delle acque straniere dove si trova, o -1
    struct PassaggioAcque { double tempo_s = 0.0; int acque = -1; bool entrata = true; };
    // Esperto (difficolta' minaccia percepita; dossier 22 §9.9): la nave vede la piuma della testa alzata del giocatore
    // e ci spara anche senza averla vista, entro 1 km di giorno e 300 m di notte, col mare fino a 4 e la piuma della
    // testa alla sua velocita' (nucleo/periscopi) almeno 0,2. Solo in guerra, nel passo dell'artiglieria
    bool vede_la_piuma(const UnitaClassica& nave) const;
    const std::vector<PassaggioAcque>& passaggi_acque() const { return passaggi_acque_; }   // del giocatore
    // Il radar del giocatore (docs/18 fase 2; TipoOrdine::radar): il radar della classe (BPS-15 sul 688, Snoop sull'Alfa)
    // emette col battello emerso o con l'albero alzato (flag54 & 4) a quota periscopica, e scendendo si spegne da solo
    // (messaggi 458 acceso, 459 spento da solo). Le emissioni entrano in quelle che sente l'ESM dell'IA; il radar vede
    // gli alberi e i battelli emersi dell'altra parte come quelli dell'IA. Solo con l'elettronica dell'Enhanced
    bool radar_giocatore_acceso() const;
    // AN/BQS-15 del 688 (docs/ricerca/06 §1.1 e §7; libreria S06): il sonar attivo HF della vela per le mine, acceso
    // e spento con TipoOrdine::bqs15. Una spazzata ogni 8 s del settore di prua (+-60 gradi in azimut, da -30 a +45 in
    // elevazione): le mine ormeggiate entro ~1,1 km, quelle di fondo (clutter) entro ~450 m, meno con la velocita'
    // (-30% a 15 kn, -60% oltre i 20; tutte STIMA). Ogni scoperta e' una posizione con l'errore del sonar (~5 m, ~1
    // grado), aggiornata a ogni spazzata; registro: mina_scoperta. Ogni spazzata e' un impulso di 205 dB a 25 kHz che
    // gli altri sentono con i sensori passivi (fuori dal fascio i lobi laterali, -25 dB). Solo nell'Enhanced e solo
    // per le classi che lo hanno (los_angeles). Messaggi di bordo: 450 acceso, 451 nuova mina, 452 spento
    struct MinaScoperta {
        int mina = -1;                 // id del CampoMinato
        Vec2 pos;                      // stimata
        double quota = 0.0;            // stimata
        double scoperta_t = 0.0, vista_t = 0.0;
        double distanza_m = 0.0, rilevamento = 0.0;   // dall'ultima spazzata
        bool ormeggiata = false;
    };
    bool bqs15(int id, bool acceso);   // falso se l'unita' non lo ha o senza l'Enhanced
    bool bqs15_acceso(int id) const { return bqs15_.count(id) != 0; }
    static bool ha_bqs15(const std::string& classe);
    const std::vector<MinaScoperta>& mine_scoperte() const { return mine_scoperte_; }   // del giocatore, in ordine
    // quelle d'un sottomarino dell'IA col sonar del fondo (BQS-15, MG-70, MG-24): il pilota le aggira (nullptr senza)
    const std::vector<MinaScoperta>* mine_scoperte_ia(int id) const;
    // la mina comandata da terra (la S18): le comandate del gruppo (vuoto: tutte) passano alla stazione; quante
    int comanda_mine(const std::string& gruppo);
    double portata_bqs15(double velocita_kn, bool ormeggiata) const;
    // la difficolta' scelta dall'app (dossier 17 §10.3), prima del primo passo: asw_enhanced la usa al posto della sua
    void imposta_difficolta_asw(DifficoltaAsw d) { difficolta_asw_ = d; }
    std::optional<DifficoltaAsw> difficolta_asw() const { return difficolta_asw_; }   // l'aiuto del telemetro (docs/25 §3.3)
    // Gli aerei ASW della missione per il briefing («PORTATE ASW NEMICHE»), anche prima del primo passo e senza avviare
    // l'ASW: dalla chiave `aereo_asw` o da data/missioni/asw/<ID>.lua; la classe dall'unita' se la configurazione ne da'
    // solo l'id, la parte dai dati (asw_aereo.toml in `cartella_dati`); vuoto senza configurazioni
    struct AereoAswPrevisto {
        int unita = -1;                        // -1: vale per la classe
        std::string classe, missione, parte;   // missione: caccia, ricognizione, pattuglia; parte: usa, urss ("" ignota)
        std::optional<Vec2> base, zona;
        int base_unita = -1;
        double raggio_zona_m = 0.0;
    };
    std::vector<AereoAswPrevisto> aerei_asw_previsti(const std::string& cartella_dati) const;
    void usa_asw(bool si) { asw_spento_ = !si; }
    // l'evasione dei sottomarini dell'IA dalla caccia aerea (nucleo/evasione.hpp), con l'ASW dell'Enhanced; accesa di
    // norma, si spegne per i confronti della taratura. Manovre scelte da un battello (0 senza)
    void usa_evasione(bool si) { evasione_spenta_ = !si; }
    void usa_esche_mobili(bool si) { esche_mobili_spente_ = !si; }   // l'IA senza MG-74 (confronti della taratura)
    // la sorgente acustica del battello del giocatore (firma R6, regime, danni), come la sente il nemico: per l'audio di
    // bordo (le proprie macchine). nullopt senza giocatore, senza catalogo o nel classico
    std::optional<Sorgente> sorgente_giocatore() const;
    int manovre_evasione(int id) const;   // false prima del primo passo: gli aerei restano del classico
    bool controllato_asw(int id) const;
    // l'aereo (controllato) caccia il giocatore: nazione diversa, o il giocatore fra i bersagli (TORPEX)
    bool aereo_ostile_al_giocatore(int id) const;
    const ControlloreAsw* controllore_asw(int id) const;
    std::vector<VoloAsw> voli_asw() const;
    std::vector<Sonoboa> boe() const;                          // tutte le boe posate, anche di aerei usciti
    const std::vector<MarcatoreAereo>& marcatori_asw() const;
    const std::vector<LancioAereo>& lanci_aerei() const;
    const std::vector<AvvisoAsw>& messaggi_asw() const { return messaggi_asw_; }
    // Il sonarista dell'equipaggio (S5d, nucleo/sonarista.hpp): con l'acustica dell'Enhanced e l'esperienza scelta
    // dall'app (prima del primo passo) segue ogni secondo il miglior ascolto del giocatore per sorgente, con le righe
    // del fascio, e fa i suoi rapporti (avvisi 410-413) con ritardi, classificazioni sbagliate e scambi d'incrocio
    // (questi solo nel registro: lui non lo sa). Senza imposta_sonarista niente (e nel classico mai).
    struct AvvisoSonarista {
        double tempo = 0.0;
        int codice = 0;                  // 410 nuovo, 411 classificato, 412 perso, 413 ritrovato
        int numero = 0;                  // Sierra
        int sorgente = -1;               // verita': l'unita' che la traccia segue
        double rilevamento = 0.0;        // misurato
        std::string classe;              // classificato: la classe detta ("" senza)
        bool errata = false;             // verita'
        std::string testo_it, testo_en;  // gia' composto dal sonarista
    };
    struct ScambioTracce { double tempo = 0.0; int numero = 0, da = -1, a = -1; };   // verita', per il debriefing
    void imposta_sonarista(Esperienza e) { esperienza_sonarista_ = e; }
    // Il sonarista dell'IA (docs/18 fase 2, C; nucleo/sonarista del disassemblatore): ogni unita' dell'IA con un sonar
    // ha il suo, col livello dall'avversario del blocco carriera per le unita' ostili al giocatore (regolare per le
    // altre), o dalla chiave d'unita' `sonarista = "recluta" | "medio" | "esperto"`. Le sue tracce decidono la
    // classificazione dei contatti acustici dell'IA: la durata resta sotto 800 finche' non ha riferito il contatto e
    // sotto 1400 finche' non l'ha classificato. Registro: classificazione_ia. usa_sonaristi_ia(false) per i confronti
    struct ClassificazioneIa {
        double tempo_s = 0.0;
        int unita = -1, sorgente = -1;   // chi ascolta, chi e' davvero la sorgente della traccia
        std::string classe;              // quella detta ("" senza la classe)
        Confidenza confidenza = Confidenza::possub;
        bool errata = false;             // verita'
        bool visiva = false;             // dal libretto, a vista (docs/25 §3.5), non dal sonarista
    };
    const std::vector<ClassificazioneIa>& classificazioni_ia() const { return classificazioni_ia_; }
    // --- il libretto di riconoscimento al periscopio (docs/25 §3.2, §3.4, §3.5; missione_libretto.cpp) ---
    // un'identificazione visiva: del giocatore (ordine identifica_visiva) o di un'unita' dell'IA (contatto «vista»)
    struct IdentificazioneVisiva {
        double t = 0.0;
        int osservatore = -1, contatto = -1;
        std::string classe;        // la chiave di classi.toml dichiarata dal giocatore o creduta dall'IA
        bool giusta = true;        // verita' (debriefing, prove)
        double distanza_m = 0.0;   // vera, al momento
    };
    // l'ultima di un osservatore su un contatto (nullptr senza), e tutte in ordine
    const IdentificazioneVisiva* identificazione_visiva(int osservatore, int contatto) const;
    const std::vector<IdentificazioneVisiva>& identificazioni_visive() const { return identificazioni_visive_; }
    // un'osservazione dell'angolo sulla prora del giocatore (ordine osserva_aob), per il MATE e la TMA
    struct OsservazioneAob {
        double t = 0.0;
        int contatto = -1;
        double aob_gradi = 0.0;          // + dritta, - sinistra
        double incertezza_gradi = 0.0;
        double rilevamento = 0.0;        // quello misurato del contatto al momento
        double rotta = 0.0;              // del bersaglio: rilevamento + 180 - AOB, gradi bussola
        double rotta_vera = 0.0;         // verita' al momento (registro, obiettivo rotta_stimata)
    };
    // una rotta stimata dalla TMA del giocatore (la soluzione del MATE, app/interfaccia; docs/17, LIB-S20): la rotta vera
    // e' verita', solo per il registro (obiettivo rotta_stimata con da_tma)
    struct StimaRottaTma {
        double t = 0.0;
        int contatto = -1;
        double rotta = 0.0, rotta_vera = 0.0;
    };
    void stima_tma_giocatore(int contatto, double rotta);
    const std::vector<StimaRottaTma>& stime_tma_giocatore() const { return stime_tma_giocatore_; }
    // una lettura del telemetro (ordine misura_telemetro): la distanza vera e' verita', solo per il registro e il debriefing
    struct MisuraTelemetro {
        double t = 0.0;
        int contatto = -1;
        double letta_m = 0.0, vera_m = 0.0, altezza_m = 0.0;
    };
    const std::vector<MisuraTelemetro>& misure_telemetro() const { return misure_telemetro_; }
    // i conti della missione del giocatore per gli achievement e le statistiche (docs/26; missione_conti.cpp)
    struct ContiGiocatore {
        int accensioni_radar = 0;
        double sotto_chiglia_20_s = 0.0;                 // secondi con meno di 20 m d'acqua sotto la chiglia (vera)
        int mare_max = 0;                                // lo stato del mare massimo nel punto del giocatore
        std::vector<std::pair<double, int>> allarmi_siluro;   // l'allarme dell'intercettatore che sale (1 in acqua, 2 in acquisizione)
        std::vector<double> attraversamenti_mine;        // fine di un passaggio in un campo di mine ostili col BQS-15 acceso, senza danni
        std::vector<std::pair<double, bool>> verifiche_radio;   // (quando, autentico)
        // il secondo giro degli achievement (31/10/2026; nota disassemblatore-achievement-31-10-proposta.md §8)
        double sotto_banchisa_s = 0.0;                   // secondi sotto la banchisa (sotto_banchisa)
        int emersioni_ghiaccio = 0;                      // emersioni (sotto 2 m) dentro la banchisa, in acqua libera o nel nilas
        double in_coda_s = 0.0, entro_500yd_s = 0.0;     // i tratti continui piu' lunghi: in coda a un sottomarino straniero
                                                         // (3 nm, poppa +/-40 gradi, col contatto, non classificati da lui) e entro 457 m
        int visive_fanali_giuste = 0, visive_luci_spente = 0;   // identificazioni visive giuste di notte: coi soli fanali (oltre la
                                                         // sagoma) e di un'unita' a luci spente
        int visive_notte_sagoma = 0, errate_luci_spente = 0;   // giuste di notte con la sagoma; errate di un'unita' a luci spente
        std::set<int> navi_traino_vicine;                // navi col VDS o la schiera filati passate entro 5 km
        std::map<int, double> mine_vicine_m;             // mina scoperta -> la distanza minima dopo la scoperta
        int luci_accese_notte = 0;                       // episodi con le luci proprie accese emerso di notte (l'esposizione)
        // lo stato dei conti fra un passo e l'altro
        bool emerso = false, luci_notte = false;
        double coda_ora_s = 0.0, vicino_ora_s = 0.0;
    };
    const ContiGiocatore& conti_giocatore() const { return conti_; }
    // I transitori dello scafo (missione_scafo.cpp; nota disassemblatore-transitori-scafo.md): portelli, attrezzi, piani,
    // pompe, TDU, soffiate d'emergenza e scricchiolii, di tutti i sottomarini immersi, giocatore e IA, nella scena acustica
    // come i transitori delle armi; chi li sente ne ricava «sottomarino». Le frequenze dipendono dall'equipaggio e dal silenzio
    void silenzio_giocatore(bool si) { silenzio_giocatore_ = si; }        // il silenzio a bordo (l'app: StazioneArmi)
    void equipaggio_giocatore(Esperienza e) { equipaggio_giocatore_ = e; }   // l'equipaggio del battello (carriera, difficolta')
    // I posti di combattimento del battello del giocatore (per l'allarme sonoro): al primo allarme siluro, a un lancio
    // proprio, con un contatto ostile vicino; di nuovo la condizione normale dopo un quarto d'ora senza
    const std::map<std::string, int>& scafo_nati() const { return scafo_nati_; }   // quanti per voce (portello_sbatte...)
    struct ScafoSentito { double t = 0.0; int ascoltatore = -1, origine = -1; std::string tipo; double se_db = 0.0; };
    const std::vector<ScafoSentito>& scafo_sentiti() const { return scafo_sentiti_; }   // chi ha sentito chi (l'IA)
    bool posti_di_combattimento() const { return posti_; }
    struct CambioPosti { double t = 0.0; bool posti = false; std::string causa; };   // siluro, lancio, contatto, normale
    const std::vector<CambioPosti>& cambi_posti() const { return cambi_posti_; }
    // le soffiate d'emergenza delle casse del giocatore (l'ordine emersione_emergenza): i tempi, per il registro e l'audio
    const std::vector<double>& soffiate_giocatore() const { return soffiate_giocatore_; }
    // il soccorso della missione (m688.salvati(n, di): gli uomini salvati su quelli in acqua), per achievement e carriera
    struct Soccorso { int salvati = 0, dispersi = 0; };
    const Soccorso& salvati() const { return soccorso_; }
    void salvati(int n, int di) { soccorso_ = {std::max(0, n), std::max(0, di)}; }
    // le luci di navigazione (docs/27, missione_luci.cpp di trailer-1): solo con luci_enhanced (Enhanced)
    void luci_enhanced(const std::string& cartella_dati);
    luci::Stato stato_luci(int id) const;
    bool luci_giocatore_accese() const;
    struct CambioLuci { double tempo = 0.0; int unita = -1; luci::Stato stato = luci::Stato::spente; };
    const std::vector<CambioLuci>& cambi_luci() const { return cambi_luci_; }
    double portata_notte(int osservato, Vec2 da, const Osservatore& o) const;   // luci e sagoma; < 0 senza modello
    // a ogni luce (missione_luci.cpp): la sagoma col giorno, il crepuscolo e la notte, di notte anche i fanali; < 0 senza
    // sagoma (aerei, armi). 7: il 7x50 delle vedette (INGRANDIMENTO_BINOCOLO di nucleo/sagoma_notte)
    double portata_vista(int osservato, Vec2 da, const Osservatore& o, double ingrandimento = 7.0) const;
    double portata_sagoma(int osservato, Vec2 da, const Osservatore& o, double ingrandimento = 7.0) const;   // senza i fanali
    // il cielo della notte in p per le sagome (missione_luci.cpp): con la data della missione e il fondale georeferenziato
    // (GEO1) la Luna vera all'ora della partita, se no la luce del Sole e le nubi senza Luna. Anche per la vista
    CieloNotte cielo_notte_in(Vec2 p) const;
    // quello che i fanali visti dicono a una vedetta o a un periscopio di notte (missione_luci.cpp): lo stato (in moto, alla
    // fonda, in pesca, manovra limitata, senza governo) dai fanali che lo caratterizzano; il tipo quando lo dicono (i
    // fanali da pesca: peschereccio; il faro ambra: sottomarino USN); l'aspetto dai laterali e dal coronamento. Niente
    // classe: quella la da' la sagoma
    struct LetturaFanali {
        int visti = 0;                                 // i fanali che arrivano all'osservatore
        std::optional<luci::Stato> stato;
        std::string tipo;                              // "peschereccio", "sottomarino" o vuoto
        std::optional<double> aspetto;                 // gradi dalla prua del bersaglio verso chi guarda, positivi a dritta
        double sigma_aspetto = 180.0;
    };
    LetturaFanali leggi_fanali(int osservato, Vec2 da, const Osservatore& o) const;
    // l'ultima lettura dei fanali d'un'unita' dell'IA su un contatto (passo_libretto_ia, di notte oltre la sagoma)
    struct FanaliVisti { double t = 0.0; LetturaFanali lettura; };
    // i ping attivi di tutte le unita' (impulso_sonar_enhanced, anche il PING degli script nel classico): per le condizioni
    // degli script m688.ha_pingato e m688.ping_entro (la simmetria con ping_giocatore_entro)
    struct PingUnita { double t = 0.0; int unita = -1; Vec2 pos; };
    const std::vector<PingUnita>& ping_unita() const { return ping_unita_; }
    const FanaliVisti* fanali_visti(int osservatore, int contatto) const;
    // l'ATTACK di uno script dell'IA su un sottomarino, nell'Enhanced: il siluro antisom vero della nave col modello
    // (missione_antisom_navi.cpp); falso se la classe non ne ha, senza la posizione o fuori portata (resta il dado)
    bool siluro_antisom_nave(int nave, int bersaglio);
    bool gps() const { return gps_opzione_; }
    const std::vector<OsservazioneAob>& osservazioni_aob() const { return osservazioni_aob_; }
    // la TMA di un'unita' dell'IA su un contatto (rilevamenti e rotte dell'AOB al periscopio); nullptr senza
    const Tma* tma_dell_ia(int id, int contatto) const {
        const auto it = tma_ia_.find({id, contatto});
        return it == tma_ia_.end() ? nullptr : &it->second.tma;
    }
    // la TMA dell'equipaggio del giocatore su un contatto (rilevamenti e rotte dell'AOB); nullptr senza
    const Tma* tma_equipaggio(int contatto) const {
        const auto it = tma_equipaggio_.find(contatto);
        return it == tma_equipaggio_.end() ? nullptr : &it->second.tma;
    }
    const Sonarista* sonarista_ia(int id) const {   // le tracce del sonarista di un'unita' dell'IA (verita': debriefing, prove)
        const auto it = sonaristi_ia_.find(id);
        return it == sonaristi_ia_.end() ? nullptr : it->second.s.get();
    }
    void usa_sonaristi_ia(bool si) { sonaristi_ia_spenti_ = !si; }
    void usa_esm_ia(bool si) { esm_ia_spento_ = !si; }   // l'ESM delle unita' dell'IA (fase 2), per i confronti
    // R6 del disassemblatore: le variazioni del rumore per unita' (ausiliari, virate, danni, usura); spente di serie
    // accese di serie (decisione del 25/10/2026): valgono solo nell'acustica Enhanced, cioe' con la regola di simulazione
    void usa_variazioni_acustiche(bool si) { variazioni_scelte_ = si; }
    bool variazioni_acustiche() const { return variazioni_scelte_.value_or(true); }
    const Sonarista* sonarista() const { return sonarista_.get(); }
    const std::vector<AvvisoSonarista>& avvisi_sonarista() const { return avvisi_sonarista_; }
    const std::vector<ScambioTracce>& scambi_tracce() const { return scambi_tracce_; }
    // Meteo e mare dell'Enhanced (docs/15_meteo_e_mare.md, M2; nucleo/meteo.hpp): il generatore del teatro per il mese
    // della missione, col seme della partita; l'origine (0, 0) e' il centro della missione. Lo stato del mare, il vento
    // e la pioggia nel punto del giocatore (ogni secondo) vanno all'acustica, all'elettronica e ai periscopi. Il modo
    // «semplificato» (mare 3, sereno: quello di prima del meteo) e' il predefinito; il realistico lo sceglie l'opzione
    // di realismo della partita (usa_meteo prima del primo passo). Senza i dati: semplificato.
    enum class ModoMeteo : std::uint8_t { realistico, semplificato };
    void meteo_enhanced(const std::string& cartella_dati);
    void usa_meteo(ModoMeteo m) { modo_meteo_ = m; }   // prima del primo passo
    const Meteo* meteo() const { return meteo_.get(); }
    Meteo* meteo() { return meteo_.get(); }
    // il meteo nel punto del giocatore (aggiornato ogni secondo); semplificato senza meteo
    const StatoMeteo& meteo_qui() const { return meteo_qui_; }
    int stato_mare() const { return meteo_qui_.stato_mare(); }
    StatoMeteo meteo_in(Vec2 pos) const;   // in un punto qualunque (con le zone del vetrino di Dio)
    double ora_utc_inizio_s() const { return ora_utc_s_; }   // ora UTC d'inizio della missione, s dalla mezzanotte
    // l'emissione dell'ultimo bollettino (00, 06, 12, 18 UTC) fino a t, in s di missione (puo' essere negativa)
    double ultimo_bollettino(double t) const;
    // i fulmini sul mare sentiti dal sonar del giocatore (M2): tempo e rilevamento vero
    struct FulmineSentito { double tempo = 0.0, rilevamento = 0.0; };
    const std::vector<FulmineSentito>& fulmini_sentiti() const { return fulmini_sentiti_; }
    // la vela del giocatore affiora (a quota periscopica col mare grosso, dossier 21 §7.3): il radar la vede da emersa
    bool vela_affiorata() const { return tempo_ < affiora_fino_; }
    // la velocita' massima di una nave col mare (dossier 21 §7.2), nodi; infinita fino a mare 3 o senza meteo
    double velocita_col_mare(const UnitaClassica& u) const;
    // Periscopi dell'Enhanced (docs/14_periscopi.md, P2; nucleo/periscopi.hpp): il posto del giocatore coi periscopi
    // della classe (688: Type 2 e Type 18), alzata coi suoi secondi (il flag del periscopio a fine corsa), esposizione
    // e sezione della testa verso i radar, ESM solo coi periscopi che ce l'hanno, danno oltre la velocita' di danno.
    // Navi, aerei e periscopi nemici vedono la testa del giocatore; i sottomarini dell'IA fanno le loro guardate,
    // visibili al giocatore. Foto e esposizioni lunghe per il registro; avvisi in messaggi_periscopi(). Senza i dati
    // (o nel classico) resta il periscopio dell'originale.
    void periscopi_enhanced(const std::string& cartella_dati);
    bool con_periscopi() const { return peri_ != nullptr; }
    const PostoPeriscopi* posto_periscopi() const;
    // ingrandimento e modo (non cambiano la simulazione): la stazione li cambia qui
    void vista_periscopio(int ingrandimento, ModoVista modo, int filtro = -1, double sdoppiamento_gradi = 0.0,
                          bool telecamera_verticale = false);
    std::vector<TestaIa> teste_ia() const;
    const std::vector<Messaggio>& messaggi_periscopi() const { return messaggi_peri_; }
    const std::vector<FotoPeriscopio>& foto_periscopio() const { return foto_; }
    const std::vector<EsposizionePeriscopio>& esposizioni_lunghe() const { return esposizioni_lunghe_; }
    const std::vector<GuastoRichiesto>& guasti_richiesti() const { return guasti_; }
    double luce_periscopi() const;   // luce ambiente dal sole della missione e dal coperto (0 notte, 1 giorno)
    // il luogo del sole per la luce (opzioni --lat e --lon della partita): solo il sole, la geografia resta del teatro
    void luogo_del_sole(double lat, double lon) { luogo_sole_ = std::make_pair(lat, lon); }
    // Artiglieria (G3 del kit): la canna del periscopio cieco del giocatore, aperta e non ancora abbassata, imbarca acqua
    // nella centrale (STIMA, dossier 22 §5.3); DanniSottomarini la legge a ogni passo. 0 se no
    double allagamento_periscopio_m3_s(int unita) const;
    // quota vera di un aereo sopra il mare: quella del controllore, o 150 m per gli elicotteri e 300 m per gli aerei
    double quota_aerea(const UnitaClassica& u) const;
    bool aereo(const UnitaClassica& u) const { return (u.flag & 3) == 3 || u.categoria == "aereo"; }
    // un cue per l'aereo `id` (-1: tutti gli aerei controllati): datum dallo script, dalla radio o dall'editor
    void cue_asw(int id, const Cue& c);
    bool con_acustica() const { return catalogo_ != nullptr; }
    const CatalogoAcustico* catalogo_acustico() const { return catalogo_.get(); }   // firme e sensori (stazione sonar)
    // R5: i campioni collegati al catalogo (la stessa libreria va a CuffiaSonar::imposta_campioni); nullptr senza
    const LibreriaCampioni* campioni() const { return campioni_ && !campioni_->vuota() ? campioni_.get() : nullptr; }
    // R5 spento: il catalogo resta quello del modello (da chiamare prima di acustica_enhanced)
    void usa_campioni(bool si) { usa_campioni_ = si; }
    void imposta_mese(int mese) { mese_ = mese; }   // 1-12; senza, quello della data della missione o del teatro
    // il mese dell'acqua per l'acustica: quello imposto, se no quello della data della missione quando la data c'e'
    // (file con `data`, imposta_data_ora: editor, rapide, scene, carriera), se no il predefinito del teatro (le
    // originali, che non hanno data)
    int mese_acustica() const;
    void impulso_sonar_enhanced(int id);   // ping dell'unita' (giocatore o script PING)
    // Sonar attivo «speciale» con la portata dello scenario (scheda I09): chiave d'unita' `sonar_speciale = { sl_db,
    // frequenza_hz, impulso_s, risoluzione_s, ogni_s, dimensione_m, antenna }` (risoluzione_s: FM con compressione, 1/B;
    // antenna: sfera, lineare o scafo, come nel catalogo; una lineare di 100-200 m fa rendere la bassa frequenza). Al ping dell'unita' prende il posto del suo sensore attivo
    // (o ne fa uno, se la classe non ne ha), con la banda piu' vicina del modello; ogni_s > 0: pinga da sola con quella
    // cadenza. Eco e contro-scoperta dal modello acustico. Solo con l'acustica dell'Enhanced
    struct SonarSpeciale {
        double sl_db = 230.0, frequenza_hz = 1000.0, impulso_s = 1.0, risoluzione_s = 0.0, ogni_s = 0.0;
        double dimensione_m = 0.0;   // 0: della classe
        std::string antenna;         // vuota: della classe (o sfera se la classe non ha un attivo)
        double prossimo = 0.0;
    };
    const SonarSpeciale* sonar_speciale(int id) const {
        const auto it = sonar_speciale_.find(id);
        return it == sonar_speciale_.end() ? nullptr : &it->second;
    }
    const std::vector<Messaggio>& messaggi_sonar() const { return messaggi_sonar_; }
    // Per la stazione sonar (cascata, spettro, DEMON del modulo): il ricevitore `id` col sensore della sua suite
    // (nullptr se non c'e' o, la schiera, non e' filata), l'ambiente nella sua posizione e le sorgenti di tutte le
    // altre unita' (aerei esclusi), come le vedono i contatti. Vuota senza l'acustica.
    struct SceneAcustica {
        Ricevitore ricevitore;
        Ambiente ambiente;
        std::vector<Sorgente> sorgenti;
        std::vector<const CampoPerdita*> campi;   // per ogni sorgente il campo che il sensore usa (nullptr: livello A)
    };
    SceneAcustica scena_acustica(int id, const std::string& sensore) const;
    // Paesaggio sonoro (S5c): le sorgenti attorno al giocatore (biologici, geofisici, pescherecci...) e i loro contatti
    // per il sonar del giocatore (id da 100000; niente posizione, la classificazione si ferma prima della classe)
    const std::vector<IstanzaSorgente>& paesaggio() const { return paesaggio_; }
    void usa_paesaggio(bool si) { usa_paesaggio_ = si; if (!si) { paesaggio_.clear(); contatti_paesaggio_.clear(); paesaggio_pronto_ = false; } }
    const std::map<int, Contatto>& contatti_paesaggio() const { return contatti_paesaggio_; }
    const IstanzaSorgente* istanza_paesaggio(int id) const;   // anche le reti a strascico
    // Reti a strascico dei pescherecci (voce «rete_strascico» di acustica.toml, richiesta I02): una per peschereccio
    // nella finestra di velocita' della voce, dietro la nave a 3 volte il fondale (300-1500 m), 2 m sopra il fondo; id
    // 930000 + id della nave. Nella scena acustica e fra i contatti del paesaggio
    const std::vector<IstanzaSorgente>& reti_strascico() const { return reti_; }
    bool alla_fisica(int id) const { return fisica_.count(id) > 0; }
    void distruggi(int id);                                    // avvia l'affondamento (+55) come un colpo letale
    // Impatti delle armi solo nelle coppie dell'osservatore del passo, come l'originale (2011:0C23): predefinito, livello
    // classico. Con false su ogni coppia a ogni passo (regola di simulazione, DanniMissione).
    void impatti_dell_originale(bool si) { impatti_originale_ = si; }
    // Enhanced (regola di simulazione, fase C6b): i siluri lanciati da qui in poi (giocatore e IA) seguono il modello
    // di nucleo/siluro.hpp: corsa, ricerca, aggancio, esche, filo e spoletta. Gli script classici dell'arma non girano
    // piu' (fine corsa, taglio del filo a tempo, impulsi); l'esplosione colpisce con la regola dei colpi del motore.
    // Classi: mk48 -> Mk 48, type53 -> USET-80, le altre se hanno lo stesso nome nella tabella (engine/README.md).
    // La tabella e' <cartella_dati>/siluri.toml, letta alla prima chiamata; se manca i siluri restano classici e
    // l'errore va in errori().
    void siluri_enhanced(const std::string& cartella_dati);
    bool con_siluri_enhanced() const { return siluri_attivi_; }
    // Missili dell'Enhanced (docs/21; missione_missili.cpp): con <cartella_dati>/missili.toml, letto con i siluri, i
    // tubi con un missile del catalogo (famiglie harpoon e tomahawk) lanciano il modello di nucleo/missili: il piano sui
    // dati del battello, il volo dalla posizione vera, il seeker che aggancia il primo eco. Il gestore di lancio della
    // missione gira ancora (i conteggi degli obiettivi), ma il suo ATTACK_RND no: l'esito e' dell'impatto
    bool con_missili() const { return catalogo_missili_ != nullptr; }
    struct TiroMissile {                       // il tubo con un missile, per la pagina F7 e la carta di tiro
        int tubo = 0;
        const DatiMissile* dati = nullptr;
        FonteDati fonte = FonteDati::tma;
        ImpostazioniMissile imp;
        std::optional<BersaglioStimato> bersaglio;   // nullopt: nessun dato per la fonte scelta (o nessun bersaglio)
        PianoTiro piano;                       // vuoto (non valido) senza bersaglio
        FasePreparazione fase = FasePreparazione::spento;
        double residuo_s = 0.0;
        bool pwr = false, aln = false, rdy = false;   // rdy = piano valido e allineato (TLAM: missione valida e ordine)
        bool slide_libera = true;              // la slide valve del suo lato
        // TLAM (attacco a terra): la missione scelta dalla libreria di bordo, lo stato sulla posizione STIMATA (il pannello)
        // e l'ordine d'esecuzione ricevuto alla radio
        int missione_tlam = -1;                // indice in libreria_tlam(), -1 nessuna
        const MissioneTlam* tlam = nullptr;
        std::optional<StatoTlam> stato_tlam;
        bool ordine_esecuzione = false;
        bool ordine_non_autentico = false;   // I12: e' arrivato solo un ordine col gruppo che non torna (la consolle lo dice)
    };
    std::optional<TiroMissile> tiro_missile(int tubo) const;   // nullopt: il tubo non ha un missile del catalogo
    struct MissileInVolo {                     // del giocatore: solo stime (decisione 4 di docs/21 §10)
        int unita = -1, tubo = 0;
        std::string missile;
        double t_lancio = 0.0;
        PianoTiro piano;
        ImpostazioniMissile imp;
        StimaVolo stima;
    };
    std::vector<MissileInVolo> missili_in_volo() const;
    const std::vector<MissioneTlam>& libreria_tlam() const { return libreria_tlam_; }   // il campo `tlam` della missione
    struct LancioTlam {                        // un TLAM lanciato: il pannello (stimato), la verita' e l'esito
        double t_lancio = 0.0;
        int tubo = 0;
        std::string missione;                  // MissioneTlam::id
        StatoTlam pannello, vero;
        EsitoTlam esito;
        bool riferito = false;                 // l'esito e' nel registro (all'arrivo) e il BDA alla radio
    };
    const std::vector<LancioTlam>& lanci_tlam() const { return lanci_tlam_; }
    bool tlam_eseguibile(const std::string& id) const { return tlam_eseguibili_.count(id) != 0; }
    bool slide_libera(int tubo) const;        // una slide valve per lato: lo stesso lato dopo 20 s, l'altro dopo 4 s
    const CatalogoMissili* catalogo_missili() const { return catalogo_missili_.get(); }
    // ATTACK_RND dentro il gestore di lancio di un missile del modello, o in un timer nato da lui: l'esito e' del volo
    bool attacco_da_missile() const { return lancio_modellato_ || timer_da_missile_; }
    // MISSILE di uno script dell'IA (docs/21 §7): con un missile antinave della classe nel catalogo (per_classe) parte il
    // modello, col piano sulla soluzione dell'unita'; l'ATTACK_RND del resto dello script e dei suoi timer non fa niente.
    // Col piano non valido il missile non parte (negato): niente lancio, niente avviso, ATTACK_RND muto come sopra
    enum class EsitoMissileScript : std::uint8_t { senza_modello, lanciato, negato };
    EsitoMissileScript missile_dello_script(int unita);
    // le impostazioni del missile del modello in volo (la scatola e il modo, per il registro): nullopt se non e' un missile
    std::optional<ImpostazioniMissile> impostazioni_missile(int arma) const;
    // Traffico civile (docs/20 §3; missione_traffico.cpp): mercantili, traghetti e pescherecci delle rotte del teatro
    // (nucleo/traffico, data/motore/teatri/<teatro>.traffico.toml) entro 100 km dal giocatore, al piu' 40. Unita' vere,
    // neutrali e non combattenti, solo sorgenti: niente sensori ne' IA, nessun gestore degli script le sceglie come
    // bersaglio. Spento di norma; acceso solo nell'Enhanced (con l'acustica). La chiave di missione `traffico = false`
    // lo spegne sempre
    void usa_traffico(bool si) { traffico_attivo_ = si; }
    bool con_traffico() const { return traffico_.has_value(); }
    bool del_traffico(int id) const { return id_traffico_.count(id) != 0; }
    const Traffico* traffico() const { return traffico_ ? &*traffico_ : nullptr; }
    // La schiera trainata dei sottomarini nell'Enhanced (docs/20 §7, D; missione_schiera.cpp; modulo nucleo/schiera): il
    // cavo filato alla velocita' del sistema, la quota vera della schiera per l'acustica (sotto il battello a bassa
    // velocita'), il fondo lungo la scia, i danni e le perdite del dossier 04 §6.3 (fondo, eccesso di velocita', quota
    // massima, reti dei pescherecci al lavoro, esplosioni vicine), per il giocatore e l'IA. Solo con l'acustica
    struct StatoSchieraUnita {
        double cavo_filato_m = 0.0, quota_centro_m = 0.0, quota_bassa_m = 0.0, dietro_m = 0.0;
        bool fondo = false, oltre_quota_max = false, danneggiata = false, persa = false;
    };
    // Posarsi sul fondo nell'Enhanced (roadmap 30; missione_fondo.cpp): dal tipo di fondo del teatro, per il giocatore e
    // l'IA. Il fango trattiene (lasciarlo costa tempo), la roccia danneggia timone e cupola a chi ci striscia, le prese a
    // mare aspirano il fango (il raffreddamento) se si resta posati a lungo. Solo con l'acustica (il clima del teatro)
    struct StatoFondoUnita { TipoFondo tipo = TipoFondo::sabbia; double posato_s = 0.0, presa = 0.0, trattenuto_s = 0.0; bool prese_intasate = false; };
    std::optional<StatoFondoUnita> stato_fondo(int id) const;   // nullopt: mai posato
    std::optional<StatoSchieraUnita> stato_schiera(int id) const;   // nullopt: nessuna schiera del modello
    bool schiera_persa(int id) const;
    // Il sonar fuori scafo delle navi dell'IA (docs/ricerca/33; missione_vds.cpp): il pesce del VDS al cavo (una schiera
    // corta, nucleo/schiera) alla quota voluta, sotto lo strato; la schiera trainata delle navi in missione_schiera.cpp.
    // Per il registro e le prove: lo stato del pesce e gli eventi di traino (filato, recuperato, con la quota)
    struct StatoVds {
        double cavo_filato_m = 0.0, quota_m = 0.0, quota_ordinata_m = 0.0, dietro_m = 0.0;
        bool in_acqua = false, recupero = false, persa = false;
    };
    std::optional<StatoVds> stato_vds(int id) const;   // nullopt: la nave non ha un VDS
    struct EventoTraino { double tempo_s = 0.0; int nave = -1; std::string sistema; bool filato = false; double quota_m = 0.0; };
    const std::vector<EventoTraino>& eventi_traino() const { return eventi_traino_; }
    double limite_traino_kn(int id) const { const auto it = traini_.find(id); return it == traini_.end() ? 1e9 : it->second.limite_kn; }
    // Il ricevitore d'intercettazione (blocco 2 del sonar, E; missione_intercettazione.cpp; nucleo/intercettazione): ogni
    // passo i ping in mare (sonar attivi di scafo, BQS-15, teste dei siluri) arrivano ai ricevitori di chi ce l'ha
    // (WLR-9, Skat, Rubin), giocatore e IA. L'allarme (siluro in acqua, in acquisizione) avvisa il giocatore e fa
    // evadere l'IA. Solo con l'acustica
    const RicevitoreIntercettazione* intercettatore(int id) const;   // nullptr: l'unita' non ne ha
    const RicevitoreIntercettazione* intercettatore_giocatore() const;
    // Esca semovente (MG-74, MOSS Mk 70: [esche.*] di siluri.toml con velocita_kn > 0; missione_esche.cpp) lanciata dal
    // tubo dell'unita' `lanciatore` sulla rotta e alla quota date (quota < 0: quella del lanciatore, entro la quota
    // massima dell'esca). Corre con la manovra programmata, suona come il lanciatore (imita_rumore) e da' un'eco con
    // Doppler vero; finisce alla durata. Solo coi siluri dell'Enhanced: -1 senza, o se la classe non e' semovente
    int lancia_esca_mobile(int lanciatore, const std::string& classe, double rotta, double quota = -1.0);
    const DatiEsca* esca_mobile(int id) const;   // i dati dell'esca semovente `id` in corsa; nullptr se non lo e'
    Siluro* siluro(int id);                                    // nullptr se l'unita' non e' un siluro del modello
    const Siluro* siluro(int id) const;
    // impostazioni per il prossimo siluro del modello lanciato dal giocatore (quelle del tubo che parte); senza,
    // rotta sul bersaglio, testa mista, RTE al 60% della distanza, ricerca alla quota del bersaglio
    // le impostazioni del tubo del giocatore per il prossimo lancio; `campi`: quelli toccati (1 << CampoSiluro), gli
    // altri il motore li ricava dal contatto e dal bersaglio come per l'IA
    void imposta_siluro_giocatore(const ImpostazioniSiluro& imp, std::uint32_t campi = ~0u) {
        impostazioni_giocatore_ = imp;
        campi_giocatore_ = campi;
    }
    // Enhanced, mine (M2b, nucleo/mine.hpp): il campo minato posato dai tubi con la famiglia `mina` (il tipo da
    // variante_tubo; rotta e distanza di corsa dalle impostazioni del tubo, rotta del battello e corsa intera senza).
    // Le mine si leggono da <cartella_dati>/carico.toml con siluri_enhanced; gli scoppi seguono la regola dei colpi,
    // i siluri della CAPTOR e della PMR-1 il modello dell'Enhanced. da_mina: le unita' create da una mina (lo scoppio
    // e i suoi siluri), che non sono lanci del posatore.
    const CampoMinato& campo_minato() const { return campo_minato_; }
    // Mine dell'IA (docs/17 §3: S06, I04). Il campo gia' posato della missione: chiave `mine = { { tipo, x, y, lato,
    // armata }, ... }` (lato: urss, usa o neutrale; armata vero per difetto), sul fondo all'inizio. La posa durante la
    // missione: posa_mina_ia (m688.posa_mina(u, tipo [, quante, ogni_s])), sotto l'unita', una ogni `ogni_s`. Solo con i
    // siluri dell'Enhanced; falso se il tipo e' ignoto
    bool posa_mina_ia(int unita, const std::string& tipo, int quante = 1, double ogni_s = 60.0);
    bool da_mina(int id) const { return da_mina_.count(id) != 0; }
    bool comanda_siluro(int id, const ComandoFilo& c);         // via filo; falso se non e' un siluro del modello
    const std::vector<EventoSiluroMissione>& eventi_siluri() const { return eventi_siluri_; }
    const std::vector<LancioMissile>& lanci_missili() const { return lanci_missili_; }   // tutti, in ordine
    void ordina(int id, const OrdineMotore& o);                // ordine che prevale sugli script
    // il duomo del sonar perso in una collisione (DanniMissione, valuta_urto): la nave non ha piu' i sonar di scafo
    // (restano la schiera trainata e il VDS)
    void perdi_sonar_scafo(int id) { sonar_scafo_perso_.insert(id); }
    void perdi_elicottero(int id);   // l'hangar bruciato (DanniMissione): la nave non fa piu' decollare l'elicottero
    double metri_al_secondo(double nodi) const { return nodi * m_s_per_nodo_; }
    void segnala_non_implementata(const std::string& nome) { non_implementate_.insert(nome); }
    // esegue lo script `gestore` per l'unita' (con i parametri di un comando del giocatore, se ci sono)
    void esegui(int unita, const std::string& gestore, const std::vector<ParametroComando>* parametri = nullptr);
    double metri_per_unita() const { return metri_per_unita_; }
    Casuale& casuale() { return casuale_; }
    std::map<int, int>& variabili() { return variabili_; }
    int& ultima_trovata() { return ultima_trovata_; }
    int& bersaglio_memorizzato() { return bersaglio_memorizzato_; }
    std::vector<double>& termoclini() { return termoclini_; }

private:
    friend class RegistroMissione;   // legge gli eventi acustici e i rapporti richiesti
    struct StatoAswMissione;         // missioni/asw_missione.hpp
    std::unique_ptr<StatoAswMissione> asw_;
    struct StatoCollisioniMissione;   // missioni/collisioni_missione.hpp
    std::unique_ptr<StatoCollisioniMissione> collisioni_;
    bool collisioni_spente_ = false, crazy_ivan_spento_ = false;
    struct ComandoManovra { double rotta_deg = 0.0, velocita_kn = 0.0, quota_m = 0.0; };
    struct BatteriaIa {
        double carica = 0.8;
        bool snorkel = false, emersa = false;
        double v_eco_kn = 2.0, ore_eco = 175.0;   // autonomia a velocita' economica
        double v_max_kn = 16.0;                   // un'ora a tutta forza
        // sotto il pack (missione_diesel.cpp): l'acqua libera dove salire (una polynya del quadro della volta, o il
        // margine del pack), nel vero, e quando l'ha cercata
        std::optional<Vec2> rifugio;
        double rifugio_t = -1e9;
    };
    std::map<int, BatteriaIa> batterie_;
    std::vector<std::pair<double, int>> emersioni_forzate_;
    std::map<int, double> batterie_iniziali_;    // dalla missione (chiave `batteria`)
    bool batterie_attive_ = false;
    void passo_batterie(double dt);
    std::optional<Vec2> acqua_libera_ia(const UnitaClassica& u) const;   // il rifugio piu' vicino per salire sotto il pack
    std::optional<ComandoManovra> guida_batteria(const UnitaClassica& u) const;
    struct AvariaIa { bool propulsione_persa = false, incagliata = false; Vec2 punto; double prossimo_rumore = 0.0; };
    std::map<int, AvariaIa> avarie_;
    struct Rimorchio { int rimorchiatore = -1, rimorchiato = -1; double velocita_kn = 6.0, cavo_m = 300.0; };
    std::vector<Rimorchio> rimorchi_;
    std::vector<EventoRimorchio> eventi_rimorchio_;
    std::vector<std::pair<double, int>> avarie_iniziate_;
    std::map<int, std::string> stati_iniziali_;   // dalla missione (chiave `stato`)
    std::map<int, Vec2> relitti_;                 // il punto sul fondo
    Casuale caso_avarie_{0xA7A21Au};
    void passo_avarie(double dt);
    std::map<int, double> bqs15_;   // unita' col BQS-15 acceso: la prossima spazzata
    Casuale caso_bqs15_{0xB015u};   // gli errori delle sue misure
    std::vector<MinaScoperta> mine_scoperte_;
    std::map<int, std::vector<MinaScoperta>> mine_scoperte_ia_;
    Casuale caso_mine_ia_{0xB015A1u};   // gli errori delle misure dell'IA (non tocca quello del giocatore)
    // le mine nel settore di prua dello sonar del fondo di u, nel suo elenco; vero se ce n'e' una nuova. `scala` sulla
    // portata del BQS-15 (l'MG-70 come lui, STIMA)
    bool rileva_mine(const UnitaClassica& u, std::vector<MinaScoperta>& dove, double scala, Casuale& caso);
    std::optional<double> evita_mine(const UnitaClassica& u, double rotta) const;   // la rotta che passa al largo
    void passo_bqs15();
    std::vector<AcqueTerritoriali> acque_territoriali_;
    DanniIniziali danni_iniziali_;
    std::map<int, SonarSpeciale> sonar_speciale_;
    bool esercitazione_tutte_ = false;
    std::set<int> armi_esercitazione_, lanciatori_esercitazione_;
    std::vector<ColpoEsercitazione> colpi_esercitazione_;
    std::vector<PassaggioAcque> passaggi_acque_;
    double piuma_giocatore() const;   // la piuma della testa alzata del giocatore (nucleo/periscopi), 0 senza
    bool bombe_avvertimento(const UnitaClassica& nave, const UnitaClassica& giocatore);
    void spazzata_bqs15(UnitaClassica& u);
    // l'impulso di p ai sensori passivi di b (contro-scoperta: ping e BQS-15)
    void intercetta_impulso(UnitaClassica& p, UnitaClassica& b, const Sorgente& sp, const Ambiente& amb);
    std::vector<UrtoMissione> urti_;
    std::vector<QuasiCollisione> quasi_collisioni_;
    std::vector<IncidenteCavo> incidenti_cavo_;
    void passo_cavi();
    // il cavo della nave come punti (x, y) e quota, ogni ~5 m dalla poppa; vuoto se non fila o non ha la schiera
    std::vector<std::pair<Vec2, double>> punti_cavo(const UnitaClassica& nave) const;
    // il profilo del cavo dietro la poppa, (distanza orizzontale, quota): la forma vera della schiera del modello e del
    // pesce del VDS (missione_schiera.cpp, missione_vds.cpp), se no i numeri fissi della classe; vuoto senza cavo in acqua
    std::vector<std::pair<double, double>> profilo_cavo(const UnitaClassica& nave) const;
    void passo_collisioni(double dt);
    std::optional<ComandoManovra> guida_crazy_ivan(const UnitaClassica& u) const;
    struct StatoForze;                // missioni/forze_missione.hpp
    std::unique_ptr<StatoForze> forze_;
    bool forze_spente_ = false;
    void passo_forze();
    int lato_creduto(const UnitaClassica& o, const UnitaClassica& b) const;
    bool ignora_zona_amica(int id, int bersaglio);   // la P della prima ora dopo un attacco, estratta per coppia
    void passo_messaggi();
    DisturboRadio disturbo_da(int lato, Vec2 p) const;   // il disturbo radio della D3 su chi trasmette (missione_forze.cpp)
    // il falso ordine radio della D3 (missione_forze.cpp): la consegna, il dubbio secondo il livello, la verifica che
    // trasmette e si fa localizzare (radiogoniometria_ia, la stessa regola della trasmissione del giocatore), le ROE
    void passo_ordini_falsi();
    std::optional<bool> roe_ordine_falso(int id) const;
    void radiogoniometria_ia(int id, double durata_s, bool rapporto = false);   // rapporto: il battello che riferisce, coi numeri casuali della radio
    // gli incidenti provocati e la spia della D3: lo scoppio senza arma (missione_transitori.cpp), il quadro vero di una
    // parte dato all'altra (missione_forze.cpp)
    void scoppio_provocato(Vec2 p, double quota_m, double kg);
    void passo_spia();
    void passo_radio_sottomarini();   // il quadro radio ai sottomarini dell'IA: la quota d'antenna, l'ascolto, la caccia sul dato
    bool quadro_sottomarini_spento_ = false;
    int colpevole_creduto(int lato, int arma, int lanciatore, Vec2 p) const;   // D4: chi crede colpevole la parte (-1 ignoto)
    void consegna_rapporto(int ricevente, const RapportoContatto& r);
    std::optional<DatiIntelligence> intelligence_della_forza(int serie) const;   // i rapporti della forza al giocatore
    struct StatoArtiglieriaMissione;  // missioni/artiglieria_missione.hpp
    std::unique_ptr<StatoArtiglieriaMissione> artiglieria_;
    bool artiglieria_spenta_ = false;
    std::vector<ImpattoArtiglieria> impatti_artiglieria_;
    std::vector<EventoArtiglieria> eventi_artiglieria_;
    void passo_artiglieria();
    std::vector<AvvisoAsw> messaggi_asw_;
    bool asw_spento_ = false, evasione_spenta_ = false;
    std::optional<DifficoltaAsw> difficolta_asw_;
    void passo_asw(double dt);
    void pensa_asw(int id, double t);
    void esegui_azione_asw(int id, const AzioneAsw& a);
    struct GuidaAsw { double rotta, velocita; };
    // variazioni acustiche (R6): elica e macchine danneggiate dalla fisica dei sottomarini
    std::optional<bool> variazioni_scelte_;
    std::map<int, std::pair<double, double>> danni_acustici_;   // unita' -> danno elica, danno macchine
    std::set<int> sonar_scafo_perso_;
    std::set<int> hangar_persi_;   // le navi con l'hangar bruciato (perdi_elicottero)
    std::map<int, double> urto_acustico_;   // unita' -> rumore irradiato in piu' dalle collisioni (dB, DanniSottomarini)
    void danni_acustici(const DanniSottomarini& sub);
    // sonarista (S5d)
    std::optional<Esperienza> esperienza_sonarista_;
    std::unique_ptr<Sonarista> sonarista_;
    std::map<int, AscoltoSonar> ascolti_sonarista_;          // il migliore del secondo, per sorgente
    std::map<int, std::string> sensore_ascolto_;             // e il sensore che l'ha dato
    std::map<int, std::pair<double, std::vector<RigaOsservata>>> righe_sonarista_;   // le righe, ogni 5 s
    std::map<int, std::vector<double>> proprie_sonarista_;   // fra quelle, le proprie: vanno col ricalcolo, non solo nel suo secondo
    std::map<int, std::vector<LetturaDemon>> demon_sonarista_;   // le serie del DEMON del fascio, col ricalcolo (passo B)
    std::map<int, int> sorgente_di_traccia_;                 // Sierra -> sorgente: gli scambi
    std::vector<AvvisoSonarista> avvisi_sonarista_;
    std::vector<ScambioTracce> scambi_tracce_;
    double prossimo_sonarista_ = 0.0;
    Casuale caso_sonarista_{0x50AA};
    void ascolto_per_sonarista(const UnitaClassica& b, const Sorgente& s, const Sensore* sensore, double se, double errore);
    void passo_sonarista();
    struct SonaristaIa {
        std::unique_ptr<Sonarista> s;
        Casuale caso;
        std::map<int, AscoltoSonar> ascolti;         // il migliore del secondo, per sorgente
        std::map<int, const Sensore*> sensore;
        std::map<int, std::pair<double, std::vector<RigaOsservata>>> righe;
        std::map<int, std::vector<double>> proprie;   // fra le righe, le proprie: col ricalcolo, non solo nel suo secondo
        std::map<int, std::vector<LetturaDemon>> demon;   // le serie del DEMON del fascio, col ricalcolo (passo B)
        double prossimo = 0.0;
    };
    std::map<int, SonaristaIa> sonaristi_ia_;
    std::map<int, std::string> sonaristi_iniziali_;   // chiave d'unita' `sonarista`
    std::vector<ClassificazioneIa> classificazioni_ia_;
    std::vector<IdentificazioneVisiva> identificazioni_visive_;
    std::map<std::pair<int, int>, std::size_t> ultima_identificazione_;   // (osservatore, contatto) -> indice
    std::vector<OsservazioneAob> osservazioni_aob_;
    std::vector<StimaRottaTma> stime_tma_giocatore_;
    std::vector<MisuraTelemetro> misure_telemetro_;
    ContiGiocatore conti_;
    bool mine_dentro_ = false, mine_bqs_ = true;   // il passaggio in corso in un campo di mine
    int mine_integrita_ = 0;
    void passo_conti(double dt);
    void leggi_luci_unita(int id, const std::string& nome);
    bool ordine_luci(const Ordine& o);
    void vedetta_luci(double passo);
    CondizioniVista condizioni_luci(Vec2 p) const;
    double portata_luci(int id, Vec2 da, const Osservatore& o) const;
    bool vista_automatica_di_notte(const UnitaClassica& o, const UnitaClassica& b) const;   // missione_luci.cpp
    std::map<std::string, luci::Configurazione> luci_conf_;
    bool luci_attive_ = false;
    std::map<int, luci::Stato> luci_chiave_;
    std::map<std::pair<int, int>, FanaliVisti> fanali_ia_;   // (osservatore, contatto)
    std::vector<PingUnita> ping_unita_;
    // i siluri antisom delle navi (missione_antisom_navi.cpp): i tubi carichi, la riserva, la ricarica, l'arma in acqua
    struct AntisomNave {
        int tubi = -1, riserva = 0;
        double prossima_ricarica = 0.0, attesa_fino = 0.0;
        int in_acqua = -1;
        // l'elicottero imbarcato (oltre la portata dei tubi): in volo, pronto di nuovo, gia' della missione
        int elicottero = -1;
        double elicottero_pronto = 0.0;
        std::optional<bool> elicottero_della_missione;
        double ultimo_cue = -1e18;   // l'ultimo contatto passato all'elicottero in volo (la nave lo dirige, MATCH)
    };
    std::map<int, AntisomNave> antisom_navi_;
    bool lancia_elicottero(int nave, Vec2 punto, int bersaglio);
    void passo_elicotteri_navi();   // l'appontaggio (dal passo dell'ASW)
    std::optional<bool> luci_giocatore_;
    std::vector<CambioLuci> cambi_luci_;
    std::optional<Casuale> caso_luci_;
    double prossime_identificazioni_ia_ = 0.0;
    std::optional<Casuale> caso_libretto_;
    bool ordine_libretto(const Ordine& o);
    void passo_libretto_ia();
    void nota_identificazione(int osservatore, int contatto, const std::string& classe, bool giusta, double distanza_m);
    std::optional<int> parte_della_classe(const std::string& classe) const;
    std::optional<int> lato_visivo(int osservatore, int contatto) const;   // la parte dell'identificazione fresca
    bool sonaristi_ia_spenti_ = false, esm_ia_spento_ = false;
    void ascolto_ia(const UnitaClassica& o, const UnitaClassica& b, const Sorgente& s, const Sensore* sensore, double se, double errore);
    // l'ascolto di una sorgente qualsiasi (unita' o istanza del paesaggio) per il sonarista dell'IA `o`
    void ascolto_ia(const UnitaClassica& o, int sorgente, double rilevamento_vero, bool biologico, const Sensore* sensore, double se, double errore);
    double ultimo_ascolto_paesaggio_ia_ = -1e18;
    void passo_sonaristi_ia();
    // meteo (M2)
    std::unique_ptr<DatiMeteo> dati_meteo_;
    std::unique_ptr<Meteo> meteo_;
    ModoMeteo modo_meteo_ = ModoMeteo::semplificato;
    bool meteo_tentato_ = false;
    StatoMeteo meteo_qui_ = meteo_semplificato();
    double prossimo_meteo_ = 0.0;
    double affiora_fino_ = -1.0;
    // fulmini al sonar (M2 con M5): i lampi nube-mare vicini come transitori della scena acustica
    std::vector<Transitorio> fulmini_;
    // transitori delle armi e degli scafi (R7, missione_transitori.cpp): lanci, esplosioni, affondamenti, implosioni,
    // ping delle teste; le armi, i colpi e gli affondamenti gia' suonati
    std::vector<Transitorio> transitori_armi_;
    std::set<int> armi_viste_, affondati_visti_;
    std::size_t colpi_suonati_ = 0;
    std::map<int, double> ultimo_ping_testa_;
    int prossimo_lancio_tr_ = 960000, prossimo_scoppio_tr_ = 980000;
    struct DimensioniClasse { double lunghezza_m = 0.0, larghezza_m = 0.0, pescaggio_m = 0.0; bool civile = false; };
    std::map<std::string, DimensioniClasse> dimensioni_classi_;   // classi.toml: implosione (R7) e spoletta dei siluri
    void carica_dimensioni_classi(const std::string& cartella_dati);
    void passo_transitori_armi();
    // esche semoventi in corsa (missione_esche.cpp): i dati, l'istante e la rotta del lancio, la classe del lanciatore
    struct EscaMobile {
        const DatiEsca* dati = nullptr;   // nullptr: il falso contatto del vetrino di Dio (D3), senza lanciatore
        double inizio = 0.0, rotta0 = 0.0;
        std::string classe_lanciatore;    // la classe di cui suona (il falso contatto: quella imitata)
        // il falso contatto: la categoria della classe imitata, la fedelta' della firma, il ping (0: muto), la velocita'
        // costante, l'intervento che l'ha creato, e chi l'ha preso col ping (unita' -> istante: AscoltoSonar::eco_esca)
        std::string categoria;
        double fedelta = 1.0, ping_s = 0.0, prossimo_ping = 0.0, velocita_kn = 0.0;
        int intervento = -1;
        std::map<int, double> eco_da;
    };
    std::map<int, EscaMobile> esche_mobili_;
    bool esche_mobili_spente_ = false;
    void passo_esche_mobili();
    void ping_falso(UnitaClassica& u, const EscaMobile& x);   // il ping del falso contatto, per l'intercettazione
    void falso_contatto(AscoltoSonar& a, int sorgente, int ascoltatore) const;   // la verita' per il sonarista (missione_sonarista.cpp)
    double ultimi_lampi_ = 0.0, ultimo_avviso_fulmine_ = -1e18;
    int prossimo_fulmine_ = 950000;
    std::vector<FulmineSentito> fulmini_sentiti_;
    // bollettini meteo alla sala radio (M4): il prossimo, la burrasca annunciata, la previsione chiesta
    double prossimo_bollettino_ = -1.0;
    int burrasca_annunciata_ = 0;    // 0 niente, 1 GALE, 2 STORM
    double previsione_chiesta_ = -1.0;
    void bollettini_meteo();
    MessaggioRadio bollettino(double emessa, bool chiesto) const;
    Casuale caso_meteo_{0x3E7E0};   // il flusso degli effetti del meteo (affioramento)
    std::map<std::string, double> lunghezze_;   // classe -> lunghezza (m), per il moto delle navi col mare
    void passo_meteo();
    struct StatoPeriscopiMissione;   // missioni/periscopi_missione.hpp
    std::unique_ptr<StatoPeriscopiMissione> peri_;
    bool peri_tentato_ = false;   // i dati si cercano una volta sola
    std::vector<Ordine> ordini_peri_in_attesa_;   // arrivati prima del posto (al secondo 0, prima di DanniMissione)
    std::vector<Messaggio> messaggi_peri_;
    std::vector<FotoPeriscopio> foto_;
    std::vector<EsposizionePeriscopio> esposizioni_lunghe_;
    std::vector<GuastoRichiesto> guasti_;
    void passo_periscopi(double dt);
    bool ordine_periscopio(const Ordine& o);
    void danni_periscopio(const DanniSottomarini* sub);
    void casse_squarciate_ia(const DanniSottomarini& sub);   // missione_artiglieria.cpp: l'IA con la cassa squarciata resta a galla
    bool testa_ia_fuori(int id) const;
    std::optional<TestaEsposta> testa_giocatore() const;   // per il radar: la testa vera, se e' fuori
    bool esm_in_testa() const;
    std::optional<GuidaAsw> guida_asw(const UnitaClassica& u) const;
    std::optional<ComandoEvasione> guida_evasione(const UnitaClassica& u) const;
    BattelloEvasione battello_evasione(const UnitaClassica& u) const;
    void passo_evasione();
    void lancia_esca_ia(const UnitaClassica& b);
    // navigazione incerta (missione_navigazione.cpp)
    bool navigazione_attiva_ = false;
    std::optional<CatalogoNavigazione> cat_nav_;
    std::optional<NavigazioneStimata> nav_;
    struct FixInCorso {
        std::string tipo;
        double passaggio = 0.0, fine = 0.0;   // il satellite (o l'inizio), la fine dell'esposizione
        bool radar = false, radar_prima = false;
    };
    std::optional<FixInCorso> fix_;
    MotivoPuntoNave ultimo_motivo_ = MotivoPuntoNave::nessuno;
    std::vector<EsitoPuntoNave> punti_nave_;
    double sigma_iniziale_nm_ = 0.1;
    bool inerziale_guasto_ = false, gps_ = false, inerziale_detto_ = false, inerziale_danni_ok_ = true, schiera_danni_ok_ = true;
    // l'IA (stesso interruttore): un navigatore per sottomarino dell'IA; esposto, prende il punto nave migliore che puo'
    std::map<int, NavigazioneStimata> nav_ia_;
    std::set<int> inerziale_ia_giu_;
    Soccorso soccorso_;
    // i transitori dello scafo e i posti di combattimento (missione_scafo.cpp)
    bool silenzio_giocatore_ = false;
    Esperienza equipaggio_giocatore_ = Esperienza::medio;
    bool posti_ = false;
    double posti_ultimo_ = -1e18;                  // l'ultima ragione per restarci
    std::vector<CambioPosti> cambi_posti_;
    std::vector<double> soffiate_giocatore_;
    std::size_t posti_allarmi_visti_ = 0;
    struct ScafoTransitori {
        std::optional<Casuale> caso;
        double quota_prec = 0.0, rateo_prec = 0.0, ultimo_piani = -1e18;
        double quota_partenza = 0.0, quota_scricchiolio = 0.0;   // del cambio di quota in corso
        bool in_cambio = false;
        double pompa_alle = -1.0, prossima_pompa = -1.0;
        int guardia = -1;                          // l'ultima guardia (ora locale / 4)
    };
    std::map<int, ScafoTransitori> scafi_tr_;
    int prossimo_scafo_tr_ = 950000;               // la fascia 950000-959999
    std::size_t urti_scafo_visti_ = 0, colpi_scafo_visti_ = 0, eventi_scafo_visti_ = 0;
    std::set<int> armi_scafo_viste_;               // i lanci gia' contati per la pompa di compensazione
    std::set<int> scafo_tr_sentiti_;               // i transitori dello scafo gia' valutati da chi ascolta
    std::map<std::string, int> scafo_nati_;
    std::vector<ScafoSentito> scafo_sentiti_;
    void passo_transitori_scafo(double h);
    void scafo_transitorio(const UnitaClassica& u, const char* voce);
    void ascolta_transitori_scafo();
    void passo_posti();
    // il vetrino di Dio (missione_interventi.cpp)
    std::vector<Intervento> interventi_;
    std::vector<StatoIntervento> stati_interventi_;
    int prossimo_intervento_ = 0;
    void passo_interventi();                                  // all'inizio del passo: applica e fa finire
    bool applica_intervento(const Intervento& x, StatoIntervento& st);
    void fine_intervento(const Intervento& x, StatoIntervento& st);
    void tieni_ordini_interventi();                           // a fine passo: gli ordini attivi scavalcano la dottrina
    struct RipristinoIntervento {   // come era prima, per la fine di quelli con durata
        double velocita_ordinata = 0.0, quota_ordinata = 0.0, rotta_ordinata = 0.0;
        bool navigazione = false;
        Vec2 meta;
        std::optional<StatoMeteo> meteo;   // il meteo di prima, se era fisso (senza: si torna al meteo del teatro)
        std::optional<std::map<CategoriaRoe, bool>> roe;   // le ROE della parte prima (le segue chi non ha ancora ricevuto)
    };
    std::map<int, RipristinoIntervento> ripristini_;
    std::map<int, std::set<int>> roe_ricevute_;               // intervento roe -> le unita' della parte che l'hanno ricevuto
    void giro_radio_roe();                                    // a ogni passo: chi riceve le ROE nuove           // id dell'intervento ->
    std::set<int> sonar_ia_giu_, schiera_ia_giu_;   // sonar_ia: guasti dai danni
    bool gps_opzione_ = false;
    // il quadro del fondale del giocatore (missione_fondale.cpp)
    std::optional<ParametriEcoscandaglio> par_eco_;
    std::optional<ParametriBqs15Fondo> par_bqs_fondo_;
    // l'IA che sonda il fondo (missione_fondale_ia.cpp)
    struct FondaleIa {
        QuadroFondale quadro;
        Casuale caso;
        double scala = 0.0, ultimo_ping = -1e9;
        std::optional<ParametriBqs15Fondo> bqs;   // il sonar del fondo della classe (bqs15_fondo_di_classe)
        double prossima_spazzata = 0.0;
    };
    std::map<int, FondaleIa> fondale_ia_;
    std::string acustica_toml_;            // da dati_fondale: i sonar del fondo delle classi dell'IA
    double prossimo_fondale_ia_ = 0.0;
    void passo_fondale_ia();
    // sotto il ghiaccio (missione_ghiaccio.cpp)
    std::optional<CampoGhiaccio> ghiaccio_;
    bool ghiaccio_tentato_ = false;
    struct VoltaUnita {
        QuadroVolta quadro;
        Casuale caso;
        std::optional<ParametriBqs15Fondo> bqs;   // il sonar del fondo (BQS-15, MG-70); senza, solo il sondatore no
        bool migliorato = false;                 // il 688I: +20% sulle chiglie
        double prossimo_sondaggio = 0.0, prossima_spazzata = 0.0, prossimi_avvisi = 0.0;
        bool libera_sopra = false;
        std::set<int> avvisi;                    // i messaggi gia' detti, finche' restano veri
    };
    std::map<int, VoltaUnita> volta_;
    std::map<int, std::pair<Vec2, double>> passo_ghiaccio_prec_;   // posizione e quota al passo prima (l'urto)
    std::map<int, double> urto_ghiaccio_fino_;
    bool prepara_ghiaccio();
    VoltaUnita* volta_di(const UnitaClassica& u);
    void passo_ghiaccio(double dt);
    void spazzata_volta(const UnitaClassica& u);   // dal BQS-15 (missione_bqs15.cpp) e dal passo per l'MG-70
    void urto_ghiaccio(UnitaClassica& u, double dt);
    double cima_vela(const UnitaClassica& u) const;   // la quota della cima della vela (chiglia vera meno diametro e vela)
    // il carattere del pack per l'acustica nel punto (missione_ghiaccio.cpp): le creste dal campo, le fessure dal freddo, la
    // rugosita' dal tipo di ghiaccio; per cella di 5 km come il meteo d'ascolto, ogni 10 minuti
    void carattere_ghiaccio(Ambiente& amb, Vec2 p, double t_aria_c) const;
    std::vector<EcoFalso> echi_falsi_;
    void echi_chiglie_ping(const UnitaClassica& p, const Sensore& sa);   // da impulso_sonar_enhanced, per il giocatore
    // gli echi delle chiglie al ping di `p` (sotto la banchisa), senza toccare quelli del giocatore: anche per l'IA
    std::vector<EcoFalso> echi_chiglie_di(const UnitaClassica& p, const Sensore& sa);
    // i falsi echi delle chiglie per la testa attiva d'un siluro sotto la banchisa (missione_ghiaccio.cpp): vero se ce ne
    // sono, e allora `con_chiglie` e' `sorgenti` piu' le chiglie (bersagli fermi e muti, come le esche)
    bool echi_chiglie_siluro(const Siluro& s, const std::vector<SorgenteAcustica>& sorgenti,
                             std::vector<SorgenteAcustica>& con_chiglie) const;
    struct CarattereGhiaccio { double t = -1e9, creste = 0.0; };
    mutable std::map<std::pair<int, int>, CarattereGhiaccio> carattere_celle_;
    std::optional<QuadroFondale> quadro_g_;
    ProgrammaEcoscandaglio prog_eco_;
    PotenzaEco potenza_eco_ = PotenzaEco::piena;
    double scala_eco_ = 0.0, prossimi_avvisi_fondo_ = 0.0;
    std::optional<Casuale> caso_fondale_;
    std::optional<Sondaggio> ultimo_sondaggio_;
    std::set<int> avvisi_fondo_attivi_;   // i tipi d'avviso gia' detti, finche' restano veri
    bool fondale_tentato_ = false;
    void dati_fondale(const std::string& cartella_dati);
    bool prepara_quadro_fondale();
    bool ordine_ecoscandaglio(const Ordine& o);
    void passo_quadro_fondale();
    void spazzata_fondo(const UnitaClassica& u);   // dal BQS-15 del giocatore
    std::map<int, double> nav_ia_prossimo_fix_;
    double nav_ia_giro_ = 0.0;
    void passo_navigazione_ia(double dt);
    Vec2 errore_navigazione(int id) const;   // stimata - vera dell'unita' (0 senza navigatore)
    GestoriEsito gestori_esito_;              // letti dal testo dello script al caricamento
    std::map<int, Vec2> mete_assolute_;       // l'ultima meta di vai_a per unita' (l'errore sposta solo quella)
    Vec2 meta_per_navigazione(const UnitaClassica& u, Vec2 meta) const;
    void navigazione_enhanced(const std::string& cartella_dati);
    void passo_navigazione(double dt);
    CondizioniPuntoNave condizioni_punto_nave() const;
    bool avvia_punto_nave(const std::string& tipo);
    // la sandbox
    std::map<int, RegolazioniParte> regolazioni_;
    bool senza_giocatore_ = false;
    std::vector<NotaComandante> note_comandante_;
    // sandbox V2, R1-R6 (missione_quadro.cpp)
    bool statistiche_ = false;
    std::vector<ScopertaContatto> scoperte_contatti_;
    std::vector<ScopertaPossibile> scoperte_possibili_;
    std::set<std::tuple<int, int, std::string>> possibili_visti_;
    std::map<std::pair<int, int>, std::pair<bool, std::string>> stato_contatti_;   // (oss, bers) -> solido, sensore
    std::vector<LancioArma> lanci_;
    std::optional<std::pair<int, StimaTma>> soluzione_giocatore_;
    std::vector<ManovraEvasione> evasioni_;
    std::vector<Emissione> emissioni_;
    std::map<std::pair<int, std::string>, std::size_t> radar_accesi_;   // (unita', radar) -> voce aperta in emissioni_
    std::size_t messaggi_ia_emessi_ = 0, eventi_radio_visti_ = 0;
    std::vector<EsitoPuntoNave> punti_nave_ia_;
    void possibile(int osservatore, int bersaglio, const std::string& sensore, double eccesso_db);
    void lancio_per_statistiche(int arma, const UnitaClassica* lanciatore, int bersaglio, const std::string& classe);
    void passo_statistiche();
    void radar_per_statistiche(const std::vector<std::pair<int, std::string>>& accesi);
    // la situazione per le decisioni della parte `chi` verso `altro`: la coppia se e' cambiata in partita, se no la
    // partenza della regolazione di `chi`, se no quella della missione
    Situazione situazione_di(int chi, int altro) const;
    // fase 4 B (missione_armi_ia.cpp)
    struct StatoArmiIa;               // missioni/armi_ia_missione.hpp
    std::unique_ptr<StatoArmiIa> armi_ia_;
    bool armi_ia_spente_ = false;
    void armi_ia_enhanced(const std::string& cartella_dati);
    void passo_armi_ia();
    // il lancio di un sottomarino dell'IA col carico: il modello scelto, o falso se non ha siluri nei tubi
    bool arma_ia_al_lancio(const UnitaClassica& l, const UnitaClassica& u, const DatiSiluro*& d);
    void guida_filo_ia(int siluro, const DatiSiluro& d);
    // il tiro di risposta (lo snap shot del 24 §8.6): un siluro dai tubi lungo il rilevamento di chi ha lanciato; vero se e' partito
    bool tiro_di_risposta(int sottomarino, int bersaglio, bool rotta_vera = false);   // rotta_vera: senza contatto, il rilevamento vero
    Esperienza esperienza_di_ia(const UnitaClassica& o) const;
    std::optional<StimaTma> stima_tma_ia(int id, int bersaglio) const;   // la soluzione della TMA dell'IA, se c'e'
    std::map<int, double> ordini_script_;   // unita' -> l'ultimo istante in cui il suo script ha dato rotta o velocita'
    // fase 4 C (missione_reazioni.cpp)
    bool reazioni_spente_ = false;
    void passo_reazioni();
    struct ReazioneInCorso {
        double inizio, fine;
        Vec2 verso;
        double velocita_kn;
        double velocita_prima = -1.0;
        bool ping = false;            // tiene sotto: l'attivo continuo
        double prossimo_ping = 0.0;
        int gravita = 0;              // una reazione piu' grave sostituisce una in corso
    };
    std::map<int, ReazioneInCorso> reazioni_in_corso_;
    std::size_t eventi_reazioni_visti_ = 0;
    std::set<std::pair<int, int>> reazioni_fatte_;   // (evento, unita'): una sola reazione per evento
    Casuale caso_reazioni_{0x5EAC7u};
    std::vector<ReazioneIa> reazioni_ia_;
    // fase 4 D
    void passo_pace();
    void passo_pedina_vicino(double dt);
    double prossima_pace_ = 0.0;
    std::map<std::pair<int, int>, double> incsea_vicini_;   // coppie di navi gia' sotto i 1000 yd (una violazione per volta)
    std::vector<ViolazioneIncsea> incsea_;
    std::optional<bool> incsea_attivo_;   // la chiave della missione, letta al primo passo
    std::set<int> allontanati_ping_;      // sottomarini gia' estratti per l'allontanamento (P 0,7), per pinger
    std::map<int, double> prossimo_ti_tengo_;
    std::map<int, double> pedina_script_da_;   // unita' ostile che tiene il giocatore come bersaglio, da quando
    std::set<int> pedina_script_detta_;
    // fase 4 A (missione_difesa.cpp)
    bool difesa_spenta_ = false;
    void passo_difesa();
    void stimola_evasione(int b, StimoloEvasione s, Vec2 da, double pavimento = 0.0, int minaccia = -1);
    struct ManovraDifesa {
        double fine;
        Vec2 verso;
        double velocita_kn;
        double velocita_prima = -1.0;   // quella dello script, rimessa alla fine
    };
    // la manovra dell'IA del motore in corso (difesa, poi TMA): nel movimento, sopra lo script e sotto i limiti dei danni
    std::optional<ManovraDifesa> manovra_ia(const UnitaClassica& u) const;
    void fine_manovra_ia(UnitaClassica& u, const ManovraDifesa& m);
    std::map<int, ManovraDifesa> manovre_difesa_;
    struct PedinaVicino {
        int bersaglio = -1;
        double distanza_m = 400.0, sopra_m = 0.0;
        Vec2 pos_vista;                                   // dal contatto
        double rotta_vista = 0.0, quota_vista = 0.0, velocita_vista = 0.0;   // col ritardo
        double cambio_da = -1.0, ritardo_s = 0.0;         // un cambio del pedinato notato e non ancora visto
        double ultimo_contatto = 0.0;
    };
    std::map<int, PedinaVicino> pedine_vicine_;   // per pedinatore
    void manovra_difesa(int id, Vec2 verso, double velocita_kn, double durata_s);
    std::set<std::pair<int, int>> siluri_evitati_;   // (nave, siluro)
    std::map<int, double> anticollisione_;            // sottomarino -> fine della manovra
    std::size_t eventi_difesa_visti_ = 0;
    Casuale caso_difesa_{0xD1FE5Au};
    std::vector<EscaIa> esche_ia_;
    std::vector<RitirataIa> ritirate_ia_;
    // aereo controllato contro un sottomarino immerso: il segnale per il contatto dell'aereo dalla traccia del
    // controllore (6000 col fix, 600 col contatto, -1 senza); nullopt se decide il classico
    std::optional<double> segnale_asw(const UnitaClassica& o, const UnitaClassica& b) const;
    std::optional<Sorgente> sorgente_aereo_per(const UnitaClassica& aereo, const UnitaClassica& ascoltatore) const;
    void transitori_asw(std::vector<Sorgente>& sorgenti, double finestra_s) const;
    void messaggio_asw(int codice, int unita, std::optional<double> rilevamento);
    RegistroMissione registro_;
    struct StoriaContatto { double cumulato_s = 0.0, ultimo = -1.0; };
    std::map<std::pair<int, int>, StoriaContatto> storia_contatti_;   // (osservatore, contatto)
    mutable std::set<int> osservatori_storia_;   // oltre al giocatore
    void passo_storia_contatti(double dt);
    std::vector<IstanzaSorgente> reti_;
    void passo_reti();
    void leggi_variante(int indice_tabella);
    std::vector<int> lista() const;                          // unita' in gioco nell'ordine della lista originale
    static std::size_t posto(const std::vector<int>& l, int id);   // indice nella lista; -1 o uscita: la testa
    void passo_sensori();
    void valuta(UnitaClassica& o, UnitaClassica& b);
    void perdi_contatto(UnitaClassica& o, int b);
    int contatto_orfano(const UnitaClassica& o, int b) const;   // contatto senza unita' prima di quello di b, o -1
    void navigazione(double dt);
    struct Pilota {   // ordini corretti dal pilota dell'IA per il fondale
        std::optional<double> rotta;
        double quota, velocita, rateo_quota;
    };
    Pilota pilota(UnitaClassica& u, Vec2 meta);
    AcquaRichiesta acqua_ia(const UnitaClassica& u) const;
    const AcqueNavigabili& acque(double minima);
    bool secca_avanti(const UnitaClassica& u, double rotta, double portata, double richiesta) const;
    std::optional<double> evita_secche(UnitaClassica& u, double rotta_voluta, double portata, double richiesta);
    void urta_fondo(UnitaClassica& u, double velocita_urto_m_s);
    void impatti();
    void controlli_giocatore(UnitaClassica& u);
    bool a_contatto(const UnitaClassica& a, const UnitaClassica& b) const;   // finestra d'impatto, senza collegati
    bool impatto(UnitaClassica& o, UnitaClassica& b);                         // colpo nella coppia dei sensori
    void colpisci(UnitaClassica& arma, UnitaClassica& vittima);
    void uccisione(int lanciatore);
    void visita();
    void colloca(UnitaClassica& u);
    bool ostili(const UnitaClassica& a, const UnitaClassica& b) const;

    struct MemoriaScript;                        // missione.cpp: tetto della memoria Lua (LimitiScript)
    std::unique_ptr<MemoriaScript> memoria_script_;
    static void* alloca_script(void* ud, void* p, std::size_t vecchia, std::size_t nuova);
    LimitiScript limiti_;
    int chiama_lua(int argomenti, int risultati);   // lua_pcall coi limiti degli script
    lua_State* L_ = nullptr;
    int rif_missione_ = 0, rif_script_ = 0;
    std::string id_, titolo_, teatro_;
    int variante_ = 0;
    double metri_per_unita_ = 37.04;
    double m_s_per_nodo_ = NODO;
    std::vector<EventoAcustico> eventi_;
    double tempo_ = 0.0;
    std::map<int, UnitaClassica> unita_;
    int prossimo_id_ = 0;
    std::vector<Timer> timer_;
    std::uint64_t ordine_timer_ = 0;
    std::vector<Messaggio> messaggi_;
    std::optional<int> esito_;
    std::map<int, int> variabili_;
    std::vector<double> termoclini_;
    int ultima_trovata_ = -1, bersaglio_memorizzato_ = -1;
    std::vector<Contesto> contesti_;
    std::set<std::string> non_implementate_;
    std::vector<std::string> errori_;
    Casuale casuale_;
    std::shared_ptr<const Fondale> fondale_;
    std::shared_ptr<const Fondale> carte_[2];   // per Parte
    std::map<std::string, double> pescaggi_;
    std::map<int, std::unique_ptr<AcqueNavigabili>> acque_;   // per soglia d'acqua, in metri
    // A2 (navigazione incerta dell'IA): il fondo come lo crede un sottomarino dell'IA col navigatore e il suo quadro del
    // fondale (fondale_ia_): il pilota governa sulla stima e sulla carta della sua parte, gli incagli restano sul vero
    struct FondoCreduto {
        const QuadroFondale* quadro = nullptr;
        const Fondale* carta = nullptr;
        Parte parte = Parte::nato;
        int raggio = 0;           // indice del raggio di prudenza (passi di RAGGIO_PASSO_M)
        Vec2 errore;              // stimata - vera
        double sigma_m = 0.0;     // del navigatore
    };
    std::optional<FondoCreduto> fondo_creduto(const UnitaClassica& u) const;
    double profondita_pilota(const UnitaClassica& u, Vec2 p) const;   // la vera, o la creduta prudente (A2)
    const Fondale& carta_prudente(const FondoCreduto& fc);   // la carta con le secche allargate del raggio (per le rotte)
    const AcqueNavigabili& acque_carta(const FondoCreduto& fc, double minima);
    std::map<std::pair<int, int>, std::unique_ptr<Fondale>> carte_prudenti_;                  // (parte, raggio)
    std::map<std::tuple<int, int, int>, std::unique_ptr<AcqueNavigabili>> acque_carta_;   // (parte, raggio, soglia)
    std::vector<std::pair<int, double>> spostamenti_;
    int conto_5100_ = 18;            // DS:5100
    int visitata_ = -1;              // DS:1F5E: prossima unita' da visitare (2011:02F3; -1 o uscita: la testa)
    int ultimo_lanciatore_ = 0;      // DS:6503: lanciatore dell'ultima arma andata a segno
    double quota_visita_ = -1.0;     // DS:1F46: quota del giocatore locale alla visita precedente (strati termici)
    // sala radio dell'Enhanced
    std::unique_ptr<SalaRadio> radio_;
    // R3: i messaggi che vanno in onda a una condizione (`se`) e quelli della missione per id (m688.radio)
    struct CondizioneRadio {
        std::string ricevuto, trasmesso, evento;      // id ricevuto; nome della trasmissione; nome dell'evento del registro
        std::vector<int> colpite, fuori;              // una qualunque colpita; una qualunque fuori dal gioco
        bool variabile = false;
        int indice = 0, valore = 0;
        std::string op = "==";
    };
    struct MessaggioCondizionato {
        MessaggioRadio m;
        double dopo = 0.0;
        CondizioneRadio se;
    };
    bool data_imposta_ = false;                     // imposta_data_ora: la data degli ordini radio non la cambia
    std::map<int, TestoRadio> messaggi_missione_;   // codice 1000 + n -> testo
    void meteo_fisso_della_missione();
    std::vector<MessaggioCondizionato> radio_condizionati_;
    std::map<std::string, MessaggioRadio> radio_catalogo_;
    bool condizione_radio(const CondizioneRadio& c) const;
    std::vector<Messaggio> messaggi_radio_;
    struct FixRadio {
        double quando;   // s di missione: arriva la minaccia
        Vec2 pos;
    };
    std::vector<FixRadio> fix_radio_;
    struct RapportoRadio {   // file radio, rapporti.contatto: il premio di un rapporto di contatto
        std::string gestore;
        double ritardo_s = 0.0;
        bool una_volta = true, usato = false;
    };
    RapportoRadio rapporto_contatto_;
    struct BoeSlot {
        int boe = 0;
        double ritardo_min_s = 300.0, ritardo_max_s = 1800.0, portata_vhf_m = 40000.0;
    };
    BoeSlot slot_;
    std::vector<std::pair<double, Vec2>> slot_attese_;          // quando trasmette e da dove
    std::vector<std::pair<double, std::string>> supporti_;      // gestori del premio in arrivo
    std::set<int> neutrali_;
    std::string gestore_saltate_;              // R2: il guinzaglio sovietico
    std::set<std::string> zone_dentro_;        // R2: zone protette in cui e' il giocatore
    std::optional<PosaUnita> posa_zona(const Zona& z) const;
    // emettitori radar ed EMCON dell'Enhanced (punto 23)
    std::map<int, Emcon> emcon_;
    std::map<int, StatoEmcon> emcon_iniziali_;   // chiave d'unita' `emcon` (S13)
    std::unique_ptr<CatalogoElettronico> catalogo_el_;
    std::unique_ptr<EsmBattello> esm_giocatore_;
    std::vector<Messaggio> messaggi_esm_;
    void passo_elettronica(double dt);
    double altezza_radar(const UnitaClassica& u, const Radar& r) const;
    DatiIntelligence compila_intelligence(const RichiestaIntelligence& r);
    int esm_nemico_vicino(Vec2 da) const;
    void esito_radiogoniometria(const EsitoTrasmissione& e, int esm, Vec2 da);
    void giudica_lancio_giocatore(const UnitaClassica& g);
    std::string file_missione_, lato_missione_;
    double latitudine_ = 60.0, longitudine_ = 0.0, ora_utc_s_ = 0.0;
    std::optional<std::pair<double, double>> luogo_sole_;
    int giorno_ = 24, mese_data_ = 10, anno_ = 1989;
    std::vector<std::string> briefing_it_, briefing_en_;
    void passo_radio(double dt);
    StatoAntenne antenne_giocatore() const;
    void messaggio_radio(int codice);
    void contatto_da_radiogoniometria(std::optional<Vec2> pos, int solo_a, std::optional<Vec2> sorgente = std::nullopt);
    // acustica dell'Enhanced
    struct CoppiaAcustica {
        Fluttuazione fluttuazione;
        double ultima = -1.0;   // s di missione dell'ultima valutazione della coppia
        // l'IA con la schiera (simmetria, punto 4): il lato dell'ambiguita' conica che crede, scelto a caso al primo
        // rilevamento della schiera (nullopt: nessuno) e risolto da una virata o da un sensore senza specchio
        std::optional<bool> lato_vero;
        double rotta_lato = 0.0;   // la rotta dell'osservatore quando ha scelto il lato
    };
    std::shared_ptr<const LibreriaCampioni> campioni_;   // R5: deve vivere quanto catalogo_ (puntatori nelle voci)
    bool usa_campioni_ = true;
    std::unique_ptr<CatalogoAcustico> catalogo_;
    std::optional<ClimaTeatro> clima_;
    int mese_ = 0;   // 0: il mese predefinito del teatro (ClimaTeatro::mese_predefinito)
    bool data_dal_file_ = false;   // la missione ha la sua `data`
    // l'effetto pomeriggio (dossier 02 §3.5): un profilo per quarto d'intensita', fatto una volta per il mese (i campi
    // dei raggi riconoscono il profilo dal puntatore: cambia solo col quarto), scelto ogni 10 minuti di gioco
    std::array<std::optional<ProfiloSuono>, 5> pomeriggio_;
    // lo strato che cambia (D4): i profili modificati per (profilo di partenza, zona, passo della transizione), con i
    // puntatori stabili perche' entrano nella chiave delle cache dei raggi
    std::vector<ZonaStrato> zone_strato_;
    mutable std::map<std::tuple<const ProfiloSuono*, std::size_t, int>, std::unique_ptr<ProfiloSuono>> profili_strato_;
    int quarto_pomeriggio_ = 0, mese_pomeriggio_ = -1;
    double prossimo_pomeriggio_ = 0.0;
    void passo_pomeriggio();
    std::map<int, double> fine_virata_;
    std::vector<IstanzaSorgente> paesaggio_;
    std::map<int, Contatto> contatti_paesaggio_;
    Vec2 centro_paesaggio_;
    int ripopolazioni_ = 0;
    bool paesaggio_pronto_ = false, usa_paesaggio_ = true;
    double ultimo_ascolto_paesaggio_ = -1e9;
    void passo_paesaggio(double dt);   // s di missione della fine dell'ultima virata oltre il limite della schiera
    enum class TipoSensori { ascolto, passivi, attivi };   // passivi: anche quelli d'intercettazione
    std::map<std::pair<int, int>, CoppiaAcustica> coppie_;
    // fase 2 delle interazioni (docs/18): dal passivo l'IA ha i rilevamenti e ricava la posizione con la sua TMA
    // (nucleo/tma_ia), come il giocatore; solo per le coppie ostili con traccia solida
    struct TmaIa {
        Tma tma;
        double ultimo_ril = -1e18, ultima_soluzione = -1e18, errore_deg = 1.0;
        double ultima_osservabile = -1e18;   // l'ultima soluzione che ha dato la posizione (la sua freschezza al lancio)
        std::optional<StimaTma> stima;
        std::deque<double> errori;       // l'errore di ogni rilevamento nella Tma, per il sigma (media quadratica)
        double somma_quadrati = 0.0;
        // il sigma dei rilevamenti che stanno nella Tma, come lo vuole risolvi_tma (disassemblatore, 29/10/2026)
        double sigma_deg(double minimo) const {
            return errori.empty() ? std::max(errore_deg, minimo) : std::max(std::sqrt(std::max(0.0, somma_quadrati) / static_cast<double>(errori.size())), minimo);
        }
        void aggiungi_errore(double e) {
            errori.push_back(e);
            somma_quadrati += e * e;
            while (errori.size() > Tma::MASSIMO) { somma_quadrati -= errori.front() * errori.front(); errori.pop_front(); }
        }
        int non_osservabili = 0;         // soluzioni di fila non osservabili
        std::optional<Vec2> data;        // la posizione che la TMA ha dato al contatto (finche' nessun altro la cambia)
    };
    std::map<std::pair<int, int>, TmaIa> tma_ia_;
    std::map<int, TmaIa> tma_equipaggio_;   // la TMA dell'equipaggio del giocatore per contatto (i contatti noti dei missili)
    std::map<int, std::unique_ptr<EsmBattello>> esm_ia_;   // fase 2: l'ESM delle unita' dell'IA
    bool radar_giocatore_ = false;   // ordinato acceso
    // un radar acceso contro un sottomarino esposto (il giocatore o l'IA), un giro alla volta (dossier 11 §6.4)
    void radar_contro(const EmissioneVista& e, UnitaClassica& s, double dt);
    void radar_contro_nave(const EmissioneVista& e, UnitaClassica& n, double dt);   // le navi in superficie
    // il meteo nel punto di chi ascolta (docs/18 fase 2), per celle di 5 km rinnovate ogni 10 s
    mutable std::map<std::pair<int, int>, std::pair<double, StatoMeteo>> meteo_celle_;
    StatoMeteo meteo_ascolto(Vec2 p) const;
    Casuale caso_acustica_{0x50A2u};
    // campi dei raggi delle sorgenti (nucleo/acustica.hpp): calcolati in un thread, in vigore 10 s di gioco dopo la
    // richiesta, quindi ripetibili; prima del primo campo il livello A. Griglia fino a 300 km: oltre la griglia torna il
    // livello A, circa 60 dB in piu', e il bordo a 100 km era un gradino (tools/re/prove/portata_sonar/rapporto.md)
    CacheCampiAsincrona raggi_{ParametriRaggi{.r_max_m = 300000.0}, 10.0};
    std::vector<Messaggio> messaggi_sonar_;
    bool cavitava_ = false, schiera_cieca_ = false;
    std::vector<const Sensore*> sensori_acustici(const UnitaClassica& u, TipoSensori tipo) const;
    Ricevitore ricevitore_acustico(const UnitaClassica& u, const Sensore* s) const;
    std::optional<Sorgente> sorgente_acustica(const UnitaClassica& u) const;
    Ambiente ambiente_acustico(Vec2 a, Vec2 b) const;
    double rateo_virata_vero(const UnitaClassica& u) const;
    bool ascolta_con_acustica(UnitaClassica& o, UnitaClassica& b, bool automatica, bool esm);
    void avvisi_sonar();
    void messaggio_sonar(int codice);
    double gauss();
    Contatto* nuovo_contatto(UnitaClassica& o, int idb);
    void scelta_bersaglio(UnitaClassica& o, UnitaClassica& b, Contatto& c);
    bool dinamica_attiva_ = false;
    std::map<std::string, ParametriSottomarino> parametri_sub_;   // per classe
    std::map<int, DinamicaSottomarino> dinamiche_;
    std::map<int, ComandiSottomarino> comandi_sub_;
    DinamicaSottomarino* muovi_con_dinamica(UnitaClassica& u, const Pilota& p, double dt);
    std::vector<PuntoScia> scia_;    // DS:47BE, indice DS:47BC
    std::size_t indice_scia_ = 0;
    int conto_scia_ = 30;            // DS:47BA (1000:962D)
    void registra_scia(int id, Vec2 pos);
    int esm_ = 0;                    // DS:476C (2011:0D60), a 0 all'avvio, quando un'unita' esce (003C) e a sensori giu'
    int osservatore_ = -1;           // DS:1F5C: da dove parte la ricerca del prossimo osservatore (-1 o tolta: la testa)
    bool impatti_originale_ = true;
    // siluri dell'Enhanced (fase C6b)
    struct SiluroMissione {
        Siluro siluro;
        double rotta_lanciatore = 0.0;   // per il rateo di virata del lanciatore (vincoli del filo)
    };
    bool siluri_attivi_ = false;
    TabellaSiluri tabella_siluri_;
    std::uint64_t seme_ = 0;
    std::map<int, SiluroMissione> siluri_;
    std::optional<ImpostazioniSiluro> impostazioni_giocatore_;
    std::uint32_t campi_giocatore_ = ~0u;
    bool campo_giocatore(CampoSiluro k) const { return (campi_giocatore_ >> static_cast<unsigned>(k)) & 1u; }
    std::optional<std::string> famiglia_lancio_;   // la famiglia del tubo del giocatore mentre ne gira il gestore
    // missili dell'Enhanced (missione_missili.cpp)
    std::unique_ptr<CatalogoMissili> catalogo_missili_;
    struct TiroTubo {                          // le impostazioni e la preparazione di un tubo, dagli ordini
        FonteDati fonte = FonteDati::tma;
        ImpostazioniMissile imp;
        std::string missile;                   // la classe preparata (si spegne se cambia)
        std::optional<PreparazioneMissile> prep;
        std::optional<double> distanza_m, rotta, velocita_kn, sigma_distanza_m, sigma_trasversale_m;
        int missione_tlam = -1;                // TLAM: indice della libreria di bordo
    };
    std::vector<MissioneTlam> libreria_tlam_;
    std::map<std::string, std::string> esecuzioni_tlam_;   // id del messaggio radio -> missione TLAM che rende eseguibile
    std::set<std::string> tlam_eseguibili_;   // gli ordini d'esecuzione ricevuti
    std::vector<LancioTlam> lanci_tlam_;
    bool lancia_tlam(UnitaClassica& g, int tubo);
    std::map<int, TiroTubo> tiri_;
    struct MissileMissione {
        Missile missile;
        int tubo = 0;
        double t_lancio = 0.0;
        bool giocatore = false;
        PianoTiro piano;                       // per la stima del volo (la carta di tiro)
        ImpostazioniMissile imp;
        bool aggancio_detto = false;           // il messaggio 479, una volta
        std::map<int, int> forze_al_lancio;    // il livello d'allerta di ogni parte al lancio: la provocazione del lancio non sveglia il bersaglio
    };
    std::map<int, MissileMissione> missili_;   // per id dell'unita' del missile
    std::map<int, ImpegnoDifese> impegno_difese_;   // canali e affusti impegnati, per nave (nucleo/difese_navi.hpp)
    std::map<int, int> livelli_forze() const;       // la fotografia per forze_al_lancio; vuota a forze spente
    // l'allerta con cui la nave `v` riceve un missile: la forza al lancio o la sua formazione adesso, la maggiore; nullopt a forze spente
    std::optional<int> allerta_contro_missile(const UnitaClassica& v, const std::map<int, int>& al_lancio) const;
    bool aggancio_visto(int nave) const;       // il giocatore sa che il suo missile ha agganciato `nave`
    std::set<int> tubi_usati_;                 // i tubi che hanno gia' lanciato: il missile caricato dopo si prepara
    std::array<double, 2> slide_dal_{};        // per lato (dritta, sinistra): da quando la slide valve e' libera
    bool lancio_modellato_ = false, timer_da_missile_ = false;   // il gestore di lancio, i timer nati da lui
    // traffico civile (missione_traffico.cpp)
    bool traffico_attivo_ = false, traffico_vietato_ = false, traffico_tentato_ = false;
    std::string cartella_dati_traffico_;
    std::unique_ptr<RotteTraffico> rotte_traffico_;
    std::optional<Traffico> traffico_;
    std::map<int, int> id_traffico_, traffico_id_;   // id dell'unita' -> indice della nave, e il contrario
    int prossimo_id_traffico_ = 10000;
    std::size_t colpi_visti_traffico_ = 0;
    std::map<int, unsigned> turni_traffico_;   // per osservatore: i turni dei sensori, il traffico uno su 4
    double traffico_dt_ = 0.0;                 // il tempo accumulato: il modulo gira una volta al secondo
    void passo_traffico(double dt);
    bool classe_civile(const std::string& classe) const;   // dai dati (ClasseNave::civile), se no per nome
    // il fondo (missione_fondo.cpp)
    struct FondoUnita {
        StatoFondoUnita s;
        double minuto_s = 0.0;
        bool avvisato_rocce = false;
    };
    std::map<int, FondoUnita> fondi_;
    std::optional<Casuale> caso_fondo_;
    void passo_fondo(double dt);
    bool trattieni_sul_fondo(UnitaClassica& u);   // vero: il fango lo tiene giu' ancora (navigazione)
    // la schiera trainata dei sottomarini (missione_schiera.cpp)
    struct SchieraUnita {
        const DatiSchiera* dati = nullptr;
        CavoSchiera stato;
        FormaSchiera forma;
        ContattoFondo fondo;
        TipoFondo tipo_fondo = TipoFondo::sabbia;
        double cavo_m = 0.0, minuto_s = 0.0;
        bool danneggiata = false, persa = false, avvisato_fondo = false;
    };
    std::map<int, SchieraUnita> schiere_;
    // la sorgente del battello per le sue righe proprie (Ricevitore::proprio), ricalcolata una volta per passo
    mutable std::map<int, std::pair<double, std::optional<Sorgente>>> sorgenti_proprie_;
    std::optional<Casuale> caso_schiera_;
    std::size_t colpi_visti_schiera_ = 0;
    void passo_schiere(double dt);
    const DatiSchiera* dati_schiera(const UnitaClassica& u) const;
    void danneggia_schiera(UnitaClassica& u, SchieraUnita& s, bool persa, int messaggio);
    // il traino delle navi dell'IA (missione_vds.cpp): il pesce, le regole per filare, recuperare, sprint and drift
    struct TrainoNave {
        SchieraUnita vds;                         // il pesce (dati nulli senza VDS)
        double quota_vds_m = 0.0, quota_s = 0.0;  // la quota voluta e quando rivederla
        double limite_kn = 1e9;                   // il pilota non va oltre (velocita_traino)
        double v_deriva_kn = 6.0, deriva_s = 300.0, cavo_schiera_prima = 0.0;
        double voluta_bassa_s = 0.0;              // da quanto la velocita' voluta regge il traino (isteresi)
        bool vds_fila = false, recupero = false, libera = false, ciclo = false;
        bool schiera_ia = false, schiera_in_filata = false, annunciare = false;
    };
    std::map<int, TrainoNave> traini_;
    std::vector<EventoTraino> eventi_traino_;
    std::optional<Casuale> caso_traino_;
    std::size_t colpi_visti_traino_ = 0;
    void passo_traini(double dt);
    const DatiSchiera* dati_vds(const UnitaClassica& u) const;
    bool vds_in_acqua(int id) const;
    // la SQR-18A della Knox e' trainata dal pesce: (quota, dietro la poppa) del pesce in acqua
    std::optional<std::pair<double, double>> traino_dal_pesce(const UnitaClassica& u) const;
    double velocita_traino(const UnitaClassica& u, double v) const;
    void nota_traino(const UnitaClassica& u, const std::string& sistema, bool filato, double quota_m);
    // l'intercettazione (missione_intercettazione.cpp)
    std::unique_ptr<CatalogoIntercettazione> catalogo_int_;
    struct EmissioneMissione {
        EmissioneAttiva e;
        double settore_deg = 180.0, lobi_db = 0.0;   // un fascio (BQS-15): fuori dal settore, i lobi laterali
    };
    std::vector<EmissioneMissione> emissioni_attive_;   // gli impulsi dall'ultimo passo dell'intercettazione
    std::map<int, std::unique_ptr<RicevitoreIntercettazione>> intercettatori_;
    std::map<int, Allarme> allarmi_intercettazione_;
    void emetti_impulso(int id, const std::string& chiave, double sl_db, double f_hz = 0.0, double settore_deg = 180.0,
                        double lobi_db = 0.0);
    // da un punto che non e' un'unita' (la boa attiva, il trasduttore del sonar a immersione): `id` e' chi la governa
    void emetti_impulso_da(int id, Vec2 pos, double quota_m, const std::string& chiave, double sl_db);
    void passo_intercettazione(double dt);
    std::vector<std::pair<int, std::string>> radar_del_passo_;   // i radar accesi dell'ultimo passo dell'elettronica (EcoNave)
    const DatiMissile* missile_del_tubo(int tubo) const;
    std::optional<BersaglioStimato> bersaglio_missile(const UnitaClassica& g, const TiroTubo& t) const;
    StatoLanciatore lanciatore_missile(const UnitaClassica& g) const;
    std::vector<ContattoNoto> contatti_noti_missile(const UnitaClassica& g) const;
    bool lancia_missile_modello(UnitaClassica& g, int tubo);
    bool lancia_missile_ia(UnitaClassica& l);
    // il tiro antinave delle navi dell'IA senza script (I11, missione_missili.cpp): la riserva, la salva, il prossimo lancio
    struct NaveMissiliIa {
        int rimasti = -1, salva = 0, bersaglio = -1, candidato = -1;
        double prossimo = 0.0, pronto_da = 0.0;
        // i sottomarini: la manovra alla quota e alla velocita' di lancio, poi di nuovo quelle di prima
        bool in_manovra = false;
        double quota_prima = -1.0, velocita_prima = -1.0, manovra_da = 0.0;
        int tubo = -1;   // gli occidentali: il tubo del carico col missile (armi_ia_)
    };
    std::map<int, NaveMissiliIa> missili_ia_;
    double prossimi_missili_ia_ = 0.0;
    void passo_missili_ia();
    void crea_missile(UnitaClassica& l, const DatiMissile& d, const ImpostazioniMissile& imp, const PianoTiro& p, int bersaglio, int tubo);
    bool imposta_missile(const Ordine& o);
    void passo_missili(double dt);
    int dimensione_creduta(const std::string& classe) const;
    std::vector<EventoSiluroMissione> eventi_siluri_;
    std::vector<LancioMissile> lanci_missili_;
    void nuovo_siluro(UnitaClassica& u, const DatiSiluro* modello = nullptr);   // modello: siluri delle mine
    void passo_siluri(double dt);
    CampoMinato campo_minato_;
    std::set<int> da_mina_;
    std::map<int, double> carica_mina_;   // siluri delle mine: la carica della testa
    void passo_mine(double dt);
    struct MinaIniziale { std::string tipo; Vec2 pos; int lato = 0; bool armata = true, sicura = false, comandata = false; std::string gruppo; };
    std::vector<MinaIniziale> mine_iniziali_;
    struct PosaIa { int unita = -1; std::string tipo; int restano = 0; double prossima = 0.0, ogni_s = 60.0; };
    std::vector<PosaIa> pose_ia_;
    void innesco_mina(const InnescoMina& x);
    bool posa_mina(const UnitaClassica& g, int tubo);
    std::uint64_t contatti_creati_ = 0;   // ordine dei contatti nelle liste degli osservatori
    int create_ = 0;                 // DS:1F56: unita' create nella missione (non cala mai), bersagli per osservazione
    std::vector<Colpo> colpi_;
    std::map<int, int> salve_asw_;   // salve ATTACK fatte per unita' (Enhanced)
    bool fuoco_amico_[2] = {false, false};
    std::set<int> fisica_;
    // comandi del giocatore
    std::map<std::string, std::string> gestori_giocatore_, gestori_armi_;   // nome -> etichetta dello script
    std::vector<std::string> tubi_;
    std::map<std::string, int> dotazione_;
    LimitiMissione limiti_carico_;
    std::optional<BloccoCarriera> blocco_carriera_;
    std::map<int, std::string> varianti_tubo_;   // tubo -> modello del siluro (carico M2)
    std::optional<std::string> variante_lancio_; // del lancio del giocatore in corso, per nuovo_siluro
    std::vector<Vec2> punti_di_rotta_;
    bool pilota_automatico_ = false, in_rotta_ = false;
    void prossimo_punto();
};

}  // namespace m688
