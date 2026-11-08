// Tavola dei simboli delle viste dall'alto (grafica/simboli_carta.hpp) sul mare della carta, con la palette della
// carta (applica_palette_carta di app/carta): le quattro parti per dominio, il battello del giocatore, le cose in acqua,
// la distrutta, le emissioni, la croce TMA, l'ellisse al 95% e la legenda.
// Uso: anteprima_simboli <dati> <uscita.bmp> [vga|nautica] [confronto|vetrino]
//   confronto: prima e dopo l'alone nero, sul mare della fascia piu' bassa e della piu' profonda e sulla terra
//   vetrino: i segni degli interventi del vetrino di Dio (docs/28 §6) e il pannello, anche ingranditi
#include <cstdio>
#include <string>
#include <vector>

#include "carta.hpp"
#include "grafica/cornici.hpp"
#include "grafica/font.hpp"
#include "grafica/palette.hpp"
#include "grafica/simboli_carta.hpp"
#include "grafica/tela.hpp"
#include "grafica/tema_stazioni.hpp"

using namespace m688;
namespace s = m688::simboli;

int main(int argc, char** argv) {
    if (argc < 3) {
        std::fprintf(stderr, "uso: %s <dati> <uscita.bmp> [vga|nautica]\n", argv[0]);
        return 2;
    }
    const std::string dati = argv[1], uscita = argv[2];
    const bool vga = !(argc > 3 && std::string(argv[3]) == "nautica");
    Palette p = Palette::carica(dati + "/palette/giorno.pal");
    applica_palette_carta(p, vga ? StileCarta::vga : StileCarta::nautica);
    const Font f = Font::carica(dati + "/font/originale.fnt");
    Tela t(640, 400);
    t.riempi(static_cast<std::uint8_t>(colori_carta::ACQUA + 3 * 8 + 1));   // il mare a mezza profondita'
    const std::uint8_t testo = static_cast<std::uint8_t>(vga ? colori::BIANCO : colori::NERO);
    const s::Parte parti[] = {s::Parte::amico, s::Parte::ostile, s::Parte::ignoto, s::Parte::neutrale};

    if (argc > 4 && std::string(argv[4]) == "confronto") {
        // tre fasce (mare basso, mare profondo, terra) x senza e con l'alone
        const int fondi[] = {colori_carta::ACQUA + 1, colori_carta::ACQUA + 15 * 3 + 1, colori_carta::TERRA + 3};
        const char* nomi_fondi[] = {"MARE, FASCIA 0", "MARE, FASCIA 15", "TERRA"};
        t.riempi(0);
        for (int b = 0; b < 3; ++b)
            for (int lato = 0; lato < 2; ++lato) {
                const int x0 = lato * 320, y0 = b * 133;
                t.rettangolo_pieno(x0, y0, 319, 132, static_cast<std::uint8_t>(fondi[b]));
                s::imposta_alone(lato == 1);
                s::scrivi(t, f, x0 + 4, y0 + 3, std::string(nomi_fondi[b]) + (lato ? ": CON L'ALONE" : ": SENZA"), colori::BIANCO);
                for (int i = 0; i < 4; ++i) {
                    s::disegna_simbolo_unita(t, {x0 + 20.0 + i * 26, y0 + 30.0}, parti[i], s::Dominio::superficie);
                    s::disegna_simbolo_unita(t, {x0 + 20.0 + i * 26, y0 + 52.0}, parti[i], s::Dominio::subacqueo);
                    s::disegna_oggetto(t, {x0 + 20.0 + i * 26, y0 + 72.0}, s::Oggetto::siluro, s::indice_parte(parti[i]), 60.0);
                }
                s::disegna_oggetto(t, {x0 + 124.0, y0 + 72.0}, s::Oggetto::mina, 12);
                s::disegna_oggetto(t, {x0 + 144.0, y0 + 72.0}, s::Oggetto::boa, s::indice_parte(s::Parte::amico));
                s::disegna_oggetto(t, {x0 + 162.0, y0 + 72.0}, s::Oggetto::biologico, 0);
                s::disegna_proprio(t, {x0 + 140.0, y0 + 40.0});
                s::disegna_ellisse(t, {x0 + 140.0, y0 + 40.0}, 22.0, 9.0, 70.0, s::VERDE_PROPRIO);
                s::disegna_croce_tma(t, {x0 + 200.0, y0 + 30.0}, s::indice_croce_tma(vga));
                s::disegna_simbolo_unita(t, {x0 + 230.0, y0 + 30.0}, s::Parte::ostile, s::Dominio::superficie);
                s::disegna_distrutta(t, {x0 + 230.0, y0 + 30.0});
                {   // il ping nello stesso pennello del simbolo: un alone solo
                    s::Pennello pn;
                    s::disegna_simbolo_unita(pn, {x0 + 270.0, y0 + 40.0}, s::Parte::ostile, s::Dominio::superficie, s::indice_parte(s::Parte::ostile));
                    s::disegna_emissione(pn, {x0 + 270.0, y0 + 40.0}, s::Emissione::ping, 0.3);
                    pn.stendi(t);
                }
                // le scritte e i segni della stazione, col pennello: lettere dei contatti, X, punti di rotta, etichette
                s::scrivi(t, f, x0 + 190, y0 + 52, "B", 9);
                s::scrivi(t, f, x0 + 204, y0 + 52, "C", 10);
                s::Pennello x;
                x.linea({x0 + 222.0, y0 + 52.0}, {x0 + 228.0, y0 + 58.0}, colori::GIALLO);
                x.linea({x0 + 222.0, y0 + 58.0}, {x0 + 228.0, y0 + 52.0}, colori::GIALLO);
                x.stendi(t);
                s::scrivi(t, f, x0 + 240, y0 + 52, "3", 13);
                s::scrivi(t, f, x0 + 190, y0 + 92, "VICTOR III A2", s::indice_parte(s::Parte::ostile));
                s::scrivi(t, f, x0 + 20, y0 + 92, "INT. 14:20 KIROV", 12);
                s::scrivi(t, f, x0 + 20, y0 + 108, "ROSSO 12, BIANCO, CIANO", colori::BIANCO);
            }
        s::imposta_alone(true);
        t.salva_bmp(uscita, p);
        return 0;
    }

    if (argc > 4 && std::string(argv[4]) == "vetrino") {
        applica_tema(p, TemaStazione{});
        const Font fo = Font::carica(dati + "/font/fosforo688_5x7.fnt");
        const s::FamigliaIntervento fam[] = {s::FamigliaIntervento::forze, s::FamigliaIntervento::guasti, s::FamigliaIntervento::ambiente,
                                            s::FamigliaIntervento::comunicazioni, s::FamigliaIntervento::inganno};
        const char* nomi[] = {"FORZE", "GUASTI", "AMBIENTE", "COMUN.", "INGANNO"};
        Tela v(320, 200);
        v.riempi(static_cast<std::uint8_t>(colori_carta::ACQUA + 3 * 8 + 1));
        for (int i = 0; i < 5; ++i) {
            const double x = 24.0 + i * 44;
            s::scrivi(v, f, static_cast<int>(x) - 14, 6, nomi[i], colori::BIANCO);
            s::disegna_intervento(v, {x, 30.0}, fam[i], s::indice_famiglia(fam[i]));
            s::disegna_intervento(v, {x, 52.0}, fam[i], s::indice_famiglia(fam[i]), true);
            // accanto a un'unita', come sulla carta del replay
            s::disegna_simbolo_unita(v, {x + 2.0, 82.0}, s::Parte::ostile, s::Dominio::superficie);
            s::disegna_intervento(v, {x - 10.0, 72.0}, fam[i], s::indice_famiglia(fam[i]));
        }
        s::disegna_simbolo_unita(v, {30.0, 150.0}, s::Parte::ostile, s::Dominio::subacqueo);
        s::disegna_doppia_freccia(v, {30.0, 150.0}, {90.0, 120.0}, 5);
        s::scrivi(v, f, 60, 160, "FALSO CONTATTO", colori::BIANCO);
        s::disegna_simbolo_unita(v, {190.0, 150.0}, s::Parte::amico, s::Dominio::superficie);
        s::disegna_portata_ridotta(v, {190.0, 150.0}, 40.0, 18.0, 6);
        t.riempi(0);
        for (int y = 0; y < 200; ++y)   // il quadro a sinistra raddoppiato, per guardare i pixel
            for (int x = 0; x < 320; ++x) t.rettangolo_pieno(2 * x, 2 * y, 2, 2, v.at(x, y));
        const Rettangolo r{400, 205, 230, 180};
        const Rettangolo a = pannello_interventi(t, fo, r, "INTERVENTI - GUASTI", 2);
        const char* voci[] = {"SENSORE GUASTO", "MOTORE IN AVARIA", "SILURO DIFETTOSO", "FALLA", "INCENDIO", "BLACKOUT"};
        const int h = fo.righe() + 3;
        for (int i = 0; i < 6; ++i) {
            const int y = a.y + 2 + i * h + 1;
            if (i == 2) s::disegna_intervento(t, {a.x + 10.0, y + 3.0}, s::FamigliaIntervento::guasti, colori::BIANCO);
            fo.scrivi(t, a.x + 20, y, voci[i], i == 2 ? colori::BIANCO : fosforo(6), 1);
        }
        fo.scrivi(t, r.x + 6, r.y + r.h - 12, "TAB VOCE  INVIO SCEGLI  ESC INDIETRO", fosforo(4), 1);
        t.salva_bmp(uscita, p);
        return 0;
    }

    // le parti per dominio
    const char* nomi_parti[] = {"AMICO", "OSTILE", "IGNOTO", "NEUTRALE"};
    const s::Dominio domini[] = {s::Dominio::superficie, s::Dominio::subacqueo, s::Dominio::aereo, s::Dominio::elicottero, s::Dominio::terra,
                                 s::Dominio::ignoto};
    const char* nomi_domini[] = {"SUP.", "SUB.", "AEREO", "ELIC.", "TERRA", "IGN."};
    for (int d = 0; d < 6; ++d) f.scrivi(t, 82 + d * 46, 8, nomi_domini[d], testo, 1);
    for (int i = 0; i < 4; ++i) {
        const int y = 34 + i * 26;
        f.scrivi(t, 8, y - 3, nomi_parti[i], testo, 1);
        for (int d = 0; d < 6; ++d) s::disegna_simbolo_unita(t, {94.0 + d * 46, static_cast<double>(y)}, parti[i], domini[d]);
        // a matita nello stile 1989
        s::disegna_simbolo_unita(t, {390.0, static_cast<double>(y)}, parti[i], s::Dominio::superficie, s::indice_matita(parti[i], vga));
        s::disegna_simbolo_unita(t, {420.0, static_cast<double>(y)}, parti[i], s::Dominio::subacqueo, s::indice_parte(parti[i], true));
    }
    f.scrivi(t, 372, 8, "MATITA  ATTEN.", testo, 1);

    // le cose in acqua, per parte, con le rotte
    const s::Oggetto oggetti[] = {s::Oggetto::siluro, s::Oggetto::missile, s::Oggetto::esca, s::Oggetto::mina, s::Oggetto::boa, s::Oggetto::biologico};
    const char* nomi_oggetti[] = {"SILURO", "MISSILE", "ESCA", "MINA", "BOA", "BIOLOGICO"};
    for (int o = 0; o < 6; ++o) {
        const int x = 90 + o * 76;
        f.scrivi(t, x - 10, 150, nomi_oggetti[o], testo, 1);
        for (int i = 0; i < 4; ++i) s::disegna_oggetto(t, {x + 6.0, 172.0 + i * 18}, oggetti[o], s::indice_parte(parti[i]), 45.0 * i);
    }
    for (int i = 0; i < 4; ++i) f.scrivi(t, 8, 169 + i * 18, nomi_parti[i], testo, 1);
    // il filo da un siluro amico al battello
    s::disegna_proprio(t, {540.0, 120.0}, true);
    s::disegna_oggetto(t, {610.0, 50.0}, s::Oggetto::siluro, s::indice_parte(s::Parte::amico), 40.0);
    s::disegna_filo(t, {610.0, 50.0}, {540.0, 120.0}, s::indice_parte(s::Parte::amico));

    // il giocatore con la scia e l'ellisse al 95%, una nave distrutta, le emissioni, la croce TMA
    const int y2 = 270;
    for (int k = 0; k < 60; k += 3) t.pixel(40 + k, y2 + 30 - k / 3, s::VERDE_PROPRIO);
    s::disegna_proprio(t, {100.0, static_cast<double>(y2 + 10)});
    s::disegna_ellisse(t, {100.0, y2 + 10.0}, s::K_ELLISSE_95 * 9.0, s::K_ELLISSE_95 * 4.0, 60.0, s::VERDE_PROPRIO);
    s::disegna_simbolo_unita(t, {180.0, y2 + 10.0}, s::Parte::ostile, s::Dominio::superficie);
    s::disegna_distrutta(t, {180.0, y2 + 10.0});
    s::disegna_simbolo_unita(t, {250.0, y2 + 10.0}, s::Parte::ostile, s::Dominio::superficie);
    s::disegna_emissione(t, {250.0, y2 + 10.0}, s::Emissione::ping, 0.4);
    s::disegna_simbolo_unita(t, {320.0, y2 + 10.0}, s::Parte::ostile, s::Dominio::superficie);
    s::disegna_emissione(t, {320.0, y2 + 10.0}, s::Emissione::radar, 0.7);
    s::disegna_simbolo_unita(t, {380.0, y2 + 10.0}, s::Parte::amico, s::Dominio::superficie);
    s::disegna_emissione(t, {380.0, y2 + 10.0}, s::Emissione::radio, 0.0);
    s::disegna_croce_tma(t, {440.0, y2 + 10.0}, s::indice_croce_tma(vga));
    // l'unita' scelta; un contatto vecchio (sbiadito) col suo rilevamento e lo specchio della schiera
    s::disegna_simbolo_unita(t, {250.0, y2 + 50.0}, s::Parte::ostile, s::Dominio::subacqueo);
    s::disegna_scelta(t, {250.0, y2 + 50.0});
    s::disegna_rilevamento(t, {40.0, y2 + 110.0}, {200.0, y2 + 60.0}, s::indice_parte_eta(s::Parte::ignoto, 120.0));
    s::disegna_rilevamento(t, {40.0, y2 + 110.0}, {200.0, y2 + 115.0}, s::indice_parte_eta(s::Parte::ignoto, 120.0), true);
    s::disegna_simbolo_unita(t, {320.0, y2 + 50.0}, s::Parte::ignoto, s::Dominio::ignoto, s::indice_parte_eta(s::Parte::ignoto, 900.0));

    // la legenda
    const std::vector<s::VoceLegenda> voci = {
        {s::VoceLegenda::Tipo::proprio, {}, {}, {}, {}, 0, "IL BATTELLO"},
        {s::VoceLegenda::Tipo::scia, {}, {}, {}, {}, s::VERDE_PROPRIO, "LA SUA SCIA"},
        {s::VoceLegenda::Tipo::unita, s::Parte::amico, s::Dominio::superficie, {}, {}, 0, "AMICI"},
        {s::VoceLegenda::Tipo::unita, s::Parte::ostile, s::Dominio::superficie, {}, {}, 0, "OSTILI"},
        {s::VoceLegenda::Tipo::unita, s::Parte::ignoto, s::Dominio::superficie, {}, {}, 0, "IGNOTI"},
        {s::VoceLegenda::Tipo::unita, s::Parte::neutrale, s::Dominio::superficie, {}, {}, 0, "NEUTRALI"},
        {s::VoceLegenda::Tipo::oggetto, s::Parte::ostile, {}, s::Oggetto::siluro, {}, 0, "SILURO"},
        {s::VoceLegenda::Tipo::distrutta, {}, {}, {}, {}, 0, "DISTRUTTA"},
        {s::VoceLegenda::Tipo::emissione, {}, {}, {}, s::Emissione::ping, 0, "PING"},
        {s::VoceLegenda::Tipo::emissione, {}, {}, {}, s::Emissione::radar, 0, "RADAR"},
        {s::VoceLegenda::Tipo::croce_tma, {}, {}, {}, {}, s::indice_croce_tma(vga), "SOLUZIONE TMA"},
    };
    s::disegna_legenda(t, f, 500, 240, voci, testo);
    t.salva_bmp(uscita, p);
    return 0;
}
