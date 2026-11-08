#pragma once
// Tasti rimappabili (docs/11_comandi_e_stazioni.md §5 e §11, fase S1): ogni tasto ha un nome leggibile in italiano
// ("Tastierino 4", "Ctrl+P", "F7", "W", "[") ed e' legato a un comando dal nome parlante ("timone_sinistra",
// "stazione_armi", ...). I predefiniti sono quelli del manuale e degli ordini rapidi; un file TOML li cambia:
//
//   [tasti]
//   timone_sinistra = "Tastierino 4"
//   lancia = ["L", "Ctrl+L"]          # piu' tasti per lo stesso comando
//   antenna = ""                       # nessun tasto
//
// Un comando elencato nel file perde i tasti predefiniti e prende quelli del file; un tasto preso da un altro
// comando viene tolto a quello. I tasti tenuti premuti (frecce, Maiuscolo) restano fissi.

#include <map>
#include <string>
#include <string_view>
#include <vector>

namespace m688 {

struct VoceComando {
    std::string_view id;            // nome del comando nel file
    std::string_view descrizione;   // per la tabella in italiano
};

// Tutti i comandi rimappabili, nell'ordine della tabella
const std::vector<VoceComando>& comandi_rimappabili();

class MappaTasti {
public:
    static MappaTasti predefinita();
    // Applica un file TOML ([tasti]); lancia std::runtime_error con il motivo su un comando sconosciuto o un valore
    // non valido
    void carica(const std::string& percorso);
    void applica_testo(const std::string& toml);   // come carica, dal testo (prove)

    // Comando del tasto premuto ("" se nessuno)
    std::string comando(const std::string& tasto) const;
    std::vector<std::string> tasti(std::string_view comando) const;
    // Il file con tutti i comandi e i loro tasti, commentato: modello da copiare e modificare
    std::string come_toml() const;

private:
    void lega(const std::string& tasto, const std::string& comando);
    std::map<std::string, std::string> tasto_comando_;
};

}  // namespace m688
