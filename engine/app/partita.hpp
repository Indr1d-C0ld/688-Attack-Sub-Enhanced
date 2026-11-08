#pragma once
// Una partita in corso (Fase 4a, docs/12_schermate_di_missione.md): la missione con danni, sottomarini, stazioni,
// posto di comando, ordini del giocatore e mondo da disegnare, e il passo che li fa avanzare insieme (prima in
// main.cpp). Serve all'app per giocare una missione scelta da un menu e per rigiocarla senza finestra fino a un
// istante (le minicartoline del debriefing): con gli stessi ordini la partita e' la stessa, passo per passo.

#include <cstdint>
#include <functional>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <set>
#include <vector>

#include "grafica/effetti.hpp"
#include "missioni/danni_missione.hpp"
#include "missioni/danni_sottomarini.hpp"
#include "missioni/missione.hpp"
#include "missioni/stazioni_missione.hpp"
#include "missioni/traccia_missione.hpp"
#include "nucleo/asw_aereo.hpp"
#include "nucleo/carico.hpp"
#include "nucleo/comando.hpp"
#include "nucleo/mondo.hpp"
#include "nucleo/ordini.hpp"
#include "nucleo/sole.hpp"
#include "nucleo/traccia_replay.hpp"
#include "risorse.hpp"
#include "scelta_fotografie.hpp"

namespace m688 {

struct ImpostazioniPartita {
    std::string dati, cartella_missioni, missione;
    std::uint64_t seme = 1;
    RegolaDanni regola = RegolaDanni::classica;
    std::string carta = "epoca";       // Enhanced: carta d'epoca della propria parte o fondale vero
    DataOra inizio{1989, 10, 15, 9.0};
    std::string urto_giocatore, urto_unita;   // urti di prova all'avvio (KG,DIST,GRADI e ID,KG,DIST,GRADI)
    double ripresa_da = -1.0;          // «Riprendi da qui»: l'istante da cui comanda di nuovo il giocatore (traccia)
};

class Partita {
public:
    // Lancia std::runtime_error se la missione non si carica o non ha il giocatore
    Partita(const Risorse& r, const ImpostazioniPartita& o, Effetti& effetti);

    // Un passo: ordini dal registro, missione, stazioni, danni, mondo da disegnare, effetti
    void avanza(double dt);
    // Gli effetti (fumo, fuoco, spruzzi, colonne): nel passo, o a ogni fotogramma quando la partita gira a 1/9 s con la
    // finestra (effetti_per_fotogramma), cosi' restano fluidi; a passi di al piu' 1/30 s
    bool effetti_per_fotogramma = false;
    void anima_effetti(double dt);
    // L'interfaccia osserva gli ordini consegnati e aggiorna le sue stazioni dopo ogni passo (vuote: nessuna)
    std::function<void(const Ordine&)> osserva;
    std::function<void()> dopo_passo;
    // Ogni fotogramma registrato nella traccia del replay (sandbox: la vista dal vivo di app/replay lo riceve uguale)
    std::function<void(const Fotogramma&)> dopo_fotogramma;

    Missione& missione() { return *mis_; }
    const Missione& missione() const { return *mis_; }
    DanniMissione& danni() { return *dm_; }
    DanniSottomarini* sottomarini() { return ds_.get(); }
    StazioniMissione& stazioni() { return *stazioni_; }
    const StazioniMissione& stazioni() const { return *stazioni_; }
    // La traccia del replay (nucleo/traccia_replay), solo con la regola di simulazione: un fotogramma dopo i passi
    // che servono; `dati_app` (sonar, stazione, bersaglio, soluzione proposta) li da' l'interfaccia, se c'e'
    RegistratoreTraccia* traccia() { return traccia_.get(); }
    std::function<DatiApp()> dati_app;
    PostoDiComando& posto() { return posto_; }
    RegistroOrdini& ordini() { return ordini_; }
    std::vector<Messaggio>& equipaggio() { return equipaggio_; }
    Mondo& mondo() { return mondo_; }
    int id_proprio() const { return id_proprio_; }
    const std::map<int, std::string>& testi() const { return testi_; }

    // Il carico scelto nella Sala siluri (Enhanced, M2), prima del primo passo: la partita tiene il catalogo vivo
    // quanto le stazioni
    void imposta_carico(CatalogoCarico catalogo, const CaricoPartenza& carico);
    const CaricoPartenza* carico() const { return carico_ ? &*carico_ : nullptr; }
    const CatalogoCarico* catalogo_carico() const { return catalogo_ ? catalogo_.get() : nullptr; }

    // Le minicartoline del debriefing (M4): il mondo e gli effetti all'istante di ogni evento che si vede (per colpi,
    // affondamenti e lanci qualche secondo dopo, quando la colonna d'acqua o il missile sono su). Solo osservazione:
    // senza replay, e con un tetto e un limite (SceltaFotografie): i colpi d'artiglieria di un tiratore ne dividono una.
    struct Fotografia {
        double tempo = 0.0;
        Mondo mondo;
        Effetti effetti;
    };
    const Fotografia* fotografia(int evento) const {
        const auto it = fotografie_.find(scelta_fotografie_.foto_di(evento));
        return it != fotografie_.end() ? &it->second : nullptr;
    }

