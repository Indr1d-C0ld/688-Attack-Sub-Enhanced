#pragma once
// Le righe trovate in automatico nel modo moderno (blocco 3 del sonar, H; docs/23_sonar_blocco3.md §3): l'integratore
// (ALI) media le ultime righe del LOFAR e conferma un picco quando torna in M medie su N, l'inseguitore (ALT) lo segue nel tempo con la sua
// deriva. Come nell'A-RCI: un aiuto, non la verita'. Sbaglia come dal vero (dossier 09 §3.9):
// - la grana del rumore fa righe false (poche, perche' M su N le filtra);
// - una riga che svanisce (frange d'interferenza, segnale che cala) si perde dopo `perdita_s` e torna con un altro id;
// - due righe piu' vicine di `fusione_hz` diventano una;
// - una riga propria non mascherata si marca come le altre.
//
// Il modulo e' puro e deterministico: riceve gli Spettro che la stazione mostra (con la sua grana), uno per riga del
// LOFAR di un fascio o di un tracker.

#include <cstdint>
#include <vector>

#include "nucleo/analisi_acustica.hpp"

namespace m688 {

struct ParametriRilevatore {
    int integrazione = 4;            // l'integratore (ALI): i picchi si cercano sulla media delle ultime righe
    double soglia_db = 3.0;          // il picco sopra il fondo locale (righe_osservate), sulla media
    int m = 3, n = 5;                // confermata quando c'e' in M delle ultime N righe (ALI)
    double tolleranza_bin = 2.0;     // fra una riga e la successiva, oltre la deriva prevista
    double perdita_s = 60.0;         // senza il picco per tanto: persa (ALT)
    double fusione_hz = 0.0;         // due tracce piu' vicine si fondono (0: due bin)
};

struct RigaAutomatica {
    int id = -1;
    double hz = 0.0, snr_db = 0.0;
    double deriva_hz_min = 0.0;      // Doppler o giri che cambiano
    double nata_s = 0.0, ultima_s = 0.0;
    bool confermata = false;
    double eta_s(double t) const { return t - nata_s; }
};

class RilevatoreRighe {
public:
    explicit RilevatoreRighe(ParametriRilevatore p = {});
    // una riga nuova del LOFAR all'istante t; aggiorna le tracce
    void aggiungi(const Spettro& s, double t);
    const std::vector<RigaAutomatica>& righe() const { return righe_; }   // tutte le vive, confermate e no
    std::vector<RigaAutomatica> confermate() const;                         // le marcate, per la vista
    void azzera();

private:
    struct Stato {
        std::uint32_t colpi = 0;     // le ultime righe: 1 se c'era il picco (bit 0 la piu' recente)
        double ultimo_t = 0.0;
    };
    ParametriRilevatore p_;
    std::vector<std::vector<float>> storia_;   // le ultime righe per la media (un anello)
    std::size_t prossima_ = 0;
    double f0_ = 0.0, df_ = 0.0;
    std::vector<RigaAutomatica> righe_;
    std::vector<Stato> stati_;
    int prossimo_ = 0;
    double ultimo_t_ = -1.0;
};

}  // namespace m688
