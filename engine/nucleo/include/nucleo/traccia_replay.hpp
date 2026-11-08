#pragma once
// Traccia campionata per il replay completo (roadmap 16; proposta approvata il 26/10/2026, nota condivisa
// replay-traccia-proposta.md): una registrazione leggera scritta mentre si gioca, per scorrere la timeline senza
// risimulare. Un fotogramma ogni 5 s (2 s per aerei e armi in acqua) e a ogni evento del registro M1: la verita' di
// tutte le unita', il quadro del giocatore (contatti, linee di rilevamento, soluzione TMA proposta), il sonar e la
// stazione.
//
// Formato del file (.m688tr), autodescritto per reggere gli aggiornamenti del motore:
// - "M688TR" e un'intestazione di testo: righe «chiave=valore» (versione, missione, seme, ripresa_da, metadati
//   dell'app) e lo schema di ogni tipo di record, «record <tag> <campo>:<tipo><passo> ...» (i interi, q quantizzati
//   col passo, a angoli col passo, s stringhe). Un lettore prende i campi per nome: quelli che non conosce li salta,
//   quelli che mancano restano al valore predefinito;
// - i fotogrammi, ciascuno preceduto dalla sua lunghezza (un fotogramma rovinato si salta): interi quantizzati in
//   varint zigzag, in differenza dall'ultimo record della stessa unita' o dello stesso contatto, con un fotogramma
//   chiave (valori interi) ogni 60. Le stringhe vanno in una tabella che cresce col file.
// Versione 2 (sandbox, docs/19 §4; nota condivisa sandbox-statistiche-contratto.md): la posizione stimata, l'ellisse,
// l'allerta e le emissioni di ogni unita' (record V, solo quando cambiano: la stima di 10 m, l'ellisse di 5 m), la TMA
// in corso e le armi a bordo (record A, quando cambiano), e i fatti
// dell'IA che il registro M1 non ha (record I: contatti di tutte le unita', eventi dei siluri). I lettori della versione 1 saltano i campi e il record nuovi; questo legge
// anche le tracce della versione 1 (i campi mancanti ai valori predefiniti).
// Il registratore legge soltanto quello che riceve: non tocca la missione ne' il generatore casuale. Il modulo e' puro.

#include <cstdint>
#include <filesystem>
#include <map>
#include <optional>
#include <string>
#include <vector>

#include "nucleo/tma.hpp"

