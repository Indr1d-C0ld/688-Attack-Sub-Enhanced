#pragma once
// ASW aereo dell'Enhanced (docs/13_asw_aereo.md, pezzo A1; dati e procedure dal dossier docs/ricerca/17_asw_aereo.md,
// citato per paragrafo): sonoboe, schemi di posa, MAD, ciclo del sonar a immersione e la macchina a stati dell'IA dei
// mezzi aerei (§7). Vale solo con la regola di simulazione: il classico non lo usa.
//
// Il modulo e' puro e deterministico:
// - la rivelazione acustica resta del motore acustico (nucleo/acustica): boe e sonar a immersione sono ricevitori, e
//   il controllore riceve soltanto le rivelazioni gia' fatte (PercezioneAsw::acustiche);
// - il MAD sta qui, perche' e' geometria e magnetismo (formula OPTEVFOR, §6.3);
// - ogni estrazione casuale usa il Casuale& passato dalla missione (il suo flusso caso_asw_): qui non se ne crea.
// La missione esegue le azioni (boe, armi, ping, marcatori, rapporti) e muove l'aereo secondo il ComandoVolo.

#include <cstdint>
#include <functional>
#include <map>
#include <optional>
#include <string>
#include <vector>

#include "nucleo/casuale.hpp"
#include "nucleo/geometria.hpp"

