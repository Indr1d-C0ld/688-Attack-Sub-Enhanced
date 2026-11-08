#include "nucleo/tma_ia.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <vector>

#include "nucleo/geometria.hpp"

namespace m688 {

namespace {

constexpr double DISTANZA_MIN_M = 300.0, DISTANZA_MAX_M = 60000.0;
constexpr int PASSI_DISTANZA = 28, PASSI_ROTTA = 24;
constexpr std::array<double, 9> VELOCITA_M_S = {0.0, 1.5, 2.5, 3.5, 5.0, 6.5, 8.0, 10.0, 13.0};   // 0-25 kn
constexpr int ITERAZIONI_LM = 30;
constexpr double VELOCITA_MAX_M_S = 18.0;   // 35 kn: oltre, una soluzione non e' un bersaglio
// la distanza e' compatibile se il chi quadro dei rilevamenti sale di al piu' tanto sopra il minimo: N (s^2 - s_min^2)
// / sigma^2 <= 1, l'intervallo a 1 sigma del profilo (il rumore e' il maggiore fra l'errore del sensore e lo scarto
// minimo, se il modello rettilineo non spiega tutto). Prima la soglia era un quarto dell'errore sopra il minimo, senza
// contare i rilevamenti: con 90 rilevamenti l'intervallo era cinque volte troppo largo e la distanza quasi mai osservabile
constexpr double DELTA_CHI_QUADRO = 1.0;
// la distanza e' determinata se l'intervallo a 2 sigma (chi quadro + 4) sta in un rapporto max/min di 2,5. Prima era
// quello a 1 sigma in 1,6: su 300 scenari a caso (una o due gambe, rumori da 0,5 a 3 gradi) il 14% delle soluzioni
// «osservabili» sbagliava la distanza di oltre la meta' (un Charlie II la dava a 1 km invece di 11 e il P-120 non
// partiva); cosi' e' il 4%, con il 10% di soluzioni buone in meno (29/10/2026). La sigma resta quella a 1 sigma
constexpr double DELTA_CHI_OSSERVABILE = 4.0;
constexpr double RAPPORTO_OSSERVABILE = 2.5;
// lo scarto massimo di una soluzione osservabile, in errori del sensore: oltre, il moto rettilineo non spiega i
// rilevamenti (il bersaglio ha manovrato) e la distanza del profilo non vale. Allora si riprova sui soli rilevamenti
// recenti, prima gli ultimi 20 minuti e poi gli ultimi 10 (STIMA: due o tre gambe della manovra propria)
constexpr std::array<double, 2> FINESTRE_TMA_S = {1200.0, 600.0};
constexpr double SCARTO_OSSERVABILE = 3.0;

// scarto quadratico medio dei rilevamenti (gradi) per la soluzione, senza allocare. Con `limite`: appena la somma
// parziale dice gia' uno scarto >= limite si ferma e da' limite (la somma non cala e la radice e' monotona, quindi il
// confronto con limite esce uguale, bit per bit; nella griglia la maggior parte dei candidati si scarta presto)
// Le osservazioni di rotta (periscopio, Tma::rotte) contano come termini in piu': (rotta della soluzione - osservata)
// x sigma dei rilevamenti / sigma dell'osservazione, cioe' nelle unita' dei rilevamenti. Anche a velocita' quasi nulla:
// chi ha visto la prua al periscopio sa che il bersaglio ha una rotta, e smorzarla lasciava scappare il risolutore
// verso soluzioni ferme e lontane. Senza osservazioni tutto resta come prima, bit per bit
double peso_rotta(const OsservazioneRotta& o, double sigma_ril) { return std::max(sigma_ril, 0.1) / std::max(o.sigma_gradi, 1.0); }

double scarto(const std::vector<Rilevamento>& r, const std::vector<OsservazioneRotta>& o, double sigma_ril, const SoluzioneTma& s,
              double limite = std::numeric_limits<double>::infinity()) {
    const Rilevamento& u = r.back();
    const double b = radianti(u.rilevamento), c = radianti(s.rotta);
    const double x0 = u.proprio.x + s.distanza_m * std::sin(b), y0 = u.proprio.y + s.distanza_m * std::cos(b);
    const double vx = s.velocita_m_s * std::sin(c), vy = s.velocita_m_s * std::cos(c);
    const double n = static_cast<double>(r.size() + o.size());
    double q = 0.0;
    std::size_t k = 0;
    for (const auto& x : r) {
        const double dt = x.tempo - u.tempo;
        const Vec2 p{x0 + vx * dt, y0 + vy * dt};
        const double d = normalizza_180(rilevamento(x.proprio, p) - x.rilevamento);
        q += d * d;
        if ((++k & 7u) == 0 && std::sqrt(q / n) >= limite) return limite;
    }
    for (const auto& x : o) {
        const double d = normalizza_180(s.rotta - x.rotta) * peso_rotta(x, sigma_ril);
        q += d * d;
    }
    return std::sqrt(q / n);
}

double distanza_griglia(int k) {
    return DISTANZA_MIN_M * std::pow(DISTANZA_MAX_M / DISTANZA_MIN_M, static_cast<double>(k) / (PASSI_DISTANZA - 1));
}

// il miglior scarto a distanza fissa (rotta e velocita' libere), partendo dalla griglia e raffinando
double migliore_a_distanza(const std::vector<Rilevamento>& r, const std::vector<OsservazioneRotta>& o, double sigma_ril,
                           double distanza, SoluzioneTma* fuori) {
    SoluzioneTma best{distanza, 0.0, 0.0};
    double e = std::numeric_limits<double>::max();
    for (int i = 0; i < PASSI_ROTTA; ++i)
        for (const double v : VELOCITA_M_S) {
            const SoluzioneTma s{distanza, 360.0 * i / PASSI_ROTTA, v};
            if (const double x = scarto(r, o, sigma_ril, s, e); x < e) e = x, best = s;
        }
    // poi Levenberg-Marquardt sulla velocita' in componenti e sull'errore dell'ultimo rilevamento, dove a distanza fissa
    // il problema e' quasi quadratico. Prima un raffinamento per coordinate su rotta e velocita' si fermava nelle valli
    // curve, e il profilo della distanza veniva frastagliato: su una gamba sola il profilo vero e' piatto (le soluzioni
    // in scala danno gli stessi rilevamenti) e un dente sembrava un minimo stretto, una distanza «osservabile» sbagliata.
    // E la traccia era appesa all'ultimo rilevamento misurato, col suo errore: le soluzioni a due gambe uscivano spesso
    // dalle loro sigma
    const Rilevamento& u = r.back();
    const double b0 = radianti(u.rilevamento);
    std::array<double, 3> x{best.velocita_m_s * std::sin(radianti(best.rotta)), best.velocita_m_s * std::cos(radianti(best.rotta)), 0.0};
    const auto residui = [&](const std::array<double, 3>& v, std::vector<double>& out) {
        out.clear();
        const Vec2 p0{u.proprio.x + distanza * std::sin(b0 + v[2]), u.proprio.y + distanza * std::cos(b0 + v[2])};
        double q = 0.0;
        for (const auto& m : r) {
            const double dt = m.tempo - u.tempo;
            const double d = normalizza_180(rilevamento(m.proprio, {p0.x + v[0] * dt, p0.y + v[1] * dt}) - m.rilevamento);
            out.push_back(d);
            q += d * d;
        }
        if (!o.empty()) {
            const double rotta = gradi(std::atan2(v[0], v[1]));
            for (const auto& m : o) {
                const double d = normalizza_180(rotta - m.rotta) * peso_rotta(m, sigma_ril);
                out.push_back(d);
                q += d * d;
            }
        }
        return q;
    };
    std::vector<double> r0, rn;
    std::array<std::vector<double>, 3> rj;
    double q = residui(x, r0), lambda = 1e-3;
    constexpr std::array<double, 3> H = {1e-3, 1e-3, 1e-6};   // m/s, m/s, radianti: per le derivate
    for (int k = 0; k < ITERAZIONI_LM; ++k) {
        for (std::size_t j = 0; j < 3; ++j) {
            std::array<double, 3> y = x;
            y[j] += H[j];
            residui(y, rj[j]);
        }
        std::array<std::array<double, 3>, 3> jtj{};
        std::array<double, 3> g{};
        for (std::size_t i = 0; i < r0.size(); ++i) {
            const std::array<double, 3> d = {(rj[0][i] - r0[i]) / H[0], (rj[1][i] - r0[i]) / H[1], (rj[2][i] - r0[i]) / H[2]};
            for (std::size_t m = 0; m < 3; ++m) {
                g[m] += d[m] * r0[i];
                for (std::size_t n = 0; n < 3; ++n) jtj[m][n] += d[m] * d[n];
            }
        }
        bool fatto = false;
        while (!fatto && lambda < 1e8) {
            // (JtJ + lambda diag) passo = -g, con Cramer
            auto m = jtj;
            for (std::size_t j = 0; j < 3; ++j) m[j][j] = m[j][j] * (1.0 + lambda) + 1e-12;
            const auto det3 = [](const std::array<std::array<double, 3>, 3>& a) {
                return a[0][0] * (a[1][1] * a[2][2] - a[1][2] * a[2][1]) - a[0][1] * (a[1][0] * a[2][2] - a[1][2] * a[2][0]) +
                       a[0][2] * (a[1][0] * a[2][1] - a[1][1] * a[2][0]);
            };
            const double det = det3(m);
            if (!(std::fabs(det) > 1e-30)) break;
            std::array<double, 3> y = x;
            for (std::size_t j = 0; j < 3; ++j) {
                auto mj = m;
                for (std::size_t i = 0; i < 3; ++i) mj[i][j] = -g[i];
                y[j] += det3(mj) / det;
            }
            if (const double v = std::hypot(y[0], y[1]); v > VELOCITA_MAX_M_S) y[0] *= VELOCITA_MAX_M_S / v, y[1] *= VELOCITA_MAX_M_S / v;
            const double qn = residui(y, rn);
            if (qn < q) {
                const bool fermo = q - qn < 1e-12 * std::max(q, 1.0);
                x = y, q = qn, r0.swap(rn), lambda = std::max(lambda * 0.3, 1e-9), fatto = true;
                if (fermo) k = ITERAZIONI_LM;
            } else {
                lambda *= 10.0;
            }
        }
        if (!fatto) break;
    }
    if (const double e_lm = std::sqrt(q / static_cast<double>(r.size() + o.size())); e_lm < e) {
        e = e_lm;
        // la soluzione resta nel modello della Tma, lungo l'ultimo rilevamento: la proiezione del punto stimato (lo scarto
        // trasversale, distanza x errore del rilevamento, e' dentro sigma_trasversale)
        best.distanza_m = distanza * std::cos(x[2]);
        best.velocita_m_s = std::hypot(x[0], x[1]);
        best.rotta = best.velocita_m_s > 0.0 ? normalizza_360(gradi(std::atan2(x[0], x[1]))) : 0.0;
    }
    if (fuori) *fuori = best;
    return e;
}

// la soluzione sui rilevamenti `r` e le rotte `o` (un tratto di quelli della Tma, che finisce col suo ultimo)
std::optional<StimaTma> risolvi_su(const Tma& tma, const std::vector<Rilevamento>& r, const std::vector<OsservazioneRotta>& o,
                                   double t, double sigma_deg) {
    if (static_cast<int>(r.size()) < RILEVAMENTI_MIN_TMA || r.back().tempo - r.front().tempo < ARCO_MIN_TMA_S) return std::nullopt;
    // il profilo dello scarto lungo la distanza: il minimo e l'intervallo compatibile
    std::array<double, PASSI_DISTANZA> profilo{};
    std::array<SoluzioneTma, PASSI_DISTANZA> soluzioni{};
    int k0 = 0;
    for (int k = 0; k < PASSI_DISTANZA; ++k) {
        profilo[static_cast<std::size_t>(k)] = migliore_a_distanza(r, o, sigma_deg, distanza_griglia(k), &soluzioni[static_cast<std::size_t>(k)]);
        if (profilo[static_cast<std::size_t>(k)] < profilo[static_cast<std::size_t>(k0)]) k0 = k;
    }
    // raffinamento della distanza attorno al minimo (sezione aurea fra i vicini della griglia)
    double a = distanza_griglia(std::max(0, k0 - 1)), b = distanza_griglia(std::min(PASSI_DISTANZA - 1, k0 + 1));
    SoluzioneTma best = soluzioni[static_cast<std::size_t>(k0)];
    double e = profilo[static_cast<std::size_t>(k0)];
    constexpr double AUREA = 0.6180339887;
    for (int k = 0; k < 30 && b - a > 5.0; ++k) {
        const double c = b - AUREA * (b - a), d = a + AUREA * (b - a);
        SoluzioneTma sc, sd;
        const double ec = migliore_a_distanza(r, o, sigma_deg, c, &sc), ed = migliore_a_distanza(r, o, sigma_deg, d, &sd);
        if (ec < ed) { b = d; if (ec < e) e = ec, best = sc; }
        else { a = c; if (ed < e) e = ed, best = sd; }
    }
    // l'intervallo di distanze compatibili attorno al minimo, coi bordi interpolati in log fra i punti della griglia; un
    // altro tratto compatibile staccato (un secondo minimo) rende la distanza ambigua e allarga l'intervallo
    const double rumore = std::max({sigma_deg, e, 0.1});
    const double n = static_cast<double>(r.size() + o.size());
    const auto chi = [&](double scarto_deg) { return n * (scarto_deg * scarto_deg - e * e) / (rumore * rumore); };
    const auto log_d = [](int k) { return std::log(distanza_griglia(k)); };
    struct Intervallo { double lo, hi; bool ambigua; };
    const auto intervallo = [&](double soglia) {
        int a_k = k0, b_k = k0;
        while (a_k > 0 && chi(profilo[static_cast<std::size_t>(a_k - 1)]) <= soglia) --a_k;
        while (b_k + 1 < PASSI_DISTANZA && chi(profilo[static_cast<std::size_t>(b_k + 1)]) <= soglia) ++b_k;
        const auto bordo = [&](int dentro, int fuori) {   // il punto fra due nodi dove il chi quadro passa la soglia
            const double c_in = std::max(0.0, chi(profilo[static_cast<std::size_t>(dentro)]));
            const double c_out = chi(profilo[static_cast<std::size_t>(fuori)]);
            const double f = c_out > c_in ? std::clamp((soglia - c_in) / (c_out - c_in), 0.0, 1.0) : 1.0;
            return std::exp(log_d(dentro) + f * (log_d(fuori) - log_d(dentro)));
        };
        Intervallo x{a_k > 0 ? bordo(a_k, a_k - 1) : DISTANZA_MIN_M, b_k + 1 < PASSI_DISTANZA ? bordo(b_k, b_k + 1) : DISTANZA_MAX_M, false};
        x.lo = std::min(x.lo, best.distanza_m);
        x.hi = std::max(x.hi, best.distanza_m);
        for (int k = 0; k < PASSI_DISTANZA; ++k)
            if ((k < a_k - 1 || k > b_k + 1) && chi(profilo[static_cast<std::size_t>(k)]) <= soglia) {
                x.ambigua = true;
                x.lo = std::min(x.lo, distanza_griglia(k));
                x.hi = std::max(x.hi, distanza_griglia(k));
            }
        return x;
    };
    const Intervallo uno = intervallo(DELTA_CHI_QUADRO), due = intervallo(DELTA_CHI_OSSERVABILE);
    StimaTma out;
    out.soluzione = best;
    out.scarto_deg = e;
    out.posizione = *tma.posizione(best, t);
    out.sigma_distanza_m = 0.5 * (uno.hi - uno.lo);
    // un minimo sul bordo della ricerca non e' una soluzione: vicinissimo al battello ci sono sempre rotte e velocita' che
    // imitano i rilevamenti, e il rumore puo' farle preferire
    // e cosi' una soluzione schiacciata sul limite della velocita': la distanza scala con la velocita' (le soluzioni in
    // scala danno gli stessi rilevamenti), e il limite la ferma dove capita (l'Oscar della SCENE9C: 35 kn invece di 14, la
    // distanza corta di un terzo)
    const bool sul_bordo = k0 == 0 || k0 == PASSI_DISTANZA - 1 || best.distanza_m < 1.05 * DISTANZA_MIN_M ||
                           best.velocita_m_s >= 0.98 * VELOCITA_MAX_M_S;
    out.osservabile = !due.ambigua && !sul_bordo && due.hi / std::max(due.lo, 1.0) <= RAPPORTO_OSSERVABILE && due.hi < DISTANZA_MAX_M * 0.99 &&
                      e <= SCARTO_OSSERVABILE * std::max(sigma_deg, 0.5);
    const double dist_t = distanza(r.back().proprio, out.posizione);
    out.sigma_trasversale_m = dist_t * radianti(std::max(sigma_deg, 0.1));
    return out;
}

}  // namespace

std::optional<StimaTma> risolvi_tma(const Tma& tma, double t, double sigma_deg) {
    // tutti i rilevamenti, se il moto rettilineo li spiega; se no il bersaglio ha manovrato, e si riprova sul tratto
    // recente (il runtime, 31/10/2026: l'Oscar della SCENE9C teneva 45 minuti di rilevamenti, con uno scarto di 7-10
    // gradi, e dava per «osservabile» un bersaglio fermo a 15 km dal vero)
    auto st = risolvi_su(tma, tma.rilevamenti(), tma.rotte(), t, sigma_deg);
    const auto coerente = [&](const std::optional<StimaTma>& x) { return x && x->scarto_deg <= SCARTO_OSSERVABILE * std::max(sigma_deg, 0.5); };
    for (const double finestra : FINESTRE_TMA_S) {
        if (!st || coerente(st) || tma.rilevamenti().empty()) break;
        const double da = tma.rilevamenti().back().tempo - finestra;
        std::vector<Rilevamento> r;
        std::vector<OsservazioneRotta> o;
        for (const auto& x : tma.rilevamenti()) if (x.tempo >= da) r.push_back(x);
        for (const auto& x : tma.rotte()) if (x.tempo >= da) o.push_back(x);
        if (static_cast<int>(r.size()) == static_cast<int>(tma.rilevamenti().size())) continue;
        if (auto y = risolvi_su(tma, r, o, t, sigma_deg)) st = y;
        else break;
    }
    return st;
}

}  // namespace m688
