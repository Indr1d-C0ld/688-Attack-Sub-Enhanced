#pragma once
// Stazione Armi del giocatore nel livello classico (docs/11_comandi_e_stazioni.md §3.3, fase C3): stato dei tubi,
// scorte, armamento in due tempi, ricarica automatica, controlli del lancio e della guida a filo, limite dei siluri
// in acqua. Nell'originale tutto questo sta nel programma e non negli script delle missioni
// (docs/originale/sensori_e_navigazione.md §10.1-§10.3, docs/originale/numeri_eseguibile.md §1, `1000:740E-7B4C`):
// all'avvio i tubi sono vuoti e le scorte sono la dotazione intera; gli script fanno solo il timer di
// armamento (`carica_tubo_n`, 20 s, poi il messaggio 10-16 del tubo) e i gestori di lancio e di guida.
//
// Enhanced (dossier docs/ricerca/14_carico_armi.md §1.5 e §5.4): la ricarica dalla rastrelliera e' un'azione con il
// suo tempo (688 300 s per tubo, Alfa 75 s col caricatore automatico, al piu' due alla volta; STIME), poi l'arma sta
// nel tubo («carico») e l'armamento resta quello dell'originale (20 s). Fuori dal silenzio a bordo ogni ricarica fa
// un colpo ogni 60 s di lavoro (un transitorio per l'acustica); in silenzio dura il doppio e non fa rumore: il rumore
// viene dalla velocita' del carrello e dai colpi della catena e della culatta, che un movimento lento evita, e
// fermare del tutto la ricarica (Cold Waters) renderebbe impossibile un lungo avvicinamento silenzioso dopo una salva.
// I tubi delle esche hanno i loro lanciatori: restano come nel livello classico. Senza attivazione nulla cambia.
//
// Carico della Sala siluri (Fase 4a, M2; nucleo/carico.hpp): con imposta_carico i tubi partono con le armi scelte,
// le scorte si tengono per variante (Mk 48 o ADCAP, TASM o TLAM-C...) in rastrelliera, nei pozzi verticali (la
// posizione 7 del Flight II: niente ricarica, le celle sono gia' cariche) e fra le esche; la famiglia dell'arma
// (l'Arma qui sotto) resta quella delle regole dell'originale e del gestore di lancio della missione. Un tubo puo'
// cambiare arma (scegli_arma_tubo): vuoto prende l'arma scelta alla prossima ricarica, carico la scambia con una
// ricarica intera, in ricarica ricomincia con l'arma nuova. Una mina armata si lancia come le altre armi: la posa la
// fa il campo minato della missione (nucleo/mine.hpp), con la rotta e la distanza (rte) impostate per il tubo.
//
// La stazione e' una funzione deterministica degli ordini consegnati e dei messaggi della missione: le azioni del
// pannello diventano ordini (registrati, quindi ripetibili), e gli ordini consegnati diventano qui i comandi per la
// missione o un rifiuto con il messaggio dell'originale. Un replay rifa' gli stessi passi.

#include <array>
#include <map>
#include <set>
#include <string>
#include <string_view>
#include <vector>

#include "nucleo/carico.hpp"
#include "nucleo/ordini.hpp"
#include "nucleo/siluro.hpp"

