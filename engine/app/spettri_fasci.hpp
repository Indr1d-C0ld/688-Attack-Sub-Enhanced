#pragma once
// Gli spettri di piu' fasci di una stessa scena, su piu' fili: nella riga del sonar erano il 99% del tempo, e fermavano il
// thread dei fotogrammi 165-200 ms ogni 2 s di missione (docs/29). spettro_fascio e' una funzione pura della scena e chi
// chiama aspetta qui, quindi nessuno scrive mentre i fili leggono: i valori sono gli stessi, bit per bit, con un filo o con piu'.

#include <algorithm>
#include <atomic>
#include <cstddef>
#include <future>
#include <system_error>
#include <thread>
#include <vector>

#include "nucleo/analisi_acustica.hpp"

namespace m688 {

struct FascioChiesto {
    double rilevamento = 0.0;
    bool coerente = false;
};

// Non piu' dei core, e non piu' dei fasci della riga: il fascio del LOFAR e i 4 vicini del colore
inline int fili_per_gli_spettri() { return std::clamp(static_cast<int>(std::thread::hardware_concurrency()), 1, 5); }

// Gli spettri nell'ordine di `fasci`, su `fili` fili al massimo, compreso chi chiama; con un filo solo, uno dopo l'altro
inline std::vector<Spettro> spettri_dei_fasci(const std::vector<Sorgente>& sorgenti, const Ricevitore& r, const Ambiente& amb, const std::vector<FascioChiesto>& fasci,
                                              double f_min, double f_max, double bin, int fili) {
    std::vector<Spettro> out(fasci.size());
    std::atomic<std::size_t> prossimo{0};
    const auto lavora = [&] {
        for (std::size_t i = prossimo.fetch_add(1); i < fasci.size(); i = prossimo.fetch_add(1))
            out[i] = spettro_fascio(sorgenti, r, amb, fasci[i].rilevamento, f_min, f_max, bin, fasci[i].coerente);
    };
    std::vector<std::future<void>> altri;   // prima di lavora: un'eccezione qui li aspetta nel distruttore
    try {
        for (int k = 1; k < std::min(fili, static_cast<int>(fasci.size())); ++k) altri.push_back(std::async(std::launch::async, lavora));
    } catch (const std::system_error&) {   // niente fili in piu': fa tutto chi c'e'
    }
    lavora();
    for (auto& f : altri) f.get();
    return out;
}

}  // namespace m688
