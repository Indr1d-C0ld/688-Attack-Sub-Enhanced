#pragma once
// Orologio a passo fisso: la simulazione avanza sempre di `passo` secondi, indipendentemente dal ritmo dei
// fotogrammi, ed e' quindi deterministica. Con la compressione del tempo si eseguono piu' passi per fotogramma,
// entro un limite che evita la "spirale della morte" quando la macchina non sta al passo.

#include <algorithm>
#include <cmath>
#include <cstdint>

namespace m688 {

class Orologio {
public:
    explicit Orologio(double passo = 1.0 / 30.0, int passi_massimi = 600)
        : passo_us_(std::llround(passo * 1e6)), passi_massimi_(passi_massimi) {}

    // Accumula il tempo reale trascorso (s) moltiplicato per la compressione e restituisce quanti passi eseguire.
    // L'accumulo e' in microsecondi interi: in virgola mobile 0,25 - 0,2 + 0,05 non fa 0,1 e un passo si perde.
    int avanza(double dt_reale, double compressione = 1.0) {
        accumulato_us_ += std::llround(std::clamp(dt_reale, 0.0, 0.25) * compressione * 1e6);
        std::int64_t n = accumulato_us_ / passo_us_;
        if (n > passi_massimi_) {           // la macchina non tiene il passo: si perde tempo simulato
            n = passi_massimi_;
            accumulato_us_ = 0;
        } else {
            accumulato_us_ -= n * passo_us_;
        }
        passi_ += static_cast<std::uint64_t>(n);
        return static_cast<int>(n);
    }

    double passo() const { return static_cast<double>(passo_us_) * 1e-6; }
    double tempo() const { return static_cast<double>(passi_) * passo(); }   // tempo simulato, s
    std::uint64_t passi() const { return passi_; }
    double alfa() const { return static_cast<double>(accumulato_us_) / static_cast<double>(passo_us_); }

private:
    std::int64_t passo_us_;
    int passi_massimi_;
    std::int64_t accumulato_us_ = 0;
    std::uint64_t passi_ = 0;
};

}  // namespace m688
