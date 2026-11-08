#include "grafica/spaccato.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <map>
#include <string_view>

#include "grafica/font.hpp"
#include "nucleo/geometria.hpp"

namespace m688 {

namespace {

// --- palette: rampe dal buio alla luce (indici 96-143 e 199-247) ---
constexpr int MARE = 96, SCAFO = 104, TAGLIO = 112, VANO = 116, METALLO = 124, BRONZO = 132, ACCENTO = 136;
constexpr int ACQUA = 199, SOMMERSO = 207, FUMO = 215, BAGLIORE = 223, CASSA = 231, ALLARME = 235, BOLLE = 239,
              BRUCIATO = 243;
enum Accento { ROSSO_SCURO, ROSSO, GIALLO, VERDE, AMBRA, VERDE_SCURO, BLU_SCHERMO, BIANCO_CALDO };

struct Rampa {
    int primo;
    std::vector<std::array<std::uint8_t, 3>> rgb;
};
const std::vector<Rampa>& rampe() {
    static const std::vector<Rampa> r = {
        {MARE, {{2, 5, 20}, {3, 9, 30}, {5, 14, 41}, {7, 20, 53}, {9, 27, 66}, {12, 36, 81}, {18, 49, 99}, {28, 66, 120}}},
        {SCAFO, {{10, 12, 16}, {19, 22, 28}, {30, 34, 42}, {43, 48, 58}, {58, 64, 76}, {76, 83, 97}, {98, 106, 122}, {128, 137, 154}}},
        {TAGLIO, {{86, 108, 118}, {146, 172, 182}, {198, 224, 230}, {244, 255, 255}}},
        {VANO, {{3, 26, 32}, {5, 38, 46}, {7, 52, 62}, {10, 68, 80}, {16, 88, 100}, {26, 110, 122}, {42, 136, 146}, {72, 170, 174}}},
        {METALLO, {{26, 30, 34}, {44, 50, 56}, {64, 72, 80}, {88, 96, 104}, {116, 124, 132}, {148, 156, 162}, {184, 190, 194}, {224, 228, 230}}},
        {BRONZO, {{72, 18, 10}, {124, 34, 16}, {182, 58, 24}, {232, 112, 52}}},
        {ACCENTO, {{110, 12, 10}, {222, 34, 26}, {242, 212, 64}, {72, 226, 98}, {252, 152, 34}, {22, 92, 44}, {70, 150, 252}, {255, 244, 214}}},
        {ACQUA, {{2, 14, 40}, {4, 23, 60}, {6, 35, 84}, {10, 50, 110}, {16, 70, 136}, {30, 96, 160}, {72, 140, 192}, {186, 224, 242}}},
        {SOMMERSO, {{4, 17, 36}, {6, 25, 50}, {9, 35, 64}, {13, 46, 80}, {19, 60, 96}, {29, 78, 114}, {43, 98, 132}, {64, 122, 152}}},
        {FUMO, {{13, 12, 11}, {24, 22, 20}, {37, 34, 31}, {52, 48, 44}, {70, 66, 60}, {92, 88, 82}, {116, 112, 106}, {146, 142, 136}}},
        {BAGLIORE, {{42, 15, 5}, {72, 27, 7}, {106, 41, 9}, {142, 59, 13}, {178, 81, 19}, {212, 109, 31}, {238, 144, 54}, {255, 188, 98}}},
        {CASSA, {{6, 16, 18}, {11, 26, 27}, {18, 38, 37}, {27, 52, 49}}},
        {ALLARME, {{96, 0, 0}, {172, 12, 10}, {242, 44, 32}, {255, 124, 94}}},
        {BOLLE, {{78, 128, 168}, {128, 178, 208}, {188, 224, 240}, {240, 250, 255}}},
        {BRUCIATO, {{9, 7, 6}, {20, 15, 11}, {34, 25, 17}, {50, 35, 23}, {68, 46, 29}}},
    };
    return r;
}

constexpr int BAYER[16] = {0, 8, 2, 10, 12, 4, 14, 6, 3, 11, 1, 9, 15, 7, 13, 5};
inline double bayer(int x, int y) { return (BAYER[(y & 3) * 4 + (x & 3)] + 0.5) / 16.0; }
// tono continuo -> intero con retinatura
inline int retina(double t, int x, int y, int massimo) {
    return std::clamp(static_cast<int>(std::floor(t + bayer(x, y))), 0, massimo);
}

std::uint32_t mescola(std::uint32_t h) {
    h ^= h >> 16;
    h *= 0x7feb352dU;
    h ^= h >> 15;
    h *= 0x846ca68bU;
    h ^= h >> 16;
    return h;
}
double caso(int a, int b, int c = 0) {
    return mescola(static_cast<std::uint32_t>(a) * 73856093U ^ static_cast<std::uint32_t>(b) * 19349663U ^
                   static_cast<std::uint32_t>(c) * 83492791U) / 4294967296.0;
}
// rumore a valori con interpolazione morbida, 0..1
double rumore(double x, double y, int seme = 0) {
    const int ix = static_cast<int>(std::floor(x)), iy = static_cast<int>(std::floor(y));
    const double fx = x - ix, fy = y - iy;
    const double sx = fx * fx * (3 - 2 * fx), sy = fy * fy * (3 - 2 * fy);
    const double a = caso(ix, iy, seme), b = caso(ix + 1, iy, seme), c = caso(ix, iy + 1, seme), d = caso(ix + 1, iy + 1, seme);
    return (a * (1 - sx) + b * sx) * (1 - sy) + (c * (1 - sx) + d * sx) * sy;
}

// --- materiali del disegno di base ---
enum class Mat : std::uint8_t { mare, scafo, taglio, vano, metallo, bronzo, accento, cassa, allagabile, parete };

struct Px {
    Mat mat = Mat::mare;
    std::uint8_t tono = 0;
    std::int8_t cella = -1;   // cella dello scafo (compartimento o cassa) per i pixel tagliati
    std::int8_t sist = -1;    // sistema del kit a cui appartiene (lampeggia se guasto)
    bool interno = false;     // dentro il taglio: acqua, fumo e fuoco lo toccano
};

// --- allestimento per classe: ponti e macchinari (coordinate del kit, m) ---
enum class Tipo {
    consolle, cuccette, tavoli, cucina, siluri, tubi, sfera, schiera_prua, periscopio, albero, reattore, vapore, turbina,
    riduttore, condensatore, generatore, asse, bombole, pompe, diesel, batterie, quadri, capsula, scala, rullo, tunnel
};
struct Elemento {
    Tipo tipo;
    double x0, x1, z0, z1;
    int sist = -1;
};
struct Allestimento {
    std::map<std::string, std::vector<double>> ponti;   // quote dei pavimenti per cella (dal basso)
    std::map<std::string, std::string> nomi;           // didascalia per cella
    std::vector<Elemento> elementi;
};

constexpr int S_CALC = static_cast<int>(Sistema::calcolatore_sonar), S_SCHIERA = static_cast<int>(Sistema::schiera),
              S_PERI = static_cast<int>(Sistema::periscopio), S_TIRO = static_cast<int>(Sistema::sonar_tiro),
              S_MOTORI = static_cast<int>(Sistema::motori), S_ANTENNA = static_cast<int>(Sistema::antenna),
              S_IDRO = static_cast<int>(Sistema::idroplani), S_TIMONE = static_cast<int>(Sistema::timone),
              S_PROP = static_cast<int>(Sistema::propulsione), S_ARIA = static_cast<int>(Sistema::aria),
              S_POMPE = static_cast<int>(Sistema::pompe), S_PRUA = static_cast<int>(Sistema::sonar_prua),
              S_INER = static_cast<int>(Sistema::inerziale);

Allestimento allestimento_688() {
    Allestimento a;
    a.ponti = {{"C1", {-6.9, -3.6, -0.6}}, {"C2", {-7.4, -0.4}}, {"C3", {-6.9, -2.9}}};
    a.nomi = {{"C1", "PRUA: COMANDO, ALLOGGI, SILURI"}, {"C2", "REATTORE"}, {"C3", "MACCHINE"},
              {"Z_PRUA", "ZAVORRA"}, {"Z_POPPA", "ZAVORRA"}};
    using T = Tipo;
    a.elementi = {
        // prua: sfera del sonar BQQ-5 nella cupola allagabile
        {T::sfera, 46.9, 52.1, -6.0, -0.8, S_PRUA},
        // ponte superiore: sonar, centrale sotto la vela, radio, alloggi degli ufficiali
        {T::consolle, 33.5, 40.0, -0.6, 1.0, S_CALC},
        {T::consolle, 26.0, 31.0, -0.6, 1.0, S_TIRO},
        {T::periscopio, 21.4, 22.2, -0.6, 13.5, S_PERI},
        {T::periscopio, 23.3, 24.2, -0.6, 13.3, S_PERI},
        {T::albero, 25.4, 25.8, 1.85, 7.0, S_ANTENNA},
        {T::albero, 20.0, 20.3, 1.85, 6.8, S_ANTENNA},
        {T::albero, 26.4, 26.8, 1.85, 6.6, -1},
        {T::consolle, 18.6, 20.8, -0.6, 1.0, S_IDRO},
        {T::consolle, 13.5, 17.8, -0.6, 1.0, S_ANTENNA},
        {T::cuccette, 3.0, 12.5, -0.6, 1.3, -1},
        {T::scala, 32.6, 33.2, -3.6, 1.85, -1},
        {T::scala, 1.4, 2.0, -6.9, -0.6, -1},
        // ponte di mezzo: alloggi, mensa e cucina, depuratori dell'aria
        {T::cuccette, 30.0, 40.0, -3.6, -0.9, -1},
        {T::tavoli, 19.5, 29.0, -3.6, -1.6, -1},
        {T::cucina, 15.5, 19.0, -3.6, -1.4, -1},
        {T::pompe, 8.5, 14.5, -3.6, -1.6, S_ARIA},
        {T::quadri, -2.0, 2.6, -3.6, -1.0, -1},
        {T::quadri, 3.0, 7.5, -3.6, -1.0, S_INER},   // il navigatore inerziale (DMINS o WSN-3)
        // ponte inferiore: tubi e siluri, batteria, locale macchine ausiliarie 1
        {T::tubi, 33.0, 41.2, -5.9, -4.1, -1},
        {T::siluri, 13.0, 32.0, -6.9, -3.7, -1},
        {T::batterie, 35.0, 40.6, -7.9, -6.9, -1},
        {T::diesel, 4.5, 10.5, -6.9, -4.6, -1},
        {T::bombole, -2.3, 4.0, -7.7, -6.0, S_ARIA},
        {T::pompe, 10.8, 12.6, -6.9, -5.0, S_POMPE},
        // reattore: recipiente, generatori di vapore, galleria schermata in alto
        {T::reattore, -9.6, -6.1, -7.4, -1.0, S_PROP},
        {T::vapore, -11.9, -10.6, -7.0, -0.8, S_PROP},
        {T::vapore, -5.2, -3.9, -7.0, -0.8, S_PROP},
        {T::tunnel, -12.85, -2.85, -0.4, 1.6, -1},
        // macchine: locale ausiliario 2, manovra, turbine, riduttore, turbogeneratori, condensatore, asse
        {T::quadri, -17.5, -13.3, -2.9, -0.2, S_PROP},
        {T::pompe, -18.0, -13.3, -6.9, -4.6, S_POMPE},
        {T::turbina, -31.0, -21.0, -2.9, 0.9, S_MOTORI},
        {T::condensatore, -31.0, -22.5, -6.9, -4.2, S_MOTORI},
        {T::generatore, -21.5, -18.5, -6.9, -4.6, S_MOTORI},
        {T::riduttore, -35.2, -31.2, -6.9, 0.2, S_MOTORI},
        {T::generatore, -38.5, -36.0, -5.0, -1.4, S_PROP},
        {T::asse, -55.4, -35.2, -3.6, -2.8, S_PROP},
        // poppa: verricello della schiera rimorchiata
        {T::rullo, -45.5, -43.0, -1.6, 0.9, S_SCHIERA},
    };
    return a;
}

Allestimento allestimento_alfa() {
    Allestimento a;
    a.ponti = {{"C1", {-3.7, -0.9}}, {"C2", {-3.9, -1.0}}, {"C3", {-3.7, -0.7}}, {"C4", {-3.9, 0.9}},
               {"C5", {-3.7, -0.9}}, {"C6", {-3.6, -0.9}}};
    // i compartimenti sovietici numerati dalla prua, traslitterati come i pulti dell'Alfa (roadmap 33, kit 31/10/2026):
    // torpediniero, accumulatori, posto centrale, reattore, turbine, ausiliari; TSGB le casse zavorra principali
    a.nomi = {{"C1", "I TORP."}, {"C2", "II AKK."}, {"C3", "III TSP"}, {"C4", "IV REAKT."}, {"C5", "V TURB."},
              {"C6", "VI VSP."}, {"Z_PRUA", "TSGB"}, {"Z_POPPA", "TSGB"}};
    using T = Tipo;
    a.elementi = {
        {T::schiera_prua, 33.5, 39.5, -4.4, -1.6, S_PRUA},
        {T::tubi, 29.5, 38.5, -1.4, 0.9, -1},
        {T::siluri, 22.3, 29.2, -3.7, 1.6, -1},
        {T::batterie, 12.2, 21.2, -4.3, -3.9, -1},
        {T::cuccette, 12.4, 18.0, -1.0, 1.6, -1},
        {T::tavoli, 18.2, 21.2, -1.0, 0.6, -1},
        {T::consolle, 4.0, 11.4, -0.7, 0.9, S_TIRO},
        {T::consolle, 1.0, 3.8, -0.7, 0.9, S_CALC},
        {T::periscopio, 7.6, 8.2, -0.7, 13.4, S_PERI},
        {T::periscopio, 9.9, 10.4, -0.7, 13.9, S_PERI},
        {T::albero, 8.7, 8.9, 2.8, 14.4, S_ANTENNA},
        {T::albero, 12.6, 13.0, 2.8, 12.8, S_ANTENNA},
        {T::capsula, 0.2, 5.4, 3.6, 6.4, -1},
        {T::cucina, 6.0, 9.0, -3.7, -1.4, -1},
        {T::quadri, 1.0, 2.8, -3.7, -0.9, S_INER},   // il navigatore inerziale (Soz)
        {T::cuccette, 3.0, 5.8, -3.7, -0.9, -1},
        {T::pompe, 9.2, 11.4, -3.7, -1.9, S_POMPE},
        {T::scala, 5.9, 6.4, -3.7, 2.7, -1},
        {T::reattore, -6.0, -2.6, -3.9, 0.6, S_PROP},
        {T::vapore, -8.6, -7.4, -3.6, 0.8, S_PROP},
        {T::vapore, -1.6, -0.4, -3.6, 0.8, S_PROP},
        {T::tunnel, -9.3, 0.7, 0.9, 2.6, -1},
        {T::turbina, -19.5, -11.0, -1.6, 1.8, S_MOTORI},
        {T::condensatore, -19.0, -11.5, -3.7, -1.8, S_MOTORI},
        {T::generatore, -12.4, -9.8, -3.7, -1.9, S_MOTORI},
        {T::quadri, -20.9, -19.8, -0.9, 1.9, S_PROP},
        {T::riduttore, -25.5, -21.6, -3.6, 1.2, S_MOTORI},
        {T::bombole, -30.8, -26.2, -3.6, -2.2, S_ARIA},
        {T::generatore, -30.5, -27.0, -0.9, 1.3, S_PROP},
        {T::asse, -42.0, -25.5, -1.2, -0.5, S_PROP},
    };
    return a;
}

// Allestimento generico dai compartimenti (Sturgeon, Trafalgar e le altre classi): ponti e macchinari per uso
Allestimento allestimento_generico(const DatiScafo& sc) {
    Allestimento a;
    double r = 0, zr = 0;
    for (const auto& s : sc.stazioni)
        if (s.r_resistente > r) { r = s.r_resistente; zr = s.z_resistente; }
    using T = Tipo;
    bool inerziale = false;
    for (const auto& c : sc.celle) {
        if (c.uso == "zavorra") { a.nomi[c.nome] = "ZAVORRA"; continue; }
        const double f = zr - r + 1.25, m = zr - 0.25 * r, u = zr + 0.35 * r, top = zr + 0.8 * r;
        const double L = c.x1 - c.x0;
        if (c.uso == "reattore") {
            a.ponti[c.nome] = {f - 0.5, zr + 0.55 * r};
            a.nomi[c.nome] = "REATTORE";
            const double xc = 0.5 * (c.x0 + c.x1);
            a.elementi.push_back({T::reattore, xc - 1.7, xc + 1.7, f - 0.5, zr + 0.45 * r, S_PROP});
            a.elementi.push_back({T::vapore, xc - 3.4, xc - 2.3, f - 0.2, zr + 0.45 * r, S_PROP});
            a.elementi.push_back({T::vapore, xc + 2.3, xc + 3.4, f - 0.2, zr + 0.45 * r, S_PROP});
            a.elementi.push_back({T::tunnel, c.x0, c.x1, zr + 0.55 * r, top, -1});
        } else if (c.uso == "macchine") {
            a.ponti[c.nome] = {f, m + 0.4};
            a.nomi[c.nome] = "MACCHINE";
            a.elementi.push_back({T::turbina, c.x0 + 0.3 * L, c.x0 + 0.75 * L, m + 0.4, u + 0.6, S_MOTORI});
            a.elementi.push_back({T::condensatore, c.x0 + 0.3 * L, c.x0 + 0.72 * L, f, m, S_MOTORI});
            a.elementi.push_back({T::riduttore, c.x0 + 0.12 * L, c.x0 + 0.28 * L, f, u, S_MOTORI});
            a.elementi.push_back({T::quadri, c.x0 + 0.8 * L, c.x1 - 0.5, m + 0.4, u, S_PROP});
            a.elementi.push_back({T::generatore, c.x0 + 0.78 * L, c.x1 - 0.6, f, m - 0.3, S_MOTORI});
        } else {
            a.ponti[c.nome] = {f, m, u - 0.3};
            a.nomi[c.nome] = c.uso == "batterie" ? "BATTERIE" : "PRUA: COMANDO, ALLOGGI, SILURI";
            a.elementi.push_back({T::siluri, c.x0 + 0.45 * L, c.x1 - 2.0, f, m - 0.1, -1});
            a.elementi.push_back({T::tubi, c.x1 - 2.2, c.x1 + 6.5, f + 0.8, m - 0.3, -1});
            a.elementi.push_back({T::cuccette, c.x0 + 0.55 * L, c.x1 - 1.0, m, u - 0.6, -1});
            a.elementi.push_back({T::tavoli, c.x0 + 0.2 * L, c.x0 + 0.5 * L, m, u - 1.5, -1});
            a.elementi.push_back({T::consolle, c.x0 + 0.5 * L, c.x0 + 0.85 * L, u - 0.3, u + 1.2, S_TIRO});
            a.elementi.push_back({T::consolle, c.x0 + 0.86 * L, c.x1 - 0.4, u - 0.3, u + 1.2, S_CALC});
            a.elementi.push_back({T::diesel, c.x0 + 0.5, c.x0 + 0.35 * L, f, m - 0.4, -1});
            a.elementi.push_back({T::bombole, c.x0 + 0.1, c.x0 + 0.2 * L, f - 0.9, f - 0.1, S_ARIA});
            if (!inerziale && c.uso != "batterie") {   // il navigatore inerziale, una volta, accanto al comando
                a.elementi.push_back({T::quadri, c.x0 + 0.22 * L, c.x0 + 0.46 * L, u - 0.3, u + 1.2, S_INER});
                inerziale = true;
            }
        }
    }
    return a;
}

}  // namespace

// ---------------------------------------------------------------------------------------------------------------

// porzione del battello da disegnare (m, coordinate del kit)
struct Spaccato::Vista {
    double xa = 0, xb = 0, za = 0, zb = 0;
    bool operator==(const Vista& o) const { return xa == o.xa && xb == o.xb && za == o.za && zb == o.zb; }
};

struct Spaccato::Dati {
    struct Tri {
        float x[3], z[3];
        std::int16_t parte;
    };
    std::vector<Tri> tri;
    std::vector<std::string> parti;
    std::vector<int> sist_parte;     // sistema per parte del modello
    std::vector<char> tipo_parte;    // 's' scafo, 'v' vela, 'e' elica, 'a' albero, 't' timoni e altro
    double xmin = 1e9, xmax = -1e9, zmin = 1e9, zmax = -1e9;
    DatiScafo scafo;
    Allestimento all;
    std::vector<int> comp_di_cella;   // indice fra le celle non di zavorra (ordine di BattelloDanni) o -1
    std::vector<int> cassa_di_cella;  // indice fra le celle di zavorra o -1

