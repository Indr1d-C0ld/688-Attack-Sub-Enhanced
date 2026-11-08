#pragma once
// Carico delle armi e Sala siluri (Fase 4a, M2; dossier docs/ricerca/14_carico_armi.md §5): il catalogo delle armi
// con le schede, i locali siluri per classe, i carichi standard per tipo di missione e la scelta del giocatore prima
// della partenza (deposito della missione, armi negate o datate, posti condivisi fra tubi e rastrelliera, pozzi
// verticali e scorta delle esche a parte). Vale solo nell'Enhanced: il classico parte con `dotazione` e `tubi` della
// missione e non usa niente di questo.
//
// Il modulo e' puro: non conosce la missione (i limiti arrivano in LimitiMissione) ne' la stazione Armi (il risultato
// e' un CaricoPartenza, che la stazione applica con StazioneArmi::imposta_carico).

#include <cstdint>
#include <map>
#include <set>
#include <string>
#include <vector>

namespace m688 {

enum class TipoArmaCarico : std::uint8_t { siluro, missile, mina, esca };
enum class Posto : std::uint8_t { tubo, rastrelliera, pozzi, esche };

struct SchedaArma {
    std::string id, nome, nazione;       // nazione: "usa" | "urss"
    TipoArmaCarico tipo = TipoArmaCarico::siluro;
    std::string famiglia;                // arma della stazione e del gestore di lancio: mk48, type53, harpoon, ...
    std::string siluro;                  // voce di siluri.toml per il modello dell'Enhanced ("" per missili e mine)
    std::string guida;
    double velocita_kn = 0.0, portata_km = 0.0, testata_kg = 0.0, peso_kg = 0.0, lunghezza_m = 0.0;   // 0 = ignoto
    int diametro_mm = 533;
    double posti = 1.0;                  // in rastrelliera; 0 per le esche, 0,75 per le mine sovietiche corte
    bool pozzo_verticale = false;        // puo' stare nei pozzi verticali (Tomahawk)
    int in_servizio_dal = 0;             // 0 = mai in servizio (what-if)
    int ritirata_nel = 0;                // 0 = ancora in servizio
    int massimo_primo_anno = 0;          // tetto per carico nei primi due anni di servizio (0 = nessuno)
    bool da_famiglia = true;             // prende anche dal deposito della famiglia
    bool solo_ia = false, what_if = false, nucleare = false;
    bool moderno = false;                // what-if consentita dal modo moderno della partita (MOSS sul 688), non dal 1989
    std::string sagoma;                  // "modello:<id>" | "sprite:<file>" | "icona:<nome>" per la grafica
    std::string fonte, confidenza, nota;
    // efficacia per bersaglio, 0-3 stelle (tabella della sala siluri, dossier 15 §7.5): "sottomarino" (in
    // immersione), "superficie", "terra", "mine" (posa). STIMA del gioco, con la sua nota, fonte e confidenza
    std::map<std::string, int> efficacia;
    std::string efficacia_nota, efficacia_fonte, efficacia_confidenza;
    int stelle(const std::string& bersaglio) const {
        const auto it = efficacia.find(bersaglio);
        return it == efficacia.end() ? 0 : it->second;
    }
};

struct LocaleSiluri {
    std::string id, nome, nazione;
    int tubi = 4, calibro_mm = 533, rastrelliera = 21, pozzi = 0, lanciatori_esche = 2, esche = 6, limite_in_acqua = 4;
    std::vector<std::string> posizioni;  // le posizioni della stazione Armi: "tubo", "esca", "pozzi" o "" (assente)
    std::string fonte;
};

struct CaricoStandard {
    std::vector<std::string> tubi;
    std::map<std::string, int> rastrelliera, pozzi;
};

struct CatalogoCarico {
    std::map<std::string, SchedaArma> armi;
    std::map<std::string, LocaleSiluri> locali;
    std::map<std::string, std::map<std::string, CaricoStandard>> carichi;   // locale -> tipo di missione
    std::map<std::string, std::string> predefiniti;                         // locale -> tipo senza indicazione
    std::map<std::string, std::map<std::string, std::string>> predefinite;  // nazione -> famiglia -> variante
    const SchedaArma* arma(const std::string& id) const;
    // classe del giocatore e variante della missione ("flight2" -> los_angeles_flight2); nullptr senza dati
    const LocaleSiluri* locale(const std::string& classe, const std::string& variante = "") const;
    // la variante predefinita della famiglia per la nazione (mk48 -> mk48, type53 -> set65, tomahawk -> tasm,
    // esca -> adc_mk2 o mg84); "" se manca
    std::string predefinita(const std::string& famiglia, const std::string& nazione) const;
};
CatalogoCarico carica_carico(const std::string& percorso);   // data/motore/carico.toml; std::runtime_error

// Quello che la missione dichiara (Missione::limiti_carico)
struct LimitiMissione {
    std::map<std::string, int> deposito;      // famiglie o varianti; vuoto = la dotazione dell'originale
    std::set<std::string> tipi_negati;        // famiglie o varianti
    std::string tipo_missione;                // antisom | antinave | attacco_terra | minamento | scorta | ""
    std::string variante_battello;            // flight1 | flight2 | 688i | ""
    int ospiti = 0;                           // posti tolti alla rastrelliera
    int anno = 1989;
    bool what_if = false;
    std::vector<std::string> tubi_originali;  // `tubi` dell'originale, per il carico predefinito
    std::map<std::string, int> dotazione;     // `dotazione` dell'originale, se manca il deposito
};

struct ArmaDisponibile {
    std::string id;
    int deposito = 0;          // quante ne puo' prendere in tutto (dal deposito della variante e della famiglia)
    int caricate = 0;          // gia' nel carico (tubi, rastrelliera, pozzi, esche)
    bool negata = false;
    std::string motivo;        // "in servizio dal 1988", "ritirata nel 1994", "negata dalla missione", "what-if",
                               // "solo IA", "nucleare", "non al deposito"
    int rimaste() const { return deposito - caricate; }
};

// La scelta del giocatore, per la stazione Armi (StazioneArmi::imposta_carico)
struct CaricoPartenza {
    std::vector<std::string> posizioni;          // arma per posizione del locale ("" vuota); pozzi ed esche: la
                                                 // variante che si prende per prima
    std::vector<std::string> tipi;               // LocaleSiluri::posizioni: "tubo", "esca", "pozzi" o ""
    std::map<std::string, int> rastrelliera, pozzi, esche;
    bool operator==(const CaricoPartenza&) const = default;
};

struct EsitoCarico {
    bool fatto = false;
    std::string motivo;        // "posti finiti", "non va nei pozzi", "tubo occupato", ... (vuoto se fatto)
};

// Quanto il carico si scosta dal carico standard del tipo di missione (0 uguale, 1 tutto diverso): meta' della somma
// delle differenze per arma (tubi, rastrelliera e pozzi) sul totale. Per il debriefing e la carriera (M3). 0 se il
// locale o il tipo non hanno un carico standard.
double scarto_dal_standard(const CaricoPartenza& c, const CatalogoCarico& cat, const std::string& locale,
                           const std::string& tipo);

class SalaSiluri {
public:
    // classe del giocatore della missione; `what_if_giocatore`: l'opzione del giocatore che consente le armi what-if;
    // `moderno`: il modo moderno della partita, che consente le what-if marcate «moderno» (la MOSS Mk 70 sul 688; col
    // modo 1989 restano negate anche col what-if del giocatore). std::runtime_error se il catalogo non ha il locale.
    SalaSiluri(const CatalogoCarico& c, const std::string& classe, const LimitiMissione& l,
               bool what_if_giocatore = false, bool moderno = false);

