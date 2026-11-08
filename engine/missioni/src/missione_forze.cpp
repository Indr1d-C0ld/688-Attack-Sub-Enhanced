// Forze dell'Enhanced: allerta della forza e della formazione, situazione per coppia di parti con le provocazioni,
// regole d'ingaggio dell'IA (fase 3 delle interazioni fra IA, docs/18_interazioni_ia.md; dossier
// docs/ricerca/24_interazioni_fra_unita.md §8.1, §4.6, §8.7; nota condivisa ia-fase3-contratto.md).
//
// Gli eventi vengono dal registro (lanci, colpi, salve, affondamenti, classificazioni dei sonaristi dell'IA) e dai
// sensori (ping nemico sentito, albero visto: evento_allerta). La formazione che vive un evento sale subito; il resto
// della forza lo sa dopo il FLASH dalla formazione al teatro (finche' non ci sono i messaggi della fase B, un ritardo
// con la sua perdita). Il livello scende di uno ogni discesa_s senza eventi, fino al valore iniziale (la `paranoia`
// dell'editor) e mai sotto il minimo della situazione.

#include <algorithm>
#include <cmath>
#include <string>

#include <toml.hpp>

#include "missioni/armi_ia_missione.hpp"
#include "missioni/forze_missione.hpp"

extern "C" {
#include "lua.h"
}

