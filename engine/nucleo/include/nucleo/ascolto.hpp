#pragma once
// Ascolto sintetico del fascio (S5d; dossier docs/ricerca/09_sonar_sistemi_e_interfacce.md §3.7): il suono che il
// sonarista sente in cuffia, in campioni PCM. Rumore colorato come lo spettro ricevuto nel fascio (rumore del mare e
// proprio meno il guadagno d'antenna, piu' il continuo delle sorgenti), le righe come sinusoidi alla frequenza
// ricevuta (Doppler), il battito dell'elica (modulazione d'ampiezza alla frequenza di pala del DEMON) per chi cava, e
// il filtro passa banda del pannello. Opzioni: le righe sotto 50 Hz traslate di ottave finche' si sentono, e
// l'ascolto accelerato x4 / x16 (NOAA PMEL) che porta su le frequenze basse. Continuo e righe con le frange
// d'interferenza dei cammini (nucleo/interferenza, blocco 1 del sonar): le righe svaniscono e tornano con la distanza.
//
// Il modulo e' puro e deterministico (le fasi del rumore vengono dal seme). Il gioco oggi non ha l'uscita audio: qui
// ci sono i campioni e un WAV per ascoltarli.

#include <cstdint>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "nucleo/analisi_acustica.hpp"
#include "nucleo/campioni.hpp"
#include "nucleo/casuale.hpp"

namespace m688 {

struct ParametriAscolto {
    int campionamento_hz = 22050;
    double durata_s = 2.0;
    double f_min_hz = 20.0, f_max_hz = 5000.0;   // il passa banda (in uscita)
    bool ottava_su = false;                      // righe sotto 50 Hz su di ottave
    double accelerazione = 1.0;                  // 4 o 16: il segmento suona k volte piu' acuto
    double rumore_dbfs = -24.0;                  // il fondo (rumore senza sorgenti) in uscita
    std::uint64_t seme = 1;
};

struct Suono {
    std::vector<std::int16_t> campioni;
    int campionamento_hz = 22050;
    double guadagno_db = 0.0;                    // quanto e' stato abbassato per non saturare (0: niente)
};

Suono sintetizza(const std::vector<Sorgente>& sorgenti, const Ricevitore& r, const Ambiente& amb, double rilevamento_fascio,
                 const ParametriAscolto& p = {});

// Il motore di sintesi comune alla cuffia (R2) e all'ambiente di bordo (R3): rumore colorato a fotogrammi sovrapposti
// per meta' sotto la radice della finestra di Hann (potenza costante, passaggi morbidi fra uno spettro e il
// successivo), righe a fase continua che passano da un'ampiezza all'altra a rampa, un battito (modulazione d'ampiezza
// del rumore) a fase continua, la scala del fondo che segue per fotogrammi e un limitatore per campione. Tutto lo
// stato evolve per campione o per fotogramma, quindi l'uscita non dipende da come si spezzano le richieste.
class GeneratoreSpettrale {
public:
    explicit GeneratoreSpettrale(int campionamento_hz = 22050, std::uint64_t seme = 1, std::size_t fotogramma = 2048);
    int campionamento_hz() const { return fs_; }
    std::size_t bin() const { return n_ / 2; }                 // bin del rumore (passo fs / fotogramma)
    double df_hz() const { return static_cast<double>(fs_) / static_cast<double>(n_); }
    // il rumore: la densita' spettrale per bin (unita' d'uscita al quadrato per Hz, il bin 0 e' la continua) e la scala
    // a cui tendere (moltiplica rumore e righe; i fotogrammi la seguono)
    void imposta_rumore(const std::vector<double>& densita, double scala);
    // le righe di questo aggiornamento: chiave, frequenza d'uscita, ampiezza (unita' d'uscita prima della scala); quelle
    // non ripetute si spengono a rampa
    struct RigaSintesi { std::pair<int, long> chiave; double hz = 0.0, ampiezza = 0.0; };
    void imposta_righe(const std::vector<RigaSintesi>& righe);
    void imposta_battito(double hz, double profondita);       // profondita' 0 = niente
    // `aggiunta` (unita' d'uscita, almeno n campioni): sommata dopo la scala e prima del limitatore (le tessiture, R5)
    std::vector<float> genera(std::size_t n, const std::vector<double>* aggiunta = nullptr);
    void azzera();

private:
    struct Riga { double fase = 0.0, ampiezza = 0.0, bersaglio = 0.0, hz = 0.0; };
    void fotogramma();

