// Tavole di controllo della carta di navigazione VGA/SVGA (grafica/carta_vga): a sinistra la carta di oggi (il disegno
// VGA di app/carta, disegna_carta_museo), a destra la nuova, sullo stesso tratto di un teatro vero.
// Uso: anteprima_carta <dati> <uscita> <teatro> <vera|nato|urss> [vga|nautica] [larghezza_km=120] [x_km=0] [y_km=0]
//   scrive <uscita>.bmp (1280x360, a destra con gli scandagli, la mobilia e i toponimi) e stampa il tempo di una ricomposizione e
//   di una traslazione
// anteprima_carta <dati> <uscita> stazione <MISSIONE> [vga|nautica]
//   la carta della stazione Navigazione vera (disegna_carta di app/carta, con la carta d'epoca della parte del
//   giocatore) e sopra la mobilia e i toponimi, con le righe 0-30 e l'ultima libere: <uscita>.bmp (640x360)

#include <cctype>
#include <chrono>
#include <cstdio>
#include <memory>
#include <string>

#include "carta.hpp"
#include "missioni/missione.hpp"
#include "grafica/carta_vga.hpp"
#include "grafica/font.hpp"

using namespace m688;

int main(int argc, char** argv) {
    if (argc < 5) {
        std::fprintf(stderr, "uso: anteprima_carta <dati> <uscita> <teatro> <vera|nato|urss> [vga|nautica] [larghezza_km] [x_km] [y_km]\n");
        return 2;
    }
    const std::string dati = argv[1], uscita = argv[2], teatro = argv[3], carta = argv[4];
    if (teatro == "stazione") {
        const StileCarta st = argc > 5 && std::string(argv[5]) == "nautica" ? StileCarta::nautica : StileCarta::vga;
        Missione m(dati + "/../missioni/" + carta + ".lua", 1, false);
        for (const Parte parte : {Parte::nato, Parte::urss})
            m.imposta_fondale_carta(parte, std::make_shared<const Fondale>(Fondale::carica(percorso_carta(dati, m.teatro(), parte))));
        m.avanza(1.0);
        const Font originale = Font::carica(dati + "/font/originale.fnt"), nato = Font::carica(dati + "/font/incisione688_5.fnt"),
                   urss = Font::carica(dati + "/font/stampino_urss_7.fnt");
        imposta_font_isobate(&nato, &urss);
        CartaGiocatore c;
        c.restyle = true;
        c.stile = st;
        c.urss = m.giocatore() && m.giocatore()->classe == "alfa";
        Palette p = Palette::carica(dati + "/palette/giorno.pal");
        applica_palette_carta(p, st);
        applica_palette_rilievo(p, st);
        Tela t(640, 360);
        t.riempi(0);
        disegna_carta(t, originale, m, c, {}, -1, std::nullopt);
        const VistaCarta v = vista_carta(m, c);
        const GeometriaCarta g{v.x, v.y, v.w, v.h, v.centro, v.metri_per_pixel};
        const Fondale* fc = m.fondale_carta(c.urss ? Parte::urss : Parte::nato);
        MobiliaCarta mob;
        mob.cornice = mob.rosa = mob.scala = mob.cartiglio = true;
        mob.riserva_alto = 30;
        mob.riserva_basso = 12;
        mob.declinazione_gradi = declinazione_stima(m.teatro());
        mob.sovietica = c.urss;
        mob.titolo = m.teatro();
        for (auto& ch : mob.titolo) ch = static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
        mob.edizione = fc ? fc->edizione() : 0;
        t.imposta_ritaglio({0, 0, 640, 360});
        if (fc) {
            const SpazioMobilia sm = disegna_mobilia(t, g, *fc, c.urss ? urss : nato, mob);
            const int n = disegna_toponimi(t, g, carica_toponimi(file_toponimi(dati, m.teatro())), c.urss ? urss : nato,
                                           {sm.interno, sm.occupati});
            std::printf("toponimi scritti: %d\n", n);
        }
        t.salva_bmp(uscita + ".bmp", p);
        return 0;
    }
    const StileCarta stile = argc > 5 && std::string(argv[5]) == "nautica" ? StileCarta::nautica : StileCarta::vga;
    const double km = argc > 6 ? std::stod(argv[6]) : 120.0;
    const Vec2 centro{argc > 7 ? std::stod(argv[7]) * 1000.0 : 0.0, argc > 8 ? std::stod(argv[8]) * 1000.0 : 0.0};
    const std::string file = carta == "vera" ? dati + "/teatri/" + teatro + ".fdl" : percorso_carta(dati, teatro, carta == "urss" ? Parte::urss : Parte::nato);
    Fondale f = Fondale::carica(file);
    const Font font = Font::carica(dati + "/font/" + (carta == "urss" ? "stampino_urss_7" : "incisione688_5") + ".fnt");
    const Font originale = Font::carica(dati + "/font/originale.fnt");

    Palette p = Palette::carica(dati + "/palette/giorno.pal");
    applica_palette_carta(p, stile);
    applica_palette_rilievo(p, stile);
    Tela t(1280, 360);
    t.riempi(0);
    // a sinistra la carta di oggi, sullo stesso tratto (il centro e' un punto senza segno: fuori dalla vista)
    t.imposta_ritaglio({0, 0, 640, 360});
    {
        const double mpp = km * 1000.0 / 640.0;
        VistaCarta v;
        v.x = 0; v.y = 0; v.w = 640; v.h = 360; v.centro = centro; v.metri_per_pixel = mpp;
        // disegna_carta_museo centra su lat/lon: ricavo il punto dal centro se la carta e' georeferenziata
        if (f.georeferenziato()) {
            const Geografiche gc = f.geografiche(centro);
            disegna_carta_museo(t, originale, {0, 0, 640, 360}, f, &gc, km);
        }
    }
    // a destra la nuova
    GeometriaCarta g;
    g.x = 640; g.y = 0; g.w = 640; g.h = 360; g.centro = centro; g.metri_per_pixel = km * 1000.0 / 640.0;
    OpzioniCarta o;
    o.stile = stile;
    o.sicurezza_m = 150.0;
    o.font_isobate = &font;
    o.scandagli = true;
    FondaleVga fv;
    t.imposta_ritaglio({640, 0, 640, 360});
    fv.disegna(t, g, f, o);
    const double prima = fv.tempo_ricomposizione_ms();
    // una traslazione di 10 pixel: niente ricomposizione
    GeometriaCarta g2 = g;
    g2.centro.x += 10 * g.metri_per_pixel;
    const auto a = std::chrono::steady_clock::now();
    Tela prova(1280, 360);
    prova.imposta_ritaglio({640, 0, 640, 360});
    fv.disegna(prova, g2, f, o);
    const double trasla = std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - a).count();
    std::printf("ricomposizione %.1f ms, traslazione %.2f ms (ricomposto: %s)\n", prima, trasla, fv.ricomposto() ? "si'" : "no");
    // la mobilia sulla carta nuova
    MobiliaCarta mob;
    mob.cornice = mob.rosa = mob.scala = mob.cartiglio = true;
    mob.declinazione_gradi = declinazione_stima(teatro);
    mob.sovietica = carta == "urss";
    mob.titolo = teatro;
    for (auto& ch : mob.titolo) ch = static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
    mob.edizione = f.edizione();
    t.imposta_ritaglio({640, 0, 640, 360});
    const SpazioMobilia sm = disegna_mobilia(t, g, f, font, mob);
    const int n = disegna_toponimi(t, g, carica_toponimi(file_toponimi(dati, teatro)), font, {sm.interno, sm.occupati});
    std::printf("toponimi scritti: %d\n", n);
    t.togli_ritaglio();
    t.salva_bmp(uscita + ".bmp", p);
    return 0;
}
