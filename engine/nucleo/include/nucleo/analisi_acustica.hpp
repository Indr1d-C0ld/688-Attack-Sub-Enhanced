#pragma once
// Dati della stazione sonar completa (Fase 3, S3; roadmap punti 19 e 24): righe emesse da una sorgente, spettro LOFAR
// di un fascio, cascata a banda larga per rilevamento (BTR), DEMON e analizzatore di firma con la banca dati delle
// classi. Dossier 02 §1.2, §2.1-2.2, §2.7.
//
// Il modulo e' puro, come nucleo/acustica.hpp su cui poggia: calcola un istante. Le storie (righe della cascata e del
// LOFAR nel tempo) le tiene la stazione. Unita': Hz, dB, gradi bussola, nodi.

#include <optional>
#include <string>
#include <vector>

#include "nucleo/acustica.hpp"
#include "nucleo/casuale.hpp"

namespace m688 {

// --- righe emesse ---
struct RigaEmessa {
    double hz = 0.0, db = 0.0;   // livello assoluto (dB re 1 uPa @1 m)
    TipoRiga tipo = TipoRiga::rete;
};
// Le righe dei macchinari della firma presenti a quella velocita' (i diesel dei battelli a snorkel solo a snorkel),
// piu' l'elica: asse a giri/60 Hz e pala a pale x giri/60 con la seconda armonica, piu' forti con la cavitazione.
// Il battello posato abbassa i macchinari di 10 dB.
std::vector<RigaEmessa> righe_emesse(const Sorgente& s);
double frequenza_asse_hz(const Sorgente& s);   // 0 senza elica o da fermo

// --- spettro a banda stretta (LOFAR) ---
struct Spettro {
    double f0_hz = 0.0, df_hz = 1.0;
    std::vector<float> snr_db;   // (segnale + rumore) / rumore per bin: 0 dB = solo rumore
    double frequenza(std::size_t i) const { return f0_hz + (static_cast<double>(i) + 0.5) * df_hz; }
};
// Lo spettro del fascio puntato a `rilevamento` (vero): per ogni sorgente il continuo (SL - TL + 10 log b) e le righe
// (L - TL, spostate per Doppler), pesati con la risposta del fascio (con lo specchio per la schiera), contro il rumore
// nel bin (NL + 10 log b - DI). f_min e f_max fanno da filtri passa-alto e passa-basso.
// `coerente` (blocco 1 del sonar, docs/20 §4): le frange d'interferenza dei cammini diretti (nucleo/interferenza) sul
// continuo e sulle righe, sopra la perdita del livello A (il livello medio resta); la distanza cambia per la velocita'
// radiale nel tempo 1 / bin. Con false lo spettro e' quello di prima, bit per bit. Lo stesso per le altre tre
Spettro spettro_fascio(const std::vector<Sorgente>& sorgenti, const Ricevitore& r, const Ambiente& amb,
                       double rilevamento, double f_min_hz, double f_max_hz, double bin_hz, bool coerente = false);

// Verniero (S5d; dossier 09 §2.2): lo spettro di zoom di `larghezza_hz` attorno a `centro_hz` con un bin fine
// (0,01-0,02 Hz, cioe' 50-100 s d'integrazione); le righe si spargono sui bin vicini con la risposta di Hann, cosi' il
// picco si stima fra i bin (misura_riga, nucleo/misure_sonar.hpp)
Spettro spettro_verniero(const std::vector<Sorgente>& sorgenti, const Ricevitore& r, const Ambiente& amb,
                         double rilevamento_fascio, double centro_hz, double larghezza_hz = 4.0, double bin_hz = 0.01,
                         bool coerente = false);
// Lo spettro del fascio senza le righe (solo il continuo delle sorgenti e il rumore): per colorare il rumore
// dell'ascolto, che le righe le aggiunge come sinusoidi
Spettro spettro_continuo(const std::vector<Sorgente>& sorgenti, const Ricevitore& r, const Ambiente& amb,
                         double rilevamento_fascio, double f_min_hz, double f_max_hz, double bin_hz, bool coerente = false);
// S/N spettrale (dB per Hz) di una sorgente della libreria durante il suo impulso, senza la media sulla finestra: lo
// schiocco di un transitorio (un colpo di fulmine) nel fascio puntato, per la cuffia
double snr_impulso_db(const Sorgente& s, const Ricevitore& r, const Ambiente& amb, double rilevamento_fascio, double f);
// Le righe che arrivano nel fascio fra f_min e f_max, con la frequenza ricevuta (Doppler) e il S/N nel bin: per
// l'ascolto sintetico e per le prove
struct RigaRicevuta {
    double hz = 0.0, snr_db = 0.0;
    int sorgente = -1;
    TipoRiga tipo = TipoRiga::rete;
    double hz_emessa = 0.0;
};
std::vector<RigaRicevuta> righe_ricevute(const std::vector<Sorgente>& sorgenti, const Ricevitore& r, const Ambiente& amb,
                                         double rilevamento_fascio, double f_min_hz, double f_max_hz, double bin_hz,
                                         bool coerente = false);

// --- cascata a banda larga per rilevamento (BTR) ---
struct RigaCascata {
    double rilevamento0 = 0.0, passo = 5.0;   // il fascio i e' a rilevamento0 + i * passo (vero)
    std::vector<float> db;                    // (S + N) / N nel fascio
    std::vector<bool> cieco;                  // baffles o endfire: il fascio non vede
};
// Energia nella banda (un'ottava attorno a banda_hz; 0 = la banda piu' alta del sensore, quella col fascio piu' stretto)
// su `fasci` fasci uguali.
// Con `caso` aggiunge la grana del rumore (1,5 dB).
// `campi`, uno per sorgente (o nullptr), sono i campi dei raggi che il sensore sta usando: la perdita e' la stessa del
// sensore, cosi' lo schermo disegna cio' che il sonar tiene; senza campo, come valuta, il livello A.
RigaCascata cascata(const std::vector<Sorgente>& sorgenti, const Ricevitore& r, const Ambiente& amb, int fasci = 72,
                    double banda_hz = 0.0, Casuale* caso = nullptr, const std::vector<const CampoPerdita*>* campi = nullptr);

// --- DEMON ---
// Inviluppo del rumore di cavitazione in 2-8 kHz: righe all'asse e alla pala con le armoniche, forti quanto il S/N in
// alta frequenza. Piatto (0 dB) se la sorgente non cava o non si sente.
Spettro demon(const Sorgente& s, const Ricevitore& r, const Ambiente& amb, double f_max_hz = 100.0, double bin_hz = 0.1);
// pale e velocita' dal DEMON: frequenza d'asse (la piu' bassa delle due righe forti) e di pala; nullopt se non si legge
struct LetturaDemon {
    double asse_hz = 0.0, pala_hz = 0.0;
    int pale = 0;
};
std::optional<LetturaDemon> leggi_demon(const Spettro& d, double soglia_db = 6.0);
// Il DEMON del fascio puntato (blocco 3 del sonar, G; docs/23 §2): l'inviluppo di tutte le sorgenti che cavitano, pesate
// con la risposta del fascio nella banda 2-8 kHz (anche le navi civili sopra l'innesco). Lo stesso Spettro di demon():
// f0 = 0, bin `bin_hz` fino a `f_max_hz`; la storia (la cascata) la tiene la vista
Spettro demon_fascio(const std::vector<Sorgente>& sorgenti, const Ricevitore& r, const Ambiente& amb, double rilevamento_fascio,
                     double f_max_hz = 100.0, double bin_hz = 0.1);
// Le serie del DEMON (asse, pala, pale), una per sorgente distinguibile, dalla piu' forte: per i cursori armonici
std::vector<LetturaDemon> leggi_demon_fascio(const Spettro& d, double soglia_db = 6.0);

// --- analizzatore di firma ---
struct RigaOsservata {
    double hz = 0.0, snr_db = 0.0;
};
std::vector<RigaOsservata> righe_osservate(const Spettro& s, double soglia_db = 3.0);   // picchi sopra la soglia

// --- righe del proprio battello (blocco 2 del sonar, F; docs/22 §3) ---
// Con Ricevitore::proprio gli spettri hanno le righe del proprio battello (le righe_emesse della sua sorgente: giri,
// regime delle pompe, snorkel), sorgente -3 in righe_ricevute: la sfera e i sensori di scafo per via strutturale in
// tutti i fasci, col loro isolamento; le schiere per l'acqua entro 30 gradi dal fuoco anteriore. Il catalogo del momento
// nel fascio dato, col S/N nel bin, per la pagina della stazione e per la maschera
struct RigaPropria {
    double hz = 0.0, snr_db = 0.0;
    TipoRiga tipo = TipoRiga::rete;
    std::string macchina;                    // "rete", "pompa", "turbina", "riduttore", "elica"...
};
std::vector<RigaPropria> righe_proprie(const Ricevitore& r, const Ambiente& amb, double rilevamento_fascio, double bin_hz);
// La maschera: toglie le righe osservate entro la tolleranza da quelle date (le proprie mascherate), prima di
// confronta_firma e confronta_rapporti. Senza maschera il sonarista sbaglia come dal vero (dossier 09 §3.9)
void togli_righe(std::vector<RigaOsservata>& righe, const std::vector<double>& hz, double tolleranza_hz = 0.2);

struct Ipotesi {
    std::string classe, nazionalita, tipo_unita;
    double confidenza = 0.0;      // 0..1, le ipotesi sommano a 1
    int righe_concordi = 0;
};
// Confronta le righe osservate con le righe dei macchinari delle classi (non quelle d'elica, che dipendono dalla
// velocita'): una riga concorda entro la tolleranza (il bin o lo 0,6% per il Doppler). Punteggio: le righe concordi
// pesate per il loro livello, meno le osservate che la classe non spiega, piu' le pale lette dal DEMON. Confidenza
// con una softmax: con poche righe le ipotesi restano vicine (contatti ambigui, errori possibili).
std::vector<Ipotesi> confronta_firma(const std::vector<RigaOsservata>& righe, const CatalogoAcustico& c,
                                     std::optional<int> pale = std::nullopt, double bin_hz = 0.1);

// --- libreria consultabile (S5b; dossier 09 §3.4-3.5) ---
struct RigaLibreria {
    double hz = 0.0, db_relativo = 0.0;
    TipoRiga tipo = TipoRiga::rete;
    std::string etichetta;                   // "rete 60 Hz x5", "pompa (da 15 kn)", "famiglia 40 Hz x3"
    double hz_fine = 0.0, durata_s = 0.0;    // sweep: da hz a hz_fine in durata_s
    double v_min_kn = 0.0, v_max_kn = 1e9;
};
struct VoceLibreria {
    std::string id, nome, nome_breve;
    std::string categoria;                   // della voce: "ssn", "fregata"... per le firme; "biologico"... per le sorgenti
    std::string categoria_scheda;            // "navale", "biologico", "geofisico", "meteo", "umano"
    std::string sottocategoria, nazionalita, tipo_unita;
    bool unita_navale = false;               // firma di una classe o sorgente della libreria
    std::vector<RigaLibreria> righe;
    std::vector<std::pair<double, double>> bande;   // [f_lo, f_hi] delle componenti a banda e degli impulsi
    int pale = 0;
    double giri_per_nodo = 0.0;
    std::vector<std::string> indizi, confonde_con;
    std::string ritmo, aurale, fonte, confidenza;   // ritmo: "impulsi ogni 12-25 s, a gruppi di 1-2, ..."
};
// gli id della libreria (firme navali con righe, e sorgenti), ordinati per nome; categoria_scheda vuota = tutte
std::vector<std::string> elenco_libreria(const CatalogoAcustico& c, const std::string& categoria_scheda = "");
std::optional<VoceLibreria> voce_libreria(const CatalogoAcustico& c, const std::string& id);

// Confronto per rapporti, insensibile al Doppler: per ogni voce il fattore k (1 + v/c) che allinea piu' righe
// osservate alle righe fisse della voce (gli sweep no), poi le concordanze entro la tolleranza relativa.
struct ConfrontoRapporti {
    std::string id;
    double fattore_doppler = 1.0;
    double velocita_radiale_kn = 0.0;        // (k - 1) c: positiva se la sorgente si avvicina (le righe salgono)
    double punteggio = 0.0, confidenza = 0.0;   // confidenza: softmax sulle voci, somma 1
    std::vector<std::pair<RigaOsservata, RigaLibreria>> concordi;
    std::vector<RigaLibreria> mancanti;      // della voce e non viste, le piu' forti prima
    std::vector<RigaOsservata> in_piu;       // osservate che la voce non spiega
};
std::vector<ConfrontoRapporti> confronta_rapporti(const std::vector<RigaOsservata>& righe, const CatalogoAcustico& c,
                                                  std::optional<int> pale = std::nullopt, double tolleranza_rel = 0.003,
                                                  double doppler_max = 0.012);

// Famiglie armoniche fra le righe osservate (§3.5): fondamentale (ai minimi quadrati), multipli presenti, copertura
// (presenti / attesi fino a f_max) e un indizio: rete 50, 60 o 400 Hz, una famiglia della libreria (per nome breve),
// "elica (pala)" sotto i 30 Hz, altrimenti "ignota". Per l'elica la frequenza di pala e le pale, se c'e' la riga
// d'asse (pala/asse vicino a un intero fra 3 e 9).
struct FamigliaArmonica {
    double f1_hz = 0.0;
    std::vector<int> armoniche;
    double copertura = 0.0;
    std::string indizio;
    double pala_hz = 0.0;
    int pale_stimate = 0;
};
std::vector<FamigliaArmonica> famiglie_armoniche(const std::vector<RigaOsservata>& righe, double f_max_hz,
                                                 const CatalogoAcustico* c = nullptr, double tolleranza_rel = 0.004,
                                                 int minimo = 3);

// a - b in dB bin per bin sulla griglia di a (b interpolato se le griglie differiscono; 0 fuori da b): per gli slot
Spettro differenza(const Spettro& a, const Spettro& b);

}  // namespace m688
