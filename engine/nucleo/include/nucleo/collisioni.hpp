#pragma once
// Danni da collisione fra unita' (docs/ricerca/16_collisioni.md, citato «16»: §2 fisica, §4.2 proposta P2). Solo
// Enhanced: nel classico le unita' si attraversano come nell'originale.
//
// Il contratto con la rilevazione del contatto (runtime, 16 §4.1 P1): al primo passo in cui le forme si toccano la
// missione produce un Urto con la sola geometria del mondo (punto, quota del punto, normale) e lo stato delle due unita';
// un Urto per contatto: il successivo solo dopo una separazione o dopo 2 s di contatto continuo. valuta_urto ne ricava
// velocita' relativa, angolo, parte colpita di ciascuno scafo ed energie, e ne fa i danni di ciascuna unita'
// (DanniUrtoUnita), che applicano BattelloDanni::urto_meccanico, DanniNave::urto_meccanico e
// DanniSottomarini::collisione. L'acustica (il colpo e lo stridio) la fa la scena dai campi livello e durata.
//
// Convenzioni:
// - mondo: x est, y nord, z in su; rotte in gradi bussola;
// - quote: positive in giu'. La quota dei sottomarini e' quella della CHIGLIA (Nave::quota): il dorso sta a
//   quota - larghezza_m (il diametro), la cima della vela a quota - (larghezza_m + altezza_vela_m). Vicino alla
//   superficie serve la chiglia vera: la quota periscopica della missione (QUOTA_PERISCOPICA_MISSIONE_M, 6,1 m) si
//   traduce con quota_classe (nucleo/periscopi.hpp; 688: 18,3 m), altrimenti la vela uscirebbe dall'acqua. Navi:
//   chiglia a pescaggio_m (duomo compreso), galleggiamento a 0;
// - scafo (coordinate del kit, come scafi/<classe>.toml e i modelli 3D): x lungo lo scafo con la prua positiva e 0 a
//   mezza nave, y a SINISTRA, z in su col galleggiamento a 0 (per i sottomarini quello in superficie: chiglia a
//   -pescaggio_m).
//
// Fisica (16 §2.1-2.4): E_n = 1/2 mu v_n^2 con mu = m1' m2' / (m1' + m2'), m' = m (1 + k) e le masse aggiunte k nella
// direzione della normale (sottomarino 0,02 assiale e 0,95 trasversale; nave 0,075 longitudinale, 0,4 laterale, 1
// verticale); lo strisciamento E_t = 0,3 x 1/2 mu v_t^2 graffia (piastrelle, casing) ma non sfonda. L'energia normale si
// divide fra i due scafi per rigidezza della parte (meta' ciascuno a pari rigidezza, altrimenti 70% al piu' cedevole) e
// in ciascuno passa verso l'interno: duomo, timoni ed eliche (2 MJ), casing e casse (12 MJ x D/10, x1,5 sui doppi
// scafi), vela (10 MJ; 50 la vela da ghiaccio del Typhoon), zona prodiera del sottomarino (1 GJ per il 688, x (D/10)^2:
// taratura San Francisco), scafo resistente (40 MJ x D/10 x (1 - 0,5 h/h_collaudo)). Le parti a lama (vela e timoni che
// salgono, prua di una nave, eliche) concentrano l'urto: la lamiera dell'altro regge 0,3 volte. Falla: R_T = max((E -
// 32,7) / 47,2 [Minorsky], (E - capacita') / 80) m^3 d'acciaio, area R_T / spessore; una lama che striscia apre uno
// squarcio lungo v_t x 1 s (Ehime Maru: piu' compartimenti). Navi sotto le 3.000 t: struttura leggera, rigidezza bassa.
// Tutte le capacita' sono STIMA.

#include <cstdint>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "nucleo/battello.hpp"
#include "nucleo/classi.hpp"
#include "nucleo/geometria.hpp"