    using Vista = Spaccato::Vista;
    // disegno di base in cache per rettangolo e vista (lo spaccato intero e un dettaglio)
    struct Cache {
        int w = 0, h = 0;
        Vista v;
        std::string carico;   // firma del carico delle rastrelliere disegnato
        std::vector<Px> base;
        std::vector<std::int16_t> parte;
    };
    mutable std::vector<Cache> cache;
    mutable Cache* corrente = nullptr;
    mutable const std::vector<SagomaArma>* carico = nullptr;   // durante costruisci
    SagomaArma predefinita;                                    // siluro delle rastrelliere senza carico

    struct Mappa {
        double s = 1, x0 = 0, z0 = 0;
        int ox = 0, oy = 0;
        double px(double x) const { return ox + (x - x0) * s; }
        double py(double z) const { return oy + (z0 - z) * s; }
        double xm(double p) const { return x0 + (p - ox) / s; }
        double zm(double p) const { return z0 - (p - oy) / s; }   // x0, z0: centro della vista in (ox, oy)
    };
    Vista intera() const { return {xmin, xmax, zmin, zmax}; }
    // la vista alla scala piu' grande che ci sta, centrata nel rettangolo
    Mappa mappa(int w, int h, const Vista& v) const {
        Mappa m;
        const double marg = 4;
        m.s = std::min((w - 2 * marg) / (v.xb - v.xa), (h - 2 * marg) / (v.zb - v.za));
        m.x0 = 0.5 * (v.xa + v.xb);
        m.z0 = 0.5 * (v.za + v.zb);
        m.ox = w / 2;
        m.oy = h / 2;
        return m;
    }
    static std::string firma(const std::vector<SagomaArma>* c) {
        if (!c) return "-";
        std::string f;
        for (const auto& a : *c) f += a.id + (a.esercitazione ? "*" : "") + ":" + std::to_string(a.lunghezza_m) + ";";
        return f;
    }
    Cache& prepara(int w, int h, const Vista& v, const std::vector<SagomaArma>* rastrelliera) const {
        const std::string f = firma(rastrelliera);
        for (auto& k : cache)
            if (k.w == w && k.h == h && k.v == v && k.carico == f) return *(corrente = &k);
        if (cache.size() >= 4) cache.erase(cache.begin());
        cache.push_back({});
        corrente = &cache.back();
        corrente->w = w;
        corrente->h = h;
        corrente->v = v;
        corrente->carico = f;
        carico = rastrelliera;
        costruisci(w, h, v);
        carico = nullptr;
        return *corrente;
    }

    // scafo resistente interpolato fra le stazioni
    void resistente(double x, double& r, double& zc) const {
        const auto& st = scafo.stazioni;
        r = 0;
        zc = 0;
        if (st.size() < 2 || x < st.front().x || x > st.back().x) return;
        std::size_t i = 0;
        while (i + 2 < st.size() && st[i + 1].x < x) ++i;
        const auto& a = st[i];
        const auto& b = st[i + 1];
        if (a.r_resistente <= 0 || b.r_resistente <= 0) return;
        const double f = (x - a.x) / (b.x - a.x);
        r = a.r_resistente * (1 - f) + b.r_resistente * f;
        zc = a.z_resistente * (1 - f) + b.z_resistente * f;
    }
    // cella che contiene x: dentro lo scafo resistente il compartimento, fuori la cassa
    int cella(double x, bool resistente_) const {
        int ris = -1;
        for (std::size_t i = 0; i < scafo.celle.size(); ++i) {
            const auto& c = scafo.celle[i];
            if (x < c.x0 || x >= c.x1) continue;
            const bool z = c.uso == "zavorra";
            if (resistente_ ? !z : z) return static_cast<int>(i);
            if (ris < 0) ris = static_cast<int>(i);
        }
        return ris;
    }
    std::vector<double> ponti(int cella_) const {
        if (cella_ < 0) return {};
        const auto it = all.ponti.find(scafo.celle[static_cast<std::size_t>(cella_)].nome);
        return it == all.ponti.end() ? std::vector<double>{} : it->second;
    }

