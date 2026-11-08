// Taratura dell'Enhanced (docs/13 A6, docs/14 P5, docs/15 M6): missioni di prova in tools/taratura/missioni, molti
// semi, misure contro i valori dei dossier. Solo la regola di simulazione; il classico non c'entra.
//   taratura asw SEMI file.lua...        ASW aereo: scoperta vera, lancio, uccisione, falsi allarmi, boe (dossier 17 §9)
//   taratura periscopi SEMI file.lua     P5: un'esposizione contro la fregata, griglia di distanza, mare, ora, velocita',
//                                        periscopio e durata (dossier 11 §6.4, dossier 20 §5)
//   taratura agguato SEMI file.lua       P5: chi vede chi fra il 688 in agguato e le guardate dell'IA, 4 ore
//   taratura meteo SEMI file.lua         M6: teatri e stagioni, il 688 a quota periscopica 12 ore col meteo realistico
//   taratura artiglieria SEMI file.lua   G7 (docs/16, dossier 22 §9.10): periscopio 30 s a 3 km da una Perry in allarme,
//                                        emersione a 8 km da un Kara, battaglia di superficie lontana al sonar
// Esce un TSV per prova e un riassunto per gruppo (righe con «#»).

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <sstream>
#include <string>
#include <vector>

extern "C" {
#include "lauxlib.h"
#include "lua.h"
#include "lualib.h"
}
#include "missioni/danni_missione.hpp"
#include "missioni/danni_sottomarini.hpp"
#include "missioni/missione.hpp"
#include "missioni/periscopi_missione.hpp"
#include "missioni/stazioni_missione.hpp"
#include "nucleo/acustica.hpp"
#include "nucleo/casuale.hpp"
#include "nucleo/classi.hpp"
#include "nucleo/elettronica.hpp"
#include "nucleo/fondale.hpp"

using namespace m688;
namespace fs = std::filesystem;

namespace {

constexpr double DT = 1.0 / 9.0;

struct Parametri {
    int aereo = 1, bersaglio = 2, osservatore = 1;
    int compagno = -1;           // il secondo aereo della coppia (Be-12): la scoperta e il lancio di uno dei due
    double durata_s = 3600.0;
    double sparpaglia_m = 0.0;   // il bersaglio a caso nel disco di questo raggio (l'errore vero del cue)
    double anticipo_s = 0.0;     // e portato avanti sulla sua rotta per l'eta' del cue (il ritardo del datum)
    bool senza_fondale = false;  // acqua profonda ovunque (i confronti dove il fondale confonde)
    std::string prova;           // artiglieria: periscopio, emersione, battaglia
};

// la chiave `taratura` della missione di prova
Parametri leggi_parametri(const std::string& file) {
    Parametri p;
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);
    if (luaL_dofile(L, file.c_str()) == LUA_OK && lua_istable(L, -1)) {
        lua_getfield(L, -1, "taratura");
        if (lua_istable(L, -1)) {
            const auto n = [&](const char* k, double d) {
                lua_getfield(L, -1, k);
                const double v = lua_isnumber(L, -1) ? lua_tonumber(L, -1) : d;
                lua_pop(L, 1);
                return v;
            };
            p.aereo = static_cast<int>(n("aereo", p.aereo));
            p.compagno = static_cast<int>(n("compagno", p.compagno));
            p.bersaglio = static_cast<int>(n("bersaglio", p.bersaglio));
            p.osservatore = static_cast<int>(n("osservatore", p.osservatore));
            p.durata_s = n("durata_s", p.durata_s);
            p.sparpaglia_m = n("sparpaglia_m", p.sparpaglia_m);
            p.anticipo_s = n("anticipo_s", p.anticipo_s);
            p.senza_fondale = n("senza_fondale", 0.0) != 0.0;
            lua_getfield(L, -1, "prova");
            if (lua_type(L, -1) == LUA_TSTRING) p.prova = lua_tostring(L, -1);
            lua_pop(L, 1);
        }
    }
    lua_close(L);
    return p;
}

