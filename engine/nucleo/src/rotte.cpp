#include "nucleo/rotte.hpp"

#include <algorithm>
#include <cmath>
#include <limits>
#include <queue>

namespace m688 {

AcqueNavigabili::AcqueNavigabili(const Fondale& f, double minima)
    : f_(f), minima_(minima), comp_(static_cast<std::size_t>(f.larghezza()) * f.altezza(), -1) {
    const int W = f.larghezza(), H = f.altezza();
    // -2: acqua sufficiente non ancora visitata; poi riempimento a pila (8 vicini) componente per componente
    for (int r = 0; r < H; ++r)
        for (int c = 0; c < W; ++c)
            if (f.profondita_cella(c, r) >= minima) comp_[static_cast<std::size_t>(r) * W + c] = -2;
    std::int32_t n = 0;
    std::vector<int> pila;
    for (int i = 0; i < W * H; ++i) {
        if (comp_[i] != -2) continue;
        comp_[i] = n;
        dimensioni_.push_back(1);
        pila.push_back(i);
        while (!pila.empty()) {
            const int k = pila.back();
            pila.pop_back();
            const int c = k % W, r = k / W;
            for (int rr = std::max(r - 1, 0); rr <= std::min(r + 1, H - 1); ++rr)
                for (int cc = std::max(c - 1, 0); cc <= std::min(c + 1, W - 1); ++cc) {
                    const int j = rr * W + cc;
                    if (comp_[j] != -2) continue;
                    comp_[j] = n;
                    ++dimensioni_[n];
                    pila.push_back(j);
                }
        }
        ++n;
    }
}

bool AcqueNavigabili::piu_vicina(int& col, int& riga, int c, int celle_minime) const {
    // anelli quadrati di raggio crescente; in ogni anello la cella piu' vicina in linea d'aria
    const int W = f_.larghezza(), H = f_.altezza();
    auto va_bene = [&](int x, int y) {
        const int k = componente(x, y);
        return k >= 0 && (c < 0 ? dimensioni_[k] >= celle_minime : k == c);
    };
    if (va_bene(col, riga)) return true;
    for (int raggio = 1; raggio < std::max(W, H); ++raggio) {
        int bx = -1, by = -1;
        double bd = std::numeric_limits<double>::infinity();
        for (int y = riga - raggio; y <= riga + raggio; ++y) {
            if (y < 0 || y >= H) continue;
            const bool bordo = y == riga - raggio || y == riga + raggio;
            for (int x = col - raggio; x <= col + raggio; x += bordo ? 1 : 2 * raggio) {
                if (x < 0 || x >= W || !va_bene(x, y)) continue;
                const double d = std::hypot(x - col, y - riga);
                if (d < bd) { bd = d; bx = x; by = y; }
            }
        }
        if (bx >= 0) { col = bx; riga = by; return true; }
    }
    return false;
}

bool tratto_libero(const Fondale& f, Vec2 da, Vec2 a, double soglia, double passo_m) {
    const double d = distanza(da, a);
    const int n = std::max(1, static_cast<int>(std::ceil(d / passo_m)));
    for (int i = 1; i <= n; ++i) {
        const double t = static_cast<double>(i) / n;
        if (f.profondita({da.x + (a.x - da.x) * t, da.y + (a.y - da.y) * t}) < soglia) return false;
    }
    return true;
}

namespace {

struct Ricerca {
    std::vector<Vec2> punti;      // centri delle celle dalla partenza al punto raggiunto
    std::vector<double> acqua;    // profondita' delle celle
    bool arrivata = false;
};

// A* nel riquadro delle celle (c0, r0) e (c1, r1) allargato di m celle per lato
Ricerca cerca(const AcqueNavigabili& acque, int c0, int r0, int c1, int r1, int m, double comoda) {
    const Fondale& f = acque.fondale();
    const int x0 = std::max(std::min(c0, c1) - m, 0), x1 = std::min(std::max(c0, c1) + m, f.larghezza() - 1);
    const int y0 = std::max(std::min(r0, r1) - m, 0), y1 = std::min(std::max(r0, r1) + m, f.altezza() - 1);
    const int W = x1 - x0 + 1, H = y1 - y0 + 1;
    const auto indice = [&](int c, int r) { return (r - y0) * W + (c - x0); };
    std::vector<float> g(static_cast<std::size_t>(W) * H, std::numeric_limits<float>::infinity());
    std::vector<std::int32_t> padre(g.size(), -1);
    std::vector<std::uint8_t> chiuso(g.size(), 0);
    const double cella = f.cella_m();
    const auto stima = [&](int c, int r) { return std::hypot(c - c1, r - r1) * cella; };
    using Voce = std::pair<double, std::int32_t>;
    std::priority_queue<Voce, std::vector<Voce>, std::greater<>> aperti;
    const int partenza = indice(c0, r0), arrivo = indice(c1, r1);
    g[partenza] = 0.0f;
    aperti.push({stima(c0, r0), partenza});
    int migliore = partenza;
    double h_migliore = stima(c0, r0);
    while (!aperti.empty()) {
        const auto [fv, i] = aperti.top();
        aperti.pop();
        if (chiuso[i]) continue;
        chiuso[i] = 1;
        const int c = i % W + x0, r = i / W + y0;
        if (const double h = stima(c, r); h < h_migliore) { h_migliore = h; migliore = i; }
        if (i == arrivo) break;
        for (int dr = -1; dr <= 1; ++dr)
            for (int dc = -1; dc <= 1; ++dc) {
                if (!dc && !dr) continue;
                const int cc = c + dc, rr = r + dr;
                if (cc < x0 || cc > x1 || rr < y0 || rr > y1) continue;
                const int j = indice(cc, rr);
                if (chiuso[j] || acque.componente(cc, rr) < 0) continue;
                const double p = f.profondita_cella(cc, rr);
                const double passo = (dc && dr ? std::sqrt(2.0) : 1.0) * cella * (p < comoda ? 4.0 : 1.0);
                const double gn = g[i] + passo;
                if (gn < g[j]) {
                    g[j] = static_cast<float>(gn);
                    padre[j] = i;
                    aperti.push({gn + stima(cc, rr), j});
                }
            }
    }
    Ricerca esito;
    esito.arrivata = migliore == arrivo;
    for (int i = migliore; i != -1; i = padre[i]) {
        const int c = i % W + x0, r = i / W + y0;
        esito.punti.push_back(f.centro_cella(c, r));
        esito.acqua.push_back(f.profondita_cella(c, r));
    }
    std::reverse(esito.punti.begin(), esito.punti.end());
    std::reverse(esito.acqua.begin(), esito.acqua.end());
    return esito;
}

}  // namespace

Rotta rotta_navigabile(const AcqueNavigabili& acque, Vec2 da, Vec2 a, double comoda) {
    const Fondale& f = acque.fondale();
    int c0, r0;
    if (!f.cella(da, c0, r0)) return {};
    // la componente della partenza: quella della cella, o della cella navigabile piu' vicina (da una secca si esce)
    int cp = c0, rp = r0;
    if (!acque.piu_vicina(cp, rp, -1)) return {};
    const int componente = acque.componente(cp, rp);
    // l'arrivo fuori dalla griglia si porta sul bordo (oltre c'e' acqua senza fondo); se non e' nelle stesse acque
    // della partenza, si va nel punto piu' vicino che lo e'
    const double mx = f.larghezza() * f.cella_m() / 2.0 - 1.0, my = f.altezza() * f.cella_m() / 2.0 - 1.0;
    Vec2 arrivo{std::clamp(a.x, -mx, mx), std::clamp(a.y, -my, my)};
    int c1, r1;
    f.cella(arrivo, c1, r1);
    Rotta rotta;
    rotta.completa = acque.componente(c1, r1) == componente;
    if (!rotta.completa) {
        acque.piu_vicina(c1, r1, componente);
        arrivo = f.centro_cella(c1, r1);
    }
    // riquadro di ricerca: partenza e arrivo con un margine di 5 km o del 30% della distanza; se l'acqua passa piu'
    // lontano (fra due fiordi, attorno a un'isola) il margine si quadruplica fino a coprire tutto il teatro
    int m = static_cast<int>(std::max(5000.0, 0.3 * distanza(da, arrivo)) / f.cella_m());
    Ricerca r = cerca(acque, c0, r0, c1, r1, m, comoda);
    while (!r.arrivata && m < std::max(f.larghezza(), f.altezza())) {
        m *= 4;
        r = cerca(acque, c0, r0, c1, r1, m, comoda);
    }
    rotta.completa = rotta.completa && r.arrivata;
    // taglio a vista: da ogni punto si salta al piu' lontano visibile con acqua non peggiore di quella che la
    // rotta a celle attraversa in quel tratto
    auto& punti = r.punti;
    punti.front() = da;
    if (r.arrivata && f.profondita(arrivo) >= acque.minima()) punti.back() = arrivo;
    std::size_t k = 0;
    while (k + 1 < punti.size()) {
        std::size_t scelto = k + 1;
        double peggiore = std::min(comoda, r.acqua[k + 1]);
        for (std::size_t j = k + 2; j < punti.size(); ++j) {
            peggiore = std::min(peggiore, r.acqua[j]);
            if (tratto_libero(f, punti[k], punti[j], std::max(peggiore, acque.minima()), f.cella_m() / 4.0)) scelto = j;
            else if (j > scelto + 12) break;   // oltre una dozzina di celle nascoste non si cerca piu'
        }
        rotta.punti.push_back(punti[scelto]);
        k = scelto;
    }
    if (rotta.completa && (a.x != arrivo.x || a.y != arrivo.y)) rotta.punti.push_back(a);
    return rotta;
}

}  // namespace m688
