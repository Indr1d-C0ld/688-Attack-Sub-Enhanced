// Il «vetrino di Dio» (docs/28_vetrino_dio.md, D1; nota condivisa runtime-interventi-contratto.md): il catalogo degli
// interventi, l'ingresso (intervieni), la coda e il punto del passo, l'applicazione e la fine. Il registro li vede in
// stati_interventi() (evento `intervento`, solo per lo spettatore).
//
// Il punto del passo (il vincolo dei numeri casuali, docs/28 §4): passo_interventi() all'inizio di Missione::avanza,
// subito dopo tempo_ += h e prima di qualunque script, IA, sensore o danno; in ordine d'id. Gli ordini attivi si
// riaffermano a fine passo (tieni_ordini_interventi), senza numeri casuali.

#include <algorithm>
#include <cmath>
#include <cstdio>

#include "missioni/forze_missione.hpp"
#include "missioni/missione.hpp"
#include "nucleo/acustica.hpp"
#include "nucleo/classi.hpp"
#include "nucleo/meteo.hpp"

namespace m688 {

namespace {

constexpr double ROE_RETE_S = 60.0;           // il messaggio delle ROE sulla rete della parte (STIMA)
constexpr double QUOTA_BROADCAST_M = 6.096 + 0.5;   // a quota periscopica il battello ascolta il broadcast (come le forze)
constexpr double SOTTOMARINO_VELA_M = 0.1;   // una classe con la vela e' un sottomarino (classi.toml, le forme del kit)
// il falso contatto (D3): gli id delle sue unita' (fuori da quelli delle missioni e dei transitori) e la quota minima,
// sotto la quale vista e radar lo coglierebbero come un'unita' in superficie (ne' scafo ne' alberi: e' solo un suono)
constexpr int FALSI_DA = 990000, FALSI_QUANTI = 5000;
constexpr double QUOTA_FALSO_MIN_M = 2.0;
// al primo passo i sistemi dell'Enhanced non ci sono ancora (li crea DanniMissione subito dopo Missione::avanza): nei primi
// secondi della missione un intervento che li chiede aspetta che ci siano; se non arrivano, fallisce col suo motivo
constexpr double ATTESA_MONDO_S = 2.0;
constexpr int MSG_GHIACCIO_NUOVO = 499;
// La durata dell'intervento per le zone di strato e ghiaccio, che la contano dalla fine della transizione: il ritorno comincia
// allo scadere dell'intervento. 0 resta per sempre
double durata_zona(const Intervento& x, double transizione_s) { return x.durata_s > 0.0 ? std::max(x.durata_s - transizione_s, 1.0) : 0.0; }
Vec2 velocita_deriva(const Intervento& x) {
    const double r = radianti(x.parametri.at("rotta")), v = x.parametri.at("velocita_kn") * NODO;
    return {v * std::sin(r), v * std::cos(r)};
}

ParametroIntervento par(const char* nome, const char* it, const char* en, double mi, double ma, double passo, double pred, bool facoltativo = false) {
    return {nome, it, en, mi, ma, passo, pred, facoltativo};
}
TestoIntervento testo(const char* nome, const char* it, const char* en, std::vector<std::string> scelte, bool facoltativo = false) {
    return {nome, it, en, std::move(scelte), facoltativo};
}

std::vector<VoceCatalogoIntervento> crea_catalogo() {
    std::vector<VoceCatalogoIntervento> c;
    const std::vector<std::string> parti = {"usa", "urss", "neutrale"};
    {   // FORZE
        VoceCatalogoIntervento v;
        v.tipo = "unita_nuova";
        v.testo_it = "Inserisci un'unita'";
        v.testo_en = "Insert a unit";
        v.gruppo = "FORZE";
        v.vuole_punto = true;
        v.ha_durata = false;
        v.testi = {testo("classe", "classe", "class", {}), testo("parte", "parte", "side", parti),
                   testo("gestore", "comportamento (script della missione)", "behaviour (mission script)", {}, true)};
        v.parametri = {par("rotta", "rotta", "course", 0, 359, 1, 0), par("velocita_kn", "velocita' (nodi)", "speed (kn)", 0, 40, 1, 10),
                       par("quota_m", "quota (m)", "depth (m)", 0, 600, 5, 0)};
        c.push_back(v);
    }
    {
        VoceCatalogoIntervento v;
        v.tipo = "togli";
        v.testo_it = "Togli un'unita'";
        v.testo_en = "Remove a unit";
        v.gruppo = "FORZE";
        v.vuole_unita = true;
        v.ha_durata = false;
        c.push_back(v);
    }
    {
        VoceCatalogoIntervento v;
        v.tipo = "ordine";
        v.testo_it = "Ordina a un'unita'";
        v.testo_en = "Order a unit";
        v.gruppo = "FORZE";
        v.vuole_unita = true;
        v.categorie = {"nave", "sottomarino"};
        v.vuole_punto = true;
        v.punto_facoltativo = true;   // «vai a»
        v.parametri = {par("rotta", "rotta", "course", 0, 359, 1, 0, true), par("velocita_kn", "velocita' (nodi)", "speed (kn)", 0, 40, 1, 10, true),
                       par("quota_m", "quota (m)", "depth (m)", 0, 600, 5, 100, true)};
        v.testi = {testo("gestore", "comportamento (script della missione)", "behaviour (mission script)", {}, true)};
        c.push_back(v);
    }
    {   // D4: per le altre parti resta quella di prima, finche' non lo sanno (Missione::parte_nota)
        VoceCatalogoIntervento v;
        v.tipo = "cambia_parte";
        v.testo_it = "Cambia parte a un'unita'";
        v.testo_en = "Change a unit's side";
        v.gruppo = "FORZE";
        v.vuole_unita = true;
        v.categorie = {"nave", "sottomarino"};
        v.ha_durata = false;
        v.testi = {testo("parte", "parte nuova", "new side", parti),
                   testo("annuncio", "chi lo sa (predefinito: la parte nuova)", "who knows (default: the new side)", {"nessuno", "nuova_parte", "tutti"}, true)};
        c.push_back(v);
    }
    {
        VoceCatalogoIntervento v;
        v.tipo = "situazione";
        v.testo_it = "Situazione fra due parti";
        v.testo_en = "Situation between two sides";
        v.gruppo = "FORZE";
        v.ha_durata = false;   // resta finche' un altro intervento (o la partita) non la cambia
        v.testi = {testo("parte_a", "parte", "side", parti), testo("parte_b", "e la parte", "and side", parti),
                   testo("situazione", "situazione", "situation", {"pace", "crisi", "guerra"})};
        c.push_back(v);
    }
    {
        VoceCatalogoIntervento v;
        v.tipo = "roe";
        v.testo_it = "Regole d'ingaggio di una parte";
        v.testo_en = "Rules of engagement of a side";
        v.gruppo = "FORZE";
        v.ha_durata = false;
        v.testi = {testo("parte", "parte", "side", parti), testo("categoria", "contro", "against", {"sottomarini", "navi", "aerei", "tutte"}),
                   testo("regola", "regola", "rule", {"libere", "trattenute"})};
        c.push_back(v);
    }
    {
        VoceCatalogoIntervento v;
        v.tipo = "allerta";
        v.testo_it = "Allerta della forza di un'unita'";
        v.testo_en = "Alert of a unit's force";
        v.gruppo = "FORZE";
        v.vuole_unita = true;
        v.ha_durata = false;   // poi sale e scende come sempre
        v.parametri = {par("livello", "livello (A0-A3)", "level (A0-A3)", 0, 3, 1, 2)};
        c.push_back(v);
    }
    {   // GUASTI
        VoceCatalogoIntervento v;
        v.tipo = "macchine";
        v.testo_it = "Guasto alle macchine";
        v.testo_en = "Machinery failure";
        v.gruppo = "GUASTI";
        v.vuole_unita = true;
        v.categorie = {"nave", "sottomarino"};
        v.parametri = {par("velocita_max_frazione", "velocita' massima (frazione; 0 = ferma)", "maximum speed (fraction; 0 = dead in the water)", 0, 1, 0.05, 0)};
        c.push_back(v);
    }
    {
        VoceCatalogoIntervento v;
        v.tipo = "sensore";
        v.testo_it = "Guasto a un sensore";
        v.testo_en = "Sensor failure";
        v.gruppo = "GUASTI";
        v.vuole_unita = true;
        v.testi = {testo("sensore", "sensore", "sensor", {"sonar", "schiera", "radar"})};
        v.parametri = {par("fattore", "portata (frazione; 0 = spento)", "range (fraction; 0 = off)", 0, 1, 0.05, 0)};
        c.push_back(v);
    }
    {
        VoceCatalogoIntervento v;
        v.tipo = "tubi";
        v.testo_it = "Tubi o lanciatori bloccati";
        v.testo_en = "Tubes or launchers jammed";
        v.gruppo = "GUASTI";
        v.vuole_unita = true;
        v.parametri = {par("tubi", "tubi bloccati (-1 tutti)", "tubes jammed (-1 all)", -1, 10, 1, -1)};
        c.push_back(v);
    }
    {
        VoceCatalogoIntervento v;
        v.tipo = "timone";
        v.testo_it = "Timone o piani bloccati";
        v.testo_en = "Rudder or planes jammed";
        v.gruppo = "GUASTI";
        v.vuole_unita = true;
        v.categorie = {"nave", "sottomarino"};
        v.parametri = {par("timone_gradi_s", "virata bloccata (gradi/s, + a dritta)", "jammed turn (deg/s, + to starboard)", -3, 3, 0.1, 0, true),
                       par("piani_m_s", "piani bloccati (m/s, + in giu')", "jammed planes (m/s, + down)", -2, 2, 0.1, 0, true)};
        c.push_back(v);
    }
    {   // AMBIENTE
        VoceCatalogoIntervento v;
        v.tipo = "meteo";
        v.testo_it = "Meteo e mare";
        v.testo_en = "Weather and sea";
        v.gruppo = "AMBIENTE";
        v.vuole_punto = true;
        v.punto_facoltativo = true;   // col punto e un raggio: nella zona; senza, in tutto il teatro
        v.vuole_raggio = true;
        // in tutto il teatro e fermo da li', alla fine il meteo di prima (fisso, o di nuovo quello del teatro); nella zona,
        // sopra il meteo del teatro finche' dura
        v.parametri = {par("mare", "stato del mare", "sea state", 0, 9, 1, 4, true), par("vento_kn", "vento (nodi)", "wind (kn)", 0, 70, 1, 20, true),
                       par("visibilita_m", "visibilita' (m)", "visibility (m)", 100, 50000, 100, 10000, true),
                       par("nubi", "nubi (frazione)", "cloud (fraction)", 0, 1, 0.1, 0.5, true)};
        c.push_back(v);
    }
    {
        VoceCatalogoIntervento v;
        v.tipo = "ora";
        v.testo_it = "Sposta l'ora";
        v.testo_en = "Move the clock";
        v.gruppo = "AMBIENTE";
        v.ha_durata = false;
        v.parametri = {par("ore", "ore (avanti o indietro)", "hours (forward or back)", -12, 12, 0.5, 6)};
        c.push_back(v);
    }
    {   // D4: lo strato che cambia (nucleo/modifica_profilo, Missione::ZonaStrato), nella zona o in tutto il teatro. Lo stesso
        // mare per tutti; allo scadere torna indietro con la stessa transizione. Il profilo dipende da teatro e mese: nelle
        // acque polari si rifiuta (strato_applicabile)
        VoceCatalogoIntervento v;
        v.tipo = "strato";
        v.testo_it = "Strato termico";
        v.testo_en = "Thermal layer";
        v.gruppo = "AMBIENTE";
        v.vuole_punto = true;
        v.punto_facoltativo = true;
        v.vuole_raggio = true;
        v.testi = {testo("modifica", "che cosa cambia", "what changes", {"sposta", "togli", "rinforza", "canale"})};
        v.parametri = {par("quota_m", "quota dello strato o dell'asse (m)", "depth of the layer or axis (m)", 5, 4000, 5, 100, true),
                       par("fattore", "gradiente del termoclino (volte)", "thermocline gradient (times)", 0.25, 4, 0.25, 2, true),
                       par("transizione_s", "transizione (s)", "transition (s)", 0, 3600, 60, 600)};
        c.push_back(v);
    }
    {   // D4: il pack che cambia deriva (CampoGhiaccio::cambia_deriva): senza salti, e il bordo gli va dietro
        VoceCatalogoIntervento v;
        v.tipo = "ghiaccio_deriva";
        v.testo_it = "Deriva del ghiaccio";
        v.testo_en = "Ice drift";
        v.gruppo = "AMBIENTE";
        v.parametri = {par("rotta", "verso dove (gradi)", "towards (degrees)", 0, 359, 1, 90),
                       par("velocita_kn", "velocita' (nodi)", "speed (kn)", 0, 3, 0.1, 0.5)};
        c.push_back(v);
    }
    {   // D4: la polynya (CampoGhiaccio::forza_zona): si apre dal centro, o si chiude in nilas che ingrossa, ferma nel mondo
        VoceCatalogoIntervento v;
        v.tipo = "polynya";
        v.testo_it = "Polynya";
        v.testo_en = "Polynya";
        v.gruppo = "AMBIENTE";
        v.vuole_punto = true;
        v.vuole_raggio = true;
        v.testi = {testo("azione", "si apre o si chiude", "opens or closes", {"apri", "chiudi"})};
        v.parametri = {par("transizione_s", "transizione (s)", "transition (s)", 0, 3600, 60, 600)};
        c.push_back(v);
    }
    {   // COMUNICAZIONI (D3)
        VoceCatalogoIntervento v;
        v.tipo = "disturbo_radio";
        v.testo_it = "Disturbo della radio di una parte";
        v.testo_en = "Jamming a side's radio";
        v.gruppo = "COMUNICAZIONI";
        v.vuole_punto = true;
        v.punto_facoltativo = true;   // col punto e un raggio: chi trasmette nella zona; senza, tutta la parte
        v.vuole_raggio = true;
        v.testi = {testo("parte", "parte", "side", parti)};
        v.parametri = {par("perdita", "messaggi persi in piu' (frazione)", "extra messages lost (fraction)", 0, 1, 0.05, 0.5),
                       par("ritardo", "ritardi moltiplicati per", "delays multiplied by", 1, 10, 0.5, 2)};
        c.push_back(v);
    }
    {
        VoceCatalogoIntervento v;
        v.tipo = "ordine_falso";
        v.testo_it = "Falso ordine radio a una parte";
        v.testo_en = "False radio order to a side";
        v.gruppo = "COMUNICAZIONI";
        v.vuole_punto = true;
        v.punto_facoltativo = true;   // per «vai a»
        // il catalogo resta aperto ad altri ordini: chi non conosce l'ordine lo ignora
        v.testi = {testo("parte", "parte che lo riceve", "receiving side", parti),
                   testo("ordine", "ordine", "order", {"rientro", "vai_a", "roe_trattenute", "roe_libere"})};
        v.parametri = {par("autenticazione", "quanto e' credibile (1 = come un ordine vero)", "how credible (1 = like a real order)", 0, 1, 0.05, 0.5)};
        c.push_back(v);
    }
    {   // INGANNO (D3)
        VoceCatalogoIntervento v;
        v.tipo = "incidente";
        v.testo_it = "Incidente provocato";
        v.testo_en = "Staged incident";
        v.gruppo = "INGANNO";
        v.vuole_punto = true;
        v.ha_durata = false;
        // lo scoppio senza arma; il siluro «di nessuno» (la parte creduta, se si vuole, e un'unita' bersaglio facoltativa)
        v.testi = {testo("incidente", "che cosa", "what", {"scoppio", "siluro"}),
                   testo("parte", "parte creduta del siluro", "believed side of the torpedo", parti, true),
                   testo("arma", "siluro (classe di siluri.toml)", "torpedo (siluri.toml class)", {}, true)};
        v.parametri = {par("kg", "carica (kg)", "charge (kg)", 1, 1000, 10, 100, true), par("quota_m", "quota (m)", "depth (m)", 0, 600, 5, 30, true),
                       par("rotta", "rotta del siluro", "torpedo course", 0, 359, 1, 0, true)};
        c.push_back(v);
    }
    {   // D4: la false flag
        VoceCatalogoIntervento v;
        v.tipo = "false_flag";
        v.testo_it = "False flag: un attacco con la parte di un altro";
        v.testo_en = "False flag: an attack under another side's colours";
        v.gruppo = "INGANNO";
        v.vuole_unita = true;   // chi attacca davvero
        v.categorie = {"nave", "sottomarino"};
        v.ha_durata = false;
        v.testi = {testo("parte", "parte che sembra", "apparent side", parti),
                   testo("arma", "siluro (classe di siluri.toml)", "torpedo (siluri.toml class)", {}, true)};
        v.parametri = {par("bersaglio", "bersaglio (id dell'unita')", "target (unit id)", 0, 1e6, 1, 0)};
        c.push_back(v);
    }
    {
        VoceCatalogoIntervento v;
        v.tipo = "spia";
        v.testo_it = "Una spia: una parte vede il quadro vero dell'altra";
        v.testo_en = "A spy: one side sees the other's true picture";
        v.gruppo = "INGANNO";
        v.testi = {testo("parte", "parte che riceve", "receiving side", parti), testo("altra", "di quale parte", "whose picture", parti)};
        c.push_back(v);
    }
    {
        VoceCatalogoIntervento v;
        v.tipo = "contatto_falso";
        v.testo_it = "Falso contatto acustico";
        v.testo_en = "False acoustic contact";
        v.gruppo = "INGANNO";
        v.vuole_punto = true;   // con la durata: poi sparisce; 0 = per sempre
        v.testi = {testo("classe", "classe imitata", "imitated class", {}),
                   testo("parte", "parte creduta", "believed side", parti, true)};
        v.parametri = {par("rotta", "rotta", "course", 0, 359, 1, 0), par("velocita_kn", "velocita' (nodi)", "speed (kn)", 0, 30, 1, 8),
                       par("quota_m", "quota (m)", "depth (m)", 0, 600, 5, 100),
                       par("fedelta", "fedelta' della firma (1 = perfetta)", "signature fidelity (1 = perfect)", 0, 1, 0.05, 0.8),
                       par("ping_s", "un ping ogni (s; 0 = muto)", "a ping every (s; 0 = silent)", 0, 600, 5, 0)};
        c.push_back(v);
    }
    return c;
}

int lato_della_parte(const std::string& p) { return p == "usa" ? 0x08 : p == "urss" ? 0x10 : 0; }

std::string numero(double v) {
    char b[32];
    std::snprintf(b, sizeof b, "%g", v);
    return b;
}

// i dati delle classi per le unita' nuove (una volta per cartella)
const std::map<std::string, ClasseNave>& classi_di(const std::string& cartella) {
    static std::map<std::string, std::map<std::string, ClasseNave>> cache;
    auto it = cache.find(cartella);
    if (it == cache.end()) {
        std::map<std::string, ClasseNave> c;
        try {
            c = carica_classi(cartella + "/classi.toml");
        } catch (const std::exception&) {}
        it = cache.emplace(cartella, std::move(c)).first;
    }
    return it->second;
}

}  // namespace

std::optional<double> parametro_attivo(const std::vector<Intervento>& v, const std::vector<StatoIntervento>& s, int unita,
                                       const std::string& tipo, const std::string& nome, const std::string& sensore) {
    std::optional<double> r;
    for (std::size_t k = 0; k < v.size() && k < s.size(); ++k) {
        const Intervento& x = v[k];
        if (x.unita != unita || x.tipo != tipo || !s[k].applicato || s[k].finito) continue;
        if (!sensore.empty()) {
            const auto t = x.testi.find("sensore");
            if (t == x.testi.end() || t->second != sensore) continue;
        }
        if (const auto p = x.parametri.find(nome); p != x.parametri.end()) r = p->second;
    }
    return r;
}

double attenuazione_guasto(const std::vector<Intervento>& v, const std::vector<StatoIntervento>& s, int unita, const Sensore& x) {
    const auto f = parametro_attivo(v, s, unita, "sensore", "fattore", x.antenna == Antenna::lineare ? "schiera" : "sonar");
    if (!f) return 0.0;
    return *f <= 0.0 ? -1e9 : 20.0 * std::log10(std::min(*f, 1.0));
}

DisturboRadio disturbo_radio(const std::vector<Intervento>& v, const std::vector<StatoIntervento>& s, int lato, Vec2 p) {
    DisturboRadio d;
    double passa = 1.0;
    for (std::size_t k = 0; k < v.size() && k < s.size(); ++k) {
        const Intervento& x = v[k];
        if (x.tipo != "disturbo_radio" || !s[k].applicato || s[k].finito) continue;
        const auto pt = x.testi.find("parte");
        if (pt == x.testi.end() || lato_della_parte(pt->second) != lato) continue;
        if (x.punto && x.raggio_m > 0.0 && distanza(p, *x.punto) > x.raggio_m) continue;
        const auto q = [&](const char* n, double def) { const auto it = x.parametri.find(n); return it == x.parametri.end() ? def : it->second; };
        passa *= 1.0 - std::clamp(q("perdita", 0.0), 0.0, 1.0);
        d.ritardo *= std::max(1.0, q("ritardo", 1.0));
    }
    d.perdita = 1.0 - passa;
    return d;
}

void zone_meteo(const std::vector<Intervento>& v, const std::vector<StatoIntervento>& s, Vec2 p, StatoMeteo& m) {
    for (std::size_t k = 0; k < v.size() && k < s.size(); ++k) {
        const Intervento& x = v[k];
        if (x.tipo != "meteo" || !x.punto || x.raggio_m <= 0.0 || !s[k].applicato || s[k].finito || distanza(p, *x.punto) > x.raggio_m) continue;
        const auto q = [&](const char* n) { const auto it = x.parametri.find(n); return it == x.parametri.end() ? std::optional<double>{} : it->second; };
        if (const auto w = q("mare")) m.hs_vento_m = centro_douglas(static_cast<int>(std::lround(*w)));
        if (const auto w = q("vento_kn")) m.vento_m_s = *w * NODO, m.raffica_m_s = std::max(m.raffica_m_s, m.vento_m_s * 1.3);
        if (const auto w = q("visibilita_m")) m.visibilita_m = *w;
        if (const auto w = q("nubi")) m.nubi_ottavi = 8.0 * *w;
    }
}

const std::vector<VoceCatalogoIntervento>& catalogo_interventi() {
    static const std::vector<VoceCatalogoIntervento> c = crea_catalogo();
    return c;
}

const std::vector<VoceInnesco>& catalogo_inneschi() {
    // unita' e altra come negli eventi del registro (registro_missione.cpp)
    static const std::vector<VoceInnesco> c = {
        {"scoperta_ia", "quando un'unita' ne scopre un'altra", "when a unit detects another", true, true, false},
        {"classificazione_ia", "quando un'unita' classifica un contatto", "when a unit classifies a contact", true, true, false},
        {"lancio_ia", "quando un'unita' lancia un'arma", "when a unit launches a weapon", true, true, false},
        {"colpo", "quando un'unita' e' colpita", "when a unit is hit", true, true, false},
        {"affondamento", "quando un'unita' affonda", "when a unit sinks", true, false, false},
        {"allerta_forza", "quando l'allerta di una forza sale", "when a force's alert rises", true, false, false},
        {"evasione_ia", "quando un sottomarino evade", "when a submarine evades", true, false, false},
        {"esca_ia", "quando un'unita' lancia un'esca", "when a unit launches a decoy", true, false, false},
        {"collisione_ia", "quando due unita' si urtano", "when two units collide", true, true, false},
        {"fuoco_amico_ia", "al fuoco amico", "on friendly fire", true, true, false},
        {"emersione_emergenza", "a un'emersione d'emergenza", "on an emergency surfacing", true, false, false},
        {"posti_di_combattimento", "quando il battello va ai posti di combattimento", "when the boat goes to battle stations", false, false, false},
        {"zona", "quando un'unita' entra in una zona", "when a unit enters an area", true, false, true},
    };
    return c;
}

const VoceCatalogoIntervento* voce_intervento(const std::string& tipo) {
    for (const auto& v : catalogo_interventi())
        if (v.tipo == tipo) return &v;
    return nullptr;
}

EsitoIntervento Missione::intervieni(const Intervento& ingresso) {
    EsitoIntervento r;
    const auto no = [&](const char* it, const char* en) {
        r.motivo_it = it;
        r.motivo_en = en;
        return r;
    };
    // nel vetrino; l'Enhanced (il catalogo acustico) si controlla all'applicazione, cosi' gli interventi dell'esperimento
    // si danno anche prima del primo passo, senza cambiare l'ordine d'avvio (la principale, --esperimento)
    if (!senza_giocatore_ || (!catalogo_ && tempo_ > 0.0)) return no("solo nel vetrino dell'Enhanced", "only in the Enhanced showcase");
    const VoceCatalogoIntervento* v = voce_intervento(ingresso.tipo);
    if (!v) return no("tipo sconosciuto", "unknown type");
    Intervento x = ingresso;
    if (v->vuole_unita) {
        const UnitaClassica* u = trova(x.unita);
        if (!u || u->distrutta || u->persa) return no("unita' assente", "no such unit");
        if (!v->categorie.empty() && std::find(v->categorie.begin(), v->categorie.end(), u->categoria) == v->categorie.end())
            return no("categoria non adatta", "wrong category");
    }
    if (v->vuole_punto && !v->punto_facoltativo && !x.punto) return no("serve un punto", "a point is needed");
    for (const auto& t : v->testi)
        if (!t.facoltativo && !x.testi.count(t.nome)) return no("manca un testo", "a text is missing");
    for (const auto& p : v->parametri) {
        const auto it = x.parametri.find(p.nome);
        if (it == x.parametri.end()) {
            if (!p.facoltativo) x.parametri[p.nome] = p.predefinito;
        } else if (it->second < p.minimo - 1e-9 || it->second > p.massimo + 1e-9) {
            return no("parametro fuori dai limiti", "parameter out of range");
        }
    }
    if (x.tipo == "strato") {
        const std::string& k = x.testi.at("modifica");
        if ((k == "sposta" || k == "canale") && !x.parametri.count("quota_m")) return no("serve la quota", "a depth is needed");
        if (k == "rinforza" && !x.parametri.count("fattore")) return no("serve il fattore", "a factor is needed");
        if (x.raggio_m > 0.0 && !x.punto) return no("la zona vuole un punto", "the area needs a point");
    }
    if (x.tipo == "polynya" && x.raggio_m <= 0.0) return no("serve un raggio", "a radius is needed");
    // la classe: subito se i dati ci sono gia', se no all'applicazione
    if ((x.tipo == "unita_nuova" || x.tipo == "contatto_falso") && !cartella_dati_traffico_.empty() &&
        !classi_di(cartella_dati_traffico_).count(x.testi.at("classe")))
        return no("classe sconosciuta", "unknown class");
    const VoceInnesco* inn = nullptr;
    if (x.innesco) {
        for (const auto& k : catalogo_inneschi()) if (k.evento == x.innesco->evento) inn = &k;
        if (!inn) return no("innesco sconosciuto", "unknown trigger");
        if (inn->vuole_punto && (!x.innesco->punto || x.innesco->raggio_m <= 0.0 || x.innesco->unita < 0))
            return no("l'innesco vuole un'unita', un punto e un raggio", "the trigger needs a unit, a point and a radius");
    }
    if (!v->ha_durata) x.durata_s = 0.0;
    x.id = prossimo_intervento_++;
    // la riga (docs/28 §1.5): «a 40 min, ordine alla Spruance (2): rotta 90, per 20 min»
    std::string it = v->testo_it, en = v->testo_en;
    if (x.unita >= 0)
        if (const auto* u = trova(x.unita)) it += " (" + u->classe + " " + std::to_string(u->id) + ")", en += " (" + u->classe + " " + std::to_string(u->id) + ")";
    for (const auto& [k, t] : x.testi) it += ", " + k + " " + t, en += ", " + k + " " + t;
    for (const auto& [k, n] : x.parametri) it += ", " + k + " " + numero(n), en += ", " + k + " " + numero(n);
    if (x.durata_s > 0.0) it += ", per " + numero(x.durata_s / 60.0) + " min", en += ", for " + numero(x.durata_s / 60.0) + " min";
    if (inn) {   // «..., quando la Krivak (2) classifica un contatto»
        it += ", " + inn->testo_it, en += ", " + inn->testo_en;
        if (x.innesco->unita >= 0) it += " (" + std::to_string(x.innesco->unita) + ")", en += " (" + std::to_string(x.innesco->unita) + ")";
        if (x.innesco->altra >= 0) it += " su " + std::to_string(x.innesco->altra), en += " on " + std::to_string(x.innesco->altra);
    }
    x.riga_it = it;
    x.riga_en = en;
    interventi_.push_back(x);
    StatoIntervento st;
    st.id = x.id;
    st.eventi_visti = eventi_missione().size();   // l'innesco guarda solo gli eventi da qui in poi
    stati_interventi_.push_back(st);
    r.ok = true;
    r.id = x.id;
    return r;
}

bool Missione::annulla_intervento(int id) {
    for (std::size_t k = 0; k < interventi_.size(); ++k)
        if (interventi_[k].id == id) {
            if (stati_interventi_[k].applicato || stati_interventi_[k].fallito) return false;
            interventi_.erase(interventi_.begin() + static_cast<std::ptrdiff_t>(k));
            stati_interventi_.erase(stati_interventi_.begin() + static_cast<std::ptrdiff_t>(k));
            return true;
        }
    return false;
}

std::vector<const Intervento*> Missione::interventi_attivi(double t) const {
    std::vector<const Intervento*> v;
    for (std::size_t k = 0; k < interventi_.size(); ++k) {
        const StatoIntervento& st = stati_interventi_[k];
        if (st.applicato && st.inizio <= t + 1e-9 && (!st.finito || st.fine > t)) v.push_back(&interventi_[k]);
    }
    return v;
}

// chi riceve le ROE nuove della sua parte: le navi e gli aerei sulla rete radio dopo ROE_RETE_S, i sottomarini al
// broadcast con l'antenna su o a quota periscopica (docs/28 §3.1: «chi e' sotto e non ascolta il broadcast le sa dopo»)
void Missione::giro_radio_roe() {
    for (std::size_t k = 0; k < interventi_.size(); ++k) {
        const Intervento& x = interventi_[k];
        if (x.tipo != "roe" || !stati_interventi_[k].applicato) continue;
        const auto pt = x.testi.find("parte");
        const int lato = pt == x.testi.end() ? -1 : lato_della_parte(pt->second);
        auto& ricevute = roe_ricevute_[x.id];
        for (const auto& [id, u] : unita_) {
            if ((u.flag & 0x18) != lato || u.distrutta || u.persa || ricevute.count(id) || (u.flag & 0x20)) continue;
            const double attesa = ROE_RETE_S * disturbo_radio(interventi_, stati_interventi_, lato, u.pos).ritardo;   // D3
            const bool rete = u.categoria != "sottomarino" && tempo_ - stati_interventi_[k].inizio >= attesa;
            const bool broadcast = u.categoria == "sottomarino" && (forze_ && forze_->quadro_sottomarini ? a_quota_antenna(u) && !sotto_banchisa(id)
                                                                                                        : (u.flag54 & 4) || u.quota <= QUOTA_BROADCAST_M);
            if (rete || broadcast) ricevute.insert(id);
        }
    }
}

namespace {
// l'innesco dell'intervento e' vero adesso? (gli eventi nuovi del registro da st.eventi_visti, o l'unita' nella zona)
bool innesco_vero_in(const Missione& m, const Intervento& x, StatoIntervento& st) {
    if (!x.innesco) return true;
    const Innesco& i = *x.innesco;
    if (i.evento == "zona") {
        const auto it = m.unita().find(i.unita);
        return it != m.unita().end() && !it->second.distrutta && !it->second.persa && i.punto && distanza(it->second.pos, *i.punto) <= i.raggio_m;
    }
    const auto& ev = m.eventi_missione();
    bool vero = false;
    for (; st.eventi_visti < ev.size(); ++st.eventi_visti) {
        const EventoMissione& e = ev[st.eventi_visti];
        if (nome_evento(e.tipo) != i.evento) continue;
        if (i.unita >= 0 && e.unita != i.unita) continue;
        if (i.altra >= 0 && e.altra != i.altra) continue;
        if (i.evento == "posti_di_combattimento" && e.livello != 1) continue;
        vero = true;
        ++st.eventi_visti;
        break;
    }
    return vero;
}
}  // namespace

void Missione::passo_interventi() {
    giro_radio_roe();
    if (forze_) passo_ordini_falsi(), passo_spia();   // D3: i falsi ordini e la spia (missione_forze.cpp)
    // D4, la polynya che si chiude: il ghiaccio nuovo si forma sopra chi e' a galla o a quota periscopica. L'equipaggio
    // avvisa una volta sola, nessun danno: a ghiaccio fatto valgono le regole di sotto_banchisa
    if (ghiaccio_)
        for (std::size_t k = 0; k < interventi_.size(); ++k) {
            const Intervento& x = interventi_[k];
            const StatoIntervento& st = stati_interventi_[k];
            if (x.tipo != "polynya" || !st.applicato || st.finito || x.testi.at("azione") != "chiudi") continue;
            for (const auto& [id, u] : unita_) {
                if (u.categoria != "sottomarino" || u.distrutta || u.persa || u.quota > QUOTA_BROADCAST_M || distanza(u.pos, *x.punto) > x.raggio_m) continue;
                if (!ghiaccio_->in(u.pos, tempo_).ghiaccio) continue;
                const auto gia = [&, id = id](const ReazioneIa& r) { return r.evento == "polynya" && r.unita == id && r.t >= st.inizio; };
                if (std::any_of(reazioni_ia_.begin(), reazioni_ia_.end(), gia)) continue;
                reazioni_ia_.push_back({tempo_, id, -1, u.categoria, "ghiaccio_nuovo", "polynya"});
                if (u.giocatore) messaggio_di_bordo(id, MSG_GHIACCIO_NUOVO);
            }
        }
    const auto aspetta = [&](const Intervento& x) {   // a t=0: il sistema che gli serve nasce alla fine di questo passo
        if (tempo_ >= ATTESA_MONDO_S) return false;
        if (!catalogo_) return true;
        if ((x.tipo == "situazione" || x.tipo == "allerta" || x.tipo == "cambia_parte") && !forze_ && !forze_spente_) return true;
        if ((x.tipo == "meteo" || x.tipo == "ghiaccio_deriva" || x.tipo == "polynya") && !meteo_) return true;
        return (x.tipo == "false_flag" || x.tipo == "incidente") && !siluri_attivi_;
    };
    for (std::size_t k = 0; k < interventi_.size(); ++k) {
        Intervento& x = interventi_[k];
        StatoIntervento& st = stati_interventi_[k];
        if (!st.applicato && !st.fallito && (x.tempo < 0.0 || tempo_ >= x.tempo - 1e-9) && !aspetta(x) && innesco_vero_in(*this, x, st)) {
            if (x.tempo < 0.0) x.tempo = tempo_;   // il «subito» col tempo del passo, per il replay e il file
            st.inizio = tempo_;
            if (applica_intervento(x, st)) st.applicato = true;
            else st.fallito = true;
            if (st.applicato && x.durata_s <= 0.0 && !voce_intervento(x.tipo)->ha_durata) {   // istantaneo: finito subito
                st.finito = true;
                st.fine = tempo_;
            }
        } else if (st.applicato && !st.finito && x.durata_s > 0.0 && tempo_ >= st.inizio + x.durata_s - 1e-9) {
            fine_intervento(x, st);
            st.finito = true;
            st.fine = tempo_;
        }
    }
}

bool Missione::applica_intervento(const Intervento& x, StatoIntervento& st) {
    const auto no = [&](const char* it, const char* en) {
        st.motivo_it = it;
        st.motivo_en = en;
        return false;
    };
    if (!catalogo_) return no("solo nell'Enhanced", "only in the Enhanced");
    const VoceCatalogoIntervento* v = voce_intervento(x.tipo);
    UnitaClassica* u = trova(x.unita);
    if (v && v->vuole_unita && (!u || u->distrutta || u->persa)) return no("l'unita' non c'e' piu'", "the unit is gone");
    const auto p = [&](const char* nome) { const auto it = x.parametri.find(nome); return it == x.parametri.end() ? std::optional<double>{} : it->second; };
    if (x.tipo == "togli") {
        togli(*u);
        return true;
    }
    if (x.tipo == "ordine") {
        ripristini_[x.id] = {u->velocita_ordinata, u->quota_ordinata, u->rotta_ordinata, u->navigazione, u->meta};
        if (const auto r = p("rotta")) u->rotta_ordinata = normalizza_360(*r), u->rotta_libera = false, u->navigazione = false;
        if (const auto q = p("quota_m"); q && u->categoria == "sottomarino") u->quota_ordinata = std::clamp(*q, 0.0, u->quota_max);
        if (const auto w = p("velocita_kn")) u->velocita_ordinata = std::clamp(*w, 0.0, u->avanti_tutta * 280.0 / 256.0);
        if (x.punto) u->meta = *x.punto, u->navigazione = true, u->insegui = false;
        if (const auto g = x.testi.find("gestore"); g != x.testi.end() && !g->second.empty()) programma(0.0, u->id, g->second, false);
        return true;
    }
    if (x.tipo == "unita_nuova") {
        const auto& classi = classi_di(cartella_dati_traffico_);
        const auto c = classi.find(x.testi.at("classe"));
        if (c == classi.end()) return no("classe sconosciuta", "unknown class");
        const bool sub = c->second.altezza_vela_m > SOTTOMARINO_VELA_M;
        UnitaClassica n;
        n.id = prossimo_id_++;
        n.classe = c->first;
        n.categoria = sub ? "sottomarino" : "nave";
        n.tipo_originale = c->second.tipo_originale;
        n.flag = lato_della_parte(x.testi.at("parte")) | (sub ? 1 : 2);   // come le unita' dei file: 1 sottomarino, 2 nave
        n.pos = *x.punto;
        n.rotta = n.rotta_ordinata = normalizza_360(p("rotta").value_or(0.0));
        // il modello dalle classi (velocita' massima) e i valori delle unita' dei file (le prove: SUB e NAVE)
        const double massima = c->second.velocita_max_nodi > 0.0 ? c->second.velocita_max_nodi : (sub ? 25.0 : 30.0);
        n.avanti_tutta = massima * 256.0 / 280.0;
        n.velocita = n.velocita_ordinata = std::clamp(p("velocita_kn").value_or(10.0), 0.0, massima);
        n.virata = sub ? 1.0 : 2.0;
        n.accelerazione = 0.5;
        n.rateo_quota_modello = 1.0;
        n.quota_max = sub ? 450.0 : 0.0;
        n.quota = n.quota_ordinata = sub ? std::clamp(p("quota_m").value_or(100.0), 0.0, n.quota_max) : 0.0;
        n.integrita = n.integrita_max = sub ? 200 : 100;
        n.rumore = sub ? 8 : 10;
        n.sonar = sub ? 19 : 20;
        unita_[n.id] = n;
        osservatore_ = visitata_ = n.id;   // come nuova_unita_da_buffer: in testa alla lista
        ++create_;
        if (const auto g = x.testi.find("gestore"); g != x.testi.end() && !g->second.empty()) programma(0.0, n.id, g->second, false);
        return true;
    }
    if (x.tipo == "contatto_falso") {   // un'esca semovente con la firma della classe, senza lanciatore (missione_esche.cpp)
        const auto& classi = classi_di(cartella_dati_traffico_);
        const auto c = classi.find(x.testi.at("classe"));
        if (c == classi.end()) return no("classe sconosciuta", "unknown class");
        int id = FALSI_DA + x.id % FALSI_QUANTI;
        for (int k = 0; unita_.count(id) && k < FALSI_QUANTI; ++k) id = FALSI_DA + (id - FALSI_DA + 1) % FALSI_QUANTI;
        if (unita_.count(id)) return no("troppi falsi contatti", "too many false contacts");
        const bool sub = c->second.altezza_vela_m > SOTTOMARINO_VELA_M;
        const auto pt = x.testi.find("parte");
        UnitaClassica e;
        e.id = id;
        e.classe = c->first;
        e.categoria = "arma";
        e.flag = 193 | (pt == x.testi.end() ? 0 : lato_della_parte(pt->second));   // falso bersaglio, mai un colpo; la parte creduta
        e.pos = *x.punto;
        e.quota = e.quota_ordinata = std::max(sub ? p("quota_m").value_or(100.0) : 0.0, QUOTA_FALSO_MIN_M);
        e.quota_max = std::max(e.quota, 600.0);
        e.rateo_quota_modello = 1.0;
        e.rotta = e.rotta_ordinata = normalizza_360(p("rotta").value_or(0.0));
        e.virata = 3.0;
        const double v = p("velocita_kn").value_or(8.0);
        e.velocita = e.velocita_ordinata = v;
        e.avanti_tutta = std::max(v, 1.0) * 256.0 / 280.0 + 1.0;
        e.accelerazione = 0.5;
        e.integrita = e.integrita_max = 2;
        e.rumore = sub ? 8 : 10;
        unita_[e.id] = e;
        ++create_;
        EscaMobile f;
        f.inizio = tempo_;
        f.rotta0 = e.rotta;
        f.classe_lanciatore = c->first;
        f.categoria = sub ? "sottomarino" : "nave";
        f.fedelta = std::clamp(p("fedelta").value_or(0.8), 0.0, 1.0);
        f.ping_s = p("ping_s").value_or(0.0);
        f.prossimo_ping = tempo_ + f.ping_s;
        f.velocita_kn = v;
        f.intervento = x.id;
        esche_mobili_[e.id] = std::move(f);
        return true;
    }
    // letti dove servono (parametro_attivo; il disturbo, i falsi ordini e la spia nella forza, missione_forze.cpp, appena ci
    // sono le forze: al primo passo non ci sono ancora)
    if (x.tipo == "sensore" || x.tipo == "macchine" || x.tipo == "tubi" || x.tipo == "disturbo_radio" || x.tipo == "ordine_falso" ||
        x.tipo == "spia")
        return true;
    if (x.tipo == "false_flag") {   // D4: il siluro dell'attaccante vero con la parte di un altro (missione_forze.cpp, travestita)
        if (!siluri_attivi_) return no("senza i siluri dell'Enhanced", "without the Enhanced torpedoes");
        const UnitaClassica* b = trova(static_cast<int>(std::lround(p("bersaglio").value_or(-1.0))));
        if (!b || b->distrutta || b->persa) return no("bersaglio assente", "no such target");
        const auto a = x.testi.find("arma");
        const std::string classe = a != x.testi.end() && !a->second.empty() ? a->second : "type53_65k";
        const DatiSiluro* d = tabella_siluri_.siluro(classe == "type53" ? "uset80" : classe);
        if (!d) return no("siluro sconosciuto", "unknown torpedo");
        UnitaClassica w;
        w.id = prossimo_id_++;
        w.classe = classe;
        w.categoria = "arma";
        w.flag = 0x20 | lato_della_parte(x.testi.at("parte"));
        w.pos = u->pos;
        // la corsa: contro una nave a 15 m (STIMA, come il siluro antinave impostato), se no alla quota del lanciatore
        w.quota = w.quota_ordinata = b->categoria == "nave" ? std::clamp(u->quota, 1.0, 15.0) : std::max(u->quota, 1.0);
        w.rotta = w.rotta_ordinata = rilevamento(w.pos, b->pos);
        w.lanciatore = u->id;
        w.bersaglio = b->id;
        w.prossimo_bersaglio = w.id;
        w.velocita = w.velocita_ordinata = d->velocita_alta_kn;
        w.difesa = 40;
        auto& n = unita_[w.id] = w;
        osservatore_ = visitata_ = w.id;
        ++create_;
        nuovo_siluro(n, d);
        return true;
    }
    if (x.tipo == "incidente") {   // D3, l'incidente provocato (docs/28 §3.5)
        const auto it = x.testi.find("incidente");
        const double quota = p("quota_m").value_or(30.0);
        if (it != x.testi.end() && it->second == "scoppio") {
            scoppio_provocato(*x.punto, quota, p("kg").value_or(100.0));
            return true;
        }
        // il siluro «di nessuno»: un'arma della parte creduta senza lanciatore, col modello, sul bersaglio vero se c'e'
        if (!siluri_attivi_) return no("senza i siluri dell'Enhanced", "without the Enhanced torpedoes");
        const auto a = x.testi.find("arma");
        const std::string classe = a != x.testi.end() && !a->second.empty() ? a->second : "type53";
        const DatiSiluro* d = tabella_siluri_.siluro(classe == "type53" ? "uset80" : classe);   // come dati_siluro_missione
        if (!d) return no("siluro sconosciuto", "unknown torpedo");
        const auto pt = x.testi.find("parte");
        const UnitaClassica* b = x.unita >= 0 ? trova(x.unita) : nullptr;
        UnitaClassica w;
        w.id = prossimo_id_++;
        w.classe = classe;
        w.categoria = "arma";
        w.flag = 0x20 | (pt == x.testi.end() ? 0 : lato_della_parte(pt->second));
        w.pos = *x.punto;
        w.quota = w.quota_ordinata = std::max(quota, 1.0);
        w.rotta = w.rotta_ordinata = b ? rilevamento(w.pos, b->pos) : normalizza_360(p("rotta").value_or(0.0));
        w.lanciatore = -1;
        w.bersaglio = b ? b->id : -1;
        w.prossimo_bersaglio = w.id;
        w.velocita = w.velocita_ordinata = d->velocita_alta_kn;
        w.difesa = 40;
        auto& u = unita_[w.id] = w;
        osservatore_ = visitata_ = w.id;   // come nuova_unita_da_buffer: in testa alla lista
        ++create_;
        nuovo_siluro(u, d);
        return true;
    }
    const auto t = [&](const char* nome) { const auto it = x.testi.find(nome); return it == x.testi.end() ? std::string() : it->second; };
    const auto situazione = [](const std::string& n) { return n == "pace" ? Situazione::pace : n == "crisi" ? Situazione::crisi : Situazione::guerra; };
    if (x.tipo == "situazione") {   // come la dichiarazione della missione: alza anche i minimi d'allerta
        if (!forze_) return no("senza le forze", "without the forces");
        const int a = lato_della_parte(t("parte_a")), b = lato_della_parte(t("parte_b"));
        if (a == b) return no("due parti uguali", "the same side twice");
        dichiara_situazione(situazione(t("situazione")), std::min(a, b), std::max(a, b));
        return true;
    }
    if (x.tipo == "cambia_parte") {   // i bit della parte; chi non lo sa la tratta come prima (parte_nota, lato_creduto, ostili)
        if (!forze_) return no("senza le forze", "without the forces");
        if (u->giocatore) return no("non il battello del comandante", "not the commander's boat");
        const int nuovo = lato_della_parte(t("parte")), prima = u->flag & 0x18;
        if (nuovo == prima) return no("e' gia' di quella parte", "already on that side");
        u->flag = (u->flag & ~0x18) | nuovo;
        auto& c = forze_->cambi_parte[u->id];
        c = {};
        c.lato_prima = prima;
        const std::string annuncio = t("annuncio");
        if (annuncio != "nessuno") c.sapra[nuovo] = tempo_;
        if (annuncio == "tutti")
            for (const int lato : {0, 0x08, 0x10, 0x18}) c.sapra[lato] = tempo_;
        // lei sa chi e' chi: i nuovi amici non sono piu' bersagli (i contatti restano)
        const auto amico = [&](int id) { const auto* b = trova(id); return b && (b->flag & 0x18) == nuovo; };
        if (amico(u->bersaglio)) u->bersaglio = -1;
        if (amico(u->agganciato)) u->agganciato = -1;
        if (amico(u->inseguito)) u->inseguito = -1;
        return true;
    }
    if (x.tipo == "roe") {   // le regolazioni della parte (come la sandbox); le unita' le seguono quando le ricevono (giro_radio_roe)
        auto& r = regolazioni_[lato_della_parte(t("parte"))];
        RipristinoIntervento rp{};
        rp.roe = r.roe;
        ripristini_[x.id] = rp;
        roe_ricevute_[x.id];
        if (!r.roe) r.roe.emplace();
        const bool libere = t("regola") == "libere";
        const std::string c = t("categoria");
        if (c == "sottomarini" || c == "tutte") (*r.roe)[CategoriaRoe::sottomarini] = libere;
        if (c == "navi" || c == "tutte") (*r.roe)[CategoriaRoe::navi] = libere, (*r.roe)[CategoriaRoe::mercantili] = libere;
        if (c == "aerei" || c == "tutte") (*r.roe)[CategoriaRoe::aerei] = libere;
        return true;
    }
    if (x.tipo == "allerta") {   // la forza della parte dell'unita' al livello; poi la discesa come sempre
        if (!forze_) return no("senza le forze", "without the forces");
        const int lato = u->flag & 0x18;
        auto& f = forze_->forza[lato];
        const int a = static_cast<int>(std::lround(p("livello").value_or(2.0)));
        if (a != f.v) forze_->cambi.push_back({tempo_, lato, -1, f.v, a, "intervento"});
        f.v = a;
        f.ultimo = tempo_;
        return true;
    }
    if (x.tipo == "meteo") meteo_celle_.clear();   // l'effetto subito, non al rinnovo delle celle (10 s)
    if (x.tipo == "meteo" && x.punto && x.raggio_m > 0.0) return true;   // nella zona: lo legge meteo_in (zone_meteo)
    if (x.tipo == "meteo") {   // in tutto il teatro, fermo da qui: i campi dati sopra il meteo di adesso
        if (!meteo_) return no("senza il meteo dell'Enhanced", "without the Enhanced weather");
        meteo_->avanza(tempo_);
        StatoMeteo m = meteo_->stato();
        RipristinoIntervento r{};
        if (meteo_->fisso()) r.meteo = m;
        ripristini_[x.id] = r;
        if (const auto w = p("mare")) m.hs_vento_m = centro_douglas(static_cast<int>(std::lround(*w)));
        if (const auto w = p("vento_kn")) m.vento_m_s = *w * NODO, m.raffica_m_s = std::max(m.raffica_m_s, m.vento_m_s * 1.3);
        if (const auto w = p("visibilita_m")) m.visibilita_m = *w;
        if (const auto w = p("nubi")) m.nubi_ottavi = 8.0 * *w;
        meteo_->fissa_sopra(m);   // il mare cambia in tutto il teatro, il pack resta dov'e'
        return true;
    }
    if (x.tipo == "strato") {
        const std::string& k = x.testi.at("modifica");
        const auto t = k == "sposta" ? ModificaStrato::Tipo::sposta : k == "rinforza" ? ModificaStrato::Tipo::rinforza : k == "canale" ? ModificaStrato::Tipo::canale : ModificaStrato::Tipo::togli;
        if (!clima_) return no("senza il clima dell'Enhanced", "without the Enhanced climate");
        if (!strato_applicabile(t)) return no("qui e in questo mese non c'e' un termoclino da cambiare", "no thermocline to change here in this month");
        ZonaStrato z;
        z.modifica = {t, t == ModificaStrato::Tipo::rinforza ? p("fattore").value_or(1.0) : p("quota_m").value_or(0.0)};
        if (x.punto) z.centro = *x.punto, z.raggio_m = x.raggio_m;
        z.t0 = tempo_;
        z.transizione_s = p("transizione_s").value_or(0.0);
        z.durata_s = durata_zona(x, z.transizione_s);
        aggiungi_strato(z);
        return true;
    }
    if (x.tipo == "ghiaccio_deriva" || x.tipo == "polynya") {
        CampoGhiaccio* g = ghiaccio_da_cambiare();
        if (!g) return no("senza il ghiaccio dell'Enhanced", "without the Enhanced ice");
        if (x.tipo == "ghiaccio_deriva") {
            g->cambia_deriva(tempo_, velocita_deriva(x));
        } else {
            ZonaGhiaccio z;
            z.centro = *x.punto;
            z.raggio_m = x.raggio_m;
            z.apri = x.testi.at("azione") == "apri";
            z.t0 = tempo_;
            z.transizione_s = p("transizione_s").value_or(0.0);
            z.durata_s = durata_zona(x, z.transizione_s);
            g->forza_zona(z);
        }
        return true;
    }
    if (x.tipo == "ora") {   // sole, luna, cielo e luci si contano dall'ora UTC della missione
        ora_utc_s_ += p("ore").value_or(0.0) * 3600.0;
        return true;
    }
    if (x.tipo == "timone") {   // la quota e la rotta di adesso, per i piani fermi e per la fine
        ripristini_[x.id] = {u->velocita_ordinata, u->quota_ordinata, u->rotta_ordinata, u->navigazione, u->meta};
        if (u->categoria == "sottomarino") ripristini_[x.id].quota_ordinata = u->quota;
        return true;
    }
    return no("non ancora disponibile", "not yet available");
}

void Missione::fine_intervento(const Intervento& x, StatoIntervento& st) {
    (void)st;
    UnitaClassica* u = trova(x.unita);
    if (x.tipo == "contatto_falso")   // sparisce: per chi lo sentiva e' un contatto perso
        for (auto it = esche_mobili_.begin(); it != esche_mobili_.end(); ++it)
            if (!it->second.dati && it->second.intervento == x.id) {
                if (UnitaClassica* f = trova(it->first); f && !f->persa) togli(*f);
                esche_mobili_.erase(it);
                break;
            }
    if (x.tipo == "ordine") {   // l'unita' torna alla sua IA: gli ordini di prima
        const auto r = ripristini_.find(x.id);
        if (u && !u->distrutta && !u->persa && r != ripristini_.end()) {
            u->velocita_ordinata = r->second.velocita_ordinata;
            u->quota_ordinata = r->second.quota_ordinata;
            u->rotta_ordinata = r->second.rotta_ordinata;
            u->navigazione = r->second.navigazione;
            u->meta = r->second.meta;
        }
        if (r != ripristini_.end()) ripristini_.erase(r);
    }
    if (x.tipo == "meteo") meteo_celle_.clear();
    if (x.tipo == "meteo" && meteo_) {   // il meteo di prima: quello fisso, o di nuovo quello del teatro (la zona finisce da se')
        if (const auto r = ripristini_.find(x.id); r != ripristini_.end()) {
            if (r->second.meteo) meteo_->fissa_sopra(*r->second.meteo);
            else meteo_->libera();
            ripristini_.erase(r);
        }
    }
    if (x.tipo == "ghiaccio_deriva" && ghiaccio_) {   // la deriva dell'ultimo ancora attivo, se no quella dei parametri
        Vec2 v = ghiaccio_->parametri().deriva_m_s;
        for (std::size_t k = 0; k < interventi_.size(); ++k)
            if (interventi_[k].tipo == x.tipo && interventi_[k].id != x.id && stati_interventi_[k].applicato && !stati_interventi_[k].finito)
                v = velocita_deriva(interventi_[k]);
        ghiaccio_->cambia_deriva(tempo_, v);
    }
    if (x.tipo == "timone") {   // timone e piani liberi: torna la rotta di prima, la quota a cui era
        const auto r = ripristini_.find(x.id);
        if (u && !u->distrutta && !u->persa) {
            u->rotta_libera = false;
            u->rateo_virata = 0.0;
            if (r != ripristini_.end()) u->rotta_ordinata = r->second.rotta_ordinata;
            if (r != ripristini_.end() && u->categoria == "sottomarino") u->quota_ordinata = r->second.quota_ordinata;
        }
        if (r != ripristini_.end()) ripristini_.erase(r);
    }
}

void Missione::tieni_ordini_interventi() {
    for (std::size_t k = 0; k < interventi_.size(); ++k) {
        const Intervento& x = interventi_[k];
        const StatoIntervento& st = stati_interventi_[k];
        if (!st.applicato || st.finito) continue;
        UnitaClassica* u = trova(x.unita);
        if (!u || u->distrutta || u->persa) continue;
        const auto p = [&](const char* nome) { const auto it = x.parametri.find(nome); return it == x.parametri.end() ? std::optional<double>{} : it->second; };
        if (x.tipo == "macchine") {   // la velocita' massima ridotta (0: propulsione persa, l'abbrivio si spegne da se')
            const double massima = p("velocita_max_frazione").value_or(0.0) * u->avanti_tutta * 280.0 / 256.0;
            u->velocita_ordinata = std::min(u->velocita_ordinata, massima);
            continue;
        }
        if (x.tipo == "timone") {
            if (const auto t = p("timone_gradi_s")) {   // la virata bloccata: gira sempre (0: dritto)
                if (std::fabs(*t) < 1e-6) u->rotta_ordinata = u->rotta, u->rotta_libera = false;
                else u->rotta_libera = true, u->rateo_virata = *t;
            }
            if (const auto q = p("piani_m_s"); q && u->categoria == "sottomarino") {   // + giu', - su, 0 ferma alla quota di prima
                const auto r = ripristini_.find(x.id);
                u->quota_ordinata = *q > 1e-6 ? u->quota_max : *q < -1e-6 ? 0.0 : r != ripristini_.end() ? r->second.quota_ordinata : u->quota;
            }
            continue;
        }
        if (x.tipo != "ordine") continue;
        if (const auto r = p("rotta")) u->rotta_ordinata = normalizza_360(*r), u->rotta_libera = false, u->navigazione = false;
        if (const auto q = p("quota_m"); q && u->categoria == "sottomarino") u->quota_ordinata = std::clamp(*q, 0.0, u->quota_max);
        if (const auto w = p("velocita_kn")) u->velocita_ordinata = std::clamp(*w, 0.0, u->avanti_tutta * 280.0 / 256.0);
        if (x.punto) u->meta = *x.punto, u->navigazione = true, u->insegui = false;
    }
}

}  // namespace m688
