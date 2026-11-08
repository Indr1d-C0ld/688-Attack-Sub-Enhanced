#include "nucleo/storia_cascata.hpp"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <istream>
#include <ostream>
#include <stdexcept>

namespace m688 {

namespace {
constexpr char MAGICO[8] = {'M', '6', '8', '8', 'S', 'C', 'A', 'S'};
constexpr std::uint32_t VERSIONE = 1;
constexpr std::size_t FONDO_BIN = 15;   // la mediana locale, come righe_osservate

template <typename T>
void scrivi(std::ostream& o, const T& x) {
    o.write(reinterpret_cast<const char*>(&x), sizeof(T));
}
template <typename T>
T leggi(std::istream& in) {
    T x{};
    in.read(reinterpret_cast<char*>(&x), sizeof(T));
    if (!in) throw std::runtime_error("storia_cascata: salvataggio troncato");
    return x;
}
}  // namespace

StoriaCascata::StoriaCascata(double f0, double df, std::size_t bin, ParametriStoria p)
    : f0_(f0), df_(df > 0.0 ? df : 1.0), bin_(bin), p_(std::move(p)), livelli_(p_.livelli.size()) {
    if (p_.passo_db <= 0.0) p_.passo_db = 0.25;
}

std::uint8_t StoriaCascata::quantizza(float db) const {
    const double q = std::round((static_cast<double>(db) - p_.minimo_db) / p_.passo_db);
    return static_cast<std::uint8_t>(std::clamp(q, 0.0, 255.0));
}

float StoriaCascata::valore(std::uint8_t q) const { return static_cast<float>(p_.minimo_db + p_.passo_db * q); }

void StoriaCascata::chiudi(Livello& l, std::size_t i) {
    l.anello.push_back(std::move(l.corrente));
    const LivelloStoria& d = p_.livelli[i];
    const std::size_t tiene = static_cast<std::size_t>(std::max(1.0, std::ceil(d.durata_s / d.passo_s)));
    while (l.anello.size() > tiene) l.anello.pop_front();
    l.corrente = Riga{};
    l.aperto = false;
}

void StoriaCascata::aggiungi(double t, const std::vector<float>& riga) {
    if (t < ultimo_t_) return;
    ultimo_t_ = t;
    std::vector<std::uint8_t> q(bin_, quantizza(static_cast<float>(p_.minimo_db)));
    for (std::size_t i = 0; i < std::min(bin_, riga.size()); ++i) q[i] = quantizza(riga[i]);
    for (std::size_t i = 0; i < livelli_.size(); ++i) {
        Livello& l = livelli_[i];
        const double passo = p_.livelli[i].passo_s;
        const double inizio = std::floor(t / passo) * passo;
        if (l.aperto && inizio != l.corrente.t) chiudi(l, i);
        if (!l.aperto) {
            l.corrente = Riga{inizio, q};
            l.aperto = true;
        } else {
            for (std::size_t k = 0; k < bin_; ++k) l.corrente.v[k] = std::max(l.corrente.v[k], q[k]);
        }
    }
}

FinestraStoria StoriaCascata::finestra(double da_s, double a_s, int livello) const {
    FinestraStoria f;
    f.bin = bin_;
    if (livelli_.empty()) return f;
    std::size_t scelto = livelli_.size() - 1;
    if (livello >= 0) {
        scelto = std::min(static_cast<std::size_t>(livello), livelli_.size() - 1);
    } else {
        for (std::size_t i = 0; i < livelli_.size(); ++i) {
            const Livello& l = livelli_[i];
            const double piu_vecchia = !l.anello.empty() ? l.anello.front().t : (l.aperto ? l.corrente.t : 1e300);
            if (piu_vecchia <= da_s) {
                scelto = i;
                break;
            }
        }
    }
    const Livello& l = livelli_[scelto];
    const double passo = p_.livelli[scelto].passo_s;
    f.passo_s = passo;
    const auto metti = [&](const Riga& r) {
        if (r.t + passo <= da_s || r.t > a_s) return;
        f.t.push_back(r.t);
        for (std::uint8_t q : r.v) f.db.push_back(valore(q));
    };
    for (const Riga& r : l.anello) metti(r);
    if (l.aperto) metti(l.corrente);
    return f;
}

std::optional<double> StoriaCascata::comparsa(double hz, double soglia_db, double buco_s, double tolleranza_hz) const {
    if (bin_ == 0 || livelli_.empty()) return std::nullopt;
    const double x = (hz - f0_) / df_;
    if (x < 0.0 || x >= static_cast<double>(bin_)) return std::nullopt;
    const std::size_t c = static_cast<std::size_t>(x);
    const std::size_t tol = tolleranza_hz > 0.0 ? static_cast<std::size_t>(std::ceil(tolleranza_hz / df_)) : 2;
    const std::size_t a = c >= tol ? c - tol : 0, b = std::min(bin_ - 1, c + tol);
    const std::size_t fa = a >= FONDO_BIN ? a - FONDO_BIN : 0, fb = std::min(bin_ - 1, b + FONDO_BIN);
    const double soglia_q = soglia_db / p_.passo_db;
    std::vector<std::uint8_t> fondo;
    const auto picco = [&](const Riga& r) {
        const std::uint8_t m = *std::max_element(r.v.begin() + static_cast<std::ptrdiff_t>(a), r.v.begin() + static_cast<std::ptrdiff_t>(b) + 1);
        fondo.clear();
        for (std::size_t k = fa; k <= fb; ++k)
            if (k < a || k > b) fondo.push_back(r.v[k]);
        if (fondo.empty()) return false;
        std::nth_element(fondo.begin(), fondo.begin() + static_cast<std::ptrdiff_t>(fondo.size() / 2), fondo.end());
        return static_cast<double>(m) - static_cast<double>(fondo[fondo.size() / 2]) >= soglia_q;
    };
    // la linea del tempo dalla piu' recente all'indietro: il livello fine, poi i piu' grossi prima di dove il fine comincia
    struct Punto {
        double t, passo;
        bool picco;
    };
    std::vector<Punto> linea;
    double limite = 1e300;
    for (std::size_t i = 0; i < livelli_.size(); ++i) {
        const Livello& l = livelli_[i];
        const double passo = p_.livelli[i].passo_s;
        double piu_vecchia = limite;
        const auto metti = [&](const Riga& r) {
            if (r.t >= limite) return;
            piu_vecchia = r.t;
            linea.push_back({r.t, passo, picco(r)});
        };
        if (l.aperto) metti(l.corrente);
        for (auto it = l.anello.rbegin(); it != l.anello.rend(); ++it) metti(*it);
        limite = std::min(limite, piu_vecchia);
    }
    // c'e' quando c'e' nella maggioranza delle righe vicine: la grana fa picchi isolati (uno su dieci col massimo di
    // cinque bin), una riga vera c'e' quasi sempre
    constexpr std::ptrdiff_t VICINE = 4;
    const std::ptrdiff_t n = static_cast<std::ptrdiff_t>(linea.size());
    const auto presente = [&](std::ptrdiff_t i) {
        int si = 0, tutte = 0;
        for (std::ptrdiff_t k = std::max<std::ptrdiff_t>(0, i - VICINE); k <= std::min(n - 1, i + VICINE); ++k) {
            ++tutte;
            si += linea[static_cast<std::size_t>(k)].picco;
        }
        return linea[static_cast<std::size_t>(i)].picco && 2 * si > tutte;
    };
    std::optional<double> vista;
    for (std::ptrdiff_t i = 0; i < n; ++i) {
        const Punto& q = linea[static_cast<std::size_t>(i)];
        if (presente(i)) vista = q.t;
        else if (vista && *vista - (q.t + q.passo) > buco_s) return vista;
        else if (!vista && ultimo_t_ - (q.t + q.passo) > buco_s) return std::nullopt;   // adesso non c'e'
    }
    return vista;
}

double StoriaCascata::inizio_s() const {
    double t = ultimo_t_;
    for (const Livello& l : livelli_) {
        if (!l.anello.empty()) t = std::min(t, l.anello.front().t);
        else if (l.aperto) t = std::min(t, l.corrente.t);
    }
    return t;
}

double StoriaCascata::fine_s() const { return ultimo_t_; }

std::size_t StoriaCascata::memoria_byte() const {
    std::size_t n = sizeof(*this);
    for (const Livello& l : livelli_) n += (l.anello.size() + 1) * (sizeof(Riga) + bin_);
    return n;
}

void StoriaCascata::salva(std::ostream& out) const {
    out.write(MAGICO, sizeof(MAGICO));
    scrivi(out, VERSIONE);
    scrivi(out, f0_);
    scrivi(out, df_);
    scrivi(out, static_cast<std::uint64_t>(bin_));
    scrivi(out, p_.passo_db);
    scrivi(out, p_.minimo_db);
    scrivi(out, ultimo_t_);
    scrivi(out, static_cast<std::uint32_t>(livelli_.size()));
    for (std::size_t i = 0; i < livelli_.size(); ++i) {
        const Livello& l = livelli_[i];
        scrivi(out, p_.livelli[i].passo_s);
        scrivi(out, p_.livelli[i].durata_s);
        scrivi(out, static_cast<std::uint8_t>(l.aperto));
        scrivi(out, static_cast<std::uint64_t>(l.anello.size() + (l.aperto ? 1 : 0)));
        const auto riga = [&](const Riga& r) {
            scrivi(out, r.t);
            out.write(reinterpret_cast<const char*>(r.v.data()), static_cast<std::streamsize>(bin_));
        };
        for (const Riga& r : l.anello) riga(r);
        if (l.aperto) riga(l.corrente);
    }
}

StoriaCascata StoriaCascata::carica(std::istream& in) {
    char m[sizeof(MAGICO)];
    in.read(m, sizeof(m));
    if (!in || std::memcmp(m, MAGICO, sizeof(m)) != 0) throw std::runtime_error("storia_cascata: non e' una storia");
    if (leggi<std::uint32_t>(in) != VERSIONE) throw std::runtime_error("storia_cascata: versione sconosciuta");
    const double f0 = leggi<double>(in), df = leggi<double>(in);
    const std::uint64_t bin = leggi<std::uint64_t>(in);
    ParametriStoria p;
    p.passo_db = leggi<double>(in);
    p.minimo_db = leggi<double>(in);
    const double ultimo = leggi<double>(in);
    const std::uint32_t nl = leggi<std::uint32_t>(in);
    if (bin > (1u << 24) || nl > 16) throw std::runtime_error("storia_cascata: dimensioni assurde");
    p.livelli.clear();
    struct Letto {
        bool aperto;
        std::vector<Riga> righe;
    };
    std::vector<Letto> letti;
    for (std::uint32_t i = 0; i < nl; ++i) {
        LivelloStoria d;
        d.passo_s = leggi<double>(in);
        d.durata_s = leggi<double>(in);
        p.livelli.push_back(d);
        Letto x;
        x.aperto = leggi<std::uint8_t>(in) != 0;
        const std::uint64_t n = leggi<std::uint64_t>(in);
        if (n > (1u << 20)) throw std::runtime_error("storia_cascata: dimensioni assurde");
        for (std::uint64_t k = 0; k < n; ++k) {
            Riga r;
            r.t = leggi<double>(in);
            r.v.resize(bin);
            in.read(reinterpret_cast<char*>(r.v.data()), static_cast<std::streamsize>(bin));
            if (!in) throw std::runtime_error("storia_cascata: salvataggio troncato");
            x.righe.push_back(std::move(r));
        }
        letti.push_back(std::move(x));
    }
    StoriaCascata s(f0, df, static_cast<std::size_t>(bin), p);
    s.ultimo_t_ = ultimo;
    for (std::size_t i = 0; i < letti.size(); ++i) {
        Livello& l = s.livelli_[i];
        std::vector<Riga>& r = letti[i].righe;
        if (letti[i].aperto && !r.empty()) {
            l.corrente = std::move(r.back());
            l.aperto = true;
            r.pop_back();
        }
        for (Riga& x : r) l.anello.push_back(std::move(x));
    }
    return s;
}

}  // namespace m688
