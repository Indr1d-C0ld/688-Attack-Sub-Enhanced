#pragma once
// Acustica dell'ASW aereo (docs/13_asw_aereo.md, pezzo A2; dossier docs/ricerca/17_asw_aereo.md §4 e §8.3): boe e
// sonar a immersione come ricevitori dell'equazione del sonar (nucleo/acustica.hpp), aerei ed elicotteri come
// sorgenti che entrano in acqua solo nella finestra di Snell, e i transitori impulsivi (tonfi, ping, esplosive, SUS)
// come sorgenti della libreria [transitori] di acustica.toml.
//
// Il modulo e' puro e deterministico: la missione gli passa le sorgenti, l'ambiente e il Casuale& (il flusso
// caso_asw_), e riceve le Rivelazione per il controllore (nucleo/asw_aereo.hpp) o le Sorgente da mettere nella scena
// acustica (sonar del giocatore, cascata). Nessuna estrazione per la scoperta: decide l'eccesso di segnale; il caso
// serve solo agli errori di rilevamento e distanza.
//
// Il costo: un ascolto delle boe e' una valutazione per coppia boa-sorgente (livello A, o il campo dei raggi della
// sorgente se la missione passa la sua cache); il ritmo (ogni 5 s) e la scelta delle sorgenti (i sottomarini e
// quelle di falso allarme) sono della missione.

#include <cstdint>
#include <functional>
#include <map>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "nucleo/acustica.hpp"
#include "nucleo/asw_aereo.hpp"

