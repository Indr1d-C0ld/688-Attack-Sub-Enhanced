#pragma once
// Testi tradotti in formato TSV (codice, TAB, testo): messaggi di bordo (data/motore/testi/).

#include <map>
#include <string>

namespace m688 {

std::map<int, std::string> carica_testi(const std::string& percorso);   // lancia std::runtime_error

// Sostituisce {COMANDANTE} con la forma di cortesia del lato (USA: "Comandante"; URSS: "Compagno comandante").
std::string componi_messaggio(const std::string& testo, bool lato_sovietico);

}  // namespace m688
