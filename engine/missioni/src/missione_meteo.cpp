// Meteo e mare dell'Enhanced nella missione (docs/15_meteo_e_mare.md, M2): il generatore di nucleo/meteo per il teatro,
// il mese e l'ora della missione; il meteo nel punto del giocatore per l'acustica, l'elettronica e i periscopi.

#include <algorithm>
#include <cmath>
#include <cstdio>

#include "missioni/missione.hpp"
#include "missioni/periscopi_missione.hpp"
#include "nucleo/audio_meteo.hpp"

extern "C" {
#include "lua.h"
}

namespace m688 {

namespace {

std::uint64_t mescola(std::uint64_t x) {
    x += 0x9e3779b97f4a7c15ull;
    x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ull;
    x = (x ^ (x >> 27)) * 0x94d049bb133111ebull;
    return x ^ (x >> 31);
}

std::uint64_t hash_testo(const std::string& s) {
    std::uint64_t h = 0xcbf29ce484222325ull;   // FNV-1a
    for (const unsigned char c : s) { h ^= c; h *= 0x100000001b3ull; }
    return h;
}

}  // namespace

void Missione::meteo_enhanced(const std::string& cartella_dati) {
    if (meteo_ || meteo_tentato_) return;
    meteo_tentato_ = true;
    try {
        dati_meteo_ = std::make_unique<DatiMeteo>(carica_meteo(cartella_dati + "/meteo.toml"));
    } catch (const std::exception&) {
        return;   // senza i dati: il meteo semplificato di prima
    }
    // seme: teatro, data e missione (dossier 21 §4), con quello della partita (il replay ridà lo stesso meteo)
    const std::uint64_t seme = mescola(hash_testo(teatro_) ^ mescola(hash_testo(id_)) ^
                                       mescola(static_cast<std::uint64_t>(anno_ * 10000 + mese_data_ * 100 + giorno_)) ^ seme_);
    const int mese = mese_ > 0 ? mese_ : mese_data_ > 0 ? mese_data_ : clima_ ? clima_->mese_predefinito : 1;
    const double ora_locale = std::fmod(ora_utc_s_ / 3600.0 + longitudine_ / 15.0 + 48.0, 24.0);
    meteo_ = std::make_unique<Meteo>(*dati_meteo_, teatro_, mese, ora_locale, seme);
    meteo_->imposta_origine(latitudine_, longitudine_);
    meteo_->imposta_anno(anno_);
    if (modo_meteo_ == ModoMeteo::semplificato) meteo_->fissa(meteo_semplificato());
    meteo_fisso_della_missione();   // E6: il meteo dell'autore (`meteo = { fisso = {...} }`) vale anche col semplificato
    caso_meteo_ = Casuale(seme ^ 0xAFF10);
    prossimo_meteo_ = tempo_;
    passo_meteo();
}

// E6: `meteo = { fisso = { regime, vento_m_s, vento_da_gradi, raffica_m_s, stato_mare, hs_swell_m, swell_da_gradi,
// nubi_ottavi, plafond_m, pioggia_mm_h, neve_mm_h, visibilita_m, nebbia, lampi_al_minuto, t_aria_c, t_mare_c,
// pressione_hpa, ghiaccio } }`: i campi dati sopra il meteo del teatro all'inizio, fermi per tutta la missione
void Missione::meteo_fisso_della_missione() {
    lua_rawgeti(L_, LUA_REGISTRYINDEX, rif_missione_);
    lua_getfield(L_, -1, "meteo");
    if (!lua_istable(L_, -1)) { lua_pop(L_, 2); return; }
    lua_getfield(L_, -1, "fisso");
    if (!lua_istable(L_, -1)) { lua_pop(L_, 3); return; }
    const int f = lua_gettop(L_);
    meteo_->avanza(tempo_);
    StatoMeteo s = meteo_->stato();
    const auto numero = [&](const char* k, double& x) {
        lua_getfield(L_, f, k);
        if (lua_isnumber(L_, -1)) x = lua_tonumber(L_, -1);
        lua_pop(L_, 1);
    };
    lua_getfield(L_, f, "regime");
    if (lua_type(L_, -1) == LUA_TSTRING)
        if (const auto r = regime_da_nome(lua_tostring(L_, -1))) s.regime = *r;
    lua_pop(L_, 1);
    numero("vento_m_s", s.vento_m_s);
    numero("vento_da_gradi", s.vento_da_gradi);
    s.raffica_m_s = std::max(s.raffica_m_s, s.vento_m_s * 1.3);
    numero("raffica_m_s", s.raffica_m_s);
    double mare = -1.0;
    numero("stato_mare", mare);
    if (mare >= 0.0) s.hs_vento_m = centro_douglas(static_cast<int>(std::lround(mare)));
    numero("hs_swell_m", s.hs_swell_m);
    numero("swell_da_gradi", s.swell_da_gradi);
    numero("nubi_ottavi", s.nubi_ottavi);
    numero("plafond_m", s.plafond_m);
    numero("pioggia_mm_h", s.pioggia_mm_h);
    numero("neve_mm_h", s.neve_mm_h);
    numero("visibilita_m", s.visibilita_m);
    lua_getfield(L_, f, "nebbia");
    if (lua_type(L_, -1) == LUA_TSTRING) {
        const std::string n = lua_tostring(L_, -1);
        s.nebbia = n == "avvezione" ? Nebbia::avvezione : n == "fumo_di_mare" ? Nebbia::fumo_di_mare
                 : n == "radiazione" ? Nebbia::radiazione : n == "precipitazione" ? Nebbia::precipitazione : Nebbia::nessuna;
    }
    lua_pop(L_, 1);
    numero("lampi_al_minuto", s.lampi_al_minuto);
    numero("t_aria_c", s.t_aria_c);
    numero("t_mare_c", s.t_mare_c);
    numero("pressione_hpa", s.pressione_hpa);
    numero("ghiaccio", s.ghiaccio);
    meteo_->fissa(s);
    lua_pop(L_, 3);
}

// Ogni secondo: il meteo al tempo della missione e nel punto del giocatore
void Missione::passo_meteo() {
    if (!meteo_ || tempo_ < prossimo_meteo_) return;
    prossimo_meteo_ = tempo_ + 1.0;
    meteo_->avanza(tempo_);
    const auto* g = giocatore();
    meteo_qui_ = g ? meteo_->in(g->pos) : meteo_->stato();
    if (g && !interventi_.empty()) zone_meteo(interventi_, stati_interventi_, g->pos, meteo_qui_);   // le zone del vetrino di Dio
    if (radio_) bollettini_meteo();
    // i fulmini che toccano il mare entro 40 km: transitori per il sonar (dossier 21 §6.2: fino a 30-40 km, anche
    // falsi allarmi); sentiti dal giocatore, l'avviso del transitorio al piu' una volta al minuto
    if (catalogo_ && g && !meteo_->fisso() && dati_meteo_) {
        for (const auto& l : meteo_->lampi(ultimi_lampi_, tempo_)) {
            if (!l.nube_mare || distanza(l.pos, g->pos) > 40000.0) continue;
            for (auto& x : transitori_lampo(l, prossimo_fulmine_, seme_ ^ static_cast<std::uint64_t>(prossimo_fulmine_),
                                            dati_meteo_->generale.lampi_intervallo_ms / 1000.0))
                fulmini_.push_back(std::move(x));
            prossimo_fulmine_ += l.colpi;
        }
        ultimi_lampi_ = tempo_;
        if (!fulmini_.empty() && tempo_ - ultimo_avviso_fulmine_ >= 60.0) {
            std::vector<Ricevitore> ric;
            for (const Sensore* sn : sensori_acustici(*g, TipoSensori::passivi)) ric.push_back(ricevitore_acustico(*g, sn));
            for (const auto& t : fulmini_) {
                if (tempo_ < t.t0) continue;
                const auto sor = sorgente_transitoria(*catalogo_, t, tempo_, 1.0);
                if (!sor) continue;
                const AscoltoTransitorio x = ascolta_transitorio(*sor, ric, ambiente_acustico(g->pos, t.pos));
                if (!x.sentito) continue;
                fulmini_sentiti_.push_back({tempo_, normalizza_360(x.rilevamento)});
                messaggio_sonar(MSG_PERI_FULMINE);   // un transitorio senza contatto (il fulmine)
                ultimo_avviso_fulmine_ = tempo_;
                break;
            }
        }
        std::erase_if(fulmini_, [&](const Transitorio& t) { return transitorio_finito(*catalogo_, t, tempo_); });
    }
    // la vela affiora a quota periscopica col mare grosso (dossier 21 §7.3): al minuto 0,2% a mare 4, 1% a 5, 5% a 6,
    // 10% oltre; piano (sotto i 4 nodi) o col mare al traverso il conto pieno, altrimenti un terzo (STIMA). Dura 15 s
    if (g && !g->distrutta && g->quota > 0.5 && g->quota <= QUOTA_PERISCOPICA_MISSIONE_M + 1.5 && !vela_affiorata()) {
        const int mare = meteo_qui_.stato_mare();
        const double al_minuto = mare <= 3 ? 0.0 : mare == 4 ? 0.002 : mare == 5 ? 0.01 : mare == 6 ? 0.05 : 0.10;
        const bool traverso = std::abs(std::abs(normalizza_180(g->rotta - meteo_qui_.vento_da_gradi)) - 90.0) < 30.0;
        const double p = al_minuto / 60.0 * (g->velocita < 4.0 || traverso ? 1.0 : 1.0 / 3.0);
        if (p > 0.0 && caso_meteo_.uniforme() < p) {
            affiora_fino_ = tempo_ + 15.0;
            messaggi_peri_.push_back({tempo_, g->id, MSG_PERI_AFFIORA});
        }
    }
}

// --- bollettini alla sala radio (dossier 21 §5.2-5.3) ---

namespace {

std::string intero(double x) { return std::to_string(static_cast<long>(std::lround(x))); }

// le parole della visibilita' (scala dei bollettini marini)
const char* vista_it(double m) { return m >= 10000.0 ? "buona" : m >= 4000.0 ? "moderata" : m >= 1000.0 ? "scarsa" : "nebbia"; }
const char* vista_en(double m) { return m >= 10000.0 ? "GOOD" : m >= 4000.0 ? "MODERATE" : m >= 1000.0 ? "POOR" : "FOG"; }

}  // namespace

// Il WEAX (o la «метеосводка») emesso a `emessa`: vento, mare e visibilita' a 12 e 24 ore con la tendenza, dalla
// previsione del 1989 (che sbaglia); senza polar low, temporali e trombe marine
MessaggioRadio Missione::bollettino(double emessa, bool chiesto) const {
    const bool sovietico = radio_->lato() == LatoRadio::urss;
    const StatoMeteo a = meteo_->previsione(emessa, emessa + 12.0 * 3600.0), b = meteo_->previsione(emessa, emessa + 24.0 * 3600.0);
    const auto riga_it = [](const StatoMeteo& x, const char* quando) {
        return std::string(quando) + ": vento da " + intero(x.vento_da_gradi) + " " + intero(x.vento_m_s / NODO) + " nodi (forza " +
               std::to_string(beaufort(x.vento_m_s)) + "), mare " + std::to_string(x.stato_mare()) + ", visibilita' " +
               vista_it(x.visibilita_m) + (x.pioggia_mm_h + x.neve_mm_h > 0.1 ? (x.neve_mm_h > x.pioggia_mm_h ? ", neve" : ", pioggia") : "") +
               (x.nebbia != Nebbia::nessuna ? ", possibili banchi di nebbia" : "") + ".";
    };
    const auto riga_en = [](const StatoMeteo& x, const char* quando) {
        return std::string(quando) + ": WIND " + intero(x.vento_da_gradi) + "/" + intero(x.vento_m_s / NODO) + "KT, SEA STATE " +
               std::to_string(x.stato_mare()) + ", VIS " + vista_en(x.visibilita_m) +
               (x.pioggia_mm_h + x.neve_mm_h > 0.1 ? (x.neve_mm_h > x.pioggia_mm_h ? ", SNOW" : ", RAIN") : "") +
               (x.nebbia != Nebbia::nessuna ? ", FOG PATCHES POSSIBLE" : "") + ".";
    };
    const double dp = b.pressione_hpa - a.pressione_hpa;
    const std::string tend_it = dp < -4.0 || b.vento_m_s > a.vento_m_s + 3.0 ? "in peggioramento" : dp > 4.0 || b.vento_m_s < a.vento_m_s - 3.0 ? "in miglioramento" : "stazionario";
    const std::string tend_en = dp < -4.0 || b.vento_m_s > a.vento_m_s + 3.0 ? "DETERIORATING" : dp > 4.0 || b.vento_m_s < a.vento_m_s - 3.0 ? "IMPROVING" : "STEADY";
    MessaggioRadio m;
    m.precedenza = Precedenza::routine;
    m.classifica = "UNCLASSIFIED";
    m.da = sovietico ? "ГИДРОМЕТЕОСЛУЖБА ФЛОТА" : "NAVEASTOCEANCEN";
    m.a = sovietico ? "ПЛ" : "SUBLANT";
    m.oggetto = sovietico ? "МЕТЕОСВОДКА" : chiesto ? "WEAX (AS REQUESTED)" : "WEAX";
    m.id = "meteo_" + intero(emessa);
    m.testo.it = std::string(sovietico ? "Метеосводка" : "Bollettino meteo") + (chiesto ? " a richiesta. " : ". ") + riga_it(a, "Fra 12 ore") + " " +
                 riga_it(b, "Fra 24 ore") + " Tendenza: " + tend_it + ".";
    m.testo.en = std::string("WEAX") + (chiesto ? " AS REQUESTED. " : ". ") + riga_en(a, "NEXT 12H") + " " + riga_en(b, "NEXT 24H") +
                 " TREND " + tend_en + ".";
    return m;
}

// Ogni secondo con la sala radio: i bollettini alle ore UTC del meteo marino, il GALE (o STORM) WARNING quando la
// previsione vede la burrasca entro 24 ore, la risposta alla previsione chiesta. Chi non ascolta li perde.
void Missione::bollettini_meteo() {
    if (meteo_->fisso() || !dati_meteo_) return;
    const auto& ore = dati_meteo_->generale.bollettini_ore_utc;
    const auto prossimo_dopo = [&](double t) {
        double migliore = 1e18;
        const double oggi = ora_utc_s_ + t;
        const double base = std::floor(oggi / 86400.0) * 86400.0;
        for (int giorno = 0; giorno <= 1; ++giorno)
            for (const double h : ore) {
                const double x = base + giorno * 86400.0 + h * 3600.0 - ora_utc_s_;
                if (x > t + 1e-6) migliore = std::min(migliore, x);
            }
        return migliore;
    };
    if (prossimo_bollettino_ < 0.0) prossimo_bollettino_ = prossimo_dopo(tempo_);
    if (tempo_ >= prossimo_bollettino_) {
        radio_->programma(bollettino(tempo_, false), tempo_);
        // burrasca prevista entro 24 ore: l'avviso col suo nome (gale 34-47 nodi, storm da 48)
        double massimo = 0.0;
        for (double h = 6.0; h <= 24.0; h += 6.0) massimo = std::max(massimo, meteo_->previsione(tempo_, tempo_ + h * 3600.0).vento_m_s);
        const int livello = massimo >= 48.0 * NODO ? 2 : massimo >= 34.0 * NODO ? 1 : 0;
        if (livello > burrasca_annunciata_) {
            const bool sovietico = radio_->lato() == LatoRadio::urss;
            MessaggioRadio w = bollettino(tempo_, false);
            w.precedenza = Precedenza::priority;
            w.oggetto = sovietico ? "ШТОРМОВОЕ ПРЕДУПРЕЖДЕНИЕ" : livello == 2 ? "STORM WARNING" : "GALE WARNING";
            w.id = "burrasca_" + intero(tempo_);
            const std::string nodi = intero(massimo / NODO);
            w.testo.it = std::string(sovietico ? "Штормовое предупреждение" : livello == 2 ? "Avviso di tempesta" : "Avviso di burrasca") +
                         ": vento fino a " + nodi + " nodi entro 24 ore. " + w.testo.it;
            w.testo.en = std::string(livello == 2 ? "STORM WARNING" : "GALE WARNING") + ": WINDS TO " + nodi + "KT WITHIN 24H. " + w.testo.en;
            radio_->programma(w, tempo_);
        }
        burrasca_annunciata_ = livello;   // scesa la burrasca, un'altra si annuncia di nuovo
        prossimo_bollettino_ = prossimo_dopo(tempo_);
    }
    if (previsione_chiesta_ >= 0.0) {   // la risposta: in coda al prossimo ciclo
        radio_->programma(bollettino(previsione_chiesta_, true), tempo_ + radio_->parametri().ciclo_s * 0.5);
        previsione_chiesta_ = -1.0;
    }
}

double Missione::ultimo_bollettino(double t) const {
    const double passo = 6.0 * 3600.0;   // 00, 06, 12, 18 UTC
    return std::floor((ora_utc_s_ + t) / passo) * passo - ora_utc_s_;
}

double Missione::velocita_col_mare(const UnitaClassica& u) const {
    if (!meteo_) return 1e9;
    const StatoMeteo m = meteo_in(u.pos);
    const int mare = m.stato_mare();
    if (mare <= 3) return 1e9;
    // velocita' sostenibile col mare di prua (dossier 21 §7.2, nodi): per lunghezza (Nanuchka 60 m, fregata 130,
    // incrociatore 170, Kirov 250, portaerei 330); oltre mare 8 come a 8
    static constexpr double L[] = {60.0, 130.0, 170.0, 250.0, 330.0};
    static constexpr double V[5][5] = {{28, 29, 32, 31, 30}, {18, 25, 28, 30, 30}, {12, 18, 22, 26, 28},
                                       {8, 12, 15, 20, 24}, {5, 8, 10, 14, 18}};
    const auto it = lunghezze_.find(u.classe);
    const double lun = it != lunghezze_.end() && it->second > 0.0 ? it->second : 130.0;
    const auto& riga = V[std::min(mare, 8) - 4];
    double v = riga[4];
    if (lun <= L[0]) v = riga[0];
    else
        for (int i = 1; i < 5; ++i)
            if (lun <= L[i]) { v = riga[i - 1] + (riga[i] - riga[i - 1]) * (lun - L[i - 1]) / (L[i] - L[i - 1]); break; }
    // col mare al traverso o in poppa si va un quarto piu' forte (STIMA)
    const bool di_prua = std::abs(normalizza_180(u.rotta - m.vento_da_gradi)) < 60.0;
    return di_prua ? v : 1.25 * v;
}

}  // namespace m688