namespace m688 {

// --- ricevitori ---

// Il sensore di una boa (DatiBoa::sensore); nullptr se la boa non ascolta (batitermografo, esplosiva, radar-faro)
const Sensore* sensore_boa(const CatalogoAcustico& c, const DatiBoa& d);
// La boa come ricevitore all'istante t: posizione con la deriva, quota dell'idrofono, velocita' della deriva
Ricevitore ricevitore_boa(const Sonoboa& b, const DatiBoa& d, const CatalogoAcustico& c, double t);
// Quota del trasduttore del sonar a immersione: il cavo filato meno l'hover (mai sopra la superficie)
double quota_trasduttore_m(double cavo_m, double hover_m);
// Il trasduttore sotto l'elicottero in hover; attivo: il sensore della voce, passivo: la voce `<sensore>_passivo`
// (o la stessa, se manca). Sensore nullptr se il catalogo non lo conosce.
Ricevitore ricevitore_dip(Vec2 pos, double cavo_m, const DatiSonarImmersione& d, const CatalogoAcustico& c, bool attivo);

// --- rivelazioni per il controllore ---

// Da una valutazione del motore alla Rivelazione: rilevamento vero dal ricevitore con l'errore del sensore (misurato,
// o quello del fascio; nessuno per le omni), distanza per gli attivi con il loro errore. nullopt se SE < 0.
std::optional<Rivelazione> rivelazione(const Rilevazione& x, const Sensore& s, const Ricevitore& r, const Sorgente& sorgente,
                                       int ricevitore_id, double t, bool sottomarino, Casuale& caso);

// Soglia delle boe sovietiche (§4.2): la boa trasmette solo quando il segnale resta sopra soglia per `soglia_s`; i
// picchi piu' brevi (tonfi, ping, un passaggio veloce) non la fanno trasmettere. Una per coppia boa-sorgente.
struct SogliaBoa {
    double da = -1.0;                       // istante in cui il segnale e' salito sopra soglia (-1: sotto)
    // `sopra`: SE >= 0 in questo ascolto; `passo_s`: il ritmo degli ascolti (il segnale vale per tutto il passo)
    bool passa(double t, bool sopra, double soglia_s, double passo_s);
};

// Una sorgente per gli ascolti, con la verita' per la classificazione
struct SorgenteAsw {
    Sorgente s;
    bool sottomarino = false;
};
struct StatoAscoltoBoe {
    std::map<std::pair<int, int>, SogliaBoa> soglie;   // (boa, sorgente)
    double passo_s = 5.0;                              // il ritmo degli ascolti
    double distanza_max_m = 0.0;                       // > 0: le coppie piu' lontane non si valutano (costo)
};
using AmbienteFra = std::function<Ambiente(Vec2 da, Vec2 a)>;

// Un ascolto passivo delle boe all'istante t: per ogni boa in ascolto (boa_in_ascolto, mare entro DatiBoa::mare_max)
// col sensore passivo, ogni sorgente sopra soglia (e, per le sovietiche, oltre la soglia di tempo) da' una Rivelazione.
// Le boe attive non ascoltano qui: ping_attivo. `campi`: la cache dei raggi della missione (per sorgente), facoltativa.
// `attenuazione_db` (il vetrino di Dio, docs/28 D1): un guasto del processore acustico dell'aereo, sommato all'eccesso
// di segnale (20 log10 del fattore; <= -1e8: non sente niente)
std::vector<Rivelazione> ascolta_boe(const std::vector<Sonoboa>& boe, const DatiAsw& d, const CatalogoAcustico& c,
                                     const std::vector<SorgenteAsw>& sorgenti, const AmbienteFra& ambiente, double t,
                                     int mare, Casuale& caso, StatoAscoltoBoe& stato, CacheCampiAsincrona* campi = nullptr,
                                     double attenuazione_db = 0.0);
// Un ascolto del ricevitore (passivo: le sorgenti; attivo: l'eco di un ping, monostatico). `id`: la boa o l'aereo.
std::vector<Rivelazione> ascolta(const Ricevitore& r, int id, const std::vector<SorgenteAsw>& sorgenti,
                                 const AmbienteFra& ambiente, double t, Casuale& caso, CacheCampiAsincrona* campi = nullptr,
                                 double attenuazione_db = 0.0);

// --- aerei ed elicotteri (§8.3) ---

// Semilarghezza orizzontale della zona in cui un ricevitore a quota d sente un aereo a quota h (regola di gioco del
// dossier: x = 0,18 h + 1,2 d dalla finestra di 13 gradi); verso la superficie il mare mosso la allunga (a quota
// periscopica 10-30 s di ascolto: STIMA 250 m per stato del mare, che sparisce a 60 m)
double finestra_snell_m(double quota_aereo_m, double quota_ricevitore_m, int mare = 3);
// Probabilita' che il sorvolo diventi l'evento «aereo in sorvolo»: 0,95 a quota periscopica, 0,8 in profondita'
double probabilita_sorvolo(double quota_ricevitore_m);
// Voce di [transitori] per la classe dell'aereo (p3c -> aereo_p3c, sh60b -> elicottero_sh60...); "" se nessuna
std::string transitorio_aereo(const std::string& classe);

struct AereoAcustico {
    int id = -1;
    std::string classe;
    Vec2 pos;
    double quota_m = 0.0, rotta = 0.0, velocita_kn = 0.0;
    bool hover = false;
    std::uint64_t seme = 0;
};
// La sorgente dell'aereo per un ricevitore in (pos, quota): solo nella finestra di Snell, appena sotto la superficie
// nel punto sotto l'aereo, col livello della libreria (riferito a 91 m di quota) scalato di -20 log(h / 91)
std::optional<Sorgente> sorgente_aerea(const CatalogoAcustico& c, const AereoAcustico& a, Vec2 ricevitore,
                                       double quota_ricevitore_m, double tempo_s, double finestra_s = 2.0, int mare = 3);
// Rotore e spruzzi dell'elicottero in hover: rumore di superficie che entra in acqua senza finestra
std::optional<Sorgente> sorgente_hover(const CatalogoAcustico& c, const AereoAcustico& a, double tempo_s,
                                       double finestra_s = 2.0);

// --- transitori impulsivi ---

struct Transitorio {
    int id = -1;                            // id della sorgente nella scena (unico)
    std::string tipo;                       // voce di [transitori] (tonfo_boa, ping_aqs13b, esplosione_mgab, ...)
    Vec2 pos;
    double quota_m = 0.0;
    double t0 = 0.0;                        // istante 0 del transitorio
    int origine = -1;                       // chi l'ha prodotto (aereo, boa), per il debriefing
    std::uint64_t seme = 0;
    double guadagno_db = 0.0;               // sul livello della voce (i colpi successivi di un fulmine)
};
// La sorgente del transitorio al tempo t, mediata sulla finestra che finisce a t; nullopt prima di t0, dopo la vita
// o se il tipo non c'e'
std::optional<Sorgente> sorgente_transitoria(const CatalogoAcustico& c, const Transitorio& x, double tempo_s,
                                             double finestra_s = 2.0);
bool transitorio_finito(const CatalogoAcustico& c, const Transitorio& x, double tempo_s);
// Il ping di un sensore attivo di boa o di sonar a immersione (aqs13b -> ping_aqs13b, ssq62b -> ping_dicass); ""
std::string transitorio_ping(const std::string& sensore);

// Chi ascolta un transitorio (il giocatore, un battello dell'IA): il migliore dei suoi ricevitori, col rilevamento
// vero e l'errore del sensore (la missione ne estrae l'errore col suo flusso)
struct AscoltoTransitorio {
    bool sentito = false;
    double se_db = -1e9;
    double rilevamento = 0.0, errore_deg = 0.0;
    std::string sensore;
};
AscoltoTransitorio ascolta_transitorio(const Sorgente& s, const std::vector<Ricevitore>& ricevitori, const Ambiente& amb);

}  // namespace m688