namespace m688 {

// Una unita' all'istante del fotogramma (la verita')
struct StatoUnita {
    int id = -1;
    std::string classe, categoria;           // anagrafica: scritta una volta, quando l'unita' compare
    int flag = 0;                            // schieramento e tipo dell'originale (+06)
    Vec2 pos;
    double quota = 0.0, rotta = 0.0, velocita_kn = 0.0;
    bool distrutta = false, persa = false, incagliata = false, sul_fondo = false;
    bool rimossa = false;                    // uscita dal gioco a quest'istante (l'ultimo campione)
    int alberi = 0;                          // flag54: 01h schiera filata, 02h periscopio, 04h antenna
    int lanciatore = -1, collegata = -1;     // armi: chi l'ha lanciata e il filo (-1 tagliato o senza)
    // versione 2 (nelle tracce della 1: la posizione vera, 0, -1, 0)
    Vec2 pos_stimata;                        // Missione::posizione_stimata
    double sigma_nav_m = 0.0;                // semiasse maggiore dell'ellisse della navigazione a 1 sigma (0 senza)
    double sigma_min_m = 0.0;                // semiasse minore
    double orientamento_deg = 0.0;           // rilevamento vero dell'asse maggiore, 0-180
    int allerta = -1;                        // Missione::allerta_unita (-1 senza forze)
    int emissioni = 0;                       // EMESSO_*: in onda a quest'istante
    // la TMA in corso dell'IA (Missione::tma_ia) e le armi a bordo (Missione::armi_rimaste, tubi_carichi): -1 senza
    int tma_bersaglio = -1, tma_gambe = 0;
    bool tma_soluzione = false, tma_osservabile = false;   // osservabile: la distanza e' determinata (StimaTma)
    Vec2 tma_pos;                            // la stima della soluzione: posizione, rotta (gradi), velocita' (kn)
    double tma_rotta = 0.0, tma_velocita_kn = 0.0;
    int armi_rimaste = -1, tubi_carichi = -1;
    static constexpr int EMESSO_PING = 1, EMESSO_RADAR = 2, EMESSO_RADIO = 4;
};

// Un fatto dell'IA o della verita' che il registro M1 non ha (record I), coi campi generici del contratto:
//   contatto_nuovo, contatto_solido, contatto_perso: unita' = osservatore, altra = bersaglio, s = sensore,
//     a = distanza vera (m), b = eccesso di segnale (dB, 0 senza acustica); il perso al momento in cui e' sparito
//   siluro: unita' = l'arma, altra = il lanciatore, n = la sorgente (aggancio, esca...), a, b = posizione, c = quota,
//     s = attivazione, aggancio, perdita, esca_sedotta, esca_rifiutata, filo_rotto, spento, esaurito, esplosione, ping
// Gli altri fatti (scoperte, classificazioni, allerta, reazioni, TMA, lanci, colpi, esche, evasioni, ritirate,
// messaggi, punti nave, INCSEA) sono gia' nel registro, cioe' nei record E (Traccia::registro)
struct EventoIa {
    double t = 0.0;
    std::string tipo;
    int unita = -1, altra = -1;
    int n = 0, m = 0;
    double a = 0.0, b = 0.0, c = 0.0, d = 0.0;
    std::string s;
};

// Un intervento del «vetrino di Dio» (docs/28, D1; record X; il tipo e' missioni/interventi.hpp del runtime): una riga
// quando e' applicato, finito o fallito. Il replay del vetrino rifa' la partita dagli ordini piu' questi: `t` e la durata
// col passo di 1 ms (la missione va a 1/9 s), il punto e il raggio di 10 cm. `chiavi`: i parametri e i testi in
// «nome=valore;...» (codifica_chiavi e leggi_chiavi, con ';', '=' e '\' protetti)
struct RecordIntervento {
    int id = -1;
    double t = 0.0;                          // dell'applicazione o della fine
    int fase = 0;                            // FASE_APPLICATO, FASE_FINITO, FASE_FALLITO
    std::string tipo;
    int unita = -1;
    bool ha_punto = false;
    Vec2 punto;
    double raggio_m = 0.0, durata_s = 0.0;
    std::string chiavi;
    std::string riga_it, riga_en;
    static constexpr int FASE_APPLICATO = 1, FASE_FINITO = 2, FASE_FALLITO = 3;
};
std::string codifica_chiavi(const std::map<std::string, std::string>& m);
std::map<std::string, std::string> leggi_chiavi(const std::string& s);

// Un contatto del giocatore (il quadro, quello che sapeva allora)
struct StatoContatto {
    int id = -1;
    char lettera = 0;
    double qualita = 0.0, durata = 0.0, rilevamento = 0.0;
    std::optional<double> specchio;          // la schiera: l'altro rilevamento dell'ambiguita'
    bool posizione_nota = false;
    Vec2 pos_stimata;
    double quota_stimata = 0.0;
    std::string sensore, classe_creduta;     // chi l'ha scoperto; la classificazione del giocatore ("" nessuna)
};

// Quello che sta nell'app: il sonar, la stazione, la soluzione TMA proposta (lo riempie l'app dopo ogni passo)
struct DatiApp {
    std::string modo_sonar;                  // "1989" | "moderno" | "" (classico)
    std::string sensore;                     // il sensore della cascata (bqq5_sfera, tb16...)
    double fascio = 0.0;                     // rilevamento vero del fascio del LOFAR
    int filtro = 0;                          // 0 passa banda, 1 passa alto
    int stazione = 0;                        // F2-F9
    bool silenzio = false;                   // silenzio a bordo
    int bersaglio = -1;                      // il bersaglio designato
    bool soluzione = false;                  // c'e' una soluzione TMA proposta
    SoluzioneTma proposta;
};

// Un evento del registro M1 col suo testo: i segnalibri della timeline
struct EventoTraccia {
    int id = -1;
    double tempo = 0.0;                      // s di missione
    std::string tipo;                        // nome_evento: "scoperta", "lancio_giocatore", ...
    std::string testo_it, testo_en;
    int interesse = 0;                       // 0..100, per scegliere i segnalibri
    bool noto = true;                        // il giocatore lo sapeva allora (falso: solo nella verita')
    int unita = -1;                          // l'unita' principale
    // per le statistiche (nucleo/statistiche): le tracce scritte prima del 26/10/2026 non li hanno
    int altra = -1;                          // il bersaglio di un lancio, il lanciatore di un colpo
    double distanza_m = 0.0;                 // dal giocatore all'unita' principale
    std::string classe, categoria, classe_arma;   // dell'unita' principale e dell'arma
    int lato = 0;                            // dell'unita' principale: 0 nessuno, 1 ostile, 2 neutrale, 3 amico
    bool giusta = true;                      // classificazioni
    int livello = 0;                         // classificazioni: 1 categoria, 2 classe
    double tempo_scoperta = -1.0;            // classificazioni: la prima scoperta del contatto
    int codice = 0;                          // il codice dell'evento del registro (411: del sonarista)
    double valore = 0.0;                     // il valore dell'evento del registro (classificazioni: 0 automatica)
    // versione 2 (statistiche della sandbox): l'arma (lanci, colpi, esche), la classe creduta (classificazioni) e dove
    int arma = -1;
    std::string classe_creduta;
    Vec2 pos;
};

// Un contatto di un'unita' qualsiasi (record K, la lettura «unita'» della sandbox): una riga quando nasce, cambia
// stato (solido, sensore, classe, posizione nota) o si sposta (al piu' ogni 15 s), e quando e' perso. Il quadro di
// un'unita' a un istante e' l'ultima riga di ogni suo bersaglio fino a li', senza i persi (Traccia::quadro_unita)
struct ContattoUnita {
    double t = 0.0;                          // s di missione della riga
    int osservatore = -1, bersaglio = -1;
    double qualita = 0.0, rilevamento = 0.0;
    double eta_s = 0.0;                      // da quanto non e' rinnovato (adesso - ultima scoperta)
    bool solido = false, posizione_nota = false, perso = false;
    Vec2 pos_stimata;
    std::string sensore, classe_creduta;
};

// Il quadro di una parte (record P, la lettura «parte» della sandbox; Missione::quadro_parte del runtime): una voce
// per bersaglio col dato migliore della parte. Si scrive tutto il quadro del lato quando cambia
struct VoceParte {
    int bersaglio = -1;                      // l'unita' vera (la parte non lo sa: per lo spettatore)
    bool posizione = false;                  // con la posizione stimata; se no un rilevamento da `origine`
    Vec2 pos, origine;
    double rilevamento = 0.0;
    double semiasse_maggiore_m = 0.0, semiasse_minore_m = 0.0, orientamento_deg = 0.0;   // a 1 sigma, 0 se ignota
    double t_dato = 0.0;                     // eta' = adesso - t_dato
    std::string sensore;                     // del contatto, "tma", "radio"
    int fonte = -1;
    std::string categoria, classe;           // credute
    double qualita = 0.0;
};
struct QuadroParte {
    double t = 0.0;
    int lato = 0;                            // 0x08, 0x10, 0
    std::vector<VoceParte> voci;
};

struct Fotogramma {
    double t = 0.0;
    int evento = -1;                         // l'ultimo evento nuovo del registro (-1 nessuno)
    std::vector<StatoUnita> unita;           // tutte quelle in gioco (il registratore sceglie quali scrivere)
    std::vector<StatoContatto> contatti;
    std::vector<Rilevamento> rilevamenti;    // della TMA: tutti (il registratore scrive solo i nuovi)
    int bersaglio_tma = -1;
    DatiApp app;
    std::vector<EventoTraccia> eventi;       // gli eventi del registro (il registratore scrive solo quelli nuovi)
    std::vector<EventoIa> eventi_ia;         // i fatti nuovi dall'ultimo fotogramma (RaccoltaTraccia): tutti scritti
    std::vector<ContattoUnita> contatti_unita;   // le righe nuove dei contatti di tutte le unita' (RaccoltaTraccia)
    std::vector<QuadroParte> quadri_parte;       // i quadri di parte cambiati (RaccoltaTraccia)
    std::vector<RecordIntervento> interventi;    // i nuovi del vetrino di Dio (fotogramma_da): tutti scritti
};

class RegistratoreTraccia {
public:
    static constexpr double PASSO_S = 5.0, PASSO_VELOCI_S = 2.0;   // aerei e armi in acqua
    // navi neutrali che il giocatore non ha a contatto (il traffico, blocco 1 del sonar): al piu' ogni 25 s (si scrive
    // quando il prossimo fotogramma lento, a PASSO_S con un quarto di margine, andrebbe oltre), sotto i 30 s oltre i
    // quali il replay non interpola; subito se cambia lo stato (colpita, distrutta, alberi)
    static constexpr double PASSO_NEUTRALI_S = 25.0;
    static constexpr int OGNI_CHIAVE = 60;                         // un fotogramma chiave ogni 60 (5 minuti)