namespace m688 {

// Una delle due unita' al momento del contatto
struct CorpoUrto {
    int unita = -1;                    // id; -1 per un ostacolo fisso (roccia, banchina, relitto)
    std::string classe;                // chiave di classi.toml; vuota per l'ostacolo fisso
    bool sottomarino = false;
    bool in_superficie = false;        // sottomarino emerso: i danni di una nave (falla_nave) piu' i suoi sistemi
    double massa_t = 0.0;              // dislocamento (immersione per i sottomarini immersi); 0 = infinita (ostacolo)
    Vec2 pos;                          // m, centro dell'unita'
    double quota_m = 0.0;              // chiglia VERA, positiva in giu'; navi 0. A quota periscopica la missione tiene i
                                       // 20 ft del classico: va tradotta con quota_classe (nucleo/periscopi.hpp)
    double rotta_gradi = 0.0;
    double velocita_m_s = 0.0;         // sull'acqua, lungo la rotta
    double rateo_verticale_m_s = 0.0;  // positivo in giu'
    bool periscopio_alzato = false, alberi_alzati = false, schiera_filata = false;
};

struct Urto {
    double tempo_s = 0.0;              // s di missione
    CorpoUrto a, b;                    // a: l'urtante; b: l'urtato
    Vec2 punto;                        // m, punto di contatto nel piano
    double quota_punto_m = 0.0;        // profondita' del punto (positiva in giu'; negativa sopra il galleggiamento)
    double normale[3] = {0, 0, 1};     // versore della normale al contatto, da a verso b (x est, y nord, z in su)
};

enum class ParteUrto : std::uint8_t {
    prua, duomo, fianco, dorso, chiglia, vela, alberi, poppa, eliche, timoni, schiera, sovrastrutture
};
const char* nome_parte_urto(ParteUrto p);

struct DanniUrtoUnita {
    ParteUrto parte = ParteUrto::fianco;
    double energia_mj = 0.0;           // energia normale assorbita da questo scafo
    double strisciamento_mj = 0.0;     // la sua parte dello strisciamento
    double x_m = 0, y_m = 0, z_m = 0;  // punto colpito nelle coordinate dello scafo (kit)
    // sottomarini: perdite dei sistemi (0..1, come BattelloDanni::guasto_sistema)
    std::vector<std::pair<Sistema, double>> sistemi;
    double falla_resistente_m2 = 0.0;  // nello scafo resistente, nel compartimento al punto x
    double falla_cassa_m2 = 0.0;       // casse zavorra e casing: le casse vicine non tengono piu' l'aria
    double ammaccatura = 0.0;          // in piu' su BattelloDanni::danno_scafo (quota di collasso x (1 - 0,5 a))
    double piastrelle_m2 = 0.0;        // rivestimento anecoico strappato
    // navi (e sottomarini in superficie): falla sotto il galleggiamento nella cella al punto, parti perse
    double falla_nave_m2 = 0.0;
    double squarcio_m = 0.0;           // lunghezza della falla lungo lo scafo quando una lama striscia (0: puntuale)
    bool duomo_perso = false, eliche_danneggiate = false, timone_bloccato = false, sovrastrutture = false;
    // per tutti
    double velocita_max = 1.0;         // frazione: elica piegata o persa
    double rumore_db = 0.0;            // aumento permanente del rumore irradiato (elica, piastrelle)
    double scossa_g = 0.0;             // picco d'accelerazione a bordo (dv / 0,5 s)
    double rollio_gradi = 0.0;         // ampiezza del rollio provocato
};

struct EsitoUrto {
    double v_n_m_s = 0.0, v_t_m_s = 0.0;   // componenti della velocita' relativa (a rispetto a b) lungo la normale e sul piano
    double angolo_gradi = 0.0;             // fra la velocita' relativa e il piano di contatto (90 = di punta)
    double massa_ridotta_t = 0.0;          // mu con le masse aggiunte
    double energia_normale_mj = 0.0, energia_tangente_mj = 0.0;
    bool radente = false;                  // angolo sotto 25 gradi
    DanniUrtoUnita a, b;
    double livello_sorgente_db = 0.0;      // il colpo, dB re 1 uPa a 1 m (STIMA 16 §2.6: 170 a 1 MJ, +10 per decade)
    double durata_strisciamento_s = 0.0;   // lo stridio
};

// Puro e deterministico. `classi` e `cartella_dati` (data/motore) per forme, scafi e quote di collaudo.
EsitoUrto valuta_urto(const Urto& u, const std::map<std::string, ClasseNave>& classi, const std::string& cartella_dati,
                      std::uint64_t seme);

// Navi senza scafo per i danni: frazione dell'integrita' dell'originale da togliere (STIMA dalla falla e dalle parti)
double perdita_integrita_urto(const DanniUrtoUnita& d);

}  // namespace m688
