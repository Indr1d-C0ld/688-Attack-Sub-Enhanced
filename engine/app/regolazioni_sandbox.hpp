#pragma once
// La sandbox, il «vetrino» dell'IA (roadmap 32, docs/19_sandbox.md): le regolazioni prima della partita. Si arriva con
// V dalla scelta della missione, su qualsiasi missione; Invio fa partire la partita senza giocatore, S una serie su piu'
// semi (docs/19 §6), Esc torna alla scelta. Frecce su e giu' scelgono la riga, sinistra e destra cambiano il valore (anche il clic sulla riga).
// Le regolazioni per parte e gli interruttori dell'IA li applica la missione del runtime (comandante IA, docs/19 §5);
// qui solo la scelta e il disegno.
// Tab passa alla seconda pagina, gli «interventi programmati» del vetrino di Dio (docs/28 §5-6), e ritorna. Li' su e
// giu' scelgono la riga (i campi dell'esperimento, poi gli interventi), Invio scrive il campo o l'ora dell'intervento,
// sinistra e destra cambiano i semi o l'ora; N aggiunge un intervento col pannello (poi l'ora), T o Backspace toglie
// quello scelto, S salva l'esperimento, C ne carica uno (dell'utente o pronto), A esce per il confronto A/B, Esc torna
// alla prima pagina.

#include <cstdint>
#include <filesystem>
#include <functional>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "esperimento.hpp"
#include "grafica/font.hpp"
#include "grafica/palette.hpp"
#include "grafica/tela.hpp"
#include "nucleo/geometria.hpp"
#include "pannello_interventi.hpp"
#include "piattaforma/finestra.hpp"

namespace m688 {

enum class SituazioneScelta : std::uint8_t { missione, pace, crisi, guerra };
enum class RoeScelta : std::uint8_t { missione, trattenute, libere };
enum class LivelloScelta : std::uint8_t { missione, recluta, regolare, esperto };   // comandanti e sonaristi dell'IA
const char* nome_situazione_scelta(SituazioneScelta s);   // «DELLA MISSIONE», «PACE», «CRISI», «GUERRA»
const char* nome_roe_scelta(RoeScelta r);                 // «DELLA MISSIONE», «TRATTENUTE», «LIBERE»
const char* nome_livello_scelta(LivelloScelta l);         // «DELLA MISSIONE», «RECLUTA», «REGOLARE», «ESPERTO»

struct SceltaParteSandbox {
    SituazioneScelta situazione = SituazioneScelta::missione;
    RoeScelta roe = RoeScelta::missione;
    LivelloScelta livello = LivelloScelta::missione;
    int allerta = -1;                 // -1: quella della missione; 0-3: A0-A3 di partenza
    bool carico_per_classe = false;   // il carico per classe e anno invece di quello della missione
};

struct RegolazioniSandbox {
    SceltaParteSandbox nato, patto, neutrali;
    bool meteo_realistico = true, navigazione_realistica = false, anni_89 = false;
    std::optional<std::uint32_t> seme;   // vuoto: uno nuovo
    int durata_min = 120;                // poi la partita finisce da sola
    bool ferma_all_esito = true;         // falso: continua anche dopo l'esito della missione, fino alla durata
    bool ferma_eventi = false;           // in finestra la compressione torna a 1x a ogni lancio, colpo o affondamento
    bool traffico = true;                // il traffico civile (docs/20 §3); spento nelle scene di prova
    // gli interruttori delle fasi dell'IA (docs/18), per i confronti
    bool difesa = true, armi = true, reazioni = true, forze = true, esm_ia = true, sonaristi_ia = true;
};

// I vetrini pronti (docs/19 §1): le scene di prova S1-S7 nella scheda della libreria, da data/missioni/prove/vetrini.toml
// (scritto da strumenti/scene_interazioni). Le regolazioni consigliate partono da quelle predefinite: valgono i campi
// presenti (meteo_realistico, navigazione_realistica, anni_89, ferma_all_esito, ferma_eventi), la durata da durata_min.
struct VetrinoPronto {
    std::string missione, titolo, mostra;   // titolo e «cosa mostra» nella lingua scelta
    std::filesystem::path file;             // la missione (.lua, missione dell'editor)
    RegolazioniSandbox regolazioni;
};
// Solo quelli pronti e con la missione presente; vuoto se il file manca o non si legge. inglese: i testi in inglese
std::vector<VetrinoPronto> leggi_vetrini(const std::filesystem::path& cartella_prove, bool inglese = false);

// Un'unita' della missione per la pagina degli interventi programmati: come la vede il pannello, e dove parte (per la
// cartina su cui si sceglie il punto degli interventi che lo vogliono)
struct UnitaEsperimento {
    UnitaPannello unita;
    Vec2 posizione;
};

// Gli esperimenti dell'utente, accanto alle serie: <cartella dei fascicoli>/../esperimenti
std::filesystem::path cartella_esperimenti();
// I .m688v di una cartella, per nome; vuoto se la cartella manca
std::vector<std::filesystem::path> elenco_esperimenti(const std::filesystem::path& cartella);

class SchermataRegolazioni {
public:
    // esperimento: A nella pagina degli interventi, il confronto A/B dell'esperimento() (lo lancia main)
    enum class Uscita { nessuna, avvia, indietro, serie, esperimento };
    enum class Pagina { regolazioni, interventi };
    SchermataRegolazioni(std::string titolo_missione, RegolazioniSandbox r);
    // dt: per le frecce tenute nel pannello degli interventi
    Uscita comandi(const Comandi& c, double dt = 1.0 / 60.0);
    void disegna(Tela& t, const Font& f) const;
    void palette(Palette& p) const;
    const RegolazioniSandbox& regolazioni() const { return r_; }

