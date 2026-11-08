#include "grafica/pagina_squadre.hpp"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdio>

#include "grafica/font.hpp"
#include "grafica/palette.hpp"
#include "grafica/spaccato.hpp"

namespace m688 {

namespace {

constexpr int PRIMA_RIGA = 14, PASSO_RIGA = 10, LARGO_SISTEMI = 408;
constexpr int X_NOME = 22, X_BARRA = 176, LARGO_BARRA = 100, X_VALORE = 282, X_SQUADRA = 312;
constexpr int ROSSO_ACCESO = 12;   // colori:: non lo nomina

// sull'Alfa i titoli su una targa rossa, come RITM e GAMMA del pult
void titolo(Tela& t, const Font& f, int x, int y, const std::string& s, bool alfa) {
    if (alfa) t.rettangolo_pieno(x - 2, y - 2, f.larghezza(s, 1) + 4, 11, colori::ROSSO);
    f.scrivi(t, x, y, s, alfa ? colori::BIANCO : colori::CIANO_CHIARO, 1);
}

std::string maiuscolo(std::string s) {
    for (auto& c : s) c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
    return s;
}

std::uint8_t colore_efficienza(double e) {
    return static_cast<std::uint8_t>(e >= 0.7 ? colori::VERDE_CHIARO : e >= 0.3 ? colori::GIALLO : ROSSO_ACCESO);
}

// la barra dell'efficienza; oltre il massimo a puntini, perche' le riparazioni li' non arrivano
void barra(Tela& t, int x, int y, int w, double e, double massimo) {
    t.rettangolo_pieno(x, y, w, 5, colori::GRIGIO_SCURO);
    const int fine = static_cast<int>(std::lround(std::clamp(massimo, 0.0, 1.0) * w));
    for (int xx = x + fine; xx < x + w; ++xx)
        for (int yy = y; yy < y + 5; ++yy)
            t.pixel(xx, yy, (xx + yy) & 1 ? colori::NERO : colori::GRIGIO_SCURO);
    t.rettangolo_pieno(x, y, static_cast<int>(std::lround(std::clamp(e, 0.0, 1.0) * w)), 5, colore_efficienza(e));
}

void cornice(Tela& t, Rettangolo r, std::uint8_t c) {
    t.linea_orizzontale(r.x, r.x + r.w - 1, r.y, c);
    t.linea_orizzontale(r.x, r.x + r.w - 1, r.y + r.h - 1, c);
    t.linea_verticale(r.x, r.y, r.y + r.h - 1, c);
    t.linea_verticale(r.x + r.w - 1, r.y, r.y + r.h - 1, c);
}

std::string stato_porta(const PortaOtsek& p, bool alfa) {
    switch (p.stato) {
        case PortaOtsek::Stato::aperto: return alfa ? "OTKRYT" : "APERTO";
        case PortaOtsek::Stato::comando: return alfa ? "POST" : "COMANDO";
        case PortaOtsek::Stato::chiuso: return alfa ? "ZAKRYT" : "CHIUSO";
        case PortaOtsek::Stato::in_chiusura: break;
    }
    char b[24];
    std::snprintf(b, sizeof b, "FUORI %d S", p.conto_s);
    return b;
}

std::uint8_t colore_porta(PortaOtsek::Stato s) {
    switch (s) {
        case PortaOtsek::Stato::aperto: return colori::VERDE_CHIARO;
        case PortaOtsek::Stato::comando: return colori::GRIGIO;
        case PortaOtsek::Stato::chiuso: return ROSSO_ACCESO;
        case PortaOtsek::Stato::in_chiusura: return colori::GIALLO;
    }
    return colori::GRIGIO;
}

}  // namespace

void disegna_pagina_squadre(Tela& t, const Font& f, Rettangolo r, const DatiPaginaSquadre& d) {
    t.rettangolo_pieno(r.x, r.y, r.w, r.h, colori::NERO);
    cornice(t, r, colori::GRIGIO_SCURO);
    t.linea_verticale(r.x + LARGO_SISTEMI, r.y + 1, r.y + r.h - 2, colori::GRIGIO_SCURO);

    // i sistemi: quelli guasti in bianco, gli altri restano indietro
    titolo(t, f, r.x + 4, r.y + 3, d.alfa ? "OCHERED REMONTA" : "ORDINE DI RIPARAZIONE", d.alfa);
    titolo(t, f, r.x + X_SQUADRA, r.y + 3, d.alfa ? "PARTIJA" : "SQUADRA", d.alfa);
    if (d.cursore >= 0 && d.cursore < static_cast<int>(d.righe.size()))   // il fondo prima delle scritte: la riga sopra ci scende
        t.rettangolo_pieno(r.x + 1, r.y + PRIMA_RIGA + d.cursore * PASSO_RIGA - 1, LARGO_SISTEMI - 1, PASSO_RIGA, colori::BLU);
    for (std::size_t i = 0; i < d.righe.size(); ++i) {
        const RigaSquadre& s = d.righe[i];
        const int y = r.y + PRIMA_RIGA + static_cast<int>(i) * PASSO_RIGA;
        const bool su_di_lui = static_cast<int>(i) == d.cursore;
        const bool guasto = s.efficienza < 0.995;
        f.scrivi(t, r.x + 4, y, std::to_string(i + 1), su_di_lui ? colori::GIALLO : colori::GRIGIO, 1);
        f.scrivi(t, r.x + X_NOME, y, maiuscolo(s.nome), su_di_lui || guasto ? colori::BIANCO : colori::GRIGIO, 1);
        barra(t, r.x + X_BARRA, y + 1, LARGO_BARRA, s.efficienza, s.massimo);
        f.scrivi(t, r.x + X_VALORE, y, std::to_string(static_cast<int>(std::lround(s.efficienza * 100))), guasto ? colore_efficienza(s.efficienza) : colori::GRIGIO, 1);
        if (s.squadre > 0) f.scrivi(t, r.x + X_SQUADRA, y, (d.alfa ? "PARTIJA " : "SQ ") + std::to_string(s.squadre), colori::GIALLO, 1);
    }

    // i compartimenti: nome e porte, poi acqua, fuoco, ossigeno; quello scelto col Tab ha la cornice gialla
    const int x_porte = r.x + LARGO_SISTEMI + 5, largo_porte = r.w - LARGO_SISTEMI - 9;
    titolo(t, f, x_porte, r.y + 3, d.alfa ? "OTSEKI" : "COMPARTIMENTI", d.alfa);
    const int n = std::max(static_cast<int>(d.porte.size()), 1);
    const int passo = std::min(24, (r.h - 28) / n);
    for (int i = 0; i < static_cast<int>(d.porte.size()); ++i) {
        const PortaOtsek& p = d.porte[static_cast<std::size_t>(i)];
        const int y = r.y + PRIMA_RIGA + i * passo;
        if (i == d.scelto) cornice(t, {x_porte - 2, y - 2, largo_porte + 4, passo}, colori::GIALLO);
        f.scrivi(t, x_porte + 1, y, maiuscolo(p.nome), colori::BIANCO, 1);
        const std::string st = stato_porta(p, d.alfa);
        f.scrivi(t, x_porte + largo_porte - f.larghezza(st, 1) - 1, y, st, colore_porta(p.stato), 1);
        int x = x_porte + 1;
        const auto voce = [&](const char* nome, double v, std::uint8_t c) {
            x = f.scrivi(t, x, y + 9, std::string(nome) + " " + std::to_string(static_cast<int>(std::lround(v * 100))) + "  ", c, 1);
        };
        if (p.acqua >= 0.005) voce(d.alfa ? "VODA" : "ACQUA", p.acqua, colori::CIANO_CHIARO);
        if (p.fuoco >= 0.005) voce(d.alfa ? "OGON" : "FUOCO", p.fuoco, colori::GIALLO);
        if (p.ossigeno < 0.195) voce("O2", p.ossigeno, p.ossigeno < 0.12 ? ROSSO_ACCESO : colori::GIALLO);
        std::string dx;
        if (p.uomini_dentro > 0) dx = std::to_string(p.uomini_dentro) + (d.alfa ? " V OTSEKE" : " DENTRO");
        else if (p.squadre > 0) dx = (d.alfa ? "PARTIJA " : "SQ ") + std::to_string(p.squadre);
        if (!dx.empty()) f.scrivi(t, x_porte + largo_porte - f.larghezza(dx, 1) - 1, y + 9, dx, p.uomini_dentro > 0 ? ROSSO_ACCESO : colori::GIALLO, 1);
    }
    char riga[48];
    std::snprintf(riga, sizeof riga, d.alfa ? "PARTII %d, SVOBODNY %d" : "SQUADRE %d, LIBERE %d", d.squadre, d.libere);
    f.scrivi(t, x_porte, r.y + r.h - 11, riga, d.libere > 0 ? colori::VERDE_CHIARO : colori::GIALLO, 1);
}

int disegna_tasti_squadre(Tela& t, const Font& f, int destra, int y, const DatiPaginaSquadre& d, bool pagina_aperta, std::vector<AreaPulsante>* aree) {
    // il primo dice che cosa fa adesso sul compartimento scelto
    const PortaOtsek::Stato st = d.scelto >= 0 && d.scelto < static_cast<int>(d.porte.size()) ? d.porte[static_cast<std::size_t>(d.scelto)].stato
                                                                                             : PortaOtsek::Stato::comando;
    const char* isola = st == PortaOtsek::Stato::chiuso         ? (d.alfa ? "OTKRYT" : "RIAPRI")
                        : st == PortaOtsek::Stato::in_chiusura  ? (d.alfa ? "ZAKRYT NEMEDLENNO" : "CHIUDI SUBITO")
                        : st == PortaOtsek::Stato::comando      ? (d.alfa ? "POST" : "NON SI ISOLA")
                                                                : (d.alfa ? "GERMETIZ." : "ISOLA");
    std::vector<std::pair<int, std::string>> tasti = {{1, isola}};
    if (pagina_aperta) {
        tasti.push_back({2, d.alfa ? "VVERKH" : "SU"});
        tasti.push_back({3, d.alfa ? "VNIZ" : "GIU'"});
        tasti.push_back({4, d.alfa ? "V NACHALO" : "IN CIMA"});
    }
    tasti.push_back({5, pagina_aperta ? (d.alfa ? "RAZREZ" : "SPACCATO") : (d.alfa ? "PARTII" : "SQUADRE")});
    int x = destra;
    for (auto q = tasti.rbegin(); q != tasti.rend(); ++q) {
        const int w = f.larghezza(q->second, 1) + 18;
        x -= w;
        const Rettangolo b{x, y, w, 12};
        const bool spento = q->first == 1 && st == PortaOtsek::Stato::comando;
        const bool urgente = q->first == 1 && st == PortaOtsek::Stato::in_chiusura;
        t.rettangolo_pieno(b.x, b.y, b.w, b.h, urgente ? colori::ROSSO : colori::GRIGIO_SCURO);
        cornice(t, b, spento ? colori::GRIGIO_SCURO : colori::GRIGIO);
        f.scrivi(t, b.x + 4, b.y + 2, std::to_string(q->first), spento ? colori::GRIGIO : colori::GIALLO, 1);
        f.scrivi(t, b.x + 13, b.y + 2, q->second, spento ? colori::GRIGIO : colori::BIANCO, 1);
        if (aree) aree->push_back({b, q->first});
        x -= 2;
    }
    return x + 2;
}

void disegna_porte_spaccato(Tela& t, const Font& f, const Spaccato& s, Rettangolo r, const DatiPaginaSquadre& d) {
    for (int i = 0; i < static_cast<int>(d.porte.size()); ++i) {
        const PortaOtsek& p = d.porte[static_cast<std::size_t>(i)];
        if (p.stato != PortaOtsek::Stato::chiuso && p.stato != PortaOtsek::Stato::in_chiusura) continue;
        double x0 = 0.0, x1 = 0.0;
        if (!s.estensione(i, x0, x1)) continue;
        const int a = s.x_tela(r, x0), b = s.x_tela(r, x1);
        const std::uint8_t c = colore_porta(p.stato);
        for (int k = 0; k < 2; ++k) {   // le paratie dei due lati
            t.linea_verticale(a + k, r.y + 18, r.y + r.h - 12, c);
            t.linea_verticale(b - k, r.y + 18, r.y + r.h - 12, c);
        }
        // la targa in alto al compartimento
        std::string uno = p.stato == PortaOtsek::Stato::chiuso ? (d.alfa ? "ZAKRYT" : "CHIUSO") : "FUORI " + std::to_string(p.conto_s) + " S";
        std::string due = p.uomini_dentro > 0 ? std::to_string(p.uomini_dentro) + (d.alfa ? " V OTSEKE" : " DENTRO") : "";
        const int w = std::max(f.larghezza(uno, 1), f.larghezza(due, 1)) + 6, h = due.empty() ? 11 : 20;
        const int x = std::clamp((a + b) / 2 - w / 2, r.x, r.x + r.w - w);
        const int y = r.y + 20;
        t.rettangolo_pieno(x, y, w, h, colori::NERO);
        cornice(t, {x, y, w, h}, c);
        f.scrivi(t, x + 3, y + 2, uno, c, 1);
        if (!due.empty()) f.scrivi(t, x + 3, y + 11, due, colori::BIANCO, 1);
    }
}

}  // namespace m688
