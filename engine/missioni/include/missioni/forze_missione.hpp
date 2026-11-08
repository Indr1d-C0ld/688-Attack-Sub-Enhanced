#pragma once
// Stato interno delle forze dell'Enhanced (fase 3 delle interazioni fra IA, docs/18_interazioni_ia.md; dossier
// docs/ricerca/24_interazioni_fra_unita.md §8.1, §4.6, §8.7): allerta A0-A3 per parte e per formazione, situazione
// per coppia di parti con le provocazioni, regole d'ingaggio dell'IA. Lo usano soltanto missione.cpp e
// missione_forze.cpp; l'interfaccia pubblica e' in missione.hpp (forze_enhanced, allerta, situazione_fra, ...).

#include <array>
#include <cstdint>
#include <map>
#include <set>
#include <string>
#include <vector>

#include "missioni/missione.hpp"
#include "nucleo/carriera.hpp"
#include "nucleo/casuale.hpp"

namespace m688 {

// data/motore/interazioni.toml, sezioni [allerta] e [roe_ia] (tools/pipeline/interazioni_forze.py)
struct DatiForze {
    struct Evento {
        int formazione = 0, formazione_max = 3, forza = 0, forza_max = 3;   // quanto sale, fino al tetto
        int formazione_fino_a = -1, forza_fino_a = -1;                      // almeno questo livello (-1: niente)
    };
    double raggio_formazione_m = 20000.0, discesa_s = 2700.0;
    int minimo_crisi = 1, minimo_guerra = 2, posto_combattimento = 2, emcon_combattimento = 3;
    std::array<double, 4> falsi_contatti{1.0, 1.5, 2.0, 3.0};
    std::map<std::string, Evento> eventi;
    // il ritardo con cui il resto della forza sa di un evento (fino alla fase B, quella dei messaggi): il FLASH dalla
    // formazione al teatro, per parte, e la sua perdita
    std::map<int, std::pair<double, double>> teatro_s;   // lato -> [minimo, massimo]
    double teatro_perdita = 0.05;
    double neutrali_a3 = 0.3;
    // [messaggi.*] (24 §8.2): ritardi [minimo, massimo] in s e perdite per parte
    struct Canali {
        std::pair<double, double> formazione_s{30.0, 120.0}, aereo_nave_s{60.0, 180.0}, flash_s{300.0, 600.0}, immediate_s{900.0, 1800.0};
        double formazione_perdita = 0.02, aereo_nave_riuscita = 1.0;
    };
    std::map<int, Canali> canali;   // lato -> canali
    double riferire_ogni_s = 600.0, r0_m = 1852.0, ellisse_max_m = 10000.0;
    std::pair<double, double> broadcast_s{600.0, 900.0};
    std::map<std::string, double> velocita_kn{{"sottomarino", 20.0}, {"nave", 30.0}, {"aereo", 0.0}, {"sconosciuto", 20.0}};
    // [zone] (24 §8.3): l'area d'azione mobile di un sottomarino amico, in miglia; la P di fuoco amico nella prima ora
    // dopo un attacco nemico, per parte
    double avanti_nm = 15.0, indietro_nm = 30.0, lato_nm = 5.0, dopo_attacco_s = 3600.0;
    std::map<int, double> fuoco_amico{{0x08, 0.05}, {0x10, 0.15}};
};
DatiForze carica_dati_forze(const std::string& percorso);   // file mancante o senza sezioni: i valori qui sopra

// la quota d'antenna: chi puo' ascoltare il broadcast, cioe' gli ordini radio e il quadro (la banchisa la guarda chi chiama)
inline constexpr double QUOTA_ANTENNA_M = 6.096 + 0.5;   // a quota periscopica il battello ascolta il broadcast e trasmette
inline bool a_quota_antenna(const UnitaClassica& u) { return (u.flag54 & 6) != 0 || u.quota <= QUOTA_ANTENNA_M; }

struct Missione::StatoForze {
    DatiForze d;
    Situazione base = Situazione::guerra;
    bool base_esplicita = false;   // dalla carriera, dall'editor o dalle originali; o dichiarata in partita
    std::map<std::pair<int, int>, Situazione> coppie;   // (lato minore, lato maggiore) -> dichiarata o provocata
    struct Livello {
        int v = 0, iniziale = 0;
        double ultimo = -1e18;   // l'ultimo evento (per la discesa)
    };
    std::map<int, Livello> forza;     // lato -> livello della forza
    std::map<int, Livello> unita;     // unita' -> livello della sua formazione
    std::map<int, int> iniziale_unita;   // la `paranoia` dell'unita' nell'editor
    int iniziale = 1;
    struct InArrivo {
        double t;
        int lato;
        std::string evento;
    };
    std::vector<InArrivo> in_arrivo;   // eventi che il resto della forza sapra' al FLASH
    std::size_t eventi_visti = 0, impatti_visti = 0;
    std::set<std::pair<int, int>> classificazioni;   // (ascoltatore, sorgente) gia' contate come PROBSUB
    std::set<int> combattimento, emcon;               // unita' gia' portate al posto di combattimento e all'EMCON
    // i falsi contatti dei sonaristi dell'IA con l'allerta (24 §8.1): il profilo di partenza e il livello applicato
    std::map<int, std::pair<ProfiloSonarista, int>> profili;
    std::map<std::pair<int, int>, bool> neutrali;     // (chi, neutrale): la P di A3 estratta una volta
    // messaggi di contatto (fase B): l'ultimo rapporto di chi riferisce su chi (tempo, livello), quello del teatro per
    // parte e bersaglio, le consegne in viaggio, il quadro radio di chi riceve, il ciclo del broadcast per parte
    std::map<std::pair<int, int>, std::pair<double, int>> riferiti;
    std::map<std::pair<int, int>, double> teatro;
    struct Consegna {
        double t;
        int ricevente;   // -2: il teatro della parte, -3: i sottomarini della parte al broadcast
        int lato;
        RapportoContatto r;
        bool broadcast = false;   // un sottomarino: riceve al ciclo, con l'antenna o a quota periscopica
    };
    std::vector<Consegna> in_viaggio;
    // D3, il falso ordine radio (missione_forze.cpp, passo_ordini_falsi): per (intervento, unita') come l'ha preso
    struct OrdineFalsoRicevuto {
        enum class Esito : std::uint8_t { creduto, ignorato, verifica, smascherato, perso } esito = Esito::creduto;
        double t = 0.0;            // la ricezione
        double risposta = -1.0;    // la verifica: quando arriva la risposta del comando (< 0: mai)
    };
    std::map<std::pair<int, int>, OrdineFalsoRicevuto> ordini_falsi;
    // D4, il cambio di parte: per unita' la parte di prima, da quando ogni parte lo sa e chi lo sa di persona (la sua vittima)
    struct CambioParte {
        int lato_prima = 0;
        std::map<int, double> sapra;   // lato -> da quando
        std::set<int> sanno;
    };
    std::map<int, CambioParte> cambi_parte;
    std::map<std::pair<int, int>, ContattoRadio> quadro;   // (ricevente, bersaglio)
    std::map<int, double> ciclo_s;                          // lato -> periodo del broadcast
    std::map<int, std::size_t> rapporti_giocatore;          // serie del broadcast del giocatore -> indice in per_giocatore
    std::set<int> flash_visti;                              // serie dei FLASH della missione gia' noti all'IA (fase 4)
    std::vector<RapportoContatto> per_giocatore;
    std::vector<MessaggioIa> messaggi;
    // il quadro radio ai sottomarini dell'IA (passo_radio_sottomarini): acceso fuori dalle originali, con le armi dell'IA;
    // per battello l'ascolto del broadcast e la caccia sul dato. Numeri casuali a parte: caso resta com'era
    bool quadro_sottomarini = false;
    struct RadioSottomarino {
        double esposto_da = -1.0;       // a quota d'antenna da quando (< 0: no), per la copia del messaggio
        double ultimo_ascolto = 0.0;    // la prima finestra dopo un intervallo dall'inizio, come il comandante IA
        bool salita = false, antenna_nostra = false;
        double salita_da = 0.0, ciclo = 0.0, quota_prima = 0.0, velocita_prima = 0.0, velocita_data = 0.0;
        double ciclo_tentato = -1.0;    // il ciclo dell'ultima salita: chiusa da altri, non si ritenta prima del ciclo dopo
        int caccia = -1;                // il bersaglio del dato radio che sta seguendo (< 0: nessuno)
        Vec2 verso;
        double velocita_caccia = 0.0, velocita_prima_caccia = 0.0, rotta_prima_caccia = 0.0, caccia_non_prima = 0.0;
        std::map<int, double> lasciati; // bersaglio -> il fix del dato gia' seguito a vuoto: serve un rapporto piu' nuovo
        double ultima_trasmissione = -1e18;   // l'ultimo rapporto trasmesso: piu' contatti nello stesso giro valgono un lancio di dado
    };
    std::map<int, RadioSottomarino> radio_sottomarini;
    Casuale caso_radio{0xA4D10u};
    // zone (fase C): da quando la parte e' stata attaccata, le zone che un'unita' non conosce (messaggio perso), la P
    // di fuoco amico estratta una volta per coppia (chi, bersaglio)
    std::map<int, double> attaccata;
    std::map<std::pair<int, int>, bool> zona_ignota, ignora_zona;
    std::vector<FuocoAmicoIa> fuoco_amico;
    double prossimo_giro = 0.0;
    std::vector<CambioAllerta> cambi;
    std::vector<Provocazione> provocazioni;
    Casuale caso{0xF02CEu};
};

}  // namespace m688
