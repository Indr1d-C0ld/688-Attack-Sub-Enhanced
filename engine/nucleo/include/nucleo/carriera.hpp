#pragma once
// Carriera del comandante (Fase 4a, M3; dossier docs/ricerca/13_carriera_gradi_decorazioni.md §2.3, §3.7, §5): il
// punteggio di missione, i gradi USN e VMF con la carriera compressa, le commissioni di selezione e le attestazioni
// simulate, la rimozione dal comando, l'inchiesta e la morte con tre livelli d'indulgenza, le decorazioni con le
// soglie filtrate per pace, crisi e guerra, le motivazioni.
//
// Il modulo e' puro e deterministico: nessun dado, a parita' di partite la stessa carriera. Gli ingressi sono un
// riassunto della partita (EsitoPerCarriera, che l'adattatore missioni/carriera_missione ricava dal registro della
// missione) e il contesto della missione; il fascicolo (nucleo/fascicolo.hpp) e' lo stato che passa da una missione
// all'altra. Vale solo con la regola di simulazione: il classico tiene il rapporto d'esito dell'originale.

#include <cstdint>
#include <map>
#include <optional>
#include <string>
#include <vector>

#include "nucleo/fascicolo.hpp"

namespace m688 {

enum class Situazione : std::uint8_t { pace, crisi, guerra };
std::string_view nome_situazione(Situazione s);
std::optional<Situazione> situazione_da_nome(std::string_view s);

// Riassunto della partita per il punteggio (dal registro della M1; conti senza pesi: i pesi sono qui)
struct EsitoPerCarriera {
    double riuscita = 0.0, completezza = 0.0;           // 0..1, pesati sugli obiettivi primari e secondari
    int classificazioni_giuste = 0, classificazioni_errate = 0;
    double anticipo_medio_s = 0.0;
    int rapporti_richiesti = 0, rapporti_inviati = 0;
    std::map<std::string, int> esposizioni;              // ping, fix, avvistamento, esm, transitorio, boa
    int ordini = 0, ordini_eseguiti = 0, serie_copiate = 0, serie_perse = 0, infrazioni_roe = 0;
    int lanci = 0, colpi = 0, armi_incoerenti = 0;
    double scarto_carico = 0.0;                          // 0..1 dal carico standard (M2)
    double danni = 0.0;                                  // 0..1 del battello
    int feriti = 0, morti = 0, armi_perse = 0;
    bool battello_perso = false, equipaggio_salvo = false, comandante_ferito = false;
    int neutrali_colpiti = 0, amici_colpiti = 0, fuoco_senza_ordine = 0, zone_vietate = 0;
    bool incaglio = false, danno_nucleare = false, salvataggio_a_bordo = false, attaccato = false;
    bool soccorso = false;                               // uomini in mare salvati (m688.salvati della missione)
    double distanza_min_siluro_m = 1e9, tempo_contatto_asw_s = 0.0, margine_collasso = 1.0;
    // acque territoriali straniere dichiarate dalla missione: le entrate, il tempo dentro, se ci hanno scoperti dentro
    // (avvistati, un fix, un ping, l'avvertimento o il fuoco delle navi)
    int acque_territoriali = 0;
    double tempo_in_acque_s = 0.0;
    bool scoperto_in_acque = false;
    std::vector<int> eventi_chiave;                      // id degli eventi del registro (fatti della motivazione)
    std::vector<std::string> fatti_it, fatti_en;         // le loro descrizioni, una frase ciascuna
};

// La missione per la carriera (blocco `carriera` della missione o tabella delle originali in carriera.toml)
struct ContestoMissione {
    std::string missione;                // id: SCENE1A, nome del file, ...
    std::string origine = "originale";   // originale | rapida | editor | libreria (come editor: col blocco `carriera`)
    std::uint64_t seme = 0;              // missioni rapide: il seme registrato (0 = nessuno)
    bool modificata = false;             // rapida modificata dopo la generazione
    bool ripresa = false;                // ripresa da un salvataggio («Riprendi da qui»): non conta, nemmeno negli
                                         // achievement globali (lo imposta l'app)
    bool blocco_carriera = false;        // la missione ha il blocco `carriera` (rapide ed editor)
    Situazione situazione = Situazione::pace;
    int avversario = 1;                  // 0 impreparato, 1 regolare, 2 veterano, 3 elite (dichiarato)
    int avversario_calcolato = 3;        // dalle unita' della missione: vale il minore
    int difficolta = 3, difficolta_calcolata = 5;   // 1-5; vale la minore
    bool operazione_speciale = false, artico = false;
    double realismo = 1.0;               // 1 senza aiuti, fino a 0,5 con tutti gli aiuti accesi
    // l'esito della missione: un fuoco senza provocazione secondo gli script (le originali senza sala radio,
    // carriera.toml `esiti_fuoco_senza_ordine`), e la categoria «richiamato» (il comandante richiamato in porto)
    bool fuoco_senza_ordine = false, richiamato = false;
    std::string battello, teatro;        // per le motivazioni («Los Angeles», «Mare di Norvegia»)
    std::string teatro_en;
};

struct Punteggio {
    double riuscita = 0, completezza = 0, discrezione = 0, ordini = 0, efficienza = 0, danni = 0, violazioni = 0;
    double m_situazione = 1, m_avversario = 1, m_realismo = 1, pericolo = 0, moltiplicatore = 1;
    double totale = 0;                   // (R + C + S + O + E + D + V) x M, mai sotto zero
    bool squalifica = false;
    std::string giudizio_it, giudizio_en;   // la riga della nota caratteristica / attestazione
};

struct Righe { std::string colore; int larghezza = 0; };   // una riga del nastrino (in trentaseiesimi)
struct Decorazione {
    std::string id, marina, nome, nome_it, tipo;   // tipo: personale | reparto | servizio | distintivo
    int precedenza = 99;
    std::vector<Righe> righe;
    bool orizzontale = false;
    std::string motivo, con, successiva, grado_max;
    std::vector<std::string> dispositivi;
    int anni = 0;
    std::string confidenza, fonte;
};
struct Grado { std::string id, sigla, nome, nome_it, nato; int ordine = 0; };
struct Fase {
    std::string grado, incarico, incarico_en, uscita, qualifica;   // uscita: missioni | qualifica | commissione | fine
    int missioni = 0;
    double media_minima = 0, in_zona = 0, sotto_zona = 0;
};
struct Soglia { double punti = 0; std::string decorazione, dispositivo, condizione; };

struct RegoleCarriera {
    std::map<std::string, double> punteggio, esposizione, situazione, avversario;
    double realismo_min = 0.5;
    int mesi_per_missione = 2, mesi_al_comando = 3, media_su = 6, missioni_fra_commissioni = 3;
    int mancate_per_congedo = 2, anni_pensione = 20, mediocri_per_trasferimento = 3;
    double soglia_mediocre = 150, zampolit = 0.10;
    std::vector<double> rendimenti, ripetizione;
    std::string data_inizio = "1989-06";
    int anno_nomina_breve_usn = 1973, anno_nomina_breve_vmf = 1978;
    std::map<std::string, std::vector<Grado>> gradi;                    // marina -> in ordine
    std::map<std::string, std::vector<Fase>> fasi;
    std::map<std::string, Decorazione> decorazioni;
    std::map<std::string, std::vector<Soglia>> soglie;                  // "usn.pace", "usn.guerra", ...
    std::map<std::string, std::vector<Soglia>> reparto, fine_comando;   // marina ->
    std::map<std::string, std::string> coraggio;
    struct Livello { double punti; std::string it, en; };
    std::vector<Livello> livelli;
    struct Voce { std::string forte_it, debole_it, forte_en, debole_en; };
    std::map<std::string, Voce> voci;
    std::map<std::string, std::map<std::string, std::string>> testi;    // motivazioni: marina -> chiave -> testo
    std::map<std::string, std::pair<std::string, std::string>> formule; // decorazione -> (chi conferisce, formula)
    struct Originale {
        Situazione situazione;
        int avversario, difficolta;
        bool speciale, artico;
        std::vector<int> esiti_fuoco;    // codici d'esito che dicono un fuoco senza ordine
    };
    std::map<std::string, Originale> originali;

