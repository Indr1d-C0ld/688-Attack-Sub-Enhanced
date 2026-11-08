#pragma once
// Le armi dell'IA (fase 4 delle interazioni fra IA, docs/18_interazioni_ia.md; inventario §5-6): il carico per classe e
// anno con le varianti, la scelta dell'arma per il bersaglio, e la guida a filo dei siluri dell'IA. Come per il
// giocatore: lo stesso catalogo (data/motore/carico.toml, nucleo/carico.hpp), gli stessi siluri (nucleo/siluro.hpp) e
// gli stessi comandi via filo.
// - Carico: le classi con un locale in carico.toml (688 e Alfa) usano i suoi carichi standard; le altre il carico di
//   [armi_ia.<classe>] di data/motore/interazioni.toml (tools/pipeline/interazioni.py; dossier 01 §2, 14 §1). Le
//   varianti non ancora in servizio nell'anno (o ritirate) diventano la predefinita della famiglia, poi la piu' recente
//   della stessa famiglia in servizio; le what-if e le nucleari non si caricano.
// - Scelta: fra le armi nei tubi, quella che arriva al bersaglio con piu' stelle d'efficacia contro di lui
//   (SchedaArma::stelle); a parita' la filoguidata se il lanciatore puo' guidare, poi la piu' lunga.
// - Guida a filo (GuidaFiloIa, dossier 02 §6.2 punto 4 e §6.4): corregge la rotta verso la posizione stimata del
//   bersaglio (o la linea del rilevamento) quando lo scarto supera la soglia; rifiuta un'eco stazionaria lontana dalla
//   stima (un'esca); alla perdita del contatto riporta il siluro sulla stima nuova in attivo; taglia dopo l'aggancio
//   finale, quando il lanciatore deve evadere o quando il filo e' a rischio. La dottrina dipende dall'esperienza del
//   sonarista ([filo_ia.<esperienza>]): la recluta corregge di rado e non rifiuta le esche.
// Il modulo e' puro e deterministico: decide, e chi comanda l'unita' passa i comandi al siluro.

#include <map>
#include <optional>
#include <set>
#include <string>
#include <vector>

#include "nucleo/carico.hpp"
#include "nucleo/geometria.hpp"
#include "nucleo/manovre_tma.hpp"
#include "nucleo/siluro.hpp"
#include "nucleo/sonarista.hpp"

