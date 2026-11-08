#pragma once
// Pacchetti delle missioni dell'editor (roadmap punto 10, fase E5): un solo file di testo .m688m da scambiare.
//
// Il pacchetto e' il file della missione (scrivi_missione_lua) con in piu' il blocco
//   pacchetto = { formato_pacchetto, formato_missione, formato_editor, libreria, autore, creato,
//                 dipendenze = { teatro, impronta_teatro, classi = { ... }, impronta_classi, dati_rapide },
//                 impronta = "sha256:..." }
// L'impronta e' lo SHA-256 del file con il suo valore azzerato: rivela un file rovinato o modificato a meta' (non e'
// una firma). Le impronte delle dipendenze dicono se chi importa ha lo stesso fondale e gli stessi modelli.
//
// L'importazione, nell'ordine: impronta, formati, scenario letto nell'ambiente isolato (tetto di memoria e di
// istruzioni), dipendenze, funzioni m688 conosciute. Poi il file si RISCRIVE dallo scenario con lo scrittore di questo
// gioco: del pacchetto entra solo lo scenario, mai il suo codice. Se il codice del pacchetto e' diverso da quello
// rigenerato (modificato a mano, o scritto da un'altra versione) lo dice un avvertimento.
#include <filesystem>
#include <optional>
#include <string>
#include <vector>

#include "missioni/scenario_missione.hpp"

namespace m688 {

inline constexpr int FORMATO_PACCHETTO = 1;

// Il testo del pacchetto. `creato`: AAAA-MM-GG. Lancia come scrivi_missione_lua, o std::runtime_error se manca il
// fondale del teatro.
std::string esporta_pacchetto(const ScenarioMissione& s, const DatiScenario& d, const std::string& cartella_dati,
                              const std::string& creato);

struct EsitoImportazione {
    bool importato = false;
    std::string sorgente;                     // il nome del file importato (per l'elenco del menu)
    std::string id, titolo, autore;
    std::filesystem::path file;               // il .lua scritto nella cartella delle importate
    std::vector<std::string> problemi;        // perche' no (errori) o che cosa sapere (avvertimenti), in italiano
    std::optional<ScenarioMissione> scenario;
};

// Controlla il pacchetto e, se va, scrive la missione rigenerata in `cartella_destinazione` / <id>.lua
EsitoImportazione importa_pacchetto(const std::string& testo, const DatiScenario& d, const std::string& cartella_dati,
                                    const std::filesystem::path& cartella_destinazione);

// Le cartelle accanto ai fascicoli: le importate (missioni_importate), dove il giocatore mette i pacchetti ricevuti
// (missioni_importate/da_importare) e le esportate (missioni_esportate)
std::filesystem::path cartella_missioni_importate();
std::filesystem::path cartella_da_importare();
std::filesystem::path cartella_missioni_esportate();

// Importa ogni .m688m di `da`: quelli importati vanno in da/importati, quelli rifiutati in da/rifiutati (con un .txt
// del perche'). Le missioni vanno in `a`.
std::vector<EsitoImportazione> importa_cartella(const std::filesystem::path& da, const std::filesystem::path& a,
                                                const DatiScenario& d, const std::string& cartella_dati);

}  // namespace m688
