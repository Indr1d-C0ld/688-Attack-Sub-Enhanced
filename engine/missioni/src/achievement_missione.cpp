#include "missioni/achievement_missione.hpp"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <map>
#include <set>
#include <vector>

#include "missioni/registro_missione.hpp"

namespace m688 {

namespace {

constexpr double DOPPIETTA_S = 60.0;           // due affondamenti entro un minuto
constexpr double SOTTOMARINO_VICINO_M = 900.0; // il passaggio del Sovereign (dossier 16, 800 yd)
// docs/26 §2
constexpr double TELEMETRO_ENTRO = 0.05, AOB_ENTRO_GRADI = 10.0;
constexpr double ALLARME_PRIMA_S = 600.0;      // un siluro evitato vale per l'allarme se e' salito entro 10 minuti prima
// l'orizzonte radar del battello verso una nave (STIMA: l'antenna a 7 m, le sovrastrutture a 20 m;
// 2,08 nm x (radice 7 + radice 20)): oltre, il bersaglio non si vede, e il dato e' della TMA o della radio
constexpr double ORIZZONTE_RADAR_M = 27400.0;
constexpr double MINA_AGGIRATA_M = 200.0;      // una mina scoperta e mai avvicinata a meno di 200 m: aggirata
const std::set<std::string> ELICOTTERI = {"sh60b", "sh60f", "sh2f", "sh3h", "lynx", "ka25", "ka27", "hkp4"};

// le misure del fascicolo (ambito carriera)
void misure_fascicolo(const Fascicolo& f, const RegoleCarriera* regole, std::map<std::string, double>& v) {
    v["qualifica_sommergibilista"] = std::any_of(f.qualifiche.begin(), f.qualifiche.end(),
                                                 [](const Qualifica& q) { return q.id == "dolphins" || q.id == "vsok"; });
    v["missioni_carriera"] = static_cast<double>(f.missioni.size());
    v["pensione"] = f.stato == "pensione";
    if (!regole) return;
    if (const Grado* gr = regole->grado(f.marina, f.grado)) v["grado_ordine"] = gr->ordine;
    int personali = 0, massima = 0;
    for (const Conferimento& k : f.decorazioni) {
        const Decorazione* d = regole->decorazione(k.decorazione);
        personali += d && d->tipo == "personale";
        const std::string& id = k.decorazione;
        const int livello = id == "medal_of_honor" || id == "eroe_urss" ? 3
                            : id == "navy_cross" || id == "lenin"        ? 2
                            : id == "silver_star" || id == "bandiera_rossa" ? 1
                                                                           : 0;
        massima = std::max(massima, livello);
    }
    v["decorazioni_personali"] = personali;
    v["decorazione_massima"] = massima;
}

}  // namespace

Misure misure_achievement(const Missione& m, const ContestoMissione& c, const OpzioniMisure& o) {
    const EsitoDettagliato x = m.esito_dettagliato();
    Misure r;
    auto& v = r.v;
    v["uno"] = 1.0;
    // esito
    v["riuscita"] = x.riuscita;
    v["completezza"] = x.completezza;
    v["successo"] = x.categoria == EsitoDettagliato::Categoria::successo;
    v["battello_perso"] = x.battello_perso;
    v["avversario"] = std::min(c.avversario, c.avversario_calcolato);
    v["lanci"] = x.lanci;
    v["lanci_a_segno"] = x.lanci_a_segno;
    v["tutti_a_segno"] = x.lanci > 0 && x.lanci_a_segno >= x.lanci ? x.lanci : 0;
    v["siluri_evitati"] = x.siluri_evitati;
    v["colpi_subiti"] = x.colpi_subiti;
    v["distanza_min_siluro_m"] = x.distanza_min_siluro_m;
    v["mine_posate"] = x.mine_posate;
    v["mine_inerti"] = x.mine_inerti;
    // affondamenti ostili del giocatore (per categoria) e la doppietta; trasmissioni; passaggi ravvicinati
    std::vector<double> affondati;
    int sottomarini = 0, navi = 0, trasmissioni = 0;
    bool vicino = false, ignota = false;
    for (const EventoMissione& e : m.eventi_missione()) {
        if (e.tipo == TipoEvento::affondamento && e.lato == Lato::ostile) {
            const auto b = x.per_bersaglio.find(e.unita);
            if (b == x.per_bersaglio.end() || !b->second.dal_giocatore) continue;
            affondati.push_back(e.tempo);
            sottomarini += e.categoria == "sottomarino";
            navi += e.categoria == "nave";
        } else if (e.tipo == TipoEvento::trasmissione) {
            ++trasmissioni;
        } else if (e.tipo == TipoEvento::quasi_collisione) {
            if (e.codice == 0) ignota = true;   // codice 1: lo sapevamo
            if (e.categoria == "sottomarino" && e.lato == Lato::ostile && e.valore < SOTTOMARINO_VICINO_M) vicino = true;
        }
    }
    std::sort(affondati.begin(), affondati.end());
    bool doppietta = false;
    for (std::size_t i = 1; i < affondati.size(); ++i) doppietta = doppietta || affondati[i] - affondati[i - 1] <= DOPPIETTA_S;
    v["affondati_ostili"] = static_cast<double>(affondati.size());
    v["sottomarini_affondati"] = sottomarini;
    v["navi_affondate"] = navi;
    v["doppietta"] = doppietta;
    // esposizione: il fantasma a tre gradini (nessuna localizzazione; poi nessun contatto ASW; poi nessun rumore proprio)
    int fantasma = 0;
    if (x.fix_subiti == 0 && x.avvistamenti == 0 && x.ping_subiti == 0) {
        fantasma = 1;
        if (x.tempo_contatto_asw_s <= 0.0) {
            fantasma = 2;
            if (x.transitori == 0 && x.ping_attivi == 0) fantasma = 3;
        }
    }
    v["fantasma"] = fantasma;
    v["trasmissioni"] = trasmissioni;
    v["serie_copiate"] = x.serie_copiate;
    v["serie_perse"] = x.serie_perse;
    v["rapporti_mancati"] = std::max(0, x.rapporti_richiesti - x.rapporti_inviati);
    v["avvistamenti"] = x.avvistamenti;
    v["boe_vicine"] = x.boe_vicine;
    v["ping_attivi"] = x.ping_attivi;
    v["sottomarino_vicino"] = vicino;
    v["esposizione_periscopio_s"] = x.esposizione_periscopio_s;
    v["quasi_collisione_ignota"] = ignota;
    // tecnica
    v["classificazioni_giuste"] = x.classificazioni_giuste;
    v["classificazioni_errate"] = x.classificazioni_errate;
    v["anticipo_medio_s"] = x.classificazioni_giuste > 0 ? x.anticipo_medio_s : 1e9;
    v["danni_domati"] = x.danni_domati;
    v["margine_collasso"] = x.margine_collasso;
    v["foto_periscopio"] = x.foto_periscopio;
    v["ordini"] = x.ordini;
    v["ordini_mancati"] = std::max(0, x.ordini - x.ordini_eseguiti);
    v["ordini_rispettati"] = x.ordini_rispettati && !c.fuoco_senza_ordine;
    v["incaglio"] = x.incaglio;
    v["armi_a_bordo"] = o.armi_a_bordo;
    // contesto
    v["originale"] = c.origine == "originale";
    v["rapida"] = c.origine == "rapida";
    v["editor"] = c.origine == "editor";
    v["libreria"] = c.origine == "libreria";
    v["artico"] = c.artico;
    v["pace"] = c.situazione == Situazione::pace;
    v["crisi"] = c.situazione == Situazione::crisi;
    v["guerra"] = c.situazione == Situazione::guerra;
    const UnitaClassica* g = m.giocatore();
    const bool vmf = o.fascicolo ? o.fascicolo->marina == Marina::vmf : g && g->classe == "alfa";
    v["usn"] = !vmf;
    v["vmf"] = vmf;
    v["realismo"] = c.realismo;
    // docs/26 §2: libretto, telemetro, AOB, missili, intercettatore, chiglia, navigazione, radio, mine, mare, artiglieria
    std::map<int, bool> visiva_ultima;          // l'ultima identificazione visiva del giocatore per unita'
    std::map<int, double> visiva_giusta_t, sonar_t;   // la prima visiva giusta e la prima classificazione non visiva
    std::vector<double> allarmi;
    std::map<int, double> missile_oth;          // missile del giocatore -> distanza al lancio
    std::set<int> colpiti_oth;
    int telemetro = 0, aob = 0, salve_neutrale = 0, non_autentici = 0, attraversamenti = 0, artiglieria = 0;
    ContaSpari spari;
    for (const EventoMissione& e : m.eventi_missione()) {
        switch (e.tipo) {
            case TipoEvento::classificazione:
                if (e.visiva && e.valore == 1.0) {
                    visiva_ultima[e.unita] = e.giusta;
                    if (e.giusta && !visiva_giusta_t.count(e.unita)) visiva_giusta_t[e.unita] = e.tempo;
                } else if (!sonar_t.count(e.unita)) {
                    sonar_t[e.unita] = e.tempo;
                }
                break;
            case TipoEvento::misura_telemetro:
                telemetro += e.distanza_m > 0.0 && std::fabs(e.valore - e.distanza_m) / e.distanza_m <= TELEMETRO_ENTRO;
                break;
            case TipoEvento::osservazione_aob: aob += e.valore <= AOB_ENTRO_GRADI; break;
            case TipoEvento::lancio_giocatore:
                if (e.livello > 0) {   // un missile del modello
                    salve_neutrale += e.valore > 0.0;
                    if (e.distanza_m > ORIZZONTE_RADAR_M) missile_oth[e.arma] = e.distanza_m;
                }
                break;
            case TipoEvento::colpo:
                if (e.altra == (g ? g->id : -1) && missile_oth.count(e.arma)) colpiti_oth.insert(e.unita);
                break;
            case TipoEvento::allarme_siluro: allarmi.push_back(e.tempo); break;
            case TipoEvento::verifica_radio: non_autentici += !e.giusta; break;
            case TipoEvento::attraversamento_mine: ++attraversamenti; break;
            case TipoEvento::artiglieria_colpo: artiglieria += spari.nuovo(e.codice); break;
            default: break;
        }
    }
    int visive = 0, prima_vista = 0, oth = 0, evitati_allarme = 0;
    for (const auto& [id, giusta] : visiva_ultima) visive += giusta;
    for (const auto& [id, t] : visiva_giusta_t) {
        const auto s = sonar_t.find(id);
        prima_vista += s == sonar_t.end() || t < s->second;
    }
    for (const EventoMissione& e : m.eventi_missione()) {
        if (e.tipo == TipoEvento::affondamento && e.lato == Lato::ostile && colpiti_oth.count(e.unita)) {
            const auto b = x.per_bersaglio.find(e.unita);
            oth += b != x.per_bersaglio.end() && b->second.dal_giocatore;
        }
        if (e.tipo == TipoEvento::siluro_evitato)
            evitati_allarme += std::any_of(allarmi.begin(), allarmi.end(), [&](double a) { return a <= e.tempo && e.tempo - a <= ALLARME_PRIMA_S; });
    }
    const auto& conti = m.conti_giocatore();
    v["visive_giuste"] = visive;
    v["telemetro_entro_5"] = telemetro;
    v["aob_entro_10"] = aob;
    v["prima_la_vista"] = prima_vista;
    v["affondati_missile_oth"] = oth;
    v["salve_neutrale_illeso"] = x.neutrali_colpiti == 0 ? salve_neutrale : 0;
    v["siluri_evitati_allarme"] = evitati_allarme;
    v["sotto_chiglia_20_s"] = conti.sotto_chiglia_20_s;
    v["navigazione_incerta"] = m.con_navigazione();
    v["gps"] = m.gps();
    if (g) {
        const Vec2 st = m.posizione_stimata(g->id);   // la stima meno la vera
        v["errore_finale_m"] = std::hypot(st.x - g->pos.x, st.y - g->pos.y);
    }
    v["non_autentici_respinti"] = non_autentici;
    v["campi_minati_attraversati"] = attraversamenti;
    v["mare_max"] = conti.mare_max;
    v["colpi_artiglieria_subiti"] = artiglieria;
    v["righe_nuove_acint"] = 0;   // il rapporto ACINT e' dell'app (la principale): da collegare
    // il secondo giro (31/10/2026; nota disassemblatore-achievement-31-10-proposta.md §8): ghiaccio, notte, coda, mine,
    // elicotteri, traino, soccorso, la libreria storica e le classi
    v["emersioni_ghiaccio"] = conti.emersioni_ghiaccio;
    v["sotto_banchisa_s"] = conti.sotto_banchisa_s;
    v["visive_fanali_giuste"] = conti.visive_fanali_giuste;
    v["visive_luci_spente"] = conti.visive_luci_spente;
    v["in_coda_s"] = conti.in_coda_s;
    v["entro_500yd_s"] = conti.entro_500yd_s;
    v["navi_traino_vicine"] = static_cast<double>(conti.navi_traino_vicine.size());
    int urti_ghiaccio = 0;
    for (const auto& u : m.urti()) urti_ghiaccio += g && u.urto.a.unita == g->id && u.urto.b.unita < 0;
    v["urti_ghiaccio"] = urti_ghiaccio;
    int collisioni = 0, mine = 0, sorvoli_elicottero = 0;
    std::set<std::string> classi;
    for (const EventoMissione& e : m.eventi_missione()) {
        collisioni += e.tipo == TipoEvento::collisione;
        mine += e.tipo == TipoEvento::mina_scoperta;
        sorvoli_elicottero += e.tipo == TipoEvento::aereo_in_sorvolo && ELICOTTERI.count(e.classe);
        if (e.tipo == TipoEvento::classificazione && e.valore == 1.0 && e.giusta && !e.classe.empty()) classi.insert(e.classe);
    }
    v["collisioni"] = collisioni;
    v["mine_scoperte"] = mine;
    v["mine_aggirate"] = static_cast<double>(std::count_if(conti.mine_vicine_m.begin(), conti.mine_vicine_m.end(),
                                                           [](const auto& k) { return k.second >= MINA_AGGIRATA_M; }));
    v["sorvoli_elicottero"] = sorvoli_elicottero;
    v["tutti_salvati"] = m.salvati().dispersi > 0 && m.salvati().salvati >= m.salvati().dispersi;
    v["libreria_storica"] = c.origine == "libreria" && c.missione.rfind("LIB_S", 0) == 0;
    std::string elenco;
    for (const auto& k : classi) elenco += (elenco.empty() ? "" : "|") + k;
    r.chiavi["classi_giuste"] = elenco;
    r.chiavi["missione"] = c.missione;
    r.chiavi["teatro"] = c.teatro;
    r.chiavi["marina"] = vmf ? "vmf" : "usn";
    if (c.origine == "rapida" && c.seme != 0) {
        char codice[24];
        std::snprintf(codice, sizeof codice, "%016llx", static_cast<unsigned long long>(c.seme));
        r.chiavi["codice"] = codice;
    }
    // fascicolo: dopo la missione, e prima (gli achievement di carriera contano solo quello che la missione cambia)
    if (o.fascicolo) misure_fascicolo(*o.fascicolo, o.regole, r.v);
    if (o.fascicolo_prima) misure_fascicolo(*o.fascicolo_prima, o.regole, r.prima);
    return r;
}

}  // namespace m688
