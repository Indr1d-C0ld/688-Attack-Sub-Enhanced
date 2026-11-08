// Armi dell'IA dell'Enhanced (fase 4 B delle interazioni fra IA, docs/18_interazioni_ia.md; nota condivisa
// ia-fase4-contratto.md; inventario §5-6). I moduli sono del nucleo (nucleo/armi_ia.hpp e nucleo/manovre_tma.hpp, del
// disassembler); qui il collegamento:
// - il carico dell'anno per ogni sottomarino dell'IA con i dati, i tubi che si vuotano al lancio e si ricaricano dalla
//   riserva (RICARICA_S), la scelta del siluro sul bersaglio;
// - le gambe di TMA su un contatto ostile col solo rilevamento, con le armi libere o l'ingaggio dello script, con
//   l'ordine del motore; finiscono con la soluzione o la rinuncia. Mai nelle missioni originali (tengono i loro script)
//   e mai nell'istante in cui lo script ha dato un ordine al battello;
// - la guida a filo dei siluri filoguidati dell'IA verso il contatto del lanciatore, coi vincoli del filo sulla sua
//   manovra.

#include <algorithm>
#include <cmath>

#include "missioni/armi_ia_missione.hpp"
#include "missioni/danni_sottomarini.hpp"
#include "missioni/forze_missione.hpp"
#include "nucleo/carriera.hpp"

extern "C" {
#include "lua.h"
}

