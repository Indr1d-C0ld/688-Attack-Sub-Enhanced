#include "nucleo/missili.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>

#include <toml.hpp>

namespace m688 {

namespace {

// --- ellissi e covarianze (x verso est, y verso nord; orientamento = rilevamento dell'asse maggiore) ---
struct Cov { double xx = 0.0, xy = 0.0, yy = 0.0; };

Cov cov_di(const EllisseErrore& e) {
    const double a = radianti(e.orientamento_deg);
    const double ux = std::sin(a), uy = std::cos(a);   // asse maggiore
    const double vx = std::cos(a), vy = -std::sin(a);  // asse minore
    const double A = e.maggiore_m * e.maggiore_m, B = e.minore_m * e.minore_m;
    return {A * ux * ux + B * vx * vx, A * ux * uy + B * vx * vy, A * uy * uy + B * vy * vy};
}

EllisseErrore ellisse_di(const Cov& c) {
    const double tr = c.xx + c.yy, det = c.xx * c.yy - c.xy * c.xy;
    const double d = std::sqrt(std::max(0.0, tr * tr / 4.0 - det));
    const double l1 = std::max(0.0, tr / 2.0 + d), l2 = std::max(0.0, tr / 2.0 - d);
    EllisseErrore e;
    e.maggiore_m = std::sqrt(l1);
    e.minore_m = std::sqrt(l2);
    // autovettore di l1: (c.xy, l1 - c.xx) oppure (l1 - c.yy, c.xy)
    double vx = c.xy, vy = l1 - c.xx;
    if (std::abs(vx) + std::abs(vy) < 1e-12) { vx = l1 - c.yy; vy = c.xy; }
    if (std::abs(vx) + std::abs(vy) < 1e-12) { vx = 0.0; vy = 1.0; }
    double o = normalizza_360(gradi(std::atan2(vx, vy)));
    if (o >= 180.0) o -= 180.0;
    e.orientamento_deg = o;
    return e;
}

Cov piu(const Cov& a, const Cov& b) { return {a.xx + b.xx, a.xy + b.xy, a.yy + b.yy}; }

Cov lungo(double rotta_deg, double sigma) {   // varianza sigma^2 lungo una direzione
    const double a = radianti(rotta_deg), ux = std::sin(a), uy = std::cos(a), s2 = sigma * sigma;
    return {s2 * ux * ux, s2 * ux * uy, s2 * uy * uy};
}

double sigma_lungo(const Cov& c, double rotta_deg) {
    const double a = radianti(rotta_deg), ux = std::sin(a), uy = std::cos(a);
    return std::sqrt(std::max(0.0, c.xx * ux * ux + 2.0 * c.xy * ux * uy + c.yy * uy * uy));
}

Vec2 avanti(Vec2 p, double rotta_deg, double d) {
    const double a = radianti(rotta_deg);
    return {p.x + d * std::sin(a), p.y + d * std::cos(a)};
}

Vec2 piu(Vec2 a, Vec2 b) { return {a.x + b.x, a.y + b.y}; }
Vec2 meno(Vec2 a, Vec2 b) { return {a.x - b.x, a.y - b.y}; }

// coordinate lungo la rotta (s) e attraverso (w, positivo a dritta) rispetto a un'origine
void lungo_attraverso(Vec2 p, Vec2 o, double rotta_deg, double& s, double& w) {
    const double a = radianti(rotta_deg), ux = std::sin(a), uy = std::cos(a);
    const double dx = p.x - o.x, dy = p.y - o.y;
    s = dx * ux + dy * uy;
    w = dx * uy - dy * ux;
}

bool dentro_poligono(Vec2 p, const std::vector<Vec2>& poly) {
    bool in = false;
    for (std::size_t i = 0, j = poly.size() - 1; i < poly.size(); j = i++) {
        const Vec2& a = poly[i];
        const Vec2& b = poly[j];
        if ((a.y > p.y) != (b.y > p.y) && p.x < (b.x - a.x) * (p.y - a.y) / (b.y - a.y) + a.x) in = !in;
    }
    return in;
}

double orizzonte_m(double h1, double h2) { return 4120.0 * (std::sqrt(std::max(0.0, h1)) + std::sqrt(std::max(0.0, h2))); }

// P che un punto gaussiano (centro, covarianza) cada nel poligono: quadratura deterministica su una griglia di
// N x N punti fra -3 e +3 sigma lungo gli assi dell'ellisse, pesati con la densita'
double p_nel_poligono(Vec2 centro, const EllisseErrore& e, const std::vector<Vec2>& poly) {
    if (poly.size() < 3) return 0.0;
    const double a = std::max(e.maggiore_m, 1.0), b = std::max(e.minore_m, 1.0);
    const double o = radianti(e.orientamento_deg);
    const double ux = std::sin(o), uy = std::cos(o), vx = std::cos(o), vy = -std::sin(o);
    constexpr int N = 25;
    double somma = 0.0, dentro = 0.0;
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j) {
            const double ki = -3.0 + 6.0 * (i + 0.5) / N, kj = -3.0 + 6.0 * (j + 0.5) / N;
            const double w = std::exp(-0.5 * (ki * ki + kj * kj));
            const Vec2 p{centro.x + ki * a * ux + kj * b * vx, centro.y + ki * a * uy + kj * b * vy};
            somma += w;
            if (dentro_poligono(p, poly)) dentro += w;
        }
    return somma > 0.0 ? dentro / somma * 0.9973 : 0.0;   // la massa fuori dai 3 sigma resta fuori
}

// il ventaglio del BOL dopo `t` secondi di ricerca: semiapertura e profondita' del cancello davanti al missile
double semiapertura_bol(double t) { return std::min(45.0, VENTAGLIO_INIZIO_GRADI + VENTAGLIO_PASSO_GRADI * t / VENTAGLIO_PASSO_S); }
double profondita_bol(double t) { return std::min(VENTAGLIO_MAX_M, VENTAGLIO_INIZIO_M + VENTAGLIO_PASSO_M * t / VENTAGLIO_PASSO_S); }

UscitaMissile uscita_da(const std::string& s, const std::string& id) {
    if (s == "capsula") return UscitaMissile::capsula;
    if (s == "booster_subacqueo") return UscitaMissile::booster_subacqueo;
    if (s == "pozzo") return UscitaMissile::pozzo;
    if (s == "contenitore") return UscitaMissile::contenitore;
    throw std::runtime_error("missile '" + id + "': uscita sconosciuta '" + s + "'");
}

RuoloMissile ruolo_da(const std::string& s, const std::string& id) {
    if (s == "antinave") return RuoloMissile::antinave;
    if (s == "attacco_terra") return RuoloMissile::attacco_terra;
    if (s == "asw") return RuoloMissile::asw;
    throw std::runtime_error("missile '" + id + "': ruolo sconosciuto '" + s + "'");
}

}  // namespace

