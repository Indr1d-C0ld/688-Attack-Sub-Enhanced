#pragma once
// Sala radio dell'Enhanced dal lato della simulazione (docs/11_comandi_e_stazioni.md §11, pezzo S4; punto 22 della
// roadmap). Numeri dal dossier docs/ricerca/08_comunicazioni_radio.md §9 (tutti STIMA salvo dove il dossier cita).
//
// Il modulo e' puro: riceve il tempo e lo stato delle antenne del battello, tiene il broadcast da terra, i messaggi
// ricevuti e persi, il filo galleggiante e le trasmissioni. La missione lo alimenta e ne esegue gli ordini.
// - Broadcast VLF (con l'HF in parallelo) come una coda continua: ogni messaggio va in onda in uno spazio
//   (acquisizione piu' i caratteri alla velocita' della banda), prima i FLASH, poi IMMEDIATE, PRIORITY, ROUTINE; si
//   ripete ogni ciclo (120 minuti) 3, 3, 2 o 1 volte. Il traffico di fondo (meteo, intelligence generica) riempie la
//   coda e da' i numeri di serie.
// - Si riceve una messa in onda solo con la banda per tutto lo spazio; con la ricezione al 50% (filo galleggiante fra
//   8 e 12 nodi) una volta su due. Dopo l'ultima messa in onda non ricevuta il messaggio e' perso: il giocatore vede il
//   buco nella serie quando arriva un numero successivo.
// - Antenne: albero o periscopio alzati a quota periscopica (tutte le bande; oltre 10 nodi al 50%); filo galleggiante
//   OE-315 (solo VLF fino a 120 m: pieno fino a 8 nodi, al 50% fino a 12; cieco due minuti dopo una virata; oltre 20
//   nodi per piu' di un minuto 30% di rottura al minuto); ELF fino a 300 m e 15 nodi col filo, per chi ce l'ha
//   (i sovietici con lo ZEVS): solo il campanello dei FLASH.
// - Satellite (SSIXS, USA): con l'albero e la copertura (fino a 72 gradi di latitudine) una richiesta breve scarica
//   in 3 minuti tutto cio' che e' gia' andato in onda.
// - Trasmettere: solo con l'albero; UHF col satellite (1-10 s), altrimenti HF (1-3 minuti); i sovietici in burst
//   da 0,7 s. Radiogoniometria: P_fix = P_rete (1 - e^(-t/tau)) (dossier §9.5).
// - Finestre di comunicazione: USA ogni 12 ore (+-2), URSS alle 00:00 e alle 12:00 di Mosca (+-30 minuti); con
//   l'orologio di missione (fase R1, dossier docs/ricerca/10_radio_ruolo_in_partita.md §0) ogni `finestra_ogni_s`.
// Fase R1 (dossier 10 §4.1-4.2): contatti d'intelligence con un'ellisse che cresce col tempo, regole d'ingaggio
// trasmesse dal comando e ricevute dal giocatore, giudizio dei lanci e debriefing (messaggi trasmessi, ricevuti, persi).

#include <array>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include "nucleo/casuale.hpp"
#include "nucleo/geometria.hpp"