namespace m688 {

namespace {

constexpr double RICARICA_S = 300.0;            // un tubo ricaricato dalla rastrelliera (STIMA: 4-6 minuti a mano o col
                                                // sistema idraulico)
constexpr double PORTATA_UTILE = 0.9;            // la parte della portata a cui l'IA lancia ancora (STIMA)
// la posizione troppo vecchia per una salva (STIMA): il bersaglio non deve essersi spostato oltre l'acquisizione passiva
// tipica di una testa (2-2,5 km, siluri.toml), alla velocita' della soluzione o, senza, a una da convoglio
constexpr double SPOSTAMENTO_MAX_M = 2000.0, VELOCITA_IGNOTA_KN = 15.0, VELOCITA_MINIMA_KN = 3.0;
constexpr double DISTANZA_IGNOTA_M = 8000.0;    // col solo rilevamento: la distanza tipica a cui si sceglie l'arma
constexpr double TMA_OGNI_S = 1.0;
constexpr double ORIZZONTE_GAMBA_M = 20000.0;   // la meta lungo la rotta della gamba

int lato_di(const UnitaClassica& u) { return u.flag & 0x18; }   // 0 neutrali, 8 usa, 16 urss, 24 la parte del luogo (M7)

}  // namespace

DottrinaTma Missione::dottrina_tma_ia(int id) const {
    const auto u = unita_.find(id);
    if (!armi_ia_ || u == unita_.end()) return {};
    return dottrina_tma(armi_ia_->ia.tma, lato_di(u->second) == 0x10 ? "urss" : "usa", u->second.classe);
}

DottrinaFilo Missione::dottrina_filo_ia(Esperienza e) const {
    return armi_ia_ ? dottrina_filo(armi_ia_->ia.armi, e) : DottrinaFilo{};
}

void Missione::armi_ia_enhanced(const std::string& cartella_dati) {
    if (armi_ia_ || armi_ia_spente_) return;
    auto s = std::make_unique<StatoArmiIa>();
    try {
        s->cat = carica_carico(cartella_dati + "/carico.toml");
        s->ia = carica_interazioni(cartella_dati + "/interazioni.toml");
    } catch (const std::exception&) {
        armi_ia_spente_ = true;   // senza i dati: niente armi dell'IA, come prima
        return;
    }
    // le missioni originali non hanno il blocco dell'editor: i loro script tengono i loro tempi
    lua_rawgeti(L_, LUA_REGISTRYINDEX, rif_missione_);
    lua_getfield(L_, -1, "editor");
    s->originale = !lua_istable(L_, -1);
    lua_pop(L_, 2);
    armi_ia_ = std::move(s);
}

const std::vector<Missione::EventoTmaIa>& Missione::eventi_tma_ia() const {
    static const std::vector<EventoTmaIa> vuoto;
    return armi_ia_ ? armi_ia_->eventi_tma : vuoto;
}

const CaricoIa* Missione::carico_ia_di(int id) const {
    if (!armi_ia_) return nullptr;
    const auto it = armi_ia_->battelli.find(id);
    return it != armi_ia_->battelli.end() ? &it->second.carico : nullptr;
}

std::optional<double> Missione::esplosivo_ia(int id) const {
    const CaricoIa* c = carico_ia_di(id);
    const auto u = unita_.find(id);
    if (!c || u == unita_.end()) return std::nullopt;
    return esplosivo_a_bordo(*c, armi_ia_->cat, u->second.classe, catalogo_missili_.get(), anno_);
}

std::optional<int> Missione::armi_rimaste(int id, const std::string& famiglia) const {
    const CaricoIa* c = carico_ia_di(id);
    if (!c) return std::nullopt;
    const auto conta = [&](const std::string& v) {
        if (v.empty()) return false;
        const auto it = armi_ia_->cat.armi.find(v);
        if (it == armi_ia_->cat.armi.end() || it->second.tipo != TipoArmaCarico::siluro) return false;
        return famiglia.empty() || it->second.famiglia == famiglia;
    };
    int n = 0;
    for (const auto& v : c->tubi) n += conta(v);
    for (const auto& [v, q] : c->riserva) if (conta(v)) n += q;
    return n;
}

std::optional<int> Missione::tubi_carichi(int id) const {
    const CaricoIa* c = carico_ia_di(id);
    if (!c) return std::nullopt;
    return static_cast<int>(std::count_if(c->tubi.begin(), c->tubi.end(), [](const std::string& v) { return !v.empty(); }));
}

std::optional<Missione::TmaCorrente> Missione::tma_ia(int id) const {
    const auto soluzione = [&](int b) -> std::optional<StimaTma> {
        auto s = stima_tma_ia(id, b);
        return s && s->osservabile ? s : std::nullopt;
    };
    if (armi_ia_)
        if (const auto it = armi_ia_->tma.lower_bound({id, -1000000}); it != armi_ia_->tma.end() && it->first.first == id)
            return TmaCorrente{it->first.second, it->second.gambe, soluzione(it->first.second)};
    const auto u = unita_.find(id);
    if (u == unita_.end()) return std::nullopt;
    const int b = u->second.agganciato >= 0 ? u->second.agganciato : u->second.bersaglio;
    if (b < 0 || b == id) return std::nullopt;
    if (auto s = soluzione(b)) return TmaCorrente{b, 0, std::move(s)};
    return std::nullopt;
}

std::optional<StimaTma> Missione::stima_tma_ia(int id, int bersaglio) const {
    const auto t = tma_ia_.find({id, bersaglio});
    return t != tma_ia_.end() ? t->second.stima : std::nullopt;
}

Esperienza Missione::esperienza_di_ia(const UnitaClassica& o) const {
    // la sandbox: il livello della parte vince sull'avversario e sulla chiave dell'unita'
    if (const auto r = regolazioni_.find(lato_di(o)); r != regolazioni_.end() && r->second.livello) return *r->second.livello;
    const auto* g = giocatore();
    const int avversario = g && ostili(o, *g) && blocco_carriera_ ? std::max(0, avversario_da_nome(blocco_carriera_->avversario)) : 1;
    const auto k = sonaristi_iniziali_.find(o.id);
    return esperienza_ia(avversario, k != sonaristi_iniziali_.end() ? k->second : std::string_view{});
}

std::optional<Vec2> punto_d_intercetto(Vec2 da, Vec2 punto, double rotta_deg, double v_m_s, double siluro_m_s);   // missione.cpp

bool Missione::arma_ia_al_lancio(const UnitaClassica& l, const UnitaClassica& u, const DatiSiluro*& d) {
    auto& S = *armi_ia_;
    const auto it = S.battelli.find(l.id);
    if (it == S.battelli.end()) return true;   // senza carico (classe senza dati): come prima
    auto& c = it->second.carico;
    // solo i siluri: il lancio dello script e' un'arma subacquea
    CaricoIa siluri = c;
    for (auto& v : siluri.tubi) {
        const auto a = S.cat.armi.find(v);
        if (a == S.cat.armi.end() || a->second.tipo != TipoArmaCarico::siluro) v.clear();
    }
    // il vetrino di Dio: i primi n tubi bloccati (-1: tutti); il comandante prova e non puo'
    if (!interventi_.empty())
        if (const auto n = parametro_attivo(interventi_, stati_interventi_, l.id, "tubi", "tubi"))
            for (std::size_t i = 0; i < siluri.tubi.size(); ++i)
                if (*n < 0.0 || static_cast<double>(i) < *n) siluri.tubi[i].clear();
    const auto* b = trova(u.inseguito >= 0 ? u.inseguito : u.bersaglio);
    double dist = DISTANZA_IGNOTA_M;
    if (b)
        if (const auto k = l.contatti.find(b->id); k != l.contatti.end() && k->second.posizione_nota) dist = distanza(l.pos, k->second.pos_stimata);
    const BersaglioIa tipo = b && b->categoria == "sottomarino" ? BersaglioIa::sottomarino : BersaglioIa::nave;
    auto sc = scegli_arma_ia(siluri, S.cat, tipo, dist, true);
    if (!sc) {   // nessuno con stelle contro questo bersaglio: il primo siluro in tubo
        for (std::size_t i = 0; i < siluri.tubi.size() && !sc; ++i)
            if (!siluri.tubi[i].empty()) {
                SceltaArmaIa x;
                x.variante = siluri.tubi[i];
                x.siluro = S.cat.armi.at(siluri.tubi[i]).siluro;
                x.tubo = static_cast<int>(i);
                sc = x;
            }
    }
    if (!sc || sc->tubo < 0) return false;
    const DatiSiluro* m = sc->siluro.empty() ? nullptr : tabella_siluri_.siluro(sc->siluro);
    // oltre la portata il siluro resta in tubo (lo script classico lancia su ogni traccia solida, a qualunque distanza):
    // la corsa fino al punto d'incontro della TMA, se c'e', se no fino alla posizione stimata
    if (m && b && dist != DISTANZA_IGNOTA_M) {
        // niente salva su una posizione vecchia: il bersaglio puo' essersi spostato oltre l'acquisizione della testa.
        // L'eta' e' quella della posizione (eco, vista, radar, ultima soluzione osservabile della TMA), che l'ascolto
        // passivo non rinnova; la velocita' quella della soluzione, se no una tipica
        const Contatto& k = l.contatti.at(b->id);
        double v = VELOCITA_IGNOTA_KN * NODO;
        if (const auto t = tma_ia_.find({l.id, b->id}); t != tma_ia_.end())
            if (t->second.stima && t->second.stima->osservabile) v = t->second.stima->soluzione.velocita_m_s;
        if ((tempo_ - k.t_posizione) * std::max(v, VELOCITA_MINIMA_KN * NODO) > SPOSTAMENTO_MAX_M) return false;
        double corsa = dist;
        if (const auto st = stima_tma_ia(l.id, b->id); st && st->osservabile) {
            if (const auto x = punto_d_intercetto(l.pos, l.contatti.at(b->id).pos_stimata, st->soluzione.rotta, st->soluzione.velocita_m_s, m->velocita_alta_kn * NODO))
                corsa = distanza(l.pos, *x);
            else
                corsa = 1e18;   // non lo raggiunge
        }
        if (corsa > PORTATA_UTILE * m->portata_alta_m) return false;
    }
    if (m) d = m;
    it->second.ultima[sc->tubo] = c.tubi[static_cast<std::size_t>(sc->tubo)];
    c.tubi[static_cast<std::size_t>(sc->tubo)].clear();
    it->second.ricarica[sc->tubo] = tempo_ + RICARICA_S;
    return true;
}

void Missione::guida_filo_ia(int siluro, const DatiSiluro& d) {
    (void)d;
    const auto* u = trova(siluro);
    const auto* l = u ? trova(u->lanciatore) : nullptr;
    if (!l) return;
    armi_ia_->filo.emplace(siluro, GuidaFiloIa(dottrina_filo(armi_ia_->ia.armi, esperienza_di_ia(*l))));
}

bool Missione::attacca_unita(int id, int bersaglio, bool senza_soluzione) {
    const auto* u = trova(id);
    if (!armi_ia_ || armi_ia_->originale || !u || u->giocatore || u->categoria != "sottomarino" || !trova(bersaglio) || bersaglio == id) return false;
    if (armi_ia_->considerati.count(id) && !armi_ia_->battelli.count(id)) return false;   // senza carico
    armi_ia_->forzati[id] = {bersaglio, senza_soluzione};
    return true;
}

bool Missione::tiro_di_risposta(int sid, int bersaglio, bool rotta_vera) {
    auto* v = trova(sid);
    if (!armi_ia_ || !v || !siluri_attivi_ || !armi_ia_->battelli.count(sid)) return false;
    // l'arma dai tubi: la scelta di arma_ia_al_lancio sul bersaglio, che vuota il tubo
    UnitaClassica w;
    w.id = prossimo_id_++;
    w.categoria = "arma";
    w.flag = 49;   // arma subacquea dell'IA, come il gestore di lancio della libreria
    w.pos = v->pos;
    w.quota = w.quota_ordinata = v->quota;
    w.lanciatore = w.collegata = sid;
    w.bersaglio = w.inseguito = bersaglio;
    w.navigazione = w.insegui = true;
    w.integrita = w.integrita_max = 20;
    w.rumore = 18;
    w.avanti_tutta = 45.7143;
    w.quota_max = 548.64;
    const DatiSiluro* d = nullptr;
    if (!arma_ia_al_lancio(*v, w, d) || !d) {
        --prossimo_id_;
        return false;
    }
    w.classe = d->classe;
    if (const auto k = v->contatti.find(bersaglio); k != v->contatti.end()) w.rotta = w.rotta_ordinata = k->second.rilevamento;
    else if (const auto* b = trova(bersaglio); b && rotta_vera) w.rotta = w.rotta_ordinata = rilevamento(v->pos, b->pos);
    ++create_;
    unita_[w.id] = w;
    colloca(unita_[w.id]);
    nuovo_siluro(unita_[w.id], d);
    return true;
}

void Missione::passo_armi_ia() {
    auto& S = *armi_ia_;
    // i sottomarini dell'IA nuovi: il loro carico dell'anno
    for (const auto& [id, u] : unita_) {
        if (u.giocatore || u.categoria != "sottomarino" || (u.flag & 0x20) || !S.considerati.insert(id).second) continue;
        CaricoIa c = carico_ia(S.cat, S.ia.armi, u.classe, anno_);
        if (c.tubi.empty()) continue;
        S.battelli[id].carico = std::move(c);
    }
    // i tubi che si ricaricano: la stessa variante se c'e' ancora, se no la piu' numerosa in riserva
    for (auto& [id, b] : S.battelli)
        for (auto it = b.ricarica.begin(); it != b.ricarica.end();) {
            if (tempo_ < it->second) { ++it; continue; }
            auto& r = b.carico.riserva;
            std::string v = b.ultima[it->first];
            if (r[v] <= 0) {
                v.clear();
                int piu = 0;
                for (const auto& [k, q] : r)
                    if (q > piu && S.cat.armi.count(k) && S.cat.armi.at(k).tipo == TipoArmaCarico::siluro) { piu = q; v = k; }
            }
            if (!v.empty()) {
                --r[v];
                b.carico.tubi[static_cast<std::size_t>(it->first)] = v;
            }
            it = b.ricarica.erase(it);
        }
    // gli attacchi forzati (m688.attacca_unita): col bersaglio noto in posizione, o subito senza soluzione; un tubo
    // vuoto aspetta la ricarica
    for (auto it = S.forzati.begin(); it != S.forzati.end();) {
        const auto* o = trova(it->first);
        const auto* b = trova(it->second.bersaglio);
        if (!o || o->distrutta || o->persa || !b || b->distrutta || b->persa) { it = S.forzati.erase(it); continue; }
        const auto k = o->contatti.find(b->id);
        const bool pronto = it->second.senza_soluzione || (k != o->contatti.end() && k->second.posizione_nota);
        if (pronto && tiro_di_risposta(it->first, b->id, true)) { it = S.forzati.erase(it); continue; }
        ++it;
    }
    // la guida a filo: gli eventi del passo per siluro, il contatto del lanciatore, i comandi
    std::map<int, std::vector<EventoSiluro>> eventi;
    for (; S.eventi_siluri_visti < eventi_siluri_.size(); ++S.eventi_siluri_visti)
        eventi[eventi_siluri_[S.eventi_siluri_visti].siluro].push_back(eventi_siluri_[S.eventi_siluri_visti].evento);
    S.vincoli.clear();
    for (auto it = S.filo.begin(); it != S.filo.end();) {
        const Siluro* s = siluro(it->first);
        const auto* w = trova(it->first);
        const auto* l = w ? trova(w->lanciatore) : nullptr;
        if (!s || !w || w->distrutta || w->persa || !l || l->distrutta || it->second.tagliato()) { it = S.filo.erase(it); continue; }
        SituazioneFilo x;
        x.t = tempo_;
        x.siluro = s;
        x.eventi = eventi[it->first];
        x.pos_lanciatore = l->pos;
        const int bersaglio = w->inseguito >= 0 ? w->inseguito : w->bersaglio;
        if (const auto k = l->contatti.find(bersaglio); k != l->contatti.end()) {
            if (k->second.posizione_nota) x.bersaglio_stimato = k->second.pos_stimata;
            else x.rilevamento = k->second.rilevamento;
        }
        x.lanciatore_evade = guida_evasione(*l).has_value();
        for (const auto& c : it->second.passo(x)) comanda_siluro(it->first, c);
        // i vincoli del filo sulla manovra del lanciatore (velocita' e virata), che il moto applica finche' guida
        if (const VincoliFilo v = it->second.vincoli(); v.attivi) {
            const auto [vi, nuovo] = S.vincoli.emplace(l->id, v);
            if (!nuovo) {
                vi->second.velocita_max_kn = std::min(vi->second.velocita_max_kn, v.velocita_max_kn);
                vi->second.virata_max_deg_s = std::min(vi->second.virata_max_deg_s, v.virata_max_deg_s);
            }
        }
        ++it;
    }
    // le gambe di TMA, una volta al secondo (non nelle originali)
    if (S.originale || tempo_ < S.prossima_tma) return;
    S.prossima_tma = tempo_ + TMA_OGNI_S;
    for (auto& [id, o] : unita_) {
        if (o.giocatore || o.distrutta || o.persa || o.categoria != "sottomarino" || (o.flag & 0x20) || o.quota <= 3.0) continue;
        // il bersaglio: quello dello script (ingaggio, poi bersaglio corrente), se no il contatto ostile piu' forte col solo
        // rilevamento su cui le armi sono libere
        int bid = o.agganciato >= 0 ? o.agganciato : o.bersaglio;
        const auto fz = S.forzati.find(id);
        const bool forzato = fz != S.forzati.end() && !fz->second.senza_soluzione;   // la TMA sul bersaglio forzato
        if (forzato) bid = fz->second.bersaglio;
        if (bid < 0 || bid == id) {
            double meglio = 0.0;
            for (const auto& [cid, c] : o.contatti) {
                const auto* x = trova(cid);
                if (!x || x->distrutta || x->persa || (x->flag & 0x20) || c.qualita < 800.0 || c.posizione_nota || !ostili(o, *x)) continue;
                if (c.eccesso_db > meglio || bid < 0) {
                    if (!armi_libere_ia(id, cid).value_or(false)) continue;
                    meglio = c.eccesso_db;
                    bid = cid;
                }
            }
        }
        const auto* b = bid >= 0 && bid != id ? trova(bid) : nullptr;
        const auto k = b ? o.contatti.find(bid) : o.contatti.end();
        const auto chiave = std::make_pair(id, bid);
        auto tm = S.tma.find(chiave);
        // via le TMA di questo battello su altri bersagli
        for (auto x = S.tma.lower_bound({id, -1000000}); x != S.tma.end() && x->first.first == id;)
            x = x->first.second != bid ? S.tma.erase(x) : std::next(x);
        tm = S.tma.find(chiave);
        const bool serve = b && !b->distrutta && !b->persa && !(b->flag & 0x20) && k != o.contatti.end() && k->second.qualita >= 800.0 &&
                           !k->second.posizione_nota &&
                           (forzato || (ostili(o, *b) && (o.agganciato == bid || armi_libere_ia(id, bid).value_or(false))));
        if (!serve) {
            if (tm != S.tma.end()) {
                S.eventi_tma.push_back({tempo_, id, bid, tm->second.gambe, k != o.contatti.end() && k->second.posizione_nota ? EventoTmaIa::Tipo::soluzione : EventoTmaIa::Tipo::rinuncia,
                                        stima_tma_ia(id, bid)});
                fine_manovra_ia(o, ManovraDifesa{tempo_, tm->second.verso, tm->second.velocita_kn, tm->second.velocita_prima});
                S.tma.erase(tm);
            }
            continue;
        }
        if (tm == S.tma.end()) {
            const std::string nazione = lato_di(o) == 0x10 ? "urss" : "usa";
            const std::uint64_t seme = seme_ ^ ((static_cast<std::uint64_t>(id) * 131 + static_cast<std::uint64_t>(bid)) * 0x9E3779B97F4A7C15ULL);
            tm = S.tma.emplace(chiave, StatoArmiIa::Tma{ManovreTma(dottrina_tma(S.ia.tma, nazione, o.classe), seme), {}, 0.0, 0, false, o.velocita_ordinata}).first;
        }
        SituazioneTma x;
        x.t = tempo_;
        x.pos = o.pos;
        x.rotta = o.rotta;
        x.velocita_kn = o.velocita;
        x.rilevamento = k->second.rilevamento;
        if (const auto t = tma_ia_.find({id, bid}); t != tma_ia_.end()) x.stima = t->second.stima;
        x.schiera = (o.flag54 & 1) != 0;
        x.quota_m = o.quota;
        if (fondale_) {
            x.fondale_m = profondita(o.pos);
            x.fondale = [this](Vec2 p) { return profondita(p); };
        }
        x.t_ultimo_rilevamento = k->second.ultima_scoperta;
        x.specchio_irrisolto = k->second.rilevamento_specchio.has_value();
        const DecisioneTma dz = tm->second.m.passo(x);
        if (dz.rinuncia != DecisioneTma::Rinuncia::no || dz.soluzione_pronta) {
            S.eventi_tma.push_back({tempo_, id, bid, dz.gambe, dz.soluzione_pronta ? EventoTmaIa::Tipo::soluzione : EventoTmaIa::Tipo::rinuncia,
                                    dz.soluzione_pronta ? stima_tma_ia(id, bid) : std::nullopt});
            fine_manovra_ia(o, ManovraDifesa{tempo_, tm->second.verso, tm->second.velocita_kn, tm->second.velocita_prima});
            S.tma.erase(tm);
            continue;
        }
        if (dz.nuova_gamba) S.eventi_tma.push_back({tempo_, id, bid, dz.gambe, EventoTmaIa::Tipo::gamba, std::nullopt});
        tm->second.gambe = dz.gambe;
        const double r = radianti(dz.rotta);
        tm->second.verso = {o.pos.x + ORIZZONTE_GAMBA_M * std::sin(r), o.pos.y + ORIZZONTE_GAMBA_M * std::cos(r)};
        tm->second.velocita_kn = dz.velocita_kn;
        // l'ordine dello script in questo istante vince (la gamba riprende al giro dopo); la difesa (fase 4 A) pure
        const auto os = ordini_script_.find(id);
        const auto ml = missili_ia_.find(id);   // la manovra di lancio dei missili (passo_missili_ia) viene prima
        tm->second.attiva = !(os != ordini_script_.end() && os->second >= tempo_) && !manovre_difesa_.count(id) &&
                            !(ml != missili_ia_.end() && ml->second.in_manovra);
    }
}

}  // namespace m688
