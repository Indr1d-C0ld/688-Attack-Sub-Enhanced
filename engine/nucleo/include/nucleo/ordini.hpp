#pragma once
// Ordini del giocatore (docs/11_comandi_e_stazioni.md §2, fase C1).
//
// Tastiera, mouse e pannelli non toccano la simulazione: producono ordini con l'istante di simulazione in cui
// valgono. La coda li consegna al nucleo all'inizio del passo, nell'ordine (istante, inserimento), e tiene lo storico.
// Lo stesso seme con lo stesso storico ridà la stessa partita: lo storico, salvato come testo, alimenta replay,
// debriefing e statistiche (punto 16 della roadmap).
//
// Il significato dei parametri e' quello dei comandi dell'originale (docs/originale/sensori_e_navigazione.md §7) per
// il livello classico; la versione Enhanced li legge con le stesse unita'.

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "nucleo/carico.hpp"

namespace m688 {

enum class TipoOrdine : std::uint8_t {
    // manovra
    telegrafo,              // a: frazione della potenza (-1 indietro tutta ... 1,09 massima), n = 1: arresto rapido
    timone,                 // a: rateo di virata, gradi/s (classico da -3,6 a +3,6; negativo a sinistra)
    quota,                  // a: quota ordinata (m), b: rateo (classico: frazione, |scatto| / 4)
    quota_periscopica,
    quota_150_piedi,
    emersione_emergenza,
    punto_di_rotta,         // a, b: est e nord (m)
    cancella_punto,         // l'ultimo punto di rotta
    pilota_automatico,      // n = 1 inserito, 0 disinserito
    // sensori
    periscopio,             // n = 1 alzato, 0 abbassato
    antenna,                // n = 1 alzata, 0 abbassata
    schiera_cala,
    schiera_recupera,
    schiera_taglia,
    impulso_sonar,
    filtro_sonar,           // n = 0 passa banda, 1 passa alto
    designa_bersaglio,      // n: id dell'unita' (contatto) designata
    // armi
    carica_tubo,            // n: tubo (1-7)
    lancia,                 // n: tubo (1-7)
    seleziona_siluro,       // n: id del siluro in acqua da guidare
    guida_punto,            // a, b: est e nord (m) verso cui dirigere il siluro selezionato (DIRECT)
    guida_cerca,            // il siluro selezionato inizia la ricerca (SEARCH)
    guida_detona,           // solo livello classico (DETONATE dell'originale; nell'Enhanced non c'e')
    spegni_siluro,          // solo Enhanced: spegnimento via filo
    taglia_filo,            // solo Enhanced
    // tempo
    compressione_tempo,     // a: fattore (1 = tempo reale)
    // solo Enhanced (docs/11 §10, C6): comandi al modello dinamico e ai siluri reali
    barra,                  // a: angolo di barra del timone, gradi (+ a dritta, +-35)
    accosta_per,            // a: rotta da prendere, b: barra massima (gradi)
    giri,                   // a: frazione dei giri massimi (-0,5 indietro tutta ... 1)
    quota_con_angolo,       // a: quota ordinata (m), b: angolo massimo di immersione (gradi)
    angolo_immersione,      // a: angolo di immersione senza quota (gradi, + prua giu')
    imposta_siluro,         // n: tubo, a: valore, b: campo (CampoSiluro, nucleo/siluro.hpp)
    comando_filo,           // n: siluro, a: valore, b: tipo (ComandoFiloTipo, nucleo/siluro.hpp)
    // sala radio dell'Enhanced (docs/11 §11, S4)
    radio_filo,             // n = 1 cala l'antenna a filo galleggiante, 0 la recupera
    radio_trasmetti,        // n: tipo di trasmissione (rapporto di contatto, d'attacco, SITREP, ...; nucleo/radio)
    radio_ssixs,            // richiesta di scarico via satellite
    radio_slot,             // lancio di una boa SLOT: a = ritardo della trasmissione in s
    // equipaggio dell'Enhanced
    silenzio_a_bordo,       // n = 1 silenzio, 0 fine (per ora: ricariche rallentate e senza transitori, StazioneArmi)
    radio_verifica,         // n = serie del messaggio: verifica del gruppo d'autenticazione (falso: avviso 339)
    scegli_arma_tubo,       // Enhanced, carico M2: n = tubo, a = indice dell'arma in StazioneArmi::varianti()
    classifica_contatto,    // n: id del contatto, testo: la classe data dal giocatore (registro M1, replay)
    // periscopi dell'Enhanced (docs/14, P2; nucleo/periscopi): ingrandimento e modo restano dell'interfaccia
    scegli_periscopio,      // n: indice del periscopio nella classe (688: 0 Type 2 d'attacco, 1 Type 18 di ricerca)
    elevazione_periscopio,  // a: elevazione della linea di mira, gradi
    foto_periscopio,        // a: rilevamento vero della vista, gradi; n: indice dell'ingrandimento (registro M1)
    // manovra d'emergenza (dossier 16 §4.4), accanto a emersione_emergenza: avanti tutta e piani a scendere fino alla
    // quota di sicurezza sotto la chiglia delle navi vicine (Missione::quota_immersione_emergenza)
    immersione_emergenza,
    // l'Alfa nell'Enhanced (dossier 19 §3.1-3.4 e §6; nota condivisa alfa-boksit-tan-poks-akkord-proposta.md)
    modo_boksit,            // n: asse (0 rotta, 1 quota), a: modo (0 manuale, 1 automatico, 2 programma)
    programma_tappa,        // n: punto di rotta (1-9), a: quota (m, < 0 la corrente), b: velocita' (kn, < 0 la corrente)
    tan,                    // n = 1 assetto automatico inserito, 0 disinserito
    sfoga,                  // ПОКС: apre gli sfoghi delle casse principali (l'aria esce, l'acqua rientra)
    compensa,               // ПОКС: a = m^3 d'acqua nelle casse d'assetto (+ imbarca, - espelle)
    akkord_designa,         // n: contatto da mettere in uno dei tre bersagli di Akkord
    akkord_scegli,          // n: bersaglio di Akkord (1-3) per le armi
    bqs15,                  // Enhanced: n = 1 accende il BQS-15 (mine a prua, docs/ricerca/06 §1.1), 0 lo spegne
    radar,                  // Enhanced: n = 1 accende il radar del giocatore (BPS-15, Snoop), 0 lo spegne (docs/18 fase 2)
    punto_nave,             // Enhanced, navigazione incerta: testo = il punto nave (transit, gps, omega, astronomico...)
    // stazione dei missili dell'Enhanced (docs/21; missioni/missione_missili.cpp)
    imposta_missile,        // n: tubo, a: valore, b: campo (CampoMissile, missioni/missione.hpp)
    prepara_missile,        // n: tubo, a = 1 avvia la preparazione (PWR, ALN), 0 la spegne
    // il quadro del fondale dell'Enhanced (docs/24 §3; missioni/missione_fondale.cpp)
    ecoscandaglio_ping,     // un ping dell'ecoscandaglio adesso
    ecoscandaglio_periodo,  // a: minuti fra un ping e l'altro (1-30), 0 a comando
    ecoscandaglio_potenza,  // n = 0 piena, 1 bassa (-20 dB: meno portata, si sente meno lontano)
    // il libretto di riconoscimento al periscopio dell'Enhanced (docs/25 §3.2, §3.4; missioni/missione_periscopi.cpp)
    identifica_visiva,      // n: id del contatto, testo: la classe dichiarata dal giocatore dalla tavola (come classifica_contatto)
    osserva_aob,            // n: id del contatto, a: AOB in gradi (+ dritta, - sinistra), b: incertezza in gradi
    misura_telemetro,       // n: id del contatto, a: distanza letta al telemetro (m), b: altezza d'albero usata (m)
    luci_giocatore,         // le luci di navigazione del giocatore (docs/27): a = 1 accese, 0 spente
    // il controllo danni dell'Enhanced (docs/10 §10.19; BattelloDanni)
    isola_compartimento,    // n: compartimento, a: 0 sgombero e porte, 1 chiude subito, 2 riapre
    priorita_sistema,       // n: Sistema da portare in cima alle riparazioni
};

std::string_view nome_ordine(TipoOrdine t);
std::optional<TipoOrdine> ordine_da_nome(std::string_view nome);

struct Ordine {
    double tempo = 0.0;     // s di simulazione in cui l'ordine vale
    TipoOrdine tipo = TipoOrdine::telegrafo;
    double a = 0.0, b = 0.0;
    int n = 0;
    std::string testo = {}; // facoltativo, senza spazi (classifica_contatto, identifica_visiva: la classe)
    bool operator==(const Ordine&) const = default;
};

// La classificazione di un contatto data dal giocatore (stazione sonar): gli spazi della classe diventano '_'
Ordine ordine_classifica(double t, int contatto, const std::string& classe);
// L'identificazione visiva dal libretto (docs/25 §3.2), con la stessa regola degli spazi
Ordine ordine_identifica_visiva(double t, int contatto, const std::string& classe);
// La lettura del telemetro a immagine sdoppiata (docs/25 §3.3): la distanza letta e l'altezza del quadrante
Ordine ordine_misura_telemetro(double t, int contatto, double letta_m, double altezza_m);

// Risposta di una stazione (Armi, Sonar) a un'azione o a un ordine consegnato: i comandi da applicare alla missione,
// in ordine, e il messaggio della stazione (0: nessuno)
struct EsitoStazione {
    std::vector<Ordine> alla_missione;
    int messaggio = 0;
};

class RegistroOrdini {
public:
    // Accoda un ordine. Un istante nel passato (prima dell'ultimo consegnato) vale al passo successivo: la storia
    // non si riscrive.
    void ordina(const Ordine& o);
    // Ordini con istante <= t non ancora consegnati, nell'ordine (istante, inserimento); passano nello storico.
    std::vector<Ordine> consegna(double t);
    bool vuota() const { return coda_.empty(); }
    const std::vector<Ordine>& storico() const { return storico_; }