    const Grado* grado(Marina m, const std::string& id) const;
    const Decorazione* decorazione(const std::string& id) const;
};
RegoleCarriera carica_carriera(const std::string& percorso);   // data/motore/carriera.toml; std::runtime_error
int avversario_da_nome(std::string_view s);                    // impreparato 0 ... elite 3; -1 se ignoto

Punteggio punteggio_missione(const EsitoPerCarriera& e, const ContestoMissione& c, const RegoleCarriera& r);

// Cio' che le schermate mostrano dopo il debriefing, nell'ordine
struct EsitoCarriera {
    bool contata = false;                // la missione conta per la carriera
    Punteggio punteggio;
    double peso = 0.0;                   // rendimenti decrescenti: 1 / 0,5 / 0,25 / 0
    double punti = 0.0;                  // punteggio x peso: quello che vale per soglie e medie
    std::optional<Conferimento> personale, reparto;
    std::vector<Conferimento> automatiche;          // servizio, distintivi, fine comando, Ordine di Lenin dell'Eroe
    std::optional<Commissione> commissione;
    std::optional<std::string> promozione;          // nuovo grado
    std::optional<Provvedimento> provvedimento;     // con la commissione d'inchiesta
    std::optional<Qualifica> qualifica;
    bool fine_carriera = false;
    std::string stato;                   // servizio | congedo | pensione | rimosso | trasferito | morto
};

struct VoceBarretta { std::string decorazione, dispositivo; int numero = 1; };

class Carriera {
public:
    Carriera(const RegoleCarriera& r, Fascicolo f) : regole_(&r), f_(std::move(f)) {}
    // Un fascicolo nuovo: dal grado piu' basso, o «carriera breve» dal comando (CDR o kapitan 2-go ranga) con un
    // fascicolo precompilato coerente con i percorsi del dossier (§1.2, §1.6). `aperta`: la carriera delle missioni
    // dell'editor, separata da quella ufficiale.
    static Fascicolo nuovo(const RegoleCarriera& r, Marina m, const std::string& nome, bool carriera_breve,
                           Indulgenza i, bool aperta = false);

