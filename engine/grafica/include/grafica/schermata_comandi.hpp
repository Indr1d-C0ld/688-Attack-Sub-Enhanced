#pragma once
// La schermata dei comandi in partita (richiesta dell'utente, kit 06/11/2026): la tela intera, nella lamiera delle pagine
// dell'Enhanced. In alto la targa e una linguetta per scheda (una per stazione, piu' «ovunque»); sotto il foglio di bordo coi
// consigli a penna rossa e i comandi per gruppo, i tasti come cappucci e il testo a capo; in basso il piede comune. Le pagine si
// contano qui. I dati li prepara la partita: qui non si guarda il battello, cosi' il disegno non tocca la partita.
// La palette: quella della schermata con applica_tema(p, TemaStazione{}), come le altre pagine di lamiera.

#include <string>
#include <vector>

#include "grafica/tela.hpp"

namespace m688 {

class Font;

struct RigaComando {
    std::string tasti, cosa;      // «Ctrl+P», «quota periscopica». Piu' tasti separati da ", ": un cappuccio ciascuno
};

struct GruppoComandi {
    std::string titolo;           // vuoto: niente titolo
    std::vector<RigaComando> righe;
};

struct SchedaComandi {
    std::string etichetta;        // «F5 PERISCOPIO», «OVUNQUE»: il tasto va sopra il nome
    std::vector<std::string> consigli;   // in evidenza, sopra, solo nella prima pagina
    std::vector<GruppoComandi> gruppi;
};

struct DatiSchermataComandi {
    bool alfa = false;            // le scritte del pult: la targa e i titoli su fondo rosso
    std::vector<SchedaComandi> schede;
    int scheda = 0, pagina = 0;   // quella in primo piano; fuori misura si riportano dentro
};

// Dove si clicca: una linguetta (scheda), la pagina dopo (pagina, in giro; scheda -1) o chiudi
struct AreaComando {
    Rettangolo r;
    int scheda = -1, pagina = -1;
    bool chiudi = false;
};

// f: originale.fnt; piccolo: fosforo688_5x7.fnt (i nomi delle linguette); targa: incisione688_7.fnt. Il testo passa da
// testo_per_font. Restituisce il numero di pagine della scheda in primo piano; `aree` puo' essere nullptr
int disegna_schermata_comandi(Tela& t, const Font& f, const Font& piccolo, const Font& targa, const DatiSchermataComandi& d,
                              std::vector<AreaComando>* aree = nullptr);

}  // namespace m688
