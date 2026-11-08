#pragma once
// Periscopi dell'Enhanced (docs/14_periscopi.md, pezzo P1; dati e dottrina dal dossier docs/ricerca/20_periscopi.md,
// citato per paragrafo). Vale solo con la regola di simulazione: il classico resta un periscopio con due
// ingrandimenti, su e giu' a 20 ft (TipoOrdine::periscopio e flag54 & 2).
//
// Il modulo e' puro e deterministico:
// - il posto del periscopio (PostoPeriscopi): scelta, alzata e discesa coi loro secondi, elevazione, ingrandimento,
//   modo, secondi d'esposizione e storia delle esposizioni, tremolio e rischio di danno dalla velocita' e dal mare;
// - la visibilita' di una testa esposta per un osservatore (nave, aereo, periscopio nemico): portata visiva e
//   probabilita' per secondo;
// - la sezione radar della testa per nucleo/elettronica (TestaEsposta);
// - i periscopi dell'IA (PeriscopioIa): guardate di ~10 s ogni 3-12 minuti, l'avvisatore radar prima per i sovietici,
//   la TV МТ-70 fino a 50 m.
// Ogni estrazione casuale usa il Casuale& passato dalla missione: qui non se ne crea. Ingrandimento e modo della vista
// non cambiano la simulazione: stanno nello stato perche' la vista (grafica/periscopio, P3) li legge da li'.

#include <cstdint>
#include <map>
#include <optional>
#include <string>
#include <vector>

#include "nucleo/casuale.hpp"

