// Le scene del dossier 24 §8.9 come missioni di prova (roadmap 31, piano docs/18_interazioni_ia.md, fase 5): le
// interazioni fra le unita' dell'IA che il giocatore puo' vedere, una missione per scena, scritte con lo scrittore
// dell'editor (missioni/scenario_missione) in data/missioni/prove. Servono a tarare le reazioni dell'IA con partite su
// piu' semi e come prove di regressione delle interazioni.
//
//   scene_interazioni [--uscita CARTELLA]                         scrive le missioni (predefinita data/missioni/prove)
//   scene_interazioni --file F [--file G ...] [--semi N] [--secondi S]   gioca file di missione qualunque (il collaudo
//                                                                 della libreria), con lo stesso riassunto
//   scene_interazioni --apertura                                  le due scene del video d'apertura in data/motore/apertura
//   scene_interazioni --semi N [--secondi S] [--scena ID]         le gioca nell'Enhanced coi semi 1..N (per la durata
//                                                                 della scena, o S secondi) e riassume: chi vede chi fra
//                                                                 parti ostili e quando (contatti che l'IA puo' usare:
//                                                                 traccia solida o posizione), armi in acqua, chi e'
//                                                                 colpito o esce; il giocatore resta fermo ad ascoltare
//
// Molte reazioni del dossier non esistono ancora (le fa il runtime nelle fasi 2-4: allerta, messaggi fra unita', zone,
// tabelle di reazione): le scene usano quello che c'e' (comportamenti, ASW aereo, radio della missione, regole) e
// portano le attese del dossier nel commento di ogni scena e nelle prove marcate «da completare»
// (test/prove_interazioni_scene.cpp). Si aggiornano man mano che arrivano le funzioni.
//
// In piu', le scene della stazione di tiro dei missili (docs/21_stazione_missili.md §11.13), accese dal 28/10/2026 con la
// stazione cablata: nella scelta delle missioni e fra i vetrini (in ritaratura finche' il comandante IA non tira i
// missili). La navigazione incerta, la libreria `tlam` e l'ordine d'esecuzione li scrive lo scrittore dell'editor.
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <functional>
#include <map>
#include <memory>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "missioni/danni_missione.hpp"
#include "missioni/danni_sottomarini.hpp"
#include "missioni/missione.hpp"
#include "missioni/registro_missione.hpp"
#include "missioni/scenario_missione.hpp"
#include "nucleo/campo_ghiaccio.hpp"
#include "nucleo/classi.hpp"
#include "nucleo/fondale.hpp"
#include "nucleo/geometria.hpp"

#include "../app/data_missione.hpp"

using namespace m688;
namespace fs = std::filesystem;

