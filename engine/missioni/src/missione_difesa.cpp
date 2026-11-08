// Difesa dell'IA dell'Enhanced (fase 4 A delle interazioni fra IA, docs/18_interazioni_ia.md; nota condivisa
// ia-fase4-contratto.md; inventario §7, §13-15; dossier 24 §8.6, 16 §4.3).
//
// - Ogni sottomarino dell'IA evade (nucleo/evasione) e lancia esche su un siluro ostile sentito, non solo su quelli
//   dell'aereo che lo caccia; i ping ostili intercettati li passa intercetta_impulso. Il transitorio di lancio sentito
//   da un sottomarino ostile: con P 0,5 lo snap shot del 24 §8.6, cioe' un contatto sul rilevamento del lanciatore, la
//   manovra antisiluro e, con le armi libere, un siluro di risposta lungo quel rilevamento.
// - Le navi si allontanano a tutta forza per 3-5 minuti da un siluro ostile che sentono.
// - Il sottomarino colpito si ritira da chi ha lanciato per 20 minuti, come le navi.
// - Anticollisione: un sottomarino dell'IA con un'unita' in rotta di collisione (CPA sotto 150 m entro 1000 m e a
//   quota vicina) cambia quota e accosta per 2 minuti.
// Le manovre le applica il movimento (manovra_ia), sopra lo script e sotto i limiti dei danni; finite, torna la
// velocita' che lo script aveva dato. I numeri sono STIMA DI GIOCO.

#include <algorithm>
#include <cmath>
#include <optional>

#include "missioni/armi_ia_missione.hpp"
#include "missioni/asw_missione.hpp"
#include "missioni/forze_missione.hpp"
#include "nucleo/acustica_asw.hpp"

