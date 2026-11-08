#pragma once
// Stato del mondo e cinematica delle unita' (prima versione: moto piano con accostata e accelerazione
// limitate; la dinamica completa del sottomarino arriva nella Fase 2).

#include <cstdint>
#include <string>
#include <vector>

#include "nucleo/geometria.hpp"

namespace m688 {

// Assetto di un corpo rigido della nave (la nave intera o un troncone) rispetto al galleggiamento intatto, nelle
// coordinate del modello (x verso prua, y verso sinistra, z verso l'alto, origine al centro nave sul galleggiamento).
struct AssettoScafo {
    double sbandamento = 0.0;    // gradi, positivo = sbanda sulla dritta (fianco destro verso il basso)
    double assetto = 0.0;        // gradi, positivo = prua alta
    double immersione = 0.0;     // m, positivo = piu' in basso
    double dx = 0.0, dy = 0.0;   // m, spostamento orizzontale (tronconi che si separano)
    double x_perno = 0.0;        // m: punto lungo la nave attorno a cui ruota (sul galleggiamento)
};

// Stato visibile di una nave danneggiata (docs/10_danni_affondamento_esplosioni.md): con `danneggiata` il disegno
// usa il modello 3D invece degli sprite. Spezzata: i triangoli a prua di `x_rottura` seguono `prodiero`, gli
// altri `poppiero`; altrimenti tutta la nave segue `prodiero`.
// Segno sullo scafo (coordinate del modello intatto): falla nera dal bordo frastagliato, o fuliggine attorno a un
// locale che ha bruciato. Raggi dell'ellissoide in metri; intensita' 0-1.
struct SegnoScafo {
    enum class Tipo { falla, fuliggine };
    Tipo tipo = Tipo::falla;
    double x = 0.0, y = 0.0, z = 0.0;
    double rx = 1.0, ry = 1.0, rz = 1.0;
    double intensita = 1.0;
};

// Parte del modello ruotata attorno a un perno (albero che cade): asse orizzontale (ax, ay) nel modello, angolo in
// radianti con la regola della mano destra.
struct ParteMossa {
    int parte = 0;
    double px = 0.0, py = 0.0, pz = 0.0;
    double ax = 1.0, ay = 0.0;
    double angolo = 0.0;
};

struct PosaNave {
    bool danneggiata = false;
    bool spezzata = false;
    double x_rottura = 0.0;
    AssettoScafo prodiero, poppiero;
    std::vector<SegnoScafo> segni;
    std::vector<ParteMossa> parti;
    // inarcamento della trave sotto una bolla (solo disegno): spostamento verticale di flessione_m metri al punto
    // flessione_x, campana gaussiana larga flessione_larghezza (positivo = in su)
    double flessione_x = 0.0, flessione_m = 0.0, flessione_larghezza = 1.0;
};

struct Nave {
    int id = 0;
    std::string classe;               // chiave in classi.toml
    Vec2 pos;                         // m
    double rotta = 0.0;               // gradi veri
    double velocita = 0.0;            // m/s
    double rotta_ordinata = 0.0;      // gradi veri
    double velocita_ordinata = 0.0;   // m/s
    double accelerazione = 0.05;      // m/s^2 (stima di gioco per navi di superficie)
    double rateo_accostata = 0.6;     // gradi/s alla velocita' di manovra (stima di gioco)
    Vec2 pos_prec;                    // stato all'inizio dell'ultimo passo (interpolazione del disegno)
    double rotta_prec = 0.0;
    PosaNave posa;
    // sottomarini e armi (E2, docs/10 §10.15): quota sotto il mare (m, 0 in superficie) e periscopio o antenna
    // alzati; il periscopio disegna dei battelli immersi solo gli alberi che escono dall'acqua. Negativa: in volo a
    // -quota metri sul mare (aerei ed elicotteri dell'Enhanced, docs/13 A4); gli aerei del classico restano a 0
    double quota = 0.0;
    bool alberi_alzati = false;
};

class Mondo {
public:
    Nave& aggiungi(Nave n);
    void passo(double dt);                       // avanza di dt secondi (passo fisso dell'orologio)
    const std::vector<Nave>& navi() const { return navi_; }
    std::vector<Nave>& navi() { return navi_; }
    const Nave* trova(int id) const;
    // Copia del mondo con posizioni e rotte interpolate tra l'ultimo passo e il precedente (alfa in [0, 1]):
    // il disegno resta fluido anche se la simulazione avanza a 30 passi al secondo o piu' lentamente.
    Mondo interpolato(double alfa) const;
    double tempo() const { return tempo_; }
    std::uint64_t impronta() const;              // hash dello stato: verifica del determinismo

private:
    std::vector<Nave> navi_;
    double tempo_ = 0.0;
};

}  // namespace m688
