#pragma once
// Moto delle navi col mare per la vista del periscopio dell'Enhanced (docs/15_meteo_e_mare.md, M3; dossier 21 §7.2 e
// §9; nota C §12.4, nota D §B6.3): rollio, beccheggio e sussulto con le ampiezze RMS per stato del mare e taglia
// (calibrate sul DD-963 in SS5: rollio 4,56° RMS), il rollio al periodo proprio (T ~ 0,75 B), beccheggio e sussulto al
// periodo d'incontro Te = Tp / (1 + V cos mu / c), c = 1,56 Tp; la prua che si pianta e lo spruzzo a prua da mare 5.
// Lo sprite gira attorno al galleggiamento dell'angolo apparente (beccheggio di fianco, rollio di prua).

#include "grafica/sprite.hpp"
#include "grafica/tela.hpp"

namespace m688 {

struct MotoNave {
    double rollio_gradi = 0.0;      // positivo: sbanda sulla dritta
    double beccheggio_gradi = 0.0;  // positivo: prua in su
    double sussulto_m = 0.0;        // positivo: la nave sale
    double spruzzo = 0.0;           // 0-1: la prua si pianta nel mare e solleva spruzzo
};

// `fase` distingue le navi (id); mare_da: direzione da cui viene il mare; tp_s <= 0: dal mare formato (5 sqrt(Hs))
MotoNave moto_nave(double lunghezza_m, double larghezza_m, double hs_m, double tp_s, double mare_da_gradi,
                   double rotta_gradi, double velocita_m_s, double tempo_s, double fase);
// Ampiezza RMS di rollio e beccheggio (gradi) con mare al traverso (rollio) e di prua (beccheggio), per lunghezza e Hs
void ampiezze_rms(double lunghezza_m, double hs_m, double& rollio, double& beccheggio);

// Angolo apparente dello sprite (radianti, positivo antiorario sullo schermo) per AOB e lato visto
double angolo_apparente(const MotoNave& m, double aob_gradi, bool dritta);

// Sprite ruotato di `angolo` (radianti) attorno all'ancora (centro al galleggiamento), con la foschia sui grigi come
// Tela::disegna_sprite; angolo nullo: identico a Tela::disegna_sprite
void disegna_sprite_ruotato(Tela& t, const Sprite& s, double x, double y, double scala, double angolo, int y_limite,
                            double foschia);

}  // namespace m688
