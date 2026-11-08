#pragma once
// Missili lanciati dal sottomarino nell'Enhanced: dati di tiro, piano, volo, ricerca e aggancio, TLAM
// (docs/21_stazione_missili.md §11; dossier docs/ricerca/27_missili_impiego.md e 05 §1-§3).
//
// Il modulo e' puro: non conosce la missione, come nucleo/siluro e nucleo/navigazione. Lavora in due mondi:
// - il PIANO usa solo cio' che il battello sa: la sua posizione stimata, la soluzione del bersaglio con la sua
//   ellisse, i contatti che conosce. Da li' vengono previsto, accensione del seeker, area di ricerca e probabilita'
//   di successo stimata (il CCS vero la calcolava: APL 1992);
// - il VOLO parte dalla posizione vera del lanciatore ma segue la rotta del piano: lo scarto fra stimata e vera al
//   lancio sposta tutta la geometria, come nel missile vero (la tratta sul mare e' inerziale e niente la corregge).
// Il seeker DSQ-28 e' un radar a ricerca sequenziale senza classificazione: aggancia il primo eco valido dell'area,
// dal piu' vicino, nemico o neutrale che sia (dossier 27 §3). Il TASM ha in piu' l'ESM sui radar del bersaglio voluto.
// L'impatto e' solo un evento: danno, testata e difese delle navi li fa la missione.
// I dati delle classi sono in data/motore/missili.toml (tools/pipeline/missili.py), con fonte e confidenza.
// Il livello classico non lo usa.
//
// Unita': metri, secondi, m/s, gradi bussola (0 = nord, orario), x verso est e y verso nord (nucleo/geometria.hpp).
// Ellissi a 1 sigma (EllisseErrore di nucleo/navigazione.hpp). Deterministico dal seme.

#include <array>
#include <cstdint>
#include <map>
#include <optional>
#include <string>
#include <vector>

#include "nucleo/casuale.hpp"
#include "nucleo/difese_navi.hpp"
#include "nucleo/geometria.hpp"
#include "nucleo/navigazione.hpp"

