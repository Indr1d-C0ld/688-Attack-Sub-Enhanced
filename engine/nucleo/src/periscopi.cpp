#include "nucleo/periscopi.hpp"

#include "nucleo/meteo.hpp"
#include "nucleo/onde.hpp"

#include <algorithm>
#include <cmath>
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
std::vector<std::string> parole(const toml::table& t, const char* k) {
    std::vector<std::string> v;
    if (const auto* a = t[k].as_array())
        for (const auto& x : *a)
            if (const auto s = x.value<std::string>()) v.push_back(*s);
    return v;
}
std::pair<double, double> coppia(const toml::table& t, const char* k) {
    const auto v = numeri(t, k);
    return {v.size() > 0 ? v[0] : 0.0, v.size() > 1 ? v[1] : 0.0};
}

double lineare(double x, double x0, double x1, double y0, double y1) {
    if (x1 <= x0) return x >= x1 ? y1 : y0;
    return y0 + (y1 - y0) * std::clamp((x - x0) / (x1 - x0), 0.0, 1.0);
}

// --- visibilita' (STIME, dossier 20 §5 e §8.3; da tarare in P5) ---

constexpr double M_PER_M_EQUIVALENTE = 7000.0;   // portata di riferimento per metro di dimensione equivalente, di giorno
constexpr double PIUMA_M = 1.0;                  // dimensione equivalente della piuma 1 (testa di ricerca a 10 nodi)
constexpr double ORIZZONTE_VISIVO_M = 3843.0;    // 2,075 miglia per radice di metro (con la rifrazione)
constexpr double ALTEZZA_PIUMA_M = 0.5;
constexpr double AVVISATORE_S = 5.0;             // l'avvisatore su prima del periscopio (sovietici)
constexpr double RIPROVA_S = 30.0;               // l'IA non e' a quota periscopica: riprova
constexpr double MARGINE_QUOTA_M = 1.0;

double fattore_mare(int mare) {   // dossier 21 §6.1, vista su periscopio e piuma (STIMA su IAMSAR)
    static constexpr double F[] = {1.3, 1.3, 1.0, 0.8, 0.5, 0.3, 0.2, 0.1};
    return F[std::clamp(mare, 0, 7)];
}

// Quanto resta di giorno col sensore notturno (dossier 21 §6.2): di notte l'occhio ha 0,05 con le stelle e 0,02 col
// coperto, l'intensificatore 0,3 e 0,1; la bassa luminosita' e la TV fra i due (STIMA); il termico non sente le nubi
double fattore_notte(ModoVista sensore, double luce, double nubi_ottavi) {
    const double c = std::clamp(nubi_ottavi / 8.0, 0.0, 1.0);
    const double n = sensore == ModoVista::termico ? 0.6
                   : sensore == ModoVista::intensificatore ? 0.3 - 0.2 * c
                   : sensore == ModoVista::bassa_luminosita || sensore == ModoVista::tv ? 0.25 - 0.17 * c : 0.05 - 0.03 * c;
    return lineare(luce, 0.1, 0.7, n, 1.0);
}

// La visibilita' per l'osservatore: il termico nella nebbia di radiazione vede fino a 4 volte l'occhio, ma niente
// sotto i 300 m (dossier 21 §6.2)
double portata_meteo(const Osservatore& o, const CondizioniVista& c) {
    if (o.notte == ModoVista::termico && c.nebbia_radiazione && c.visibilita_m > 300.0) return 4.0 * c.visibilita_m;
    return c.visibilita_m;
}

double tasso_massimo(TipoOsservatore t) {   // guardate per secondo con la testa vicina (STIMA)
    switch (t) {
        case TipoOsservatore::nave: return 0.1;
        case TipoOsservatore::aereo: return 0.08;
        case TipoOsservatore::periscopio: return 0.15;   // guarda apposta, per i suoi 10 s
    }
    return 0.1;
}

double orizzonte_visivo_m(double occhio_m, double bersaglio_m) {
    return ORIZZONTE_VISIVO_M * (std::sqrt(std::max(occhio_m, 0.0)) + std::sqrt(std::max(bersaglio_m, 0.0)));
}

