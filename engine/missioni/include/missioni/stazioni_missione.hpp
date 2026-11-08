#pragma once
// Stazioni del giocatore (Armi, Sonar) collegate a una missione del livello classico (docs/11_comandi_e_stazioni.md,
// fasi C3-C4): leggono dalla missione cio' che serve ai controlli dell'originale (bersaglio, contatto, distanza vera,
// arma guidata, velocita'), applicano i comandi che ne escono e seguono i messaggi degli script (armamento dei tubi,
// schiera filata o recuperata) e le armi in acqua.

#include <array>
#include <optional>
#include <set>
#include <vector>

#include "missioni/missione.hpp"
#include "nucleo/armi_giocatore.hpp"
#include "nucleo/dinamica_sottomarino.hpp"
#include "nucleo/guasti_giocatore.hpp"
#include "nucleo/sonar_giocatore.hpp"
#include "nucleo/tma.hpp"

namespace m688 {

class DanniSottomarini;

inline constexpr int MSG_TIMONE_BLOCCATO = 214, MSG_IDROPLANI_BLOCCATI = 215;   // Enhanced: sistemi danneggiati
inline constexpr int MSG_TESTA_ATTIVA = 263, MSG_SILURO_AGGANCIO = 264, MSG_SILURO_PERDITA = 265, MSG_SILURO_SPENTO = 266;
inline constexpr int MSG_SERVE_ALBERO = 269;   // trasmissione o satellite senza l'antenna alzata
inline constexpr int MSG_NESSUNA_BOA = 377;    // boa SLOT: finite, o ritardo fuori dai limiti della missione
inline constexpr int MSG_POSATI = 216, MSG_POSARSI_SCHIERA = 217, MSG_POSARSI_RIPIDO = 218, MSG_POSARSI_PROFONDO = 219;

// Boksit dell'Alfa (dossier 19 §3.1 e §6.2): i modi per rotta e quota. MANUALE (barra e angolo) e AUTOMATICO (rotta e
// quota impostate) sono gli ordini dinamici di sempre; PROGRAMMA segue i punti di rotta della carta col pilota
// automatico e a ogni punto prende la quota e la velocita' della sua tappa (se date)
enum class ModoBoksit : std::uint8_t { manuale, automatico, programma };
struct TappaBoksit {
    Vec2 punto;                          // il punto di rotta della tappa
    std::optional<double> quota_m, velocita_kn;
};
inline constexpr int MSG_TAPPA_BOKSIT = 447, MSG_PROGRAMMA_FINITO = 448;   // fascia dell'Alfa 440-449

class StazioniMissione {
public:
    // Tubi e dotazione della missione; 6 tubi e 6 armi in acqua per l'Alfa, 7 e 4 per il 688 (lato DS:032F, DS:0AFC
    // su un PC veloce). `seme`: il rand() dei guasti (nell'originale dall'orologio DOS).
    explicit StazioniMissione(const Missione& m, std::uint32_t seme = 1);

    StazioneArmi& armi() { return armi_; }
    const StazioneArmi& armi() const { return armi_; }
    StazioneSonar& sonar() { return sonar_; }
    const StazioneSonar& sonar() const { return sonar_; }
    const GuastiGiocatore& guasti() const { return guasti_; }
    // Enhanced (regola di simulazione): i guasti e i limiti vengono dai danni del battello (BattelloDanni, docs/10
    // §10) al posto del tiro 1/2 del livello classico
    void usa_danni(DanniSottomarini* d) { danni_ = d; }   // anche per le casse d'assetto (posarsi sul fondo)
    const DanniSottomarini* danni() const { return danni_; }
    // Enhanced: rilevamenti del bersaglio per la TMA, uno ogni 5 s di simulazione finche' c'e' il contatto (con lo
    // scarto del sonar: gaussiano, deviazione `rumore_gradi`); cambiando bersaglio si ricomincia
    void raccogli_tma(bool si, double rumore_gradi = 0.5) { tma_attiva_ = si; rumore_tma_ = rumore_gradi; }
    const Tma& tma() const { return tma_; }
    int bersaglio_tma() const { return bersaglio_tma_; }
    // Il sistema dell'originale non funziona: guasto classico, o efficienza a zero o limite nell'Enhanced
    bool fuori_uso(const Missione& m, SistemaClassico s) const;
    // Quadro di stato: integrita' x 100 / iniziale, al minimo 1 (la voce «n PERCENT OPERATIONAL»)
    int percento_operativo(const Missione& m) const;
    // ANALYZE con i controlli dell'originale: guasto del calcolatore, poi quelli della stazione Sonar
    int analizza(const Missione& m) const;
    SituazioneArmi situazione_armi(const Missione& m) const;
    SituazioneSonar situazione_sonar(const Missione& m) const;

