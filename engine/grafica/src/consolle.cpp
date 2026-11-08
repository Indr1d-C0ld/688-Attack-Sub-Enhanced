#include "grafica/consolle.hpp"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <string>

#include "grafica/font.hpp"
#include "grafica/palette.hpp"
#include "nucleo/geometria.hpp"

namespace m688 {

namespace {

using namespace colori_tema;

std::uint8_t idx(int i) { return static_cast<std::uint8_t>(i); }

void disco(Tela& t, double cx, double cy, double r, std::uint8_t c) {
    for (int y = static_cast<int>(std::floor(cy - r)); y <= static_cast<int>(std::ceil(cy + r)); ++y)
        for (int x = static_cast<int>(std::floor(cx - r)); x <= static_cast<int>(std::ceil(cx + r)); ++x)
            if ((x + 0.5 - cx) * (x + 0.5 - cx) + (y + 0.5 - cy) * (y + 0.5 - cy) <= r * r) t.pixel(x, y, c);
}

// corona fra r0 e r1; con `luce` la meta' in alto a sinistra di un tono e l'altra di un altro (smusso)
void anello(Tela& t, double cx, double cy, double r0, double r1, std::uint8_t c, int luce = -1, int ombra = -1) {
    for (int y = static_cast<int>(std::floor(cy - r1)); y <= static_cast<int>(std::ceil(cy + r1)); ++y)
        for (int x = static_cast<int>(std::floor(cx - r1)); x <= static_cast<int>(std::ceil(cx + r1)); ++x) {
            const double dx = x + 0.5 - cx, dy = y + 0.5 - cy, d2 = dx * dx + dy * dy;
            if (d2 > r1 * r1 || d2 < r0 * r0) continue;
            std::uint8_t k = c;
            if (luce >= 0 && dx + dy < -0.3 * r1) k = idx(luce);
            else if (ombra >= 0 && dx + dy > 0.3 * r1) k = idx(ombra);
            t.pixel(x, y, k);
        }
}

void linea(Tela& t, double x0, double y0, double x1, double y1, std::uint8_t c, int spessore = 1) {
    const int n = static_cast<int>(std::max(std::fabs(x1 - x0), std::fabs(y1 - y0))) + 1;
    for (int i = 0; i <= n; ++i) {
        const double x = x0 + (x1 - x0) * i / n, y = y0 + (y1 - y0) * i / n;
        if (spessore <= 1) t.pixel(static_cast<int>(std::lround(x)), static_cast<int>(std::lround(y)), c);
        else disco(t, x, y, spessore / 2.0, c);
    }
}

// rettangolo con lo smusso: luce in alto a sinistra, ombra in basso a destra (rovesciato se `dentro`)
void smussato(Tela& t, Rettangolo r, std::uint8_t fondo, std::uint8_t luce, std::uint8_t ombra, bool dentro = false) {
    t.rettangolo_pieno(r.x, r.y, r.w, r.h, fondo);
    const std::uint8_t a = dentro ? ombra : luce, b = dentro ? luce : ombra;
    t.linea_orizzontale(r.x, r.x + r.w - 1, r.y, a);
    t.linea_verticale(r.x, r.y, r.y + r.h - 1, a);
    t.linea_orizzontale(r.x, r.x + r.w - 1, r.y + r.h - 1, b);
    t.linea_verticale(r.x + r.w - 1, r.y, r.y + r.h - 1, b);
}

double rad(double gradi) { return gradi * PI / 180.0; }

int spazio(const Font& f) { return f.formato() == 2 ? f.spazio_predefinito() : 1; }

void scritta_centrata(Tela& t, Rettangolo r, std::string_view s, const Font& f, std::array<int, 3> toni) {
    const int w = f.larghezza(s, 1, spazio(f), false);
    f.scrivi_toni(t, r.x + (r.w - w) / 2, r.y + (r.h - f.righe()) / 2, s, toni, 1, spazio(f));
}

}  // namespace

void pannello(Tela& t, Rettangolo r, BattelloTema b, int tono) {
    smussato(t, r, smalto(tono), smalto(tono + 2), smalto(tono - 3));
    if (b == BattelloTema::usa688) {   // viti negli angoli, con la fessura
        for (const int vx : {r.x + 4, r.x + r.w - 5})
            for (const int vy : {r.y + 4, r.y + r.h - 5}) {
                disco(t, vx + 0.5, vy + 0.5, 1.6, smalto(tono - 2));
                t.pixel(vx - 1, vy + 1, smalto(tono + 3));
                t.pixel(vx + 1, vy - 1, smalto(tono + 3));
            }
    } else {   // spigoli arrotondati dello smalto sovietico
        for (const int vx : {r.x, r.x + r.w - 1})
            for (const int vy : {r.y, r.y + r.h - 1}) t.pixel(vx, vy, smalto(tono - 4));
    }
}

Rettangolo targhetta(Tela& t, Rettangolo r, std::string_view testo, const Font& f) {
    if (r.w <= 0) r.w = f.larghezza(testo, 1, spazio(f), false) + 8;
    if (r.h <= 0) r.h = f.righe() + 2;
    t.rettangolo_pieno(r.x, r.y, r.w, r.h, idx(TARGHETTA));
    t.linea_orizzontale(r.x, r.x + r.w - 1, r.y + r.h - 1, idx(TARGHETTA + 1));
    t.linea_verticale(r.x + r.w - 1, r.y, r.y + r.h - 1, idx(TARGHETTA + 1));
    scritta_centrata(t, r, testo, f, {TARGHETTA + 3, TARGHETTA + 2, TARGHETTA + 2});
    return r;
}

void quadrante(Tela& t, int cx, int cy, int r, double valore, const ScalaQuadrante& s, StileQuadrante stile, const Font* f,
               const double* comandato) {
    const bool scuro = stile == StileQuadrante::scuro_688;
    const double xc = cx + 0.5, yc = cy + 0.5;
    // cornice: anello grigio smussato (688) o di bachelite (Alfa); il fondo
    if (scuro) anello(t, xc, yc, r - 2.5, r + 0.5, smalto(5), smalto(8), smalto(2));
    else anello(t, xc, yc, r - 3.5, r + 0.5, idx(ACCENTO), ACCENTO + 1, ACCENTO);
    const double rf = scuro ? r - 2.5 : r - 3.5;
    disco(t, xc, yc, rf, scuro ? smalto(0) : smalto(11));
    const std::uint8_t segno = scuro ? smalto(11) : idx(colori::NERO);
    const double arco = s.a_gradi - s.da_gradi, giro = std::fabs(arco) >= 359.9;
    const auto angolo = [&](double v) {
        const double u = std::clamp((v - s.minimo) / (s.massimo - s.minimo), 0.0, 1.0);
        return rad(s.da_gradi + u * arco);
    };
    for (int i = 0; i <= s.tacche; ++i) {
        if (giro && i == s.tacche) break;
        const double a = rad(s.da_gradi + arco * i / std::max(s.tacche, 1));
        const bool lunga = s.numeri > 0 && (i * s.numeri) % std::max(s.tacche, 1) == 0;
        const double r0 = rf - (lunga ? 4.0 : 2.0), r1 = rf - 0.5;
        linea(t, xc + r0 * std::sin(a), yc - r0 * std::cos(a), xc + r1 * std::sin(a), yc - r1 * std::cos(a), segno);
    }
    if (f && s.numeri > 0)
        for (int i = 0; i <= s.numeri; ++i) {
            if (giro && i == s.numeri) break;
            const double v = s.minimo + (s.massimo - s.minimo) * i / s.numeri, a = angolo(v), rn = rf - 4.0 - f->righe() * 0.55;
            char txt[16];
            std::snprintf(txt, sizeof txt, "%.0f", v * s.fattore_numeri);
            const int w = f->larghezza(txt, 1, spazio(*f), false);
            f->scrivi(t, static_cast<int>(std::lround(xc + rn * std::sin(a) - w / 2.0)),
                      static_cast<int>(std::lround(yc - rn * std::cos(a) - f->righe() / 2.0)), txt, segno, 1, spazio(*f), false);
        }
    // il comandato: un indice corto e colorato sul bordo; l'indice del valore, poi il mozzo
    if (comandato) {
        const double a = angolo(*comandato);
        linea(t, xc + (rf - 6) * std::sin(a), yc - (rf - 6) * std::cos(a), xc + rf * std::sin(a), yc - rf * std::cos(a),
              lampada(scuro ? ColoreLampada::ambra : ColoreLampada::rossa, true), 2);
    }
    const double a = angolo(valore);
    linea(t, xc - 3 * std::sin(a), yc + 3 * std::cos(a), xc + (rf - 2) * std::sin(a), yc - (rf - 2) * std::cos(a),
          scuro ? smalto(11) : idx(colori::NERO), r >= 18 ? 2 : 1);
    disco(t, xc, yc, std::max(1.5, r * 0.09), scuro ? smalto(3) : idx(ACCENTO));
}

void barra(Tela& t, Rettangolo r, double frazione, StileQuadrante stile, double comandato, int tacche) {
    const bool scuro = stile == StileQuadrante::scuro_688;
    smussato(t, r, scuro ? smalto(1) : smalto(11), scuro ? smalto(0) : smalto(8), scuro ? smalto(5) : smalto(11), true);
    const int h = static_cast<int>(std::lround((r.h - 2) * std::clamp(frazione, 0.0, 1.0)));
    t.rettangolo_pieno(r.x + 1, r.y + r.h - 1 - h, r.w - 2, h, scuro ? smalto(9) : idx(ACCENTO));
    for (int i = 1; i < tacche; ++i) {
        const int y = r.y + r.h - 1 - (r.h - 2) * i / tacche;
        t.linea_orizzontale(r.x + r.w - (i % 5 == 0 ? 4 : 2), r.x + r.w - 2, y, scuro ? smalto(11) : idx(colori::NERO));
    }
    if (comandato >= 0.0) {
        const int y = r.y + r.h - 1 - static_cast<int>(std::lround((r.h - 2) * std::clamp(comandato, 0.0, 1.0)));
        const std::uint8_t c = lampada(scuro ? ColoreLampada::ambra : ColoreLampada::rossa, true);
        for (int k = 0; k < 3; ++k) t.linea_verticale(r.x - 1 - k, y - k, y + k, c);   // freccia a sinistra
    }
}

void lampada_rettangolare(Tela& t, Rettangolo r, ColoreLampada c, bool accesa) {
    smussato(t, r, lampada(c, accesa), smalto(2), smalto(1));
    if (accesa && r.w > 4 && r.h > 3) {   // il riflesso della lente
        t.linea_orizzontale(r.x + 2, r.x + std::min(r.w - 3, 3), r.y + 1, idx(colori::BIANCO));
        t.pixel(r.x + 1, r.y + 2, idx(colori::BIANCO));
    }
}

void lampada_tonda(Tela& t, int cx, int cy, int r, ColoreLampada c, bool accesa) {
    const double xc = cx + 0.5, yc = cy + 0.5;
    disco(t, xc, yc, r + 1.0, idx(ACCENTO));
    disco(t, xc, yc, r, lampada(c, accesa));
    if (accesa) t.pixel(static_cast<int>(xc - r * 0.4), static_cast<int>(yc - r * 0.4), idx(colori::BIANCO));
}

void finestrella(Tela& t, Rettangolo r, std::string_view testo, bool accesa, const Font& f) {
    smussato(t, r, lampada(ColoreLampada::rossa, accesa), idx(ACCENTO + 1), idx(ACCENTO));
    t.rettangolo_pieno(r.x, r.y, r.w, 1, idx(ACCENTO));
    t.rettangolo_pieno(r.x, r.y + r.h - 1, r.w, 1, idx(ACCENTO));
    // accesa: la scritta chiara sul rosso vivo; spenta: incisa scura sul vetro rosso scuro (si legge, ma non brilla)
    const int s = accesa ? colori::BIANCO : ACCENTO + 1;
    scritta_centrata(t, {r.x + 1, r.y + 1, r.w - 2, r.h - 2}, testo, f, {-1, s, s});
}

void pulsante(Tela& t, Rettangolo r, std::string_view testo, ColoreLampada c, bool acceso, bool premuto, const Font& f) {
    smussato(t, r, smalto(2), smalto(premuto ? 1 : 9), smalto(premuto ? 9 : 0), false);
    const Rettangolo v{r.x + 2, r.y + 2, r.w - 4, r.h - 4};
    t.rettangolo_pieno(v.x, v.y, v.w, v.h, lampada(c, acceso));
    if (premuto) t.linea_orizzontale(v.x, v.x + v.w - 1, v.y, smalto(1));
    // la scritta: nera sulla lente accesa; sulla spenta il filtro colorato della scritta, che si legge anche con la luce
    // rossa o blu della sala (non la prende: e' una lampada)
    const int s = acceso ? colori::NERO : lampada(c, true);
    scritta_centrata(t, {v.x, v.y + (premuto ? 1 : 0), v.w, v.h}, testo, f, {-1, s, s});
}

void pulsante_coperto(Tela& t, Rettangolo r, bool coperchio_alzato, bool premuto) {
    smussato(t, r, smalto(2), smalto(6), smalto(0));
    const Rettangolo p{r.x + r.w / 4, r.y + r.h / 3, r.w / 2, r.h / 2};
    smussato(t, p, lampada(ColoreLampada::rossa, true), smalto(premuto ? 0 : 9), smalto(premuto ? 9 : 0), premuto);
    if (coperchio_alzato) {   // il coperchio aperto in su: una striscia in prospettiva sopra la cerniera
        t.rettangolo_pieno(r.x + 1, r.y + 1, r.w - 2, std::max(2, r.h / 6), lampada(ColoreLampada::rossa, false));
        t.linea_orizzontale(r.x + 1, r.x + r.w - 2, r.y + 1 + std::max(2, r.h / 6), smalto(9));
        return;
    }
    // chiuso: il guscio rosso a strisce oblique sopra tutto il pulsante, la cerniera in alto
    const Rettangolo g{r.x + 2, r.y + 3, r.w - 4, r.h - 5};
    for (int y = g.y; y < g.y + g.h; ++y)
        for (int x = g.x; x < g.x + g.w; ++x)
            t.pixel(x, y, (x + y) / 3 % 2 ? lampada(ColoreLampada::rossa, true) : lampada(ColoreLampada::rossa, false));
    t.linea_orizzontale(r.x + 1, r.x + r.w - 2, r.y + 2, smalto(10));
}

void manopola(Tela& t, int cx, int cy, int r, double angolo_gradi) {
    const double xc = cx + 0.5, yc = cy + 0.5;
    disco(t, xc, yc, r + 1.0, smalto(4));   // la rondella
    disco(t, xc, yc, r, smalto(1));
    for (int k = 0; k < 16; ++k)   // la zigrinatura
        if (k % 2) t.pixel(static_cast<int>(xc + (r - 0.5) * std::sin(k * PI / 8)), static_cast<int>(yc - (r - 0.5) * std::cos(k * PI / 8)), smalto(3));
    const double a = rad(angolo_gradi);
    linea(t, xc + 1.5 * std::sin(a), yc - 1.5 * std::cos(a), xc + (r - 1) * std::sin(a), yc - (r - 1) * std::cos(a), smalto(11));
}

void levetta(Tela& t, int cx, int cy, bool su) {
    disco(t, cx + 0.5, cy + 0.5, 3.0, smalto(4));
    disco(t, cx + 0.5, cy + 0.5, 1.6, smalto(1));
    const int verso = su ? -1 : 1;
    linea(t, cx + 0.5, cy + 0.5, cx + 0.5, cy + 0.5 + 6 * verso, idx(ACCENTO), 2);
    disco(t, cx + 0.5, cy + 0.5 + 7 * verso, 1.6, idx(ACCENTO + 1));
}

void tamburo(Tela& t, int x, int y, std::string_view cifre, double rotolamento, const Font& f) {
    const int cw = f.larghezza("0", 1, spazio(f), false) + 2, ch = f.righe() + 1;
    const Rettangolo prima = t.ritaglio();
    const auto testo = std::string(cifre);
    for (std::size_t i = 0; i < testo.size(); ++i) {
        const Rettangolo w{x + static_cast<int>(i) * (cw + 1), y, cw, ch};
        t.rettangolo_pieno(w.x - 1, w.y - 1, w.w + 2, w.h + 2, smalto(4));   // la feritoia
        t.rettangolo_pieno(w.x, w.y, w.w, w.h, idx(colori::NERO));
        // il ritaglio nella feritoia (intersecato con quello di prima)
        const int x0 = std::max(w.x, prima.x), y0 = std::max(w.y, prima.y);
        const int x1 = std::min(w.x + w.w, prima.x + prima.w), y1 = std::min(w.y + w.h, prima.y + prima.h);
        if (x1 <= x0 || y1 <= y0) continue;
        t.imposta_ritaglio({x0, y0, x1 - x0, y1 - y0});
        const char c = testo[i];
        const bool ultima = i + 1 == testo.size() && c >= '0' && c <= '9';
        const int dy = ultima ? static_cast<int>(std::lround(std::clamp(rotolamento, 0.0, 1.0) * ch)) : 0;
        f.scrivi(t, w.x + 1, w.y - dy, std::string(1, c), smalto(11), 1, spazio(f), false);
        if (ultima && dy > 0) f.scrivi(t, w.x + 1, w.y - dy + ch, std::string(1, c == '9' ? '0' : static_cast<char>(c + 1)), smalto(11), 1, spazio(f), false);
        t.imposta_ritaglio(prima);
    }
}

void sette_segmenti(Tela& t, int x, int y, std::string_view testo, ColoreLampada c, const Font& f, int scala) {
    // la finestrella nera del display, un pixel attorno alle cifre
    const int w = f.larghezza(testo, scala, spazio(f), false);
    t.rettangolo_pieno(x - 2 * scala, y - 2 * scala, w + 4 * scala, (f.righe() + 4) * scala, idx(colori::NERO));
    f.scrivi_toni(t, x, y, testo, {lampada(ColoreLampada::rossa, false), lampada(c, true), lampada(c, true)}, scala, spazio(f));
}

Rettangolo crt_rettangolare(Tela& t, Rettangolo r) {
    smussato(t, r, smalto(6), smalto(9), smalto(3));
    t.rettangolo_pieno(r.x + 2, r.y, r.w - 4, std::max(2, r.h / 14), smalto(1));   // il cappuccio sopra lo schermo
    const Rettangolo v{r.x + 5, r.y + std::max(2, r.h / 14) + 3, r.w - 10, r.h - std::max(2, r.h / 14) - 8};
    t.rettangolo_pieno(v.x - 1, v.y - 1, v.w + 2, v.h + 2, idx(ACCENTO));
    t.rettangolo_pieno(v.x, v.y, v.w, v.h, fosforo(0));
    // gli angoli del vetro arrotondati
    for (const int px : {v.x, v.x + v.w - 1})
        for (const int py : {v.y, v.y + v.h - 1}) t.pixel(px, py, idx(ACCENTO));
    return v;
}

Rettangolo crt_tondo(Tela& t, int cx, int cy, int r) {
    const double xc = cx + 0.5, yc = cy + 0.5;
    disco(t, xc, yc, r + 9.0, idx(ACCENTO));   // la cappa conica nera, vista di fronte
    anello(t, xc, yc, r + 2.0, r + 8.0, idx(ACCENTO), ACCENTO + 1, ACCENTO);
    anello(t, xc, yc, r + 0.5, r + 2.0, smalto(3));   // il collare dello schermo
    disco(t, xc, yc, r + 0.5, fosforo(0));
    return {cx - r, cy - r, 2 * r + 1, 2 * r + 1};
}

void testo_fosforo(Tela& t, int x, int y, std::string_view testo, const Font& f, int scala) {
    f.scrivi_toni(t, x, y, testo, {fosforo(2), fosforo(5), fosforo(7)}, scala, spazio(f));
}

std::vector<std::string> a_capo(const std::string& testo, const Font& f, int larghezza) {
    std::vector<std::string> out;
    const auto largo = [&](const std::string& x) { return f.larghezza(x, 1, spazio(f), false); };
    std::string riga, parola;
    const auto chiudi_parola = [&]() {
        if (parola.empty()) return;
        // la parola da sola non ci sta: si spezza (a byte interi UTF-8, al confine di un carattere)
        while (largo(parola) > larghezza && parola.size() > 1) {
            if (!riga.empty()) { out.push_back(riga); riga.clear(); }
            std::size_t n = parola.size();
            while (n > 1 && (largo(parola.substr(0, n)) > larghezza || (static_cast<unsigned char>(parola[n]) & 0xc0) == 0x80)) --n;
            out.push_back(parola.substr(0, n));
            parola.erase(0, n);
        }
        const std::string prova = riga.empty() ? parola : riga + " " + parola;
        if (largo(prova) > larghezza && !riga.empty()) {
            out.push_back(riga);
            riga = parola;
        } else {
            riga = prova;
        }
        parola.clear();
    };
    for (const char c : testo) {
        if (c == ' ') chiudi_parola();
        else parola += c;
    }
    chiudi_parola();
    if (!riga.empty()) out.push_back(riga);
    return out;
}

int testo_fosforo_nel_vetro(Tela& t, Rettangolo vetro, const std::vector<std::string>& righe, const Font& f) {
    const Rettangolo prima = t.ritaglio();
    const int x0 = std::max(vetro.x, prima.x), y0 = std::max(vetro.y, prima.y);
    const int x1 = std::min(vetro.x + vetro.w, prima.x + prima.w), y1 = std::min(vetro.y + vetro.h, prima.y + prima.h);
    if (x1 <= x0 || y1 <= y0) return 0;
    t.imposta_ritaglio({x0, y0, x1 - x0, y1 - y0});
    const int passo = f.righe() + 1, margine = 3;
    int y = vetro.y + 2, scritte = 0;
    for (const auto& r : righe)
        for (const auto& pezzo : a_capo(r, f, vetro.w - 2 * margine)) {
            if (y + f.righe() > vetro.y + vetro.h) {
                t.imposta_ritaglio(prima);
                return scritte;
            }
            testo_fosforo(t, vetro.x + margine, y, pezzo, f);
            y += passo;
            ++scritte;
        }
    t.imposta_ritaglio(prima);
    return scritte;
}

void cloche(Tela& t, Rettangolo r, double barra, double piani) {
    const double cx = r.x + r.w / 2.0;
    const double yv = r.y + r.h * (0.32 + 0.18 * std::clamp(piani, -1.0, 1.0));   // il volante sale e scende
    linea(t, cx, r.y + r.h - 1.0, cx, yv, smalto(4), 5);   // la colonna
    linea(t, cx + 1, r.y + r.h - 1.0, cx + 1, yv, smalto(7), 1);
    const double a = rad(35.0 * std::clamp(barra, -1.0, 1.0)), mezzo = r.w * 0.36, manico = r.h * 0.22;
    const double ux = std::cos(a), uy = std::sin(a);
    const double xs = cx - mezzo * ux, ys = yv - mezzo * uy, xd = cx + mezzo * ux, yd = yv + mezzo * uy;
    linea(t, xs, ys, xd, yd, smalto(3), 4);   // la barra del volante
    for (const auto& [x, y] : {std::pair{xs, ys}, std::pair{xd, yd}})   // le impugnature, perpendicolari alla barra
        linea(t, x + manico * 0.5 * uy, y - manico * 0.5 * ux, x - manico * 0.5 * uy, y + manico * 0.5 * ux, smalto(1), 5);
    disco(t, cx, yv, 3.0, smalto(6));   // il mozzo
}

void albero_di_natale(Tela& t, Rettangolo r, const std::vector<bool>& aperte, int colonne) {
    smussato(t, r, smalto(1), smalto(0), smalto(4), true);
    if (aperte.empty() || colonne <= 0) return;
    const int righe = static_cast<int>((aperte.size() + static_cast<std::size_t>(colonne) - 1) / static_cast<std::size_t>(colonne));
    const double cw = (r.w - 2.0) / colonne, ch = (r.h - 2.0) / righe;
    for (std::size_t i = 0; i < aperte.size(); ++i) {
        const double xc = r.x + 1 + cw * (static_cast<double>(i % static_cast<std::size_t>(colonne)) + 0.5);
        const double yc = r.y + 1 + ch * (static_cast<double>(i / static_cast<std::size_t>(colonne)) + 0.5);
        if (aperte[i]) disco(t, xc, yc, std::max(1.2, std::min(cw, ch) * 0.3), lampada(ColoreLampada::rossa, true));
        else {
            const int w = std::max(2, static_cast<int>(cw * 0.6));
            t.rettangolo_pieno(static_cast<int>(xc) - w / 2, static_cast<int>(yc), w, 2, lampada(ColoreLampada::verde, true));
        }
    }
}

}  // namespace m688
