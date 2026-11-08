#include "nucleo/manovre_tma.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>

#include <toml.hpp>

namespace m688 {

namespace {

// i campi numerici della dottrina per nome, per leggerli dal file e per le differenze delle classi
double* campo(DottrinaTma& d, const std::string& k) {
    if (k == "gamba_min_s") return &d.gamba_min_s;
    if (k == "gamba_max_s") return &d.gamba_max_s;
    if (k == "distanza_gamba_lunga_m") return &d.distanza_gamba_lunga_m;
    if (k == "mascone_deg") return &d.mascone_deg;
    if (k == "mascone_vicino_deg") return &d.mascone_vicino_deg;
    if (k == "virata_min_deg") return &d.virata_min_deg;
    if (k == "virata_max_deg") return &d.virata_max_deg;
    if (k == "vicino_m") return &d.vicino_m;
    if (k == "velocita_kn") return &d.velocita_kn;
    if (k == "velocita_max_kn") return &d.velocita_max_kn;
    if (k == "sigma_relativa") return &d.sigma_relativa;
    if (k == "raddrizzamento_s") return &d.raddrizzamento_s;
    if (k == "tolleranza_rotta_deg") return &d.tolleranza_rotta_deg;
    if (k == "margine_fondo_m") return &d.margine_fondo_m;
    if (k == "tenuta_s") return &d.tenuta_s;
    if (k == "variazione_gamba") return &d.variazione_gamba;
    return nullptr;
}
// un campo per nome, numerico o intero; falso se non c'e'
bool imposta(DottrinaTma& d, const std::string& k, double v) {
    if (k == "gambe_max") return d.gambe_max = static_cast<int>(v), true;
    if (k == "gambe_max_specchio") return d.gambe_max_specchio = static_cast<int>(v), true;
    if (double* p = campo(d, k)) return *p = v, true;
    return false;
}

constexpr double DISTANZA_IGNOTA_FRAZIONE = 0.5;   // senza stima: la gamba di meta' strada (10 km per la NATO)
constexpr double CONTROLLO_VIRATA_S = 10.0;
constexpr double LATI_PARI_DEG = 20.0;             // due virate che differiscono meno di cosi': il lato a caso

}  // namespace

const char* nome_fase_tma(DecisioneTma::Fase f) {
    switch (f) {
        case DecisioneTma::Fase::gamba: return "gamba";
        case DecisioneTma::Fase::virata: return "virata";
        case DecisioneTma::Fase::raddrizzamento: return "raddrizzamento";
        case DecisioneTma::Fase::soluzione: return "soluzione";
    }
    return "?";
}

CatalogoTma carica_tma(const std::string& percorso) {
    const toml::table t = toml::parse_file(percorso);
    const auto* tma = t["tma"].as_table();
    if (!tma) throw std::runtime_error(percorso + ": senza [tma]");
    CatalogoTma c;
    for (const auto& [k, n] : *tma) {
        const std::string chiave(k.str());
        const auto* tab = n.as_table();
        if (!tab) continue;
        if (chiave == "marine") {
            for (const auto& [m, v] : *tab) c.marine[std::string(m.str())] = v.value_or(std::string{});
        } else if (chiave == "classi") {
            for (const auto& [cl, v] : *tab)
                if (const auto* r = v.as_table()) {
                    auto& diff = c.classi[std::string(cl.str())];
                    for (const auto& [f, x] : *r)
                        if (x.is_number()) diff[std::string(f.str())] = x.value_or(0.0);
                    c.fonti_classi[std::string(cl.str())] = {(*r)["fonte"].value_or(std::string{}),
                                                             (*r)["confidenza"].value_or(std::string{})};
                }
        } else {
            DottrinaTma d;
            d.marina = chiave;
            for (const auto& [f, x] : *tab)
                if (x.is_number()) imposta(d, std::string(f.str()), x.value_or(0.0));
            d.fonte = (*tab)["fonte"].value_or(std::string{});
            d.confidenza = (*tab)["confidenza"].value_or(std::string{});
            c.dottrine[chiave] = d;
        }
    }
    for (const auto& [m, d] : c.marine)
        if (!c.dottrine.count(d)) throw std::runtime_error(percorso + ": la marina " + m + " punta a una dottrina che manca");
    if (!c.dottrine.count("nato")) throw std::runtime_error(percorso + ": manca la dottrina nato");
    return c;
}

DottrinaTma dottrina_tma(const CatalogoTma& c, const std::string& marina, const std::string& classe) {
    const auto m = c.marine.find(marina);
    const std::string nome = m != c.marine.end() ? m->second : (c.dottrine.count(marina) ? marina : "nato");
    DottrinaTma d = c.dottrine.at(nome);
    if (const auto it = c.classi.find(classe); it != c.classi.end()) {
        for (const auto& [k, v] : it->second) imposta(d, k, v);
        const auto& [fonte, conf] = c.fonti_classi.at(classe);
        d.fonte += "; " + classe + ": " + fonte;
        if (!conf.empty()) d.confidenza = conf;
    }
    return d;
}

double ManovreTma::durata_gamba(double distanza_m) {
    const double k = std::clamp(distanza_m / std::max(d_.distanza_gamba_lunga_m, 1.0), 0.0, 1.0);
    const double base = d_.gamba_min_s + k * (d_.gamba_max_s - d_.gamba_min_s);
    const double variazione = (caso_.uniforme() - 0.5) * d_.variazione_gamba * (d_.gamba_max_s - d_.gamba_min_s);
    return std::clamp(base + variazione, d_.gamba_min_s, d_.gamba_max_s);
}

double ManovreTma::rotta_gamba(double rilevamento, double distanza_m, int lato) const {
    // lato +1: il bersaglio sul mascone di dritta, cioe' a +mascone dalla prua
    const bool vicino = distanza_m < d_.vicino_m;
    const double m = vicino ? d_.mascone_vicino_deg : d_.mascone_deg;
    const double voluta = normalizza_360(rilevamento - lato * m);
    // la virata dalla gamba di prima resta fra il minimo e il massimo della dottrina, nel verso voluto: il rilevamento si
    // e' spostato durante la gamba. Da vicino si apre, anche oltre il massimo
    const double virata = normalizza_180(voluta - rotta_);
    const double modulo = std::clamp(std::fabs(virata), d_.virata_min_deg, vicino ? 180.0 : d_.virata_max_deg);
    return normalizza_360(rotta_ + (virata < 0.0 ? -modulo : modulo));
}

bool ManovreTma::fondo_buono(const SituazioneTma& s, double rotta, double durata_s) const {
    const double serve = s.quota_m + d_.margine_fondo_m;
    if (!s.fondale) return s.fondale_m <= 0.0 || s.fondale_m >= serve;   // senza carta: solo il fondale di adesso
    // lungo la gamba, a un quarto, a meta' e alla fine
    const double lunghezza = std::min(d_.velocita_kn, d_.velocita_max_kn) * 1852.0 / 3600.0 * durata_s;
    const double r = radianti(rotta);
    for (const double f : {0.25, 0.5, 1.0}) {
        const Vec2 p{s.pos.x + f * lunghezza * std::sin(r), s.pos.y + f * lunghezza * std::cos(r)};
        if (s.fondale(p) < serve) return false;
    }
    return true;
}

DecisioneTma ManovreTma::passo(const SituazioneTma& s) {
    DecisioneTma out;
    // la distanza della stima vale solo se osservabile: prima e' un numero qualunque lungo il rilevamento
    const double distanza = s.stima && s.stima->osservabile ? s.stima->soluzione.distanza_m
                                                            : DISTANZA_IGNOTA_FRAZIONE * d_.distanza_gamba_lunga_m;
    const double velocita = std::min(d_.velocita_kn, d_.velocita_max_kn);
    out.gambe = gambe_;
    out.rotta = s.rotta;
    out.velocita_kn = s.velocita_kn;
    out.prossima_s = s.t;
    // basta: la distanza e' osservabile e nota entro la sigma relativa. Se la soluzione si guasta le gambe riprendono
    if (s.stima && s.stima->osservabile && s.stima->sigma_distanza_m <= d_.sigma_relativa * std::max(distanza, 1.0)) {
        if (avviata_) fase_ = DecisioneTma::Fase::soluzione;
        out.fase = DecisioneTma::Fase::soluzione;
        out.soluzione_pronta = true;
        return out;
    }
    // rinuncia: contatto perso, specchio che non si risolve, troppe gambe
    if (s.t_ultimo_rilevamento && s.t - *s.t_ultimo_rilevamento > d_.tenuta_s) out.rinuncia = DecisioneTma::Rinuncia::contatto_perso;
    else if (s.specchio_irrisolto && gambe_ >= d_.gambe_max_specchio) out.rinuncia = DecisioneTma::Rinuncia::specchio;
    else if (gambe_ >= d_.gambe_max) out.rinuncia = DecisioneTma::Rinuncia::gambe;
    if (out.rinuncia != DecisioneTma::Rinuncia::no) {
        out.fase = fase_;
        return out;
    }
    // una gamba sul lato voluto se il fondo lo consente, altrimenti sull'altro; con tutti e due cattivi si resta dove si e'
    const auto nuova = [&](int lato_voluto) {
        const double durata = durata_gamba(distanza);
        for (const int lato : {lato_voluto, -lato_voluto}) {
            const double r = rotta_gamba(s.rilevamento, distanza, lato);
            if (fondo_buono(s, r, durata)) {
                lato_ = lato;
                rotta_ = r;
                durata_ = durata;
                return true;
            }
            out.fondo_rifiutato = true;
        }
        rotta_ = s.rotta;
        durata_ = durata;
        return false;
    };
    if (!avviata_) {   // la prima gamba: il mascone che costa la virata piu' corta (a parita', a caso)
        avviata_ = true;
        gambe_ = 0;
        rotta_ = s.rotta;
        const double m = distanza < d_.vicino_m ? d_.mascone_vicino_deg : d_.mascone_deg;
        const double a = std::fabs(normalizza_180(s.rilevamento - m - s.rotta));
        const double b = std::fabs(normalizza_180(s.rilevamento + m - s.rotta));
        const int lato = std::fabs(a - b) < LATI_PARI_DEG ? (caso_.uniforme() < 0.5 ? 1 : -1) : (a <= b ? 1 : -1);
        // la prima gamba non ha una virata minima: dalla rotta di adesso al mascone
        const double durata = durata_gamba(distanza);
        bool fatto = false;
        for (const int l : {lato, -lato}) {
            const double r = normalizza_360(s.rilevamento - l * m);
            if (fondo_buono(s, r, durata)) {
                lato_ = l;
                rotta_ = r;
                fatto = true;
                break;
            }
            out.fondo_rifiutato = true;
        }
        durata_ = durata;
        fase_ = DecisioneTma::Fase::virata;
        out.nuova_gamba = fatto;
    } else if (fase_ == DecisioneTma::Fase::soluzione) {
        fase_ = DecisioneTma::Fase::gamba;   // la soluzione si e' guastata: si riparte dalla gamba in corso
        inizio_gamba_ = s.t;
    }
    if (fase_ == DecisioneTma::Fase::virata && std::fabs(normalizza_180(s.rotta - rotta_)) <= d_.tolleranza_rotta_deg) {
        if (s.schiera) {
            fase_ = DecisioneTma::Fase::raddrizzamento;
            fine_raddrizzamento_ = s.t + d_.raddrizzamento_s;
        } else {
            fase_ = DecisioneTma::Fase::gamba;
            inizio_gamba_ = s.t;
        }
    }
    if (fase_ == DecisioneTma::Fase::raddrizzamento && s.t >= fine_raddrizzamento_) {
        fase_ = DecisioneTma::Fase::gamba;
        inizio_gamba_ = s.t;
    }
    if (fase_ == DecisioneTma::Fase::gamba && s.t - inizio_gamba_ >= durata_) {
        // gamba finita: il bersaglio sull'altro mascone, cosi' cambia la componente attraverso la linea di vista
        ++gambe_;
        if (nuova(-lato_)) {
            fase_ = DecisioneTma::Fase::virata;
            out.nuova_gamba = true;
        } else {
            inizio_gamba_ = s.t;   // tutti e due i lati sul basso: si tiene la rotta, e la gamba conta
        }
    }
    out.fase = fase_;
    out.rotta = rotta_;
    out.velocita_kn = velocita;
    out.gambe = gambe_;
    out.prossima_s = fase_ == DecisioneTma::Fase::virata ? s.t + CONTROLLO_VIRATA_S
                     : fase_ == DecisioneTma::Fase::raddrizzamento ? fine_raddrizzamento_
                                                                   : inizio_gamba_ + durata_;
    return out;
}

}  // namespace m688
