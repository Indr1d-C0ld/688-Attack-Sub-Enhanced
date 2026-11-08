#pragma once
// La volta del ghiaccio vista dal battello (opzione A di sotto-ghiaccio-piano.md, punti 2-4; dossier docs/ricerca/06
// §1.1 e §5): il sondatore verso l'alto del BQS-15 e il quadro della volta.
// - Il sondatore: un fascio di 12 gradi verso l'alto, fino a 1000 m, una misura al secondo. Misura la distanza dalla
//   volta: il pescaggio piu' profondo nel cono (la chiglia vicina si vede prima di esserci sotto, come il fondo
//   nell'ecoscandaglio). Il ghiaccio sottile e liscio si confonde con l'acqua libera (esperienza del 1962: «non
//   distingue bene 3 ft di ghiaccio liscio dall'acqua libera»): sotto `liscio_m` lo prende per acqua con
//   `p_scambio` (STIMA). Errore come l'ecoscandaglio ad alta frequenza (STIMA: 0,5 m + 1%).
// - Il quadro della volta: per cella il pescaggio piu' profondo misurato e quante volte la cella e' sembrata acqua
//   libera; le polynye candidate sono le celle viste libere (per l'emersione), con la loro eta' (il pack deriva: una
//   polynya di un'ora fa non e' piu' li', a meno di correggere per la deriva stimata).
// - Gli avvisi dalle sole misure, come per il fondo (QuadroFondale::avvisi): la volta bassa sopra il sondatore, la volta
//   che scende (due sondaggi recenti), la chiglia che arriva alla quota a prua entro il tempo di reazione, la velocita'
//   sicura dalla portata vista dall'ultima spazzata.
// Le misure arrivano in posizione stimata, come per il fondo; sotto la banchisa non c'e' punto nave, quindi il quadro non
// si corregge. Il quadro si salva e si ricarica con la missione (il campo del ghiaccio si rifa' dal seme). Modulo puro.

#include <cstdint>
#include <iosfwd>
#include <map>
#include <optional>
#include <vector>

#include "nucleo/campo_ghiaccio.hpp"
#include "nucleo/casuale.hpp"
#include "nucleo/ritorni_fondo.hpp"

