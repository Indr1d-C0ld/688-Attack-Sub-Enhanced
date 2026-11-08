#pragma once
// Scelta della missione (Fase 4a, docs/12_schermate_di_missione.md M5; proposta in docs/ricerca/15 §7.3): le missioni
// originali con le colonne 688 e Alfa come nell'originale, la scheda della missione con l'anteprima del teatro e le
// opzioni della partita (stazioni 1989 o moderne, carta d'epoca o fondale vero). La scheda Missione rapida (R, o il
// clic) compone i parametri del generatore (missioni/missione_rapida.hpp) o legge un codice missione, e mostra
// l'operazione generata col teatro, la data e il briefing (E la apre nell'editor). La scheda Editor elenca le missioni
// dell'editor (missioni/scenario_missione.hpp) con «Nuova missione»: Invio apre l'editor (app/editor_missione). R passa
// da una scheda alla successiva. Carriera resta in grigio finche' non c'e'.

#include <cstdint>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "grafica/font.hpp"
#include "grafica/palette.hpp"
#include "grafica/tela.hpp"
#include "missioni/missione_rapida.hpp"
#include "missioni/pacchetto_missione.hpp"
#include "regolazioni_sandbox.hpp"
#include "nucleo/fondale.hpp"
#include "piattaforma/finestra.hpp"

namespace m688 {

// Una missione nella cartella delle missioni (dall'intestazione del file Lua, senza caricarla)
struct MissioneInElenco {
    std::string id;             // SCENE4A
    int numero = 0;             // 1-10, come nell'originale
    std::string titolo_it, titolo_en;
    std::string lato;           // usa, urss, due_giocatori
    std::string teatro;
};
std::vector<MissioneInElenco> elenco_missioni(const std::string& cartella);

// Le scelte con cui la partita parte
struct SceltaPartita {
    std::string missione;       // id
    bool anni_89 = false;       // stazioni nel modo d'epoca
    bool carta_epoca = true;    // carta d'epoca della propria parte o fondale vero
    bool meteo_realistico = false;   // meteo dinamico del teatro e della stagione (W); falso: mare 3, sereno
    int asw = 1;                // difficolta' dell'ASW aereo (K): 0 facile, 1 normale, 2 realistico, 3 minaccia percepita
    int sonarista = 2;          // il sonarista dell'equipaggio (S): 0 nessuno, 1 recluta, 2 medio, 3 esperto
    bool navigazione_realistica = false;   // N: la posizione propria incerta e i punti nave (nucleo/navigazione)
    bool gps = false;                      // N, la terza posizione: col GPS fra i punti nave (l'opzione «1990+»)
    std::optional<MissioneRapida> rapida;   // una missione rapida generata: il file da scrivere, seme, data e ora
    // una missione dell'editor o della libreria giocata subito (G): la sua cartella e lo scenario (data, ora, teatro)
    std::optional<ScenarioMissione> scenario;
    std::string cartella_scenario;
    std::optional<std::uint32_t> seme;   // dal codice partita (P): la stessa partita; vuoto, un seme nuovo
    bool classico = false;      // O: il gioco del 1989, senza briefing ne' carriera (come --regola classica)
    bool sandbox = false;       // V: il vetrino dell'IA (docs/19_sandbox.md), poi le sue regolazioni
    std::optional<RegolazioniSandbox> regolazioni;   // dal codice sandbox (P): le regolazioni gia' compilate
};

class SchermataScelta {
public:
    SchermataScelta(std::vector<MissioneInElenco> missioni, std::string dati);
    // la scelta fatta (Invio), vuota finche' si sceglie; `esci` vero con Esc
    std::optional<SceltaPartita> comandi(const Comandi& ingresso, bool& esci);
    void disegna(Tela& t, const Font& f) const;
    void palette(Palette& p) const;

