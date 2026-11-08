#pragma once
// Fenomeni visibili delle esplosioni e degli incendi (docs/10_danni_affondamento_esplosioni.md §5.4-5.5, fase D4).
// Solo numeri: dimensioni, tempi e regimi dalla fisica, che gli effetti grafici (grafica/effetti.hpp) mettono in
// scena. Formule e tabelle dal dossier docs/ricerca/07_danni_affondamento_esplosioni.md:
// - bolla: periodo di Willis e raggio massimo (§1.4), oscillazioni prima della superficie (Tab. 12 di Swisdak);
// - superficie: anello "slick" (§2.2), cupola di spruzzi di Kolsky (§2.3), pennacchi di Swisdak (§2.4 a), base
//   surge, chiazza e pulsazioni (§2.5), con il regime dalla profondita' relativa al raggio della bolla (§2.1);
// - nell'aria: palla di fuoco, schegge e nube dei depositi (§4.3, stime di gioco);
// - fumo degli incendi: pennacchio piegato dal vento di Briggs e altezza di fiamma di Heskestad (letteratura
//   classica sulla dispersione e sugli incendi; il calore rilasciato e' una stima di gioco).

#include <limits>

namespace m688 {

enum class RegimeSubacqueo {
    sfiato,         // d/A_max < 1: la bolla sfonda subito, colonna cava con la corona scura dei gas
    getto,          // 1-1,5: getto sottile e alto con una gonna di spruzzi alla base
    pennacchi,      // 1,5-3: cupola, poi pennacchi bianchi all'impulso della bolla
    cupola_bassa,   // 3-5: cupola bassa, pennacchio piccolo e chiazza
    profonda,       // oltre 5: piccola cupola o solo una gobba, poi la chiazza
};

const char* nome_regime(RegimeSubacqueo r);

struct FenomenoSubacqueo {
    double W = 0.0, d = 0.0;          // kg di TNT equivalente per la bolla, m di profondita' della carica
    double A_max = 0.0;               // m: raggio massimo della bolla
    double T1 = 0.0, T2 = 0.0, T3 = 0.0;   // s: periodi della bolla
    double d_su_A = 0.0;
    RegimeSubacqueo regime = RegimeSubacqueo::profonda;
    // anello scuro: corre a r = sqrt((1500 t)^2 - d^2) fino a R_slick
    double R_slick = 0.0, t_slick = 0.0;
    // cupola di spruzzi (0 se fuori dal campo di validita' di Kolsky)
    double v0_cupola = 0.0, H_cupola = 0.0, t_apice_cupola = 0.0, R_cupola = 0.0;
    // pennacchi (0 se profonda oltre la tabella): altezza e raggio massimi, istante di uscita e salita all'apice
    double H_pennacchio = 0.0, R_pennacchio = 0.0, t_pennacchio = 0.0, t_salita = 0.0;
    double H_gobba = 0.0;             // m: gobba dell'acqua quando non c'e' pennacchio
    bool corona_scura = false;        // sfiato: nube grigio-nera di gas in cima alla colonna
    double R_base = 0.0;              // m: anello di base surge (0 se non c'e')
    double durata_nebbia = 0.0;       // s: spruzzi e nebbia sospesi dopo la ricaduta
    double R_chiazza = 0.0, durata_chiazza = 0.0;   // schiuma (o fango) sul mare
    bool fango = false;               // fondo vicino: acqua marrone
    int pulsazioni = 0;               // sussulti successivi visibili (a T1 + T2, T1 + T2 + T3)
};

// W: TNT equivalente per la bolla (massa x RBE, dossier 07 §1.5); d: profondita' della carica; fondale: profondita'
// dell'acqua nel punto (infinita se ignota).
FenomenoSubacqueo fenomeno_subacqueo(double W, double d, double fondale = std::numeric_limits<double>::infinity());

struct FenomenoAereo {
    double W = 0.0;                    // kg di TNT
    double D_palla = 0.0, durata_palla = 0.0;   // m, s
    double v_schegge = 0.0;            // m/s: velocita' massima dei frammenti visibili
    double H_nube = 0.0;               // m: cima della nube di fumo (fungo dei depositi; sbuffo del missile)
    double lampo = 0.0;                // 0-1: intensita' del lampo sulla palette
};

// Esplosione nell'aria o a filo d'acqua: testata di un missile (deposito = false) o deposito di munizioni.
FenomenoAereo fenomeno_aereo(double W, bool deposito);

// Pennacchio di fumo di un incendio di Q watt (Briggs, atmosfera neutra): flusso di galleggiamento, quota di
// risalita alla distanza x sottovento con vento u, e quota finale.
double flusso_galleggiamento(double Q);                                    // m^4/s^3
double risalita_fumo(double F, double u, double x);                        // m
double risalita_finale(double F, double u);                                // m
// Altezza media della fiamma (Heskestad) per Q watt su un focolare di diametro D metri; 0 se Q e' troppo piccolo.
double altezza_fiamma(double Q, double D);

}  // namespace m688
