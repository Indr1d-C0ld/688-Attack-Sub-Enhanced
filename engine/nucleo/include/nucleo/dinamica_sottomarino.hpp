#pragma once
// Dinamica del sottomarino per la versione Enhanced (docs/11_comandi_e_stazioni.md §3.1 e §10, fase C6a; numeri dal
// dossier docs/ricerca/01_piattaforme_e_armi.md §2 e §8.1 e 02_acustica_oceanografia_dottrina.md §2.3).
//
// Tre gradi di liberta' al passo fisso del motore: avanzo, imbardata, beccheggio con la quota.
// - Avanzo: spinta dell'elica T = c Vmax^2 n|n| (n: frazione dei giri massimi, che la turbina insegue in una
//   ventina di secondi; a marcia indietro rende il 70%) contro la resistenza D = c u|u| (1 + 1,5 (barra/35)^2). A giri
//   costanti la velocita' di regime e' n Vmax; in accostata con la barra tutta scende di circa un terzo.
//   c = 0,7 P / Vmax^3 dalla potenza all'asse pubblicata; massa del dislocamento in immersione piu' il 10% di massa
//   aggiunta.
// - Imbardata (Nomoto del primo ordine): il rateo insegue u / R (barra / 35), con R la meta' del diametro tattico a
//   barra tutta (4-6 lunghezze nel dossier 01, «inversione in decine di secondi» per l'Alfa), costante di tempo
//   1,5 L / u. La barra si muove a 5 gradi/s.
// - Beccheggio: l'assetto insegue l'angolo ordinato, o quello che porta alla quota ordinata, a un rateo che cresce
//   con la velocita' (idroplani). Il momento d'assetto dei danni lo sposta quanto i piani non compensano (fino a 15
//   gradi di momento di raddrizzamento Delta g BG a 8 m/s, in proporzione al quadrato della velocita').
// - Quota: dz/dt = -u sin(assetto) + w (dossier 01: dz/dt ~ v sin theta). w e' la deriva del peso netto contro la
//   resistenza verticale della pianta: rateo_verticale_regime del battello (nucleo/battello.hpp) a velocita' 0, lo
//   stesso modello del kit senza la sua portanza, che qui e' l'assetto stesso (un battello pesante tiene la quota
//   con la prua in su). Peso netto e momento vengono da BattelloDanni (zero a battello intatto).
// - Cavitazione: le pale cavitano quando i giri, in nodi equivalenti (|n| Vmax), superano
//   V_inc(z) = V_inc0 sqrt((10,33 + z)/10,33) (dossier 02 §2.3), con z la quota dell'asse.
// Velocita' in m/s e gradi all'interno; rotta in gradi bussola (+ a dritta), quota in m positiva verso il basso,
// assetto in gradi positivo a prua in su (come ForzeIdrostatiche::momento_assetto_Nm).

#include <optional>
#include <string>

#include "nucleo/battello.hpp"
#include "nucleo/classi.hpp"
#include "nucleo/scafo.hpp"

namespace m688 {

struct ParametriSottomarino {
    double lunghezza_m = 110.3, larghezza_m = 10.06;
    double dislocamento_t = 6927.0;   // in immersione
    double velocita_max_kn = 32.0;
    double potenza_kW = 22370.0;      // all'asse
    double diametro_tattico_L = 5.0;  // a barra tutta, in lunghezze
    double v_cavitazione0_kn = 5.0;   // insorgenza a quota zero
    double bg_m = 0.5;                // distanza fra centro di carena e baricentro in immersione
    double area_pianta_m2 = 0.0;      // per il moto verticale; 0: 0,8 L B
};

// Parametri di una classe: lunghezza e velocita' da classi.toml, dislocamento dallo scafo (sezione [sottomarino]),
// potenza, diametro tattico e cavitazione dalla tabella del dossier 01 (stime dove il dossier non da' il dato).
ParametriSottomarino parametri_sottomarino(const std::string& classe, const ClasseNave* c = nullptr,
                                           const DatiSottomarino* s = nullptr);

struct ComandiSottomarino {
    double barra_deg = 0.0;              // angolo di barra, + a dritta, +-35 (ignorato se c'e' la rotta)
    std::optional<double> rotta;         // «accostare per»: il timoniere accosta con al piu' barra_max_deg
    double barra_max_deg = 15.0;
    double giri = 0.0;                   // frazione dei giri massimi, da -0,5 (indietro tutta) a 1 (massima)
    std::optional<double> quota_m;       // quota ordinata: l'assetto la insegue entro angolo_max_deg
    double angolo_deg = 0.0;             // senza quota ordinata: angolo di immersione, + a prua in giu'
    double angolo_max_deg = 15.0;        // dottrina 15-20 gradi, emergenza 30 (dossier 01)
};

class DinamicaSottomarino {
public:
    static constexpr double BARRA_MAX = 35.0, GIRI_INDIETRO = -0.5;

    DinamicaSottomarino(const ParametriSottomarino& p, double rotta, double quota_m, double velocita_kn = 0.0);

    // Un passo: comandi del giocatore (o dell'IA), forze del battello danneggiato, limiti dei sistemi
    // (velocita' massima, timone e idroplani in avaria: la barra resta ferma, i piani non governano).
    void avanza(double dt, const ComandiSottomarino& c, const ForzeIdrostatiche& f = {}, const LimitiBattello& l = {});

    double velocita_kn() const;
    double velocita_m_s() const { return u_; }
    double rotta() const { return rotta_; }
    double quota_m() const { return z_; }
    double assetto_deg() const { return theta_; }       // + prua in su
    double rateo_virata_deg_s() const { return r_; }    // + a dritta
    double rateo_quota_m_s() const { return w_; }       // + verso il basso
    double barra_deg() const { return barra_; }
    double giri() const { return n_; }
    double velocita_cavitazione_kn() const { return velocita_cavitazione_kn(z_); }
    double velocita_cavitazione_kn(double quota_m) const;
    bool cavita() const;
    const ParametriSottomarino& parametri() const { return p_; }
    // giri che portano a regime alla velocita' data (telegrafo in nodi)
    double giri_per_velocita(double kn) const;

    // stato imposto da fuori (fondale, script, emersione forzata dal motore): la dinamica riparte da li'
    void imposta_quota(double quota_m) { z_ = quota_m; }
    void imposta_rotta(double rotta);
    void imposta_velocita(double kn);

private:
    ParametriSottomarino p_;
    double c_ = 0.0, massa_ = 0.0, raggio_ = 0.0, vmax_ = 0.0, area_ = 0.0, raddrizzamento_ = 0.0;
    double u_ = 0.0, rotta_ = 0.0, r_ = 0.0, z_ = 0.0, theta_ = 0.0, w_ = 0.0, barra_ = 0.0, n_ = 0.0;
};

}  // namespace m688