namespace m688 {

struct ParametriSondatoreAlto {
    double cono_deg = 12.0;
    double portata_m = 1000.0;
    double liscio_m = 1.0;          // sotto questo spessore livellato il ghiaccio liscio...
    double p_scambio = 0.4;         // ...sembra acqua libera con questa probabilita' (STIMA)
    double errore_fisso_m = 0.5, errore_rel = 0.01;
};

struct SondaggioAlto {
    double t = 0.0;
    Vec2 pos;                       // stimata
    double quota_m = 0.0;
    bool eco = false;               // la volta entro la portata (altrimenti niente)
    bool libera = false;            // sembrata acqua libera (vera o il liscio scambiato)
    double distanza_m = 0.0;        // dalla volta (con libera: dalla superficie)
    double pescaggio_m = 0.0;       // quota - distanza (0 con libera)
    double sigma_m = 0.0;
    // verita', per le prove e il debriefing
    bool scambio = false;
    double pescaggio_vero_m = 0.0;
};

SondaggioAlto sonda_alto(const CampoGhiaccio& g, Vec2 pos_vera, Vec2 pos_stimata, double quota_m, double t,
                         const ParametriSondatoreAlto& p, Casuale& caso);

// Il BQS-15 sulle chiglie (punto 3; dossier 06 §1.1: chiglie di ghiaccio a ~2,6 km): la spazzata del settore di prua
// verso la volta. Su ogni raggio la prima chiglia (o un pescaggio di almeno `pescaggio_min_m`) entro la portata ridotta
// dalla velocita' come per il fondo (portata_effettiva), dentro il ventaglio d'elevazione, e, se quella non e' un
// ostacolo, anche il primo ostacolo piu' in la' (una chiglia che arriva alla quota del battello meno `margine_m`).
// Errori del sonar: 1 grado e 5 m
inline constexpr double PORTATA_CHIGLIA_M = 2500.0;   // dossier 06 §1.1: le chiglie a ~2,6 km
inline constexpr double CHIGLIA_688I = 1.2;           // il 688I (Flight III, BQS-15 migliorato): +20%
// la portata sulle chiglie del sonar (il fattore dei sonar sovietici compreso), prima del rumore proprio
double portata_chiglia_m(const ParametriBqs15Fondo& p, bool migliorato = false);
struct RitornoChiglia {
    double t = 0.0;
    Vec2 origine;                   // stimata
    double rilevamento = 0.0, distanza_m = 0.0;
    Vec2 punto;                     // stimata
    double pescaggio_m = 0.0;
    bool ostacolo = false;
};
std::vector<RitornoChiglia> spazza_ghiaccio(const CampoGhiaccio& g, Vec2 pos_vera, Vec2 pos_stimata, double quota_m, double rotta,
                                            double velocita_kn, double t, const ParametriBqs15Fondo& p, Casuale& caso,
                                            double portata_chiglia_m = 2500.0, double margine_m = 10.0,
                                            double pescaggio_min_m = 3.0);

// I falsi echi delle chiglie per un attivo (la testa d'un siluro, il sonar attivo; opzione A, punto 5; dossier 06 §5.1):
// le creste del campo vero che stanno nel settore `settore_deg` attorno alla rotta, entro `portata_m` e dentro il ventaglio
// d'elevazione ±`elevazione_deg` dalla quota dell'attivo, la piu' vicina per raggio (ogni 5 gradi). La forza di bersaglio
// cresce col pescaggio: 0 dB a 10 m, +6 dB a ogni raddoppio, fra -15 e +10 dB (STIMA: una cresta e' un riflettore rugoso
// grande come uno scafo). Il motore le presenta alle teste come bersagli fermi, come le esche
struct EcoChiglia {
    Vec2 pos;
    double pescaggio_m = 0.0, ts_db = 0.0, distanza_m = 0.0;
};
double forza_bersaglio_chiglia_db(double pescaggio_m);
// `pescaggio_min_m`: solo le creste almeno cosi' profonde (le piu' piccole sono la volta, gia' nella sua riverberazione)
std::vector<EcoChiglia> echi_chiglie(const CampoGhiaccio& g, Vec2 pos, double quota_m, double rotta, double settore_deg,
                                     double portata_m, double elevazione_deg, double t, double passo_m = 50.0,
                                     double pescaggio_min_m = 0.0);

struct AvvisoVolta {
    enum class Tipo : std::uint8_t { volta_bassa, volta_in_discesa, chiglia_a_prua, velocita_eccessiva };
    Tipo tipo = Tipo::volta_bassa;
    double valore = 0.0;                // m dalla volta, m di discesa, m alla chiglia, kn sicuri
    double t = 0.0;                     // della misura che lo dice
};

struct CellaVolta {
    double pescaggio_max_m = 0.0;   // il piu' profondo misurato
    int misure = 0, libere = 0;
    double ultima_t = -1.0, ultima_libera_t = -1.0;
};

class QuadroVolta {
public:
    explicit QuadroVolta(double cella_m = 200.0) : cella_(cella_m) {}
    void aggiungi(const SondaggioAlto& s);
    void aggiungi(const RitornoChiglia& r);   // la chiglia nella cella del suo punto
    // una spazzata intera del BQS-15: i ritorni e la portata effettiva che ha guardato (per la velocita' sicura)
    void aggiungi_spazzata(double t, double portata_m, const std::vector<RitornoChiglia>& ritorni);
    std::optional<CellaVolta> leggi(Vec2 p) const;
    // le celle viste libere, la piu' recente prima, entro `raggio_m` da p e non piu' vecchie di `eta_max_s`
    struct Candidata { Vec2 centro; double eta_s; int libere, misure; };
    std::vector<Candidata> polynye(Vec2 p, double raggio_m, double t, double eta_max_s) const;
    // il pescaggio piu' profondo misurato lungo un tratto (per l'avviso «chiglia a prua» dalle sole misure)
    double pescaggio_max(Vec2 a, Vec2 b) const;
    std::size_t celle() const { return celle_.size(); }
    double cella_m() const { return cella_; }
    // per la vista (la pagina del fondale, kit): i sondaggi alti con l'eco degli ultimi STORIA_S, i piu' vecchi prima
    const std::vector<SondaggioAlto>& storia() const { return sondaggi_; }
    // i ritorni del BQS-15 sulle chiglie entro `finestra_s` da t (l'archivio tiene RITORNI_ARCHIVIO_S)
    std::vector<RitornoChiglia> ritorni_recenti(double t, double finestra_s) const;
    static constexpr double STORIA_S = 1800.0, RITORNI_ARCHIVIO_S = 600.0;

    // gli avvisi dalle sole misure: `quota_m` del sondatore, `margine_m` la distanza dalla volta sotto cui e' bassa;
    // la chiglia a prua entro `reazione_s` alla velocita' (almeno 1 m/s), nel settore di 30 gradi della rotta
    std::vector<AvvisoVolta> avvisi(Vec2 pos_stimata, double quota_m, double rotta, double velocita_kn, double t,
                                    double reazione_s = 180.0, double margine_m = 30.0) const;

    void salva(std::ostream& out) const;
    static QuadroVolta carica(std::istream& in);   // std::runtime_error se il formato non torna
private:
    std::int64_t chiave(Vec2 p) const;
    Vec2 centro(std::int64_t k) const;
    void dimentica(double t);           // i sondaggi e i ritorni troppo vecchi per gli avvisi
    double cella_;
    std::map<std::int64_t, CellaVolta> celle_;
    std::vector<SondaggioAlto> sondaggi_;   // con l'eco, degli ultimi STORIA_S (gli avvisi guardano l'ultimo minuto)
    std::vector<RitornoChiglia> ritorni_;   // degli ultimi RITORNI_ARCHIVIO_S (gli avvisi gli ultimi 30 s)
    double spazzata_t_ = -1.0, spazzata_portata_m_ = 0.0;
};

}  // namespace m688