// una missione di prova con la regola di simulazione intera, come la partita
struct Prova {
    std::map<std::string, ClasseNave>& classi;
    std::unique_ptr<Missione> m;
    std::unique_ptr<DanniMissione> dm;
    std::unique_ptr<DanniSottomarini> ds;
    Prova(std::map<std::string, ClasseNave>& c, const std::string& file, std::uint64_t seme, bool meteo_realistico,
          bool con_fondale = true)
        : classi(c), m(std::make_unique<Missione>(file, seme)) {
        if (meteo_realistico) m->usa_meteo(Missione::ModoMeteo::realistico);
        // confronti delle fasi 3 e 4 (docs/18): le forze, la difesa, le armi e le reazioni dell'IA
        if (std::getenv("TARATURA_SENZA_FORZE")) m->usa_forze(false);
        if (std::getenv("TARATURA_SENZA_DIFESA")) m->usa_difesa(false);
        if (std::getenv("TARATURA_SENZA_ARMI_IA")) m->usa_armi_ia(false);
        if (std::getenv("TARATURA_SENZA_REAZIONI")) m->usa_reazioni(false);
        // confronti dell'acustica: il mese forzato (senza, quello della data della missione)
        if (const char* mese = std::getenv("TARATURA_MESE")) m->imposta_mese(std::atoi(mese));
        std::map<std::string, double> pescaggi;
        for (const auto& [id, k] : classi) pescaggi[id] = k.pescaggio_m;
        const std::string fdl = std::string(M688_DATI) + "/teatri/" + m->teatro() + ".fdl";
        if (con_fondale && fs::exists(fdl)) m->imposta_fondale(std::make_shared<const Fondale>(Fondale::carica(fdl)), pescaggi);
        dm = std::make_unique<DanniMissione>(classi, M688_DATI, seme, RegolaDanni::simulazione);
        ds = std::make_unique<DanniSottomarini>(classi, M688_DATI, seme);
        if (const auto* g = m->giocatore()) ds->prepara(*m, g->id);
    }
    void passo() {
        m->avanza(DT);
        dm->avanza(*m, DT);
        ds->avanza(*m, DT);
        m->raccogli_danni(ds.get(), dm.get());
    }
    bool viva(int id) const {
        const auto it = m->unita().find(id);
        return it != m->unita().end() && !it->second.distrutta && !it->second.persa;
    }
};

double mediana(std::vector<double> v) {
    if (v.empty()) return -1.0;
    std::sort(v.begin(), v.end());
    return v[v.size() / 2];
}

// --- ASW aereo ---

