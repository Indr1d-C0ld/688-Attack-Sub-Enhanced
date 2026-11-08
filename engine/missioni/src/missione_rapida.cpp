#include "missioni/missione_rapida.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <limits>
#include <map>
#include <optional>
#include <set>
#include <sstream>
#include <stdexcept>

#define TOML_EXCEPTIONS 1
#include <toml.hpp>

#include "nucleo/casuale.hpp"
#include "nucleo/classi.hpp"
#include "nucleo/fascicolo.hpp"
#include "nucleo/fondale.hpp"
#include "nucleo/geometria.hpp"
#include "nucleo/sole.hpp"
#include "testo_lua.hpp"

namespace m688 {

namespace {

// --- codice missione -------------------------------------------------------------------------------------------------
// 65 bit in 13 caratteri di Crockford: versione 3, teatro 3, stagione 3, ora 3, meteo 1, tipo 4, parte 1, forze 2,
// paranoia 2, difficolta' 3, ASW aereo 1, carriera 1, seme 32, controllo 6.

constexpr const char* ALFABETO = "0123456789ABCDEFGHJKMNPQRSTVWXYZ";
constexpr int BIT_CODICE = 65, BIT_DATI = 59, CARATTERI = 13;

struct Bit {
    std::array<std::uint8_t, BIT_CODICE> b{};
    int n = 0;
    void scrivi(std::uint64_t v, int k) {
        for (int i = k - 1; i >= 0; --i) b[static_cast<std::size_t>(n++)] = static_cast<std::uint8_t>((v >> i) & 1u);
    }
    std::uint64_t leggi(int& pos, int k) const {
        std::uint64_t v = 0;
        for (int i = 0; i < k; ++i) v = (v << 1) | b[static_cast<std::size_t>(pos++)];
        return v;
    }
};

// CRC-6 col polinomio primitivo x^6 + x + 1: cambia sempre per un carattere sbagliato (5 bit) e per lo scambio di due
// caratteri vicini (l'errore e' d(x)(x^5 + 1), che un polinomio primitivo di grado 6 non divide)
int crc6(const Bit& d, int n) {
    int r = 0;
    for (int i = 0; i < n; ++i) {
        const int alto = ((r >> 5) & 1) ^ d.b[static_cast<std::size_t>(i)];
        r = (r << 1) & 0x3F;
        if (alto) r ^= 0x03;
    }
    return r;
}

int indice_teatro(const std::string& t) {
    if (t.empty()) return 0;
    for (int i = 0; i < 7; ++i)
        if (t == TEATRI_RAPIDE[i]) return i + 1;
    return -1;
}

bool parametri_validi(const ParametriRapida& p) {
    return indice_teatro(p.teatro) >= 0 && static_cast<int>(p.stagione) <= 4 && static_cast<int>(p.ora) <= 4 &&
           static_cast<int>(p.tipo) <= 7 && p.forze >= 0 && p.forze <= 3 && p.paranoia >= 0 && p.paranoia <= 3 &&
           p.difficolta >= 0 && p.difficolta <= 5 && p.versione >= 1 && p.versione <= VERSIONE_RAPIDE;
}

Bit dati_codice(const ParametriRapida& p) {
    if (!parametri_validi(p)) throw std::invalid_argument("parametri della missione rapida fuori dai limiti");
    Bit d;
    d.scrivi(static_cast<std::uint64_t>(p.versione), 3);
    d.scrivi(static_cast<std::uint64_t>(indice_teatro(p.teatro)), 3);
    d.scrivi(static_cast<std::uint64_t>(p.stagione), 3);
    d.scrivi(static_cast<std::uint64_t>(p.ora), 3);
    d.scrivi(p.meteo_realistico ? 1 : 0, 1);
    d.scrivi(static_cast<std::uint64_t>(p.tipo), 4);
    d.scrivi(p.alfa ? 1 : 0, 1);
    d.scrivi(static_cast<std::uint64_t>(p.forze), 2);
    d.scrivi(static_cast<std::uint64_t>(p.paranoia), 2);
    d.scrivi(static_cast<std::uint64_t>(p.difficolta), 3);
    d.scrivi(p.asw_aereo ? 1 : 0, 1);
    d.scrivi(p.carriera ? 1 : 0, 1);
    d.scrivi(p.seme, 32);
    return d;
}

std::string caratteri_codice(const ParametriRapida& p) {
    Bit d = dati_codice(p);
    d.scrivi(static_cast<std::uint64_t>(crc6(d, BIT_DATI)), 6);
    std::string s;
    int pos = 0;
    for (int i = 0; i < CARATTERI; ++i) s += ALFABETO[d.leggi(pos, 5)];
    return s;
}

std::uint64_t mescola(std::uint64_t x) {   // splitmix64
    x += 0x9e3779b97f4a7c15ull;
    x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ull;
    x = (x ^ (x >> 27)) * 0x94d049bb133111ebull;
    return x ^ (x >> 31);
}

// I 59 bit dei parametri come numero, per i semi dei generatori
// il seme del caso: i bit dei parametri senza la versione, cosi' la stessa missione in tutte le versioni del generatore
// (dalla 2 cambia solo lo script dei comportamenti)
std::uint64_t impronta(const ParametriRapida& p) {
    ParametriRapida q = p;
    q.versione = 1;
    const Bit d = dati_codice(q);
    std::uint64_t v = 0;
    for (int i = 0; i < BIT_DATI; ++i) v = (v << 1) | d.b[static_cast<std::size_t>(i)];
    return v;
}

int intero(Casuale& r, int a, int b) {   // [a, b]
    return a + static_cast<int>(r.prossimo() % static_cast<std::uint64_t>(b - a + 1));
}

template <class T>
const T& scegli(Casuale& r, const std::vector<T>& v) {
    return v[static_cast<std::size_t>(r.prossimo() % v.size())];
}

// --- dati ------------------------------------------------------------------------------------------------------------

using Testi = std::array<std::string, 2>;                // it, en
using Righe = std::array<std::vector<std::string>, 2>;   // it, en
constexpr int IT = 0, EN = 1;
constexpr int USA = 0, URSS = 1;
const char* const PARTI[] = {"usa", "urss"};

struct Luogo {
    Testi nome;
    std::string tipo, parte;
    double lat = 0.0, lon = 0.0;
};

struct TeatroRapide {
    Testi nome;
    std::array<Testi, 2> comando;
    std::array<std::map<std::string, int>, 2> pesi;
    std::vector<Luogo> luoghi;
    std::array<std::map<std::string, std::vector<std::string>>, 2> forze;
};

struct DatiRapide {
    std::int64_t versione = 0;
    std::map<std::string, TeatroRapide> teatri;
    std::map<std::string, std::array<Testi, 2>> nomi;   // [lingua] -> singolare, plurale
    std::array<std::vector<std::string>, 2> operazioni;
    toml::table tab;
    DatiScenario base;   // modelli classici, kit, pescaggi, testi degli esiti (missioni/scenario_missione.hpp)
};

Testi testi(const toml::node_view<const toml::node>& n) {
    return {n["it"].value_or(std::string{}), n["en"].value_or(std::string{})};
}

Righe righe(const toml::node_view<const toml::node>& n) {
    Righe r;
    for (int l = 0; l < 2; ++l)
        if (const auto* a = n[l == IT ? "it" : "en"].as_array())
            for (const auto& v : *a) r[static_cast<std::size_t>(l)].push_back(v.value_or(std::string{}));
    return r;
}

std::vector<std::string> stringhe(const toml::node_view<const toml::node>& n) {
    std::vector<std::string> out;
    if (const auto* a = n.as_array())
        for (const auto& v : *a) out.push_back(v.value_or(std::string{}));
    return out;
}

DatiRapide carica_dati(const std::string& cartella) {
    DatiRapide d;
    const std::string percorso = cartella + "/missioni_rapide.toml";
    try {
        d.tab = toml::parse_file(percorso);
    } catch (const toml::parse_error& e) {
        throw std::runtime_error(percorso + ": " + std::string(e.description()));
    }
    d.versione = d.tab["versione"].value_or(std::int64_t{0});
    if (d.versione != VERSIONE_DATI_RAPIDE)
        throw std::runtime_error(percorso + ": versione " + std::to_string(d.versione) + ", il generatore legge la " +
                                 std::to_string(VERSIONE_DATI_RAPIDE));
    const toml::node_view<const toml::node> t{d.tab};
    for (const char* id : TEATRI_RAPIDE) {
        const auto n = t["teatri"][id];
        if (!n.is_table()) throw std::runtime_error(percorso + ": manca il teatro " + std::string(id));
        TeatroRapide tr;
        tr.nome = testi(n["nome"]);
        for (int l = 0; l < 2; ++l) {
            tr.comando[static_cast<std::size_t>(l)] = testi(n["comando"][PARTI[l]]);
            if (const auto* p = n["pesi"][PARTI[l]].as_table())
                for (auto&& [k, v] : *p) tr.pesi[static_cast<std::size_t>(l)][std::string(k.str())] = static_cast<int>(v.value_or(std::int64_t{0}));
            if (const auto* f = n["forze"][PARTI[l]].as_table())
                for (auto&& [k, v] : *f) {
                    auto& lista = tr.forze[static_cast<std::size_t>(l)][std::string(k.str())];
                    if (const auto* a = v.as_array())
                        for (const auto& c : *a) lista.push_back(c.value_or(std::string{}));
                }
        }
        if (const auto* a = n["luoghi"].as_array())
            for (const auto& v : *a) {
                const toml::node_view<const toml::node> l{v};
                tr.luoghi.push_back({{l["nome"].value_or(std::string{}), l["en"].value_or(l["nome"].value_or(std::string{}))},
                                     l["tipo"].value_or(std::string{}),
                                     l["parte"].value_or(std::string{}), l["lat"].value_or(0.0), l["lon"].value_or(0.0)});
            }
        d.teatri[id] = std::move(tr);
    }
    if (const auto* n = t["nomi_classi"].as_table())
        for (auto&& [k, v] : *n) {
            const toml::node_view<const toml::node> c{v};
            std::array<Testi, 2> nomi;
            for (int l = 0; l < 2; ++l) {
                const auto s = stringhe(c[l == IT ? "it" : "en"]);
                if (s.size() == 2) nomi[static_cast<std::size_t>(l)] = {s[0], s[1]};
            }
            d.nomi[std::string(k.str())] = nomi;
        }
    for (int l = 0; l < 2; ++l) d.operazioni[static_cast<std::size_t>(l)] = stringhe(t["nomi_operazioni"][PARTI[l]]);
    d.base = carica_dati_scenario(cartella);
    return d;
}

int peso(const DatiRapide& d, const std::string& teatro, TipoRapida tipo, bool alfa) {
    const auto& p = d.teatri.at(teatro).pesi[alfa ? URSS : USA];
    const auto it = p.find(nome_tipo_rapida(tipo));
    return it == p.end() ? 0 : it->second;
}

ParametriRapida risolvi_con(const ParametriRapida& p, const DatiRapide& d) {
    ParametriRapida q = p;
    Casuale r(mescola(impronta(p) ^ 0x7261706964615f31ull));
    if (q.teatro.empty()) {
        std::vector<std::pair<std::string, int>> v;
        for (const char* t : TEATRI_RAPIDE) {
            int w = 0;
            if (q.tipo != TipoRapida::a_caso) w = peso(d, t, q.tipo, q.alfa);
            else
                for (int k = 1; k <= 7; ++k) w = std::max(w, peso(d, t, static_cast<TipoRapida>(k), q.alfa) > 0 ? 1 : 0);
            if (w > 0) v.emplace_back(t, w);
        }
        if (v.empty()) v.emplace_back(TEATRI_RAPIDE[0], 1);
        int somma = 0;
        for (const auto& [t, w] : v) somma += w;
        int x = intero(r, 0, somma - 1);
        for (const auto& [t, w] : v) {
            if (x < w) { q.teatro = t; break; }
            x -= w;
        }
    } else {
        r.prossimo();
    }
    if (q.tipo == TipoRapida::a_caso) {
        int somma = 0;
        for (int k = 1; k <= 7; ++k) somma += peso(d, q.teatro, static_cast<TipoRapida>(k), q.alfa);
        q.tipo = TipoRapida::pattuglia;
        if (somma > 0) {
            int x = intero(r, 0, somma - 1);
            for (int k = 1; k <= 7; ++k) {
                const int w = peso(d, q.teatro, static_cast<TipoRapida>(k), q.alfa);
                if (x < w) { q.tipo = static_cast<TipoRapida>(k); break; }
                x -= w;
            }
        }
    } else {
        r.prossimo();
    }
    const int st = intero(r, 1, 4), ora = intero(r, 1, 4), forze = intero(r, 1, 3), diff = intero(r, 1, 5);
    if (q.stagione == StagioneRapida::a_caso) q.stagione = static_cast<StagioneRapida>(st);
    if (q.ora == MomentoRapido::a_caso) q.ora = static_cast<MomentoRapido>(ora);
    if (q.forze == 0) q.forze = forze;
    if (q.difficolta == 0) q.difficolta = diff;
    return q;
}

using testo_lua::num;

// --- testo Lua -------------------------------------------------------------------------------------------------------

std::string sostituisci(std::string s, const std::map<std::string, std::string>& valori) {
    for (const auto& [k, v] : valori)
        for (std::size_t i = 0; (i = s.find("{" + k + "}", i)) != std::string::npos; i += v.size()) s.replace(i, k.size() + 2, v);
    return s;
}

// --- generazione -----------------------------------------------------------------------------------------------------

using UnitaGen = UnitaScenario;
using ObiettivoGen = ObiettivoScenario;
using AereoGen = AereoAswScenario;

class Generatore {
public:
    Generatore(const ParametriRapida& p, const DatiRapide& d, const std::string& cartella)
        : p_(p), d_(d), t_(d.teatri.at(p.teatro)), f_(Fondale::carica(cartella + "/teatri/" + p.teatro + ".fdl")),
          r_(mescola(impronta(p) ^ mescola(p.seme))), lato_(p.alfa ? URSS : USA), ostile_(p.alfa ? USA : URSS),
          baltico_(p.teatro == "baltico") {
        if (!f_.georeferenziato()) throw std::runtime_error(p.teatro + ": fondale senza georeferenziazione");
        mezza_x_ = f_.larghezza() * f_.cella_m() / 2.0;
        mezza_y_ = f_.altezza() * f_.cella_m() / 2.0;
    }

