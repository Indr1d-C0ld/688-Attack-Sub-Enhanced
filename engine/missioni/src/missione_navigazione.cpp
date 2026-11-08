// Navigazione incerta del giocatore nell'Enhanced (roadmap 26; docs/08 §6; dossier docs/ricerca/06 §2, §6, §7.3; nota
// condivisa navigazione-aggancio-contratto.md). Il modulo e' nucleo/navigazione.hpp; qui il collegamento:
// - il navigatore della classe nel teatro, dal seme, con la chiave di missione `navigazione`;
// - ogni passo la deriva alla latitudine del battello (inerziale guasto per la chiave);
// - l'ordine punto_nave: la valutazione sulle condizioni vere, l'attesa del satellite, l'esposizione (le condizioni
//   devono reggere fino alla fine), le emissioni (il radar della costa accende il radar del giocatore, il batimetrico
//   fa un impulso che l'IA sente), poi la correzione;
// - la stima per la carta e per il pilota automatico.
// Messaggi di bordo 470-474 (testi in tools/pipeline/esporta_motore.py). L'IA con l'errore di posizione: il navigatore
// dei sottomarini dell'IA, e le zone e le mete dei loro script sulla stima, fuori dai gestori d'esito.

#include <algorithm>
#include <cctype>
#include <cmath>
#include <map>
#include <sstream>

#include "missioni/missione.hpp"
#include "nucleo/fari.hpp"

extern "C" {
#include "lua.h"
}