void taratura_asw(std::map<std::string, ClasseNave>& classi, int semi, const std::vector<std::string>& file) {
    const bool diag = std::getenv("TARATURA_DIAG") != nullptr;
    const int traccia = std::getenv("TARATURA_TRACCIA") ? std::atoi(std::getenv("TARATURA_TRACCIA")) : -1;
    const bool senza_evasione = std::getenv("TARATURA_SENZA_EVASIONE") != nullptr;   // confronti: l'IA non evade
    const bool senza_esche_mobili = std::getenv("TARATURA_SENZA_ESCHE_MOBILI") != nullptr;   // l'IA senza MG-74
    const bool senza_esm_ia = std::getenv("TARATURA_SENZA_ESM_IA") != nullptr;   // confronti della fase 2 (docs/18)
    const bool senza_sonaristi_ia = std::getenv("TARATURA_SENZA_SONARISTI_IA") != nullptr;
    const char* da = std::getenv("TARATURA_PRIMO_SEME");   // per dividere i semi fra piu' processi
    const int primo = da ? std::max(1, std::atoi(da)) : 1;
    std::printf("missione\tseme\tscoperta_s\tlancio_s\tcolpito_s\tmenomato_s\taffondato_s\tfalsi\tboe\tstato_finale\n");
    for (const auto& f : file) {
        const Parametri par = leggi_parametri(f);
        int scoperte = 0, lanci = 0, uccisioni = 0, colpiti = 0, menomati = 0;
        double falsi = 0.0, boe = 0.0, manovre = 0.0;
        std::vector<double> t_scoperta, t_lancio;
        const std::string nome = fs::path(f).stem().string();
        for (int s = primo; s < primo + semi; ++s) {
            Prova p(classi, f, static_cast<std::uint64_t>(s), false, !par.senza_fondale);
            if (senza_evasione) p.m->usa_evasione(false);
            if (senza_esche_mobili) p.m->usa_esche_mobili(false);
            if (senza_esm_ia) p.m->usa_esm_ia(false);
            if (senza_sonaristi_ia) p.m->usa_sonaristi_ia(false);
            if (par.sparpaglia_m > 0.0)
                if (auto* b = p.m->trova(par.bersaglio)) {   // uniforme nel disco, ripetibile col seme
                    Casuale caso(static_cast<std::uint64_t>(s) * 0x9E3779B97F4A7C15ull + 17);
                    const double r = par.sparpaglia_m * std::sqrt(caso.uniforme()), a = 2.0 * 3.14159265358979 * caso.uniforme();
                    b->pos = {b->pos.x + r * std::sin(a), b->pos.y + r * std::cos(a)};
                }
            if (par.anticipo_s > 0.0)
                if (auto* b = p.m->trova(par.bersaglio)) {
                    const double d = b->velocita * NODO * par.anticipo_s, a = b->rotta * 3.14159265358979 / 180.0;
                    b->pos = {b->pos.x + d * std::sin(a), b->pos.y + d * std::cos(a)};
                }
            double scoperta = -1.0, lancio = -1.0, uccisione = -1.0, colpito = -1.0, menomato = -1.0;
            int falsi_qui = 0;
            bool falsa_prima = false;
            double d_min = -1.0;   // diagnostica (TARATURA_DIAG): la boa in ascolto piu' vicina al bersaglio
            for (long i = 0; i < static_cast<long>(par.durata_s / DT); ++i) {
                p.passo();
                if (traccia == s && i % 9 == 0)   // TARATURA_TRACCIA=seme: il bersaglio e i siluri aviolanciati, ogni secondo
                    if (const auto* u = p.m->trova(par.bersaglio))
                        for (const auto& l : p.m->lanci_aerei())
                            if (const auto* w = p.m->trova(l.arma); w && !w->distrutta && !w->persa)
                                std::fprintf(stderr, "%6.0f siluro d %5.0f rlv_poppa %4.0f | w q %4.0f v %4.1f | b q %4.0f v %4.1f rotta %3.0f ev %d esche %d\n",
                                             p.m->tempo(), distanza(w->pos, u->pos),
                                             std::abs(normalizza_180(rilevamento(u->pos, w->pos) - u->rotta - 180.0)), w->quota,
                                             w->velocita, u->quota, u->velocita, u->rotta, p.m->manovre_evasione(par.bersaglio),
                                             static_cast<int>(std::count_if(p.m->unita().begin(), p.m->unita().end(), [](const auto& x) {
                                                 return x.second.classe == "esca" && !x.second.persa && !x.second.distrutta; })));
                if (diag && i % 9 == 0)
                    if (const auto* u = p.m->trova(par.bersaglio))
                        for (const auto& b : p.m->boe())
                            if (b.lanciatore == par.aereo && boa_in_ascolto(b, p.m->tempo())) {
                                const double d = distanza(posizione_boa(b, p.m->tempo()), u->pos);
                                if (d_min < 0.0 || d < d_min) d_min = d;
                            }
                if (const ControlloreAsw* k = par.compagno >= 0 ? p.m->controllore_asw(par.compagno) : nullptr)
                    if (scoperta < 0.0 && k->traccia().confermata && k->traccia().sorgente == par.bersaglio) scoperta = p.m->tempo();
                const ControlloreAsw* c = p.m->controllore_asw(par.aereo);
                if (c) {
                    const auto& tr = c->traccia();
                    if (scoperta < 0.0 && tr.confermata && tr.sorgente == par.bersaglio) scoperta = p.m->tempo();
                    const bool falsa = tr.confermata && tr.sorgente != par.bersaglio;
                    if (falsa && !falsa_prima) ++falsi_qui;
                    falsa_prima = falsa;
                }
                if (lancio < 0.0)
                    for (const auto& l : p.m->lanci_aerei())
                        if (l.aereo == par.aereo || l.aereo == par.compagno) { lancio = l.tempo; break; }
                if (colpito < 0.0)
                    for (const auto& k : p.m->colpi())
                        if (k.vittima == par.bersaglio) {
                            colpito = p.m->tempo();
                            if (diag)
                                if (const auto* u = p.m->trova(par.bersaglio))
                                    std::fprintf(stderr, "seme %d colpo: arma %s da %d danno %d | bersaglio q %.0f v %.1f fondale %.0f\n", s,
                                                 k.classe_arma.c_str(), k.lanciatore, k.danno, u->quota, u->velocita, p.m->profondita(u->pos));
                            break;
                        }
                // l'uccisione di missione (dottrina): il battello fuori combattimento; l'affondamento a parte
                if (menomato < 0.0)
                    if (const auto* sc = p.ds->sottomarino(par.bersaglio); sc && DanniSottomarini::menomato(*sc)) menomato = p.m->tempo();
                if (uccisione < 0.0 && !p.viva(par.bersaglio)) { uccisione = p.m->tempo(); if (menomato < 0.0) menomato = uccisione; break; }
            }
            if (traccia == s)
                for (const auto& e : p.m->eventi_siluri())
                    std::fprintf(stderr, "evento siluro %d t %.0f tipo %d sorgente %d\n", e.siluro, e.evento.tempo, static_cast<int>(e.evento.tipo), e.evento.sorgente);
            int nb = 0;
            for (const auto& b : p.m->boe()) nb += b.lanciatore == par.aereo || b.lanciatore == par.compagno;
            const ControlloreAsw* c = p.m->controllore_asw(par.aereo);
            std::printf("%s\t%d\t%.0f\t%.0f\t%.0f\t%.0f\t%.0f\t%d\t%d\t%s", nome.c_str(), s, scoperta, lancio, colpito, menomato, uccisione, falsi_qui, nb,
                        c ? nome_stato(c->stato()) : "-");
            if (diag)   // come i controllori arrivano alla conferma: i passaggi di stato e il perche'
                for (const int a : {par.aereo, par.compagno})
                    if (const ControlloreAsw* k = a >= 0 ? p.m->controllore_asw(a) : nullptr) {
                        std::fprintf(stderr, "seme %d aereo %d:", s, a);
                        for (const auto& x : k->storia()) std::fprintf(stderr, " %.0f %s(%s)", x.t, nome_stato(x.a), x.motivo.c_str());
                        std::fprintf(stderr, "\n");
                    }
            if (diag) {
                int esche = 0;
                for (const auto& [id, u] : p.m->unita()) esche += u.classe == "esca";
                std::printf("\t%.0f\tesche %d", d_min, esche);
            }
            std::printf("\n");
            std::fflush(stdout);
            scoperte += scoperta >= 0.0;
            lanci += lancio >= 0.0;
            uccisioni += uccisione >= 0.0;
            colpiti += colpito >= 0.0;
            menomati += menomato >= 0.0;
            falsi += falsi_qui;
            boe += nb;
            manovre += p.m->manovre_evasione(par.bersaglio);
            if (scoperta >= 0.0) t_scoperta.push_back(scoperta);
            if (lancio >= 0.0) t_lancio.push_back(lancio);
        }
        const double n = semi, ore = par.durata_s / 3600.0;
        std::printf("# %s: P(scoperta) %.2f, P(lancio) %.2f, P(colpito) %.2f, P(uccisione di missione) %.2f, P(affondato) %.2f, scoperta mediana %.0f s, lancio mediano %.0f s, "
                    "falsi allarmi %.2f/h, boe %.1f, sortite per scoperta %.1f, manovre d'evasione %.1f\n",
                    nome.c_str(), scoperte / n, lanci / n, colpiti / n, menomati / n, uccisioni / n, mediana(t_scoperta), mediana(t_lancio), falsi / n / ore, boe / n,
                    scoperte > 0 ? n / scoperte : -1.0, manovre / n);
        std::fflush(stdout);
    }
}

