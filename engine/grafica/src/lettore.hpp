#pragma once
// Lettura di file binari little-endian (formati di data/motore).

#include <cstdint>
#include <cstring>
#include <fstream>
#include <iterator>
#include <stdexcept>
#include <string>
#include <vector>

namespace m688::interno {

inline std::vector<std::uint8_t> leggi_file(const std::string& percorso) {
    std::ifstream f(percorso, std::ios::binary);
    if (!f) throw std::runtime_error("impossibile aprire " + percorso);
    return {std::istreambuf_iterator<char>(f), std::istreambuf_iterator<char>()};
}

class Lettore {
public:
    Lettore(const std::vector<std::uint8_t>& d, std::string nome) : d_(d), nome_(std::move(nome)) {}

    template <class T>
    T leggi() {
        T v;
        richiedi(sizeof v);
        std::memcpy(&v, d_.data() + p_, sizeof v);   // le piattaforme supportate sono little-endian
        p_ += sizeof v;
        return v;
    }
    const std::uint8_t* blocco(std::size_t n) {
        richiedi(n);
        const auto* r = d_.data() + p_;
        p_ += n;
        return r;
    }
    void firma(const char (&attesa)[5]) {
        const auto* f = blocco(4);
        if (std::memcmp(f, attesa, 4) != 0) throw std::runtime_error(nome_ + ": firma diversa da " + attesa);
    }

private:
    void richiedi(std::size_t n) const {
        if (p_ + n > d_.size()) throw std::runtime_error(nome_ + ": file troncato");
    }
    const std::vector<std::uint8_t>& d_;
    std::string nome_;
    std::size_t p_ = 0;
};

}  // namespace m688::interno
