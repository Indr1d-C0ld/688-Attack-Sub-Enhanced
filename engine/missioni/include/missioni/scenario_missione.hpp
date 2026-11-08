#pragma once
// Scenario di missione (roadmap punto 10): la missione «come la pensa un autore», comune all'editor e alle missioni
// rapide. Teatro, data e meteo, unita' con rotte e comportamenti, zone, obiettivi M1 (docs/09 §5.2), regole «quando...
// allora...» a vocabolario chiuso, briefing. Un solo scrittore lo trasforma nel file Lua del formato 1
// (docs/09_schema_missioni.md) che il motore carica gia', con l'API classica: la missione si gioca nei due livelli.
//
// - Le missioni rapide (missioni/missione_rapida.hpp) producono uno scenario con la libreria dei comportamenti 1, la
//   stessa del 25/10/2026: i loro file restano identici byte per byte. Si riaprono rigenerandole dal codice.
// - Le missioni dell'editor usano la libreria 2 (i parametri del comportamento per unita') e portano nel file il blocco
//   dichiarativo `editor = { formato_editor, scenario = { ... } }`: leggi_scenario lo rilegge nell'ambiente isolato
//   senza eseguire gli script (il motore lo ignora).
// - verifica() elenca i problemi (unita' sulla terra, rotte che la attraversano, obiettivi senza unita'...).
//
// Lo scrittore e' deterministico: lo stesso scenario da' lo stesso testo, e scrivere uno scenario riletto da' lo stesso
// file.
#include <array>
#include <cstdint>
#include <filesystem>
#include <map>
#include <optional>
#include <string>
#include <vector>

#include "nucleo/geometria.hpp"