namespace m688 {

// --- dati: data/motore/asw_aereo.toml (tools/pipeline/asw_aereo.py) ---

enum class ParteAsw : std::uint8_t { usa, urss };
enum class SituazioneAsw : std::uint8_t { pace, crisi, guerra };   // §5.5: in pace e in crisi niente armi
enum class LivelloEquipaggio : std::uint8_t { esperto, medio, scarso };   // §7.4
enum class DifficoltaAsw : std::uint8_t { facile, normale, realistico, minaccia_percepita };   // §10.3

enum class FunzioneBoa : std::uint8_t {
    passiva_omni,       // SSQ-41B, RGB-NM, RGB-1, RGB-75, RGB-15: solo «contatto nel cerchio»
    passiva_dir,        // SSQ-53B DIFAR, SSQ-77A VLAD, RGB-2, RGB-25: rilevamento
    tarata,             // SSQ-57A: rumore ambiente
    attiva_distanza,    // SSQ-47B: ping periodico, solo distanza
    attiva_dir,         // SSQ-62B DICASS: a comando, distanza e rilevamento
    attiva_omni,        // RGB-55A: a comando, distanza
    batitermografo,     // SSQ-36
    esplosiva,          // MGAB: sorgente esplosiva per l'eco (con RGB-15)
    radar_faro          // RLB «Poplavok-1A»: non acustica, marca il punto
};

struct DatiBoa {
    std::string id, nome;
    ParteAsw parte = ParteAsw::usa;
    FunzioneBoa funzione = FunzioneBoa::passiva_omni;
    std::string sensore;                    // voce di acustica.toml usata dal motore acustico (A2); "" se nessuna
    std::vector<double> quote_m;            // quote dell'idrofono selezionabili; la prima e' la predefinita
    std::vector<double> vite_s;             // vite selezionabili; la prima e' la predefinita
    double caduta_m_s = 30.0;               // discesa col paracadute (§4.1)
    double attivazione_s = 120.0;           // dall'impatto all'idrofono stabilizzato (con la quota: stabilizzazione)
    double guasto = 0.05;                   // probabilita' che la boa non funzioni
    double sigma_rilevamento_deg = 0.0;     // passive direzionali
    double portata_m = 0.0;                 // portata indicativa contro il bersaglio di riferimento (per gli schemi)
    double soglia_s = 0.0;                  // sovietiche: picchi piu' brevi di cosi' non fanno trasmettere la boa
    bool soglia = false;                    // sovietiche: trasmette solo oltre la soglia
    double periodo_ping_s = 0.0;            // attive: cadenza (SSQ-47B 10 s); 0 = a comando
    double ping_secondi = 0.0;              // DICASS: energia totale (100 ping-secondi); 0 = senza limite
    double impulso_s = 0.0;                 // attive: durata di un ping (quella del sensore in acustica.toml), per il budget
    double mare_max = 9.0;                  // oltre questo stato del mare la boa e' inutile (RGB-NM: 3)
    double falsi_allarmi_h = 0.0;           // falsi allarmi spontanei per ora di boa attiva (STIMA, §7.2 e §9)
    std::vector<double> frequenze_hz;       // attive: frequenze selezionabili; passive: estremi della banda
    double sl_db = 0.0;                     // attive ed esplosive: livello di sorgente
    std::string fonte, conf;
};

struct DatiMarcatore {
    std::string id, nome;
    ParteAsw parte = ParteAsw::usa;
    double durata_s = 0.0;                  // fiamma o fumo (Mk 25 10-20 min, Mk 58 40-60 min)
    double visibile_m = 0.0;
    bool acustico = false;                  // SUS Mk 84: toni a ~3 kHz
    double frequenza_hz = 0.0;
    std::string fonte, conf;
};

struct DatiSonarImmersione {
    std::string id, nome;                   // AQS-13B, AQS-13F, VGS-2, VGS-3
    std::string sensore;                    // voce di acustica.toml (A2)
    double hover_m = 12.0;                  // quota dell'hover di dip
    double cavo_max_m = 150.0;
    double avvicinamento_s = 75.0;          // dall'arrivo sul punto all'hover stabile
    double calata_m_s = 2.0, recupero_m_s = 2.0;
    double ascolto_min_s = 60.0, ascolto_max_s = 180.0;   // passivo prima dei ping
    int ping_min = 3, ping_max = 6;
    double periodo_ping_s = 8.0;
    double p_attivo = 1.0;                  // probabilita' di passare all'attivo senza contatto (VGS-3: raro)
    double dip_ricerca_s = 900.0;           // durata della stazione in ricerca (SH-3H 15 min)
    double salto_ricerca_m = 18520.0, salto_inseguimento_m = 11112.0;
    double salto_velocita_kn = 100.0;
    double frequenza_hz = 10000.0;
    double sl_db = 211.0;
    bool notte = true;                      // Ka-25/27: niente dip di notte
    double mare_max = 4.0, vento_max_kn = 35.0;   // dossier 02 §7.7; dossier 21 §8 punto 6
    double hs_max_m = 0.0;                  // AQS-13: a mare_max solo con onde sotto questa Hs (0: nessun limite)
    std::string fonte, conf;
};

struct DatiMad {
    double fattore = 1.0;                   // sul modello OPTEVFOR (ASQ-81 = 1)
    double quota_giorno_m = 60.0, quota_notte_m = 60.0;   // quota delle corse MAD
    double velocita_kn = 200.0;
    double cavo_m = 0.0;                    // MAD rimorchiato (elicotteri): dietro e sotto l'aereo
    std::string fonte, conf;
};

struct DatiMezzo {
    std::string id, nome;                   // id = classe di classi.toml
    ParteAsw parte = ParteAsw::usa;
    bool elicottero = false;
    int dal_anno = 0;                       // SH-60F: 1990
    std::string radar;                      // voce di emettitori.toml
    std::optional<DatiMad> mad;
    std::string sonar;                      // voce di sonar_immersione; "" se nessuno
    std::map<std::string, int> boe;         // carico tipico: tipo -> numero
    std::map<std::string, int> armi;        // voce di siluri.toml o bomba -> numero
    std::map<std::string, int> marcatori;
    struct Carico { std::map<std::string, int> boe, armi, marcatori; };
    std::map<std::string, Carico> carichi;  // carichi alternativi (Ka-25: ricerca oppure attacco)
    std::string carico_predefinito;         // quello di boe, armi e marcatori qui sopra
    int canali_boe = 16;                    // boe seguite insieme
    double velocita_transito_kn = 300.0, velocita_ricerca_kn = 200.0, velocita_posa_kn = 200.0;
    double quota_transito_m = 7600.0, quota_ricerca_m = 460.0, quota_posa_m = 460.0, quota_attacco_m = 150.0;
    double tos_base_h = 8.1, tos_nm_per_h = 165.0;   // TOS(d) = tos_base_h - d_nm / tos_nm_per_h (§6.8)
    double riserva_h = 0.5;
    LivelloEquipaggio equipaggio = LivelloEquipaggio::medio;
    int gruppo_tipico = 1;                  // Be-12 in coppia, Ka-27 dal Kiev in 4-6
    // l'elaborazione di bordo: moltiplicatori della conferma (K -> L) e dei falsi allarmi delle boe (Il-38 «Berkut»)
    double conferma = 1.0, falsi_allarmi = 1.0;
    double intervallo_posa_s = 0.0;         // ritmo di posa della ricerca d'area: fra due boe almeno tanto (0: nessun limite)
    std::string fonte, conf;
};

// Moltiplicatori dell'equipaggio (§7.4)
struct DatiEquipaggio {
    double errore_posa_m = 183.0;
    double falsi_allarmi = 1.0, ritardo = 1.0, perdita_contatto = 1.0;
};

// Moltiplicatori di difficolta' (§10.3)
struct DatiDifficolta {
    double scoperta = 1.0;                  // sulla probabilita' delle transizioni di conferma e fix
    double scoperta_urss = 1.0;             // in piu' per i sovietici («minaccia percepita»: x5)
    std::optional<LivelloEquipaggio> equipaggio;   // forza il livello (facile: scarso); vuoto = quello del mezzo
    bool notte = true;                      // operazioni notturne
    bool guasti = true, deriva = true;
};

// Probabilita' (§7.2) per parte: tutte STIMA, «P in finestra_s» diventa un rischio per passo
struct ProbabilitaAsw {
    double falso_allarme = 0.2;             // K -> R: il contatto e' scartato come falso
    double conferma = 0.7, finestra_conferma_s = 600.0;   // K -> L
    double fix = 0.7, finestra_fix_s = 600.0;             // L -> A/I
    double mad_bassa = 0.6, mad_alta = 0.3;               // conferma MAD a 400 ft e a 1000 ft
    double passaggio_contatto = 0.5;        // I: contatto passato alle navi
    double riacquisizione = 0.4, finestra_riacquisizione_s = 1800.0;   // P -> L
    double perdita_s = 600.0;               // senza rivelazioni per tanto: L, I -> P
    double rinuncia_s = 3600.0;             // in P per tanto: -> R
    double scarta_vero = 0.05;              // K -> R anche su un sottomarino vero (errore)
    double crede_falso = 0.03;              // K -> L su un falso contatto (si insegue il nulla: Falkland)
    double perso_spirale = 0.5;             // P, elicotteri: spirale di dip (altrimenti salto lungo davanti o boe)
    double attacco_hover = 0.4;             // USA: Mk 46 dall'hover (altrimenti a 150 ft e 100 kt)
    double attacco_altro = 0.7;             // URSS: attacca un altro elicottero gia' armato del gruppo
};

// Schemi di posa (§6.1 USA, §6.6 URSS) e scelta (§7.3)
enum class SchemaBoe : std::uint8_t {
    barriera, barriera_stretta, chevron, cerchio, cuneo, trappola, box_dicass, cloverleaf,   // USA
    scacchiera, barriera_circolare, spirale, intercettazione, campo,                         // URSS e campi
    incrocio,           // due DIFAR per l'incrocio del rilevamento
    passaggio_mad,      // nessuna boa: passaggio MAD diretto sul fix
    eco_esplosiva,      // Tu-142M: MGAB sopra le RGB-15 (2-3 colpi a 3-4 min)
    attiva,             // RGB-55A o DICASS sul punto
    lungo_rilevamento   // DIFAR lungo un rilevamento solo, lontano: datum = la boa, rotta = il rilevamento, r0 = fin dove
};
const char* nome_schema(SchemaBoe s);

struct OpzioneSchema {
    SchemaBoe schema = SchemaBoe::cerchio;
    double p = 1.0;
    FunzioneBoa funzione = FunzioneBoa::passiva_dir;   // la boa del carico da usare (la prima di quella funzione)
    int boe = 0;                            // 0 = numero tipico dello schema
};
struct SceltaSchema {
    std::string situazione;                 // chiave della tabella §7.3 (es. "datum_fresco_rotta_ignota")
    std::vector<OpzioneSchema> scelte;
};

struct DatiAsw {
    std::map<std::string, DatiBoa> boe;
    std::map<std::string, DatiMarcatore> marcatori;
    std::map<std::string, DatiSonarImmersione> sonar_immersione;
    std::map<std::string, DatiMezzo> mezzi;
    std::map<LivelloEquipaggio, DatiEquipaggio> equipaggi;
    std::map<DifficoltaAsw, DatiDifficolta> difficolta;
    std::map<ParteAsw, ProbabilitaAsw> probabilita;
    std::map<ParteAsw, std::map<std::string, SceltaSchema>> schemi;   // parte -> situazione -> scelte
    double pensiero_s = 5.0;                // il controllore pensa ogni tanto
    // dislocamento in immersione (t) e fattore magnetico (Alfa in titanio: 0,65) dei sottomarini per il MAD (§6.3)
    struct ScafoMad { double dislocamento_t = 0.0, fattore = 1.0; };
    std::map<std::string, ScafoMad> dislocamenti;
    const DatiMezzo* mezzo(const std::string& classe) const;   // nullptr se la classe non e' un mezzo ASW
    const DatiBoa* boa(const std::string& id) const;
    const DatiSonarImmersione* sonar(const std::string& id) const;
};
DatiAsw carica_asw_aereo(const std::string& percorso);   // std::runtime_error

// --- sonoboe ---

struct Sonoboa {
    int id = 0, lanciatore = -1;            // id: lanciatore * 1000 + progressivo (unico nella missione)
    std::string tipo;                       // voce di DatiAsw::boe
    Vec2 pos, deriva;                       // posizione all'impatto; deriva in m/s
    double quota_m = 0.0;                   // dell'idrofono
    double t_posa = 0.0, t_impatto = 0.0, t_attiva = 0.0, t_fine = 0.0;   // s di missione
    bool guasta = false;
    bool attiva_sonar = false;              // boa attiva (SSQ-47B, DICASS, RGB-55A)
    double ping_secondi_usati = 0.0;        // DICASS: l'energia gia' spesa nei ping a comando
};

// Crea la boa lanciata da `quota_lancio_m` in `pos` all'istante t: impatto dopo la discesa, stabilizzazione con la
// quota dell'idrofono (§4.1), vita, guasto e deriva estratti da `caso` (deriva e guasti secondo la difficolta').
Sonoboa crea_boa(const DatiBoa& d, Vec2 pos, double quota_lancio_m, double t, int id, int lanciatore,
                 const DatiDifficolta& diff, Casuale& caso, double quota_m = -1.0, double vita_s = -1.0);
Vec2 posizione_boa(const Sonoboa& b, double t);
bool boa_in_ascolto(const Sonoboa& b, double t);   // stabilizzata, viva e non guasta
bool ping_disponibile(const Sonoboa& b, const DatiBoa& d);   // DICASS: resta energia per un ping (§4.1, 100 ping-secondi)
// stabilizzazione dell'idrofono (§4.1): 100 s a 30 m, 125 s a 60 m, 160 s a 120 m, 240 s a 300 m
double stabilizzazione_s(double quota_m);

// --- schemi di posa ---

struct ParametriSchema {
    Vec2 datum;                             // centro, fix o punto d'arrivo previsto
    double ritardo_s = 0.0;                 // eta' del datum
    std::optional<double> rotta_deg;        // rotta stimata del sottomarino
    double velocita_kn = 8.0;               // velocita' stimata
    double portata_m = 7408.0;              // R50 della boa contro il bersaglio previsto
    double raggio0_m = 1852.0;              // r0 dell'incertezza iniziale (§5.2)
    int boe = 0;                            // 0 = numero tipico dello schema
};
// Raggio del cerchio d'incertezza r(t) = r0 + u·t (§5.2)
double raggio_incertezza_m(double raggio0_m, double velocita_kn, double ritardo_s);
// I punti di posa nell'ordine di volo
std::vector<Vec2> posa_schema(SchemaBoe s, const ParametriSchema& p);

// --- MAD (§6.3) ---

// R50 obliqua del modello OPTEVFOR: 376 m per 4000 t con rumore 0,1 gamma, scalata con la radice cubica del
// dislocamento (in t, immersione) e del rapporto dei rumori
double portata_mad_m(double dislocamento_t, double rumore_gamma = 0.1, double fattore = 1.0);
// Il rumore del MAD col mare (dossier 21 §6.1 e §8 punto 7): la portata x0,8 a mare 4-5 e x0,6 da 6, cioe' il
// rumore diviso per il cubo del fattore (la portata va con la radice cubica)
double rumore_mad_col_mare(double rumore_gamma, int stato_mare);
// probabilita' di contatto a una distanza obliqua: normale con P = 0,5 a R50 e sigma = 0,2·R50
double probabilita_mad(double r50_m, double distanza_obliqua_m);
// semilarghezza della spazzata: sqrt(R^2 - dz^2), 0 se dz >= R
double spazzata_mad_m(double r50_m, double dz_m);

// MAD di un aereo nella missione: per ogni sottomarino in immersione entro 2·R50 segue il passaggio e, al punto di
// minima distanza (quando la distanza riprende a crescere o il bersaglio esce), estrae una volta sola con
// probabilita_mad. Il MAD rimorchiato sta a meta' del cavo sotto l'aereo.
struct BersaglioMad { int id = -1; Vec2 pos; double quota_m = 0.0, dislocamento_t = 0.0; };
class SensoreMad {
public:
    SensoreMad() = default;
    explicit SensoreMad(const DatiMad& d) : d_(d) {}
    // le unita' sentite in questo passo
    std::vector<int> passo(Vec2 aereo, double quota_aereo_m, double rumore_gamma,
                           const std::vector<BersaglioMad>& bersagli, Casuale& caso);
    const DatiMad& dati() const { return d_; }

private:
    DatiMad d_;
    std::map<int, double> minimo_;          // bersaglio -> distanza obliqua minima del passaggio in corso
};

// --- sonar a immersione (§6.4-6.5) ---

enum class FaseDip : std::uint8_t { nessuna, avvicinamento, calata, ascolto, ping, recupero, salto };
const char* nome_fase_dip(FaseDip f);

class CicloDip {
public:
    CicloDip() = default;
    explicit CicloDip(const DatiSonarImmersione* d) : d_(d) {}
    // comincia una stazione in `t` con il trasduttore a `quota_m`; ricerca: durata lunga, inseguimento: corta
    void inizia(double t, double quota_m, bool inseguimento, Casuale& caso);
    // avanza fino a t: aggiorna la fase e il cavo; restituisce i ping emessi in (t_prec, t]
    int avanza(double t);
    void forza_attivo(double t);            // contatto o sospetto: passa ai ping
    void recupera(double t);                // interrompe la stazione
    FaseDip fase() const { return fase_; }
    double cavo_m() const { return cavo_; }   // filato dall'elicottero: hover piu' quota del trasduttore
    double quota_trasduttore_m() const;
    bool in_acqua() const { return quota_trasduttore_m() > 0.0; }
    bool in_hover() const { return fase_ != FaseDip::nessuna && fase_ != FaseDip::salto; }
    bool finito() const { return fase_ == FaseDip::nessuna || fase_ == FaseDip::salto; }
    double t_fine_previsto() const { return t_fine_; }
    // durata totale prevista della stazione, dall'avvicinamento alla fine del recupero
    double durata_stazione_s() const;

private:
    double durata_fase(FaseDip f) const;
    void aggiorna_cavo(double t);

