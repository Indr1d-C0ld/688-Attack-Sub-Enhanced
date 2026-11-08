#pragma once
// Interferenza dei cammini sul LOFAR (blocco 1 del sonar, docs/20_sonar_blocco1.md §4): le frange dello specchio di
// Lloyd e dei rimbalzi sul fondo, che sulla cascata a banda stretta si piegano a «vasca da bagno» quando un contatto
// passa al CPA. Con le frange si stima la distanza (prassi del sonarista, dossier 09).
//
// Metodo delle immagini in una guida d'onda isoveloce di fondale D (Jensen, Kuperman, Porter e Schmidt, Computational
// Ocean Acoustics, §2.4.2): per m = 0, 1, ... le quattro immagini
//   z = 2mD + zs - zr (m in superficie, m sul fondo), 2(m+1)D - zs - zr (m, m+1),
//   2mD + zs + zr (m+1, m), 2(m+1)D - zs + zr (m+1, m+1);
// ogni immagine porta 1/L, (-1) a ogni riflessione in superficie, la perdita sul fondo del livello A
// (perdita_fondo_db, per angolo e frequenza) a ogni rimbalzo. La parte coerente di ogni cammino cala con la coerenza
// di Eckart della superficie (dossier 02 §3.4 h), con la rugosita' del fondo e, per i cammini col fondo, con la
// distanza (STIMA); il resto arriva in potenza.
//
// Il risultato e' un rapporto in dB fra il campo coerente e lo stesso campo in potenza (salvo il dipolo di Lloyd del
// livello A sulla coppia diretto e superficie): i termini incrociati, che in media sulle frequenze valgono zero, sono
// le frange. Sommato alla perdita del livello A le aggiunge senza cambiare il livello medio. Sotto pi/4 (il dipolo,
// lontano) il livello A e' gia' coerente e il rapporto vale circa 1. Le frange fitte si spianano col moto nel tempo
// d'integrazione (media su tre istanti).
//
// Il modulo e' puro. Non tocca la scoperta (valuta resta incoerente): serve agli spettri della stazione
// (nucleo/analisi_acustica, argomento `coerente`) e alla cuffia.

#include <complex>
#include <cstddef>
#include <vector>

#include "nucleo/acustica.hpp"

namespace m688 {

struct ImmagineCammino {
    double lunghezza_m = 0.0;
    double radenza_gradi = 0.0;      // angolo del cammino sull'orizzontale
    int superficie = 0, fondo = 0;   // riflessioni
};
// Le immagini delle prime `ordini` famiglie (m < ordini, quattro per famiglia); con il fondale da 1e8 m in su (ignoto)
// solo il diretto e la superficie. Quote fra 0,5 m e il fondale, come il livello A
std::vector<ImmagineCammino> immagini_cammini(double r_m, double zs_m, double zr_m, double fondale_m, int ordini);

struct ParametriInterferenza {
    int ordini = 4;                      // famiglie d'immagini (16 cammini)
    double coerenza_fondo_m = 30000.0;   // i cammini col fondo perdono coerenza con la distanza: exp(-r / questa), STIMA
    double min_db = -30.0, max_db = 10.0;
};

class Interferenza {
public:
    // dr_m: di quanto cambia la distanza nel tempo d'integrazione (velocita' radiale per 1 / bin); il rapporto e' la
    // media a r - dr/2, r e r + dr/2
    Interferenza(double r_m, double zs_m, double zr_m, const Ambiente& amb, double dr_m = 0.0, ParametriInterferenza par = {});
    // dB da sommare al livello alla frequenza f (gia' limitati fra min_db e max_db)
    double db(double f_hz) const;
    // Gli stessi su f0 + (i + 0.5) df per i < n: le fasi esatte bin per bin, le ampiezze calcolate ogni ~1 Hz e
    // interpolate (differenze dal calcolo di db() sotto il decimo di dB)
    std::vector<float> db_griglia(double f0_hz, double df_hz, std::size_t n) const;
    const std::vector<ImmagineCammino>& immagini() const { return istanti_[istanti_.size() / 2].immagini; }

private:
    struct Istante {
        double r = 0.0;
        std::vector<ImmagineCammino> immagini;
        std::vector<char> coppia;   // il diretto e la superficie (il dipolo del livello A)
    };
    // le ampiezze di un istante a una frequenza: coerente con segno (per cammino), energia incoerente della coppia e
    // del resto, il minimo della coppia e l'energia di riferimento del livello A
    struct Ampiezze {
        std::vector<double> coerente;
        double incoerente_coppia = 0.0, incoerente_resto = 0.0, minimo_coppia = 0.0, riferimento = 0.0;
    };
    Ampiezze ampiezze(const Istante& t, double f_hz) const;
    static double rapporto(std::complex<double> p, std::complex<double> q, double incoerente_coppia, double incoerente_resto,
                           double minimo_coppia, double riferimento);
    double limita(double r) const;

    std::vector<Istante> istanti_;
    double zs_ = 0.0, zr_ = 0.0;
    Ambiente amb_;
    ParametriInterferenza par_;
    double c0_ = 1500.0, sigma_onda_ = 0.0;
};

}  // namespace m688
