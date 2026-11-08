#include "nucleo/statistiche.hpp"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <set>
#include <stdexcept>

#include <toml.hpp>

#include "nucleo/albo.hpp"
#include "nucleo/artiglieria.hpp"
#include "nucleo/geometria.hpp"

namespace m688 {

namespace {

constexpr double BUCO_S = 30.0;                  // fra due campioni piu' lontani di cosi' non si integra
constexpr double MINUTO_S = 60.0;                // un punto del livello irradiato al minuto
constexpr double METRI_PER_MIGLIO = 1852.0;
constexpr int ALBERO_SCHIERA = 0x01, ALBERO_PERISCOPIO = 0x02, ALBERO_ANTENNA = 0x04;
constexpr int CODICE_SONARISTA = 411;            // le classificazioni del sonarista dell'equipaggio (registro M1)
constexpr int CODICE_VISIVA = 2;                 // quelle visive del giocatore, dal libretto (docs/25)
constexpr double ALLARME_PRIMA_S = 600.0;        // un siluro evitato vale per l'allarme salito entro 10 minuti prima
// gli elicotteri ASW (dei sorvoli): gli altri aerei sono ad ala fissa
const std::set<std::string> ELICOTTERI = {"sh60b", "sh60f", "sh2f", "sh3h", "lynx", "ka25", "ka27", "hkp4"};

std::pair<int, int> cella(Vec2 p, double lato) {
    return {static_cast<int>(std::floor(p.x / lato)), static_cast<int>(std::floor(p.y / lato))};
}

int giocatore_della(const Traccia& t, int g) {
    if (g >= 0) return g;
    if (const auto it = t.metadati.find("giocatore"); it != t.metadati.end()) return std::atoi(it->second.c_str());
    return 0;
}

// il campione di una serie piu' vicino a tk (nullptr se piu' lontano di BUCO_S)
const StatoUnita* vicino_a(const std::vector<std::pair<double, StatoUnita>>& serie, double tk) {
    const auto it = std::lower_bound(serie.begin(), serie.end(), tk, [](const auto& x, double t) { return x.first < t; });
    const std::pair<double, StatoUnita>* m = nullptr;
    if (it != serie.end()) m = &*it;
    if (it != serie.begin() && (!m || tk - std::prev(it)->first < m->first - tk)) m = &*std::prev(it);
    return m && std::fabs(m->first - tk) <= BUCO_S ? &m->second : nullptr;
}

// il secondo giro (31/10/2026): i metadati «conti.*», le mine, i sorvoli, i siluri aerei, le classi, il sottomarino piu' vicino
void secondo_giro(const Traccia& t, int g, ReportMissione& r) {
    const auto conto = [&](const char* nome) {
        const auto it = t.metadati.find(std::string("conti.") + nome);
        return it == t.metadati.end() ? 0.0 : std::atof(it->second.c_str());
    };
    r.sotto_banchisa_s = conto("sotto_banchisa_s");
    r.emersioni_ghiaccio = static_cast<int>(conto("emersioni_ghiaccio"));
    r.urti_ghiaccio = static_cast<int>(conto("urti_ghiaccio"));
    r.avvisi_chiglia = static_cast<int>(conto("avvisi_chiglia"));
    r.visive_notte_fanali = static_cast<int>(conto("visive_fanali_giuste"));
    r.visive_luci_spente = static_cast<int>(conto("visive_luci_spente"));
    r.visive_notte_sagoma = static_cast<int>(conto("visive_notte_sagoma"));
    r.errate_luci_spente = static_cast<int>(conto("errate_luci_spente"));
    r.luci_accese_s = conto("luci_accese_s");
    r.in_coda_s = conto("in_coda_s");
    r.classificati_da_sottomarini = static_cast<int>(conto("classificati_da_sottomarini"));
    r.mine_aggirate = static_cast<int>(conto("mine_aggirate"));
    r.salvati = static_cast<int>(conto("salvati"));
    const auto sm = t.metadati.find("conti.sensore_mine");
    const std::string sensore = sm != t.metadati.end() ? sm->second : std::string("bqs15");
    const auto categoria_di = [&](int id) {
        const auto it = t.unita.find(id);
        return it == t.unita.end() || it->second.empty() ? std::string() : it->second.front().second.categoria;
    };
    for (const EventoTraccia& e : t.registro) {
        if (e.tipo == "mina_scoperta") {
            ++r.mine_per_sensore[sensore];
            e.livello == 1 ? ++r.mine_ormeggiate : ++r.mine_di_fondo;
        } else if (e.tipo == "aereo_in_sorvolo") {
            ELICOTTERI.count(e.classe) ? ++r.sorvoli_elicotteri : ++r.sorvoli_aerei;
        } else if (e.tipo == "siluro_evitato") {   // l'arma, e chi l'ha lanciata dalla sua serie
            const auto a = t.unita.find(e.unita);
            if (a != t.unita.end() && !a->second.empty() && categoria_di(a->second.front().second.lanciatore) == "aereo") ++r.siluri_aerei_evitati;
        } else if (e.tipo == "classificazione" && e.codice != CODICE_SONARISTA && (e.livello >= 2 || e.valore != 0.0) && e.giusta && !e.classe.empty()) {
            r.classi_giuste.insert(e.classe);
        }
    }
    // il sottomarino straniero piu' vicino: sui suoi campioni, contro quello del giocatore piu' vicino nel tempo
    const auto gi = t.unita.find(g);
    if (gi == t.unita.end() || gi->second.empty()) return;
    const int lato = gi->second.front().second.flag & 0x18;
    for (const auto& [id, serie] : t.unita) {
        if (id == g || serie.empty() || serie.front().second.categoria != "sottomarino" || (serie.front().second.flag & 0x18) == lato) continue;
        for (const auto& [tk, u] : serie) {
            if (u.distrutta || u.rimossa) continue;
            if (const StatoUnita* p = vicino_a(gi->second, tk)) {
                const double d = distanza(p->pos, u.pos);
                if (r.distanza_min_sottomarino_m <= 0.0 || d < r.distanza_min_sottomarino_m) r.distanza_min_sottomarino_m = d;
            }
        }
    }
}

}  // namespace

ReportMissione report_missione(const Traccia& t, const CatalogoAcustico* cat, int giocatore) {
    ReportMissione r;
    r.missione = t.missione;
    if (const auto it = t.metadati.find("teatro"); it != t.metadati.end()) r.teatro = it->second;
    const int g = giocatore_della(t, giocatore);
    // la verita' del battello: tempi, quote, velocita', alberi, cavitazione, livello irradiato, heatmap
    if (const auto it = t.unita.find(g); it != t.unita.end() && !it->second.empty()) {
        const auto& serie = it->second;
        r.classe = serie.front().second.classe;
        const FirmaRumore* firma = cat ? cat->firma(r.classe) : nullptr;
        double energia = 0.0, tempo_livello = 0.0, prossimo_minuto = serie.front().first;
        double quota_tempo = 0.0, velocita_tempo = 0.0;
        for (std::size_t k = 0; k < serie.size(); ++k) {
            const auto& [tk, u] = serie[k];
            r.quota_max_m = std::max(r.quota_max_m, u.quota);
            r.velocita_max_kn = std::max(r.velocita_max_kn, u.velocita_kn);
            double livello = 0.0;
            if (firma) {
                Sorgente s;
                s.firma = firma;
                s.velocita_kn = u.velocita_kn;
                s.quota_m = u.quota;
                s.sul_fondo = u.sul_fondo;
                livello = livello_banda_larga(s);
                r.livello_max_db = std::max(r.livello_max_db, livello);
                if (tk >= prossimo_minuto - 1e-9) {
                    r.livello_irradiato.emplace_back(tk, livello);
                    prossimo_minuto = tk + MINUTO_S;
                }
            }
            if (u.rimossa || k + 1 >= serie.size()) continue;
            const double dt = serie[k + 1].first - tk;
            if (dt <= 0.0 || dt > BUCO_S) continue;
            r.durata_s += dt;
            r.distanza_nm += distanza(u.pos, serie[k + 1].second.pos) / METRI_PER_MIGLIO;
            quota_tempo += u.quota * dt;
            velocita_tempo += u.velocita_kn * dt;
            if (u.quota < QUOTA_SUPERFICIE_M) r.superficie_s += dt;
            else if (u.quota <= QUOTA_PERISCOPICA_MAX_M) r.periscopica_s += dt;
            if (u.alberi & ALBERO_PERISCOPIO) r.periscopio_s += dt;
            if (u.alberi & ALBERO_ANTENNA) r.antenna_s += dt;
            if (u.alberi & ALBERO_SCHIERA) r.schiera_s += dt;
            if (firma && u.quota >= QUOTA_SUPERFICIE_M && cavita(*firma, u.velocita_kn, u.quota)) r.cavitazione_s += dt;
            if (firma) {
                energia += std::pow(10.0, livello / 10.0) * dt;
                tempo_livello += dt;
            }
            r.heatmap[cella(u.pos, CELLA_MISSIONE_M)] += dt;
        }
        if (r.durata_s > 0.0) {
            r.quota_media_m = quota_tempo / r.durata_s;
            r.velocita_media_kn = velocita_tempo / r.durata_s;
        }
        if (tempo_livello > 0.0) r.livello_medio_db = 10.0 * std::log10(energia / tempo_livello);
    }
    // gli eventi: trasmissioni e ping, armi, la prima scoperta nemica, i nostri contatti
    std::set<int> colpiti_dal_giocatore, scoperti, classificati, missili;
    ContaSpari spari;
    double somma_classificazione = 0.0, somma_telemetro = 0.0, somma_aob = 0.0, somma_nav = 0.0;
    int con_tempo = 0;
    std::vector<double> allarmi;
    for (const EventoTraccia& e : t.registro) {
        // docs/26 §3
        if (e.tipo == "misura_telemetro" && e.distanza_m > 0.0) {
            ++r.misure_telemetro;
            somma_telemetro += 100.0 * std::fabs(e.valore - e.distanza_m) / e.distanza_m;
        } else if (e.tipo == "osservazione_aob") {
            ++r.osservazioni_aob;
            somma_aob += e.valore;
        } else if (e.tipo == "ping_ecoscandaglio") {
            ++r.ping_ecoscandaglio;
        } else if (e.tipo == "punto_nave" && e.giusta) {
            ++r.punti_nave;
            somma_nav += e.valore;
            r.errore_navigazione_max_m = std::max(r.errore_navigazione_max_m, e.valore);
        } else if (e.tipo == "allarme_siluro") {
            ++r.allarmi_siluro;
            allarmi.push_back(e.tempo);
        } else if (e.tipo == "siluro_evitato") {
            r.siluri_evitati_allarme += std::any_of(allarmi.begin(), allarmi.end(), [&](double a) { return e.tempo - a <= ALLARME_PRIMA_S; });
        } else if (e.tipo == "mina_scoperta") {
            ++r.mine_scoperte;
        } else if (e.tipo == "attraversamento_mine") {
            ++r.attraversamenti_mine;
        } else if (e.tipo == "verifica_radio") {
            ++r.verifiche_radio;
            r.non_autentici += !e.giusta;
        } else if (e.tipo == "collisione") {
            ++r.collisioni;
        } else if (e.tipo == "quasi_collisione") {
            ++r.quasi_collisioni;
        } else if (e.tipo == "artiglieria_colpo") {
            r.colpi_artiglieria += spari.nuovo(e.codice);
        }
        if (e.tipo == "lancio_giocatore" && e.livello > 0) {   // il missile del modello: la scatola nel livello
            ++r.missili_lanciati;
            missili.insert(e.arma);
            r.neutrali_in_scatola += e.valore > 0.0;
        }
        if (e.tipo == "colpo" && e.altra == g && e.unita != g && missili.count(e.arma)) ++r.missili_a_segno;
        if (e.tipo == "trasmissione") ++r.trasmissioni;
        else if (e.tipo == "ping_attivo") ++r.ping;
        else if (e.tipo == "lancio_giocatore") {
            ++r.lanci;
            ++r.per_arma[e.classe_arma].lanci;
        } else if (e.tipo == "colpo" && e.altra == g && e.unita != g) {
            ++r.colpi;
            ++r.per_arma[e.classe_arma].colpi;
            BersaglioStat& b = r.per_bersaglio[e.unita];
            b.classe = e.classe;
            ++b.colpi;
            colpiti_dal_giocatore.insert(e.unita);
        } else if (e.tipo == "affondamento" && colpiti_dal_giocatore.count(e.unita)) {
            r.per_bersaglio[e.unita].affondato = true;
            if (e.lato == 1) ++r.affondati_ostili;
        } else if (e.tipo == "avvistato" || e.tipo == "fix_radiogoniometria" || e.tipo == "ping_subito") {
            ++r.scoperte_nemiche;
            if (!r.prima_scoperta) r.prima_scoperta = ScopertaNemica{e.tempo, e.tipo, e.classe, e.distanza_m};
        } else if (e.tipo == "scoperta") {
            scoperti.insert(e.unita);
        } else if (e.tipo == "classificazione") {
            // le regole dell'esito (RegistroMissione::esito): il sonarista (411) a parte, la categoria automatica no
            if (e.codice == CODICE_SONARISTA) {
                ++r.classificazioni_sonarista;
                continue;
            }
            if (e.livello < 2 && e.valore == 0.0) continue;
            if (e.codice == CODICE_VISIVA) e.giusta ? ++r.visive_giuste : ++r.visive_errate;
            e.giusta ? ++r.giuste : ++r.errate;
            if (classificati.insert(e.unita).second && e.tempo_scoperta >= 0.0) {
                somma_classificazione += std::max(0.0, e.tempo - e.tempo_scoperta);
                ++con_tempo;
            }
        }
    }
    scoperti.insert(classificati.begin(), classificati.end());   // un contatto classificato e' anche scoperto
    r.contatti = static_cast<int>(scoperti.size());
    r.classificati = static_cast<int>(classificati.size());
    if (con_tempo > 0) r.tempo_classificazione_s = somma_classificazione / con_tempo;
    if (r.misure_telemetro > 0) r.errore_telemetro_pct = somma_telemetro / r.misure_telemetro;
    if (r.osservazioni_aob > 0) r.errore_aob_deg = somma_aob / r.osservazioni_aob;
    if (r.punti_nave > 0) r.errore_navigazione_medio_m = somma_nav / r.punti_nave;
    secondo_giro(t, g, r);
    return r;
}

void accumula(StatisticheCumulate& c, const ReportMissione& r) {
    ++c.missioni;
    c.durata_s += r.durata_s;
    c.distanza_nm += r.distanza_nm;
    c.superficie_s += r.superficie_s;
    c.periscopica_s += r.periscopica_s;
    c.cavitazione_s += r.cavitazione_s;
    c.quota_per_tempo += r.quota_media_m * r.durata_s;
    c.periscopio_s += r.periscopio_s;
    c.antenna_s += r.antenna_s;
    c.schiera_s += r.schiera_s;
    c.trasmissioni += r.trasmissioni;
    c.ping += r.ping;
    c.lanci += r.lanci;
    c.colpi += r.colpi;
    c.affondati_ostili += r.affondati_ostili;
    for (const auto& [arma, k] : r.per_arma) {
        c.per_arma[arma].lanci += k.lanci;
        c.per_arma[arma].colpi += k.colpi;
    }
    for (const auto& [id, b] : r.per_bersaglio)
        if (b.affondato) ++c.affondati_per_classe[b.classe];
    if (r.prima_scoperta) {
        ++c.missioni_scoperti;
        auto& [quante, somma] = c.scoperte_per_modo[r.prima_scoperta->come];
        ++quante;
        somma += r.prima_scoperta->distanza_m;
        c.scoperta_piu_lontana_m = std::max(c.scoperta_piu_lontana_m, r.prima_scoperta->distanza_m);
        c.scoperta_piu_vicina_m = c.scoperta_piu_vicina_m > 0.0 ? std::min(c.scoperta_piu_vicina_m, r.prima_scoperta->distanza_m)
                                                                : r.prima_scoperta->distanza_m;
    }
    c.classificati += r.classificati;
    c.giuste += r.giuste;
    c.errate += r.errate;
    c.tempo_classificazione_tot_s += r.tempo_classificazione_s * r.classificati;
    // docs/26 §3
    c.visive_giuste += r.visive_giuste;
    c.visive_errate += r.visive_errate;
    c.misure_telemetro += r.misure_telemetro;
    c.errore_telemetro_pct_tot += r.errore_telemetro_pct * r.misure_telemetro;
    c.osservazioni_aob += r.osservazioni_aob;
    c.errore_aob_deg_tot += r.errore_aob_deg * r.osservazioni_aob;
    c.missili_lanciati += r.missili_lanciati;
    c.missili_a_segno += r.missili_a_segno;
    c.neutrali_in_scatola += r.neutrali_in_scatola;
    c.ping_ecoscandaglio += r.ping_ecoscandaglio;
    c.punti_nave += r.punti_nave;
    c.errore_navigazione_tot_m += r.errore_navigazione_medio_m * r.punti_nave;
    c.errore_navigazione_max_m = std::max(c.errore_navigazione_max_m, r.errore_navigazione_max_m);
    c.allarmi_siluro += r.allarmi_siluro;
    c.siluri_evitati_allarme += r.siluri_evitati_allarme;
    c.mine_scoperte += r.mine_scoperte;
    c.attraversamenti_mine += r.attraversamenti_mine;
    c.verifiche_radio += r.verifiche_radio;
    c.non_autentici += r.non_autentici;
    c.collisioni += r.collisioni;
    c.quasi_collisioni += r.quasi_collisioni;
    c.colpi_artiglieria += r.colpi_artiglieria;
    c.sotto_banchisa_s += r.sotto_banchisa_s;
    c.luci_accese_s += r.luci_accese_s;
    c.in_coda_max_s = std::max(c.in_coda_max_s, r.in_coda_s);
    if (r.distanza_min_sottomarino_m > 0.0 && (c.distanza_min_sottomarino_m <= 0.0 || r.distanza_min_sottomarino_m < c.distanza_min_sottomarino_m))
        c.distanza_min_sottomarino_m = r.distanza_min_sottomarino_m;
    c.urti_ghiaccio += r.urti_ghiaccio;
    c.emersioni_ghiaccio += r.emersioni_ghiaccio;
    c.avvisi_chiglia += r.avvisi_chiglia;
    c.visive_notte_fanali += r.visive_notte_fanali;
    c.visive_luci_spente += r.visive_luci_spente;
    c.visive_notte_sagoma += r.visive_notte_sagoma;
    c.errate_luci_spente += r.errate_luci_spente;
    c.classificati_da_sottomarini += r.classificati_da_sottomarini;
    c.mine_ormeggiate += r.mine_ormeggiate;
    c.mine_di_fondo += r.mine_di_fondo;
    c.mine_aggirate += r.mine_aggirate;
    c.sorvoli_elicotteri += r.sorvoli_elicotteri;
    c.sorvoli_aerei += r.sorvoli_aerei;
    c.siluri_aerei_evitati += r.siluri_aerei_evitati;
    c.salvati += r.salvati;
    for (const auto& [k, n] : r.mine_per_sensore) c.mine_per_sensore[k] += n;
    c.classi_giuste.insert(r.classi_giuste.begin(), r.classi_giuste.end());
    c.quota_max_m = std::max(c.quota_max_m, r.quota_max_m);
    c.velocita_max_kn = std::max(c.velocita_max_kn, r.velocita_max_kn);
    c.missione_piu_lunga_s = std::max(c.missione_piu_lunga_s, r.durata_s);
    // la heatmap della missione (celle di 1 km) nelle celle di 2 km del teatro
    Heatmap& h = c.heatmap[r.teatro.empty() ? std::string("ignoto") : r.teatro];
    const int k = static_cast<int>(std::lround(CELLA_CUMULATA_M / CELLA_MISSIONE_M));
    for (const auto& [xy, s] : r.heatmap) {
        const auto giu = [k](int v) { return v >= 0 ? v / k : -((-v + k - 1) / k); };
        h[{giu(xy.first), giu(xy.second)}] += s;
    }
}

// --- file ---

std::filesystem::path percorso_statistiche() { return percorso_albo().parent_path() / "statistiche.toml"; }

namespace {

toml::table scrivi(const StatisticheCumulate& c) {
    toml::table t;
    t.insert("missioni", c.missioni);
    for (const auto& [k, v] : std::initializer_list<std::pair<const char*, double>>{
             {"durata_s", c.durata_s}, {"distanza_nm", c.distanza_nm}, {"superficie_s", c.superficie_s},
             {"periscopica_s", c.periscopica_s}, {"cavitazione_s", c.cavitazione_s}, {"quota_per_tempo", c.quota_per_tempo},
             {"periscopio_s", c.periscopio_s}, {"antenna_s", c.antenna_s}, {"schiera_s", c.schiera_s},
             {"tempo_classificazione_tot_s", c.tempo_classificazione_tot_s}, {"quota_max_m", c.quota_max_m},
             {"velocita_max_kn", c.velocita_max_kn}, {"missione_piu_lunga_s", c.missione_piu_lunga_s},
             {"scoperta_piu_lontana_m", c.scoperta_piu_lontana_m}, {"scoperta_piu_vicina_m", c.scoperta_piu_vicina_m},
             {"errore_telemetro_pct_tot", c.errore_telemetro_pct_tot}, {"errore_aob_deg_tot", c.errore_aob_deg_tot},
             {"errore_navigazione_tot_m", c.errore_navigazione_tot_m}, {"errore_navigazione_max_m", c.errore_navigazione_max_m},
             {"sotto_banchisa_s", c.sotto_banchisa_s}, {"luci_accese_s", c.luci_accese_s}, {"in_coda_max_s", c.in_coda_max_s},
             {"distanza_min_sottomarino_m", c.distanza_min_sottomarino_m}})
        t.insert(k, v);
    for (const auto& [k, v] : std::initializer_list<std::pair<const char*, int>>{
             {"trasmissioni", c.trasmissioni}, {"ping", c.ping}, {"lanci", c.lanci}, {"colpi", c.colpi},
             {"affondati_ostili", c.affondati_ostili}, {"missioni_scoperti", c.missioni_scoperti},
             {"classificati", c.classificati}, {"giuste", c.giuste}, {"errate", c.errate},
             {"visive_giuste", c.visive_giuste}, {"visive_errate", c.visive_errate}, {"misure_telemetro", c.misure_telemetro},
             {"osservazioni_aob", c.osservazioni_aob}, {"missili_lanciati", c.missili_lanciati}, {"missili_a_segno", c.missili_a_segno},
             {"neutrali_in_scatola", c.neutrali_in_scatola}, {"ping_ecoscandaglio", c.ping_ecoscandaglio}, {"punti_nave", c.punti_nave},
             {"allarmi_siluro", c.allarmi_siluro}, {"siluri_evitati_allarme", c.siluri_evitati_allarme},
             {"mine_scoperte", c.mine_scoperte}, {"attraversamenti_mine", c.attraversamenti_mine},
             {"verifiche_radio", c.verifiche_radio}, {"non_autentici", c.non_autentici}, {"collisioni", c.collisioni},
             {"quasi_collisioni", c.quasi_collisioni}, {"colpi_artiglieria", c.colpi_artiglieria},
             {"urti_ghiaccio", c.urti_ghiaccio}, {"emersioni_ghiaccio", c.emersioni_ghiaccio}, {"avvisi_chiglia", c.avvisi_chiglia},
             {"visive_notte_fanali", c.visive_notte_fanali}, {"visive_luci_spente", c.visive_luci_spente},
             {"visive_notte_sagoma", c.visive_notte_sagoma}, {"errate_luci_spente", c.errate_luci_spente},
             {"classificati_da_sottomarini", c.classificati_da_sottomarini}, {"mine_ormeggiate", c.mine_ormeggiate},
             {"mine_di_fondo", c.mine_di_fondo}, {"mine_aggirate", c.mine_aggirate}, {"sorvoli_elicotteri", c.sorvoli_elicotteri},
             {"sorvoli_aerei", c.sorvoli_aerei}, {"siluri_aerei_evitati", c.siluri_aerei_evitati}, {"salvati", c.salvati}})
        t.insert(k, v);
    toml::table per_sensore;
    for (const auto& [k, n] : c.mine_per_sensore) per_sensore.insert(k, n);
    t.insert("mine_per_sensore", per_sensore);
    toml::array classi;
    for (const auto& k : c.classi_giuste) classi.push_back(k);
    t.insert("classi_giuste", classi);
    toml::table armi, affondati, scoperte, heatmap;
    for (const auto& [a, k] : c.per_arma) armi.insert(a.empty() ? "ignota" : a, toml::array{k.lanci, k.colpi});
    for (const auto& [cl, n] : c.affondati_per_classe) affondati.insert(cl.empty() ? "ignota" : cl, n);
    for (const auto& [come, x] : c.scoperte_per_modo) scoperte.insert(come, toml::array{x.first, x.second});
    for (const auto& [teatro, h] : c.heatmap) {
        toml::array celle;
        for (const auto& [xy, s] : h) celle.push_back(toml::array{xy.first, xy.second, s});
        heatmap.insert(teatro, celle);
    }
    t.insert("per_arma", armi);              // [lanci, colpi]
    t.insert("affondati_per_classe", affondati);
    t.insert("scoperte_per_modo", scoperte);  // [quante, somma delle distanze in m]
    t.insert("heatmap", heatmap);             // teatro -> [[x, y, secondi], ...] in celle di 2 km
    return t;
}

StatisticheCumulate leggi(const toml::table& t) {
    StatisticheCumulate c;
    c.missioni = t["missioni"].value_or(0);
    const auto d = [&](const char* k) { return t[k].value_or(0.0); };
    const auto i = [&](const char* k) { return t[k].value_or(0); };
    c.durata_s = d("durata_s"); c.distanza_nm = d("distanza_nm"); c.superficie_s = d("superficie_s");
    c.periscopica_s = d("periscopica_s"); c.cavitazione_s = d("cavitazione_s"); c.quota_per_tempo = d("quota_per_tempo");
    c.periscopio_s = d("periscopio_s"); c.antenna_s = d("antenna_s"); c.schiera_s = d("schiera_s");
    c.tempo_classificazione_tot_s = d("tempo_classificazione_tot_s"); c.quota_max_m = d("quota_max_m");
    c.velocita_max_kn = d("velocita_max_kn"); c.missione_piu_lunga_s = d("missione_piu_lunga_s");
    c.scoperta_piu_lontana_m = d("scoperta_piu_lontana_m"); c.scoperta_piu_vicina_m = d("scoperta_piu_vicina_m");
    c.trasmissioni = i("trasmissioni"); c.ping = i("ping"); c.lanci = i("lanci"); c.colpi = i("colpi");
    c.affondati_ostili = i("affondati_ostili"); c.missioni_scoperti = i("missioni_scoperti");
    c.classificati = i("classificati"); c.giuste = i("giuste"); c.errate = i("errate");
    // docs/26 §3 (i file di prima non le hanno: 0)
    c.visive_giuste = i("visive_giuste"); c.visive_errate = i("visive_errate"); c.misure_telemetro = i("misure_telemetro");
    c.osservazioni_aob = i("osservazioni_aob"); c.missili_lanciati = i("missili_lanciati"); c.missili_a_segno = i("missili_a_segno");
    c.neutrali_in_scatola = i("neutrali_in_scatola"); c.ping_ecoscandaglio = i("ping_ecoscandaglio"); c.punti_nave = i("punti_nave");
    c.allarmi_siluro = i("allarmi_siluro"); c.siluri_evitati_allarme = i("siluri_evitati_allarme");
    c.mine_scoperte = i("mine_scoperte"); c.attraversamenti_mine = i("attraversamenti_mine");
    c.verifiche_radio = i("verifiche_radio"); c.non_autentici = i("non_autentici"); c.collisioni = i("collisioni");
    c.quasi_collisioni = i("quasi_collisioni"); c.colpi_artiglieria = i("colpi_artiglieria");
    c.errore_telemetro_pct_tot = d("errore_telemetro_pct_tot"); c.errore_aob_deg_tot = d("errore_aob_deg_tot");
    c.errore_navigazione_tot_m = d("errore_navigazione_tot_m"); c.errore_navigazione_max_m = d("errore_navigazione_max_m");
    // il secondo giro (31/10/2026; i file di prima: 0)
    c.sotto_banchisa_s = d("sotto_banchisa_s"); c.luci_accese_s = d("luci_accese_s"); c.in_coda_max_s = d("in_coda_max_s");
    c.distanza_min_sottomarino_m = d("distanza_min_sottomarino_m");
    c.urti_ghiaccio = i("urti_ghiaccio"); c.emersioni_ghiaccio = i("emersioni_ghiaccio"); c.avvisi_chiglia = i("avvisi_chiglia");
    c.visive_notte_fanali = i("visive_notte_fanali"); c.visive_luci_spente = i("visive_luci_spente");
    c.visive_notte_sagoma = i("visive_notte_sagoma"); c.errate_luci_spente = i("errate_luci_spente");
    c.classificati_da_sottomarini = i("classificati_da_sottomarini"); c.mine_ormeggiate = i("mine_ormeggiate");
    c.mine_di_fondo = i("mine_di_fondo"); c.mine_aggirate = i("mine_aggirate"); c.sorvoli_elicotteri = i("sorvoli_elicotteri");
    c.sorvoli_aerei = i("sorvoli_aerei"); c.siluri_aerei_evitati = i("siluri_aerei_evitati"); c.salvati = i("salvati");
    if (const auto* a = t["mine_per_sensore"].as_table())
        for (const auto& [k, v] : *a) c.mine_per_sensore[std::string(k.str())] = v.value_or(0);
    if (const auto* a = t["classi_giuste"].as_array())
        for (const auto& v : *a)
            if (const auto x = v.value<std::string>()) c.classi_giuste.insert(*x);
    if (const auto* a = t["per_arma"].as_table())
        for (const auto& [k, v] : *a)
            if (const auto* x = v.as_array(); x && x->size() == 2)
                c.per_arma[std::string(k.str())] = {(*x)[0].value_or(0), (*x)[1].value_or(0)};
    if (const auto* a = t["affondati_per_classe"].as_table())
        for (const auto& [k, v] : *a) c.affondati_per_classe[std::string(k.str())] = v.value_or(0);
    if (const auto* a = t["scoperte_per_modo"].as_table())
        for (const auto& [k, v] : *a)
            if (const auto* x = v.as_array(); x && x->size() == 2)
                c.scoperte_per_modo[std::string(k.str())] = {(*x)[0].value_or(0), (*x)[1].value_or(0.0)};
    if (const auto* a = t["heatmap"].as_table())
        for (const auto& [teatro, v] : *a)
            if (const auto* celle = v.as_array())
                for (const auto& n : *celle)
                    if (const auto* x = n.as_array(); x && x->size() == 3)
                        c.heatmap[std::string(teatro.str())][{(*x)[0].value_or(0), (*x)[1].value_or(0)}] += (*x)[2].value_or(0.0);
    return c;
}

}  // namespace

FileStatistiche leggi_statistiche(const std::filesystem::path& p) {
    FileStatistiche f;
    if (!std::filesystem::exists(p)) return f;
    try {
        const toml::table t = toml::parse_file(p.string());
        if (const auto* g = t["globali"].as_table()) f.globali = leggi(*g);
        if (const auto* c = t["comandanti"].as_table())
            for (const auto& [k, v] : *c)
                if (const auto* x = v.as_table()) f.comandanti[std::string(k.str())] = leggi(*x);
    } catch (const std::exception&) {   // un file rovinato non ferma il gioco
        return {};
    }
    return f;
}

void salva_statistiche(const FileStatistiche& f, const std::filesystem::path& p) {
    std::filesystem::create_directories(p.parent_path());
    toml::table c;
    for (const auto& [id, s] : f.comandanti) c.insert(id, scrivi(s));
    toml::table t;
    t.insert("versione", 1);
    t.insert("globali", scrivi(f.globali));
    t.insert("comandanti", c);
    const auto tmp = p.string() + ".tmp";
    {
        std::ofstream o(tmp);
        if (!o) throw std::runtime_error("impossibile scrivere " + tmp);
        o << "# statistiche cumulate: globali e per comandante (id del fascicolo)\n" << t << "\n";
    }
    std::filesystem::rename(tmp, p);
}

bool registra_statistiche(const ReportMissione& r, const std::string& comandante, bool conta_comandante, bool ripresa,
                          const std::filesystem::path& p) {
    if (ripresa) return false;
    FileStatistiche f = leggi_statistiche(p);
    accumula(f.globali, r);
    if (conta_comandante && !comandante.empty()) accumula(f.comandanti[comandante], r);
    salva_statistiche(f, p);
    return true;
}

}  // namespace m688