    // Consegna un ordine consegnato dal registro: quelli delle stazioni passano dai loro controlli, i comandi di un
    // sistema guasto vengono rifiutati con il suo messaggio, gli altri vanno alla missione. Falso se la missione non
    // l'ha applicato o se e' stato rifiutato.
    bool consegna(Missione& m, const Ordine& o);
    bool riguarda(const Ordine& o) const {
        return armi_.riguarda(o) || sonar_.riguarda(o) || dinamica(o) || filo(o) || boksit(o);
    }
    static bool boksit(const Ordine& o) { return o.tipo == TipoOrdine::modo_boksit || o.tipo == TipoOrdine::programma_tappa; }
    // Boksit (Alfa, Enhanced): il modo dell'asse (0 rotta, 1 quota), le tappe del programma, la tappa in corso (1..)
    // e quante erano alla partenza (0 senza programma), per «PROGR. 3/7»
    ModoBoksit modo_boksit(int asse) const { return modi_boksit_[asse == 0 ? 0 : 1]; }
    const std::vector<TappaBoksit>& programma_boksit() const { return tappe_; }
    int tappa_boksit(const Missione& m) const;
    int tappe_boksit() const { return programma_in_corso_ ? tappe_programma_ : 0; }
    static bool dinamica(const Ordine& o);   // ordini dell'Enhanced per il modello dinamico (C6a)
    static bool filo(const Ordine& o);       // comandi via filo dell'Enhanced (C6b): comando_filo, spegni, taglia
    // Posarsi sul fondo (Enhanced, docs/11 §6): rifiuto con la schiera fuori, su un pendio oltre `pendenza_max`
    // gradi o oltre la quota di collaudo; altrimenti discesa lenta (3 nodi, 10 gradi) verso un metro dal fondo, e
    // macchine ferme quando il battello ci arriva (dopo_passo). Gli ordini sono da accodare nel registro, cosi' il
    // replay li ripete. Con i danni del battello (regola di simulazione) al fermo imbarca 2 t nelle casse d'assetto e
    // si appoggia; il primo ordine di macchina o di quota le fa scaricare. Vuoto e messaggio 0 nel classico.
    EsitoStazione posarsi(const Missione& m, double quota_collaudo_m, double pendenza_max = 5.0) const;
    const std::optional<ComandiSottomarino>& comandi_dinamica() const { return comandi_; }
    // Enhanced, carico M2 (nucleo/carico.hpp): prima del primo passo applica il carico della sala siluri alla stazione
    // Armi (con la ricarica realistica della classe) e alla missione (famiglie per tubo; il modello del siluro di ogni
    // lancio dalla variante uscita dal tubo). `cat` deve vivere quanto le stazioni.
    void imposta_carico(Missione& m, const CaricoPartenza& c, const CatalogoCarico& cat);
    // Il comando usa un sistema guasto: il messaggio del rifiuto, altrimenti 0
    int rifiuto_per_guasto(const Missione& m, const Ordine& o) const;
    // Dopo ogni passo della missione: messaggi 10-16 (tubo armato), 17 e 18 (schiera), 20, 26 e 30 (guasti, con i
    // loro effetti sulla missione) e armi uscite dal gioco
    void dopo_passo(Missione& m);
    // Messaggi delle stazioni, da mostrare con quelli della missione (che invece salta i 10-16: vedi nascosto())
    const std::vector<Messaggio>& messaggi() const { return messaggi_; }
    void aggiungi_messaggio(const Missione& m, int codice);   // risposte dell'interfaccia (TARGET, ANALYZE, ...)
    static bool nascosto(int codice) { return codice >= 10 && codice <= 16; }   // sostituiti dal messaggio del tipo d'arma

    // TARGET dal periscopio: il contatto del giocatore piu' vicino al rilevamento della vista entro `semiampiezza`
    // gradi (le proprie armi escluse), -1 se nessuno.
    int contatto_in_vista(const Missione& m, double rilevamento, double semiampiezza) const;
    // SELEZ.: il prossimo siluro filoguidato del giocatore dopo quello scelto (in giro), -1 se non ce ne sono
    int prossimo_siluro(const Missione& m) const;

private:
    StazioneArmi armi_;
    const CatalogoCarico* catalogo_ = nullptr;   // con il carico della sala siluri
    StazioneSonar sonar_;
    GuastiGiocatore guasti_;
    DanniSottomarini* danni_ = nullptr;
    int integrita_iniziale_ = 1;   // DS:0F48
    bool tma_attiva_ = false;
    double rumore_tma_ = 0.5, prossimo_tma_ = 0.0;
    int bersaglio_tma_ = -1;
    Tma tma_;
    // Enhanced (C6c): l'ultimo stato dei comandi del giocatore al modello dinamico, rimandato intero a ogni ordine
    std::optional<ComandiSottomarino> comandi_;
    std::optional<double> posarsi_a_;
    bool appesantito_ = false;          // posato: 2 t nelle casse d'assetto, da scaricare al prossimo ordine di manovra
    std::size_t eventi_letti_ = 0;      // eventi dei siluri dell'Enhanced gia' riferiti
    std::set<int> filo_avvisato_;       // siluri per cui si e' gia' detto che il filo sta per finire   // discesa per posarsi in corso (quota di arrivo all'ordine)
    bool consegna_dinamica(Missione& m, const Ordine& o);
    bool consegna_boksit(Missione& m, const Ordine& o);
    void passo_boksit(Missione& m);
    std::array<ModoBoksit, 2> modi_boksit_{ModoBoksit::automatico, ModoBoksit::automatico};
    std::vector<TappaBoksit> tappe_;
    std::optional<Vec2> tappa_applicata_;   // il punto di cui si sono gia' date quota e velocita'
    bool programma_in_corso_ = false;
    int tappe_programma_ = 0;             // i punti di rotta alla partenza del programma
    RandBorland rumore_;           // scarti dei rilevamenti (deterministici col seme)
    std::vector<Messaggio> messaggi_;
    std::size_t letti_ = 0;
};

}  // namespace m688
