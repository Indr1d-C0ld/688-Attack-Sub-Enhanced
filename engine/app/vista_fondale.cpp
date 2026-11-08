#include "vista_fondale.hpp"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <string>
#include <vector>

#include "grafica/carta_vga.hpp"
#include "grafica/lamiera.hpp"
#include "grafica/quadro.hpp"
#include "grafica/palette.hpp"
#include "grafica/tema_stazioni.hpp"

namespace m688 {

namespace {

// soglie di Bayer 4x4 in (0, 1), per la confidenza come retinatura
double bayer(int x, int y) {
    static constexpr int M[4][4] = {{0, 8, 2, 10}, {12, 4, 14, 6}, {3, 11, 1, 9}, {15, 7, 13, 5}};
    return (M[y & 3][x & 3] + 0.5) / 16.0;
}

void linea(Tela& t, Vec2 a, Vec2 b, std::uint8_t c, bool tratti = false) {
    const int n = static_cast<int>(std::max(std::fabs(b.x - a.x), std::fabs(b.y - a.y)));
    for (int i = 0; i <= n; ++i) {
        if (tratti && (i / 3) % 2) continue;
        const double s = n ? static_cast<double>(i) / n : 0.0;
        t.pixel(static_cast<int>(std::lround(a.x + (b.x - a.x) * s)), static_cast<int>(std::lround(a.y + (b.y - a.y) * s)), c);
    }
}

void cornice(Tela& t, Rettangolo r, std::uint8_t c) {
    t.linea_orizzontale(r.x, r.x + r.w - 1, r.y, c);
    t.linea_orizzontale(r.x, r.x + r.w - 1, r.y + r.h - 1, c);
    t.linea_verticale(r.x, r.y, r.y + r.h - 1, c);
    t.linea_verticale(r.x + r.w - 1, r.y, r.y + r.h - 1, c);
}

// la scala delle profondita': il massimo per 1,15, arrotondato a 50 m, almeno 50
double scala_50(double massimo) { return std::max(50.0, std::ceil(massimo * 1.15 / 50.0) * 50.0); }

std::string metri(double m) { return std::to_string(static_cast<int>(std::lround(m))) + " M"; }

}  // namespace

void disegna_misure_carta(Tela& t, const VistaCarta& v, const QuadroFondale& q, double tempo) {
    namespace cc = colori_carta_vga;
    const Rettangolo prima = t.ritaglio();
    t.imposta_ritaglio({v.x, v.y, v.w, v.h});
    std::vector<std::uint8_t> misurato(static_cast<std::size_t>(std::max(0, v.w * v.h)), 0);
    const double c = q.cella_m(), lato = c / v.metri_per_pixel;
    std::vector<Vec2> ostacoli;
    for (const auto& [col, riga] : q.celle_misurate()) {
        const LetturaCella l = q.leggi(col, riga, tempo);
        if (l.da_carta) continue;
        const Vec2 centro = q.centro_cella(col, riga);
        const Vec2 a = v.a_tela({centro.x - c / 2.0, centro.y + c / 2.0});   // l'angolo in alto a sinistra
        const int x0 = static_cast<int>(std::floor(a.x)), y0 = static_cast<int>(std::floor(a.y));
        const int x1 = std::max(x0 + 1, static_cast<int>(std::floor(a.x + lato))), y1 = std::max(y0 + 1, static_cast<int>(std::floor(a.y + lato)));
        // la confidenza: quanto la misura stringe l'incertezza della carta (almeno un quarto: la misura si vede sempre)
        const double conf = l.sigma_carta_m > 0.0 ? std::clamp(1.0 - l.sigma_m / l.sigma_carta_m, 0.25, 1.0) : 1.0;
        // la fascia della profondita' misurata, retinata verso la successiva come fa la carta (verso la fine della fascia)
        double dentro = 0.0;
        const int fascia = fascia_acqua_vga(l.profondita_m, dentro);
        for (int y = std::max(y0, v.y); y < std::min(y1, v.y + v.h); ++y)
            for (int x = std::max(x0, v.x); x < std::min(x1, v.x + v.w); ++x) {
                misurato[static_cast<std::size_t>((y - v.y) * v.w + (x - v.x))] = 1;
                if (bayer(x, y) < conf) t.pixel(x, y, indice_acqua(fascia < 15 && dentro > bayer(y, x) + 0.35 ? fascia + 1 : fascia, 1));
            }
        if (l.ostacolo) ostacoli.push_back(v.a_tela(centro));
    }
    // «da carta»: una puntinatura rada a matita sull'acqua non misurata (il tratteggio diagonale e' gia' della carta
    // d'epoca, per le zone non rilevate)
    for (int y = v.y + 2; y < v.y + v.h; y += 6)
        for (int x = v.x + 2 + (y / 6 % 2) * 3; x < v.x + v.w; x += 6) {
            if (misurato[static_cast<std::size_t>((y - v.y) * v.w + (x - v.x))]) continue;
            const int p = t.at(x, y);
            if (p >= cc::ACQUA && p < cc::ACQUA + 48) t.pixel(x, y, static_cast<std::uint8_t>(cc::MATITA));
        }
    // il nastro dei sondaggi: un punto per ping (pieno con l'eco, un pixel senza), nella posizione corretta
    for (const Sondaggio& s : q.sondaggi()) {
        const Vec2 p = v.a_tela(s.pos);
        const int x = static_cast<int>(p.x), y = static_cast<int>(p.y);
        if (s.eco) t.rettangolo_pieno(x, y, 2, 2, static_cast<std::uint8_t>(cc::MAGENTA));
        else t.pixel(x, y, static_cast<std::uint8_t>(cc::MAGENTA));
    }
    // gli ostacoli: una croce rossa
    for (const Vec2& o : ostacoli) {
        linea(t, {o.x - 3, o.y - 3}, {o.x + 3, o.y + 3}, colori::ROSSO);
        linea(t, {o.x - 3, o.y + 3}, {o.x + 3, o.y - 3}, colori::ROSSO);
    }
    t.imposta_ritaglio(prima);
}

void disegna_ecoscandaglio(Tela& t, const Font& f, Rettangolo r, const QuadroFondale& q, double tempo, const StatoEcoscandaglio& s,
                           double finestra_s) {
    // la carta termica: chiara, la quadrettatura sottile, i segni bruciati in nero con l'alone della riverberazione
    const std::uint8_t carta = smalto(11), quadretti = smalto(8), alone = smalto(6), nero = colori::NERO;
    const Rettangolo prima = t.ritaglio();
    t.rettangolo_pieno(r.x, r.y, r.w, r.h, carta);
    cornice(t, r, smalto(4));
    char riga[96];
    std::snprintf(riga, sizeof riga, "%s  %s  %s", s.nome.c_str(), s.periodo_min > 0.0 ? ("OGNI " + std::to_string(static_cast<int>(s.periodo_min)) + " MIN").c_str() : "A COMANDO",
                  s.potenza_piena ? "POTENZA PIENA" : "POTENZA BASSA");
    f.scrivi(t, r.x + 4, r.y + 3, riga, nero, 1);
    const Rettangolo p{r.x + 34, r.y + 14, r.w - 38, r.h - 28};
    const double t0 = tempo - finestra_s;
    std::vector<const Sondaggio*> visti;
    double massimo = 0.0;
    for (const Sondaggio& x : q.sondaggi())
        if (x.t >= t0 && x.t <= tempo) {
            visti.push_back(&x);
            if (x.eco) massimo = std::max(massimo, x.sotto_chiglia_m);
        }
    const double scala = scala_50(massimo);
    const auto x_di = [&](double tc) { return p.x + static_cast<int>((tc - t0) / finestra_s * (p.w - 1)); };
    const auto y_di = [&](double m) { return p.y + static_cast<int>(std::clamp(m / scala, 0.0, 1.0) * (p.h - 1)); };
    // la quadrettatura: quattro fasce di profondita' con le etichette, una riga ogni 5 minuti
    for (int k = 0; k <= 4; ++k) {
        const double m = scala * k / 4.0;
        const int y = y_di(m);
        t.linea_orizzontale(p.x, p.x + p.w - 1, y, quadretti);
        const std::string n = std::to_string(static_cast<int>(m));
        f.scrivi(t, p.x - 3 - f.larghezza(n, 1), std::clamp(y - 3, p.y, p.y + p.h - 7), n, nero, 1);
    }
    for (double tc = std::ceil(t0 / 300.0) * 300.0; tc <= tempo; tc += 300.0) t.linea_verticale(x_di(tc), p.y, p.y + p.h - 1, quadretti);
    // i sondaggi: il fondo bruciato, e sotto l'alone; senza eco una tacca in fondo
    t.imposta_ritaglio(p);
    for (const Sondaggio* x : visti) {
        const int xx = x_di(x->t);
        if (!x->eco) {
            t.linea_verticale(xx, p.y + p.h - 4, p.y + p.h - 1, alone);
            continue;
        }
        const int y = y_di(x->sotto_chiglia_m);
        t.rettangolo_pieno(xx - 1, y - 1, 3, 2, nero);
        t.linea_verticale(xx, y + 1, y + 4, alone);
    }
    t.imposta_ritaglio(prima);
    // in fondo l'ultima lettura
    const Sondaggio* ultimo = visti.empty() ? nullptr : visti.back();
    if (!ultimo) std::snprintf(riga, sizeof riga, "NESSUN SONDAGGIO NEGLI ULTIMI %d MIN", static_cast<int>(finestra_s / 60.0));
    else if (!ultimo->eco) std::snprintf(riga, sizeof riga, "ULTIMO PING: NESSUN ECO");
    else std::snprintf(riga, sizeof riga, "SOTTO LA CHIGLIA %s   FONDO %s", metri(ultimo->sotto_chiglia_m).c_str(), metri(ultimo->fondo_m).c_str());
    f.scrivi(t, r.x + 4, r.y + r.h - 11, riga, nero, 1);
}

void disegna_sezione_prua(Tela& t, const Font& f, Rettangolo r, const QuadroFondale& q, Vec2 pos, double rotta, double quota_m,
                          double margine_m, double tempo, double lunghezza_m) {
    const Rettangolo prima = t.ritaglio();
    t.rettangolo_pieno(r.x, r.y, r.w, r.h, fosforo(0));
    cornice(t, r, fosforo(2));
    char riga[96];
    std::snprintf(riga, sizeof riga, "SEZIONE DI PRUA  %.0f KM", lunghezza_m / 1000.0);
    f.scrivi(t, r.x + 4, r.y + 3, riga, fosforo(6), 1);
    const Rettangolo p{r.x + 34, r.y + 14, r.w - 38, r.h - 18};
    const auto prof = q.profilo(pos, rotta, lunghezza_m, lunghezza_m / std::max(8, p.w / 4), tempo);
    double massimo = quota_m + margine_m;
    // la scala sulle profondita' plausibili (fuori dai dati del teatro la carta non ha un numero sensato)
    const auto plausibile = [](double m) { return std::isfinite(m) && m < 11000.0 ? m : 0.0; };
    for (const auto& x : prof) massimo = std::max({massimo, plausibile(x.carta_m), x.misurato ? plausibile(x.stima_m + x.sigma_m) : 0.0});
    const double scala = scala_50(massimo);
    const auto x_di = [&](double d) { return p.x + std::clamp(d / lunghezza_m, 0.0, 1.0) * (p.w - 1); };
    const auto y_di = [&](double m) { return p.y + std::clamp(m / scala, 0.0, 1.0) * (p.h - 1); };
    for (int k = 0; k <= 2; ++k) {   // la scala: superficie, meta', fondo
        const double m = scala * k / 2.0;
        const int y = static_cast<int>(y_di(m));
        for (int x = p.x; x < p.x + p.w; x += 3) t.pixel(x, y, fosforo(2));
        const std::string n = std::to_string(static_cast<int>(m));
        f.scrivi(t, p.x - 3 - f.larghezza(n, 1), std::clamp(y - 3, p.y, p.y + p.h - 7), n, fosforo(4), 1);
    }
    t.imposta_ritaglio(p);
    // la stima misurata con la sua fascia di incertezza, sotto la carta a tratti
    for (std::size_t i = 0; i + 1 < prof.size(); ++i) {
        const auto& a = prof[i];
        const auto& b = prof[i + 1];
        if (a.misurato && b.misurato) {
            for (int x = static_cast<int>(x_di(a.distanza_m)); x <= static_cast<int>(x_di(b.distanza_m)); ++x)
                t.linea_verticale(x, static_cast<int>(y_di(a.stima_m - a.sigma_m)), static_cast<int>(y_di(a.stima_m + a.sigma_m)), fosforo(1));
            linea(t, {x_di(a.distanza_m), y_di(a.stima_m)}, {x_di(b.distanza_m), y_di(b.stima_m)}, fosforo(6));
        }
        linea(t, {x_di(a.distanza_m), y_di(a.carta_m)}, {x_di(b.distanza_m), y_di(b.carta_m)}, fosforo(3), true);
    }
    // i ritorni del BQS-15 degli ultimi 10 minuti, sulla sezione: la distanza lungo la rotta
    const Vec2 dir{std::sin(radianti(rotta)), std::cos(radianti(rotta))};
    for (const RitornoFondo& x : q.ritorni_recenti(tempo, 600.0)) {
        if (!x.eco) continue;
        const double lungo = (x.punto.x - pos.x) * dir.x + (x.punto.y - pos.y) * dir.y;
        if (lungo < 0.0 || lungo > lunghezza_m) continue;
        t.rettangolo_pieno(static_cast<int>(x_di(lungo)) - 1, static_cast<int>(y_di(x.profondita_m)) - 1, 2, 2, x.ostacolo ? std::uint8_t{12} : fosforo(7));
    }
    // la quota propria e il margine
    const int yq = static_cast<int>(y_di(quota_m)), ym = static_cast<int>(y_di(quota_m + margine_m));
    t.linea_orizzontale(p.x, p.x + p.w - 1, yq, colori::VERDE_CHIARO);
    for (int x = p.x; x < p.x + p.w; x += 4) t.linea_orizzontale(x, x + 1, ym, colori::GIALLO);
    t.rettangolo_pieno(p.x, yq - 1, 4, 3, colori::VERDE_CHIARO);   // il battello, a sinistra
    t.imposta_ritaglio(prima);
}

void disegna_ritorni_prospettiva(Tela& t, const Font& f, Rettangolo r, const QuadroFondale& q, Vec2 pos, double rotta,
                                 double quota_m, double tempo, double lunghezza_m) {
    const Rettangolo prima = t.ritaglio();
    t.rettangolo_pieno(r.x, r.y, r.w, r.h, fosforo(0));
    cornice(t, r, fosforo(2));
    char riga[96];
    std::snprintf(riga, sizeof riga, "RITORNI E QUADRO  %.0f KM", lunghezza_m / 1000.0);
    f.scrivi(t, r.x + 4, r.y + 3, riga, fosforo(6), 1);
    f.scrivi(t, r.x + 4, r.y + r.h - 11, "PIENO: MISURATO   A TRATTI: DA CARTA", fosforo(4), 1);
    // la camera: dietro e sopra il battello; la profondita' esagerata 4 volte, l'orizzonte a un quarto dell'altezza
    constexpr int N = 20, M = 16;
    constexpr double ESAGERA = 4.0;
    const double largo = lunghezza_m * 0.75, dietro = lunghezza_m * 0.35;
    const double zc = quota_m - dietro * 0.5;   // la camera sta sopra il battello
    const double k = (r.w / 2.0) * dietro / (largo / 2.0);
    const double cx = r.x + r.w / 2.0, hy = r.y + 14 + (r.h - 28) * 0.15;
    const Vec2 dir{std::sin(radianti(rotta)), std::cos(radianti(rotta))}, destra{dir.y, -dir.x};
    const auto proietta = [&](double lungo, double lato, double z) {
        const double avanti = lungo + dietro;
        return Vec2{cx + lato / avanti * k, hy + (z - zc) * ESAGERA / avanti * k};
    };
    struct Vertice { Vec2 schermo; bool misurato = false; };
    std::vector<Vertice> g(static_cast<std::size_t>((N + 1) * (M + 1)));
    for (int i = 0; i <= N; ++i)
        for (int j = 0; j <= M; ++j) {
            const double lungo = lunghezza_m * i / N, lato = largo * (j - M / 2.0) / M;
            const Vec2 p{pos.x + dir.x * lungo + destra.x * lato, pos.y + dir.y * lungo + destra.y * lato};
            const LetturaCella l = q.leggi(p, tempo);
            g[static_cast<std::size_t>(i * (M + 1) + j)] = {proietta(lungo, lato, l.profondita_m), !l.da_carta};
        }
    t.imposta_ritaglio({r.x + 1, r.y + 13, r.w - 2, r.h - 26});
    // dal fondo verso il battello, cosi' le linee vicine coprono le lontane
    for (int i = N; i >= 0; --i)
        for (int j = 0; j <= M; ++j) {
            const Vertice& a = g[static_cast<std::size_t>(i * (M + 1) + j)];
            const auto tratto = [&](const Vertice& b) {
                const bool pieno = a.misurato && b.misurato;
                linea(t, a.schermo, b.schermo, pieno ? fosforo(6) : fosforo(2), !pieno);
            };
            if (j < M) tratto(g[static_cast<std::size_t>(i * (M + 1) + j + 1)]);
            if (i < N) tratto(g[static_cast<std::size_t>((i + 1) * (M + 1) + j)]);
        }
    // i ritorni del BQS-15 degli ultimi 5 minuti
    for (const RitornoFondo& x : q.ritorni_recenti(tempo, 300.0)) {
        if (!x.eco) continue;
        const Vec2 d{x.punto.x - pos.x, x.punto.y - pos.y};
        const double lungo = d.x * dir.x + d.y * dir.y, lato = d.x * destra.x + d.y * destra.y;
        if (lungo < 0.0 || lungo > lunghezza_m || std::fabs(lato) > largo / 2.0) continue;
        const Vec2 s = proietta(lungo, lato, x.profondita_m);
        t.rettangolo_pieno(static_cast<int>(s.x) - 1, static_cast<int>(s.y) - 1, 2, 2, x.ostacolo ? std::uint8_t{12} : fosforo(7));
    }
    // il battello, alla sua quota
    const Vec2 b = proietta(0.0, 0.0, quota_m);
    t.rettangolo_pieno(static_cast<int>(b.x) - 3, static_cast<int>(b.y) - 1, 7, 3, colori::VERDE_CHIARO);
    t.imposta_ritaglio(prima);
}

}  // namespace m688

