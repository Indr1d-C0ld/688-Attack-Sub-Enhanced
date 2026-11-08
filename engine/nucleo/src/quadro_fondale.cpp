#include "nucleo/quadro_fondale.hpp"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <istream>
#include <limits>
#include <ostream>
#include <stdexcept>

namespace m688 {

namespace {
constexpr char MAGICO[8] = {'M', '6', '8', '8', 'Q', 'F', 'O', 'N'};
constexpr std::uint32_t VERSIONE = 1;
constexpr double SONDAGGI_RECENTI_S = 1800.0;   // gli avvisi dei sondaggi: al piu' di mezz'ora fa
constexpr double RITORNI_RECENTI_S = 30.0;      // e dei ritorni del BQS-15: dell'ultima mezza dozzina di spazzate
constexpr double SALITA_MIN_M = 20.0, SALITA_MIN_REL = 0.05;
constexpr double SETTORE_PRUA_DEG = 30.0;
constexpr double MARGINE_OSTACOLO_M = 30.0;     // un eco piu' alto della chiglia meno questo e' un ostacolo

template <typename T>
void scrivi(std::ostream& o, const T& x) {
    o.write(reinterpret_cast<const char*>(&x), sizeof(T));
}
template <typename T>
T leggi_bin(std::istream& in) {
    T x{};
    in.read(reinterpret_cast<char*>(&x), sizeof(T));
    if (!in) throw std::runtime_error("quadro_fondale: salvataggio troncato");
    return x;
}
}  // namespace

QuadroFondale::QuadroFondale(const Fondale& carta, ParametriQuadro p) : carta_(&carta), p_(p) {}

int QuadroFondale::larghezza() const { return carta_->larghezza(); }
int QuadroFondale::altezza() const { return carta_->altezza(); }
double QuadroFondale::cella_m() const { return carta_->cella_m(); }
bool QuadroFondale::cella(Vec2 p, int& col, int& riga) const { return carta_->cella(p, col, riga); }
Vec2 QuadroFondale::centro_cella(int col, int riga) const { return carta_->centro_cella(col, riga); }

double QuadroFondale::sigma_carta(int col, int riga) const {
    const double d = std::max(1.0, carta_->profondita_cella(col, riga));
    if (!carta_->carta_epoca()) return 0.01 * d;   // il fondale vero (navigazione perfetta): la carta e' giusta
    const int k = carta_->campagna_cella(col, riga);
    if (k < 0) return p_.sigma_non_rilevato_rel * d;
    // il rilievo che cade fra due linee: la pendenza locale per mezza spaziatura
    const auto prof = [&](int c, int r) {
        c = std::clamp(c, 0, larghezza() - 1);
        r = std::clamp(r, 0, altezza() - 1);
        return carta_->profondita_cella(c, r);
    };
    const double pendenza = std::max({std::fabs(prof(col + 1, riga) - prof(col - 1, riga)),
                                      std::fabs(prof(col, riga + 1) - prof(col, riga - 1))}) / (2.0 * cella_m());
    const Campagna& c = carta_->campagne()[static_cast<std::size_t>(k)];
    const double spaziatura = d > 1000.0 && c.spaziatura_bacini_m > 0.0 ? c.spaziatura_bacini_m : c.spaziatura_m;
    return p_.sigma_rilevato_rel * d + p_.sigma_spaziatura * pendenza * spaziatura;
}

void QuadroFondale::fondi(Vec2 p, double profondita, double var, double t, FonteFondo f, bool ostacolo) {
    int c = 0, r = 0;
    if (!cella(p, c, r)) return;
    Misurata& m = celle_[static_cast<std::int64_t>(r) * larghezza() + c];
    if (m.var > 0.0) {
        const double k = m.var / (m.var + var);
        m.z += k * (profondita - m.z);
        m.var *= 1.0 - k;
    } else {
        m.z = profondita;
        m.var = std::max(var, 1.0);
    }
    if (t >= m.t) {
        m.t = t;
        m.fonte = f;
    }
    m.ostacolo = m.ostacolo || ostacolo;
}

void QuadroFondale::vincolo(Vec2 p, double almeno) {
    int c = 0, r = 0;
    if (!cella(p, c, r)) return;
    Misurata& m = celle_[static_cast<std::int64_t>(r) * larghezza() + c];
    m.almeno = std::max(m.almeno, almeno);
}

void QuadroFondale::aggiungi(const Sondaggio& s, double sigma_posizione_m) {
    sondaggi_.push_back(s);
    sigma_sondaggi_.push_back(sigma_posizione_m);
    applica(s, sigma_posizione_m);
    ++versione_;
}

void QuadroFondale::aggiungi(const RitornoFondo& r, double sigma_posizione_m) {
    ritorni_.push_back(r);
    sigma_ritorni_.push_back(sigma_posizione_m);
    applica(r, sigma_posizione_m);
    ++versione_;
}

void QuadroFondale::applica(const Sondaggio& s, double sp) {
    if (!s.eco) return;
    fondi(s.pos, s.fondo_m, s.sigma_m * s.sigma_m + 0.0001 * sp * sp, s.t, FonteFondo::ecoscandaglio, false);
    // nel resto dell'impronta il fondo e' almeno a questa profondita'
    const double raggio = s.impronta_m, passo = cella_m();
    for (double dx = -raggio; dx <= raggio; dx += passo)
        for (double dy = -raggio; dy <= raggio; dy += passo)
            if (dx * dx + dy * dy <= raggio * raggio) vincolo({s.pos.x + dx, s.pos.y + dy}, s.fondo_m);
}

void QuadroFondale::applica(const RitornoFondo& r, double sp) {
    if (r.eco) {
        fondi(r.punto, r.profondita_m, r.sigma_m * r.sigma_m + 0.0001 * sp * sp, r.t, FonteFondo::bqs15, r.ostacolo);
        return;
    }
    // un raggio senza eco: lungo il raggio il fondo sta piu' giu' del raggio (con meno fiducia)
    const double b = radianti(r.rilevamento), e = radianti(r.elevazione);
    for (double d = cella_m(); d <= r.distanza_m; d += cella_m()) {
        const Vec2 q{r.origine.x + d * std::cos(e) * std::sin(b), r.origine.y + d * std::cos(e) * std::cos(b)};
        const double prof = r.quota_m - d * std::sin(e);   // elevazione positiva verso l'alto
        if (prof > 0.0) vincolo(q, prof * (1.0 - p_.sigma_libero_rel));
    }
}

void QuadroFondale::punto_nave(double t, Vec2 correzione) {
    const double t0 = ultimo_punto_nave_, durata = t - t0;
    const auto frazione = [&](double tm) { return durata > 0.0 ? std::clamp((tm - t0) / durata, 0.0, 1.0) : 1.0; };
    for (Sondaggio& s : sondaggi_)
        if (s.t > t0 && s.t <= t) {
            const double f = frazione(s.t);
            s.pos = {s.pos.x + correzione.x * f, s.pos.y + correzione.y * f};
        }
    for (RitornoFondo& r : ritorni_)
        if (r.t > t0 && r.t <= t) {
            const double f = frazione(r.t);
            r.origine = {r.origine.x + correzione.x * f, r.origine.y + correzione.y * f};
            r.punto = {r.punto.x + correzione.x * f, r.punto.y + correzione.y * f};
        }
    ultimo_punto_nave_ = t;
    rifai();
}

void QuadroFondale::rifai() {
    celle_.clear();
    // in ordine di tempo, sondaggi e ritorni insieme (la fusione dipende poco dall'ordine; l'ultima fonte si')
    std::size_t i = 0, j = 0;
    while (i < sondaggi_.size() || j < ritorni_.size()) {
        if (j >= ritorni_.size() || (i < sondaggi_.size() && sondaggi_[i].t <= ritorni_[j].t)) {
            applica(sondaggi_[i], sigma_sondaggi_[i]);
            ++i;
        } else {
            applica(ritorni_[j], sigma_ritorni_[j]);
            ++j;
        }
    }
    ++versione_;
}

LetturaCella QuadroFondale::leggi(int col, int riga, double t) const {
    LetturaCella l;
    l.carta_m = carta_->profondita_cella(col, riga);
    l.sigma_carta_m = sigma_carta(col, riga);
    l.profondita_m = l.carta_m;
    l.sigma_m = l.sigma_carta_m;
    const auto it = celle_.find(static_cast<std::int64_t>(riga) * larghezza() + col);
    if (it == celle_.end()) return l;
    const Misurata& m = it->second;
    l.almeno_m = m.almeno;
    l.ostacolo = m.ostacolo;
    if (m.var > 0.0) {
        const double eta_h = std::max(0.0, t - m.t) / 3600.0;
        const double var = m.var + (p_.invecchiamento_m_h * eta_h) * (p_.invecchiamento_m_h * eta_h);
        const double vc = l.sigma_carta_m * l.sigma_carta_m;
        l.profondita_m = (l.carta_m / vc + m.z / var) / (1.0 / vc + 1.0 / var);
        l.sigma_m = std::sqrt(1.0 / (1.0 / vc + 1.0 / var));
        l.fonte = m.fonte;
        l.eta_s = t - m.t;
        l.da_carta = false;
    }
    if (l.almeno_m > l.profondita_m) l.profondita_m = l.almeno_m;
    return l;
}

LetturaCella QuadroFondale::leggi(Vec2 p, double t) const {
    int c = 0, r = 0;
    if (!cella(p, c, r)) {
        LetturaCella l;
        l.profondita_m = l.carta_m = std::numeric_limits<double>::infinity();
        return l;
    }
    return leggi(c, r, t);
}

std::vector<std::pair<int, int>> QuadroFondale::celle_misurate() const {
    std::vector<std::pair<int, int>> out;
    out.reserve(celle_.size());
    for (const auto& [k, m] : celle_)
        out.emplace_back(static_cast<int>(k % larghezza()), static_cast<int>(k / larghezza()));
    return out;
}

std::vector<PuntoProfilo> QuadroFondale::profilo(Vec2 da, double rotta, double lunghezza_m, double passo_m, double t) const {
    std::vector<PuntoProfilo> out;
    const double b = radianti(rotta);
    for (double d = 0.0; d <= lunghezza_m + 1e-9; d += std::max(1.0, passo_m)) {
        const LetturaCella l = leggi(Vec2{da.x + d * std::sin(b), da.y + d * std::cos(b)}, t);
        out.push_back({d, l.carta_m, l.profondita_m, l.sigma_m, !l.da_carta});
    }
    return out;
}

std::vector<RitornoFondo> QuadroFondale::ritorni_recenti(double t, double finestra_s) const {
    std::vector<RitornoFondo> out;
    for (const RitornoFondo& r : ritorni_)
        if (r.t <= t && t - r.t <= finestra_s) out.push_back(r);
    return out;
}

std::vector<AvvisoFondo> QuadroFondale::avvisi(Vec2 pos_stimata, double quota_m, double rotta, double velocita_kn,
                                               double t, double reazione_s) const {
    std::vector<AvvisoFondo> out;
    // i sondaggi recenti con l'eco
    const Sondaggio* ultimo = nullptr;
    const Sondaggio* prima = nullptr;
    for (const Sondaggio& s : sondaggi_)
        if (s.eco && s.t <= t && t - s.t <= SONDAGGI_RECENTI_S) {
            prima = ultimo;
            ultimo = &s;
        }
    if (ultimo) out.push_back({AvvisoFondo::Tipo::sotto_chiglia, ultimo->fondo_m - quota_m, ultimo->t});
    if (ultimo && prima) {
        const double salita = prima->fondo_m - ultimo->fondo_m;
        if (salita > std::max(SALITA_MIN_M, SALITA_MIN_REL * prima->fondo_m))
            out.push_back({AvvisoFondo::Tipo::fondo_in_salita, salita, ultimo->t});
    }
    // il BQS-15: un ostacolo a prua entro il tempo di reazione; la velocita' sicura dalla portata che ha visto
    const double v_m_s = velocita_kn * NODO;
    double ostacolo = std::numeric_limits<double>::infinity(), t_ostacolo = 0.0, vista = 0.0;
    bool spazzata = false;
    for (const RitornoFondo& r : ritorni_) {
        if (r.t > t || t - r.t > RITORNI_RECENTI_S) continue;
        if (std::fabs(normalizza_180(r.rilevamento - rotta)) > SETTORE_PRUA_DEG) continue;
        spazzata = true;
        if (!r.eco) {
            vista = std::max(vista, r.distanza_m);
            continue;
        }
        const double d = distanza(pos_stimata, r.punto);
        if (r.ostacolo || r.profondita_m < quota_m + MARGINE_OSTACOLO_M) {
            if (d < ostacolo) ostacolo = d, t_ostacolo = r.t;
        } else {
            vista = std::max(vista, d);
        }
    }
    if (std::isfinite(ostacolo) && ostacolo <= std::max(v_m_s, 1.0) * reazione_s)
        out.push_back({AvvisoFondo::Tipo::ostacolo_a_prua, ostacolo, t_ostacolo});
    if (spazzata) {
        const double sicura_kn = std::min(vista, std::isfinite(ostacolo) ? ostacolo : vista) / reazione_s / NODO;
        if (velocita_kn > sicura_kn) out.push_back({AvvisoFondo::Tipo::velocita_eccessiva, sicura_kn, t});
    }
    return out;
}

void QuadroFondale::salva(std::ostream& out) const {
    out.write(MAGICO, sizeof(MAGICO));
    scrivi(out, VERSIONE);
    scrivi(out, ultimo_punto_nave_);
    scrivi(out, static_cast<std::uint64_t>(sondaggi_.size()));
    for (std::size_t i = 0; i < sondaggi_.size(); ++i) {
        const Sondaggio& s = sondaggi_[i];
        for (double v : {s.t, s.pos.x, s.pos.y, s.quota_m, s.sotto_chiglia_m, s.fondo_m, s.sigma_m, s.impronta_m, s.pos_vera.x,
                         s.pos_vera.y, s.fondo_vero_m, sigma_sondaggi_[i]})
            scrivi(out, v);
        scrivi(out, static_cast<std::uint8_t>((s.eco ? 1 : 0) | (s.falso ? 2 : 0) | (s.potenza == PotenzaEco::bassa ? 4 : 0)));
    }
    scrivi(out, static_cast<std::uint64_t>(ritorni_.size()));
    for (std::size_t i = 0; i < ritorni_.size(); ++i) {
        const RitornoFondo& r = ritorni_[i];
        for (double v : {r.t, r.origine.x, r.origine.y, r.quota_m, r.rilevamento, r.elevazione, r.distanza_m, r.punto.x, r.punto.y,
                         r.profondita_m, r.sigma_m, sigma_ritorni_[i]})
            scrivi(out, v);
        scrivi(out, static_cast<std::uint8_t>((r.eco ? 1 : 0) | (r.ostacolo ? 2 : 0)));
    }
}

QuadroFondale QuadroFondale::carica(std::istream& in, const Fondale& carta) {
    char m[sizeof(MAGICO)];
    in.read(m, sizeof(m));
    if (!in || std::memcmp(m, MAGICO, sizeof(m)) != 0) throw std::runtime_error("quadro_fondale: non e' un quadro");
    if (leggi_bin<std::uint32_t>(in) != VERSIONE) throw std::runtime_error("quadro_fondale: versione sconosciuta");
    QuadroFondale q(carta);
    q.ultimo_punto_nave_ = leggi_bin<double>(in);
    const std::uint64_t ns = leggi_bin<std::uint64_t>(in);
    if (ns > (1u << 24)) throw std::runtime_error("quadro_fondale: dimensioni assurde");
    for (std::uint64_t i = 0; i < ns; ++i) {
        Sondaggio s;
        double* campi[] = {&s.t, &s.pos.x, &s.pos.y, &s.quota_m, &s.sotto_chiglia_m, &s.fondo_m, &s.sigma_m, &s.impronta_m,
                           &s.pos_vera.x, &s.pos_vera.y, &s.fondo_vero_m};
        for (double* c : campi) *c = leggi_bin<double>(in);
        q.sigma_sondaggi_.push_back(leggi_bin<double>(in));
        const auto f = leggi_bin<std::uint8_t>(in);
        s.eco = f & 1;
        s.falso = f & 2;
        s.potenza = (f & 4) ? PotenzaEco::bassa : PotenzaEco::piena;
        q.sondaggi_.push_back(s);
    }
    const std::uint64_t nr = leggi_bin<std::uint64_t>(in);
    if (nr > (1u << 24)) throw std::runtime_error("quadro_fondale: dimensioni assurde");
    for (std::uint64_t i = 0; i < nr; ++i) {
        RitornoFondo r;
        double* campi[] = {&r.t, &r.origine.x, &r.origine.y, &r.quota_m, &r.rilevamento, &r.elevazione, &r.distanza_m, &r.punto.x,
                           &r.punto.y, &r.profondita_m, &r.sigma_m};
        for (double* c : campi) *c = leggi_bin<double>(in);
        q.sigma_ritorni_.push_back(leggi_bin<double>(in));
        const auto f = leggi_bin<std::uint8_t>(in);
        r.eco = f & 1;
        r.ostacolo = f & 2;
        q.ritorni_.push_back(r);
    }
    q.rifai();
    return q;
}

}  // namespace m688