    bool conta(const ContestoMissione& c) const;
    double peso(const ContestoMissione& c) const;   // rendimenti decrescenti sulle missioni gia' fatte
    // Chiude una missione: punteggio, provvedimenti, decorazioni, commissioni e promozioni; aggiorna il fascicolo
    EsitoCarriera chiudi_missione(const ContestoMissione& c, const EsitoPerCarriera& e);

    const Fascicolo& fascicolo() const { return f_; }
    const Grado* grado() const { return regole_->grado(f_.marina, f_.grado); }
    const Fase* fase() const;
    std::vector<VoceBarretta> barretta() const;     // nastrini in ordine di precedenza (senza i distintivi)
    static int per_fila(Marina m) { return m == Marina::usn ? 3 : 4; }   // VMF: da verificare (dossier 13 §3.4)
    double media() const;                            // delle ultime `media_su` missioni contate

    // Motivazione di un conferimento (dossier 13 §5.1-5.2), in italiano e in inglese
    std::pair<std::vector<std::string>, std::vector<std::string>> motivazione(const Conferimento& k, const ContestoMissione& c,
                                                                              const EsitoPerCarriera& e) const;

private:
    int possedute(const std::string& decorazione) const;
    Conferimento conferisci(const std::string& decorazione, const std::string& dispositivo, const ContestoMissione& c,
                            const EsitoPerCarriera& e, bool postuma, bool con_motivazione);
    std::optional<std::string> personale(const ContestoMissione& c, const EsitoPerCarriera& e, const Punteggio& p,
                                         double punti, std::string& dispositivo) const;
    void promuovi(EsitoCarriera& out, const ContestoMissione& c, const EsitoPerCarriera& e);
    void commissione(EsitoCarriera& out, const Fase& fa, const ContestoMissione& c, const EsitoPerCarriera& e);
    void chiudi_carriera(EsitoCarriera& out, const std::string& stato);
    void automatiche(EsitoCarriera& out, const ContestoMissione& c, const EsitoPerCarriera& e);
    void avanza_calendario(int mesi);
    int anni_servizio() const;

    const RegoleCarriera* regole_;
    Fascicolo f_;
};

}  // namespace m688