// --- periscopi: un'esposizione contro la fregata ---

void fissa_mare(Missione& m, int mare) {
    StatoMeteo s = meteo_semplificato();
    s.hs_vento_m = centro_douglas(mare);
    s.vento_m_s = vento_per_stato_del_mare(std::min(mare, 7));
    s.raffica_m_s = 1.3 * s.vento_m_s;
    if (m.meteo()) m.meteo()->fissa(s);
}

void taratura_periscopi(std::map<std::string, ClasseNave>& classi, int semi, const std::string& file) {
    const Parametri par = leggi_parametri(file);
    std::printf("periscopio\tdistanza_km\tmare\tora\tvelocita_kn\tesposizione_s\tP\tradar\tvista\tt_mediano_s\n");
    for (const int periscopio : {0, 1})
        for (const double km : {3.0, 6.0, 10.0})
            for (const int mare : {2, 5})
                for (const double ora : {12.0, 0.0})
                    for (const double v : {4.0, 10.0})
                        for (const double esposizione : {10.0, 30.0, 60.0}) {
                            int viste = 0, radar = 0, vista = 0;
                            std::vector<double> tempi;
                            for (int s = 1; s <= semi; ++s) {
                                Prova p(classi, file, static_cast<std::uint64_t>(s * 7919 + periscopio), false);
                                p.m->imposta_data_ora(15, 10, 1989, ora);
                                p.passo();
                                fissa_mare(*p.m, mare);
                                if (auto* f = p.m->trova(par.osservatore)) f->pos = {0.0, 1000.0 * km};
                                if (auto* g = p.m->trova(0)) g->velocita = g->velocita_ordinata = v;
                                p.m->applica({p.m->tempo(), TipoOrdine::scegli_periscopio, 0, 0, periscopio});
                                for (int i = 0; i < 9 * 20; ++i) p.passo();
                                p.m->applica({p.m->tempo(), TipoOrdine::periscopio, 0, 0, 1});
                                double fuori = -1.0, scoperta = -1.0;
                                std::string sensore;
                                for (int i = 0; i < 9 * 200; ++i) {
                                    p.passo();
                                    const auto* posto = p.m->posto_periscopi();
                                    if (fuori < 0.0 && posto && posto->stato().testa_fuori) fuori = p.m->tempo();
                                    if (fuori >= 0.0 && p.m->tempo() >= fuori + esposizione && posto->stato().comandato_su)
                                        p.m->applica({p.m->tempo(), TipoOrdine::periscopio, 0, 0, 0});
                                    const auto& u = p.m->unita().at(par.osservatore);
                                    if (const auto c = u.contatti.find(0); fuori >= 0.0 && c != u.contatti.end() &&
                                                                          (c->second.sensore == "radar" || c->second.sensore == "vista") &&
                                                                          c->second.ultima_scoperta >= fuori) {
                                        scoperta = c->second.ultima_scoperta - fuori;
                                        sensore = c->second.sensore;
                                        break;
                                    }
                                    if (fuori >= 0.0 && posto && !posto->stato().testa_fuori && p.m->tempo() > fuori + esposizione + 10.0) break;
                                }
                                if (scoperta >= 0.0) {
                                    ++viste;
                                    tempi.push_back(scoperta);
                                    (sensore == "radar" ? radar : vista) += 1;
                                }
                            }
                            std::printf("%s\t%.0f\t%d\t%s\t%.0f\t%.0f\t%.2f\t%d\t%d\t%.0f\n", periscopio ? "type18" : "type2", km, mare,
                                        ora > 6.0 ? "giorno" : "notte", v, esposizione, static_cast<double>(viste) / semi, radar, vista,
                                        mediana(tempi));
                            std::fflush(stdout);
                        }
}