namespace m688 {

// --- dati: data/motore/periscopi.toml (tools/pipeline/periscopi.py, pezzo P0) ---

enum class ModoVista : std::uint8_t { giorno, bassa_luminosita, intensificatore, tv, termico };
const char* nome_modo_vista(ModoVista m);
std::optional<ModoVista> modo_vista_da_nome(const std::string& s);

struct DatiPeriscopio {
    std::string id, nome;
    std::string ruolo = "attacco";              // attacco, ricerca, universale, tv
    std::string nazione = "usa";                // usa, uk, urss
    std::vector<double> ingrandimenti, campi_gradi;   // uno per ingrandimento
    double elevazione_min = -10.0, elevazione_max = 60.0;   // gradi della linea di mira
    std::string telemetro = "sdoppiato";        // sdoppiato (stadimetro a immagine sdoppiata) o reticolo
    double telemetro_altezza_min_m = 0.0, telemetro_altezza_max_m = 0.0;     // altezze impostabili sul quadrante
    double telemetro_distanza_min_m = 0.0, telemetro_distanza_max_m = 0.0;   // distanze leggibili
    double telemetro_angolo_max_gradi = 0.0;    // sdoppiamento massimo
    std::vector<ModoVista> modi;                // il primo e' il giorno
    bool tv = false, foto = false, esm = false, stabilizzato = false;
    double testa_m = 0.06;                      // diametro della testa
    double sezione_m2 = 0.2;                    // sezione radar in banda X con 1 m esposto (STIMA, §5 e §8.1)
    double piuma = 0.5;                         // fattore della piuma (1 la testa di ricerca)
    double alzata_s = 6.0, discesa_s = 5.0;
    // velocita' col periscopio alzato: sotto v_libera niente tremolio, 0,5 a v_tremolio, 1 (immagine inservibile) a
    // v_inservibile; oltre v_danno il rischio di danno
    double v_libera_kn = 6.0, v_tremolio_kn = 8.0, v_inservibile_kn = 10.0, v_danno_kn = 15.0;
    std::string reticolo = "americano";         // americano, sovietico, britannico
    std::vector<double> reticolo_gradi, reticolo_fine_gradi;   // passi grandi e piccoli, uno per ingrandimento
    std::vector<double> fermi_gradi;            // fermi a molla dell'elevazione (Type 2: 14 e 44)
    std::vector<std::string> filtri;
    // l'ottica per i punti luminosi di notte (luci::guadagno_ottico di nucleo/portata_luci): l'apertura utile e la
    // trasmissione dell'ottica lunga (STIMA)
    double obiettivo_mm = 30.0, trasmissione = 0.3;
    std::vector<std::string> stime;             // i campi STIMA (da tarare, P5)
};

// Per classe di sottomarino: i periscopi e la dottrina delle guardate (§7, §8.3)
struct DatiClassePeriscopi {
    std::vector<std::string> periscopi;         // il primo e' quello d'attacco (o l'unico)
    double quota_m = 18.3;                      // quota periscopica alla chiglia
    double esposto_m = 1.5;                     // testa sull'acqua calma a quota periscopica, periscopio tutto su
    double esposizione_s = 10.0;                // durata di una guardata dell'IA
    double intervallo_min_s = 180.0, intervallo_max_s = 720.0;   // fra due guardate dell'IA
    bool avvisatore_prima = false;              // sovietici: l'antenna dell'avvisatore radar prima del periscopio
    double avvisatore_quota_m = 0.0;            // quota a cui esce l'avvisatore (16-18 m)
    std::string tv;                             // complesso TV (МТ-70) e sua quota massima
    double tv_quota_max_m = 0.0;
    bool telecamera_verticale = false;          // Alfa: la telecamera della vela
};

struct CatalogoPeriscopi {
    std::map<std::string, DatiPeriscopio> periscopi;
    std::map<std::string, DatiClassePeriscopi> classi;
    const DatiPeriscopio* periscopio(const std::string& id) const;
    const DatiClassePeriscopi* classe(const std::string& id) const;
    std::vector<const DatiPeriscopio*> di_classe(const std::string& classe) const;   // nell'ordine della classe
};
CatalogoPeriscopi carica_periscopi(const std::string& percorso);   // std::runtime_error se manca o e' incompleto

// --- geometria della testa ---

inline constexpr double CORSA_PERISCOPIO_M = 6.0;     // corsa dell'albero dall'alloggio alla fine (STIMA)
inline constexpr double ESPOSIZIONE_LUNGA_S = 30.0;   // «due o piu' esposizioni brevi sotto i 30 s» [USF25]
inline constexpr double FINESTRA_M = 0.12;            // mezza finestra della testa: come la vista (grafica/oculare)

// Altezza significativa del mare formato per lo stato del mare (Douglas, centro della classe): la stessa va data alla
// vista (VistaPeriscopio::altezza_onde_m) perche' l'onda sulla finestra coincida (nucleo/onde.hpp)
double altezza_significativa_m(int stato_mare);   // = centro_douglas (nucleo/meteo): la tabella unica
// Errore di quota della testa a quota periscopica: la tabella del dossier 21 §7.3 (0,6 m fino a 3, 1,5 a 4, 2,5 a 5, 4 a 6,
// 6 oltre) per 0,4, il comandante che tiene la quota (taratura M6)
double sigma_quota_m(int stato_mare);

// Altezza della testa sull'acqua calma: quella della classe a quota periscopica, piu' quanto si e' sopra la quota
// periscopica, meno la corsa che manca (alzata 0-1); 0 se la testa e' sott'acqua
double altezza_testa_m(const DatiClassePeriscopi& c, double quota_m, double alzata);

// Tremolio della vista (0-1) dalla velocita' e dal mare; dimezzato dalla stabilizzazione
double tremolio(const DatiPeriscopio& p, double velocita_kn, int stato_mare);
// Probabilita' per secondo di danno al periscopio alzato oltre v_danno (STIMA: 2% al secondo a v_danno + 5 kn)
double probabilita_danno_s(const DatiPeriscopio& p, double velocita_kn);
// Fattore della piuma (0 ferma; 1 la testa di ricerca a 10 nodi): cresce con la velocita' oltre ~2 nodi
double piuma(const DatiPeriscopio& p, double velocita_kn);

// --- sezione radar per nucleo/elettronica ---

// La testa vera al posto della sezione unica dell'esposizione (elettronica: TestaEsposta)
struct SagomaTesta {
    double sezione_m2 = 0.0;                    // efficace: sezione per 1 m esposto per l'altezza al quadrato
    double altezza_m = 0.0;
    double piuma = 0.0;
};
SagomaTesta sagoma_testa(const DatiPeriscopio& p, double altezza_m, double velocita_kn);

// --- visibilita' della testa per un osservatore ---

enum class TipoOsservatore : std::uint8_t { nave, aereo, periscopio };

struct Osservatore {
    TipoOsservatore tipo = TipoOsservatore::nave;
    double altezza_occhio_m = 20.0;             // vedette sul ponte, quota dell'aereo, testa del periscopio nemico
    ModoVista notte = ModoVista::giorno;        // il miglior sensore notturno (giorno: solo l'occhio)
    bool allertato = false;                     // vedette avvisate (contatto, allarme)
};

struct CondizioniVista {
    int stato_mare = 2;
    double luce = 1.0;                          // 0 notte, 1 giorno (nucleo/sole)
    double visibilita_m = 30000.0;              // portata meteorologica
    double riflesso = 0.0;                      // 0-1: il sole basso sulla lente puntata verso l'osservatore
    double nubi_ottavi = 4.0;                   // di notte: stelle col sereno, buio col coperto (dossier 21 §6.2)
    bool nebbia_radiazione = false;             // il termico vi vede 4 volte l'occhio (fino a 4 volte la visibilita')
};

// Portata di riferimento dell'osservatore contro la testa (STIMA, §5 e §8.3): a questa distanza il tasso di scoperta e'
// la meta' del massimo: testa e piuma
// danno la dimensione equivalente; mare, notte e sensori notturni la riducono; il riflesso del sole la allunga;
// mai oltre l'orizzonte geometrico e la visibilita'
double portata_visiva_m(const DatiPeriscopio& p, const SagomaTesta& testa, const Osservatore& o, const CondizioniVista& c);
// Probabilita' che l'osservatore veda la testa nei dt secondi, a distanza `distanza_m`
double probabilita_vista(const DatiPeriscopio& p, const SagomaTesta& testa, const Osservatore& o,
                         const CondizioniVista& c, double distanza_m, double dt);

// --- il posto del periscopio del giocatore ---

struct EsposizionePeriscopio {
    std::string periscopio;
    double inizio = 0.0, fine = -1.0;           // s di missione; fine < 0 mentre e' in corso
    double durata() const { return fine >= inizio ? fine - inizio : 0.0; }
};

// Il contratto con la vista (P3) e la stazione (P4)
struct StatoPeriscopio {
    std::string scelto;                         // id in periscopi.toml
    double alzata = 0.0;                        // 0 giu', 1 su
    double elevazione = 0.0;                    // gradi della linea di mira
    int ingrandimento = 0;                      // indice in DatiPeriscopio::ingrandimenti
    ModoVista modo = ModoVista::giorno;
    double esposto_s = 0.0;                     // secondi della guardata in corso (0 con la testa sott'acqua)
    double tremolio = 0.0;                      // 0-1 per la vista, gia' ridotto dalla stabilizzazione
    // della vista (P3), non della simulazione: li cambia la stazione
    int filtro = -1;                            // indice in DatiPeriscopio::filtri, -1 nessuno
    double sdoppiamento_gradi = 0.0;            // stadimetro: spostamento verticale dell'immagine fantasma (0 spento)
    bool telecamera_verticale = false;          // Alfa: la telecamera della vela al posto del periscopio
    // in piu' del contratto di docs/14
    bool comandato_su = false;                  // l'ordine: su (sale o resta su) o giu'
    bool su = false;                            // a fine corsa: il flag classico del periscopio alzato
    bool testa_fuori = false;                   // la testa e' sull'acqua calma (espone e vede)
    bool finestra_coperta = false;              // un'onda (o l'errore di quota) copre la finestra al tempo del passo:
                                                // non si vede, e radar e vedette in quell'istante non vedono la testa
    double altezza_vera_m = 0.0;                // con l'errore di quota del mare (la sezione radar e la vista)
    double altezza_testa_m = 0.0;
    bool danneggiato = false;                   // non sale piu' (resta giu')
};

class PostoPeriscopi {
public:
    PostoPeriscopi();                           // un Type 2 di riserva
    // I periscopi della classe; senza dati (classe sconosciuta) un Type 2 di riserva
    PostoPeriscopi(const CatalogoPeriscopi& cat, const std::string& classe);

