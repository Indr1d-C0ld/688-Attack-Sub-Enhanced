#include "grafica/grafici_serie.hpp"

#include <algorithm>
#include <cmath>
#include <cstdio>

#include "grafica/font.hpp"
#include "grafica/palette.hpp"

namespace m688::grafici {

namespace {

constexpr std::uint8_t BIANCO = colori::BIANCO, GRIGIO = colori::GRIGIO, GRIGIO_SCURO = colori::GRIGIO_SCURO;

std::vector<double> validi(std::span<const double> v) {
    std::vector<double> r;
    for (const double x : v)
        if (std::isfinite(x)) r.push_back(x);
    return r;
}

// il percentile p (0-1) dei valori ordinati, interpolando fra i due vicini
double percentile(const std::vector<double>& o, double p) {
    if (o.empty()) return 0.0;
    const double k = p * static_cast<double>(o.size() - 1);
    const auto i = static_cast<std::size_t>(std::floor(k));
    const std::size_t j = std::min(i + 1, o.size() - 1);
    return o[i] + (o[j] - o[i]) * (k - static_cast<double>(i));
}

void linea_tratteggiata_verticale(Tela& t, int x, int y0, int y1, std::uint8_t c) {
    for (int y = y0; y <= y1; y += 2) t.pixel(x, y, c);
}

// il segno ± se il font ce l'ha, se no «+-»
std::string piu_meno(const Font& f) { return f.ha(U'±') ? " ± " : " +- "; }

}  // namespace

Riassunto riassumi(std::span<const double> valori) {
    std::vector<double> o = validi(valori);
    Riassunto s;
    s.n = static_cast<int>(o.size());
    if (o.empty()) return s;
    std::sort(o.begin(), o.end());
    double somma = 0.0;
    for (const double x : o) somma += x;
    s.media = somma / s.n;
    double q = 0.0;
    for (const double x : o) q += (x - s.media) * (x - s.media);
    s.dev = s.n > 1 ? std::sqrt(q / (s.n - 1)) : 0.0;
    s.min = o.front(), s.max = o.back();
    s.mediana = percentile(o, 0.5), s.p10 = percentile(o, 0.1), s.p90 = percentile(o, 0.9);
    return s;
}

std::string numero(double v, std::string_view unita) {
    char b[32];
    const double a = std::fabs(v);
    if (std::fabs(v - std::round(v)) < 1e-9 || a >= 100.0) std::snprintf(b, sizeof b, "%.0f", v);
    else if (a >= 10.0) std::snprintf(b, sizeof b, "%.1f", v);
    else std::snprintf(b, sizeof b, "%.2f", v);
    std::string s = b;
    // sotto 100 col decimale: gli zeri finali via, poi la virgola
    if (s.find('.') != std::string::npos) {
        while (s.back() == '0') s.pop_back();
        if (s.back() == '.') s.pop_back();
    }
    for (char& c : s)
        if (c == '.') c = ',';
    if (s == "-0") s = "0";
    if (!unita.empty()) s += " " + std::string(unita);
    return s;
}

void istogramma(Tela& t, const Font& f, Rettangolo r, std::span<const double> valori, const OpzioniIstogramma& o) {
    std::vector<double> v = validi(valori);
    if (!o.titolo.empty()) f.scrivi(t, r.x, r.y, o.titolo + (o.unita.empty() ? "" : " (" + o.unita + ")"), o.colore_testo, 1);
    const int x0 = r.x + 18, x1 = r.x + r.w - 2, top = r.y + 12, asse = r.y + r.h - 22;
    if (v.empty() || x1 - x0 < 10 || asse - top < 10) {
        f.scrivi(t, r.x + r.w / 2 - f.larghezza("NESSUN DATO", 1) / 2, r.y + r.h / 2 - 3, "NESSUN DATO", GRIGIO, 1);
        return;
    }
    const Riassunto s = riassumi(v);
    const double lo = s.min, hi = s.max;
    // valori interi su un intervallo stretto (perse, lanci): una classe per intero, centrata
    const bool interi = std::all_of(v.begin(), v.end(), [](double x) { return std::fabs(x - std::round(x)) < 1e-9; }) && hi - lo <= 20.0;
    const int classi = interi ? static_cast<int>(hi - lo) + 1 : (hi > lo ? std::max(1, o.classi) : 1);
    std::vector<int> conta(static_cast<std::size_t>(classi), 0);
    for (const double x : v) {
        int k = interi ? static_cast<int>(std::lround(x - lo)) : hi > lo ? static_cast<int>((x - lo) / (hi - lo) * classi) : 0;
        conta[static_cast<std::size_t>(std::clamp(k, 0, classi - 1))]++;
    }
    const int massimo = *std::max_element(conta.begin(), conta.end());
    const double larga = static_cast<double>(x1 - x0 + 1) / classi;
    const int alto = asse - top;
    // dove cade un valore sull'asse
    const auto x_di = [&](double x) {
        if (interi) return x0 + static_cast<int>(std::lround((x - lo + 0.5) * larga));
        return hi > lo ? x0 + static_cast<int>(std::lround((x - lo) / (hi - lo) * (x1 - x0))) : (x0 + x1) / 2;
    };
    // la scala delle partite e la griglia leggera a meta'
    f.scrivi(t, r.x, top - 3, std::to_string(massimo), o.colore_testo, 1);
    f.scrivi(t, r.x + 6, asse - 6, "0", o.colore_testo, 1);
    for (int x = x0; x <= x1; x += 3) t.pixel(x, top + alto / 2, GRIGIO_SCURO);
    // le barre, con un pixel di stacco fra l'una e l'altra
    for (int k = 0; k < classi; ++k) {
        if (!conta[static_cast<std::size_t>(k)]) continue;
        const int bx0 = x0 + static_cast<int>(std::lround(k * larga)), bx1 = x0 + static_cast<int>(std::lround((k + 1) * larga)) - (larga >= 3 ? 2 : 1);
        const int h = std::max(1, static_cast<int>(std::lround(static_cast<double>(conta[static_cast<std::size_t>(k)]) / massimo * alto)));
        t.rettangolo_pieno(bx0, asse - h, std::max(1, bx1 - bx0 + 1), h, o.colore);
        t.linea_orizzontale(bx0, std::max(bx0, bx1), asse - h, BIANCO);   // il filo di luce in cima
    }
    // l'asse, la mediana, la fascia p10-p90 sotto l'asse
    t.linea_orizzontale(x0, x1, asse, o.colore_testo);
    linea_tratteggiata_verticale(t, std::clamp(x_di(s.mediana), x0, x1), top, asse - 1, o.colore_mediana);
    const int pa = std::clamp(x_di(s.p10), x0, x1), pb = std::clamp(x_di(s.p90), x0, x1);
    t.linea_orizzontale(pa, pb, asse + 3, o.colore);
    t.linea_verticale(pa, asse + 2, asse + 4, o.colore);
    t.linea_verticale(pb, asse + 2, asse + 4, o.colore);
    // i numeri: min a sinistra, max a destra, la mediana sotto la sua linea se non li copre
    const std::string smin = numero(lo), smax = numero(hi), smed = "MED " + numero(s.mediana, o.unita);
    const int ty = asse + 7;
    f.scrivi(t, x0, ty, smin, o.colore_testo, 1);
    f.scrivi(t, x1 - f.larghezza(smax, 1) + 1, ty, smax, o.colore_testo, 1);
    const int wm = f.larghezza(smed, 1);
    const int xm = std::clamp(x_di(s.mediana) - wm / 2, x0 + f.larghezza(smin, 1) + 4, x1 - f.larghezza(smax, 1) - wm - 4);
    if (xm + wm < x1 - f.larghezza(smax, 1) - 2) f.scrivi(t, xm, ty, smed, o.colore_mediana, 1);
    const std::string n = std::to_string(s.n) + " PARTITE";
    f.scrivi(t, r.x + r.w - f.larghezza(n, 1) - 1, r.y, n, o.colore_testo, 1);
}

void barre_esiti(Tela& t, const Font& f, Rettangolo r, std::span<const VoceEsito> voci, std::uint8_t colore,
                 std::uint8_t colore_testo, std::uint8_t colore_attenuato) {
    int totale = 0, piu = 0;
    for (const auto& e : voci) totale += e.volte, piu = std::max(piu, e.volte);
    if (!totale) {
        f.scrivi(t, r.x + r.w / 2 - f.larghezza("NESSUNA PARTITA", 1) / 2, r.y + r.h / 2 - 3, "NESSUNA PARTITA", GRIGIO, 1);
        return;
    }
    const int etichetta = std::min(200, r.w * 2 / 5), coda = 76;
    const int bx = r.x + etichetta + 6, bw = std::max(10, r.w - etichetta - 6 - coda);
    int y = r.y;
    for (const auto& e : voci) {
        if (y + 12 > r.y + r.h) break;
        // i codici negativi sono interni (-1: la partita non e' finita): si scrive solo il testo
        std::string testo = e.codice >= 0 ? std::to_string(e.codice) + " " + e.testo : e.testo;
        while (testo.size() > 3 && f.larghezza(testo, 1) > etichetta) testo.pop_back();
        f.scrivi(t, r.x, y + 2, testo, colore_testo, 1);
        const int w = static_cast<int>(std::lround(static_cast<double>(e.volte) / piu * bw));
        t.rettangolo_pieno(bx, y + 1, bw, 9, GRIGIO_SCURO);   // la corsia
        if (w > 0) {
            t.rettangolo_pieno(bx, y + 1, w, 9, e.volte == piu || !colore_attenuato ? colore : colore_attenuato);
            t.linea_orizzontale(bx, bx + w - 1, y + 1, BIANCO);
        }
        // «14 SU 20»: nel font del 1989 il posto del «%» ha un altro disegno (sembra un «;»)
        const std::string q = std::to_string(e.volte) + " SU " + std::to_string(totale);
        f.scrivi(t, bx + bw + 6, y + 2, q, colore_testo, 1);
        y += 14;
    }
}

void riga_dispersione(Tela& t, const Font& f, Rettangolo r, const Riassunto& s, std::string_view unita,
                      std::uint8_t colore, std::uint8_t colore_testo, double scala_min, double scala_max) {
    const int ty = r.y + 1;   // il carattere e' alto 9 pixel: sta nella riga di 10
    if (s.n == 0) {
        f.scrivi(t, r.x, ty, "-", GRIGIO, 1);
        return;
    }
    const std::string a = numero(s.media) + piu_meno(f) + numero(s.dev, unita);
    const std::string b = numero(s.min) + "-" + numero(s.max);
    const int wa = std::max(f.larghezza(a, 1), r.w * 2 / 5), wb = std::max(f.larghezza(b, 1), r.w / 5);
    f.scrivi(t, r.x, ty, a, colore_testo, 1);
    f.scrivi(t, r.x + r.w - f.larghezza(b, 1), ty, b, GRIGIO, 1);
    // il grafico piccolo fra i due testi
    const int g0 = r.x + wa + 6, g1 = r.x + r.w - wb - 6, gy = r.y + r.h / 2;
    if (g1 - g0 < 12) return;
    double lo = scala_min, hi = scala_max;
    if (!(hi > lo)) lo = s.min, hi = s.max;
    const auto x_di = [&](double x) {
        return hi > lo ? g0 + static_cast<int>(std::lround(std::clamp((x - lo) / (hi - lo), 0.0, 1.0) * (g1 - g0))) : (g0 + g1) / 2;
    };
    t.linea_orizzontale(x_di(s.min), x_di(s.max), gy, GRIGIO);                         // min-max
    t.rettangolo_pieno(x_di(s.p10), gy - 2, std::max(1, x_di(s.p90) - x_di(s.p10) + 1), 5, colore);   // p10-p90
    t.linea_verticale(x_di(s.mediana), gy - 4, gy + 4, BIANCO);                        // mediana
    t.pixel(x_di(s.media), gy - 4, colore_testo), t.pixel(x_di(s.media), gy + 4, colore_testo);   // la media, sopra e sotto
}

std::vector<Rettangolo> barre_partite(Tela& t, Rettangolo r, std::span<const double> valori, std::uint8_t colore, int scelta) {
    std::vector<Rettangolo> zone;
    const int n = static_cast<int>(valori.size());
    if (n == 0 || r.w < n) return zone;
    double massimo = 0.0;
    for (const double x : valori)
        if (std::isfinite(x)) massimo = std::max(massimo, x);
    const double larga = static_cast<double>(r.w) / n;
    const int base = r.y + r.h - 1;
    t.linea_orizzontale(r.x, r.x + r.w - 1, base, GRIGIO);
    for (int i = 0; i < n; ++i) {
        const int bx0 = r.x + static_cast<int>(std::lround(i * larga)), bx1 = r.x + static_cast<int>(std::lround((i + 1) * larga)) - 1;
        const int w = std::max(1, bx1 - bx0 + 1 - (larga >= 3 ? 1 : 0));
        zone.push_back({bx0, r.y, bx1 - bx0 + 1, r.h});
        const double x = valori[static_cast<std::size_t>(i)];
        if (!std::isfinite(x)) {   // mancante: un puntino grigio sulla base
            t.pixel(bx0 + w / 2, base - 2, GRIGIO);
            continue;
        }
        const int h = massimo > 0.0 ? std::max(1, static_cast<int>(std::lround(x / massimo * (r.h - 3)))) : 1;
        t.rettangolo_pieno(bx0, base - h, w, h, colore);
        if (i == scelta) {   // la scelta: il bordo bianco attorno alla barra
            t.linea_orizzontale(bx0 - 1, bx0 + w, base - h - 1, BIANCO);
            t.linea_verticale(bx0 - 1, base - h - 1, base, BIANCO);
            t.linea_verticale(bx0 + w, base - h - 1, base, BIANCO);
        }
    }
    return zone;
}

}  // namespace m688::grafici
