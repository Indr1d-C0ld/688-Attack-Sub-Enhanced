#pragma once
// Il quadro del fondale: carta + misure (docs/24 §2.3; docs/08 §4; dossier docs/ricerca/06 §7.2). Un battello sa del
// fondo quello che dice la sua carta d'epoca e quello che ha misurato: niente di piu'.
// - La griglia e' quella della carta (200 m). Per ogni cella la carta da' la profondita' e quanto fidarsene (la
//   spaziatura del rilievo, le zone non rilevate); le misure (ecoscandaglio, BQS-15, sondatori) si fondono sopra con un
//   filtro di Kalman scalare.
// - Un sondaggio misura il centro dell'impronta; nel resto dell'impronta vale solo «il fondo e' almeno a questa
//   profondita'» (il vincolo di minimo). Lungo la rotta si forma un nastro di celle note.
// - Una misura vecchia si fida meno (la posizione del battello era incerta): torna piano verso la carta.
// - Il mosaico scorre: ogni misura e' archiviata con la posizione stimata del momento; a un punto nave la correzione si
//   distribuisce in modo lineare sulle misure prese dall'ultimo punto nave e le celle si rifanno (una SLAM semplificata).
// - Gli avvisi dell'equipaggio nascono solo dalle misure, mai dal fondale vero (docs/08 §7).
// Il modulo e' puro: le misure arrivano gia' fatte (nucleo/ecoscandaglio, nucleo/ritorni_fondo) con la posizione
// stimata. Le celle misurate stanno in una mappa sparsa (un teatro ha milioni di celle, un battello ne misura migliaia).

#include <cstdint>
#include <iosfwd>
#include <map>
#include <optional>
#include <vector>

#include "nucleo/ecoscandaglio.hpp"
#include "nucleo/fondale.hpp"
#include "nucleo/geometria.hpp"

namespace m688 {

enum class FonteFondo : std::uint8_t { carta, ecoscandaglio, bqs15, sondatore };

// Un ritorno del BQS-15 sul fondo o su una parete (nucleo/ritorni_fondo, docs/24 §2.2), gia' messo in posizione
// stimata. eco = false: il raggio non ha trovato niente fino a `distanza_m` (libera le celle, con meno fiducia)
struct RitornoFondo {
    double t = 0.0;
    Vec2 origine;                       // il battello, stimata
    double quota_m = 0.0;
    double rilevamento = 0.0, elevazione = 0.0, distanza_m = 0.0;
    bool eco = true;
    Vec2 punto;                         // dove sta l'eco (stimata)
    double profondita_m = 0.0;          // dell'eco
    double sigma_m = 5.0;
    bool ostacolo = false;              // una parete o un pinnacolo che sporge sopra la quota propria
};

struct ParametriQuadro {
    double sigma_rilevato_rel = 0.02;   // la carta dove c'e' il rilievo: 2% della profondita'...
    double sigma_spaziatura = 0.15;     // ...piu' il rilievo che cade fra due linee: x pendenza x spaziatura (STIMA)
    double sigma_non_rilevato_rel = 0.25;   // dove la carta non ha rilievo
    double invecchiamento_m_h = 20.0;   // sigma in piu' per ora d'eta' della misura (la posizione si perde)
    double sigma_libero_rel = 0.10;     // un raggio senza eco: «niente fino a X», con meno fiducia
};

// Quello che il battello sa di una cella
struct LetturaCella {
    double profondita_m = 0.0;          // la migliore stima (carta e misure fuse)
    double sigma_m = 0.0;
    double carta_m = 0.0, sigma_carta_m = 0.0;
    FonteFondo fonte = FonteFondo::carta;   // l'ultima misura, o carta
    double eta_s = -1.0;                // dall'ultima misura (< 0 mai misurata)
    double almeno_m = 0.0;              // il vincolo di minimo dell'impronta (0 senza)
    bool da_carta = true;               // nessuna misura: la vista la segna «da carta»
    bool ostacolo = false;
};

struct PuntoProfilo {
    double distanza_m = 0.0;            // lungo la rotta
    double carta_m = 0.0;
    double stima_m = 0.0, sigma_m = 0.0;
    bool misurato = false;
};

struct AvvisoFondo {
    enum class Tipo : std::uint8_t { sotto_chiglia, fondo_in_salita, ostacolo_a_prua, velocita_eccessiva };
    Tipo tipo = Tipo::sotto_chiglia;
    double valore = 0.0;                // m sotto la chiglia, m di salita, m all'ostacolo, kn sicuri
    double t = 0.0;                     // della misura che lo dice
};

class QuadroFondale {
public:
    // `carta`: la carta della parte (o il fondale vero con la navigazione «perfetta»: allora la carta e' esatta)
    explicit QuadroFondale(const Fondale& carta, ParametriQuadro p = {});

