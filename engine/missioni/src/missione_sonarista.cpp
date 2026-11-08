// Il sonarista dell'equipaggio nella missione (S5d; nucleo/sonarista.hpp del disassemblatore): gli ascolti del giocatore
// per sorgente con le righe del fascio, i rapporti al comandante come avvisi e voci del registro.

#include <algorithm>
#include <map>

#include "missioni/missione.hpp"
#include "missioni/danni_sottomarini.hpp"
#include "nucleo/analisi_acustica.hpp"
#include "nucleo/battello.hpp"
#include "nucleo/carriera.hpp"

namespace m688 {

namespace {

constexpr int MSG_SONARISTA_NUOVO = 410, MSG_SONARISTA_CLASSIFICATO = 411, MSG_SONARISTA_PERSO = 412,
              MSG_SONARISTA_RITROVATO = 413;
constexpr double ECO_ESCA_S = 2.0;            // l'eco d'esca del falso contatto vale per il passo del sonarista che segue
constexpr double RIGHE_OGNI_S = 5.0;          // le righe del fascio di una sorgente si rifanno ogni 5 s
constexpr std::size_t RIGHE_CONTATTI_MAX = 8;  // i contatti piu' forti con le righe, per unita' (STIMA: il costo dei fasci)
constexpr double LOFAR_MIN_HZ = 5.0, LOFAR_MAX_HZ = 1000.0;
constexpr double SOGLIA_RIGHE_DB = 0.0;       // tutte le righe sopra il rumore: la soglia vera e' del Sonarista (ProfiloSonarista::soglia_righe_db)

int codice_di(RapportoSonar::Tipo t) {
    switch (t) {
        case RapportoSonar::Tipo::nuovo: return MSG_SONARISTA_NUOVO;
        case RapportoSonar::Tipo::classificato: return MSG_SONARISTA_CLASSIFICATO;
        case RapportoSonar::Tipo::perso: return MSG_SONARISTA_PERSO;
        case RapportoSonar::Tipo::ritrovato: return MSG_SONARISTA_RITROVATO;
    }
    return 0;
}

// La verita' di una classificazione per il registro e il debriefing (la principale, 29/10/2026: in SCENE1B ogni contatto
// «Portarinfuse» risultava giusto). Il Sonarista segna errata solo quando sceglie apposta la seconda ipotesi; qui si
// confronta con la sorgente vera: la classe detta con la sua classe (le chiavi delle firme sono quelle di classi.toml),
// la sola categoria con la sua natura, un biologico giusto solo se detto biologico o non sottomarino senza classe
bool errata_per_verita(const UnitaClassica* u, bool biologico, const std::string& classe, Confidenza conf, bool errata) {
    if (biologico) return classe != "biologico" && (!classe.empty() || conf != Confidenza::nonsub);
    if (!u) return errata;   // un'istanza del paesaggio (F): quella del Sonarista
    if (!classe.empty()) return classe != u->classe;
    const bool sub = u->categoria == "sottomarino";
    return (conf != Confidenza::nonsub) != sub;
}

}  // namespace

// Il falso contatto del vetrino di Dio (D3; nota d3-contatto-falso-contratto.md): la verita' per il sonarista (falso,
// fedelta', la categoria della classe imitata) e l'eco d'esca trovata dal ping di chi ascolta da poco
void Missione::falso_contatto(AscoltoSonar& a, int sorgente, int ascoltatore) const {
    const auto f = esche_mobili_.find(sorgente);
    if (f == esche_mobili_.end() || f->second.dati) return;
    a.falso = true;
    a.fedelta = f->second.fedelta;
    a.sottomarino = f->second.categoria == "sottomarino";
    const auto e = f->second.eco_da.find(ascoltatore);
    a.eco_esca = e != f->second.eco_da.end() && tempo_ - e->second <= ECO_ESCA_S;
}

// Il miglior ascolto del secondo per sorgente (da ascolta_con_acustica, solo i sensori acustici del giocatore)
void Missione::ascolto_per_sonarista(const UnitaClassica& b, const Sorgente& s, const Sensore* sensore, double se, double errore) {
    auto& a = ascolti_sonarista_[b.id];
    if (a.sorgente >= 0 && a.se_db >= se) return;
    a.sorgente = b.id;
    a.rilevamento = rilevamento(giocatore()->pos, b.pos);
    a.se_db = se;
    a.errore_deg = errore;
    a.biologico = s.firma && s.firma->tipo_unita == "biologico";
    sensore_ascolto_[b.id] = sensore->id;
}

// Elica (propulsione) e macchine (motori) dei sottomarini colpiti, per il rumore delle variazioni (R6)
void Missione::danni_acustici(const DanniSottomarini& sub) {
    for (const auto& p : sub.sottomarini()) {
        const auto* s = sub.sistemi(p->unita);
        if (!s) continue;
        const auto eff = [&](Sistema x) { const auto i = static_cast<std::size_t>(x); return i < s->size() ? (*s)[i].efficienza : 1.0; };
        danni_acustici_[p->unita] = {1.0 - eff(Sistema::propulsione), 1.0 - eff(Sistema::motori)};
        urto_acustico_[p->unita] = sub.rumore_urto_db(p->unita);   // eliche e piastrelle dopo un urto (nucleo/acustica, urto_db)
    }
}

void Missione::passo_sonarista() {
    const auto* g = giocatore();
    if (!g || g->distrutta || g->persa) return;
    if (!sonarista_) {
        sonarista_ = std::make_unique<Sonarista>(*esperienza_sonarista_, catalogo_.get());
        caso_sonarista_ = Casuale(seme_ ^ 0x50AA50AAull);
        prossimo_sonarista_ = tempo_;
    }
    if (tempo_ < prossimo_sonarista_) return;
    prossimo_sonarista_ = tempo_ + 1.0;
    // le righe del fascio puntato sulla sorgente, col sensore che l'ha sentita (LOFAR), ogni RIGHE_OGNI_S
    std::vector<AscoltoSonar> ascolti;
    std::map<int, bool> biologici;   // per la verita' delle classificazioni
    for (auto& [id, a] : ascolti_sonarista_) {
        biologici[id] = a.biologico;
        const auto it = unita_.find(id);
        if (it == unita_.end() || it->second.distrutta || it->second.persa) continue;
        auto& [quando, righe] = righe_sonarista_[id];
        auto& proprie = proprie_sonarista_[id];
        auto& demon = demon_sonarista_[id];
        if (tempo_ - quando >= RIGHE_OGNI_S || quando == 0.0) {
            quando = tempo_;
            righe.clear();
            proprie.clear();
            demon.clear();
            // il sensore che l'ha sentita, ma le righe dal LOFAR piu' fine fra quelli in ascolto (la schiera filata), come
            // per il sonarista dell'IA (simmetria, 03/11/2026: il giocatore classificava sulla sfera, l'IA sulla schiera)
            const Sensore* usato = nullptr;
            const auto in_ascolto = sensori_acustici(*g, TipoSensori::ascolto);
            for (const Sensore* sn : in_ascolto) if (sn->id == sensore_ascolto_[id]) usato = sn;
            if (usato)
                for (const Sensore* x : in_ascolto)
                    if (x->lofar_bin_hz > 0.0 && (usato->lofar_bin_hz <= 0.0 || x->lofar_bin_hz < usato->lofar_bin_hz)) usato = x;
            const auto s = sorgente_acustica(it->second);
            // il DEMON del fascio sulla sorgente: per la classificazione a orecchio senza righe utili (passo B)
            if (usato && s)
                demon = leggi_demon_fascio(demon_fascio({*s}, ricevitore_acustico(*g, usato), ambiente_acustico(g->pos, it->second.pos), a.rilevamento));
            if (usato && s && usato->lofar_bin_hz > 0.0)
                for (const auto& r : righe_ricevute({*s}, ricevitore_acustico(*g, usato), ambiente_acustico(g->pos, it->second.pos),
                                                    a.rilevamento, LOFAR_MIN_HZ, LOFAR_MAX_HZ, usato->lofar_bin_hz))
                    if (r.snr_db >= SOGLIA_RIGHE_DB) {
                        righe.push_back({r.hz, r.snr_db});
                        // del proprio battello (-3) e della vibrazione del cavo della schiera filata (-2): le maschera
                        // l'operatore. Senza le -2, a 4 nodi le righe del cavo della TB-16 (43,8 e 87,6 Hz) e della
                        // SQR-19 votavano «nave» per un Victor III vero a 3,7 km (nota del runtime, 03/11/2026)
                        if (r.sorgente == -3 || r.sorgente == -2) proprie.push_back(r.hz);
                    }
        }
        // righe e proprie insieme a ogni secondo: con le sole righe della cache, nei secondi fra un ricalcolo e l'altro, la
        // maschera non toglieva niente e le navi di M1 diventavano «los_angeles»
        a.righe = righe;
        a.proprie = proprie;
        a.demon = demon;
        a.sottomarino = it->second.categoria == "sottomarino";
        falso_contatto(a, id, g->id);
        ascolti.push_back(a);
    }
    ascolti_sonarista_.clear();
    for (const auto& r : sonarista_->passo(tempo_, ascolti, caso_sonarista_)) {
        AvvisoSonarista v;
        v.tempo = r.t;
        v.codice = codice_di(r.tipo);
        v.numero = r.numero;
        v.testo_it = r.testo_it;
        v.testo_en = r.testo_en;
        for (const auto& t : sonarista_->tracce())
            if (t.numero == r.numero) {
                v.sorgente = t.sorgente;
                v.rilevamento = t.rilevamento;
                v.classe = t.classe;
                const auto bi = biologici.find(t.sorgente);
                v.errata = contatto_falso(t.sorgente) ? t.tipo != "falso contatto"
                                                      : errata_per_verita(trova(t.sorgente), bi != biologici.end() && bi->second, t.classe, t.confidenza, t.errata);
            }
        avvisi_sonarista_.push_back(std::move(v));
    }
    // gli scambi d'incrocio: la traccia ora segue un'altra sorgente (il sonarista non lo sa)
    for (const auto& t : sonarista_->tracce()) {
        auto [it, nuova] = sorgente_di_traccia_.try_emplace(t.numero, t.sorgente);
        if (!nuova && it->second != t.sorgente) {
            scambi_tracce_.push_back({tempo_, t.numero, it->second, t.sorgente});
            it->second = t.sorgente;
        }
    }
}

// --- i sonaristi dell'IA (docs/18 fase 2, C) ---

// Il miglior ascolto del secondo per sorgente, per ogni unita' dell'IA (da ascolta_con_acustica)
void Missione::ascolto_ia(const UnitaClassica& o, const UnitaClassica& b, const Sorgente& s, const Sensore* sensore, double se, double errore) {
    ascolto_ia(o, b.id, rilevamento(o.pos, b.pos), s.firma && s.firma->tipo_unita == "biologico", sensore, se, errore);
}

void Missione::ascolto_ia(const UnitaClassica& o, int sorgente, double rilevamento_vero, bool biologico, const Sensore* sensore, double se,
                          double errore) {
    auto it = sonaristi_ia_.find(o.id);
    if (it == sonaristi_ia_.end()) {
        SonaristaIa n;
        n.s = std::make_unique<Sonarista>(esperienza_di_ia(o), catalogo_.get());   // l'avversario, la chiave d'unita' o la sandbox
        n.caso = Casuale(seme_sonarista_ia(seme_, o.id));
        n.prossimo = tempo_;
        it = sonaristi_ia_.emplace(o.id, std::move(n)).first;
    }
    auto& a = it->second.ascolti[sorgente];
    if (a.sorgente >= 0 && a.se_db >= se) return;
    a.sorgente = sorgente;
    a.rilevamento = rilevamento_vero;
    a.se_db = se;
    a.errore_deg = errore;
    a.biologico = biologico;
    it->second.sensore[sorgente] = sensore;
}

// Una volta al secondo per unita': le righe del fascio sulle sorgenti con traccia solida (le piu' forti: il costo), il
// passo del sonarista, le classificazioni nella lista per il registro
void Missione::passo_sonaristi_ia() {
    for (auto& [id, si] : sonaristi_ia_) {
        const auto u = unita_.find(id);
        if (u == unita_.end() || u->second.distrutta || u->second.persa) { si.ascolti.clear(); continue; }
        if (tempo_ < si.prossimo) continue;
        si.prossimo = tempo_ + 1.0;
        const UnitaClassica& o = u->second;
        std::vector<AscoltoSonar> ascolti;
        std::map<int, bool> biologici;
        // le righe per ogni contatto con traccia solida, di qualunque parte (simmetria, punto 1: il sonarista del
        // giocatore le ha per tutti), ma per i soli RIGHE_CONTATTI_MAX piu' forti: il costo dei fasci
        std::set<int> con_righe;
        {
            std::vector<std::pair<double, int>> forti;
            for (const auto& [sid, a] : si.ascolti) {
                const auto b = unita_.find(sid);
                const auto c = o.contatti.find(sid);
                if (b != unita_.end() && !b->second.distrutta && !b->second.persa && c != o.contatti.end() && c->second.qualita >= 800.0)
                    forti.emplace_back(-c->second.eccesso_db, sid);
            }
            std::sort(forti.begin(), forti.end());
            for (std::size_t i = 0; i < forti.size() && i < RIGHE_CONTATTI_MAX; ++i) con_righe.insert(forti[i].second);
        }
        for (auto& [sid, a] : si.ascolti) {
            biologici[sid] = a.biologico;
            // un transitorio dello scafo di quella sorgente sentito nell'ultimo secondo (missione_scafo.cpp): il sonarista
            // ne fa un PROBSUB, o un POSSUB sul contatto nuovo (nucleo/sonarista, disassemblatore)
            for (auto x = scafo_sentiti_.rbegin(); x != scafo_sentiti_.rend() && tempo_ - x->t <= 1.5; ++x)
                if (x->ascoltatore == id && x->origine == sid) a.transitorio_scafo = true;
            const auto b = unita_.find(sid);
            if (b == unita_.end()) {   // un'istanza del paesaggio (F): senza righe, il sonarista ne da' la confidenza
                if (istanza_paesaggio(sid)) ascolti.push_back(a);
                continue;
            }
            if (b->second.distrutta || b->second.persa) continue;
            const auto c = o.contatti.find(sid);
            if (con_righe.count(sid) && c != o.contatti.end()) {
                auto& [quando, righe] = si.righe[sid];
                auto& proprie = si.proprie[sid];
                auto& demon = si.demon[sid];
                if (tempo_ - quando >= RIGHE_OGNI_S || quando == 0.0) {
                    quando = tempo_;
                    righe.clear();
                    proprie.clear();
                    demon.clear();
                    // le righe dal LOFAR piu' fine fra i sensori in ascolto (la schiera filata): l'operatore classifica sul
                    // LOFAR, non sul sonar di prua che sente di piu' a banda larga ma non vede le righe basse (disassemblatore,
                    // 30/10/2026: la Perry a 2,5 km da un Alfa, 30 dB sull'SQS-53, nessuna riga)
                    const Sensore* usato = si.sensore[sid];
                    for (const Sensore* x : sensori_acustici(o, TipoSensori::ascolto))
                        if (x->lofar_bin_hz > 0.0 && (!usato || usato->lofar_bin_hz <= 0.0 || x->lofar_bin_hz < usato->lofar_bin_hz)) usato = x;
                    const auto s = sorgente_acustica(b->second);
                    if (usato && s)   // il DEMON del fascio (passo B)
                        demon = leggi_demon_fascio(demon_fascio({*s}, ricevitore_acustico(o, usato), ambiente_acustico(o.pos, b->second.pos), a.rilevamento));
                    if (usato && s && usato->lofar_bin_hz > 0.0)
                        for (const auto& r : righe_ricevute({*s}, ricevitore_acustico(o, usato), ambiente_acustico(o.pos, b->second.pos),
                                                            a.rilevamento, LOFAR_MIN_HZ, LOFAR_MAX_HZ, usato->lofar_bin_hz))
                            if (r.snr_db >= SOGLIA_RIGHE_DB) {
                                righe.push_back({r.hz, r.snr_db});
                                if (r.sorgente == -3 || r.sorgente == -2) proprie.push_back(r.hz);   // le proprie e il cavo, come per il giocatore
                            }
                }
                a.righe = righe;
                a.proprie = proprie;   // a ogni secondo con le righe della cache (disassemblatore, 29/10/2026)
                a.demon = demon;
                a.sottomarino = b->second.categoria == "sottomarino";
            }
            falso_contatto(a, sid, id);
            ascolti.push_back(a);
        }
        si.ascolti.clear();
        for (const auto& r : si.s->passo(tempo_, ascolti, si.caso)) {
            if (r.tipo != RapportoSonar::Tipo::classificato) continue;
            for (const auto& t : si.s->tracce())
                if (t.numero == r.numero) {
                    const auto bi = biologici.find(t.sorgente);
                    const bool errata = contatto_falso(t.sorgente) ? t.tipo != "falso contatto"
                                                                    : errata_per_verita(trova(t.sorgente), bi != biologici.end() && bi->second, t.classe, t.confidenza, t.errata);
                    classificazioni_ia_.push_back({tempo_, id, t.sorgente, t.classe, t.confidenza, errata});
                }
        }
    }
}

}  // namespace m688
