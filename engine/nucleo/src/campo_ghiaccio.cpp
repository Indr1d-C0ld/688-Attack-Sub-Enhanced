#include "nucleo/campo_ghiaccio.hpp"

#include <algorithm>
#include <cmath>

#include "nucleo/modifica_profilo.hpp"

namespace m688 {

namespace {
constexpr double Z99 = 2.3263478740408408;   // il quantile 0,99 della normale
constexpr std::uint64_t SALE_APERTA = 0xA9E57Aull, SALE_NILAS = 0x1A5ull, SALE_SPESSORE = 0x5BE5ull,
                        SALE_CHIGLIA = 0xC1161Aull, SALE_PESCAGGIO = 0xFE5CAull, SALE_PESCAGGIO2 = 0xFE5CBull;

std::uint64_t mescola(std::uint64_t z) {
    z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ull;
    z = (z ^ (z >> 27)) * 0x94d049bb133111ebull;
    return z ^ (z >> 31);
}
}  // namespace

ParametriGhiaccio ghiaccio_pluriennale() {
    ParametriGhiaccio p;
    p.spessore_livellato_m = 3.0;
    p.chiglia_media_m = 6.0;
    p.chiglia_p99_m = 20.0;
    p.chiglia_max_m = 50.0;
    p.creste_per_km = 6.0;
    return p;
}

CampoGhiaccio::CampoGhiaccio(std::uint64_t seme, ParametriGhiaccio p, std::function<double(Vec2)> copertura)
    : seme_(seme), p_(p), copertura_(std::move(copertura)) {
    // la lognormale dalla media e dal 99° percentile: mu + s^2/2 = ln(media), mu + Z99 s = ln(p99)
    const double a = std::log(p_.chiglia_media_m), b = std::log(p_.chiglia_p99_m);
    const double d = Z99 * Z99 - 2.0 * (b - a);   // s^2/2 - Z99 s + (b - a) = 0, la radice piccola
    sigma_ = d > 0.0 ? Z99 - std::sqrt(d) : 0.5;
    mu_ = a - 0.5 * sigma_ * sigma_;
}

double CampoGhiaccio::u01(std::int64_t i, std::int64_t j, std::uint64_t sale) const {
    const std::uint64_t z = mescola(seme_ ^ sale ^ (static_cast<std::uint64_t>(i) * 0x9E3779B97F4A7C15ull) ^
                                    (static_cast<std::uint64_t>(j) * 0xC2B2AE3D27D4EB4Full));
    return static_cast<double>(z >> 11) * 0x1.0p-53;
}

void CampoGhiaccio::cambia_deriva(double t, Vec2 v_m_s) {
    if (!tratti_.empty() && t < tratti_.back().t) return;
    tratti_.push_back({t, v_m_s});
}

Vec2 CampoGhiaccio::spostamento(double t) const {
    if (tratti_.empty()) return {p_.deriva_m_s.x * t, p_.deriva_m_s.y * t};
    Vec2 s{};
    double da = 0.0;
    Vec2 v = p_.deriva_m_s;
    for (const TrattoDeriva& x : tratti_) {
        const double fino = std::min(t, x.t);
        if (fino > da) {
            s.x += v.x * (fino - da);
            s.y += v.y * (fino - da);
            da = fino;
        }
        v = x.v;
    }
    if (t > da) {
        s.x += v.x * (t - da);
        s.y += v.y * (t - da);
    }
    return s;
}

double CampoGhiaccio::copertura_in(Vec2 p, double t) const {
    Vec2 q = p;
    if (!tratti_.empty()) {   // il bordo del pack va dietro alla deriva cambiata; quella dei parametri non lo muove
        const Vec2 s = spostamento(t);
        q = {p.x - (s.x - p_.deriva_m_s.x * t), p.y - (s.y - p_.deriva_m_s.y * t)};
    }
    double cop = copertura_ ? std::clamp(copertura_(q), 0.0, 1.0) : 1.0;
    for (const ZonaGhiaccio& z : zone_) {
        const double f = fase_cambiamento(t, z.t0, z.transizione_s, z.durata_s);
        if (f <= 0.0) continue;
        if (z.apri) {
            if (distanza(p, z.centro) <= z.raggio_m * f) cop = 0.0;
        } else if (distanza(p, z.centro) <= z.raggio_m) {
            cop = std::max(cop, f);
        }
    }
    return cop;
}

double CampoGhiaccio::spessore_cella(Vec2 q) const {
    const auto ci = static_cast<std::int64_t>(std::floor(q.x / p_.cella_m)), cj = static_cast<std::int64_t>(std::floor(q.y / p_.cella_m));
    return p_.spessore_livellato_m * (1.0 + p_.variazione_spessore * (2.0 * u01(ci, cj, SALE_SPESSORE) - 1.0));
}

PuntoGhiaccio CampoGhiaccio::in(Vec2 p, double t) const {
    PuntoGhiaccio out = in_campo(p, t);
    for (const ZonaGhiaccio& z : zone_) {
        const double f = fase_cambiamento(t, z.t0, z.transizione_s, z.durata_s);
        if (f <= 0.0) continue;
        if (z.apri) {
            if (distanza(p, z.centro) <= z.raggio_m * f) {
                out = PuntoGhiaccio{};
                out.aperta = true;
            }
        } else if (distanza(p, z.centro) <= z.raggio_m && (!out.ghiaccio || out.nilas)) {
            out = PuntoGhiaccio{};
            out.ghiaccio = true;
            if (f < 1.0) {
                out.nilas = true;
                out.spessore_m = p_.nilas_min_m + (p_.nilas_max_m - p_.nilas_min_m) * f;
            } else {
                const Vec2 s = spostamento(t);
                out.spessore_m = spessore_cella({p.x - s.x, p.y - s.y});
            }
            out.pescaggio_m = 0.9 * out.spessore_m;
        }
    }
    return out;
}

PuntoGhiaccio CampoGhiaccio::in_campo(Vec2 p, double t) const {
    PuntoGhiaccio out;
    Vec2 c = p;   // la copertura: dove il bordo del pack e' arrivato (copertura_in, senza le zone)
    const Vec2 s = spostamento(t);
    if (!tratti_.empty()) c = {p.x - (s.x - p_.deriva_m_s.x * t), p.y - (s.y - p_.deriva_m_s.y * t)};
    const double cop = copertura_ ? std::clamp(copertura_(c), 0.0, 1.0) : 1.0;
    if (cop <= 0.0) return out;
    // il punto nel campo che deriva
    const Vec2 q{p.x - s.x, p.y - s.y};
    // l'acqua libera a blocchi: a copertura c la frazione aperta e' 1 - c + c x frazione_aperta
    const auto bi = static_cast<std::int64_t>(std::floor(q.x / p_.scala_aperta_m)), bj = static_cast<std::int64_t>(std::floor(q.y / p_.scala_aperta_m));
    const double aperta = 1.0 - cop + cop * p_.frazione_aperta;
    if (u01(bi, bj, SALE_APERTA) < aperta) {
        if (u01(bi, bj, SALE_NILAS) < p_.frazione_nilas && cop >= 0.5) {   // richiuso dal ghiaccio nuovo (col freddo del pack)
            out.ghiaccio = out.nilas = true;
            out.spessore_m = p_.nilas_min_m + (p_.nilas_max_m - p_.nilas_min_m) * u01(bi, bj, SALE_SPESSORE);
            out.pescaggio_m = 0.9 * out.spessore_m;
        } else {
            out.aperta = true;
        }
        return out;
    }
    // il ghiaccio della cella: lo spessore livellato e la chiglia
    const auto ci = static_cast<std::int64_t>(std::floor(q.x / p_.cella_m)), cj = static_cast<std::int64_t>(std::floor(q.y / p_.cella_m));
    out.ghiaccio = true;
    out.spessore_m = p_.spessore_livellato_m * (1.0 + p_.variazione_spessore * (2.0 * u01(ci, cj, SALE_SPESSORE) - 1.0));
    out.pescaggio_m = 0.9 * out.spessore_m;
    const double lambda = p_.creste_per_km * p_.cella_m / 1000.0;   // creste attese nella cella
    if (u01(ci, cj, SALE_CHIGLIA) < 1.0 - std::exp(-lambda)) {
        // la normale di Box-Muller da due numeri della cella, poi la lognormale troncata
        const double u1 = std::max(u01(ci, cj, SALE_PESCAGGIO), 1e-12), u2 = u01(ci, cj, SALE_PESCAGGIO2);
        const double z = std::sqrt(-2.0 * std::log(u1)) * std::cos(2.0 * PI * u2);
        const double chiglia = std::min(p_.chiglia_max_m, std::exp(mu_ + sigma_ * z));
        if (chiglia > out.pescaggio_m) {
            out.pescaggio_m = chiglia;
            out.chiglia = true;
        }
    }
    return out;
}

double CampoGhiaccio::pescaggio_max(Vec2 a, Vec2 b, double t) const {
    const double d = distanza(a, b);
    const int n = std::max(1, static_cast<int>(std::ceil(d / (0.5 * p_.cella_m))));
    double m = 0.0;
    for (int k = 0; k <= n; ++k) {
        const double f = static_cast<double>(k) / n;
        m = std::max(m, in({a.x + f * (b.x - a.x), a.y + f * (b.y - a.y)}, t).pescaggio_m);
    }
    return m;
}

}  // namespace m688
