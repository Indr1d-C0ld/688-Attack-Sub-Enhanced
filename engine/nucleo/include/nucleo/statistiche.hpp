#pragma once
// Statistiche (roadmap punto 6; proposta approvata il 26/10/2026, nota condivisa statistiche-proposta.md): il report
// di una missione, ricalcolato dalla traccia del replay (nucleo/traccia_replay.hpp: la verita' campionata e gli eventi
// del registro M1), e le statistiche cumulate per comandante e globali, salvate in statistiche.toml accanto ad
// achievement.toml. Il livello irradiato e la cavitazione vengono dalla firma della classe (nucleo/acustica) sui campioni
// di velocita' e quota. Vale dove c'e' la traccia (regola di simulazione): il classico non ha statistiche.
// Regole come gli achievement: per il comandante le missioni che contano per la carriera, per i globali tutte quelle
// non riprese. Il modulo e' puro, salvo leggi_statistiche, salva_statistiche e registra_statistiche.

#include <filesystem>
#include <map>
#include <optional>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "nucleo/acustica.hpp"
#include "nucleo/traccia_replay.hpp"

namespace m688 {

// superficie sotto 2 m; periscopica fino a 7,1 m: la quota e' nella scala della missione, dove periscopio e antenna
// escono fino a 6,096 m (QUOTA_PERISCOPICA_MISSIONE_M), piu' il metro di tolleranza delle altre prove della missione
inline constexpr double QUOTA_SUPERFICIE_M = 2.0, QUOTA_PERISCOPICA_MAX_M = 6.096 + 1.0;
inline constexpr double CELLA_MISSIONE_M = 1000.0, CELLA_CUMULATA_M = 2000.0;       // le heatmap

struct ContoArmaStat {
    int lanci = 0, colpi = 0;
};
struct BersaglioStat {
    std::string classe;
    int colpi = 0;
    bool affondato = false;
};
// La prima volta che il nemico ci ha trovati: avvistati, un fix radiogoniometrico dopo una trasmissione, un ping
struct ScopertaNemica {
    double tempo = 0.0;
    std::string come;                    // "avvistato" | "fix_radiogoniometria" | "ping_subito"
    std::string classe;                  // chi
    double distanza_m = 0.0;
};
using Heatmap = std::map<std::pair<int, int>, double>;   // (cella x, cella y) -> secondi

struct ReportMissione {
    std::string missione, teatro, classe;
    // abitudini e tempi
    double durata_s = 0.0, distanza_nm = 0.0;
    double superficie_s = 0.0, periscopica_s = 0.0, cavitazione_s = 0.0;
    double quota_media_m = 0.0, quota_max_m = 0.0, velocita_media_kn = 0.0, velocita_max_kn = 0.0;
    // esposizioni
    double periscopio_s = 0.0, antenna_s = 0.0, schiera_s = 0.0;
    int trasmissioni = 0, ping = 0;
    // armi
    int lanci = 0, colpi = 0, affondati_ostili = 0;
    std::map<std::string, ContoArmaStat> per_arma;      // classe dell'arma ->
    std::map<int, BersaglioStat> per_bersaglio;         // unita' colpite dal giocatore
    // rilevamenti
    std::optional<ScopertaNemica> prima_scoperta;
    int scoperte_nemiche = 0;                            // avvistamenti, fix e ping subiti
    // i contatti scoperti; quelli classificati dal giocatore (la classe, o la categoria data da lui) e le sue
    // classificazioni giuste ed errate, con le regole dell'esito; quelle del sonarista dell'equipaggio a parte
    int contatti = 0, classificati = 0, giuste = 0, errate = 0, classificazioni_sonarista = 0;
    double tempo_classificazione_s = 0.0;                // medio, dalla scoperta alla classificazione (0 se nessuna)
    // esposizione acustica: il livello irradiato a banda larga (dB re 1 uPa a 1 m) ogni minuto
    std::vector<std::pair<double, double>> livello_irradiato;
    double livello_medio_db = 0.0, livello_max_db = 0.0;   // medio in energia; 0 senza la firma
    Heatmap heatmap;                                        // celle di 1 km nelle coordinate della missione
    // docs/26 §3, dagli eventi del registro nella traccia
    int visive_giuste = 0, visive_errate = 0;               // le identificazioni visive del giocatore (libretto)
    int misure_telemetro = 0, osservazioni_aob = 0;
    double errore_telemetro_pct = 0.0, errore_aob_deg = 0.0; // medi, in valore assoluto
    int missili_lanciati = 0, missili_a_segno = 0, neutrali_in_scatola = 0;   // i missili del modello
    int ping_ecoscandaglio = 0;
    int punti_nave = 0;                                     // riusciti; l'errore e' lo spostamento della stima al fix
    double errore_navigazione_medio_m = 0.0, errore_navigazione_max_m = 0.0;
    int allarmi_siluro = 0, siluri_evitati_allarme = 0;     // l'intercettatore; evitati entro 10 minuti dall'allarme
    int mine_scoperte = 0, attraversamenti_mine = 0;
    int verifiche_radio = 0, non_autentici = 0;             // i non autentici riconosciuti
    int collisioni = 0, quasi_collisioni = 0, colpi_artiglieria = 0;
    // il secondo giro (31/10/2026; nota disassemblatore-achievement-31-10-proposta.md §4): dai metadati «conti.*» della
    // traccia (missioni/traccia_missione: metadati_conti), dagli eventi e dai campioni
    double sotto_banchisa_s = 0.0;                          // ghiaccio
    int urti_ghiaccio = 0, emersioni_ghiaccio = 0, avvisi_chiglia = 0;
    int visive_notte_fanali = 0, visive_luci_spente = 0;    // notte: le visive giuste coi soli fanali e a luci spente
    int visive_notte_sagoma = 0, errate_luci_spente = 0;    // le giuste di notte con la sagoma; le errate a luci spente
    double luci_accese_s = 0.0;                             // le luci proprie accese
    double in_coda_s = 0.0;                                 // pedinamento: il tratto in coda piu' lungo
    double distanza_min_sottomarino_m = 0.0;                // da un sottomarino straniero (0: nessuno)
    int classificati_da_sottomarini = 0;                    // le volte che un sottomarino ci ha classificati
    std::map<std::string, int> mine_per_sensore;            // mine: bqs15, bqs8, mg24, mg70 ->
    int mine_ormeggiate = 0, mine_di_fondo = 0, mine_aggirate = 0;
    int sorvoli_elicotteri = 0, sorvoli_aerei = 0, siluri_aerei_evitati = 0;   // ASW aereo
    int salvati = 0;                                        // il soccorso (m688.salvati)
    std::set<std::string> classi_giuste;                    // le classi classificate giuste dal giocatore
};

// Dalla traccia: `giocatore` -1 = il metadato «giocatore» (o, senza, l'unita' con l'id 0); `cat` per il livello
// irradiato e la cavitazione (nullptr: senza)
ReportMissione report_missione(const Traccia& t, const CatalogoAcustico* cat = nullptr, int giocatore = -1);

struct StatisticheCumulate {
    int missioni = 0;
    double durata_s = 0.0, distanza_nm = 0.0, superficie_s = 0.0, periscopica_s = 0.0, cavitazione_s = 0.0;
    double quota_per_tempo = 0.0;                        // integrale della quota (m s): la quota media e' questo / durata
    double periscopio_s = 0.0, antenna_s = 0.0, schiera_s = 0.0;
    int trasmissioni = 0, ping = 0, lanci = 0, colpi = 0, affondati_ostili = 0;
    std::map<std::string, ContoArmaStat> per_arma;
    std::map<std::string, int> affondati_per_classe;
    int missioni_scoperti = 0;                           // missioni in cui il nemico ci ha trovati
    std::map<std::string, std::pair<int, double>> scoperte_per_modo;   // come -> (quante, somma delle distanze)
    int classificati = 0, giuste = 0, errate = 0;
    double tempo_classificazione_tot_s = 0.0;            // somma dei tempi medi pesati per i classificati
    // docs/26 §3 (le medie dalle somme)
    int visive_giuste = 0, visive_errate = 0, misure_telemetro = 0, osservazioni_aob = 0;
    int missili_lanciati = 0, missili_a_segno = 0, neutrali_in_scatola = 0, ping_ecoscandaglio = 0, punti_nave = 0;
    int allarmi_siluro = 0, siluri_evitati_allarme = 0, mine_scoperte = 0, attraversamenti_mine = 0;
    int verifiche_radio = 0, non_autentici = 0, collisioni = 0, quasi_collisioni = 0, colpi_artiglieria = 0;
    double errore_telemetro_pct_tot = 0.0, errore_aob_deg_tot = 0.0, errore_navigazione_tot_m = 0.0, errore_navigazione_max_m = 0.0;
    // il secondo giro (31/10/2026)
    double sotto_banchisa_s = 0.0, luci_accese_s = 0.0, in_coda_max_s = 0.0, distanza_min_sottomarino_m = 0.0;
    int urti_ghiaccio = 0, emersioni_ghiaccio = 0, avvisi_chiglia = 0, visive_notte_fanali = 0, visive_luci_spente = 0;
    int visive_notte_sagoma = 0, errate_luci_spente = 0;
    int classificati_da_sottomarini = 0, mine_ormeggiate = 0, mine_di_fondo = 0, mine_aggirate = 0;
    int sorvoli_elicotteri = 0, sorvoli_aerei = 0, siluri_aerei_evitati = 0, salvati = 0;
    std::map<std::string, int> mine_per_sensore;
    std::set<std::string> classi_giuste;                 // cumulata: le classi diverse
    double errore_telemetro_pct() const { return misure_telemetro > 0 ? errore_telemetro_pct_tot / misure_telemetro : 0.0; }
    double errore_aob_deg() const { return osservazioni_aob > 0 ? errore_aob_deg_tot / osservazioni_aob : 0.0; }
    double errore_navigazione_m() const { return punti_nave > 0 ? errore_navigazione_tot_m / punti_nave : 0.0; }
    // primati
    double quota_max_m = 0.0, velocita_max_kn = 0.0, missione_piu_lunga_s = 0.0;
    double scoperta_piu_lontana_m = 0.0, scoperta_piu_vicina_m = 0.0;   // della prima scoperta nemica (0: mai)
    std::map<std::string, Heatmap> heatmap;              // teatro -> celle di 2 km

    double quota_media_m() const { return durata_s > 0.0 ? quota_per_tempo / durata_s : 0.0; }
    double tempo_classificazione_s() const { return classificati > 0 ? tempo_classificazione_tot_s / classificati : 0.0; }
};
void accumula(StatisticheCumulate& c, const ReportMissione& r);

struct FileStatistiche {
    std::map<std::string, StatisticheCumulate> comandanti;   // id del fascicolo ->
    StatisticheCumulate globali;
};
std::filesystem::path percorso_statistiche();               // <cartella dei fascicoli>/../statistiche.toml
FileStatistiche leggi_statistiche(const std::filesystem::path& p = percorso_statistiche());   // vuoto se manca o rovinato
void salva_statistiche(const FileStatistiche& f, const std::filesystem::path& p = percorso_statistiche());
// A fine missione: le globali (se non ripresa) e il comandante (se conta per la carriera), poi salva. Falso se non ha
// registrato niente (ripresa)
bool registra_statistiche(const ReportMissione& r, const std::string& comandante, bool conta_comandante, bool ripresa,
                          const std::filesystem::path& p = percorso_statistiche());

}  // namespace m688
