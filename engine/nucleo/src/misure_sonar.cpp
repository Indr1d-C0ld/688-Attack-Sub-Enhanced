#include "nucleo/misure_sonar.hpp"

#include <algorithm>
#include <array>
#include <cmath>

namespace m688 {

namespace {

constexpr double SIGMA_MIN_T_S = 30.0, SIGMA_MIN_R_M = 100.0, SIGMA_MIN_V_M_S = 0.3;   // per la coerenza con la TMA
constexpr double FINESTRA_CPA_S = 120.0;   // i rilevamenti attorno al CPA per il rilevamento e il suo rateo (blocco 3, L)
constexpr double MARGINE_CPA = 0.1;   // il CPA dev'essere dentro i punti, a un decimo dagli estremi

using Vettore4 = std::array<double, 4>;
using Matrice4 = std::array<std::array<double, 4>, 4>;

// inversa di Gauss-Jordan con pivot; false se singolare
bool inverti(Matrice4 a, Matrice4& inv) {
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j) inv[i][j] = i == j ? 1.0 : 0.0;
    for (int c = 0; c < 4; ++c) {
        int p = c;
        for (int r = c + 1; r < 4; ++r)
            if (std::fabs(a[r][c]) > std::fabs(a[p][c])) p = r;
        if (std::fabs(a[p][c]) < 1e-300) return false;
        std::swap(a[p], a[c]);
        std::swap(inv[p], inv[c]);
        const double d = a[c][c];
        for (int j = 0; j < 4; ++j) { a[c][j] /= d; inv[c][j] /= d; }
        for (int r = 0; r < 4; ++r) {
            if (r == c) continue;
            const double k = a[r][c];
            for (int j = 0; j < 4; ++j) { a[r][j] -= k * a[c][j]; inv[r][j] -= k * inv[c][j]; }
        }
    }
    return true;
}

// p = (f0, v, t_cpa, R_cpa)
double modello(const Vettore4& p, double t) {
    const double v = std::fabs(p[1]), r = std::max(std::fabs(p[3]), 1.0), dt = t - p[2];
    return p[0] * (1.0 - v * v * dt / (C_SUONO_MISURE * std::sqrt(r * r + v * v * dt * dt)));
}

double peso(const PuntoFrequenza& x) { return x.sigma_hz > 0.0 ? 1.0 / x.sigma_hz : 1.0; }

double chi2(const std::vector<PuntoFrequenza>& pt, const Vettore4& p) {
    double s = 0.0;
    for (const auto& x : pt) {
        const double r = (x.hz - modello(p, x.t)) * peso(x);
        s += r * r;
    }
    return s;
}

}  // namespace

// --- verniero ---

MisuraRiga misura_riga(const Spettro& s, double attorno_hz, double finestra_hz, double integrazione_s, double soglia_db) {
    MisuraRiga m;
    const std::size_t n = s.snr_db.size();
    if (n == 0 || s.df_hz <= 0.0) return m;
    std::size_t migliore = n;
    std::vector<float> finestra;
    for (std::size_t i = 0; i < n; ++i) {
        if (std::fabs(s.frequenza(i) - attorno_hz) > finestra_hz) continue;
        finestra.push_back(s.snr_db[i]);
        if (migliore == n || s.snr_db[i] > s.snr_db[migliore]) migliore = i;
    }
    if (migliore == n) return m;
    // il fondo locale (mediana della finestra): il continuo della sorgente e il rumore sono il «rumore» della riga
    std::nth_element(finestra.begin(), finestra.begin() + static_cast<long>(finestra.size() / 2), finestra.end());
    const double fondo = finestra[finestra.size() / 2];
    const double b = s.snr_db[migliore];
    double delta = 0.0;
    if (migliore > 0 && migliore + 1 < n) {
        const double a = s.snr_db[migliore - 1], c = s.snr_db[migliore + 1], den = a - 2.0 * b + c;
        if (den < 0.0) delta = std::clamp(0.5 * (a - c) / den, -0.5, 0.5);
    }
    m.hz = s.frequenza(migliore) + delta * s.df_hz;
    m.snr_db = b;
    const double snr = std::pow(10.0, (b - fondo) / 10.0) - 1.0;
    const double durata = 1.0 / s.df_hz;
    const double guadagno = std::max(1.0, integrazione_s / durata);
    m.sigma_hz = snr > 0.0 ? std::sqrt(3.0) / (PI * durata * std::sqrt(snr * guadagno)) : s.df_hz;
    m.sigma_hz = std::max(m.sigma_hz, 0.01 * s.df_hz);
    m.valida = b - fondo >= soglia_db && snr > 0.0;
    return m;
}

