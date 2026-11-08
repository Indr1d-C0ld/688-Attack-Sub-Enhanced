#include "nucleo/interferenza.hpp"

#include <algorithm>
#include <cmath>
#include <complex>

namespace m688 {

namespace {

constexpr double FONDALE_IGNOTO_M = 1e8;
constexpr double MINIMO_LLOYD = 1e-3;   // il minimo del fattore di Lloyd nel livello A (acustica.cpp)
// rugosita' del fondo per la parte coerente del rimbalzo (scarto in metri, STIMA: Jensen §1.6 e APL-UW, ordini di
// grandezza per ghiaia e roccia, sabbia, fango)
double rugosita_fondo_m(TipoFondo t) { return t == TipoFondo::roccia ? 1.0 : t == TipoFondo::sabbia ? 0.3 : 0.2; }

}  // namespace

std::vector<ImmagineCammino> immagini_cammini(double r_m, double zs, double zr, double fondale_m, int ordini) {
    const double d = std::max(fondale_m, 1.0);
    zs = std::clamp(zs, 0.5, d);
    zr = std::clamp(zr, 0.5, d);
    const double r = std::max(r_m, 1.0);
    std::vector<ImmagineCammino> out;
    const auto aggiungi = [&](double z, int sup, int fon) {
        out.push_back({std::hypot(r, z), gradi(std::atan2(std::fabs(z), r)), sup, fon});
    };
    if (fondale_m >= FONDALE_IGNOTO_M) {
        aggiungi(zs - zr, 0, 0);
        aggiungi(zs + zr, 1, 0);
        return out;
    }
    for (int m = 0; m < std::max(ordini, 1); ++m) {
        const double md = 2.0 * m * d;
        aggiungi(md + zs - zr, m, m);
        aggiungi(md + 2.0 * d - zs - zr, m, m + 1);
        aggiungi(md + zs + zr, m + 1, m);
        aggiungi(md + 2.0 * d - zs + zr, m + 1, m + 1);
    }
    return out;
}

Interferenza::Interferenza(double r_m, double zs_m, double zr_m, const Ambiente& amb, double dr_m, ParametriInterferenza par)
    : zs_(zs_m), zr_(zr_m), amb_(amb), par_(par) {
    const SpecchioLloyd sl = specchio_lloyd(1000.0, r_m, zs_m, zr_m, amb);   // c0 e onda non dipendono da f
    c0_ = sl.c0_m_s;
    sigma_onda_ = sl.sigma_onda_m;
    const double r = std::max(r_m, 1.0);
    const double mezzo = 0.5 * std::fabs(dr_m);
    std::vector<double> distanze = mezzo > 0.5 ? std::vector<double>{std::max(r - mezzo, 1.0), r, r + mezzo} : std::vector<double>{r};
    for (double x : distanze) {
        Istante t{x, immagini_cammini(x, zs_m, zr_m, amb.fondale_m, par_.ordini), {}};
        for (const auto& im : t.immagini) t.coppia.push_back(im.fondo == 0 && im.superficie <= 1);
        istanti_.push_back(std::move(t));
    }
}

Interferenza::Ampiezze Interferenza::ampiezze(const Istante& t, double f) const {
    Ampiezze a;
    a.coerente.reserve(t.immagini.size());
    // il riferimento: tutti i cammini in potenza, salvo la coppia diretto e superficie che sotto pi/4 (il dipolo) e'
    // coerente come nel livello A, col suo stesso minimo. Cosi' i termini incrociati, che in media sulle frequenze
    // valgono zero, sono le frange
    const double lloyd = std::max(specchio_lloyd(f, t.r, zs_, zr_, amb_).fattore, MINIMO_LLOYD) / 2.0;
    const double k = 2.0 * PI * f / c0_;
    const double rug = rugosita_fondo_m(amb_.fondo);
    const double lontano = std::exp(-t.r / par_.coerenza_fondo_m);
    double coppia = 0.0;
    for (std::size_t i = 0; i < t.immagini.size(); ++i) {
        const ImmagineCammino& im = t.immagini[i];
        double ampiezza = 1.0 / im.lunghezza_m;
        const double s = std::sin(radianti(im.radenza_gradi));
        double g = std::exp(-2.0 * im.superficie * std::pow(k * sigma_onda_ * s, 2.0));
        if (im.fondo > 0) {
            ampiezza *= std::pow(10.0, -im.fondo * perdita_fondo_db(amb_.fondo, f, im.radenza_gradi) / 20.0);
            g *= std::exp(-2.0 * im.fondo * std::pow(k * rug * s, 2.0)) * lontano;
        }
        const double e = ampiezza * ampiezza;
        a.coerente.push_back((im.superficie % 2 ? -g : g) * ampiezza);
        if (t.coppia[i]) {
            coppia += e;
            a.incoerente_coppia += e * (1.0 - g * g);
        } else {
            a.incoerente_resto += e * (1.0 - g * g);
            a.riferimento += e;
        }
    }
    a.riferimento += coppia * lloyd;
    a.minimo_coppia = coppia * MINIMO_LLOYD / 2.0;
    return a;
}

double Interferenza::rapporto(std::complex<double> p, std::complex<double> q, double incoerente_coppia, double incoerente_resto,
                              double minimo_coppia, double riferimento) {
    // |p + q|^2 con la coppia mai sotto il minimo del livello A
    const double campo = std::max(std::norm(p) + incoerente_coppia, minimo_coppia) + std::norm(q) + incoerente_resto +
                         2.0 * (p * std::conj(q)).real();
    return std::max(campo, 0.0) / riferimento;
}

double Interferenza::limita(double r) const {
    return std::clamp(10.0 * std::log10(r > 1e-30 ? r : 1e-30), par_.min_db, par_.max_db);   // anche un NaN va al minimo
}

double Interferenza::db(double f) const {
    double somma = 0.0;
    const double k = 2.0 * PI * f / c0_;
    for (const Istante& t : istanti_) {
        const Ampiezze a = ampiezze(t, f);
        std::complex<double> p{0.0, 0.0}, q{0.0, 0.0};
        for (std::size_t i = 0; i < t.immagini.size(); ++i)
            (t.coppia[i] ? p : q) += a.coerente[i] * std::polar(1.0, k * t.immagini[i].lunghezza_m);   // con segno: polar vuole rho >= 0
        somma += rapporto(p, q, a.incoerente_coppia, a.incoerente_resto, a.minimo_coppia, a.riferimento);
    }
    return limita(somma / static_cast<double>(istanti_.size()));
}

std::vector<float> Interferenza::db_griglia(double f0, double df, std::size_t n) const {
    std::vector<double> somma(n, 0.0);
    if (n == 0 || df <= 0.0) return {};
    const std::size_t passo = std::max<std::size_t>(1, static_cast<std::size_t>(std::lround(1.0 / df)));
    const auto frequenza = [&](std::size_t i) { return f0 + (static_cast<double>(i) + 0.5) * df; };
    const auto tra = [](double x, double y, double u) { return x + u * (y - x); };
    for (const Istante& t : istanti_) {
        const std::size_t m = t.immagini.size();
        std::vector<std::complex<double>> giro(m), z(m);
        for (std::size_t j = 0; j < m; ++j) giro[j] = std::polar(1.0, 2.0 * PI * df * t.immagini[j].lunghezza_m / c0_);
        Ampiezze da = ampiezze(t, frequenza(0));
        for (std::size_t a = 0; a < n;) {
            const std::size_t b = std::min(a + passo, n - 1);
            const Ampiezze ab = b > a ? ampiezze(t, frequenza(b)) : da;
            const double k = 2.0 * PI * frequenza(a) / c0_;
            for (std::size_t j = 0; j < m; ++j) z[j] = std::polar(1.0, k * t.immagini[j].lunghezza_m);   // fasi esatte al nodo
            const std::size_t fine = b > a && b < n - 1 ? b : b + 1;   // l'ultimo blocco comprende l'ultimo bin
            for (std::size_t i = a; i < fine; ++i) {
                const double u = b > a ? static_cast<double>(i - a) / static_cast<double>(b - a) : 0.0;
                std::complex<double> p{0.0, 0.0}, q{0.0, 0.0};
                for (std::size_t j = 0; j < m; ++j) {
                    (t.coppia[j] ? p : q) += tra(da.coerente[j], ab.coerente[j], u) * z[j];
                    z[j] *= giro[j];
                }
                somma[i] += rapporto(p, q, tra(da.incoerente_coppia, ab.incoerente_coppia, u), tra(da.incoerente_resto, ab.incoerente_resto, u),
                                     tra(da.minimo_coppia, ab.minimo_coppia, u), tra(da.riferimento, ab.riferimento, u));
            }
            da = ab;
            a = fine;
        }
    }
    std::vector<float> out(n);
    for (std::size_t i = 0; i < n; ++i) out[i] = static_cast<float>(limita(somma[i] / static_cast<double>(istanti_.size())));
    return out;
}

}  // namespace m688