namespace m688 {

// DS:02D8+i; mina, attacco_terra ed esca_mobile solo con il carico dell'Enhanced (famiglie di carico.toml).
// esca_mobile: le esche semoventi dai tubi da siluri (MG-74, MOSS Mk 70), con la ricarica, le scorte in rastrelliera
// e la variante di un siluro; al lancio la missione mette in mare l'esca (lancia_esca_mobile) e non un siluro
enum class Arma : std::uint8_t { mk48, type53, sea_lance, tomahawk, harpoon, esca, nessuna, mina, attacco_terra,
                                 esca_mobile };
Arma arma_da_nome(std::string_view nome);                // nomi delle missioni convertite ("mk48", "esca", ...)
std::string_view nome_arma(Arma a);                      // l'inverso ("" per nessuna)
bool e_siluro(Arma a);                                    // Mk 48 e Type 53
bool e_missile(Arma a);                                   // Sea Lance, Tomahawk, Harpoon (e il TLAM-C)

// DS:02D1+i; in_ricarica e carico solo nell'Enhanced (ricarica realistica)
enum class StatoTubo : std::uint8_t { vuoto = 0, in_armamento = 2, armato = 3, in_ricarica = 4, carico = 5 };

// Messaggi della stazione (testi dell'EXE, data/motore/testi/messaggi_bordo.tsv)
inline constexpr int MSG_ARMATO = 10;   // tabella di bordo: 10 + tipo d'arma (siluro, missile, esca pronta)
inline constexpr int MSG_TUBO_LIBERO = 220, MSG_IN_ARMAMENTO = 221;
inline constexpr int MSG_TUBO_SILURI = 222, MSG_TUBO_MISSILI = 223, MSG_TUBO_AUSILIARIO = 224;
inline constexpr int MSG_ARMA_ESAURITA = 225, MSG_NON_ARMATA = 226, MSG_SERVE_BERSAGLIO = 227;
inline constexpr int MSG_SOLO_SUPERFICIE = 228, MSG_SERVE_DISTANZA = 229, MSG_TROPPO_VICINO = 230;
inline constexpr int MSG_QUOTA_PERISCOPICA = 231, MSG_NON_ANCORA = 232;
inline constexpr int MSG_NESSUNA_ARMA = 233, MSG_NON_GUIDATA = 234, MSG_ARMA_DIRETTA = 236;
inline constexpr int MSG_BERSAGLIO_ACQUISITO = 237, MSG_NESSUN_BERSAGLIO = 238, MSG_NESSUN_SILURO = 239;
// Enhanced, ricarica realistica (fascia 340-369): 353-359 = tubo 1-7 ricaricato
inline constexpr int MSG_RICARICA_INIZIATA = 348, MSG_RICARICA_IN_CODA = 349, MSG_RICARICA_ANNULLATA = 350;
inline constexpr int MSG_SILENZIO = 351, MSG_FINE_SILENZIO = 352, MSG_RICARICATO = 353, MSG_DISARMATO = 360;
// carico M2: cambio d'arma nel tubo
inline constexpr int MSG_CAMBIO_ARMA = 361, MSG_ARMA_SCELTA = 362, MSG_PRIMA_DISARMARE = 363;
inline constexpr int MSG_MINA_SENZA_POSA = 364, MSG_ARMA_NON_IN_RASTRELLIERA = 365, MSG_MINA_ARMATA = 366;
// posa delle mine (M2b): li emette la missione
inline constexpr int MSG_MINA_POSATA = 367, MSG_MINA_INERTE = 368, MSG_MINA_SCOPPIATA = 369;
// l'Alfa (dossier 19, fascia 440-449): Akkord guasto (nessuna arma), di nuovo in servizio, bersaglio nello slot
inline constexpr int MSG_AKKORD_GUASTO = 440, MSG_AKKORD_RIPRISTINATO = 441, MSG_AKKORD_DESIGNATO = 442;

// Ricarica realistica per classe (dossier 14 §1.5, STIMA)
struct ParametriRicarica {
    double secondi = 300.0;              // dalla rastrelliera al tubo
    int contemporanee = 2;
    double colpo_ogni_s = 60.0;          // un transitorio ogni tanti secondi di lavoro (fuori dal silenzio)
    double colpo_s = 2.0;                // durata del transitorio (evento acustico della missione)
    double fattore_silenzio = 2.0;       // in silenzio la ricarica dura tanto di piu', senza transitori
    bool arma_quando_carico = true;      // a ricarica finita il tubo si arma da solo
    bool tubi_carichi_alla_partenza = true;   // all'attivazione i tubi delle armi prendono l'arma dalle scorte
};
// 688 300 s, Sturgeon 420 (prima del carrello idraulico del 1976), Trafalgar e Victor III 240, Alfa 75; le altre 300
ParametriRicarica parametri_ricarica(const std::string& classe);

// Quello che la stazione chiede alla missione dopo un passo (ricariche finite, armamenti, transitori)
struct EsitoRicarica {
    std::vector<Ordine> alla_missione;   // carica_tubo per armare i tubi appena ricaricati
    std::vector<int> messaggi;
    int transitori = 0;                  // colpi della ricarica in questo passo
};

// Cio' che la stazione legge dalla missione quando un ordine viene consegnato
struct SituazioneArmi {
    double quota = 0.0;                 // m, del giocatore
    // bersaglio selezionato (per i missili del tubo 7): l'unita' esiste ancora?
    bool bersaglio_presente = false;
    bool bersaglio_nave = false;        // categoria 2 dell'originale (solo navi per l'Harpoon)
    bool contatto = false;              // il giocatore ha un contatto sul bersaglio
    bool traccia_solida = false;        // qualita' >= 800 (bit 01h di 990C)
    double distanza_m = 0.0;            // distanza vera (DS:0386)
    // arma selezionata per la guida a filo
    bool arma_presente = false;
    bool arma_guidata = false;          // collegata al giocatore (+0C) e Mk 48 o Type 53 (bit 40h di 990C)
    // Enhanced, missili del catalogo (docs/21, nucleo/missili): per tubo (1-7) 0 = non modellato (i controlli
    // dell'originale), 1 = modellato ma non pronto, 2 = pronto (RDY della preparazione e slide valve libera). Il
    // modello decide al posto di traccia solida e distanza, che bloccherebbero il BOL
    std::array<std::uint8_t, 8> missile_tubo{};
};

// Akkord dell'Alfa (dossier 19 §3.1, §3.4 e §6.5): fino a 3 bersagli designati, uno scelto per le armi; in avaria
// nessuna arma e' utilizzabile (i dati passavano solo per via elettrica), ma la ricarica meccanica (УБЗ) continua

class StazioneArmi {
public:
    static constexpr int TUBI_MAX = 7;
    static constexpr double QUOTA_PERISCOPICA = 6.096;     // 160 ottavi di piede
    static constexpr int DISTANZA_MIN_DECIMI = 50;          // DS:0386 > 49 decimi di nm