namespace m688 {

enum class Banda : std::uint8_t { elf, vlf, hf, uhf };
enum class Precedenza : std::uint8_t { flash, immediate, priority, routine };
enum class LatoRadio : std::uint8_t { usa, urss };
const char* nome_banda(Banda b);
const char* nome_precedenza(Precedenza p);   // FLASH, IMMEDIATE, PRIORITY, ROUTINE
char sigla_precedenza(Precedenza p);         // Z, O, P, R

struct TestoRadio {
    std::string it, en;
};

// --- regole d'ingaggio (dossier 10 §4.2): stato delle armi per categoria di bersaglio ---
enum class CategoriaRoe : std::uint8_t { sottomarini, navi, mercantili, neutrali, aerei, numero };
enum class StatoRoe : std::uint8_t { libero, stretto, trattenuto };   // stretto: solo bersagli identificati ostili
const char* nome_categoria(CategoriaRoe c);   // chiavi del file radio: sottomarini, navi, mercantili, neutrali, aerei
const char* nome_stato_roe(StatoRoe s);     // libero, stretto, trattenuto
std::optional<CategoriaRoe> categoria_da_nome(const std::string& s);
std::optional<StatoRoe> stato_roe_da_nome(const std::string& s);

inline constexpr std::size_t CATEGORIE_ROE = static_cast<std::size_t>(CategoriaRoe::numero);
using CambioRoe = std::array<std::optional<StatoRoe>, CATEGORIE_ROE>;

struct Roe {
    std::array<StatoRoe, CATEGORIE_ROE> stato{};   // tutto libero
    std::array<int, CATEGORIE_ROE> serie{};          // il messaggio che l'ha stabilito (0: ordini di missione)
    std::array<bool, CATEGORIE_ROE> dubbia{};        // stabilita da un messaggio verificato non autentico
    StatoRoe operator[](CategoriaRoe c) const { return stato[static_cast<std::size_t>(c)]; }
    bool dubbio(CategoriaRoe c) const { return dubbia[static_cast<std::size_t>(c)]; }
    void applica(const CambioRoe& c, int da_serie, bool da_dubbio = false);
};

// --- contatti d'intelligence (dossier 10 §4.1) ---
struct RichiestaIntelligence {   // dal file della missione: la missione la compila con la verita' quando va in onda
    std::vector<int> unita;
    double eta_s = 1200.0;          // eta' dell'osservazione quando il messaggio parte
    double errore_m = 5000.0;       // 1 sigma della posizione pubblicata
    std::string sensore = "sosus";
    double rotta_err_deg = 20.0, velocita_err_kn = 3.0;
    bool falso = false;             // falso allarme: nessuna unita' vera
};

struct DatiIntelligence {
    std::vector<int> unita;
    Vec2 pos;                       // posizione pubblicata all'ora dell'osservazione (con l'errore)
    double t_oss = 0.0;             // s di missione dell'osservazione (anche negativo: prima dell'inizio)
    double rotta = 0.0, velocita_kn = 0.0;   // stimate
    double errore_m = 5000.0, rotta_err_deg = 20.0, velocita_err_kn = 3.0;
    double m_s_per_nodo = NODO;     // la scala del moto della missione (il moto classico va 3,46 volte il vero)
    std::string sensore;
    bool falso = false;
};

struct ContattoIntelligence {
    int serie = 0;
    DatiIntelligence dati;
    double ricevuto = 0.0;          // s di missione
    double eta(double t) const { return t - dati.t_oss; }
    Vec2 centro(double t) const;    // avanza con rotta e velocita' stimate
    double semiasse_rotta(double t) const;      // lungo la rotta: errore + sigma_v eta
    double semiasse_traverso(double t) const;   // di traverso: errore + v sin(sigma_rotta) eta
    double orientamento() const { return dati.rotta; }   // gradi veri dell'asse lungo
};

// --- zone dello spazio d'acqua (fase R2, dossier 10 §4.3) ---
enum class TipoZona : std::uint8_t { azione, libera_asw, protetta, protetta_mobile };
const char* nome_tipo_zona(TipoZona t);   // azione, libera_asw, protetta, protetta_mobile (chiavi del file radio)
std::optional<TipoZona> tipo_zona_da_nome(const std::string& s);

struct Zona {
    std::string id;
    TipoZona tipo = TipoZona::protetta;
    std::optional<Vec2> centro;         // cerchio: centro e raggio (m)
    double raggio_m = 0.0;
    std::vector<Vec2> vertici;          // oppure un poligono (m)
    int unita = -1;                     // protetta mobile: il battello amico; rettangolo orientato sulla sua rotta
    double avanti_m = 0.0, dietro_m = 0.0, lato_m = 0.0;
    double dal_s = 0.0, al_s = 1e18;    // validita' (s di missione)
    int serie = 0;                      // il messaggio che l'ha portata (0: ordini di missione)
    bool dubbia = false;                // portata da un messaggio verificato non autentico (nelle ricevute)
    bool valida(double t) const { return t >= dal_s && t <= al_s; }
};

struct CambioZone {
    std::vector<Zona> aggiungi;         // con un id gia' presente: la sposta
    std::vector<std::string> togli;
};

struct PosaUnita {                      // dove e verso dove va il battello di una zona mobile
    Vec2 pos;
    double rotta = 0.0;
};
// il poligono della zona (il rettangolo della mobile con la posa del suo battello); vuoto per un cerchio
std::vector<Vec2> poligono_zona(const Zona& z, std::optional<PosaUnita> posa = std::nullopt);
bool dentro_zona(const Zona& z, Vec2 p, std::optional<PosaUnita> posa = std::nullopt);

struct MessaggioRadio {
    int serie = 0;                      // numero del broadcast (lo assegna la sala; 0: ordini di missione)
    Precedenza precedenza = Precedenza::routine;
    std::string dtg;                    // gruppo data-ora («241530Z OCT 89»), dall'ora della prima messa in onda
    std::string da, a, oggetto;
    std::string classifica = "CONFIDENTIAL";
    TestoRadio testo;
    int caratteri = 0;                  // per il tempo in onda; 0: dalla lunghezza del testo inglese
    std::string autenticazione;         // gruppo da verificare con la tabella del giorno
    bool autentico = true;              // falso: inganno (il gruppo non torna)
    std::string ordine;                 // gestore della missione da eseguire alla ricezione (ordini che cambiano)
    std::string id;                     // nome per i riferimenti (file radio della missione)
    std::optional<CambioRoe> roe;       // nuove regole d'ingaggio
    std::optional<RichiestaIntelligence> richiesta;   // contatto d'intelligence da compilare
    std::optional<DatiIntelligence> intelligence;     // compilato dalla missione prima della messa in onda
    std::optional<CambioZone> zone;     // fase R2: zone aggiunte, spostate o tolte
    double scade_s = -1.0;              // fase R2: compito valido fino a dal_s + scade_s (s di missione); < 0: senza
    std::string gestore_scaduto;        // eseguito se il compito scade senza essere stato ricevuto in tempo
    // R3 (contenuti radio, dossier 10 §5.2)
    int ripetizioni = 0;                // messe in onda; 0: dalla precedenza (3, 3, 2, 1)
    bool ricevuta = false;              // il comando vuole la ricevuta: il giocatore deve trasmettere dopo averlo ricevuto
    std::string annulla;                // «disregard»: id del messaggio che non va piu' in onda quando parte questo
};

// Un lancio del giocatore giudicato con le regole d'ingaggio
struct GiudizioRoe {
    enum class Esito : std::uint8_t { permesso, nota, infrazione_non_copiato, infrazione };
    Esito esito = Esito::permesso;
    double tempo = 0.0;
    int bersaglio = -1;
    CategoriaRoe categoria = CategoriaRoe::navi;
    StatoRoe ricevute = StatoRoe::libero, trasmesse = StatoRoe::libero;
    int serie = 0;                      // il messaggio delle regole trasmesse che il lancio viola
    // come il giocatore conosceva il bersaglio al lancio (classificazione del contatto): il debriefing distingue
    // «hai sparato a un mercantile scambiato per una fregata» dalla violazione consapevole
    std::optional<CategoriaRoe> categoria_creduta;   // nullopt: non classificato
    std::string classe_creduta;         // vuota: classe non nota
    std::string zona;                   // R2: la zona protetta in cui stava il bersaglio (vuota: nessuna)
    bool scambiato() const { return categoria_creduta && *categoria_creduta != categoria; }
    bool ordine_falso_creduto = false;  // R2: il lancio era permesso solo da un ordine non autenticato
};

struct Ricevuto {
    MessaggioRadio messaggio;
    double tempo = 0.0;                 // s di missione
    Banda banda = Banda::vlf;
    bool verificato = false;            // il giocatore ha controllato il gruppo d'autenticazione
    bool tardi = false;                 // ricevuto dopo la scadenza del compito: non vale piu'
    bool ricevuta_inviata = false;      // R3: chiesta la ricevuta, il giocatore ha trasmesso dopo averlo ricevuto
};

// Una sessione di comunicazione (fase R2, dossier 10 §4.6: il guinzaglio sovietico)
struct Sessione {
    enum class Stato : std::uint8_t { futura, in_corso, fatta, mancata };
    double inizio = 0.0, fine = 0.0;    // la finestra con la tolleranza (s di missione)
    bool copiata = false;               // il battello ha avuto la ricezione nella finestra
    bool ricevuta_inviata = false;      // ha trasmesso (ricevuta o rapporto) nella finestra
    Stato stato = Stato::futura;
};

// Una messa in onda: senza il contenuto, che si conosce solo ricevendola
struct MessaInOnda {
    int serie = 0;
    Precedenza precedenza = Precedenza::routine;
    double inizio = 0.0, fine = 0.0;    // s di missione
    int ripetizione = 1;                // 1 = prima messa in onda
};

struct StatoAntenne {
    double quota_m = 0.0, velocita_kn = 0.0, rateo_virata_deg_s = 0.0;
    bool albero = false;                // antenna o periscopio alzati (a quota periscopica)
    double latitudine = 60.0;           // gradi: copertura del satellite
    bool ghiaccio = false;              // sotto la banchisa: il filo non arriva alla superficie, passa solo l'ELF
};

enum class Ricezione : std::uint8_t { no, meta, si };

// previsione: la richiesta di un bollettino meteo al comando (dossier 21 §5.2-5.3: costa l'esposizione della trasmissione)
enum class TipoTrasmissione : std::uint8_t { contatto, attacco, situazione, conferma, richiesta, previsione };
const char* nome_trasmissione(TipoTrasmissione t);

struct ContestoDF {
    bool rete_nemica = true;            // il teatro e' coperto dalla rete strategica avversaria (Krug, Bullseye)
    bool esm_vicino = false;            // un'unita' nemica con ESM entro l'orizzonte radio
};

// La radiogoniometria nemica su una trasmissione (dossier docs/ricerca/10 §6.4), una regola sola per il giocatore
// (SalaRadio::trasmetti) e per l'IA (Missione::radiogoniometria_ia): in HF la rete strategica (Krug, Bullseye) con P_rete
// 0,9 (0,2 senza la rete), tau 1,5 s, un fix di 65 km dopo 1-3 ore; fuori dall'HF solo un ESM vicino, col rilevamento
// (P_rete 0,6, tau 3 s), o senza ESM P_rete 0,05 e un fix di 185 km dopo 3-6 ore. P del fix = P_rete (1 - e^(-durata/tau))
struct RegolaDF {
    double p_rete = 0.0, tau_s = 1.0;
    double errore_m = 0.0;                    // 1 sigma; 0: solo il rilevamento
    double ritardo_min_s = 0.0, ritardo_extra_s = 0.0;   // ritardo = min + extra x uniforme (nessun dado se extra = 0)
    double probabilita(double durata_s) const;
};
RegolaDF regola_df(Banda banda, const ContestoDF& df);

struct EsitoTrasmissione {
    bool fatta = false;                 // senza albero: niente
    Banda banda = Banda::hf;
    double durata_s = 0.0;              // emissione
    double probabilita_fix = 0.0;
    bool localizzato = false;
    double errore_m = 0.0;              // 1 sigma del fix; 0 con localizzato: solo il rilevamento (ESM vicino)
    double ritardo_s = 0.0;             // prima che la minaccia (pattugliatori, unita' dirottate) arrivi
};

struct EventoRadio {
    enum class Tipo : std::uint8_t { in_arrivo, ricevuto, perso, campanello, filo_rotto, filo_senza_segnale,
                                     satellite_assente, trasmissione, ssixs, ordini_cambiati, intelligence,
                                     scaduto, sessione_mancata, guinzaglio };   // R2
    Tipo tipo = Tipo::ricevuto;
    double tempo = 0.0;
    int serie = 0;
    Precedenza precedenza = Precedenza::routine;
};

struct ParametriRadio {
    double ciclo_s = 7200.0;                        // ciclo del broadcast (ISABPS, dossier §2.2)
    double acquisizione_vlf_s = 60.0, acquisizione_hf_s = 120.0, acquisizione_uhf_s = 180.0;
    double caratteri_s_vlf = 6.0, caratteri_s_hf = 10.0, caratteri_s_uhf = 600.0;
    double quota_albero_m = 18.0;                   // oltre: l'albero non esce
    double velocita_albero_kn = 10.0;               // oltre: ricezione al 50%
    double quota_filo_m = 120.0, filo_pieno_kn = 8.0, filo_meta_kn = 12.0;
    double filo_cieco_dopo_virata_s = 120.0, filo_virata_deg_s = 0.5;
    double filo_rottura_kn = 20.0, filo_rottura_al_minuto = 0.3, filo_filata_s = 120.0;
    double quota_elf_m = 300.0, velocita_elf_kn = 15.0;
    double latitudine_satellite = 72.0;
    double fondo_ogni_s = 1800.0;                   // traffico di fondo in media ogni mezz'ora
    // finestre di comunicazione: 0 = orari veri (USA 00Z e 12Z, URSS 00:00 e 12:00 di Mosca, ogni 12 ore); con
    // l'orologio di missione ogni `finestra_ogni_s` dal tempo di missione `finestra_primo_s`
    double finestra_ogni_s = 0.0, finestra_primo_s = 0.0;
    double tolleranza_finestra_s = 0.0;             // 0: quella del lato (USA 2 ore, URSS 30 minuti)
    double vita_intelligence_cicli = 3.0;           // poi il contatto d'intelligence si toglie dalla carta
    // sessioni obbligatorie (R2, dossier 10 §4.6): con `obbligatorie` una finestra senza ricezione (e senza la
    // trasmissione, con `ricevuta`) e' mancata; dopo `saltate_max` di fila il guinzaglio
    bool sessioni_obbligatorie = false, ricevuta_richiesta = false;
    int saltate_max = 2;
};

// Orologio radio compresso per le missioni (dossier 10 §0): ciclo di 15 minuti (scelta dell'utente), traffico di
// fondo ogni terzo di ciclo, finestre ogni 6 cicli (il rapporto vero 12 ore / 2 ore), tolleranza di un ciclo
ParametriRadio parametri_di_missione(double ciclo_s = 900.0);

// Messaggi della sala radio per il debriefing (dossier 10 §4.2: il rapporto finale elenca le serie perse)
struct Debriefing {
    struct Voce {
        MessaggioRadio messaggio;
        double trasmesso = -1.0;        // prima messa in onda (-1: mai in onda)
        double ricevuto = -1.0;         // -1: non ricevuto
    };
    std::vector<Voce> trasmessi;        // tutti quelli andati in onda almeno una volta, per serie
    std::vector<Voce> persi;            // andati in onda e non ricevuti
    std::vector<GiudizioRoe> lanci;     // i lanci giudicati con le regole d'ingaggio
    std::vector<std::pair<double, std::string>> ingressi;   // R2: ingressi nelle zone protette (tempo, zona)
    std::vector<int> ricevute_mancate;  // R3: serie ricevute che chiedevano la ricevuta e non l'hanno avuta
};

class SalaRadio {
public:
    // `ora_inizio_s`: ora UTC della missione in secondi dalla mezzanotte; giorno, mese e anno per il DTG
    SalaRadio(LatoRadio lato, std::uint64_t seme, double ora_inizio_s = 0.0, int giorno = 24, int mese = 10,
              int anno = 1989, ParametriRadio p = {});

