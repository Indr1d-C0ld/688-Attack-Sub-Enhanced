#pragma once
// Mine dell'Enhanced (Fase 4a, M2b; dossier docs/ricerca/14_carico_armi.md §2.6-2.7 e §2.12): il campo minato posato
// dai tubi. Tre comportamenti:
// - influenza (Mk 67 SLMM): corre dal tubo lungo la rotta impostata per la distanza impostata (al piu' la sua corsa),
//   si posa sul fondo e scoppia quando una nave o un sottomarino passa entro il raggio;
// - captor (Mk 60 CAPTOR): ormeggiata sopra il fondo dove e' stata posata, rileva un sottomarino in immersione e
//   libera un siluro leggero (Mk 46) contro di lui; ignora le navi di superficie;
// - mina_siluro: sul fondo, rileva navi e sottomarini e lancia un siluro (nessuna mina del catalogo, oggi);
// - mina_razzo (PMR-1, PMR-2): ormeggiata a una quota fissa sopra fondali profondi, rileva un sottomarino in
//   immersione sopra di lei e lancia un razzo subacqueo che lo raggiunge in pochi secondi e scoppia vicino a lui.
// Fuori dalle sue profondita' la mina resta inerte; dopo la posa si arma in `armamento_s`; non distingue amici e
// nemici; scoppia o lancia una volta sola.
//
// Il modulo e' puro e deterministico: la missione gli passa i bersagli e il fondale a ogni passo e applica lei gli
// scoppi (regola dei colpi del motore) e i siluri (modello dell'Enhanced).

#include <cstdint>
#include <functional>
#include <map>
#include <string>
#include <vector>

#include "nucleo/geometria.hpp"

namespace m688 {

enum class TipoMina : std::uint8_t { influenza, captor, mina_siluro, mina_razzo };

struct DatiMina {
    std::string id;
    TipoMina tipo = TipoMina::influenza;
    double carica_kg = 0.0;
    double raggio_m = 0.0;                    // influenza: distanza dello scoppio; captor e mina_siluro: rilevamento
    double profondita_min_m = 0.0, profondita_max_m = 1e9;   // del fondo dove funziona
    double corsa_max_m = 0.0, velocita_corsa_kn = 0.0;       // mine mobili: la corsa dal tubo
    double ormeggio_m = 0.0;                  // altezza sopra il fondo (captor)
    double quota_mina_m = 0.0;                // mina_razzo: quota fissa dalla superficie (0 = sul fondo o ormeggio)
    double errore_razzo_m = 0.0;              // mina_razzo: distanza dal bersaglio dello scoppio del razzo
    double armamento_s = 600.0;
    std::string siluro;                       // voce di siluri.toml del siluro lanciato ("" per l'influenza)
    bool navi = true, sottomarini = true;
    double quota_min_bersaglio_m = 0.0;       // captor e mina_razzo: solo sottomarini in immersione
    bool solo_sopra = false;                  // mina_razzo: solo bersagli sopra la mina (settore verso l'alto)
    std::string fonte, nota;
};
// la tabella [mine] di data/motore/carico.toml; std::runtime_error
std::map<std::string, DatiMina> carica_mine(const std::string& percorso);

struct Mina {
    enum class Stato : std::uint8_t { in_corsa, armandosi, armata, scoppiata, inerte };
    int id = 0;
    std::string tipo;                         // id della mina (mk67, mk60, pmr1)
    Stato stato = Stato::in_corsa;
    Vec2 pos;
    double quota = 0.0, fondale = 0.0;        // m
    double posata_t = 0.0, armata_t = 0.0;    // s di missione: lancio dal tubo, armamento
    int posatore = -1, lato = 0;              // chi l'ha posata e il suo schieramento (flag & 0x18)
    double rotta = 0.0, corsa_m = 0.0;        // mine mobili: rotta e distanza che restano da correre
    bool gia_armata = false;                  // un campo gia' posato all'inizio della missione: armata sul fondo
    bool sicura = false;                      // «in sicurezza» (la I17, in pace): in acqua e visibile, ma non si arma mai
    // comandata da terra (la S18): in sicurezza finche' non passa alla stazione (comanda), poi scoppia su ordine
    // dell'operatore quando un sottomarino e' sopra di lei entro il raggio, dopo il suo ritardo
    bool comandata = false, alla_stazione = false;
    std::string gruppo;
    double ordine_t = -1.0;                   // quando l'operatore ha deciso (lo scoppio dopo il ritardo); -1 nessuno
};

struct BersaglioMina {
    int id = -1;
    Vec2 pos;
    double quota = 0.0;
    bool nave = false, sottomarino = false;
};

struct InnescoMina {
    int mina = -1, bersaglio = -1, posatore = -1, lato = 0;
    std::string tipo;                         // id della mina
    bool siluro = false;                      // true: lancia `classe_siluro`; false: scoppia (il razzo: vicino al bersaglio)
    std::string classe_siluro;
    Vec2 pos;                                 // della mina (del razzo che scoppia, per la mina_razzo)
    double quota = 0.0;
    double distanza_m = 0.0, dz_m = 0.0;      // dal bersaglio; dz: quota della mina meno quella del bersaglio
    double carica_kg = 0.0;
    double rotta = 0.0;                       // verso il bersaglio
};

class CampoMinato {
public:
    // Le mine che il campo sa trattare (carica_mine)
    void imposta_dati(std::map<std::string, DatiMina> d) { dati_ = std::move(d); }
    const DatiMina* dati(const std::string& tipo) const;
    bool vuoto() const { return mine_.empty(); }

    // Posa dal tubo: le mobili partono lungo `rotta` per `distanza_m` (al piu' la corsa); le altre si posano sotto il
    // posatore. -1 se il tipo e' ignoto.
    int posa(const std::string& tipo, Vec2 da, double rotta, double distanza_m, double tempo, int posatore, int lato,
             bool gia_armata = false, bool sicura = false);

    struct Esito {
        std::vector<int> posate, inerti, armate;   // mine arrivate sul fondo, rimaste inerti, appena armate
        std::vector<InnescoMina> inneschi;
    };
    // Un passo: `tempo` s di missione, `dt_m` il passo in secondi gia' scalato al moto della missione (come i siluri);
    // `fondale(p)` la profondita' del fondo in metri (<= 0 terra)
    Esito avanza(double tempo, double dt_m, const std::vector<BersaglioMina>& bersagli, const std::function<double(Vec2)>& fondale);

    const std::vector<Mina>& mine() const { return mine_; }
    // la mina comandata da terra (la S18): imposta_comando la fa comandata del gruppo; comanda passa alla stazione le
    // comandate del gruppo (vuoto: tutte) e dice quante
    void imposta_comando(int id, const std::string& gruppo);
    int comanda(const std::string& gruppo);

private:
    void posa_sul_fondo(Mina& m, const DatiMina& d, double tempo, double fondo, Esito& e);

    std::map<std::string, DatiMina> dati_;
    std::vector<Mina> mine_;
    int prossimo_id_ = 1;
};

}  // namespace m688