    MissioneRapida genera(const std::string& codice);

private:
    // geometria
    bool acqua(Vec2 p, double minima, double massima = std::numeric_limits<double>::infinity()) const {
        if (std::fabs(p.x) > mezza_x_ - 4000.0 || std::fabs(p.y) > mezza_y_ - 4000.0 || !f_.dentro(p)) return false;
        const double z = f_.profondita(p);
        return z >= minima && z <= massima;
    }
    static Vec2 polare(Vec2 c, double rotta, double d) {
        return {c.x + d * std::sin(radianti(rotta)), c.y + d * std::cos(radianti(rotta))};
    }
    bool libero(Vec2 a, Vec2 b, double minima) const {
        const double l = distanza(a, b);
        const int n = std::max(1, static_cast<int>(l / 1000.0));
        for (int i = 0; i <= n; ++i) {
            const double s = static_cast<double>(i) / n;
            if (!acqua({a.x + (b.x - a.x) * s, a.y + (b.y - a.y) * s}, minima)) return false;
        }
        return true;
    }
    // un punto d'acqua utile vicino a `c`: a caso nel cerchio, poi in cerchi piu' grandi, poi il piu' vicino sulla griglia
    Vec2 trova(Vec2 c, double raggio, double minima, double massima = std::numeric_limits<double>::infinity()) {
        for (int giro = 0; giro < 3; ++giro, raggio *= 2.0)
            for (int i = 0; i < 300; ++i) {
                const Vec2 q = polare(c, r_.uniforme(0.0, 360.0), raggio * std::sqrt(r_.uniforme()));
                if (acqua(q, minima, massima)) return q;
            }
        std::optional<Vec2> meglio;
        for (double y = -mezza_y_; y <= mezza_y_; y += 2000.0)
            for (double x = -mezza_x_; x <= mezza_x_; x += 2000.0)
                if (acqua({x, y}, minima, massima) && (!meglio || distanza({x, y}, c) < distanza(*meglio, c))) meglio = Vec2{x, y};
        if (!meglio) throw std::runtime_error(p_.teatro + ": nessun punto con " + num(minima, 0) + "-" + num(massima, 0) + " m d'acqua");
        return *meglio;
    }
    Vec2 luogo(const Luogo& l) const { return f_.proiettata({l.lat, l.lon}); }
    std::optional<Luogo> scegli_luogo(const std::set<std::string>& tipi, const std::set<std::string>& parti,
                                      bool anche_fuori = false) {
        std::vector<Luogo> v;
        for (const auto& l : t_.luoghi)
            if (tipi.count(l.tipo) && parti.count(l.parte)) {
                const Vec2 q = luogo(l);
                if (anche_fuori || (std::fabs(q.x) < mezza_x_ && std::fabs(q.y) < mezza_y_)) v.push_back(l);
            }
        if (v.empty()) return std::nullopt;
        return scegli(r_, v);
    }
    // il mare aperto visto da un punto: il rilevamento dell'acqua piu' profonda a 20 km
    double verso_il_largo(Vec2 c) const {
        double meglio = 0.0, z_max = -1e9;
        for (int k = 0; k < 24; ++k) {
            const Vec2 q = polare(c, k * 15.0, 20000.0);
            const double z = f_.dentro(q) ? f_.profondita(q) : -1e9;
            if (z > z_max) { z_max = z; meglio = k * 15.0; }
        }
        return meglio;
    }

