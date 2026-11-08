// Reazioni dell'IA per situazione (fase 4 C delle interazioni fra IA, docs/18_interazioni_ia.md; nota condivisa
// ia-fase4-contratto.md; dossier 24 §8.4). Su tre eventi del registro:
// - alleato attaccato (un colpo, o un lancio contro di lui) e alleato affondato;
// - nemico scoperto: un sottomarino classificato POSSUB o di piu' dal sonarista di un'IA.
// La formazione della parte (entro 20 km dal punto) reagisce dopo il ritardo della voce (30-120 s, KPUG 60-180), per
// ruolo, secondo la situazione fra le due parti:
//
// | evento    | situazione | nave antisom piu' vicina        | altre navi            | sottomarini amici       | aerei |
// |-----------|------------|---------------------------------|-----------------------|-------------------------|-------|
// | scoperto  | crisi      | tiene sotto (attivo continuo)   | -                     | coprono (P 0,5)         | datum |
// | scoperto  | guerra     | sul contatto                    | -                     | (le gambe di TMA, B)    | datum |
// | attaccato | crisi      | sul datum (legittima difesa)    | si allontanano        | si fermano ad ascoltare | datum |
// | attaccato | guerra     | sul datum entro 1-15 min        | via a tutta 10-20 min | si fermano ad ascoltare | datum |
// | affondato | (come attaccato in guerra)                                                                         |
//
// In pace la scoperta e' della parte D (pedinamento). Le armi restano agli script e alle ROE della fase 3: qui le
// manovre. Numeri STIMA DI GIOCO dal 24 §8.4.

#include <algorithm>
#include <cmath>

#include "missioni/armi_ia_missione.hpp"
#include "missioni/asw_missione.hpp"
#include "missioni/forze_missione.hpp"

extern "C" {
#include "lua.h"
}

