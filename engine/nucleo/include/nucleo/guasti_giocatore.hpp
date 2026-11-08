#pragma once
// Guasti dei sistemi del giocatore nel livello classico (docs/11_comandi_e_stazioni.md §7, fase C4;
// docs/originale/numeri_eseguibile.md §5, `1000:8B39-8C36`): con i messaggi 20 (bomba di profondita'), 26 (colpiti)
// e 30 (fondo strisciato), una volta su due si guasta uno di sei sistemi a caso; se e' gia' guasto non succede nulla.
// Nessuna riparazione. Il generatore e' il rand() di Borland C, non quello della VM delle missioni.

#include <array>
#include <cstdint>

namespace m688 {

// rand() di Borland C (1000:FE22): seme a 32 bit, seme = seme x 015A4E35h + 1, risultato (seme >> 16) & 7FFFh
class RandBorland {
public:
    explicit RandBorland(std::uint32_t seme = 1) : seme_(seme) {}
    int operator()() {
        seme_ = seme_ * 0x015A4E35u + 1u;
        return static_cast<int>((seme_ >> 16) & 0x7FFFu);
    }

private:
    std::uint32_t seme_;
};

enum class SistemaClassico : std::uint8_t { calcolatore_sonar, schiera, periscopio, sonar_di_tiro, motori, antenna };   // DS:CC86+2i
inline constexpr int SISTEMI = 6;

// Messaggi: annuncio del guasto 60 + indice (voci 3Ch-41h dell'EXE); motori: MAX e arresto rapido rifiutati con 66
inline constexpr int MSG_GUASTO = 60, MSG_MOTORI_NON_REGGONO = 66;
inline constexpr int MSG_BOMBA = 20, MSG_COLPITI = 26, MSG_FONDO = 30;

class GuastiGiocatore {
public:
    explicit GuastiGiocatore(std::uint32_t seme = 1) : rand_(seme) {}
    // Un messaggio della missione al giocatore: con 20, 26 e 30 il tiro (rand() % 2, poi rand() % 6). Restituisce il
    // sistema appena guastato, o -1.
    int messaggio(int codice);
    bool guasto(SistemaClassico s) const { return guasti_[static_cast<std::size_t>(s)]; }
    static int annuncio(SistemaClassico s) { return MSG_GUASTO + static_cast<int>(s); }

private:
    RandBorland rand_;
    std::array<bool, SISTEMI> guasti_{};
};

}  // namespace m688