    int missione_scelta() const { return riga_; }
    bool museo_chiesto() { const bool m = museo_; museo_ = false; return m; }   // F1: il Museo
    bool albo_chiesto() { const bool a = albo_; albo_ = false; return a; }      // C: l'albo dei comandanti
    // L'editor: dalla scheda Editor (una missione salvata o nuova, nullopt) o con E dalla rapida in vista
    bool editor_chiesto() { const bool e = editor_; editor_ = false; return e; }
    std::optional<ScenarioMissione> scenario_per_editor() { return std::move(scenario_editor_); }
    SceltaPartita opzioni_partita() const { return opzioni_; }   // per una missione dell'editor: le opzioni scelte qui
    void scheda_editor() { scheda_ = 3; ricarica_scenari(); }   // per le istantanee
    void imposta_cartella_missioni(std::string c) { cartella_missioni_ = std::move(c); }   // le originali, per l'editor
    // I comandanti in carica (grado e nome) per la US Navy e la VMF, "" se nessuno: va in mare quello della parte
    void imposta_comandanti(std::string usn, std::string vmf) { comandante_usn_ = std::move(usn), comandante_vmf_ = std::move(vmf); }
    bool alfa() const { return alfa_; }
    // La riga dell'opzione col tasto `t` nell'ultimo disegno, per le prove; vuota se non c'e'
    std::optional<Rettangolo> zona_opzione(const std::string& t) const {
        for (const ZonaOpzione& z : zone_opzioni_)
            if (z.tasto == t) return z.r;
        return std::nullopt;
    }
    // Il battello del comandante appena scelto: il lato nelle originali (la prima missione che ce l'ha, se questa no) e nella rapida
    void vai_al_battello(bool alfa);
    // O, o il clic sul tasto del modo: classico o Enhanced. Nel classico solo le originali, solo Musica e Video fra le opzioni
    bool classico() const { return classico_; }
    void imposta_classico(bool si);
    int musica() const { return musica_; }   // M: l'uscita della musica, FM, MT-32, 1989 AdLib, Tandy, Covox, spenta (scelta_missione.cpp)
    void imposta_musica(int m) { musica_ = m; }
    int video() const { return video_; }   // A: il video d'apertura, 0 sempre, 1 solo la prima volta, 2 mai
    void imposta_video(int v) { video_ = v; }
    bool vuole_testo() const { return scrive_codice_; }   // il codice missione si digita
    void scheda_rapida(bool si) { scheda_ = si ? 2 : 1; }  // per le istantanee
    void imposta_rapida(const ParametriRapida& p) { rapida_ = p; }

private:
    const MissioneInElenco* voce(int numero, bool alfa) const;
    std::optional<SceltaPartita> comandi_originali(const Comandi& c, bool& esci);
    std::optional<SceltaPartita> comandi_rapida(const Comandi& c, bool& esci);
    std::optional<SceltaPartita> comandi_editor(const Comandi& c, bool& esci);
    // P: il codice digitato, di una rapida (la scheda Missione rapida coi suoi parametri) o di una partita (app/
    // codice_partita.hpp: la missione dell'impronta con il seme e le opzioni, subito al briefing)
    std::optional<SceltaPartita> comandi_codice(const Comandi& c, bool& esci);
    void ricarica_scenari();                 // le missioni dell'editor e le importate
    void importa();                          // I: i pacchetti .m688m ricevuti (missioni/pacchetto_missione.hpp)
    void disegna_editor(Tela& t, const Font& f) const;
    void disegna_rapida(Tela& t, const Font& f) const;
    // su_lamiera: la scheda delle originali (variante A, i tasti della tastiera e il testo inciso); altrimenti sul
    // vetro nero delle schede rapida ed editor
    void disegna_opzioni(Tela& t, const Font& f, int y, bool su_lamiera = false) const;
    // il piede comune (lamiera::piede): la riga d'aiuto, o il codice in scrittura e l'avviso, e i tre tasti
    void disegna_piede(Tela& t, const Font& f, const char* aiuto, const char* centro, const char* avanti) const;
    const MissioneRapida* anteprima() const;   // la missione dei parametri correnti (nullptr: non si genera)
    const Fondale* fondale(const std::string& teatro) const;

    std::vector<MissioneInElenco> missioni_;
    std::vector<int> numeri_;   // le missioni originali, in ordine
    std::string dati_;
    int riga_ = 0;
    bool alfa_ = false;
    int prec_su_giu_ = 0, prec_ruota_ = 0;   // le frecce tenute: un passo per pressione
    SceltaPartita opzioni_;
    bool museo_ = false, albo_ = false;
    int musica_ = 0;
    int video_ = 0;
    bool vetrino_ = false;   // V premuto in questo fotogramma: la scelta va alla sandbox
    struct ZonaOpzione { Rettangolo r; std::string tasto; };
    mutable std::vector<ZonaOpzione> zone_opzioni_;   // le righe delle opzioni nell'ultimo disegno: si cliccano
    bool classico_ = false;
    std::string comandante_usn_, comandante_vmf_;
    int scheda_ = 1;                         // 1 missioni originali, 2 missione rapida, 3 editor
    bool editor_ = false;
    std::optional<ScenarioMissione> scenario_editor_;
    std::vector<ScenarioInElenco> scenari_;  // la scheda Editor: le missioni salvate, poi le importate
    std::vector<bool> importate_, libreria_; // per riga di scenari_: importata, della libreria (data/missioni/libreria)
    std::vector<VetrinoPronto> vetrini_;     // i vetrini pronti della sandbox (data/missioni/prove/vetrini.toml)
    int prima_scenario() const { return 2 + static_cast<int>(vetrini_.size()); }   // le righe: nuova, originale, vetrini
    std::vector<std::string> esiti_import_;  // l'ultima importazione, una riga per pacchetto
    int riga_editor_ = 0;                    // 0 «Nuova missione», 1 «Da una originale», poi i vetrini e gli scenari
    int riga_nav_proposta_ = -1;             // l'ultima riga per cui si e' proposta la navigazione di serie dello scenario
    int originale_ = 0;                      // la missione originale scelta per la riga 1 (indice in missioni_)
    std::string cartella_missioni_;
    std::vector<std::string> note_originale_;   // le note dell'ultima conversione (scenario_da_originale)
    ParametriRapida rapida_;
    int campo_ = 0;                          // la riga dei parametri della rapida
    bool scrive_codice_ = false;
    std::string codice_, avviso_;
    mutable std::optional<MissioneRapida> anteprima_;
    mutable std::optional<ParametriRapida> anteprima_di_;
    mutable std::string errore_;
    mutable std::map<std::string, std::unique_ptr<Fondale>> fondali_;   // anteprime caricate alla prima occasione
};

}  // namespace m688
