#include "grafica/lamiera.hpp"

#include <algorithm>
#include <array>
#include <cmath>

#include "grafica/cerimonia.hpp"
#include "grafica/consolle.hpp"

namespace m688::lamiera {

namespace {

namespace cer = m688::cerimonia;
constexpr std::uint8_t NERO = 0;

int spazio(const Font& f) { return f.formato() == 2 ? f.spazio_predefinito() : 1; }
int largo(const Font& f, std::string_view s) { return f.larghezza(s, 1, spazio(f), f.formato() != 2); }
void scrivi(Tela& t, const Font& f, int x, int y, std::string_view s, std::uint8_t c) {
    f.scrivi(t, x, y, s, c, 1, spazio(f), f.formato() != 2);
}

}  // namespace

void smusso(Tela& t, Rettangolo r, std::uint8_t chiaro, std::uint8_t scuro, int spessore) {
    for (int k = 0; k < spessore; ++k) {
        t.linea_orizzontale(r.x + k, r.x + r.w - 1 - k, r.y + k, chiaro);
        t.linea_verticale(r.x + k, r.y + k, r.y + r.h - 1 - k, chiaro);
        t.linea_orizzontale(r.x + k, r.x + r.w - 1 - k, r.y + r.h - 1 - k, scuro);
        t.linea_verticale(r.x + r.w - 1 - k, r.y + k, r.y + r.h - 1 - k, scuro);
    }
}

void lamiera(Tela& t, Rettangolo r, int tono, std::uint32_t seme) {
    const int s = static_cast<int>(seme);
    for (int y = r.y; y < r.y + r.h; ++y)
        for (int x = r.x; x < r.x + r.w; ++x) {
            const double v = 0.5 + 0.18 * std::sin(y * 0.045 + x * 0.006) + 0.25 * (cer::caso(x / 3, y, s) - 0.5);
            t.pixel(x, y, smalto(tono - 1 + cer::retina(v, x, y, 2)));
        }
    for (int k = 0; k < r.w * r.h / 9000; ++k) {   // graffi sottili, rari
        const int x = r.x + static_cast<int>(cer::caso(k, 1, s) * r.w), y = r.y + static_cast<int>(cer::caso(k, 2, s) * r.h);
        const int l = 4 + static_cast<int>(cer::caso(k, 3, s) * 9);
        for (int i = 0; i < l && x + i < r.x + r.w && y + i / 3 < r.y + r.h; ++i) t.pixel(x + i, y + i / 3, smalto(tono + 1));
    }
}

void pannello(Tela& t, Rettangolo r, int tono) {
    lamiera(t, {r.x + 2, r.y + 2, r.w - 4, r.h - 4}, tono - 1, static_cast<std::uint32_t>(r.x * 7 + r.y));
    smusso(t, r, smalto(tono - 5), smalto(tono + 4), 2);
    for (const int vx : {r.x + 5, r.x + r.w - 6})
        for (const int vy : {r.y + 5, r.y + r.h - 6}) {   // la vite: 3x3, chiara sopra, scura sotto e a destra
            t.rettangolo_pieno(vx - 1, vy - 1, 3, 3, smalto(tono - 2));
            t.pixel(vx - 1, vy - 1, smalto(tono + 4));
            t.pixel(vx, vy - 1, smalto(tono + 3));
            t.pixel(vx + 1, vy + 1, smalto(tono - 5));
            t.pixel(vx - 1, vy + 1, smalto(tono - 4));
        }
}

void vetro(Tela& t, Rettangolo r) {
    t.rettangolo_pieno(r.x, r.y, r.w, r.h, NERO);
    smusso(t, r, smalto(2), smalto(10));
}

void incisa(Tela& t, const Font& f, int x, int y, std::string_view s, std::uint8_t c, std::uint8_t ombra) {
    scrivi(t, f, x + 1, y + 1, s, ombra);
    scrivi(t, f, x, y, s, c);
}

void principale(Tela& t, const Font& f, int x, int y, std::string_view s, Scritte sc) {
    if (sc == Scritte::scure) scrivi(t, f, x, y, s, NERO);
    else incisa(t, f, x, y, s, 15);
}

void secondaria(Tela& t, const Font& f, int x, int y, std::string_view s, bool piccola, Scritte sc) {
    if (sc == Scritte::scure) {   // sul beige: piatta e scura, come le targhette dell'Alfa
        if (piccola) f.scrivi(t, x, y, s, 8, 1, spazio(f), false);
        else scrivi(t, f, x, y, s, 8);
        return;
    }
    if (!piccola) {
        incisa(t, f, x, y, s, smalto(11), NERO);
        return;
    }
    f.scrivi(t, x + 1, y + 1, s, NERO, 1, spazio(f), false);
    f.scrivi(t, x, y, s, smalto(11), 1, spazio(f), false);
}

Rettangolo targa(Tela& t, const Font& f, int cx, int y, std::string_view s) {
    const int w = largo(f, s) + 28, h = f.righe() + 8;
    const Rettangolo r{cx - w / 2, y, w, h};
    t.rettangolo_pieno(r.x, r.y, r.w, r.h, smalto(1));
    smusso(t, r, smalto(9), NERO);
    for (const int vx : {r.x + 5, r.x + r.w - 6}) {   // i rivetti
        t.rettangolo_pieno(vx - 1, r.y + h / 2 - 1, 3, 3, smalto(6));
        t.pixel(vx - 1, r.y + h / 2 - 1, smalto(10));
    }
    incisa(t, f, r.x + 14, r.y + 4, s, colori::BIANCO, smalto(3));
    return r;
}

void tasto(Tela& t, Rettangolo r, std::string_view testo, ColoreLampada c, Stato s, bool predefinito, const Font& f) {
    if (s == Stato::disabilitato) {   // piatto, senza smusso e senza lente: si vede che non si puo'
        t.rettangolo_pieno(r.x, r.y, r.w, r.h, smalto(4));
        const int x = r.x + (r.w - largo(f, testo)) / 2, y = r.y + (r.h - f.righe()) / 2;
        scrivi(t, f, x, y, testo, smalto(9));   // spento ma leggibile
    } else {
        pulsante(t, r, testo, c, s == Stato::evidenziato || s == Stato::premuto, s == Stato::premuto, f);
    }
    if (predefinito) smusso(t, {r.x - 2, r.y - 2, r.w + 4, r.h + 4}, lampada(ColoreLampada::ambra, true), lampada(ColoreLampada::ambra, false));
}

void spia(Tela& t, Rettangolo r, bool accesa, std::uint8_t lente) {
    if (!accesa) {
        smusso(t, r, smalto(3), smalto(10));
        return;
    }
    t.rettangolo_pieno(r.x, r.y, r.w, r.h, smalto(6));
    smusso(t, r, smalto(11), smalto(1));
    const Rettangolo l{r.x + 4, r.y + 4, r.w - 8, r.h - 8};
    t.rettangolo_pieno(l.x, l.y, l.w, l.h, lente);
    smusso(t, l, NERO, colori::BIANCO);
}

void tasto_tastiera(Tela& t, Rettangolo r, std::string_view lettera, const Font& f) {
    t.rettangolo_pieno(r.x, r.y, r.w, r.h, smalto(9));
    smusso(t, r, smalto(11), smalto(2));
    scrivi(t, f, r.x + (r.w - largo(f, lettera)) / 2, r.y + (r.h - f.righe()) / 2, lettera, NERO);
}

void piede(Tela& t, const Font& f, int y, std::string_view aiuto, const std::vector<Tasto>& tasti, int predefinito, Scritte sc) {
    if (!aiuto.empty()) {   // la riga d'aiuto: sempre su una fascia, nella stessa forma; scura sul beige
        t.rettangolo_pieno(8, y, 624, 13, smalto(sc == Scritte::scure ? 10 : 3));
        scrivi(t, f, 14, y + (13 - f.righe()) / 2, aiuto, sc == Scritte::scure ? NERO : smalto(11));
    }
    const int n = static_cast<int>(tasti.size());
    if (n == 0) return;
    const int w = (624 - 8 * (n - 1)) / n;
    for (int i = 0; i < n; ++i) {
        const auto& k = tasti[static_cast<std::size_t>(i)];
        tasto(t, {8 + i * (w + 8), y + 22, w, 26}, k.testo, k.colore, k.stato, i == predefinito, f);
    }
}

std::vector<std::string> a_capo(const Font& f, std::string_view testo, int w) {
    std::vector<std::string> out;
    std::string riga, parola;
    const auto chiudi = [&] {
        if (parola.empty()) return;
        const std::string prova = riga.empty() ? parola : riga + " " + parola;
        if (largo(f, prova) > w && !riga.empty()) out.push_back(riga), riga = parola;
        else riga = prova;
        parola.clear();
    };
    for (const char ch : testo) {
        if (ch == ' ') chiudi();
        else parola += ch;
    }
    chiudi();
    if (!riga.empty()) out.push_back(riga);
    return out;
}

void testata_classificata(Tela& t, const Font& f, std::string_view classifica, std::string_view dtg, Scritte sc) {
    if (sc == Scritte::scure) {
        scrivi(t, f, 12, 9, classifica, 4);
        scrivi(t, f, t.larghezza() - 12 - largo(f, dtg), 9, dtg, NERO);
        return;
    }
    incisa(t, f, 12, 9, classifica, 12);
    incisa(t, f, t.larghezza() - 12 - largo(f, dtg), 9, dtg, 15);
}

Rettangolo modulo_continuo(Tela& t, Rettangolo r, int passo_riga) {
    constexpr std::uint8_t BIANCO = 15;
    t.rettangolo_pieno(r.x, r.y, r.w, r.h, BIANCO);
    const int m = 14, dentro = r.x + m, fuori = r.x + r.w - m;
    // le fasce allineate alle righe del testo, che comincia a r.y + 6
    for (int y = r.y + 4, k = 0; y < r.y + r.h - 2; y += 3 * passo_riga, ++k)
        if (k % 2 == 1) t.rettangolo_pieno(dentro, y, fuori - dentro, std::min(3 * passo_riga, r.y + r.h - 2 - y), smalto(11));
    for (const int x : {r.x + m - 1, r.x + r.w - m})   // la perforazione
        for (int y = r.y; y < r.y + r.h; y += 2) t.pixel(x, y, smalto(7));
    for (int y = r.y + 6; y + 5 < r.y + r.h; y += 12)   // i fori di trascinamento, ogni mezzo pollice
        for (const int cx : {r.x + m / 2, r.x + r.w - m / 2}) {
            t.rettangolo_pieno(cx - 2, y, 5, 5, smalto(5));
            for (const auto& [dx, dy] : {std::pair{-2, 0}, {2, 0}, {-2, 4}, {2, 4}}) t.pixel(cx + dx, y + dy, BIANCO);
        }
    smusso(t, r, BIANCO, smalto(4));
    return {dentro + 6, r.y + 6, fuori - dentro - 12, r.h - 12};
}

Rettangolo foglio(Tela& t, Rettangolo r) {
    t.rettangolo_pieno(r.x + 3, r.y + 3, r.w, r.h, smalto(2));   // l'ombra sulla lamiera
    t.rettangolo_pieno(r.x, r.y, r.w, r.h, 15);
    smusso(t, r, 15, smalto(6));
    return {r.x + 8, r.y + 8, r.w - 16, r.h - 16};
}

Rettangolo timbro(Tela& t, const Font& f, int x, int y, std::string_view testo, std::uint8_t colore) {
    const Rettangolo r{x, y, largo(f, testo) + 10, 15};
    for (const int k : {0, 1}) smusso(t, {r.x - k, r.y - k, r.w + 2 * k, r.h + 2 * k}, colore, colore);
    scrivi(t, f, r.x + 5, r.y + 3, testo, colore);
    return r;
}

void voce_modulo(Tela& t, const Font& f, int x, int y, int w, std::string_view etichetta, std::string_view valore,
                 std::uint8_t penna) {
    scrivi(t, f, x, y, etichetta, NERO);
    const int a = x + largo(f, etichetta) + 3, b = x + w - largo(f, valore) - 3;
    for (int xx = a; xx < b; xx += 3) t.pixel(xx, y + 7, smalto(6));
    scrivi(t, f, x + w - largo(f, valore), y, valore, penna);
}

void casella(Tela& t, int x, int y, bool spuntata, std::uint8_t penna) {
    smusso(t, {x, y + 1, 7, 7}, NERO, NERO);
    if (spuntata)
        for (int k = 0; k < 7; ++k) t.pixel(x + k, y + 1 + k, penna), t.pixel(x + 6 - k, y + 1 + k, penna);
}

void spia_voce(Tela& t, Rettangolo r, ColoreLampada c, bool accesa) {
    smusso(t, {r.x - 1, r.y - 1, r.w + 2, r.h + 2}, smalto(2), smalto(10));
    t.rettangolo_pieno(r.x, r.y, r.w, r.h, lampada(c, accesa));
    if (accesa) t.linea_orizzontale(r.x + 2, r.x + r.w - 4, r.y + 1, 15);   // il riflesso
}

int campo_vetro(Tela& t, const Font& f, int x, int y, int w, std::string_view etichetta,
                const std::vector<std::pair<std::string, std::uint8_t>>& righe, Scritte sc) {
    secondaria(t, f, x + 2, y, etichetta, false, sc);
    const int yv = y + f.righe() + 2;
    const int h = static_cast<int>(righe.size()) * (f.righe() + 1) + 4;
    vetro(t, {x, yv, w, h});
    int yy = yv + 3;
    for (const auto& [s, c] : righe) scrivi(t, f, x + 4, yy, s, c), yy += f.righe() + 1;
    return yv + h - y;
}

void elenco_vetro(Tela& t, Rettangolo r, const Font& f, const std::vector<std::string>& voci, int scelta, int passo,
                  int primo) {
    vetro(t, r);
    const int n = std::max(1, (r.h - 6) / std::max(passo, 1)), tot = static_cast<int>(voci.size());
    primo = std::clamp(primo, 0, std::max(0, tot - n));
    for (int i = primo; i < tot && i < primo + n; ++i) {
        const int y = r.y + 4 + (i - primo) * passo;
        if (i == scelta) {
            t.rettangolo_pieno(r.x + 3, y - 2, r.w - 12, passo, lampada(ColoreLampada::ambra, true));
            scrivi(t, f, r.x + 6, y, voci[static_cast<std::size_t>(i)], NERO);
        } else {
            scrivi(t, f, r.x + 6, y, voci[static_cast<std::size_t>(i)], 15);
        }
    }
    // la guida e la tacca: lunga quanto la parte visibile, dove sta la parte visibile
    const int guida = r.h - 8;
    t.linea_verticale(r.x + r.w - 5, r.y + 3, r.y + r.h - 4, 8);
    const int h = tot > n ? std::max(4, guida * n / tot) : guida, y0 = tot > n ? (guida - h) * primo / (tot - n) : 0;
    t.rettangolo_pieno(r.x + r.w - 6, r.y + 4 + y0, 3, h, 7);
}

void in_colori_vista(Tela& t, Rettangolo r, const Palette& p) {
    std::array<std::uint8_t, 256> mappa{};
    for (int i = 0; i < 256; ++i) mappa[static_cast<std::size_t>(i)] = static_cast<std::uint8_t>(i);
    for (int i = colori_tema::PRIMO; i <= colori_tema::ULTIMO; ++i) {
        const auto c = p.rgb(i);
        int meglio = 0;
        long d_min = -1;
        const auto prova = [&](int j) {
            const auto q = p.rgb(j);
            const long dr = c[0] - q[0], dg = c[1] - q[1], db = c[2] - q[2];
            const long d = 3 * dr * dr + 4 * dg * dg + 2 * db * db;   // pesi della luminosita', come gli occhi
            if (d_min < 0 || d < d_min) d_min = d, meglio = j;
        };
        // le lampade restano colorate: solo gli EGA con una tinta (non il nero, i grigi e il bianco)
        const bool lampada = i >= colori_tema::LAMPADE && i < colori_tema::LAMPADE + 6;
        for (int j = 0; j < 16; ++j)
            if (!lampada || (j != 0 && j != 7 && j != 8 && j != 15)) prova(j);
        if (!lampada)
            for (int j = 144; j <= 158; ++j) prova(j);
        mappa[static_cast<std::size_t>(i)] = static_cast<std::uint8_t>(meglio);
    }
    for (int y = std::max(r.y, 0); y < std::min(r.y + r.h, t.altezza()); ++y)
        for (int x = std::max(r.x, 0); x < std::min(r.x + r.w, t.larghezza()); ++x) t.pixel(x, y, mappa[t.at(x, y)]);
}

}  // namespace m688::lamiera