    // ordini di missione (serie 0): gia' ricevuti all'inizio, rileggibili
    void ordini_di_missione(MessaggioRadio m);
    // un messaggio in programma dal tempo dato (la sala assegna la serie e lo mette in coda)
    int programma(MessaggioRadio m, double dal_s);
    // R3: un messaggio con questo id, non ancora andato in onda, non ci va piu' (vero se c'era)
    bool annulla(const std::string& id);
    // R3: in programma o in onda e non ancora ricevuto col dato id (per non ripetere un messaggio da script)
    bool in_coda(const std::string& id) const;
    void avanza(double t, double dt, const StatoAntenne& s);

    void cala_filo(double t);
    void recupera_filo();
    EsitoTrasmissione trasmetti(TipoTrasmissione tipo, double t, const StatoAntenne& s, const ContestoDF& df);

    const std::vector<Ricevuto>& ricevuti() const { return ricevuti_; }
    std::vector<int> persi() const;                  // buchi nella serie sotto l'ultimo numero ricevuto
    std::optional<MessaInOnda> in_onda() const;      // lo spazio in corso
    double progresso() const;                        // 0..1 della messa in onda in corso, se la si sta ricevendo
    // telescrivente: la parte del testo gia' arrivata della messa in onda in corso (vuota se non la si riceve o se
    // il messaggio e' gia' stato ricevuto prima)
    TestoRadio testo_in_arrivo() const;
    std::vector<MessaInOnda> in_programma() const;   // le prossime messe in onda gia' decise, in ordine
    std::vector<const Ricevuto*> ordini() const;     // gli ordini di missione e i messaggi che li cambiano
    Ricezione ricezione(Banda b) const;              // con l'ultimo stato delle antenne
    bool filo() const { return filo_ == Filo::filato; }
    bool filo_in_filata() const { return filo_ == Filo::in_filata; }
    bool filo_rotto() const { return filo_ == Filo::rotto; }
    bool elf() const { return elf_; }
    LatoRadio lato() const { return lato_; }
    double prossima_finestra(double t) const;        // s di missione della prossima (o di quella in corso, +-tolleranza)
    double tolleranza_finestra_s() const;
    std::optional<MessaInOnda> prossimo_broadcast() const;   // la prossima messa in onda decisa (per la compressione)
    double prossimo_inizio_ciclo(double t) const;            // s di missione del prossimo broadcast deciso dopo t, -1: nessuno
    const ParametriRadio& parametri() const { return p_; }
    std::string dtg(double t) const;