// --- catalogo ---

const DatiMissile* CatalogoMissili::missile(const std::string& id) const {
    const auto it = missili.find(id);
    return it == missili.end() ? nullptr : &it->second;
}

const DatiMissile* CatalogoMissili::per_arma(const std::string& arma, int anno) const {
    for (const Arma& a : armi)
        if (a.arma == arma && anno >= a.dal && anno <= a.al) return missile(a.missile);
    return nullptr;
}

const DatiMissile* CatalogoMissili::per_classe(const std::string& classe, int anno) const {
    for (const Lanciatore& l : lanciatori)
        if (l.classe == classe && anno >= l.dal && anno <= l.al) return missile(l.missile);
    return nullptr;
}

const CatalogoMissili::Lanciatore* CatalogoMissili::lanciatore(const std::string& classe, int anno) const {
    for (const Lanciatore& l : lanciatori)
        if (l.classe == classe && anno >= l.dal && anno <= l.al) return &l;
    return nullptr;
}

const DifesaClasse* CatalogoMissili::difesa(const std::string& classe) const {
    const auto it = difese.find(classe);
    return it != difese.end() ? &it->second : nullptr;
}

CatalogoMissili carica_missili(const std::string& percorso) {
    toml::table t;
    try {
        t = toml::parse_file(percorso);
    } catch (const toml::parse_error& e) {
        throw std::runtime_error(percorso + ": " + std::string(e.description()));
    }
    CatalogoMissili out;
    const auto numero = [&](const toml::table& tab, const std::string& id, const char* campo) {
        const auto v = tab[campo].value<double>();
        if (!v) throw std::runtime_error(percorso + ": missile '" + id + "' senza " + campo);
        return *v;
    };
    const auto testo = [&](const toml::table& tab, const std::string& id, const char* campo) {
        const auto v = tab[campo].value<std::string>();
        if (!v) throw std::runtime_error(percorso + ": missile '" + id + "' senza " + campo);
        return *v;
    };
    const auto* missili = t["missili"].as_table();
    if (!missili || missili->empty()) throw std::runtime_error(percorso + ": nessun missile");
    for (auto&& [chiave, nodo] : *missili) {
        const auto* tab = nodo.as_table();
        if (!tab) continue;
        DatiMissile d;
        d.id = std::string(chiave.str());
        d.nome = testo(*tab, d.id, "nome");
        d.fonte = testo(*tab, d.id, "fonte");
        d.confidenza = testo(*tab, d.id, "confidenza");
        d.nota = (*tab)["nota"].value_or(std::string());
        d.ruolo = ruolo_da(testo(*tab, d.id, "ruolo"), d.id);
        d.uscita = uscita_da(testo(*tab, d.id, "uscita"), d.id);
        d.velocita_m_s = numero(*tab, d.id, "velocita_m_s");
        d.quota_crociera_m = numero(*tab, d.id, "quota_crociera_m");
        d.portata_m = numero(*tab, d.id, "portata_m");
        d.distanza_min_m = numero(*tab, d.id, "distanza_min_m");
        d.quota_lancio_max_m = numero(*tab, d.id, "quota_lancio_max_m");
        d.velocita_lancio_max_kn = numero(*tab, d.id, "velocita_lancio_max_kn");
        d.quota_lancio_min_m = numero(*tab, d.id, "quota_lancio_min_m");
        d.salita_s = numero(*tab, d.id, "salita_s");
        d.boost_s = numero(*tab, d.id, "boost_s");
        d.discesa_s = numero(*tab, d.id, "discesa_s");
        const std::string term = testo(*tab, d.id, "terminale");
        if (term != "radente" && term != "pop_up") throw std::runtime_error(percorso + ": missile '" + d.id + "' terminale '" + term + "'");
        d.terminale = term == "pop_up" ? Terminale::pop_up : Terminale::radente;
        d.pop_up_m = numero(*tab, d.id, "pop_up_m");
        const auto* acq = (*tab)["acquisizione_m"].as_array();
        if (!acq || acq->size() != 3) throw std::runtime_error(percorso + ": missile '" + d.id + "' senza acquisizione_m[3]");
        for (std::size_t k = 0; k < 3; ++k) d.acquisizione_m[k] = (*acq)[k].value_or(0.0);
        d.semiapertura_gradi = numero(*tab, d.id, "semiapertura_gradi");
        d.mare_max = static_cast<int>(numero(*tab, d.id, "mare_max"));
        d.p_acquisizione_scorta = numero(*tab, d.id, "p_acquisizione_scorta");
        const auto esm = (*tab)["esm"].value<bool>();
        if (!esm) throw std::runtime_error(percorso + ": missile '" + d.id + "' senza esm");
        d.esm = *esm;
        if (const auto* er = (*tab)["esm_radar"].as_array())
            for (const auto& r : *er) d.esm_radar.push_back(r.value_or(std::string()));
        d.radar_seeker = (*tab)["radar_seeker"].value_or(std::string());
        d.quota_ricerca_m = numero(*tab, d.id, "quota_ricerca_m");
        d.testata_kg = numero(*tab, d.id, "testata_kg");
        d.esplosivo_kg = (*tab)["esplosivo_kg"].value_or(0.0);
        d.affidabilita = numero(*tab, d.id, "affidabilita");
        d.accensione_s = numero(*tab, d.id, "accensione_s");
        d.allineamento_s = numero(*tab, d.id, "allineamento_s");
        d.preparazione_primo_s = numero(*tab, d.id, "preparazione_primo_s");
        d.preparazione_altri_s = numero(*tab, d.id, "preparazione_altri_s");
        d.anno_dal = static_cast<int>(numero(*tab, d.id, "anno_dal"));
        d.anno_al = static_cast<int>(numero(*tab, d.id, "anno_al"));
        out.missili.emplace(d.id, std::move(d));
    }
    if (const auto* armi = t["arma"].as_array())
        for (const auto& n : *armi) {
            const auto* a = n.as_table();
            if (!a) continue;
            CatalogoMissili::Arma x;
            x.arma = (*a)["arma"].value_or(std::string());
            x.missile = (*a)["missile"].value_or(std::string());
            x.dal = (*a)["dal"].value_or(0);
            x.al = (*a)["al"].value_or(9999);
            if (!out.missile(x.missile)) throw std::runtime_error(percorso + ": arma '" + x.arma + "' con un missile che non c'e'");
            out.armi.push_back(x);
        }
    if (const auto* lanciatori = t["lanciatore"].as_array())
        for (const auto& n : *lanciatori) {
            const auto* a = n.as_table();
            if (!a) continue;
            CatalogoMissili::Lanciatore x;
            x.classe = (*a)["classe"].value_or(std::string());
            x.missile = (*a)["missile"].value_or(std::string());
            x.dal = (*a)["dal"].value_or(0);
            x.al = (*a)["al"].value_or(9999);
            x.salva = (*a)["salva"].value_or(0);
            x.riserva = (*a)["riserva"].value_or(0);
            if (!out.missile(x.missile)) throw std::runtime_error(percorso + ": lanciatore '" + x.classe + "' con un missile che non c'e'");
            out.lanciatori.push_back(x);
        }
    if (const auto* difese = t["difesa_nave"].as_array())
        for (const auto& n : *difese) {
            const auto* a = n.as_table();
            if (!a) continue;
            DifesaClasse x;
            x.classe = (*a)["classe"].value_or(std::string());
            x.phalanx = (*a)["phalanx"].value_or(0);
            x.canali = (*a)["canali"].value_or(0);
            x.block1_dal = (*a)["block1_dal"].value_or(0);
            x.fonte = (*a)["fonte"].value_or(std::string());
            x.confidenza = (*a)["confidenza"].value_or(std::string());
            if (x.canali < 1) throw std::runtime_error(percorso + ": difesa_nave '" + x.classe + "' senza canali");
            out.difese.emplace(x.classe, std::move(x));
        }
    return out;
}

