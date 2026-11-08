#pragma once
// Meteo nella vista del periscopio dell'Enhanced (docs/15_meteo_e_mare.md, M3; dossier 21 §9 e nota D §B2, B-bis):
// nubi sul loro piano alla quota del plafond (in prospettiva, portate dal vento), cumulonembi con la base scura e le
// cortine di pioggia, foschia e nebbia per distanza (Koschmieder), fumo di mare come fascia bassa che sale, pioggia e
// neve davanti all'occhio, lampi e fulmini (3-4 colpi a ~57 ms, canale solo da vicino, bagliore da lontano, lampo della
// palette drammatico di notte e leggero di giorno). Con MeteoVista::attivo falso la vista resta quella di oggi.

#include <cstdint>
#include <vector>

#include "grafica/palette.hpp"
#include "grafica/tela.hpp"

namespace m688 {

struct CieloNotte;   // nucleo/sagoma_notte.hpp

enum class NebbiaVista : std::uint8_t { nessuna, avvezione, fumo_di_mare, radiazione };

// Una scarica: istante del primo colpo, dove (rilevamento vero e distanza dall'occhio), quanti colpi
struct LampoVista {
    double t = 0.0;
    double rilevamento_gradi = 0.0;
    double distanza_m = 10000.0;
    int colpi = 3;
    bool nube_mare = true;          // falso: dentro la nube (solo bagliore)
    std::uint32_t seme = 1;         // forma del canale
};

struct MeteoVista {
    bool attivo = false;
    double nubi = 0.0;              // copertura 0-1
    double plafond_m = 1500.0;      // base delle nubi
    double temporale = 0.0;         // 0-1: cumulonembi (base scura, cortine di pioggia)
    double pioggia_mm_h = 0.0, neve_mm_h = 0.0;
    double vento_m_s = 5.0, vento_da_gradi = 270.0;
    double mare_da_gradi = 270.0, tp_s = 0.0;   // mare per il moto delle navi; tp_s <= 0: dal mare formato
    NebbiaVista nebbia = NebbiaVista::nessuna;
    double strato_nebbia_m = 0.0;   // fumo di mare: spessore dello strato
    std::vector<LampoVista> lampi;
};

// Geometria della vista per il meteo: centro, pixel per radiante, linea di mira, area, occhio
struct GeometriaVista {
    int cx = 320, cy = 180;
    double ppr = 1000.0;
    double elevazione_rad = 0.0;    // linea di mira
    double rilevamento_gradi = 0.0;
    Rettangolo area{0, 0, 640, 360};
    double quota_occhio_m = 1.5;
    double elevazione_riga(int y) const;          // elevazione apparente della riga (radianti)
    double azimut_colonna(int x) const;           // rilevamento vero della colonna (gradi)
    double riga_elevazione(double el) const;      // riga di un'elevazione
};

namespace cielo {

inline constexpr int FOSCHIA = 157;   // grigio chiaro degli sprite (preso dalla luce ambiente): foschia, nebbia, nubi chiare
inline constexpr int NUBE_SCURA = 146, NUBE = 150, PIOGGIA = 154, NEVE = 158;
inline constexpr double COLPO_S = 0.03;           // durata di un colpo (~2 fotogrammi a 70 Hz)
inline constexpr double INTERVALLO_COLPI_S = 0.057;

// Nubi sul piano del plafond sopra il cielo gia' disegnato (copertura, base scura dei cumulonembi, cortine di pioggia)
void nubi(Tela& t, const GeometriaVista& q, const MeteoVista& m, double tempo_s);
// Foschia e nebbia sullo sfondo (mare e cielo vicino all'orizzonte), verso il grigio della foschia, per distanza
void foschia(Tela& t, const GeometriaVista& q, const MeteoVista& m, double visibilita_m);
// Fulmini: il canale (solo scariche nube-mare entro LAMPI_CANALE_M, durante i colpi) e il bagliore delle lontane
void fulmini(Tela& t, const GeometriaVista& q, const MeteoVista& m, double tempo_s);
// Fumo di mare: fascia bassa sopra l'orizzonte con colonne che salgono (va disegnata dopo le navi: nasconde gli scafi)
void fumo_di_mare(Tela& t, const GeometriaVista& q, const MeteoVista& m, double tempo_s);
// Pioggia e neve davanti all'occhio (ultime, sopra tutto); la pioggia pende col vento di traverso
void precipitazione(Tela& t, const GeometriaVista& q, const MeteoVista& m, double tempo_s);
// Luce del lampo 0-1 all'istante (per Palette::schiarita sugli INDICI_VISTA): somma dei colpi, piu' forte da vicino; di
// giorno al 25% (nota D B-bis)
double lampo(const MeteoVista& m, double tempo_s, double luce);
// Luce ambiente sotto le nubi: il coperto toglie fino a circa meta' della luce, il cumulonembo di piu' (STIMA)
double luce_sotto_le_nubi(double luce, const MeteoVista& m);
// La luna nella vista dell'Enhanced, dal cielo con cui vede l'IA (CieloNotte: l'illuminamento con la fase, l'altezza e
// le nubi; la Luna col suo azimut e la sua altezza). luce_della_notte: la luce della vista (0-1, come luce_ambiente) che
// danno i lux del cielo: quella del crepuscolo con lo stesso illuminamento o, di notte, quella dell'occhio adattato al
// buio (in scala logaritmica, 0,22 con la luna piena alta, niente con le sole stelle); luna: il disco con la sua fase
// (la falce verso luna_angolo_lembo, la parte buia resta cielo; prima delle nubi, che lo
// coprono) e di notte un alone rado; scia_lunare: il riflesso sul mare, sotto la luna, largo con le onde (dopo il mare,
// la banchisa e la costa, prima delle navi)
double luce_della_notte(double lux);
void luna(Tela& t, const GeometriaVista& q, const CieloNotte& c, double luce);
void scia_lunare(Tela& t, const GeometriaVista& q, const CieloNotte& c, double luce, double altezza_onde_m, double tempo_s);
inline constexpr double LAMPI_CANALE_M = 15000.0, LAMPI_BAGLIORE_M = 150000.0;

}  // namespace cielo

}  // namespace m688
