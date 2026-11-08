#pragma once
// Oculare dei periscopi dell'Enhanced (docs/14_periscopi.md, P3; dossier 20 §6.1 e §8): campo tondo con la
// vignettatura, reticoli per scuola (croce e tacche americane, griglia sovietica, pali britannici) senza numeri dentro
// il cerchio, elevazione verso il cielo, tremolio, immagine fantasma dello stadimetro, filtri, viste monocrome
// (bassa luminosita' verde, intensificatore sovietico verde giallino, TV in bianco e nero con le righe, termico) e la
// telecamera verticale della vela dell'Alfa. Con ParametriOculare::attivo falso la vista del periscopio resta quella
// del classico.

#include <climits>
#include <cstdint>
#include <functional>
#include <string>
#include <utility>
#include <vector>

#include "grafica/palette.hpp"
#include "grafica/tela.hpp"
#include "nucleo/geometria.hpp"

namespace m688 {

enum class Reticolo { americano, sovietico, britannico };
enum class ModoOculare { giorno, bassa_luminosita, intensificatore, tv, termico };
enum class Filtro { nessuno, polaroid, rosso, verde, giallo, affumicato };

struct ParametriOculare {
    bool attivo = false;               // falso: vista del classico (rettangolare, due ingrandimenti fissi)
    double guadagno_luci = 1.0;        // i punti luminosi di notte all'ingrandimento scelto (luci::guadagno_ottico)
    double campo_gradi = 32.0;         // campo vero: il diametro del cerchio (la larghezza dello schermo in TV)
    Reticolo reticolo = Reticolo::americano;
    double passo_gradi = 4.0;          // divisioni grandi e piccole del reticolo (periscopi.toml, per ingrandimento)
    double passo_fine_gradi = 1.0;
    double elevazione_gradi = 0.0;     // linea di mira sopra l'orizzontale
    ModoOculare modo = ModoOculare::giorno;
    Filtro filtro = Filtro::nessuno;
    double tremolio = 0.0;             // 0-1, gia' ridotto dalla stabilizzazione
    double sdoppiamento_gradi = 0.0;   // stadimetro: spostamento dell'immagine fantasma (0 spento)
    bool sdoppiamento_orizzontale = false;   // lente ruotata di 90 gradi: angolo sulla prua dalla lunghezza
    // telemetro a reticolo (sovietici): sdoppiamento_gradi e' il cursore sulla scala verticale, sopra la linea di mezzo,
    // invece dell'immagine fantasma
    bool reticolo_misura = false;
    bool sott_acqua = false;           // testa sotto il pelo dell'acqua (alzata in corso, onde): acqua torbida
    // Finestra della testa sull'acqua calma (StatoPeriscopio::altezza_testa_m); negativa: sconosciuta, niente onde
    // sulla finestra (vista del classico e anteprime senza posto)
    double altezza_testa_m = -1.0;
    double pioggia = 0.0;              // 0-1: gocce di pioggia sulla finestra (meteo)
    // Mare mosso (meteo): direzione da cui vengono le onde (gradi veri) e vento (nodi; negativo: quello del mare
    // formato per l'altezza delle onde, Pierson-Moskowitz)
    double direzione_mare_gradi = 270.0;
    double vento_kn = -1.0;
    // Alfa: la telecamera verticale della vela al posto del periscopio, alla quota del battello sotto il ghiaccio
    // (0-1 copertura)
    bool telecamera_verticale = false;
    double quota_m = 0.0, ghiaccio = 0.0;
    // Artiglieria (docs/16, G4; dossier 22 §6.2 e §9.6): l'acqua delle colonne cadute entro ~100 m sulla finestra
    // (0-1, Effetti::acqua_sul_vetro); il colpo diretto alla testa: l'immagine che si spegne (0-1: dal rumore al nero)
    // o il vetro incrinato (0-1: le crepe, poi la macchia d'acqua che sale); seme_colpo sceglie il punto e le crepe
    double spruzzi_proietti = 0.0;
    double spento = 0.0, incrinato = 0.0;
    std::uint32_t seme_colpo = 0;
};

struct DatiPeriscopio;
struct StatoPeriscopio;

// Dal modello (periscopi.toml) e dallo stato del posto (nucleo/periscopi.hpp, P1): campo, passi del reticolo e
// scuola per l'ingrandimento scelto, elevazione, modo, tremolio, testa sott'acqua, filtro, sdoppiamento dello
// stadimetro e telecamera verticale. Quota del battello, ghiaccio e mare (meteo) li aggiunge la stazione (P4).
ParametriOculare parametri_oculare(const DatiPeriscopio& d, const StatoPeriscopio& s);
// "polaroid", "rosso", "verde", "giallo", "affumicato" (DatiPeriscopio::filtri); altro: nessuno
Filtro filtro_da_nome(const std::string& nome);

namespace oculare {

inline constexpr int CENTRO_X = 320, CENTRO_Y = 180;   // centro della vista (ALTEZZA_VISTA / 2)
inline constexpr int RAGGIO = 150;                     // cerchio dell'oculare: righe 30-330, sotto la scala in alto
inline constexpr int TV_MEZZA_L = 200;                 // schermo TV 4:3, 400 x 300
// rampa monocroma dei modi notte e TV (indici liberi della palette del giorno, non toccati dalla luce ambiente)
inline constexpr int RAMPA = 160, N_RAMPA = 15;
inline constexpr double TREMOLIO_GRADI = 0.5;          // ampiezza angolare del tremolio pieno (STIMA)

// Pixel per radiante della vista: il campo copre il diametro del cerchio (in TV la larghezza dello schermo)
double pixel_per_radiante(const ParametriOculare& p);
// Rettangolo occupato dall'immagine (cerchio o schermo)
Rettangolo area(const ParametriOculare& p);
bool dentro(const ParametriOculare& p, int x, int y);
// Spostamento della linea di mira per il tremolio al tempo t (gradi di rilevamento e di elevazione)
void tremolio(const ParametriOculare& p, double tempo_s, double& d_rilevamento, double& d_elevazione);

// Rampa monocroma (modi notte, TV) e filtro: la palette della vista (gia' illuminata) si tinge negli INDICI_VISTA
void palette(Palette& pal, const ParametriOculare& p);
// Immagine fantasma dello stadimetro, sovrapposta a scacchiera (meta' luce per ciascuna delle due immagini)
void sdoppia(Tela& t, const ParametriOculare& p);
// Modi monocromi: luminanza dalla palette del giorno con la luce ambiente amplificata, grana e righe della TV
void converti(Tela& t, const Palette& giorno, const ParametriOculare& p, double luce, double tempo_s);
void disegna_reticolo(Tela& t, const ParametriOculare& p);
// Fuori dal cerchio nero, vignettatura retinata verso il bordo (angoli tondi e bordo scuro per lo schermo TV)
void maschera(Tela& t, const ParametriOculare& p);

// Testa sott'acqua: acqua torbida schiarita verso la superficie, con le bolle
void acqua(Tela& t, const ParametriOculare& p, double luce, double tempo_s);

// Il mare sulla finestra della testa (nucleo/onde.hpp) con le onde di altezza significativa hs_m: l'acqua sale dal
// basso col suo profilo mosso e la schiuma e copre la vista; quando scende resta un velo che scola e le gocce, che
// scivolano e asciugano in pochi secondi; con l'onda vicina sotto la finestra e mare grosso gli spruzzi; la pioggia
// fa le sue gocce. Va chiamata sopra la scena, prima dei modi monocromi. Ritorna la copertura (0 libera, 1 sommersa).
double acqua_sulla_finestra(Tela& t, const ParametriOculare& p, double hs_m, double luce, double tempo_s);
// La testa colpita (artiglieria, G4): l'immagine che si spegne col rumore e il vetro incrinato con l'acqua che sale.
// Va chiamata dopo i modi monocromi (converti) e prima del reticolo; nulla se spento e incrinato sono 0
void guasto(Tela& t, const ParametriOculare& p, double luce, double tempo_s);
inline constexpr double FINESTRA_M = 0.12;   // mezza altezza della finestra della testa (STIMA)

// Fin dove il mare vicino copre, colonna per colonna (lo riempie mare): per ogni colonna le distanze crescenti a cui la
// riga piu' alta del mare gia' disegnato sale. Un oggetto a distanza r e' coperto dalle righe >= riga(x, r): le creste
// piu' vicine di lui, che lo nascondono (l'occhio basso nel cavo di un'onda grossa non vede le navi dietro la cresta).
struct ProfiloMare {
    int x0 = 0;                                                    // prima colonna
    std::vector<std::vector<std::pair<float, short>>> colonne;     // (distanza m, riga) con la riga che sale
    int riga(int x, double r) const;                               // INT_MAX: niente copre
};

// La superficie del mare sotto l'orizzonte, mossa (Enhanced): onde lunghe attorno al picco di Pierson-Moskowitz e onde
// corte del vento, viste in prospettiva dall'occhio a `quota_occhio_m` sul mare che ha davanti (posizione nel mondo x,
// y: le onde stanno ferme nel mondo e corrono); il chiaro e lo scuro vengono dal cielo riflesso dalle facce verso
// l'occhio; le onde piu' corte del pixel si spengono; foschia verso l'orizzonte.
// - Il mare vicino e' un campo d'altezze percorso colonna per colonna dal vicino al lontano: le creste coprono il mare
//   dietro e, sopra l'occhio, l'orizzonte e il cielo (orizzonte frastagliato, muri d'acqua col mare grosso e la testa
//   bassa). Oltre, dove l'onda e' piu' piccola del pixel, il mare per righe.
// - Schiuma per forza del vento (dossier 21 §9, Beaufort): creste che frangono con la frazione di Monahan (W = 3,84e-6
//   U^3,41) da forza 3; strisce di schiuma lungo il vento da forza 7 (Bf 7 ~7%, 8 ~15%, 9 ~23%, 10 ~30%, che con le
//   creste fanno il «mare bianco», 11-12 fino al 45%: STIMA); spruzzaglia strappata dalle creste da forza 8, che il
//   vento porta un paio di metri sopra il mare.
// Mare calmo (hs_m <= 0): niente. Con `profilo` restituisce fin dove il mare vicino copre, per navi e particelle.
void mare(Tela& t, const ParametriOculare& p, double rilevamento_gradi, double occhio_x, double occhio_y,
          double quota_occhio_m, double hs_m, double visibilita_m, double tempo_s, ProfiloMare* profilo = nullptr);
// Il ghiaccio attorno alla testa (Enhanced, sotto la banchisa e al suo margine). Il campione in un punto del mondo lo
// da' la stazione dal campo del ghiaccio della missione (CampoGhiaccio::in); la vista ci aggiunge solo la forma fine:
// fratture fra le lastre, gobbe della neve, la linea di una cresta per cella.
struct CampioneGhiaccio {
    bool ghiaccio = false;      // lastra o nilas
    bool nilas = false;         // ghiaccio nuovo sottile, scuro e liscio
    double spessore_m = 0.0;    // livellato
    double cresta_m = 0.0;      // la vela della cresta di pressione sopra la lastra (0: nessuna)
};
struct GhiaccioVista {
    bool attivo = false;        // spento (predefinito): nessun ghiaccio, la vista di sempre
    double copertura = 0.0;     // del meteo dove sta l'occhio: dentro il pack (>= 0,3) il mare e' calmo
    double cella_m = 200.0;     // le celle del campo: una cresta per cella
    Vec2 deriva_m{0.0, 0.0};    // lo spostamento del pack (deriva per tempo): lastre e creste ferme nel ghiaccio
    std::function<CampioneGhiaccio(Vec2)> campione;
    // dal meteo (la vista li copia da VistaPeriscopio::meteo): le nubi riflettono il ghiaccio e l'acqua sotto di loro
    double nubi = 0.0;          // copertura 0-1
    double plafond_m = 1500.0;  // base delle nubi
};
// La banchisa sotto l'orizzonte, sopra il mare gia' disegnato: come il mare vicino un campo d'altezze percorso colonna
// per colonna dal vicino al lontano, fino all'orizzonte delle creste.
// - Lastre col bordo libero di un decimo dello spessore e la neve a gobbe; nilas scuro e liscio; fratture strette fra le
//   lastre; le creste di pressione a blocchi lungo una linea per cella, alte quanto la vela (pescaggio / 4,5), con le
//   falde al sole e in ombra; l'acqua dei canali e delle polynye calma e scura.
// - Il bordo delle lastre verso l'occhio e' un filo piu' scuro; le creste nascondono quello che c'e' dietro.
// - Fuori dal pack (copertura sotto 0,3) l'acqua resta quella del mare mosso e il ghiaccio lontano e' il margine.
// - Il cielo: la base delle nubi chiara sopra il ghiaccio e scura sopra l'acqua, anche oltre l'orizzonte; col cielo
//   sereno solo una fascia chiara bassa sopra la banchisa lontana, vista da fuori.
// - La foschia schiarisce il ghiaccio lontano; la notte, la nebbia e i modi dell'oculare li fanno la palette e il meteo.
// Con `profilo` aggiunge al profilo del mare fin dove il ghiaccio copre (navi dietro le creste).
void banchisa(Tela& t, const ParametriOculare& p, double rilevamento_gradi, double occhio_x, double occhio_y,
              double quota_occhio_m, double visibilita_m, double tempo_s, const GhiaccioVista& g, ProfiloMare* profilo = nullptr);
// La terraferma attorno (Enhanced): coste, isole, pareti dei fiordi viste al periscopio. La quota del terreno nel punto
// del mondo (metri sul mare, negativa in mare) la da' la stazione dal fondale della missione (Fondale::quota); vuota,
// nessuna costa (il classico e le missioni in mare aperto).
struct CostaVista {
    bool attiva = false;
    double cella_m = 200.0;     // il passo della griglia del fondale: la vista lo liscia
    std::function<double(Vec2)> quota;
};
// La terra sopra il mare gia' disegnato: colonna per colonna dal vicino al lontano, con la curvatura e la rifrazione,
// le creste a strati nella foschia, la roccia e il bosco di giorno, le sagome scure di notte (`luce`), la risacca
// bianca sulla linea di costa. Con `profilo` aggiunge fin dove la terra copre (le navi dietro un'isola).
void costa(Tela& t, const ParametriOculare& p, double rilevamento_gradi, double occhio_x, double occhio_y,
           double quota_occhio_m, double visibilita_m, double luce, const CostaVista& c, ProfiloMare* profilo = nullptr);
// La quota delle creste di pressione nel punto del mondo, come le disegna la banchisa (senza il bordo libero della
// lastra): per i test
double quota_cresta(const GhiaccioVista& g, Vec2 p);
// Frazione di superficie coperta dalle strisce di schiuma col vento a 10 m (m/s): 0 sotto forza 7 (STIMA, dossier 21 §9)
double frazione_strisce(double vento_m_s);
// Frazione di superficie coperta dalle creste che frangono col vento a 10 m (m/s): Monahan e O'Muircheartaigh 1980
double frazione_schiuma(double vento_m_s);

// Telecamera verticale nella vela dell'Alfa (dossier 20 §4.4): guarda in alto, verso la superficie o il ghiaccio.
// `ghiaccio` 0-1 copertura (lastre scure, canali chiari, creste di pressione); senza ghiaccio la superficie mossa e le
// caustiche. Disegna nella rampa con la grana e le righe della TV; `palette` col modo tv la colora.
void telecamera_verticale(Tela& t, const Rettangolo& r, double quota_m, double luce, double ghiaccio, double tempo_s);

}  // namespace oculare

}  // namespace m688
