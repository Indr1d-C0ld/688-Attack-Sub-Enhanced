#pragma once
// Stato interno dell'ASW aereo nella missione (docs/13_asw_aereo.md, A3): lo usano soltanto missione.cpp e
// missione_asw.cpp. L'interfaccia pubblica e' in missione.hpp (asw_enhanced, voli_asw, boe, messaggi_asw...).

#include <map>
#include <optional>
#include <set>
#include <string>
#include <vector>

#include "missioni/missione.hpp"
#include "nucleo/acustica_asw.hpp"
#include "nucleo/asw_aereo.hpp"
#include "nucleo/casuale.hpp"
#include "nucleo/evasione.hpp"

struct lua_State;

namespace m688 {

// avvisi al giocatore (testi in tools/pipeline/esporta_motore.py, pezzo A5)
inline constexpr int MSG_ASW_IN_ZONA = 380, MSG_ASW_TONFO = 381, MSG_ASW_SORVOLO = 382, MSG_ASW_DIP = 383,
                     MSG_ASW_MARCATORE = 384, MSG_ASW_SUS = 385, MSG_ASW_BOA_ATTIVA = 386, MSG_ASW_SILURO = 387,
                     MSG_ASW_PERISCOPIO = 388, MSG_ASW_RICOGNIZIONE = 389;

// La configurazione di un aereo dalla missione (chiave `aereo_asw`) o da data/missioni/asw/<ID>.lua
struct ConfigAereoAsw {
    int unita = -1;                         // -1: vale per la classe
    std::string classe;
    std::string missione = "caccia";        // caccia, ricognizione, pattuglia
    std::optional<SituazioneAsw> situazione;
    std::vector<int> bersagli;              // vuoto: i sottomarini dell'altra parte
    bool bersaglio_giocatore = false;
    std::optional<bool> armi;
    std::string carico;
    std::optional<LivelloEquipaggio> equipaggio;
    std::optional<Vec2> base, zona;
    int base_unita = -1;
    double raggio_zona_m = 0.0;
    std::optional<double> rotta_presunta;
    std::vector<Cue> cue;
    int capogruppo = -1, indice_gruppo = 0, numero_gruppo = 1;
    // le originali: il sensore classico dello script (tarato sulla missione) resta, accanto alla traccia del controllore
    bool sensore_classico = false;
};

// Le configurazioni della missione (chiave `aereo_asw` o data/missioni/asw/<ID>.lua), senza avviare l'ASW
std::vector<ConfigAereoAsw> configurazioni_asw_missione(lua_State* L, int rif_missione, const std::string& file_missione,
                                                        const std::string& id, double tempo);

struct Missione::StatoAswMissione {
    DatiAsw dati;
    DifficoltaAsw difficolta = DifficoltaAsw::normale;
    Casuale caso{0xA5A};                     // il flusso caso_asw_ (docs/13: nessun flusso nuovo nel classico)
    std::vector<ConfigAereoAsw> configurazioni;
    struct Aereo {
        ControlloreAsw c;
        std::optional<SensoreMad> mad;
        double prossimo = 0.0;               // s di missione del prossimo pensiero
        double quota_m = 0.0;                // vera
        double quota_minima_m = 0.0;         // INCSEA in pace vicino a una nave dell'altra parte (fase 4 D)
        std::set<int> bersagli;              // sottomarini cercati
        bool attivo = true;                  // in gioco
        bool annunciato = false;             // 380 dato
        bool nel_sorvolo = false;            // il giocatore e' nella finestra di Snell
        double bersaglio_visto = -1e18;      // s di missione: l'ultimo albero o battello in superficie visto, al radar o a vista
        std::vector<Rivelazione> in_attesa;  // echi fra un pensiero e l'altro
        std::vector<int> mad_sentiti;
        std::vector<Cue> cue;
        double ultimo_ping_boe = -1e18;      // SSQ-47B: il prossimo ping
        bool sensore_classico = false;
    };
    std::map<int, Aereo> aerei;
    std::set<int> considerate;               // unita' gia' esaminate (aerei o no)
    StatoAscoltoBoe ascolto;
    struct TransitorioAsw { Transitorio x; bool sentito = false; int codice = 0; };
    std::vector<TransitorioAsw> transitori;
    int prossimo_transitorio = 900000;       // id delle sorgenti dei transitori nella scena
    double ultimo_ascolto_transitori = -1e18;
    std::vector<MarcatoreAereo> marcatori;
    std::set<std::size_t> marcatori_visti;
    std::vector<LancioAereo> lanci;
    struct Bomba { int aereo = -1; std::string tipo; Vec2 pos; double quota_scoppio_m = 60.0, scoppio = 0.0, carica_kg = 0.0; };
    std::vector<Bomba> bombe;
    std::map<int, int> marcatrici;           // unita' boa_marcatrice dello script -> id della boa DICASS
    std::vector<Sonoboa> boe_extra;          // boe che non sono di un controllore (le marcatrici)
    std::size_t colpi_visti = 0;             // per dire ai controllori se la loro arma ha colpito
    std::set<int> periscopio_annunciato;     // aerei col radar a modo periscopio gia' segnalati (388)
    // evasione dei sottomarini dell'IA (nucleo/evasione.hpp): le manovre, i transitori gia' valutati, i sorvoli in
    // corso (aereo, battello), un flusso suo
    bool evasione = true;
    std::map<int, Evasione> evasioni;
    std::set<int> transitori_evasione;
    std::map<int, double> ultima_evasione_ping;   // battello -> inizio dell'ultima manovra per un tonfo o un ping
    std::set<std::pair<int, int>> sorvoli_evasione;
    Casuale caso_evasione{0xE7A5};
    std::map<int, int> esche_ia;                       // battello -> esche lanciate
    std::map<int, int> esche_mobili_ia;                // battello -> MG-74 lanciate
    std::map<int, double> ultima_esca_mobile;          // battello -> istante dell'ultima MG-74
    std::map<int, double> ultima_esca;                 // battello -> istante dell'ultima
    std::vector<std::pair<int, double>> esche_attive;  // esca -> fine
};

}  // namespace m688