// --- agguato: chi vede chi ---

void taratura_agguato(std::map<std::string, ClasseNave>& classi, int semi, const std::string& file) {
    const Parametri par = leggi_parametri(file);
    std::printf("seme\tguardate_ia\tnoi_vediamo\tloro_vedono\tesposizioni_nostre\n");
    double g = 0, n = 0, l = 0;
    for (int s = 1; s <= semi; ++s) {
        Prova p(classi, file, static_cast<std::uint64_t>(s), false);
        int guardate = 0, loro = 0, nostre = 0;
        bool testa_prima = false;
        double ultima_loro = -1e9;
        for (long i = 0; i < 9L * 4 * 3600; ++i) {
            const double t = p.m->tempo();
            // 30 s di periscopio ogni 5 minuti
            const double fase = std::fmod(t, 300.0);
            const auto* posto = p.m->posto_periscopi();
            if (posto) {
                if (fase < 1.0 && !posto->stato().comandato_su) { p.m->applica({t, TipoOrdine::periscopio, 0, 0, 1}); ++nostre; }
                if (fase >= 38.0 && fase < 39.0 && posto->stato().comandato_su) p.m->applica({t, TipoOrdine::periscopio, 0, 0, 0});
            }
            p.passo();
            bool testa = false;
            for (const auto& x : p.m->teste_ia()) testa = testa || (x.unita == par.osservatore && !x.da_script);
            if (testa && !testa_prima) ++guardate;
            testa_prima = testa;
            const auto& u = p.m->unita().at(par.osservatore);
            if (const auto c = u.contatti.find(0); c != u.contatti.end() && c->second.sensore == "vista" &&
                                                  c->second.ultima_scoperta > ultima_loro + 60.0) {
                ++loro;
                ultima_loro = c->second.ultima_scoperta;
            }
        }
        int noi = 0;
        for (const auto& x : p.m->messaggi_periscopi()) noi += x.codice == MSG_PERI_NEMICO;
        std::printf("%d\t%d\t%d\t%d\t%d\n", s, guardate, noi, loro, nostre);
        std::fflush(stdout);
        g += guardate; n += noi; l += loro;
    }
    std::printf("# agguato, 4 h: guardate dell'IA %.1f, le vediamo %.1f volte, ci vedono %.1f volte (media per seme)\n", g / semi,
                n / semi, l / semi);
}

// --- meteo: teatri e stagioni ---