int rango_notte(ModoVista x) {
    return x == ModoVista::termico ? 4 : x == ModoVista::intensificatore ? 3 : x == ModoVista::giorno ? 0 : 2;
}

ModoVista miglior_notte(const DatiPeriscopio* p, ModoVista m = ModoVista::giorno) {
    if (!p) return m;
    for (const ModoVista x : p->modi)
        if (rango_notte(x) > rango_notte(m)) m = x;
    return m;
}

DatiPeriscopio type2_di_riserva() {
    DatiPeriscopio p;
    p.id = "type2";
    p.nome = "Type 2 (attacco)";
    p.ingrandimenti = {1.5, 6.0};
    p.campi_gradi = {32.0, 8.0};
    p.elevazione_max = 74.5;
    p.modi = {ModoVista::giorno};
    p.reticolo_gradi = {4.0, 1.0};
    p.reticolo_fine_gradi = {1.0, 0.25};
    return p;
}

}  // namespace

const char* nome_modo_vista(ModoVista m) {
    switch (m) {
        case ModoVista::giorno: return "giorno";
        case ModoVista::bassa_luminosita: return "bassa_luminosita";
        case ModoVista::intensificatore: return "intensificatore";
        case ModoVista::tv: return "tv";
        case ModoVista::termico: return "termico";
    }
    return "";
}

std::optional<ModoVista> modo_vista_da_nome(const std::string& s) {
    for (const ModoVista m : {ModoVista::giorno, ModoVista::bassa_luminosita, ModoVista::intensificatore, ModoVista::tv,
                              ModoVista::termico})
        if (s == nome_modo_vista(m)) return m;
    return std::nullopt;
}

// --- catalogo ---

const DatiPeriscopio* CatalogoPeriscopi::periscopio(const std::string& id) const {
    const auto it = periscopi.find(id);
    return it == periscopi.end() ? nullptr : &it->second;
}

const DatiClassePeriscopi* CatalogoPeriscopi::classe(const std::string& id) const {
    const auto it = classi.find(id);
    return it == classi.end() ? nullptr : &it->second;
}

std::vector<const DatiPeriscopio*> CatalogoPeriscopi::di_classe(const std::string& id) const {
    std::vector<const DatiPeriscopio*> v;
    if (const auto* c = classe(id))
        for (const auto& p : c->periscopi)
            if (const auto* d = periscopio(p)) v.push_back(d);
    return v;
}

