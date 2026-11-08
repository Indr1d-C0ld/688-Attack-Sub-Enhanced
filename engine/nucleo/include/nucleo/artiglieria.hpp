#pragma once
// Artiglieria navale contro il sottomarino (docs/16_artiglieria.md, pezzi G0 e G1; numeri e fonti nel dossier
// docs/ricerca/22_artiglieria_contro_sottomarini.md, citato «22»). Solo Enhanced, con la regola di simulazione.
//
// - I dati (G0): data/motore/artiglieria.toml (tools/pipeline/artiglieria.py), pezzi, classi e sagome.
// - Il tiro di un colpo (22 §9.4): mira sul bersaglio stimato con l'anticipo del tempo di volo, errore gaussiano in
//   direzione e in distanza (22 §3.6), caduta sulla sagoma vera del battello esposto, spazio pericoloso compreso.
// - L'impatto (ImpattoArtiglieria) e' il contratto per i danni (G3, kit) e per l'app (G6); gli EventoArtiglieria
//   (nucleo/acustica_artiglieria.hpp) per audio (G5) e grafica (G4).
//
// Il modulo e' puro: il caso lo passa il chiamante.

#include <cstdint>
#include <map>
#include <set>
#include <string>
#include <vector>

#include "nucleo/acustica_artiglieria.hpp"
#include "nucleo/casuale.hpp"
#include "nucleo/geometria.hpp"

