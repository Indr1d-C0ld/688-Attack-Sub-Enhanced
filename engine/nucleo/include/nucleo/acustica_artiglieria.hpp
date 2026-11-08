#pragma once
// Acustica dell'artiglieria navale contro il sottomarino (pezzo G5 di docs/16_artiglieria.md; numeri e fonti nel
// dossier docs/ricerca/22_artiglieria_contro_sottomarini.md §7, §9.7 e §10, citato «22»). Solo Enhanced.
//
// Il contratto con il modello di tiro (G1) e con l'IA delle navi (G2, runtime): la missione produce gli
// EventoArtiglieria; qui diventano le sorgenti transitorie della scena acustica (sonar, cascata, cuffia), e
// AmbienteBordo::artiglieria (nucleo/bordo_audio.hpp) ne fa i suoni a bordo.
// - Lo scoppio sull'acqua e' un «crack» corto e secco: lo specchio della superficie ne taglia il basso (22 §7.1). Il
//   taglio lo fa la perdita del modello con la sorgente a 0,5 m; il livello e' tarato sul 5" a 1 km. Con mare 3 la
//   superficie ruvida toglie coerenza allo specchio, e oltre i 3 km il modello lo sente piu' forte della formula a
//   specchio liscio (picco_scoppio_db), che serve al bordo e alle prove.
// - Il colpo di partenza e' un tonfo sordo a 40-150 Hz alla cadenza del pezzo (22 §7.3); i proietti non hanno bolla.
//
// Il modulo e' puro e deterministico.

#include <cstdint>
#include <vector>

#include "nucleo/acustica_asw.hpp"

namespace m688 {

enum class TipoEventoArtiglieria : std::uint8_t {
    partenza,            // il colpo di partenza, alla nave che spara (pos = la nave)
    scoppio_superficie,  // spoletta a impatto o a ritardo: scoppia sull'acqua (pos = il punto di caduta)
    scoppio_aria,        // spoletta di prossimita' (VT): scoppia a quota_m metri sopra l'acqua
    tonfo,               // proietto inerte o spoletta fallita: entra in acqua senza esplodere
    rimbalzo,            // rimbalza sotto l'angolo critico (~8 gradi): il tonfo, e il proietto riparte
    raffica,             // CIWS o 30 mm (AK-630, AK-230): `colpi` micro-scoppi a `intervallo_s`
    colpo_testa,         // la testa del periscopio o un albero colpiti (pos = il battello)
    passaggio,           // il proietto passa vicino alla plancia: pos = il punto della traiettoria piu' vicino
    illuminante          // proietto illuminante a paracadute: pos e quota_m dell'apertura (piccolo scoppio d'espulsione)
};
const char* nome_evento_artiglieria(TipoEventoArtiglieria t);

struct EventoArtiglieria {
    double tempo_s = 0.0;          // s di missione, all'evento (partenza: allo sparo; scoppio e tonfo: all'impatto)
    TipoEventoArtiglieria tipo = TipoEventoArtiglieria::scoppio_superficie;
    Vec2 pos;
    double quota_m = 0.0;          // scoppio_aria e illuminante: altezza sopra l'acqua; per gli altri 0
    double calibro_mm = 127.0;
    double carica_kg = 3.5;        // TNT equivalente dello scoppio (0 per l'inerte e per la partenza)
    int colpi = 1;                 // raffica: i micro-scoppi; i binati mandano due eventi
    double intervallo_s = 0.0;     // raffica: fra un colpo e l'altro (AK-630 ~0,013 s)
    double velocita_m_s = 0.0;     // passaggio: velocita' del proietto (oltre 340 lo schiocco supersonico)
    double durata_s = 0.0;         // illuminante: quanto resta acceso dall'apertura (5"/38: ~50 s, 22 §6.3); 0 per gli altri
    int tiratore = -1, bersaglio = -1;   // id delle unita', per il debriefing
};

// Il picco in acqua di uno scoppio a `quota_scoppio_m` sotto la superficie, sentito a `distanza_m` e
// `quota_ricevitore_m` (22 §7.1): Cole, P = 52,4 (W^1/3 / R)^1,13 MPa e theta = 0,084 W^1/3 (W^1/3 / R)^-0,23 ms, per
// lo specchio di Lloyd 1 - exp(-dt / theta) con dt = 2 d_s d_r / (R c). dB re 1 uPa
double picco_scoppio_db(double tnt_kg, double distanza_m, double quota_ricevitore_m, double quota_scoppio_m = 0.5);

// Le sorgenti della scena di un evento (voci [transitori.art_*] di acustica.toml), con id da `id_base` (al piu' 7:
// la raffica a pezzi di 0,5 s); vuoto per il passaggio e l'illuminante, che in acqua non si sentono
std::vector<Transitorio> transitori_artiglieria(const EventoArtiglieria& e, int id_base, std::uint64_t seme);

}  // namespace m688
