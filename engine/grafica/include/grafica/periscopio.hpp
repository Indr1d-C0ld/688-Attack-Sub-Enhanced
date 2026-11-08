#pragma once
// Vista al periscopio: cielo, mare, orizzonte con la curvatura terrestre, navi in scala corretta per
// distanza e ingrandimento, reticolo e scala dei rilevamenti. Le navi oltre l'orizzonte geometrico
// dell'occhio mostrano solo la parte che sporge sopra la linea dell'orizzonte.

#include <array>
#include <future>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "grafica/cielo.hpp"
#include "grafica/effetti.hpp"
#include "grafica/font.hpp"
#include "grafica/modello.hpp"
#include "grafica/oculare.hpp"
#include "grafica/palette.hpp"
#include "grafica/sprite.hpp"
#include "grafica/tela.hpp"
#include "nucleo/classi.hpp"
#include "nucleo/luci_navigazione.hpp"
#include "nucleo/sagoma_notte.hpp"
#include "nucleo/mondo.hpp"

namespace m688 {

struct VistaPeriscopio {
    double rilevamento = 0.0;        // rilevamento vero del centro della vista, gradi
    bool alto_ingrandimento = false;
    double quota_occhio_m = 1.5;     // altezza della lente sull'acqua a quota periscopica (stima di gioco)
    bool mostra_verita = false;      // diagnosi: classe e distanza dei contatti nella vista
    double luce = 1.0;               // luce ambiente (0 notte, 1 giorno): fumo illuminato dal fuoco di notte
    double altezza_onde_m = 1.0;     // altezza significativa: le creste nascondono la base di navi e colonne
    double visibilita_m = 30000.0;   // portata visiva meteorologica (Koschmieder): la foschia schiarisce le cose lontane
    double tempo_s = 0.0;            // per il moto delle onde
    bool reticolo = true;            // croce, tacche e scala; spento: la scena vista da fuori (grafica/cartolina). Con l'oculare non conta
    // Enhanced (docs/14, P3): oculare tondo, campo e reticolo del modello, elevazione, modi notte e TV, stadimetro.
    // Spento (predefinito) la vista e' quella del classico e `alto_ingrandimento` sceglie il campo.
    ParametriOculare oculare;
    // Enhanced (docs/15, M3): nubi, foschia e nebbia, fumo di mare, pioggia e neve, lampi e fulmini, moto delle navi
    // col mare, elicotteri in hover col vento. Spento (predefinito) la vista e' quella di oggi.
    MeteoVista meteo;
    // Enhanced (docs/27): le luci di navigazione. Le configurazioni di luci.toml e lo stato delle luci per id di nave
    // (luci::stato_predefinito, poi la chiave dello scenario e i danni). Senza configurazioni (predefinito) o senza lo
    // stato di una nave, nessuna luce: il classico resta com'e'
    struct LuciVista {
        const std::map<std::string, luci::Configurazione>* configurazioni = nullptr;
        std::map<int, luci::Stato> stati;
    } luci;
    // Enhanced: la banchisa attorno alla testa (oculare::banchisa; la riempie la stazione dal campo del ghiaccio della
    // missione). Spenta (predefinita) nessun ghiaccio: il classico resta com'e'
    oculare::GhiaccioVista ghiaccio;
    // la terraferma (Enhanced; vuota nel classico): coste, isole e fiordi del fondale della missione
    oculare::CostaVista costa;
    // Enhanced: i fari della costa (nucleo/fari, C9 §2.15), gia' nel piano della missione col loro lampo di adesso;
    // vuoti (predefinito) nel classico
    struct FaroVista {
        Vec2 pos;
        double quota_m = 20.0, portata_m = 0.0;
        luci::Colore colore = luci::Colore::bianco;
        bool acceso = true;
        int seme = 0;   // per lo scintillio del riflesso
    };
    std::vector<FaroVista> fari;
    // Enhanced: il cielo della notte che vede l'IA (Missione::cielo_notte_in: la Luna con fase, altezza e azimut,
    // l'illuminamento), per disegnarci sopra la Luna. Spento (predefinito) il classico resta com'e'
    bool cielo_lunare = false;
    CieloNotte cielo;
};

// La luce che arriva da un fanale, in multipli della soglia di visibilita' (sotto 1 non si vede): luci::rapporto_soglia
// (nucleo/portata_luci, del disassemblatore), la stessa della vedetta dell'IA. Legge di Allard con la K dell'Annesso I
// (0,8 per miglio: la portata nominale vale con circa 32 km di visibilita'), il quadrato della distanza, la foschia di
// Koschmieder; la soglia sale col giorno (1 + 3000 g^2), e di giorno i fanali spariscono
// `guadagno`: l'ottica (luci::guadagno_ottico; ParametriOculare::guadagno_luci), 1 a occhio nudo
double luce_fanale(double portata_m, double distanza_m, double visibilita_m, double luce_ambiente, double guadagno = 1.0);

// Periscopio tipo 18 (688): 1,5x con campo di 32 gradi e 6x con campo di 8 gradi (stima di gioco).
inline constexpr double CAMPO_BASSO_GRADI = 32.0;
inline constexpr double CAMPO_ALTO_GRADI = 8.0;
inline constexpr int ALTEZZA_VISTA = 360;   // righe della tela occupate dalla vista; sotto, il pannello
// Indici di palette della vista (cielo, mare, grigi degli sprite, fango e fuliggine degli effetti) che la luce
// ambiente scurisce; l'interfaccia (0-15, 80-95) e il fuoco (175-178, 190-194) restano luminosi anche di notte.
inline const std::vector<std::pair<int, int>> INDICI_VISTA = {{16, 79}, {96, 143}, {144, 158}, {196, 198}, {199, 247}};

struct StatoMeteo;
struct Lampo;

// Dal meteo della missione (nucleo/meteo.hpp, M1) alla vista (M3): accende VistaPeriscopio::meteo e riempie onde,
// visibilita', mare e vento dell'oculare; i lampi diventano rilevamento e distanza dall'occhio. Legge solo i campi.
void applica_meteo(VistaPeriscopio& v, const StatoMeteo& s, const std::vector<Lampo>& lampi, Vec2 occhio);

class Periscopio {
public:
    Periscopio(const Font& font, const std::map<std::string, ClasseNave>& classi,
               const std::map<std::string, InsiemeSprite>& sprite)
        : font_(font), classi_(classi), sprite_(sprite) {}