    // `ripresa_da`: la partita ripresa da un salvataggio a T («Riprendi da qui»): il giocatore comanda di nuovo da li'
    explicit RegistratoreTraccia(std::string missione = {}, std::uint64_t seme = 0, double ripresa_da = -1.0);
    void metadato(const std::string& chiave, const std::string& valore);   // comandante, data reale, origine...
    // E' ora di un fotogramma: al tempo t (s di missione) con n eventi del registro finora
    bool serve(double t, std::size_t eventi) const;
    // Lo scrive in memoria: i veloci ogni 2 s, gli altri ogni 5 s o a un evento nuovo; il quadro, la TMA e l'app con
    // le unita' lente. `eventi`: quanti ne ha il registro adesso
    void aggiungi(const Fotogramma& f, std::size_t eventi);
    std::vector<std::uint8_t> dati() const;              // il file intero
    void salva(const std::filesystem::path& p) const;     // std::runtime_error se non si scrive
    std::size_t fotogrammi() const { return fotogrammi_; }
    // l'id dell'ultimo evento scritto (-1 nessuno): fotogramma_da puo' copiare solo quelli dopo
    int ultimo_evento() const { return ultimo_evento_; }

private:
    std::vector<std::uint8_t> corpo_;
    std::map<std::string, std::string> meta_;
    std::map<std::string, int> stringhe_;
    std::map<int, std::vector<std::int64_t>> ultimo_unita_, ultimo_contatto_;
    struct UltimoExtra {   // l'ultimo record V scritto per un'unita'
        Vec2 errore;
        double sigma = 0.0, sigma_min = 0.0, orientamento = 0.0;
        int allerta = -1, emissioni = 0;
    };
    std::map<int, UltimoExtra> ultimo_extra_;
    std::map<int, StatoUnita> ultima_tma_armi_;   // l'ultimo record A scritto per un'unita' (i soli campi di TMA e armi)
    std::map<int, bool> veloce_;
    std::map<int, double> scritta_;   // l'ultima volta che un'unita' e' stata scritta
    std::vector<std::int64_t> ultimo_app_;
    double prossimo_ = 0.0, prossimo_veloci_ = 0.0, ultima_tma_ = -1e18;
    std::size_t eventi_visti_ = 0, fotogrammi_ = 0;
    int ultimo_evento_ = -1;
    int stringa(std::vector<std::uint8_t>& b, const std::string& s);
};

// Lo stato interpolato a un istante
struct IstanteTraccia {
    double t = 0.0;
    std::vector<StatoUnita> unita;           // in gioco a t: posizione, quota e velocita' lineari, rotta sul cerchio
    std::vector<StatoContatto> contatti;     // il quadro dell'ultimo fotogramma <= t
    std::vector<Rilevamento> rilevamenti;    // le linee di rilevamento fino a t
    int bersaglio_tma = -1;
    DatiApp app;
};

struct Traccia {
    int versione = 0;
    std::string missione;
    std::uint64_t seme = 0;
    double ripresa_da = -1.0;
    std::map<std::string, std::string> metadati;
    std::map<int, std::vector<std::pair<double, StatoUnita>>> unita;   // per unita', i campioni in ordine di tempo
    struct Quadro {
        double t = 0.0;
        std::vector<StatoContatto> contatti;
        int bersaglio_tma = -1;
        DatiApp app;
    };
    std::vector<Quadro> quadri;
    std::vector<Rilevamento> rilevamenti;
    std::vector<std::pair<double, int>> eventi;   // (tempo, id dell'evento del registro): i salti della timeline
    std::vector<EventoTraccia> registro;          // gli eventi coi testi, in ordine d'id (vuoto nelle tracce vecchie)
    std::vector<EventoIa> eventi_ia;              // in ordine di tempo (vuoto nelle tracce della versione 1)
    std::vector<ContattoUnita> contatti_unita;    // le righe K in ordine di tempo (vuoto senza la raccolta)
    std::vector<QuadroParte> quadri_parte;        // i quadri di parte (record P) in ordine di tempo
    std::vector<RecordIntervento> interventi;     // del vetrino di Dio (record X), in ordine di tempo (vuoto se nessuno)
    double durata = 0.0;
    int fotogrammi_saltati = 0;                   // rovinati

