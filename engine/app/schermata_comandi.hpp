#pragma once
// La schermata dei comandi in partita (F11): i dati da data/motore/comandi_schermata.toml, generato da tools/pipeline/manuale_tasti.py dalla
// stessa tabella dell'appendice A del manuale, cosi' non restano indietro. Qui si tengono le righe del modo di gioco e del battello e si scrivono i
// tasti dalla mappa vera, quindi quelli rimappati. Il disegno e' in grafica/schermata_comandi, del kit.

#include <string>
#include <vector>

#include "grafica/schermata_comandi.hpp"
#include "nucleo/mappa_tasti.hpp"

namespace m688 {

class TestoComandi {
public:
    // Lancia std::runtime_error se il file manca o non si legge
    explicit TestoComandi(const std::string& cartella_dati);
    // Le schede per questa partita, nell'ordine del file
    DatiSchermataComandi dati(bool enhanced, bool alfa, const MappaTasti& tasti) const;
    // La scheda di una stazione (2-8); le altre stazioni aprono OVUNQUE
    int scheda_di(int stazione) const;
    int schede() const { return static_cast<int>(schede_.size()); }

private:
    struct Riga { std::vector<std::string> tasti; std::string solo, cosa; };
    struct Gruppo { std::string titolo, solo; std::vector<Riga> righe; };
    struct Consiglio { std::string solo, testo; };
    struct Scheda { std::string etichetta; int stazione = 0; std::vector<Consiglio> consigli; std::vector<Gruppo> gruppi; };
    std::vector<Scheda> schede_;
};

// «solo» di una riga: "", "enhanced", "classico", "alfa", "688"
bool riga_per(const std::string& solo, bool enhanced, bool alfa);
// Le voci dei tasti scritte per il giocatore: comando:X coi tasti della mappa, pulsanti:A-B come «1-5», fisso:testo com'e'
std::string scrivi_tasti_voci(const std::vector<std::string>& voci, const MappaTasti& tasti);

}  // namespace m688
