#pragma once
// Enciclopedia dell'Enhanced (requisito 14, Museo; data/motore/enciclopedia.toml dal kit, tools/pipeline/enciclopedia.py):
// una voce per ogni cosa della simulazione (sottomarini, navi, aerei, armi, sensori, periscopi, marcatori, sistemi di
// bordo, teatri e basi), con la scheda tecnica dai cataloghi del motore (fonte e confidenza per riga), il testo storico
// e d'impiego in italiano e in inglese, il manuale di bordo (uso), i riferimenti incrociati, le immagini (descrittori
// per disegnarle: sprite, modello, sagoma, oculare, effetto, cascata, carta, spaccato, foto) e i crediti delle foto.
//
// Solo lettura: il Museo (app/museo) e la consultazione in missione la usano cosi' com'e'.

#include <map>
#include <optional>
#include <string>
#include <vector>

namespace m688 {

struct RigaScheda {
    std::string campo, nome_it, nome_en, testo_it, testo_en, unita, fonte, conf;
    std::optional<double> valore;               // per ordinare e confrontare (le righe di solo testo non l'hanno)
    std::string gioco_testo_it, gioco_testo_en; // il valore del gioco del 1989, se diverso dal reale
    std::string testo_lat;                      // traslitterazione delle righe in cirillico (il font non l'ha)
};

struct RiferimentoVoce {
    std::string relazione, voce;                // «armi», «portata da», «sensori»...; id della voce («siluro.mk48»)
};

// Un'immagine della voce: il tipo e i suoi parametri, cosi' come sono nel file (numeri, testi, vero/falso)
struct ImmagineVoce {
    std::string tipo;                           // sprite, modello, sagoma, oculare, effetto, cascata, carta, spaccato, foto
    std::string didascalia_it, didascalia_en;
    std::map<std::string, std::string> testi;   // file, arma, periscopio, modo, effetto, oggetto, classe, licenza...
    std::map<std::string, double> numeri;       // aob, elevazione, ingrandimento, lat, lon...
    std::map<std::string, bool> flag;           // dritta...
    std::string testo(const std::string& k, const std::string& predefinito = "") const;
    double numero(const std::string& k, double predefinito = 0.0) const;
    bool vero(const std::string& k, bool predefinito = false) const;
};

struct CreditoVoce {
    std::string titolo, autore, licenza, url;
};

struct VoceEnciclopedia {
    std::string id, nome, nome_en, nome_lat, categoria, gruppo, gruppo_en, nazione;
    std::string testo_it, testo_en, uso_it, uso_en;
    std::vector<std::string> fonti, armi_tipiche_altre;
    std::vector<RigaScheda> scheda;
    std::vector<RiferimentoVoce> riferimenti;
    std::vector<ImmagineVoce> immagini;
    std::vector<CreditoVoce> crediti;
    // il nome da mostrare col font del gioco: la traslitterazione se il nome e' in cirillico
    const std::string& nome_mostrato(bool inglese = false) const;
};

struct Enciclopedia {
    std::map<std::string, VoceEnciclopedia> voci;
    std::vector<std::string> gruppi;            // nell'ordine del Museo (quello del file)
    const VoceEnciclopedia* voce(const std::string& id) const;
    std::vector<const VoceEnciclopedia*> del_gruppo(const std::string& gruppo) const;   // per nome
    bool vuota() const { return voci.empty(); }
};

// std::runtime_error se il file manca o non si legge
Enciclopedia carica_enciclopedia(const std::string& percorso);

}  // namespace m688