namespace m688 {

class Fondale;

using TestiScenario = std::array<std::string, 2>;   // italiano, inglese

// --- dati dello scrittore ---------------------------------------------------------------------------------------------

// Modello del livello classico di una classe (data/motore/missioni_rapide.toml, [modelli])
struct ModelloClassico {
    double massima = 20.0, virata = 3.6, accel = 0.257, quota_m_s = 1.0, quota_max = 457.2;
    int integrita = 100, rumore = 10, sonar = 18;
    std::optional<int> difesa;
    bool schiera = false, fondale = false;
    int periodo = 1;
};

// Comandi del giocatore e lanci delle armi di una parte (data/motore/missioni_rapide/kit_<parte>.lua): i campi delle
// tabelle `gestori_giocatore` e `armi` e gli script, gia' come testo Lua
struct KitScenario {
    std::string gestori, armi, script;
};

struct DatiScenario {
    std::map<std::string, ModelloClassico> modelli;   // per classe; «giocatore_<classe>» per il battello del giocatore
    std::array<KitScenario, 2> kit;                    // usa, urss
    std::map<std::string, double> pescaggi;            // da classi.toml, per la verifica
    TestiScenario esito_fine, esito_tempo;
};
// data/motore: missioni_rapide.toml, missioni_rapide/kit_*.lua, classi.toml. Lancia std::runtime_error.
DatiScenario carica_dati_scenario(const std::string& cartella_dati);

// --- lo scenario -------------------------------------------------------------------------------------------------------

struct CerchioScenario {
    double x = 0.0, y = 0.0, r = 0.0;
};

// Parametri del comportamento di un'unita' (libreria 2): senza valore vale quello della missione
struct ParametriComportamento {
    std::optional<int> paranoia;       // 0-3: ping, tracce medie, avvicinamento, allarme
    std::optional<int> salve;          // lanci di un sottomarino
    std::optional<double> portata_nm;  // armi antisom delle navi, lancio dei sottomarini
    std::optional<double> distanza_nm; // pedinamento: la distanza da tenere dal bersaglio (predefinita 2 nm)
};

// Un sonar attivo speciale (chiave d'unita' `sonar_speciale`, Enhanced con l'acustica; docs/09 §4, scheda I09): al ping
// prende il posto del sensore attivo della classe; con ogni_s > 0 pinga da solo
struct SonarSpecialeScenario {
    double sl_db = 0.0, frequenza_hz = 0.0, impulso_s = 0.0, risoluzione_s = 0.0, ogni_s = 0.0, dimensione_m = 0.0;
    std::string antenna{};             // sfera (vuoto), lineare o scafo
};

struct UnitaScenario {
    int id = 0;
    std::string classe, categoria, parte, ruolo;   // categoria: sottomarino, nave, aereo; parte: usa, urss, neutrale
    int flag = 0;
    bool giocatore = false, non_combattente = false, nascosta = false;   // nascosta: non nominata nel briefing
    Vec2 pos;
    double quota = 0.0, rotta = 0.0, velocita = 0.0;
    std::vector<Vec2> punti;           // rotta: il primo punto parte con la missione, gli altri all'arrivo
    bool ciclica = false, esce = false;   // a fine rotta: ricomincia, esce dal teatro, altrimenti si ferma
    std::string comportamento;         // R_nave, R_sottomarino, R_aereo, R_pedina (libreria 2); vuoto: solo la rotta
    double profonda = 0.0;             // quota d'evasione dei sottomarini
    ParametriComportamento parametri;
    std::string entra_con;             // id di una regola: l'unita' aspetta fuori dal teatro e entra quando scatta
    std::optional<double> batteria;    // diesel dell'IA (Enhanced, M6): la carica iniziale 0..1; a 0,05 emerge per forza
    std::string stato;                 // M6: vuoto, "avaria" (senza propulsione) o "incagliata" (ferma finche' la rimorchiano);
                                       // "relitto": affondata, ferma sul fondo e muta (I10)
    std::optional<SonarSpecialeScenario> sonar_speciale;   // un sonar attivo diverso da quello della classe (Enhanced)
    bool armi_esercitazione = false;   // le armi che lancia sono inerti: colpiscono senza danni (I03, S01)
    std::string emcon{};               // l'EMCON di partenza (Enhanced): silenzio, navigazione, ricerca, intermittente, ingaggio;
                                       // vuoto: quello della parte. Gli eventi possono solo alzarlo (S13: i simulatori radar)
    std::string luci{};                // le luci di navigazione (Enhanced, docs/27): moto, fonda, pesca, manovra_limitata,
                                       // non_governa, spente; vuoto: le sceglie il motore (luci::stato_predefinito)
    std::string cisterne{};            // le petroliere (danni critici, T7): zavorra, zavorra_inerte; vuoto: cariche
};

struct ObiettivoScenario {
    std::string id, tipo;              // tipi M1: affondare, proteggere, raggiungere, scoprire, non_essere_scoperto, contatto, ...
    std::vector<int> unita;
    int minimo = -1;
    std::optional<CerchioScenario> cerchio;
    double entro_s = -1.0;
    bool primario = true, colpire = true;
    TestiScenario testo;
    int indice = -1;                   // variabile: la variabile d'autore (300 + indice), raggiunto con almeno `minimo`
    double minuti = 0.0;               // contatto: minuti di contatto sonar visibile, cumulativi, con ognuna delle unita';
                                       // zona_inviolata: il tempo che un'unita' puo' restare dentro
    double distanza_m = 0.0;           // foto: entro questa distanza; raggiungere senza cerchio: da un'unita' in moto
    std::string parte{};               // zona_inviolata: ostile (vuoto), amica o neutrale rispetto al giocatore
    std::string missione{};            // tlam: la missione della libreria (vuota: qualunque); `minimo` i TLAM arrivati
    bool finestra = true;              // tlam: conta solo l'arrivo entro tot +- finestra
    std::string massima{};             // scatola: la taglia di ricerca piu' grande permessa (piccola, media, grande)
    double errore = 0.0;               // telemetro: l'errore relativo massimo della distanza letta (0,1 = 10%);
                                       // rotta_stimata: l'errore massimo sulla rotta vera, in gradi (docs/25, I13 e I14)
    bool da_tma = false;               // rotta_stimata: dalla TMA al MATE invece che dall'AOB al periscopio; `minimo` stime,
                                       // anche della stessa unita', ad almeno 5 minuti l'una dall'altra
};

struct AereoAswScenario {             // una voce di `aereo_asw` (docs/09 §5.4)
    int unita = 0, base_unita = -1;
    std::optional<Vec2> base;
    std::string situazione = "guerra";
    bool contro_giocatore = true;      // bersagli = "giocatore", altrimenti `bersagli`
    std::vector<int> bersagli;
    Vec2 zona;
    double raggio = 20000.0;
};

// La chiusura principale (codice 120): il controllo ogni 20 s sul battello del giocatore
struct ChiusuraScenario {
    enum class Tipo : std::uint8_t {
        solo_tempo,          // nessuna condizione: chiude il tempo (121)
        unita_finita,        // la prima di `unita` non e' piu' in gioco (affondata o uscita)
        unita_finite,        // nessuna di `unita` e' piu' in gioco
        dentro_poi_fuori,    // il giocatore entra in `zona`, poi esce da `fuori`
        dentro_poi_dentro,   // il giocatore entra in `zona`, poi in `fuori`
    };
    Tipo tipo = Tipo::solo_tempo;
    std::vector<int> unita;
    CerchioScenario zona, fuori;       // il controllo usa il quadrato circoscritto (in_rettangolo)
};

struct ZonaScenario {                 // per le regole: un cerchio (in_cerchio dell'API), che una regola puo' spostare
    std::string id;
    TestiScenario nome;
    CerchioScenario cerchio;
    // la sala radio la dichiara al giocatore (Enhanced, R2; docs/18 fase 3): azione (la sua area d'azione: navi e aerei
    // amici non vi usano armi antisom), libera_asw, protetta; vuoto: solo per le regole. Con `da_messaggio` non c'e'
    // all'inizio: la porta un messaggio (MessaggioScenario::zone_aggiungi)
    std::string radio{};
    bool da_messaggio = false;
};

struct CondizioneScenario {
    enum class Tipo : std::uint8_t {
        tempo,                 // trascorsi almeno `valore` s (il primo controllo della regola)
        unita_colpita,         // `unita` colpita o in affondamento
        unita_fuori,           // `unita` non piu' in gioco: affondata o uscita
        giocatore_in_zona, giocatore_fuori_zona,
        unita_in_zona,         // `unita` dentro `zona` (la regola gira su quell'unita')
        allarme,               // un'unita' dell'IA ha dato l'allarme (variabile 11)
        giocatore_ha_lanciato, // variabile 1 del kit
        variabile,             // variabile `indice` `confronto` (=, >, <) `valore`
        regola_scattata,       // la regola `regola` e' gia' scattata
        // dalle rapide v2 del runtime (data/missioni/API.md, «Condizioni delle missioni nuove»)
        giocatore_sotto_nodi,  // il giocatore va a meno di `valore` nodi
        giocatore_a_quota,     // il giocatore e' fra `valore` e `fino_a` metri
        giocatore_in_superficie,
        giocatore_alberi_alzati,   // periscopio o antenna
        obiettivo_compiuto,    // l'obiettivo `obiettivo` e' raggiunto ora
        contatto_su,           // il giocatore ha avuto almeno `valore` minuti di contatto visibile su `unita`
        senza_contatto_su,     // il giocatore non vede `unita` da almeno `valore` minuti
        ping_vicino,           // il giocatore ha fatto ping entro `valore` m da `unita` (negli ultimi `fino_a` s, se > 0)
        ostile_ha_lanciato,    // `unita` ha lanciato un'arma; con -1 una qualunque ostile al giocatore
        unita_vede,            // `unita` ha avuto almeno `valore` minuti di contatto visibile su `bersaglio`
        unita_non_vede,        // `unita` non vede `bersaglio` da almeno `valore` minuti
        vicino_al_cavo,        // il giocatore e' a meno di `valore` m dal cavo della schiera di `unita` (Enhanced)
        unita_in_superficie,   // `unita` e' in superficie (un diesel emerso per la batteria)
        giocatore_sopra_nodi,  // il giocatore va ad almeno `valore` nodi
        unita_in_avaria,       // `unita` e' in avaria o incagliata (M6)
        unita_rimorchiata,     // `unita` e' a rimorchio (M6)
        giocatore_ha_trasmesso,   // il giocatore ha fatto una trasmissione `trasmissione` dopo `valore` s (m688.trasmesso)
        evento,                // nel registro c'e' un evento `evento` (nome_evento) dopo `valore` s (m688.evento)
        unita_classifica,      // il sonarista di `unita` ha classificato `bersaglio`, o il suo contatto e' lungo
                               // (m688.classificato; Enhanced, nel classico falsa) (S10). `valore`: la confidenza minima
                               // (0 NONSUB, 1 POSSUB, 2 PROBSUB, 3 CERTSUB), il terzo argomento di m688.classificato
        giocatore_vicino_a,    // il giocatore e' a meno di `valore` m da `unita` (m688.distanza_da, la posizione vera) (S14)
    };
    Tipo tipo = Tipo::tempo;
    double valore = 0.0;
    int unita = -1, indice = 0;
    std::string zona, regola, confronto = "=";
    double fino_a = 0.0;
    std::string obiettivo;
    int bersaglio = -1;                // unita_vede, unita_non_vede, unita_classifica: l'unita' osservata; -1 il giocatore
    std::string trasmissione{};        // giocatore_ha_trasmesso: contatto, attacco, situazione, conferma, richiesta, previsione
    std::string evento{};              // evento: il nome dell'evento del registro (incaglio, collisione, colpo_esercitazione, ...)
};

struct AzioneScenario {
    enum class Tipo : std::uint8_t {
        messaggio,             // messaggio di bordo `codice` (messaggi_bordo.tsv); con `testo`, uno proprio della missione (1000 + n)
        fine,                  // esito 100 + `codice` col testo; `categoria` facoltativa (docs/09 §5.2)
        variabile,             // variabile `indice` `operazione` (=, +, -) `valore`
        comportamento,         // `unita` passa al comportamento `comportamento`
        vai_a,                 // `unita` va al punto `punto`
        entra,                 // `unita` di riserva entra in scena nella sua posizione
        cue_asw,               // datum per gli aerei ASW sul punto, raggio `raggio`
        allarme,               // accende l'allarme (variabile 11)
        sposta_zona,           // la zona `zona` (per le regole) si sposta sul punto `punto`
        radio,                 // il messaggio radio `zona` (il suo id) va in onda subito (m688.radio, E6)
        situazione,            // le armi dell'IA passano a `comportamento` (pace, crisi, guerra)
        posa_mina,             // `unita` posa `valore` mine `comportamento` (mk67, mdt...) sotto di se', una ogni `codice` s
        avaria,                // `unita` va in avaria (valore 0: riparata) (M6)
        rimorchia,             // `unita` prende a rimorchio `bersaglio` a `velocita` nodi (6), cavo di `valore` m (300) (M6)
        molla_rimorchio,       // `unita` molla il rimorchio (M6)
        esercitazione,         // le armi di `unita` (o l'arma stessa) diventano inerti (valore 0: di nuovo vere) (I03, S01)
        attacca_unita,         // `unita` attacca `bersaglio` ignorando parte, ROE e non combattenti; `valore` 1: anche senza
                               // soluzione, sul rilevamento e la distanza veri (il tiro pianificato d'esercitazione) (I03, I05)
        lancia_missile,        // `unita` lancia `valore` missili antinave (1) col modello su `bersaglio` (-1: il giocatore),
                               // pianificati sulla posizione che ne ha (I11)
        quota,                 // `unita` va alla quota di `valore` m col rateo del modello (m688.quota): un sottomarino che
                               // si immerge o risale (S14)
        rumore,                // `unita` fa un rumore transitorio di 11 s (m688.rumore_transitorio): una bomba a mano in
                               // acqua, una granata di segnalazione (S14, S15)
        comanda_mine,          // le mine comandate da terra del gruppo `zona` (vuoto: tutte) passano alla stazione, che le fa
                               // scoppiare sotto un sottomarino (m688.comanda_mine; S18)
        togli,                 // `unita` esce dal gioco senza essere distrutta (m688.togli_unita, l'opcode 37h): un battello che
                               // affonda fuori dalla vista del giocatore (S23)
        salvati,               // il soccorso: `valore` uomini salvati sugli `indice` che erano in acqua (m688.salvati: l'achievement
                               // «tutti salvati» e, in pace, la carriera; l'ultima chiamata vince) (S23)
    };
    Tipo tipo = Tipo::messaggio;
    int codice = 0, unita = -1, indice = 0, valore = 0;
    std::string comportamento, operazione = "=", categoria, zona;
    Vec2 punto;
    double raggio = 1852.0;
    TestiScenario testo;
    double velocita = -1.0;            // vai_a: la nuova velocita' di crociera in nodi; negativa: quella di prima
    int bersaglio = -1;                // rimorchia: l'unita' rimorchiata
    std::string fra_a{}, fra_b{};      // situazione: fra due parti (usa, urss, luogo); vuote: fra tutte (M7)
};

struct RegolaScenario {
    std::string id;
    std::vector<CondizioneScenario> quando;   // tutte vere
    std::vector<AzioneScenario> allora;
    bool una_volta = true;
};

struct VoceCarico {
    std::string arma;
    int numero = 0;
};
struct CaricoScenario {               // docs/09 §5.3, nell'ordine in cui va scritto
    std::vector<VoceCarico> dotazione, deposito;
    std::vector<std::string> tubi;
    std::string tipo_missione = "antisom";
    int ospiti = 0;
};

struct CarrieraScenario {             // il blocco `carriera` (docs/09 §5.3)
    std::string situazione = "pace", avversario = "regolare";
    int difficolta = 3;
    bool operazione_speciale = false, artico = false;
};

// Da dove viene uno scenario delle missioni rapide: il codice e i parametri risolti, per l'intestazione e la chiave
// `rapida` del file
// --- radio (docs/09 §5.1, fase R3 ed E6) e meteo fissato (§5.1b) ---

// regole d'ingaggio per categoria (sottomarini, navi, mercantili, neutrali, aerei): libero, stretto, trattenuto
using RoeScenario = std::map<std::string, std::string>;

struct MessaggioScenario {
    std::string id;
    double dopo_s = 0.0;               // in onda da qui in poi (con `se`: non prima)
    bool su_richiesta = false;         // solo da una regola (azione «radio»)
    std::string precedenza = "routine", da, oggetto, classifica = "CONFIDENTIAL";
    TestiScenario testo;
    bool autentico = true;             // falso: un inganno, il gruppo d'autenticazione non torna
    RoeScenario roe;                   // le ROE che cambia
    std::string se_evento, se_ricevuto;   // condizioni facoltative: un evento del registro, un messaggio ricevuto
    std::vector<int> se_unita_colpita;
    std::string annulla;               // id del messaggio che questo supera («disregard»)
    std::vector<int> contatto;         // un contatto d'intelligence su queste unita' (vuoto: nessuno)
    double contatto_errore_m = 5000.0;
    std::string contatto_sensore = "sosus";
    std::vector<std::string> zone_aggiungi{}, zone_togli{};   // zone della radio (ZonaScenario::radio) dichiarate o tolte
    std::string esecuzione_tlam{};     // l'ordine d'esecuzione della missione TLAM con questo id (docs/21 §5)
};

struct OrdiniScenario {                // l'intestazione degli ordini di missione (serie 0); il corpo e' il briefing
    std::string da, a, oggetto, classifica = "SECRET";
};

struct RadioScenario {
    std::optional<OrdiniScenario> ordini;   // la data del DTG e' quella della missione
    RoeScenario roe;                   // ROE di partenza
    std::vector<MessaggioScenario> messaggi;
};

struct MeteoFissoScenario {            // fermo per tutta la missione, anche col meteo semplificato
    std::optional<double> vento_m_s, vento_da_gradi, stato_mare, visibilita_m, pioggia_mm_h, neve_mm_h, nubi_ottavi;
    std::string nebbia;                // nessuna, avvezione, fumo_di_mare, radiazione, precipitazione; vuoto: del teatro
};

// I danni del battello del giocatore alla partenza (chiave della missione `danni_iniziali`, Enhanced con la regola di
// simulazione; docs/09 §4, scheda I07)
struct UrtoIniziale {
    double tnt_kg = 0.0, distanza_m = 0.0, angolo_gradi = 0.0;
};
struct VoceDanno {                     // un compartimento (nome o uso dello scafo: deposito, reattore, macchine...)
    std::string compartimento;
    double quanto = 0.0;               // acqua: m3; falle: area_m2; incendi: intensita'
};
struct DanniInizialiScenario {
    std::optional<UrtoIniziale> urto;
    std::map<std::string, double> sistemi;   // efficienza 0..1, coi nomi di nucleo/battello
    std::vector<VoceDanno> acqua, falle, incendi;
};

// M7, la parte del luogo (Enhanced): le unita' con `parte = "luogo"` (schieramento della missione) e la situazione di
// partenza fra il luogo e le altre due parti (chiave della missione `situazione_luogo`); senza, il luogo e' in pace con tutti
struct SituazioneLuogo {
    std::string usa = "pace", urss = "pace";
};

// La navigazione realistica del giocatore (chiave della missione `navigazione`, docs/19): l'errore di partenza
struct NavigazioneScenario {
    double sigma_iniziale_nm = 0.0;
    bool realistica_di_serie = false;   // il menu propone la navigazione realistica (il giocatore puo' toglierla; LIB-I16)
};
// Una missione TLAM della libreria di bordo (chiave della missione `tlam`, docs/21 §11.8; nucleo/missili MissioneTlam):
// tot_s dall'inizio della missione
struct TlamScenario {
    std::string id, bersaglio;
    Vec2 plp, landfall;
    double tot_s = 0.0, finestra_s = 600.0, tratta_terra_m = 0.0;
};

// Una mina gia' posata all'inizio (Enhanced, docs/09 §5.1c): un tipo di carico.toml sul fondo nel punto dato
struct MinaScenario {
    std::string tipo = "mk67";
    Vec2 pos;
    std::string lato = "urss";
    bool armata = true;
    bool sicura = false;   // in sicurezza: inerte per sempre, ma i sonar da mine la vedono (runtime, 30/10/2026; I17)
    bool comandata = false;   // comandata da terra: in sicurezza finche' m688.comanda_mine non la passa alla stazione (S18)
    std::string gruppo{};     // il gruppo delle comandate (vuoto: nessuno); m688.comanda_mine(u, gruppo)
};

struct OrigineRapida {
    int versione = 1;
    std::string codice, tipo;
    std::uint32_t seme = 0;
    int forze = 0, paranoia = 0, difficolta = 0;
    bool asw_aereo = false, meteo_realistico = false, carriera_richiesta = false;
};

struct ScenarioMissione {
    std::string id;
    TestiScenario titolo;
    std::string autore;
    std::string teatro;
    bool alfa = false;                 // il giocatore: false 688 (lato usa), true Alfa (lato urss)
    double centro_lat = 0.0, centro_lon = 0.0;   // centro del teatro (sole e meteo)
    int giorno = 1, mese = 1, anno = 1989;
    double ora_utc = 0.0;
    bool meteo_realistico = false;     // lo applica l'app (Missione::usa_meteo)
    std::string situazione = "guerra"; // pace, crisi, guerra: armi dell'IA e testo delle ROE
    int paranoia = 1, difficolta = 3;
    double durata_s = 3.0 * 3600.0;
    CaricoScenario carico;
    std::optional<CarrieraScenario> carriera;
    std::vector<UnitaScenario> unita;  // la prima e' il giocatore
    std::vector<ObiettivoScenario> obiettivi;
    std::vector<AereoAswScenario> aerei_asw;
    std::array<std::vector<std::string>, 2> briefing;
    TestiScenario esito_fine, esito_tempo;
    ChiusuraScenario chiusura;
    std::vector<ZonaScenario> zone;
    std::vector<RegolaScenario> regole;
    std::optional<OrigineRapida> rapida;
    std::optional<RadioScenario> radio;   // la sala radio dell'Enhanced (chiave `radio` della missione)
    std::optional<MeteoFissoScenario> meteo_fisso;
    std::vector<MinaScenario> mine;    // un campo gia' posato (Enhanced)
    std::optional<DanniInizialiScenario> danni_iniziali;   // il battello del giocatore parte danneggiato (Enhanced)
    std::optional<SituazioneLuogo> situazione_luogo;  // M7: la situazione di partenza col luogo (Enhanced)
    std::optional<NavigazioneScenario> navigazione;   // l'errore di partenza della navigazione stimata (Enhanced)
    std::vector<TlamScenario> tlam;    // la libreria di bordo dei TLAM (Enhanced)
    bool esercitazione = false;        // tutte le armi della missione sono inerti (I03, S01)
    int libreria = 2;                  // comportamenti: 1 quella delle rapide v1, 2 coi parametri per unita'
};

inline constexpr int FORMATO_EDITOR = 1;

// Il file Lua del formato 1. Per le missioni dell'editor (senza `rapida`) anche il blocco `editor` con lo scenario.
// Lancia std::invalid_argument se lo scenario non si puo' scrivere (per esempio senza giocatore).
std::string scrivi_missione_lua(const ScenarioMissione& s, const DatiScenario& d);

// Lo scenario dal blocco `editor` di un file (testo Lua), nell'ambiente isolato e senza chiamare gli script; nullopt se il
// blocco manca (missioni originali e rapide: queste si riaprono dal codice). Lancia std::runtime_error se il file non
// si carica.
std::optional<ScenarioMissione> leggi_scenario(const std::string& testo_lua);

// «Nuova da una missione originale»: lo scenario di una delle 22 missioni convertite (`cartella_missioni`/<id>.lua),
// nella variante piu' probabile. Ne riprende teatro, parte, unita' (posizioni, quote, rotte), dotazione, tubi, briefing e
// gli obiettivi di data/missioni/obiettivi/<id>.lua che non dipendono dagli script; i comportamenti diventano quelli
// dell'editor (libreria 2). La logica degli script originali non si riprende: `note` lo dice, con le unita' e gli
// obiettivi lasciati fuori. Lancia std::runtime_error se il file manca o non si legge.
ScenarioMissione scenario_da_originale(const std::string& cartella_missioni, const std::string& id, const std::string& cartella_dati,
                                       std::vector<std::string>* note = nullptr);

// Le missioni dell'editor: la cartella accanto ai fascicoli (nucleo/fascicolo.hpp), «missioni_editor», e l'elenco dei file
// che hanno il blocco `editor` (gli altri si saltano), in ordine di nome
std::filesystem::path cartella_missioni_editor();
struct ScenarioInElenco {
    std::filesystem::path file;
    std::string id, autore, teatro;
    TestiScenario titolo;
    bool alfa = false;
};
std::vector<ScenarioInElenco> elenco_scenari(const std::filesystem::path& cartella);

// --- verifica ----------------------------------------------------------------------------------------------------------

struct AvvisoScenario {
    enum class Gravita : std::uint8_t { errore, attenzione };
    Gravita gravita = Gravita::errore;
    std::string codice;                // chiave stabile: «terra», «rotta_su_terra», «senza_giocatore», ...
    std::string testo;                 // in italiano, per la schermata
    int unita = -1;                    // l'unita' da mostrare, se c'e'
    std::string riferimento;           // id dell'obiettivo, della zona o della regola
};
// Errori: lo scenario non si scrive o non si gioca. Attenzioni: si gioca, ma probabilmente non come voluto.
std::vector<AvvisoScenario> verifica(const ScenarioMissione& s, const Fondale& f, const DatiScenario& d);

}  // namespace m688