CatalogoPeriscopi carica_periscopi(const std::string& percorso) {
    toml::table t;
    try {
        t = toml::parse_file(percorso);
    } catch (const toml::parse_error& e) {
        throw std::runtime_error(percorso + ": " + std::string(e.description()));
    }
    const auto* tp = t["periscopi"].as_table();
    if (!tp || tp->empty()) throw std::runtime_error(percorso + ": manca la tabella [periscopi]");
    CatalogoPeriscopi c;
    for (auto&& [k, nodo] : *tp) {
        const auto* r = nodo.as_table();
        if (!r) continue;
        DatiPeriscopio p;
        p.id = std::string(k.str());
        p.nome = str(*r, "nome", p.id);
        p.ruolo = str(*r, "ruolo", p.ruolo);
        p.nazione = str(*r, "nazione", p.nazione);
        p.ingrandimenti = numeri(*r, "ingrandimenti");
        p.campi_gradi = numeri(*r, "campi_gradi");
        if (p.ingrandimenti.empty() || p.ingrandimenti.size() != p.campi_gradi.size())
            throw std::runtime_error(percorso + ": " + p.id + ": ingrandimenti e campi_gradi mancanti o diversi");
        p.elevazione_min = num(*r, "elevazione_min", p.elevazione_min);
        p.elevazione_max = num(*r, "elevazione_max", p.elevazione_max);
        p.telemetro = str(*r, "telemetro", p.telemetro);
        std::tie(p.telemetro_altezza_min_m, p.telemetro_altezza_max_m) = coppia(*r, "telemetro_altezza_m");
        std::tie(p.telemetro_distanza_min_m, p.telemetro_distanza_max_m) = coppia(*r, "telemetro_distanza_m");
        p.telemetro_angolo_max_gradi = num(*r, "telemetro_angolo_max_gradi", 0.0);
        for (const auto& m : parole(*r, "modi")) {
            const auto x = modo_vista_da_nome(m);
            if (!x) throw std::runtime_error(percorso + ": " + p.id + ": modo sconosciuto '" + m + "'");
            p.modi.push_back(*x);
        }
        if (p.modi.empty()) p.modi = {ModoVista::giorno};
        p.tv = vero(*r, "tv", false);
        p.foto = vero(*r, "foto", false);
        p.esm = vero(*r, "esm", false);
        p.stabilizzato = vero(*r, "stabilizzato", false);
        p.testa_m = num(*r, "testa_m", p.testa_m);
        p.obiettivo_mm = num(*r, "obiettivo_mm", p.obiettivo_mm);
        p.trasmissione = num(*r, "trasmissione", p.trasmissione);
        p.sezione_m2 = num(*r, "sezione_m2", p.sezione_m2);
        p.piuma = num(*r, "piuma", p.piuma);
        p.alzata_s = num(*r, "alzata_s", p.alzata_s);
        p.discesa_s = num(*r, "discesa_s", p.discesa_s);
        p.v_libera_kn = num(*r, "v_libera_kn", p.v_libera_kn);
        p.v_tremolio_kn = num(*r, "v_tremolio_kn", p.v_tremolio_kn);
        p.v_inservibile_kn = num(*r, "v_inservibile_kn", p.v_inservibile_kn);
        p.v_danno_kn = num(*r, "v_danno_kn", p.v_danno_kn);
        p.reticolo = str(*r, "reticolo", p.reticolo);
        p.reticolo_gradi = numeri(*r, "reticolo_gradi");
        p.reticolo_fine_gradi = numeri(*r, "reticolo_fine_gradi");
        p.fermi_gradi = numeri(*r, "fermi_gradi");
        p.filtri = parole(*r, "filtri");
        p.stime = parole(*r, "stime");
        c.periscopi[p.id] = std::move(p);
    }
    if (const auto* tc = t["classi"].as_table())
        for (auto&& [k, nodo] : *tc) {
            const auto* r = nodo.as_table();
            if (!r) continue;
            DatiClassePeriscopi d;
            d.periscopi = parole(*r, "periscopi");
            for (const auto& p : d.periscopi)
                if (!c.periscopio(p))
                    throw std::runtime_error(percorso + ": classe " + std::string(k.str()) + ": periscopio sconosciuto '" + p + "'");
            d.quota_m = num(*r, "quota_m", d.quota_m);
            d.esposto_m = num(*r, "esposto_m", d.esposto_m);
            d.esposizione_s = num(*r, "esposizione_s", d.esposizione_s);
            const auto iv = numeri(*r, "intervallo_s");
            if (iv.size() >= 2) { d.intervallo_min_s = iv[0]; d.intervallo_max_s = iv[1]; }
            d.avvisatore_prima = vero(*r, "avvisatore_prima", false);
            d.avvisatore_quota_m = num(*r, "avvisatore_quota_m", 0.0);
            d.tv = str(*r, "tv");
            d.tv_quota_max_m = num(*r, "tv_quota_max_m", 0.0);
            d.telecamera_verticale = vero(*r, "telecamera_verticale", false);
            c.classi[std::string(k.str())] = std::move(d);
        }
    return c;
}

// --- geometria, tremolio, danno, piuma ---

double altezza_significativa_m(int stato_mare) { return centro_douglas(stato_mare); }

double sigma_quota_m(int stato_mare) {
    // la tabella del dossier 21 §7.3 per il 688; per la testa ne resta 0,4: il comandante tiene la quota (guardata alta,
    // periscopio piu' su). Taratura M6 del 25/10/2026: col fattore la testa coperta torna all'1-3% a mare 4 e al 7-15% a
    // mare 5 del §8 punto 8 (con la tabella intera era il 14% e il 19%)
    static constexpr double S[] = {0.3, 0.4, 0.5, 0.6, 1.5, 2.5, 4.0, 6.0};
    constexpr double TENUTA = 0.4;
    return TENUTA * S[std::clamp(stato_mare, 0, 7)];
}

