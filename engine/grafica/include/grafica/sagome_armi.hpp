#pragma once
// Sagome delle armi di carico.toml (e, con sagoma_da_id, dei siluri e delle classi delle missioni senza voce nel
// carico) nello stile VGA del kit: profilo di fianco in scala con lunghezza e diametro, prua a destra, riconoscibile
// per tipo (Mk 48 e ADCAP, siluri sovietici del Type 53, Mk 67 SLMM, Mk 60 CAPTOR, PMR-1, capsule di Harpoon e
// Tomahawk, esche, Tigerfish, Spearfish, Mk 46, aviolanciati sovietici, SET-40, la balena del contatto biologico), con la colorazione di guerra o d'esercitazione. Poi la sezione del
// tubo lanciasiluri con l'arma dentro e lo stato (ricarica che scorre dalla rastrelliera, tubo allagato, porta
// esterna aperta, arma armata, mina pronta alla posa).
//
// Il disegno passa da un tracciatore unico (materiale e tono per pixel): disegna_sagoma_arma lo traduce in indici
// con una TavolozzaSagome, lo spaccato del quadro danni nei suoi materiali. La tavolozza fissa usa solo indici che
// la vista non schiarisce ne' scurisce (0-15, 80-95, 160-195, 254: INDICI_VISTA li lascia stare), quindi va bene
// anche nel pannello Armi di notte; quella ricca ridefinisce 48 posti (96-143) per le schermate a tutto schermo
// come la sala siluri.
//
// Fonti dell'aspetto (foto e schede, annotate anche in docs/06_kit_navi.md):
// - Mk 48 e ADCAP: Wikimedia Commons, «Mk 48 torpedo maintenance 1982» e «Mk 48 torpedo on retriever boat 1982»
//   (ogiva nera, sezioni verdi con fasce bianche ai giunti, meta' poppiera arancione in esercitazione),
//   «Pasadena (SSN-752) weapons» (rastrelliera: corpi verde scuro); nuclearcompanion.com, Mark 48 (gruppi: ogiva,
//   testata, controllo, serbatoio, corpo di poppa; propulsore a idrogetto intubato con quattro alette davanti).
// - Type 53 sovietici: Commons «53-65K torpedo at Park Patriot» e il memoriale di Kaliningrad (verde oliva di
//   guerra, alette a croce con i timoni e le eliche controrotanti), «53-65K torpedo MW» (testa d'esercitazione rossa,
//   corpo grigio).
// - Mk 67 SLMM: corpo del Mk 37 (Wikipedia, Submarine Launched Mobile Mines; globalsecurity, Mk 67): Commons «Mark
//   37 Torpedo» (ogiva nera con fascia bianca, corpo verde chiaro, alette a croce, elica singola). La sezione mina
//   e' in verde oliva con la fascia gialla dell'esplosivo (MIL-STD-709): STIMA.
// - Mk 60 CAPTOR: Wikipedia, Mark 60 CAPTOR (contenitore d'alluminio ormeggiato con il Mk 46 e l'ancora); Commons
//   «Mk.60 Captor» (capsula color kaki con fasce scure, calotta dorata) e «CAPTOR mine being loaded aboard sub».
// - PMR-1 «Kal'mar»: ru.wikipedia e allmines.net (mina-razzo ancorata, 534 mm x 7,8 m, 1700 kg; tre parti: razzo,
//   sezione strumenti con il cavo, ancora); Commons, memoriale di Kaliningrad (cilindro grigio chiaro).
// - Harpoon: capsula (ENCAP) «grande e fatta come un siluro a muso tondo», con ogiva, corpo e coda alettata
//   (designation-systems.net, globalsecurity): lunghezza come il Mk 48, nera, STIMA.
// - Tomahawk: Commons «Defense.gov News Photo 990331-N-1556A-001» (capsula d'acciaio lucido con le feritoie nella
//   sala siluri dell'USS Norfolk); lunghezza missile e booster 6,25 m.
// - Esche: ADC Mk 2 da 3 pollici (navysbir.com N191-023), MG-84: forma e colori STIMA. Esche semoventi da tubo
//   (dossier 05 §5): MG-74 Korund, siluro elettrico corto da 533 mm x 3,9 m con alette a croce ed eliche
//   controrotanti; MOSS Mk 70, corpo del Mk 37 da 10 pollici nel manicotto da 533 mm (guide e anelli). Colori STIMA.
// - Siluri senza voce nel carico e classi delle missioni (ricerca del kit, 27/10/2026; i paracadute e gli
//   stabilizzatori degli aviolanciati si staccano all'ammaraggio e non si disegnano):
//   - Mk 46 Mod 5: 2,59 m x 324 mm (en.wikipedia, navweaps); Commons «JMSDF Mark 46 torpedo(Dummy) Left Side View at
//     Maizuru Air Station 20150726» (CC BY-SA 4.0); colori e testa d'esercitazione arancione: STIMA media;
//   - Tigerfish Mk 24: 6,46 m (navweaps); naso «a balena», quattro alette rettangolari, due eliche controrotanti
//     (scheda Jane's, torp.esrc.unimelb.edu.au P00002813); Commons «Tigerfish torpedo.jpg» (CC BY-SA); colori STIMA;
//   - Spearfish: 5,95-7 m nelle fonti, si prende 6,1 (IMarEST 6,12); come il Tigerfish ma senza alette, col pump-jet
//     intubato (Jane's P00002932); nessuna foto libera; colori STIMA;
//   - AT-1M e AT-2M: 450 mm x 4,0 m (ru.wikipedia Ka-25, «Tekhnika i vooruzhenie» 2000/02) e 533 mm (4,8-5,2 m nelle
//     fonti, si disegna la forma dell'AT-1 allungata dal catalogo quando c'e'); eliche coassiali, timoni; forma STIMA;
//   - UMGT-1: 400 mm x 3,58 m (ru.wikipedia; navweaps 3,80), idrogetto intubato, muso di gomma fonotrasparente;
//   - APR-2: 350 mm x 3,7 m (ru.wikipedia, missilery.info, navweaps), razzo a solido: ugello e carenatura STIMA;
//   - SET-40 (il «Type 40» dell'originale): 400 mm x 4,5 m, copertura nera del muso, quattro alette coi timoni,
//     un'elica (ru.wikipedia); Commons «Torpedo SET-40 SET65 Kaliningrad 20190801» (CC BY-SA 4.0); corpo chiaro STIMA;
//   - balenottera comune (il contatto biologico): 21 m, altezza circa 0,12 della lunghezza, dorsale piccola e
//     uncinata a circa 0,72 dal muso, dorso scuro e ventre bianco (NOAA Fisheries, en.wikipedia; Commons
//     «Balaenoptera physalus NOAA.jpg», pubblico dominio).