    IstanteTraccia istante(double t) const;
    // quanti eventi_ia hanno tempo <= t: quelli fino a t sono eventi_ia[0, n) (il registro dal vivo, senza copie)
    std::size_t eventi_ia_fino(double t) const;
    // i contatti di un'unita' all'istante t (l'ultima riga di ogni bersaglio, senza i persi), con l'eta' riportata a t
    std::vector<ContattoUnita> quadro_unita(int osservatore, double t) const;
    // il quadro della parte `lato` all'istante t (l'ultimo scritto fino a li'); nullptr se non e' registrato
    const QuadroParte* quadro_parte(int lato, double t) const;
};
// I segnalibri della timeline: gli eventi coi testi, in ordine di tempo, con interesse almeno `interesse_min`; `noti`:
// solo quelli che il giocatore sapeva allora (il quadro), altrimenti anche quelli della verita'. Una traccia senza i
// testi (scritta prima del 26/10/2026) da' gli eventi col solo tempo e l'id
std::vector<EventoTraccia> segnalibri(const Traccia& t, int interesse_min = 0, bool noti = false);
Traccia leggi_traccia(const std::vector<std::uint8_t>& dati);    // std::runtime_error se non e' una traccia
// Aggiunge a una traccia in memoria un fotogramma appena prodotto (la vista dal vivo della sandbox, che non rilegge il
// file): quello che leggi_traccia ricava dal record di RegistratoreTraccia::aggiungi, senza la quantizzazione e con
// tutte le unita' del fotogramma (il registratore scrive le lente ogni 5 s). Le unita' sparite dall'ultimo fotogramma
// prendono il campione d'uscita (rimossa); il quadro c'e' se il fotogramma ha contatti, rilevamenti o app; registro
// ed eventi_ia solo i nuovi (id piu' alto, tempo dopo l'ultimo). I fotogrammi vanno dati in ordine di tempo
void aggiungi_fotogramma(Traccia& t, const Fotogramma& f);
Traccia carica_traccia(const std::filesystem::path& p);

// Cartelle: <cartella dei fascicoli>/../replay/<comandante>/ in carriera, .../replay/liberi/ fuori carriera (e per
// le riprese); nome <missione>_<data>.m688tr
std::filesystem::path cartella_replay(const std::string& comandante = {});
std::filesystem::path percorso_replay(const std::string& comandante, const std::string& missione, const std::string& data);
// Fuori carriera se ne tengono gli ultimi `tenere` (per data di modifica): restituisce quanti ne ha cancellati
int pulisci_replay_liberi(int tenere = 20);

}  // namespace m688