void taratura_meteo(std::map<std::string, ClasseNave>& classi, int semi, const std::string& file) {
    std::ifstream in(file);
    std::stringstream ss;
    ss << in.rdbuf();
    const std::string base = ss.str();
    const CatalogoElettronico el = carica_elettronica(std::string(M688_DATI) + "/emettitori.toml");
    const Radar* aps115 = el.trova_radar("aps115");
    std::printf("teatro\tmese\tseme\tmare_medio\tore_mare5\tcoperta\taffioramenti_h\tgiro_radar\tgiro_calmo\tdip\tboe\tvel_fregata\n");
    for (const char* teatro : {"faroe", "barents", "gibilterra", "otranto", "kunashir", "celtica", "baltico"})
        for (const int mese : {1, 4, 7, 10}) {
            std::string testo = base;
            const auto sostituisci = [&](const std::string& a, const std::string& b) {
                const auto i = testo.find(a);
                if (i != std::string::npos) testo.replace(i, a.size(), b);
            };
            sostituisci("teatro = 'faroe'", std::string("teatro = '") + teatro + "'");
            char data[32];
            std::snprintf(data, sizeof data, "data = '1989-%02d-15'", mese);
            sostituisci("data = '1989-01-15'", data);
            const auto f = fs::temp_directory_path() / (std::string("m688_taratura_m6_") + teatro + "_" + std::to_string(mese) + ".lua");
            std::ofstream(f) << testo;
            double s_mare = 0, s_m5 = 0, s_cop = 0, s_aff = 0, s_giro = 0, s_calmo = 0, s_dip = 0, s_boe = 0, s_vel = 0;
            for (int s = 1; s <= semi; ++s) {
                Prova p(classi, f.string(), static_cast<std::uint64_t>(s * 101 + mese), true);
                p.passo();
                p.m->applica({p.m->tempo(), TipoOrdine::periscopio, 0, 0, 1});
                double secondi = 0, su = 0, coperta = 0, m5 = 0, dip = 0, boe = 0, affioramenti = 0;
                double mare = 0, giro = 0, calmo = 0, vel = 0;
                bool affiorata_prima = false;
                UnitaClassica fregata;
                fregata.classe = "perry";
                fregata.categoria = "nave";
                for (long i = 0; i < 9L * 12 * 3600; ++i) {
                    p.passo();
                    const bool aff = p.m->vela_affiorata();
                    if (aff && !affiorata_prima) ++affioramenti;
                    affiorata_prima = aff;
                    if (i % 9) continue;
                    ++secondi;
                    const StatoMeteo& q = p.m->meteo_qui();
                    const int sm = q.stato_mare();
                    mare += sm;
                    m5 += sm >= 5;
                    // il sonar a immersione AQS-13 e la SSQ-53B coi limiti del §8; la fregata col mare di prua
                    dip += sm <= 5 && !(sm == 5 && q.hs_m() > 3.0) && q.vento_m_s / NODO <= 35.0 && q.lampi_al_minuto <= 0.0;
                    boe += sm <= 6;
                    fregata.pos = p.m->giocatore()->pos;
                    fregata.rotta = q.vento_da_gradi;
                    vel += std::min(p.m->velocita_col_mare(fregata), 29.0);
                    // la testa: sott'acqua per l'onda e per l'errore di quota; il giro di un P-3C a 8 km contro la testa
                    if (const auto* posto = p.m->posto_periscopi(); posto && posto->stato().su) {
                        ++su;
                        coperta += posto->stato().finestra_coperta;
                        const SagomaTesta t = posto->sagoma(5.0);
                        if (aps115 && !posto->stato().finestra_coperta)
                            giro += probabilita_giro(*aps115, 8000.0, 1000.0 * 0.3048, TestaEsposta{t.sezione_m2, t.altezza_m, t.piuma}, sm, 0.7, 5.0,
                                                     q.pioggia_mm_h);
                        if (aps115) {
                            const SagomaTesta t2 = sagoma_testa(*posto->dati(), posto->stato().altezza_testa_m, 5.0);
                            calmo += probabilita_giro(*aps115, 8000.0, 1000.0 * 0.3048, TestaEsposta{t2.sezione_m2, t2.altezza_m, t2.piuma}, 2, 0.7, 5.0);
                        }
                    }
                }
                const double n = std::max(secondi, 1.0), ns = std::max(su, 1.0);
                std::printf("%s\t%d\t%d\t%.2f\t%.3f\t%.3f\t%.2f\t%.3f\t%.3f\t%.3f\t%.3f\t%.1f\n", teatro, mese, s, mare / n, m5 / n, coperta / ns,
                            affioramenti / 12.0, giro / ns, calmo / ns, dip / n, boe / n, vel / n);
                std::fflush(stdout);
                s_mare += mare / n; s_m5 += m5 / n; s_cop += coperta / ns; s_aff += affioramenti / 12.0;
                s_giro += giro / ns; s_calmo += calmo / ns; s_dip += dip / n; s_boe += boe / n; s_vel += vel / n;
            }
            const double k = semi;
            std::printf("# %s %d: mare %.2f, mare>=5 %.0f%%, testa coperta %.1f%%, affioramenti %.2f/h, giro del radar %.3f (calmo %.3f, "
                        "copertura x%.2f), dip %.0f%%, boe %.0f%%, fregata %.1f kn\n",
                        teatro, mese, s_mare / k, 100 * s_m5 / k, 100 * s_cop / k, s_aff / k, s_giro / k, s_calmo / k,
                        s_calmo > 0 ? s_giro / s_calmo : 0.0, 100 * s_dip / k, 100 * s_boe / k, s_vel / k);
            std::fflush(stdout);
            fs::remove(f);
        }
}

// --- artiglieria (G7): le tre prove del dossier 22 §9.10 ---

