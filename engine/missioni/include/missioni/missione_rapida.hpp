#pragma once
// Missione rapida procedurale (roadmap punto 10, seconda parte): una missione nel formato 1
// (docs/09_schema_missioni.md) generata da un seme e da pochi parametri. Stesso seme e stessi parametri danno lo stesso
// file, byte per byte, quindi basta scambiarsi il codice missione (13 caratteri in base 32 di Crockford).
//
// Il generatore usa solo i dati (data/motore/missioni_rapide.toml, il fondale del teatro, i kit del giocatore in
// data/motore/missioni_rapide/) e il generatore casuale di nucleo/casuale.hpp, non lo stato del motore. Il file
// generato usa l'API classica, quindi si gioca nei due livelli (il minamento e l'ASW aereo solo nell'Enhanced).
//
// Uso nel menu: componi ParametriRapida (o leggi_codice), genera_missione_rapida, scrivi `lua` in
// cartella_missioni_rapide() / (id + ".lua"), apri la partita con quella cartella, `missione = id`, il seme della
// partita uguale a `parametri.seme`, data e ora della missione, Missione::usa_meteo secondo `meteo_realistico`.
#include <cstdint>
#include <filesystem>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "missioni/scenario_missione.hpp"

namespace m688 {

enum class TipoRapida : std::uint8_t { a_caso, pattuglia, caccia, antinave, scorta, ricognizione, minamento,
                                       inserimento };
enum class StagioneRapida : std::uint8_t { a_caso, inverno, primavera, estate, autunno };
enum class MomentoRapido : std::uint8_t { a_caso, alba, giorno, tramonto, notte };

// I teatri nell'ordine del codice (1-7); "" = a caso
inline constexpr const char* TEATRI_RAPIDE[] = {"faroe", "barents", "celtica", "gibilterra", "otranto", "kunashir",
                                                "baltico"};
const char* nome_tipo_rapida(TipoRapida t);   // "pattuglia", ... ("a_caso")

// La versione del generatore, nel codice. La 2 (29/10/2026): R_nave non va piu' a caccia antisom di una nave di
// superficie. I codici della 1 si leggono ancora e danno gli stessi byte di prima (ParametriRapida::versione)
inline constexpr int VERSIONE_RAPIDE = 2;
inline constexpr int VERSIONE_DATI_RAPIDE = 1;   // quella di missioni_rapide.toml

struct ParametriRapida {
    std::uint32_t seme = 0;
    std::string teatro;                 // "" = a caso, altrimenti una voce di TEATRI_RAPIDE
    StagioneRapida stagione = StagioneRapida::a_caso;
    MomentoRapido ora = MomentoRapido::a_caso;
    bool meteo_realistico = false;      // lo applica l'app (Missione::usa_meteo)
    TipoRapida tipo = TipoRapida::a_caso;
    bool alfa = false;                  // false: 688 (lato usa); true: Alfa (lato urss)
    int forze = 2;                      // 1 poche, 2 medie, 3 molte (0 = a caso)
    int paranoia = 1;                   // 0 distratta, 1 normale, 2 all'erta, 3 paranoica
    int difficolta = 3;                 // 1-5 (0 = a caso)
    bool asw_aereo = true;
    bool carriera = false;              // richiesta di contare per la carriera (vale con difficolta >= 3 e paranoia >= 1)
    int versione = VERSIONE_RAPIDE;     // del generatore: 1 per i codici vecchi, che rigenerano gli stessi byte
    bool operator==(const ParametriRapida&) const = default;
};

struct MissioneRapida {
    ParametriRapida parametri;          // con i campi «a caso» gia' risolti
    std::string codice;                 // il codice dei parametri richiesti, «7KQ2-M9A3-PX4F1»
    std::string id;                     // «RAPIDA_7KQ2M9A3PX4F1»: anche il nome del file
    std::string titolo_it, titolo_en;   // «Operazione GRAY WIND»
    int giorno = 1, mese = 1, anno = 1989;
    double ora_utc = 0.0;               // ore decimali
    bool conta_carriera = false;        // richiesta e soglie rispettate: il file ha il blocco `carriera`
    std::vector<std::string> briefing_it, briefing_en;   // gli stessi del file, per il menu
    std::string lua;                    // il file completo
    ScenarioMissione scenario;          // lo scenario da cui e' scritto: per aprire la rapida nell'editor
};

// Il codice missione: versione, parametri, seme e controllo. Lettura senza distinzione fra maiuscole e minuscole, con
// o senza trattini; O vale 0, I e L valgono 1 (Crockford). nullopt se e' sbagliato o di una versione che non c'e'.
std::string codice_missione(const ParametriRapida& p);
std::optional<ParametriRapida> leggi_codice(std::string_view codice);

// I campi «a caso» risolti dal seme, senza generare (per mostrarli nel menu). `cartella_dati` e' data/motore.
ParametriRapida risolvi(const ParametriRapida& p, const std::string& cartella_dati);
// Il tipo si puo' giocare nel teatro con quella parte (per esempio niente inserimento sulla Piattaforma Celtica)
bool compatibile(const std::string& teatro, TipoRapida tipo, bool alfa, const std::string& cartella_dati);

// Genera la missione. Lancia std::runtime_error se mancano i dati o se teatro e tipo non sono compatibili.
MissioneRapida genera_missione_rapida(const ParametriRapida& p, const std::string& cartella_dati);

// Dove l'app salva le missioni generate: accanto ai fascicoli (nucleo/fascicolo.hpp), «missioni_rapide»
std::filesystem::path cartella_missioni_rapide();

}  // namespace m688