// --- il bersaglio stimato ---

EllisseErrore ellisse_a(const BersaglioStimato& b, double t) {
    const double eta = std::max(0.0, t - b.t_pos);
    Cov c = cov_di(b.ellisse);
    if (eta > 0.0) {
        if (b.moto_noto) {
            c = piu(c, lungo(b.rotta, b.sigma_velocita_m_s * eta));
            c = piu(c, lungo(b.rotta + 90.0, b.velocita_m_s * eta * radianti(b.sigma_rotta_deg)));
        } else {
            const double s = b.velocita_max_m_s * eta / 2.0;   // il cerchio dei 2 sigma cresce come velocita_max * eta'
            c = piu(c, {s * s, 0.0, s * s});
        }
    }
    return ellisse_di(c);
}

Vec2 posizione_a(const BersaglioStimato& b, double t) {
    if (!b.moto_noto) return b.pos;
    return avanti(b.pos, b.rotta, b.velocita_m_s * (t - b.t_pos));
}

EllisseErrore somma(const EllisseErrore& a, const EllisseErrore& b) { return ellisse_di(piu(cov_di(a), cov_di(b))); }

BersaglioStimato da_rapporto(int contatto, Vec2 pos, double t_fix, double raggio_m, std::optional<double> rotta,
                             std::optional<double> velocita_m_s, double velocita_max_m_s) {
    BersaglioStimato b;
    b.fonte = FonteDati::radio;
    b.contatto = contatto;
    b.t_pos = t_fix;
    b.pos = pos;
    b.ellisse = {raggio_m, raggio_m, 0.0};
    b.moto_noto = rotta.has_value() && velocita_m_s.has_value();
    if (b.moto_noto) {
        b.rotta = *rotta;
        b.velocita_m_s = *velocita_m_s;
    }
    b.velocita_max_m_s = velocita_max_m_s;
    b.relativo = false;
    return b;
}

// --- avvisi ---

bool bloccante(Avviso a) {
    switch (a) {
        case Avviso::fuori_portata:
        case Avviso::troppo_vicino:
        case Avviso::quota:
        case Avviso::solo_bol:
        case Avviso::senza_bersaglio: return true;
        default: return false;
    }
}