namespace m688 {

namespace {

std::pair<int, int> coppia(int a, int b) { return {std::min(a, b), std::max(a, b)}; }
int lato_di(const UnitaClassica& u) { return u.flag & 0x18; }   // 0 neutrali, 8 usa, 16 urss, 24 la parte del luogo (M7)
bool arma_o_boa(const UnitaClassica& u) { return (u.flag & 0x20) || u.categoria == "arma" || u.categoria == "boa"; }
constexpr int ALBERO_PACE_MAX = 1;          // l'allerta che un albero visto in pace porta al piu': vigilanza (STIMA)
constexpr double AMICO_NOTO_M = 5000.0;   // la sua parte cosi' vicina e in superficie: la conosce a vista e per radio (STIMA)

}  // namespace

DatiForze carica_dati_forze(const std::string& percorso) {
    DatiForze d;
    toml::table t;
    try { t = toml::parse_file(percorso); } catch (const std::exception&) { return d; }
    if (const auto* a = t["allerta"].as_table()) {
        d.raggio_formazione_m = (*a)["raggio_formazione_m"].value_or(d.raggio_formazione_m);
        d.discesa_s = (*a)["discesa_s"].value_or(d.discesa_s);
        d.minimo_crisi = (*a)["minimo_crisi"].value_or(d.minimo_crisi);
        d.minimo_guerra = (*a)["minimo_guerra"].value_or(d.minimo_guerra);
        d.posto_combattimento = (*a)["posto_combattimento"].value_or(d.posto_combattimento);
        d.emcon_combattimento = (*a)["emcon_combattimento"].value_or(d.emcon_combattimento);
        if (const auto* f = (*a)["falsi_contatti"].as_array(); f && f->size() == 4)
            for (std::size_t i = 0; i < 4; ++i) d.falsi_contatti[i] = (*f)[i].value_or(d.falsi_contatti[i]);
        if (const auto* ev = (*a)["eventi"].as_table())
            for (const auto& [k, v] : *ev) {
                const auto* e = v.as_table();
                if (!e) continue;
                DatiForze::Evento x;
                x.formazione = (*e)["formazione"].value_or(0);
                x.formazione_max = (*e)["formazione_max"].value_or(3);
                x.forza = (*e)["forza"].value_or(0);
                x.forza_max = (*e)["forza_max"].value_or(3);
                x.formazione_fino_a = (*e)["formazione_fino_a"].value_or(-1);
                x.forza_fino_a = (*e)["forza_fino_a"].value_or(-1);
                d.eventi[std::string(k.str())] = x;
            }
    }
    const auto intervallo = [](const toml::node_view<const toml::node> n, std::pair<double, double> def) {
        if (const auto* r = n.as_array(); r && r->size() == 2) return std::pair<double, double>{(*r)[0].value_or(def.first), (*r)[1].value_or(def.second)};
        return def;
    };
    for (const auto& [nome, lato] : {std::pair<const char*, int>{"usa", 0x08}, {"urss", 0x10}})
        if (const auto* m = t["messaggi"][nome].as_table()) {
            const toml::node_view<const toml::node> v{m};
            DatiForze::Canali c;
            c.formazione_s = intervallo(v["formazione_s"], c.formazione_s);
            c.aereo_nave_s = intervallo(v["aereo_nave_s"], c.aereo_nave_s);
            c.flash_s = intervallo(v["teatro_flash_s"], c.flash_s);
            c.immediate_s = intervallo(v["teatro_immediate_s"], c.immediate_s);
            c.formazione_perdita = v["formazione_perdita"].value_or(c.formazione_perdita);
            c.aereo_nave_riuscita = v["aereo_nave_riuscita"].value_or(c.aereo_nave_riuscita);
            d.canali[lato] = c;
            d.teatro_s[lato] = c.flash_s;
            d.teatro_perdita = v["teatro_perdita"].value_or(d.teatro_perdita);
        }
    if (const auto* m = t["messaggi"]["comuni"].as_table()) {
        const toml::node_view<const toml::node> v{m};
        d.riferire_ogni_s = v["riferire_ogni_s"].value_or(d.riferire_ogni_s);
        d.r0_m = v["r0_m"].value_or(d.r0_m);
        d.ellisse_max_m = v["ellisse_max_m"].value_or(d.ellisse_max_m);
        d.broadcast_s = intervallo(v["broadcast_ciclo_s"], d.broadcast_s);
        if (const auto* k = v["velocita_kn"].as_table())
            for (const auto& [c, x] : *k) d.velocita_kn[std::string(c.str())] = x.value_or(20.0);
    }
    if (const auto* r = t["roe_ia"].as_table()) d.neutrali_a3 = (*r)["neutrali_a3"].value_or(d.neutrali_a3);
    if (const auto* z = t["zone"].as_table()) {
        d.avanti_nm = (*z)["avanti_nm"].value_or(d.avanti_nm);
        d.indietro_nm = (*z)["indietro_nm"].value_or(d.indietro_nm);
        d.lato_nm = (*z)["lato_nm"].value_or(d.lato_nm);
        d.dopo_attacco_s = (*z)["dopo_attacco_s"].value_or(d.dopo_attacco_s);
        d.fuoco_amico[0x08] = (*z)["fuoco_amico_usa"].value_or(d.fuoco_amico[0x08]);
        d.fuoco_amico[0x10] = (*z)["fuoco_amico_urss"].value_or(d.fuoco_amico[0x10]);
    }
    return d;
}

void Missione::forze_enhanced(const std::string& cartella_dati) {
    if (forze_ || forze_spente_) {
        if (!forze_ && !armi_ia_ && !armi_ia_spente_) {   // forze spente: difesa, armi e navigazione hanno i loro interruttori
            caso_difesa_ = Casuale(seme_ ^ 0xD1FE5AD1FE5Aull);
            caso_reazioni_ = Casuale(seme_ ^ 0x5EAC75EAC7ull);
            armi_ia_enhanced(cartella_dati);
        }
        if (!forze_) navigazione_enhanced(cartella_dati);
        return;
    }
    auto s = std::make_unique<StatoForze>();
    s->d = carica_dati_forze(cartella_dati + "/interazioni.toml");
    s->caso = Casuale(seme_ ^ 0xF02CEF02CEF02CEull);
    s->caso_radio = Casuale(seme_ ^ 0xA4D10A4D10A4D10ull);
    // i generatori della fase 4 dal seme della missione (difesa e reazioni: ogni partita i suoi)
    caso_difesa_ = Casuale(seme_ ^ 0xD1FE5AD1FE5Aull);
    caso_reazioni_ = Casuale(seme_ ^ 0x5EAC75EAC7ull);
    // la situazione di partenza: il blocco carriera, o quella dell'editor, o le originali in carriera.toml; senza
    // niente la guerra (le originali sono missioni di guerra, come l'ASW aereo)
    std::optional<Situazione> base;
    if (blocco_carriera_) base = situazione_da_nome(blocco_carriera_->situazione);
    lua_rawgeti(L_, LUA_REGISTRYINDEX, rif_missione_);
    lua_getfield(L_, -1, "editor");
    if (lua_istable(L_, -1)) {
        lua_getfield(L_, -1, "scenario");
        if (lua_istable(L_, -1)) {
            const int sc = lua_gettop(L_);
            lua_getfield(L_, sc, "situazione");
            if (!base && lua_isstring(L_, -1)) base = situazione_da_nome(lua_tostring(L_, -1));
            lua_pop(L_, 1);
            lua_getfield(L_, sc, "paranoia");
            if (lua_isnumber(L_, -1)) s->iniziale = std::clamp(static_cast<int>(lua_tointeger(L_, -1)), 0, 3);
            lua_pop(L_, 1);
            lua_getfield(L_, sc, "unita");   // la `paranoia` delle unita' (parametri)
            if (lua_istable(L_, -1)) {
                const int n = static_cast<int>(lua_rawlen(L_, -1));
                for (int i = 1; i <= n; ++i) {
                    lua_rawgeti(L_, -1, i);
                    if (lua_istable(L_, -1)) {
                        lua_getfield(L_, -1, "id");
                        const int id = lua_isnumber(L_, -1) ? static_cast<int>(lua_tointeger(L_, -1)) : -1;
                        lua_pop(L_, 1);
                        lua_getfield(L_, -1, "parametri");
                        if (id >= 0 && lua_istable(L_, -1)) {
                            lua_getfield(L_, -1, "paranoia");
                            if (lua_isnumber(L_, -1)) s->iniziale_unita[id] = std::clamp(static_cast<int>(lua_tointeger(L_, -1)), 0, 3);
                            lua_pop(L_, 1);
                        }
                        lua_pop(L_, 1);
                    }
                    lua_pop(L_, 1);
                }
            }
            lua_pop(L_, 1);
        }
        lua_pop(L_, 1);
    }
    lua_pop(L_, 2);
    if (!base) {
        try {
            const RegoleCarriera r = carica_carriera(cartella_dati + "/carriera.toml");
            if (const auto it = r.originali.find(id_); it != r.originali.end()) base = it->second.situazione;
        } catch (const std::exception&) {}
    }
    s->base = base.value_or(Situazione::guerra);
    s->base_esplicita = base.has_value();
    // M7, la parte del luogo (unita' con schieramento = 'luogo', lato 24): la sua forza e la situazione di partenza con
    // le due parti, chiave di missione `situazione_luogo = { usa = 'guerra', urss = 'pace' }` (senza: pace con tutti)
    std::vector<int> lati = {0x08, 0x10, 0};
    if (std::any_of(unita_.begin(), unita_.end(), [](const auto& k) { return (k.second.flag & 0x18) == 0x18; })) {
        lati.push_back(0x18);
        lua_rawgeti(L_, LUA_REGISTRYINDEX, rif_missione_);
        lua_getfield(L_, -1, "situazione_luogo");
        if (lua_istable(L_, -1))
            for (const auto& [nome, lato] : {std::pair<const char*, int>{"usa", 0x08}, {"urss", 0x10}}) {
                lua_getfield(L_, -1, nome);
                if (lua_isstring(L_, -1))
                    if (const auto x = situazione_da_nome(lua_tostring(L_, -1))) s->coppie[coppia(lato, 0x18)] = *x;
                lua_pop(L_, 1);
            }
        lua_pop(L_, 2);
    }
    for (const int lato : lati) {
        auto& f = s->forza[lato];
        f.v = f.iniziale = s->iniziale;
        // la sandbox: l'allerta di partenza della parte vale per la forza e per tutte le sue formazioni
        if (const auto r = regolazioni_.find(lato); r != regolazioni_.end() && r->second.allerta) {
            f.v = f.iniziale = std::clamp(*r->second.allerta, 0, 3);
            std::erase_if(s->iniziale_unita, [&](const auto& k) {
                const auto u = unita_.find(k.first);
                return u != unita_.end() && lato_di(u->second) == lato;
            });
        }
    }
    s->eventi_visti = registro_.eventi().size();
    for (const int lato : lati)   // il ciclo del broadcast di ogni parte, nell'intervallo dei dati
        s->ciclo_s[lato] = s->d.broadcast_s.first + (s->d.broadcast_s.second - s->d.broadcast_s.first) * s->caso.uniforme();
    forze_ = std::move(s);
    armi_ia_enhanced(cartella_dati);   // fase 4 B: carico, TMA e filo dell'IA
    forze_->quadro_sottomarini = !quadro_sottomarini_spento_ && armi_ia_ && !armi_ia_->originale;
    navigazione_enhanced(cartella_dati);   // la navigazione incerta del giocatore, se accesa (usa_navigazione)
    // i minimi della situazione di partenza (come la vede la parte); l'allerta regolata della sandbox vince: una forza
    // ad A0 in guerra e' una forza sorpresa
    for (auto& [lato, f] : forze_->forza) {
        if (const auto r = regolazioni_.find(lato); r != regolazioni_.end() && r->second.allerta) continue;
        const Situazione sit = std::max(situazione_di(lato, 0x08), situazione_di(lato, 0x10));
        const int minimo = sit == Situazione::guerra ? forze_->d.minimo_guerra : sit == Situazione::crisi ? forze_->d.minimo_crisi : 0;
        f.v = f.iniziale = std::max(f.iniziale, minimo);
    }
}

std::optional<int> Missione::allerta(int lato) const {
    if (!forze_) return std::nullopt;
    const auto it = forze_->forza.find(lato);
    return it != forze_->forza.end() ? it->second.v : forze_->iniziale;
}

std::optional<int> Missione::allerta_unita(int id) const {
    if (!forze_) return std::nullopt;
    const auto ui = unita_.find(id);
    if (ui == unita_.end()) return std::nullopt;
    int v = *allerta(lato_di(ui->second));
    if (const auto it = forze_->unita.find(id); it != forze_->unita.end()) v = std::max(v, it->second.v);
    else if (const auto k = forze_->iniziale_unita.find(id); k != forze_->iniziale_unita.end()) v = std::max(v, k->second);
    return v;
}

std::map<int, int> Missione::livelli_forze() const {
    std::map<int, int> l;
    if (forze_)
        for (const auto& [lato, f] : forze_->forza) l[lato] = f.v;
    return l;
}

// non allerta_unita: la forza di adesso ha gia' la provocazione del lancio stesso, minuti prima che qualcuno possa saperlo
std::optional<int> Missione::allerta_contro_missile(const UnitaClassica& v, const std::map<int, int>& al_lancio) const {
    if (!forze_) return std::nullopt;
    const auto f = al_lancio.find(lato_di(v));
    int n = f != al_lancio.end() ? f->second : forze_->iniziale;
    if (const auto it = forze_->unita.find(v.id); it != forze_->unita.end()) n = std::max(n, it->second.v);
    else if (const auto k = forze_->iniziale_unita.find(v.id); k != forze_->iniziale_unita.end()) n = std::max(n, k->second);
    return n;
}

Situazione Missione::situazione_fra(int a, int b) const {
    if (!forze_) {
        if (blocco_carriera_) return situazione_da_nome(blocco_carriera_->situazione).value_or(Situazione::guerra);
        return Situazione::guerra;
    }
    if (a == b) return Situazione::pace;
    const auto it = forze_->coppie.find(coppia(a, b));
    if (it != forze_->coppie.end()) return it->second;
    // i neutrali e la parte del luogo (M7, senza situazione_luogo): in pace con tutti finche' nessuno li provoca
    return a == 0 || b == 0 || a == 0x18 || b == 0x18 ? Situazione::pace : forze_->base;
}

void Missione::interruttori_ia(const InterruttoriIa& i) {
    usa_forze(i.forze);
    usa_difesa(i.difesa);
    usa_armi_ia(i.armi);
    usa_reazioni(i.reazioni);
    usa_esm_ia(i.esm);
    usa_sonaristi_ia(i.sonaristi);
    usa_evasione(i.evasione);
    usa_esche_mobili(i.esche_mobili);
}

std::optional<bool> Missione::roe_parte(int lato, CategoriaRoe c) const {
    const auto r = regolazioni_.find(lato);
    if (r == regolazioni_.end() || !r->second.roe) return std::nullopt;
    const auto k = r->second.roe->find(c);
    return k != r->second.roe->end() ? std::optional<bool>(k->second) : std::nullopt;
}

Situazione Missione::situazione_di(int chi, int altro) const {
    if (forze_ && forze_->coppie.count(coppia(chi, altro))) return situazione_fra(chi, altro);
    if (const auto r = regolazioni_.find(chi); r != regolazioni_.end() && r->second.situazione && chi != altro) return *r->second.situazione;
    return situazione_fra(chi, altro);
}

std::optional<Situazione> Missione::situazione_nota(int a, int b) const {
    if (!forze_) return std::nullopt;
    if (const auto r = regolazioni_.find(a); r != regolazioni_.end() && r->second.situazione && !forze_->coppie.count(coppia(a, b)) && a != b)
        return *r->second.situazione;   // la sandbox: la partenza di questa parte
    if (!forze_->base_esplicita && !forze_->coppie.count(coppia(a, b))) return std::nullopt;
    return situazione_fra(a, b);
}

void Missione::dichiara_situazione(Situazione s, int a, int b) {
    if (!forze_) return;
    if (a < 0 || b < 0) forze_->base_esplicita = true;
    const auto cambia = [&](int x, int y) { forze_->coppie[coppia(x, y)] = s; };
    if (a < 0 || b < 0) {
        forze_->base = s;
        for (auto& [k, v] : forze_->coppie)
            if (k.first != 0 && k.second != 0x18) v = s;   // le coppie coi neutrali e col luogo restano come sono
        cambia(0x08, 0x10);
    } else {
        cambia(a, b);
    }
    // i minimi: almeno A1 in crisi, A2 in guerra, per le parti coinvolte
    const int minimo = s == Situazione::guerra ? forze_->d.minimo_guerra : s == Situazione::crisi ? forze_->d.minimo_crisi : 0;
    for (auto& [lato, f] : forze_->forza) {
        if (a >= 0 && b >= 0 && lato != a && lato != b) continue;
        if ((a < 0 || b < 0) && (lato == 0 || lato == 0x18)) continue;
        if (f.v < minimo) {
            forze_->cambi.push_back({tempo_, lato, -1, f.v, minimo, "situazione"});
            f.v = minimo;
            f.ultimo = tempo_;
        }
    }
}

const std::vector<Missione::CambioAllerta>& Missione::cambi_allerta() const {
    static const std::vector<CambioAllerta> vuoto;
    return forze_ ? forze_->cambi : vuoto;
}
const std::vector<Missione::FuocoAmicoIa>& Missione::fuoco_amico_ia() const {
    static const std::vector<FuocoAmicoIa> vuoto;
    return forze_ ? forze_->fuoco_amico : vuoto;
}
const std::vector<Missione::MessaggioIa>& Missione::messaggi_ia() const {
    static const std::vector<MessaggioIa> vuoto;
    return forze_ ? forze_->messaggi : vuoto;
}
const std::vector<Missione::Provocazione>& Missione::provocazioni() const {
    static const std::vector<Provocazione> vuoto;
    return forze_ ? forze_->provocazioni : vuoto;
}

// Un evento della tabella del 24 §8.1 vissuto da un'unita': la sua formazione sale subito, la forza al FLASH
void Missione::evento_allerta(int id, const std::string& evento) {
    if (!forze_) return;
    auto& F = *forze_;
    const auto ev = F.d.eventi.find(evento);
    const auto* u = trova(id);
    if (ev == F.d.eventi.end() || !u || u->giocatore || arma_o_boa(*u)) return;
    const auto& e = ev->second;
    const int lato = lato_di(*u);
    const int forza = *allerta(lato);
    // un albero o un battello visto in pace (con la parte del giocatore, o da un neutrale) non e' una minaccia: al piu'
    // ALBERO_PACE_MAX, non il combattimento (la principale, 31/10/2026: l'Orion della S23 in A3 dopo 150 s per un battello
    // in superficie e in avaria)
    int massimo = e.formazione_max;
    if (evento == "albero")
        if (const auto* g = giocatore(); lato == 0 || (g && situazione_fra(lato, lato_di(*g)) == Situazione::pace))
            massimo = std::min(massimo, ALBERO_PACE_MAX);
    // la formazione: le unita' della parte entro il raggio (il giocatore no: il suo equipaggio non e' dell'IA)
    if (e.formazione > 0 || e.formazione_fino_a >= 0) {
        const int prima = *allerta_unita(id);
        int dopo = prima;
        for (const auto& [k, v] : unita_) {
            if (v.distrutta || v.persa || v.giocatore || arma_o_boa(v) || lato_di(v) != lato) continue;
            if (k != id && distanza(v.pos, u->pos) > F.d.raggio_formazione_m) continue;
            auto it = F.unita.find(k);
            if (it == F.unita.end()) {
                StatoForze::Livello l;
                const auto ki = F.iniziale_unita.find(k);
                l.iniziale = std::max(forza, ki != F.iniziale_unita.end() ? ki->second : 0);
                l.v = l.iniziale;
                it = F.unita.emplace(k, l).first;
            }
            auto& l = it->second;
            const int base = std::max(l.v, forza);
            int n = std::max(base, std::min(base + e.formazione, massimo));
            if (e.formazione_fino_a >= 0) n = std::max(n, e.formazione_fino_a);
            n = std::min(n, 3);
            l.v = n;
            l.ultimo = tempo_;
            if (k == id) dopo = n;
        }
        if (dopo != prima) F.cambi.push_back({tempo_, lato, id, prima, dopo, evento});
    }
    // il resto della forza: al FLASH dalla formazione al teatro, se non si perde (col disturbo della D3: si perde di piu'
    // e arriva piu' tardi; senza, gli stessi numeri casuali di prima)
    if (e.forza > 0 || e.forza_fino_a >= 0) {
        if (F.caso.uniforme() < F.d.teatro_perdita) return;
        const DisturboRadio dz = disturbo_da(lato, u ? u->pos : Vec2{});
        if (dz.perdita > 0.0 && F.caso.uniforme() < dz.perdita) {
            if (u) reazioni_ia_.push_back({tempo_, u->id, -1, "radio", "messaggio_perso", "disturbo_radio"});   // per lo spettatore
            return;
        }
        const auto r = F.d.teatro_s.find(lato);
        const auto [da, a] = r != F.d.teatro_s.end() ? r->second : std::pair<double, double>{300.0, 600.0};
        F.in_arrivo.push_back({tempo_ + (da + (a - da) * F.caso.uniforme()) * dz.ritardo, lato, evento});
    }
}

int Missione::parte_nota(const UnitaClassica& o, const UnitaClassica& b) const {
    const int vera = b.flag & 0x18;
    if (!forze_ || forze_->cambi_parte.empty() || o.id == b.id) return vera;
    const auto c = forze_->cambi_parte.find(b.id);
    if (c == forze_->cambi_parte.end() || c->second.sanno.count(o.id)) return vera;
    const auto s = c->second.sapra.find(o.flag & 0x18);
    return s != c->second.sapra.end() && tempo_ >= s->second ? vera : c->second.lato_prima;
}

int Missione::parte_creduta(int osservatore, int unita) const {
    const auto o = unita_.find(osservatore), b = unita_.find(unita);
    return o != unita_.end() && b != unita_.end() ? lato_creduto(o->second, b->second) : -2;
}

int Missione::lato_creduto(const UnitaClassica& o, const UnitaClassica& b) const {
    // la parte che l'IA crede: quella vera, salvo una classificazione sbagliata del suo sonarista (la parte di
    // un'unita' della missione di quella classe; un biologico non e' di nessuno). Un'identificazione visiva fresca
    // (il libretto, docs/25 §3.5) viene prima del sonarista
    // un'unita' della sua parte vicina e in superficie la conosce (a vista e per radio, la formazione all'ancora): ne'
    // il sonarista ne' la vedetta la cambiano (la principale, 30/10/2026: nella S15 l'Ugra prendeva il suo Charlie II
    // affiancato per un Los Angeles). Sott'acqua l'errore fra amici resta possibile (24 §8.7)
    const int nota = parte_nota(o, b);   // il cambio di parte (D4): chi non lo sa la crede quella di prima
    if (lato_di(o) != 0 && lato_di(o) == nota && (b.categoria == "nave" || b.quota < 1.0) && distanza(o.pos, b.pos) <= AMICO_NOTO_M)
        return nota;
    if (const auto v = lato_visivo(o.id, b.id)) return *v;
    // La parte segue la classe creduta, giusta o no (la principale, 29/10/2026: come errata_per_verita del registro):
    // la classe detta da' la parte di un'unita' di quella classe; un biologico, o un sottomarino creduto NONSUB senza
    // classe, non e' di nessuno
    if (const auto si = sonaristi_ia_.find(o.id); si != sonaristi_ia_.end())
        if (const TracciaSonar* t = si->second.s->traccia_di(b.id); t && t->stato == TracciaSonar::Stato::classificata) {
            if (t->classe == "biologico") return -1;
            if (t->classe.empty()) {
                if (t->confidenza == Confidenza::nonsub && b.categoria == "sottomarino") return -1;
            } else {
                for (const auto& [k, v] : unita_)
                    if (v.classe == t->classe) return lato_di(v);
            }
        }
    return nota;
}

bool Missione::in_zona_amica(int id, int bersaglio) {
    if (!forze_) return false;
    const auto* u = trova(id);
    if (!u) return false;
    if (bersaglio < 0) bersaglio = u->bersaglio;
    const auto* b = bersaglio >= 0 ? trova(bersaglio) : nullptr;
    if (!b) return false;
    // dove crede che sia: la posizione del contatto, se no quella vera (l'arma cade li')
    Vec2 p = b->pos;
    if (const auto c = u->contatti.find(b->id); c != u->contatti.end() && c->second.posizione_nota) p = c->second.pos_stimata;
    return zona_amica_in(id, p);
}

bool Missione::zona_amica_in(int id, Vec2 p) {
    if (!forze_) return false;
    auto& F = *forze_;
    const auto* u = trova(id);
    if (!u) return false;
    const int lu = lato_di(*u);
    // le aree d'azione dichiarate al giocatore dalla missione (R2): se ce ne sono, valgono al posto della sua mobile
    std::vector<Zona> dichiarate;
    if (radio_)
        for (const auto& z : radio_->zone_trasmesse(tempo_))
            if (z.tipo == TipoZona::azione && z.valida(tempo_)) dichiarate.push_back(z);
    const double nm = 1852.0;
    for (const auto& [k, s] : unita_) {
        if (k == id || s.distrutta || s.persa || s.categoria != "sottomarino" || lato_di(s) != lu || (s.flag & 0x20)) continue;
        auto [ig, nuovo] = F.zona_ignota.try_emplace({id, k}, false);
        if (nuovo) ig->second = F.caso.uniforme() < F.d.teatro_perdita;   // il messaggio della zona perso
        if (ig->second) continue;
        if (s.giocatore && !dichiarate.empty()) {
            for (const auto& z : dichiarate)
                if (dentro_zona(z, p, posa_zona(z))) return true;
            continue;
        }
        Zona z;
        z.tipo = TipoZona::protetta_mobile;
        z.unita = k;
        z.avanti_m = F.d.avanti_nm * nm;
        z.dietro_m = F.d.indietro_nm * nm;
        z.lato_m = F.d.lato_nm * nm;
        if (dentro_zona(z, p, PosaUnita{s.pos, s.rotta})) return true;
    }
    return false;
}

// la P della prima ora dopo un attacco nemico alla parte: chi la estrae per la coppia ignora la zona (24 §8.3)
bool Missione::ignora_zona_amica(int id, int bersaglio) {
    auto& F = *forze_;
    const int lu = lato_di(*trova(id));
    const auto at = F.attaccata.find(lu);
    if (at == F.attaccata.end() || tempo_ - at->second > F.d.dopo_attacco_s) return false;
    auto [it, nuovo] = F.ignora_zona.try_emplace({id, bersaglio}, false);
    if (nuovo) {
        const auto p = F.d.fuoco_amico.find(lu);
        it->second = F.caso.uniforme() < (p != F.d.fuoco_amico.end() ? p->second : 0.05);
    }
    return it->second;
}

bool Missione::sgancio_vietato(int aereo, int bersaglio, Vec2 punto) {
    if (!forze_ || !trova(aereo) || !zona_amica_in(aereo, punto)) return false;
    const auto* u = trova(aereo);
    if (const auto c = u->contatti.find(bersaglio); bersaglio >= 0 && c != u->contatti.end() && c->second.durata >= 1400.0) return false;
    return !ignora_zona_amica(aereo, bersaglio);
}

std::optional<bool> Missione::armi_libere_ia(int id, int bersaglio) {
    if (!forze_) return std::nullopt;
    const auto* u = trova(id);
    if (!u) return false;
    const int lu = lato_di(*u);
    if (bersaglio < 0) bersaglio = u->bersaglio;
    const auto* b = bersaglio >= 0 ? trova(bersaglio) : nullptr;
    if (!b) {   // senza bersaglio: le armi sono libere contro la parte ostile
        if (lu == 0) return false;
        return situazione_di(lu, lu == 0x08 ? 0x10 : 0x08) == Situazione::guerra;
    }
    const int lb = lato_creduto(*u, *b);
    if (lb < 0 || lb == lu) return false;
    // il falso ordine creduto (D3): le armi trattenute o libere sull'altra parte, sopra la situazione; i neutrali no
    if (lb != 0)
        if (const auto f = roe_ordine_falso(id)) return *f;
    // la sandbox: le ROE per categoria della parte prima della situazione
    const std::map<CategoriaRoe, bool>* roe = nullptr;
    if (const auto r = regolazioni_.find(lu); r != regolazioni_.end() && r->second.roe) roe = &*r->second.roe;
    // il vetrino di Dio: chi non ha ancora ricevuto le ROE nuove della sua parte (giro_radio_roe) segue quelle di prima
    for (std::size_t k = interventi_.size(); k-- > 0;) {
        const Intervento& x = interventi_[k];
        const auto pt = x.testi.find("parte");
        const int lx = pt == x.testi.end() ? -1 : pt->second == "usa" ? 0x08 : pt->second == "urss" ? 0x10 : 0;
        if (x.tipo != "roe" || !stati_interventi_[k].applicato || lx != lu) continue;
        if (const auto ric = roe_ricevute_.find(x.id); ric == roe_ricevute_.end() || !ric->second.count(id)) {
            const auto rp = ripristini_.find(x.id);
            roe = rp != ripristini_.end() && rp->second.roe ? &*rp->second.roe : nullptr;
        }
        break;
    }
    if (roe)
        if (const auto c = roe->find(lb == 0 ? CategoriaRoe::neutrali : categoria_roe(*b)); c != roe->end()) return c->second;
    // un neutrale: mai, salvo ad A3 su un contatto non classificato (24 §8.7), una volta per coppia. Vale solo per i
    // neutrali dichiarati dalla missione: le navi del traffico (docs/20 §3) ne restano fuori, perche' con 40 navi
    // attorno P per coppia sarebbe una strage e non un errore raro (principale, 28/10/2026); il loro rischio storico
    // lo porta l'arma, il TASM o l'Harpoon in ricerca che aggancia la nave sbagliata
    if (lb == 0) {
        if (situazione_di(lu, 0) == Situazione::guerra) return true;   // provocati dal neutrale
        if (del_traffico(b->id)) return false;   // il traffico: niente errore d'identificazione ad A3 (sopra)
        const auto c = u->contatti.find(b->id);
        const bool classificato = c != u->contatti.end() && c->second.durata >= 1400.0;
        if (classificato || allerta_unita(id).value_or(0) < 3) return false;
        auto [it, nuovo] = forze_->neutrali.try_emplace({id, b->id}, false);
        if (nuovo) it->second = forze_->caso.uniforme() < forze_->d.neutrali_a3;
        return it->second;
    }
    if (situazione_di(lu, lb) != Situazione::guerra) return false;
    // le zone dei sottomarini amici (fase C): navi e aerei non usano armi antisom su un contatto subacqueo dentro,
    // salvo uno classificato per classe (ostile, giusto o sbagliato) e la P della prima ora dopo un attacco nemico
    if (u->categoria != "sottomarino" && b->categoria == "sottomarino") {
        const auto c = u->contatti.find(b->id);
        const bool per_classe = c != u->contatti.end() && c->second.durata >= 1400.0;
        if (!per_classe && in_zona_amica(id, b->id)) return ignora_zona_amica(id, b->id);
    }
    return true;
}

// --- messaggi di contatto (fase B) ----------------------------------------------------------------------------------

namespace {
// il rapporto porta il suo errore: quello di un fix della rete, se no quello di ogni rapporto
double errore_rapporto(const Missione::RapportoContatto& r, const DatiForze& d) { return r.errore_m > 0.0 ? r.errore_m : d.r0_m; }
}  // namespace

std::optional<Missione::ContattoRadio> Missione::contatto_radio(int ricevente, int bersaglio) const {
    if (!forze_) return std::nullopt;
    const auto it = forze_->quadro.find({ricevente, bersaglio});
    if (it == forze_->quadro.end()) return std::nullopt;
    ContattoRadio c = it->second;
    const auto v = forze_->d.velocita_kn.find(c.r.categoria);
    const double u = (v != forze_->d.velocita_kn.end() ? v->second : 20.0) * m_s_per_nodo_;
    c.raggio_m = errore_rapporto(c.r, forze_->d) + u * std::max(0.0, tempo_ - c.r.t_fix);
    c.posizione_valida = c.r.pos.has_value() && c.raggio_m < forze_->d.ellisse_max_m;
    return c;
}

std::vector<std::pair<int, Missione::ContattoRadio>> Missione::quadro_radio(int ricevente) const {
    std::vector<std::pair<int, ContattoRadio>> v;
    if (!forze_) return v;
    for (auto it = forze_->quadro.lower_bound({ricevente, -1000000}); it != forze_->quadro.end() && it->first.first == ricevente; ++it)
        if (const auto c = contatto_radio(ricevente, it->first.second)) v.emplace_back(it->first.second, *c);
    return v;
}

void Missione::consegna_rapporto(int k, const RapportoContatto& r) {
    auto& F = *forze_;
    const auto* u = trova(k);
    if (!u || u->distrutta || u->persa) return;
    // chi ha un contatto suo e fresco non ha bisogno del rapporto
    if (const auto c = u->contatti.find(r.su); c != u->contatti.end() && tempo_ - c->second.ultima_scoperta <= 60.0 &&
                                                (c->second.qualita >= 800.0 || c->second.posizione_nota))
        return;
    auto& q = F.quadro[{k, r.su}];
    if (q.t_arrivo > 0.0 && q.r.t_fix > r.t_fix) return;   // ne ha gia' uno piu' recente
    q.r = r;
    q.t_arrivo = tempo_;
    // l'aereo dell'ASW aereo: il rapporto con la posizione su un sottomarino e' un datum (24 §8.4: «sul contatto»)
    if (r.pos && controllato_asw(k) && (r.categoria == "sottomarino" || r.categoria == "sconosciuto")) {
        Cue c;
        c.tipo = Cue::Tipo::contatto_nave;
        c.pos = *r.pos;
        c.t = r.t_fix;
        c.raggio_m = errore_rapporto(r, F.d);
        c.unita = r.su;
        cue_asw(k, c);
    }
}

// il disturbo della radio della parte (D3) per chi trasmette da p; senza interventi niente (e nessun numero casuale)
DisturboRadio Missione::disturbo_da(int lato, Vec2 p) const {
    return interventi_.empty() ? DisturboRadio{} : disturbo_radio(interventi_, stati_interventi_, lato, p);
}

namespace {
constexpr double CACCIA_DOPO_SCRIPT_S = 60.0;   // il dato radio torna buono solo un minuto dopo un ordine dello script (STIMA)
constexpr double COPIA_S = 80.0;   // il messaggio si copia tutto: acquisizione VLF e caratteri (CONTACT REPORT, misurato 78 s dal giocatore)
const ParametriRadio RADIO;        // le regole della radio del giocatore: l'albero oltre 10 nodi al 50%, la vita del dato in cicli
constexpr double RAPPORTO_UHF_S = 1.5;      // il rapporto di 300 caratteri in UHF (STIMA); l'URSS trasmette la raffica di sempre
constexpr double TRASMISSIONE_OGNI_S = 60.0;   // al piu' una trasmissione al minuto per battello (STIMA)
}  // namespace

void Missione::passo_messaggi() {
    auto& F = *forze_;
    const auto* g = giocatore();
    const auto ritardo = [&](std::pair<double, double> r) { return r.first + (r.second - r.first) * F.caso.uniforme(); };
    const auto canali = [&](int lato) { const auto it = F.d.canali.find(lato); return it != F.d.canali.end() ? it->second : DatiForze::Canali{}; };
    const auto e_aereo = [&](const UnitaClassica& v) { return (v.flag & 3) == 3 || v.categoria == "aereo"; };
    const auto riceve = [&](const UnitaClassica& v) { return (v.flag54 & 6) != 0 || v.quota <= 20.0; };
    // il sottomarino al ciclo: a quota d'antenna da tutta la durata del messaggio, con la regola della velocita' del giocatore
    const auto copia = [&](int k, const UnitaClassica& v) {
        if (!F.quadro_sottomarini) return riceve(v);
        const auto r = F.radio_sottomarini.find(k);
        if (r == F.radio_sottomarini.end() || r->second.esposto_da < 0.0 || tempo_ - r->second.esposto_da < COPIA_S) return false;
        return std::fabs(v.velocita) <= RADIO.velocita_albero_kn || F.caso_radio.uniforme() < 0.5;
    };
    // i rapporti nuovi: ogni unita' dell'IA sui contatti ostili che puo' usare
    for (const auto& [id, o] : unita_) {
        if (o.giocatore || o.distrutta || o.persa || arma_o_boa(o)) continue;
        const int lo = lato_di(o);
        const bool sub = o.categoria == "sottomarino";
        if (sub && (F.quadro_sottomarini ? !a_quota_antenna(o) || sotto_banchisa(id) : !riceve(o))) continue;   // un sottomarino immerso non trasmette
        for (const auto& [bid, c] : o.contatti) {
            const auto* b = trova(bid);
            if (!b || b->distrutta || b->persa || arma_o_boa(*b)) continue;
            if (!(c.qualita >= 800.0 || c.posizione_nota) || tempo_ - c.ultima_scoperta > 60.0) continue;
            const int lb = lato_creduto(o, *b);
            if (lb < 0 || lb == lo || lb == 0) continue;
            const int livello = c.durata >= 1400.0 ? 2 : c.durata >= 800.0 ? 1 : 0;
            auto& [quando, gia] = F.riferiti.try_emplace({id, bid}, std::pair<double, int>{-1e18, -1}).first->second;
            const bool nuovo = gia < 0 || livello > gia || (c.posizione_nota && tempo_ - quando >= F.d.riferire_ogni_s);
            if (!nuovo) continue;
            quando = tempo_;
            gia = std::max(gia, livello);
            RapportoContatto r;
            r.da = id;
            r.su = bid;
            r.t_fix = c.posizione_nota ? c.t_posizione : c.ultima_scoperta;   // l'ellisse cresce dall'eta' della posizione
            // dove crede di essere chi riferisce (la navigazione incerta): la posizione riferita si sposta con la sua stima
            const Vec2 e = errore_navigazione(id);
            if (c.posizione_nota) r.pos = Vec2{c.pos_stimata.x + e.x, c.pos_stimata.y + e.y};
            r.origine = {o.pos.x + e.x, o.pos.y + e.y};
            r.rilevamento = c.rilevamento;
            r.livello = livello;
            r.categoria = livello == 0 ? "sconosciuto" : e_aereo(*b) ? "aereo" : b->categoria == "sottomarino" ? "sottomarino" : "nave";
            if (livello == 2) {
                r.classe = b->classe;
                if (const auto si = sonaristi_ia_.find(id); si != sonaristi_ia_.end())
                    if (const TracciaSonar* t = si->second.s->traccia_di(bid); t && !t->classe.empty()) r.classe = t->classe;
            }
            const DatiForze::Canali ch = canali(lo);
            const DisturboRadio dz = disturbo_da(lo, o.pos);   // il disturbo della D3 su chi trasmette
            const auto disturbato = [&] { return dz.perdita > 0.0 && F.caso.uniforme() < dz.perdita; };
            // la formazione: voce e Link 11, dall'aereo alla nave col suo canale (i sottomarini no: niente link)
            if (!sub) {
                bool voce = false, aereo_nave = false, persi = false;
                for (const auto& [k, v] : unita_) {
                    if (k == id || v.giocatore || v.distrutta || v.persa || arma_o_boa(v) || lato_di(v) != lo || v.categoria == "sottomarino") continue;
                    if (distanza(v.pos, o.pos) > F.d.raggio_formazione_m) continue;
                    const bool an = e_aereo(o) && !e_aereo(v);
                    bool perso = an ? F.caso.uniforme() >= ch.aereo_nave_riuscita : F.caso.uniforme() < ch.formazione_perdita;
                    if (!perso && disturbato()) perso = persi = true;
                    (an ? aereo_nave : voce) = true;
                    if (!perso) F.in_viaggio.push_back({tempo_ + ritardo(an ? ch.aereo_nave_s : ch.formazione_s) * dz.ritardo, k, lo, r});
                }
                using C = MessaggioIa::Canale;
                if (voce) F.messaggi.push_back({tempo_, tempo_ + ritardo(ch.formazione_s) * dz.ritardo, id, bid, -1, C::formazione, persi});
                if (aereo_nave) F.messaggi.push_back({tempo_, tempo_ + ritardo(ch.aereo_nave_s) * dz.ritardo, id, bid, -1, C::aereo_nave, persi});
                if (persi) reazioni_ia_.push_back({tempo_, id, bid, "radio", "messaggio_perso", "disturbo_radio"});   // per lo spettatore
            }
            // il teatro: un rapporto per parte e bersaglio ogni riferire_ogni_s, o quando la classificazione sale
            auto& [tq, lq] = F.riferiti.try_emplace({-1 - lo, bid}, std::pair<double, int>{-1e18, -1}).first->second;
            if (lq < 0 || livello > lq || tempo_ - tq >= F.d.riferire_ogni_s) {
                tq = tempo_;
                lq = std::max(lq, livello);
                if (sub && F.quadro_sottomarini) {   // il battello che riferisce trasmette, e si fa localizzare come il giocatore
                    auto& R = F.radio_sottomarini[id];
                    if (tempo_ - R.ultima_trasmissione >= TRASMISSIONE_OGNI_S) {
                        R.ultima_trasmissione = tempo_;
                        radiogoniometria_ia(id, RAPPORTO_UHF_S, true);
                    }
                }
                const bool flash = livello >= 1 && r.categoria == "sottomarino";
                const double arrivo = tempo_ + ritardo(flash ? ch.flash_s : ch.immediate_s) * dz.ritardo;
                bool perso = F.caso.uniforme() < F.d.teatro_perdita;
                if (!perso && disturbato()) {
                    perso = true;
                    reazioni_ia_.push_back({tempo_, id, bid, "radio", "messaggio_perso", "disturbo_radio"});
                }
                F.messaggi.push_back({tempo_, arrivo, id, bid, -1, MessaggioIa::Canale::teatro, perso});
                if (!perso) F.in_viaggio.push_back({arrivo, -2, lo, r});
            }
        }
    }
    // le consegne arrivate
    std::vector<StatoForze::Consegna> nuove;
    for (auto it = F.in_viaggio.begin(); it != F.in_viaggio.end();) {
        if (it->t > tempo_) { ++it; continue; }
        const StatoForze::Consegna x = *it;
        it = F.in_viaggio.erase(it);
        if (x.ricevente >= 0 && !x.broadcast) { consegna_rapporto(x.ricevente, x.r); continue; }
        if (x.ricevente >= 0) {   // un sottomarino al broadcast: riceve solo esposto, se no al ciclo dopo (per un'ora)
            const auto* v = trova(x.ricevente);
            if (!v || v->distrutta || v->persa) continue;
            if (copia(x.ricevente, *v)) consegna_rapporto(x.ricevente, x.r);
            else if (tempo_ - x.r.t_fix < 3600.0) nuove.push_back({tempo_ + F.ciclo_s[x.lato], x.ricevente, x.lato, x.r, true});
            continue;
        }
        // il teatro: a tutte le unita' di superficie e agli aerei della parte, ai sottomarini al prossimo broadcast, al
        // giocatore della parte dalla sua sala radio (contatto d'intelligence, coi suoi limiti di ricezione)
        const double ciclo = F.ciclo_s[x.lato];
        const double prossimo = (std::floor(tempo_ / ciclo) + 1.0) * ciclo;
        bool sottomarini = false;
        for (const auto& [k, v] : unita_) {
            if (k == x.r.da || v.giocatore || v.distrutta || v.persa || arma_o_boa(v) || lato_di(v) != x.lato) continue;
            if (v.categoria == "sottomarino") {
                nuove.push_back({prossimo, k, x.lato, x.r, true});
                sottomarini = true;
            } else {
                consegna_rapporto(k, x.r);
            }
        }
        if (sottomarini) F.messaggi.push_back({tempo_, prossimo, x.r.da, x.r.su, -1, MessaggioIa::Canale::broadcast, false});
        if (g && !g->distrutta && !g->persa && radio_ && lato_di(*g) == x.lato && x.r.pos && x.r.su != g->id) {
            MessaggioRadio m;
            const bool flash = x.r.livello >= 1 && x.r.categoria == "sottomarino";
            m.precedenza = flash ? Precedenza::flash : Precedenza::immediate;
            m.da = x.lato == 0x08 ? "OTC" : "KP";
            m.oggetto = "CONTACT REPORT";
            m.classifica = "SECRET";
            const std::string cat_it = x.r.categoria == "sottomarino" ? "sottomarino" : x.r.categoria == "nave" ? "nave" : x.r.categoria == "aereo" ? "aereo" : "contatto";
            const std::string cat_en = x.r.categoria == "sottomarino" ? "submarine" : x.r.categoria == "nave" ? "surface" : x.r.categoria == "aereo" ? "air" : "unknown";
            m.testo = {"Rapporto di contatto della forza: " + cat_it + (x.r.classe.empty() ? "" : " (" + x.r.classe + ")") + ".",
                       "Force contact report: " + cat_en + (x.r.classe.empty() ? "" : " (" + x.r.classe + ")") + "."};
            RichiestaIntelligence q;
            q.unita = {x.r.su};
            q.eta_s = std::max(0.0, tempo_ - x.r.t_fix);
            q.errore_m = errore_rapporto(x.r, F.d);
            q.sensore = "forza";
            q.rotta_err_deg = 180.0;
            q.velocita_err_kn = 10.0;
            m.richiesta = q;
            const int serie = radio_->programma(m, tempo_);
            F.rapporti_giocatore[serie] = F.per_giocatore.size();
            F.per_giocatore.push_back(x.r);
            F.messaggi.push_back({tempo_, tempo_, x.r.da, x.r.su, g->id, MessaggioIa::Canale::giocatore, false});
        }
    }
    F.in_viaggio.insert(F.in_viaggio.end(), nuove.begin(), nuove.end());
}

std::optional<DatiIntelligence> Missione::intelligence_della_forza(int serie) const {
    if (!forze_) return std::nullopt;
    const auto it = forze_->rapporti_giocatore.find(serie);
    if (it == forze_->rapporti_giocatore.end()) return std::nullopt;
    const RapportoContatto& r = forze_->per_giocatore[it->second];
    DatiIntelligence d;
    d.unita = {r.su};
    d.pos = r.pos.value_or(r.origine);
    d.t_oss = r.t_fix;
    d.errore_m = errore_rapporto(r, forze_->d);
    d.rotta_err_deg = 180.0;
    d.velocita_err_kn = 10.0;
    d.sensore = "forza";
    d.m_s_per_nodo = m_s_per_nodo_;
    return d;
}

void Missione::passo_forze() {
    auto& F = *forze_;
    // l'ordine dello script chiude la caccia sul dato nello stesso passo: la sua velocita' e la sua rotta restano
    if (F.quadro_sottomarini)
        for (auto& [id, R] : F.radio_sottomarini) {
            if (R.caccia < 0) continue;
            const auto os = ordini_script_.find(id);
            if (os == ordini_script_.end() || os->second < tempo_) continue;
            if (const auto* u = trova(id)) reazioni_ia_.push_back({tempo_, id, R.caccia, u->categoria, "dato_radio_lasciato", "script"});
            R.caccia = -1;
            R.caccia_non_prima = tempo_ + CACCIA_DOPO_SCRIPT_S;
        }
    if (tempo_ >= F.prossimo_giro) {   // i messaggi di contatto, una volta al secondo
        F.prossimo_giro = tempo_ + 1.0;
        if (F.quadro_sottomarini) passo_radio_sottomarini();
        passo_messaggi();
    }
    // gli eventi del registro: atti ostili (provocazioni ed eventi d'arma), affondamenti, classificazioni
    const auto& ev = registro_.eventi();
    // `lato_chi`: la parte creduta di chi ha agito, quando non e' quella vera (la false flag della D4)
    const auto provoca = [&](int chi, int su, const char* causa, std::optional<int> lato_chi = std::nullopt) {
        const auto* a = trova(chi);
        const auto* b = trova(su);
        if ((!a && !lato_chi) || !b) return;
        const int la = lato_chi ? *lato_chi : lato_di(*a), lb = lato_di(*b);
        if (la == lb) {   // fuoco amico (fase C): un'IA sulla sua parte
            if (a && !a->giocatore && !lato_chi) F.fuoco_amico.push_back({tempo_, chi, su, causa});
            return;
        }
        F.attaccata[lb] = tempo_;
        if (situazione_fra(la, lb) == Situazione::crisi || (la == 0 || lb == 0)) {
            if (situazione_fra(la, lb) != Situazione::guerra) {
                F.coppie[coppia(la, lb)] = Situazione::guerra;
                F.provocazioni.push_back({tempo_, lb, la, chi, su, causa});
                for (const int l : {la, lb}) {   // la guerra: almeno A2 per le due parti
                    auto& f = F.forza[l];
                    if (f.v >= F.d.minimo_guerra) continue;
                    F.cambi.push_back({tempo_, l, -1, f.v, F.d.minimo_guerra, "situazione"});
                    f.v = F.d.minimo_guerra;
                    f.ultimo = tempo_;
                }
            }
        }
    };
    // la classificazione che alza l'allerta: un sottomarino (la classe detta e' d'un sottomarino della missione, o
    // senza classe la confidenza), creduto della parte ostile o d'una parte ignota; un amico, un neutrale o una nave no.
    // La parte del luogo e' ostile solo a chi e' con lei in crisi o in guerra (come ostili)
    const auto minaccia_classificata = [&](int chi, int classificato) {
        const auto* o = trova(chi);
        const auto* b = trova(classificato);
        if (!o || !b) return false;
        bool sottomarino = b->categoria == "sottomarino";
        if (const auto si = sonaristi_ia_.find(chi); si != sonaristi_ia_.end())
            if (const TracciaSonar* t = si->second.s->traccia_di(classificato); t && !t->classe.empty()) {
                if (t->classe == "biologico") return false;
                for (const auto& [k, v] : unita_)
                    if (v.classe == t->classe) { sottomarino = v.categoria == "sottomarino"; break; }
            }
        if (!sottomarino) return false;
        const int lo = lato_di(*o), lb = lato_creduto(*o, *b);
        // i neutrali, i civili e i non combattenti (flag 40h: il cargo appoggio della S08) non vanno in combattimento
        if (lo == 0 || (o->flag & 0x40) || classe_civile(o->classe)) return false;
        if (lb < 0) return true;   // di nessuno o ignoto
        if (lb == lo || lb == 0) return false;
        if (lo == 0x18 || lb == 0x18) return situazione_fra(lo, lb) != Situazione::pace;
        return true;
    };
    // D4, la false flag (docs/28 §3.5): un'arma senza lanciatore (il siluro «di nessuno» della D3) o travestita da un
    // intervento false_flag non provoca chi l'ha lanciata davvero: la parte colpita deduce il colpevole dai suoi sensori
    // (colpevole_creduto) e la provocazione segue la deduzione. Gli altri atti come prima (la parte vera di chi agisce)
    // l'arma del colpo e' gia' tolta (persa): si legge dalla mappa, non con trova
    const auto in_mappa = [&](int id) -> const UnitaClassica* { const auto it = unita_.find(id); return it == unita_.end() ? nullptr : &it->second; };
    // solo le armi nate da un intervento: il siluro «di nessuno» (incidente) e quello della false_flag. Le mine e le altre
    // armi senza lanciatore delle missioni restano come prima (S06: la mina del posamine non porta la guerra da se')
    const auto travestita = [&](int arma, int lanciatore) {
        if (interventi_.empty()) return false;
        const auto* w = arma >= 0 ? in_mappa(arma) : nullptr;
        if (!w || !(w->flag & 0x20) || da_mina_.count(arma)) return false;
        for (std::size_t k = 0; k < interventi_.size() && k < stati_interventi_.size(); ++k) {
            const Intervento& x = interventi_[k];
            if (!stati_interventi_[k].applicato) continue;
            if (lanciatore < 0 && x.tipo == "incidente" && x.testi.count("incidente") && x.testi.at("incidente") == "siluro" &&
                w->lanciatore < 0 && w->categoria == "arma")
                return true;
            if (lanciatore >= 0 && x.tipo == "false_flag" && x.unita == lanciatore) {
                const auto* l = in_mappa(lanciatore);
                return l && lato_di(*l) != lato_di(*w);
            }
        }
        return false;
    };
    // D4: chi ha cambiato parte e attacca i suoi di prima si tradisce: la vittima lo sa subito, la sua parte dopo il
    // rapporto della formazione. Solo con un cambio di parte: senza, nessun numero casuale in piu'
    const auto tradito = [&](int chi, int vittima) {
        const auto c = F.cambi_parte.find(chi);
        const auto* v = in_mappa(vittima);
        const auto* l = in_mappa(chi);
        if (c == F.cambi_parte.end() || !v || !l || vittima == chi || parte_nota(*v, *l) == lato_di(*l)) return;
        c->second.sanno.insert(vittima);
        const int lv = lato_di(*v);
        const auto ch = F.d.canali.find(lv);
        const auto [da, a] = ch != F.d.canali.end() ? ch->second.formazione_s : std::pair<double, double>{30.0, 120.0};
        c->second.sapra.try_emplace(lv, tempo_ + da + (a - da) * F.caso.uniforme());
        reazioni_ia_.push_back({tempo_, vittima, chi, v->categoria, "cambio_saputo", "cambia_parte"});
    };
    // l'istante in cui lo sa tutta la parte, per lo spettatore. Una volta detto, `sapra` resta negativo: per parte_nota e'
    // comunque passato, e con try_emplace nessun altro lo riscrive
    for (auto& [chi, c] : F.cambi_parte)
        for (auto& [lato, quando] : c.sapra) {
            if (quando < 0.0 || tempo_ < quando) continue;
            quando = -1.0;
            if (lato == 0x08 || lato == 0x10)
                reazioni_ia_.push_back({tempo_, chi, -1, lato == 0x08 ? "usa" : "urss", lato == 0x08 ? "cambio_saputo_usa" : "cambio_saputo_urss", "cambia_parte"});
        }
    for (; F.eventi_visti < ev.size(); ++F.eventi_visti) {
        const auto& e = ev[F.eventi_visti];
        if (!F.cambi_parte.empty()) {
            if (e.tipo == TipoEvento::colpo && e.altra >= 0) tradito(e.altra, e.unita);
            if ((e.tipo == TipoEvento::lancio_ia || e.tipo == TipoEvento::lancio_giocatore) && e.altra >= 0) tradito(e.unita, e.altra);
        }
        switch (e.tipo) {
            case TipoEvento::lancio_giocatore:
            case TipoEvento::lancio_ia:
                if (travestita(e.arma, e.unita)) {   // il lancio travestito: si saprà (o si crederà) solo dal colpo
                    evento_allerta(e.unita, "arma");
                    break;
                }
                if (e.altra >= 0) provoca(e.unita, e.altra, "lancio");
                else if (const auto* l = trova(e.unita); l && lato_di(*l) != 0)   // senza bersaglio: contro l'altra parte
                    for (const auto& [k, v] : unita_)
                        if (!v.distrutta && !v.persa && !arma_o_boa(v) && lato_di(v) != 0 && lato_di(v) != lato_di(*l)) {
                            provoca(e.unita, k, "lancio");
                            break;
                        }
                // chi lancia e chi gli sta intorno: siluro in acqua (anche i bersagli, se sentono)
                evento_allerta(e.unita, "arma");
                break;
            case TipoEvento::colpo:
                if (travestita(e.arma, e.altra)) {   // la parte colpita deduce il colpevole; la reazione segue la deduzione
                    const auto* v = in_mappa(e.unita);   // anche affondata: la sua parte reagisce
                    if (v) {
                        const int lv = lato_di(*v);
                        const int creduto = colpevole_creduto(lv, e.arma, e.altra, e.pos);
                        const char* nomi[] = {"neutrale", "usa", "urss", "luogo"};
                        reazioni_ia_.push_back({tempo_, e.unita, e.altra, nomi[lv >> 3],
                                                creduto < 0 ? "attribuisce_ignoto" : std::string("attribuisce_") + nomi[creduto >> 3], "false_flag"});
                        if (creduto >= 0) provoca(e.altra, e.unita, "colpo", creduto);
                    }
                } else if (e.altra >= 0) provoca(e.altra, e.unita, "colpo");
                evento_allerta(e.unita, "arma");
                if (e.altra >= 0) evento_allerta(e.altra, "arma");
                break;
            case TipoEvento::affondamento:
                // gli amici dell'affondato: la sua formazione (le unita' vicine della sua parte) e la forza
                for (const auto& [k, v] : unita_)
                    if (k != e.unita && !v.distrutta && !v.persa && !v.giocatore && !arma_o_boa(v) && distanza(v.pos, e.pos) <= F.d.raggio_formazione_m) {
                        if (const auto* m = trova(e.unita); m && lato_di(*m) == lato_di(v)) { evento_allerta(k, "affondato"); break; }
                    }
                break;
            case TipoEvento::classificazione_ia:
                // solo un sottomarino creduto ostile o di parte ignota (la principale, 30/10/2026: «Perry classifica
                // Perry» portava la formazione ad A3 al secondo 0 in 14 missioni della libreria)
                if (!minaccia_classificata(e.unita, e.altra)) break;
                if (e.livello >= static_cast<int>(Confidenza::probsub) && F.classificazioni.insert({e.unita, e.altra}).second)
                    evento_allerta(e.unita, "probsub");
                else if (e.livello == static_cast<int>(Confidenza::possub))
                    evento_allerta(e.unita, "possub");
                break;
            default:
                break;
        }
    }
    // l'artiglieria: i colpi caduti sul bersaglio (gli avvertimenti non provocano)
    for (; F.impatti_visti < impatti_artiglieria_.size(); ++F.impatti_visti) {
        const auto& i = impatti_artiglieria_[F.impatti_visti];
        if (i.avvertimento || i.bersaglio < 0) continue;
        provoca(i.tiratore, i.bersaglio, "salva");
        evento_allerta(i.tiratore, "arma");
        evento_allerta(i.bersaglio, "arma");
    }
    // il FLASH «avviso radio» della missione (24 §8.1, evento `flash`): il messaggio FLASH del comando alla parte del
    // giocatore lo riceve anche l'IA della parte, alla prima messa in onda (le navi e gli aerei in ascolto continuo,
    // e per semplicita' anche i sottomarini: STIMA). I rapporti di contatto della forza al giocatore ne restano fuori
    if (radio_)
        if (const auto* g = giocatore(); g && !g->distrutta && !g->persa)
            if (const auto o = radio_->in_onda(); o && o->precedenza == Precedenza::flash && o->ripetizione == 1 &&
                                                  !F.rapporti_giocatore.count(o->serie) && F.flash_visti.insert(o->serie).second)
                F.in_arrivo.push_back({tempo_, lato_di(*g), "flash"});
    // gli eventi che la forza sa ora (il FLASH dalla formazione al teatro)
    for (auto it = F.in_arrivo.begin(); it != F.in_arrivo.end();) {
        if (it->t > tempo_) { ++it; continue; }
        const auto ev_d = F.d.eventi.find(it->evento);
        if (ev_d != F.d.eventi.end()) {
            auto& f = F.forza[it->lato];
            int n = std::max(f.v, std::min(f.v + ev_d->second.forza, ev_d->second.forza_max));
            if (ev_d->second.forza_fino_a >= 0) n = std::max(n, ev_d->second.forza_fino_a);
            n = std::min(n, 3);
            if (n != f.v) F.cambi.push_back({tempo_, it->lato, -1, f.v, n, it->evento});
            f.v = n;
            f.ultimo = tempo_;
        }
        it = F.in_arrivo.erase(it);
    }
    // la discesa, fino al valore iniziale e mai sotto il minimo della situazione
    const auto minimo_di = [&](int lato) {
        const Situazione sit = lato == 0 ? std::max(situazione_fra(0, 0x08), situazione_fra(0, 0x10))
                                         : situazione_fra(lato, lato == 0x08 ? 0x10 : 0x08);
        return sit == Situazione::guerra ? F.d.minimo_guerra : sit == Situazione::crisi ? F.d.minimo_crisi : 0;
    };
    for (auto& [lato, f] : F.forza)
        if (f.v > std::max(f.iniziale, minimo_di(lato)) && tempo_ - f.ultimo >= F.d.discesa_s) {
            F.cambi.push_back({tempo_, lato, -1, f.v, f.v - 1, "discesa"});
            --f.v;
            f.ultimo = tempo_;
        }
    for (auto& [id, l] : F.unita) {
        const auto* u = trova(id);
        if (!u || u->distrutta || u->persa) continue;
        if (l.v > std::max(l.iniziale, minimo_di(lato_di(*u))) && tempo_ - l.ultimo >= F.d.discesa_s) {
            --l.v;
            l.ultimo = tempo_;
        }
    }
    // i falsi contatti dei sonaristi dell'IA crescono con l'allerta (x1 / 1,5 / 2 / 3 sul biologico preso per
    // sottomarino), dal profilo di partenza: l'allerta che scende lo riporta li'
    for (auto& [id, si] : sonaristi_ia_) {
        const auto a = allerta_unita(id);
        if (!a) continue;
        auto [it, nuovo] = F.profili.try_emplace(id, std::make_pair(si.s->profilo(), -1));
        if (it->second.second == *a) continue;
        it->second.second = *a;
        ProfiloSonarista p = it->second.first;
        p.biologico_come_sub = std::min(0.9, p.biologico_come_sub * F.d.falsi_contatti[static_cast<std::size_t>(std::clamp(*a, 0, 3))]);
        si.s->profilo(p);
    }
    // gli effetti gia' nel motore: le navi al posto di combattimento dell'artiglieria e l'EMCON di combattimento
    for (const auto& [id, u] : unita_) {
        if (u.distrutta || u.persa || u.giocatore || arma_o_boa(u)) continue;
        const int a = *allerta_unita(id);
        if (artiglieria_ && u.categoria == "nave" && a >= F.d.posto_combattimento && F.combattimento.insert(id).second) posto_di_combattimento(id);
        if (a >= F.d.emcon_combattimento && !F.emcon.count(id))
            if (const auto em = emcon_.find(id); em != emcon_.end()) {
                em->second.evento(EventoEmcon::scoperti, tempo_);
                F.emcon.insert(id);
            }
    }
}

// --- D3, il falso ordine radio (docs/28 §3.4; nota runtime-d3-falsi-ordini-proposta.md) ---
//
// L'ordine arriva come le ROE vere del vetrino: navi e aerei sulla rete dopo FALSO_RETE_S, i sottomarini al broadcast
// esposti, coi ritardi e le perdite del disturbo radio. Alla ricezione l'unita' dubita secondo il suo livello (l'utente,
// 31/10: «secondo il livello»), di piu' se l'ordine e' poco credibile o va contro la situazione. Chi dubita puo' chiedere
// conferma: e' una trasmissione, che le altre parti possono localizzare (radiogoniometria_ia); la risposta del comando
// smaschera l'ordine. Chi ci crede lo esegue finche' l'intervento dura. Registro: reazione_ia con evento «ordine_falso».

namespace {

constexpr double P_DUBBIO[] = {0.1, 0.3, 0.6};    // recluta, medio, esperto (STIMA)
constexpr double P_CHIEDE[] = {0.3, 0.6, 0.9};    // chi dubita chiede conferma (STIMA)
constexpr double FALSO_RETE_S = 60.0;             // come le ROE vere sulla rete della parte (missione_interventi.cpp)
constexpr double RISPOSTA_MAX_S = 600.0;          // senza risposta del comando entro 10 minuti: non esegue
constexpr double VERIFICA_S = 30.0;               // la richiesta di conferma in onda (STIMA)
constexpr double RIENTRO_KN = 15.0, RIENTRO_M = 30000.0;
constexpr double DURATA_SENZA_FINE_S = 1.0e7;     // l'ordine senza durata: per tutta la partita
constexpr double ORIZZONTE_DF_M = 50000.0;        // l'ESM nemico entro l'orizzonte radio (ORIZZONTE_ESM_M per il giocatore, missione.cpp)
constexpr double RISERVA_Y_M = 1.0e6;             // oltre: una riserva non ancora in scena (parcheggio, scenario_missione.cpp, a piu' di 2000 km; le scene arrivano a 165 km)

int lato_della(const std::string& p) { return p == "usa" ? 0x08 : p == "urss" ? 0x10 : 0; }

}  // namespace

void Missione::passo_ordini_falsi() {
    auto& F = *forze_;
    using E = StatoForze::OrdineFalsoRicevuto::Esito;
    const auto nome_esito = [](E e) -> const char* {   // la reazione nel registro dello spettatore
        switch (e) {
            case E::creduto: return "ordine_creduto";
            case E::ignorato: return "ordine_ignorato";
            case E::verifica: return "ordine_verifica";
            case E::smascherato: return "ordine_smascherato";
            case E::perso: return "ordine_perso";
        }
        return "ordine";
    };
    for (std::size_t k = 0; k < interventi_.size() && k < stati_interventi_.size(); ++k) {
        const Intervento& x = interventi_[k];
        const StatoIntervento& st = stati_interventi_[k];
        if (x.tipo != "ordine_falso" || !st.applicato) continue;
        const auto t = [&](const char* n) { const auto it = x.testi.find(n); return it == x.testi.end() ? std::string() : it->second; };
        const int lato = lato_della(t("parte"));
        const std::string ordine = t("ordine");
        const auto pa = x.parametri.find("autenticazione");
        const double autenticazione = pa == x.parametri.end() ? 0.5 : std::clamp(pa->second, 0.0, 1.0);
        const int altra = lato == 0x08 ? 0x10 : 0x08;
        for (auto& [id, u] : unita_) {
            if (u.giocatore || u.distrutta || u.persa || arma_o_boa(u) || lato_di(u) != lato || u.pos.y > RISERVA_Y_M) continue;
            const auto it = F.ordini_falsi.find({x.id, id});
            if (it != F.ordini_falsi.end()) {   // la verifica in corso: la risposta, o niente
                auto& o = it->second;
                if (o.esito != E::verifica) continue;
                if (o.risposta >= 0.0 && tempo_ >= o.risposta) o.esito = E::smascherato;
                else if (tempo_ - o.t >= RISPOSTA_MAX_S) o.esito = E::ignorato;
                else continue;
                reazioni_ia_.push_back({tempo_, id, -1, u.categoria, nome_esito(o.esito), "ordine_falso"});
                continue;
            }
            if (st.finito) continue;
            const bool sub = u.categoria == "sottomarino";
            const DisturboRadio dz = disturbo_da(lato, u.pos);
            const bool rete = !sub && tempo_ - st.inizio >= FALSO_RETE_S * dz.ritardo;
            const bool broadcast = sub && (F.quadro_sottomarini ? a_quota_antenna(u) && !sotto_banchisa(id) : (u.flag54 & 4) || u.quota <= QUOTA_ANTENNA_M);
            if (!rete && !broadcast) continue;
            StatoForze::OrdineFalsoRicevuto o;
            o.t = tempo_;
            if (dz.perdita > 0.0 && F.caso.uniforme() < dz.perdita) {   // perso nel disturbo: non arriva mai
                o.esito = E::perso;
                F.ordini_falsi[{x.id, id}] = o;
                continue;
            }
            const int livello = static_cast<int>(esperienza_di_ia(u));
            double p = P_DUBBIO[livello] * (1.5 - autenticazione);
            // contro la situazione: le armi libere in pace, il rientro in guerra
            const Situazione sit = situazione_di(lato, altra);
            if ((ordine == "roe_libere" && sit == Situazione::pace) || (ordine == "rientro" && sit == Situazione::guerra)) p *= 2.0;
            p = std::min(p, 1.0);
            if (F.caso.uniforme() >= p) {
                o.esito = E::creduto;
            } else if (F.caso.uniforme() < P_CHIEDE[livello]) {   // chiede conferma: trasmette, e la risposta smaschera
                o.esito = E::verifica;
                radiogoniometria_ia(id, VERIFICA_S);
                const auto ch = F.d.canali.find(lato);
                const auto [da, a] = ch != F.d.canali.end() ? ch->second.formazione_s : std::pair<double, double>{30.0, 120.0};
                const double ritardo = (da + (a - da) * F.caso.uniforme()) * dz.ritardo;
                o.risposta = dz.perdita > 0.0 && F.caso.uniforme() < dz.perdita ? -1.0 : tempo_ + ritardo;
            } else {
                o.esito = E::ignorato;
            }
            F.ordini_falsi[{x.id, id}] = o;
            reazioni_ia_.push_back({tempo_, id, -1, u.categoria, nome_esito(o.esito), "ordine_falso"});
            if (o.esito != E::creduto) continue;
            // l'esecuzione: il rientro e il «vai a» con la manovra dell'IA per la durata che resta; le ROE le legge
            // armi_libere_ia (roe_ordine_falso)
            const double resta = x.durata_s > 0.0 ? std::max(1.0, st.inizio + x.durata_s - tempo_) : DURATA_SENZA_FINE_S;
            const double kn = std::min(RIENTRO_KN, u.avanti_tutta * 280.0 / 256.0);
            if (ordine == "vai_a" && x.punto) {
                manovra_difesa(id, *x.punto, kn, resta);
            } else if (ordine == "rientro") {
                // via dal nemico piu' vicino che conosce; senza, indietro sulla sua rotta
                std::optional<Vec2> nemico;
                double vicino = 1e18;
                for (const auto& [bid, c] : u.contatti) {
                    const auto* b = trova(bid);
                    if (!b || b->distrutta || b->persa || arma_o_boa(*b)) continue;
                    const int lb = lato_creduto(u, *b);
                    if (lb < 0 || lb == lato || lb == 0) continue;
                    const Vec2 q = c.posizione_nota ? c.pos_stimata
                                                    : Vec2{u.pos.x + 8000.0 * std::sin(radianti(c.rilevamento)), u.pos.y + 8000.0 * std::cos(radianti(c.rilevamento))};
                    if (const double d = distanza(u.pos, q); d < vicino) { vicino = d; nemico = q; }
                }
                double via = nemico ? std::atan2(u.pos.x - nemico->x, u.pos.y - nemico->y) : radianti(u.rotta + 180.0);
                manovra_difesa(id, {u.pos.x + RIENTRO_M * std::sin(via), u.pos.y + RIENTRO_M * std::cos(via)}, kn, resta);
            }
        }
    }
}

std::optional<bool> Missione::roe_ordine_falso(int id) const {
    if (!forze_ || interventi_.empty()) return std::nullopt;
    std::optional<bool> r;
    for (std::size_t k = 0; k < interventi_.size() && k < stati_interventi_.size(); ++k) {   // l'ultimo creduto vince
        const Intervento& x = interventi_[k];
        if (x.tipo != "ordine_falso" || !stati_interventi_[k].applicato || stati_interventi_[k].finito) continue;
        const auto it = forze_->ordini_falsi.find({x.id, id});
        if (it == forze_->ordini_falsi.end() || it->second.esito != StatoForze::OrdineFalsoRicevuto::Esito::creduto) continue;
        const auto o = x.testi.find("ordine");
        if (o == x.testi.end()) continue;
        if (o->second == "roe_trattenute") r = false;
        else if (o->second == "roe_libere") r = true;
    }
    return r;
}

// La radiogoniometria delle altre parti su una trasmissione dell'IA, con la regola della trasmissione del giocatore
// (regola_df, nucleo/radio.hpp): l'URSS a raffica HF di 0,72 s, che la rete strategica avversaria coglie col fix; gli USA
// in UHF satellitare, che coglie solo un ESM nemico entro l'orizzonte radio, col solo rilevamento. Il fix va al teatro
// della parte nemica come un rapporto di contatto; il rilevamento all'unita' con l'ESM
void Missione::radiogoniometria_ia(int id, double durata_s, bool rapporto) {
    auto& F = *forze_;
    Casuale& caso = rapporto ? F.caso_radio : F.caso;   // il rapporto del battello non sposta il caso delle forze
    const auto* tx = trova(id);
    if (!tx) return;
    const int lt = lato_di(*tx);
    int esm = -1;
    double vicino = ORIZZONTE_DF_M;
    for (const auto& [k, u] : unita_)
        if (!u.giocatore && !u.distrutta && !u.persa && !arma_o_boa(u) && (u.flag & 6) && lato_di(u) != lt && ostili(u, *tx))
            if (const double d = distanza(u.pos, tx->pos); d < vicino) { vicino = d; esm = k; }
    ContestoDF ctx;
    ctx.esm_vicino = esm >= 0;
    const bool raffica = lt == 0x10;   // come la sala radio sovietica: 0,72 s in HF
    const RegolaDF r = regola_df(raffica ? Banda::hf : Banda::uhf, ctx);
    const double p = r.probabilita(raffica ? 0.72 : durata_s);
    const double errore = r.errore_m;
    const double ritardo = r.ritardo_min_s + (r.ritardo_extra_s > 0.0 ? r.ritardo_extra_s * caso.uniforme() : 0.0);
    if (caso.uniforme() >= p) return;
    if (errore <= 0.0) {   // l'ESM vicino: il rilevamento, senza la distanza
        auto& u = unita_.at(esm);
        Contatto* c = u.contatti.count(id) ? &u.contatti.at(id) : nuovo_contatto(u, id);
        if (!c) return;
        c->qualita = std::max(c->qualita, 400.0);
        c->lampeggio = 0;
        c->ultima_scoperta = tempo_;
        c->rilevamento = rilevamento(u.pos, tx->pos);
        c->sensore = "radiogoniometria";
        reazioni_ia_.push_back({tempo_, esm, id, u.categoria, "localizza", "radiogoniometria"});   // per lo spettatore
        return;
    }
    // il fix della rete strategica, con l'errore (Box-Muller sul caso delle forze), al teatro delle parti nemiche
    const double raggio = errore * std::sqrt(-2.0 * std::log(std::max(1e-12, caso.uniforme())));
    const double a = 2.0 * 3.14159265358979323846 * caso.uniforme();
    RapportoContatto rp;
    rp.da = -1;
    rp.su = id;
    rp.t_fix = tempo_;
    rp.pos = Vec2{tx->pos.x + raggio * std::sin(a), tx->pos.y + raggio * std::cos(a)};
    rp.origine = *rp.pos;
    rp.categoria = "sconosciuto";
    if (F.quadro_sottomarini) rp.errore_m = errore;
    for (const int lato : {0x08, 0x10})
        if (lato != lt) F.in_viaggio.push_back({tempo_ + ritardo, -2, lato, rp});
    reazioni_ia_.push_back({tempo_, -1, id, "rete", "localizza", "radiogoniometria"});   // la rete strategica: unita' -1
}

// D4, chi crede colpevole la parte `lato` di un atto con l'arma `arma` del lanciatore vero `lanciatore` (-1: nessuno) nel
// punto p, da quello che sanno le sue unita' (STIMA di gioco sul docs/28 §3.5):
// 1. chi ha lanciato, identificato da poco (classificato per classe, o visto): la parte che gli crede (anche la propria:
//    allora e' fuoco amico, e nessuna guerra);
// 2. la firma dell'arma: la sua parte (un siluro sovietico e' sovietico);
// 3. chi era vicino: il contatto non amico con la posizione piu' vicino al punto, entro 20 km (anche qui lato_creduto: un
//    contatto non classificato conta per la sua parte vera, il limite noto di E6 §2.4);
// 4. -1: ignoto, nessuna reazione politica.
int Missione::colpevole_creduto(int lato, int arma, int lanciatore, Vec2 p) const {
    constexpr double RECENTE_S = 120.0, VICINO_M = 20000.0, IDENTIFICATO = 1400.0;   // la durata della classe
    const auto trova = [&](int id) -> const UnitaClassica* { const auto it = unita_.find(id); return it == unita_.end() ? nullptr : &it->second; };
    const auto* l = lanciatore >= 0 ? trova(lanciatore) : nullptr;
    if (l)
        for (const auto& [id, u] : unita_) {
            if (u.distrutta || u.persa || arma_o_boa(u) || lato_di(u) != lato) continue;
            const auto c = u.contatti.find(lanciatore);
            if (c == u.contatti.end() || tempo_ - c->second.ultima_scoperta > RECENTE_S) continue;
            // solo un lanciatore identificato (classificato per classe, o visto: la vista da' 1400): un rilevamento sul
            // transitorio del lancio dice dove, non di chi (lato_creduto darebbe la parte vera)
            if (c->second.durata < IDENTIFICATO) continue;
            if (const int lc = lato_creduto(u, *l); lc >= 0) return lc;
        }
    if (const auto* w = arma >= 0 ? trova(arma) : nullptr; w && lato_di(*w) != 0) return lato_di(*w);
    int creduto = -1;
    double vicino = VICINO_M;
    for (const auto& [id, u] : unita_) {
        if (u.distrutta || u.persa || arma_o_boa(u) || lato_di(u) != lato) continue;
        for (const auto& [bid, c] : u.contatti) {
            const auto* b = trova(bid);
            if (!b || arma_o_boa(*b) || !c.posizione_nota) continue;
            const int lb = lato_creduto(u, *b);
            if (lb < 0 || lb == lato) continue;
            if (const double d = distanza(c.pos_stimata, p); d < vicino) { vicino = d; creduto = lb; }
        }
    }
    return creduto;
}

// D3, la spia (docs/28 §3.5): finche' dura, una parte riceve ogni SPIA_OGNI_S il quadro vero dell'altra, come rapporti di
// contatto con la posizione e la classe vere (consegna_rapporto: chi ha un contatto suo e fresco lo tiene; l'aereo
// dell'ASW ne fa un datum). Rompe la simmetria apposta: e' l'esperimento «quanto vale l'informazione». L'ultima consegna
// sta in ordini_falsi con la chiave (intervento, -1), per non toccare StatoForze
void Missione::passo_spia() {
    constexpr double SPIA_OGNI_S = 60.0;   // STIMA: un quadro al minuto
    auto& F = *forze_;
    for (std::size_t k = 0; k < interventi_.size() && k < stati_interventi_.size(); ++k) {
        const Intervento& x = interventi_[k];
        const StatoIntervento& st = stati_interventi_[k];
        if (x.tipo != "spia" || !st.applicato || st.finito) continue;
        auto& ultima = F.ordini_falsi[{x.id, -1}];
        if (ultima.t > 0.0 && tempo_ - ultima.t < SPIA_OGNI_S) continue;
        ultima.t = tempo_;
        const auto t = [&](const char* n) { const auto it = x.testi.find(n); return it == x.testi.end() ? std::string() : it->second; };
        const int riceve = lato_della(t("parte")), spiata = lato_della(t("altra"));
        if (riceve == spiata) continue;
        std::vector<RapportoContatto> quadro;
        for (const auto& [id, b] : unita_) {
            if (b.distrutta || b.persa || arma_o_boa(b) || lato_di(b) != spiata) continue;
            RapportoContatto r;
            r.da = -1;
            r.su = id;
            r.t_fix = tempo_;
            r.pos = b.pos;
            r.origine = b.pos;
            r.categoria = (b.flag & 3) == 3 || b.categoria == "aereo" ? "aereo" : b.categoria == "sottomarino" ? "sottomarino" : "nave";
            r.classe = b.classe;
            r.livello = 2;
            quadro.push_back(r);
        }
        for (const auto& [id, u] : unita_)
            if (!u.giocatore && !u.distrutta && !u.persa && !arma_o_boa(u) && lato_di(u) == riceve) {
                // il sottomarino dal canale: al ciclo della parte, a quota d'antenna, col disturbo della parte; per ciclo
                // e bersaglio resta l'ultimo quadro
                if (const double ciclo = F.ciclo_s[riceve]; F.quadro_sottomarini && u.categoria == "sottomarino" && ciclo > 0.0) {
                    const DisturboRadio dz = disturbo_da(riceve, u.pos);
                    const double arrivo = (std::floor((tempo_ + SPIA_OGNI_S * (dz.ritardo - 1.0)) / ciclo) + 1.0) * ciclo;
                    for (const auto& r : quadro) {
                        if (dz.perdita > 0.0 && F.caso_radio.uniforme() < dz.perdita) continue;
                        std::erase_if(F.in_viaggio, [&, id = id](const StatoForze::Consegna& c) { return c.broadcast && c.ricevente == id && c.r.da < 0 && c.r.su == r.su && c.t == arrivo; });
                        F.in_viaggio.push_back({arrivo, id, riceve, r, true});
                    }
                    continue;
                }
                for (const auto& r : quadro) consegna_rapporto(id, r);
            }
    }
}

// --- il quadro radio ai sottomarini dell'IA (tools/re/prove/quadro_radio/proposta.md, decisioni dell'utente del 05/11/2026) ---
//
// Il battello riceve il quadro con la regola del giocatore e degli ordini radio: a quota d'antenna, al ciclo della parte, dopo
// il ritardo del teatro. In crisi e in guerra sale apposta alle finestre, come il comandante IA, e paga l'esposizione dell'albero.
// Con le armi libere il dato orienta: va verso il punto riferito finche' il dato regge, e il lancio resta al contatto proprio
// (il dato radio non basta per un siluro). Restano fuori il filo galleggiante e il campanello ELF. Tutti i numeri sono STIMA.

namespace {

constexpr double ASCOLTO_OGNI_S = 1800.0, ASCOLTO_A3_OGNI_S = 3600.0;   // il broadcast: come il comandante IA; in A3 meno spesso
constexpr double ASCOLTO_PRIMA_S = 120.0, ASCOLTO_DOPO_S = 5.0;         // a quota prima del ciclo; giu' appena copiato
constexpr double ASCOLTO_MAX_S = 900.0;                                 // non ci arriva: rinuncia fino alla finestra dopo
constexpr double RATEO_SALITA_M_S = 0.25;                               // misurato in salita da 100 m: l'anticipo
constexpr double QUOTA_ORDINE_ANTENNA_M = 5.5, QUOTA_ALBERO_M = 7.096;  // l'ordine del comandante IA; gli alberi si ammainano oltre
constexpr double CONTATTO_FRESCO_S = 60.0, QUALITA_SOLIDA = 800.0;
constexpr double QUALUNQUE_M = 1e18;
// per esperienza (recluta, medio, esperto): rimanda col contatto ostile entro, la velocita' a quota e quella della caccia. La
// recluta sale piu' in fretta (oltre 10 nodi copia al 50%) e corre al punto. Il limite dell'ellisse e' uguale per tutti
constexpr double RIMANDA_M[] = {5000.0, 15000.0, QUALUNQUE_M};
constexpr double KN_A_QUOTA[] = {12.0, 10.0, 5.0};
constexpr double LIMITE_ELLISSE_M = 30000.0;
constexpr double KN_CACCIA[] = {15.0, 10.0, 8.0};
constexpr double ARRIVATO_M = 2000.0;   // sul punto riferito senza contatto: il dato e' finito

}  // namespace

void Missione::passo_radio_sottomarini() {
    auto& F = *forze_;
    for (auto& [id, u] : unita_) {
        if (u.giocatore || u.categoria != "sottomarino" || u.distrutta || u.persa || arma_o_boa(u) || u.pos.y > RISERVA_Y_M) continue;
        auto& R = F.radio_sottomarini[id];
        const bool esposto = a_quota_antenna(u) && !sotto_banchisa(id);
        if (!esposto) R.esposto_da = -1.0;
        else if (R.esposto_da < 0.0) R.esposto_da = tempo_;
        const int lu = lato_di(u);
        const auto livello = static_cast<std::size_t>(esperienza_di_ia(u));
        const int a = allerta_unita(id).value_or(0);
        // chi comanda il battello adesso, sopra l'ascolto e la caccia
        const auto os = ordini_script_.find(id);
        const bool script_ora = os != ordini_script_.end() && os->second >= tempo_;
        const auto rz = reazioni_in_corso_.find(id);
        const auto ml = missili_ia_.find(id);
        const auto tm = armi_ia_->tma.lower_bound({id, -1000000});
        bool ordine_vetrino = false;
        for (std::size_t k = 0; k < interventi_.size() && k < stati_interventi_.size() && !ordine_vetrino; ++k)
            ordine_vetrino = interventi_[k].tipo == "ordine" && interventi_[k].unita == id && stati_interventi_[k].applicato && !stati_interventi_[k].finito;
        const bool occupato = u.affondamento > 0 || manovre_difesa_.count(id) || (rz != reazioni_in_corso_.end() && tempo_ < rz->second.fine) ||
                              (tm != armi_ia_->tma.end() && tm->first.first == id) || u.agganciato >= 0 || (ml != missili_ia_.end() && ml->second.in_manovra) ||
                              pedine_vicine_.count(id) || ordine_vetrino || guida_evasione(u).has_value();
        const auto contatto_ostile = [&, &u = u](double entro_m) {
            for (const auto& [cid, c] : u.contatti) {
                const auto* b = trova(cid);
                if (!b || b->distrutta || b->persa || arma_o_boa(*b) || c.qualita < QUALITA_SOLIDA || tempo_ - c.ultima_scoperta > CONTATTO_FRESCO_S || !ostili(u, *b)) continue;
                if (entro_m >= QUALUNQUE_M || (c.posizione_nota && distanza(u.pos, c.pos_stimata) <= entro_m)) return true;
            }
            return false;
        };
        // la velocita' di prima, anche per chi ha gia' preso in mano il battello in questo passo
        const auto velocita_di_prima = [&, id = id, &u = u](double v) {
            u.velocita_ordinata = v;
            if (const auto d = manovre_difesa_.find(id); d != manovre_difesa_.end()) d->second.velocita_prima = v;
            if (const auto r = reazioni_in_corso_.find(id); r != reazioni_in_corso_.end()) r->second.velocita_prima = v;
            for (auto x = armi_ia_->tma.lower_bound({id, -1000000}); x != armi_ia_->tma.end() && x->first.first == id; ++x) x->second.velocita_prima = v;
            if (const auto n = missili_ia_.find(id); n != missili_ia_.end() && n->second.in_manovra) n->second.velocita_prima = v;
        };
        const auto fine_ascolto = [&, id = id, &u = u] {
            const bool quota_sua = std::fabs(u.quota_ordinata - QUOTA_ORDINE_ANTENNA_M) > 1e-9;   // lo script gliene ha data un'altra
            const bool velocita_sua = std::fabs(u.velocita_ordinata - R.velocita_data) > 1e-9;
            R.salita = false;
            if (R.antenna_nostra) u.flag54 &= ~4;
            R.antenna_nostra = false;
            if (!quota_sua) u.quota_ordinata = R.quota_prima;
            if (const auto n = missili_ia_.find(id); n != missili_ia_.end() && n->second.in_manovra) n->second.quota_prima = R.quota_prima;
            if (!velocita_sua) velocita_di_prima(R.velocita_prima);
        };
        const double ciclo = F.ciclo_s[lu];
        const double prossimo = ciclo > 0.0 ? (std::floor(tempo_ / ciclo) + 1.0) * ciclo : 1e18;
        if (R.salita) {
            if (script_ora || occupato || contatto_ostile(RIMANDA_M[livello]) || std::fabs(u.quota_ordinata - QUOTA_ORDINE_ANTENNA_M) > 1e-9 ||
                std::fabs(u.velocita_ordinata - R.velocita_data) > 1e-9) {
                fine_ascolto();
            } else if (!esposto && tempo_ - R.salita_da > ASCOLTO_MAX_S) {
                fine_ascolto();
                R.ultimo_ascolto = tempo_;
            } else {
                if (u.quota <= QUOTA_ALBERO_M && !(u.flag54 & 4)) {
                    u.flag54 |= 4;
                    R.antenna_nostra = true;
                }
                if (tempo_ >= R.ciclo + ASCOLTO_DOPO_S) {
                    if (R.esposto_da >= 0.0 && R.ciclo - R.esposto_da >= COPIA_S) {
                        fine_ascolto();
                        R.ultimo_ascolto = tempo_;
                    } else {
                        R.ciclo = R.ciclo_tentato = prossimo;   // arrivato tardi: il ciclo dopo
                    }
                }
            }
        } else if (esposto) {
            R.ultimo_ascolto = tempo_;   // e' gia' li' per conto suo
        } else if (const int altra = lu == 0x08 ? 0x10 : 0x08; lu != 0 && !(u.flag & 0x40) && !script_ora && !occupato && situazione_di(lu, altra) != Situazione::pace &&
                   prossimo != R.ciclo_tentato && prossimo >= R.ultimo_ascolto + (a >= 3 ? ASCOLTO_A3_OGNI_S : ASCOLTO_OGNI_S) &&
                   prossimo - tempo_ <= u.quota / RATEO_SALITA_M_S + ASCOLTO_PRIMA_S && !contatto_ostile(RIMANDA_M[livello]) && !sotto_banchisa(id)) {
            R.salita = true;
            R.salita_da = tempo_;
            R.ciclo = R.ciclo_tentato = prossimo;
            R.quota_prima = u.quota_ordinata;
            R.velocita_prima = u.velocita_ordinata;
            u.quota_ordinata = QUOTA_ORDINE_ANTENNA_M;
            u.velocita_ordinata = R.velocita_data = std::min(std::fabs(u.velocita_ordinata), KN_A_QUOTA[livello]);
            reazioni_ia_.push_back({tempo_, id, -1, u.categoria, "ascolto_broadcast", "quadro_radio"});
        }
        // il dato radio: orienta chi ha le armi libere e nessun contatto suo. Fresco sotto i 10 km, vecchio fino al limite (non in A3)
        const auto usabile = [&, id = id, &u = u](int rb, const ContattoRadio& cr) {
            const auto* b = trova(rb);
            if (!b || b->distrutta || b->persa || b->affondamento > 0 || arma_o_boa(*b) || (b->flag & 0x40) || !cr.r.pos) return false;
            if (cr.r.categoria == "aereo" || (b->flag & 3) == 3 || b->categoria == "aereo") return false;
            if (livello == static_cast<std::size_t>(Esperienza::esperto) && cr.r.categoria == "sconosciuto") return false;
            if (const int lb = lato_creduto(u, *b); lb <= 0 || lb == lu) return false;
            if (const auto* v = identificazione_visiva(id, rb); v && classe_civile(v->classe)) return false;
            if (const auto si = sonaristi_ia_.find(id); si != sonaristi_ia_.end())
                if (const TracciaSonar* tr = si->second.s->traccia_di(rb); tr && !tr->classe.empty() && classe_civile(tr->classe)) return false;
            if (tempo_ - cr.t_arrivo > RADIO.vita_intelligence_cicli * ciclo || cr.raggio_m > LIMITE_ELLISSE_M || (a >= 3 && !cr.posizione_valida)) return false;
            if (const auto l = R.lasciati.find(rb); l != R.lasciati.end() && cr.r.t_fix <= l->second) return false;
            return armi_libere_ia(id, rb).value_or(false);
        };
        const Vec2 e = errore_navigazione(id);
        const auto verso_di = [&](const ContattoRadio& cr) { return Vec2{cr.r.pos->x - e.x, cr.r.pos->y - e.y}; };   // nel mondo vero, dalla sua stima
        const auto lascia = [&, id = id, &u = u](const char* perche, bool rotta) {
            reazioni_ia_.push_back({tempo_, id, R.caccia, u.categoria, "dato_radio_lasciato", perche});
            R.caccia = -1;
            velocita_di_prima(R.velocita_prima_caccia);
            if (rotta && !u.navigazione) u.rotta_ordinata = R.rotta_prima_caccia;
        };
        const bool contatto_suo = contatto_ostile(QUALUNQUE_M);
        if (R.caccia >= 0) {
            const auto cr = contatto_radio(id, R.caccia);
            if (!cr || !usabile(R.caccia, *cr)) {
                lascia("scaduto", !occupato && !contatto_suo);
            } else if (occupato || contatto_suo) {
                lascia("contatto", false);
            } else {
                R.verso = verso_di(*cr);
                if (distanza(u.pos, R.verso) <= ARRIVATO_M) {
                    R.lasciati[R.caccia] = cr->r.t_fix;
                    lascia("a_vuoto", true);
                }
            }
        }
        if (R.caccia < 0 && !occupato && !contatto_suo && !R.salita && !script_ora && tempo_ >= R.caccia_non_prima) {
            double vicina = QUALUNQUE_M;
            for (const auto& [rb, cr] : quadro_radio(id)) {
                if (!usabile(rb, cr)) continue;
                const Vec2 v = verso_di(cr);
                const double d = distanza(u.pos, v);
                if (d <= ARRIVATO_M) {   // gia' li' e senza contatto: questo dato non serve
                    R.lasciati[rb] = cr.r.t_fix;
                } else if (d < vicina) {
                    vicina = d;
                    R.caccia = rb;
                    R.verso = v;
                }
            }
            if (R.caccia >= 0) {
                R.velocita_prima_caccia = u.velocita_ordinata;
                R.rotta_prima_caccia = u.rotta_ordinata;
                R.velocita_caccia = std::min(KN_CACCIA[livello], u.avanti_tutta);
                reazioni_ia_.push_back({tempo_, id, R.caccia, u.categoria, "dato_radio_usato", "quadro_radio"});
            }
        }
    }
}

}  // namespace m688