    // --- la pagina degli interventi programmati (docs/28 §5-6); main la prepara prima del ciclo ---
    // le unita' della missione (per il pannello e la cartina) e le classi che si possono inserire (chiave, nome)
    void imposta_missione(std::vector<UnitaEsperimento> unita, std::vector<std::pair<std::string, std::string>> classi);
    // il codice sandbox della partita con queste regolazioni (vuoto se non si puo'): per salvare e per il confronto
    void imposta_codice(std::function<std::string(const RegolazioniSandbox&)> codice) { codice_ = std::move(codice); }
    // dove salvare (vuota: cartella_esperimenti()) e dove stanno gli esperimenti pronti (data/motore/esperimenti)
    void imposta_cartelle(std::filesystem::path utente, std::filesystem::path pronti);
    // L'esperimento corrente: i campi, gli interventi (tempo >= 0, id -1, in ordine di tempo) e il codice sandbox (quello
    // del file caricato finche' le regolazioni non cambiano, poi quello delle regolazioni correnti)
    Esperimento esperimento() const;
    bool vuole_testo() const { return campo_.has_value(); }   // un campo in scrittura: la finestra passa i caratteri
    Pagina pagina() const { return pagina_; }
    void pagina(Pagina p) { pagina_ = p; }
    const std::string& avviso() const { return avviso_; }    // l'ultimo messaggio della pagina (salvato, errore...)

private:
    enum class CampoTesto { titolo, mostra, ipotesi, nome_file };
    struct Campo {
        CampoTesto quale;
        std::string buffer;
    };
    struct VoceCaricamento {   // un file nella lista di C
        std::filesystem::path file;
        std::string titolo;
        bool pronto = false, altra_missione = false, rovinato = false;
    };

    void cambia(int riga, int verso);
    Uscita comandi_interventi(const Comandi& c, double dt);
    void disegna_interventi(Tela& t, const Font& f) const;
    void apri_pannello();
    void conferma_ora();
    void togli_scelto();
    void ordina(int scelto);              // gli interventi in ordine di tempo; la riga resta su `scelto`
    void apri_caricamento();
    void carica(const std::filesystem::path& p);
    void salva(const std::string& nome);
    std::string riga_intervento(const Intervento& x) const;
    std::optional<std::uint32_t> impronta_corrente() const;   // dal codice delle regolazioni correnti
    std::filesystem::path cartella_utente() const;

    std::string titolo_;
    RegolazioniSandbox r_;
    int riga_ = 0, prec_su_giu_ = 0, prec_ruota_ = 0;
    // la seconda pagina
    Pagina pagina_ = Pagina::regolazioni;
    Esperimento esperimento_;
    std::vector<UnitaEsperimento> unita_;
    std::vector<std::pair<std::string, std::string>> classi_;
    PannelloInterventi pannello_;
    std::function<std::string(const RegolazioniSandbox&)> codice_;
    std::filesystem::path cartella_utente_, cartella_pronti_;
    std::string codice_caricato_;          // il codice del file caricato
    bool regolazioni_cambiate_ = false;    // dopo il caricamento
    int riga2_ = 0;                        // 0-3 i campi, poi gli interventi
    std::optional<Campo> campo_;
    std::optional<double> ora_;            // l'ora in scrittura (s di missione)
    std::optional<Intervento> nuovo_;      // l'intervento appena composto, in attesa dell'ora
    std::optional<std::vector<VoceCaricamento>> caricamento_;
    int scelta_caricamento_ = 0;
    std::string avviso_;
};

}  // namespace m688