const char* nome_avviso(Avviso a) {
    switch (a) {
        case Avviso::fuori_portata: return "BERSAGLIO FUORI PORTATA";
        case Avviso::troppo_vicino: return "BERSAGLIO TROPPO VICINO";
        case Avviso::quota: return "QUOTA DI LANCIO TROPPO PROFONDA";
        case Avviso::solo_bol: return "DISTANZA IGNOTA: SOLO BOL";
        case Avviso::senza_bersaglio: return "NESSUN BERSAGLIO";
        case Avviso::velocita: return "VELOCITA' DI LANCIO ALTA";
        case Avviso::quota_bassa: return "QUOTA DI LANCIO TROPPO BASSA";
        case Avviso::mare: return "MARE OLTRE IL LIMITE DEL SEEKER";
        case Avviso::dato_vecchio: return "DATO DEL BERSAGLIO VECCHIO";
        case Avviso::neutrali_in_area: return "NEUTRALI NELL'AREA DI RICERCA";
        case Avviso::rotta_ignota: return "ROTTA DEL BERSAGLIO IGNOTA";
        case Avviso::amici_in_area: return "AMICI NELL'AREA DI RICERCA";
    }
    return "?";
}

const char* nome_fase(FasePreparazione f) {
    switch (f) {
        case FasePreparazione::spento: return "spento";
        case FasePreparazione::accensione: return "accensione";
        case FasePreparazione::allineamento: return "allineamento";
        case FasePreparazione::pronto: return "pronto";
    }
    return "?";
}

const char* nome_fase(FaseMissile f) {
    switch (f) {
        case FaseMissile::salita: return "salita";
        case FaseMissile::boost: return "boost";
        case FaseMissile::crociera: return "crociera";
        case FaseMissile::ricerca: return "ricerca";
        case FaseMissile::inseguimento: return "inseguimento";
        case FaseMissile::terminale: return "terminale";
        case FaseMissile::impatto: return "impatto";
        case FaseMissile::esaurito: return "esaurito";
        case FaseMissile::guasto: return "guasto";
    }
    return "?";
}

const char* nome_validita(ValiditaTlam v) {
    switch (v) {
        case ValiditaTlam::valida: return "VALIDA";
        case ValiditaTlam::ripianifica: return "RIPIANIFICA";
        case ValiditaTlam::non_valida: return "NON VALIDA";
        case ValiditaTlam::fuori_finestra: return "FUORI FINESTRA";
    }
    return "?";
}

int dimensione_da_lunghezza(double lunghezza_m) { return lunghezza_m < 90.0 ? 0 : lunghezza_m <= 180.0 ? 1 : 2; }

// --- il piano ---

namespace {

// la serpentina del TASM dal punto di salita: gambe trasversali alla rotta, che avanzano finche' c'e' carburante
std::vector<Vec2> serpentina(Vec2 inizio, double rotta, double budget_m) {
    std::vector<Vec2> p{inizio};
    Vec2 c = inizio;
    double usato = 0.0, lato = 1.0;
    const auto vai = [&](Vec2 q) {
        usato += distanza(c, q);
        c = q;
        p.push_back(q);
    };
    vai(avanti(c, rotta + 90.0 * lato, SERPENTINA_GAMBA_M / 2.0));
    while (usato < budget_m && p.size() < 64) {
        vai(avanti(c, rotta, SERPENTINA_PASSO_M));
        lato = -lato;
        vai(avanti(c, rotta + 90.0 * lato, SERPENTINA_GAMBA_M));
    }
    return p;
}

}  // namespace