double altezza_testa_m(const DatiClassePeriscopi& c, double quota_m, double alzata) {
    const double piena = std::min(c.esposto_m + c.quota_m - quota_m, c.esposto_m + c.quota_m);
    return std::max(0.0, piena - (1.0 - std::clamp(alzata, 0.0, 1.0)) * CORSA_PERISCOPIO_M);
}

double tremolio(const DatiPeriscopio& p, double velocita_kn, int stato_mare) {
    const double v = std::abs(velocita_kn);
    double x = v <= p.v_tremolio_kn ? lineare(v, p.v_libera_kn, p.v_tremolio_kn, 0.0, 0.5)
                                    : lineare(v, p.v_tremolio_kn, p.v_inservibile_kn, 0.5, 1.0);
    x += 0.08 * std::max(0, stato_mare - 2);   // il mare scuote la testa (STIMA)
    if (p.stabilizzato) x *= 0.5;
    return std::clamp(x, 0.0, 1.0);
}

double probabilita_danno_s(const DatiPeriscopio& p, double velocita_kn) {
    const double oltre = std::abs(velocita_kn) - p.v_danno_kn;
    return oltre <= 0.0 ? 0.0 : std::min(0.5, 0.004 * oltre);
}

double piuma(const DatiPeriscopio& p, double velocita_kn) {
    return p.piuma * std::clamp((std::abs(velocita_kn) - 2.0) / 8.0, 0.0, 1.5);
}

SagomaTesta sagoma_testa(const DatiPeriscopio& p, double altezza_m, double velocita_kn) {
    if (altezza_m <= 0.0) return {};
    return {p.sezione_m2 * altezza_m * altezza_m, altezza_m, piuma(p, velocita_kn)};
}

// --- visibilita' ---

double portata_visiva_m(const DatiPeriscopio& p, const SagomaTesta& testa, const Osservatore& o, const CondizioniVista& c) {
    if (testa.altezza_m <= 0.0) return 0.0;
    double ft = 1.0, fp = 1.0;   // l'aereo vede bene la scia dall'alto e poco la testa; il periscopio guarda radente
    if (o.tipo == TipoOsservatore::aereo) { ft = 0.6; fp = 1.6; }
    else if (o.tipo == TipoOsservatore::periscopio) { ft = 0.8; fp = 0.8; }
    const double s_testa = std::sqrt(p.testa_m * testa.altezza_m) * ft;
    const double s_piuma = PIUMA_M * testa.piuma * fp;
    const double dimensione = std::hypot(s_testa, s_piuma);
    double r = M_PER_M_EQUIVALENTE * dimensione * fattore_mare(c.stato_mare) * fattore_notte(o.notte, c.luce, c.nubi_ottavi) *
               (1.0 + 0.5 * std::clamp(c.riflesso, 0.0, 1.0)) * (o.allertato ? 1.0 : 0.7);
    const double alto = testa.piuma > 0.0 ? std::max(testa.altezza_m, ALTEZZA_PIUMA_M) : testa.altezza_m;
    return std::min({r, orizzonte_visivo_m(o.altezza_occhio_m, alto), portata_meteo(o, c)});
}

double probabilita_vista(const DatiPeriscopio& p, const SagomaTesta& testa, const Osservatore& o,
                         const CondizioniVista& c, double distanza_m, double dt) {
    const double r = portata_visiva_m(p, testa, o, c);
    if (!(r > 0.0) || dt <= 0.0) return 0.0;
    const double alto = testa.piuma > 0.0 ? std::max(testa.altezza_m, ALTEZZA_PIUMA_M) : testa.altezza_m;
    if (distanza_m > orizzonte_visivo_m(o.altezza_occhio_m, alto) || distanza_m > portata_meteo(o, c)) return 0.0;
    const double tasso = tasso_massimo(o.tipo) / (1.0 + std::pow(distanza_m / r, 4.0));
    return 1.0 - std::exp(-tasso * dt);
}