    // Carico di partenza della Sala siluri (M2), applicato prima del primo passo: fa parte dello storico, cosi' il
    // replay lo riapplica uguale. Senza (file vecchi, missioni senza Sala siluri) resta vuoto.
    void imposta_carico(const CaricoPartenza& c) { carico_ = c; }
    const std::optional<CaricoPartenza>& carico() const { return carico_; }

    // Storico come testo, una riga per ordine: "istante nome a b n [testo]" (numeri con 17 cifre: si rilegge identico). Il
    // carico, se c'e', va in testa su righe "carico <campo> ...": posizioni e tipi come "n v1 ... vn" ("-" per un
    // posto vuoto), rastrelliera, pozzi ed esche come "arma=numero ...". Le righe che cominciano con '#' sono commenti.
    std::string serializza() const;
    // Coda con gli ordini di uno storico salvato (replay), e il carico se c'e'; lancia std::runtime_error su una riga
    // non valida.
    static RegistroOrdini da_testo(const std::string& testo);

private:
    struct InCoda {
        Ordine o;
        std::uint64_t sequenza;
    };
    std::vector<InCoda> coda_;
    std::vector<Ordine> storico_;
    std::optional<CaricoPartenza> carico_;
    std::uint64_t sequenza_ = 0;
    double ultimo_ = -1e300;
};

}  // namespace m688