#include <array>
#include <cstdint>
#include <functional>
#include <string>
#include <string_view>
#include <vector>

#include "grafica/palette.hpp"
#include "grafica/tela.hpp"
#include "nucleo/carico.hpp"

namespace m688 {

enum class MaterialeArma : std::uint8_t {
    nero,          // ogive, capsula dell'Harpoon, gomme
    grigio,        // acciaio, tubo, culatta
    argento,       // capsula del Tomahawk, PMR-1
    bianco,        // fasce ai giunti
    verde,         // Mk 48 di guerra
    oliva,         // siluri sovietici, sezione mina del Mk 67
    verde_chiaro,  // corpo del Mk 37 (Mk 67)
    kaki,          // CAPTOR
    arancione,     // sezioni d'esercitazione americane
    rosso,         // teste d'esercitazione sovietiche, spie
    giallo,        // fasce dell'esplosivo, maniglie, spie
    bronzo,        // eliche, calotta del CAPTOR
    acqua,         // tubo allagato, mare oltre la porta
};
inline constexpr int N_MATERIALI_ARMA = 13;

// Indici di palette per materiale, dal buio alla luce
struct TavolozzaSagome {
    std::array<std::vector<std::uint8_t>, N_MATERIALI_ARMA> rampe;
    std::uint8_t indice(MaterialeArma m, double tono, int x, int y) const;   // tono 0-1, retinato

