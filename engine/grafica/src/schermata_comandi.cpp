#include "grafica/schermata_comandi.hpp"

#include <algorithm>
#include <string>
#include <vector>

#include "grafica/font.hpp"
#include "grafica/lamiera.hpp"
#include "grafica/palette.hpp"
#include "grafica/quadro.hpp"
#include "grafica/testo_font.hpp"

namespace m688 {

namespace {

constexpr int PIEDE_Y = 428, TASTI_PIEDE_Y = PIEDE_Y + 22, ALTO_TASTI_PIEDE = 26;   // come le altre pagine di lamiera
constexpr int ALTO_LINGUETTA = 25, SOTTO_LINGUETTE = 4;
constexpr int COLONNA_TASTI = 140, PASSO_TESTO = 11, ALTO_CAPPUCCIO = 12, PASSO_CAPPUCCIO = 14;
constexpr int ALTO_TITOLO = 20, FONDO_RISERVATO = 12, ARIA_RIGA = 3;

// Un elemento di una pagina del foglio, con la sua altezza
struct Elemento {
    enum class Tipo { consiglio, titolo, riga } tipo = Tipo::riga;
    std::string titolo;
    std::vector<std::string> testo;                  // a capo
    std::vector<std::vector<std::string>> cappucci;  // righe di cappucci
    int h = 0;
};

std::vector<std::string> spezza_tasti(const std::string& tasti) {
    std::vector<std::string> v;
    std::size_t da = 0;
    while (da <= tasti.size()) {
        const std::size_t k = tasti.find(", ", da);
        const std::string s = tasti.substr(da, k == std::string::npos ? std::string::npos : k - da);
        if (!s.empty()) v.push_back(testo_per_font(s));
        if (k == std::string::npos) break;
        da = k + 2;
    }
    return v;
}

int largo_cappuccio(const Font& f, const std::string& s) { return f.larghezza(s, 1) + 8; }

std::vector<std::vector<Elemento>> impagina(const Font& f, const SchedaComandi& s, int w, int h) {
    std::vector<Elemento> tutti;
    for (const auto& c : s.consigli) {
        Elemento e;
        e.tipo = Elemento::Tipo::consiglio;
        e.testo = lamiera::a_capo(f, testo_per_font(c), w - 8);
        e.h = static_cast<int>(e.testo.size()) * PASSO_TESTO + 2;
        tutti.push_back(std::move(e));
    }
    for (const auto& g : s.gruppi) {
        if (!g.titolo.empty()) {
            Elemento e;
            e.tipo = Elemento::Tipo::titolo;
            e.titolo = testo_per_font(g.titolo);
            e.h = ALTO_TITOLO;
            tutti.push_back(std::move(e));
        }
        for (const auto& r : g.righe) {
            Elemento e;
            e.testo = lamiera::a_capo(f, testo_per_font(r.cosa), w - COLONNA_TASTI);
            std::vector<std::string> fila;
            int x = 0;
            for (auto c : spezza_tasti(r.tasti)) {
                while (c.size() > 1 && largo_cappuccio(f, c) > COLONNA_TASTI - 6) c.pop_back();   // un nome troppo lungo non invade il testo
                const int l = largo_cappuccio(f, c);
                if (!fila.empty() && x + l > COLONNA_TASTI - 6) {
                    e.cappucci.push_back(std::move(fila));
                    fila.clear();
                    x = 0;
                }
                fila.push_back(c);
                x += l + 3;
            }
            if (!fila.empty()) e.cappucci.push_back(std::move(fila));
            e.h = std::max(static_cast<int>(e.cappucci.size()) * PASSO_CAPPUCCIO, static_cast<int>(e.testo.size()) * PASSO_TESTO) + ARIA_RIGA;
            tutti.push_back(std::move(e));
        }
    }
    std::vector<std::vector<Elemento>> pagine(1);
    int y = 0;
    for (std::size_t i = 0; i < tutti.size(); ++i) {
        // un titolo non resta in fondo da solo: va giu' insieme alla sua prima riga
        const int serve = tutti[i].h + (tutti[i].tipo == Elemento::Tipo::titolo && i + 1 < tutti.size() ? tutti[i + 1].h : 0);
        if (y + serve > h && !pagine.back().empty()) {
            pagine.emplace_back();
            y = 0;
        }
        y += tutti[i].h;
        pagine.back().push_back(std::move(tutti[i]));
    }
    return pagine;
}

void disegna_linguetta(Tela& t, const Font& f, const Font& piccolo, Rettangolo r, const std::string& etichetta, bool scelta) {
    lamiera::tasto(t, r, "", ColoreLampada::ambra, scelta ? lamiera::Stato::evidenziato : lamiera::Stato::normale, false, f);
    const std::uint8_t c = scelta ? colori::NERO : colori::BIANCO;
    const std::string s = testo_per_font(etichetta);
    const std::size_t sp = s.find(' ');
    const bool con_tasto = sp != std::string::npos && s[0] == 'F' && sp >= 2 && sp <= 3;
    std::string nome = con_tasto ? s.substr(sp + 1) : s;
    while (!nome.empty() && piccolo.larghezza(nome, 1) > r.w - 2) nome.pop_back();
    if (con_tasto) {
        const std::string k = s.substr(0, sp);
        f.scrivi(t, r.x + (r.w - f.larghezza(k, 1)) / 2, r.y + 3, k, c, 1);
        piccolo.scrivi(t, r.x + (r.w - piccolo.larghezza(nome, 1)) / 2, r.y + 14, nome, c, 1);
    } else {
        piccolo.scrivi(t, r.x + (r.w - piccolo.larghezza(nome, 1)) / 2, r.y + (r.h - piccolo.righe()) / 2, nome, c, 1);
    }
}

void disegna_elemento(Tela& t, const Font& f, const Elemento& e, Rettangolo a, int y, bool alfa) {
    switch (e.tipo) {
        case Elemento::Tipo::consiglio: {   // a penna rossa col segno a margine, come «IN GIOCO» nel Museo
            t.rettangolo_pieno(a.x + 1, y - 1, 2, e.h - 2, colori::ROSSO);
            int yy = y;
            for (const auto& r : e.testo) {
                f.scrivi(t, a.x + 8, yy, r, colori::ROSSO, 1);
                yy += PASSO_TESTO;
            }
            break;
        }
        case Elemento::Tipo::titolo: {
            const int yt = y + 6;
            if (alfa) {   // sull'Alfa i titoli su una targa rossa, come nella pagina SQUADRE
                t.rettangolo_pieno(a.x - 2, yt - 2, f.larghezza(e.titolo, 1) + 4, 11, colori::ROSSO);
                f.scrivi(t, a.x, yt, e.titolo, colori::BIANCO, 1);
            } else {
                f.scrivi(t, a.x, yt, e.titolo, colori::NERO, 1);
                t.linea_orizzontale(a.x, a.x + a.w - 1, yt + 9, colori::NERO);
            }
            break;
        }
        case Elemento::Tipo::riga: {
            int yc = y;
            for (const auto& fila : e.cappucci) {
                int x = a.x;
                for (const auto& c : fila) {
                    const int l = largo_cappuccio(f, c);
                    lamiera::tasto_tastiera(t, {x, yc, l, ALTO_CAPPUCCIO}, c, f);
                    x += l + 3;
                }
                yc += PASSO_CAPPUCCIO;
            }
            int yy = y + 2;
            for (const auto& r : e.testo) {
                f.scrivi(t, a.x + COLONNA_TASTI, yy, r, colori::NERO, 1);
                yy += PASSO_TESTO;
            }
            break;
        }
    }
}

}  // namespace

int disegna_schermata_comandi(Tela& t, const Font& f, const Font& piccolo, const Font& targa, const DatiSchermataComandi& d,
                              std::vector<AreaComando>* aree) {
    if (aree) aree->clear();
    const Rettangolo u = quadro::consolle(t, targa, d.alfa ? "KOMANDY" : "COMANDI DI BORDO");
    const int n = static_cast<int>(d.schede.size());
    const int scelta = n ? std::clamp(d.scheda, 0, n - 1) : 0;

    // le linguette, tutte uguali sulla larghezza
    for (int i = 0; i < n; ++i) {
        const int x0 = u.x + 2 + i * (u.w - 4) / n, x1 = u.x + 2 + (i + 1) * (u.w - 4) / n;
        const Rettangolo r{x0, u.y, x1 - x0 - 2, ALTO_LINGUETTA};
        disegna_linguetta(t, f, piccolo, r, d.schede[static_cast<std::size_t>(i)].etichetta, i == scelta);
        if (aree) aree->push_back({r, i, -1, false});
    }

    const int y_foglio = u.y + ALTO_LINGUETTA + SOTTO_LINGUETTE;
    const Rettangolo a = lamiera::foglio(t, {u.x, y_foglio, u.w, PIEDE_Y - 4 - y_foglio});
    int pagine = 1, pagina = 0;
    if (n) {
        const auto impaginate = impagina(f, d.schede[static_cast<std::size_t>(scelta)], a.w, a.h - FONDO_RISERVATO);
        pagine = static_cast<int>(impaginate.size());
        pagina = std::clamp(d.pagina, 0, pagine - 1);
        int y = a.y;
        for (const auto& e : impaginate[static_cast<std::size_t>(pagina)]) {
            disegna_elemento(t, f, e, a, y, d.alfa);
            y += e.h;
        }
        if (impaginate[static_cast<std::size_t>(pagina)].empty()) f.scrivi(t, a.x, a.y, "NESSUN COMANDO", colori::GRIGIO_SCURO, 1);
    }
    const std::string conto = (pagina + 1 < pagine ? "(SEGUE) " : "(FINE) ") + std::to_string(pagina + 1) + "/" + std::to_string(pagine);
    if (pagine > 1) f.scrivi(t, a.x + a.w - f.larghezza(conto, 1), a.y + a.h - f.righe(), conto, colori::BLU, 1);

    const std::string di_pagine = std::to_string(pagina + 1) + "/" + std::to_string(pagine);
    lamiera::piede(t, f, PIEDE_Y, "F11 O ESC CHIUDE   TAB SCHEDA   FRECCE PAGINA",
                   {{"ESC  CHIUDE", ColoreLampada::rossa},
                    {"TAB  SCHEDA " + std::to_string(scelta + 1) + "/" + std::to_string(std::max(n, 1)), ColoreLampada::ambra,
                     n > 1 ? lamiera::Stato::normale : lamiera::Stato::disabilitato},
                    {"FRECCE  PAGINA " + di_pagine, ColoreLampada::verde, pagine > 1 ? lamiera::Stato::normale : lamiera::Stato::disabilitato}},
                   pagine > 1 && pagina + 1 < pagine ? 2 : 0);
    if (aree) {
        aree->push_back({{8, TASTI_PIEDE_Y, 202, ALTO_TASTI_PIEDE}, -1, -1, true});
        if (n > 1) aree->push_back({{218, TASTI_PIEDE_Y, 202, ALTO_TASTI_PIEDE}, (scelta + 1) % n, -1, false});
        if (pagine > 1) aree->push_back({{428, TASTI_PIEDE_Y, 202, ALTO_TASTI_PIEDE}, -1, (pagina + 1) % pagine, false});
    }
    return pagine;
}

}  // namespace m688