    // regole d'ingaggio: quelle di partenza, quelle trasmesse dal comando fino a t e quelle che il giocatore ha
    // ricevuto (ognuna con la serie che l'ha stabilita); il giudizio di un lancio contro un bersaglio della categoria
    // (`identificato`: classificato ostile, per lo stato «stretto») e il debriefing
    void roe_iniziali(const CambioRoe& c);
    Roe roe_trasmesse(double t) const;
    // quelle che il giocatore ha ricevuto, comprese quelle dei messaggi falsi: dopo la sua verifica (verifica()) quelle
    // con il gruppo che non torna sono marcate `dubbia` e il giocatore decide se crederci; con `solo_autentiche` solo
    // le autentiche: il giudizio usa queste (sparare fidandosi di un ordine falso e' un'infrazione, e il debriefing lo dice)
    Roe roe_ricevute(bool solo_autentiche = false) const;
    // zone dello spazio d'acqua (R2): di partenza, trasmesse fino a t, ricevute (stesse regole delle ROE)
    void zone_iniziali(std::vector<Zona> z);
    std::vector<Zona> zone_trasmesse(double t) const;
    std::vector<Zona> zone_ricevute(bool solo_autentiche = false) const;
    // inganno (R2, dossier 10 §4.8): il giocatore verifica il gruppo d'autenticazione; vero se torna
    bool verifica(int serie);
    // R2: l'ultimo lancio giudicato peggiora per una zona protetta (il peggiore fra ROE e zona); ingressi nelle zone
    void aggrava_ultimo_lancio(GiudizioRoe::Esito e, const std::string& zona);
    void registra_ingresso(double t, const std::string& zona) { ingressi_.push_back({t, zona}); }
    const MessaggioRadio* messaggio(int serie) const;   // un messaggio in programma o gia' andato, per serie
    // sessioni di comunicazione (R2): le passate, quella in corso e la prossima
    std::vector<Sessione> sessioni(double t) const;
    GiudizioRoe giudica_lancio(int bersaglio, CategoriaRoe c, bool identificato, double t,
                               std::optional<CategoriaRoe> creduta = std::nullopt, const std::string& classe_creduta = {});
    Debriefing debriefing() const;