    const DatiSonarImmersione* d_ = nullptr;
    FaseDip fase_ = FaseDip::nessuna;
    double t_ = 0.0, t_fase_ = 0.0, t_fine_ = 0.0;
    double quota_ = 0.0, cavo_ = 0.0, cavo_inizio_recupero_ = 0.0, ascolto_s_ = 0.0;
    int ping_ = 0, ping_fatti_ = 0;
    bool inseguimento_ = false;
};

// --- percezione e azioni ---

struct Cue {
    enum class Tipo : std::uint8_t {
        sosus,              // contatto d'intelligence dalla radio (SOSUS, SURTASS, HF/DF)
        nave_colpita,       // datum sulla nave colpita (il GOTO_TGT degli script)
        siluro_sentito,     // flaming datum: lancio sentito da una nave amica
        radar_esm,          // albero visto al radar o all'ESM
        contatto_nave,      // contatto passato da una nave
        immersione_vista,   // sottomarino visto al radar mentre s'immerge
        zona                // missione di ricerca d'area senza contatto
    };
    Tipo tipo = Tipo::sosus;
    Vec2 pos;
    double t = 0.0;                         // istante del fix (il ritardo cresce da qui)
    double raggio_m = 1852.0;               // incertezza iniziale
    std::optional<double> rotta_deg, velocita_kn;
    int unita = -1;                         // nave colpita o nave che passa il contatto
};

// Una rivelazione gia' fatta dal motore acustico (o dal radar della missione)
struct Rivelazione {
    int boa_o_aereo = 0;                    // id della boa, o id dell'aereo per il suo sonar a immersione
    double t = 0.0, livello_db = 0.0;       // eccesso di segnale
    std::optional<double> rilevamento, distanza_m;   // vero dal ricevitore, gia' con l'errore del sensore
    int sorgente = -1;                      // unita' sentita (verita'); -1 rumore o paesaggio sonoro
    bool sottomarino = false;               // verita': la sorgente e' un sottomarino (per la classificazione)
};

struct PercezioneAsw {
    Vec2 pos;                               // dell'aereo
    double quota_m = 0.0, rotta = 0.0;
    std::vector<Rivelazione> acustiche;
    std::vector<int> mad;                   // unita' sentite dal MAD in questo passo (le calcola rivela_mad)
    std::vector<Vec2> radar;                // alberi o battelli in superficie al radar
    std::vector<Cue> cue;                   // cue nuovi
    double ora_locale = 12.0;
    int mare = 3;
    double vento_kn = 10.0;
    double hs_m = -1.0;                     // Hs vera del meteo (< 0: il centro della classe)
    bool temporale_vicino = false;          // fulmini entro 5 nm: niente immersione (dossier 21 §7.1)
    double temperatura_c = 15.0;            // 40 °C: l'SH-3H non tiene l'hover di dip
    double strato_m = 0.0;                  // SLD dal batitermografo (0: nessuno strato): dove calare il trasduttore
    double fondale_m = 1e9;                 // sotto l'aereo
    std::map<std::string, double> fondale_minimo;   // arma -> fondale minimo per l'aviolancio (dossier 17 §6.7)
    std::optional<SituazioneAsw> situazione;   // cambio di ROE dalla radio
    // dove una boa, il trasduttore del sonar a immersione o un'arma entrano in acqua (sotto il pack no: polynye e canali
    // si'); vuota: ovunque (il mare senza ghiaccio, le prove)
    std::function<bool(Vec2)> acqua_libera;
};

enum class StatoAsw : std::uint8_t {
    transito, ricerca, datum, contatto, localizzazione, inseguimento, attacco, verifica, perso, cambio, rientro
};
const char* nome_stato(StatoAsw s);
char sigla_stato(StatoAsw s);               // T R D K L I A V P C X del §7.1

struct ComandoVolo {
    Vec2 meta;
    double quota_m = 0.0, velocita_kn = 0.0;
    bool hover = false;
    double cavo_m = 0.0;                    // trasduttore in acqua (lunghezza filata)
    bool luci = true;                       // anticollisione e navigazione (spente in caccia, §8.1)
    bool radar = false;                     // radar acceso (EMCON)
    bool libero = false;                    // in transito o in ricerca senza niente da fare: segue la rotta dello script
};

struct AzioneAsw {
    enum class Tipo : std::uint8_t {
        posa_boa, cala_sonar, ping, recupera_sonar, lancia_arma, marcatore, sus, passa_contatto, rapporto,
        comanda_boa        // ping a comando di una boa attiva (DICASS, RGB-55A)
    } tipo = Tipo::rapporto;
    Vec2 pos;
    std::string oggetto;                    // tipo di boa, arma, marcatore o sonar
    int bersaglio = -1;                     // boa comandata, o unita' stimata dell'arma (-1 = sul punto)
    double quota_m = 0.0;                   // idrofono o trasduttore; arma: quota di sgancio
    double rotta = 0.0;                     // arma: rotta di sgancio
    std::optional<Sonoboa> boa;             // posa_boa: la boa creata (la missione la mette in boe())
    std::string codice;                     // rapporto, sus: codice del messaggio (fascia 380-389)
};

struct TracciaAsw {                         // la stima del controllore sul bersaglio
    bool valida = false;
    Vec2 pos;
    double errore_m = 1e9, t = 0.0;         // incertezza e istante della stima
    std::optional<double> rotta_deg, velocita_kn;
    int rivelazioni = 0;
    bool confermata = false;
    int sorgente = -1;                      // verita': la sorgente piu' sentita fra le rivelazioni associate
    std::optional<double> rilevamento_solo; // la stima viene da un rilevamento DIFAR solo: da dove e fin dove
    Vec2 da_rilevamento;
    double lontano_m = 0.0;
    std::map<int, int> voti;                // sorgente -> rivelazioni associate
};

// --- controllore (§7) ---

struct ConfigurazioneAsw {
    SituazioneAsw situazione = SituazioneAsw::pace;
    DifficoltaAsw difficolta = DifficoltaAsw::normale;
    std::optional<LivelloEquipaggio> equipaggio;   // vuoto: quello del mezzo (o della difficolta')
    Vec2 base;                              // aeroporto, portaerei o nave madre
    Vec2 zona;                              // centro della zona di pattuglia
    double raggio_zona_m = 0.0;
    double rotta_presunta = 0.0;            // per barriere senza cue
    std::string carico;                     // voce di DatiMezzo::carichi; "" = il predefinito
    bool armi = true;                       // false: niente attacco nemmeno in guerra
    bool ricognizione = false;              // passa le coordinate (rapporto «ricognizione» ogni 5 min col fix)
    int capogruppo = -1, indice_gruppo = 0, numero_gruppo = 1;
    double distanza_gruppo_m = 3250.0;      // Ka-27 in linea a 3-3,5 km
};

class ControlloreAsw {
public:
    ControlloreAsw() = default;
    ControlloreAsw(const DatiAsw& dati, const std::string& classe, int unita, const ConfigurazioneAsw& conf,
                   double t0);