namespace m688 {

// --- dati delle classi (data/motore/missili.toml) ---
enum class RuoloMissile : std::uint8_t { antinave, attacco_terra, asw };
enum class UscitaMissile : std::uint8_t { capsula, booster_subacqueo, pozzo, contenitore };   // Harpoon, Tomahawk da tubo, VLS, sovietici
enum class Terminale : std::uint8_t { radente, pop_up };   // dal missile: sui sottomarini del 1989 non si sceglie

struct DatiMissile {
    std::string id, nome, fonte, confidenza, nota;
    RuoloMissile ruolo = RuoloMissile::antinave;
    UscitaMissile uscita = UscitaMissile::capsula;
    double velocita_m_s = 240.0, quota_crociera_m = 15.0, portata_m = 0.0, distanza_min_m = 0.0;
    double quota_lancio_max_m = 60.0, velocita_lancio_max_kn = 10.0;   // oltre: avviso (dossier 05 §1.3)
    double quota_lancio_min_m = 0.0;           // sotto: avviso e rischio in salita (capsula vicino alla vela); 0 = nessun minimo
    double salita_s = 0.0, boost_s = 0.0, discesa_s = 0.0;             // capsula senza motore, booster, discesa in crociera
    Terminale terminale = Terminale::radente;
    double pop_up_m = 0.0;
    std::array<double, 3> acquisizione_m{};   // portata del seeker per dimensione del bersaglio (piccola, media, grande)
    double semiapertura_gradi = 45.0;
    int mare_max = 5;                          // oltre il seeker vede a -30% e perde l'aggancio
    double p_acquisizione_scorta = 0.4;        // un eco che non e' il piu' grande dell'area si prende con questa P (Becker)
    bool esm = false;                          // TASM
    std::vector<std::string> esm_radar;        // id dei radar (emettitori.toml) che l'ESM cerca
    std::string radar_seeker;                  // id dell'emettitore della testa (dsq28, granit, ...); vuoto se non c'e'
    double quota_ricerca_m = 0.0;              // TASM: sale per cercare; 0 = cerca alla quota di crociera
    double testata_kg = 0.0;
    double esplosivo_kg = 0.0;               // l'esplosivo della testata: il danno del colpo (0: dalla testata)
    double affidabilita = 0.95;                // in crociera (GAO 1995); il resto si guasta in salita o in volo
    double accensione_s = 0.0, allineamento_s = 0.0;               // le spie PWR e ALN
    double preparazione_primo_s = 0.0, preparazione_altri_s = 0.0;  // TLAM, in parallelo dopo il primo
    int anno_dal = 0, anno_al = 9999;
};

struct CatalogoMissili {
    std::map<std::string, DatiMissile> missili;
    struct Arma { std::string arma, missile; int dal = 0, al = 9999; };
    std::vector<Arma> armi;                    // quale classe per un'arma del carico in un anno
    const DatiMissile* missile(const std::string& id) const;                    // nullptr se manca
    const DatiMissile* per_arma(const std::string& arma, int anno) const;       // harpoon -> 84A o 84C; nullptr se no
    // salva e riserva: il tiro dell'IA senza script (missione_missili.cpp, passo_missili_ia); 0 = la STIMA del motore
    struct Lanciatore { std::string classe, missile; int dal = 0, al = 9999; int salva = 0, riserva = 0; };
    std::vector<Lanciatore> lanciatori;        // il missile antinave di una classe dell'IA (chiave di classi.toml) in un anno
    const DatiMissile* per_classe(const std::string& classe, int anno) const;   // oscar -> p700; nullptr se non ne ha
    const Lanciatore* lanciatore(const std::string& classe, int anno) const;    // la voce di per_classe; nullptr se no
    std::map<std::string, DifesaClasse> difese;   // le difese delle navi per classe, [[difesa_nave]] (nucleo/difese_navi.hpp)
    const DifesaClasse* difesa(const std::string& classe) const;                // nullptr se fuori tabella: resta il solo byte
};
CatalogoMissili carica_missili(const std::string& percorso);   // std::runtime_error se manca o e' incompleto

// --- il bersaglio come lo sa il battello (dossier 27 §4.1) ---
enum class FonteDati : std::uint8_t { tma, periscopio, esm, radio };

struct BersaglioStimato {
    FonteDati fonte = FonteDati::tma;
    int contatto = -1;                  // l'id del contatto (del giocatore o dell'IA), per i messaggi
    int dimensione = 1;                 // creduta: 0 piccola, 1 media, 2 grande (dalla classe; 1 se ignota)
    double t_pos = 0.0;                 // l'ora della posizione: per la radio quella del rapporto, non dell'arrivo
    Vec2 pos;                           // stimata a t_pos
    bool distanza_nota = true;          // falso: solo rilevamento (ESM, TMA inosservabile) -> solo BOL
    double rilevamento = 0.0;           // dal battello, quando la distanza non e' nota
    bool moto_noto = false;             // rotta e velocita' note
    double rotta = 0.0, velocita_m_s = 0.0;
    EllisseErrore ellisse;              // a t_pos, 1 sigma
    double sigma_velocita_m_s = 1.0, sigma_rotta_deg = 10.0, velocita_max_m_s = 15.0;   // per la crescita
    bool relativo = true;               // vero: misurato dal battello (TMA, periscopio, ESM); falso: geografico (radio)
};

// L'ellisse del bersaglio all'istante t, dall'ora della posizione (appunti OTH §3.2). Col moto noto cresce lungo la
// rotta (sigma_velocita * eta') e attraverso (velocita * eta' * sigma_rotta); senza, e' un cerchio il cui raggio cresce
// come velocita_max * eta'. Prima di t_pos resta quella data
EllisseErrore ellisse_a(const BersaglioStimato& b, double t);
Vec2 posizione_a(const BersaglioStimato& b, double t);   // la stima proiettata col moto noto (ferma senza)
// Somma di due ellissi indipendenti (covarianze), per esempio il dato geografico piu' la navigazione del battello
EllisseErrore somma(const EllisseErrore& a, const EllisseErrore& b);
// Aiuto per i rapporti radio (il quadro radio della missione): posizione, ora del fix, raggio dell'ellisse a 1 sigma
// all'ora del fix, rotta e velocita' se il rapporto le ha
BersaglioStimato da_rapporto(int contatto, Vec2 pos, double t_fix, double raggio_m, std::optional<double> rotta,
                             std::optional<double> velocita_m_s, double velocita_max_m_s);

// --- il piano di tiro, per la consolle e per la carta ---
enum class ModoLancio : std::uint8_t { rbl, bol };
enum class Taglia : std::uint8_t { piccola, media, grande };

struct ImpostazioniMissile {
    ModoLancio modo = ModoLancio::rbl;
    Taglia taglia = Taglia::media;      // RBL
    double srch_m = 0.0, dstr_m = 0.0;  // BOL: accensione e autodistruzione dal lancio (0 = predefinita, fino alla portata)
};

struct StatoLanciatore {
    Vec2 pos;                           // STIMATA: il piano non vede la verita'
    EllisseErrore navigazione;          // l'ellisse di navigazione dichiarata (1 sigma)
    double quota_m = 0.0, velocita_kn = 0.0, rotta = 0.0;
    int stato_mare = 3;
};

struct ContattoNoto {                   // i contatti del battello, per l'avviso e la probabilita'
    int id = -1;
    Vec2 pos;                           // stimata, all'istante del piano
    int dimensione = 1;                 // 0 piccola, 1 media, 2 grande (dalla classe creduta; 1 se ignota)
    bool neutrale_creduto = false;
    bool amico_creduto = false;
};

enum class Avviso : std::uint8_t {
    // bloccanti
    fuori_portata, troppo_vicino, quota, solo_bol, senza_bersaglio,
    // solo avvisi
    velocita, quota_bassa, mare, dato_vecchio, neutrali_in_area, rotta_ignota, amici_in_area
};
bool bloccante(Avviso a);
const char* nome_avviso(Avviso a);      // per la barra dei messaggi

struct AreaRicerca {                    // nel teatro, come il battello la crede
    enum class Forma : std::uint8_t { scatola, ventaglio, serpentina };
    Forma forma = Forma::scatola;
    std::vector<Vec2> contorno;         // poligono chiuso da disegnare
    std::vector<Vec2> percorso;         // la serpentina del TASM; vuoto per gli altri
};

struct PianoTiro {
    bool valido = false;                // nessun avviso bloccante
    std::vector<Avviso> avvisi;         // i bloccanti prima
    int contatto = -1;                  // il bersaglio designato (BersaglioStimato::contatto)
    double t = 0.0;                     // istante del piano
    Vec2 lancio;                        // stimato
    double rotta = 0.0;                 // del missile
    double tempo_volo_s = 0.0;          // fino al punto previsto (o all'accensione per il BOL)
    double distanza_m = 0.0;            // dal lancio al previsto
    Vec2 previsto;                      // la stima del bersaglio all'arrivo
    EllisseErrore ellisse_arrivo;       // del bersaglio all'arrivo, sommata alla navigazione se il dato e' geografico
    Vec2 accensione;                    // dove si accende il seeker
    double accensione_m = 0.0;          // dal lancio
    double fine_m = 0.0;                // dal lancio, dove finisce la ricerca (autodistruzione o carburante)
    AreaRicerca area;
    double eta_dato_s = 0.0;            // eta' del dato all'accensione
    std::vector<int> in_area;           // contatti noti nell'area, nell'ordine in cui il seeker li vedrebbe
    std::vector<int> neutrali_in_area;  // quelli di in_area creduti neutrali (ContattoNoto::neutrale_creduto), per la carta
    std::vector<int> amici_in_area;     // quelli di in_area creduti amici (ContattoNoto::amico_creduto)
    double p_area = 0.0;                // P(il bersaglio e' nell'area)
    double p_primo = 0.0;               // P(e' il primo eco preso fra i contatti noti)
    double p_successo = 0.0;            // stimata: p_area * p_primo * affidabilita' (senza le difese delle navi)
    double orizzonte_radar_m = 0.0;     // del bersaglio contro il missile in crociera
};

PianoTiro pianifica(const DatiMissile& m, const ImpostazioniMissile& imp, const BersaglioStimato& b,
                    const StatoLanciatore& l, const std::vector<ContattoNoto>& contatti, double t);

// costanti del piano (STIME DI GIOCO, docs/21 §11.5 e dossier 27 §3)
inline constexpr std::array<double, 3> SCATOLA_SIGMA = {1.7, 2.5, 3.5};            // mezza profondita' in sigma
inline constexpr std::array<double, 3> SCATOLA_MIN_M = {5000.0, 10000.0, 15000.0};  // mezza profondita' minima
inline constexpr std::array<double, 3> SCATOLA_LARGA_MIN_M = {3000.0, 6000.0, 10000.0};   // mezza larghezza minima
inline constexpr double VENTAGLIO_INIZIO_GRADI = 15.0, VENTAGLIO_INIZIO_M = 5000.0;
inline constexpr double VENTAGLIO_PASSO_GRADI = 7.5, VENTAGLIO_PASSO_M = 2500.0, VENTAGLIO_PASSO_S = 10.0;
inline constexpr double VENTAGLIO_MAX_M = 20000.0;
inline constexpr double SERPENTINA_SALITA_M = 40000.0, SERPENTINA_GAMBA_M = 40000.0, SERPENTINA_PASSO_M = 25000.0;
inline constexpr double P_ACQUISIZIONE_GRANDE = 0.97;   // un eco piu' grande del bersaglio lo «ruba» (Becker)
inline constexpr double DATO_VECCHIO_S = 1800.0;        // oltre: avviso
inline constexpr double DATUM_M = 3.0 * MIGLIO;         // per la carta: dove arrivera' l'ASW
inline constexpr double ALTEZZA_BERSAGLIO_M = 20.0;     // per l'orizzonte radar, se la classe non la da'

// --- la preparazione (spie PWR e ALN; DOOR e SLIDE sono del tubo) ---
enum class FasePreparazione : std::uint8_t { spento, accensione, allineamento, pronto };
const char* nome_fase(FasePreparazione f);

class PreparazioneMissile {
public:
    // Antinave: accensione_s e allineamento_s. TLAM (senza quei tempi, con preparazione_primo_s): il primo missile di
    // bordo 15 minuti, gli altri preparazione_altri_s ciascuno (docs/21 §3.4), un terzo per l'accensione e due terzi per
    // l'allineamento. `primo`: nessun altro TLAM di bordo e' gia' pronto o in preparazione (lo sa chi prepara)
    explicit PreparazioneMissile(const DatiMissile& m, bool primo = true);
    void avvia();                       // dallo spento comincia l'accensione; altrimenti niente
    void spegni();
    void avanza(double dt);
    FasePreparazione fase() const { return fase_; }
    double residuo_s() const { return residuo_; }   // della fase in corso
    bool pwr() const { return fase_ == FasePreparazione::allineamento || fase_ == FasePreparazione::pronto; }
    bool aln() const { return fase_ == FasePreparazione::pronto; }

private:
    double accensione_s_, allineamento_s_;
    FasePreparazione fase_ = FasePreparazione::spento;
    double residuo_ = 0.0;
};
// TLAM: il primo missile e poi gli altri in parallelo (secondi per averne n pronti)
double preparazione_tlam_s(const DatiMissile& m, int n_missili);

// --- il volo ---
enum class FaseMissile : std::uint8_t { salita, boost, crociera, ricerca, inseguimento, terminale, impatto, esaurito,
                                        guasto };
const char* nome_fase(FaseMissile f);

struct EcoNave {                        // la VERITA': ogni unita' di superficie del mondo, a ogni passo
    int id = -1;
    Vec2 pos;
    double rotta = 0.0, velocita_m_s = 0.0;
    int dimensione = 1;                 // dalla classe: dimensione_da_lunghezza
    double altezza_m = ALTEZZA_BERSAGLIO_M;   // sovrastruttura, per l'orizzonte radar
    std::vector<std::string> radar_accesi;    // id dei radar accesi (per l'ESM del TASM)
};
int dimensione_da_lunghezza(double lunghezza_m);   // 0 sotto 90 m, 1 fino a 180 m, 2 oltre (STIMA)

struct EventoMissile {
    enum class Tipo : std::uint8_t { affioramento, boost, crociera, seeker_acceso, aggancio, perdita, terminale,
                                     impatto, esaurito, guasto };
    Tipo tipo;
    double tempo = 0.0;                 // dal lancio
    int nave = -1;                      // aggancio, perdita, impatto
    Vec2 pos;
};

class Missile {
public:
    // `piano` e' quello del battello; `lancio_vero` la posizione vera del lanciatore: lo scarto sposta tutto il volo
    Missile(const DatiMissile& m, const ImpostazioniMissile& imp, const PianoTiro& piano, Vec2 lancio_vero,
            double quota_lancio_m, double velocita_lancio_kn, int lanciatore, std::uint64_t seme);
    void avanza(double dt, const std::vector<EcoNave>& navi, int stato_mare);
    std::vector<EventoMissile> eventi();   // dall'ultima chiamata (si svuota)

