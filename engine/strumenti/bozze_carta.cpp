// Le bozze della carta di navigazione (F6; proposte A e B del kit, scelte dell'utente del 29/10/2026), disegnate dal
// codice vero sullo stesso tratto della stazione: il fondale VGA (grafica/carta_vga), lo strato meteo
// (grafica/strati_meteo), la mobilia e i toponimi. Tre tavole 640x360:
//   oggi:  la striscia in alto (riga dell'ingrandimento e della carta), il retino a 6 px, una barba col mare ogni 80 px;
//   A:     la carta pulita: il retino a 12 px, una barba ogni 192 px in grigio senza cifre, il vento e il mare una volta
//          sola nella legenda;
//   B:     A senza la striscia: la carta usa anche le 30 righe di sopra, il titolo resta nel cartiglio.
// Contatti, rotte, zone e simboli li disegna app/carta.cpp sopra il fondale: qui non ci sono, uguali nelle tre.
// Uso: bozze_carta <dati> <cartella d'uscita> [MISSIONE=SCENE1A]
#include <cctype>
#include <cmath>
#include <cstdio>
#include <memory>
#include <string>

#include "carta.hpp"
#include "grafica/carta_vga.hpp"
#include "grafica/font.hpp"
#include "grafica/strati_meteo.hpp"
#include "missioni/missione.hpp"
#include "nucleo/meteo.hpp"

using namespace m688;

namespace {

struct Variante {
    const char* nome;
    int alto;               // le righe della striscia in alto (0: niente striscia)
    int passo_tratteggio;   // il retino delle zone non rilevate
    bool meteo_rado;        // una barba ogni 192 px, grigia, e la legenda
};

}  // namespace