// --- Doppler e CPA ---

double frequenza_cpa(const MisuraCpa& m, double t) {
    return modello({m.f0_hz, m.velocita_relativa_m_s, m.t_cpa, m.distanza_cpa_m}, t);
}

MisuraCpa misura_cpa(const std::vector<PuntoFrequenza>& punti) {
    MisuraCpa m;
    std::vector<PuntoFrequenza> pt = punti;
    std::sort(pt.begin(), pt.end(), [](const auto& a, const auto& b) { return a.t < b.t; });
    if (pt.size() < 8) {
        m.motivo = "troppo pochi punti";
        return m;
    }
    const double t0 = pt.front().t, t1 = pt.back().t;
    if (t1 - t0 <= 0.0) {
        m.motivo = "punti tutti allo stesso istante";
        return m;
    }
    // stima iniziale: estremi dai primi e dagli ultimi punti, CPA alla pendenza massima (retta locale)
    const std::size_t k = std::max<std::size_t>(3, pt.size() / 10);
    const auto media = [&](std::size_t da, std::size_t a) {
        double s = 0.0;
        for (std::size_t i = da; i < a; ++i) s += pt[i].hz;
        return s / static_cast<double>(a - da);
    };
    const double f_alta = media(0, k), f_bassa = media(pt.size() - k, pt.size());
    Vettore4 p{0.5 * (f_alta + f_bassa), 0.0, 0.5 * (t0 + t1), 1000.0};
    p[1] = std::max(0.5, C_SUONO_MISURE * (f_alta - f_bassa) / (2.0 * p[0]));
    double ripida = 0.0;
    for (std::size_t i = 0; i + k <= pt.size(); ++i) {
        std::vector<std::pair<double, double>> tv;
        for (std::size_t j = i; j < i + k; ++j) tv.emplace_back(pt[j].t, pt[j].hz);
        if (const auto d = pendenza(tv); d && -d->valore > ripida) {
            ripida = -d->valore;
            p[2] = 0.5 * (pt[i].t + pt[i + k - 1].t);
        }
    }
    if (ripida > 0.0) p[3] = std::clamp(p[0] * p[1] * p[1] / (C_SUONO_MISURE * ripida), 50.0, 1e5);
    // Levenberg-Marquardt con lo Jacobiano numerico
    double lambda = 1e-3, attuale = chi2(pt, p);
    Matrice4 jtj{};
    const auto jacobiano = [&](const Vettore4& q, Matrice4& a, Vettore4& g) {
        a = {};
        g = {};
        const Vettore4 passo{std::max(1e-6, 1e-7 * std::fabs(q[0])), 1e-4, 1e-2, 1e-1};
        for (const auto& x : pt) {
            const double w = peso(x), r = (x.hz - modello(q, x.t)) * w;
            Vettore4 d;
            for (int j = 0; j < 4; ++j) {
                Vettore4 qq = q;
                qq[j] += passo[j];
                d[j] = (modello(qq, x.t) - modello(q, x.t)) / passo[j] * w;
            }
            for (int i = 0; i < 4; ++i) {
                g[i] += d[i] * r;
                for (int j = 0; j < 4; ++j) a[i][j] += d[i] * d[j];
            }
        }
    };
    for (int iter = 0; iter < 200; ++iter) {
        Vettore4 g;
        jacobiano(p, jtj, g);
        Matrice4 a = jtj, inv;
        for (int i = 0; i < 4; ++i) a[i][i] *= 1.0 + lambda;
        if (!inverti(a, inv)) break;
        Vettore4 q = p;
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j) q[i] += inv[i][j] * g[j];
        const double nuovo = chi2(pt, q);
        if (nuovo < attuale) {
            const bool fermo = attuale - nuovo < 1e-12 * std::max(attuale, 1e-30);
            p = q;
            attuale = nuovo;
            lambda = std::max(lambda / 10.0, 1e-12);
            if (fermo) break;
        } else {
            lambda *= 10.0;
            if (lambda > 1e12) break;
        }
    }
    p[1] = std::fabs(p[1]);
    p[3] = std::fabs(p[3]);
    m.f0_hz = p[0];
    m.velocita_relativa_m_s = p[1];
    m.t_cpa = p[2];
    m.distanza_cpa_m = p[3];
    const std::size_t n = pt.size();
    m.residuo_hz = 0.0;
    for (const auto& x : pt) m.residuo_hz += std::pow(x.hz - modello(p, x.t), 2.0);
    m.residuo_hz = std::sqrt(m.residuo_hz / static_cast<double>(n));
    // covarianza: (J^T J)^-1 per la varianza dei residui pesati (o 1 se le sigma sono date e il chi2 e' piccolo)
    Vettore4 g;
    jacobiano(p, jtj, g);
    Matrice4 cov;
    const bool sigma_date = std::all_of(pt.begin(), pt.end(), [](const auto& x) { return x.sigma_hz > 0.0; });
    const double s2 = attuale / static_cast<double>(n - 4);
    const double scala = sigma_date ? std::max(1.0, s2) : s2;
    if (inverti(jtj, cov)) {
        m.sigma_f0_hz = std::sqrt(std::max(0.0, cov[0][0] * scala));
        m.sigma_velocita_m_s = std::sqrt(std::max(0.0, cov[1][1] * scala));
        m.sigma_t_cpa = std::sqrt(std::max(0.0, cov[2][2] * scala));
        m.sigma_distanza_m = std::sqrt(std::max(0.0, cov[3][3] * scala));
    }
    const double margine = MARGINE_CPA * (t1 - t0);
    m.cpa_osservato = m.t_cpa > t0 + margine && m.t_cpa < t1 - margine;
    if (!m.cpa_osservato) m.motivo = "CPA non osservato: distanza e velocita' non si separano";
    else if (m.velocita_relativa_m_s > 40.0 || m.distanza_cpa_m > 1e5) m.motivo = "soluzione fuori misura";
    else if (m.sigma_distanza_m > 0.5 * m.distanza_cpa_m) m.motivo = "distanza del CPA troppo incerta";
    m.valida = m.motivo.empty();
    return m;
}