    // Tracce per il debriefing: posizione vera di ogni unita' ogni 20 s di missione, in ordine di tempo
    struct PuntoTraccia {
        double tempo = 0.0;
        int id = -1;
        Vec2 pos;
    };
    const std::vector<PuntoTraccia>& tracce() const { return tracce_; }

    // L'ultimo messaggio di bordo con un testo (o la prima riga del rapporto d'esito)
    std::string ultimo_messaggio() const;
    // L'archivio dei messaggi di bordo del giocatore, in ordine di tempo (stazione Radio del classico)
    std::vector<std::pair<double, std::string>> storico_messaggi() const;
    // I messaggi, i colpi e l'esito su stdout (istantanee senza finestra)
    void stampa_resoconto() const;

    // luce ambiente del momento (0 notte, 1 giorno), dalla vista: per i marcatori sovietici di giorno o di notte
    void imposta_luce(double l) { luce_ = l; }

private:
    std::vector<const std::vector<Messaggio>*> liste_messaggi() const;
    std::optional<std::string> testo_di(int codice) const;   // tabella globale o, da 1000, testi della missione
    double x_albero(const std::string& classe);

    const Risorse& r_;
    Effetti& effetti_;
    std::unique_ptr<Missione> mis_;
    std::unique_ptr<DanniMissione> dm_;
    std::unique_ptr<DanniSottomarini> ds_;
    std::unique_ptr<StazioniMissione> stazioni_;
    PostoDiComando posto_;
    RegistroOrdini ordini_;
    std::vector<Messaggio> equipaggio_;
    Mondo mondo_;
    std::map<int, std::string> testi_;
    int id_proprio_ = -1;
    // gia' passati agli effetti
    std::size_t navi_seguite_ = 0, colpi_mostrati_ = 0, emersioni_mostrate_ = 0, lanci_mostrati_ = 0;
    std::set<int> sottomarini_seguiti_;   // gli emersi danneggiati gia' passati agli effetti (fuoco e fumo li seguono)
    std::vector<bool> segni_mostrati_;
    std::map<std::string, double> alberi_x_;   // E2: punto piu' alto del modello (periscopio o antenna), m a prua del centro
    std::vector<PuntoTraccia> tracce_;
    std::map<int, Fotografia> fotografie_;
    std::vector<std::pair<double, int>> da_fotografare_;   // (istante, evento)
    SceltaFotografie scelta_fotografie_;
    std::size_t eventi_visti_ = 0;
    std::unique_ptr<CatalogoCarico> catalogo_;
    std::optional<CaricoPartenza> carico_;
    double prossima_traccia_ = 0.0;
    // ASW aereo (docs/13, A5): i dati dei mezzi per gli effetti (rotore, MAD, parte), letti alla prima occorrenza
    void effetti_asw(const Missione& mis);
    std::string cartella_dati_;
    std::optional<DatiAsw> asw_;
    std::size_t marcatori_mostrati_ = 0, lanci_aerei_mostrati_ = 0;
    double luce_ = 1.0;
    // gli avvisi gia' composti (ASW 380-389 col rilevamento, sonarista 410-413): barra e storico dei messaggi
    std::vector<std::pair<double, std::string>> avvisi_testo_;
    std::size_t avvisi_testo_visti_ = 0, avvisi_sonarista_visti_ = 0;
    std::unique_ptr<RegistratoreTraccia> traccia_;
    RaccoltaTraccia raccolta_;   // traccia v2 (disassemblatore): i fatti di tutte le unita' per le statistiche della sandbox
    // artiglieria navale (docs/16, G6): gli eventi agli effetti e gli avvisi 420-429, coi loro intervalli minimi
    void artiglieria(const Missione& mis);
    void avviso(int codice, double tempo, std::optional<double> ril = std::nullopt);
    std::size_t eventi_art_visti_ = 0, impatti_art_visti_ = 0;
    std::map<int, double> ultima_vampa_;   // tiratore -> ultimo avviso di vampe
    std::map<int, double> ultimo_colpo_;   // codice 424-428 -> ultimo avviso
    double ultimo_sonar_art_ = -1e9, ultimo_illuminante_ = -1e9, ultimo_avvertimento_ = -1e9, ultima_colonna_ = -1e9;
    // collisioni (dossier 16, P4): gli allarmi dell'equipaggio sui contatti (431-432) e le quasi collisioni (433-434)
    void allarmi_collisione(const Missione& mis);
    std::map<int, double> ultimo_cpa_, ultimo_vicino_;   // contatto -> ultimo avviso
    std::size_t quasi_viste_ = 0, cavi_visti_ = 0;
    std::map<int, double> ultimo_cavo_;   // nave -> ultimo avviso del cavo vicino
    // lo spettatore (docs/18, fase 5): i lanci, le salve e gli urti fra unita' dell'IA che l'equipaggio puo' sapere
    // (EventoMissione::noto), come avvisi 460-467; il resto solo nel debriefing e nel replay
    void interazioni_ia(const Missione& mis);
    std::size_t eventi_ia_visti_ = 0;
    std::map<int, double> ultimo_ia_;   // codice 460-467 -> ultimo avviso
};

// Stato della missione nel mondo da disegnare; se il sottomarino del giocatore e' perduto (incaglio, siluri) la vista
// resta ferma dov'era, per mostrare l'esito
void sincronizza(const Missione& mis, Mondo& mondo, int id_proprio);

}  // namespace m688