PianoTiro pianifica(const DatiMissile& m, const ImpostazioniMissile& imp, const BersaglioStimato& b,
                    const StatoLanciatore& l, const std::vector<ContattoNoto>& contatti, double t) {
    PianoTiro p;
    p.t = t;
    p.contatto = b.contatto;
    p.lancio = l.pos;
    const auto avvisa = [&](Avviso a) {
        if (std::find(p.avvisi.begin(), p.avvisi.end(), a) == p.avvisi.end()) p.avvisi.push_back(a);
    };
    if (b.contatto < 0) avvisa(Avviso::senza_bersaglio);
    if (imp.modo == ModoLancio::rbl && !b.distanza_nota) avvisa(Avviso::solo_bol);
    if (l.quota_m > m.quota_lancio_max_m) avvisa(Avviso::quota);
    if (l.velocita_kn > m.velocita_lancio_max_kn) avvisa(Avviso::velocita);
    if (l.quota_m < m.quota_lancio_min_m) avvisa(Avviso::quota_bassa);
    if (l.stato_mare > m.mare_max) avvisa(Avviso::mare);
    if (b.fonte == FonteDati::radio && !b.moto_noto) avvisa(Avviso::rotta_ignota);

    const double v = std::max(1.0, m.velocita_m_s);
    // il punto previsto: la stima del bersaglio all'arrivo del missile (qualche iterazione sul tempo di volo)
    p.previsto = posizione_a(b, t);
    for (int k = 0; k < 6; ++k) {
        const double d = distanza(p.lancio, p.previsto);
        p.previsto = posizione_a(b, t + m.salita_s + d / v);
    }
    p.distanza_m = distanza(p.lancio, p.previsto);
    p.tempo_volo_s = m.salita_s + p.distanza_m / v;
    p.rotta = (imp.modo == ModoLancio::bol && !b.distanza_nota) ? b.rilevamento : rilevamento(p.lancio, p.previsto);
    p.ellisse_arrivo = ellisse_a(b, t + p.tempo_volo_s);
    if (!b.relativo) p.ellisse_arrivo = somma(p.ellisse_arrivo, l.navigazione);
    const Cov c = cov_di(p.ellisse_arrivo);
    const double sr = sigma_lungo(c, p.rotta), sc = sigma_lungo(c, p.rotta + 90.0);
    const std::size_t tg = static_cast<std::size_t>(imp.taglia);

    if (m.esm && m.quota_ricerca_m > 0.0) {
        // TASM: sale e cerca a serpentina dal punto di salita fino al carburante
        p.area.forma = AreaRicerca::Forma::serpentina;
        p.accensione_m = std::max(m.distanza_min_m, p.distanza_m - SERPENTINA_SALITA_M);
        p.accensione = avanti(p.lancio, p.rotta, p.accensione_m);
        p.fine_m = m.portata_m;
        p.area.percorso = serpentina(p.accensione, p.rotta, m.portata_m - p.accensione_m);
        double fondo = 0.0;
        for (const Vec2& q : p.area.percorso) {
            double s, w;
            lungo_attraverso(q, p.accensione, p.rotta, s, w);
            fondo = std::max(fondo, s);
        }
        const double mezza = SERPENTINA_GAMBA_M / 2.0 + m.acquisizione_m[static_cast<std::size_t>(std::clamp(b.dimensione, 0, 2))] / 2.0;
        const Vec2 a0 = avanti(p.accensione, p.rotta, -SERPENTINA_PASSO_M / 2.0), a1 = avanti(p.accensione, p.rotta, fondo + SERPENTINA_PASSO_M / 2.0);
        p.area.contorno = {avanti(a0, p.rotta - 90.0, mezza), avanti(a1, p.rotta - 90.0, mezza), avanti(a1, p.rotta + 90.0, mezza),
                           avanti(a0, p.rotta + 90.0, mezza)};
    } else if (imp.modo == ModoLancio::rbl) {
        p.area.forma = AreaRicerca::Forma::scatola;
        const double mp = std::max(SCATOLA_MIN_M[tg], SCATOLA_SIGMA[tg] * sr);
        const double ml = std::max(SCATOLA_LARGA_MIN_M[tg], SCATOLA_SIGMA[tg] * sc);
        p.accensione_m = std::max(0.0, p.distanza_m - mp);
        p.accensione = avanti(p.lancio, p.rotta, p.accensione_m);
        p.fine_m = std::min(m.portata_m, p.distanza_m + mp);
        const Vec2 a0 = avanti(p.previsto, p.rotta, -mp), a1 = avanti(p.previsto, p.rotta, mp);
        p.area.contorno = {avanti(a0, p.rotta - 90.0, ml), avanti(a1, p.rotta - 90.0, ml), avanti(a1, p.rotta + 90.0, ml),
                           avanti(a0, p.rotta + 90.0, ml)};
    } else {
        // BOL: il ventaglio che si allarga dal punto d'accensione
        p.area.forma = AreaRicerca::Forma::ventaglio;
        const double srch = imp.srch_m > 0.0 ? imp.srch_m
                            : b.distanza_nota ? std::max(m.distanza_min_m, p.distanza_m - 10000.0)
                                              : m.distanza_min_m;
        p.accensione_m = srch;
        p.accensione = avanti(p.lancio, p.rotta, srch);
        p.fine_m = std::min(m.portata_m, imp.dstr_m > 0.0 ? imp.dstr_m : m.portata_m);
        // il bordo: alla distanza r dal punto d'accensione, la mezza larghezza piu' grande fra i cancelli che la raggiungono
        const double lungh = std::max(0.0, p.fine_m - srch) + VENTAGLIO_MAX_M;
        constexpr int N = 40;
        std::vector<Vec2> destra, sinistra;
        for (int i = 0; i <= N; ++i) {
            const double r = lungh * i / N;
            double w = 0.0;
            for (int j = 0; j <= N; ++j) {
                const double sm = std::min(r, std::max(0.0, p.fine_m - srch)) * j / N;   // dove era il missile
                const double tt = sm / v;
                if (r - sm <= profondita_bol(tt)) w = std::max(w, (r - sm) * std::tan(radianti(semiapertura_bol(tt))));
            }
            destra.push_back(avanti(avanti(p.accensione, p.rotta, r), p.rotta + 90.0, w));
            sinistra.push_back(avanti(avanti(p.accensione, p.rotta, r), p.rotta - 90.0, w));
        }
        p.area.contorno = destra;
        p.area.contorno.insert(p.area.contorno.end(), sinistra.rbegin(), sinistra.rend());
    }

    if (p.distanza_m > m.portata_m && !(imp.modo == ModoLancio::bol && !b.distanza_nota)) avvisa(Avviso::fuori_portata);
    if (p.distanza_m < m.distanza_min_m && b.distanza_nota) avvisa(Avviso::troppo_vicino);
    p.eta_dato_s = t + m.salita_s + p.accensione_m / v - b.t_pos;
    if (p.eta_dato_s > DATO_VECCHIO_S) avvisa(Avviso::dato_vecchio);
    p.orizzonte_radar_m = orizzonte_m(ALTEZZA_BERSAGLIO_M, std::max(m.quota_crociera_m, m.quota_ricerca_m));

    // i contatti noti nell'area, nell'ordine in cui il seeker li vedrebbe (lungo la rotta dal punto d'accensione)
    struct Visto { int id; double s; int dim; bool neutrale, amico; };
    std::vector<Visto> visti;
    for (const ContattoNoto& k : contatti) {
        const Vec2 q = k.pos;
        if (!dentro_poligono(q, p.area.contorno)) continue;
        double s, w;
        lungo_attraverso(q, p.accensione, p.rotta, s, w);
        visti.push_back({k.id, p.area.forma == AreaRicerca::Forma::serpentina ? distanza(q, p.accensione) : s, k.dimensione,
                         k.neutrale_creduto, k.amico_creduto});
    }
    std::sort(visti.begin(), visti.end(), [](const Visto& a, const Visto& c2) { return a.s < c2.s || (a.s == c2.s && a.id < c2.id); });
    for (const Visto& k : visti) {
        p.in_area.push_back(k.id);
        if (k.neutrale) {
            p.neutrali_in_area.push_back(k.id);
            avvisa(Avviso::neutrali_in_area);
        }
        if (k.amico && k.id != b.contatto) {
            p.amici_in_area.push_back(k.id);
            avvisa(Avviso::amici_in_area);
        }
    }

    // le probabilita': il bersaglio nell'area, poi primo eco preso fra i contatti noti davanti a lui
    p.p_area = p_nel_poligono(p.previsto, p.ellisse_arrivo, p.area.contorno);
    double s_bersaglio, w_b;
    lungo_attraverso(p.previsto, p.accensione, p.rotta, s_bersaglio, w_b);
    if (p.area.forma == AreaRicerca::Forma::serpentina) s_bersaglio = distanza(p.previsto, p.accensione);
    int massima = std::clamp(b.dimensione, 0, 2);
    for (const Visto& k : visti)
        if (k.id != b.contatto) massima = std::max(massima, k.dim);
    double primo = 1.0;
    for (const Visto& k : visti) {
        if (k.id == b.contatto || k.s >= s_bersaglio) continue;
        primo *= 1.0 - (k.dim >= massima ? (k.dim > b.dimensione ? P_ACQUISIZIONE_GRANDE : 1.0) : m.p_acquisizione_scorta);
    }
    primo *= b.dimensione >= massima ? 1.0 : m.p_acquisizione_scorta;
    p.p_primo = primo;

    std::stable_sort(p.avvisi.begin(), p.avvisi.end(), [](Avviso a, Avviso c2) { return bloccante(a) && !bloccante(c2); });
    p.valido = std::none_of(p.avvisi.begin(), p.avvisi.end(), bloccante);
    p.p_successo = p.valido ? p.p_area * p.p_primo * m.affidabilita : 0.0;
    return p;
}

