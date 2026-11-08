#pragma once
// Acustica dell'Enhanced (Fase 2 della roadmap): equazione del sonar, perdita di trasmissione dal profilo della
// velocita' del suono, rumore ambiente, rumore irradiato e rumore proprio.
//
// Fonti: docs/ricerca/02_acustica_oceanografia_dottrina.md §1-4 e §10 (modello consigliato, livello A analitico),
// docs/ricerca/01_piattaforme_e_armi.md §3 (rumore irradiato per classe). Le voci [bassa] dei dossier sono
// parametri di taratura del gioco e sono segnate STIMA nel codice.
//
// Il modulo e' puro: non conosce la missione. Unita': metri, secondi, nodi per le velocita' delle piattaforme,
// gradi bussola, quote positive verso il basso, livelli in dB re 1 uPa (spettrali: re 1 uPa^2/Hz) a 1 m.
//
// Convenzione dei livelli: sorgente, rumore e soglia sono spettrali (per Hz) alla frequenza della banda; la soglia
// DT e' il rapporto S/N nella banda W che da' il 50% di rilevazione, per cui
//   passivo  SE = SL - TL - (NL - DI) - DT
//   attivo   SE = SL - 2 TL + TS - max(NL - DI, RL) - DT
// (dossier 02 §1.1-1.2; W si elide fra segnale e rumore integrati sulla stessa banda).

#include <array>
#include <cstdint>
#include <future>
#include <limits>
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include "nucleo/casuale.hpp"
#include "nucleo/geometria.hpp"

namespace m688 {

// --- formule di base (dossier 02 §3.1, §3.2, §4) ---
double velocita_suono_mackenzie(double t_c, double s_psu, double z_m);              // m/s
// Ainslie-McColm (1998), dB/km; f in Hz
double assorbimento_db_km(double f_hz, double t_c, double s_psu, double z_m, double ph = 8.0);
double assorbimento_thorp_db_km(double f_hz);
int stato_del_mare(double vento_m_s);                     // WMO/Douglas, 0..7
double vento_per_stato_del_mare(int ss);                  // m/s (centro della classe)
double altezza_onda_m(int ss);                            // Hs (centro della classe): centro_douglas di nucleo/meteo
// Stato del mare dell'ambiente: dalla Hs del meteo se c'e' (hs_m >= 0, dossier 21 §8 punto 3), altrimenti dal vento
int mare_di(double vento_m_s, double hs_m);

// --- profilo della velocita' del suono di un mese ---
struct ProfiloSuono {
    std::vector<double> z_m, c_m_s;
    double temperatura_superficie = 10.0, salinita_superficie = 35.0;

    double c(double z) const;   // interpolazione lineare; sotto l'ultimo livello prosegue a pressione
    double gradiente(double z) const;   // dc/dz del tratto che contiene z (s^-1)

