#pragma once
// Scena di prova della Fase 1: il 688 a quota periscopica con navi delle classi gia' pronte attorno,
// e il pannello del periscopio sotto la vista.

#include <map>
#include <array>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "grafica/font.hpp"
#include "grafica/periscopio.hpp"
#include "grafica/sagome_armi.hpp"
#include "grafica/sale.hpp"
#include "grafica/tema_stazioni.hpp"
#include "nucleo/classi.hpp"
#include "nucleo/danni.hpp"
#include "nucleo/mondo.hpp"

namespace m688 {

inline constexpr int ID_PROPRIO = 0;

// Popola il mondo: il 688 (id 0) all'origine e le navi disponibili in `classi` a distanze diverse.
void crea_scena_prova(Mondo& mondo, const std::map<std::string, ClasseNave>& classi);

// Pannello Manovra minimo (docs/11_comandi_e_stazioni.md §4, fase C2): posizioni del posto di comando e stato del
// battello del giocatore in missione
struct StatoManovra {
    int telegrafo = 1, timone = 0, barra = 0;
    bool pilota = false, periscopio = false, antenna = false;
    bool radar = false;             // Enhanced: il radar del battello emette (Missione::radar_giocatore_acceso)
    int punti = 0;
    double rotta = 0.0, velocita_kn = 0.0, quota_m = 0.0, quota_ordinata_m = 0.0;
    bool cambia_quota = false;   // la quota ordinata e' diversa e il rateo non e' zero
    // Enhanced con la dinamica (C6a): campane, barra in gradi, giri, assetto e cavitazione
    bool enhanced = false, cavita = false;
    int campana = 3;
    double barra_gradi = 0.0, giri = 0.0, assetto = 0.0, velocita_cavitazione_kn = 0.0;
    double quota_max_m = 450.0;   // quota di collaudo del battello (scala della colonna della quota)
    double piani_gradi = 0.0;     // timoni orizzontali, a scendere positivo (stima dalla quota ordinata)
    // Alfa (Boksit): modo di rotta e di quota (0 manuale, 1 automatico, 2 programma) e la tappa del programma
    int modo_rotta = 1, modo_quota = 1, tappa = 0, tappe = 0;
    bool tan = false, sfoga = false, guasto_casse = false;   // Alfa (ПОКС e Tan, casse del runtime)
    // Enhanced sul 688: le spie della quota periscopica e della schiera trainata (DatiGoverno; -1 nessuna spia)
    int quota_periscopica = -1, schiera = -1;
};

// Pannello Armi minimo (docs/11_comandi_e_stazioni.md §4, fase C3): tubi, scorte, armi in acqua, bersaglio e siluro
// filoguidato scelto
struct StatoArmi {
    struct Tubo {
        std::string arma;         // "MK48", "ESCA", ...
        int stato = 0;            // 0 vuoto, 2 in armamento, 3 armato (DS:02D1+i); Enhanced 4 in ricarica, 5 carico
        double secondi = 0.0;     // Enhanced: secondi mancanti alla fine della ricarica
        bool coda = false;        // Enhanced: ricarica in attesa di un posto (al piu' due insieme)
        SagomaInTubo sezione;     // il tubo in sezione con l'arma (kit, grafica/sagome_armi.hpp)
        double ricarica = -1.0;   // Enhanced: frazione della ricarica 0-1 (< 0 nessuna)
    };
    std::vector<Tubo> tubi;
    int selezionato = 1;
    std::vector<std::pair<std::string, int>> scorte;
    int in_acqua = 0, limite = 4;
    std::string siluro;              // il siluro filoguidato scelto, breve ("" = nessuno)
    std::string impostazioni, filo;  // Enhanced: impostazioni del tubo selezionato; fase e filo del siluro scelto
    bool ricarica = false, silenzio = false;   // Enhanced: ricarica realistica e silenzio a bordo (ricariche lente)
    // Alfa (Akkord): i tre bersagli designati (lettera del contatto, "" libero), lo slot scelto (0 nessuno) e l'avaria
    std::array<std::string, 3> akkord;
    int akkord_scelto = 0;
    bool akkord_guasto = false;
};

// Pannello Sonar minimo (fase C4): l'azione possibile sulla schiera, il filtro, i pulsanti
struct StatoSonar {
    std::string schiera;            // "FILA", "RECUPERA", "PERSA" (tabella DS:0294 dal suo stato)
    std::string manovra;            // "IN FILATURA", "IN RECUPERO" o ""
    bool ascolto_schiera = false;   // bit 0 di +54: la schiera conta per i contatti
    bool filtro = false;            // passa alto
    bool enhanced = false;          // stazione completa: i tasti nella riga d'aiuto
    bool slot = true;               // slot del quaderno (S Y C): non sull'Alfa del 1989 (dossier 19 §6.6)
};

// Pannello Navigazione (fase C4): punti di rotta, pilota, proiezione; la carta e' nella vista
struct StatoNavigazione {
    int punti = 0, minuti = 10, fattore = 3;
    bool pilota = false, proiezione = false, tutte_le_scie = false, scia_bersaglio = false;
    // strumento MATE (Enhanced, T): la soluzione proposta e il suo scarto sui rilevamenti raccolti
    bool tma = false;
    double tma_distanza_nm = 0.0, tma_rotta = 0.0, tma_velocita_kn = 0.0, tma_scarto = 0.0;
    int tma_rilevamenti = 0;
    std::optional<double> tma_rateo;   // gradi/min negli ultimi 2 minuti
    bool strumenti = false;            // Enhanced: gli strumenti della carta (N2) nella riga d'aiuto
    // Enhanced: la pagina del quadro del fondale (Ctrl+H) aperta, con la riga dell'ecoscandaglio
    bool fondale = false;
    std::string fondale_riga;
};

// Quadro di stato (F3, numeri_eseguibile.md §5.4): i sei sistemi e «n PERCENT OPERATIONAL» o «HULL BREACHED»
struct StatoSistemi {
    struct Voce {
        std::string nome;
        bool guasto = false;
        int percento = -1;                              // Enhanced: efficienza (-1 nel classico: OK o GUASTO)
    };
    std::vector<Voce> guasti;
    int percento = 100;
    bool scafo_violato = false;                         // dopo il messaggio 53
};

// Centrale (F9, Fase 3 S1): stato rapido del battello e menu delle stazioni
struct StatoCentrale {
    double quota_m = 0.0, rotta = 0.0, velocita_kn = 0.0;
    int contatti = 0, tracce = 0, armi_in_acqua = 0, guasti = 0, percento = 100;
    bool pausa = false, periscopio = false, antenna = false, schiera = false, radar = false;
};

// Periscopi dell'Enhanced (F5, docs/14 P4): il periscopio scelto fra quelli della classe, la sua corsa, ingrandimento,
// modo, elevazione, l'esposizione in corso col contatore e, nel modo moderno, la lista della guardata NWP
struct StatoPeriscopi {
    std::vector<std::string> nomi;      // i periscopi della classe, il primo d'attacco
    int scelto = 0;
    std::string corsa;                  // GIU', IN SALITA 40, SU, IN DISCESA, AVARIA
    std::string ingrandimento, modo, filtro;
    double elevazione = 0.0, esposto_s = 0.0, tremolio = 0.0;
    bool su = false, finestra_coperta = false, foto = false, danneggiato = false;
    bool nwp = false;                   // lista della guardata (modo moderno)
    int giri = 0;                       // giri completi d'orizzonte nella guardata
    bool aria = false;                  // ricerca aerea fatta (elevazione oltre il primo fermo)
    bool telecamera = false, telecamera_accesa = false;   // Alfa: la telecamera della vela, col pulsante 7
};

// Sala radio (F2, pezzo S4): pulsanti del classico (periscopio, antenna, rilettura) o dell'Enhanced (antenna, filo
// galleggiante, archivio, ordini, tipo di trasmissione, trasmetti, satellite)
struct StatoRadio {
    bool enhanced = false, periscopio = false, antenna = false, radar = false;
    std::string filo;               // Enhanced: azione possibile sul filo galleggiante
    std::string trasmissione;       // Enhanced: tipo della prossima trasmissione
    bool solo_ordini = false;
    // Fase 3, pannello radio/ESM del kit (Enhanced con le consolle): pulsanti brevi, telescrivente e ESM
    std::string filo_breve;         // FILO, RECUP., FILATA, ROTTO
    bool filo_filato = false;
    int ritardo_min = 10;           // della boa SLOT
    std::vector<std::string> telescrivente;   // le ultime righe, la piu' recente in fondo
    std::vector<float> panoramica;  // ESM: livello 0-1 per frequenza, vuota con l'albero giu'
    std::vector<std::string> intercettazioni;
    bool minaccia = false;          // un'intercettazione a rischio probabile o peggio
};

struct StatoPannello {
    double tempo_s = 0.0;          // ora UTC del giorno, s
    double compressione = 1.0;
    double elevazione_sole = 0.0;  // gradi
    std::string missione;          // titolo della missione in corso (vuoto nella scena di prova)
    std::string messaggio;         // ultimo messaggio di bordo
    bool due_righe = false;        // Enhanced (punto 21): il messaggio lungo va a capo invece di tagliarsi
    std::optional<StatoManovra> manovra;   // in missione: pannello Manovra al posto di quello della scena di prova
    std::optional<StatoArmi> armi;         // in missione, con la stazione Armi (F7) scelta: pannello Armi
    std::optional<StatoSonar> sonar;       // ... Sonar (F8)
    std::optional<StatoNavigazione> navigazione;   // ... Navigazione (F6)
    std::optional<StatoSistemi> stato;     // ... Stato (F3)
    std::optional<StatoCentrale> centrale; // ... Centrale (F9)
    std::optional<StatoRadio> radio;       // ... Radio (F2)
    std::optional<StatoPeriscopi> periscopi;   // ... Periscopio (F5) dell'Enhanced coi periscopi veri
    std::string barra_titolo, barra_dati;  // barra del bersaglio (8213): vuota senza bersaglio
    // Fase 3 (Enhanced, docs/ricerca/19): i pannelli delle consolle del kit (grafica/sale) per Manovra e Armi, coi
    // font del battello; nullptr: i pannelli di oggi
    const FontConsolle* consolle = nullptr;
    BattelloTema battello = BattelloTema::usa688;
    bool consolle_vista = false;   // anche la vista ha pezzi di consolle (centrale, cornice del periscopio, Sozh)
};

// Il tema delle consolle: la luce della sala e' rossa di notte sul 688 (luce della vista sotto quella del sole a -6°,
// coperto compreso), bianca sull'Alfa
TemaStazione tema_consolle(BattelloTema b, double luce);
// Il pannello Navigazione sulla lamiera (gruppo 5): l'Enhanced con le consolle e gli strumenti della carta (688 e Alfa)
bool navigazione_lamiera(const StatoPannello& s);
// Il tema del fotogramma (nessuno se pannello e vista sono quelli di oggi). La palette lo applica dopo la luce della
// vista (indici 16-47)
std::optional<TemaStazione> tema_pannello(const StatoPannello& s, double luce);

// Colpo di prova per la fisica dei danni (nucleo/danni.hpp) su una nave della scena: siluro_chiglia (Mk 48 a 6 m
// sotto la chiglia, 544 kg di TNT per la bolla), siluro_fianco, siluro_prua, siluro_poppa (300 kg a contatto),
// due_siluri, missile (165 kg sopra il galleggiamento). Lancia se il caso e' sconosciuto.
void colpo_di_prova(DanniNave& n, const std::string& caso);

// Area attiva del pannello per il mouse (Fase 3 S1): un clic dentro vale il comando (nomi di nucleo/mappa_tasti)
struct AreaAttiva {
    int x = 0, y = 0, w = 0, h = 0;
    std::string comando;
    bool contiene(int px, int py) const { return px >= x && py >= y && px < x + w && py < y + h; }
};

// `aree`: se c'e', riceve le aree attive del pannello disegnato
void disegna_pannello(Tela& t, const Font& f, const Nave& proprio, const VistaPeriscopio& v, const StatoPannello& s,
                      std::vector<AreaAttiva>* aree = nullptr);

}  // namespace m688
