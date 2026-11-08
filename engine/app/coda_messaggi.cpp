#include "coda_messaggi.hpp"

#include <algorithm>

namespace m688 {

double CodaMessaggi::durata(const std::string& testo) {
    return std::clamp(2.0 + 0.05 * static_cast<double>(testo.size()), 2.5, 7.0);
}

bool CodaMessaggi::critico(const std::string& testo) { return testo.find('!') != std::string::npos; }

void CodaMessaggi::aggiorna(const std::vector<std::pair<double, std::string>>& storico, double ora) {
    if (storico.size() < visti_) visti_ = 0;   // un'altra partita
    for (; visti_ < storico.size(); ++visti_) {
        const std::string& x = storico[visti_].second;
        if (x.empty() || (!coda_.empty() && coda_.back() == x) || x == corrente_) continue;   // ripetuti di fila
        if (critico(x)) {   // davanti, dopo gli altri critici gia' in coda
            auto it = std::find_if(coda_.begin(), coda_.end(), [](const std::string& c) { return !critico(c); });
            coda_.insert(it, x);
        } else {
            coda_.push_back(x);
        }
    }
    while (coda_.size() > MASSIMO) {   // i normali piu' vecchi restano solo nel registro
        auto it = std::find_if(coda_.begin(), coda_.end(), [](const std::string& c) { return !critico(c); });
        if (it == coda_.end()) break;
        coda_.erase(it);
    }
    if (coda_.empty()) return;   // l'ultimo resta in barra, come nell'originale
    const bool scaduto = ora >= fino_ || corrente_.empty();
    const bool sorpasso = critico(coda_.front()) && !corrente_critico_ && ora - inizio_ >= 1.0;
    if (scaduto || sorpasso) {
        corrente_ = coda_.front();
        coda_.pop_front();
        corrente_critico_ = critico(corrente_);
        inizio_ = ora;
        fino_ = ora + durata(corrente_);
    }
}

}  // namespace m688
