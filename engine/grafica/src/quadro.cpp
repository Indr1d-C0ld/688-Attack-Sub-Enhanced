#include "grafica/quadro.hpp"

#include <algorithm>
#include <cmath>
#include <string>

#include "grafica/consolle.hpp"
#include "grafica/lamiera.hpp"
#include "grafica/tema_stazioni.hpp"

namespace m688::quadro {

namespace {

int spazio(const Font& f) { return f.formato() == 2 ? f.spazio_predefinito() : 1; }
int largo(const Font& f, std::string_view s) { return f.larghezza(s, 1, spazio(f), f.formato() != 2); }
void scrivi(Tela& t, const Font& f, int x, int y, std::string_view s, std::uint8_t c) {
    f.scrivi(t, x, y, s, c, 1, spazio(f), f.formato() != 2);
}

}  // namespace

Rettangolo consolle(Tela& t, const Font& targa, std::string_view posto) {
    lamiera::lamiera(t, {0, 0, t.larghezza(), t.altezza()}, 7, 11);
    lamiera::smusso(t, {0, 0, t.larghezza(), t.altezza()}, smalto(10), smalto(2), 2);
    const Rettangolo r = lamiera::targa(t, targa, t.larghezza() / 2, 4, posto);
    return {6, r.y + r.h + 4, t.larghezza() - 12, t.altezza() - r.y - r.h - 10};
}

Rettangolo schermo(Tela& t, Rettangolo r, const Font& f, std::string_view titolo, std::string_view destra) {
    // la cornice: incassata di due pixel nella lamiera, poi il bordo del vetro
    lamiera::smusso(t, r, smalto(3), smalto(10), 2);
    const Rettangolo v{r.x + 3, r.y + 3, r.w - 6, r.h - 6};
    t.rettangolo_pieno(v.x, v.y, v.w, v.h, fosforo(0));
    for (const int px : {v.x, v.x + v.w - 1})
        for (const int py : {v.y, v.y + v.h - 1}) t.pixel(px, py, smalto(1));
    const int h = f.righe() + 4;
    t.rettangolo_pieno(v.x + 1, v.y + 1, v.w - 2, h, fosforo(2));
    scrivi(t, f, v.x + 4, v.y + 3, titolo, fosforo(7));
    if (!destra.empty()) scrivi(t, f, v.x + v.w - 4 - largo(f, destra), v.y + 3, destra, fosforo(7));
    return {v.x + 2, v.y + h + 3, v.w - 4, v.h - h - 5};
}

int linguette(Tela& t, Rettangolo area, int y, const Font& f, const std::vector<std::string>& pagine, int scelta) {
    int x = area.x + 2;
    for (int i = 0; i < static_cast<int>(pagine.size()); ++i) {
        const auto& p = pagine[static_cast<std::size_t>(i)];
        const int w = largo(f, p) + 8;
        if (x + w > area.x + area.w) break;
        if (i == scelta) {
            t.rettangolo_pieno(x, y, w, f.righe() + 3, fosforo(5));
            scrivi(t, f, x + 4, y + 2, p, fosforo(0));
        } else {
            t.linea_orizzontale(x, x + w - 1, y + f.righe() + 2, fosforo(2));
            scrivi(t, f, x + 4, y + 2, p, fosforo(4));
        }
        x += w + 4;
    }
    return f.righe() + 5;
}

void riga(Tela& t, Rettangolo area, int y, const Font& f, std::string_view etichetta, const std::vector<int>& colonne,
          const std::vector<std::string>& valori, bool evidente) {
    if (evidente) t.rettangolo_pieno(area.x, y - 1, area.w, f.righe() + 2, fosforo(1));
    scrivi(t, f, area.x + 4, y, etichetta, fosforo(4));
    for (std::size_t i = 0; i < valori.size() && i < colonne.size(); ++i)
        scrivi(t, f, area.x + colonne[i], y, valori[i], fosforo(6));
}

int intestazione(Tela& t, Rettangolo area, int y, const Font& f, std::string_view testo, const std::vector<int>& colonne,
                 const std::vector<std::string>& nomi) {
    testo_fosforo(t, area.x + 4, y, testo, f);
    for (std::size_t i = 0; i < nomi.size() && i < colonne.size(); ++i) testo_fosforo(t, area.x + colonne[i], y, nomi[i], f);
    t.linea_orizzontale(area.x + 2, area.x + area.w - 3, y + f.righe() + 1, fosforo(3));
    return f.righe() + 5;
}

Rettangolo riquadro(Tela& t, Rettangolo r, const Font& f, std::string_view titolo) {
    t.rettangolo_pieno(r.x, r.y, r.w, r.h, fosforo(0));
    t.linea_orizzontale(r.x, r.x + r.w - 1, r.y, fosforo(3));
    t.linea_orizzontale(r.x, r.x + r.w - 1, r.y + r.h - 1, fosforo(3));
    t.linea_verticale(r.x, r.y, r.y + r.h - 1, fosforo(3));
    t.linea_verticale(r.x + r.w - 1, r.y, r.y + r.h - 1, fosforo(3));
    const int h = f.righe() + 3;
    t.rettangolo_pieno(r.x + 1, r.y + 1, r.w - 2, h, fosforo(2));
    scrivi(t, f, r.x + 4, r.y + 2, titolo, fosforo(7));
    return {r.x + 2, r.y + h + 3, r.w - 4, r.h - h - 5};
}

Rettangolo fascia_tempo(Tela& t, Rettangolo r, const Font& f, std::string_view sinistra, std::string_view destra, double ora,
                        const std::vector<TaccaTempo>& tacche) {
    lamiera::smusso(t, r, smalto(3), smalto(10), 1);
    t.rettangolo_pieno(r.x + 1, r.y + 1, r.w - 2, r.h - 2, fosforo(0));
    const int ty = r.y + (r.h - f.righe()) / 2;
    scrivi(t, f, r.x + 5, ty, sinistra, fosforo(6));
    const int wd = destra.empty() ? 0 : largo(f, destra);
    if (wd) scrivi(t, f, r.x + r.w - 5 - wd, ty, destra, fosforo(6));
    const int x0 = r.x + 5 + largo(f, sinistra) + 10, x1 = r.x + r.w - 5 - wd - (wd ? 10 : 0);
    const Rettangolo bar{x0, r.y + 3, std::max(0, x1 - x0), r.h - 6};
    if (bar.w <= 0 || bar.h <= 0) return bar;
    const auto x_di = [&](double u) { return bar.x + static_cast<int>(std::clamp(u, 0.0, 1.0) * (bar.w - 1) + 0.5); };
    const int mezzo = bar.y + bar.h / 2;
    t.linea_orizzontale(bar.x, bar.x + bar.w - 1, mezzo, fosforo(2));
    t.linea_orizzontale(bar.x, x_di(ora), mezzo, fosforo(4));
    std::vector<TaccaTempo> in_ordine = tacche;
    std::stable_sort(in_ordine.begin(), in_ordine.end(), [](const TaccaTempo& a, const TaccaTempo& b) { return a.interesse < b.interesse; });
    const int mezza = (bar.h - 1) / 2;
    for (const TaccaTempo& k : in_ordine) {
        const int alta = k.interesse >= 70 ? mezza : k.interesse >= 50 ? std::max(1, mezza * 2 / 3) : k.interesse >= 30 ? std::max(1, mezza / 3) : 0;
        t.linea_verticale(x_di(k.u), mezzo - alta, mezzo + alta, k.colore);
    }
    t.rettangolo_pieno(x_di(ora) - 1, bar.y, 3, bar.h, fosforo(7));
    return bar;
}

void barra_statistica(Tela& t, Rettangolo r, double scala, double media, double scarto, double minimo, double massimo,
                      std::uint8_t colore) {
    if (r.w <= 1 || r.h <= 0) return;
    const double s = std::max(scala, 1e-9);
    const auto x_di = [&](double v) { return r.x + static_cast<int>(std::clamp(v / s, 0.0, 1.0) * (r.w - 1) + 0.5); };
    const int ym = r.y + r.h / 2, mezza = std::max(1, r.h / 2 - 1);
    t.linea_orizzontale(r.x, r.x + r.w - 1, ym, fosforo(1));   // la scala della riga
    t.linea_orizzontale(x_di(minimo), x_di(massimo), ym, fosforo(4));   // i baffi
    t.linea_verticale(x_di(minimo), ym - mezza, ym + mezza, fosforo(4));
    t.linea_verticale(x_di(massimo), ym - mezza, ym + mezza, fosforo(4));
    const int b0 = x_di(std::max(0.0, media - scarto)), b1 = x_di(media + scarto);
    t.rettangolo_pieno(b0, ym - mezza + 1, std::max(1, b1 - b0 + 1), 2 * mezza - 1, colore);
    t.linea_verticale(x_di(media), ym - mezza, ym + mezza, 15);
}

void stato_vuoto(Tela& t, Rettangolo r, const Font& f, std::string_view titolo, const std::vector<std::string>& righe) {
    const int passo = f.righe() + 3, h = f.righe() + 15 + static_cast<int>(righe.size()) * passo;
    const Rettangolo b{r.x + 12, r.y + std::max(0, (r.h - h) / 2), std::max(r.w - 24, 8), std::min(h, r.h)};
    for (int x = b.x; x < b.x + b.w; x += 2) t.pixel(x, b.y, fosforo(2)), t.pixel(x, b.y + b.h - 1, fosforo(2));
    for (int y = b.y; y < b.y + b.h; y += 2) t.pixel(b.x, y, fosforo(2)), t.pixel(b.x + b.w - 1, y, fosforo(2));
    const auto centro = [&](std::string_view s) { return b.x + (b.w - f.larghezza(s, 1, f.formato() == 2 ? f.spazio_predefinito() : 1, f.formato() != 2)) / 2; };
    scrivi(t, f, centro(titolo), b.y + 6, titolo, fosforo(6));
    int y = b.y + 8 + f.righe() + 4;
    for (const auto& s : righe) scrivi(t, f, centro(s), y, s, fosforo(4)), y += passo;
}

void profilo_quota(Tela& t, Rettangolo area, const Font& f, const ProfiloQuota& p) {
    // la scala: il piu' profondo fra la quota, il fondo misurato e lo strato, piu' un decimo, a multipli di 50 m
    double fondo_scala = p.scala_m;
    if (fondo_scala <= 0.0) {
        double massimo = std::max(p.strato_m, p.periscopica_m);
        for (const double q : p.quota_m) massimo = std::max(massimo, q);
        for (const double q : p.fondo_m) massimo = std::max(massimo, q);
        fondo_scala = std::max(50.0, std::ceil(massimo * 1.1 / 50.0) * 50.0);
    }
    const int sinistra = largo(f, std::to_string(static_cast<int>(fondo_scala)) + " M") + 6;
    const Rettangolo r{area.x + sinistra, area.y + 8, area.w - sinistra - 8, area.h - f.righe() - 16};
    if (r.w < 8 || r.h < 8) return;
    const auto y_di = [&](double m) { return r.y + static_cast<int>(std::clamp(m / fondo_scala, 0.0, 1.0) * (r.h - 1)); };
    // la griglia delle quote e dei minuti
    const double passo_m = fondo_scala <= 200.0 ? 50.0 : fondo_scala <= 500.0 ? 100.0 : 250.0;
    for (double m = 0.0; m <= fondo_scala + 0.5; m += passo_m) {
        scrivi(t, f, area.x + 2, y_di(m) - f.righe() / 2, std::to_string(static_cast<int>(m)) + " M", fosforo(3));
        for (int x = r.x; x < r.x + r.w; x += 3) t.pixel(x, y_di(m), fosforo(1));
    }
    const int minuti = std::max(1, static_cast<int>(std::lround(p.durata_s / 60.0)));
    const int passo_min = std::max(1, static_cast<int>(std::lround(minuti / 5.0)));
    for (int k = 0; k <= minuti; k += passo_min) {
        const int x = r.x + k * (r.w - 1) / minuti;
        const std::string s = "-" + std::to_string(minuti - k);
        scrivi(t, f, x - largo(f, s) / 2, r.y + r.h + 4, s, fosforo(3));
    }
    // il valore del campione alla colonna x (interpolato), dal piu' vecchio a sinistra
    const auto campione = [&](const std::vector<double>& v, int x) {
        if (v.empty()) return 0.0;
        if (v.size() == 1) return v.front();
        const double s = static_cast<double>(x) / std::max(1, r.w - 1) * static_cast<double>(v.size() - 1);
        const auto i = static_cast<std::size_t>(std::floor(s));
        if (i + 1 >= v.size()) return v.back();
        const double a = v[i], b = v[i + 1];
        if (a <= 0.0 || b <= 0.0) return s - static_cast<double>(i) < 0.5 ? a : b;   // dove manca una misura, il piu' vicino
        return a + (b - a) * (s - static_cast<double>(i));
    };
    // il fondo a retino, dove e' misurato
    if (!p.fondo_m.empty()) {
        int x_scritta = -1, y_alto = r.y + r.h;   // la scritta sul primo tratto misurato, sotto il punto piu' alto
        for (int x = 0; x < r.w; ++x) {
            const double fondo = campione(p.fondo_m, x);
            if (fondo <= 0.0) continue;
            for (int y = y_di(fondo); y < r.y + r.h; ++y) t.pixel(r.x + x, y, (x + y) % 2 ? fosforo(2) : fosforo(1));
            if (x_scritta < 0) x_scritta = r.x + x + 4;
            y_alto = std::min(y_alto, y_di(fondo));
        }
        if (x_scritta >= 0 && r.y + r.h - y_alto > f.righe() + 6)
            scrivi(t, f, x_scritta, r.y + r.h - f.righe() - 3, "FONDO DALL'ECOSCANDAGLIO", fosforo(5));
    }
    // lo strato, a tratti in ciano
    if (p.strato_m > 0.0) {
        for (int x = r.x; x < r.x + r.w; ++x)
            if ((x / 4) % 2 == 0) t.pixel(x, y_di(p.strato_m), 11);
        const std::string s = "STRATO " + std::to_string(static_cast<int>(std::lround(p.strato_m))) + " M";
        scrivi(t, f, r.x + r.w - largo(f, s) - 2, y_di(p.strato_m) - f.righe() - 2, s, 11);
    }
    // la traccia del battello, continua
    int prima = -1;
    if (!p.quota_m.empty())
        for (int x = 0; x < r.w; ++x) {
            const int y = y_di(campione(p.quota_m, x));
            if (prima >= 0) t.linea_verticale(r.x + x, std::min(prima, y), std::max(prima, y), fosforo(7));
            else t.pixel(r.x + x, y, fosforo(7));
            prima = y;
        }
    // la quota periscopica, a tratti in verde, sopra la traccia
    if (p.periscopica_m > 0.0) {
        for (int x = r.x; x < r.x + r.w; x += 2) t.pixel(x, y_di(p.periscopica_m), 10);
        scrivi(t, f, r.x + 4, y_di(p.periscopica_m) - f.righe() - 2, "QUOTA PERISCOPICA", 10);
    }
}

DisposizioneVivo consolle_vivo(Tela& t, const Font& targa, const Font& f, std::string_view titolo, std::string_view destra,
                               std::string_view filtro, int colonna, int piede_y) {
    const Rettangolo u = consolle(t, targa, "SALA ANALISI - DAL VIVO");
    const int bh = f.righe() + 6, by = piede_y - 3 - bh;
    DisposizioneVivo d;
    d.carta = schermo(t, {u.x, u.y, u.w - colonna - 4, by - 3 - u.y}, f, titolo, destra);
    d.registro = schermo(t, {u.x + u.w - colonna, u.y, colonna, by - 3 - u.y}, f, "REGISTRO", filtro);
    d.fascia = {u.x, by, u.w, bh};
    return d;
}

void riga_registro(Tela& t, Rettangolo area, int y, const Font& f, std::string_view ora, std::string_view testo,
                   std::uint8_t colore, bool scelta) {
    if (scelta) t.rettangolo_pieno(area.x, y - 1, area.w, f.righe() + 2, fosforo(1));
    scrivi(t, f, area.x + 2, y, ora, scelta ? fosforo(7) : fosforo(4));
    const int x = area.x + 2 + largo(f, "00:00") + 6, w = area.x + area.w - 2 - x;
    std::string s(testo);
    while (!s.empty() && largo(f, s) > w) s.pop_back();
    scrivi(t, f, x, y, s, scelta ? fosforo(7) : colore);
}

}  // namespace m688::quadro