namespace m688 {

namespace {

constexpr double SILURO_SENTITO_M = 3000.0;      // la testa attiva a 25-30 kHz, o il rumore del siluro a 40-50 kn
constexpr double SILURO_VICINO_M = 1500.0;       // dentro l'acquisizione: la manovra lenta, a poppa, sotto lo strato
constexpr double SILURO_NAVE_M = 4000.0;         // una nave che ha il contatto sul siluro
constexpr double EVITA_NAVE_MIN_S = 180.0, EVITA_NAVE_MAX_S = 300.0;
constexpr double TRANSITORIO_LANCIO_M = 20000.0;  // il lancio (aria, porta del tubo) si sente lontano
constexpr double P_OCCHIATA = 0.5;                // 24 §8.6: lo snap shot lungo il rilevamento
constexpr double RITIRATA_S = 1200.0;             // come le navi colpite (danni_missione.cpp)
constexpr double ANTICOLLISIONE_M = 1000.0, CPA_MIN_M = 150.0, QUOTA_VICINA_M = 30.0, ANTICOLLISIONE_S = 120.0;
constexpr double QUOTA_VELA_M = 15.0;   // la vela sopra lo scafo del sottomarino (688: circa 10 m; STIMA con margine)
constexpr double SALTO_QUOTA_M = 40.0;

int lato_di(const UnitaClassica& u) { return u.flag & 0x18; }   // 0 neutrali, 8 usa, 16 urss, 24 la parte del luogo (M7)
bool ostili_lati(int a, int b) { return a != b && a != 0 && b != 0; }
bool e_siluro(const UnitaClassica& w) { return (w.flag & 0x20) && !(w.flag & 0x80) && w.quota > 0.5 && w.categoria != "boa"; }
bool aereo_u(const UnitaClassica& u) { return (u.flag & 3) == 3 || u.categoria == "aereo"; }

}  // namespace

void Missione::manovra_difesa(int id, Vec2 verso, double velocita_kn, double durata_s) {
    const auto it = manovre_difesa_.find(id);
    const auto* u = trova(id);
    // la velocita' dello script: quella d'una manovra gia' in corso, o d'una reazione in corso (che ha gia' cambiato
    // l'ordinata), se no l'ordinata (S08: dopo l'anticollisione restava quella del pedinamento)
    const auto r = reazioni_in_corso_.find(id);
    const double prima = it != manovre_difesa_.end()                                     ? it->second.velocita_prima
                         : r != reazioni_in_corso_.end() && r->second.velocita_prima >= 0.0 ? r->second.velocita_prima
                         : u                                                               ? u->velocita_ordinata
                                                                                           : -1.0;
    manovre_difesa_[id] = {tempo_ + durata_s, verso, velocita_kn, prima};
}

std::optional<Missione::ManovraDifesa> Missione::manovra_ia(const UnitaClassica& u) const {
    if (const auto it = manovre_difesa_.find(u.id); it != manovre_difesa_.end() && tempo_ < it->second.fine) return it->second;
    if (const auto it = reazioni_in_corso_.find(u.id); it != reazioni_in_corso_.end() && tempo_ >= it->second.inizio && tempo_ < it->second.fine) {
        const auto os = ordini_script_.find(u.id);   // l'ordine dello script in questo istante vince
        if (os == ordini_script_.end() || os->second < tempo_) return ManovraDifesa{it->second.fine, it->second.verso, it->second.velocita_kn, it->second.velocita_prima};
    }
    if (armi_ia_)
        for (auto it = armi_ia_->tma.lower_bound({u.id, -1000000}); it != armi_ia_->tma.end() && it->first.first == u.id; ++it)
            if (it->second.attiva) return ManovraDifesa{tempo_ + 1.0, it->second.verso, it->second.velocita_kn, it->second.velocita_prima};
    // la caccia sul quadro radio (passo_radio_sottomarini): per ultima, mentre non sale per l'ascolto; lo script nell'istante vince
    if (forze_)
        if (const auto it = forze_->radio_sottomarini.find(u.id); it != forze_->radio_sottomarini.end() && it->second.caccia >= 0 && !it->second.salita) {
            const auto os = ordini_script_.find(u.id);
            if (os == ordini_script_.end() || os->second < tempo_) return ManovraDifesa{tempo_ + 1.0, it->second.verso, it->second.velocita_caccia, it->second.velocita_prima_caccia};
        }
    return std::nullopt;
}

void Missione::fine_manovra_ia(UnitaClassica& u, const ManovraDifesa& m) {
    if (m.velocita_prima >= 0.0) u.velocita_ordinata = m.velocita_prima;
}

void Missione::passo_difesa() {
    // 1. i siluri ostili sentiti: evasione ed esche dei sottomarini, allontanamento delle navi
    std::set<int> aviolanciati_cacciati;   // (li stimola gia' passo_evasione per i battelli che l'aereo caccia)
    std::map<int, std::set<int>> cacciati_da;
    if (asw_)
        for (const auto& [id, a] : asw_->aerei)
            if (a.attivo)
                for (const int b : a.bersagli) cacciati_da[b].insert(id);
    std::map<int, int> aereo_dell_arma;
    if (asw_)
        for (const auto& l : asw_->lanci)
            if (l.arma >= 0) aereo_dell_arma[l.arma] = l.aereo;
    for (const auto& [wid, w] : unita_) {
        if (w.distrutta || w.persa || !e_siluro(w)) continue;
        const auto* l = trova(w.lanciatore);
        const int lw = l ? lato_di(*l) : (w.flag & 0x18);
        for (auto& [id, v] : unita_) {
            if (v.giocatore || v.distrutta || v.persa || id == w.lanciatore || (v.flag & 0x20) || !ostili_lati(lato_di(v), lw)) continue;
            const double d = distanza(v.pos, w.pos);
            const bool contatto = v.contatti.count(wid) > 0;
            if (v.categoria == "sottomarino" && v.quota > 3.0) {
                if (d > SILURO_SENTITO_M || !(contatto || d <= SILURO_VICINO_M)) continue;
                if (const auto a = aereo_dell_arma.find(wid); a != aereo_dell_arma.end() && cacciati_da[id].count(a->second)) continue;
                const DatiSiluro* ds = tabella_siluri_.siluro(w.classe);
                stimola_evasione(id, d <= SILURO_VICINO_M ? StimoloEvasione::siluro_vicino : StimoloEvasione::siluro, w.pos,
                                 ds ? ds->quota_max_m : 0.0, wid);
            } else if (v.categoria == "nave" && !aereo_u(v)) {
                if (d > SILURO_NAVE_M || !contatto || !siluri_evitati_.insert({id, wid}).second) continue;
                const double r = radianti(rilevamento(w.pos, v.pos));
                const double durata = EVITA_NAVE_MIN_S + (EVITA_NAVE_MAX_S - EVITA_NAVE_MIN_S) * caso_difesa_.uniforme();
                manovra_difesa(id, {v.pos.x + 10000.0 * std::sin(r), v.pos.y + 10000.0 * std::cos(r)}, v.avanti_tutta, durata);
                ritirate_ia_.push_back({tempo_, id, wid, "siluro"});
            }
        }
    }
    // 2. dal registro: i transitori di lancio sentiti dai sottomarini ostili, i sottomarini colpiti che si ritirano
    const auto& ev = registro_.eventi();
    for (; eventi_difesa_visti_ < ev.size(); ++eventi_difesa_visti_) {
        const auto& e = ev[eventi_difesa_visti_];
        if (e.tipo == TipoEvento::lancio_ia || e.tipo == TipoEvento::lancio_giocatore) {
            const auto* l = trova(e.unita);
            if (!l || aereo_u(*l)) continue;   // gli aviolanciati: il tonfo lo da' l'ASW aereo
            // i transitori di questo lancio (R7, missione_transitori.cpp): col catalogo si sentono col modello acustico,
            // coi sensori passivi di chi ascolta, lo strato e il rumore proprio, come il giocatore (la simmetria,
            // disassemblatore 31/10/2026); senza, la regola di prima (20 km, una volta su due)
            std::vector<const Transitorio*> lancio;
            if (catalogo_)
                for (const auto& x : transitori_armi_)
                    if (x.origine == l->id && std::abs(x.t0 - e.tempo) <= 10.0) lancio.push_back(&x);
            for (auto& [id, v] : unita_) {
                if (v.giocatore || v.distrutta || v.persa || v.categoria != "sottomarino" || v.quota <= 3.0 || !ostili_lati(lato_di(v), lato_di(*l))) continue;
                std::optional<AscoltoTransitorio> sentito;
                if (catalogo_) {
                    std::vector<Ricevitore> ric;
                    for (const Sensore* sn : sensori_acustici(v, TipoSensori::passivi)) ric.push_back(ricevitore_acustico(v, sn));
                    for (const Transitorio* x : lancio) {
                        if (ric.empty() || sentito) break;
                        if (const auto s = sorgente_transitoria(*catalogo_, *x, x->t0, 1.0))
                            if (const AscoltoTransitorio h = ascolta_transitorio(*s, ric, ambiente_acustico(v.pos, x->pos)); h.sentito) sentito = h;
                    }
                    if (!sentito) continue;
                } else if (distanza(v.pos, l->pos) > TRANSITORIO_LANCIO_M || caso_difesa_.uniforme() >= P_OCCHIATA) {
                    continue;
                }
                Contatto* c = v.contatti.count(l->id) ? &v.contatti.at(l->id) : nuovo_contatto(v, l->id);
                if (c) {
                    c->qualita = std::max(c->qualita, 400.0);
                    c->lampeggio = 0;
                    c->ultima_scoperta = tempo_;
                    c->rilevamento = sentito ? normalizza_360(sentito->rilevamento + sentito->errore_deg * gauss())
                                             : normalizza_360(rilevamento(v.pos, l->pos) + 2.0 * gauss());
                    if (c->sensore.empty()) c->sensore = "transitorio";
                }
                stimola_evasione(id, StimoloEvasione::siluro, l->pos, 0.0, l->id);
                // e il tiro di risposta lungo il rilevamento, se le armi sono libere su chi ha lanciato (24 §8.6)
                if (armi_ia_ && armi_libere_ia(id, l->id).value_or(false)) tiro_di_risposta(id, l->id);
            }
        } else if (e.tipo == TipoEvento::colpo && e.unita >= 0) {
            auto* v = trova(e.unita);
            if (!v || v->giocatore || v->distrutta || v->persa || v->categoria != "sottomarino") continue;
            const auto* l = e.altra >= 0 ? trova(e.altra) : nullptr;
            const Vec2 da = l ? l->pos : e.pos;
            const double r = radianti(rilevamento(da, v->pos));
            manovra_difesa(v->id, {v->pos.x + 20000.0 * std::sin(r), v->pos.y + 20000.0 * std::cos(r)}, 0.8 * v->avanti_tutta, RITIRATA_S);
            ritirate_ia_.push_back({tempo_, v->id, e.altra, "colpito"});
        }
    }
    // 3. anticollisione dei sottomarini dell'IA (16 §4.3): la geometria che i loro sensori danno da vicino
    for (auto& [id, v] : unita_) {
        if (v.giocatore || v.distrutta || v.persa || v.categoria != "sottomarino" || (v.flag & 0x20)) continue;
        // fermo in superficie (all'ormeggio, S08): non si immerge ne' parte per evitare chi passa
        if (std::fabs(v.velocita) < 0.5 && std::fabs(v.velocita_ordinata) < 0.5 && v.quota < 1.0) continue;
        if (const auto it = anticollisione_.find(id); it != anticollisione_.end() && tempo_ < it->second) continue;
        const double rv = radianti(v.rotta), vv = metri_al_secondo(v.velocita);
        for (const auto& [k, o] : unita_) {
            if (k == id || o.distrutta || o.persa || (o.flag & 0x20) || aereo_u(o) || o.categoria == "boa") continue;
            const double d = distanza(v.pos, o.pos);
            if (d > ANTICOLLISIONE_M || !v.contatti.count(k)) continue;
            if (in_pedinamento_vicino(id, k)) continue;   // la coppia la guarda il pedinatore, col suo ritardo (S24)
            const double quota_o = o.categoria == "nave" ? 10.0 : o.quota;
            if (std::fabs(quota_o - v.quota) > QUOTA_VICINA_M) continue;
            const double ro = radianti(o.rotta), vo = metri_al_secondo(o.velocita);
            const Vec2 dp{o.pos.x - v.pos.x, o.pos.y - v.pos.y};
            const Vec2 dv{vo * std::sin(ro) - vv * std::sin(rv), vo * std::cos(ro) - vv * std::cos(rv)};
            const double dv2 = dv.x * dv.x + dv.y * dv.y;
            if (dv2 < 1e-6) continue;
            const double tc = -(dp.x * dv.x + dp.y * dv.y) / dv2;
            if (tc <= 0.0) continue;   // si allontanano
            const double cx = dp.x + dv.x * tc, cy = dp.y + dv.y * tc;
            if (std::hypot(cx, cy) >= CPA_MIN_M) continue;
            // cambia quota (sotto, se il fondo e la quota massima lo lasciano; se no sopra) e accosta via dall'altro
            const double fondo = profondita(v.pos);
            const double sotto = std::min(v.quota_max, (fondo > 0.0 ? fondo - 30.0 : 1e9));
            v.quota_ordinata = v.quota + SALTO_QUOTA_M <= sotto ? v.quota + SALTO_QUOTA_M : std::max(20.0, v.quota - SALTO_QUOTA_M);
            const double via = radianti(normalizza_360(rilevamento(v.pos, o.pos) + 180.0));
            manovra_difesa(id, {v.pos.x + 3000.0 * std::sin(via), v.pos.y + 3000.0 * std::cos(via)}, std::max(v.velocita, 5.0), ANTICOLLISIONE_S);
            anticollisione_[id] = tempo_ + ANTICOLLISIONE_S;
            ritirate_ia_.push_back({tempo_, id, k, "collisione"});
            break;
        }
    }
    // 3b. le navi dell'IA non investono il sottomarino che tengono vicino alla superficie (S16, 30/10/2026: la fregata
    // in caccia accostava sull'Alfa che emergeva e gli passava sopra): con la sua posizione nota, sopra la chiglia della
    // nave piu' la vela (pescaggio + QUOTA_VELA_M) e un CPA sotto CPA_MIN_M entro ANTICOLLISIONE_M, accosta via. Le armi
    // antisom tirano da lontano (il Terne fino a 1600 m), l'ASROC e i siluri anche: passare sopra non serve
    for (auto& [id, v] : unita_) {
        if (v.giocatore || v.distrutta || v.persa || v.categoria != "nave" || (v.flag & 0x20)) continue;
        if (const auto it = anticollisione_.find(id); it != anticollisione_.end() && tempo_ < it->second) continue;
        const double rv = radianti(v.rotta), vv = metri_al_secondo(v.velocita);
        for (const auto& [k, o] : unita_) {
            if (o.distrutta || o.persa || o.categoria != "sottomarino" || o.quota > pescaggio(v) + QUOTA_VELA_M) continue;
            // l'incagliata, quella in avaria o a rimorchio: ci si va apposta (la S11: il rimorchiatore va a prendere il
            // Foxtrot incagliato)
            if (incagliata_ia(k) || in_avaria(k) || rimorchiatore_di(k) >= 0) continue;
            const auto c = v.contatti.find(k);
            if (c == v.contatti.end() || !c->second.posizione_nota || distanza(v.pos, o.pos) > ANTICOLLISIONE_M) continue;
            const double ro = radianti(o.rotta), vo = metri_al_secondo(o.velocita);
            const Vec2 dp{o.pos.x - v.pos.x, o.pos.y - v.pos.y};
            const Vec2 dv{vo * std::sin(ro) - vv * std::sin(rv), vo * std::cos(ro) - vv * std::cos(rv)};
            const double dv2 = dv.x * dv.x + dv.y * dv.y;
            if (dv2 < 1e-6) continue;
            const double tc = -(dp.x * dv.x + dp.y * dv.y) / dv2;
            if (tc <= 0.0 || std::hypot(dp.x + dv.x * tc, dp.y + dv.y * tc) >= CPA_MIN_M) continue;
            const double via = radianti(normalizza_360(rilevamento(v.pos, o.pos) + 180.0));
            manovra_difesa(id, {v.pos.x + 3000.0 * std::sin(via), v.pos.y + 3000.0 * std::cos(via)}, std::max(v.velocita, 5.0), ANTICOLLISIONE_S);
            anticollisione_[id] = tempo_ + ANTICOLLISIONE_S;
            ritirate_ia_.push_back({tempo_, id, k, "collisione"});
            break;
        }
    }
    // 4. le manovre finite: la velocita' dello script torna (le manovre in corso le applica il movimento, manovra_ia)
    for (auto it = manovre_difesa_.begin(); it != manovre_difesa_.end();) {
        auto* u = trova(it->first);
        if (!u || u->distrutta || u->persa) { it = manovre_difesa_.erase(it); continue; }
        if (tempo_ >= it->second.fine) {
            fine_manovra_ia(*u, it->second);
            it = manovre_difesa_.erase(it);
            continue;
        }
        ++it;
    }
}

}  // namespace m688