std::optional<ScartoCpa> confronta_cpa(const Tma& tma, const SoluzioneTma& soluzione, const MisuraCpa& misura) {
    const auto& ril = tma.rilevamenti();
    if (ril.size() < 2) return std::nullopt;
    const Rilevamento& a = ril.front();
    const Rilevamento& b = ril.back();
    const double dt = b.tempo - a.tempo;
    if (dt <= 0.0) return std::nullopt;
    const Vec2 v_propria{(b.proprio.x - a.proprio.x) / dt, (b.proprio.y - a.proprio.y) / dt};
    const auto bersaglio = tma.posizione(soluzione, b.tempo);
    if (!bersaglio) return std::nullopt;
    const double rr = radianti(soluzione.rotta);
    const Vec2 v_bersaglio{soluzione.velocita_m_s * std::sin(rr), soluzione.velocita_m_s * std::cos(rr)};
    // moto relativo rettilineo dall'ultimo rilevamento
    const Vec2 d0{bersaglio->x - b.proprio.x, bersaglio->y - b.proprio.y};
    const Vec2 u{v_bersaglio.x - v_propria.x, v_bersaglio.y - v_propria.y};
    const double u2 = u.x * u.x + u.y * u.y;
    ScartoCpa s;
    const double tau = u2 > 1e-12 ? -(d0.x * u.x + d0.y * u.y) / u2 : 0.0;
    s.t_cpa_previsto = b.tempo + tau;
    s.distanza_prevista_m = std::hypot(d0.x + u.x * tau, d0.y + u.y * tau);
    s.velocita_prevista_m_s = std::sqrt(u2);
    s.scarto_t_s = s.t_cpa_previsto - misura.t_cpa;
    s.scarto_distanza_m = s.distanza_prevista_m - misura.distanza_cpa_m;
    s.scarto_velocita_m_s = s.velocita_prevista_m_s - misura.velocita_relativa_m_s;
    s.coerente = misura.valida && std::fabs(s.scarto_t_s) <= 2.0 * std::max(misura.sigma_t_cpa, SIGMA_MIN_T_S) &&
                 std::fabs(s.scarto_distanza_m) <= 2.0 * std::max(misura.sigma_distanza_m, SIGMA_MIN_R_M) &&
                 std::fabs(s.scarto_velocita_m_s) <= 2.0 * std::max(misura.sigma_velocita_m_s, SIGMA_MIN_V_M_S);
    return s;
}

