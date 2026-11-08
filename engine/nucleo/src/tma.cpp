#include "nucleo/tma.hpp"

#include <cmath>

namespace m688 {

namespace {
constexpr double GRADI_MIN_PER_RAD_S = 180.0 / PI * 60.0;
}  // namespace

void Tma::aggiungi(const Rilevamento& r) {
    ril_.push_back(r);
    if (ril_.size() > MASSIMO) ril_.erase(ril_.begin());
}

void Tma::aggiungi_rotta(const OsservazioneRotta& o) {
    rotte_.push_back(o);
    if (rotte_.size() > MASSIMO_ROTTE) rotte_.erase(rotte_.begin());
}

std::vector<double> Tma::scarti_rotta(const SoluzioneTma& s) const {
    std::vector<double> r;
    r.reserve(rotte_.size());
    for (const auto& o : rotte_) r.push_back(normalizza_180(s.rotta - o.rotta));
    return r;
}

std::optional<Vec2> Tma::posizione(const SoluzioneTma& s, double t) const {
    if (ril_.empty()) return std::nullopt;
    const Rilevamento& u = ril_.back();
    const double b = radianti(u.rilevamento), c = radianti(s.rotta), dt = t - u.tempo;
    return Vec2{u.proprio.x + s.distanza_m * std::sin(b) + s.velocita_m_s * dt * std::sin(c),
                u.proprio.y + s.distanza_m * std::cos(b) + s.velocita_m_s * dt * std::cos(c)};
}

std::vector<double> Tma::scarti(const SoluzioneTma& s) const {
    std::vector<double> r;
    r.reserve(ril_.size());
    for (const auto& x : ril_) {
        const Vec2 p = *posizione(s, x.tempo);
        r.push_back(normalizza_180(rilevamento(x.proprio, p) - x.rilevamento));
    }
    return r;
}

double Tma::scarto_quadratico_medio(const SoluzioneTma& s) const {
    const auto e = scarti(s);
    if (e.empty()) return 0.0;
    double q = 0.0;
    for (const double d : e) q += d * d;
    return std::sqrt(q / static_cast<double>(e.size()));
}

std::optional<double> Tma::rateo(double t0, double t1) const {
    // minimi quadrati su rilevamenti srotolati (senza salti a 0/360)
    double n = 0, st = 0, sb = 0, stt = 0, stb = 0, prec = 0.0, b = 0.0;
    bool primo = true;
    for (const auto& x : ril_) {
        if (x.tempo < t0 || x.tempo > t1) continue;
        b = primo ? x.rilevamento : b + normalizza_180(x.rilevamento - prec);
        prec = x.rilevamento;
        primo = false;
        const double t = (x.tempo - t0) / 60.0;
        n += 1; st += t; sb += b; stt += t * t; stb += t * b;
    }
    const double den = n * stt - st * st;
    if (n < 2 || std::fabs(den) < 1e-12) return std::nullopt;
    return (n * stb - st * sb) / den;
}

double Tma::attraverso(Vec2 v, double ril) {
    const double b = radianti(ril);
    return v.x * std::cos(b) - v.y * std::sin(b);   // versore dei rilevamenti crescenti: (cos b, -sin b)
}

double Tma::rateo_1934(double v_perp, double distanza) { return distanza > 0.0 ? v_perp / distanza * GRADI_MIN_PER_RAD_S : 0.0; }

std::optional<double> Tma::distanza_ekelund(double osa1, double osa2, double b1, double b2) {
    const double db = (b1 - b2) / GRADI_MIN_PER_RAD_S;   // rad/s
    if (std::fabs(db) < 1e-9) return std::nullopt;
    const double r = (osa2 - osa1) / db;
    if (r <= 0.0) return std::nullopt;
    return r;
}

}  // namespace m688
