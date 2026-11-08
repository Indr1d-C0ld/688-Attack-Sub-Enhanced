// ASW aereo dell'Enhanced nella missione (docs/13_asw_aereo.md, pezzo A3): i ControlloreAsw degli aerei, le boe e i
// sonar a immersione come ricevitori del motore acustico (nucleo/acustica_asw.hpp), i transitori, le armi col
// paracadute, i marcatori, gli avvisi 380-389 e il ponte verso gli script (la traccia del controllore diventa il
// contatto dell'aereo).

#include <algorithm>
#include <cmath>
#include <filesystem>

#include "missioni/asw_missione.hpp"
#include "missioni/interventi.hpp"
#include "missioni/missione.hpp"
#include "nucleo/acustica_asw.hpp"

extern "C" {
#include "lauxlib.h"
#include "lua.h"
}

namespace m688 {

namespace {

// il vetrino di Dio (docs/28, D1): il guasto del processore acustico di un aereo ASW, sulle boe che segue, sul sonar a
// immersione e sugli echi: 20 log10 del fattore del «sonar» dell'aereo, -1e9 se spento, 0 senza guasto
double attenuazione_asw(const Missione& m, int aereo) {
    if (m.interventi().empty()) return 0.0;
    const auto f = parametro_attivo(m.interventi(), m.stati_interventi(), aereo, "sensore", "fattore", "sonar");
    if (!f) return 0.0;
    return *f <= 0.0 ? -1e9 : 20.0 * std::log10(*f);
}

constexpr double DISTANZA_FALSI_M = 30000.0;      // navi e biologici entro tanto dall'aereo danno falsi allarmi
constexpr double QUOTA_AEREO_M = 300.0, QUOTA_ELICOTTERO_M = 150.0;   // senza controllore (radar ed ESM, vista)
constexpr double SALITA_ELICOTTERO_M_S = 5.0, SALITA_AEREO_M_S = 12.0;   // STIMA
constexpr double PLAB_AFFONDA_M_S = 3.5, PLAB_QUOTA_M = 100.0, PLAB_RAGGIO_M = 100.0;   // STIMA
constexpr double AVVISO_OGNI_S = 60.0;            // lo stesso avviso ASW al piu' una volta al minuto

double num(lua_State* L, int t, const char* k, double d) {
    lua_getfield(L, t, k);
    const double v = lua_isnumber(L, -1) ? lua_tonumber(L, -1) : d;
    lua_pop(L, 1);
    return v;
}
std::string testo(lua_State* L, int t, const char* k) {
    lua_getfield(L, t, k);
    std::string v = lua_isstring(L, -1) ? lua_tostring(L, -1) : "";
    lua_pop(L, 1);
    return v;
}
std::optional<Vec2> punto(lua_State* L, int t, const char* k, double* raggio = nullptr) {
    lua_getfield(L, t, k);
    std::optional<Vec2> v;
    if (lua_istable(L, -1)) {
        const int p = lua_gettop(L);
        v = Vec2{num(L, p, "x", 0.0), num(L, p, "y", 0.0)};
        if (raggio) *raggio = num(L, p, "raggio_m", *raggio);
    }
    lua_pop(L, 1);
    return v;
}
std::optional<SituazioneAsw> situazione_da(const std::string& s) {
    if (s == "pace" || s == "esercitazione") return SituazioneAsw::pace;
    if (s == "crisi" || s == "tensione" || s == "minaccia") return SituazioneAsw::crisi;
    if (s == "guerra") return SituazioneAsw::guerra;
    return std::nullopt;
}
Cue::Tipo tipo_cue(const std::string& s) {
    if (s == "nave_colpita") return Cue::Tipo::nave_colpita;
    if (s == "siluro_sentito") return Cue::Tipo::siluro_sentito;
    if (s == "radar_esm") return Cue::Tipo::radar_esm;
    if (s == "contatto_nave") return Cue::Tipo::contatto_nave;
    if (s == "immersione_vista") return Cue::Tipo::immersione_vista;
    if (s == "zona") return Cue::Tipo::zona;
    return Cue::Tipo::sosus;
}
Cue leggi_cue(lua_State* L, int t, double tempo) {
    Cue c;
    c.tipo = tipo_cue(testo(L, t, "tipo"));
    c.pos = {num(L, t, "x", 0.0), num(L, t, "y", 0.0)};
    c.raggio_m = num(L, t, "raggio_m", c.raggio_m);
    c.t = tempo - num(L, t, "ritardo_s", 0.0);
    lua_getfield(L, t, "rotta");
    if (lua_isnumber(L, -1)) c.rotta_deg = lua_tonumber(L, -1);
    lua_pop(L, 1);
    lua_getfield(L, t, "velocita_kn");
    if (lua_isnumber(L, -1)) c.velocita_kn = lua_tonumber(L, -1);
    lua_pop(L, 1);
    return c;
}

// la voce { unita = 6, missione = "ricognizione", ... } in cima alla pila
ConfigAereoAsw leggi_config(lua_State* L, int t, double tempo) {
    ConfigAereoAsw c;
    c.unita = static_cast<int>(num(L, t, "unita", -1.0));
    c.classe = testo(L, t, "classe");
    if (const std::string m = testo(L, t, "missione"); !m.empty()) c.missione = m;
    c.situazione = situazione_da(testo(L, t, "situazione"));
    lua_getfield(L, t, "bersagli");
    if (lua_isstring(L, -1) && std::string(lua_tostring(L, -1)) == "giocatore") c.bersaglio_giocatore = true;
    else if (lua_istable(L, -1))
        for (lua_Integer i = 1; lua_rawgeti(L, -1, i) == LUA_TNUMBER; ++i) {
            c.bersagli.push_back(static_cast<int>(lua_tointeger(L, -1)));
            lua_pop(L, 1);
        }
    if (lua_istable(L, -1)) lua_pop(L, 1);   // il valore che ha chiuso il ciclo
    lua_pop(L, 1);
    lua_getfield(L, t, "armi");
    if (lua_isboolean(L, -1)) c.armi = lua_toboolean(L, -1) != 0;
    lua_pop(L, 1);
    lua_getfield(L, t, "sensore_classico");
    c.sensore_classico = lua_toboolean(L, -1) != 0;
    lua_pop(L, 1);
    c.carico = testo(L, t, "carico");
    const std::string e = testo(L, t, "equipaggio");
    if (e == "esperto") c.equipaggio = LivelloEquipaggio::esperto;
    else if (e == "medio") c.equipaggio = LivelloEquipaggio::medio;
    else if (e == "scarso") c.equipaggio = LivelloEquipaggio::scarso;
    c.base = punto(L, t, "base");
    c.base_unita = static_cast<int>(num(L, t, "base_unita", -1.0));
    c.zona = punto(L, t, "zona", &c.raggio_zona_m);
    lua_getfield(L, t, "rotta_presunta");
    if (lua_isnumber(L, -1)) c.rotta_presunta = lua_tonumber(L, -1);
    lua_pop(L, 1);
    lua_getfield(L, t, "cue");
    if (lua_istable(L, -1)) {
        const int ct = lua_gettop(L);
        if (lua_rawgeti(L, ct, 1) == LUA_TTABLE) {   // un elenco di cue
            lua_pop(L, 1);
            for (lua_Integer i = 1; lua_rawgeti(L, ct, i) == LUA_TTABLE; ++i) {
                c.cue.push_back(leggi_cue(L, lua_gettop(L), tempo));
                lua_pop(L, 1);
            }
            lua_pop(L, 1);
        } else {
            lua_pop(L, 1);
            c.cue.push_back(leggi_cue(L, ct, tempo));
        }
    }
    lua_pop(L, 1);
    lua_getfield(L, t, "gruppo");
    if (lua_istable(L, -1)) {
        const int g = lua_gettop(L);
        c.capogruppo = static_cast<int>(num(L, g, "capo", -1.0));
        c.indice_gruppo = static_cast<int>(num(L, g, "indice", 0.0));
        c.numero_gruppo = static_cast<int>(num(L, g, "numero", 1.0));
    }
    lua_pop(L, 1);
    return c;
}

// l'elenco in cima alla pila
void leggi_configurazioni(lua_State* L, double tempo, std::vector<ConfigAereoAsw>& out) {
    if (!lua_istable(L, -1)) return;
    const int t = lua_gettop(L);
    for (lua_Integer i = 1; lua_rawgeti(L, t, i) == LUA_TTABLE; ++i) {
        out.push_back(leggi_config(L, lua_gettop(L), tempo));
        lua_pop(L, 1);
    }
    lua_pop(L, 1);
}

int lato_nazione(ParteAsw p) { return p == ParteAsw::usa ? 0x08 : 0x10; }

bool elicottero_noto(const std::string& classe) {
    return classe == "sh60b" || classe == "ka25" || classe == "sh3h" || classe == "sh60f" || classe == "ka27";
}

}  // namespace

// --------------------------------------------------------------------------------------------- avvio

void Missione::asw_enhanced(const std::string& cartella_dati, DifficoltaAsw difficolta) {
    if (asw_ || asw_spento_) return;
    auto s = std::make_unique<StatoAswMissione>();
    try {
        s->dati = carica_asw_aereo(cartella_dati + "/asw_aereo.toml");
    } catch (const std::exception& e) {
        errori_.push_back(e.what());   // senza i dati gli aerei restano quelli del classico
        asw_spento_ = true;
        return;
    }
    s->difficolta = difficolta_asw_.value_or(difficolta);
    s->caso = Casuale(seme_ * 0x9E3779B97F4A7C15ULL + 0xA5A0A5A1ULL);
    s->ascolto.passo_s = s->dati.pensiero_s;
    s->ascolto.distanza_max_m = 40000.0;
    s->configurazioni = configurazioni_asw_missione(L_, rif_missione_, file_missione_, id_, tempo_);
    asw_ = std::move(s);
}

// configurazione: la chiave della missione, altrimenti il file delle originali
std::vector<ConfigAereoAsw> configurazioni_asw_missione(lua_State* L, int rif_missione, const std::string& file_missione,
                                                        const std::string& id, double tempo) {
    std::vector<ConfigAereoAsw> v;
    const int base = lua_gettop(L);
    lua_rawgeti(L, LUA_REGISTRYINDEX, rif_missione);
    lua_getfield(L, -1, "aereo_asw");
    if (lua_istable(L, -1)) leggi_configurazioni(L, tempo, v);
    lua_settop(L, base);
    if (v.empty()) {
        const auto file = std::filesystem::path(file_missione).parent_path() / "asw" / (id + ".lua");
        if (std::filesystem::exists(file) && luaL_loadfilex(L, file.string().c_str(), "t") == LUA_OK && lua_pcall(L, 0, 1, 0) == LUA_OK)
            leggi_configurazioni(L, tempo, v);
        lua_settop(L, base);
    }
    return v;
}

std::vector<Missione::AereoAswPrevisto> Missione::aerei_asw_previsti(const std::string& cartella_dati) const {
    std::vector<AereoAswPrevisto> v;
    std::optional<DatiAsw> dati;
    if (!asw_) {
        try { dati = carica_asw_aereo(cartella_dati + "/asw_aereo.toml"); } catch (const std::exception&) {}
    }
    const DatiAsw* d = asw_ ? &asw_->dati : dati ? &*dati : nullptr;
    for (const auto& c : asw_ ? asw_->configurazioni : configurazioni_asw_missione(L_, rif_missione_, file_missione_, id_, tempo_)) {
        AereoAswPrevisto a;
        a.unita = c.unita;
        a.classe = c.classe;
        if (a.classe.empty())
            if (const auto it = unita_.find(c.unita); it != unita_.end()) a.classe = it->second.classe;
        a.missione = c.missione;
        if (d)
            if (const DatiMezzo* m = d->mezzo(a.classe)) a.parte = m->parte == ParteAsw::usa ? "usa" : "urss";
        a.base = c.base;
        a.base_unita = c.base_unita;
        a.zona = c.zona;
        a.raggio_zona_m = c.raggio_zona_m;
        v.push_back(std::move(a));
    }
    return v;
}

bool Missione::controllato_asw(int id) const { return asw_ && asw_->aerei.count(id) && asw_->aerei.at(id).attivo; }

bool Missione::aereo_ostile_al_giocatore(int id) const {
    const auto* g = giocatore();
    if (!asw_ || !g) return false;
    const auto it = asw_->aerei.find(id);
    if (it == asw_->aerei.end()) return false;
    return it->second.bersagli.count(g->id) > 0 || (g->flag & 0x18) != lato_nazione(it->second.c.mezzo()->parte);
}

const ControlloreAsw* Missione::controllore_asw(int id) const {
    if (!asw_) return nullptr;
    const auto it = asw_->aerei.find(id);
    return it == asw_->aerei.end() ? nullptr : &it->second.c;
}

std::vector<VoloAsw> Missione::voli_asw() const {
    std::vector<VoloAsw> v;
    if (!asw_) return v;
    for (const auto& [id, a] : asw_->aerei) {
        if (!a.attivo) continue;
        VoloAsw x;
        x.unita = id;
        x.classe = a.c.mezzo()->id;
        x.quota_m = a.quota_m;
        x.comando = a.c.volo();
        x.stato = a.c.stato();
        x.fase_dip = a.c.dip().fase();
        x.quota_trasduttore_m = a.c.dip().quota_trasduttore_m();
        const auto* u = unita_.count(id) ? &unita_.at(id) : nullptr;
        x.libero = x.comando.libero && u && u->navigazione;
        v.push_back(x);
    }
    return v;
}

std::vector<Sonoboa> Missione::boe() const {
    std::vector<Sonoboa> v;
    if (!asw_) return v;
    for (const auto& [id, a] : asw_->aerei) v.insert(v.end(), a.c.boe().begin(), a.c.boe().end());
    v.insert(v.end(), asw_->boe_extra.begin(), asw_->boe_extra.end());
    std::sort(v.begin(), v.end(), [](const Sonoboa& x, const Sonoboa& y) { return x.id < y.id; });
    return v;
}

const std::vector<MarcatoreAereo>& Missione::marcatori_asw() const {
    static const std::vector<MarcatoreAereo> nessuno;
    return asw_ ? asw_->marcatori : nessuno;
}

const std::vector<LancioAereo>& Missione::lanci_aerei() const {
    static const std::vector<LancioAereo> nessuno;
    return asw_ ? asw_->lanci : nessuno;
}

double Missione::quota_aerea(const UnitaClassica& u) const {
    if (asw_)
        if (const auto it = asw_->aerei.find(u.id); it != asw_->aerei.end() && it->second.attivo) return it->second.quota_m;
    const DatiMezzo* m = asw_ ? asw_->dati.mezzo(u.classe) : nullptr;
    const bool elicottero = m ? m->elicottero : elicottero_noto(u.classe);
    return elicottero ? QUOTA_ELICOTTERO_M : QUOTA_AEREO_M;
}

void Missione::cue_asw(int id, const Cue& c) {
    if (!asw_) return;
    for (auto& [k, a] : asw_->aerei)
        if (a.attivo && (id < 0 || k == id)) a.cue.push_back(c);
}

void Missione::messaggio_asw(int codice, int unita, std::optional<double> rilevamento) {
    for (auto it = messaggi_asw_.rbegin(); it != messaggi_asw_.rend() && tempo_ - it->tempo < AVVISO_OGNI_S; ++it)
        if (it->codice == codice) return;
    messaggi_asw_.push_back({tempo_, unita, codice, rilevamento});
}

// --------------------------------------------------------------------------------------------- guida e contatti

std::optional<Missione::GuidaAsw> Missione::guida_asw(const UnitaClassica& u) const {
    if (!asw_) return std::nullopt;
    const auto it = asw_->aerei.find(u.id);
    if (it == asw_->aerei.end() || !it->second.attivo) return std::nullopt;
    const ComandoVolo& v = it->second.c.volo();
    if (v.libero && u.navigazione) return std::nullopt;   // la rotta di pattuglia dello script
    const double d = distanza(u.pos, v.meta);
    GuidaAsw g;
    g.rotta = d > 30.0 ? rilevamento(u.pos, v.meta) : u.rotta;
    // velocita' vera: il moto classico la scala in m_s_per_nodo_; vicino alla meta si rallenta
    double v_m_s = std::min(v.velocita_kn * NODO, d / std::max(1.0, asw_->dati.pensiero_s * 0.5) + 2.0);
    // con la meta non davanti si rallenta finche' il raggio di virata sta in meta' distanza: non le si gira intorno
    if (std::abs(normalizza_180(g.rotta - u.rotta)) > 30.0 && u.virata > 0.0)
        v_m_s = std::min(v_m_s, std::max(10.0, radianti(u.virata) * d / 2.0));
    // l'ala fissa non scende sotto la velocita' d'attesa (quella di ricerca, o la comandata se e' piu' bassa): con la
    // meta dietro e dentro il cerchio di virata tira dritto e poi torna, come nei passaggi veri
    if (const DatiMezzo* m = it->second.c.mezzo(); m && !m->elicottero && !v.hover) {
        const double attesa = std::min(m->velocita_ricerca_kn, v.velocita_kn) * NODO;
        if (v_m_s < attesa) {
            v_m_s = attesa;
            if (u.virata > 0.0 && std::abs(normalizza_180(g.rotta - u.rotta)) > 30.0 && d < 2.0 * v_m_s / radianti(u.virata))
                g.rotta = u.rotta;
        }
    }
    g.velocita = (v.hover ? 0.0 : v_m_s) / m_s_per_nodo_;
    return g;
}

std::optional<double> Missione::segnale_asw(const UnitaClassica& o, const UnitaClassica& b) const {
    if (!controllato_asw(o.id) || b.categoria != "sottomarino" || b.quota <= 0.381) return std::nullopt;
    const auto& a = asw_->aerei.at(o.id);
    const ControlloreAsw& c = a.c;
    const TracciaAsw& t = c.traccia();
    const std::optional<double> nessuno = a.sensore_classico ? std::nullopt : std::optional<double>(-1.0);
    if (!t.valida || t.sorgente != b.id || tempo_ - t.t > 600.0) return nessuno;
    const StatoAsw s = c.stato();
    if (t.confermata && t.errore_m <= 914.0) return 6000.0;   // fix: traccia solida per gli script
    if (s == StatoAsw::contatto || s == StatoAsw::localizzazione || s == StatoAsw::inseguimento ||
        s == StatoAsw::attacco || s == StatoAsw::verifica)
        return a.sensore_classico ? std::nullopt : std::optional<double>(600.0);
    return nessuno;
}

std::optional<Sorgente> Missione::sorgente_aereo_per(const UnitaClassica& a, const UnitaClassica& ascoltatore) const {
    if (!asw_ || !catalogo_) return std::nullopt;
    AereoAcustico x;
    x.id = a.id;
    x.classe = a.classe;
    x.pos = a.pos;
    x.quota_m = quota_aerea(a);
    x.rotta = a.rotta;
    x.velocita_kn = a.velocita * m_s_per_nodo_ / NODO;
    x.seme = seme_;
    if (const auto it = asw_->aerei.find(a.id); it != asw_->aerei.end()) x.hover = it->second.c.volo().hover;
    if (x.hover)
        if (auto s = sorgente_hover(*catalogo_, x, tempo_)) return s;
    return sorgente_aerea(*catalogo_, x, ascoltatore.pos, ascoltatore.quota, tempo_, 2.0, stato_mare());
}

void Missione::transitori_asw(std::vector<Sorgente>& sorgenti, double finestra_s) const {
    if (!asw_ || !catalogo_) return;
    for (const auto& t : asw_->transitori)
        if (auto s = sorgente_transitoria(*catalogo_, t.x, tempo_, finestra_s)) sorgenti.push_back(*s);
}

// --------------------------------------------------------------------------------------------- passo

void Missione::passo_asw(double dt) {
    auto& S = *asw_;
    const auto* g = giocatore();
    // aerei nuovi: quelli con la classe fra i mezzi
    for (const auto& [id, u] : unita_) {
        if (S.considerate.count(id)) continue;
        S.considerate.insert(id);
        if (u.distrutta || u.persa || !aereo(u)) continue;
        const DatiMezzo* m = S.dati.mezzo(u.classe);
        if (!m) continue;
        const ConfigAereoAsw* cf = nullptr;
        for (const auto& c : S.configurazioni)
            if (c.unita == id || (c.unita < 0 && c.classe == u.classe)) { cf = &c; break; }
        if (cf && hangar_persi_.count(cf->base_unita)) {   // l'evento lo fa partire da una nave con l'hangar bruciato: non parte
            trova(id)->persa = true;
            reazioni_ia_.push_back({tempo_, id, cf->base_unita, u.categoria, "hangar_perso", "asw"});
            continue;
        }
        ConfigurazioneAsw conf;
        conf.difficolta = S.difficolta;
        // la situazione dell'aereo (la principale, 31/10/2026, dal giro della S23): un neutrale e' sempre in pace, e in
        // pace nessuno tira senza un ordine dello scenario; se no quella della sua voce aereo_asw; se no quella della
        // missione fra la sua parte e quella del giocatore (le forze); se no il blocco carriera; se no la guerra
        const int lato_aereo = u.flag & 0x18;
        std::optional<SituazioneAsw> della_missione;
        if (forze_)
            if (const auto* g = giocatore()) {
                const Situazione x = situazione_fra(lato_aereo, g->flag & 0x18);
                della_missione = x == Situazione::guerra ? SituazioneAsw::guerra : x == Situazione::crisi ? SituazioneAsw::crisi : SituazioneAsw::pace;
            }
        conf.situazione = lato_aereo == 0                ? SituazioneAsw::pace
                          : cf && cf->situazione         ? *cf->situazione
                          : della_missione               ? *della_missione
                          : blocco_carriera_             ? situazione_da(blocco_carriera_->situazione).value_or(SituazioneAsw::pace)
                                                         : SituazioneAsw::guerra;
        conf.equipaggio = cf ? cf->equipaggio : std::nullopt;
        conf.base = u.pos;
        if (cf && cf->base) conf.base = *cf->base;
        if (cf && cf->base_unita >= 0)
            if (const auto* b = trova(cf->base_unita)) conf.base = b->pos;
        conf.zona = cf && cf->zona ? *cf->zona : u.navigazione ? u.meta : u.pos;
        conf.raggio_zona_m = cf && cf->raggio_zona_m > 0.0 ? cf->raggio_zona_m : 10000.0;
        conf.rotta_presunta = cf && cf->rotta_presunta ? *cf->rotta_presunta : u.rotta;
        conf.carico = cf ? cf->carico : std::string{};
        conf.ricognizione = cf && cf->missione == "ricognizione";
        conf.armi = cf && cf->armi ? *cf->armi : !conf.ricognizione;
        if (cf) {
            conf.capogruppo = cf->capogruppo;
            conf.indice_gruppo = cf->indice_gruppo;
            conf.numero_gruppo = cf->numero_gruppo;
        }
        StatoAswMissione::Aereo a;
        a.c = ControlloreAsw(S.dati, u.classe, id, conf, tempo_);
        if (m->mad) a.mad = SensoreMad(*m->mad);
        a.quota_m = m->quota_ricerca_m;
        a.prossimo = tempo_;
        // bersagli: quelli della configurazione, o i sottomarini dell'altra parte (la nazione della classe)
        if (cf && cf->bersaglio_giocatore && g) a.bersagli.insert(g->id);
        if (cf) a.bersagli.insert(cf->bersagli.begin(), cf->bersagli.end());
        if (a.bersagli.empty())
            for (const auto& [k, v] : unita_)
                if (v.categoria == "sottomarino" && (v.flag & 0x18) && (v.flag & 0x18) != lato_nazione(m->parte))
                    a.bersagli.insert(k);
        if (cf) {
            a.cue = cf->cue;
            a.sensore_classico = cf->sensore_classico;
        }
        S.aerei.emplace(id, std::move(a));
    }

    // la boa marcatrice degli script (missione 7): una DICASS vera, posata dall'aereo controllato piu' vicino
    for (const auto& [id, u] : unita_) {
        if (u.classe != "boa_marcatrice" || u.distrutta || u.persa || S.marcatrici.count(id)) continue;
        int chi = -1;
        double vicino = 50000.0;
        for (const auto& [k, a] : S.aerei)
            if (const auto* v = trova(k); a.attivo && v && distanza(v->pos, u.pos) < vicino) { vicino = distanza(v->pos, u.pos); chi = k; }
        const DatiBoa* d = S.dati.boa("ssq62b");
        if (chi < 0 || !d) { S.marcatrici[id] = -1; continue; }
        const auto& a = S.aerei.at(chi);
        Sonoboa b = crea_boa(*d, u.pos, a.quota_m, tempo_, chi * 1000 + 900 + static_cast<int>(S.boe_extra.size()), chi,
                             DatiDifficolta{}, S.caso);
        b.guasta = false;
        b.deriva = {};
        S.marcatrici[id] = b.id;
        S.boe_extra.push_back(b);
        Cue q;   // il controllore lavora sul punto marcato
        q.tipo = Cue::Tipo::contatto_nave;
        q.pos = u.pos;
        q.t = tempo_;
        q.raggio_m = 500.0;
        S.aerei.at(chi).cue.push_back(q);
        S.transitori.push_back({{S.prossimo_transitorio++, "tonfo_boa", b.pos, 0.0, b.t_impatto, chi, seme_}, false, MSG_ASW_TONFO});
    }
    for (auto& b : S.boe_extra)   // segue la sua unita' finche' c'e'
        for (const auto& [unita, boa] : S.marcatrici)
            if (boa == b.id) {
                const auto* v = trova(unita);
                if (v && !v->distrutta && !v->persa) b.pos = v->pos;
                else b.t_fine = std::min(b.t_fine, tempo_);
            }
    // l'esito delle armi aviolanciate per la verifica del controllore
    for (; S.colpi_visti < colpi_.size(); ++S.colpi_visti) {
        const Colpo& c = colpi_[S.colpi_visti];
        if (const auto it = S.aerei.find(c.lanciatore); it != S.aerei.end()) it->second.c.esito_arma(tempo_, c.distrutta);
    }

    for (auto& [id, a] : S.aerei) {
        if (!a.attivo) continue;
        auto* u = trova(id);
        if (!u || u->distrutta || u->persa) {   // abbattuto o tolto: le sue boe restano
            a.attivo = false;
            continue;
        }
        // quota vera verso quella comandata
        const double salita = (a.c.mezzo()->elicottero ? SALITA_ELICOTTERO_M_S : SALITA_AEREO_M_S) * dt;
        const double voluta = a.c.volo().hover ? a.c.volo().quota_m : std::max(a.c.volo().quota_m, a.quota_minima_m);   // INCSEA
        a.quota_m += std::clamp(voluta - a.quota_m, -salita, salita);
        // MAD a ogni passo (il punto piu' vicino del passaggio)
        if (a.mad && a.quota_m < 600.0) {
            std::vector<BersaglioMad> bm;
            for (const int b : a.bersagli)
                if (const auto* v = trova(b); v && !v->distrutta && !v->persa && v->quota > 3.0) {
                    const auto sc = S.dati.dislocamenti.find(v->classe);
                    const double disl = sc != S.dati.dislocamenti.end() ? sc->second.dislocamento_t : 5000.0;
                    const double f = sc != S.dati.dislocamenti.end() ? sc->second.fattore : 1.0;
                    bm.push_back({b, v->pos, v->quota, disl * f * f * f});   // il fattore e' sulla portata
                }
            const double rumore = rumore_mad_col_mare(0.1, meteo_in(u->pos).stato_mare());   // dossier 21 §8 punto 7
            for (const int s : a.mad->passo(u->pos, a.quota_m, rumore, bm, S.caso)) a.mad_sentiti.push_back(s);
        }
        // sorvolo del giocatore nella finestra di Snell (§8.3): un avviso per passaggio, con probabilita'
        if (g && catalogo_ && !a.c.volo().hover) {
            const bool dentro = distanza(u->pos, g->pos) <= finestra_snell_m(a.quota_m, g->quota, stato_mare());
            if (dentro && !a.nel_sorvolo && S.caso.uniforme() < probabilita_sorvolo(g->quota))
                messaggio_asw(MSG_ASW_SORVOLO, id, rilevamento(g->pos, u->pos));
            a.nel_sorvolo = dentro;
        }
        if (tempo_ + 1e-9 >= a.prossimo) {
            a.prossimo += S.dati.pensiero_s;
            pensa_asw(id, tempo_);
        }
    }

    // bombe di profondita' (PLAB): affondano e scoppiano con la regola dei colpi, la carica e la geometria vere
    for (auto it = S.bombe.begin(); it != S.bombe.end();) {
        if (tempo_ < it->scoppio) { ++it; continue; }
        const StatoAswMissione::Bomba b = *it;
        it = S.bombe.erase(it);
        S.transitori.push_back({{S.prossimo_transitorio++, "esplosione_mgab", b.pos, b.quota_scoppio_m, tempo_, b.aereo, seme_}, false, 0});
        UnitaClassica* v = nullptr;
        double migliore = PLAB_RAGGIO_M, dz = 0.0, oriz = 0.0;
        for (auto& [k, x] : unita_) {
            if (x.distrutta || x.persa || x.categoria != "sottomarino") continue;
            const double h = distanza(x.pos, b.pos), z = b.quota_scoppio_m - x.quota, r = std::hypot(h, z);
            if (r < migliore) { migliore = r; v = &x; dz = z; oriz = h; }
        }
        if (!v) continue;
        const auto* l = trova(b.aereo);
        UnitaClassica arma;
        arma.id = prossimo_id_++;
        arma.classe = b.tipo;
        arma.categoria = "arma";
        arma.flag = 0x20 | (l ? lato_nazione(S.aerei.at(b.aereo).c.mezzo()->parte) : 0);
        arma.pos = b.pos;
        arma.quota = arma.quota_ordinata = b.quota_scoppio_m;
        arma.lanciatore = b.aereo;
        arma.bersaglio = v->id;
        arma.prossimo_bersaglio = arma.id;
        arma.difesa = static_cast<int>(std::lround(75.0 * b.carica_kg / 293.0));   // in scala col Mk 48, come le mine
        auto& r = unita_[arma.id] = arma;
        const auto n = colpi_.size();
        colpisci(r, *v);
        if (colpi_.size() > n) {
            auto& c = colpi_.back();
            c.geometria_vera = true;
            c.distanza_m = oriz;
            c.dz_m = dz;
            c.tnt_kg = b.carica_kg;
            c.influenza = true;
        }
    }

    // transitori: chi li sente fra quelli del giocatore (una volta al secondo), poi via quelli finiti
    if (g && catalogo_ && tempo_ - S.ultimo_ascolto_transitori >= 1.0) {
        S.ultimo_ascolto_transitori = tempo_;
        std::vector<Ricevitore> ric;
        for (const Sensore* sn : sensori_acustici(*g, TipoSensori::passivi)) ric.push_back(ricevitore_acustico(*g, sn));
        for (auto& t : S.transitori) {
            if (t.sentito || t.codice == 0 || tempo_ < t.x.t0) continue;
            const auto s = sorgente_transitoria(*catalogo_, t.x, tempo_, 1.0);
            if (!s) continue;
            const AscoltoTransitorio x = ascolta_transitorio(*s, ric, ambiente_acustico(g->pos, t.x.pos));
            if (!x.sentito) continue;
            t.sentito = true;
            const double u1 = std::max(S.caso.uniforme(), 1e-12), u2 = S.caso.uniforme();
            const double gs = std::sqrt(-2.0 * std::log(u1)) * std::cos(2.0 * PI * u2);
            messaggio_asw(t.codice, t.x.origine, normalizza_360(x.rilevamento + x.errore_deg * gs));
        }
    }
    if (catalogo_)
        std::erase_if(S.transitori, [&](const auto& t) { return transitorio_finito(*catalogo_, t.x, tempo_); });

    // ESM: il modo periscopio dell'APS-116/137 (300 giri/min), la firma del cacciatore di periscopi (§8.2)
    for (const auto& x : intercettazioni())
        if ((x.radar == "aps116" || x.radar == "aps137") && S.aerei.count(x.unita) && !S.periscopio_annunciato.count(x.unita)) {
            S.periscopio_annunciato.insert(x.unita);
            messaggio_asw(MSG_ASW_PERISCOPIO, x.unita, x.rilevamento);
        }
    // marcatori in vista del periscopio alzato (§8.1)
    if (g && (g->flag54 & 2))
        for (std::size_t i = 0; i < S.marcatori.size(); ++i) {
            const auto& m = S.marcatori[i];
            if (S.marcatori_visti.count(i) || tempo_ < m.inizio || tempo_ > m.fine) continue;
            const auto d = S.dati.marcatori.find(m.tipo);
            if (d == S.dati.marcatori.end() || d->second.acustico || distanza(g->pos, m.pos) > d->second.visibile_m) continue;
            S.marcatori_visti.insert(i);
            messaggio_asw(MSG_ASW_MARCATORE, m.aereo, rilevamento(g->pos, m.pos));
        }
    // i contatti degli aerei controllati nella loro lista (trailer-1, la principale, 31/10/2026: per la simmetria e per
    // le condizioni «unita' vede», «tenuto da», la storia dei contatti): la traccia del controllore, viva da meno di
    // TRACCIA_VIVA_S, sulla sorgente con piu' rivelazioni associate (la verita': puo' essere una nave o un biologico, e il
    // falso contatto entra come tale). La qualita' dalle rivelazioni, solida e classificata (la durata lunga) quando e'
    // confermata; la posizione stimata; il rilevamento dall'aereo; il sensore «boe»
    constexpr double TRACCIA_VIVA_S = 120.0;
    for (auto& [id, a] : S.aerei) {
        if (!a.attivo) continue;
        auto* u = trova(id);
        if (!u || u->distrutta || u->persa) continue;
        const TracciaAsw& tr = a.c.traccia();
        if (!tr.valida || tr.sorgente < 0 || tr.sorgente == id || tempo_ - tr.t > TRACCIA_VIVA_S || !trova(tr.sorgente)) continue;
        Contatto* c = u->contatti.count(tr.sorgente) ? &u->contatti.at(tr.sorgente) : nuovo_contatto(*u, tr.sorgente);
        if (!c) continue;
        c->qualita = std::max(c->qualita, tr.confermata ? 1000.0 : std::min(790.0, 400.0 + 100.0 * tr.rivelazioni));
        if (tr.confermata) c->durata = std::max(c->durata, 1400.0);
        c->pos_stimata = tr.pos;
        c->posizione_nota = true;
        c->t_posizione = tr.t;
        c->rilevamento = rilevamento(u->pos, tr.pos);
        c->ultima_scoperta = std::max(c->ultima_scoperta, tr.t);
        c->lampeggio = 0;
        c->sensore = "boe";
    }
}

void Missione::pensa_asw(int id, double t) {
    auto& S = *asw_;
    auto& a = S.aerei.at(id);
    auto* u = trova(id);
    const auto* g = giocatore();
    PercezioneAsw p;
    p.pos = u->pos;
    p.quota_m = a.quota_m;
    p.rotta = u->rotta;
    p.ora_locale = std::fmod(std::fmod((ora_utc_s_ + t) / 3600.0 + longitudine_ / 15.0, 24.0) + 24.0, 24.0);
    // la difficolta' senza operazioni notturne (facile, chiave `notte` di [difficolta.*]): di notte l'aereo rientra
    if (const auto df = S.dati.difficolta.find(S.difficolta);
        df != S.dati.difficolta.end() && !df->second.notte && notte_asw(p.ora_locale) && a.c.stato() != StatoAsw::rientro)
        a.c.richiama(t);
    // il meteo nel punto dell'aereo (M2): mare, vento, onde; fulmini entro 5 nm fermano il sonar a immersione
    const StatoMeteo qui = meteo_in(u->pos);
    p.mare = qui.stato_mare();
    p.vento_kn = qui.vento_m_s / NODO;
    p.hs_m = qui.hs_m();
    if (clima_) p.strato_m = sld_in(u->pos);
    if (fondale_) p.fondale_m = profondita(u->pos);
    if (forze_) {   // fase 3: la situazione della sua parte con l'altra, se e' salita (guerra dichiarata, provocazione)
        const int l = u->flag & 0x18;
        const Situazione sf = situazione_nota(l, l == 0x08 ? 0x10 : 0x08).value_or(Situazione::pace);
        const SituazioneAsw sa = sf == Situazione::guerra ? SituazioneAsw::guerra : sf == Situazione::crisi ? SituazioneAsw::crisi : SituazioneAsw::pace;
        if (static_cast<int>(sa) > static_cast<int>(a.c.situazione())) p.situazione = sa;
    }
    // la sandbox: le ROE della parte sui sottomarini vengono prima della situazione (libere: come in guerra; trattenute:
    // l'aereo caccia ma non sgancia, come in pace)
    if (const auto r = roe_parte(u->flag & 0x18, CategoriaRoe::sottomarini)) p.situazione = *r ? SituazioneAsw::guerra : SituazioneAsw::pace;
    for (const auto& [classe, d] : tabella_siluri_.siluri)
        if (d.fondale_minimo_m > 0.0) p.fondale_minimo[classe] = d.fondale_minimo_m;
    if (meteo_)
        for (const auto& c : meteo_->celle(t))
            p.temporale_vicino = p.temporale_vicino || distanza(c.pos, u->pos) < c.raggio_m + 5.0 * MIGLIO;
    // il ghiaccio (LIB-I16 di trailer-1): boe, trasduttore del sonar a immersione e armi solo in acqua libera, le polynye
    // e i canali del campo (missione_ghiaccio.cpp); il nilas no, come per il periscopio. Senza il campo, sotto meta'
    // copertura del meteo (STIMA)
    if (meteo_ && prepara_ghiaccio())
        p.acqua_libera = [this](Vec2 q) {
            const double copertura = copertura_ghiaccio(q);
            if (copertura <= 0.0) return true;
            if (!ghiaccio_) return copertura < 0.5;
            const PuntoGhiaccio g = ghiaccio_->in(q, tempo_);
            return !g.ghiaccio || g.aperta;
        };
    p.cue = std::move(a.cue);
    a.cue.clear();
    p.mad = std::move(a.mad_sentiti);
    a.mad_sentiti.clear();
    p.acustiche = std::move(a.in_attesa);
    a.in_attesa.clear();
    // radar: i bersagli che il radar dell'aereo ha visto (albero o battello emerso) in questo pensiero
    for (const int b : a.bersagli)
        if (const auto it = u->contatti.find(b); it != u->contatti.end() && it->second.sensore == "radar" &&
                                                 t - it->second.ultima_scoperta <= S.dati.pensiero_s)
            p.radar.push_back(it->second.pos_stimata);
    for (const int b : a.bersagli)   // per il radar dell'aereo, che resta acceso su quello che vede (passo_elettronica)
        if (const auto it = u->contatti.find(b); it != u->contatti.end() && (it->second.sensore == "radar" || it->second.sensore == "vista") &&
                                                 t - it->second.ultima_scoperta <= S.dati.pensiero_s)
            a.bersaglio_visto = t;
    // avviso «aereo ASW in zona» all'arrivo (facile, §10.3)
    if (!a.annunciato && S.difficolta == DifficoltaAsw::facile && g && a.bersagli.count(g->id) &&
        a.c.stato() != StatoAsw::transito) {
        a.annunciato = true;
        messaggio_asw(MSG_ASW_IN_ZONA, id, rilevamento(g->pos, u->pos));
    }
    // ascolto: boe seguite e dip passivo, contro i bersagli e le sorgenti dei falsi allarmi vicine
    std::vector<SorgenteAsw> sorgenti;
    if (catalogo_) {
        for (const auto& [k, v] : unita_) {
            if (v.distrutta || v.persa || aereo(v) || (v.flag & 0x20)) continue;
            const bool bersaglio = a.bersagli.count(k) > 0;
            const bool falso = !bersaglio && (v.categoria == "nave" || v.classe == "biologico") &&
                               distanza(v.pos, u->pos) <= DISTANZA_FALSI_M;
            if (!bersaglio && !falso) continue;
            if (auto s = sorgente_acustica(v)) sorgenti.push_back({*s, bersaglio});
        }
        const AmbienteFra amb = [this](Vec2 da, Vec2 x) { return ambiente_acustico(da, x); };
        const std::vector<int> seguite = a.c.boe_seguite(t);
        std::vector<Sonoboa> ascolto;
        for (const auto& b : a.c.boe())
            if (std::find(seguite.begin(), seguite.end(), b.id) != seguite.end()) ascolto.push_back(b);
        const double guasto = attenuazione_asw(*this, id);   // il vetrino di Dio: il processore acustico dell'aereo
        const auto rb = ascolta_boe(ascolto, S.dati, *catalogo_, sorgenti, amb, t, meteo_in(u->pos).stato_mare(), S.caso, S.ascolto, &raggi_,
                                    guasto);
        p.acustiche.insert(p.acustiche.end(), rb.begin(), rb.end());
        // SSQ-47B: pingano da sole, ognuna ogni periodo dalla sua attivazione; l'eco a ogni pensiero, il ping per chi
        // ascolta quando fra il pensiero di prima (ultimo_ping_boe) e questo cade un multiplo del periodo (disassemblatore,
        // 31/10/2026: prima un ping per aereo a ogni periodo, qualunque fosse il numero delle boe in acqua)
        const double pensiero_prima = a.ultimo_ping_boe > -1e17 ? a.ultimo_ping_boe : t - S.dati.pensiero_s;
        for (const auto& b : ascolto) {
            const DatiBoa* d = S.dati.boa(b.tipo);
            if (!d || !b.attiva_sonar || d->periodo_ping_s <= 0.0) continue;
            const auto e = ascolta(ricevitore_boa(b, *d, *catalogo_, t), b.id, sorgenti, amb, t, S.caso, &raggi_, guasto);
            p.acustiche.insert(p.acustiche.end(), e.begin(), e.end());
        }
        for (const auto& b : a.c.boe()) {   // il ping lo fa ogni boa viva, seguita o no dai canali dell'aereo
            const DatiBoa* d = S.dati.boa(b.tipo);
            if (!d || !b.attiva_sonar || d->periodo_ping_s <= 0.0 || !boa_in_ascolto(b, t)) continue;
            const double k = std::floor((t - b.t_attiva) / d->periodo_ping_s);
            if (k < 0.0 || k <= std::floor((pensiero_prima - b.t_attiva) / d->periodo_ping_s)) continue;
            const double t0 = b.t_attiva + k * d->periodo_ping_s;   // l'ultimo ping del pensiero
            if (statistiche_) emissioni_.push_back({t0, id, "ping", d->impulso_s, d->sensore});   // R5: i ping delle boe dell'aereo
            if (const Sensore* se = catalogo_->sensore(d->sensore))   // agli intercettatori (blocco 2, E)
                emetti_impulso_da(id, posizione_boa(b, t0), b.quota_m, d->sensore, se->sl_attivo_db);
            S.transitori.push_back({{S.prossimo_transitorio++, transitorio_ping(d->sensore), posizione_boa(b, t0),
                                     b.quota_m, t0, id, seme_}, false, MSG_ASW_BOA_ATTIVA});
        }
        a.ultimo_ping_boe = t;
        if (const DatiSonarImmersione* sn = S.dati.sonar(a.c.mezzo()->sonar); sn && a.c.dip().in_acqua()) {
            const Ricevitore r = ricevitore_dip(u->pos, a.c.dip().cavo_m(), *sn, *catalogo_, false);
            if (r.sensore) {
                const auto e = ascolta(r, id, sorgenti, amb, t, S.caso, &raggi_, guasto);
                p.acustiche.insert(p.acustiche.end(), e.begin(), e.end());
            }
        }
    }
    // azioni del controllore
    for (const auto& x : a.c.passo(t, p, S.caso)) esegui_azione_asw(id, x);
    (void)g;
}

void Missione::esegui_azione_asw(int id, const AzioneAsw& x) {
    auto& S = *asw_;
    auto& a = S.aerei.at(id);
    auto* u = trova(id);
    auto* g = trova(giocatore() ? giocatore()->id : -1);
    const auto nuovo_transitorio = [&](const std::string& tipo, Vec2 pos, double quota, double t0, int codice) {
        if (tipo.empty()) return;
        S.transitori.push_back({{S.prossimo_transitorio++, tipo, pos, quota, t0, id, seme_}, false, codice});
    };
    // gli echi: il ricevitore attivo contro i bersagli (le altre unita' non rimandano al controllore)
    const auto eco = [&](const Ricevitore& r, int ricevitore_id) {
        if (!catalogo_ || !r.sensore) return;
        std::vector<SorgenteAsw> bersagli;
        for (const int b : a.bersagli)
            if (const auto* v = trova(b); v && !v->distrutta && !v->persa)
                if (auto s = sorgente_acustica(*v)) bersagli.push_back({*s, true});
        const AmbienteFra amb = [this](Vec2 da, Vec2 y) { return ambiente_acustico(da, y); };
        const auto e = ascolta(r, ricevitore_id, bersagli, amb, tempo_, S.caso, &raggi_, attenuazione_asw(*this, id));
        a.in_attesa.insert(a.in_attesa.end(), e.begin(), e.end());
    };
    switch (x.tipo) {
        case AzioneAsw::Tipo::posa_boa:
            if (x.boa) nuovo_transitorio("tonfo_boa", x.boa->pos, 0.0, x.boa->t_impatto, MSG_ASW_TONFO);
            break;
        case AzioneAsw::Tipo::cala_sonar:
            nuovo_transitorio("tonfo_trasduttore", u->pos, 0.0, tempo_, MSG_ASW_TONFO);
            break;
        case AzioneAsw::Tipo::ping: {
            const DatiSonarImmersione* sn = S.dati.sonar(x.oggetto);
            if (!sn) break;
            nuovo_transitorio(transitorio_ping(sn->sensore), u->pos, x.quota_m, tempo_, MSG_ASW_DIP);
            if (catalogo_)   // agli intercettatori (blocco 2, E): dal trasduttore calato
                if (const Sensore* se = catalogo_->sensore(sn->sensore)) emetti_impulso_da(id, u->pos, x.quota_m, sn->sensore, se->sl_attivo_db);
            if (catalogo_) eco(ricevitore_dip(u->pos, a.c.dip().cavo_m(), *sn, *catalogo_, true), id);
            break;
        }
        case AzioneAsw::Tipo::comanda_boa: {
            const auto it = std::find_if(a.c.boe().begin(), a.c.boe().end(), [&](const Sonoboa& b) { return b.id == x.bersaglio; });
            const DatiBoa* d = it != a.c.boe().end() ? S.dati.boa(it->tipo) : nullptr;
            if (!d) break;
            nuovo_transitorio(transitorio_ping(d->sensore), posizione_boa(*it, tempo_), it->quota_m, tempo_, MSG_ASW_BOA_ATTIVA);
            if (statistiche_) emissioni_.push_back({tempo_, id, "ping", d->impulso_s, d->sensore});   // R5
            if (catalogo_)   // agli intercettatori, come la SSQ-47B e il sonar a immersione: la DICASS e la RGB-55A a comando
                if (const Sensore* se = catalogo_->sensore(d->sensore)) emetti_impulso_da(id, posizione_boa(*it, tempo_), it->quota_m, d->sensore, se->sl_attivo_db);
            if (catalogo_) eco(ricevitore_boa(*it, *d, *catalogo_, tempo_), it->id);
            break;
        }
        case AzioneAsw::Tipo::lancia_arma: {
            // siluri e bombe antisom solo su un sottomarino: la traccia finita su una nave (un mercantile sentito dalle
            // boe) non si attacca, l'equipaggio la riconosce al radar o a vista
            if (const auto* b = trova(x.bersaglio); b && b->categoria != "sottomarino") break;
            // le zone dei sottomarini amici (fase 3 C e 4, docs/18): il punto dentro un'area d'azione amica, l'equipaggio
            // non sgancia, salvo il contatto classificato per classe e la P della prima ora dopo un attacco
            if (sgancio_vietato(id, x.bersaglio, x.pos)) break;
            LancioAereo l;
            l.tempo = tempo_;
            l.aereo = id;
            l.tipo = x.oggetto;
            l.pos = x.pos;
            l.quota_sgancio_m = a.quota_m;
            l.rotta = x.rotta;
            const DatiSiluro* d = siluri_attivi_ ? tabella_siluri_.siluro(x.oggetto) : nullptr;
            // l'ingresso in acqua col mare (dossier 21 §7.4): 3% di guasti fino a mare 4, 10% a 6, 25% a 7
            const int mare = meteo_in(x.pos).stato_mare();
            const double p_guasto = mare <= 4 ? 0.03 : mare == 5 ? 0.06 : mare == 6 ? 0.10 : 0.25;
            if (d && meteo_ && S.caso.uniforme() < p_guasto) {
                nuovo_transitorio("tonfo_siluro", x.pos, 0.0, tempo_, MSG_ASW_TONFO);   // il tonfo c'e', il siluro no
                S.lanci.push_back(l);
                break;
            }
            if (d) {   // siluro col paracadute: entra in acqua sul punto e parte col modello dell'Enhanced
                UnitaClassica w;
                w.id = prossimo_id_++;
                w.classe = x.oggetto;
                w.categoria = "arma";
                w.flag = 0x20 | lato_nazione(a.c.mezzo()->parte);
                w.pos = x.pos;
                w.quota = w.quota_ordinata = 15.0;
                w.rotta = w.rotta_ordinata = x.rotta;
                w.lanciatore = id;
                w.bersaglio = trova(x.bersaglio) ? x.bersaglio : -1;
                w.prossimo_bersaglio = w.id;
                w.velocita = w.velocita_ordinata = d->velocita_alta_kn;
                w.difesa = 40;
                auto& r = unita_[w.id] = w;
                osservatore_ = visitata_ = w.id;   // come nuova_unita_da_buffer: in testa alla lista
                ++create_;
                nuovo_siluro(r, d);
                l.arma = w.id;
                nuovo_transitorio("tonfo_siluro", x.pos, 0.0, tempo_, MSG_ASW_SILURO);
            } else if (x.oggetto.rfind("plab", 0) == 0 || x.oggetto == "sjunkbomb51") {   // bomba di profondita' (la Sjunkbomb 51 dell'Hkp 4)
                StatoAswMissione::Bomba b;
                b.aereo = id;
                b.tipo = x.oggetto;
                b.pos = x.pos;
                b.quota_scoppio_m = PLAB_QUOTA_M;
                // PLAB-250-120; PLAB-50 STIMA; la Sjunkbomb 51 «debole, deve scoppiare vicino» (docs/ricerca/35 §5): 25 kg STIMA
                b.carica_kg = x.oggetto == "plab250" ? 120.0 : x.oggetto == "sjunkbomb51" ? 25.0 : 30.0;
                b.scoppio = tempo_ + PLAB_QUOTA_M / PLAB_AFFONDA_M_S;
                S.bombe.push_back(b);
                nuovo_transitorio("tonfo_siluro", x.pos, 0.0, tempo_, MSG_ASW_TONFO);
            }
            S.lanci.push_back(l);
            break;
        }
        case AzioneAsw::Tipo::marcatore: {
            const auto d = S.dati.marcatori.find(x.oggetto);
            S.marcatori.push_back({id, x.oggetto, x.pos, tempo_, tempo_ + (d != S.dati.marcatori.end() ? d->second.durata_s : 900.0)});
            break;
        }
        case AzioneAsw::Tipo::sus:
            nuovo_transitorio("sus_mk84", x.pos, 20.0, tempo_, MSG_ASW_SUS);
            S.marcatori.push_back({id, x.oggetto, x.pos, tempo_, tempo_ + 80.0});
            break;
        case AzioneAsw::Tipo::passa_contatto: {
            auto* v = trova(x.bersaglio);
            if (!v || !g) break;
            // amico del giocatore: la stessa nazione e non lo caccia (nella TORPEX il giocatore e' il bersaglio)
            const bool amico = (g->flag & 0x18) == lato_nazione(a.c.mezzo()->parte) && !a.bersagli.count(g->id);
            if (amico && v->id != g->id) {   // al giocatore: il contatto con la posizione (ricognizione)
                Contatto* c = g->contatti.count(v->id) ? &g->contatti.at(v->id) : nuovo_contatto(*g, v->id);
                if (c) {
                    c->qualita = std::max(c->qualita, 400.0);
                    c->lampeggio = 0;
                    c->ultima_scoperta = tempo_;
                    c->rilevamento = rilevamento(g->pos, x.pos);
                    c->pos_stimata = x.pos;
                    c->quota_stimata = v->quota;
                    c->posizione_nota = true;
                    c->t_posizione = tempo_;
                    c->sensore = "aereo";
                }
                messaggio_asw(MSG_ASW_RICOGNIZIONE, id, rilevamento(g->pos, x.pos));
            } else if (!amico && v->id == g->id) {   // alle navi della sua parte: il contatto sul giocatore
                contatto_da_radiogoniometria(x.pos, -1, x.pos);
            }
            break;
        }
        default: break;   // recupera_sonar, rapporto: solo nel registro
    }
}

// --- evasione dei sottomarini dell'IA dalla caccia aerea (nucleo/evasione.hpp) ---

namespace {

// portata d'intercettazione dei ping dal battello (dossier 17 §8.3: DICASS ~20 km, AQS-13 ~27 km; gli altri STIMA)
double intercettazione_ping_m(const std::string& tipo) {
    if (tipo == "ping_aqs13b" || tipo == "ping_aqs13f") return 27000.0;
    if (tipo == "ping_vgs3" || tipo == "ping_vgs2" || tipo == "ping_dicass") return 20000.0;
    if (tipo == "ping_ssq47b" || tipo == "ping_rgb55a" || tipo == "esplosione_mgab") return 15000.0;
    return 0.0;
}
constexpr double PING_VICINO_M = 5000.0;    // STIMA: entro la portata della scoperta dell'eco (1,6-4 km) con margine
// la minaccia credibile (la principale, 31/10/2026: nella S22 i Victor evadevano per ogni ping lontano della Perry):
// un tonfo fa evadere se sentito con margine; dopo una manovra per un tonfo o un ping, gli altri per un quarto d'ora no
constexpr double TONFO_EVASIONE_DB = 6.0;   // STIMA: l'eccesso di segnale di un tonfo sentito chiaramente, vicino
constexpr double RAFFREDDAMENTO_S = 900.0;  // STIMA: il comandante non riparte per ogni ping della stessa boa
constexpr double SILURO_SENTITO_M = 3000.0; // STIMA: la testa attiva a 25-30 kHz di un siluro aviolanciato
constexpr double SILURO_VICINO_M = 1500.0;  // entro l'acquisizione del Mk 46 (1600 yd, §6.7)
// esche dell'IA (STIMA): 4 per battello (i sovietici avevano le loro, come i Mk 2 ADC del 688), una ogni 45 s al piu',
// con il modello dell'esca del giocatore (flag 193, rumore 40) e la durata di siluri.toml (ADC Mk 2: 10 min di batteria,
// dossier 05 §5.1; senza la voce i 160 s del gestore delle missioni)
constexpr int ESCHE_IA = 4;
constexpr double ESCA_OGNI_S = 45.0, ESCA_VITA_S = 160.0;
// MG-74 dei nucleari sovietici col siluro in coda (dossier 05 §5.2 e §7: «2-4 al posto dei siluri», STIMA 2), una ogni
// due minuti, sulla rotta che il battello lascia: corre dove il siluro se lo aspetta mentre lui accosta e cambia quota
constexpr int ESCHE_MOBILI_IA = 2;
constexpr double ESCA_MOBILE_OGNI_S = 120.0;

}  // namespace

void Missione::lancia_esca_ia(const UnitaClassica& b) {
    auto& S = *asw_;
    if (!esche_mobili_spente_ && (b.flag & 0x10) && !battello_diesel(b.classe) && S.esche_mobili_ia[b.id] < ESCHE_MOBILI_IA) {
        const auto it = S.ultima_esca_mobile.find(b.id);
        if ((it == S.ultima_esca_mobile.end() || tempo_ - it->second >= ESCA_MOBILE_OGNI_S) &&
            lancia_esca_mobile(b.id, "mg74", b.rotta) >= 0) {
            ++S.esche_mobili_ia[b.id];
            S.ultima_esca_mobile[b.id] = tempo_;
            esche_ia_.push_back({tempo_, b.id, prossimo_id_ - 1, "mg74"});
        }
    }
    if (S.esche_ia[b.id] >= ESCHE_IA) return;
    if (const auto it = S.ultima_esca.find(b.id); it != S.ultima_esca.end() && tempo_ - it->second < ESCA_OGNI_S) return;
    ++S.esche_ia[b.id];
    S.ultima_esca[b.id] = tempo_;
    UnitaClassica e;
    e.id = prossimo_id_++;
    e.classe = "esca";
    e.categoria = "arma";
    e.flag = 193;
    e.pos = b.pos;
    e.quota = e.quota_ordinata = b.quota;
    e.rotta = e.rotta_ordinata = b.rotta;
    e.velocita = e.velocita_ordinata = e.avanti_tutta = 0.9143;
    e.integrita = e.integrita_max = 2;
    e.rumore = 40;
    e.collegata = e.lanciatore = b.id;
    ++create_;
    unita_[e.id] = e;
    const DatiEsca* adc = tabella_siluri_.esca("adc_mk2");
    S.esche_attive.push_back({e.id, tempo_ + (adc ? adc->durata_s : ESCA_VITA_S)});
    esche_ia_.push_back({tempo_, b.id, e.id, "adc_mk2"});
}

// uno stimolo alla manovra d'evasione di un sottomarino dell'IA; su un siluro anche le esche, nella virata
void Missione::stimola_evasione(int b, StimoloEvasione s, Vec2 da, double pavimento, int minaccia) {
    if (!asw_ || evasione_spenta_ || !asw_->evasione) return;
    auto& S = *asw_;
    const auto* v = trova(b);
    if (!v || v->giocatore || v->distrutta || v->persa) return;
    // solo una minaccia credibile fa evadere: il ping lontano no (restano il contatto e l'allerta); il tonfo e il ping
    // vicino al piu' una volta per RAFFREDDAMENTO_S, contando dall'inizio della manovra; siluri e sorvoli sempre
    if (s == StimoloEvasione::ping_lontano) return;
    const bool ping_o_tonfo = s == StimoloEvasione::tonfo || s == StimoloEvasione::ping_vicino;
    if (ping_o_tonfo)
        if (const auto u = S.ultima_evasione_ping.find(b); u != S.ultima_evasione_ping.end() && tempo_ - u->second < RAFFREDDAMENTO_S) return;
    BattelloEvasione be = battello_evasione(*v);
    be.pavimento_siluro_m = pavimento;
    auto& ev = S.evasioni[b];
    const int prima = ev.manovre();
    ev.stimolo(s, rilevamento(v->pos, da), tempo_, be, S.caso_evasione);
    if (ping_o_tonfo && ev.manovre() > prima) S.ultima_evasione_ping[b] = tempo_;
    if (statistiche_ && ev.manovre() > prima) {   // R4 della sandbox: solo l'inizio di una manovra
        static const char* const NOMI[] = {"ping_lontano", "tonfo", "sorvolo", "ping_vicino", "siluro", "siluro_vicino"};
        evasioni_.push_back({tempo_, b, minaccia, NOMI[static_cast<int>(s)]});
    }
    if (s == StimoloEvasione::siluro || s == StimoloEvasione::siluro_vicino) lancia_esca_ia(*v);
}

int Missione::manovre_evasione(int id) const {
    if (!asw_) return 0;
    const auto it = asw_->evasioni.find(id);
    return it == asw_->evasioni.end() ? 0 : it->second.manovre();
}

BattelloEvasione Missione::battello_evasione(const UnitaClassica& u) const {
    BattelloEvasione b;
    b.pos = u.pos;
    b.rotta_deg = u.rotta;
    b.velocita_kn = u.velocita;
    b.quota_m = u.quota;
    b.quota_max_m = u.quota_max;
    b.fondale_m = profondita(u.pos);
    b.velocita_max_kn = u.avanti_tutta;
    b.diesel = battello_diesel(u.classe);
    if (clima_) b.strato_m = sld_in(u.pos);
    return b;
}

std::optional<ComandoEvasione> Missione::guida_evasione(const UnitaClassica& u) const {
    if (!asw_ || evasione_spenta_ || !asw_->evasione || u.giocatore) return std::nullopt;
    const auto it = asw_->evasioni.find(u.id);
    if (it == asw_->evasioni.end()) return std::nullopt;
    return it->second.comando(tempo_, battello_evasione(u));
}

void Missione::passo_evasione() {
    auto& S = *asw_;
    const bool evasione = !evasione_spenta_ && S.evasione;
    // i transitori nuovi degli aerei (tonfi di boa, di trasduttore e di siluro; ping di boe e dip): li sente ogni
    // battello dell'IA immerso col modello acustico, coi suoi sensori passivi, lo strato e il rumore proprio
    // (ascolta_transitorio, come il giocatore), e chi sente ha un contatto sull'aereo col rilevamento del transitorio,
    // poi l'allerta e l'evasione (la simmetria, disassemblatore 31/10/2026: prima solo il battello cacciato, a distanze
    // fisse, e senza contatto)
    std::vector<UnitaClassica*> battelli;
    for (auto& [id, v] : unita_)
        if (!v.giocatore && !v.distrutta && !v.persa && v.categoria == "sottomarino" && v.quota > 3.0) battelli.push_back(&v);
    for (const auto& x : S.transitori) {
        if (x.x.t0 > tempo_ || S.transitori_evasione.count(x.x.id)) continue;
        S.transitori_evasione.insert(x.x.id);
        const std::string& tipo = x.x.tipo;
        const bool tonfo = tipo.rfind("tonfo_", 0) == 0;
        if (!tonfo && intercettazione_ping_m(tipo) <= 0.0) continue;
        const auto s = catalogo_ ? sorgente_transitoria(*catalogo_, x.x, tempo_, 1.0) : std::nullopt;
        const auto* aereo = trova(x.x.origine);
        if (!s || !aereo) continue;
        for (auto* b : battelli) {
            if (!ostili(*b, *aereo)) continue;
            std::vector<Ricevitore> ric;
            for (const Sensore* sn : sensori_acustici(*b, TipoSensori::passivi)) ric.push_back(ricevitore_acustico(*b, sn));
            if (ric.empty()) continue;
            const AscoltoTransitorio h = ascolta_transitorio(*s, ric, ambiente_acustico(b->pos, x.x.pos));
            if (!h.sentito) continue;
            if (!aereo->distrutta && !aereo->persa)
                if (Contatto* c = b->contatti.count(aereo->id) ? &b->contatti.at(aereo->id) : nuovo_contatto(*b, aereo->id)) {
                    c->qualita = std::max(c->qualita, 400.0);
                    c->lampeggio = 0;
                    c->ultima_scoperta = tempo_;
                    c->rilevamento = normalizza_360(h.rilevamento + h.errore_deg * gauss());
                    if (c->sensore.empty()) c->sensore = "transitorio";
                }
            if (!tonfo) evento_allerta(b->id, "ping");
            else if (tipo == "tonfo_siluro") evento_allerta(b->id, "arma");
            if (!evasione) continue;
            if (tonfo && tipo != "tonfo_siluro" && h.se_db < TONFO_EVASIONE_DB) continue;   // un tonfo appena sentito: solo l'allerta
            if (tonfo)
                stimola_evasione(b->id, tipo == "tonfo_siluro" ? StimoloEvasione::siluro : StimoloEvasione::tonfo, x.x.pos, 0.0, x.x.origine);
            else
                stimola_evasione(b->id, distanza(b->pos, x.x.pos) <= PING_VICINO_M ? StimoloEvasione::ping_vicino : StimoloEvasione::ping_lontano,
                                 x.x.pos, 0.0, x.x.origine);
        }
    }
    if (!evasione) return;
    // le esche finite
    for (const auto& [id, fine] : S.esche_attive)
        if (tempo_ >= fine)
            if (auto* e = trova(id); e && !e->distrutta) e->persa = true;
    std::erase_if(S.esche_attive, [&](const auto& x) { return tempo_ >= x.second; });
    // i battelli dell'IA cacciati da qualche aereo (i bersagli dei controllori), immersi
    std::map<int, std::vector<int>> cacciati;   // battello -> aerei che lo cercano
    for (const auto& [id, a] : S.aerei)
        if (a.attivo)
            for (const int b : a.bersagli)
                if (const auto* v = trova(b); v && !v->giocatore && !v->distrutta && !v->persa && v->categoria == "sottomarino" && v->quota > 3.0)
                    cacciati[b].push_back(id);
    if (cacciati.empty()) return;
    const auto stimola = [&](int b, StimoloEvasione s, Vec2 da, double pavimento = 0.0, int minaccia = -1) {
        stimola_evasione(b, s, da, pavimento, minaccia);
    };
    // siluri aviolanciati in corsa vicino
    for (const auto& l : S.lanci) {
        const auto* w = l.arma >= 0 ? trova(l.arma) : nullptr;
        if (!w || w->distrutta || w->persa) continue;
        for (const auto& [b, aerei] : cacciati)
            if (std::find(aerei.begin(), aerei.end(), l.aereo) != aerei.end())
                if (const double d = distanza(trova(b)->pos, w->pos); d <= SILURO_SENTITO_M) {
                    const DatiSiluro* ds = tabella_siluri_.siluro(l.tipo);
                    stimola(b, d <= SILURO_VICINO_M ? StimoloEvasione::siluro_vicino : StimoloEvasione::siluro, w->pos,
                            ds ? ds->quota_max_m : 0.0, l.arma);
                }
    }
    // sorvoli nella finestra di Snell (§8.3): uno stimolo per passaggio, con la probabilita' del giocatore
    for (const auto& [b, aerei] : cacciati) {
        const auto* v = trova(b);
        for (const int id : aerei) {
            const auto& a = S.aerei.at(id);
            const auto* u = trova(id);
            if (!u || a.c.volo().hover) continue;
            const bool dentro = distanza(u->pos, v->pos) <= finestra_snell_m(a.quota_m, v->quota, meteo_in(v->pos).stato_mare());
            const bool prima = S.sorvoli_evasione.count({id, b}) > 0;
            if (dentro && !prima && S.caso_evasione.uniforme() < probabilita_sorvolo(v->quota)) stimola(b, StimoloEvasione::sorvolo, u->pos, 0.0, id);
            if (dentro) S.sorvoli_evasione.insert({id, b});
            else S.sorvoli_evasione.erase({id, b});
        }
    }
}

}  // namespace m688