std::optional<SoluzioneDaCpa> soluzione_da_cpa(const Tma& tma, const MisuraCpa& misura) {
    const auto& ril = tma.rilevamenti();
    if (!misura.valida || !misura.cpa_osservato || ril.size() < 3) return std::nullopt;
    const Rilevamento& a = ril.front();
    const Rilevamento& b = ril.back();
    const double tc = misura.t_cpa;
    if (tc < a.tempo || tc > b.tempo || b.tempo - a.tempo <= 0.0) return std::nullopt;
    // il rilevamento e il suo rateo al CPA: la retta sui rilevamenti entro 2 minuti (svolti attorno al piu' vicino)
    const Rilevamento* vicino = &ril.front();
    for (const Rilevamento& r : ril)
        if (std::fabs(r.tempo - tc) < std::fabs(vicino->tempo - tc)) vicino = &r;
    std::vector<std::pair<double, double>> tv;
    for (const Rilevamento& r : ril)
        if (std::fabs(r.tempo - tc) <= FINESTRA_CPA_S) tv.emplace_back(r.tempo - tc, normalizza_180(r.rilevamento - vicino->rilevamento));
    const auto p = pendenza(tv);
    if (!p || p->punti < 3) return std::nullopt;
    double media_t = 0.0, media_b = 0.0;
    for (const auto& [t, y] : tv) { media_t += t; media_b += y; }
    media_t /= static_cast<double>(tv.size());
    media_b /= static_cast<double>(tv.size());
    const double beta = normalizza_360(vicino->rilevamento + media_b - p->valore * media_t);   // la retta a t = t_cpa
    // il moto proprio e la posizione propria al CPA (lineare fra i due rilevamenti attorno)
    const double dt = b.tempo - a.tempo;
    const Vec2 v_propria{(b.proprio.x - a.proprio.x) / dt, (b.proprio.y - a.proprio.y) / dt};
    Vec2 proprio_cpa = a.proprio;
    for (std::size_t i = 1; i < ril.size(); ++i)
        if (ril[i].tempo >= tc) {
            const Rilevamento& x = ril[i - 1];
            const Rilevamento& y = ril[i];
            const double u = y.tempo > x.tempo ? (tc - x.tempo) / (y.tempo - x.tempo) : 0.0;
            proprio_cpa = {x.proprio.x + u * (y.proprio.x - x.proprio.x), x.proprio.y + u * (y.proprio.y - x.proprio.y)};
            break;
        }
    // il bersaglio al CPA e la sua velocita': quella propria piu' la relativa, perpendicolare nel verso del rateo
    const double rb = radianti(beta);
    const Vec2 bersaglio_cpa{proprio_cpa.x + misura.distanza_cpa_m * std::sin(rb), proprio_cpa.y + misura.distanza_cpa_m * std::cos(rb)};
    const double verso = radianti(beta + (p->valore >= 0.0 ? 90.0 : -90.0));
    const Vec2 v_bersaglio{v_propria.x + misura.velocita_relativa_m_s * std::sin(verso),
                           v_propria.y + misura.velocita_relativa_m_s * std::cos(verso)};
    const double dopo = b.tempo - tc;
    const Vec2 bersaglio{bersaglio_cpa.x + v_bersaglio.x * dopo, bersaglio_cpa.y + v_bersaglio.y * dopo};
    SoluzioneDaCpa out;
    out.soluzione.distanza_m = distanza(b.proprio, bersaglio);
    const double vb = std::hypot(v_bersaglio.x, v_bersaglio.y);
    out.soluzione.velocita_m_s = vb;
    out.soluzione.rotta = vb > 1e-6 ? normalizza_360(gradi(std::atan2(v_bersaglio.x, v_bersaglio.y))) : 0.0;
    out.sigma_velocita_m_s = misura.sigma_velocita_m_s;
    out.sigma_distanza_m = std::hypot(misura.sigma_distanza_m, misura.sigma_velocita_m_s * dopo);
    out.sigma_rotta_deg = gradi(misura.sigma_velocita_m_s / std::max(vb, 0.5));
    return out;
}