    // forze
    double sub_minima() const { return baltico_ ? 30.0 : 120.0; }
    double pescaggio(const std::string& c) const {
        const auto it = d_.base.pescaggi.find(c);
        return it == d_.base.pescaggi.end() ? 8.0 : it->second;
    }
    double minima_classe(const std::string& c, const std::string& categoria) const {
        if (categoria == "sottomarino") return sub_minima();
        if (categoria == "aereo") return -1e9;
        return std::max(15.0, pescaggio(c) + 10.0);
    }
    std::string classe_di(int parte, const std::string& ruolo, const std::string& riserva = "") {
        const auto& f = t_.forze[static_cast<std::size_t>(parte)];
        if (const auto it = f.find(ruolo); it != f.end() && !it->second.empty()) return scegli(r_, it->second);
        if (!riserva.empty()) return classe_di(parte, riserva);
        return "";
    }
    static std::string categoria_di(const std::string& ruolo) {
        if (ruolo == "sottomarini" || ruolo == "ssbn") return "sottomarino";
        if (ruolo == "elicotteri" || ruolo == "aerei") return "aereo";
        return "nave";
    }
    const ModelloClassico& modello(const UnitaGen& u) const {
        const auto it = d_.base.modelli.find(u.giocatore ? "giocatore_" + u.classe : u.classe);
        if (it == d_.base.modelli.end()) throw std::runtime_error("manca il modello classico di " + u.classe);
        return it->second;
    }
    int aggiungi(const std::string& classe, const std::string& ruolo, const std::string& parte, Vec2 pos, double rotta,
                 double velocita) {
        UnitaGen u;
        u.id = static_cast<int>(unita_.size());
        u.classe = classe;
        u.ruolo = ruolo;
        u.categoria = categoria_di(ruolo);
        u.parte = parte;
        u.non_combattente = ruolo == "mercantili" || ruolo == "logistica" || ruolo == "neutrali";
        const int lato_bit = parte == "usa" ? 0x08 : parte == "urss" ? 0x10 : 0;
        u.flag = (u.categoria == "sottomarino" ? 1 : u.categoria == "aereo" ? 3 : 2) | lato_bit | (u.non_combattente ? 0x40 : 0);
        u.pos = pos;
        u.rotta = normalizza_360(rotta);
        u.velocita = velocita;
        if (u.categoria == "sottomarino") {
            const double fondo = f_.profondita(pos);
            u.quota = std::max(20.0, std::min(r_.uniforme(60.0, 150.0), fondo - 25.0));
            u.profonda = std::max(u.quota, std::min(modello(u).quota_max * 0.8, fondo - 30.0));
            u.comportamento = "R_sottomarino";
        } else if (u.categoria == "aereo") {
            u.comportamento = "R_aereo";
        } else if (!u.non_combattente) {
            u.comportamento = "R_nave";
        }
        unita_.push_back(u);
        return u.id;
    }
    void rotta(int id, std::vector<Vec2> punti, bool ciclica, bool esce) {
        auto& u = unita_[static_cast<std::size_t>(id)];
        u.punti = std::move(punti);
        u.ciclica = ciclica;
        u.esce = esce;
        if (!u.punti.empty()) u.rotta = rilevamento(u.pos, u.punti.front());
    }
    // quattro punti a caso attorno a `c`, in ordine di rilevamento
    std::vector<Vec2> giro(Vec2 c, double raggio, double minima) {
        std::vector<Vec2> v;
        const double a0 = r_.uniforme(0.0, 360.0);
        for (int k = 0; k < 4; ++k) v.push_back(trova(polare(c, a0 + k * 90.0, raggio), raggio * 0.4, minima));
        return v;
    }
    // un passaggio che entra da un lato, attraversa `c` ed esce dall'altro
    std::pair<Vec2, Vec2> transito(Vec2 c, double lato, double minima) {
        std::pair<Vec2, Vec2> meglio{trova(polare(c, 0.0, lato), lato * 0.3, minima), trova(polare(c, 180.0, lato), lato * 0.3, minima)};
        for (int i = 0; i < 30; ++i) {
            const double a = r_.uniforme(0.0, 360.0);
            const Vec2 e = trova(polare(c, a, lato), lato * 0.25, minima);
            const Vec2 u = trova(polare(c, a + 180.0 + r_.uniforme(-30.0, 30.0), lato), lato * 0.25, minima);
            meglio = {e, u};
            if (libero(e, c, minima) && libero(c, u, minima)) break;
        }
        return meglio;
    }
    double crociera(const UnitaGen& u) const {   // la velocita' come frazione del telegrafo (telegrafo * avanti tutta)
        const double massima = modello(u).massima;
        return std::clamp(u.velocita * 280.0 / (256.0 * massima), 0.0, 1.0);
    }
    void aerei(int parte, Vec2 zona, double raggio, bool contro_giocatore, const std::vector<int>& bersagli, int nave_madre) {
        if (!p_.asw_aereo) return;
        const int n = 1 + (p_.forze == 3 ? 1 : 0);
        for (int i = 0; i < n; ++i) {
            const bool eli = nave_madre >= 0 && i == 0 && !classe_di(parte, "elicotteri").empty();
            const std::string classe = classe_di(parte, eli ? "elicotteri" : "aerei", "elicotteri");
            if (classe.empty()) return;
            const Vec2 pos = eli ? unita_[static_cast<std::size_t>(nave_madre)].pos : polare(zona, r_.uniforme(0.0, 360.0), raggio * 0.6);
            const int id = aggiungi(classe, eli ? "elicotteri" : "aerei", PARTI[parte], pos, 0.0, 0.0);
            auto& u = unita_[static_cast<std::size_t>(id)];
            u.velocita = modello(u).massima * 0.6;
            std::vector<Vec2> punti;
            const double a0 = r_.uniforme(0.0, 360.0);
            for (int k = 0; k < 4; ++k) punti.push_back(polare(zona, a0 + k * 90.0, raggio * 0.7));
            rotta(id, punti, true, false);
            AereoGen a;
            a.unita = id;
            a.situazione = situazione_;
            a.contro_giocatore = contro_giocatore;
            a.bersagli = bersagli;
            a.zona = zona;
            a.raggio = raggio;
            if (eli) a.base_unita = nave_madre;
            else if (const auto b = scegli_luogo({"aeroporto"}, {parte == USA ? "nato" : "urss"}, true)) a.base = luogo(*b);
            aerei_.push_back(a);
        }
    }
    void neutrali(Vec2 c, int n) {
        for (int i = 0; i < n; ++i) {
            const std::string classe = scegli(r_, std::vector<std::string>{"mercantile", "cargo", "petroliera"});
            const double minima = minima_classe(classe, "nave");
            const auto [e, u] = transito(c, r_.uniforme(30000.0, 50000.0), minima);
            const double s = r_.uniforme(0.0, 0.6);
            const Vec2 pos{e.x + (u.x - e.x) * s, e.y + (u.y - e.y) * s};
            const int id = aggiungi(classe, "neutrali", "neutrale", acqua(pos, minima) ? pos : e, 0.0, r_.uniforme(9.0, 13.0));
            rotta(id, {u}, false, true);
        }
    }
    std::string nome(const std::string& classe, int n, int lingua) const {
        const auto it = d_.nomi.find(classe);
        const std::string s = it == d_.nomi.end() ? classe : it->second[static_cast<std::size_t>(lingua)][n == 1 ? 0 : 1];
        return std::to_string(n) + " " + s;
    }
    std::string nome_singolo(const std::string& classe, int lingua) const {
        const auto it = d_.nomi.find(classe);
        return it == d_.nomi.end() ? classe : it->second[static_cast<std::size_t>(lingua)][0];
    }
    std::string elenco(const std::vector<int>& ids, int lingua) const {
        std::vector<std::pair<std::string, int>> conti;
        for (const int id : ids) {
            const auto& u = unita_[static_cast<std::size_t>(id)];
            if (u.nascosta) continue;
            auto it = std::find_if(conti.begin(), conti.end(), [&](const auto& c) { return c.first == u.classe; });
            if (it == conti.end()) conti.emplace_back(u.classe, 1);
            else ++it->second;
        }
        std::string s;
        for (std::size_t i = 0; i < conti.size(); ++i) {
            if (i) s += i + 1 == conti.size() ? (lingua == IT ? " e " : " and ") : ", ";
            s += nome(conti[i].first, conti[i].second, lingua);
        }
        return s.empty() ? (lingua == IT ? "nessuna unita' segnalata" : "no units reported") : s;
    }
    std::vector<int> ostili(const std::string& categoria = "") const {
        std::vector<int> v;
        for (const auto& u : unita_)
            if (u.parte == PARTI[ostile_] && (categoria.empty() || u.categoria == categoria)) v.push_back(u.id);
        return v;
    }