    // `tubi`: tipo d'arma per tubo (Missione::tubi()); `dotazione`: scorte per tipo; `numero_tubi`: 7 sul 688, 6
    // sull'Alfa (7 - DS:032F); `limite_in_acqua`: DS:0AFC, 4 sul 688 e 6 sull'Alfa.
    StazioneArmi(const std::vector<std::string>& tubi, const std::map<std::string, int>& dotazione, int numero_tubi,
                 int limite_in_acqua);

    // --- azioni del pannello (tasti R, A, M, L, mouse): ordini da registrare, o un messaggio ---
    // Primo clic: seleziona il tubo (messaggio del tipo di tubo); secondo clic sullo stesso: ordine carica_tubo, che
    // alla consegna arma un tubo vuoto o libera uno in armamento o armato.
    EsitoStazione clic_tubo(int tubo, double tempo);
    Ordine lancio(double tempo) const { return {tempo, TipoOrdine::lancia, 0.0, 0.0, selezionato_}; }

    // --- consegna degli ordini, nel passo di simulazione ---
    // Ordini delle armi (carica_tubo, lancia, designa_bersaglio, seleziona_siluro, guida_*): comandi per la missione
    // o rifiuto. Gli altri ordini passano invariati.
    EsitoStazione consegna(const Ordine& o, const SituazioneArmi& s);
    bool riguarda(const Ordine& o) const;
    // Messaggio della missione: 10-16 = l'arma del tubo 1-7 e' armata (fine del timer dello script). Se il tubo e' in
    // armamento lo arma e restituisce il messaggio da mostrare al suo posto, 10 + tipo d'arma; altrimenti 0 (il tubo
    // e' stato liberato prima: il messaggio si sopprime).
    int messaggio_missione(int codice);
    // Unita' in gioco lanciate dal giocatore (+62 = il giocatore): chi e' uscito dal gioco libera un posto
    // (DS:03DE scende), siluri, esche e missili compresi.
    void unita_lanciate(const std::set<int>& in_gioco);

