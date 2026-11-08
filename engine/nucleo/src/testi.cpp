#include "nucleo/testi.hpp"

#include <fstream>
#include <stdexcept>

namespace m688 {

std::map<int, std::string> carica_testi(const std::string& percorso) {
    std::ifstream f(percorso);
    if (!f) throw std::runtime_error("impossibile aprire " + percorso);
    std::map<int, std::string> out;
    std::string riga;
    while (std::getline(f, riga)) {
        const auto tab = riga.find('\t');
        if (tab == std::string::npos || tab == 0) continue;
        out[std::stoi(riga.substr(0, tab))] = riga.substr(tab + 1);
    }
    return out;
}

std::string componi_messaggio(const std::string& testo, bool lato_sovietico) {
    std::string s = testo;
    const std::string segnaposto = "{COMANDANTE}";
    for (auto p = s.find(segnaposto); p != std::string::npos; p = s.find(segnaposto, p))
        s.replace(p, segnaposto.size(), lato_sovietico ? "Compagno comandante" : "Comandante");
    return s;
}

}  // namespace m688