    // le tipologie
    void pattuglia();
    void caccia();
    void antinave();
    void scorta();
    void ricognizione();
    void minamento();
    void inserimento();
    void partenza(Vec2 zona, double distanza_m);
    void sorpresa(Vec2 zona);
    void difensori(Vec2 c, double raggio, int n, const std::string& ruolo);


    const ParametriRapida& p_;
    const DatiRapide& d_;
    const TeatroRapide& t_;
    Fondale f_;
    Casuale r_;
    int lato_, ostile_;
    bool baltico_;
    double mezza_x_ = 0.0, mezza_y_ = 0.0;

    std::vector<UnitaGen> unita_;
    std::vector<ObiettivoGen> obiettivi_;
    std::vector<AereoGen> aerei_;
    std::string situazione_ = "guerra", uscita_nome_, bersaglio_classe_, mina_;
    Testi zona_nome_;
    Vec2 zona_;
    std::vector<int> forze_briefing_, amici_briefing_;
    double durata_ = 3.0 * 3600.0;
    int mine_ = 0, minimo_convoglio_ = 0;
    ChiusuraScenario chiusura_;   // la condizione di fine (codice 120)
    std::string tipo_missione_ = "antisom";
    int ospiti_ = 0;
};

// Il giocatore parte a `distanza_m` dalla zona, in acqua profonda, con la prua sulla zona
void Generatore::partenza(Vec2 zona, double distanza_m) {
    const double minima = baltico_ ? 25.0 : 80.0;
    Vec2 pos = zona;
    for (int i = 0; i < 40; ++i) {
        const Vec2 q = polare(zona, r_.uniforme(0.0, 360.0), distanza_m * r_.uniforme(0.85, 1.15));
        pos = q;
        if (acqua(q, minima) && libero(q, zona, baltico_ ? 15.0 : 40.0)) break;
    }
    if (!acqua(pos, minima)) pos = trova(pos, 8000.0, minima);
    auto& g = unita_[0];
    g.pos = pos;
    g.rotta = rilevamento(pos, zona);
    g.quota = std::max(12.0, std::min(45.0, f_.profondita(pos) - 20.0));
}

// Un sottomarino ostile che il briefing non nomina (difficolta' 4-5 con forze medie o molte)
void Generatore::sorpresa(Vec2 zona) {
    if (p_.difficolta < 4 || p_.forze < 2) return;
    const std::string c = classe_di(ostile_, "sottomarini");
    if (c.empty()) return;
    const Vec2 pos = trova(zona, 15000.0, sub_minima());
    const int id = aggiungi(c, "sottomarini", PARTI[ostile_], pos, 0.0, r_.uniforme(4.0, 7.0));
    unita_[static_cast<std::size_t>(id)].nascosta = true;
    rotta(id, giro(pos, r_.uniforme(6000.0, 10000.0), sub_minima()), true, false);
}

// Navi di pattuglia attorno a un punto (ricognizione, minamento, inserimento)
void Generatore::difensori(Vec2 c, double raggio, int n, const std::string& ruolo) {
    for (int i = 0; i < n; ++i) {
        const std::string classe = classe_di(ostile_, ruolo, "scorte");
        if (classe.empty()) return;
        const double minima = minima_classe(classe, categoria_di(ruolo));
        const Vec2 pos = trova(polare(c, r_.uniforme(0.0, 360.0), raggio), raggio * 0.5, minima);
        const int id = aggiungi(classe, ruolo, PARTI[ostile_], pos, 0.0, r_.uniforme(6.0, 12.0));
        rotta(id, giro(c, raggio * r_.uniforme(0.7, 1.3), minima), true, false);
        forze_briefing_.push_back(id);
    }
}

void Generatore::pattuglia() {
    situazione_ = r_.uniforme() < 0.5 ? "crisi" : "pace";
    const auto l = scegli_luogo({"mare", "stretto"}, {"nato", "urss", "neutrale"});
    zona_nome_ = l ? l->nome : t_.nome;
    zona_ = trova(l ? luogo(*l) : Vec2{}, 15000.0, baltico_ ? 25.0 : 100.0);
    const double r = 6.0 * MIGLIO;
    durata_ = (2.5 + 0.5 * p_.forze) * 3600.0;
    partenza(zona_, (6.0 + 2.0 * p_.difficolta) * MIGLIO);
    const int n = 1 + p_.forze + (p_.difficolta >= 4 ? 1 : 0);
    std::vector<int> transiti;
    for (int i = 0; i < n; ++i) {
        const bool sub = r_.uniforme() < 0.6 || classe_di(ostile_, "scorte").empty();
        const std::string ruolo = sub ? "sottomarini" : "scorte";
        const std::string classe = classe_di(ostile_, ruolo, "scorte");
        if (classe.empty()) continue;
        const double minima = minima_classe(classe, categoria_di(ruolo));
        const auto [e, u] = transito(zona_, r_.uniforme(35000.0, 55000.0), minima);
        // partono scaglionate lungo la rotta: passano nel settore in tempi diversi
        const double s = r_.uniforme(0.0, 0.45);
        Vec2 pos{e.x + (zona_.x - e.x) * s, e.y + (zona_.y - e.y) * s};
        if (!acqua(pos, minima)) pos = e;
        const int id = aggiungi(classe, ruolo, PARTI[ostile_], pos, 0.0, sub ? r_.uniforme(5.0, 9.0) : r_.uniforme(10.0, 15.0));
        rotta(id, {zona_, u}, false, true);
        transiti.push_back(id);
    }
    forze_briefing_ = transiti;
    neutrali(zona_, intero(r_, 0, 2));
    aerei(ostile_, zona_, 25000.0, true, {}, -1);
    sorpresa(zona_);
    const int minimo = static_cast<int>(transiti.size()) - (p_.difficolta < 4 && transiti.size() > 1 ? 1 : 0);
    obiettivi_.push_back({"settore", "raggiungere", {}, -1, CerchioScenario{zona_.x, zona_.y, r}, durata_ * 0.5, true, true, {}});
    obiettivi_.push_back({"contatti", "scoprire", transiti, minimo, std::nullopt, -1.0, true, true, {}});
    obiettivi_.push_back({"occulto", "non_essere_scoperto", {}, -1, std::nullopt, -1.0, false, true, {}});
    obiettivi_.push_back({"non_attaccare", "proteggere", transiti, -1, std::nullopt, -1.0, false, false, {}});
    chiusura_ = {ChiusuraScenario::Tipo::unita_finite, transiti, {}, {}};
}

void Generatore::caccia() {
    situazione_ = r_.uniforme() < 0.6 ? "guerra" : "crisi";
    const auto l = scegli_luogo({"mare", "stretto"}, {"nato", "urss", "neutrale"});
    zona_nome_ = l ? l->nome : t_.nome;
    zona_ = trova(l ? luogo(*l) : Vec2{}, 15000.0, sub_minima() + 30.0);
    durata_ = (4.5 - 0.3 * p_.difficolta) * 3600.0;
    partenza(zona_, (8.0 + 3.0 * p_.difficolta) * MIGLIO);
    bersaglio_classe_ = classe_di(ostile_, "sottomarini");
    const int b = aggiungi(bersaglio_classe_, "sottomarini", PARTI[ostile_], trova(zona_, 5000.0, sub_minima()), 0.0,
                           r_.uniforme(4.0, 8.0));
    rotta(b, giro(zona_, r_.uniforme(8000.0, 12000.0), sub_minima()), true, false);
    forze_briefing_ = {b};
    for (int i = 0; i < p_.forze - 1; ++i) {
        const std::string c = classe_di(ostile_, "scorte");
        if (c.empty()) break;
        const double minima = minima_classe(c, "nave");
        const int id = aggiungi(c, "scorte", PARTI[ostile_], trova(zona_, 20000.0, minima), 0.0, r_.uniforme(8.0, 12.0));
        rotta(id, giro(zona_, r_.uniforme(10000.0, 18000.0), minima), true, false);
        forze_briefing_.push_back(id);
    }
    int nave = -1;
    for (const int id : forze_briefing_)
        if (unita_[static_cast<std::size_t>(id)].categoria == "nave") { nave = id; break; }
    aerei(ostile_, zona_, 25000.0, true, {}, nave);
    sorpresa(zona_);
    if (situazione_ == "guerra") {
        obiettivi_.push_back({"bersaglio", "affondare", {b}, -1, std::nullopt, -1.0, true, true, {}});
        chiusura_ = {ChiusuraScenario::Tipo::unita_finita, {b}, {}, {}};
    } else {
        obiettivi_.push_back({"bersaglio", "scoprire", {b}, -1, std::nullopt, -1.0, true, true, {}});
        obiettivi_.push_back({"occulto", "non_essere_scoperto", {}, -1, std::nullopt, -1.0, true, true, {}});
        obiettivi_.push_back({"non_attaccare", "proteggere", {b}, -1, std::nullopt, -1.0, false, false, {}});
        chiusura_ = {};
    }
}

void Generatore::antinave() {
    situazione_ = "guerra";
    tipo_missione_ = "antinave";
    const auto l = scegli_luogo({"mare", "stretto"}, {"nato", "urss", "neutrale"});
    zona_nome_ = l ? l->nome : t_.nome;
    zona_ = trova(l ? luogo(*l) : Vec2{}, 15000.0, baltico_ ? 25.0 : 150.0);
    const bool convoglio = r_.uniforme() < 0.4 && !t_.forze[static_cast<std::size_t>(ostile_)].at("mercantili").empty();
    // il gruppo: l'unita' di pregio (o la nave piu' grande del convoglio) e le scorte
    std::vector<std::string> classi;
    bersaglio_classe_ = convoglio ? classe_di(ostile_, "mercantili") : classe_di(ostile_, "pregio", "scorte");
    classi.push_back(bersaglio_classe_);
    const int scorte = std::max(1, p_.forze + p_.difficolta / 2 - 1);
    for (int i = 0; i < scorte; ++i) classi.push_back(classe_di(ostile_, "scorte"));
    if (convoglio)
        for (int i = 0; i < 1 + p_.forze; ++i) classi.push_back(classe_di(ostile_, "mercantili"));
    double minima = 15.0, vmax = 30.0;
    for (const auto& c : classi) {
        minima = std::max(minima, pescaggio(c) + 12.0);
        if (const auto it = d_.base.modelli.find(c); it != d_.base.modelli.end()) vmax = std::min(vmax, it->second.massima);
    }
    const double v = std::min(14.0, vmax * 0.85);
    const auto [e, u] = transito(zona_, r_.uniforme(45000.0, 65000.0), minima);
    const double rotta_g = rilevamento(e, u);
    std::vector<int> gruppo;
    for (std::size_t i = 0; i < classi.size(); ++i) {
        // formazione: il pregio al centro, le scorte avanti e ai lati, i mercantili in colonna
        double avanti = 0.0, lato = 0.0;
        if (i > 0 && i <= static_cast<std::size_t>(scorte)) {
            const double a = 360.0 * static_cast<double>(i - 1) / scorte;
            avanti = 3500.0 * std::cos(radianti(a));
            lato = 3500.0 * std::sin(radianti(a));
        } else if (i > static_cast<std::size_t>(scorte)) {
            avanti = -1200.0 * static_cast<double>(i - scorte);
            lato = (i % 2 ? 900.0 : -900.0);
        }
        const auto sposta = [&](Vec2 q) { return polare(polare(q, rotta_g, avanti), rotta_g + 90.0, lato); };
        Vec2 pos = sposta(e);
        if (!acqua(pos, minima)) pos = trova(pos, 3000.0, minima);
        const std::string ruolo = i == 0 ? (convoglio ? "mercantili" : "pregio") : i <= static_cast<std::size_t>(scorte) ? "scorte" : "mercantili";
        const int id = aggiungi(classi[i], ruolo, PARTI[ostile_], pos, rotta_g, v);
        std::vector<Vec2> punti{sposta(zona_), sposta(u)};
        for (auto& q : punti)
            if (!acqua(q, minima)) q = trova(q, 3000.0, minima);
        // zig-zag del gruppo con la paranoia alta: un lato in piu' a meta' strada
        if (p_.paranoia >= 2) punti.insert(punti.begin() + 1, polare(sposta(Vec2{(e.x + zona_.x) / 2, (e.y + zona_.y) / 2}), rotta_g + 90.0, (p_.seme & 1u) ? 3000.0 : -3000.0));
        rotta(id, punti, false, true);
        gruppo.push_back(id);
    }
    forze_briefing_ = gruppo;
    // il giocatore davanti al gruppo, di lato alla rotta
    const double l_rotta = distanza(e, u);
    partenza(polare(e, rotta_g, l_rotta * r_.uniforme(0.45, 0.6)), (8.0 + p_.difficolta) * MIGLIO);
    durata_ = std::min(6.0 * 3600.0, l_rotta / (v * NODO) + 900.0);
    neutrali(zona_, intero(r_, 0, 1));
    int nave = -1;
    for (const int id : gruppo)
        if (!unita_[static_cast<std::size_t>(id)].non_combattente) { nave = id; break; }
    aerei(ostile_, zona_, 30000.0, true, {}, nave);
    sorpresa(zona_);
    if (convoglio) {
        std::vector<int> mercantili;
        for (const int id : gruppo)
            if (unita_[static_cast<std::size_t>(id)].ruolo == "mercantili") mercantili.push_back(id);
        const int n = static_cast<int>(mercantili.size());
        minimo_convoglio_ = std::clamp(static_cast<int>(std::lround(n * (0.4 + 0.1 * p_.difficolta))), 1, n);
        obiettivi_.push_back({"convoglio", "affondare", mercantili, minimo_convoglio_, std::nullopt, -1.0, true, true, {}});
        chiusura_ = {ChiusuraScenario::Tipo::unita_finite, mercantili, {}, {}};
    } else {
        obiettivi_.push_back({"pregio", "affondare", {gruppo[0]}, -1, std::nullopt, -1.0, true, true, {}});
        chiusura_ = {ChiusuraScenario::Tipo::unita_finita, {gruppo[0]}, {}, {}};
    }
}

void Generatore::scorta() {
    situazione_ = "guerra";
    tipo_missione_ = "scorta";
    const auto l = scegli_luogo({"mare", "stretto"}, {"nato", "urss", "neutrale"});
    zona_nome_ = l ? l->nome : t_.nome;
    zona_ = trova(l ? luogo(*l) : Vec2{}, 15000.0, baltico_ ? 25.0 : 150.0);
    std::vector<std::string> classi{classe_di(lato_, "pregio", "scorte")};
    std::vector<std::string> ruoli{"pregio"};
    for (int i = 0; i < 1 + (p_.forze >= 2 ? 1 : 0); ++i) { classi.push_back(classe_di(lato_, "scorte")); ruoli.push_back("scorte"); }
    if (const std::string c = classe_di(lato_, "logistica"); !c.empty() && p_.forze == 3) { classi.push_back(c); ruoli.push_back("logistica"); }
    double minima = 15.0, vmax = 30.0;
    for (const auto& c : classi) {
        minima = std::max(minima, pescaggio(c) + 12.0);
        if (const auto it = d_.base.modelli.find(c); it != d_.base.modelli.end()) vmax = std::min(vmax, it->second.massima);
    }
    const double v = std::min(12.0, vmax * 0.8);
    const auto [e, u] = transito(zona_, r_.uniforme(40000.0, 55000.0), minima);
    const double rotta_g = rilevamento(e, u);
    std::vector<int> gruppo;
    for (std::size_t i = 0; i < classi.size(); ++i) {
        const double a = i == 0 ? 0.0 : 360.0 * static_cast<double>(i - 1) / static_cast<double>(classi.size() - 1);
        const double raggio = i == 0 ? 0.0 : 3000.0;
        const auto sposta = [&](Vec2 q) { return polare(q, rotta_g + a, raggio); };
        Vec2 pos = sposta(e);
        if (!acqua(pos, minima)) pos = trova(pos, 3000.0, minima);
        const int id = aggiungi(classi[i], ruoli[i], PARTI[lato_], pos, rotta_g, v);
        std::vector<Vec2> punti{sposta(zona_), sposta(u)};
        for (auto& q : punti)
            if (!acqua(q, minima)) q = trova(q, 3000.0, minima);
        rotta(id, punti, false, true);
        gruppo.push_back(id);
    }
    amici_briefing_ = gruppo;
    // il giocatore davanti al gruppo
    {
        auto& g = unita_[0];
        g.pos = trova(polare(e, rotta_g, 4.0 * MIGLIO), 4000.0, baltico_ ? 25.0 : 80.0);
        g.rotta = rotta_g;
        g.quota = std::max(12.0, std::min(45.0, f_.profondita(g.pos) - 20.0));
    }
    // i sottomarini ostili in agguato lungo la rotta
    std::vector<int> minacce;
    const double l_rotta = distanza(e, u);
    for (int i = 0; i < p_.forze + (p_.difficolta >= 4 ? 1 : 0); ++i) {
        const std::string c = classe_di(ostile_, "sottomarini");
        if (c.empty()) break;
        const Vec2 q = polare(polare(e, rotta_g, l_rotta * r_.uniforme(0.3, 0.75)), rotta_g + 90.0, r_.uniforme(-8000.0, 8000.0));
        const Vec2 pos = trova(q, 5000.0, sub_minima());
        const int id = aggiungi(c, "sottomarini", PARTI[ostile_], pos, 0.0, r_.uniforme(3.0, 6.0));
        rotta(id, giro(pos, r_.uniforme(3000.0, 6000.0), sub_minima()), true, false);
        minacce.push_back(id);
    }
    forze_briefing_ = minacce;
    durata_ = std::min(6.0 * 3600.0, l_rotta / (v * NODO) + 900.0);
    aerei(lato_, zona_, 25000.0, false, minacce, gruppo.size() > 1 ? gruppo[1] : gruppo[0]);
    obiettivi_.push_back({"gruppo", "proteggere", gruppo, -1, std::nullopt, -1.0, true, true, {}});
    obiettivi_.push_back({"minacce", "affondare", minacce, -1, std::nullopt, -1.0, false, true, {}});
    chiusura_ = {ChiusuraScenario::Tipo::unita_finite, gruppo, {}, {}};
}

void Generatore::ricognizione() {
    situazione_ = "crisi";
    const auto l = scegli_luogo({"base", "porto"}, {ostile_ == URSS ? "urss" : "nato"});
    if (!l) throw std::runtime_error(p_.teatro + ": nessuna base avversaria per la ricognizione");
    zona_nome_ = l->nome;
    const Vec2 base = luogo(*l);
    zona_ = trova(polare(base, verso_il_largo(base), 6000.0), 6000.0, baltico_ ? 18.0 : 40.0);
    durata_ = (4.0 - 0.2 * p_.difficolta) * 3600.0;
    partenza(zona_, (12.0 + p_.difficolta) * MIGLIO);
    // le unita' in rada, ferme
    std::vector<int> rada;
    const std::vector<std::string> ruoli{"pregio", "scorte", "logistica", "mercantili"};
    for (int i = 0; i < 2 + p_.forze; ++i) {
        const std::string ruolo = scegli(r_, ruoli);
        const std::string c = classe_di(ostile_, ruolo, "scorte");
        if (c.empty()) continue;
        const Vec2 pos = trova(base, 6000.0, pescaggio(c) + 6.0, baltico_ ? 25.0 : 120.0);
        rada.push_back(aggiungi(c, ruolo, PARTI[ostile_], pos, r_.uniforme(0.0, 360.0), 0.0));
    }
    forze_briefing_ = rada;
    difensori(zona_, r_.uniforme(5000.0, 9000.0), (p_.forze >= 2 ? 1 : 0) + (p_.difficolta >= 4 ? 1 : 0), "scorte");
    aerei(ostile_, zona_, 20000.0, true, {}, -1);
    neutrali(zona_, intero(r_, 0, 1));
    const double r = 2.0 * MIGLIO;
    obiettivi_.push_back({"osservazione", "raggiungere", {}, -1, CerchioScenario{zona_.x, zona_.y, r}, durata_ * 0.6, true, true, {}});
    obiettivi_.push_back({"rada", "scoprire", rada, std::max(1, static_cast<int>(rada.size()) - (p_.difficolta <= 2 ? 1 : 0)), std::nullopt, -1.0, true, true, {}});
    obiettivi_.push_back({"occulto", "non_essere_scoperto", {}, -1, std::nullopt, -1.0, true, true, {}});
    obiettivi_.push_back({"non_attaccare", "proteggere", rada, -1, std::nullopt, -1.0, false, false, {}});
    chiusura_ = {ChiusuraScenario::Tipo::dentro_poi_fuori, {}, {zona_.x, zona_.y, r}, {zona_.x, zona_.y, 12.0 * MIGLIO}};
}

void Generatore::minamento() {
    situazione_ = "guerra";
    tipo_missione_ = "minamento";
    const auto l = scegli_luogo({"base", "porto", "stretto"}, {ostile_ == URSS ? "urss" : "nato", "neutrale"});
    zona_nome_ = l ? l->nome : t_.nome;
    const Vec2 c = l ? luogo(*l) : Vec2{};
    // la mina secondo il fondale vicino (data/motore/carico.toml, [mine])
    struct Mina { const char* id; double minima, massima; int deposito; };
    const std::vector<Mina> mine = lato_ == USA ? std::vector<Mina>{{"mk67", 25.0, 100.0, 10}, {"mk60", 200.0, 900.0, 6}}
                                                : std::vector<Mina>{{"mdt", 20.0, 50.0, 20}, {"pmr2", 200.0, 1000.0, 6}};
    std::optional<Vec2> punto;
    for (const auto& m : mine) {
        for (int i = 0; i < 400 && !punto; ++i) {
            const Vec2 q = polare(c, r_.uniforme(0.0, 360.0), 20000.0 * std::sqrt(r_.uniforme()));
            if (acqua(q, m.minima, m.massima) && acqua(polare(q, 0.0, 1500.0), m.minima, m.massima) &&
                acqua(polare(q, 180.0, 1500.0), m.minima, m.massima))
                punto = q;
        }
        if (punto) { mina_ = m.id; mine_ = m.deposito; break; }
    }
    if (!punto) {
        punto = trova(c, 20000.0, mine[0].minima, mine[0].massima);
        mina_ = mine[0].id;
        mine_ = mine[0].deposito;
    }
    zona_ = *punto;
    durata_ = (4.0 - 0.2 * p_.difficolta) * 3600.0;
    partenza(zona_, (10.0 + p_.difficolta) * MIGLIO);
    difensori(zona_, r_.uniforme(6000.0, 10000.0), 1 + (p_.forze >= 2 ? 1 : 0) + (p_.difficolta >= 4 ? 1 : 0), "scorte");
    aerei(ostile_, zona_, 20000.0, true, {}, -1);
    const int minimo = 4 + (p_.difficolta >= 3 ? 2 : 0) + (p_.difficolta >= 5 ? 2 : 0);
    const double r = 1500.0;
    obiettivi_.push_back({"campo", "minare", {}, std::min(minimo, mine_), CerchioScenario{zona_.x, zona_.y, r}, durata_, true, true, {}});
    obiettivi_.push_back({"occulto", "non_essere_scoperto", {}, -1, std::nullopt, -1.0, false, true, {}});
    mine_ = std::min(minimo, mine_);
    chiusura_ = {ChiusuraScenario::Tipo::dentro_poi_fuori, {}, {zona_.x, zona_.y, r}, {zona_.x, zona_.y, 8.0 * MIGLIO}};
}

void Generatore::inserimento() {
    situazione_ = "crisi";
    ospiti_ = 6;
    const auto l = scegli_luogo({"base", "porto"}, {ostile_ == URSS ? "urss" : "nato"});
    if (!l) throw std::runtime_error(p_.teatro + ": nessuna costa avversaria per l'inserimento");
    zona_nome_ = l->nome;
    const Vec2 costa = luogo(*l);
    // lo sbarco: acqua da 15 a 40 m, il piu' vicino possibile alla costa
    std::optional<Vec2> sbarco;
    for (int i = 0; i < 600; ++i) {
        const Vec2 q = polare(costa, r_.uniforme(0.0, 360.0), 12000.0 * std::sqrt(r_.uniforme()));
        if (acqua(q, 15.0, 40.0) && (!sbarco || distanza(q, costa) < distanza(*sbarco, costa))) sbarco = q;
    }
    zona_ = sbarco ? *sbarco : trova(costa, 12000.0, 15.0, 40.0);
    const double largo = verso_il_largo(zona_);
    const Vec2 esfiltrazione = trova(polare(zona_, largo, (12.0 + p_.difficolta) * MIGLIO), 6000.0, baltico_ ? 25.0 : 80.0);
    uscita_nome_ = num(rilevamento(zona_, esfiltrazione), 0) + "°, " + num(distanza(zona_, esfiltrazione) / MIGLIO, 0) + " nm";
    durata_ = (4.0 - 0.2 * p_.difficolta) * 3600.0;
    partenza(zona_, (16.0 + p_.difficolta) * MIGLIO);
    difensori(zona_, r_.uniforme(4000.0, 8000.0), 1 + p_.forze / 2 + (p_.difficolta >= 4 ? 1 : 0), "scorte");
    aerei(ostile_, zona_, 20000.0, true, {}, -1);
    const double r = 1000.0, r_esf = 1.5 * MIGLIO;
    obiettivi_.push_back({"sbarco", "raggiungere", {}, -1, CerchioScenario{zona_.x, zona_.y, r}, durata_ * 0.6, true, true, {}});
    obiettivi_.push_back({"esfiltrazione", "raggiungere", {}, -1, CerchioScenario{esfiltrazione.x, esfiltrazione.y, r_esf}, -1.0, true, true, {}});
    obiettivi_.push_back({"occulto", "non_essere_scoperto", {}, -1, std::nullopt, -1.0, false, true, {}});
    chiusura_ = {ChiusuraScenario::Tipo::dentro_poi_dentro, {}, {zona_.x, zona_.y, r}, {esfiltrazione.x, esfiltrazione.y, r_esf}};
}


std::string data_testo(int g, int m, int a, int lingua) {
    static const char* const MESI_IT[] = {"gennaio", "febbraio", "marzo", "aprile", "maggio", "giugno", "luglio", "agosto",
                                          "settembre", "ottobre", "novembre", "dicembre"};
    static const char* const MESI_EN[] = {"January", "February", "March", "April", "May", "June", "July", "August",
                                          "September", "October", "November", "December"};
    return std::to_string(g) + " " + (lingua == IT ? MESI_IT : MESI_EN)[m - 1] + " " + std::to_string(a);
}

MissioneRapida Generatore::genera(const std::string& codice) {
    MissioneRapida m;
    m.parametri = p_;
    m.codice = codice;
    std::string compatto;
    for (const char c : codice)
        if (c != '-') compatto += c;
    m.id = "RAPIDA_" + compatto;
    // data e ora
    static constexpr int MESI[4][3] = {{12, 1, 2}, {3, 4, 5}, {6, 7, 8}, {9, 10, 11}};
    m.anno = 1989;
    m.mese = MESI[static_cast<int>(p_.stagione) - 1][intero(r_, 0, 2)];
    m.giorno = intero(r_, 1, 28);
    const Geografiche c = f_.centro();
    const auto elevazione = [&](double ore) { return elevazione_sole({m.anno, m.mese, m.giorno, ore}, c.lat, c.lon); };
    const double mezzogiorno = std::fmod(12.0 - c.lon / 15.0 + 48.0, 24.0);
    std::optional<double> alba, tramonto;
    for (int i = 0; i < 288; ++i) {
        const double a = i / 12.0, b = (i + 1) / 12.0;
        if (elevazione(a) < 0.0 && elevazione(b) >= 0.0 && !alba) alba = b;
        if (elevazione(a) >= 0.0 && elevazione(b) < 0.0 && !tramonto) tramonto = b;
    }
    double ora = mezzogiorno;
    switch (p_.ora) {
        case MomentoRapido::alba: ora = alba ? *alba + r_.uniforme(-0.5, 0.25) : mezzogiorno; break;
        case MomentoRapido::tramonto: ora = tramonto ? *tramonto + r_.uniforme(-0.25, 0.5) : mezzogiorno; break;
        case MomentoRapido::notte: ora = mezzogiorno + 12.0 + r_.uniforme(-2.5, 2.5); break;
        default: ora = mezzogiorno + r_.uniforme(-3.0, 3.0); break;
    }
    m.ora_utc = std::round(std::fmod(ora + 48.0, 24.0) * 60.0) / 60.0;
    // nome dell'operazione
    const std::string op = scegli(r_, d_.operazioni[static_cast<std::size_t>(lato_)]);
    m.titolo_it = "Operazione " + op;
    m.titolo_en = "Operation " + op;
    // il giocatore e la situazione
    {
        UnitaGen g;
        g.id = 0;
        g.classe = lato_ == USA ? "los_angeles" : "alfa";
        g.categoria = "sottomarino";
        g.parte = PARTI[lato_];
        g.flag = lato_ == USA ? 0x09 : 0x11;
        g.giocatore = true;
        g.velocita = 5.0;
        unita_.push_back(g);
    }
    switch (p_.tipo) {
        case TipoRapida::pattuglia: pattuglia(); break;
        case TipoRapida::caccia: caccia(); break;
        case TipoRapida::antinave: antinave(); break;
        case TipoRapida::scorta: scorta(); break;
        case TipoRapida::ricognizione: ricognizione(); break;
        case TipoRapida::minamento: minamento(); break;
        case TipoRapida::inserimento: inserimento(); break;
        case TipoRapida::a_caso: throw std::logic_error("tipo non risolto");
    }
    durata_ = std::round(durata_ / 60.0) * 60.0;
    obiettivi_.push_back({"vivo", "sopravvivere", {}, -1, std::nullopt, -1.0, true, true, {}});
    m.conta_carriera = p_.carriera && p_.difficolta >= 3 && p_.paranoia >= 1;
    // testi
    const toml::node_view<const toml::node> t{d_.tab};
    const auto& g = unita_[0];
    std::map<std::string, std::string> v[2];
    char ora_s[8];
    const int minuti = static_cast<int>(std::lround(m.ora_utc * 60.0)) % 1440;
    std::snprintf(ora_s, sizeof ora_s, "%02d%02dZ", minuti / 60, minuti % 60);
    char rotta_s[8];
    std::snprintf(rotta_s, sizeof rotta_s, "%03d°", static_cast<int>(std::lround(rilevamento(g.pos, zona_))) % 360);
    for (int l = 0; l < 2; ++l) {
        auto& x = v[l];
        x["operazione"] = op;
        x["data"] = data_testo(m.giorno, m.mese, m.anno, l);
        x["ora"] = ora_s;
        x["teatro"] = t_.nome[static_cast<std::size_t>(l)];
        x["comando"] = t_.comando[static_cast<std::size_t>(lato_)][static_cast<std::size_t>(l)];
        x["battello"] = testi(t["briefing"]["comune"]["battello"][PARTI[lato_]])[static_cast<std::size_t>(l)];
        x["zona"] = zona_nome_[static_cast<std::size_t>(l)];
        x["rotta"] = rotta_s;
        x["distanza"] = num(distanza(g.pos, zona_) / MIGLIO, 0);
        x["forze"] = elenco(forze_briefing_, l);
        x["amici"] = elenco(amici_briefing_, l);
        x["bersaglio"] = bersaglio_classe_.empty() ? "" : nome_singolo(bersaglio_classe_, l);
        if (minimo_convoglio_ > 0)
            x["bersaglio"] = l == IT ? "almeno " + std::to_string(minimo_convoglio_) + " mercantili del convoglio"
                                     : "at least " + std::to_string(minimo_convoglio_) + " of the convoy's merchant ships";
        const int ore = static_cast<int>(durata_ / 3600.0), min = static_cast<int>(std::lround(durata_ / 60.0)) % 60;
        x["durata"] = l == IT ? std::to_string(ore) + " ore" + (min ? " e " + std::to_string(min) + " minuti" : "")
                              : std::to_string(ore) + " hours" + (min ? " " + std::to_string(min) + " minutes" : "");
        x["uscita"] = uscita_nome_;
        x["mine"] = std::to_string(mine_);
        x["minimo"] = "";
        x["azione"] = testi(t["briefing"]["caccia"]["azione"][situazione_ == "guerra" ? "guerra" : "crisi"])[static_cast<std::size_t>(l)];
    }
    Righe briefing;
    const auto aggiungi_righe = [&](const Righe& r) {
        for (int l = 0; l < 2; ++l)
            for (const auto& riga : r[static_cast<std::size_t>(l)]) {
                const std::string s = sostituisci(riga, v[l]);
                if (!s.empty()) briefing[static_cast<std::size_t>(l)].push_back(s);
            }
    };
    const auto aggiungi_testo = [&](const Testi& x) { aggiungi_righe({std::vector<std::string>{x[IT]}, std::vector<std::string>{x[EN]}}); };
    aggiungi_righe(righe(t["briefing"]["comune"]["apertura"]));
    aggiungi_righe(righe(t["briefing"][nome_tipo_rapida(p_.tipo)][PARTI[lato_]]));
    {
        const Righe intel = righe(t["briefing"]["comune"]["intel"]);
        const std::size_t i = p_.difficolta <= 2 ? 0 : p_.difficolta == 3 ? 1 : 2;
        if (intel[IT].size() > i) aggiungi_testo({intel[IT][i], intel[EN][i]});
    }
    aggiungi_testo(testi(t["briefing"]["comune"]["roe"][situazione_]));
    bool aerei_ostili = false;
    for (const auto& a : aerei_) aerei_ostili = aerei_ostili || a.contro_giocatore;
    if (aerei_ostili) aggiungi_testo(testi(t["briefing"]["comune"]["asw"][PARTI[lato_]]));
    aggiungi_testo(testi(t["briefing"]["comune"]["durata"]));
    aggiungi_testo(testi(t["briefing"]["comune"]["carriera"][m.conta_carriera ? "si" : "no"]));
    m.briefing_it = briefing[IT];
    m.briefing_en = briefing[EN];
    // testi degli obiettivi
    for (auto& o : obiettivi_) {
        std::string chiave = o.id;
        if (o.id == "settore") chiave = "raggiungere_zona";
        else if (o.id == "contatti" || o.id == "rada") chiave = "scoprire";
        else if (o.id == "bersaglio") chiave = o.tipo == "affondare" ? "affondare_sottomarino" : "scoprire";
        else if (o.id == "pregio") chiave = "affondare_pregio";
        else if (o.id == "convoglio") chiave = "affondare_minimo";
        else if (o.id == "gruppo") chiave = "proteggere";
        else if (o.id == "campo") chiave = "minare";
        else if (o.id == "osservazione") chiave = "raggiungere_zona";
        else if (o.id == "non_attaccare") chiave = "proteggere";
        Testi x = testi(t["obiettivi"][chiave]);
        if (o.id == "non_attaccare") x = {"Non attaccare le unita' osservate", "Do not attack the observed units"};
        for (int l = 0; l < 2; ++l) {
            auto valori = v[l];
            valori["minimo"] = std::to_string(o.minimo);
            x[static_cast<std::size_t>(l)] = sostituisci(x[static_cast<std::size_t>(l)], valori);
        }
        o.testo = x;
    }
    // lo scenario (libreria dei comportamenti 1) e il file
    ScenarioMissione& sc = m.scenario;
    sc.id = m.id;
    sc.titolo = {m.titolo_it, m.titolo_en};
    sc.teatro = p_.teatro;
    sc.alfa = p_.alfa;
    sc.centro_lat = c.lat;
    sc.centro_lon = c.lon;
    sc.giorno = m.giorno;
    sc.mese = m.mese;
    sc.anno = m.anno;
    sc.ora_utc = m.ora_utc;
    sc.meteo_realistico = p_.meteo_realistico;
    sc.situazione = situazione_;
    sc.paranoia = p_.paranoia;
    sc.difficolta = p_.difficolta;
    sc.durata_s = durata_;
    sc.libreria = 1;
    if (lato_ == USA) {
        sc.carico.tubi = {"mk48", "mk48", "mk48", "mk48", "esca", "esca", "harpoon"};
        if (tipo_missione_ == "antinave") {
            sc.carico.dotazione = {{"mk48", 18}, {"harpoon", 4}, {"tomahawk", 4}, {"esca", 6}};
            sc.carico.deposito = {{"mk48", 17}, {"harpoon", 4}, {"tomahawk", 4}, {"esca", 6}};
        } else {
            sc.carico.dotazione = {{"mk48", 22}, {"harpoon", 2}, {"esca", 6}};
            sc.carico.deposito = tipo_missione_ == "minamento"
                                     ? std::vector<VoceCarico>{{"mk48", 13}, {"harpoon", 2}, {"esca", 6}, {mina_, std::max(mine_, 10)}}
                                     : std::vector<VoceCarico>{{"mk48", 21}, {"harpoon", 2}, {"esca", 6}};
        }
    } else {
        sc.carico.tubi = {"type53", "type53", "type53", "type53", "type53", "esca", "esca"};
        sc.carico.dotazione = {{"type53", 18}, {"esca", 4}};
        sc.carico.deposito = tipo_missione_ == "minamento"
                                 ? std::vector<VoceCarico>{{"type53", 4}, {"esca", 4}, {mina_, std::max(mine_, mina_ == "mdt" ? 20 : 6)}}
                                 : std::vector<VoceCarico>{{"type53", 18}, {"esca", 4}};
    }
    sc.carico.tipo_missione = tipo_missione_ == "scorta" && lato_ == URSS ? "antisom" : tipo_missione_;
    sc.carico.ospiti = ospiti_;
    if (m.conta_carriera) {
        static const char* const AVVERSARIO[] = {"impreparato", "regolare", "veterano", "elite"};
        sc.carriera = CarrieraScenario{situazione_, AVVERSARIO[p_.paranoia], p_.difficolta, p_.tipo == TipoRapida::inserimento,
                                       p_.teatro == "barents"};
    }
    sc.unita = unita_;
    sc.obiettivi = obiettivi_;
    sc.aerei_asw = aerei_;
    sc.briefing = briefing;
    sc.esito_fine = d_.base.esito_fine;
    sc.esito_tempo = d_.base.esito_tempo;
    sc.chiusura = chiusura_;
    sc.rapida = OrigineRapida{p_.versione, codice, nome_tipo_rapida(p_.tipo), p_.seme, p_.forze, p_.paranoia, p_.difficolta,
                              p_.asw_aereo, p_.meteo_realistico, p_.carriera};
    m.lua = scrivi_missione_lua(sc, d_.base);
    return m;
}

}  // namespace

const char* nome_tipo_rapida(TipoRapida t) {
    switch (t) {
        case TipoRapida::pattuglia: return "pattuglia";
        case TipoRapida::caccia: return "caccia";
        case TipoRapida::antinave: return "antinave";
        case TipoRapida::scorta: return "scorta";
        case TipoRapida::ricognizione: return "ricognizione";
        case TipoRapida::minamento: return "minamento";
        case TipoRapida::inserimento: return "inserimento";
        case TipoRapida::a_caso: break;
    }
    return "a_caso";
}

std::string codice_missione(const ParametriRapida& p) {
    const std::string s = caratteri_codice(p);
    return s.substr(0, 4) + "-" + s.substr(4, 4) + "-" + s.substr(8, 5);
}

std::optional<ParametriRapida> leggi_codice(std::string_view codice) {
    std::string s;
    for (char c : codice) {
        if (c == '-' || c == ' ') continue;
        if (c >= 'a' && c <= 'z') c = static_cast<char>(c - 'a' + 'A');
        if (c == 'O') c = '0';
        if (c == 'I' || c == 'L') c = '1';
        s += c;
    }
    if (s.size() != CARATTERI) return std::nullopt;
    Bit d;
    for (const char c : s) {
        const char* p = std::strchr(ALFABETO, c);
        if (!p || c == '\0') return std::nullopt;
        d.scrivi(static_cast<std::uint64_t>(p - ALFABETO), 5);
    }
    int pos = BIT_DATI;
    if (static_cast<int>(d.leggi(pos, 6)) != crc6(d, BIT_DATI)) return std::nullopt;
    pos = 0;
    ParametriRapida p;
    p.versione = static_cast<int>(d.leggi(pos, 3));   // le versioni vecchie si leggono ancora: stessi byte di allora
    if (p.versione < 1 || p.versione > VERSIONE_RAPIDE) return std::nullopt;
    const int teatro = static_cast<int>(d.leggi(pos, 3));
    if (teatro > 7) return std::nullopt;
    p.teatro = teatro == 0 ? "" : TEATRI_RAPIDE[teatro - 1];
    p.stagione = static_cast<StagioneRapida>(d.leggi(pos, 3));
    p.ora = static_cast<MomentoRapido>(d.leggi(pos, 3));
    p.meteo_realistico = d.leggi(pos, 1) != 0;
    p.tipo = static_cast<TipoRapida>(d.leggi(pos, 4));
    p.alfa = d.leggi(pos, 1) != 0;
    p.forze = static_cast<int>(d.leggi(pos, 2));
    p.paranoia = static_cast<int>(d.leggi(pos, 2));
    p.difficolta = static_cast<int>(d.leggi(pos, 3));
    p.asw_aereo = d.leggi(pos, 1) != 0;
    p.carriera = d.leggi(pos, 1) != 0;
    p.seme = static_cast<std::uint32_t>(d.leggi(pos, 32));
    if (!parametri_validi(p)) return std::nullopt;
    return p;
}

ParametriRapida risolvi(const ParametriRapida& p, const std::string& cartella_dati) {
    return risolvi_con(p, carica_dati(cartella_dati));
}

bool compatibile(const std::string& teatro, TipoRapida tipo, bool alfa, const std::string& cartella_dati) {
    if (tipo == TipoRapida::a_caso || teatro.empty()) return true;
    const DatiRapide d = carica_dati(cartella_dati);
    return d.teatri.count(teatro) && peso(d, teatro, tipo, alfa) > 0;
}

MissioneRapida genera_missione_rapida(const ParametriRapida& p, const std::string& cartella_dati) {
    const std::string codice = codice_missione(p);   // valida i parametri
    const DatiRapide d = carica_dati(cartella_dati);
    const ParametriRapida q = risolvi_con(p, d);
    if (peso(d, q.teatro, q.tipo, q.alfa) <= 0)
        throw std::runtime_error(std::string("missione rapida: ") + nome_tipo_rapida(q.tipo) + " non si gioca in " + q.teatro +
                                 (q.alfa ? " con l'Alfa" : " con il 688"));
    Generatore g(q, d, cartella_dati);
    return g.genera(codice);
}

std::filesystem::path cartella_missioni_rapide() { return cartella_fascicoli().parent_path() / "missioni_rapide"; }

}  // namespace m688