    // --- stato per il pannello ---
    int numero_tubi() const { return numero_tubi_; }
    int selezionato() const { return selezionato_; }
    StatoTubo stato(int tubo) const { return stato_[indice(tubo)]; }
    Arma arma(int tubo) const { return arma_[indice(tubo)]; }
    int scorte(Arma a) const;
    int in_acqua() const { return in_acqua_; }
    int limite_in_acqua() const { return limite_; }
    std::vector<std::pair<Arma, int>> scorte_tutte() const;   // per tipo, nell'ordine dell'originale (solo i tipi presenti)

    // --- Enhanced: ricarica realistica ---
    // Attiva la ricarica (una volta: StazioniMissione la attiva quando la missione passa all'Enhanced). Con
    // tubi_carichi_alla_partenza i tubi delle armi vuoti prendono l'arma dalle scorte (carico, da armare), salvo
    // che il carico sia gia' stato imposto.
    void attiva_ricarica(const ParametriRicarica& p);
    bool ricarica_realistica() const { return ricarica_attiva_; }
    const ParametriRicarica& parametri_ricarica() const { return parametri_; }
    // Al tempo di simulazione `tempo`: avanza le ricariche, scorre la coda, chiede gli armamenti
    EsitoRicarica avanza(double tempo);
    double secondi_ricarica(int tubo) const;            // mancanti (gia' allungati dal silenzio); 0 se non in ricarica
    bool in_coda(int tubo) const;                        // in ricarica ma in attesa di un posto
    int ricariche_in_corso() const;                      // senza quelle in coda
    int ricariche_max() const { return parametri_.contemporanee; }
    bool silenzio() const { return silenzio_; }
    int bersaglio() const { return bersaglio_; }           // bersaglio selezionato (38h al lancio)
    int arma_selezionata() const { return arma_guidata_; }
    // --- Enhanced: carico della sala siluri (M2) ---
    // Prima del primo passo: posizioni, scorte per variante e tubi carichi; attiva anche la ricarica realistica con
    // `p`. Le posizioni oltre numero_tubi() si ignorano.
    void imposta_carico(const CaricoPartenza& c, const CatalogoCarico& cat, const ParametriRicarica& p);
    bool con_carico() const { return con_carico_; }
    const std::string& variante(int tubo) const;          // arma nel tubo o da caricare ("" senza carico)
    const std::string& lanciata(int tubo) const;          // variante dell'ultimo lancio dal tubo
    bool pozzo(int tubo) const { return valido(tubo) && pozzo_[indice(tubo)]; }
    const std::map<std::string, int>& rastrelliera() const { return rastrelliera_; }
    const std::map<std::string, int>& pozzi() const { return pozzi_; }
    const std::map<std::string, int>& esche() const { return esche_; }
    // le armi del carico in ordine fisso: l'indice e' quello dell'ordine scegli_arma_tubo (-1 se assente)
    const std::vector<std::string>& varianti() const { return varianti_; }
    int indice_variante(const std::string& id) const;
    Ordine scegli_arma(int tubo, const std::string& id, double tempo) const {
        return {tempo, TipoOrdine::scegli_arma_tubo, static_cast<double>(indice_variante(id)), 0.0, tubo};
    }

    // --- Enhanced, Alfa: Akkord ---
    static constexpr int SLOT_AKKORD = 3;
    void attiva_akkord() { akkord_ = true; }   // la stazione dell'Alfa (StazioniMissione)
    bool akkord() const { return akkord_; }
    // i contatti nei tre bersagli (-1 libero) e quello scelto per le armi (1-3, 0 nessuno)
    const std::array<int, SLOT_AKKORD>& bersagli_akkord() const { return slot_; }
    int bersaglio_scelto() const { return scelto_; }
    bool akkord_guasto() const { return akkord_ && guasto_akkord_; }
    // L'avaria (dai danni): il messaggio al cambio (MSG_AKKORD_GUASTO, MSG_AKKORD_RIPRISTINATO), altrimenti 0. In avaria
    // niente armamento, lancio, impostazioni, guida; i tubi armati tornano carichi
    int imposta_guasto_akkord(bool guasto);
    // I contatti che ci sono ancora: gli altri lasciano libero il loro bersaglio
    void aggiorna_akkord(const std::set<int>& presenti);