    // Scelta del periscopio (indice nella classe): se quello alzato e' un altro, scende e poi sale il nuovo
    bool scegli(int indice);
    void comanda(bool su) { stato_.comandato_su = su; }
    void elevazione(double gradi);              // limitata al modello (e al cielo che la testa vede)
    void ingrandimento(int indice);
    void modo(ModoVista m);                     // solo i modi del modello
    void filtro(int indice);                    // -1 o un indice valido dei filtri del modello
    void sdoppiamento(double gradi);            // telemetro sdoppiato, o angolo letto sul reticolo; fino all'angolo massimo
    void telecamera_verticale(bool si);         // solo con la telecamera della classe
    void danneggia() { stato_.danneggiato = true; stato_.comandato_su = false; }
    void ripara() { stato_.danneggiato = false; }
    // La testa di un periscopio colpita dall'artiglieria (docs/16 G3, dossier 22 §5.3): degradata (l'immagine), bloccata
    // (non sale e non scende piu', resta dov'era) o cieca; con `allaga` la canna aperta imbarca acqua finche' il
    // periscopio non e' giu' (abbassato e chiuso)
    enum class DannoTesta : std::uint8_t { nessuno, degradata, bloccata, cieca };
    void danneggia_testa(int indice, DannoTesta d, bool allaga = false);
    DannoTesta danno_testa(int indice) const;
    bool bloccato() const { return danno_testa(indice_) == DannoTesta::bloccata; }   // quello scelto
    bool canna_aperta() const;   // quello scelto cieco con la canna che si allaga, e non ancora tutto giu'
    int indice_ruolo(const std::string& ruolo) const;   // il primo periscopio col ruolo (attacco, ricerca), -1 se manca

