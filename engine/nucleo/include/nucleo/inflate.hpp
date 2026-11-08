#pragma once
// Decompressore DEFLATE (RFC 1951, blocchi senza intestazione zlib) per i formati compressi di data/motore
// (modelli MDL2, sprite SPR2, fondali FDL2): codici di Huffman canonici decodificati bit per bit, come puff.c di Mark
// Adler.

#include <array>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace m688::interno {

class Inflate {
public:
    Inflate(const std::uint8_t* d, std::size_t n) : d_(d), n_(n) {}

    std::vector<std::uint8_t> esegui(std::size_t atteso) {
        out_.reserve(atteso);
        bool ultimo = false;
        while (!ultimo) {
            ultimo = bit(1) != 0;
            const auto tipo = bit(2);
            if (tipo == 0) {
                memorizzato();
            } else if (tipo == 1) {
                Huffman lun, dist;
                std::array<std::uint8_t, 288> l{};
                for (int i = 0; i < 288; ++i) l[static_cast<std::size_t>(i)] = i < 144 ? 8 : i < 256 ? 9 : i < 280 ? 7 : 8;
                costruisci(lun, l.data(), 288);
                l.fill(5);
                costruisci(dist, l.data(), 30);
                codici(lun, dist);
            } else if (tipo == 2) {
                dinamico();
            } else {
                throw std::runtime_error("deflate: blocco non valido");
            }
            if (out_.size() > atteso) break;
        }
        if (out_.size() != atteso) throw std::runtime_error("deflate: lunghezza inattesa");
        return std::move(out_);
    }

private:
    struct Huffman {
        std::array<std::uint16_t, 16> conta{};      // codici per lunghezza
        std::array<std::uint16_t, 288> simbolo{};   // simboli in ordine di codice
    };

    std::uint32_t bit(int k) {
        while (nb_ < k) {
            if (pos_ >= n_) throw std::runtime_error("deflate: dati troncati");
            buf_ |= static_cast<std::uint32_t>(d_[pos_++]) << nb_;
            nb_ += 8;
        }
        const std::uint32_t v = buf_ & ((1u << k) - 1u);
        buf_ >>= k;
        nb_ -= k;
        return v;
    }

    static void costruisci(Huffman& h, const std::uint8_t* lun, int n) {
        h.conta.fill(0);
        for (int s = 0; s < n; ++s) ++h.conta[lun[s]];
        h.conta[0] = 0;
        std::array<std::uint16_t, 16> inizio{};
        for (std::size_t l = 1; l < 15; ++l) inizio[l + 1] = static_cast<std::uint16_t>(inizio[l] + h.conta[l]);
        for (int s = 0; s < n; ++s)
            if (lun[s] != 0) h.simbolo[inizio[lun[s]]++] = static_cast<std::uint16_t>(s);
    }

    int decodifica(const Huffman& h) {
        int codice = 0, primo = 0, indice = 0;
        for (std::size_t l = 1; l < 16; ++l) {
            codice |= static_cast<int>(bit(1));
            const int conta = h.conta[l];
            if (codice - conta < primo) return h.simbolo[static_cast<std::size_t>(indice + (codice - primo))];
            indice += conta;
            primo = (primo + conta) << 1;
            codice <<= 1;
        }
        throw std::runtime_error("deflate: codice non valido");
    }

    void memorizzato() {
        buf_ = 0;
        nb_ = 0;   // al byte successivo
        if (pos_ + 4 > n_) throw std::runtime_error("deflate: dati troncati");
        const auto len = static_cast<std::size_t>(d_[pos_] | (d_[pos_ + 1] << 8));
        pos_ += 4;
        if (pos_ + len > n_) throw std::runtime_error("deflate: dati troncati");
        out_.insert(out_.end(), d_ + pos_, d_ + pos_ + len);
        pos_ += len;
    }

    void dinamico() {
        static constexpr std::uint8_t ORDINE[19] = {16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15};
        const int nlun = static_cast<int>(bit(5)) + 257, ndist = static_cast<int>(bit(5)) + 1, ncod = static_cast<int>(bit(4)) + 4;
        std::array<std::uint8_t, 320> l{};
        for (int i = 0; i < ncod; ++i) l[ORDINE[i]] = static_cast<std::uint8_t>(bit(3));
        Huffman h, lun, dist;
        costruisci(h, l.data(), 19);
        l.fill(0);
        for (int i = 0; i < nlun + ndist;) {
            const int s = decodifica(h);
            if (s < 16) { l[static_cast<std::size_t>(i++)] = static_cast<std::uint8_t>(s); continue; }
            std::uint8_t v = 0;
            int r = 0;
            if (s == 16) {
                if (i == 0) throw std::runtime_error("deflate: ripetizione senza lunghezza");
                v = l[static_cast<std::size_t>(i - 1)];
                r = 3 + static_cast<int>(bit(2));
            } else {
                r = s == 17 ? 3 + static_cast<int>(bit(3)) : 11 + static_cast<int>(bit(7));
            }
            if (i + r > nlun + ndist) throw std::runtime_error("deflate: troppe lunghezze");
            while (r--) l[static_cast<std::size_t>(i++)] = v;
        }
        costruisci(lun, l.data(), nlun);
        costruisci(dist, l.data() + nlun, ndist);
        codici(lun, dist);
    }

    void codici(const Huffman& lun, const Huffman& dist) {
        static constexpr std::uint16_t BASE_L[29] = {3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17, 19, 23, 27, 31, 35, 43, 51, 59,
                                                     67, 83, 99, 115, 131, 163, 195, 227, 258};
        static constexpr std::uint8_t EXTRA_L[29] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 0};
        static constexpr std::uint16_t BASE_D[30] = {1, 2, 3, 4, 5, 7, 9, 13, 17, 25, 33, 49, 65, 97, 129, 193, 257, 385, 513, 769,
                                                     1025, 1537, 2049, 3073, 4097, 6145, 8193, 12289, 16385, 24577};
        static constexpr std::uint8_t EXTRA_D[30] = {0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13};
        for (;;) {
            int s = decodifica(lun);
            if (s < 256) { out_.push_back(static_cast<std::uint8_t>(s)); continue; }
            if (s == 256) return;
            s -= 257;
            if (s >= 29) throw std::runtime_error("deflate: lunghezza non valida");
            const std::size_t n = BASE_L[s] + bit(EXTRA_L[s]);
            const int sd = decodifica(dist);
            if (sd >= 30) throw std::runtime_error("deflate: distanza non valida");
            const std::size_t d = BASE_D[sd] + bit(EXTRA_D[sd]);
            if (d > out_.size()) throw std::runtime_error("deflate: distanza oltre l'inizio");
            for (std::size_t i = 0; i < n; ++i) out_.push_back(out_[out_.size() - d]);
        }
    }

    const std::uint8_t* d_;
    std::size_t n_, pos_ = 0;
    std::uint32_t buf_ = 0;
    int nb_ = 0;
    std::vector<std::uint8_t> out_;
};

// Decomprime n byte DEFLATE in esattamente `atteso` byte (std::runtime_error se i dati non tornano)
inline std::vector<std::uint8_t> inflate(const std::uint8_t* d, std::size_t n, std::size_t atteso) {
    return Inflate(d, n).esegui(atteso);
}

}  // namespace m688::interno
