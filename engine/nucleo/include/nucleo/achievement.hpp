#pragma once
// Achievement del remake (roadmap punto 15; proposta approvata il 26/10/2026, nota condivisa
// achievement-albo-onore-proposta.md): tattici, di stealth, tecnici, storici, di carriera, dell'editor e nascosti, con
// tre livelli (bronzo, argento, oro) o solo l'oro. Dati in data/motore/achievement.toml (tools/pipeline/achievement.py).
//
// Il modulo e' puro e deterministico. Le condizioni sono dichiarative: un elenco di (misura, operatore, valore) sulle
// misure con un nome che l'adattatore missioni/achievement_missione ricava a fine missione dall'esito, dal registro, dal
// contesto e dal fascicolo. Gli ambiti:
// - missione: la misura di una sola missione (il livello e' quante soglie supera);
// - somma: la misura sommata sulle missioni in cui le condizioni valgono, o il numero di valori diversi di una chiave
//   (`distinti`: missioni originali diverse, codici delle rapide, le due marine);
// - carriera: la misura dal fascicolo (grado, qualifiche, decorazioni).
// Un livello raggiunto non scende. Lo stato e' per comandante e globale: per il comandante conta la missione che conta
// per la carriera (Carriera::conta), per i globali ogni missione non ripresa (decisione della principale, 26/10/2026).

#include <cstdint>
#include <filesystem>
#include <map>
#include <set>
#include <string>
#include <vector>

namespace m688 {

enum class AmbitoAchievement : std::uint8_t { missione, somma, carriera };

struct CondizioneAchievement {
    std::string misura;                  // una misura numerica, o una chiave se `testo` non e' vuoto
    std::string op;                      // >= > <= < == !=
    double valore = 0.0;
    std::string testo;                   // confronto con una chiave (teatro == "barents"): solo == e !=
};

struct DefAchievement {
    std::string id, nome_it, nome_en, descrizione_it, descrizione_en;
    std::string categoria;               // tattico | stealth | tecnico | storico | carriera | editor | nascosto
    AmbitoAchievement ambito = AmbitoAchievement::missione;
    std::string misura;                  // la misura del livello
    std::vector<double> livelli;         // una soglia (solo oro) o tre (bronzo, argento, oro)
    std::vector<CondizioneAchievement> condizioni;
    std::string distinti;                // somma: la chiave dei valori diversi da contare ("" = somma della misura)
    std::string icona;                   // id del medaglione della grafica
    bool nascosto = false;
    // il livello (0 nessuno, 1 bronzo, 2 argento, 3 oro) per un valore della misura
    int livello(double v) const;
};

struct CatalogoAchievement {
    std::vector<DefAchievement> achievement;   // nell'ordine del file
    const DefAchievement* cerca(const std::string& id) const;
};
// data/motore/achievement.toml; std::runtime_error (anche per una misura o una chiave sconosciute)
CatalogoAchievement carica_achievement(const std::string& percorso);
// Le misure e le chiavi che l'adattatore calcola (la pipeline usa lo stesso elenco)
const std::set<std::string>& misure_achievement_note();
const std::set<std::string>& chiavi_achievement_note();

// Le misure di una missione chiusa
struct Misure {
    std::map<std::string, double> v;
    std::map<std::string, std::string> chiavi;   // missione, teatro, marina, codice
    // ambito carriera: le misure del fascicolo prima della missione. Un livello che il fascicolo aveva gia' prima non
    // si assegna (le voci d'ingresso della carriera breve); vuota: nessun confronto
    std::map<std::string, double> prima;
    double operator[](const std::string& k) const {
        const auto it = v.find(k);
        return it == v.end() ? 0.0 : it->second;
    }
};

struct ProgressoAchievement {
    int livello = 0;                     // 0 nessuno, 1 bronzo, 2 argento, 3 oro
    double somma = 0.0;                  // ambito somma
    std::set<std::string> chiavi;        // ambito somma con `distinti`
    std::string data, missione;          // dell'ultimo livello raggiunto ("AAAA-MM" della carriera, id della missione)
};
struct StatoAchievement {
    std::map<std::string, ProgressoAchievement> p;
    int livello(const std::string& id) const;
    double avanzamento(const DefAchievement& d) const;   // la misura accumulata (somma o distinti), per la bacheca
};

struct Sblocco {
    std::string id;
    int livello = 0, prima = 0;          // il livello raggiunto e quello di prima (0 la prima volta)
};

// Valuta una missione chiusa sullo stato: aggiorna somme e livelli e restituisce i livelli saliti, nell'ordine del
// catalogo
std::vector<Sblocco> valuta_achievement(const CatalogoAchievement& c, const Misure& m, StatoAchievement& s,
                                        const std::string& data, const std::string& missione);

// --- il file dello stato: achievement.toml accanto ad albo.toml ---
struct FileAchievement {
    std::map<std::string, StatoAchievement> comandanti;   // id del fascicolo ->
    StatoAchievement globali;
};
std::filesystem::path percorso_achievement();             // <cartella dei fascicoli>/../achievement.toml
FileAchievement leggi_achievement(const std::filesystem::path& p = percorso_achievement());   // vuoto se manca
void salva_achievement(const FileAchievement& f, const std::filesystem::path& p = percorso_achievement());

struct EsitoAchievement {
    std::vector<Sblocco> comandante, globali;
};
// Chiude una missione negli achievement e salva: `comandante` vuoto o `conta_comandante` falso = solo i globali; una
// missione ripresa non conta per nessuno
EsitoAchievement registra_achievement(const CatalogoAchievement& c, const Misure& m, const std::string& comandante,
                                      bool conta_comandante, bool ripresa, const std::string& data,
                                      const std::string& missione,
                                      const std::filesystem::path& p = percorso_achievement());

}  // namespace m688