    // contatti d'intelligence: le richieste da compilare (la missione da' la verita' prima della messa in onda) e i
    // contatti ricevuti ancora sulla carta
    std::vector<std::pair<int, RichiestaIntelligence>> da_compilare(double t) const;   // serie e richiesta
    void compila(int serie, const DatiIntelligence& d);
    std::vector<ContattoIntelligence> intelligence(double t) const;
    // eventi e messaggi ricevuti dall'ultima chiamata (la missione esegue gli ordini e avvisa il giocatore)
    std::vector<EventoRadio> eventi();
    std::vector<MessaggioRadio> nuovi();

private:
    enum class Filo : std::uint8_t { recuperato, in_filata, filato, rotto };
    struct InCoda {
        MessaggioRadio m;
        double dovuto = 0.0;      // prima messa in onda possibile
        int rimaste = 1;          // messe in onda ancora da fare
        int fatte = 0;
        bool ricevuto = false;
        bool perso_avvisato = false, campanello = false;
        double trasmesso = -1.0, ricevuto_t = -1.0;   // prima messa in onda e ricezione
        bool scadenza_avvisata = false;
    };
    Roe roe_con(bool solo_ricevuti, double t, bool solo_autentiche = true) const;
    bool verificato_falso(int serie) const;
    std::vector<Zona> zone_con(bool solo_ricevuti, double t, bool solo_autentiche) const;
    void passo_sessioni(double t);
    double durata_in_onda(const MessaggioRadio& m) const;
    void prossimo_spazio(double t);
    void fondo(double t);
    void ricevi(InCoda& q, double t, Banda b);
    void evento(EventoRadio::Tipo tipo, double t, int serie = 0, Precedenza p = Precedenza::routine);

