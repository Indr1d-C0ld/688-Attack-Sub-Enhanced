#pragma once
// Rotte per acque navigabili sul fondale di un teatro: il "pilota" delle unita' dell'IA (engine/README.md,
// "Fondale e incagli").
//
// - AcqueNavigabili: le celle con almeno `minima` metri d'acqua (sotto, la chiglia toccherebbe), divise in
//   componenti collegate (8 vicini). Dice subito se una meta e' raggiungibile e qual e' il punto raggiungibile
//   piu' vicino.
// - rotta_navigabile: A* sulle celle in un riquadro attorno a partenza e arrivo; l'acqua tra `minima` e `comoda`
//   costa 4 volte (la rotta la evita se puo'); poi la rotta e' tagliata a vista in pochi punti. La cella di
//   partenza e' sempre ammessa, cosi' un'unita' gia' in acqua bassa puo' uscirne.

#include <cstdint>
#include <vector>

#include "nucleo/fondale.hpp"
#include "nucleo/geometria.hpp"

namespace m688 {

class AcqueNavigabili {
public:
    AcqueNavigabili(const Fondale& f, double minima);
    const Fondale& fondale() const { return f_; }
    double minima() const { return minima_; }
    // componente della cella (da 0), -1 se l'acqua e' meno di `minima`
    int componente(int col, int riga) const { return comp_[static_cast<std::size_t>(riga) * f_.larghezza() + col]; }
    int celle(int componente) const { return dimensioni_[componente]; }
    // la cella piu' vicina a (col, riga) nella componente data (-1: in una qualsiasi con almeno `celle_minime`
    // celle, per evitare pozze chiuse); falso se non ce n'e'
    bool piu_vicina(int& col, int& riga, int componente, int celle_minime = 0) const;

private:
    const Fondale& f_;
    double minima_;
    std::vector<std::int32_t> comp_;
    std::vector<int> dimensioni_;
};

// Acqua sotto la chiglia: `minima` per passare, `comoda` per passare senza penalita'
struct AcquaRichiesta {
    double minima = 0.0;
    double comoda = 0.0;
};

// Vero se lungo il segmento (campioni ogni `passo_m`) l'acqua non scende sotto `soglia`.
bool tratto_libero(const Fondale& f, Vec2 da, Vec2 a, double soglia, double passo_m = 50.0);

struct Rotta {
    std::vector<Vec2> punti;   // da `da` (escluso) all'arrivo (incluso)
    bool completa = false;     // falso: `a` non e' raggiungibile e la rotta finisce nel punto navigabile piu' vicino
};

// Vuota se la partenza e' fuori dalla griglia o chiusa in un'acqua senza uscita.
Rotta rotta_navigabile(const AcqueNavigabili& acque, Vec2 da, Vec2 a, double acqua_comoda);

}  // namespace m688
