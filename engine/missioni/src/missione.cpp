#include "missioni/missione.hpp"
#include "missioni/asw_missione.hpp"
#include "missioni/artiglieria_missione.hpp"
#include "missioni/collisioni_missione.hpp"
#include "missioni/periscopi_missione.hpp"
#include "missioni/forze_missione.hpp"
#include "missioni/armi_ia_missione.hpp"

#include "nucleo/rotte.hpp"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <cstdio>
#include <deque>
#include <limits>
#include <mutex>
#include <stdexcept>
#include <vector>

extern "C" {
#include "lauxlib.h"
#include "lua.h"
#include "lualib.h"
}

namespace m688 {

namespace {

constexpr double PASSO_S = 1.0 / 9.0;    // passo della VM: 9 al secondo (sensori, moto e timer)
constexpr char FINE_SCRIPT[] = "__m688_fine_script__";   // terminazione voluta (es. SELECT_TGT senza bersaglio)

// Fondale (engine/README.md, "Fondale e incagli")
constexpr double FRANCO_IA_M = 7.62;          // l'IA tiene la chiglia dei sottomarini 25 piedi sopra il fondo (§9.3)
constexpr double ACQUA_NAVE_IA_M = 3.0;       // e cerca acqua di almeno pescaggio + 3 m per le navi
constexpr double ACQUA_SOTTOMARINO_IA_M = 10.0;
constexpr double QUOTA_STIMATA_SIGMA_M = 60.0;   // errore della quota di ricerca dei siluri aviolanciati (STIMA, A6.1)   //   e pescaggio + 10 m per i sottomarini
constexpr double ANTICIPO_FILO_DEG = 10.0;     // fase 4 B: la prua ordinata al lanciatore che guida a filo, avanti alla sua
constexpr double MANOVRA_IA_KN = 5.0;         // mentre aggira una secca l'IA rallenta per stringere la virata
constexpr double DISINCAGLIO_M = 50.0;        // l'incaglio finisce a 50 m dal punto in cui e' avvenuto
constexpr double URTO_SENZA_DANNI_M_S = 1.0;  // sotto 1 m/s (circa 2 nodi) l'urto col fondo non fa danni
constexpr double URTO_FATALE_M_S = 20.0;      // danno = integrita' del modello * (v / 20 m/s)^2 (30 nodi: 59%)
constexpr int ACQUE_APERTE_CELLE = 500;      // le unita' si collocano in acque collegate di almeno 20 km^2
// messaggi dell'originale (docs/originale/sensori_e_navigazione.md §9): 9 urto fatale (esito 1), 30 fondo
// strisciato con danni allo scafo, 38 siluro perso
constexpr int MSG_URTO_FATALE = 9, MSG_FONDO_DANNI = 30, MSG_SILURO_PERSO = 38, ESITO_INCAGLIATO = 1;
// colpi e distruzione (§9.4, §9.8): messaggi al giocatore locale
constexpr int MSG_A_SEGNO = 7, MSG_BOMBA = 20, MSG_FINE = 25, MSG_COLPITI = 26, MSG_ALTRO_COLPITO = 28,
              MSG_ALTRO_DISTRUTTO = 29, MSG_MISSILE_A_SEGNO = 50, MSG_EMERSIONE = 53, MSG_ALTRO_BOMBA = 56,
              MSG_FUOCO_AMICO = 67;
constexpr int ESITO_DISTRUTTO = 2, ESITO_ALTRO_DISTRUTTO = 4;
// controlli dei giocatori e navigazione (§9.6)
constexpr int MSG_PERISCOPIO_GIU = 3, MSG_STRATO_TERMICO = 31, MSG_ANTENNA_GIU = 37, MSG_REGOLO_VELOCITA = 45,
              MSG_RIDUCO_PER_ACCOSTARE = 47;
constexpr double QUOTA_100_PIEDI_M = 30.48, QUOTA_PERISCOPICA_M = 6.096;
constexpr double TOLLERANZA_PERISCOPICA_M = 1.0;     // la dinamica oscilla attorno alla quota ordinata (Enhanced)
constexpr double QUOTA_PERISCOPICA_DINAMICA_M = 5.5;  // l'ordine quota periscopica con la dinamica (PostoEnhanced, comandante)
constexpr double FINESTRA_IMPATTO_QUARTI = 20.0;    // |dx| e |dy| < 20 quarti di unita' (5 unita')
constexpr double FINESTRA_IMPATTO_QUOTA_M = 30.48;  // |dquota| < 800 ottavi di piede (100 ft)
constexpr double QUOTA_RELITTO_M = 0.762;           // sotto 20 ottavi (2,5 ft) il giocatore distrutto resta a galla
constexpr double TRAFFICO_CAMPO_M = 30000.0;   // oltre, il traffico senza campo di raggi (livello A)
constexpr unsigned TRAFFICO_OGNI_TURNI = 4;     // i sensori valutano il traffico un turno su 4 (passo_sensori)
constexpr int CODICI_MISSIONE = 1000;               // E6: i messaggi di bordo propri della missione da qui (1000 + n)
// la navigazione incerta dell'IA nel pilota (acqua_ia, pilota): l'acqua in piu' per la sigma e la vista piu' lunga
constexpr double MARGINE_INCERTO_SIGMA_PER_M = 20.0, MARGINE_INCERTO_MAX_M = 40.0, PORTATA_INCERTA_MAX_M = 5000.0;
// A2: la profondita' creduta prudente (il fondo meno PRUDENZA_SIGMA sigma della cella) e il rallentamento in acqua bassa
// con la posizione incerta (STIMA)
constexpr double PRUDENZA_SIGMA = 1.0, SIGMA_RALLENTA_M = 500.0, ACQUA_RALLENTA_M = 100.0;
// A2: le rotte sulla carta con le secche allargate di RAGGIO_ROTTA_SIGMA sigma (al piu' RAGGIO_ROTTA_MAX_M, a passi di
// RAGGIO_PASSO_M); se cosi' il passaggio si chiude (uno stretto) la carta com'e'
constexpr double RAGGIO_ROTTA_SIGMA = 1.5, RAGGIO_ROTTA_MAX_M = 3000.0, RAGGIO_PASSO_M = 500.0;

// la carta col fondo di ogni cella al minimo entro il raggio (un quadrato: due passate a finestra mobile)
Fondale allarga_secche(const Fondale& f, double raggio_m) {
    const int w = f.larghezza(), h = f.altezza(), k = static_cast<int>(std::ceil(raggio_m / f.cella_m()));
    std::vector<double> d(static_cast<std::size_t>(w) * h), t(d.size());
    for (int r = 0; r < h; ++r)
        for (int c = 0; c < w; ++c) d[static_cast<std::size_t>(r) * w + c] = f.profondita_cella(c, r);
    const auto passata = [&](const std::vector<double>& in, std::vector<double>& out, bool righe) {
        const int n = righe ? w : h, m = righe ? h : w;
        std::deque<int> q;
        for (int j = 0; j < m; ++j) {
            const auto at = [&](int i) -> double { return righe ? in[static_cast<std::size_t>(j) * w + i] : in[static_cast<std::size_t>(i) * w + j]; };
            q.clear();
            for (int i = 0, e = 0; i < n; ++i) {
                for (; e < std::min(n, i + k + 1); ++e) {
                    while (!q.empty() && at(q.back()) >= at(e)) q.pop_back();
                    q.push_back(e);
                }
                while (q.front() < i - k) q.pop_front();
                (righe ? out[static_cast<std::size_t>(j) * w + i] : out[static_cast<std::size_t>(i) * w + j]) = at(q.front());
            }
        }
    };
    passata(d, t, true);
    passata(t, d, false);
    constexpr double SCALA = 0.25, OFFSET = -11000.0;   // come i .fdl
    std::vector<std::uint16_t> v(d.size());
    for (std::size_t i = 0; i < d.size(); ++i) v[i] = static_cast<std::uint16_t>(std::clamp(std::round((-d[i] - OFFSET) / SCALA), 0.0, 65535.0));
    return Fondale(w, h, f.cella_m(), SCALA, OFFSET, std::move(v));
}
constexpr double QUOTA_SUPERFICIE_M = 0.381;        // in superficie per la scoperta automatica (10 ottavi di piede)

bool segue_il_fondale(const UnitaClassica& u) { return u.categoria == "nave" || u.categoria == "sottomarino"; }

// --- lettura della tabella della missione -------------------------------------------------------------------

double campo_numero(lua_State* L, int t, const char* k, double def) {
    lua_getfield(L, t, k);
    const double v = lua_isnumber(L, -1) ? lua_tonumber(L, -1) : def;
    lua_pop(L, 1);
    return v;
}

std::string campo_stringa(lua_State* L, int t, const char* k, const std::string& def = {}) {
    lua_getfield(L, t, k);
    std::string v = lua_type(L, -1) == LUA_TSTRING ? lua_tostring(L, -1) : def;
    lua_pop(L, 1);
    return v;
}

bool campo_bool(lua_State* L, int t, const char* k) {
    lua_getfield(L, t, k);
    const bool v = lua_toboolean(L, -1);
    lua_pop(L, 1);
    return v;
}

// --- binding m688: accesso al runtime e agli argomenti -------------------------------------------------------

Missione& M(lua_State* L) { return *static_cast<Missione*>(lua_touserdata(L, lua_upvalueindex(1))); }
double num(lua_State* L, int i, double def = 0.0) { return lua_isnumber(L, i) ? lua_tonumber(L, i) : def; }
std::string str(lua_State* L, int i) { return lua_type(L, i) == LUA_TSTRING ? lua_tostring(L, i) : std::string{}; }
int vero(lua_State* L, bool v) { lua_pushboolean(L, v); return 1; }

UnitaClassica& B(lua_State* L) { return M(L).contesto().buffer; }
UnitaClassica* bersaglio(lua_State* L) {
    const int b = B(L).bersaglio;
    return b >= 0 ? M(L).trova(b) : nullptr;
}
double metri_da_nm(lua_State* L, double nm) { return nm * 50.0 * M(L).metri_per_unita(); }   // 1 nm di gioco = 50 unita'
bool entro_per_asse(Vec2 a, Vec2 b, double soglia) { return std::fabs(a.x - b.x) <= soglia && std::fabs(a.y - b.y) <= soglia; }
// la fascia di tiro dei lanciarazzi antisom che non arrivano alle 2 nm dell'ATTACK: il Terne III della Oslo, 425-1600 m
// (Wikipedia «Terne ASW»; designation-systems.net da' 400-5000 m e avverte che le fonti discordano): la carica da 50 kg
// resta il colpo a dado dello script, come l'RBU della Koni
std::optional<std::pair<double, double>> fascia_lanciarazzi(const std::string& classe) {
    if (classe == "oslo") return std::pair{425.0, 1600.0};
    return std::nullopt;
}

using Funzione = int (*)(lua_State*);
struct Voce { const char* nome; Funzione f; };

// -- condizioni
int f_bersaglio_id(lua_State* L) { return vero(L, B(L).bersaglio == static_cast<int>(num(L, 2))); }
int f_bersaglio_arma(lua_State* L) { auto* t = bersaglio(L); return vero(L, t && ((t->flag & 0x20) || t->categoria == "arma")); }
int f_bersaglio_nave(lua_State* L) { auto* t = bersaglio(L); return vero(L, t && t->categoria == "nave"); }
int f_bersaglio_aereo(lua_State* L) { auto* t = bersaglio(L); return vero(L, t && t->categoria == "aereo"); }
// M7: la parte del luogo (18h) e' nemica di chi e' con lei in crisi o in guerra, come Missione::ostili. Con l'opcode
// dell'originale (i due bit diversi) le erano nemici i neutrali e mai le parti in crisi: le fregate della S16 non
// cacciavano l'Alfa (trailer-1, 30/10/2026). Senza la parte del luogo resta l'opcode dell'originale
int f_bersaglio_nemico(lua_State* L) {
    auto* t = bersaglio(L);
    if (!t) return vero(L, false);
    const int la = B(L).flag & 0x18, lb = M(L).parte_nota(B(L), *t);
    if (la == 0x18 || lb == 0x18) return vero(L, la != lb && M(L).con_forze() && M(L).situazione_fra(la, lb) != Situazione::pace);
    return vero(L, (la ^ lb) == 0x18);
}
int f_bersaglio_neutrale(lua_State* L) { auto* t = bersaglio(L); return vero(L, t && (t->flag & 0x18) == 0); }
int f_bersaglio_mi_punta(lua_State* L) { auto* t = bersaglio(L); return vero(L, t && t->bersaglio == M(L).ultima_trovata()); }
int f_bersaglio_vicino(lua_State* L) {
    auto* t = bersaglio(L);
    return vero(L, t && entro_per_asse(B(L).pos, t->pos, metri_da_nm(L, num(L, 2))));
}
int traccia(lua_State* L, double Contatto::*campo, double soglia) {
    const auto& c = B(L).contatti;
    const auto it = c.find(B(L).bersaglio);
    return vero(L, it != c.end() && it->second.*campo >= soglia);
}
int f_traccia_solida(lua_State* L) { return traccia(L, &Contatto::qualita, 800.0); }
int f_traccia_media(lua_State* L) { return traccia(L, &Contatto::durata, 800.0); }
int f_traccia_lunga(lua_State* L) { return traccia(L, &Contatto::durata, 1400.0); }
// Enhanced: il bersaglio ha una posizione stimata (eco, vista, radar o la soluzione della TMA dell'IA), non il solo
// rilevamento. Nel classico la posizione la da' la traccia solida
int f_soluzione(lua_State* L) {
    const auto& c = B(L).contatti;
    const auto it = c.find(B(L).bersaglio);
    return vero(L, it != c.end() && (M(L).con_acustica() ? it->second.posizione_nota : it->second.qualita >= 800.0));
}
int f_caso(lua_State* L) { return vero(L, M(L).casuale().uniforme() < num(L, 2)); }
int f_unita_esiste(lua_State* L) {
    const int id = static_cast<int>(num(L, 2));
    const bool c = M(L).trova(id) != nullptr;
    if (c) M(L).ultima_trovata() = id;
    return vero(L, c);
}
// IF_HIT (52h, 2011:1C29): colpita (+5E) o in affondamento (+55)
int f_unita_colpita(lua_State* L) { auto* u = M(L).trova(static_cast<int>(num(L, 2))); return vero(L, u && (u->colpo != 0 || u->affondamento != 0)); }
int f_unita_id(lua_State* L) { return vero(L, B(L).id == static_cast<int>(num(L, 2))); }
// le prove di zona dello script sull'unita' corrente: un sottomarino dell'IA col navigatore le fa sulla sua stima
// (fuori dai gestori d'esito, Missione::posizione_per_script)
int f_x_maggiore(lua_State* L) { return vero(L, M(L).posizione_per_script(B(L)).x > num(L, 2)); }
int f_in_rettangolo(lua_State* L) {
    const Vec2 p = M(L).posizione_per_script(B(L));
    return vero(L, p.x >= num(L, 2) && p.y <= num(L, 3) && p.x <= num(L, 4) && p.y >= num(L, 5));
}
int f_variabile_uguale(lua_State* L) { return vero(L, M(L).variabili()[static_cast<int>(num(L, 2))] == static_cast<int>(num(L, 3))); }
int f_variabile_maggiore(lua_State* L) { return vero(L, M(L).variabili()[static_cast<int>(num(L, 2))] > static_cast<int>(num(L, 3))); }
int f_variabile_minore(lua_State* L) { return vero(L, M(L).variabili()[static_cast<int>(num(L, 2))] < static_cast<int>(num(L, 3))); }
int f_confronta_variabili(lua_State* L) {   // IF_VARCMP (difettoso nell'originale, docs/04 §4.1): uguaglianza
    auto& v = M(L).variabili();
    return vero(L, v[static_cast<int>(num(L, 2))] == v[static_cast<int>(num(L, 3))]);
}
int f_flag_scena_uguale(lua_State* L) { return vero(L, B(L).flag_scena == static_cast<int>(num(L, 2))); }
int f_flag_scena_bit(lua_State* L) { return vero(L, (B(L).flag_scena & static_cast<int>(num(L, 2))) != 0); }
int f_giocatore_secondo(lua_State* L) { return vero(L, false); }   // partita a due giocatori: non nel motore
int f_livello_standard(lua_State* L) { return vero(L, true); }
int f_manomesso(lua_State* L) { return vero(L, false); }            // controllo anti-manomissione dell'originale

// -- manovra
// Rotte in gradi bussola e virate con il positivo a dritta: il convertitore traduce gli angoli matematici
// dell'originale (docs/originale/numeri_eseguibile.md §3.3-§4). Nell'originale una virata con il rateo (+36) si
// ferma quando la prua passa per la rotta ordinata (+34, 2011:09FC-0A20), salvo con HEAD_TO -1 (rotta libera).
// HEAD_TO (24h): solo la rotta ordinata; il rateo resta quello che era (0: l'accostata del motore, verso piu' breve)
int f_rotta(lua_State* L) {
    auto& b = B(L);
    if (lua_isnumber(L, 2)) { b.rotta_ordinata = normalizza_360(num(L, 2)); b.rotta_libera = false; }
    else b.rotta_libera = true;
    return 0;
}
int f_rotta_ora(lua_State* L) {
    if (lua_isnumber(L, 2)) { auto& b = B(L); b.rotta = b.rotta_ordinata = normalizza_360(num(L, 2)); b.rateo_virata = 0; b.rotta_libera = false; }
    return 0;
}
// TURN_REL (63h, 1E9E): rotta + valore nel verso piu' breve al rateo standard
int f_virata_relativa(lua_State* L) {
    if (lua_isnumber(L, 2)) { auto& b = B(L); b.rotta_ordinata = normalizza_360(b.rotta + num(L, 2)); b.rateo_virata = 0; b.rotta_libera = false; }
    return 0;
}
int f_rateo_virata(lua_State* L) { B(L).rateo_virata = num(L, 2); return 0; }
// TURN_TO (19h, 2011:167F): rotta ordinata con il rateo standard +3E, che e' positivo nell'originale (antiorario):
// l'accostata va sempre a sinistra, anche quando a dritta sarebbe piu' breve
int f_accosta_a(lua_State* L) {
    if (lua_isnumber(L, 2)) { auto& b = B(L); b.rotta_ordinata = normalizza_360(num(L, 2)); b.rateo_virata = -b.virata; b.rotta_libera = false; }
    return 0;
}
int f_telegrafo(lua_State* L) {
    auto& b = B(L);
    if (lua_type(L, 2) == LUA_TSTRING) b.velocita_ordinata = 0.0;   // arresto rapido
    else b.velocita_ordinata = std::min(num(L, 2), 280.0 / 256.0) * b.avanti_tutta;
    return 0;
}
int f_velocita_ora(lua_State* L) { B(L).velocita = B(L).velocita_ordinata = num(L, 2); return 0; }
// DEPTH_TO (22h, 2011:175D): quota ordinata (al piu' la quota massima +5A) e rateo in 1/256 del rateo del modello;
// rateo 0 = nessun cambio di quota (timoni orizzontali a zero); se il battello e' gia' a quella quota non cambia nulla
int f_quota(lua_State* L) {
    auto& b = B(L);
    const double q = std::min(num(L, 2), b.quota_max);
    if (std::fabs(b.quota - q) < 1e-9) return 0;
    b.quota_ordinata = q;
    b.rateo_quota = num(L, 3, 0.0) * b.rateo_quota_modello;
    return 0;
}
int f_quota_ora(lua_State* L) { B(L).quota = B(L).quota_ordinata = num(L, 2); B(L).rateo_quota = 0; return 0; }
int f_azzera_rateo_quota(lua_State* L) { B(L).quota_ordinata = B(L).quota; B(L).rateo_quota = 0; return 0; }
int f_accosta_verso_bersaglio(lua_State* L) {
    if (auto* t = bersaglio(L)) { B(L).rotta_ordinata = rilevamento(B(L).pos, t->pos); B(L).rateo_virata = 0; B(L).rotta_libera = false; }
    return 0;
}
int f_accosta_opposto(lua_State* L) {
    if (auto* t = bersaglio(L)) { B(L).rotta_ordinata = normalizza_360(rilevamento(B(L).pos, t->pos) + 180.0); B(L).rateo_virata = 0; B(L).rotta_libera = false; }
    return 0;
}
int f_uguaglia_velocita(lua_State* L) { if (auto* t = bersaglio(L)) B(L).velocita_ordinata = std::min(t->velocita, B(L).avanti_tutta * 280.0 / 256.0); return 0; }
int f_uguaglia_quota(lua_State* L) { if (auto* t = bersaglio(L)) B(L).quota_ordinata = std::min(t->quota, B(L).quota_max); return 0; }
int f_ferma_navigazione(lua_State* L) { B(L).navigazione = B(L).insegui = false; B(L).rateo_virata = 0; return 0; }
int f_riprendi_navigazione(lua_State* L) { B(L).navigazione = true; return 0; }
int f_navigazione(lua_State* L) { B(L).navigazione = num(L, 2) != 0.0; return 0; }
int f_vai_a(lua_State* L) {
    auto& b = B(L);
    b.meta = {num(L, 2), num(L, 3)};
    b.soglia_arrivo = static_cast<int>(num(L, 4, 30.0));
    b.g_arrivo = str(L, 5);
    b.navigazione = true; b.insegui = false; b.rateo_virata = 0; b.rotta_libera = false;
    M(L).meta_assoluta(b.id, b.meta);   // un punto fisso: col navigatore dell'IA si governa dalla stima
    return 0;
}
int f_vai_al_bersaglio(lua_State* L) {
    auto& b = B(L);
    if (auto* t = bersaglio(L)) {
        b.meta = t->pos;
        if (M(L).controllato_asw(b.id)) {   // ASW aereo dell'Enhanced: il punto del bersaglio diventa un datum
            Cue c;
            c.tipo = t->colpo ? Cue::Tipo::nave_colpita : Cue::Tipo::contatto_nave;
            c.pos = t->pos;
            c.t = M(L).tempo();
            c.raggio_m = 1852.0;
            c.unita = t->id;
            M(L).cue_asw(b.id, c);
        }
    }
    b.soglia_arrivo = static_cast<int>(num(L, 2, 60.0));
    b.g_arrivo = str(L, 3);
    b.navigazione = true; b.insegui = false; b.rateo_virata = 0; b.rotta_libera = false;
    return 0;
}
int f_insegui(lua_State* L) {
    auto& b = B(L);
    b.soglia_arrivo = static_cast<int>(num(L, 2, 0.0));   // 0: il siluro insegue finche' colpisce o finisce la corsa
    b.g_arrivo = str(L, 3); b.g_reazione = str(L, 4);
    b.navigazione = b.insegui = true; b.rateo_virata = 0; b.rotta_libera = false;
    b.inseguito = b.bersaglio;
    return 0;
}
int f_teletrasporta_al_bersaglio(lua_State* L) {
    if (auto* t = bersaglio(L)) B(L).pos = {t->pos.x + 45.0 * M(L).metri_per_unita(), t->pos.y + 45.0 * M(L).metri_per_unita()};
    return 0;
}
int f_posizione(lua_State* L) { B(L).pos = {num(L, 2), num(L, 3)}; return 0; }
// ASW aereo dell'Enhanced: cue_asw(u, aereo, x, y [, raggio_m [, ritardo_s [, rotta, velocita_kn]]]): un datum per
// l'aereo (-1: tutti gli aerei controllati); nel classico non fa niente
int f_cue_asw(lua_State* L) {
    Missione& m = M(L);
    Cue c;
    c.tipo = Cue::Tipo::sosus;
    c.pos = {num(L, 3), num(L, 4)};
    c.raggio_m = num(L, 5, 1852.0);
    c.t = m.tempo() - num(L, 6, 0.0);
    if (lua_isnumber(L, 7)) c.rotta_deg = lua_tonumber(L, 7);
    if (lua_isnumber(L, 8)) c.velocita_kn = lua_tonumber(L, 8);
    m.cue_asw(static_cast<int>(num(L, 2, -1.0)), c);
    return 0;
}
// Condizioni delle rapide v2 e della libreria (docs/17 §3), sull'unita' corrente u; valgono anche nel classico
int f_in_cerchio(lua_State* L) { return vero(L, distanza(M(L).posizione_per_script(B(L)), Vec2{num(L, 2), num(L, 3)}) <= num(L, 4)); }
int f_velocita_sotto(lua_State* L) { return vero(L, B(L).velocita < num(L, 2)); }
int f_quota_entro(lua_State* L) { const double q = B(L).quota; return vero(L, q >= num(L, 2) && q <= num(L, 3)); }
int f_in_superficie(lua_State* L) { return vero(L, B(L).quota <= QUOTA_SUPERFICIE_M); }
int f_alberi_alzati(lua_State* L) { return vero(L, (B(L).flag54 & 6) != 0); }   // periscopio o antenna
int f_obiettivo_compiuto(lua_State* L) { return vero(L, M(L).obiettivo_compiuto(str(L, 2))); }
// u ha avuto `minuti` di contatto visibile, cumulativi, sull'unita' `id`; u non vede `id` da `minuti`
int f_contatto_da(lua_State* L) { return vero(L, M(L).contatto_cumulato_s(B(L).id, static_cast<int>(num(L, 2))) >= 60.0 * num(L, 3)); }
int f_senza_contatto_da(lua_State* L) { return vero(L, M(L).senza_contatto_s(B(L).id, static_cast<int>(num(L, 2))) >= 60.0 * num(L, 3)); }
// il giocatore ha fatto ping entro `raggio_m` dall'unita' u (negli ultimi `ultimi_s` secondi: predefinito tutta la
// missione)
int f_ping_giocatore_entro(lua_State* L) {
    const Missione& m = M(L);
    const Vec2 p = B(L).pos;
    const double r = num(L, 2), da = m.tempo() - num(L, 3, 1e18);
    for (const auto& e : m.registro().eventi())
        if (e.tipo == TipoEvento::ping_attivo && e.tempo >= da && distanza(e.pos, p) <= r) return vero(L, true);
    return vero(L, false);
}
// m688.comanda_mine(u [, gruppo]): le mine comandate da terra del gruppo (senza: tutte) passano alla stazione (la S18); vero
// se ce n'era almeno una
int f_comanda_mine(lua_State* L) { return vero(L, M(L).comanda_mine(lua_gettop(L) >= 2 && lua_isstring(L, 2) ? str(L, 2) : std::string()) > 0); }
// m688.distanza_da(u [, id]): i metri veri dall'unita' corrente all'unita' `id` (o al bersaglio), per le regole delle
// missioni (fanno da arbitro: la posizione vera); nil se l'altra non c'e', e' distrutta o fuori dal teatro (persa)
int f_distanza_da(lua_State* L) {
    const Missione& m = M(L);
    const UnitaClassica& u = B(L);
    const auto* t = bersaglio(L);
    const int id = lua_gettop(L) >= 2 && lua_isnumber(L, 2) ? static_cast<int>(num(L, 2)) : t ? t->id : -1;
    const auto it = id >= 0 ? m.unita().find(id) : m.unita().end();
    if (it == m.unita().end() || it->second.distrutta || it->second.persa) { lua_pushnil(L); return 1; }
    lua_pushnumber(L, distanza(u.pos, it->second.pos));
    return 1;
}
// l'unita' `id` ha fatto ping (negli ultimi `ultimi_s` secondi: predefinito tutta la missione); -1 una qualunque
// altra da u
int f_ha_pingato(lua_State* L) {
    const Missione& m = M(L);
    const int id = static_cast<int>(num(L, 2, -1)), io = B(L).id;
    const double da = m.tempo() - num(L, 3, 1e18);
    for (const auto& p : m.ping_unita())
        if (p.t >= da && (id >= 0 ? p.unita == id : p.unita != io)) return vero(L, true);
    return vero(L, false);
}
// un'unita' qualunque (non u) ha fatto ping entro `raggio_m` da u (negli ultimi `ultimi_s` secondi), come
// ping_giocatore_entro per tutti
int f_ping_entro(lua_State* L) {
    const Missione& m = M(L);
    const Vec2 q = B(L).pos;
    const int io = B(L).id;
    const double r = num(L, 2), da = m.tempo() - num(L, 3, 1e18);
    for (const auto& p : m.ping_unita())
        if (p.t >= da && p.unita != io && distanza(p.pos, q) <= r) return vero(L, true);
    return vero(L, false);
}
// un'unita' ostile al giocatore ha lanciato un'arma: l'unita' `id`, o una qualunque con -1 (predefinito)
// E6: un messaggio radio della missione per id, subito in onda (m688.radio(u, "ID")); vero se e' partito
int f_radio(lua_State* L) { return vero(L, M(L).manda_radio(str(L, 2))); }
// mine dell'IA: m688.posa_mina(u, tipo [, quante, ogni_s]): l'unita' u posa mine sotto di se' (Enhanced)
// S10: m fra lo scafo dell'unita' u e il cavo della schiera della nave `id` (-1 se non fila o non c'e')
int f_distanza_cavo(lua_State* L) {
    lua_pushnumber(L, M(L).distanza_dal_cavo(B(L).id, static_cast<int>(num(L, 2))));
    return 1;
}
int f_posto_di_combattimento(lua_State* L) { M(L).posto_di_combattimento(B(L).id); return 0; }   // artiglieria
int f_posa_mina(lua_State* L) {
    return vero(L, M(L).posa_mina_ia(B(L).id, str(L, 2), static_cast<int>(num(L, 3, 1.0)), num(L, 4, 60.0)));
}
int f_ha_lanciato(lua_State* L) {
    const int id = static_cast<int>(num(L, 2, -1.0));
    for (const auto& e : M(L).registro().eventi())
        if (e.tipo == TipoEvento::lancio_nemico && (id < 0 || e.unita == id)) return vero(L, true);
    return vero(L, false);
}
bool argomento_vero(lua_State* L, int i) { return lua_isboolean(L, i) ? lua_toboolean(L, i) != 0 : num(L, i) != 0.0; }
int f_bqs15(lua_State* L) {   // S06: m688.bqs15(u [, acceso]); falso se l'unita' non lo ha
    return vero(L, M(L).bqs15(B(L).id, lua_gettop(L) < 2 || argomento_vero(L, 2)));
}
// M6: avarie, incagli e rimorchi dell'IA (missione_avaria.cpp)
int f_avaria(lua_State* L) { return vero(L, M(L).avaria(B(L).id, lua_gettop(L) < 2 || argomento_vero(L, 2))); }
int f_incaglia(lua_State* L) { return vero(L, M(L).incaglia(B(L).id)); }
// m688.pedina_vicino(u, bersaglio [, distanza_m, sopra_m]): il pedinamento ravvicinato nel cono sordo (S24); bersaglio
// -1 smette. Nel classico non fa niente
int f_pedina_vicino(lua_State* L) {
    if (!M(L).con_acustica()) return vero(L, false);
    return vero(L, M(L).pedina_vicino(B(L).id, static_cast<int>(num(L, 2)), num(L, 3, 400.0), num(L, 4, 0.0)));
}
int f_rimorchia(lua_State* L) {   // m688.rimorchia(rimorchiatore, rimorchiato [, velocita_kn, cavo_m])
    return vero(L, M(L).rimorchia(B(L).id, static_cast<int>(num(L, 2)), num(L, 3, 6.0), num(L, 4, 300.0)));
}
int f_molla_rimorchio(lua_State* L) { return vero(L, M(L).molla_rimorchio(B(L).id)); }
int f_in_avaria(lua_State* L) { return vero(L, M(L).in_avaria(B(L).id) || M(L).incagliata_ia(B(L).id)); }
int f_rimorchiata(lua_State* L) { return vero(L, M(L).rimorchiatore_di(B(L).id) >= 0); }
int f_in_acque_territoriali(lua_State* L) { return vero(L, M(L).acque_territoriali_di(B(L)) >= 0); }   // straniere
int f_esercitazione(lua_State* L) {   // m688.esercitazione(u [, si]): l'arma u inerte, o le armi che u lancera'
    return vero(L, M(L).esercitazione(B(L).id, lua_gettop(L) < 2 || argomento_vero(L, 2)));
}
// Forze dell'Enhanced (fase 3, docs/18): nil nel classico, cosi' la libreria ricade sulle sue costanti
int lato_da_nome(const std::string& s) { return s == "usa" ? 0x08 : s == "urss" ? 0x10 : s == "neutrale" ? 0 : s == "luogo" ? 0x18 : -1; }
int f_allerta(lua_State* L) {   // m688.allerta(u): 0-3, l'allerta della formazione dell'unita' o della sua forza
    const auto a = M(L).allerta_unita(B(L).id);
    if (!a) { lua_pushnil(L); return 1; }
    lua_pushinteger(L, *a);
    return 1;
}
int f_armi_libere(lua_State* L) {   // m688.armi_libere(u [, bersaglio]): le ROE dell'IA sulla parte creduta
    const auto a = M(L).armi_libere_ia(B(L).id, lua_gettop(L) >= 2 ? static_cast<int>(num(L, 2)) : B(L).bersaglio);
    if (!a) { lua_pushnil(L); return 1; }
    return vero(L, *a);
}
int f_in_zona_amica(lua_State* L) {   // m688.in_zona_amica(u [, bersaglio]): dentro l'area d'azione di un sottomarino amico
    return vero(L, M(L).in_zona_amica(B(L).id, lua_gettop(L) >= 2 ? static_cast<int>(num(L, 2)) : B(L).bersaglio));
}
int f_bersaglio_non_combattente(lua_State* L) {   // m688.bersaglio_non_combattente(u): flag 40h; nil nel classico
    if (!M(L).con_acustica()) { lua_pushnil(L); return 1; }
    const auto* t = bersaglio(L);
    return vero(L, t && (t->flag & 0x40) != 0);
}
// m688.classificato(u [, b]): il sonarista dell'unita' ha classificato il bersaglio (o b), o il contatto e' lungo; nil nel
// classico
int f_classificato(lua_State* L) {
    if (!M(L).con_acustica()) { lua_pushnil(L); return 1; }
    const auto& u = B(L);
    const auto* t = bersaglio(L);
    const int b = lua_gettop(L) >= 2 && lua_isnumber(L, 2) ? static_cast<int>(num(L, 2)) : t ? t->id : -1;
    if (b < 0) return vero(L, false);
    // il terzo argomento: la confidenza minima (0 NONSUB, 1 POSSUB, 2 PROBSUB, 3 CERTSUB); senza, qualunque
    const int minima = lua_gettop(L) >= 3 && lua_isnumber(L, 3) ? static_cast<int>(num(L, 3)) : 0;
    bool si = false;
    for (const auto& k : M(L).classificazioni_ia())
        si = si || (k.unita == u.id && k.sorgente == b && static_cast<int>(k.confidenza) >= minima);
    if (const auto c = u.contatti.find(b); c != u.contatti.end() && c->second.durata >= 1400.0) si = true;   // il contatto lungo resta
    return vero(L, si);
}
// m688.salvati(u, n, di): n uomini salvati sui di che erano in acqua (il soccorso della missione: achievement e carriera);
// anche senza u, m688.salvati(n, di)
int f_salvati(lua_State* L) {
    const int i = lua_gettop(L) >= 3 ? 2 : 1;
    M(L).salvati(static_cast<int>(num(L, i)), static_cast<int>(num(L, i + 1)));
    return 0;
}
// m688.tempo_contatto(u [, b]): i secondi dalla nascita del contatto attuale sul bersaglio (o b); 0 senza; nil nel classico
int f_tempo_contatto(lua_State* L) {
    if (!M(L).con_acustica()) { lua_pushnil(L); return 1; }
    const auto& u = B(L);
    const auto* t = bersaglio(L);
    const int b = lua_gettop(L) >= 2 && lua_isnumber(L, 2) ? static_cast<int>(num(L, 2)) : t ? t->id : -1;
    const auto c = b >= 0 ? u.contatti.find(b) : u.contatti.end();
    lua_pushnumber(L, c != u.contatti.end() ? M(L).tempo() - c->second.prima_scoperta : 0.0);
    return 1;
}
// m688.trasmesso(u, tipo [, dopo_s]): il giocatore ha fatto una trasmissione di quel tipo (contatto, attacco, situazione,
// conferma, richiesta, previsione) dopo dopo_s secondi di missione, anche da una boa SLOT (libreria I07; dal registro)
int f_trasmesso(lua_State* L) {
    const std::string tipo = str(L, 2);
    const double dopo = lua_gettop(L) >= 3 ? num(L, 3) : 0.0;
    for (const EventoMissione& e : M(L).eventi_missione())
        if (e.tipo == TipoEvento::trasmissione && e.tempo >= dopo && tipo == nome_trasmissione(static_cast<TipoTrasmissione>(e.codice)))
            return vero(L, true);
    return vero(L, false);
}
// m688.evento(u, nome [, dopo_s]): nel registro c'e' un evento con quel nome (incaglio, collisione, colpo_esercitazione,
// lancio_nemico, ...; i nomi di nome_evento) dopo dopo_s secondi di missione. Per le regole dell'editor (libreria I03)
int f_evento(lua_State* L) {
    const std::string nome = str(L, 2);
    const double dopo = lua_gettop(L) >= 3 ? num(L, 3) : 0.0;
    for (const EventoMissione& e : M(L).eventi_missione())
        if (e.tempo >= dopo && nome == nome_evento(e.tipo)) return vero(L, true);
    return vero(L, false);
}
int f_armi_rimaste(lua_State* L) {   // m688.armi_rimaste(u [, famiglia]): siluri nei tubi e di riserva; nil senza carico
    const auto n = M(L).armi_rimaste(B(L).id, lua_gettop(L) >= 2 ? str(L, 2) : std::string{});
    if (!n) { lua_pushnil(L); return 1; }
    lua_pushinteger(L, *n);
    return 1;
}
int f_attacca_unita(lua_State* L) {   // m688.attacca_unita(u, bersaglio [, senza_soluzione]): l'attacco forzato dell'IA
    return vero(L, M(L).attacca_unita(B(L).id, static_cast<int>(num(L, 2)), lua_gettop(L) >= 3 && lua_toboolean(L, 3)));
}
int f_situazione(lua_State* L) {   // m688.situazione(u, "guerra" [, "usa", "urss"]): fra due parti, o fra tutte
    const auto s = situazione_da_nome(str(L, 2));
    if (!s || !M(L).con_forze()) return vero(L, false);
    const int a = lua_gettop(L) >= 4 ? lato_da_nome(str(L, 3)) : -1, b = lua_gettop(L) >= 4 ? lato_da_nome(str(L, 4)) : -1;
    M(L).dichiara_situazione(*s, a, b);
    return vero(L, true);
}
int f_mine_scoperte(lua_State* L) {   // quante mine ha visto il BQS-15 del giocatore
    lua_pushinteger(L, static_cast<lua_Integer>(M(L).mine_scoperte().size()));
    return 1;
}
// F54_1: schiera trainata filata (ascolto a +-90 gradi) e sonar +3B += 2. Difetto dell'originale riprodotto: il
// recupero toglie sempre 2, anche se la schiera era gia' dentro (sensori_e_navigazione.md §2).
int f_schiera_trainata(lua_State* L) {
    auto& b = B(L);
    if (argomento_vero(L, 2) && M(L).schiera_persa(b.id)) return 0;   // Enhanced: la schiera persa non si fila piu'
    if (argomento_vero(L, 2)) { b.flag54 |= 1; b.sonar += 2; }
    else { b.flag54 &= ~1; b.sonar -= 2; }
    return 0;
}
int f_periscopio(lua_State* L) { auto& b = B(L); b.flag54 = argomento_vero(L, 2) ? (b.flag54 | 2) : (b.flag54 & ~2); return 0; }
int f_antenna(lua_State* L) { auto& b = B(L); b.flag54 = argomento_vero(L, 2) ? (b.flag54 | 4) : (b.flag54 & ~4); return 0; }

// -- ingaggio
int f_bersaglio(lua_State* L) { B(L).bersaglio = static_cast<int>(num(L, 2)); return 0; }
int f_memorizza_bersaglio(lua_State* L) { M(L).bersaglio_memorizzato() = B(L).bersaglio; return 0; }
int f_marca_contatti(lua_State* L) { for (auto& [id, c] : B(L).contatti) c.marcato = true; return 0; }
int f_ingaggia(lua_State* L) {
    // ENGAGE n: aggancia il bersaglio corrente (+60), ne porta il conto a n, imposta +10 e +56 (§8)
    auto& b = B(L);
    if (auto it = b.contatti.find(b.bersaglio); it != b.contatti.end()) {
        it->second.marcato = true;
        it->second.conto = std::max(1, static_cast<int>(num(L, 2)));
        b.agganciato = b.bersaglio;
    }
    b.g_bersaglio = str(L, 3); b.g_reazione = str(L, 4);
    return 0;
}
// ATTACK (54h, 2011:1CC1): la portata viene letta e scartata; il byte (0: +3D) e' una soglia sul contatore DS:5100,
// quindi riesce con probabilita' (18 - b)/18 (mai con b >= 18, difetto dell'originale); danno fisso 30 sul bersaglio.
// Enhanced: l'aereo guidato dal controllore ASW sgancia solo quando lo decide il controllore (che ha le sue regole
// d'ingaggio), e nessuno colpisce oltre la portata, per asse come IF_TGT_NEAR e almeno 2 nm (l'arma cerca il bersaglio)
int f_attacca(lua_State* L) {
    auto& m = M(L);
    int b = static_cast<int>(num(L, 3));
    if (b == 0) b = B(L).difesa;
    auto* t = bersaglio(L);
    if (!t) return 0;
    if (m.con_acustica() && (m.controllato_asw(B(L).id) || !entro_per_asse(B(L).pos, t->pos, metri_da_nm(L, std::max(num(L, 2), 2.0)))))
        return 0;
    if (m.con_acustica() && t->affondamento > 0) return 0;   // Enhanced: non su chi sta gia' affondando
    // Enhanced: mai su un amico (la SCENE8C: la Nimitz attaccava il suo SH-60B)
    if (m.con_acustica() && (B(L).flag & 0x18) != 0 && (B(L).flag & 0x18) == (t->flag & 0x18)) return 0;
    if (m.con_acustica())   // Enhanced: il lanciarazzi corto tira solo dentro la sua fascia (il Terne della Oslo)
        if (const auto f = fascia_lanciarazzi(B(L).classe)) {
            const double d = distanza(B(L).pos, t->pos);
            if (d < f->first || d > f->second) return 0;
        }
    // Enhanced: la nave tira il siluro antisom vero della classe col modello (S12; missione_antisom_navi.cpp), se puo';
    // tubi e riserva sono i suoi, quindi le salve della dotazione contano solo per l'attacco a dado qui sotto (prima si
    // consumavano anche mentre il siluro vero aspettava, e dopo 12 ATTACK la nave non tirava piu' nemmeno quello)
    if (m.con_acustica() && m.siluro_antisom_nave(B(L).id, t->id)) return 0;
    if (m.con_acustica() && !m.salva_asw(B(L))) return 0;   // Enhanced: le salve della dotazione sono finite
    if (m.conto_passi() <= b) {   // mancato: la carica esplode lo stesso (per gli effetti)
        Colpo c{Colpo::Tipo::attacco, m.tempo(), t->id, -1, B(L).id, t->classe, {}, t->pos, t->quota, 0, false, false, B(L).classe};
        m.registra_colpo(c);
        return 0;
    }
    m.applica_attacco(*t, 30, Colpo::Tipo::attacco);
    return 0;
}
// le salve antisom di bordo per classe (STIMA: una salva = un ATTACK della libreria, circa 12 razzi o due siluri):
// Kara 2 RBU-6000 + 2 RBU-1000, Kresta II e Udaloy 2 RBU-6000 coi tubi, Krivak 2 RBU-6000 con 144 razzi, Grisha e
// Petya i loro RBU; Spruance ASROC e Mk 32, Perry i Mk 32 col carico dell'elicottero; la Oslo il Terne III, sei salve
// da sei razzi. Le altre 12
int salve_asw_classe(const std::string& c) {
    if (c == "kara") return 20;
    if (c.rfind("kresta", 0) == 0 || c == "udaloy" || c == "spruance") return 16;
    if (c.rfind("grisha", 0) == 0 || c.rfind("petya", 0) == 0) return 8;
    if (c == "oslo") return 6;
    return 12;
}

// ATTACK_RND (6Ah, 2011:1C96): riesce se un byte casuale e' >= +3D del modello corrente; la vittima e' l'unita'
// corrente (DS:475A: chi esegue lo script, il bersaglio del missile); messaggio 50 sempre, poi il danno del byte.
int f_attacco_casuale(lua_State* L) {
    auto& m = M(L);
    if (m.attacco_da_missile()) return 0;   // Enhanced: il missile del modello vola, l'esito e' del suo impatto
    const int r = static_cast<int>(m.casuale().prossimo() >> 56);
    if (r < B(L).difesa) return 0;
    m.messaggio(m.contesto().esecutore, MSG_MISSILE_A_SEGNO);
    auto* v = m.trova(m.ultima_trovata());
    if (!v) return 0;
    if (v->affondamento == 0) v->colpo = 0xC9;
    m.applica_attacco(*v, static_cast<int>(num(L, 2)), Colpo::Tipo::attacco_casuale);
    return 0;
}
// LAUNCH (2Dh, 2011:18B7): prepara nel modello corrente l'arma che `nuova_unita` creera' (l'opcode non crea l'unita'):
// soglia d'arrivo 0, script all'arrivo e di reazione, inseguimento del bersaglio memorizzato (DS:4762) con il
// contatto del lanciatore copiato a traccia solida (qualita' 1200). Senza bersaglio memorizzato l'arma va dritta.
int f_lancia(lua_State* L) {
    auto& m = M(L);
    auto& c = m.contesto();
    auto& b = c.buffer;
    b.soglia_arrivo = 0;
    b.g_arrivo = str(L, 2); b.g_reazione = str(L, 3);
    if (b.categoria.empty() || b.categoria == "sottomarino" || b.categoria == "nave") b.categoria = "arma";
    b.lanciatore = c.esecutore;   // +62 (lo riscrive LINK)
    b.colpo = b.affondamento = 0;
    const int t = m.bersaglio_memorizzato();
    const auto* bersaglio_m = t >= 0 ? m.trova(t) : nullptr;
    if (!bersaglio_m) {
        b.bersaglio = b.inseguito = -1;
        b.navigazione = b.insegui = false;
        c.contatto_lancio.reset();
        return 0;
    }
    b.navigazione = b.insegui = true;
    b.bersaglio = b.inseguito = t;
    Contatto k;
    k.pos_stimata = bersaglio_m->pos;
    k.quota_stimata = bersaglio_m->quota;
    if (const auto* l = m.trova(c.esecutore))
        if (const auto it = l->contatti.find(t); it != l->contatti.end()) k = it->second;
    k.qualita = 1200.0;   // +0E = 4B0h
    k.durata = 0.0;       // +14 = 0
    c.contatto_lancio = std::make_pair(t, k);
    return 0;
}

// -- stato e script
int f_comportamento(lua_State* L) { B(L).g_bersaglio = str(L, 2); return 0; }   // BEHAVIOR: solo +10
int f_nessun_comportamento(lua_State* L) { B(L).g_bersaglio.clear(); return 0; }
int f_dopo(lua_State* L) {
    auto& m = M(L);
    m.programma(num(L, 2), m.contesto().selezionata, str(L, 3), str(L, 4) == "assoluto");
    return 0;
}
int f_annulla_timer(lua_State* L) { const std::string g = str(L, 2); M(L).annulla_timer(M(L).contesto().selezionata, &g); return 0; }
int f_annulla_tutti_i_timer(lua_State* L) { M(L).annulla_timer(M(L).contesto().selezionata, nullptr); return 0; }
int f_messaggio(lua_State* L) { M(L).messaggio(M(L).contesto().esecutore, static_cast<int>(num(L, 2))); return 0; }
int f_messaggio_al_giocatore(lua_State* L) {
    auto& m = M(L);
    const auto* e = m.trova(m.contesto().esecutore);
    const auto* g = m.giocatore();
    // l'unita' e' il giocatore o e' stata lanciata da lui (+62, anche dopo il taglio del filo)
    if (e && g && (e->id == g->id || e->collegata == g->id || e->lanciatore == g->id)) m.messaggio(e->id, static_cast<int>(num(L, 2)));
    return 0;
}
// Eventi acustici (§4): l'unita' corrente si rivela a tutti entro 20 nm; il ping illumina anche il proprio settore
// di prua. Agli altri giocatori arrivano i messaggi 8 (sonar attivo) e 30h (lancio di missile).
void avvisa_giocatore(lua_State* L, int emittente, int codice) {
    auto& m = M(L);
    const auto* g = m.giocatore();
    if (g && g->id != emittente) m.messaggio(emittente, codice);
}
int f_impulso_sonar(lua_State* L) {   // PING (2Ah): 300 passi = 33 s
    const int id = B(L).id;
    M(L).evento_acustico(id, 300.0 / 9.0);
    M(L).impulso_sonar_enhanced(id);   // nell'Enhanced: eco e contro-scoperta (niente senza l'acustica)
    if (id == 0) ++M(L).variabili()[0];
    avvisa_giocatore(L, id, 8);
    return 0;
}
int f_lancio_missile(lua_State* L) {   // MISSILE (5Ah): 900 passi = 100 s
    const int id = B(L).id;
    // Enhanced: il missile dell'IA col modello (docs/21 §7); col piano non valido non parte: niente rumore, voce, avviso
    if (M(L).missile_dello_script(id) == Missione::EsitoMissileScript::negato) return 0;
    M(L).evento_acustico(id, 900.0 / 9.0);
    M(L).registra_lancio_missile(id);
    avvisa_giocatore(L, id, 0x30);
    return 0;
}
int f_rumore_transitorio(lua_State* L) { M(L).evento_acustico(B(L).id, 100.0 / 9.0); return 0; }   // TRANSIENT (73h): 11 s
int f_variabile(lua_State* L) { M(L).variabili()[static_cast<int>(num(L, 2))] = static_cast<int>(num(L, 3)); return 0; }
int f_variabile_piu(lua_State* L) { ++M(L).variabili()[static_cast<int>(num(L, 2))]; return 0; }
int f_variabile_meno(lua_State* L) { --M(L).variabili()[static_cast<int>(num(L, 2))]; return 0; }
int f_flag_scena(lua_State* L) { B(L).flag_scena = static_cast<int>(num(L, 2)); return 0; }
int f_flag_scena_piu(lua_State* L) { ++B(L).flag_scena; return 0; }
int f_flag_scena_meno(lua_State* L) { --B(L).flag_scena; return 0; }
int f_flag_scena_accendi(lua_State* L) { B(L).flag_scena |= static_cast<int>(num(L, 2)); return 0; }
int f_flag_scena_spegni(lua_State* L) { B(L).flag_scena &= ~static_cast<int>(num(L, 2)); return 0; }
int f_collega(lua_State* L) {
    auto& m = M(L);
    auto& c = m.contesto();
    // LINK (197Fh): con n != 0 collega (+0C) e registra il lanciatore (+62); LINK 0 azzera solo +0C (taglio del filo)
    if (num(L, 2) == 0.0 && m.siluro(c.selezionata)) return 0;   // il filo dei siluri del modello lo decide il modello
    c.buffer.collegata = num(L, 2) != 0.0 ? (c.esecutore != c.selezionata ? c.esecutore : m.ultima_trovata()) : -1;
    if (c.buffer.collegata >= 0) c.buffer.lanciatore = c.buffer.collegata;
    return 0;
}
int f_devia_riferimenti(lua_State* L) {
    auto& m = M(L);
    const int da = B(L).collegata, a = m.ultima_trovata();
    std::vector<int> ids;   // tutte le unita' che puntavano la collegata puntano l'ultima trovata
    for (const auto& kv : m.unita()) ids.push_back(kv.first);
    for (int id : ids)
        if (auto* u = m.trova(id); u && u->bersaglio == da) u->bersaglio = a;
    return 0;
}

// -- modello, selezione e creazione
int f_salva_modello(lua_State* L) { M(L).salva_buffer(); return 0; }
int f_nuova_unita(lua_State* L) { M(L).nuova_unita_da_buffer(); return 0; }
int f_seleziona(lua_State* L) { M(L).carica_buffer(static_cast<int>(num(L, 2))); return 0; }
// opcode 37h (2011:19CB): toglie dal gioco l'unita' corrente DS:475A (chi esegue lo script, se nel frattempo non se
// ne e' selezionata un'altra): fine corsa dei siluri, esca esaurita, carica esplosa
int f_togli_unita(lua_State* L) {
    auto& m = M(L);
    if (auto* u = m.trova(m.ultima_trovata()); u && !m.siluro(u->id)) m.togli(*u);
    return 0;
}
int f_seleziona_bersaglio(lua_State* L) {   // SELECT_TGT (59h, 2011:1DA0): il bersaglio diventa l'unita' corrente
    const int b = B(L).bersaglio;
    if (b < 0 || !M(L).trova(b)) return luaL_error(L, FINE_SCRIPT);   // la VM termina lo script
    M(L).carica_buffer(b);
    M(L).ultima_trovata() = b;
    return 0;
}
// SELECT_LINKED (6Bh, 2011:1DCD): come SELECT_TGT, ma prima collega il bersaglio (+62) all'unita' corrente: cosi' il
// timer del missile gira sul bersaglio, che e' la vittima di ATTACK_RND
int f_seleziona_collegata(lua_State* L) {
    auto& m = M(L);
    const int b = B(L).bersaglio;
    auto* t = b >= 0 ? m.trova(b) : nullptr;
    if (!t) return luaL_error(L, FINE_SCRIPT);
    t->lanciatore = m.ultima_trovata();
    m.carica_buffer(b);
    m.ultima_trovata() = b;
    return 0;
}
int f_modello_azzera(lua_State* L) {
    auto& b = B(L);
    b.contatti.clear();
    b.navigazione = b.insegui = false; b.rateo_virata = 0; b.rateo_quota = 0;
    b.g_bersaglio.clear(); b.g_reazione.clear(); b.g_arrivo.clear();
    b.agganciato = b.inseguito = -1;
    b.colpo = b.affondamento = 0;
    return 0;
}
int f_modello_classe(lua_State* L) { B(L).classe = str(L, 2); return 0; }
int f_modello_tipo(lua_State* L) { B(L).tipo = B(L).conto_tipo = static_cast<int>(num(L, 2)) & 0xFF; return 0; }   // 2011:13BF: i due byte
int f_modello_flag(lua_State* L) {
    auto& b = B(L);
    b.flag = static_cast<int>(num(L, 2));
    if (b.flag & 0x20) b.categoria = "arma";
    return 0;
}
int f_modello_integrita(lua_State* L) { B(L).integrita = B(L).integrita_max = static_cast<int>(num(L, 2)); return 0; }
int f_modello_quota_max(lua_State* L) { B(L).quota_max = num(L, 2); return 0; }
int f_modello_rateo_quota(lua_State* L) { B(L).rateo_quota_modello = num(L, 2); return 0; }
int f_modello_virata(lua_State* L) { B(L).virata = num(L, 2); return 0; }
int f_modello_accelerazione(lua_State* L) { B(L).accelerazione = num(L, 2); return 0; }
int f_modello_avanti_tutta(lua_State* L) { B(L).avanti_tutta = num(L, 2); return 0; }
int f_modello_rumore(lua_State* L) { B(L).rumore = static_cast<int>(num(L, 2)); return 0; }
int f_modello_sonar(lua_State* L) { B(L).sonar = static_cast<int>(num(L, 2)); return 0; }
int f_modello_difesa(lua_State* L) { B(L).difesa = static_cast<int>(num(L, 2)); return 0; }
int f_modello_3c(lua_State* L) { B(L).p3c = static_cast<int>(num(L, 2)); return 0; }
int f_modello_4c(lua_State* L) { B(L).p4c = static_cast<int>(num(L, 2)); return 0; }
int f_nuovo_caso(lua_State* L) { M(L).casuale().prossimo(); return 0; }
int f_termoclini(lua_State* L) {
    auto& t = M(L).termoclini();
    t.clear();
    for (int i = 2; i <= lua_gettop(L); ++i) if (lua_isnumber(L, i)) t.push_back(lua_tonumber(L, i));
    return 0;
}

constexpr Voce API[] = {
    {"bersaglio_id", f_bersaglio_id}, {"bersaglio_arma", f_bersaglio_arma}, {"bersaglio_nave", f_bersaglio_nave},
    {"bersaglio_aereo", f_bersaglio_aereo}, {"bersaglio_nemico", f_bersaglio_nemico},
    {"bersaglio_neutrale", f_bersaglio_neutrale}, {"bersaglio_mi_punta", f_bersaglio_mi_punta},
    {"bersaglio_vicino", f_bersaglio_vicino}, {"traccia_solida", f_traccia_solida}, {"traccia_media", f_traccia_media},
    {"traccia_lunga", f_traccia_lunga}, {"soluzione", f_soluzione}, {"caso", f_caso}, {"unita_esiste", f_unita_esiste},
    {"unita_colpita", f_unita_colpita}, {"unita_id", f_unita_id}, {"x_maggiore", f_x_maggiore},
    {"in_rettangolo", f_in_rettangolo}, {"variabile_uguale", f_variabile_uguale},
    {"variabile_maggiore", f_variabile_maggiore}, {"variabile_minore", f_variabile_minore},
    {"confronta_variabili", f_confronta_variabili}, {"flag_scena_uguale", f_flag_scena_uguale},
    {"flag_scena_bit", f_flag_scena_bit}, {"giocatore_secondo", f_giocatore_secondo},
    {"livello_standard", f_livello_standard}, {"manomesso", f_manomesso},
    {"rotta", f_rotta}, {"rotta_ora", f_rotta_ora}, {"virata_relativa", f_virata_relativa},
    {"rateo_virata", f_rateo_virata}, {"accosta_a", f_accosta_a}, {"telegrafo", f_telegrafo}, {"velocita_ora", f_velocita_ora},
    {"quota", f_quota}, {"quota_ora", f_quota_ora}, {"azzera_rateo_quota", f_azzera_rateo_quota},
    {"accosta_verso_bersaglio", f_accosta_verso_bersaglio}, {"accosta_opposto", f_accosta_opposto},
    {"uguaglia_velocita", f_uguaglia_velocita}, {"uguaglia_quota", f_uguaglia_quota},
    {"ferma_navigazione", f_ferma_navigazione}, {"riprendi_navigazione", f_riprendi_navigazione},
    {"navigazione", f_navigazione}, {"vai_a", f_vai_a}, {"vai_al_bersaglio", f_vai_al_bersaglio},
    {"cue_asw", f_cue_asw},
    {"in_cerchio", f_in_cerchio}, {"velocita_sotto", f_velocita_sotto}, {"quota_entro", f_quota_entro},
    {"in_superficie", f_in_superficie}, {"alberi_alzati", f_alberi_alzati}, {"obiettivo_compiuto", f_obiettivo_compiuto},
    {"contatto_da", f_contatto_da}, {"senza_contatto_da", f_senza_contatto_da},
    {"ping_giocatore_entro", f_ping_giocatore_entro}, {"ha_pingato", f_ha_pingato}, {"ping_entro", f_ping_entro},
    {"distanza_da", f_distanza_da}, {"comanda_mine", f_comanda_mine},
    {"ha_lanciato", f_ha_lanciato},
    {"radio", f_radio}, {"posa_mina", f_posa_mina}, {"distanza_cavo", f_distanza_cavo},
    {"posto_di_combattimento", f_posto_di_combattimento}, {"bqs15", f_bqs15}, {"mine_scoperte", f_mine_scoperte},
    {"avaria", f_avaria}, {"incaglia", f_incaglia}, {"rimorchia", f_rimorchia}, {"pedina_vicino", f_pedina_vicino}, {"molla_rimorchio", f_molla_rimorchio},
    {"in_avaria", f_in_avaria}, {"rimorchiata", f_rimorchiata}, {"in_acque_territoriali", f_in_acque_territoriali},
    {"esercitazione", f_esercitazione}, {"allerta", f_allerta}, {"armi_libere", f_armi_libere}, {"situazione", f_situazione}, {"in_zona_amica", f_in_zona_amica}, {"armi_rimaste", f_armi_rimaste}, {"attacca_unita", f_attacca_unita}, {"trasmesso", f_trasmesso}, {"evento", f_evento}, {"bersaglio_non_combattente", f_bersaglio_non_combattente},
    {"classificato", f_classificato}, {"tempo_contatto", f_tempo_contatto}, {"salvati", f_salvati},
    {"insegui", f_insegui}, {"teletrasporta_al_bersaglio", f_teletrasporta_al_bersaglio},
    {"posizione", f_posizione}, {"schiera_trainata", f_schiera_trainata}, {"periscopio", f_periscopio},
    {"antenna", f_antenna}, {"bersaglio", f_bersaglio}, {"memorizza_bersaglio", f_memorizza_bersaglio},
    {"marca_contatti", f_marca_contatti}, {"ingaggia", f_ingaggia}, {"attacca", f_attacca},
    {"attacco_casuale", f_attacco_casuale}, {"lancia", f_lancia}, {"comportamento", f_comportamento},
    {"nessun_comportamento", f_nessun_comportamento}, {"dopo", f_dopo}, {"annulla_timer", f_annulla_timer},
    {"annulla_tutti_i_timer", f_annulla_tutti_i_timer}, {"messaggio", f_messaggio},
    {"messaggio_al_giocatore", f_messaggio_al_giocatore}, {"impulso_sonar", f_impulso_sonar},
    {"lancio_missile", f_lancio_missile}, {"rumore_transitorio", f_rumore_transitorio},
    {"variabile", f_variabile}, {"variabile_piu", f_variabile_piu},
    {"variabile_meno", f_variabile_meno}, {"flag_scena", f_flag_scena}, {"flag_scena_piu", f_flag_scena_piu},
    {"flag_scena_meno", f_flag_scena_meno}, {"flag_scena_accendi", f_flag_scena_accendi},
    {"flag_scena_spegni", f_flag_scena_spegni}, {"collega", f_collega}, {"devia_riferimenti", f_devia_riferimenti},
    {"salva_modello", f_salva_modello}, {"nuova_unita", f_nuova_unita}, {"seleziona", f_seleziona},
    {"togli_unita", f_togli_unita}, {"seleziona_collegata", f_seleziona_collegata},
    {"seleziona_bersaglio", f_seleziona_bersaglio}, {"modello_azzera", f_modello_azzera},
    {"modello_classe", f_modello_classe}, {"modello_tipo", f_modello_tipo}, {"modello_flag", f_modello_flag},
    {"modello_integrita", f_modello_integrita}, {"modello_quota_max", f_modello_quota_max},
    {"modello_rateo_quota", f_modello_rateo_quota}, {"modello_virata", f_modello_virata},
    {"modello_accelerazione", f_modello_accelerazione}, {"modello_avanti_tutta", f_modello_avanti_tutta},
    {"modello_rumore", f_modello_rumore}, {"modello_sonar", f_modello_sonar}, {"modello_difesa", f_modello_difesa},
    {"modello_3c", f_modello_3c}, {"modello_4c", f_modello_4c}, {"nuovo_caso", f_nuovo_caso},
    {"termoclini", f_termoclini},
};

// funzione sconosciuta: la si registra e restituisce falso (una condizione non soddisfatta)
int f_sconosciuta(lua_State* L) {
    M(L).segnala_non_implementata(lua_tostring(L, lua_upvalueindex(2)));
    lua_pushboolean(L, 0);
    return 1;
}
int indice_m688(lua_State* L) {   // __index(tabella, nome)
    lua_pushvalue(L, lua_upvalueindex(1));
    lua_pushvalue(L, 2);
    lua_pushcclosure(L, f_sconosciuta, 2);
    return 1;
}

}  // namespace

// ---------------------------------------------------------------------------------------------------------------

// La memoria dello stato Lua con un tetto (LimitiScript::memoria_byte), che vale solo dentro gli script: fuori le
// poche allocazioni del motore (argomenti, tabelle dei parametri) non devono fallire, perche' Lua lo farebbe con un
// panico
struct Missione::MemoriaScript {
    std::size_t usata = 0, tetto = 0;
    bool in_script = false;
};

void* Missione::alloca_script(void* ud, void* p, std::size_t vecchia, std::size_t nuova) {
    auto* m = static_cast<MemoriaScript*>(ud);
    if (p == nullptr) vecchia = 0;
    if (nuova == 0) {
        std::free(p);
        m->usata -= vecchia;
        return nullptr;
    }
    if (m->in_script && nuova > vecchia && m->usata - vecchia + nuova > m->tetto) return nullptr;   // errore di Lua
    void* q = std::realloc(p, nuova);
    if (q) m->usata = m->usata - vecchia + nuova;
    return q;
}

namespace {
void troppe_istruzioni(lua_State* L, lua_Debug*) { luaL_error(L, "troppe istruzioni (limite degli script)"); }
}  // namespace

static Zona leggi_zona(lua_State* L, int t);   // zone della radio R2 e acque territoriali (piu' avanti)

int Missione::chiama_lua(int argomenti, int risultati) {
    // il conteggio delle istruzioni ricomincia a ogni chiamata: lua_sethook rimette il contatore al limite
    if (limiti_.istruzioni_per_chiamata > 0)
        lua_sethook(L_, troppe_istruzioni, LUA_MASKCOUNT, static_cast<int>(std::min<std::size_t>(limiti_.istruzioni_per_chiamata, 0x7fffffff)));
    if (memoria_script_) memoria_script_->in_script = true;
    const int r = lua_pcall(L_, argomenti, risultati, 0);
    if (memoria_script_) memoria_script_->in_script = false;
    if (limiti_.istruzioni_per_chiamata > 0) lua_sethook(L_, nullptr, 0, 0);
    if (r != LUA_OK && memoria_script_) lua_gc(L_, LUA_GCCOLLECT);   // dopo un errore di memoria, quella libera
    return r;
}

Missione::Missione(const std::string& file, std::uint64_t seme, const LimitiScript& limiti, bool moto_classico)
    : casuale_(seme) {
    seme_ = seme;
    limiti_ = limiti;
    if (limiti.memoria_byte > 0) {
        memoria_script_ = std::make_unique<MemoriaScript>();
        memoria_script_->tetto = limiti.memoria_byte;
        L_ = lua_newstate(alloca_script, memoria_script_.get());
    } else {
        L_ = luaL_newstate();
    }
    if (!L_) throw std::runtime_error("Lua: memoria esaurita");
    // ambiente isolato: solo base (senza caricamento di codice e file), math, string, table
    luaL_requiref(L_, LUA_GNAME, luaopen_base, 1); lua_pop(L_, 1);
    luaL_requiref(L_, LUA_MATHLIBNAME, luaopen_math, 1); lua_pop(L_, 1);
    luaL_requiref(L_, LUA_STRLIBNAME, luaopen_string, 1); lua_pop(L_, 1);
    luaL_requiref(L_, LUA_TABLIBNAME, luaopen_table, 1); lua_pop(L_, 1);
    for (const char* via : {"dofile", "loadfile", "load", "require", "collectgarbage", "print"}) {
        lua_pushnil(L_);
        lua_setglobal(L_, via);
    }
    // math.random di Lua ha un seme casuale: tolto, le missioni usano m688.caso (generatore con seme)
    lua_getglobal(L_, LUA_MATHLIBNAME);
    lua_pushnil(L_); lua_setfield(L_, -2, "random");
    lua_pushnil(L_); lua_setfield(L_, -2, "randomseed");
    lua_pop(L_, 1);
    lua_newtable(L_);
    for (const auto& v : API) {
        lua_pushlightuserdata(L_, this);
        lua_pushcclosure(L_, v.f, 1);
        lua_setfield(L_, -2, v.nome);
    }
    lua_newtable(L_);                 // metatabella: funzioni non implementate
    lua_pushlightuserdata(L_, this);
    lua_pushcclosure(L_, indice_m688, 1);
    lua_setfield(L_, -2, "__index");
    lua_setmetatable(L_, -2);
    lua_setglobal(L_, "m688");

    std::ifstream f(file, std::ios::binary);
    if (!f) throw std::runtime_error("impossibile aprire " + file);
    const std::string testo{std::istreambuf_iterator<char>(f), std::istreambuf_iterator<char>()};
    if (luaL_loadbufferx(L_, testo.data(), testo.size(), ("@" + file).c_str(), "t") != LUA_OK ||
        chiama_lua(0, 1) != LUA_OK) {
        const std::string e = lua_tostring(L_, -1);
        lua_close(L_); L_ = nullptr;
        throw std::runtime_error(e);
    }
    if (!lua_istable(L_, -1)) throw std::runtime_error(file + ": il file non restituisce una tabella");
    gestori_esito_ = leggi_gestori_esito(testo);   // per l'errore di posizione dell'IA (missione_navigazione.cpp)
    const int t = lua_gettop(L_);
    id_ = campo_stringa(L_, t, "id");
    teatro_ = campo_stringa(L_, t, "teatro");
    // per la sala radio dell'Enhanced: lato, data e ora (DTG e finestre), testo del briefing (ordini di missione)
    file_missione_ = file;
    lato_missione_ = campo_stringa(L_, t, "lato");
    lua_getfield(L_, t, "traffico");   // false: niente traffico civile dell'Enhanced (docs/20 §3)
    if (lua_isboolean(L_, -1)) traffico_vietato_ = lua_toboolean(L_, -1) == 0;
    lua_pop(L_, 1);
    lua_getfield(L_, t, "paesaggio_sonoro");   // false: niente paesaggio sonoro dell'Enhanced (scene di prova isolate)
    if (lua_isboolean(L_, -1)) usa_paesaggio_ = lua_toboolean(L_, -1) != 0;
    lua_pop(L_, 1);
    ora_utc_s_ = campo_numero(L_, t, "ora_utc", 0.0) * 3600.0;   // ore UTC; nil: mezzanotte
    if (const std::string d = campo_stringa(L_, t, "data"); d.size() >= 10)   // "AAAA-MM-GG"
        data_dal_file_ = std::sscanf(d.c_str(), "%d-%d-%d", &anno_, &mese_data_, &giorno_) == 3;
    lua_getfield(L_, t, "briefing");
    if (lua_istable(L_, -1)) {
        for (const auto& [lingua, righe] : {std::pair<const char*, std::vector<std::string>*>{"it", &briefing_it_}, {"en", &briefing_en_}}) {
            lua_getfield(L_, -1, lingua);
            if (lua_istable(L_, -1)) {
                for (lua_Integer i = 1; lua_rawgeti(L_, -1, i) == LUA_TSTRING; ++i) { righe->push_back(lua_tostring(L_, -1)); lua_pop(L_, 1); }
                lua_pop(L_, 1);   // il valore che ha chiuso il ciclo
            }
            lua_pop(L_, 1);
        }
    }
    lua_pop(L_, 1);
    lua_getfield(L_, t, "titolo");
    if (lua_istable(L_, -1)) titolo_ = campo_stringa(L_, lua_gettop(L_), "it", id_);
    lua_pop(L_, 1);
    lua_getfield(L_, t, "posa");
    if (lua_istable(L_, -1)) {
        lua_getfield(L_, -1, "metri_per_unita");
        if (lua_istable(L_, -1)) metri_per_unita_ = campo_numero(L_, lua_gettop(L_), "x", metri_per_unita_);
        lua_pop(L_, 1);
        // moto dell'originale: 1 kn = 0,048065 unita' di gioco al secondo (circa 3,46 volte il vero in scala di gioco)
        if (moto_classico) m_s_per_nodo_ = campo_numero(L_, lua_gettop(L_), "moto_classico_m_s_per_nodo", 0.048065 * metri_per_unita_);
        lua_getfield(L_, -1, "centro");
        if (lua_istable(L_, -1)) {
            latitudine_ = campo_numero(L_, lua_gettop(L_), "lat", latitudine_);
            longitudine_ = campo_numero(L_, lua_gettop(L_), "lon", longitudine_);
        }
        lua_pop(L_, 1);
    }
    lua_pop(L_, 1);
    // comandi del giocatore: gestori, tubi e gestori di lancio delle armi
    lua_getfield(L_, t, "gestori_giocatore");
    if (lua_istable(L_, -1))
        for (lua_pushnil(L_); lua_next(L_, -2); lua_pop(L_, 1))
            if (lua_type(L_, -2) == LUA_TSTRING && lua_type(L_, -1) == LUA_TSTRING) gestori_giocatore_[lua_tostring(L_, -2)] = lua_tostring(L_, -1);
    lua_pop(L_, 1);
    lua_getfield(L_, t, "tubi");
    if (lua_istable(L_, -1)) {
        for (lua_Integer i = 1; lua_rawgeti(L_, -1, i) != LUA_TNIL; ++i) { tubi_.push_back(lua_tostring(L_, -1) ? lua_tostring(L_, -1) : ""); lua_pop(L_, 1); }
        lua_pop(L_, 1);   // il nil che chiude la lista
    }
    lua_pop(L_, 1);
    lua_getfield(L_, t, "dotazione");   // scorte iniziali per tipo d'arma, { mk48 = 22, harpoon = 2, esca = 6 }
    if (lua_istable(L_, -1))
        for (lua_pushnil(L_); lua_next(L_, -2); lua_pop(L_, 1))
            if (lua_type(L_, -2) == LUA_TSTRING && lua_isnumber(L_, -1)) dotazione_[lua_tostring(L_, -2)] = static_cast<int>(lua_tointeger(L_, -1));
    lua_pop(L_, 1);
    // carico della sala siluri (Enhanced, M2): tutti facoltativi
    limiti_carico_.dotazione = dotazione_;
    limiti_carico_.tubi_originali = tubi_;
    limiti_carico_.anno = anno_;
    lua_getfield(L_, t, "deposito");   // massimo per famiglia o variante, { mk48 = 21, mk48_adcap = 4, ... }
    if (lua_istable(L_, -1))
        for (lua_pushnil(L_); lua_next(L_, -2); lua_pop(L_, 1))
            if (lua_type(L_, -2) == LUA_TSTRING && lua_isnumber(L_, -1)) limiti_carico_.deposito[lua_tostring(L_, -2)] = static_cast<int>(lua_tointeger(L_, -1));
    lua_pop(L_, 1);
    lua_getfield(L_, t, "tipi_negati");   // { "tomahawk", "mk48_adcap" }
    if (lua_istable(L_, -1)) {
        for (lua_Integer i = 1; lua_rawgeti(L_, -1, i) == LUA_TSTRING; ++i) { limiti_carico_.tipi_negati.insert(lua_tostring(L_, -1)); lua_pop(L_, 1); }
        lua_pop(L_, 1);   // il valore che ha chiuso il ciclo
    }
    lua_pop(L_, 1);
    limiti_carico_.tipo_missione = campo_stringa(L_, t, "tipo_missione");
    limiti_carico_.variante_battello = campo_stringa(L_, t, "variante_battello");
    limiti_carico_.ospiti = static_cast<int>(campo_numero(L_, t, "ospiti", 0.0));
    lua_getfield(L_, t, "what_if");
    if (lua_isboolean(L_, -1)) limiti_carico_.what_if = lua_toboolean(L_, -1) != 0;
    lua_pop(L_, 1);
    lua_getfield(L_, t, "carriera");   // M3: { situazione = "crisi", avversario = "veterano", difficolta = 3, ... }
    if (lua_istable(L_, -1)) {
        const int c = lua_gettop(L_);
        BloccoCarriera b;
        b.situazione = campo_stringa(L_, c, "situazione", b.situazione);
        b.avversario = campo_stringa(L_, c, "avversario", b.avversario);
        b.difficolta = static_cast<int>(campo_numero(L_, c, "difficolta", b.difficolta));
        for (const auto& [k, v] : {std::pair<const char*, bool*>{"operazione_speciale", &b.operazione_speciale}, {"artico", &b.artico}}) {
            lua_getfield(L_, c, k);
            if (lua_isboolean(L_, -1)) *v = lua_toboolean(L_, -1) != 0;
            lua_pop(L_, 1);
        }
        blocco_carriera_ = b;
    }
    lua_pop(L_, 1);
    lua_getfield(L_, t, "armi");
    if (lua_istable(L_, -1))
        for (lua_pushnil(L_); lua_next(L_, -2); lua_pop(L_, 1))
            if (lua_type(L_, -2) == LUA_TSTRING && lua_istable(L_, -1))
                gestori_armi_[lua_tostring(L_, -2)] = campo_stringa(L_, lua_gettop(L_), "gestore");
    lua_pop(L_, 1);
    // la libreria di bordo dei TLAM (docs/21 §11.8): { id, bersaglio, plp = {x, y}, landfall = {x, y}, tot_s, finestra_s,
    // tratta_terra_m }, tot_s dall'inizio della missione
    lua_getfield(L_, t, "tlam");
    if (lua_istable(L_, -1)) {
        for (lua_Integer i = 1; lua_rawgeti(L_, -1, i) == LUA_TTABLE; ++i) {
            const int k = lua_gettop(L_);
            MissioneTlam x;
            x.id = campo_stringa(L_, k, "id");
            x.bersaglio = campo_stringa(L_, k, "bersaglio");
            for (const auto& [nome, v] : {std::pair<const char*, Vec2*>{"plp", &x.plp}, {"landfall", &x.landfall}}) {
                lua_getfield(L_, k, nome);
                if (lua_istable(L_, -1)) *v = {campo_numero(L_, lua_gettop(L_), "x", 0.0), campo_numero(L_, lua_gettop(L_), "y", 0.0)};
                lua_pop(L_, 1);
            }
            x.tot_s = campo_numero(L_, k, "tot_s", 0.0);
            x.finestra_s = campo_numero(L_, k, "finestra_s", x.finestra_s);
            x.tratta_terra_m = campo_numero(L_, k, "tratta_terra_m", 0.0);
            x.mezza_mappa_m = campo_numero(L_, k, "mezza_mappa_m", x.mezza_mappa_m);
            libreria_tlam_.push_back(std::move(x));
            lua_pop(L_, 1);
        }
        lua_pop(L_, 1);   // l'elemento che chiude la lista
    }
    lua_pop(L_, 1);
    lua_getfield(L_, t, "ambiente");
    if (lua_istable(L_, -1)) {
        lua_getfield(L_, -1, "termoclini_m");
        if (lua_istable(L_, -1))
            for (lua_Integer i = 1; lua_rawgeti(L_, -1, i) != LUA_TNIL; ++i) { termoclini_.push_back(lua_tonumber(L_, -1)); lua_pop(L_, 1); }
        lua_pop(L_, 2);
    }
    lua_pop(L_, 1);
    lua_getfield(L_, t, "script");
    if (!lua_istable(L_, -1)) throw std::runtime_error(file + ": manca la tabella script");
    rif_script_ = luaL_ref(L_, LUA_REGISTRYINDEX);
    lua_pushvalue(L_, t);
    rif_missione_ = luaL_ref(L_, LUA_REGISTRYINDEX);
    registro_.carica(L_, t, file, id_);   // obiettivi e categorie degli esiti (M1)
    lua_getfield(L_, t, "mine");   // il campo gia' posato (mine dell'IA, S06 e I04): lo posa siluri_enhanced
    if (lua_istable(L_, -1)) {
        const int mt = lua_gettop(L_);
        for (lua_Integer i = 1; lua_rawgeti(L_, mt, i) == LUA_TTABLE; ++i) {
            const int k = lua_gettop(L_);
            MinaIniziale x;
            x.tipo = campo_stringa(L_, k, "tipo");
            x.pos = {campo_numero(L_, k, "x", 0.0), campo_numero(L_, k, "y", 0.0)};
            const std::string lato = campo_stringa(L_, k, "lato", "urss");
            x.lato = lato == "usa" ? 0x08 : lato == "urss" ? 0x10 : 0;
            lua_getfield(L_, k, "armata");
            x.armata = lua_isnil(L_, -1) || lua_toboolean(L_, -1);
            lua_pop(L_, 1);
            lua_getfield(L_, k, "sicura");   // «in sicurezza» (la I17): in acqua, visibile, mai armata
            x.sicura = lua_toboolean(L_, -1) != 0;
            lua_pop(L_, 1);
            lua_getfield(L_, k, "comandata");   // comandata da terra (la S18), col suo gruppo
            x.comandata = lua_toboolean(L_, -1) != 0;
            lua_pop(L_, 1);
            x.gruppo = campo_stringa(L_, k, "gruppo");
            if (!x.tipo.empty()) mine_iniziali_.push_back(x);
            lua_pop(L_, 1);
        }
        lua_pop(L_, 1);   // il valore che ha chiuso la lista
    }
    lua_pop(L_, 1);
    lua_getfield(L_, t, "esercitazione");   // schede I03 e S01: tutte le armi della missione sono inerti
    esercitazione_tutte_ = lua_toboolean(L_, -1) != 0;
    lua_pop(L_, 1);
    lua_getfield(L_, t, "danni_iniziali");   // scheda I07: il battello del giocatore gia' danneggiato (DanniSottomarini)
    if (lua_istable(L_, -1)) {
        const int dt = lua_gettop(L_);
        auto& D = danni_iniziali_;
        lua_getfield(L_, dt, "urto");
        if (lua_istable(L_, -1)) {
            const int k = lua_gettop(L_);
            D.urto = true;
            D.urto_tnt_kg = campo_numero(L_, k, "tnt_kg", 0.0);
            D.urto_distanza_m = campo_numero(L_, k, "distanza_m", 100.0);
            D.urto_angolo_gradi = campo_numero(L_, k, "angolo_gradi", 0.0);
        }
        lua_pop(L_, 1);
        lua_getfield(L_, dt, "sistemi");
        if (lua_istable(L_, -1)) {
            const int k = lua_gettop(L_);
            lua_pushnil(L_);
            while (lua_next(L_, k)) {
                if (lua_type(L_, -2) == LUA_TSTRING && lua_isnumber(L_, -1)) {
                    const std::string n = lua_tostring(L_, -2);
                    if (sistema_da_chiave(n)) D.sistemi.push_back({n, std::clamp(lua_tonumber(L_, -1), 0.0, 1.0)});   // nucleo/battello.hpp
                    else errori_.push_back("danni_iniziali: sistema '" + n + "' sconosciuto");
                }
                lua_pop(L_, 1);
            }
        }
        lua_pop(L_, 1);
        const auto locali = [&](const char* chiave, const char* campo, std::vector<DanniIniziali::Locale>& v) {
            lua_getfield(L_, dt, chiave);
            if (lua_istable(L_, -1)) {
                const int k = lua_gettop(L_);
                for (lua_Integer i = 1; lua_rawgeti(L_, k, i) == LUA_TTABLE; ++i) {
                    const int e = lua_gettop(L_);
                    v.push_back({campo_stringa(L_, e, "compartimento"), campo_numero(L_, e, campo, 0.0)});
                    lua_pop(L_, 1);
                }
                lua_pop(L_, 1);
            }
            lua_pop(L_, 1);
        };
        locali("acqua", "m3", D.acqua);
        locali("falle", "area_m2", D.falle);
        locali("incendi", "intensita", D.incendi);
    }
    lua_pop(L_, 1);
    lua_getfield(L_, t, "acque_territoriali");   // dossier 22 §2.4: le regole in pace del paese costiero (missione_artiglieria)
    if (lua_istable(L_, -1)) {
        const int at = lua_gettop(L_);
        for (lua_Integer i = 1; lua_rawgeti(L_, at, i) == LUA_TTABLE; ++i) {
            const int k = lua_gettop(L_);
            AcqueTerritoriali a;
            a.nome = campo_stringa(L_, k, "nome");
            const std::string lato = campo_stringa(L_, k, "lato", "neutrale");
            a.lato = lato == "usa" ? 0x08 : lato == "urss" ? 0x10 : 0x18;
            a.zona = leggi_zona(L_, k);
            if (a.zona.centro || a.zona.vertici.size() >= 3) acque_territoriali_.push_back(std::move(a));
            else errori_.push_back("acque_territoriali " + std::to_string(i) + ": manca il cerchio o il poligono");
            lua_pop(L_, 1);
        }
        lua_pop(L_, 1);
    }
    lua_pop(L_, 1);
    lua_getfield(L_, t, "messaggi");   // E6: i messaggi di bordo propri (1000 + n), con italiano e inglese accanto
    if (lua_istable(L_, -1)) {
        const int mt = lua_gettop(L_);
        for (lua_pushnil(L_); lua_next(L_, mt); lua_pop(L_, 1))
            if (lua_isinteger(L_, -2) && lua_istable(L_, -1)) {
                const int n = static_cast<int>(lua_tointeger(L_, -2));
                const int v = lua_gettop(L_);
                if (n >= 0) messaggi_missione_[CODICI_MISSIONE + n] = {campo_stringa(L_, v, "it"), campo_stringa(L_, v, "en", campo_stringa(L_, v, "it"))};
            }
    }
    lua_pop(L_, 1);

    // variante iniziale: probabilita' esatte dell'originale
    lua_getfield(L_, t, "varianti");
    if (!lua_istable(L_, -1)) throw std::runtime_error(file + ": mancano le varianti");
    const int v = lua_gettop(L_);
    const auto n = static_cast<int>(lua_rawlen(L_, v));
    const double estratto = casuale_.uniforme();
    double cumulata = 0.0;
    variante_ = n;
    for (int i = 1; i <= n; ++i) {
        lua_rawgeti(L_, v, i);
        cumulata += campo_numero(L_, lua_gettop(L_), "probabilita", 0.0);
        lua_pop(L_, 1);
        if (estratto < cumulata) { variante_ = i; break; }
    }
    lua_rawgeti(L_, v, variante_);
    leggi_variante(lua_gettop(L_));
    lua_settop(L_, 0);
}

Missione::~Missione() {
    if (L_) lua_close(L_);
}

void Missione::leggi_variante(int vt) {
    lua_getfield(L_, vt, "unita");
    const int ut = lua_gettop(L_);
    for (lua_Integer i = 1; lua_rawgeti(L_, ut, i) == LUA_TTABLE; ++i) {
        const int e = lua_gettop(L_);
        UnitaClassica u;
        u.id = static_cast<int>(campo_numero(L_, e, "id", static_cast<double>(i - 1)));
        u.classe = campo_stringa(L_, e, "classe");
        u.categoria = campo_stringa(L_, e, "categoria");
        u.tipo_originale = static_cast<int>(campo_numero(L_, e, "tipo_originale", -1));
        u.flag = static_cast<int>(campo_numero(L_, e, "flag", 0));
        if (campo_stringa(L_, e, "schieramento") == "luogo") u.flag |= 0x18;   // M7: la parte del luogo, lato 24
        u.tipo = u.conto_tipo = static_cast<int>(campo_numero(L_, e, "tipo", 1)) & 0xFF;
        u.giocatore = campo_bool(L_, e, "giocatore");
        u.cisterne = campo_stringa(L_, e, "cisterne");
        lua_getfield(L_, e, "posizione");
        if (lua_istable(L_, -1)) u.pos = {campo_numero(L_, lua_gettop(L_), "x", 0), campo_numero(L_, lua_gettop(L_), "y", 0)};
        lua_pop(L_, 1);
        u.quota = u.quota_ordinata = campo_numero(L_, e, "quota_m", 0);
        u.rotta = u.rotta_ordinata = campo_numero(L_, e, "rotta", 0);
        u.velocita = u.velocita_ordinata = campo_numero(L_, e, "velocita_kn", 0);
        if (const double b = campo_numero(L_, e, "batteria", -1.0); b >= 0.0) batterie_iniziali_[u.id] = std::min(b, 1.0);   // M6
        if (const std::string st = campo_stringa(L_, e, "stato"); !st.empty()) stati_iniziali_[u.id] = st;   // M6: avaria, incagliata
        lua_getfield(L_, e, "esercitazione");   // schede I03 e S01: un'arma della missione inerte
        if (lua_toboolean(L_, -1)) armi_esercitazione_.insert(u.id);
        lua_pop(L_, 1);
        lua_getfield(L_, e, "armi_esercitazione");   // ...o le armi che questa unita' lancera'
        if (lua_toboolean(L_, -1)) lanciatori_esercitazione_.insert(u.id);
        lua_pop(L_, 1);
        if (const std::string so = campo_stringa(L_, e, "sonarista"); !so.empty()) sonaristi_iniziali_[u.id] = so;   // fase 2
        leggi_luci_unita(u.id, campo_stringa(L_, e, "luci"));   // docs/27: lo stato delle luci della missione
        if (const std::string em = campo_stringa(L_, e, "emcon"); !em.empty())   // S13: l'EMCON di partenza della missione
            for (const StatoEmcon x : {StatoEmcon::silenzio, StatoEmcon::navigazione, StatoEmcon::ricerca, StatoEmcon::intermittente, StatoEmcon::ingaggio})
                if (em == nome_emcon(x)) emcon_iniziali_[u.id] = x;
        lua_getfield(L_, e, "sonar_speciale");   // scheda I09: il sonar attivo con la portata dello scenario
        if (lua_istable(L_, -1)) {
            const int k = lua_gettop(L_);
            SonarSpeciale s;
            s.sl_db = campo_numero(L_, k, "sl_db", s.sl_db);
            s.frequenza_hz = campo_numero(L_, k, "frequenza_hz", s.frequenza_hz);
            s.impulso_s = std::max(campo_numero(L_, k, "impulso_s", s.impulso_s), 0.001);
            s.risoluzione_s = std::max(campo_numero(L_, k, "risoluzione_s", 0.0), 0.0);
            s.ogni_s = campo_numero(L_, k, "ogni_s", 0.0);
            s.dimensione_m = campo_numero(L_, k, "dimensione_m", 0.0);
            s.antenna = campo_stringa(L_, k, "antenna");
            if (!s.antenna.empty() && s.antenna != "sfera" && s.antenna != "lineare" && s.antenna != "scafo")
                errori_.push_back("sonar_speciale: antenna '" + s.antenna + "' sconosciuta (sfera, lineare, scafo)");
            s.prossimo = s.ogni_s;
            sonar_speciale_[u.id] = s;
        }
        lua_pop(L_, 1);
        lua_getfield(L_, e, "modello");
        if (lua_istable(L_, -1)) {
            const int mo = lua_gettop(L_);
            const double massima = campo_numero(L_, mo, "massima_kn", 20.0);
            u.avanti_tutta = massima * 256.0 / 280.0;
            u.virata = campo_numero(L_, mo, "virata_gradi_s", u.virata);
            u.accelerazione = campo_numero(L_, mo, "accel_kn_s", u.accelerazione);
            u.rateo_quota_modello = campo_numero(L_, mo, "quota_m_s", u.rateo_quota_modello);
            u.quota_max = campo_numero(L_, mo, "quota_max_m", u.quota_max);
            u.integrita = u.integrita_max = static_cast<int>(campo_numero(L_, mo, "integrita", u.integrita));
            u.rumore = static_cast<int>(campo_numero(L_, mo, "rumore", u.rumore));
            u.sonar = static_cast<int>(campo_numero(L_, mo, "sonar", u.sonar));
            u.difesa = static_cast<int>(campo_numero(L_, mo, "difesa", u.difesa));
            if (campo_bool(L_, mo, "schiera_trainata")) u.flag54 |= 1;
        }
        lua_pop(L_, 1);
        lua_getfield(L_, e, "rotta_punti");   // primo punto della rotta: navigazione automatica
        if (lua_istable(L_, -1) && lua_rawgeti(L_, -1, 1) == LUA_TTABLE) {
            u.meta = {campo_numero(L_, lua_gettop(L_), "x", 0), campo_numero(L_, lua_gettop(L_), "y", 0)};
            u.navigazione = true;
            mete_assolute_[u.id] = u.meta;   // un punto fisso: col navigatore dell'IA vale la stima, come per vai_a
            lua_pop(L_, 1);
        } else if (lua_istable(L_, -1)) {
            lua_pop(L_, 1);
        }
        lua_pop(L_, 1);
        lua_getfield(L_, e, "gestori");
        if (lua_istable(L_, -1)) {
            const int g = lua_gettop(L_);
            u.g_bersaglio = campo_stringa(L_, g, "bersaglio", campo_stringa(L_, g, "periodico"));   // "periodico": formato precedente
            u.g_reazione = campo_stringa(L_, g, "reazione");
            u.g_arrivo = campo_stringa(L_, g, "arrivo");
        }
        lua_pop(L_, 1);
        prossimo_id_ = std::max(prossimo_id_, u.id + 1);
        u.prossimo_bersaglio = osservatore_ = visitata_ = u.id;   // come nuova_unita_da_buffer: in testa alla lista
        ++create_;
        unita_[u.id] = u;
        lua_pop(L_, 1);
    }
    lua_settop(L_, ut - 1);
    if (esercitazione_tutte_)   // schede I03 e S01: anche le armi gia' nella missione
        for (const auto& [id, u] : unita_) if (u.flag & 0x20) armi_esercitazione_.insert(id);
    for (const auto& [id, st] : stati_iniziali_) {   // M6: gia' in avaria o sugli scogli all'inizio (non sono eventi)
        if (st == "incagliata") incaglia(id);
        else if (st == "relitto") {
            UnitaClassica& u = unita_.at(id);
            relitti_[id] = u.pos;
            u.velocita = u.velocita_ordinata = 0.0;
            u.rumore = 0;   // classico: non si sente
            u.sonar = 0;    // e non sente
            u.flag54 = 0;
        } else if (st == "avaria" && avaria(id)) avarie_iniziate_.pop_back();
        else errori_.push_back("unita' " + std::to_string(id) + ": stato '" + st + "' sconosciuto (avaria, incagliata, relitto)");
    }
    lua_getfield(L_, vt, "timer");
    if (lua_istable(L_, -1)) {
        const int tt = lua_gettop(L_);
        for (lua_Integer i = 1; lua_rawgeti(L_, tt, i) == LUA_TTABLE; ++i) {
            const int e = lua_gettop(L_);
            programma(campo_numero(L_, e, "dopo_s", 0), static_cast<int>(campo_numero(L_, e, "unita", 0)),
                      campo_stringa(L_, e, "gestore"), false);
            lua_pop(L_, 1);
        }
        lua_settop(L_, tt);
    }
    lua_pop(L_, 1);
    lua_getfield(L_, vt, "variabili");
    if (lua_istable(L_, -1)) {
        lua_pushnil(L_);
        while (lua_next(L_, -2)) {
            if (lua_isnumber(L_, -2) && lua_isnumber(L_, -1))
                variabili_[static_cast<int>(lua_tointeger(L_, -2))] = static_cast<int>(lua_tointeger(L_, -1));
            lua_pop(L_, 1);
        }
    }
    lua_pop(L_, 1);
}

std::string Missione::testo_esito(int n) const {
    lua_rawgeti(L_, LUA_REGISTRYINDEX, rif_missione_);
    lua_getfield(L_, -1, "esiti");
    std::string s;
    if (lua_istable(L_, -1)) {
        lua_rawgeti(L_, -1, n);
        if (lua_istable(L_, -1)) s = campo_stringa(L_, lua_gettop(L_), "it");
        lua_pop(L_, 1);
    }
    lua_pop(L_, 2);
    return s;
}

const UnitaClassica* Missione::giocatore() const {
    for (const auto& [id, u] : unita_) if (u.giocatore) return &u;
    return nullptr;
}

UnitaClassica* Missione::trova(int id) {
    const auto it = unita_.find(id);
    return it == unita_.end() || it->second.distrutta || it->second.persa ? nullptr : &it->second;
}

Missione::Contesto& Missione::contesto() {
    if (contesti_.empty()) throw std::logic_error("funzione m688 fuori da uno script");
    return contesti_.back();
}

void Missione::carica_buffer(int id) {
    auto& c = contesto();
    if (auto* u = trova(id)) { c.selezionata = id; c.buffer = *u; ultima_trovata_ = id; }
}

void Missione::salva_buffer() {
    auto& c = contesto();
    auto it = unita_.find(c.selezionata);
    if (it == unita_.end()) return;
    const bool spostata = distanza(it->second.pos, c.buffer.pos) > 1.0;
    const int colpo = it->second.colpo, affondamento = it->second.affondamento;   // stato dei colpi: del motore
    const OrdineMotore ordine = it->second.ordine;
    if (std::fabs(it->second.rotta_ordinata - c.buffer.rotta_ordinata) > 1e-9 || std::fabs(it->second.velocita_ordinata - c.buffer.velocita_ordinata) > 1e-9 ||
        it->second.navigazione != c.buffer.navigazione || it->second.meta.x != c.buffer.meta.x || it->second.meta.y != c.buffer.meta.y)
        ordini_script_[it->first] = tempo_;   // fase 4: nello stesso istante l'ordine dello script vince sul motore
    it->second = c.buffer;
    it->second.colpo = colpo;
    it->second.affondamento = affondamento;
    it->second.ordine = ordine;
    if (spostata) colloca(it->second);
}

const char* siluro_antisom_della_classe(const std::string& classe);   // missione_antisom_navi.cpp

int Missione::nuova_unita_da_buffer() {
    auto& c = contesto();
    const int id = prossimo_id_++;
    c.buffer.id = id;
    c.buffer.giocatore = false;
    c.buffer.contatti.clear();
    if (c.contatto_lancio) {   // arma preparata da LAUNCH: eredita il contatto del bersaglio
        c.buffer.contatti[c.contatto_lancio->first] = c.contatto_lancio->second;
        c.contatto_lancio.reset();
    }
    // 2011:0116: la nuova unita' entra in testa alla lista, la ricerca dell'osservatore e le visite ripartono da
    // lei, il suo primo bersaglio e' se stessa (saltata); DS:1F56 cresce
    c.buffer.prossimo_bersaglio = osservatore_ = visitata_ = id;
    ++create_;
    unita_[id] = c.buffer;
    colloca(unita_[id]);
    if ((unita_[id].flag & 0x20) && (esercitazione_tutte_ || lanciatori_esercitazione_.count(unita_[id].lanciatore)))
        armi_esercitazione_.insert(id);   // schede I03 e S01: inerte
    // Enhanced: l'arma che uno script fa lanciare all'IA e' della dotazione del lanciatore, adatta al bersaglio, e mai su
    // un amico (la principale, 31/10/2026, dal giro lungo: nella SCENE8C dell'originale la Kidd e la Perry lanciavano Mk
    // 48 e Spearfish, siluri da sottomarino, e un Harpoon sull'Alfa immerso). Una nave tira il siluro antisom della sua
    // classe (senza, niente); un antinave non parte su un sottomarino immerso. L'arma che non parte esce subito di gioco
    bool non_parte = false;
    if (siluri_attivi_ && (unita_[id].flag & 0x20)) {
        UnitaClassica& a = unita_[id];
        const UnitaClassica* l = trova(a.lanciatore);
        const UnitaClassica* b = a.bersaglio >= 0 ? trova(a.bersaglio) : nullptr;
        if (l && !l->giocatore) {
            if (b && (l->flag & 0x18) != 0 && (l->flag & 0x18) == (b->flag & 0x18)) non_parte = true;   // un amico
            if (l->categoria == "nave" && tabella_siluri_.siluro(a.classe)) {
                if (const char* s = siluro_antisom_della_classe(l->classe)) a.classe = s;
                else non_parte = true;
            } else if (catalogo_missili_ && b && b->categoria == "sottomarino" && b->quota > 1.0) {
                if (const DatiMissile* d = catalogo_missili_->missile(a.classe); d && d->ruolo == RuoloMissile::antinave) non_parte = true;
            }
        }
        if (non_parte) a.persa = true;
    }
    if (siluri_attivi_ && !non_parte) nuovo_siluro(unita_[id]);
    c.selezionata = id;
    ultima_trovata_ = id;
    return id;
}

void Missione::programma(double dopo_s, int unita, const std::string& gestore, bool assoluto) {
    if (gestore.empty()) return;
    const double t = assoluto ? dopo_s : tempo_ + dopo_s;
    if (assoluto && t < tempo_) return;   // tempo assoluto gia' passato: la VM lo salta
    timer_.push_back({t, unita, gestore, ordine_timer_++, lancio_modellato_ || timer_da_missile_});
}

void Missione::annulla_timer(int unita, const std::string* gestore) {
    std::erase_if(timer_, [&](const Timer& t) { return t.unita == unita && (!gestore || t.gestore == *gestore); });
}

const TestoRadio* Missione::testo_messaggio_missione(int codice) const {
    const auto it = messaggi_missione_.find(codice);
    return it == messaggi_missione_.end() ? nullptr : &it->second;
}

bool Missione::obiettivo_compiuto(const std::string& id) const {
    for (const auto& o : esito_dettagliato().obiettivi)
        if (o.id == id) return o.stato == Obiettivo::Stato::raggiunto;
    return false;
}

void Missione::passo_storia_contatti(double dt) {
    const auto passa = [&](const UnitaClassica& u) {
        if (u.distrutta || u.persa) return;
        for (const auto& [c, k] : u.contatti)
            if (k.visibile()) {
                auto& s = storia_contatti_[{u.id, c}];
                s.cumulato_s += dt;
                s.ultimo = tempo_;
            }
    };
    const auto* g = giocatore();
    if (g) passa(*g);
    for (const int id : osservatori_storia_)
        if (const auto* u = trova(id); u && u != g) passa(*u);
}

double Missione::contatto_cumulato_s(int osservatore, int id) const {
    osservatori_storia_.insert(osservatore);
    const auto it = storia_contatti_.find({osservatore, id});
    return it == storia_contatti_.end() ? 0.0 : it->second.cumulato_s;
}

double Missione::senza_contatto_s(int osservatore, int id) const {
    osservatori_storia_.insert(osservatore);
    const auto it = storia_contatti_.find({osservatore, id});
    return it == storia_contatti_.end() || it->second.ultimo < 0.0 ? tempo_ : tempo_ - it->second.ultimo;
}

void Missione::messaggio(int unita, int codice) {
    messaggi_.push_back({tempo_, unita, codice});
    if (codice >= 100 && codice < CODICI_MISSIONE && !esito_) esito_ = codice - 100;   // i propri della missione no
    if (!esito_ && codice == MSG_FINE) esito_ = ESITO_DISTRUTTO;            // §9.5
    if (!esito_ && codice == MSG_ALTRO_DISTRUTTO) esito_ = ESITO_ALTRO_DISTRUTTO;
}

void Missione::evento_acustico(int unita, double durata_s) {
    if (eventi_.size() >= 12) eventi_.erase(eventi_.begin());   // la lista dell'originale ha 12 posti
    eventi_.push_back({unita, tempo_ + durata_s});
}

bool Missione::salva_asw(const UnitaClassica& u) {
    int& n = salve_asw_[u.id];
    if (n >= salve_asw_classe(u.classe)) return false;
    ++n;
    return true;
}

void Missione::registra_lancio_missile(int unita) {
    const auto* u = trova(unita);
    if (!u) return;
    LancioMissile l;
    l.tempo = tempo_; l.unita = unita; l.bersaglio = u->bersaglio;
    l.pos = u->pos; l.quota = u->quota; l.rotta = u->rotta;
    l.classe_lanciatore = u->classe; l.categoria_lanciatore = u->categoria;
    if (const auto* b = u->bersaglio >= 0 ? trova(u->bersaglio) : nullptr) l.rotta = normalizza_360(rilevamento(u->pos, b->pos));
    // l'arma: la famiglia del tubo che parte, se e' il giocatore; se no dal gestore (due armi possono averne uno solo:
    // Harpoon e Tomahawk antinave nella libreria)
    if (famiglia_lancio_ && gestori_armi_.count(*famiglia_lancio_)) l.arma = *famiglia_lancio_;
    else if (!contesti_.empty())
        for (const auto& [arma, g] : gestori_armi_)
            if (!g.empty() && g == contesti_.back().gestore) l.arma = arma;
    // il missile del modello partito nello stesso istante (la stazione di tiro, poi il gestore della missione) ha gia'
    // la sua voce, con l'arma vera: niente doppione per gli effetti
    const bool gia = std::any_of(lanci_missili_.begin(), lanci_missili_.end(),
                                 [&](const LancioMissile& x) { return x.unita == unita && std::fabs(x.tempo - tempo_) < 1e-6; });
    if (!gia) lanci_missili_.push_back(l);
    lancio_per_statistiche(-1, u, u->bersaglio, l.arma);   // R3 della sandbox
    if (auto em = emcon_.find(unita); em != emcon_.end()) em->second.evento(EventoEmcon::lancio_missili, tempo_);   // EMCON d'ingaggio
}

void Missione::esegui(int unita, const std::string& gestore, const std::vector<ParametroComando>* parametri) {
    if (gestore.empty() || siluri_.count(unita) || missili_.count(unita)) return;   // i siluri e i missili del modello non eseguono script
    auto* u = trova(unita);
    if (!u) return;
    contesti_.push_back({unita, unita, *u});
    contesti_.back().gestore = gestore;
    ultima_trovata_ = unita;   // DS:475A all'avvio dello script (2011:0289)
    const bool modellato_prima = lancio_modellato_;   // il MISSILE dell'IA del modello vale fino alla fine di questo script
    lua_rawgeti(L_, LUA_REGISTRYINDEX, rif_script_);
    lua_getfield(L_, -1, gestore.c_str());
    if (lua_isfunction(L_, -1)) {
        lua_pushinteger(L_, unita);
        int argomenti = 1;
        if (parametri) {   // gestore del giocatore: tabella dei parametri del comando (p)
            lua_createtable(L_, static_cast<int>(parametri->size()), 0);
            for (std::size_t i = 0; i < parametri->size(); ++i) {
                if (const auto* d = std::get_if<double>(&(*parametri)[i])) lua_pushnumber(L_, *d);
                else lua_pushstring(L_, std::get<std::string>((*parametri)[i]).c_str());
                lua_rawseti(L_, -2, static_cast<lua_Integer>(i + 1));
            }
            argomenti = 2;
        }
        if (chiama_lua(argomenti, 0) != LUA_OK) {
            const std::string e = lua_tostring(L_, -1) ? lua_tostring(L_, -1) : "errore";
            if (e.find(FINE_SCRIPT) == std::string::npos) errori_.push_back(gestore + ": " + e);
            lua_pop(L_, 1);
        }
    } else {
        errori_.push_back("script mancante: " + gestore);
        lua_pop(L_, 1);
    }
    lua_pop(L_, 1);
    lancio_modellato_ = modellato_prima;
    contesti_.pop_back();
}

bool Missione::ostili(const UnitaClassica& a, const UnitaClassica& b) const {
    const int la = a.flag & 0x18, lb = parte_nota(a, b);   // a giudica b da quello che sa (il cambio di parte, D4)
    // M7: la parte del luogo e' ostile a chi e' con lei in guerra o in crisi (la sua situazione per coppia)
    if (la == 0x18 || lb == 0x18) return la != lb && forze_ && situazione_fra(la, lb) != Situazione::pace;
    return (la ^ lb) == 0x18;
}

std::vector<int> Missione::lista() const {
    // la lista delle unita' dell'originale (DS:1F58): dalla piu' recente alla piu' vecchia, quindi per id decrescente
    std::vector<int> l;
    for (auto it = unita_.rbegin(); it != unita_.rend(); ++it)
        if (!it->second.distrutta && !it->second.persa) l.push_back(it->first);
    return l;
}

std::size_t Missione::posto(const std::vector<int>& l, int id) {
    // un riferimento a un'unita' uscita dal gioco (o -1) vale la testa, come quelli che 003C rimanda li' (00B4-00DB)
    const auto p = std::find(l.begin(), l.end(), id);
    return p == l.end() ? 0 : static_cast<std::size_t>(p - l.begin());
}

void Missione::passo_sensori() {
    // Cadenza della VM (2011:07BC-0839, 1350; §3.1). La lista delle unita' va dalla piu' recente alla piu' vecchia
    // (le nuove entrano in testa), qui per id decrescente. A ogni passo un solo osservatore: la ricerca parte da
    // DS:1F5C e salta chi ha il flag 40h e le unita' dell'IA che affondano; alle altre scende il conto dei giri +04
    // e osserva la prima che arriva a zero. L'osservatore valuta DS:1F56 bersagli (le unita' create nella missione,
    // quindi tutte le altre e, se ne sono uscite, qualcuna due volte) dal suo cursore +12, saltando se stesso.
    std::erase_if(eventi_, [this](const EventoAcustico& e) { return e.fine <= tempo_; });
    const auto l = lista();
    if (l.size() < 2) return;
    std::size_t i = posto(l, osservatore_);
    int al = -1;
    UnitaClassica* o = nullptr;
    for (; i < l.size(); ++i) {
        auto& u = unita_.at(l[i]);
        if ((u.flag & 0x40) || (u.affondamento > 0 && u.id >= 2)) continue;
        al = u.tipo;
        u.conto_tipo = (u.conto_tipo - 1) & 0xFF;
        if (u.conto_tipo == 0) { u.conto_tipo = u.tipo; o = &u; break; }
    }
    if (!o) {   // fine della lista: osserva la testa senza controlli e prende il +04 dell'ultima esaminata (difetto)
        i = 0;
        o = &unita_.at(l[0]);
        if (al >= 0) o->tipo = al;
        o->conto_tipo = o->tipo;
    }
    const int ido = o->id;
    // 07D8-07F7: il contatore della scia scende solo nel turno del giocatore locale; a 0 torna a 30 e registra lui
    if (const auto* g = giocatore(); g && g->id == ido && --conto_scia_ == 0) { conto_scia_ = 30; registra_scia(ido, o->pos); }
    std::size_t j = posto(l, o->prossimo_bersaglio);
    // il traffico civile: ogni osservatore lo valuta un turno su TRAFFICO_OGNI_TURNI (docs/20 §3, il costo: 40 navi in
    // piu' a ogni turno raddoppiavano il tempo dei sensori); i contatti sui mercantili crescono piu' piano
    const bool col_traffico = !id_traffico_.empty() && (turni_traffico_[ido]++ % TRAFFICO_OGNI_TURNI) == 0;
    for (int n = std::max(1, create_); n > 0; j = (j + 1) % l.size()) {
        if (l[j] == ido) continue;
        --n;
        if (!trova(ido)) break;   // l'osservatore e' uscito dal gioco durante uno script
        auto* b = trova(l[j]);
        if (!b || ((o->flag & 0x20) && (b->flag & 0x20))) continue;   // 0C25: fra due armi ne' colpo ne' scoperta
        if (!col_traffico && id_traffico_.count(b->id)) continue;
        // 0C33-0C51: se nella lista dei contatti dell'osservatore, prima di quello del bersaglio, ce n'e' uno rimasto
        // senza unita' (003C ne azzera il puntatore), lo libera come un contatto perso (127B: bersaglio, aggancio,
        // inseguimento e script +56) e la valutazione finisce li'
        if (const int orfano = contatto_orfano(*o, b->id); orfano >= 0) { perdi_contatto(*o, orfano); continue; }
        // un colpo (0B2D) restituisce 1 e chiude il passo (0818): il cursore +12 e DS:1F5C restano dove erano
        if (impatti_originale_ && impatto(*o, *b)) return;
        valuta(*o, *b);
    }
    o->prossimo_bersaglio = l[j];
    osservatore_ = i + 1 < l.size() ? l[i + 1] : -1;   // la prossima ricerca parte da chi segue (o dalla testa)
}

void Missione::registra_scia(int id, Vec2 pos) {   // 12EE: anello di 150 voci da 6 byte
    if (scia_.size() < 150) scia_.push_back({id, pos});
    else scia_[indice_scia_] = {id, pos};
    indice_scia_ = (indice_scia_ + 1) % 150;
}

// 10AF-10C8 e 10EA-1103: il lampeggio riparte nascosto per n = (550 - segnale)/8 + 1 valutazioni
void Contatto::lampeggio_da_capo(double s) {
    conto_giu = (550 - static_cast<int>(s)) / 8 + 1;
    conto_su = 0;
    lampeggio = conto_giu | 0x80;
}

// 1106-112B: un passo del lampeggio (byte come nell'originale)
void Contatto::passo_lampeggio() {
    int al = (lampeggio - 1) & 0xFF;
    if (lampeggio & 0x80) {
        if (!(al & 0x80)) {   // fine della fase nascosta: visibile per conto_su + 1 valutazioni
            al = conto_su;
            conto_giu = (conto_giu - 1) & 0xFF;
            conto_su = (conto_su + 1) & 0xFF;
        }
    } else if (al & 0x80) {   // fine della fase visibile: nascosto per conto_giu + 1, o visibile per sempre
        al = conto_giu ? conto_giu | 0x80 : 0;
    }
    lampeggio = al;
}

void Missione::valuta(UnitaClassica& o, UnitaClassica& b) {
    // Modello di scoperta della VM (2011:0C23-1180), docs/originale/sensori_e_navigazione.md §3: distanze in quarti
    // di unita' di gioco per asse (1 nm = 200), limite 20 nm; scoperta automatica; radar/ESM contro periscopio e
    // antenna; ascolto passivo solo nel settore di prua (+-30 gradi, +-90 con la schiera) con portata additiva
    // T = G + S*(Nb + 40 - v_oss) + E, termoclini (G/2, rumore/4) ed eventi acustici. Poi qualita', durata e
    // posizione stimata del contatto, scelta del bersaglio e contatto perso (§8).
    const double mu = metri_per_unita_;
    const int ido = o.id, idb = b.id;
    const bool o_nave_aereo = (o.flag & 6) != 0;
    const double dxq = std::fabs(o.pos.x - b.pos.x) / mu * 4.0, dyq = std::fabs(o.pos.y - b.pos.y) / mu * 4.0;
    bool automatica = b.collegata == ido;                                   // armi del proprio lanciatore
    // di notte, con le luci dell'Enhanced, solo entro la portata notturna (luci o sagoma: missione_luci.cpp)
    if (o_nave_aereo && b.quota <= 0.381 && dxq <= 1800 && dyq <= 1800 && vista_automatica_di_notte(o, b)) automatica = true;   // in superficie, 9 nm
    if ((o.flag54 & 2) && (b.flag & 6) && dxq <= 1800 && dyq <= 1800 && vista_automatica_di_notte(o, b)) automatica = true;    // a vista dal periscopio
    // radar/ESM di navi e aerei contro periscopio (900 quarti) e antenna (+450) alzati
    double bonus = 0.0;
    if (o_nave_aereo && (b.flag54 & 6)) {
        const double portata = ((b.flag54 & 2) ? 900.0 : 0.0) + ((b.flag54 & 4) ? 450.0 : 0.0);
        if (dxq <= portata && dyq <= portata) {
            // 0D3E-0D72: livello 85 - (|dx| + |dy|)/32 in quarti, bonus (DS:476A) un quarto del livello, intero
            const int livello = 85 - ((static_cast<int>(dxq) + static_cast<int>(dyq)) >> 5);
            bonus = livello >> 2;
            // per il giocatore locale l'ultimo livello, senza massimo ne' decadimento
            if (const auto* g = giocatore(); g && g->id == idb) esm_ = livello;
        }
    }
    // Enhanced: equazione del sonar; con l'ESM ripensato l'intercettazione dell'originale lascia il posto ai radar accesi
    // ASW aereo dell'Enhanced: contro un sottomarino immerso l'aereo controllato sa quello che sa il suo controllore
    const std::optional<double> da_asw = asw_ ? segnale_asw(o, b) : std::nullopt;
    if (!da_asw && catalogo_ && ascolta_con_acustica(o, b, automatica, bonus > 0.0 && !catalogo_el_)) return;
    double segnale = -1.0;   // < 0: nessuna scoperta
    if (da_asw) {
        segnale = *da_asw;
    } else if (automatica) {
        segnale = 6000.0;
    } else if (o.sonar > 0 && b.rumore > 0 && dxq < 4000 && dyq < 4000) {
        const double rel = std::fabs(normalizza_180(o.rotta - rilevamento(o.pos, b.pos)));   // 0..180 gradi
        const double rel_vm = rel * 25.0;                                                  // 9000 = 360 gradi
        double G = (o.flag54 & 1) ? (rel_vm < 2500 ? 2.0 * std::floor((2500 - rel_vm) / 256) : 0.0)
                                  : (rel_vm < 1000 ? std::floor((1000 - rel_vm) / 256) : 0.0);
        double E = 0.0;
        for (const auto& e : eventi_) {
            if (e.unita == idb) E += 1000.0;   // il bersaglio si e' rivelato (difetto: quasi sempre 1000)
            if (e.unita == ido && rel <= 72.0) E += rel <= 36.0 ? 2200.0 : (rel <= 52.0 ? 1400.0 : 800.0);   // ping
        }
        if (G > 0.0 || E > 0.0) {
            const double f = b.velocita > b.avanti_tutta ? 3.0 : 1.0;
            double Nb = (1.0 + std::floor(8.0 * b.velocita / std::max(b.avanti_tutta, 0.1)) * f) * b.rumore;
            if (std::fabs(b.velocita - b.velocita_ordinata) > 0.01) Nb = 1.0;   // difetto DS:1F64 dell'originale
            double Nt = (Nb + std::max(0.0, 40.0 - std::floor(32.0 * o.velocita / std::max(o.avanti_tutta, 0.1)))) * o.sonar;
            for (double strato : termoclini_)
                if ((o.quota < strato) != (b.quota < strato)) { G = std::floor(G / 2.0); Nt = std::floor(Nt / 4.0); }
            const double T = G + Nt + E, dxu = dxq / 4.0, dyu = dyq / 4.0;
            if (T > dxu && T > dyu) segnale = T - (dxu + dyu) / 2.0;
        }
        // 0EDC e 1003-1016: senza ascolto (G ed E nulli) o fuori portata passiva l'intercettazione da' comunque un
        // contatto con segnale 550; dentro la portata vale il segnale passivo, anche se minore
        if (segnale < 0.0 && bonus > 0.0) segnale = 550.0;
    }
    auto it = o.contatti.find(idb);
    if (segnale < 0.0) {
        // contatto perso alla prima valutazione senza scoperta; le unita' lanciate da un'altra (+0C) lo tengono
        // con segnale 1 finche' la durata e' <= 450
        if (it == o.contatti.end()) return;
        if (o.collegata >= 0 && it->second.durata <= 450.0) { it->second.segnale = 1.0; return; }
        perdi_contatto(o, idb);
        return;
    }
    if (it == o.contatti.end()) {   // contatto nuovo: conto 1
        Contatto* n = nuovo_contatto(o, idb);
        if (!n) return;
        if (segnale > 500.0) n->qualita = std::floor((segnale - 500.0) / 4.0);
        else n->lampeggio_da_capo(segnale);
        n->segnale = segnale;   // il nuovo passa subito dall'aggiornamento (10CB-10D7), con differenza di segnale 0
        it = o.contatti.find(idb);
    } else if (automatica) {
        it->second.durata = 1400.0;   // 0DA4: solo un contatto che c'e' gia'
    }
    auto& c = it->second;
    const double prima = c.segnale;
    c.segnale = segnale;
    c.ultima_scoperta = tempo_;
    c.rilevamento = rilevamento(o.pos, b.pos);   // +04 (1131, 1226)
    if (segnale <= 500.0) {
        // 10DF-1134: solo il lampeggio, poi la valutazione finisce (niente posizione stimata ne' scelta del bersaglio)
        if (std::fabs(segnale - prima) > 5.0) { c.qualita = 0.0; c.lampeggio_da_capo(segnale); }
        c.passo_lampeggio();
        return;
    }
    c.durata += 1.0;
    if (o.flag54 & 2) c.durata = std::max(c.durata, 800.0);
    // 114C-116D: (differenza/4 + 1) col segno della differenza, mai sotto 0, poi il bonus radar/ESM; senza limite a 1200
    const double d = segnale - prima;
    c.qualita = std::max(0.0, c.qualita + (d < 0.0 ? -(std::floor(-d / 4.0) + 1.0) : std::floor(d / 4.0) + 1.0)) + bonus;
    if (c.qualita >= 800.0) {
        c.pos_stimata = b.pos;
        c.quota_stimata = b.quota;
        c.posizione_nota = true;
        c.t_posizione = tempo_;
        // 1207-1220: nel turno in cui il giocatore locale ha registrato se stesso, anche i suoi contatti solidi
        if (conto_scia_ == 30)
            if (const auto* g = giocatore(); g && g->id == ido) registra_scia(idb, b.pos);   // posizione vera
    }
    scelta_bersaglio(o, b, c);
}

// 2011:1043-108C: record nuovo nella lista dell'osservatore (conto 1), o nullptr con il pool pieno
Contatto* Missione::nuovo_contatto(UnitaClassica& o, int idb) {
    // il pool dei contatti (DS:2C52) ha 300 record, e il contatore si ferma a 299: pieno, niente contatto
    std::size_t in_uso = 0;
    for (const auto& [id, u] : unita_) if (!u.distrutta && !u.persa) in_uso += u.contatti.size();
    if (in_uso >= 299) return nullptr;
    Contatto c;
    c.ordine = contatti_creati_++;
    c.prima_scoperta = tempo_;
    if (o.id < 2) {   // 1056-1083: ai giocatori la lettera piu' bassa non ancora usata nei loro contatti
        c.lettera = 'A';
        for (bool usata = true; usata;) {
            usata = false;
            for (const auto& [k, x] : o.contatti) if (x.lettera == c.lettera) { ++c.lettera; usata = true; break; }
        }
    }
    return &o.contatti.emplace(idb, c).first->second;
}

// --- acustica dell'Enhanced (piano: note della sessione, piano-acustica-enhanced.md) ---

namespace {

// messaggi per il giocatore (testi in tools/pipeline/esporta_motore.py)
constexpr int MSG_NUOVO_CONTATTO = 310, MSG_SILURO_IN_ACQUA = 311, MSG_CI_PINGANO = 312, MSG_CAVITIAMO = 313,
              MSG_SCHIERA_CIECA = 314;
// avvisi della stazione sonar dell'Enhanced (S3) che vengono dalla simulazione (342 e 343 li da' la stazione)
constexpr int MSG_CLASSIFICATO_SOTTOMARINO = 340, MSG_CLASSIFICATO_NAVE = 341, MSG_CONTATTO_CAVITA = 344,
              MSG_TRANSITORIO = 345, MSG_BIOLOGICO = 346, MSG_RIGA_LOFAR = 347;
constexpr double DURATA_CLASSIFICATO = 800.0;   // come la barra del bersaglio e la carta (990C, 02h)
constexpr double QUALITA_PER_DB_S = 0.55;   // +10 dB stabili: traccia solida (800) in circa due minuti e mezzo
constexpr double SE_AUTOMATICA = 30.0, SE_ESM = 5.0, SE_CLASSIFICA = 3.0;
constexpr double TAU_PASSIVO_S = 300.0, SIGMA_DB = 4.0;   // dossier 02 §1.6: 2-10 minuti, 3-6 dB
constexpr double SCHIERA_VIRATA = 0.3;      // gradi/s oltre cui la schiera non da' rilevamenti (docs/07 §3.4)
constexpr double INTERVALLO_PING_S = 10.0;   // fra due ping (l'IA e, nell'Enhanced, il giocatore): l'eco da ~7,5 km (2R/c; STIMA)
constexpr int MSG_PING_PRESTO = 414;            // «il ping precedente non e' ancora tornato» (esporta_motore.py)

}  // namespace

std::shared_ptr<const LibreriaCampioni> libreria_campioni(const std::string& cartella_dati) {
    static std::mutex guardia;
    static std::map<std::string, std::shared_ptr<const LibreriaCampioni>> caricate;
    const std::lock_guard<std::mutex> chiave(guardia);
    auto& l = caricate[cartella_dati];
    if (!l) {
        std::filesystem::path cartella(cartella_dati);
        if (!cartella.has_filename()) cartella = cartella.parent_path();   // "data/motore/"
        try {
            l = std::make_shared<const LibreriaCampioni>(carica_campioni(
                (cartella / "campioni.toml").string(), cartella.parent_path().string()));
        } catch (const std::exception&) {
            l = std::make_shared<const LibreriaCampioni>();   // un WAV che non si legge: il modello di sempre
        }
    }
    return l;
}

void Missione::acustica_enhanced(const std::string& cartella_dati) {
    if (catalogo_) return;
    try {
        catalogo_ = std::make_unique<CatalogoAcustico>(carica_acustica(cartella_dati + "/acustica.toml"));
    } catch (const std::exception&) {
        return;   // senza i dati resta la scoperta del classico
    }
    try { clima_ = carica_clima(file_clima(cartella_dati, teatro_)); } catch (const std::exception&) {}
    if (usa_campioni_) {   // R5: l'attivita' delle voci registrate segue l'inviluppo del campione, come la cuffia
        campioni_ = libreria_campioni(cartella_dati);
        if (!campioni_->vuota()) collega_campioni(*catalogo_, *campioni_);
    }
    carica_dimensioni_classi(cartella_dati);   // le dimensioni degli scafi per l'implosione (R7)
    caso_acustica_ = Casuale(seme_ ^ 0x50A2A50A2ull);
    try {   // l'intercettazione (blocco 2 del sonar, E): [intercettatori.*] ed [emissioni.*] di acustica.toml
        catalogo_int_ = std::make_unique<CatalogoIntercettazione>(carica_intercettazione(cartella_dati + "/acustica.toml"));
    } catch (const std::exception&) {}
    cartella_dati_traffico_ = cartella_dati;   // le rotte del traffico si leggono al primo passo, se acceso
    dati_fondale(cartella_dati);   // l'ecoscandaglio e il BQS-15 sul fondo del giocatore (missione_fondale.cpp)
}

// Sensori sonar dell'unita' (suite della classe in acustica.toml): passivi d'ascolto, attivi, o d'intercettazione
// degli impulsi; la schiera solo filata. Navi senza suite: il sonar dell'originale (+3B) per schieramento
std::vector<const Sensore*> Missione::sensori_acustici(const UnitaClassica& u, TipoSensori tipo) const {
    std::vector<const Sensore*> v;
    if (!catalogo_ || (u.flag & 0x20) || (u.flag & 3) == 3 || (u.flag & 0x80)) return v;   // armi, aerei, esche
    auto suite = catalogo_->sensori_di(u.classe);
    // il ripiego per le navi senza voce in suite; una suite dichiarata vuota (portaerei, ausiliarie, civili: acustica.py,
    // docs/ricerca/35 §6) vuol dire nessun sonar, anche col sonar del modello del classico (la Nimitz ha 4)
    if (suite.empty() && !catalogo_->suite.count(u.classe) && u.categoria == "nave" && u.sonar > 0) {
        const bool occidentale = !(u.flag & 0x10);
        for (const char* id : occidentale ? std::initializer_list<const char*>{"sqs53", "sqs53_attivo"}
                                          : std::initializer_list<const char*>{"sovietico_3gen", "titan2_attivo"})
            if (const Sensore* x = catalogo_->sensore(id)) suite.push_back(x);
    }
    for (const Sensore* x : suite) {
        const bool attivo = x->modo == ModoSensore::attivo;
        const bool intercettazione = x->id.rfind("wlr", 0) == 0 || x->id.rfind("intercettazione", 0) == 0;
        if (x->antenna == Antenna::lineare && !(u.flag54 & 1)) continue;   // schiera non filata
        if (x->antenna == Antenna::lineare && u.giocatore && !schiera_danni_ok_) continue;   // guasta (i danni del giocatore)
        if (x->antenna == Antenna::lineare && !u.giocatore && schiera_ia_giu_.count(u.id)) continue;   // e quelli dell'IA
        // il vetrino di Dio: il sensore spento (fattore 0) non c'e'; a meta' portata lo attenua attenuazione_guasto
        if (!interventi_.empty() && !intercettazione) {
            const auto f = parametro_attivo(interventi_, stati_interventi_, u.id, "sensore", "fattore", x->antenna == Antenna::lineare ? "schiera" : "sonar");
            if (f && *f <= 0.0) continue;
        }
        // il sonar di scafo dell'IA guasto dai danni: tace la cupola (non il VDS ne' gli idrofoni d'intercettazione)
        if (x->antenna != Antenna::lineare && !u.giocatore && !intercettazione && x->id.find("vds") == std::string::npos && sonar_ia_giu_.count(u.id))
            continue;
        if (x->antenna != Antenna::lineare && x->id.find("vds") == std::string::npos && sonar_scafo_perso_.count(u.id)) continue;   // duomo perso
        if (x->id.find("vds") != std::string::npos && !vds_in_acqua(u.id)) continue;   // il pesce dentro (missione_vds.cpp)
        if (tipo == TipoSensori::attivi ? attivo : tipo == TipoSensori::ascolto ? !attivo && !intercettazione : !attivo)
            v.push_back(x);
    }
    return v;
}

// Sorgente dell'unita' per i sonar (nucleo/acustica.hpp, la stessa per sensori e siluri): velocita' vera,
// cavitazione dalla dinamica se c'e', transitori della lista degli eventi, posato. Nessuna per gli aerei.
std::optional<Sorgente> Missione::sorgente_giocatore() const {
    const auto* g = giocatore();
    return g ? sorgente_acustica(*g) : std::nullopt;
}

std::optional<Sorgente> Missione::sorgente_acustica(const UnitaClassica& u) const {
    if (!catalogo_) return std::nullopt;
    UnitaAcustica a;
    a.id = u.id;
    a.classe = u.classe;
    a.categoria = (u.flag & 3) == 3 ? "aereo" : u.categoria;
    a.pos = u.pos;
    a.quota_m = u.quota;
    a.rotta = u.rotta;
    a.velocita_kn = u.velocita;
    if (const auto* d = dinamica(u.id)) a.cavitazione = d->cavita();
    for (const auto& e : eventi_) if (e.unita == u.id) a.transitorio_db = 10.0;   // lancio, portelli, ping
    a.sul_fondo = u.sul_fondo || relitti_.count(u.id);   // il relitto: l'eco si confonde col fondo
    a.falso_bersaglio = (u.flag & 0x80) != 0;
    a.snorkel = batterie_attive_ && snorkel(u.id);   // M6: i diesel dello snorkel
    if (traffico_)   // la nave del traffico: la firma dell'individuo (nucleo/navi_civili)
        if (const auto t = id_traffico_.find(u.id); t != id_traffico_.end()) a.firma = traffico_->firma(t->second);
    const auto mobile = esche_mobili_.find(u.id);
    const bool falso = mobile != esche_mobili_.end() && !mobile->second.dati;   // il falso contatto del vetrino di Dio (D3)
    const bool imita = mobile != esche_mobili_.end() && (falso || mobile->second.dati->imita_rumore);
    if (imita) {   // l'esca semovente suona come il suo lanciatore alla propria velocita' (dossier 05 §5.2)
        a.classe = mobile->second.classe_lanciatore;
        a.categoria = falso ? mobile->second.categoria : "sottomarino";   // un falso mercantile resta nave
        a.falso_bersaglio = false;
        if (falso) a.fedelta = mobile->second.fedelta;
    }
    // R6: solo navi e sottomarini veri (non armi, esche, biologici); seme stabile per unita' nella missione
    if (variazioni_acustiche() && (u.categoria == "nave" || u.categoria == "sottomarino") && !a.falso_bersaglio && !(u.flag & 0x20)) {
        a.variazioni = true;
        std::uint64_t z = seme_ ^ (static_cast<std::uint64_t>(u.id) * 0x9E3779B97F4A7C15ull);
        z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ull;
        a.seme = z ^ (z >> 27);
        a.tempo_s = tempo_;
        a.rateo_virata_deg_s = std::fabs(rateo_virata_vero(u));
        if (const auto it = danni_acustici_.find(u.id); it != danni_acustici_.end()) {
            a.danno_elica = it->second.first;
            a.danno_macchine = it->second.second;
        }
        if (const auto it = urto_acustico_.find(u.id); it != urto_acustico_.end()) a.urto_db = it->second;   // le collisioni
    }
    Sorgente s = ::m688::sorgente_acustica(*catalogo_, a);
    if (!s.firma) return std::nullopt;
    if (relitti_.count(u.id)) s.muta = true;   // scheda I10: solo eco, niente rumore
    if (imita) { s.esca = true; s.lunghezza_m = 0.0; }   // resta un falso bersaglio puntiforme
    return s;
}

Ricevitore Missione::ricevitore_acustico(const UnitaClassica& u, const Sensore* s) const {
    Ricevitore r;
    r.pos = u.pos;
    r.quota_m = u.quota;
    r.rotta = u.rotta;
    r.velocita_kn = u.velocita;
    r.rateo_virata_deg_s = std::fabs(rateo_virata_vero(u));
    if (const auto it = fine_virata_.find(u.id); it != fine_virata_.end()) r.tempo_da_virata_s = tempo_ - it->second;
    r.sul_fondo = u.sul_fondo;
    r.sensore = s;
    // la schiera trainata del modello (missione_schiera.cpp): alla sua quota, dietro la poppa, col cavo e lo sfregamento
    if (s && s->antenna == Antenna::lineare)
        if (const auto it = schiere_.find(u.id); it != schiere_.end() && it->second.cavo_m > 0.0) {
            const SchieraUnita& x = it->second;
            const double a = radianti(u.rotta);
            r.pos = {u.pos.x - x.forma.dietro_m * std::sin(a), u.pos.y - x.forma.dietro_m * std::cos(a)};
            r.quota_m = x.forma.quota_centro_m;
            r.cavo_filato_m = x.cavo_m;
            r.sfregamento = x.fondo.frazione_schiera;
            r.fondo_sfregamento = x.tipo_fondo;
            r.schiera_danneggiata = x.danneggiata;   // -6 dB e niente sopra 300 Hz (dossier 04 §6.3), nell'acustica
        }
    // il pesce del VDS delle navi (missione_vds.cpp): alla sua quota, dietro la poppa
    if (s && s->id.find("vds") != std::string::npos)
        if (const auto it = traini_.find(u.id); it != traini_.end() && it->second.vds.cavo_m > 0.0) {
            const double a = radianti(u.rotta), d = it->second.vds.forma.dietro_m;
            r.pos = {u.pos.x - d * std::sin(a), u.pos.y - d * std::cos(a)};
            r.quota_m = it->second.vds.forma.quota_centro_m;
        }
    // le righe del proprio battello (blocco 2 del sonar, F): la sua sorgente, una volta per passo
    if (s) {
        auto& p = sorgenti_proprie_[u.id];
        if (p.first != tempo_ || !p.second) p = {tempo_, sorgente_acustica(u)};
        if (p.second) r.proprio = &*p.second;
    }
    return r;
}

constexpr double METEO_CELLA_M = 5000.0, METEO_CELLA_S = 10.0;   // il meteo di chi ascolta (docs/18 fase 2)
constexpr double POMERIGGIO_OGNI_S = 600.0;   // l'effetto pomeriggio si ricontrolla ogni 10 minuti di gioco

Ambiente Missione::ambiente_acustico(Vec2 a, Vec2 b) const {
    Ambiente amb;
    if (clima_) {
        amb.profilo = profilo_in(a, b);
        amb.traffico = clima_->traffico;
        amb.ph = clima_->ph;
    }
    if (fondale_) amb.fondale_m = std::min(profondita(a), profondita(b));
    if (clima_) {   // il fondo per teatro e profondita' e i gamberi dell'acqua bassa e calda (stima dell'acustica)
        amb.fondo = clima_->fondo(amb.fondale_m);
        amb.gamberi = gamberi_attivi(amb.fondale_m, amb.profilo->temperatura_superficie);
    }
    if (meteo_) {   // il meteo nel punto di chi ascolta (M2; docs/18 fase 2): vento per il mare e il rumore, pioggia, ghiaccio
        const StatoMeteo qui = meteo_ascolto(a);
        amb.vento_m_s = qui.vento_m_s;
        amb.pioggia_mm_h = qui.pioggia_mm_h;
        amb.ghiaccio = ghiaccio_ && ghiaccio_->modificato() ? ghiaccio_->copertura_in(a, tempo_) : qui.ghiaccio;
        carattere_ghiaccio(amb, a, qui.t_aria_c);
        if (!meteo_->fisso()) amb.hs_m = qui.hs_m();   // il mare dalla Hs vera (§8.3); il semplificato resta col vento
    }
    return amb;
}

// Il profilo di un percorso: quello del mese col pomeriggio e, sopra, gli strati cambiati del vetrino di Dio (D4) la cui zona
// contiene il punto medio. Il passo della transizione viene dal tempo a inizio passo, quindi e' deterministico
const ProfiloSuono* Missione::profilo_in(Vec2 a, Vec2 b) const {
    if (!clima_) return nullptr;
    const int mese = mese_acustica();
    const ProfiloSuono* p = quarto_pomeriggio_ > 0 && mese == mese_pomeriggio_ && pomeriggio_[static_cast<std::size_t>(quarto_pomeriggio_)]
                              ? &*pomeriggio_[static_cast<std::size_t>(quarto_pomeriggio_)] : &clima_->profilo(mese);
    if (zone_strato_.empty()) return p;
    const Vec2 medio{(a.x + b.x) / 2.0, (a.y + b.y) / 2.0};
    for (std::size_t i = 0; i < zone_strato_.size(); ++i) {
        const ZonaStrato& z = zone_strato_[i];
        if (z.raggio_m > 0.0 && distanza(medio, z.centro) > z.raggio_m) continue;
        const int passo = static_cast<int>(std::lround(4.0 * fase_cambiamento(tempo_, z.t0, z.transizione_s, z.durata_s)));
        if (passo <= 0) continue;
        auto& q = profili_strato_[{p, i, passo}];
        if (!q) q = std::make_unique<ProfiloSuono>(modifica_profilo(*p, z.modifica, passo / 4.0));
        p = q.get();
    }
    return p;
}

bool Missione::strato_applicabile(ModificaStrato::Tipo t) const {
    return clima_ && modifica_applicabile(clima_->profilo(mese_acustica()), t);
}

double Missione::sld_in(Vec2 p) const {
    if (!clima_) return 0.0;
    return zone_strato_.empty() ? clima_->profilo(mese_acustica()).sld_m : profilo_in(p, p)->sld_m;
}

// L'effetto pomeriggio: dall'ora solare (UTC piu' la longitudine), dal vento e dalle nubi nel punto del giocatore,
// l'intensita' a quarti; il profilo di ogni quarto si fa la prima volta che serve
void Missione::passo_pomeriggio() {
    if (tempo_ < prossimo_pomeriggio_) return;
    prossimo_pomeriggio_ = tempo_ + POMERIGGIO_OGNI_S;
    const int mese = mese_acustica();
    if (mese != mese_pomeriggio_) {
        for (auto& p : pomeriggio_) p.reset();
        mese_pomeriggio_ = mese;
    }
    const double ora_locale = std::fmod((ora_utc_s_ + tempo_) / 3600.0 + longitudine_ / 15.0 + 48.0, 24.0);
    const double vento = meteo_ ? meteo_qui_.vento_m_s : 5.0, nubi = meteo_ ? meteo_qui_.nubi_ottavi / 8.0 : 0.5;
    const int quarto = static_cast<int>(std::lround(4.0 * std::clamp(intensita_pomeriggio(ora_locale, vento, nubi), 0.0, 1.0)));
    auto& p = pomeriggio_[static_cast<std::size_t>(quarto)];
    if (quarto > 0 && !p) p = applica_pomeriggio(clima_->profilo(mese), quarto / 4.0);
    quarto_pomeriggio_ = quarto;
}

StatoMeteo Missione::meteo_in(Vec2 pos) const {
    StatoMeteo s = meteo_ ? meteo_->in(pos) : meteo_qui_;
    if (!interventi_.empty()) zone_meteo(interventi_, stati_interventi_, pos, s);   // le zone del vetrino di Dio
    return s;
}

StatoMeteo Missione::meteo_ascolto(Vec2 p) const {
    // vicino al giocatore il suo (le sue scoperte restano quelle di prima); altrove la cella di 5 km, ogni 10 s
    if (const auto* g = giocatore(); g && distanza(g->pos, p) < METEO_CELLA_M / 2.0) return meteo_qui_;
    if (!meteo_) return meteo_qui_;   // senza il meteo dell'Enhanced (una prova che accende i periscopi prima) quello di qui
    const std::pair<int, int> k{static_cast<int>(std::floor(p.x / METEO_CELLA_M)), static_cast<int>(std::floor(p.y / METEO_CELLA_M))};
    auto it = meteo_celle_.find(k);
    if (it == meteo_celle_.end() || tempo_ - it->second.first >= METEO_CELLA_S || tempo_ < it->second.first) {
        const Vec2 centro{(k.first + 0.5) * METEO_CELLA_M, (k.second + 0.5) * METEO_CELLA_M};
        it = meteo_celle_.insert_or_assign(k, std::make_pair(tempo_, meteo_->in(centro))).first;
    }
    if (interventi_.empty()) return it->second.second;
    StatoMeteo s = it->second.second;   // le zone del vetrino di Dio sopra la cella
    zone_meteo(interventi_, stati_interventi_, p, s);
    return s;
}

double Missione::rateo_virata_vero(const UnitaClassica& u) const {
    if (const auto* d = dinamica(u.id)) return d->rateo_virata_deg_s();
    return u.rateo_virata;
}

double Missione::gauss() {
    const double u1 = std::max(caso_acustica_.uniforme(), 1e-12), u2 = caso_acustica_.uniforme();
    return std::sqrt(-2.0 * std::log(u1)) * std::cos(2.0 * 3.14159265358979323846 * u2);
}

void Missione::messaggio_sonar(int codice) {
    const auto* g = giocatore();
    if (!g) return;
    for (const auto& x : messaggi_sonar_) if (x.codice == codice && x.tempo == tempo_) return;
    messaggi_sonar_.push_back({tempo_, g->id, codice});
}

// Scoperta dell'Enhanced per una coppia (§3.1 dell'originale per la cadenza, dossier 02 §1 per il segnale). Falso
// la TMA dell'IA (docs/18 fase 2): un rilevamento ogni TMA_IA_PASSO_S, una soluzione al minuto (STIMA)
constexpr double TMA_IA_PASSO_S = 20.0, TMA_IA_SOLUZIONE_S = 60.0, TMA_IA_ERRORE_MIN_DEG = 0.3;
constexpr int TMA_IA_NON_OSSERVABILI = 3;   // soluzioni di fila (3 minuti) dopo cui la posizione della TMA decade
// l'eco di una chiglia confonde quella del bersaglio dell'IA (STIMA): stesso fascio di un sonar di scafo attivo, stessa
// cella di distanza dell'operatore
constexpr double FASCIO_ECO_DEG = 3.0, CELLA_DISTANZA_ECO_M = 500.0;
constexpr double VIRATA_LATO_DEG = 30.0;   // l'accostata che risolve lo specchio della schiera (STIMA: dossier 02, la
                                           // regola pratica di 30-60 gradi; dopo, il rilevamento falso salta)

// se l'osservatore non ha sonar o il bersaglio non ha firma: allora decide il classico.
bool Missione::ascolta_con_acustica(UnitaClassica& o, UnitaClassica& b, bool automatica, bool esm) {
    const auto sensori = sensori_acustici(o, TipoSensori::ascolto);
    if (sensori.empty()) return false;
    auto s = sorgente_acustica(b);
    if (!s && asw_ && aereo(b)) s = sorgente_aereo_per(b, o);   // aerei: la finestra di Snell, o l'hover
    if (!s) return false;
    const int ido = o.id, idb = b.id;
    const auto* g = giocatore();
    const bool del_giocatore = g && g->id == ido;
    // il sensore migliore (la schiera in virata e appena dopo la penalizza il modulo)
    const Ambiente amb = ambiente_acustico(o.pos, b.pos);
    Rilevazione x;
    const Sensore* usato = nullptr;
    // il traffico oltre i 30 km: il livello A, senza chiedere un campo di raggi (docs/20 §3, il costo)
    const bool col_campo = !id_traffico_.count(idb) || distanza(o.pos, b.pos) <= TRAFFICO_CAMPO_M;
    // la tonale per la classificazione dal sensore che la vede meglio (la schiera filata col suo LOFAR), non da quello che
    // sente di piu' a banda larga (30/10/2026: la Perry con l'Alfa sulla SQR-19 ferma la durata quando l'SQS-53 diventa
    // il migliore, -19 dB di tonale; come le righe del sonarista dell'IA, missione_sonarista.cpp)
    double tonale_migliore = -1e9;
    for (const Sensore* sn : sensori) {
        const Rilevazione r = col_campo ? ::m688::valuta(*s, ricevitore_acustico(o, sn), amb, raggi_, tempo_)
                                        : ::m688::valuta(*s, ricevitore_acustico(o, sn), amb);
        if (r.se_db > x.se_db) { x = r; usato = sn; }
        tonale_migliore = std::max(tonale_migliore, r.se_tonale_db);
    }
    double se = x.se_db;
    if (!interventi_.empty() && usato) se += attenuazione_guasto(interventi_, stati_interventi_, ido, *usato);   // vetrino di Dio
    auto& coppia = coppie_[{ido, idb}];
    const double dt = coppia.ultima < 0.0 ? 0.0 : tempo_ - coppia.ultima;
    coppia.ultima = tempo_;
    coppia.fluttuazione.avanza(dt, TAU_PASSIVO_S, SIGMA_DB, caso_acustica_);
    se += coppia.fluttuazione.db;
    if (usato) possibile(ido, idb, usato->id, se);   // R2 della sandbox
    // sensori non acustici: armi proprie, radar e vista danno posizione; l'intercettazione solo il rilevamento
    std::string da = usato ? usato->id : "";
    if (automatica && se < SE_AUTOMATICA) { se = SE_AUTOMATICA; da = "vista"; }
    if (esm && se < SE_ESM) { se = SE_ESM; da = "esm"; }

    auto it = o.contatti.find(idb);
    if (it == o.contatti.end()) {
        if (se < 0.0) return true;
        if (!nuovo_contatto(o, idb)) return true;
        it = o.contatti.find(idb);
        // col sonarista il nuovo contatto lo riferisce lui (410); il siluro in acqua resta un allarme
        const bool siluro = (b.flag & 0x20) && !(b.flag & 0x80);
        if (del_giocatore && (siluro || !esperienza_sonarista_)) messaggio_sonar(siluro ? MSG_SILURO_IN_ACQUA : MSG_NUOVO_CONTATTO);
    }
    Contatto& c = it->second;
    c.qualita = std::clamp(c.qualita + QUALITA_PER_DB_S * std::clamp(se, -10.0, 20.0) * dt, 0.0, 1500.0);
    c.segnale = 500.0 + 25.0 * se;
    c.eccesso_db = se;
    c.lampeggio = se < 0.0 ? 0x80 : 0;
    if (c.qualita <= 0.0 && se < 0.0) { perdi_contatto(o, idb); return true; }
    if (se < 0.0) return true;
    c.ultima_scoperta = tempo_;
    c.sensore = da;
    // rilevamento misurato con l'errore del sensore; la schiera da' anche lo specchio rispetto al suo asse
    const double errore = usato && x.se_db > -1e8 ? errore_rilevamento_deg(x, *usato) : 0.5;
    c.rilevamento = normalizza_360(rilevamento(o.pos, b.pos) + errore * gauss());
    c.rilevamento_specchio.reset();
    if (x.conico) c.rilevamento_specchio = normalizza_360(2.0 * o.rotta - c.rilevamento);
    // l'IA non sa da che lato sta il rilevamento della schiera, come il giocatore (simmetria, punto 4): al primo ne
    // crede uno a caso; un'accostata o un sensore senza specchio lo risolvono, e la TMA fatta sul lato falso si rifa'
    if (!o.giocatore) {
        bool risolto = false;
        if (c.rilevamento_specchio) {
            if (!coppia.lato_vero) {
                Casuale lato(seme_ ^ ((static_cast<std::uint64_t>(ido) + 1) * 0x9E3779B97F4A7C15ULL) ^ ((static_cast<std::uint64_t>(idb) + 3) * 0xC2B2AE3D27D4EB4FULL));
                coppia.lato_vero = lato.uniforme() < 0.5;
                coppia.rotta_lato = o.rotta;
            } else if (!*coppia.lato_vero && std::fabs(normalizza_180(o.rotta - coppia.rotta_lato)) >= VIRATA_LATO_DEG) {
                risolto = true;
            }
        } else if (coppia.lato_vero && !*coppia.lato_vero) {
            risolto = true;
        }
        if (risolto) {
            coppia.lato_vero = true;
            if (const auto t = tma_ia_.find({ido, idb}); t != tma_ia_.end()) {
                if (t->second.data && c.posizione_nota && c.pos_stimata.x == t->second.data->x && c.pos_stimata.y == t->second.data->y) c.posizione_nota = false;
                tma_ia_.erase(t);
            }
        }
        if (coppia.lato_vero && !*coppia.lato_vero && c.rilevamento_specchio) std::swap(c.rilevamento, *c.rilevamento_specchio);
    }
    if (del_giocatore && sonarista_ && usato && da == usato->id) ascolto_per_sonarista(b, *s, usato, se, errore);   // S5d
    if (!del_giocatore && !sonaristi_ia_spenti_ && usato && da == usato->id) ascolto_ia(o, b, *s, usato, se, errore);   // fase 2
    // classificazione: righe leggibili sopra +3 dB; un'esca non arriva alla classe
    // (la tonale piu' forte sul LOFAR, con la fluttuazione della coppia)
    const double tonale = (sensori.empty() ? x.se_tonale_db : std::max(x.se_tonale_db, tonale_migliore)) + coppia.fluttuazione.db;
    const double durata_prima = c.durata;
    if (tonale >= SE_CLASSIFICA) c.durata += dt * (1.0 + tonale / 5.0);
    // l'IA classifica col suo sonarista (docs/18 fase 2): finche' non ha riferito il contatto resta sotto la media,
    // finche' non l'ha classificato sotto la lunga
    if (!del_giocatore && !sonaristi_ia_spenti_ && !automatica && !(o.flag54 & 2)) {
        const auto si = sonaristi_ia_.find(ido);
        const TracciaSonar* t = si != sonaristi_ia_.end() ? si->second.s->traccia_di(idb) : nullptr;
        c.durata = std::min(c.durata, !t ? DURATA_CLASSIFICATO - 1.0 : t->stato == TracciaSonar::Stato::classificata ? 1e9 : 1399.0);
        // e la traccia solida (su cui gli script accostano, ingaggiano e lanciano) arriva solo col suo rapporto
        if (usato && da == usato->id && (!t || t->stato == TracciaSonar::Stato::persa)) c.qualita = std::min(c.qualita, 799.0);
    }
    if (automatica) c.durata = std::max(c.durata, 1400.0);
    if (o.flag54 & 2) c.durata = std::max(c.durata, 800.0);
    // un'esca non arriva alla classe; il falso contatto si' (la firma e' quella della classe imitata, D3)
    const auto falso = esche_mobili_.find(idb);
    const bool e_falso = falso != esche_mobili_.end() && !falso->second.dati;
    if ((b.flag & 0x80) && !e_falso) c.durata = std::min(c.durata, 1399.0);
    // avvisi del sonar al giocatore: classificazione, cavitazione, transitorio e riga LOFAR quando compaiono
    if (del_giocatore) {
        if (!esperienza_sonarista_ && durata_prima < DURATA_CLASSIFICATO && c.durata >= DURATA_CLASSIFICATO) {   // col sonarista: 411
            const std::string& tipo = s->firma->tipo_unita;
            const std::string& categoria = e_falso ? falso->second.categoria : b.categoria;
            if (tipo == "biologico") messaggio_sonar(MSG_BIOLOGICO);
            else if (tipo == "ssn" || tipo == "ssgn" || tipo == "ssbn" || tipo == "ssk" || categoria == "sottomarino")
                messaggio_sonar(MSG_CLASSIFICATO_SOTTOMARINO);
            else if (categoria == "nave") messaggio_sonar(MSG_CLASSIFICATO_NAVE);
        }
        if (x.cavitazione && !c.indizio_cavitazione) messaggio_sonar(MSG_CONTATTO_CAVITA);
        if (x.transitorio && !c.indizio_transitorio) messaggio_sonar(MSG_TRANSITORIO);
        if (tonale >= SE_CLASSIFICA && !c.indizio_lofar) messaggio_sonar(MSG_RIGA_LOFAR);
    }
    c.indizio_cavitazione = x.cavitazione;
    c.indizio_transitorio = x.transitorio;
    c.indizio_lofar = c.indizio_lofar || tonale >= SE_CLASSIFICA;
    // posizione: vista, radar e armi proprie. Col solo passivo il rilevamento, per tutti: il giocatore ne fa la TMA
    // alla stazione, l'IA con la sua (docs/18 fase 2, nucleo/tma_ia) sulle coppie ostili con traccia solida. La quota
    // dal passivo non si sa: una nave classificata e' in superficie, altrimenti la si crede alla propria
    if (automatica) {
        c.pos_stimata = b.pos;
        c.quota_stimata = b.quota;
        c.posizione_nota = true;
        c.t_posizione = tempo_;
    } else if (!o.giocatore && usato && da == usato->id && c.qualita >= 800.0 && ostili(o, b)) {
        auto& t = tma_ia_[{ido, idb}];
        if (tempo_ - t.ultimo_ril >= TMA_IA_PASSO_S) {
            t.tma.aggiungi({tempo_, o.pos, c.rilevamento});
            t.ultimo_ril = tempo_;
            t.errore_deg = errore;
            t.aggiungi_errore(errore);
        }
        if (tempo_ - t.ultima_soluzione >= TMA_IA_SOLUZIONE_S) {
            t.ultima_soluzione = tempo_;
            const auto st = risolvi_tma(t.tma, tempo_, t.sigma_deg(TMA_IA_ERRORE_MIN_DEG));
            t.stima = st;   // per le gambe della fase 4 B (ManovreTma)
            if (st && st->osservabile) {
                t.ultima_osservabile = tempo_;
                c.pos_stimata = st->posizione;
                c.quota_stimata = b.categoria == "nave" && c.durata >= DURATA_CLASSIFICATO ? 0.0 : o.quota;
                c.posizione_nota = true;
                c.t_posizione = tempo_;
                t.non_osservabili = 0;
                t.data = st->posizione;
            } else if (++t.non_osservabili >= TMA_IA_NON_OSSERVABILI && t.data && c.posizione_nota && c.pos_stimata.x == t.data->x &&
                       c.pos_stimata.y == t.data->y) {
                // la posizione della TMA, che nessun altro sensore ha rinnovato, non regge piu': di nuovo il solo
                // rilevamento (disassemblatore, 29/10/2026: le prime soluzioni «osservabili» a 1-2 km restavano ferme)
                c.posizione_nota = false;
                t.data.reset();
            }
        }
    }
    // la TMA dell'equipaggio del giocatore (fire control party) sui contatti con traccia solida, per il piano dei missili
    // (i contatti noti nell'area di ricerca): la stessa dell'IA, ma non da' posizioni al giocatore, che ha il suo MATE
    if (o.giocatore && catalogo_missili_ && !automatica && usato && da == usato->id && c.qualita >= 800.0) {
        auto& t = tma_equipaggio_[idb];
        if (tempo_ - t.ultimo_ril >= TMA_IA_PASSO_S) {
            t.tma.aggiungi({tempo_, o.pos, c.rilevamento});
            t.ultimo_ril = tempo_;
            t.errore_deg = errore;
            t.aggiungi_errore(errore);
        }
        if (tempo_ - t.ultima_soluzione >= TMA_IA_SOLUZIONE_S) {
            t.ultima_soluzione = tempo_;
            t.stima = risolvi_tma(t.tma, tempo_, t.sigma_deg(TMA_IA_ERRORE_MIN_DEG));
        }
    }
    if (c.posizione_nota && conto_scia_ == 30 && del_giocatore) registra_scia(idb, c.pos_stimata);
    scelta_bersaglio(o, b, c);
    return true;
}

// Ping dell'Enhanced: un impulso alla volta. Eco su chi e' nel fascio (posizione dalla distanza e dal rilevamento),
// poi l'intercettazione di chi ascolta (contro-scoperta, dossier 02 §5.3): un contatto sul pinger, con la posizione
// per l'IA e solo il rilevamento per il giocatore, avvisato.
void Missione::impulso_sonar_enhanced(int id) {
    auto* p = trova(id);
    // Enhanced: un sonar di scafo dell'IA non ripinga prima che torni l'eco della sua scala (giro della libreria del
    // 30/10/2026: nella S12 le Perry, col PING dello script a ogni passaggio, davano al giocatore 2,4 ping al secondo
    // per un'ora); il classico e il giocatore come prima
    if (p && catalogo_ && !p->giocatore)
        for (auto it = ping_unita_.rbegin(); it != ping_unita_.rend() && tempo_ - it->t < INTERVALLO_PING_S; ++it)
            if (it->unita == id) return;
    if (p) ping_unita_.push_back({tempo_, id, p->pos});   // anche senza l'acustica: le condizioni degli script
    if (!p || !catalogo_) return;
    const auto attivi = sensori_acustici(*p, TipoSensori::attivi);
    const Sensore* scelto = attivi.empty() ? nullptr : attivi.front();
    Sensore speciale;   // scheda I09: il sonar dello scenario al posto di quello della classe
    if (const auto sp = sonar_speciale_.find(id); sp != sonar_speciale_.end()) {
        if (scelto) speciale = *scelto;
        speciale.id = "sonar_speciale";
        speciale.modo = ModoSensore::attivo;
        if (sp->second.dimensione_m > 0.0 || !scelto) speciale.dimensione_m = sp->second.dimensione_m > 0.0 ? sp->second.dimensione_m : 8.0;
        speciale.sl_attivo_db = sp->second.sl_db;
        speciale.impulso_s = sp->second.impulso_s;
        speciale.risoluzione_s = sp->second.risoluzione_s;
        if (const std::string& a = sp->second.antenna; !a.empty())
            speciale.antenna = a == "lineare" ? Antenna::lineare : a == "scafo" ? Antenna::scafo : Antenna::sfera;
        double banda = BANDE_HZ.front();
        for (const double b : BANDE_HZ)
            if (std::fabs(std::log(b / sp->second.frequenza_hz)) < std::fabs(std::log(banda / sp->second.frequenza_hz))) banda = b;
        speciale.bande_hz = {banda};
        scelto = &speciale;
    }
    if (!scelto) return;
    if (statistiche_) emissioni_.push_back({tempo_, id, "ping", scelto->impulso_s, scelto->id});   // R5 della sandbox
    const Sensore& sa = *scelto;
    // l'impulso per gli intercettatori (E); il sonar speciale dello scenario con la sua frequenza
    emetti_impulso(id, sa.id, sa.sl_attivo_db, sa.id == "sonar_speciale" && !sa.bande_hz.empty() ? sa.bande_hz.front() : 0.0);
    auto sp = sorgente_acustica(*p);   // il pinger con il suo impulso, per l'intercettazione
    if (sp) { sp->ping_db = sa.sl_attivo_db; sp->ping_hz = sa.bande_hz.empty() ? 3500.0 : sa.bande_hz.front(); sp->ping_s = sa.impulso_s; }
    // sotto la banchisa gli echi delle chiglie anche al ping dell'IA (simmetria, punto 6): nessun contatto fantasma, ma
    // una chiglia piu' forte nello stesso fascio e nella stessa cella di distanza prende il posto dell'eco del bersaglio
    const std::vector<EcoFalso> chiglie = p->giocatore ? std::vector<EcoFalso>{} : echi_chiglie_di(*p, sa);
    for (auto& [idb, b] : unita_) {
        if (idb == id || b.distrutta || b.persa) continue;
        const Ambiente amb = ambiente_acustico(p->pos, b.pos);
        const double r = std::max(distanza(p->pos, b.pos), 1.0);
        // eco
        if (const auto s = sorgente_acustica(b)) {
            Rilevazione x = ::m688::valuta(*s, ricevitore_acustico(*p, &sa), amb, raggi_, tempo_);
            if (!interventi_.empty()) x.se_db += attenuazione_guasto(interventi_, stati_interventi_, id, sa);   // vetrino di Dio
            if (x.se_db >= 0.0) {
                possibile(id, idb, sa.id, x.se_db);
                // l'eco d'esca del falso contatto (D3): troppo piccola per la classe, il sonarista di chi pinga lo smaschera
                if (const auto f = esche_mobili_.find(idb); f != esche_mobili_.end() && !f->second.dati) f->second.eco_da[id] = tempo_;
                Contatto* c = p->contatti.count(idb) ? &p->contatti.at(idb) : nuovo_contatto(*p, idb);
                if (c) {
                    double rlv = rilevamento(p->pos, b.pos) + gauss();
                    double rr = r * (1.0 + 0.01 * gauss());
                    double quota_eco = b.quota;
                    const EcoFalso* confusa = nullptr;
                    for (const EcoFalso& f : chiglie)
                        if (f.eccesso_db > x.se_db && std::fabs(normalizza_180(f.rilevamento - rlv)) <= FASCIO_ECO_DEG &&
                            std::fabs(f.distanza_m - rr) <= CELLA_DISTANZA_ECO_M && (!confusa || f.eccesso_db > confusa->eccesso_db))
                            confusa = &f;
                    if (confusa) rlv = confusa->rilevamento, rr = confusa->distanza_m, quota_eco = confusa->pescaggio_m;
                    const double a = rlv * 3.14159265358979323846 / 180.0;
                    c->pos_stimata = {p->pos.x + rr * std::sin(a), p->pos.y + rr * std::cos(a)};
                    c->quota_stimata = quota_eco;
                    c->posizione_nota = true;
                    c->t_posizione = tempo_;
                    c->rilevamento = normalizza_360(rlv);
                    c->qualita = std::max(c->qualita, 800.0);
                    c->segnale = 500.0 + 25.0 * x.se_db;
                    c->eccesso_db = x.se_db;
                    c->sensore = sa.id;
                    c->lampeggio = 0;
                    c->ultima_scoperta = tempo_;
                }
            }
        }
        // intercettazione: l'impulso di sola andata ai sensori passivi di chi ascolta (WLR-9 e simili compresi)
        if (sp) intercetta_impulso(*p, b, *sp, amb);
    }
    if (const auto* g = giocatore(); g && g->id == id) echi_chiglie_ping(*p, sa);   // i falsi echi delle chiglie (missione_ghiaccio.cpp)
}

void Missione::intercetta_impulso(UnitaClassica& p, UnitaClassica& b, const Sorgente& sp, const Ambiente& amb) {
    // l'impulso di sola andata ai sensori passivi di b (WLR-9 e simili compresi): b sente chi pinga, col rilevamento e
    // senza la posizione, giocatore o IA (docs/18 fase 2)
    const auto* g = giocatore();
    double se = -1e9;
    std::string da;
    for (const Sensore* sn : sensori_acustici(b, TipoSensori::passivi))
        if (const double x = ::m688::valuta(sp, ricevitore_acustico(b, sn), amb, raggi_, tempo_).se_db; x > se) { se = x; da = sn->id; }
    if (se < 0.0) return;
    possibile(b.id, p.id, da, se);
    Contatto* c = b.contatti.count(p.id) ? &b.contatti.at(p.id) : nuovo_contatto(b, p.id);
    if (!c) return;
    c->qualita = std::max(c->qualita, 400.0);
    c->segnale = 500.0 + 25.0 * std::min(se, 40.0);
    c->eccesso_db = se;
    c->sensore = da;
    c->lampeggio = 0;
    c->rilevamento = normalizza_360(rilevamento(b.pos, p.pos) + gauss());
    c->ultima_scoperta = tempo_;
    if (g && g->id == b.id) messaggio_sonar(MSG_CI_PINGANO);
    if (forze_ && !b.giocatore && ostili(p, b)) evento_allerta(b.id, "ping");   // fase 3: ping nemico sentito
    if (forze_ && armi_ia_ && !armi_ia_->originale && !reazioni_spente_ && !b.giocatore && b.categoria == "sottomarino" && ostili(p, b) &&
        situazione_fra(p.flag & 0x18, b.flag & 0x18) == Situazione::pace && distanza(p.pos, b.pos) <= 3704.0 &&
        allontanati_ping_.insert(b.id * 100000 + p.id).second && caso_reazioni_.uniforme() < 0.7) {   // fase 4 D: «ti tengo», via
        const double via = radianti(rilevamento(p.pos, b.pos));
        manovra_difesa(b.id, {b.pos.x + 10000.0 * std::sin(via), b.pos.y + 10000.0 * std::cos(via)}, std::min(b.avanti_tutta, 12.0), 600.0);
        ritirate_ia_.push_back({tempo_, b.id, p.id, "ping"});
    }
    if (forze_ && !difesa_spenta_ && !b.giocatore && b.categoria == "sottomarino" && b.quota > 3.0 && ostili(p, b))   // fase 4 A
        stimola_evasione(b.id, distanza(p.pos, b.pos) <= 5000.0 ? StimoloEvasione::ping_vicino : StimoloEvasione::ping_lontano, p.pos, 0.0, p.id);
}

Missione::SceneAcustica Missione::scena_acustica(int id, const std::string& sensore) const {
    SceneAcustica sc;
    const auto it = unita_.find(id);
    if (!catalogo_ || it == unita_.end() || it->second.distrutta || it->second.persa) return sc;
    const UnitaClassica& u = it->second;
    const Sensore* scelto = nullptr;
    for (const auto tipo : {TipoSensori::passivi, TipoSensori::attivi})
        for (const Sensore* s : sensori_acustici(u, tipo)) if (s->id == sensore) scelto = s;
    sc.ricevitore = ricevitore_acustico(u, scelto);
    sc.ambiente = ambiente_acustico(u.pos, u.pos);
    for (const auto& [idb, b] : unita_) {
        if (idb == id || b.distrutta || b.persa) continue;
        if (auto s = sorgente_acustica(b)) sc.sorgenti.push_back(*s);
        else if (asw_ && aereo(b))
            if (auto a = sorgente_aereo_per(b, u)) sc.sorgenti.push_back(*a);   // ASW aereo: Snell e hover
    }
    transitori_asw(sc.sorgenti, 2.0);   // tonfi, ping, esplosive, SUS
    for (const auto& t : fulmini_)   // i fulmini sul mare vicini (meteo, M2)
        if (auto f = sorgente_transitoria(*catalogo_, t, tempo_, 2.0)) sc.sorgenti.push_back(*f);
    for (const auto& t : transitori_armi_) {   // lanci, scoppi, affondamenti, implosioni e ping delle teste (R7)
        if (t.id >= 950000 && t.id <= 959999 && t.origine == id) continue;   // lo scafo proprio non si ascolta da se'
        if (auto f = sorgente_transitoria(*catalogo_, t, tempo_, 2.0)) sc.sorgenti.push_back(*f);
    }
    for (const auto& i : paesaggio_) sc.sorgenti.push_back(sorgente_da_istanza(*catalogo_, i, tempo_, 2.0));   // S5c
    for (const auto& i : reti_) sc.sorgenti.push_back(sorgente_da_istanza(*catalogo_, i, tempo_, 2.0));
    // la perdita del sensore, come in ascolta_con_acustica: le unita' col loro campo, il traffico oltre i 30 km col livello A
    sc.campi.reserve(sc.sorgenti.size());
    for (const Sorgente& s : sc.sorgenti) {
        const auto b = unita_.find(s.id);
        const bool col_campo = b != unita_.end() && (!id_traffico_.count(s.id) || distanza(u.pos, b->second.pos) <= TRAFFICO_CAMPO_M);
        sc.campi.push_back(col_campo ? raggi_.in_vigore(s.id) : nullptr);
    }
    return sc;
}

// --- sala radio dell'Enhanced (pezzo S4; progetto nelle note della sessione, progetto-radio.md) ---

namespace {

// avvisi al giocatore (testi in tools/pipeline/esporta_motore.py)
constexpr int MSG_RADIO_IN_ARRIVO = 315, MSG_RADIO_FLASH = 316, MSG_RADIO_IMMEDIATE = 317, MSG_RADIO_RICEVUTO = 318,
              MSG_RADIO_PERSO = 319, MSG_RADIO_ELF = 320, MSG_FILO_ROTTO = 321, MSG_FILO_SENZA_SEGNALE = 322,
              MSG_SATELLITE_ASSENTE = 323, MSG_TRASMISSIONE = 324, MSG_SSIXS = 325;
// fase R1 (dossier 10 §6): ordini in vigore cambiati, contatto d'intelligence, supporto in arrivo, boa SLOT,
// lancio contro le regole d'ingaggio
constexpr int MSG_ORDINI_CAMBIATI = 326, MSG_INTELLIGENCE = 327, MSG_SUPPORTO = 331, MSG_SLOT = 332, MSG_CONTRO_ROE = 333;
// fase R2: in una zona protetta, compito scaduto, sessione mancata, autenticazione errata
constexpr int MSG_ZONA_PROTETTA = 328, MSG_COMPITO_SCADUTO = 329, MSG_SESSIONE_MANCATA = 330, MSG_AUTENTICAZIONE = 339;
constexpr double ORIZZONTE_ESM_M = 50000.0;   // un ESM nemico entro l'orizzonte radio intercetta l'emissione (STIMA)
constexpr double METRI_PER_GRADO_LAT = 111120.0;

// il campo dopo "FM:" (o "DA:") nelle righe del briefing; vuoto se manca
std::string campo_briefing(const std::vector<std::string>& righe, std::initializer_list<const char*> chiavi) {
    for (const auto& r : righe)
        for (const char* k : chiavi) {
            const std::string p = k;
            if (r.rfind(p, 0) == 0) { std::string v = r.substr(p.size()); v.erase(0, v.find_first_not_of(' ')); return v; }
        }
    return {};
}

// il testo del briefing senza le righe d'intestazione
std::string corpo_briefing(const std::vector<std::string>& righe) {
    std::string s;
    for (std::size_t i = 0; i < righe.size(); ++i) {
        const auto& r = righe[i];
        if (i == 0 || r.rfind("FM:", 0) == 0 || r.rfind("TO:", 0) == 0 || r.rfind("SUBJ:", 0) == 0 || r.rfind("DA:", 0) == 0 ||
            r.rfind("A:", 0) == 0 || r.rfind("OGG.:", 0) == 0)
            continue;
        if (!s.empty()) s += "\n";
        s += r;
    }
    return s;
}

}  // namespace

// cambio delle regole d'ingaggio da una tabella del file radio: { navi = "stretto", neutrali = "trattenuto" }
static CambioRoe leggi_roe(lua_State* L, int t) {
    CambioRoe c{};
    for (lua_pushnil(L); lua_next(L, t); lua_pop(L, 1))
        if (lua_type(L, -2) == LUA_TSTRING && lua_type(L, -1) == LUA_TSTRING)
            if (const auto k = categoria_da_nome(lua_tostring(L, -2)))
                if (const auto v = stato_roe_da_nome(lua_tostring(L, -1))) c[static_cast<std::size_t>(*k)] = *v;
    return c;
}

// interi di una tabella (o un intero solo)
// una zona dello spazio d'acqua da una tabella del file radio (docs/09 §5.1)
static Zona leggi_zona(lua_State* L, int t) {
    Zona z;
    z.id = campo_stringa(L, t, "id");
    z.tipo = tipo_zona_da_nome(campo_stringa(L, t, "tipo", "protetta")).value_or(TipoZona::protetta);
    lua_getfield(L, t, "cerchio");
    if (lua_istable(L, -1)) {
        const int k = lua_gettop(L);
        z.centro = Vec2{campo_numero(L, k, "x", 0.0), campo_numero(L, k, "y", 0.0)};
        z.raggio_m = campo_numero(L, k, "r", 0.0);
    }
    lua_pop(L, 1);
    lua_getfield(L, t, "vertici");
    if (lua_istable(L, -1)) {
        const int v = lua_gettop(L);
        for (lua_Integer i = 1; lua_rawgeti(L, v, i) == LUA_TTABLE; ++i) {
            const int p = lua_gettop(L);
            lua_rawgeti(L, p, 1);
            lua_rawgeti(L, p, 2);
            z.vertici.push_back({lua_tonumber(L, -2), lua_tonumber(L, -1)});
            lua_pop(L, 3);
        }
        lua_pop(L, 1);
    }
    lua_pop(L, 1);
    z.unita = static_cast<int>(campo_numero(L, t, "unita", -1.0));
    if (z.unita >= 0 && z.tipo == TipoZona::protetta) z.tipo = TipoZona::protetta_mobile;
    z.avanti_m = campo_numero(L, t, "avanti_m", 0.0);
    z.dietro_m = campo_numero(L, t, "dietro_m", 0.0);
    z.lato_m = campo_numero(L, t, "lato_m", 0.0);
    z.dal_s = campo_numero(L, t, "dal_s", 0.0);
    z.al_s = campo_numero(L, t, "al_s", 1e18);
    return z;
}

static std::vector<Zona> leggi_zone(lua_State* L, int t) {
    std::vector<Zona> v;
    for (lua_Integer i = 1; lua_rawgeti(L, t, i) == LUA_TTABLE; ++i) { v.push_back(leggi_zona(L, lua_gettop(L))); lua_pop(L, 1); }
    lua_pop(L, 1);
    return v;
}

static std::vector<int> leggi_interi(lua_State* L, int idx) {
    std::vector<int> v;
    if (lua_isnumber(L, idx)) v.push_back(static_cast<int>(lua_tointeger(L, idx)));
    else if (lua_istable(L, idx))
        for (lua_Integer i = 1; lua_rawgeti(L, idx, i) == LUA_TNUMBER; ++i) { v.push_back(static_cast<int>(lua_tointeger(L, -1))); lua_pop(L, 1); }
    if (lua_istable(L, idx)) lua_pop(L, 1);
    return v;
}

void Missione::radio_enhanced() {
    if (radio_) return;
    const auto* g = giocatore();
    const LatoRadio lato = lato_missione_ == "urss" || (g && (g->flag & 0x10)) ? LatoRadio::urss : LatoRadio::usa;
    ParametriRadio par = parametri_di_missione(900.0);   // orologio di missione (dossier 10 §0): ciclo di 15 minuti
    CambioRoe roe_di_partenza{};
    std::vector<Zona> zone_di_partenza;
    std::vector<std::pair<MessaggioRadio, double>> programmati;
    // file della missione, facoltativo (docs/09 §5.1): parametri, stato iniziale, rapporti, boe, gestori e messaggi
    // E6: la chiave `radio` della missione (rapide, editor, libreria), con la stessa forma, prevale sul file
    const auto file = std::filesystem::path(file_missione_).parent_path() / "radio" / (id_ + ".lua");
    const int base = lua_gettop(L_);
    bool con_radio = false;
    lua_rawgeti(L_, LUA_REGISTRYINDEX, rif_missione_);
    lua_getfield(L_, -1, "radio");
    if (lua_istable(L_, -1)) {
        lua_remove(L_, -2);
        con_radio = true;
    } else {
        lua_pop(L_, 2);
        con_radio = std::filesystem::exists(file) && luaL_loadfilex(L_, file.string().c_str(), "t") == LUA_OK &&
                    chiama_lua(0, 1) == LUA_OK && lua_istable(L_, -1);
    }
    // R3: intestazione e data degli ordini di missione (serie 0)
    std::string ordini_da, ordini_a, ordini_oggetto, ordini_classifica = "SECRET";
    double ora_dtg_s = ora_utc_s_;
    int giorno_dtg = giorno_, mese_dtg = mese_data_, anno_dtg = anno_;
    if (con_radio) {
        const int t = lua_gettop(L_);
        lua_getfield(L_, t, "ordini");
        if (lua_istable(L_, -1)) {
            const int o = lua_gettop(L_);
            ordini_da = campo_stringa(L_, o, "da");
            ordini_a = campo_stringa(L_, o, "a");
            ordini_oggetto = campo_stringa(L_, o, "oggetto");
            ordini_classifica = campo_stringa(L_, o, "classifica", ordini_classifica);
            lua_getfield(L_, o, "data");
            if (lua_istable(L_, -1) && !data_imposta_) {
                const int d = lua_gettop(L_);
                giorno_dtg = static_cast<int>(campo_numero(L_, d, "giorno", giorno_dtg));
                mese_dtg = static_cast<int>(campo_numero(L_, d, "mese", mese_dtg));
                anno_dtg = static_cast<int>(campo_numero(L_, d, "anno", anno_dtg));
                ora_dtg_s = campo_numero(L_, d, "ora_utc", ora_dtg_s / 3600.0) * 3600.0;
            }
            lua_pop(L_, 1);
        }
        lua_pop(L_, 1);
        lua_getfield(L_, t, "parametri");
        if (lua_istable(L_, -1)) {
            const int p = lua_gettop(L_);
            par = parametri_di_missione(campo_numero(L_, p, "ciclo_s", par.ciclo_s));
            par.fondo_ogni_s = campo_numero(L_, p, "fondo_ogni_s", par.fondo_ogni_s);
            par.finestra_ogni_s = campo_numero(L_, p, "finestra_ogni_s", par.finestra_ogni_s);
            par.finestra_primo_s = campo_numero(L_, p, "finestra_primo_s", par.finestra_primo_s);
            par.tolleranza_finestra_s = campo_numero(L_, p, "tolleranza_s", par.tolleranza_finestra_s);
        }
        lua_pop(L_, 1);
        // R2: finestre di comunicazione (il guinzaglio sovietico, dossier 10 §4.6)
        lua_getfield(L_, t, "finestre");
        if (lua_istable(L_, -1)) {
            const int f = lua_gettop(L_);
            par.finestra_ogni_s = campo_numero(L_, f, "ogni_s", par.finestra_ogni_s);
            par.finestra_primo_s = campo_numero(L_, f, "primo_s", par.finestra_primo_s);
            par.tolleranza_finestra_s = campo_numero(L_, f, "tolleranza_s", par.tolleranza_finestra_s);
            par.saltate_max = static_cast<int>(campo_numero(L_, f, "saltate_max", par.saltate_max));
            lua_getfield(L_, f, "obbligatorie");
            par.sessioni_obbligatorie = lua_toboolean(L_, -1) != 0;
            lua_pop(L_, 1);
            lua_getfield(L_, f, "ricevuta");
            par.ricevuta_richiesta = lua_toboolean(L_, -1) != 0;
            lua_pop(L_, 1);
            gestore_saltate_ = campo_stringa(L_, f, "gestore_saltate");
        }
        lua_pop(L_, 1);
        lua_getfield(L_, t, "stato_iniziale");
        if (lua_istable(L_, -1)) {
            const int si = lua_gettop(L_);
            lua_getfield(L_, si, "roe");
            if (lua_istable(L_, -1)) roe_di_partenza = leggi_roe(L_, lua_gettop(L_));
            lua_pop(L_, 1);
            lua_getfield(L_, si, "neutrali");
            for (const int id : leggi_interi(L_, lua_gettop(L_))) neutrali_.insert(id);
            lua_pop(L_, 1);
            lua_getfield(L_, si, "zone");
            if (lua_istable(L_, -1)) zone_di_partenza = leggi_zone(L_, lua_gettop(L_));
            lua_pop(L_, 1);
        }
        lua_pop(L_, 1);
        lua_getfield(L_, t, "rapporti");
        if (lua_istable(L_, -1)) {
            lua_getfield(L_, -1, "contatto");
            if (lua_istable(L_, -1)) {
                const int r = lua_gettop(L_);
                rapporto_contatto_.gestore = campo_stringa(L_, r, "gestore");
                rapporto_contatto_.ritardo_s = campo_numero(L_, r, "ritardo_s", 0.0);
                lua_getfield(L_, r, "una_volta");
                rapporto_contatto_.una_volta = lua_isnil(L_, -1) || lua_toboolean(L_, -1);
                lua_pop(L_, 1);
            }
            lua_pop(L_, 1);
        }
        lua_pop(L_, 1);
        lua_getfield(L_, t, "slot");
        if (lua_istable(L_, -1)) {
            const int b = lua_gettop(L_);
            slot_.boe = static_cast<int>(campo_numero(L_, b, "boe", 0.0));
            slot_.ritardo_min_s = campo_numero(L_, b, "ritardo_min_s", slot_.ritardo_min_s);
            slot_.ritardo_max_s = campo_numero(L_, b, "ritardo_max_s", slot_.ritardo_max_s);
            slot_.portata_vhf_m = campo_numero(L_, b, "portata_vhf_m", slot_.portata_vhf_m);
        }
        lua_pop(L_, 1);
        // gestori del file radio: fra gli script della missione, cosi' messaggi e rapporti li eseguono come gli altri
        lua_getfield(L_, t, "gestori");
        if (lua_istable(L_, -1)) {
            const int gt = lua_gettop(L_);
            lua_rawgeti(L_, LUA_REGISTRYINDEX, rif_script_);
            const int sc = lua_gettop(L_);
            for (lua_pushnil(L_); lua_next(L_, gt); lua_pop(L_, 1))
                if (lua_type(L_, -2) == LUA_TSTRING && lua_isfunction(L_, -1)) {
                    lua_pushvalue(L_, -2);
                    lua_pushvalue(L_, -2);
                    lua_settable(L_, sc);
                }
            lua_pop(L_, 1);
        }
        lua_pop(L_, 1);
        // messaggi: la parte lista della tabella
        for (lua_Integer i = 1; lua_rawgeti(L_, t, i) == LUA_TTABLE; ++i) {
            const int e = lua_gettop(L_);
            MessaggioRadio m;
            const std::string p = campo_stringa(L_, e, "precedenza", "routine");
            m.precedenza = p == "flash" ? Precedenza::flash : p == "immediate" ? Precedenza::immediate
                         : p == "priority" ? Precedenza::priority : Precedenza::routine;
            m.id = campo_stringa(L_, e, "id");
            m.da = campo_stringa(L_, e, "da");
            m.a = campo_stringa(L_, e, "a");
            m.oggetto = campo_stringa(L_, e, "oggetto");
            m.classifica = campo_stringa(L_, e, "classifica", "CONFIDENTIAL");
            m.ordine = campo_stringa(L_, e, "gestore");
            // l'ordine d'esecuzione di una missione TLAM della libreria di bordo (docs/21 §11.8)
            if (const std::string x = campo_stringa(L_, e, "esecuzione_tlam"); !x.empty() && !m.id.empty()) esecuzioni_tlam_[m.id] = x;
            m.autentico = !lua_getfield(L_, e, "autentico") || lua_toboolean(L_, -1);
            lua_pop(L_, 1);
            lua_getfield(L_, e, "testo");
            if (lua_istable(L_, -1)) m.testo = {campo_stringa(L_, lua_gettop(L_), "it"), campo_stringa(L_, lua_gettop(L_), "en")};
            lua_pop(L_, 1);
            lua_getfield(L_, e, "roe");
            if (lua_istable(L_, -1)) m.roe = leggi_roe(L_, lua_gettop(L_));
            lua_pop(L_, 1);
            // R2: zone aggiunte, spostate o tolte; compito con scadenza
            lua_getfield(L_, e, "zone");
            if (lua_istable(L_, -1)) {
                const int zt = lua_gettop(L_);
                CambioZone cz;
                lua_getfield(L_, zt, "aggiungi");
                if (lua_istable(L_, -1)) cz.aggiungi = leggi_zone(L_, lua_gettop(L_));
                lua_pop(L_, 1);
                lua_getfield(L_, zt, "togli");
                if (lua_istable(L_, -1)) {
                    const int tt = lua_gettop(L_);
                    for (lua_Integer k = 1; lua_rawgeti(L_, tt, k) == LUA_TSTRING; ++k) { cz.togli.push_back(lua_tostring(L_, -1)); lua_pop(L_, 1); }
                    lua_pop(L_, 1);
                }
                lua_pop(L_, 1);
                m.zone = cz;
            }
            lua_pop(L_, 1);
            m.scade_s = campo_numero(L_, e, "scade_s", -1.0);
            m.gestore_scaduto = campo_stringa(L_, e, "gestore_scaduto");
            lua_getfield(L_, e, "contatto");
            if (lua_istable(L_, -1)) {
                const int k = lua_gettop(L_);
                RichiestaIntelligence r;
                lua_getfield(L_, k, "unita");
                r.unita = leggi_interi(L_, lua_gettop(L_));
                lua_pop(L_, 1);
                r.eta_s = campo_numero(L_, k, "eta_s", r.eta_s);
                r.errore_m = campo_numero(L_, k, "errore_m", r.errore_m);
                r.sensore = campo_stringa(L_, k, "sensore", r.sensore);
                r.rotta_err_deg = campo_numero(L_, k, "rotta_err", r.rotta_err_deg);
                r.velocita_err_kn = campo_numero(L_, k, "velocita_err_kn", r.velocita_err_kn);
                lua_getfield(L_, k, "falso");
                r.falso = lua_toboolean(L_, -1);
                lua_pop(L_, 1);
                m.richiesta = r;
            }
            lua_pop(L_, 1);
            // R3: ripetizioni, ricevuta richiesta, «disregard»; i messaggi su condizione e quelli solo da script
            m.ripetizioni = static_cast<int>(campo_numero(L_, e, "ripetizioni", 0.0));
            lua_getfield(L_, e, "ricevuta");
            m.ricevuta = lua_toboolean(L_, -1);
            lua_pop(L_, 1);
            m.annulla = campo_stringa(L_, e, "annulla");
            const double dopo = campo_numero(L_, e, "dopo_s", 0.0);
            if (!m.id.empty()) radio_catalogo_[m.id] = m;
            lua_getfield(L_, e, "su_richiesta");
            const bool su_richiesta = lua_toboolean(L_, -1);
            lua_pop(L_, 1);
            lua_getfield(L_, e, "se");
            if (lua_istable(L_, -1)) {
                const int c = lua_gettop(L_);
                CondizioneRadio x;
                x.ricevuto = campo_stringa(L_, c, "ricevuto");
                x.trasmesso = campo_stringa(L_, c, "trasmesso");
                x.evento = campo_stringa(L_, c, "evento");
                lua_getfield(L_, c, "unita_colpita");
                if (lua_istable(L_, -1)) x.colpite = leggi_interi(L_, lua_gettop(L_));
                else if (lua_isnumber(L_, -1)) x.colpite = {static_cast<int>(lua_tointeger(L_, -1))};
                lua_pop(L_, 1);
                lua_getfield(L_, c, "unita_fuori");
                if (lua_istable(L_, -1)) x.fuori = leggi_interi(L_, lua_gettop(L_));
                else if (lua_isnumber(L_, -1)) x.fuori = {static_cast<int>(lua_tointeger(L_, -1))};
                lua_pop(L_, 1);
                lua_getfield(L_, c, "variabile");   // { indice, "op", valore }
                if (lua_istable(L_, -1)) {
                    const int v = lua_gettop(L_);
                    lua_rawgeti(L_, v, 1); x.indice = static_cast<int>(lua_tointeger(L_, -1)); lua_pop(L_, 1);
                    lua_rawgeti(L_, v, 2); if (lua_type(L_, -1) == LUA_TSTRING) x.op = lua_tostring(L_, -1); lua_pop(L_, 1);
                    lua_rawgeti(L_, v, 3); x.valore = static_cast<int>(lua_tointeger(L_, -1)); lua_pop(L_, 1);
                    x.variabile = true;
                }
                lua_pop(L_, 1);
                radio_condizionati_.push_back({m, dopo, x});
            } else if (!su_richiesta) {
                programmati.push_back({m, dopo});
            }
            lua_pop(L_, 1);
            lua_pop(L_, 1);
        }
    }
    lua_settop(L_, base);
    radio_ = std::make_unique<SalaRadio>(lato, seme_, ora_dtg_s, giorno_dtg, mese_dtg, anno_dtg, par);
    // la sandbox: le ROE per categoria della parte del giocatore sono quelle di partenza della sua sala radio
    if (g)
        if (const auto r = regolazioni_.find(g->flag & 0x18); r != regolazioni_.end() && r->second.roe)
            for (const auto& [c, libere] : *r->second.roe) roe_di_partenza[static_cast<std::size_t>(c)] = libere ? StatoRoe::libero : StatoRoe::trattenuto;
    radio_->roe_iniziali(roe_di_partenza);
    radio_->zone_iniziali(zone_di_partenza);
    // ordini di missione dal briefing (serie 0), rileggibili dalla sala radio
    MessaggioRadio o;
    o.precedenza = Precedenza::immediate;
    o.classifica = ordini_classifica;
    o.da = !ordini_da.empty() ? ordini_da : campo_briefing(briefing_en_, {"FM:"});
    o.a = !ordini_a.empty() ? ordini_a : campo_briefing(briefing_en_, {"TO:"});
    o.oggetto = !ordini_oggetto.empty() ? ordini_oggetto : campo_briefing(briefing_en_, {"SUBJ:"});
    if (o.oggetto.empty()) o.oggetto = titolo_;
    o.testo = {corpo_briefing(briefing_it_), corpo_briefing(briefing_en_)};
    o.roe = roe_di_partenza;
    radio_->ordini_di_missione(o);
    for (auto& [m, dopo] : programmati) {
        if (m.a.empty()) m.a = o.a;
        radio_->programma(m, dopo);
    }
}

// le classi civili (ROE «mercantili»): dai dati (ClasseNave::civile di classi.toml, con le classi del traffico); senza i
// dati (il classico non li legge) per nome, le quattro di sempre
bool Missione::classe_civile(const std::string& c) const {
    if (const auto it = dimensioni_classi_.find(c); it != dimensioni_classi_.end()) return it->second.civile;
    return c == "mercantile" || c == "cargo" || c == "petroliera" || c == "peschereccio";
}

CategoriaRoe Missione::categoria_roe(const UnitaClassica& u) const {
    if (neutrali_.count(u.id)) return CategoriaRoe::neutrali;
    if ((u.flag & 3) == 3 || u.categoria == "aereo") return CategoriaRoe::aerei;
    if (u.categoria == "sottomarino") return CategoriaRoe::sottomarini;
    if (classe_civile(u.classe)) return CategoriaRoe::mercantili;
    return CategoriaRoe::navi;
}

// Il contatto d'intelligence con la verita' al momento della messa in onda: le unita' vive, all'ora dell'osservazione
// (eta' prima), con l'errore gaussiano della posizione, della rotta e della velocita'; un falso allarme fra 10 e 30 km
// dal giocatore
DatiIntelligence Missione::compila_intelligence(const RichiestaIntelligence& r) {
    DatiIntelligence d;
    d.unita = r.unita;
    d.t_oss = tempo_ - r.eta_s;
    d.errore_m = r.errore_m;
    d.rotta_err_deg = r.rotta_err_deg;
    d.velocita_err_kn = r.velocita_err_kn;
    d.sensore = r.sensore;
    d.falso = r.falso;
    d.m_s_per_nodo = m_s_per_nodo_;
    Vec2 somma{};
    double sx = 0.0, sy = 0.0, v = 0.0;
    int n = 0;
    for (const int id : r.unita)
        if (const auto* u = trova(id)) {
            const double a = radianti(u->rotta), m = metri_al_secondo(u->velocita) * r.eta_s;
            somma.x += u->pos.x - m * std::sin(a);
            somma.y += u->pos.y - m * std::cos(a);
            sx += std::sin(a);
            sy += std::cos(a);
            v += u->velocita;
            ++n;
        }
    if (r.falso || n == 0) {
        const auto* g = giocatore();
        const Vec2 c = g ? g->pos : Vec2{};
        const double a = 2.0 * 3.14159265358979323846 * caso_acustica_.uniforme(), dist = 10000.0 + 20000.0 * caso_acustica_.uniforme();
        d.pos = {c.x + dist * std::sin(a), c.y + dist * std::cos(a)};
        d.rotta = 360.0 * caso_acustica_.uniforme();
        d.velocita_kn = 5.0 + 10.0 * caso_acustica_.uniforme();
        return d;
    }
    d.pos = {somma.x / n + r.errore_m * gauss(), somma.y / n + r.errore_m * gauss()};
    d.rotta = normalizza_360(gradi(std::atan2(sx, sy)) + r.rotta_err_deg * gauss());
    d.velocita_kn = std::max(0.0, v / n + r.velocita_err_kn * gauss());
    return d;
}

// Un lancio del giocatore contro il bersaglio designato, giudicato con le regole d'ingaggio (dossier 10 §4.2): la
// categoria vera, e come il giocatore lo conosceva (classe nota dalla durata oltre 1400, sopra o sotto la superficie
// dalla classificazione oltre 800; un neutrale si vede come un mercantile)
void Missione::giudica_lancio_giocatore(const UnitaClassica& g) {
    if (!radio_) return;
    const auto* b = trova(g.bersaglio);
    if (!b) return;
    const CategoriaRoe vera = categoria_roe(*b);
    std::optional<CategoriaRoe> creduta;
    std::string classe;
    bool identificato = false;
    if (const auto it = g.contatti.find(b->id); it != g.contatti.end()) {
        const Contatto& c = it->second;
        if (c.durata >= 1400.0) {
            creduta = vera == CategoriaRoe::neutrali ? CategoriaRoe::mercantili : vera;
            classe = b->classe;
            identificato = ((b->flag ^ g.flag) & 0x10) != 0;   // classificato e dell'altro schieramento: ostile
        } else if (c.durata > 800.0) {
            creduta = b->quota > 1.0 ? CategoriaRoe::sottomarini : CategoriaRoe::navi;
        }
    }
    // la classe dichiarata a vista dal libretto (docs/25 §3.2) prevale: e' cio' che il comandante credeva, anche se
    // sbagliata. Colpire un neutrale identificato male resta un'infrazione (la categoria vera giudica)
    if (const auto* v = identificazione_visiva(g.id, b->id)) {
        classe = v->classe;
        const auto parte = parte_della_classe(v->classe);
        const bool civile = classe_civile(v->classe);
        bool sub = b->categoria == "sottomarino";
        for (const auto& [id, u] : unita_)
            if (u.classe == v->classe) { sub = u.categoria == "sottomarino"; break; }
        creduta = civile || (parte && *parte == 0) ? CategoriaRoe::mercantili : sub ? CategoriaRoe::sottomarini : CategoriaRoe::navi;
        identificato = parte && *parte != 0 && ((*parte ^ g.flag) & 0x10) != 0;
    }
    GiudizioRoe r = radio_->giudica_lancio(b->id, vera, identificato, tempo_, creduta, classe);
    // R2: un bersaglio immerso in una zona protetta (stesse regole: ricevute, trasmesse da un ciclo, nota)
    if (b->quota > 1.0) {
        const auto protetta = [&](const std::vector<Zona>& zs) -> std::string {
            for (const auto& z : zs)
                if ((z.tipo == TipoZona::protetta || z.tipo == TipoZona::protetta_mobile) && z.valida(tempo_) && dentro_zona(z, b->pos, posa_zona(z)))
                    return z.id;
            return {};
        };
        const std::string zr = protetta(radio_->zone_ricevute(true)), z1 = protetta(radio_->zone_trasmesse(tempo_ - radio_->parametri().ciclo_s)),
                          z0 = protetta(radio_->zone_trasmesse(tempo_));
        const auto e = !zr.empty() ? GiudizioRoe::Esito::infrazione : !z1.empty() ? GiudizioRoe::Esito::infrazione_non_copiato
                     : !z0.empty() ? GiudizioRoe::Esito::nota : GiudizioRoe::Esito::permesso;
        radio_->aggrava_ultimo_lancio(e, !zr.empty() ? zr : !z1.empty() ? z1 : z0);
        if (static_cast<int>(e) > static_cast<int>(r.esito)) { r.esito = e; r.zona = !zr.empty() ? zr : !z1.empty() ? z1 : z0; }
    }
    registro_.giudizio(*this, r);
    if (r.esito == GiudizioRoe::Esito::infrazione || r.esito == GiudizioRoe::Esito::infrazione_non_copiato) messaggio_radio(MSG_CONTRO_ROE);
}

std::optional<PosaUnita> Missione::posa_zona(const Zona& z) const {
    if (z.tipo != TipoZona::protetta_mobile) return std::nullopt;
    const auto it = unita_.find(z.unita);
    if (it == unita_.end() || it->second.distrutta || it->second.persa) return std::nullopt;
    return PosaUnita{it->second.pos, it->second.rotta};
}

std::vector<Missione::ZonaCarta> Missione::zone_ricevute() const {
    std::vector<ZonaCarta> v;
    if (!radio_) return v;
    for (const auto& z : radio_->zone_ricevute()) {
        ZonaCarta c;
        c.zona = z;
        if (const auto p = posa_zona(z)) {
            c.vertici = poligono_zona(z, p);
            c.centro = p->pos;
            c.orientamento = p->rotta;
        } else if (z.tipo == TipoZona::protetta_mobile) {
            continue;   // il battello non c'e' piu'
        } else if (z.centro) {
            c.centro = z.centro;
            c.raggio_m = z.raggio_m;
        } else {
            c.vertici = z.vertici;
        }
        v.push_back(std::move(c));
    }
    return v;
}

bool Missione::radio_verifica(int serie) {
    if (!radio_) return true;
    const bool ok = radio_->verifica(serie);
    if (!ok) messaggio_radio(MSG_AUTENTICAZIONE);
    conti_.verifiche_radio.push_back({tempo_, ok});   // docs/26: il non autentico riconosciuto
    return ok;
}

bool Missione::radio_slot(double ritardo_s) {
    const auto* g = giocatore();
    if (!radio_ || !g || slot_.boe <= 0 || ritardo_s < slot_.ritardo_min_s || ritardo_s > slot_.ritardo_max_s) return false;
    --slot_.boe;
    slot_attese_.push_back({tempo_ + ritardo_s, g->pos});
    return true;
}

std::optional<double> Missione::slot_in_attesa() const {
    std::optional<double> v;
    for (const auto& [t, p] : slot_attese_) if (!v || t < *v) v = t;
    return v;
}

StatoAntenne Missione::antenne_giocatore() const {
    StatoAntenne s;
    if (const auto* g = giocatore()) {
        s.quota_m = g->quota;
        s.velocita_kn = g->velocita;
        s.rateo_virata_deg_s = rateo_virata_vero(*g);
        s.ghiaccio = sotto_banchisa(g->id);   // sotto la banchisa niente alberi ne' filo: solo l'ELF (missione_ghiaccio.cpp)
        s.albero = (g->flag54 & 6) != 0 && !s.ghiaccio;   // antenna o periscopio: solo a quota periscopica
        s.latitudine = latitudine_ + g->pos.y / METRI_PER_GRADO_LAT;
    }
    return s;
}

void Missione::messaggio_radio(int codice) {
    const auto* g = giocatore();
    if (!g) return;
    for (const auto& x : messaggi_radio_) if (x.codice == codice && x.tempo == tempo_) return;
    messaggi_radio_.push_back({tempo_, g->id, codice});
}

void Missione::radio_filo(bool cala) {
    if (!radio_) return;
    if (cala) radio_->cala_filo(tempo_);
    else radio_->recupera_filo();
}

// Trasmissione del giocatore: la radiogoniometria nemica la localizza con la probabilita' del modulo. Con un ESM
// nemico entro l'orizzonte, quell'unita' ha subito il rilevamento; altrimenti, dopo il ritardo, tutte le unita'
// avversarie hanno la posizione del fix (con il suo errore)
// la nave o l'aereo nemico piu' vicino al punto, entro l'orizzonte ESM (-1: nessuno)
int Missione::esm_nemico_vicino(Vec2 da) const {
    const auto* g = giocatore();
    int esm = -1;
    double vicino = ORIZZONTE_ESM_M;
    for (const auto& [id, u] : unita_) {
        if (!g || u.distrutta || u.persa || (u.flag & 0xA0) || !(u.flag & 6) || ((u.flag ^ g->flag) & 0x10) == 0) continue;   // navi e aerei nemici
        if (const double d = distanza(u.pos, da); d < vicino) { vicino = d; esm = id; }
    }
    return esm;
}

// l'esito della radiogoniometria nemica su una trasmissione partita da `da`
void Missione::esito_radiogoniometria(const EsitoTrasmissione& e, int esm, Vec2 da) {
    if (!e.localizzato) return;
    if (e.errore_m <= 0.0) contatto_da_radiogoniometria(std::nullopt, esm, da);
    else fix_radio_.push_back({tempo_ + e.ritardo_s, {da.x + e.errore_m * gauss(), da.y + e.errore_m * gauss()}});
}

EsitoTrasmissione Missione::radio_trasmetti(TipoTrasmissione tipo) {
    const auto* g = giocatore();
    if (!radio_ || !g) return {};
    ContestoDF df;
    const int esm = esm_nemico_vicino(g->pos);
    df.esm_vicino = esm >= 0;
    const EsitoTrasmissione e = radio_->trasmetti(tipo, tempo_, antenne_giocatore(), df);
    esito_radiogoniometria(e, esm, g->pos);
    registro_.trasmissione(*this, tipo, e, g->pos, false);
    // la previsione chiesta al comando (M4): la risposta va in onda col prossimo ciclo del broadcast
    if (e.fatta && tipo == TipoTrasmissione::previsione && meteo_) previsione_chiesta_ = tempo_;
    // il premio del rapporto di contatto (file radio, rapporti.contatto), dopo il ritardo
    if (e.fatta && tipo == TipoTrasmissione::contatto && !rapporto_contatto_.gestore.empty() &&
        !(rapporto_contatto_.una_volta && rapporto_contatto_.usato)) {
        rapporto_contatto_.usato = true;
        supporti_.push_back({tempo_ + rapporto_contatto_.ritardo_s, rapporto_contatto_.gestore});
    }
    return e;
}

// Contatto delle unita' avversarie sul giocatore da una radiogoniometria: col fix anche la posizione (quella del fix,
// non la vera); con solo_a >= 0 solo quell'unita' (l'ESM vicino), col solo rilevamento. La scelta del bersaglio scatta.
void Missione::contatto_da_radiogoniometria(std::optional<Vec2> pos, int solo_a, std::optional<Vec2> sorgente) {
    auto* g = trova(giocatore() ? giocatore()->id : -1);
    if (!g) return;
    const Vec2 da = sorgente.value_or(g->pos);   // la boa SLOT trasmette da dove e' stata lanciata
    for (auto& [id, u] : unita_) {
        if (u.distrutta || u.persa || (u.flag & 0xA0) || id == g->id || ((u.flag ^ g->flag) & 0x10) == 0) continue;
        if (solo_a >= 0 && id != solo_a) continue;
        Contatto* c = u.contatti.count(g->id) ? &u.contatti.at(g->id) : nuovo_contatto(u, g->id);
        if (!c) continue;
        c->qualita = std::max(c->qualita, 400.0);
        c->lampeggio = 0;
        c->ultima_scoperta = tempo_;
        c->rilevamento = rilevamento(u.pos, da);
        c->sensore = "radiogoniometria";
        if (auto em = emcon_.find(id); em != emcon_.end()) em->second.evento(EventoEmcon::scoperti, tempo_);
        if (pos) {
            c->pos_stimata = *pos;
            c->quota_stimata = 0.0;
            c->posizione_nota = true;
            c->t_posizione = tempo_;
        }
        scelta_bersaglio(u, *g, *c);
    }
}

// Un passo della sala radio: ricezione con le antenne del giocatore, ordini dei messaggi ricevuti, avvisi, fix maturi
bool Missione::condizione_radio(const CondizioneRadio& c) const {
    if (!c.ricevuto.empty() && std::none_of(radio_->ricevuti().begin(), radio_->ricevuti().end(),
                                            [&](const Ricevuto& r) { return r.messaggio.id == c.ricevuto; }))
        return false;
    const auto fuori = [&](int id) { const auto it = unita_.find(id); return it == unita_.end() || it->second.distrutta || it->second.persa; };
    if (!c.colpite.empty() && std::none_of(c.colpite.begin(), c.colpite.end(), [&](int id) {
            const auto it = unita_.find(id);
            return it == unita_.end() || it->second.colpo != 0 || it->second.affondamento != 0;
        }))
        return false;
    if (!c.fuori.empty() && std::none_of(c.fuori.begin(), c.fuori.end(), fuori)) return false;
    if (c.variabile) {
        const auto it = variabili_.find(c.indice);
        const int v = it == variabili_.end() ? 0 : it->second;
        const bool ok = c.op == "==" ? v == c.valore : c.op == "~=" || c.op == "!=" ? v != c.valore : c.op == ">=" ? v >= c.valore
                      : c.op == "<=" ? v <= c.valore : c.op == ">" ? v > c.valore : c.op == "<" ? v < c.valore : false;
        if (!ok) return false;
    }
    const auto& ev = registro_.eventi();
    if (!c.trasmesso.empty() && std::none_of(ev.begin(), ev.end(), [&](const EventoMissione& e) {
            return e.tipo == TipoEvento::trasmissione && nome_trasmissione(static_cast<TipoTrasmissione>(e.codice)) == c.trasmesso;
        }))
        return false;
    if (!c.evento.empty() && std::none_of(ev.begin(), ev.end(), [&](const EventoMissione& e) { return nome_evento(e.tipo) == c.evento; }))
        return false;
    return true;
}

bool Missione::manda_radio(const std::string& id) {
    if (!radio_) return false;
    const auto it = radio_catalogo_.find(id);
    if (it == radio_catalogo_.end() || radio_->in_coda(id)) return false;
    std::erase_if(radio_condizionati_, [&](const MessaggioCondizionato& x) { return x.m.id == id; });
    MessaggioRadio m = it->second;
    if (m.a.empty() && !radio_->ordini().empty()) m.a = radio_->ordini().front()->messaggio.a;
    radio_->programma(m, tempo_);
    return true;
}

void Missione::passo_radio(double dt) {
    const auto* g = giocatore();
    if (!g || g->distrutta || g->persa) return;
    const int id = g->id;
    // contatti d'intelligence: la verita' quando il messaggio va in onda
    // (i rapporti della forza, fase 3 B: quello che la forza sapeva, non la verita')
    for (const auto& [serie, r] : radio_->da_compilare(tempo_)) {
        if (const auto f = intelligence_della_forza(serie)) radio_->compila(serie, *f);
        else radio_->compila(serie, compila_intelligence(r));
    }
    // R3: i messaggi su condizione, alla prima volta che e' vera (non prima del loro dopo_s)
    for (auto it = radio_condizionati_.begin(); it != radio_condizionati_.end();) {
        if (tempo_ < it->dopo || !condizione_radio(it->se)) { ++it; continue; }
        MessaggioRadio m = it->m;
        if (m.a.empty()) m.a = radio_->ordini().empty() ? std::string{} : radio_->ordini().front()->messaggio.a;
        radio_->programma(m, tempo_);
        it = radio_condizionati_.erase(it);
    }
    radio_->avanza(tempo_, dt, antenne_giocatore());
    for (const auto& m : radio_->nuovi()) {
        if (!m.ordine.empty()) esegui(id, m.ordine);
        // TLAM: l'ordine d'esecuzione rende eseguibile la missione, solo se e' autentico (I12: un ordine col gruppo che non
        // torna non arma niente, e RDY resta spento; la sala radio gia' non li consegna fra i nuovi)
        if (const auto e = esecuzioni_tlam_.find(m.id); e != esecuzioni_tlam_.end() && m.autentico) tlam_eseguibili_.insert(e->second);
    }
    // premi dei rapporti di contatto
    for (auto it = supporti_.begin(); it != supporti_.end();) {
        if (it->first > tempo_) { ++it; continue; }
        const std::string gestore = it->second;
        it = supporti_.erase(it);
        messaggio_radio(MSG_SUPPORTO);
        esegui(id, gestore);
    }
    // boe SLOT: trasmettono dal punto di lancio, solo verso un aereo amico entro la portata VHF
    for (auto it = slot_attese_.begin(); it != slot_attese_.end();) {
        if (it->first > tempo_) { ++it; continue; }
        const Vec2 boa = it->second;
        it = slot_attese_.erase(it);
        messaggio_radio(MSG_SLOT);
        bool amico = false;
        for (const auto& [uid, u] : unita_)
            amico = amico || (!u.distrutta && !u.persa && (u.flag & 3) == 3 && ((u.flag ^ g->flag) & 0x10) == 0 && uid != id &&
                              distanza(u.pos, boa) <= slot_.portata_vhf_m);
        ContestoDF df;
        const int esm = esm_nemico_vicino(boa);
        df.esm_vicino = esm >= 0;
        StatoAntenne s;
        s.albero = true;
        s.latitudine = latitudine_ + boa.y / METRI_PER_GRADO_LAT;
        const EsitoTrasmissione es = radio_->trasmetti(TipoTrasmissione::contatto, tempo_, s, df);
        esito_radiogoniometria(es, esm, boa);
        registro_.trasmissione(*this, TipoTrasmissione::contatto, es, boa, true);
        if (amico && !rapporto_contatto_.gestore.empty() && !(rapporto_contatto_.una_volta && rapporto_contatto_.usato)) {
            rapporto_contatto_.usato = true;
            supporti_.push_back({tempo_ + rapporto_contatto_.ritardo_s, rapporto_contatto_.gestore});
        }
    }

    for (const auto& e : radio_->eventi()) {
        registro_.radio(*this, e);
        switch (e.tipo) {
            case EventoRadio::Tipo::in_arrivo: messaggio_radio(MSG_RADIO_IN_ARRIVO); break;
            case EventoRadio::Tipo::ricevuto:
                messaggio_radio(e.precedenza == Precedenza::flash ? MSG_RADIO_FLASH
                                : e.precedenza == Precedenza::immediate ? MSG_RADIO_IMMEDIATE : MSG_RADIO_RICEVUTO);
                break;
            case EventoRadio::Tipo::perso: messaggio_radio(MSG_RADIO_PERSO); break;
            case EventoRadio::Tipo::campanello: messaggio_radio(MSG_RADIO_ELF); break;
            case EventoRadio::Tipo::filo_rotto: messaggio_radio(MSG_FILO_ROTTO); break;
            case EventoRadio::Tipo::filo_senza_segnale: messaggio_radio(MSG_FILO_SENZA_SEGNALE); break;
            case EventoRadio::Tipo::satellite_assente: messaggio_radio(MSG_SATELLITE_ASSENTE); break;
            case EventoRadio::Tipo::trasmissione: messaggio_radio(MSG_TRASMISSIONE); break;
            case EventoRadio::Tipo::ssixs: messaggio_radio(MSG_SSIXS); break;
            case EventoRadio::Tipo::ordini_cambiati: messaggio_radio(MSG_ORDINI_CAMBIATI); break;
            case EventoRadio::Tipo::intelligence: messaggio_radio(MSG_INTELLIGENCE); break;
            case EventoRadio::Tipo::scaduto:
                messaggio_radio(MSG_COMPITO_SCADUTO);
                if (const auto* m = radio_->messaggio(e.serie); m && !m->gestore_scaduto.empty()) esegui(id, m->gestore_scaduto);
                break;
            case EventoRadio::Tipo::sessione_mancata: messaggio_radio(MSG_SESSIONE_MANCATA); break;
            case EventoRadio::Tipo::guinzaglio: if (!gestore_saltate_.empty()) esegui(id, gestore_saltate_); break;
        }
    }
    // R2: ingresso nelle zone protette che il giocatore conosce
    for (const auto& z : radio_->zone_ricevute()) {
        if (z.tipo != TipoZona::protetta && z.tipo != TipoZona::protetta_mobile) continue;
        const bool dentro = z.valida(tempo_) && dentro_zona(z, g->pos, posa_zona(z));
        if (dentro && zone_dentro_.insert(z.id).second) {
            messaggio_radio(MSG_ZONA_PROTETTA);
            radio_->registra_ingresso(tempo_, z.id);
            registro_.ingresso(*this, z.id);
        } else if (!dentro) {
            zone_dentro_.erase(z.id);
        }
    }
    for (auto it = fix_radio_.begin(); it != fix_radio_.end();) {
        if (it->quando > tempo_) { ++it; continue; }
        contatto_da_radiogoniometria(it->pos, -1);
        it = fix_radio_.erase(it);
    }
}

// --- emettitori radar, EMCON ed ESM dell'Enhanced (punto 23; progetto nelle note, progetto-esm.md) ---

namespace {

constexpr int MSG_ESM_NUOVO = 334, MSG_ESM_AEREO = 335, MSG_ESM_RISCHIO = 336, MSG_ESM_ILLUMINATI = 337, MSG_ESM_PERSO = 338;

}  // namespace

void Missione::elettronica_enhanced(const std::string& cartella_dati) {
    if (catalogo_el_) return;
    try {
        catalogo_el_ = std::make_unique<CatalogoElettronico>(carica_elettronica(cartella_dati + "/emettitori.toml"));
    } catch (const std::exception&) {
        return;   // senza i dati resta l'intercettazione dell'originale
    }
    if (const auto* g = giocatore())
        if (const RicevitoreEsm* r = catalogo_el_->esm_di(g->classe)) esm_giocatore_ = std::make_unique<EsmBattello>(*catalogo_el_, *r, seme_);
}

const std::vector<Intercettazione>& Missione::intercettazioni() const {
    static const std::vector<Intercettazione> nessuna;
    return esm_giocatore_ ? esm_giocatore_->intercettazioni() : nessuna;
}

std::optional<StatoEmcon> Missione::emcon(int id) const {
    const auto it = emcon_.find(id);
    if (it == emcon_.end()) return std::nullopt;
    return it->second.stato();
}

double Missione::altezza_radar(const UnitaClassica& u, const Radar& r) const {
    // l'antenna della classe misurata sul modello del kit, se c'e' (emettitori.toml [antenne], kit 31/10/2026)
    if (r.altezza_m > 0.0) return catalogo_el_ ? catalogo_el_->altezza_antenna(u.classe, r) : r.altezza_m;
    if (aereo(u)) return quota_aerea(u);   // le missioni danno quota 0 agli aerei: la vera dall'ASW, o 150/300 m
    return std::max(10.0, -u.quota);   // missili: qualche metro sul mare
}

// Un passo dell'elettronica: EMCON di ogni unita' con radar, radar accesi, ESM del giocatore, radar che vedono il suo
constexpr double NAVE_SOVRASTRUTTURE = 0.15, NAVE_ALTA_MIN_M = 6.0, NAVE_ALTA_MAX_M = 35.0, NAVE_P_GIRO = 0.9;   // radar contro nave
constexpr int MSG_RADAR_ACCESO = 458, MSG_RADAR_SPENTO = 459;   // il radar del giocatore (docs/18 fase 2)
constexpr double RADAR_SUL_BERSAGLIO_S = 1200.0;   // l'aereo sul bersaglio visto: come l'allarme dell'EMCON (nucleo/elettronica)

// albero (o il battello emerso)
void Missione::passo_elettronica(double dt) {
    auto* g = trova(giocatore() ? giocatore()->id : -1);
    std::vector<EmissioneVista> emissioni;
    // il radar del giocatore: emette solo fuori dall'acqua, e scendendo si spegne da solo (docs/18 fase 2)
    if (g && radar_giocatore_ && !radar_giocatore_acceso()) {
        radar_giocatore_ = false;
        messaggio_di_bordo(g->id, MSG_RADAR_SPENTO);
    }
    const bool radar_guasto_g = g && !interventi_.empty() && [&] {
        const auto f = parametro_attivo(interventi_, stati_interventi_, g->id, "sensore", "fattore", "radar");
        return f && *f <= 0.0;
    }();
    if (g && radar_giocatore_ && !radar_guasto_g)
        for (const Radar* r : catalogo_el_->radar_di(g->classe))
            emissioni.push_back({g->id, r, g->pos, altezza_radar(*g, *r), false, false});
    for (auto& [id, u] : unita_) {
        if (u.distrutta || u.persa || (g && id == g->id)) continue;   // il radar del giocatore: qui sopra
        const auto radar = catalogo_el_->radar_di(u.classe);
        if (radar.empty()) continue;
        auto it = emcon_.find(id);
        if (it == emcon_.end()) {
            const bool aereo = (u.flag & 3) == 3 || u.categoria == "aereo";
            // chi naviga col radar acceso come un mercantile: i civili e l'AGI Okean, che fa intelligence passiva ma
            // e' una nave della Marina (per le ROE resta fra le navi)
            const bool mercantile = classe_civile(u.classe) || u.classe == "agi";
            std::optional<Intermittenza> ciclo;
            if (const Intermittenza* c = catalogo_el_->intermittenza_di(u.classe)) ciclo = *c;
            // la chiave d'unita' `emcon` della missione (S13: i simulatori radar del bersaglio sempre accesi), se no la
            // dottrina della parte; gli eventi lo alzano e la discesa torna li'
            const auto ki = emcon_iniziali_.find(id);
            const StatoEmcon partenza = ki != emcon_iniziali_.end() ? ki->second : emcon_di_partenza(u.categoria, (u.flag & 0x10) != 0, aereo, mercantile);
            it = emcon_.emplace(id, Emcon(partenza, ciclo,
                                          seme_ ^ (static_cast<std::uint64_t>(id) * 0x9E37ull))).first;
        }
        // gli eventi dell'EMCON (dossier 11 §6.2): una minaccia aerea, cioe' un contatto su un aereo ostile
        if (u.categoria == "nave")
            for (const auto& [cid, c] : u.contatti)
                if (const auto ka = unita_.find(cid); ka != unita_.end() && !ka->second.distrutta && ((ka->second.flag & 3) == 3 || ka->second.categoria == "aereo") &&
                                                      ostili(u, ka->second) && c.qualita >= 800.0) {
                    it->second.evento(EventoEmcon::minaccia_aerea, tempo_);
                    break;
                }
        it->second.avanza(tempo_);
        // il vetrino di Dio: il radar spento da un guasto non emette (nessuno lo intercetta); a portata ridotta emette
        if (!interventi_.empty())
            if (const auto f = parametro_attivo(interventi_, stati_interventi_, id, "sensore", "fattore", "radar"); f && *f <= 0.0) continue;
        // l'aereo dell'ASW: in hover col sonar calato il radar e' spento; sul contatto sonar e nell'attacco lo spegne il
        // controllore, salvo che stia tenendo un albero o un battello in superficie (dossier 11 §6.2). Quando e' acceso,
        // decide l'EMCON come per tutti
        if (asw_)
            if (const auto a = asw_->aerei.find(id); a != asw_->aerei.end() && a->second.attivo) {
                const ComandoVolo& volo = a->second.c.volo();
                if (volo.hover || (!volo.radar && tempo_ - a->second.bersaglio_visto > RADAR_SUL_BERSAGLIO_S)) continue;
            }
        for (const Radar* r : radar)
            if (it->second.emette(*r))
                emissioni.push_back({id, r, u.pos, altezza_radar(u, *r), r->puntato && g && u.bersaglio == g->id,
                                     (u.flag & 3) == 3 || u.categoria == "aereo"});
    }
    // il seeker dei missili del modello acceso (DSQ-28: l'emettitore c'e', ruolo testa_missile), in aria
    for (const auto& [id, x] : missili_)
        if (x.missile.seeker_acceso() && !x.missile.dati().radar_seeker.empty())
            if (const Radar* r = catalogo_el_->trova_radar(x.missile.dati().radar_seeker))
                emissioni.push_back({id, r, x.missile.pos(), std::max(x.missile.quota(), 5.0), false, true});
    radar_del_passo_.clear();   // per le EcoNave dei missili (l'ESM del TASM)
    for (const auto& e : emissioni) radar_del_passo_.emplace_back(e.unita, e.radar->id);
    if (statistiche_) radar_per_statistiche(radar_del_passo_);   // R5 della sandbox: i radar accesi in questo passo
    if (g) {   // il giocatore: il suo ESM
        // con i periscopi dell'Enhanced: la testa vera fuori dall'acqua, l'ESM solo dall'antenna o da una testa che ce l'ha
        const auto testa = testa_giocatore();
        const bool albero = peri_ ? (testa.has_value() || (g->flag54 & 4) != 0) : (g->flag54 & 6) != 0;
        const bool albero_esm = peri_ ? ((g->flag54 & 4) != 0 || esm_in_testa()) : albero;
        const bool emerso = g->quota < 0.5;
        const bool affiorata = vela_affiorata();   // la vela fuori dall'acqua col mare grosso (M2)
        const Esposizione esp = emerso || affiorata ? Esposizione::emerso : (g->flag54 & 4) ? Esposizione::albero : Esposizione::periscopio;
        // ESM del giocatore: intercettazioni, avvisi, contatti col solo rilevamento
        if (esm_giocatore_) {
            std::vector<EmissioneVista> altrui;   // non il proprio radar
            for (const auto& e : emissioni) if (e.unita != g->id) altrui.push_back(e);
            esm_giocatore_->avanza(tempo_, dt, g->pos, g->quota, albero_esm, esp, stato_mare(), altrui);
            for (const auto& e : esm_giocatore_->eventi()) {
                const int codice = e.tipo == EventoEsm::Tipo::nuovo ? MSG_ESM_NUOVO : e.tipo == EventoEsm::Tipo::aereo_asw ? MSG_ESM_AEREO
                                 : e.tipo == EventoEsm::Tipo::rischio_alto ? MSG_ESM_RISCHIO
                                 : e.tipo == EventoEsm::Tipo::illuminato ? MSG_ESM_ILLUMINATI : MSG_ESM_PERSO;
                bool gia = false;
                for (const auto& x : messaggi_esm_) gia = gia || (x.codice == codice && x.tempo == tempo_);
                if (!gia) messaggi_esm_.push_back({tempo_, g->id, codice});
            }
            for (const auto& i : esm_giocatore_->intercettazioni()) {
                if (i.ultima != tempo_ || !trova(i.unita)) continue;
                possibile(g->id, i.unita, "esm", 0.0);
                Contatto* c = g->contatti.count(i.unita) ? &g->contatti.at(i.unita) : nuovo_contatto(*g, i.unita);
                if (!c) continue;
                c->qualita = std::max(c->qualita, 200.0);
                c->lampeggio = 0;
                c->ultima_scoperta = tempo_;
                if (!c->posizione_nota || c->sensore == "esm") { c->rilevamento = i.rilevamento; c->sensore = "esm"; }
            }
        }
    }
    // ESM dell'IA (docs/18 fase 2): ogni unita' la cui classe ha un ricevitore; navi e aerei con l'antenna sempre fuori,
    // i sottomarini emersi, con l'albero o con la testa. Contatti col solo rilevamento, sugli emettitori ostili
    for (auto& [id, u] : unita_) {
        if (esm_ia_spento_ || u.giocatore || u.distrutta || u.persa || (u.flag & 0x20)) continue;
        const RicevitoreEsm* r = catalogo_el_->esm_di(u.classe);
        if (!r) continue;
        const bool sub = u.categoria == "sottomarino";
        const bool fuori = !sub || u.quota < 0.5 || (u.flag54 & 4) || testa_ia_fuori(id);
        auto it = esm_ia_.find(id);
        if (it == esm_ia_.end()) it = esm_ia_.emplace(id, std::make_unique<EsmBattello>(*catalogo_el_, *r, seme_ ^ (static_cast<std::uint64_t>(id) * 0x5E5Dull))).first;
        const Esposizione e_ia = !sub || u.quota < 0.5 ? Esposizione::emerso : (u.flag54 & 4) ? Esposizione::albero : Esposizione::periscopio;
        it->second->avanza(tempo_, dt, u.pos, u.quota, fuori, e_ia, stato_mare(), emissioni);
        it->second->eventi();   // gli avvisi sono per l'equipaggio del giocatore
        for (const auto& i : it->second->intercettazioni()) {
            if (i.ultima != tempo_ || i.unita == id) continue;
            const UnitaClassica* emittente = trova(i.unita);
            if (!emittente || !ostili(u, *emittente)) continue;
            possibile(id, i.unita, "esm", 0.0);
            Contatto* c = u.contatti.count(i.unita) ? &u.contatti.at(i.unita) : nuovo_contatto(u, i.unita);
            if (!c) continue;
            c->qualita = std::max(c->qualita, 200.0);
            c->lampeggio = 0;
            c->ultima_scoperta = tempo_;
            if (!c->posizione_nota || c->sensore == "esm") { c->rilevamento = i.rilevamento; c->sensore = "esm"; }
        }
    }
    // radar contro gli alberi e contro le navi in superficie (docs/18 fase 2: per tutti i radar accesi)
    for (auto& [id, s] : unita_) {
        if (s.distrutta || s.persa || (s.flag & 0x20)) continue;
        if (s.categoria == "sottomarino") for (const auto& e : emissioni) radar_contro(e, s, dt);
        else if (s.categoria == "nave") for (const auto& e : emissioni) radar_contro_nave(e, s, dt);
    }
}

// Una nave in superficie: entro l'orizzonte radar dalle sue sovrastrutture (0,15 della lunghezza, 6-35 m: STIMA) e, se
// il radar ce l'ha, la sua portata contro nave; 0,9 per giro fino all'80% della portata, poi a scendere (STIMA, kit §4.6).
// Il radar del giocatore le vede tutte, quelli dell'IA le navi dell'altro schieramento
void Missione::radar_contro_nave(const EmissioneVista& e, UnitaClassica& n, double dt) {
    if (e.unita == n.id) return;
    if (id_traffico_.count(e.unita) || missili_.count(e.unita)) return;   // il traffico e il seeker emettono, non scoprono
    auto& u = unita_.at(e.unita);
    if (!u.giocatore && ((u.flag ^ n.flag) & 0x18) == 0) return;   // la stessa parte (i neutrali, 0x18, sono una parte)
    double lunghezza = 100.0;
    if (const auto d = dimensioni_classi_.find(n.classe); d != dimensioni_classi_.end() && d->second.lunghezza_m > 0.0) lunghezza = d->second.lunghezza_m;
    const double alto = std::clamp(NAVE_SOVRASTRUTTURE * lunghezza, NAVE_ALTA_MIN_M, NAVE_ALTA_MAX_M);
    const Radar& r = *e.radar;
    double portata = orizzonte_radar_m(e.altezza_m, alto);
    if (r.contro_nave_m > 0.0) portata = std::min(portata, r.contro_nave_m);
    const double d = distanza(u.pos, n.pos);
    if (d > portata) return;
    const double p_giro = d <= 0.8 * portata ? NAVE_P_GIRO : NAVE_P_GIRO * (portata - d) / (0.2 * portata);
    if (p_giro > 0.0) possibile(u.id, n.id, "radar", 0.0);
    const double p = 1.0 - std::pow(1.0 - p_giro, dt / std::max(r.periodo_s, 0.05));
    if (p <= 0.0 || caso_acustica_.uniforme() >= p) return;
    Contatto* c = u.contatti.count(n.id) ? &u.contatti.at(n.id) : nuovo_contatto(u, n.id);
    if (!c) return;
    c->qualita = std::max(c->qualita, 800.0);
    c->lampeggio = 0;
    c->ultima_scoperta = tempo_;
    c->rilevamento = rilevamento(u.pos, n.pos);
    c->pos_stimata = n.pos;
    c->quota_stimata = 0.0;
    c->posizione_nota = true;
    c->t_posizione = tempo_;
    c->sensore = "radar";
    if (!u.giocatore) scelta_bersaglio(u, n, *c);
}

bool Missione::radar_giocatore_acceso() const {
    const auto* g = giocatore();
    return g && radar_giocatore_ && catalogo_el_ && (g->quota < 0.5 || ((g->flag54 & 4) && g->quota <= QUOTA_PERISCOPICA_M + 0.5));
}

void Missione::radar_contro(const EmissioneVista& e, UnitaClassica& s, double dt) {
    if (id_traffico_.count(e.unita) || missili_.count(e.unita)) return;   // il traffico e il seeker emettono, non scoprono
    auto& u = unita_.at(e.unita);
    if (((u.flag ^ s.flag) & 0x18) == 0) return;   // solo le altre parti (anche i neutrali, 0x18, per tutti)
    // l'esposizione: per il giocatore la testa vera dei periscopi dell'Enhanced, per l'IA albero, testa e superficie
    const bool emerso = s.quota < 0.5;
    std::optional<TestaEsposta> testa;
    bool affiorata = false, albero = false;
    if (s.giocatore) {
        testa = testa_giocatore();
        affiorata = vela_affiorata();   // la vela fuori dall'acqua col mare grosso (M2)
        albero = peri_ ? (testa.has_value() || (s.flag54 & 4) != 0) : (s.flag54 & 6) != 0;
    } else {
        albero = (s.flag54 & 6) != 0 || testa_ia_fuori(s.id);
    }
    if (!albero && !emerso && !affiorata) return;
    const Esposizione esp = emerso || affiorata ? Esposizione::emerso : (s.flag54 & 4) ? Esposizione::albero : Esposizione::periscopio;
    const Radar& r = *e.radar;
    const auto ci = u.contatti.find(s.id);
    // dossier 11 §6.4: 0,3 non allertato (anche le navi da guerra senza contatti ne' allarme: taratura P5), 0,7 con
    // un contatto sul bersaglio o l'EMCON in ingaggio, 0,9 con la sua posizione; i mercantili sempre 0,3
    const bool mercantile = classe_civile(u.classe) || u.classe == "agi";   // come in passo_elettronica
    const auto em = emcon_.find(e.unita);   // il giocatore non ha EMCON: e' lui che decide
    const bool allerta = ci != u.contatti.end() || (em != emcon_.end() && em->second.stato() == StatoEmcon::ingaggio);
    const double f_op = mercantile ? 0.3 : ci != u.contatti.end() && ci->second.posizione_nota ? 0.9 : allerta ? 0.7 : 0.3;
    const double pioggia = meteo_ascolto(s.pos).pioggia_mm_h;
    // il vetrino di Dio: il radar guasto a una frazione della portata (la distanza vista come piu' lunga), spento: niente
    double d_radar = distanza(u.pos, s.pos);
    if (!interventi_.empty())
        if (const auto f = parametro_attivo(interventi_, stati_interventi_, u.id, "sensore", "fattore", "radar")) {
            if (*f <= 0.0) return;
            d_radar /= *f;
        }
    const double p_giro = testa && !emerso && !affiorata && !(s.flag54 & 4)
                            ? probabilita_giro(r, d_radar, e.altezza_m, *testa, stato_mare(), f_op, s.velocita, pioggia)
                            : probabilita_giro(r, d_radar, e.altezza_m, esp, stato_mare(), f_op, s.velocita, pioggia,
                                               artiglieria_ ? artiglieria_->dati.sagoma(s.classe).falsatorre_altezza_m : -1.0);   // la vela vera (kit)
    if (p_giro > 0.0) possibile(u.id, s.id, "radar", 0.0);
    const double p = 1.0 - std::pow(1.0 - p_giro, dt / std::max(r.periodo_s, 0.05));
    if (p <= 0.0 || caso_acustica_.uniforme() >= p) return;
    Contatto* c = ci != u.contatti.end() ? &ci->second : nuovo_contatto(u, s.id);
    if (!c) return;
    c->qualita = std::max(c->qualita, 800.0);
    c->lampeggio = 0;
    c->ultima_scoperta = tempo_;
    c->rilevamento = rilevamento(u.pos, s.pos);
    c->pos_stimata = s.pos;      // il radar da' distanza e rilevamento
    c->quota_stimata = s.quota;
    c->posizione_nota = true;
    c->t_posizione = tempo_;
    c->sensore = "radar";
    if (em != emcon_.end()) em->second.evento(EventoEmcon::albero_visto, tempo_);
    if (!u.giocatore) evento_allerta(u.id, "albero");   // fase 3: albero visto al radar
    if (!u.giocatore) scelta_bersaglio(u, s, *c);
}

// --- paesaggio sonoro (S5c): biologici, geofisici, attivita' umane attorno al giocatore ---

namespace {
constexpr double RAGGIO_PAESAGGIO_M = 100000.0, SPOSTAMENTO_PAESAGGIO_M = 50000.0, ASCOLTO_PAESAGGIO_S = 2.0;
constexpr double ASCOLTO_PAESAGGIO_IA_S = 10.0, PAESAGGIO_IA_M = 30000.0;   // gli ascoltatori dell'IA (docs/18 fase 2, F)
constexpr int ID_PAESAGGIO = 100000, PASSO_ID_PAESAGGIO = 10000;
// la rete a strascico: il cavo e' circa 3 volte il fondale (300-1500 m), la rete 2 m sopra il fondo (STIMA)
constexpr int ID_RETI = 930000;
constexpr double RETE_CAVO_PER_FONDALE = 3.0, RETE_MIN_M = 300.0, RETE_MAX_M = 1500.0, RETE_SUL_FONDO_M = 2.0;
}  // namespace

// Popola le sorgenti nel disco attorno al giocatore (dossier 09 §6.2 del disassemblatore), le muove con la scala del
// moto della missione, le ripopola con id e semi nuovi quando il giocatore si sposta di oltre 50 km (restano quelle
// vive dentro il disco nuovo, delle nuove solo quelle fuori dal disco vecchio) e le ascolta col sonar del giocatore
// (livello A, ogni 2 s) in contatti_paesaggio(): niente posizione, classificazione e avviso 346 per i biologici
void Missione::passo_paesaggio(double dt) {
    const auto* g = giocatore();
    if (!usa_paesaggio_ || !g || g->distrutta || g->persa) return;
    const int mese = clima_ ? mese_acustica() : (mese_ > 0 ? mese_ : 2);
    if (!paesaggio_pronto_ || distanza(g->pos, centro_paesaggio_) > SPOSTAMENTO_PAESAGGIO_M) {
        const int n = paesaggio_pronto_ ? ++ripopolazioni_ : 0;
        auto nuove = popola_sorgenti(*catalogo_, teatro_, mese, -1.0, g->pos, RAGGIO_PAESAGGIO_M,
                                     seme_ ^ (static_cast<std::uint64_t>(n) * 0x51ED27ull), ID_PAESAGGIO + n * PASSO_ID_PAESAGGIO);
        if (paesaggio_pronto_) {
            const Vec2 vecchio = centro_paesaggio_;
            std::erase_if(paesaggio_, [&](const IstanzaSorgente& i) {
                const bool via = distanza(i.pos, g->pos) > RAGGIO_PAESAGGIO_M;
                if (via) { contatti_paesaggio_.erase(i.id); coppie_.erase({g->id, i.id}); }
                return via;
            });
            for (auto& i : nuove) if (distanza(i.pos, vecchio) > RAGGIO_PAESAGGIO_M) paesaggio_.push_back(i);
        } else {
            paesaggio_ = std::move(nuove);
        }
        centro_paesaggio_ = g->pos;
        paesaggio_pronto_ = true;
    }
    const double dt_m = dt * m_s_per_nodo_ / NODO;   // il moto classico accelera il mondo
    for (auto& i : paesaggio_) muovi_sorgente(i, dt_m);
    // fase 2 (docs/18, F): gli ascoltatori dell'IA dentro il disco sentono le stesse istanze, ogni 10 s ed entro 30 km.
    // Niente contatti (il pool): vanno al loro sonarista, che puo' sbagliarle (un peschereccio come POSSUB)
    if (!sonaristi_ia_spenti_ && tempo_ - ultimo_ascolto_paesaggio_ia_ >= ASCOLTO_PAESAGGIO_IA_S) {
        const double finestra_ia = std::min(tempo_ - ultimo_ascolto_paesaggio_ia_, ASCOLTO_PAESAGGIO_IA_S);
        ultimo_ascolto_paesaggio_ia_ = tempo_;
        for (const auto& [id, o] : unita_) {
            if (o.giocatore || o.distrutta || o.persa || (o.flag & 0x20) || aereo(o)) continue;
            if ((o.categoria != "sottomarino" && o.categoria != "nave") || distanza(o.pos, centro_paesaggio_) > RAGGIO_PAESAGGIO_M) continue;
            const auto sens = sensori_acustici(o, TipoSensori::ascolto);
            if (sens.empty()) continue;
            const auto ascolta = [&](const IstanzaSorgente& i) {
                if (distanza(o.pos, i.pos) > PAESAGGIO_IA_M) return;
                const Sorgente s = sorgente_da_istanza(*catalogo_, i, tempo_, finestra_ia);
                const Ambiente amb = ambiente_acustico(o.pos, i.pos);
                Rilevazione x;
                const Sensore* usato = nullptr;
                for (const Sensore* sn : sens)
                    if (const Rilevazione r = ::m688::valuta(s, ricevitore_acustico(o, sn), amb); r.se_db > x.se_db) { x = r; usato = sn; }
                if (!usato || x.se_db < 0.0) return;
                ascolto_ia(o, i.id, rilevamento(o.pos, i.pos), s.fonte && s.fonte->categoria == "biologico", usato, x.se_db,
                           errore_rilevamento_deg(x, *usato));
            };
            for (const auto& i : paesaggio_) ascolta(i);
            for (const auto& i : reti_) ascolta(i);
        }
    }
    if (tempo_ - ultimo_ascolto_paesaggio_ < ASCOLTO_PAESAGGIO_S) return;
    const double finestra = tempo_ - ultimo_ascolto_paesaggio_;
    ultimo_ascolto_paesaggio_ = tempo_;
    const auto sensori = sensori_acustici(*g, TipoSensori::ascolto);
    if (sensori.empty()) return;
    std::vector<const IstanzaSorgente*> ascoltate;
    for (const auto& i : paesaggio_) ascoltate.push_back(&i);
    for (const auto& i : reti_) ascoltate.push_back(&i);
    for (const IstanzaSorgente* pi : ascoltate) {
        const IstanzaSorgente& i = *pi;
        const Sorgente s = sorgente_da_istanza(*catalogo_, i, tempo_, finestra);
        const Ambiente amb = ambiente_acustico(g->pos, i.pos);
        Rilevazione x;
        const Sensore* usato = nullptr;
        for (const Sensore* sn : sensori)
            if (const Rilevazione r = ::m688::valuta(s, ricevitore_acustico(*g, sn), amb); r.se_db > x.se_db) { x = r; usato = sn; }
        auto& coppia = coppie_[{g->id, i.id}];
        const double dc = coppia.ultima < 0.0 ? 0.0 : tempo_ - coppia.ultima;
        coppia.ultima = tempo_;
        coppia.fluttuazione.avanza(dc, TAU_PASSIVO_S, SIGMA_DB, caso_acustica_);
        const double se = x.se_db + coppia.fluttuazione.db;
        auto it = contatti_paesaggio_.find(i.id);
        if (it == contatti_paesaggio_.end()) {
            if (se < 0.0) continue;
            it = contatti_paesaggio_.emplace(i.id, Contatto{}).first;
        }
        Contatto& c = it->second;
        c.qualita = std::clamp(c.qualita + QUALITA_PER_DB_S * std::clamp(se, -10.0, 20.0) * dc, 0.0, 1500.0);
        c.segnale = 500.0 + 25.0 * se;
        c.eccesso_db = se;
        c.lampeggio = se < 0.0 ? 0x80 : 0;
        if (c.qualita <= 0.0 && se < 0.0) { contatti_paesaggio_.erase(it); continue; }
        if (se < 0.0) continue;
        c.ultima_scoperta = tempo_;
        c.sensore = usato ? usato->id : "";
        c.rilevamento = normalizza_360(rilevamento(g->pos, i.pos) + (usato ? errore_rilevamento_deg(x, *usato) : 1.0) * gauss());
        const double prima = c.durata;
        const double tonale = x.se_tonale_db + coppia.fluttuazione.db;
        if (tonale >= SE_CLASSIFICA) c.durata = std::min(1399.0, c.durata + dc * (1.0 + tonale / 5.0));   // mai una classe
        if (prima < DURATA_CLASSIFICATO && c.durata >= DURATA_CLASSIFICATO && s.fonte && s.fonte->categoria == "biologico")
            messaggio_sonar(MSG_BIOLOGICO);
    }
}

const IstanzaSorgente* Missione::istanza_paesaggio(int id) const {
    for (const auto& i : paesaggio_) if (i.id == id) return &i;
    for (const auto& i : reti_) if (i.id == id) return &i;
    return nullptr;
}

void Missione::passo_reti() {
    reti_.clear();
    const SorgenteLibreria* rete = catalogo_->sorgente("rete_strascico");
    if (!rete) return;
    for (const auto& [id, u] : unita_) {
        // i pescherecci a strascico (il tipo civile del catalogo: peschereccio e BMRT, anche del traffico)
        const TipoCivile* tc = catalogo_->tipo_civile(u.classe);
        if (!(tc ? tc->strascico : u.classe == "peschereccio") || u.distrutta || u.persa) continue;
        if (u.velocita < rete->v_min_kn || u.velocita > rete->v_max_kn) continue;   // non sta strascicando
        const double fondo = profondita(u.pos);
        if (!std::isfinite(fondo)) continue;
        const double cavo = std::clamp(RETE_CAVO_PER_FONDALE * fondo, RETE_MIN_M, RETE_MAX_M);
        const double r = radianti(u.rotta);
        IstanzaSorgente i;
        i.id = ID_RETI + id;
        i.sorgente = rete->id;
        i.pos = {u.pos.x - cavo * std::sin(r), u.pos.y - cavo * std::cos(r)};
        const double qui = profondita(i.pos);
        i.quota_m = std::max(0.0, (std::isfinite(qui) ? qui : fondo) - RETE_SUL_FONDO_M);
        i.rotta = u.rotta;
        i.velocita_kn = u.velocita;
        i.seme = seme_ ^ (static_cast<std::uint64_t>(id) * 0x7E7E5A1Dull);
        reti_.push_back(i);
    }
}

// Avvisi del giocatore che non vengono da un contatto: cavitazione propria e schiera cieca in virata
std::uint64_t Missione::impronta_contatti() const {
    const auto* g = giocatore();
    if (!g) return 0;
    std::uint64_t h = 1469598103934665603ull;
    const auto mescola = [&h](std::int64_t v) {
        for (int i = 0; i < 8; ++i) {
            h ^= (static_cast<std::uint64_t>(v) >> (8 * i)) & 0xffu;
            h *= 1099511628211ull;
        }
    };
    mescola(static_cast<std::int64_t>(g->contatti.size()));
    std::vector<int> id;
    for (const auto& [k, c] : g->contatti) id.push_back(k);
    std::sort(id.begin(), id.end());
    for (const int k : id) {
        const Contatto& c = g->contatti.at(k);
        mescola(k);
        mescola(static_cast<std::int64_t>(std::llround(c.rilevamento * 10.0)));
        mescola(c.posizione_nota ? 1 : 0);
    }
    return h;
}

void Missione::avvisi_sonar() {
    // fine dell'ultima virata oltre il limite: il raddrizzamento della schiera, per tutti anche senza giocatore
    for (const auto& [id, u] : unita_)
        if (std::fabs(rateo_virata_vero(u)) > SCHIERA_VIRATA) fine_virata_[id] = tempo_;
    const auto* g = giocatore();
    if (!g || g->distrutta || g->persa) return;
    bool cavita = false;
    if (const auto* d = dinamica(g->id)) cavita = d->cavita();
    if (cavita && !cavitava_) messaggio_sonar(MSG_CAVITIAMO);
    cavitava_ = cavita;
    const bool cieca = (g->flag54 & 1) && std::fabs(rateo_virata_vero(*g)) > SCHIERA_VIRATA;
    if (cieca && !schiera_cieca_) messaggio_sonar(MSG_SCHIERA_CIECA);
    schiera_cieca_ = cieca;
}

// scelta del bersaglio (§8): con uno script +10, il conto del contatto (o solo di quello agganciato) scende;
// a zero il contatto diventa +58 e lo script gira una volta, poi +10 e +60 si azzerano
void Missione::scelta_bersaglio(UnitaClassica& o, UnitaClassica& b, Contatto& c) {
    const int ido = o.id, idb = b.id;
    if (o.g_bersaglio.empty()) return;
    if (id_traffico_.count(idb)) return;   // il traffico civile non entra negli script (docs/20 §3)
    if (o.agganciato >= 0 && o.agganciato != idb) return;
    if ((o.flag & 0x20) && (c.qualita <= 800.0 || (b.flag & 0x20))) return;   // le armi: traccia solida, non armi
    if (--c.conto > 0) return;
    c.conto = 1;
    o.bersaglio = idb;
    const std::string script = o.g_bersaglio;
    o.g_bersaglio.clear();
    o.agganciato = -1;
    esegui(ido, script);
}

int Missione::contatto_orfano(const UnitaClassica& o, int b) const {
    const auto it = o.contatti.find(b);
    std::uint64_t primo = it == o.contatti.end() ? UINT64_MAX : it->second.ordine;
    int orfano = -1;
    for (const auto& [id, c] : o.contatti) {
        const auto u = unita_.find(id);
        if (c.ordine < primo && (u == unita_.end() || u->second.distrutta || u->second.persa)) { primo = c.ordine; orfano = id; }
    }
    return orfano;
}

void Missione::perdi_contatto(UnitaClassica& o, int b) {
    o.contatti.erase(b);
    if (o.bersaglio == b) o.bersaglio = -1;
    if (o.agganciato == b || o.inseguito == b) {   // bersaglio perso: navigazione e +10 azzerati, poi +56
        o.navigazione = o.insegui = false;
        o.g_bersaglio.clear();
        o.agganciato = o.inseguito = -1;
        const std::string r = o.g_reazione;
        if (!r.empty()) esegui(o.id, r);
    }
}

void Missione::dinamica_sottomarini(const std::map<std::string, ClasseNave>& classi, const std::string& cartella_dati) {
    if (dinamica_attiva_) return;
    dinamica_attiva_ = true;
    for (const auto& [id, c] : classi) lunghezze_[id] = c.lunghezza_m;   // per le navi col mare (meteo, M2)
    for (const auto& [id, c] : classi) {
        if (c.scafo.empty()) continue;
        DatiScafo scafo;
        try { scafo = DatiScafo::carica(cartella_dati + "/" + c.scafo); } catch (const std::exception&) { continue; }
        if (scafo.sottomarino.presente) parametri_sub_[id] = parametri_sottomarino(id, &c, &scafo.sottomarino);
    }
}

const DinamicaSottomarino* Missione::dinamica(int id) const {
    const auto it = dinamiche_.find(id);
    return it == dinamiche_.end() ? nullptr : &it->second;
}

// Moto dell'Enhanced per un sottomarino con la dinamica (fase C6a); falso se l'unita' segue il moto classico.
DinamicaSottomarino* Missione::muovi_con_dinamica(UnitaClassica& u, const Pilota& p, double dt) {
    if (!dinamica_attiva_ || u.categoria != "sottomarino" || u.affondamento > 0) return nullptr;
    const auto par = parametri_sub_.find(u.classe);
    if (par == parametri_sub_.end()) return nullptr;
    auto it = dinamiche_.find(u.id);
    if (it == dinamiche_.end()) it = dinamiche_.emplace(u.id, DinamicaSottomarino(par->second, u.rotta, u.quota, u.velocita)).first;
    DinamicaSottomarino& d = it->second;
    // fondale, script ed emersione del motore possono spostare la quota: la dinamica riparte da li'
    if (u.quota != d.quota_m()) d.imposta_quota(u.quota);
    if (u.rotta != d.rotta()) d.imposta_rotta(u.rotta);
    if (u.velocita != d.velocita_kn()) d.imposta_velocita(u.velocita);
    ComandiSottomarino c;
    if (const auto cs = comandi_sub_.find(u.id); cs != comandi_sub_.end()) {
        c = cs->second;
    } else {
        // timoniere: rotta, velocita' e quota ordinate (dagli script o dal pilota del fondale). Una virata con il
        // rateo diventa barra in proporzione (3,6 gradi/s = barra tutta), finche' la prua passa la rotta ordinata
        c.giri = d.giri_per_velocita(p.velocita);
        c.quota_m = p.quota;
        c.barra_max_deg = 20.0;
        if (u.rateo_virata != 0.0 && !p.rotta) c.barra_deg = std::clamp(u.rateo_virata / 3.6, -1.0, 1.0) * DinamicaSottomarino::BARRA_MAX;
        else c.rotta = p.rotta.value_or(u.rotta_ordinata);
    }
    ForzeIdrostatiche f{u.ordine.peso_netto_N, u.ordine.momento_assetto_Nm};
    LimitiBattello l;
    l.timone = u.ordine.timone;
    l.idroplani = u.ordine.idroplani;
    if (u.ordine.velocita_max_kn >= 0.0) l.velocita = std::min(1.0, u.ordine.velocita_max_kn / d.parametri().velocita_max_kn);
    const double rotta_prima = d.rotta();
    d.avanza(dt, c, f, l);
    if (c.barra_deg != 0.0 && !c.rotta && !u.rotta_libera && u.rateo_virata != 0.0) {
        // come il moto classico: la virata si ferma quando la prua passa la rotta ordinata
        const double fatto = std::fabs(normalizza_180(d.rotta() - rotta_prima));
        const double resto = u.rateo_virata > 0.0 ? normalizza_360(u.rotta_ordinata - rotta_prima) : normalizza_360(rotta_prima - u.rotta_ordinata);
        if (resto <= fatto) u.rateo_virata = 0.0;
    }
    u.rotta = d.rotta();
    u.velocita = d.velocita_kn();
    // peso netto non ancora passato dal battello danneggiato: la quota la da' il suo rateo (moto del kit)
    if (u.ordine.quota_fisica && f.peso_netto_N == 0.0 && f.momento_assetto_Nm == 0.0) {
        u.quota = std::max(0.0, u.quota + u.ordine.rateo_quota_fisica * dt);
        d.imposta_quota(u.quota);
    } else {
        u.quota = d.quota_m();
    }
    return &d;
}

void Missione::navigazione(double dt) {
    // Navigazione e arrivo (2011:0533-0691, §5): soglia +50 in quarti di unita' per asse, entrambi gli assi
    // richiesti; con soglia 120 alle navi basta un asse (difetto della terza condizione); soglia 0: mai. L'inseguimento
    // accosta solo sul rilevamento finche' la traccia del bersaglio non e' solida, poi va sulla sua posizione e,
    // se chi insegue non e' una nave o un aereo (un siluro, un sottomarino), ne uguaglia la quota (2011:057F-05A2).
    std::vector<std::pair<int, std::string>> arrivi;
    for (auto& [id, u] : unita_) {
        if (u.distrutta || u.persa || siluri_.count(id) || missili_.count(id) || id_traffico_.count(id)) continue;   // siluri, missili e traffico: i loro passi
        if (!rimorchi_.empty() && rimorchiatore_di(id) >= 0) continue;   // M6: il rimorchiato lo muove passo_avarie
        Vec2 meta = meta_per_navigazione(u, u.meta);   // il vai_a di un sottomarino dell'IA col navigatore: dalla stima
        // in affondamento (+55) niente navigazione, fondale e controlli: l'unita' prosegue con gli ultimi ordini
        const bool affonda = u.affondamento > 0;
        if (u.navigazione && !affonda) {
            bool controlla_arrivo = true;
            if (u.insegui) {
                if (const auto* t = trova(u.inseguito >= 0 ? u.inseguito : u.bersaglio)) {
                    meta = t->pos;
                    const auto c = u.contatti.find(u.bersaglio);
                    const bool solida = c != u.contatti.end() && c->second.qualita >= 800.0;
                    controlla_arrivo = solida;
                    if (solida && !(u.flag & 6)) u.quota_ordinata = std::min(t->quota, u.quota_max);   // chi insegue non e' nave o aereo (057F)
                } else {
                    controlla_arrivo = false;
                }
            }
            const double s = u.soglia_arrivo / 4.0 * metri_per_unita_;
            const bool cx = std::fabs(u.pos.x - meta.x) <= s, cy = std::fabs(u.pos.y - meta.y) <= s;
            const bool arrivato = u.soglia_arrivo > 0 && controlla_arrivo &&
                                  ((cx && cy) || (u.soglia_arrivo == 120 && u.quota < 4.572 && (cx || cy)));
            if (arrivato) {
                u.navigazione = u.insegui = false;
                if (!u.g_arrivo.empty()) arrivi.push_back({id, u.g_arrivo});
            } else if (distanza(u.pos, meta) > 1.0) {
                u.rotta_ordinata = rilevamento(u.pos, meta);
            }
            // accostata (2011:0705-0790): oltre 3 gradi dalla rotta da prendere si rallenta a un quarto di avanti tutta,
            // ricordando la velocita' (al piu' avanti tutta: da MAX si torna a TUTTA); allineati, la si riprende
            if (u.navigazione) {
                const double errore = std::fabs(normalizza_180(u.rotta_ordinata - u.rotta));
                const double lenta = std::max(u.avanti_tutta / 4.0, 1.0 / 350.0);
                if (errore > 3.0) {
                    const double v = std::fabs(u.velocita_ordinata - u.velocita) > 1e-9 ? u.velocita_ordinata : u.velocita;
                    if (u.velocita_salvata == 0.0 && v > lenta) {
                        u.velocita_salvata = std::min(v, u.avanti_tutta);
                        u.velocita_ordinata = lenta;
                        if (const auto* g = giocatore(); g && g->id == u.id) messaggio(u.id, MSG_RIDUCO_PER_ACCOSTARE);
                    }
                } else if (u.velocita_salvata != 0.0) {
                    u.velocita_ordinata = u.velocita_salvata;
                    u.velocita_salvata = 0.0;
                }
            }
        }
        // Fondale: l'IA segue rotte per acque navigabili, aggira le secche, rallenta per accostare vicino alla costa
        // e tiene un franco sotto la chiglia dei sottomarini; il giocatore no.
        // ordini del motore (reazioni ai danni): prevalgono sugli script finche' durano
        if (u.ordine.fino_a < tempo_) u.ordine = {};
        if (u.ordine.meta && !affonda) {
            meta = u.ordine.verso;
            u.navigazione = false;
            if (distanza(u.pos, meta) > 200.0) u.rotta_ordinata = rilevamento(u.pos, meta);
            u.velocita_ordinata = u.ordine.velocita_kn;
        }
        if (!affonda && !u.giocatore && (manovre_difesa_.size() || reazioni_in_corso_.size() || armi_ia_))   // fase 4: difesa, reazioni, TMA
            if (const auto mi = manovra_ia(u)) {
                meta = mi->verso;
                if (distanza(u.pos, meta) > 200.0) u.rotta_ordinata = rilevamento(u.pos, meta);
                u.velocita_ordinata = mi->velocita_kn;
            }
        if (u.ordine.velocita_max_kn >= 0.0) u.velocita_ordinata = std::min(u.velocita_ordinata, u.ordine.velocita_max_kn);
        // il giocatore cambia quota solo al rateo ordinato (0: tiene la quota); l'IA almeno a 0,5 m/s per il pilota
        Pilota p{std::nullopt, u.quota_ordinata, u.velocita_ordinata, u.giocatore ? u.rateo_quota : std::max(u.rateo_quota, 0.5)};
        if (fondale_ && segue_il_fondale(u) && !u.giocatore && !affonda) p = pilota(u, meta);
        if (asw_ && !affonda)   // ASW aereo dell'Enhanced: fuori dalla pattuglia il controllore guida l'aereo
            if (const auto ga = guida_asw(u)) { p.rotta = ga->rotta; p.velocita = ga->velocita; }
        bool evade = false;
        if (asw_ && !affonda && !u.giocatore)   // il sottomarino dell'IA che evade la caccia aerea (nucleo/evasione.hpp)
            if (const auto ev = guida_evasione(u)) {
                evade = true;
                const bool fondo = fondale_ && segue_il_fondale(u);
                // le secche restano del pilota; anche le sue frenate e il franco sotto la chiglia
                p.rotta = fondo ? evita_secche(u, ev->rotta_deg, 1500.0, acqua_ia(u).comoda).value_or(ev->rotta_deg) : ev->rotta_deg;
                const bool frena = p.velocita < u.velocita_ordinata - 1e-9;
                p.velocita = frena ? std::min(p.velocita, ev->velocita_kn) : ev->velocita_kn;
                if (ev->sul_fondo) {
                    p.quota = ev->quota_m;
                    p.rateo_quota = 0.5;   // ci si posa piano: sotto 1 m/s l'urto non fa danni
                } else {
                    p.quota = p.quota < u.quota_ordinata - 1e-9 ? std::min(p.quota, ev->quota_m) : ev->quota_m;
                    p.rateo_quota = std::max(p.rateo_quota, 1.0);
                }
            }
        bool batteria = false;
        if (batterie_attive_ && !affonda && !u.giocatore)   // il diesel che snorkella o emerge senza batteria (M6)
            if (const auto b = guida_batteria(u); b && (!evade || emersione_forzata(u.id))) {
                batteria = true;
                // sotto il pack verso l'acqua libera dove salire (missione_diesel.cpp)
                if (const auto bi = batterie_.find(u.id); bi != batterie_.end() && bi->second.rifugio) p.rotta = b->rotta_deg;
                p.velocita = std::min(p.velocita, b->velocita_kn);
                p.quota = b->quota_m;
                p.rateo_quota = std::max(p.rateo_quota, 1.0);
            }
        const auto ml = missili_ia_.find(u.id);   // la manovra di lancio dei missili dell'IA: niente Crazy Ivan
        const bool lancio = ml != missili_ia_.end() && ml->second.in_manovra;
        if (collisioni_ && !affonda && !u.giocatore && !evade && !batteria && !lancio)   // Crazy Ivan dei sottomarini sovietici (dossier 03 §C4)
            if (const auto ci = guida_crazy_ivan(u)) {
                const bool fondo = fondale_ && segue_il_fondale(u);
                const auto devia = fondo ? evita_secche(u, ci->rotta_deg, 1500.0, acqua_ia(u).comoda) : std::nullopt;
                // A2: con la posizione incerta il comandante prudente non fa il Crazy Ivan verso una secca (guardando della
                // sigma due volte piu' lontano): tiene la rotta del pilota
                const auto n = fondo ? nav_ia_.find(u.id) : nav_ia_.end();
                const bool incerta = n != nav_ia_.end() && fondo_creduto(u);
                if (!(incerta && (devia || secca_avanti(u, ci->rotta_deg, std::min(1500.0 + 2.0 * n->second.sigma_m(), PORTATA_INCERTA_MAX_M),
                                                        acqua_ia(u).comoda)))) {
                    p.rotta = devia.value_or(ci->rotta_deg);
                    p.velocita = ci->velocita_kn;
                    p.quota = ci->quota_m;
                    p.rateo_quota = std::max(p.rateo_quota, 1.0);
                }
            }
        double virata_max = u.virata;
        if (armi_ia_ && !u.giocatore && !affonda)   // fase 4 B: il lanciatore che guida a filo non rompe il filo
            if (const auto vf = armi_ia_->vincoli.find(u.id); vf != armi_ia_->vincoli.end()) {
                p.velocita = std::min(p.velocita, vf->second.velocita_max_kn);
                virata_max = std::min(virata_max, vf->second.virata_max_deg_s);
                // la prua ordinata poco avanti alla prua: anche la dinamica accosta piano
                const double e = normalizza_180(p.rotta.value_or(u.rotta_ordinata) - u.rotta);
                p.rotta = normalizza_360(u.rotta + std::clamp(e, -ANTICIPO_FILO_DEG, ANTICIPO_FILO_DEG));
            }
        if (catalogo_ && !u.giocatore && !affonda && u.categoria == "nave")   // il traino del sonar (missione_vds.cpp)
            p.velocita = velocita_traino(u, p.velocita);
        const double quota_prec = u.quota;
        if (!muovi_con_dinamica(u, p, dt)) {
            // rotta: virata continua (timone) o accostata verso la rotta ordinata (o del pilota)
            if (!u.ordine.timone) {
                // il timone bloccato da una collisione (DanniMissione): la nave tiene la prua
            } else if (u.rateo_virata != 0.0 && !p.rotta) {
                // virata con il rateo: si ferma sulla rotta ordinata quando la prua ci passa (salvo rotta libera).
                // Con la prua gia' sulla rotta ordinata l'originale si ferma subito virando a sinistra, mentre a dritta fa
                // il giro intero (confronti di 09FC-0A20)
                const double passo = u.rateo_virata * dt;
                double resto = u.rateo_virata > 0.0 ? normalizza_360(u.rotta_ordinata - u.rotta) : normalizza_360(u.rotta - u.rotta_ordinata);
                if (resto == 0.0 && u.rateo_virata > 0.0) resto = 360.0;
                if (!u.rotta_libera && resto <= std::fabs(passo)) { u.rotta = u.rotta_ordinata; u.rateo_virata = 0.0; }
                else u.rotta = normalizza_360(u.rotta + passo);
            } else {
                const double e = normalizza_180(p.rotta.value_or(u.rotta_ordinata) - u.rotta), m = virata_max * dt;
                u.rotta = normalizza_360(u.rotta + std::clamp(e, -m, m));
            }
            if (meteo_ && u.categoria == "nave") p.velocita = std::min(p.velocita, velocita_col_mare(u));   // M2
            const double dv = p.velocita - u.velocita, mv = u.accelerazione * dt;
            u.velocita += std::clamp(dv, -mv, mv);
            const double dq = p.quota - u.quota, mq = p.rateo_quota * dt;
            if (u.ordine.quota_fisica) u.quota = std::max(0.0, u.quota + u.ordine.rateo_quota_fisica * dt);   // D6c
            else u.quota += std::clamp(dq, -mq, mq);
        }
        const double r = radianti(u.rotta), vm = metri_al_secondo(u.velocita);
        const Vec2 nuova{u.pos.x + vm * std::sin(r) * dt, u.pos.y + vm * std::cos(r) * dt};
        if (!fondale_ || affonda) { u.pos = nuova; continue; }
        if (!segue_il_fondale(u)) {   // armi e oggetti sott'acqua: non scendono sotto il fondo
            u.pos = nuova;
            if (u.quota <= 0.0) continue;
            if (profondita(u.pos) <= 0.0 && u.categoria == "arma") {   // siluro sulla costa: perso (§9.3)
                u.persa = true;
                esm_ = 0;   // 003C
                if (const auto* l = trova(u.collegata); l && l->giocatore) messaggio(l->id, MSG_SILURO_PERSO);
                continue;
            }
            u.quota = std::min(u.quota, std::max(profondita(u.pos) - 1.0, 0.0));
            continue;
        }
        // Moto orizzontale: si ferma davanti a un fondale piu' alto della chiglia (che non sia gia' quello in cui
        // si trova: da una secca si puo' sempre uscire verso acqua piu' profonda).
        const double chiglia = std::max(u.quota, pescaggio(u)), qui = profondita(u.pos), la = profondita(nuova);
        if (la < chiglia && la < qui) {
            if (!u.incagliata) {
                urta_fondo(u, u.velocita * NODO);   // velocita' vera, non quella del moto classico
                u.incagliata = true;
                u.punto_incaglio = u.pos;
            }
            u.velocita = 0.0;
        } else {
            u.pos = nuova;
            if (u.incagliata && distanza(u.pos, u.punto_incaglio) > DISINCAGLIO_M) u.incagliata = false;
        }
        // Moto verticale: la chiglia si appoggia sul fondo
        const double fondo = profondita(u.pos);
        if (u.quota > fondo && fondo >= 0.0) {
            const double v_verticale = std::max(u.quota - quota_prec, 0.0) / dt;
            u.quota = fondo;
            if (!u.sul_fondo) urta_fondo(u, v_verticale);
            u.sul_fondo = true;
        } else if (u.quota < fondo - 1.0) {
            if (u.sul_fondo && trattieni_sul_fondo(u)) u.quota = fondo;   // Enhanced: il fango lo trattiene (missione_fondo.cpp)
            else u.sul_fondo = false;
        }
    }
    for (const auto& [id, g] : arrivi) esegui(id, g);
}

double Missione::profondita(Vec2 p) const {
    return fondale_ ? fondale_->profondita(p) : std::numeric_limits<double>::infinity();
}

double Missione::pescaggio(const UnitaClassica& u) const {
    if (const auto it = pescaggi_.find(u.classe); it != pescaggi_.end() && it->second > 0.0) return it->second;
    return u.categoria == "sottomarino" ? 8.0 : (u.categoria == "nave" ? 6.0 : 0.0);   // classe senza dati
}

void Missione::imposta_fondale(std::shared_ptr<const Fondale> fondale, std::map<std::string, double> pescaggi) {
    acque_.clear();
    acque_carta_.clear();
    carte_prudenti_.clear();
    fondale_ = std::move(fondale);
    pescaggi_ = std::move(pescaggi);
    for (auto& [id, u] : unita_) colloca(u);
}

void Missione::colloca(UnitaClassica& u) {
    // Come il convertitore (docs/09_schema_missioni.md §3), ma con il pescaggio della classe: le navi e i
    // sottomarini che partono (o che gli script creano o spostano) in acqua meno che comoda per l'IA (pescaggio
    // + 3 m per le navi, + 10 m per i sottomarini) vanno nell'acqua comoda piu' vicina, con la chiglia dei
    // sottomarini ad almeno 10 m dal fondo.
    // Il battello del giocatore solo sotto l'acqua minima (pescaggio + 0,5 m) o in acqua chiusa: la comoda serve all'IA
    // in navigazione, non al punto di partenza scelto dalla missione (SCENE0A: in 15 m di fondo, a vista del bersaglio
    // dei missili; con la comoda finiva a 10-35 km, oltre l'orizzonte). Vale anche nella sandbox, col comandante IA
    if (!fondale_ || !segue_il_fondale(u)) return;
    const double minima = u.giocatore ? acqua_ia(u).minima : acqua_ia(u).comoda;
    int c, r;
    const AcqueNavigabili& a = acque(minima);
    const bool chiusa = fondale_->cella(u.pos, c, r) && a.componente(c, r) >= 0 && a.celle(a.componente(c, r)) < ACQUE_APERTE_CELLE;
    if ((profondita(u.pos) < minima || chiusa) && fondale_->cella(u.pos, c, r) && a.piu_vicina(c, r, -1, ACQUE_APERTE_CELLE)) {
        const Vec2 nuova = fondale_->centro_cella(c, r);
        spostamenti_.push_back({u.id, distanza(u.pos, nuova)});
        u.pos = nuova;
    }
    const double limite = std::max(profondita(u.pos) - FRANCO_IA_M, 0.0);
    u.quota = std::min(u.quota, limite);
    u.quota_ordinata = std::min(u.quota_ordinata, limite);
}

bool Missione::secca_avanti(const UnitaClassica& u, double rotta, double portata, double richiesta) const {
    // vero se lungo la rotta, entro la portata, il fondo sale sotto l'acqua richiesta (e sotto quella attuale)
    const double qui = profondita_pilota(u, u.pos), r = radianti(rotta);
    for (int k = 1; k <= 4; ++k) {
        const double d = portata * k / 4.0;
        const double p = profondita_pilota(u, {u.pos.x + d * std::sin(r), u.pos.y + d * std::cos(r)});
        if (p < richiesta && p < qui) return true;
    }
    return false;
}

std::optional<Missione::FondoCreduto> Missione::fondo_creduto(const UnitaClassica& u) const {
    if (u.giocatore || u.categoria != "sottomarino") return std::nullopt;
    const auto n = nav_ia_.find(u.id);
    const auto f = fondale_ia_.find(u.id);
    if (n == nav_ia_.end() || f == fondale_ia_.end()) return std::nullopt;   // senza la navigazione incerta o il quadro: il vero
    const Parte parte = (u.flag & 0x18) == 0x10 ? Parte::urss : Parte::nato;
    const Fondale* carta = fondale_carta(parte);
    if (!carta) return std::nullopt;
    const double sigma = n->second.sigma_m();
    const int raggio = static_cast<int>(std::ceil(std::min(RAGGIO_ROTTA_SIGMA * sigma, RAGGIO_ROTTA_MAX_M) / RAGGIO_PASSO_M));
    return FondoCreduto{&f->second.quadro, carta, parte, raggio, n->second.errore(), sigma};
}

// il fondo dove il battello crede di essere, come lo sa (carta e misure fuse), prudente: meno PRUDENZA_SIGMA sigma della
// cella. Il margine di lato per l'errore di posizione lo danno le rotte sulla carta con le secche allargate
// (carta_prudente): qui no, se no la terra di fianco finirebbe sotto la prua e il battello salirebbe per niente. Senza A2
// la profondita' vera
double Missione::profondita_pilota(const UnitaClassica& u, Vec2 p) const {
    const auto fc = fondo_creduto(u);
    if (!fc) return profondita(p);
    const LetturaCella l = fc->quadro->leggi(Vec2{p.x + fc->errore.x, p.y + fc->errore.y}, tempo_);
    return l.profondita_m - PRUDENZA_SIGMA * l.sigma_m;
}

const Fondale& Missione::carta_prudente(const FondoCreduto& fc) {
    if (fc.raggio <= 0) return *fc.carta;
    auto& c = carte_prudenti_[{static_cast<int>(fc.parte), fc.raggio}];
    if (!c) c = std::make_unique<Fondale>(allarga_secche(*fc.carta, fc.raggio * RAGGIO_PASSO_M));
    return *c;
}

const AcqueNavigabili& Missione::acque_carta(const FondoCreduto& fc, double minima) {
    const auto chiave = std::make_tuple(static_cast<int>(fc.parte), fc.raggio, static_cast<int>(std::ceil(minima)));
    auto& a = acque_carta_[chiave];
    if (!a) a = std::make_unique<AcqueNavigabili>(carta_prudente(fc), std::get<2>(chiave));
    return *a;
}

Missione::Pilota Missione::pilota(UnitaClassica& u, Vec2 meta) {
    Pilota p{std::nullopt, u.quota_ordinata, u.velocita_ordinata, std::max(u.rateo_quota, 0.5)};
    const AcquaRichiesta acqua = acqua_ia(u);
    const auto fc = fondo_creduto(u);   // A2: il fondo creduto, o il vero
    const double vg = metri_al_secondo(u.velocita);   // velocita' del moto (classico o vero)
    const double frenata = vg * vg / (2.0 * std::max(u.accelerazione, 0.01) * metri_al_secondo(1.0));   // spazio, m
    double portata = std::clamp(vg * 60.0, 300.0, 3000.0);
    const double portata_classica = portata;
    if (const auto n = nav_ia_.find(u.id); n != nav_ia_.end() && u.categoria == "sottomarino")   // la navigazione incerta: guarda piu' avanti (A1)
        portata = std::min(portata + 2.0 * n->second.sigma_m(), PORTATA_INCERTA_MAX_M);
    bool rallenta = false;
    // 1. verso una meta: rotta per acque navigabili (vuota se la via diretta e' libera), ripianificata quando la
    //    meta si sposta di oltre 500 m (al piu' ogni 10 s, come nell'inseguimento; ogni minuto se non e'
    //    raggiungibile), quando la via diretta va ricontrollata (ogni 30 s) e dopo un incaglio. Se la meta non e'
    //    raggiungibile (un'acqua non collegata, troppo bassa per il pescaggio) l'unita' si ferma nel punto piu'
    //    vicino.
    if (u.navigazione) {
        const double da_ultimo = tempo_ - u.pianificato;
        const bool meta_nuova = distanza(meta, u.meta_percorso) > 500.0 && da_ultimo >= (u.percorso_parziale ? 60.0 : 10.0);
        const bool ricontrolla = da_ultimo >= 30.0 && ((u.percorso.empty() && !u.percorso_parziale) || u.incagliata);
        if (meta_nuova || ricontrolla) {
            u.pianificato = tempo_;
            u.meta_percorso = meta;
            const double soglia = std::clamp(std::min(profondita_pilota(u, u.pos), profondita_pilota(u, meta)), acqua.minima, acqua.comoda);
            u.percorso.clear();
            u.percorso_parziale = false;
            if (fc) {   // A2: sulla carta della sua parte, nel riferimento della stima, al largo; i punti tornano nel vero
                const Vec2 e = fc->errore, da{u.pos.x + e.x, u.pos.y + e.y}, a{meta.x + e.x, meta.y + e.y};
                // gia' piu' vicino alla secca del largo voluto (la deriva, un Crazy Ivan): prima torna al largo, se e' vicino
                // (entro due volte il raggio); se no il largo che ha, non di meno
                FondoCreduto piano = *fc;
                std::optional<Vec2> al_largo;
                for (int c, r; piano.raggio > 0; --piano.raggio) {
                    const AcqueNavigabili& aq = acque_carta(piano, acqua.minima);
                    if (!aq.fondale().cella(da, c, r) || aq.componente(c, r) >= 0) break;
                    if (piano.raggio == fc->raggio && aq.piu_vicina(c, r, -1, ACQUE_APERTE_CELLE)) {
                        const Vec2 q = aq.fondale().centro_cella(c, r);
                        if (distanza(da, q) < 2.0 * piano.raggio * RAGGIO_PASSO_M) {
                            al_largo = q;
                            break;
                        }
                    }
                }
                const Vec2 inizio = al_largo.value_or(da);
                if (al_largo) u.percorso.push_back({inizio.x - e.x, inizio.y - e.y});
                if (al_largo || !tratto_libero(carta_prudente(piano), da, a, soglia)) {
                    Rotta r = rotta_navigabile(acque_carta(piano, acqua.minima), inizio, a, acqua.comoda);
                    if (!r.completa && piano.raggio > 0) {   // il largo chiude il passaggio: la carta com'e'
                        FondoCreduto stretto = piano;
                        stretto.raggio = 0;
                        if (Rotta s = rotta_navigabile(acque_carta(stretto, acqua.minima), inizio, a, acqua.comoda); s.completa) r = std::move(s);
                    }
                    for (const Vec2& q : r.punti) u.percorso.push_back({q.x - e.x, q.y - e.y});
                    u.percorso_parziale = !r.completa;
                }
            } else if (!tratto_libero(*fondale_, u.pos, meta, soglia)) {
                Rotta r = rotta_navigabile(acque(acqua.minima), u.pos, meta, acqua.comoda);
                u.percorso = std::move(r.punti);
                u.percorso_parziale = !r.completa;
            }
        }
        // punto raggiunto (100 m) o superato: il successivo e' gia' in vista con acqua sufficiente
        const auto libero = [&](Vec2 da, Vec2 a, double soglia, double passo) {
            if (!fc) return tratto_libero(*fondale_, da, a, soglia, passo);
            const Vec2 e = fc->errore;
            return tratto_libero(carta_prudente(*fc), {da.x + e.x, da.y + e.y}, {a.x + e.x, a.y + e.y}, soglia, passo);
        };
        while (!u.percorso.empty() && (distanza(u.pos, u.percorso.front()) < 100.0 ||
                                       (u.percorso.size() > 1 && libero(u.pos, u.percorso[1], acqua.minima, 100.0))))
            u.percorso.erase(u.percorso.begin());
        portata = std::min(portata, std::max(distanza(u.pos, u.percorso.empty() ? meta : u.percorso.front()), 50.0));
        if (!u.percorso.empty()) {
            p.rotta = rilevamento(u.pos, u.percorso.front());
            rallenta = std::fabs(normalizza_180(*p.rotta - u.rotta)) > 45.0;   // accostata vicino alla costa
        } else if (u.percorso_parziale) {
            p.rotta = u.rotta;   // arrivata il piu' vicino possibile: ferma
            p.velocita = 0.0;
        }
        if (u.percorso_parziale && u.percorso.size() == 1 && distanza(u.pos, u.percorso.front()) < frenata + 100.0)
            p.velocita = 0.0;   // frena in tempo per fermarsi sull'ultimo punto
    }
    // 2. senza rotta pianificata: aggiramento a vista delle secche davanti
    if (!p.rotta) {
        p.rotta = evita_secche(u, u.rateo_virata != 0.0 ? u.rotta : u.rotta_ordinata, portata, acqua.comoda);
        rallenta = p.rotta.has_value();
    }
    // 2b. le mine viste dal suo sonar del fondo (missione_bqs15.cpp): al largo, piano
    if (!mine_scoperte_ia_.empty())
        if (const auto rm = evita_mine(u, p.rotta.value_or(u.rotta_ordinata))) {
            p.rotta = *rm;
            rallenta = true;
        }
    // 3. quota: franco sotto la chiglia qui e lungo la prua; se il fondo sale piu' in fretta di quanto il
    //    sottomarino possa risalire al rateo massimo del modello, rallenta
    const double rateo_max = std::max({u.rateo_quota, u.rateo_quota_modello, 0.5});
    const double r = radianti(u.rotta);
    double fondo = profondita_pilota(u, u.pos);
    // (lungo la prua alla portata di sempre: con A1 piu' avanti si guarda per accostare, non per salire)
    const double portata_quota = std::min(portata, portata_classica);
    for (int k = 1; k <= 4; ++k) {
        const double d = portata_quota * k / 4.0, pf = profondita_pilota(u, {u.pos.x + d * std::sin(r), u.pos.y + d * std::cos(r)});
        fondo = std::min(fondo, pf);
        if (u.quota - rateo_max * d / std::max(vg, 0.1) > pf - FRANCO_IA_M / 2.0) rallenta = true;
    }
    p.quota = std::min(p.quota, std::max(fondo - FRANCO_IA_M, 0.0));
    if (p.quota < u.quota && p.quota < u.quota_ordinata) p.rateo_quota = rateo_max;   // risalita imposta dal fondo
    // A2: con la posizione incerta in acqua bassa il comandante prudente rallenta
    if (fc && fc->sigma_m > SIGMA_RALLENTA_M && fondo - u.quota < ACQUA_RALLENTA_M) rallenta = true;
    if (rallenta) p.velocita = std::min(p.velocita, MANOVRA_IA_KN);
    // 4. freno di emergenza: se entro una volta e mezza lo spazio di frenata (piu' 200 m), dritto o lungo l'arco
    //    della virata verso la rotta voluta, la chiglia incontra un fondo piu' alto, macchine ferme (l'unita'
    //    accosta comunque)
    const double chiglia = std::max(u.quota, pescaggio(u)) + 0.3, qui = profondita_pilota(u, u.pos);
    //    (48 campioni al piu', ogni 25 m o piu')
    const double tratto = 1.5 * frenata + 200.0, ds = std::max(25.0, tratto / 48.0), dt_arco = ds / std::max(vg, 1.0);
    const double voluta = p.rotta.value_or(u.rotta_ordinata);
    const bool timone = u.rateo_virata != 0.0 && !p.rotta;
    double h = u.rotta;
    Vec2 q = u.pos;
    for (double d = ds; d <= tratto && p.velocita > 0.0; d += ds) {
        if (timone) h += u.rateo_virata * dt_arco;
        else h += std::clamp(normalizza_180(voluta - h), -u.virata * dt_arco, u.virata * dt_arco);
        q = {q.x + ds * std::sin(radianti(h)), q.y + ds * std::cos(radianti(h))};
        for (const Vec2 x : {q, Vec2{u.pos.x + d * std::sin(r), u.pos.y + d * std::cos(r)}}) {
            const double pf = profondita_pilota(u, x);
            if (pf < chiglia && pf < qui) p.velocita = 0.0;
        }
    }
    return p;
}

const AcqueNavigabili& Missione::acque(double minima) {
    // una mappa delle acque collegate per soglia, arrotondata al metro per eccesso
    const int chiave = static_cast<int>(std::ceil(minima));
    auto& a = acque_[chiave];
    if (!a) a = std::make_unique<AcqueNavigabili>(*fondale_, chiave);
    return *a;
}

AcquaRichiesta Missione::acqua_ia(const UnitaClassica& u) const {
    const double pc = pescaggio(u);
    double comoda = pc + (u.categoria == "sottomarino" ? ACQUA_SOTTOMARINO_IA_M : ACQUA_NAVE_IA_M);
    // la navigazione incerta (A1 della principale, 29/10/2026): il sottomarino che non sa bene dov'e' si tiene in acqua
    // piu' profonda, cioe' piu' al largo, come un comandante prudente (STIMA: 1 m ogni 20 m di sigma, al piu' 40)
    if (const auto n = nav_ia_.find(u.id); n != nav_ia_.end() && !u.giocatore && u.categoria == "sottomarino")
        comoda += std::min(MARGINE_INCERTO_MAX_M, n->second.sigma_m() / MARGINE_INCERTO_SIGMA_PER_M);
    return {pc + 0.5, comoda};
}

std::optional<double> Missione::evita_secche(UnitaClassica& u, double rotta_voluta, double portata, double richiesta) {
    // Prima rotta libera a scarti di 15 gradi dalla voluta, dal lato gia' scelto (per non oscillare) o da quello
    // con piu' acqua; nessuna se la voluta e' libera o se non ce n'e' una.
    if (!secca_avanti(u, rotta_voluta, portata, richiesta)) { u.lato_evasione = 0; return std::nullopt; }
    int lato = u.lato_evasione;
    if (lato == 0) {
        auto acqua = [&](double g) {
            const double r = radianti(rotta_voluta + g);
            return profondita_pilota(u, {u.pos.x + portata * std::sin(r), u.pos.y + portata * std::cos(r)});
        };
        lato = acqua(45.0) >= acqua(-45.0) ? 1 : -1;
    }
    for (int passo = 1; passo <= 12; ++passo)
        for (int l : {lato, -lato}) {
            const double rotta = normalizza_360(rotta_voluta + l * 15.0 * passo);
            if (!secca_avanti(u, rotta, portata, richiesta)) { u.lato_evasione = l; return rotta; }
        }
    return std::nullopt;
}

void Missione::urta_fondo(UnitaClassica& u, double v) {
    // Danno proporzionale all'energia dell'urto (l'originale toglie sempre 40). Il giocatore riceve i messaggi
    // dell'originale: 30 se lo scafo e' danneggiato, 9 se cede, con l'esito 1 delle missioni ("si e' incagliato"),
    // che nessuno script imposta (§9.5). Un tocco sotto 1 m/s non fa danni e non da' messaggi.
    const double f = v < URTO_SENZA_DANNI_M_S ? 0.0 : std::min(1.0, (v / URTO_FATALE_M_S) * (v / URTO_FATALE_M_S));
    const int danno = static_cast<int>(std::ceil(f * u.integrita_max));
    if (danno == 0) return;
    u.integrita -= danno;
    if (u.integrita > 0) {
        if (u.giocatore) messaggio(u.id, MSG_FONDO_DANNI);
        return;
    }
    u.distrutta = true;
    if (u.giocatore) {
        messaggio(u.id, MSG_URTO_FATALE);
        if (!esito_) esito_ = ESITO_INCAGLIATO;
    }
}

// --- colpi e distruzione (docs/originale/sensori_e_navigazione.md §9.4 e §9.8) ---------------------------------------

void Missione::togli(UnitaClassica& u) {
    u.persa = true;
    esm_ = 0;   // 003C
    annulla_timer(u.id, nullptr);
}

void Missione::uccisione(int lanciatore) {   // 2011:0B18: contatore delle uccisioni per lanciatore (variabili 10..19)
    if (lanciatore >= 0 && lanciatore < 10) ++variabili_[10 + lanciatore];
}

constexpr int MSG_ESERCITAZIONE_A_SEGNO = 456, MSG_ESERCITAZIONE_COLPITI = 457;   // schede I03 e S01

void Missione::impatti() {
    // 2011:0C23: un'arma colpisce qualunque unita' (non solo il suo bersaglio, anche amici e neutrali) entro 5 unita'
    // per asse (finestra quadrata, |dx| e |dy| < 20 quarti) e 100 piedi di quota; mai un'altra arma, un aereo, un
    // falso bersaglio (80h) o chi e' collegato con lei (+0C, finche' c'e' il filo). L'originale controlla solo le
    // coppie dell'osservatore del passo (Missione::impatto, predefinito); qui, per la regola di simulazione, ogni
    // coppia a ogni passo.
    for (auto& [ida, a] : unita_) {
        if (!(a.flag & 0x20) || a.distrutta || a.persa || siluri_.count(ida) || missili_.count(ida)) continue;   // modello: la sua spoletta
        for (auto& [idv, v] : unita_) {
            if (idv == ida || v.distrutta || v.persa) continue;
            if ((v.flag & 0x20) || (v.flag & 3) == 3 || (v.flag & 0x80)) continue;
            if (!a_contatto(a, v)) continue;
            colpisci(a, v);
            break;
        }
    }
}

bool Missione::a_contatto(const UnitaClassica& a, const UnitaClassica& b) const {
    // 2011:0C5C-0CD1: |dx| e |dy| < 20 quarti di unita', |dquota| < 100 piedi, e nessuno dei due collegato all'altro
    const double q = 4.0 / metri_per_unita_;
    return std::floor(std::fabs(a.pos.x - b.pos.x) * q) < FINESTRA_IMPATTO_QUARTI &&
           std::floor(std::fabs(a.pos.y - b.pos.y) * q) < FINESTRA_IMPATTO_QUARTI &&
           std::fabs(a.quota - b.quota) < FINESTRA_IMPATTO_QUOTA_M && a.collegata != b.id && b.collegata != a.id;
}

bool Missione::impatto(UnitaClassica& o, UnitaClassica& b) {
    // 2011:0CD3-0D04, nella coppia osservatore/bersaglio: se l'osservatore e' un'arma colpisce il bersaglio, che non
    // sia un aereo o un falso bersaglio; se e' il bersaglio a essere un'arma colpisce l'osservatore, che non sia un
    // aereo (un falso bersaglio si'). Fra due armi non si arriva qui (0C25).
    if (siluri_.count(o.id) || siluri_.count(b.id) || missili_.count(o.id) || missili_.count(b.id) || !a_contatto(o, b)) return false;
    if (o.flag & 0x20) {
        if ((b.flag & 3) == 3 || (b.flag & 0x80)) return false;
        colpisci(o, b);
        return true;
    }
    if (!(b.flag & 0x20) || (o.flag & 3) == 3) return false;
    colpisci(b, o);
    return true;
}

bool Missione::esercitazione(int id, bool si) {
    const UnitaClassica* u = trova(id);
    if (!u) return false;
    auto& insieme = (u->flag & 0x20) ? armi_esercitazione_ : lanciatori_esercitazione_;
    if (si) insieme.insert(id); else insieme.erase(id);
    return true;
}

void Missione::colpisci(UnitaClassica& a, UnitaClassica& v) {
    // schede I03 e S01: un'arma da esercitazione esce dal gioco come all'impatto, col colpo registrato e senza danni
    if (!armi_esercitazione_.empty() && armi_esercitazione_.count(a.id)) {
        colpi_esercitazione_.push_back({tempo_, a.id, a.lanciatore, v.id, a.classe, a.pos});
        const auto* g = giocatore();
        if (g && a.lanciatore == g->id) messaggio(g->id, MSG_ESERCITAZIONE_A_SEGNO);
        if (g && v.id == g->id) messaggio(g->id, MSG_ESERCITAZIONE_COLPITI);
        togli(a);
        return;
    }
    // 2011:0B2D: l'arma esce subito dal gioco (con i suoi timer, anche quello di fine corsa), senza eventi acustici
    const int lanciatore = a.lanciatore;
    const bool categoria3 = (a.flag & 3) == 3;
    ultimo_lanciatore_ = lanciatore;
    togli(a);
    const auto* g = giocatore();
    const int locale = g ? g->id : -1;
    const auto* l = lanciatore >= 0 ? trova(lanciatore) : nullptr;
    if (l && lanciatore < 2 && !ostili(*l, v)) {   // 2011:0AD4: fuoco amico (anche sui neutrali)
        fuoco_amico_[lanciatore] = true;
        if (lanciatore == locale) messaggio(lanciatore, MSG_FUOCO_AMICO);
    }
    int danno = a.difesa;   // +3D dell'arma
    if (conto_5100_ < 8) danno /= 2;   // dimezzato in 7 passi su 18
    Colpo c{Colpo::Tipo::arma, tempo_, v.id, a.id, lanciatore, v.classe, a.classe, a.pos, a.quota, danno, true, false,
            l ? l->classe : std::string{}};
    const int a_segno = categoria3 ? MSG_MISSILE_A_SEGNO : MSG_A_SEGNO;
    const int resto = alla_fisica(v.id) ? std::max(v.integrita, 0) : v.integrita - danno;   // simulazione: decide il motore
    if (alla_fisica(v.id) && v.affondamento == 0) {   // l'integrita' scende ma non distrugge: decide il motore
        v.integrita = std::max(0, v.integrita - danno);
        v.colpo = v.colpo ? 0xC9 : 1;
        v.lanciatore = lanciatore;   // per il contatore delle uccisioni di distruggi()
        messaggio(v.id, a_segno);
    } else if (resto >= 0) {   // la vittima sopravvive (anche a integrita' 0)
        v.integrita = resto;
        v.colpo = v.colpo ? 0xC9 : 1;
        v.sonar = std::max(0, v.sonar - 2);   // 2011:0C10: scafo danneggiato, piu' rumoroso e sordo
        v.rumore += 2;
        if (v.id == locale) messaggio(v.id, MSG_COLPITI);
        else if (v.id < 2) { messaggio(v.id, MSG_ALTRO_COLPITO); messaggio(v.id, a_segno); }
        else messaggio(v.id, a_segno);
    } else if (v.affondamento != 0) {   // sta gia' affondando: l'arma si consuma e basta
        if (v.id != locale) messaggio(v.id, a_segno);
    } else {   // distrutta: parte il conto delle visite
        uccisione(lanciatore);
        v.affondamento = 18; v.integrita = 0; v.colpo = 0xD3;
        c.distrutta = true;
        if (v.id >= 2) messaggio(v.id, a_segno);
        else {
            const bool relitto = (v.flag54 & 0x80) != 0;
            v.affondamento = relitto ? 1 : 30;
            if (v.id == locale) { messaggio(v.id, MSG_COLPITI); if (!relitto) messaggio(v.id, MSG_EMERSIONE); }
            else messaggio(v.id, MSG_ALTRO_COLPITO);
        }
    }
    colpi_.push_back(c);
}

void Missione::applica_attacco(UnitaClassica& v, int danno, Colpo::Tipo tipo) {
    // 2011:1CDE (ATTACK e ATTACK_RND)
    if (v.affondamento != 0) return;
    const auto* g = giocatore();
    const int locale = g ? g->id : -1;
    // missili: il lanciatore e' il +62 della vittima (SELECT_LINKED); cariche: chi esegue ATTACK
    const int lanciatore = tipo == Colpo::Tipo::attacco_casuale ? v.lanciatore : contesto().esecutore;
    const auto* l = lanciatore >= 0 ? trova(lanciatore) : nullptr;
    Colpo c{tipo, tempo_, v.id, -1, lanciatore, v.classe, {}, v.pos, v.quota, danno, true, false, l ? l->classe : std::string{}};
    if (tipo == Colpo::Tipo::attacco_casuale)   // il missile: l'arma dell'ultimo lancio di quel lanciatore (la testata)
        for (auto it = lanci_missili_.rbegin(); it != lanci_missili_.rend(); ++it)
            if (it->unita == lanciatore && !it->arma.empty()) { c.classe_arma = it->arma; break; }
    if (alla_fisica(v.id)) {   // simulazione: il colpo arriva alla fisica, la distruzione la decide il motore
        v.integrita = std::max(0, v.integrita - danno);
        colpi_.push_back(c);
        return;
    }
    const int resto = v.integrita - danno;
    if (resto < 0) {
        v.colpo = 0xD3;
        if (v.flag & 0x20) v.affondamento = 2;   // un'arma distrutta sparisce presto
        else { uccisione(ultimo_lanciatore_); v.affondamento = 18; }   // difetto: lanciatore dell'ultimo siluro (DS:6503)
        v.integrita = 0;
        c.distrutta = true;
        if (v.id < 2) {
            v.sonar = std::max(0, v.sonar - 2); v.rumore += 2;
            const bool relitto = (v.flag54 & 0x80) != 0;
            v.affondamento = relitto ? 1 : 30;
            if (v.id == locale) { messaggio(v.id, MSG_BOMBA); if (!relitto) messaggio(v.id, MSG_EMERSIONE); }
            else messaggio(v.id, MSG_ALTRO_BOMBA);
        }
    } else {   // sopravvive: +5E resta com'era (IF_HIT non vede i colpi non letali di ATTACK)
        v.integrita = resto;
        if (v.id < 2) {
            if (v.id == locale) { messaggio(v.id, MSG_BOMBA); v.sonar = std::max(0, v.sonar - 2); v.rumore += 2; }
            else messaggio(v.id, MSG_ALTRO_BOMBA);
        }
    }
    colpi_.push_back(c);
}

void Missione::distruggi(int id) {
    auto* v = trova(id);
    if (!v || v->affondamento != 0) return;
    const auto* g = giocatore();
    uccisione(v->lanciatore);
    v->affondamento = 18; v->integrita = 0; v->colpo = 0xD3;
    if (!g || g->id != id) messaggio(id, MSG_A_SEGNO);
}

// --- siluri dell'Enhanced (fase C6b) ---

namespace {
// classe dell'arma nelle missioni -> classe del modello (engine/README.md, "Siluri dell'Enhanced")
const DatiSiluro* dati_siluro_missione(const TabellaSiluri& t, const std::string& classe) {
    if (classe == "type53") return t.siluro("uset80");   // antisom; il 53-65K a scia e' antinave
    return t.siluro(classe);
}
}  // namespace

void Missione::carica_dimensioni_classi(const std::string& cartella_dati) {
    if (!dimensioni_classi_.empty()) return;
    try {
        for (const auto& [id, c] : carica_classi(cartella_dati + "/classi.toml"))
            dimensioni_classi_[id] = {c.lunghezza_m, c.larghezza_m, c.pescaggio_m, c.civile};
    } catch (const std::exception&) {}
}

void Missione::siluri_enhanced(const std::string& cartella_dati) {
    if (siluri_attivi_) return;
    try {
        tabella_siluri_ = carica_siluri(cartella_dati + "/siluri.toml");
    } catch (const std::exception& e) {
        if (errori_.empty() || errori_.back() != e.what()) errori_.push_back(e.what());
        return;
    }
    siluri_attivi_ = true;
    carica_dimensioni_classi(cartella_dati);   // lo scafo per la spoletta
    try {   // i missili del modello (docs/21): senza missili.toml restano quelli dell'originale (gestore e ATTACK_RND)
        catalogo_missili_ = std::make_unique<CatalogoMissili>(carica_missili(cartella_dati + "/missili.toml"));
    } catch (const std::exception& e) {
        if (errori_.empty() || errori_.back() != e.what()) errori_.push_back(e.what());
    }
    try {   // le mine (M2b): senza carico.toml il campo minato non sa posare nulla
        campo_minato_.imposta_dati(carica_mine(cartella_dati + "/carico.toml"));
    } catch (const std::exception& e) {
        if (errori_.empty() || errori_.back() != e.what()) errori_.push_back(e.what());
    }
    for (const auto& x : mine_iniziali_)   // il campo gia' posato della missione (S06, I04)
        if (const int id = campo_minato_.posa(x.tipo, x.pos, 0.0, 0.0, tempo_, -1, x.lato, x.armata, x.sicura); id < 0)
            errori_.push_back("mina sconosciuta nel campo della missione: " + x.tipo);
        else if (x.comandata)
            campo_minato_.imposta_comando(id, x.gruppo);
    mine_iniziali_.clear();
}

int Missione::comanda_mine(const std::string& gruppo) { return campo_minato_.comanda(gruppo); }

bool Missione::posa_mina_ia(int unita, const std::string& tipo, int quante, double ogni_s) {
    if (!siluri_attivi_ || !campo_minato_.dati(tipo) || quante <= 0) return false;
    const UnitaClassica* u = trova(unita);
    if (!u || u->distrutta || u->persa) return false;
    pose_ia_.push_back({unita, tipo, quante, tempo_, std::max(1.0, ogni_s)});
    return true;
}

Siluro* Missione::siluro(int id) {
    const auto it = siluri_.find(id);
    return it == siluri_.end() ? nullptr : &it->second.siluro;
}

const Siluro* Missione::siluro(int id) const {
    const auto it = siluri_.find(id);
    return it == siluri_.end() ? nullptr : &it->second.siluro;
}

bool Missione::comanda_siluro(int id, const ComandoFilo& c) {
    auto* s = siluro(id);
    if (!s) return false;
    s->comanda(c);
    return true;
}

void Missione::variante_tubo(int n, const std::string& siluro) {
    if (siluro.empty()) varianti_tubo_.erase(n);
    else varianti_tubo_[n] = siluro;
}

constexpr double RTE_SOLO_RILEVAMENTO_M = 2000.0;   // lancio dell'IA sul solo rilevamento: la testa si arma presto (STIMA)

// il punto d'incontro di un siluro a `siluro_m_s` partito da `da` con un bersaglio in `punto` che tiene rotta e velocita'
// (la soluzione della TMA): nullopt se non lo raggiunge
std::optional<Vec2> punto_d_intercetto(Vec2 da, Vec2 punto, double rotta_deg, double v_m_s, double siluro_m_s) {
    const Vec2 r{punto.x - da.x, punto.y - da.y};
    const Vec2 v{v_m_s * std::sin(radianti(rotta_deg)), v_m_s * std::cos(radianti(rotta_deg))};
    const double a = v.x * v.x + v.y * v.y - siluro_m_s * siluro_m_s, b = 2.0 * (r.x * v.x + r.y * v.y), c = r.x * r.x + r.y * r.y;
    double t = -1.0;
    if (std::abs(a) < 1e-9) t = b < 0.0 ? -c / b : -1.0;
    else if (const double delta = b * b - 4.0 * a * c; delta >= 0.0) {
        const double t1 = (-b - std::sqrt(delta)) / (2.0 * a), t2 = (-b + std::sqrt(delta)) / (2.0 * a);
        t = t1 > 0.0 && (t2 <= 0.0 || t1 < t2) ? t1 : t2;
    }
    if (t <= 0.0) return std::nullopt;
    return Vec2{punto.x + v.x * t, punto.y + v.y * t};
}

void Missione::nuovo_siluro(UnitaClassica& u, const DatiSiluro* modello) {
    if (!(u.flag & 0x20) || (u.flag & 3) == 3 || (u.flag & 0x80)) return;   // armi, non missili ne' esche
    const auto* l = trova(u.lanciatore);
    const DatiSiluro* d = modello;
    if (!d && l && l->giocatore && variante_lancio_) {   // la variante del carico (ADCAP, SET-65...): una volta sola
        if (!variante_lancio_->empty()) d = tabella_siluri_.siluro(*variante_lancio_);
        variante_lancio_.reset();
    }
    if (!d && l && !l->giocatore && armi_ia_ && !arma_ia_al_lancio(*l, u, d)) {   // fase 4 B: niente in tubo, niente lancio
        u.persa = true;
        return;
    }
    if (!d) d = dati_siluro_missione(tabella_siluri_, u.classe);
    if (!d) return;
    const auto* b = trova(u.inseguito >= 0 ? u.inseguito : u.bersaglio);
    ImpostazioniSiluro imp;
    // il giocatore: i campi del tubo che ha toccato; gli altri come per tutti, dal contatto e dal bersaglio
    std::optional<ImpostazioniSiluro> scelte;
    std::uint32_t campi = ~0u;
    if (!modello && l && l->giocatore && impostazioni_giocatore_) {
        scelte = *impostazioni_giocatore_;
        campi = campi_giocatore_;
        impostazioni_giocatore_.reset();
    }
    {   // come LAUNCH, sul bersaglio memorizzato
        // ...ma con quello che ne sa il lanciatore (docs/18 fase 1, inventario §6): la posizione stimata del suo contatto;
        // col solo rilevamento, quello e una distanza d'armamento tipica, alla quota del lanciatore. La verita' solo se
        // non ha nessun contatto (lo script ha lanciato alla cieca)
        const double fondo = std::min(imp.pavimento_m, d->quota_max_m);
        const Contatto* k = nullptr;
        // il siluro di una CAPTOR nasce dalla mina, che il bersaglio l'ha scoperto da se': non il contatto del posatore
        if (l && b && !da_mina_.count(u.id))
            if (const auto it = l->contatti.find(b->id); it != l->contatti.end()) k = &it->second;
        const bool posizione = b && (!k || k->posizione_nota);
        Vec2 punto = !b ? u.pos : k && k->posizione_nota ? k->pos_stimata : b->pos;
        // l'IA dell'Enhanced con la soluzione della sua TMA mira al punto d'incontro, come il giocatore con la sua
        if (armi_ia_ && l && !l->giocatore && k && k->posizione_nota)
            if (const auto st = stima_tma_ia(l->id, b->id); st && st->osservabile)
                if (const auto x = punto_d_intercetto(u.pos, punto, st->soluzione.rotta, st->soluzione.velocita_m_s, d->velocita_alta_kn * NODO))
                    punto = *x;
        const double quota_b = !b ? u.quota : k && k->posizione_nota ? k->quota_stimata : k ? u.quota : b->quota;
        imp.rotta = !b ? u.rotta : posizione ? rilevamento(u.pos, punto) : normalizza_360(k->rilevamento);
        imp.modo = ModoTesta::misto;
        imp.velocita = VelocitaSiluro::alta;
        if (b) {
            imp.rte_m = posizione ? std::max(500.0, 0.6 * distanza(u.pos, punto)) : RTE_SOLO_RILEVAMENTO_M;
            imp.quota_ricerca_m = std::clamp(quota_b, imp.soffitto_m, fondo);
        }
        imp.quota_corsa_m = std::clamp(u.quota, imp.soffitto_m, fondo);
        // contro una nave l'IA toglie il soffitto, come il giocatore che imposta il tiro antinave: se no la testa (non a
        // scia) ignora il bersaglio in superficie (03/11/2026; prima i siluri dell'IA non agganciavano le navi)
        if (b && b->categoria == "nave" && !(l && l->giocatore)) imp.soffitto_m = 0.0;
        if (l && aereo(*l)) {   // aviolanciato (A3): attivo all'ingresso in acqua, ricerca circolare sul punto
            imp.rte_m = 0.0;
            imp.schema = SchemaRicerca::circolare;
            imp.modo = ModoTesta::attivo;
            if (b) {
                // l'aereo non sa la quota vera: la stima dell'equipaggio (STIMA, sigma 60 m: batitermografo, ping del dip
                // o DICASS senza quota), dal seme dell'arma
                Casuale stima(seme_ ^ ((static_cast<std::uint64_t>(u.id) + 7) * 0x2545F4914F6CDD1DULL));
                const double g = std::sqrt(-2.0 * std::log(1.0 - stima.uniforme())) * std::cos(2.0 * 3.14159265358979 * stima.uniforme());
                imp.quota_ricerca_m = std::clamp(b->quota + QUOTA_STIMATA_SIGMA_M * g, std::max(imp.soffitto_m, 15.0), fondo);
                imp.quota_corsa_m = imp.quota_ricerca_m;   // scende subito alla quota stimata del bersaglio
            }
        }
    }
    if (scelte) {
        const auto c = [&](CampoSiluro k) { return (campi >> static_cast<unsigned>(k)) & 1u; };
        if (c(CampoSiluro::rotta)) imp.rotta = scelte->rotta;
        if (c(CampoSiluro::modo)) imp.modo = scelte->modo;
        if (c(CampoSiluro::velocita)) imp.velocita = scelte->velocita;
        if (c(CampoSiluro::rte)) imp.rte_m = scelte->rte_m;
        if (c(CampoSiluro::quota_corsa)) imp.quota_corsa_m = scelte->quota_corsa_m;
        if (c(CampoSiluro::quota_ricerca)) imp.quota_ricerca_m = scelte->quota_ricerca_m;
        if (c(CampoSiluro::soffitto)) imp.soffitto_m = scelte->soffitto_m;
        if (c(CampoSiluro::pavimento)) imp.pavimento_m = scelte->pavimento_m;
        if (c(CampoSiluro::schema)) imp.schema = scelte->schema;
        if (c(CampoSiluro::anti_circolare)) imp.anti_circolare = scelte->anti_circolare;
        // soffitto o pavimento toccati: le quote ricavate dal motore stanno dentro quelli
        if (c(CampoSiluro::soffitto) || c(CampoSiluro::pavimento)) {
            const double alto = imp.soffitto_m, basso = std::max(alto, std::min(imp.pavimento_m, d->quota_max_m));
            if (!c(CampoSiluro::quota_corsa)) imp.quota_corsa_m = std::clamp(imp.quota_corsa_m, alto, basso);
            if (!c(CampoSiluro::quota_ricerca)) imp.quota_ricerca_m = std::clamp(imp.quota_ricerca_m, alto, basso);
        }
    }
    const std::uint64_t seme = seme_ ^ ((static_cast<std::uint64_t>(u.id) + 1) * 0x9E3779B97F4A7C15ULL);
    siluri_.emplace(u.id, SiluroMissione{Siluro(*d, imp, u.pos, u.quota, u.lanciatore, seme), l ? l->rotta : 0.0});
    lancio_per_statistiche(u.id, l, b ? b->id : -1, d->classe);   // R3 della sandbox
    if (!d->filoguidato) u.collegata = -1;
    else if (armi_ia_ && l && !l->giocatore && l->categoria == "sottomarino") guida_filo_ia(u.id, *d);   // fase 4 B
    // aviolanciato in acqua piu' bassa del suo minimo: nel tuffo d'ingresso urta il fondo e si perde
    if (l && aereo(*l) && fondale_ && profondita(u.pos) < d->fondale_minimo_m) {
        u.persa = true;
        siluri_.erase(u.id);
    }
}

// --- mine dell'Enhanced (M2b) ---

namespace {
constexpr int MSG_MINA_SENZA_POSA = 364, MSG_MINA_POSATA = 367, MSG_MINA_INERTE = 368, MSG_MINA_SCOPPIATA = 369;
}  // namespace

bool Missione::posa_mina(const UnitaClassica& g, int tubo) {
    const auto v = varianti_tubo_.find(tubo);
    const DatiMina* d = v != varianti_tubo_.end() ? campo_minato_.dati(v->second) : nullptr;
    if (!siluri_attivi_ || !d) {
        impostazioni_giocatore_.reset();
        messaggio_di_bordo(g.id, MSG_MINA_SENZA_POSA);
        return false;
    }
    // le mobili corrono lungo la rotta e per la distanza (RTE) impostate per il tubo; senza, sulla rotta del battello
    // per tutta la corsa
    double rotta = g.rotta, corsa = d->corsa_max_m;
    if (impostazioni_giocatore_) {   // solo i campi toccati, come per i siluri
        if (campo_giocatore(CampoSiluro::rotta)) rotta = impostazioni_giocatore_->rotta;
        if (campo_giocatore(CampoSiluro::rte)) corsa = impostazioni_giocatore_->rte_m;
        impostazioni_giocatore_.reset();
    }
    campo_minato_.posa(v->second, g.pos, rotta, corsa, tempo_, g.id, g.flag & 0x18);
    return true;
}

void Missione::passo_mine(double dt) {
    const double dt_m = dt * m_s_per_nodo_ / NODO;   // il moto classico accelera il mondo: la corsa con lui
    std::vector<BersaglioMina> bersagli;
    for (const auto& [id, u] : unita_) {
        if (u.distrutta || u.persa || (u.flag & 0x20)) continue;
        const bool nave = u.categoria == "nave", sub = u.categoria == "sottomarino";
        if (nave || sub) bersagli.push_back({id, u.pos, u.quota, nave, sub});
    }
    const auto fondo = [this](Vec2 p) { return fondale_ ? profondita(p) : 1000.0; };
    // la posa dell'IA: una mina sotto l'unita' ogni `ogni_s`
    for (auto it = pose_ia_.begin(); it != pose_ia_.end();) {
        const UnitaClassica* u = trova(it->unita);
        if (!u || u->distrutta || u->persa || it->restano <= 0) { it = pose_ia_.erase(it); continue; }
        if (tempo_ >= it->prossima) {
            campo_minato_.posa(it->tipo, u->pos, u->rotta, 0.0, tempo_, u->id, u->flag & 0x18);
            --it->restano;
            it->prossima = tempo_ + it->ogni_s;
        }
        ++it;
    }
    const CampoMinato::Esito e = campo_minato_.avanza(tempo_, dt_m, bersagli, fondo);
    const auto* g = giocatore();
    const auto mina = [this](int id) -> const Mina* {
        for (const auto& m : campo_minato_.mine())
            if (m.id == id) return &m;
        return nullptr;
    };
    for (const auto& [lista, inerte] : {std::pair{&e.posate, false}, std::pair{&e.inerti, true}})
        for (const int id : *lista)
            if (const Mina* m = mina(id)) {
                if (m->posatore < 0) continue;   // il campo gia' posato della missione: non e' un evento
                registro_.mina_posata(*this, m->posatore, m->tipo, m->pos, m->quota, inerte);
                if (g && m->posatore == g->id) messaggio_di_bordo(g->id, inerte ? MSG_MINA_INERTE : MSG_MINA_POSATA);
            }
    for (const auto& x : e.inneschi) {
        registro_.mina_esplosa(*this, x.posatore, x.bersaglio, x.tipo, x.pos, x.quota, x.siluro, x.carica_kg);
        if (g) messaggio_di_bordo(g->id, MSG_MINA_SCOPPIATA);
        innesco_mina(x);
    }
}

// Lo scoppio di una mina (a influenza o mina-razzo PMR), o il siluro di una CAPTOR: un'arma creata dalla mina, del suo
// schieramento e con il posatore come lanciatore, cosi' colpi e affondamenti vanno a chi l'ha posata
void Missione::innesco_mina(const InnescoMina& x) {
    auto* v = trova(x.bersaglio);
    if (!v || v->distrutta || v->persa) return;
    UnitaClassica a;
    a.id = prossimo_id_++;
    a.classe = x.tipo;
    a.categoria = "arma";
    a.flag = 0x20 | (x.lato & 0x18);
    a.pos = x.pos;
    a.quota = a.quota_ordinata = x.quota;
    a.rotta = a.rotta_ordinata = x.rotta;
    a.lanciatore = x.posatore;
    a.bersaglio = x.bersaglio;
    a.prossimo_bersaglio = a.id;
    da_mina_.insert(a.id);
    if (!x.siluro) {   // scoppio sul posto, con la regola dei colpi: la carica e la geometria vere alla fisica
        a.difesa = static_cast<int>(std::lround(75.0 * x.carica_kg / 293.0));   // il danno classico in scala col Mk 48
        auto& r = unita_[a.id] = a;
        const auto n = colpi_.size();
        colpisci(r, *v);
        if (colpi_.size() > n) {
            auto& c = colpi_.back();
            c.geometria_vera = true;
            c.distanza_m = x.distanza_m;
            c.dz_m = x.dz_m;
            c.tnt_kg = x.carica_kg;
            c.influenza = true;
        }
        return;
    }
    const DatiSiluro* d = tabella_siluri_.siluro(x.classe_siluro);
    if (!d) return;
    a.velocita = a.velocita_ordinata = d->velocita_alta_kn;
    a.difesa = 40;
    auto& r = unita_[a.id] = a;
    osservatore_ = visitata_ = a.id;   // come nuova_unita_da_buffer: in testa alla lista
    ++create_;
    carica_mina_[a.id] = x.carica_kg;
    nuovo_siluro(r, d);
}

void Missione::passo_siluri(double dt) {
    // il moto classico accelera il mondo di m_s_per_nodo_/NODO: il modello corre con lui (stesse distanze)
    const double dt_m = dt * m_s_per_nodo_ / NODO;
    const DatiEsca* adc = tabella_siluri_.esca("adc_mk2");
    const double livello_esca = adc ? adc->livello_db : dati_adc_mk2().livello_db;
    std::vector<SorgenteAcustica> sorgenti;
    for (const auto& [id, v] : unita_) {
        if (v.distrutta || v.persa || (v.flag & 3) == 3) continue;   // non gli aerei
        const bool esca = (v.flag & 0x80) != 0;                     // falso bersaglio: esca lanciata o biologico
        if ((v.flag & 0x20) && !esca) continue;                     // ne' le altre armi
        SorgenteAcustica s;
        s.id = id; s.pos = v.pos; s.quota = v.quota; s.rotta = v.rotta; s.velocita = v.velocita * NODO;
        s.esca = esca;
        s.nave = v.categoria == "nave";
        // con l'acustica dell'Enhanced lo stesso livello dei sonar (nucleo/acustica.hpp, sulla scala della testa).
        // Senza: STIMA, 130 dB per il rumore 10 dell'originale, +2,5 dB per punto e +0,5 dB per nodo oltre i 5;
        // l'esca lanciata (LAUNCH la fa arma) come l'ADC Mk 2
        const DatiEsca* mobile = esca ? esca_mobile(id) : nullptr;   // MG-74, MOSS: ripetitore d'eco
        if (const auto a = sorgente_acustica(v)) s.livello_db = sorgente_per_siluri(*a).livello_db;
        else s.livello_db = mobile ? mobile->livello_db
                            : esca && v.categoria == "arma" ? livello_esca
                                                            : 105.0 + 2.5 * v.rumore + 0.5 * std::max(0.0, v.velocita - 5.0);
        s.forza_bersaglio_db = mobile ? mobile->eco_db : esca ? 0.0 : s.nave ? 25.0 : 15.0;
        s.lunghezza_m = esca ? 0.0 : s.nave ? 150.0 : 100.0;
        if (const auto d = dimensioni_classi_.find(v.classe); !esca && d != dimensioni_classi_.end() && d->second.lunghezza_m > 0.0) {
            s.lunghezza_m = d->second.lunghezza_m;   // lo scafo vero per la spoletta (e per l'estensione dell'eco)
            s.larghezza_m = d->second.larghezza_m;
            s.pescaggio_m = d->second.pescaggio_m;
        }
        sorgenti.push_back(s);
    }
    for (auto it = siluri_.begin(); it != siluri_.end();) {
        auto* u = trova(it->first);
        if (!u || u->distrutta || u->persa) { it = siluri_.erase(it); continue; }
        Siluro& s = it->second.siluro;
        for (auto& v : sorgenti) v.lanciatore = v.id == u->lanciatore;
        std::optional<Lanciatore> l;
        if (const auto* lu = trova(u->lanciatore); lu && s.filo()) {
            l = Lanciatore{lu->pos, lu->velocita * NODO, std::fabs(normalizza_180(lu->rotta - it->second.rotta_lanciatore)) / dt_m};
            it->second.rotta_lanciatore = lu->rotta;
        }
        if (fondale_) s.imposta_fondale(profondita(s.pos()));
        // sotto la banchisa le chiglie come falsi echi per la testa attiva (missione_ghiaccio.cpp)
        std::vector<SorgenteAcustica> con_chiglie;
        s.avanza(dt_m, echi_chiglie_siluro(s, sorgenti, con_chiglie) ? con_chiglie : sorgenti, termoclini_, l);
        u->pos = s.pos(); u->quota = s.quota(); u->rotta = s.rotta(); u->velocita = s.velocita() / NODO;
        u->rotta_ordinata = u->rotta; u->velocita_ordinata = u->velocita; u->quota_ordinata = u->quota;
        u->rateo_virata = 0.0;
        if (!s.filo()) u->collegata = -1;
        bool via = false;
        for (auto& e : s.eventi()) {
            e.tempo = tempo_;
            eventi_siluri_.push_back({u->id, u->lanciatore, e});
            if (via) continue;
            switch (e.tipo) {
                case EventoSiluro::Tipo::ping: evento_acustico(u->id, 1.0); break;
                case EventoSiluro::Tipo::esplosione: {
                    auto* v = e.sorgente >= 0 ? trova(e.sorgente) : nullptr;
                    if (v && !v->distrutta && !v->persa && !(v->flag & 0x80)) {
                        const auto n = colpi_.size();
                        const double dx = u->pos.x - v->pos.x, dy = u->pos.y - v->pos.y, dz = u->quota - v->quota;
                        colpisci(*u, *v);
                        if (colpi_.size() > n) {   // geometria vera dello scoppio, per la fisica dei danni
                            auto& c = colpi_.back();
                            c.geometria_vera = true;
                            c.distanza_m = std::sqrt(dx * dx + dy * dy + dz * dz);
                            c.dz_m = dz;
                            if (const auto k = carica_mina_.find(u->id); k != carica_mina_.end()) c.tnt_kg = k->second;   // siluro di una mina
                        }
                    } else {
                        togli(*u);   // su un'esca o a vuoto: nessun colpo
                    }
                    via = true;
                    break;
                }
                case EventoSiluro::Tipo::spento:
                case EventoSiluro::Tipo::esaurito: togli(*u); via = true; break;
                default: break;
            }
        }
        if (!via && fondale_ && profondita(u->pos) <= 0.0) {   // sulla costa: perso (§9.3)
            if (const auto* lu = trova(u->lanciatore); lu && lu->giocatore && s.filo()) messaggio(lu->id, MSG_SILURO_PERSO);
            togli(*u);
            via = true;
        }
        if (via) it = siluri_.erase(it);
        else ++it;
    }
}

void Missione::ordina(int id, const OrdineMotore& o) {
    if (auto* u = trova(id)) u->ordine = o;
}

void Missione::controlli_giocatore(UnitaClassica& u) {
    // 2011:0387-0452, a ogni visita di un giocatore (id 0 e 1); i messaggi solo al giocatore locale
    const auto* g = giocatore();
    const bool locale = g && g->id == u.id;
    if (locale) {   // strati termici attraversati dalla visita precedente (DS:1F46)
        if (quota_visita_ >= 0.0)
            for (double s : termoclini_)
                if ((quota_visita_ <= s) != (u.quota <= s)) { messaggio(u.id, MSG_STRATO_TERMICO); break; }
        quota_visita_ = u.quota;
    }
    if (u.quota < QUOTA_100_PIEDI_M) {   // sopra i 100 ft al massimo meta' di avanti tutta, un quarto col periscopio su
        const double limite = u.avanti_tutta / ((u.flag54 & 2) ? 4.0 : 2.0);
        const double v = std::fabs(u.velocita_ordinata - u.velocita) > 1e-9 ? u.velocita_ordinata : u.velocita;
        if (limite < v) {
            u.velocita_ordinata = limite;
            if (locale) messaggio(u.id, MSG_REGOLO_VELOCITA);
        }
    }
    // sotto i 20 ft periscopio e antenna si abbassano da soli; con la dinamica dell'Enhanced il battello oscilla attorno
    // alla quota ordinata, quindi con la tolleranza di 1 m delle altre prove di quota periscopica (il classico: 20 ft)
    const double soglia_alberi = dinamica_attiva_ && dinamica(u.id) ? QUOTA_PERISCOPICA_M + TOLLERANZA_PERISCOPICA_M : QUOTA_PERISCOPICA_M;
    if ((u.flag54 & 6) && u.quota > soglia_alberi) {
        if (u.flag54 & 2) { u.flag54 &= ~2; if (locale) messaggio(u.id, MSG_PERISCOPIO_GIU); }
        if (u.flag54 & 4) { u.flag54 &= ~4; if (locale) messaggio(u.id, MSG_ANTENNA_GIU); }
    }
}

void Missione::visita() {
    // 2011:02F3: un'unita' per passo, a rotazione sulla lista come i sensori, da DS:1F5E (che torna in testa quando
    // nasce un'unita' o esce quella da visitare); il conto +55 scende di una visita (circa 2 N secondi per 18 visite
    // con N unita'); a zero l'IA esce dal gioco, il giocatore resta a galla come relitto se e' in superficie
    const auto l = lista();
    if (l.empty()) return;
    const std::size_t i = posto(l, visitata_);
    visitata_ = i + 1 < l.size() ? l[i + 1] : -1;   // poi chi segue (dopo l'ultima, la testa)
    UnitaClassica& u = unita_.at(l[i]);
    if (u.id < 2 && !u.distrutta && !u.persa) controlli_giocatore(u);
    if (u.affondamento <= 0 || --u.affondamento > 0) return;
    if (u.id >= 2) { u.distrutta = true; annulla_timer(u.id, nullptr); visitata_ = -1; esm_ = 0; return; }   // 003C: la testa
    // il relitto e' dell'originale, dove il colpo seguente lo finisce; con la fisica i colpi non distruggono: perso e' perso
    if (!alla_fisica(u.id) && u.quota < QUOTA_RELITTO_M && !(u.flag54 & 0x80)) { u.flag54 |= 0x80; u.integrita = 1; return; }
    const auto* g = giocatore();
    messaggio(u.id, g && g->id == u.id ? MSG_FINE : MSG_ALTRO_DISTRUTTO);
    u.distrutta = true;
}

// --- comandi del giocatore (docs/11_comandi_e_stazioni.md, fase C1) --------------------------------------------------

bool Missione::comando(const std::string& nome, const std::vector<ParametroComando>& parametri) {
    const auto* g = giocatore();
    const auto it = gestori_giocatore_.find(nome);
    if (!g || it == gestori_giocatore_.end()) return false;
    esegui(g->id, it->second, &parametri);
    return true;
}

int Missione::mese_acustica() const {
    if (mese_ > 0) return mese_;
    if ((data_dal_file_ || data_imposta_) && mese_data_ >= 1 && mese_data_ <= 12) return mese_data_;
    return clima_ ? clima_->mese_predefinito : 10;
}

void Missione::prossimo_punto() {
    // pilota automatico: il giocatore naviga verso il primo punto di rotta con il gestore dell'originale
    // con la navigazione incerta il punto e' dove il giocatore crede di andare: la meta vera e' spostata dall'errore
    const Vec2 e = nav_ ? nav_->errore() : Vec2{};
    in_rotta_ = !punti_di_rotta_.empty() && comando("punto_di_rotta", {punti_di_rotta_.front().x - e.x, punti_di_rotta_.front().y - e.y});
    if (!in_rotta_ && pilota_automatico_) { pilota_automatico_ = false; comando("pilota_automatico_off"); }
}

bool Missione::applica(const Ordine& o) {
    const auto* g = giocatore();
    if (!g) return false;
    switch (o.tipo) {
        case TipoOrdine::telegrafo:
            return o.n == 1 ? comando("telegrafo", {std::string("arresto_rapido")}) : comando("telegrafo", {o.a});
        case TipoOrdine::timone:   // toccare il timone disinserisce il pilota automatico (manuale, r. 2012-2020)
            if (pilota_automatico_) { pilota_automatico_ = in_rotta_ = false; comando("pilota_automatico_off"); }
            return comando("timone", {o.a});
        case TipoOrdine::quota: return comando("quota", {o.a, o.b});
        case TipoOrdine::quota_periscopica:   // codice 37h: 20 ft; con la dinamica dell'Enhanced 5,5 m, come Ctrl+P e il comandante
            return comando("quota", {dinamica_attiva_ && dinamica(g->id) ? QUOTA_PERISCOPICA_DINAMICA_M : 6.096, 0.75});
        case TipoOrdine::quota_150_piedi: return comando("quota", {45.72, 1.0});      // 40h: 150 ft
        case TipoOrdine::emersione_emergenza:   // 41h: emersione rapida, con la soffiata delle casse (missione_scafo.cpp)
            if (const auto* g = giocatore(); g && g->quota > 3.0) {
                scafo_transitorio(*g, "soffiata_emergenza");
                soffiate_giocatore_.push_back(tempo_);   // l'evento emersione_emergenza del giocatore (registro, audio)
            }
            return comando("quota", {0.0, 4.0});
        case TipoOrdine::immersione_emergenza: {   // dossier 16 §4.4: quota di sicurezza e avanti tutta
            const double q = quota_immersione_emergenza();
            if (comando("quota", {q, 4.0}) && comando("telegrafo", {1.0})) return true;
            UnitaClassica* u = trova(g->id);   // senza i gestori dell'originale (rapide, editor, prove): direttamente
            if (!u) return false;
            u->quota_ordinata = std::min(q, u->quota_max);
            u->rateo_quota = std::max(u->rateo_quota, u->rateo_quota_modello);
            u->velocita_ordinata = u->avanti_tutta;
            return true;
        }
        case TipoOrdine::punto_di_rotta:
            if (punti_di_rotta_.size() >= 9) return false;   // al massimo 9 punti (manuale, r. 2033)
            punti_di_rotta_.push_back({o.a, o.b});
            if (pilota_automatico_ && !in_rotta_) prossimo_punto();
            return true;
        case TipoOrdine::cancella_punto:   // l'ultimo punto (CLEAR)
            if (punti_di_rotta_.empty()) return false;
            punti_di_rotta_.pop_back();
            if (pilota_automatico_ && punti_di_rotta_.empty()) prossimo_punto();
            return true;
        case TipoOrdine::pilota_automatico:
            if (o.n != 0) {   // funziona solo con almeno un punto di rotta (manuale, r. 2012)
                if (punti_di_rotta_.empty()) return false;
                pilota_automatico_ = true;
                prossimo_punto();
                return true;
            }
            pilota_automatico_ = in_rotta_ = false;
            return comando("pilota_automatico_off");
        case TipoOrdine::periscopio:   // Enhanced: il posto alza coi suoi secondi, il flag a fine corsa
            if (peri_) return ordine_periscopio(o);
            // prima del primo passo non si sa ancora se ci sara' il posto: vale il classico, e il posto lo rifa'
            if (!peri_tentato_ && tempo_ <= 0.0) ordini_peri_in_attesa_.push_back(o);
            return comando("periscopio", {o.n != 0 ? 1.0 : 0.0});
        case TipoOrdine::scegli_periscopio:
        case TipoOrdine::elevazione_periscopio:
        case TipoOrdine::foto_periscopio:
            if (!peri_ && !peri_tentato_ && tempo_ <= 0.0) {   // prima del primo passo il posto non c'e' ancora: aspetta
                ordini_peri_in_attesa_.push_back(o);
                return true;
            }
            return peri_ && ordine_periscopio(o);
        case TipoOrdine::antenna:
            // sotto la banchisa l'albero non sale (missione_ghiaccio.cpp): l'avviso 489
            if (const auto* g = giocatore(); o.n != 0 && g && sotto_banchisa(g->id)) {
                messaggio_di_bordo(g->id, 489);
                return false;
            }
            return comando("antenna", {o.n != 0 ? 1.0 : 0.0});
        case TipoOrdine::schiera_cala: return !schiera_persa(g->id) && comando("schiera_cala");   // la persa resta persa
        case TipoOrdine::schiera_recupera: return comando("schiera_recupera");
        case TipoOrdine::schiera_taglia: return comando("schiera_persa");
        case TipoOrdine::impulso_sonar:   // 2Dh: ping, evento acustico di 300 passi (nessun gestore)
            // Enhanced: come l'IA, il sonar non ripinga prima che torni l'eco (10 s; la simmetria, l'utente 31/10/2026)
            if (catalogo_)
                for (auto it = ping_unita_.rbegin(); it != ping_unita_.rend() && tempo_ - it->t < INTERVALLO_PING_S; ++it)
                    if (it->unita == g->id) {
                        messaggio_sonar(MSG_PING_PRESTO);
                        return false;
                    }
            evento_acustico(g->id, 300.0 / 9.0);
            impulso_sonar_enhanced(g->id);
            return true;
        case TipoOrdine::designa_bersaglio:
        case TipoOrdine::seleziona_siluro:   // SELEZ. del controllo siluri: il siluro diventa il bersaglio del giocatore
            return comando("designa_bersaglio", {static_cast<double>(o.n)});
        case TipoOrdine::carica_tubo:
            return o.n >= 1 && o.n <= 7 && comando("carica_tubo_" + std::to_string(o.n));
        case TipoOrdine::lancia: {   // 24h-29h: il gestore di lancio del tipo d'arma del tubo
            if (o.n < 1 || o.n > static_cast<int>(tubi_.size())) return false;
            const std::string& famiglia = tubi_[static_cast<std::size_t>(o.n - 1)];
            if (famiglia == "mina") return posa_mina(*g, o.n);   // M2b: il campo minato, nessun gestore dello script
            // esca semovente dal tubo (MOSS Mk 70, MG-74: la variante del carico e' un'[esche.*] di siluri.toml con
            // velocita'): corre con la rotta e la quota impostate del tubo, se no con quelle del battello
            if (const auto v = varianti_tubo_.find(o.n); v != varianti_tubo_.end() && siluri_attivi_)
                if (const DatiEsca* d = tabella_siluri_.esca(v->second); d && d->mobile()) {
                    // solo i campi toccati del tubo, come per i siluri
                    const bool imp = impostazioni_giocatore_.has_value();
                    const double rotta = imp && campo_giocatore(CampoSiluro::rotta) ? impostazioni_giocatore_->rotta : g->rotta;
                    const double quota = imp && campo_giocatore(CampoSiluro::quota_corsa) ? impostazioni_giocatore_->quota_corsa_m : -1.0;
                    impostazioni_giocatore_.reset();
                    if (lancia_esca_mobile(g->id, v->second, rotta, quota) < 0) return false;
                    registro_.lancio_senza_arma(*this, v->second);
                    return true;
                }
            if (famiglia == "attacco_terra" || famiglia == "tlam_c") {   // TLAM-C (carico M2): nessun bersaglio in mare
                // il modello (docs/21 §11.8): la missione della libreria, l'esito con la posizione vera
                if (missile_del_tubo(o.n) && !lancia_tlam(*trova(g->id), o.n)) return false;
                registro_.lancio_senza_arma(*this, "tlam_c");
                // il gestore facoltativo della missione (armi.attacco_terra): il suo MISSILE non conta un secondo
                // lancio, il registro lo trova gia' nell'evento di questo passo
                if (const auto it = gestori_armi_.find(famiglia); it != gestori_armi_.end() && !it->second.empty()) {
                    famiglia_lancio_ = famiglia;
                    esegui(g->id, it->second);
                    famiglia_lancio_.reset();
                }
                return true;
            }
            // Enhanced: un missile del catalogo vola col modello; il gestore gira ancora per i conteggi della missione
            const bool modellato = missile_del_tubo(o.n) != nullptr;
            if (modellato && !lancia_missile_modello(*trova(g->id), o.n)) return false;
            const auto it = gestori_armi_.find(famiglia);
            if (it == gestori_armi_.end() || it->second.empty()) return modellato;
            giudica_lancio_giocatore(*g);
            if (const auto v = varianti_tubo_.find(o.n); v != varianti_tubo_.end()) variante_lancio_ = v->second;
            else variante_lancio_.reset();
            famiglia_lancio_ = famiglia;
            lancio_modellato_ = modellato;
            esegui(g->id, it->second);
            lancio_modellato_ = false;
            famiglia_lancio_.reset();
            variante_lancio_.reset();   // un missile non crea un siluro: la variante non resta per altri
            return true;
        }
        case TipoOrdine::guida_punto: return comando("guida_e2", {o.a, o.b});   // DIRECT
        case TipoOrdine::guida_cerca: return comando("guida_e1");               // SEARCH
        case TipoOrdine::guida_detona: return comando("guida_e0");              // DETONATE (solo livello classico)
        case TipoOrdine::filtro_sonar:
        case TipoOrdine::spegni_siluro:
        case TipoOrdine::taglia_filo:
        case TipoOrdine::compressione_tempo:
        case TipoOrdine::barra:              // Enhanced: li traduce StazioniMissione per il modello dinamico e i siluri
        case TipoOrdine::accosta_per:
        case TipoOrdine::giri:
        case TipoOrdine::quota_con_angolo:
        case TipoOrdine::angolo_immersione:
        case TipoOrdine::imposta_siluro:
        case TipoOrdine::comando_filo:
        case TipoOrdine::radio_filo:         // sala radio dell'Enhanced: li traduce StazioniMissione
        case TipoOrdine::radio_trasmetti:
        case TipoOrdine::radio_ssixs:
        case TipoOrdine::radio_slot:
        case TipoOrdine::radio_verifica:
        case TipoOrdine::silenzio_a_bordo:   // equipaggio dell'Enhanced: lo consuma StazioneArmi (ricarica)
        case TipoOrdine::scegli_arma_tubo:   // Enhanced: lo consuma StazioneArmi (carico M2)
        case TipoOrdine::akkord_designa:     // l'Alfa (dossier 19): Akkord lo consuma la stazione Armi,
        case TipoOrdine::akkord_scegli:
        case TipoOrdine::modo_boksit:        // Boksit StazioniMissione,
        case TipoOrdine::programma_tappa:
        case TipoOrdine::tan:                // le casse StazioniMissione coi danni del sottomarino
        case TipoOrdine::sfoga:
        case TipoOrdine::compensa:
        case TipoOrdine::isola_compartimento:   // il controllo danni: DanniSottomarini::consegna
        case TipoOrdine::priorita_sistema:
            return false;
        case TipoOrdine::bqs15: return bqs15(g->id, o.n != 0);   // Enhanced: il sonar delle mine (missione_bqs15.cpp)
        case TipoOrdine::radar:   // Enhanced: il radar della classe, col battello fuori o l'albero su (docs/18 fase 2)
            if (!catalogo_el_ || catalogo_el_->radar_di(g->classe).empty()) return false;
            if (o.n != 0 && !radar_giocatore_ && (g->quota < 0.5 || (g->flag54 & 4))) messaggio_di_bordo(g->id, MSG_RADAR_ACCESO);
            if (o.n != 0 && !radar_giocatore_) ++conti_.accensioni_radar;   // l'esposizione (docs/26)
            radar_giocatore_ = o.n != 0;
            return true;
        case TipoOrdine::imposta_missile:
        case TipoOrdine::prepara_missile:
            return imposta_missile(o);   // la stazione dei missili (missione_missili.cpp)
        case TipoOrdine::punto_nave:   // Enhanced, navigazione incerta: il punto nave del tipo dato
            return avvia_punto_nave(o.testo);
        case TipoOrdine::ecoscandaglio_ping:
        case TipoOrdine::ecoscandaglio_periodo:
        case TipoOrdine::ecoscandaglio_potenza:
            return ordine_ecoscandaglio(o);   // Enhanced: il quadro del fondale (missione_fondale.cpp)
        case TipoOrdine::luci_giocatore:
            return ordine_luci(o);   // Enhanced: le luci di navigazione (missione_luci.cpp)
        case TipoOrdine::identifica_visiva:
        case TipoOrdine::osserva_aob:
        case TipoOrdine::misura_telemetro:
            return ordine_libretto(o);   // Enhanced: il libretto al periscopio (missione_libretto.cpp)
        case TipoOrdine::classifica_contatto:   // registro M1: la classe data dal giocatore
            if (o.testo.empty()) return false;
            classifica_contatto(g->id, o.n, o.testo);
            return true;
    }
    return false;
}

void Missione::avanza(double dt) {
    const double fine = tempo_ + dt;
    while (tempo_ < fine - 1e-9) {
        const double h = std::min(PASSO_S, fine - tempo_);
        conto_5100_ = conto_5100_ <= 1 ? 18 : conto_5100_ - 1;   // DS:5100 (2011:01E1)
        navigazione(h);
        tempo_ += h;
        if (!interventi_.empty()) passo_interventi();   // il vetrino di Dio: prima di tutto il resto (missione_interventi.cpp)
        if (catalogo_) passo_schiere(h);   // la schiera trainata dei sottomarini (docs/20 §7)
        if (catalogo_) passo_traini(h);    // il VDS e la schiera delle navi dell'IA (missione_vds.cpp)
        if (clima_) passo_fondo(h);
    if (catalogo_) passo_quadro_fondale();         // i rischi di chi e' posato sul fondo (roadmap 30)
    if (catalogo_) passo_fondale_ia();             // l'IA che sonda il fondo (missione_fondale_ia.cpp)
        if (!siluri_.empty()) passo_siluri(h);
        if (!missili_.empty() || !tiri_.empty() || !lanci_tlam_.empty()) passo_missili(h);   // i missili del modello, la preparazione, i TLAM
        if (traffico_attivo_ && !traffico_vietato_ && catalogo_) passo_traffico(h);   // il traffico civile (docs/20 §3)
        if (!campo_minato_.vuoto() || !pose_ia_.empty()) passo_mine(h);   // anche la prima posa dell'IA
        if (!avarie_.empty() || !rimorchi_.empty() || !relitti_.empty()) passo_avarie(h);   // M6: avarie, incagli e rimorchi dell'IA
        if (catalogo_)   // scheda I09: i sonar speciali che pingano da soli
            for (auto& [sid, s] : sonar_speciale_)
                if (s.ogni_s > 0.0 && tempo_ >= s.prossimo) {
                    s.prossimo = tempo_ + s.ogni_s;
                    if (const auto* u = trova(sid); u && !u->distrutta && !u->persa) {
                        evento_acustico(sid, 300.0 / 9.0);
                        impulso_sonar_enhanced(sid);
                    }
                }
        if (!esche_mobili_.empty()) passo_esche_mobili();   // MG-74 e MOSS: manovra programmata e batteria
        if (catalogo_) {
            passo_posti();                  // i posti di combattimento del giocatore (prima che i lanci nuovi siano visti)
            passo_transitori_scafo(h);      // portelli, pompe, TDU, scricchiolii... (missione_scafo.cpp)
            ascolta_transitori_scafo();
        }
        if (catalogo_) passo_transitori_armi();   // R7: lanci, scoppi, affondamenti e ping nella scena acustica
        visita();
        // 1000:2460: con periscopio e antenna del giocatore entrambi giu' il livello ESM torna a 0
        if (const auto* g = giocatore(); g && !(g->flag54 & 6)) esm_ = 0;
        // pilota automatico: arrivato al punto (la navigazione del giocatore si e' fermata), il prossimo
        if (pilota_automatico_ && in_rotta_)
            if (const auto* g = giocatore(); g && !g->navigazione) {
                if (!punti_di_rotta_.empty()) punti_di_rotta_.erase(punti_di_rotta_.begin());
                prossimo_punto();
            }
        // timer scaduti, in ordine di tempo e di inserimento
        for (;;) {
            auto it = std::min_element(timer_.begin(), timer_.end(), [](const Timer& a, const Timer& b) {
                return a.tempo != b.tempo ? a.tempo < b.tempo : a.ordine < b.ordine;
            });
            if (it == timer_.end() || it->tempo > tempo_) break;
            const Timer t = *it;
            timer_.erase(it);
            timer_da_missile_ = t.da_missile;
            esegui(t.unita, t.gestore);
            timer_da_missile_ = false;
        }
        if (!impatti_originale_) impatti();
        if (meteo_) passo_meteo();   // prima dei sensori: il mare e la pioggia del secondo
        if (clima_) passo_pomeriggio();   // l'effetto pomeriggio nel profilo (dossier 02 §3.5)
        passo_sensori();
        if (catalogo_ && esperienza_sonarista_) passo_sonarista();
        if (catalogo_ && !sonaristi_ia_spenti_) passo_sonaristi_ia();   // fase 2: i sonaristi dell'IA
        if (catalogo_) { avvisi_sonar(); passo_reti(); passo_paesaggio(h); }
        if (radio_) passo_radio(h);
        if (peri_) passo_periscopi(h);   // prima dell'elettronica: la testa del passo
        if (peri_) passo_libretto_ia();
        if (dinamica_attiva_) passo_conti(h);   // docs/26: sotto la chiglia, il mare, i campi di mine (solo l'Enhanced)   // l'identificazione visiva dell'IA sui contatti «vista» (docs/25 §3.5)
        if (catalogo_el_) passo_elettronica(h);
        if (catalogo_int_) passo_intercettazione(h);   // gli impulsi del passo agli intercettatori (blocco 2, E)
        if (collisioni_) passo_collisioni(h);    // urti fra unita', quasi collisioni, Crazy Ivan (dossier 16)
        if (batterie_attive_) passo_batterie(h);  // batteria e snorkel dei diesel dell'IA (M6)
        if (!bqs15_.empty()) passo_bqs15();       // il sonar delle mine del 688 (S06)
        if (catalogo_) passo_ghiaccio(h);         // sotto il ghiaccio: la volta, l'IA sotto le chiglie, l'urto (missione_ghiaccio.cpp)
        if (artiglieria_) passo_artiglieria();   // navi che sparano al giocatore esposto (docs/16, G2)
        if (asw_) {
            passo_asw(h);
            passo_evasione();
            passo_elicotteri_navi();   // la base e la fine del volo, gli elicotteri imbarcati (missione_antisom_navi.cpp)
        }
        if (nav_) passo_navigazione(h);   // la navigazione incerta del giocatore
        if (armi_ia_) passo_armi_ia();   // fase 4 B: tubi, gambe di TMA, guida a filo dell'IA
        if (armi_ia_ && catalogo_missili_) passo_missili_ia();   // il tiro antinave delle navi dell'IA (I11)
        if (forze_ && !difesa_spenta_) passo_difesa();
        if (forze_ && armi_ia_ && !armi_ia_->originale && !reazioni_spente_) {
            passo_reazioni();   // fase 4 C
            passo_pace();       // fase 4 D
        }   // fase 4 A: evasione, esche, ritirate e anticollisione dell'IA
        if (forze_ && !pedine_vicine_.empty()) passo_pedina_vicino(h);   // il pedinamento ravvicinato (S24)
        if (!interventi_.empty()) tieni_ordini_interventi();   // gli ordini del vetrino di Dio scavalcano la dottrina
        passo_storia_contatti(h);
        registro_.osserva(*this, h);   // prima che le unita' uscite si tolgano
        if (forze_) passo_forze();   // fase 3: allerta e provocazioni dagli eventi del registro
        if (statistiche_) passo_statistiche();   // sandbox V2: R1 e la radio di R5
        std::erase_if(unita_, [this](const auto& kv) {
            const bool via = kv.second.distrutta || kv.second.persa;
            if (via) raggi_.dimentica(kv.first);   // i campi dei raggi della sorgente uscita
            return via;
        });
    }
}

void Missione::sincronizza(Mondo& mondo) const {
    auto& navi = mondo.navi();
    std::erase_if(navi, [this](const Nave& n) {
        const auto it = unita_.find(n.id);
        return it == unita_.end() || it->second.distrutta || it->second.persa;
    });
    for (const auto& [id, u] : unita_) {
        if (u.distrutta || u.persa) continue;
        auto it = std::find_if(navi.begin(), navi.end(), [id = id](const Nave& n) { return n.id == id; });
        if (it == navi.end()) {
            Nave n;
            n.id = id; n.classe = u.classe; n.pos = n.pos_prec = u.pos; n.rotta = n.rotta_prec = u.rotta;
            n.quota = aereo(u) ? -quota_aerea(u) : u.quota; n.alberi_alzati = (u.flag54 & 6) != 0 || testa_ia_fuori(id);
            mondo.aggiungi(n);
            continue;
        }
        it->pos_prec = it->pos; it->rotta_prec = it->rotta;
        it->classe = u.classe; it->pos = u.pos; it->rotta = u.rotta;
        it->quota = aereo(u) ? -quota_aerea(u) : u.quota;   // aerei: quota negativa, in volo a -quota metri
        it->alberi_alzati = (u.flag54 & 6) != 0 || testa_ia_fuori(id);   // al periscopio solo gli alberi (E2); le guardate dell'IA
        it->velocita = u.velocita * NODO; it->rotta_ordinata = u.rotta_ordinata; it->velocita_ordinata = u.velocita_ordinata * NODO;
    }
}

}  // namespace m688
