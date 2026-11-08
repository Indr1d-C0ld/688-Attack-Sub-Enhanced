#pragma once
// Analisi del moto del bersaglio dai soli rilevamenti (TMA) per l'Enhanced (docs/11_comandi_e_stazioni.md §3.4 e
// §10, fase C6c; docs/ricerca/02_acustica_oceanografia_dottrina.md §5.1).
//
// Strumenti per il giocatore, non un risolutore automatico:
// - MATE (Manual Adaptive Target Estimator): il giocatore propone distanza, rotta e velocita' del bersaglio; lo
//   strumento mostra gli scarti fra i rilevamenti previsti e quelli misurati. Con lo scarto piccolo su due gambe la
//   soluzione e' buona; su una gamba sola e' inosservabile (molte soluzioni danno scarti nulli);
// - rateo di rilevamento ai minimi quadrati su un intervallo, regola del 1934 e distanza di Ekelund su due gambe.
//
// Angoli in gradi bussola (0 = nord, orario), posizioni in metri, velocita' in m/s, tempi in s.

#include <optional>
#include <vector>

#include "nucleo/geometria.hpp"

namespace m688 {

struct Rilevamento {
    double tempo = 0.0;
    Vec2 proprio;               // posizione del battello quando e' stato preso
    double rilevamento = 0.0;   // gradi veri
};

// Un'osservazione della rotta del bersaglio: l'AOB stimato al periscopio (libretto di riconoscimento, docs/25 §3.4),
// rotta = rilevamento + 180 - AOB (AOB + dritta), con la sua incertezza (+-15-30 gradi)
struct OsservazioneRotta {
    double tempo = 0.0;
    double rotta = 0.0;         // gradi veri
    double sigma_gradi = 20.0;
};

// Soluzione proposta, riferita all'ultimo rilevamento: il bersaglio e' lungo quel rilevamento a `distanza_m`
struct SoluzioneTma {
    double distanza_m = 5000.0;
    double rotta = 0.0;          // gradi
    double velocita_m_s = 5.0;
};

class Tma {
public:
    static constexpr std::size_t MASSIMO = 720;   // rilevamenti tenuti (un'ora a uno ogni 5 s)
    static constexpr std::size_t MASSIMO_ROTTE = 20;   // osservazioni di rotta tenute, le piu' recenti

    void aggiungi(const Rilevamento& r);
    void aggiungi_rotta(const OsservazioneRotta& o);
    void azzera() { ril_.clear(); rotte_.clear(); }
    const std::vector<Rilevamento>& rilevamenti() const { return ril_; }
    const std::vector<OsservazioneRotta>& rotte() const { return rotte_; }

    // Posizione del bersaglio all'istante t secondo la soluzione (moto rettilineo uniforme)
    std::optional<Vec2> posizione(const SoluzioneTma& s, double t) const;
    // Scarti previsto - misurato per ogni rilevamento, gradi in (-180, 180]
    std::vector<double> scarti(const SoluzioneTma& s) const;
    double scarto_quadratico_medio(const SoluzioneTma& s) const;   // gradi; 0 senza rilevamenti
    // Scarti rotta della soluzione - rotta osservata per ogni osservazione di rotta, gradi in (-180, 180] (scarti e
    // scarto_quadratico_medio restano dei soli rilevamenti: il MATE li mostra cosi'; risolvi_tma conta anche questi)
    std::vector<double> scarti_rotta(const SoluzioneTma& s) const;

    // Rateo di rilevamento (gradi/min, positivo in senso orario) ai minimi quadrati sui rilevamenti in [t0, t1]
    std::optional<double> rateo(double t0, double t1) const;

    // Componente della velocita' propria attraverso la linea di vista, nel verso dei rilevamenti crescenti (m/s)
    static double attraverso(Vec2 velocita_propria, double rilevamento);
    // Regola del 1934 in unita' SI: rateo (gradi/min) = V_perp / R
    static double rateo_1934(double v_perp_m_s, double distanza_m);
    // Distanza di Ekelund: R = (OSA2 - OSA1) / (B1 - B2), con i ratei di due gambe e la velocita' propria attraverso la
    // linea di vista in ciascuna. Vuota se i ratei coincidono (nessuna manovra utile).
    static std::optional<double> distanza_ekelund(double osa1_m_s, double osa2_m_s, double rateo1_gradi_min,
                                                  double rateo2_gradi_min);

private:
    std::vector<Rilevamento> ril_;
    std::vector<OsservazioneRotta> rotte_;
};

}  // namespace m688