void taratura_artiglieria(std::map<std::string, ClasseNave>& classi, int semi, const std::string& file) {
    const Parametri par = leggi_parametri(file);
    const auto sulla_testa = [](const ImpattoArtiglieria& i) {
        using E = ImpattoArtiglieria::Esito;
        return !i.avvertimento && i.bersaglio == 0 && (i.esito == E::diretto || i.esito == E::schegge) &&
               i.parte != ParteEsposta::nessuna && i.parte != ParteEsposta::scafo && i.parte != ParteEsposta::falsatorre &&
               i.parte != ParteEsposta::plancia;
    };
    if (par.prova == "periscopio") {
        // il periscopio alzato `durata_s` dalla testa fuori: colpita (diretto o schegge) o no; il primo colpo dopo quanto
        std::printf("seme\tcolpita\tcolpi_sparati\tprimo_colpo_s\n");
        int colpite = 0;
        std::vector<double> primi;
        for (int s = 1; s <= semi; ++s) {
            Prova p(classi, file, static_cast<std::uint64_t>(s), false);
            p.passo();
            p.m->posto_di_combattimento(par.osservatore);   // in allarme
            p.m->applica({p.m->tempo(), TipoOrdine::periscopio, 0, 0, 1});
            double fuori = -1.0;
            for (int i = 0; i < 9 * 120; ++i) {
                p.passo();
                const auto* posto = p.m->posto_periscopi();
                if (fuori < 0.0 && posto && posto->stato().testa_fuori) fuori = p.m->tempo();
                if (fuori >= 0.0 && p.m->tempo() >= fuori + par.durata_s && posto && posto->stato().comandato_su)
                    p.m->applica({p.m->tempo(), TipoOrdine::periscopio, 0, 0, 0});
                if (fuori >= 0.0 && p.m->tempo() > fuori + par.durata_s + 20.0) break;
            }
            const auto& imp = p.m->impatti_artiglieria();
            const bool colpita = std::any_of(imp.begin(), imp.end(), sulla_testa);
            double primo = -1.0;
            int sparati = 0;
            for (const auto& e : p.m->eventi_artiglieria())
                if (e.tipo == TipoEventoArtiglieria::partenza) { ++sparati; if (primo < 0.0 && fuori >= 0.0) primo = e.tempo_s - fuori; }
            colpite += colpita;
            if (primo >= 0.0) primi.push_back(primo);
            std::printf("%d\t%d\t%d\t%.1f\n", s, colpita ? 1 : 0, sparati, primo);
            std::fflush(stdout);
        }
        std::printf("# G7.1 periscopio %.0f s a 3 km da una Perry in allarme: colpita %.2f (atteso circa 0,5), primo colpo mediano %.1f s\n",
                    par.durata_s, static_cast<double>(colpite) / semi, mediana(primi));
    } else if (par.prova == "emersione") {
        // in superficie: al primo colpo, 10 s di reazione e l'immersione d'emergenza; i colpi presi prima di essere sotto
        std::printf("seme\tprimo_colpo_s\tsotto_s\tdiretti\tscafo\tschegge\n");
        int con_diretti = 0, con_scafo = 0, sotto = 0, gravi = 0;
        double diretti_tot = 0.0;
        for (int s = 1; s <= semi; ++s) {
            Prova p(classi, file, static_cast<std::uint64_t>(s), false);
            StazioniMissione st(*p.m);
            double primo = -1.0, al_sicuro = -1.0;
            bool ordinata = false;
            for (long i = 0; i < 9L * static_cast<long>(par.durata_s); ++i) {
                p.passo();
                const double t = p.m->tempo();
                if (primo < 0.0)
                    for (const auto& e : p.m->eventi_artiglieria()) if (e.tipo == TipoEventoArtiglieria::partenza) { primo = e.tempo_s; break; }
                if (primo >= 0.0 && !ordinata && t >= primo + 10.0) {
                    st.consegna(*p.m, {t, TipoOrdine::immersione_emergenza});
                    ordinata = true;
                }
                if (ordinata && al_sicuro < 0.0 && p.m->sagoma_esposta(*p.m->giocatore()).vuota()) { al_sicuro = t; break; }
            }
            // i danni gravi dal modello del kit (G3): non si immerge piu', o una falla nello scafo
            const DanniArtiglieriaBattello* da = p.ds->artiglieria(0);
            const bool grave = da && da->non_si_immerge;
            const SottomarinoColpito* sc = p.ds->sottomarino(0);
            const bool falla = sc && sc->danni && !sc->danni->falle().empty();
            gravi += grave || falla;
            int diretti = 0, scafo = 0, schegge = 0;
            for (const auto& x : p.m->impatti_artiglieria()) {
                if (x.bersaglio != 0 || x.avvertimento || (al_sicuro >= 0.0 && x.tempo_impatto_s > al_sicuro)) continue;
                diretti += x.esito == ImpattoArtiglieria::Esito::diretto;
                scafo += x.esito == ImpattoArtiglieria::Esito::diretto && x.parte == ParteEsposta::scafo;
                schegge += x.esito == ImpattoArtiglieria::Esito::schegge;
            }
            con_diretti += diretti > 0;
            con_scafo += scafo > 0;
            sotto += al_sicuro >= 0.0;
            diretti_tot += diretti;
            std::printf("%d\t%.0f\t%.0f\t%d\t%d\t%d\n", s, primo, al_sicuro >= 0.0 ? al_sicuro - primo : -1.0, diretti, scafo, schegge);
            std::fflush(stdout);
        }
        std::printf("# G7.2 emersione a 8 km da un Kara: sotto %.2f, con colpi diretti %.2f (%.1f in media), scafo colpito %.2f, "
                    "danni gravi %.2f (atteso: quasi sempre sotto prima dei danni gravi)\n",
                    static_cast<double>(sotto) / semi, static_cast<double>(con_diretti) / semi, diretti_tot / semi,
                    static_cast<double>(con_scafo) / semi, static_cast<double>(gravi) / semi);
    } else if (par.prova == "battaglia") {
        // il 688 immerso a 10, 20 e 40 km dalla battaglia: l'eccesso di segnale dei colpi al sonar sferico. Due misure:
        // i campioni con un colpo nella scena (un transitorio resta in scena per la sua vita, anche nella coda che non
        // si sente: la quota dipende dalla cadenza del tiro) e i transitori sentiti almeno una volta
        std::printf("km\tseme\tcolpi\tcampioni_con_colpi\tsentiti\tse_max_db\ttransitori\ttransitori_sentiti\n");
        for (const double km : {10.0, 20.0, 40.0}) {
            long con_tot = 0, sentiti_tot = 0, tr_tot = 0, tr_sentiti_tot = 0;
            double se_max = -1e9;
            for (int s = 1; s <= semi; ++s) {
                Prova p(classi, file, static_cast<std::uint64_t>(s), false);
                p.passo();
                if (auto* g = p.m->trova(0)) g->pos = {0.0, -1000.0 * km};
                long con = 0, sentiti = 0;
                double massimo = -1e9;
                std::map<int, bool> transitori;   // id -> sentito almeno una volta
                for (long i = 0; i < 9L * static_cast<long>(par.durata_s); ++i) {
                    p.passo();
                    if (i % 4) continue;   // ogni 0,44 s: c'e' un colpo nella scena del sonar? lo si sente?
                    const auto sc = p.m->scena_acustica(0, "bqq5_sfera");
                    double se = -1e9;
                    bool c_e = false;
                    for (const auto& x : sc.sorgenti) {
                        if (x.id < 990000 || x.id > 999999) continue;
                        c_e = true;
                        const double sx = valuta(x, sc.ricevitore, sc.ambiente).se_db;
                        se = std::max(se, sx);
                        transitori[x.id] = transitori[x.id] || sx > 0.0;
                    }
                    if (!c_e) continue;
                    ++con;
                    sentiti += se > 0.0;
                    massimo = std::max(massimo, se);
                }
                int colpi = 0;
                for (const auto& e : p.m->eventi_artiglieria()) colpi += e.tipo == TipoEventoArtiglieria::partenza;
                const long tr = static_cast<long>(transitori.size());
                const long tr_sentiti = std::count_if(transitori.begin(), transitori.end(), [](const auto& x) { return x.second; });
                con_tot += con;
                sentiti_tot += sentiti;
                tr_tot += tr;
                tr_sentiti_tot += tr_sentiti;
                se_max = std::max(se_max, massimo);
                std::printf("%.0f\t%d\t%d\t%ld\t%ld\t%.1f\t%ld\t%ld\n", km, s, colpi, con, sentiti, massimo, tr, tr_sentiti);
                std::fflush(stdout);
            }
            std::printf("# G7.3 battaglia di superficie a %.0f km: con colpi in scena %ld campioni, sentiti %.2f, SE massimo %.1f dB;"
                        " transitori sentiti %.2f su %ld\n", km, con_tot,
                        con_tot ? static_cast<double>(sentiti_tot) / static_cast<double>(con_tot) : 0.0, se_max,
                        tr_tot ? static_cast<double>(tr_sentiti_tot) / static_cast<double>(tr_tot) : 0.0, tr_tot);
        }
    } else {
        std::fprintf(stderr, "%s: taratura.prova sconosciuta (periscopio, emersione, battaglia)\n", file.c_str());
    }
}

}  // namespace

int main(int argc, char** argv) {
    if (argc < 4) {
        std::fprintf(stderr, "uso: taratura asw|periscopi|agguato|meteo|artiglieria SEMI file.lua...\n");
        return 1;
    }
    const std::string modo = argv[1];
    const int semi = std::max(1, std::atoi(argv[2]));
    std::vector<std::string> file(argv + 3, argv + argc);
    auto classi = carica_classi(std::string(M688_DATI) + "/classi.toml");
    if (modo == "asw") taratura_asw(classi, semi, file);
    else if (modo == "periscopi") taratura_periscopi(classi, semi, file.front());
    else if (modo == "agguato") taratura_agguato(classi, semi, file.front());
    else if (modo == "meteo") taratura_meteo(classi, semi, file.front());
    else if (modo == "artiglieria") for (const auto& f : file) taratura_artiglieria(classi, semi, f);
    else { std::fprintf(stderr, "modo sconosciuto: %s\n", modo.c_str()); return 1; }
    return 0;
}