    const DatiMissile& dati() const { return m_; }
    FaseMissile fase() const { return fase_; }
    bool attivo() const;
    Vec2 pos() const { return pos_; }      // vera
    double quota() const { return quota_; }
    double rotta() const { return rotta_; }
    double tempo_s() const { return t_; }  // dal lancio
    int bersaglio() const { return bersaglio_; }
    int lanciatore() const { return lanciatore_; }
    bool seeker_acceso() const;            // il radar `radar_seeker` emette (per l'ESM delle navi)
    double percorso_m() const { return percorso_; }

private:
    void evento(EventoMissile::Tipo t, int nave = -1);
    bool nell_area(Vec2 p) const;          // nell'area del piano, riportata nel mondo vero
    void cerca(const std::vector<EcoNave>& navi, int stato_mare);
    void insegui(double dt, const std::vector<EcoNave>& navi, int stato_mare);

    DatiMissile m_;
    ImpostazioniMissile imp_;
    PianoTiro piano_;
    Vec2 scarto_;                          // vera - stimata al lancio: sposta il piano nel mondo vero
    Vec2 pos_;
    double quota_ = 0.0, rotta_ = 0.0, t_ = 0.0, percorso_ = 0.0;
    FaseMissile fase_ = FaseMissile::salita;
    int lanciatore_ = -1, bersaglio_ = -1;
    std::vector<int> scartati_;            // echi gia' esaminati e scartati
    std::size_t gamba_ = 0;                // TASM: il tratto della serpentina
    bool guasto_volo_ = false;
    double t_guasto_ = 0.0;
    Casuale rng_;
    std::vector<EventoMissile> eventi_;
};

// La stima del missile per la carta (decisione 4 di docs/21 §10): dal piano e dal tempo dal lancio, mai dalla verita'
struct StimaVolo { Vec2 pos; FaseMissile fase = FaseMissile::salita; };
StimaVolo stima_volo(const PianoTiro& p, const ImpostazioniMissile& imp, const DatiMissile& m, double t_dal_lancio);

// --- TLAM (dossier 27 §4.2; appunti OTH §6.2) ---
struct MissioneTlam {                   // dalla libreria di bordo (file della missione)
    std::string id, bersaglio;          // bersaglio: il nome a terra (cifrato in consolle)
    Vec2 plp;                           // punto di lancio pianificato
    Vec2 landfall;                      // primo waypoint pianificato (prima mappa TERCOM)
    double tot_s = 0.0, finestra_s = 600.0;   // ora sul bersaglio e tolleranza
    double tratta_terra_m = 0.0;
    double mezza_mappa_m = 2.5 * MIGLIO;      // H, STIMA
};
enum class ValiditaTlam : std::uint8_t { valida, ripianifica, non_valida, fuori_finestra };
const char* nome_validita(ValiditaTlam v);

struct StatoTlam {
    ValiditaTlam validita = ValiditaTlam::non_valida;
    double distanza_plp_m = 0.0;
    double ripianifica_s = 0.0;         // il ricalcolo della tratta sul mare, se ripianifica
    double p_landfall = 0.0;            // presa della prima mappa: 1 - exp(-H^2 / 2 sigma^2)
    double tempo_volo_s = 0.0;          // dal punto di lancio al bersaglio
    double ora_lancio_s = 0.0;          // per arrivare al tot
};
// Due chiamate, stessa funzione: con la stimata e la sigma dichiarata (pannello) e con la vera e l'errore vero (esito).
// `sigma_nav_m` e' l'errore radiale a 1 sigma della posizione del battello
StatoTlam valuta_tlam(const DatiMissile& m, const MissioneTlam& t, Vec2 pos, double sigma_nav_m, double t_ora);
struct EsitoTlam { bool arrivato = false; double t_arrivo = 0.0; Vec2 caduta; };   // caduta: dove finisce se si perde
EsitoTlam esito_tlam(const DatiMissile& m, const MissioneTlam& t, const StatoTlam& vero, double t_lancio,
                     std::uint64_t seme);

// costanti TLAM (STIME DI GIOCO)
inline constexpr double TLAM_VALIDA_M = 10.0 * MIGLIO, TLAM_RIPIANIFICA_M = 40.0 * MIGLIO;
inline constexpr double TLAM_RIPIANIFICA_S = 600.0;

}  // namespace m688
