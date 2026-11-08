#include "nucleo/asw_aereo.hpp"

#include <algorithm>
#include <cmath>
#include <limits>
#include <stdexcept>

#include <toml.hpp>

namespace m688 {

namespace {

// --- lettura del file ---

double num(const toml::table& t, const char* k, double d) {
    if (const auto v = t[k].value<double>()) return *v;
    if (const auto v = t[k].value<std::int64_t>()) return static_cast<double>(*v);
    return d;
}
std::string str(const toml::table& t, const char* k, const std::string& d = {}) { return t[k].value_or(d); }
bool vero(const toml::table& t, const char* k, bool d) { return t[k].value_or(d); }
std::vector<double> numeri(const toml::table& t, const char* k) {
    std::vector<double> v;
    if (const auto* a = t[k].as_array())
        for (const auto& x : *a) {
            if (const auto d = x.value<double>()) v.push_back(*d);
            else if (const auto i = x.value<std::int64_t>()) v.push_back(static_cast<double>(*i));
        }
    return v;
}
std::map<std::string, int> conti(const toml::table& t, const char* k) {
    std::map<std::string, int> m;
    if (const auto* x = t[k].as_table())
        for (auto&& [c, v] : *x) m[std::string(c.str())] = static_cast<int>(v.value_or(std::int64_t{0}));
    return m;
}

ParteAsw parte_da(const std::string& s, const std::string& dove) {
    if (s == "usa") return ParteAsw::usa;
    if (s == "urss") return ParteAsw::urss;
    throw std::runtime_error(dove + ": parte sconosciuta '" + s + "'");
}
LivelloEquipaggio livello_da(const std::string& s, const std::string& dove) {
    if (s == "esperto") return LivelloEquipaggio::esperto;
    if (s == "medio") return LivelloEquipaggio::medio;
    if (s == "scarso") return LivelloEquipaggio::scarso;
    throw std::runtime_error(dove + ": equipaggio sconosciuto '" + s + "'");
}
FunzioneBoa funzione_da(const std::string& s, const std::string& dove) {
    static const std::pair<const char*, FunzioneBoa> nomi[] = {
        {"passiva_omni", FunzioneBoa::passiva_omni}, {"passiva_dir", FunzioneBoa::passiva_dir},
        {"tarata", FunzioneBoa::tarata}, {"attiva_distanza", FunzioneBoa::attiva_distanza},
        {"attiva_dir", FunzioneBoa::attiva_dir}, {"attiva_omni", FunzioneBoa::attiva_omni},
        {"batitermografo", FunzioneBoa::batitermografo}, {"esplosiva", FunzioneBoa::esplosiva},
        {"radar_faro", FunzioneBoa::radar_faro}};
    for (const auto& [n, f] : nomi)
        if (s == n) return f;
    throw std::runtime_error(dove + ": funzione di boa sconosciuta '" + s + "'");
}
constexpr SchemaBoe SCHEMI[] = {SchemaBoe::barriera, SchemaBoe::barriera_stretta, SchemaBoe::chevron,
                                SchemaBoe::cerchio, SchemaBoe::cuneo, SchemaBoe::trappola, SchemaBoe::box_dicass,
                                SchemaBoe::cloverleaf, SchemaBoe::scacchiera, SchemaBoe::barriera_circolare,
                                SchemaBoe::spirale, SchemaBoe::intercettazione, SchemaBoe::campo, SchemaBoe::incrocio,
                                SchemaBoe::passaggio_mad, SchemaBoe::eco_esplosiva, SchemaBoe::attiva,
                                SchemaBoe::lungo_rilevamento};
SchemaBoe schema_da(const std::string& s, const std::string& dove) {
    for (const auto x : SCHEMI)
        if (s == nome_schema(x)) return x;
    throw std::runtime_error(dove + ": schema sconosciuto '" + s + "'");
}

// --- geometria ---

Vec2 verso(double rotta_deg) { return {std::sin(radianti(rotta_deg)), std::cos(radianti(rotta_deg))}; }
Vec2 piu(Vec2 a, Vec2 b, double k = 1.0) { return {a.x + k * b.x, a.y + k * b.y}; }
Vec2 polare(Vec2 c, double rotta_deg, double r) { return piu(c, verso(rotta_deg), r); }

bool attiva(FunzioneBoa f) {
    return f == FunzioneBoa::attiva_distanza || f == FunzioneBoa::attiva_dir || f == FunzioneBoa::attiva_omni;
}
bool ascolta(FunzioneBoa f) { return f == FunzioneBoa::passiva_omni || f == FunzioneBoa::passiva_dir || attiva(f); }

// un contatto classificato come non sottomarino (nave, biologico) non si ripropone per tanto (STIMA)
constexpr double MEMORIA_SCARTATI_S = 1800.0;
// l'ala fissa posa il box DICASS in due passaggi col giro in mezzo: ~4 min a meta' posa (STIMA, taratura A6)
constexpr double MEZZA_POSA_BOX_S = 240.0;
constexpr double PASSAGGIO_MAD_M = 1500.0;   // STIMA: l'elicottero esce di tanto dal fix a ogni passaggio MAD (quadrifoglio)
constexpr double PASSAGGIO_MAD_SOPRA_M = 150.0;   // STIMA: sopra il punto del fix, dove il passaggio MAD si chiude
constexpr double ACQUISIZIONE_MK46_M = 1463.0;   // 1600 yd (§6.7): oltre, il siluro sganciato sul fix vecchio non trova nulla

int intero(Casuale& c, int a, int b) {   // uniforme in [a, b]
    if (b <= a) return a;
    return a + std::min(b - a, static_cast<int>(c.uniforme() * (b - a + 1)));
}

}  // namespace

// ------------------------------------------------------------------------------------------------- nomi

const char* nome_schema(SchemaBoe s) {
    switch (s) {
        case SchemaBoe::barriera: return "barriera";
        case SchemaBoe::barriera_stretta: return "barriera_stretta";
        case SchemaBoe::chevron: return "chevron";
        case SchemaBoe::cerchio: return "cerchio";
        case SchemaBoe::cuneo: return "cuneo";
        case SchemaBoe::trappola: return "trappola";
        case SchemaBoe::box_dicass: return "box_dicass";
        case SchemaBoe::cloverleaf: return "cloverleaf";
        case SchemaBoe::scacchiera: return "scacchiera";
        case SchemaBoe::barriera_circolare: return "barriera_circolare";
        case SchemaBoe::spirale: return "spirale";
        case SchemaBoe::intercettazione: return "intercettazione";
        case SchemaBoe::campo: return "campo";
        case SchemaBoe::incrocio: return "incrocio";
        case SchemaBoe::passaggio_mad: return "passaggio_mad";
        case SchemaBoe::eco_esplosiva: return "eco_esplosiva";
        case SchemaBoe::attiva: return "attiva";
        case SchemaBoe::lungo_rilevamento: return "lungo_rilevamento";
    }
    return "?";
}

const char* nome_fase_dip(FaseDip f) {
    switch (f) {
        case FaseDip::nessuna: return "nessuna";
        case FaseDip::avvicinamento: return "avvicinamento";
        case FaseDip::calata: return "calata";
        case FaseDip::ascolto: return "ascolto";
        case FaseDip::ping: return "ping";
        case FaseDip::recupero: return "recupero";
        case FaseDip::salto: return "salto";
    }
    return "?";
}

const char* nome_stato(StatoAsw s) {
    switch (s) {
        case StatoAsw::transito: return "transito";
        case StatoAsw::ricerca: return "ricerca";
        case StatoAsw::datum: return "datum";
        case StatoAsw::contatto: return "contatto";
        case StatoAsw::localizzazione: return "localizzazione";
        case StatoAsw::inseguimento: return "inseguimento";
        case StatoAsw::attacco: return "attacco";
        case StatoAsw::verifica: return "verifica";
        case StatoAsw::perso: return "perso";
        case StatoAsw::cambio: return "cambio";
        case StatoAsw::rientro: return "rientro";
    }
    return "?";
}

char sigla_stato(StatoAsw s) {
    static constexpr char SIGLE[] = "TRDKLIAVPCX";
    return SIGLE[static_cast<int>(s)];
}

// ------------------------------------------------------------------------------------------------- dati

const DatiMezzo* DatiAsw::mezzo(const std::string& classe) const {
    const auto it = mezzi.find(classe);
    return it == mezzi.end() ? nullptr : &it->second;
}
const DatiBoa* DatiAsw::boa(const std::string& id) const {
    const auto it = boe.find(id);
    return it == boe.end() ? nullptr : &it->second;
}
const DatiSonarImmersione* DatiAsw::sonar(const std::string& id) const {
    const auto it = sonar_immersione.find(id);
    return it == sonar_immersione.end() ? nullptr : &it->second;
}

DatiAsw carica_asw_aereo(const std::string& percorso) {
    toml::table t;
    try {
        t = toml::parse_file(percorso);
    } catch (const toml::parse_error& e) {
        throw std::runtime_error(percorso + ": " + std::string(e.description()));
    }
    DatiAsw d;
    d.pensiero_s = num(t, "pensiero_s", 5.0);
    if (const auto* x = t["dislocamenti"].as_table())
        for (auto&& [k, nodo] : *x)
            if (const auto* r = nodo.as_table())
                d.dislocamenti[std::string(k.str())] = {num(*r, "dislocamento_t", 0.0), num(*r, "fattore", 1.0)};
    if (const auto* x = t["boe"].as_table())
        for (auto&& [k, nodo] : *x) {
            const auto* r = nodo.as_table();
            if (!r) continue;
            DatiBoa b;
            b.id = std::string(k.str());
            const std::string dove = percorso + ": boa '" + b.id + "'";
            b.nome = str(*r, "nome", b.id);
            b.parte = parte_da(str(*r, "parte", "usa"), dove);
            b.funzione = funzione_da(str(*r, "funzione"), dove);
            b.sensore = str(*r, "sensore");
            b.quote_m = numeri(*r, "quote_m");
            b.vite_s = numeri(*r, "vite_s");
            if (b.quote_m.empty() || b.vite_s.empty()) throw std::runtime_error(dove + " senza quote o vite");
            b.caduta_m_s = num(*r, "caduta_m_s", 30.0);
            b.attivazione_s = num(*r, "attivazione_s", 0.0);
            b.guasto = num(*r, "guasto", 0.05);
            b.sigma_rilevamento_deg = num(*r, "sigma_rilevamento_deg", 0.0);
            b.portata_m = num(*r, "portata_m", 0.0);
            b.soglia = vero(*r, "soglia", false);
            b.soglia_s = num(*r, "soglia_s", 0.0);
            b.periodo_ping_s = num(*r, "periodo_ping_s", 0.0);
            b.ping_secondi = num(*r, "ping_secondi", 0.0);
            b.impulso_s = num(*r, "impulso_s", 0.0);
            b.mare_max = num(*r, "mare_max", 9.0);
            b.falsi_allarmi_h = num(*r, "falsi_allarmi_h", 0.0);
            b.frequenze_hz = numeri(*r, "frequenze_hz");
            b.sl_db = num(*r, "sl_db", 0.0);
            b.fonte = str(*r, "fonte");
            b.conf = str(*r, "conf");
            d.boe[b.id] = std::move(b);
        }
    if (const auto* x = t["marcatori"].as_table())
        for (auto&& [k, nodo] : *x) {
            const auto* r = nodo.as_table();
            if (!r) continue;
            DatiMarcatore m;
            m.id = std::string(k.str());
            m.nome = str(*r, "nome", m.id);
            m.parte = parte_da(str(*r, "parte", "usa"), percorso + ": marcatore '" + m.id + "'");
            m.durata_s = num(*r, "durata_s", 0.0);
            m.visibile_m = num(*r, "visibile_m", 0.0);
            m.acustico = vero(*r, "acustico", false);
            m.frequenza_hz = num(*r, "frequenza_hz", 0.0);
            m.fonte = str(*r, "fonte");
            m.conf = str(*r, "conf");
            d.marcatori[m.id] = std::move(m);
        }
    if (const auto* x = t["sonar_immersione"].as_table())
        for (auto&& [k, nodo] : *x) {
            const auto* r = nodo.as_table();
            if (!r) continue;
            DatiSonarImmersione s;
            s.id = std::string(k.str());
            s.nome = str(*r, "nome", s.id);
            s.sensore = str(*r, "sensore");
            s.hover_m = num(*r, "hover_m", s.hover_m);
            s.cavo_max_m = num(*r, "cavo_max_m", s.cavo_max_m);
            s.avvicinamento_s = num(*r, "avvicinamento_s", s.avvicinamento_s);
            s.calata_m_s = num(*r, "calata_m_s", s.calata_m_s);
            s.recupero_m_s = num(*r, "recupero_m_s", s.recupero_m_s);
            s.ascolto_min_s = num(*r, "ascolto_min_s", s.ascolto_min_s);
            s.ascolto_max_s = num(*r, "ascolto_max_s", s.ascolto_max_s);
            s.ping_min = static_cast<int>(num(*r, "ping_min", s.ping_min));
            s.ping_max = static_cast<int>(num(*r, "ping_max", s.ping_max));
            s.periodo_ping_s = num(*r, "periodo_ping_s", s.periodo_ping_s);
            s.p_attivo = num(*r, "p_attivo", s.p_attivo);
            s.dip_ricerca_s = num(*r, "dip_ricerca_s", s.dip_ricerca_s);
            s.salto_ricerca_m = num(*r, "salto_ricerca_m", s.salto_ricerca_m);
            s.salto_inseguimento_m = num(*r, "salto_inseguimento_m", s.salto_inseguimento_m);
            s.salto_velocita_kn = num(*r, "salto_velocita_kn", s.salto_velocita_kn);
            s.frequenza_hz = num(*r, "frequenza_hz", s.frequenza_hz);
            s.sl_db = num(*r, "sl_db", s.sl_db);
            s.notte = vero(*r, "notte", s.notte);
            s.mare_max = num(*r, "mare_max", s.mare_max);
            s.hs_max_m = num(*r, "hs_max_m", 0.0);
            s.vento_max_kn = num(*r, "vento_max_kn", s.vento_max_kn);
            s.fonte = str(*r, "fonte");
            s.conf = str(*r, "conf");
            d.sonar_immersione[s.id] = std::move(s);
        }
    if (const auto* x = t["mezzi"].as_table())
        for (auto&& [k, nodo] : *x) {
            const auto* r = nodo.as_table();
            if (!r) continue;
            DatiMezzo m;
            m.id = std::string(k.str());
            const std::string dove = percorso + ": mezzo '" + m.id + "'";
            m.nome = str(*r, "nome", m.id);
            m.parte = parte_da(str(*r, "parte", "usa"), dove);
            m.elicottero = vero(*r, "elicottero", false);
            m.dal_anno = static_cast<int>(num(*r, "dal_anno", 0.0));
            m.radar = str(*r, "radar");
            m.sonar = str(*r, "sonar");
            if (!m.sonar.empty() && !d.sonar_immersione.count(m.sonar))
                throw std::runtime_error(dove + ": sonar a immersione '" + m.sonar + "' sconosciuto");
            if (const auto* mad = (*r)["mad"].as_table()) {
                DatiMad a;
                a.fattore = num(*mad, "fattore", 1.0);
                a.quota_giorno_m = num(*mad, "quota_giorno_m", 60.0);
                a.quota_notte_m = num(*mad, "quota_notte_m", a.quota_giorno_m);
                a.velocita_kn = num(*mad, "velocita_kn", 200.0);
                a.cavo_m = num(*mad, "cavo_m", 0.0);
                a.fonte = str(*mad, "fonte");
                a.conf = str(*mad, "conf");
                m.mad = a;
            }
            if (const auto* car = (*r)["carichi"].as_table())
                for (auto&& [nome, c] : *car)
                    if (const auto* ct = c.as_table()) {
                        DatiMezzo::Carico carico{conti(*ct, "boe"), conti(*ct, "armi"), conti(*ct, "marcatori")};
                        for (const auto& [b, n] : carico.boe)
                            if (!d.boe.count(b)) throw std::runtime_error(dove + ": boa '" + b + "' sconosciuta");
                        m.carichi[std::string(nome.str())] = std::move(carico);
                    }
            m.carico_predefinito = str(*r, "carico_predefinito");
            if (m.carichi.empty()) throw std::runtime_error(dove + " senza carichi");
            const auto pred = m.carichi.find(m.carico_predefinito);
            if (pred == m.carichi.end()) throw std::runtime_error(dove + ": carico predefinito mancante");
            m.boe = pred->second.boe;
            m.armi = pred->second.armi;
            m.marcatori = pred->second.marcatori;
            m.canali_boe = static_cast<int>(num(*r, "canali_boe", 16.0));
            m.conferma = num(*r, "conferma", 1.0);
            m.falsi_allarmi = num(*r, "falsi_allarmi", 1.0);
            m.intervallo_posa_s = num(*r, "intervallo_posa_s", 0.0);
            m.velocita_transito_kn = num(*r, "velocita_transito_kn", m.velocita_transito_kn);
            m.velocita_ricerca_kn = num(*r, "velocita_ricerca_kn", m.velocita_ricerca_kn);
            m.velocita_posa_kn = num(*r, "velocita_posa_kn", m.velocita_posa_kn);
            m.quota_transito_m = num(*r, "quota_transito_m", m.quota_transito_m);
            m.quota_ricerca_m = num(*r, "quota_ricerca_m", m.quota_ricerca_m);
            m.quota_posa_m = num(*r, "quota_posa_m", m.quota_posa_m);
            m.quota_attacco_m = num(*r, "quota_attacco_m", m.quota_attacco_m);
            m.tos_base_h = num(*r, "tos_base_h", m.tos_base_h);
            m.tos_nm_per_h = num(*r, "tos_nm_per_h", m.tos_nm_per_h);
            m.riserva_h = num(*r, "riserva_h", m.riserva_h);
            m.equipaggio = livello_da(str(*r, "equipaggio", "medio"), dove);
            m.gruppo_tipico = static_cast<int>(num(*r, "gruppo_tipico", 1.0));
            m.fonte = str(*r, "fonte");
            m.conf = str(*r, "conf");
            d.mezzi[m.id] = std::move(m);
        }
    if (const auto* x = t["equipaggi"].as_table())
        for (auto&& [k, nodo] : *x)
            if (const auto* r = nodo.as_table()) {
                DatiEquipaggio e;
                e.errore_posa_m = num(*r, "errore_posa_m", e.errore_posa_m);
                e.falsi_allarmi = num(*r, "falsi_allarmi", 1.0);
                e.ritardo = num(*r, "ritardo", 1.0);
                e.perdita_contatto = num(*r, "perdita_contatto", 1.0);
                d.equipaggi[livello_da(std::string(k.str()), percorso)] = e;
            }
    if (const auto* x = t["difficolta"].as_table())
        for (auto&& [k, nodo] : *x)
            if (const auto* r = nodo.as_table()) {
                const std::string nome(k.str());
                DifficoltaAsw l;
                if (nome == "facile") l = DifficoltaAsw::facile;
                else if (nome == "normale") l = DifficoltaAsw::normale;
                else if (nome == "realistico") l = DifficoltaAsw::realistico;
                else if (nome == "minaccia_percepita") l = DifficoltaAsw::minaccia_percepita;
                else throw std::runtime_error(percorso + ": difficolta' sconosciuta '" + nome + "'");
                DatiDifficolta f;
                f.scoperta = num(*r, "scoperta", 1.0);
                f.scoperta_urss = num(*r, "scoperta_urss", 1.0);
                const std::string e = str(*r, "equipaggio");
                if (!e.empty()) f.equipaggio = livello_da(e, percorso);
                f.notte = vero(*r, "notte", true);
                f.guasti = vero(*r, "guasti", true);
                f.deriva = vero(*r, "deriva", true);
                d.difficolta[l] = f;
            }
    if (const auto* x = t["probabilita"].as_table())
        for (auto&& [k, nodo] : *x)
            if (const auto* r = nodo.as_table()) {
                ProbabilitaAsw p;
                p.falso_allarme = num(*r, "falso_allarme", p.falso_allarme);
                p.conferma = num(*r, "conferma", p.conferma);
                p.finestra_conferma_s = num(*r, "finestra_conferma_s", p.finestra_conferma_s);
                p.fix = num(*r, "fix", p.fix);
                p.finestra_fix_s = num(*r, "finestra_fix_s", p.finestra_fix_s);
                p.mad_bassa = num(*r, "mad_bassa", p.mad_bassa);
                p.mad_alta = num(*r, "mad_alta", p.mad_alta);
                p.passaggio_contatto = num(*r, "passaggio_contatto", p.passaggio_contatto);
                p.riacquisizione = num(*r, "riacquisizione", p.riacquisizione);
                p.finestra_riacquisizione_s = num(*r, "finestra_riacquisizione_s", p.finestra_riacquisizione_s);
                p.perdita_s = num(*r, "perdita_s", p.perdita_s);
                p.rinuncia_s = num(*r, "rinuncia_s", p.rinuncia_s);
                p.scarta_vero = num(*r, "scarta_vero", p.scarta_vero);
                p.crede_falso = num(*r, "crede_falso", p.crede_falso);
                p.perso_spirale = num(*r, "perso_spirale", p.perso_spirale);
                p.attacco_hover = num(*r, "attacco_hover", p.attacco_hover);
                p.attacco_altro = num(*r, "attacco_altro", p.attacco_altro);
                d.probabilita[parte_da(std::string(k.str()), percorso)] = p;
            }
    if (const auto* x = t["schemi"].as_table())
        for (auto&& [parte, nodo] : *x)
            if (const auto* sit = nodo.as_table()) {
                const ParteAsw pa = parte_da(std::string(parte.str()), percorso);
                for (auto&& [nome, s] : *sit) {
                    const auto* st = s.as_table();
                    if (!st) continue;
                    SceltaSchema sc;
                    sc.situazione = std::string(nome.str());
                    const std::string dove = percorso + ": schemi." + std::string(parte.str()) + "." + sc.situazione;
                    if (const auto* a = (*st)["scelte"].as_array())
                        for (const auto& o : *a)
                            if (const auto* ot = o.as_table()) {
                                OpzioneSchema op;
                                op.schema = schema_da(str(*ot, "schema"), dove);
                                op.p = num(*ot, "p", 1.0);
                                op.funzione = funzione_da(str(*ot, "funzione", "passiva_dir"), dove);
                                op.boe = static_cast<int>(num(*ot, "boe", 0.0));
                                sc.scelte.push_back(op);
                            }
                    d.schemi[pa][sc.situazione] = std::move(sc);
                }
            }
    for (const ParteAsw p : {ParteAsw::usa, ParteAsw::urss})
        if (!d.probabilita.count(p)) throw std::runtime_error(percorso + ": mancano le probabilita' di una parte");
    if (!d.equipaggi.count(LivelloEquipaggio::medio) || !d.difficolta.count(DifficoltaAsw::normale))
        throw std::runtime_error(percorso + ": mancano l'equipaggio medio o la difficolta' normale");
    return d;
}

// ------------------------------------------------------------------------------------------------- sonoboe

double stabilizzazione_s(double quota_m) {
    static constexpr double Q[] = {30.0, 60.0, 120.0, 300.0}, S[] = {100.0, 125.0, 160.0, 240.0};
    if (quota_m <= Q[0]) return S[0];
    for (int i = 1; i < 4; ++i)
        if (quota_m <= Q[i]) return S[i - 1] + (S[i] - S[i - 1]) * (quota_m - Q[i - 1]) / (Q[i] - Q[i - 1]);
    return S[3] + (quota_m - Q[3]) * (S[3] - S[2]) / (Q[3] - Q[2]);
}

Sonoboa crea_boa(const DatiBoa& d, Vec2 pos, double quota_lancio_m, double t, int id, int lanciatore,
                 const DatiDifficolta& diff, Casuale& caso, double quota_m, double vita_s) {
    Sonoboa b;
    b.id = id;
    b.lanciatore = lanciatore;
    b.tipo = d.id;
    b.pos = pos;
    b.quota_m = quota_m >= 0.0 ? quota_m : d.quote_m.front();
    const double vita = vita_s >= 0.0 ? vita_s : d.vite_s.front();
    b.t_posa = t;
    b.t_impatto = t + std::max(0.0, quota_lancio_m) / std::max(1.0, d.caduta_m_s);
    if (d.funzione == FunzioneBoa::esplosiva)   // affonda a ~4 m/s fino alla quota dello scoppio (STIMA)
        b.t_attiva = b.t_impatto + b.quota_m / 4.0;
    else
        b.t_attiva = b.t_impatto + (d.attivazione_s > 0.0 ? d.attivazione_s : stabilizzazione_s(b.quota_m));
    b.t_fine = d.funzione == FunzioneBoa::esplosiva ? b.t_attiva : b.t_impatto + vita;
    b.attiva_sonar = attiva(d.funzione);
    // sempre tre estrazioni, cosi' la difficolta' non sposta il resto del flusso
    const double u_guasto = caso.uniforme(), u_v = caso.uniforme(), u_r = caso.uniforme();
    b.guasta = diff.guasti && u_guasto < d.guasto;
    if (diff.deriva) {   // 0,5-2 kn (§4.1, STIMA)
        const double v = (0.5 + 1.5 * u_v) * NODO;
        b.deriva = {v * std::sin(2.0 * PI * u_r), v * std::cos(2.0 * PI * u_r)};
    }
    return b;
}

Vec2 posizione_boa(const Sonoboa& b, double t) { return piu(b.pos, b.deriva, std::max(0.0, t - b.t_impatto)); }
bool boa_in_ascolto(const Sonoboa& b, double t) { return !b.guasta && t >= b.t_attiva && t < b.t_fine; }

bool ping_disponibile(const Sonoboa& b, const DatiBoa& d) {
    return d.ping_secondi <= 0.0 || b.ping_secondi_usati + d.impulso_s <= d.ping_secondi + 1e-9;
}

// ------------------------------------------------------------------------------------------------- schemi

double raggio_incertezza_m(double raggio0_m, double velocita_kn, double ritardo_s) {
    return raggio0_m + velocita_kn * NODO * std::max(0.0, ritardo_s);
}

std::vector<Vec2> posa_schema(SchemaBoe s, const ParametriSchema& p) {
    const double R = std::max(100.0, p.portata_m);
    const double rotta = p.rotta_deg.value_or(0.0);
    const double r_t = raggio_incertezza_m(p.raggio0_m, p.velocita_kn, p.ritardo_s);
    const auto quante = [&](int tipico) { return p.boe > 0 ? p.boe : tipico; };
    std::vector<Vec2> v;
    // linea di n boe perpendicolare a `rotta`, centrata in c, passo S, nell'ordine di volo da sinistra a destra
    const auto linea = [&](Vec2 c, int n, double S) {
        for (int i = 0; i < n; ++i) v.push_back(polare(c, rotta + 90.0, (i - (n - 1) / 2.0) * S));
    };
    const auto arco = [&](Vec2 c, double r, double centro, double ampiezza, int n) {
        for (int i = 0; i < n; ++i) {
            const double a = n == 1 ? centro : centro - ampiezza / 2.0 + ampiezza * i / (n - 1);
            v.push_back(polare(c, a, r));
        }
    };
    switch (s) {
        case SchemaBoe::barriera: linea(p.datum, quante(6), 1.75 * R); break;   // S = 1,5-2 R50 (§6.1)
        case SchemaBoe::barriera_stretta: linea(p.datum, quante(4), R); break;
        case SchemaBoe::chevron: {   // vertice verso il sottomarino, bracci a 40° in avanti, 3 + 3
            const int n = quante(6), braccio = (n + 1) / 2;
            const Vec2 vertice = polare(p.datum, rotta, r_t);
            for (int i = 1; i <= braccio; ++i) v.push_back(polare(vertice, rotta - 40.0, i * 1.25 * R));
            for (int i = 1; i <= n - braccio; ++i) v.push_back(polare(vertice, rotta + 40.0, i * 1.25 * R));
            break;
        }
        case SchemaBoe::cerchio: {   // anello di r = 0,7·(r0 + u·t) piu' la boa centrale
            const int n = quante(8);
            const double r = std::max(0.5 * R, 0.7 * r_t);
            for (int i = 0; i < n; ++i) v.push_back(polare(p.datum, rotta + 360.0 * i / n, r));
            v.push_back(p.datum);
            break;
        }
        case SchemaBoe::cuneo: {   // settore di 75° verso la fuga, archi a 1/2 e 1 portata
            const int n = quante(6), dentro = (n + 2) / 3;
            const double D = std::max(R, r_t);
            arco(p.datum, 0.5 * D, rotta, 75.0, dentro);
            arco(p.datum, D, rotta, 75.0, n - dentro);
            break;
        }
        case SchemaBoe::trappola:   // davanti alla posizione stimata, 1,5 portate oltre la strada fatta
            linea(polare(p.datum, rotta, p.velocita_kn * NODO * p.ritardo_s + 1.5 * R), quante(4), 1.5 * R);
            break;
        case SchemaBoe::box_dicass: {   // vertici attorno al fix, lato 1,5 kyd
            const int n = quante(4);
            const double r = n == 4 ? 1372.0 / std::sqrt(2.0) : 1372.0 / std::sqrt(3.0);
            for (int i = 0; i < n; ++i) v.push_back(polare(p.datum, rotta + 45.0 + 360.0 * i / n, r));
            break;
        }
        case SchemaBoe::cloverleaf:   // boa centrale e 4 a 2 nm
            v.push_back(p.datum);
            for (int i = 0; i < quante(5) - 1; ++i) v.push_back(polare(p.datum, rotta + 90.0 * i, 2.0 * MIGLIO));
            break;
        case SchemaBoe::scacchiera:
        case SchemaBoe::campo: {   // file sfalsate di mezza maglia, maglia 3 R50 (scacchiera) o 2 R50 (campo)
            const int n = quante(s == SchemaBoe::scacchiera ? 16 : 12);
            const int colonne = std::max(1, static_cast<int>(std::lround(std::sqrt(static_cast<double>(n)))));
            const int file = (n + colonne - 1) / colonne;
            const double S = (s == SchemaBoe::scacchiera ? 3.0 : 2.0) * R;
            int fatte = 0;
            for (int f = 0; f < file && fatte < n; ++f) {
                const Vec2 c = polare(p.datum, rotta, (f - (file - 1) / 2.0) * S);
                const int k = std::min(colonne, n - fatte);
                for (int i = 0; i < k; ++i) {
                    const double x = (i - (colonne - 1) / 2.0 + (f % 2 ? 0.5 : 0.0)) * S;
                    v.push_back(polare(c, rotta + 90.0, f % 2 ? -x : x));   // a serpentina
                }
                fatte += k;
            }
            break;
        }
        case SchemaBoe::barriera_circolare: {   // anello sul cerchio d'incertezza, almeno 3000 m, boe a 0,75 R50
            const double r = std::max(3000.0, r_t);
            const int n = p.boe > 0 ? p.boe
                                    : std::clamp(static_cast<int>(std::ceil(2.0 * PI * r / (0.75 * R))), 6, 24);
            for (int i = 0; i < n; ++i) v.push_back(polare(p.datum, rotta + 360.0 * i / n, r));
            break;
        }
        case SchemaBoe::spirale: {   // spirale logaritmica attorno al punto d'immersione: 6 boe a giro, +50% a giro
            const int n = quante(10);
            const double b = std::log(1.5) / (2.0 * PI);
            const double r0 = std::max(1000.0, p.raggio0_m);
            for (int i = 0; i < n; ++i) {
                const double th = 2.0 * PI * i / 6.0;
                v.push_back(polare(p.datum, rotta + gradi(th), r0 * std::exp(b * th)));
            }
            break;
        }
        case SchemaBoe::intercettazione: {   // arco di 120° davanti alla rotta, almeno 3000 m, boe a 0,75 R50
            const double r = std::max(3000.0, p.velocita_kn * NODO * p.ritardo_s + 0.5 * R);
            const int n = p.boe > 0 ? p.boe
                                    : std::clamp(static_cast<int>(std::ceil(radianti(120.0) * r / (0.75 * R))) + 1, 3, 16);
            arco(p.datum, r, rotta, 120.0, n);
            break;
        }
        case SchemaBoe::incrocio:   // due DIFAR a meta' portata, a 120° fra loro attorno al punto stimato
            v.push_back(polare(p.datum, rotta + 30.0, 0.5 * R));
            v.push_back(polare(p.datum, rotta + 150.0, 0.5 * R));
            break;
        case SchemaBoe::passaggio_mad: break;
        case SchemaBoe::eco_esplosiva:   // 2-3 colpi nello stesso punto, a 2,5 km dalla boa che ascolta
            for (int i = 0; i < quante(3); ++i) v.push_back(polare(p.datum, rotta + 90.0, 2500.0));
            break;
        case SchemaBoe::lungo_rilevamento: {   // da una portata a r0, passo 1,5 R50, mezza portata a destra e a sinistra
            const int n = quante(6);
            const double passo = std::max(1.5 * R, (p.raggio0_m - R) / std::max(1, n - 1));
            for (int i = 0; i < n; ++i)
                v.push_back(polare(polare(p.datum, rotta, R + i * passo), rotta + (i % 2 ? -90.0 : 90.0), 0.5 * R));
            break;
        }
        case SchemaBoe::attiva:
            v.push_back(p.datum);
            for (int i = 1; i < quante(1); ++i) v.push_back(polare(p.datum, rotta + 90.0 * i, 1000.0));
            break;
    }
    return v;
}

// ------------------------------------------------------------------------------------------------- MAD

double portata_mad_m(double dislocamento_t, double rumore_gamma, double fattore) {
    if (dislocamento_t <= 0.0) return 0.0;
    return fattore * 376.0 * std::cbrt(dislocamento_t / 4000.0) * std::cbrt(0.1 / std::max(1e-3, rumore_gamma));
}

double rumore_mad_col_mare(double rumore_gamma, int stato_mare) {
    const double f = stato_mare >= 6 ? 0.6 : stato_mare >= 4 ? 0.8 : 1.0;
    return rumore_gamma / (f * f * f);
}

double probabilita_mad(double r50_m, double distanza_obliqua_m) {
    if (r50_m <= 0.0) return 0.0;
    return 0.5 * std::erfc((distanza_obliqua_m - r50_m) / (0.2 * r50_m * std::sqrt(2.0)));
}

double spazzata_mad_m(double r50_m, double dz_m) {
    return std::abs(dz_m) >= r50_m ? 0.0 : std::sqrt(r50_m * r50_m - dz_m * dz_m);
}

std::vector<int> SensoreMad::passo(Vec2 aereo, double quota_aereo_m, double rumore_gamma,
                                   const std::vector<BersaglioMad>& bersagli, Casuale& caso) {
    // minimo_: distanza minima del passaggio in corso, o -1 se il passaggio e' gia' stato giocato
    std::vector<int> sentiti;
    const double quota_sensore = std::max(5.0, quota_aereo_m - 0.5 * d_.cavo_m);
    std::map<int, double> nuovi;
    for (const auto& b : bersagli) {
        const double r50 = portata_mad_m(b.dislocamento_t, rumore_gamma, d_.fattore);
        const double h = distanza(aereo, b.pos), dz = quota_sensore + std::max(0.0, b.quota_m);
        const double r = std::sqrt(h * h + dz * dz);
        const auto it = minimo_.find(b.id);
        const bool in_corso = it != minimo_.end() && it->second >= 0.0;
        if (r <= 2.0 * r50) {
            if (it == minimo_.end() || (in_corso && r <= it->second)) nuovi[b.id] = r;   // in avvicinamento
            else {
                if (in_corso && caso.uniforme() < probabilita_mad(r50, it->second)) sentiti.push_back(b.id);
                nuovi[b.id] = -1.0;   // si riallontana: punto piu' vicino passato
            }
        } else if (in_corso && caso.uniforme() < probabilita_mad(r50, it->second)) {   // uscito dalla finestra
            sentiti.push_back(b.id);
        }
    }
    minimo_ = std::move(nuovi);
    return sentiti;
}

// ------------------------------------------------------------------------------------------------- dip

double CicloDip::durata_fase(FaseDip f) const {
    if (!d_) return 0.0;
    switch (f) {
        case FaseDip::avvicinamento: return d_->avvicinamento_s;
        case FaseDip::calata: return (d_->hover_m + quota_) / std::max(0.1, d_->calata_m_s);
        case FaseDip::ascolto: return ascolto_s_;
        case FaseDip::ping: return ping_ * d_->periodo_ping_s;
        case FaseDip::recupero: return cavo_inizio_recupero_ / std::max(0.1, d_->recupero_m_s);
        default: return 0.0;
    }
}

double CicloDip::durata_stazione_s() const {
    if (!d_) return 0.0;
    return d_->avvicinamento_s + (d_->hover_m + quota_) / std::max(0.1, d_->calata_m_s) + ascolto_s_ +
           ping_ * d_->periodo_ping_s + (d_->hover_m + quota_) / std::max(0.1, d_->recupero_m_s);
}

double CicloDip::quota_trasduttore_m() const { return d_ ? std::max(0.0, cavo_ - d_->hover_m) : 0.0; }

void CicloDip::inizia(double t, double quota_m, bool inseguimento, Casuale& caso) {
    if (!d_) return;
    quota_ = std::clamp(quota_m, 10.0, d_->cavo_max_m - d_->hover_m);
    inseguimento_ = inseguimento;
    const double u_ascolto = caso.uniforme(), u_attivo = caso.uniforme();
    ping_ = intero(caso, d_->ping_min, d_->ping_max);
    if (!inseguimento && u_attivo >= d_->p_attivo) ping_ = 0;   // ricerca passiva (VGS-3: l'attivo e' raro)
    if (inseguimento) {
        ascolto_s_ = d_->ascolto_min_s + (d_->ascolto_max_s - d_->ascolto_min_s) * u_ascolto;
    } else {   // la stazione di ricerca dura dip_ricerca_s: il resto e' ascolto
        const double fisso = d_->avvicinamento_s + 2.0 * (d_->hover_m + quota_) / std::max(0.1, d_->calata_m_s) +
                             ping_ * d_->periodo_ping_s;
        ascolto_s_ = std::max(d_->ascolto_min_s, d_->dip_ricerca_s - fisso);
    }
    ping_fatti_ = 0;
    cavo_ = 0.0;
    cavo_inizio_recupero_ = d_->hover_m + quota_;
    fase_ = FaseDip::avvicinamento;
    t_ = t_fase_ = t;
    t_fine_ = t + durata_stazione_s();
}

void CicloDip::aggiorna_cavo(double t) {
    if (!d_) return;
    const double tf = t - t_fase_;
    switch (fase_) {
        case FaseDip::calata: cavo_ = std::min(d_->hover_m + quota_, tf * d_->calata_m_s); break;
        case FaseDip::ascolto:
        case FaseDip::ping: cavo_ = d_->hover_m + quota_; break;
        case FaseDip::recupero: cavo_ = std::max(0.0, cavo_inizio_recupero_ - tf * d_->recupero_m_s); break;
        default: cavo_ = 0.0; break;
    }
}

int CicloDip::avanza(double t) {
    if (!d_ || fase_ == FaseDip::nessuna || fase_ == FaseDip::salto) { t_ = t; return 0; }
    int ping = 0;
    while (fase_ != FaseDip::salto) {
        const double fine = t_fase_ + durata_fase(fase_);
        if (fase_ == FaseDip::ping)   // un ping all'inizio di ogni periodo
            while (ping_fatti_ < ping_ && t_fase_ + ping_fatti_ * d_->periodo_ping_s <= t + 1e-9) {
                ++ping_fatti_;
                ++ping;
            }
        if (t + 1e-9 < fine) break;
        static constexpr FaseDip DOPO[] = {FaseDip::nessuna, FaseDip::calata, FaseDip::ascolto, FaseDip::ping,
                                           FaseDip::recupero, FaseDip::salto, FaseDip::salto};
        const FaseDip prossima = DOPO[static_cast<int>(fase_)];
        if (prossima == FaseDip::recupero) cavo_inizio_recupero_ = d_->hover_m + quota_;
        fase_ = prossima;
        t_fase_ = fine;
    }
    aggiorna_cavo(t);
    t_ = t;
    return ping;
}

void CicloDip::forza_attivo(double t) {
    if (!d_ || fase_ != FaseDip::ascolto) return;
    ping_ = std::max(ping_, std::max(1, d_->ping_min));
    fase_ = FaseDip::ping;
    t_fase_ = t;
    ping_fatti_ = 0;
    t_fine_ = t + durata_fase(FaseDip::ping) + (d_->hover_m + quota_) / std::max(0.1, d_->recupero_m_s);
}

void CicloDip::recupera(double t) {
    if (!d_ || fase_ == FaseDip::nessuna || fase_ == FaseDip::salto || fase_ == FaseDip::recupero) return;
    aggiorna_cavo(t);
    cavo_inizio_recupero_ = cavo_;
    fase_ = cavo_ > 0.0 ? FaseDip::recupero : FaseDip::salto;
    t_fase_ = t;
    t_fine_ = t + durata_fase(FaseDip::recupero);
}

// ------------------------------------------------------------------------------------------------- utilita'

double tempo_in_zona_h(const DatiMezzo& m, double distanza_m) {
    return std::max(0.0, m.tos_base_h - distanza_m / MIGLIO / std::max(1.0, m.tos_nm_per_h));
}

double probabilita_passo(double p_finestra, double finestra_s, double dt_s) {
    const double p = std::clamp(p_finestra, 0.0, 1.0);
    if (p >= 1.0) return 1.0;
    if (finestra_s <= 0.0 || dt_s <= 0.0) return 0.0;
    return 1.0 - std::pow(1.0 - p, dt_s / finestra_s);
}

bool notte_asw(double ora_locale) {
    const double h = std::fmod(std::fmod(ora_locale, 24.0) + 24.0, 24.0);
    return h < 6.0 || h >= 19.0;
}

// ------------------------------------------------------------------------------------------------- controllore

ControlloreAsw::ControlloreAsw(const DatiAsw& dati, const std::string& classe, int unita, const ConfigurazioneAsw& conf,
                               double t0)
    : dati_(&dati), mezzo_(dati.mezzo(classe)), conf_(conf), unita_(unita), t_stato_(t0), t_ultimo_(t0) {
    if (!mezzo_) throw std::runtime_error("asw_aereo: la classe '" + classe + "' non e' un mezzo ASW");
    sonar_ = mezzo_->sonar.empty() ? nullptr : dati.sonar(mezzo_->sonar);
    dip_ = CicloDip(sonar_);
    const auto d = dati.difficolta.find(conf.difficolta);
    diff_ = d != dati.difficolta.end() ? d->second : dati.difficolta.at(DifficoltaAsw::normale);
    livello_ = conf.equipaggio ? *conf.equipaggio : diff_.equipaggio ? *diff_.equipaggio : mezzo_->equipaggio;
    const auto e = dati.equipaggi.find(livello_);
    equip_ = e != dati.equipaggi.end() ? e->second : dati.equipaggi.at(LivelloEquipaggio::medio);
    const auto c = conf.carico.empty() ? mezzo_->carichi.end() : mezzo_->carichi.find(conf.carico);
    if (c != mezzo_->carichi.end()) {
        carico_boe_ = c->second.boe;
        carico_armi_ = c->second.armi;
        carico_marcatori_ = c->second.marcatori;
    } else {
        carico_boe_ = mezzo_->boe;
        carico_armi_ = mezzo_->armi;
        carico_marcatori_ = mezzo_->marcatori;
    }
    senza_direzionali_ = std::none_of(carico_boe_.begin(), carico_boe_.end(), [&](const auto& x) {
        const DatiBoa* b = dati.boa(x.first);
        return x.second > 0 && b && (b->funzione == FunzioneBoa::passiva_dir || b->funzione == FunzioneBoa::attiva_dir);
    });
    tos_h_ = tempo_in_zona_h(*mezzo_, distanza(conf.base, conf.zona));
    volo_.meta = conf.zona;
    volo_.quota_m = mezzo_->quota_transito_m;
    volo_.velocita_kn = mezzo_->velocita_transito_kn;
    storia_.push_back({t0, StatoAsw::transito, StatoAsw::transito, "decollo"});
}

bool ControlloreAsw::usa() const { return mezzo_->parte == ParteAsw::usa; }
const ProbabilitaAsw& ControlloreAsw::prob() const { return dati_->probabilita.at(mezzo_->parte); }
double ControlloreAsw::moltiplicatore_scoperta() const {
    return diff_.scoperta * (usa() ? 1.0 : diff_.scoperta_urss);
}

int ControlloreAsw::boe_rimaste(const std::string& tipo) const {
    if (tipo.empty()) {
        int n = 0;
        for (const auto& [k, v] : carico_boe_) n += v;
        return n;
    }
    const auto it = carico_boe_.find(tipo);
    return it == carico_boe_.end() ? 0 : it->second;
}

int ControlloreAsw::armi_rimaste() const {
    int n = 0;
    for (const auto& [k, v] : carico_armi_) n += v;
    return n;
}

std::vector<int> ControlloreAsw::boe_seguite(double t) const {
    std::vector<int> v;
    for (auto it = boe_.rbegin(); it != boe_.rend() && static_cast<int>(v.size()) < mezzo_->canali_boe; ++it) {
        const DatiBoa* d = dati_->boa(it->tipo);
        if (d && ascolta(d->funzione) && boa_in_ascolto(*it, t)) v.push_back(it->id);
    }
    return v;
}

// contatto scartato: si torna al datum o allo schema di prima, con le boe che restavano da posare
void ControlloreAsw::riprendi(double t, const std::string& motivo) {
    if (!ripresa_.valida) {
        traccia_ = TracciaAsw{};
        cambia(StatoAsw::ricerca, t, motivo);
        return;
    }
    ripresa_.valida = false;
    traccia_ = ripresa_.traccia;
    da_posare_ = ripresa_.da_posare;
    tipo_da_posare_ = ripresa_.tipo;
    quote_da_posare_ = ripresa_.quote;
    cambia(ripresa_.stato, t, motivo + ", si riprende");
    scelta_fatta_ = true;
}

void ControlloreAsw::cambia(StatoAsw s, double t, const std::string& motivo) {
    if (s == stato_) return;
    scelta_fatta_ = false;
    if (s == StatoAsw::localizzazione) ripresa_.valida = false;
    storia_.push_back({t, stato_, s, motivo});
    stato_ = s;
    t_stato_ = t;
}

bool ControlloreAsw::dip_possibile(const PercezioneAsw& p) const {
    if (!sonar_) return false;
    if (!sonar_->notte && notte_asw(p.ora_locale)) return false;   // Ka-25/27: di notte solo boe
    if (p.mare > sonar_->mare_max || p.vento_kn > sonar_->vento_max_kn || p.temporale_vicino) return false;
    if (sonar_->hs_max_m > 0.0 && p.mare >= sonar_->mare_max && p.hs_m > sonar_->hs_max_m) return false;   // AQS-13: onde oltre 10 ft
    if (mezzo_->id == "sh3h" && p.temperatura_c >= 40.0) return false;   // non tiene l'hover fuori effetto suolo
    return true;
}

std::string ControlloreAsw::boa_per(FunzioneBoa f) const {
    const auto cerca = [&](FunzioneBoa g) {
        std::string scelto;
        int piu = 0;
        for (const auto& [tipo, n] : carico_boe_) {
            const DatiBoa* d = dati_->boa(tipo);
            if (d && d->funzione == g && n > piu) { scelto = tipo; piu = n; }
        }
        return scelto;
    };
    std::string s = cerca(f);
    if (!s.empty()) return s;
    static const std::pair<FunzioneBoa, FunzioneBoa> RIPIEGHI[] = {
        {FunzioneBoa::passiva_dir, FunzioneBoa::passiva_omni}, {FunzioneBoa::passiva_omni, FunzioneBoa::passiva_dir},
        {FunzioneBoa::attiva_dir, FunzioneBoa::attiva_omni}, {FunzioneBoa::attiva_omni, FunzioneBoa::attiva_dir},
        {FunzioneBoa::attiva_dir, FunzioneBoa::attiva_distanza}, {FunzioneBoa::attiva_omni, FunzioneBoa::attiva_distanza}};
    for (const auto& [da, a] : RIPIEGHI)
        if (da == f && !(s = cerca(a)).empty()) return s;
    return {};
}

void ControlloreAsw::pianifica(SchemaBoe s, FunzioneBoa f, int n, const ParametriSchema& par, double t) {
    (void)t;
    if (s == SchemaBoe::passaggio_mad) {   // nessuna boa: il passaggio lo fa lo stato
        da_posare_.clear();
        return;
    }
    const std::string tipo = boa_per(f);
    if (tipo.empty()) return;
    ParametriSchema q = par;
    if (n > 0) q.boe = n;
    const DatiBoa* d = dati_->boa(tipo);
    if (d && d->portata_m > 0.0 && f != FunzioneBoa::esplosiva) q.portata_m = d->portata_m;
    if (mezzo_->id == "tu142" && s == SchemaBoe::campo) q.portata_m = std::max(q.portata_m, 8750.0);   // 15-20 km
    da_posare_ = posa_schema(s, q);
    // sotto il pack (I16 di trailer-1): ogni punto sul ghiaccio va nell'acqua libera piu' vicina entro la portata della
    // boa, se non ce n'e' gia' un'altra a meno di mezza portata; se tutto lo schema cade sul ghiaccio, niente boe
    if (acqua_libera_) {
        std::vector<Vec2> liberi;
        for (const Vec2 v : da_posare_) {
            const auto w = acqua_vicina(v, q.portata_m);
            if (!w) continue;
            const bool spostato = distanza(*w, v) > 1.0;
            if (spostato && std::any_of(liberi.begin(), liberi.end(), [&](Vec2 x) { return distanza(x, *w) < 0.5 * q.portata_m; })) continue;
            liberi.push_back(*w);
        }
        sul_pack_ = liberi.empty() && !da_posare_.empty();
        da_posare_ = std::move(liberi);
    }
    tipo_da_posare_ = tipo;
    quote_da_posare_.clear();
    for (std::size_t i = 0; i < da_posare_.size(); ++i)   // le DIFAR alternano sopra e sotto lo strato (§2.5)
        quote_da_posare_.push_back(d && d->funzione == FunzioneBoa::passiva_dir && d->quote_m.size() > 1
                                       ? d->quote_m[i % 2] : d ? d->quote_m.front() : 0.0);
}

bool ControlloreAsw::anello_chiude(const ParametriSchema& par) const {
    const auto parte = dati_->schemi.find(mezzo_->parte);
    if (parte == dati_->schemi.end()) return true;
    const auto sit = parte->second.find("datum");
    if (sit == parte->second.end()) return true;
    for (const auto& o : sit->second.scelte) {
        if (o.schema != SchemaBoe::barriera_circolare) continue;
        const DatiBoa* d = dati_->boa(boa_per(o.funzione));
        if (!d || d->portata_m <= 0.0 || o.boe <= 0) continue;
        const double r = std::max(3000.0, raggio_incertezza_m(par.raggio0_m, par.velocita_kn, par.ritardo_s));
        return 2.0 * PI * r <= o.boe * 2.0 * d->portata_m;
    }
    return true;
}

void ControlloreAsw::scegli_schema(const std::string& situazione, const ParametriSchema& par, double t, Casuale& caso) {
    const auto parte = dati_->schemi.find(mezzo_->parte);
    const double u = caso.uniforme();
    if (parte == dati_->schemi.end()) return;
    const auto sit = parte->second.find(situazione);
    if (sit == parte->second.end() || sit->second.scelte.empty()) return;
    double somma = 0.0;
    const OpzioneSchema* scelta = &sit->second.scelte.back();
    for (const auto& o : sit->second.scelte)
        if (u < (somma += o.p)) { scelta = &o; break; }
    // il box DICASS solo con DICASS a bordo (§7.3), altrimenti il passaggio MAD
    if (scelta->schema == SchemaBoe::box_dicass && boa_per(FunzioneBoa::attiva_dir).empty()) {
        da_posare_.clear();
        return;
    }
    pianifica(scelta->schema, scelta->funzione, scelta->boe, par, t);
}

ParametriSchema ControlloreAsw::parametri_traccia(double t, double portata_m) const {
    ParametriSchema p;
    p.datum = traccia_.pos;
    p.ritardo_s = std::max(0.0, t - traccia_.t);
    p.rotta_deg = traccia_.rotta_deg;
    p.velocita_kn = traccia_.velocita_kn.value_or(8.0);
    p.portata_m = portata_m;
    p.raggio0_m = std::clamp(traccia_.errore_m, 300.0, 20.0 * MIGLIO);
    return p;
}

Vec2 ControlloreAsw::errore_posa(Vec2 punto, Casuale& caso) const {
    // distribuzione di Rayleigh con media uguale all'errore dell'equipaggio (§7.4)
    const double u = caso.uniforme(), a = caso.uniforme();
    const double sigma = equip_.errore_posa_m / 1.2533;
    return polare(punto, 360.0 * a, sigma * std::sqrt(-2.0 * std::log(1.0 - u)));
}

std::optional<Vec2> ControlloreAsw::acqua_vicina(Vec2 p, double entro_m) const {
    if (!acqua_libera_ || acqua_libera_(p)) return p;
    // a cerchi di 250 m, un punto ogni 250 m d'arco (almeno 16): il primo libero, il piu' vicino
    for (double r = 250.0; r <= entro_m + 1e-9; r += 250.0) {
        const int n = std::max(16, static_cast<int>(std::ceil(2.0 * PI * r / 250.0)));
        for (int k = 0; k < n; ++k)
            if (const Vec2 q = polare(p, 360.0 * k / n, r); acqua_libera_(q)) return q;
    }
    return std::nullopt;
}

void ControlloreAsw::posa_in_volo(double t, const PercezioneAsw& p, Casuale& caso, std::vector<AzioneAsw>& out) {
    if (da_posare_.empty()) return;
    const double v = mezzo_->velocita_posa_kn * NODO;
    const double soglia = v * dati_->pensiero_s * 0.55 + 50.0;
    const auto in_testa = [&](std::size_t i) {
        std::swap(da_posare_[0], da_posare_[i]);
        if (quote_da_posare_.size() > i) std::swap(quote_da_posare_[0], quote_da_posare_[i]);
    };
    // l'ala fissa non si ferma sui punti: posa quello che le passa sotto e va verso il piu' raggiungibile dalla prua
    // (la strada piu' l'arco della virata, raggio v/omega a 3°/s: STIMA), non per forza il successivo dell'elenco
    if (!mezzo_->elicottero) {
        for (std::size_t i = 1; i < da_posare_.size(); ++i)
            if (distanza(p.pos, da_posare_[i]) <= soglia) { in_testa(i); break; }
        if (distanza(p.pos, da_posare_.front()) > soglia) {
            const double raggio = v / radianti(3.0);
            const auto costo = [&](Vec2 q) {
                return distanza(p.pos, q) + raggio * radianti(std::abs(normalizza_180(rilevamento(p.pos, q) - p.rotta)));
            };
            std::size_t migliore = 0;
            for (std::size_t i = 1; i < da_posare_.size(); ++i)
                if (costo(da_posare_[i]) < costo(da_posare_[migliore])) migliore = i;
            in_testa(migliore);
        }
    }
    // il ritmo di posa del mezzo vale per la ricerca d'area; le boe che seguono un contatto vanno subito
    const bool area = stato_ == StatoAsw::ricerca || stato_ == StatoAsw::transito;
    while (!da_posare_.empty() && distanza(p.pos, da_posare_.front()) <= soglia &&
           (!area || t - t_ultima_posa_ >= mezzo_->intervallo_posa_s)) {
        t_ultima_posa_ = t;
        auto& n = carico_boe_[tipo_da_posare_];
        const DatiBoa* d = dati_->boa(tipo_da_posare_);
        if (n <= 0 || !d) { da_posare_.clear(); break; }
        --n;
        AzioneAsw a;
        a.tipo = AzioneAsw::Tipo::posa_boa;
        a.oggetto = tipo_da_posare_;
        a.quota_m = quote_da_posare_.empty() ? d->quote_m.front() : quote_da_posare_.front();
        a.pos = errore_posa(p.pos, caso);
        if (acqua_libera_ && !acqua_libera_(a.pos)) {   // l'errore di posa l'ha messa sul ghiaccio: persa
            da_posare_.erase(da_posare_.begin());
            if (!quote_da_posare_.empty()) quote_da_posare_.erase(quote_da_posare_.begin());
            continue;
        }
        a.boa = crea_boa(*d, a.pos, p.quota_m, t, unita_ * 1000 + (++progressivo_boe_), unita_, diff_, caso, a.quota_m);
        boe_.push_back(*a.boa);
        out.push_back(std::move(a));
        da_posare_.erase(da_posare_.begin());
        if (!quote_da_posare_.empty()) quote_da_posare_.erase(quote_da_posare_.begin());
    }
}

void ControlloreAsw::prossimo_dip(Vec2 punto, double t, bool inseguimento) {
    (void)t;
    dip_da_fare_.insert(dip_da_fare_.begin(), punto);
    dip_inseguimento_ = inseguimento;
}

void ControlloreAsw::gestisci_dip(double t, const PercezioneAsw& p, Casuale& caso, std::vector<AzioneAsw>& out) {
    if (!sonar_) return;
    const FaseDip prima = fase_dip_prec_;   // anche i recuperi forzati fuori di qui (attacco, cambio)
    const int ping = dip_.avanza(t);
    for (int i = 0; i < ping; ++i) {
        AzioneAsw a;
        a.tipo = AzioneAsw::Tipo::ping;
        a.pos = p.pos;
        a.oggetto = sonar_->id;
        a.quota_m = dip_.quota_trasduttore_m();
        out.push_back(a);
    }
    if (prima != dip_.fase()) {
        if (prima == FaseDip::avvicinamento) {
            AzioneAsw a;
            a.tipo = AzioneAsw::Tipo::cala_sonar;
            a.pos = p.pos;
            a.oggetto = sonar_->id;
            a.quota_m = std::clamp(dip_.quota_trasduttore_m(), 0.0, sonar_->cavo_max_m);
            out.push_back(a);
        }
        if (prima > FaseDip::avvicinamento && prima < FaseDip::recupero && dip_.fase() >= FaseDip::recupero) {
            AzioneAsw a;
            a.tipo = AzioneAsw::Tipo::recupera_sonar;
            a.pos = p.pos;
            a.oggetto = sonar_->id;
            out.push_back(a);
        }
    }
    fase_dip_prec_ = dip_.fase();
    if (!dip_.finito()) return;
    // la stazione sul ghiaccio: il trasduttore va nell'acqua libera piu' vicina entro mezzo salto, se no la si salta
    while (!dip_da_fare_.empty()) {
        if (const auto w = acqua_vicina(dip_da_fare_.front(), 0.5 * sonar_->salto_inseguimento_m)) {
            dip_da_fare_.front() = *w;
            break;
        }
        dip_da_fare_.erase(dip_da_fare_.begin());
        sul_pack_ = dip_da_fare_.empty();   // tutte sul ghiaccio: si rinuncia al datum
    }
    if (!dip_possibile(p) || dip_da_fare_.empty()) { dip_in_volo_ = false; return; }
    // salto verso la prossima stazione; arrivati (entro 150 m) si comincia
    dip_in_volo_ = true;
    if (distanza(p.pos, dip_da_fare_.front()) <= std::max(150.0, sonar_->salto_velocita_kn * NODO * dati_->pensiero_s * 0.55)) {
        // il trasduttore sopra o sotto lo strato (§6.4 punto 3): l'operatore non sa a che quota sta il battello, meta' e
        // meta' (STIMA); senza strato a 60 m
        const double massima = sonar_->cavo_max_m - sonar_->hover_m;
        double quota = 60.0;
        if (p.strato_m > 0.0) quota = caso.uniforme() < 0.5 ? std::max(10.0, p.strato_m - 20.0) : p.strato_m + 30.0;
        dip_.inizia(t, std::min(quota, massima), dip_inseguimento_, caso);
        dip_da_fare_.erase(dip_da_fare_.begin());
        dip_in_volo_ = false;
        fase_dip_prec_ = dip_.fase();
    }
}

namespace {

constexpr double OMNI_INSIEME_S = 90.0;      // le omni che sentono lo stesso contatto entro questo tempo (STIMA)
constexpr double DISTANZE_INSIEME_S = 8.0;   // le distanze delle SSQ-47B dello stesso pensiero (ogni 5 s)

// La posizione dalle distanze di tre o piu' boe: i minimi quadrati sulle differenze dei cerchi (ognuno meno il primo),
// col residuo medio. Niente se le boe sono quasi allineate
std::optional<std::pair<Vec2, double>> trilatera(const std::vector<std::pair<Vec2, double>>& m) {
    if (m.size() < 3) return std::nullopt;
    const Vec2 o = m.front().first;
    const double r0 = m.front().second;
    double a11 = 0.0, a12 = 0.0, a22 = 0.0, b1 = 0.0, b2 = 0.0;
    for (std::size_t i = 1; i < m.size(); ++i) {
        const double x = m[i].first.x - o.x, y = m[i].first.y - o.y;   // dal primo
        const double ax = 2.0 * x, ay = 2.0 * y, bb = r0 * r0 - m[i].second * m[i].second + x * x + y * y;
        a11 += ax * ax;
        a12 += ax * ay;
        a22 += ay * ay;
        b1 += ax * bb;
        b2 += ay * bb;
    }
    const double det = a11 * a22 - a12 * a12;
    if (det <= 0.05 * a11 * a22) return std::nullopt;
    const Vec2 q{o.x + (a22 * b1 - a12 * b2) / det, o.y + (a11 * b2 - a12 * b1) / det};
    double s = 0.0;
    for (const auto& [da, d] : m) s += (distanza(q, da) - d) * (distanza(q, da) - d);
    return std::pair<Vec2, double>{q, std::sqrt(s / static_cast<double>(m.size()))};
}

}  // namespace

bool ControlloreAsw::aggiorna_traccia(const Rivelazione& r, const PercezioneAsw& p) {
    Vec2 da = p.pos;
    const Sonoboa* boa = nullptr;
    for (const auto& b : boe_)
        if (b.id == r.boa_o_aereo) { boa = &b; da = posizione_boa(b, r.t); break; }
    const DatiBoa* db = boa ? dati_->boa(boa->tipo) : nullptr;
    const double portata = db && db->portata_m > 0.0 ? db->portata_m : 3000.0;
    Vec2 stima = da;
    double errore = portata, lontano = 0.0;
    std::optional<double> solo;   // la stima da un rilevamento solo
    if (r.distanza_m && r.rilevamento) {   // eco: fix diretto
        stima = polare(da, *r.rilevamento, *r.distanza_m);
        errore = std::max(100.0, *r.distanza_m * radianti(db ? std::max(2.0, db->sigma_rilevamento_deg) : 3.0));
    } else if (r.rilevamento) {   // DIFAR: incrocio con un rilevamento recente di un'altra boa
        const double sigma = radianti(db && db->sigma_rilevamento_deg > 0.0 ? db->sigma_rilevamento_deg : 7.0);
        rilevamenti_.erase(std::remove_if(rilevamenti_.begin(), rilevamenti_.end(),
                                          [&](const Rilevamento& x) { return r.t - x.t > 180.0 || x.boa == r.boa_o_aereo; }),
                           rilevamenti_.end());
        bool incrocio = false;
        for (const auto& x : rilevamenti_) {
            const double angolo = std::abs(normalizza_180(*r.rilevamento - x.rilevamento));
            if (angolo < 30.0 || angolo > 150.0) continue;
            // l'incrocio dei due rilevamenti: davanti a tutte e due le boe, ed entro il doppio della portata
            const auto incrocia = [&](double r1, double r2) -> std::optional<Vec2> {
                const Vec2 a = verso(r1), b = verso(r2);
                const double det = a.x * (-b.y) - a.y * (-b.x);
                if (std::abs(det) < 1e-6) return std::nullopt;
                const double dx = x.da.x - da.x, dy = x.da.y - da.y;
                const double s = (dx * (-b.y) - dy * (-b.x)) / det, t_altra = (a.x * dy - a.y * dx) / det;
                if (s <= 0.0 || t_altra <= 0.0 || s > 2.0 * portata || t_altra > 2.0 * portata) return std::nullopt;
                return polare(da, r1, s);
            };
            const auto punto = incrocia(*r.rilevamento, x.rilevamento);
            if (!punto) continue;
            // l'errore dalla sensibilita': i rilevamenti spostati di un sigma; se un incrocio sparisce (rilevamenti
            // quasi paralleli: il bersaglio puo' essere lontano) il punto non vale
            const double sd = gradi(sigma);
            double peggiore = 0.0;
            bool stabile = true;
            for (const double d1 : {-sd, sd})
                for (const double d2 : {-sd, sd}) {
                    const auto q = incrocia(*r.rilevamento + d1, x.rilevamento + d2);
                    if (!q) stabile = false;
                    else peggiore = std::max(peggiore, distanza(*q, *punto));
                }
            if (!stabile) continue;
            stima = *punto;
            errore = std::max(150.0, peggiore);
            incrocio = true;
            break;
        }
        if (!incrocio) {   // un solo rilevamento: la distanza dall'eccesso di segnale (sferica, STIMA), molto incerta
            lontano = portata * std::clamp(std::pow(10.0, r.livello_db / 20.0), 0.5, 6.0);
            stima = polare(da, *r.rilevamento, 0.5 * lontano);
            errore = 0.7 * lontano;
            solo = r.rilevamento;
        }
        rilevamenti_.push_back({r.boa_o_aereo, da, *r.rilevamento, r.t});
    } else if (r.distanza_m) {   // attiva omni o SSQ-47B: solo distanza
        errore = std::max(150.0, *r.distanza_m);
        if (senza_direzionali_) {   // il LAMPS I: le distanze delle boe nello stesso pensiero, trilaterate (STIMA)
            distanze_.erase(std::remove_if(distanze_.begin(), distanze_.end(),
                                           [&](const Misura& x) { return r.t - x.t > DISTANZE_INSIEME_S || x.boa == r.boa_o_aereo; }),
                            distanze_.end());
            distanze_.push_back({r.boa_o_aereo, da, *r.distanza_m, r.t});
            std::vector<std::pair<Vec2, double>> m;
            for (const auto& x : distanze_) m.push_back({x.da, x.valore});
            if (const auto q = trilatera(m)) {
                stima = q->first;
                errore = std::max(150.0, 2.0 * q->second + 100.0);
            } else if (m.size() == 2) {   // due cerchi: l'incrocio piu' vicino alla traccia, ancora incerto
                const Vec2 a = m[0].first, b = m[1].first;
                const double dab = std::max(1.0, distanza(a, b)), l = (m[0].second * m[0].second - m[1].second * m[1].second + dab * dab) / (2.0 * dab);
                const double h2 = m[0].second * m[0].second - l * l;
                if (h2 >= 0.0) {
                    const Vec2 u{(b.x - a.x) / dab, (b.y - a.y) / dab}, piede{a.x + l * u.x, a.y + l * u.y};
                    const double hh = std::sqrt(h2);
                    const Vec2 p1{piede.x - hh * u.y, piede.y + hh * u.x}, p2{piede.x + hh * u.y, piede.y - hh * u.x};
                    stima = !traccia_.valida || distanza(p1, traccia_.pos) <= distanza(p2, traccia_.pos) ? p1 : p2;
                    errore = std::max(1200.0, std::min(errore, 2.0 * hh));
                }
            }
        }
    } else if (senza_direzionali_) {   // il LAMPS I: le omni che sentono insieme, pesate col segnale (STIMA)
        omni_.erase(std::remove_if(omni_.begin(), omni_.end(),
                                   [&](const Misura& x) { return r.t - x.t > OMNI_INSIEME_S || x.boa == r.boa_o_aereo; }),
                    omni_.end());
        omni_.push_back({r.boa_o_aereo, da, r.livello_db, r.t});
        if (omni_.size() >= 2) {
            double sx = 0.0, sy = 0.0, sw = 0.0;
            for (const auto& x : omni_) {
                const double w = std::pow(10.0, std::clamp(x.valore, -20.0, 40.0) / 20.0);
                sx += w * x.da.x;
                sy += w * x.da.y;
                sw += w;
            }
            stima = {sx / sw, sy / sw};
            errore = std::max(1200.0, portata / static_cast<double>(omni_.size()));
        }
    }
    // associazione: con la traccia confermata, una rivelazione che non torna col punto previsto e' un altro contatto.
    // Rilevamento: lo scarto angolare entro 3 sigma piu' l'incertezza vista dal ricevitore; distanza: il punto; omni:
    // la portata della boa
    if (traccia_.valida && traccia_.confermata) {
        const double deriva_m = traccia_.velocita_kn.value_or(8.0) * NODO * std::max(0.0, r.t - traccia_.t);
        const double incerto = traccia_.errore_m + deriva_m, d = distanza(da, traccia_.pos);
        bool torna;
        if (r.distanza_m) torna = distanza(stima, traccia_.pos) <= 3.0 * incerto + 500.0;
        else if (r.rilevamento) {
            const double sigma = db && db->sigma_rilevamento_deg > 0.0 ? db->sigma_rilevamento_deg : 7.0;
            const double scarto = std::abs(normalizza_180(*r.rilevamento - rilevamento(da, traccia_.pos)));
            torna = d <= incerto || scarto <= 3.0 * sigma + gradi(std::atan2(incerto, std::max(1.0, d)));
        } else
            torna = d <= portata + incerto;
        if (!torna) {
            if (r.rilevamento && !r.distanza_m) rilevamenti_.pop_back();   // niente incroci con un altro contatto
            return false;
        }
    }
    // rotta e velocita' dallo spostamento fra due stime buone a distanza di tempo
    if (traccia_.valida && traccia_.errore_m < 1000.0 && errore < 1000.0 && r.t - traccia_.t >= 120.0) {
        const double d = distanza(traccia_.pos, stima), dt = r.t - traccia_.t;
        if (d > errore + traccia_.errore_m) {
            traccia_.rotta_deg = rilevamento(traccia_.pos, stima);
            traccia_.velocita_kn = std::min(40.0, d / dt / NODO);
        }
    }
    // la nuova stima vale se migliore o se la vecchia e' invecchiata
    const double eta = traccia_.velocita_kn.value_or(8.0) * NODO * std::max(0.0, r.t - traccia_.t);
    if (!traccia_.valida || errore <= traccia_.errore_m + eta) {
        traccia_.pos = stima;
        traccia_.errore_m = errore;
        traccia_.t = r.t;
        traccia_.rilevamento_solo = solo;
        traccia_.da_rilevamento = da;
        traccia_.lontano_m = lontano;
    }
    traccia_.valida = true;
    ++traccia_.rivelazioni;
    const int voti = ++traccia_.voti[r.sorgente];
    if (traccia_.sorgente < 0 || voti > traccia_.voti[traccia_.sorgente]) traccia_.sorgente = r.sorgente;
    ultima_vera_ = r.sottomarino;
    t_ultima_rivelazione_ = std::max(t_ultima_rivelazione_, r.t);
    return true;
}

void ControlloreAsw::falsi_allarmi(double t, double dt, Casuale& caso, std::vector<Rivelazione>& out) {
    for (const int id : boe_seguite(t)) {
        const auto it = std::find_if(boe_.begin(), boe_.end(), [&](const Sonoboa& b) { return b.id == id; });
        const DatiBoa* d = dati_->boa(it->tipo);
        const double p = d->falsi_allarmi_h * equip_.falsi_allarmi * mezzo_->falsi_allarmi * dt / 3600.0;
        if (caso.uniforme() >= p) continue;
        Rivelazione r;
        r.boa_o_aereo = id;
        r.t = t;
        r.livello_db = 0.5;
        if (d->funzione == FunzioneBoa::passiva_dir) r.rilevamento = 360.0 * caso.uniforme();
        out.push_back(r);
    }
}

void ControlloreAsw::ricevi_cue(const Cue& c, double t, Casuale& caso) {
    // il MATCH (docs/ricerca/38): un mezzo col solo MAD, gia' sul contatto, prende dalla nave il punto aggiornato senza
    // ricominciare dal datum, e il contatto della nave tiene viva la traccia; il fix resta al MAD (il runtime, 31/10/2026:
    // dopo il primo MAD il Lynx restava sul punto vecchio, il Victor a 4 kn ne usciva e in 3 minuti il contatto era perso)
    const bool solo_mad = mezzo_->mad && !sonar_ &&
                          std::none_of(carico_boe_.begin(), carico_boe_.end(), [](const auto& x) { return x.second > 0; });
    if (solo_mad && c.tipo == Cue::Tipo::contatto_nave && traccia_.valida &&
        (stato_ == StatoAsw::contatto || stato_ == StatoAsw::localizzazione || stato_ == StatoAsw::inseguimento)) {
        if (c.t >= traccia_.t) {
            traccia_.pos = c.pos;
            traccia_.t = c.t;
            traccia_.errore_m = std::max(traccia_.errore_m, c.raggio_m);
            if (c.rotta_deg) traccia_.rotta_deg = c.rotta_deg;
            if (c.velocita_kn) traccia_.velocita_kn = c.velocita_kn;
            t_ultima_rivelazione_ = std::max(t_ultima_rivelazione_, t);
        }
        return;
    }
    // un cue piu' vecchio o meno preciso della traccia in corso non serve
    if (traccia_.valida && stato_ != StatoAsw::ricerca && stato_ != StatoAsw::transito &&
        stato_ != StatoAsw::perso && stato_ != StatoAsw::datum)
        return;
    traccia_ = TracciaAsw{};
    traccia_.valida = true;
    traccia_.pos = c.pos;
    traccia_.t = c.t;
    traccia_.errore_m = c.raggio_m;
    traccia_.rotta_deg = c.rotta_deg;
    traccia_.velocita_kn = c.velocita_kn;
    traccia_.sorgente = c.unita;
    da_posare_.clear();
    dip_da_fare_.clear();
    simulato_ = passato_ = marcato_ = false;
    scelta_fatta_ = false;
    (void)caso;
    if (stato_ == StatoAsw::transito && t_arrivo_ < 0.0) {
        // si va sul datum: il TOS si conta dalla distanza dalla base al datum
        tos_h_ = tempo_in_zona_h(*mezzo_, distanza(conf_.base, c.pos));
    }
    cambia(StatoAsw::datum, t, "cue");
}

void ControlloreAsw::marcatore(Vec2 pos, std::vector<AzioneAsw>& out) {
    for (const char* m : {"mk58", "mk25", "omab"}) {
        auto it = carico_marcatori_.find(m);
        if (it == carico_marcatori_.end() || it->second <= 0) continue;
        --it->second;
        AzioneAsw a;
        a.tipo = AzioneAsw::Tipo::marcatore;
        a.pos = pos;
        a.oggetto = m;
        out.push_back(a);
        return;
    }
}

Vec2 ControlloreAsw::punto_previsto(double t) const {
    if (!traccia_.rotta_deg || !traccia_.velocita_kn) return traccia_.pos;
    return polare(traccia_.pos, *traccia_.rotta_deg, *traccia_.velocita_kn * NODO * std::max(0.0, t - traccia_.t));
}

double ControlloreAsw::incertezza_prevista(double t) const {
    // con rotta e velocita' stimate cresce col loro errore (STIMA: un terzo della strada), senza con la strada intera
    const double dt = std::max(0.0, t - traccia_.t);
    const double v = traccia_.velocita_kn.value_or(8.0) * NODO;
    return traccia_.errore_m + (traccia_.rotta_deg && traccia_.velocita_kn ? 0.33 : 1.0) * v * dt;
}

std::string ControlloreAsw::arma_adatta(const PercezioneAsw& p) const {
    // prima i siluri e i razzi-siluro, poi le bombe; non quelle che il fondale non consente (l'AT-1 sotto 60 m)
    static const char* PREFERITE[] = {"stingray", "mk46", "tp42", "apr2", "umgt1", "at2", "at1m", "plab250", "plab50",   // stingray: il Lynx
                                      "sjunkbomb51"};   // tp42, sjunkbomb51: l'Hkp 4
    const auto consentita = [&](const std::string& a) {
        const auto f = p.fondale_minimo.find(a);
        return f == p.fondale_minimo.end() || p.fondale_m >= f->second;
    };
    for (const char* a : PREFERITE) {
        const auto it = carico_armi_.find(a);
        if (it != carico_armi_.end() && it->second > 0 && consentita(a)) return a;
    }
    for (const auto& [k, n] : carico_armi_)
        if (n > 0 && consentita(k)) return k;
    return "";
}

void ControlloreAsw::lancia(double t, const PercezioneAsw& p, Casuale& caso, bool da_hover, std::vector<AzioneAsw>& out) {
    const std::string arma = arma_adatta(p);
    if (arma.empty()) return;
    // 300-500 yd davanti al sottomarino sulla sua rotta stimata (§6.2), dal punto previsto ora; sul pack nell'acqua
    // libera entro 1 km, se no niente lancio (l'arma non passa il ghiaccio)
    const Vec2 qui = punto_previsto(t);
    const auto punto = acqua_vicina(traccia_.rotta_deg ? polare(qui, *traccia_.rotta_deg, 366.0) : qui, 1000.0);
    if (!punto) return;
    --carico_armi_[arma];
    AzioneAsw a;
    a.tipo = AzioneAsw::Tipo::lancia_arma;
    a.oggetto = arma;
    a.pos = *punto;
    // errore di sgancio (STIMA, dossier 17 §6.7 e §7.4): quello di posa dell'equipaggio (100-200-300/500 yd, Rayleigh),
    // la meta' dall'hover a 40 ft sul proprio contatto
    const Vec2 sbagliato = errore_posa(a.pos, caso);
    a.pos = da_hover ? Vec2{0.5 * (a.pos.x + sbagliato.x), 0.5 * (a.pos.y + sbagliato.y)} : sbagliato;
    a.quota_m = p.quota_m;
    a.rotta = p.rotta;
    a.bersaglio = traccia_.sorgente;
    out.push_back(a);
    marcatore(a.pos, out);
    t_lancio_ = t;
}

void ControlloreAsw::imposta_volo(const PercezioneAsw& p) {
    volo_.hover = dip_.in_hover() && dip_.fase() != FaseDip::avvicinamento;
    volo_.cavo_m = dip_.cavo_m();
    const bool fuori = stato_ == StatoAsw::transito || stato_ == StatoAsw::rientro || stato_ == StatoAsw::cambio;
    volo_.luci = fuori || conf_.situazione != SituazioneAsw::guerra;   // oscurati in caccia in guerra (§8.1)
    volo_.radar = !dip_.in_hover() && (stato_ == StatoAsw::transito || stato_ == StatoAsw::ricerca ||
                                       stato_ == StatoAsw::datum || stato_ == StatoAsw::perso);
    volo_.libero = (stato_ == StatoAsw::ricerca || (stato_ == StatoAsw::transito && !traccia_.valida)) &&
                   da_posare_.empty() && !dip_.in_hover() && dip_da_fare_.empty();
    if (dip_.in_hover()) {   // fermo sul punto della stazione
        volo_.meta = p.pos;
        volo_.quota_m = sonar_->hover_m;
        volo_.velocita_kn = 0.0;
        return;
    }
    switch (stato_) {
        case StatoAsw::transito:
            volo_.meta = traccia_.valida ? traccia_.pos : conf_.zona;
            volo_.quota_m = mezzo_->quota_transito_m;
            volo_.velocita_kn = mezzo_->velocita_transito_kn;
            break;
        case StatoAsw::rientro:
        case StatoAsw::cambio:
            volo_.meta = conf_.base;
            volo_.quota_m = mezzo_->quota_transito_m;
            volo_.velocita_kn = mezzo_->velocita_transito_kn;
            break;
        case StatoAsw::attacco:
            volo_.meta = punto_previsto(t_ultimo_);
            volo_.quota_m = mezzo_->quota_attacco_m;
            volo_.velocita_kn = mezzo_->velocita_ricerca_kn;
            break;
        default: {
            const Vec2 centro = traccia_.valida ? traccia_.pos : conf_.zona;
            // un mezzo col solo MAD (ne' boe ne' sonar: il Lynx del MATCH, docs/ricerca/38) cerca col MAD gia' sul datum e
            // sul contatto, attorno al punto che la nave gli passa; anche se e' un elicottero
            const bool solo_mad = mezzo_->mad && !sonar_ &&
                                  std::none_of(carico_boe_.begin(), carico_boe_.end(), [](const auto& x) { return x.second > 0; });
            const bool cerca_mad = stato_ == StatoAsw::localizzazione || stato_ == StatoAsw::inseguimento || stato_ == StatoAsw::verifica ||
                                   (solo_mad && (stato_ == StatoAsw::datum || stato_ == StatoAsw::contatto));
            if (!da_posare_.empty()) {
                volo_.meta = da_posare_.front();
                volo_.quota_m = mezzo_->quota_posa_m;
                volo_.velocita_kn = mezzo_->velocita_posa_kn;
            } else if (sonar_ && !dip_da_fare_.empty() && dip_in_volo_) {
                volo_.meta = dip_da_fare_.front();
                volo_.quota_m = mezzo_->quota_ricerca_m;
                volo_.velocita_kn = sonar_->salto_velocita_kn;
            } else if (mezzo_->mad && (!mezzo_->elicottero || solo_mad) && cerca_mad && traccia_.errore_m >= 2000.0 &&
                       (traccia_.errore_m <= 6000.0 || solo_mad)) {
                // ricerca MAD sull'area d'incertezza: passaggi su punti a girasole (passo aureo), il successivo quando
                // si e' sul punto. Senza boe direzionali e' l'unica via al fix (Be-12, §6.6; STIMA sulla geometria)
                const double raggio = std::min(traccia_.errore_m, 6000.0);
                const auto punto = [&](int k) {
                    return polare(traccia_.pos, 137.5 * k, raggio * std::sqrt(((k % 12) + 0.5) / 12.0));
                };
                if (distanza(p.pos, punto(passo_mad_)) < 400.0) ++passo_mad_;
                volo_.meta = punto(passo_mad_);
                volo_.quota_m = notte_asw(p.ora_locale) ? mezzo_->mad->quota_notte_m : mezzo_->mad->quota_giorno_m;
                volo_.velocita_kn = std::min(mezzo_->velocita_ricerca_kn, mezzo_->mad->velocita_kn);
            } else if (mezzo_->mad && cerca_mad && traccia_.errore_m < 2000.0) {
                // passaggi MAD sul fix. L'elicottero li fa a quadrifoglio: punta il punto e lo attraversa (passo pari), poi
                // tira dritto finche' ne e' a PASSAGGIO_MAD_M (dispari) e torna a puntarlo; la virata fa girare l'asse. Cosi'
                // il MAD chiude il passaggio e ne apre un altro (il runtime, 31/10/2026: il Lynx girava a 30-550 m dal
                // Victor e restava nello stesso passaggio, senza fix). Non coi capi fissi a 45 gradi: a 90 nodi il raggio
                // di virata (~900 m) lo faceva passare a 700-800 m dal punto, e un tratto su due era una corda (la Type 22
                // del runtime, senza lanci in 30 minuti). L'ala fissa ci passa sopra ed esce larga da se'
                volo_.meta = traccia_.pos;
                if (mezzo_->elicottero) {
                    const double d = distanza(p.pos, traccia_.pos);
                    if (passo_mad_ % 2 == 0 && d < PASSAGGIO_MAD_SOPRA_M) ++passo_mad_;   // sopra il punto: si esce
                    else if (passo_mad_ % 2 == 1 && d >= PASSAGGIO_MAD_M) ++passo_mad_;   // fuori: si torna
                    if (passo_mad_ % 2 == 1)   // in uscita, dritto lungo la direzione dal punto
                        volo_.meta = polare(traccia_.pos, rilevamento(traccia_.pos, p.pos), PASSAGGIO_MAD_M + 1000.0);
                }
                volo_.quota_m = notte_asw(p.ora_locale) ? mezzo_->mad->quota_notte_m : mezzo_->mad->quota_giorno_m;
                volo_.velocita_kn = std::min(mezzo_->velocita_ricerca_kn, mezzo_->mad->velocita_kn);
            } else {
                volo_.meta = centro;
                volo_.quota_m = mezzo_->quota_ricerca_m;
                volo_.velocita_kn = mezzo_->velocita_ricerca_kn;
            }
            break;
        }
    }
}

void ControlloreAsw::esito_arma(double t, bool colpito) {
    (void)t;
    if (colpito) colpito_ = true;
}

void ControlloreAsw::richiama(double t) {
    dip_.recupera(t);
    da_posare_.clear();
    dip_da_fare_.clear();
    cambia(StatoAsw::rientro, t, "richiamato");
}

std::vector<AzioneAsw> ControlloreAsw::passo(double t, const PercezioneAsw& p, Casuale& caso) {
    std::vector<AzioneAsw> out;
    const double dt = std::max(0.0, t - t_ultimo_);
    t_ultimo_ = t;
    acqua_libera_ = p.acqua_libera;
    const ProbabilitaAsw& P = prob();
    const double perdita_s = P.perdita_s / equip_.perdita_contatto;
    if (p.situazione) conf_.situazione = *p.situazione;

    if (stato_ == StatoAsw::rientro) {
        dip_.avanza(t);
        imposta_volo(p);
        return out;
    }

    // carburante, boe e armi (§6.8)
    if (t_arrivo_ >= 0.0 && stato_ != StatoAsw::cambio && t >= fine_zona_t_ - mezzo_->riserva_h * 3600.0) {
        dip_.recupera(t);
        cambia(StatoAsw::cambio, t, "carburante");
    }
    if (stato_ != StatoAsw::cambio && t_arrivo_ >= 0.0 && boe_rimaste("") == 0 && !sonar_ && armi_rimaste() == 0 &&
        boe_seguite(t).empty()) {
        cambia(StatoAsw::cambio, t, "carico finito");
    }
    if (stato_ == StatoAsw::cambio) {
        if (!rapporto_cambio_) {
            rapporto_cambio_ = true;
            AzioneAsw a;
            a.tipo = traccia_.valida && traccia_.confermata ? AzioneAsw::Tipo::passa_contatto : AzioneAsw::Tipo::rapporto;
            a.pos = traccia_.pos;
            a.bersaglio = traccia_.sorgente;
            a.codice = "cambio";
            out.push_back(a);
        }
        gestisci_dip(t, p, caso, out);
        if (!dip_.in_hover()) cambia(StatoAsw::rientro, t, "cambio fatto");
        imposta_volo(p);
        return out;
    }

    for (const auto& c : p.cue) ricevi_cue(c, t, caso);
    for (const auto& r : p.radar) {   // albero o battello in superficie: datum fresco e preciso
        if (stato_ == StatoAsw::transito || stato_ == StatoAsw::ricerca || stato_ == StatoAsw::perso) {
            Cue c;
            c.tipo = Cue::Tipo::radar_esm;
            c.pos = r;
            c.t = t;
            c.raggio_m = 500.0;
            ricevi_cue(c, t, caso);
        }
    }

    // rivelazioni: solo dalle boe seguite e dal proprio sonar
    const std::vector<int> seguite = boe_seguite(t);
    std::vector<Rivelazione> riv;
    for (const auto& r : p.acustiche) {
        if (r.boa_o_aereo != unita_ && std::find(seguite.begin(), seguite.end(), r.boa_o_aereo) == seguite.end()) continue;
        const auto sc = r.sorgente >= 0 ? scartati_.find(r.sorgente) : scartati_.end();
        if (sc != scartati_.end() && t < sc->second) continue;   // gia' classificato come non sottomarino
        riv.push_back(r);
    }
    if (stato_ != StatoAsw::transito) falsi_allarmi(t, dt, caso, riv);
    const TracciaAsw traccia_prima = traccia_;
    const auto sospendi = [&] { ripresa_ = {true, stato_, traccia_prima, da_posare_, tipo_da_posare_, quote_da_posare_}; };
    bool sentito = false, da_sottomarino = false;
    for (const auto& r : riv) {
        // con la traccia confermata, l'operatore riconosce dalle righe una nave o un biologico diverso da lei
        if (traccia_.confermata && !r.sottomarino && r.sorgente != traccia_.sorgente) continue;
        if (aggiorna_traccia(r, p)) {
            sentito = true;
            da_sottomarino = da_sottomarino || r.sottomarino;
        }
    }
    if (sentito) ultima_vera_ = da_sottomarino;
    if (!p.mad.empty()) {   // «MAD! MAD! MAD!»: fix sulla verticale e fumogeno
        traccia_.valida = true;
        traccia_.pos = p.pos;
        traccia_.errore_m = 250.0;
        traccia_.t = t;
        traccia_.sorgente = p.mad.front();
        ++traccia_.voti[p.mad.front()];
        t_mad_ = t;
        t_ultima_rivelazione_ = t;
        ultima_vera_ = true;
        marcatore(p.pos, out);
    }
    const bool mad = t - t_mad_ < 1e-9 && !p.mad.empty();
    const bool recente = t - t_ultima_rivelazione_ <= perdita_s;

    switch (stato_) {
        case StatoAsw::transito: {
            const Vec2 meta = traccia_.valida ? traccia_.pos : conf_.zona;
            if (distanza(p.pos, meta) <= std::max(2000.0, 0.5 * conf_.raggio_zona_m)) {
                t_arrivo_ = t;
                fine_zona_t_ = t + tos_h_ * 3600.0;
                cambia(StatoAsw::ricerca, t, "arrivo in zona");
                if (sonar_ && dip_possibile(p)) {   // dip di ricerca in linea, col posto nel gruppo
                    const Vec2 c = polare(conf_.zona, conf_.rotta_presunta + 90.0,
                                          (conf_.indice_gruppo - (conf_.numero_gruppo - 1) / 2.0) * conf_.distanza_gruppo_m);
                    for (int i = 0; i < 8; ++i)
                        dip_da_fare_.push_back(polare(c, conf_.rotta_presunta, i * sonar_->salto_ricerca_m));
                    dip_inseguimento_ = false;
                } else {
                    ParametriSchema par;
                    par.datum = conf_.zona;
                    par.rotta_deg = conf_.rotta_presunta;
                    const std::string sit = usa() ? "ricerca" : mezzo_->id == "tu142" ? "arrivo_campo" : "arrivo";
                    scegli_schema(sit, par, t, caso);
                }
            }
            break;
        }
        case StatoAsw::ricerca:
            if (sentito || mad) { sospendi(); cambia(StatoAsw::contatto, t, mad ? "MAD" : "rivelazione"); break; }
            // niente da posare e nessuna boa in ascolto: un altro schema di ricerca nella zona, ogni 10 min al piu'
            if (t_arrivo_ >= 0.0 && t < fine_zona_t_ && da_posare_.empty() && (!sonar_ || !dip_possibile(p)) &&
                t - t_stato_ > 300.0 && t - t_ripiano_ > 600.0 && boe_seguite(t).empty()) {
                t_ripiano_ = t;
                ParametriSchema par;
                par.datum = conf_.zona;
                par.rotta_deg = conf_.rotta_presunta;
                scegli_schema(usa() ? "ricerca" : mezzo_->id == "tu142" ? "arrivo_campo" : "arrivo", par, t, caso);
            }
            break;
        case StatoAsw::datum: {
            if (t_arrivo_ < 0.0 && distanza(p.pos, traccia_.pos) <= 2.0 * traccia_.errore_m + 3000.0) {
                t_arrivo_ = t;
                fine_zona_t_ = t + tos_h_ * 3600.0;
            }
            if (sentito || mad) { sospendi(); cambia(StatoAsw::contatto, t, mad ? "MAD" : "rivelazione"); break; }
            if (t_arrivo_ < 0.0) break;
            if (!scelta_fatta_) {   // prima scelta sul datum (§7.3)
                scelta_fatta_ = true;
                sul_pack_ = false;
                const double ritardo = t - traccia_.t;
                if (sonar_ && dip_possibile(p)) {   // spirale di dip disgiunti (NPS)
                    ParametriSchema par = parametri_traccia(t, sonar_->salto_inseguimento_m);
                    par.raggio0_m = 0.5 * sonar_->salto_inseguimento_m;
                    dip_da_fare_.push_back(traccia_.pos);
                    for (const Vec2 v : posa_schema(SchemaBoe::spirale, par)) dip_da_fare_.push_back(v);
                    dip_inseguimento_ = true;
                } else {
                    std::string sit;
                    if (usa())
                        sit = ritardo < 900.0 ? (traccia_.rotta_deg ? "datum_fresco_rotta_nota" : "datum_fresco_rotta_ignota")
                              : ritardo < 3600.0 ? "datum_medio" : "datum_vecchio";
                    else
                        sit = ritardo > 3600.0 && mezzo_->id == "tu142" ? "arrivo_campo" : "datum";
                    const ParametriSchema par = parametri_traccia(t, 7408.0);
                    // l'anello si chiude solo con le boe a meno di 2 R50: sul cerchio di un cue d'area (SOSUS) non
                    // ci sta, e l'equipaggio stende un campo sul datum (STIMA, taratura A6.2 del 25/10/2026)
                    if (sit == "datum" && !anello_chiude(par)) sit = "arrivo_campo";
                    scegli_schema(sit, par, t, caso);
                }
            }
            if (sul_pack_ && da_posare_.empty() && dip_da_fare_.empty() && !dip_.in_hover()) {
                sul_pack_ = false;
                cambia(StatoAsw::perso, t, "datum sul pack");   // niente acqua libera dove buttare le boe
                break;
            }
            if (da_posare_.empty() && dip_da_fare_.empty() && !dip_.in_hover() && t - t_stato_ > 2.0 * perdita_s)
                cambia(StatoAsw::perso, t, "datum senza contatto");
            break;
        }
        case StatoAsw::contatto: {
            const double m = moltiplicatore_scoperta();
            if (dip_.fase() == FaseDip::ascolto) dip_.forza_attivo(t);
            if (!scelta_fatta_) {   // boe per confermare (§7.3)
                scelta_fatta_ = true;
                t_decisione_ = -1.0;
                if (!sonar_ || !dip_possibile(p)) {
                    if (usa() && traccia_.rilevamento_solo && traccia_.errore_m > 2.0 * 7408.0) {   // lontano
                        ParametriSchema par = parametri_traccia(t, 7408.0);
                        par.datum = traccia_.da_rilevamento;
                        par.rotta_deg = traccia_.rilevamento_solo;
                        par.raggio0_m = 1.2 * traccia_.lontano_m;
                        scegli_schema("difar_lontano", par, t, caso);
                    } else
                        scegli_schema(usa() ? "difar_singolo" : "boa_reagisce", parametri_traccia(t, 7408.0), t, caso);
                }
            }
            if (!ultima_vera_) {   // falso contatto: si decide una volta, col ritardo dell'equipaggio
                if (t_decisione_ < 0.0) {
                    decisione_crede_ = caso.uniforme() < std::min(1.0, P.crede_falso * equip_.falsi_allarmi);
                    t_decisione_ = t + (60.0 + 540.0 * caso.uniforme()) * equip_.ritardo;
                }
                if (t >= t_decisione_) {
                    if (decisione_crede_) cambia(StatoAsw::localizzazione, t, "falso contatto creduto");
                    else {
                        if (traccia_.sorgente >= 0) scartati_[traccia_.sorgente] = t + MEMORIA_SCARTATI_S;
                        riprendi(t, "falso allarme");
                    }
                }
                break;
            }
            t_decisione_ = -1.0;
            if (mad || (recente && caso.uniforme() < probabilita_passo(P.conferma * mezzo_->conferma * m, P.finestra_conferma_s, dt))) {
                traccia_.confermata = true;
                cambia(StatoAsw::localizzazione, t, mad ? "conferma MAD" : "conferma");
            } else if (caso.uniforme() < probabilita_passo(P.scarta_vero, P.finestra_conferma_s, dt)) {
                if (traccia_.sorgente >= 0) scartati_[traccia_.sorgente] = t + MEMORIA_SCARTATI_S / 3.0;
                riprendi(t, "contatto vero scartato");
            } else if (!recente) {
                cambia(StatoAsw::perso, t, "contatto perso");
            }
            break;
        }
        case StatoAsw::localizzazione: {
            const double m = moltiplicatore_scoperta();
            if (!recente) { cambia(StatoAsw::perso, t, "contatto perso"); break; }
            if (!scelta_fatta_ && traccia_.errore_m <= 2.0 * MIGLIO && (!sonar_ || !dip_possibile(p))) {
                scelta_fatta_ = true;
                ParametriSchema par = parametri_traccia(t, 1800.0);
                if (!mezzo_->elicottero) {   // l'ala fissa non rallenta: il box va dove sara' il battello a meta' posa
                    const double volo = distanza(p.pos, punto_previsto(t)) / std::max(1.0, mezzo_->velocita_posa_kn * NODO);
                    par.datum = punto_previsto(t + volo + MEZZA_POSA_BOX_S);
                    par.ritardo_s = 0.0;
                }
                scegli_schema(usa() ? "fix" : "boa_reagisce", par, t, caso);
            }
            if (sonar_ && dip_possibile(p) && dip_.finito()) {   // prossimo dip sul fix
                // la spirale di ricerca rimasta in coda dal datum non serve piu': si cala sul punto della traccia
                if (!dip_da_fare_.empty() && distanza(dip_da_fare_.front(), traccia_.pos) > traccia_.errore_m + 1000.0) dip_da_fare_.clear();
                if (dip_da_fare_.empty()) prossimo_dip(traccia_.pos, t, true);
            }
            // DICASS a comando, una boa alla volta, ogni 30 s, finche' ha energia (i 100 ping-secondi: con l'impulso di
            // acustica.toml il conto dei ping; finita, si passa alla boa dopo)
            if (t - t_dicass_ >= 30.0)
                for (auto& b : boe_)
                    if (b.attiva_sonar && boa_in_ascolto(b, t) && distanza(posizione_boa(b, t), traccia_.pos) < 4000.0) {
                        const DatiBoa* d = dati_->boa(b.tipo);
                        if (d && d->periodo_ping_s > 0.0) continue;   // SSQ-47B: pinga da sola
                        if (d && !ping_disponibile(b, *d)) continue;
                        AzioneAsw a;
                        a.tipo = AzioneAsw::Tipo::comanda_boa;
                        a.bersaglio = b.id;
                        a.pos = posizione_boa(b, t);
                        a.oggetto = b.tipo;
                        out.push_back(a);
                        if (d) b.ping_secondi_usati += d->impulso_s;
                        t_dicass_ = t;
                        break;
                    }
            const bool fix = traccia_.errore_m <= 914.0 &&
                             (mad || (sentito && caso.uniforme() < probabilita_passo(P.fix * m, P.finestra_fix_s, dt)));
            if (!fix) break;
            traccia_.confermata = true;
            if (conf_.situazione == SituazioneAsw::guerra && conf_.armi && armi_rimaste() > 0 && !arma_adatta(p).empty())
                cambia(StatoAsw::attacco, t, mad ? "fix e MAD" : "fix");
            else
                cambia(StatoAsw::inseguimento, t, "fix");
            break;
        }
        case StatoAsw::inseguimento: {
            if (!recente) { cambia(StatoAsw::perso, t, "contatto perso"); break; }
            if (!passato_) {   // passa il contatto alle navi (§7.2)
                passato_ = true;
                if (caso.uniforme() < P.passaggio_contatto && !conf_.ricognizione) {
                    AzioneAsw a;
                    a.tipo = AzioneAsw::Tipo::passa_contatto;
                    a.pos = traccia_.pos;
                    a.bersaglio = traccia_.sorgente;
                    a.codice = "contatto";
                    out.push_back(a);
                }
            }
            if (conf_.ricognizione && !marcato_ && da_posare_.empty()) {   // ricognizione: una boa attiva sul fix
                marcato_ = true;
                pianifica(SchemaBoe::attiva, FunzioneBoa::attiva_dir, 1, parametri_traccia(t, 1800.0), t);
            }
            if (conf_.ricognizione && t - t_dicass_ >= 60.0)   // che pulsa: la sente anche il giocatore
                for (auto it = boe_.rbegin(); it != boe_.rend(); ++it)
                    if (it->attiva_sonar && boa_in_ascolto(*it, t) && distanza(posizione_boa(*it, t), traccia_.pos) < 4000.0) {
                        const DatiBoa* d = dati_->boa(it->tipo);
                        if (d && !ping_disponibile(*it, *d)) continue;   // finita l'energia, la boa prima
                        AzioneAsw a;
                        a.tipo = AzioneAsw::Tipo::comanda_boa;
                        a.bersaglio = it->id;
                        a.pos = posizione_boa(*it, t);
                        a.oggetto = it->tipo;
                        out.push_back(a);
                        if (d) it->ping_secondi_usati += d->impulso_s;
                        t_dicass_ = t;
                        break;
                    }
            if (conf_.ricognizione && t - t_ricognizione_ >= 300.0) {   // ricognizione: le coordinate ogni 5 min
                t_ricognizione_ = t;
                AzioneAsw a;
                a.tipo = AzioneAsw::Tipo::passa_contatto;
                a.pos = traccia_.pos;
                a.bersaglio = traccia_.sorgente;
                a.codice = "ricognizione";
                out.push_back(a);
            }
            if (!simulato_ && conf_.situazione != SituazioneAsw::guerra && !conf_.ricognizione) {   // pace: attacco simulato (§7.1)
                simulato_ = true;
                AzioneAsw a;
                a.pos = traccia_.pos;
                a.bersaglio = traccia_.sorgente;
                if (conf_.situazione == SituazioneAsw::pace && usa() && carico_marcatori_["mk84"] > 0) {
                    --carico_marcatori_["mk84"];
                    a.tipo = AzioneAsw::Tipo::sus;
                    a.oggetto = "mk84";
                    a.codice = "attacco_simulato";
                } else {
                    a.tipo = AzioneAsw::Tipo::rapporto;
                    a.codice = conf_.situazione == SituazioneAsw::pace ? "attacco_simulato" : "contatto";
                }
                out.push_back(a);
            }
            if (conf_.situazione == SituazioneAsw::guerra && conf_.armi && armi_rimaste() > 0 && !arma_adatta(p).empty() && traccia_.errore_m <= 914.0) {
                cambia(StatoAsw::attacco, t, "guerra");
                break;
            }
            // boe davanti alla traccia ogni 20-30 min (USA) o ogni ~26 min (URSS); dip ogni ciclo
            if (sonar_ && dip_possibile(p)) {
                if (dip_.finito() && dip_da_fare_.empty()) {
                    const double avanti = traccia_.velocita_kn.value_or(5.0) * NODO * 300.0;
                    prossimo_dip(traccia_.rotta_deg ? polare(traccia_.pos, *traccia_.rotta_deg, avanti) : traccia_.pos,
                                 t, true);
                }
            } else if (da_posare_.empty() && t - t_schema_inseguimento_ >= 1500.0) {
                t_schema_inseguimento_ = t;
                ParametriSchema par = parametri_traccia(t, 7408.0);
                pianifica(usa() ? SchemaBoe::trappola : SchemaBoe::intercettazione,
                          usa() ? FunzioneBoa::passiva_dir : FunzioneBoa::passiva_omni, usa() ? 2 : 0, par, t);
            }
            break;
        }
        case StatoAsw::attacco: {
            if (conf_.situazione != SituazioneAsw::guerra || !conf_.armi || armi_rimaste() == 0) {
                cambia(StatoAsw::inseguimento, t, "niente attacco");
                break;
            }
            if (!recente) { cambia(StatoAsw::perso, t, "contatto perso"); break; }
            if (arma_adatta(p).empty()) { cambia(StatoAsw::inseguimento, t, "fondale troppo basso per le armi"); break; }
            // elicotteri USA: dall'hover sul proprio contatto (0,4) o in volo; URSS: un altro del gruppo gia' armato
            if (!usa() && mezzo_->elicottero && conf_.numero_gruppo > 1 && t_lancio_ < 0.0 &&
                caso.uniforme() < P.attacco_altro) {
                AzioneAsw a;
                a.tipo = AzioneAsw::Tipo::passa_contatto;
                a.pos = traccia_.pos;
                a.bersaglio = traccia_.sorgente;
                a.codice = "ataka";
                out.push_back(a);
                cambia(StatoAsw::inseguimento, t, "attacca il gruppo");
                break;
            }
            // il fix invecchia: se la strada incerta fatta dal fix supera l'acquisizione del Mk 46 (1600 yd, §6.7) non
            // si sgancia, si rifa' il fix (l'errore del fix lo ha gia' giudicato la localizzazione)
            if (incertezza_prevista(t) - traccia_.errore_m > ACQUISIZIONE_MK46_M) {
                cambia(StatoAsw::localizzazione, t, "fix vecchio");
                break;
            }
            const Vec2 mira = punto_previsto(t);
            const bool da_hover = dip_.in_hover() && usa() && distanza(p.pos, mira) <= 1500.0;
            if (dip_.in_hover() && !da_hover) dip_.recupera(t);
            const double soglia = mezzo_->velocita_ricerca_kn * NODO * dati_->pensiero_s * 0.6 + 300.0;
            if (da_hover || (!dip_.in_hover() && distanza(p.pos, mira) <= soglia)) {
                if (da_hover && caso.uniforme() >= P.attacco_hover) dip_.recupera(t);   // meglio in volo
                else {
                    lancia(t, p, caso, da_hover, out);
                    attesa_verifica_s_ = 360.0 + 180.0 * caso.uniforme();   // corsa del siluro 6-9 min
                    cambia(StatoAsw::verifica, t, "arma lanciata");
                }
            }
            break;
        }
        case StatoAsw::verifica:
            if (colpito_) {
                colpito_ = false;
                traccia_ = TracciaAsw{};
                cambia(StatoAsw::ricerca, t, "bersaglio colpito");
                break;
            }
            if (t - t_stato_ < attesa_verifica_s_) break;
            if (recente && traccia_.errore_m <= 914.0 && conf_.armi && armi_rimaste() > 0 && !arma_adatta(p).empty() && t - t_lancio_ >= 300.0 &&
                t_ultima_rivelazione_ > t_lancio_)
                cambia(StatoAsw::attacco, t, "nuovo fix");
            else if (recente)
                cambia(StatoAsw::localizzazione, t, "fix da rifare");
            else
                cambia(StatoAsw::perso, t, "contatto perso dopo l'attacco");
            break;
        case StatoAsw::perso: {
            if (!scelta_fatta_) {
                scelta_fatta_ = true;
                da_posare_.clear();
                dip_da_fare_.clear();
                ParametriSchema par = parametri_traccia(t, 7408.0);
                if (sonar_ && dip_possibile(p)) {
                    if (caso.uniforme() < P.perso_spirale) {   // spirale di dip
                        par.portata_m = sonar_->salto_inseguimento_m;
                        par.raggio0_m = 0.5 * sonar_->salto_inseguimento_m;
                        for (const Vec2 v : posa_schema(SchemaBoe::spirale, par)) dip_da_fare_.push_back(v);
                    } else {   // salto lungo davanti alla rotta
                        const double avanti = par.velocita_kn * NODO * par.ritardo_s * 1.5 + sonar_->salto_inseguimento_m;
                        dip_da_fare_.push_back(polare(traccia_.pos, par.rotta_deg.value_or(conf_.rotta_presunta), avanti));
                    }
                    dip_inseguimento_ = true;
                } else {
                    scegli_schema(usa() || anello_chiude(par) ? "perso" : "arrivo_campo", par, t, caso);
                }
            }
            if (sentito || mad) {
                const double previsto = traccia_.errore_m + traccia_.velocita_kn.value_or(8.0) * NODO * (t - traccia_.t);
                cambia(distanza(traccia_.pos, p.pos) <= 2.0 * previsto + 5000.0 && ultima_vera_
                           ? StatoAsw::localizzazione : StatoAsw::contatto, t, "riacquisizione");
                break;
            }
            if (t - t_stato_ > P.rinuncia_s) {
                traccia_ = TracciaAsw{};
                cambia(StatoAsw::ricerca, t, "rinuncia");
            }
            break;
        }
        default: break;
    }

    gestisci_dip(t, p, caso, out);
    posa_in_volo(t, p, caso, out);
    imposta_volo(p);
    return out;
}

}  // namespace m688
