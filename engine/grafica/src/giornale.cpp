#include "grafica/giornale.hpp"

#include <algorithm>
#include <cctype>
#include <cmath>

#include "nucleo/geometria.hpp"

namespace m688 {

using namespace cerimonia;

namespace {

std::string minuscolo(std::string_view s) {   // le minuscole del font sono le maiuscole grandi 7x7
    std::string r(s);
    for (auto& c : r) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    return r;
}
std::string maiuscolo(std::string_view s) {
    std::string r(s);
    for (auto& c : r) c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
    return r;
}

// righe a capo in una larghezza (a parole; '\n' forza)
std::vector<std::string> a_capo(const Font& f, std::string_view testo, int w, int scala) {
    std::vector<std::string> out;
    std::string riga, parola;
    auto chiudi_parola = [&]() {
        if (parola.empty()) return;
        while (f.larghezza(parola, scala) > w && parola.size() > 1) {   // parola piu' larga della colonna: si spezza
            std::size_t n = parola.size() - 1;
            while (n > 1 && f.larghezza(parola.substr(0, n), scala) > w) --n;
            if (!riga.empty()) { out.push_back(riga); riga.clear(); }
            out.push_back(parola.substr(0, n));
            parola = parola.substr(n);
        }
        const std::string prova = riga.empty() ? parola : riga + " " + parola;
        if (f.larghezza(prova, scala) > w && !riga.empty()) {
            out.push_back(riga);
            riga = parola;
        } else {
            riga = prova;
        }
        parola.clear();
    };
    for (const char c : testo) {
        if (c == ' ' || c == '\n') {
            chiudi_parola();
            if (c == '\n') { out.push_back(riga); riga.clear(); }
        } else {
            parola += c;
        }
    }
    chiudi_parola();
    if (!riga.empty()) out.push_back(riga);
    return out;
}

// carta di giornale ingiallita, con la piega orizzontale e le macchie
void carta_giornale(Tela& t, Rettangolo r, std::uint32_t seme) {
    const int s = static_cast<int>(seme);
    for (int y = r.y; y < r.y + r.h; ++y)
        for (int x = r.x; x < r.x + r.w; ++x) {
            const double fibra = 0.45 * caso(x, y, s) + 0.3 * caso(x / 9, y / 9, s + 1);
            const int b = std::min({x - r.x, y - r.y, r.x + r.w - 1 - x, r.y + r.h - 1 - y});
            const double piega = std::exp(-std::pow((y - (r.y + r.h * 0.5)) / 2.0, 2.0));
            const double tt = 3.6 + fibra - 1.4 * piega - (b < 5 ? 0.9 * (5 - b) / 5.0 : 0.0);
            t.pixel(x, y, static_cast<std::uint8_t>(CARTA + retina(tt, x, y, N_CARTA - 1)));
        }
}

void filetto(Tela& t, int x0, int x1, int y, int spessore = 1) {
    for (int k = 0; k < spessore; ++k)
        for (int x = x0; x < x1; ++x) t.pixel(x, y + k, NERO + 1);
}

// testo finto: righe di «parole» d'inchiostro alte 3 pixel
void righe_finte(Tela& t, Rettangolo r, std::uint32_t seme, int passo = 6) {
    const int s = static_cast<int>(seme);
    for (int y = r.y; y + 3 <= r.y + r.h; y += passo) {
        const bool ultima = caso(y, 1, s) < 0.12;
        const int fine = ultima ? r.x + static_cast<int>(r.w * (0.3 + 0.5 * caso(y, 2, s))) : r.x + r.w;
        int x = r.x + (caso(y, 3, s) < 0.1 ? 4 : 0);
        while (x < fine) {
            const int lung = 2 + static_cast<int>(caso(x, y, s) * 7);
            for (int xx = x; xx < std::min(x + lung, fine); ++xx)
                for (int yy = 0; yy < 3; ++yy)
                    if (caso(xx, y + yy, s + 3) < 0.62) t.pixel(xx, y + yy, static_cast<std::uint8_t>(NERO + (yy == 1 ? 1 : 2)));
            x += lung + 2;
        }
    }
}

// foto a retino: il cielo, il mare e la sagoma di un sommergibile in superficie con la vela
void foto_retino(Tela& t, Rettangolo r, std::uint32_t seme) {
    const double cx = r.x + r.w * (0.45 + 0.1 * caso(static_cast<int>(seme), 4));
    const double oriz = r.y + r.h * 0.55;
    auto luce = [&](double x, double y) {
        double l = y < oriz ? 0.75 - 0.25 * (y - r.y) / (oriz - r.y) : 0.45 - 0.15 * std::sin(x * 0.4 + y * 0.9);
        const double scafo = std::fabs(x - cx) < r.w * 0.38 * std::sqrt(std::max(0.0, 1 - std::pow((y - oriz) / (r.h * 0.07), 2.0))) && y > oriz - r.h * 0.05 && y < oriz + r.h * 0.04;
        const bool vela = x > cx - r.w * 0.05 && x < cx + r.w * 0.09 && y > oriz - r.h * 0.3 && y < oriz;
        if (scafo || vela) l = 0.08;
        return l;
    };
    const int passo = 3;
    for (int y = r.y; y < r.y + r.h; ++y)
        for (int x = r.x; x < r.x + r.w; ++x) {
            // retino a 45 gradi: punto tanto piu' grosso quanto piu' l'immagine e' scura
            const double u = (x + y) / static_cast<double>(passo) / 1.414, v = (x - y) / static_cast<double>(passo) / 1.414;
            const double du = u - std::round(u), dv = v - std::round(v);
            const double d = std::sqrt(du * du + dv * dv);
            const double scuro = 1.0 - luce(x, y);
            t.pixel(x, y, static_cast<std::uint8_t>(d < 0.62 * std::sqrt(scuro) ? NERO + 2 : CARTA + 4));
        }
    for (int x = r.x; x < r.x + r.w; ++x) { t.pixel(x, r.y, NERO + 1); t.pixel(x, r.y + r.h - 1, NERO + 1); }
    for (int y = r.y; y < r.y + r.h; ++y) { t.pixel(r.x, y, NERO + 1); t.pixel(r.x + r.w - 1, y, NERO + 1); }
}

void stella_rossa(Tela& t, int cx, int cy, int R) {
    for (int y = -R; y <= R; ++y)
        for (int x = -R; x <= R; ++x) {
            const double a = std::atan2(x, -y), rr = std::hypot(x, y);
            const double ff = std::fmod(a + 2 * PI + PI / 5, 2 * PI / 5) - PI / 5;
            if (rr <= R * (0.42 + 0.58 * std::pow(std::fabs(std::cos(ff * 2.5)), 6.0)) || rr < R * 0.42)
                t.pixel(cx + x, cy + y, static_cast<std::uint8_t>(SMALTO + (x < 0 ? 4 : 2)));
        }
}

}  // namespace

void disegna_giornale(Tela& t, const Font& f, Rettangolo r, Marina m, std::string_view testata, std::string_view titolo,
                      std::string_view testo, std::string_view data, std::uint32_t seme) {
    carta_giornale(t, r, seme);
    const int mx = r.x + 10, mw = r.w - 20;
    int y = r.y + 8;
    const bool usn = m == Marina::usn;
    // testata grande (le minuscole del font sono le maiuscole grandi), centrata, con i filetti
    std::string tes = minuscolo(testata);
    int sc = 3;
    while (sc > 1 && f.larghezza(tes, sc) > mw - (usn ? 0 : 70)) --sc;
    if (sc == 1 && f.larghezza(maiuscolo(testata), 2) <= mw - (usn ? 0 : 70)) {   // le maiuscole piccole a scala 2
        tes = maiuscolo(testata);
        sc = 2;
    }
    const int tw = f.larghezza(tes, sc);
    f.scrivi(t, r.x + (r.w - tw) / 2, y, tes, static_cast<std::uint8_t>(NERO + 1), sc);
    if (!usn) {   // la Pravda porta gli ordini accanto alla testata
        stella_rossa(t, r.x + (r.w - tw) / 2 - 22, y + 10, 9);
        stella_rossa(t, r.x + (r.w + tw) / 2 + 22, y + 10, 9);
    }
    y += f.altezza_riga(sc) + 2;
    filetto(t, mx, mx + mw, y, 2);
    y += 4;
    if (!data.empty()) f.scrivi(t, mx, y, maiuscolo(data), static_cast<std::uint8_t>(NERO + 2), 1);
    const char* prezzo = usn ? "25 CENTS" : "3 KOP.";
    f.scrivi(t, mx + mw - f.larghezza(prezzo, 1), y, prezzo, static_cast<std::uint8_t>(NERO + 2), 1);
    y += f.altezza_riga(1);
    filetto(t, mx, mx + mw, y, 1);
    y += 6;
    if (usn) {
        // titolo su tutta la pagina, poi foto a sinistra e il testo in due colonne
        for (const auto& riga : a_capo(f, minuscolo(titolo), mw, 2)) {
            f.scrivi(t, r.x + (r.w - f.larghezza(riga, 2)) / 2, y, riga, static_cast<std::uint8_t>(NERO + 0), 2);
            y += f.altezza_riga(2);
        }
        y += 2;
        filetto(t, mx, mx + mw, y, 1);
        y += 5;
        const int ncol = mw >= 400 ? 3 : 2;
        const int colonna = (mw - 6 * (ncol - 1)) / ncol;
        // foto sulle prime due colonne (su tutta la pagina se le colonne sono due), didascalia, testo in colonne
        const int fw = ncol == 3 ? colonna * 2 + 6 : mw;
        const Rettangolo foto{mx, y, fw, static_cast<int>(fw * (ncol == 3 ? 0.5 : 0.3))};
        foto_retino(t, foto, seme);
        int yc = foto.y + foto.h + 3;
        f.scrivi(t, mx, yc, "IL BATTELLO IN UNA FOTO D'ARCHIVIO", static_cast<std::uint8_t>(NERO + 2), 1);
        yc += f.altezza_riga(1) + 2;
        const int passo = f.altezza_riga(1) - 2, fondo = r.y + r.h - 8;
        auto righe = a_capo(f, maiuscolo(testo), colonna, 1);
        std::size_t k = 0;
        std::vector<int> fine_col(static_cast<std::size_t>(ncol), yc);
        // colonne sotto la foto, poi (con tre colonne) la terza dall'alto
        for (int c = 0; c < ncol; ++c) {
            int yy = (ncol == 3 && c == 2) ? y : yc;
            const int xx = mx + c * (colonna + 6);
            while (yy + passo <= fondo && k < righe.size()) {
                f.scrivi(t, xx, yy, righe[k++], static_cast<std::uint8_t>(NERO + 1), 1);
                yy += passo;
            }
            fine_col[static_cast<std::size_t>(c)] = yy;
        }
        // il resto della pagina: altri articoli
        for (int c = 0; c < ncol; ++c) {
            const int yy = fine_col[static_cast<std::size_t>(c)];
            if (yy > fondo - 30) continue;
            const int xx = mx + c * (colonna + 6);
            filetto(t, xx, xx + colonna, yy + 3, 1);
            const char* rubrica = c == 0 ? "DAL CONGRESSO" : c == 1 ? "BORSA DI NEW YORK" : "MERCATI";
            f.scrivi(t, xx, yy + 7, rubrica, static_cast<std::uint8_t>(NERO + 1), 1);
            righe_finte(t, {xx, yy + 19, colonna, fondo - (yy + 19)}, seme + static_cast<std::uint32_t>(c));
        }
        for (int c = 1; c < ncol; ++c)   // filetti verticali fra le colonne
            for (int yy = (ncol == 3 && c == 2) ? y : yc; yy < fondo; ++yy) t.pixel(mx + c * (colonna + 6) - 3, yy, NERO + 2);
    } else {
        // prima pagina sovietica: colonne fitte e, in un riquadro, il comunicato TASS
        const int colonna = (mw - 12) / 3;
        righe_finte(t, {mx, y, colonna, r.y + r.h - 10 - y}, seme + 1);
        const Rettangolo box{mx + colonna + 6, y, 2 * colonna + 6, r.y + r.h - 10 - y};
        for (int yy = box.y; yy < box.y + box.h; ++yy)
            for (int xx = box.x; xx < box.x + box.w; ++xx)
                if (xx == box.x || xx == box.x + box.w - 1 || yy == box.y || yy == box.y + box.h - 1 || xx == box.x + 2 ||
                    xx == box.x + box.w - 3 || yy == box.y + 2 || yy == box.y + box.h - 3)
                    t.pixel(xx, yy, NERO + 1);
        int yb = box.y + 8;
        const std::string tass = "soobshchenie tass";   // СООБЩЕНИЕ ТАСС, traslitterato
        f.scrivi(t, box.x + (box.w - f.larghezza(tass, 1)) / 2, yb, tass, static_cast<std::uint8_t>(NERO + 0), 1);
        yb += f.altezza_riga(1) + 4;
        for (const auto& riga : a_capo(f, minuscolo(titolo), box.w - 16, 1)) {
            f.scrivi(t, box.x + (box.w - f.larghezza(riga, 1)) / 2, yb, riga, static_cast<std::uint8_t>(NERO + 1), 1);
            yb += f.altezza_riga(1);
        }
        filetto(t, box.x + 20, box.x + box.w - 20, yb + 2, 1);
        yb += 8;
        for (const auto& riga : a_capo(f, maiuscolo(testo), box.w - 16, 1)) {
            if (yb + f.altezza_riga(1) > box.y + box.h - 6) break;
            f.scrivi(t, box.x + 8, yb, riga, static_cast<std::uint8_t>(NERO + 1), 1);
            yb += f.altezza_riga(1) - 2;
        }
        for (int yy = y; yy < r.y + r.h - 8; ++yy) t.pixel(mx + colonna + 2, yy, NERO + 2);
    }
}

int disegna_tabella(Tela& t, const Font& f, Rettangolo r, const std::vector<ColonnaTabella>& colonne,
                    const std::vector<std::vector<std::string>>& righe, std::string_view titolo) {
    double tot = 0;
    for (const auto& c : colonne) tot += c.larghezza;
    std::vector<int> x;
    double acc = 0;
    for (const auto& c : colonne) {
        x.push_back(r.x + static_cast<int>(r.w * acc / tot));
        acc += c.larghezza;
    }
    x.push_back(r.x + r.w);
    int y = r.y;
    const int h = f.altezza_riga(1) - 1;
    if (!titolo.empty()) {
        f.scrivi(t, r.x, y, maiuscolo(titolo), static_cast<std::uint8_t>(NERO + 0), 1);
        y += h + 3;
    }
    for (std::size_t i = 0; i < colonne.size(); ++i) f.scrivi(t, x[i] + 2, y, maiuscolo(colonne[i].titolo), static_cast<std::uint8_t>(NERO + 1), 1);
    y += h;
    filetto(t, r.x, r.x + r.w, y, 1);
    filetto(t, r.x, r.x + r.w, y + 2, 1);
    y += 5;
    for (std::size_t k = 0; k < righe.size() && y + h <= r.y + r.h; ++k) {
        if (k % 2 == 1)   // riga alternata appena piu' scura
            for (int yy = y - 1; yy < y + h - 2; ++yy)
                for (int xx = r.x; xx < r.x + r.w; ++xx)
                    if ((xx + yy) % 2 == 0) t.pixel(xx, yy, static_cast<std::uint8_t>(CARTA + 2));
        for (std::size_t i = 0; i < colonne.size() && i < righe[k].size(); ++i) {
            std::string c = maiuscolo(righe[k][i]);
            while (!c.empty() && f.larghezza(c, 1) > x[i + 1] - x[i] - 4) c.pop_back();
            f.scrivi(t, x[i] + 2, y, c, static_cast<std::uint8_t>(NERO + (i == 0 ? 2 : 1)), 1);
        }
        y += h;
    }
    filetto(t, r.x, r.x + r.w, y, 1);
    for (std::size_t i = 1; i < colonne.size(); ++i)
        for (int yy = r.y + (titolo.empty() ? 0 : h + 3); yy < y; yy += 2) t.pixel(x[i] - 1, yy, static_cast<std::uint8_t>(NERO + 3));
    return y + 2;
}

}  // namespace m688