    const LocaleSiluri& locale() const { return *locale_; }
    const CatalogoCarico& catalogo() const { return *catalogo_; }
    // le armi della nazione del battello, negate comprese (con il motivo), nell'ordine del catalogo
    std::vector<ArmaDisponibile> disponibili() const;
    ArmaDisponibile disponibile(const std::string& id) const;
    const SchedaArma& scheda(const std::string& id) const;   // std::out_of_range

    // posti condivisi: tubi + rastrelliera - ospiti; i pozzi a parte (solo armi da pozzo); le esche a parte
    int posti_totali() const;
    double posti_usati() const;              // tubi occupati + rastrelliera pesata con i posti dell'arma
    double posti_liberi() const { return posti_totali() - posti_usati(); }
    int pozzi_usati() const;
    int esche_caricate() const;
    int pronti_nei_tubi() const;
    double peso_kg() const;                  // informativo (dossier 14 §3.1)

    // azioni del giocatore. `tubo` (1..tubi) solo per Posto::tubo. Una alla volta per arma.
    EsitoCarico metti(Posto p, const std::string& arma, int tubo = 0);
    EsitoCarico togli(Posto p, const std::string& arma, int tubo = 0);
    void svuota();
    // carico standard del tipo (vuoto: quello della missione, poi il predefinito del locale), adattato al deposito e
    // all'anno: un'arma mancante lascia il posto alla variante predefinita della sua famiglia, poi al siluro principale
    void carico_standard(const std::string& tipo = "");
    // dotazione e tubi dell'originale (missioni senza deposito): i missili del tubo 7 in un tubo da siluri sul
    // Flight I, nei pozzi sul Flight II; il resto in rastrelliera finche' c'e' posto
    void carico_predefinito();
    // senza deposito il predefinito, altrimenti lo standard
    void carico_iniziale();

    const CaricoPartenza& carico() const { return carico_; }
    std::vector<std::string> avvisi() const;   // "tubi vuoti: 2", "posti liberi: 3", "nessuna arma antinave", ...

private:
    int deposito_proprio(const SchedaArma& a) const;
    int deposito_famiglia(const std::string& famiglia) const;
    int caricate(const std::string& id) const;
    int dalla_famiglia(const SchedaArma& a) const;             // quelle di `a` che prendono dal deposito della famiglia
    int tetto(const SchedaArma& a) const;                      // massimo_primo_anno se vale, altrimenti un numero alto
    std::string motivo_negata(const SchedaArma& a) const;
    bool dichiarata(const SchedaArma& a) const;                // la missione la dichiara (o la sua famiglia)
    std::string principale() const;                            // il siluro predefinito della nazione
    int tubo_libero() const;                                   // 0 se nessuno
    std::vector<int> indici_tubi() const;                      // indici delle posizioni "tubo"
    int indice_pozzi() const;                                  // -1 se il locale non ha pozzi
    void aggiorna_posizione_pozzi();

    const CatalogoCarico* catalogo_;
    const LocaleSiluri* locale_;
    LimitiMissione limiti_;
    bool what_if_, moderno_, deposito_dichiarato_;
    CaricoPartenza carico_;
};

}  // namespace m688