// --- preparazione ---

PreparazioneMissile::PreparazioneMissile(const DatiMissile& m, bool primo) : accensione_s_(m.accensione_s), allineamento_s_(m.allineamento_s) {
    if (accensione_s_ + allineamento_s_ <= 0.0 && m.preparazione_primo_s > 0.0) {   // TLAM: il pacchetto della missione
        const double tutto = primo ? m.preparazione_primo_s : m.preparazione_altri_s;
        accensione_s_ = tutto / 3.0;
        allineamento_s_ = tutto - accensione_s_;
    }
}

void PreparazioneMissile::avvia() {
    if (fase_ != FasePreparazione::spento) return;
    fase_ = FasePreparazione::accensione;
    residuo_ = accensione_s_;
    avanza(0.0);
}

void PreparazioneMissile::spegni() {
    fase_ = FasePreparazione::spento;
    residuo_ = 0.0;
}

void PreparazioneMissile::avanza(double dt) {
    double resto = dt;
    while (fase_ == FasePreparazione::accensione || fase_ == FasePreparazione::allineamento) {
        if (residuo_ > resto) {
            residuo_ -= resto;
            return;
        }
        resto -= residuo_;
        if (fase_ == FasePreparazione::accensione) {
            fase_ = FasePreparazione::allineamento;
            residuo_ = allineamento_s_;
        } else {
            fase_ = FasePreparazione::pronto;
            residuo_ = 0.0;
        }
    }
}

double preparazione_tlam_s(const DatiMissile& m, int n_missili) {
    if (n_missili <= 0) return 0.0;
    return m.preparazione_primo_s + (n_missili - 1) * m.preparazione_altri_s;
}

// --- il volo ---

Missile::Missile(const DatiMissile& m, const ImpostazioniMissile& imp, const PianoTiro& piano, Vec2 lancio_vero,
                 double quota_lancio_m, double velocita_lancio_kn, int lanciatore, std::uint64_t seme)
    : m_(m), imp_(imp), piano_(piano), scarto_(meno(lancio_vero, piano.lancio)), pos_(lancio_vero), quota_(quota_lancio_m),
      rotta_(piano.rotta), lanciatore_(lanciatore), rng_(seme) {
    // guasti: in salita oltre i limiti di lancio (dossier 05 §1.3: +5% ogni 10 m oltre la quota, +10% oltre la velocita',
    // +10% sotto la quota minima, dove la capsula affiora vicino alla vela),
    // poi in volo con l'affidabilita' della classe, a un istante a caso della crociera
    double p_salita = 0.0;
    if (quota_lancio_m > m.quota_lancio_max_m) p_salita += 0.05 * std::ceil((quota_lancio_m - m.quota_lancio_max_m) / 10.0);
    if (velocita_lancio_kn > m.velocita_lancio_max_kn) p_salita += 0.10;
    if (quota_lancio_m < m.quota_lancio_min_m) p_salita += 0.10;
    if (rng_.uniforme() < p_salita) {
        guasto_volo_ = true;
        t_guasto_ = m.salita_s * rng_.uniforme();
    } else if (rng_.uniforme() > m.affidabilita) {
        guasto_volo_ = true;
        const double volo = std::max(1.0, piano.fine_m / std::max(1.0, m.velocita_m_s));
        t_guasto_ = m.salita_s + m.boost_s + volo * rng_.uniforme();
    }
    if (m.salita_s <= 0.0) fase_ = FaseMissile::boost;
}

bool Missile::attivo() const {
    return fase_ != FaseMissile::impatto && fase_ != FaseMissile::esaurito && fase_ != FaseMissile::guasto;
}

bool Missile::seeker_acceso() const {
    return !m_.radar_seeker.empty() &&
           (fase_ == FaseMissile::ricerca || fase_ == FaseMissile::inseguimento || fase_ == FaseMissile::terminale);
}

std::vector<EventoMissile> Missile::eventi() {
    std::vector<EventoMissile> e;
    e.swap(eventi_);
    return e;
}

void Missile::evento(EventoMissile::Tipo t, int nave) { eventi_.push_back({t, t_, nave, pos_}); }

bool Missile::nell_area(Vec2 p) const { return dentro_poligono(meno(p, scarto_), piano_.area.contorno); }