namespace m688 {

namespace {

constexpr double RAGGIO_REAZIONE_M = 20000.0;   // la formazione che sa dell'evento (la voce, il Link 11)
constexpr double RAGGIO_ASW_M = 30000.0;        // la nave antisom piu' vicina, fin qui
constexpr double DATUM_S = 900.0;               // sul datum o sul contatto per 15 minuti
constexpr double DATUM_GUERRA_MIN_S = 60.0, DATUM_GUERRA_MAX_S = 900.0;   // 24 §8.4: «entro 1-15 min»
constexpr double ALLONTANA_MIN_S = 600.0, ALLONTANA_MAX_S = 1200.0;
constexpr double ASCOLTA_S = 600.0, ASCOLTA_KN = 3.0;
constexpr double P_COPRE = 0.5;
constexpr double PING_OGNI_S = 30.0;            // l'attivo continuo di chi tiene sotto
constexpr double DISTANZA_SOLO_RILEVAMENTO_M = 8000.0;

int lato_di(const UnitaClassica& u) { return u.flag & 0x18; }   // 0 neutrali, 8 usa, 16 urss, 24 la parte del luogo (M7)
// le portaerei e le ausiliarie: non pedinano mai (al piu' si allontanano o chiamano la scorta)
bool non_pedina_mai(const std::string& classe) {
    return classe == "nimitz" || classe == "kitty_hawk" || classe == "kiev" || classe == "kuznetsov" || classe == "sacramento" ||
           classe == "ugra" || classe == "petroliera" || classe == "boris_chilikin" || classe == "iowa";
}
// i sottomarini lanciamissili balistici: in pace non pedinano un contatto (lo evitano e tengono la pattuglia)
bool strategico(const std::string& classe) {
    return classe == "ohio" || classe == "typhoon" || classe == "delta3" || classe == "delta4" || classe == "yankee" || classe == "lafayette";
}
bool aereo_u(const UnitaClassica& u) { return (u.flag & 3) == 3 || u.categoria == "aereo"; }

}  // namespace

void Missione::passo_reazioni() {
    const auto ritardo = [&](int lato) {
        const auto& c = forze_->d.canali;
        const auto it = c.find(lato);
        const auto r = it != c.end() ? it->second.formazione_s : std::pair<double, double>{30.0, 120.0};
        return r.first + (r.second - r.first) * caso_reazioni_.uniforme();
    };
    const auto avvia = [&](int id, const std::string& ruolo, const std::string& reazione, const std::string& evento, int causa, double inizio,
                           double durata, Vec2 verso, double velocita, bool ping, int gravita) {
        auto* u = trova(id);
        if (!u) return;
        if (const auto it = reazioni_in_corso_.find(id); it != reazioni_in_corso_.end() && tempo_ < it->second.fine && it->second.gravita > gravita) return;
        const auto vecchia = reazioni_in_corso_.find(id);
        const double prima = vecchia != reazioni_in_corso_.end() ? vecchia->second.velocita_prima : u->velocita_ordinata;
        reazioni_in_corso_[id] = {inizio, inizio + durata, verso, velocita, prima, ping, inizio, gravita};
        reazioni_ia_.push_back({tempo_, id, causa, ruolo, reazione, evento});
    };
    const auto reagisci = [&](int ev_id, const std::string& evento, int lato, int ostile, Vec2 punto, int causa) {
        const Situazione sit = situazione_fra(lato, ostile);
        if (sit == Situazione::pace) return;   // la pace e' della parte D
        const bool guerra = sit == Situazione::guerra || evento == "affondato";
        const int gravita = evento == "scoperto" ? 1 : evento == "attaccato" ? 2 : 3;
        // la nave antisom piu' vicina: militare, col sonar
        int asw = -1;
        double piu_vicina = RAGGIO_ASW_M;
        for (const auto& [k, v] : unita_) {
            // ne' le portaerei ne' le ausiliarie (la S15: l'Ugra, nave appoggio col sonar, teneva sotto e speronava)
            if (k == causa || v.giocatore || v.distrutta || v.persa || lato_di(v) != lato || v.categoria != "nave" || aereo_u(v) ||
                classe_civile(v.classe) || v.classe == "agi" || non_pedina_mai(v.classe))
                continue;
            if (sensori_acustici(v, TipoSensori::passivi).empty() && sensori_acustici(v, TipoSensori::attivi).empty()) continue;
            if (const double d = distanza(v.pos, punto); d < piu_vicina) { piu_vicina = d; asw = k; }
        }
        for (const auto& [k, v] : unita_) {
            if (v.giocatore || v.distrutta || v.persa || (v.flag & 0x20) || lato_di(v) != lato || k == causa) continue;
            if (!reazioni_fatte_.insert({ev_id, k}).second) continue;
            const double d = distanza(v.pos, punto);
            const double t0 = tempo_ + ritardo(lato);
            if (aereo_u(v)) {   // un datum all'aereo dell'ASW aereo
                if (!controllato_asw(k) || d > 2.0 * RAGGIO_ASW_M) continue;
                Cue c;
                c.tipo = evento == "scoperto" ? Cue::Tipo::contatto_nave : Cue::Tipo::nave_colpita;
                c.pos = punto;
                c.t = tempo_;
                c.raggio_m = 1852.0;
                c.unita = evento == "scoperto" ? causa : -1;
                cue_asw(k, c);
                reazioni_ia_.push_back({tempo_, k, causa, "aereo", "cue", evento});
                continue;
            }
            if (k == asw) {
                const double inizio = guerra && evento != "scoperto" ? tempo_ + DATUM_GUERRA_MIN_S + (DATUM_GUERRA_MAX_S - DATUM_GUERRA_MIN_S) * caso_reazioni_.uniforme() : t0;
                const bool tiene = evento == "scoperto" && !guerra;
                avvia(k, "asw", tiene ? "tiene_sotto" : "datum", evento, causa, inizio, DATUM_S, punto, v.avanti_tutta * (tiene ? 0.6 : 0.9), tiene, gravita);
                continue;
            }
            if (d > RAGGIO_REAZIONE_M) continue;
            if (v.categoria == "nave" && evento != "scoperto") {   // le altre navi: via dal punto
                const double r = radianti(rilevamento(punto, v.pos));
                const double durata = guerra ? ALLONTANA_MIN_S + (ALLONTANA_MAX_S - ALLONTANA_MIN_S) * caso_reazioni_.uniforme() : ALLONTANA_MIN_S;
                avvia(k, "nave", "allontana", evento, causa, t0, durata, {v.pos.x + 20000.0 * std::sin(r), v.pos.y + 20000.0 * std::cos(r)},
                      guerra ? v.avanti_tutta : 0.7 * v.avanti_tutta, false, gravita);
            } else if (v.categoria == "sottomarino") {
                if (evento != "scoperto") {   // fermi ad ascoltare
                    avvia(k, "sottomarino", "ascolta", evento, causa, t0, ASCOLTA_S, v.pos, std::min(ASCOLTA_KN, v.avanti_tutta), false, gravita);
                } else if (!guerra && caso_reazioni_.uniforme() < P_COPRE) {   // in crisi uno su due si porta a coprire
                    avvia(k, "sottomarino", "copre", evento, causa, t0, DATUM_S, punto, std::min(10.0, v.avanti_tutta), false, gravita);
                }
            }
        }
    };
    const auto& ev = registro_.eventi();
    for (; eventi_reazioni_visti_ < ev.size(); ++eventi_reazioni_visti_) {
        const auto& e = ev[eventi_reazioni_visti_];
        if (e.tipo == TipoEvento::colpo || e.tipo == TipoEvento::affondamento) {
            const auto* v = trova(e.unita);
            if (!v || (v->flag & 0x20)) continue;
            const auto* l = e.altra >= 0 ? trova(e.altra) : nullptr;
            const int lv = lato_di(*v);
            const int ll = l ? lato_di(*l) : (lv == 0x08 ? 0x10 : 0x08);
            if (ll == lv) continue;
            reagisci(e.id, e.tipo == TipoEvento::colpo ? "attaccato" : "affondato", lv, ll, e.pos, e.unita);
        } else if (e.tipo == TipoEvento::lancio_ia || e.tipo == TipoEvento::lancio_giocatore) {
            const auto* b = e.altra >= 0 ? trova(e.altra) : nullptr;
            const auto* l = trova(e.unita);
            if (!b || !l || lato_di(*b) == lato_di(*l) || (b->flag & 0x20)) continue;
            // l'attacco lo sa chi sente il siluro: il bersaglio stesso e la sua formazione, sul punto del bersaglio
            if (!b->giocatore && !b->contatti.count(e.arma)) continue;
            reagisci(e.id, "attaccato", lato_di(*b), lato_di(*l), b->pos, b->id);
        } else if (e.tipo == TipoEvento::classificazione_ia && e.livello >= static_cast<int>(Confidenza::possub) && e.altra >= 0) {
            const auto* o = trova(e.unita);
            const auto* b = trova(e.altra);
            if (!o || !b || b->categoria != "sottomarino") continue;
            const int lb = lato_creduto(*o, *b);
            if (lb <= 0 || lb == lato_di(*o)) continue;
            // il punto: la posizione del contatto, o lungo il rilevamento a una distanza tipica
            Vec2 p = b->pos;
            if (const auto c = o->contatti.find(b->id); c != o->contatti.end()) {
                if (c->second.posizione_nota) p = c->second.pos_stimata;
                else {
                    const double r = radianti(c->second.rilevamento);
                    p = {o->pos.x + DISTANZA_SOLO_RILEVAMENTO_M * std::sin(r), o->pos.y + DISTANZA_SOLO_RILEVAMENTO_M * std::cos(r)};
                }
            }
            reagisci(e.id, "scoperto", lato_di(*o), lb, p, b->id);
        }
    }
    // le reazioni in corso: l'attivo di chi tiene sotto; quelle finite rimettono la velocita' dello script
    for (auto it = reazioni_in_corso_.begin(); it != reazioni_in_corso_.end();) {
        auto* u = trova(it->first);
        if (!u || u->distrutta || u->persa) { it = reazioni_in_corso_.erase(it); continue; }
        auto& r = it->second;
        if (tempo_ >= r.fine) {
            if (r.velocita_prima >= 0.0) u->velocita_ordinata = r.velocita_prima;
            it = reazioni_in_corso_.erase(it);
            continue;
        }
        if (r.ping && tempo_ >= r.inizio && tempo_ >= r.prossimo_ping) {
            r.prossimo_ping = tempo_ + PING_OGNI_S;
            evento_acustico(it->first, 300.0 / 9.0);
            impulso_sonar_enhanced(it->first);
        }
        ++it;
    }
}

// --- pace e INCSEA (fase 4 D, 24 §8.5) -------------------------------------------------------------------------------

namespace {
constexpr double PACE_OGNI_S = 1.0;
constexpr double PEDINA_NAVE_M = 3500.0;               // 2-5 km
constexpr double PEDINA_SUB_SCHIERA_M = 6000.0, PEDINA_SUB_M = 1200.0;   // 2-10 km con la schiera, 500-2000 senza
constexpr double CPA_INCSEA_M = 914.4;                 // 1000 yd
constexpr double INCSEA_AEREO_M = 1852.0, INCSEA_AEREO_QUOTA_M = 152.4;   // 1 nm, 500 ft
constexpr double INCSEA_AEREO_MARGINE_M = 30.0;       // l'IA vola un centinaio di piedi sopra il limite (STIMA)
constexpr double TI_TENGO_OGNI_S = 60.0;
constexpr double PING_ALLERTA[] = {0.05, 0.2, 0.4, 0.7};   // il `ping` della paranoia dell'editor (24 §8.1)
constexpr double PING_VICINO_PACE_M = 3704.0, P_ALLONTANA_PING = 0.7, ALLONTANA_PING_S = 600.0;
constexpr double PEDINA_SCRIPT_M = 10000.0, PEDINA_SCRIPT_S = 300.0;
}  // namespace

void Missione::passo_pace() {
    if (tempo_ < prossima_pace_) return;
    prossima_pace_ = tempo_ + PACE_OGNI_S;
    if (!incsea_attivo_) {
        lua_rawgeti(L_, LUA_REGISTRYINDEX, rif_missione_);
        lua_getfield(L_, -1, "incsea");
        incsea_attivo_ = !(lua_isboolean(L_, -1) && !lua_toboolean(L_, -1));
        lua_pop(L_, 2);
    }
    // il pedinamento in pace: chi ha un sottomarino ostile riferito dal suo sonarista lo segue alla sua distanza
    for (auto& [id, o] : unita_) {
        if (o.giocatore || o.distrutta || o.persa || (o.flag & 0x20) || aereo_u(o) || (classe_civile(o.classe) || o.classe == "agi")) continue;
        if (o.categoria != "nave" && o.categoria != "sottomarino") continue;
        const int lo = lato_di(o);
        if (lo == 0) continue;   // i neutrali non pedinano (S08: i Foxtrot ai moli di Valona partivano dietro al giocatore)
        // pedina solo chi ha il ruolo e il sensore (la principale, 30/10/2026: la Nimitz e la Nanuchka pedinavano il
        // Trafalgar nella S01 e nella I08): i sottomarini d'attacco, e le navi di scorta e di caccia col sonar attivo
        // antisom. Mai le portaerei, le ausiliarie, gli SSBN, ne' le navi senza sonar antisom
        if (o.categoria == "sottomarino" ? strategico(o.classe)
                                         : non_pedina_mai(o.classe) || sensori_acustici(o, TipoSensori::attivi).empty())
            continue;
        int scelto = -1;
        double vicino = 1e18;
        for (const auto& [cid, c] : o.contatti) {
            const auto* b = trova(cid);
            if (!b || b->distrutta || b->persa || b->categoria != "sottomarino" || c.qualita < 800.0 || tempo_ - c.ultima_scoperta > 60.0) continue;
            const int lb = lato_creduto(o, *b);
            if (lb <= 0 || lb == lo || situazione_fra(lo, lb) != Situazione::pace) continue;
            if (const double d = distanza(o.pos, b->pos); d < vicino) { vicino = d; scelto = cid; }
        }
        if (scelto < 0) continue;
        const auto& c = o.contatti.at(scelto);
        const auto* b = trova(scelto);
        const double voluta = o.categoria == "nave" ? PEDINA_NAVE_M : (o.flag54 & 1) ? PEDINA_SUB_SCHIERA_M : PEDINA_SUB_M;
        // con la stima: il punto a distanza voluta dal contatto, dalla parte di chi pedina, a velocita' di poco sopra la
        // sua (quella che il contatto mostra fra due stime non la si ha: la vera, come la vedrebbe il plot); col solo
        // rilevamento si va lungo il rilevamento a 10 nodi, finche' il ping o la TMA non danno la posizione
        const double r = radianti(c.rilevamento);
        Vec2 punto{o.pos.x + 2.0 * voluta * std::sin(r), o.pos.y + 2.0 * voluta * std::cos(r)};
        double vel = std::min(10.0, o.avanti_tutta);
        if (c.posizione_nota) {
            // la direzione dal rilevamento misurato (la stima passiva puo' cadere vicino a chi pedina e capovolgerla)
            const Vec2 p = c.pos_stimata;
            punto = {p.x - voluta * std::sin(r), p.y - voluta * std::cos(r)};
            vel = std::clamp(distanza(o.pos, p) > voluta * 1.5 ? b->velocita + 5.0 : b->velocita, 3.0, o.avanti_tutta);
        }
        if (!reazioni_in_corso_.count(id) || reazioni_in_corso_.at(id).gravita <= 0) {
            const bool nuovo = !reazioni_in_corso_.count(id);
            const double prima = nuovo ? o.velocita_ordinata : reazioni_in_corso_.at(id).velocita_prima;
            reazioni_in_corso_[id] = {tempo_, tempo_ + 30.0, punto, vel, prima, false, tempo_, 0};
            if (nuovo) reazioni_ia_.push_back({tempo_, id, scelto, o.categoria == "nave" ? "asw" : "sottomarino", "pedina", "scoperto"});
        }
        // il ping «ti tengo» della nave, con la P dell'allerta, una volta al minuto
        if (o.categoria == "nave") {
            auto& prossimo = prossimo_ti_tengo_[id];
            if (tempo_ >= prossimo) {
                prossimo = tempo_ + TI_TENGO_OGNI_S;
                const int a = std::clamp(allerta_unita(id).value_or(1), 0, 3);
                if (caso_reazioni_.uniforme() < PING_ALLERTA[a] && !sensori_acustici(o, TipoSensori::attivi).empty()) {
                    evento_acustico(id, 300.0 / 9.0);
                    impulso_sonar_enhanced(id);
                }
            }
        }
    }
    // il pedinamento del giocatore fatto dallo script (R_pedina e simili): un'unita' ostile che in pace lo tiene come
    // bersaglio entro PEDINA_SCRIPT_M per PEDINA_SCRIPT_S e' un pedinamento anche per il registro (causa «script»)
    if (const auto* g = giocatore(); g && !g->distrutta && !g->persa)
        for (const auto& [id, o] : unita_) {
            if (o.giocatore || o.distrutta || o.persa || (o.flag & 0x20) || aereo_u(o) || !ostili(o, *g)) continue;
            const bool segue = situazione_fra(lato_di(o), lato_di(*g)) == Situazione::pace && (o.bersaglio == g->id || o.agganciato == g->id) &&
                               o.contatti.count(g->id) && distanza(o.pos, g->pos) <= PEDINA_SCRIPT_M;
            if (!segue) { pedina_script_da_.erase(id); continue; }
            const double da = pedina_script_da_.try_emplace(id, tempo_).first->second;
            if (tempo_ - da < PEDINA_SCRIPT_S || !pedina_script_detta_.insert(id).second) continue;
            const bool gia = std::any_of(reazioni_ia_.begin(), reazioni_ia_.end(),
                                         [&](const ReazioneIa& r) { return r.unita == id && r.causa == g->id && r.reazione == "pedina"; });
            if (!gia) reazioni_ia_.push_back({tempo_, id, g->id, o.categoria == "nave" ? "asw" : "sottomarino", "pedina", "script"});
        }
    // INCSEA: le navi di parti diverse tengono almeno 1000 yd; sotto, chi e' dell'IA accosta via (una violazione per volta)
    if (*incsea_attivo_)
        for (auto& [id, o] : unita_) {
            if (o.giocatore || o.distrutta || o.persa || o.categoria != "nave" || aereo_u(o) || (classe_civile(o.classe) || o.classe == "agi")) continue;
            for (const auto& [k, n] : unita_) {
                if (k == id || n.distrutta || n.persa || n.categoria != "nave" || aereo_u(n) || lato_di(n) == lato_di(o)) continue;
                if (lato_di(o) == 0x18 || lato_di(n) == 0x18) continue;   // INCSEA e' fra le due marine, non per il luogo (M7)
                if (situazione_fra(lato_di(o), lato_di(n)) == Situazione::guerra) continue;
                const double d = distanza(o.pos, n.pos);
                const auto chiave = std::make_pair(std::min(id, k), std::max(id, k));
                if (d >= CPA_INCSEA_M * 1.2) { incsea_vicini_.erase(chiave); continue; }
                if (d < CPA_INCSEA_M && incsea_vicini_.emplace(chiave, tempo_).second) incsea_.push_back({tempo_, id, k, d});
                if (d < CPA_INCSEA_M * 1.2) {
                    const double via = radianti(rilevamento(n.pos, o.pos));
                    manovra_difesa(id, {o.pos.x + 3000.0 * std::sin(via), o.pos.y + 3000.0 * std::cos(via)}, std::max(o.velocita, 8.0), 60.0);
                }
            }
        }
    // INCSEA degli aerei (24 §4.2, STIMA): nessun passaggio sotto i 500 ft entro 1 nm da una nave dell'altra parte, con
    // la quota vera del controllore dell'ASW aereo (gli altri aerei hanno solo la crociera di prima, e restano fuori).
    // Una violazione per passaggio; l'aereo dell'IA sale alla quota minima da 2 nm (l'elicottero sale a 5 m/s), salvo
    // in hover col sonar a immersione: lo dice il registro
    if (*incsea_attivo_ && asw_)
        for (auto& [id, a] : asw_->aerei) {
            const auto* o = trova(id);
            if (!a.attivo || !o || o->distrutta || o->persa) continue;
            bool vicino = false;
            for (const auto& [k, n] : unita_) {
                if (k == id || n.distrutta || n.persa || n.categoria != "nave" || aereo_u(n) || lato_di(n) == lato_di(*o)) continue;
                // l'accordo e' fra le due marine: i neutrali e il traffico ne restano fuori
                if (lato_di(n) == 0 || lato_di(n) == 0x18 || lato_di(*o) == 0x18 || del_traffico(k) ||
                    situazione_fra(lato_di(*o), lato_di(n)) == Situazione::guerra)
                    continue;
                const double d = distanza(o->pos, n.pos);
                const auto chiave = std::make_pair(std::min(id, k), std::max(id, k));
                vicino = vicino || d < INCSEA_AEREO_M * 2.0;
                if (d >= INCSEA_AEREO_M * 1.2 || a.quota_m >= INCSEA_AEREO_QUOTA_M + INCSEA_AEREO_MARGINE_M) { incsea_vicini_.erase(chiave); continue; }
                if (d < INCSEA_AEREO_M && a.quota_m < INCSEA_AEREO_QUOTA_M && incsea_vicini_.emplace(chiave, tempo_).second)
                    incsea_.push_back({tempo_, id, k, d});
            }
            a.quota_minima_m = vicino ? INCSEA_AEREO_QUOTA_M + INCSEA_AEREO_MARGINE_M : 0.0;
        }
}

}  // namespace m688
