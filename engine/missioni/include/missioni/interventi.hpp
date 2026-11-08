#pragma once
// Il «vetrino di Dio» (docs/28_vetrino_dio.md, D1; nota condivisa runtime-interventi-contratto.md): lo spettatore
// interviene dall'esterno durante il vetrino. Un intervento e' un fatto del mondo (una nave in piu', un'avaria, un ordine),
// non un potere sull'IA: le unita' lo scoprono coi loro sensori. Solo nell'Enhanced e senza giocatore ai comandi
// (Missione::sandbox). Deterministico: tutti si applicano all'inizio del passo (Missione::avanza, dopo tempo_ e prima di
// script, IA, sensori e danni), in ordine d'id; la fine di quelli con durata nello stesso punto.
// Il catalogo e' generico per il pannello di trailer-1: i tipi nuovi della D3 e della D4 entrano senza toccarlo.

#include <map>
#include <optional>
#include <string>
#include <vector>

#include "nucleo/geometria.hpp"
#include "nucleo/meteo.hpp"

namespace m688 {

// L'innesco di un intervento su evento (D2): un evento del registro (il suo nome stabile, nome_evento: classificazione_ia,
// lancio_ia, colpo...) con l'unita' principale e l'altra (-1: qualunque), o l'ingresso di un'unita' in una zona («zona»:
// l'unita' entro raggio_m dal punto). Scatta all'inizio del passo dopo l'evento, come le regole delle missioni
struct Innesco {
    std::string evento;
    int unita = -1, altra = -1;
    std::optional<Vec2> punto;
    double raggio_m = 0.0;
};

struct Intervento {
    int id = -1;                     // lo assegna la missione (progressivo, stabile nel replay); -1 in ingresso
    double tempo = -1.0;             // s di missione: -1 = subito (il prossimo passo); >= 0 = a tempo
    std::string tipo;                // nome stabile del catalogo
    int unita = -1;                  // -1: nessuna
    std::optional<Vec2> punto;       // unita_nuova, «vai a» dell'ordine, meteo nella zona
    double raggio_m = 0.0;           // 0: tutto il teatro (meteo)
    std::map<std::string, double> parametri;      // nome -> numero
    std::map<std::string, std::string> testi;     // nome -> testo (classe, parte, gestore, sensore...)
    double durata_s = 0.0;           // 0 = per sempre; poi si torna come prima
    std::string riga_it, riga_en;    // la riga del registro, composta dalla missione
    std::optional<Innesco> innesco;  // su evento: scatta quando l'innesco e' vero, non prima di `tempo` (se >= 0)
};

struct EsitoIntervento {
    bool ok = false;
    std::string motivo_it, motivo_en;   // se non e' accettato
    int id = -1;
};

struct StatoIntervento {
    int id = -1;
    double inizio = -1.0, fine = -1.0;   // s di missione dell'applicazione e della fine (-1: non ancora)
    bool applicato = false, finito = false, fallito = false;   // fallito: al momento dell'applicazione non valeva piu'
    std::string motivo_it, motivo_en;    // di fallito
    std::size_t eventi_visti = 0;        // l'innesco: gli eventi del registro gia' guardati (quelli di prima non contano)
};

// gli inneschi che il pannello puo' offrire (D2): l'evento, il testo, che cosa chiede
struct VoceInnesco {
    std::string evento, testo_it, testo_en;
    bool vuole_unita = false, vuole_altra = false, vuole_punto = false;
};
const std::vector<VoceInnesco>& catalogo_inneschi();

// --- il catalogo ---
struct ParametroIntervento {
    std::string nome, testo_it, testo_en;
    double minimo = 0.0, massimo = 1.0, passo = 1.0, predefinito = 0.0;
    bool facoltativo = false;        // assente: non cambia (l'ordine: solo quello che si da')
};
struct TestoIntervento {
    std::string nome, testo_it, testo_en;
    std::vector<std::string> scelte;   // vuoto: libero (la classe: le chiavi di classi.toml)
    bool facoltativo = false;
};
struct VoceCatalogoIntervento {
    std::string tipo, testo_it, testo_en;
    std::string gruppo;              // FORZE | GUASTI | AMBIENTE | COMUNICAZIONI | INGANNO (il primo passo del pannello)
    bool vuole_unita = false;
    std::vector<std::string> categorie;   // nave, sottomarino, aereo (vuoto: tutte)
    bool vuole_punto = false, punto_facoltativo = false, vuole_raggio = false, ha_durata = true;
    std::vector<ParametroIntervento> parametri;
    std::vector<TestoIntervento> testi;
};
// Il parametro `nome` dell'ultimo intervento `tipo` applicato e non finito sull'unita' (con `sensore`: solo quelli col
// testo sensore uguale); nullopt se non ce n'e'. Per i guasti, letti dove servono (sensori, lanci, fine passo)
std::optional<double> parametro_attivo(const std::vector<Intervento>& v, const std::vector<StatoIntervento>& s, int unita,
                                       const std::string& tipo, const std::string& nome, const std::string& sensore = {});

// Le zone del meteo (intervento «meteo» con punto e raggio): i campi dati sopra il meteo del teatro nel punto p
void zone_meteo(const std::vector<Intervento>& v, const std::vector<StatoIntervento>& s, Vec2 p, StatoMeteo& m);

struct Sensore;
// L'eccesso di segnale tolto al sensore `x` dell'unita' da un guasto «sensore» attivo (schiera per le lineari, sonar per
// le altre): a frazione f della portata 20 log10 f dB (la perdita sferica: meta' portata -6 dB); 0 senza guasto
double attenuazione_guasto(const std::vector<Intervento>& v, const std::vector<StatoIntervento>& s, int unita, const Sensore& x);

// Il disturbo della radio di una parte (D3, intervento disturbo_radio, docs/28 §3.4): per un messaggio della parte `lato`
// trasmesso dal punto p, la perdita in piu' (0..1) e il fattore sui ritardi; senza disturbo {0, 1}. Con un punto e un
// raggio vale per chi trasmette dentro la zona. Piu' disturbi insieme: perdite indipendenti, fattori moltiplicati
struct DisturboRadio {
    double perdita = 0.0, ritardo = 1.0;
    bool attivo() const { return perdita > 0.0 || ritardo != 1.0; }
};
DisturboRadio disturbo_radio(const std::vector<Intervento>& v, const std::vector<StatoIntervento>& s, int lato, Vec2 p);

const std::vector<VoceCatalogoIntervento>& catalogo_interventi();
const VoceCatalogoIntervento* voce_intervento(const std::string& tipo);   // nullptr se il tipo non c'e'

}  // namespace m688