// --- posto del periscopio ---

PostoPeriscopi::PostoPeriscopi() : periscopi_{type2_di_riserva()} { stato_.scelto = periscopi_.front().id; }

PostoPeriscopi::PostoPeriscopi(const CatalogoPeriscopi& cat, const std::string& classe) {
    for (const auto* p : cat.di_classe(classe)) periscopi_.push_back(*p);
    if (const auto* c = cat.classe(classe)) classe_ = *c;
    if (periscopi_.empty()) periscopi_ = {type2_di_riserva()};
    stato_.scelto = periscopi_.front().id;
}

void PostoPeriscopi::danneggia_testa(int indice, DannoTesta d, bool allaga) {
    if (indice < 0 || indice >= static_cast<int>(periscopi_.size())) return;
    danni_.resize(periscopi_.size(), DannoTesta::nessuno);
    allaga_.resize(periscopi_.size(), false);
    danni_[static_cast<std::size_t>(indice)] = d;
    allaga_[static_cast<std::size_t>(indice)] = allaga && d == DannoTesta::cieca;
}

PostoPeriscopi::DannoTesta PostoPeriscopi::danno_testa(int indice) const {
    return indice >= 0 && indice < static_cast<int>(danni_.size()) ? danni_[static_cast<std::size_t>(indice)] : DannoTesta::nessuno;
}

bool PostoPeriscopi::canna_aperta() const {
    return danno_testa(indice_) == DannoTesta::cieca && indice_ < static_cast<int>(allaga_.size()) &&
           allaga_[static_cast<std::size_t>(indice_)] && stato_.alzata > 0.0;
}

int PostoPeriscopi::indice_ruolo(const std::string& ruolo) const {
    for (std::size_t i = 0; i < periscopi_.size(); ++i)
        if (periscopi_[i].ruolo == ruolo) return static_cast<int>(i);
    return -1;
}

bool PostoPeriscopi::scegli(int indice) {
    if (indice < 0 || indice >= static_cast<int>(periscopi_.size())) return false;
    if (indice == indice_) { prossimo_ = -1; return true; }
    if (bloccato() && stato_.alzata > 0.0) return false;   // bloccato su: non scende, l'altro non puo' salire
    if (stato_.alzata > 0.0) { prossimo_ = indice; return true; }
    indice_ = indice;
    const DatiPeriscopio& d = *dati();
    stato_.scelto = d.id;
    stato_.ingrandimento = std::min(stato_.ingrandimento, static_cast<int>(d.ingrandimenti.size()) - 1);
    if (std::find(d.modi.begin(), d.modi.end(), stato_.modo) == d.modi.end()) stato_.modo = ModoVista::giorno;
    stato_.elevazione = std::clamp(stato_.elevazione, d.elevazione_min, d.elevazione_max);
    stato_.filtro = -1;
    stato_.sdoppiamento_gradi = 0.0;
    return true;
}

void PostoPeriscopi::elevazione(double gradi) {
    const DatiPeriscopio& d = *dati();
    stato_.elevazione = std::clamp(gradi, d.elevazione_min, d.elevazione_max);
}

void PostoPeriscopi::ingrandimento(int indice) {
    stato_.ingrandimento = std::clamp(indice, 0, static_cast<int>(dati()->ingrandimenti.size()) - 1);
}

void PostoPeriscopi::modo(ModoVista m) {
    const auto& modi = dati()->modi;
    if (std::find(modi.begin(), modi.end(), m) != modi.end()) stato_.modo = m;
}

void PostoPeriscopi::filtro(int indice) {
    stato_.filtro = indice >= 0 && indice < static_cast<int>(dati()->filtri.size()) ? indice : -1;
}