    // Un pensiero all'istante t (la missione lo chiama ogni dati.pensiero_s): aggiorna lo stato e restituisce le
    // azioni da eseguire. Il volo si legge con volo().
    std::vector<AzioneAsw> passo(double t, const PercezioneAsw& p, Casuale& caso);

    const ComandoVolo& volo() const { return volo_; }
    StatoAsw stato() const { return stato_; }
    const TracciaAsw& traccia() const { return traccia_; }
    const std::vector<Sonoboa>& boe() const { return boe_; }
    const DatiMezzo* mezzo() const { return mezzo_; }
    const ConfigurazioneAsw& configurazione() const { return conf_; }
    void sposta_base(Vec2 p) { conf_.base = p; }   // la nave madre si e' mossa (base_unita): il rientro va dov'e' ora
    const CicloDip& dip() const { return dip_; }
    int unita() const { return unita_; }
    double tos_h() const { return tos_h_; }
    double fine_zona_t() const { return fine_zona_t_; }
    int boe_rimaste(const std::string& tipo) const;
    int armi_rimaste() const;
    // le boe ascoltate in questo momento (al piu' canali_boe, le piu' recenti in ascolto): la missione puo' valutare
    // solo queste, e il controllore ignora le rivelazioni delle altre
    std::vector<int> boe_seguite(double t) const;
    LivelloEquipaggio equipaggio() const { return livello_; }
    SituazioneAsw situazione() const { return conf_.situazione; }
    double t_stato() const { return t_stato_; }
    // registro dei passaggi di stato (debriefing, prove)
    struct Passaggio { double t; StatoAsw da, a; std::string motivo; };
    const std::vector<Passaggio>& storia() const { return storia_; }

