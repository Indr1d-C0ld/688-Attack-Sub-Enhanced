#pragma once
// Il ricevitore d'intercettazione acustica e l'allarme siluro (blocco 2 del sonar, E; docs/22_sonar_blocco2.md §2).
//
// Il ricevitore sente gli impulsi attivi altrui a sola andata (sonar di scafo, a immersione, boe attive, teste dei
// siluri, BQS-15), li misura (frequenza col Doppler, durata, livello, rilevamento, CW o FM), li lega in tracce per
// emettitore (cadenza, andamento del livello, deriva del rilevamento), li classifica e accende l'allarme siluro.
//
// Fonti: docs/ricerca/28_intercettazione_righe_proprie.md
// - §1.3: il WLR-9 a 4 bande (1-6, 6-15, 15-45, 45-100 kHz), rilevamento +-10-15 gradi in B1 e +-5-8 nelle altre,
//   risoluzione 100 Hz, soglia circa 10 dB in 1 kHz: tutto STIMA, nessun numero aperto;
// - §2: i sovietici (Kazakov 2019): lo «Skat» misura tutto (anche la deviazione FM), il «Rubin» solo frequenza e
//   rilevamento, con molti falsi allarmi;
// - §3.3: l'allarme dai brevetti USN (STIMA): testa d'arma = banda alta, impulso breve, cadenza breve; acquisizione =
//   cadenza in calo, livello in salita, rilevamento fermo, Doppler positivo.
//
// Il modulo e' puro e ripetibile: gli errori di misura vengono dal seme e dall'istante. Dati: [intercettatori.*] e
// [emissioni.*] di acustica.toml, piu' le teste dei siluri dai [transitori.ping_testa_*].

#include <cstdint>
#include <deque>
#include <map>
#include <string>
#include <vector>

#include "nucleo/acustica.hpp"

namespace m688 {

// --- dati ---
struct BandaIntercettazione {
    double f_lo_hz = 0.0, f_hi_hz = 0.0;
    double errore_deg = 10.0;                // scarto del rilevamento misurato
};
struct DatiIntercettatore {
    std::string id;                          // wlr9, skat, rubin...
    std::vector<BandaIntercettazione> bande;
    double soglia_db = 10.0;                 // S/N in 1 kHz per un intercetto
    double risoluzione_hz = 100.0;
    bool misura_durata = true, misura_fm = false;   // senza la durata, niente cadenza ne' testa d'arma sicura
    double falsi_allarmi_h = 0.0;            // intercetti spuri all'ora (impulsi del proprio battello)
    std::vector<std::string> classi;         // le classi che lo portano
    std::string fonte;
};
enum class TipoEmissione : std::uint8_t { scafo, immersione, boa, testa, ricerca_hf, ignoto };
const char* nome_tipo_emissione(TipoEmissione t);
struct DatiEmissione {
    std::string id;                          // il sensore attivo (sqs53_attivo...) o il transitorio della testa
    TipoEmissione tipo = TipoEmissione::scafo;
    double f_hz = 3500.0, durata_s = 0.5;
    bool fm = false;
    double pri_s = 20.0;                     // cadenza tipica (le teste: dalla distanza, nella missione)
};
struct CatalogoIntercettazione {
    std::map<std::string, DatiIntercettatore> intercettatori;
    std::map<std::string, DatiEmissione> emissioni;
    const DatiIntercettatore* intercettatore_di(const std::string& classe) const;   // nullptr: la classe non ne ha
    const DatiEmissione* emissione(const std::string& id) const;
};
CatalogoIntercettazione carica_intercettazione(const std::string& percorso);   // acustica.toml; std::runtime_error

// --- un impulso in mare (lo compone la missione da quello che emette a ogni passo) ---
struct EmissioneAttiva {
    int emettitore = -1;                     // l'unita' (per la verita' e per le prove; il ricevitore non la vede)
    std::string chiave;                      // DatiEmissione::id
    Vec2 pos;
    double quota_m = 0.0;
    double rotta = 0.0, velocita_kn = 0.0;   // per il Doppler
    double t = 0.0;                          // istante dell'impulso
    double sl_db = 220.0;                    // verso il ricevitore (lobi compresi: lo decide chi emette)
    double f_hz = 0.0;                       // 0: quella del catalogo
};

// --- quello che il ricevitore misura ---
struct Intercetto {
    double t = 0.0;
    int banda = -1;
    double f_hz = 0.0;                       // misurata, col Doppler a sola andata
    double durata_s = -1.0;                  // < 0 non misurata
    double livello_db = 0.0;                 // sopra la soglia
    double rilevamento = 0.0;                // vero, con l'errore della banda
    int fm = -1;                             // 1 FM, 0 CW, -1 non misurato
    bool spurio = false;                     // falso allarme (il proprio battello)
    int emettitore = -1;                     // verita' (per le prove e il debriefing)
};
enum class Allarme : std::uint8_t { nessuno, siluro_in_acqua, siluro_in_acquisizione };
const char* nome_allarme(Allarme a);
struct TracciaIntercetto {
    int id = -1;
    std::vector<Intercetto> intercetti;      // gli ultimi (al piu' 12)
    double f_hz = 0.0, rilevamento = 0.0, durata_s = -1.0, livello_db = 0.0;
    double pri_s = -1.0;                     // cadenza stimata (mediana degli ultimi intervalli); < 0 ignota
    double deriva_deg_min = 0.0;             // del rilevamento (la retta sull'ultimo minuto)
    double incertezza_deriva_deg_min = 0.0;  // il suo errore, dall'errore di rilevamento della banda
    double salita_db_10s = 0.0;              // del livello
    int doppler = 0;                         // +1 si avvicina, -1 si allontana, 0 non si sa
    int fm = -1;
    TipoEmissione classe = TipoEmissione::ignoto;
    Allarme allarme = Allarme::nessuno;
    double ultimo_t = 0.0;
};

class RicevitoreIntercettazione {
public:
    RicevitoreIntercettazione(const DatiIntercettatore& dati, const CatalogoIntercettazione& catalogo, std::uint64_t seme);
    // Ascolta gli impulsi di un passo (t: adesso). `r`: posizione, quota, rotta e velocita' del battello, col sensore
    // d'intercettazione per il rumore proprio. Restituisce gli intercetti nuovi; aggiorna tracce, storia e allarme
    std::vector<Intercetto> ascolta(const std::vector<EmissioneAttiva>& emissioni, const Ricevitore& r, const Ambiente& amb,
                                    double t, double dt_s);
    const std::vector<TracciaIntercetto>& tracce() const { return tracce_; }
    const std::deque<Intercetto>& storia() const { return storia_; }   // gli ultimi 30 minuti, per la cascata
    Allarme allarme() const;                                            // il piu' grave fra le tracce vive
    const DatiIntercettatore& dati() const { return dati_; }

private:
    void lega(const Intercetto& x, double t);
    void valuta_traccia(TracciaIntercetto& tr) const;
    const DatiIntercettatore& dati_;
    const CatalogoIntercettazione& catalogo_;
    std::uint64_t seme_;
    std::vector<TracciaIntercetto> tracce_;
    std::deque<Intercetto> storia_;
    int prossima_ = 0;
    double ultimo_spurio_t_ = 0.0;
};

}  // namespace m688
