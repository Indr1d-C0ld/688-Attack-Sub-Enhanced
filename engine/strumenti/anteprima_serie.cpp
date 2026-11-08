// Tavola dei grafici della schermata delle serie (grafica/grafici_serie.hpp) con dati finti: gli esiti, due righe di
// dispersione per parte, due istogrammi affiancati (uno a valori interi, uno continuo) e le barre per partita con la
// scelta. Colori delle parti dal modulo dei simboli, con la palette della carta.
// Uso: anteprima_serie <dati> <uscita.bmp> [esiti|istogrammi]
#include <cmath>
#include <cstdio>
#include <string>
#include <vector>

#include "carta.hpp"
#include "grafica/cornici.hpp"
#include "grafica/font.hpp"
#include "grafica/grafici_serie.hpp"
#include "grafica/palette.hpp"
#include "grafica/simboli_carta.hpp"
#include "grafica/tela.hpp"

using namespace m688;
namespace g = m688::grafici;
namespace s = m688::simboli;

int main(int argc, char** argv) {
    if (argc < 3) {
        std::fprintf(stderr, "uso: %s <dati> <uscita.bmp> [esiti|istogrammi]\n", argv[0]);
        return 2;
    }
    const std::string dati = argv[1], uscita = argv[2], pagina = argc > 3 ? argv[3] : "istogrammi";
    Palette p = Palette::carica(dati + "/palette/giorno.pal");
    applica_palette_carta(p, StileCarta::vga);
    const Font f = Font::carica(dati + "/font/originale.fnt");
    Tela t(640, 480);
    t.riempi(0);
    pannello(t, {0, 0, 640, 14});
    f.scrivi(t, 6, 3, "SERIE: PROVA_S5_CONVOGLIO   20 SEMI DA 1   60 MIN", colori::BIANCO, 1);
    const char* linguette[] = {"1 ESITI", "2 PER PARTE", "3 ISTOGRAMMI"};
    for (int i = 0; i < 3; ++i) pulsante(t, f, {6 + i * 110, 17, 104, 13}, linguette[i], (pagina == "esiti") == (i == 0) && i != 1);

    // dati finti, deterministici: venti partite
    std::vector<double> perse_nato, perse_patto, ritardo_nato, ritardo_patto;
    for (int k = 0; k < 20; ++k) {
        perse_nato.push_back(static_cast<double>((k * 7) % 3 == 0 ? 1 : 0));
        perse_patto.push_back(static_cast<double>((k * 5) % 4));
        ritardo_nato.push_back(120.0 + 90.0 * std::sin(k * 1.7) + 4.0 * k);
        ritardo_patto.push_back(k == 7 ? std::nan("") : 300.0 + 140.0 * std::cos(k * 0.9));
    }
    const auto nato = s::indice_parte(s::Parte::amico), patto = s::indice_parte(s::Parte::ostile);

    if (pagina == "esiti") {
        const std::vector<g::VoceEsito> esiti = {{21, "Tempo scaduto", 14}, {2, "Battello affondato", 4}, {5, "Obiettivo raggiunto", 2}};
        cornice(t, {6, 36, 628, 300}, StileCornice::console, 4);
        g::barre_esiti(t, f, {16, 48, 608, 280}, esiti, colori::CIANO_CHIARO, colori::GRIGIO, colori::CIANO);
        // per parte: due righe di dispersione con la stessa scala
        const g::Riassunto a = g::riassumi(ritardo_nato), b = g::riassumi(ritardo_patto);
        const double lo = std::min(a.min, b.min), hi = std::max(a.max, b.max);
        f.scrivi(t, 16, 350, "RITARDO MEDIO DI SCOPERTA", colori::GRIGIO, 1);
        f.scrivi(t, 16, 364, "NATO", nato, 1);
        g::riga_dispersione(t, f, {60, 362, 250, 10}, a, "s", nato, colori::BIANCO, lo, hi);
        f.scrivi(t, 330, 364, "PATTO", patto, 1);
        g::riga_dispersione(t, f, {374, 362, 250, 10}, b, "s", patto, colori::BIANCO, lo, hi);
    } else {
        g::OpzioniIstogramma o;
        o.titolo = "NATO: PERSE";
        o.colore = nato;
        g::istogramma(t, f, {12, 40, 300, 200}, perse_nato, o);
        o.titolo = "PATTO: PERSE";
        o.colore = patto;
        g::istogramma(t, f, {328, 40, 300, 200}, perse_patto, o);
        o.titolo = "NATO: RITARDO MEDIO";
        o.unita = "s";
        o.colore = nato;
        g::istogramma(t, f, {12, 250, 300, 100}, ritardo_nato, o);
        o.titolo = "PATTO: RITARDO MEDIO";
        o.colore = patto;
        g::istogramma(t, f, {328, 250, 300, 100}, ritardo_patto, o);
        f.scrivi(t, 12, 356, "UNA BARRA PER PARTITA (CLIC: IL SUO REPLAY)", colori::GRIGIO, 1);
        g::barre_partite(t, {12, 366, 616, 100}, ritardo_patto, patto, 4);
    }
    f.scrivi(t, 6, 470, "TAB: PAGINA   SU/GIU: NUMERO   ESC: INDIETRO", colori::GRIGIO, 1);
    t.salva_bmp(uscita, p);
    return 0;
}
