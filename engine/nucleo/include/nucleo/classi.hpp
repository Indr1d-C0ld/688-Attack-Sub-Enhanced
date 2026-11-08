#pragma once
// Database delle classi navali (data/motore/classi.toml, generato da tools/pipeline/esporta_motore.py).

#include <map>
#include <string>

namespace m688 {

struct ClasseNave {
    std::string id;              // chiave della tabella, es. "kirov"
    std::string nome;            // nome mostrato
    std::string sprite;          // file degli sprite, relativo alla cartella dei dati
    std::string modello;         // modello 3D per le navi danneggiate (vuoto se manca), relativo ai dati
    std::string scafo;           // sezioni e compartimentazione per la fisica dei danni (vuoto se manca)
    int tipo_originale = -1;     // tipo d'unita' del gioco del 1989, -1 se assente
    double lunghezza_m = 0.0;
    double larghezza_m = 0.0;
    double velocita_max_nodi = 0.0;
    double pescaggio_m = 0.0;    // a pieno carico (sottomarini: in superficie); 0 = non indicato (aerei, armi)
    std::string fonte;
    std::string affidabilita;
    bool civile = false;         // nave civile (ROE, radar di navigazione, traffico): blocco 1 del sonar
    // forme per le collisioni (docs/ricerca/16_collisioni.md §4.1, misurate sui modelli 3D del kit); x lungo lo scafo,
    // prua positiva, 0 a mezza nave
    double altezza_vela_m = 0.0;         // sottomarini: la vela sopra il dorso; 0 = senza vela nota
    double vela_x_m[2] = {0.0, 0.0};     // poppa e prua della vela
    double duomo_m = 0.0;                // navi: sporgenza del duomo sonar sotto la chiglia (compresa in pescaggio_m)
    double duomo_x_m[2] = {0.0, 0.0};    // poppa e prua del duomo
};

// Carica il file; in caso di errore lancia std::runtime_error con il messaggio del parser.
std::map<std::string, ClasseNave> carica_classi(const std::string& percorso);

}  // namespace m688
