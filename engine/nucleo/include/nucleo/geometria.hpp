#pragma once
// Geometria del piano nautico e unita' di misura.
// Coordinate in metri: x verso est, y verso nord. Angoli nautici in gradi: 0 = nord, crescenti in senso orario.

namespace m688 {

inline constexpr double PI = 3.14159265358979323846;
inline constexpr double NODO = 1852.0 / 3600.0;      // m/s
inline constexpr double MIGLIO = 1852.0;             // m
inline constexpr double IARDA = 0.9144;              // m
inline constexpr double RAGGIO_TERRA = 6371000.0;    // m
// raggio efficace per la rifrazione atmosferica standard (k = 1/7): orizzonte e navi "sotto l'orizzonte"
inline constexpr double RAGGIO_TERRA_RIFRAZIONE = RAGGIO_TERRA * 7.0 / 6.0;

constexpr double radianti(double gradi) { return gradi * PI / 180.0; }
constexpr double gradi(double rad) { return rad * 180.0 / PI; }

double normalizza_360(double g);   // in [0, 360)
double normalizza_180(double g);   // in (-180, 180]

struct Vec2 {
    double x = 0.0, y = 0.0;
};

double distanza(Vec2 a, Vec2 b);
double rilevamento(Vec2 da, Vec2 a);   // rilevamento vero da `da` verso `a`, gradi

// Angolo sulla prua del bersaglio (AOB): 0 = prua verso l'osservatore, 180 = poppa.
// `dritta` vero se l'osservatore sta sulla dritta del bersaglio (rilevamento relativo positivo).
struct AngoloSullaPrua {
    double gradi = 0.0;
    bool dritta = true;
};
AngoloSullaPrua angolo_sulla_prua(Vec2 bersaglio, double rotta_bersaglio, Vec2 osservatore);

// Orizzonte geometrico con rifrazione: angolo di depressione (rad) e distanza (m) per un occhio a quota h.
double depressione_orizzonte(double quota_occhio);
double distanza_orizzonte(double quota_occhio);

}  // namespace m688