    LatoRadio lato_;
    Casuale caso_;
    double ora_inizio_s_;
    int giorno_, mese_, anno_;
    ParametriRadio p_;
    bool elf_;
    std::vector<InCoda> coda_;
    std::vector<Ricevuto> ricevuti_;
    std::vector<EventoRadio> eventi_;
    std::vector<MessaggioRadio> nuovi_;
    int serie_ = 0;
    // spazio in corso
    int corrente_ = -1;               // indice in coda_, -1: nessuno
    double inizio_ = 0.0, fine_ = 0.0;
    bool interrotto_ = false, al_50_ = false;
    double copiato_ = 0.0;            // s di ricezione utile nello spazio in corso
    double prossimo_fondo_ = 0.0;
    // antenne
    StatoAntenne stato_;
    double t_ = 0.0;                  // ultimo istante di avanza

    Filo filo_ = Filo::recuperato;
    double filo_pronto_ = 0.0, filo_cieco_fino_ = -1.0, oltre_rottura_s_ = 0.0;
    bool senza_segnale_avvisato_ = false;
    double ssixs_fino_ = -1.0;
    // R1
    CambioRoe roe_iniziali_{};
    std::vector<GiudizioRoe> lanci_;
    std::vector<ContattoIntelligence> intelligence_;
    // R2
    std::vector<Zona> zone_iniziali_;
    std::vector<std::pair<double, std::string>> ingressi_;
    std::vector<Sessione> sessioni_;
    int sessioni_saltate_ = 0;
};

}  // namespace m688