    // Con `effetti` anche fuoco, fumo, spruzzi e chiazze, ordinati per distanza insieme alle navi.
    void disegna(Tela& t, const Mondo& mondo, const Nave& proprio, const VistaPeriscopio& v,
                 const Effetti* effetti = nullptr) const;

    // Modelli 3D per le navi danneggiate (Nave::posa.danneggiata): il loro sprite si genera al volo dal modello,
    // alla risoluzione che serve per la distanza (al massimo quella degli sprite precalcolati).
    void usa_modelli(const std::map<std::string, Modello>& modelli, const std::array<double, 15>& rampa) {
        modelli_ = &modelli;
        rampa_ = rampa;
    }

    // Le navi intatte vicine dal modello a passi di 1 grado si generano in un filo di lavoro (la finestra); senza,
    // subito, nel filo della vista (predefinito: istantanee e prove restano deterministiche)
    void usa_filo_di_lavoro(bool si) { filo_di_lavoro_ = si; }

    // Palette del giorno (non illuminata): luminanze dei modi monocromi dell'oculare
    void usa_palette(const Palette& giorno) { giorno_ = &giorno; }

    // Geometria esposta per i test: pixel per radiante e riga dell'orizzonte nella vista.
    static double pixel_per_radiante(bool alto_ingrandimento);
    static double pixel_per_radiante(const VistaPeriscopio& v);   // col campo dell'oculare quando e' attivo
    static double riga_orizzonte(const VistaPeriscopio& v);
    // Riga della vista per un'elevazione apparente (radianti), con l'elevazione della linea di mira dell'oculare
    static double riga_elevazione(const VistaPeriscopio& v, double elevazione);
    // Geometria della vista per il meteo (grafica/cielo)
    static GeometriaVista geometria(const VistaPeriscopio& v);

private:
    void disegna_fondo(Tela& t, const VistaPeriscopio& v) const;
    // con `profilo` e `mare_vicino` (l'oculare col mare mosso) le creste piu' vicine di una nave o di una particella la coprono
    void disegna_navi(Tela& t, const Mondo& mondo, const Nave& proprio, const VistaPeriscopio& v, const Effetti* effetti,
                      const oculare::ProfiloMare* profilo = nullptr, const Tela* mare_vicino = nullptr) const;
    void disegna_reticolo(Tela& t, const VistaPeriscopio& v) const;
    void disegna_scala(Tela& t, const VistaPeriscopio& v) const;

    const Font& font_;
    const std::map<std::string, ClasseNave>& classi_;
    const std::map<std::string, InsiemeSprite>& sprite_;
    const std::map<std::string, Modello>* modelli_ = nullptr;
    const Palette* giorno_ = nullptr;
    std::array<double, 15> rampa_{};
    struct Generato {   // ultimo sprite generato per una nave: si rigenera quando vista, posa o scala cambiano
        double aob = -1.0, scala = 0.0;
        bool dritta = true;
        PosaNave posa;
        Sprite sprite;
    };
    mutable std::map<int, Generato> generati_;
    const Sprite& sprite_generato(const Nave& n, const Modello& m, double aob, bool dritta, double scala) const;
    // le navi intatte vicine dal modello a passi di 1 grado (E9 §2.3): l'ultimo sprite per nave e, col filo di
    // lavoro, quello in preparazione; senza il filo si generano subito (istantanee e prove, deterministiche)
    struct Intatto {
        double aob = -1.0, scala = 0.0;
        bool dritta = true;
        Sprite sprite;
        std::shared_ptr<std::future<Sprite>> lavoro;
        double lavoro_aob = -1.0, lavoro_scala = 0.0;
        bool lavoro_dritta = true;
    };
    mutable std::map<int, Intatto> intatti_;
    bool filo_di_lavoro_ = false;
    const Sprite* sprite_intatto(const Nave& n, const Modello& m, double aob, bool dritta, double scala) const;
};

}  // namespace m688