namespace m688 {

// --- dati (G0) ---

enum class Spoletta : std::uint8_t { pd, vt, raffica };   // a percussione, di prossimita', raffica di 30 mm

struct PuntoVolo {                         // la tavola di volo del pezzo (22 §3.4)
    double distanza_m = 0.0, volo_s = 0.0, caduta_gradi = 0.0, velocita_m_s = 0.0;
};

struct DatiPezzo {
    std::string id, nome;
    double calibro_mm = 76.0;
    int canne = 1;                         // i binati sparano due proietti per salva
    double cadenza_min = 60.0;             // per canna (raffiche: raffiche al minuto)
    int colpi_pronti = 80;                 // poi la pausa di ricarica_s (rifornimento, raffreddamento)
    double ricarica_s = 0.0;
    int deposito = 0;                      // colpi a bordo per l'affusto, caricatore compreso (ricerca 36); 0 senza limite
    double carica_kg = 1.0;                // TNT equivalente dello scoppio
    double v0_m_s = 800.0;
    double altezza_m = 8.0;                // asse del pezzo sull'acqua
    double depressione_gradi = -10.0;
    double portata_minima_m = 0.0;         // sotto non batte il pelo dell'acqua (22 §3.3)
    double portata_superficie_m = 8000.0;  // contro il battello in superficie
    double portata_periscopio_m = 0.0;     // contro teste e alberi; 0 = mai (16")
    double reazione_combattimento_s = 15.0, reazione_condizione3_s = 90.0;   // fino al primo colpo (22 §4.2)
    double sigma_direzione_mrad = 1.5, sigma_distanza_m = 20.0, sigma_distanza_frazione = 0.005;
    double r_eff_m = 2.0;                  // schegge efficaci su teste e alberi (22 §5.3)
    Spoletta spoletta = Spoletta::pd;
    int raffica_colpi = 0;                 // proietti di una raffica
    double raffica_s = 0.0;
    std::string fonte, confidenza;
    std::vector<PuntoVolo> volo;
    // interpolati sulla tavola (da 0 con v0); oltre l'ultima riga si prolunga la pendenza
    double tempo_volo_s(double distanza_m) const;
    double caduta_gradi(double distanza_m) const;
    double velocita_arrivo_m_s(double distanza_m) const;
    double intervallo_s() const { return 60.0 / (cadenza_min > 0.0 ? cadenza_min : 1.0); }   // fra due colpi di una canna
    bool raffica() const { return spoletta == Spoletta::raffica; }
};

struct SagomaClasse {                      // la parte del battello in superficie fuori dall'acqua (22 §5.4, STIMA)
    double bordo_libero_m = 2.5;
    double falsatorre_lunghezza_m = 10.0, falsatorre_larghezza_m = 2.8, falsatorre_altezza_m = 6.0;
    double falsatorre_avanti_m = 0.0;      // centro della falsatorre a prora del centro dello scafo
};

struct TabellaArtiglieria {
    std::map<std::string, DatiPezzo> pezzi;
    std::map<std::string, std::vector<std::string>> classi;   // classe -> pezzi, uno per affusto (anche ripetuti)
    std::map<std::string, SagomaClasse> sagome;                // "_predefinita" per le classi senza voce
    const DatiPezzo* pezzo(const std::string& id) const;
    std::vector<const DatiPezzo*> pezzi_di(const std::string& classe) const;   // vuoto: disarmata
    SagomaClasse sagoma(const std::string& classe) const;
    bool vuota() const { return pezzi.empty(); }
};
// data/motore/artiglieria.toml: una tabella vuota se il file manca (l'artiglieria resta spenta); std::runtime_error se
// e' malformato
TabellaArtiglieria carica_artiglieria(const std::string& percorso);

// --- la sagoma esposta del bersaglio ---

enum class ParteEsposta : std::uint8_t { nessuna, periscopio_attacco, periscopio_ricerca, albero_esm, albero_radio,
                                         radar, snorkel, falsatorre, plancia, scafo };
const char* nome_parte(ParteEsposta p);

struct PezzoEsposto {                      // una testa o un albero: un cilindro verticale dall'acqua
    ParteEsposta parte = ParteEsposta::periscopio_attacco;
    Vec2 pos;
    double altezza_m = 1.5;                // sopra l'acqua
    double diametro_m = 0.2;
};

struct SagomaEsposta {
    int unita = -1;
    Vec2 pos;                              // centro dello scafo
    double rotta_deg = 0.0, velocita_m_s = 0.0;
    bool in_superficie = false;            // scafo, falsatorre e plancia fuori
    double lunghezza_m = 0.0, larghezza_m = 0.0, bordo_libero_m = 0.0;
    double falsatorre_lunghezza_m = 0.0, falsatorre_larghezza_m = 0.0, falsatorre_altezza_m = 0.0;   // sull'acqua
    double falsatorre_avanti_m = 0.0;
    std::vector<PezzoEsposto> alberi;      // teste e alberi alzati
    bool vuota() const { return !in_superficie && falsatorre_altezza_m <= 0.0 && alberi.empty(); }   // 22 §5.2
};

struct AlberoAlzato {                      // per sagoma_sottomarino: sulla falsatorre, all'altezza data sull'acqua
    ParteEsposta parte = ParteEsposta::periscopio_attacco;
    double altezza_m = 1.5, diametro_m = 0.2;
};
// La sagoma di un sottomarino qualunque. In superficie (quota fino a 1 m): scafo, falsatorre e plancia della classe,
// con gli alberi sopra la falsatorre. Immerso: gli alberi alla loro altezza sull'acqua e, se `vela_affiorata_m` > 0,
// la cima della falsatorre (mare grosso, docs/15 M2)
SagomaEsposta sagoma_sottomarino(const SagomaClasse& c, int unita, Vec2 pos, double rotta_deg, double velocita_m_s,
                                 double lunghezza_m, double larghezza_m, double quota_m, double vela_affiorata_m,
                                 const std::vector<AlberoAlzato>& alberi);

// Dove mira chi vede il battello: immerso la testa o l'albero piu' alto (o la vela che affiora), in superficie la
// falsatorre
Vec2 punto_di_mira(const SagomaEsposta& g);

// --- il tiro (G1) ---

struct CorrezioniTiro {                    // 22 §3.6: fattori sulla probabilita' per colpo, applicati alle sigma
    int stato_mare = 2;
    bool notte = false, illuminante = false, radar_tiro = true;
    double velocita_nave_kn = 0.0;
    bool in_accostata = false, primo_colpo = false;
    double fattore_difficolta = 1.0;       // principiante 0,5 contro la superficie (22 §9.9)
    double fattore() const;                // il prodotto (P x k: le sigma / sqrt(k))
};

struct ImpattoArtiglieria {
    int tiratore = -1, bersaglio = -1;
    std::string pezzo;                     // id di [pezzi.*]
    double calibro_mm = 0.0, carica_kg = 0.0;
    double tempo_sparo_s = 0.0, tempo_impatto_s = 0.0;
    Vec2 punto;                            // caduta sull'acqua, scoppio o colpo diretto
    double quota_m = 0.0;                  // sopra l'acqua: VT o colpo diretto in alto; 0 sull'acqua
    enum class Esito : std::uint8_t { acqua, rimbalzo, tonfo, aria, diretto, schegge } esito = Esito::acqua;
    ParteEsposta parte = ParteEsposta::nessuna;   // colpita direttamente, o la piu' vicina entro r_eff (schegge)
    double distanza_parte_m = -1.0;        // 0 diretto, o dalla parte piu' vicina; -1 se nessuna entro r_eff
    double r_eff_m = 0.0;                  // il raggio delle schegge del pezzo (DatiPezzo::r_eff_m), per i danni
    double angolo_caduta_gradi = 0.0, velocita_m_s = 0.0;
    bool avvertimento = false;             // colpo d'avvertimento (crisi, acque territoriali): mai sul bersaglio
    bool raffica = false;                  // il centro di una raffica (i proietti a segno sono impatti a parte)
    int colpi = 1;                         // raffica: i proietti
    int sparo = 0;                         // il numero dell'uscita di una canna nella partita: una raffica e un rimbalzo lo dividono
};

// I colpi subiti: uno per uscita di una canna che ha toccato, quanti che siano i proietti. Il numero viene da
// ImpattoArtiglieria::sparo, che il registro scrive nel `codice` di artiglieria_colpo; 0 nelle tracce di prima del
// 05/11/2026, dove ogni evento vale uno
class ContaSpari {
public:
    bool nuovo(int sparo) { return sparo == 0 || visti_.insert(sparo).second; }

private:
    std::set<int> visti_;
};
const char* nome_esito(ImpattoArtiglieria::Esito e);

inline constexpr double ANGOLO_RIMBALZO_GRADI = 8.0;   // sotto, una PD rimbalza meta' delle volte (22 §5.1)

// Un colpo (o una salva di una canna, o una raffica) del pezzo dalla nave sul bersaglio stimato, con la velocita'
// stimata per l'anticipo; `vera` e' la sagoma all'istante dello sparo, portata avanti col suo moto fino all'impatto.
// Gli impatti in ordine di tempo: uno, due col rimbalzo, piu' di uno con la raffica. Gli avvertimenti cadono a
// 300-500 m dal bersaglio
std::vector<ImpattoArtiglieria> spara(const DatiPezzo& p, Vec2 nave, Vec2 stimata, Vec2 velocita_stimata_m_s,
                                      const SagomaEsposta& vera, double tempo_s, const CorrezioniTiro& c, Casuale& caso,
                                      bool avvertimento = false);

// Gli eventi per audio e grafica di un impatto: la partenza alla nave (se `partenza`), poi lo scoppio, il tonfo, il
// rimbalzo, la raffica o il colpo sulla testa
std::vector<EventoArtiglieria> eventi_di(const ImpattoArtiglieria& i, Vec2 nave, bool partenza);

}  // namespace m688