    // L'arma e' arrivata: la missione dice se ha colpito (per la verifica)
    void esito_arma(double t, bool colpito);
    // La missione rinuncia all'aereo (abbattuto, richiamato)
    void richiama(double t);
    // il sonar a immersione si puo' calare (notte, mare, onde, vento, temporale, caldo: dossier 21 §8 punto 6)
    bool dip_possibile(const PercezioneAsw& p) const;

private:
    struct Rilevamento { int boa; Vec2 da; double rilevamento, t; };
    void riprendi(double t, const std::string& motivo);
    std::string arma_adatta(const PercezioneAsw& p) const;   // la preferita a bordo che il fondale consente ("" nessuna)
    Vec2 punto_previsto(double t) const;          // la traccia portata avanti con rotta e velocita' stimate
    double incertezza_prevista(double t) const;   // e il suo errore
    bool anello_chiude(const ParametriSchema& par) const;   // la barriera circolare del datum sovietico
    void cambia(StatoAsw s, double t, const std::string& motivo);
    bool usa() const;
    const ProbabilitaAsw& prob() const;
    double moltiplicatore_scoperta() const;
    std::string boa_per(FunzioneBoa f) const;
    void pianifica(SchemaBoe s, FunzioneBoa f, int n, const ParametriSchema& par, double t);
    void scegli_schema(const std::string& situazione, const ParametriSchema& par, double t, Casuale& caso);
    ParametriSchema parametri_traccia(double t, double portata_m) const;
    void posa_in_volo(double t, const PercezioneAsw& p, Casuale& caso, std::vector<AzioneAsw>& out);
    void gestisci_dip(double t, const PercezioneAsw& p, Casuale& caso, std::vector<AzioneAsw>& out);
    void prossimo_dip(Vec2 punto, double t, bool inseguimento);
    bool aggiorna_traccia(const Rivelazione& r, const PercezioneAsw& p);   // falso: rivelazione di un altro contatto
    void falsi_allarmi(double t, double dt, Casuale& caso, std::vector<Rivelazione>& out);
    void ricevi_cue(const Cue& c, double t, Casuale& caso);
    void lancia(double t, const PercezioneAsw& p, Casuale& caso, bool da_hover, std::vector<AzioneAsw>& out);
    void marcatore(Vec2 pos, std::vector<AzioneAsw>& out);
    void imposta_volo(const PercezioneAsw& p);
    Vec2 errore_posa(Vec2 punto, Casuale& caso) const;
    // l'acqua libera piu' vicina a p entro entro_m (p stesso se e' libera; nessuna sul pack chiuso)
    std::optional<Vec2> acqua_vicina(Vec2 p, double entro_m) const;