    // le misure, con la posizione stimata del momento e la sua sigma (radiale, m)
    void aggiungi(const Sondaggio& s, double sigma_posizione_m);
    void aggiungi(const RitornoFondo& r, double sigma_posizione_m);
    // un punto nave all'istante t: `correzione` = posizione corretta - stimata di prima. Si distribuisce sulle misure
    // dall'ultimo punto nave (0 all'ultimo punto nave, tutta a t) e il quadro si rifa'
    void punto_nave(double t, Vec2 correzione);

    // la griglia (la stessa della carta)
    int larghezza() const;
    int altezza() const;
    double cella_m() const;
    bool cella(Vec2 p, int& col, int& riga) const;
    Vec2 centro_cella(int col, int riga) const;
    LetturaCella leggi(int col, int riga, double t) const;
    LetturaCella leggi(Vec2 p, double t) const;
    // le celle con almeno una misura (per la vista: la si disegna senza scorrere tutta la griglia)
    std::vector<std::pair<int, int>> celle_misurate() const;

    // la sezione verticale di prua: carta e stima lungo la rotta, ogni `passo_m`
    std::vector<PuntoProfilo> profilo(Vec2 da, double rotta, double lunghezza_m, double passo_m, double t) const;

    // l'archivio, con le posizioni gia' corrette dai punti nave
    const std::vector<Sondaggio>& sondaggi() const { return sondaggi_; }
    const std::vector<RitornoFondo>& ritorni() const { return ritorni_; }
    std::vector<RitornoFondo> ritorni_recenti(double t, double finestra_s) const;

    // gli avvisi dalle sole misure: sotto la chiglia dall'ultimo sondaggio; il fondo in salita da due sondaggi recenti;
    // un ostacolo a prua da un ritorno del BQS-15 entro `reazione_s` alla velocita'; la velocita' sicura (la portata
    // vista dal BQS-15 per il tempo di reazione) se si va oltre
    std::vector<AvvisoFondo> avvisi(Vec2 pos_stimata, double quota_m, double rotta, double velocita_kn, double t,
                                    double reazione_s = 180.0) const;

    std::uint64_t versione() const { return versione_; }   // cresce a ogni cambiamento: la vista sa quando ridisegnare

    void salva(std::ostream& out) const;
    static QuadroFondale carica(std::istream& in, const Fondale& carta);   // std::runtime_error se il formato non torna

private:
    struct Misurata {
        double z = 0.0, var = 0.0;      // le misure fuse (senza la carta)
        double t = -1.0;
        FonteFondo fonte = FonteFondo::carta;
        double almeno = 0.0;
        bool ostacolo = false;
    };
    void rifai();                       // da capo, dall'archivio (dopo un punto nave o un caricamento)
    void applica(const Sondaggio& s, double sigma_posizione_m);
    void applica(const RitornoFondo& r, double sigma_posizione_m);
    void fondi(Vec2 p, double profondita, double var, double t, FonteFondo f, bool ostacolo);
    void vincolo(Vec2 p, double almeno);
    double sigma_carta(int col, int riga) const;

    const Fondale* carta_;
    ParametriQuadro p_;
    std::map<std::int64_t, Misurata> celle_;
    std::vector<Sondaggio> sondaggi_;
    std::vector<double> sigma_sondaggi_;
    std::vector<RitornoFondo> ritorni_;
    std::vector<double> sigma_ritorni_;
    double ultimo_punto_nave_ = 0.0;
    std::uint64_t versione_ = 0;
};

}  // namespace m688