    // Solo indici fissi della palette del giorno che la vista non tocca (pannello Armi in missione)
    static const TavolozzaSagome& fissa();
    // Rampe proprie nei posti primo..primo+47 della palette (sala siluri a tutto schermo)
    static TavolozzaSagome ricca(Palette& p, int primo = 96);
};

struct SagomaArma {
    std::string id;                 // voce di carico.toml (mk48, set65, mk67, ...); sconosciuta: forma per tipo
    TipoArmaCarico tipo = TipoArmaCarico::siluro;
    double lunghezza_m = 0.0;       // 0: quella della forma (5,8 m per i siluri)
    double diametro_mm = 533.0;
    bool sovietica = false;         // per le forme generiche
    bool esercitazione = false;     // colori d'esercitazione (sezioni arancioni, teste rosse)
};
SagomaArma sagoma_da_scheda(const SchedaArma& a, bool esercitazione = false);
// La sagoma per un nome qualunque, con la lunghezza e il diametro della forma: una voce di carico.toml, un siluro di
// siluri.toml senza voce nel carico (tigerfish, spearfish, mk46, at1, at2, umgt1, apr2) o una classe delle missioni
// (mk48, type53, type40, spearfish, sea_lance, biologico). Per le voci di carico.toml sagoma_da_scheda resta la buona:
// misure del catalogo.
SagomaArma sagoma_da_id(const std::string& id, bool esercitazione = false);
// Vero se il nome ha una forma sua (non quella generica del tipo)
bool sagoma_nota(const std::string& id);
// Lunghezza disegnata: le capsule (Harpoon, Tomahawk) sono piu' lunghe del missile del catalogo
double lunghezza_disegnata(const SagomaArma& s);

// Un pixel della sagoma: materiale e tono (0 buio, 1 luce)
using PuntoSagoma = std::function<void(int x, int y, MaterialeArma m, double tono)>;
// La sagoma con la poppa in x_poppa e l'asse in y_asse (pixel), px_m pixel per metro; il diametro scende fino a
// un minimo di 2 pixel. Prua verso le x crescenti.
void traccia_sagoma_arma(const SagomaArma& s, double x_poppa, double y_asse, double px_m, const PuntoSagoma& punto);

// Sagoma centrata in r, prua a destra. px_m = 0: la scala piu' grande che ci sta (in larghezza e in altezza).
// Restituisce il rettangolo occupato.
Rettangolo disegna_sagoma_arma(Tela& t, const SagomaArma& s, Rettangolo r, double px_m = 0.0,
                               const TavolozzaSagome& tav = TavolozzaSagome::fissa());

// --- sezione del tubo lanciasiluri ---
enum class FaseTubo : std::uint8_t { vuoto, ricarica, carico, armamento, armato };

struct SagomaInTubo {
    SagomaArma arma;                // id vuoto: tubo vuoto
    FaseTubo stato = FaseTubo::vuoto;
    double ricarica = 1.0;          // 0-1 durante la ricarica: l'arma scorre dalla rastrelliera dentro il tubo
    bool allagato = false;
    bool porta_aperta = false;      // porta esterna (di volata)
    double tempo_s = 0.0;           // spie che lampeggiano, bolle nel tubo che si allaga
};
// Il tubo di fianco in r, culatta a sinistra e volata a destra: spaccato col vano, la culatta (aperta in ricarica),
// la porta esterna, l'acqua, la spia (verde carico, ambra in armamento, rossa armato) e per le mine armate il segno
// della mina pronta alla posa accanto alla spia.
void disegna_tubo(Tela& t, Rettangolo r, const SagomaInTubo& s, const TavolozzaSagome& tav = TavolozzaSagome::fissa());
// Dove disegna_tubo mette l'arma (per le prove e per chi scrive accanto): poppa, asse e scala
struct GeometriaTubo {
    double x_culatta = 0, x_volata = 0, y_asse = 0, px_m = 0, x_poppa_arma = 0;
};
GeometriaTubo geometria_tubo(Rettangolo r, const SagomaInTubo& s);

}  // namespace m688