void PostoPeriscopi::sdoppiamento(double gradi) {
    const DatiPeriscopio& d = *dati();
    // col reticolo (sovietici) e' l'angolo letto sulla scala verticale, fino a meta' della scala (kit, 31/10/2026)
    if (d.telemetro == "reticolo") stato_.sdoppiamento_gradi = std::clamp(gradi, 0.0, d.telemetro_angolo_max_gradi);
    else stato_.sdoppiamento_gradi = d.telemetro == "sdoppiato" ? std::clamp(gradi, -d.telemetro_angolo_max_gradi, d.telemetro_angolo_max_gradi) : 0.0;
}

void PostoPeriscopi::telecamera_verticale(bool si) { stato_.telecamera_verticale = si && classe_.telecamera_verticale; }

bool PostoPeriscopi::avanza(double t, double dt, double quota_m, double velocita_kn, int stato_mare, Casuale& caso, double hs_m) {
    const bool sale = stato_.comandato_su && !stato_.danneggiato && prossimo_ < 0;
    if (!bloccato()) {   // la testa piegata non sale e non scende piu' (22 §5.3)
        const DatiPeriscopio& d = *dati();
        stato_.alzata = std::clamp(stato_.alzata + (sale ? dt / std::max(d.alzata_s, 0.1) : -dt / std::max(d.discesa_s, 0.1)), 0.0, 1.0);
    }
    if (prossimo_ >= 0 && stato_.alzata <= 0.0) {   // l'altro e' giu': si cambia
        const int n = prossimo_;
        prossimo_ = -1;
        scegli(n);
    }
    const DatiPeriscopio& d = *dati();
    stato_.altezza_testa_m = altezza_testa_m(classe_, quota_m, stato_.alzata);
    stato_.testa_fuori = stato_.altezza_testa_m > 0.0;
    stato_.su = stato_.alzata >= 1.0 || (stato_.su && stato_.testa_fuori && stato_.alzata > 0.0);
    // l'errore di quota col mare (AR(1) di 20 s): la testa vera sale e scende; poi l'onda nel punto della testa
    const double hs = hs_m >= 0.0 ? hs_m : centro_douglas(stato_mare);
    if (stato_.testa_fuori) {
        const double f = std::exp(-dt / 20.0), u1 = std::max(caso.uniforme(), 1e-300), u2 = caso.uniforme();
        const double g = std::sqrt(-2.0 * std::log(u1)) * std::cos(2.0 * PI * u2);
        errore_quota_m_ = f * errore_quota_m_ + std::sqrt(1.0 - f * f) * sigma_quota_m(classe_douglas(hs)) * g;
    } else {
        errore_quota_m_ = 0.0;
    }
    stato_.altezza_vera_m = stato_.testa_fuori ? std::max(0.0, stato_.altezza_testa_m - errore_quota_m_) : 0.0;
    stato_.finestra_coperta = stato_.testa_fuori && elevazione_onda_m(hs, t) > stato_.altezza_vera_m - FINESTRA_M;
    // esposizione
    const bool aperta = !esposizioni_.empty() && esposizioni_.back().fine < 0.0;
    if (stato_.testa_fuori) {
        if (!aperta) esposizioni_.push_back({d.id, t, -1.0});
        stato_.esposto_s += dt;
    } else {
        if (aperta) {
            esposizioni_.back().fine = t;
            finite_.push_back(esposizioni_.back());
        }
        stato_.esposto_s = 0.0;
    }
    // il tremolio della velocita' e del mare, piu' il moto della quota (dossier 21 §8 punto 11)
    stato_.tremolio = stato_.testa_fuori
                          ? std::min(1.0, tremolio(d, velocita_kn, classe_douglas(hs)) + 0.05 * sigma_quota_m(classe_douglas(hs)))
                          : 0.0;
    // danno: l'albero alzato oltre la velocita' di danno
    if (stato_.alzata > 0.0 && !stato_.danneggiato) {
        const double ps = probabilita_danno_s(d, velocita_kn);
        if (ps > 0.0 && caso.uniforme() < 1.0 - std::pow(1.0 - ps, dt)) {
            danneggia();
            return true;
        }
    }
    return false;
}

