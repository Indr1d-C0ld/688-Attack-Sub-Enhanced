#pragma once
// La storia lunga delle cascate (blocco 3 del sonar, I; docs/23_sonar_blocco3.md §4): le righe di BTR, LOFAR e DEMON
// per ore, per sfogliarle all'indietro e cercare quando e' comparsa una riga o una traccia. Un contenitore puro, uno per
// cascata (un fascio, un tracker, il BTR).
// - Piu' risoluzioni, ognuna un anello: di serie 1 riga al secondo per 10 minuti, ogni 10 s per 2 ore, ogni 60 s per
//   12 ore. Ogni livello prende il massimo delle righe che cadono nel suo passo (come la decimazione della stazione):
//   una riga breve non sparisce quando si scende di risoluzione.
// - I valori si quantizzano in 8 bit a `passo_db` (0,25 dB: da `minimo_db` a minimo + 63,75 dB). Con 4000 bin le tre
//   risoluzioni stanno in circa 8 MB.
// - Il BTR ci sta uguale: i bin sono i rilevamenti (f0 = 0, df = un grado).
// Il registratore della stazione (S5d) resta per l'ascolto.

#include <cstddef>
#include <cstdint>
#include <deque>
#include <iosfwd>
#include <optional>
#include <vector>

#include "nucleo/analisi_acustica.hpp"

namespace m688 {

struct LivelloStoria {
    double passo_s = 1.0;     // una riga ogni tanto
    double durata_s = 600.0;  // quanto tiene: poi la piu' vecchia esce
};

struct ParametriStoria {
    std::vector<LivelloStoria> livelli = {{1.0, 600.0}, {10.0, 7200.0}, {60.0, 43200.0}};   // dal piu' fine
    double passo_db = 0.25, minimo_db = 0.0;
};

// Una finestra letta dalla storia: `righe()` righe di `bin` valori, dalla piu' vecchia; t e' l'inizio del passo
struct FinestraStoria {
    double passo_s = 0.0;
    std::size_t bin = 0;
    std::vector<double> t;
    std::vector<float> db;    // righe una dopo l'altra
    std::size_t righe() const { return t.size(); }
    float valore(std::size_t riga, std::size_t i) const { return db[riga * bin + i]; }
};

class StoriaCascata {
public:
    StoriaCascata(double f0, double df, std::size_t bin, ParametriStoria p = {});
    // una riga della cascata all'istante t (crescente; una riga piu' vecchia dell'ultima si scarta)
    void aggiungi(double t, const std::vector<float>& riga);
    void aggiungi(const Spettro& s, double t) { aggiungi(t, s.snr_db); }

    // le righe fra da_s e a_s al livello dato (-1: il piu' fine che arriva indietro fino a da_s); il passo in corso compreso
    FinestraStoria finestra(double da_s, double a_s, int livello = -1) const;
    // quando e' comparsa la riga a `hz` (o la traccia al rilevamento, per il BTR) che c'e' adesso: l'inizio del tratto,
    // tornando indietro dalla riga piu' recente fino a un buco piu' lungo di buco_s. Un picco e' il massimo entro la
    // tolleranza (0: due bin) sopra la mediana locale di soglia_db. Niente se adesso non c'e'. Una riga che deriva molto
    // (Doppler, giri) esce dalla tolleranza: si trova l'inizio del tratto alla frequenza di adesso
    std::optional<double> comparsa(double hz, double soglia_db = 3.0, double buco_s = 120.0, double tolleranza_hz = 0.0) const;

    double inizio_s() const;   // la riga piu' vecchia tenuta (nel livello piu' grosso)
    double fine_s() const;     // la piu' recente
    std::size_t memoria_byte() const;
    double f0() const { return f0_; }
    double df() const { return df_; }
    std::size_t bin() const { return bin_; }
    const ParametriStoria& parametri() const { return p_; }

    // col salvataggio della partita: binario, piccolo, versionato
    void salva(std::ostream& out) const;
    static StoriaCascata carica(std::istream& in);   // std::runtime_error se il formato non torna

private:
    struct Riga {
        double t = 0.0;                // l'inizio del passo
        std::vector<std::uint8_t> v;
    };
    struct Livello {
        std::deque<Riga> anello;
        Riga corrente;                 // il passo in corso, col massimo
        bool aperto = false;
    };
    std::uint8_t quantizza(float db) const;
    float valore(std::uint8_t q) const;
    void chiudi(Livello& l, std::size_t i);

    double f0_, df_;
    std::size_t bin_;
    ParametriStoria p_;
    std::vector<Livello> livelli_;
    double ultimo_t_ = -1e300;
};

}  // namespace m688
