#pragma once
// Contatti del giocatore per l'interfaccia (docs/11_comandi_e_stazioni.md §2, fase C4): carta, periscopio e barra
// del bersaglio mostrano i contatti dell'unita' del giocatore, non la verita'. Regola di `990C`
// (docs/originale/sensori_e_navigazione.md §10.5): indicatori, filtro passa alto, colori della carta.

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "missioni/missione.hpp"

namespace m688 {

// Indicatori di 990C
inline constexpr int SEGNO_TRACCIA = 0x01;       // posizione nota (lettera, distanza, missili): nel classico la
                                                 // traccia solida (qualita' >= 800); nell'Enhanced eco, vista o radar
inline constexpr int SEGNO_CLASSIFICATO = 0x02;  // durata > 800: colore "classificato" (§10.5 lo scrive anche con la
                                                 // traccia solida; qui conta solo la durata, che decide i colori)
inline constexpr int SEGNO_CLASSE = 0x04;        // durata >= 1400: classe nota
inline constexpr int SEGNO_NAVE_AEREO = 0x08;    // flag & 6
inline constexpr int SEGNO_ARMA = 0x10;          // flag 20h
inline constexpr int SEGNO_OCCIDENTALE = 0x20;   // flag 10h assente
inline constexpr int SEGNO_FILOGUIDATO = 0x40;   // proprio Mk 48 o Type 53 collegato al giocatore

struct SegnoContatto {
    int id = -1;
    char lettera = 0;
    int indicatori = 0;
    double rilevamento = 0.0;   // gradi veri dal giocatore (+04); nell'Enhanced misurato, con l'errore del sensore
    std::optional<double> specchio;   // Enhanced, schiera: l'altro rilevamento dell'ambiguita' (una seconda X)
    std::string sensore;        // Enhanced: chi l'ha scoperto (bqq5_sfera, tb16, vista, esm, ...)
    double eccesso_db = 0.0;    // Enhanced: eccesso di segnale dell'ultima valutazione
    Vec2 pos_stimata;           // +08/+0A: valida con la traccia solida
    double quota_stimata = 0.0;
    bool traccia() const { return (indicatori & SEGNO_TRACCIA) != 0; }
};

// I contatti del giocatore nell'ordine della sua lista, senza gli orfani (unita' uscite dal gioco), senza le tracce
// deboli spente dal lampeggio e, con il filtro passa alto, solo le armi
std::vector<SegnoContatto> segni_contatti(const Missione& m, bool filtro);

// Colore della carta (palette EGA): X 14 senza traccia; con traccia 13 se la durata e' <= 800, poi 1 sottomarino o
// ignoto, 2 nave o aereo, 4 arma, + 8 con la classe nota; 14 il proprio siluro filoguidato
std::uint8_t colore_segno(const SegnoContatto& s);

// Barra del bersaglio (8150-8320, numeri_eseguibile.md §3.4): il titolo dalla durata della traccia; sempre il
// rilevamento relativo (in senso orario dalla prua) e la rotta vera; con la traccia solida ed entro 20 nm per asse
// distanza, velocita' e quota vere (il rumore della velocita' e della quota lo aggiunge l'interfaccia).
struct BarraBersaglio {
    enum class Titolo : std::uint8_t { nessuno, sconosciuto, sconosciuto_superficie, sconosciuto_immersione, classe };
    Titolo titolo = Titolo::nessuno;
    std::string classe;            // con Titolo::classe: la classe della missione convertita
    int tipo_originale = -1;       // e il tipo dell'originale, per il nome (nome_tipo_originale)
    char lettera = 0;
    double rilevamento_relativo = 0.0, rotta = 0.0;   // gradi, sempre
    bool dati = false;             // traccia solida ed entro 20 nm per asse: i tre campi sotto
    int distanza_decimi = 0;       // decimi di nm
    double velocita_kn = 0.0, quota_ft = 0.0;
    bool superficie = false;       // quota vera 0: niente rumore sulla quota
    std::string sensore;           // Enhanced: il sensore dell'ultima scoperta e l'eccesso di segnale
    double eccesso_db = 0.0;
};
BarraBersaglio barra_bersaglio(const Missione& m, int bersaglio);

// Nome italiano del tipo d'unita' dell'originale (tabella DS:019A, 0x00-0x2D), vuoto se ignoto
std::string_view nome_tipo_originale(int tipo);

}  // namespace m688