namespace m688 {

namespace {

constexpr int MSG_FIX_FATTO = 470, MSG_FIX_SATELLITE = 471, MSG_FIX_IMPOSSIBILE = 472, MSG_FIX_INTERROTTO = 473,
              MSG_INERZIALE_GUASTO = 474;
constexpr double NOTTE_LUCE = 0.2;          // come i periscopi e l'artiglieria
constexpr double SERENO_OTTAVI = 3.0;       // per l'astronomico: le stelle con poche nubi
constexpr double METRI_PER_GRADO = 111120.0;
constexpr double METRI_PER_NM = 1852.0;
constexpr double QUALITA_BASE = 0.2, QUALITA_PER_MARE = 0.08, QUALITA_PER_KN = 0.01;   // STIMA
constexpr double GRADIENTE_M = 1000.0;                 // la pendenza della carta su +-1 km
constexpr double PIATTAFORMA_M = 200.0, ISOBATE_PIATTAFORMA_M = 20.0, ISOBATE_PROFONDO_M = 100.0;   // STIMA
constexpr double SPAZIATURA_MAX_NM = 20.0;             // il fondo piatto: nessuna isobata utile
constexpr double ERRORE_NON_RILEVATO_NM = 1.0;         // dove la carta d'epoca non ha rilievo (STIMA)
// il punto nave dai fari: l'occhio alla testa del periscopio; la portata nominale vale con 10 nm di visibilita'
constexpr double OCCHIO_PERISCOPIO_M = 1.5, VISIBILITA_NOMINALE_M = 10.0 * 1852.0;

// i fari del teatro (data/motore/fari.toml del kit), una volta per cartella e teatro
const std::vector<Faro>& fari_di(const std::string& cartella, const std::string& teatro) {
    static std::map<std::string, std::vector<Faro>> cache;
    const std::string k = cartella + "|" + teatro;
    auto it = cache.find(k);
    if (it == cache.end()) {
        std::vector<Faro> v;
        try {
            if (!cartella.empty()) v = carica_fari(cartella + "/fari.toml", teatro);
        } catch (const std::exception&) {}
        it = cache.emplace(k, std::move(v)).first;
    }
    return it->second;
}

}  // namespace

void Missione::navigazione_enhanced(const std::string& cartella_dati) {
    if (nav_ || !navigazione_attiva_) return;
    const auto* g = giocatore();
    if (!g) return;
    try {
        cat_nav_ = carica_navigazione(cartella_dati + "/navigazione.toml");
    } catch (const std::exception&) {
        navigazione_attiva_ = false;   // senza i dati: la posizione esatta di prima
        return;
    }
    lua_rawgeti(L_, LUA_REGISTRYINDEX, rif_missione_);
    lua_getfield(L_, -1, "navigazione");
    if (lua_istable(L_, -1)) {
        lua_getfield(L_, -1, "sigma_iniziale_nm");
        if (lua_isnumber(L_, -1)) sigma_iniziale_nm_ = std::max(0.0, lua_tonumber(L_, -1));
        lua_pop(L_, 1);
        lua_getfield(L_, -1, "inerziale_guasto");
        inerziale_guasto_ = lua_toboolean(L_, -1);
        lua_pop(L_, 1);
        lua_getfield(L_, -1, "gps");
        gps_ = lua_toboolean(L_, -1);
        lua_pop(L_, 1);
    }
    lua_pop(L_, 2);
    nav_.emplace(*cat_nav_, g->classe, teatro_, seme_ ^ 0x4E415649u, sigma_iniziale_nm_);
}

Vec2 Missione::errore_navigazione(int id) const {
    const auto* g = giocatore();
    if (nav_ && g && g->id == id) return nav_->errore();
    if (const auto it = nav_ia_.find(id); it != nav_ia_.end()) return it->second.errore();
    return {};
}

Vec2 Missione::posizione_stimata(int id) const {
    const auto it = unita_.find(id);
    if (it == unita_.end()) return {};
    const Vec2 e = errore_navigazione(id);
    return {it->second.pos.x + e.x, it->second.pos.y + e.y};
}

// I sottomarini dell'IA (docs/18, simmetria): lo stesso navigatore della classe; a quota periscopica con un albero fuori
// (o col filo) prendono il punto nave piu' preciso che la marina, il teatro e l'anno consentono, senza emissioni, al
// piu' uno ogni FIX_IA_OGNI_S; un satellite solo se passa adesso. L'errore entra nelle posizioni dei loro rapporti di
// contatto (fase 3 B). Le rotte dello script restano vere (la VM dell'originale non ha posizioni stimate).
// Le navi dell'IA (E della principale, 29/10/2026): lo stesso, a stima fra un punto nave e l'altro, con l'antenna sempre
// fuori e un punto nave al piu' ogni FIX_NAVE_OGNI_S (LORAN-C, Transit, Cajka, Parus; il GPS col 1990+): un errore
// piccolo al largo, che entra nei loro rapporti e nei missili oltre l'orizzonte. Non il traffico civile
void Missione::passo_navigazione_ia(double dt) {
    constexpr double FIX_IA_OGNI_S = 1800.0, FIX_NAVE_OGNI_S = 600.0, GIRO_S = 10.0;
    for (const auto& [id, u] : unita_) {
        const bool nave = u.categoria == "nave" && !del_traffico(id);
        if (u.giocatore || u.distrutta || u.persa || (u.categoria != "sottomarino" && !nave) || (u.flag & 0x20) || nav_ia_.count(id)) continue;
        nav_ia_.emplace(id, NavigazioneStimata(*cat_nav_, u.classe, teatro_, seme_ ^ (0x4E415649ull + static_cast<std::uint64_t>(id) * 0x9E37ull), sigma_iniziale_nm_));
    }
    nav_ia_giro_ += dt;
    if (nav_ia_giro_ < GIRO_S) return;
    const double passo = nav_ia_giro_;
    nav_ia_giro_ = 0.0;
    for (auto it = nav_ia_.begin(); it != nav_ia_.end();) {
        const auto u = unita_.find(it->first);
        if (u == unita_.end() || u->second.distrutta || u->second.persa) { it = nav_ia_.erase(it); continue; }
        const UnitaClassica& o = u->second;
        const double lat = latitudine_ + o.pos.y / METRI_PER_GRADO;
        it->second.avanza(passo, lat, !inerziale_ia_giu_.count(it->first));   // l'inerziale dai danni (DanniSottomarini)
        const bool nave = o.categoria == "nave";
        // sotto la banchisa ne' albero ne' filo: niente punto nave (come il giocatore, missione_ghiaccio.cpp)
        const bool esposto = nave || (((o.flag54 & 6) != 0 || o.quota <= 18.0) && !sotto_banchisa(it->first));
        if (esposto && tempo_ >= nav_ia_prossimo_fix_[it->first]) {
            CondizioniPuntoNave c;
            c.teatro = teatro_;
            c.marina = (o.flag & 0x10) ? "vmf" : "usn";
            const bool gps = gps_ || gps_opzione_;   // l'opzione «1990+» vale per tutto il mondo, come per il giocatore
            c.anno = gps ? std::max(anno_, 1990) : anno_;
            c.gps = gps;
            c.quota_m = o.quota;
            c.velocita_kn = std::fabs(o.velocita);
            c.albero = true;
            c.filo = true;
            const StatoMeteo m = meteo_in(o.pos);
            c.sereno = m.nubi_ottavi <= SERENO_OTTAVI;
            c.notte = luce_periscopi() < NOTTE_LUCE;
            c.errore_velocita_kn = it->second.sigma_velocita_kn();
            std::optional<ValutazionePuntoNave> meglio;
            std::string tipo;
            for (const auto& [pid, d] : cat_nav_->punti) {
                if (d.emissione || d.carta) continue;   // l'IA non si fa sentire per un punto nave
                const ValutazionePuntoNave v = valuta_punto_nave(d, c);
                if (!v.possibile) continue;
                if (d.satellite()) {
                    const double p = prossimo_passaggio_s(d, tempo_ - GIRO_S, lat, seme_ ^ (0x5A7E1ull + static_cast<std::uint64_t>(it->first)));
                    if (p < 0.0 || p > tempo_) continue;   // passa adesso, o niente
                }
                if (!meglio || v.sigma_nm < meglio->sigma_nm) { meglio = v; tipo = pid; }
            }
            // solo se serve: un punto nave meno preciso della stima non migliora niente
            if (meglio && meglio->sigma_nm * 1852.0 < it->second.sigma_m()) {
                const double prima = it->second.sigma_m();
                const Vec2 s = it->second.punto_nave(*meglio);
                // per il registro (punto_nave_ia, verita' del debriefing) e per R6 della sandbox
                punti_nave_ia_.push_back({tempo_, tipo, true, MotivoPuntoNave::nessuno, std::hypot(s.x, s.y), prima, it->second.sigma_m(), it->first});
                nav_ia_prossimo_fix_[it->first] = tempo_ + (nave ? FIX_NAVE_OGNI_S : FIX_IA_OGNI_S);
            }
        }
        ++it;
    }
}

std::optional<EllisseErrore> Missione::ellisse_navigazione(int id, double k) const {
    const auto* g = giocatore();
    if (!nav_ || !g || g->id != id) return std::nullopt;
    return nav_->ellisse(k);
}

std::optional<double> Missione::sigma_navigazione_m(int id) const {
    const auto* g = giocatore();
    if (nav_ && g && g->id == id) return nav_->sigma_m();
    if (const auto it = nav_ia_.find(id); it != nav_ia_.end()) return it->second.sigma_m();   // le unita' dell'IA col navigatore
    return std::nullopt;
}

CondizioniPuntoNave Missione::condizioni_punto_nave() const {
    CondizioniPuntoNave c;
    const auto* g = giocatore();
    if (!g) return c;
    c.teatro = teatro_;
    c.marina = (g->flag & 0x10) ? "vmf" : "usn";
    const bool gps = gps_ || gps_opzione_;   // la chiave di missione o l'opzione di partita «1990+»
    c.anno = gps ? std::max(anno_, 1990) : anno_;
    c.gps = gps;
    c.quota_m = g->quota;
    c.velocita_kn = std::fabs(g->velocita);
    c.albero = peri_ ? (testa_giocatore().has_value() || (g->flag54 & 4) != 0) : (g->flag54 & 6) != 0;
    c.filo = radio_ && radio_->filo();
    // sotto la banchisa (missione_ghiaccio.cpp) alberi e filo non arrivano alla superficie: resta il batimetrico
    if (sotto_banchisa(g->id)) c.albero = c.filo = false;
    const StatoMeteo m = meteo_in(g->pos);
    c.sereno = m.nubi_ottavi <= SERENO_OTTAVI;
    c.notte = luce_periscopi() < NOTTE_LUCE;
    // la costa: il primo punto di terra lungo 16 direzioni, a passi di 2 km fino a 60 km (dal fondale del teatro)
    if (fondale_) {
        for (double d = 2000.0; d <= 60000.0 && c.distanza_costa_km > 1e8; d += 2000.0)
            for (int k = 0; k < 16; ++k) {
                const double a = 2.0 * 3.14159265358979323846 * k / 16.0;
                if (profondita({g->pos.x + d * std::sin(a), g->pos.y + d * std::cos(a)}) <= 0.0) {
                    c.distanza_costa_km = d / 1000.0;
                    break;
                }
            }
    }
    // i fari accesi in vista dalla testa del periscopio: entro l'orizzonte della loro luce (2,08 nm x (radice h_faro +
    // radice h_occhio)) e dentro la portata, ridotta dalla visibilita' sotto i 10 nm (STIMA)
    if (fondale_ && c.notte)
        for (const Faro& f : fari_di(cartella_dati_traffico_, teatro_)) {
            const Vec2 p = fondale_->proiettata({f.lat, f.lon});
            const double d = distanza(g->pos, p);
            const double orizzonte = 2.08 * METRI_PER_NM * (std::sqrt(std::max(f.quota_m, 0.0)) + std::sqrt(OCCHIO_PERISCOPIO_M));
            const double portata = f.portata_m * std::min(1.0, m.visibilita_m / VISIBILITA_NOMINALE_M);
            if (d <= orizzonte && d <= portata) ++c.fari_in_vista;
        }
    if (nav_) c.errore_velocita_kn = nav_->sigma_velocita_kn();
    // la qualita' del fix dentro la forchetta della sua sigma: il mare e la velocita' la peggiorano (STIMA)
    c.qualita = std::clamp(QUALITA_BASE + QUALITA_PER_MARE * m.stato_mare() + QUALITA_PER_KN * c.velocita_kn, 0.0, 1.0);
    // la carta della sua parte: il batimetrico legge le isobate (la spaziatura dalla pendenza, la direzione debole lungo
    // l'isobata); i fix sulla carta (costa, visivo, batimetrico) portano l'errore del rilievo
    if (const Fondale* carta = fondale_carta((g->flag & 0x18) == 0x10 ? Parte::urss : Parte::nato)) {
        const Vec2 p = posizione_stimata(g->id);   // dove crede di essere: li' legge la carta
        const double gx = (carta->profondita({p.x + GRADIENTE_M, p.y}) - carta->profondita({p.x - GRADIENTE_M, p.y})) / (2.0 * GRADIENTE_M);
        const double gy = (carta->profondita({p.x, p.y + GRADIENTE_M}) - carta->profondita({p.x, p.y - GRADIENTE_M})) / (2.0 * GRADIENTE_M);
        const double pendenza = std::hypot(gx, gy), z = carta->profondita(p);
        const double intervallo = z < PIATTAFORMA_M ? ISOBATE_PIATTAFORMA_M : ISOBATE_PROFONDO_M;
        c.spaziatura_isobate_nm = pendenza > 1e-6 ? std::min(intervallo / pendenza / METRI_PER_NM, SPAZIATURA_MAX_NM) : SPAZIATURA_MAX_NM;
        c.direzione_debole_deg = normalizza_360(std::atan2(gx, gy) * 180.0 / 3.14159265358979323846 + 90.0);
        if (carta->carta_epoca()) {
            const int k = carta->campagna(p);
            c.errore_carta_nm = k >= 0 ? carta->campagne()[static_cast<std::size_t>(k)].spostamento_m / METRI_PER_NM : ERRORE_NON_RILEVATO_NM;
        }
    }
    return c;
}

std::vector<std::pair<std::string, ValutazionePuntoNave>> Missione::punti_nave_possibili() const {
    std::vector<std::pair<std::string, ValutazionePuntoNave>> v;
    if (!nav_ || !cat_nav_) return v;
    const CondizioniPuntoNave c = condizioni_punto_nave();
    for (const auto& [id, d] : cat_nav_->punti) {
        const ValutazionePuntoNave x = valuta_punto_nave(d, c);
        if (x.motivo == MotivoPuntoNave::marina) continue;   // non e' della sua marina: non e' nel menu
        v.emplace_back(id, x);
    }
    return v;
}

std::optional<Missione::PuntoNaveInCorso> Missione::punto_nave_in_corso() const {
    if (!nav_) return std::nullopt;
    PuntoNaveInCorso p;
    p.motivo = ultimo_motivo_;
    if (fix_) {
        p.tipo = fix_->tipo;
        p.in_corso = true;
        p.attesa_s = std::max(0.0, fix_->passaggio - tempo_);
        p.rimasto_s = std::max(0.0, fix_->fine - std::max(tempo_, fix_->passaggio));
    }
    return p;
}

bool Missione::avvia_punto_nave(const std::string& tipo) {
    const auto* g = giocatore();
    if (!nav_ || !cat_nav_ || !g) return false;
    const DefPuntoNave* d = cat_nav_->punto(tipo);
    if (!d) return false;
    const ValutazionePuntoNave v = valuta_punto_nave(*d, condizioni_punto_nave());
    // un satellite si aspetta anche se adesso le condizioni non ci sono: conta l'istante del passaggio
    const bool satellite = d->satellite();
    if (!v.possibile && !(satellite && (v.motivo == MotivoPuntoNave::albero || v.motivo == MotivoPuntoNave::quota ||
                                        v.motivo == MotivoPuntoNave::velocita))) {
        ultimo_motivo_ = v.motivo;
        punti_nave_.push_back({tempo_, tipo, false, v.motivo, 0.0, nav_->sigma_m(), nav_->sigma_m()});
        messaggio_di_bordo(g->id, MSG_FIX_IMPOSSIBILE);
        return false;
    }
    FixInCorso f;
    f.tipo = tipo;
    const double lat = latitudine_ + g->pos.y / METRI_PER_GRADO;
    f.passaggio = satellite ? prossimo_passaggio_s(*d, tempo_, lat, seme_ ^ 0x5A7E1u) : tempo_;
    if (f.passaggio < 0.0) f.passaggio = tempo_;
    f.fine = f.passaggio + std::max(1.0, d->durata_s);
    f.radar = d->emissione && tipo.find("radar") != std::string::npos;
    f.radar_prima = radar_giocatore_;
    if (f.radar && catalogo_el_ && !catalogo_el_->radar_di(g->classe).empty()) radar_giocatore_ = true;   // il radar della costa
    if (d->emissione && !f.radar) evento_acustico(g->id, std::max(1.0, d->durata_s));   // il batimetrico: un impulso che si sente
    ultimo_motivo_ = MotivoPuntoNave::nessuno;
    if (satellite && f.passaggio > tempo_ + 1.0) messaggio_di_bordo(g->id, MSG_FIX_SATELLITE);
    fix_ = f;
    return true;
}

void Missione::passo_navigazione(double dt) {
    if (nav_ && cat_nav_) passo_navigazione_ia(dt);
    const auto* g = giocatore();
    if (!nav_ || !g || g->distrutta || g->persa) return;
    const double lat = latitudine_ + g->pos.y / METRI_PER_GRADO;
    const bool ok = !inerziale_guasto_ && inerziale_danni_ok_;
    nav_->avanza(dt, lat, ok);
    if (!ok && !inerziale_detto_ && cat_nav_->sistema_di(g->classe).inerziale()) messaggio_di_bordo(g->id, MSG_INERZIALE_GUASTO);
    inerziale_detto_ = !ok;   // di nuovo, se torna su e poi cade ancora
    if (!fix_ || tempo_ < fix_->passaggio) return;
    // durante l'esposizione le condizioni devono reggere (il satellite: dal passaggio)
    const DefPuntoNave* d = cat_nav_->punto(fix_->tipo);
    const ValutazionePuntoNave v = d ? valuta_punto_nave(*d, condizioni_punto_nave()) : ValutazionePuntoNave{};
    const auto chiudi = [&] {
        if (fix_->radar && !fix_->radar_prima) radar_giocatore_ = false;
        fix_.reset();
    };
    if (!v.possibile) {
        ultimo_motivo_ = v.motivo;
        punti_nave_.push_back({tempo_, fix_->tipo, false, v.motivo, 0.0, nav_->sigma_m(), nav_->sigma_m()});
        messaggio_di_bordo(g->id, MSG_FIX_INTERROTTO);
        chiudi();
        return;
    }
    if (tempo_ < fix_->fine) return;
    const double prima = nav_->sigma_m();
    const Vec2 s = nav_->punto_nave(v);
    if (quadro_g_) quadro_g_->punto_nave(tempo_, s);   // il mosaico del fondale scorre con la correzione (missione_fondale.cpp)
    punti_nave_.push_back({tempo_, fix_->tipo, true, MotivoPuntoNave::nessuno, std::hypot(s.x, s.y), prima, nav_->sigma_m()});
    messaggio_di_bordo(g->id, MSG_FIX_FATTO);
    chiudi();
}

// I codici che danno un esito, come Missione::messaggio: 100-999 (i propri della missione da 1000 no), la fine del
// battello (25) e un'altra unita' distrutta (29)
static bool codice_esito(long c) { return (c >= 100 && c < 1000) || c == 25 || c == 29; }

GestoriEsito leggi_gestori_esito(const std::string& testo) {
    GestoriEsito g;
    std::map<std::string, std::set<std::string>> chiama;
    std::map<std::string, std::vector<std::string>> arrivi;
    const auto nome = [](const std::string& r, std::size_t k) {
        std::size_t e = k;
        while (e < r.size() && (std::isalnum(static_cast<unsigned char>(r[e])) || r[e] == '_')) ++e;
        return r.substr(k, e - k);
    };
    std::istringstream in(testo);
    std::string riga, corrente;
    bool in_funzione = false;
    while (std::getline(in, riga)) {
        if (!riga.empty() && riga.back() == '\r') riga.pop_back();
        if (riga.rfind("script.", 0) == 0 && riga.find("= function") != std::string::npos) {
            corrente = nome(riga, 7);
            in_funzione = true;
            continue;
        }
        if (riga.rfind("local function", 0) == 0 || riga.rfind("function", 0) == 0) { corrente.clear(); in_funzione = true; }
        if (riga == "end") { corrente.clear(); in_funzione = false; continue; }
        for (const char* f : {"m688.messaggio(", "m688.messaggio_al_giocatore("}) {
            const std::string chiave = f;
            for (std::size_t k = riga.find(chiave); k != std::string::npos; k = riga.find(chiave, k + 1)) {
                const std::size_t v = riga.find(',', k), c = riga.find(')', k);
                if (v == std::string::npos || c == std::string::npos || c < v) { g.leggibile = false; continue; }
                std::string a = riga.substr(v + 1, c - v - 1);
                a.erase(0, a.find_first_not_of(' '));
                a.erase(a.find_last_not_of(' ') + 1);
                const bool numero = !a.empty() && std::all_of(a.begin(), a.end(), [](char x) { return std::isdigit(static_cast<unsigned char>(x)); });
                if (!numero) { g.leggibile = false; continue; }
                if (!codice_esito(std::stol(a))) continue;
                if (corrente.empty()) g.leggibile = false;   // fuori da una funzione dello script: non si sa chi la chiama
                else g.esito.insert(corrente);
            }
        }
        if (!in_funzione || corrente.empty()) continue;
        for (std::size_t k = riga.find("script."); k != std::string::npos; k = riga.find("script.", k + 1))
            chiama[corrente].insert(nome(riga, k + 7));
        for (std::size_t k = riga.find("m688.vai_a("); k != std::string::npos; k = riga.find("m688.vai_a(", k + 1)) {
            const std::size_t c = riga.find(')', k), q2 = riga.rfind('"', c), q1 = q2 == std::string::npos || q2 == 0 ? std::string::npos : riga.rfind('"', q2 - 1);
            if (c != std::string::npos && q1 != std::string::npos && q1 > k) arrivi[corrente].push_back(riga.substr(q1 + 1, q2 - q1 - 1));
        }
    }
    for (bool cambiato = true; cambiato;) {   // chi chiama un gestore d'esito e' d'esito
        cambiato = false;
        for (const auto& [f, chiamate] : chiama)
            if (!g.esito.count(f))
                for (const auto& x : chiamate)
                    if (g.esito.count(x)) { g.esito.insert(f); cambiato = true; break; }
    }
    for (const auto& [f, lista] : arrivi)
        for (const auto& a : lista)
            if (g.esito.count(a)) g.vai_a_veri.emplace_back(f, a);
    return g;
}

Vec2 Missione::posizione_per_script(const UnitaClassica& u) const {
    if (!gestori_esito_.leggibile || u.giocatore || !nav_ia_.count(u.id)) return u.pos;
    if (!contesti_.empty() && gestori_esito_.esito.count(contesti_.back().gestore)) return u.pos;
    const Vec2 e = errore_navigazione(u.id);
    return {u.pos.x + e.x, u.pos.y + e.y};
}

// la meta di vai_a governata dalla stima: il battello va dove crede che sia, quindi arriva spostato del suo errore. Resta
// vera se l'arrivo da' un esito (se no girerebbe attorno a un punto che non raggiunge mai), e per l'inseguimento e
// vai_al_bersaglio, che puntano un'unita'
Vec2 Missione::meta_per_navigazione(const UnitaClassica& u, Vec2 meta) const {
    if (!gestori_esito_.leggibile || u.giocatore || u.insegui) return meta;
    const auto m = mete_assolute_.find(u.id);
    if (m == mete_assolute_.end() || m->second.x != meta.x || m->second.y != meta.y) return meta;
    if (!u.g_arrivo.empty() && gestori_esito_.esito.count(u.g_arrivo)) return meta;
    const auto n = nav_ia_.find(u.id);
    if (n == nav_ia_.end()) return meta;
    const Vec2 e = n->second.errore();
    return {meta.x - e.x, meta.y - e.y};
}

}  // namespace m688
