// Tavole di controllo delle stazioni della Fase 3 (dossier docs/ricerca/19_consolle_688_alfa.md): gli strumenti delle
// consolle (grafica/consolle) col tema del battello (grafica/tema_stazioni) e i font delle stazioni, 688 e Alfa.
// Uso: anteprima_stazioni <dati> <uscita> [688|alfa] [bianca|rossa|blu] [strumenti|pannelli|viste|cornici|sala|stazione]
//   scrive <uscita>.bmp (640x480, 8 bit con la palette del tema); con dati d'esempio:
//   pannelli: governo e armi (grafica/sale) alti 120 e 88 righe (quella delle stazioni); viste: la centrale e sotto Ritm (Alfa) o radio ed ESM;
//   cornici: la cornice del periscopio (Alfa: la TV-1 con Okean dentro) e sotto radio ed ESM; sala: la sala sonar del
//   688 o il plottatore Sozh dell'Alfa; stazione: lo schermo come in gioco, la sala sonar (688) o il Sozh (Alfa) nella
//   vista, sotto l'intestazione di 14 righe, radio ed ESM coi 9 pulsanti della sala radio nelle 88 righe delle stazioni
//   e la riga del messaggio

#include <cmath>
#include <cstdio>
#include <string>
#include <vector>

#include "grafica/consolle.hpp"
#include "grafica/font.hpp"
#include "grafica/palette.hpp"
#include "grafica/sale.hpp"
#include "grafica/tema_stazioni.hpp"
#include "nucleo/geometria.hpp"

using namespace m688;