    void costruisci(int w, int h, const Vista& v) const;
    void elemento(const Elemento& e, const Mappa& m) const;
    // pixel dell'elemento: solo dentro il taglio (o nella vela per gli alberi)
    void metti(int x, int y, Mat mat, double tono, int sist, bool anche_fuori = false) const {
        if (x < 0 || y < 0 || x >= corrente->w || y >= corrente->h) return;
        auto& p = corrente->base[static_cast<std::size_t>(y) * corrente->w + x];
        if (!anche_fuori && !p.interno) return;
        if (p.mat == Mat::taglio || p.mat == Mat::scafo || p.mat == Mat::mare) return;
        p.mat = mat;
        const int mx = mat == Mat::bronzo || mat == Mat::cassa ? 3 : 7;
        p.tono = static_cast<std::uint8_t>(retina(tono, x, y, mx));
        p.sist = static_cast<std::int8_t>(sist);
    }
    void rettangolo(double x0, double y0, double x1, double y1, Mat mat, double tono, int sist, bool fuori = false) const {
        for (int y = static_cast<int>(std::floor(std::min(y0, y1))); y < static_cast<int>(std::ceil(std::max(y0, y1))); ++y)
            for (int x = static_cast<int>(std::floor(std::min(x0, x1))); x < static_cast<int>(std::ceil(std::max(x0, x1))); ++x)
                metti(x, y, mat, tono, sist, fuori);
    }
    // cilindro orizzontale (asse lungo x) da x0 a x1, raggio in pixel che puo' variare da ra a rb: luce dall'alto
    void cil_h(double x0, double x1, double yc, double ra, double rb, Mat mat, int sist, double luce = 1.0) const {
        for (int x = static_cast<int>(std::floor(x0)); x < static_cast<int>(std::ceil(x1)); ++x) {
            const double f = std::clamp((x + 0.5 - x0) / std::max(x1 - x0, 1.0), 0.0, 1.0);
            const double r = ra * (1 - f) + rb * f;
            for (int y = static_cast<int>(std::floor(yc - r)); y < static_cast<int>(std::ceil(yc + r)); ++y) {
                const double v = (y + 0.5 - yc) / std::max(r, 0.5);   // -1 in alto, +1 in basso
                if (std::fabs(v) > 1.0) continue;
                const double t = (6.6 - 3.0 * (v + 1.0) - (v < -0.55 ? 0.0 : 0.0)) * luce + (v > -0.75 && v < -0.45 ? 0.9 : 0.0);
                metti(x, y, mat, std::clamp(t, 0.6, 7.0), sist);
            }
        }
    }
    // cilindro verticale: luce da sinistra in alto
    void cil_v(double xc, double r, double y0, double y1, Mat mat, int sist, bool fuori = false) const {
        for (int y = static_cast<int>(std::floor(y0)); y < static_cast<int>(std::ceil(y1)); ++y)
            for (int x = static_cast<int>(std::floor(xc - r)); x < static_cast<int>(std::ceil(xc + r)); ++x) {
                const double u = (x + 0.5 - xc) / std::max(r, 0.5);
                if (std::fabs(u) > 1.0) continue;
                const double t = 6.4 - 2.6 * (u + 1.0) + (u > -0.6 && u < -0.25 ? 1.0 : 0.0);
                metti(x, y, mat, std::clamp(t, 0.8, 7.0), sist, fuori);
            }
    }
    void disco(double xc, double yc, double r, Mat mat, int sist, double scuro = 0.0) const {
        for (int y = static_cast<int>(std::floor(yc - r)); y < static_cast<int>(std::ceil(yc + r)); ++y)
            for (int x = static_cast<int>(std::floor(xc - r)); x < static_cast<int>(std::ceil(xc + r)); ++x) {
                const double u = (x + 0.5 - xc) / r, v = (y + 0.5 - yc) / r;
                const double d = u * u + v * v;
                if (d > 1.0) continue;
                // sfera illuminata da sinistra in alto
                const double nz = std::sqrt(std::max(0.0, 1 - d));
                const double l = std::clamp(-0.55 * u - 0.6 * v + 0.6 * nz, 0.0, 1.0);
                metti(x, y, mat, 0.8 + 6.2 * l - scuro, sist);
            }
    }
    void linea(double x0, double y0, double x1, double y1, Mat mat, double tono, int sist, bool fuori = false) const {
        const int n = static_cast<int>(std::ceil(std::max(std::fabs(x1 - x0), std::fabs(y1 - y0)))) + 1;
        for (int i = 0; i <= n; ++i) {
            const double f = static_cast<double>(i) / n;
            metti(static_cast<int>(std::floor(x0 + (x1 - x0) * f)), static_cast<int>(std::floor(y0 + (y1 - y0) * f)), mat, tono, sist, fuori);
        }
    }
};

Spaccato::Spaccato(const std::string& classe, const Modello& modello, const DatiScafo& scafo)
    : classe_(classe), d_(std::make_unique<Dati>()) {
    auto& d = *d_;
    d.scafo = scafo;
    d.parti = modello.parti();
    for (const auto& p : d.parti) {
        int s = -1;
        char t = 't';
        if (p == "scafo") t = 's';
        else if (p == "vela") t = 'v';
        else if (p == "elica") { t = 'e'; s = S_PROP; }
        else if (p.rfind("periscopio", 0) == 0) { t = 'a'; s = S_PERI; }
        else if (p.rfind("albero", 0) == 0 || p == "antenna" || p == "cupola_albero") { t = 'a'; s = S_ANTENNA; }
        else if (p == "timone" || p == "timone_inferiore") s = S_TIMONE;
        else if (p.find("timoni") != std::string::npos || p == "piastre_timoni") s = S_IDRO;
        d.tipo_parte.push_back(t);
        d.sist_parte.push_back(s);
    }
    for (const auto& t : modello.triangoli()) {
        Dati::Tri q{};
        for (int k = 0; k < 3; ++k) {
            q.x[k] = t.v[static_cast<std::size_t>(k)][0];
            q.z[k] = t.v[static_cast<std::size_t>(k)][2];
            d.xmin = std::min(d.xmin, static_cast<double>(q.x[k]));
            d.xmax = std::max(d.xmax, static_cast<double>(q.x[k]));
            d.zmin = std::min(d.zmin, static_cast<double>(q.z[k]));
            d.zmax = std::max(d.zmax, static_cast<double>(q.z[k]));
        }
        q.parte = static_cast<std::int16_t>(t.parte);
        d.tri.push_back(q);
    }
    d.predefinita.id = classe == "alfa" ? "type53_65k" : "mk48";
    d.predefinita.sovietica = classe == "alfa";
    d.all = classe == "los_angeles" ? allestimento_688() : classe == "alfa" ? allestimento_alfa() : allestimento_generico(scafo);
    int nc = 0, nz = 0;
    for (const auto& c : scafo.celle) {
        const bool z = c.uso == "zavorra";
        d.comp_di_cella.push_back(z ? -1 : nc++);
        d.cassa_di_cella.push_back(z ? nz++ : -1);
    }
}

Spaccato::~Spaccato() = default;
Spaccato::Spaccato(Spaccato&&) noexcept = default;

void Spaccato::imposta_palette(Palette& p) {
    for (const auto& r : rampe())
        for (std::size_t i = 0; i < r.rgb.size(); ++i)
            p.imposta(r.primo + static_cast<int>(i), r.rgb[i][0], r.rgb[i][1], r.rgb[i][2]);
}

StatoSpaccato StatoSpaccato::da(const BattelloDanni& b, double quota_m, double assetto_rad, double tempo_s) {
    StatoSpaccato s;
    s.compartimenti = &b.compartimenti();
    s.falle = &b.falle();
    s.paratie = &b.paratie();
    s.casse = &b.casse();
    s.sistemi = &b.sistemi();
    s.quota_m = quota_m;
    s.assetto_rad = assetto_rad;
    s.tempo_s = tempo_s;
    s.squadre = &b.squadre();
    s.esaurimento = b.esaurimento();
    s.scram = b.limiti().scram;
    return s;
}

int Spaccato::x_tela(Rettangolo r, double x_m) const {
    return r.x + static_cast<int>(std::lround(d_->mappa(r.w, r.h, d_->intera()).px(x_m)));
}

int Spaccato::compartimenti() const {
    int n = 0;
    for (const auto& k : d_->comp_di_cella) n += k >= 0;
    return n;
}

bool Spaccato::estensione(int compartimento, double& x0, double& x1) const {
    for (std::size_t i = 0; i < d_->comp_di_cella.size(); ++i)
        if (d_->comp_di_cella[i] == compartimento) {
            x0 = d_->scafo.celle[i].x0;
            x1 = d_->scafo.celle[i].x1;
            return true;
        }
    return false;
}

void Spaccato::disegna(Tela& t, Rettangolo r, const StatoSpaccato& s) const { disegna_vista(t, r, s, d_->intera()); }

void Spaccato::disegna_dettaglio(Tela& t, Rettangolo r, const StatoSpaccato& s, double x0_m, double x1_m) const {
    // dal fondo dello scafo a 3 m sopra lo scafo resistente (la base della vela); in larghezza almeno x0-x1
    double zb = -1e9, za = 1e9;
    for (const auto& st : d_->scafo.stazioni)
        if (st.r_resistente > 0) {
            zb = std::max(zb, st.z_resistente + st.r_resistente);
            za = std::min(za, st.z_resistente - st.r_resistente);
        }
    if (zb < za) { za = d_->zmin; zb = d_->zmax; }
    za -= 0.8;
    zb += 3.0;
    const double xc = 0.5 * (x0_m + x1_m);
    double meta = 0.5 * std::max(x1_m - x0_m, 1.0);
    const double s_alt = (r.h - 8) / (zb - za);
    meta = std::max(meta, 0.5 * (r.w - 8) / s_alt);   // riempie la larghezza alla scala dell'altezza
    // senza uscire dal battello finche' c'e' battello da mostrare
    double c = xc;
    if (2 * meta < d_->xmax - d_->xmin) c = std::clamp(xc, d_->xmin - 1.0 + meta, d_->xmax + 1.0 - meta);
    disegna_vista(t, r, s, {c - meta, c + meta, za, zb});
}

// --- disegno di base: profilo, taglio, locali e macchinari ---
void Spaccato::Dati::costruisci(int w, int h, const Vista& v) const {
    auto& base = corrente->base;
    auto& parte_px = corrente->parte;
    base.assign(static_cast<std::size_t>(w) * h, Px{});
    parte_px.assign(static_cast<std::size_t>(w) * h, -1);
    const Mappa m = mappa(w, h, v);
    // 1. profilo: proiezione dei triangoli sul piano x-z (lo scafo per ultimo, sotto vela e timoni)
    for (int giro = 0; giro < 2; ++giro)
        for (const auto& t : tri) {
            const bool sc = tipo_parte[static_cast<std::size_t>(t.parte)] == 's';
            if ((giro == 0) != sc) continue;
            double X[3], Y[3];
            for (int k = 0; k < 3; ++k) { X[k] = m.px(t.x[k]); Y[k] = m.py(t.z[k]); }
            const double area = (X[1] - X[0]) * (Y[2] - Y[0]) - (X[2] - X[0]) * (Y[1] - Y[0]);
            if (std::fabs(area) < 1e-9) continue;
            const int x0 = std::max(0, static_cast<int>(std::floor(std::min({X[0], X[1], X[2]})))),
                      x1 = std::min(w - 1, static_cast<int>(std::ceil(std::max({X[0], X[1], X[2]})))),
                      y0 = std::max(0, static_cast<int>(std::floor(std::min({Y[0], Y[1], Y[2]})))),
                      y1 = std::min(h - 1, static_cast<int>(std::ceil(std::max({Y[0], Y[1], Y[2]}))));
            for (int y = y0; y <= y1; ++y)
                for (int x = x0; x <= x1; ++x) {
                    const double px = x + 0.5, py = y + 0.5;
                    const double a = ((X[1] - px) * (Y[2] - py) - (X[2] - px) * (Y[1] - py)) / area,
                                 b = ((X[2] - px) * (Y[0] - py) - (X[0] - px) * (Y[2] - py)) / area;
                    if (a < -1e-6 || b < -1e-6 || a + b > 1 + 1e-6) continue;
                    auto& q = parte_px[static_cast<std::size_t>(y) * w + x];
                    if (q < 0 || sc == false) q = t.parte;
                }
        }
    auto parte = [&](int x, int y) -> int {
        if (x < 0 || y < 0 || x >= w || y >= h) return -1;
        return parte_px[static_cast<std::size_t>(y) * w + x];
    };
    auto tipo = [&](int x, int y) -> char {
        const int p = parte(x, y);
        return p < 0 ? ' ' : tipo_parte[static_cast<std::size_t>(p)];
    };
    const int pelle = std::max(2, static_cast<int>(std::lround(0.35 * m.s)));     // spessore dello scafo esterno
    const int taglio = std::max(1, static_cast<int>(std::lround(0.18 * m.s)));    // scafo resistente tagliato
    // 2. materiali
    for (int y = 0; y < h; ++y)
        for (int x = 0; x < w; ++x) {
            auto& p = base[static_cast<std::size_t>(y) * w + x];
            const double xm = m.xm(x + 0.5), zm = m.zm(y + 0.5);
            // mare: tono che scende con la quota del disegno
            p.mat = Mat::mare;
            p.tono = static_cast<std::uint8_t>(retina(5.2 - 4.8 * y / std::max(h - 1, 1), x, y, 7));
            const char t = tipo(x, y);
            if (t == ' ') continue;
            const int pt = parte(x, y);
            p.sist = static_cast<std::int8_t>(sist_parte[static_cast<std::size_t>(pt)]);
            // distanza dal bordo della propria sagoma (scafo e vela insieme per la pelle)
            auto stessa = [&](int xx, int yy) {
                const char q = tipo(xx, yy);
                return t == 's' || t == 'v' ? (q == 's' || q == 'v') : q == t;
            };
            int bordo = 99;
            for (int k = 1; k <= pelle && bordo == 99; ++k)
                if (!stessa(x, y - k) || !stessa(x, y + k) || !stessa(x - k, y) || !stessa(x + k, y)) bordo = k;
            if (t == 's' || t == 'v') {
                // sopra o sotto? per l'ombreggiatura della pelle
                const bool sopra = !stessa(x, y - bordo) || (stessa(x, y + bordo) && stessa(x, y - bordo) && zm > 0);
                if (bordo <= pelle) {
                    p.mat = Mat::scafo;
                    double tt = sopra ? 6.0 - 1.4 * (bordo - 1) : 2.2 - 0.6 * (bordo - 1);
                    if (bordo == 1 && sopra) tt = 7.0;
                    p.tono = static_cast<std::uint8_t>(retina(tt, x, y, 7));
                    continue;
                }
                double r, zc;
                resistente(xm, r, zc);
                const bool dentro = t == 's' && r > 0 && std::fabs(zm - zc) <= r;
                if (dentro) {
                    const double dist = (r - std::fabs(zm - zc)) * m.s;   // pixel dal bordo dello scafo resistente
                    const int c = cella(xm, true);
                    p.cella = static_cast<std::int8_t>(c);
                    if (dist < taglio) {
                        p.mat = Mat::taglio;
                        p.tono = static_cast<std::uint8_t>(zm > zc ? 3 : 1);
                        continue;
                    }
                    p.interno = true;
                    p.mat = Mat::vano;
                    p.sist = -1;
                    // ponti: pavimenti, luci sotto il soffitto
                    const auto pv = ponti(c);
                    double pav = zc - r, sof = zc + r;
                    for (std::size_t k = 0; k < pv.size(); ++k)
                        if (zm >= pv[k]) {
                            pav = pv[k];
                            sof = k + 1 < pv.size() ? pv[k + 1] : zc + r;
                        }
                    if (pv.empty() || zm < pv.front()) {   // sentina
                        p.tono = static_cast<std::uint8_t>(retina(1.0, x, y, 7));
                        continue;
                    }
                    const double piano = (zm - pav) * m.s;   // pixel sopra il pavimento
                    if (piano < std::max(1.5, 0.22 * m.s)) {   // lamiera del ponte
                        p.mat = Mat::metallo;
                        p.tono = static_cast<std::uint8_t>(piano < 1.0 ? 5 : 2);
                        continue;
                    }
                    const double rel = (zm - pav) / std::max(sof - pav, 0.1);
                    const bool luce = (sof - zm) * m.s < 1.2 && std::fmod(std::fabs(xm), 3.0) < 1.1;
                    p.tono = static_cast<std::uint8_t>(luce ? 7 : retina(1.6 + 3.6 * rel, x, y, 6));
                    continue;
                }
                // fuori dallo scafo resistente: casse di zavorra, vela e prua allagabili
                const int c = t == 's' ? cella(xm, false) : -1;
                if (c >= 0 && scafo.celle[static_cast<std::size_t>(c)].uso == "zavorra") {
                    p.mat = Mat::cassa;
                    p.cella = static_cast<std::int8_t>(c);
                    p.interno = true;
                    p.sist = -1;
                    // ordinate interne della cassa
                    p.tono = static_cast<std::uint8_t>(std::fmod(std::fabs(xm) + 0.3, 1.6) < 0.25 ? 3 : retina(1.2 + 0.8 * (zm > 0), x, y, 2));
                } else {
                    p.mat = Mat::allagabile;
                    p.sist = -1;
                    p.tono = static_cast<std::uint8_t>(retina(0.6 + 0.8 * (1 - y / static_cast<double>(h)), x, y, 3));
                }
                continue;
            }
            // parti esterne: elica in bronzo, alberi in metallo, timoni come lo scafo
            if (t == 'e') {
                p.mat = Mat::bronzo;
                p.tono = static_cast<std::uint8_t>(retina(bordo == 1 ? 3.0 : 1.6 + 1.2 * ((x + y) % 5 == 0), x, y, 3));
            } else if (t == 'a') {
                p.mat = Mat::metallo;
                p.tono = static_cast<std::uint8_t>(bordo == 1 && !stessa(x - 1, y) ? 6 : 3);
            } else {
                p.mat = Mat::scafo;
                p.tono = static_cast<std::uint8_t>(bordo == 1 ? (!stessa(x, y - 1) ? 6 : 1) : retina(3.6 - 1.6 * (zm < -3), x, y, 7));
            }
        }
    // 3. paratie fra i compartimenti, con le porte stagne
    for (std::size_t i = 0; i + 1 <= scafo.celle.size(); ++i) {
        const auto& c = scafo.celle[i];
        if (c.uso == "zavorra") continue;
        for (const double xb : {c.x0, c.x1}) {
            const int px = static_cast<int>(std::lround(m.px(xb)));
            const auto pv = ponti(static_cast<int>(i));
            for (int y = 0; y < h; ++y)
                for (int dx = -1; dx <= 0; ++dx) {
                    const int x = px + dx;
                    if (x < 0 || x >= w) continue;
                    auto& p = base[static_cast<std::size_t>(y) * w + x];
                    if (!p.interno && p.mat != Mat::taglio) continue;
                    if (p.mat == Mat::taglio) continue;
                    // porta: un'apertura ovale sopra ogni pavimento
                    const double zm = m.zm(y + 0.5);
                    bool porta = false;
                    for (const double z : pv)
                        if (zm > z + 0.35 && zm < z + 1.75) porta = true;
                    p.mat = Mat::parete;
                    p.tono = static_cast<std::uint8_t>(porta ? (dx == 0 ? 2 : 1) : (dx == 0 ? 5 : 3));
                }
        }
    }
    // 4. macchinari e arredi
    for (const auto& e : all.elementi) elemento(e, m);
}

void Spaccato::Dati::elemento(const Elemento& e, const Mappa& m) const {
    const double X0 = m.px(e.x0), X1 = m.px(e.x1), Y0 = m.py(e.z1), Y1 = m.py(e.z0);   // Y0 in alto
    const double W = X1 - X0, H = Y1 - Y0, s = m.s;
    const int sist = e.sist;
    switch (e.tipo) {
        case Tipo::consolle: {
            // armadi da 0,9 m con schermi verdi, ambra e blu
            const int n = std::max(1, static_cast<int>(std::lround(W / (0.9 * s))));
            for (int k = 0; k < n; ++k) {
                const double a = X0 + W * k / n, b = X0 + W * (k + 1) / n - 1;
                rettangolo(a, Y0, b, Y1, Mat::metallo, 2.6 + 0.8 * (k & 1), sist);
                linea(a, Y0, b, Y0, Mat::metallo, 6, sist);
                const int scherm = k % 3 == 0 ? VERDE : k % 3 == 1 ? AMBRA : BLU_SCHERMO;
                rettangolo(a + 1, Y0 + 0.18 * H, b - 1, Y0 + 0.5 * H, Mat::accento, scherm, sist);
                rettangolo(a + 1, Y0 + 0.62 * H, b - 1, Y0 + 0.7 * H, Mat::metallo, 5, sist);
                if ((k * 7) % 3 == 0) metti(static_cast<int>(a + 1), static_cast<int>(Y0 + 0.8 * H), Mat::accento, ROSSO, sist);
            }
            break;
        }
        case Tipo::cuccette: {
            // tre file di brande con coperta e cuscino
            const int file = std::max(2, std::min(3, static_cast<int>(H / (0.6 * s))));
            const int n = std::max(1, static_cast<int>(std::lround(W / (2.0 * s))));
            for (int k = 0; k < n; ++k) {
                const double a = X0 + W * k / n + 1, b = X0 + W * (k + 1) / n - 1;
                linea(a, Y0, a, Y1, Mat::metallo, 4, sist);
                for (int f = 0; f < file; ++f) {
                    const double y = Y0 + H * (f + 1) / (file + 0.2);
                    linea(a, y, b, y, Mat::metallo, 5, sist);
                    rettangolo(a + 1, y - std::max(1.0, 0.18 * s), b, y, Mat::metallo, 3.2, sist);
                    rettangolo(b - std::max(1.0, 0.35 * s), y - std::max(1.0, 0.2 * s), b, y, Mat::accento, BIANCO_CALDO, sist);
                }
            }
            break;
        }
        case Tipo::tavoli: {
            const int n = std::max(1, static_cast<int>(std::lround(W / (2.4 * s))));
            for (int k = 0; k < n; ++k) {
                const double a = X0 + W * k / n + 0.3 * s, b = X0 + W * (k + 1) / n - 0.3 * s;
                const double ytav = Y1 - 0.75 * s;
                linea(a, ytav, b, ytav, Mat::metallo, 7, sist);
                linea(a + 1, ytav + 1, b - 1, ytav + 1, Mat::metallo, 4, sist);
                linea(0.5 * (a + b), ytav, 0.5 * (a + b), Y1, Mat::metallo, 3, sist);
                rettangolo(a - 0.3 * s, Y1 - 0.45 * s, a + 0.1 * s, Y1, Mat::accento, ROSSO_SCURO, sist);
                rettangolo(b - 0.1 * s, Y1 - 0.45 * s, b + 0.3 * s, Y1, Mat::accento, ROSSO_SCURO, sist);
            }
            break;
        }
        case Tipo::cucina:
            rettangolo(X0, Y1 - 0.9 * s, X1, Y1, Mat::metallo, 5.5, sist);
            linea(X0, Y1 - 0.9 * s, X1, Y1 - 0.9 * s, Mat::metallo, 7, sist);
            for (double x = X0 + 0.4 * s; x < X1 - 0.3 * s; x += 0.8 * s) rettangolo(x, Y1 - 0.6 * s, x + 0.4 * s, Y1 - 0.2 * s, Mat::metallo, 2, sist);
            rettangolo(X0, Y0, X1, Y0 + 0.5 * s, Mat::metallo, 4, sist);   // cappa
            break;
        case Tipo::siluri: {
            // rastrelliere col carico vero (StatoSpaccato::rastrelliera): una sagoma per posto, in scala, prua a destra
            const double r = std::max(1.2, 0.27 * s), passo = std::max(2 * r + 1, 0.65 * s);
            double lmax = 0.0;
            if (carico)
                for (const auto& a : *carico)
                    if (!a.id.empty()) lmax = std::max(lmax, lunghezza_disegnata(a));
            const double lung = std::min(W, (lmax > 0.0 ? lmax : 6.0) * s);
            std::vector<std::pair<double, double>> posti;   // (x, asse)
            for (double y = Y1 - r - 1; y - r > Y0; y -= passo) {
                for (double x = X0; x + lung * 0.6 <= X1; x += lung + 0.4 * s) posti.push_back({x, y});
                linea(X0, y + r + 0.5, X1, y + r + 0.5, Mat::metallo, 2, sist);   // rastrelliera
            }
            for (std::size_t k = 0; k < posti.size(); ++k) {
                const SagomaArma* a = &predefinita;
                if (carico) {
                    if (carico->empty()) break;
                    a = &(*carico)[k * carico->size() / posti.size()];
                }
                if (a->id.empty()) continue;
                const auto [x, y] = posti[k];
                const double l = lunghezza_disegnata(*a);
                const double px = std::min(s, (std::min(x + lung, X1) - x) / l);
                traccia_sagoma_arma(*a, x, y, px, [&](int xx, int yy, MaterialeArma mm, double t) {
                    // i materiali delle sagome nelle rampe dello spaccato
                    switch (mm) {
                        case MaterialeArma::nero: return metti(xx, yy, Mat::metallo, 2.0 * t, sist);
                        case MaterialeArma::grigio: return metti(xx, yy, Mat::metallo, 1.0 + 5.0 * t, sist);
                        case MaterialeArma::argento: return metti(xx, yy, Mat::metallo, 3.0 + 4.0 * t, sist);
                        case MaterialeArma::bianco: return metti(xx, yy, Mat::accento, BIANCO_CALDO, sist);
                        case MaterialeArma::kaki: return metti(xx, yy, Mat::metallo, 1.5 + 3.5 * t, sist);
                        case MaterialeArma::verde:
                        case MaterialeArma::oliva:
                            if (t < 0.3) return metti(xx, yy, Mat::metallo, 1, sist);
                            if (t > 0.8 && (xx + yy) % 3 == 0)   // riflesso retinato
                                return metti(xx, yy, mm == MaterialeArma::verde ? Mat::accento : Mat::metallo,
                                             mm == MaterialeArma::verde ? VERDE : 4, sist);
                            return metti(xx, yy, Mat::accento, VERDE_SCURO, sist);
                        case MaterialeArma::verde_chiaro:
                            return (xx + yy) & 1 ? metti(xx, yy, Mat::accento, VERDE_SCURO, sist) : metti(xx, yy, Mat::metallo, 2.0 + 4.0 * t, sist);
                        case MaterialeArma::arancione: return metti(xx, yy, Mat::accento, AMBRA, sist);
                        case MaterialeArma::rosso: return metti(xx, yy, Mat::accento, t < 0.4 ? ROSSO_SCURO : ROSSO, sist);
                        case MaterialeArma::giallo: return metti(xx, yy, Mat::accento, GIALLO, sist);
                        case MaterialeArma::bronzo: return metti(xx, yy, Mat::bronzo, 3.0 * t, sist);
                        case MaterialeArma::acqua: return metti(xx, yy, Mat::accento, BLU_SCHERMO, sist);
                    }
                });
            }
            break;
        }
        case Tipo::tubi: {
            const int n = H > 2.5 * s ? 2 : 1;
            for (int k = 0; k < n; ++k) {
                const double yc = Y0 + H * (k + 0.5) / n, r = std::max(1.5, 0.38 * s);
                cil_h(X0, X1, yc, r, r * 0.9, Mat::metallo, sist);
                // culatta con il portello giallo
                for (int yy = static_cast<int>(yc - r - 1); yy <= yc + r + 1; ++yy) {
                    metti(static_cast<int>(X0), yy, Mat::accento, GIALLO, sist);
                    metti(static_cast<int>(X0) + 1, yy, Mat::metallo, 6, sist);
                }
            }
            break;
        }
        case Tipo::sfera: {
            // sfera del sonar con gli idrofoni a punti
            const double r = std::min(W, H) / 2, xc = (X0 + X1) / 2, yc = (Y0 + Y1) / 2;
            for (int y = static_cast<int>(yc - r); y <= yc + r; ++y)
                for (int x = static_cast<int>(xc - r); x <= xc + r; ++x) {
                    const double u = (x + 0.5 - xc) / r, v = (y + 0.5 - yc) / r, d = u * u + v * v;
                    if (d > 1) continue;
                    const double nz = std::sqrt(1 - d);
                    double t = 1.2 + 5.6 * std::clamp(-0.5 * u - 0.55 * v + 0.65 * nz, 0.0, 1.0);
                    if ((x + 2 * y) % 4 == 0 && (y % 2 == 0) && d < 0.85) t -= 2.2;   // idrofoni
                    metti(x, y, Mat::metallo, t, sist, true);
                }
            break;
        }
        case Tipo::schiera_prua: {
            rettangolo(X0, Y0, X1, Y1, Mat::metallo, 2.2, sist, true);
            for (double x = X0 + 1; x < X1; x += std::max(2.0, 0.45 * s)) linea(x, Y0 + 1, x, Y1 - 1, Mat::metallo, 5, sist, true);
            linea(X0, Y0, X1, Y0, Mat::metallo, 6.5, sist, true);
            break;
        }
        case Tipo::periscopio:
        case Tipo::albero: {
            // tubo dal ponte (o dalla vela) fino alla testa dell'albero nel modello
            const double xc = (X0 + X1) / 2, r = std::max(e.tipo == Tipo::periscopio ? 1.0 : 0.6, W / 2);
            cil_v(xc, r, Y0, Y1, Mat::metallo, sist, true);
            if (e.tipo == Tipo::periscopio) {   // oculare con le maniglie
                rettangolo(xc - r - 0.3 * s, Y1 - 1.5 * s, xc + r + 0.3 * s, Y1 - 1.0 * s, Mat::metallo, 6, sist);
                rettangolo(xc - r - 0.6 * s, Y1 - 1.3 * s, xc + r + 0.6 * s, Y1 - 1.2 * s, Mat::accento, GIALLO, sist);
            }
            break;
        }
        case Tipo::reattore: {
            // schermo (vasca d'acqua) attorno al recipiente a cupola, simbolo giallo
            rettangolo(X0 - 0.4 * s, Y0 + 0.2 * H, X1 + 0.4 * s, Y1, Mat::metallo, 1.6, sist);
            const double xc = (X0 + X1) / 2, r = W / 2;
            cil_v(xc, r * 0.8, Y0 + 0.25 * H, Y1 - 0.3 * s, Mat::metallo, sist);
            for (int y = static_cast<int>(Y0); y < Y0 + 0.25 * H + 1; ++y) {   // cupola
                const double v = (Y0 + 0.25 * H - y) / (0.25 * H);
                const double rr = r * 0.8 * std::sqrt(std::max(0.0, 1 - v * v));
                for (int x = static_cast<int>(xc - rr); x < xc + rr; ++x)
                    metti(x, y, Mat::metallo, 6.5 - 2.5 * (x - xc + rr) / std::max(2 * rr, 1.0), sist);
            }
            for (double x = xc - r; x < xc + r; x += std::max(2.0, 0.5 * s))   // barre di controllo
                linea(x, Y0 - 0.6 * s, x, Y0 + 0.1 * H, Mat::metallo, 5, sist);
            const double ys = Y0 + 0.55 * H, rs = std::max(2.0, 0.45 * s);
            disco(xc, ys, rs, Mat::accento, sist);
            for (int y = static_cast<int>(ys - rs); y <= ys + rs; ++y)
                for (int x = static_cast<int>(xc - rs); x <= xc + rs; ++x) {
                    const double u = x + 0.5 - xc, v = y + 0.5 - ys;
                    if (u * u + v * v > rs * rs) continue;
                    const double a = std::atan2(v, u) * 3 / PI;
                    const bool petalo = std::fmod(a + 6.5, 2.0) < 1.0 && u * u + v * v > 0.15 * rs * rs;
                    metti(x, y, Mat::accento, petalo || u * u + v * v < 0.05 * rs * rs ? GIALLO : ROSSO_SCURO, sist);
                }
            break;
        }
        case Tipo::vapore:
            cil_v((X0 + X1) / 2, W / 2, Y0 + W / 2, Y1, Mat::metallo, sist);
            disco((X0 + X1) / 2, Y0 + W / 2, W / 2, Mat::metallo, sist);
            linea((X0 + X1) / 2, Y0, (X0 + X1) / 2, Y0 - 0.8 * s, Mat::bronzo, 2, sist);
            break;
        case Tipo::turbina: {
            // turbina d'alta pressione (piccola, a prua) e di bassa (grande, a poppa), con le flange
            const double yc = (Y0 + Y1) / 2;
            cil_h(X0, X0 + 0.45 * W, yc, 0.42 * H / 2, 0.62 * H / 2, Mat::metallo, sist);
            cil_h(X0 + 0.5 * W, X1, yc, H / 2, 0.72 * H / 2, Mat::metallo, sist);
            for (double x = X0 + 0.6 * s; x < X1; x += 1.4 * s) linea(x, yc - H / 2, x, yc + H / 2, Mat::metallo, 1.5, sist);
            linea(X0, yc, X1, yc, Mat::metallo, 6.5, sist);
            break;
        }
        case Tipo::riduttore: {
            rettangolo(X0, Y0 + 0.2 * H, X1, Y1, Mat::metallo, 3.2, sist);
            for (int y = static_cast<int>(Y0); y < Y0 + 0.2 * H + 1; ++y)
                for (int x = static_cast<int>(X0); x < X1; ++x) {
                    const double u = (x + 0.5 - (X0 + X1) / 2) / (W / 2), v = (Y0 + 0.2 * H - y) / (0.2 * H);
                    if (u * u + v * v <= 1) metti(x, y, Mat::metallo, 5.5 - 2 * u, sist);
                }
            // ingranaggi
            for (const double f : {0.32, 0.7}) {
                const double xc = X0 + f * W, yc = Y0 + 0.55 * H, r = std::min(W, H) * (f < 0.5 ? 0.22 : 0.3);
                for (int k = 0; k < 64; ++k) {
                    const double a = k * 2 * PI / 64, rr = r * (k % 4 < 2 ? 1.0 : 0.88);
                    metti(static_cast<int>(xc + rr * std::cos(a)), static_cast<int>(yc + rr * std::sin(a)), Mat::metallo, 6.5, sist);
                }
                metti(static_cast<int>(xc), static_cast<int>(yc), Mat::metallo, 7, sist);
            }
            break;
        }
        case Tipo::condensatore:
            rettangolo(X0, Y0, X1, Y1, Mat::metallo, 2.4, sist);
            for (double y = Y0 + 1; y < Y1; y += 2) linea(X0 + 1, y, X1 - 1, y, Mat::metallo, 4, sist);
            linea(X0, Y0, X1, Y0, Mat::metallo, 6, sist);
            break;
        case Tipo::generatore: {
            const double yc = (Y0 + Y1) / 2;
            cil_h(X0, X1, yc, H / 2 * 0.8, H / 2 * 0.8, Mat::metallo, sist);
            for (double x = X0 + 1; x < X1; x += 2) linea(x, yc - H * 0.4, x, yc + H * 0.4, Mat::metallo, 2, sist);
            rettangolo(X0, Y1 - 0.3 * s, X1, Y1, Mat::metallo, 1.5, sist);
            break;
        }
        case Tipo::asse: {
            const double yc = (Y0 + Y1) / 2;
            cil_h(X0, X1, yc, std::max(0.8, H / 2), std::max(0.8, H / 2), Mat::metallo, sist);
            for (double x = X1 - 1.5 * s; x > X0 + s; x -= 5.0 * s)   // supporti
                rettangolo(x, yc - H, x + 0.5 * s, yc + H, Mat::metallo, 2, sist);
            break;
        }
        case Tipo::bombole: {
            const int n = std::max(1, static_cast<int>(H / std::max(2.0, 0.55 * s)));
            for (int k = 0; k < n; ++k) {
                const double yc = Y0 + H * (k + 0.5) / n, r = H / n / 2 - 0.2;
                cil_h(X0 + r, X1 - r, yc, r, r, Mat::metallo, sist);
                disco(X0 + r, yc, r, Mat::metallo, sist);
                disco(X1 - r, yc, r, Mat::metallo, sist);
            }
            break;
        }
        case Tipo::pompe: {
            const int n = std::max(1, static_cast<int>(W / (1.6 * s)));
            for (int k = 0; k < n; ++k) {
                const double a = X0 + W * k / n, b = X0 + W * (k + 1) / n - 0.3 * s;
                rettangolo(a, Y1 - 0.5 * H, b, Y1, Mat::metallo, 3, sist);
                cil_h(a + 0.1 * (b - a), b - 0.1 * (b - a), Y1 - 0.72 * H, 0.2 * H, 0.2 * H, Mat::metallo, sist);
                linea(b - 2, Y1 - 0.5 * H, b - 2, Y0, Mat::bronzo, 2, sist);
                linea(a + 1, Y0 + 1, b - 2, Y0 + 1, Mat::bronzo, 2, sist);
            }
            break;
        }
        case Tipo::diesel:
            rettangolo(X0, Y0 + 0.3 * H, X1, Y1, Mat::metallo, 3.4, sist);
            for (double x = X0 + 0.3 * s; x < X1 - 0.3 * s; x += 0.7 * s) rettangolo(x, Y0 + 0.1 * H, x + 0.45 * s, Y0 + 0.32 * H, Mat::metallo, 5, sist);
            linea(X1 - 0.6 * s, Y0, X1 - 0.6 * s, Y0 - 1.5 * s, Mat::bronzo, 2, sist);
            rettangolo(X0, Y1 - 1, X1, Y1, Mat::metallo, 1, sist);
            break;
        case Tipo::batterie:
            for (double x = X0; x < X1 - 1; x += std::max(2.0, 0.45 * s)) {
                rettangolo(x, Y0, x + std::max(1.0, 0.45 * s - 1), Y1, Mat::metallo, 2.2, sist);
                metti(static_cast<int>(x), static_cast<int>(Y0), Mat::accento, ROSSO, sist);
            }
            break;
        case Tipo::quadri: {
            const int n = std::max(1, static_cast<int>(std::lround(W / (1.0 * s))));
            for (int k = 0; k < n; ++k) {
                const double a = X0 + W * k / n, b = X0 + W * (k + 1) / n - 1;
                rettangolo(a, Y0, b, Y1, Mat::metallo, 3.8, sist);
                for (double y = Y0 + 0.2 * H; y < Y0 + 0.6 * H; y += 3)
                    for (double x = a + 1; x < b - 1; x += 3) metti(static_cast<int>(x), static_cast<int>(y), Mat::metallo, 7, sist);
                metti(static_cast<int>(a + 1), static_cast<int>(Y0 + 0.75 * H), Mat::accento, k & 1 ? VERDE : AMBRA, sist);
            }
            break;
        }
        case Tipo::capsula: {
            // capsula di salvataggio nella vela (Alfa)
            const double xc = (X0 + X1) / 2, yc = (Y0 + Y1) / 2;
            for (int y = static_cast<int>(Y0); y < Y1; ++y)
                for (int x = static_cast<int>(X0); x < X1; ++x) {
                    const double u = (x + 0.5 - xc) / (W / 2), v = (y + 0.5 - yc) / (H / 2);
                    if (std::pow(std::fabs(u), 4) + std::pow(std::fabs(v), 4) > 1) continue;
                    metti(x, y, Mat::accento, v < -0.5 ? BIANCO_CALDO : AMBRA, sist, true);
                }
            rettangolo(xc - 0.4 * s, Y1 - 1, xc + 0.4 * s, Y1 + 0.6 * s, Mat::metallo, 5, sist, true);
            break;
        }
        case Tipo::scala:
            linea(X0, Y0, X0, Y1, Mat::metallo, 6, sist);
            linea(X1, Y0, X1, Y1, Mat::metallo, 4, sist);
            for (double y = Y0 + 2; y < Y1; y += std::max(2.0, 0.35 * s)) linea(X0, y, X1, y, Mat::metallo, 5, sist);
            break;
        case Tipo::rullo: {
            const double xc = (X0 + X1) / 2, yc = (Y0 + Y1) / 2, r = std::min(W, H) / 2;
            disco(xc, yc, r, Mat::metallo, sist, 1.5);
            for (int k = 0; k < 120; ++k) {   // cavo avvolto a spirale
                const double a = k * 0.3, rr = r * (0.25 + 0.7 * k / 120.0);
                metti(static_cast<int>(xc + rr * std::cos(a)), static_cast<int>(yc + rr * std::sin(a)), Mat::accento, GIALLO, sist);
            }
            break;
        }
        case Tipo::tunnel:
            // galleria schermata sopra il reattore: pavimento, oblo' di piombo
            rettangolo(X0, Y1 - std::max(2.0, 0.3 * s), X1, Y1, Mat::metallo, 4.5, sist);
            for (double x = X0 + 1.2 * s; x < X1 - 1.2 * s; x += 2.4 * s) {
                rettangolo(x, Y1 + 1, x + 0.9 * s, Y1 + 0.7 * s, Mat::accento, AMBRA, sist);
                rettangolo(x + 1, Y1 + 2, x + 0.9 * s - 1, Y1 + 0.7 * s - 1, Mat::accento, BIANCO_CALDO, sist);
            }
            break;
    }
}

// --- composizione con i danni ---
void Spaccato::disegna_vista(Tela& t, Rettangolo r, const StatoSpaccato& s, const Vista& v) const {
    auto& d = *d_;
    if (r.w <= 0 || r.h <= 0) return;
    const auto& cc = d.prepara(r.w, r.h, v, s.rastrelliera);
    const auto& base = cc.base;
    const auto m = d.mappa(r.w, r.h, v);
    const double tm = s.tempo_s;
    const auto& celle = d.scafo.celle;
    const std::size_t nc = celle.size();

    // stato per cella: acqua (quota della superficie), fumo, fuoco, sventrato, imploso; aria nelle casse
    struct Cella {
        double z_acqua = -1e9, fumo = 0, fuoco = 0, zc = 0, r = 0, z_fumo = 1e9, ossigeno = 0.21, xc = 0;
        bool distrutto = false, imploso = false;
        bool squarciata = false;
        double zfondo = 0, zcima = 0;
    };
    std::vector<Cella> st(nc);
    for (std::size_t i = 0; i < nc; ++i) {
        const auto& c = celle[i];
        auto& q = st[i];
        q.xc = 0.5 * (c.x0 + c.x1);
        d.resistente(q.xc, q.r, q.zc);
        if (q.r <= 0) {   // casse fuori dallo scafo resistente: altezza del profilo a meta' cella
            q.zc = 0.5 * (d.zmin + 2.0);
            q.r = 4;
        }
        q.zfondo = q.zc - q.r;
        q.zcima = q.zc + q.r;
        const int ci = d.comp_di_cella[i], ki = d.cassa_di_cella[i];
        if (ci >= 0 && s.compartimenti && ci < static_cast<int>(s.compartimenti->size())) {
            const auto& cb = (*s.compartimenti)[static_cast<std::size_t>(ci)];
            const double f = cb.volume > 0 ? std::clamp(cb.acqua / cb.volume, 0.0, 1.0) : 0.0;
            if (f > 0.001) {
                // altezza del segmento di cerchio con quella frazione d'area
                double lo = 0, hi = 2 * q.r;
                for (int k = 0; k < 40; ++k) {
                    const double hh = 0.5 * (lo + hi), a = q.r - hh;
                    const double area = q.r * q.r * std::acos(std::clamp(a / q.r, -1.0, 1.0)) - a * std::sqrt(std::max(0.0, 2 * q.r * hh - hh * hh));
                    (area / (PI * q.r * q.r) < f ? lo : hi) = hh;
                }
                q.z_acqua = q.zfondo + 0.5 * (lo + hi);
            }
            q.fumo = std::clamp(cb.fumo, 0.0, 1.0);
            q.fuoco = std::clamp(cb.fuoco, 0.0, 1.0);
            q.distrutto = cb.distrutto;
            q.imploso = cb.imploso;
            q.ossigeno = cb.ossigeno;
            if (q.imploso) q.z_acqua = q.zcima + 1;
            q.z_fumo = q.zcima - q.fumo * 0.92 * (q.zcima - q.zfondo);
        } else if (ki >= 0) {
            // casse: piene d'acqua in immersione, l'aria (Nm^3) occupa la parte alta
            double aria = 0;
            if (s.casse && ki < static_cast<int>(s.casse->size())) {
                const auto& k = (*s.casse)[static_cast<std::size_t>(ki)];
                aria = k.volume > 0 ? std::clamp(k.aria_Nm3 / (1.0 + std::max(0.0, s.quota_m) / 10.06) / k.volume, 0.0, 1.0) : 0;
                q.squarciata = k.squarciata;
            }
            q.z_acqua = d.zmax;   // fissata per colonna sotto
            q.fumo = aria;        // (riuso: frazione d'aria)
        }
    }
    // quota dell'acqua nelle casse per colonna: dal fondo della sagoma, per la frazione d'acqua
    std::vector<double> top_col(static_cast<std::size_t>(r.w), -1e9), bot_col(static_cast<std::size_t>(r.w), 1e9);
    for (int x = 0; x < r.w; ++x)
        for (int y = 0; y < r.h; ++y) {
            const auto& p = base[static_cast<std::size_t>(y) * r.w + x];
            if (p.mat != Mat::cassa) continue;
            const double z = m.zm(y + 0.5);
            top_col[static_cast<std::size_t>(x)] = std::max(top_col[static_cast<std::size_t>(x)], z);
            bot_col[static_cast<std::size_t>(x)] = std::min(bot_col[static_cast<std::size_t>(x)], z);
        }
    const double ta = std::tan(std::clamp(s.assetto_rad, -0.6, 0.6));
    // focolai: posizione lungo il compartimento, pavimento su cui bruciano e soffitto sopra
    std::vector<std::vector<std::array<double, 3>>> fiamme(nc);
    for (std::size_t i = 0; i < nc; ++i) {
        const auto& q = st[i];
        if (q.fuoco <= 0 || q.distrutto) continue;
        const auto& c = celle[i];
        const auto pv = d.ponti(static_cast<int>(i));
        double pav = pv.empty() ? q.zfondo + 0.6 : pv.front();
        for (const double z : pv)
            if (z > q.z_acqua + 0.1) { pav = z; break; }
        if (q.z_acqua > pav) pav = q.z_acqua;   // fuoco sull'acqua (gasolio)
        double sof = q.zcima;
        for (const double z : pv)
            if (z > pav + 0.5) { sof = z; break; }
        const int lingue = 2 + static_cast<int>(q.fuoco * 7);
        for (int k = 0; k < lingue; ++k)
            fiamme[i].push_back({c.x0 + (c.x1 - c.x0) * (0.12 + 0.76 * caso(static_cast<int>(i), k, 21)), pav, sof});
    }
    // bagliore del fuoco: piu' forte vicino ai focolai, raggio che cresce col fuoco
    auto bagliore = [&](int cella, double xm, double zm) {
        const auto& q = st[static_cast<std::size_t>(cella)];
        double b = 0;
        const double R = 2.0 + 4.5 * q.fuoco;
        for (const auto& f : fiamme[static_cast<std::size_t>(cella)]) {
            const double dx = xm - f[0], dz = (zm - f[1]) * 1.6;
            b = std::max(b, 1.0 - std::sqrt(dx * dx + dz * dz) / R);
        }
        return std::clamp(b, 0.0, 1.0) * (0.55 + 0.45 * q.fuoco);
    };
    auto guasto = [&](int sist) {
        if (sist < 0 || !s.sistemi || sist >= static_cast<int>(s.sistemi->size())) return 0.0;
        return 1.0 - std::clamp((*s.sistemi)[static_cast<std::size_t>(sist)].efficienza, 0.0, 1.0);
    };
    const bool lampo = std::fmod(tm, 1.0) < 0.55;
    // raggi di luce nel mare: una funzione di x + 0,35 y, calcolata una volta per fotogramma
    std::vector<double> raggi(static_cast<std::size_t>(r.w + r.h));
    for (std::size_t i = 0; i < raggi.size(); ++i) raggi[i] = rumore(static_cast<double>(i) * 0.035 + 0.15 * tm, 0.5, 7);

    // per colonna: onda del pelo dell'acqua e pulsazione del bagliore
    std::vector<double> onda(static_cast<std::size_t>(r.w)), pulsa(static_cast<std::size_t>(r.w));
    for (int x = 0; x < r.w; ++x) {
        const double xm = m.xm(x + 0.5);
        onda[static_cast<std::size_t>(x)] = 0.06 * std::sin(xm * 2.1 + tm * 3.3);
        pulsa[static_cast<std::size_t>(x)] = 0.85 + 0.15 * rumore(tm * 5.0, xm * 0.3, 5);
    }
    // bagliore su una griglia di 4 pixel (si legge il punto piu' vicino, la retinatura copre il resto)
    const int gw = r.w / 4 + 1, gh = r.h / 4 + 1;
    std::vector<float> luce_fuoco;
    bool qualche_fuoco = false;
    for (const auto& f : fiamme) qualche_fuoco |= !f.empty();
    if (qualche_fuoco) {
        luce_fuoco.assign(static_cast<std::size_t>(gw) * gh, 0.0f);
        for (int gy = 0; gy < gh; ++gy)
            for (int gx = 0; gx < gw; ++gx) {
                const int x = std::min(gx * 4 + 2, r.w - 1), y = std::min(gy * 4 + 2, r.h - 1);
                const auto& p = base[static_cast<std::size_t>(y) * r.w + x];
                if (p.cella < 0 || fiamme[static_cast<std::size_t>(p.cella)].empty()) continue;
                luce_fuoco[static_cast<std::size_t>(gy) * gw + gx] = static_cast<float>(bagliore(p.cella, m.xm(x + 0.5), m.zm(y + 0.5)));
            }
    }
    for (int y = 0; y < r.h; ++y) {
        const int ty = r.y + y;
        if (ty < 0 || ty >= t.altezza()) continue;
        for (int x = 0; x < r.w; ++x) {
            const int tx = r.x + x;
            if (tx < 0 || tx >= t.larghezza()) continue;
            const auto& p = base[static_cast<std::size_t>(y) * r.w + x];
            int col = 0;
            switch (p.mat) {
                case Mat::mare: {
                    // raggi di luce dall'alto che ondeggiano
                    const double raggio = raggi[static_cast<std::size_t>(x + static_cast<int>(0.35 * y))];
                    const double tt = p.tono + (raggio > 0.68 ? 1.0 : 0.0) * (1.0 - y / static_cast<double>(r.h));
                    col = MARE + std::clamp(static_cast<int>(tt), 0, 7);
                    if (caso(x, y, 11) > 0.9985) col = MARE + 7;   // particelle in sospensione
                    break;
                }
                case Mat::scafo: col = SCAFO + p.tono; break;
                case Mat::taglio: col = TAGLIO + std::min<int>(p.tono, 3); break;
                case Mat::vano: col = VANO + p.tono; break;
                case Mat::metallo: col = METALLO + p.tono; break;
                case Mat::parete: col = METALLO + p.tono; break;
                case Mat::bronzo: col = BRONZO + std::min<int>(p.tono, 3); break;
                case Mat::accento: col = ACCENTO + std::min<int>(p.tono, 7); break;
                case Mat::cassa: col = CASSA + std::min<int>(p.tono, 3); break;
                case Mat::allagabile: col = SOMMERSO + std::min<int>(p.tono, 3); break;
            }
            const double xm = m.xm(x + 0.5), zm = m.zm(y + 0.5);
            // sistemi guasti: il contorno del macchinario in rosso, che pulsa; sopra l'acqua delle casse e dei locali
            // allagati (la sfera del sonar sta nella cassa di prua, sempre piena)
            int allarme = -1;
            const double g = guasto(p.sist);
            if (g > 0.001 && p.mat != Mat::mare && p.mat != Mat::vano) {
                auto altro = [&](int xx, int yy) {
                    return xx < 0 || yy < 0 || xx >= r.w || yy >= r.h || base[static_cast<std::size_t>(yy) * r.w + xx].sist != p.sist;
                };
                if (altro(x - 1, y) || altro(x + 1, y) || altro(x, y - 1) || altro(x, y + 1))
                    col = allarme = ALLARME + (lampo ? 3 : (g > 0.5 ? 2 : 1));
            }
            if (p.interno && p.cella >= 0) {
                const auto& q = st[static_cast<std::size_t>(p.cella)];
                const bool cassa = p.mat == Mat::cassa || d.cassa_di_cella[static_cast<std::size_t>(p.cella)] >= 0;
                if (cassa) {
                    // acqua e aria nella cassa; in esaurimento le bolle salgono dagli sfoghi
                    const double tc = top_col[static_cast<std::size_t>(x)], bc = bot_col[static_cast<std::size_t>(x)];
                    const double livello = tc - q.fumo * (tc - bc);
                    if (p.mat != Mat::cassa) {
                        if (zm < livello && p.mat != Mat::allagabile) col = SOMMERSO + std::clamp(static_cast<int>(p.tono) + 1, 0, 7);
                    } else if (zm < livello) {
                        const double prof = (livello - zm) * m.s;
                        col = ACQUA + std::clamp(retina(3.2 - prof / 10.0, x, y, 7), 1, 4);
                        if (prof < 1.0) col = ACQUA + 6;
                        if (s.esaurimento) {
                            const double b = rumore(x * 0.5, y * 0.5 + tm * 6.0, 3);
                            if (b > 0.83) col = BOLLE + std::min(3, static_cast<int>((b - 0.83) * 24));
                        }
                    } else if (q.fumo > 0) {
                        col = CASSA + std::min<int>(p.tono, 3);
                    }
                } else {
                    const double livello = q.z_acqua + (xm - q.xc) * -ta + onda[static_cast<std::size_t>(x)];
                    // fuoco: bagliore arancio che si allarga col fuoco
                    if (q.fuoco > 0 && (p.mat == Mat::vano || p.mat == Mat::metallo || p.mat == Mat::parete)) {
                        const double b = luce_fuoco[static_cast<std::size_t>(y / 4) * gw + x / 4] * pulsa[static_cast<std::size_t>(x)];
                        if (bayer(x, y) < b * 1.4)
                            col = BAGLIORE + std::clamp(static_cast<int>(p.tono * 0.8 + 3.0 * b), 0, 7);
                    }
                    // reattore fermo: luci d'emergenza
                    if (s.scram && celle[static_cast<std::size_t>(p.cella)].uso == "reattore" && p.mat == Mat::vano)
                        col = (p.tono >= 7 && std::fmod(tm, 1.6) < 0.8) ? ACCENTO + ROSSO : VANO + std::max(0, p.tono - 3);
                    // sventrato: bruciato e rottami
                    if (q.distrutto && p.mat != Mat::taglio) {
                        const double n = rumore(xm * 1.3, zm * 1.3, 9);
                        col = n > 0.72 ? MARE + 1 : BRUCIATO + std::clamp(static_cast<int>(n * 6.0), 0, 4);
                    }
                    if (zm < livello) {
                        const double prof = (livello - zm) * m.s;
                        if (p.mat == Mat::vano || (q.distrutto && p.mat != Mat::taglio)) {
                            const double caust = rumore(xm * 2.2 + tm * 0.8, zm * 3.0 - tm * 0.5, 4);
                            col = ACQUA + std::clamp(retina(4.6 - prof / 7.0 + (caust > 0.72 ? 1.2 : 0.0), x, y, 7), 1, 6);
                        } else {
                            col = SOMMERSO + std::clamp(static_cast<int>(p.tono * 0.9), 0, 7);
                        }
                        if (prof < 1.0) col = ACQUA + (std::fmod(xm * 1.7 + tm * 2.0, 1.0) < 0.5 ? 7 : 6);   // pelo dell'acqua
                        if (q.imploso && p.mat != Mat::vano) col = ALLARME + std::min(3, p.tono / 3);
                    } else if (q.fumo > 0.01 && zm > q.z_fumo - 0.6) {
                        // fumo dal soffitto: denso in alto, a volute che si muovono
                        const double n = rumore(xm * 0.55 + tm * 0.35, zm * 0.9 - tm * 0.15, 2);
                        const double dens = std::clamp((zm - q.z_fumo) / 1.6 + 0.35, 0.0, 1.0) * (0.55 + 0.6 * n) * (0.4 + 0.6 * q.fumo);
                        if (bayer(x, y) < dens) {
                            int tf = static_cast<int>(1 + 5 * n);
                            col = q.fuoco > 0.05 && n > 0.6 ? BAGLIORE + std::min(3, tf - 2) : FUMO + std::clamp(tf, 0, 7);
                        }
                    }
                }
            }
            if (allarme >= 0 && !(p.cella >= 0 && st[static_cast<std::size_t>(p.cella)].distrutto)) col = allarme;
            t.riga(ty)[tx] = static_cast<std::uint8_t>(col);
        }
    }

    auto metti = [&](double x, double y, int col) {
        const int ix = static_cast<int>(std::floor(x)), iy = static_cast<int>(std::floor(y));
        if (ix < 0 || iy < 0 || ix >= r.w || iy >= r.h) return;
        const auto& rr = t.ritaglio();
        const int tx = r.x + ix, ty = r.y + iy;
        if (tx < rr.x || ty < rr.y || tx >= rr.x + rr.w || ty >= rr.y + rr.h) return;
        t.riga(ty)[tx] = static_cast<std::uint8_t>(col);
    };
    auto interno = [&](double x, double y, int cella) {
        const int ix = static_cast<int>(std::floor(x)), iy = static_cast<int>(std::floor(y));
        if (ix < 0 || iy < 0 || ix >= r.w || iy >= r.h) return false;
        const auto& p = base[static_cast<std::size_t>(iy) * r.w + ix];
        return p.interno && (cella < 0 || p.cella == cella);
    };

    // fiamme: lingue sul pavimento piu' basso sopra l'acqua
    for (std::size_t i = 0; i < nc; ++i) {
        const auto& q = st[i];
        if (q.fuoco <= 0 || q.distrutto) continue;
        for (std::size_t k = 0; k < fiamme[i].size(); ++k) {
            const auto [xc, pav, sof] = fiamme[i][k];
            const double larg = (0.6 + 1.2 * caso(static_cast<int>(i), static_cast<int>(k), 22)) * m.s;
            const double alt = (0.35 + 0.6 * q.fuoco) * (sof - pav) * m.s;
            const double x0 = m.px(xc) - larg / 2, yb = m.py(pav);
            for (int dx = 0; dx < larg; ++dx) {
                const double u = (dx + 0.5) / larg * 2 - 1;
                const double kk = static_cast<double>(k);
                const double h = alt * (1 - u * u) * (0.65 + 0.45 * rumore(tm * 7.0 + kk * 3.1, (x0 + dx) * 0.25, 8));
                for (int dy = 0; dy < h; ++dy) {
                    const double vv = dy / std::max(h, 1.0);
                    const double x = x0 + dx + 0.12 * m.s * std::sin(tm * 9.0 + dy * 0.4 + kk);
                    if (!interno(x, yb - dy, static_cast<int>(i))) continue;
                    const int f = std::clamp(static_cast<int>((1 - vv) * 5.5 + bayer(static_cast<int>(x), dy) - 0.5), 0, 5);
                    metti(x, yb - dy - 1, colori::FUOCO[f]);
                }
            }
        }
    }

    // falle: getti d'acqua dal fondo dello scafo resistente, bolle sott'acqua
    std::vector<double> x_falla(s.falle ? s.falle->size() : 0, 0.0);
    if (s.falle) {
        std::vector<int> per_comp(nc, 0), indice(s.falle->size(), 0);
        for (std::size_t k = 0; k < s.falle->size(); ++k) {
            const int c = (*s.falle)[k].compartimento;
            for (std::size_t i = 0; i < nc; ++i)
                if (d.comp_di_cella[i] == c) indice[k] = per_comp[i]++;
        }
        for (std::size_t k = 0; k < s.falle->size(); ++k) {
            const auto& f = (*s.falle)[k];
            int ic = -1;
            for (std::size_t i = 0; i < nc; ++i)
                if (d.comp_di_cella[i] == f.compartimento) ic = static_cast<int>(i);
            if (ic < 0 || f.area <= 0) continue;
            const auto& c = celle[static_cast<std::size_t>(ic)];
            const auto& q = st[static_cast<std::size_t>(ic)];
            const double xf = c.x0 + (c.x1 - c.x0) * (0.25 + 0.5 * caso(ic, indice[k], 31));
            x_falla[k] = xf;
            double rr, zc;
            d.resistente(xf, rr, zc);
            const double zb = zc - rr;
            const double px = m.px(xf), pyb = m.py(zb);
            const double forza = std::sqrt(f.area);   // m: lato della falla
            // squarcio nello scafo
            const double lato = std::max(2.0, std::min(forza, 1.2) * m.s);
            for (int dy = -3; dy <= static_cast<int>(std::max(3.0, 0.4 * m.s)); ++dy)
                for (int dx = static_cast<int>(-lato); dx <= lato; ++dx)
                    if (std::abs(dx) + std::abs(dy) * 1.6 < lato + 1.5 * caso(dx, dy, 41)) metti(px + dx, pyb + dy, MARE + 1);
            // getto verso l'alto fino a sopra il pelo dell'acqua
            const double alto = std::min(q.zcima, std::max(q.z_acqua, zb) + 0.8 + 3.5 * std::min(forza, 1.0));
            const double hp = (alto - zb) * m.s;
            for (int dy = 0; dy < hp; ++dy) {
                const double z = zb + dy / m.s;
                const double larg = std::max(1.0, 0.5 * lato) * (1 + 0.8 * dy / std::max(hp, 1.0));
                for (int dx = static_cast<int>(-larg); dx <= larg; ++dx) {
                    const double n = rumore((px + dx) * 0.6, dy * 0.5 - tm * 14.0, 12);
                    if (n < 0.45 + 0.25 * std::fabs(dx) / larg) continue;
                    const bool sotto = z < q.z_acqua;
                    metti(px + dx, pyb - dy, sotto ? BOLLE + std::min(3, static_cast<int>(n * 4)) : ACQUA + (n > 0.75 ? 7 : 6));
                }
            }
            // spruzzi che ricadono
            for (int g = 0; g < 18; ++g) {
                const double fase = std::fmod(tm * 1.3 + caso(ic, g, 51), 1.0);
                const double ang = (caso(ic, g, 52) - 0.5) * 2.2;
                const double v0 = (0.4 + 0.6 * caso(ic, g, 53)) * hp * 0.5;
                const double gx = px + std::sin(ang) * v0 * fase * 1.5, gy = pyb - hp + 0.0 - std::cos(ang) * v0 * fase + 40 * fase * fase;
                if (interno(gx, gy, ic) && m.zm(gy) > q.z_acqua) metti(gx, gy, ACQUA + 7);
            }
        }
    }

    // paratie cedute: la lamiera piegata e un varco
    if (s.paratie)
        for (const auto& pa : *s.paratie) {
            if (!pa.ceduta) continue;
            int ia = -1, ib = -1;
            for (std::size_t i = 0; i < nc; ++i) {
                if (d.comp_di_cella[i] == pa.a) ia = static_cast<int>(i);
                if (d.comp_di_cella[i] == pa.b) ib = static_cast<int>(i);
            }
            if (ia < 0 || ib < 0) continue;
            const auto& a = celle[static_cast<std::size_t>(ia)];
            const auto& b = celle[static_cast<std::size_t>(ib)];
            const double xb = std::fabs(a.x1 - b.x0) < 1e-3 ? a.x1 : a.x0;
            double rr, zc;
            d.resistente(xb, rr, zc);
            const double px = m.px(xb);
            for (double z = zc - 0.7 * rr; z < zc + 0.7 * rr; z += 1.0 / m.s) {
                const double py = m.py(z);
                const double n = rumore(z * 1.5, 0.5, 61);
                for (int dx = -2; dx <= 1; ++dx) {
                    const auto& p = base[static_cast<std::size_t>(std::clamp(static_cast<int>(py), 0, r.h - 1)) * r.w +
                                           std::clamp(static_cast<int>(px + dx), 0, r.w - 1)];
                    if (p.mat == Mat::parete) metti(px + dx, py, n > 0.4 ? MARE + 1 : ALLARME + 1);
                }
                // lembi piegati
                if (n > 0.75) metti(px + 2 + 3 * (n - 0.75) * 8, py, METALLO + 5);
            }
        }

    // squadre di controllo danni: figurine sul ponte vicino al lavoro (blu le squadre di falla e di riparazione, come
    // le tute dell'equipaggio del classico; ambra quelle antincendio, con la maschera e la manichetta); le pompe sono
    // una manichetta gialla dalla sentina, con l'acqua che scorre
    if (s.squadre) {
        auto cella_di = [&](int comp) {
            for (std::size_t i = 0; i < nc; ++i)
                if (d.comp_di_cella[i] == comp) return static_cast<int>(i);
            return -1;
        };
        // pavimento piu' alto sotto la quota z, nella cella
        auto pavimento = [&](int i, double z) {
            const auto pv = d.ponti(i);
            double p = pv.empty() ? st[static_cast<std::size_t>(i)].zfondo + 0.4 : pv.front();
            for (const double q : pv)
                if (q <= z + 0.05) p = q;
            return p;
        };
        auto figura = [&](double xm, double zp, SquadraBattello::Compito c, int k, double verso) {
            const double H = std::max(7.0, 1.75 * m.s);
            const double px = m.px(xm), yp = m.py(zp) - 1;
            const bool fuoco = c == SquadraBattello::Compito::spegnere;
            const bool chino = c == SquadraBattello::Compito::tamponare;
            const double h = chino ? 0.72 * H : H;
            const int tuta = ACCENTO + (fuoco ? AMBRA : BLU_SCHERMO), ombra = fuoco ? BRONZO + 2 : ACQUA + 4;
            const double fase = std::sin(tm * 6.0 + k * 1.7);
            const double lw = std::max(1.0, 0.13 * H);   // mezza larghezza del corpo
            // gambe
            for (int dy = 0; dy < 0.42 * h; ++dy) {
                metti(px - lw, yp - dy, ombra);
                metti(px + lw - 1, yp - dy, ombra);
            }
            // busto
            for (int dy = static_cast<int>(0.42 * h); dy < 0.8 * h; ++dy)
                for (int dx = static_cast<int>(-lw - 1); dx <= lw; ++dx) metti(px + dx, yp - dy, dx < 0 ? tuta : ombra);
            // testa (casco o maschera per l'antincendio)
            const double rt = std::max(1.0, 0.12 * H), yt = yp - 0.8 * h - rt;
            for (int dy = static_cast<int>(-rt); dy <= rt; ++dy)
                for (int dx = static_cast<int>(-rt); dx <= rt; ++dx)
                    if (dx * dx + dy * dy <= rt * rt + 0.5)
                        metti(px + dx, yt + dy, fuoco ? (dy >= 0 && dx * verso > 0 ? METALLO + 1 : ACCENTO + GIALLO) : ACCENTO + BIANCO_CALDO);
            // braccia e attrezzi
            const double ys = yp - 0.72 * h;
            if (c == SquadraBattello::Compito::spegnere) {
                const double lunghe = 0.35 * H;
                for (int i = 0; i < lunghe; ++i) metti(px + verso * i, ys + i * 0.15, ACCENTO + GIALLO);   // lancia
                for (int i = 0; i < 14; ++i) {   // getto nebulizzato verso il fuoco
                    const double f = std::fmod(tm * 2.5 + caso(k, i, 91), 1.0);
                    metti(px + verso * (lunghe + f * 1.6 * m.s), ys + (caso(k, i, 92) - 0.5) * f * 0.9 * m.s, BOLLE + 2 + (i & 1));
                }
                for (int i = 0; i < 0.6 * H; ++i) metti(px - verso * i, yp - 1 - 0.3 * std::sin(i * 0.5), ACCENTO + GIALLO);   // manichetta
            } else if (chino) {
                const double yb = ys + (0.25 + 0.15 * fase) * H;   // martella il tappo sulla falla
                for (int i = 0; i < 0.3 * H; ++i) metti(px + verso * i * 0.6, ys + (yb - ys) * i / (0.3 * H), tuta);
                metti(px + verso * 0.2 * H, yb, METALLO + 6);
            } else {
                for (int i = 0; i < 0.3 * H; ++i) metti(px + verso * i * 0.7, ys - i * 0.5, tuta);   // braccio alzato
                if (std::fmod(tm * 3.0 + k, 1.0) < 0.3) metti(px + verso * 0.22 * H, ys - 0.17 * H, ACCENTO + GIALLO);   // scintilla
            }
        };
        std::vector<int> gruppi(nc, 0);
        for (std::size_t g = 0; g < s.squadre->size(); ++g) {
            const auto& l = (*s.squadre)[g];
            const int i = cella_di(l.compartimento);
            if (i < 0) continue;
            const auto& c = celle[static_cast<std::size_t>(i)];
            const auto& q = st[static_cast<std::size_t>(i)];
            if (q.distrutto || q.imploso) continue;
            const int k = gruppi[static_cast<std::size_t>(i)]++;
            if (l.compito == SquadraBattello::Compito::pompare) {
                // manichetta dalla sentina lungo la paratia poppiera, con l'acqua che sale a tratti
                const double xh = c.x0 + 0.5 + 0.3 * k;
                const double z0 = q.zfondo + 0.4, z1 = q.zcima - 0.6;
                for (double z = z0; z < z1; z += 1.0 / m.s) {
                    const bool tratto = std::fmod((z - z0) * m.s * 0.25 - tm * 4.0 + 100.0, 2.0) < 1.0;
                    metti(m.px(xh), m.py(z), tratto ? ACQUA + 7 : ACCENTO + GIALLO);
                }
                continue;
            }
            double xm = 0.5 * (c.x0 + c.x1), zp = 0, verso = 1;
            if (l.compito == SquadraBattello::Compito::tamponare && l.falla >= 0 && l.falla < static_cast<int>(x_falla.size())) {
                xm = x_falla[static_cast<std::size_t>(l.falla)] - 0.7;
                zp = pavimento(i, q.zfondo + 0.5);
            } else if (l.compito == SquadraBattello::Compito::spegnere && !fiamme[static_cast<std::size_t>(i)].empty()) {
                const auto& f = fiamme[static_cast<std::size_t>(i)][static_cast<std::size_t>(k) % fiamme[static_cast<std::size_t>(i)].size()];
                verso = f[0] > 0.5 * (c.x0 + c.x1) ? 1 : -1;
                xm = f[0] - verso * 1.8;
                zp = f[1];
            } else {
                for (const auto& e : d.all.elementi)
                    if (e.sist == l.sistema && 0.5 * (e.x0 + e.x1) >= c.x0 && 0.5 * (e.x0 + e.x1) <= c.x1) {
                        xm = e.x0 - 0.6;
                        zp = pavimento(i, e.z0);
                        break;
                    }
                if (zp == 0) zp = pavimento(i, q.zc);
            }
            const int n = std::clamp(l.numero, 1, 3);
            for (int j = 0; j < n; ++j) figura(xm - verso * j * 0.75, zp, l.compito, k * 3 + j, verso);
        }
    }

    // casse in esaurimento: bolle che escono dagli sfoghi in basso
    if (s.esaurimento)
        for (std::size_t i = 0; i < nc; ++i) {
            if (d.cassa_di_cella[i] < 0) continue;
            const auto& c = celle[i];
            for (int g = 0; g < 40; ++g) {
                const double xg = c.x0 + (c.x1 - c.x0) * caso(static_cast<int>(i), g, 71);
                const double fase = std::fmod(tm * 0.6 + caso(static_cast<int>(i), g, 72), 1.0);
                const double y0 = m.py(d.zmin + 0.3);
                const double gx = m.px(xg) + 3 * std::sin(tm * 4 + g), gy = y0 + 10 - fase * (y0 - m.py(d.zmax));
                const auto ix = static_cast<int>(gx), iy = static_cast<int>(gy);
                if (ix < 0 || iy < 0 || ix >= r.w || iy >= r.h) continue;
                if (base[static_cast<std::size_t>(iy) * r.w + ix].mat != Mat::mare) continue;
                metti(gx, gy, BOLLE + (g % 3 == 0 ? 3 : 1));
                if (g % 4 == 0) metti(gx + 1, gy, BOLLE + 2);
            }
        }

    // casse zavorra squarciate (artiglieria alle estremita', urti): lo squarcio nella parte alta della cassa, da cui
    // l'aria se ne va; in esaurimento esce di li' in bolle che salgono nel mare
    for (std::size_t i = 0; i < nc; ++i) {
        if (d.cassa_di_cella[i] < 0 || !st[i].squarciata) continue;
        const auto& c = celle[i];
        // la colonna piu' vicina al punto scelto che passa davvero dalla cassa (alle estremita' la cella e' anche asse,
        // timoni, sfera del sonar)
        const double xs = c.x0 + (c.x1 - c.x0) * (0.3 + 0.4 * caso(static_cast<int>(i), 0, 91));
        const int a0 = std::max(0, static_cast<int>(m.px(c.x0))), a1 = std::min(r.w - 1, static_cast<int>(m.px(c.x1)));
        int cx = -1;
        for (int k = 0; cx < 0 && k <= a1 - a0; ++k)
            for (const int x : {static_cast<int>(m.px(xs)) - k, static_cast<int>(m.px(xs)) + k})
                if (x >= a0 && x <= a1 && top_col[static_cast<std::size_t>(x)] - bot_col[static_cast<std::size_t>(x)] > 1.0 / m.s) { cx = x; break; }
        if (cx < 0) continue;
        // lo squarcio sulla lamiera esterna in cima alla cassa, aperto sul mare
        const double py = m.py(top_col[static_cast<std::size_t>(cx)]);
        const double lato = std::max(4.0, 1.1 * m.s);
        for (int dy = static_cast<int>(-lato); dy <= lato; ++dy)
            for (int dx = static_cast<int>(-lato * 1.4); dx <= lato * 1.4; ++dx) {
                const double e = std::abs(dx) / 1.4 + std::abs(dy) * 1.2;
                if (e < lato * (0.55 + 0.45 * caso(dx, dy, 92))) metti(cx + dx, py + dy, MARE + 1);
                else if (e < lato * 1.05 && caso(dx, dy, 93) > 0.6) metti(cx + dx, py + dy, SCAFO + 2);   // lamiera strappata
            }
        if (!s.esaurimento) continue;
        for (int g = 0; g < 30; ++g) {
            const double fase = std::fmod(tm * 0.8 + caso(static_cast<int>(i), g, 94), 1.0);
            const double gx = cx + (caso(static_cast<int>(i), g, 95) - 0.5) * lato + 3 * std::sin(tm * 5 + g);
            const double gy = py - fase * (py - m.py(d.zmax + 2.0));
            const auto ix = static_cast<int>(gx), iy = static_cast<int>(gy);
            if (ix < 0 || iy < 0 || ix >= r.w || iy >= r.h) continue;
            if (base[static_cast<std::size_t>(iy) * r.w + ix].mat != Mat::mare && fase > 0.15) continue;
            metti(gx, gy, BOLLE + (g % 3 == 0 ? 3 : 1));
        }
    }

    // compartimenti sventrati: lo scafo aperto verso il mare
    for (std::size_t i = 0; i < nc; ++i) {
        const auto& q = st[i];
        if (!q.distrutto && !q.imploso) continue;
        const auto& c = celle[i];
        for (double x = c.x0 + 0.3; x < c.x1 - 0.3; x += 1.0 / m.s) {
            double rr, zc;
            d.resistente(x, rr, zc);
            if (rr <= 0) continue;
            const double n = rumore(x * 0.8, 0.0, 81);
            if (q.imploso) {   // lo scafo schiacciato verso l'interno
                const double dent = (0.5 + 0.5 * std::sin(x * 1.4)) * 0.35 * rr * m.s;
                for (int k = 0; k < dent; ++k) {
                    metti(m.px(x), m.py(zc + rr) + k, SCAFO + 2);
                    metti(m.px(x), m.py(zc - rr) - k, SCAFO + 2);
                }
            } else if (n > 0.55) {   // squarcio in alto
                const double prof = (n - 0.55) * 4 * m.s;
                for (int k = -2; k < prof; ++k) metti(m.px(x), m.py(zc + rr) + k, MARE + 2);
            }
        }
    }
}

// --- didascalie allineate ai compartimenti ---
namespace {
// testo che sta in `largo` pixel: prima si tolgono le parole in coda, poi si abbrevia col punto
std::string adatta(const Font& f, std::string s, int largo) {
    while (f.larghezza(s, 1) > largo) {
        const auto k = s.find_last_of(" ,:");
        if (k == std::string::npos || k == 0) break;
        if (k <= 4) {   // resterebbe il solo numero («IV REAKT.»): la parola si accorcia col punto, almeno tre lettere
            std::string p = s.substr(k + 1);
            if (!p.empty() && p.back() == '.') p.pop_back();
            while (p.size() > 3 && f.larghezza(s.substr(0, k + 1) + p + ".", 1) > largo) p.pop_back();
            if (f.larghezza(s.substr(0, k + 1) + p + ".", 1) <= largo) return s.substr(0, k + 1) + p + ".";
        }
        s = s.substr(0, k);
    }
    while (s.size() > 2 && f.larghezza(s, 1) > largo) {
        if (s.back() == '.') s.pop_back();
        s.back() = '.';
    }
    return s;
}
}  // namespace

void Spaccato::disegna_didascalie(Tela& t, const Font& f, Rettangolo r, Rettangolo d, const StatoSpaccato& s) const {
    const auto& dd = *d_;
    const auto m = dd.mappa(r.w, r.h, dd.intera());
    const auto& celle = dd.scafo.celle;
    for (std::size_t i = 0; i < celle.size(); ++i) {
        const auto& c = celle[i];
        const int ci = dd.comp_di_cella[i];
        if (ci < 0) continue;
        const int a = r.x + static_cast<int>(std::lround(m.px(c.x0))) + 1, b = r.x + static_cast<int>(std::lround(m.px(c.x1))) - 2;
        if (b - a < 6) continue;
        // parentesi del compartimento
        t.linea_orizzontale(a, b, d.y + 1, METALLO + 3);
        t.linea_verticale(a, d.y, d.y + 3, METALLO + 5);
        t.linea_verticale(b, d.y, d.y + 3, METALLO + 5);
        const CompartimentoBattello* cb = s.compartimenti && ci < static_cast<int>(s.compartimenti->size()) ? &(*s.compartimenti)[static_cast<std::size_t>(ci)] : nullptr;
        const auto it = dd.all.nomi.find(c.nome);
        std::string nome = it != dd.all.nomi.end() ? it->second : c.nome;
        const int largo = b - a - 2;
        nome = adatta(f, nome, largo);
        int y = d.y + 5;
        f.scrivi(t, a + 2, y, nome, cb && (cb->distrutto || cb->imploso) ? ACCENTO + ROSSO : TAGLIO + 2, 1);
        y += 11;
        // barra dell'acqua
        const double pieno = cb && cb->volume > 0 ? std::clamp(cb->acqua / cb->volume, 0.0, 1.0) : 0.0;
        const int bw = std::min(largo - 18, 60);
        t.rettangolo_pieno(a + 2, y, bw, 5, ACQUA + 1);
        t.rettangolo_pieno(a + 2, y, static_cast<int>(std::lround(pieno * bw)), 5, ACQUA + 5);
        t.linea_orizzontale(a + 2, a + 2 + bw - 1, y, ACQUA + 6);
        if (pieno > 0.0) {
            char buf[16];
            std::snprintf(buf, sizeof buf, "%d", static_cast<int>(std::lround(pieno * 100)));
            f.scrivi(t, a + 4 + bw, y - 3, buf, ACQUA + 7, 1);
        }
        y += 8;
        std::string stato;
        int col = TAGLIO + 1;
        if (cb) {
            if (cb->imploso) { stato = "IMPLOSO"; col = ACCENTO + ROSSO; }
            else if (cb->distrutto) { stato = "PERSO"; col = ACCENTO + ROSSO; }
            else if (cb->fuoco > 0) { stato = "FUOCO"; col = ACCENTO + AMBRA; }
            else if (cb->fumo >= 0.95) { stato = "FUMO"; col = FUMO + 7; }
            else if (pieno > 0.5) { stato = "ALLAGATO"; col = ACQUA + 6; }
            if (cb->ossigeno < 0.17 && !cb->distrutto) {
                char o2[24];
                std::snprintf(o2, sizeof o2, "%sO2 %.0f", stato.empty() ? "" : "  ", cb->ossigeno * 100.0);
                stato += o2;
            }
        }
        if (stato.empty()) { stato = "OK"; col = ACCENTO + VERDE; }
        stato = adatta(f, stato, largo);
        f.scrivi(t, a + 2, y, stato, static_cast<std::uint8_t>(col), 1);
    }
}

std::vector<Sistema> sistemi_spaccato(const std::string& nome) {
    if (const auto s = sistema_da_chiave(nome)) return {*s};
    if (nome == "reattore") return {Sistema::propulsione};   // reattore e generatori di vapore stanno con la propulsione
    if (nome == "centrale") return {Sistema::periscopio, Sistema::idroplani};   // sotto la vela: periscopi e posto di manovra
    if (nome == "direzione_tiro") return {Sistema::sonar_tiro};   // la consolle del tiro
    if (nome == "boa_radio" || nome == "filo_galleggiante") return {Sistema::antenna};   // radio e alberi
    return {};
}

void disegna_spaccato_sistema(Tela& t, Rettangolo r, const Spaccato& s, const std::string& sistema, double tempo_s) {
    std::vector<StatoSistema> st(static_cast<std::size_t>(Sistema::numero));
    for (const Sistema x : sistemi_spaccato(sistema)) st[static_cast<std::size_t>(x)].efficienza = 0.3;
    StatoSpaccato ss;
    ss.sistemi = &st;
    ss.tempo_s = tempo_s;
    s.disegna(t, r, ss);
}

}  // namespace m688
