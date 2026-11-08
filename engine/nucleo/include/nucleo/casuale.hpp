#pragma once
// Generatore pseudo-casuale deterministico (xoshiro256**, semi da splitmix64): stessa sequenza su ogni
// piattaforma e compilatore, a differenza delle distribuzioni di <random>. Base per missioni con seme e replay.

#include <cstdint>

namespace m688 {

class Casuale {
public:
    explicit Casuale(std::uint64_t seme = 0x688u) {
        std::uint64_t z = seme;
        for (auto& v : s_) {
            z += 0x9e3779b97f4a7c15ull;
            std::uint64_t t = z;
            t = (t ^ (t >> 30)) * 0xbf58476d1ce4e5b9ull;
            t = (t ^ (t >> 27)) * 0x94d049bb133111ebull;
            v = t ^ (t >> 31);
        }
    }

    std::uint64_t prossimo() {
        const std::uint64_t r = ruota(s_[1] * 5, 7) * 9;
        const std::uint64_t t = s_[1] << 17;
        s_[2] ^= s_[0];
        s_[3] ^= s_[1];
        s_[1] ^= s_[2];
        s_[0] ^= s_[3];
        s_[2] ^= t;
        s_[3] = ruota(s_[3], 45);
        return r;
    }

    double uniforme() { return static_cast<double>(prossimo() >> 11) * 0x1.0p-53; }   // [0, 1)
    double uniforme(double a, double b) { return a + (b - a) * uniforme(); }

private:
    static std::uint64_t ruota(std::uint64_t x, int k) { return (x << k) | (x >> (64 - k)); }
    std::uint64_t s_[4]{};
};

}  // namespace m688