int main(int argc, char** argv) {
    if (argc < 3) {
        std::fprintf(stderr, "uso: anteprima_stazioni <dati> <uscita> [688|alfa] [bianca|rossa|blu]\n");
        return 2;
    }
    const std::string dati = argv[1], uscita = argv[2];
    const bool alfa = argc > 3 && std::string(argv[3]) == "alfa";
    const std::string l = argc > 4 ? argv[4] : "bianca";
    TemaStazione tema{alfa ? BattelloTema::alfa : BattelloTema::usa688,
                      l == "rossa" ? LuceSala::rossa : l == "blu" ? LuceSala::blu : LuceSala::bianca, Fosforo::del_battello};
    Palette p = Palette::carica(dati + "/palette/giorno.pal");
    applica_tema(p, tema);
    const std::string modo = argc > 5 ? argv[5] : "strumenti";
    if (modo == "viste" || modo == "cornici" || modo == "sala" || modo == "stazione") {
        const FontConsolle fc = FontConsolle::carica(dati, tema.battello);
        Tela t;
        t.riempi(colori::NERO);
        const Rettangolo sotto{0, 364, 640, 112};
        DatiRadioEsm re;
        re.righe_telescrivente = alfa ? std::vector<std::string>{"OT KOMANDOVANIJA SF", "RAJON 4 ZANJAT", "SVJAZ V 0600"}
                                      : std::vector<std::string>{"FM COMSUBLANT", "TO USS DALLAS", "BT", "PROCEED STATION ALFA", "BT"};
        for (int i = 0; i < 120; ++i) re.panoramica.push_back(static_cast<float>(0.15 + 0.1 * std::sin(i * 0.4) + (i == 40 || i == 41 ? 0.7 : 0.0)));
        re.intercettazioni = alfa ? std::vector<std::string>{"045 X  RLS", "120 S  SAMOLJOT"} : std::vector<std::string>{"045 X SPS-49", "120 S APS-137"};
        re.minaccia = true;
        if (modo == "stazione")   // i comandi della stazione Radio (app/stazione_radio)
            re.pulsanti = alfa ? std::vector<PulsanteStazione>{{"ANTENN", "pulsante_1", true}, {"BUJ", "pulsante_2"}, {"PARAVAN", "pulsante_3"},
                                                               {"SDV", "pulsante_4"}, {"PEREDAT", "pulsante_5"}, {"ZADERZH", "pulsante_6"},
                                                               {"PRIKAZ", "pulsante_7"}, {"SOOBSHCH", "pulsante_8"}, {"ARKHIV", "pulsante_9"}}
                               : std::vector<PulsanteStazione>{{"MAST", "pulsante_1", true}, {"WIRE", "pulsante_2", true}, {"ELF", "pulsante_3"},
                                                               {"SLOT", "pulsante_4"}, {"DELAY", "pulsante_5"}, {"XMIT", "pulsante_6"},
                                                               {"ORDERS", "pulsante_7"}, {"TRAFFIC", "pulsante_8"}, {"FILE", "pulsante_9"}};
        else if (modo == "cornici")   // la variante coi pulsanti della sala radio
            re.pulsanti = {{"FILO", "pulsante_1", true}, {"SSIXS", "pulsante_2"}, {"SLOT", "pulsante_3"},
                           {"TRASM", "pulsante_4"}, {"BOA", "pulsante_5"}, {"ARCHIV", "pulsante_6"}};
        if (modo == "viste") {
            vista_centrale(t, tema, fc, 12.0);
            if (alfa) {
                DatiStato st;
                st.sinottici = {{"GAMMA", {ColoreLampada::verde, ColoreLampada::verde, ColoreLampada::ambra, ColoreLampada::rossa}, {true, true, true, false}},
                                {"TEMBR", {ColoreLampada::verde, ColoreLampada::verde, ColoreLampada::verde}, {true, false, true}},
                                {"TAKT-T", {ColoreLampada::verde, ColoreLampada::ambra, ColoreLampada::rossa, ColoreLampada::verde, ColoreLampada::verde}, {true, true, false, true, true}}};
                st.allarmi = {"T OTSEK > 60", "DAVL VOZDUH"};
                st.potenza_reattore = 0.62;
                st.temperatura = 290;
                pannello_stato_alfa(t, sotto, tema, fc, st);
            } else {
                pannello_radio_esm(t, sotto, tema, fc, re);
            }
        } else if (modo == "cornici") {
            for (int y = 30; y <= 330; ++y)   // un oculare finto: il disco del mare grigio
                for (int x = 170; x <= 470; ++x)
                    if ((x - 320) * (x - 320) + (y - 180) * (y - 180) <= 150 * 150) t.pixel(x, y, smalto(4));
            if (alfa) {
                cornice_periscopio(t, tema, fc, true, 1);
                std::vector<float> livelli(360, 0.15f);
                for (int g = 40; g < 50; ++g) livelli[static_cast<std::size_t>(g)] = 0.9f;
                schermo_okean(t, 320, 190, 110, livelli, 45.0, fc);
            } else {
                cornice_periscopio(t, tema, fc, false, 2);
            }
            pannello_radio_esm(t, sotto, tema, fc, re);
        } else {
            if (modo == "stazione") {   // come app/scena_prova consolle(): intestazione, 88 righe di pannello, messaggio
                const Font originale = Font::carica(dati + "/font/originale.fnt");
                t.rettangolo_pieno(0, 360, 640, 120, colori::NERO);
                originale.scrivi(t, 12, 362, "RADIO   12:04:30   ROTTA 285  QUOTA 46 M  12,5 NODI", colori::BIANCO, 1);
                std::vector<AreaPulto> aree;
                pannello_radio_esm(t, {0, 374, 640, 88}, tema, fc, re, &aree);
                originale.scrivi(t, 12, 466, "CLIC SUI PULSANTI   A ALBERO   F FILO   M MESSAGGI", colori::GRIGIO, 1);
                for (const auto& ar : aree)
                    if (!ar.comando.empty()) std::printf("%d %d %d %d %s\n", ar.r.x, ar.r.y, ar.r.w, ar.r.h, ar.comando.c_str());
            }
            if (alfa) {
                const Rettangolo carta{40, 40, 560, 290};
                t.rettangolo_pieno(carta.x, carta.y, carta.w, carta.h, smalto(11));
                for (int x = carta.x; x < carta.x + carta.w; x += 40) t.linea_verticale(x, carta.y, carta.y + carta.h - 1, smalto(8));
                cornice_sozh(t, carta, tema, fc, 300, 140);
            } else {
                const Rettangolo libero = cornice_sala_sonar(t, tema, fc);
                t.rettangolo_pieno(libero.x, libero.y, libero.w, libero.h, fosforo(0));
                testo_fosforo(t, libero.x + 10, libero.y + 10, "(CASCATE DELLA STAZIONE)", fc.schermo);
            }
        }
        t.salva_bmp(uscita + ".bmp", p);
        return 0;
    }
    if (modo == "pannelli") {
        const FontConsolle fc = FontConsolle::carica(dati, tema.battello);
        DatiGoverno g;
        g.rotta = 285; g.rotta_ordinata = 300; g.quota_m = 46; g.quota_ordinata_m = 120; g.cambia_quota = true;
        g.velocita_kn = 12.5; g.barra_gradi = -15; g.piani_gradi = 10; g.campana = 6; g.cavita = false; g.pilota = true;
        g.punti_rotta = 3; g.aperture = std::vector<bool>(16, false); g.aperture[5] = true; g.compenso_prua = 0.6;
        g.compenso_poppa = 0.4; g.modo_rotta = DatiGoverno::Modo::programma; g.modo_quota = DatiGoverno::Modo::automatico;
        g.tan = true; g.guasto_casse = true; g.tappa = 3; g.tappe = 7; g.compensa = true; g.compensa_verso = -1; g.sfoga = true;
        g.radar = true;
        DatiArmi a;
        const int nt = alfa ? 6 : 4;
        for (int i = 0; i < nt; ++i) {
            DatiArmi::Tubo tb;
            tb.arma = alfa ? (i < 4 ? "53-65K" : "TEST-71") : (i < 3 ? "MK48" : "ADCAP");
            tb.pronto = i < 2; tb.allagato = i < 3; tb.porta = i == 0; tb.filo = i == 0;
            tb.ricarica = i == nt - 1 ? 0.4 : -1.0;
            a.tubi.push_back(tb);
        }
        a.selezionato = 0;
        a.schermo = alfa ? std::vector<std::string>{"(DAL LANCIO) [ROTTA 000] PASSIVO VEL. ALTA, QUOTA 100 M, FILO", "CEL 1  PELENG 045",
                                                    "DIST 12 KM  KURS 270", "SKOR 8 UZ"}
                         : std::vector<std::string>{"TUBE 1  MK48  WIRE  (DAL LANCIO) [ROTTA 000] PASSIVO VEL. ALTA", "BRG 045  RNG 12.4 KYD",
                                                    "SPD 40  DEP 150 FT", "ENABLE RUN 3000 YD"};
        a.bersagli_akkord = 2; a.bersaglio_scelto = 0;
        a.slot_akkord = {"B 4,2 NM", "D 9,8 NM"};
        Tela t;
        t.riempi(colori::NERO);
        std::vector<AreaPulto> aree;
        pannello_governo(t, {0, 0, 640, 120}, tema, fc, g, &aree);
        pannello_armi(t, {0, 124, 640, 120}, tema, fc, a, &aree);
        pannello_governo(t, {0, 250, 640, 88}, tema, fc, g);   // l'altezza delle stazioni (sotto intestazione e messaggi)
        pannello_armi(t, {0, 342, 640, 88}, tema, fc, a);
        // le aree cliccabili del primo pannello di governo e del primo d'armi, come cornici sottili (controllo)
        for (const auto& ar : aree)
            if (!ar.comando.empty()) {
                t.linea_orizzontale(ar.r.x, ar.r.x + ar.r.w - 1, ar.r.y + ar.r.h - 1, colori::GIALLO);
            }
        t.salva_bmp(uscita + ".bmp", p);
        for (const auto& ar : aree) std::printf("%d %d %d %d %s\n", ar.r.x, ar.r.y, ar.r.w, ar.r.h, ar.comando.c_str());
        return 0;
    }
    const auto font = [&](const char* n) { return Font::carica(dati + "/font/" + n + ".fnt"); };
    const Font targa = font(alfa ? "stampino_urss_7" : "incisione688_7"), piccolo = font(alfa ? "stampino_urss_7" : "incisione688_5"),
               schermo = font(alfa ? "stampino_urss_10" : "fosforo688_5x7"), schermo2 = font(alfa ? "stampino_urss_7" : "fosforo688_7x9"),
               cifre = font("cifre_6x11"), tamb = font(alfa ? "stampino_urss_10" : "incisione688_7");

    Tela t;
    t.riempi(smalto(alfa ? 2 : 3));
    // pannello degli strumenti di governo
    pannello(t, {8, 8, 300, 220}, tema.battello);
    targhetta(t, {20, 14, 0, 0}, alfa ? "BOKSIT / UPRAVLENIE" : "SHIP CONTROL", targa);
    const StileQuadrante sq = alfa ? StileQuadrante::bianco_alfa : StileQuadrante::scuro_688;
    const double ordinata = 300.0, quota_ord = 0.6;
    quadrante(t, 70, 90, 42, 285.0, {0, 360, 0, 360, 36, 12, 0.1}, sq, &piccolo, &ordinata);
    quadrante(t, 170, 90, 30, 12.0, {-35, 35, -120, 120, 14, 2, 1.0}, sq, &piccolo);
    quadrante(t, 250, 90, 30, 18.0, {0, 30, -135, 135, 30, 6, 1.0}, sq, &piccolo);
    barra(t, {40, 145, 14, 70}, 0.45, sq, quota_ord);
    tamburo(t, 70, 150, "0150", 0.35, tamb);
    sette_segmenti(t, 70, 185, "12.5", ColoreLampada::rossa, cifre);
    if (!alfa) cloche(t, {200, 135, 90, 85}, -0.4, 0.3);
    else
        for (int i = 0; i < 4; ++i) pulsante(t, {190 + (i % 2) * 54, 140 + (i / 2) * 36, 50, 30}, i == 0 ? "PROGR" : i == 1 ? "AVTO" : i == 2 ? "RUCHN" : "TAN",
                                             i == 3 ? ColoreLampada::verde : ColoreLampada::ambra, i == 0 || i == 3, i == 0, piccolo);

    // pannello delle lampade, degli allarmi e dei comandi
    pannello(t, {316, 8, 316, 220}, tema.battello);
    targhetta(t, {328, 14, 0, 0}, alfa ? "POKS / AVARIJA" : "BALLAST CONTROL", targa);
    if (!alfa) {
        std::vector<bool> aperte(24, false);
        aperte[3] = aperte[17] = true;
        albero_di_natale(t, {330, 40, 140, 70}, aperte, 8);
        for (int i = 0; i < 6; ++i) lampada_rettangolare(t, {480 + (i % 3) * 46, 44 + (i / 3) * 30, 40, 22}, static_cast<ColoreLampada>(i % 3), i < 3);
        pulsante_coperto(t, {340, 125, 60, 60}, false, false);
        pulsante_coperto(t, {410, 125, 60, 60}, true, true);
        for (int i = 0; i < 4; ++i) manopola(t, 500 + i * 30, 150, 11, -60.0 + 40.0 * i);
    } else {
        for (int i = 0; i < 12; ++i) lampada_tonda(t, 340 + i * 22, 50, 6, static_cast<ColoreLampada>(i % 3), i % 4 != 0);
        finestrella(t, {336, 72, 136, 22}, "AKKORD AVARIJA", true, piccolo);
        finestrella(t, {480, 72, 136, 22}, "T OTSEK > 60", false, piccolo);
        for (int i = 0; i < 6; ++i) pulsante(t, {336 + i * 48, 104, 44, 40}, i % 2 ? "DUT" : "PRODUV", ColoreLampada::verde, i < 2, false, piccolo);
        for (int i = 0; i < 8; ++i) levetta(t, 350 + i * 30, 175, i % 3 == 0);
    }

    // schermi
    if (!alfa) {
        const Rettangolo v = crt_rettangolare(t, {8, 236, 330, 236});
        testo_fosforo(t, v.x + 6, v.y + 6, "BQQ-5  BRG 045 T  RNG 12.4 KYD", schermo);
        testo_fosforo(t, v.x + 6, v.y + 20, "S1 CERTSUB  VICTOR III  L 50 HZ", schermo);
        testo_fosforo(t, v.x + 6, v.y + 40, "MK 81  TUBE 1 READY", schermo2);
        for (int x = 0; x < v.w - 12; ++x) t.pixel(v.x + 6 + x, v.y + 120 + static_cast<int>(10 * std::sin(x * 0.11)), fosforo(5));
    } else {
        const Rettangolo v = crt_tondo(t, 120, 352, 92);
        testo_fosforo(t, v.x + 40, v.y + 40, "OKEAN", schermo);
        for (int k = -30; k <= 30; ++k)   // ventaglio dei rilevamenti
            for (int d = 10; d < 80; d += 2) t.pixel(v.x + 92 + static_cast<int>(d * std::sin(k * PI / 180 * 2)), v.y + 150 - d, fosforo(1 + (k == 8) * 5));
        const Rettangolo tv = crt_rettangolare(t, {240, 270, 98, 80});
        testo_fosforo(t, tv.x + 4, tv.y + 4, "TV-1", schermo2);
    }
    pannello(t, {346, 236, 286, 236}, tema.battello);
    targhetta(t, {358, 244, 0, 0}, alfa ? "SARGAN / ARMI" : "WEAPONS CONTROL", targa);
    for (int i = 0; i < (alfa ? 6 : 4); ++i) {
        const int y = 272 + i * 30;
        char n[8];
        std::snprintf(n, sizeof n, "%d", i + 1);
        targa.scrivi(t, 360, y + 6, n, alfa ? static_cast<std::uint8_t>(colori::NERO) : smalto(11), 1, 1, false);
        for (int k = 0; k < 4; ++k) {
            static const char* const S688[] = {"RDY", "FLD", "DOOR", "FIRE"};
            static const char* const SALFA[] = {"GOT", "ZAL", "KRYSH", "PUSK"};
            if (alfa) pulsante(t, {380 + k * 60, y, 54, 24}, SALFA[k], k == 3 ? ColoreLampada::rossa : ColoreLampada::verde, k <= i % 3, false, piccolo);
            else {
                if (i == 0) piccolo.scrivi(t, 384 + k * 56, y - 10, S688[k], smalto(11), 1, 1, false);
                lampada_rettangolare(t, {380 + k * 56, y + 2, 48, 20}, k == 3 ? ColoreLampada::rossa : ColoreLampada::verde, k <= i % 3);
            }
        }
    }
    t.salva_bmp(uscita + ".bmp", p);
    return 0;
}