    const DatiAsw* dati_ = nullptr;
    const DatiMezzo* mezzo_ = nullptr;
    const DatiSonarImmersione* sonar_ = nullptr;
    DatiDifficolta diff_;
    DatiEquipaggio equip_;
    LivelloEquipaggio livello_ = LivelloEquipaggio::medio;
    ConfigurazioneAsw conf_;
    int unita_ = -1;
    StatoAsw stato_ = StatoAsw::transito;
    double t_stato_ = 0.0, t_ultimo_ = 0.0;
    ComandoVolo volo_;
    TracciaAsw traccia_;
    std::vector<Sonoboa> boe_;
    std::map<std::string, int> carico_boe_, carico_armi_, carico_marcatori_;
    int progressivo_boe_ = 0;
    std::vector<Vec2> da_posare_;           // punti dello schema ancora da posare, nell'ordine di volo
    std::function<bool(Vec2)> acqua_libera_;   // dall'ultima percezione (PercezioneAsw::acqua_libera)
    bool sul_pack_ = false;                 // l'ultimo schema e' caduto tutto sul ghiaccio: si rinuncia al datum
    std::string tipo_da_posare_;
    std::vector<double> quote_da_posare_;
    std::vector<Vec2> dip_da_fare_;         // punti delle prossime stazioni di dip
    // il lavoro che un contatto interrompe, ripreso se l'equipaggio lo giudica falso (taratura A6, Il-38)
    struct Ripresa {
        bool valida = false;
        StatoAsw stato = StatoAsw::ricerca;
        TracciaAsw traccia;
        std::vector<Vec2> da_posare;
        std::string tipo;
        std::vector<double> quote;
    };
    Ripresa ripresa_;
    double t_ripiano_ = -1e18;              // ultimo schema di ricerca rifatto con le boe esaurite
    int passo_mad_ = 0;
    double t_ultima_posa_ = -1e18;                     // ricerca MAD dell'ala fissa sull'area d'incertezza: il punto in corso
    bool dip_inseguimento_ = false, dip_in_volo_ = false;
    FaseDip fase_dip_prec_ = FaseDip::nessuna;
    CicloDip dip_;
    std::vector<Rilevamento> rilevamenti_;  // rilevamenti DIFAR recenti per gli incroci
    // il LAMPS I (SH-2F, docs/ricerca/37): senza boe direzionali la stima viene dalle omni che sentono insieme, poi dalle
    // distanze delle attive di sola distanza (SSQ-47B) trilaterate
    struct Misura { int boa; Vec2 da; double valore, t; };   // omni: il livello (dB); SSQ-47B: la distanza (m)
    std::vector<Misura> omni_, distanze_;
    bool senza_direzionali_ = false;        // nessuna boa direzionale nel carico (passiva_dir, attiva_dir)
    double tos_h_ = 0.0, fine_zona_t_ = 1e18;
    double t_arrivo_ = -1.0, t_ultima_rivelazione_ = -1e18, t_lancio_ = -1e18, t_mad_ = -1e18;
    double t_decisione_ = -1.0;             // K su un falso contatto: quando si decide
    bool decisione_crede_ = false, ultima_vera_ = false;
    bool scelta_fatta_ = false;             // lo schema dello stato in corso e' stato scelto (si azzera a ogni cambio)
    bool simulato_ = false, passato_ = false, colpito_ = false, marcato_ = false;
    double attesa_verifica_s_ = 420.0, t_schema_inseguimento_ = -1e18, t_dicass_ = -1e18;
    bool rapporto_cambio_ = false;
    double t_ricognizione_ = -1e18;
    std::map<int, double> scartati_;        // sorgente classificata come non sottomarino -> fino a quando
    std::vector<Passaggio> storia_;
};

// TOS(d) in ore per un mezzo (§6.8); 0 se la zona e' fuori raggio
double tempo_in_zona_h(const DatiMezzo& m, double distanza_m);
// probabilita' P in una finestra trasformata in rischio per un passo dt: 1 - (1 - P)^(dt / finestra)
double probabilita_passo(double p_finestra, double finestra_s, double dt_s);
// notte per le operazioni (Ka-25/27 senza dip, luci): prima delle 6 e dopo le 19 locali (STIMA)
bool notte_asw(double ora_locale);

}  // namespace m688
