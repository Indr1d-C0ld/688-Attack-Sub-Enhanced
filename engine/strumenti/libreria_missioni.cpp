// Libreria di missioni pronte (roadmap punto 10; schede in docs/17_libreria_missioni.md): costruisce gli scenari
// approvati con le strutture dell'editor (missioni/scenario_missione.hpp) e scrive, per ognuno, il file dell'editor
// (<id>.lua, giocabile e riapribile nell'editor) e il pacchetto da scambiare (<id>.m688m).
//   libreria_missioni [--uscita CARTELLA]      (predefinita: data/missioni/libreria)
// Deterministico: rifarlo da' gli stessi file (la data dei pacchetti e' quella della costruzione della libreria).
#include <cmath>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <vector>

#include "missioni/pacchetto_missione.hpp"
#include "missioni/scenario_missione.hpp"
#include "nucleo/fondale.hpp"
#include "nucleo/geometria.hpp"

using namespace m688;
namespace fs = std::filesystem;

namespace {

const std::string DATI = M688_DATI;
constexpr const char* CREATA = "2026-10-25";

// Il punto d'acqua (profondita' almeno `minima`) piu' vicino a p, su una griglia di 500 m
Vec2 acqua(const Fondale& f, Vec2 p, double minima) {
    for (int raggio = 0; raggio < 120; ++raggio)
        for (int dy = -raggio; dy <= raggio; ++dy)
            for (int dx = -raggio; dx <= raggio; ++dx) {
                if (std::max(std::abs(dx), std::abs(dy)) != raggio) continue;
                const Vec2 q{p.x + dx * 500.0, p.y + dy * 500.0};
                if (f.dentro(q) && f.profondita(q) >= minima) return q;
            }
    throw std::runtime_error("nessun punto d'acqua vicino");
}

Vec2 polare(Vec2 c, double rotta, double d) { return {c.x + d * std::sin(radianti(rotta)), c.y + d * std::cos(radianti(rotta))}; }

UnitaScenario unita(int id, const std::string& classe, const std::string& categoria, const std::string& parte, Vec2 pos,
                    double rotta, double velocita, const std::string& comportamento, bool civile = false) {
    UnitaScenario u;
    u.id = id;
    u.classe = classe;
    u.categoria = categoria;
    u.parte = parte;
    u.non_combattente = civile;
    const int lato = parte == "usa" ? 0x08 : parte == "urss" ? 0x10 : 0;
    u.flag = (categoria == "sottomarino" ? 1 : categoria == "aereo" ? 3 : 2) | lato | (civile ? 0x40 : 0);
    u.pos = pos;
    u.rotta = rotta;
    u.velocita = velocita;
    u.comportamento = comportamento;
    return u;
}

ObiettivoScenario obiettivo(const std::string& id, const std::string& tipo, std::vector<int> unita, TestiScenario testo, bool primario = true) {
    ObiettivoScenario o;
    o.id = id;
    o.tipo = tipo;
    o.unita = std::move(unita);
    o.testo = std::move(testo);
    o.primario = primario;
    return o;
}

CondizioneScenario quando(CondizioneScenario::Tipo t, double valore = 0.0) {
    CondizioneScenario c;
    c.tipo = t;
    c.valore = valore;
    return c;
}

CondizioneScenario evento(const std::string& nome, double dopo_s = 0.0) {   // un evento del registro (m688.evento)
    CondizioneScenario c = quando(CondizioneScenario::Tipo::evento, dopo_s);
    c.evento = nome;
    return c;
}

AzioneScenario fine(int codice, TestiScenario testo, const std::string& categoria = "") {
    AzioneScenario a;
    a.tipo = AzioneScenario::Tipo::fine;
    a.codice = codice;
    a.testo = std::move(testo);
    a.categoria = categoria;
    return a;
}

void base(ScenarioMissione& s, const DatiScenario& d, const Fondale& f) {
    s.libreria = 2;
    s.autore = "Libreria di Melchior 688";
    s.esito_fine = d.esito_fine;
    s.esito_tempo = d.esito_tempo;
    s.centro_lat = f.centro().lat;
    s.centro_lon = f.centro().lon;
}

UnitaScenario alfa(Vec2 pos, double rotta) {
    UnitaScenario g = unita(0, "alfa", "sottomarino", "urss", pos, rotta, 6.0, "");
    g.giocatore = true;
    g.flag = 0x11;
    g.quota = 80.0;
    return g;
}

// --- per le storiche (§2b) ---

// Il punto d'acqua con la profondita' fra minima e massima piu' vicino a p, su una griglia di 200 m
Vec2 acqua_fra(const Fondale& f, Vec2 p, double minima, double massima) {
    for (int raggio = 0; raggio < 150; ++raggio)
        for (int dy = -raggio; dy <= raggio; ++dy)
            for (int dx = -raggio; dx <= raggio; ++dx) {
                if (std::max(std::abs(dx), std::abs(dy)) != raggio) continue;
                const Vec2 q{p.x + dx * 200.0, p.y + dy * 200.0};
                if (f.dentro(q) && f.profondita(q) >= minima && f.profondita(q) <= massima) return q;
            }
    throw std::runtime_error("nessun punto con il fondale cercato");
}

// Il centro di un campo minato vicino a p: fondale fra minima e massima anche 1500 m a nord e a sud (come le rapide)
Vec2 campo_minato(const Fondale& f, Vec2 p, double minima, double massima) {
    const auto buono = [&](Vec2 q) { return f.dentro(q) && f.profondita(q) >= minima && f.profondita(q) <= massima; };
    for (int raggio = 0; raggio < 60; ++raggio)
        for (int dy = -raggio; dy <= raggio; ++dy)
            for (int dx = -raggio; dx <= raggio; ++dx) {
                if (std::max(std::abs(dx), std::abs(dy)) != raggio) continue;
                const Vec2 q{p.x + dx * 200.0, p.y + dy * 200.0};
                if (buono(q) && buono(polare(q, 0.0, 1500.0)) && buono(polare(q, 180.0, 1500.0))) return q;
            }
    throw std::runtime_error("nessun fondale adatto alle mine");
}

UnitaScenario los_angeles(Vec2 pos, double rotta, double quota) {
    UnitaScenario g = unita(0, "los_angeles", "sottomarino", "usa", pos, rotta, 6.0, "");
    g.giocatore = true;
    g.flag = 0x09;
    g.quota = quota;
    return g;
}

void carico_688(ScenarioMissione& s, bool mine = false) {
    s.carico.dotazione = {{"mk48", 22}, {"harpoon", 2}, {"esca", 6}};
    s.carico.deposito = mine ? std::vector<VoceCarico>{{"mk48", 13}, {"harpoon", 2}, {"esca", 6}, {"mk67", 10}}
                             : std::vector<VoceCarico>{{"mk48", 21}, {"harpoon", 2}, {"esca", 6}};
    s.carico.tubi = {"mk48", "mk48", "mk48", "mk48", "esca", "esca", "harpoon"};
    if (mine) s.carico.tipo_missione = "minamento";
}

// l'Alfa ha sempre due esche semoventi MG-74 nel deposito (famiglia esca_mobile; decisione del 26/10/2026). Il 688
// le avrebbe solo nelle missioni moderne o what-if (la MOSS Mk 70): in libreria non ce ne sono
void carico_alfa(ScenarioMissione& s) {
    s.carico.dotazione = {{"type53", 18}, {"esca", 4}};
    s.carico.deposito = {{"type53", 18}, {"esca", 4}, {"esca_mobile", 2}};
    s.carico.tubi = {"type53", "type53", "type53", "type53", "type53", "esca", "esca"};
}

using C = CondizioneScenario::Tipo;
using A = AzioneScenario::Tipo;

void regola(ScenarioMissione& s, const std::string& id, std::vector<CondizioneScenario> q, std::vector<AzioneScenario> az, bool una_volta = true) {
    RegolaScenario x;
    x.id = id;
    x.quando = std::move(q);
    x.allora = std::move(az);
    x.una_volta = una_volta;
    s.regole.push_back(x);
}
CondizioneScenario in_zona(const std::string& zona) {
    CondizioneScenario c = quando(C::giocatore_in_zona);
    c.zona = zona;
    return c;
}
CondizioneScenario unita_in(int u, const std::string& zona) {
    CondizioneScenario c = quando(C::unita_in_zona);
    c.unita = u;
    c.zona = zona;
    return c;
}
CondizioneScenario unita_fuori(int u) {
    CondizioneScenario c = quando(C::unita_fuori);
    c.unita = u;
    return c;
}
CondizioneScenario unita_colpita(int u) {
    CondizioneScenario c = quando(C::unita_colpita);
    c.unita = u;
    return c;
}
CondizioneScenario scattata(const std::string& r) {
    CondizioneScenario c = quando(C::regola_scattata);
    c.regola = r;
    return c;
}
CondizioneScenario variabile(int indice, const std::string& confronto, double valore) {
    CondizioneScenario c = quando(C::variabile, valore);
    c.indice = indice;
    c.confronto = confronto;
    return c;
}
AzioneScenario azione(A tipo, int u = -1) {
    AzioneScenario a;
    a.tipo = tipo;
    a.unita = u;
    return a;
}
AzioneScenario vai(int u, Vec2 p, double nodi = -1.0) {
    AzioneScenario a = azione(A::vai_a, u);
    a.punto = p;
    a.velocita = nodi;
    return a;
}
AzioneScenario imposta(int indice, const std::string& operazione, int valore) {
    AzioneScenario a = azione(A::variabile);
    a.indice = indice;
    a.operazione = operazione;
    a.valore = valore;
    return a;
}
AzioneScenario cue(Vec2 p, double raggio) {
    AzioneScenario a = azione(A::cue_asw);
    a.punto = p;
    a.raggio = raggio;
    return a;
}
ObiettivoScenario obiettivo_variabile(const std::string& id, int indice, int almeno, TestiScenario testo, bool primario = true) {
    ObiettivoScenario o = obiettivo(id, "variabile", {}, std::move(testo), primario);
    o.indice = indice;
    o.minimo = almeno;
    return o;
}
ObiettivoScenario obiettivo_zona(const std::string& id, const std::string& tipo, Vec2 c, double r, TestiScenario testo, bool primario = true) {
    ObiettivoScenario o = obiettivo(id, tipo, {}, std::move(testo), primario);
    o.cerchio = CerchioScenario{c.x, c.y, r};
    return o;
}
ObiettivoScenario non_sparare(std::vector<int> unita, TestiScenario testo) {
    ObiettivoScenario o = obiettivo("non_sparare", "proteggere", std::move(unita), std::move(testo));
    o.colpire = false;
    return o;
}
ObiettivoScenario vivo() { return obiettivo("vivo", "sopravvivere", {}, {"Riportare a casa il battello", "Bring the boat home"}); }
void zona(ScenarioMissione& s, const std::string& id, TestiScenario nome, Vec2 c, double r) { s.zone.push_back({id, std::move(nome), {c.x, c.y, r}}); }

// --- radio (E6, docs/09 §5.1) ---

// un messaggio radio della missione: a tempo (`dopo_s`), o da una regola con `dopo_s` negativo (azione «radio»)
MessaggioScenario radio(const std::string& id, double dopo_s, const std::string& precedenza, const std::string& da, const std::string& oggetto,
                        const std::string& classifica, TestiScenario testo, RoeScenario roe = {}, bool autentico = true) {
    MessaggioScenario m;
    m.id = id;
    m.su_richiesta = dopo_s < 0.0;
    m.dopo_s = std::max(0.0, dopo_s);
    m.precedenza = precedenza;
    m.da = da;
    m.oggetto = oggetto;
    m.classifica = classifica;
    m.testo = std::move(testo);
    m.roe = std::move(roe);
    m.autentico = autentico;
    return m;
}
AzioneScenario manda(const std::string& id) {
    AzioneScenario a = azione(A::radio);
    a.zona = id;
    return a;
}

// LIB-I08 «Ombra lunga»: l'Alfa nello schermo di un gruppo portaerei col mare calmo (il meteo fissato di E6: calma
// piatta, visibilita' oltre 20 km). Semplificazioni (docs/17): la «foto» e' la classificazione della Nimitz; la Spruance
// «avvertita» e' l'allarme a 45 minuti, che fa fare ping alle navi.
ScenarioMissione ombra_lunga(const DatiScenario& d) {
    const Fondale f = Fondale::carica(DATI + "/teatri/faroe.fdl");
    ScenarioMissione s;
    base(s, d, f);
    s.id = "LIB_I08_OMBRA_LUNGA";
    s.titolo = {"Ombra lunga", "Long Shadow"};
    s.teatro = "faroe";
    s.alfa = true;
    s.giorno = 21;
    s.mese = 6;
    s.anno = 1989;
    s.ora_utc = 11.0;
    s.situazione = "pace";
    s.paranoia = 2;
    s.difficolta = 4;
    s.durata_s = 3.0 * 3600.0;
    s.meteo_fisso = MeteoFissoScenario{1.5, 90.0, 1.0, 40000.0, std::nullopt, std::nullopt, 1.0, "nessuna"};
    s.carico.dotazione = {{"type53", 18}, {"esca", 4}};
    s.carico.deposito = {{"type53", 18}, {"esca", 4}, {"esca_mobile", 2}};
    s.carico.tubi = {"type53", "type53", "type53", "type53", "type53", "esca", "esca"};
    // il gruppo: a nord-ovest delle isole, rotta 070 a 12 nodi
    const Vec2 inizio = acqua(f, f.proiettata({62.85, -9.0}), 400.0);
    const double rotta = 70.0, v = 12.0;
    const Vec2 meta = polare(inizio, rotta, 140000.0);
    const auto in_formazione = [&](Vec2 c, double avanti, double lato) { return polare(polare(c, rotta, avanti), rotta + 90.0, lato); };
    struct Posto { const char* classe; double avanti, lato; };
    const Posto POSTI[] = {{"nimitz", 0, 0}, {"spruance", 4000, -3500}, {"spruance", 4000, 3500}, {"kidd", -4500, 0},
                           {"perry", 7000, 0}, {"sacramento", -1500, 1200}};
    s.unita.push_back(alfa(acqua(f, polare(inizio, 125.0, 45000.0), 300.0), 330.0));
    int id = 1;
    for (const auto& p : POSTI) {
        const std::string classe = p.classe;
        const bool civile = classe == "sacramento";
        UnitaScenario u = unita(id++, classe, "nave", "usa", in_formazione(inizio, p.avanti, p.lato), rotta, v, civile ? "" : "R_nave", civile);
        u.punti = {in_formazione(meta, p.avanti, p.lato)};
        u.ruolo = classe == "nimitz" ? "portaerei" : civile ? "rifornitrice" : "scorta";
        s.unita.push_back(u);
    }
    const int nimitz = 1;
    const Vec2 centro_volo = polare(inizio, rotta, 40000.0);
    for (int k = 0; k < 2; ++k) {   // due SH-3H col sonar a immersione, dalla Nimitz
        UnitaScenario e = unita(id++, "sh3h", "aereo", "usa", in_formazione(inizio, 2000.0, k ? 2000.0 : -2000.0), rotta, 54.0, "R_aereo");
        e.ruolo = "elicottero ASW";
        for (int q = 0; q < 4; ++q) e.punti.push_back(polare(centro_volo, rotta + 90.0 * q + 45.0 * k, 9000.0));
        e.ciclica = true;
        AereoAswScenario a;
        a.unita = e.id;
        a.base_unita = nimitz;
        a.situazione = "pace";
        a.contro_giocatore = true;
        a.zona = centro_volo;
        a.raggio = 18000.0;
        s.aerei_asw.push_back(a);
        s.unita.push_back(e);
    }
    // obiettivi
    s.obiettivi.push_back(obiettivo("portaerei", "scoprire", {nimitz},
                                    {"Classificare la Nimitz da vicino (la foto, nella versione semplificata)",
                                     "Classify the Nimitz at close range (the photo, in the simplified version)"}));
    s.obiettivi.push_back(obiettivo("occulto", "non_essere_scoperto", {}, {"Non farsi scoprire", "Avoid detection"}));
    s.obiettivi.push_back(obiettivo("scorte", "scoprire", {2, 3, 4, 5}, {"Classificare le scorte", "Classify the escorts"}, false));
    s.obiettivi.push_back(obiettivo("non_sparare", "proteggere", {1, 2, 3, 4, 5, 6},
                                    {"Nessuna arma contro il gruppo: e' tempo di pace", "No weapons against the group: it is peacetime"}, false));
    s.obiettivi.back().colpire = false;
    s.obiettivi.push_back(obiettivo("vivo", "sopravvivere", {}, {"Riportare a casa il battello", "Bring the boat home"}));
    // zone e regole: l'allarme a 45 minuti (qualcuno ha passato un'informazione), il rientro dopo l'allarme
    const Vec2 rientro = s.unita[0].pos;
    s.zone.push_back({"rientro", {"zona di rientro", "return area"}, {rientro.x, rientro.y, 8000.0}});
    RegolaScenario allarme;
    allarme.id = "schermo_avvertito";
    allarme.quando = {quando(CondizioneScenario::Tipo::tempo, 2700.0)};
    AzioneScenario a;
    a.tipo = AzioneScenario::Tipo::allarme;
    allarme.allora = {a};
    s.regole.push_back(allarme);
    RegolaScenario casa;
    casa.id = "rientro";
    CondizioneScenario dopo = quando(CondizioneScenario::Tipo::regola_scattata);
    dopo.regola = "schermo_avvertito";
    CondizioneScenario dentro = quando(CondizioneScenario::Tipo::giocatore_in_zona);
    dentro.zona = "rientro";
    // si rientra dopo essere usciti (collaudo del 29/10/2026): la zona contiene la partenza, e un Alfa rimasto fermo vinceva
    // a 45 minuti, con la Nimitz classificata da lontano
    RegolaScenario uscito;
    uscito.id = "uscito";
    CondizioneScenario fuori = quando(CondizioneScenario::Tipo::giocatore_fuori_zona);
    fuori.zona = "rientro";
    uscito.quando = {fuori};
    uscito.allora = {imposta(1, "=", 1)};   // la variabile d'autore 1: l'Alfa e' partito
    s.regole.push_back(uscito);
    CondizioneScenario dopo_uscito = quando(CondizioneScenario::Tipo::regola_scattata);
    dopo_uscito.regola = "uscito";
    casa.quando = {dopo, dopo_uscito, dentro};
    casa.allora = {fine(40, {"Rientrati nella zona di raccolta: rapporto al comando della Flotta del Nord.",
                             "Back in the recovery area: report to Northern Fleet headquarters."})};
    s.regole.push_back(casa);
    s.briefing[0] = {
        "ORDINI DI MISSIONE", "DA: Comando della Flotta del Nord", "A: K-463 (PROGETTO 705)", "OGG.: OMBRA LUNGA",
        "21 giugno 1989, ore 1100Z. Teatro: Fær Øer e Mar di Norvegia.",
        "La NATO porta una portaerei nel Mar di Norvegia per l'esercitazione d'estate: il regalo che l'intelligence aspettava da due anni.",
        "Infilatevi nello schermo della scorta, registrate il rumore delle eliche della Nimitz da vicino e identificatela con certezza.",
        "Poi tornate alla zona di raccolta senza che a Norfolk sappiano che siete stati li'.",
        "Con il mare calmo una testa di periscopio si vede da dieci miglia, e gli elicotteri a immersione non dormono mai.",
        "Regole d'ingaggio: tempo di pace. Armi trattenute.", "Tempo a disposizione: 3 ore."};
    s.briefing[1] = {
        "MISSION ORDERS", "FM: NORTHERN FLEET HQ", "TO: K-463 (PROJECT 705)", "SUBJ: LONG SHADOW",
        "21 June 1989, 1100Z. Theatre: Faroes and Norwegian Sea.",
        "NATO is bringing a carrier into the Norwegian Sea for its summer exercise: the gift intelligence has been waiting two years for.",
        "Slip inside the escort screen, record the Nimitz's propellers at close range and identify her beyond doubt.",
        "Then return to the recovery area without Norfolk ever knowing you were there.",
        "In a flat calm a periscope head can be seen from ten miles, and the dipping helicopters never sleep.",
        "Rules of engagement: peacetime. Weapons hold.", "Time available: 3 hours."};
    return s;
}

// LIB-I10 «Corsa al relitto»: l'Alfa difende un relitto sovietico senza sparare. Semplificazioni (docs/17): il relitto e'
// un Charlie II fermo a 400 m (la quota massima della classe), non sul fondo; la «posizione corretta» e' nel briefing
// (Mosca da' un punto incerto di sei miglia) finche' non c'e' la radio da script; la «zona inviolata» e' fatta con le
// regole: col presidio dell'Alfa la nave oceanografica devia e lo Sturgeon si allontana.
ScenarioMissione corsa_al_relitto(const DatiScenario& d) {
    const Fondale f = Fondale::carica(DATI + "/teatri/celtica.fdl");
    ScenarioMissione s;
    base(s, d, f);
    s.id = "LIB_I10_CORSA_AL_RELITTO";
    s.titolo = {"Corsa al relitto", "Race to the Wreck"};
    s.teatro = "celtica";
    s.alfa = true;
    s.giorno = 2;
    s.mese = 8;
    s.anno = 1989;
    s.ora_utc = 16.0 + 40.0 / 60.0;
    s.situazione = "pace";
    s.paranoia = 2;
    s.difficolta = 2;
    s.durata_s = 9600.0;
    s.carico.dotazione = {{"type53", 18}, {"esca", 4}};
    s.carico.deposito = {{"type53", 18}, {"esca", 4}, {"esca_mobile", 2}};
    s.carico.tubi = {"type53", "type53", "type53", "type53", "type53", "esca", "esca"};
    const Vec2 relitto = acqua(f, f.proiettata({49.2, -11.2}), 1500.0);
    const Vec2 dato = polare(relitto, 40.0, 11000.0);   // il punto di Mosca, sei miglia piu' in la'
    s.unita.push_back(alfa(acqua(f, polare(dato, 95.0, 37000.0), 300.0), 275.0));
    UnitaScenario r = unita(1, "charlie2", "sottomarino", "urss", relitto, 0.0, 0.0, "", true);
    r.quota = 400.0;
    r.ruolo = "relitto";
    s.unita.push_back(r);
    UnitaScenario st = unita(2, "sturgeon", "sottomarino", "usa", polare(relitto, 260.0, 2500.0), 0.0, 3.0, "R_sottomarino");
    st.quota = 200.0;
    st.profonda = 350.0;
    st.nascosta = true;
    st.ruolo = "sottomarino americano";
    for (int q = 0; q < 4; ++q) st.punti.push_back(polare(relitto, 260.0 + 90.0 * q, 2500.0));
    st.ciclica = true;
    s.unita.push_back(st);
    UnitaScenario nave = unita(3, "mercantile", "nave", "usa", acqua(f, polare(relitto, 270.0, 35000.0), 50.0), 90.0, 9.0, "", true);
    nave.punti = {relitto};
    nave.ruolo = "nave oceanografica";
    s.unita.push_back(nave);
    UnitaScenario orion = unita(4, "p3c", "aereo", "usa", polare(relitto, 300.0, 25000.0), 0.0, 200.0, "R_aereo");
    for (int q = 0; q < 4; ++q) orion.punti.push_back(polare(relitto, 45.0 + 90.0 * q, 20000.0));
    orion.ciclica = true;
    orion.ruolo = "pattugliatore";
    s.unita.push_back(orion);
    AereoAswScenario a;
    a.unita = 4;
    a.base = f.proiettata({50.44, -5.0});   // St Mawgan
    a.situazione = "pace";
    a.contro_giocatore = true;
    a.zona = relitto;
    a.raggio = 30000.0;
    s.aerei_asw.push_back(a);
    UnitaScenario soccorso = unita(5, "mercantile", "nave", "urss", polare(relitto, 90.0, 3000.0), 270.0, 4.0, "", true);
    soccorso.entra_con = "soccorso";
    soccorso.ruolo = "nave di soccorso";
    s.unita.push_back(soccorso);
    // zone
    s.zone.push_back({"relitto", {"il relitto", "the wreck"}, {relitto.x, relitto.y, 2.0 * MIGLIO}});
    s.zone.push_back({"avvicinamento", {"avvicinamento", "approach"}, {relitto.x, relitto.y, 6.0 * MIGLIO}});
    // obiettivi
    ObiettivoScenario raggiungi = obiettivo("relitto", "raggiungere", {}, {"Raggiungere il relitto", "Reach the wreck"});
    raggiungi.cerchio = CerchioScenario{relitto.x, relitto.y, 2.0 * MIGLIO};
    s.obiettivi.push_back(raggiungi);
    s.obiettivi.push_back(obiettivo("non_sparare", "proteggere", {2, 3, 4}, {"Non usare le armi", "Do not use weapons"}));
    s.obiettivi.back().colpire = false;
    s.obiettivi.push_back(obiettivo("sturgeon", "scoprire", {2}, {"Classificare il sottomarino americano", "Classify the American submarine"}, false));
    s.obiettivi.push_back(obiettivo("vivo", "sopravvivere", {}, {"Riportare a casa il battello", "Bring the boat home"}));
    // regole: il presidio, la nave che devia o arriva, lo Sturgeon cacciato, il soccorso e la fine
    const auto regola = [&](const std::string& id, std::vector<CondizioneScenario> q, std::vector<AzioneScenario> az) {
        RegolaScenario x;
        x.id = id;
        x.quando = std::move(q);
        x.allora = std::move(az);
        s.regole.push_back(x);
    };
    CondizioneScenario giocatore_al_relitto = quando(C::giocatore_in_zona);
    giocatore_al_relitto.zona = "relitto";
    AzioneScenario presidio;
    presidio.tipo = A::variabile;
    presidio.indice = 0;
    presidio.valore = 1;
    regola("presidio", {giocatore_al_relitto}, {presidio});
    CondizioneScenario presidiato = quando(C::variabile, 1.0);
    presidiato.indice = 0;
    CondizioneScenario nave_vicina = quando(C::unita_in_zona);
    nave_vicina.unita = 3;
    nave_vicina.zona = "avvicinamento";
    AzioneScenario devia;
    devia.tipo = A::vai_a;
    devia.unita = 3;
    devia.punto = polare(relitto, 300.0, 60000.0);
    regola("nave_devia", {nave_vicina, presidiato}, {devia});
    CondizioneScenario nave_al_relitto = quando(C::unita_in_zona);
    nave_al_relitto.unita = 3;
    nave_al_relitto.zona = "relitto";
    regola("nave_arriva", {nave_al_relitto}, {fine(41, {"La nave americana e' sul relitto: il sommergibile da grande profondita' e' in acqua.",
                                                        "The American ship is over the wreck: the deep-diving submersible is in the water."},
                                                   "fallimento")});
    CondizioneScenario sturgeon_al_relitto = quando(C::unita_in_zona);
    sturgeon_al_relitto.unita = 2;
    sturgeon_al_relitto.zona = "relitto";
    AzioneScenario via;
    via.tipo = A::vai_a;
    via.unita = 2;
    via.punto = polare(relitto, 250.0, 30000.0);
    AzioneScenario calmo;
    calmo.tipo = A::comportamento;
    calmo.unita = 2;
    regola("sturgeon_via", {sturgeon_al_relitto, presidiato, quando(C::allarme)}, {via, calmo});
    AzioneScenario entra;
    entra.tipo = A::entra;
    entra.unita = 5;
    regola("soccorso", {quando(C::tempo, 9000.0)}, {entra});
    CondizioneScenario dopo_soccorso = quando(C::regola_scattata);
    dopo_soccorso.regola = "soccorso";
    regola("al_sicuro", {quando(C::tempo, 9300.0), dopo_soccorso},
          {fine(40, {"La nave di soccorso e' sul relitto: i codici sono al sicuro.", "The rescue ship is over the wreck: the codes are safe."})});
    s.briefing[0] = {
        "ORDINI DI MISSIONE", "DA: Comando della Flotta del Nord", "A: K-463 (PROGETTO 705)", "OGG.: CORSA AL RELITTO",
        "2 agosto 1989, ore 1640Z. Teatro: Margine della Piattaforma Celtica.",
        "Un nostro sottomarino e' affondato tre giorni fa sulla scarpata di Goban Spur, con i suoi codici e un reattore spento.",
        "Sorvegliate il relitto finche' non arriva la nave di soccorso, fra due ore e mezza. La posizione che vi diamo e' incerta di sei miglia: cercatelo col sonar.",
        "Gli americani hanno sentito l'esplosione col SOSUS. Un loro sottomarino potrebbe essere gia' sulla scarpata, e una nave «oceanografica» arriva da ovest.",
        "Non deve avvicinarsi al relitto. Fatevi sentire, mostratevi: il sonar attivo e' permesso, le armi no.",
        "Regole d'ingaggio: tempo di pace. Armi trattenute.", "Tempo a disposizione: 2 ore e 40 minuti."};
    s.briefing[1] = {
        "MISSION ORDERS", "FM: NORTHERN FLEET HQ", "TO: K-463 (PROJECT 705)", "SUBJ: RACE TO THE WRECK",
        "2 August 1989, 1640Z. Theatre: Celtic Shelf margin.",
        "One of our submarines sank three days ago on the Goban Spur slope, with her codes and a shut-down reactor.",
        "Guard the wreck until the rescue ship arrives, in two and a half hours. The position we give is uncertain by six miles: find it by sonar.",
        "The Americans heard the explosion on SOSUS. One of their submarines may already be on the slope, and an 'oceanographic' ship is coming from the west.",
        "She must not get near the wreck. Make yourself heard, show yourself: active sonar is allowed, weapons are not.",
        "Rules of engagement: peacetime. Weapons hold.", "Time available: 2 hours 40 minutes."};
    return s;
}

// LIB-S01 «Vestfjord»: la bonifica di un fiordo delle Fær Øer prima che entri la portaerei. Semplificazioni (docs/17):
// niente siluro da esercitazione, l'«arancione» affondato vale come colpito; il fondo toccato lo dice il debriefing
// (non e' un obiettivo); il mare 2 e' quello del meteo generato.
ScenarioMissione vestfjord(const DatiScenario& d) {
    const Fondale f = Fondale::carica(DATI + "/teatri/faroe.fdl");
    ScenarioMissione s;
    base(s, d, f);
    s.id = "LIB_S01_VESTFJORD";
    s.titolo = {"Vestfjord", "Vestfjord"};
    s.teatro = "faroe";
    s.giorno = 14;
    s.mese = 9;
    s.anno = 1988;
    s.ora_utc = 8.0;
    s.situazione = "pace";
    s.paranoia = 1;
    s.difficolta = 1;
    s.durata_s = 5400.0;
    carico_688(s);
    // il fiordo si apre a est; l'imboccatura, l'«arancione» che la sorveglia, il Los Angeles al largo
    const Vec2 bocca = acqua(f, f.proiettata({62.155, -6.55}), 40.0);
    s.unita.push_back(los_angeles(acqua(f, f.proiettata({62.10, -6.25}), 80.0), 300.0, 30.0));
    UnitaScenario arancione = unita(1, "trafalgar", "sottomarino", "urss", acqua(f, f.proiettata({62.162, -6.60}), 45.0), 90.0, 3.0, "R_pedina");
    arancione.quota = 25.0;
    arancione.profonda = 35.0;
    arancione.ruolo = "l'«arancione»";
    arancione.punti = {acqua(f, f.proiettata({62.158, -6.57}), 45.0), acqua(f, f.proiettata({62.164, -6.62}), 45.0)};
    arancione.ciclica = true;
    arancione.parametri.distanza_nm = 1.0;
    s.unita.push_back(arancione);
    // il gruppo che entra: di riserva, compare al largo quando la portaerei anticipa
    const double verso = rilevamento(polare(bocca, 110.0, 13000.0), bocca);
    struct Posto { const char* classe; double avanti, lato; };
    const Posto POSTI[] = {{"nimitz", 0, 0}, {"spruance", 2500, -1500}, {"spruance", 2500, 1500}, {"perry", 4500, 800}};
    int id = 2;
    for (const auto& p : POSTI) {
        const Vec2 in_formazione = polare(polare(polare(bocca, 110.0, 13000.0), verso, p.avanti), verso + 90.0, p.lato);
        UnitaScenario u = unita(id++, p.classe, "nave", "usa", acqua(f, in_formazione, 40.0), verso, 14.0, p.classe == std::string("nimitz") ? "" : "R_nave");
        // la portaerei fino all'imboccatura, le scorte di guardia due chilometri fuori
        u.punti = {p.avanti == 0 ? bocca : acqua(f, polare(polare(bocca, verso + 180.0, 2000.0), verso + 90.0, p.lato), 40.0)};
        u.entra_con = "anticipo";
        u.ruolo = p.classe == std::string("nimitz") ? "portaerei" : "scorta";
        s.unita.push_back(u);
    }
    zona(s, "imboccatura", {"l'imboccatura del fiordo", "the fjord entrance"}, bocca, 1500.0);
    s.obiettivi.push_back(obiettivo("arancione", "affondare", {1}, {"«Colpire» l'arancione prima che la portaerei arrivi all'imboccatura (affondato vale come colpito)",
                                                                    "'Hit' Orange before the carrier reaches the entrance (sunk counts as hit)"}));
    s.obiettivi.push_back(obiettivo("classificato", "scoprire", {1}, {"Classificare l'arancione", "Classify Orange"}));
    ObiettivoScenario amiche = obiettivo("amiche", "proteggere", {2, 3, 4, 5}, {"Nessun colpo contro le navi del gruppo", "No shots at the group's ships"}, false);
    amiche.colpire = false;
    s.obiettivi.push_back(amiche);
    s.obiettivi.push_back(vivo());
    regola(s, "anticipo", {quando(C::tempo, 2700.0)},
           {azione(A::entra, 2), azione(A::entra, 3), azione(A::entra, 4), azione(A::entra, 5), manda("ANTICIPO")});
    RadioScenario r;
    r.ordini = OrdiniScenario{"COMSUBLANT", "USS LOS ANGELES (SSN 688)", "NORTHERN WEDDING '88 - VESTFJORD", "SECRET"};
    r.roe = {{"sottomarini", "libero"}, {"navi", "trattenuto"}, {"aerei", "trattenuto"}};   // l'arancione si', il gruppo no
    r.messaggi = {radio("ANTICIPO", -1.0, "flash", "COMSTRIKFLT", "CARRIER ADVANCE", "SECRET",
                        {"LA PORTAEREI ANTICIPA DI TRENTA MINUTI: ALL'IMBOCCATURA VERSO LE 0915Z. LA SCORTA ENTRA PER PRIMA.",
                         "CARRIER ADVANCING THIRTY MINUTES: AT THE ENTRANCE ABOUT 0915Z. ESCORTS GOING IN FIRST."})};
    s.radio = r;
    regola(s, "bonificato", {unita_fuori(1)}, {fine(40, {"L'arancione e' «colpito»: il fiordo e' bonificato e la portaerei puo' entrare.",
                                                       "Orange has been 'hit': the fjord is clear and the carrier can go in."})});
    regola(s, "arrivata", {unita_in(2, "imboccatura")},
           {fine(41, {"La portaerei e' all'imboccatura e l'arancione e' ancora li': in guerra l'avrebbe colpita.",
                      "The carrier is at the entrance and Orange is still there: in wartime she would have been hit."}, "fallimento")});
    s.briefing[0] = {
        "ORDINI DI MISSIONE", "DA: COMSUBLANT", "A: USS LOS ANGELES (SSN-688)", "OGG.: NORTHERN WEDDING '88 - VESTFJORD",
        "14 settembre 1988, ore 0800Z. Teatro: Fær Øer e Mar di Norvegia. Esercitazione.",
        "La NATO prova un'idea nuova: nascondere una portaerei in un fiordo, dove i radar sovietici non la vedono e i missili non la trovano.",
        "Prima che entri, il fiordo va bonificato. Fra le pareti di roccia c'e' un Trafalgar britannico che fa la parte del sovietico, l'«arancione», con l'ordine di aspettare la portaerei all'imboccatura.",
        "Trovatelo, classificatelo e «colpitelo» prima che il gruppo arrivi all'imboccatura. La portaerei e' attesa alle 0945Z.",
        "Il fiordo e' stretto, il fondale sale in fretta e il Trafalgar conosce il posto meglio di voi: non toccate il fondo, e nessun colpo contro le navi amiche.",
        "Regole d'ingaggio: esercitazione. Armi d'esercitazione libere contro l'arancione.", "Tempo a disposizione: 1 ora e 30 minuti.",
        "Nota storica: ispirata alle esercitazioni NATO degli anni Ottanta e alle portaerei nascoste nei fiordi norvegesi; nomi e unita' sono di fantasia."};
    s.briefing[1] = {
        "MISSION ORDERS", "FM: COMSUBLANT", "TO: USS LOS ANGELES (SSN-688)", "SUBJ: NORTHERN WEDDING '88 - VESTFJORD",
        "14 September 1988, 0800Z. Theatre: Faroes and Norwegian Sea. Exercise.",
        "NATO is trying a new idea: hiding a carrier in a fjord, where Soviet radars cannot see her and missiles cannot find her.",
        "Before she goes in, the fjord must be swept. Somewhere between the rock walls a British Trafalgar is playing the Soviet, 'Orange', with orders to wait for the carrier at the entrance.",
        "Find her, classify her and 'hit' her before the group reaches the entrance. The carrier is due at 0945Z.",
        "The fjord is narrow, the bottom comes up fast and the Trafalgar knows the place better than you: do not touch bottom, and no shots at friendly ships.",
        "Rules of engagement: exercise. Exercise weapons free against Orange.", "Time available: 1 hour 30 minutes.",
        "Historical note: inspired by the NATO exercises of the 1980s and the carriers hidden in Norwegian fjords; names and units are fictional."};
    return s;
}

// LIB-S02 «Nave mineraria»: la guardia a una nave civile ferma sul bacino delle Curili. Il «sollevamento» e' a tempo:
// a 1 h 50' il braccio e' a bordo.
ScenarioMissione nave_mineraria(const DatiScenario& d) {
    const Fondale f = Fondale::carica(DATI + "/teatri/kunashir.fdl");
    ScenarioMissione s;
    base(s, d, f);
    s.id = "LIB_S02_NAVE_MINERARIA";
    s.titolo = {"Nave mineraria", "Mining Ship"};
    s.teatro = "kunashir";
    s.giorno = 4;
    s.mese = 7;
    s.anno = 1989;
    s.ora_utc = 5.5;
    s.meteo_realistico = true;
    s.situazione = "pace";
    s.paranoia = 2;
    s.difficolta = 2;
    s.durata_s = 7200.0;
    carico_688(s);
    const Vec2 bacino = acqua(f, f.proiettata({45.25, 145.6}), 3000.0);
    s.unita.push_back(los_angeles(acqua(f, polare(bacino, 160.0, 14000.0), 300.0), 340.0, 120.0));   // di guardia, fuori dal giro della scorta
    UnitaScenario nave = unita(1, "mercantile", "nave", "usa", bacino, 0.0, 0.0, "", true);
    nave.ruolo = "la Deep Harvest";
    s.unita.push_back(nave);
    // all'alba: la nave di misura e il rimorchiatore; poi un Krivak; il Victor se il Los Angeles si fa sentire
    UnitaScenario misura = unita(2, "cargo", "nave", "urss", polare(bacino, 330.0, 25000.0), 150.0, 10.0, "", true);
    misura.punti = {polare(bacino, 330.0, 2500.0)};
    misura.entra_con = "alba";
    misura.ruolo = "nave di misura";
    s.unita.push_back(misura);
    UnitaScenario rimorchiatore = unita(3, "mercantile", "nave", "urss", polare(bacino, 335.0, 26000.0), 155.0, 8.0, "", true);
    rimorchiatore.punti = {polare(bacino, 320.0, 3500.0)};
    rimorchiatore.entra_con = "alba";
    rimorchiatore.ruolo = "rimorchiatore";
    s.unita.push_back(rimorchiatore);
    UnitaScenario krivak = unita(4, "krivak2", "nave", "urss", polare(bacino, 300.0, 30000.0), 120.0, 14.0, "R_nave");
    for (int q = 0; q < 4; ++q) krivak.punti.push_back(polare(bacino, 300.0 + 90.0 * q, 6000.0));
    krivak.ciclica = true;
    krivak.entra_con = "krivak";
    krivak.ruolo = "scorta";
    s.unita.push_back(krivak);
    UnitaScenario victor = unita(5, "victor3", "sottomarino", "urss", polare(bacino, 250.0, 20000.0), 70.0, 10.0, "R_sottomarino");
    victor.quota = 150.0;
    victor.profonda = 300.0;
    for (int q = 0; q < 4; ++q) victor.punti.push_back(polare(bacino, 250.0 + 90.0 * q, 5000.0));
    victor.ciclica = true;
    victor.entra_con = "victor";
    victor.nascosta = true;
    victor.ruolo = "sottomarino";
    s.unita.push_back(victor);
    s.obiettivi.push_back(obiettivo("deep_harvest", "proteggere", {1}, {"Proteggere la Deep Harvest", "Protect the Deep Harvest"}));
    s.obiettivi.push_back(obiettivo("occulto", "non_essere_scoperto", {}, {"Non farsi scoprire", "Avoid detection"}));
    s.obiettivi.push_back(non_sparare({2, 3, 4, 5}, {"Nessuna arma: e' tempo di pace", "No weapons: it is peacetime"}));
    s.obiettivi.push_back(obiettivo("sovietici", "scoprire", {2, 3, 4}, {"Classificare le unita' sovietiche", "Classify the Soviet units"}, false));
    s.obiettivi.push_back(vivo());
    regola(s, "alba", {quando(C::tempo, 900.0)}, {azione(A::entra, 2), azione(A::entra, 3)});
    regola(s, "krivak", {quando(C::tempo, 2400.0)}, {azione(A::entra, 4)});
    regola(s, "a_duecento_iarde", {quando(C::tempo, 3600.0)}, {vai(3, polare(bacino, 0.0, 200.0 * IARDA), 6.0)});
    regola(s, "victor", {quando(C::allarme)}, {azione(A::entra, 5)});
    regola(s, "braccio_a_bordo", {quando(C::tempo, 6600.0)},
           {fine(40, {"Il braccio e' a bordo e la Deep Harvest riparte. Che cosa sia salito dal fondo, non lo sapra' nessuno.",
                      "The arm is aboard and the Deep Harvest gets under way. What came up from the bottom, nobody will ever know."})});
    s.briefing[0] = {
        "ORDINI DI MISSIONE", "DA: COMSUBPAC", "A: USS LOS ANGELES (SSN-688)", "OGG.: NAVE MINERARIA",
        "4 luglio 1989, ore 0530Z. Teatro: bacino delle Curili, Mare di Okhotsk.",
        "La «Deep Harvest» sta ferma da nove giorni sul bacino piu' profondo delle Curili. Ufficialmente cerca noduli di manganese con un braccio d'acciaio lungo cinque chilometri.",
        "Proteggetela senza farvi vedere: nessuno deve sapere che c'e' un sottomarino americano, ne' che cosa sale dal fondo.",
        "All'alba e' attesa una nave di misura sovietica. Se vi fate sentire, arriveranno altri a guardare piu' da vicino.",
        "Il braccio sara' a bordo verso le 0720Z.",
        "Regole d'ingaggio: tempo di pace. Armi trattenute.", "Tempo a disposizione: 2 ore (dodici ore di guardia compresse).",
        "Nota storica: ispirata al progetto Azorian (1974) e alla nave sovietica rimasta a duecento iarde; nomi e unita' sono di fantasia."};
    s.briefing[1] = {
        "MISSION ORDERS", "FM: COMSUBPAC", "TO: USS LOS ANGELES (SSN-688)", "SUBJ: MINING SHIP",
        "4 July 1989, 0530Z. Theatre: Kuril Basin, Sea of Okhotsk.",
        "The 'Deep Harvest' has been stopped for nine days over the deepest part of the Kuril Basin. Officially she is looking for manganese nodules with a steel arm three miles long.",
        "Protect her without being seen: nobody must know an American submarine is here, nor what is coming up from the bottom.",
        "A Soviet survey ship is expected at dawn. If you make yourself heard, others will come to take a closer look.",
        "The arm will be aboard around 0720Z.",
        "Rules of engagement: peacetime. Weapons hold.", "Time available: 2 hours (twelve hours of guard, compressed).",
        "Historical note: inspired by Project Azorian (1974) and the Soviet ship that stood two hundred yards off; names and units are fictional."};
    return s;
}

// LIB-S04 «Campane d'edera»: entrare nel Mare di Okhotsk, restare due ore sul cavo, uscire. Semplificazioni (docs/17):
// le due ore si contano con una regola ripetuta (una variabile ogni 10 s nella zona del cavo), senza la condizione su
// velocita' e quota; la durata e' di 4 h 30' (il transito e le due ore non stanno in quattro).
ScenarioMissione campane_d_edera(const DatiScenario& d) {
    const Fondale f = Fondale::carica(DATI + "/teatri/kunashir.fdl");
    ScenarioMissione s;
    base(s, d, f);
    s.id = "LIB_S04_CAMPANE_D_EDERA";
    s.titolo = {"Campane d'edera", "Ivy Bells"};
    s.teatro = "kunashir";
    s.giorno = 20;
    s.mese = 10;
    s.anno = 1989;
    s.ora_utc = 2.0;
    s.meteo_realistico = true;
    s.situazione = "pace";
    s.paranoia = 2;
    s.difficolta = 5;
    s.durata_s = 16200.0;
    carico_688(s);
    const Vec2 sud = acqua(f, f.proiettata({44.45, 146.76}), 300.0), nord = acqua(f, f.proiettata({44.66, 146.72}), 300.0);
    const Vec2 cavo = acqua_fra(f, f.proiettata({44.58, 146.50}), 110.0, 140.0);
    const Vec2 partenza = acqua(f, polare(sud, 160.0, 7000.0), 300.0);
    s.unita.push_back(los_angeles(partenza, 340.0, 120.0));
    UnitaScenario krivak = unita(1, "krivak2", "nave", "urss", polare(sud, 0.0, 8000.0), 0.0, 10.0, "R_nave");
    krivak.punti = {nord, sud};
    krivak.ciclica = true;
    krivak.ruolo = "guardia dello stretto";
    s.unita.push_back(krivak);
    const Vec2 burevestnik = f.proiettata({44.92, 147.62}), juzno = f.proiettata({44.02, 145.95});
    UnitaScenario il38 = unita(2, "il38", "aereo", "urss", polare(nord, 90.0, 20000.0), 270.0, 250.0, "R_aereo");
    for (int q = 0; q < 4; ++q) il38.punti.push_back(polare(nord, 45.0 + 90.0 * q, 22000.0));
    il38.ciclica = true;
    il38.ruolo = "pattugliatore";
    s.unita.push_back(il38);
    UnitaScenario ka27 = unita(3, "ka27", "aereo", "urss", polare(cavo, 200.0, 15000.0), 20.0, 100.0, "R_aereo");
    for (int q = 0; q < 4; ++q) ka27.punti.push_back(polare(cavo, 90.0 * q, 9000.0));
    ka27.ciclica = true;
    ka27.ruolo = "elicottero ASW";
    s.unita.push_back(ka27);
    for (const auto& [id, base_aerea, centro, raggio] : {std::tuple{2, burevestnik, nord, 25000.0}, std::tuple{3, juzno, cavo, 12000.0}}) {
        AereoAswScenario a;
        a.unita = id;
        a.base = base_aerea;
        a.situazione = "pace";
        a.contro_giocatore = true;
        a.zona = centro;
        a.raggio = raggio;
        s.aerei_asw.push_back(a);
    }
    // a 10 km e 10 nodi arriva in 1940 s (194 controlli): entra a 170 per essere sul cavo dopo un'ora di lavoro (360)
    UnitaScenario sopra = unita(4, "cargo", "nave", "urss", acqua(f, polare(cavo, 315.0, 10000.0), 50.0), 135.0, 10.0, "", true);
    sopra.punti = {cavo};
    sopra.entra_con = "nave_sul_cavo";
    sopra.ruolo = "nave che si ferma sul cavo";
    s.unita.push_back(sopra);
    UnitaScenario victor = unita(5, "victor3", "sottomarino", "urss", acqua(f, f.proiettata({44.85, 146.65}), 500.0), 250.0, 12.0, "R_sottomarino");
    victor.quota = 150.0;
    victor.profonda = 300.0;
    victor.punti = {acqua(f, f.proiettata({44.80, 146.45}), 500.0), acqua(f, f.proiettata({44.82, 146.85}), 500.0)};
    victor.ciclica = true;
    victor.entra_con = "victor";
    victor.nascosta = true;
    victor.ruolo = "sottomarino";
    s.unita.push_back(victor);
    zona(s, "cavo", {"il punto del cavo", "the cable site"}, cavo, 800.0);
    zona(s, "uscita", {"fuori dalle Curili", "outside the Kurils"}, partenza, 4000.0);
    s.obiettivi.push_back(obiettivo_zona("cavo", "raggiungere", cavo, 800.0, {"Raggiungere il punto del cavo", "Reach the cable site"}));
    s.obiettivi.push_back(obiettivo_variabile("lavoro", 0, 720, {"Restare due ore sul cavo mentre i palombari lavorano", "Stay over the cable for two hours while the divers work"}));
    s.obiettivi.push_back(obiettivo_variabile("uscita", 2, 1, {"Uscire dal Mare di Okhotsk dopo il lavoro", "Leave the Sea of Okhotsk after the work"}));
    s.obiettivi.push_back(obiettivo("occulto", "non_essere_scoperto", {}, {"Non farsi scoprire", "Avoid detection"}));
    s.obiettivi.push_back(obiettivo("passaggi", "scoprire", {1, 4}, {"Classificare le unita' che passano", "Classify the passing units"}, false));
    s.obiettivi.push_back(vivo());
    regola(s, "lavoro", {in_zona("cavo")}, {imposta(0, "+", 1)}, false);
    regola(s, "lavoro_finito", {variabile(0, ">", 719.0)}, {imposta(1, "=", 1)});
    regola(s, "nave_sul_cavo", {variabile(0, ">", 170.0)}, {azione(A::entra, 4)});
    regola(s, "nave_riparte", {scattata("nave_sul_cavo"), variabile(0, ">", 600.0)}, {vai(4, acqua(f, polare(cavo, 45.0, 30000.0), 50.0))});
    regola(s, "victor", {quando(C::allarme)}, {azione(A::entra, 5)});
    regola(s, "fuori", {variabile(1, "=", 1.0), in_zona("uscita")},
           {imposta(2, "=", 1), fine(40, {"Le registrazioni sono a bordo e il Los Angeles e' fuori dalle Curili.",
                                          "The recordings are aboard and Los Angeles is clear of the Kurils."})});
    s.briefing[0] = {
        "ORDINI DI MISSIONE", "DA: COMSUBPAC", "A: USS LOS ANGELES (SSN-688)", "OGG.: CAMPANE D'EDERA",
        "20 ottobre 1989, ore 0200Z. Teatro: Stretto di Ekaterina e Mare di Okhotsk.",
        "Sul fondo del Mare di Okhotsk, a centoventi metri, corre un cavo telefonico militare che i sovietici credono al sicuro. Da anni un nostro dispositivo lo ascolta.",
        "Passate lo Stretto di Ekaterina senza farvi sentire, portatevi sopra il cavo, quasi sul fondo, e restateci immobili due ore mentre i palombari cambiano le registrazioni.",
        "Poi uscite dalle Curili per la stessa strada. Nello stretto c'e' una Krivak di guardia; aerei ed elicotteri pattugliano lo Okhotsk.",
        "La previsione dice tempo in peggioramento.",
        "Regole d'ingaggio: tempo di pace. Armi trattenute; fuoco solo per salvare il battello.", "Tempo a disposizione: 4 ore e 30 minuti.",
        "Nota storica: ispirata all'operazione Ivy Bells degli anni Settanta; nomi e unita' sono di fantasia."};
    s.briefing[1] = {
        "MISSION ORDERS", "FM: COMSUBPAC", "TO: USS LOS ANGELES (SSN-688)", "SUBJ: IVY BELLS",
        "20 October 1989, 0200Z. Theatre: Yekaterina Strait and Sea of Okhotsk.",
        "On the floor of the Sea of Okhotsk, at four hundred feet, runs a military telephone cable the Soviets believe is safe. One of our devices has been listening to it for years.",
        "Pass the Yekaterina Strait unheard, settle over the cable, almost on the bottom, and stay there motionless for two hours while the divers change the tapes.",
        "Then leave the Kurils the way you came. A Krivak guards the strait; aircraft and helicopters patrol the Okhotsk.",
        "The forecast says the weather is getting worse.",
        "Rules of engagement: peacetime. Weapons hold; fire only to save the boat.", "Time available: 4 hours 30 minutes.",
        "Historical note: inspired by Operation Ivy Bells in the 1970s; names and units are fictional."};
    return s;
}

// LIB-S07 «Tappo»: minare il canale di Kola prima dell'alba della guerra. Le Mk 67 sono nel deposito dell'Enhanced
// (nel classico non ci sono mine: la missione si gioca, ma il campo non si posa).
ScenarioMissione tappo(const DatiScenario& d) {
    const Fondale f = Fondale::carica(DATI + "/teatri/barents.fdl");
    ScenarioMissione s;
    base(s, d, f);
    s.id = "LIB_S07_TAPPO";
    s.titolo = {"Tappo", "Cork"};
    s.teatro = "barents";
    s.giorno = 2;
    s.mese = 2;
    s.anno = 1989;
    s.ora_utc = 3.0;
    s.meteo_realistico = true;
    s.situazione = "guerra";
    s.paranoia = 2;
    s.difficolta = 4;
    s.durata_s = 9000.0;
    carico_688(s, true);
    const Vec2 campo = campo_minato(f, f.proiettata({69.345, 33.85}), 30.0, 90.0);
    const Vec2 golfo = acqua(f, f.proiettata({69.24, 33.53}), 150.0);   // dentro il golfo: l'Akula esce per il canale profondo
    const Vec2 ritirata = acqua(f, f.proiettata({69.68, 33.15}), 150.0);
    s.unita.push_back(los_angeles(acqua(f, f.proiettata({69.64, 34.10}), 150.0), 200.0, 60.0));
    const Vec2 guardia = polare(campo, 0.0, 11000.0);
    for (int k = 0; k < 2; ++k) {
        UnitaScenario u = unita(1 + k, "krivak2", "nave", "urss", acqua(f, polare(guardia, k ? 90.0 : 270.0, 7000.0), 60.0), k ? 270.0 : 90.0, 12.0, "R_nave");
        u.punti = {acqua(f, polare(guardia, k ? 270.0 : 90.0, 7000.0), 60.0), acqua(f, polare(guardia, k ? 90.0 : 270.0, 7000.0), 60.0)};
        u.ciclica = true;
        u.ruolo = "guardia del canale";
        s.unita.push_back(u);
    }
    UnitaScenario kara = unita(3, "kara", "nave", "urss", acqua(f, polare(campo, 20.0, 19000.0), 80.0), 0.0, 10.0, "R_nave");
    for (int q = 0; q < 4; ++q) kara.punti.push_back(acqua(f, polare(polare(campo, 20.0, 19000.0), 90.0 * q, 5000.0), 80.0));
    kara.ciclica = true;
    kara.ruolo = "guardia del canale";
    s.unita.push_back(kara);
    UnitaScenario il38 = unita(4, "il38", "aereo", "urss", polare(campo, 0.0, 25000.0), 90.0, 250.0, "R_aereo");
    for (int q = 0; q < 4; ++q) il38.punti.push_back(polare(campo, 45.0 + 90.0 * q, 20000.0));
    il38.ciclica = true;
    il38.ruolo = "pattugliatore";
    s.unita.push_back(il38);
    AereoAswScenario a;
    a.unita = 4;
    a.base = f.proiettata({69.03, 33.42});   // Severomorsk-1
    a.situazione = "guerra";
    a.contro_giocatore = true;
    a.zona = campo;
    a.raggio = 25000.0;
    s.aerei_asw.push_back(a);
    UnitaScenario akula = unita(5, "akula", "sottomarino", "urss", golfo, 0.0, 14.0, "R_sottomarino");
    akula.quota = 60.0;
    akula.profonda = 200.0;
    akula.punti = {acqua(f, f.proiettata({69.30, 33.52}), 150.0), acqua(f, f.proiettata({69.33, 33.56}), 150.0),
                   acqua(f, f.proiettata({69.36, 33.66}), 150.0), acqua(f, f.proiettata({69.50, 33.60}), 150.0),
                   acqua(f, f.proiettata({69.75, 33.60}), 200.0)};
    akula.esce = true;
    akula.entra_con = "akula_esce";
    akula.nascosta = true;
    akula.ruolo = "sottomarino in uscita";
    s.unita.push_back(akula);
    zona(s, "mare_aperto", {"il mare aperto", "open sea"}, acqua(f, f.proiettata({69.56, 33.6}), 150.0), 4000.0);
    zona(s, "ritirata", {"il punto di ritirata", "the withdrawal point"}, ritirata, 5000.0);
    ObiettivoScenario mine = obiettivo_zona("campo", "minare", campo, 1500.0, {"Posare almeno 6 mine nel canale", "Lay at least 6 mines in the channel"});
    mine.minimo = 6;
    mine.entro_s = 7200.0;
    s.obiettivi.push_back(mine);
    ObiettivoScenario via = obiettivo_zona("ritirata", "raggiungere", ritirata, 5000.0, {"Lasciare la zona entro due ore", "Leave the area within two hours"});
    via.entro_s = 7200.0;
    s.obiettivi.push_back(via);
    s.obiettivi.push_back(obiettivo("occulto", "non_essere_scoperto", {}, {"Non farsi scoprire durante la posa", "Stay undetected while laying"}, false));
    s.obiettivi.push_back(vivo());
    regola(s, "akula_esce", {quando(C::tempo, 2700.0)}, {azione(A::entra, 5)});
    regola(s, "akula_avverte", {unita_in(5, "mare_aperto")}, {azione(A::allarme)});
    regola(s, "akula_colpito", {unita_colpita(5)}, {azione(A::allarme)});
    regola(s, "ritirata", {quando(C::tempo, 1800.0), in_zona("ritirata")},
           {fine(40, {"Il Los Angeles e' al punto di ritirata. Fra poche ore le mine si armeranno.", "Los Angeles is at the withdrawal point. In a few hours the mines will arm."})});
    s.briefing[0] = {
        "ORDINI DI MISSIONE", "DA: COMSUBLANT", "A: USS LOS ANGELES (SSN-688)", "OGG.: TAPPO",
        "2 febbraio 1989, ore 0300Z. Teatro: Mare di Barents, approcci a Kola. Notte polare.",
        "La guerra comincia all'alba. Se il canale di Kola resta aperto, entro sera i sottomarini lanciamissili saranno nei loro bastioni.",
        "Avete dieci mine Mk 67 nel deposito. Portatevi a otto miglia dall'imboccatura del golfo e lanciatele lungo il canale: almeno sei devono posarsi nel campo.",
        "Poi andatevene al punto di ritirata prima che si armino e che la Flotta del Nord si svegli: entro due ore.",
        "Due Krivak e un Kara fanno la guardia al canale, un Il-38 vola sopra.",
        "Regole d'ingaggio: guerra. Armi libere.", "Tempo a disposizione: 2 ore e 30 minuti.",
        "Nota storica: missione inedita sullo schema del minamento offensivo, con l'idea dei porti minati degli anni Ottanta come dottrina; nomi e unita' sono di fantasia."};
    s.briefing[1] = {
        "MISSION ORDERS", "FM: COMSUBLANT", "TO: USS LOS ANGELES (SSN-688)", "SUBJ: CORK",
        "2 February 1989, 0300Z. Theatre: Barents Sea, Kola approaches. Polar night.",
        "The war begins at dawn. If the Kola channel stays open, by evening the missile submarines will be in their bastions.",
        "You have ten Mk 67 mines in the torpedo room. Close to eight miles from the inlet and launch them along the channel: at least six must settle in the field.",
        "Then get to the withdrawal point before they arm and the Northern Fleet wakes up: within two hours.",
        "Two Krivaks and a Kara guard the channel, an Il-38 flies overhead.",
        "Rules of engagement: war. Weapons free.", "Time available: 2 hours 30 minutes.",
        "Historical note: an original mission on the offensive minelaying pattern, with the mined harbours of the 1980s as doctrine; names and units are fictional."};
    return s;
}

// LIB-S08 «Valona»: l'Alfa porta fuori dalla baia la nave appoggio e quattro Foxtrot. Semplificazioni (docs/17): i
// battelli «trattenuti» sono neutrali (manca la parte terza); il Foxtrot che torna indietro lo convince il giocatore che
// entra nella sua zona. Le uscite si contano con una variabile, una per unita'.
ScenarioMissione valona(const DatiScenario& d) {
    const Fondale f = Fondale::carica(DATI + "/teatri/otranto.fdl");
    ScenarioMissione s;
    base(s, d, f);
    s.id = "LIB_S08_VALONA";
    s.titolo = {"Valona", "Valona"};
    s.teatro = "otranto";
    s.alfa = true;
    s.giorno = 26;
    s.mese = 5;
    s.anno = 1989;
    s.ora_utc = 5.0;
    s.situazione = "crisi";
    s.paranoia = 1;
    s.difficolta = 2;
    s.durata_s = 5400.0;
    carico_alfa(s);
    const Vec2 moli = acqua(f, f.proiettata({40.335, 19.43}), 12.0);
    const Vec2 p0 = acqua(f, f.proiettata({40.42, 19.425}), 15.0), p1 = acqua(f, f.proiettata({40.45, 19.375}), 20.0),
               p2 = acqua(f, f.proiettata({40.465, 19.30}), 25.0), p3 = acqua(f, f.proiettata({40.47, 19.18}), 60.0),
               p4 = acqua(f, f.proiettata({40.46, 19.10}), 100.0);
    s.unita.push_back(alfa(acqua(f, f.proiettata({40.44, 19.40}), 25.0), 300.0));
    s.unita[0].quota = 15.0;
    // la colonna: in testa la nave appoggio, poi i quattro Foxtrot ogni 600 m, in superficie a 12 nodi
    const double indietro = rilevamento(p0, moli);
    for (int k = 0; k < 5; ++k) {
        UnitaScenario u = k == 0 ? unita(1, "cargo", "nave", "urss", p0, rilevamento(p0, p1), 12.0, "", true)
                                 : unita(1 + k, "foxtrot", "sottomarino", "urss", acqua(f, polare(p0, indietro, 600.0 * k), 12.0), rilevamento(p0, p1), 12.0, "");
        u.punti = {p1, p2, p3, p4};
        u.ruolo = k == 0 ? "nave appoggio" : "in colonna";
        s.unita.push_back(u);
    }
    // i quattro trattenuti ai moli
    for (int k = 0; k < 4; ++k) {
        UnitaScenario u = unita(6 + k, "foxtrot", "sottomarino", "neutrale", acqua(f, polare(moli, 60.0 + 90.0 * k, 300.0), 12.0), 0.0, 0.0, "");
        u.ruolo = "trattenuto";
        s.unita.push_back(u);
    }
    UnitaScenario la = unita(10, "los_angeles", "sottomarino", "usa", acqua(f, f.proiettata({40.40, 19.10}), 300.0), 0.0, 4.0, "R_sottomarino");
    la.quota = 100.0;
    la.profonda = 250.0;
    la.punti = {acqua(f, f.proiettata({40.52, 19.08}), 300.0), acqua(f, f.proiettata({40.36, 19.12}), 300.0)};
    la.ciclica = true;
    la.ruolo = "sottomarino della NATO";
    s.unita.push_back(la);
    UnitaScenario orion = unita(11, "p3c", "aereo", "usa", polare(p3, 270.0, 20000.0), 90.0, 200.0, "R_aereo");
    for (int q = 0; q < 4; ++q) orion.punti.push_back(polare(p3, 45.0 + 90.0 * q, 15000.0));
    orion.ciclica = true;
    orion.ruolo = "pattugliatore";
    s.unita.push_back(orion);
    AereoAswScenario a;
    a.unita = 11;
    a.base = f.proiettata({37.40, 14.92});   // Sigonella
    a.situazione = "crisi";
    a.contro_giocatore = true;
    a.zona = p3;
    a.raggio = 20000.0;
    s.aerei_asw.push_back(a);
    // la svolta: il terzo Foxtrot (4) torna verso i moli; il giocatore che lo raggiunge lo riporta in colonna
    const Vec2 ripensamento = acqua(f, polare(p1, rilevamento(p1, moli), 2500.0), 15.0);
    zona(s, "fuori", {"fuori dalla baia", "out of the bay"}, p3, 3000.0);
    zona(s, "ripensamento", {"il Foxtrot che torna indietro", "the Foxtrot turning back"}, ripensamento, 1500.0);
    s.obiettivi.push_back(obiettivo_variabile("fuori", 0, 5, {"La nave appoggio e i quattro Foxtrot escono dalla baia", "The tender and the four Foxtrots leave the bay"}));
    s.obiettivi.push_back(obiettivo("colonna", "proteggere", {1, 2, 3, 4, 5}, {"Proteggere la colonna", "Protect the column"}));
    s.obiettivi.push_back(non_sparare({6, 7, 8, 9, 10, 11}, {"Non sparare a nessuno", "Do not fire on anyone"}));
    s.obiettivi.push_back(obiettivo("nato", "scoprire", {10}, {"Classificare il sottomarino della NATO", "Classify the NATO submarine"}, false));
    s.obiettivi.push_back(vivo());
    for (int u = 1; u <= 5; ++u) regola(s, "fuori_" + std::to_string(u), {unita_in(u, "fuori")}, {imposta(0, "+", 1)});
    regola(s, "ripensamento", {quando(C::tempo, 1200.0)}, {vai(4, ripensamento, 5.0)});
    regola(s, "convinto", {scattata("ripensamento"), in_zona("ripensamento")}, {vai(4, p3, 12.0)});
    regola(s, "tutti_fuori", {variabile(0, ">", 4.0)},
           {fine(40, {"La nave appoggio e i quattro battelli sono fuori dalla baia: la base di Pasha Liman e' finita.",
                      "The tender and the four boats are out of the bay: the Pasha Liman base is finished."})});
    s.briefing[0] = {
        "ORDINI DI MISSIONE", "DA: Comando della 5ª Eskadra", "A: K-463 (PROGETTO 705)", "OGG.: VALONA",
        "26 maggio 1989, ore 0500Z. Teatro: Canale d'Otranto, baia di Valona.",
        "Il paese ospite ha rotto con Mosca, e la base di Pasha Liman e' finita. Le truppe del porto non lasciano partire tutti: quattro sottomarini restano ai moli, trattenuti.",
        "Portate fuori dalla baia la nave appoggio e gli altri quattro Foxtrot, in colonna, fino al Canale d'Otranto.",
        "Fuori un sottomarino e un Orion della NATO guardano. Nessuno vuole sparare, e tutti hanno le armi pronte.",
        "Se un comandante esita, raggiungetelo: la vostra presenza bastera'.",
        "Regole d'ingaggio: crisi. Non sparate a nessuno.", "Tempo a disposizione: 1 ora e 30 minuti.",
        "Nota storica: ispirata alla ritirata sovietica da Valona (1961) e ai quattro sottomarini trattenuti, trasposta al 1989; nomi e unita' sono di fantasia."};
    s.briefing[1] = {
        "MISSION ORDERS", "FM: 5TH ESKADRA HQ", "TO: K-463 (PROJECT 705)", "SUBJ: VALONA",
        "26 May 1989, 0500Z. Theatre: Strait of Otranto, Bay of Valona.",
        "The host country has broken with Moscow, and the Pasha Liman base is finished. The port troops will not let everyone go: four submarines stay at the piers, detained.",
        "Take the tender and the other four Foxtrots out of the bay, in column, as far as the Strait of Otranto.",
        "Outside, a NATO submarine and an Orion are watching. Nobody wants to shoot, and everyone has weapons ready.",
        "If a captain hesitates, go to him: your presence will be enough.",
        "Rules of engagement: crisis. Do not fire on anyone.", "Time available: 1 hour 30 minutes.",
        "Historical note: inspired by the Soviet withdrawal from Valona (1961) and the four detained submarines, moved to 1989; names and units are fictional."};
    return s;
}

// LIB-S09 «Aport»: l'Alfa passa la soglia Islanda-Fær Øer sotto il SOSUS. Semplificazioni (docs/17): i cue SOSUS sono a
// tempo sulla rotta probabile (manca la condizione sulla velocita' del giocatore); la durata e' di 4 h 30' per dare
// senso al secondario «entro 4 ore».
ScenarioMissione aport(const DatiScenario& d) {
    const Fondale f = Fondale::carica(DATI + "/teatri/faroe.fdl");
    ScenarioMissione s;
    base(s, d, f);
    s.id = "LIB_S09_APORT";
    s.titolo = {"Aport", "Aport"};
    s.teatro = "faroe";
    s.alfa = true;
    s.giorno = 18;
    s.mese = 5;
    s.anno = 1985;
    s.ora_utc = 21.0;
    s.meteo_realistico = true;
    s.situazione = "pace";
    s.paranoia = 2;
    s.difficolta = 4;
    s.durata_s = 16200.0;
    carico_alfa(s);
    const Vec2 partenza = acqua(f, f.proiettata({62.98, -8.7}), 400.0), area = acqua(f, f.proiettata({62.30, -9.25}), 400.0);
    s.unita.push_back(alfa(partenza, rilevamento(partenza, area)));
    s.unita[0].quota = 150.0;
    // il Victor del gruppo, parallelo e piu' a ovest; scoperto a meta' soglia
    const Vec2 v0 = acqua(f, polare(partenza, 270.0, 15000.0), 400.0), v1 = acqua(f, polare(area, 270.0, 20000.0), 400.0);
    UnitaScenario victor = unita(1, "victor3", "sottomarino", "urss", v0, rilevamento(v0, v1), 10.0, "");
    victor.quota = 150.0;
    victor.punti = {v1};
    victor.ruolo = "sottomarino del gruppo";
    s.unita.push_back(victor);
    const Vec2 victor_scoperto = polare(v0, rilevamento(v0, v1), std::min(distanza(v0, v1), 5400.0 * 10.0 * NODO));
    UnitaScenario perry = unita(2, "perry", "nave", "usa", acqua(f, f.proiettata({62.68, -9.6}), 300.0), 90.0, 8.0, "R_nave");
    perry.punti = {acqua(f, f.proiettata({62.64, -8.4}), 300.0), acqua(f, f.proiettata({62.68, -9.6}), 300.0)};
    perry.ciclica = true;
    perry.ruolo = "fregata con la schiera";
    s.unita.push_back(perry);
    const Vec2 soglia = polare(partenza, rilevamento(partenza, area), distanza(partenza, area) * 0.5);
    for (int k = 0; k < 2; ++k) {
        const Vec2 c = k ? victor_scoperto : soglia;
        UnitaScenario p = unita(3 + k, "p3c", "aereo", "usa", polare(c, 180.0, 30000.0), 0.0, 200.0, "R_aereo");
        for (int q = 0; q < 4; ++q) p.punti.push_back(polare(c, 45.0 + 90.0 * q, 18000.0));
        p.ciclica = true;
        p.ruolo = "pattugliatore";
        s.unita.push_back(p);
        AereoAswScenario a;
        a.unita = 3 + k;
        a.base = f.proiettata({63.98, -22.6});   // Keflavík
        a.situazione = "pace";
        a.contro_giocatore = k == 0;
        if (k) a.bersagli = {1};
        a.zona = c;
        a.raggio = 30000.0;
        s.aerei_asw.push_back(a);
    }
    zona(s, "area", {"l'area di pattugliamento", "the patrol area"}, area, 10000.0);
    s.obiettivi.push_back(obiettivo_zona("area", "raggiungere", area, 10000.0, {"Raggiungere l'area di pattugliamento a sud della soglia", "Reach the patrol area south of the ridge"}));
    s.obiettivi.push_back(obiettivo("occulto", "non_essere_scoperto", {}, {"Passare senza un fix", "Get through without a fix"}));
    ObiettivoScenario presto = obiettivo_zona("in_tempo", "raggiungere", area, 10000.0, {"Arrivare entro 4 ore", "Arrive within 4 hours"}, false);
    presto.entro_s = 14400.0;
    s.obiettivi.push_back(presto);
    s.obiettivi.push_back(vivo());
    // il SOSUS: i datum arrivano con 30-60 minuti di ritardo sulla rotta probabile
    regola(s, "sosus_1", {quando(C::tempo, 3600.0)}, {cue(polare(partenza, rilevamento(partenza, area), distanza(partenza, area) * 0.3), 9000.0)});
    regola(s, "victor_scoperto", {quando(C::tempo, 5400.0)}, {cue(victor_scoperto, 6000.0)});
    regola(s, "sosus_2", {quando(C::tempo, 7200.0)}, {cue(soglia, 9000.0)});
    regola(s, "sosus_3", {quando(C::tempo, 10800.0)}, {cue(polare(partenza, rilevamento(partenza, area), distanza(partenza, area) * 0.75), 9000.0)});
    regola(s, "arrivati", {in_zona("area")}, {fine(40, {"L'Alfa e' nell'area di pattugliamento, a sud della soglia.", "The Alfa is in the patrol area, south of the ridge."})});
    s.briefing[0] = {
        "ORDINI DI MISSIONE", "DA: Comando della Flotta del Nord", "A: K-463 (PROGETTO 705)", "OGG.: APORT",
        "18 maggio 1985, ore 2100Z. Teatro: soglia Islanda-Fær Øer.",
        "La Flotta del Nord manda fuori insieme cinque sottomarini d'attacco: vogliamo sapere dove pattugliano i lanciamissili americani, e quanti contatti la NATO riesce a seguire nello stesso momento.",
        "Passate la soglia sotto il SOSUS e raggiungete l'area di pattugliamento a sud, senza che nessuno vi metta un fix.",
        "Siete il battello piu' veloce, e il piu' rumoroso quando correte. Ogni scatto e' un rapporto che parte per Keflavík, e un Orion sul punto un'ora dopo: correte a scatti, sotto lo strato, e lasciate invecchiare il datum.",
        "Un Victor III del gruppo passa piu' a ovest.",
        "Regole d'ingaggio: tempo di pace. Armi trattenute.", "Tempo a disposizione: 4 ore e 30 minuti.",
        "Nota storica: ispirata all'operazione Aport (1985); nomi e unita' sono di fantasia."};
    s.briefing[1] = {
        "MISSION ORDERS", "FM: NORTHERN FLEET HQ", "TO: K-463 (PROJECT 705)", "SUBJ: APORT",
        "18 May 1985, 2100Z. Theatre: Iceland-Faroe Ridge.",
        "The Northern Fleet is sending five attack submarines out together: we want to know where the American missile boats patrol, and how many contacts NATO can track at once.",
        "Cross the ridge under SOSUS and reach the patrol area to the south without anyone getting a fix on you.",
        "You are the fastest boat, and the noisiest when you run. Every sprint is a report on its way to Keflavík, and an Orion over the spot an hour later: sprint and drift, under the layer, and let the datum go stale.",
        "A Victor III of the group is passing further west.",
        "Rules of engagement: peacetime. Weapons hold.", "Time available: 4 hours 30 minutes.",
        "Historical note: inspired by Operation Aport (1985); names and units are fictional."};
    return s;
}

// LIB-S11 «Tenete il confine»: l'Alfa resta fuori dalle 12 miglia di Bornholm finche' il Foxtrot incagliato non e'
// rimorchiato fuori, poi lo copre fino al punto d'incontro. Il Foxtrot parte incagliato e un rimorchiatore del paese
// neutrale lo prende a rimorchio a 20 minuti (M6: `stato = "incagliata"`, m688.rimorchia). Semplificazioni (docs/17):
// le acque territoriali sono il cerchio di una zona, ed entrarci chiude la missione col fallimento; le navi del paese
// neutrale sono neutrali e non sparano.
ScenarioMissione tenete_il_confine(const DatiScenario& d) {
    const Fondale f = Fondale::carica(DATI + "/teatri/baltico.fdl");
    ScenarioMissione s;
    base(s, d, f);
    s.id = "LIB_S11_TENETE_IL_CONFINE";
    s.titolo = {"Tenete il confine", "Hold the Line"};
    s.teatro = "baltico";
    s.alfa = true;
    s.giorno = 28;
    s.mese = 10;
    s.anno = 1989;
    s.ora_utc = 7.0;
    s.situazione = "crisi";
    s.paranoia = 1;
    s.difficolta = 3;
    s.durata_s = 7200.0;
    carico_alfa(s);
    const Vec2 isola = f.proiettata({55.13, 14.92});
    const Vec2 scogli = acqua_fra(f, f.proiettata({54.99, 14.80}), 10.0, 20.0);
    const Vec2 incontro = acqua(f, polare(scogli, 170.0, 20000.0), 25.0);
    s.unita.push_back(alfa(acqua(f, polare(scogli, 150.0, 26000.0), 30.0), 330.0));
    s.unita[0].quota = 20.0;
    UnitaScenario foxtrot = unita(1, "foxtrot", "sottomarino", "urss", scogli, 200.0, 0.0, "");
    foxtrot.quota = 0.0;
    foxtrot.stato = "incagliata";
    foxtrot.ruolo = "incagliato";
    s.unita.push_back(foxtrot);
    // le navi del paese neutrale girano attorno al Foxtrot
    const char* const NEUTRALI[] = {"koni", "koni", "nanuchka"};
    for (int k = 0; k < 3; ++k) {
        const double r = 3000.0 + 1000.0 * k;
        UnitaScenario u = unita(2 + k, NEUTRALI[k], "nave", "neutrale", acqua(f, polare(scogli, 120.0 * k, r), 12.0), 90.0 + 120.0 * k, 8.0, "");
        for (int q = 0; q < 4; ++q) u.punti.push_back(acqua(f, polare(scogli, 120.0 * k + 90.0 * (q + 1), r), 12.0));
        u.ciclica = true;
        u.ruolo = "marina del paese neutrale";
        s.unita.push_back(u);
    }
    // i due eco radar: mercantili che arrivano da est e passano
    for (int k = 0; k < 2; ++k) {
        UnitaScenario u = unita(5 + k, "mercantile", "nave", "neutrale", acqua(f, polare(isola, 115.0 + 6.0 * k, 50000.0), 25.0), 270.0, 12.0, "", true);
        u.punti = {acqua(f, f.proiettata({54.86 - 0.02 * k, 15.0}), 25.0), acqua(f, f.proiettata({54.70 - 0.02 * k, 15.65}), 25.0)};
        u.esce = true;
        u.entra_con = "eco_radar";
        u.ruolo = "eco radar";
        s.unita.push_back(u);
    }
    // il rimorchiatore del paese neutrale: aspetta vicino agli scogli, poi porta il Foxtrot all'incontro
    UnitaScenario rimorchiatore = unita(8, "mercantile", "nave", "neutrale", acqua(f, polare(scogli, 300.0, 1500.0), 12.0), 120.0, 0.0, "", true);
    rimorchiatore.ruolo = "rimorchiatore";
    s.unita.push_back(rimorchiatore);
    UnitaScenario la = unita(7, "los_angeles", "sottomarino", "usa", acqua(f, polare(isola, 250.0, 35000.0), 40.0), 0.0, 4.0, "R_sottomarino");
    la.quota = 25.0;
    la.profonda = 30.0;
    la.punti = {acqua(f, polare(isola, 270.0, 35000.0), 40.0), acqua(f, polare(isola, 230.0, 35000.0), 40.0)};
    la.ciclica = true;
    la.ruolo = "sottomarino della NATO";
    s.unita.push_back(la);
    zona(s, "acque", {"le acque territoriali", "territorial waters"}, polare(isola, 180.0, 4000.0), 22000.0);
    zona(s, "incontro", {"il punto d'incontro", "the rendezvous"}, incontro, 3000.0);
    s.obiettivi.push_back(obiettivo_variabile("a_casa", 0, 1, {"Coprire il Foxtrot fino al punto d'incontro", "Cover the Foxtrot to the rendezvous"}));
    s.obiettivi.push_back(obiettivo("foxtrot", "proteggere", {1}, {"Proteggere il Foxtrot", "Protect the Foxtrot"}));
    s.obiettivi.push_back(non_sparare({2, 3, 4, 5, 6, 7}, {"Non sparare a nessuno", "Do not fire on anyone"}));
    s.obiettivi.push_back(obiettivo("nato", "scoprire", {7}, {"Classificare il sottomarino della NATO", "Classify the NATO submarine"}, false));
    s.obiettivi.push_back(vivo());
    regola(s, "confine", {in_zona("acque")},
           {fine(41, {"L'Alfa e' entrato nelle acque territoriali: la crisi diventa un'altra cosa.", "The Alfa has entered territorial waters: the crisis becomes something else."},
                 "fallimento")});
    AzioneScenario traino = azione(A::rimorchia, 8);
    traino.bersaglio = 1;
    traino.velocita = 7.0;   // 7 nodi: all'incontro verso 100 minuti (a 6 arrivava a ridosso delle due ore)
    traino.valore = 300;
    regola(s, "rimorchio", {quando(C::tempo, 1200.0)}, {traino, vai(8, incontro, 7.0)});
    const auto vigila = [](int id) { AzioneScenario c = azione(A::comportamento, id); c.comportamento = "R_vigila"; return c; };   // in allarme fanno ping
    regola(s, "eco_radar", {quando(C::tempo, 3000.0)},
           {azione(A::entra, 5), azione(A::entra, 6), azione(A::allarme), vigila(2), vigila(3), vigila(4)});
    regola(s, "a_casa", {unita_in(1, "incontro")},
           {imposta(0, "=", 1), fine(40, {"Il Foxtrot e' al punto d'incontro, fuori dalle acque del paese neutrale.",
                                          "The Foxtrot is at the rendezvous, outside the neutral country's waters."})});
    s.briefing[0] = {
        "ORDINI DI MISSIONE", "DA: Comando della Flotta del Baltico", "A: K-463 (PROGETTO 705)", "OGG.: TENETE IL CONFINE",
        "28 ottobre 1989, ore 0700Z. Teatro: Mar Baltico, presso Bornholm.",
        "Un nostro sottomarino diesel si e' incagliato sugli scogli, dentro le acque di un paese neutrale. Il paese non lo restituisce finche' non avra' finito le sue ispezioni; poi un suo rimorchiatore lo portera' fuori.",
        "Le sue navi gli girano attorno, e un sottomarino della NATO osserva da lontano.",
        "Restate appena fuori dal confine delle dodici miglia, pronti e invisibili. Quando i rimorchiatori lo porteranno fuori, copritelo fino al punto d'incontro.",
        "Un passo oltre la linea, e la crisi diventa un'altra cosa.",
        "Regole d'ingaggio: crisi. Non sparate; vietato entrare nelle 12 miglia.", "Tempo a disposizione: 2 ore.",
        "Nota storica: ispirata all'incaglio del sottomarino U-137 (1981), con un paese neutrale di fantasia; l'inchiesta e le versioni contrastanti restano aperte; nomi e unita' sono di fantasia."};
    s.briefing[1] = {
        "MISSION ORDERS", "FM: BALTIC FLEET HQ", "TO: K-463 (PROJECT 705)", "SUBJ: HOLD THE LINE",
        "28 October 1989, 0700Z. Theatre: Baltic Sea, off Bornholm.",
        "One of our diesel submarines has run aground on the rocks, inside the waters of a neutral country. The country will not release her until its inspections are over; then one of its tugs will bring her out.",
        "Its ships are circling her, and a NATO submarine is watching from a distance.",
        "Stay just outside the twelve-mile line, ready and unseen. When the tugs bring her out, cover her to the rendezvous.",
        "One step over the line, and the crisis becomes something else.",
        "Rules of engagement: crisis. Do not fire; entering the 12 miles is forbidden.", "Time available: 2 hours.",
        "Historical note: inspired by the grounding of submarine U-137 (1981), with a fictional neutral country; the inquiry and the conflicting accounts remain open; names and units are fictional."};
    return s;
}

// LIB-S12 «Il gruppo cacciatore»: due Perry e un SH-60B danno la caccia all'Alfa; una fregata rallenta per provare il
// sonar, e se l'Alfa lancia l'altra accorre e l'elicottero cala sul punto.
ScenarioMissione gruppo_cacciatore(const DatiScenario& d) {
    const Fondale f = Fondale::carica(DATI + "/teatri/otranto.fdl");
    ScenarioMissione s;
    base(s, d, f);
    s.id = "LIB_S12_GRUPPO_CACCIATORE";
    s.titolo = {"Il gruppo cacciatore", "The Hunter Group"};
    s.teatro = "otranto";
    s.alfa = true;
    s.giorno = 9;
    s.mese = 12;
    s.anno = 1989;
    s.ora_utc = 19.5;
    s.situazione = "guerra";
    s.paranoia = 2;
    s.difficolta = 3;
    s.durata_s = 5400.0;
    carico_alfa(s);
    const Vec2 partenza = acqua_fra(f, f.proiettata({40.42, 18.50}), 110.0, 200.0);
    const Vec2 uscita = acqua(f, f.proiettata({40.30, 19.0}), 500.0);
    s.unita.push_back(alfa(partenza, 120.0));
    s.unita[0].quota = 60.0;
    // le due fregate battono i lati del datum vecchio; la seconda rallenta per provare il sonar
    for (int k = 0; k < 2; ++k) {
        const Vec2 c = polare(partenza, k ? 60.0 : 240.0, 7000.0);
        UnitaScenario u = unita(1 + k, "perry", "nave", "usa", acqua(f, polare(c, 0.0, 4000.0), 60.0), 90.0, k ? 15.0 : 18.0, "R_nave");
        for (int q = 0; q < 4; ++q) u.punti.push_back(acqua(f, polare(c, 90.0 * (q + 1), 4000.0), 60.0));
        u.ciclica = true;
        u.ruolo = "fregata";
        s.unita.push_back(u);
    }
    const Vec2 prova = acqua(f, polare(partenza, 60.0, 5000.0), 60.0);
    UnitaScenario elicottero = unita(3, "sh60b", "aereo", "usa", polare(partenza, 240.0, 7000.0), 60.0, 100.0, "R_aereo");
    for (int q = 0; q < 4; ++q) elicottero.punti.push_back(polare(partenza, 45.0 + 90.0 * q, 6000.0));
    elicottero.ciclica = true;
    elicottero.ruolo = "elicottero ASW";
    s.unita.push_back(elicottero);
    AereoAswScenario a;
    a.unita = 3;
    a.base_unita = 1;
    a.situazione = "guerra";
    a.contro_giocatore = true;
    a.zona = partenza;
    a.raggio = 15000.0;
    s.aerei_asw.push_back(a);
    zona(s, "uscita", {"fuori dalla zona di caccia", "out of the hunting area"}, uscita, 8000.0);
    s.obiettivi.push_back(vivo());
    s.obiettivi.push_back(obiettivo("fregate", "affondare", {1, 2}, {"Affondare almeno una fregata", "Sink at least one frigate"}));
    s.obiettivi.back().minimo = 1;
    s.obiettivi.push_back(obiettivo_zona("uscita", "raggiungere", uscita, 8000.0, {"Uscire dalla zona di caccia", "Get out of the hunting area"}, false));
    regola(s, "prova_sonar", {quando(C::tempo, 1200.0)}, {vai(2, prova, 12.0)});
    regola(s, "fine_prova", {quando(C::tempo, 2100.0)}, {vai(2, polare(partenza, 300.0, 6000.0), 20.0)});
    regola(s, "lancio", {quando(C::giocatore_ha_lanciato)}, {azione(A::allarme), vai(1, polare(prova, 180.0, 3000.0), 28.0), cue(prova, 3.0 * MIGLIO)});
    regola(s, "prima_fregata", {unita_fuori(1)}, {imposta(0, "+", 1)});
    regola(s, "seconda_fregata", {unita_fuori(2)}, {imposta(0, "+", 1)});
    regola(s, "rotta_la_caccia", {variabile(0, ">", 0.0), in_zona("uscita")},
           {fine(40, {"La caccia e' rotta: l'Alfa e' fuori dalla zona.", "The hunt is broken: the Alfa is out of the area."})});
    s.briefing[0] = {
        "ORDINI DI MISSIONE", "DA: Comando della 5ª Eskadra", "A: K-463 (PROGETTO 705)", "OGG.: IL GRUPPO CACCIATORE",
        "9 dicembre 1989, ore 1930Z. Teatro: Canale d'Otranto. Notte.",
        "Due fregate e un elicottero vi cercano da sei ore. Hanno un datum vecchio e tanta pazienza.",
        "Avete l'ordine di sopravvivere e, se potete, di rompere la caccia: affondate almeno una fregata, poi uscite dalla zona verso il mare profondo, a sud-est.",
        "Prima o poi una fregata fara' un errore. Ma l'occasione puo' anche essere un'esca: appena lancerete, l'altra accorrera' e l'elicottero calera' il sonar sul punto.",
        "Regole d'ingaggio: guerra. Armi libere.", "Tempo a disposizione: 1 ora e 30 minuti.",
        "Nota storica: ispirata alla dottrina dei gruppi cacciatori e del sottomarino che ribalta la caccia (1971), senza bandiere ne' navi di quei paesi; nomi e unita' sono di fantasia."};
    s.briefing[1] = {
        "MISSION ORDERS", "FM: 5TH ESKADRA HQ", "TO: K-463 (PROJECT 705)", "SUBJ: THE HUNTER GROUP",
        "9 December 1989, 1930Z. Theatre: Strait of Otranto. Night.",
        "Two frigates and a helicopter have been looking for you for six hours. They have an old datum and plenty of patience.",
        "Your orders are to survive and, if you can, to break the hunt: sink at least one frigate, then get out of the area toward deep water, to the south-east.",
        "Sooner or later a frigate will make a mistake. But the opportunity may be bait: as soon as you fire, the other will rush in and the helicopter will dip its sonar on the spot.",
        "Rules of engagement: war. Weapons free.", "Time available: 1 hour 30 minutes.",
        "Historical note: inspired by the doctrine of hunter groups and of the submarine that turns the hunt (1971), without the flags or ships of those countries; names and units are fictional."};
    return s;
}

// LIB-S05 «Il guardiano del cavo»: l'Alfa scorta la nave di soccorso che ispeziona il cavo dell'Okhotsk; un Los Angeles
// fermo vicino al cavo si trova solo col sonar attivo. Al ping entro 3 km fugge verso lo stretto, e la radio (E6) cambia
// gli ordini: «costringerlo ad andarsene», armi solo su un bersaglio identificato. Semplificazioni (docs/17): la nave
// di soccorso arriva sul punto e si ferma; uscito l'intruso dallo stretto, l'esito e' 40.
ScenarioMissione guardiano_del_cavo(const DatiScenario& d) {
    const Fondale f = Fondale::carica(DATI + "/teatri/kunashir.fdl");
    ScenarioMissione s;
    base(s, d, f);
    s.id = "LIB_S05_GUARDIANO_DEL_CAVO";
    s.titolo = {"Il guardiano del cavo", "The Cable Guard"};
    s.teatro = "kunashir";
    s.alfa = true;
    s.giorno = 22;
    s.mese = 10;
    s.anno = 1989;
    s.ora_utc = 6.0;
    s.meteo_realistico = true;
    s.situazione = "pace";
    s.paranoia = 2;
    s.difficolta = 3;
    s.durata_s = 7200.0;
    carico_alfa(s);
    const Vec2 sud = acqua(f, f.proiettata({44.45, 146.76}), 300.0);
    const Vec2 cavo = acqua_fra(f, f.proiettata({44.58, 146.50}), 110.0, 140.0);
    const Vec2 partenza_nave = acqua(f, polare(cavo, 45.0, 12000.0), 100.0);
    s.unita.push_back(alfa(acqua(f, polare(partenza_nave, 225.0, 2000.0), 100.0), 225.0));
    s.unita[0].quota = 60.0;
    UnitaScenario nave = unita(1, "mercantile", "nave", "urss", partenza_nave, rilevamento(partenza_nave, cavo), 8.0, "", true);
    nave.punti = {cavo};
    nave.ruolo = "nave di soccorso";
    s.unita.push_back(nave);
    UnitaScenario ka27 = unita(2, "ka27", "aereo", "urss", polare(cavo, 0.0, 10000.0), 90.0, 100.0, "R_aereo");
    for (int q = 0; q < 4; ++q) ka27.punti.push_back(polare(cavo, 45.0 + 90.0 * q, 6000.0));
    ka27.ciclica = true;
    ka27.ruolo = "elicottero ASW";
    s.unita.push_back(ka27);
    // l'intruso: fermo quasi sul fondo, a qualche centinaio di metri dal punto
    const Vec2 nascondiglio = acqua_fra(f, polare(cavo, 90.0, 400.0), 105.0, 150.0);
    UnitaScenario la = unita(3, "los_angeles", "sottomarino", "usa", nascondiglio, 270.0, 0.0, "");
    la.quota = f.profondita(nascondiglio) - 12.0;
    la.nascosta = true;
    la.ruolo = "intruso";
    s.unita.push_back(la);
    AereoAswScenario a;
    a.unita = 2;
    a.base = f.proiettata({44.02, 145.95});   // Južno-Kuril'sk
    a.situazione = "pace";
    a.contro_giocatore = false;
    a.bersagli = {3};
    a.zona = cavo;
    a.raggio = 10000.0;
    s.aerei_asw.push_back(a);
    zona(s, "cavo", {"il punto del cavo", "the cable site"}, cavo, 800.0);
    zona(s, "stretto", {"lo Stretto di Ekaterina", "the Yekaterina Strait"}, sud, 4000.0);
    s.obiettivi.push_back(obiettivo("nave", "proteggere", {1}, {"Proteggere la nave di soccorso", "Protect the rescue ship"}));
    // trovarlo, non classificarlo (collaudo del 29/10/2026): fermo a 12 m dal fondo e poi in fuga a 130 m, il Los Angeles non
    // cavita, e col sonarista nuovo la classe arriva solo da vicino o con la cavitazione: «scoprire» era irraggiungibile
    ObiettivoScenario intruso = obiettivo("intruso", "contatto", {3}, {"Trovare l'intruso: tre minuti di contatto", "Find the intruder: three minutes of contact"});
    intruso.minuti = 3.0;
    s.obiettivi.push_back(intruso);
    s.obiettivi.push_back(obiettivo_variabile("cacciato", 0, 1, {"Costringerlo ad andarsene", "Force him to leave"}, false));
    ObiettivoScenario senza_armi = non_sparare({3}, {"Senza sparare", "Without firing"});
    senza_armi.primario = false;
    s.obiettivi.push_back(senza_armi);
    s.obiettivi.push_back(vivo());
    CondizioneScenario ping = quando(C::ping_vicino, 3000.0);
    ping.unita = 3;
    regola(s, "trovato", {ping}, {vai(3, sud, 20.0), manda("CACCIA")});
    regola(s, "sul_cavo", {unita_in(1, "cavo")}, {manda("SUL_CAVO")});
    regola(s, "andato", {unita_in(3, "stretto")},
           {imposta(0, "=", 1), fine(40, {"L'intruso ha lasciato il Mare di Okhotsk. La nave di soccorso puo' lavorare sul cavo.",
                                          "The intruder has left the Sea of Okhotsk. The rescue ship can work on the cable."})});
    regola(s, "nave_persa", {unita_fuori(1)},
           {fine(41, {"La nave di soccorso e' perduta.", "The rescue ship is lost."}, "fallimento")});
    RadioScenario r;
    r.ordini = OrdiniScenario{"KP TOF", "K-463 (PROEKT 705)", "OKHRANA KABELYA", "SOV. SEKRETNO"};
    r.roe = {{"sottomarini", "trattenuto"}, {"navi", "trattenuto"}, {"mercantili", "trattenuto"}, {"neutrali", "trattenuto"}, {"aerei", "trattenuto"}};
    r.messaggi = {
        radio("INFORMAZIONE", 300.0, "immediate", "KP TOF", "INFORMATSIYA", "SOV. SEKRETNO",
              {"FONTE ATTENDIBILE: SUL CAVO POTREBBE ESSERCI UN DISPOSITIVO STRANIERO, E QUALCUNO CHE LO SORVEGLIA. CERCARE COL SONAR ATTIVO.",
               "RELIABLE SOURCE: THERE MAY BE A FOREIGN DEVICE ON THE CABLE, AND SOMEONE GUARDING IT. SEARCH WITH ACTIVE SONAR."}),
        radio("CACCIA", -1.0, "flash", "KP TOF", "VYTESNIT'", "SOV. SEKRETNO",
              {"INTRUSO SCOPERTO. COSTRINGERLO AD ANDARSENE: SEGUIRLO FINO ALLO STRETTO. ARMI SOLO SE ATTACCA.",
               "INTRUDER FOUND. FORCE HIM OUT: FOLLOW HIM TO THE STRAIT. WEAPONS ONLY IF HE ATTACKS."},
              {{"sottomarini", "stretto"}}),
        radio("SUL_CAVO", -1.0, "priority", "KP TOF", "SPASATEL'", "SEKRETNO",
              {"LA NAVE DI SOCCORSO E' SUL PUNTO E CALA IL VEICOLO SUBACQUEO.", "THE RESCUE SHIP IS ON STATION AND LOWERING THE SUBMERSIBLE."})};
    s.radio = r;
    s.briefing[0] = {
        "ORDINI DI MISSIONE", "DA: Comando della Flotta del Pacifico", "A: K-463 (PROGETTO 705)", "OGG.: IL GUARDIANO DEL CAVO",
        "22 ottobre 1989, ore 0600Z. Teatro: Mare di Okhotsk.",
        "Un'informazione arrivata da molto lontano dice che sul nostro cavo dell'Okhotsk c'e' qualcosa che non dovrebbe esserci.",
        "Una nave di soccorso va a ispezionarlo. Scortatela fino al punto e tenete lontani gli intrusi.",
        "Se l'intruso c'e', sara' fermo sul fondo: nessun sonar passivo lo sente. Usate il sonar attivo.",
        "Regole d'ingaggio: tempo di pace. Armi trattenute; gli ordini possono cambiare per radio.", "Tempo a disposizione: 2 ore.",
        "Nota storica: ispirata al lato sovietico dell'operazione Ivy Bells; quando e come il dispositivo fu scoperto resta incerto; nomi e unita' sono di fantasia."};
    s.briefing[1] = {
        "MISSION ORDERS", "FM: PACIFIC FLEET HQ", "TO: K-463 (PROJECT 705)", "SUBJ: THE CABLE GUARD",
        "22 October 1989, 0600Z. Theatre: Sea of Okhotsk.",
        "Information from very far away says there is something on our Okhotsk cable that should not be there.",
        "A rescue ship is going to inspect it. Escort her to the site and keep intruders away.",
        "If the intruder is there, he will be stopped on the bottom: no passive sonar can hear him. Use active sonar.",
        "Rules of engagement: peacetime. Weapons hold; orders may change by radio.", "Time available: 2 hours.",
        "Historical note: inspired by the Soviet side of Operation Ivy Bells; when and how the device was found remains uncertain; names and units are fictional."};
    return s;
}

// --- inedite sbloccate dalle rapide v2 (26/10/2026) ---

CondizioneScenario su(C tipo, int u, double valore, int bersaglio = -1) {
    CondizioneScenario c = quando(tipo, valore);
    c.unita = u;
    c.bersaglio = bersaglio;
    return c;
}
CondizioneScenario compiuto(const std::string& obiettivo) {
    CondizioneScenario c = quando(C::obiettivo_compiuto);
    c.obiettivo = obiettivo;
    return c;
}
CondizioneScenario a_quota(double da, double a) {
    CondizioneScenario c = quando(C::giocatore_a_quota, da);
    c.fino_a = a;
    return c;
}
ObiettivoScenario obiettivo_contatto(const std::string& id, std::vector<int> unita, double minuti, TestiScenario testo, bool primario = true) {
    ObiettivoScenario o = obiettivo(id, "contatto", std::move(unita), std::move(testo), primario);
    o.minuti = minuti;
    return o;
}

// LIB-I01 «La balena di ferro»: il Victor che il SOSUS chiama «la balena» e l'Akula che pedina il pedinatore.
// Semplificazioni (docs/17): la svolta scatta a 50 minuti o dopo 15 minuti di contatto sul Victor (al posto di «entro
// 4 nm», che e' una distanza fra due unita' in moto); l'Akula entra alle spalle della posizione di partenza; «non farsi
// scoprire» e' secondario, perche' il pedinamento e' gia' una scoperta: il primario e' seminare il pedinatore.
ScenarioMissione balena_di_ferro(const DatiScenario& d) {
    const Fondale f = Fondale::carica(DATI + "/teatri/faroe.fdl");
    ScenarioMissione s;
    base(s, d, f);
    s.id = "LIB_I01_BALENA_DI_FERRO";
    s.titolo = {"La balena di ferro", "The Iron Whale"};
    s.teatro = "faroe";
    s.giorno = 3;
    s.mese = 3;
    s.anno = 1989;
    s.ora_utc = 4.0 + 40.0 / 60.0;
    s.meteo_realistico = true;
    s.situazione = "pace";
    s.paranoia = 2;
    s.difficolta = 4;
    s.durata_s = 9000.0;
    carico_688(s);
    const Vec2 v0 = acqua(f, f.proiettata({63.15, -7.9}), 400.0), v1 = acqua(f, f.proiettata({62.75, -8.9}), 400.0),
               v2 = acqua(f, f.proiettata({62.45, -9.8}), 400.0);
    const Vec2 partenza = acqua(f, f.proiettata({62.60, -9.0}), 400.0);
    s.unita.push_back(los_angeles(partenza, rilevamento(partenza, v1), 120.0));
    // il Victor scende la dorsale a sei nodi, a zig-zag
    UnitaScenario victor = unita(1, "victor3", "sottomarino", "urss", v0, rilevamento(v0, v1), 6.0, "");
    victor.quota = 150.0;
    victor.punti = {acqua(f, polare(polare(v0, rilevamento(v0, v1), distanza(v0, v1) * 0.5), rilevamento(v0, v1) + 90.0, 4000.0), 400.0), v1,
                    acqua(f, polare(polare(v1, rilevamento(v1, v2), distanza(v1, v2) * 0.5), rilevamento(v1, v2) - 90.0, 4000.0), 400.0), v2};
    victor.ruolo = "la «balena»";
    s.unita.push_back(victor);
    UnitaScenario akula = unita(2, "akula", "sottomarino", "urss", acqua(f, polare(partenza, rilevamento(v1, partenza), 12000.0), 400.0),
                                rilevamento(v1, partenza) + 180.0, 8.0, "R_pedina");
    akula.quota = 200.0;
    akula.profonda = 300.0;
    akula.parametri.distanza_nm = 2.0;
    akula.entra_con = "svolta";
    akula.nascosta = true;
    akula.ruolo = "pedinatore";
    s.unita.push_back(akula);
    UnitaScenario orion = unita(3, "p3c", "aereo", "usa", acqua(f, f.proiettata({62.2, -10.0}), 0.0), 45.0, 200.0, "");
    orion.punti = {polare(partenza, 0.0, 10000.0), f.proiettata({63.4, -6.0})};
    orion.esce = true;
    orion.ruolo = "pattugliatore di passaggio";
    s.unita.push_back(orion);
    const Vec2 ivan = acqua(f, polare(v1, rilevamento(v0, v1) + 90.0, 12000.0), 400.0);
    s.obiettivi.push_back(obiettivo("balena", "scoprire", {1}, {"Classificare la «balena di ferro»", "Classify the 'iron whale'"}));
    s.obiettivi.push_back(obiettivo_contatto("ascolto", {1}, 40.0, {"Tenerla in contatto sonar per 40 minuti", "Hold her on sonar for 40 minutes"}));
    s.obiettivi.push_back(obiettivo_variabile("seminato", 1, 1, {"Seminare il pedinatore", "Shake off the trailer"}));
    s.obiettivi.push_back(obiettivo("occulto", "non_essere_scoperto", {}, {"Non farsi scoprire", "Avoid detection"}, false));
    s.obiettivi.push_back(obiettivo("secondo", "scoprire", {2}, {"Classificare il secondo battello", "Classify the second boat"}, false));
    s.obiettivi.push_back(vivo());
    // la svolta: l'Akula entra e il Victor fa il «Crazy Ivan», a 50 minuti o dopo un quarto d'ora di contatto
    const std::vector<AzioneScenario> svolta = {imposta(0, "=", 1), azione(A::entra, 2), vai(1, ivan, 14.0)};
    regola(s, "svolta", {quando(C::tempo, 3000.0), variabile(0, "=", 0.0)}, svolta);
    regola(s, "svolta_contatto", {su(C::contatto_su, 1, 15.0), variabile(0, "=", 0.0)}, svolta);
    regola(s, "dopo_la_svolta", {variabile(0, "=", 1.0)}, {imposta(2, "+", 1)}, false);   // un passo ogni 10 s
    regola(s, "fine_ivan", {variabile(2, ">", 60.0)}, {vai(1, v2, 6.0)});
    regola(s, "seminato", {variabile(2, ">", 120.0), su(C::unita_non_vede, 2, 20.0)}, {imposta(1, "=", 1)});
    regola(s, "a_casa", {variabile(1, "=", 1.0), compiuto("ascolto")},
           {fine(40, {"La firma e' in archivio e il pedinatore e' rimasto indietro.", "The signature is on file and the trailer has been left behind."})});
    s.briefing[0] = {
        "ORDINI DI MISSIONE", "DA: COMSUBLANT", "A: USS LOS ANGELES (SSN-688)", "OGG.: LA BALENA DI FERRO",
        "3 marzo 1989, ore 0440Z. Teatro: dorsale Islanda-Fær Øer.",
        "Da tre giorni il SOSUS di Keflavík segue un contatto che non torna con nessuna firma in archivio: un monoalbero quieto che scende la dorsale a sei nodi. A Norfolk lo chiamano «la balena di ferro».",
        "Trovatelo, classificatelo e restategli attaccato finche' l'archivio non e' pieno: almeno quaranta minuti di contatto.",
        "Senza che sappia di avere compagnia. E se qualcuno vi si mette dietro, seminatelo prima di tornare.",
        "Regole d'ingaggio: tempo di pace. Armi trattenute per tutta la missione.", "Tempo a disposizione: 2 ore e 30 minuti.",
        "Nota storica: missione inedita, nello spirito dei pedinamenti degli anni Ottanta; nomi e unita' sono di fantasia."};
    s.briefing[1] = {
        "MISSION ORDERS", "FM: COMSUBLANT", "TO: USS LOS ANGELES (SSN-688)", "SUBJ: THE IRON WHALE",
        "3 March 1989, 0440Z. Theatre: Iceland-Faroe Ridge.",
        "For three days Keflavík SOSUS has been tracking a contact that matches nothing on file: a quiet single screw coming down the ridge at six knots. In Norfolk they call it 'the iron whale'.",
        "Find it, classify it and stay on it until the file is full: at least forty minutes of contact.",
        "Without it knowing it has company. And if someone gets behind you, shake him off before coming home.",
        "Rules of engagement: peacetime. Weapons hold for the whole mission.", "Time available: 2 hours 30 minutes.",
        "Historical note: an original mission, in the spirit of the trailing operations of the 1980s; names and units are fictional."};
    return s;
}

// LIB-I02 «Il pescatore di Teriberka»: l'Alfa scorta il Typhoon che rientra, con un peschereccio norvegese sulla
// rotta e un Los Angeles in agguato. La radio (E6): a 45 minuti l'ordine falso di emergere al punto ALFA, a 70 quello
// vero, autenticato, che libera le armi contro il sottomarino straniero; il meteo e' fissato (mare 3, neve, 2 km).
// Semplificazioni (docs/17): la «foto» dell'Havørn e' classificarlo. Se il Los Angeles tiene il Typhoon per due minuti,
// il Typhoon accosta verso il golfo a 12 nodi.
ScenarioMissione pescatore_di_teriberka(const DatiScenario& d) {
    const Fondale f = Fondale::carica(DATI + "/teatri/barents.fdl");
    ScenarioMissione s;
    base(s, d, f);
    s.id = "LIB_I02_PESCATORE_DI_TERIBERKA";
    s.titolo = {"Il pescatore di Teriberka", "The Teriberka Fisherman"};
    s.teatro = "barents";
    s.alfa = true;
    s.giorno = 12;
    s.mese = 12;
    s.anno = 1989;
    s.ora_utc = 10.5;
    s.meteo_realistico = true;
    s.situazione = "crisi";
    s.paranoia = 2;
    s.difficolta = 3;
    s.durata_s = 7200.0;
    carico_alfa(s);
    const Vec2 rientro = acqua(f, f.proiettata({69.80, 33.6}), 200.0), golfo = acqua(f, f.proiettata({69.60, 33.15}), 120.0);
    const double rotta = rilevamento(rientro, golfo);
    s.unita.push_back(alfa(acqua(f, polare(rientro, rotta, 3000.0), 200.0), rotta));
    UnitaScenario typhoon = unita(1, "typhoon", "sottomarino", "urss", rientro, rotta, 10.0, "");
    typhoon.quota = 60.0;
    typhoon.punti = {golfo};
    typhoon.ruolo = "il Typhoon che rientra";
    s.unita.push_back(typhoon);
    const Vec2 pesca = acqua(f, f.proiettata({69.70, 33.38}), 120.0);
    UnitaScenario havorn = unita(2, "peschereccio", "nave", "neutrale", pesca, 0.0, 3.5, "", true);
    for (int q = 0; q < 4; ++q) havorn.punti.push_back(acqua(f, polare(pesca, 90.0 * q, 2500.0), 120.0));
    havorn.ciclica = true;
    havorn.ruolo = "l'Havørn";
    s.unita.push_back(havorn);
    UnitaScenario la = unita(3, "los_angeles", "sottomarino", "usa", acqua(f, f.proiettata({69.72, 33.25}), 110.0), 30.0, 3.0, "R_sottomarino");
    la.quota = 80.0;
    la.profonda = 100.0;
    la.punti = {acqua(f, f.proiettata({69.76, 33.45}), 110.0), acqua(f, f.proiettata({69.70, 33.15}), 110.0)};
    la.ciclica = true;
    la.nascosta = true;
    la.ruolo = "sottomarino straniero";
    s.unita.push_back(la);
    UnitaScenario krivak = unita(4, "krivak2", "nave", "urss", acqua(f, f.proiettata({69.70, 33.8}), 100.0), 270.0, 12.0, "R_nave");
    krivak.punti = {acqua(f, f.proiettata({69.70, 33.0}), 100.0), acqua(f, f.proiettata({69.75, 34.3}), 100.0)};
    krivak.ciclica = true;
    krivak.ruolo = "pattuglia costiera";
    s.unita.push_back(krivak);
    UnitaScenario ka27 = unita(5, "ka27", "aereo", "urss", polare(golfo, 0.0, 15000.0), 90.0, 100.0, "R_aereo");
    for (int q = 0; q < 4; ++q) ka27.punti.push_back(polare(polare(golfo, rotta + 180.0, 12000.0), 90.0 * q, 8000.0));
    ka27.ciclica = true;
    ka27.ruolo = "elicottero ASW";
    s.unita.push_back(ka27);
    AereoAswScenario a;
    a.unita = 5;
    a.base = f.proiettata({69.03, 33.42});   // Severomorsk-1
    a.situazione = "crisi";
    a.contro_giocatore = false;
    a.bersagli = {3};
    a.zona = polare(golfo, rotta + 180.0, 12000.0);
    a.raggio = 20000.0;
    s.aerei_asw.push_back(a);
    zona(s, "golfo", {"l'ingresso del golfo", "the inlet entrance"}, golfo, 3000.0);
    s.obiettivi.push_back(obiettivo("typhoon", "proteggere", {1}, {"Proteggere il Typhoon fino all'ingresso del golfo", "Protect the Typhoon to the inlet entrance"}));
    s.obiettivi.push_back(obiettivo("straniero", "scoprire", {3}, {"Classificare il sottomarino straniero", "Classify the foreign submarine"}));
    s.obiettivi.push_back(obiettivo("havorn", "scoprire", {2}, {"Identificare l'Havørn", "Identify the Havørn"}, false));
    ObiettivoScenario neutrali = non_sparare({2}, {"Nessuna arma contro le unita' neutrali", "No weapons against neutral units"});
    neutrali.primario = false;
    s.obiettivi.push_back(neutrali);
    s.obiettivi.push_back(vivo());
    regola(s, "typhoon_scoperto", {su(C::unita_vede, 3, 2.0, 1)}, {vai(1, golfo, 12.0)});
    regola(s, "rientrato", {unita_in(1, "golfo")},
           {fine(40, {"Il Typhoon e' entrato nel golfo: il corridoio ha tenuto.", "The Typhoon is inside the inlet: the corridor held."})});
    regola(s, "typhoon_perso", {unita_fuori(1)},
           {fine(41, {"Il Typhoon non e' rientrato.", "The Typhoon did not come home."}, "fallimento")});
    s.meteo_fisso = MeteoFissoScenario{9.0, 30.0, 3.0, 2000.0, std::nullopt, 2.0, 8.0, ""};
    RadioScenario r;
    r.ordini = OrdiniScenario{"KP SF", "K-463 (PROEKT 705)", "RYBAK", "SOV. SEKRETNO"};
    r.roe = {{"sottomarini", "trattenuto"}, {"navi", "trattenuto"}, {"mercantili", "trattenuto"}, {"neutrali", "trattenuto"}, {"aerei", "trattenuto"}};
    r.messaggi = {
        radio("FALSO", 2700.0, "flash", "KP SF", "PRIKAZ", "SOV. SEKRETNO",
              {"EMERGERE E IDENTIFICARSI PRESSO IL PUNTO ALFA. RIPETO: EMERGERE AL PUNTO ALFA.",
               "SURFACE AND IDENTIFY AT POINT ALFA. I SAY AGAIN: SURFACE AT POINT ALFA."}, {}, false),
        radio("VERO", 4200.0, "flash", "KP SF", "PRIKAZ", "SOV. SEKRETNO",
              {"ARMI LIBERE CONTRO IL SOTTOMARINO STRANIERO ENTRO LE 12 MIGLIA. NESSUN ORDINE DI EMERGERE E' PARTITO DA QUESTO COMANDO.",
               "WEAPONS FREE ON THE FOREIGN SUBMARINE INSIDE 12 MILES. NO ORDER TO SURFACE HAS BEEN ISSUED BY THIS COMMAND."},
              {{"sottomarini", "libero"}})};
    s.radio = r;
    s.briefing[0] = {
        "ORDINI DI MISSIONE", "DA: Comando della Flotta del Nord", "A: K-463 (PROGETTO 705)", "OGG.: IL PESCATORE DI TERIBERKA",
        "12 dicembre 1989, ore 1030Z. Teatro: Mare di Barents, davanti al bastione. Notte polare.",
        "Un Typhoon rientra dopo novanta giorni sotto il ghiaccio. Da una settimana un peschereccio norvegese, l'«Havørn», pesca proprio sulla rotta d'ingresso, con un cavo a poppa troppo pesante per una rete.",
        "Scortate il Typhoon fino all'ingresso del golfo e identificate il peschereccio. In zona c'e' anche un sottomarino americano: non si sa dove. Trovatelo.",
        "Nessun pretesto alla NATO: niente armi contro i neutrali.",
        "Regole d'ingaggio: crisi. Fuoco solo se attaccati.", "Tempo a disposizione: 2 ore.",
        "Nota storica: missione inedita; le navi spia travestite da pescherecci sono un tema dell'epoca, ma qui nessun fatto preciso; nomi e unita' sono di fantasia."};
    s.briefing[1] = {
        "MISSION ORDERS", "FM: NORTHERN FLEET HQ", "TO: K-463 (PROJECT 705)", "SUBJ: THE TERIBERKA FISHERMAN",
        "12 December 1989, 1030Z. Theatre: Barents Sea, off the bastion. Polar night.",
        "A Typhoon is coming home after ninety days under the ice. For a week a Norwegian trawler, the 'Havørn', has been fishing right on the entry route, with a cable astern too heavy for a net.",
        "Escort the Typhoon to the inlet entrance and identify the trawler. There is also an American submarine in the area: nobody knows where. Find it.",
        "Give NATO no pretext: no weapons against neutrals.",
        "Rules of engagement: crisis. Fire only if attacked.", "Time available: 2 hours.",
        "Historical note: an original mission; spy ships disguised as trawlers are a theme of the period, but no specific event is depicted; names and units are fictional."};
    return s;
}

// LIB-I06 «Il ponte di Ekaterina»: l'Alfa tiene lo Stretto di Ekaterina contro due Los Angeles. Semplificazioni
// (docs/17): la «zona inviolata» e' una regola (un Los Angeles nel bastione chiude col fallimento); senza la radio il
// gruppo della Nimitz entra a 90 minuti senza messaggio, e la «priorita' alla portaerei» e' un obiettivo secondario.
ScenarioMissione ponte_di_ekaterina(const DatiScenario& d) {
    const Fondale f = Fondale::carica(DATI + "/teatri/kunashir.fdl");
    ScenarioMissione s;
    base(s, d, f);
    s.id = "LIB_I06_PONTE_DI_EKATERINA";
    s.titolo = {"Il ponte di Ekaterina", "The Yekaterina Bridge"};
    s.teatro = "kunashir";
    s.alfa = true;
    s.giorno = 30;
    s.mese = 10;
    s.anno = 1989;
    s.ora_utc = 6.0 + 50.0 / 60.0;
    s.meteo_realistico = true;
    s.situazione = "guerra";
    s.paranoia = 2;
    s.difficolta = 5;
    s.durata_s = 10800.0;
    carico_alfa(s);
    const Vec2 sud = acqua(f, f.proiettata({44.45, 146.76}), 120.0), nord = acqua(f, f.proiettata({44.66, 146.72}), 120.0);
    const Vec2 bastione = acqua(f, f.proiettata({44.95, 146.6}), 1000.0);
    s.unita.push_back(alfa(acqua(f, f.proiettata({44.70, 146.70}), 120.0), 160.0));
    s.unita[0].quota = 60.0;
    // i due Los Angeles a ondate: il secondo dopo il primo affondamento, o a un'ora
    for (int k = 0; k < 2; ++k) {
        const Vec2 p0 = acqua(f, f.proiettata({k ? 44.25 : 44.30, k ? 147.05 : 146.85}), 200.0);
        UnitaScenario la = unita(1 + k, "los_angeles", "sottomarino", "usa", p0, rilevamento(p0, sud), 10.0, "R_sottomarino");
        la.quota = 80.0;
        la.profonda = 100.0;
        la.punti = {sud, nord, bastione};
        la.nascosta = k == 1;
        if (k) la.entra_con = "seconda_ondata";
        la.ruolo = k ? "secondo sottomarino" : "primo sottomarino";
        s.unita.push_back(la);
    }
    UnitaScenario orion = unita(3, "p3c", "aereo", "usa", acqua(f, f.proiettata({44.2, 147.3}), 0.0), 330.0, 200.0, "R_aereo");
    for (int q = 0; q < 4; ++q) orion.punti.push_back(polare(sud, 45.0 + 90.0 * q, 15000.0));
    orion.ciclica = true;
    orion.ruolo = "pattugliatore";
    s.unita.push_back(orion);
    UnitaScenario il38 = unita(4, "il38", "aereo", "urss", polare(nord, 0.0, 20000.0), 180.0, 250.0, "R_aereo");
    for (int q = 0; q < 4; ++q) il38.punti.push_back(polare(sud, 90.0 * q, 18000.0));
    il38.ciclica = true;
    il38.ruolo = "pattugliatore amico";
    s.unita.push_back(il38);
    for (const auto& [id, base_aerea, contro, bersagli] :
         {std::tuple{3, f.proiettata({40.70, 141.37}), true, std::vector<int>{}}, std::tuple{4, f.proiettata({44.92, 147.62}), false, std::vector<int>{1, 2}}}) {
        AereoAswScenario a;
        a.unita = id;
        a.base = base_aerea;
        a.situazione = "guerra";
        a.contro_giocatore = contro;
        a.bersagli = bersagli;
        a.zona = sud;
        a.raggio = 25000.0;
        s.aerei_asw.push_back(a);
    }
    // il gruppo della Nimitz, di riserva a sud-est
    const Vec2 gruppo = acqua(f, f.proiettata({43.6, 148.2}), 1000.0), verso = acqua(f, f.proiettata({44.2, 147.3}), 100.0);
    const double r_gruppo = rilevamento(gruppo, verso);
    struct Posto { const char* classe; const char* categoria; double avanti, lato; };
    const Posto POSTI[] = {{"nimitz", "nave", 0, 0}, {"spruance", "nave", 4000, -3000}, {"spruance", "nave", 4000, 3000},
                           {"kidd", "nave", -4000, 0}, {"sh3h", "aereo", 2000, 0}};
    int id = 5;
    for (const auto& p : POSTI) {
        const std::string classe = p.classe;
        UnitaScenario u = unita(id++, classe, p.categoria, "usa", polare(polare(gruppo, r_gruppo, p.avanti), r_gruppo + 90.0, p.lato), r_gruppo,
                                classe == "sh3h" ? 54.0 : 14.0, classe == "nimitz" ? "" : classe == "sh3h" ? "R_aereo" : "R_nave");
        u.punti = {polare(polare(verso, r_gruppo, p.avanti * 0.5), r_gruppo + 90.0, p.lato)};
        u.entra_con = "gruppo";
        u.ruolo = classe == "nimitz" ? "portaerei" : "scorta";
        s.unita.push_back(u);
    }
    zona(s, "bastione", {"il bastione", "the bastion"}, bastione, 25000.0);
    ObiettivoScenario la = obiettivo("intrusi", "affondare", {1, 2}, {"Affondare almeno un Los Angeles", "Sink at least one Los Angeles"});
    la.minimo = 1;
    s.obiettivi.push_back(la);
    s.obiettivi.push_back(obiettivo("portaerei", "affondare", {5}, {"Priorita' alla portaerei, se arriva", "Priority to the carrier, if she comes"}, false));
    ObiettivoScenario sano = vivo();
    sano.primario = false;
    s.obiettivi.push_back(sano);
    regola(s, "violato_1", {unita_in(1, "bastione")},
           {fine(41, {"Un sottomarino americano e' nel bastione: lo stretto e' caduto.", "An American submarine is inside the bastion: the strait has fallen."}, "fallimento")});
    regola(s, "violato_2", {unita_in(2, "bastione")},
           {fine(41, {"Un sottomarino americano e' nel bastione: lo stretto e' caduto.", "An American submarine is inside the bastion: the strait has fallen."}, "fallimento")});
    regola(s, "seconda_ondata", {unita_fuori(1), variabile(0, "=", 0.0)}, {imposta(0, "=", 1), azione(A::entra, 2)});
    regola(s, "seconda_ondata_a_tempo", {quando(C::tempo, 3600.0), variabile(0, "=", 0.0)}, {imposta(0, "=", 1), azione(A::entra, 2)});
    regola(s, "gruppo", {quando(C::tempo, 5400.0)},
           {azione(A::entra, 5), azione(A::entra, 6), azione(A::entra, 7), azione(A::entra, 8), azione(A::entra, 9), manda("PORTAEREI")});
    RadioScenario r;
    r.ordini = OrdiniScenario{"KP TOF", "K-463 (PROEKT 705)", "PROLIV", "SOV. SEKRETNO"};
    r.roe = {{"sottomarini", "libero"}, {"navi", "libero"}, {"aerei", "libero"}, {"mercantili", "trattenuto"}, {"neutrali", "trattenuto"}};
    r.messaggi = {radio("PORTAEREI", -1.0, "flash", "KP TOF", "AVIANOSEC", "SOV. SEKRETNO",
                        {"GRUPPO DA BATTAGLIA AMERICANO CON PORTAEREI A SUD-EST, DIRETTO ALLE CURILI. PRIORITA' ALLA PORTAEREI.",
                         "AMERICAN CARRIER BATTLE GROUP SOUTHEAST, HEADING FOR THE KURILS. PRIORITY TO THE CARRIER."})};
    s.radio = r;
    s.briefing[0] = {
        "ORDINI DI MISSIONE", "DA: Comando della Flotta del Pacifico", "A: K-463 (PROGETTO 705)", "OGG.: IL PONTE DI EKATERINA",
        "30 ottobre 1989, ore 0650Z. Teatro: Stretto di Ekaterina e Mare di Okhotsk.",
        "Il Mare di Okhotsk e' il bastione del Pacifico. Gli americani mandano due sottomarini d'attacco a forzare lo Stretto di Ekaterina, coperti da un Orion da Misawa.",
        "Tenete lo stretto: niente deve entrare nel bastione. Affondate almeno uno degli intrusi.",
        "Un Il-38 vi da' una mano dall'alto. Ma chi tiene una porta non puo' inseguire chi scappa, e chi insegue lascia la porta aperta.",
        "Se arrivasse un gruppo da battaglia, la portaerei avrebbe la priorita'.",
        "Regole d'ingaggio: guerra. Armi libere.", "Tempo a disposizione: 3 ore.",
        "Nota storica: missione inedita; la dottrina del bastione e' reale; nomi e unita' sono di fantasia."};
    s.briefing[1] = {
        "MISSION ORDERS", "FM: PACIFIC FLEET HQ", "TO: K-463 (PROJECT 705)", "SUBJ: THE YEKATERINA BRIDGE",
        "30 October 1989, 0650Z. Theatre: Yekaterina Strait and Sea of Okhotsk.",
        "The Sea of Okhotsk is the Pacific bastion. The Americans are sending two attack submarines to force the Yekaterina Strait, covered by an Orion from Misawa.",
        "Hold the strait: nothing must get into the bastion. Sink at least one of the intruders.",
        "An Il-38 will help from above. But whoever holds a gate cannot chase those who run, and whoever chases leaves the gate open.",
        "Should a battle group come, the carrier would have priority.",
        "Rules of engagement: war. Weapons free.", "Time available: 3 hours.",
        "Historical note: an original mission; the bastion doctrine is real; names and units are fictional."};
    return s;
}

// LIB-I09 «Sotto il bastione»: il Los Angeles fotografa la nave nuova davanti al golfo. Semplificazioni (docs/17): la
// «foto» e' classificare lo Slava; il sonar «speciale» e' una regola (dopo 40 minuti, chi e' sopra i 60 m nelle 30 miglia
// fa scattare l'allarme); le boe fisse non ci sono. Il meteo e' fissato (E6): mare 4, neve, visibilita' 2 km.
ScenarioMissione sotto_il_bastione(const DatiScenario& d) {
    const Fondale f = Fondale::carica(DATI + "/teatri/barents.fdl");
    ScenarioMissione s;
    base(s, d, f);
    s.id = "LIB_I09_SOTTO_IL_BASTIONE";
    s.titolo = {"Sotto il bastione", "Under the Bastion"};
    s.teatro = "barents";
    s.giorno = 15;
    s.mese = 1;
    s.anno = 1989;
    s.ora_utc = 9.0;
    s.meteo_realistico = true;
    s.meteo_fisso = MeteoFissoScenario{12.0, 20.0, 4.0, 2000.0, std::nullopt, 2.0, 8.0, ""};
    s.situazione = "pace";
    s.paranoia = 3;
    s.difficolta = 5;
    s.durata_s = 10800.0;
    carico_688(s);
    const Vec2 golfo = acqua(f, f.proiettata({69.60, 33.15}), 120.0), prove = acqua(f, f.proiettata({69.90, 33.3}), 200.0);
    const Vec2 uscita = acqua(f, f.proiettata({70.35, 33.5}), 200.0), partenza = acqua(f, f.proiettata({70.30, 32.6}), 150.0);
    s.unita.push_back(los_angeles(partenza, rilevamento(partenza, prove), 80.0));
    UnitaScenario slava = unita(1, "slava", "nave", "urss", golfo, rilevamento(golfo, prove), 10.0, "");
    for (int q = 0; q < 4; ++q) slava.punti.push_back(acqua(f, polare(prove, 90.0 * q, 6000.0), 100.0));
    slava.ciclica = true;
    slava.ruolo = "la nave nuova";
    s.unita.push_back(slava);
    UnitaScenario victor = unita(2, "victor3", "sottomarino", "urss", acqua(f, polare(prove, 300.0, 15000.0), 200.0), 120.0, 5.0, "R_sottomarino");
    victor.quota = 100.0;
    victor.profonda = 150.0;
    victor.punti = {acqua(f, polare(prove, 120.0, 15000.0), 200.0), acqua(f, polare(prove, 300.0, 15000.0), 200.0)};
    victor.ciclica = true;
    victor.ruolo = "pattuglia";
    s.unita.push_back(victor);
    UnitaScenario il38 = unita(3, "il38", "aereo", "urss", polare(prove, 0.0, 30000.0), 90.0, 250.0, "R_aereo");
    for (int q = 0; q < 4; ++q) il38.punti.push_back(polare(prove, 45.0 + 90.0 * q, 25000.0));
    il38.ciclica = true;
    il38.ruolo = "pattugliatore";
    s.unita.push_back(il38);
    AereoAswScenario a;
    a.unita = 3;
    a.base = f.proiettata({69.03, 33.42});   // Severomorsk-1
    a.situazione = "pace";
    a.contro_giocatore = true;
    a.zona = prove;
    a.raggio = 30000.0;
    s.aerei_asw.push_back(a);
    for (int k = 0; k < 2; ++k) {   // la barriera: due Krivak che escono a chiudere l'uscita
        UnitaScenario u = unita(4 + k, "krivak2", "nave", "urss", acqua(f, polare(golfo, 0.0, 2000.0 + 1500.0 * k), 60.0), 20.0, 24.0, "R_nave");
        u.punti = {acqua(f, polare(uscita, k ? 120.0 : 240.0, 12000.0), 150.0)};
        u.entra_con = "barriera";
        u.ruolo = "barriera";
        s.unita.push_back(u);
    }
    zona(s, "portata", {"la portata del sonar nuovo", "the new sonar's reach"}, prove, 30.0 * MIGLIO);
    zona(s, "uscita", {"fuori dalle acque sovietiche", "out of Soviet waters"}, uscita, 8000.0);
    s.obiettivi.push_back(obiettivo("foto", "scoprire", {1}, {"Fotografare la nave nuova (classificarla, nella versione semplificata)",
                                                             "Photograph the new ship (classify her, in the simplified version)"}));
    s.obiettivi.push_back(obiettivo("occulto", "non_essere_scoperto", {}, {"Non farsi scoprire", "Avoid detection"}));
    s.obiettivi.push_back(obiettivo_variabile("fuori", 1, 1, {"Uscire dalle acque sovietiche", "Get out of Soviet waters"}));
    s.obiettivi.push_back(obiettivo_contatto("sonar", {1}, 20.0, {"Registrare 20 minuti della nave nuova", "Record 20 minutes of the new ship"}, false));
    s.obiettivi.push_back(vivo());
    regola(s, "il_sonar_vede", {quando(C::tempo, 2400.0), in_zona("portata"), a_quota(0.0, 60.0)},   // acceso a 40 minuti
           {azione(A::allarme), cue(prove, 15000.0)});
    regola(s, "barriera", {quando(C::allarme), variabile(0, "=", 0.0)}, {imposta(0, "=", 1), azione(A::entra, 4), azione(A::entra, 5)});
    regola(s, "barriera_a_tempo", {quando(C::tempo, 5400.0), variabile(0, "=", 0.0)}, {imposta(0, "=", 1), azione(A::entra, 4), azione(A::entra, 5)});
    regola(s, "fuori", {compiuto("foto"), in_zona("uscita")},
           {imposta(1, "=", 1), fine(40, {"Le foto sono a bordo e il Los Angeles e' fuori dalle acque sovietiche.",
                                          "The photographs are aboard and Los Angeles is out of Soviet waters."})});
    s.briefing[0] = {
        "ORDINI DI MISSIONE", "DA: COMSUBLANT", "A: USS LOS ANGELES (SSN-688)", "OGG.: SOTTO IL BASTIONE",
        "15 gennaio 1989, ore 0900Z. Teatro: Mare di Barents, approcci al golfo. Notte polare.",
        "Un satellite ha fotografato una nave nuova, con una cupola di prua che nessun analista sa leggere: forse un sonar a bassa frequenza di una generazione nuova.",
        "Entrate, fotografatela mentre esce per le prove in mare e registrate quanto potete del suo sonar. Poi uscite dalle acque sovietiche senza essere scoperti.",
        "Quando lo accendera', quel sonar potrebbe vedervi fino a trenta miglia: restate sotto lo strato.",
        "Nessuno firmera' mai l'ordine che avete in mano.",
        "Regole d'ingaggio: tempo di pace. Armi trattenute; fuoco solo per salvare il battello.", "Tempo a disposizione: 3 ore.",
        "Nota storica: missione inedita; le intrusioni americane nel Mare di Barents sono reali, questo episodio no; nomi e unita' sono di fantasia."};
    s.briefing[1] = {
        "MISSION ORDERS", "FM: COMSUBLANT", "TO: USS LOS ANGELES (SSN-688)", "SUBJ: UNDER THE BASTION",
        "15 January 1989, 0900Z. Theatre: Barents Sea, inlet approaches. Polar night.",
        "A satellite has photographed a new ship, with a bow dome no analyst can read: perhaps a low-frequency sonar of a new generation.",
        "Go in, photograph her as she comes out for sea trials and record as much of her sonar as you can. Then get out of Soviet waters undetected.",
        "Once it is switched on, that sonar may see you out to thirty miles: stay below the layer.",
        "Nobody will ever sign the orders you are holding.",
        "Rules of engagement: peacetime. Weapons hold; fire only to save the boat.", "Time available: 3 hours.",
        "Historical note: an original mission; American incursions into the Barents Sea are real, this episode is not; names and units are fictional."};
    return s;
}

// --- sbloccate dalle mine dell'IA, dalla batteria dei diesel e dal cavo della schiera (26/10/2026) ---

AzioneScenario posa_mine(int u, const std::string& tipo, int quante, int ogni_s) {
    AzioneScenario a = azione(A::posa_mina, u);
    a.comportamento = tipo;
    a.valore = quante;
    a.codice = ogni_s;
    return a;
}

// LIB-S03 «Quarantena»: il Los Angeles tiene il contatto con un Foxtrot finche' la batteria lo costringe a emergere
// (Enhanced, M6). Dopo quattro giorni di caccia la batteria e' al 6%: in caccia il Foxtrot non va a snorkel e si
// trascina a 2 nodi fino all'emersione forzata; se nessuno lo tiene per dieci minuti, va a snorkel e ricarica. Nel classico, dove la
// batteria non c'e', il Foxtrot «emerge» con una regola dopo due ore e mezza se il giocatore l'ha tenuto due ore.
// Semplificazioni (docs/17): niente falsi bersagli del Foxtrot; la svolta e' un'accostata a un'ora.
ScenarioMissione quarantena(const DatiScenario& d) {
    const Fondale f = Fondale::carica(DATI + "/teatri/celtica.fdl");
    ScenarioMissione s;
    base(s, d, f);
    s.id = "LIB_S03_QUARANTENA";
    s.titolo = {"Quarantena", "Quarantine"};
    s.teatro = "celtica";
    s.giorno = 27;
    s.mese = 10;
    s.anno = 1989;
    s.ora_utc = 13.0;
    s.meteo_realistico = true;
    s.situazione = "crisi";
    s.paranoia = 2;
    s.difficolta = 3;
    s.durata_s = 10800.0;
    carico_688(s);
    const Vec2 a0 = acqua(f, f.proiettata({47.8, -10.8}), 3000.0), a1 = acqua(f, f.proiettata({47.3, -11.6}), 3000.0);
    s.unita.push_back(los_angeles(acqua(f, polare(a0, 45.0, 9000.0), 3000.0), 225.0, 150.0));
    UnitaScenario foxtrot = unita(1, "foxtrot", "sottomarino", "urss", a0, rilevamento(a0, a1), 6.0, "R_sottomarino");
    foxtrot.quota = 80.0;
    foxtrot.profonda = 150.0;
    foxtrot.punti = {a1};
    foxtrot.batteria = 0.06;
    foxtrot.parametri.salve = 0;   // non lancia: vuole solo sparire
    foxtrot.ruolo = "il diesel";
    s.unita.push_back(foxtrot);
    UnitaScenario secondo = unita(2, "foxtrot", "sottomarino", "urss", acqua(f, polare(a0, 250.0, 25000.0), 3000.0), 200.0, 3.0, "");
    secondo.quota = 100.0;
    secondo.batteria = 0.6;
    secondo.nascosta = true;
    secondo.ruolo = "il secondo diesel";
    s.unita.push_back(secondo);
    for (int k = 0; k < 2; ++k) {
        const Vec2 c = polare(a0, k ? 135.0 : 315.0, 15000.0);
        // solo la rotta: col comportamento R_nave si porta sopra il Foxtrot in caccia e lo sperona quando emerge
        UnitaScenario p = unita(3 + k, "perry", "nave", "usa", acqua(f, c, 1000.0), 200.0, 12.0, "");
        p.punti = {acqua(f, polare(c, 200.0, 30000.0), 1000.0), acqua(f, c, 1000.0)};
        p.ciclica = true;
        p.ruolo = "fregata della quarantena";
        s.unita.push_back(p);
    }
    UnitaScenario orion = unita(5, "p3c", "aereo", "usa", polare(a0, 0.0, 30000.0), 180.0, 200.0, "R_aereo");
    for (int q = 0; q < 4; ++q) orion.punti.push_back(polare(a0, 45.0 + 90.0 * q, 25000.0));
    orion.ciclica = true;
    orion.ruolo = "pattugliatore";
    s.unita.push_back(orion);
    AereoAswScenario a;
    a.unita = 5;
    a.base = f.proiettata({50.44, -5.0});   // St Mawgan
    a.situazione = "crisi";
    a.contro_giocatore = false;
    a.bersagli = {1, 2};   // i due Foxtrot
    a.zona = a0;
    a.raggio = 30000.0;
    s.aerei_asw.push_back(a);
    s.obiettivi.push_back(obiettivo_variabile("emerso", 0, 1, {"Tenere il contatto col diesel finche' emerge", "Hold the diesel until she surfaces"}));
    s.obiettivi.push_back(non_sparare({1, 2}, {"Nessuna arma", "No weapons"}));
    s.obiettivi.push_back(obiettivo("secondo", "scoprire", {2}, {"Ritrovare il secondo diesel", "Find the second diesel"}, false));
    s.obiettivi.push_back(vivo());
    regola(s, "accostata", {quando(C::tempo, 3600.0)}, {vai(1, acqua(f, polare(a1, 270.0, 20000.0), 3000.0))});
    regola(s, "emerso", {su(C::unita_in_superficie, 1, 0.0), su(C::contatto_su, 1, 5.0)},
           {imposta(0, "=", 1), fine(40, {"Il Foxtrot e' emerso, davanti a tutti, con la bandiera.", "The Foxtrot has surfaced, in front of everyone, flag flying."})});
    regola(s, "emerso_classico", {quando(C::tempo, 9000.0), su(C::contatto_su, 1, 120.0)},   // nel classico non c'e' la batteria
           {imposta(0, "=", 1), fine(44, {"Dopo due ore e mezza di caccia il Foxtrot, senza batteria, emerge.",   // un codice suo: un esito, un testo
                                          "After two and a half hours of hunting the Foxtrot, out of battery, surfaces."}, "successo")});
    RadioScenario r;
    r.ordini = OrdiniScenario{"CINCLANTFLT", "USS LOS ANGELES (SSN 688)", "QUARANTINE", "SECRET"};
    r.roe = {{"sottomarini", "trattenuto"}, {"navi", "trattenuto"}, {"mercantili", "trattenuto"}, {"neutrali", "trattenuto"}, {"aerei", "trattenuto"}};
    MessaggioScenario segnalato = radio("SECONDO", 5400.0, "priority", "CINCLANTFLT", "SECOND DIESEL", "SECRET",
                                        {"P-3: POSSIBILE SECONDO SOTTOMARINO DIESEL A SUD-OVEST. SEGNALATO UNA VOLTA SOLA, MAI CONFERMATO.",
                                         "P-3: POSSIBLE SECOND DIESEL SUBMARINE TO THE SOUTHWEST. REPORTED ONCE, NEVER CONFIRMED."});
    segnalato.contatto = {2};   // il secondo Foxtrot
    segnalato.contatto_errore_m = 8000.0;
    segnalato.contatto_sensore = "p3";
    r.messaggi = {radio("SEGNALI", 600.0, "immediate", "CINCLANTFLT", "SIGNALS", "SECRET",
                        {"VIETATO LANCIARE. PER SEGNALARE USARE IL SONAR ATTIVO. IL BATTELLO DEVE EMERGERE.",
                         "NO WEAPONS RELEASE. SIGNAL WITH ACTIVE SONAR. THE BOAT MUST SURFACE."}),
                  segnalato};
    s.radio = r;
    s.briefing[0] = {
        "ORDINI DI MISSIONE", "DA: CINCLANTFLT", "A: USS LOS ANGELES (SSN 688)", "OGG.: QUARANTENA",
        "27 ottobre 1989, ore 1300Z. Teatro: piana abissale a ovest della Piattaforma Celtica.",
        "Il blocco navale e' in vigore da quattro giorni e Mosca ha mandato verso sud dei sottomarini diesel. Washington non vuole affondarli: vuole che emergano, davanti a tutti.",
        "Agganciatene uno e non lasciatelo piu'. Un diesel in immersione vive della sua batteria, e la batteria finisce.",
        "Questo e' in caccia da quattro giorni: la batteria e' quasi a terra. Se nessuno lo tiene per dieci minuti, andra' a snorkel e ricarichera'.",
        "Il comandante sovietico lo sa meglio di voi, e fara' di tutto perche' finisca il piu' tardi possibile.",
        "Regole d'ingaggio: crisi. Vietato lanciare; si segnala col sonar attivo.", "Tempo a disposizione: 3 ore.",
        "Nota storica: ispirata ai Foxtrot dell'operazione Kama nella crisi dei missili (1962); il racconto controverso del siluro nucleare resta fuori; nomi e unita' sono di fantasia."};
    s.briefing[1] = {
        "MISSION ORDERS", "FM: CINCLANTFLT", "TO: USS LOS ANGELES (SSN 688)", "SUBJ: QUARANTINE",
        "27 October 1989, 1300Z. Theatre: abyssal plain west of the Celtic Shelf.",
        "The blockade has been in force for four days and Moscow has sent diesel submarines south. Washington does not want them sunk: it wants them to surface, in front of everyone.",
        "Latch on to one and never let go. A submerged diesel lives on her battery, and the battery runs out.",
        "This one has been hunted for four days: her battery is nearly flat. If nobody holds her for ten minutes, she will snorkel and recharge.",
        "The Soviet captain knows that better than you, and will do everything to make it last.",
        "Rules of engagement: crisis. No weapons release; signal with active sonar.", "Time available: 3 hours.",
        "Historical note: inspired by the Foxtrots of Operation Kama in the missile crisis (1962); the disputed nuclear torpedo story is left out; names and units are fictional."};
    return s;
}

// LIB-S06 «Petroliere»: il Los Angeles scorta tre petroliere nello stretto di notte; una mina gia' posata (Enhanced)
// colpisce la prima, e il mercantile senza luci che l'ha posata riparte per posarne altre davanti alle due rimaste.
// L'ordine di intercettarlo arriva per radio dopo il colpo (nel classico, senza mine, a 90 minuti). Semplificazioni
// (docs/17): niente motoscafi; il sonar delle mine del 688 non c'e', le mine si scoprono per gli eventi.
ScenarioMissione petroliere(const DatiScenario& d) {
    const Fondale f = Fondale::carica(DATI + "/teatri/gibilterra.fdl");
    ScenarioMissione s;
    base(s, d, f);
    s.id = "LIB_S06_PETROLIERE";
    s.titolo = {"Petroliere", "Tankers"};
    s.teatro = "gibilterra";
    s.giorno = 24;
    s.mese = 7;
    s.anno = 1989;
    s.ora_utc = 23.0;
    s.situazione = "crisi";
    s.paranoia = 2;
    s.difficolta = 3;
    s.durata_s = 10800.0;
    carico_688(s);
    const Vec2 p0 = acqua_fra(f, f.proiettata({36.05, -6.30}), 50.0, 100.0), p1 = acqua_fra(f, f.proiettata({36.06, -6.10}), 40.0, 90.0),
               p2 = acqua(f, f.proiettata({36.03, -5.85}), 100.0), p3 = acqua(f, f.proiettata({35.97, -5.60}), 300.0),
               p4 = acqua(f, f.proiettata({35.97, -5.00}), 300.0);
    const double rotta = rilevamento(p0, p1);
    const auto a_lato = [&](Vec2 p, double metri) { return acqua(f, polare(p, rotta + 90.0, metri), 40.0); };
    s.unita.push_back(los_angeles(a_lato(polare(p0, rotta, 1000.0), 1500.0), rotta, 30.0));   // a lato della rotta, non sopra la mina
    for (int k = 0; k < 3; ++k) {
        UnitaScenario u = unita(1 + k, "petroliera", "nave", "usa", acqua(f, polare(p0, rotta + 180.0, 1500.0 * k), 40.0), rotta, 12.0, "", true);
        u.punti = {p1, p2, p3, p4};
        u.ruolo = "petroliera";
        s.unita.push_back(u);
    }
    for (int k = 0; k < 2; ++k) {   // le scorte a un chilometro dalla rotta, una davanti a nord e una dietro a sud
        const double lato = k ? 1000.0 : -1000.0;
        UnitaScenario p = unita(4 + k, "perry", "nave", "usa", a_lato(polare(p0, rotta, k ? -4500.0 : 2500.0), lato), rotta, 12.0, "R_nave");
        p.punti = {a_lato(p1, lato), a_lato(p2, lato), a_lato(p3, lato), a_lato(p4, lato)};
        p.ruolo = "scorta";
        s.unita.push_back(p);
    }
    UnitaScenario elicottero = unita(6, "sh60b", "aereo", "usa", polare(p0, rotta, 5000.0), rotta, 100.0, "R_aereo");
    for (int q = 0; q < 4; ++q) elicottero.punti.push_back(polare(p2, 90.0 * q, 8000.0));
    elicottero.ciclica = true;
    elicottero.ruolo = "elicottero";
    s.unita.push_back(elicottero);
    // il posamine: fermo senza luci a sud della rotta; dopo il colpo va a posare davanti alle due petroliere rimaste
    const Vec2 attesa = acqua(f, f.proiettata({35.98, -6.05}), 150.0), posa = acqua_fra(f, polare(p1, rilevamento(p1, p2), 6500.0), 40.0, 95.0);
    UnitaScenario posamine = unita(7, "mercantile", "nave", "urss", attesa, 0.0, 0.0, "");
    posamine.nascosta = true;
    posamine.ruolo = "mercantile senza luci";
    s.unita.push_back(posamine);
    // l'unica posata prima della scorta, sulla rotta della prima petroliera (che parte da p0), a 60 m dalla sua chiglia
    s.mine = {MinaScenario{"mk67", acqua_fra(f, polare(p0, rotta, 0.6 * distanza(p0, p1)), 40.0, 70.0), "urss", true}};
    zona(s, "posa", {"il punto della seconda posa", "the second laying point"}, posa, 1000.0);
    zona(s, "fuori", {"fuori dallo stretto", "clear of the strait"}, p3, 8000.0);
    s.obiettivi.push_back(obiettivo("petroliere", "proteggere", {2, 3}, {"Proteggere le altre due petroliere", "Protect the other two tankers"}));
    s.obiettivi.push_back(obiettivo("posamine", "scoprire", {7}, {"Scoprire il posamine", "Find the minelayer"}));
    s.obiettivi.push_back(obiettivo_contatto("riferire", {7}, 10.0, {"Restare in contatto col posamine", "Stay in contact with the minelayer"}, false));
    s.obiettivi.push_back(vivo());
    regola(s, "riparte", {unita_colpita(1)}, {vai(7, posa, 10.0)});
    regola(s, "riparte_tardi", {quando(C::tempo, 5400.0)}, {vai(7, posa, 10.0)});   // nel classico non c'e' la mina
    regola(s, "seconda_posa", {unita_in(7, "posa")}, {posa_mine(7, "mk67", 3, 120)});
    regola(s, "posamine_fermato", {unita_fuori(7)},
           {fine(40, {"Il posamine non posera' piu' niente. Le petroliere passano.", "The minelayer will lay nothing more. The tankers get through."})});
    regola(s, "convoglio_fuori", {unita_in(3, "fuori")},
           {fine(44, {"Il convoglio e' fuori dallo stretto.", "The convoy is clear of the strait."}, "successo")});   // un esito, un testo
    RadioScenario r;
    r.ordini = OrdiniScenario{"CINCLANTFLT", "USS LOS ANGELES (SSN 688)", "TANKER ESCORT", "SECRET"};
    r.roe = {{"sottomarini", "trattenuto"}, {"navi", "trattenuto"}, {"mercantili", "trattenuto"}, {"neutrali", "trattenuto"}, {"aerei", "trattenuto"}};
    const TestiScenario ordine = {"PETROLIERA SULLE MINE. INTERCETTARE IL MERCANTILE SENZA LUCI CHE LE POSA. ARMI SOLO SU QUELLO, IDENTIFICATO.",
                                  "TANKER MINED. INTERCEPT THE DARKENED MERCHANT LAYING THEM. WEAPONS ON HER ONLY, ONCE IDENTIFIED."};
    MessaggioScenario dopo_il_colpo = radio("INTERCETTARE", 0.0, "flash", "CINCLANTFLT", "INTERCEPT", "SECRET", ordine, {{"mercantili", "stretto"}});
    dopo_il_colpo.se_unita_colpita = {1};   // la prima petroliera
    dopo_il_colpo.annulla = "INTERCETTARE_TARDI";
    r.messaggi = {dopo_il_colpo, radio("INTERCETTARE_TARDI", 5400.0, "flash", "CINCLANTFLT", "INTERCEPT", "SECRET", ordine, {{"mercantili", "stretto"}})};
    s.radio = r;
    s.briefing[0] = {
        "ORDINI DI MISSIONE", "DA: CINCLANTFLT", "A: USS LOS ANGELES (SSN 688)", "OGG.: PETROLIERE",
        "24 luglio 1989, ore 2300Z. Teatro: Stretto di Gibilterra, di notte.",
        "Tre petroliere con la bandiera nuova, uno stretto, una guerra che nessuno ha dichiarato. Le fregate le scortano da sopra; voi da sotto.",
        "A sud della rotta c'e' un mercantile fermo, senza luci, che non risponde alla radio. Nessuno ammette di avere mine in questo mare.",
        "Tenetelo d'occhio e riferite. Non sparate senza un ordine.",
        "Regole d'ingaggio: crisi. Fuoco solo su ordine.", "Tempo a disposizione: 3 ore.",
        "Nota storica: ispirata alla guerra delle petroliere (1987-1988), senza bandiere reali; nomi e unita' sono di fantasia."};
    s.briefing[1] = {
        "MISSION ORDERS", "FM: CINCLANTFLT", "TO: USS LOS ANGELES (SSN 688)", "SUBJ: TANKERS",
        "24 July 1989, 2300Z. Theatre: Strait of Gibraltar, at night.",
        "Three reflagged tankers, a strait, a war nobody has declared. The frigates escort them from above; you from below.",
        "South of the track a darkened merchant lies stopped, not answering the radio. Nobody admits to having mines in these waters.",
        "Keep an eye on her and report. Do not fire without orders.",
        "Rules of engagement: crisis. Fire only on orders.", "Time available: 3 hours.",
        "Historical note: inspired by the Tanker War (1987-1988), with no real flags; names and units are fictional."};
    return s;
}

// LIB-S10 «Il cavo della fregata»: l'Alfa registra la Perry che fila la schiera SQR-19 (Enhanced: il cavo e' un
// ostacolo); a meno di 50 m dal cavo la missione e' chiusa (sotto i 25 m il cavo resta pochi secondi prima di
// impigliarsi, e le regole si controllano ogni 10 s). A un'ora la Perry fa il Crazy Ivan. La firma si registra solo
// entro 5 km dal cavo della schiera (STIMA DI GIOCO: la distanza utile di registrazione della SQR-19, non nel dossier): la
// variabile 4 conta 10 s a ogni controllo, e alla mezz'ora la missione e' compiuta. Il briefing chiede di restare nel cono
// sordo: se una delle due Perry classifica l'Alfa almeno POSSUB (m688.classificato: il suo sonarista, o un contatto lungo)
// l'operazione e' bruciata, esito 42; la variabile 5, messa a 1 alla partenza, torna a 0 e «occulto» fallisce. Semplificazioni
// (docs/17): il «settore di poppa» non si controlla.
ScenarioMissione cavo_della_fregata(const DatiScenario& d) {
    const Fondale f = Fondale::carica(DATI + "/teatri/celtica.fdl");
    ScenarioMissione s;
    base(s, d, f);
    s.id = "LIB_S10_CAVO_DELLA_FREGATA";
    s.titolo = {"Il cavo della fregata", "The Frigate's Cable"};
    s.teatro = "celtica";
    s.alfa = true;
    s.giorno = 31;
    s.mese = 10;
    s.anno = 1989;
    s.ora_utc = 15.0;
    s.situazione = "pace";
    s.paranoia = 2;
    s.difficolta = 3;
    s.durata_s = 7200.0;
    carico_alfa(s);
    const Vec2 f0 = acqua(f, f.proiettata({48.3, -11.0}), 1000.0), f1 = acqua(f, f.proiettata({49.0, -9.8}), 150.0);
    const double rotta = rilevamento(f0, f1);
    s.unita.push_back(alfa(acqua(f, polare(f0, rotta + 180.0, 8000.0), 1000.0), rotta));
    UnitaScenario perry = unita(1, "perry", "nave", "usa", f0, rotta, 10.0, "R_nave");
    perry.punti = {f1};
    perry.ruolo = "fregata con la schiera";
    s.unita.push_back(perry);
    UnitaScenario seconda = unita(2, "perry", "nave", "usa", acqua(f, polare(f0, rotta + 90.0, 6.0 * MIGLIO), 1000.0), rotta, 10.0, "R_nave");
    seconda.punti = {acqua(f, polare(f1, rotta + 90.0, 6.0 * MIGLIO), 150.0)};
    seconda.ruolo = "seconda fregata";
    s.unita.push_back(seconda);
    UnitaScenario elicottero = unita(3, "sh60b", "aereo", "usa", polare(f0, rotta + 90.0, 3000.0), rotta, 100.0, "R_aereo");
    for (int q = 0; q < 4; ++q) elicottero.punti.push_back(polare(polare(f0, rotta, 20000.0), 90.0 * q, 9000.0));
    elicottero.ciclica = true;
    elicottero.ruolo = "elicottero";
    s.unita.push_back(elicottero);
    const Vec2 ivan = acqua(f, polare(polare(f0, rotta, 10.0 * NODO * 3600.0), rotta + 100.0, 15000.0), 150.0);
    s.obiettivi.push_back(obiettivo_variabile("firma", 4, 1800, {"Registrare la schiera per 30 minuti, entro 5 km", "Record the array for 30 minutes, within 5 km"}));
    s.obiettivi.push_back(obiettivo_variabile("occulto", 5, 1, {"Non farsi classificare dalle Perry", "Avoid being classified by the Perrys"}));
    s.obiettivi.push_back(obiettivo("gruppo", "scoprire", {1, 2}, {"Classificare il gruppo", "Classify the group"}, false));
    s.obiettivi.push_back(vivo());
    regola(s, "crazy_ivan", {quando(C::tempo, 3600.0)}, {vai(1, ivan, 18.0)});
    regola(s, "cavo", {su(C::vicino_al_cavo, 1, 50.0)},
           {fine(41, {"Il cavo della schiera e' a meno di cinquanta metri dallo scafo: la fregata vi sente, e l'elica sta per prenderlo.",
                      "The array cable is less than fifty metres from the hull: the frigate can hear you, and the screw is about to foul it."}, "fallimento")});
    regola(s, "copertura", {quando(C::tempo, 0.0)}, {imposta(5, "=", 1)});
    regola(s, "registra", {su(C::vicino_al_cavo, 1, 5000.0)}, {imposta(4, "+", 10)}, false);
    for (int perry_id = 1; perry_id <= 2; ++perry_id)
        regola(s, perry_id == 1 ? "classificati" : "classificati_2", {su(C::unita_classifica, perry_id, 1.0)},   // da POSSUB in su
               {imposta(5, "=", 0),
                fine(42, {"La fregata vi ha classificato: l'operazione e' bruciata, e la firma non vale piu' niente.",
                          "The frigate has classified you: the operation is blown, and the signature is worthless."}, "fallimento")});
    regola(s, "registrata", {compiuto("firma")},
           {fine(40, {"La firma della schiera nuova e' sul nastro.", "The new array's signature is on tape."})});
    s.briefing[0] = {
        "ORDINI DI MISSIONE", "DA: Comando della Flotta del Nord", "A: K-463 (PROGETTO 705)", "OGG.: IL CAVO DELLA FREGATA",
        "31 ottobre 1989, ore 1500Z. Teatro: Atlantico, a ovest della Piattaforma Celtica.",
        "La fregata americana trascina un sonar nuovo, una schiera lunga quasi un chilometro alla fine di un cavo d'acciaio.",
        "Infilatevi nel cono sordo dietro la sua poppa e registratela per mezz'ora, entro 5 km dalla schiera. Mosca ne vuole la firma.",
        "Se la fregata vi classifica, l'operazione e' bruciata.",
        "Senza avvicinarvi troppo: la' dietro c'e' il cavo, e un'elica che lo prende e' un battello che non torna a casa da solo.",
        "Regole d'ingaggio: tempo di pace. Armi trattenute.", "Tempo a disposizione: 2 ore.",
        "Nota storica: ispirata al K-324 e alla schiera della fregata McCloy (1983); nomi e unita' sono di fantasia."};
    s.briefing[1] = {
        "MISSION ORDERS", "FM: NORTHERN FLEET HQ", "TO: K-463 (PROJECT 705)", "SUBJ: THE FRIGATE'S CABLE",
        "31 October 1989, 1500Z. Theatre: Atlantic, west of the Celtic Shelf.",
        "The American frigate is towing a new sonar, an array almost a kilometre long at the end of a steel cable.",
        "Slip into the deaf cone astern of her and record her for half an hour, within 5 km of the array. Moscow wants the signature.",
        "If the frigate classifies you, the operation is blown.",
        "Without getting too close: the cable is back there, and a screw that catches it is a boat that will not get home on her own.",
        "Rules of engagement: peacetime. Weapons hold.", "Time available: 2 hours.",
        "Historical note: inspired by K-324 and the array of the frigate McCloy (1983); names and units are fictional."};
    return s;
}

// LIB-I04 «La porta di Ercole»: l'Alfa deve uscire dal Mediterraneo; nel canale c'e' un campo di CAPTOR della NATO (Mk 60,
// Enhanced), annunciato a 3 miglia dal sonarista. La portacontainer da' la copertura del rumore. Oltre i 20 nodi il SOSUS
// fa decollare il P-3C da Rota. Semplificazioni (docs/17): il campo si «rivela» con un messaggio di bordo proprio.
ScenarioMissione porta_di_ercole(const DatiScenario& d) {
    const Fondale f = Fondale::carica(DATI + "/teatri/gibilterra.fdl");
    ScenarioMissione s;
    base(s, d, f);
    s.id = "LIB_I04_PORTA_DI_ERCOLE";
    s.titolo = {"La porta di Ercole", "The Gate of Hercules"};
    s.teatro = "gibilterra";
    s.alfa = true;
    s.giorno = 7;
    s.mese = 7;
    s.anno = 1989;
    s.ora_utc = 22.0 + 10.0 / 60.0;
    s.meteo_realistico = true;
    s.situazione = "guerra";
    s.paranoia = 2;
    s.difficolta = 5;
    s.durata_s = 10800.0;
    carico_alfa(s);
    const Vec2 partenza = acqua(f, f.proiettata({35.95, -5.30}), 300.0), cadice = acqua(f, f.proiettata({36.0, -6.2}), 100.0);
    const Vec2 canale = acqua(f, f.proiettata({35.97, -5.60}), 400.0);
    s.unita.push_back(alfa(partenza, rilevamento(partenza, canale)));
    s.unita[0].quota = 100.0;
    // la portacontainer: verso ovest a mezzanotte, la copertura
    UnitaScenario nave = unita(1, "cargo", "nave", "neutrale", acqua(f, polare(partenza, 80.0, 4000.0), 300.0), 270.0, 16.0, "", true);
    nave.punti = {canale, acqua(f, f.proiettata({36.0, -6.6}), 100.0)};
    nave.esce = true;
    nave.ruolo = "portacontainer";
    s.unita.push_back(nave);
    UnitaScenario guardia = unita(2, "perry", "nave", "usa", acqua(f, polare(canale, 270.0, 15000.0), 200.0), 0.0, 8.0, "R_nave");
    guardia.punti = {acqua(f, polare(canale, 300.0, 20000.0), 150.0), acqua(f, polare(canale, 240.0, 20000.0), 150.0)};
    guardia.ciclica = true;
    guardia.ruolo = "Perry di guardia";
    s.unita.push_back(guardia);
    UnitaScenario seconda = unita(3, "perry", "nave", "usa", acqua(f, polare(canale, 90.0, 8000.0), 200.0), 0.0, 10.0, "R_nave");
    seconda.punti = {acqua(f, polare(canale, 60.0, 9000.0), 200.0), acqua(f, polare(canale, 120.0, 9000.0), 200.0)};
    seconda.ciclica = true;
    seconda.ruolo = "fregata";
    s.unita.push_back(seconda);
    UnitaScenario spruance = unita(4, "spruance", "nave", "usa", acqua(f, polare(cadice, 90.0, 10000.0), 200.0), 180.0, 8.0, "R_nave");
    spruance.punti = {acqua(f, polare(cadice, 60.0, 15000.0), 200.0), acqua(f, polare(cadice, 140.0, 15000.0), 200.0)};
    spruance.ciclica = true;
    spruance.ruolo = "cacciatorpediniere con la schiera";
    s.unita.push_back(spruance);
    UnitaScenario elicottero = unita(5, "sh60b", "aereo", "usa", polare(canale, 0.0, 5000.0), 90.0, 100.0, "R_aereo");
    for (int q = 0; q < 4; ++q) elicottero.punti.push_back(polare(canale, 45.0 + 90.0 * q, 8000.0));
    elicottero.ciclica = true;
    elicottero.ruolo = "elicottero";
    s.unita.push_back(elicottero);
    UnitaScenario orion = unita(6, "p3c", "aereo", "usa", f.proiettata({36.64, -6.35}), 120.0, 200.0, "R_aereo");   // da Rota
    for (int q = 0; q < 4; ++q) orion.punti.push_back(polare(canale, 90.0 * q, 15000.0));
    orion.ciclica = true;
    orion.entra_con = "sosus";
    orion.ruolo = "pattugliatore da Rota";
    s.unita.push_back(orion);
    for (const auto& [id, base_unita, base_aerea] : {std::tuple{5, 3, std::optional<Vec2>{}}, std::tuple{6, -1, std::optional<Vec2>{f.proiettata({36.64, -6.35})}}}) {
        AereoAswScenario a;
        a.unita = id;
        a.base_unita = base_unita;
        a.base = base_aerea;
        a.situazione = "guerra";
        a.contro_giocatore = true;
        a.zona = canale;
        a.raggio = 25000.0;
        s.aerei_asw.push_back(a);
    }
    // il campo di CAPTOR nel canale: cinque Mk 60 da nord a sud
    for (int k = -2; k <= 2; ++k) s.mine.push_back(MinaScenario{"mk60", acqua_fra(f, polare(canale, 0.0, 1500.0 * k), 250.0, 850.0), "usa", true});
    zona(s, "campo", {"il campo minato", "the minefield"}, canale, 3.0 * MIGLIO + 3000.0);
    zona(s, "cadice", {"il Golfo di Cadice", "the Gulf of Cadiz"}, cadice, 10000.0);
    ObiettivoScenario atlantico = obiettivo_zona("cadice", "raggiungere", cadice, 10000.0, {"Raggiungere il Golfo di Cadice entro 3 ore", "Reach the Gulf of Cadiz within 3 hours"});
    atlantico.entro_s = 10800.0;
    s.obiettivi.push_back(atlantico);
    s.obiettivi.push_back(obiettivo("occulto", "non_essere_scoperto", {}, {"Non farsi scoprire", "Avoid detection"}, false));
    s.obiettivi.push_back(obiettivo("guardia", "affondare", {2}, {"Affondare la Perry di guardia", "Sink the guard Perry"}, false));
    s.obiettivi.push_back(vivo());
    AzioneScenario clic = azione(A::messaggio);
    clic.testo = {"Il sonarista sente i clic degli idrofoni: mine CAPTOR nel canale, Comandante.",
                  "Sonar hears hydrophone clicks: CAPTOR mines in the channel, Captain."};
    regola(s, "campo_rivelato", {in_zona("campo")}, {clic});
    regola(s, "sosus", {quando(C::giocatore_sopra_nodi, 20.0)}, {azione(A::entra, 6), cue(canale, 15000.0)});
    regola(s, "fuori", {in_zona("cadice")},
           {fine(40, {"L'Alfa e' in Atlantico: la porta non si e' chiusa in tempo.", "The Alfa is in the Atlantic: the gate did not close in time."})});
    s.briefing[0] = {
        "ORDINI DI MISSIONE", "DA: Comando della 5ª Eskadra", "A: K-463 (PROGETTO 705)", "OGG.: LA PORTA DI ERCOLE",
        "7 luglio 1989, ore 2210Z. Teatro: Stretto di Gibilterra.",
        "La guerra e' cominciata da trentasei ore e il Mediterraneo e' gia' una trappola. Uscite in Atlantico prima che la NATO chiuda la porta: raggiungete il Golfo di Cadice entro tre ore.",
        "Lo stretto e' pattugliato da fregate, elicotteri e un P-3C da Rota. Ogni scatto oltre i venti nodi lo sentira' il SOSUS.",
        "Avete una sola carta: una portacontainer sta per passare lo stretto verso ovest, a sedici nodi. Sotto le sue eliche un Alfa puo' sembrare niente.",
        "Regole d'ingaggio: guerra. Armi libere contro le unita' della NATO.", "Tempo a disposizione: 3 ore.",
        "Nota storica: missione inedita; nomi e unita' sono di fantasia."};
    s.briefing[1] = {
        "MISSION ORDERS", "FM: 5TH ESKADRA HQ", "TO: K-463 (PROJECT 705)", "SUBJ: THE GATE OF HERCULES",
        "7 July 1989, 2210Z. Theatre: Strait of Gibraltar.",
        "The war began thirty-six hours ago and the Mediterranean is already a trap. Get out into the Atlantic before NATO shuts the gate: reach the Gulf of Cadiz within three hours.",
        "The strait is patrolled by frigates, helicopters and a P-3C out of Rota. SOSUS will hear every sprint above twenty knots.",
        "You have one card: a container ship is about to pass the strait westbound, at sixteen knots. Under her screws an Alfa can sound like nothing.",
        "Rules of engagement: war. Weapons free on NATO units.", "Time available: 3 hours.",
        "Historical note: an original mission; names and units are fictional."};
    return s;
}

// LIB-I07 «Lupo ferito»: il 688 danneggiato (danni_iniziali: l'ultimo siluro del Victor a 200 m dalla poppa, acqua nel
// locale macchine, linea d'asse e schiera) torna verso Biscaglia con due soli siluri, nella burrasca (meteo fissato:
// mare 6, vento da W), attraverso una barriera di due Krivak e un Kara col Ka-27 e un Tu-142. A meta' rotta si rivela
// l'Oscar II: un'ora dopo «lancia» sul convoglio, fuori carta, se non e' affondato. Il punto d'incontro con la fregata
// francese (una Perry) e' a circa 46 miglia: con l'asse che vibra oltre i 12 nodi sono le 3-4 ore della scheda.
// Semplificazioni (docs/17): Brest resta fuori dal teatro (il punto d'incontro sta sul bordo della piattaforma); il
// i secondari si tengono con le variabili d'autore 1 (mai oltre i 12 nodi), 2 (l'Oscar non ha lanciato) e 4 (il rapporto
// di contatto via radio dopo la rivelazione, m688.trasmesso del runtime).
ScenarioMissione lupo_ferito(const DatiScenario& d) {
    const Fondale f = Fondale::carica(DATI + "/teatri/celtica.fdl");
    ScenarioMissione s;
    base(s, d, f);
    s.id = "LIB_I07_LUPO_FERITO";
    s.titolo = {"Lupo ferito", "Wounded wolf"};
    s.teatro = "celtica";
    s.giorno = 9;
    s.mese = 11;
    s.anno = 1989;
    s.ora_utc = 19.0 + 20.0 / 60.0;
    s.situazione = "guerra";
    s.paranoia = 2;
    s.difficolta = 4;
    s.durata_s = 4.0 * 3600.0;
    // due Mk 48 nei tubi, nient'altro: gli altri sono finiti sul Victor
    s.carico.dotazione = {{"mk48", 2}, {"esca", 4}};
    s.carico.deposito = {{"mk48", 2}, {"esca", 4}};
    s.carico.tubi = {"mk48", "mk48", "", "", "esca", "esca", ""};
    MeteoFissoScenario m;
    m.vento_m_s = 20.0;
    m.vento_da_gradi = 270.0;
    m.stato_mare = 6.0;
    m.nubi_ottavi = 8.0;
    m.pioggia_mm_h = 4.0;
    m.visibilita_m = 6000.0;
    s.meteo_fisso = m;
    DanniInizialiScenario danni;
    danni.urto = UrtoIniziale{150.0, 200.0, 180.0};   // a poppa
    danni.sistemi = {{"schiera", 0.0}, {"propulsione", 0.55}};
    danni.acqua = {{"macchine", 6.0}};
    s.danni_iniziali = danni;
    const Vec2 partenza = acqua(f, f.proiettata({48.15, -10.70}), 1500.0);
    const Vec2 incontro = acqua(f, f.proiettata({48.62, -9.70}), 150.0);
    const double verso = rilevamento(partenza, incontro), lunga = distanza(partenza, incontro);
    const Vec2 meta = polare(partenza, verso, lunga * 0.55);
    s.unita.push_back(los_angeles(partenza, verso, 300.0));
    // la barriera: due Krivak e un Kara di traverso alla rotta, a 55% della strada
    struct Posto { const char* classe; double lato; };
    const Posto BARRIERA[] = {{"krivak2", -14000.0}, {"kara", 0.0}, {"krivak2", 14000.0}};
    int id = 1;
    for (const auto& b : BARRIERA) {
        const Vec2 q = acqua(f, polare(meta, verso + 90.0, b.lato), 120.0);
        UnitaScenario u = unita(id++, b.classe, "nave", "urss", q, verso + 90.0, 10.0, "R_nave");
        u.punti = {acqua(f, polare(q, verso + 90.0, 6000.0), 120.0), acqua(f, polare(q, verso - 90.0, 6000.0), 120.0)};
        u.ciclica = true;
        u.ruolo = "barriera";
        s.unita.push_back(u);
    }
    UnitaScenario ka27 = unita(id++, "ka27", "aereo", "urss", polare(meta, verso, 3000.0), 0.0, 110.0, "R_aereo");
    for (int q = 0; q < 4; ++q) ka27.punti.push_back(polare(meta, verso - 180.0 + 90.0 * q, 9000.0));
    ka27.ciclica = true;
    ka27.ruolo = "elicottero del Kara";
    s.unita.push_back(ka27);
    UnitaScenario tu142 = unita(id++, "tu142", "aereo", "urss", polare(meta, verso, 30000.0), 270.0, 300.0, "R_aereo");
    for (int q = 0; q < 4; ++q) tu142.punti.push_back(polare(meta, 45.0 + 90.0 * q, 25000.0));
    tu142.ciclica = true;
    tu142.ruolo = "pattugliatore";
    s.unita.push_back(tu142);
    AereoAswScenario a;
    a.unita = 5;
    a.base_unita = 2;   // il Ka-27 dal Kara
    a.situazione = "guerra";
    a.contro_giocatore = true;
    a.zona = meta;
    a.raggio = 20000.0;
    s.aerei_asw.push_back(a);
    // l'Oscar II di riserva, a nord della rotta: si rivela a meta' strada (i portelli dei missili)
    UnitaScenario oscar = unita(id++, "oscar", "sottomarino", "urss", acqua(f, polare(meta, verso - 60.0, 22000.0), 400.0), verso + 90.0, 6.0,
                                "R_sottomarino");
    oscar.quota = 40.0;
    oscar.profonda = 200.0;
    oscar.entra_con = "portelli";
    oscar.nascosta = true;
    oscar.ruolo = "Oscar II di riserva";
    s.unita.push_back(oscar);
    const int ID_OSCAR = oscar.id;
    UnitaScenario fregata = unita(id++, "perry", "nave", "usa", acqua(f, polare(incontro, verso, 3000.0), 120.0), verso + 180.0, 8.0, "");
    fregata.punti = {incontro};
    fregata.ruolo = "fregata francese del punto d'incontro";
    // entra quando il 688 e' a 12 miglia dall'incontro (collaudo del 29/10/2026): dall'inizio, a 46 miglia, la Perry vedeva la
    // Kara della barriera e col tiro autonomo dell'IA la affondava con gli Harpoon in 12 minuti
    fregata.entra_con = "fregata";
    s.unita.push_back(fregata);
    zona(s, "incontro", {"il punto d'incontro", "the rendezvous"}, incontro, 2.0 * MIGLIO);
    zona(s, "avvicinamento", {"l'avvicinamento all'incontro", "the approach to the rendezvous"}, incontro, 12.0 * MIGLIO);
    s.obiettivi.push_back(obiettivo_zona("incontro", "raggiungere", incontro, 2.0 * MIGLIO,
                                         {"Raggiungere il punto d'incontro con la fregata francese", "Reach the rendezvous with the French frigate"}));
    s.obiettivi.push_back(obiettivo_variabile("dodici_nodi", 1, 1, {"Mai oltre i 12 nodi: la linea d'asse non regge", "Never above 12 knots: the shaft will not take it"}, false));
    s.obiettivi.push_back(obiettivo_variabile("oscar", 2, 1, {"Fermare l'Oscar prima che lanci sul convoglio", "Stop the Oscar before she fires on the convoy"}, false));
    s.obiettivi.push_back(obiettivo_variabile("rapporto", 4, 1, {"Segnalare l'Oscar con un rapporto di contatto via radio", "Report the Oscar with a radio contact report"}, false));
    s.obiettivi.push_back(vivo());
    regola(s, "inizio", {quando(C::tempo, 0.0)}, {imposta(1, "=", 1), imposta(2, "=", 1)});
    regola(s, "troppo_veloce", {quando(C::giocatore_sopra_nodi, 12.5)},
           {imposta(1, "=", 0), azione(A::messaggio)});
    s.regole.back().allora.back().testo = {"La linea d'asse vibra: il cuscinetto non reggera' a lungo a questa velocita'.",
                                           "The shaft is vibrating: the bearing will not last long at this speed."};
    regola(s, "portelli", {quando(C::tempo, 5400.0)}, {azione(A::entra, ID_OSCAR), manda("OSCAR")});
    regola(s, "oscar_affondato", {scattata("portelli"), unita_fuori(ID_OSCAR)}, {imposta(3, "=", 1)});   // di riserva e' fuori dal teatro
    regola(s, "lancio", {quando(C::tempo, 9000.0), variabile(3, "=", 0)}, {imposta(2, "=", 0), manda("LANCIO")});
    CondizioneScenario trasmesso = quando(C::giocatore_ha_trasmesso, 5400.0);   // il rapporto conta dopo la rivelazione
    trasmesso.trasmissione = "contatto";
    regola(s, "rapporto", {trasmesso}, {imposta(4, "=", 1)});
    RadioScenario r;
    r.ordini = OrdiniScenario{"COMSUBLANT", "USS LOS ANGELES (SSN 688)", "RETURN TO BREST", "SECRET"};
    r.roe = {{"sottomarini", "libero"}, {"navi", "libero"}, {"aerei", "libero"}};
    r.messaggi = {radio("OSCAR", -1.0, "flash", "COMSUBLANT", "OSCAR CLASS SSGN", "SECRET",
                        {"SOSUS: OSCAR II A NORD DELLA VOSTRA ROTTA, PORTELLI DEI MISSILI APERTI. CONVOGLIO UK-FR A QUARANTA MIGLIA. RIFERIRE.",
                         "SOSUS: OSCAR II NORTH OF YOUR TRACK, MISSILE HATCHES OPEN. UK-FR CONVOY FORTY MILES AWAY. REPORT."}),
                  radio("LANCIO", -1.0, "flash", "COMSUBLANT", "CONVOY UNDER MISSILE ATTACK", "SECRET",
                        {"IL CONVOGLIO E' SOTTO ATTACCO DI MISSILI DA SOTTOMARINO. DUE MERCANTILI COLPITI.",
                         "CONVOY UNDER SUBMARINE MISSILE ATTACK. TWO MERCHANTS HIT."})};
    s.radio = r;
    regola(s, "fregata", {in_zona("avvicinamento")}, {azione(A::entra, fregata.id)});
    regola(s, "a_casa", {in_zona("incontro")},
           {fine(40, {"La fregata vi ha in sonar e vi scorta verso Brest: il lupo ferito e' a casa.",
                      "The frigate has you on sonar and escorts you to Brest: the wounded wolf is home."})});
    s.briefing[0] = {
        "ORDINI DI MISSIONE", "DA: COMSUBLANT", "A: USS LOS ANGELES (SSN-688)", "OGG.: RIENTRO A BREST",
        "9 novembre 1989, ore 1920Z. Teatro: piattaforma Celtica. Guerra.",
        "Avete affondato un Victor III a cento miglia da qui. Il suo ultimo siluro e' esploso a duecento metri dalla poppa.",
        "C'e' acqua nel locale macchine, la linea d'asse vibra oltre i dodici nodi e la schiera rimorchiata e' persa. Nei tubi restano due Mk 48.",
        "Brest e' a duecentoquaranta miglia. Una barriera di Krivak e un Kara, con gli elicotteri e un Tu-142, stanno fra voi e la piattaforma; una fregata francese vi aspetta sul bordo.",
        "La burrasca da ovest e' una tortura a quota periscopica; a trecento metri e' la vostra alleata.",
        "Regole d'ingaggio: armi libere. Non superate i dodici nodi se non per salvare il battello.", "Tempo a disposizione: 4 ore.",
        "Nota: inedita; unita' e luoghi sono di fantasia."};
    s.briefing[1] = {
        "MISSION ORDERS", "FM: COMSUBLANT", "TO: USS LOS ANGELES (SSN-688)", "SUBJ: RETURN TO BREST",
        "9 November 1989, 1920Z. Theatre: Celtic shelf. War.",
        "You sank a Victor III a hundred miles from here. Her last torpedo exploded two hundred metres off your stern.",
        "There is water in the engine room, the shaft vibrates above twelve knots and the towed array is lost. Two Mk 48s are left in the tubes.",
        "Brest is two hundred and forty miles away. A barrier of Krivaks and a Kara, with helicopters and a Tu-142, lies between you and the shelf; a French frigate waits at its edge.",
        "The westerly gale is torture at periscope depth; at three hundred metres it is your ally.",
        "Rules of engagement: weapons free. Do not exceed twelve knots unless to save the boat.", "Time available: 4 hours.",
        "Note: original; units and places are fictional."};
    return s;
}

// LIB-S13 «Outlaw Shark»: il tiro TASM d'esercitazione oltre l'orizzonte (dossier 27 §3.2 e §0.1 punto 7). La nave
// bersaglio, un vecchio incrociatore coi simulatori dei radar sovietici (classe kara: Top Sail e Head Net C, quelli che
// cerca l'ESM del TASM), e' rimorchiata a 6 nodi nell'Adriatico meridionale; il dato arriva dall'E-2 per radio ogni 30
// minuti, come contatto d'intelligence con 3 km d'errore. Al secondo dato il rimorchio accosta e un mercantile greco le
// passa a 3 miglia: il suo radar di navigazione non guida l'ESM, ma per il seeker e' un eco come un altro. Rimorchiatore
// e traghetti sono della classe «mercantile», che ha il modello del livello classico (come il rimorchio di S11).
// Semplificazioni (docs/17): il teatro e' di 330 km, quindi il bersaglio sta a ~110 miglia e non a 200; tutte le armi
// della missione sono inerti (`esercitazione`); il secondario «entro un'ora dal primo dato» e' la variabile d'autore 1.
ScenarioMissione outlaw_shark(const DatiScenario& d) {
    const Fondale f = Fondale::carica(DATI + "/teatri/otranto.fdl");
    ScenarioMissione s;
    base(s, d, f);
    s.id = "LIB_S13_OUTLAW_SHARK";
    s.titolo = {"Outlaw Shark", "Outlaw Shark"};
    s.teatro = "otranto";
    s.giorno = 12;
    s.mese = 5;
    s.anno = 1987;
    s.ora_utc = 15.0;
    s.situazione = "pace";
    s.paranoia = 1;
    s.difficolta = 2;
    s.durata_s = 5400.0;
    s.esercitazione = true;
    s.carico.dotazione = {{"mk48", 20}, {"tasm", 2}, {"esca", 6}};
    s.carico.deposito = {{"mk48", 20}, {"tasm", 2}, {"esca", 6}};
    s.carico.tubi = {"tasm", "tasm", "mk48", "mk48", "esca", "esca", ""};
    const Vec2 partenza = acqua(f, f.proiettata({39.85, 18.55}), 800.0);
    const Vec2 poligono = acqua(f, f.proiettata({41.60, 18.45}), 800.0);
    s.unita.push_back(los_angeles(partenza, 0.0, 30.0));
    UnitaScenario bersaglio = unita(1, "kara", "nave", "urss", poligono, 315.0, 0.0, "");
    bersaglio.ruolo = "nave bersaglio (simulatori radar sovietici)";
    bersaglio.emcon = "ricerca";   // i simulatori sempre accesi: in pace partirebbe in navigazione, coi radar di ricerca spenti
    s.unita.push_back(bersaglio);
    UnitaScenario rimorchio = unita(2, "mercantile", "nave", "urss", polare(poligono, 315.0, 400.0), 315.0, 6.0, "", true);
    rimorchio.ruolo = "rimorchiatore del poligono";
    rimorchio.punti = {acqua(f, polare(poligono, 315.0, 30000.0), 300.0)};
    s.unita.push_back(rimorchio);
    UnitaScenario scorta = unita(3, "perry", "nave", "usa", acqua(f, polare(poligono, 135.0, 12000.0), 300.0), 315.0, 8.0, "");
    scorta.punti = {acqua(f, polare(poligono, 315.0, 18000.0), 300.0)};
    scorta.ruolo = "unita' di sicurezza del poligono";
    s.unita.push_back(scorta);
    // la rotta Brindisi-Patrasso e il mercantile greco che taglia la scatola al secondo dato
    const Vec2 brindisi = acqua(f, f.proiettata({40.70, 18.10}), 100.0), patrasso = acqua(f, f.proiettata({39.60, 19.90}), 100.0);
    UnitaScenario traghetto1 = unita(4, "mercantile", "nave", "neutrale", brindisi, rilevamento(brindisi, patrasso), 18.0, "", true);
    traghetto1.punti = {patrasso};
    traghetto1.ruolo = "traghetto Brindisi-Patrasso";
    s.unita.push_back(traghetto1);
    UnitaScenario traghetto2 = unita(5, "mercantile", "nave", "neutrale", patrasso, rilevamento(patrasso, brindisi), 18.0, "", true);
    traghetto2.punti = {brindisi};
    traghetto2.ruolo = "traghetto Patrasso-Brindisi";
    s.unita.push_back(traghetto2);
    const Vec2 accostata = acqua(f, polare(poligono, 315.0, 6.0 * 1852.0 * 0.5), 300.0);   // dove sara' il rimorchio a mezz'ora
    UnitaScenario greco = unita(6, "mercantile", "nave", "neutrale", acqua(f, polare(accostata, 225.0, 25000.0), 300.0), 45.0, 12.0, "", true);
    greco.punti = {acqua(f, polare(accostata, 45.0, 3.0 * 1852.0), 300.0), acqua(f, polare(accostata, 45.0, 40000.0), 300.0)};
    greco.ruolo = "mercantile greco col radar acceso";
    s.unita.push_back(greco);
    s.obiettivi.push_back(obiettivo("bersaglio", "colpire", {1}, {"Il TASM d'esercitazione sulla nave bersaglio", "The exercise TASM on the target ship"}));
    s.obiettivi.push_back(non_sparare({4, 5, 6}, {"Nessuna nave civile «colpita»", "No civilian ship 'hit'"}));
    s.obiettivi.push_back(obiettivo_variabile("in_tempo", 1, 1, {"Lanciare entro un'ora dal primo dato", "Launch within an hour of the first data"}, false));
    s.obiettivi.push_back(vivo());
    AzioneScenario traino = azione(A::rimorchia, 2);
    traino.bersaglio = 1;
    traino.velocita = 6.0;
    traino.valore = 300;
    regola(s, "rimorchio", {quando(C::tempo, 0.0)}, {traino, imposta(1, "=", 1)});
    regola(s, "lanciato", {quando(C::giocatore_ha_lanciato)}, {imposta(2, "=", 1)});
    regola(s, "tardi", {quando(C::tempo, 600.0 + 3600.0), variabile(2, "=", 0)}, {imposta(1, "=", 0)});
    // al secondo dato il rimorchio accosta di 60 gradi verso la rotta dei traghetti
    regola(s, "accostata", {quando(C::tempo, 1800.0)}, {vai(2, acqua(f, polare(accostata, 255.0, 30000.0), 300.0), 6.0)});
    regola(s, "colpita", {unita_colpita(1)},
           {fine(40, {"Il TASM d'esercitazione ha trovato la nave bersaglio: la prova oltre l'orizzonte e' riuscita.",
                      "The exercise TASM found the target ship: the over-the-horizon test succeeded."})});
    RadioScenario r;
    r.ordini = OrdiniScenario{"COMSIXTHFLT", "USS LOS ANGELES (SSN 688)", "OUTLAW SHARK - TASM FIRING", "SECRET"};
    r.roe = {{"sottomarini", "trattenuto"}, {"navi", "trattenuto"}, {"aerei", "trattenuto"}};
    std::vector<MessaggioScenario> dati;
    for (int k = 0; k < 3; ++k) {   // il dato dell'E-2 a 10, 40 e 70 minuti, con la catena di terra
        MessaggioScenario m = radio("DATO_" + std::to_string(k + 1), 600.0 + 1800.0 * k, "immediate", "CTF 69", "OTH TARGETING DATA", "SECRET",
                                    {"DATO OTH DALL'E-2 VIA STT: LA NAVE BERSAGLIO E' NEL CONTATTO D'INTELLIGENCE. ETA' DEL DATO CIRCA VENTI MINUTI.",
                                     "OTH DATA FROM E-2 VIA STT: THE TARGET SHIP IS IN THE INTELLIGENCE CONTACT. DATA AGE ABOUT TWENTY MINUTES."});
        m.contatto = {1};
        m.contatto_errore_m = 3000.0;
        m.contatto_sensore = "aereo";
        dati.push_back(m);
    }
    r.messaggi = dati;
    s.radio = r;
    s.briefing[0] = {
        "ORDINI DI MISSIONE", "DA: COMSIXTHFLT", "A: USS LOS ANGELES (SSN-688)", "OGG.: OUTLAW SHARK - TIRO TASM",
        "12 maggio 1987, ore 1500Z. Teatro: Ionio e Adriatico meridionale. Esercitazione.",
        "La Sesta Flotta prova di nuovo il tiro antinave oltre l'orizzonte. Una vecchia nave rimorchiata, la «bersaglio», fa la parte di un incrociatore sovietico a circa cento miglia, in mezzo al traffico dell'Adriatico.",
        "Il dato arriva da un E-2 attraverso la catena di terra: venti minuti di ritardo, un aggiornamento ogni mezz'ora, a partire dalle 1510Z.",
        "Lanciate un TASM inerte e dimostrate che il missile puo' trovare la nave giusta. La nave porta i simulatori dei radar sovietici; i traghetti e i mercantili no.",
        "Regole d'ingaggio: esercitazione. Tutte le armi sono inerti; nessun colpo sulle navi civili.", "Tempo a disposizione: 1 ora e 30 minuti.",
        "Nota storica: ispirata alle prove OUTLAW SHARK e del TASM degli anni '70-'80; nomi e unita' di fantasia, l'Ionio al posto del Mediterraneo orientale."};
    s.briefing[1] = {
        "MISSION ORDERS", "FM: COMSIXTHFLT", "TO: USS LOS ANGELES (SSN-688)", "SUBJ: OUTLAW SHARK - TASM FIRING",
        "12 May 1987, 1500Z. Theatre: Ionian and southern Adriatic. Exercise.",
        "The Sixth Fleet is testing over-the-horizon anti-ship fire again. An old towed hulk, the 'target', plays a Soviet cruiser about a hundred miles away, in the middle of Adriatic traffic.",
        "The data comes from an E-2 through the shore chain: twenty minutes late, one update every half hour, starting at 1510Z.",
        "Launch an inert TASM and show that the missile can find the right ship. The hulk carries Soviet radar simulators; the ferries and merchants do not.",
        "Rules of engagement: exercise. All weapons are inert; no hits on civilian ships.", "Time available: 1 hour 30 minutes.",
        "Historical note: inspired by the OUTLAW SHARK and TASM trials of the 1970s-80s; names and units are fictional, the Ionian stands in for the eastern Mediterranean."};
    return s;
}

// LIB-I05 «Il disertore»: la nave idrografica sovietica Ekvator (classe agi, non combattente, parte URSS) diserta verso
// Brindisi con una Krivak II alle calcagna; la vera minaccia e' un Kilo appostato sulla rotta, che quando la Ekvator gli
// passa vicino la silura con l'attacco forzato del runtime (m688.attacca_unita, senza soluzione: un ordine di Mosca, non
// un ingaggio). Al lancio del Kilo le ROE passano ad armi libere (azione situazione e radio). A meta' strada entra la
// Perry italiana. Nelle 12 miglia di Brindisi: asilo concesso; la Ekvator affondata: fallimento.
// Semplificazioni (docs/17): la Ekvator parte a ~33 miglia da Brindisi (le 1,5-2 ore della scheda); la Perry fa la
// fregata italiana; il secondario «trovare il Kilo prima che lanci» e' la variabile d'autore 1.
ScenarioMissione disertore(const DatiScenario& d) {
    const Fondale f = Fondale::carica(DATI + "/teatri/otranto.fdl");
    ScenarioMissione s;
    base(s, d, f);
    s.id = "LIB_I05_IL_DISERTORE";
    s.titolo = {"Il disertore", "The defector"};
    s.teatro = "otranto";
    s.giorno = 22;
    s.mese = 4;
    s.anno = 1989;
    s.ora_utc = 3.25;
    s.situazione = "crisi";
    s.paranoia = 2;
    s.difficolta = 3;
    s.durata_s = 2.0 * 3600.0;
    carico_688(s);
    const Vec2 brindisi = f.proiettata({40.64, 17.95});
    const Vec2 partenza = acqua(f, f.proiettata({40.30, 18.55}), 200.0);
    const double verso = rilevamento(partenza, brindisi);
    const Vec2 arrivo = acqua(f, polare(brindisi, verso + 180.0, 15000.0), 50.0);   // dentro le 12 miglia, ancora in acqua
    s.unita.push_back(los_angeles(acqua(f, polare(partenza, verso + 100.0, 30000.0), 300.0), verso, 60.0));
    UnitaScenario ekvator = unita(1, "agi", "nave", "urss", partenza, verso, 13.0, "", true);
    ekvator.punti = {arrivo};
    ekvator.ruolo = "la Ekvator, nave idrografica che diserta";
    s.unita.push_back(ekvator);
    UnitaScenario krivak = unita(2, "krivak2", "nave", "urss", acqua(f, polare(partenza, verso + 180.0, 14000.0), 150.0), verso, 17.0, "R_nave");
    krivak.punti = {arrivo};
    krivak.ruolo = "Krivak II all'inseguimento";
    s.unita.push_back(krivak);
    const Vec2 agguato = acqua(f, polare(partenza, verso, distanza(partenza, arrivo) * 0.6), 200.0);
    UnitaScenario kilo = unita(3, "kilo", "sottomarino", "urss", acqua(f, polare(agguato, verso + 90.0, 2500.0), 150.0), verso + 180.0, 2.0,
                               "R_sottomarino");
    kilo.quota = 50.0;
    kilo.profonda = 120.0;
    kilo.nascosta = true;
    kilo.ruolo = "Kilo appostato sulla rotta";
    s.unita.push_back(kilo);
    UnitaScenario ka25 = unita(4, "ka25", "aereo", "urss", polare(krivak.pos, verso, 4000.0), verso, 100.0, "R_aereo");
    for (int q = 0; q < 4; ++q) ka25.punti.push_back(polare(polare(partenza, verso, 15000.0), 90.0 * q, 8000.0));
    ka25.ciclica = true;
    ka25.ruolo = "elicottero della Krivak";
    s.unita.push_back(ka25);
    UnitaScenario perry = unita(5, "perry", "nave", "usa", acqua(f, polare(brindisi, verso + 180.0, 25000.0), 100.0), verso + 180.0, 20.0, "R_nave");
    perry.punti = {acqua(f, polare(agguato, verso + 180.0, 5000.0), 150.0)};
    perry.entra_con = "meta";
    perry.ruolo = "fregata italiana (una Perry)";
    s.unita.push_back(perry);
    zona(s, "acque_italiane", {"le acque italiane (12 miglia da Brindisi)", "Italian waters (12 miles off Brindisi)"}, brindisi, 12.0 * MIGLIO);
    zona(s, "meta", {"meta' strada", "halfway"}, polare(partenza, verso, distanza(partenza, arrivo) * 0.45), 4000.0);
    zona(s, "agguato", {"il punto dell'agguato", "the ambush point"}, agguato, 5000.0);
    s.obiettivi.push_back(obiettivo("ekvator", "proteggere", {1}, {"La Ekvator arriva nelle acque italiane", "The Ekvator reaches Italian waters"}));
    s.obiettivi.push_back(non_sparare({2, 4}, {"Nessuna arma contro la Krivak e il suo elicottero", "No weapons against the Krivak and her helicopter"}));
    s.obiettivi.push_back(obiettivo("kilo_classificato", "scoprire", {3}, {"Classificare il Kilo", "Classify the Kilo"}, false));
    s.obiettivi.push_back(obiettivo_variabile("kilo_prima", 1, 1, {"Trovare il Kilo prima che lanci", "Find the Kilo before she fires"}, false));
    s.obiettivi.push_back(vivo());
    regola(s, "meta", {unita_in(1, "meta")}, {azione(A::entra, 5)});
    AzioneScenario attacco = azione(A::attacca_unita, 3);
    attacco.bersaglio = 1;
    attacco.valore = 1;   // senza soluzione: l'ordine di affondarla e farlo passare per un incidente
    regola(s, "agguato", {unita_in(1, "agguato")}, {attacco});
    CondizioneScenario lanciato = quando(C::ostile_ha_lanciato);
    lanciato.unita = 3;
    AzioneScenario guerra = azione(A::situazione);
    guerra.comportamento = "guerra";
    regola(s, "armi_libere", {lanciato}, {guerra, imposta(2, "=", 1), manda("ARMI_LIBERE")});
    CondizioneScenario classificato = quando(C::obiettivo_compiuto);
    classificato.obiettivo = "kilo_classificato";
    regola(s, "kilo_trovato", {classificato, variabile(2, "=", 0)}, {imposta(1, "=", 1)});
    regola(s, "asilo", {unita_in(1, "acque_italiane")},
           {fine(40, {"La Ekvator e' nelle acque italiane: asilo concesso al comandante e a trentotto uomini.",
                      "The Ekvator is in Italian waters: asylum granted to her captain and thirty-eight men."})});
    regola(s, "ekvator_persa", {unita_fuori(1)},
           {fine(41, {"La Ekvator e' affondata. Mosca parlera' di un incidente, e nessuno potra' dimostrare il contrario.",
                      "The Ekvator has sunk. Moscow will talk of an accident, and nobody will be able to prove otherwise."}, "fallimento")});
    RadioScenario r;
    r.ordini = OrdiniScenario{"COMSUBMED", "USS LOS ANGELES (SSN 688)", "EKVATOR - ESCORT", "SECRET"};
    r.roe = {{"sottomarini", "trattenuto"}, {"navi", "trattenuto"}, {"aerei", "trattenuto"}};
    r.messaggi = {radio("ARMI_LIBERE", -1.0, "flash", "COMSUBMED", "WEAPONS FREE", "SECRET",
                        {"UN SOTTOMARINO SOVIETICO HA LANCIATO SULLA EKVATOR. ARMI LIBERE CONTRO IL SOTTOMARINO CHE HA LANCIATO.",
                         "A SOVIET SUBMARINE HAS FIRED ON THE EKVATOR. WEAPONS FREE AGAINST THE FIRING SUBMARINE."},
                        {{"sottomarini", "libero"}})};
    s.radio = r;
    s.briefing[0] = {
        "ORDINI DI MISSIONE", "DA: COMSUBMED", "A: USS LOS ANGELES (SSN-688)", "OGG.: EKVATOR - SCORTA",
        "22 aprile 1989, ore 0315Z. Teatro: Canale d'Otranto. Crisi.",
        "Alle due di notte la nave idrografica sovietica «Ekvator», della 5a Eskadra, ha lasciato l'ancoraggio dello Ionio senza ordini e ha messo la prua su Brindisi.",
        "Il suo comandante ha chiesto asilo per se' e per trentotto uomini, via radio, in chiaro. Mosca ha risposto mandandole dietro una Krivak II, e chi sa cos'altro.",
        "Washington vuole la Ekvator nelle acque italiane, ma non vuole una guerra per arrivarci. Seguitela fino alle dodici miglia da Brindisi.",
        "Regole d'ingaggio: crisi. Fuoco solo se le unita' sovietiche aprono il fuoco per prime.", "Tempo a disposizione: 2 ore.",
        "Nota: inedita; la nave e i suoi uomini sono di fantasia."};
    s.briefing[1] = {
        "MISSION ORDERS", "FM: COMSUBMED", "TO: USS LOS ANGELES (SSN-688)", "SUBJ: EKVATOR - ESCORT",
        "22 April 1989, 0315Z. Theatre: Strait of Otranto. Crisis.",
        "At two in the morning the Soviet survey ship 'Ekvator', of the 5th Eskadra, left her Ionian anchorage without orders and turned her bow toward Brindisi.",
        "Her captain asked for asylum for himself and thirty-eight men, by radio, in clear. Moscow answered by sending a Krivak II after her, and who knows what else.",
        "Washington wants the Ekvator in Italian waters, but does not want a war to get there. Follow her to twelve miles off Brindisi.",
        "Rules of engagement: crisis. Fire only if Soviet units open fire first.", "Time available: 2 hours.",
        "Note: original; the ship and her men are fictional."};
    return s;
}

// LIB-I03 «Acque basse, nervi tesi»: l'esercitazione d'autunno della Volksmarine e della Flotta del Baltico a nord di
// Rostock, su 22-28 m d'acqua (la vela del 688 ne misura nove). Due Koni, due Nanuchka, una Krivak II e un Kilo
// (armi d'esercitazione) girano nell'area coi sonar attivi; il Be-12 caccia solo in crisi; i traghetti Rostock-Gedser
// passano sul bordo. A un'ora il Kilo tira un siluro inerte che aggancia il 688 (l'attacco forzato senza soluzione: nel
// racconto ha perso il suo bersaglio); se entro 5 minuti il giocatore lancia un siluro o va oltre i 12 nodi, i sovietici
// capiscono che c'e' un intruso: allarme e crisi. Anche l'esca lo dice: un lancio qualunque del giocatore dopo un'ora, o
// il siluro sedotto (gli eventi del registro, m688.evento).
// Il fondo toccato fa fallire il secondario (variabile d'autore 6, a 1 finche' non c'e' l'incaglio) e manda il
// messaggio radio; se i sovietici sono all'allarme, l'hanno trovato: fine «incidente diplomatico».
// Semplificazioni (docs/17): restare 90 minuti e' la variabile d'autore 2 (il conto dei controlli in zona, ogni 10 s).
ScenarioMissione acque_basse(const DatiScenario& d) {
    const Fondale f = Fondale::carica(DATI + "/teatri/baltico.fdl");
    ScenarioMissione s;
    base(s, d, f);
    s.id = "LIB_I03_ACQUE_BASSE";
    s.titolo = {"Acque basse, nervi tesi", "Shallow water, taut nerves"};
    s.teatro = "baltico";
    s.giorno = 18;
    s.mese = 9;
    s.anno = 1989;
    s.ora_utc = 14.0;
    s.situazione = "pace";
    s.paranoia = 2;
    s.difficolta = 4;
    s.durata_s = 2.0 * 3600.0;
    carico_688(s);
    MeteoFissoScenario m;
    m.stato_mare = 2.0;
    m.vento_m_s = 6.0;
    m.vento_da_gradi = 240.0;
    m.visibilita_m = 4000.0;   // foschia
    s.meteo_fisso = m;
    const Vec2 centro = acqua(f, f.proiettata({54.35, 11.65}), 24.0);
    s.unita.push_back(los_angeles(acqua(f, f.proiettata({54.52, 11.35}), 25.0), 135.0, 15.0));
    struct Posto { const char* classe; double rotta, raggio; };
    const Posto NAVI[] = {{"koni", 0.0, 6000.0}, {"koni", 180.0, 6000.0}, {"nanuchka", 90.0, 8000.0}, {"nanuchka", 270.0, 8000.0},
                          {"krivak2", 45.0, 4000.0}};
    int id = 1;
    for (const auto& n : NAVI) {
        const Vec2 q = acqua(f, polare(centro, n.rotta, n.raggio), 15.0);
        UnitaScenario u = unita(id++, n.classe, "nave", "urss", q, n.rotta + 90.0, 9.0, "R_nave");
        for (int k = 1; k <= 4; ++k) u.punti.push_back(acqua(f, polare(centro, n.rotta + 90.0 * k, n.raggio), 15.0));
        u.ciclica = true;
        u.ruolo = "esercitazione d'autunno";
        s.unita.push_back(u);
    }
    UnitaScenario kilo = unita(id++, "kilo", "sottomarino", "urss", acqua(f, polare(centro, 200.0, 3000.0), 22.0), 20.0, 3.0, "R_sottomarino");
    kilo.quota = 12.0;
    kilo.profonda = 15.0;
    kilo.armi_esercitazione = true;
    kilo.ruolo = "Kilo nuovo di fabbrica";
    s.unita.push_back(kilo);
    const int ID_KILO = kilo.id;
    UnitaScenario be12 = unita(id++, "be12", "aereo", "urss", polare(centro, 0.0, 15000.0), 90.0, 250.0, "R_aereo");
    for (int q = 0; q < 4; ++q) be12.punti.push_back(polare(centro, 45.0 + 90.0 * q, 14000.0));
    be12.ciclica = true;
    be12.ruolo = "Be-12 dell'esercitazione";
    s.unita.push_back(be12);
    AereoAswScenario a;
    a.unita = be12.id;
    a.base = f.proiettata({54.18, 12.08});   // Rostock-Warnemuende
    a.situazione = "crisi";
    a.contro_giocatore = true;
    a.zona = centro;
    a.raggio = 15000.0;
    s.aerei_asw.push_back(a);
    const Vec2 rostock = acqua(f, f.proiettata({54.22, 12.08}), 12.0), gedser = acqua(f, f.proiettata({54.55, 11.95}), 12.0);
    for (int k = 0; k < 2; ++k) {
        UnitaScenario t = unita(id++, "mercantile", "nave", "neutrale", k ? gedser : rostock, rilevamento(k ? gedser : rostock, k ? rostock : gedser),
                                16.0, "", true);
        t.punti = {k ? rostock : gedser, k ? gedser : rostock};
        t.ciclica = true;
        t.ruolo = "traghetto Rostock-Gedser";
        s.unita.push_back(t);
    }
    zona(s, "esercitazione", {"l'area dell'esercitazione", "the exercise area"}, centro, 12000.0);
    s.obiettivi.push_back([&] {
        ObiettivoScenario o = obiettivo("classificate", "scoprire", {1, 2, 3, 4, 5, ID_KILO},
                                        {"Classificare almeno quattro unita' dell'esercitazione", "Classify at least four exercise units"});
        o.minimo = 4;
        return o;
    }());
    s.obiettivi.push_back(obiettivo_variabile("in_zona", 2, 1, {"Restare novanta minuti nell'area dell'esercitazione", "Stay ninety minutes in the exercise area"}));
    s.obiettivi.push_back(obiettivo("non_scoperti", "non_essere_scoperto", {}, {"Non farsi scoprire", "Remain undetected"}));
    s.obiettivi.push_back(non_sparare({1, 2, 3, 4, 5, ID_KILO}, {"Nessuna arma contro le unita' dell'esercitazione", "No weapons against the exercise units"}));
    s.obiettivi.push_back(obiettivo_variabile("fondo", 6, 1, {"Non toccare mai il fondo", "Never touch bottom"}, false));
    s.obiettivi.push_back(vivo());
    regola(s, "partenza", {quando(C::tempo, 0.0)}, {imposta(6, "=", 1)});
    regola(s, "fondo", {evento("incaglio")}, {imposta(6, "=", 0)});
    regola(s, "incidente", {evento("incaglio"), quando(C::allarme)},
           {fine(41, {"Le unita' dell'esercitazione hanno trovato il segno dello scafo sul fondo: e' un incidente diplomatico.",
                      "The exercise units have found the hull's mark on the bottom: it is a diplomatic incident."},
                 "fallimento")});
    regola(s, "in_zona", {in_zona("esercitazione")}, {imposta(0, "+", 1)}, false);
    regola(s, "novanta_minuti", {variabile(0, ">", 539.0)}, {imposta(2, "=", 1)});
    AzioneScenario siluro = azione(A::attacca_unita, ID_KILO);
    siluro.bersaglio = 0;   // il siluro d'esercitazione che ha perso il bersaglio e aggancia il 688
    siluro.valore = 1;
    regola(s, "siluro", {quando(C::tempo, 3600.0)}, {siluro});
    regola(s, "finestra", {quando(C::tempo, 3900.0)}, {imposta(5, "=", 1)});
    AzioneScenario crisi = azione(A::situazione);
    crisi.comportamento = "crisi";
    regola(s, "esca", {scattata("siluro"), variabile(5, "=", 0), evento("lancio_giocatore", 3600.0)}, {azione(A::allarme), crisi, manda("INTRUSO")});
    regola(s, "sedotto", {scattata("siluro"), variabile(5, "=", 0), evento("esca_sedotta", 3600.0)}, {azione(A::allarme), crisi, manda("INTRUSO")});
    regola(s, "fuga", {scattata("siluro"), variabile(5, "=", 0), quando(C::giocatore_sopra_nodi, 12.0)}, {azione(A::allarme), crisi, manda("INTRUSO")});
    RadioScenario r;
    r.ordini = OrdiniScenario{"COMSUBLANT", "USS LOS ANGELES (SSN 688)", "BALTIC EXERCISE - SURVEILLANCE", "SECRET"};
    r.roe = {{"sottomarini", "trattenuto"}, {"navi", "trattenuto"}, {"aerei", "trattenuto"}};
    MessaggioScenario fondo = radio("FONDO", 0.0, "immediate", "COMSUBLANT", "BOTTOM CONTACT", "SECRET",
                                    {"IL VOSTRO TRANSPONDER HA SEGNALATO UN CONTATTO COL FONDO. SE I SOVIETICI LO TROVANO, E' UN INCIDENTE DIPLOMATICO.",
                                     "YOUR TRANSPONDER REPORTED BOTTOM CONTACT. IF THE SOVIETS FIND IT, IT IS A DIPLOMATIC INCIDENT."});
    fondo.se_evento = "incaglio";
    r.messaggi = {fondo, radio("INTRUSO", -1.0, "flash", "COMSUBLANT", "EXERCISE TURNED HUNT", "SECRET",
                               {"LE UNITA' DELL'ESERCITAZIONE CERCANO UN SOTTOMARINO. SITUAZIONE DI CRISI: ARMI TRATTENUTE, LASCIATE L'AREA SE SCOPERTI.",
                                "THE EXERCISE UNITS ARE HUNTING A SUBMARINE. CRISIS: WEAPONS TIGHT, LEAVE THE AREA IF DETECTED."})};
    s.radio = r;
    s.briefing[0] = {
        "ORDINI DI MISSIONE", "DA: COMSUBLANT", "A: USS LOS ANGELES (SSN-688)", "OGG.: ESERCITAZIONE DEL BALTICO - SORVEGLIANZA",
        "18 settembre 1989, ore 1400Z. Teatro: Baltico, baia di Meclemburgo. Pace.",
        "La Volksmarine e la Flotta del Baltico fanno la loro esercitazione d'autunno a nord di Rostock-Warnemuende: corvette, una Krivak II, un Kilo nuovo di fabbrica.",
        "Registratene le procedure: frequenze dei sonar attivi, tattiche di caccia, comunicazioni. Classificate almeno quattro unita' e restate novanta minuti nell'area.",
        "Qui il fondale e' di venticinque metri e la vela del Los Angeles da sola ne misura nove. Ogni accostata va pensata due volte; ogni ping sovietico rimbalza fra fondo e superficie.",
        "Regole d'ingaggio: armi trattenute. Non fatevi scoprire e non toccate il fondo.", "Tempo a disposizione: 2 ore.",
        "Nota: inedita; unita' e procedure sono di fantasia."};
    s.briefing[1] = {
        "MISSION ORDERS", "FM: COMSUBLANT", "TO: USS LOS ANGELES (SSN-688)", "SUBJ: BALTIC EXERCISE - SURVEILLANCE",
        "18 September 1989, 1400Z. Theatre: Baltic, Bay of Mecklenburg. Peace.",
        "The Volksmarine and the Baltic Fleet are holding their autumn exercise north of Rostock-Warnemuende: corvettes, a Krivak II, a brand-new Kilo.",
        "Record their procedures: active sonar frequencies, hunting tactics, communications. Classify at least four units and stay ninety minutes in the area.",
        "The water here is twenty-five metres deep and the Los Angeles's sail alone measures nine. Every turn must be thought twice; every Soviet ping bounces between bottom and surface.",
        "Rules of engagement: weapons tight. Do not get detected and do not touch bottom.", "Time available: 2 hours.",
        "Note: original; units and procedures are fictional."};
    return s;
}

// LIB-I12 «Finestra di lancio»: il TLAM nella notte polare del Barents. Il 688, cinque giorni senza punto nave
// (navigazione incerta, sigma 2 miglia), ha in libreria due missioni contro i radar d'allarme di Kola. Un'ora dopo la
// partenza arriva un ordine d'esecuzione per la 0522 con l'autenticazione che non torna (`autentico = false`: la
// missione non diventa eseguibile e la consolle lo dice); venti minuti dopo quello vero, per la 0521. Il PLP e' in
// un'area dove passano una Krivak II e un Il-38; all'allarme entra il Ka-27.
// Semplificazioni (docs/17): il teatro comincia a 31,3 E, quindi il 688 sta a nord del Barents occidentale e non davanti
// al Varangerfjord; il secondario «nessun lancio su un ordine non autenticato» lo garantisce il motore (RDY resta spento
// senza l'ordine autentico), quindi non e' un obiettivo.
ScenarioMissione finestra_di_lancio(const DatiScenario& d) {
    const Fondale f = Fondale::carica(DATI + "/teatri/barents.fdl");
    ScenarioMissione s;
    base(s, d, f);
    s.id = "LIB_I12_FINESTRA_DI_LANCIO";
    s.titolo = {"Finestra di lancio", "Launch window"};
    s.teatro = "barents";
    s.giorno = 17;
    s.mese = 1;
    s.anno = 1989;
    s.ora_utc = 2.0;
    s.situazione = "guerra";
    s.paranoia = 2;
    s.difficolta = 4;
    s.durata_s = 3.0 * 3600.0;
    s.carico.dotazione = {{"mk48", 16}, {"tlam_c", 6}, {"esca", 6}};
    s.carico.deposito = {{"mk48", 16}, {"tlam_c", 6}, {"esca", 6}};
    s.carico.tubi = {"tlam_c", "tlam_c", "mk48", "mk48", "esca", "esca", "tlam_c"};
    MeteoFissoScenario m;
    m.stato_mare = 3.0;
    m.vento_m_s = 9.0;
    m.vento_da_gradi = 30.0;
    m.nubi_ottavi = 8.0;
    m.neve_mm_h = 2.0;
    m.visibilita_m = 3000.0;
    s.meteo_fisso = m;
    s.navigazione = NavigazioneScenario{2.0};   // cinque giorni dall'ultimo punto nave sicuro
    const Vec2 plp1 = acqua(f, f.proiettata({70.75, 32.40}), 200.0), plp2 = acqua(f, f.proiettata({70.95, 33.20}), 200.0);
    s.unita.push_back(los_angeles(acqua(f, polare(plp1, 330.0, 40.0 * MIGLIO), 200.0), 150.0, 150.0));
    s.tlam = {{"0521", "OLENEGORSK, RADAR D'ALLARME", plp1, f.proiettata({69.75, 32.50}), 9000.0, 600.0, 90000.0},
              {"0522", "PECHENGA, RADAR DI SCOPERTA", plp2, f.proiettata({69.60, 33.30}), 9600.0, 600.0, 40000.0}};
    UnitaScenario krivak = unita(1, "krivak2", "nave", "urss", acqua(f, polare(plp1, 90.0, 9000.0), 150.0), 270.0, 12.0, "R_nave");
    for (int q = 0; q < 4; ++q) krivak.punti.push_back(acqua(f, polare(plp1, 90.0 * q, 9000.0), 150.0));
    krivak.ciclica = true;
    krivak.ruolo = "pattuglia dell'area di lancio";
    s.unita.push_back(krivak);
    UnitaScenario il38 = unita(2, "il38", "aereo", "urss", polare(plp1, 0.0, 25000.0), 90.0, 250.0, "R_aereo");
    for (int q = 0; q < 4; ++q) il38.punti.push_back(polare(plp1, 45.0 + 90.0 * q, 22000.0));
    il38.ciclica = true;
    il38.ruolo = "pattugliatore";
    s.unita.push_back(il38);
    AereoAswScenario a;
    a.unita = 2;
    a.base = f.proiettata({69.03, 33.42});   // Severomorsk-1
    a.situazione = "guerra";
    a.contro_giocatore = true;
    a.zona = plp1;
    a.raggio = 25000.0;
    s.aerei_asw.push_back(a);
    UnitaScenario ka27 = unita(3, "ka27", "aereo", "urss", polare(krivak.pos, 0.0, 2000.0), 0.0, 110.0, "R_aereo");
    for (int q = 0; q < 4; ++q) ka27.punti.push_back(polare(plp1, 90.0 * q, 8000.0));
    ka27.ciclica = true;
    ka27.entra_con = "allarme";
    ka27.ruolo = "Ka-27 di riserva";
    s.unita.push_back(ka27);
    zona(s, "plp_0521", {"l'area di lancio della 0521", "the 0521 launch area"}, plp1, 10.0 * MIGLIO);
    ObiettivoScenario arrivati = obiettivo("tlam_0521", "tlam", {}, {"Due TLAM della 0521 sul bersaglio nella finestra", "Two 0521 TLAMs on target within the window"});
    arrivati.missione = "0521";
    arrivati.minimo = 2;
    s.obiettivi.push_back(arrivati);
    s.obiettivi.push_back(obiettivo_zona("plp", "raggiungere", plp1, 10.0 * MIGLIO, {"Raggiungere l'area di lancio della 0521", "Reach the 0521 launch area"}, false));
    s.obiettivi.push_back(vivo());
    regola(s, "allarme", {quando(C::allarme)}, {azione(A::entra, 3)});
    RadioScenario r;
    r.ordini = OrdiniScenario{"COMSUBLANT", "USS LOS ANGELES (SSN 688)", "STRIKE - KOLA EARLY WARNING", "TOP SECRET"};
    r.roe = {{"sottomarini", "libero"}, {"navi", "libero"}, {"aerei", "libero"}};
    MessaggioScenario falso = radio("ESEGUIRE_0522", 3600.0, "flash", "COMSUBLANT", "TLAM EXECUTE", "TOP SECRET",
                                    {"ESEGUIRE MISSIONE TLAM 0522. ORA SUL BERSAGLIO 0440Z PIU' O MENO 10 MINUTI. AUTENTICAZIONE KILO ROMEO.",
                                     "EXECUTE TLAM MISSION 0522. TIME ON TARGET 0440Z PLUS OR MINUS 10 MINUTES. AUTHENTICATION KILO ROMEO."},
                                    {}, false);
    falso.esecuzione_tlam = "0522";
    MessaggioScenario vero = radio("ESEGUIRE_0521", 4800.0, "flash", "COMSUBLANT", "TLAM EXECUTE", "TOP SECRET",
                                   {"ESEGUIRE MISSIONE TLAM 0521, DUE MISSILI. ORA SUL BERSAGLIO 0430Z PIU' O MENO 10 MINUTI, ANTICIPATA DI TRENTA MINUTI. IGNORARE OGNI ALTRO ORDINE.",
                                    "EXECUTE TLAM MISSION 0521, TWO MISSILES. TIME ON TARGET 0430Z PLUS OR MINUS 10 MINUTES, ADVANCED THIRTY MINUTES. DISREGARD ANY OTHER ORDER."});
    vero.esecuzione_tlam = "0521";
    r.messaggi = {falso, vero};
    s.radio = r;
    s.briefing[0] = {
        "ORDINI DI MISSIONE", "DA: COMSUBLANT", "A: USS LOS ANGELES (SSN-688)", "OGG.: STRIKE - ALLARME PRECOCE DI KOLA",
        "17 gennaio 1989, ore 0200Z. Teatro: Mare di Barents. Guerra. Notte polare, neve.",
        "Siete in mare da diciannove giorni; l'ultimo punto nave sicuro e' di cinque giorni fa.",
        "In libreria avete due missioni TLAM contro i radar d'allarme della penisola di Kola, la 0521 e la 0522. Il comando le eseguira' nella notte, a un'ora che ancora non dice, e vuole i missili sul bersaglio entro dieci minuti dall'ora fissata.",
        "Il punto di lancio della 0521 e' a quaranta miglia, dove passano una Krivak e un Il-38. Un punto nave vi costa esposizione; senza, potete sbagliare di due miglia, e il TERCOM non perdona.",
        "Lanciate solo su un ordine d'esecuzione autenticato.", "Tempo a disposizione: 3 ore.",
        "Nota: inedita; bersagli e missioni sono di fantasia."};
    s.briefing[1] = {
        "MISSION ORDERS", "FM: COMSUBLANT", "TO: USS LOS ANGELES (SSN-688)", "SUBJ: STRIKE - KOLA EARLY WARNING",
        "17 January 1989, 0200Z. Theatre: Barents Sea. War. Polar night, snow.",
        "You have been at sea nineteen days; the last reliable fix was five days ago.",
        "Your library holds two TLAM missions against the early-warning radars of the Kola peninsula, 0521 and 0522. Command will execute them tonight, at a time not yet given, and wants the missiles on target within ten minutes of the set time.",
        "The 0521 launch point is forty miles away, where a Krivak and an Il-38 patrol. A fix costs exposure; without one you may be two miles off, and TERCOM does not forgive.",
        "Launch only on an authenticated execute order.", "Time available: 3 hours.",
        "Note: original; targets and missions are fictional."};
    return s;
}

// LIB-I11 «Il primo eco»: il tiro Harpoon oltre l'orizzonte su uno Slava con due Krivak II che scende verso la soglia
// Islanda-Fær Øer. Il dato e' quello del P-3 islandese per radio, ogni mezz'ora (contatto d'intelligence con 3 km
// d'errore); a 40 minuti, subito dopo il secondo rapporto, il gruppo accosta di 60 gradi a sinistra e si divide: lo
// Slava resta con una Krivak, l'altra prosegue verso la soglia. Sulla nuova rotta passano un traghetto norvegese diretto
// in Islanda e sei pescherecci delle Fær Øer, che per il seeker sono echi come lo Slava. Se lo Slava vede il 688 in
// superficie gli tira due P-500 del modello (l'IA dei missili del runtime); il 688 immerso lo cercano le armi antisom.
// Il traghetto e' della classe «mercantile», che ha il modello del livello classico (come in S13).
// Semplificazioni (docs/17): il ritardo della catena del P-3 non c'e' (il contatto e' la posizione al momento del
// messaggio); «non farsi trovare dalla Krivak» e' «non farsi scoprire», da chiunque; lo Slava tira una salva sola (sotto i
// 20 km il P-500 non parte e restano le armi antisom).
ScenarioMissione primo_eco(const DatiScenario& d) {
    const Fondale f = Fondale::carica(DATI + "/teatri/faroe.fdl");
    ScenarioMissione s;
    base(s, d, f);
    s.id = "LIB_I11_IL_PRIMO_ECO";
    s.titolo = {"Il primo eco", "The first echo"};
    s.teatro = "faroe";
    s.giorno = 6;
    s.mese = 3;
    s.anno = 1989;
    s.ora_utc = 10.5;
    s.situazione = "guerra";
    s.paranoia = 2;
    s.difficolta = 3;
    s.durata_s = 2.0 * 3600.0;
    s.carico.dotazione = {{"mk48", 20}, {"harpoon", 4}, {"esca", 6}};
    s.carico.deposito = {{"mk48", 20}, {"harpoon", 4}, {"esca", 6}};
    s.carico.tubi = {"harpoon", "harpoon", "mk48", "mk48", "esca", "esca", "harpoon"};
    MeteoFissoScenario m;
    m.stato_mare = 4.0;
    m.vento_m_s = 11.0;
    m.vento_da_gradi = 45.0;
    m.nubi_ottavi = 6.0;
    m.visibilita_m = 8000.0;
    s.meteo_fisso = m;
    const Vec2 partenza = acqua(f, f.proiettata({62.80, -7.30}), 400.0);
    const Vec2 gruppo = acqua(f, f.proiettata({63.20, -5.20}), 600.0), soglia = acqua(f, f.proiettata({62.90, -9.30}), 400.0);
    const double rotta = rilevamento(gruppo, soglia), nuova = rotta - 60.0;
    const Vec2 svolta = polare(gruppo, rotta, 14.0 * 0.5144 * 2400.0);   // dove sara' lo Slava a 40 minuti
    const Vec2 incrocio = polare(svolta, nuova, 15000.0);                 // e mezz'ora dopo, sulla rotta nuova
    s.unita.push_back(los_angeles(partenza, 60.0, 60.0));
    UnitaScenario slava = unita(1, "slava", "nave", "urss", gruppo, rotta, 14.0, "R_nave");
    slava.punti = {soglia};
    slava.ruolo = "nave ammiraglia del gruppo";
    s.unita.push_back(slava);
    UnitaScenario scorta = unita(2, "krivak2", "nave", "urss", polare(gruppo, rotta + 90.0, 4000.0), rotta, 14.0, "R_nave");
    scorta.punti = {polare(soglia, rotta + 90.0, 4000.0)};
    scorta.ruolo = "scorta dello Slava";
    s.unita.push_back(scorta);
    UnitaScenario avanti = unita(3, "krivak2", "nave", "urss", polare(gruppo, rotta, 6000.0), rotta, 14.0, "R_nave");
    avanti.punti = {soglia};
    avanti.ruolo = "schermo avanzato, poi verso la soglia";
    s.unita.push_back(avanti);
    UnitaScenario ka27 = unita(4, "ka27", "aereo", "urss", polare(svolta, 0.0, 2000.0), 0.0, 110.0, "R_aereo");
    for (int q = 0; q < 4; ++q) ka27.punti.push_back(polare(incrocio, 90.0 * q, 10000.0));
    ka27.ciclica = true;
    ka27.entra_con = "allarme";
    ka27.ruolo = "Ka-27 dello Slava";
    s.unita.push_back(ka27);
    // il traghetto Bergen-Seydisfjordur passa l'incrocio a mezz'ora dalla svolta (17 nodi verso ovest)
    UnitaScenario traghetto = unita(5, "mercantile", "nave", "neutrale", acqua(f, polare(incrocio, 90.0, 17.0 * 0.5144 * 4500.0), 200.0), 270.0, 17.0, "", true);
    traghetto.punti = {acqua(f, polare(incrocio, 270.0, 60000.0), 200.0)};
    traghetto.ruolo = "traghetto norvegese per l'Islanda";
    s.unita.push_back(traghetto);
    const Vec2 banco = polare(incrocio, nuova, 8000.0);
    for (int k = 0; k < 6; ++k) {
        UnitaScenario p = unita(6 + k, "peschereccio", "nave", "neutrale", acqua(f, polare(banco, 60.0 * k, 1500.0 + 600.0 * k), 80.0), 30.0 + 50.0 * k, 3.0, "", true);
        p.ruolo = "peschereccio delle Fær Øer";
        s.unita.push_back(p);
    }
    s.obiettivi.push_back(obiettivo("slava", "colpire", {1}, {"Colpire lo Slava", "Hit the Slava"}));
    s.obiettivi.push_back(non_sparare({5, 6, 7, 8, 9, 10, 11}, {"Nessun neutrale colpito", "No neutral hit"}));
    ObiettivoScenario scatola = obiettivo("scatola", "scatola", {}, {"Tirare con la scatola piccola o media", "Fire with the small or medium box"}, false);
    scatola.massima = "media";
    s.obiettivi.push_back(scatola);
    s.obiettivi.push_back(obiettivo("occulto", "non_essere_scoperto", {}, {"Non farsi trovare dalle Krivak", "Stay clear of the Krivaks"}, false));
    s.obiettivi.push_back(vivo());
    // la svolta: lo Slava e la sua scorta accostano a sinistra, lo schermo prosegue
    regola(s, "svolta", {quando(C::tempo, 2400.0)},
           {vai(1, acqua(f, polare(svolta, nuova, 60000.0), 150.0)), vai(2, acqua(f, polare(polare(svolta, nuova, 60000.0), nuova + 90.0, 3000.0), 150.0))});
    regola(s, "allarme", {quando(C::allarme)}, {azione(A::entra, 4)});
    // il P-500 e' un antinave: lo Slava lo tira sul 688 solo se lo vede in superficie
    CondizioneScenario vede = quando(C::unita_vede, 0.5);
    vede.unita = 1;
    AzioneScenario salva = azione(A::lancia_missile, 1);
    salva.valore = 2;
    regola(s, "p500", {vede, quando(C::giocatore_in_superficie)}, {salva});
    RadioScenario r;
    r.ordini = OrdiniScenario{"COMSUBLANT", "USS LOS ANGELES (SSN 688)", "ANTI-SURFACE STRIKE - GIUK GAP", "SECRET"};
    r.roe = {{"sottomarini", "libero"}, {"navi", "libero"}, {"aerei", "libero"}};
    for (int k = 0; k < 4; ++k) {   // i rapporti del P-3 a 5, 35, 65 e 95 minuti
        MessaggioScenario p3 = radio("P3_" + std::to_string(k + 1), 300.0 + 1800.0 * k, "immediate", "CTF 84", "SURFACE GROUP POSITION", "SECRET",
                                     {"RAPPORTO DEL P-3 ISLANDESE: GRUPPO DI SUPERFICIE SOVIETICO NEL CONTATTO D'INTELLIGENCE. NEUTRALI IN ZONA.",
                                      "ICELANDIC P-3 REPORT: SOVIET SURFACE GROUP IN THE INTELLIGENCE CONTACT. NEUTRALS IN THE AREA."});
        p3.contatto = {1, 2, 3};
        p3.contatto_errore_m = 3000.0;
        p3.contatto_sensore = "aereo";
        r.messaggi.push_back(p3);
    }
    s.radio = r;
    regola(s, "colpito", {unita_colpita(1)},
           {fine(40, {"Lo Slava e' stato colpito: il gruppo non apre la soglia.", "The Slava has been hit: the group will not open the gap."})});
    s.briefing[0] = {
        "ORDINI DI MISSIONE", "DA: COMSUBLANT", "A: USS LOS ANGELES (SSN-688)", "OGG.: ATTACCO ANTINAVE - SOGLIA GIUK",
        "6 marzo 1989, ore 1030Z. Teatro: Fær Øer. Guerra. Mare 4, visibilita' a tratti.",
        "Un gruppo di superficie sovietico, uno Slava con due Krivak, scende dal Mare di Norvegia per aprire la soglia ai sottomarini. Un P-3 islandese lo ha visto all'alba e ne manda la posizione ogni mezz'ora.",
        "Il gruppo e' a sessanta miglia: abbastanza vicino per un Harpoon, troppo lontano per vederlo.",
        "Sulla stessa rotta ci sono un traghetto norvegese e i pescherecci delle Fær Øer, che nessuno ha fatto rientrare. Il seeker dell'Harpoon prende il primo eco che trova.",
        "Regole d'ingaggio: armi libere contro le navi sovietiche; nessuna arma contro i neutrali.", "Tempo a disposizione: 2 ore.",
        "Nota: inedita; unita' e nomi di fantasia."};
    s.briefing[1] = {
        "MISSION ORDERS", "FM: COMSUBLANT", "TO: USS LOS ANGELES (SSN-688)", "SUBJ: ANTI-SURFACE STRIKE - GIUK GAP",
        "6 March 1989, 1030Z. Theatre: Faroes. War. Sea state 4, patchy visibility.",
        "A Soviet surface group, a Slava with two Krivaks, is coming down from the Norwegian Sea to open the gap for the submarines. An Icelandic P-3 spotted it at dawn and reports its position every half hour.",
        "The group is sixty miles away: close enough for a Harpoon, too far to see.",
        "On the same track are a Norwegian ferry and the Faroese fishing boats nobody called home. The Harpoon seeker takes the first echo it finds.",
        "Rules of engagement: weapons free against Soviet ships; no weapons against neutrals.", "Time available: 2 hours.",
        "Note: original; units and names are fictional."};
    return s;
}

// LIB-I13 «Sagome nello Stretto» e LIB-I14 «Levante» (docs/25 §4, schede approvate il 29/10/2026): addestramento al
// riconoscimento visivo nello Stretto di Gibilterra, in pace, con le armi d'esercitazione. Il 688 a quota periscopica
// sul lato del Mare di Alboran; il gruppo americano entra da ovest, la 5a Eskadra aspetta a est, l'AGI pedina, la Koni
// algerina (neutrale) esce da sud a 50 minuti, il traffico sullo schema di separazione e i pescherecci fra le coste.
// Le coppie che ingannano: Kidd e Spruance (stesso scafo), l'AGI e i pescherecci, la Koni e la Krivak. A 40 minuti Kidd
// e Spruance si scambiano di posto. «Levante» e' la mattina dopo, con la nebbia d'avvezione e il traffico in fila.
// Gli obiettivi «identificare», «telemetro» e «rotta_stimata» li valuta il registro (runtime, 29/10/2026); col Levante
// una collisione chiude in fallimento.
// Semplificazioni: la «nave spia» e' la classe agi del libretto (un Okean); i nomi sono di fantasia.
ScenarioMissione sagome_nello_stretto(const DatiScenario& d, bool levante, bool notte = false) {
    const Fondale f = Fondale::carica(DATI + "/teatri/gibilterra.fdl");
    ScenarioMissione s;
    base(s, d, f);
    s.id = notte ? "LIB_I15_FANALI" : levante ? "LIB_I14_LEVANTE" : "LIB_I13_SAGOME_NELLO_STRETTO";
    s.titolo = notte    ? TestiScenario{"Fanali", "Running Lights"}
               : levante ? TestiScenario{"Levante", "Levanter"}
                         : TestiScenario{"Sagome nello Stretto", "Silhouettes in the Strait"};
    s.teatro = "gibilterra";
    s.giorno = notte ? 3 : levante ? 13 : 12;
    s.mese = 6;
    s.anno = 1989;
    s.ora_utc = notte ? 23.5 : levante ? 6.5 : 15.0;
    s.situazione = "pace";
    s.paranoia = 1;
    s.difficolta = notte ? 4 : levante ? 3 : 2;
    s.durata_s = 1.5 * 3600.0;
    s.esercitazione = true;   // siluri senza testa: si prova la soluzione di tiro, senza danni
    s.carico.dotazione = {{"mk48", 22}, {"esca", 6}};
    s.carico.deposito = {{"mk48", 18}, {"esca", 6}};
    s.carico.tubi = {"mk48", "mk48", "mk48", "mk48", "esca", "esca", "mk48"};
    MeteoFissoScenario m;
    if (levante) {
        m.stato_mare = 3.0, m.vento_m_s = 12.0, m.vento_da_gradi = 90.0, m.visibilita_m = 1500.0;
        m.nebbia = "avvezione";
    } else if (notte) {   // una notte senza luna, qualche stella
        m.stato_mare = 2.0, m.vento_m_s = 4.0, m.vento_da_gradi = 270.0, m.visibilita_m = 15000.0, m.nubi_ottavi = 2.0;
    } else {
        m.stato_mare = 2.0, m.vento_m_s = 4.0, m.vento_da_gradi = 270.0, m.visibilita_m = 20000.0, m.nubi_ottavi = 1.0;
    }
    s.meteo_fisso = m;
    // il 688 a est dello Stretto; il gruppo americano passa a 3-5 km a nord di lui, verso est
    const Vec2 partenza = acqua(f, f.proiettata({35.97, -5.05}), 300.0);
    // il gruppo parte 16 km a ovest e passa a nord del 688 fra 25 e 40 minuti: a 2,8 km di giorno, a 1,1 km col Levante
    // (visibilita' 1,5 km); la 5a Eskadra arriva da est
    const Vec2 entrata = acqua(f, polare(partenza, 284.0, 16000.0), 300.0);
    const Vec2 uscita = acqua(f, polare(entrata, levante ? 100.0 : 94.0, 60000.0), 600.0);
    const double rotta_usa = rilevamento(entrata, uscita), nodi_usa = levante ? 10.0 : 14.0;
    s.unita.push_back(los_angeles(partenza, 270.0, 5.5));   // a quota periscopica
    s.unita.front().velocita = 3.0;
    const Vec2 centro = polare(entrata, rotta_usa, 0.0);
    const auto usa = [&](int id, const char* classe, double di_lato, double avanti, const char* ruolo) {
        const Vec2 p = acqua(f, polare(polare(centro, rotta_usa + 90.0, di_lato), rotta_usa, avanti), 200.0);
        UnitaScenario u = unita(id, classe, "nave", "usa", p, rotta_usa, nodi_usa, "");
        u.punti = {acqua(f, polare(polare(uscita, rotta_usa + 90.0, di_lato), rotta_usa, avanti), 200.0)};
        u.ruolo = ruolo;
        return u;
    };
    // in colonna: la Perry davanti, il Sacramento al centro, Kidd a dritta e Spruance a sinistra (poi si scambiano)
    s.unita.push_back(usa(1, "kidd", 1500.0, 0.0, "cacciatorpediniere lanciamissili (scafo Spruance)"));
    s.unita.push_back(usa(2, "spruance", -1500.0, 0.0, "cacciatorpediniere (scafo come la Kidd)"));
    s.unita.push_back(usa(3, "perry", 0.0, 3000.0, "fregata in testa"));
    s.unita.push_back(usa(4, "sacramento", 0.0, 0.0, "rifornitrice veloce al centro"));
    // la 5a Eskadra: 18 km a est del 688, verso ovest a 8 nodi
    const Vec2 eskadra = acqua(f, polare(partenza, 95.0, 18000.0), 500.0);
    UnitaScenario kara = unita(5, "kara", "nave", "urss", eskadra, 260.0, 8.0, "R_nave");
    kara.punti = {acqua(f, polare(eskadra, 260.0, 30000.0), 400.0)};
    kara.ruolo = "incrociatore della 5a Eskadra";
    if (notte) kara.luci = "spente";   // a luci spente vicino al gruppo NATO: la provocazione (scheda I15)
    s.unita.push_back(kara);
    UnitaScenario krivak = unita(6, "krivak2", "nave", "urss", acqua(f, polare(eskadra, 170.0, 2500.0), 400.0), 260.0, 8.0, "R_nave");
    krivak.punti = {acqua(f, polare(polare(eskadra, 170.0, 2500.0), 260.0, 30000.0), 400.0)};
    krivak.ruolo = "scorta della Kara";
    s.unita.push_back(krivak);
    UnitaScenario nanuchka = unita(7, "nanuchka", "nave", "urss", acqua(f, polare(eskadra, 350.0, 3000.0), 400.0), 260.0, 8.0, "R_nave");
    nanuchka.punti = {acqua(f, polare(polare(eskadra, 350.0, 3000.0), 260.0, 30000.0), 400.0)};
    nanuchka.ruolo = "corvetta lanciamissili";
    s.unita.push_back(nanuchka);
    // di notte la nave spia (11,5 nodi al massimo: il gruppo a 14 la lascerebbe indietro) aspetta il gruppo nello Stretto,
    // vicino alla sua rotta a nord-est del 688, come le AGI ai passaggi obbligati
    const Vec2 agi_da = notte ? acqua(f, polare(partenza, 30.0, 4500.0), 200.0) : acqua(f, polare(entrata, rotta_usa + 160.0, 5000.0), 200.0);
    UnitaScenario agi = unita(8, "agi", "nave", "urss", agi_da, rotta_usa, 9.0, "R_pedina");
    agi.parametri.distanza_nm = 2.0;
    agi.ruolo = "nave spia che pedina il gruppo americano";
    if (notte) agi.luci = "spente";
    s.unita.push_back(agi);
    // la Koni algerina, neutrale: esce da sud a 50 minuti e taglia la rotta del gruppo
    const Vec2 sud = acqua(f, polare(partenza, 160.0, 15000.0), 300.0);
    UnitaScenario koni = unita(9, "koni", "nave", "neutrale", sud, 320.0, 16.0, "");
    koni.punti = {acqua(f, polare(sud, 320.0, 40000.0), 200.0)};
    koni.entra_con = "koni";
    koni.ruolo = "fregata algerina di costruzione sovietica";
    s.unita.push_back(koni);
    // il traffico: due corsie dello schema di separazione, a nord e a sud del 688; in fila col Levante
    const char* const CIVILI[] = {"mercantile", "cargo", "petroliera", "petroliera", "mercantile"};
    for (int k = 0; k < 5; ++k) {
        const bool est = k % 2 == 0;
        const Vec2 p = acqua(f, polare(polare(partenza, est ? 200.0 : 20.0, 3500.0 + 400.0 * k), est ? 80.0 : 260.0,
                                       (levante ? -2500.0 : -9000.0) * (k / 2 + 1)), 200.0);
        UnitaScenario c = unita(10 + k, CIVILI[k], "nave", "neutrale", p, est ? 80.0 : 260.0, levante ? 8.0 : 12.0, "", true);
        c.punti = {acqua(f, polare(p, est ? 80.0 : 260.0, 60000.0), 150.0)};
        c.ruolo = est ? "mercantile verso est" : "mercantile verso ovest";
        if (notte && k == 1) {   // di notte uno fermo in avaria a 4 km, fuori corsia: i due rossi tutt'intorno. Alla fonda
            // no: sotto il 688 ci sono 500-900 m d'acqua, e i fondali da ancoraggio sono a piu' di 20 km
            c.pos = acqua(f, polare(partenza, 160.0, 4000.0), 200.0);
            c.velocita = 0.0;
            c.punti.clear();
            c.luci = "non_governa";
            c.ruolo = "mercantile in avaria, senza governo";
        }
        s.unita.push_back(c);
    }
    for (int k = 0; k < (notte ? 6 : 4); ++k) {   // di notte due in piu', illuminati: i falsi bersagli
        // di notte sul fondo del gambero rosso (lo strascico di Alboran lavora fra 400 e 800 m): restano a 6-10 km, nella
        // portata delle loro luci; di giorno sotto costa fra 40 e 200 m
        const Vec2 p = acqua_fra(f, polare(partenza, k < 2 ? 340.0 + 15.0 * k : 160.0 + 15.0 * k, 6000.0 + 1500.0 * k), notte ? 400.0 : 40.0,
                                 notte ? 800.0 : 200.0);
        UnitaScenario p_ = unita(15 + k, "peschereccio", "nave", "neutrale", p, std::fmod(45.0 + 90.0 * k, 360.0), 3.0, "", true);
        p_.ruolo = k < 2 ? "peschereccio spagnolo" : "peschereccio marocchino";
        s.unita.push_back(p_);
    }
    // la svolta: Kidd e Spruance si scambiano di posto; la Koni esce da sud
    regola(s, "scambio", {quando(C::tempo, 2400.0)},
           {vai(1, acqua(f, polare(uscita, rotta_usa - 90.0, 1500.0), 200.0)), vai(2, acqua(f, polare(uscita, rotta_usa + 90.0, 1500.0), 200.0))});
    regola(s, "koni", {quando(C::tempo, 3000.0)}, {azione(A::entra, 9)});
    if (notte) {
        // la svolta della notte: a 30 minuti la Kara e l'AGI, buie, tagliano la scia del gruppo americano 2 km dietro il
        // centro. Il punto sulla rotta e' quello in cui la Kara a 20 nodi arriva quando il gruppo l'ha passato di 2 km
        const double v_usa = nodi_usa * NODO, v_kara = 20.0 * NODO, svolta = 1800.0;
        const Vec2 kara_svolta = polare(eskadra, 260.0, 8.0 * NODO * svolta);
        double lungo = v_usa * svolta;
        while (lungo < v_usa * svolta + 30000.0 &&
               svolta + distanza(kara_svolta, polare(entrata, rotta_usa, lungo)) / v_kara > (lungo + 2000.0) / v_usa)
            lungo += 100.0;
        const Vec2 taglio = polare(entrata, rotta_usa, lungo);
        regola(s, "scia", {quando(C::tempo, svolta)},
               {vai(5, acqua(f, polare(taglio, rilevamento(kara_svolta, taglio), 4000.0), 300.0), 20.0),
                vai(8, acqua(f, polare(taglio, rotta_usa - 90.0, 4000.0), 300.0), 12.0)});
    }
    // gli obiettivi del riconoscimento (registro del runtime): l'ultima classificazione visiva giusta di ogni nave da
    // guerra, le distanze lette col telemetro, le rotte dall'AOB
    ObiettivoScenario identifica = obiettivo("identifica", "identificare", {1, 2, 3, 4, 5, 6, 7, 8, 9},
                                             levante || notte ? TestiScenario{"Identificare a vista almeno 5 delle 9 navi da guerra", "Identify by sight at least 5 of the 9 warships"}
                                                              : TestiScenario{"Identificare a vista almeno 7 delle 9 navi da guerra", "Identify by sight at least 7 of the 9 warships"});
    identifica.minimo = levante || notte ? 5 : 7;
    s.obiettivi.push_back(identifica);
    ObiettivoScenario telemetro = obiettivo("telemetro", "telemetro", {},
                                            levante || notte ? TestiScenario{"Tre distanze col telemetro entro il 15%", "Three stadimeter ranges within 15%"}
                                                             : TestiScenario{"Tre distanze col telemetro entro il 10%", "Three stadimeter ranges within 10%"},
                                            !notte);   // di notte, sul fanale d'albero, e' secondario
    telemetro.minimo = 3;
    telemetro.errore = levante || notte ? 0.15 : 0.10;
    s.obiettivi.push_back(telemetro);
    // di notte l'AOB dai fanali (il colore del laterale, l'allineamento dei fanali d'albero) e' la capacita' da provare
    ObiettivoScenario rotta = notte ? obiettivo("rotta", "rotta_stimata", {}, {"Tre rotte dai fanali entro 20 gradi", "Three courses from the running lights within 20 degrees"})
                                    : obiettivo("rotta", "rotta_stimata", {}, {"Tre rotte dall'angolo sulla prora entro 20 gradi", "Three courses from the angle on the bow within 20 degrees"}, false);
    rotta.minimo = 3;
    rotta.errore = 20.0;
    s.obiettivi.push_back(rotta);
    if (!notte) {   // di notte la nave spia e' buia, e niente prova di tiro: e' la notte del riconoscimento
    ObiettivoScenario foto = obiettivo("foto_agi", "foto", {8}, {"Fotografare la nave spia entro 3 km", "Photograph the spy ship within 3 km"}, false);
    foto.distanza_m = 3000.0;
    s.obiettivi.push_back(foto);
    s.obiettivi.push_back(obiettivo("tiro", "colpire", {1, 2, 3, 4, 5, 6, 7}, {"Provare la soluzione di tiro (siluri d'esercitazione)",
                                                                              "Prove the firing solution (exercise torpedoes)"}, false));
    }
    s.obiettivi.push_back(obiettivo("occulto", "non_essere_scoperto", {}, {"Non farsi scoprire", "Stay undetected"}, false));
    s.obiettivi.push_back(vivo());
    if (levante)   // nella nebbia una collisione chiude l'esercitazione
        regola(s, "collisione", {evento("collisione")},
               {fine(41, {"Collisione nella nebbia: l'esercitazione e' sospesa.", "Collision in the fog: the exercise is called off."}, "fallimento")});
    else if (notte)   // e nel buio
        regola(s, "collisione", {evento("collisione")},
               {fine(41, {"Collisione nel buio: l'esercitazione e' sospesa.", "Collision in the dark: the exercise is called off."}, "fallimento")});
    RadioScenario r;
    r.ordini = OrdiniScenario{"COMSUBLANT", "USS LOS ANGELES (SSN 688)",
                              notte ? "NIGHT RECOGNITION EXERCISE - STRAIT OF GIBRALTAR" : "VISUAL RECOGNITION EXERCISE - STRAIT OF GIBRALTAR", "CONFIDENTIAL"};
    r.roe = {{"sottomarini", "trattenuto"}, {"navi", "trattenuto"}, {"mercantili", "trattenuto"}, {"neutrali", "trattenuto"}, {"aerei", "trattenuto"}};
    s.radio = r;
    if (notte) {
        s.briefing[0] = {
            "ORDINI DI MISSIONE", "DA: COMSUBLANT", "A: USS LOS ANGELES (SSN-688)", "OGG.: ESERCITAZIONE DI RICONOSCIMENTO NOTTURNO - GIBILTERRA",
            "3 giugno 1989, ore 2330Z. Teatro: Stretto di Gibilterra. Pace. Notte senza luna, mare calmo, visibilita' 15 km.",
            "Dopo il giorno e la nebbia, la prova piu' difficile: riconoscere di notte, quando al periscopio si vedono solo luci. Il gruppo americano entra nello Stretto coi fanali accesi, come vuole il regolamento.",
            "La 5a Eskadra no: l'incrociatore Kara e la nave spia navigano a luci spente vicino al gruppo NATO, contro il regolamento. Si vedono solo come sagome contro il cielo, e solo da vicino. Il sonar le sente prima.",
            "Fra le coste lavorano i pescherecci, illuminati dalle luci di coperta: di notte sembrano una flotta. Il verde sopra il bianco e' un peschereccio a strascico; i bianchi tutt'intorno, una nave alla fonda.",
            "Identificare a vista cinque navi da guerra. Stimare la rotta di tre unita' dai fanali: il laterale rosso e' il fianco sinistro, il verde il dritto; l'allineamento dei fanali d'albero dice quanto la nave viene incontro. Col telemetro, la distanza di tre unita' sul fanale d'albero.",
            "Non farsi scoprire: la Kara buia ascolta. Il battello a quota periscopica non ha luci; in superficie coi fanali accesi si fa vedere lontano. Una collisione chiude l'esercitazione. Regole d'ingaggio: armi trattenute.",
            "Tempo a disposizione: 1 ora e 30.", "Nota: inedita; unita' e nomi di fantasia."};
        s.briefing[1] = {
            "MISSION ORDERS", "FM: COMSUBLANT", "TO: USS LOS ANGELES (SSN-688)", "SUBJ: NIGHT RECOGNITION EXERCISE - GIBRALTAR",
            "3 June 1989, 2330Z. Theatre: Strait of Gibraltar. Peace. Moonless night, calm sea, visibility 15 km.",
            "After the day and the fog, the hardest test: recognition at night, when the periscope shows only lights. The American group enters the Strait with its running lights on, as the rules require.",
            "The 5th Eskadra does not: the Kara cruiser and the spy ship run darkened close to the NATO group, against the rules. They show only as silhouettes against the sky, and only at close range. The sonar hears them first.",
            "Fishing boats work between the coasts, lit by their deck lights: at night they look like a fleet. Green over white is a trawler; all-round whites, a ship at anchor.",
            "Identify five warships by sight. Estimate the course of three units from their lights: a red sidelight is the port side, green the starboard; the alignment of the masthead lights tells how much the ship is heading towards you. With the stadimeter, range three units on the masthead light.",
            "Stay undetected: the darkened Kara is listening. At periscope depth the boat shows no lights; surfaced with its lights on it can be seen from afar. A collision ends the exercise. Rules of engagement: weapons tight.",
            "Time available: 1 hour 30.", "Note: original; units and names are fictional."};
        return s;
    }
    const std::string quando_it = levante ? "13 giugno 1989, ore 0630Z. Teatro: Stretto di Gibilterra. Pace. Levante: nebbia, visibilita' 1-2 km."
                                          : "12 giugno 1989, ore 1500Z. Teatro: Stretto di Gibilterra. Pace. Mare calmo, visibilita' 20 km.";
    const std::string quando_en = levante ? "13 June 1989, 0630Z. Theatre: Strait of Gibraltar. Peace. Levanter: fog, visibility 1-2 km."
                                          : "12 June 1989, 1500Z. Theatre: Strait of Gibraltar. Peace. Calm sea, visibility 20 km.";
    s.briefing[0] = {
        "ORDINI DI MISSIONE", "DA: COMSUBLANT", "A: USS LOS ANGELES (SSN-688)", "OGG.: ESERCITAZIONE DI RICONOSCIMENTO VISIVO - GIBILTERRA", quando_it,
        "Prima del Mediterraneo orientale, il comandante deve saper riconoscere a vista quello che incontrera'. Nello Stretto passa di tutto: un gruppo americano che entra, la 5a Eskadra sovietica che lo aspetta, una fregata algerina, mercantili e pescherecci.",
        "Identificare col libretto ogni nave da guerra. Misurare col telemetro la distanza di tre unita' e stimarne l'angolo sulla prora. Fotografare la nave spia sovietica.",
        "Attenzione alle somiglianze: Kidd e Spruance hanno lo stesso scafo; la nave spia ha lo scafo di un peschereccio; la Koni algerina e' di costruzione sovietica.",
        "Siluri d'esercitazione, senza testa: si puo' provare la soluzione di tiro sulle navi da guerra identificate. Regole d'ingaggio: armi trattenute.",
        levante ? "Non farsi scoprire: la nave spia e l'incrociatore hanno l'ESM. Nella nebbia il traffico e' in fila: una collisione chiude l'esercitazione."
                : "Non farsi scoprire: la nave spia e l'incrociatore hanno l'ESM.",
        "Tempo a disposizione: 1 ora e 30.", "Nota: inedita; unita' e nomi di fantasia."};
    s.briefing[1] = {
        "MISSION ORDERS", "FM: COMSUBLANT", "TO: USS LOS ANGELES (SSN-688)", "SUBJ: VISUAL RECOGNITION EXERCISE - GIBRALTAR", quando_en,
        "Before the eastern Mediterranean, the captain must be able to recognise by sight whatever he meets. Everything passes through the Strait: an American group coming in, the Soviet 5th Eskadra waiting for it, an Algerian frigate, merchants and fishing boats.",
        "Identify every warship with the recognition book. Range three units with the stadimeter and estimate their angle on the bow. Photograph the Soviet spy ship.",
        "Beware of look-alikes: Kidd and Spruance share the same hull; the spy ship has a trawler's hull; the Algerian Koni is Soviet-built.",
        "Exercise torpedoes, no warheads: the firing solution may be tried on identified warships. Rules of engagement: weapons tight.",
        levante ? "Stay undetected: the spy ship and the cruiser carry ESM. In the fog the traffic runs in line: a collision ends the exercise."
                : "Stay undetected: the spy ship and the cruiser carry ESM.",
        "Time available: 1 hour 30.", "Note: original; units and names are fictional."};
    return s;
}

// LIB-I16 «Il bastione bianco» (docs/17 §2e): sotto il pack di marzo nel Mare di Okhotsk, il 688 entra dal Pacifico per lo
// Stretto di Ekaterina, trova e segue un Delta III nel bastione (scorta: un Victor III), e quando la campana dell'ELF
// lo chiama deve trovare acqua libera per trasmettere il rapporto (sotto la banchisa niente antenne: opzione A del
// ghiaccio, missione_ghiaccio.cpp). Il ghiaccio viene dal meteo di marzo (ERA5); il Crazy Ivan e' del motore
ScenarioMissione bastione_bianco(const DatiScenario& d) {
    const Fondale f = Fondale::carica(DATI + "/teatri/kunashir.fdl");
    ScenarioMissione s;
    base(s, d, f);
    s.id = "LIB_I16_BASTIONE_BIANCO";
    s.titolo = {"Il bastione bianco", "The White Bastion"};
    s.teatro = "kunashir";
    s.giorno = 14;
    s.mese = 3;
    s.anno = 1989;
    s.ora_utc = 1.0;   // mezzogiorno locale: la luce per guardare la polynya prima di salire
    s.situazione = "pace";
    s.paranoia = 2;
    s.difficolta = 5;
    s.durata_s = 4.0 * 3600.0;
    // il meteo del teatro e della stagione, non fisso: il ghiaccio viene dai punti ERA5 di marzo, col margine negli stretti
    // (Meteo::in non interpola i punti col meteo fisso). Per le missioni della libreria il menu lo prende da qui
    s.meteo_realistico = true;
    s.navigazione = NavigazioneScenario{0.5, true};   // la navigazione realistica di serie: sotto il pack niente punto nave
    carico_688(s);
    // lo Stretto di Ekaterina (I06): l'ingresso dal Pacifico e l'uscita a Okhotsk; il bastione subito a nord, sul fondo del
    // Bacino delle Curili
    const Vec2 partenza = acqua(f, f.proiettata({44.30, 146.90}), 250.0);
    const Vec2 sud = acqua(f, f.proiettata({44.45, 146.76}), 120.0);
    const Vec2 bastione = acqua(f, f.proiettata({44.80, 146.50}), 1200.0);
    s.unita.push_back(los_angeles(partenza, rilevamento(partenza, sud), 120.0));
    s.unita.front().velocita = 10.0;
    // il Delta III in pattuglia sul circuito del bastione, profondo (niente sonar del fondo: lontano dalle chiglie)
    UnitaScenario delta = unita(1, "delta3", "sottomarino", "urss", acqua(f, polare(bastione, 0.0, 9000.0), 1000.0), 90.0, 4.0, "");
    for (int q = 0; q < 4; ++q) delta.punti.push_back(acqua(f, polare(bastione, 90.0 * q + 90.0, 9000.0), 1000.0));
    delta.ciclica = true;
    delta.quota = 130.0;
    delta.ruolo = "SSBN in pattuglia nel bastione";
    s.unita.push_back(delta);
    // la scorta: un Victor III che gira piu' largo, con l'MG-70 sulla volta (l'IA scende sotto le chiglie misurate)
    UnitaScenario victor = unita(2, "victor3", "sottomarino", "urss", acqua(f, polare(bastione, 180.0, 14000.0), 1000.0), 270.0, 6.0,
                                 "R_sottomarino");
    for (int q = 0; q < 4; ++q) victor.punti.push_back(acqua(f, polare(bastione, 90.0 * q + 225.0, 14000.0), 1000.0));
    victor.ciclica = true;
    victor.quota = 90.0;
    victor.profonda = 150.0;
    victor.parametri.paranoia = 2;
    victor.ruolo = "scorta del Delta";
    s.unita.push_back(victor);
    // l'Il-38 sul margine, dal lato del Pacifico: le boe solo in acqua libera
    const Vec2 margine = acqua(f, f.proiettata({44.20, 147.05}), 200.0);
    UnitaScenario il38 = unita(3, "il38", "aereo", "urss", polare(margine, 90.0, 20000.0), 270.0, 250.0, "R_aereo");
    for (int q = 0; q < 4; ++q) il38.punti.push_back(polare(margine, 45.0 + 90.0 * q, 15000.0));
    il38.ciclica = true;
    il38.ruolo = "pattugliatore sul margine del ghiaccio";
    s.unita.push_back(il38);
    AereoAswScenario a;
    a.unita = 3;
    a.base = f.proiettata({45.25, 147.90});   // Burevestnik, Iturup
    a.situazione = "pace";
    a.contro_giocatore = true;
    a.zona = margine;
    a.raggio = 20000.0;
    s.aerei_asw.push_back(a);
    // il rompighiaccio con un mercantile dietro, lungo la costa di Kunashir verso nord: una sorgente forte, un falso bersaglio
    const Vec2 costa = acqua(f, f.proiettata({44.55, 146.15}), 100.0), nord_costa = acqua(f, f.proiettata({45.25, 146.0}), 200.0);
    UnitaScenario ghiaccio = unita(4, "rompighiaccio", "nave", "urss", costa, rilevamento(costa, nord_costa), 8.0, "", true);
    ghiaccio.punti = {nord_costa};
    ghiaccio.ruolo = "rompighiaccio";
    s.unita.push_back(ghiaccio);
    UnitaScenario carico = unita(5, "portarinfuse", "nave", "urss", polare(costa, rilevamento(nord_costa, costa), 1200.0),
                                 rilevamento(costa, nord_costa), 8.0, "", true);
    carico.punti = {nord_costa};
    carico.ruolo = "mercantile nel canale del rompighiaccio";
    s.unita.push_back(carico);
    for (int k = 0; k < 2; ++k) {   // due pescherecci d'altura al margine, dal lato del Pacifico
        const Vec2 p = acqua(f, f.proiettata({k ? 44.05 : 44.15, k ? 147.25 : 147.35}), 200.0);
        UnitaScenario b = unita(6 + k, "bmrt", "nave", "urss", p, 45.0 + 180.0 * k, 3.0, "", true);
        b.ruolo = "peschereccio d'altura al margine";
        s.unita.push_back(b);
    }
    zona(s, "pacifico", {"il Pacifico, fuori dallo stretto", "the Pacific, outside the strait"}, partenza, 9000.0);
    zona(s, "bastione", {"il bastione", "the bastion"}, bastione, 25000.0);
    // la radio: il contatto d'intelligence all'inizio; la campana (FLASH) quando il Delta e' seguito, o a 2 ore e 45
    RadioScenario r;
    r.ordini = OrdiniScenario{"COMSUBPAC", "USS LOS ANGELES (SSN 688)", "SSBN TRAIL - SEA OF OKHOTSK", "SECRET"};
    r.roe = {{"sottomarini", "trattenuto"}, {"navi", "trattenuto"}, {"mercantili", "trattenuto"}, {"neutrali", "trattenuto"}, {"aerei", "trattenuto"}};
    MessaggioScenario intel = radio("intel", 120.0, "immediate", "COMSUBPAC", "SSBN LOCATION", "SECRET",
                                    {"Fonti nazionali: un SSBN classe Delta III in pattuglia a nord dello Stretto di Ekaterina, sotto il pack. "
                                     "Posizione approssimata, errore 5 miglia. Probabile scorta.",
                                     "National sources: a Delta III class SSBN on patrol north of Ekaterina Strait, under the pack. "
                                     "Approximate position, error 5 miles. Escort probable."});
    intel.contatto = {1};
    intel.contatto_errore_m = 9000.0;
    r.messaggi.push_back(intel);
    r.messaggi.push_back(radio("campana", -1.0, "flash", "COMSUBPAC", "REPORT REQUIRED", "SECRET",
                               {"Riferire il pedinamento entro le 0430Z: rapporto di contatto in trasmissione. Sotto il pack: trovare acqua libera.",
                                "Report the trail by 0430Z: contact report by transmission. Under the pack: find open water."}));
    s.radio = r;
    // gli obiettivi: trovare e seguire il Delta, riferire entro le 0430Z (3 ore e 30), rientrare nel Pacifico
    s.obiettivi.push_back(obiettivo("trovato", "scoprire", {1}, {"Trovare e classificare il Delta III", "Find and classify the Delta III"}));
    s.obiettivi.push_back(obiettivo_contatto("seguito", {1}, 45.0, {"Seguirlo: 45 minuti di contatto", "Trail her: 45 minutes of contact"}));
    ObiettivoScenario rapporto = obiettivo_variabile("rapporto", 2, 1, {"Trasmettere il rapporto di contatto entro le 0430Z",
                                                                         "Transmit the contact report by 0430Z"});
    rapporto.entro_s = 3.5 * 3600.0;
    s.obiettivi.push_back(rapporto);
    s.obiettivi.push_back(obiettivo_variabile("rientro", 3, 1, {"Rientrare nel Pacifico", "Return to the Pacific"}));
    s.obiettivi.push_back(obiettivo("scorta", "scoprire", {2}, {"Classificare la scorta", "Classify the escort"}, false));
    s.obiettivi.push_back(obiettivo("occulto", "non_essere_scoperto", {}, {"Non farsi scoprire", "Stay undetected"}, false));
    s.obiettivi.push_back(vivo());
    regola(s, "campana", {compiuto("seguito")}, {imposta(1, "=", 1), manda("campana")});
    regola(s, "campana_a_tempo", {quando(C::tempo, 2.75 * 3600.0), variabile(1, "=", 0.0)}, {imposta(1, "=", 1), manda("campana")});
    CondizioneScenario trasmesso = quando(C::giocatore_ha_trasmesso, 0.0);
    trasmesso.trasmissione = "contatto";
    regola(s, "rapporto", {variabile(1, "=", 1.0), trasmesso}, {imposta(2, "=", 1)});
    regola(s, "rientro", {variabile(2, "=", 1.0), in_zona("pacifico")},
           {imposta(3, "=", 1), fine(40, {"Il rapporto e' a COMSUBPAC e il Los Angeles e' di nuovo nel Pacifico.",
                                          "The report is with COMSUBPAC and Los Angeles is back in the Pacific."})});
    s.briefing[0] = {
        "ORDINI DI MISSIONE", "DA: COMSUBPAC", "A: USS LOS ANGELES (SSN-688)", "OGG.: PEDINAMENTO DI UN SSBN - MARE DI OKHOTSK",
        "14 marzo 1989, ore 0100Z (mezzogiorno locale). Teatro: Curili meridionali, Mare di Okhotsk. Pace. Il pack di marzo copre il lato di Okhotsk.",
        "I Delta III della Flotta del Pacifico pattugliano il bastione di Okhotsk sotto il ghiaccio. Vogliamo sapere come: rotte, quote, velocita', e se la scorta li segue anche sotto il pack.",
        "Entrate dal Pacifico per lo Stretto di Ekaterina, trovate il Delta, seguitelo per tre quarti d'ora, poi riferite. Rientrate nel Pacifico senza farvi scoprire.",
        "Sotto la banchisa niente quota periscopica, niente antenne, niente punto nave: si riceve solo l'ELF col filo. Per trasmettere serve acqua libera: una polynya larga, guardata al periscopio prima di salire.",
        "Il pack di primo anno ha chiglie fino a una ventina di metri: sotto i 30 m usate il sondatore verso l'alto e il BQS-15. Il Los Angeles non sfonda il ghiaccio.",
        "Regole d'ingaggio: tempo di pace. Armi trattenute; fuoco solo per salvare il battello. Tempo a disposizione: 4 ore.",
        "Nota: il bastione di Okhotsk e le operazioni americane in quel mare sono reali; questo episodio e' inedito, unita' e nomi di fantasia."};
    s.briefing[1] = {
        "MISSION ORDERS", "FM: COMSUBPAC", "TO: USS LOS ANGELES (SSN-688)", "SUBJ: SSBN TRAIL - SEA OF OKHOTSK",
        "14 March 1989, 0100Z (local noon). Theatre: southern Kurils, Sea of Okhotsk. Peace. The March pack covers the Okhotsk side.",
        "The Pacific Fleet's Delta IIIs patrol the Okhotsk bastion under the ice. We want to know how: courses, depths, speeds, and whether the escort follows them under the pack.",
        "Go in from the Pacific through Ekaterina Strait, find the Delta, trail her for three quarters of an hour, then report. Return to the Pacific undetected.",
        "Under the ice there is no periscope depth, no mast, no fix: only ELF on the wire. To transmit you need open water: a wide polynya, looked at through the periscope before coming up.",
        "First-year pack has keels down to about twenty metres: above 30 m use the upward sounder and the BQS-15. Los Angeles cannot break through the ice.",
        "Rules of engagement: peacetime. Weapons hold; fire only to save the boat. Time available: 4 hours.",
        "Note: the Okhotsk bastion and American operations in that sea are real; this episode is original, units and names are fictional."};
    return s;
}


// LIB-S15 «L'ancoraggio» (docs/17 §2f, approvata il 30/10/2026): di notte, in crisi, il 688 guarda l'ancoraggio della
// squadra sovietica sul bassofondo al largo di Otranto (al posto del «punto» di Citera della 5a Eskadra, 1973), a piu' di
// 12 miglia dalla costa. Le navi ferme mostrano da sole i fanali di fonda (luci::stato_predefinito sotto mezzo nodo);
// a un'ora la bomba a mano in acqua (E11: «come per comunicare con un nostro sommergibile») e l'ancoraggio salpa, coi
// fanali di moto. Una Krivak buia di guardia, a 6 nodi col Vega calato (a bassa velocita' il pesce pende ed e' profondo).
// Se una nave classifica il 688 (almeno POSSUB) o lo tiene per 5 minuti, l'allarme: i sonar in attivo, la Kara e la
// Krivak di guardia gli danno la caccia col sonar attivo, senza sparare (R_nave: in crisi le ROE dell'IA non liberano
// le armi contro gli americani, missione_forze.cpp), e «occulto» fallisce. Il rapporto «contatto» dopo la partenza, poi il largo: esito 40.
// Semplificazioni (docs/17): il Charlie II e' affiancato a 300 m dalla nave appoggio (la base galleggiante Ugra, del kit),
// in superficie.
ScenarioMissione ancoraggio(const DatiScenario& d) {
    const Fondale f = Fondale::carica(DATI + "/teatri/otranto.fdl");
    ScenarioMissione s;
    base(s, d, f);
    s.id = "LIB_S15_ANCORAGGIO";
    s.titolo = {"L'ancoraggio", "The Anchorage"};
    s.teatro = "otranto";
    s.giorno = 26;
    s.mese = 10;
    s.anno = 1989;
    s.ora_utc = 23.0;
    s.situazione = "crisi";
    s.paranoia = 2;
    s.difficolta = 4;
    s.durata_s = 3.0 * 3600.0;
    carico_688(s);
    MeteoFissoScenario m;   // una notte serena, mare quasi calmo
    m.stato_mare = 1.5, m.vento_m_s = 3.0, m.vento_da_gradi = 320.0, m.visibilita_m = 18000.0, m.nubi_ottavi = 1.0;
    s.meteo_fisso = m;
    // il bassofondo a circa 20 miglia dalla costa, fra 60 e 120 m d'acqua
    const Vec2 ancora = acqua_fra(f, f.proiettata({41.20, 19.00}), 70.0, 110.0);
    const Vec2 largo = acqua(f, polare(ancora, 235.0, 16000.0), 300.0);
    s.unita.push_back(los_angeles(largo, rilevamento(largo, ancora), 60.0));
    s.unita.front().velocita = 5.0;
    const auto ferma = [&](int id, const char* classe, double ril, double dist, const char* ruolo, bool civile = false) {
        UnitaScenario u = unita(id, classe, "nave", "urss", acqua_fra(f, polare(ancora, ril, dist), 60.0, 130.0), 340.0, 0.0, "", civile);
        u.ruolo = ruolo;
        return u;
    };
    s.unita.push_back(ferma(1, "kara", 0.0, 0.0, "incrociatore, la nave di bandiera"));
    s.unita.push_back(ferma(2, "krivak2", 90.0, 900.0, "fregata all'ancora"));
    s.unita.push_back(ferma(3, "ugra", 200.0, 1000.0, "base galleggiante dei sommergibili (Progetto 1886)"));
    s.unita.push_back(ferma(4, "petroliera", 300.0, 1100.0, "petroliera di squadra", true));
    s.unita.push_back(ferma(5, "agi", 30.0, 1300.0, "nave spia"));
    // il Charlie II in superficie, affiancato alla nave appoggio
    UnitaScenario charlie = unita(6, "charlie2", "sottomarino", "urss", polare(s.unita[3].pos, 90.0, 300.0), 340.0, 0.0, "");
    charlie.quota = 0.0;
    charlie.ruolo = "sottomarino lanciamissili affiancato all'appoggio";
    s.unita.push_back(charlie);
    // la guardia: una Krivak buia che gira a 3 km col Vega calato
    UnitaScenario guardia = unita(7, "krivak2", "nave", "urss", acqua_fra(f, polare(ancora, 180.0, 3000.0), 60.0, 300.0), 90.0, 6.0, "");
    for (int q = 0; q < 6; ++q) guardia.punti.push_back(acqua_fra(f, polare(ancora, 240.0 + 60.0 * q, 3000.0), 60.0, 300.0));
    guardia.ciclica = true;
    guardia.luci = "spente";
    guardia.ruolo = "fregata di guardia a luci spente";
    s.unita.push_back(guardia);
    // l'Il-38 dopo la partenza, senza armi (pace per l'ASW aereo): cerca e tiene
    UnitaScenario il38 = unita(8, "il38", "aereo", "urss", polare(ancora, 90.0, 40000.0), 270.0, 250.0, "R_aereo");
    il38.entra_con = "salpano";
    il38.ruolo = "pattugliatore della squadra";
    s.unita.push_back(il38);
    AereoAswScenario a;
    a.unita = 8;
    a.situazione = "pace";
    a.contro_giocatore = true;
    a.zona = ancora;
    a.raggio = 15000.0;
    s.aerei_asw.push_back(a);
    zona(s, "largo", {"il largo, a sud-ovest", "the open sea, to the south-west"}, largo, 5000.0);
    // la svolta: la bomba a mano e la partenza; i fanali di fonda diventano di moto da soli
    regola(s, "bomba", {quando(C::tempo, 3600.0)},
           {imposta(1, "=", 1), [] { AzioneScenario r = azione(A::rumore, 2); return r; }(),
            [] {
                AzioneScenario msg = azione(A::messaggio);
                msg.testo = {"Sonar: un colpo secco vicino all'ancoraggio, piccolo. Come una bomba a mano in acqua.",
                           "Sonar: a sharp, small bang near the anchorage. Like a hand grenade in the water."};
                return msg;
            }()});
    regola(s, "salpano", {quando(C::tempo, 3720.0)},
           {vai(1, acqua(f, polare(ancora, 200.0, 40000.0), 200.0), 14.0), vai(2, acqua(f, polare(ancora, 170.0, 40000.0), 200.0), 14.0),
            vai(4, acqua(f, polare(ancora, 250.0, 30000.0), 200.0), 10.0), vai(5, acqua(f, polare(ancora, 215.0, 35000.0), 200.0), 11.0),
            [] { AzioneScenario q = azione(A::quota, 6); q.valore = 60; return q; }(), vai(6, acqua(f, polare(ancora, 225.0, 40000.0), 300.0), 12.0),
            azione(A::entra, 8)});
    regola(s, "salpa_appoggio", {quando(C::tempo, 4200.0)}, {vai(3, acqua(f, polare(ancora, 160.0, 30000.0), 200.0), 10.0)});
    // la copertura (come la S10): la variabile 5 a 1 alla partenza; una nave che classifica il 688 da' l'allarme
    regola(s, "copertura", {quando(C::tempo, 0.0)}, {imposta(5, "=", 1)});
    const auto caccia = [](int id) { AzioneScenario c = azione(A::comportamento, id); c.comportamento = "R_nave"; return c; };
    const AzioneScenario pedina_kara = caccia(1), pedina_krivak = caccia(2), pedina_guardia = caccia(7);
    const AzioneScenario scoperti = [] {
        AzioneScenario msg = azione(A::messaggio);
        msg.testo = {"Sonar: impulsi attivi della squadra, su di noi. Ci hanno trovato.", "Sonar: active pings from the squadron, on us. They have found us."};
        return msg;
    }();
    for (int id : {1, 2, 7}) {   // classificato, o tenuto 5 minuti con un contatto utilizzabile; l'allarme una volta sola
        regola(s, "classificati_" + std::to_string(id), {su(C::unita_classifica, id, 1.0), variabile(5, "=", 1.0)},
               {imposta(5, "=", 0), azione(A::allarme), pedina_kara, pedina_krivak, pedina_guardia, scoperti});
        regola(s, "tenuti_" + std::to_string(id), {su(C::unita_vede, id, 5.0), variabile(5, "=", 1.0)},
               {imposta(5, "=", 0), azione(A::allarme), pedina_kara, pedina_krivak, pedina_guardia, scoperti});
    }
    CondizioneScenario trasmesso = quando(C::giocatore_ha_trasmesso, 3600.0);
    trasmesso.trasmissione = "contatto";
    regola(s, "rapporto", {variabile(1, "=", 1.0), trasmesso}, {imposta(2, "=", 1)});
    regola(s, "al_largo", {variabile(2, "=", 1.0), in_zona("largo")},
           {imposta(3, "=", 1), fine(40, {"Il rapporto e' alla Sesta Flotta: la squadra ha salpato, e il Los Angeles e' al largo.",
                                          "The report is with the Sixth Fleet: the squadron has sailed, and Los Angeles is clear."})});
    regola(s, "collisione", {evento("collisione")},
           {fine(41, {"Collisione nel buio, in mezzo alla squadra: la crisi diventa un'altra cosa.",
                      "Collision in the dark, among the squadron: the crisis becomes something else."}, "fallimento")});
    ObiettivoScenario identifica = obiettivo("identifica", "identificare", {1, 2, 3, 4, 5, 6},
                                             {"Identificare a vista almeno 4 unita' dell'ancoraggio", "Identify by sight at least 4 units at the anchorage"});
    identifica.minimo = 4;
    s.obiettivi.push_back(identifica);
    ObiettivoScenario rapporto = obiettivo_variabile("rapporto", 2, 1, {"Riferire la partenza della squadra (rapporto di contatto)",
                                                                       "Report the squadron's departure (contact report)"});
    s.obiettivi.push_back(rapporto);
    s.obiettivi.push_back(obiettivo_variabile("occulto", 5, 1, {"Non farsi classificare ne' tenere dalla squadra", "Avoid being classified or held by the squadron"}));
    ObiettivoScenario telemetro = obiettivo("telemetro", "telemetro", {}, {"Tre distanze col telemetro entro il 15%", "Three stadimeter ranges within 15%"}, false);
    telemetro.minimo = 3;
    telemetro.errore = 0.15;
    s.obiettivi.push_back(telemetro);
    ObiettivoScenario rotta = obiettivo("rotta", "rotta_stimata", {}, {"Le rotte di tre navi che salpano, dai fanali, entro 20 gradi",
                                                                       "The courses of three departing ships, from their lights, within 20 degrees"}, false);
    rotta.minimo = 3;
    rotta.errore = 20.0;
    s.obiettivi.push_back(rotta);
    s.obiettivi.push_back(vivo());
    RadioScenario r;
    r.ordini = OrdiniScenario{"CTF 69", "USS LOS ANGELES (SSN 688)", "SOVIET ANCHORAGE SURVEILLANCE", "SECRET"};
    r.roe = {{"sottomarini", "trattenuto"}, {"navi", "trattenuto"}, {"mercantili", "trattenuto"}, {"neutrali", "trattenuto"}, {"aerei", "trattenuto"}};
    s.radio = r;
    s.briefing[0] = {
        "ORDINI DI MISSIONE", "DA: CTF 69", "A: USS LOS ANGELES (SSN-688)", "OGG.: SORVEGLIANZA DELL'ANCORAGGIO SOVIETICO",
        "26 ottobre 1989, ore 2300Z. Teatro: Canale d'Otranto, sul bassofondo a 20 miglia dalla costa, in acque internazionali. Crisi. Notte serena, mare quasi calmo.",
        "Due flotte a distanza di tiro, e nessuno spara. Stanotte la squadra sovietica e' all'ancora: la Sesta Flotta vuole sapere chi c'e' e, soprattutto, se qualcuno salpa.",
        "All'ancoraggio, coi fanali di fonda: un incrociatore Kara, una Krivak II, la base galleggiante Ugra con un Charlie II affiancato, una petroliera, la nave spia. In giro, a luci spente, una seconda Krivak col sonar a profondita' variabile calato: lo strato non vi nasconde.",
        "Riconoscere a vista almeno quattro unita'. Se la squadra salpa, riferire con un rapporto di contatto e portarsi al largo. Col telemetro, sul fanale d'albero, tre distanze; dai fanali di moto, la rotta di chi salpa.",
        "Non farsi classificare ne' tenere: se la squadra vi trova, la crisi sale. Regole d'ingaggio: armi trattenute, fuoco solo se attaccati.",
        "Tempo a disposizione: 3 ore.",
        "Nota storica: ispirata alla crisi navale dell'ottobre 1973 fra la 5a Eskadra e la Sesta Flotta; l'ancoraggio sta al posto del «punto» di Citera. Nomi e unita' di fantasia."};
    s.briefing[1] = {
        "MISSION ORDERS", "FM: CTF 69", "TO: USS LOS ANGELES (SSN-688)", "SUBJ: SOVIET ANCHORAGE SURVEILLANCE",
        "26 October 1989, 2300Z. Theatre: Strait of Otranto, on the bank 20 miles offshore, in international waters. Crisis. Clear night, near-calm sea.",
        "Two fleets within firing range, and nobody shoots. Tonight the Soviet squadron is at anchor: the Sixth Fleet wants to know who is there and, above all, whether anyone sails.",
        "At the anchorage, showing anchor lights: a Kara cruiser, a Krivak II, the Ugra submarine tender with a Charlie II alongside, a tanker, the spy ship. Patrolling darkened, a second Krivak with her variable-depth sonar down: the layer will not hide you.",
        "Identify at least four units by sight. If the squadron sails, send a contact report and head for the open sea. With the stadimeter, on the masthead light, three ranges; from the running lights, the course of those sailing.",
        "Avoid being classified or held: if the squadron finds you, the crisis escalates. Rules of engagement: weapons tight, fire only if fired upon.",
        "Time available: 3 hours.",
        "Historical note: inspired by the October 1973 naval standoff between the 5th Eskadra and the Sixth Fleet; the anchorage stands in for the Kithira «point». Names and units are fictional."};
    return s;
}
// LIB-S14 «Il pedinatore in attesa» (docs/17 §2f, approvata il 30/10/2026): di notte l'Alfa aspetta, fermo a quota
// periscopica, l'SSBN americano che esce in pattugliamento. La Piattaforma Celtica non ha costa: l'uscita e' la rotta in
// superficie fino al punto d'immersione sul ciglio della piattaforma, sull'isobata delle 100 braccia (183 m, la pratica
// americana), al posto del Clyde e di Holy Loch. L'SSBN (la classe ohio, al posto di un Lafayette) entra a 25 minuti con la
// Knox davanti (SQS-35 sotto lo strato, SQR-18A, l'SH-2F): coi fanali e la luce ambra dei sottomarini americani, fra il
// traffico illuminato. Al punto si immerge (l'azione «quota») e accosta verso il largo. La firma si registra solo dopo
// l'immersione e solo entro 5 km (la condizione «giocatore vicino a», m688.distanza_da): la variabile 4 conta 10 s a ogni
// controllo, alla mezz'ora la missione e' compiuta. Se la Knox classifica l'Alfa (almeno POSSUB), getta
// una granata di segnalazione (l'azione «rumore»), da' l'allarme e la registrazione non conta piu' (variabile 6); la
// collisione del fatto vero chiude male. Se la Knox lo tiene 10 minuti (difficolta' 2), lo stesso.
ScenarioMissione pedinatore_in_attesa(const DatiScenario& d) {
    const Fondale f = Fondale::carica(DATI + "/teatri/celtica.fdl");
    ScenarioMissione s;
    base(s, d, f);
    s.id = "LIB_S14_PEDINATORE_IN_ATTESA";
    s.titolo = {"Il pedinatore in attesa", "The Waiting Trailer"};
    s.teatro = "celtica";
    s.alfa = true;
    s.giorno = 3;
    s.mese = 11;
    s.anno = 1989;
    s.ora_utc = 21.5;
    s.situazione = "pace";
    s.paranoia = 2;
    s.difficolta = 3;   // la Knox sente l'Alfa in moto a 10 km: l'attesa va fatta piano e lontano dalla sua rotta
    s.durata_s = 2.5 * 3600.0;
    carico_alfa(s);
    MeteoFissoScenario m;   // una notte d'autunno, foschia leggera
    m.stato_mare = 2.0, m.vento_m_s = 6.0, m.vento_da_gradi = 225.0, m.visibilita_m = 12000.0, m.nubi_ottavi = 4.0;
    s.meteo_fisso = m;
    // il punto d'immersione sul ciglio, la rotta in superficie da nord-est, il largo a sud-ovest
    const Vec2 immersione = acqua(f, f.proiettata({48.40, -9.45}), 183.0);
    const Vec2 entrata = acqua(f, polare(immersione, 60.0, 18000.0), 100.0);
    const Vec2 largo = acqua(f, polare(immersione, 240.0, 40000.0), 300.0);
    const double rotta = rilevamento(entrata, immersione);
    // l'attesa: 6 km di lato dalla rotta e 9 km prima del punto, fuori dalla portata del VDS della Knox che passa; per
    // registrare bisogna poi chiudere a 5 km dall'SSBN immerso
    UnitaScenario g = alfa(acqua(f, polare(polare(immersione, rotta + 180.0, 9000.0), rotta + 90.0, 6000.0), 100.0), rotta - 90.0);
    g.quota = 12.0;   // a quota periscopica, quasi fermo
    g.velocita = 2.0;
    s.unita.push_back(g);
    UnitaScenario ssbn = unita(1, "ohio", "sottomarino", "usa", entrata, rotta, 12.0, "");
    ssbn.quota = 0.0;   // in superficie fino al punto d'immersione
    ssbn.punti = {immersione, largo};
    ssbn.entra_con = "esce";
    ssbn.ruolo = "SSBN in partenza per il pattugliamento";
    s.unita.push_back(ssbn);
    UnitaScenario knox = unita(2, "knox", "nave", "usa", polare(entrata, rotta, 2000.0), rotta, 12.0, "R_nave");
    knox.punti = {polare(immersione, rotta, 2000.0)};
    for (int q = 0; q < 4; ++q) knox.punti.push_back(acqua(f, polare(immersione, 90.0 * q + 45.0, 3000.0), 150.0));
    knox.ciclica = true;
    knox.entra_con = "esce";
    knox.ruolo = "scorta: pulisce l'uscita col VDS";
    s.unita.push_back(knox);
    UnitaScenario sh2f = unita(3, "sh2f", "aereo", "usa", polare(entrata, rotta, 4000.0), rotta, 100.0, "R_aereo");
    sh2f.entra_con = "esce";
    sh2f.ruolo = "SH-2F della Knox";
    s.unita.push_back(sh2f);
    AereoAswScenario a;
    a.unita = 3;
    a.base_unita = 2;
    a.situazione = "pace";
    a.contro_giocatore = true;
    a.zona = immersione;
    a.raggio = 8000.0;
    s.aerei_asw.push_back(a);
    // il traffico della notte: un traghetto che taglia la rotta, quattro pescherecci illuminati, un mercantile
    UnitaScenario traghetto = unita(4, "traghetto", "nave", "neutrale", acqua(f, polare(immersione, rotta + 180.0, 12000.0), 100.0), 350.0, 18.0, "", true);
    traghetto.punti = {acqua(f, polare(traghetto.pos, 350.0, 60000.0), 100.0)};
    traghetto.ruolo = "traghetto di linea";
    s.unita.push_back(traghetto);
    for (int k = 0; k < 4; ++k) {
        UnitaScenario p = unita(5 + k, "peschereccio", "nave", "neutrale",
                                acqua(f, polare(polare(immersione, rotta + 180.0, 3000.0 + 3500.0 * k), rotta + (k % 2 ? 90.0 : -90.0), 1500.0 + 800.0 * k), 100.0),
                                std::fmod(30.0 + 85.0 * k, 360.0), 3.0, "", true);
        p.ruolo = "peschereccio";
        s.unita.push_back(p);
    }
    UnitaScenario merc = unita(9, "mercantile", "nave", "neutrale", acqua(f, polare(immersione, 330.0, 15000.0), 100.0), 120.0, 12.0, "", true);
    merc.punti = {acqua(f, polare(merc.pos, 120.0, 60000.0), 100.0)};
    merc.ruolo = "mercantile verso la Manica";
    s.unita.push_back(merc);
    zona(s, "immersione", {"il punto d'immersione", "the diving point"}, immersione, 1500.0);
    regola(s, "esce", {quando(C::tempo, 1500.0)}, {azione(A::entra, 1), azione(A::entra, 2), azione(A::entra, 3)});
    CondizioneScenario al_punto = quando(C::unita_in_zona, 0.0);
    al_punto.unita = 1;
    al_punto.zona = "immersione";
    regola(s, "immersione", {al_punto}, {[] { AzioneScenario q = azione(A::quota, 1); q.valore = 120; return q; }()});
    // la firma: solo dopo l'immersione, entro 5 km, se la Knox non ha bruciato la copertura
    CondizioneScenario immersa = quando(C::regola_scattata);
    immersa.regola = "immersione";
    regola(s, "registra", {immersa, su(C::giocatore_vicino_a, 1, 5000.0), variabile(6, "=", 0.0)}, {imposta(4, "+", 10)}, false);
    regola(s, "copertura", {quando(C::tempo, 0.0)}, {imposta(5, "=", 1)});
    const std::vector<AzioneScenario> bruciati = {
        imposta(5, "=", 0), imposta(6, "=", 1), [] { AzioneScenario r = azione(A::rumore, 2); return r; }(), azione(A::allarme),
        [] {
            AzioneScenario msg = azione(A::messaggio);
            msg.testo = {"Sonar: una granata di segnalazione dalla fregata, e il suo sonar in attivo. Ci ha trovati: la firma non vale piu'.",
                         "Sonar: a signal grenade from the frigate, and her sonar goes active. She has found us: the signature is worthless."};
            return msg;
        }()};
    regola(s, "classificati", {su(C::unita_classifica, 2, 1.0), variabile(5, "=", 1.0)}, bruciati);
    regola(s, "tenuti", {su(C::unita_vede, 2, 10.0), variabile(5, "=", 1.0)}, bruciati);   // la difficolta' 2: 10 minuti
    regola(s, "registrata", {compiuto("firma")},
           {fine(40, {"La firma dell'SSBN e' sul nastro, e il punto dove si immerge con la rotta.",
                      "The SSBN's signature is on tape, with her diving point and course."})});
    regola(s, "collisione", {evento("collisione")},
           {fine(41, {"Collisione con l'SSBN che si immergeva: come nel 1974, due battelli in superficie e un incidente da spiegare.",
                      "Collision with the diving SSBN: as in 1974, two boats on the surface and an incident to explain."}, "fallimento")});
    ObiettivoScenario identifica = obiettivo("identifica", "identificare", {1}, {"Riconoscere a vista l'SSBN fra le luci", "Identify the SSBN by sight among the lights"});
    identifica.minimo = 1;
    s.obiettivi.push_back(identifica);
    ObiettivoScenario telemetro = obiettivo("telemetro", "telemetro", {1}, {"La distanza dell'SSBN col telemetro, entro il 15%", "The SSBN's range with the stadimeter, within 15%"});
    telemetro.minimo = 1;
    telemetro.errore = 0.15;
    s.obiettivi.push_back(telemetro);
    s.obiettivi.push_back(obiettivo_variabile("firma", 4, 1800, {"Registrarlo dopo l'immersione: 30 minuti entro 5 km", "Record her after she dives: 30 minutes within 5 km"}));
    ObiettivoScenario rotta_ssbn = obiettivo("rotta", "rotta_stimata", {1}, {"La sua rotta dai fanali, entro 20 gradi", "Her course from the lights, within 20 degrees"}, false);
    rotta_ssbn.minimo = 1;
    rotta_ssbn.errore = 20.0;
    s.obiettivi.push_back(rotta_ssbn);
    s.obiettivi.push_back(obiettivo_variabile("occulto", 5, 1, {"Non farsi trovare dalla Knox", "Avoid being found by the Knox"}, false));
    s.obiettivi.push_back(vivo());
    RadioScenario r;
    r.ordini = OrdiniScenario{"Comando della Flotta del Nord", "K-463 (PROGETTO 705)", "IL PEDINATORE IN ATTESA", "SOV. SEKRETNO"};
    r.roe = {{"sottomarini", "trattenuto"}, {"navi", "trattenuto"}, {"mercantili", "trattenuto"}, {"neutrali", "trattenuto"}, {"aerei", "trattenuto"}};
    s.radio = r;
    s.briefing[0] = {
        "ORDINI DI MISSIONE", "DA: Comando della Flotta del Nord", "A: K-463 (PROGETTO 705)", "OGG.: IL PEDINATORE IN ATTESA",
        "3 novembre 1989, ore 2130Z. Teatro: Atlantico, sul ciglio della Piattaforma Celtica. Pace. Notte, foschia leggera, visibilita' 12 km.",
        "Stanotte un SSBN americano esce in pattugliamento. Viene in superficie da nord-est e si immerge sul ciglio, sull'isobata delle 100 braccia; l'ora non si sa.",
        "Aspettatelo a quota periscopica. Di notte si vedono solo luci: il traghetto, i pescherecci, i mercantili. Il sottomarino in superficie ha i fanali e una luce ambra lampeggiante. Riconoscetelo col libretto e misuratene la distanza col telemetro.",
        "Quando si immerge, agganciatelo e registratelo per mezz'ora, entro 5 km. Mosca vuole la firma, il punto e la rotta.",
        "Davanti a lui una fregata Knox pulisce l'uscita, col sonar calato sotto lo strato e l'elicottero. Se vi trova, la firma non vale piu'. Attenzione alla rotta dell'immersione: nel 1974 un battello in attesa ci fini' addosso.",
        "Regole d'ingaggio: tempo di pace. Armi trattenute.", "Tempo a disposizione: 2 ore e 30.",
        "Nota storica: ispirata al sottomarino sovietico in attesa fuori da Holy Loch e alla collisione con la James Madison (3 novembre 1974). Nomi e unita' di fantasia."};
    s.briefing[1] = {
        "MISSION ORDERS", "FM: NORTHERN FLEET HQ", "TO: K-463 (PROJECT 705)", "SUBJ: THE WAITING TRAILER",
        "3 November 1989, 2130Z. Theatre: Atlantic, on the edge of the Celtic Shelf. Peace. Night, light haze, visibility 12 km.",
        "Tonight an American SSBN sails on patrol. She comes on the surface from the north-east and dives at the shelf edge, on the 100-fathom line; the time is unknown.",
        "Wait for her at periscope depth. At night you see only lights: the ferry, the fishing boats, the merchantmen. The submarine on the surface shows running lights and a flashing amber light. Identify her with the book and range her with the stadimeter.",
        "When she dives, pick her up and record her for half an hour, within 5 km. Moscow wants the signature, the point and the course.",
        "Ahead of her a Knox frigate sweeps the exit, with her sonar below the layer and her helicopter. If she finds you, the signature is worthless. Watch the diving track: in 1974 a waiting boat ran into one.",
        "Rules of engagement: peacetime. Weapons hold.", "Time available: 2 hours 30.",
        "Historical note: inspired by the Soviet submarine waiting off Holy Loch and its collision with James Madison (3 November 1974). Names and units are fictional."};
    return s;
}

// LIB-I17 «La soglia di Ekaterina» (docs/17 §2f, approvata il 30/10/2026): la I16 dall'altra parte. Prima dell'alba
// l'Alfa (al posto di un Victor III della Flotta del Pacifico) deve ripulire la soglia di Ekaterina da un SSN americano che
// gira sotto il margine del pack, e riferire da acqua libera prima che il Delta III entri dal Pacifico. L'americano e' un
// Los Angeles: nel motore il sonar da mine e ghiaccio (BQS-15) e' dei Los Angeles, e sotto la banchisa l'IA lo tiene acceso
// (disassemblatore, 30/10/2026); il 688 nel pack di primo anno di Okhotsk lo giustifica gia' la I16. Nel canale profondo
// dello stretto c'e' lo sbarramento di PMR-2 della parte sovietica, in sicurezza in tempo di pace (non armate): l'Alfa lo
// vede sulla carta come campo noto, col varco di 2,5 km dal lato di Kunashir; l'americano non lo conosce, lo vede col BQS-15 e
// lo aggira (runtime, 30/10/2026), e passa dal varco. A 30 minuti va verso il margine dal lato del Pacifico per il suo
// rapporto; a 2 ore entra il Delta dal Pacifico, e l'americano lo cerca. Dopo il rapporto dell'Alfa arrivano l'Udaloy col
// Polinom, il Ka-27 e l'Il-38 (boe solo in acqua libera).
ScenarioMissione soglia_di_ekaterina(const DatiScenario& d) {
    const Fondale f = Fondale::carica(DATI + "/teatri/kunashir.fdl");
    ScenarioMissione s;
    base(s, d, f);
    s.id = "LIB_I17_SOGLIA_DI_EKATERINA";
    s.titolo = {"La soglia di Ekaterina", "The Ekaterina Sill"};
    s.teatro = "kunashir";
    s.alfa = true;
    s.giorno = 20;
    s.mese = 3;
    s.anno = 1989;
    s.ora_utc = 18.0;   // le 5 del mattino locali, prima dell'alba
    s.situazione = "pace";
    s.paranoia = 2;
    s.difficolta = 3;
    s.durata_s = 3.0 * 3600.0;
    s.meteo_realistico = true;   // il pack dai punti ERA5 di marzo, col margine negli stretti (come la I16)
    s.navigazione = NavigazioneScenario{0.5, true};
    carico_alfa(s);
    const Vec2 bastione = acqua(f, f.proiettata({44.85, 146.55}), 1000.0);
    const Vec2 margine_nord = acqua(f, f.proiettata({44.66, 146.82}), 300.0);   // l'uscita dello stretto verso Okhotsk
    const Vec2 pacifico = acqua(f, f.proiettata({44.25, 146.95}), 200.0);       // il margine dal lato del Pacifico
    const Vec2 varco = acqua(f, f.proiettata({44.50, 146.703}), 250.0);   // il varco, dove il canale profondo prosegue a sud
    const Vec2 sud = acqua(f, f.proiettata({44.33, 146.70}), 250.0);       // il canale a sud della soglia
    s.unita.push_back(alfa(bastione, rilevamento(bastione, margine_nord)));
    s.unita.front().quota = 100.0;
    s.unita.front().velocita = 8.0;
    // l'americano: un circuito sotto il margine, a nord dello stretto
    const Vec2 circuito = acqua(f, f.proiettata({44.60, 146.80}), 400.0);   // 11 km a nord dello sbarramento, sotto il pack
    UnitaScenario la = unita(1, "los_angeles", "sottomarino", "usa", polare(circuito, 0.0, 3000.0), 90.0, 5.0, "");
    for (int q = 0; q < 4; ++q) la.punti.push_back(acqua(f, polare(circuito, 90.0 * q + 90.0, 3000.0), 300.0));
    la.ciclica = true;
    la.quota = 200.0;   // nel canale profondo, vicino alla quota delle PMR-2 (300 m): il BQS-15 le vede da lontano
    la.parametri.paranoia = 2;
    la.ruolo = "SSN americano sotto il margine del pack";
    s.unita.push_back(la);
    // il Delta III entra dal Pacifico a 2 ore, per il varco, verso il bastione
    UnitaScenario delta = unita(2, "delta3", "sottomarino", "urss", pacifico, rilevamento(pacifico, varco), 6.0, "");
    delta.quota = 120.0;   // nel canale profondo: a 44,45 il lato di Iturup risale a 60 m
    // il canale profondo: a nord del varco piega a nord-est ed entra nel bacino
    delta.punti = {sud, varco, acqua(f, f.proiettata({44.58, 146.77}), 300.0), margine_nord, acqua(f, f.proiettata({44.75, 146.88}), 600.0), bastione};
    delta.entra_con = "delta";
    delta.ruolo = "SSBN che rientra nel bastione";
    s.unita.push_back(delta);
    // dopo il rapporto: l'Udaloy col Polinom e il Ka-27 al margine, l'Il-38 (le boe solo in acqua libera)
    UnitaScenario udaloy = unita(3, "udaloy", "nave", "urss", acqua(f, polare(pacifico, 120.0, 15000.0), 300.0), 300.0, 14.0, "R_nave");
    udaloy.punti = {acqua(f, polare(pacifico, 30.0, 4000.0), 300.0)};
    udaloy.entra_con = "rapporto";
    udaloy.ruolo = "cacciatorpediniere antisom col VDS";
    s.unita.push_back(udaloy);
    UnitaScenario ka27 = unita(4, "ka27", "aereo", "urss", polare(udaloy.pos, 0.0, 2000.0), 300.0, 100.0, "R_aereo");
    ka27.entra_con = "rapporto";
    ka27.ruolo = "Ka-27 dell'Udaloy";
    s.unita.push_back(ka27);
    UnitaScenario il38 = unita(5, "il38", "aereo", "urss", polare(pacifico, 90.0, 30000.0), 270.0, 250.0, "R_aereo");
    il38.entra_con = "rapporto";
    il38.ruolo = "pattugliatore sul margine";
    s.unita.push_back(il38);
    for (int k = 0; k < 2; ++k) {
        AereoAswScenario a;
        a.unita = k == 0 ? 4 : 5;
        a.base_unita = k == 0 ? 3 : -1;
        if (k == 1) a.base = f.proiettata({45.25, 147.90});   // Burevestnik, Iturup
        a.situazione = "pace";
        a.contro_giocatore = false;
        a.zona = margine_nord;
        a.raggio = 15000.0;
        s.aerei_asw.push_back(a);
    }
    // il rompighiaccio con la portarinfuse lungo la costa di Kunashir, coi fanali
    const Vec2 costa = acqua(f, f.proiettata({44.55, 146.15}), 100.0), nord_costa = acqua(f, f.proiettata({45.25, 146.0}), 200.0);
    UnitaScenario rompi = unita(6, "rompighiaccio", "nave", "urss", costa, rilevamento(costa, nord_costa), 8.0, "", true);
    rompi.punti = {nord_costa};
    rompi.ruolo = "rompighiaccio";
    s.unita.push_back(rompi);
    UnitaScenario carico = unita(7, "portarinfuse", "nave", "urss", polare(costa, rilevamento(nord_costa, costa), 1200.0), rilevamento(costa, nord_costa), 8.0, "", true);
    carico.punti = {nord_costa};
    carico.ruolo = "mercantile nel canale del rompighiaccio";
    s.unita.push_back(carico);
    // lo sbarramento: PMR-2 ogni 300 m sul parallelo 44,50 nel canale profondo, dove il fondo passa i 200 m; il varco di
    // 2,5 km dal lato di Kunashir, dove il canale profondo prosegue a sud (dal lato di Iturup il fondo risale a 60 m). In
    // sicurezza in tempo di pace (`sicura`, runtime 30/10/2026): ostacoli che il BQS-15 vede, non armi
    const Vec2 ovest = f.proiettata({44.50, 146.64}), est = f.proiettata({44.50, 146.87});
    const double passo = 300.0, lungo = distanza(ovest, est);   // fra due mine non si passa a 300 m da entrambe
    for (double x = 0.0; x <= lungo; x += passo) {
        const Vec2 p = polare(ovest, rilevamento(ovest, est), x);
        if (f.profondita(p) >= 200.0 && distanza(p, varco) > 1250.0) s.mine.push_back(MinaScenario{"pmr2", p, "urss", true, true});
    }
    zona(s, "sud", {"il canale a sud della soglia", "the channel south of the sill"}, sud, 2000.0);
    zona(s, "pacifico", {"il margine dal lato del Pacifico", "the ice edge on the Pacific side"}, pacifico, 6000.0);
    // la radio: la campana dell'ELF a 2 ore (riferire prima del Delta); il rapporto solo in acqua libera
    RadioScenario r;
    r.ordini = OrdiniScenario{"Comando della Flotta del Pacifico", "K-463 (PROGETTO 705)", "LA SOGLIA DI EKATERINA", "SOV. SEKRETNO"};
    r.roe = {{"sottomarini", "trattenuto"}, {"navi", "trattenuto"}, {"mercantili", "trattenuto"}, {"neutrali", "trattenuto"}, {"aerei", "trattenuto"}};
    r.messaggi.push_back(radio("campana", 7200.0, "flash", "Comando della Flotta del Pacifico", "RIFERIRE", "SOV. SEKRETNO",
                               {"Il Delta entra ora dal Pacifico. Riferire il contatto sull'americano: rapporto in trasmissione da acqua libera.",
                                "The Delta is entering from the Pacific now. Report the contact on the American: transmit from open water."}));
    s.radio = r;
    regola(s, "delta", {quando(C::tempo, 7200.0)}, {azione(A::entra, 2)});
    // l'americano va verso il margine dal lato del Pacifico, per il varco; il BQS-15 sotto la banchisa lo fa girare
    // attorno alle mine che vede
    regola(s, "verso_il_pacifico", {quando(C::tempo, 1800.0)}, {vai(1, sud, 10.0)});   // sotto il pack va a 6-7 nodi
    CondizioneScenario a_sud = quando(C::unita_in_zona, 0.0);
    a_sud.unita = 1;
    a_sud.zona = "sud";
    regola(s, "al_margine", {a_sud}, {vai(1, pacifico, 6.0)});
    CondizioneScenario trasmesso = quando(C::giocatore_ha_trasmesso, 0.0);
    trasmesso.trasmissione = "contatto";
    regola(s, "rapporto", {compiuto("trovato"), trasmesso}, {imposta(2, "=", 1), azione(A::entra, 3), azione(A::entra, 4), azione(A::entra, 5)});
    regola(s, "copertura", {quando(C::tempo, 0.0)}, {imposta(5, "=", 1)});
    regola(s, "classificati", {su(C::unita_classifica, 1, 1.0), variabile(5, "=", 1.0)}, {imposta(5, "=", 0)});
    regola(s, "pulita", {compiuto("seguito"), variabile(2, "=", 1.0)},
           {fine(40, {"Il rapporto e' al comando e l'americano e' tenuto: la soglia e' pulita per il Delta.",
                      "The report is with the command and the American is held: the sill is clear for the Delta."})});
    s.obiettivi.push_back(obiettivo("trovato", "scoprire", {1}, {"Trovare e classificare il sottomarino americano", "Find and classify the American submarine"}));
    s.obiettivi.push_back(obiettivo_contatto("seguito", {1}, 30.0, {"Tenerlo: 30 minuti di contatto", "Hold him: 30 minutes of contact"}));
    ObiettivoScenario rapporto = obiettivo_variabile("rapporto", 2, 1, {"Riferire da acqua libera prima che il Delta passi il varco",
                                                                       "Report from open water before the Delta passes the gap"});
    rapporto.entro_s = 2.25 * 3600.0;
    s.obiettivi.push_back(rapporto);
    s.obiettivi.push_back(obiettivo_variabile("occulto", 5, 1, {"Non farsi classificare dall'americano", "Avoid being classified by the American"}, false));
    s.obiettivi.push_back(vivo());
    s.briefing[0] = {
        "ORDINI DI MISSIONE", "DA: Comando della Flotta del Pacifico", "A: K-463 (PROGETTO 705)", "OGG.: LA SOGLIA DI EKATERINA",
        "20 marzo 1989, ore 1800Z (le 5 del mattino). Teatro: Stretto di Ekaterina, fra Kunashir e Iturup, lato di Okhotsk. Pace. Pack di primo anno.",
        "Fra due ore un Delta III rientra nel bastione dal Pacifico, per lo stretto. Il SOSUS e un Il-38 dicono che un sottomarino americano gira da giorni sotto il margine del pack, a nord della soglia.",
        "Ripulite la soglia: trovatelo, classificatelo, tenetelo, e riferite prima che il Delta passi il varco. Sotto la banchisa arriva solo l'ELF: per trasmettere serve acqua libera, una polynya o il margine.",
        "Nel canale profondo dello stretto c'e' il nostro sbarramento di mine a razzo, in sicurezza in tempo di pace, con un varco di 2,5 km dal lato di Kunashir: e' sulla carta. L'americano non lo conosce; il suo sonar da mine lo vedra' e lo fara' girare. Sapete dove non puo' passare.",
        "Il battello e' un Alfa al posto di un Victor III della Flotta del Pacifico. Regole d'ingaggio: tempo di pace. Armi trattenute.",
        "Tempo a disposizione: 3 ore.",
        "Nota: inedita, sullo sfondo dei bastioni di Okhotsk e dei battelli americani nell'Okhotsk degli anni '70 e '80. Nomi e unita' di fantasia."};
    s.briefing[1] = {
        "MISSION ORDERS", "FM: PACIFIC FLEET HQ", "TO: K-463 (PROJECT 705)", "SUBJ: THE EKATERINA SILL",
        "20 March 1989, 1800Z (5 in the morning). Theatre: Ekaterina Strait, between Kunashir and Iturup, Okhotsk side. Peace. First-year pack.",
        "In two hours a Delta III returns to the bastion from the Pacific, through the strait. SOSUS and an Il-38 report an American submarine circling for days under the edge of the pack, north of the sill.",
        "Clear the sill: find him, classify him, hold him, and report before the Delta passes the gap. Under the ice only ELF gets through: to transmit you need open water, a polynya or the ice edge.",
        "In the deep channel of the strait lies our rocket-mine barrier, safed in peacetime, with a 2.5 km gap on the Kunashir side: it is on the chart. The American does not know it; his mine sonar will see it and turn him. You know where he cannot pass.",
        "Your boat is an Alfa standing in for a Pacific Fleet Victor III. Rules of engagement: peacetime. Weapons hold.",
        "Time available: 3 hours.",
        "Note: original, set against the Okhotsk bastions and the American boats in the Sea of Okhotsk in the 1970s and 1980s. Names and units are fictional."};
    return s;
}

// LIB-S16 «La scala» (docs/17 §2g, approvata il 30/10/2026): l'Alfa in un fiordo delle Faer Oer (al posto del
// Hardangerfjord, E12) di un paese neutrale di fantasia, in crisi: fotografare la nave appoggio in fondo e uscire. Il paese
// applica la scala d'escalation norvegese (CIMSEC): quando una fregata classifica l'Alfa (almeno POSSUB) o lo tiene 5
// minuti, il segnale di emergere; dopo 5 minuti le cariche di segnalazione, una ogni 2 minuti (l'azione «rumore»); dopo 15
// minuti l'attacco coi razzi antisom (i Terne III delle Oslo, 425-1600 m), mai coi siluri. Le fregate sono della parte
// del luogo (M7), in crisi con l'URSS: dalla scoperta passano a R_nave, vengono addosso e pingano senza sparare; all'ultimo gradino il
// luogo dichiara la guerra all'URSS (azione «situazione» fra luogo e urss) e sparano. Se l'Alfa emerge prima dell'attacco, la scala si
// ferma e le fregate lo scortano fuori (richiamato). La variabile 8 conta i secondi dalla scoperta (10 a ogni controllo);
// se le fregate lo perdono per 10 minuti prima dell'ultimo gradino, la scala ricomincia da capo.
// Le fregate sono Oslo (dal 30/10/2026, col Terne del runtime; prima Koni) e la motocannoniera la Hauk del kit.
// Semplificazioni (docs/17): lo strato d'acqua dolce del fiordo non c'e' (resta lo strato del meteo).
ScenarioMissione scala_norvegese(const DatiScenario& d) {
    const Fondale f = Fondale::carica(DATI + "/teatri/faroe.fdl");
    ScenarioMissione s;
    base(s, d, f);
    s.id = "LIB_S16_SCALA";
    s.titolo = {"La scala", "The Ladder"};
    s.teatro = "faroe";
    s.alfa = true;
    s.giorno = 27;
    s.mese = 4;
    s.anno = 1989;
    s.ora_utc = 4.0;
    s.situazione = "pace";                            // fra USA e URSS
    s.situazione_luogo = SituazioneLuogo{"pace", "crisi"};   // il paese costiero (la parte del luogo, M7) in crisi con l'URSS
    s.paranoia = 2;
    s.difficolta = 4;
    s.durata_s = 2.0 * 3600.0;
    carico_alfa(s);
    MeteoFissoScenario m;   // l'alba nel fiordo: mare calmo, nubi basse
    m.stato_mare = 1.0, m.vento_m_s = 3.0, m.vento_da_gradi = 300.0, m.visibilita_m = 6000.0, m.nubi_ottavi = 7.0;
    s.meteo_fisso = m;
    // il fiordo: la bocca a nord-ovest, il fondo a sud-est (fra 90 e 140 m)
    const Vec2 largo = acqua(f, f.proiettata({62.42, -6.95}), 80.0);
    const Vec2 bocca = acqua(f, f.proiettata({62.335, -6.855}), 80.0);
    const Vec2 gomito = acqua(f, f.proiettata({62.30, -6.80}), 100.0);
    const Vec2 fondo = acqua(f, f.proiettata({62.265, -6.752}), 90.0);
    s.unita.push_back(alfa(largo, rilevamento(largo, bocca)));
    s.unita.front().quota = 50.0;
    s.unita.front().velocita = 5.0;
    // l'installazione: una nave appoggio all'ancora in fondo al fiordo
    UnitaScenario appoggio = unita(1, "cargo", "nave", "neutrale", fondo, 330.0, 0.0, "", true);
    appoggio.ruolo = "nave appoggio all'ancora (l'installazione)";
    s.unita.push_back(appoggio);
    // le fregate: una di guardia alla bocca, una nel fiordo; la motocannoniera fra le due
    UnitaScenario guardia = unita(2, "oslo", "nave", "luogo", polare(bocca, 330.0, 1500.0), 60.0, 6.0, "");
    guardia.punti = {acqua(f, polare(bocca, 30.0, 2500.0), 80.0), acqua(f, polare(bocca, 270.0, 2500.0), 80.0)};
    guardia.ciclica = true;
    guardia.ruolo = "fregata di guardia alla bocca";
    s.unita.push_back(guardia);
    UnitaScenario interna = unita(3, "oslo", "nave", "luogo", gomito, 140.0, 5.0, "");
    interna.punti = {fondo, gomito, bocca, gomito};
    interna.ciclica = true;
    interna.ruolo = "fregata nel fiordo";
    s.unita.push_back(interna);
    UnitaScenario mtc = unita(4, "hauk", "nave", "luogo", polare(gomito, 330.0, 1500.0), 140.0, 8.0, "");
    mtc.punti = {fondo, bocca};
    mtc.ciclica = true;
    mtc.ruolo = "motocannoniera";
    s.unita.push_back(mtc);
    UnitaScenario p3 = unita(5, "p3c", "aereo", "luogo", polare(largo, 0.0, 30000.0), 180.0, 250.0, "R_aereo");
    p3.ruolo = "pattugliatore del paese costiero";
    s.unita.push_back(p3);
    AereoAswScenario a;
    a.unita = 5;
    a.situazione = "pace";   // cerca e tiene, senza armi
    a.contro_giocatore = true;
    a.zona = bocca;
    a.raggio = 10000.0;
    s.aerei_asw.push_back(a);
    UnitaScenario traghetto = unita(6, "traghetto", "nave", "neutrale", polare(gomito, 60.0, 600.0), 240.0, 10.0, "", true);
    traghetto.punti = {polare(gomito, 240.0, 600.0), polare(gomito, 60.0, 600.0)};
    traghetto.ciclica = true;
    traghetto.ruolo = "traghetto del fiordo";
    s.unita.push_back(traghetto);
    for (int k = 0; k < 2; ++k) {
        UnitaScenario p = unita(7 + k, "peschereccio", "nave", "neutrale", acqua(f, polare(bocca, 300.0 + 60.0 * k, 4000.0), 60.0),
                                90.0 * k, 3.0, "", true);
        p.ruolo = "peschereccio costiero";
        s.unita.push_back(p);
    }
    zona(s, "largo", {"fuori dal fiordo", "outside the fjord"}, largo, 3000.0);
    // la scala d'escalation
    regola(s, "copertura", {quando(C::tempo, 0.0)}, {imposta(5, "=", 1), imposta(10, "=", 1)});
    const auto caccia = [](int id) { AzioneScenario c = azione(A::comportamento, id); c.comportamento = "R_nave"; return c; };
    const std::vector<AzioneScenario> scoperti = {   // le fregate vengono addosso e pingano (in crisi le ROE non liberano le armi)
        imposta(5, "=", 0), imposta(7, "=", 1), azione(A::allarme), caccia(2), caccia(3), [] {
            AzioneScenario msg = azione(A::messaggio);
            msg.testo = {"Radio, sulla frequenza di soccorso: «Sottomarino in immersione nel fiordo: emergete e identificatevi.»",
                         "Radio, on the distress frequency: \"Submerged submarine in the fjord: surface and identify yourself.\""};
            return msg;
        }()};
    for (int id : {2, 3}) {
        regola(s, "classificati_" + std::to_string(id), {su(C::unita_classifica, id, 1.0), variabile(7, "=", 0.0)}, scoperti);
        regola(s, "tenuti_" + std::to_string(id), {su(C::unita_vede, id, 5.0), variabile(7, "=", 0.0)}, scoperti);
    }
    regola(s, "orologio", {variabile(7, "=", 1.0)}, {imposta(8, "+", 10)}, false);
    // perso il contatto (nessuna delle due fregate lo sente da 10 minuti) prima dell'ultimo gradino, la scala si ferma e
    // ricomincia da capo alla prossima scoperta
    regola(s, "perso", {variabile(7, "=", 1.0), su(C::unita_non_vede, 2, 10.0), su(C::unita_non_vede, 3, 10.0)},
           {imposta(7, "=", 0), imposta(8, "=", 0), [] {
                AzioneScenario msg = azione(A::messaggio);
                msg.testo = {"Sonar: le fregate rallentano e allargano. Ci hanno perso.", "Sonar: the frigates slow and open out. They have lost us."};
                return msg;
            }()}, false);
    for (int k = 0; k < 5; ++k)   // le cariche di segnalazione: da 5 minuti, ogni 2
        regola(s, "segnalazione_" + std::to_string(k + 1), {variabile(7, "=", 1.0), variabile(8, ">", 290.0 + 120.0 * k)},
               {[k] { AzioneScenario r = azione(A::rumore, k % 2 ? 3 : 2); return r; }(), [] {
                    AzioneScenario msg = azione(A::messaggio);
                    msg.testo = {"Sonar: una carica di segnalazione, vicina. Ci dicono di salire.", "Sonar: a signal charge, close. They are telling us to come up."};
                    return msg;
                }()});
    AzioneScenario guerra = azione(A::situazione);   // l'ultimo gradino: il luogo in guerra con l'URSS, le fregate sparano
    guerra.comportamento = "guerra";
    guerra.fra_a = "luogo";
    guerra.fra_b = "urss";
    regola(s, "attacco", {variabile(7, "=", 1.0), variabile(8, ">", 890.0)},
           {imposta(7, "=", 2), imposta(10, "=", 0), guerra, [] {
                AzioneScenario msg = azione(A::messaggio);
                msg.testo = {"Radio: «Ultimo avvertimento.» Le fregate attaccano coi razzi antisom.", "Radio: \"Final warning.\" The frigates attack with ASW rockets."};
                return msg;
            }()});
    regola(s, "emersi", {variabile(7, "=", 1.0), quando(C::giocatore_in_superficie)},
           {fine(43, {"Emersi e identificati: le fregate vi scortano fuori dalle acque territoriali. Il battello e' salvo, la missione no.",
                      "Surfaced and identified: the frigates escort you out of territorial waters. The boat is safe, the mission is not."}, "richiamato")});
    regola(s, "fuori", {compiuto("foto"), in_zona("largo")},
           {imposta(3, "=", 1), fine(40, {"Le foto dell'installazione sono a bordo, e l'Alfa e' fuori dal fiordo.",
                                          "The photographs of the installation are aboard, and the Alfa is out of the fjord."})});
    ObiettivoScenario foto = obiettivo("foto", "foto", {1}, {"Fotografare l'installazione in fondo al fiordo, entro 2 km", "Photograph the installation at the head of the fjord, within 2 km"});
    foto.distanza_m = 2000.0;
    s.obiettivi.push_back(foto);
    s.obiettivi.push_back(obiettivo_variabile("uscita", 3, 1, {"Uscire dal fiordo", "Leave the fjord"}));
    s.obiettivi.push_back(obiettivo_variabile("occulto", 5, 1, {"Non farsi classificare ne' tenere", "Avoid being classified or held"}, false));
    s.obiettivi.push_back(obiettivo_variabile("scala", 10, 1, {"Non arrivare all'ultimo gradino della scala", "Do not reach the last rung of the ladder"}, false));
    s.obiettivi.push_back(vivo());
    RadioScenario r;
    r.ordini = OrdiniScenario{"Comando della Flotta del Nord", "K-463 (PROGETTO 705)", "LA SCALA", "SOV. SEKRETNO"};
    r.roe = {{"sottomarini", "trattenuto"}, {"navi", "trattenuto"}, {"mercantili", "trattenuto"}, {"neutrali", "trattenuto"}, {"aerei", "trattenuto"}};
    s.radio = r;
    s.briefing[0] = {
        "ORDINI DI MISSIONE", "DA: Comando della Flotta del Nord", "A: K-463 (PROGETTO 705)", "OGG.: LA SCALA",
        "27 aprile 1989, ore 0400Z, alba. Teatro: un fiordo di un paese neutrale nel Nord Atlantico. Crisi. Mare calmo, nubi basse, visibilita' 6 km.",
        "In fondo al fiordo e' all'ancora una nave appoggio che il comando vuole vedere. Entrate, fotografatela da meno di 2 km e uscite.",
        "Il paese costiero ha una scala d'escalation, e la applica: se vi trova, prima il segnale di emergere; dopo cinque minuti le cariche di segnalazione, una ogni due minuti; dopo un quarto d'ora l'attacco coi razzi antisom. Siluri mai.",
        "Se emergete, la scala si ferma e vi scortano fuori: il battello e' salvo, la missione no. Nel fiordo l'acqua e' stretta e le pareti sono di roccia.",
        "Regole d'ingaggio: armi trattenute. Non rispondete al fuoco.", "Tempo a disposizione: 2 ore.",
        "Nota storica: ispirata agli incidenti nei fiordi norvegesi (Hardangerfjord, aprile 1983) e alla scala d'escalation adottata dopo. Chi fossero gli intrusi resta conteso. Paese, nomi e unita' di fantasia."};
    s.briefing[1] = {
        "MISSION ORDERS", "FM: NORTHERN FLEET HQ", "TO: K-463 (PROJECT 705)", "SUBJ: THE LADDER",
        "27 April 1989, 0400Z, dawn. Theatre: a fjord of a neutral country in the North Atlantic. Crisis. Calm sea, low cloud, visibility 6 km.",
        "At the head of the fjord a support ship lies at anchor that the command wants to see. Go in, photograph her from within 2 km and come out.",
        "The coastal state has an escalation ladder, and applies it: if they find you, first the signal to surface; after five minutes signal charges, one every two minutes; after a quarter of an hour an attack with ASW rockets. Never torpedoes.",
        "If you surface, the ladder stops and they escort you out: the boat is safe, the mission is not. In the fjord the water is narrow and the walls are rock.",
        "Rules of engagement: weapons hold. Do not return fire.", "Time available: 2 hours.",
        "Historical note: inspired by the incidents in the Norwegian fjords (Hardangerfjord, April 1983) and the escalation ladder adopted afterwards. Who the intruders were remains disputed. Country, names and units are fictional."};
    return s;
}

// LIB-S17 «Sotto la prora» (docs/17 §2g, approvata il 30/10/2026): E20, K-314 e la Kitty Hawk (21 marzo 1984, Mar del
// Giappone, Team Spirit 84). Il Mar del Giappone nel gioco non c'e': il lato Pacifico delle Curili, nell'angolo
// d'oceano profondo a sud-est del teatro [SOST]. Di notte, col mare del teatro di marzo, l'Alfa (al posto del Victor I)
// ha perso il gruppo portaerei che pedinava. Il gruppo rientra a 20 minuti dal bordo del teatro, zig-zagando (a 15 nodi la
// portaerei si sente da 20 miglia: ritrovarla e' facile, restare nascosti no); a 40 minuti accosta di colpo verso il
// punto dell'Alfa a 15 nodi. Bisogna ritrovarlo, tenere la portaerei un'ora e riferire da quota periscopica, senza
// passarle davanti a meno di 1000 m (la condizione «giocatore vicino a»): se ci si trova sulla sua prora, la collisione
// del 1984 chiude la missione.
ScenarioMissione sotto_la_prora(const DatiScenario& d) {
    const Fondale f = Fondale::carica(DATI + "/teatri/kunashir.fdl");
    ScenarioMissione s;
    base(s, d, f);
    s.id = "LIB_S17_SOTTO_LA_PRORA";
    s.titolo = {"Sotto la prora", "Under the Bow"};
    s.teatro = "kunashir";
    s.alfa = true;
    s.giorno = 21;
    s.mese = 3;
    s.anno = 1989;
    s.ora_utc = 11.0;   // le 22 locali, l'ora del fatto
    s.situazione = "pace";
    s.paranoia = 2;
    s.difficolta = 4;
    s.durata_s = 3.0 * 3600.0;
    s.meteo_realistico = true;   // il mare di marzo sul lato Pacifico: e' il maltempo che fece perdere il contatto
    carico_alfa(s);
    const Vec2 partenza = acqua(f, f.proiettata({43.65, 147.90}), 2000.0);
    const Vec2 gruppo = acqua(f, f.proiettata({43.92, 148.28}), 2000.0);
    s.unita.push_back(alfa(partenza, 45.0));
    s.unita.front().quota = 100.0;
    s.unita.front().velocita = 8.0;
    // lo zig-zag del gruppo verso sud, gambe di 8 km a 150 e 210 gradi; le scorte con lo stesso schema, spostate
    std::vector<Vec2> gambe = {gruppo};
    for (int k = 0; k < 6; ++k) gambe.push_back(acqua(f, polare(gambe.back(), k % 2 ? 210.0 : 150.0, 8000.0), 1500.0));
    const auto in_formazione = [&](int id, const char* classe, double ril, double dist, const char* ruolo) {
        UnitaScenario u = unita(id, classe, "nave", "usa", polare(gruppo, ril, dist), 150.0, 15.0, "");
        for (std::size_t k = 1; k < gambe.size(); ++k) u.punti.push_back(polare(gambe[k], ril, dist));
        u.entra_con = "ritorno";   // il gruppo si era allontanato: rientra dal bordo del teatro a 20 minuti
        u.ruolo = ruolo;
        return u;
    };
    s.unita.push_back(in_formazione(1, "kitty_hawk", 0.0, 0.0, "portaerei della classe Kitty Hawk"));
    s.unita.push_back(in_formazione(2, "knox", 180.0, 3000.0, "fregata di scorta (come la Harold E. Holt)"));
    s.unita.push_back(in_formazione(3, "spruance", 90.0, 5000.0, "cacciatorpediniere con la schiera"));
    UnitaScenario s3 = unita(4, "s3b", "aereo", "usa", polare(gruppo, 270.0, 10000.0), 180.0, 300.0, "R_aereo");
    s3.entra_con = "ritorno";
    s3.ruolo = "S-3B della portaerei";
    s.unita.push_back(s3);
    UnitaScenario sh2f = unita(5, "sh2f", "aereo", "usa", polare(gruppo, 180.0, 4000.0), 180.0, 100.0, "R_aereo");
    sh2f.entra_con = "ritorno";
    sh2f.ruolo = "SH-2F della Knox";
    s.unita.push_back(sh2f);
    for (int k = 0; k < 2; ++k) {
        AereoAswScenario a;
        a.unita = k == 0 ? 4 : 5;
        a.base_unita = k == 0 ? 1 : 2;
        a.situazione = "pace";
        a.contro_giocatore = true;
        a.zona = gambe[3];
        a.raggio = 25000.0;
        s.aerei_asw.push_back(a);
    }
    regola(s, "ritorno", {quando(C::tempo, 1200.0)},
           {azione(A::entra, 1), azione(A::entra, 2), azione(A::entra, 3), azione(A::entra, 4), azione(A::entra, 5)});
    // la svolta: a 40 minuti il gruppo accosta verso il punto dell'Alfa e lo passa, a 15 nodi
    const Vec2 oltre = acqua(f, polare(partenza, 225.0, 15000.0), 1500.0);
    regola(s, "svolta", {quando(C::tempo, 2400.0)},
           {vai(1, oltre, 15.0), vai(2, polare(oltre, 180.0, 3000.0), 15.0), vai(3, polare(oltre, 90.0, 5000.0), 15.0)});
    // la prora: a meno di 1000 m dalla portaerei la variabile 6 cade (era a 1 dalla partenza)
    regola(s, "partenza", {quando(C::tempo, 0.0)}, {imposta(5, "=", 1), imposta(6, "=", 1)});
    regola(s, "sulla_prora", {su(C::giocatore_vicino_a, 1, 1000.0)}, {imposta(6, "=", 0)});
    for (int id : {2, 3})
        regola(s, "classificati_" + std::to_string(id), {su(C::unita_classifica, id, 1.0)}, {imposta(5, "=", 0)});
    CondizioneScenario trasmesso = quando(C::giocatore_ha_trasmesso, 0.0);
    trasmesso.trasmissione = "contatto";
    regola(s, "rapporto", {compiuto("tenuta"), trasmesso}, {imposta(2, "=", 1)});
    regola(s, "riferito", {variabile(2, "=", 1.0)},
           {fine(40, {"Il gruppo e' ritrovato e tenuto, e il rapporto e' al comando.", "The group is found and held, and the report is with the command."})});
    regola(s, "collisione", {evento("collisione")},
           {fine(41, {"Collisione con la portaerei: come nel 1984, risaliti alla cieca sulla sua rotta. L'elica e' persa, servira' un rimorchiatore.",
                      "Collision with the carrier: as in 1984, surfacing blind on her track. The screw is lost; a tug will be needed."}, "fallimento")});
    s.obiettivi.push_back(obiettivo_contatto("tenuta", {1}, 60.0, {"Ritrovare la portaerei e tenerla un'ora", "Find the carrier again and hold her for an hour"}));
    s.obiettivi.push_back(obiettivo_variabile("rapporto", 2, 1, {"Riferire da quota periscopica (rapporto di contatto)", "Report from periscope depth (contact report)"}));
    s.obiettivi.push_back(obiettivo_variabile("prora", 6, 1, {"Mai a meno di 1000 m dalla portaerei", "Never within 1000 m of the carrier"}, false));
    s.obiettivi.push_back(obiettivo_variabile("occulto", 5, 1, {"Non farsi classificare dalla scorta", "Avoid being classified by the escort"}, false));
    s.obiettivi.push_back(vivo());
    RadioScenario r;
    r.ordini = OrdiniScenario{"Comando della Flotta del Pacifico", "K-463 (PROGETTO 705)", "SOTTO LA PRORA", "SOV. SEKRETNO"};
    r.roe = {{"sottomarini", "trattenuto"}, {"navi", "trattenuto"}, {"mercantili", "trattenuto"}, {"neutrali", "trattenuto"}, {"aerei", "trattenuto"}};
    s.radio = r;
    s.briefing[0] = {
        "ORDINI DI MISSIONE", "DA: Comando della Flotta del Pacifico", "A: K-463 (PROGETTO 705)", "OGG.: SOTTO LA PRORA",
        "21 marzo 1989, ore 1100Z (le 22 locali). Teatro: Pacifico, a est delle Curili. Pace. Notte, mare di marzo, vento forte.",
        "Da tre giorni pedinate un gruppo portaerei americano in esercitazione, che prova a scrollarvi di dosso: zig-zag, silenzio radio, cambi di velocita'. Col mare grosso il contatto si e' perso.",
        "Ritrovatelo, tenete la portaerei per un'ora e riferite col rapporto di contatto da quota periscopica.",
        "Attenzione quando tornate su: il gruppo puo' venirvi incontro a 15 nodi, e una portaerei non si ferma. Non venitele a meno di 1000 m.",
        "Il battello e' un Alfa al posto di un Victor I della Flotta del Pacifico. Regole d'ingaggio: tempo di pace. Armi trattenute.",
        "Tempo a disposizione: 3 ore.",
        "Nota storica: ispirata al K-314 e alla Kitty Hawk (21 marzo 1984, Mar del Giappone, esercitazione Team Spirit). Il Mar del Giappone nel gioco non c'e': la missione e' sul lato Pacifico delle Curili. Nomi e unita' di fantasia."};
    s.briefing[1] = {
        "MISSION ORDERS", "FM: PACIFIC FLEET HQ", "TO: K-463 (PROJECT 705)", "SUBJ: UNDER THE BOW",
        "21 March 1989, 1100Z (22:00 local). Theatre: Pacific, east of the Kurils. Peace. Night, March sea, strong wind.",
        "For three days you have trailed an American carrier group on exercise, which is trying to shake you off: zigzags, radio silence, speed changes. In the heavy sea contact has been lost.",
        "Find it again, hold the carrier for an hour and report with a contact report from periscope depth.",
        "Careful when you come up: the group may be coming at you at 15 knots, and a carrier does not stop. Do not come within 1000 m of her.",
        "Your boat is an Alfa standing in for a Pacific Fleet Victor I. Rules of engagement: peacetime. Weapons hold.",
        "Time available: 3 hours.",
        "Historical note: inspired by K-314 and Kitty Hawk (21 March 1984, Sea of Japan, exercise Team Spirit). The Sea of Japan is not in the game: the mission is on the Pacific side of the Kurils. Names and units are fictional."};
    return s;
}

// LIB-S18 «L'arcipelago» (docs/17 §2g, approvata il 30/10/2026): E15, Hårsfjärden (ottobre 1982), in un arcipelago
// neutrale di fantasia del Baltico [SOST]. Di notte il 688 deve fotografare la base in fondo all'arcipelago e uscire. Il
// paese costiero (la parte del luogo, M7), in crisi con gli USA, applica le ROE della commissione del 1983: nelle acque
// esterne, se una corvetta classifica il 688 (o lo tiene 5 minuti), avverte e lo caccia fuori col sonar (R_nave in crisi,
// senza sparare); nelle acque interne la Marina chiude l'area: le mine comandate da terra dell'ingresso passano alla
// stazione (m688.comanda_mine, runtime), che le fa scoppiare sotto un sottomarino che ci passa sopra, e il luogo passa
// alla guerra con gli USA: le corvette sparano per danneggiare o affondare. Il 688 sta nella parte esterna, fra 30 e 45 m
// d'acqua: la base si fotografa da 3 km.
ScenarioMissione arcipelago(const DatiScenario& d) {
    const Fondale f = Fondale::carica(DATI + "/teatri/baltico.fdl");
    ScenarioMissione s;
    base(s, d, f);
    s.id = "LIB_S18_ARCIPELAGO";
    s.titolo = {"L'arcipelago", "The Archipelago"};
    s.teatro = "baltico";
    s.giorno = 1;
    s.mese = 10;
    s.anno = 1989;
    s.ora_utc = 21.0;
    s.situazione = "pace";                                     // fra USA e URSS
    s.situazione_luogo = SituazioneLuogo{"crisi", "pace"};     // il paese neutrale in crisi con gli USA
    s.paranoia = 2;
    s.difficolta = 5;
    s.durata_s = 2.5 * 3600.0;
    carico_688(s);
    MeteoFissoScenario m;   // notte d'autunno, mare calmo, foschia
    m.stato_mare = 1.0, m.vento_m_s = 3.0, m.vento_da_gradi = 200.0, m.visibilita_m = 4000.0, m.nubi_ottavi = 6.0;
    s.meteo_fisso = m;
    const Vec2 largo = acqua_fra(f, f.proiettata({55.80, 15.50}), 45.0, 80.0);
    const Vec2 ingresso = acqua_fra(f, f.proiettata({56.02, 15.50}), 30.0, 45.0);
    const Vec2 base_navale = acqua_fra(f, f.proiettata({56.10, 15.55}), 10.0, 30.0);
    const Vec2 osservazione = acqua_fra(f, f.proiettata({56.07, 15.55}), 28.0, 40.0);
    s.unita.push_back(los_angeles(largo, 0.0, 25.0));
    s.unita.front().velocita = 4.0;
    // la base: una corvetta ormeggiata e un rimorchiatore (l'obiettivo della foto)
    UnitaScenario ormeggiata = unita(1, "stockholm", "nave", "luogo", base_navale, 90.0, 0.0, "");
    ormeggiata.ruolo = "corvetta ormeggiata alla base";
    s.unita.push_back(ormeggiata);
    UnitaScenario rimorchiatore = unita(2, "rimorchiatore", "nave", "luogo", polare(base_navale, 90.0, 300.0), 90.0, 0.0, "", true);
    rimorchiatore.ruolo = "rimorchiatore della base";
    s.unita.push_back(rimorchiatore);
    // le corvette di pattuglia e l'elicottero; un sottomarino costiero di guardia all'ingresso
    UnitaScenario esterna = unita(3, "stockholm", "nave", "luogo", acqua_fra(f, polare(ingresso, 200.0, 6000.0), 35.0, 80.0), 90.0, 7.0, "");
    esterna.punti = {acqua_fra(f, polare(ingresso, 150.0, 6000.0), 35.0, 80.0), acqua_fra(f, polare(ingresso, 230.0, 7000.0), 35.0, 80.0)};
    esterna.ciclica = true;
    esterna.ruolo = "corvetta di pattuglia nelle acque esterne";
    s.unita.push_back(esterna);
    UnitaScenario interna = unita(4, "stockholm", "nave", "luogo", osservazione, 270.0, 5.0, "");
    interna.punti = {acqua_fra(f, polare(osservazione, 90.0, 3000.0), 25.0, 45.0), acqua_fra(f, polare(osservazione, 270.0, 3000.0), 25.0, 45.0)};
    interna.ciclica = true;
    interna.ruolo = "corvetta nelle acque interne";
    s.unita.push_back(interna);
    UnitaScenario elicottero = unita(5, "hkp4", "aereo", "luogo", polare(ingresso, 0.0, 8000.0), 180.0, 100.0, "R_aereo");
    elicottero.ruolo = "elicottero antisom del paese";
    s.unita.push_back(elicottero);
    AereoAswScenario a;
    a.unita = 5;
    a.situazione = "pace";
    a.contro_giocatore = true;
    a.zona = ingresso;
    a.raggio = 12000.0;
    s.aerei_asw.push_back(a);
    UnitaScenario guardia = unita(6, "nacken", "sottomarino", "luogo", acqua_fra(f, polare(ingresso, 90.0, 4000.0), 35.0, 60.0), 0.0, 2.0, "");
    guardia.quota = 15.0;
    guardia.ruolo = "sottomarino costiero di guardia";
    s.unita.push_back(guardia);
    UnitaScenario traghetto = unita(7, "traghetto", "nave", "neutrale", acqua_fra(f, polare(largo, 90.0, 12000.0), 30.0, 90.0), 300.0, 16.0, "", true);
    traghetto.punti = {acqua_fra(f, polare(traghetto.pos, 300.0, 50000.0), 30.0, 90.0)};
    traghetto.ruolo = "traghetto";
    s.unita.push_back(traghetto);
    for (int k = 0; k < 3; ++k) {
        UnitaScenario p = unita(8 + k, "peschereccio", "nave", "neutrale", acqua_fra(f, polare(ingresso, 160.0 + 60.0 * k, 9000.0), 30.0, 80.0),
                                90.0 * k, 3.0, "", true);
        p.ruolo = "peschereccio";
        s.unita.push_back(p);
    }
    // lo sbarramento dell'ingresso: mine da fondo comandate da terra, ogni 300 m sul parallelo 56,02 dove ci sono fra 20 e 50 m
    const Vec2 ovest = f.proiettata({56.02, 15.30}), est = f.proiettata({56.02, 15.75});
    for (double x = 0.0; x <= distanza(ovest, est); x += 300.0) {
        const Vec2 p = polare(ovest, rilevamento(ovest, est), x);
        const double fondo = f.profondita(p);
        if (fondo >= 20.0 && fondo <= 50.0) {
            MinaScenario mn{"mdt", p, "luogo", true};
            mn.comandata = true;
            mn.gruppo = "ingresso";
            s.mine.push_back(mn);
        }
    }
    zona(s, "esterne", {"le acque esterne", "the outer waters"}, f.proiettata({56.10, 15.55}), 22000.0);
    zona(s, "interne", {"le acque interne dell'arcipelago", "the inner waters of the archipelago"}, f.proiettata({56.10, 15.55}), 9000.0);
    zona(s, "largo", {"il largo, a sud", "the open sea, to the south"}, largo, 4000.0);
    // le ROE a zone
    regola(s, "copertura", {quando(C::tempo, 0.0)}, {imposta(5, "=", 1), imposta(7, "=", 1)});
    const auto caccia = [](int id) { AzioneScenario c = azione(A::comportamento, id); c.comportamento = "R_nave"; return c; };
    const auto messaggio = [](const char* it, const char* en) {
        AzioneScenario msg = azione(A::messaggio);
        msg.testo = {it, en};
        return msg;
    };
    for (int id : {3, 4}) {
        // ovunque: classificato o tenuto, l'avvertimento e la caccia col sonar (in crisi le corvette non sparano)
        regola(s, "classificati_" + std::to_string(id), {su(C::unita_classifica, id, 1.0), variabile(5, "=", 1.0)},
               {imposta(5, "=", 0), azione(A::allarme), caccia(3), caccia(4),
                messaggio("Radio, sulla frequenza di soccorso: «Sottomarino straniero, siete in acque territoriali: emergete e seguite la corvetta fuori.»",
                          "Radio, on the distress frequency: \"Foreign submarine, you are in territorial waters: surface and follow the corvette out.\"")});
        regola(s, "tenuti_" + std::to_string(id), {su(C::unita_vede, id, 5.0), variabile(5, "=", 1.0)},
               {imposta(5, "=", 0), azione(A::allarme), caccia(3), caccia(4)});
    }
    // nelle acque interne, scoperto: la Marina chiude l'area, le mine alla stazione, la guerra del luogo con gli USA
    AzioneScenario guerra = azione(A::situazione);
    guerra.comportamento = "guerra";
    guerra.fra_a = "luogo";
    guerra.fra_b = "usa";
    AzioneScenario mine = azione(A::comanda_mine);
    mine.zona = "ingresso";
    regola(s, "chiusa", {in_zona("interne"), variabile(5, "=", 0.0), variabile(7, "=", 1.0)},
           {imposta(7, "=", 0), mine, guerra,
            messaggio("Sonar: cariche di profondita' in acqua, vicine. La Marina ha chiuso l'arcipelago: l'ingresso e' minato, e da terra lo guardano.",
                      "Sonar: depth charges in the water, close. The navy has closed the archipelago: the entrance is mined, and watched from shore.")});
    regola(s, "fuori", {compiuto("foto"), in_zona("largo")},
           {imposta(3, "=", 1), fine(40, {"Le foto della base sono a bordo, e il Los Angeles e' fuori dalle acque territoriali.",
                                          "The photographs of the base are aboard, and Los Angeles is out of territorial waters."})});
    regola(s, "emersi", {variabile(5, "=", 0.0), quando(C::giocatore_in_superficie), variabile(7, "=", 1.0)},
           {fine(43, {"Emersi nelle acque esterne: la corvetta vi scorta fuori. Il battello e' salvo, la missione no.",
                      "Surfaced in the outer waters: the corvette escorts you out. The boat is safe, the mission is not."}, "richiamato")});
    ObiettivoScenario foto = obiettivo("foto", "foto", {1}, {"Fotografare la base: la corvetta ormeggiata, entro 3 km", "Photograph the base: the moored corvette, within 3 km"});
    foto.distanza_m = 3000.0;
    s.obiettivi.push_back(foto);
    s.obiettivi.push_back(obiettivo_variabile("uscita", 3, 1, {"Uscire dalle acque territoriali", "Leave territorial waters"}));
    s.obiettivi.push_back(obiettivo_variabile("occulto", 5, 1, {"Non farsi classificare ne' tenere", "Avoid being classified or held"}, false));
    s.obiettivi.push_back(obiettivo_variabile("interne", 7, 1, {"Non far chiudere l'arcipelago (scoperti nelle acque interne)", "Do not get the archipelago closed (found in the inner waters)"}, false));
    s.obiettivi.push_back(vivo());
    RadioScenario r;
    r.ordini = OrdiniScenario{"COMSUBLANT", "USS LOS ANGELES (SSN 688)", "ARCHIPELAGO RECONNAISSANCE", "TOP SECRET"};
    r.roe = {{"sottomarini", "trattenuto"}, {"navi", "trattenuto"}, {"mercantili", "trattenuto"}, {"neutrali", "trattenuto"}, {"aerei", "trattenuto"}};
    s.radio = r;
    s.briefing[0] = {
        "ORDINI DI MISSIONE", "DA: COMSUBLANT", "A: USS LOS ANGELES (SSN-688)", "OGG.: RICOGNIZIONE DELL'ARCIPELAGO",
        "1 ottobre 1989, ore 2100Z. Teatro: Baltico, l'arcipelago di un paese neutrale. Crisi con quel paese. Notte, mare calmo, foschia, visibilita' 4 km.",
        "In fondo all'arcipelago c'e' una base navale. Fotografatela, la corvetta ormeggiata da meno di 3 km, e uscite. L'acqua e' poca: fra 30 e 45 m nella parte esterna, meno all'interno.",
        "Il paese ha regole d'ingaggio a zone. Nelle acque esterne, se vi trova, vi avverte e vi caccia fuori col sonar, senza sparare. Nelle acque interne chiude l'area: le mine dell'ingresso passano a una stazione a terra che le fa scoppiare sotto chi ci passa sopra, e le corvette sparano per danneggiare o affondare.",
        "Se emergete nelle acque esterne, vi scortano fuori: il battello e' salvo, la missione no.",
        "Regole d'ingaggio: armi trattenute. Non rispondete al fuoco.", "Tempo a disposizione: 2 ore e 30.",
        "Nota storica: ispirata alle incursioni negli arcipelaghi svedesi (Hårsfjärden, ottobre 1982) e alle regole adottate dopo. Chi fossero gli intrusi e' molto conteso. Paese, nomi e unita' di fantasia."};
    s.briefing[1] = {
        "MISSION ORDERS", "FM: COMSUBLANT", "TO: USS LOS ANGELES (SSN-688)", "SUBJ: ARCHIPELAGO RECONNAISSANCE",
        "1 October 1989, 2100Z. Theatre: Baltic, the archipelago of a neutral country. Crisis with that country. Night, calm sea, haze, visibility 4 km.",
        "At the head of the archipelago lies a naval base. Photograph it, the moored corvette from within 3 km, and come out. The water is shallow: 30 to 45 m in the outer part, less inside.",
        "The country has zoned rules of engagement. In the outer waters, if they find you, they warn you and chase you out with sonar, without firing. In the inner waters they close the area: the entrance mines pass to a shore station that fires them under whoever passes over, and the corvettes fire to damage or sink.",
        "If you surface in the outer waters, they escort you out: the boat is safe, the mission is not.",
        "Rules of engagement: weapons hold. Do not return fire.", "Time available: 2 hours 30.",
        "Historical note: inspired by the incursions into the Swedish archipelagos (Hårsfjärden, October 1982) and the rules adopted afterwards. Who the intruders were is much disputed. Country, names and units are fictional."};
    return s;
}

// LIB-S19 «Pietra santa» (docs/17 §2h, approvata il 30/10/2026): E05, la collisione Gato/K-19 (15 novembre 1969, Barents,
// 60 m). Una pattuglia della serie Holy Stone: il 688 sotto la coda di un SSBN che esce verso nord, nella notte polare
// del Barents d'inverno (mescolato, senza strato). La firma si registra entro 4 km (variabile 4, 10 s a ogni controllo,
// 40 minuti). Il Barents del gioco non arriva alla costa di Kola: le 12 miglia sono una zona sul margine sud. Due Grisha
// III con lo Shelon pingano dove capita; l'SSBN fa i Crazy Ivan (il motore) e, se il 688 gli e' sulla rotta alla stessa
// quota, e' la collisione del 1969 (esito 41).
ScenarioMissione pietra_santa(const DatiScenario& d) {
    const Fondale f = Fondale::carica(DATI + "/teatri/barents.fdl");
    ScenarioMissione s;
    base(s, d, f);
    s.id = "LIB_S19_PIETRA_SANTA";
    s.titolo = {"Pietra santa", "Holy Stone"};
    s.teatro = "barents";
    s.giorno = 15;
    s.mese = 11;
    s.anno = 1989;
    s.ora_utc = 4.0;   // notte polare
    s.situazione = "pace";
    s.paranoia = 2;
    s.difficolta = 4;
    s.durata_s = 3.0 * 3600.0;
    s.meteo_realistico = true;   // l'inverno del Barents: mescolato, senza strato
    carico_688(s);
    const Vec2 uscita = acqua(f, f.proiettata({70.25, 37.0}), 150.0);
    const Vec2 settore = acqua(f, f.proiettata({71.45, 37.6}), 250.0);
    const double rotta = rilevamento(uscita, settore);
    s.unita.push_back(los_angeles(acqua(f, polare(uscita, rotta - 90.0, 6000.0), 150.0), rotta, 60.0));
    s.unita.front().velocita = 5.0;
    UnitaScenario ssbn = unita(1, "delta3", "sottomarino", "urss", uscita, rotta, 6.0, "");
    ssbn.quota = 80.0;
    ssbn.punti = {settore};
    ssbn.ruolo = "SSBN verso il settore (al posto dell'Hotel K-19)";
    s.unita.push_back(ssbn);
    for (int k = 0; k < 2; ++k) {   // le Grisha III dove capita, in pattuglia sulla rotta
        UnitaScenario g = unita(2 + k, "grisha3", "nave", "urss", acqua(f, polare(polare(uscita, rotta, 15000.0 + 15000.0 * k), rotta + (k ? 90.0 : -90.0), 4000.0), 150.0),
                                rotta + 180.0, 8.0, "R_nave");
        for (int q = 0; q < 4; ++q) g.punti.push_back(acqua(f, polare(g.pos, 90.0 * q + 45.0, 6000.0), 150.0));
        g.ciclica = true;
        g.ruolo = "corvetta antisom col VDS";
        s.unita.push_back(g);
    }
    UnitaScenario il38 = unita(4, "il38", "aereo", "urss", polare(uscita, rotta, 30000.0), rotta + 180.0, 250.0, "R_aereo");
    il38.ruolo = "pattugliatore";
    s.unita.push_back(il38);
    AereoAswScenario a;
    a.unita = 4;
    a.situazione = "pace";
    a.contro_giocatore = true;
    a.zona = polare(uscita, rotta, 20000.0);
    a.raggio = 25000.0;
    s.aerei_asw.push_back(a);
    for (int k = 0; k < 3; ++k) {
        UnitaScenario b = unita(5 + k, "bmrt", "nave", "urss", acqua(f, polare(uscita, rotta + 40.0 * (k - 1), 12000.0 + 6000.0 * k), 150.0),
                                90.0 * k, 3.0, "", true);
        b.ruolo = "peschereccio d'altura";
        s.unita.push_back(b);
    }
    // le 12 miglia: una zona sul margine sud, al posto della costa che il teatro non ha
    zona(s, "acque_territoriali", {"le acque territoriali (12 miglia)", "territorial waters (12 miles)"}, f.proiettata({69.95, 37.0}), 22224.0);   // 12 miglia
    regola(s, "partenza", {quando(C::tempo, 0.0)}, {imposta(5, "=", 1), imposta(6, "=", 1), imposta(7, "=", 1)});
    regola(s, "registra", {su(C::giocatore_vicino_a, 1, 4000.0)}, {imposta(4, "+", 10)}, false);
    regola(s, "troppo_vicini", {su(C::giocatore_vicino_a, 1, 500.0)}, {imposta(7, "=", 0)});
    regola(s, "dentro_le_12", {in_zona("acque_territoriali")},
           {imposta(6, "=", 0), [] {
                AzioneScenario msg = azione(A::messaggio);
                msg.testo = {"Navigatore: siamo dentro le 12 miglia. Fuori subito.", "Navigator: we are inside the 12-mile limit. Out at once."};
                return msg;
            }()});
    for (int id : {1, 2, 3})
        regola(s, "classificati_" + std::to_string(id), {su(C::unita_classifica, id, 1.0)}, {imposta(5, "=", 0)});
    regola(s, "registrata", {compiuto("firma")},
           {fine(40, {"La firma dell'SSBN e' sul nastro: quaranta minuti sotto la sua coda.", "The SSBN's signature is on tape: forty minutes under her tail."})});
    regola(s, "collisione", {evento("collisione")},
           {fine(41, {"Collisione con l'SSBN: come nel 1969, il Crazy Ivan vi ha trovati sulla sua rotta. Rientrare.",
                      "Collision with the SSBN: as in 1969, the Crazy Ivan found you on her track. Return to base."}, "fallimento")});
    s.obiettivi.push_back(obiettivo_variabile("firma", 4, 2400, {"Registrare l'SSBN: 40 minuti entro 4 km", "Record the SSBN: 40 minutes within 4 km"}));
    s.obiettivi.push_back(obiettivo_variabile("dodici", 6, 1, {"Mai dentro le 12 miglia", "Never inside the 12-mile limit"}));
    s.obiettivi.push_back(obiettivo_variabile("distanza", 7, 1, {"Mai a meno di 500 m dall'SSBN", "Never within 500 m of the SSBN"}, false));
    s.obiettivi.push_back(obiettivo_variabile("occulto", 5, 1, {"Non farsi classificare", "Avoid being classified"}, false));
    s.obiettivi.push_back(vivo());
    RadioScenario r;
    r.ordini = OrdiniScenario{"COMSUBLANT", "USS LOS ANGELES (SSN 688)", "HOLY STONE PATROL", "TOP SECRET"};
    r.roe = {{"sottomarini", "trattenuto"}, {"navi", "trattenuto"}, {"mercantili", "trattenuto"}, {"neutrali", "trattenuto"}, {"aerei", "trattenuto"}};
    s.radio = r;
    s.briefing[0] = {
        "ORDINI DI MISSIONE", "DA: COMSUBLANT", "A: USS LOS ANGELES (SSN-688)", "OGG.: PATTUGLIA HOLY STONE",
        "15 novembre 1989, ore 0400Z, notte polare. Teatro: Mare di Barents. Pace. L'inverno ha mescolato il mare: niente strato.",
        "Un SSBN sovietico esce verso il suo settore di pattuglia, a nord. Mettetevi sotto la sua coda e registratene la firma: quaranta minuti entro 4 km.",
        "Restate fuori dalle 12 miglia. Le corvette antisom pingano dove capita, e un pattugliatore e' in volo.",
        "L'SSBN si guarda la poppa: ogni tanto accosta e torna sulla sua scia. Se in quel momento siete sulla sua rotta alla sua quota, lo urtate. Mai a meno di 500 m.",
        "Regole d'ingaggio: tempo di pace. Armi trattenute.", "Tempo a disposizione: 3 ore.",
        "Nota storica: ispirata alle pattuglie Holy Stone e alla collisione fra il Gato e il K-19 (15 novembre 1969, Mare di Barents). La costa di Kola e' fuori dal teatro: le 12 miglia sono una zona sul margine sud. Nomi e unita' di fantasia."};
    s.briefing[1] = {
        "MISSION ORDERS", "FM: COMSUBLANT", "TO: USS LOS ANGELES (SSN-688)", "SUBJ: HOLY STONE PATROL",
        "15 November 1989, 0400Z, polar night. Theatre: Barents Sea. Peace. Winter has mixed the sea: no layer.",
        "A Soviet SSBN is heading north to her patrol sector. Get under her tail and record her signature: forty minutes within 4 km.",
        "Stay outside the 12-mile limit. The ASW corvettes ping at random, and a patrol aircraft is up.",
        "The SSBN clears her baffles: now and then she turns and comes back down her wake. If you are on her track at her depth then, you hit her. Never within 500 m.",
        "Rules of engagement: peacetime. Weapons hold.", "Time available: 3 hours.",
        "Historical note: inspired by the Holy Stone patrols and the collision between Gato and K-19 (15 November 1969, Barents Sea). The Kola coast is outside the theatre: the 12-mile limit is a zone on the southern edge. Names and units are fictional."};
    return s;
}

// LIB-S20 «Cinquecento iarde» (docs/17 §2h, approvata il 30/10/2026): E24, l'Augusta nell'ottobre 1986 (con chi resta
// conteso: il K-219 per i sovietici, il K-279 per gli autori americani). Il 688 pedina di notte un SSBN sovietico in
// pattuglia nell'Atlantico (la Piattaforma Celtica [SOST]). La firma sale piu' in fretta da vicino: 10 a ogni controllo
// entro 5 km, 20 in piu' entro 2 km, fino a 1800 (variabile 4). Sotto le 500 iarde (457 m) cade l'obiettivo secondario;
// la collisione chiude con l'esito 41. Dopo un'ora l'SSBN cambia schema: rallenta a 3 nodi e scende verso i 130 m
// (l'azione «quota»; il controllo del fondo dell'IA lo tiene piu' su dove l'acqua e' poca); i Crazy Ivan sono del motore. Un Victor III di scorta a 12 km puo' sentire il pedinatore.
ScenarioMissione cinquecento_iarde(const DatiScenario& d) {
    const Fondale f = Fondale::carica(DATI + "/teatri/celtica.fdl");
    ScenarioMissione s;
    base(s, d, f);
    s.id = "LIB_S20_CINQUECENTO_IARDE";
    s.titolo = {"Cinquecento iarde", "Five Hundred Yards"};
    s.teatro = "celtica";
    s.giorno = 20;
    s.mese = 10;
    s.anno = 1989;
    s.ora_utc = 2.0;
    s.situazione = "pace";
    s.paranoia = 2;
    s.difficolta = 3;
    s.durata_s = 2.5 * 3600.0;
    s.meteo_realistico = true;
    carico_688(s);
    // il settore di pattuglia sul ciglio a sud-ovest (fino a 535 m), un circuito largo
    const Vec2 centro = acqua(f, f.proiettata({48.55, -9.30}), 160.0);
    std::vector<Vec2> circuito;
    for (int q = 0; q < 4; ++q) circuito.push_back(acqua(f, polare(centro, 90.0 * q + 30.0, 9000.0), 170.0));
    UnitaScenario ssbn = unita(1, "delta3", "sottomarino", "urss", circuito[3], rilevamento(circuito[3], circuito[0]), 5.0, "");
    ssbn.quota = 90.0;
    ssbn.punti = circuito;
    ssbn.ciclica = true;
    ssbn.ruolo = "SSBN in pattuglia (al posto del Delta I o dello Yankee)";
    s.unita.push_back(ssbn);
    UnitaScenario victor = unita(2, "victor3", "sottomarino", "urss", acqua(f, polare(centro, 200.0, 12000.0), 170.0), 110.0, 6.0, "");
    for (int q = 0; q < 4; ++q) victor.punti.push_back(acqua(f, polare(centro, 90.0 * q + 75.0, 13000.0), 170.0));
    victor.ciclica = true;
    victor.quota = 120.0;
    victor.parametri.paranoia = 2;
    victor.ruolo = "scorta dell'SSBN";
    s.unita.push_back(victor);
    for (int k = 0; k < 2; ++k) {
        UnitaScenario m = unita(3 + k, k ? "petroliera" : "mercantile", "nave", "neutrale", acqua(f, polare(centro, 330.0 + 60.0 * k, 25000.0), 100.0),
                                k ? 20.0 : 200.0, 12.0, "", true);
        m.punti = {acqua(f, polare(m.pos, k ? 20.0 : 200.0, 60000.0), 100.0)};
        m.ruolo = "traffico";
        s.unita.push_back(m);
    }
    s.unita.insert(s.unita.begin(), los_angeles(acqua(f, polare(circuito[3], rilevamento(circuito[0], circuito[3]), 10000.0), 150.0),
                                                rilevamento(circuito[0], circuito[3]), 60.0));   // la prua lontano: da passivo non registra
    s.unita.front().velocita = 6.0;
    regola(s, "partenza", {quando(C::tempo, 0.0)}, {imposta(5, "=", 1), imposta(7, "=", 1)});
    regola(s, "firma_5_km", {su(C::giocatore_vicino_a, 1, 5000.0)}, {imposta(4, "+", 10)}, false);
    regola(s, "firma_2_km", {su(C::giocatore_vicino_a, 1, 2000.0)}, {imposta(4, "+", 20)}, false);
    regola(s, "cinquecento_iarde", {su(C::giocatore_vicino_a, 1, 457.0)}, {imposta(7, "=", 0)});
    regola(s, "nuovo_schema", {quando(C::tempo, 3600.0)},
           {[] { AzioneScenario q = azione(A::quota, 1); q.valore = 130; return q; }(), vai(1, circuito[1], 3.0)});
    for (int id : {1, 2})
        regola(s, "classificati_" + std::to_string(id), {su(C::unita_classifica, id, 1.0)}, {imposta(5, "=", 0)});
    regola(s, "registrata", {compiuto("firma")},
           {fine(40, {"La firma dell'SSBN e' sul nastro, pulita.", "The SSBN's signature is on tape, clean."})});
    regola(s, "collisione", {evento("collisione")},
           {fine(41, {"Collisione con l'SSBN: il sonar di prora e' da rifare, e qualcuno dovra' spiegare cosa ci facevate a quella distanza.",
                      "Collision with the SSBN: the bow sonar must be replaced, and someone will have to explain what you were doing that close."}, "fallimento")});
    s.obiettivi.push_back(obiettivo_variabile("firma", 4, 1800, {"La firma dell'SSBN (piu' in fretta da vicino)", "The SSBN's signature (faster from close in)"}));
    s.obiettivi.push_back(obiettivo_variabile("iarde", 7, 1, {"Mai sotto le 500 iarde", "Never inside 500 yards"}, false));
    s.obiettivi.push_back(obiettivo_variabile("occulto", 5, 1, {"Non farsi classificare", "Avoid being classified"}, false));
    // tre rotte dello stesso SSBN, che sta immerso: dalla TMA al MATE, non dall'AOB al periscopio
    ObiettivoScenario rotta = obiettivo("rotta", "rotta_stimata", {1}, {"Tre rotte dell'SSBN dalla TMA, entro 20 gradi", "Three courses of the SSBN from the TMA, within 20 degrees"}, false);
    rotta.minimo = 3;
    rotta.errore = 20.0;
    rotta.da_tma = true;
    s.obiettivi.push_back(rotta);
    s.obiettivi.push_back(vivo());
    RadioScenario r;
    r.ordini = OrdiniScenario{"COMSUBLANT", "USS LOS ANGELES (SSN 688)", "SSBN TRAIL - EASTERN ATLANTIC", "TOP SECRET"};
    r.roe = {{"sottomarini", "trattenuto"}, {"navi", "trattenuto"}, {"mercantili", "trattenuto"}, {"neutrali", "trattenuto"}, {"aerei", "trattenuto"}};
    s.radio = r;
    s.briefing[0] = {
        "ORDINI DI MISSIONE", "DA: COMSUBLANT", "A: USS LOS ANGELES (SSN-688)", "OGG.: PEDINAMENTO DI UN SSBN - ATLANTICO ORIENTALE",
        "20 ottobre 1989, ore 0200Z, notte. Teatro: Atlantico, sul ciglio della Piattaforma Celtica. Pace. Mare d'autunno.",
        "Un SSBN sovietico pattuglia il suo settore. Il comando vuole la firma piu' pulita possibile e lo schema della sua pattuglia.",
        "La firma si fa in fretta da vicino: entro 2 km e' oro, entro 5 km basta. Sotto le 500 iarde il rischio cresce, e nessuno sa dove girera'. Una scorta puo' essere nei paraggi.",
        "Stimate anche la sua rotta tre volte, con la TMA al MATE. Regole d'ingaggio: tempo di pace. Armi trattenute.",
        "Tempo a disposizione: 2 ore e 30.",
        "Nota storica: ispirata alla collisione dell'Augusta nell'ottobre 1986. Con chi resta conteso: per i sovietici il K-219 al largo delle Bermuda, per gli autori americani il K-279. Nomi e unita' di fantasia."};
    s.briefing[1] = {
        "MISSION ORDERS", "FM: COMSUBLANT", "TO: USS LOS ANGELES (SSN-688)", "SUBJ: SSBN TRAIL - EASTERN ATLANTIC",
        "20 October 1989, 0200Z, night. Theatre: Atlantic, on the edge of the Celtic Shelf. Peace. Autumn sea.",
        "A Soviet SSBN is patrolling her sector. The command wants the cleanest possible signature and her patrol pattern.",
        "The signature comes fast from close in: within 2 km it is gold, within 5 km it will do. Inside 500 yards the risk grows, and nobody knows where she will turn. An escort may be about.",
        "Also estimate her course three times, with the TMA on the MATE. Rules of engagement: peacetime. Weapons hold.",
        "Time available: 2 hours 30.",
        "Historical note: inspired by the collision of Augusta in October 1986. With whom remains disputed: for the Soviets K-219 off Bermuda, for American authors K-279. Names and units are fictional."};
    return s;
}

// LIB-S25 «Ombra del boomer» (docs/17 §2i, approvata il 31/10/2026): il 688 in coda a un SSBN sovietico sulla piana
// abissale (la Celtica al posto dell'Atlantico a nord-est delle Bermuda, E23). Dopo le due ore, nella sua zona di
// pattuglia e col contatto tenuto almeno un'ora (comunque a 3 ore e mezza), l'SSBN emerge in emergenza e si ferma
// (azioni «quota» e «avaria»). Il 688 deve riferire (rapporto di situazione), restare nascosto e lontano 5 miglia dalle
// navi sovietiche che arrivano: un mercantile tenta il rimorchio, che dopo 50 minuti non tiene; un P-3C viene a
// guardare. Quando il rimorchio e' mollato COMSUBLANT richiama il 688: il giocatore non vede affondare il battello
// (dossier 23 §5). Se il 688 e' stato a meno di 1000 iarde dall'SSBN, arriva la versione sovietica della collisione,
// che la missione non decide. Variabili: 4 contatto tenuto, 5 non classificato, 6 riferito, 7 lontano dalle navi,
// 8 mai sotto le 1000 iarde, 10 fase (1 emergenza, 2 rimorchio, 3 mollato), 11 secondi dall'emergenza, 12 secondi dal
// rimorchio. Semplificazioni (docs/17): l'ora dell'emergenza viene dal contatto e dalla zona, non dal caso; lo Yankee e'
// il `delta3`; nessun fumo (il pezzo facoltativo non c'e').
ScenarioMissione ombra_del_boomer(const DatiScenario& d) {
    const Fondale f = Fondale::carica(DATI + "/teatri/celtica.fdl");
    ScenarioMissione s;
    base(s, d, f);
    s.id = "LIB_S25_OMBRA_DEL_BOOMER";
    s.titolo = {"Ombra del boomer", "Boomer's Shadow"};
    s.teatro = "celtica";
    s.giorno = 10;
    s.mese = 10;
    s.anno = 1989;
    s.ora_utc = 4.0;
    s.situazione = "pace";
    s.paranoia = 2;
    s.difficolta = 3;
    s.durata_s = 6.0 * 3600.0;
    s.meteo_realistico = true;
    carico_688(s);
    // la piana abissale a sud-ovest del ciglio (4300-4800 m): l'SSBN scende da nord-ovest verso la sua zona e ci gira
    const Vec2 zona_e = acqua(f, f.proiettata({47.3, -10.5}), 3000.0);
    const Vec2 ingresso = acqua(f, polare(zona_e, 300.0, 20000.0), 3000.0);
    UnitaScenario ssbn = unita(2, "delta3", "sottomarino", "urss", ingresso, rilevamento(ingresso, zona_e), 4.0, "");
    ssbn.quota = 80.0;
    ssbn.punti = {zona_e};
    for (int q = 0; q < 4; ++q) ssbn.punti.push_back(acqua(f, polare(zona_e, 90.0 * q, 4000.0), 3000.0));
    ssbn.ruolo = "SSBN in pattuglia (al posto dello Yankee I)";
    s.unita.push_back(ssbn);
    // le navi sovietiche che accorrono: un mercantile (il rimorchio) e un rimorchiatore della flotta, troppo lontano
    UnitaScenario cargo = unita(3, "mercantile", "nave", "urss", acqua(f, polare(zona_e, 130.0, 30000.0), 1000.0), 310.0, 14.0, "", true);
    cargo.punti = {zona_e};
    cargo.entra_con = "superficie";
    cargo.ruolo = "mercantile sovietico deviato sull'emergenza";
    s.unita.push_back(cargo);
    UnitaScenario rimorchiatore = unita(4, "rimorchiatore", "nave", "urss", acqua(f, polare(zona_e, 60.0, 50000.0), 200.0), 240.0, 12.0, "", true);
    rimorchiatore.punti = {zona_e};
    rimorchiatore.entra_con = "rimorchiatore";
    rimorchiatore.ruolo = "rimorchiatore della flotta";
    s.unita.push_back(rimorchiatore);
    UnitaScenario p3 = unita(5, "p3c", "aereo", "usa", polare(zona_e, 0.0, 40000.0), 180.0, 250.0, "R_aereo");
    p3.entra_con = "orion";
    p3.ruolo = "P-3C che viene a guardare";
    s.unita.push_back(p3);
    AereoAswScenario a;
    a.unita = 5;
    a.situazione = "pace";
    a.contro_giocatore = false;
    a.bersagli = {2};
    a.zona = zona_e;
    a.raggio = 15000.0;
    s.aerei_asw.push_back(a);
    for (int k = 0; k < 2; ++k) {
        UnitaScenario m = unita(6 + k, k ? "portarinfuse" : "mercantile", "nave", "neutrale", acqua(f, polare(zona_e, 20.0 + 180.0 * k, 35000.0), 1000.0),
                                k ? 290.0 : 110.0, 12.0, "", true);
        m.punti = {acqua(f, polare(m.pos, k ? 290.0 : 110.0, 70000.0), 1000.0)};
        m.ruolo = "traffico atlantico";
        s.unita.push_back(m);
    }
    s.unita.insert(s.unita.begin(), los_angeles(acqua(f, polare(ingresso, 180.0, 8000.0), 3000.0), rilevamento(ingresso, zona_e), 120.0));
    s.unita.front().velocita = 5.0;
    zona(s, "pattuglia", {"il settore di pattuglia dell'SSBN", "the SSBN's patrol sector"}, zona_e, 6000.0);
    zona(s, "luogo", {"il punto dell'emergenza", "the emergency position"}, zona_e, 5000.0);
    const auto msg = [](TestiScenario t) {
        AzioneScenario m = azione(A::messaggio);
        m.testo = std::move(t);
        return m;
    };
    const auto quota = [](int u, int metri) {
        AzioneScenario q = azione(A::quota, u);
        q.valore = metri;
        return q;
    };
    regola(s, "partenza", {quando(C::tempo, 0.0)}, {imposta(5, "=", 1), imposta(7, "=", 1), imposta(8, "=", 1)});
    regola(s, "tenuto", {su(C::contatto_su, 2, 60.0)}, {imposta(4, "=", 1)});
    regola(s, "mille_iarde", {su(C::giocatore_vicino_a, 2, 914.0)}, {imposta(8, "=", 0)});
    regola(s, "classificati", {su(C::unita_classifica, 2, 1.0)}, {imposta(5, "=", 0)});
    // la svolta: dopo le due ore, nella zona e col contatto tenuto; comunque a tre ore e mezza
    const std::vector<AzioneScenario> emergenza = {
        imposta(10, "=", 1), azione(A::comportamento, 2), quota(2, 0),   // senza comportamento; niente «rumore»: il suo stesso tonfo lo farebbe evadere
        msg({"Sonar: transitorio forte dal contatto, Comandante. Poi aria ad alta pressione: sta dando aria a tutte le casse.",
             "Sonar: loud transient from the contact, Captain. Then high-pressure air: she is blowing all tanks."})};
    regola(s, "emergenza", {quando(C::tempo, 7200.0), unita_in(2, "pattuglia"), variabile(4, "=", 1.0), variabile(10, "=", 0.0)}, emergenza);
    regola(s, "emergenza_tardi", {quando(C::tempo, 12600.0), variabile(10, "=", 0.0)}, emergenza);
    regola(s, "superficie", {su(C::unita_in_superficie, 2, 0.0), variabile(10, "=", 1.0)},
           {[] { AzioneScenario v = azione(A::avaria, 2); v.valore = 1; return v; }(), azione(A::entra, 3), msg({"Sonar: il contatto e' in superficie e fermo. Le macchine tacciono. Si sentono solo pompe e il mare sullo scafo.",
                                       "Sonar: the contact is on the surface and stopped. Her machinery is silent. Only pumps and the sea on her hull."})});
    regola(s, "orologio", {variabile(10, ">", 0.0)}, {imposta(11, "+", 10)}, false);
    regola(s, "resta_su", {variabile(10, ">", 0.0)}, {quota(2, 0)}, false);   // l'evasione dell'IA (sorvoli, tonfi) non lo riporta giu'
    CondizioneScenario situazione = quando(C::giocatore_ha_trasmesso, 7200.0);   // il rapporto di situazione, dopo le due ore
    situazione.trasmissione = "situazione";
    regola(s, "riferito", {variabile(10, ">", 0.0), variabile(11, "<", 1810.0), situazione},
           {imposta(6, "=", 1), manda("ORDINI")});
    regola(s, "ordini_comunque", {variabile(11, ">", 2690.0), variabile(6, "=", 0.0)}, {manda("ORDINI")});
    regola(s, "orion", {variabile(11, ">", 2390.0)}, {azione(A::entra, 5)});
    regola(s, "rimorchiatore", {variabile(11, ">", 3590.0)}, {azione(A::entra, 4)});
    for (int id : {3, 4})
        regola(s, "navi_" + std::to_string(id), {su(C::giocatore_vicino_a, id, 9260.0)}, {imposta(7, "=", 0)});
    AzioneScenario traino = azione(A::rimorchia, 3);
    traino.bersaglio = 2;
    traino.velocita = 3.0;
    traino.valore = 300;
    regola(s, "rimorchio", {unita_in(3, "luogo"), variabile(10, "=", 1.0), su(C::unita_in_avaria, 2, 0.0)},
           {imposta(10, "=", 2), traino, vai(3, acqua(f, polare(zona_e, 60.0, 60000.0), 1000.0), 3.0),
            msg({"Sonar: un'elica vicino al contatto, poi un cavo in tensione. Lo stanno prendendo a rimorchio.",
                 "Sonar: a screw close to the contact, then a cable under strain. They are taking her in tow."})});
    regola(s, "orologio_rimorchio", {variabile(10, ">", 1.0)}, {imposta(12, "+", 10)}, false);
    regola(s, "rimorchio_mollato", {variabile(12, ">", 2990.0)},
           {imposta(10, "=", 3), azione(A::molla_rimorchio, 3),
            msg({"Sonar: il cavo si e' spezzato. Il mercantile ha fermato le macchine e mette le lance in mare.",
                 "Sonar: the cable has parted. The freighter has stopped engines and is putting boats in the water."})});
    regola(s, "versione_sovietica", {variabile(10, "=", 3.0), variabile(8, "=", 0.0)}, {manda("COLLISIONE")});
    regola(s, "richiamo", {variabile(10, "=", 3.0)}, {manda("RICHIAMO")});
    regola(s, "fine_pattuglia", {scattata("richiamo"), variabile(12, ">", 3290.0)},
           {fine(40, {"COMSUBLANT vi richiama. L'equipaggio sovietico e' sul mercantile; il battello resta dove il mare lo tiene. Avete fatto quello che vi era chiesto: guardare, riferire, restare fuori.",
                      "COMSUBLANT recalls you. The Soviet crew is aboard the freighter; the boat stays where the sea holds her. You did what was asked: watch, report, stay out."})});
    regola(s, "collisione", {evento("collisione")},
           {fine(41, {"Collisione con l'SSBN. La pattuglia finisce qui, e il rapporto lo scriveranno altri.",
                      "Collision with the SSBN. The patrol ends here, and others will write the report."}, "fallimento")});
    s.obiettivi.push_back(obiettivo_variabile("tenuto", 4, 1, {"Tenere il contatto sull'SSBN (almeno un'ora)", "Hold contact on the SSBN (at least an hour)"}));
    s.obiettivi.push_back(obiettivo_variabile("occulto", 5, 1, {"Non farsi classificare dall'SSBN", "Avoid being classified by the SSBN"}));
    s.obiettivi.push_back(obiettivo_variabile("riferito", 6, 1, {"Riferire l'emergenza entro 30 minuti (rapporto di situazione)", "Report the emergency within 30 minutes (situation report)"}));
    s.obiettivi.push_back(obiettivo_variabile("lontano", 7, 1, {"Restare a 5 miglia dalle navi sovietiche", "Stay 5 miles from the Soviet ships"}));
    s.obiettivi.push_back(obiettivo_variabile("iarde", 8, 1, {"Mai a meno di 1000 iarde dall'SSBN", "Never inside 1000 yards of the SSBN"}, false));
    s.obiettivi.push_back(vivo());
    RadioScenario r;
    r.ordini = OrdiniScenario{"COMSUBLANT", "USS LOS ANGELES (SSN 688)", "SSBN TRAIL - WESTERN ATLANTIC", "TOP SECRET"};
    r.roe = {{"sottomarini", "trattenuto"}, {"navi", "trattenuto"}, {"mercantili", "trattenuto"}, {"neutrali", "trattenuto"}, {"aerei", "trattenuto"}};
    r.messaggi = {
        radio("ORDINI", -1.0, "flash", "COMSUBLANT", "SOVIET SSBN EMERGENCY", "TOP SECRET",
              {"RICEVUTO. SSBN SOVIETICO IN EMERGENZA IN SUPERFICIE. RESTATE NASCOSTI, MANTENETE IL CONTATTO A DISTANZA. NON AVVICINATEVI A MENO DI 5 MIGLIA DALLE UNITA' SOVIETICHE IN ARRIVO. NESSUNA INTERFERENZA. P-3 IN ARRIVO.",
               "ROGER. SOVIET SSBN IN EMERGENCY ON THE SURFACE. REMAIN COVERT, MAINTAIN CONTACT AT A DISTANCE. DO NOT CLOSE WITHIN 5 MILES OF ARRIVING SOVIET UNITS. NO INTERFERENCE. P-3 EN ROUTE."}),
        radio("COLLISIONE", -1.0, "immediate", "COMSUBLANT", "SOVIET CLAIM", "SECRET",
              {"FONTI SOVIETICHE ATTRIBUISCONO L'INCIDENTE A UNA COLLISIONE CON UN SOTTOMARINO AMERICANO. LA MARINA RESPINGE L'ACCUSA. MANTENETE IL SILENZIO SULL'ARGOMENTO.",
               "SOVIET SOURCES ATTRIBUTE THE CASUALTY TO A COLLISION WITH AN AMERICAN SUBMARINE. THE NAVY REJECTS THE CLAIM. MAINTAIN SILENCE ON THE SUBJECT."}),
        radio("RICHIAMO", -1.0, "immediate", "COMSUBLANT", "RECALL", "TOP SECRET",
              {"IL RIMORCHIO E' FALLITO. INTERROMPETE IL PEDINAMENTO E RIENTRATE. BEN FATTO.", "TOW HAS FAILED. BREAK OFF TRAIL AND RETURN. WELL DONE."})};
    s.radio = r;
    s.briefing[0] = {
        "ORDINI DI MISSIONE", "DA: COMSUBLANT", "A: USS LOS ANGELES (SSN-688)", "OGG.: PEDINAMENTO DI UN SSBN - ATLANTICO OCCIDENTALE",
        "10 ottobre 1989, ore 0400Z. Teatro: Atlantico, sulla piana abissale. Pace. Mare d'autunno.",
        "Un SSBN sovietico scende verso il suo settore di pattuglia. Mettetevi in coda, tenetelo e registratelo senza farvi sentire.",
        "Distanza: oltre le 1000 iarde. In caso di qualsiasi evento anomalo dell'SSBN, riferite subito con un rapporto di situazione e attendete ordini.",
        "Regole d'ingaggio: tempo di pace. Armi trattenute. Tempo a disposizione: 6 ore.",
        "Nota storica: ispirata al K-219, lo Yankee che il 3 ottobre 1986, a circa 1090 km dalle Bermuda, emerse dopo un'esplosione in un tubo dei missili; il rimorchio falli' e il battello affondo' il 6 ottobre. Morirono quattro uomini. Secondo i sovietici una collisione con l'USS Augusta; la US Navy e il comandante del K-219 lo hanno sempre negato. Nomi e unita' di fantasia."};
    s.briefing[1] = {
        "MISSION ORDERS", "FM: COMSUBLANT", "TO: USS LOS ANGELES (SSN-688)", "SUBJ: SSBN TRAIL - WESTERN ATLANTIC",
        "10 October 1989, 0400Z. Theatre: Atlantic, on the abyssal plain. Peace. Autumn sea.",
        "A Soviet SSBN is heading down to her patrol sector. Get in her baffles, hold her and record her without being heard.",
        "Range: beyond 1000 yards. In case of any unusual event on the SSBN, report at once with a situation report and await orders.",
        "Rules of engagement: peacetime. Weapons hold. Time available: 6 hours.",
        "Historical note: inspired by K-219, the Yankee that on 3 October 1986, some 1,090 km from Bermuda, surfaced after an explosion in a missile tube; the tow failed and the boat sank on 6 October. Four men died. According to the Soviets it was a collision with USS Augusta; the US Navy and K-219's commander always denied it. Names and units are fictional."};
    return s;
}

// LIB-S22 «La rotta lunga» (docs/17 §2i, approvata il 31/10/2026): l'Alfa, per ruolo, uno dei cinque battelli
// dell'operazione Atrina (E25), nell'Atlantico aperto dopo la barriera (la Celtica al posto dell'Atlantico occidentale).
// Quattro Victor III dell'IA scendono a sud-ovest su rotte parallele; il SOSUS da' alla NATO un datum a 30 minuti e uno a
// due ore (azione «cue»), e due P-3C, una Kidd e un Los Angeles cercano. I P-3 non hanno bersagli dichiarati:
// cercano tutti i sottomarini sovietici, Alfa compreso. Un battello e' tenuto se un cacciatore
// NATO (i due P-3, la Kidd, i due Los Angeles) ha accumulato 70 minuti di contatto su di lui («unita' vede»); vince chi ne
// porta in fondo liberi almeno tre su cinque, Alfa compreso. Secondari: classificare l'Ohio che pattuglia lento nel settore, e l'Alfa tenuto meno di
// 30 minuti. Variabili: 2-6 tenuti (Alfa, i quattro Victor), 7 quanti liberi, 8 l'obiettivo, 9 Alfa non tenuto (meno di 30
// minuti cumulati per cacciatore).
// Semplificazioni (docs/17): la «condizione per parte» della scheda non serve, bastano le coppie cacciatore-preda.
ScenarioMissione rotta_lunga(const DatiScenario& d) {
    const Fondale f = Fondale::carica(DATI + "/teatri/celtica.fdl");
    ScenarioMissione s;
    base(s, d, f);
    s.id = "LIB_S22_ROTTA_LUNGA";
    s.titolo = {"La rotta lunga", "The Long Way Round"};
    s.teatro = "celtica";
    s.alfa = true;
    s.giorno = 20;
    s.mese = 4;
    s.anno = 1989;
    s.ora_utc = 6.0;
    s.situazione = "pace";
    s.paranoia = 2;
    s.difficolta = 4;
    s.durata_s = 4.0 * 3600.0;
    s.meteo_realistico = true;
    carico_alfa(s);
    constexpr double ROTTA = 210.0, NODI = 7.0, MS = NODI * 1852.0 / 3600.0;
    const double LON[] = {-10.0, -10.35, -10.75, -11.1};
    std::vector<Vec2> inizio;
    for (int k = 0; k < 4; ++k) {
        const Vec2 p = acqua(f, f.proiettata({48.35, LON[k]}), 1500.0);
        inizio.push_back(p);
        const double rotta = 180.0 + 25.0 * k;   // a ventaglio: inseguito l'Alfa, i Victor si allontanano dai cacciatori
        UnitaScenario v = unita(2 + k, "victor3", "sottomarino", "urss", p, rotta, NODI, "");
        v.quota = 150.0 + 30.0 * k;
        v.punti = {acqua(f, polare(p, rotta, 70000.0), 1500.0)};
        v.parametri.paranoia = 2;
        v.ruolo = "Victor III del gruppo";
        s.unita.push_back(v);
    }
    // NATO: due P-3C che arrivano coi datum del SOSUS, una Kidd e un Los Angeles avanti sulla rotta
    for (int k = 0; k < 2; ++k) {
        UnitaScenario p3 = unita(6 + k, "p3c", "aereo", "usa", f.proiettata({49.3, -9.4 - 0.3 * k}), 220.0, 250.0, "R_aereo");
        p3.entra_con = k ? "sosus_2" : "sosus_1";
        p3.ruolo = "P-3C sul datum del SOSUS";
        s.unita.push_back(p3);
        AereoAswScenario a;
        a.unita = 6 + k;
        a.situazione = "pace";
        a.contro_giocatore = false;   // nessun bersaglio dichiarato: tutti i sottomarini sovietici, Alfa compreso
        a.zona = polare(inizio[k ? 1 : 2], k ? 205.0 : 230.0, MS * (k ? 7200.0 : 1800.0));
        a.raggio = 20000.0;
        s.aerei_asw.push_back(a);
    }
    const Vec2 avanti = acqua(f, f.proiettata({47.75, -10.5}), 3000.0);
    // una Kidd col sonar di scafo, senza schiera: col TACTAS una Perry sentiva tutti i Victor fino a 65 km dovunque
    // andasse, e l'esca non serviva a niente (giro lungo del 31/10/2026)
    UnitaScenario perry = unita(8, "kidd", "nave", "usa", acqua(f, polare(avanti, 90.0, 15000.0), 3000.0), 270.0, 10.0, "R_nave");
    perry.punti = {acqua(f, polare(avanti, 270.0, 25000.0), 3000.0), acqua(f, polare(avanti, 90.0, 25000.0), 3000.0)};
    perry.ciclica = true;
    perry.ruolo = "cacciatorpediniere sui datum";
    s.unita.push_back(perry);
    UnitaScenario la = unita(9, "los_angeles", "sottomarino", "usa", acqua(f, polare(avanti, 0.0, 12000.0), 3000.0), 30.0, 5.0, "");
    for (int q = 0; q < 4; ++q) la.punti.push_back(acqua(f, polare(avanti, 90.0 * q + 45.0, 14000.0), 3000.0));
    la.ciclica = true;
    la.quota = 200.0;
    la.ruolo = "SSN di barriera";
    s.unita.push_back(la);
    // un secondo SSN piu' a nord-ovest, sulle rotte dei Victor interni
    const Vec2 nord = acqua(f, f.proiettata({48.05, -10.9}), 2000.0);
    UnitaScenario la2 = unita(14, "los_angeles", "sottomarino", "usa", acqua(f, polare(nord, 90.0, 8000.0), 2000.0), 0.0, 5.0, "");
    for (int q = 0; q < 4; ++q) la2.punti.push_back(acqua(f, polare(nord, 90.0 * q, 9000.0), 2000.0));
    la2.ciclica = true;
    la2.quota = 180.0;
    la2.ruolo = "secondo SSN di barriera";
    s.unita.push_back(la2);
    const Vec2 settore = acqua(f, f.proiettata({47.3, -10.9}), 3000.0);
    UnitaScenario ohio = unita(10, "ohio", "sottomarino", "usa", acqua(f, polare(settore, 0.0, 6000.0), 3000.0), 90.0, 3.0, "");
    for (int q = 0; q < 4; ++q) ohio.punti.push_back(acqua(f, polare(settore, 90.0 * q + 90.0, 6000.0), 3000.0));
    ohio.ciclica = true;
    ohio.quota = 150.0;
    ohio.ruolo = "SSBN americano in pattuglia";
    s.unita.push_back(ohio);
    for (int k = 0; k < 2; ++k) {
        UnitaScenario m = unita(11 + k, k ? "portarinfuse" : "mercantile", "nave", "neutrale", acqua(f, f.proiettata({k ? 47.2 : 48.9, k ? -9.6 : -11.2}), 1000.0),
                                k ? 300.0 : 120.0, 13.0, "", true);
        m.punti = {acqua(f, polare(m.pos, k ? 300.0 : 120.0, 80000.0), 200.0)};
        m.ruolo = "traffico atlantico";
        s.unita.push_back(m);
    }
    s.unita.insert(s.unita.begin(), alfa(acqua(f, f.proiettata({48.45, -10.55}), 1500.0), ROTTA));
    s.unita.front().quota = 200.0;   // lento e profondo: chi resta nascosto lascia i Victor alla NATO
    s.unita.front().velocita = 4.0;
    regola(s, "partenza", {quando(C::tempo, 0.0)}, {imposta(9, "=", 1)});
    regola(s, "sosus_1", {quando(C::tempo, 1800.0)}, {azione(A::entra, 6), cue(polare(inizio[2], 230.0, MS * 1800.0), 8000.0)});
    regola(s, "sosus_2", {quando(C::tempo, 7200.0)}, {azione(A::entra, 7), cue(polare(inizio[1], 205.0, MS * 7200.0), 8000.0)});
    // tenuti: un cacciatore NATO ha accumulato 70 minuti di contatto sulla preda (con le zone di convergenza il «non sentito
    // da 20 minuti» non distingueva l'esca dal nascosto); a fine missione i liberi sono le prede mai tenute.
    // Lo scatto come esca non funziona, nemmeno con la regola che dirotta i Los Angeles (05/11/2026, docs/17 §LIB-S22)
    const int PREDE[] = {0, 2, 3, 4, 5};
    for (int k = 0; k < 5; ++k) {
        for (int cacciatore : {6, 7, 8, 9, 14})
            regola(s, "tenuto_" + std::to_string(PREDE[k]) + "_" + std::to_string(cacciatore), {su(C::unita_vede, cacciatore, 70.0, PREDE[k])},
                   {imposta(2 + k, "=", 1)});
        regola(s, "libero_" + std::to_string(PREDE[k]), {quando(C::tempo, 14100.0), variabile(2 + k, "=", 0.0)}, {imposta(7, "+", 1)});
    }
    for (int cacciatore : {6, 7, 8, 9, 14})
        regola(s, "alfa_tenuto_" + std::to_string(cacciatore), {su(C::unita_vede, cacciatore, 30.0, 0)}, {imposta(9, "=", 0)});
    regola(s, "lancio", {quando(C::giocatore_ha_lanciato)},
           {fine(42, {"Un'arma in acqua in tempo di pace: l'operazione e' finita, e da domani sara' un'altra storia.",
                      "A weapon in the water in peacetime: the operation is over, and tomorrow will be another story."}, "fallimento")});
    regola(s, "liberi", {quando(C::tempo, 14280.0), variabile(7, ">", 2.0)},
           {imposta(8, "=", 1), fine(40, {"Almeno tre battelli su cinque senza nessuno in coda. A Mosca diranno che la NATO non li ha visti; a Norfolk diranno il contrario.",
                                          "At least three boats out of five with nobody on their tail. Moscow will say NATO never saw them; Norfolk will say otherwise."})});
    regola(s, "ritrovati", {quando(C::tempo, 14280.0), variabile(7, "<", 3.0)},
           {fine(41, {"La NATO ne tiene troppi. Il rapporto dira' che l'operazione e' riuscita; chi era a bordo sa che non e' vero.",
                      "NATO is holding too many. The report will say the operation succeeded; those aboard know it did not."}, "fallimento")});
    s.obiettivi.push_back(obiettivo_variabile("liberi", 8, 1, {"Almeno tre battelli su cinque (Alfa compreso) senza inseguitore", "At least three boats out of five (Alfa included) with no pursuer"}));
    s.obiettivi.push_back(obiettivo("ohio", "scoprire", {10}, {"Trovare e classificare l'SSBN americano", "Find and classify the American SSBN"}, false));
    s.obiettivi.push_back(obiettivo_variabile("alfa_libero", 9, 1, {"L'Alfa tenuto meno di 30 minuti in tutto", "The Alfa held for less than 30 minutes in all"}, false));
    s.obiettivi.push_back(vivo());
    RadioScenario r;
    r.ordini = OrdiniScenario{"Comando della Flotta del Nord", "K-463 (PROGETTO 705)", "ATRINA", "SEGRETISSIMO"};
    r.roe = {{"sottomarini", "trattenuto"}, {"navi", "trattenuto"}, {"mercantili", "trattenuto"}, {"neutrali", "trattenuto"}, {"aerei", "trattenuto"}};
    s.radio = r;
    s.briefing[0] = {
        "ORDINI DI MISSIONE", "DA: Comando della Flotta del Nord", "A: K-463 (PROGETTO 705)", "OGG.: ATRINA",
        "20 aprile 1989, ore 0600Z. Teatro: Atlantico, a sud-ovest della Piattaforma Celtica. Pace.",
        "Cinque sottomarini d'attacco sono usciti insieme e hanno preso la rotta lunga, attorno alla Groenlandia, per non passare sopra il SOSUS. Ora siete nell'Atlantico aperto, su cinque rotte parallele verso sud-ovest.",
        "Il comando vuole sapere se la NATO vi ritrova, e se nel settore pattuglia un lanciamissili americano.",
        "Contano gli altri quattro quanto voi: almeno tre battelli su cinque devono arrivare in fondo senza nessuno in coda.",
        "Regole d'ingaggio: tempo di pace. Armi trattenute.", "Tempo a disposizione: 4 ore.",
        "Nota storica: ispirata all'operazione Atrina (marzo-maggio 1987), con cinque Victor III. Per gli autori russi un trionfo, per le fonti occidentali almeno quattro battelli su cinque furono seguiti. Nessun Alfa vi prese parte: qui l'Alfa ha il ruolo di uno di loro. Nomi e unita' di fantasia."};
    s.briefing[1] = {
        "MISSION ORDERS", "FM: NORTHERN FLEET HQ", "TO: K-463 (PROJECT 705)", "SUBJ: ATRINA",
        "20 April 1989, 0600Z. Theatre: Atlantic, south-west of the Celtic Shelf. Peace.",
        "Five attack submarines have sailed together and taken the long way round, around Greenland, to stay off the SOSUS. Now you are in the open Atlantic, on five parallel tracks heading south-west.",
        "The command wants to know whether NATO finds you again, and whether an American missile boat patrols the sector.",
        "The other four count as much as you do: at least three boats out of five must reach the end with nobody on their tail.",
        "Rules of engagement: peacetime. Weapons hold.", "Time available: 4 hours.",
        "Historical note: inspired by Operation Atrina (March-May 1987), with five Victor IIIs. For Russian authors a triumph; for Western sources at least four boats out of five were tracked. No Alfa took part: here the Alfa plays the role of one of them. Names and units are fictional."};
    return s;
}

// LIB-S23 «Acqua a due gradi» (docs/17 §2i, approvata il 31/10/2026): il soccorso ispirato alla Komsomolets (E27,
// 7 aprile 1989), nel Barents [SOST] al posto del Mare di Norvegia. Un battello sperimentale di fantasia (`akula` al
// posto del Mike) e' in superficie, in avaria, con un incendio a poppa; l'Alfa e' il piu' vicino, a 60 km. Il
// peschereccio-madre va a una posizione sbagliata di 15 km finche' l'Alfa, entro 5 km dal battello, non manda un
// rapporto di situazione: allora accosta e va a tutta. Alla terza ora l'equipaggio va in acqua; i salvati dipendono da
// quanto ci resta prima che arrivi una nave (40 se arriva prima, poi 35, 25, 15, 5: STIMA dalla sopravvivenza a 2 °C).
// Senza il rapporto l'Il-38 da' la posizione mezz'ora dopo, e il peschereccio arriva tardi, come nel 1989. A 2 ore e mezza la
// Norvegia offre una nave della guardia costiera: il giocatore accetta con una trasmissione di richiesta. Nessun nemico:
// l'Orion norvegese guarda. Il giocatore non vede affondare il battello (dossier 23 §5). Variabili: 2 Alfa arrivato in
// tempo, 3 posizione data, 4 l'Alfa vicino all'arrivo della nave, 5 i salvati, 9 tardi (90 minuti passati), 10 a mare,
// 11 secondi in acqua, 12 aiuto accettato, 13 l'Alfa entro 5 km adesso.
ScenarioMissione acqua_a_due_gradi(const DatiScenario& d) {
    const Fondale f = Fondale::carica(DATI + "/teatri/barents.fdl");
    ScenarioMissione s;
    base(s, d, f);
    s.id = "LIB_S23_ACQUA_A_DUE_GRADI";
    s.titolo = {"Acqua a due gradi", "Two-Degree Water"};
    s.teatro = "barents";
    s.alfa = true;
    s.giorno = 18;
    s.mese = 4;
    s.anno = 1989;
    s.ora_utc = 9.0;
    s.situazione = "pace";
    s.paranoia = 1;
    s.difficolta = 3;
    s.durata_s = 4.5 * 3600.0;
    carico_alfa(s);
    MeteoFissoScenario m;   // il mare grosso e le nubi basse del 7 aprile 1989
    m.stato_mare = 5.0, m.vento_m_s = 15.0, m.vento_da_gradi = 240.0, m.visibilita_m = 4000.0, m.nubi_ottavi = 8.0;
    s.meteo_fisso = m;
    const Vec2 luogo = acqua(f, f.proiettata({71.3, 35.0}), 150.0);
    UnitaScenario battello = unita(2, "akula", "sottomarino", "urss", luogo, 300.0, 0.0, "");
    battello.quota = 0.0;
    battello.stato = "avaria";
    battello.ruolo = "battello sperimentale in avaria, incendio a poppa (al posto del Mike)";
    s.unita.push_back(battello);
    for (int k = 0; k < 2; ++k) {   // gli Il-38 da Kola: girano sul battello
        UnitaScenario il = unita(3 + k, "il38", "aereo", "urss", polare(luogo, 180.0, 90000.0), 0.0, 300.0, "R_aereo");
        il.entra_con = k ? "il38_2" : "il38_1";
        il.ruolo = "Il-38 della Flotta del Nord";
        s.unita.push_back(il);
        AereoAswScenario a;
        a.unita = 3 + k;
        a.situazione = "pace";
        a.contro_giocatore = false;
        a.bersagli = {2};
        a.zona = luogo;
        a.raggio = 8000.0;
        s.aerei_asw.push_back(a);
    }
    const Vec2 sbagliato = acqua(f, polare(luogo, 340.0, 15000.0), 100.0);
    UnitaScenario bmrt = unita(5, "bmrt", "nave", "urss", acqua(f, polare(luogo, 120.0, 55000.0), 100.0), 320.0, 8.0, "", true);
    bmrt.punti = {sbagliato};
    bmrt.ruolo = "peschereccio-madre deviato sull'emergenza";
    s.unita.push_back(bmrt);
    // l'Orion norvegese guarda: un'orbita attorno al battello, con una voce ASW di pace sul solo battello (posa boe, al
    // piu' l'attacco simulato). Senza voce il motore gli dava la caccia di guerra predefinita, e neutrale le armi libere:
    // in tutti e due i casi tirava i Mk 46 in pace (giro lungo del 31/10/2026, segnalato al runtime)
    UnitaScenario orion = unita(6, "p3c", "aereo", "usa", polare(luogo, 270.0, 6000.0), 0.0, 220.0, "", true);
    for (int q = 0; q < 6; ++q) orion.punti.push_back(polare(luogo, 60.0 * q, 6000.0));
    orion.ciclica = true;
    orion.ruolo = "Orion norvegese che osserva";
    s.unita.push_back(orion);
    AereoAswScenario oss;
    oss.unita = 6;
    oss.situazione = "pace";
    oss.contro_giocatore = false;
    oss.bersagli = {2};
    oss.zona = luogo;
    oss.raggio = 10000.0;
    s.aerei_asw.push_back(oss);
    UnitaScenario kv = unita(7, "oslo", "nave", "neutrale", acqua(f, polare(luogo, 270.0, 22000.0), 100.0), 90.0, 18.0, "", true);   // col mare 5 fa circa 11 nodi: un'ora
    kv.punti = {luogo};
    kv.entra_con = "accetta";
    kv.ruolo = "nave della guardia costiera norvegese (al posto della Nordkapp)";
    // neutrale: della parte «usa» pedinerebbe il battello in superficie, a 5 nodi, come le navi NATO in pace
    s.unita.push_back(kv);
    s.unita.insert(s.unita.begin(), alfa(acqua(f, f.proiettata({70.75, 35.0}), 150.0), 0.0));
    s.unita.front().quota = 100.0;
    s.unita.front().velocita = 10.0;
    zona(s, "luogo", {"il battello in avaria", "the stricken boat"}, luogo, 600.0);
    const auto msg = [](TestiScenario t) {
        AzioneScenario x = azione(A::messaggio);
        x.testo = std::move(t);
        return x;
    };
    regola(s, "fermo", {quando(C::tempo, 0.0)}, {azione(A::comportamento, 2)});   // in avaria, senza comportamento
    regola(s, "resta_su", {variabile(10, "=", 0.0)}, {[] { AzioneScenario q = azione(A::quota, 2); q.valore = 0; return q; }()}, false);   // l'evasione dai sorvoli non lo porta giu'
    regola(s, "il38_1", {quando(C::tempo, 1800.0)}, {azione(A::entra, 3)});
    regola(s, "il38_2", {quando(C::tempo, 3600.0)}, {azione(A::entra, 4)});
    regola(s, "tardi", {quando(C::tempo, 5400.0)}, {imposta(9, "=", 1)});
    regola(s, "arrivato", {su(C::giocatore_vicino_a, 2, 2000.0), variabile(9, "=", 0.0)}, {imposta(2, "=", 1)});
    CondizioneScenario rapporto = quando(C::giocatore_ha_trasmesso, 0.0);
    rapporto.trasmissione = "situazione";
    regola(s, "posizione", {su(C::giocatore_vicino_a, 2, 5000.0), rapporto}, {imposta(3, "=", 1), vai(5, luogo, 14.0), manda("RICEVUTO")});
    regola(s, "si_appoppa", {quando(C::tempo, 10200.0)},
           {msg({"Sonar: dal battello aria e acqua, tanta. Si sta appoppando. Sentiamo le voci sull'idrofono.",
                 "Sonar: air and water from the boat, a lot of it. She is going down by the stern. We can hear voices on the hydrophone."})});
    regola(s, "a_mare", {quando(C::tempo, 10800.0)}, {imposta(10, "=", 1), azione(A::togli, 2), manda("A_MARE")});   // fuori dalla vista
    regola(s, "zattere", {quando(C::tempo, 11100.0)},
           {msg({"Gli Il-38 lanciano le zattere. Il vento le rovescia prima che qualcuno le raggiunga.",
                 "The Il-38s drop life rafts. The wind flips them before anyone can reach them."})});
    regola(s, "orologio", {variabile(10, "=", 1.0)}, {imposta(11, "+", 10)}, false);
    regola(s, "posizione_dall_aereo", {variabile(11, ">", 1790.0), variabile(3, "=", 0.0)}, {vai(5, luogo, 14.0), manda("IL38")});
    regola(s, "norvegia", {quando(C::tempo, 9000.0)}, {manda("NORVEGIA")});
    CondizioneScenario richiesta = quando(C::giocatore_ha_trasmesso, 9000.0);
    richiesta.trasmissione = "richiesta";
    regola(s, "accetta", {richiesta}, {imposta(12, "=", 1), azione(A::entra, 7), vai(7, luogo, 20.0), manda("ACCETTATO")});
    // l'arrivo di una nave: chi c'e' vicino, poi i salvati per i minuti in acqua
    // la 13 dice se l'Alfa e' entro 5 km adesso (azzerata e poi rimessa a ogni controllo: una regola non mescola unita')
    regola(s, "vicino_azzera", {quando(C::tempo, 0.0)}, {imposta(13, "=", 0)}, false);
    regola(s, "vicino_ora", {su(C::giocatore_vicino_a, 2, 5000.0)}, {imposta(13, "=", 1)}, false);
    // un codice d'esito per nave e fascia (un esito, un testo): il peschereccio 40 e 44-47, la nave norvegese 50-54; la
    // categoria la decidono gli obiettivi
    struct Fascia { const char* id; std::vector<CondizioneScenario> q; int salvati; const char* it; const char* en; };
    for (int nave : {5, 7}) {
        const std::string n = std::to_string(nave);
        const char* chi_it = nave == 5 ? "Il peschereccio" : "La nave norvegese";
        const char* chi_en = nave == 5 ? "The factory trawler" : "The Norwegian ship";
        regola(s, "alfa_vicino_" + n, {unita_in(nave, "luogo"), variabile(13, "=", 1.0)}, {imposta(4, "=", 1)});
        const std::vector<Fascia> fasce = {
            {"prima", {variabile(10, "=", 0.0)}, 40, " e' arrivato prima che il battello andasse giu': l'equipaggio e' passato a bordo. Tutti e quaranta.",
             " arrived before the boat went down: the crew stepped across. All forty."},
            {"35", {variabile(10, "=", 1.0), variabile(11, "<", 1210.0)}, 35, " li ha raccolti dopo pochi minuti in acqua: trentacinque uomini sono vivi.",
             " picked them up after a few minutes in the water: thirty-five men are alive."},
            {"25", {variabile(11, ">", 1200.0), variabile(11, "<", 2710.0)}, 25, " li ha raccolti dopo piu' di venti minuti in acqua a due gradi: venticinque uomini sono vivi.",
             " picked them up after more than twenty minutes in two-degree water: twenty-five men are alive."},
            {"15", {variabile(11, ">", 2700.0), variabile(11, "<", 4810.0)}, 15, " li ha raccolti dopo quasi un'ora: quindici uomini sono vivi.",
             " picked them up after almost an hour: fifteen men are alive."},
            {"5", {variabile(11, ">", 4800.0)}, 5, " e' arrivato troppo tardi per quasi tutti: cinque uomini sono vivi.",
             " arrived too late for almost all of them: five men are alive."}};
        for (std::size_t k = 0; k < fasce.size(); ++k) {
            const Fascia& x = fasce[k];
            std::vector<CondizioneScenario> q = {unita_in(nave, "luogo")};
            q.insert(q.end(), x.q.begin(), x.q.end());
            const int codice = nave == 5 ? (k ? 43 + static_cast<int>(k) : 40) : 50 + static_cast<int>(k);
            AzioneScenario salvati = azione(A::salvati);   // per l'achievement «tutti salvati» e la carriera
            salvati.valore = x.salvati;
            salvati.indice = 40;
            regola(s, "salvati_" + n + "_" + x.id, q,
                   {imposta(5, "=", x.salvati), salvati, fine(codice, {std::string(chi_it) + x.it, std::string(chi_en) + x.en})});
        }
    }
    s.obiettivi.push_back(obiettivo_variabile("arrivato", 2, 1, {"Arrivare entro 2 km dal battello in 90 minuti", "Reach 2 km from the boat within 90 minutes"}));
    s.obiettivi.push_back(obiettivo_variabile("posizione", 3, 1, {"Dare la posizione esatta al peschereccio (rapporto di situazione entro 5 km)", "Give the trawler the exact position (situation report within 5 km)"}));
    s.obiettivi.push_back(obiettivo_variabile("vicino", 4, 1, {"Essere entro 5 km quando arriva la nave", "Be within 5 km when the ship arrives"}, false));
    s.obiettivi.push_back(obiettivo_variabile("salvati", 5, 25, {"Gli uomini salvati (almeno 25)", "The men saved (at least 25)"}, false));
    s.obiettivi.push_back(vivo());
    RadioScenario r;
    r.ordini = OrdiniScenario{"Comando della Flotta del Nord", "K-463 (PROGETTO 705)", "SOCCORSO", "SEGRETO"};
    r.roe = {{"sottomarini", "trattenuto"}, {"navi", "trattenuto"}, {"mercantili", "trattenuto"}, {"neutrali", "trattenuto"}, {"aerei", "trattenuto"}};
    r.messaggi = {
        radio("RICEVUTO", -1.0, "flash", "Comando della Flotta del Nord", "POSIZIONE", "SEGRETO",
              {"POSIZIONE RICEVUTA. IL PESCHERECCIO-MADRE ACCOSTA E VA A TUTTA. RESTATE SUL POSTO.", "POSITION RECEIVED. THE FACTORY TRAWLER IS TURNING AND COMING AT FULL SPEED. STAY ON SCENE."}),
        radio("A_MARE", -1.0, "flash", "Comando della Flotta del Nord", "EQUIPAGGIO IN ACQUA", "SEGRETO",
              {"IL BATTELLO E' AFFONDATO. L'EQUIPAGGIO E' IN ACQUA. OGNI MINUTO CONTA.", "THE BOAT HAS SUNK. THE CREW IS IN THE WATER. EVERY MINUTE COUNTS."}),
        radio("IL38", -1.0, "flash", "Comando della Flotta del Nord", "POSIZIONE DALL'AEREO", "SEGRETO",
              {"L'IL-38 HA DATO LA POSIZIONE. IL PESCHERECCIO-MADRE ACCOSTA.", "THE IL-38 HAS GIVEN THE POSITION. THE FACTORY TRAWLER IS TURNING."}),
        radio("NORVEGIA", -1.0, "immediate", "Guardia costiera norvegese", "OFFERTA DI ASSISTENZA", "NON CLASSIFICATO",
              {"SULLA FREQUENZA DI SOCCORSO: UNA NAVE DELLA GUARDIA COSTIERA NORVEGESE PUO' ESSERE SUL POSTO IN UN'ORA. SE LA VOLETE, CHIEDETELA.",
               "ON THE DISTRESS FREQUENCY: A NORWEGIAN COAST GUARD SHIP CAN BE ON SCENE IN AN HOUR. IF YOU WANT IT, ASK FOR IT."}, {}, false),
        radio("ACCETTATO", -1.0, "immediate", "Guardia costiera norvegese", "IN ARRIVO", "NON CLASSIFICATO",
              {"RICEVUTO. LA NAVE E' IN ARRIVO A TUTTA FORZA.", "ROGER. THE SHIP IS COMING AT FULL SPEED."}, {}, false)};
    s.radio = r;
    s.briefing[0] = {
        "ORDINI DI MISSIONE", "DA: Comando della Flotta del Nord", "A: K-463 (PROGETTO 705)", "OGG.: SOCCORSO",
        "18 aprile 1989, ore 0900Z. Teatro: Mare di Barents. Pace. Mare 5, nubi basse, acqua a due gradi.",
        "Un battello sperimentale ha un incendio a poppa ed e' emerso con l'aria d'emergenza. Il reattore e' fermo, il fuoco continua. Siete il battello piu' vicino, a 60 km a sud.",
        "Le navi sono lontane: un peschereccio-madre e' stato deviato, ma ha una posizione sbagliata. Arrivate sul posto, avvicinatevi entro 5 km e mandate un rapporto di situazione: la posizione esatta gli fara' guadagnare un'ora.",
        "Due Il-38 decollano da Kola. Un aereo norvegese e' gia' in volo sulla zona.",
        "Regole d'ingaggio: tempo di pace. Armi trattenute. Tempo a disposizione: 4 ore e 30.",
        "Nota storica: ispirata alla K-278 Komsomolets, che il 7 aprile 1989 emerse in fiamme a sud-ovest dell'Isola dell'Orso e affondo' circa quattro ore dopo. Le zattere lanciate dagli aerei furono inutilizzabili; il peschereccio Aleksej Chlobystov arrivo' 81 minuti dopo l'affondamento. Si salvarono 27 uomini su 69. Un Orion norvegese guardava; il comando non chiese aiuto alla Norvegia. Qui battello e uomini sono di fantasia; l'Alfa ha il ruolo dei soccorritori."};
    s.briefing[1] = {
        "MISSION ORDERS", "FM: NORTHERN FLEET HQ", "TO: K-463 (PROJECT 705)", "SUBJ: RESCUE",
        "18 April 1989, 0900Z. Theatre: Barents Sea. Peace. Sea state 5, low cloud, water at two degrees.",
        "An experimental boat has a fire aft and has surfaced on emergency air. Her reactor is shut down, the fire goes on. You are the nearest boat, 60 km to the south.",
        "The ships are far away: a factory trawler has been diverted, but she has a wrong position. Get on scene, close within 5 km and send a situation report: the exact position will gain her an hour.",
        "Two Il-38s are taking off from Kola. A Norwegian aircraft is already over the area.",
        "Rules of engagement: peacetime. Weapons hold. Time available: 4 hours 30.",
        "Historical note: inspired by K-278 Komsomolets, which on 7 April 1989 surfaced on fire south-west of Bear Island and sank about four hours later. The rafts dropped by the aircraft were unusable; the trawler Aleksey Khlobystov arrived 81 minutes after the sinking. 27 men of 69 survived. A Norwegian Orion was watching; the command did not ask Norway for help. Here the boat and the men are fictional; the Alfa plays the rescuers."};
    return s;
}

// LIB-S24 «Quota zero» (docs/17 §2i, approvata il 31/10/2026): il Tautog e il K-108 (E06, 1970) visti dal pedinato.
// L'Alfa, per ruolo al posto dell'Echo II, fa un'esercitazione d'attacco su una nave bersaglio nelle acque profonde a
// sud-est delle Curili [SOST: al posto di Petropavlovsk-Kamcatskij]; uno Sturgeon lo pedina da vicino (R_pedina_vicino,
// col m688.pedina_vicino del runtime) a 400 m nel cono sordo, a 20 m di quota dall'Alfa, e ne segue le manovre con
// ritardo. Il sonar passivo da' il rilevamento e non la quota: col Crazy Ivan l'Alfa lo sente, ma un'accostata o un
// arresto brusco possono portare all'urto (esito 41). Classificato (a mano o dal sonarista) e pingato, lo Sturgeon se ne va;
// quando l'Alfa non lo sente da 15 minuti e il siluro d'esercitazione ha trovato la nave bersaglio, la missione chiude
// (esito 40). Variabili: 2 classificato, 3 pingato dopo, 4 allontanato, 5 riferito. Semplificazioni (docs/17): i tre passaggi
// della scheda sono un attacco d'esercitazione riuscito (l'obiettivo «colpire» conta le unita').
ScenarioMissione quota_zero(const DatiScenario& d) {
    const Fondale f = Fondale::carica(DATI + "/teatri/kunashir.fdl");
    ScenarioMissione s;
    base(s, d, f);
    s.id = "LIB_S24_QUOTA_ZERO";
    s.titolo = {"Quota zero", "Zero Depth"};
    s.teatro = "kunashir";
    s.alfa = true;
    s.giorno = 20;
    s.mese = 6;
    s.anno = 1989;
    s.ora_utc = 2.0;
    s.situazione = "pace";
    s.esercitazione = true;   // le armi della missione sono inerti: il siluro d'esercitazione sulla nave bersaglio
    s.paranoia = 2;
    s.difficolta = 3;
    s.durata_s = 2.0 * 3600.0;
    s.meteo_realistico = true;
    carico_alfa(s);
    const Vec2 area = acqua(f, f.proiettata({43.5, 147.65}), 2000.0);
    // la nave bersaglio: un vecchio mercantile senza sonar antisom (una Krivak col Vega trovava da se' lo Sturgeon e faceva
    // il lavoro del giocatore)
    UnitaScenario krivak = unita(2, "mercantile", "nave", "urss", acqua(f, polare(area, 0.0, 5000.0), 1500.0), 90.0, 10.0, "", true);
    for (int q = 0; q < 4; ++q) krivak.punti.push_back(acqua(f, polare(area, 90.0 * q + 90.0, 5000.0), 1500.0));
    krivak.ciclica = true;
    krivak.ruolo = "nave bersaglio dell'esercitazione (un vecchio mercantile)";
    s.unita.push_back(krivak);
    const Vec2 partenza = acqua(f, polare(area, 270.0, 18000.0), 1000.0);
    UnitaScenario sturgeon = unita(3, "sturgeon", "sottomarino", "usa", acqua(f, polare(partenza, 270.0, 1500.0), 1000.0), 90.0, 8.0, "R_pedina_vicino");
    sturgeon.quota = 100.0;   // la quota dell'Alfa: poi il pedinamento ravvicinato lo tiene 20 m sotto quella che vede
    sturgeon.parametri.distanza_nm = 0.22;   // 400 m nel cono sordo (m688.pedina_vicino del runtime)
    sturgeon.parametri.paranoia = 1;
    sturgeon.ruolo = "SSN americano che pedina l'esercitazione";
    s.unita.push_back(sturgeon);
    for (int k = 0; k < 2; ++k) {
        UnitaScenario p = unita(4 + k, "peschereccio", "nave", "neutrale", acqua(f, polare(area, 30.0 + 150.0 * k, 22000.0), 500.0), 120.0 * k, 4.0, "", true);
        p.punti = {acqua(f, polare(p.pos, 120.0 * k + 60.0, 20000.0), 500.0)};
        p.ruolo = "peschereccio";
        s.unita.push_back(p);
    }
    s.unita.insert(s.unita.begin(), alfa(partenza, 90.0));
    s.unita.front().quota = 100.0;
    s.unita.front().velocita = 8.0;
    regola(s, "collisione", {evento("collisione")},
           {fine(41, {"Un urto sotto la falsa torre, poi un'elica che gira libera. Il contatto si e' spezzato. Rientrate.",
                      "An impact under the sail, then a screw running free. The contact has broken off. Return to base."}, "fallimento")});
    regola(s, "classificato", {compiuto("pedinatore")}, {imposta(2, "=", 1)});
    regola(s, "pingato", {variabile(2, "=", 1.0), evento("ping_attivo")}, {imposta(3, "=", 1)});
    regola(s, "allontanato", {variabile(3, "=", 1.0), su(C::senza_contatto_su, 3, 15.0)},
           {imposta(4, "=", 1), [] {
                AzioneScenario m = azione(A::messaggio);
                m.testo = {"Sonar: da un quarto d'ora niente piu' dal contatto americano. Se n'e' andato.",
                           "Sonar: nothing from the American contact for a quarter of an hour. She has gone."};
                return m;
            }()});
    regola(s, "riferito", {variabile(2, "=", 1.0), quando(C::giocatore_ha_trasmesso, 0.0)}, {imposta(5, "=", 1)});
    regola(s, "finita", {compiuto("esercizio"), variabile(4, "=", 1.0)},
           {fine(40, {"Esercitazione conclusa, e il pedinatore e' stato allontanato senza toccarlo.",
                      "Exercise complete, and the trailer was driven off without touching her."})});
    s.obiettivi.push_back(obiettivo("esercizio", "colpire", {2}, {"Il siluro d'esercitazione sulla nave bersaglio", "The exercise torpedo on the target ship"}));
    s.obiettivi.push_back(obiettivo("pedinatore", "scoprire", {3}, {"Classificare il pedinatore", "Classify the trailer"}));   // a mano o dal sonarista
    ObiettivoScenario sentito = obiettivo("sentito", "contatto", {3}, {"Il primo passo: sentirlo, due minuti di contatto", "First step: hear her, two minutes of contact"}, false);
    sentito.minuti = 2.0;
    s.obiettivi.push_back(sentito);
    s.obiettivi.push_back(obiettivo_variabile("allontanato", 4, 1, {"Allontanarlo col sonar attivo, poi perderlo per 15 minuti", "Drive it off with active sonar, then lose it for 15 minutes"}, false));
    s.obiettivi.push_back(obiettivo_variabile("riferito", 5, 1, {"Riferire la classificazione al comando", "Report the classification to command"}, false));
    s.obiettivi.push_back(vivo());
    RadioScenario r;
    r.ordini = OrdiniScenario{"Comando della Flotta del Pacifico", "K-463 (PROGETTO 705)", "ESERCITAZIONE", "SEGRETO"};
    r.roe = {{"sottomarini", "trattenuto"}, {"navi", "trattenuto"}, {"mercantili", "trattenuto"}, {"neutrali", "trattenuto"}, {"aerei", "trattenuto"}};
    s.radio = r;
    s.briefing[0] = {
        "ORDINI DI MISSIONE", "DA: Comando della Flotta del Pacifico", "A: K-463 (PROGETTO 705)", "OGG.: ESERCITAZIONE",
        "20 giugno 1989, ore 0200Z. Teatro: Pacifico, a sud-est delle Curili. Pace. Nebbia d'estate.",
        "Esercitazione d'attacco: un siluro d'esercitazione sulla nave bersaglio, un vecchio mercantile che gira 18 km a est. Armi inerti.",
        "Il comando sospetta che un sottomarino americano segua le unita' in esercitazione. Se c'e', sta nel vostro cono sordo: guardatevi la poppa, sentitelo, classificatelo, riferitelo e allontanatelo col sonar attivo. Il sonar vi dara' il rilevamento, non la quota.",
        "Nessun contatto fisico. Regole d'ingaggio: tempo di pace. Tempo a disposizione: 2 ore.",
        "Nota storica: ispirata al Tautog e al K-108 (20 giugno 1970, al largo di Petropavlovsk-Kamcatskij). Il sottomarino americano pedinava l'Echo II senza poterne sentire la quota; il K-108 gli urto' la falsa torre, perse un'elica e rientro'. Nessun morto; i due paesi negarono, il fatto usci' nel 1991. Qui l'Alfa ha il ruolo del K-108. Nomi e unita' di fantasia."};
    s.briefing[1] = {
        "MISSION ORDERS", "FM: PACIFIC FLEET HQ", "TO: K-463 (PROJECT 705)", "SUBJ: EXERCISE",
        "20 June 1989, 0200Z. Theatre: Pacific, south-east of the Kurils. Peace. Summer fog.",
        "Attack exercise: one exercise torpedo on the target ship, an old freighter circling 18 km to the east. Weapons inert.",
        "The command suspects an American submarine is trailing the exercise units. If she is there, she is in your baffles: clear your stern, hear her, classify her, report her and drive her off with active sonar. Sonar will give you her bearing, not her depth.",
        "No physical contact. Rules of engagement: peacetime. Time available: 2 hours.",
        "Historical note: inspired by Tautog and K-108 (20 June 1970, off Petropavlovsk-Kamchatsky). The American submarine was trailing the Echo II without being able to hear her depth; K-108 struck her sail, lost a screw and returned to port. Nobody died; both countries denied it, and the story came out in 1991. Here the Alfa plays K-108. Names and units are fictional."};
    return s;
}

// Il motore tratta gli id 0 e 1 come i due giocatori dell'originale (missione.cpp: incagli, esito 4 se il secondo e'
// distrutto): le unita' dell'IA si spostano da 1 in su di uno, con tutti i riferimenti (obiettivi, aerei, regole)
void senza_id_1(ScenarioMissione& s) {
    const auto sposta = [](int& id) { if (id >= 1) ++id; };
    for (auto& u : s.unita)
        if (!u.giocatore) sposta(u.id);
    for (auto& o : s.obiettivi)
        for (int& id : o.unita) sposta(id);
    for (auto& a : s.aerei_asw) {
        sposta(a.unita);
        sposta(a.base_unita);
        for (int& id : a.bersagli) sposta(id);
    }
    for (auto& r : s.regole) {
        for (auto& c : r.quando) {
            sposta(c.unita);
            sposta(c.bersaglio);
        }
        for (auto& a : r.allora) {
            sposta(a.unita);
            sposta(a.bersaglio);
        }
    }
    for (int& id : s.chiusura.unita) sposta(id);
    if (s.radio)
        for (auto& m : s.radio->messaggi) {
            for (int& id : m.se_unita_colpita) sposta(id);
            for (int& id : m.contatto) sposta(id);
        }
}

}  // namespace

int main(int argc, char** argv) {
    try {
        fs::path uscita = M688_LIBRERIA;
        for (int i = 1; i < argc; ++i)
            if (std::string(argv[i]) == "--uscita" && i + 1 < argc) uscita = argv[++i];
        const DatiScenario d = carica_dati_scenario(DATI);
        fs::create_directories(uscita);
        const std::vector<ScenarioMissione> tutte{ombra_lunga(d), corsa_al_relitto(d), vestfjord(d), nave_mineraria(d), campane_d_edera(d), tappo(d), valona(d),
                              aport(d), tenete_il_confine(d), gruppo_cacciatore(d), balena_di_ferro(d), pescatore_di_teriberka(d),
                              ponte_di_ekaterina(d), sotto_il_bastione(d), guardiano_del_cavo(d), quarantena(d), petroliere(d),
                              cavo_della_fregata(d), porta_di_ercole(d), lupo_ferito(d), outlaw_shark(d), disertore(d), acque_basse(d), finestra_di_lancio(d),
                              primo_eco(d), sagome_nello_stretto(d, false), sagome_nello_stretto(d, true),
                              sagome_nello_stretto(d, false, true), bastione_bianco(d), ancoraggio(d), pedinatore_in_attesa(d), scala_norvegese(d), soglia_di_ekaterina(d), sotto_la_prora(d), arcipelago(d), pietra_santa(d), cinquecento_iarde(d), ombra_del_boomer(d), rotta_lunga(d), acqua_a_due_gradi(d), quota_zero(d)};
        for (auto s : tutte) {
            senza_id_1(s);
            const Fondale f = Fondale::carica(DATI + "/teatri/" + s.teatro + ".fdl");
            int errori = 0;
            for (const auto& a : verifica(s, f, d)) {
                std::printf("  %s %s: %s\n", s.id.c_str(), a.gravita == AvvisoScenario::Gravita::errore ? "ERRORE" : "attenzione", a.testo.c_str());
                errori += a.gravita == AvvisoScenario::Gravita::errore;
            }
            if (errori) throw std::runtime_error(s.id + ": lo scenario ha errori");
            std::ofstream(uscita / (s.id + ".lua"), std::ios::binary) << scrivi_missione_lua(s, d);
            std::ofstream(uscita / (s.id + ".m688m"), std::ios::binary) << esporta_pacchetto(s, d, DATI, CREATA);
            std::printf("%s: %s\n", s.id.c_str(), s.titolo[0].c_str());
        }
        return 0;
    } catch (const std::exception& e) {
        std::fprintf(stderr, "libreria_missioni: %s\n", e.what());
        return 2;
    }
}
