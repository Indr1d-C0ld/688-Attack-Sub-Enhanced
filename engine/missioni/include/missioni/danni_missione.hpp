#pragma once
// Danni, relitti e reazioni nelle missioni (docs/10_danni_affondamento_esplosioni.md §7, fase D5).
//
// La missione decide i colpi con la regola dell'originale (Missione::colpi(), docs/originale/sensori_e_navigazione.md
// §9.8); qui ogni nave di superficie colpita riceve la sua fisica dei danni (nucleo/danni.hpp) con la testata
// dell'arma, e nel mondo da disegnare la sua immagine segue la fisica:
// - **regola classica** (livello classico, predefinita): gli script restano fedeli. Quando la missione da' una nave
//   per distrutta, la nave diventa un relitto staccato dagli script, fermo dove e' stato colpito; se la fisica non la
//   condanna gia', riceve un "colpo di grazia" (falle sul fondo) cosi' affonda davvero. Se invece la fisica la
//   affonda mentre per gli script e' ancora viva, l'unita' degli script continua invisibile e immagine e suono sono
//   quelli del relitto.
// - **regola di simulazione** (Enhanced): le navi con la fisica non perdono integrita' per i colpi; la missione le
//   da' per distrutte (Missione::distruggi) quando la fisica le condanna o le affonda, oppure quando l'integrita'
//   dell'originale e' esaurita e la nave e' menomata (§2.7: senza energia, spezzata, in fiamme o allagata): una
//   nave fuori combattimento, anche se galleggia ancora, cosi' gli script non la bersagliano all'infinito. Reagiscono:
//   propulsione persa (ferme), velocita' ridotta con sbandamento o acqua imbarcata, ritirata lontano da chi ha
//   attaccato, soccorso delle navi amiche vicine quando l'equipaggio abbandona la nave.
// Il giocatore riceve i messaggi del motore (codici 200+) per cio' che sonar e radio sentirebbero entro 20 nm.

#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "missioni/missione.hpp"
#include "nucleo/casuale.hpp"
#include "nucleo/classi.hpp"
#include "nucleo/danni.hpp"
#include "nucleo/mondo.hpp"

namespace m688 {

enum class RegolaDanni { classica, simulazione };

struct NaveColpita {
    int unita = -1;                 // id nella missione
    int id_mondo = -1;              // id della nave disegnata nel mondo (DanniMissione::ID_IMMAGINE + unita)
    std::string classe;
    std::unique_ptr<DanniNave> danni;
    Vec2 pos;                       // ultima posizione (il relitto resta li')
    double rotta = 0.0, velocita = 0.0;
    bool relitto = false;           // staccata dagli script
    bool colpo_di_grazia = false;
    int lanciatore = -1;            // ultimo che l'ha colpita (ritirata)
    // eventi gia' annunciati al giocatore
    std::size_t scoppi = 0;
    bool ferma = false, spezzata = false, sbanda = false, abbandono = false, affondata = false, soccorso = false;
    bool deposito_brucia = false, deposito_allagato = false;
    double fine_ritirata = -1.0;
    double velocita_urto = 1.0;     // frazione della massima dopo le collisioni (elica piegata o persa, valuta_urto)
    bool timone_bloccato = false;   // dopo le collisioni: la nave non accosta piu'
};

class DanniMissione {
public:
    static constexpr int ID_IMMAGINE = 100000;

    // `classi` e `cartella_dati` per gli scafi (ClasseNave::scafo); il seme rende ripetibili testate e punti d'impatto.
    DanniMissione(const std::map<std::string, ClasseNave>& classi, std::string cartella_dati, std::uint64_t seme,
                  RegolaDanni regola = RegolaDanni::classica);

    // Legge i colpi nuovi della missione, avanza le fisiche, decide relitti e reazioni (dopo Missione::avanza).
    void avanza(Missione& m, double dt);
    // Nel mondo da disegnare (dopo Missione::sincronizza): le navi colpite diventano la loro immagine con la posa della
    // fisica; le unita' degli script che sono gia' relitti spariscono.
    void sincronizza(Mondo& mondo) const;

    const std::vector<std::unique_ptr<NaveColpita>>& navi() const { return navi_; }
    const NaveColpita* nave(int unita) const;
    const std::vector<Messaggio>& messaggi() const { return messaggi_; }
    RegolaDanni regola() const { return regola_; }
    // Colpi della missione su unita' senza fisica (sottomarini, armi, esche): gli effetti li mostrano come scoppi.
    const std::vector<Colpo>& altri_colpi() const { return altri_; }

    // fuori combattimento (*mission kill*, §2.7): senza energia, spezzata, incendi estesi, molta acqua o sbandata
    static bool menomata(const DanniNave& d);

private:
    bool ha_fisica(const UnitaClassica& u) const;
    NaveColpita& prendi(const UnitaClassica& u);
    void colpisci(NaveColpita& n, const Colpo& c);
    void reazioni(Missione& m, NaveColpita& n);
    void annuncia(const Missione& m, const NaveColpita& n, int codice);

    const std::map<std::string, ClasseNave>& classi_;
    std::string dati_;
    Casuale caso_;
    RegolaDanni regola_;
    std::vector<std::unique_ptr<NaveColpita>> navi_;
    std::map<std::string, DatiScafo> scafi_;
    std::vector<Messaggio> messaggi_;
    std::vector<Colpo> altri_;
    std::size_t colpi_visti_ = 0;
    std::size_t urti_visti_ = 0;   // Missione::urti gia' applicati (la nave dell'urto)
    std::size_t impatti_visti_ = 0;   // Missione::impatti_artiglieria gia' applicati (le navi colpite, G3)
    double tempo_ = 0.0;
};

}  // namespace m688