    // ricavati da analizza() (dossier 02 §3.3, §3.4)
    double sld_m = 0.0;              // layer: massimo di c entro i primi 400 m (0: gradiente negativo dalla superficie)
    double gradiente_strato = 0.0;   // dc/dz sopra la SLD (s^-1, positivo: condotto)
    double gradiente_sotto = 0.0;    // dc/dz nei 100 m sotto la SLD (negativo: termoclino)
    double asse_m = 0.0, c_asse = 0.0;   // minimo di c (asse del canale sonoro)
    double profondita_critica_m = -1.0;  // sotto l'asse, dove c torna al massimo sopra l'asse (-1: mai nei dati)
    double distanza_cz_m = 0.0;          // prima zona di convergenza (0: nessuna nei dati)
    // microstrato superficiale a gradiente negativo sopra un condotto (effetto pomeriggio): spessore e salto di c
    double microstrato_m = 0.0, microstrato_dc = 0.0;
    void analizza();
};

// Profili per mese di un teatro: data/motore/teatri/<teatro>.suono.toml (tools/pipeline/suono.py)
enum class TipoFondo { roccia, sabbia, fango };

struct ClimaTeatro {
    std::string teatro;
    double ph = 8.0, traffico = 0.5;      // parametri del teatro per assorbimento e rumore ambiente
    int mese_predefinito = 2;             // se la missione non ha un mese (scelta di gioco, tools/pipeline/suono.py)
    std::array<ProfiloSuono, 12> mesi;
    // il tipo di fondo per fascia di profondita' (STIMA, tools/pipeline/suono.py FONDI): piattaforma sotto
    // FONDO_SCARPATA_M, scarpata fino a FONDO_PROFONDO_M, poi la piana
    TipoFondo fondo_piattaforma = TipoFondo::sabbia, fondo_scarpata = TipoFondo::sabbia, fondo_profondo = TipoFondo::fango;
    const ProfiloSuono& profilo(int mese) const;   // 1..12 (fuori: ripiegato)
    TipoFondo fondo(double profondita_m) const;    // per Ambiente::fondo, col fondale del punto
};
inline constexpr double FONDO_SCARPATA_M = 200.0, FONDO_PROFONDO_M = 2000.0;
// I gamberi schioccatori (dossier 02 §4.5, 09 §5.3): acque basse e calde, sotto GAMBERI_FONDALE_M di fondale e con
// l'acqua in superficie da GAMBERI_TEMPERATURA_C in su (STIMA: nei teatri del gioco le coste di Otranto e Gibilterra
// d'estate). Per Ambiente::gamberi, col fondale del punto e la temperatura del profilo del mese
inline constexpr double GAMBERI_FONDALE_M = 60.0, GAMBERI_TEMPERATURA_C = 18.0;
bool gamberi_attivi(double fondale_m, double temperatura_superficie_c);
ClimaTeatro carica_clima(const std::string& percorso);   // std::runtime_error se manca o e' incompleto
// file del teatro della missione: <cartella_dati>/teatri/<teatro>.suono.toml (Missione::teatro())
std::string file_clima(const std::string& cartella_dati, const std::string& teatro);

// --- effetto pomeriggio (dossier 02 §3.5) ---
// Dalle 10 alle 18 locali, con vento sotto 11 kn e nubi sotto il 50%, il sole scalda i primi 5-15 m: un
// microtermoclino di 0,5-2 gradi spezza il condotto superficiale; si dissipa in 3-6 h dopo il tramonto o con il vento.
// Intensita' 0..1 (STIMA: sale dalle 10 alle 14, piena fino alle 18, a zero alle 22).
double intensita_pomeriggio(double ora_locale, double vento_m_s, double nuvolosita);
// Il profilo con il microtermoclino (1,5 gradi per l'intensita' 1 nei primi 10 m), gia' analizzato. Il runtime lo
// tiene da parte e lo rifa' quando l'intensita' cambia (per esempio ogni 10 minuti di gioco).
ProfiloSuono applica_pomeriggio(const ProfiloSuono& p, double intensita);

// --- ambiente del percorso ---

struct Ambiente {
    const ProfiloSuono* profilo = nullptr;   // nullptr: isotermo a 1500 m/s piu' la pressione
    double fondale_m = 1e9;                  // il piu' basso fra sorgente e ricevitore (Fondale del teatro)
    TipoFondo fondo = TipoFondo::sabbia;
    double vento_m_s = 5.0;                  // rumore (Knudsen) e, senza hs_m, lo stato del mare
    double hs_m = -1.0;                      // Hs vera dal meteo (Enhanced, M2): lo stato del mare e le onde; < 0 nessuna
    double pioggia_mm_h = 0.0;
    double traffico = 0.5;                   // 0..1 (dossier 02 §4.3)
    double ghiaccio = 0.0;                   // frazione di copertura 0..1
    // la rugosita' della volta (rms, m: 1 il primo anno, 2,5 il pack pluriennale; dossier 06 §5.1): nella riflessione in
    // superficie dei raggi al posto delle onde, pesata con la copertura
    double rugosita_ghiaccio_m = 1.0;
    // il carattere del pack per il rumore (dossier 02 §4.6, 06 §5.1; STIMA): la frazione di ghiaccio deformato dalle creste
    // nell'intorno (le creste che si spingono scricchiolano a bassa frequenza, attorno ai 100 Hz) e il freddo che spacca la
    // volta (le fessure termiche, impulsive attorno ai 2 kHz), 0..1; 0 e 0 il pack quieto di sempre
    double ghiaccio_creste = 0.0, ghiaccio_fessure = 0.0;
    bool gamberi = false;                    // gamberi schioccatori (acque basse e calde)
    double ph = 8.0;
};

// --- rumore irradiato (dossier 01 §3.2-3.3, 02 §2.3-2.6) ---
// Sorgente vista dalla testa di un siluro (nucleo/siluro.hpp): livello a banda larga sulla scala della testa
struct SorgenteAcustica {
    int id = -1;
    Vec2 pos;
    double quota = 0.0;
    double rotta = 0.0;                          // gradi
    double velocita = 0.0;                       // m/s
    double livello_db = 120.0;                   // rumore irradiato a banda larga (dB re 1 uPa @1 m)
    double forza_bersaglio_db = 15.0;            // TS per l'attivo (sottomarino ~ 10-25, esca ~ 0)
    double lunghezza_m = 100.0;                  // estensione dell'eco: le esche sono puntiformi
    double larghezza_m = 0.0;                    // scafo per la spoletta del siluro: 0 = non nota (resta il margine di prima)
    double pescaggio_m = 0.0;                    // navi: la carena sotto il galleggiamento; 0 = non nota
    bool esca = false;                           // esca acustica (stazionaria, Doppler zero)
    bool nave = false;                           // in superficie (scia per i segui-scia)
    bool lanciatore = false;
};

// riga dei macchinari di una classe (analizzatore di firma, S3): frequenza, livello rispetto alla tonale, tipo e
// intervallo di velocita' in cui e' presente (pompe lente e veloci)
enum class TipoRiga { rete, pompa, turbina, riduttore, ausiliario, diesel, elica, biologico, geofisico, industriale };
const char* nome_tipo_riga(TipoRiga t);
struct RigaFirma {
    double hz = 0.0, db_relativo = 0.0;
    TipoRiga tipo = TipoRiga::rete;
    double v_min_kn = 0.0, v_max_kn = 1e9;
    // blocco 1 del sonar: > 0 la riga e' legata ai giri dell'asse, a questo multiplo della frequenza d'asse (le
    // accensioni di un diesel lento in presa diretta: cilindri x giri / 60); hz e' allora quella alla velocita' di
    // riferimento della firma, per la libreria. 0: riga fissa (le firme di sempre)
    double multiplo_asse = 0.0;
};

struct FirmaRumore {
    std::string classe;
    // livello a banda larga 5-200 Hz per velocita' (kn, dB re 1 uPa @1 m), dalla tabella del dossier 01 §3.2,
    // interpolato in log v; vuota: sl_quieto + 20 log(v / v_quieto)
    std::vector<std::pair<double, double>> punti;
    double sl_quieto_db = 120.0;     // livello a banda larga 5-200 Hz alla velocita' silenziosa
    double v_quieto_kn = 5.0;
    double v_cavitazione_kn = 5.0;   // insorgenza a quota zero (02 §2.3); cresce con la profondita'. La stessa
                                     // tabella della dinamica (DinamicaSottomarino::v_cavitazione0_kn)
    double tonale_db = 110.0;        // la tonale piu' forte (assoluta, dB re 1 uPa @1 m)
    double tonale_hz = 300.0;        // la sua frequenza (rete a 60 Hz USA, 50 Hz URSS, armoniche e pompe)
    double ts_prua_db = 12.0, ts_traverso_db = 25.0;   // forza di bersaglio (§1.4), gia' ridotta se anecoico
    bool nave = false;               // navi, armi, esche: la tabella comprende gia' la cavitazione
    double snorkel_db = 0.0;         // aumento con i diesel in moto (SSK: +20-25)
    // per l'analizzatore di firma (S3): righe dei macchinari, elica, nazionalita' e tipo d'unita'
    std::vector<RigaFirma> righe;
    int pale = 0, assi = 1;
    double giri_per_nodo = 0.0;      // giri dell'asse al minuto per nodo
    // elica a passo variabile (blocco 1 del sonar: traghetti, pescherecci): > 0 l'asse gira a questi giri al minuto
    // costanti in moto (da 0,5 kn), e asse, pala e DEMON non dicono la velocita'; 0: giri_per_nodo
    double giri_costanti = 0.0;
    std::string nazionalita = "ignota", tipo_unita = "generica";
    std::string fonte;
};

// --- tipi di navi civili (blocco 1 del sonar, docs/20_sonar_blocco1.md §2): gli intervalli dell'individuo ---
// Da [civili.*] di acustica.toml (tools/pipeline/civili.py, dossier docs/ricerca/26_navi_civili_traffico.md). Ogni nave
// del traffico e' un individuo generato dal seme (nucleo/navi_civili.hpp); il catalogo ha in piu' la firma tipica del
// tipo fra le firme, per la libreria e per le missioni che mettono una nave di quel tipo.
enum class Propulsione { diesel_lento, diesel_medio, diesel_veloce, diesel_elettrico, turbina };
struct Intervallo {
    double min = 0.0, max = 0.0;
};
struct TipoCivile {
    std::string id, nome, categoria;          // categoria: mercantile, petroliera, traghetto, rimorchiatore, ...
    std::string classe;                       // classe di classi.toml (modello, ROE)
    Intervallo lunghezza_m, pescaggio_m, velocita_servizio_kn, cavitazione_kn;
    double velocita_max_rel = 1.1;            // la massima sulla velocita' di servizio
    double larghezza_rel = 0.15;              // B / L
    // livello: il modello JOMOPANS-ECHO (MacGillivray e de Jong 2021) della sua classe, sulla velocita' e la lunghezza
    double echo_vc_kn = 12.0;                 // velocita' di riferimento della classe ECHO (f1 = 480 / V_C)
    double echo_d_lf = 0.0;                   // > 0: le classi da carico, col picco sotto 100 Hz (D_LF)
    double epoca_db = 0.0;                    // correzione d'epoca sul modello ECHO (navi degli anni '80, STIMA)
    Propulsione propulsione = Propulsione::diesel_lento;
    bool passo_variabile = false;
    Intervallo assi, pale, giri_asse;         // giri dell'asse al minuto alla velocita' di servizio
    Intervallo motori, cilindri, giri_motore; // per i diesel con riduttore e i generatori del diesel-elettrico
    Intervallo generatori, cilindri_generatori, giri_generatori;
    double rete_60 = 0.5;                     // probabilita' della rete a 60 Hz (altrimenti 50)
    bool strascico = false;                   // al lavoro tira la rete (sorgente rete_strascico del paesaggio)
    std::string fonte, confidenza;
};

// --- sorgenti non navali: paesaggio sonoro (Fase 3, S5c; dossier 09 §5 e §6.2) ---
// Componenti dello spettro e del tempo di una sorgente della libreria. Livelli relativi al livello della sorgente.
enum class TipoComponente { tonale, famiglia, banda, sweep, impulso };
struct Componente {
    TipoComponente tipo = TipoComponente::banda;
    double livello_rel_db = 0.0;
    double hz = 0.0;                         // tonale; famiglia: la fondamentale
    int armoniche = 1;                       // famiglia: quante
    double decadimento_db = 0.0;             // famiglia: perdita per armonica
    double f_lo = 0.0, f_hi = 0.0;           // banda e impulso
    double picco_hz = 0.0, pendenza_db_ott = 0.0;   // forma della banda: dB per ottava dal picco (0: piatta)
    double f_da = 0.0, f_a = 0.0;            // sweep (discendente se f_a < f_da)
    double durata_s = 0.0;                   // impulso, sweep, e tonali o famiglie ritmate
    bool ritmata = false;                    // segue il ritmo della sorgente (altrimenti continua)
    // banda e impulso: 10 log dell'integrale della forma (integrale_forma_db), calcolato al caricamento; NaN: da fare
    double integrale_db = std::numeric_limits<double>::quiet_NaN();
};
double integrale_forma_db(const Componente& c);
enum class Processo { continuo, periodico, poisson, treno, singolo };   // singolo: un evento all'istante 0 (transitori)
struct Ritmo {
    Processo processo = Processo::continuo;
    double intervallo_min_s = 10.0, intervallo_max_s = 10.0;   // periodico e treno
    double tasso_h = 0.0;                    // poisson: eventi all'ora
    int gruppo_min = 1, gruppo_max = 1;      // doppiette e terzine
    double interno_min_s = 0.0, interno_max_s = 0.0;
    double serie_min = 0.0, pausa_s = 0.0;   // treno: serie di serie_min minuti, poi pausa_s di silenzio
};
struct Campione;   // nucleo/campioni.hpp
struct SorgenteLibreria {
    std::string id, nome, nome_breve, categoria, sottocategoria;   // categoria: biologico, geofisico, meteo, civile, industriale
    std::vector<std::string> teatri, ore;    // ore: "giorno", "crepuscolo", "notte"; vuote = sempre
    std::vector<int> mesi;                   // vuoti = tutto l'anno
    double densita = 0.0;                    // istanze per 1000 nm^2
    double quota_min_m = 0.0, quota_max_m = 0.0, v_min_kn = 0.0, v_max_kn = 0.0;
    double estensione_m = 0.0;               // > 0: sorgente diffusa (coro, branco, ghiaccio): settore largo sul BTR
    double livello_db = 150.0;               // rms (i picco-picco sono gia' convertiti, -9 dB)
    double ts_db = -10.0;                    // eco per l'attivo
    double diurno_db = 0.0;                  // aumento nella fascia diurno_ore (coro al crepuscolo)
    std::string diurno_ore;
    std::vector<Componente> componenti;
    Ritmo ritmo;
    std::vector<std::string> indizi, confonde_con;
    std::string aurale, fonte, confidenza;
    double vita_s = 0.0;                     // transitori: durata dall'istante 0 (0 = senza fine)
    // R5 (collega_campioni): i campioni registrati della voce e l'attivita' media di lungo periodo di ogni
    // componente; con un campione l'attivita' segue il suo inviluppo nel tempo (la cuffia suona lo stesso tratto)
    std::vector<const Campione*> campioni;
    std::vector<double> attivita_media;
};
const char* fascia_oraria(double ora_locale);   // "giorno" 7-18, "crepuscolo" 5-7 e 18-21, "notte"

// --- variazioni dell'unita' (pezzo R6, dossier docs/ricerca/18_audio_reale.md §3; solo nell'Enhanced) ---
// Il regime delle pompe del reattore: automatico = per velocita' (le v_min/v_max delle righe della firma, come senza
// variazioni); spente (circolazione naturale), lente, veloci forzate
enum class RegimePompe : std::uint8_t { automatico, spente, lente, veloci };
const char* nome_regime_pompe(RegimePompe r);
// Quello che rende un'unita' diversa dalla sua classe e dal momento prima. Tutto spento per default: la Sorgente e'
// quella della classe (e il classico non lo vede). sorgente_acustica le ricava da UnitaAcustica
struct VariazioniSorgente {
    bool attive = false;
    std::uint64_t seme = 0;              // dell'unita': le stesse differenze per tutta la missione
    double scarto_db = 0.0;              // banda larga dell'individuo (Sponagle: sigma 2,4 dB)
    double fattore_giri = 1.0;           // asse, pala, turbine e riduttori: +-1-3% dell'individuo (STIMA)
    double fattore_pompe = 1.0;          // pompe: +-0,5% (STIMA: motori asincroni sulla rete)
    double virata_db = 0.0;              // eccesso in virata (Trevorrow: 1,65 dB per grado/s)
    double usura_db = 0.0;               // fino a +5 dB (Mia98)
    double urto_db = 0.0;                // dopo le collisioni: scafo ammaccato, eliche, piastrelle (al piu' +12)
    double danno_elica = 0.0, danno_macchine = 0.0;   // 0..1
    RegimePompe regime = RegimePompe::automatico;
    double tempo_s = 0.0;                // per gli ausiliari che si accendono e si spengono
    double cantante_hz = 0.0;            // elica «cantante» (rara): la riga e la finestra di velocita' (0: nessuna)
    double cantante_da_kn = 0.0, cantante_a_kn = 0.0;
};

struct Sorgente {
    int id = -1;
    Vec2 pos;
    double quota_m = 0.0;
    double rotta = 0.0;
    double velocita_kn = 0.0;        // velocita' vera (non quella del moto classico)
    const FirmaRumore* firma = nullptr;   // nullptr: non e' una sorgente per il sonar (aerei)
    std::optional<bool> cavitazione;      // dalla dinamica quando c'e'; altrimenti dalla firma (cavita())
    bool snorkel = false;
    bool sul_fondo = false;          // posato: assetto silenzioso (-10 dB), l'eco si confonde con il fondo
    bool muta = false;               // solo eco (un relitto, I10): niente rumore irradiato, righe ne' DEMON; resta la TS
    bool esca = false;               // falso bersaglio (flag 80h): esca o biologico
    bool nave = false;               // in superficie (scia per i segui-scia)
    double lunghezza_m = 100.0;      // estensione dell'eco
    double transitorio_db = 0.0;     // aumento momentaneo (porte dei tubi, lancio: +10-30 dB per pochi secondi)
    // impulso attivo emesso (per l'intercettazione: la sorgente e' chi fa il ping)
    double ping_db = 0.0;            // SL dell'impulso (0: nessuno)
    double ping_hz = 3500.0;
    double ping_s = 0.5;
    // sorgente della libreria (firma = nullptr): impulsi e sweep dipendono dal tempo di gioco e dal seme; l'energia e'
    // la media sulla finestra d'integrazione che finisce a tempo_s
    const SorgenteLibreria* fonte = nullptr;
    double tempo_s = 0.0, finestra_s = 2.0, ora_locale = -1.0;
    std::uint64_t seme = 0;
    double estensione_m = 0.0;       // sorgente diffusa
    double guadagno_db = 0.0;        // sorgente della libreria: sul suo livello (aerei: la quota, nucleo/acustica_asw)
    double fedelta = 1.0;            // il falso contatto (UnitaAcustica::fedelta): righe e DEMON rovinati sotto 1
    VariazioniSorgente var;          // R6: l'individuo, il regime, le virate, i danni (spente: la classe)
};

// Dati di un'unita' per costruire la sua Sorgente (sorgente_acustica): li da' il runtime, una volta per passo
struct UnitaAcustica {
    int id = -1;
    std::string classe, categoria;   // categoria: sottomarino, nave, arma, aereo, nessuna (biologici)
    Vec2 pos;
    double quota_m = 0.0, rotta = 0.0, velocita_kn = 0.0;
    std::optional<bool> cavitazione;
    double transitorio_db = 0.0;
    bool snorkel = false, sul_fondo = false;
    bool falso_bersaglio = false;    // flag 80h
    double lunghezza_m = 0.0;        // 0: dalla categoria
    // R6 (solo Enhanced): con variazioni spente tutto il resto e' ignorato e la sorgente e' quella della classe
    bool variazioni = false;
    std::uint64_t seme = 0;          // dell'unita' nella missione (per esempio l'hash di seme della missione e id)
    double tempo_s = 0.0;            // s di missione
    double rateo_virata_deg_s = 0.0; // modulo
    double danno_elica = 0.0, danno_macchine = 0.0, usura = 0.0;   // 0..1
    double urto_db = 0.0;            // rumore in piu' dalle collisioni (DanniSottomarini::rumore_urto_db, 0..12 dB)
    RegimePompe regime_pompe = RegimePompe::automatico;
    // blocco 1 del sonar: la firma dell'individuo (navi civili generate, nucleo/navi_civili), al posto di quella della
    // classe; deve vivere quanto la sorgente. nullptr: la firma della classe
    const FirmaRumore* firma = nullptr;
    // il falso contatto del vetrino di Dio (docs/28 §3.5, D3): quanto la firma imita la classe (1 = perfetta, il valore di
    // tutte le sorgenti vere). Sotto 1 mancano righe e le altre sono piu' deboli; sotto 0,7 il DEMON non ha le pale
    double fedelta = 1.0;
};
// la fedelta' del falso contatto sotto cui il DEMON non ha le pale (una sorgente senza elica vera; STIMA)
inline constexpr double FEDELTA_SENZA_PALE = 0.7;

// --- sensori ---
enum class Antenna { sfera, lineare, scafo };
enum class ModoSensore { passivo, attivo };

struct Sensore {
    std::string id;
    ModoSensore modo = ModoSensore::passivo;
    Antenna antenna = Antenna::sfera;
    double dimensione_m = 4.6;               // diametro della sfera o lunghezza della schiera
    std::vector<double> bande_hz;            // centri delle bande in cui lavora (fra quelle di BANDE_HZ)
    double integrazione_s = 10.0;            // tempo d'integrazione t
    double indice_rilevazione = 14.0;        // d (Pfa 1e-4, Pd 0,5: dossier 02 §1.2)
    double abilita_db = 0.0;                 // operatore: +-3 dB sulla soglia (positivo: piu' bravo)
    double nl_macchina_db = 55.0;            // rumore proprio del macchinario a 1 kHz (spettrale)
    double nl_flusso_db = 50.0;              // rumore di flusso a 1 kHz alla velocita' v0
    double v0_flusso_kn = 8.0;               // §2.5: 8 kn sfera di prua, 12 kn schiera trainata
    double velocita_max_kn = 25.0;           // oltre il sensore e' sordo (schiera: 15-18 kn)
    double baffles_gradi = 35.0;             // semiapertura del settore cieco a poppa (0: nessuno)
    double baffles_db = 15.0;                // aumento del rumore proprio nei baffles
    // attivo
    double sl_attivo_db = 225.0;             // livello di sorgente dell'impulso (§1.5)
    double impulso_s = 0.1;
    double risoluzione_s = 0.0;              // FM con compressione: 1/B per l'area della riverberazione (0: impulso_s)
    // schiera trainata (docs/07 §3.4): cieca oltre il rateo di virata, poi si raddrizza; endfire anteriore cieco
    double virata_max_deg_s = 1e9;
    double raddrizzamento_s = 0.0;
    double endfire_gradi = 0.0;              // semiapertura del settore cieco verso prua
    bool conico = false;                     // ambiguita' conica (destra-sinistra) del rilevamento
    // LOFAR (dossier 02 §1.2 e §2.7): analisi a banda stretta sulle tonali; 0 = nessuna
    double lofar_bin_hz = 0.0;
    double lofar_integrazione_s = 60.0;
    // boe e sonar a immersione (nucleo/acustica_asw): guadagno d'antenna dato (DIFAR, VLAD) invece che dalle
    // dimensioni (< 0); errore del rilevamento misurato (> 0) invece che dal fascio; omni: nessun rilevamento; errore
    // della distanza degli attivi
    double di_fisso_db = -1.0;
    double sigma_rilevamento_deg = 0.0;
    bool omni = false;
    double sigma_distanza_m = 0.0;
    // il mare sul sensore (rumore_mare_proprio_db): il massimo in dB a quota zero; < 0 dall'antenna (10 sonar di scafo e
    // sfere, 6 schiere). Boe e sonar a immersione 0: il dossier 24 §7.4 lo da' ai soli sonar di scafo e schiere, e il
    // mare li tocca gia' con DatiBoa::mare_max e col dip vietato col mare forte
    double mare_proprio_db = -1.0;
    // gamma di frequenze del sensore per le righe di pala nel LOFAR (0: dalle bande, mezza ottava oltre gli estremi)
    double f_min_hz = 0.0, f_max_hz = 0.0;
    // schiere trainate (blocco 1 del sonar, D): il diametro del cavo, per le righe della sua vibrazione (Strouhal)
    // quando il cavo e' filato (Ricevitore::cavo_filato_m); 0 nessuna
    double cavo_diametro_m = 0.0;
    // le righe del proprio battello (blocco 2 del sonar, F): l'isolamento fra le macchine e il sensore per la via
    // strutturale (STIMA: sfera 30 dB, sensori di scafo 22); < 0 nessuna. Le schiere lineari le prendono per l'acqua
    double isolamento_proprio_db = -1.0;
};

// --- schiere trainate (blocco 1 del sonar, D; dossier 04 §1.5 e §6.2): [schiere.*], la geometria in nucleo/schiera ---
struct DatiSchiera {
    std::string id;                                   // la chiave: il sensore (tb16) o il sistema
    double schiera_m = 73.0, cavo_max_m = 790.0;      // lunghezza della schiera e cavo filabile
    double cavo_diametro_m = 0.0094, peso_cavo_n_m = 2.0;   // il cavo, il peso in acqua
    double cn = 1.2;                                  // coefficiente di resistenza normale (cilindro, Re ~1e4)
    double v_flessione_kn = 1.2;                      // la flessione della schiera: L min(1, vD / v)
    double filata_m_s = 0.9, recupero_m_s = 0.75;
    double v_manovra_kn = 12.0, v_max_kn = 20.0;      // per filare o recuperare; di traino
    double quota_max_m = 700.0;                       // oltre: schiacciata (il runtime la danneggia)
    std::string fonte;
};

// Firme per classe (chiavi di classi.toml) e sensori: data/motore/acustica.toml (tools/pipeline/acustica.py)
struct CatalogoAcustico {
    std::map<std::string, FirmaRumore> firme;
    std::map<std::string, Sensore> sensori;
    std::map<std::string, SorgenteLibreria> sorgenti;   // [sorgenti.*]
    const SorgenteLibreria* sorgente(const std::string& id) const;
    // [transitori.*]: aerei, tonfi, ping ed esplosive dell'ASW aereo (nucleo/acustica_asw), fuori dal paesaggio
    std::map<std::string, SorgenteLibreria> transitori;
    const SorgenteLibreria* transitorio(const std::string& id) const;
    std::map<std::string, std::vector<std::string>> suite;   // classe -> sensori di bordo
    std::map<std::string, TipoCivile> civili;                 // [civili.*] (blocco 1 del sonar)
    const TipoCivile* tipo_civile(const std::string& id) const;
    std::map<std::string, DatiSchiera> schiere;               // [schiere.*] (blocco 1 del sonar, D)
    const DatiSchiera* schiera(const std::string& id) const;
    const FirmaRumore* firma(const std::string& classe) const;   // nullptr se manca
    const Sensore* sensore(const std::string& id) const;
    std::vector<const Sensore*> sensori_di(const std::string& classe) const;   // vuoto se la classe non ne ha
};
CatalogoAcustico carica_acustica(const std::string& percorso);   // std::runtime_error se manca o e' incompleto

struct Ricevitore {
    Vec2 pos;
    double quota_m = 0.0;
    double rotta = 0.0;
    double velocita_kn = 0.0;
    const Sensore* sensore = nullptr;
    // dopo il sensore, cosi' Ricevitore{pos, quota, rotta, velocita, sensore} resta valido
    double rateo_virata_deg_s = 0.0;     // modulo; la schiera e' cieca oltre virata_max_deg_s
    double tempo_da_virata_s = 1e9;      // dalla fine dell'ultima virata oltre il limite (raddrizzamento)
    bool sul_fondo = false;              // posato: niente rumore di flusso proprio
    // la schiera trainata (blocco 1 del sonar, D): il cavo filato (> 0 con le righe della sua vibrazione, se il
    // sensore ha il diametro del cavo) e la frazione della schiera che striscia sul fondo, col tipo di fondo (il rumore
    // di sfregamento nel rumore proprio). La quota e' quota_m: quella della schiera (nucleo/schiera)
    double cavo_filato_m = 0.0;
    double sfregamento = 0.0;
    TipoFondo fondo_sfregamento = TipoFondo::sabbia;
    // blocco 2 del sonar, F: la sorgente del proprio battello (firma, velocita', regime delle pompe), per le sue righe
    // negli spettri; nullptr: nessuna, come prima. La schiera danneggiata (dossier 04 §6.3): -6 dB di guadagno
    // d'antenna e niente sopra 300 Hz
    const Sorgente* proprio = nullptr;
    bool schiera_danneggiata = false;
};

enum class Percorso { diretto, condotto, zona_convergenza, fondo, acqua_bassa, raggi, nessuno };
const char* nome_percorso(Percorso p);

struct Perdita {
    double tl_db = 0.0;
    Percorso percorso = Percorso::nessuno;   // il contributo piu' forte
    int zona = 0;                            // con zona_convergenza: 1, 2, 3...
};

struct Rilevazione {
    double se_db = -1e9;                     // eccesso di segnale nella banda migliore
    double banda_hz = 0.0;
    double sl_db = 0.0, tl_db = 0.0, nl_db = 0.0, di_db = 0.0, dt_db = 0.0;
    double ts_db = 0.0, rl_db = -1e9;        // attivo
    Percorso percorso = Percorso::nessuno;
    int zona = 0;
    bool baffles = false;                    // la sorgente e' nel settore cieco del ricevitore
    bool sordo = false;                      // il ricevitore va piu' veloce del limite del sensore
    bool cieco = false;                      // schiera: endfire anteriore o in virata
    double rilevamento_relativo = 0.0;       // dalla prua del ricevitore (-180..180)
    bool conico = false;                     // il sensore ha l'ambiguita' conica (rilevamento specchio)
    bool cavitazione = false, transitorio = false, ping = false;   // indizi per la classificazione
    // LOFAR: eccesso sulla tonale piu' forte (con +3 dB le righe si leggono: classificazione); -1e9 senza LOFAR
    double se_tonale_db = -1e9;
    bool lofar = false;                      // la scoperta viene dalla tonale (piu' forte della larga banda)
};

// bande di calcolo (dossier 02 §10.2)
inline constexpr std::array<double, 7> BANDE_HZ = {50.0, 150.0, 300.0, 1000.0, 3500.0, 10000.0, 25000.0};

// --- paesaggio sonoro: istanze delle sorgenti della libreria nella scena (la popolazione la fa il runtime) ---
struct IstanzaSorgente {
    int id = -1;
    std::string sorgente;                    // chiave di [sorgenti.*]
    Vec2 pos;
    double quota_m = 0.0, rotta = 0.0, velocita_kn = 0.0;
    std::uint64_t seme = 0;
    double eta_s = 0.0;                      // da quanto e' in scena (cambi di rotta)
};
// Istanze per un'area: per ogni sorgente del teatro, del mese e dell'ora (ora < 0 = ignota: non filtra) un conteggio di
// Poisson dalla densita'; posizioni nel disco, quote, rotte e velocita' dal seme. Id da id_base in su.
std::vector<IstanzaSorgente> popola_sorgenti(const CatalogoAcustico& c, const std::string& teatro, int mese,
                                             double ora_locale, Vec2 centro, double raggio_m, std::uint64_t seme,
                                             int id_base = 100000);
void muovi_sorgente(IstanzaSorgente& i, double dt_s);   // in moto lungo la rotta, che cambia ogni 10 minuti
// La Sorgente al tempo di gioco t con la finestra d'integrazione (2 s per la stazione, il dt della coppia nei contatti)
Sorgente sorgente_da_istanza(const CatalogoAcustico& c, const IstanzaSorgente& i, double tempo_s, double finestra_s = 2.0,
                             double ora_locale = -1.0);
// frazione della finestra in cui la componente suona (1 per le continue): dagli istanti degli eventi, da (seme, tempo)
double attivita(const SorgenteLibreria& f, const Componente& c, std::uint64_t seme, double tempo_s, double finestra_s);
// livello spettrale (dB re 1 uPa^2/Hz @1 m) della sorgente della libreria alla frequenza f, mediato sulla finestra; con
// `righe` le tonali, le famiglie e gli sweep entrano spalmati su un'ottava (per la larga banda), altrimenti restano fuori
// (lo spettro LOFAR le mette nei loro bin)
double livello_fonte(const Sorgente& s, double f_hz, bool righe);
// Lo stesso calcolo a pezzi, per gli spettri (mille frequenze per sorgente): i livelli delle componenti (base, livello
// relativo e attivita' della finestra, NaN se spenta) non dipendono dalla frequenza e si fanno una volta.
// livello_fonte(s, f, righe) == livello_fonte_da(s, f, righe, livelli_componenti(s)) bit per bit; vuoto se l'attivita'
// dipende dalla frequenza (un campione collegato, R5): allora livello_fonte_da ripiega su livello_fonte
std::vector<double> livelli_componenti(const Sorgente& s);
double livello_fonte_da(const Sorgente& s, double f_hz, bool righe, const std::vector<double>& livelli);
double livello_totale_fonte(const Sorgente& s);   // tutte le componenti, mediate sulla finestra
// tonali e famiglie della sorgente della libreria in quella finestra: (hz, dB); `tipo` riceve il tipo di riga
std::vector<std::pair<double, double>> righe_fonte(const Sorgente& s, TipoRiga* tipo = nullptr);

// --- costruzione delle sorgenti (una per unita' e per passo, la stessa per sensori e siluri) ---
// firma: la classe; type53 -> uset80; esca lanciata (arma con flag 80h) -> adc_mk2; biologico; altrimenti la firma
// generica della categoria (sottomarino_generico, nave_generica, siluro_generico); gli aerei restano senza firma
Sorgente sorgente_acustica(const CatalogoAcustico& c, const UnitaAcustica& u);
// la stessa sorgente sulla scala della testa dei siluri (STIMA: 130 dB = 688 silenzioso del livello classico)
SorgenteAcustica sorgente_per_siluri(const Sorgente& s);
double velocita_cavitazione_kn(const FirmaRumore& f, double quota_m);
bool cavita(const FirmaRumore& f, double velocita_kn, double quota_m);
double livello_banda_larga(const Sorgente& s);   // 5-200 Hz, con cavitazione, snorkel, posato e transitorio
// Riga di pala (dossier 02 §2.2: f = pale x giri / 60; 0 senza elica o sotto 0,5 kn) e il suo livello: righe di pala
// non cavitanti alla Ross, 60 log V attorno a 10 kn, dove stanno 15 dB sopra il continuo a bassa frequenza; +10 dB
// con la cavitazione, -10 posato (STIMA). La stessa per la scoperta (LOFAR di valuta) e per lo spettro del giocatore.
double frequenza_pala_hz(const Sorgente& s);
double livello_pala_db(const Sorgente& s);
// R6: l'aspetto (Trevorrow 2008: nave cos^1,95(theta - 97) + 0,08, prua -10 dB e poppa -7; i sottomarini al 30%, STIMA)
// visto da `ascoltatore`, dB sul livello della sorgente; sotto 100 Hz al 30% (le basse frequenze sono quasi
// omnidirezionali). 0 senza variazioni, per armi, esche e sorgenti della libreria
double aspetto_db(const Sorgente& s, Vec2 ascoltatore, double f_hz);

// --- le parti ---
Perdita perdita_trasmissione(double f_hz, double r_m, double zs_m, double zr_m, const Ambiente& amb);
// Lo specchio di Lloyd del livello A (dossier 02 §3.4 h), lo stesso calcolo di perdita_trasmissione: il fattore
// sull'intensita' del diretto (2 = incoerente, oltre pi/4; sotto, il dipolo con la coerenza di Eckart), la velocita'
// del suono in superficie e lo scarto dell'onda (Hs / 4) che usa. Per le frange del LOFAR (nucleo/interferenza)
struct SpecchioLloyd {
    double fattore = 2.0, c0_m_s = 1500.0, sigma_onda_m = 0.0;
};
SpecchioLloyd specchio_lloyd(double f_hz, double r_m, double zs_m, double zr_m, const Ambiente& amb);
// perdita di una riflessione sul fondo (dB): per tipo, frequenza e angolo di radenza; sabbia e roccia sotto l'angolo
// critico quasi senza perdita (C2 §2.2)
double perdita_fondo_db(TipoFondo t, double f_hz, double angolo_gradi);
double rumore_ambiente(double f_hz, const Ambiente& amb);                  // spettrale (Coates, pioggia, ghiaccio)
// Le parti del rumore ambiente (dB spettrali, -300 se assenti), con le stesse formule: la loro somma in potenza e'
// rumore_ambiente. Per la cuffia, che suona vento e pioggia con le tessiture registrate (R5)
struct ComponentiRumore {
    double turbolenza = -300.0, traffico = -300.0, vento = -300.0, termico = -300.0, pioggia = -300.0, gamberi = -300.0,
           ghiaccio = -300.0;
};
ComponentiRumore componenti_rumore_ambiente(double f_hz, const Ambiente& amb);
double livello_sorgente(double f_hz, const Sorgente& s);                   // spettrale, a 1 m
// lo stesso con la banda larga gia' calcolata (livello_banda_larga(s)): bit per bit uguale, per gli spettri
double livello_sorgente_da(double f_hz, const Sorgente& s, double banda_larga_db);
double rumore_proprio(double f_hz, const Sensore& s, double velocita_kn, bool nei_baffles);
// Lo sfregamento della schiera sul fondo (blocco 1 del sonar, D; STIMA): spettrale, dB re 1 uPa^2/Hz; piatto fino a
// 300 Hz, poi -20 dB/decade; 30 log V, 10 log della frazione sul fondo, roccia +6 dB, fango -8. -300 senza contatto
double rumore_sfregamento_db(double f_hz, double velocita_kn, double frazione, TipoFondo fondo);
// Il rumore proprio del ricevitore: rumore_proprio piu' lo sfregamento della schiera (Ricevitore::sfregamento)
double rumore_proprio_ricevitore(double f_hz, const Ricevitore& r, double velocita_kn, bool nei_baffles);
// Il mare nel rumore proprio di un sensore vicino alla superficie (dossier 24 §7.4, 21 §6.1, 02 §8): beccheggio, bolle
// sul duomo, la schiera che si muove con l'onda. Da mare 5: sonar di scafo fino a +10 dB (a quota zero, niente oltre
// i 100 m), schiere trainate fino a +6 dB (niente oltre i 150 m); con mare 5 il 60%. Boe e sonar a immersione no
// (Sensore::mare_proprio_db = 0). Per tutti allo stesso modo
// (fase 2 delle interazioni fra IA): la differenza la fanno i mezzi. STIMA
double rumore_mare_proprio_db(const Sensore& s, double quota_m, const Ambiente& amb);
double indice_direttivita(const Sensore& s, double f_hz);
double soglia(const Sensore& s);                                           // DT: S/N nella banda
double forza_bersaglio(const FirmaRumore& f, double aspetto_gradi);        // aspetto 0 = prua, 90 = traverso
// forza di diffusione della superficie (Chapman e Harris 1962, dossier 02 §1.3) col vento e l'angolo radente
double forza_superficie_db(double f_hz, double vento_m_s, double radente_deg);
// larghezza del fascio a -3 dB alla frequenza f: sfera 58,4 lambda/D; schiera 50,8 lambda/L al traverso, piu' larga
// verso l'endfire (al piu' 2 sqrt(lambda/L) rad); rilevamento relativo dalla prua (dossier 02 §1.2)
double larghezza_fascio_deg(const Sensore& s, double f_hz, double rilevamento_relativo);
// deviazione standard del rilevamento misurato (dossier 02 §10.5: theta_3dB / sqrt(SNR), almeno 0,2 gradi), con
// l'allargamento del fascio verso l'endfire per le schiere lineari
double errore_rilevamento_deg(const Rilevazione& r, const Sensore& s);

// --- equazione del sonar ---
// Passivo: il ricevitore ascolta la sorgente. Attivo: il ricevitore emette e ascolta l'eco (monostatico).
Rilevazione valuta(const Sorgente& s, const Ricevitore& r, const Ambiente& amb);

// --- tracciamento dei raggi (livello B del dossier 02 §10.2) ---
// Campo di perdita di una sorgente su una griglia (distanza, quota): raggi incoerenti a ventaglio nel profilo
// (equazioni dei raggi con passo d'arco, riflessione su superficie e fondo con le loro perdite), intensita' con i
// tubi di raggi fra i vicini (fasci gaussiani). Ambiente uniforme lungo il percorso (fondale = amb.fondale_m).
// L'assorbimento si aggiunge alla frequenza in tl(); le perdite al contorno sono a 1 kHz. Nelle zone dove non arriva
// nessun raggio la perdita e' al piu' la sferica + 25 dB (diffrazione e diffusione).
struct CampoPerdita {
    double zs_m = 0.0, dr_m = 500.0, dz_m = 10.0, r_max_m = 0.0;
    int nr = 0, nz = 0;
    std::vector<float> tl_db;                  // senza assorbimento; riga per distanza
    // sotto la frequenza di taglio del condotto superficiale (taglio_condotto_hz) i raggi che non ne sono mai usciti
    // non portano energia: la stessa griglia senza di loro (vuota: nessun condotto)
    std::vector<float> tl_senza_condotto_db;
    double taglio_condotto_hz = 0.0;
    // le stesse griglie con le perdite al contorno (fondo, superficie) a 250 Hz, per le bande sotto FREQ_BASSE_HZ: sotto
    // l'angolo critico la sabbia perde 0,5 dB a 250 Hz e circa 1,8 a 1 kHz (vuote: si usa tl_db)
    std::vector<float> tl_basse_db, tl_senza_condotto_basse_db;
    static constexpr double FREQ_BASSE_HZ = 500.0;
    double temperatura = 10.0, salinita = 35.0, ph = 8.0;
    // perdita a distanza orizzontale r e quota zr, alla frequenza f; nullopt fuori dalla griglia
    std::optional<double> tl(double f_hz, double r_m, double zr_m) const;
};
struct ParametriRaggi {
    int raggi = 101;                 // ventaglio da -angolo_max a +angolo_max
    double angolo_max_gradi = 30.0;
    double passo_m = 100.0;          // passo d'arco
    double r_max_m = 100000.0, dr_m = 500.0, dz_m = 10.0;
    double quota_max_m = 1000.0;     // griglia fino al fondo o a questa quota
    // acqua bassa (fondale sotto acqua_bassa_m): un ventaglio piu' fitto (0,2 gradi), dove arriva l'energia lontano
    // (301 raggi: entro 2,5 dB da 1601 sui casi della prova, a un quinto del costo).
    // Resta largo 30 gradi: vicino alla sorgente i cammini diretti verso la superficie sono ripidi (27/10: con 15 gradi
    // una boa a 18 m e un SSN a 300 m, a 400 m l'uno dall'altra, perdevano il diretto e 20 dB)
    double acqua_bassa_m = 500.0;
    int raggi_acqua_bassa = 301;
    double angolo_acqua_bassa_gradi = 30.0;
};
CampoPerdita calcola_campo(double zs_m, const Ambiente& amb, const ParametriRaggi& par = {});

// Campi per sorgente, ricalcolati quando la sorgente si sposta (3 nm), cambia quota (30 m) o cambia l'ambiente
// (profilo, fondale del 10%, fondo, stato del mare). Il calcolo e' nello stesso passo; al piu' `limite_per_passo`
// calcoli fra due nuovo_passo(): oltre, resta il campo vecchio se c'e', altrimenti nullptr (e valuta usa il livello A).
// Il limite dipende solo dallo stato della partita, quindi le partite restano ripetibili.
class CacheCampi {
public:
    explicit CacheCampi(ParametriRaggi par = {}) : par_(par) {}
    const CampoPerdita* campo(int id, Vec2 pos, double zs_m, const Ambiente& amb);
    void nuovo_passo() { nel_passo_ = 0; }
    void dimentica(int id) { voci_.erase(id); }
    std::size_t calcoli() const { return calcoli_; }
    double soglia_spostamento_m = 3.0 * 1852.0, soglia_quota_m = 30.0;
    int limite_per_passo = 1;
private:
    struct Voce {
        CampoPerdita campo;
        Vec2 pos;
        double zs = 0.0, fondale = 0.0, vento = 0.0;
        const ProfiloSuono* profilo = nullptr;
        TipoFondo fondo = TipoFondo::sabbia;
        double hs = -1.0, ghiaccio = 0.0;
    };
    ParametriRaggi par_;
    std::map<int, Voce> voci_;
    std::size_t calcoli_ = 0;
    int nel_passo_ = 0;
};
// Campi calcolati in un thread, senza scatti e ripetibili. Una richiesta parte quando la sorgente si sposta (3 nm),
// cambia quota (30 m) o cambia l'ambiente, come in CacheCampi; il campo nuovo entra in vigore a un istante fisso di
// gioco, `ritardo_s` dopo la richiesta, e se a quell'istante il thread non ha finito lo si aspetta. Il campo in uso
// dipende quindi solo dal tempo di gioco, non dalla velocita' della macchina ne' dalla compressione del tempo. Prima
// del primo campo di una sorgente: nullptr (livello A).
class CacheCampiAsincrona {
public:
    explicit CacheCampiAsincrona(ParametriRaggi par = {}, double ritardo_s = 10.0) : par_(par), ritardo_s_(ritardo_s) {}
    // chiede un campo nuovo se serve e restituisce quello in vigore al tempo di gioco `tempo_s` (non decrescente)
    const CampoPerdita* campo(int id, Vec2 pos, double zs_m, const Ambiente& amb, double tempo_s);
    // Il campo che la partita sta usando per `id`, senza chiederne uno: per chi disegna soltanto (la cascata), che cosi'
    // non cambia il momento delle richieste e quindi la partita. nullptr se la partita non ne ha chiesto ancora nessuno
    const CampoPerdita* in_vigore(int id) const {
        if (condivisi_.count(id)) {
            const auto u = ultimi_.find(id);
            return u != ultimi_.end() ? u->second.get() : nullptr;
        }
        const auto v = voci_.find(id);
        return v != voci_.end() ? v->second.in_vigore.get() : nullptr;
    }
    // Le navi del traffico civile (le segna il runtime): un campo per chiave, condiviso. La chiave e' la quota a passi
    // di `passo_quota_m`, il profilo, il fondo, il mare e il fondale a fasce del 10% (la stessa tolleranza che vale per
    // una nave sola). Il primo che chiede una chiave fa calcolare il campo coi suoi dati; una chiave che nessuno usa da
    // `scadenza_s` si butta. Senza navi segnate la cache e' quella di prima, bit per bit
    void condividi(int id) { condivisi_.insert(id); }
    void dimentica(int id) { voci_.erase(id); condivisi_.erase(id); ultimi_.erase(id); }
    std::size_t richieste() const { return richieste_; }
    std::size_t chiavi_condivise() const { return insieme_.size(); }
    double soglia_spostamento_m = 3.0 * 1852.0, soglia_quota_m = 30.0;
    double passo_quota_m = 10.0, scadenza_s = 600.0;
private:
    struct Chiesto {   // i dati dell'ultima richiesta (per decidere la prossima)
        Vec2 pos;
        double zs = 0.0, fondale = 0.0, vento = 0.0;
        const ProfiloSuono* profilo = nullptr;
        TipoFondo fondo = TipoFondo::sabbia;
        double hs = -1.0, ghiaccio = 0.0;
    };
    struct Voce {
        std::shared_ptr<const CampoPerdita> in_vigore;
        std::shared_future<std::shared_ptr<const CampoPerdita>> in_arrivo;
        double pronto_al = 0.0;
        bool attesa = false;
        Chiesto chiesto;
    };
    void lancia(Voce& v, Vec2 pos, double zs, const Ambiente& amb, double tempo_s);
    const CampoPerdita* condiviso(int id, Vec2 pos, double zs, const Ambiente& amb, double tempo_s);
    using Chiave = std::tuple<long, const ProfiloSuono*, int, int, long, long>;   // quota, profilo, fondo, mare, fondale, ghiaccio
    struct Condivisa {
        Voce v;
        double ultimo_uso = 0.0;
    };
    ParametriRaggi par_;
    double ritardo_s_;
    std::map<int, Voce> voci_;
    std::size_t richieste_ = 0;
    std::set<int> condivisi_;
    std::map<Chiave, Condivisa> insieme_;
    std::map<int, std::shared_ptr<const CampoPerdita>> ultimi_;   // l'ultimo campo dato a ogni nave condivisa
    double prossima_pulizia_ = 0.0;
};

// Come valuta, con la perdita dal campo della sorgente (Sorgente::id) quando la distanza e' nella griglia; fuori, o
// senza campo (limite del passo), il livello A. Nell'attivo vale la reciprocita': lo stesso campo per andata e ritorno.
Rilevazione valuta(const Sorgente& s, const Ricevitore& r, const Ambiente& amb, CacheCampi& campi);
Rilevazione valuta(const Sorgente& s, const Ricevitore& r, const Ambiente& amb, CacheCampiAsincrona& campi, double tempo_s);
inline double eccesso_segnale(const Sorgente& s, const Ricevitore& r, const Ambiente& amb) { return valuta(s, r, amb).se_db; }

// probabilita' di rilevazione con la fluttuazione sigma (modello lambda-sigma, dossier 02 §1.6)
double probabilita_rilevazione(double se_db, double sigma_db);

// Fluttuazione AR(1) del segnale per una coppia sensore-sorgente (§1.6: 2-10 min nel passivo a bassa frequenza,
// 10-60 s nell'attivo): la tiene chi valuta, una per coppia, e la somma a SE
struct Fluttuazione {
    double db = 0.0;
    void avanza(double dt_s, double tau_s, double sigma_db, Casuale& caso);
};

}  // namespace m688