    int fs_ = 22050;
    std::uint64_t seme_ = 1;
    Casuale caso_;
    std::size_t n_ = 2048;
    std::vector<double> ampiezze_;
    double scala_bersaglio_ = 0.0, scala_ = 0.0, limite_ = 1.0;
    std::vector<float> coda_, scale_coda_, sovrapposto_;
    std::map<std::pair<int, long>, Riga> righe_;
    double battito_hz_ = 0.0, battito_ = 0.0, battito_bersaglio_ = 0.0, fase_battito_ = 0.0;
};

// La cuffia del sonarista in tempo reale (pezzo R2): lo stesso suono di sintetizza, generato a blocchi continui per la
// stazione sonar col GeneratoreSpettrale. A ogni lettura della scena (sorgenti, ricevitore, fascio puntato) si aggiornano
// lo spettro del rumore (spettro_continuo: le righe sono a parte), le righe ricevute e il battito d'elica del DEMON; il
// fondo si scala a rumore_dbfs. L'uscita non dipende da come si spezzano i passi: a scena ferma, 20 passi da 0,1 s danno
// gli stessi campioni di un passo da 2 s. La scena si rilegge ogni 0,25 s d'audio (il costo: lo spettro del fascio e le
// righe), e i cambi passano a rampa.
// `dt_s` e' il tempo vero da riempire (non quello di gioco: con la compressione del tempo la stazione passa il tempo
// reale, o tace la cuffia).
// I transitori impulsivi della libreria (meteo, armi e scafi: fulmini, lanci, esplosioni, cedimenti; la prima
// componente un impulso di al piu' 0,5 s) non passano dallo spettro, che li spalmerebbe sulla rilettura: al loro
// inizio diventano uno schiocco a campione esatto, rumore nella banda dell'impulso con l'attacco di 1,5 ms e la
// caduta della sua durata, al S/N che il fascio puntato gli da' durante l'impulso (snr_impulso_db), e piu' scuro con
// la distanza come vuole l'assorbimento del modello. Il fulmine ha la sua forma (21e §5.2): caduta di 40 ms, coda di
// multipercorso e schiocco acuto sotto 3 km.
// Con la libreria di campioni (R5, imposta_campioni) le sorgenti che hanno un campione (la voce della libreria o la
// classe della firma) suonano con la tessitura registrata invece del rumore colorato: la tessitura sbiancata, letta al
// tempo vero (le sorgenti della libreria alla posizione del tempo di gioco, cosi' la stessa partita suona uguale),
// ricampionata col Doppler (e, per le navi, al battito d'elica del modello), e ricolorata a fotogrammi con lo spettro
// che il modello da' a quella sorgente nel fascio (spettro_continuo della sola sorgente). Fuori dalla banda valida del
// campione resta il rumore del modello; le righe sono sempre quelle del modello. Senza libreria l'uscita e' quella di
// prima.
class CuffiaSonar {
public:
    explicit CuffiaSonar(const ParametriAscolto& p = {});
    void imposta(const ParametriAscolto& p);      // filtri, ottava e accelerazione cambiano dal prossimo passo
    void imposta_campioni(const LibreriaCampioni* libreria) { libreria_ = libreria; lettori_.clear(); }
    const ParametriAscolto& parametri() const { return p_; }
    std::vector<float> passo(const std::vector<Sorgente>& sorgenti, const Ricevitore& r, const Ambiente& amb,
                             double rilevamento_fascio, double dt_s);
    void azzera();                                 // silenzio e stato iniziale (cambio di scena)

private:
    void prepara(const std::vector<Sorgente>& sorgenti, const Ricevitore& r, const Ambiente& amb, double rilevamento_fascio);
    struct Schiocco {
        std::uint64_t inizio = 0;                  // campione
        double ampiezza = 0.0;                     // rms del colpo, unita' d'uscita
        double taglio_hz = 400.0, scala_t = 1.0;   // passa basso della distanza; durate / accelerazione
        double basso_hz = 40.0, tau_s = 0.04;      // passa alto (l'inizio della banda) e caduta
        double coda = 0.0;                         // il fulmine: la coda di multipercorso
        bool acuto = false;                        // il fulmine: lo schiocco sotto 3 km
        Casuale caso{1};
        double f1 = 0.0, f2 = 0.0, f3 = 0.0, norma = 1.0;
    };
    double campione_schiocco(Schiocco& s, std::uint64_t k) const;
    // una tessitura in lettura (R5): posizione nel file, passo per campione d'uscita, guadagni per bin del fotogramma
    struct Lettore {
        const Campione* c = nullptr;
        double pos = 0.0, passo = 1.0;
        std::vector<double> guadagni;
        std::vector<float> coda, sovrapposto;
        bool visto = false;
    };
    void fotogramma_lettore(Lettore& l);

    ParametriAscolto p_;
    GeneratoreSpettrale g_;
    double resto_ = 0.0;                           // frazione di campione rimasta dai passi precedenti
    double dal_aggiornamento_ = 0.0;               // campioni dall'ultima lettura della scena
    bool pronto_ = false;
    std::uint64_t campione_ = 0;                   // campioni prodotti
    std::vector<Schiocco> schiocchi_;
    std::map<int, bool> attivi_;                   // fulmini gia' sentiti (sorgente -> attivo)
    std::uint64_t prossimo_seme_ = 1;
    const LibreriaCampioni* libreria_ = nullptr;
    std::map<int, Lettore> lettori_;               // sorgente -> tessitura
};

// vero per i transitori che la cuffia suona a campione esatto (meteo, armi e scafi; la prima componente un impulso di al
// piu' 0,5 s, processo singolo; non i ping, che la cuffia non sente sopra 5 kHz)
bool impulsivo(const Sorgente& s);

// WAV PCM a 16 bit, mono; false se il file non si scrive
bool scrivi_wav(const std::string& percorso, const Suono& s);

// ampiezza di una frequenza nei campioni (algoritmo di Goertzel, normalizzata a 1 per un seno di ampiezza piena): prove
double ampiezza_a(const Suono& s, double hz);

}  // namespace m688