namespace m688 {

struct DatiArmiIa {
    std::string classe, nazione;
    int tubi_533 = 0, tubi_650 = 0;           // i tubi da 650 restano vuoti finche' il 65-76 non e' nel catalogo
    std::map<std::string, int> armi;          // variante -> quante (nei tubi e di riserva)
    std::vector<std::string> tubi;            // nei tubi alla partenza
    std::string fonte, confidenza;
};

struct DottrinaFilo {
    double correzione_s = 20.0;               // ogni quanto si guarda la rotta del siluro
    double soglia_deg = 8.0;                  // scarto oltre cui si corregge
    double taglio_dopo_aggancio_s = 30.0;     // dall'aggancio della testa al taglio
    double distanza_taglio_m = 1500.0;        // o quando il siluro e' cosi' vicino alla stima, agganciato
    bool rifiuta_esche = true;
    double scarto_esca_deg = 15.0;            // un'eco stazionaria piu' lontana di cosi' dalla stima e' un'esca
    bool attivo_su_perdita = true;            // contatto perso: la testa in attivo e la rotta sulla stima
    std::string fonte, confidenza;
};

struct CatalogoArmiIa {
    std::map<std::string, DatiArmiIa> classi;
    std::map<std::string, DottrinaFilo> filo;   // recluta, medio, esperto
};
// [armi_ia.*] e [filo_ia.*] di interazioni.toml; lancia se manca o se manca un livello d'esperienza
CatalogoArmiIa carica_armi_ia(const std::string& percorso);
const DottrinaFilo& dottrina_filo(const CatalogoArmiIa& c, Esperienza e);

// Le sezioni del nucleo di interazioni.toml insieme ([tma.*], [armi_ia.*], [filo_ia.*]); le altre sono del runtime
struct CatalogoInterazioni {
    CatalogoTma tma;
    CatalogoArmiIa armi;
};
CatalogoInterazioni carica_interazioni(const std::string& percorso);

// --- carico ---
struct CaricoIa {
    std::vector<std::string> tubi;            // varianti nei tubi da 533 ("" vuoto)
    std::map<std::string, int> riserva;       // varianti in rastrelliera e nei pozzi
    int tubi_650_vuoti = 0;
    std::string fonte;                        // da dove viene: il carico standard o [armi_ia]
    int totale(const std::string& variante) const;   // nei tubi e di riserva
};
// Il carico della classe nell'anno; `tipo_missione` vale per le classi con i carichi standard (vuoto: il predefinito).
// Vuoto per le classi senza dati
CaricoIa carico_ia(const CatalogoCarico& cat, const CatalogoArmiIa& ia, const std::string& classe, int anno = 1989,
                   const std::string& tipo_missione = {});
// La variante che l'anno consente al posto di `variante` della nazione (la stessa se va bene, "" se nessuna)
std::string variante_dell_anno(const CatalogoCarico& cat, const std::string& variante, const std::string& nazione, int anno);

// --- scelta dell'arma ---
enum class BersaglioIa { sottomarino, nave };
struct SceltaArmaIa {
    std::string variante, siluro;             // la voce di carico.toml e il modello di siluri.toml ("" per i missili)
    TipoArmaCarico tipo = TipoArmaCarico::siluro;   // siluro (anche le esche mobili), missile, mina, esca: il percorso di lancio
    std::string famiglia;                     // mk48, type53, harpoon, esca_mobile...: l'arma dei gestori delle missioni
    int tubo = -1;                            // l'indice nel CaricoIa::tubi
    bool in_portata = true;
    bool filoguidata = false;
};
// Fra le armi nei tubi: in portata (portata x 0,9 sopra la distanza) prima, poi piu' stelle contro il bersaglio, poi la
// filoguidata se `filo_possibile`, poi la piu' lunga. Nullopt se nessuna arma nei tubi ha stelle contro il bersaglio
std::optional<SceltaArmaIa> scegli_arma_ia(const CaricoIa& carico, const CatalogoCarico& cat, BersaglioIa tipo,
                                           double distanza_m, bool filo_possibile);

// --- guida a filo ---
struct SituazioneFilo {
    double t = 0.0;
    const Siluro* siluro = nullptr;
    std::vector<EventoSiluro> eventi;         // quelli del passo (Siluro::eventi(), che chi li legge svuota)
    std::optional<Vec2> bersaglio_stimato;    // la posizione del contatto del lanciatore (TMA, eco, radar)
    std::optional<double> rilevamento;        // altrimenti la linea del rilevamento dal lanciatore
    Vec2 pos_lanciatore;
    bool lanciatore_evade = false;            // deve accelerare o virare: il filo si taglia
};
struct VincoliFilo {
    bool attivi = false;                      // mentre guida
    double velocita_max_kn = Siluro::FILO_VELOCITA_MAX_KN;
    double virata_max_deg_s = Siluro::FILO_VIRATA_MAX_GRADI_S;
};

class GuidaFiloIa {
public:
    explicit GuidaFiloIa(DottrinaFilo d) : d_(std::move(d)) {}
    std::vector<ComandoFilo> passo(const SituazioneFilo& s);
    VincoliFilo vincoli() const;              // da dare alla manovra del lanciatore
    bool tagliato() const { return tagliato_; }
    const DottrinaFilo& dottrina() const { return d_; }

private:
    std::optional<Vec2> mira(const SituazioneFilo& s) const;

    DottrinaFilo d_;
    bool tagliato_ = false, filo_ = true;
    double prossima_correzione_ = 0.0;
    std::optional<double> aggancio_t_;
    std::set<int> rifiutate_;
};

}  // namespace m688