int main(int argc, char** argv) {
    if (argc < 3) {
        std::fprintf(stderr, "uso: %s <dati> <cartella d'uscita> [MISSIONE]\n", argv[0]);
        return 2;
    }
    const std::string dati = argv[1], fuori = argv[2], missione = argc > 3 ? argv[3] : "SCENE1A";
    Missione m(dati + "/../missioni/" + missione + ".lua", 1, false);
    for (const Parte parte : {Parte::nato, Parte::urss})
        m.imposta_fondale_carta(parte, std::make_shared<const Fondale>(Fondale::carica(percorso_carta(dati, m.teatro(), parte))));
    m.meteo_enhanced(dati);   // come la partita Enhanced: il meteo del teatro, il vento e il mare
    m.avanza(1.0);
    const Font originale = Font::carica(dati + "/font/originale.fnt"), nato = Font::carica(dati + "/font/incisione688_5.fnt");
    CartaGiocatore c;
    c.restyle = true;
    c.stile = StileCarta::vga;
    Palette p = Palette::carica(dati + "/palette/giorno.pal");
    applica_palette_carta(p, c.stile);
    applica_palette_rilievo(p, c.stile);
    const VistaCarta v = vista_carta(m, c);
    const Fondale* fc = m.fondale_carta(Parte::nato);
    const auto* g = m.giocatore();
    const Meteo* me = m.meteo();
    if (!fc || !g) {
        std::fprintf(stderr, "%s: senza carta d'epoca o senza battello\n", missione.c_str());
        return 1;
    }
    const Variante varianti[] = {{"carta_oggi", 30, 6, false}, {"carta_pulita", 30, 12, true}, {"carta_grande", 0, 12, true}};
    for (const auto& x : varianti) {
        Tela t(640, 360);
        t.riempi(0);
        const GeometriaCarta geo{v.x, v.y, v.w, v.h, v.centro, v.metri_per_pixel};
        OpzioniCarta o;
        o.stile = c.stile;
        o.sicurezza_m = 150.0;
        o.font_isobate = &nato;
        o.scandagli = true;
        o.passo_tratteggio = x.passo_tratteggio;
        FondaleVga fv;
        t.imposta_ritaglio({0, 0, 640, 360});
        fv.disegna(t, geo, *fc, o);
        // lo strato meteo: oggi una barba bianca col mare ogni 80 px; A e B una ogni 192, grigia, senza cifre
        if (me) {
            StratiMeteo s;
            s.a_tela = [&v](Vec2 q) { return v.a_tela(q); };
            s.a_mondo = [&v](Vec2 q) { return v.a_mondo(q); };
            s.area = {v.x, v.y + x.alto, v.w, v.h - x.alto};
            s.passo_px = x.meteo_rado ? 192 : 80;
            s.campioni_px = 8;
            s.vento = [me](Vec2 q, double& u, double& da) {
                const StatoMeteo st = me->in(q);
                u = st.vento_m_s;
                da = st.vento_da_gradi;
            };
            if (!x.meteo_rado) s.stato_mare = [me](Vec2 q) { return me->in(q).stato_mare(); };
            s.colori.vento = s.colori.mare = static_cast<std::uint8_t>(x.meteo_rado ? colori_carta::RETICOLO : colori_carta::MATITA + 5);
            disegna_strati_meteo(t, originale, s);
            t.imposta_ritaglio({0, 0, 640, 360});
        }
        // la mobilia e i toponimi, sotto la striscia se c'e'
        MobiliaCarta mob;
        mob.cornice = mob.rosa = mob.scala = mob.cartiglio = true;
        mob.riserva_alto = x.alto;
        mob.riserva_basso = 12;
        mob.declinazione_gradi = declinazione_stima(m.teatro());
        mob.titolo = m.teatro();
        for (auto& ch : mob.titolo) ch = static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
        mob.edizione = fc->edizione();
        SpazioMobilia sm = disegna_mobilia(t, geo, *fc, nato, mob);
        // la legenda del meteo (A e B): il vento e il mare del bollettino una volta sola, in alto a destra sull'acqua,
        // coi colori del cartiglio (matita nera su bianco); i toponimi la evitano
        if (me && x.meteo_rado) {
            const auto nero = static_cast<std::uint8_t>(colori_carta::MATITA), bianco = static_cast<std::uint8_t>(colori_carta::MATITA + 5);
            const StatoMeteo st = me->in(g->pos);
            char r1[48], r2[48];
            std::snprintf(r1, sizeof r1, "VENTO %03.0f / %.0f KN", st.vento_da_gradi, st.vento_m_s * 1.943844);
            std::snprintf(r2, sizeof r2, "MARE %d (DOUGLAS)", st.stato_mare());
            const int passo = nato.righe() + 3, lw = 104, lh = 3 * passo + 6;
            const int lx = sm.interno.x + sm.interno.w - lw - 6, ly = sm.interno.y + 6;
            t.rettangolo_pieno(lx, ly, lw, lh, bianco);
            t.linea_orizzontale(lx, lx + lw - 1, ly, nero), t.linea_orizzontale(lx, lx + lw - 1, ly + lh - 1, nero);
            t.linea_verticale(lx, ly, ly + lh - 1, nero), t.linea_verticale(lx + lw - 1, ly, ly + lh - 1, nero);
            nato.scrivi(t, lx + 4, ly + 4, "METEO, BOLLETTINO", nero, 1);
            nato.scrivi(t, lx + 4, ly + 4 + passo, r1, nero, 1);
            nato.scrivi(t, lx + 4, ly + 4 + 2 * passo, r2, nero, 1);
            sm.occupati.push_back({lx, ly, lw, lh});
        }
        disegna_toponimi(t, geo, carica_toponimi(file_toponimi(dati, m.teatro())), nato, {sm.interno, sm.occupati});
        // il battello, come il simbolo NTDS amico della stazione
        const Vec2 q = v.a_tela(g->pos);
        t.rettangolo_pieno(static_cast<int>(q.x) - 4, static_cast<int>(q.y) - 4, 9, 9, 0);
        t.rettangolo_pieno(static_cast<int>(q.x) - 3, static_cast<int>(q.y) - 3, 7, 7, static_cast<std::uint8_t>(colori_carta::NTDS + 2));
        // la striscia di oggi: la riga dell'ingrandimento e la riga della carta, sul chiaro
        if (x.alto > 0) {
            t.rettangolo_pieno(0, 0, 640, x.alto, 15);
            char riga[96];
            std::snprintf(riga, sizeof riga, "INGRANDIMENTO: %d   PROIEZIONE %02d MIN (SPENTA)", c.fattore(), c.minuti());
            originale.scrivi(t, 8, 4, riga, 0, 1);
            std::snprintf(riga, sizeof riga, "CARTA NATO ED. %d - ZONE TRATTEGGIATE NON RILEVATE", fc->edizione());
            originale.scrivi(t, 640 - 8 - originale.larghezza(riga, 1), 18, riga, static_cast<std::uint8_t>(colori_carta::MAGENTA), 1);
        }
        t.salva_bmp(fuori + "/" + x.nome + ".bmp", p);
        std::printf("%s\n", x.nome);
    }
    return 0;
}