// --- pendenze e FRAZ ---

std::optional<Pendenza> pendenza(const std::vector<std::pair<double, double>>& tv) {
    const std::size_t n = tv.size();
    if (n < 3) return std::nullopt;
    double mt = 0.0, my = 0.0;
    for (const auto& [t, y] : tv) { mt += t; my += y; }
    mt /= static_cast<double>(n);
    my /= static_cast<double>(n);
    double sxx = 0.0, sxy = 0.0;
    for (const auto& [t, y] : tv) { sxx += (t - mt) * (t - mt); sxy += (t - mt) * (y - my); }
    if (sxx <= 0.0) return std::nullopt;
    Pendenza p;
    p.valore = sxy / sxx;
    double ssr = 0.0;
    for (const auto& [t, y] : tv) ssr += std::pow(y - my - p.valore * (t - mt), 2.0);
    p.sigma = std::sqrt(ssr / static_cast<double>(n - 2) / sxx);
    p.punti = static_cast<int>(n);
    return p;
}

StimaFraz distanza_fraz(double f0_hz, const Pendenza& df_dt, const Pendenza& rateo, bool manovra) {
    StimaFraz s;
    const double b = radianti(rateo.valore), sb = radianti(rateo.sigma);
    if (manovra) s.motivo = "manovra nella finestra: il moto relativo non e' rettilineo";
    else if (f0_hz <= 0.0) s.motivo = "frequenza a riposo sconosciuta";
    else if (std::fabs(b) < 3.0 * sb || b == 0.0) s.motivo = "rateo di rilevamento troppo piccolo";
    else if (df_dt.valore >= 0.0 || -df_dt.valore < 3.0 * df_dt.sigma) s.motivo = "la frequenza non scende abbastanza";
    if (!s.motivo.empty()) return s;
    s.distanza_m = -C_SUONO_MISURE * df_dt.valore / (f0_hz * b * b);
    const double rel = std::sqrt(std::pow(df_dt.sigma / df_dt.valore, 2.0) + std::pow(2.0 * sb / b, 2.0));
    s.sigma_m = s.distanza_m * rel;
    if (rel > 0.5) s.motivo = "stima troppo incerta";
    s.valida = s.motivo.empty();
    return s;
}

StimaFraz distanza_fraz(double f0_hz, const std::vector<PuntoFrequenza>& frequenze, const std::vector<Rilevamento>& rilevamenti,
                        bool manovra) {
    std::vector<std::pair<double, double>> f, r;
    for (const auto& x : frequenze) f.emplace_back(x.t, x.hz);
    double base = rilevamenti.empty() ? 0.0 : rilevamenti.front().rilevamento;
    for (const auto& x : rilevamenti) r.emplace_back(x.tempo, base + normalizza_180(x.rilevamento - base));   // srotolati
    const auto pf = pendenza(f), pr = pendenza(r);
    if (!pf || !pr) {
        StimaFraz s;
        s.motivo = "troppo pochi punti";
        return s;
    }
    return distanza_fraz(f0_hz, *pf, *pr, manovra);
}

}  // namespace m688