void Missile::cerca(const std::vector<EcoNave>& navi, int stato_mare) {
    const double fattore_mare = stato_mare > m_.mare_max ? 0.7 : 1.0;
    const double quota_seeker = std::max(m_.quota_crociera_m, quota_);
    const bool bol = piano_.area.forma == AreaRicerca::Forma::ventaglio;
    const double t_ricerca = std::max(0.0, (percorso_ - piano_.accensione_m) / std::max(1.0, m_.velocita_m_s));
    // TASM: l'ESM porta il missile sul radar del bersaglio voluto, se uno e' acceso a portata
    std::optional<double> su_esm;
    if (m_.esm && !m_.esm_radar.empty()) {
        double migliore = 1e18;
        for (const EcoNave& n : navi) {
            const double d = distanza(pos_, n.pos);
            if (d > 2.0 * m_.acquisizione_m[2] || d > 1.2 * orizzonte_m(quota_seeker, n.altezza_m)) continue;
            const bool emette = std::any_of(n.radar_accesi.begin(), n.radar_accesi.end(), [&](const std::string& r) {
                return std::find(m_.esm_radar.begin(), m_.esm_radar.end(), r) != m_.esm_radar.end();
            });
            if (emette && d < migliore) {
                migliore = d;
                su_esm = rilevamento(pos_, n.pos);
            }
        }
        if (su_esm) rotta_ = *su_esm;
    }
    struct Candidato { const EcoNave* n; double d; };
    std::vector<Candidato> visti;
    for (const EcoNave& n : navi) {
        if (std::find(scartati_.begin(), scartati_.end(), n.id) != scartati_.end()) continue;
        const double d = distanza(pos_, n.pos);
        const std::size_t dim = static_cast<std::size_t>(std::clamp(n.dimensione, 0, 2));
        if (d > m_.acquisizione_m[dim] * fattore_mare) continue;
        if (d > orizzonte_m(quota_seeker, n.altezza_m)) continue;
        const double ril = rilevamento(pos_, n.pos), scarto_ril = std::abs(normalizza_180(ril - rotta_));
        if (su_esm) {
            if (std::abs(normalizza_180(ril - *su_esm)) > 5.0) continue;
        } else if (bol) {
            if (scarto_ril > semiapertura_bol(t_ricerca) || d > profondita_bol(t_ricerca)) continue;
        } else {
            if (scarto_ril > m_.semiapertura_gradi || !nell_area(n.pos)) continue;
        }
        visti.push_back({&n, d});
    }
    if (visti.empty()) return;
    std::sort(visti.begin(), visti.end(), [](const Candidato& a, const Candidato& b) { return a.d < b.d || (a.d == b.d && a.n->id < b.n->id); });
    int massima = 0;
    for (const Candidato& c : visti) massima = std::max(massima, c.n->dimensione);
    for (const Candidato& c : visti) {
        const bool preso = c.n->dimensione >= massima || rng_.uniforme() < m_.p_acquisizione_scorta;
        if (preso) {
            bersaglio_ = c.n->id;
            fase_ = FaseMissile::inseguimento;
            evento(EventoMissile::Tipo::aggancio, bersaglio_);
            return;
        }
        scartati_.push_back(c.n->id);
    }
}

void Missile::insegui(double dt, const std::vector<EcoNave>& navi, int stato_mare) {
    const EcoNave* b = nullptr;
    for (const EcoNave& n : navi)
        if (n.id == bersaglio_) b = &n;
    const double fattore_mare = stato_mare > m_.mare_max ? 0.7 : 1.0;
    if (!b || distanza(pos_, b->pos) > m_.acquisizione_m[static_cast<std::size_t>(std::clamp(b->dimensione, 0, 2))] * fattore_mare * 1.2) {
        evento(EventoMissile::Tipo::perdita, bersaglio_);
        scartati_.push_back(bersaglio_);
        bersaglio_ = -1;
        fase_ = FaseMissile::ricerca;
        return;
    }
    const double d = distanza(pos_, b->pos);
    rotta_ = rilevamento(pos_, b->pos);
    if (fase_ == FaseMissile::inseguimento && d < 2000.0) {
        fase_ = FaseMissile::terminale;
        evento(EventoMissile::Tipo::terminale, bersaglio_);
    }
    if (fase_ == FaseMissile::terminale && m_.terminale == Terminale::pop_up) quota_ = std::min(m_.pop_up_m, d * 0.5);
    else quota_ = m_.quota_crociera_m;
    const double passo = m_.velocita_m_s * dt;
    if (passo >= d) {
        pos_ = b->pos;
        percorso_ += d;
        fase_ = FaseMissile::impatto;
        evento(EventoMissile::Tipo::impatto, bersaglio_);
        return;
    }
    pos_ = avanti(pos_, rotta_, passo);
    percorso_ += passo;
}

void Missile::avanza(double dt, const std::vector<EcoNave>& navi, int stato_mare) {
    if (!attivo() || dt <= 0.0) return;
    t_ += dt;
    if (guasto_volo_ && t_ >= t_guasto_) {
        fase_ = FaseMissile::guasto;
        evento(EventoMissile::Tipo::guasto);
        return;
    }
    const double v = m_.velocita_m_s;
    switch (fase_) {
        case FaseMissile::salita:
            if (t_ >= m_.salita_s) {
                quota_ = 0.0;
                fase_ = FaseMissile::boost;
                evento(EventoMissile::Tipo::affioramento);
                evento(EventoMissile::Tipo::boost);
            }
            return;
        case FaseMissile::boost: {
            const double passo = v * 0.5 * dt;
            pos_ = avanti(pos_, rotta_, passo);
            percorso_ += passo;
            quota_ = std::max(quota_, 50.0);
            if (t_ >= m_.salita_s + m_.boost_s) {
                fase_ = FaseMissile::crociera;
                quota_ = m_.quota_crociera_m;
                evento(EventoMissile::Tipo::crociera);
            }
            return;
        }
        case FaseMissile::crociera: {
            const double passo = v * dt;
            pos_ = avanti(pos_, rotta_, passo);
            percorso_ += passo;
            if (percorso_ >= piano_.accensione_m) {
                fase_ = FaseMissile::ricerca;
                if (m_.quota_ricerca_m > 0.0) quota_ = m_.quota_ricerca_m;
                evento(EventoMissile::Tipo::seeker_acceso);
                cerca(navi, stato_mare);
            }
            return;
        }
        case FaseMissile::ricerca: {
            if (piano_.area.forma == AreaRicerca::Forma::serpentina && gamba_ + 1 < piano_.area.percorso.size()) {
                const Vec2 meta = piu(piano_.area.percorso[gamba_ + 1], scarto_);
                if (distanza(pos_, meta) < v * dt + 1.0) ++gamba_;
                else rotta_ = rilevamento(pos_, meta);
            }
            const double passo = v * dt;
            pos_ = avanti(pos_, rotta_, passo);
            percorso_ += passo;
            cerca(navi, stato_mare);
            if (fase_ == FaseMissile::ricerca && (percorso_ >= piano_.fine_m || percorso_ >= m_.portata_m)) {
                fase_ = FaseMissile::esaurito;
                evento(EventoMissile::Tipo::esaurito);
            }
            return;
        }
        case FaseMissile::inseguimento:
        case FaseMissile::terminale:
            insegui(dt, navi, stato_mare);
            if (attivo() && percorso_ >= m_.portata_m) {
                fase_ = FaseMissile::esaurito;
                evento(EventoMissile::Tipo::esaurito);
            }
            return;
        default: return;
    }
}