    // Enhanced: impostazioni del siluro di ciascun tubo, da passare al modello al lancio
    const ImpostazioniSiluro& impostazioni(int tubo) const { return impostazioni_[indice(tubo)]; }
    bool impostato(int tubo) const { return valido(tubo) && impostato_[indice(tubo)]; }   // il giocatore l'ha toccato
    // i campi toccati, uno per bit (1 << CampoSiluro): al lancio gli altri restano quelli che il motore ricava
    std::uint32_t campi_impostati(int tubo) const { return valido(tubo) ? campi_[indice(tubo)] : 0u; }

private:
    static std::size_t indice(int tubo) { return static_cast<std::size_t>(tubo - 1); }
    bool valido(int tubo) const { return tubo >= 1 && tubo <= numero_tubi_; }
    EsitoStazione carica(int tubo, double tempo);
    EsitoStazione carica_enhanced(int tubo, double tempo, bool automatica);
    bool ausiliario(int tubo) const { return arma(tubo) == Arma::esca; }   // i lanciatori delle esche
    // scorte del tubo: quelle della sua arma (classico) o della fonte del carico (rastrelliera, pozzi, esche)
    int scorta_tubo(int tubo) const;
    bool preleva(int tubo);              // una in meno nelle scorte; col carico sceglie la variante
    void restituisci(int tubo);          // l'arma del tubo torna nelle scorte
    std::map<std::string, int>& fonte(int tubo);
    const std::map<std::string, int>& fonte(int tubo) const;
    EsitoStazione scegli_arma_tubo(int tubo, int indice);
    EsitoStazione lancia(int tubo, double tempo, const SituazioneArmi& s);
    EsitoStazione guida(const Ordine& o, const SituazioneArmi& s);
    EsitoStazione designa_akkord(const Ordine& o);
    bool bloccato(const Ordine& o) const;   // Akkord in avaria: l'ordine non passa
    void tubo_successivo();

    int numero_tubi_, limite_, selezionato_ = 1, in_acqua_ = 0;
    int bersaglio_ = -1, arma_guidata_ = -1;
    std::array<StatoTubo, TUBI_MAX> stato_{};
    std::array<Arma, TUBI_MAX> arma_{};
    std::array<int, 10> scorte_{};                          // DS:02DF+t, per Arma (col carico: somma delle varianti)
    std::set<int> lanciate_;
    std::array<ImpostazioniSiluro, TUBI_MAX> impostazioni_{};
    std::array<bool, TUBI_MAX> impostato_{};
    std::array<std::uint32_t, TUBI_MAX> campi_{};
    // ricarica realistica
    bool ricarica_attiva_ = false, silenzio_ = false;
    ParametriRicarica parametri_;
    double tempo_ = 0.0;
    std::array<double, TUBI_MAX> lavoro_{};             // secondi di lavoro fatti sulla ricarica del tubo
    std::array<double, TUBI_MAX> rumore_{};             // secondi di lavoro rumoroso dall'ultimo colpo
    std::vector<int> coda_;                             // tubi in attesa di un posto, nell'ordine delle richieste
    // carico M2
    bool con_carico_ = false;
    std::array<std::string, TUBI_MAX> variante_{}, lanciata_{};
    std::array<bool, TUBI_MAX> pozzo_{};
    std::map<std::string, int> rastrelliera_, pozzi_, esche_;
    std::map<std::string, Arma> famiglia_;              // variante -> famiglia
    std::vector<std::string> varianti_;
    // Akkord (Alfa)
    bool akkord_ = false, guasto_akkord_ = false;
    std::array<int, SLOT_AKKORD> slot_{-1, -1, -1};
    std::array<double, SLOT_AKKORD> designato_il_{};   // per sostituire il meno recente
    int scelto_ = 0;
};

}  // namespace m688
