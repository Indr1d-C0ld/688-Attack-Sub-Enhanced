#include "missioni/registro_missione.hpp"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <filesystem>

#include "missioni/danni_missione.hpp"
#include "missioni/danni_sottomarini.hpp"
#include "missioni/missione.hpp"

extern "C" {
#include "lauxlib.h"
#include "lua.h"
}

namespace m688 {

namespace {

constexpr double MIGLIO_M = 1852.0;
constexpr double AVVISTATO_DI_NUOVO_S = 600.0;   // contro-scoperta nuova dopo 10 minuti senza la posizione
constexpr double BOA_VICINA_M = 2.0 * MIGLIO_M;
constexpr double DURATA_CATEGORIA = 800.0, DURATA_CLASSE = 1400.0;   // come la carta e la barra del bersaglio
constexpr double COLLASSO_SU_COLLAUDO = 1.5;     // STIMA senza il modello dei danni: collasso = 1,5 x quota massima
// messaggi della missione che il registro legge
constexpr int MSG_URTO_FATALE = 9, MSG_FONDO_DANNI = 30;
constexpr int MSG_CI_PINGANO = 312, MSG_CAVITIAMO = 313, MSG_ESM_RISCHIO = 336, MSG_ESM_ILLUMINATI = 337;
constexpr double EPISODIO_PING_S = 120.0;   // un ping dopo tanto silenzio apre un episodio nuovo (STIMA)
// ASW aereo (missioni/asw_missione.hpp)
constexpr int MSG_ASW_SORVOLO = 382, MSG_ASW_DIP = 383, MSG_ASW_MARCATORE = 384, MSG_ASW_SUS = 385, MSG_ASW_BOA_ATTIVA = 386;

constexpr double ARTIGLIERIA_VICINA_M = 50.0;   // il primo colpo vicino: un momento da minicartolina
constexpr int CODICE_VISIVA = 2;                // la classificazione visiva del giocatore (docs/25), come 411 per il sonarista

const char* const NOMI_EVENTO[] = {
    "inizio", "fine", "scoperta", "classificazione", "lancio_giocatore", "lancio_nemico", "colpo", "affondamento",
    "emersione_emergenza", "siluro_evitato", "esca_sedotta", "danno_subito", "danno_domato", "scram",
    "camera_di_lancio_persa", "messaggio_ricevuto", "messaggio_perso", "compito_scaduto", "sessione_mancata",
    "ordine_ricevuto", "trasmissione", "infrazione_roe", "ingresso_zona", "fix_radiogoniometria", "avvistato",
    "esm_rischio_alto", "ping_attivo", "ping_subito", "transitorio_proprio", "boa_vicina", "incaglio", "fuoco_amico",
    "mina_posata", "mina_esplosa", "aereo_in_sorvolo", "ping_sonar_immersione", "marcatore_avvistato",
    "attacco_simulato", "foto_periscopio", "esposizione_lunga", "scambio_tracce", "artiglieria_salva",
    "artiglieria_colpo", "artiglieria_vicino", "collisione", "quasi_collisione", "emersione_forzata",
    "cavo_impigliato", "cavo_toccato", "mina_scoperta", "avaria_ia", "rimorchio", "acque_territoriali", "colpo_esercitazione", "lancio_ia", "scoperta_ia", "ingaggio_ia", "evasione_ia",
    "artiglieria_ia", "collisione_ia", "classificazione_ia", "allerta_forza", "messaggio_ia", "provocazione", "fuoco_amico_ia",
    "esca_ia", "ritirata_ia", "tma_ia", "reazione_ia", "incsea", "punto_nave", "comandante_ia", "esito_tlam",
    "fine_missile", "misura_telemetro", "osservazione_aob", "ping_ecoscandaglio", "allarme_siluro", "attraversamento_mine",
    "verifica_radio", "luci", "punto_nave_ia", "sonar_traino", "intervento", "posti_di_combattimento",
};
static_assert(sizeof(NOMI_EVENTO) / sizeof(NOMI_EVENTO[0]) == static_cast<std::size_t>(TipoEvento::numero));

// --- lettura degli obiettivi (Lua) ---

std::string stringa(lua_State* L, int t, const char* k, const std::string& def = {}) {
    lua_getfield(L, t, k);
    std::string s = lua_type(L, -1) == LUA_TSTRING ? lua_tostring(L, -1) : def;
    lua_pop(L, 1);
    return s;
}

std::optional<double> numero(lua_State* L, int t, const char* k) {
    lua_getfield(L, t, k);
    std::optional<double> v;
    if (lua_isnumber(L, -1)) v = lua_tonumber(L, -1);
    lua_pop(L, 1);
    return v;
}

std::optional<bool> booleano(lua_State* L, int t, const char* k) {
    lua_getfield(L, t, k);
    std::optional<bool> v;
    if (lua_isboolean(L, -1)) v = lua_toboolean(L, -1) != 0;
    lua_pop(L, 1);
    return v;
}

std::vector<int> interi(lua_State* L, int t, const char* k) {
    std::vector<int> v;
    lua_getfield(L, t, k);
    if (lua_isnumber(L, -1)) v.push_back(static_cast<int>(lua_tointeger(L, -1)));
    else if (lua_istable(L, -1))
        for (lua_Integer i = 1; lua_rawgeti(L, -1, i) == LUA_TNUMBER; ++i) { v.push_back(static_cast<int>(lua_tointeger(L, -1))); lua_pop(L, 1); }
    if (lua_istable(L, -1)) lua_pop(L, 1);   // il valore che ha chiuso la lista
    lua_pop(L, 1);
    return v;
}

std::vector<std::string> stringhe(lua_State* L, int t, const char* k) {
    std::vector<std::string> v;
    lua_getfield(L, t, k);
    if (lua_type(L, -1) == LUA_TSTRING) v.push_back(lua_tostring(L, -1));
    else if (lua_istable(L, -1))
        for (lua_Integer i = 1; lua_rawgeti(L, -1, i) == LUA_TSTRING; ++i) { v.push_back(lua_tostring(L, -1)); lua_pop(L, 1); }
    if (lua_istable(L, -1)) lua_pop(L, 1);
    lua_pop(L, 1);
    return v;
}

std::optional<Obiettivo::Tipo> tipo_obiettivo(const std::string& s) {
    using T = Obiettivo::Tipo;
    if (s == "affondare") return T::affondare;
    if (s == "proteggere") return T::proteggere;
    if (s == "raggiungere") return T::raggiungere;
    if (s == "scoprire") return T::scoprire;
    if (s == "non_essere_scoperto") return T::non_essere_scoperto;
    if (s == "sopravvivere") return T::sopravvivere;
    if (s == "variabile") return T::variabile;
    if (s == "esito") return T::esito;
    if (s == "minare") return T::minare;
    if (s == "contatto") return T::contatto;
    if (s == "foto") return T::foto;
    if (s == "zona_inviolata") return T::zona_inviolata;
    if (s == "colpire") return T::colpire;
    if (s == "tlam") return T::tlam;
    if (s == "scatola") return T::scatola;
    if (s == "identificare") return T::identificare;
    if (s == "telemetro") return T::telemetro;
    if (s == "rotta_stimata") return T::rotta_stimata;
    return std::nullopt;
}

std::optional<EsitoDettagliato::Categoria> categoria_esito(const std::string& s) {
    using C = EsitoDettagliato::Categoria;
    for (int i = 0; i <= static_cast<int>(C::fuoco_amico); ++i)
        if (s == nome_categoria_esito(static_cast<C>(i))) return static_cast<C>(i);
    return std::nullopt;
}

// --- testi ---

// nome leggibile di una classe: "koni" -> "Koni", "los_angeles" -> "Los Angeles", le armi note per nome
std::string nome_classe(const std::string& c) {
    if (c.empty()) return "unità sconosciuta";
    if (c == "mk48") return "Mk 48";
    if (c == "mk67") return "Mk 67";
    if (c == "mk60") return "Mk 60 CAPTOR";
    if (c == "pmr1") return "PMR-1";
    if (c == "pmr2") return "PMR-2";
    if (c == "mdt") return "MDT";
    if (c == "type53") return "siluro 53 cm";
    if (c == "uset80") return "USET-80";
    if (c == "harpoon") return "Harpoon";
    if (c == "tomahawk") return "Tomahawk";
    if (c == "tlam_c") return "Tomahawk d'attacco a terra";
    if (c == "subroc") return "SUBROC";
    if (c == "esca") return "esca";
    std::string s = c;
    bool inizio = true;
    for (auto& ch : s) {
        if (ch == '_') { ch = ' '; inizio = true; continue; }
        if (inizio && ch >= 'a' && ch <= 'z') ch = static_cast<char>(ch - 'a' + 'A');
        inizio = false;
    }
    return s;
}

std::string nome_en(const std::string& c);

// l'arma come complemento: "un missile", "una carica", altrimenti il nome ("Mk 48")
bool e_mina(const std::string& c) {
    return c == "mk67" || c == "mk60" || c == "pmr1" || c == "pmr2" || c == "mdt";
}

std::string arma_it(const std::string& c) {
    if (e_mina(c)) return "una mina " + nome_classe(c);
    if (c == "missile") return "un missile";
    if (c == "carica") return "una carica di profondità";
    if (c == "esca") return "un'esca";
    if (c.empty()) return "un'arma";
    return "un " + nome_classe(c);
}

std::string arma_en(const std::string& c) {
    if (e_mina(c)) return "a " + nome_classe(c) + " mine";
    if (c == "carica") return "a depth charge";
    if (c.empty()) return "a weapon";
    const std::string n = c == "missile" || c == "esca" || c == "type53" || c == "tlam_c" ? nome_en(c) : nome_classe(c);
    return (n[0] == 'a' || n[0] == 'e' || n[0] == 'i' || n[0] == 'o' || n[0] == 'u' ? "an " : "a ") + n;
}

std::string nome_en(const std::string& c) {
    if (c.empty()) return "unknown unit";
    if (c == "type53") return "53 cm torpedo";
    if (c == "esca") return "decoy";
    if (c == "tlam_c") return "land-attack Tomahawk";
    if (c == "missile") return "missile";
    return nome_classe(c);
}

std::string miglia(double m) {
    char b[32];
    std::snprintf(b, sizeof b, "%.1f", m / MIGLIO_M);
    return b;
}

double lunghezza(const std::string& categoria) {
    if (categoria == "nave") return 130.0;
    if (categoria == "sottomarino") return 100.0;
    if (categoria == "aereo") return 35.0;
    return 6.0;
}

const char* nome_tipo_battello(EventoBattello::Tipo t) {
    using T = EventoBattello::Tipo;
    switch (t) {
        case T::allagamento: return "flooding";
        case T::paratia_ceduta: return "bulkhead failure";
        case T::esaurimento: return "emergency blow";
        case T::aria_finita: return "HP air exhausted";
        case T::collasso: return "hull collapse";
        case T::implosione: return "implosion";
        case T::scossa: return "shock";
        case T::guasto: return "system failure";
        case T::scram: return "reactor scram";
        case T::riparazione: return "repair";
        case T::falla_tamponata: return "leak plugged";
        case T::incendio: return "fire";
        case T::esplosione: return "internal explosion";
        case T::incendio_spento: return "fire out";
        case T::cloro: return "chlorine gas";
        case T::aria_nell_incendio: return "air feeding the fire";
        case T::cassa_squarciata: return "ballast tank ruptured";
        case T::sgombero: return "compartment evacuated";
        case T::isolamento: return "compartment isolated";
        case T::riapertura: return "compartment reopened";
        case T::caduti: return "men lost in an isolated compartment";
    }
    return "damage";
}

// gravita' di un rapporto dei danni (0..1); < 0: non e' un danno
double gravita(const EventoBattello& e) {
    using T = EventoBattello::Tipo;
    switch (e.tipo) {
        case T::collasso: case T::implosione: return 1.0;
        case T::esplosione: return 0.9;
        case T::paratia_ceduta: return 0.8;
        case T::allagamento: return 0.6;
        case T::incendio: case T::aria_nell_incendio: return 0.6;
        case T::cloro: return 0.5;
        case T::scossa: return std::clamp(e.intensita, 0.1, 0.7);
        case T::guasto: return 0.3;
        case T::aria_finita: return 0.4;
        case T::cassa_squarciata: return 0.5;   // l'aria della cassa che non tiene: il battello pesa (kit)
        case T::caduti: return 0.8;
        default: return -1.0;
    }
}

}  // namespace

const char* nome_evento(TipoEvento t) {
    const auto i = static_cast<std::size_t>(t);
    return i < static_cast<std::size_t>(TipoEvento::numero) ? NOMI_EVENTO[i] : "?";
}

const char* nome_categoria_esito(EsitoDettagliato::Categoria c) {
    using C = EsitoDettagliato::Categoria;
    switch (c) {
        case C::in_corso: return "in_corso";
        case C::successo: return "successo";
        case C::successo_parziale: return "successo_parziale";
        case C::fallimento: return "fallimento";
        case C::battello_perso: return "battello_perso";
        case C::richiamato: return "richiamato";
        case C::fuoco_amico: return "fuoco_amico";
    }
    return "in_corso";
}

long passi_per_evento(const EventoMissione& e, double passo) {
    return passo > 0.0 ? std::lround(e.tempo / passo) + 1 : 0;
}

// Interesse (progetto nelle note, progetto-registro-m1.md §3): base per tipo e modificatori; STIMA da tarare
int interesse(const EventoMissione& e, bool primo) {
    const bool sub = e.categoria == "sottomarino";
    const bool maggiore = e.classe == "kirov" || e.classe == "kiev" || e.classe == "slava" || e.classe == "kara" ||
                          e.classe == "nimitz" || e.classe == "ticonderoga";
    int i = 0;
    switch (e.tipo) {
        case TipoEvento::inizio: i = 10; break;
        case TipoEvento::fine: i = 100; break;
        case TipoEvento::scoperta: i = (sub ? 55 : 30) + (primo ? 15 : 0) + (e.distanza_m > 0 && e.distanza_m < 5 * MIGLIO_M ? 10 : 0); break;
        case TipoEvento::classificazione: i = (sub ? 45 : 25) + (e.livello >= 2 ? 5 : 0) + (e.giusta ? 0 : 20); break;
        case TipoEvento::lancio_giocatore: i = primo ? 60 : 50; break;
        case TipoEvento::intervento: i = e.livello == 1 ? 70 : 40; break;   // il vetrino di Dio: lo spettatore lo vuole trovare
        case TipoEvento::posti_di_combattimento: i = e.livello ? 65 : 20; break;
        case TipoEvento::lancio_nemico: i = e.altra == e.giocatore ? 70 : 40; break;
        case TipoEvento::colpo: i = e.unita == e.giocatore ? 85 : e.altra == e.giocatore ? 75 : e.valore > 0.0 ? 45 : 20; break;
        case TipoEvento::affondamento: i = (sub || maggiore ? 90 : 70) + (e.altra == e.giocatore ? 10 : 0); break;
        case TipoEvento::emersione_emergenza: i = 85; break;
        case TipoEvento::siluro_evitato: i = 80 + (e.livello > 0 ? 10 : 0); break;
        case TipoEvento::esca_sedotta: i = 70; break;
        case TipoEvento::danno_subito: i = e.valore < 0.35 ? 20 : static_cast<int>(40.0 + 50.0 * e.valore); break;
        case TipoEvento::danno_domato: i = 45; break;
        case TipoEvento::scram: i = 70; break;
        case TipoEvento::camera_di_lancio_persa: i = 75; break;
        case TipoEvento::messaggio_ricevuto: i = e.valore > 0 ? 50 : e.livello == 0 ? 35 : 15; break;
        case TipoEvento::messaggio_perso: i = e.valore > 0 ? 45 : 10; break;
        case TipoEvento::compito_scaduto: i = 40; break;
        case TipoEvento::sessione_mancata: i = 30; break;
        case TipoEvento::ordine_ricevuto: i = 55; break;
        case TipoEvento::trasmissione: i = 20; break;
        case TipoEvento::infrazione_roe: i = 75 + (e.livello > 0 ? 15 : 0); break;
        case TipoEvento::ingresso_zona: i = 50; break;
        case TipoEvento::fix_radiogoniometria: i = 55 + (e.valore > 0 ? 10 : 0); break;
        case TipoEvento::avvistato: i = 60 + (primo ? 15 : 0); break;
        case TipoEvento::esm_rischio_alto: i = 45 + (e.livello > 0 ? 10 : 0); break;
        case TipoEvento::ping_attivo: i = 20; break;
        case TipoEvento::ping_subito: i = 35 + (primo ? 10 : 0); break;
        case TipoEvento::transitorio_proprio: i = 25; break;
        case TipoEvento::boa_vicina: i = 50; break;
        case TipoEvento::incaglio: i = 50; break;
        case TipoEvento::fuoco_amico: i = 90; break;
        case TipoEvento::mina_posata: i = e.livello > 0 ? 35 : primo ? 50 : 40; break;
        case TipoEvento::mina_esplosa: i = 75 + (e.categoria == "sottomarino" ? 10 : 0); break;
        case TipoEvento::aereo_in_sorvolo: i = 45 + (primo ? 10 : 0); break;
        case TipoEvento::ping_sonar_immersione: i = 50 + (primo ? 10 : 0); break;
        case TipoEvento::marcatore_avvistato: i = 60; break;
        case TipoEvento::attacco_simulato: i = 70; break;
        case TipoEvento::foto_periscopio: i = 40 + (e.categoria == "sottomarino" ? 20 : e.unita >= 0 ? 10 : 0); break;
        case TipoEvento::esposizione_lunga: i = 35 + (e.valore > 60.0 ? 15 : 0); break;
        case TipoEvento::scambio_tracce: i = 30; break;
        case TipoEvento::artiglieria_salva: i = (e.codice ? 50 : 65) + (primo ? 10 : 0); break;
        case TipoEvento::artiglieria_colpo: i = 80 + (e.valore >= 100.0 ? 10 : 0); break;
        case TipoEvento::artiglieria_vicino: i = 70; break;
        case TipoEvento::collisione: i = 85 + (e.valore > 10.0 ? 10 : 0); break;
        case TipoEvento::quasi_collisione: i = 55 + (e.valore < 100.0 ? 20 : 0) + (e.codice ? 0 : 10); break;
        case TipoEvento::emersione_forzata: i = 70; break;
        case TipoEvento::cavo_impigliato: i = 90; break;
        case TipoEvento::cavo_toccato: i = 60; break;
        case TipoEvento::mina_scoperta: i = primo ? 65 : 45; break;
        case TipoEvento::avaria_ia: i = e.noto ? 60 : 40; break;
        case TipoEvento::rimorchio: i = (e.livello ? 40 : 55) + (e.noto ? 10 : 0); break;
        case TipoEvento::acque_territoriali: i = e.livello ? 40 : 70; break;
        case TipoEvento::colpo_esercitazione: i = (e.unita == e.giocatore || e.altra == e.giocatore) ? 70 : 40; break;
        case TipoEvento::lancio_ia: i = 45 + (e.noto ? 15 : 0); break;
        case TipoEvento::scoperta_ia: i = 20; break;
        case TipoEvento::ingaggio_ia: i = 35; break;
        case TipoEvento::evasione_ia: i = 30; break;
        case TipoEvento::artiglieria_ia: i = 40 + (e.noto ? 15 : 0); break;
        case TipoEvento::collisione_ia: i = 50 + (e.noto ? 15 : 0); break;
        case TipoEvento::classificazione_ia: i = e.giusta ? 20 : 30; break;
        case TipoEvento::allerta_forza: i = e.unita < 0 ? 25 + 10 * e.livello : 15 + 5 * e.livello; break;
        case TipoEvento::messaggio_ia: i = 15; break;
        case TipoEvento::provocazione: i = 75; break;
        case TipoEvento::fuoco_amico_ia: i = 80; break;
        case TipoEvento::esca_ia: i = 35 + (e.noto ? 15 : 0); break;
        case TipoEvento::tma_ia: i = e.codice == 1 ? 35 : 15; break;
        case TipoEvento::reazione_ia: i = 20 + 10 * e.codice + (e.noto ? 10 : 0); break;
        case TipoEvento::incsea: i = 45 + (e.noto ? 10 : 0); break;
        case TipoEvento::punto_nave: i = e.giusta ? (e.valore > 1852.0 ? 45 : 25) : 15; break;
        case TipoEvento::comandante_ia: i = e.classe_arma == "attacco" || e.classe_arma == "difesa" ? 40 : 15; break;
        case TipoEvento::esito_tlam: i = e.giusta ? 70 : 55; break;   // l'attacco a terra della missione
        case TipoEvento::fine_missile: i = 40; break;
        case TipoEvento::misura_telemetro: i = 25; break;
        case TipoEvento::osservazione_aob: i = 15; break;
        case TipoEvento::ping_ecoscandaglio: i = 5; break;
        case TipoEvento::allarme_siluro: i = 70 + 10 * e.codice; break;
        case TipoEvento::attraversamento_mine: i = 60; break;
        case TipoEvento::verifica_radio: i = e.giusta ? 10 : 55; break;
        case TipoEvento::luci: i = 20; break;
        case TipoEvento::punto_nave_ia: i = 10; break;
        case TipoEvento::sonar_traino: i = 10; break;
        case TipoEvento::ritirata_ia: i = (e.classe_arma == "colpito" ? 40 : 25) + (e.noto ? 10 : 0); break;
        case TipoEvento::numero: break;
    }
    return std::clamp(i, 0, 100);
}

// --- caricamento ---

void RegistroMissione::carica(lua_State* L, int t, const std::string& file_missione, const std::string& id) {
    const int base = lua_gettop(L);
    int sorgente = t;
    lua_getfield(L, t, "obiettivi");
    const bool nella_missione = lua_istable(L, -1);
    lua_pop(L, 1);
    if (!nella_missione) {   // le 22 originali: data/missioni/obiettivi/<ID>.lua
        const auto f = std::filesystem::path(file_missione).parent_path() / "obiettivi" / (id + ".lua");
        if (!std::filesystem::exists(f) || luaL_loadfilex(L, f.string().c_str(), "t") != LUA_OK || lua_pcall(L, 0, 1, 0) != LUA_OK ||
            !lua_istable(L, -1)) {
            lua_settop(L, base);
            return;
        }
        sorgente = lua_gettop(L);
    }
    lua_getfield(L, sorgente, "categorie_esiti");
    if (lua_istable(L, -1))
        for (lua_pushnil(L); lua_next(L, -2); lua_pop(L, 1))
            if (lua_isinteger(L, -2) && lua_type(L, -1) == LUA_TSTRING) categorie_[static_cast<int>(lua_tointeger(L, -2))] = lua_tostring(L, -1);
    lua_pop(L, 1);
    lua_getfield(L, sorgente, "obiettivi");
    if (lua_istable(L, -1)) {
        obiettivi_dichiarati_ = true;
        const int lt = lua_gettop(L);
        for (lua_Integer i = 1; lua_rawgeti(L, lt, i) == LUA_TTABLE; ++i) {
            const int o = lua_gettop(L);
            DefObiettivo d;
            const auto tipo = tipo_obiettivo(stringa(L, o, "tipo"));
            if (!tipo) { lua_pop(L, 1); continue; }
            d.o.tipo = *tipo;
            d.o.id = stringa(L, o, "id", "obiettivo" + std::to_string(i));
            lua_getfield(L, o, "testo");
            if (lua_istable(L, -1)) {
                d.o.testo_it = stringa(L, lua_gettop(L), "it");
                d.o.testo_en = stringa(L, lua_gettop(L), "en", d.o.testo_it);
            } else if (lua_type(L, -1) == LUA_TSTRING) {
                d.o.testo_it = d.o.testo_en = lua_tostring(L, -1);
            }
            lua_pop(L, 1);
            d.o.primario = booleano(L, o, "primario").value_or(true);
            d.o.peso = numero(L, o, "peso").value_or(1.0);
            d.o.da_radio = stringa(L, o, "da_radio");
            d.unita = interi(L, o, "unita");
            d.classi = stringhe(L, o, "classi");
            d.minimo = static_cast<int>(numero(L, o, "minimo").value_or(-1.0));
            d.colpire = booleano(L, o, "colpire").value_or(true);
            d.missione = stringa(L, o, "missione");   // tlam
            d.finestra = booleano(L, o, "finestra").value_or(true);
            if (const std::string mx = stringa(L, o, "massima"); !mx.empty())   // scatola
                d.scatola_max = mx == "piccola" ? 0 : mx == "media" ? 1 : 2;
            d.errore = numero(L, o, "errore").value_or(0.0);   // telemetro (frazione), rotta_stimata (gradi)
            d.da_tma = booleano(L, o, "da_tma").value_or(false);
            lua_getfield(L, o, "cerchio");
            if (lua_istable(L, -1)) {
                const int c = lua_gettop(L);
                d.centro = Vec2{numero(L, c, "x").value_or(0.0), numero(L, c, "y").value_or(0.0)};
                d.raggio = numero(L, c, "r").value_or(0.0);
            }
            lua_pop(L, 1);
            d.entro_s = numero(L, o, "entro_s").value_or(-1.0);
            d.indice = static_cast<int>(numero(L, o, "indice").value_or(0.0));
            if (const auto v = numero(L, o, "uguale")) d.uguale = static_cast<int>(*v);
            if (const auto v = numero(L, o, "almeno")) d.almeno = static_cast<int>(*v);
            if (const auto v = numero(L, o, "al_piu")) d.al_piu = static_cast<int>(*v);
            d.codici = interi(L, o, "codici");
            d.minuti = numero(L, o, "minuti").value_or(0.0);
            d.distanza_m = numero(L, o, "distanza_m").value_or(-1.0);
            if (const std::string p = stringa(L, o, "parte"); !p.empty())
                d.parte = p == "amica" ? Lato::amico : p == "neutrale" ? Lato::neutrale : Lato::ostile;
            d.varianti = interi(L, o, "varianti");
            definizioni_.push_back(std::move(d));
            lua_pop(L, 1);
        }
        lua_pop(L, 1);   // il valore che ha chiuso la lista
    }
    lua_settop(L, base);
}

// --- eventi ---

PosaEvento RegistroMissione::posa(int id) const {
    const auto it = info_.find(id);
    if (it == info_.end()) return {};
    return it->second.ultima;
}

Lato RegistroMissione::lato_di(const Missione& m, const UnitaClassica& u) const {
    const auto* g = m.giocatore();
    if (!g) return Lato::nessuno;
    if (u.id == g->id) return Lato::amico;
    if ((u.flag & 0x18) == 0x18)   // M7, la parte del luogo: ostile se in guerra o in crisi con la parte del giocatore
        return m.situazione_fra(0x18, g->flag & 0x18) == Situazione::pace ? Lato::neutrale : Lato::ostile;
    if ((u.flag & 0x18) == 0 || m.categoria_roe(u) == CategoriaRoe::neutrali) return Lato::neutrale;
    if (((u.flag ^ g->flag) & 0x18) == 0x18) return Lato::ostile;
    return Lato::amico;
}

EventoMissione& RegistroMissione::aggiungi(const Missione& m, TipoEvento t, int unita, int altra, int arma, double tempo) {
    EventoMissione e;
    e.id = prossimo_id_++;
    e.tipo = t;
    e.tempo = tempo >= 0.0 ? std::min(tempo, m.tempo()) : m.tempo();
    e.giocatore = giocatore_;
    e.unita = unita; e.altra = altra; e.arma = arma;
    for (const int id : {giocatore_, unita, altra, arma}) {
        if (id < 0 || info_.count(id) == 0) continue;
        bool c = false;
        for (const auto& p : e.pose) c = c || p.unita == id;
        if (!c) e.pose.push_back(posa(id));
    }
    const int principale = unita >= 0 ? unita : giocatore_;
    if (const auto it = info_.find(principale); it != info_.end()) {
        e.pos = it->second.ultima.pos;
        e.quota = it->second.ultima.quota;
        e.classe = it->second.classe;
        e.categoria = it->second.categoria;
        e.lato = it->second.lato;
        if (!it->second.arma) e.categoria_roe = it->second.roe;
        if (const auto ig = info_.find(giocatore_); ig != info_.end() && principale != giocatore_)
            e.distanza_m = distanza(ig->second.ultima.pos, it->second.ultima.pos);
    }
    if (const auto it = info_.find(arma); it != info_.end()) e.classe_arma = it->second.classe;
    // in ordine di tempo, poi d'inserimento (i danni arrivano dopo il passo con il loro tempo)
    const auto dove = std::upper_bound(eventi_.begin(), eventi_.end(), e.tempo,
                                       [](double quando, const EventoMissione& x) { return quando < x.tempo; });
    return *eventi_.insert(dove, std::move(e));
}

void RegistroMissione::componi(EventoMissione& e) const {
    bool primo = true;
    for (const auto& x : eventi_)
        if (x.id != e.id && x.tipo == e.tipo && x.id < e.id) { primo = false; break; }
    e.interesse = interesse(e, primo);
    const std::string c = nome_classe(e.classe), ce = nome_en(e.classe);
    const std::string a = arma_it(e.classe_arma), ae = arma_en(e.classe_arma);
    const auto altra = info_.find(e.altra);
    const std::string b = altra != info_.end() ? nome_classe(altra->second.classe) : "", be = altra != info_.end() ? nome_en(altra->second.classe) : "";
    const bool contro_di_noi = e.altra == e.giocatore;
    auto& q = e.inquadratura;
    q.soggetto = e.unita >= 0 ? e.unita : e.giocatore;
    q.distanza_m = 3.0 * lunghezza(e.categoria);
    q.azimut_deg = 90.0;
    q.elevazione_deg = 5.0;
    auto icona = [&](const char* i) { q.visibile = false; q.icona = i; };
    switch (e.tipo) {
        case TipoEvento::inizio:
            e.testo_it = "Inizio della missione"; e.testo_en = "Mission start";
            q.soggetto = e.giocatore; q.distanza_m = 400.0; break;
        case TipoEvento::fine:
            e.testo_it = "Fine della missione (esito " + std::to_string(e.codice) + ")";
            e.testo_en = "End of mission (outcome " + std::to_string(e.codice) + ")";
            q.soggetto = e.giocatore; q.distanza_m = 400.0; break;
        case TipoEvento::scoperta:
            e.testo_it = "Primo contatto con " + c + " a " + miglia(e.distanza_m) + " nm";
            e.testo_en = "First contact with " + ce + " at " + miglia(e.distanza_m) + " nm";
            q.distanza_m = 2.0 * MIGLIO_M; q.secondo = e.giocatore; break;
        case TipoEvento::classificazione:
            if (e.livello < 2 && (e.classe_creduta == "nave" || e.classe_creduta == "sottomarino" || e.classe_creduta == "aereo")) {
                e.testo_it = "Contatto classificato come " + e.classe_creduta;
                e.testo_en = std::string{"Contact classified as "} + (e.classe_creduta == "nave" ? "surface ship" : e.classe_creduta == "sottomarino" ? "submarine" : "aircraft");
            } else {
                e.testo_it = "Contatto identificato: " + (e.classe_creduta.empty() ? c : nome_classe(e.classe_creduta)) +
                             (e.giusta ? "" : " (era " + c + ")");
                e.testo_en = "Contact identified: " + (e.classe_creduta.empty() ? ce : nome_en(e.classe_creduta)) +
                             (e.giusta ? "" : " (it was " + ce + ")");
            }
            icona("sonar"); break;
        case TipoEvento::lancio_giocatore:
            e.testo_it = "Lanciato " + a + (b.empty() ? "" : " contro " + b);
            e.testo_en = "Launched " + ae + (be.empty() ? "" : " at " + be);
            q.soggetto = e.arma >= 0 ? e.arma : e.giocatore; q.secondo = e.giocatore; q.distanza_m = 400.0;
            q.azimut_deg = 150.0; q.elevazione_deg = e.quota > 1.0 ? -5.0 : 8.0; break;
        case TipoEvento::lancio_nemico:
            e.testo_it = c + " ha lanciato " + a + (contro_di_noi ? " contro di noi" : b.empty() ? "" : " contro " + b);
            e.testo_en = ce + " launched " + ae + (contro_di_noi ? " at us" : be.empty() ? "" : " at " + be);
            q.soggetto = e.arma >= 0 ? e.arma : e.unita; q.secondo = e.unita; q.distanza_m = 400.0; q.azimut_deg = 150.0;
            q.elevazione_deg = -5.0; break;
        case TipoEvento::colpo:
            if (e.unita == e.giocatore) {
                e.testo_it = "Siamo stati colpiti da " + a;
                e.testo_en = "We have been hit by " + ae;
            } else if (e.valore > 0.0 || e.altra == e.giocatore) {
                e.testo_it = "Colpo a segno su " + c + " (" + a + (b.empty() ? "" : " di " + b) + ")";
                e.testo_en = ce + " hit by " + ae + (be.empty() ? "" : " from " + be);
            } else {   // un colpo mancato (carica dell'ATTACK fallita, artiglieria fuori bersaglio)
                e.testo_it = "Colpo mancato su " + c + " (" + a + (b.empty() ? "" : " di " + b) + ")";
                e.testo_en = ce + " missed by " + ae + (be.empty() ? "" : " from " + be);
            }
            q.elevazione_deg = 3.0; break;
        case TipoEvento::affondamento:
            if (e.unita == e.giocatore) { e.testo_it = "Il nostro battello è perduto"; e.testo_en = "Our boat is lost"; }
            else { e.testo_it = c + " affondata"; e.testo_en = ce + " sunk"; }
            q.elevazione_deg = 4.0; break;
        case TipoEvento::emersione_emergenza:
            if (e.unita == giocatore_) { e.testo_it = "Emersione d'emergenza: aria alle casse"; e.testo_en = "Emergency blow: air to the tanks"; }
            else { e.testo_it = c + " costretto all'emersione d'emergenza"; e.testo_en = ce + " forced to an emergency surfacing"; }
            q.distanza_m = 600.0; q.azimut_deg = 60.0; q.elevazione_deg = 5.0; break;
        case TipoEvento::siluro_evitato:
            e.testo_it = "Evitato " + arma_it(e.classe) + " nemico"; e.testo_en = "Evaded " + arma_en(e.classe) + " fired at us";
            q.secondo = e.giocatore; q.distanza_m = 200.0; q.elevazione_deg = -5.0; break;
        case TipoEvento::esca_sedotta:
            e.testo_it = "Un " + c + " ha inseguito un'esca"; e.testo_en = "A " + ce + " went for a decoy";
            q.secondo = e.altra; q.distanza_m = 200.0; q.elevazione_deg = -5.0; break;
        case TipoEvento::danno_subito:
        case TipoEvento::danno_domato:
        case TipoEvento::scram:
            if (e.testo_it.empty()) e.testo_it = "Danni a bordo";
            if (e.testo_en.empty()) e.testo_en = "Damage aboard";
            icona("danni"); break;
        case TipoEvento::camera_di_lancio_persa:
            e.testo_it = "Camera di lancio allagata: le armi a bordo sono perse";
            e.testo_en = "Torpedo room flooded: the weapons aboard are lost";
            icona("danni"); break;
        case TipoEvento::messaggio_ricevuto:
            e.testo_it = "Ricevuto il messaggio serie " + std::to_string(e.codice) + (e.valore > 0 ? ": ordini cambiati" : "");
            e.testo_en = "Message serial " + std::to_string(e.codice) + " received" + (e.valore > 0 ? ": new orders" : "");
            icona("radio"); break;
        case TipoEvento::messaggio_perso:
            e.testo_it = "Perso il messaggio serie " + std::to_string(e.codice);
            e.testo_en = "Missed message serial " + std::to_string(e.codice);
            icona("radio"); break;
        case TipoEvento::compito_scaduto:
            e.testo_it = "Compito della serie " + std::to_string(e.codice) + " scaduto";
            e.testo_en = "Tasking of serial " + std::to_string(e.codice) + " expired";
            icona("radio"); break;
        case TipoEvento::sessione_mancata:
            e.testo_it = "Sessione di comunicazione mancata"; e.testo_en = "Communication window missed";
            icona("radio"); break;
        case TipoEvento::ordine_ricevuto:
            if (e.testo_it.empty()) e.testo_it = "Nuovo ordine";
            if (e.testo_en.empty()) e.testo_en = "New orders";
            icona("radio"); break;
        case TipoEvento::trasmissione:
            e.testo_it = std::string{"Trasmesso un rapporto ("} + nome_trasmissione(static_cast<TipoTrasmissione>(e.codice)) + ")";
            e.testo_en = std::string{"Transmitted a report ("} + nome_trasmissione(static_cast<TipoTrasmissione>(e.codice)) + ")";
            icona("radio"); break;
        case TipoEvento::infrazione_roe:
            e.testo_it = "Lancio contro le regole d'ingaggio" + (b.empty() ? std::string{} : " su " + b);
            e.testo_en = "Launch against the rules of engagement" + (be.empty() ? std::string{} : " on " + be);
            icona("zona"); break;
        case TipoEvento::ingresso_zona:
            e.testo_it = "Ingresso nella zona protetta " + e.classe_creduta;
            e.testo_en = "Entered protected area " + e.classe_creduta;
            icona("zona"); break;
        case TipoEvento::fix_radiogoniometria:
            e.testo_it = e.valore > 0 ? "Il nemico ci ha localizzati con la radiogoniometria" : "Il nemico ha rilevato la nostra trasmissione";
            e.testo_en = e.valore > 0 ? "The enemy fixed us by direction finding" : "The enemy got a bearing on our transmission";
            icona("radiogoniometro"); break;
        case TipoEvento::avvistato:
            e.testo_it = c + " ci ha scoperti"; e.testo_en = ce + " has detected us";
            q.soggetto = e.unita; q.secondo = e.giocatore; q.distanza_m = std::max(300.0, std::min(e.distanza_m, 3000.0));
            q.azimut_deg = 0.0; q.elevazione_deg = 2.0; break;
        case TipoEvento::esm_rischio_alto:
            e.testo_it = "ESM: rischio di scoperta alto (" + c + ")"; e.testo_en = "ESM: high counter-detection risk (" + ce + ")";
            icona("esm"); break;
        case TipoEvento::ping_attivo:
            e.testo_it = "Ping del sonar attivo"; e.testo_en = "Active sonar ping"; icona("sonar"); break;
        case TipoEvento::ping_subito:   // un episodio: con piu' ping, quanti
            e.testo_it = "Ci stanno pingando"; e.testo_en = "We are being pinged";
            if (e.valore >= 2.0) {
                e.testo_it += " (" + std::to_string(static_cast<int>(e.valore)) + " ping)";
                e.testo_en += " (" + std::to_string(static_cast<int>(e.valore)) + " pings)";
            }
            icona("sonar"); break;
        case TipoEvento::transitorio_proprio:
            e.testo_it = "Stiamo cavitando"; e.testo_en = "We are cavitating"; icona("sonar"); break;
        case TipoEvento::boa_vicina:
            e.testo_it = "Boa nemica a " + miglia(e.distanza_m) + " nm"; e.testo_en = "Enemy buoy at " + miglia(e.distanza_m) + " nm";
            q.secondo = e.giocatore; q.distanza_m = 300.0; break;
        case TipoEvento::incaglio:
            e.testo_it = "Urto col fondo"; e.testo_en = "Grounding";
            q.soggetto = e.giocatore; q.distanza_m = 300.0; q.elevazione_deg = -10.0; break;
        case TipoEvento::fuoco_amico:
            e.testo_it = "Fuoco amico su " + c; e.testo_en = "Friendly fire on " + ce; break;
        case TipoEvento::mina_posata: {
            const std::string n = nome_classe(e.classe_arma);
            e.testo_it = "Mina " + n + (e.livello > 0 ? " posata su un fondale inadatto" : " posata");
            e.testo_en = n + " mine " + (e.livello > 0 ? "laid on unsuitable bottom" : "laid");
            q.soggetto = e.giocatore; icona("mina"); break;
        }
        case TipoEvento::mina_esplosa: {
            const std::string n = nome_classe(e.classe_arma);
            if (e.livello > 0) {
                e.testo_it = "La mina " + n + " ha lanciato il suo siluro contro " + c;
                e.testo_en = "The " + n + " mine launched its torpedo at " + ce;
            } else {
                e.testo_it = "Una mina " + n + " è esplosa sotto " + c;
                e.testo_en = "A " + n + " mine exploded under " + ce;
            }
            q.elevazione_deg = -10.0; break;
        }
        case TipoEvento::aereo_in_sorvolo:
            e.testo_it = "Aereo in sorvolo: " + c; e.testo_en = "Aircraft overhead: " + ce;
            q.soggetto = e.unita; q.secondo = e.giocatore; q.distanza_m = 400.0; q.elevazione_deg = -20.0; break;
        case TipoEvento::ping_sonar_immersione:
            e.testo_it = "Sonar a immersione: " + c; e.testo_en = "Dipping sonar: " + ce;
            q.soggetto = e.unita; q.distanza_m = 600.0; q.elevazione_deg = 5.0; break;
        case TipoEvento::marcatore_avvistato:
            e.testo_it = "Fumogeno in vista: il nemico ha un datum su di noi";
            e.testo_en = "Smoke marker in sight: the enemy has a datum on us"; icona("marcatore"); break;
        case TipoEvento::attacco_simulato:
            e.testo_it = "Attacco simulato di " + c; e.testo_en = "Simulated attack by " + ce; icona("sus"); break;
        case TipoEvento::foto_periscopio: {
            // la vista dal periscopio: la camera sulla testa del giocatore, verso il soggetto
            const std::string r = std::to_string(static_cast<int>(std::lround(e.valore)) % 360);
            if (e.unita >= 0) {
                e.testo_it = "Foto al periscopio: " + c + " a " + miglia(e.distanza_m) + " nm";
                e.testo_en = "Periscope photo: " + ce + " at " + miglia(e.distanza_m) + " nm";
                q.soggetto = e.unita; q.secondo = -1; q.distanza_m = e.distanza_m; q.elevazione_deg = 0.5;
                if (const auto s = std::find_if(e.pose.begin(), e.pose.end(), [&](const PosaEvento& p) { return p.unita == e.unita; });
                    s != e.pose.end())
                    if (const auto g = std::find_if(e.pose.begin(), e.pose.end(), [&](const PosaEvento& p) { return p.unita == e.giocatore; });
                        g != e.pose.end())
                        q.azimut_deg = normalizza_360(rilevamento(s->pos, g->pos) - s->rotta);
            } else {
                e.testo_it = "Foto al periscopio, rilevamento " + r; e.testo_en = "Periscope photo, bearing " + r;
                q.soggetto = e.giocatore; q.distanza_m = 0.0; q.elevazione_deg = 0.5; q.azimut_deg = e.valore;
            }
            break;
        }
        case TipoEvento::scambio_tracce: {
            const std::string n = std::to_string(e.livello), b2 = altra != info_.end() ? nome_classe(altra->second.classe) : "?";
            e.testo_it = "Sierra " + n + " ha cambiato bersaglio in un incrocio: ora segue " + c + ", prima " + b2;
            e.testo_en = "Sierra " + n + " swapped targets at a crossing: now on " + ce + ", before " +
                         (altra != info_.end() ? nome_en(altra->second.classe) : "?");
            icona("sonar"); break;
        }
        case TipoEvento::esposizione_lunga: {
            const std::string sec = std::to_string(static_cast<int>(std::lround(e.valore)));
            e.testo_it = "Periscopio esposto per " + sec + " s"; e.testo_en = "Periscope exposed for " + sec + " s";
            icona("periscopio"); break;
        }
        case TipoEvento::artiglieria_salva:
            e.testo_it = e.codice ? c + " spara colpi d'avvertimento" : c + " apre il fuoco su di noi";
            e.testo_en = e.codice ? ce + " fires warning shots" : ce + " opens fire on us";
            q.soggetto = e.unita; q.secondo = e.giocatore; q.distanza_m = std::max(300.0, std::min(e.distanza_m, 3000.0));
            q.elevazione_deg = 3.0; break;
        case TipoEvento::artiglieria_colpo:
            e.testo_it = "Colpiti dall'artiglieria di " + c; e.testo_en = "Hit by gunfire from " + ce;
            q.soggetto = e.giocatore; q.distanza_m = 150.0; q.elevazione_deg = 5.0; break;
        case TipoEvento::artiglieria_vicino: {
            const std::string m = std::to_string(static_cast<int>(std::lround(e.valore)));
            e.testo_it = "Colonna d'acqua a " + m + " m: tiro di " + c; e.testo_en = "Shell splash " + m + " m away: gunfire from " + ce;
            q.soggetto = e.giocatore; q.distanza_m = 200.0; q.elevazione_deg = 2.0; break;
        }
        case TipoEvento::collisione:
            e.testo_it = e.livello ? "Strisciato contro " + c : "Collisione con " + c;
            e.testo_en = e.livello ? "Scraped against " + ce : "Collision with " + ce;
            q.soggetto = e.giocatore; q.secondo = e.unita; q.distanza_m = 250.0; q.elevazione_deg = 0.0; break;
        case TipoEvento::quasi_collisione: {
            const std::string m = std::to_string(static_cast<int>(std::lround(e.valore)));
            e.testo_it = "Passaggio ravvicinato con " + c + " a " + m + " m" + (e.codice ? "" : ", senza saperlo");
            e.testo_en = "Close pass with " + ce + " at " + m + " m" + (e.codice ? "" : ", unaware");
            q.soggetto = e.giocatore; q.secondo = e.unita; q.distanza_m = std::max(300.0, e.valore * 2.0); q.elevazione_deg = 0.0; break;
        }
        case TipoEvento::cavo_impigliato:
            e.testo_it = "Il cavo della schiera di " + c + " nell'elica"; e.testo_en = ce + "'s towed array cable fouled our propeller";
            q.soggetto = e.giocatore; q.distanza_m = 150.0; q.elevazione_deg = -5.0; break;
        case TipoEvento::cavo_toccato:
            e.testo_it = "Abbiamo toccato il cavo della schiera di " + c; e.testo_en = "We touched " + ce + "'s towed array cable";
            q.soggetto = e.giocatore; q.distanza_m = 150.0; q.elevazione_deg = -5.0; break;
        case TipoEvento::mina_scoperta: {
            const std::string m = std::to_string(static_cast<int>(std::lround(e.valore)));
            e.testo_it = std::string("BQS-15: mina ") + (e.livello ? "ormeggiata" : "sul fondo") + " a " + m + " m";
            e.testo_en = std::string("BQS-15: ") + (e.livello ? "moored" : "bottom") + " mine at " + m + " m";
            q.soggetto = e.giocatore; q.distanza_m = std::max(200.0, e.valore); q.elevazione_deg = -10.0; icona("mina"); break;
        }
        case TipoEvento::lancio_ia:
            e.testo_it = c + " lancia " + a + (b.empty() ? "" : " su " + b);   // arma_it ha gia' l'articolo
            e.testo_en = ce + " launches " + ae + (be.empty() ? "" : " at " + be);
            q.soggetto = e.unita; q.secondo = e.altra; q.distanza_m = 400.0; q.elevazione_deg = 2.0; break;
        case TipoEvento::scoperta_ia:
            e.testo_it = c + " scopre " + b; e.testo_en = ce + " detects " + be;
            q.soggetto = e.unita; q.secondo = e.altra; q.distanza_m = 500.0; q.elevazione_deg = 3.0; break;
        case TipoEvento::ingaggio_ia:   // codice 1 in guerra; se no l'aggancio senza fuoco
            if (e.codice == 1) { e.testo_it = c + " attacca " + b; e.testo_en = ce + " engages " + be; }
            else { e.testo_it = c + " tiene sotto " + b + ", senza fuoco"; e.testo_en = ce + " holds down " + be + ", without firing"; }
            q.soggetto = e.unita; q.secondo = e.altra; q.distanza_m = 500.0; q.elevazione_deg = 3.0; break;
        case TipoEvento::evasione_ia:
            e.testo_it = c + " manovra per sfuggire alla caccia"; e.testo_en = ce + " manoeuvres to evade";
            q.soggetto = e.unita; q.distanza_m = 300.0; q.elevazione_deg = 0.0; break;
        case TipoEvento::artiglieria_ia:
            e.testo_it = c + " apre il fuoco su " + b; e.testo_en = ce + " opens fire on " + be;
            q.soggetto = e.unita; q.secondo = e.altra; q.distanza_m = 1500.0; q.elevazione_deg = 4.0; break;
        case TipoEvento::classificazione_ia: {
            const std::string k = e.classe_creduta.empty() ? std::string("contatto") : nome_classe(e.classe_creduta);
            e.testo_it = c + " classifica " + k + (e.giusta ? "" : " (in realta' " + b + ")");
            e.testo_en = ce + " classifies " + (e.classe_creduta.empty() ? std::string("contact") : nome_en(e.classe_creduta)) +
                         (e.giusta ? "" : " (actually " + be + ")");
            q.soggetto = e.unita; q.secondo = e.altra; q.distanza_m = 500.0; q.elevazione_deg = 3.0; break;
        }
        case TipoEvento::allerta_forza: {
            static const char* const NOMI_IT[] = {"routine", "vigilanza", "minaccia", "combattimento"};
            static const char* const NOMI_EN[] = {"routine", "alert", "threat", "battle stations"};
            const int l = std::clamp(e.livello, 0, 3);
            const std::string forza_it = e.codice == 0x08 ? "NATO" : e.codice == 0x10 ? "sovietica" : "neutrale";
            const std::string forza_en = e.codice == 0x08 ? "NATO" : e.codice == 0x10 ? "Soviet" : "neutral";
            e.testo_it = (e.unita < 0 ? "Allerta della forza " + forza_it : "Allerta della formazione di " + c) + ": A" +
                         std::to_string(l) + " " + NOMI_IT[l];
            e.testo_en = (e.unita < 0 ? forza_en + " force alert" : ce + "'s group alert") + ": A" + std::to_string(l) + " " + NOMI_EN[l];
            q.soggetto = e.unita; q.distanza_m = 800.0; q.elevazione_deg = 5.0; icona("radio"); break;
        }
        case TipoEvento::provocazione:
            e.testo_it = c + " attacca " + b + ": fra le due parti e' guerra";
            e.testo_en = ce + " attacks " + be + ": the two sides are at war";
            q.soggetto = e.unita; q.secondo = e.altra; q.distanza_m = 800.0; q.elevazione_deg = 4.0; break;
        case TipoEvento::messaggio_ia: {
            static const char* const CANALI_IT[] = {"alla formazione", "dall'aereo alle navi", "al comando", "ai sottomarini col broadcast", "a noi"};
            static const char* const CANALI_EN[] = {"to the group", "from the aircraft to the ships", "to the command", "to the submarines on the broadcast", "to us"};
            const int k = std::clamp(e.codice, 0, 4);
            e.testo_it = c + " riferisce " + b + " " + CANALI_IT[k] + (e.giusta ? "" : " (perso)");
            e.testo_en = ce + " reports " + be + " " + CANALI_EN[k] + (e.giusta ? "" : " (lost)");
            q.soggetto = e.unita; q.secondo = e.altra; q.distanza_m = 800.0; q.elevazione_deg = 4.0; icona("radio"); break;
        }
        case TipoEvento::fine_missile: {   // la verita' per il debriefing
            if (e.codice == 2) {   // fermato dalla difesa: altra = la nave
                e.testo_it = "Il missile " + e.classe_arma + " di " + c + " e' abbattuto dalla difesa della " + (b.empty() ? "nave" : b);
                e.testo_en = ce + "'s " + e.classe_arma + " missile is shot down by the defences of the " + (be.empty() ? "ship" : be);
                break;
            }
            const std::string sec = std::to_string(static_cast<int>(std::lround(e.valore)));
            e.testo_it = "Il missile " + e.classe_arma + " di " + c + (e.codice == 1 ? " si guasta in volo dopo " : " finisce la corsa senza bersaglio dopo ") + sec + " s";
            e.testo_en = ce + "'s " + e.classe_arma + " missile" + (e.codice == 1 ? " fails in flight after " : " ends its run without a target after ") + sec + " s";
            break;
        }
        case TipoEvento::misura_telemetro: {   // la verita' per il debriefing: la distanza vera il giocatore non la sa
            const auto km = [](double m_) {
                char t[16];
                std::snprintf(t, sizeof t, "%.1f", m_ / 1000.0);
                std::string s(t);
                std::replace(s.begin(), s.end(), '.', ',');
                return s;
            };
            const int pct = e.distanza_m > 0.0 ? static_cast<int>(std::lround(100.0 * (e.valore - e.distanza_m) / e.distanza_m)) : 0;
            const std::string segno = pct > 0 ? "+" : "";
            const std::string alt = std::to_string(static_cast<int>(std::lround(e.misura_m)));
            e.testo_it = "Telemetro su " + c + ": " + km(e.valore) + " km (veri " + km(e.distanza_m) + ", " + segno + std::to_string(pct) + "%), albero " + alt + " m";
            e.testo_en = "Rangefinder on " + ce + ": " + km(e.valore) + " km (true " + km(e.distanza_m) + ", " + segno + std::to_string(pct) + "%), mast " + alt + " m";
            break;
        }
        case TipoEvento::osservazione_aob: {   // la verita' per il debriefing
            const std::string g_ = std::to_string(static_cast<int>(std::lround(e.valore)));
            e.testo_it = "AOB al periscopio su " + c + ": la rotta stimata a " + g_ + " gradi dalla vera";
            e.testo_en = "Periscope AOB on " + ce + ": estimated course " + g_ + " degrees off";
            break;
        }
        case TipoEvento::ping_ecoscandaglio:
            e.testo_it = "Ping dell'ecoscandaglio";
            e.testo_en = "Echo sounder ping";
            break;
        case TipoEvento::allarme_siluro:
            e.testo_it = e.codice == 2 ? "Intercettatore: siluro in acquisizione" : "Intercettatore: siluro in acqua";
            e.testo_en = e.codice == 2 ? "Intercept receiver: torpedo acquiring" : "Intercept receiver: torpedo in the water";
            break;
        case TipoEvento::attraversamento_mine:
            e.testo_it = "Campo di mine attraversato col BQS-15, senza danni";
            e.testo_en = "Minefield crossed with the BQS-15, undamaged";
            break;
        case TipoEvento::verifica_radio:
            e.testo_it = e.giusta ? "Autenticazione verificata" : "Messaggio non autentico riconosciuto";
            e.testo_en = e.giusta ? "Authentication verified" : "Non-authentic message recognised";
            break;
        case TipoEvento::intervento: break;   // il testo e' la riga dell'intervento, gia' composta (osserva)
        case TipoEvento::posti_di_combattimento: {
            const std::string perche = e.classe_arma == "siluro" ? "siluro in acqua" : e.classe_arma == "lancio" ? "lancio" : "contatto ostile vicino";
            const std::string because = e.classe_arma == "siluro" ? "torpedo in the water" : e.classe_arma == "lancio" ? "weapon launched" : "hostile contact close";
            e.testo_it = e.livello ? "Posti di combattimento (" + perche + ")" : "Condizione normale";
            e.testo_en = e.livello ? "Battle stations (" + because + ")" : "Normal condition";
            q.soggetto = e.giocatore;
            break;
        }
        case TipoEvento::sonar_traino: {
            const std::string q_ = std::to_string(static_cast<int>(std::lround(e.valore)));
            const bool quota = e.livello && e.valore >= 0.0;
            e.testo_it = c + (e.livello ? " fila " : " recupera ") + e.classe_arma + (quota ? " a " + q_ + " m" : "");
            e.testo_en = ce + (e.livello ? " streams " : " recovers ") + e.classe_arma + (quota ? " at " + q_ + " m" : "");
            break;
        }
        case TipoEvento::punto_nave_ia: {
            const std::string m_ = std::to_string(static_cast<int>(std::lround(e.valore)));
            e.testo_it = c + ": punto nave " + e.classe_arma + ", la stima si sposta di " + m_ + " m";
            e.testo_en = ce + ": " + e.classe_arma + " fix, the estimate moves " + m_ + " m";
            break;
        }
        case TipoEvento::luci:
            e.testo_it = e.valore == 0.0 ? "Luci di navigazione spente" : "Luci di navigazione accese";
            e.testo_en = e.valore == 0.0 ? "Navigation lights off" : "Navigation lights on";
            break;
        case TipoEvento::esito_tlam:   // la verita' per il debriefing: il giocatore lo sa dal BDA alla radio
            e.testo_it = "TLAM missione " + e.classe_arma + (e.giusta ? ": colpito " : ": perso prima di ") + e.classe_creduta;
            e.testo_en = "TLAM mission " + e.classe_arma + (e.giusta ? ": hit " : ": lost before ") + e.classe_creduta;
            break;
        case TipoEvento::comandante_ia:
            e.testo_it = "Comandante IA: " + e.classe_arma + ", " + e.classe_creduta + (b.empty() ? "" : " (" + b + ")");
            e.testo_en = "AI captain: " + e.classe_arma + (be.empty() ? "" : " (" + be + ")");
            q.soggetto = e.giocatore; q.secondo = e.altra; q.distanza_m = 500.0; q.elevazione_deg = 10.0; break;
        case TipoEvento::punto_nave: {
            const std::string nm = std::to_string(static_cast<int>(std::lround(e.valore / 185.2))), dec = nm.size() > 1 ? nm.substr(0, nm.size() - 1) + "," + nm.back() : "0," + nm;
            e.testo_it = e.giusta ? "Punto nave " + e.classe_arma + ": la stima si sposta di " + dec + " miglia" : "Punto nave " + e.classe_arma + " non riuscito";
            e.testo_en = e.giusta ? e.classe_arma + " fix: the estimate moves " + dec + " nm" : e.classe_arma + " fix failed";
            q.soggetto = e.giocatore; q.distanza_m = 300.0; q.elevazione_deg = 20.0; icona("radio"); break;
        }
        case TipoEvento::incsea: {
            const std::string m = std::to_string(static_cast<int>(e.valore));
            const auto iu = info_.find(e.unita);
            const bool aereo = iu != info_.end() && ((iu->second.flag & 3) == 3 || iu->second.categoria == "aereo");
            e.testo_it = aereo ? c + " sorvola " + b + " a " + m + " m e sotto i 500 piedi, contro l'accordo INCSEA"
                               : c + " passa a " + m + " m da " + b + ", sotto i 1000 yd dell'accordo INCSEA";
            e.testo_en = aereo ? ce + " overflies " + be + " at " + m + " m below 500 ft, against the INCSEA agreement"
                               : ce + " passes " + m + " m from " + be + ", inside the 1000 yd of the INCSEA agreement";
            q.soggetto = e.unita; q.secondo = e.altra; q.distanza_m = 1200.0; q.elevazione_deg = 8.0; break;
        }
        case TipoEvento::reazione_ia: {
            const std::string& r = e.classe_arma;
            // quelle del vetrino di Dio (D3 e D4), per lo spettatore: il disturbo, il falso ordine, chi localizza, chi da' la colpa
            if (r == "localizza") {
                e.testo_it = (e.unita < 0 ? std::string("La rete strategica") : c) + " localizza " + b + " con la radiogoniometria";
                e.testo_en = (e.unita < 0 ? std::string("The strategic network") : ce) + " fixes " + be + " by direction finding";
                q.soggetto = e.unita >= 0 ? e.unita : e.altra; q.secondo = e.unita >= 0 ? e.altra : -1; q.distanza_m = 800.0; q.elevazione_deg = 4.0; icona("radio");
                break;
            }
            if (r == "messaggio_perso") {
                e.testo_it = e.altra >= 0 ? "Il rapporto di " + c + " su " + b + " si perde nel disturbo radio" : "Il FLASH di " + c + " si perde nel disturbo radio";
                e.testo_en = e.altra >= 0 ? ce + "'s report on " + be + " is lost in the radio jamming" : ce + "'s FLASH is lost in the radio jamming";
                q.soggetto = e.unita; q.secondo = e.altra; q.distanza_m = 800.0; q.elevazione_deg = 4.0; icona("radio");
                break;
            }
            if (r.rfind("ordine_", 0) == 0) {
                const bool cr = r == "ordine_creduto", ig = r == "ordine_ignorato", ve = r == "ordine_verifica", sm = r == "ordine_smascherato";
                e.testo_it = c + (cr ? " crede all'ordine falso" : ig ? " ignora l'ordine falso" : ve ? " chiede conferma dell'ordine falso" : sm ? " smaschera l'ordine falso" : " non riceve l'ordine falso");
                e.testo_en = ce + (cr ? " believes the false order" : ig ? " ignores the false order" : ve ? " asks to confirm the false order" : sm ? " exposes the false order" : " does not receive the false order");
                q.soggetto = e.unita; q.distanza_m = 800.0; q.elevazione_deg = 4.0; icona("radio");
                break;
            }
            if (r == "ammaraggio") {
                e.testo_it = c + " ammara: la nave madre non c'e' piu'";
                e.testo_en = ce + " ditches: its mother ship is gone";
                q.soggetto = e.unita; q.distanza_m = 800.0; q.elevazione_deg = 12.0;
                break;
            }
            if (r == "hangar_perso") {
                e.testo_it = c + " non decolla: l'hangar di " + b + " e' bruciato";
                e.testo_en = ce + " does not take off: the hangar of " + be + " has burned";
                q.soggetto = e.altra; q.distanza_m = 800.0; q.elevazione_deg = 12.0;
                break;
            }
            if (r == "ghiaccio_nuovo") {
                e.testo_it = "Ghiaccio nuovo sopra " + c + ": la polynya si chiude";
                e.testo_en = "New ice over " + ce + ": the polynya is closing";
                q.soggetto = e.unita; q.distanza_m = 800.0; q.elevazione_deg = 12.0;
                break;
            }
            if (r == "cambio_saputo") {
                e.testo_it = c + " scopre che " + b + " ha cambiato parte";
                e.testo_en = ce + " finds out that " + be + " has changed sides";
                q.soggetto = e.unita; q.secondo = e.altra; q.distanza_m = 800.0; q.elevazione_deg = 4.0;
                break;
            }
            if (r == "cambio_saputo_usa" || r == "cambio_saputo_urss") {
                const bool usa = r == "cambio_saputo_usa";
                e.testo_it = std::string(usa ? "La NATO" : "Il Patto") + " sa che " + c + " ha cambiato parte";
                e.testo_en = std::string(usa ? "NATO" : "The Pact") + " knows that " + ce + " has changed sides";
                q.soggetto = e.unita; q.distanza_m = 800.0; q.elevazione_deg = 4.0;
                break;
            }
            if (r.rfind("attribuisce_", 0) == 0) {
                const std::string a = r.substr(12);
                e.testo_it = c + " da' la colpa " + (a == "usa" ? "agli americani" : a == "urss" ? "ai sovietici" : a == "ignoto" ? "a ignoti" : "a " + a);
                e.testo_en = ce + " blames " + (a == "usa" ? "the Americans" : a == "urss" ? "the Soviets" : a == "ignoto" ? "persons unknown" : a);
                q.soggetto = e.unita; q.secondo = e.altra; q.distanza_m = 800.0; q.elevazione_deg = 4.0;
                break;
            }
            // il quadro radio ai sottomarini dell'IA, per lo spettatore
            if (r == "ascolto_broadcast" || r == "dato_radio_usato" || r == "dato_radio_lasciato") {
                e.testo_it = r == "ascolto_broadcast" ? c + " sale ad ascoltare il broadcast" : r == "dato_radio_usato" ? c + " va sul dato radio di " + b : c + " lascia il dato radio di " + b;
                e.testo_en = r == "ascolto_broadcast" ? ce + " comes up to copy the broadcast" : r == "dato_radio_usato" ? ce + " heads for the radio report of " + be : ce + " drops the radio report of " + be;
                q.soggetto = e.unita; q.secondo = e.altra; q.distanza_m = 800.0; q.elevazione_deg = 4.0; icona("radio");
                break;
            }
            e.testo_it = c + (r == "pedina" ? " pedina " : r == "pedina_vicino" ? " pedina da vicino, nel cono sordo, " :
                              r == "fine_pedina_vicino" ? " smette di pedinare da vicino " : r == "datum" ? " va sul datum di " : r == "tiene_sotto" ? " tiene sotto col sonar attivo " : r == "allontana" ? " si allontana da " :
                              r == "ascolta" ? " si ferma ad ascoltare dopo l'attacco a " : r == "copre" ? " si porta a coprire su " : " riceve il datum di ") + b;
            e.testo_en = ce + (r == "pedina" ? " trails " : r == "pedina_vicino" ? " trails closely, in the baffles of, " :
                               r == "fine_pedina_vicino" ? " stops trailing closely " : r == "datum" ? " heads for the datum of " : r == "tiene_sotto" ? " holds down with active sonar " : r == "allontana" ? " moves away from " :
                               r == "ascolta" ? " stops to listen after the attack on " : r == "copre" ? " moves to cover " : " gets the datum of ") + be;
            q.soggetto = e.unita; q.secondo = e.altra; q.distanza_m = 800.0; q.elevazione_deg = 4.0; break;
        }
        case TipoEvento::tma_ia:
            e.testo_it = e.codice == 1 ? c + " ha la soluzione su " + b : e.codice == 2 ? c + " rinuncia alla TMA su " + b : c + " accosta per la TMA su " + b;
            e.testo_en = e.codice == 1 ? ce + " has a solution on " + be : e.codice == 2 ? ce + " gives up the TMA on " + be : ce + " turns for TMA on " + be;
            q.soggetto = e.unita; q.secondo = e.altra; q.distanza_m = 400.0; q.elevazione_deg = 0.0; break;
        case TipoEvento::esca_ia:
            e.testo_it = c + (e.classe_arma == "mg74" ? " lancia un'esca semovente" : " lancia un'esca");
            e.testo_en = ce + (e.classe_arma == "mg74" ? " launches a mobile decoy" : " launches a decoy");
            q.soggetto = e.unita; q.distanza_m = 200.0; q.elevazione_deg = -5.0; break;
        case TipoEvento::ritirata_ia:
            e.testo_it = c + (e.classe_arma == "colpito" ? " colpito, si ritira" : e.classe_arma == "siluro" ? " si allontana dal siluro" :
                              e.classe_arma == "ping" ? " scoperto dal ping, si allontana" : " evita la collisione");
            e.testo_en = ce + (e.classe_arma == "colpito" ? " hit, withdraws" : e.classe_arma == "siluro" ? " turns away from the torpedo" :
                               e.classe_arma == "ping" ? " detected by the ping, opens" : " avoids a collision");
            q.soggetto = e.unita; q.secondo = e.altra; q.distanza_m = 400.0; q.elevazione_deg = 2.0; break;
        case TipoEvento::fuoco_amico_ia:
            e.testo_it = "Fuoco amico: " + c + " attacca " + b; e.testo_en = "Friendly fire: " + ce + " attacks " + be;
            q.soggetto = e.unita; q.secondo = e.altra; q.distanza_m = 500.0; q.elevazione_deg = 3.0; break;
        case TipoEvento::collisione_ia:
            e.testo_it = "Collisione fra " + c + " e " + b; e.testo_en = "Collision between " + ce + " and " + be;
            q.soggetto = e.unita; q.secondo = e.altra; q.distanza_m = 250.0; q.elevazione_deg = 0.0; break;
        case TipoEvento::colpo_esercitazione:
            e.testo_it = "Colpo d'esercitazione su " + c + (b.empty() ? "" : " da " + b);
            e.testo_en = "Exercise hit on " + ce + (be.empty() ? "" : " by " + be);
            q.soggetto = e.unita; q.secondo = e.altra; q.distanza_m = 200.0; q.elevazione_deg = -5.0; break;
        case TipoEvento::acque_territoriali: {
            const std::string n = e.classe_arma.empty() ? "" : " (" + e.classe_arma + ")";
            e.testo_it = e.livello ? "Usciti dalle acque territoriali" + n : "Entrati nelle acque territoriali" + n;
            e.testo_en = e.livello ? "Left territorial waters" + n : "Entered territorial waters" + n;
            q.soggetto = e.giocatore; q.distanza_m = 300.0; q.elevazione_deg = 5.0; break;
        }
        case TipoEvento::avaria_ia:
            e.testo_it = c + " in avaria: propulsione persa"; e.testo_en = ce + " broken down: propulsion lost";
            q.soggetto = e.unita; q.distanza_m = 400.0; q.elevazione_deg = 3.0; break;
        case TipoEvento::rimorchio:
            e.testo_it = e.livello ? "Finito il rimorchio di " + c : c + " preso a rimorchio" + (b.empty() ? "" : " da " + b);
            e.testo_en = e.livello ? ce + " no longer under tow" : ce + " taken in tow" + (be.empty() ? "" : " by " + be);
            q.soggetto = e.unita; q.secondo = e.altra; q.distanza_m = 600.0; q.elevazione_deg = 4.0; break;
        case TipoEvento::emersione_forzata:
            e.testo_it = c + " emerge: batteria scarica"; e.testo_en = ce + " surfaces: battery exhausted";
            q.soggetto = e.unita; q.distanza_m = 400.0; q.elevazione_deg = 3.0; break;
        case TipoEvento::numero: break;
    }
    if (e.tipo == TipoEvento::colpo || e.tipo == TipoEvento::affondamento || e.tipo == TipoEvento::fuoco_amico)
        q.distanza_m = 3.0 * lunghezza(e.categoria);
    if (terza_persona_) in_terza_persona(e);
}

// la sandbox (Missione::sandbox()): il battello «giocatore» lo comanda il comandante IA e lo spettatore guarda da fuori.
// I testi detti dal battello («ci ha scoperti») passano in terza persona col suo nome
void RegistroMissione::in_terza_persona(EventoMissione& e) const {
    const auto ig = info_.find(e.giocatore);
    if (ig == info_.end()) return;
    const std::string p = nome_classe(ig->second.classe), pe = nome_en(ig->second.classe);
    const std::string c = nome_classe(e.classe), ce = nome_en(e.classe);
    const std::string a = arma_it(e.classe_arma), ae = arma_en(e.classe_arma);
    const auto altra = info_.find(e.altra);
    const std::string b = altra != info_.end() ? nome_classe(altra->second.classe) : "", be = altra != info_.end() ? nome_en(altra->second.classe) : "";
    const bool suo = e.unita == e.giocatore;
    switch (e.tipo) {
        case TipoEvento::scoperta:
            e.testo_it = p + ": primo contatto con " + c + " a " + miglia(e.distanza_m) + " nm";
            e.testo_en = pe + ": first contact with " + ce + " at " + miglia(e.distanza_m) + " nm"; break;
        case TipoEvento::lancio_giocatore:
            e.testo_it = p + " lancia " + a + (b.empty() ? "" : " su " + b);
            e.testo_en = pe + " launches " + ae + (be.empty() ? "" : " at " + be); break;
        case TipoEvento::lancio_nemico:
            if (e.altra != e.giocatore) break;
            e.testo_it = c + " ha lanciato " + a + " contro " + p; e.testo_en = ce + " launched " + ae + " at " + pe; break;
        case TipoEvento::colpo:
            if (!suo) break;
            e.testo_it = p + " colpito da " + a; e.testo_en = pe + " hit by " + ae; break;
        case TipoEvento::affondamento:
            if (!suo) break;
            e.testo_it = p + " è perduto"; e.testo_en = pe + " is lost"; break;
        case TipoEvento::siluro_evitato:
            e.testo_it = p + " evita " + arma_it(e.classe) + " nemico"; e.testo_en = pe + " evades " + arma_en(e.classe) + " fired at it"; break;
        case TipoEvento::fix_radiogoniometria:
            e.testo_it = e.valore > 0 ? "Il nemico ha localizzato " + p + " con la radiogoniometria" : "Il nemico ha rilevato la trasmissione di " + p;
            e.testo_en = e.valore > 0 ? "The enemy fixed " + pe + " by direction finding" : "The enemy got a bearing on " + pe + "'s transmission"; break;
        case TipoEvento::avvistato:
            e.testo_it = c + " ha scoperto " + p; e.testo_en = ce + " has detected " + pe; break;
        case TipoEvento::ping_attivo:
            e.testo_it = p + ": ping del sonar attivo"; e.testo_en = pe + ": active sonar ping"; break;
        case TipoEvento::ping_subito:
            e.testo_it = p + " è sotto il ping del sonar attivo"; e.testo_en = pe + " is being pinged"; break;
        case TipoEvento::transitorio_proprio:
            e.testo_it = p + " cavita"; e.testo_en = pe + " is cavitating"; break;
        case TipoEvento::incaglio:
            e.testo_it = p + " urta il fondo"; e.testo_en = pe + " runs aground"; break;
        case TipoEvento::marcatore_avvistato:
            e.testo_it = "Fumogeno in vista: il nemico ha un datum su " + p;
            e.testo_en = "Smoke marker in sight: the enemy has a datum on " + pe; break;
        case TipoEvento::artiglieria_salva:
            if (e.codice) break;
            e.testo_it = c + " apre il fuoco su " + p; e.testo_en = ce + " opens fire on " + pe; break;
        case TipoEvento::artiglieria_colpo:
            e.testo_it = p + " colpito dall'artiglieria di " + c; e.testo_en = pe + " hit by gunfire from " + ce; break;
        case TipoEvento::collisione:
            e.testo_it = p + (e.livello ? " striscia contro " : " entra in collisione con ") + c;
            e.testo_en = pe + (e.livello ? " scrapes against " : " collides with ") + ce; break;
        case TipoEvento::cavo_impigliato:
            e.testo_it = "Il cavo della schiera di " + c + " nell'elica di " + p; e.testo_en = ce + "'s towed array cable fouled " + pe + "'s propeller"; break;
        case TipoEvento::cavo_toccato:
            e.testo_it = p + " tocca il cavo della schiera di " + c; e.testo_en = pe + " touched " + ce + "'s towed array cable"; break;
        case TipoEvento::acque_territoriali: {
            const std::string n = e.classe_arma.empty() ? "" : " (" + e.classe_arma + ")";
            e.testo_it = p + (e.livello ? " esce dalle acque territoriali" : " entra nelle acque territoriali") + n;
            e.testo_en = pe + (e.livello ? " left territorial waters" : " entered territorial waters") + n; break;
        }
        case TipoEvento::messaggio_ia:
            if (e.codice != 4) break;   // il canale «a noi»
            e.testo_it = c + " riferisce " + b + " a " + p + (e.giusta ? "" : " (perso)");
            e.testo_en = ce + " reports " + be + " to " + pe + (e.giusta ? "" : " (lost)"); break;
        default: break;
    }
}

void RegistroMissione::aggiorna_unita(const Missione& m) {
    const auto* g = m.giocatore();
    for (const auto& [id, u] : m.unita()) {
        auto [it, nuova] = info_.try_emplace(id);
        auto& i = it->second;
        if (nuova) {
            i.classe = u.classe;
            i.categoria = u.categoria;
            i.flag = u.flag;
            i.lato = lato_di(m, u);
            i.roe = m.categoria_roe(u);
            i.arma = (u.flag & 0x20) != 0;
            i.lanciatore = u.lanciatore;
        }
        if (!nuova && (u.flag & 0x18) == 0x18) i.lato = lato_di(m, u);   // M7: col luogo la situazione cambia in partita
        if (!u.distrutta && !u.persa) {
            i.ultima = {id, u.classe, u.categoria, u.pos, u.quota, u.rotta, u.velocita};
            if (i.arma && g && i.lato == Lato::ostile) {
                const Siluro* s = m.siluro(id);
                if (u.bersaglio == g->id || (s && s->bersaglio() == g->id)) i.minaccia = true;
            }
        }
    }
}

double RegistroMissione::quota_collasso(const UnitaClassica& g) const {
    if (quota_collasso_danni_ > 0.0) return quota_collasso_danni_;
    return std::max(1.0, g.quota_max * COLLASSO_SU_COLLAUDO);
}

void RegistroMissione::osserva(const Missione& m, double dt) {
    const auto* g = m.giocatore();
    const double t = m.tempo();
    if (!avviato_) {
        avviato_ = true;
        giocatore_ = g ? g->id : -1;
        terza_persona_ = m.sandbox();
        aggiorna_unita(m);
        componi(aggiungi(m, TipoEvento::inizio));
    }
    // unita' nuove: lanci (armi) e boe
    std::vector<int> nuove;
    for (const auto& [id, u] : m.unita()) if (!info_.count(id)) nuove.push_back(id);
    aggiorna_unita(m);
    for (const int id : nuove) {
        if (m.da_mina(id)) continue;   // lo scoppio di una mina e i suoi siluri: non sono lanci (mina_esplosa)
        if (m.unita().at(id).persa) continue;   // fase 4 B: un lancio dell'IA senza siluri in tubo, che non parte
        auto& i = info_.at(id);
        const auto& u = m.unita().at(id);
        const bool boa = i.classe.find("boa") != std::string::npos || i.categoria == "boa";
        if (boa) continue;
        if (!i.arma) continue;
        const int l = i.lanciatore;
        const auto li = info_.find(l);
        int bersaglio = u.bersaglio;
        if (bersaglio < 0 && l >= 0) if (const auto it = m.unita().find(l); it != m.unita().end()) bersaglio = it->second.bersaglio;
        if (l == giocatore_ && g) {
            auto& e = aggiungi(m, TipoEvento::lancio_giocatore, l, bersaglio, id);
            if (const auto bi = info_.find(bersaglio); bi != info_.end()) e.lato = bi->second.lato;
            if (giudizio_in_attesa_ && std::fabs(giudizio_in_attesa_->tempo - t) <= 2.5) { e.giudizio = giudizio_in_attesa_->esito; giudizio_in_attesa_.reset(); }
            if (const auto imp = m.impostazioni_missile(id)) {   // il missile del modello: la scatola e il modo (I11)
                e.livello = static_cast<int>(imp->taglia) + 1;
                e.codice = imp->modo == ModoLancio::bol ? 1 : 0;
                for (const auto& lm : m.lanci_missili())   // docs/26: i neutrali nella scatola e la distanza vera al lancio
                    if (lm.missile == id) {
                        e.valore = lm.neutrali_scatola;
                        e.distanza_m = lm.distanza_bersaglio_m;
                    }
            }
            ++per_arma_[i.classe].lanci;
            componi(e);
        } else if (li != info_.end() && li->second.lato == Lato::ostile && !(i.flag & 0x80)) {
            auto& e = aggiungi(m, TipoEvento::lancio_nemico, l, bersaglio, id);
            e.noto = g && g->contatti.count(l) && g->contatti.at(l).visibile();
            if (bersaglio == giocatore_) i.minaccia = true;
            componi(e);
        }
        if (l != giocatore_ && li != info_.end() && !(i.flag & 0x80)) {   // fase 5: ogni lancio dell'IA, di qualsiasi parte
            auto& e = aggiungi(m, TipoEvento::lancio_ia, l, info_.count(bersaglio) ? bersaglio : -1, id);
            e.classe_arma = i.classe;
            e.noto = g && g->contatti.count(l) && g->contatti.at(l).visibile();
            componi(e);
        }
    }
    // boe ostili vicine
    if (g)
        for (const auto& [id, u] : m.unita()) {
            const auto& i = info_.at(id);
            if (u.distrutta || u.persa || i.lato != Lato::ostile) continue;
            if (i.classe.find("boa") == std::string::npos && i.categoria != "boa") continue;
            if (boe_vicine_.count(id) || distanza(u.pos, g->pos) > BOA_VICINA_M) continue;
            boe_vicine_.insert(id);
            componi(aggiungi(m, TipoEvento::boa_vicina, id));
        }
    // sonoboe dell'ASW aereo degli aerei che cacciano il giocatore (id delle boe dal milione, per non confonderle)
    if (g && m.con_asw())
        for (const auto& b : m.boe()) {
            const int chiave = 1000000 + b.id;
            if (boe_vicine_.count(chiave) || t < b.t_impatto || t >= b.t_fine || !m.aereo_ostile_al_giocatore(b.lanciatore)) continue;
            const double d = distanza(posizione_boa(b, t), g->pos);
            if (d > BOA_VICINA_M) continue;
            boe_vicine_.insert(chiave);
            auto& e = aggiungi(m, TipoEvento::boa_vicina, b.lanciatore);
            e.distanza_m = d;
            e.classe_arma = b.tipo;
            componi(e);
        }
    // missili senza unita' (MISSILE dell'IA, i lanci del giocatore che non creano un'arma)
    for (; lanci_visti_ < m.lanci_missili().size(); ++lanci_visti_) {
        const auto& lm = m.lanci_missili()[lanci_visti_];
        bool gia = false;
        for (auto it = eventi_.rbegin(); it != eventi_.rend() && it->tempo >= lm.tempo - 2.5; ++it)
            gia = gia || ((it->tipo == TipoEvento::lancio_giocatore || it->tipo == TipoEvento::lancio_nemico) && it->unita == lm.unita);
        if (gia) continue;
        const auto li = info_.find(lm.unita);
        if (lm.unita == giocatore_) {
            auto& e = aggiungi(m, TipoEvento::lancio_giocatore, lm.unita, lm.bersaglio);
            e.classe_arma = lm.arma.empty() ? "missile" : lm.arma;
            ++per_arma_[e.classe_arma].lanci;
            componi(e);
        } else if (li != info_.end() && li->second.lato == Lato::ostile) {
            auto& e = aggiungi(m, TipoEvento::lancio_nemico, lm.unita, lm.bersaglio);
            e.classe_arma = "missile";
            e.noto = g && g->contatti.count(lm.unita);
            componi(e);
        }
    }
    // colpi
    for (; colpi_visti_ < m.colpi().size(); ++colpi_visti_) {
        const Colpo& c = m.colpi()[colpi_visti_];
        if (const auto vi = info_.find(c.vittima); vi != info_.end() && vi->second.arma) continue;   // armi ed esche
        if (c.arma >= 0) armi_a_segno_.insert(c.arma);
        int lanciatore = c.lanciatore;
        if (lanciatore < 0)   // missili di ATTACK_RND senza il +62: chi ha lanciato contro la vittima da ultimo
            for (auto it = eventi_.rbegin(); it != eventi_.rend() && it->tempo >= c.tempo - 300.0; ++it)
                if ((it->tipo == TipoEvento::lancio_nemico || it->tipo == TipoEvento::lancio_giocatore) && it->altra == c.vittima) {
                    lanciatore = it->unita;
                    break;
                }
        if (c.vittima >= 0) ultimo_colpitore_[c.vittima] = lanciatore;
        auto& e = aggiungi(m, TipoEvento::colpo, c.vittima, lanciatore, c.arma);
        e.pos = c.pos; e.quota = c.quota;
        if (e.classe_arma.empty()) e.classe_arma = c.classe_arma.empty() ? (c.tipo == Colpo::Tipo::attacco ? "carica" : "missile") : c.classe_arma;
        e.valore = c.danno;
        e.noto = c.vittima == giocatore_ || lanciatore == giocatore_;
        componi(e);
        if (lanciatore == giocatore_) {
            ++per_arma_[e.classe_arma].colpi;
            colpiti_dal_giocatore_.insert(c.vittima);
            if (e.lato == Lato::neutrale || e.lato == Lato::amico) componi(aggiungi(m, TipoEvento::fuoco_amico, c.vittima, lanciatore, c.arma));
        }
    }
    // siluri del modello: esche ed agganci sul giocatore
    for (; siluri_visti_ < m.eventi_siluri().size(); ++siluri_visti_) {
        const auto& es = m.eventi_siluri()[siluri_visti_];
        if (es.evento.tipo == EventoSiluro::Tipo::aggancio && es.evento.sorgente == giocatore_)
            if (auto it = info_.find(es.siluro); it != info_.end()) it->second.minaccia = it->second.agganciato = true;
        if (es.evento.tipo == EventoSiluro::Tipo::esca_sedotta) {
            auto& e = aggiungi(m, TipoEvento::esca_sedotta, es.siluro, es.evento.sorgente);
            e.pos = es.evento.pos; e.quota = es.evento.quota;
            componi(e);
        }
    }
    // affondamenti e armi finite
    for (const auto& [id, u] : m.unita()) {
        auto& i = info_.at(id);
        if (i.arma) {
            if (!(u.distrutta || u.persa) || finite_.count(id)) continue;
            finite_.insert(id);
            if (i.minaccia && !armi_a_segno_.count(id)) {
                auto& e = aggiungi(m, TipoEvento::siluro_evitato, id, giocatore_);
                e.livello = i.agganciato ? 1 : 0;
                componi(e);
            }
            continue;
        }
        if ((u.affondamento > 0 || u.distrutta) && affondati_.insert(id).second) {
            const auto chi = ultimo_colpitore_.find(id);
            const int lanciatore = chi != ultimo_colpitore_.end() ? chi->second : u.lanciatore;
            auto& e = aggiungi(m, TipoEvento::affondamento, id, lanciatore);
            e.noto = id == giocatore_ || lanciatore == giocatore_ || (g && g->contatti.count(id));
            componi(e);
            if (id == giocatore_) battello_perso_ = true;
        }
    }
    if (g) {
        // contatti del giocatore: scoperte e classificazioni
        for (const auto& [id, c] : g->contatti) {
            const auto ii = info_.find(id);
            if (ii == info_.end()) continue;
            if (ii->second.arma) {   // un'arma sentita: il lancio era noto
                if (scoperti_.insert(id).second)
                    for (auto& e : eventi_) if (e.arma == id) e.noto = true;
                continue;
            }
            // l'esca appena lanciata ha la classe del battello (flag 0xC1 dello script): non e' un contatto scoperto
            if (m.con_acustica() && (ii->second.flag & 0x80))
                if (const auto w = m.unita().find(id); w != m.unita().end() && w->second.lanciatore == giocatore_) continue;
            if (scoperti_.insert(id).second) {
                prima_scoperta_[id] = t;
                componi(aggiungi(m, TipoEvento::scoperta, id));
            }
            const auto classifica = [&](int livello) {
                auto& e = aggiungi(m, TipoEvento::classificazione, id);
                e.livello = livello;
                const std::string& cat = ii->second.categoria;
                e.classe_creduta = livello >= 2 || cat.empty() || cat == "nessuna" ? ii->second.classe : cat;
                e.tempo_scoperta = prima_scoperta_[id];
                componi(e);
            };
            if (c.durata >= DURATA_CATEGORIA && classificati_.insert(id).second) classifica(1);
            if (c.durata >= DURATA_CLASSE && identificati_.insert(id).second) classifica(2);
        }
        // unita' ostili con un contatto sul giocatore: contro-scoperta e tempo in contatto con l'ASW
        bool asw = false;
        for (const auto& [id, u] : m.unita()) {
            const auto& i = info_.at(id);
            if (u.distrutta || u.persa || i.arma || i.lato != Lato::ostile) continue;
            const auto c = u.contatti.find(giocatore_);
            if (c == u.contatti.end()) continue;
            asw = true;
            const bool posizione = c->second.posizione_nota || c->second.qualita >= 800.0;
            if (!posizione) continue;
            const auto ultima = avvistato_.find(id);
            if (ultima == avvistato_.end() || t - ultima->second > AVVISTATO_DI_NUOVO_S) {
                auto& e = aggiungi(m, TipoEvento::avvistato, id, giocatore_);
                e.noto = false;
                e.classe_creduta = c->second.sensore;
                componi(e);
            }
            avvistato_[id] = t;
        }
        if (asw) asw_s_ += dt;
        // armi ostili in acqua: distanza minima
        for (const auto& [id, u] : m.unita()) {
            const auto& i = info_.at(id);
            if (!i.arma || u.distrutta || u.persa || i.lato != Lato::ostile || (u.flag & 0x80) || (u.flag & 3) == 3) continue;
            const double dz = u.quota - g->quota;
            distanza_min_siluro_ = std::min(distanza_min_siluro_, std::sqrt(std::pow(distanza(u.pos, g->pos), 2) + dz * dz));
        }
        if ((g->flag54 & 6) || g->quota <= 0.5) esposto_s_ += dt;
        margine_collasso_ = std::min(margine_collasso_, 1.0 - g->quota / quota_collasso(*g));
    }
    // ping propri (lista degli eventi acustici: 300 passi per il ping)
    for (const auto& a : m.eventi_) {
        if (a.unita != giocatore_ || a.fine <= ultimo_acustico_) continue;
        const double durata = a.fine - t;
        if (durata > 300.0 / 9.0 - 2.5 && durata <= 300.0 / 9.0 + 0.01) componi(aggiungi(m, TipoEvento::ping_attivo, -1));
    }
    for (const auto& a : m.eventi_) if (a.unita == giocatore_) ultimo_acustico_ = std::max(ultimo_acustico_, a.fine);
    // un evento per episodio, col numero dei ping in valore: quelli del sonar e quelli delle boe attive (disassemblatore,
    // 30/10/2026: la boa metteva in valore il rilevamento, e il registro lo leggeva come «359 ping»)
    const auto ping_nell_episodio = [&](double tp, int da) {
        if (episodio_ping_ >= 0 && tp - ultimo_ping_t_ <= EPISODIO_PING_S) {
            auto& e = eventi_[static_cast<std::size_t>(episodio_ping_)];
            e.valore += 1.0;
            componi(e);
        } else {
            auto& e = aggiungi(m, TipoEvento::ping_subito, da);
            e.valore = 1.0;
            episodio_ping_ = static_cast<long>(eventi_.size()) - 1;
            componi(e);
        }
        ultimo_ping_t_ = tp;
    };
    // avvisi del sonar e dell'ESM
    for (; sonar_visti_ < m.messaggi_sonar().size(); ++sonar_visti_) {
        const int k = m.messaggi_sonar()[sonar_visti_].codice;
        if (k == MSG_CI_PINGANO) ping_nell_episodio(m.messaggi_sonar()[sonar_visti_].tempo, -1);
        else if (k == MSG_CAVITIAMO) componi(aggiungi(m, TipoEvento::transitorio_proprio));
    }
    // avvisi dell'ASW aereo
    for (; asw_visti_ < m.messaggi_asw().size(); ++asw_visti_) {
        const auto& x = m.messaggi_asw()[asw_visti_];
        if (x.codice == MSG_ASW_BOA_ATTIVA) { ping_nell_episodio(x.tempo, x.unita); continue; }
        const TipoEvento tipo = x.codice == MSG_ASW_SORVOLO ? TipoEvento::aereo_in_sorvolo
                              : x.codice == MSG_ASW_DIP ? TipoEvento::ping_sonar_immersione
                              : x.codice == MSG_ASW_MARCATORE ? TipoEvento::marcatore_avvistato
                              : x.codice == MSG_ASW_SUS ? TipoEvento::attacco_simulato : TipoEvento::numero;
        if (tipo == TipoEvento::numero) continue;
        auto& e = aggiungi(m, tipo, x.unita);
        e.codice = x.codice;
        if (x.rilevamento) e.valore = *x.rilevamento;
        componi(e);
    }
    // periscopi dell'Enhanced: foto e esposizioni lunghe
    for (; foto_viste_ < m.foto_periscopio().size(); ++foto_viste_) {
        const FotoPeriscopio& f = m.foto_periscopio()[foto_viste_];
        auto& e = aggiungi(m, TipoEvento::foto_periscopio, f.in_vista.empty() ? -1 : f.in_vista.front(), -1, -1, f.tempo);
        for (const int id : f.in_vista)   // per gli obiettivi `foto`
            if (const auto it = m.unita().find(id); it != m.unita().end() && g)
                fotografate_.push_back({id, f.tempo, distanza(g->pos, it->second.pos)});
        for (std::size_t k = 1; k < f.in_vista.size() && k < 6; ++k)
            if (info_.count(f.in_vista[k])) e.pose.push_back(posa(f.in_vista[k]));
        e.valore = f.rilevamento;
        e.livello = f.ingrandimento;
        e.noto = true;   // la foto l'ha scattata il giocatore: la conosce
        componi(e);
    }
    // il sonarista: le sue classificazioni (giuste o no) e gli scambi d'incrocio, che il giocatore non sa
    for (; sonarista_visti_ < m.avvisi_sonarista().size(); ++sonarista_visti_) {
        const auto& a = m.avvisi_sonarista()[sonarista_visti_];
        if (a.codice != 411 || a.sorgente < 0 || !info_.count(a.sorgente)) continue;
        auto& e = aggiungi(m, TipoEvento::classificazione, a.sorgente, -1, -1, a.tempo);
        e.codice = a.codice;
        e.classe_creduta = a.classe;
        e.livello = a.classe.empty() ? 1 : 2;
        e.giusta = !a.errata;
        e.valore = 1.0;
        componi(e);
        // «scoprire» conta anche la classificazione giusta del sonarista (collaudo della libreria, 29/10/2026: col sonarista
        // nuovo da lontano arriva solo il rilevamento, e la sola durata classica non bastava); una sbagliata non conta
        if (e.giusta) classificati_.insert(a.sorgente);
    }
    // artiglieria navale (G2): la salva aperta, il colpo sul giocatore, il primo colpo vicino
    for (; eventi_art_visti_ < m.eventi_artiglieria().size(); ++eventi_art_visti_) {
        const auto& x = m.eventi_artiglieria()[eventi_art_visti_];
        if (x.tipo == TipoEventoArtiglieria::partenza && x.bersaglio != giocatore_ && info_.count(x.tiratore) &&
            salve_ia_.insert({x.tiratore, x.bersaglio}).second) {   // fase 5: una nave su un'altra unita'
            auto& e = aggiungi(m, TipoEvento::artiglieria_ia, x.tiratore, info_.count(x.bersaglio) ? x.bersaglio : -1, -1, x.tempo_s);
            e.valore = x.calibro_mm;
            e.noto = g && g->contatti.count(x.tiratore) && g->contatti.at(x.tiratore).visibile();
            componi(e);
        }
        if (x.tipo != TipoEventoArtiglieria::partenza || x.bersaglio != giocatore_ || !info_.count(x.tiratore)) continue;
        if (!salve_aperte_.insert(x.tiratore).second) continue;
        auto& e = aggiungi(m, TipoEvento::artiglieria_salva, x.tiratore, -1, -1, x.tempo_s);
        e.codice = m.artiglieria_avvertimento() ? 1 : 0;   // l'impatto e' ancora in volo: decide la situazione
        e.valore = x.calibro_mm;
        componi(e);
    }
    for (; impatti_art_visti_ < m.impatti_artiglieria().size(); ++impatti_art_visti_) {
        const auto& x = m.impatti_artiglieria()[impatti_art_visti_];
        const bool a_segno = x.esito == ImpattoArtiglieria::Esito::diretto || x.esito == ImpattoArtiglieria::Esito::schegge;
        if (x.bersaglio != giocatore_ && !x.avvertimento && info_.count(x.bersaglio)) {   // fra unita' dell'IA
            // anche i mancati, col valore 0 come le cariche dell'ATTACK: le statistiche contano un attacco per colpo e
            // il Pk dell'artiglieria e' a segno su sparati
            auto& e = aggiungi(m, TipoEvento::colpo, x.bersaglio, info_.count(x.tiratore) ? x.tiratore : -1, -1, x.tempo_impatto_s);
            e.classe_arma = x.pezzo;
            e.valore = a_segno ? x.calibro_mm : 0.0;
            e.pos = x.punto;
            e.noto = g && g->contatti.count(x.bersaglio) && g->contatti.at(x.bersaglio).visibile();
            componi(e);
            continue;
        }
        if (x.bersaglio != giocatore_ || x.avvertimento || !g) continue;
        if (x.esito == ImpattoArtiglieria::Esito::diretto || x.esito == ImpattoArtiglieria::Esito::schegge) {
            auto& e = aggiungi(m, TipoEvento::artiglieria_colpo, info_.count(x.tiratore) ? x.tiratore : -1, -1, -1, x.tempo_impatto_s);
            e.classe_arma = x.pezzo;
            e.valore = x.calibro_mm;
            e.codice = x.sparo;
            e.pos = x.punto;
            componi(e);
        }
        if (!artiglieria_vicina_ && distanza(x.punto, g->pos) <= ARTIGLIERIA_VICINA_M) {
            artiglieria_vicina_ = true;
            auto& e = aggiungi(m, TipoEvento::artiglieria_vicino, info_.count(x.tiratore) ? x.tiratore : -1, -1, -1, x.tempo_impatto_s);
            e.classe_arma = x.pezzo;
            e.valore = distanza(x.punto, g->pos);
            e.pos = x.punto;
            componi(e);
        }
    }
    // collisioni (dossier 16): gli urti e i passaggi ravvicinati del giocatore
    for (; urti_visti_ < m.urti().size(); ++urti_visti_) {
        const auto& x = m.urti()[urti_visti_];
        if (x.urto.a.unita != giocatore_ && x.urto.b.unita != giocatore_) {   // fase 5: fra due unita' dell'IA
            auto& e = aggiungi(m, TipoEvento::collisione_ia, info_.count(x.urto.a.unita) ? x.urto.a.unita : -1,
                               info_.count(x.urto.b.unita) ? x.urto.b.unita : -1, -1, x.urto.tempo_s);
            e.valore = x.esito.energia_normale_mj;
            e.pos = x.urto.punto;
            e.noto = g && ((g->contatti.count(x.urto.a.unita) && g->contatti.at(x.urto.a.unita).visibile()) ||
                           (g->contatti.count(x.urto.b.unita) && g->contatti.at(x.urto.b.unita).visibile()));
            componi(e);
            continue;
        }
        const int altra = x.urto.a.unita == giocatore_ ? x.urto.b.unita : x.urto.a.unita;
        auto& e = aggiungi(m, TipoEvento::collisione, info_.count(altra) ? altra : -1, -1, -1, x.urto.tempo_s);
        e.valore = x.esito.energia_normale_mj;
        e.livello = x.esito.radente ? 1 : 0;
        e.pos = x.urto.punto;
        componi(e);
    }
    for (; cavi_visti_ < m.incidenti_cavo().size(); ++cavi_visti_) {   // S10
        const auto& x = m.incidenti_cavo()[cavi_visti_];
        if (x.sottomarino != giocatore_) continue;
        auto& e = aggiungi(m, x.impigliato ? TipoEvento::cavo_impigliato : TipoEvento::cavo_toccato, info_.count(x.nave) ? x.nave : -1, -1, -1, x.tempo_s);
        e.pos = x.pos;
        componi(e);
    }
    // docs/26: gli AOB, i ping dell'ecoscandaglio, gli allarmi dell'intercettatore, i campi di mine, le verifiche radio
    for (; aob_visti_ < m.osservazioni_aob().size(); ++aob_visti_) {
        const auto& x = m.osservazioni_aob()[aob_visti_];
        auto& e = aggiungi(m, TipoEvento::osservazione_aob, info_.count(x.contatto) ? x.contatto : -1, -1, -1, x.t);
        e.valore = std::fabs(normalizza_180(x.rotta - x.rotta_vera));
        e.noto = false;
        componi(e);
    }
    if (g)
        if (const QuadroFondale* q = m.quadro_fondale(g->id))
            for (; eco_visti_ < q->sondaggi().size(); ++eco_visti_) componi(aggiungi(m, TipoEvento::ping_ecoscandaglio, giocatore_, -1, -1, q->sondaggi()[eco_visti_].t));
    const auto& conti = m.conti_giocatore();
    for (; allarmi_visti_ < conti.allarmi_siluro.size(); ++allarmi_visti_) {
        auto& e = aggiungi(m, TipoEvento::allarme_siluro, giocatore_, -1, -1, conti.allarmi_siluro[allarmi_visti_].first);
        e.codice = conti.allarmi_siluro[allarmi_visti_].second;
        componi(e);
    }
    for (; mine_att_viste_ < conti.attraversamenti_mine.size(); ++mine_att_viste_)
        componi(aggiungi(m, TipoEvento::attraversamento_mine, giocatore_, -1, -1, conti.attraversamenti_mine[mine_att_viste_]));
    for (; verifiche_viste_ < conti.verifiche_radio.size(); ++verifiche_viste_) {
        auto& e = aggiungi(m, TipoEvento::verifica_radio, giocatore_, -1, -1, conti.verifiche_radio[verifiche_viste_].first);
        e.giusta = conti.verifiche_radio[verifiche_viste_].second;
        componi(e);
    }
    for (; traini_visti_ < m.eventi_traino().size(); ++traini_visti_) {   // il VDS e la schiera delle navi (docs/ricerca/33)
        const auto& x = m.eventi_traino()[traini_visti_];
        auto& e = aggiungi(m, TipoEvento::sonar_traino, info_.count(x.nave) ? x.nave : -1, -1, -1, x.tempo_s);
        e.livello = x.filato ? 1 : 0;
        e.valore = x.quota_m;
        e.classe_arma = x.sistema;
        e.noto = false;
        componi(e);
    }
    // il vetrino di Dio (docs/28): gli interventi applicati, finiti o falliti, solo per lo spettatore
    for (std::size_t k = 0; k < m.stati_interventi().size() && k < m.interventi().size(); ++k) {
        const StatoIntervento& st = m.stati_interventi()[k];
        const Intervento& x = m.interventi()[k];
        // i livelli in ordine: applicato (1), finito (2, non per gli istantanei), fallito (3, solo lui)
        const bool istantaneo = st.finito && st.fine <= st.inizio;
        const int livello = st.fallito ? 3 : st.finito && !istantaneo ? 2 : st.applicato ? 1 : 0;
        int& visto = interventi_visti_[st.id];
        while (visto < livello) {
            visto = st.fallito ? 3 : visto + 1;
            auto& e = aggiungi(m, TipoEvento::intervento, info_.count(x.unita) ? x.unita : -1, -1, -1,
                               visto == 1 ? st.inizio : visto == 2 ? st.fine : m.tempo());
            e.codice = st.id;
            e.livello = visto;
            e.classe_arma = x.tipo;
            e.noto = false;
            if (x.punto) e.pos = *x.punto;
            e.testo_it = visto == 1 ? "Intervento: " + x.riga_it : visto == 2 ? "Fine dell'intervento: " + x.riga_it
                                                                             : "Intervento non applicato (" + st.motivo_it + "): " + x.riga_it;
            e.testo_en = visto == 1 ? "Intervention: " + x.riga_en : visto == 2 ? "End of intervention: " + x.riga_en
                                                                               : "Intervention not applied (" + st.motivo_en + "): " + x.riga_en;
            const std::string it = e.testo_it, en = e.testo_en;
            componi(e);
            e.testo_it = it;
            e.testo_en = en;
        }
    }
    for (; soffiate_viste_ < m.soffiate_giocatore().size(); ++soffiate_viste_) {   // la soffiata d'emergenza del giocatore
        auto& e = aggiungi(m, TipoEvento::emersione_emergenza, giocatore_, -1, -1, m.soffiate_giocatore()[soffiate_viste_]);
        if (const auto* g = m.giocatore()) e.pos = g->pos;
        componi(e);
    }
    for (; posti_visti_ < m.cambi_posti().size(); ++posti_visti_) {   // i posti di combattimento del giocatore
        const auto& x = m.cambi_posti()[posti_visti_];
        auto& e = aggiungi(m, TipoEvento::posti_di_combattimento, giocatore_, -1, -1, x.t);
        e.livello = x.posti ? 1 : 0;
        e.classe_arma = x.causa;
        e.noto = true;
        componi(e);
    }
    for (; punti_nave_ia_visti_ < m.punti_nave_ia().size(); ++punti_nave_ia_visti_) {   // la navigazione incerta dell'IA
        const auto& x = m.punti_nave_ia()[punti_nave_ia_visti_];
        auto& e = aggiungi(m, TipoEvento::punto_nave_ia, info_.count(x.unita) ? x.unita : -1, -1, -1, x.t);
        e.valore = x.spostamento_m;
        e.distanza_m = x.sigma_dopo_m;
        e.classe_arma = x.tipo;
        e.noto = false;
        componi(e);
    }
    for (; luci_visti_ < m.cambi_luci().size(); ++luci_visti_) {   // le luci di navigazione (docs/27)
        const auto& x = m.cambi_luci()[luci_visti_];
        auto& e = aggiungi(m, TipoEvento::luci, info_.count(x.unita) ? x.unita : -1, -1, -1, x.tempo);
        e.valore = static_cast<double>(static_cast<int>(x.stato));
        componi(e);
    }
    for (; telemetro_visti_ < m.misure_telemetro().size(); ++telemetro_visti_) {   // le letture del telemetro (docs/25 §3.3)
        const auto& x = m.misure_telemetro()[telemetro_visti_];
        auto& e = aggiungi(m, TipoEvento::misura_telemetro, info_.count(x.contatto) ? x.contatto : -1, -1, -1, x.t);
        e.valore = x.letta_m;
        e.distanza_m = x.vera_m;
        e.misura_m = x.altezza_m;
        componi(e);
    }
    for (; classificazioni_ia_viste_ < m.classificazioni_ia().size(); ++classificazioni_ia_viste_) {   // fase 2: i sonaristi dell'IA
        const auto& x = m.classificazioni_ia()[classificazioni_ia_viste_];
        if (!info_.count(x.unita)) continue;
        auto& e = aggiungi(m, TipoEvento::classificazione_ia, x.unita, info_.count(x.sorgente) ? x.sorgente : -1, -1, x.tempo_s);
        e.classe_creduta = x.classe;
        e.giusta = !x.errata;
        e.livello = static_cast<int>(x.confidenza);
        e.visiva = x.visiva;
        e.noto = false;
        componi(e);
    }
    for (; cambi_allerta_visti_ < m.cambi_allerta().size(); ++cambi_allerta_visti_) {   // fase 3: le forze
        const auto& x = m.cambi_allerta()[cambi_allerta_visti_];
        auto& e = aggiungi(m, TipoEvento::allerta_forza, info_.count(x.unita) ? x.unita : -1, -1, -1, x.t);
        e.codice = x.lato;
        e.valore = x.da;
        e.livello = x.a;
        e.classe_arma = x.causa;
        e.noto = false;
        componi(e);
    }
    for (; messaggi_ia_visti_ < m.messaggi_ia().size(); ++messaggi_ia_visti_) {
        const auto& x = m.messaggi_ia()[messaggi_ia_visti_];
        auto& e = aggiungi(m, TipoEvento::messaggio_ia, info_.count(x.da) ? x.da : -1, info_.count(x.su) ? x.su : -1, -1, x.t_partenza);
        e.codice = static_cast<int>(x.canale);
        e.valore = x.t_arrivo - x.t_partenza;
        e.giusta = !x.perso;
        e.noto = x.canale == Missione::MessaggioIa::Canale::giocatore;
        componi(e);
    }
    for (; fuochi_amici_visti_ < m.fuoco_amico_ia().size(); ++fuochi_amici_visti_) {
        const auto& x = m.fuoco_amico_ia()[fuochi_amici_visti_];
        auto& e = aggiungi(m, TipoEvento::fuoco_amico_ia, info_.count(x.da) ? x.da : -1, info_.count(x.su) ? x.su : -1, -1, x.t);
        e.classe_arma = x.causa;
        e.noto = x.su == giocatore_;
        componi(e);
    }
    for (; esche_ia_viste_ < m.esche_ia().size(); ++esche_ia_viste_) {   // fase 4 A
        const auto& x = m.esche_ia()[esche_ia_viste_];
        if (!info_.count(x.unita)) continue;
        auto& e = aggiungi(m, TipoEvento::esca_ia, x.unita, -1, -1, x.t);
        e.arma = x.esca;
        e.classe_arma = x.tipo;
        e.noto = g && g->contatti.count(x.unita) && g->contatti.at(x.unita).visibile();
        componi(e);
    }
    for (; tma_ia_viste_ < m.eventi_tma_ia().size(); ++tma_ia_viste_) {
        const auto& x = m.eventi_tma_ia()[tma_ia_viste_];
        if (!info_.count(x.unita)) continue;
        auto& e = aggiungi(m, TipoEvento::tma_ia, x.unita, info_.count(x.bersaglio) ? x.bersaglio : -1, -1, x.t);
        e.livello = x.gambe;
        e.codice = static_cast<int>(x.tipo);
        e.noto = false;
        componi(e);
    }
    for (; note_comandante_viste_ < m.note_comandante().size(); ++note_comandante_viste_) {   // sandbox
        const auto& x = m.note_comandante()[note_comandante_viste_];
        auto& e = aggiungi(m, TipoEvento::comandante_ia, giocatore_, info_.count(x.bersaglio) ? x.bersaglio : -1, -1, x.t);
        e.classe_arma = x.fase;
        e.classe_creduta = x.motivo;
        componi(e);
    }
    for (; punti_nave_visti_ < m.punti_nave().size(); ++punti_nave_visti_) {   // navigazione incerta
        const auto& x = m.punti_nave()[punti_nave_visti_];
        auto& e = aggiungi(m, TipoEvento::punto_nave, giocatore_, -1, -1, x.t);
        e.classe_arma = x.tipo;
        e.giusta = x.fatto;
        e.valore = x.spostamento_m;
        e.distanza_m = x.sigma_dopo_m;
        e.codice = static_cast<int>(x.motivo);
        componi(e);
    }
    for (; incsea_viste_ < m.incsea().size(); ++incsea_viste_) {
        const auto& x = m.incsea()[incsea_viste_];
        if (!info_.count(x.unita)) continue;
        auto& e = aggiungi(m, TipoEvento::incsea, x.unita, info_.count(x.altra) ? x.altra : -1, -1, x.t);
        e.valore = x.distanza_m;
        e.noto = x.unita == giocatore_ || x.altra == giocatore_;
        componi(e);
    }
    for (; reazioni_ia_viste_ < m.reazioni_ia().size(); ++reazioni_ia_viste_) {
        const auto& x = m.reazioni_ia()[reazioni_ia_viste_];
        const bool rete = x.unita < 0 && x.evento == "radiogoniometria";   // la rete strategica localizza: nessuna unita'
        if (!rete && !info_.count(x.unita)) continue;
        auto& e = aggiungi(m, TipoEvento::reazione_ia, rete ? -1 : x.unita, info_.count(x.causa) ? x.causa : -1, -1, x.t);
        e.classe_arma = x.reazione;
        e.classe_creduta = x.ruolo;
        e.codice = x.evento == "scoperto" ? 0 : x.evento == "attaccato" ? 1 : x.evento == "affondato" ? 2 : 3;   // 3: dallo script
        e.noto = g && g->contatti.count(x.unita) && g->contatti.at(x.unita).visibile();
        componi(e);
    }
    for (; ritirate_ia_viste_ < m.ritirate_ia().size(); ++ritirate_ia_viste_) {
        const auto& x = m.ritirate_ia()[ritirate_ia_viste_];
        if (!info_.count(x.unita)) continue;
        auto& e = aggiungi(m, TipoEvento::ritirata_ia, x.unita, info_.count(x.da) ? x.da : -1, -1, x.t);
        e.classe_arma = x.causa;
        e.noto = g && g->contatti.count(x.unita) && g->contatti.at(x.unita).visibile();
        componi(e);
    }
    for (; provocazioni_viste_ < m.provocazioni().size(); ++provocazioni_viste_) {
        const auto& x = m.provocazioni()[provocazioni_viste_];
        auto& e = aggiungi(m, TipoEvento::provocazione, info_.count(x.unita) ? x.unita : -1, info_.count(x.bersaglio) ? x.bersaglio : -1, -1, x.t);
        e.codice = x.colpito;
        e.livello = x.provocatore;
        e.classe_arma = x.causa;
        e.noto = x.unita == giocatore_ || x.bersaglio == giocatore_;
        componi(e);
    }
    // fase 5 (lo spettatore): scoperte, ingaggi ed evasioni dell'IA, una volta al secondo
    if ((cadenza_ia_ += dt) >= 1.0) {
        cadenza_ia_ = 0.0;
        for (const auto& [id, u] : m.unita()) {
            if (id == giocatore_ || u.distrutta || u.persa || (u.flag & 0x20) || !info_.count(id)) continue;
            for (const auto& [cid, c] : u.contatti) {
                const auto b = m.unita().find(cid);
                // scoperta: un contatto che l'IA puo' usare (traccia solida, cioe' riferita dal suo sonarista, o posizione)
                if (b == m.unita().end() || !(c.qualita >= 800.0 || c.posizione_nota) || (b->second.flag & 0x20) || !info_.count(cid)) continue;
                if (((u.flag ^ b->second.flag) & 0x18) != 0x18 || !scoperte_ia_.insert({id, cid}).second) continue;
                auto& e = aggiungi(m, TipoEvento::scoperta_ia, id, cid, -1, t);
                e.noto = false;   // decisioni interne dell'IA: nel quadro del giocatore non ci sono
                componi(e);
            }
            const int agg = u.agganciato;
            if (const auto it = agganci_ia_.find(id); agg >= 0 && (it == agganci_ia_.end() || it->second != agg)) {
                const auto b = m.unita().find(agg);
                if (b != m.unita().end() && info_.count(agg) && ((u.flag ^ b->second.flag) & 0x18) == 0x18) {
                    auto& e = aggiungi(m, TipoEvento::ingaggio_ia, id, agg, -1, t);
                    e.noto = false;
                    // in guerra un attacco; senza guerra (crisi, pace) l'aggancio e' senza fuoco: allarme, ping, inseguimento
                    // (principale, 30/10/2026: «Krivak2 attacca Los Angeles» in crisi sembrava un attacco nel debriefing)
                    e.codice = m.situazione_fra(u.flag & 0x18, b->second.flag & 0x18) == Situazione::guerra ? 1 : 0;
                    componi(e);
                }
            }
            agganci_ia_[id] = agg;
            if (const int n = m.manovre_evasione(id); n > evasioni_ia_[id]) {
                evasioni_ia_[id] = n;
                auto& e = aggiungi(m, TipoEvento::evasione_ia, id, -1, -1, t);
                e.valore = n;
                e.noto = false;
                componi(e);
            }
        }
    }
    for (; mine_scoperte_viste_ < m.mine_scoperte().size(); ++mine_scoperte_viste_) {   // S06: il BQS-15
        const auto& x = m.mine_scoperte()[mine_scoperte_viste_];
        auto& e = aggiungi(m, TipoEvento::mina_scoperta, -1, -1, -1, x.scoperta_t);
        e.pos = x.pos;
        e.quota = x.quota;
        e.valore = x.distanza_m;
        e.livello = x.ormeggiata ? 1 : 0;
        e.noto = true;
        for (const auto& mm : m.campo_minato().mine()) if (mm.id == x.mina) e.classe_arma = mm.tipo;
        componi(e);
    }
    for (; esercitazioni_viste_ < m.colpi_esercitazione().size(); ++esercitazioni_viste_) {   // schede I03 e S01
        const auto& x = m.colpi_esercitazione()[esercitazioni_viste_];
        auto& e = aggiungi(m, TipoEvento::colpo_esercitazione, info_.count(x.vittima) ? x.vittima : -1,
                           info_.count(x.lanciatore) ? x.lanciatore : -1, -1, x.tempo_s);
        e.classe_arma = x.classe_arma;
        e.pos = x.pos;
        e.noto = x.lanciatore == giocatore_ || x.vittima == giocatore_;
        if (x.lanciatore == giocatore_) colpiti_esercitazione_.insert(x.vittima);
        componi(e);
    }
    for (; acque_viste_ < m.passaggi_acque().size(); ++acque_viste_) {   // acque territoriali dichiarate
        const auto& x = m.passaggi_acque()[acque_viste_];
        auto& e = aggiungi(m, TipoEvento::acque_territoriali, -1, -1, -1, x.tempo_s);
        e.codice = x.acque;
        e.livello = x.entrata ? 0 : 1;
        if (x.acque >= 0 && x.acque < static_cast<int>(m.acque_territoriali().size())) e.classe_arma = m.acque_territoriali()[static_cast<std::size_t>(x.acque)].nome;
        if (g) e.pos = g->pos;
        e.noto = true;
        componi(e);
    }
    for (; avarie_viste_ < m.avarie_iniziate().size(); ++avarie_viste_) {   // M6
        const auto& [quando, id] = m.avarie_iniziate()[avarie_viste_];
        if (!info_.count(id)) continue;
        auto& e = aggiungi(m, TipoEvento::avaria_ia, id, -1, -1, quando);
        e.noto = g && g->contatti.count(id) && g->contatti.at(id).visibile();
        componi(e);
    }
    for (; rimorchi_visti_ < m.eventi_rimorchio().size(); ++rimorchi_visti_) {
        const auto& x = m.eventi_rimorchio()[rimorchi_visti_];
        if (!info_.count(x.rimorchiato)) continue;
        auto& e = aggiungi(m, TipoEvento::rimorchio, x.rimorchiato, info_.count(x.rimorchiatore) ? x.rimorchiatore : -1, -1, x.tempo_s);
        e.livello = x.inizio ? 0 : 1;
        e.noto = g && g->contatti.count(x.rimorchiato) && g->contatti.at(x.rimorchiato).visibile();
        componi(e);
    }
    for (; forzate_viste_ < m.emersioni_forzate().size(); ++forzate_viste_) {   // M6
        const auto& [quando, id] = m.emersioni_forzate()[forzate_viste_];
        if (!info_.count(id)) continue;
        auto& e = aggiungi(m, TipoEvento::emersione_forzata, id, -1, -1, quando);
        e.noto = g && g->contatti.count(id) && g->contatti.at(id).visibile();
        componi(e);
    }
    for (; quasi_visti_ < m.quasi_collisioni().size(); ++quasi_visti_) {
        const auto& x = m.quasi_collisioni()[quasi_visti_];
        if (x.a != giocatore_) continue;
        auto& e = aggiungi(m, TipoEvento::quasi_collisione, info_.count(x.b) ? x.b : -1, -1, -1, x.tempo_s);
        e.valore = x.distanza_m;
        e.codice = x.a_sapeva ? 1 : 0;
        e.noto = x.a_sapeva;
        e.pos = x.pos;
        componi(e);
    }
    for (; scambi_visti_ < m.scambi_tracce().size(); ++scambi_visti_) {
        const auto& x = m.scambi_tracce()[scambi_visti_];
        if (!info_.count(x.a)) continue;
        auto& e = aggiungi(m, TipoEvento::scambio_tracce, x.a, info_.count(x.da) ? x.da : -1, -1, x.tempo);
        e.livello = x.numero;
        e.noto = false;
        componi(e);
    }
    for (; lunghe_viste_ < m.esposizioni_lunghe().size(); ++lunghe_viste_) {
        const auto& x = m.esposizioni_lunghe()[lunghe_viste_];
        auto& e = aggiungi(m, TipoEvento::esposizione_lunga, -1, -1, -1, x.fine);
        e.valore = x.durata();
        componi(e);
    }
    for (; esm_visti_ < m.messaggi_esm().size(); ++esm_visti_) {
        const int k = m.messaggi_esm()[esm_visti_].codice;
        if (k != MSG_ESM_RISCHIO && k != MSG_ESM_ILLUMINATI) continue;
        const Intercettazione* peggiore = nullptr;
        for (const auto& x : m.intercettazioni())
            if (!peggiore || static_cast<int>(x.rischio) > static_cast<int>(peggiore->rischio)) peggiore = &x;
        auto& e = aggiungi(m, TipoEvento::esm_rischio_alto, peggiore ? peggiore->unita : -1);
        e.codice = k;
        e.livello = peggiore && peggiore->asw_aereo ? 1 : 0;
        componi(e);
    }
    // messaggi della missione: urti col fondo
    for (; msg_visti_ < m.messaggi().size(); ++msg_visti_) {
        const int k = m.messaggi()[msg_visti_].codice;
        if ((k == MSG_FONDO_DANNI || k == MSG_URTO_FATALE) && m.messaggi()[msg_visti_].unita == giocatore_) {
            incaglio_ = true;
            componi(aggiungi(m, TipoEvento::incaglio));
        }
    }
    // obiettivi di posizione: il cerchio, o la distanza da un'unita' in moto (unita' o classi)
    if (g)
        for (std::size_t k = 0; k < definizioni_.size(); ++k) {
            const auto& d = definizioni_[k];
            if (d.o.tipo != Obiettivo::Tipo::raggiungere || raggiunti_.count(k)) continue;
            if (d.entro_s >= 0.0 && t > d.entro_s) continue;
            if (d.centro) {
                if (distanza(g->pos, *d.centro) <= d.raggio) raggiunti_.insert(k);
                continue;
            }
            if (d.distanza_m < 0.0) continue;
            for (const auto& [id, u] : m.unita()) {
                if (id == g->id || u.distrutta || u.persa) continue;
                const bool sua = std::find(d.unita.begin(), d.unita.end(), id) != d.unita.end() ||
                                 std::find(d.classi.begin(), d.classi.end(), u.classe) != d.classi.end();
                if (sua && distanza(g->pos, u.pos) <= d.distanza_m) { raggiunti_.insert(k); break; }
            }
        }
    // zone inviolate: le unita' della parte (navi e sottomarini) nel cerchio, oltre la tolleranza
    for (std::size_t k = 0; k < definizioni_.size(); ++k) {
        const auto& d = definizioni_[k];
        if (d.o.tipo != Obiettivo::Tipo::zona_inviolata || !d.centro || violate_.count(k)) continue;
        if (d.entro_s >= 0.0 && t > d.entro_s) continue;
        for (const auto& [id, u] : m.unita()) {
            if (u.giocatore || u.distrutta || u.persa || (u.flag & 0x20) || (u.flag & 3) == 3) continue;
            if (u.categoria != "nave" && u.categoria != "sottomarino") continue;
            const auto i = info_.find(id);
            if (i == info_.end() || i->second.lato != d.parte) continue;
            if (!d.unita.empty() || !d.classi.empty()) {
                const bool sua = std::find(d.unita.begin(), d.unita.end(), id) != d.unita.end() ||
                                 std::find(d.classi.begin(), d.classi.end(), u.classe) != d.classi.end();
                if (!sua) continue;
            }
            if (distanza(u.pos, *d.centro) > d.raggio) continue;
            if ((dentro_zona_s_[{k, id}] += dt) > 60.0 * d.minuti) { violate_.insert(k); break; }
        }
    }
    if (m.esito() && !fine_) {
        fine_ = true;
        auto& e = aggiungi(m, TipoEvento::fine);
        e.codice = *m.esito();
        componi(e);
    }
}

void RegistroMissione::radio(const Missione& m, const EventoRadio& e) {
    if (!avviato_) return;
    const SalaRadio* r = m.radio();
    switch (e.tipo) {
        case EventoRadio::Tipo::ricevuto: {
            auto& x = aggiungi(m, TipoEvento::messaggio_ricevuto);
            x.codice = e.serie;
            x.livello = static_cast<int>(e.precedenza);
            componi(x);
            const MessaggioRadio* mr = r ? r->messaggio(e.serie) : nullptr;
            if (mr && !mr->id.empty())
                for (const auto& d : definizioni_)
                    if (d.o.da_radio == mr->id && ordini_ricevuti_.insert(mr->id).second) {
                        serie_ordini_[mr->id] = e.serie;
                        auto& o = aggiungi(m, TipoEvento::ordine_ricevuto);
                        o.codice = e.serie;
                        o.testo_it = "Nuovo ordine: " + d.o.testo_it;
                        o.testo_en = "New orders: " + d.o.testo_en;
                        componi(o);
                    }
            break;
        }
        case EventoRadio::Tipo::ordini_cambiati:   // segue il ricevuto della stessa serie
            for (auto it = eventi_.rbegin(); it != eventi_.rend(); ++it)
                if (it->tipo == TipoEvento::messaggio_ricevuto && it->codice == e.serie) { it->valore = 1.0; componi(*it); break; }
            break;
        case EventoRadio::Tipo::perso: {
            auto& x = aggiungi(m, TipoEvento::messaggio_perso);
            x.codice = e.serie;
            const MessaggioRadio* mr = r ? r->messaggio(e.serie) : nullptr;
            x.valore = mr && (!mr->ordine.empty() || mr->roe || mr->zone) ? 1.0 : 0.0;
            componi(x);
            break;
        }
        case EventoRadio::Tipo::scaduto: {
            auto& x = aggiungi(m, TipoEvento::compito_scaduto);
            x.codice = e.serie;
            componi(x);
            break;
        }
        case EventoRadio::Tipo::sessione_mancata: componi(aggiungi(m, TipoEvento::sessione_mancata)); break;
        default: break;
    }
}

void RegistroMissione::giudizio(const Missione& m, const GiudizioRoe& r) {
    if (!avviato_) return;
    giudizi_.push_back(r);
    // il lancio del giocatore: gia' registrato in questo passo o in arrivo al prossimo
    bool dato = false;
    for (auto it = eventi_.rbegin(); it != eventi_.rend() && it->tempo >= r.tempo - 2.5; ++it)
        if (it->tipo == TipoEvento::lancio_giocatore && !it->giudizio) { it->giudizio = r.esito; dato = true; break; }
    if (!dato) giudizio_in_attesa_ = r;
    if (r.esito == GiudizioRoe::Esito::infrazione || r.esito == GiudizioRoe::Esito::infrazione_non_copiato) {
        auto& e = aggiungi(m, TipoEvento::infrazione_roe, giocatore_, r.bersaglio);
        if (const auto bi = info_.find(r.bersaglio); bi != info_.end()) { e.lato = bi->second.lato; e.categoria_roe = r.categoria; }
        e.giudizio = r.esito;
        e.livello = r.esito == GiudizioRoe::Esito::infrazione && !r.scambiato() ? 1 : 0;   // consapevole
        if (!r.zona.empty()) e.classe_creduta = r.zona;
        componi(e);
    }
}

void RegistroMissione::ingresso(const Missione& m, const std::string& zona) {
    if (!avviato_) return;
    ingressi_.push_back({m.tempo(), zona});
    auto& e = aggiungi(m, TipoEvento::ingresso_zona);
    e.classe_creduta = zona;
    componi(e);
}

void RegistroMissione::lancio_senza_arma(const Missione& m, const std::string& classe_arma) {
    if (!avviato_) osserva(m, 0.0);
    auto& e = aggiungi(m, TipoEvento::lancio_giocatore, giocatore_);
    e.classe_arma = classe_arma;
    ++per_arma_[classe_arma].lanci;
    componi(e);
}

void RegistroMissione::esito_tlam(const Missione& m, const std::string& missione, const std::string& bersaglio, bool arrivato,
                                  double volo_s, Vec2 pos, bool in_finestra) {
    if (!avviato_) osserva(m, 0.0);
    auto& e = aggiungi(m, TipoEvento::esito_tlam, giocatore_);
    e.classe_arma = missione;
    e.classe_creduta = bersaglio;
    e.giusta = arrivato;
    e.valore = volo_s;
    e.pos = pos;
    e.codice = in_finestra ? 1 : 0;
    e.noto = false;   // il giocatore lo sa solo dal BDA alla radio (messaggio_ricevuto), mezz'ora dopo
    componi(e);
}

void RegistroMissione::fine_missile(const Missione& m, int lanciatore, int missile, const std::string& classe, bool guasto,
                                    double volo_s, Vec2 pos) {
    if (!avviato_) osserva(m, 0.0);
    auto& e = aggiungi(m, TipoEvento::fine_missile, lanciatore, -1, missile);
    e.classe_arma = classe;
    e.codice = guasto ? 1 : 0;
    e.valore = volo_s;
    e.pos = pos;
    e.noto = false;
    componi(e);
}

void RegistroMissione::missile_abbattuto(const Missione& m, int lanciatore, int missile, const std::string& classe, int nave,
                                         Vec2 pos, double quota_m, double esplosivo_kg) {
    if (!avviato_) osserva(m, 0.0);
    auto& e = aggiungi(m, TipoEvento::fine_missile, lanciatore, nave, missile);
    e.classe_arma = classe;
    e.codice = 2;
    e.valore = esplosivo_kg;
    e.pos = pos;
    e.quota = quota_m;
    e.noto = false;
    componi(e);
}

void RegistroMissione::mina_posata(const Missione& m, int posatore, const std::string& tipo, Vec2 pos, double quota,
                                   bool inerte) {
    if (!avviato_) osserva(m, 0.0);
    auto& e = aggiungi(m, TipoEvento::mina_posata, posatore);
    e.classe_arma = tipo;
    e.pos = pos;
    e.quota = quota;
    e.livello = inerte ? 1 : 0;
    e.noto = posatore == giocatore_;
    componi(e);
}

void RegistroMissione::mina_esplosa(const Missione& m, int posatore, int bersaglio, const std::string& tipo, Vec2 pos,
                                    double quota, bool siluro, double carica_kg) {
    if (!avviato_) osserva(m, 0.0);
    auto& e = aggiungi(m, TipoEvento::mina_esplosa, bersaglio, posatore);
    e.classe_arma = tipo;
    e.pos = pos;
    e.quota = quota;
    e.livello = siluro ? 1 : 0;
    e.valore = carica_kg;
    e.noto = posatore == giocatore_ || bersaglio == giocatore_;
    componi(e);
}

void RegistroMissione::trasmissione(const Missione& m, TipoTrasmissione t, const EsitoTrasmissione& r, Vec2 da, bool boa) {
    if (!avviato_ || !r.fatta) return;
    if (t == TipoTrasmissione::contatto || t == TipoTrasmissione::attacco || t == TipoTrasmissione::situazione) ++rapporti_inviati_;
    auto& e = aggiungi(m, TipoEvento::trasmissione);
    e.codice = static_cast<int>(t);
    e.livello = boa ? 1 : 0;
    e.pos = da;
    componi(e);
    if (r.localizzato) {
        auto& f = aggiungi(m, TipoEvento::fix_radiogoniometria);
        f.valore = r.errore_m;   // 0: solo il rilevamento (ESM vicino)
        f.pos = da;
        f.noto = false;
        componi(f);
    }
}

void RegistroMissione::classifica(const Missione& m, int giocatore, int contatto, const std::string& classe, bool visiva) {
    if (!avviato_ || giocatore != giocatore_) return;
    const auto ii = info_.find(contatto);
    if (ii == info_.end()) return;
    auto& e = aggiungi(m, TipoEvento::classificazione, contatto);
    e.livello = 2;
    e.classe_creduta = classe;
    e.giusta = classe == ii->second.classe || classe == ii->second.categoria;
    e.valore = 1.0;   // del giocatore
    e.visiva = visiva;
    if (visiva) e.codice = CODICE_VISIVA;   // anche nella traccia del replay, che non ha il campo visiva (statistiche)
    const auto p = prima_scoperta_.find(contatto);
    e.tempo_scoperta = p != prima_scoperta_.end() ? p->second : -1.0;
    componi(e);
    if (visiva && e.giusta) classificati_.insert(contatto);   // «scoprire»: anche l'identificazione visiva giusta col libretto
    // «scoprire»: anche la classificazione giusta fatta a mano dalla stazione, ma solo al primo tentativo su quel contatto
    // (trailer-1, 31/10/2026: lo Sturgeon silenzioso della S24 che il sonarista non classifica mai); provare le classi una
    // dopo l'altra finche' una e' giusta non vale
    if (!visiva && classificati_a_mano_.insert(contatto).second && e.giusta) classificati_.insert(contatto);
}

void RegistroMissione::danni(const Missione& m, const DanniSottomarini* sub, const DanniMissione*) {
    if (!avviato_ || !sub) return;
    // emersioni d'emergenza dell'IA
    for (; emersioni_viste_ < sub->emersioni().size(); ++emersioni_viste_) {
        const auto& em = sub->emersioni()[emersioni_viste_];
        if (em.unita == giocatore_) continue;
        auto& e = aggiungi(m, TipoEvento::emersione_emergenza, em.unita, -1, -1, em.inizio);
        e.pos = em.pos;
        componi(e);
    }
    // rapporti del giocatore
    const auto& rapporti = sub->rapporti(giocatore_);
    for (; rapporti_visti_ < rapporti.size(); ++rapporti_visti_) {
        const EventoBattello& r = rapporti[rapporti_visti_];
        using T = EventoBattello::Tipo;
        TipoEvento tipo;
        if (r.tipo == T::incendio_spento || r.tipo == T::falla_tamponata) { tipo = TipoEvento::danno_domato; ++danni_domati_; }
        else if (r.tipo == T::scram) { if (scram_) continue; scram_ = true; tipo = TipoEvento::scram; }
        else if (gravita(r) >= 0.0) tipo = TipoEvento::danno_subito;
        else continue;
        auto& e = aggiungi(m, tipo, -1, -1, -1, r.tempo);
        e.valore = std::max(0.0, gravita(r));
        e.codice = static_cast<int>(r.tipo);
        e.testo_it = r.testo;
        e.testo_en = std::string{nome_tipo_battello(r.tipo)};
        if (r.tipo == T::caduti) e.livello = r.uomini;
        danno_massimo_ = std::max(danno_massimo_, e.valore);
        componi(e);
    }
    const SottomarinoColpito* s = sub->sottomarino(giocatore_);
    if (s && s->danni) {
        quota_collasso_danni_ = s->danni->quota_collasso_m();
        danno_massimo_ = std::max(danno_massimo_, s->danni->danno_scafo());
        if (s->distrutto) { battello_perso_ = true; equipaggio_salvo_ = s->emerso_danneggiato; }
    }
    // camera di lancio: il compartimento abitabile piu' a prora (i tubi del 688 e dei sovietici sono li')
    if (const auto* comp = sub->compartimenti(giocatore_); comp && !camera_persa_) {
        const CompartimentoBattello* prora = nullptr;
        for (const auto& c : *comp)
            if (c.uso != "zavorra" && (!prora || c.xc > prora->xc)) prora = &c;
        if (prora && (prora->distrutto || prora->imploso || (prora->volume > 0.0 && prora->acqua / prora->volume > 0.5))) {
            camera_persa_ = true;
            componi(aggiungi(m, TipoEvento::camera_di_lancio_persa));
        }
    }
}

// --- consultazione ---

const EventoMissione* RegistroMissione::evento(int id) const {
    for (const auto& e : eventi_) if (e.id == id) return &e;
    return nullptr;
}

std::vector<const EventoMissione*> RegistroMissione::momenti_salienti(int n, double spazio_s) const {
    const auto obbligato = [](const EventoMissione& e) {
        return e.tipo == TipoEvento::fine || e.tipo == TipoEvento::colpo || e.tipo == TipoEvento::affondamento ||
               (e.tipo == TipoEvento::danno_subito && e.valore >= 0.6) ||
               (e.tipo == TipoEvento::scoperta && e.categoria == "sottomarino") ||
               e.tipo == TipoEvento::foto_periscopio;   // scelta dal giocatore
    };
    std::vector<const EventoMissione*> tutti;
    for (const auto& e : eventi_) if (e.tipo != TipoEvento::inizio) tutti.push_back(&e);
    std::stable_sort(tutti.begin(), tutti.end(), [&](const EventoMissione* a, const EventoMissione* b) {
        const bool oa = obbligato(*a), ob = obbligato(*b);
        if (oa != ob) return oa;
        if (a->interesse != b->interesse) return a->interesse > b->interesse;
        return a->id < b->id;
    });
    std::vector<const EventoMissione*> scelti;
    for (const auto* e : tutti) {
        if (static_cast<int>(scelti.size()) >= n) break;
        bool vicino = false;
        if (!obbligato(*e))
            for (const auto* s : scelti) vicino = vicino || std::fabs(s->tempo - e->tempo) < spazio_s;
        if (!vicino) scelti.push_back(e);
    }
    std::sort(scelti.begin(), scelti.end(), [](const EventoMissione* a, const EventoMissione* b) {
        return a->tempo != b->tempo ? a->tempo < b->tempo : a->id < b->id;
    });
    return scelti;
}

EsitoDettagliato RegistroMissione::esito(const Missione& m) const {
    using C = EsitoDettagliato::Categoria;
    using S = Obiettivo::Stato;
    using T = Obiettivo::Tipo;
    EsitoDettagliato r;
    r.codice = m.esito();
    if (r.codice) r.testo_it = m.testo_esito(*r.codice);
    r.durata_s = m.tempo();
    const bool finita = r.codice.has_value() || battello_perso_;

    // conti dagli eventi
    std::vector<double> anticipi;
    ContaSpari spari;
    for (const auto& e : eventi_) {
        switch (e.tipo) {
            case TipoEvento::lancio_giocatore: ++r.lanci; break;
            case TipoEvento::colpo:
                if (e.unita == giocatore_) ++r.colpi_subiti;
                if (e.altra == giocatore_) {
                    ++r.lanci_a_segno;
                    auto& b = r.per_bersaglio[e.unita];
                    b.classe = e.classe; b.lato = e.lato; ++b.colpi;
                    if (e.lato == Lato::neutrale) ++r.neutrali_colpiti;
                    if (e.lato == Lato::amico) ++r.amici_colpiti;
                }
                break;
            case TipoEvento::artiglieria_colpo: r.colpi_subiti += spari.nuovo(e.codice); break;   // nasce solo sul giocatore: diretti e schegge, niente avvertimenti; una raffica vale uno
            case TipoEvento::affondamento:
                if (e.unita != giocatore_) {
                    auto& b = r.per_bersaglio[e.unita];
                    b.classe = e.classe; b.lato = e.lato; b.affondato = true; b.dal_giocatore = e.altra == giocatore_;
                    if (e.altra == giocatore_) {
                        if (e.lato == Lato::ostile) ++r.affondati_ostili;
                        else if (e.lato == Lato::neutrale) ++r.affondati_neutrali;
                        else ++r.affondati_amici;
                    }
                }
                break;
            case TipoEvento::siluro_evitato: ++r.siluri_evitati; break;
            case TipoEvento::classificazione:
                if (e.codice == 411) {   // del sonarista: non nel giudizio del giocatore
                    ++r.classificazioni_sonarista;
                    if (!e.giusta) ++r.errori_sonarista;
                    break;
                }
                if (e.livello < 2 && e.valore == 0.0) break;   // la categoria: conta la classe
                if (e.giusta) ++r.classificazioni_giuste; else ++r.classificazioni_errate;
                if (e.giusta && e.tempo_scoperta >= 0.0) anticipi.push_back(e.tempo - e.tempo_scoperta);
                break;
            case TipoEvento::ping_subito: r.ping_subiti += std::max(1, static_cast<int>(e.valore)); break;   // i ping, non gli episodi
            case TipoEvento::fix_radiogoniometria: ++r.fix_subiti; break;
            case TipoEvento::avvistato: ++r.avvistamenti; break;
            case TipoEvento::esm_rischio_alto: ++r.esm_rischio_alto; break;
            case TipoEvento::transitorio_proprio: ++r.transitori; break;
            case TipoEvento::boa_vicina: ++r.boe_vicine; break;
            case TipoEvento::aereo_in_sorvolo: ++r.sorvoli; break;
            case TipoEvento::ping_sonar_immersione: ++r.ping_immersione; break;
            case TipoEvento::marcatore_avvistato: ++r.marcatori_avvistati; break;
            case TipoEvento::attacco_simulato: ++r.attacchi_simulati; break;
            case TipoEvento::foto_periscopio: ++r.foto_periscopio; break;
            case TipoEvento::esposizione_lunga: ++r.esposizioni_lunghe; break;
            case TipoEvento::scambio_tracce: ++r.scambi_tracce; break;
            case TipoEvento::ping_attivo: ++r.ping_attivi; break;
            case TipoEvento::messaggio_ricevuto: ++r.serie_copiate; break;
            case TipoEvento::messaggio_perso: ++r.serie_perse; break;
            case TipoEvento::sessione_mancata: ++r.sessioni_mancate; break;
            case TipoEvento::compito_scaduto: ++r.compiti_scaduti; break;
            case TipoEvento::scram: r.danno_nucleare = true; break;
            case TipoEvento::danno_subito:   // i soli caduti che il modello conosce: chi e' rimasto in un compartimento isolato
                if (e.codice == static_cast<int>(EventoBattello::Tipo::caduti)) r.morti += e.livello;
                break;
            case TipoEvento::mina_posata:
                if (e.unita == giocatore_) ++(e.livello > 0 ? r.mine_inerti : r.mine_posate);
                break;
            case TipoEvento::mina_esplosa:
                if (e.altra == giocatore_) ++r.mine_esplose;
                break;
            default: break;
        }
    }
    if (!anticipi.empty()) {
        double s = 0.0;
        for (double a : anticipi) s += a;
        r.anticipo_medio_s = s / static_cast<double>(anticipi.size());
    }
    r.per_arma = per_arma_;
    for (const auto& g : giudizi_) {
        if (g.esito == GiudizioRoe::Esito::infrazione || g.esito == GiudizioRoe::Esito::infrazione_non_copiato) {
            r.infrazioni.push_back(g);
            if (g.zona.empty()) ++r.fuoco_senza_ordine;
        }
    }
    r.ingressi_zone = ingressi_;
    r.zone_vietate = static_cast<int>(ingressi_.size());
    r.ordini_rispettati = r.infrazioni.empty() && ingressi_.empty();
    r.tempo_esposto_s = esposto_s_;
    if (const auto* p = m.posto_periscopi()) r.esposizione_periscopio_s = p->esposizione_totale_s();
    r.tempo_contatto_asw_s = asw_s_;
    r.distanza_min_siluro_m = distanza_min_siluro_;
    r.margine_collasso = margine_collasso_;
    r.danno_massimo = danno_massimo_;
    r.incaglio = incaglio_;
    r.danni_domati = danni_domati_;
    r.battello_perso = battello_perso_;
    r.equipaggio_salvo = !battello_perso_ || equipaggio_salvo_;
    r.rapporti_inviati = rapporti_inviati_;
    if (const SalaRadio* sr = m.radio()) {
        if (sr->parametri().ricevuta_richiesta)
            for (const auto& s : sr->sessioni(m.tempo())) r.rapporti_richiesti += s.stato == Sessione::Stato::fatta || s.stato == Sessione::Stato::mancata;
        if (!m.rapporto_contatto_.gestore.empty()) ++r.rapporti_richiesti;
    }

    // obiettivi
    const auto stato_di = [&](const DefObiettivo& d) {
        Obiettivo o = d.o;
        std::vector<int> unita = d.unita;
        for (const auto& [id, i] : info_)
            if (!i.arma && std::find(d.classi.begin(), d.classi.end(), i.classe) != d.classi.end() &&
                std::find(unita.begin(), unita.end(), id) == unita.end())
                unita.push_back(id);
        o.unita = unita;
        switch (d.o.tipo) {
            case T::affondare: {
                o.richiesti = d.minimo >= 0 ? d.minimo : static_cast<int>(unita.size());
                for (int id : unita) o.fatti += affondati_.count(id) ? 1 : 0;
                o.stato = o.fatti >= o.richiesti && o.richiesti > 0 ? S::raggiunto : !finita ? S::in_corso : o.fatti > 0 ? S::parziale : S::fallito;
                break;
            }
            case T::proteggere: {
                o.richiesti = static_cast<int>(unita.size());
                int perse = 0;
                for (int id : unita) perse += affondati_.count(id) || (!d.colpire && colpiti_dal_giocatore_.count(id)) ? 1 : 0;
                o.fatti = o.richiesti - perse;
                o.stato = perse > 0 ? (o.fatti > 0 ? S::parziale : S::fallito) : finita ? S::raggiunto : S::in_corso;
                break;
            }
            case T::raggiungere: {
                const auto k = static_cast<std::size_t>(&d - definizioni_.data());
                o.fatti = raggiunti_.count(k) ? 1 : 0;
                const bool scaduto = d.entro_s >= 0.0 && m.tempo() > d.entro_s;
                o.stato = o.fatti ? S::raggiunto : (finita || scaduto) ? S::fallito : S::in_corso;
                break;
            }
            case T::scoprire: {
                o.richiesti = d.minimo >= 0 ? d.minimo : static_cast<int>(unita.size());
                for (int id : unita) o.fatti += classificati_.count(id) ? 1 : 0;
                o.stato = o.fatti >= o.richiesti && o.richiesti > 0 ? S::raggiunto : !finita ? S::in_corso : o.fatti > 0 ? S::parziale : S::fallito;
                break;
            }
            case T::non_essere_scoperto: {
                const bool scoperto = std::any_of(eventi_.begin(), eventi_.end(), [](const EventoMissione& e) {
                    return e.tipo == TipoEvento::avvistato || e.tipo == TipoEvento::fix_radiogoniometria;
                });
                o.fatti = scoperto ? 0 : 1;
                o.stato = scoperto ? S::fallito : finita ? S::raggiunto : S::in_corso;
                break;
            }
            case T::sopravvivere:
                o.fatti = battello_perso_ ? 0 : 1;
                o.stato = battello_perso_ ? S::fallito : finita ? S::raggiunto : S::in_corso;
                break;
            case T::variabile: {
                const auto it = m.variabili().find(d.indice);
                const int v = it != m.variabili().end() ? it->second : 0;
                const bool ok = (!d.uguale || v == *d.uguale) && (!d.almeno || v >= *d.almeno) && (!d.al_piu || v <= *d.al_piu);
                o.fatti = ok ? 1 : 0;
                o.stato = ok ? S::raggiunto : finita ? S::fallito : S::in_corso;
                break;
            }
            case T::esito: {
                const bool ok = r.codice && std::find(d.codici.begin(), d.codici.end(), *r.codice) != d.codici.end();
                o.fatti = ok ? 1 : 0;
                o.stato = ok ? S::raggiunto : finita ? S::fallito : S::in_corso;
                break;
            }
            case T::roe:
                break;
            case T::contatto: {   // minuti di contatto sonar visibile del giocatore, cumulativi, con ogni unita'
                o.richiesti = d.minimo >= 0 ? d.minimo : static_cast<int>(unita.size());
                for (int id : unita) o.fatti += m.contatto_cumulato_s(giocatore_, id) >= 60.0 * d.minuti ? 1 : 0;
                const bool scaduto = d.entro_s >= 0.0 && m.tempo() > d.entro_s;
                o.stato = o.fatti >= o.richiesti && o.richiesti > 0 ? S::raggiunto
                          : !(finita || scaduto) ? S::in_corso : o.fatti > 0 ? S::parziale : S::fallito;
                break;
            }
            case T::foto: {   // le unita' nel campo di una foto del giocatore, entro la distanza se c'e'
                o.richiesti = d.minimo >= 0 ? d.minimo : static_cast<int>(unita.size());
                for (int id : unita)
                    o.fatti += std::any_of(fotografate_.begin(), fotografate_.end(), [&](const Fotografata& f) {
                        return f.unita == id && (d.entro_s < 0.0 || f.tempo <= d.entro_s) && (d.distanza_m < 0.0 || f.distanza_m <= d.distanza_m);
                    }) ? 1 : 0;
                const bool scaduto = d.entro_s >= 0.0 && m.tempo() > d.entro_s;
                o.stato = o.fatti >= o.richiesti && o.richiesti > 0 ? S::raggiunto
                          : !(finita || scaduto) ? S::in_corso : o.fatti > 0 ? S::parziale : S::fallito;
                break;
            }
            case T::colpire: {   // colpite dal giocatore, con armi vere o da esercitazione
                o.richiesti = d.minimo >= 0 ? d.minimo : static_cast<int>(unita.size());
                for (int id : unita) o.fatti += colpiti_dal_giocatore_.count(id) || colpiti_esercitazione_.count(id) ? 1 : 0;
                const bool scaduto = d.entro_s >= 0.0 && m.tempo() > d.entro_s;
                o.stato = o.fatti >= o.richiesti && o.richiesti > 0 ? S::raggiunto
                          : !(finita || scaduto) ? S::in_corso : o.fatti > 0 ? S::parziale : S::fallito;
                break;
            }
            case T::tlam: {   // i TLAM della missione arrivati sul bersaglio (nella finestra, se chiesta), dal loro esito
                o.richiesti = d.minimo >= 0 ? d.minimo : 1;
                for (const auto& e : eventi_)
                    if (e.tipo == TipoEvento::esito_tlam && e.giusta && (d.missione.empty() || e.classe_arma == d.missione) &&
                        (!d.finestra || e.codice == 1))
                        ++o.fatti;
                o.stato = o.fatti >= o.richiesti ? S::raggiunto : !finita ? S::in_corso : o.fatti > 0 ? S::parziale : S::fallito;
                break;
            }
            case T::scatola: {   // I11: nessun missile del giocatore con una scatola di ricerca oltre la massima
                bool oltre = false;
                for (const auto& e : eventi_)
                    oltre = oltre || (e.tipo == TipoEvento::lancio_giocatore && e.livello > 0 && e.livello - 1 > d.scatola_max &&
                                      (d.classi.empty() || std::find(d.classi.begin(), d.classi.end(), e.classe_arma) != d.classi.end()));
                o.fatti = oltre ? 0 : 1;
                o.richiesti = 1;
                o.stato = oltre ? S::fallito : finita ? S::raggiunto : S::in_corso;
                break;
            }
            case T::identificare: {   // l'ultima identificazione visiva del giocatore di ogni unita' (chi corregge recupera)
                o.richiesti = d.minimo >= 0 ? d.minimo : static_cast<int>(unita.size());
                std::map<int, bool> ultima;
                for (const auto& e : eventi_)
                    if (e.tipo == TipoEvento::classificazione && e.visiva && e.valore == 1.0) ultima[e.unita] = e.giusta;
                for (int id : unita) o.fatti += ultima.count(id) && ultima.at(id) ? 1 : 0;
                const bool scaduto = d.entro_s >= 0.0 && m.tempo() > d.entro_s;
                o.stato = o.fatti >= o.richiesti && o.richiesti > 0 ? S::raggiunto
                          : !(finita || scaduto) ? S::in_corso : o.fatti > 0 ? S::parziale : S::fallito;
                break;
            }
            case T::telemetro:      // unita' diverse lette entro l'errore relativo
            case T::rotta_stimata: {   // unita' diverse con un AOB entro `errore` gradi dalla rotta vera
                o.richiesti = d.minimo >= 0 ? d.minimo : 1;
                std::set<int> buone;
                int ripetute = 0;   // da_tma: le stime buone di ogni unita', a STIMA_TMA_OGNI_S l'una dall'altra
                const auto ammessa = [&](int id) { return d.unita.empty() || std::find(d.unita.begin(), d.unita.end(), id) != d.unita.end(); };
                if (d.o.tipo == T::telemetro) {
                    for (const auto& e : eventi_)
                        if (e.tipo == TipoEvento::misura_telemetro && e.distanza_m > 0.0 && ammessa(e.unita) &&
                            std::fabs(e.valore - e.distanza_m) / e.distanza_m <= d.errore)
                            buone.insert(e.unita);
                } else if (d.da_tma) {
                    constexpr double STIMA_TMA_OGNI_S = 300.0;   // la stessa rotta rivista subito non e' un'altra stima
                    std::map<int, double> ultima;
                    for (const auto& x : m.stime_tma_giocatore()) {
                        if (!ammessa(x.contatto) || std::fabs(normalizza_180(x.rotta - x.rotta_vera)) > d.errore) continue;
                        const auto it = ultima.find(x.contatto);
                        if (it != ultima.end() && x.t - it->second < STIMA_TMA_OGNI_S) continue;
                        ultima[x.contatto] = x.t;
                        ++ripetute;
                    }
                } else {
                    for (const auto& x : m.osservazioni_aob())
                        if (ammessa(x.contatto) && std::fabs(normalizza_180(x.rotta - x.rotta_vera)) <= d.errore) buone.insert(x.contatto);
                }
                o.fatti = d.da_tma ? ripetute : static_cast<int>(buone.size());
                const bool scaduto = d.entro_s >= 0.0 && m.tempo() > d.entro_s;
                o.stato = o.fatti >= o.richiesti ? S::raggiunto : !(finita || scaduto) ? S::in_corso : o.fatti > 0 ? S::parziale : S::fallito;
                break;
            }
            case T::zona_inviolata: {   // fallito alla prima violazione; raggiunto a fine missione o a entro_s
                const auto k = static_cast<std::size_t>(&d - definizioni_.data());
                const bool violata = violate_.count(k) != 0;
                const bool scaduto = d.entro_s >= 0.0 && m.tempo() > d.entro_s;
                o.fatti = violata ? 0 : 1;
                o.stato = violata ? S::fallito : (finita || scaduto) ? S::raggiunto : S::in_corso;
                break;
            }
            case T::minare: {   // mine del giocatore arrivate sul fondo (non inerti), nel cerchio se c'e'
                o.richiesti = d.minimo >= 0 ? d.minimo : 1;
                for (const auto& e : eventi_)
                    if (e.tipo == TipoEvento::mina_posata && e.unita == giocatore_ && e.livello == 0 &&
                        (!d.centro || distanza(e.pos, *d.centro) <= d.raggio))
                        ++o.fatti;
                const bool scaduto = d.entro_s >= 0.0 && m.tempo() > d.entro_s;
                o.stato = o.fatti >= o.richiesti ? S::raggiunto : !(finita || scaduto) ? S::in_corso : o.fatti > 0 ? S::parziale : S::fallito;
                break;
            }
        }
        return o;
    };
    for (const auto& d : definizioni_) {
        if (!d.varianti.empty() && std::find(d.varianti.begin(), d.varianti.end(), m.variante()) == d.varianti.end()) continue;
        if (!d.o.da_radio.empty() && !ordini_ricevuti_.count(d.o.da_radio)) continue;
        Obiettivo o = stato_di(d);
        if (!d.o.da_radio.empty()) {
            o.serie = serie_ordini_.at(d.o.da_radio);
            ++r.ordini;
            r.ordini_eseguiti += o.stato == S::raggiunto;
        }
        r.obiettivi.push_back(std::move(o));
    }
    if (!obiettivi_dichiarati_) {   // forma generica
        DefObiettivo d;
        d.o.id = "sopravvivere";
        d.o.tipo = T::sopravvivere;
        d.o.testo_it = "Riportare a casa il battello";
        d.o.testo_en = "Bring the boat home";
        r.obiettivi.push_back(stato_di(d));
    }
    if (m.radio()) {   // le regole d'ingaggio trasmesse
        Obiettivo o;
        o.id = "roe";
        o.tipo = T::roe;
        o.primario = false;
        o.testo_it = "Rispettare le regole d'ingaggio e le zone protette";
        o.testo_en = "Follow the rules of engagement and the protected areas";
        o.fatti = r.ordini_rispettati ? 1 : 0;
        o.stato = !r.ordini_rispettati ? S::fallito : finita ? S::raggiunto : S::in_corso;
        r.obiettivi.push_back(o);
    }
    // completezza: gli obiettivi attivi (affondare, raggiungere, scoprire, variabili, esiti), i primari contati doppi;
    // i vincoli (proteggere, non essere scoperti, ROE) non aggiungono nulla se rispettati ma tolgono fino a meta' se
    // violati; sopravvivere resta fuori (battello_perso). Senza obiettivi attivi: la quota dei vincoli rispettati
    const auto attivo = [](const Obiettivo& o) {
        return o.tipo != T::sopravvivere && o.tipo != T::proteggere && o.tipo != T::non_essere_scoperto && o.tipo != T::roe;
    };
    const auto quota = [](const Obiettivo& o) { return o.stato == S::raggiunto ? 1.0 : o.stato == S::parziale ? o.quota() : 0.0; };
    double peso = 0.0, fatto = 0.0, peso_p = 0.0, fatto_p = 0.0, peso_v = 0.0, violati = 0.0;
    bool primari_ok = true, qualcosa = false;
    for (const auto& o : r.obiettivi) {
        const double q = quota(o);
        if (o.primario) primari_ok = primari_ok && o.stato == S::raggiunto;
        if (attivo(o)) {
            const double w = o.peso * (o.primario ? 2.0 : 1.0);
            peso += w; fatto += w * q;
            if (o.primario) { peso_p += o.peso; fatto_p += o.peso * q; qualcosa = qualcosa || q > 0.0; }
        } else if (o.tipo != T::sopravvivere) {
            peso_v += o.peso;
            if (o.stato == S::fallito || o.stato == S::parziale) violati += o.peso * (1.0 - o.quota());
        }
    }
    const double vincoli = peso_v > 0.0 ? 1.0 - violati / peso_v : 1.0;
    r.completezza = peso > 0.0 ? fatto / peso * (1.0 - 0.5 * (1.0 - vincoli)) : vincoli * (finita ? 1.0 : 0.0);
    r.riuscita = peso_p > 0.0 ? fatto_p / peso_p : primari_ok && finita ? 1.0 : 0.0;
    if (peso_p == 0.0) qualcosa = primari_ok;

    // categoria
    if (r.codice && categorie_.count(*r.codice)) r.categoria = categoria_esito(categorie_.at(*r.codice)).value_or(C::in_corso);
    else if (battello_perso_) r.categoria = C::battello_perso;
    else if (!finita) r.categoria = C::in_corso;
    else r.categoria = primari_ok ? C::successo : qualcosa ? C::successo_parziale : C::fallimento;
    if (r.categoria != C::battello_perso && r.categoria != C::in_corso && (r.affondati_neutrali > 0 || r.affondati_amici > 0))
        r.categoria = C::fuoco_amico;
    return r;
}

std::vector<RegistroMissione::AreaObiettivo> RegistroMissione::aree_obiettivi() const {
    std::vector<AreaObiettivo> v;
    for (const auto& d : definizioni_) {
        AreaObiettivo a;
        a.id = d.o.id;
        a.tipo = d.o.tipo;
        a.primario = d.o.primario;
        a.classi = d.classi;
        for (const int u : d.o.unita)
            if (const auto it = info_.find(u); it != info_.end() && std::find(a.classi.begin(), a.classi.end(), it->second.classe) == a.classi.end())
                a.classi.push_back(it->second.classe);
        a.centro = d.centro;
        a.raggio_m = d.raggio;
        v.push_back(std::move(a));
    }
    return v;
}

}  // namespace m688