namespace {

const std::string DATI = M688_DATI;
using C = CondizioneScenario::Tipo;
using A = AzioneScenario::Tipo;

// --- aiuti (gli stessi della libreria, strumenti/libreria_missioni.cpp) ---

Vec2 polare(Vec2 c, double rotta, double d) { return {c.x + d * std::sin(radianti(rotta)), c.y + d * std::cos(radianti(rotta))}; }

Vec2 acqua(const Fondale& f, Vec2 p, double minima) {   // il punto d'acqua piu' vicino a p con almeno `minima` di fondale
    for (int raggio = 0; raggio < 120; ++raggio)
        for (int dy = -raggio; dy <= raggio; ++dy)
            for (int dx = -raggio; dx <= raggio; ++dx) {
                if (std::max(std::abs(dx), std::abs(dy)) != raggio) continue;
                const Vec2 q{p.x + dx * 500.0, p.y + dy * 500.0};
                if (f.dentro(q) && f.profondita(q) >= minima) return q;
            }
    throw std::runtime_error("nessun punto d'acqua vicino");
}

UnitaScenario unita(int id, const std::string& classe, const std::string& categoria, const std::string& parte, Vec2 pos, double rotta,
                    double velocita, const std::string& comportamento, bool civile = false) {
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

UnitaScenario giocatore(bool alfa, Vec2 pos, double rotta, double quota) {
    UnitaScenario g = unita(0, alfa ? "alfa" : "los_angeles", "sottomarino", alfa ? "urss" : "usa", pos, rotta, 5.0, "");
    g.giocatore = true;
    g.flag = alfa ? 0x11 : 0x09;
    g.quota = quota;
    return g;
}

void base(ScenarioMissione& s, const DatiScenario& d, const Fondale& f, const std::string& id, const std::string& teatro, bool alfa) {
    s.libreria = 2;
    s.autore = "Prove delle interazioni (docs/18)";
    s.esito_fine = d.esito_fine;
    s.esito_tempo = d.esito_tempo;
    s.centro_lat = f.centro().lat;
    s.centro_lon = f.centro().lon;
    s.id = id;
    s.teatro = teatro;
    s.alfa = alfa;
    s.anno = 1989;
    s.paranoia = 2;
    s.difficolta = 3;
    s.durata_s = 7200.0;
    if (alfa) {
        s.carico.dotazione = {{"type53", 18}, {"esca", 4}};
        s.carico.deposito = {{"type53", 18}, {"esca", 4}, {"esca_mobile", 2}};
        s.carico.tubi = {"type53", "type53", "type53", "type53", "type53", "esca", "esca"};
    } else {
        s.carico.dotazione = {{"mk48", 22}, {"harpoon", 2}, {"esca", 6}};
        s.carico.deposito = {{"mk48", 21}, {"harpoon", 2}, {"esca", 6}};
        s.carico.tubi = {"mk48", "mk48", "mk48", "mk48", "esca", "esca", "harpoon"};
    }
    ObiettivoScenario vivo;
    vivo.id = "vivo";
    vivo.tipo = "sopravvivere";
    vivo.testo = {"Riportare a casa il battello", "Bring the boat home"};
    s.obiettivi.push_back(vivo);
}

void meteo(ScenarioMissione& s, double mare) {
    MeteoFissoScenario m;
    m.stato_mare = mare;
    m.visibilita_m = 15000.0;
    s.meteo_fisso = m;
}

AereoAswScenario aereo(int u, int base_unita, std::optional<Vec2> base_aerea, const std::string& situazione, bool contro_giocatore,
                       std::vector<int> bersagli, Vec2 zona, double raggio) {
    AereoAswScenario a;
    a.unita = u;
    a.base_unita = base_unita;
    a.base = base_aerea;
    a.situazione = situazione;
    a.contro_giocatore = contro_giocatore;
    a.bersagli = std::move(bersagli);
    a.zona = zona;
    a.raggio = raggio;
    return a;
}

CondizioneScenario quando(C tipo, double valore = 0.0) {
    CondizioneScenario c;
    c.tipo = tipo;
    c.valore = valore;
    return c;
}
CondizioneScenario su(C tipo, int u, double valore = 0.0) {
    CondizioneScenario c = quando(tipo, valore);
    c.unita = u;
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
AzioneScenario cue(Vec2 p, double raggio) {
    AzioneScenario a = azione(A::cue_asw);
    a.punto = p;
    a.raggio = raggio;
    return a;
}
AzioneScenario manda(const std::string& id) {
    AzioneScenario a = azione(A::radio);
    a.zona = id;
    return a;
}
void regola(ScenarioMissione& s, const std::string& id, std::vector<CondizioneScenario> q, std::vector<AzioneScenario> az) {
    RegolaScenario r;
    r.id = id;
    r.quando = std::move(q);
    r.allora = std::move(az);
    s.regole.push_back(r);
}
void zona(ScenarioMissione& s, const std::string& id, TestiScenario nome, Vec2 c, double r) { s.zone.push_back({id, std::move(nome), {c.x, c.y, r}}); }

MessaggioScenario radio(const std::string& id, double dopo_s, const std::string& precedenza, const std::string& da, const std::string& oggetto,
                        TestiScenario testo, RoeScenario roe = {}) {
    MessaggioScenario m;
    m.id = id;
    m.su_richiesta = dopo_s < 0.0;
    m.dopo_s = std::max(0.0, dopo_s);
    m.precedenza = precedenza;
    m.da = da;
    m.oggetto = oggetto;
    m.classifica = "SECRET";
    m.testo = std::move(testo);
    m.roe = std::move(roe);
    return m;
}

RoeScenario roe_tutte(const std::string& stato) {
    return {{"sottomarini", stato}, {"navi", stato}, {"mercantili", "trattenuto"}, {"neutrali", "trattenuto"}, {"aerei", "trattenuto"}};
}

// --- le scene ---------------------------------------------------------------------------------------------------------
// Gli id delle unita' partono da 2 (il motore tratta 0 e 1 come i due giocatori dell'originale).

// S1 — Un SSN alleato scoperto da un KPUG sovietico (Barents, crisi, mare 3, notte). Attese del dossier (24 §8.9):
// t=0 il Krivak di destra ha un contatto sul VDS a 6 km, allerta A1 -> A2; t=1-3 min contatto agli altri due, il gruppo
// accosta a 30 gradi dal rilevamento e rallenta a 8 kn; t=5-15 min i Ka-27 posano boe in circolo (di notte non
// immergono); t=15 min PROBSUB, FLASH alla flotta, attivo ogni 20-30 s; lo Sturgeon scende sotto lo strato e si
// allontana a 4-6 kn; il giocatore sente i ping lontani; t=30 min il broadcast americano avvisa il giocatore; t=60-120
// min se lo Sturgeon esce dal campo il KPUG lo perde (P 0,6). In guerra (variante S1G): attacco SS-N-14 o AT-1 a ~20
// min e allerta A3 in tutto il teatro sovietico.
// Oggi: allerta per parte, contatti passati fra le navi, boe dei Ka-27 su un alleato, ping «tenere sotto» e A3 di teatro
// non ci sono (fasi 2-4); il broadcast e' la radio della missione.
ScenarioMissione s1(const DatiScenario& d, bool guerra) {
    const Fondale f = Fondale::carica(DATI + "/teatri/barents.fdl");
    ScenarioMissione s;
    base(s, d, f, guerra ? "PROVA_S1G_KPUG_GUERRA" : "PROVA_S1_KPUG", "barents", false);
    s.titolo = guerra ? TestiScenario{"S1G - KPUG e SSN alleato (guerra)", "S1G - KPUG and allied SSN (war)"}
                      : TestiScenario{"S1 - KPUG e SSN alleato", "S1 - KPUG and allied SSN"};
    s.giorno = 14, s.mese = 1, s.ora_utc = 22.0;
    s.situazione = guerra ? "guerra" : "crisi";
    meteo(s, 3.0);
    const Vec2 dat = acqua(f, f.proiettata({71.0, 36.0}), 150.0);
    s.unita.push_back(giocatore(false, acqua(f, polare(dat, 270.0, 15.0 * MIGLIO), 150.0), 90.0, 100.0));
    UnitaScenario sturgeon = unita(2, "sturgeon", "sottomarino", "usa", dat, 270.0, 5.0, "R_sottomarino");
    sturgeon.quota = 90.0;
    sturgeon.profonda = 150.0;
    sturgeon.parametri.salve = 0;   // non attacca: deve uscirne
    sturgeon.punti = {acqua(f, polare(dat, 270.0, 30000.0), 150.0)};
    sturgeon.ruolo = "SSN alleato";
    s.unita.push_back(sturgeon);
    const Vec2 kpug = polare(dat, 90.0, 6000.0);
    const char* const NAVI[] = {"krivak2", "krivak2", "kara"};   // il Kara al posto dell'Udaloy (classe che manca)
    for (int k = 0; k < 3; ++k) {
        UnitaScenario n = unita(3 + k, NAVI[k], "nave", "urss", acqua(f, polare(kpug, k == 0 ? 0.0 : k == 1 ? 340.0 : 200.0, k ? 2000.0 : 0.0), 60.0),
                                270.0, 12.0, "R_nave");
        n.punti = {acqua(f, polare(n.pos, 270.0, 40000.0), 60.0)};
        n.ruolo = k == 0 ? "Krivak di destra (VDS)" : k == 1 ? "Krivak" : "capogruppo (al posto dell'Udaloy)";
        s.unita.push_back(n);
    }
    for (int k = 0; k < 2; ++k) {
        UnitaScenario e = unita(6 + k, "ka27", "aereo", "urss", polare(kpug, 180.0, 1500.0), 270.0, 0.0, "R_aereo");
        e.entra_con = "elicotteri";
        e.ruolo = "Ka-27 del capogruppo";
        s.unita.push_back(e);
        s.aerei_asw.push_back(aereo(6 + k, 5, std::nullopt, s.situazione, false, {2}, dat, 15000.0));
    }
    regola(s, "elicotteri", {quando(C::tempo, 300.0)}, {azione(A::entra, 6), azione(A::entra, 7), cue(dat, 6000.0)});
    RadioScenario r;
    r.ordini = OrdiniScenario{"COMSUBLANT", "USS LOS ANGELES (SSN 688)", "AREA PATROL", "SECRET"};
    r.roe = roe_tutte(guerra ? "libero" : "trattenuto");
    r.messaggi = {radio("AMICO", 1800.0, "immediate", "COMSUBLANT", "FRIENDLY SSN",
                        {"UNITA' AMICA IN CONTATTO CON FORZE ASW SOVIETICHE A EST DELLA VOSTRA AREA. NON AVVICINARSI.",
                         "FRIENDLY UNIT IN CONTACT WITH SOVIET ASW FORCES EAST OF YOUR AREA. DO NOT CLOSE."})};
    s.radio = r;
    s.briefing[0] = {"PROVA S1", "Uno Sturgeon alleato pattuglia a 15 miglia a est. Un gruppo ASW sovietico lo trova."};
    s.briefing[1] = {"TEST S1", "An allied Sturgeon patrols 15 miles east. A Soviet ASW group finds her."};
    return s;
}

// S2 — Una fregata amica pedina un Victor III (Mar di Norvegia, pace, mare 2, giorno). Attese: t=0 la Perry col TACTAS
// ha il contatto in seconda zona di convergenza (~55 nm), allerta A1; t=5-30 min l'SH-60B posa 4 DIFAR, il contatto va
// in Link 11 al P-3 che arriva dopo 30-60 min; t=60 min Crazy Ivan del Victor, il P-3 rifa' il campo davanti; t=90 min
// la Perry a 5 km fa due ping «ti tengo», il Victor accelera, cambia quota, lancia un'esca; nessun attacco, rapporto
// PRIORITY; il giocatore riceve il contatto al broadcast e puo' prendere il pedinamento.
// Oggi: c'e' il Crazy Ivan (Enhanced), il pedinamento della Perry (R_pedina a 2,7 nm) e il P-3 che entra; il Link 11, i
// ping «ti tengo» e l'esca del Victor in pace no.
ScenarioMissione s2(const DatiScenario& d) {
    const Fondale f = Fondale::carica(DATI + "/teatri/faroe.fdl");
    ScenarioMissione s;
    base(s, d, f, "PROVA_S2_PEDINAMENTO", "faroe", false);
    s.titolo = {"S2 - Una Perry pedina un Victor III", "S2 - A Perry trails a Victor III"};
    s.giorno = 10, s.mese = 5, s.ora_utc = 10.0;
    s.situazione = "pace";
    meteo(s, 2.0);
    const Vec2 v0 = acqua(f, f.proiettata({62.8, -6.5}), 800.0);
    UnitaScenario victor = unita(2, "victor3", "sottomarino", "urss", v0, 45.0, 8.0, "R_sottomarino");
    victor.quota = 150.0;
    victor.profonda = 250.0;
    victor.punti = {acqua(f, polare(v0, 45.0, 80000.0), 500.0)};
    victor.ruolo = "il pedinato";
    s.unita.push_back(victor);
    UnitaScenario perry = unita(3, "perry", "nave", "usa", acqua(f, polare(v0, 225.0, 30000.0), 300.0), 45.0, 15.0, "R_pedina");
    perry.parametri.distanza_nm = 2.7;   // 5 km
    perry.ruolo = "fregata col TACTAS";
    s.unita.push_back(perry);
    s.unita.insert(s.unita.begin(), giocatore(false, acqua(f, polare(v0, 160.0, 20.0 * MIGLIO), 500.0), 45.0, 120.0));
    UnitaScenario elicottero = unita(4, "sh60b", "aereo", "usa", polare(perry.pos, 0.0, 500.0), 45.0, 0.0, "R_aereo");
    elicottero.ruolo = "SH-60B della Perry";
    s.unita.push_back(elicottero);
    UnitaScenario orion = unita(5, "p3c", "aereo", "usa", f.proiettata({62.0, -7.2}), 45.0, 200.0, "R_aereo");
    orion.entra_con = "orion";
    orion.ruolo = "P-3C da Keflavik";
    s.unita.push_back(orion);
    s.aerei_asw.push_back(aereo(4, 3, std::nullopt, "pace", false, {2}, v0, 20000.0));
    s.aerei_asw.push_back(aereo(5, -1, f.proiettata({62.0, -7.2}), "pace", false, {2}, v0, 30000.0));
    regola(s, "orion", {quando(C::tempo, 2700.0)}, {azione(A::entra, 5)});
    RadioScenario r;
    r.ordini = OrdiniScenario{"COMSUBLANT", "USS LOS ANGELES (SSN 688)", "TRAIL HANDOVER", "SECRET"};
    r.roe = roe_tutte("trattenuto");
    MessaggioScenario contatto = radio("CONTATTO", 1800.0, "priority", "COMSUBLANT", "VICTOR III CONTACT",
                                       {"FFG IN CONTATTO CON UN VICTOR III. PRONTI A PRENDERE IL PEDINAMENTO QUANDO LA FREGATA LO LASCIA.",
                                        "FFG HOLDING A VICTOR III. STAND BY TO TAKE THE TRAIL WHEN THE FRIGATE RELEASES IT."});
    contatto.contatto = {2};
    contatto.contatto_errore_m = 4000.0;
    contatto.contatto_sensore = "p3";
    r.messaggi = {contatto};
    s.radio = r;
    s.briefing[0] = {"PROVA S2", "Una Perry pedina un Victor III in pace. Il P-3 arriva dopo."};
    s.briefing[1] = {"TEST S2", "A Perry trails a Victor III in peacetime. The P-3 comes later."};
    return s;
}

// S3 — Navi, aerei e sottomarini sovietici insieme contro un intruso (Barents, guerra, inverno). Attese: t=0 un sensore
// fisso da' un'area di 20x10 nm, il comando manda l'Il-38 e il KPUG piu' vicino; t=30-60 min l'Il-38 posa una barriera
// di RGB-1; il Victor III resta nella sua area e ascolta, le navi stanno fra le due aree; t=60-90 min una boa reagisce,
// barriera di precisione, contatto al KPUG con 12 min di ritardo; t=90 min il Ka-27 immerge, il capogruppo assegna
// l'attacco (AT-1 o SS-N-14); se l'intruso entra nell'area del Victor aerei e navi smettono e il Victor va a cercarlo.
// Variante S3A: il giocatore e' l'Alfa nel bastione e vede la scena dal lato sovietico, coi contatti in ritardo.
// Oggi: il datum del sensore fisso e' un cue ASW, l'Il-38 e i Ka-27 hanno il controllore, il Victor pattuglia la sua
// zona; il passaggio dei contatti col ritardo e la regola dell'area del Victor no.
ScenarioMissione s3(const DatiScenario& d, bool alfa) {
    const Fondale f = Fondale::carica(DATI + "/teatri/barents.fdl");
    ScenarioMissione s;
    base(s, d, f, alfa ? "PROVA_S3A_BASTIONE_ALFA" : "PROVA_S3_BASTIONE", "barents", alfa);
    s.titolo = alfa ? TestiScenario{"S3A - Il bastione visto dall'Alfa", "S3A - The bastion from the Alfa"}
                    : TestiScenario{"S3 - L'intruso nel bastione", "S3 - The intruder in the bastion"};
    s.giorno = 8, s.mese = 2, s.ora_utc = 20.0;
    s.situazione = "guerra";
    meteo(s, 4.0);
    // l'intruso entra a 25 km a nord del KPUG (il runtime, 27/10: da 160 km l'attesa dei 60-90 minuti non poteva arrivare)
    const Vec2 area_victor = acqua(f, f.proiettata({70.2, 33.8}), 200.0);
    const Vec2 ingresso = acqua(f, polare(acqua(f, polare(area_victor, 30.0, 30000.0), 150.0), 0.0, 25000.0), 150.0);
    const Vec2 severomorsk = f.proiettata({69.03, 33.42});
    UnitaScenario intruso = alfa ? unita(2, "los_angeles", "sottomarino", "usa", ingresso, 160.0, 6.0, "R_sottomarino")
                                 : giocatore(false, ingresso, 160.0, 120.0);
    if (alfa) {
        intruso.quota = 120.0;
        intruso.profonda = 180.0;
        intruso.punti = {area_victor};
        intruso.ruolo = "l'intruso";
        s.unita.push_back(giocatore(true, acqua(f, polare(area_victor, 0.0, 12000.0), 200.0), 0.0, 100.0));
    }
    s.unita.push_back(intruso);
    const int bersaglio = alfa ? 2 : 0;
    const int base_id = 10;
    UnitaScenario victor = unita(3, "victor3", "sottomarino", "urss", area_victor, 90.0, 4.0, "R_sottomarino");
    victor.quota = 150.0;
    victor.profonda = 220.0;
    for (int q = 0; q < 4; ++q) victor.punti.push_back(acqua(f, polare(area_victor, 90.0 * q, 8000.0), 200.0));
    victor.ciclica = true;
    victor.ruolo = "Victor III di difesa del bastione";
    s.unita.push_back(victor);
    const Vec2 kpug = acqua(f, polare(area_victor, 30.0, 30000.0), 150.0);
    const char* const NAVI[] = {"krivak2", "krivak2", "kara"};
    for (int k = 0; k < 3; ++k) {
        UnitaScenario n = unita(4 + k, NAVI[k], "nave", "urss", acqua(f, polare(kpug, 120.0 * k, k ? 2500.0 : 0.0), 80.0), 330.0, 10.0, "R_nave");
        n.punti = {acqua(f, polare(ingresso, 180.0, 15000.0), 80.0)};
        n.ruolo = k == 2 ? "capogruppo (al posto dell'Udaloy)" : "Krivak del KPUG";
        s.unita.push_back(n);
    }
    UnitaScenario il38 = unita(7, "il38", "aereo", "urss", severomorsk, 0.0, 300.0, "R_aereo");
    il38.ruolo = "Il-38 a chiamata";
    s.unita.push_back(il38);
    UnitaScenario ka27 = unita(8, "ka27", "aereo", "urss", polare(kpug, 180.0, 1500.0), 0.0, 0.0, "R_aereo");
    ka27.entra_con = "elicottero";
    ka27.ruolo = "Ka-27 del capogruppo";
    s.unita.push_back(ka27);
    s.aerei_asw.push_back(aereo(7, -1, severomorsk, "guerra", !alfa, alfa ? std::vector<int>{2} : std::vector<int>{}, ingresso, 30000.0));
    s.aerei_asw.push_back(aereo(8, 6, std::nullopt, "guerra", !alfa, alfa ? std::vector<int>{2} : std::vector<int>{}, ingresso, 20000.0));
    zona(s, "area_victor", {"l'area d'azione del Victor", "the Victor's action area"}, area_victor, 12000.0);
    regola(s, "sensore_fisso", {quando(C::tempo, 10.0)}, {cue(ingresso, 18000.0)});   // un'area di 20 x 10 nm
    regola(s, "elicottero", {quando(C::tempo, 5400.0)}, {azione(A::entra, 8)});
    (void)bersaglio;
    (void)base_id;
    s.briefing[0] = {alfa ? "PROVA S3A" : "PROVA S3", "Un intruso entra nel bastione: Il-38, KPUG e un Victor III insieme."};
    s.briefing[1] = {alfa ? "TEST S3A" : "TEST S3", "An intruder enters the bastion: Il-38, KPUG and a Victor III together."};
    return s;
}

// S4 — Due SSN amici nella stessa acqua (canale Faroe-Shetland, guerra). Attese: il 688 ha l'area A, un Trafalgar
// l'area B adiacente con una fascia di 5 nm; t=0 il Trafalgar sente un contatto debole sul confine, non classificato; nella
// fascia non lancia, riferisce e si allontana. Variante «messaggio perso»: il giocatore non ha copiato lo spostamento
// della sua area e il Trafalgar lo trova dentro l'area B: lo classifica ostile con P 0,15, fuoco amico possibile; il
// debriefing elenca il messaggio perso.
// Oggi: l'area A e' un'area d'azione dichiarata dalla radio (fase 3: navi e aerei amici non vi usano armi antisom) e
// il messaggio a 15 minuti la sposta davvero; l'area B e la fascia sono zone per le regole. Il fuoco amico fra
// sottomarini per errore di classificazione e la variante col messaggio perso non ci sono ancora.
ScenarioMissione s4(const DatiScenario& d) {
    const Fondale f = Fondale::carica(DATI + "/teatri/faroe.fdl");
    ScenarioMissione s;
    base(s, d, f, "PROVA_S4_DUE_SSN", "faroe", false);
    s.titolo = {"S4 - Due SSN amici nella stessa acqua", "S4 - Two friendly SSNs in the same water"};
    s.giorno = 3, s.mese = 3, s.ora_utc = 4.0;
    s.situazione = "guerra";
    meteo(s, 4.0);
    const Vec2 confine = acqua(f, f.proiettata({60.9, -4.6}), 600.0);
    const Vec2 a = acqua(f, polare(confine, 240.0, 12000.0), 600.0), b = acqua(f, polare(confine, 60.0, 12000.0), 600.0);
    s.unita.push_back(giocatore(false, acqua(f, polare(confine, 240.0, 2000.0), 600.0), 60.0, 150.0));
    // il Trafalgar parte sul bordo della sua area, a 4 km dal confine, poi gira nell'area: due SSN silenziosi si sentono
    // solo da vicino (a 14 km nei primi 10 minuti nessuno dei due sentiva l'altro, 28/10)
    UnitaScenario trafalgar = unita(2, "trafalgar", "sottomarino", "usa", acqua(f, polare(confine, 60.0, 4000.0), 600.0), 240.0, 4.0, "R_sottomarino");
    trafalgar.quota = 180.0;
    trafalgar.profonda = 250.0;
    // il circuito del Trafalgar tocca il lato della fascia: il primo punto e' verso il confine
    for (int q = 0; q < 4; ++q) trafalgar.punti.push_back(acqua(f, polare(b, 240.0 + 90.0 * q, 6000.0), 500.0));
    trafalgar.ciclica = true;
    trafalgar.ruolo = "Trafalgar nell'area B";
    s.unita.push_back(trafalgar);
    UnitaScenario victor = unita(3, "victor3", "sottomarino", "urss", acqua(f, polare(b, 30.0, 25000.0), 600.0), 210.0, 6.0, "R_sottomarino");
    victor.quota = 200.0;
    victor.profonda = 300.0;
    victor.punti = {acqua(f, polare(b, 210.0, 30000.0), 500.0)};
    victor.ruolo = "Victor III che attraversa";
    s.unita.push_back(victor);
    zona(s, "area_a", {"l'area A (la tua)", "area A (yours)"}, a, 10000.0);
    s.zone.back().radio = "azione";   // dichiarata dalla radio: navi e aerei amici non vi usano armi antisom
    zona(s, "area_a2", {"l'area A spostata", "area A moved"}, acqua(f, polare(a, 225.0, 10.0 * MIGLIO), 600.0), 10000.0);
    s.zone.back().radio = "azione";
    s.zone.back().da_messaggio = true;
    zona(s, "area_b", {"l'area B (del Trafalgar)", "area B (the Trafalgar's)"}, b, 10000.0);
    zona(s, "fascia", {"la fascia di separazione", "the separation band"}, confine, 5.0 * MIGLIO / 2.0);
    RadioScenario r;
    r.ordini = OrdiniScenario{"COMSUBEASTLANT", "USS LOS ANGELES (SSN 688)", "WATERSPACE", "SECRET"};
    r.roe = roe_tutte("libero");
    MessaggioScenario area = radio("AREA", 900.0, "immediate", "COMSUBEASTLANT", "WATERSPACE CHANGE",
                                   {"AREA A SPOSTATA DI 10 MIGLIA A SUD-OVEST. UN SSN AMICO E' NELL'AREA B: NON LANCIARE SU CONTATTI NON CLASSIFICATI NELLA FASCIA.",
                                    "AREA A MOVED 10 MILES SOUTHWEST. A FRIENDLY SSN IS IN AREA B: DO NOT FIRE ON UNCLASSIFIED CONTACTS IN THE BAND."});
    area.zone_aggiungi = {"area_a2"};
    area.zone_togli = {"area_a"};
    r.messaggi = {area};
    s.radio = r;
    s.briefing[0] = {"PROVA S4", "Due sottomarini amici in aree adiacenti, con una fascia di 5 miglia in mezzo."};
    s.briefing[1] = {"TEST S4", "Two friendly submarines in adjacent areas, with a 5-mile band between them."};
    return s;
}

// S5 — Un alleato silurato in un convoglio (Atlantico, guerra, mare 4). Attese: t=0 un mercantile e' colpito, allerta
// del convoglio A3, FLASH; t=1-2 min la Perry piu' vicina accosta verso il datum e manda l'SH-60B, le altre scorte si
// mettono fra il datum e il convoglio, il convoglio accosta di 45-90 gradi e accelera; t=5-15 min cerchio di DIFAR; t=15-60
// min falsi contatti e attacchi su falsi contatti possibili; il 688 in supporto resta fuori dall'area delle armi amiche
// o si fa riconoscere; dopo 60 min senza contatti l'allerta scende.
// Oggi: il Victor attacca da solo (R_sottomarino in guerra) da dentro lo schermo; le scorte navigano senza cacciare
// finche' un mercantile non e' colpito (il dossier parte dal colpo), poi le regole le mettono in caccia, fanno accostare il
// convoglio e danno il cue al datum; le scorte fra datum e convoglio, i falsi contatti e la riconoscibilita' del 688 no.
ScenarioMissione s5(const DatiScenario& d) {
    const Fondale f = Fondale::carica(DATI + "/teatri/celtica.fdl");
    ScenarioMissione s;
    base(s, d, f, "PROVA_S5_CONVOGLIO", "celtica", false);
    s.titolo = {"S5 - Un mercantile silurato nel convoglio", "S5 - A merchant torpedoed in the convoy"};
    s.giorno = 21, s.mese = 10, s.ora_utc = 14.0;
    s.situazione = "guerra";
    meteo(s, 4.0);
    const Vec2 c0 = acqua(f, f.proiettata({48.2, -9.0}), 2000.0);
    const double rotta = 260.0;
    const char* const CLASSI[] = {"mercantile", "cargo", "petroliera", "mercantile", "cargo", "petroliera"};
    for (int k = 0; k < 6; ++k) {
        const Vec2 p = polare(polare(c0, rotta + 90.0, k % 2 ? 1200.0 : -1200.0), rotta, -1500.0 * (k / 2));
        UnitaScenario m = unita(2 + k, CLASSI[k], "nave", "usa", p, rotta, 10.0, "", true);
        m.punti = {polare(p, rotta, 120000.0)};
        m.ruolo = "convoglio";
        s.unita.push_back(m);
    }
    for (int k = 0; k < 3; ++k) {
        const Vec2 p = polare(c0, rotta + (k == 0 ? 0.0 : k == 1 ? 90.0 : 270.0), k ? 4000.0 : 5000.0);
        UnitaScenario e = unita(8 + k, "perry", "nave", "usa", p, rotta, 12.0, "");   // R_nave al primo colpo (regole «colpito»)
        e.punti = {polare(p, rotta, 120000.0)};
        e.ruolo = "scorta";
        s.unita.push_back(e);
    }
    UnitaScenario elicottero = unita(11, "sh60b", "aereo", "usa", polare(c0, rotta + 90.0, 4200.0), rotta, 0.0, "R_aereo");
    elicottero.entra_con = "colpito";
    elicottero.ruolo = "SH-60B della Perry di dritta";
    s.unita.push_back(elicottero);
    s.aerei_asw.push_back(aereo(11, 9, std::nullopt, "guerra", false, {12}, c0, 15000.0));
    // dentro lo schermo, fra le due colonne: i mercantili sono i bersagli piu' vicini
    UnitaScenario victor = unita(12, "victor3", "sottomarino", "urss", polare(c0, rotta, -2250.0), rotta, 4.0, "R_sottomarino");
    victor.quota = 80.0;
    victor.profonda = 200.0;
    victor.parametri.salve = 2;
    victor.ruolo = "l'attaccante";
    s.unita.push_back(victor);
    s.unita.insert(s.unita.begin(), giocatore(false, acqua(f, polare(c0, rotta + 180.0, 8000.0), 2000.0), rotta, 120.0));
    // il primo colpo: il convoglio accosta di 60 gradi e accelera, il datum all'ASW
    for (int k = 0; k < 3; ++k) {
        std::vector<AzioneScenario> az = {cue(polare(c0, rotta, -2250.0), 8000.0), azione(A::entra, 11)};
        for (int e = 0; e < 3; ++e) {
            AzioneScenario caccia = azione(A::comportamento, 8 + e);
            caccia.comportamento = "R_nave";
            az.push_back(caccia);
        }
        for (int m = 0; m < 6; ++m) az.push_back(vai(2 + m, polare(s.unita[static_cast<std::size_t>(1 + m)].pos, rotta - 60.0, 120000.0), 13.0));
        regola(s, k == 0 ? "colpito" : "colpito_" + std::to_string(k), {su(C::unita_colpita, 2 + k)}, az);
    }
    RadioScenario r;
    r.ordini = OrdiniScenario{"COMSUBLANT", "USS LOS ANGELES (SSN 688)", "CONVOY DIRECT SUPPORT", "SECRET"};
    r.roe = roe_tutte("libero");
    MessaggioScenario flash = radio("FLASH", 0.0, "flash", "CTG 24.1", "CONVOY UNDER ATTACK",
                                    {"CONVOGLIO ATTACCATO. RESTARE FUORI DALL'AREA DELLE ARMI DELLE SCORTE, 5 MIGLIA DAL DATUM.",
                                     "CONVOY UNDER ATTACK. STAY CLEAR OF THE ESCORTS' WEAPON AREA, 5 MILES FROM DATUM."});
    flash.se_unita_colpita = {2, 3, 4};
    r.messaggi = {flash};
    s.radio = r;
    s.briefing[0] = {"PROVA S5", "Il 688 in supporto diretto a un convoglio; un Victor III attacca."};
    s.briefing[1] = {"TEST S5", "The 688 in direct support of a convoy; a Victor III attacks."};
    return s;
}

// S6 — Uscita di un SSBN con AGI e pescherecci (pace; variante di Kola dal lato sovietico, perche' manca la classe di un
// SSBN alleato). Attese: t=0 la nave spia e' in stazione all'uscita (la norvegese tipo Marjata, come l'AGI del dossier);
// t=0-30 min l'SSBN esce in superficie, la nave spia lo vede e manda il messaggio alla sua forza (ritardo 15-30 min);
// t=30-60 min l'SSN che aspettava fuori riceve il contatto e va verso l'uscita; il giocatore deve «pulire» l'uscita:
// trovare l'SSN che aspetta, senza armi; i pescherecci danno falsi contatti e rischio di rete (CPA 3000 yd).
// Oggi: il messaggio della nave spia all'SSN e' una regola a 30 minuti che lo manda verso l'uscita; il passaggio vero
// dei contatti (fase 3) e il rischio di rete no.
ScenarioMissione s6(const DatiScenario& d) {
    const Fondale f = Fondale::carica(DATI + "/teatri/barents.fdl");
    ScenarioMissione s;
    base(s, d, f, "PROVA_S6_USCITA", "barents", true);
    s.titolo = {"S6 - L'uscita del Typhoon", "S6 - The Typhoon sails"};
    s.giorno = 4, s.mese = 9, s.ora_utc = 9.0;
    s.situazione = "pace";
    meteo(s, 2.0);
    const Vec2 uscita = acqua(f, f.proiettata({69.40, 33.55}), 50.0), fuori = acqua(f, f.proiettata({70.1, 33.7}), 150.0);
    const Vec2 spia = acqua(f, f.proiettata({69.75, 33.9}), 120.0), attesa = acqua(f, f.proiettata({70.2, 32.3}), 220.0);
    // l'SSN aspetta sul fango della scarpata (oltre 200 m d'acqua), fuori dalla sabbia che riflette, a ~54 km dalla rotta
    // d'uscita e a ~101 km dal Typhoon alla partenza. Con la schiera agganciata (D) l'IA fila solo il cavo che non tocca
    // il fondo (cavo_sicuro_m) e la TB-16 ascolta a ~180 m: a 70.1N 32.4E (49 km, 89 km dal Typhoon) lo sentiva in 8-13
    // minuti; qui, appena oltre il salto della propagazione, lo trova a 5560 e 6580 s nei semi 1 e 2, dopo il messaggio
    // dei 30 minuti (trailer-1, 28/10; prima della schiera a 4370 e 4410 s). Sonde: tools/taratura/sonde/*_s6.cpp
    s.unita.push_back(giocatore(true, acqua(f, polare(uscita, 0.0, 6000.0), 80.0), 0.0, 60.0));
    UnitaScenario typhoon = unita(2, "typhoon", "sottomarino", "urss", uscita, 0.0, 8.0, "");
    typhoon.quota = 0.0;
    typhoon.punti = {fuori};
    typhoon.ruolo = "SSBN in uscita";
    s.unita.push_back(typhoon);
    UnitaScenario nave_spia = unita(3, "agi", "nave", "usa", spia, 90.0, 2.0, "", true);
    for (int q = 0; q < 4; ++q) nave_spia.punti.push_back(acqua(f, polare(spia, 90.0 * q, 2000.0), 100.0));
    nave_spia.ciclica = true;
    nave_spia.ruolo = "nave spia norvegese (tipo Marjata)";
    s.unita.push_back(nave_spia);
    UnitaScenario la = unita(4, "los_angeles", "sottomarino", "usa", attesa, 90.0, 3.0, "R_sottomarino");
    la.quota = 90.0;
    la.profonda = 110.0;
    la.parametri.salve = 0;
    la.ruolo = "SSN che aspetta";
    s.unita.push_back(la);
    for (int k = 0; k < 4; ++k) {
        const Vec2 p = acqua(f, polare(fuori, 45.0 + 90.0 * k, 9000.0), 80.0);
        UnitaScenario pesca = unita(5 + k, "peschereccio", "nave", "urss", p, 90.0 * k, 4.0, "", true);
        pesca.punti = {acqua(f, polare(p, 90.0 * k + 90.0, 8000.0), 80.0), p};
        pesca.ciclica = true;
        pesca.ruolo = "peschereccio";
        s.unita.push_back(pesca);
    }
    regola(s, "contatto_alla_forza", {quando(C::tempo, 1800.0)}, {vai(4, acqua(f, polare(uscita, 0.0, 15000.0), 100.0), 6.0)});
    ObiettivoScenario trova;
    trova.id = "trova";
    trova.tipo = "scoprire";
    trova.unita = {4};
    trova.testo = {"Trovare l'SSN che aspetta", "Find the waiting SSN"};
    s.obiettivi.push_back(trova);
    ObiettivoScenario armi;
    armi.id = "armi";
    armi.tipo = "proteggere";
    armi.unita = {3, 4};
    armi.colpire = false;
    armi.testo = {"Nessuna arma", "No weapons"};
    s.obiettivi.push_back(armi);
    s.briefing[0] = {"PROVA S6", "Un Typhoon esce da Kola. Una nave spia norvegese e' in stazione; un SSN americano aspetta fuori."};
    s.briefing[1] = {"TEST S6", "A Typhoon sails from Kola. A Norwegian spy ship is on station; an American SSN waits outside."};
    return s;
}

// S7 — Un Victor III pedina un gruppo portaerei, la crisi diventa guerra (Mediterraneo, crisi). Attese: t=0 il Victor e'
// a 5-8 nm dietro il gruppo, un Kara in pedinamento diretto a vista della portaerei, allerta USA A2; il gruppo zig-zaga,
// gli SH-3H sull'asse, i ping trovano il Victor a ~40 min, la scorta lo tiene sotto; t=60 min ordine radio di guerra:
// il gruppo USA lo copia subito, il Victor solo alla sua sessione successiva (10-20 min dopo); intanto il Kara e' il
// primo bersaglio della scorta, e il Victor senza l'ordine non lancia («battaglia per il primo salvo»).
// Oggi: la guerra a 60 minuti e' l'azione «situazione» (anche m688.situazione per motore e ASW aereo), il ritardo del
// Victor una regola che alla guerra gli toglie il lancio (R_pedina_vicino) e a 75 minuti gli da' il comportamento d'attacco;
// la copia in ritardo dell'ordine per una sola unita' no: le regole d'ingaggio dell'IA (m688.armi_libere) valgono per tutta la
// parte.
ScenarioMissione s7(const DatiScenario& d) {
    const Fondale f = Fondale::carica(DATI + "/teatri/gibilterra.fdl");
    ScenarioMissione s;
    base(s, d, f, "PROVA_S7_PORTAEREI", "gibilterra", false);
    s.titolo = {"S7 - La crisi diventa guerra", "S7 - The crisis turns to war"};
    s.giorno = 19, s.mese = 6, s.ora_utc = 11.0;
    s.situazione = "crisi";
    meteo(s, 2.0);
    const Vec2 g0 = acqua(f, f.proiettata({36.0, -4.6}), 800.0);
    const double rotta = 80.0;
    const Vec2 arrivo = acqua(f, f.proiettata({36.1, -3.7}), 800.0);
    UnitaScenario nimitz = unita(2, "nimitz", "nave", "usa", g0, rotta, 15.0, "R_nave");
    nimitz.punti = {arrivo};
    nimitz.ruolo = "portaerei";
    s.unita.push_back(nimitz);
    const char* const SCORTA[] = {"spruance", "spruance", "perry"};
    for (int k = 0; k < 3; ++k) {
        const Vec2 p = polare(g0, rotta + (k == 0 ? 0.0 : k == 1 ? 120.0 : 240.0), 4000.0);
        UnitaScenario e = unita(3 + k, SCORTA[k], "nave", "usa", acqua(f, p, 500.0), rotta, 15.0, "R_nave");
        e.punti = {acqua(f, polare(arrivo, rotta + (k == 0 ? 0.0 : k == 1 ? 120.0 : 240.0), 4000.0), 300.0)};
        e.ruolo = "scorta";
        s.unita.push_back(e);
    }
    for (int k = 0; k < 2; ++k) {
        UnitaScenario h = unita(6 + k, "sh3h", "aereo", "usa", polare(g0, rotta + 180.0, 3000.0 + 1000.0 * k), rotta, 0.0, "R_aereo");
        h.ruolo = "SH-3H sull'asse";
        s.unita.push_back(h);
        s.aerei_asw.push_back(aereo(6 + k, 2, std::nullopt, "crisi", false, {8}, polare(g0, rotta + 180.0, 10000.0), 12000.0));
    }
    UnitaScenario victor = unita(8, "victor3", "sottomarino", "urss", acqua(f, polare(g0, rotta + 180.0, 6.0 * MIGLIO), 800.0), rotta, 15.0, "R_pedina");
    victor.quota = 150.0;
    victor.profonda = 300.0;
    victor.parametri.distanza_nm = 6.0;
    victor.ruolo = "Victor III in pedinamento";
    s.unita.push_back(victor);
    UnitaScenario kara = unita(9, "kara", "nave", "urss", acqua(f, polare(g0, rotta + 150.0, 6000.0), 500.0), rotta, 15.0, "R_pedina");
    kara.parametri.distanza_nm = 3.0;
    kara.ruolo = "Kara in pedinamento diretto";
    s.unita.push_back(kara);
    s.unita.insert(s.unita.begin(), giocatore(false, acqua(f, polare(g0, rotta, 15000.0), 800.0), rotta, 150.0));
    AzioneScenario guerra = azione(A::situazione);
    guerra.comportamento = "guerra";
    AzioneScenario attacca = azione(A::comportamento, 8);
    attacca.comportamento = "R_sottomarino";
    // la situazione vale per tutta la parte: alla guerra anche il Victor ha le armi libere, e R_pedina lancerebbe appena ha la
    // soluzione. Finche' non copia l'ordine sta in R_pedina_vicino, che non lancia; il tiro di risposta resta
    AzioneScenario senza_ordine = azione(A::comportamento, 8);
    senza_ordine.comportamento = "R_pedina_vicino";
    regola(s, "guerra", {quando(C::tempo, 3600.0)}, {guerra, senza_ordine, manda("GUERRA")});
    regola(s, "copia_del_victor", {quando(C::tempo, 4500.0)}, {attacca});
    RadioScenario r;
    r.ordini = OrdiniScenario{"CINCUSNAVEUR", "USS LOS ANGELES (SSN 688)", "CVBG SUPPORT", "SECRET"};
    r.roe = roe_tutte("trattenuto");
    r.messaggi = {radio("GUERRA", -1.0, "flash", "CINCUSNAVEUR", "HOSTILITIES", {"OSTILITA' IN CORSO. ARMI LIBERE SULLE UNITA' SOVIETICHE.",
                                                                                 "HOSTILITIES COMMENCED. WEAPONS FREE ON SOVIET UNITS."},
                        roe_tutte("libero"))};
    s.radio = r;
    s.briefing[0] = {"PROVA S7", "Un Victor III e un Kara pedinano un gruppo portaerei; a un'ora la crisi diventa guerra."};
    s.briefing[1] = {"TEST S7", "A Victor III and a Kara trail a carrier group; after an hour the crisis turns to war."};
    return s;
}

// S8 — La parte del luogo (M7; Otranto, al largo di Valona, pace fra USA e URSS, mare 2, giorno). Una Koni della marina
// del luogo pattuglia il golfo, in crisi con gli USA e in pace con l'URSS; un Foxtrot sovietico va in superficie verso
// la baia, vicino a lei. A 15 minuti il luogo dichiara la guerra agli USA. Attese (piano M7, nota condivisa): la Koni
// caccia solo il 688 e in guerra lo attacca; il Foxtrot non e' un suo bersaglio, e fra il 688 e il Foxtrot resta la pace.
ScenarioMissione s8(const DatiScenario& d) {
    const Fondale f = Fondale::carica(DATI + "/teatri/otranto.fdl");
    ScenarioMissione s;
    base(s, d, f, "PROVA_S8_LUOGO", "otranto", false);
    s.titolo = {"S8 - La parte del luogo", "S8 - The local party"};
    s.giorno = 14, s.mese = 5, s.ora_utc = 10.0;
    s.situazione = "pace";
    s.situazione_luogo = SituazioneLuogo{"crisi", "pace"};
    s.durata_s = 3600.0;
    meteo(s, 2.0);
    const Vec2 golfo = acqua(f, f.proiettata({40.47, 19.18}), 60.0);
    UnitaScenario koni = unita(2, "koni", "nave", "luogo", golfo, 270.0, 10.0, "R_nave");
    for (int k = 1; k <= 4; ++k) koni.punti.push_back(acqua(f, polare(golfo, 90.0 * k, 5000.0), 40.0));
    koni.ciclica = true;
    koni.ruolo = "motovedetta del luogo";
    s.unita.push_back(koni);
    UnitaScenario foxtrot = unita(3, "foxtrot", "sottomarino", "urss", acqua(f, polare(golfo, 300.0, 4000.0), 40.0), 120.0, 6.0, "");
    foxtrot.punti = {acqua(f, f.proiettata({40.42, 19.425}), 15.0)};
    foxtrot.ruolo = "Foxtrot sovietico verso la baia, in superficie";
    s.unita.push_back(foxtrot);
    s.unita.insert(s.unita.begin(), giocatore(false, acqua(f, polare(golfo, 260.0, 9000.0), 120.0), 80.0, 60.0));
    AzioneScenario guerra = azione(A::situazione);
    guerra.comportamento = "guerra";
    guerra.fra_a = "luogo";
    guerra.fra_b = "usa";
    regola(s, "guerra_del_luogo", {quando(C::tempo, 900.0)}, {guerra, manda("LUOGO")});
    RadioScenario r;
    r.ordini = OrdiniScenario{"CINCUSNAVEUR", "USS LOS ANGELES (SSN 688)", "GULF OF VLORE SURVEILLANCE", "SECRET"};
    r.roe = roe_tutte("trattenuto");
    r.messaggi = {radio("LUOGO", -1.0, "flash", "CINCUSNAVEUR", "LOCAL HOSTILITIES",
                        {"LA MARINA DEL LUOGO HA DICHIARATO OSTILI LE UNITA' AMERICANE. EVITATE LE SUE UNITA': ARMI SOLO PER DIFESA.",
                         "THE LOCAL NAVY HAS DECLARED US UNITS HOSTILE. AVOID ITS UNITS: WEAPONS FOR SELF-DEFENCE ONLY."})};
    s.radio = r;
    s.briefing[0] = {"PROVA S8", "Una Koni della marina del luogo, in crisi con gli USA e in pace con l'URSS; un Foxtrot sovietico le passa vicino. A 15 minuti il luogo dichiara la guerra agli USA."};
    s.briefing[1] = {"TEST S8", "A local-navy Koni, in crisis with the US and at peace with the USSR; a Soviet Foxtrot passes close by. After 15 minutes the locals declare war on the US."};
    return s;
}

std::vector<ScenarioMissione> scene(const DatiScenario& d) {
    return {s1(d, false), s1(d, true), s2(d), s3(d, false), s3(d, true), s4(d), s5(d), s6(d), s7(d), s8(d)};
}
std::vector<ScenarioMissione> scene_missili(const DatiScenario& d);
std::vector<ScenarioMissione> scene_trailer(const DatiScenario& d);
// tutte le scene di prova, accese: le interazioni e, dal 28/10/2026 con la stazione cablata, i missili; dal 29/10/2026 le
// scene del trailer
std::vector<ScenarioMissione> tutte(const DatiScenario& d) {
    auto v = scene(d);
    for (auto& s : scene_missili(d)) v.push_back(std::move(s));
    for (auto& s : scene_trailer(d)) v.push_back(std::move(s));
    return v;
}

// --- le scene della stazione missili (docs/21 §11.13): spente, in prove/missili ------------------------------------------

// M1 — Antinave con scorta e un neutrale nella scatola (Faroe, guerra, mare 3, giorno). Uno Slava a ~38 km con due
// Krivak: uno davanti sulla linea di tiro, l'altro di lato; un cargo neutrale grande a 3 km dal previsto, dentro la
// scatola media. Attese con la stazione cablata (dossier 27 §3): con la scatola media e la TMA buona il seeker prende
// il primo eco grande dal piu' vicino, quindi spesso il cargo se e' davanti allo Slava; con la piccola lo lascia fuori ma
// la probabilita' stimata cala se la TMA e' scarsa; il Krivak davanti, piu' piccolo, si prende con P 0,4. Il pannello
// mostra l'avviso dei neutrali e la probabilita' stimata; colpire il cargo e' un'infrazione delle ROE.
ScenarioMissione m1(const DatiScenario& d) {
    const Fondale f = Fondale::carica(DATI + "/teatri/faroe.fdl");
    ScenarioMissione s;
    base(s, d, f, "PROVA_M1_ANTINAVE", "faroe", false);
    s.autore = "Prove della stazione missili (docs/21)";
    s.titolo = {"M1 - Antinave con scorta e un neutrale", "M1 - Antiship shot with escort and a neutral"};
    s.giorno = 12, s.mese = 6, s.ora_utc = 11.0;
    s.situazione = "guerra";
    s.durata_s = 3600.0;
    meteo(s, 3.0);
    s.carico.dotazione = {{"mk48", 18}, {"harpoon", 4}, {"esca", 6}};
    s.carico.deposito = {{"mk48", 18}, {"harpoon", 4}, {"esca", 6}};
    s.carico.tubi = {"harpoon", "harpoon", "mk48", "mk48", "esca", "esca", "harpoon"};
    const Vec2 io = acqua(f, f.proiettata({61.6, -9.6}), 800.0);   // a ovest delle Faroe, lontano dai bordi della griglia
    s.unita.push_back(giocatore(false, io, 45.0, 30.0));   // dentro la fascia di lancio dell'Harpoon (20-60 m)
    const Vec2 slava = acqua(f, polare(io, 45.0, 38000.0), 300.0);
    // acqua() cerca il punto valido piu' vicino: fuori dalla griglia o su un banco puo' spostare tutto (il runtime ha
    // trovato lo Slava a 520 m dal 688 il 28/10). Le distanze della scena devono restare quelle volute
    if (std::abs(distanza(io, slava) - 38000.0) > 1500.0)
        throw std::runtime_error("M1: lo Slava non e' a 38 km dal 688 (" + std::to_string(distanza(io, slava)) + " m)");
    UnitaScenario sl = unita(2, "slava", "nave", "urss", slava, 210.0, 15.0, "R_nave");
    sl.punti = {acqua(f, polare(slava, 210.0, 60000.0), 300.0)};
    sl.ruolo = "bersaglio (incrociatore)";
    s.unita.push_back(sl);
    UnitaScenario k1 = unita(3, "krivak2", "nave", "urss", acqua(f, polare(slava, 225.0, 4000.0), 300.0), 210.0, 15.0, "R_nave");
    k1.punti = {acqua(f, polare(k1.pos, 210.0, 60000.0), 300.0)};
    k1.ruolo = "scorta sulla linea di tiro";
    s.unita.push_back(k1);
    UnitaScenario k2 = unita(4, "krivak2", "nave", "urss", acqua(f, polare(slava, 120.0, 5000.0), 300.0), 210.0, 15.0, "R_nave");
    k2.punti = {acqua(f, polare(k2.pos, 210.0, 60000.0), 300.0)};
    k2.ruolo = "scorta di lato";
    s.unita.push_back(k2);
    UnitaScenario cargo = unita(5, "cargo", "nave", "neutrale", acqua(f, polare(slava, 315.0, 3000.0), 300.0), 100.0, 12.0, "", true);
    cargo.punti = {acqua(f, polare(cargo.pos, 100.0, 60000.0), 300.0)};
    cargo.ruolo = "cargo neutrale nella scatola";
    s.unita.push_back(cargo);
    UnitaScenario lontano = unita(6, "mercantile", "nave", "neutrale", acqua(f, polare(slava, 20.0, 25000.0), 300.0), 270.0, 11.0, "", true);
    lontano.punti = {acqua(f, polare(lontano.pos, 270.0, 60000.0), 300.0)};
    lontano.ruolo = "mercantile fuori dalla scatola (controllo)";
    s.unita.push_back(lontano);
    ObiettivoScenario affonda;
    affonda.id = "slava";
    affonda.tipo = "affondare";
    affonda.unita = {2};
    affonda.testo = {"Colpire lo Slava", "Hit the Slava"};
    s.obiettivi.push_back(affonda);
    ObiettivoScenario neutrali;
    neutrali.id = "neutrali";
    neutrali.tipo = "proteggere";
    neutrali.unita = {5, 6};
    neutrali.colpire = false;
    neutrali.primario = false;
    neutrali.testo = {"Nessun neutrale colpito", "No neutral hit"};
    s.obiettivi.push_back(neutrali);
    RadioScenario r;
    r.ordini = OrdiniScenario{"COMSUBLANT", "USS LOS ANGELES (SSN 688)", "ANTI-SURFACE ATTACK", "SECRET"};
    r.roe = roe_tutte("libero");
    s.radio = r;
    s.briefing[0] = {"PROVA M1", "Uno Slava con due Krivak scende verso sud-ovest a 15 nodi. Un cargo neutrale incrocia vicino al gruppo."};
    s.briefing[1] = {"TEST M1", "A Slava with two Krivaks heads south-west at 15 knots. A neutral freighter crosses close to the group."};
    return s;
}

// M2 — TLAM con area di lancio e navigazione incerta (Barents, guerra, mare 4, notte). Il 688 parte a 22 miglia dal
// punto di lancio pianificato della missione 0417 con 1,5 miglia d'incertezza; l'ordine d'esecuzione arriva alla radio
// dopo 30 minuti, ora sul bersaglio 04:30Z ±10 min. Attese con la stazione cablata (dossier 27 §4.2): la pagina TLAM dice
// VALIDA / RIPIANIFICA / NON VALIDA e la presa stimata della prima mappa; un punto nave prima del lancio la alza; senza,
// l'esito vero (sulla posizione vera) puo' perdere il landfall anche con il pannello a «VALIDA». Il Krivak di pattuglia
// verso costa rende costoso salire per il punto nave.
ScenarioMissione m2(const DatiScenario& d) {
    const Fondale f = Fondale::carica(DATI + "/teatri/barents.fdl");
    ScenarioMissione s;
    base(s, d, f, "PROVA_M2_TLAM", "barents", false);
    s.autore = "Prove della stazione missili (docs/21)";
    s.titolo = {"M2 - TLAM: area di lancio e navigazione incerta", "M2 - TLAM: launch area and uncertain navigation"};
    s.giorno = 10, s.mese = 11, s.ora_utc = 1.0;
    s.situazione = "guerra";
    s.durata_s = 4.0 * 3600.0;
    meteo(s, 4.0);
    s.carico.dotazione = {{"mk48", 18}, {"tlam_c", 4}, {"esca", 6}};
    s.carico.deposito = {{"mk48", 18}, {"tlam_c", 4}, {"esca", 6}};
    s.carico.tubi = {"tlam_c", "tlam_c", "mk48", "mk48", "esca", "esca", "tlam_c"};
    const Vec2 plp = acqua(f, f.proiettata({71.5, 31.8}), 200.0);
    const Vec2 partenza = acqua(f, polare(plp, 250.0, 22.0 * MIGLIO), 200.0);
    if (std::abs(distanza(partenza, plp) - 22.0 * MIGLIO) > 3000.0)
        throw std::runtime_error("M2: il 688 non parte a 22 miglia dal PLP (" + std::to_string(distanza(partenza, plp)) + " m)");
    s.unita.push_back(giocatore(false, partenza, 70.0, 60.0));
    const Vec2 pattuglia = acqua(f, polare(plp, 170.0, 30000.0), 150.0);
    UnitaScenario k = unita(2, "krivak2", "nave", "urss", pattuglia, 90.0, 10.0, "R_nave");
    k.punti = {acqua(f, polare(pattuglia, 90.0, 15000.0), 150.0), acqua(f, polare(pattuglia, 270.0, 15000.0), 150.0)};
    k.ciclica = true;
    k.ruolo = "pattuglia verso costa";
    s.unita.push_back(k);
    UnitaScenario pesca = unita(3, "peschereccio", "nave", "urss", acqua(f, polare(plp, 300.0, 9000.0), 150.0), 0.0, 4.0, "", true);
    pesca.punti = {acqua(f, polare(pesca.pos, 90.0, 6000.0), 150.0), pesca.pos};
    pesca.ciclica = true;
    pesca.ruolo = "peschereccio";
    s.unita.push_back(pesca);
    ObiettivoScenario arriva;
    arriva.id = "plp";
    arriva.tipo = "raggiungere";
    arriva.cerchio = CerchioScenario{plp.x, plp.y, 10.0 * MIGLIO};
    arriva.testo = {"Raggiungere l'area di lancio della 0417", "Reach the 0417 launch area"};
    s.obiettivi.push_back(arriva);
    zona(s, "plp_0417", {"Area di lancio 0417", "Launch area 0417"}, plp, 10.0 * MIGLIO);
    RadioScenario r;
    r.ordini = OrdiniScenario{"COMSUBLANT", "USS LOS ANGELES (SSN 688)", "STRIKE", "SECRET"};
    r.roe = roe_tutte("libero");
    r.messaggi = {radio("ESEGUIRE", 1800.0, "immediate", "COMSUBLANT", "TLAM EXECUTE",
                        {"ESEGUIRE MISSIONE TLAM 0417. ORA SUL BERSAGLIO 0430Z PIU' O MENO 10 MINUTI. LANCIO DALL'AREA PIANIFICATA.",
                         "EXECUTE TLAM MISSION 0417. TIME ON TARGET 0430Z PLUS OR MINUS 10 MINUTES. LAUNCH FROM PLANNED AREA."})};
    r.messaggi[0].esecuzione_tlam = "0417";   // l'ordine d'esecuzione (docs/21 §5)
    s.radio = r;
    s.navigazione = NavigazioneScenario{1.5};   // il 688 a giorni dall'ultimo punto nave (docs/08 §6)
    // la libreria di bordo (docs/21 §11.8): tot_s dall'inizio della missione
    const Vec2 plp2 = acqua(f, f.proiettata({70.8, 30.9}), 200.0);
    const double d1 = distanza(plp, f.proiettata({71.5, 31.8})), d2 = distanza(plp2, f.proiettata({70.8, 30.9}));
    if (d1 > 3000.0 || d2 > 3000.0)
        throw std::runtime_error("M2: un PLP e' finito lontano dal punto voluto (" + std::to_string(d1) + ", " + std::to_string(d2) + " m)");
    s.tlam = {{"0417", "SEVEROMORSK, DEPOSITO CARBURANTI", plp, f.proiettata({69.42, 33.05}), 12600.0, 600.0, 25000.0},
              {"0418", "OLENJA GUBA, PONTILI", plp2, f.proiettata({69.25, 32.10}), 13500.0, 600.0, 12000.0}};
    s.briefing[0] = {"PROVA M2", "Due missioni TLAM in libreria: 0417 e 0418. Portarsi nell'area di lancio, attendere l'ordine d'esecuzione."};
    s.briefing[1] = {"TEST M2", "Two TLAM missions in the library: 0417 and 0418. Proceed to the launch area and await the execute order."};
    return s;
}

// Le due scene del video d'apertura (docs/29_video_apertura.md), lo stesso mare visto da ciascun battello: il 688 e l'Alfa a
// 8 km, a quota periscopica, che si vengono incontro; a `LONTANA_S` una nave lontana fa la riga debole sulle cascate, a
// `VICINE_S` una nave per parte entra vicino a chi la guardera' al periscopio, verso il tramonto: la riga e la nave non sono la stessa;
// il battello dell'IA lancia sull'altro col tiro pianificato allo stesso istante in cui, nell'altra scena, lancia quello a cui
// la regia da' gli ordini. Stanno in data/motore/apertura, fuori dagli elenchi delle missioni: le recita app/video_apertura.
constexpr double LONTANA_S = 108.0, VICINE_S = 162.0, LANCIO_688_S = 243.6, LANCIO_ALFA_S = 247.0;   // come data/motore/apertura.toml
constexpr double LONTANA_M = 9000.0, LONTANA_ROTTA = 140.0;   // dal centro: la riga attorno a 3,6 dB su tutti e due (misurato il 06/11, sui dati finali della portata): col sonar dell'Alfa, piu' sordo, un po' piu' vicina a lui

ScenarioMissione apertura(const DatiScenario& d, bool alfa) {
    const Fondale f = Fondale::carica(DATI + "/teatri/faroe.fdl");
    ScenarioMissione s;
    base(s, d, f, alfa ? "APERTURA_ALFA" : "APERTURA_688", "faroe", alfa);
    s.autore = "Video d'apertura (docs/29)";
    s.titolo = {"Lo stesso mare", "The same sea"};
    s.giorno = 12, s.mese = 6, s.ora_utc = 21.55;   // il sole a un grado sull'orizzonte quando salgono i periscopi
    s.situazione = "pace";
    s.durata_s = 3600.0;
    meteo(s, 2.0);
    s.meteo_fisso->visibilita_m = 40000.0;   // da 40 km non c'e' foschia: il cielo del periscopio resta la rampa, che il film tinge
    const Vec2 centro = acqua(f, f.proiettata({60.9, -7.0}), 1000.0);
    const Vec2 p688 = acqua(f, polare(centro, 240.0, 4000.0), 800.0), palfa = acqua(f, polare(centro, 60.0, 4000.0), 800.0);
    s.unita.push_back(giocatore(alfa, alfa ? palfa : p688, alfa ? 240.0 : 60.0, 18.0));
    UnitaScenario altro = unita(1, alfa ? "los_angeles" : "alfa", "sottomarino", alfa ? "usa" : "urss", alfa ? p688 : palfa, alfa ? 60.0 : 240.0, 5.0, "");
    altro.quota = 18.0;
    altro.punti = {acqua(f, polare(altro.pos, altro.rotta, 40000.0), 300.0)};
    s.unita.push_back(altro);
    // dove saranno i due battelli a VICINE_S (circa 3 m/s): la nave a 321 gradi, sotto il sole di quell'ora, al traverso
    UnitaScenario krivak = unita(2, "krivak2", "nave", "urss", acqua(f, polare(polare(p688, 60.0, 500.0), 321.0, 2000.0), 300.0), 231.0, 8.0, "");
    krivak.punti = {acqua(f, polare(krivak.pos, 231.0, 40000.0), 300.0)};
    krivak.entra_con = "vicine";
    s.unita.push_back(krivak);
    UnitaScenario perry = unita(3, "perry", "nave", "usa", acqua(f, polare(polare(palfa, 240.0, 500.0), 321.0, 2000.0), 300.0), 51.0, 8.0, "");
    perry.punti = {acqua(f, polare(perry.pos, 51.0, 40000.0), 300.0)};
    perry.entra_con = "vicine";
    s.unita.push_back(perry);
    // la riga: una nave sola, che ciascuno sente allo stesso tempo e con la stessa forza
    UnitaScenario lontana = unita(4, "perry", "nave", "usa", acqua(f, polare(centro, LONTANA_ROTTA, LONTANA_M), 300.0), 240.0, 12.0, "");
    lontana.punti = {acqua(f, polare(lontana.pos, 240.0, 40000.0), 300.0)};
    lontana.entra_con = "lontana";
    s.unita.push_back(lontana);
    RegolaScenario riga;
    riga.id = "lontana";
    riga.quando = {quando(C::tempo, LONTANA_S)};
    riga.allora = {azione(A::entra, 4)};
    s.regole.push_back(riga);
    RegolaScenario vicine;
    vicine.id = "vicine";
    vicine.quando = {quando(C::tempo, VICINE_S)};
    vicine.allora = {azione(A::entra, 2), azione(A::entra, 3)};
    s.regole.push_back(vicine);
    RegolaScenario lancio;
    lancio.id = "lancio";
    lancio.quando = {quando(C::tempo, alfa ? LANCIO_688_S : LANCIO_ALFA_S)};   // il 688 per primo, come nella scaletta
    AzioneScenario colpo = azione(A::attacca_unita, 1);
    colpo.bersaglio = 0;
    colpo.valore = 1;
    lancio.allora = {colpo};
    s.regole.push_back(lancio);
    RadioScenario r;
    r.ordini = alfa ? OrdiniScenario{"KSF", "K-123", "LO STESSO MARE", "SEKRETNO"} : OrdiniScenario{"COMSUBLANT", "USS LOS ANGELES (SSN 688)", "LO STESSO MARE", "SECRET"};
    r.roe = roe_tutte("libero");
    s.radio = r;
    s.briefing[0] = {"LO STESSO MARE", "La scena del video d'apertura."};
    s.briefing[1] = {"THE SAME SEA", "The scene of the opening video."};
    return s;
}

// M3 — Oltre l'orizzonte, per l'esperimento della spia (docs/28 §3.5; trailer-1, 04/11/2026). In guerra, in acqua fonda a
// sud delle Faroe: uno Slava con una Krivak e, a 120 km, due Perry. Ai 300 km della griglia si sentono, ma nessuno ha la
// posizione dell'altro, ciascuno sulla sua rotta, e nessuno script comanda il tiro: lo Slava lancia solo se ha il bersaglio dai
// sensori o dal quadro radio. La nave spia della S5 non cambiava nulla perche' il Victor attacca coi siluri: il dato radio orienta
// un battello e non gli basta per lanciare, e fra chi legge il quadro ci sono i lanci di missili
ScenarioMissione m3(const DatiScenario& d) {
    const Fondale f = Fondale::carica(DATI + "/teatri/faroe.fdl");
    ScenarioMissione s;
    base(s, d, f, "PROVA_M3_ORIZZONTE", "faroe", false);
    s.autore = "Esperimenti del vetrino di Dio (docs/28)";
    s.titolo = {"M3 - Oltre l'orizzonte", "M3 - Beyond the horizon"};
    s.giorno = 12, s.mese = 6, s.ora_utc = 11.0;
    s.situazione = "guerra";
    s.durata_s = 3600.0;
    meteo(s, 3.0);
    const Vec2 slava = acqua(f, f.proiettata({60.5, -7.0}), 800.0);
    const double rotta = 45.0;
    const Vec2 perry = acqua(f, polare(slava, 90.0, 120000.0), 800.0);
    if (std::abs(distanza(slava, perry) - 120000.0) > 3000.0)   // acqua() puo' spostare il punto: la distanza e' la scena
        throw std::runtime_error("M3: le Perry non sono a 120 km dallo Slava (" + std::to_string(distanza(slava, perry)) + " m)");
    s.unita.push_back(giocatore(false, acqua(f, polare(perry, 180.0, 6000.0), 300.0), rotta, 150.0));   // accanto alle Perry
    UnitaScenario sl = unita(2, "slava", "nave", "urss", slava, rotta, 15.0, "R_nave");
    sl.punti = {acqua(f, polare(slava, rotta, 60000.0), 300.0)};
    sl.ruolo = "il tiratore";
    s.unita.push_back(sl);
    UnitaScenario kr = unita(3, "krivak2", "nave", "urss", acqua(f, polare(slava, 120.0, 4000.0), 300.0), rotta, 15.0, "R_nave");
    kr.punti = {acqua(f, polare(kr.pos, rotta, 60000.0), 300.0)};
    kr.ruolo = "la scorta";
    s.unita.push_back(kr);
    UnitaScenario p1 = unita(4, "perry", "nave", "usa", perry, rotta, 15.0, "R_nave");
    p1.punti = {acqua(f, polare(perry, rotta, 60000.0), 300.0)};
    p1.ruolo = "il bersaglio";
    s.unita.push_back(p1);
    UnitaScenario p2 = unita(5, "perry", "nave", "usa", acqua(f, polare(perry, 90.0, 4000.0), 300.0), rotta, 15.0, "R_nave");
    p2.punti = {acqua(f, polare(p2.pos, rotta, 60000.0), 300.0)};
    p2.ruolo = "il secondo bersaglio";
    s.unita.push_back(p2);
    RadioScenario r;
    r.ordini = OrdiniScenario{"COMSUBLANT", "USS LOS ANGELES (SSN 688)", "OLTRE L'ORIZZONTE", "SECRET"};
    r.roe = roe_tutte("libero");
    s.radio = r;
    s.briefing[0] = {"PROVA M3", "Uno Slava e due Perry a 120 km, in guerra: nessuno vede l'altro."};
    s.briefing[1] = {"TEST M3", "A Slava and two Perrys 120 km apart, at war: neither sees the other."};
    return s;
}

// M4 — Il sottomarino oltre l'orizzonte, per il quadro radio ai sottomarini (tools/re/prove/quadro_radio/proposta.md). In guerra,
// in acqua fonda: un Victor III di pattuglia, senza limite di salve, e a 40 km un convoglio lento con una Perry. Doveva trovare il
// convoglio solo col dato radio, ma lo sente da solo: a 40 km primo contatto solido a 85-119 s, sul mercantile a 113-235 s
// (10 semi, 06/11; le portate oltre i 40 km sono in E6 §7). La regola orienta solo chi non ha un contatto suo, quindi questa
// scena non misura la caccia sul dato
ScenarioMissione m4(const DatiScenario& d) {
    const Fondale f = Fondale::carica(DATI + "/teatri/faroe.fdl");
    ScenarioMissione s;
    base(s, d, f, "PROVA_M4_QUADRO", "faroe", false);
    s.autore = "Esperimenti del vetrino di Dio (docs/28)";
    s.titolo = {"M4 - Il sottomarino oltre l'orizzonte", "M4 - The submarine beyond the horizon"};
    s.giorno = 12, s.mese = 6, s.ora_utc = 11.0;
    s.situazione = "guerra";
    s.durata_s = 7200.0;
    meteo(s, 4.0);
    const Vec2 c0 = acqua(f, f.proiettata({60.65, -7.0}), 800.0);
    const double rotta = 0.0;
    // il primo rilevamento, di fianco al convoglio, con acqua a 40 km esatti: la distanza e' la scena
    Vec2 pattuglia{};
    bool trovata = false;
    for (const double r : {90.0, 75.0, 105.0, 60.0, 120.0, 135.0, 150.0, 165.0, 180.0, 195.0, 210.0, 225.0, 240.0, 255.0, 270.0, 285.0, 300.0, 315.0})
        if (const Vec2 q = polare(c0, r, 40000.0); !trovata && f.dentro(q) && f.profondita(q) >= 400.0) {
            pattuglia = q;
            trovata = true;
            std::printf("M4: il Victor a rilevamento %.0f, fondale %.0f m\n", r, f.profondita(q));
        }
    if (!trovata) throw std::runtime_error("M4: niente acqua a 40 km dal convoglio");
    const char* const CLASSI[] = {"mercantile", "cargo", "petroliera"};
    for (int k = 0; k < 3; ++k) {
        const Vec2 p = polare(c0, rotta, -1500.0 * k);
        UnitaScenario m = unita(2 + k, CLASSI[k], "nave", "usa", p, rotta, 6.0, "", true);
        m.punti = {acqua(f, polare(p, rotta, 40000.0), 300.0)};
        m.ruolo = "convoglio";
        s.unita.push_back(m);
    }
    UnitaScenario e = unita(5, "perry", "nave", "usa", polare(c0, rotta, 4000.0), rotta, 6.0, "R_nave");
    e.punti = {acqua(f, polare(e.pos, rotta, 40000.0), 300.0)};
    e.ruolo = "scorta";
    s.unita.push_back(e);
    UnitaScenario victor = unita(6, "victor3", "sottomarino", "urss", pattuglia, rotta, 4.0, "R_sottomarino");
    victor.quota = 80.0;
    victor.profonda = 200.0;
    victor.parametri.salve = 6;
    victor.ruolo = "il cacciatore";
    s.unita.push_back(victor);
    s.unita.insert(s.unita.begin(), giocatore(false, acqua(f, polare(c0, rotta + 270.0, 120000.0), 300.0), rotta, 150.0));   // lontano da tutti
    RadioScenario r;
    r.ordini = OrdiniScenario{"COMSUBLANT", "USS LOS ANGELES (SSN 688)", "OLTRE L'ORIZZONTE", "SECRET"};
    r.roe = roe_tutte("libero");
    s.radio = r;
    s.briefing[0] = {"PROVA M4", "Un Victor III e, a 40 km, un convoglio con una Perry, in guerra: il Victor sente il convoglio da solo, con un contatto solido in meno di 2 minuti."};
    s.briefing[1] = {"TEST M4", "A Victor III and, 40 km away, a convoy with a Perry, at war: the Victor hears the convoy on its own, with a solid contact in under 2 minutes."};
    return s;
}

std::vector<ScenarioMissione> scene_missili(const DatiScenario& d) { return {m1(d), m2(d), m3(d), m4(d)}; }

// --- le scene del trailer (docs/trailer/01/scaletta_v4.md) ------------------------------------------------------------

// T1 — Un sottomarino colpito che emerge violentemente (R5 del trailer n. 1; Celtica, pace, mare 3, pomeriggio). Il 688 a
// quota periscopica, fermo quasi, col Sole alle spalle; un Victor III a 1500 m per 350 gradi, a 60 m, sulla stessa rotta,
// senza comportamento (non attacca, non scappa); niente altro in vista. L'urto lo da' la riga di comando:
//   melchior688 --cartella-missioni data/missioni/prove --missione PROVA_T1_EMERSIONE --regola simulazione --seme 6
//               --urto-unita 2,100,20,90 --periscopio-su 2 --tasti-a-tempo F (0 periscopio, 1 cambia_ingrandimento)
// Allagato sopra i 150 m, il Victor tenta l'esaurimento delle casse (30-70% secondo il seme) e rompe la superficie con
// la prua alta (E1, docs/10 §10.14).
ScenarioMissione t1(const DatiScenario& d) {
    const Fondale f = Fondale::carica(DATI + "/teatri/celtica.fdl");
    ScenarioMissione s;
    base(s, d, f, "PROVA_T1_EMERSIONE", "celtica", false);
    s.titolo = {"T1 - Un sottomarino colpito torna su", "T1 - A stricken submarine comes up"};
    s.giorno = 21, s.mese = 10, s.ora_utc = 14.0;
    s.situazione = "pace";
    meteo(s, 3.0);
    const Vec2 p = acqua(f, f.proiettata({48.2, -9.0}), 2000.0);
    const double rotta = 260.0;
    UnitaScenario victor = unita(2, "victor3", "sottomarino", "urss", polare(p, 350.0, 1500.0), rotta, 4.0, "");
    victor.quota = 60.0;
    victor.punti = {polare(victor.pos, rotta, 60000.0)};
    victor.ruolo = "il colpito";
    s.unita.push_back(victor);
    UnitaScenario g = giocatore(false, p, rotta, 5.5);   // quota periscopica dell'Enhanced (Ctrl+P)
    g.velocita = 3.0;
    s.unita.insert(s.unita.begin(), g);
    RadioScenario r;
    r.ordini = OrdiniScenario{"COMSUBLANT", "USS LOS ANGELES (SSN 688)", "TRAILER T1", "SECRET"};
    r.roe = roe_tutte("trattenuto");
    s.radio = r;
    s.briefing[0] = {"PROVA T1", "Un Victor III a 1500 m; colpirlo con --urto-unita e guardarlo emergere al periscopio."};
    s.briefing[1] = {"TEST T1", "A Victor III at 1500 m; hit it with --urto-unita and watch it surface through the periscope."};
    return s;
}

// T2 — Missili nel mare grosso (scene 03, 10 e 24 del trailer; Fær Øer, pace, mare 4 con la pioggia, pomeriggio). Il
// 688 a quota periscopica guarda di traverso, a 1500 m, uno Slava con una Krivak; a 30 s lo Slava tira due P-500 su una
// Perry a 25 km, oltre i 20 km minimi del P-500 (azione «lancia missile» su un'unita', che non guarda la situazione).
// Le navi sovietiche e la Perry non hanno comportamento: niente caccia al 688, che resta a guardare.
ScenarioMissione t2(const DatiScenario& d) {
    const Fondale f = Fondale::carica(DATI + "/teatri/faroe.fdl");
    ScenarioMissione s;
    base(s, d, f, "PROVA_T2_MISSILI", "faroe", false);
    s.titolo = {"T2 - Missili nel mare grosso", "T2 - Missiles in a heavy sea"};
    s.giorno = 6, s.mese = 3, s.ora_utc = 14.0;
    s.situazione = "pace";   // in guerra le navi sparano coi cannoni al periscopio alzato a 1500 m (docs/16)
    MeteoFissoScenario m;
    m.stato_mare = 4.0, m.vento_m_s = 12.0, m.vento_da_gradi = 230.0, m.visibilita_m = 12000.0, m.pioggia_mm_h = 1.5, m.nubi_ottavi = 7.0;
    s.meteo_fisso = m;
    const Vec2 p = acqua(f, f.proiettata({62.6, -6.0}), 500.0);
    const double rotta = 0.0;
    UnitaScenario slava = unita(2, "slava", "nave", "urss", polare(p, 90.0, 1500.0), rotta, 12.0, "");
    slava.punti = {polare(slava.pos, rotta, 60000.0)};
    slava.ruolo = "il tiratore";
    s.unita.push_back(slava);
    UnitaScenario krivak = unita(3, "krivak2", "nave", "urss", polare(slava.pos, 180.0, 1500.0), rotta, 12.0, "");
    krivak.punti = {polare(krivak.pos, rotta, 60000.0)};
    krivak.ruolo = "la scorta";
    s.unita.push_back(krivak);
    UnitaScenario perry = unita(4, "perry", "nave", "usa", polare(slava.pos, 60.0, 25000.0), 240.0, 15.0, "");
    perry.punti = {polare(perry.pos, 240.0, 60000.0)};
    perry.ruolo = "il bersaglio";
    s.unita.push_back(perry);
    s.unita.insert(s.unita.begin(), giocatore(false, p, rotta, 5.5));
    s.unita.front().velocita = 3.0;
    AzioneScenario salva = azione(A::lancia_missile, 2);
    salva.bersaglio = 4;
    salva.valore = 2;
    regola(s, "salva", {quando(C::tempo, 30.0)}, {salva});
    RadioScenario r;
    r.ordini = OrdiniScenario{"COMSUBLANT", "USS LOS ANGELES (SSN 688)", "TRAILER T2", "SECRET"};
    r.roe = roe_tutte("trattenuto");
    s.radio = r;
    s.briefing[0] = {"PROVA T2", "Uno Slava a 1500 m tira due P-500 su una Perry; mare 4 e pioggia."};
    s.briefing[1] = {"TEST T2", "A Slava at 1500 m fires two P-500 at a Perry; sea state 4 and rain."};
    return s;
}

// T3 — Il fuoco di notte (scena 41 del trailer; Celtica, guerra, mare 2, notte). L'Alfa a quota periscopica a 2 km da una
// Perry che va al posto di combattimento (allarme a 10 s); col periscopio alzato la Perry lo vede e spara, coi
// proietti illuminanti di notte (docs/16 §9.2).
ScenarioMissione t3(const DatiScenario& d) {
    const Fondale f = Fondale::carica(DATI + "/teatri/celtica.fdl");
    ScenarioMissione s;
    base(s, d, f, "PROVA_T3_FUOCO", "celtica", true);
    s.titolo = {"T3 - Il fuoco di notte", "T3 - Gunfire at night"};
    s.giorno = 21, s.mese = 10, s.ora_utc = 22.0;
    s.situazione = "guerra";
    meteo(s, 2.0);
    const Vec2 p = acqua(f, f.proiettata({48.2, -9.0}), 2000.0);
    UnitaScenario perry = unita(2, "perry", "nave", "usa", polare(p, 0.0, 2000.0), 270.0, 6.0, "R_nave");
    perry.punti = {polare(perry.pos, 270.0, 60000.0)};
    perry.ruolo = "la fregata al posto di combattimento";
    s.unita.push_back(perry);
    s.unita.insert(s.unita.begin(), giocatore(true, p, 0.0, 5.5));
    s.unita.front().velocita = 3.0;
    regola(s, "allarme", {quando(C::tempo, 10.0)}, {azione(A::allarme)});
    RadioScenario r;
    r.ordini = OrdiniScenario{"KSF", "K-463 (PROEKT 705)", "TRAILER T3", "SOV. SEKRETNO"};
    r.roe = roe_tutte("libero");
    s.radio = r;
    s.briefing[0] = {"PROVA T3", "L'Alfa al periscopio a 2 km da una Perry in allarme, di notte."};
    s.briefing[1] = {"TEST T3", "The Alfa at periscope depth 2 km from an alerted Perry, at night."};
    return s;
}

// T4 — L'elicottero (scena 26 del trailer; Mare di Barents, guerra, mare 2, mezzogiorno). Il 688 a quota periscopica; una
// Krivak a 8 km col Ka-27, che caccia sul 688 (ASW aereo contro il giocatore, zona di 3 km attorno a lui): hover,
// sonar calato, boe.
ScenarioMissione t4(const DatiScenario& d) {
    const Fondale f = Fondale::carica(DATI + "/teatri/barents.fdl");
    ScenarioMissione s;
    base(s, d, f, "PROVA_T4_ELICOTTERO", "barents", false);
    s.titolo = {"T4 - L'elicottero", "T4 - The helicopter"};
    s.giorno = 15, s.mese = 6, s.ora_utc = 11.0;
    s.situazione = "guerra";
    meteo(s, 2.0);
    const Vec2 p = acqua(f, f.proiettata({71.0, 36.0}), 150.0);
    UnitaScenario krivak = unita(2, "krivak2", "nave", "urss", acqua(f, polare(p, 20.0, 8000.0), 150.0), 290.0, 8.0, "");
    krivak.punti = {acqua(f, polare(krivak.pos, 290.0, 60000.0), 150.0)};
    krivak.ruolo = "la nave del Ka-27";
    s.unita.push_back(krivak);
    UnitaScenario ka27 = unita(3, "ka27", "aereo", "urss", polare(p, 20.0, 7500.0), 200.0, 0.0, "R_aereo");
    ka27.ruolo = "Ka-27 della Krivak";
    s.unita.push_back(ka27);
    s.aerei_asw.push_back(aereo(3, 2, std::nullopt, "guerra", true, {}, p, 3000.0));
    s.unita.insert(s.unita.begin(), giocatore(false, p, 200.0, 5.5));
    s.unita.front().velocita = 3.0;
    RadioScenario r;
    r.ordini = OrdiniScenario{"COMSUBLANT", "USS LOS ANGELES (SSN 688)", "TRAILER T4", "SECRET"};
    r.roe = roe_tutte("trattenuto");
    s.radio = r;
    s.briefing[0] = {"PROVA T4", "Un Ka-27 caccia il 688 a quota periscopica, di giorno."};
    s.briefing[1] = {"TEST T4", "A Ka-27 hunts the 688 at periscope depth, by day."};
    return s;
}


// --- le partite su piu' semi ----------------------------------------------------------------------------------------

struct Enhanced {
    std::map<std::string, ClasseNave> classi = carica_classi(DATI + "/classi.toml");
    Missione m;
    DanniMissione dm;
    DanniSottomarini ds;
    Enhanced(const std::string& file, std::uint64_t seme) : m(file, seme), dm(classi, DATI, seme, RegolaDanni::simulazione), ds(classi, DATI, seme) {
        std::map<std::string, double> pescaggi;
        for (const auto& [id, c] : classi) pescaggi[id] = c.pescaggio_m;
        m.imposta_fondale(std::make_shared<const Fondale>(Fondale::carica(DATI + "/teatri/" + m.teatro() + ".fdl")), pescaggi);
        // la data della scena, come in partita (app/data_missione.hpp): il riassunto e' quello che si gioca
        const fs::path p(file);
        if (const auto d = data_della_missione(p.parent_path(), p.stem().string())) m.imposta_data_ora(d->giorno, d->mese, d->anno, d->ore_utc);
        m.radio_enhanced();
        if (const auto* g = m.giocatore()) ds.prepara(m, g->id);
    }
    void passo(double dt) {
        m.avanza(dt);
        dm.avanza(m, dt);
        ds.avanza(m, dt);
        m.raccogli_danni(&ds, &dm);
    }
};

// --- le scene del trailer n. 3 «A vista» sotto il ghiaccio (I16, Mare di Okhotsk, 14 marzo 1989, mezzogiorno locale) --------
// Il meteo del teatro, non fisso: il ghiaccio viene dai punti ERA5 di marzo (Meteo::in non li interpola col meteo fisso).
// Le riprese vanno girate con «--meteo realistico» e «--seme 1»: il campo del ghiaccio e' procedurale dal seme.

void base_okhotsk(ScenarioMissione& s, const DatiScenario& d, const Fondale& f, const std::string& id) {
    base(s, d, f, id, "kunashir", false);
    s.giorno = 14, s.mese = 3, s.ora_utc = 1.0;
    s.situazione = "pace";
    s.meteo_realistico = true;
    RadioScenario r;
    r.ordini = OrdiniScenario{"COMSUBPAC", "USS LOS ANGELES (SSN 688)", "TRAILER " + id, "SECRET"};
    r.roe = roe_tutte("trattenuto");
    s.radio = r;
}

// T5 — La volta (atto III del trailer n. 3): il 688 a 40 m sotto il pack del bastione, a 6 nodi verso nord, col Delta III
// 3 km a prua a 130 m. Col BQS-15 acceso (tasto bqs15) la pagina del fondale diventa quella della volta: le chiglie a prua
ScenarioMissione t5(const DatiScenario& d) {
    const Fondale f = Fondale::carica(DATI + "/teatri/kunashir.fdl");
    ScenarioMissione s;
    base_okhotsk(s, d, f, "PROVA_T5_VOLTA");
    s.titolo = {"T5 - La volta", "T5 - The ice canopy"};
    const Vec2 p = acqua(f, f.proiettata({44.85, 146.45}), 1200.0);
    s.unita.push_back(giocatore(false, p, 0.0, 40.0));
    s.unita.front().velocita = 6.0;
    UnitaScenario delta = unita(2, "delta3", "sottomarino", "urss", acqua(f, polare(p, 0.0, 3000.0), 1000.0), 0.0, 4.0, "");
    delta.quota = 130.0;
    delta.punti = {acqua(f, polare(p, 0.0, 30000.0), 1000.0)};
    delta.ruolo = "SSBN sotto il pack";
    s.unita.push_back(delta);
    s.briefing[0] = {"PROVA T5", "Il 688 sotto il pack di Okhotsk, col Delta III a prua: la volta e le chiglie."};
    s.briefing[1] = {"TEST T5", "The 688 under the Okhotsk pack, the Delta III ahead: the canopy and the keels."};
    return s;
}

// il centro della polynya larga piu' vicina a `p` (5 x 5 celle di 200 m aperte), nel campo del ghiaccio della missione `s`
// col seme 1 e il meteo del teatro, come le riprese
std::optional<Vec2> polynya_vicina(const ScenarioMissione& s, const DatiScenario& d, Vec2 p) {
    const auto file = fs::temp_directory_path() / ("m688_" + s.id + "_ghiaccio.lua");
    std::ofstream(file, std::ios::binary) << scrivi_missione_lua(s, d);
    Enhanced e(file.string(), 1);
    e.m.usa_meteo(Missione::ModoMeteo::realistico);
    for (int k = 0; k < 9; ++k) e.passo(1.0 / 9.0);
    const auto* g = e.m.ghiaccio();
    if (!g) return std::nullopt;
    for (double r = 0.0; r < 30000.0; r += 200.0)
        for (double a = 0.0; a < 360.0; a += r > 0.0 ? std::max(2.0, 200.0 / r * 57.3) : 360.0) {
            const Vec2 q = polare(p, a, r);
            bool aperta = true;
            for (int i = -2; i <= 2 && aperta; ++i)
                for (int j = -2; j <= 2 && aperta; ++j) {
                    const PuntoGhiaccio x = g->in({q.x + 200.0 * i, q.y + 200.0 * j}, e.m.tempo());
                    aperta = !x.ghiaccio || x.aperta;
                }
            if (aperta) return q;
        }
    return std::nullopt;
}

// T6 — La polynya (atto III del trailer n. 3): il 688 a quota periscopica in una polynya del bastione, fermo. Il punto lo
// cerca polynya_vicina nel campo del ghiaccio della missione (seme 1)
ScenarioMissione t6(const DatiScenario& d) {
    const Fondale f = Fondale::carica(DATI + "/teatri/kunashir.fdl");
    ScenarioMissione s;
    base_okhotsk(s, d, f, "PROVA_T6_POLYNYA");
    s.titolo = {"T6 - La polynya", "T6 - The polynya"};
    const Vec2 p = acqua(f, f.proiettata({44.85, 146.45}), 1200.0);
    s.unita.push_back(giocatore(false, p, 0.0, 5.5));
    s.unita.front().velocita = 1.0;
    if (const auto q = polynya_vicina(s, d, p)) s.unita.front().pos = *q;
    s.briefing[0] = {"PROVA T6", "Il 688 a quota periscopica in una polynya del pack di Okhotsk."};
    s.briefing[1] = {"TEST T6", "The 688 at periscope depth in a polynya of the Okhotsk pack."};
    return s;
}

// T7 — Il margine (atto III del trailer n. 3): il 688 a quota periscopica nell'acqua libera al margine, dal lato del
// Pacifico; l'Il-38 lo caccia (ASW aereo contro il giocatore, zona di 4 km attorno a lui): le boe solo in acqua libera
ScenarioMissione t7(const DatiScenario& d) {
    const Fondale f = Fondale::carica(DATI + "/teatri/kunashir.fdl");
    ScenarioMissione s;
    base_okhotsk(s, d, f, "PROVA_T7_MARGINE");
    s.titolo = {"T7 - Il margine", "T7 - The ice edge"};
    const Vec2 p = acqua(f, f.proiettata({43.90, 147.30}), 400.0);
    UnitaScenario il38 = unita(2, "il38", "aereo", "urss", polare(p, 330.0, 9000.0), 150.0, 250.0, "R_aereo");
    il38.ruolo = "Il-38 sul margine";
    s.unita.push_back(il38);
    s.aerei_asw.push_back(aereo(2, -1, f.proiettata({45.25, 147.90}), "pace", true, {}, p, 4000.0));
    s.unita.insert(s.unita.begin(), giocatore(false, p, 300.0, 5.5));
    s.unita.front().velocita = 3.0;
    s.briefing[0] = {"PROVA T7", "L'Il-38 caccia il 688 al margine del pack."};
    s.briefing[1] = {"TEST T7", "The Il-38 hunts the 688 at the edge of the pack."};
    return s;
}

// T8 — I fanali da vicino (atto II del trailer n. 3): la notte della I15 (Gibilterra, 3 giugno 1989, 2330Z, senza luna),
// con le navi a portata dell'oculare. Nella I15 i pescherecci lavorano a 6-10 km, la Kara passa oltre i 10 km e il
// mercantile senza governo e' a 4 km: al periscopio sono punti. Qui tre pescherecci illuminati a 0,9-2,2 km, la Kara a
// luci spente che taglia la prua a 1,8 km (dentro i 2,7 km della sua sagoma) e il mercantile coi due rossi a 1,5 km
ScenarioMissione t8(const DatiScenario& d) {
    const Fondale f = Fondale::carica(DATI + "/teatri/gibilterra.fdl");
    ScenarioMissione s;
    base(s, d, f, "PROVA_T8_FANALI", "gibilterra", false);
    s.titolo = {"T8 - I fanali da vicino", "T8 - Running lights up close"};
    s.giorno = 3, s.mese = 6, s.ora_utc = 23.5;
    s.situazione = "pace";
    MeteoFissoScenario m;   // la notte della I15
    m.stato_mare = 2.0, m.vento_m_s = 4.0, m.vento_da_gradi = 270.0, m.visibilita_m = 15000.0, m.nubi_ottavi = 2.0;
    s.meteo_fisso = m;
    const Vec2 p = acqua(f, f.proiettata({35.97, -5.05}), 300.0);
    s.unita.push_back(giocatore(false, p, 0.0, 5.5));
    s.unita.front().velocita = 1.0;
    const double RIL[] = {330.0, 350.0, 315.0}, DIST[] = {900.0, 1500.0, 2200.0};
    for (int k = 0; k < 3; ++k) {
        UnitaScenario pesca = unita(2 + k, "peschereccio", "nave", "neutrale", acqua(f, polare(p, RIL[k], DIST[k]), 200.0),
                                    std::fmod(45.0 + 90.0 * k, 360.0), 3.0, "", true);
        pesca.ruolo = "peschereccio";
        s.unita.push_back(pesca);
    }
    UnitaScenario kara = unita(5, "kara", "nave", "urss", acqua(f, polare(p, 50.0, 1800.0), 200.0), 270.0, 10.0, "");
    kara.punti = {acqua(f, polare(kara.pos, 270.0, 30000.0), 200.0)};
    kara.luci = "spente";
    kara.ruolo = "incrociatore a luci spente";
    s.unita.push_back(kara);
    UnitaScenario cargo = unita(6, "cargo", "nave", "neutrale", acqua(f, polare(p, 160.0, 1500.0), 200.0), 80.0, 0.0, "", true);
    cargo.luci = "non_governa";
    cargo.ruolo = "mercantile senza governo";
    s.unita.push_back(cargo);
    RadioScenario r;
    r.ordini = OrdiniScenario{"COMSUBLANT", "USS LOS ANGELES (SSN 688)", "TRAILER T8", "SECRET"};
    r.roe = roe_tutte("trattenuto");
    s.radio = r;
    s.briefing[0] = {"PROVA T8", "Di notte nello Stretto: pescherecci illuminati, la Kara a luci spente, un mercantile senza governo."};
    s.briefing[1] = {"TEST T8", "At night in the Strait: lit fishing boats, the darkened Kara, a ship not under command."};
    return s;
}

// T9 — La costa da vicino (la verifica della costa al periscopio, 30/10/2026): il 688 a quota periscopica a ~2,5 km
// dalla costa marocchina sotto il Jebel Musa (nel fondale le alture salgono a 300 m a 3 km dalla riva e a 600-900 m a
// 5-8 km), la prua a sud, con un peschereccio illuminato a 1,2 km fra il 688 e la riva. Il 19 giugno 1989 la Luna e'
// piena: alle 14 UTC il giorno (la scena), alle 20 il crepuscolo (Sole a -4 gradi), alle 23.30 la notte di luna (Luna
// a 24 gradi, azimut 160: alle spalle delle alture). Le istantanee, dalla radice del progetto:
//   melchior688 --dati data/motore --cartella-missioni data/missioni/prove --missione PROVA_T9_COSTA --regola simulazione
//     --seme 1 --tasti "0 periscopio" --rilevamento 180 --secondi 60 --istantanea costa.bmp [--data 1989-06-19 --ora 20]
ScenarioMissione t9(const DatiScenario& d) {
    const Fondale f = Fondale::carica(DATI + "/teatri/gibilterra.fdl");
    ScenarioMissione s;
    base(s, d, f, "PROVA_T9_COSTA", "gibilterra", false);
    s.titolo = {"T9 - La costa da vicino", "T9 - The coast up close"};
    s.giorno = 19, s.mese = 6, s.ora_utc = 14.0;
    s.situazione = "pace";
    s.durata_s = 1800.0;
    MeteoFissoScenario m;   // sereno e trasparente: la terra a 40 km
    m.stato_mare = 2.0, m.vento_m_s = 3.0, m.vento_da_gradi = 90.0, m.visibilita_m = 40000.0, m.nubi_ottavi = 0.0;
    s.meteo_fisso = m;
    const Vec2 p = acqua(f, f.proiettata({35.895, -5.42}), 200.0);
    s.unita.push_back(giocatore(false, p, 180.0, 5.5));
    s.unita.front().velocita = 1.0;
    UnitaScenario pesca = unita(2, "peschereccio", "nave", "neutrale", acqua(f, polare(p, 185.0, 1200.0), 50.0), 90.0, 2.0, "", true);
    pesca.ruolo = "peschereccio";
    s.unita.push_back(pesca);
    RadioScenario r;
    r.ordini = OrdiniScenario{"COMSUBLANT", "USS LOS ANGELES (SSN 688)", "PROVA T9", "SECRET"};
    r.roe = roe_tutte("trattenuto");
    s.radio = r;
    s.briefing[0] = {"PROVA T9", "Il 688 a 2,5 km dalla costa del Jebel Musa: le alture al periscopio di giorno, al crepuscolo e con la luna."};
    s.briefing[1] = {"TEST T9", "The 688 1.5 nm off the Jebel Musa coast: the high ground through the periscope by day, at dusk and by moonlight."};
    return s;
}

std::vector<ScenarioMissione> scene_trailer(const DatiScenario& d) { return {t1(d), t2(d), t3(d), t4(d), t5(d), t6(d), t7(d), t8(d), t9(d)}; }

// una partita: chi vede chi la prima volta (contatti dell'IA, ogni 10 s), chi e' distrutto o uscito, quando
void gioca(const fs::path& file, std::uint64_t seme, double secondi) {
    Enhanced e(file.string(), seme);
    if (const auto* g = e.m.giocatore()) {   // nessuno ai comandi: il giocatore si ferma e ascolta, senza finire sul fondo
        auto* u = e.m.trova(g->id);
        u->velocita_ordinata = 0.0;
        u->velocita_salvata = 0.0;
    }
    std::map<std::pair<int, int>, double> visto;   // solo i contatti fra parti ostili, armi escluse
    std::map<std::pair<int, int>, std::string> sensore;   // e con che cosa, la prima volta
    std::map<int, double> colpita, finito, incagliata;   // incagliata: le unita' dell'IA ferme su un fondale (M6 o rotta)
    std::map<int, std::string> classe;
    std::map<int, std::pair<double, std::string>> armi;
    const auto battello = [](const UnitaClassica& u) { return (u.flag & 0x20) == 0 && (u.categoria == "sottomarino" || u.categoria == "nave"); };
    for (double t = 0.0; t < secondi && !e.m.esito(); t += 10.0) {
        for (int k = 0; k < 90; ++k) e.passo(1.0 / 9.0);
        for (const auto& [id, u] : e.m.unita()) {
            if (u.flag & 0x20) {   // un'arma in acqua: si annota la prima volta
                if (!armi.count(id)) {
                    const auto da = e.m.unita().find(u.lanciatore), su = e.m.unita().find(u.bersaglio);
                    armi[id] = {e.m.tempo(), u.classe + " da " + std::to_string(u.lanciatore) + (da != e.m.unita().end() ? " " + da->second.classe : "") +
                                                 " su " + std::to_string(u.bersaglio) + (su != e.m.unita().end() ? " " + su->second.classe : "")};
                }
                continue;
            }
            classe[id] = u.classe;
            if (u.colpo && !colpita.count(id)) colpita[id] = e.m.tempo();
            if ((u.distrutta || u.persa) && !finito.count(id)) finito[id] = e.m.tempo();
            if (!u.giocatore && (u.incagliata || e.m.incagliata_ia(id)) && !incagliata.count(id)) incagliata[id] = e.m.tempo();
            for (const auto& [altro, c] : u.contatti) {
                // solo il contatto che l'IA puo' usare, come l'evento scoperta_ia del registro: traccia solida o posizione
                if (c.qualita < 800.0 && !c.posizione_nota) continue;
                const auto it = e.m.unita().find(altro);
                if (it == e.m.unita().end() || !battello(it->second) || ((u.flag ^ it->second.flag) & 0x18) != 0x18) continue;
                if (!visto.count({id, altro})) visto[{id, altro}] = e.m.tempo(), sensore[{id, altro}] = c.sensore;
            }
        }
    }
    const auto* g = e.m.giocatore();
    std::printf("  seme %llu: %.0f s%s%s", static_cast<unsigned long long>(seme), e.m.tempo(),
                e.m.esito() ? (", esito " + std::to_string(*e.m.esito())).c_str() : "", g && g->distrutta ? ", giocatore distrutto" : "");
    if (g) std::printf(", giocatore a %.0f m, scafo %d/%d", g->quota, g->integrita, g->integrita_max);
    std::printf("\n");
    std::vector<std::pair<double, std::string>> righe;
    char riga[160];
    for (const auto& [k, t] : visto) {
        std::snprintf(riga, sizeof riga, "    %5.0f s  %d %s vede %d %s (%s)", t, k.first, classe[k.first].c_str(), k.second, classe[k.second].c_str(),
                      sensore[k].c_str());
        righe.emplace_back(t, riga);
    }
    for (const auto& [id, t] : colpita) {
        std::snprintf(riga, sizeof riga, "    %5.0f s  %d %s colpita", t, id, classe[id].c_str());
        righe.emplace_back(t, riga);
    }
    for (const auto& [id, t] : finito) {
        std::snprintf(riga, sizeof riga, "    %5.0f s  %d %s fuori gioco", t, id, classe[id].c_str());
        righe.emplace_back(t, riga);
    }
    for (const auto& [id, t] : incagliata) {
        std::snprintf(riga, sizeof riga, "    %5.0f s  %d %s incagliata", t, id, classe[id].c_str());
        righe.emplace_back(t, riga);
    }
    for (const auto& [id, a] : armi) {
        std::snprintf(riga, sizeof riga, "    %5.0f s  arma in acqua: %s", a.first, a.second.c_str());
        righe.emplace_back(a.first, riga);
    }
    // dal registro: i colpi sul giocatore (anche quelli delle navi, che non mettono armi in acqua), gli affondamenti e
    // l'urto del giocatore col fondo
    std::map<std::pair<int, std::string>, std::pair<double, int>> colpi;   // (chi, arma) -> primo, quanti
    for (const auto& ev : e.m.eventi_missione()) {
        if (ev.tipo == TipoEvento::colpo && ev.unita == 0) {
            auto& c = colpi[{ev.altra, ev.classe_arma}];
            if (c.second++ == 0) c.first = ev.tempo;
        } else if (ev.tipo == TipoEvento::affondamento || ev.tipo == TipoEvento::incaglio) {
            std::snprintf(riga, sizeof riga, "    %5.0f s  %s: %d %s", ev.tempo, nome_evento(ev.tipo), ev.unita, ev.classe.c_str());
            righe.emplace_back(ev.tempo, riga);
        }
    }
    for (const auto& [k, c] : colpi) {
        std::snprintf(riga, sizeof riga, "    %5.0f s  giocatore colpito da %d %s (%s, %d colpi)", c.first, k.first, classe[k.first].c_str(),
                      k.second.c_str(), c.second);
        righe.emplace_back(c.first, riga);
    }
    std::stable_sort(righe.begin(), righe.end(), [](const auto& x, const auto& y) { return x.first < y.first; });
    for (const auto& r : righe) std::printf("%s\n", r.second.c_str());
    // i contatti del giocatore (registro): scoperti, classificati dal sonarista o a vista, e quelli mai classificati
    std::set<int> scoperti, classificati;
    for (const auto& ev : e.m.eventi_missione()) {
        if (ev.tipo == TipoEvento::scoperta) scoperti.insert(ev.unita);
        if (ev.tipo == TipoEvento::classificazione) classificati.insert(ev.unita);
    }
    std::string mai;
    for (const int id : scoperti)
        if (!classificati.count(id)) mai += " " + std::to_string(id) + " " + classe[id];
    std::printf("    contatti del giocatore: %zu scoperti, %zu classificati%s%s\n", scoperti.size(), classificati.size(),
                mai.empty() ? "" : "; mai classificati:", mai.c_str());
    if (!e.m.errori().empty()) std::printf("    ERRORI: %s\n", e.m.errori().front().c_str());
}

}  // namespace

// --- i vetrini pronti della sandbox (docs/19 §1, V3) ---------------------------------------------------------------------
// data/missioni/prove/vetrini.toml: per ogni scena cosa mostra nel vetrino e le regolazioni consigliate (i campi di
// RegolazioniSandbox in app/regolazioni_sandbox.hpp; quelli assenti restano della missione). Lo legge la scelta della
// missione. `pronto` falso: la scena aspetta una ritaratura (runtime e disassembler) e non si propone ancora.
struct Vetrino {
    const char* id;
    const char* mostra_it;
    const char* mostra_en;
    bool pronto;
};
const Vetrino VETRINI[] = {
    {"PROVA_S1_KPUG", "Un gruppo ASW sovietico trova uno Sturgeon alleato: il VDS lo sente subito, la traccia solida arriva per integrazione.",
     "A Soviet ASW group finds an allied Sturgeon: the VDS hears her at once, the solid track comes with integration.", true},
    {"PROVA_S1G_KPUG_GUERRA", "La stessa scena in guerra: le navi cacciano fino al PROBSUB, poi attaccano con le salve antisom.",
     "The same scene at war: the ships hunt until PROBSUB, then attack with ASW salvos.", true},
    {"PROVA_S2_PEDINAMENTO", "Una Perry col sonar trainato pedina un Victor III in pace; il P-3 arriva sul suo contatto.",
     "A Perry with her towed array trails a Victor III in peacetime; the P-3 comes onto her contact.", true},
    {"PROVA_S3_BASTIONE", "Un intruso nel bastione: Il-38, gruppo ASW e un Victor III lo cercano insieme.",
     "An intruder in the bastion: Il-38, ASW group and a Victor III hunt him together.", true},
    {"PROVA_S3A_BASTIONE_ALFA", "Il bastione visto dall'Alfa: il gruppo ASW trova l'intruso in pochi minuti.",
     "The bastion seen from the Alfa: the ASW group finds the intruder within minutes.", true},
    {"PROVA_S4_DUE_SSN", "Due sottomarini amici in aree vicine; il Trafalgar e un Victor III si trovano e si sparano.",
     "Two friendly submarines in adjacent areas; the Trafalgar and a Victor III find each other and fight.", true},
    {"PROVA_S5_CONVOGLIO", "Un Victor attacca un convoglio: una petroliera colpita, le scorte che cercano il sottomarino.",
     "A Victor attacks a convoy: a tanker hit, the escorts hunting the submarine.", true},
    {"PROVA_S6_USCITA", "Un Typhoon esce da Kola con la nave spia e i pescherecci; un SSN aspetta fuori.",
     "A Typhoon sails from Kola past a spy ship and trawlers; an SSN waits outside.", true},
    {"PROVA_S7_PORTAEREI", "Un gruppo portaerei e un Victor: la crisi diventa guerra, e ogni unita' lo sa in tempi diversi.",
     "A carrier group and a Victor: the crisis turns into war, and each unit learns it at a different time.", true},
    {"PROVA_S8_LUOGO", "La parte del luogo: una Koni in crisi con gli USA e in pace con l'URSS caccia solo il 688, e a 15 minuti dichiara la guerra.",
     "The local party: a Koni in crisis with the US and at peace with the USSR hunts only the 688, and after 15 minutes declares war.", true},
    // le scene dei missili (docs/21 §11.13): in ritaratura finche' il comandante IA non tira i missili del giocatore
    {"PROVA_M3_ORIZZONTE", "Uno Slava e due Perry a 120 km, in guerra: nessuno vede l'altro. Da sola non succede nulla; e' la scena dell'esperimento della spia.",
     "A Slava and two Perrys 120 km apart, at war: neither sees the other. Alone nothing happens; it is the scene of the spy experiment.", true},
    // la M4 non e' fra i vetrini: non mostra la caccia sul dato, perche' il Victor sente il convoglio da solo (tools/re/prove/quadro_radio/proposta.md §4)
    {"PROVA_M1_ANTINAVE", "Harpoon su uno Slava con due Krivak e un cargo neutrale dentro la scatola: il seeker prende il primo eco.",
     "Harpoon at a Slava with two Krivaks and a neutral freighter inside the box: the seeker takes the first echo.", false},
    {"PROVA_M2_TLAM", "Una missione TLAM dalla libreria di bordo: l'area di lancio, la navigazione incerta e l'ordine dalla radio.",
     "A TLAM mission from the onboard library: the launch area, uncertain navigation and the order by radio.", false},
};

void scrivi_vetrini(const fs::path& cartella, const std::vector<ScenarioMissione>& scene_) {
    const auto stringa = [](const std::string& x) {
        std::string o = "\"";
        for (const char c : x) o += (c == '"' || c == '\\') ? std::string("\\") + c : std::string(1, c);
        return o + "\"";
    };
    std::ostringstream o;
    o << "# I vetrini pronti della sandbox (docs/19 §1): scritto da strumenti/scene_interazioni, non modificare a mano.\n"
         "# missione, titolo e cosa mostra (it/en), pronto (falso: in ritaratura), durata in minuti, regolazioni consigliate\n"
         "# (i campi di RegolazioniSandbox; quelli assenti restano della missione)\n";
    for (const Vetrino& v : VETRINI) {
        const auto it = std::find_if(scene_.begin(), scene_.end(), [&](const ScenarioMissione& x) { return x.id == v.id; });
        if (it == scene_.end()) throw std::runtime_error(std::string("vetrino senza scena: ") + v.id);
        o << "\n[[vetrino]]\nmissione = " << stringa(v.id) << "\ntitolo = { it = " << stringa(it->titolo[0]) << ", en = " << stringa(it->titolo[1])
          << " }\nmostra = { it = " << stringa(v.mostra_it) << ", en = " << stringa(v.mostra_en) << " }\npronto = " << (v.pronto ? "true" : "false")
          << "\ndurata_min = " << static_cast<int>(std::lround(it->durata_s / 60.0))
          << "\n[vetrino.regolazioni]\nmeteo_realistico = false   # le scene hanno il meteo fisso\nnavigazione_realistica = false\n"
             "ferma_all_esito = false\n";
    }
    std::ofstream f(cartella / "vetrini.toml", std::ios::binary);
    if (!f) throw std::runtime_error("impossibile scrivere vetrini.toml");
    f << o.str();
}

int main(int argc, char** argv) {
    try {
        fs::path uscita = fs::path(M688_MISSIONI) / "prove";
        int semi = 0;
        double secondi = 0.0;   // 0: la durata della scena
        std::string sola;
        bool scena_apertura = false, con_uscita = false;
        std::vector<fs::path> file;   // --file F: gioca un file di missione qualunque (il collaudo della libreria), coi semi e i secondi dati
        for (int i = 1; i < argc; ++i) {
            const std::string a = argv[i];
            if (a == "--uscita" && i + 1 < argc) uscita = argv[++i], con_uscita = true;
            else if (a == "--file" && i + 1 < argc) file.emplace_back(argv[++i]);
            else if (a == "--semi" && i + 1 < argc) semi = std::stoi(argv[++i]);
            else if (a == "--secondi" && i + 1 < argc) secondi = std::stod(argv[++i]);
            else if (a == "--scena" && i + 1 < argc) sola = argv[++i];
            else if (a == "--apertura") scena_apertura = true;
        }
        const DatiScenario d = carica_dati_scenario(DATI);
        if (scena_apertura) {   // solo le due scene del video d'apertura, non si giocano ne' vanno fra le prove
            const fs::path dove = con_uscita ? uscita : fs::path(DATI + "/apertura");
            fs::create_directories(dove);
            for (const bool alfa : {false, true}) {
                const ScenarioMissione s = apertura(d, alfa);
                const Fondale f = Fondale::carica(DATI + "/teatri/" + s.teatro + ".fdl");
                for (const auto& a : verifica(s, f, d))
                    if (a.gravita == AvvisoScenario::Gravita::errore) throw std::runtime_error(s.id + ": " + a.testo);
                std::ofstream(dove / (s.id + ".lua"), std::ios::binary) << scrivi_missione_lua(s, d);
                std::printf("%s\n", s.id.c_str());
            }
            return 0;
        }
        if (!file.empty()) {
            for (const auto& f : file) {
                const double durata = secondi > 0.0 ? secondi : 3600.0;
                std::printf("%s (%.0f s)\n", f.stem().string().c_str(), durata);
                for (int k = 1; k <= std::max(semi, 1); ++k) gioca(f, static_cast<std::uint64_t>(k), durata);
            }
            return 0;
        }
        if (semi > 0) {
            for (const auto& s : tutte(d)) {
                if (!sola.empty() && s.id.find(sola) == std::string::npos) continue;
                const double durata = secondi > 0.0 ? secondi : s.durata_s;
                std::printf("%s (%s, %.0f s)\n", s.id.c_str(), s.situazione.c_str(), durata);
                for (int k = 1; k <= semi; ++k) gioca(uscita / (s.id + ".lua"), static_cast<std::uint64_t>(k), durata);
            }
            return 0;
        }
        fs::create_directories(uscita);
        for (const auto& s : tutte(d)) {   // navigazione, tlam ed esecuzione_tlam (M2) dallo scrittore
            const Fondale f = Fondale::carica(DATI + "/teatri/" + s.teatro + ".fdl");
            int errori = 0;
            for (const auto& a : verifica(s, f, d)) {
                std::printf("  %s %s: %s\n", s.id.c_str(), a.gravita == AvvisoScenario::Gravita::errore ? "ERRORE" : "attenzione", a.testo.c_str());
                errori += a.gravita == AvvisoScenario::Gravita::errore;
            }
            if (errori) throw std::runtime_error(s.id + ": la scena ha errori");
            std::ofstream(uscita / (s.id + ".lua"), std::ios::binary) << scrivi_missione_lua(s, d);
            std::printf("%s: %s\n", s.id.c_str(), s.titolo[0].c_str());
        }
        scrivi_vetrini(uscita, tutte(d));
        std::printf("vetrini: %s\n", (uscita / "vetrini.toml").c_str());
        return 0;
    } catch (const std::exception& e) {
        std::fprintf(stderr, "scene_interazioni: %s\n", e.what());
        return 2;
    }
}