// --- la volta del ghiaccio ---

namespace m688 {

namespace {

namespace qd = quadro;

std::string eta_minuti(double s) { return std::to_string(static_cast<int>(std::lround(s / 60.0))) + "'"; }

// il fondo della fascia del ghiaccio (scelta dell'utente del 29/10/2026: due fasce, il ghiaccio ingrandito sopra): il
// ghiaccio piu' profondo e la linea della vela meno il margine, con 5 m d'aria, almeno 20 m, a passi di 5
double fascia_ghiaccio(double ghiaccio_max_m, const BattelloVolta& b) {
    return std::ceil(std::max({ghiaccio_max_m + 5.0, b.cima_vela_m - b.margine_m + 5.0, 20.0}) / 5.0) * 5.0;
}

// la volta a prua in pianta: da 1 km a poppa a 2,6 km a prua, il settore del BQS-15 (±45 gradi, 2,5 km)
void volta_a_prua(Tela& t, const Font& f, Rettangolo a, const QuadroVolta& v, const BattelloVolta& b, double tempo) {
    const Rettangolo prima = t.ritaglio();
    t.imposta_ritaglio(a);
    const double m_px = 3700.0 / std::max(20, a.h - 4);
    const int cx = a.x + a.w / 2, cy = a.y + a.h - 2 - static_cast<int>(1000.0 / m_px);
    const Vec2 avanti{std::sin(radianti(b.rotta)), std::cos(radianti(b.rotta))}, destra{avanti.y, -avanti.x};
    const auto px = [&](double av, double lato) { return Vec2{cx + lato / m_px, cy - av / m_px}; };
    const double soglia = b.cima_vela_m - b.margine_m;
    // le celle del quadro: un quadratino per cella, piu' pieno piu' spesso; rosse le chiglie sotto la vela
    const double c = v.cella_m();
    const double lato_max = (a.w / 2.0) * m_px;
    for (double av = -1000.0; av <= 2700.0; av += c / 2.0)
        for (double la = -lato_max; la <= lato_max; la += c / 2.0) {
            const Vec2 p{b.pos.x + avanti.x * av + destra.x * la, b.pos.y + avanti.y * av + destra.y * la};
            const auto cella = v.leggi(p);
            if (!cella || cella->misure == 0 || cella->pescaggio_max_m <= 0.0) continue;
            const Vec2 s = px(av, la);
            const int lato = std::max(2, static_cast<int>(c / 2.0 / m_px));
            const double pm = cella->pescaggio_max_m;
            const bool ostacolo = pm >= soglia;
            const int tono = std::clamp(2 + static_cast<int>(pm / 5.0), 2, 6);
            for (int y = 0; y < lato; ++y)
                for (int x = 0; x < lato; ++x) {
                    const int X = static_cast<int>(s.x) + x - lato / 2, Y = static_cast<int>(s.y) + y - lato / 2;
                    if (ostacolo) t.pixel(X, Y, (X + Y) % 2 ? std::uint8_t{12} : std::uint8_t{colori::ROSSO});
                    else if (tono > 4 || (X + Y) % 2 == 0) t.pixel(X, Y, fosforo(tono));
                }
        }
    // i ritorni delle chiglie degli ultimi 5 minuti: la tacca e, dai 15 m, il pescaggio (rosso l'ostacolo); i numeri non
    // si accavallano: uno solo per zona di 12 pixel
    std::vector<Vec2> numeri;
    for (const RitornoChiglia& x : v.ritorni_recenti(tempo, 300.0)) {
        const Vec2 d{x.punto.x - b.pos.x, x.punto.y - b.pos.y};
        const Vec2 s = px(d.x * avanti.x + d.y * avanti.y, d.x * destra.x + d.y * destra.y);
        const bool ostacolo = x.ostacolo || x.pescaggio_m >= soglia;
        const std::uint8_t col = ostacolo ? std::uint8_t{12} : x.pescaggio_m >= 15.0 ? fosforo(7) : fosforo(5);
        const int X = static_cast<int>(s.x), Y = static_cast<int>(s.y);
        t.linea_orizzontale(X - 3, X + 3, Y, col);
        t.linea_verticale(X, Y - 1, Y + 1, col);
        if (x.pescaggio_m < 15.0 && !ostacolo) continue;
        if (std::any_of(numeri.begin(), numeri.end(), [&](Vec2 q) { return std::fabs(q.x - s.x) < 40 && std::fabs(q.y - s.y) < 11; })) continue;
        numeri.push_back(s);
        f.scrivi(t, X + 5, Y - 3, std::to_string(static_cast<int>(std::lround(x.pescaggio_m))) + (ostacolo ? " M!" : ""), col, 1);
    }
    // il settore e gli anelli a 1, 2 e 2,5 km; i numeri sul lato sinistro, fuori dagli echi
    for (const double r : {1000.0, 2000.0, 2500.0})
        for (int g = -45; g <= 45; g += 2) {
            const Vec2 s = px(r * std::cos(radianti(g)), r * std::sin(radianti(g)));
            t.pixel(static_cast<int>(s.x), static_cast<int>(s.y), r == 2500.0 ? fosforo(4) : fosforo(2));
        }
    for (const int g : {-45, 45})
        for (double r = 100; r < 2500; r += 60) {
            const Vec2 s = px(r * std::cos(radianti(g)), r * std::sin(radianti(g)));
            t.pixel(static_cast<int>(s.x), static_cast<int>(s.y), fosforo(2));
        }
    for (const double r : {1000.0, 2000.0}) {
        const Vec2 s = px(r * std::cos(radianti(45.0)), -r * std::sin(radianti(45.0)));
        const std::string n = r == 1000.0 ? "1 KM" : "2 KM";
        f.scrivi(t, static_cast<int>(s.x) - f.larghezza(n, 1) - 3, static_cast<int>(s.y) - 3, n, fosforo(3), 1);
    }
    // le polynye candidate dell'ultima ora: il riquadro ciano e l'eta' (le tre piu' recenti)
    int etichette = 0;
    std::vector<int> righe_usate;
    for (const auto& k : v.polynye(b.pos, 4000.0, tempo, 3600.0)) {
        const Vec2 d{k.centro.x - b.pos.x, k.centro.y - b.pos.y};
        const Vec2 s = px(d.x * avanti.x + d.y * avanti.y, d.x * destra.x + d.y * destra.y);
        if (s.y < a.y || s.y > a.y + a.h - 4) continue;
        const int l = std::max(3, static_cast<int>(c / 2.0 / m_px)) + 1;
        const int x0 = static_cast<int>(s.x) - l / 2 - 1, y0 = static_cast<int>(s.y) - l / 2 - 1;
        t.linea_orizzontale(x0, x0 + l + 1, y0, colori::CIANO_CHIARO), t.linea_orizzontale(x0, x0 + l + 1, y0 + l + 1, colori::CIANO_CHIARO);
        t.linea_verticale(x0, y0, y0 + l + 1, colori::CIANO_CHIARO), t.linea_verticale(x0 + l + 1, y0, y0 + l + 1, colori::CIANO_CHIARO);
        // le etichette delle tre piu' recenti, una per riga di testo (le celle vicine non si accavallano)
        if (etichette < 3 && std::none_of(righe_usate.begin(), righe_usate.end(), [&](int r) { return std::abs(r - y0) < 10; })) {
            righe_usate.push_back(y0);
            ++etichette;
            f.scrivi(t, x0 + l + 4, y0, "LIBERA " + eta_minuti(k.eta_s) + "  " + std::to_string(k.libere) + "/" + std::to_string(k.misure),
                     colori::CIANO_CHIARO, 1);
        }
    }
    // il battello e la rotta
    t.rettangolo_pieno(cx - 1, cy - 4, 3, 9, colori::VERDE_CHIARO);
    for (int y = cy - 6; y > a.y + 4; y -= 4) t.pixel(cx, y, fosforo(3));
    // gli avvisi dalle sole misure, in alto a sinistra
    int y = a.y + 2;
    for (const AvvisoVolta& x : v.avvisi(b.pos, b.chiglia_m, b.rotta, b.velocita_kn, tempo)) {
        char riga[64];
        switch (x.tipo) {
            case AvvisoVolta::Tipo::volta_bassa: std::snprintf(riga, sizeof riga, "VOLTA BASSA: %.0f M", x.valore); break;
            case AvvisoVolta::Tipo::volta_in_discesa: std::snprintf(riga, sizeof riga, "VOLTA IN DISCESA: %.0f M", x.valore); break;
            case AvvisoVolta::Tipo::chiglia_a_prua: std::snprintf(riga, sizeof riga, "CHIGLIA A PRUA A %.1f KM", x.valore / 1000.0); break;
            case AvvisoVolta::Tipo::velocita_eccessiva: std::snprintf(riga, sizeof riga, "VELOCITA' SICURA %.0f KN", x.valore); break;
        }
        f.scrivi(t, a.x + 4, y, riga, x.tipo == AvvisoVolta::Tipo::velocita_eccessiva ? fosforo(6) : std::uint8_t{12}, 1);
        y += 11;
    }
    f.scrivi(t, a.x + a.w - 4 - f.larghezza("ROSSO: SOTTO LA VELA", 1), a.y + 2, "ROSSO: SOTTO LA VELA", fosforo(3), 1);
    t.imposta_ritaglio(prima);
}

// la carta termica, ultimi `finestra_s` secondi: il ghiaccio sopra dal sondatore alto (le tacche blu dell'acqua libera),
// il fondo sotto dal BQN-17, la quota del battello in verde; in due fasce come la sezione
void carta_volta_fondo(Tela& t, const Font& f, Rettangolo r, const QuadroVolta& v, const QuadroFondale* q, const BattelloVolta& b,
                       double tempo, double finestra_s = 1800.0) {
    const std::uint8_t carta = smalto(11), quadretti = smalto(8), alone = smalto(6), nero = colori::NERO;
    const Rettangolo prima = t.ritaglio();
    t.rettangolo_pieno(r.x, r.y, r.w, r.h, carta);
    cornice(t, r, smalto(4));
    f.scrivi(t, r.x + 4, r.y + 3, "VOLTA E FONDO  " + std::to_string(static_cast<int>(finestra_s / 60.0)) + " MIN", nero, 1);
    const Rettangolo p{r.x + 30, r.y + 14, r.w - 34, r.h - 28};
    const auto& storia = v.storia();   // gli ultimi 30 minuti del sondatore alto
    const double t0 = tempo - finestra_s;
    double ghiaccio_max = 0.0, fondo_max = 0.0;
    for (const auto& s : storia)
        if (s.t >= t0 && s.eco && !s.libera) ghiaccio_max = std::max(ghiaccio_max, s.pescaggio_m);
    if (q)
        for (const Sondaggio& s : q->sondaggi())
            if (s.t >= t0 && s.eco) fondo_max = std::max(fondo_max, s.fondo_m);
    // due fasce: sopra il ghiaccio ingrandito (55%), sotto dalla fine del ghiaccio al fondo
    const double zg = fascia_ghiaccio(ghiaccio_max, b), zf = std::max(zg + 50.0, scala_50(fondo_max));
    const int hg = p.h * 55 / 100, gap = 4;
    const auto y_di = [&](double m) {
        if (m <= zg) return p.y + static_cast<int>(m / zg * (hg - 1));
        return p.y + hg + gap + static_cast<int>(std::clamp((m - zg) / (zf - zg), 0.0, 1.0) * (p.h - hg - gap - 1));
    };
    const auto x_di = [&](double tc) { return p.x + static_cast<int>((tc - t0) / finestra_s * (p.w - 1)); };
    for (const double m : {0.0, zg / 2.0, zg, (zg + zf) / 2.0, zf}) {
        const int y = y_di(m);
        t.linea_orizzontale(p.x, p.x + p.w - 1, y, quadretti);
        const std::string n = std::to_string(static_cast<int>(m));
        f.scrivi(t, p.x - 3 - f.larghezza(n, 1), std::clamp(y - 3, p.y, p.y + p.h - 7), n, nero, 1);
    }
    for (int k = 0; k <= 6; ++k) t.linea_verticale(p.x + k * (p.w - 1) / 6, p.y, p.y + p.h - 1, quadretti);
    for (int x = p.x; x < p.x + p.w; x += 2) t.pixel(x, p.y + hg + gap / 2, nero);   // la spezzatura fra le fasce
    // il ghiaccio: dal sondatore alto, ogni misura una colonna dal pelo dell'acqua al pescaggio
    int prima_x = -1, prima_y = -1;
    for (const auto& s : storia) {
        if (s.t < t0 || s.t > tempo) continue;
        const int x = x_di(s.t);
        if (!s.eco) { prima_x = -1; continue; }
        if (s.libera) {
            t.linea_verticale(x, p.y, p.y + 3, colori::BLU);
            prima_x = -1;
            continue;
        }
        const int y = y_di(s.pescaggio_m);
        t.linea_verticale(x, p.y, y, alone);
        if (prima_x >= 0 && x - prima_x <= 3) t.linea_verticale(x, std::min(prima_y, y), std::max(prima_y, y), nero);
        else t.pixel(x, y, nero);
        prima_x = x, prima_y = y;
    }
    // il fondo: dal BQN-17
    if (q) {
        prima_x = -1;
        for (const Sondaggio& s : q->sondaggi()) {
            if (s.t < t0 || s.t > tempo || !s.eco) continue;
            const int x = x_di(s.t), y = y_di(s.fondo_m);
            t.linea_verticale(x, y, p.y + p.h - 1, alone);
            if (prima_x >= 0 && x - prima_x <= 3) t.linea_verticale(x, std::min(prima_y, y), std::max(prima_y, y), nero);
            else t.pixel(x, y, nero);
            prima_x = x, prima_y = y;
        }
    }
    // la quota del battello: le misure del sondatore alto la portano; adesso, a destra
    for (const auto& s : storia)
        if (s.t >= t0 && s.t <= tempo) t.pixel(x_di(s.t), y_di(s.quota_m), colori::VERDE);
    t.rettangolo_pieno(p.x + p.w - 3, y_di(b.chiglia_m) - 1, 3, 3, colori::VERDE);
    if (storia.empty()) f.scrivi(t, p.x + 4, p.y + 4, "SONDATORE ALTO: NESSUNA MISURA", nero, 1);
    f.scrivi(t, r.x + 4, r.y + r.h - 11, "SU: SONDATORE ALTO   GIU': BQN-17", nero, 1);
    t.imposta_ritaglio(prima);
}

// la sezione da `dietro_m` a poppa a `avanti_m` a prua, in due fasce: sopra il ghiaccio ingrandito (misurato pieno a
// poppa, le chiglie del BQS-15 a tacche a prua, a puntini dove non si sa), la cima della vela meno il margine, la quota e
// il battello; sotto il fondo (misurato pieno, la carta a tratti)
void sezione_volta(Tela& t, const Font& f, Rettangolo a, const QuadroVolta& v, const QuadroFondale* q, const BattelloVolta& b,
                   double tempo, double dietro_m = 2000.0, double avanti_m = 8000.0) {
    const Rettangolo prima = t.ritaglio();
    const Rettangolo p{a.x + 30, a.y + 2, a.w - 34, a.h - 4};
    const double lungo = dietro_m + avanti_m, c = v.cella_m();
    const Vec2 dir{std::sin(radianti(b.rotta)), std::cos(radianti(b.rotta))};
    const Vec2 da{b.pos.x - dir.x * dietro_m, b.pos.y - dir.y * dietro_m};
    const auto punto = [&](double d) { return Vec2{da.x + dir.x * d, da.y + dir.y * d}; };
    // il ghiaccio lungo la rotta, a passo di mezza cella
    struct Campione { double d; bool misurato = false, libera = false; double pescaggio = 0.0; };
    std::vector<Campione> ghiaccio;
    double gmax = 0.0;
    for (double d = 0.0; d <= lungo; d += c / 2.0) {
        Campione k{d};
        if (const auto cella = v.leggi(punto(d)); cella && cella->misure > 0) {
            k.misurato = true;
            k.libera = cella->libere > 0 && cella->pescaggio_max_m <= 0.5;
            k.pescaggio = cella->pescaggio_max_m;
            gmax = std::max(gmax, k.pescaggio);
        }
        ghiaccio.push_back(k);
    }
    const auto prof = q ? q->profilo(da, b.rotta, lungo, lungo / std::max(8, p.w / 4), tempo) : std::vector<PuntoProfilo>{};
    double fmax = 0.0;
    const auto plausibile = [](double m) { return std::isfinite(m) && m < 11000.0 ? m : 0.0; };
    for (const auto& x : prof) fmax = std::max({fmax, plausibile(x.carta_m), x.misurato ? plausibile(x.stima_m + x.sigma_m) : 0.0});
    const double zg = fascia_ghiaccio(gmax, b);
    const double zf = std::max(zg + 50.0, scala_50(fmax));
    const int hg = p.h * 60 / 100, gap = 5;
    const auto x_di = [&](double d) { return p.x + std::clamp(d / lungo, 0.0, 1.0) * (p.w - 1); };
    const auto y_di = [&](double m) {
        if (m <= zg) return p.y + std::clamp(m / zg, 0.0, 1.0) * (hg - 1);
        return p.y + hg + gap + std::clamp((m - zg) / (zf - zg), 0.0, 1.0) * (p.h - hg - gap - 1);
    };
    for (const double m : {0.0, zg / 2.0, zg, zf}) {   // la scala: due numeri per fascia
        const int y = static_cast<int>(y_di(m));
        for (int x = p.x; x < p.x + p.w; x += 3) t.pixel(x, y, fosforo(1));
        const std::string n = std::to_string(static_cast<int>(m));
        f.scrivi(t, p.x - 3 - f.larghezza(n, 1), std::clamp(y - 3, p.y, p.y + p.h - 7), n, fosforo(4), 1);
    }
    for (int x = p.x; x < p.x + p.w; x += 2) t.pixel(x, p.y + hg + gap / 2, fosforo(3));   // la spezzatura
    t.imposta_ritaglio(p);
    const double soglia = b.cima_vela_m - b.margine_m;
    const int xb = static_cast<int>(x_di(dietro_m));
    for (std::size_t i = 0; i + 1 < ghiaccio.size(); ++i) {
        const Campione& k = ghiaccio[i];
        const int x0 = static_cast<int>(x_di(k.d)), x1 = static_cast<int>(x_di(ghiaccio[i + 1].d));
        for (int x = x0; x < std::max(x0 + 1, x1); ++x) {
            if (!k.misurato) {
                if (x % 5 == 0) t.pixel(x, static_cast<int>(y_di(3.0)), fosforo(2));
                continue;
            }
            if (k.libera) {
                t.linea_verticale(x, static_cast<int>(p.y), static_cast<int>(p.y) + 1, colori::CIANO_CHIARO);
                continue;
            }
            const int y = static_cast<int>(y_di(k.pescaggio));
            const bool ostacolo = k.pescaggio >= soglia;
            if (x <= xb) {   // a poppa: il sondatore alto, pieno
                t.linea_verticale(x, static_cast<int>(p.y), y, fosforo(3));
            } else {   // a prua: le chiglie del BQS-15, a tacche
                for (int yy = static_cast<int>(p.y); yy <= y; yy += 2) t.pixel(x, yy, ostacolo ? std::uint8_t{colori::ROSSO} : fosforo(3));
            }
            t.pixel(x, y, ostacolo ? std::uint8_t{12} : fosforo(7));
        }
    }
    // il fondo
    for (std::size_t i = 0; i + 1 < prof.size(); ++i) {
        const auto& u = prof[i];
        const auto& w = prof[i + 1];
        if (u.misurato && w.misurato) {
            for (int x = static_cast<int>(x_di(u.distanza_m)); x <= static_cast<int>(x_di(w.distanza_m)); ++x)
                t.linea_verticale(x, static_cast<int>(y_di(u.stima_m)), static_cast<int>(p.y + p.h - 1), fosforo(1));
            linea(t, {x_di(u.distanza_m), y_di(u.stima_m)}, {x_di(w.distanza_m), y_di(w.stima_m)}, fosforo(6));
        }
        linea(t, {x_di(u.distanza_m), y_di(u.carta_m)}, {x_di(w.distanza_m), y_di(w.carta_m)}, fosforo(3), true);
    }
    // la cima della vela meno il margine, la quota, il battello con la vela
    for (int x = static_cast<int>(p.x); x < p.x + p.w; x += 4) t.linea_orizzontale(x, x + 1, static_cast<int>(y_di(soglia)), colori::GIALLO);
    const int yq = static_cast<int>(y_di(b.chiglia_m));
    t.linea_orizzontale(p.x, p.x + p.w - 1, yq, colori::VERDE);
    t.rettangolo_pieno(xb - 8, yq - 2, 17, 3, colori::VERDE_CHIARO);
    t.rettangolo_pieno(xb + 1, static_cast<int>(y_di(b.cima_vela_m)), 2, std::max(1, yq - 2 - static_cast<int>(y_di(b.cima_vela_m))), colori::VERDE_CHIARO);
    const double portata = dietro_m + PORTATA_CHIGLIA_M;
    for (int y = static_cast<int>(p.y); y < p.y + p.h; y += 3) t.pixel(static_cast<int>(x_di(portata)), y, fosforo(3));
    t.imposta_ritaglio(prima);
    f.scrivi(t, static_cast<int>(x_di(portata)) + 3, p.y + 2, "PORTATA BQS-15", fosforo(3), 1);
    const std::string vela = "VELA - " + std::to_string(static_cast<int>(b.margine_m)) + " M";
    // sopra la linea, ma dentro lo schermo (a quota periscopica la linea e' sulla superficie)
    f.scrivi(t, p.x + p.w - 4 - f.larghezza(vela, 1), std::max(p.y + 12, static_cast<int>(y_di(soglia)) - 9), vela, colori::GIALLO, 1);
}

}  // namespace

void disegna_pagina_volta(Tela& t, const Font& f, const QuadroVolta& volta, const QuadroFondale* fondale, const BattelloVolta& b,
                          double tempo) {
    constexpr int VISTA_H = 360;   // la parte di sopra della stazione; sotto resta il pannello
    lamiera::lamiera(t, {0, 0, LARGHEZZA_TELA, VISTA_H}, 7, 6);
    const Rettangolo a = qd::schermo(t, {4, 4, 400, 214}, f, "VOLTA A PRUA  BQS-15  2,5 KM",
                                     "QUADRO " + std::to_string(static_cast<int>(volta.cella_m())) + " M");
    volta_a_prua(t, f, a, volta, b, tempo);
    carta_volta_fondo(t, f, {412, 6, 222, 214}, volta, fondale, b, tempo);
    char destra[48];
    std::snprintf(destra, sizeof destra, "VELA A %.0f M", b.cima_vela_m);
    const Rettangolo c = qd::schermo(t, {4, 222, LARGHEZZA_TELA - 8, VISTA_H - 224}, f, "SEZIONE DI PRUA  -2 / +8 KM", destra);
    sezione_volta(t, f, c, volta, fondale, b, tempo);
}

void disegna_volta_carta(Tela& t, const VistaCarta& v, const Font& f, const QuadroVolta& volta, const BattelloVolta& b, double tempo) {
    const Rettangolo prima = t.ritaglio();
    t.imposta_ritaglio({v.x, v.y, v.w, v.h});
    const double c = volta.cella_m(), soglia = b.cima_vela_m - b.margine_m;
    // le celle misurate nel riquadro della carta: un quadrato pieno per cella (almeno 2 pixel); con le celle piu' piccole
    // di un pixel, un campione per pixel
    struct Croce { Vec2 p; double pescaggio; };
    std::vector<Croce> croci;
    const auto colore_cella = [&](const CellaVolta& cella) {
        if (cella.libere > 0 && cella.pescaggio_max_m <= 0.5) return std::uint8_t{colori::CIANO_CHIARO};
        if (cella.pescaggio_max_m < 3.5) return std::uint8_t{colori::BIANCO};
        if (cella.pescaggio_max_m < 9.0) return std::uint8_t{colori::GRIGIO};
        return std::uint8_t{colori::NERO};
    };
    const double lato_px = c / v.metri_per_pixel;
    const Vec2 m0 = v.a_mondo({static_cast<double>(v.x), static_cast<double>(v.y)});
    const Vec2 m1 = v.a_mondo({static_cast<double>(v.x + v.w), static_cast<double>(v.y + v.h)});
    if (lato_px >= 1.0) {
        const int l = std::max(2, static_cast<int>(std::ceil(lato_px)));
        for (double y = std::floor(std::min(m0.y, m1.y) / c) * c + c / 2.0; y < std::max(m0.y, m1.y); y += c)
            for (double x = std::floor(std::min(m0.x, m1.x) / c) * c + c / 2.0; x < std::max(m0.x, m1.x); x += c) {
                const auto cella = volta.leggi({x, y});
                if (!cella || cella->misure == 0) continue;
                const Vec2 s = v.a_tela({x, y});
                t.rettangolo_pieno(static_cast<int>(s.x) - l / 2, static_cast<int>(s.y) - l / 2, l, l, colore_cella(*cella));
                if (cella->pescaggio_max_m >= soglia) croci.push_back({s, cella->pescaggio_max_m});
            }
    } else {
        for (int y = v.y; y < v.y + v.h; ++y)
            for (int x = v.x; x < v.x + v.w; ++x) {
                const auto cella = volta.leggi(v.a_mondo({x + 0.5, y + 0.5}));
                if (!cella || cella->misure == 0) continue;
                t.pixel(x, y, colore_cella(*cella));
                if (cella->pescaggio_max_m >= soglia) croci.push_back({{x + 0.5, y + 0.5}, cella->pescaggio_max_m});
            }
    }
    // le chiglie sotto la vela: la croce rossa e il pescaggio
    std::vector<Vec2> fatte;   // una croce per chiglia: le celle vicine della stessa cresta non si ripetono
    for (const Croce& k : croci) {
        if (std::any_of(fatte.begin(), fatte.end(), [&](Vec2 q) { return std::fabs(q.x - k.p.x) < 10 && std::fabs(q.y - k.p.y) < 10; })) continue;
        fatte.push_back(k.p);
        linea(t, {k.p.x - 3, k.p.y - 3}, {k.p.x + 3, k.p.y + 3}, colori::ROSSO);
        linea(t, {k.p.x - 3, k.p.y + 3}, {k.p.x + 3, k.p.y - 3}, colori::ROSSO);
        f.scrivi(t, static_cast<int>(k.p.x) + 5, static_cast<int>(k.p.y) - 3, std::to_string(static_cast<int>(k.pescaggio)), colori::ROSSO, 1);
    }
    // le polynye candidate dell'ultima ora: il cerchio ciano col filo nero e l'eta' con l'ombra
    for (const auto& k : volta.polynye(b.pos, 50000.0, tempo, 3600.0)) {
        const Vec2 s = v.a_tela(k.centro);
        for (const auto& [r, col] : {std::pair{8.0, colori::NERO}, std::pair{7.0, colori::CIANO_CHIARO}, std::pair{6.0, colori::CIANO_CHIARO}})
            for (int i = 0; i < 48; ++i)
                t.pixel(static_cast<int>(std::lround(s.x + r * std::cos(i * 0.1309))), static_cast<int>(std::lround(s.y + r * std::sin(i * 0.1309))),
                        static_cast<std::uint8_t>(col));
        const std::string e = "LIBERA " + eta_minuti(k.eta_s);
        f.scrivi(t, static_cast<int>(s.x) + 12, static_cast<int>(s.y) - 2, e, colori::NERO, 1);
        f.scrivi(t, static_cast<int>(s.x) + 11, static_cast<int>(s.y) - 3, e, colori::CIANO_CHIARO, 1);
    }
    // la legenda, come il bollettino del meteo
    const Rettangolo l{v.x + 12, v.y + 10, 196, 54};
    t.rettangolo_pieno(l.x, l.y, l.w, l.h, colori::BIANCO);
    cornice(t, l, colori::NERO);
    f.scrivi(t, l.x + 6, l.y + 4, "VOLTA MISURATA", colori::NERO, 1);
    t.rettangolo_pieno(l.x + 6, l.y + 17, 4, 5, colori::BIANCO), t.linea_orizzontale(l.x + 6, l.x + 9, l.y + 17, colori::NERO);
    t.rettangolo_pieno(l.x + 11, l.y + 17, 4, 5, colori::GRIGIO), t.rettangolo_pieno(l.x + 16, l.y + 17, 4, 5, colori::NERO);
    f.scrivi(t, l.x + 24, l.y + 16, "SOTTILE / SPESSA / CHIGLIA", colori::NERO, 1);
    t.rettangolo_pieno(l.x + 6, l.y + 28, 14, 5, colori::CIANO_CHIARO);
    f.scrivi(t, l.x + 24, l.y + 27, "ACQUA LIBERA (ETA')", colori::NERO, 1);
    t.rettangolo_pieno(l.x + 6, l.y + 39, 14, 5, colori::ROSSO);
    f.scrivi(t, l.x + 24, l.y + 38, "CHIGLIA SOTTO LA VELA", colori::NERO, 1);
    t.imposta_ritaglio(prima);
}

void disegna_echi_carta(Tela& t, const VistaCarta& v, const std::vector<EcoCarta>& echi, Vec2 pos, double tempo, double durata_s) {
    const Rettangolo prima = t.ritaglio();
    t.imposta_ritaglio({v.x, v.y, v.w, v.h});
    for (const EcoCarta& e : echi) {
        const double eta = (tempo - e.t) / durata_s;
        if (eta < 0.0 || eta >= 1.0) continue;
        const double a = radianti(e.rilevamento);
        const Vec2 s = v.a_tela({pos.x + std::sin(a) * e.distanza_m, pos.y + std::cos(a) * e.distanza_m});
        // pieno e scuro appena arrivato, poi un pixel solo: si spegne come l'eco sullo schermo
        if (eta < 0.5) t.rettangolo_pieno(static_cast<int>(s.x) - 1, static_cast<int>(s.y) - 1, 3, 3, colori::NERO);
        t.pixel(static_cast<int>(s.x), static_cast<int>(s.y), eta < 0.5 ? std::uint8_t{colori::GIALLO} : std::uint8_t{colori::NERO});
    }
    t.imposta_ritaglio(prima);
}

}  // namespace m688