StimaVolo stima_volo(const PianoTiro& p, const ImpostazioniMissile& imp, const DatiMissile& m, double t_dal_lancio) {
    (void)imp;
    StimaVolo s;
    const double v = m.velocita_m_s;
    if (t_dal_lancio < m.salita_s) {
        s.pos = p.lancio;
        s.fase = FaseMissile::salita;
        return s;
    }
    double d;
    if (t_dal_lancio < m.salita_s + m.boost_s) {
        d = v * 0.5 * (t_dal_lancio - m.salita_s);
        s.fase = FaseMissile::boost;
    } else {
        d = v * 0.5 * m.boost_s + v * (t_dal_lancio - m.salita_s - m.boost_s);
        s.fase = d >= p.accensione_m ? FaseMissile::ricerca : FaseMissile::crociera;
    }
    if (d >= p.fine_m) {
        d = p.fine_m;
        s.fase = FaseMissile::esaurito;
    }
    if (p.area.forma == AreaRicerca::Forma::serpentina && d > p.accensione_m && !p.area.percorso.empty()) {
        double resto = d - p.accensione_m;
        Vec2 c = p.area.percorso.front();
        for (std::size_t i = 1; i < p.area.percorso.size(); ++i) {
            const double l = distanza(c, p.area.percorso[i]);
            if (resto <= l) {
                s.pos = avanti(c, rilevamento(c, p.area.percorso[i]), resto);
                return s;
            }
            resto -= l;
            c = p.area.percorso[i];
        }
        s.pos = c;
        return s;
    }
    s.pos = avanti(p.lancio, p.rotta, d);
    return s;
}

// --- TLAM ---

StatoTlam valuta_tlam(const DatiMissile& m, const MissioneTlam& t, Vec2 pos, double sigma_nav_m, double t_ora) {
    StatoTlam s;
    s.distanza_plp_m = distanza(pos, t.plp);
    const double mare = distanza(pos, t.landfall);
    const double v = std::max(1.0, m.velocita_m_s);
    s.tempo_volo_s = m.salita_s + m.boost_s + (mare + t.tratta_terra_m) / v;
    s.ora_lancio_s = t.tot_s - s.tempo_volo_s;
    // errore al landfall: posizione del battello, deriva dell'inerziale del missile sul mare (0,5 nm/h) e rotta (0,1°)
    const double t_mare = mare / v;
    const double deriva = 0.5 * MIGLIO / 3600.0 * t_mare, rotta = mare * radianti(0.1);
    const double sigma = std::sqrt(sigma_nav_m * sigma_nav_m + deriva * deriva + rotta * rotta);
    s.p_landfall = sigma > 0.0 ? 1.0 - std::exp(-t.mezza_mappa_m * t.mezza_mappa_m / (2.0 * sigma * sigma)) : 1.0;
    if (s.distanza_plp_m > TLAM_RIPIANIFICA_M || mare + t.tratta_terra_m > m.portata_m) s.validita = ValiditaTlam::non_valida;
    else if (std::abs(t_ora + (s.distanza_plp_m > TLAM_VALIDA_M ? TLAM_RIPIANIFICA_S : 0.0) - s.ora_lancio_s) > t.finestra_s)
        s.validita = ValiditaTlam::fuori_finestra;
    else if (s.distanza_plp_m > TLAM_VALIDA_M) {
        s.validita = ValiditaTlam::ripianifica;
        s.ripianifica_s = TLAM_RIPIANIFICA_S;
    } else s.validita = ValiditaTlam::valida;
    return s;
}

EsitoTlam esito_tlam(const DatiMissile& m, const MissioneTlam& t, const StatoTlam& vero, double t_lancio,
                     std::uint64_t seme) {
    Casuale rng(seme);
    EsitoTlam e;
    e.t_arrivo = t_lancio + vero.tempo_volo_s;
    const bool parte = vero.validita != ValiditaTlam::non_valida;
    const bool vola = rng.uniforme() < m.affidabilita;
    const bool aggancia = rng.uniforme() < vero.p_landfall;
    e.arrivato = parte && vola && aggancia;
    if (!e.arrivato) {
        // perso: sul mare se non vola, oltre il landfall in inerziale se non aggancia la prima mappa
        const double r = rilevamento(t.plp, t.landfall);
        if (!vola) e.caduta = avanti(t.plp, r, distanza(t.plp, t.landfall) * rng.uniforme());
        else e.caduta = avanti(avanti(t.landfall, r, t.tratta_terra_m * rng.uniforme()), r + 90.0, (rng.uniforme() - 0.5) * 4.0 * t.mezza_mappa_m);
    }
    return e;
}

}  // namespace m688