    // Un passo: corsa dell'albero, esposizione, tremolio, onda sulla finestra. `quota_m` alla chiglia. Ritorna vero se la
    // velocita' ha danneggiato il periscopio in questo passo (estrazione da `caso` solo oltre v_danno con l'albero
    // alzato). `hs_m` la Hs vera del meteo (< 0: il centro della classe); col mare la testa sale e scende con l'errore
    // di quota (AR(1) di 20 s, una gaussiana da `caso` per passo con la testa fuori), che non spezza la guardata
    bool avanza(double t, double dt, double quota_m, double velocita_kn, int stato_mare, Casuale& caso, double hs_m = -1.0);

    const StatoPeriscopio& stato() const { return stato_; }
    const DatiPeriscopio* dati() const { return &periscopi_[static_cast<std::size_t>(indice_)]; }   // quello scelto
    const std::vector<DatiPeriscopio>& periscopi() const { return periscopi_; }   // copie: il posto si copia
    int indice() const { return indice_; }
    const DatiClassePeriscopi& classe() const { return classe_; }
    SagomaTesta sagoma(double velocita_kn) const;           // della testa alzata (zero se sott'acqua)
    const std::vector<EsposizionePeriscopio>& esposizioni() const { return esposizioni_; }
    double esposizione_totale_s() const;
    // Esposizioni finite in questo passo (per il registro: esposizione_lunga oltre ESPOSIZIONE_LUNGA_S)
    std::vector<EsposizionePeriscopio> finite();

private:
    std::vector<DatiPeriscopio> periscopi_;
    DatiClassePeriscopi classe_;
    int indice_ = 0, prossimo_ = -1;            // prossimo_: scelto mentre l'altro era su
    std::vector<DannoTesta> danni_;             // per periscopio (vuoto: tutti intatti)
    std::vector<bool> allaga_;
    StatoPeriscopio stato_;
    std::vector<EsposizionePeriscopio> esposizioni_, finite_;
    double errore_quota_m_ = 0.0;
};

// --- i periscopi dell'IA ---

enum class FaseGuardata : std::uint8_t { sotto, avvisatore, esposto, tv };

class PeriscopioIa {
public:
    PeriscopioIa() = default;
    // La prima guardata dopo un intervallo estratto da `caso`
    PeriscopioIa(const CatalogoPeriscopi& cat, const std::string& classe, double t, Casuale& caso);

    // `allarme`: siluro in acqua, attaccata o cacciata (niente guardate); `radar_sentito`: l'avvisatore ha sentito un
    // radar (la guardata si rimanda); `luce` per la TV. Estrae da `caso` solo all'inizio d'ogni intervallo.
    void avanza(double t, double quota_m, double velocita_kn, bool allarme, bool radar_sentito, double luce, Casuale& caso);

    FaseGuardata fase() const { return fase_; }
    bool esposto() const { return fase_ == FaseGuardata::esposto; }
    bool avvisatore_su() const { return fase_ == FaseGuardata::avvisatore || (fase_ == FaseGuardata::esposto && classe_.avvisatore_prima); }
    const DatiPeriscopio* modello() const { return modello_; }   // quello della guardata (o della TV)
    double altezza_testa_m(double quota_m) const;
    double prossima() const { return prossima_; }
    int guardate() const { return guardate_; }
    // Chi guarda (dalla testa o dalla TV) come osservatore: altezza; di notte il miglior sensore dei periscopi della classe
    Osservatore osservatore(double quota_m) const;
    // La TV МТ-70 vede la superficie senza esporsi: portata ridotta (STIMA 0,3 di una guardata)
    double fattore_tv() const { return fase_ == FaseGuardata::tv ? 0.3 : 1.0; }

private:
    const DatiPeriscopio* modello_ = nullptr;
    const DatiPeriscopio* tv_ = nullptr;
    DatiClassePeriscopi classe_;
    ModoVista notte_ = ModoVista::giorno;
    FaseGuardata fase_ = FaseGuardata::sotto;
    double prossima_ = 0.0, fine_ = 0.0;
    int guardate_ = 0;
    void programma(double t, Casuale& caso);
};

}  // namespace m688