SagomaTesta PostoPeriscopi::sagoma(double velocita_kn) const {
    return sagoma_testa(*dati(), stato_.altezza_vera_m, velocita_kn);
}

double PostoPeriscopi::esposizione_totale_s() const {
    double s = 0.0;
    for (const auto& e : esposizioni_) s += e.fine < 0.0 ? stato_.esposto_s : e.durata();
    return s;
}

std::vector<EsposizionePeriscopio> PostoPeriscopi::finite() {
    std::vector<EsposizionePeriscopio> v;
    v.swap(finite_);
    return v;
}

// --- periscopi dell'IA ---

PeriscopioIa::PeriscopioIa(const CatalogoPeriscopi& cat, const std::string& classe, double t, Casuale& caso) {
    if (const auto* c = cat.classe(classe)) classe_ = *c;
    // per guardare: quello di ricerca o universale, altrimenti il primo che non sia la sola TV
    for (const auto* p : cat.di_classe(classe)) {
        if (p->ruolo == "tv") continue;
        if (!modello_ || ((p->ruolo == "ricerca" || p->ruolo == "universale") && modello_->ruolo == "attacco")) modello_ = p;
        notte_ = miglior_notte(p, notte_);   // di notte il comandante usa quello che vede meglio (CH84: termico)
    }
    if (!classe_.tv.empty()) tv_ = cat.periscopio(classe_.tv);
    programma(t, caso);
}

void PeriscopioIa::programma(double t, Casuale& caso) {
    prossima_ = t + caso.uniforme(classe_.intervallo_min_s, std::max(classe_.intervallo_min_s, classe_.intervallo_max_s));
}

void PeriscopioIa::avanza(double t, double quota_m, double velocita_kn, bool allarme, bool radar_sentito, double luce,
                          Casuale& caso) {
    const bool a_quota = quota_m <= classe_.quota_m + MARGINE_QUOTA_M;
    switch (fase_) {
        case FaseGuardata::sotto:
            if (t < prossima_ || (!modello_ && !tv_)) return;
            if (allarme) { programma(t, caso); return; }
            if (tv_ && quota_m <= classe_.tv_quota_max_m && luce > 0.5 && !a_quota) {
                fase_ = FaseGuardata::tv;   // МТ-70: la superficie senza esporsi
                fine_ = t + classe_.esposizione_s;
                ++guardate_;
            } else if (modello_ && a_quota && std::abs(velocita_kn) <= modello_->v_libera_kn) {
                fase_ = classe_.avvisatore_prima ? FaseGuardata::avvisatore : FaseGuardata::esposto;
                fine_ = t + (classe_.avvisatore_prima ? AVVISATORE_S : classe_.esposizione_s);
                if (!classe_.avvisatore_prima) ++guardate_;
            } else {
                prossima_ = t + RIPROVA_S;
            }
            return;
        case FaseGuardata::avvisatore:
            if (allarme || radar_sentito || !a_quota) { fase_ = FaseGuardata::sotto; programma(t, caso); }
            else if (t >= fine_) { fase_ = FaseGuardata::esposto; fine_ = t + classe_.esposizione_s; ++guardate_; }
            return;
        case FaseGuardata::esposto:
        case FaseGuardata::tv:
            if (allarme || t >= fine_ || (fase_ == FaseGuardata::esposto && !a_quota)) { fase_ = FaseGuardata::sotto; programma(t, caso); }
            return;
    }
}

double PeriscopioIa::altezza_testa_m(double quota_m) const {
    return fase_ == FaseGuardata::esposto ? m688::altezza_testa_m(classe_, quota_m, 1.0) : 0.0;
}

Osservatore PeriscopioIa::osservatore(double quota_m) const {
    Osservatore o;
    o.tipo = TipoOsservatore::periscopio;
    o.altezza_occhio_m = fase_ == FaseGuardata::tv ? 0.5 : std::max(0.5, altezza_testa_m(quota_m));
    o.notte = fase_ == FaseGuardata::tv ? miglior_notte(tv_) : notte_;
    o.allertato = true;
    return o;
}

}  // namespace m688
