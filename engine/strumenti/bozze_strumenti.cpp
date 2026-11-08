// Bozze del gruppo 2 della rifinitura estetica (roadmap 33; dossier docs/ricerca/25_estetica_simulatori.md §8.5): gli
// strumenti come schermi di consolle (variante C, scelta dell'utente del 28/10/2026), coi pezzi di grafica/quadro e il
// piede comune di grafica/lamiera. I dati sono quelli delle istantanee di oggi (docs/ricerca/schermate_prima:
// sandbox_statistiche_1 e 2, replay_quadro), cosi' il confronto prima e dopo e' sugli stessi numeri.
// La carta del replay la incolla tools/estetica/bozze_strumenti.py dal ritaglio dell'istantanea, al rettangolo che
// questo programma stampa («CARTA nome x y w h»).
// Uso: bozze_strumenti <dati> <cartella d'uscita>
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <tuple>
#include <string>
#include <vector>

#include "grafica/cerimonia.hpp"
#include "grafica/consolle.hpp"
#include "grafica/cornici.hpp"
#include "grafica/font.hpp"
#include "grafica/lamiera.hpp"
#include "grafica/palette.hpp"
#include "grafica/quadro.hpp"
#include "grafica/tela.hpp"
#include "grafica/tema_stazioni.hpp"

using namespace m688;
namespace q = m688::quadro;

namespace {

struct Fonti {
    Font orig, inc7, fos5, fos7;
};
int largo(const Font& f, const std::string& s) { return f.larghezza(s, 1, f.formato() == 2 ? f.spazio_predefinito() : 1, f.formato() != 2); }
void scrivi(Tela& t, const Font& f, int x, int y, const std::string& s, std::uint8_t c) {
    f.scrivi(t, x, y, s, c, 1, f.formato() == 2 ? f.spazio_predefinito() : 1, f.formato() != 2);
}

const std::vector<std::string> PAGINE = {"1 RIEPILOGO", "2 SCOPERTE", "3 ARMI", "4 REAZIONI E TMA", "5 ESPOSIZIONE"};

void piede_statistiche(Tela& t, const Fonti& f) {
    lamiera::piede(t, f.orig, 428, "TAB PAGINA   FRECCE SCORRONO   CLIC SU UNA RIGA: IL REPLAY DA LI'",
                   {{"TAB  PAGINA SEGUENTE", ColoreLampada::ambra}, {"ESC  INDIETRO", ColoreLampada::verde}}, 1);
}

// La pagina 1: il riepilogo per parte a sinistra, e a destra il grafico che oggi manca (lo spazio nero)
void statistiche_riepilogo(Tela& t, const Fonti& f) {
    const Rettangolo u = q::consolle(t, f.inc7, "SALA ANALISI - STATISTICHE DELLA SANDBOX");
    const Font& g = f.fos5;
    const Rettangolo a = q::schermo(t, {u.x, u.y, 384, 424 - u.y}, g, "PROVA S5 CONVOGLIO", "0:09:58");
    int y = a.y + 2;
    y += q::linguette(t, a, y, g, {"1 RIEPILOGO", "2 SCOPERTE", "3 ARMI", "4 REAZIONI", "5 ESPOSIZ."}, 0) + 4;
    const std::vector<int> col = {232, 306};
    y += q::intestazione(t, a, y, g, "PER PARTE", col, {"NATO", "PATTO"});
    struct R {
        const char *n, *a, *b;
    };
    const R righe[] = {{"UNITA'", "11", "1"}, {"PERSE / INFLITTE", "0 / 0", "0 / 0"}, {"SCAMBIO", "0,0", "0,0"},
                       {"SCOPERTE FATTE / SUBITE", "3 / 10", "10 / 3"}, {"RITARDO MEDIO DI SCOPERTA", "0 S", "0 S"},
                       {"DISTANZA MEDIA DI SCOPERTA", "4,3 KM", "3,1 KM"}, {"LANCI / A SEGNO", "0 / 0", "2 / 1"},
                       {"ESCHE / EFFICACI", "2 / 0", "0 / 0"}, {"SCOPERTA - ALLERTA", "-", "274 S"},
                       {"SCOPERTA - INGAGGIO", "371 S", "32 S"}, {"TEMPO ALLA SOLUZIONE", "-", "273 S"},
                       {"ERRORE AL LANCIO", "-", "685 M"}};
    int k = 0;
    for (const auto& r : righe) {
        q::riga(t, a, y, g, r.n, col, {r.a, r.b}, k++ % 2 == 1);   // righe alterne velate: l'occhio segue la riga
        y += 11;
    }
    y += 6;
    y += q::intestazione(t, a, y, g, "PK PER ARMA (A SEGNO / LANCIATE)");
    q::riga(t, a, y, g, "TYPE 53", col, {"-", "1 / 2"});
    // il secondo schermo: i numeri della sinistra in figura
    const Rettangolo b = q::schermo(t, {u.x + 388, u.y, u.w - 388, 424 - u.y}, g, "IN FIGURA", "NATO  PATTO");
    int yb = b.y + 4;
    yb += q::intestazione(t, b, yb, g, "SCOPERTE FATTE E SUBITE");
    const auto barre = [&](int yy, const char* n, int fatte, int subite) {
        scrivi(t, g, b.x + 4, yy + 4, n, fosforo(5));
        for (int i = 0; i < fatte; ++i) t.rettangolo_pieno(b.x + 52 + i * 15, yy, 12, 7, fosforo(6));
        for (int i = 0; i < subite; ++i) t.rettangolo_pieno(b.x + 52 + i * 15, yy + 10, 12, 7, fosforo(3));
    };
    barre(yb, "NATO", 3, 10);
    barre(yb + 26, "PATTO", 10, 3);
    yb += 54;
    t.rettangolo_pieno(b.x + 4, yb, 10, 6, fosforo(6));
    scrivi(t, g, b.x + 18, yb, "FATTE", fosforo(4));
    t.rettangolo_pieno(b.x + 74, yb, 10, 6, fosforo(3));
    scrivi(t, g, b.x + 88, yb, "SUBITE", fosforo(4));
    yb += 20;
    yb += q::intestazione(t, b, yb, g, "LANCI E ESCHE");
    scrivi(t, g, b.x + 4, yb, "PATTO: 2 LANCI, 1 A SEGNO", fosforo(6));
    scrivi(t, g, b.x + 4, yb + 11, "NATO: 2 ESCHE, NESSUNA EFFICACE", fosforo(6));
    yb += 32;
    yb += q::intestazione(t, b, yb, g, "TEMPI DEL PATTO (S)");
    const struct { const char* n; int s; } tempi[] = {{"ALLERTA", 274}, {"SOLUZIONE", 273}, {"INGAGGIO", 32}};
    for (const auto& x : tempi) {   // barre orizzontali in scala comune: 300 s = tutta la riga
        scrivi(t, g, b.x + 4, yb, x.n, fosforo(4));
        const int w = (b.w - 96) * x.s / 300;
        t.rettangolo_pieno(b.x + 64, yb, w, 7, fosforo(5));
        scrivi(t, g, b.x + 68 + w, yb, std::to_string(x.s), fosforo(6));
        yb += 12;
    }
    scrivi(t, g, b.x + 4, b.y + b.h - 10, "CLIC SU UNA RIGA: IL REPLAY DA LI'", fosforo(3));
    piede_statistiche(t, f);
}

// La pagina 2: la tabella larga delle scoperte su tutto lo schermo
void statistiche_scoperte(Tela& t, const Fonti& f) {
    const Rettangolo u = q::consolle(t, f.inc7, "SALA ANALISI - STATISTICHE DELLA SANDBOX");
    const Font& g = f.fos5;
    const Rettangolo a = q::schermo(t, {u.x, u.y, u.w, 424 - u.y}, g, "PROVA S5 CONVOGLIO", "0:09:58");
    int y = a.y + 2;
    y += q::linguette(t, a, y, g, PAGINE, 1) + 4;
    const std::vector<int> col = {112, 226, 304, 352, 400, 448, 500, 562};
    y += q::intestazione(t, a, y, g, "OSSERVATORE", col, {"BERSAGLIO", "SENSORE", "POSS.", "CONT.", "SOLIDO", "CLASS.", "DIST.", "RITARDO"});
    struct R {
        const char* o;
        std::vector<std::string> v;
    };
    const R righe[] = {
        {"VICTOR III 12", {"MERCANTILE 2", "", "00:00", "00:00", "00:40", "01:16", "2,6 KM", "-"}},
        {"VICTOR III 12", {"CARGO 3", "", "00:00", "00:00", "00:34", "01:16", "2,6 KM", "-"}},
        {"VICTOR III 12", {"PETROLIERA 4", "", "00:00", "00:00", "00:32", "01:16", "1,4 KM", "-"}},
        {"VICTOR III 12", {"MERCANTILE 5", "", "00:00", "00:00", "00:39", "01:16", "1,4 KM", "-"}},
        {"VICTOR III 12", {"PERRY 8", "", "00:00", "00:00", "00:38", "-", "7,3 KM", "-"}},
        {"VICTOR III 12", {"PERRY 9", "", "00:00", "00:00", "00:32", "-", "4,6 KM", "-"}},
        {"VICTOR III 12", {"PERRY 10", "", "00:00", "00:00", "00:48", "-", "4,6 KM", "-"}},
        {"LOS ANGELES 0", {"VICTOR III 12", "BQQ-5 SFERA", "00:00", "00:00", "-", "01:17", "3,7 KM", "0 S"}},
        {"VICTOR III 12", {"CARGO 6", "SOVIETICO 3", "00:00", "00:00", "01:14", "01:16", "1,4 KM", "0 S"}},
        {"VICTOR III 12", {"PETROLIERA 7", "SOVIETICO 3", "00:00", "00:00", "01:14", "01:16", "1,4 KM", "0 S"}},
        {"PERRY 9", {"VICTOR III 12", "SQS-53", "00:10", "00:10", "04:18", "-", "4,6 KM", "0 S"}},
        {"PERRY 10", {"VICTOR III 12", "SQS-53", "00:13", "00:13", "05:44", "-", "4,6 KM", "0 S"}},
        {"VICTOR III 12", {"LOS ANGELES 0", "SOVIETICO 3", "05:03", "05:03", "06:40", "06:41", "4,1 KM", "0 S"}},
    };
    int k = 0;
    for (const auto& r : righe) {
        q::riga(t, a, y, g, r.o, col, r.v, k == 7);   // la riga scelta: il clic apre il replay da li'
        y += 12;
        ++k;
    }
    y += 6;
    scrivi(t, g, a.x + 4, y, "13 SCOPERTE. TEMPI DALL'INIZIO DELLA PARTITA (MM:SS). LA RIGA ACCESA: INVIO O CLIC, IL REPLAY DA LI'.",
           fosforo(3));
    piede_statistiche(t, f);
}

// Il replay (scelta dell'utente del 28/10/2026): la carta grande a tutto schermo, la legenda e la lettura in due riquadri
// sopra la carta, la linea del tempo come fascia sottile sotto, il piede comune. Le aree dei riquadri si stampano
// («SOPRA nome x y w h»): lo script le rimette sopra la carta incollata
void replay(Tela& t, const Fonti& f) {
    const Rettangolo u = q::consolle(t, f.inc7, "SALA ANALISI - REPLAY");
    const Font& g = f.fos5;
    const int bh = g.righe() + 6, by = 428 - 3 - bh;
    const Rettangolo c = q::schermo(t, {u.x, u.y, u.w, by - 3 - u.y}, g, "SCENE1A  QUADRO DEL GIOCATORE", "SEGUE IL BATTELLO");
    std::printf("CARTA replay %d %d %d %d\n", c.x, c.y, c.w, c.h);
    // la legenda, in basso a sinistra sulla carta. I segni della carta (grafica/simboli_carta), qui con gli indici fissi
    // 0-15: ottagono giallo l'ignoto, croce bianca la soluzione, freccia ciano l'arma, quadrato verde il battello
    const Rettangolo rl{c.x + 6, c.y + c.h - 6 - 58, 104, 58};
    std::printf("SOPRA replay %d %d %d %d\n", rl.x, rl.y, rl.w, rl.h);
    const Rettangolo l = q::riquadro(t, rl, g, "CIO' CHE SAPEVI");
    const char* leg[] = {"I CONTATTI", "SOLUZIONE TMA", "LE TUE ARMI", "IL BATTELLO"};
    for (int i = 0; i < 4; ++i) {
        const int cx = l.x + 6, cy = l.y + i * 10 + 3, y = l.y + i * 10;
        switch (i) {
            case 0:
                t.linea_orizzontale(cx - 1, cx + 1, cy - 3, 14), t.linea_orizzontale(cx - 1, cx + 1, cy + 3, 14);
                t.linea_verticale(cx - 3, cy - 1, cy + 1, 14), t.linea_verticale(cx + 3, cy - 1, cy + 1, 14);
                t.pixel(cx - 2, cy - 2, 14), t.pixel(cx + 2, cy - 2, 14), t.pixel(cx - 2, cy + 2, 14), t.pixel(cx + 2, cy + 2, 14);
                break;
            case 1: t.linea_orizzontale(cx - 3, cx + 3, cy, 15), t.linea_verticale(cx, cy - 3, cy + 3, 15); break;
            case 2:
                t.linea_orizzontale(cx - 3, cx + 3, cy, 11);
                t.pixel(cx + 2, cy - 1, 11), t.pixel(cx + 2, cy + 1, 11), t.pixel(cx + 1, cy - 2, 11), t.pixel(cx + 1, cy + 2, 11);
                break;
            default: t.rettangolo_pieno(cx - 2, cy - 2, 5, 5, 10); break;
        }
        scrivi(t, g, l.x + 16, y, leg[i], fosforo(5));
    }
    // la lettura, in alto a destra sulla carta: gli ultimi eventi e la quota degli ultimi 10 minuti
    const Rettangolo rd{c.x + c.w - 6 - 196, c.y + 6, 196, 112};
    std::printf("SOPRA replay %d %d %d %d\n", rd.x, rd.y, rd.w, rd.h);
    const Rettangolo d = q::riquadro(t, rd, g, "LETTURA: QUADRO");
    int y = d.y;
    const struct { const char *ora, *testo; bool qui; } ev[] = {
        {"03:13", "IDENTIFICATO: SPRUANCE", false}, {"03:21", "IDENTIFICATO: SPRUANCE", false},
        {"03:35", "MESSAGGIO SERIE 5", false},      {"03:44", "IDENTIFICATO: SPRUANCE", true}};
    for (const auto& e : ev) {
        if (e.qui) t.rettangolo_pieno(d.x, y - 1, d.w, g.righe() + 2, fosforo(1));
        scrivi(t, g, d.x + 2, y, e.ora, e.qui ? fosforo(7) : fosforo(4));
        scrivi(t, g, d.x + 36, y, e.testo, e.qui ? fosforo(7) : fosforo(5));
        y += 10;
    }
    y += 3;
    scrivi(t, g, d.x + 2, y, "QUOTA 10 MIN", fosforo(4));
    scrivi(t, g, d.x + d.w - 2 - largo(g, "NOI 6 M  BERS. 0"), y, "NOI 6 M", 10);
    scrivi(t, g, d.x + d.w - 2 - largo(g, "BERS. 0"), y, "BERS. 0", 12);
    y += 11;
    const Rettangolo p{d.x + 30, y + 3, d.w - 34, d.y + d.h - y - 6};
    for (const int m : {0, 100}) {   // la scala e la quota, a tratto; le etichette dentro il riquadro
        const int yy = p.y + m * (p.h - 1) / 100;
        scrivi(t, g, d.x + 2, m ? yy - g.righe() + 1 : yy, m ? "100M" : "0 M", fosforo(3));
        for (int x = p.x; x < p.x + p.w; x += 3) t.pixel(x, yy, fosforo(2));
    }
    for (int x = 0; x < p.w; ++x) {
        const int m = x < p.w / 3 ? 6 : 6 + (x - p.w / 3) * 40 / (p.w * 2 / 3);
        t.pixel(p.x + x, p.y + m * (p.h - 1) / 100, 10);
        if (x % 4 < 2) t.pixel(p.x + x, p.y, 12);   // il bersaglio in superficie, a tratti
    }
    // la linea del tempo, fascia sottile sotto la carta: le tacche degli eventi, in rosso il lancio nemico
    std::vector<double> tacche;
    for (const int s : {8, 12, 160, 172, 190, 233, 251, 265, 302, 333, 445}) tacche.push_back(s / 600.0);
    q::fascia_tempo(t, {u.x, by, u.w, bh}, g, "0:05:00 / 0:10:00  X1  PAUSA", "03:44 CONTATTO IDENTIFICATO: SPRUANCE", 0.5,
                    tacche, {30 / 600.0});
    lamiera::piede(t, f.orig, 428, "P GIOCA/PAUSA  + - VELOCITA'  FRECCE 30 S  TAB EVENTO  V LETTURA  I O ZOOM  CLIC: SALTA",
                   {{"ESC  ESCI", ColoreLampada::rossa}, {"V  LETTURA: VERITA'", ColoreLampada::ambra},
                    {"INVIO  RIPRENDI DA QUI", ColoreLampada::verde}},
                   2);
}


// --- gruppo 6 (03/11/2026): la vista dal vivo della sandbox in variante C, come il replay ---
// La consolle «SALA ANALISI - DAL VIVO»: la carta grande a sinistra (dall'istantanea di oggi), a destra lo schermo del
// registro con i filtri; sotto la fascia del tempo con ADESSO a destra, e il piede comune coi tasti di oggi
void vivo(Tela& t, const Fonti& f) {
    const Font& g = f.fos5;
    const q::DisposizioneVivo d = q::consolle_vivo(t, f.inc7, g, "SCENE0A  VERITA'", "DAL VIVO 0:04:59  X1", "TUTTI");
    std::printf("CARTA vivo %d %d %d %d\n", d.carta.x, d.carta.y, d.carta.w, d.carta.h);
    int y = d.registro.y + 2;
    const struct { const char *ora, *testo; std::uint8_t c; } ev[] = {
        {"03:51", "SLAVA PERDE BIOLOGICO", 0}, {"03:53", "KRIVAK2: TRACCIA TASM", 11}, {"04:02", "AKULA PERDE BIOLOGICO", 0},
        {"04:02", "LOS ANGELES: TRACCIA", 11}, {"04:11", "LOS ANGELES LANCIA TASM", 12}, {"04:12", "COMANDANTE IA: TRANSITO", 0},
        {"04:13", "LOS ANGELES: TRACCIA", 11}, {"04:14", "LOS ANGELES CAVITA", 0}, {"04:14", "PERISCOPIO ESPOSTO 250 S", 0},
        {"04:16", "KRIVAK2: TRACCIA BQM74", 11}};
    for (const auto& e : ev) {
        q::riga_registro(t, d.registro, y, g, e.ora, e.testo, e.c ? e.c : fosforo(5));
        y += 10;
    }
    q::riga(t, d.registro, d.registro.y + d.registro.h - 24, g, "SEGUE IL BATTELLO", {}, {});
    q::riga(t, d.registro, d.registro.y + d.registro.h - 13, g, "TAB SCEGLI UN'UNITA'", {}, {});
    std::vector<double> tacche;
    for (const int s : {12, 40, 95, 160, 172, 190, 233, 251, 265}) tacche.push_back(s / 300.0);
    q::fascia_tempo(t, d.fascia, g, "0:04:14 PERISCOPIO ESPOSTO 250 S", "ADESSO", 1.0, tacche, {251 / 300.0});
    lamiera::piede(t, f.orig, 428, "P PAUSA  + - VELOCITA'  FRECCE EVENTO  TAB UNITA'  S SEGUI  V LETTURA  I O ZOOM  G INTERVENTI",
                   {{"ESC  FINE", ColoreLampada::rossa}, {"C  CONFRONTO", ColoreLampada::ambra},
                    {"INVIO  ADESSO", ColoreLampada::verde}},
                   2);
}

// --- gruppo 3 (roadmap 33): briefing e debriefing nella lamiera di A coi documenti di B, la serie in C ---

constexpr std::uint8_t NERO = 0, BLU = 1, ROSSO = 4, CIANO_CHIARO = 11, ROSSO_CHIARO = 12,
                       GIALLO = 14, BIANCO = 15;

// il briefing: gli ordini sulla telescrivente a sinistra, a destra i livelli come interruttori a spia e la scheda
// della missione nei vetri; il piede comune
void briefing(Tela& t, const Fonti& f) {
    const Rettangolo u = q::consolle(t, f.inc7, "SALA BRIEFING - TORPEX '89");
    lamiera::testata_classificata(t, f.inc7, "SECRET", "141000Z MAR 89");
    const Font& o = f.orig;
    const int passo = o.righe() + 3;
    // gli ordini
    const Rettangolo pl{u.x, u.y, 420, 424 - u.y};
    lamiera::pannello(t, pl, 8);
    const Rettangolo a = lamiera::modulo_continuo(t, {pl.x + 9, pl.y + 9, pl.w - 18, pl.h - 18}, passo);
    int y = a.y;
    scrivi(t, o, a.x, y, "ORDINI DI MISSIONE  141000Z MAR 89", NERO);
    lamiera::timbro(t, o, a.x + a.w - largo(o, "CONFIDENTIAL") - 12, y - 2, "CONFIDENTIAL", ROSSO);
    y += passo;
    scrivi(t, o, a.x, y, "DA: CINCLANTFLT    A: USS LOS ANGELES (SSN 688)", NERO);
    y += passo;
    scrivi(t, o, a.x, y, "OGG.: TORPEX NLANT", NERO);
    y += passo;
    const char* paragrafi[] = {
        "BENVENUTO NELL'ATLANTICO DEL NORD, COMANDANTE. LEI DARA' INIZIO A UN'ESERCITAZIONE AL LARGO DELLA COSTA "
        "DELL'ISOLA PIU' MERIDIONALE DELL'ARCIPELAGO DELLE FAER OER. TRE CACCIATORPEDINIERE RADIATI DELLA CLASSE FORREST "
        "SHERMAN STANNO ATTRAVERSANDO L'ARCIPELAGO DIRETTI A NORD-OVEST. SONO STATI DOTATI DI PILOTI AUTOMATICI "
        "TELECOMANDATI, CHE CONSENTONO LORO ALCUNE SEMPLICI MANOVRE DURANTE L'ESERCITAZIONE.",
        "LA SUA MISSIONE E' DISTRUGGERE TUTTE LE UNITA' DA GUERRA ENTRO IL TEMPO ASSEGNATO. SILURI ATTIVI IN ACQUA, O IL "
        "TROPPO RUMORE DELLE MANOVRE AD ALTA VELOCITA', POSSONO RIVELARE LA SUA POSIZIONE AL SONAR AVVERSARIO, E UN "
        "PERISCOPIO ALZATO LA ESPONE ALL'INDIVIDUAZIONE RADAR."};
    y += passo;   // una riga bianca: il testo resta sulle righe del modulo
    for (const char* par : paragrafi) {
        for (const auto& r : lamiera::a_capo(o, par, a.w)) {
            if (y + passo > a.y + a.h - 2 * passo) break;
            scrivi(t, o, a.x, y, r, NERO);
            y += passo;
        }
        y += passo;
    }
    scrivi(t, o, a.x, a.y + a.h - passo, "(SEGUE)   1/2", BLU);
    // a destra: i livelli
    const int x0 = u.x + 426, w0 = u.w - 426;
    const Rettangolo pv{x0, u.y, w0, 8 * 12 + 26};
    lamiera::pannello(t, pv, 8);
    lamiera::secondaria(t, o, pv.x + 10, pv.y + 8, "LIVELLI  1-8, 9 TUTTO");
    const char* livelli[] = {"ORDINI E OBIETTIVI", "ZONE OPERATIVE", "INTELLIGENCE", "BASI E RADAR COSTIERI",
                             "PORTATE ASW NEMICHE", "SOSUS", "FONDALE", "EVENTI SPECIALI"};
    for (int i = 0; i < 8; ++i) {
        const bool acceso = i == 0 || i == 6;
        const int yy = pv.y + 22 + i * 12;
        lamiera::spia_voce(t, {pv.x + 9, yy - 1, 12, 9}, ColoreLampada::verde, acceso);
        const std::string s = std::to_string(i + 1) + " " + livelli[i];
        if (acceso) lamiera::incisa(t, o, pv.x + 26, yy, s, BIANCO);
        else lamiera::secondaria(t, o, pv.x + 26, yy, s);
    }
    // la scheda della missione: etichetta incisa, valore nel vetro
    int ys = pv.y + pv.h + 4;
    const Rettangolo ps{x0, ys, w0, 424 - ys};
    lamiera::pannello(t, ps, 8);
    ys += 8;
    const auto campo = [&](const char* et, const std::vector<std::pair<std::string, std::uint8_t>>& righe) {
        ys += lamiera::campo_vetro(t, o, ps.x + 8, ys, ps.w - 16, et, righe) + 4;
    };
    campo("BATTELLO, TEATRO, RADIO", {{"LOS ANGELES  FAROE", BIANCO}, {"RADIO 1005Z +- 1 MIN", BIANCO}});
    campo("REGOLE D'INGAGGIO", {{"SOTT. TRATTENUTO", GIALLO}, {"NAVI LIBERO  MERC. TRATT.", GIALLO}, {"NEUTR. TRATT.  AEREI TRATT.", GIALLO}});
    campo("METEO E PREVISIONE 12 H", {{"VENTO 270 FORZA 3, MARE 3", CIANO_CHIARO}, {"VIS. 30 KM, NUBI 1/8", CIANO_CHIARO}});
    campo("CODICE DELLA PARTITA", {{"4XGQT4-8R0000-02K3QN", CIANO_CHIARO}});
    lamiera::piede(t, o, 428, "1-8 LIVELLI   9 TUTTI   TAB CHIUDE GLI ORDINI   MAIUSC ACCELERA",
                   {{"ESC  RIFIUTA LA MISSIONE", ColoreLampada::rossa}, {"F7  SALA SILURI", ColoreLampada::ambra},
                    {"INVIO  IN MARE", ColoreLampada::verde}},
                   2);
}

// il debriefing: la carta nel pannello a sinistra, a destra il rapporto dattiloscritto su carta; il piede comune
void debriefing(Tela& t, const Fonti& f) {
    const Rettangolo u = q::consolle(t, f.inc7, "DEBRIEFING - TORPEX '89");
    lamiera::testata_classificata(t, f.inc7, "SECRET", "141006Z MAR 89");
    const Font& o = f.orig;
    const Rettangolo pl{u.x, u.y, 420, 424 - u.y};
    lamiera::pannello(t, pl, 8);
    const Rettangolo cr{pl.x + 9, pl.y + 9, pl.w - 18, pl.h - 18};
    lamiera::vetro(t, cr);
    const Rettangolo c{cr.x + 2, cr.y + 2, cr.w - 4, cr.h - 4};
    std::printf("CARTA debriefing %d %d %d %d\n", c.x, c.y, c.w, c.h);
    // il rapporto
    const int x0 = u.x + 426, w0 = u.w - 426;
    const Rettangolo fo{x0, u.y, w0, 424 - u.y};
    const Rettangolo fa = lamiera::foglio(t, fo);
    const int passo = 13, xa = fa.x, wa = fa.w;   // la riga della macchina da scrivere (8 di lettera)
    int y = fa.y;
    scrivi(t, o, xa, y, "RAPPORTO DI MISSIONE", NERO);
    for (int x = xa; x < xa + wa; ++x) t.pixel(x, y + 10, NERO);
    y += passo + 6;
    // il timbro dell'esito
    scrivi(t, o, xa, y + 2, "ESITO", NERO);
    lamiera::timbro(t, o, xa + 50, y - 1, "IN CORSO", ROSSO);
    y += passo + 6;
    const auto voce = [&](const std::string& et, const std::string& v) {
        lamiera::voce_modulo(t, o, xa, y, wa, et, v, BLU);
        y += passo;
    };
    voce("COMPLETEZZA", "0 PER CENTO");
    voce("DURATA", "00:06:59");
    voce("METEO", "MARE 3, VIS. 30 KM");
    y += 5;
    scrivi(t, o, xa, y, "OBIETTIVI", NERO);
    y += passo;
    const char* ob[] = {"AFFONDARE I TRE FORREST SHERMAN IN 15 MINUTI: 0 SU 3", "NON FARSI INDIVIDUARE DA NAVI O LAMPS",
                        "RIPORTARE A CASA IL BATTELLO", "RISPETTARE ROE E ZONE PROTETTE"};
    for (const char* s : ob) {
        lamiera::casella(t, xa, y, false);
        for (const auto& r : lamiera::a_capo(o, s, wa - 14)) scrivi(t, o, xa + 13, y, r, NERO), y += passo;
        y += 1;
    }
    y += 4;
    scrivi(t, o, xa, y, "ARMI E BERSAGLI", NERO);
    y += passo;
    voce("LANCI / A SEGNO", "1 / 0");
    voce("AFFONDATI: OSTILI", "0");
    voce("NEUTRALI / AMICI", "0 / 0");
    voce("SILURI EVITATI", "0");
    voce("COLPI SUBITI", "0");
    voce("CLASSIFICAZIONI GIUSTE", "3");
    voce("ERRATE", "0");
    scrivi(t, o, xa, fo.y + fo.h - 2 * passo - 4, "CODICE 4XGQT4-8R0000-02K3QN", NERO);
    scrivi(t, o, xa, fo.y + fo.h - passo - 4, "INVIO: RAPPORTO ACINT", BLU);
    lamiera::piede(t, o, 428, "< > EVENTO PRECEDENTE E SEGUENTE   TAB TUTTI   V VERITA'   I O ZOOM   INVIO ACINT",
                   {{"ESC  CHIUDI", ColoreLampada::rossa}, {"R  RIPRENDI", ColoreLampada::ambra, lamiera::Stato::disabilitato},
                    {"P  REPLAY", ColoreLampada::ambra}, {"E  ESITO", ColoreLampada::verde}},
                   3);
}

// la serie, pagina 2 «per parte»: per ogni grandezza la media e lo scarto, la barra (scatola media +- scarto, baffi
// dal minimo al massimo, la media bianca) e l'intervallo, NATO in ciano e Patto in rosso, sulla stessa scala per riga
struct Stat { double m, sd, lo, hi; bool c; };
void serie(Tela& t, const Fonti& f) {
    const Rettangolo u = q::consolle(t, f.inc7, "SALA ANALISI - SERIE DELLA SANDBOX");
    const Font& g = f.fos5;
    const Rettangolo a = q::schermo(t, {u.x, u.y, u.w, 424 - u.y}, g, "PROVA S5 CONVOGLIO", "3 PARTITE, SEMI 1-3");
    int y = a.y + 2;
    y += q::linguette(t, a, y, g, {"1 ESITI", "2 PER PARTE", "3 ISTOGRAMMI"}, 1) + 3;
    const int XN = 150, BN = 236, RN = 322, XP = 374, BP = 460, RP = 546, BW = 80;
    y += q::intestazione(t, a, y, g, "MEDIA +- SCARTO", {XN, RN, XP, RP}, {"NATO", "MIN-MAX", "PATTO", "MIN-MAX"});
    const auto num = [](double v) {
        char b[24];
        if (v == static_cast<long>(v)) std::snprintf(b, sizeof b, "%ld", static_cast<long>(v));
        else std::snprintf(b, sizeof b, v < 10 ? "%.2f" : "%.1f", v);
        std::string s = b;
        for (auto& ch : s) if (ch == '.') ch = ',';
        return s;
    };
    struct R { const char* n; Stat a, b; const char* u; };
    const Stat NO{0, 0, 0, 0, false};
    const R righe[] = {
        {"UNITA'", {11, 0, 11, 11, true}, {1, 0, 1, 1, true}, ""},
        {"PERSE", {0.33, 0.58, 0, 1, true}, {0, 0, 0, 0, true}, ""},
        {"INFLITTE", {0, 0, 0, 0, true}, {0.33, 0.58, 0, 1, true}, ""},
        {"SCAMBIO", {0, 0, 0, 0, true}, {0.33, 0.58, 0, 1, true}, ""},
        {"SCOPERTE FATTE", {3, 0, 3, 3, true}, {10, 0, 10, 10, true}, ""},
        {"SCOPERTE SUBITE", {10, 0, 10, 10, true}, {3, 0, 3, 3, true}, ""},
        {"RITARDO DI SCOPERTA", {0, 0, 0, 0, true}, {0, 0, 0, 0, true}, " S"},
        {"DISTANZA DI SCOPERTA", {4.31, 0, 4.31, 4.31, true}, {3.13, 0.01, 3.12, 3.15, true}, " KM"},
        {"SCOPERTA - ALLERTA", {674, 0, 674, 674, true}, {294, 34.8, 274, 335, true}, " S"},
        {"SCOPERTA - INGAGGIO", {578, 60.9, 533, 647, true}, {32.3, 0.52, 32, 32.9, true}, " S"},
        {"SCOPERTA - LANCIO", NO, {294, 34.8, 274, 335, true}, " S"},
        {"TEMPO ALLA SOLUZIONE", NO, {293, 35, 273, 334, true}, " S"},
        {"ERRORE AL LANCIO", NO, {1110, 761, 656, 1989, true}, " M"},
        {"LANCI", {0, 0, 0, 0, true}, {2, 0, 2, 2, true}, ""},
        {"A SEGNO", {0, 0, 0, 0, true}, {1, 0, 1, 1, true}, ""},
        {"ESCHE", {2, 0, 2, 2, true}, {0, 0, 0, 0, true}, ""},
        {"ESCHE EFFICACI", {0, 0, 0, 0, true}, {0, 0, 0, 0, true}, ""},
        {"MINACCIA - EVASIONE", {32.2, 1.91, 31, 34.4, true}, NO, " S"},
        {"PERISCOPIO ALZATO", {0, 0, 0, 0, true}, {0, 0, 0, 0, true}, " S"},
        {"ANTENNA ALZATA", {0, 0, 0, 0, true}, {0, 0, 0, 0, true}, " S"},
        {"RADAR ACCESO", {866, 42, 818, 891, true}, {0, 0, 0, 0, true}, " S"},
        {"PING", {214, 74.4, 147, 294, true}, {0, 0, 0, 0, true}, ""},
        {"TRASMISSIONI RADIO", {4.33, 1.15, 3, 5, true}, {0, 0, 0, 0, true}, ""},
        {"CONTRO-SCOPERTE", {0, 0, 0, 0, true}, {0, 0, 0, 0, true}, ""},
        {"PUNTI NAVE", {0, 0, 0, 0, true}, {0, 0, 0, 0, true}, ""},
    };
    int k = 0;
    for (const auto& r : righe) {
        q::riga(t, a, y, g, r.n, {}, {}, k++ % 2 == 1);
        const double scala = std::max({r.a.c ? r.a.hi : 0.0, r.b.c ? r.b.hi : 0.0, 1e-9});
        for (const auto& [s, xv, xb, xr, col] : {std::tuple{r.a, XN, BN, RN, CIANO_CHIARO}, std::tuple{r.b, XP, BP, RP, ROSSO_CHIARO}}) {
            if (!s.c) { scrivi(t, g, a.x + xv, y, "-", fosforo(3)); continue; }
            scrivi(t, g, a.x + xv, y, num(s.m) + " +- " + num(s.sd) + r.u, fosforo(6));
            scrivi(t, g, a.x + xr, y, s.lo == s.hi ? num(s.lo) : num(s.lo) + "-" + num(s.hi), fosforo(4));
            q::barra_statistica(t, {a.x + xb, y, BW, g.righe()}, scala, s.m, s.sd, s.lo, s.hi, col);
        }
        y += 10;
    }
    // in fondo la sintesi della pagina 1, che sta sempre sotto gli occhi
    y += 8;
    y += q::intestazione(t, a, y, g, "IN SINTESI (PAGINA 1)", {XN, XP}, {"NATO", "PATTO"});
    q::riga(t, a, y, g, "COME SONO FINITE", {XN}, {"NON FINITA: 3 SU 3"});
    y += 10;
    q::riga(t, a, y, g, "PK TYPE 53", {XN, XP}, {"-", "3 SU 6"});
    y += 10;
    q::riga(t, a, y, g, "CODICE DELLA SERIE", {XN}, {"451YS2-CG0000-02K000-0000HZ-G002TV"});
    lamiera::piede(t, f.orig, 428, "TAB PAGINA   FRECCE SCORRONO   CLIC SU UNA PARTITA: IL REPLAY",
                   {{"TAB  PAGINA SEGUENTE", ColoreLampada::ambra}, {"ESC  INDIETRO", ColoreLampada::verde}}, 1);
}

// --- gruppo 4 (roadmap 33): museo in A coi documenti di B, statistiche cumulate ed editor in C, albo vuoto in B ---
// I pezzi nuovi sono in grafica (approvati dall'utente il 29/10/2026): lamiera::elenco_vetro e in_colori_vista,
// quadro::stato_vuoto, posto_libero e piede_cerimonia in grafica/cornici.

// le rampe di cerimonia (l'albo, il fascicolo, la bacheca hanno i ritratti negli indici 16-47 del tema: niente lamiera)
constexpr std::uint8_t LEGNO = cerimonia::LEGNO, ORO = cerimonia::ORO, INCHIOSTRO = cerimonia::NERO + 1,
                       TENUE = cerimonia::NERO + 3;

const Palette* PALETTE_TEMA = nullptr;   // per in_colori_vista: il tema applicato e i grigi del giorno

// il museo: lo schedario a sinistra, a destra l'immagine nel vetro, la scheda tecnica e il testo su carta
void museo(Tela& t, const Fonti& f) {
    const Rettangolo u = q::consolle(t, f.inc7, "MUSEO - ENCICLOPEDIA DELLA SIMULAZIONE");
    const Font& o = f.orig;
    // lo schedario: la sezione fra le frecce, l'indice nel vetro
    const Rettangolo pl{u.x, u.y, 196, 424 - u.y};
    lamiera::pannello(t, pl, 8);
    lamiera::tasto(t, {pl.x + 8, pl.y + 8, 22, 17}, "<", ColoreLampada::ambra, lamiera::Stato::normale, false, o);
    lamiera::tasto(t, {pl.x + pl.w - 30, pl.y + 8, 22, 17}, ">", ColoreLampada::ambra, lamiera::Stato::normale, false, o);
    lamiera::incisa(t, o, pl.x + (pl.w - largo(o, "SOTTOMARINI")) / 2, pl.y + 13, "SOTTOMARINI", BIANCO);
    lamiera::secondaria(t, o, pl.x + 10, pl.y + 32, "SEZIONE 1 DI 18, 11 VOCI");
    const std::vector<std::string> voci = {"AKULA I", "ALFA", "CHARLIE II", "FOXTROT", "KILO", "LOS ANGELES", "OSCAR II",
                                           "STURGEON", "TRAFALGAR", "TYPHOON", "VICTOR III"};
    lamiera::elenco_vetro(t, {pl.x + 8, pl.y + 46, pl.w - 16, pl.h - 54}, o, voci, 0, 14);
    // la voce
    const int x0 = u.x + 202, w0 = u.w - 202;
    const Rettangolo pr{x0, u.y, w0, 424 - u.y};
    lamiera::pannello(t, pr, 8);
    lamiera::incisa(t, f.inc7, pr.x + 10, pr.y + 8, "AKULA I  (PROGETTO 971)", BIANCO);
    lamiera::secondaria(t, o, pr.x + 10, pr.y + 22, "SOTTOMARINI - URSS");
    const Rettangolo iv{pr.x + 8, pr.y + 34, 240, 132};
    lamiera::vetro(t, iv);
    std::printf("CARTA museo %d %d %d %d\n", iv.x + 2, iv.y + 2, iv.w - 4, iv.h - 4);
    lamiera::secondaria(t, o, iv.x, iv.y + iv.h + 5, "IN GIOCO, AOB 90 DRITTA");
    lamiera::tasto_tastiera(t, {iv.x + iv.w - 54, iv.y + iv.h + 3, 26, 12}, "TAB", o);
    lamiera::incisa(t, o, iv.x + iv.w - 24, iv.y + iv.h + 5, "1/8", BIANCO);
    // la scheda tecnica: un cartellino battuto a macchina
    const Rettangolo sc = lamiera::foglio(t, {iv.x + iv.w + 6, iv.y, pr.x + pr.w - 8 - (iv.x + iv.w + 6), iv.h + 16});
    int y = sc.y;
    scrivi(t, o, sc.x, y, "SCHEDA TECNICA", NERO);
    y += 14;
    const char* sch[][2] = {{"LUNGHEZZA", "110,3 M"}, {"LARGHEZZA", "13,6 M"}, {"PESCAGGIO", "9,7 M"}, {"VELOCITA'", "33 KN"},
                            {"ELICA", "7 PALE"},      {"CAVITAZIONE", "5,5 KN"}, {"PERISCOPI", "2 (VEDI)"}, {"Q. PERISC.", "13 M"}};
    for (const auto& s : sch) lamiera::voce_modulo(t, o, sc.x, y, sc.w, s[0], s[1], BLU), y += 12;
    // il testo: dattiloscritto; «in gioco» a penna; i rimandi numerati
    const Rettangolo ft = lamiera::foglio(t, {pr.x + 8, iv.y + iv.h + 20, pr.w - 16, pr.y + pr.h - 8 - (iv.y + iv.h + 20)});
    const int passo = 11;
    y = ft.y;
    const char* storia =
        "IL PROGETTO 971 \"SHCHUKA-B\", AKULA PER LA NATO, FU LO CHOC DEGLI ANNI '80 PER LA US NAVY: LA PRIMA UNITA', "
        "K-284, ENTRO' IN SERVIZIO ALLA FINE DEL 1984 E NEL 1989 CE N'ERANO CIRCA CINQUE. ERA PIU' SILENZIOSO DI OGNI "
        "SOTTOMARINO NUCLEARE SOVIETICO PRECEDENTE: AL SALTO CONTRIBUIRONO LE INFORMAZIONI DELLA RETE WALKER E LE "
        "FRESATRICI TOSHIBA-KONGSBERG USATE PER LE ELICHE. FILA A 33 NODI, LAVORA FINO A CIRCA 500 METRI, HA 73 UOMINI "
        "E IL SONAR DIGITALE SKAT-3 CON ANTENNE LATERALI E UNA SCHIERA RIMORCHIATA NEL BULBO SUL TIMONE.";
    for (const auto& r : lamiera::a_capo(o, storia, ft.w)) scrivi(t, o, ft.x, y, r, NERO), y += passo;
    y += 4;
    const char* gioco =
        "IN GIOCO: E' L'AVVERSARIO PIU' DIFFICILE. A VELOCITA' SILENZIOSA FA CIRCA 112 DB, QUANTO IL 688, E CHI SENTE "
        "PER PRIMO DIPENDE DA QUOTA, STRATO E PAZIENZA. IL SUO SONAR CLASSIFICA DA SOLO I CONTATTI. PER TROVARLO "
        "CONVIENE ANDARE PIANO CON LA SCHIERA RIMORCHIATA E CERCARE LE SUE RIGHE AL LOFAR.";
    const int y_gioco = y;
    for (const auto& r : lamiera::a_capo(o, gioco, ft.w - 8)) scrivi(t, o, ft.x + 6, y, r, BLU), y += passo;
    t.linea_verticale(ft.x, y_gioco - 1, y - 3, BLU);   // il segno a margine della penna
    y += 4;
    scrivi(t, o, ft.x, y, "VEDI:", NERO);
    const char* vedi[] = {"1 LEBED'-11/21", "2 SIGNAL-3", "3 BUKHTA", "4 SNOOP TRAY"};
    int xv = ft.x + largo(o, "VEDI: ");
    for (const char* v : vedi) {
        scrivi(t, o, xv, y, v, ROSSO);
        xv += largo(o, v) + 14;
    }
    scrivi(t, o, ft.x + ft.w - largo(o, "(SEGUE) 1/2"), ft.y + ft.h - passo + 2, "(SEGUE) 1/2", BLU);
    lamiera::piede(t, o, 428, "FRECCE VOCE E SEZIONE   TAB IMMAGINE   PAG TESTO   1-9 VEDI   L LINGUA",
                   {{"ESC  INDIETRO", ColoreLampada::rossa}, {"TAB  IMMAGINE 1/8", ColoreLampada::ambra},
                    {"PAG  SEGUE 1/2", ColoreLampada::verde}},
                   2);
    // la lamiera nei colori della vista (scelta dell'utente del 29/10/2026): l'immagine tiene la palette della vista
    lamiera::in_colori_vista(t, {0, 0, 640, 480}, *PALETTE_TEMA);
}

// le statistiche cumulate senza missioni: i trattini spenti al posto degli zeri, lo stato vuoto sulla carta
void statistiche_cumulate(Tela& t, const Fonti& f) {
    const Rettangolo u = q::consolle(t, f.inc7, "SALA ANALISI - STATISTICHE GLOBALI");
    const Font& g = f.fos5;
    const Rettangolo a = q::schermo(t, {u.x, u.y, 300, 424 - u.y}, g, "TUTTI I COMANDANTI", "0 MISSIONI");
    int y = a.y + 4;
    const std::vector<int> col = {206};
    const auto gruppo = [&](const char* titolo, const std::vector<const char*>& righe) {
        y += q::intestazione(t, a, y, g, titolo);
        int k = 0;
        for (const char* r : righe) {
            q::riga(t, a, y, g, r, {}, {}, k++ % 2 == 1);
            scrivi(t, g, a.x + col[0], y, "-", fosforo(3));
            y += 10;
        }
        y += 4;
    };
    gruppo("TOTALI", {"IN MARE", "MIGLIA PERCORSE", "IN SUPERFICIE", "A QUOTA PERISCOPICA", "IN CAVITAZIONE", "QUOTA MEDIA",
                      "PERISCOPIO / ANTENNA", "TRASMISSIONI / PING", "LANCI / A SEGNO", "OSTILI AFFONDATI"});
    gruppo("MEDIE PER MISSIONE", {"DURATA", "MIGLIA", "LANCI"});
    gruppo("RILEVAMENTI", {"TI HANNO TROVATO", "CLASSIFICATI (GIUSTE / ERRATE)"});
    gruppo("PRIMATI", {"QUOTA MASSIMA", "VELOCITA' MASSIMA", "MISSIONE PIU' LUNGA"});
    scrivi(t, g, a.x + 4, a.y + a.h - 12, "I NUMERI ARRIVANO DOPO IL PRIMO DEBRIEFING", fosforo(3));
    const Rettangolo b = q::schermo(t, {u.x + 304, u.y, u.w - 304, 424 - u.y}, g, "DOVE SEI STATO", "FAER OER  1/6");
    const Rettangolo carta{b.x + 4, b.y + 4, b.w - 8, 230};
    for (int yy = carta.y; yy < carta.y + carta.h; yy += 12)   // la griglia delle celle, spenta
        for (int xx = carta.x; xx < carta.x + carta.w; xx += 2) t.pixel(xx, yy, fosforo(1));
    for (int xx = carta.x; xx < carta.x + carta.w; xx += 12)
        for (int yy = carta.y; yy < carta.y + carta.h; yy += 2) t.pixel(xx, yy, fosforo(1));
    q::stato_vuoto(t, carta, g, "NESSUNA MISSIONE ANCORA",
                {"LA CARTA DEL TEATRO SI COLORA DOVE PORTI", "IL BATTELLO, A CELLE DI 2 KM.", "TAB: GLI ALTRI TEATRI"});
    y = carta.y + carta.h + 6;
    scrivi(t, g, b.x + 4, y, "POCO", fosforo(3));
    for (int i = 0; i < 8; ++i) t.rettangolo_pieno(b.x + 34 + i * 12, y, 11, 7, fosforo(i < 2 ? 1 : 2));
    scrivi(t, g, b.x + 34 + 8 * 12 + 4, y, "MOLTO", fosforo(3));
    y += 16;
    y += q::intestazione(t, b, y, g, "ARMI (LANCI / A SEGNO)", {150}, {"AFFONDATI"});
    scrivi(t, g, b.x + 4, y, "NESSUN LANCIO", fosforo(3));
    scrivi(t, g, b.x + 150, y, "NESSUNO", fosforo(3));
    lamiera::piede(t, f.orig, 428, "TAB ALTRO TEATRO   CLIC SULLA CARTA: IL TEATRO SEGUENTE",
                   {{"TAB  ALTRO TEATRO", ColoreLampada::ambra}, {"ESC  INDIETRO", ColoreLampada::verde}}, 1);
}

// l'albo vuoto: il cartellino della carriera nuova e i sette posti liberi, il piede di cerimonia
void albo(Tela& t, const Fonti& f) {
    const Font& o = f.orig;
    for (int y = 0; y < 480; ++y)   // la scrivania, come app/albo.cpp
        for (int x = 0; x < 640; ++x)
            t.pixel(x, y, static_cast<std::uint8_t>(LEGNO + cerimonia::retina(2.2 + 1.2 * std::sin((x + 9 * std::sin(y * 0.05)) * 0.05) +
                                                                                0.5 * cerimonia::caso(x / 5, y, 2), x, y, 7)));
    intestazione(t, o, {0, 0, 640, 38}, "ALBO DEI COMANDANTI", "CHI VA IN MARE CON LA PROSSIMA MISSIONE", Marina::usn, false);
    const auto cartellino = [](int i) { return Rettangolo{12 + (i % 4) * 156, 46 + (i / 4) * 190, 148, 182}; };
    for (int i = 1; i < 8; ++i) m688::posto_libero(t, cartellino(i), o, i + 1);
    const Rettangolo r = cartellino(0);
    carta(t, r, 11);
    scrivi(t, o, r.x + (r.w - largo(o, "NUOVA CARRIERA")) / 2, r.y + 8, "NUOVA CARRIERA", INCHIOSTRO);
    const Rettangolo foto{r.x + 34, r.y + 22, 80, 96};   // il posto della foto: vuoto, col segno a penna
    for (int x = foto.x; x < foto.x + foto.w; x += 3) t.pixel(x, foto.y, TENUE), t.pixel(x, foto.y + foto.h - 1, TENUE);
    for (int y = foto.y; y < foto.y + foto.h; y += 3) t.pixel(foto.x, y, TENUE), t.pixel(foto.x + foto.w - 1, y, TENUE);
    const int cx = foto.x + foto.w / 2, cy = foto.y + foto.h / 2;
    for (int d = -10; d <= 10; ++d) t.pixel(cx + d, cy, INCHIOSTRO), t.pixel(cx, cy + d, INCHIOSTRO);
    int y = foto.y + foto.h + 6;
    for (const char* s : {"IL NOME", "688 O ALFA", "COMPLETA O BREVE", "L'INDULGENZA"})
        scrivi(t, o, r.x + (r.w - largo(o, s)) / 2, y, s, TENUE), y += 10;
    for (int k = 1; k <= 3; ++k) {   // la cornice della scelta
        t.linea_orizzontale(r.x - k, r.x + r.w - 1 + k, r.y - k, ORO + 6), t.linea_orizzontale(r.x - k, r.x + r.w - 1 + k, r.y + r.h - 1 + k, ORO + 6);
        t.linea_verticale(r.x - k, r.y - k, r.y + r.h - 1 + k, ORO + 6), t.linea_verticale(r.x + r.w - 1 + k, r.y - k, r.y + r.h - 1 + k, ORO + 6);
    }
    m688::piede_cerimonia(t, o, 428, "F FASCICOLO, B RICONOSCIMENTI, BACKSP ELIMINA: QUANDO C'E' UN COMANDANTE",
                    {{"ESC  MENU"}, {"O  ALBO D'ONORE"}, {"S  STATISTICHE"}, {"INVIO  NUOVA CARRIERA"}}, 3);
}

// --- il piede della cerimonia (proposta c del kit, 29/10/2026): fascicolo, bacheca ed esito prendono il piede di legno e
// ottone dell'albo (piede_cerimonia a y = 428) al posto della riga d'aiuto o dei pulsanti grigi. Sopra, l'istantanea di
// oggi fino a 428: la carta e la scheda vanno accorciate perche' finiscano a 424
void fascicolo_piede(Tela& t, const Fonti& f) {
    std::printf("CARTA fascicolo_piede 0 0 640 428\n");
    m688::piede_cerimonia(t, f.orig, 428, "PAGINA 1 DI 5: LA COPERTINA   SU/GIU' SCORRE",
                          {{"ESC  INDIETRO"}, {"P  REPLAY", false}, {"SINISTRA  PRIMA"}, {"DESTRA  DOPO"}}, 3);
}
void bacheca_piede(Tela& t, const Fonti& f) {
    std::printf("CARTA bacheca_piede 0 0 640 428\n");
    m688::piede_cerimonia(t, f.orig, 428, "1-7 O TAB LE SCHEDE, LE FRECCE IL MEDAGLIONE",
                          {{"ESC  INDIETRO"}, {"G  COMANDANTE", false}, {"TAB  SCHEDA"}}, -1);
}
void esito_piede(Tela& t, const Fonti& f) {
    std::printf("CARTA esito_piede 0 0 640 428\n");
    m688::piede_cerimonia(t, f.orig, 428, "PAGINA 1 DI 2: IL RAPPORTO DI PATTUGLIA",
                          {{"ESC  DEBRIEFING"}, {"INVIO  CONTINUA"}}, 1);
}

// l'editor, scheda 1: la carta nello schermo grande, a destra le schede a linguette e i campi per gruppi, sotto la
// verifica sempre in vista
// la cornice comune delle schede: la carta nello schermo grande, a destra le linguette; restituisce l'area della scheda
// e la y dove comincia. In fondo la verifica sempre in vista, sotto il piede comune
Rettangolo editor_scheda(Tela& t, const Fonti& f, int scheda, const char* nome, int& y) {
    const Rettangolo u = q::consolle(t, f.inc7, "SALA PIANI - EDITOR DI MISSIONE");
    const Font& g = f.fos5;
    const int wl = 392;
    const Rettangolo c = q::schermo(t, {u.x, u.y, wl, 424 - u.y}, g, "NUOVA MISSIONE", "FAER OER - 688");
    std::printf("CARTA %s %d %d %d %d\n", scheda == 1 ? "editor" : scheda == 2 ? "editor_unita" : "editor_briefing", c.x, c.y, c.w, c.h);
    const Rettangolo a = q::schermo(t, {u.x + wl + 4, u.y, u.w - wl - 4, 424 - u.y}, g, "SCHEDA " + std::to_string(scheda), nome);
    y = a.y + 2;
    y += q::linguette(t, a, y, g, {"1 MISS", "2 UNIT", "3 ROTT", "4 ZONE"}, scheda <= 4 ? scheda - 1 : -1);
    y += q::linguette(t, a, y, g, {"5 OBIE", "6 REGO", "7 BRIE", "8 PROV"}, scheda > 4 ? scheda - 5 : -1) + 4;
    const int yv = a.y + a.h - 14;   // la verifica: la spia verde senza errori
    t.linea_orizzontale(a.x, a.x + a.w - 1, yv - 3, fosforo(2));
    lamiera::spia_voce(t, {a.x + 3, yv - 1, 12, 9}, ColoreLampada::verde, true);
    scrivi(t, g, a.x + 20, yv, "0 ERRORI, 0 ATTENZIONI", fosforo(6));
    lamiera::piede(t, f.orig, 428, "1-8 SCHEDE  CLIC SCEGLIE  TRASCINA SPOSTA  DESTRO AGGIUNGE  I O SCALA",
                   {{"ESC  INDIETRO", ColoreLampada::rossa}, {"S  SALVA", ColoreLampada::ambra}, {"E  ESPORTA", ColoreLampada::ambra},
                    {"G  PROVA IN MARE", ColoreLampada::verde}},
                   3);
    return a;
}

// i campi di una scheda, per gruppi: la riga scelta accesa, col segno ambra a sinistra e il cursore
struct Campi {
    Tela& t;
    Rettangolo a;
    const Font& g;
    int& y;
    int scelto, k = 0;
    std::vector<int> col = {84};
    void gruppo(const std::string& titolo, const std::vector<std::pair<std::string, std::string>>& righe, bool cursore = true) {
        y += q::intestazione(t, a, y, g, titolo);
        for (const auto& [n, v] : righe) {
            const bool s = k == scelto;
            q::riga(t, a, y, g, n, col, {s && cursore ? v + "_" : v}, s || k % 2 == 1);
            if (s) t.rettangolo_pieno(a.x, y - 1, 2, g.righe() + 2, lampada(ColoreLampada::ambra, true));
            y += 10;
            ++k;
        }
        y += 3;
    }
    void nota(const std::string& s) {
        for (const auto& r : lamiera::a_capo(g, s, a.w - 8)) scrivi(t, g, a.x + 4, y, r, fosforo(3)), y += 9;
    }
};

// l'editor, scheda 1: i campi in tre gruppi (scelta dell'utente del 29/10/2026)
void editor(Tela& t, const Fonti& f) {
    int y = 0;
    const Rettangolo a = editor_scheda(t, f, 1, "LA MISSIONE", y);
    Campi c{t, a, f.fos5, y, 0};
    c.gruppo("LA MISSIONE", {{"TITOLO", "NUOVA MISSIONE"}, {"TITOLO EN", "NEW MISSION"}, {"AUTORE", "-"}, {"ID", "EDITOR NUOVA"}});
    c.gruppo("LUOGO E TEMPO", {{"TEATRO", "FAER OER"}, {"PARTE", "688 (USA)"}, {"DATA", "15/10/1989"}, {"ORA UTC", "12:00"},
                               {"METEO", "SEMPLIFICATO"}, {"METEO FISSO", "NO"}, {"SITUAZIONE", "GUERRA"}});
    c.gruppo("REGOLE", {{"DANNI INIZ.", "NESSUNO"}, {"ARMI", "VERE"}, {"PARANOIA", "1"}, {"DIFFICOLTA'", "3"}, {"DURATA", "3:00"},
                        {"CARICO", "ANTISOM"}, {"ESCHE MOSS", "0"}, {"OSPITI", "0"}, {"CARRIERA", "NO"}, {"FINE", "SOLO TEMPO"}});
    c.nota("BERSAGLI FINITI: LE UNITA' DEGLI OBIETTIVI AFFONDARE. LE ALTRE FINI CON LE REGOLE.");
}

// la scheda 2, le unita': la prossima unita' da mettere, l'elenco delle unita' con la scelta accesa, i campi di quella
void editor_unita(Tela& t, const Fonti& f) {
    int y = 0;
    const Rettangolo a = editor_scheda(t, f, 2, "LE UNITA'", y);
    const Font& g = f.fos5;
    Campi c{t, a, g, y, -1};
    c.gruppo("LA PROSSIMA", {{"PARTE", "< URSS >"}, {"CLASSE", "< KRIVAK II >"}}, false);
    c.nota("CLIC DESTRO SULLA CARTA: LA METTE LI'.");
    y += 5;
    y += q::intestazione(t, a, y, g, "SULLA CARTA: 1 UNITA'");
    q::riga(t, a, y, g, "0", {18, 108, 140}, {"LOS ANGELES", "USA", "GIOCATORE"}, true);
    t.rettangolo_pieno(a.x, y - 1, 2, g.righe() + 2, lampada(ColoreLampada::ambra, true));
    y += 13;
    Campi d{t, a, g, y, 2};
    d.gruppo("UNITA' 0, IL GIOCATORE", {{"CLASSE", "LOS ANGELES"}, {"PARTE", "USA"}, {"ROTTA", "0"}, {"VELOCITA' KN", "5"},
                                        {"QUOTA M", "45"}});
    d.nota("TRASCINA L'UNITA' SULLA CARTA PER SPOSTARLA. X LA ELIMINA.");
}

// la scheda 7, il briefing: le righe numerate come le vedra' il comandante, la scelta accesa col cursore, sotto i tre
// comandi come tasti
void editor_briefing(Tela& t, const Fonti& f) {
    int y = 0;
    const Rettangolo a = editor_scheda(t, f, 7, "IL BRIEFING", y);
    const Font& g = f.fos5;
    y += q::intestazione(t, a, y, g, "ITALIANO", {}, {});
    const char* righe[] = {"ORDINI DI MISSIONE", "OGG.: NUOVA MISSIONE", "15 OTTOBRE 1989, ORE 1200Z. TEATRO: FAER OER.",
                           "- RIPORTARE A CASA IL BATTELLO", "REGOLE D'INGAGGIO: OSTILITA' APERTE, ARMI LIBERE.",
                           "TEMPO A DISPOSIZIONE: 3:00."};
    int k = 0;
    for (const char* r : righe) {
        const bool s = k == 3;
        bool prima = true;
        for (const auto& pezzo : lamiera::a_capo(g, s ? std::string(r) + "_" : std::string(r), a.w - 24)) {
            if (s) t.rettangolo_pieno(a.x + 2, y - 1, a.w - 4, g.righe() + 2, fosforo(1));
            if (prima) scrivi(t, g, a.x + 4, y, std::to_string(k + 1), fosforo(3));
            scrivi(t, g, a.x + 18, y, pezzo, s ? fosforo(7) : fosforo(5));
            if (s) t.rettangolo_pieno(a.x, y - 1, 2, g.righe() + 2, lampada(ColoreLampada::ambra, true));
            y += 10;
            prima = false;
        }
        ++k;
    }
    y += 6;
    const int w = (a.w - 12) / 2;
    lamiera::tasto(t, {a.x + 4, y, w, 18}, "AGGIUNGI UNA RIGA", ColoreLampada::verde, lamiera::Stato::normale, false, f.orig);
    lamiera::tasto(t, {a.x + 8 + w, y, w, 18}, "X  TOGLI LA RIGA", ColoreLampada::rossa, lamiera::Stato::normale, false, f.orig);
    y += 22;
    lamiera::tasto(t, {a.x + 4, y, a.w - 8, 18}, "RIFAI IL BRIEFING AUTOMATICO", ColoreLampada::ambra, lamiera::Stato::normale, false, f.orig);
    y += 26;
    Campi c{t, a, g, y, -1};
    c.nota("CLIC SU UNA RIGA: LA SCRIVI. CLIC SU ITALIANO: L'INGLESE, POI LA RADIO.");
}


// --- gruppo 5 (roadmap 33): le stazioni dell'Enhanced. La parte di sopra (la vista, 640x360) e il pannello di sotto
// (360-480, le consolle di grafica/sale) sono separati: ogni tavola ridisegna una delle due e incolla l'altra
// dall'istantanea di oggi (CARTA nome x y w h). Indici: il tema delle stazioni (16-47) e gli EGA, come le consolle.

constexpr int VISTA_H = 360;

// le tavole dell'Alfa (da approvare a parte): le stesse disposizioni col tema dell'Alfa (smalto beige, fosforo verde
// giallino), le scritte incise a stampino e i nomi, le armi e i messaggi del battello sovietico
bool ALFA = false;
const char* testo(const char* usa, const char* urss) { return ALFA ? urss : usa; }
std::string carta_nome(const char* nome) { return ALFA ? std::string(nome) + "_alfa" : std::string(nome); }
// le scritte sulla lamiera: sul grigio del 688 chiare con l'ombra; sul beige dell'Alfa scure e piatte, come le
// targhette delle sue consolle (KURS, UGROZA)
lamiera::Scritte scritte() { return ALFA ? lamiera::Scritte::scure : lamiera::Scritte::chiare; }
void incisa_su(Tela& t, const Font& f, int x, int y, const std::string& s, std::uint8_t c) {
    if (ALFA) lamiera::principale(t, f, x, y, s, lamiera::Scritte::scure);
    else lamiera::incisa(t, f, x, y, s, c);
}
void seconda(Tela& t, const Font& f, int x, int y, const std::string& s) { lamiera::secondaria(t, f, x, y, s, false, scritte()); }

// la sala radio: sopra, il messaggio scelto sulla telescrivente a sinistra; a destra lo stato in vetri e spie e
// l'elenco dei messaggi ricevuti. Il pannello di sotto resta quello di oggi
void radio(Tela& t, const Fonti& f) {
    const Font& o = f.orig;
    lamiera::lamiera(t, {0, 0, 640, VISTA_H}, 7, 5);
    std::printf("CARTA %s %d %d %d %d\n", carta_nome("radio").c_str(), 0, VISTA_H, 640, 480 - VISTA_H);
    // la fascia di stato: antenne, bande, la finestra, le ROE come spie
    const Rettangolo ps{4, 4, 632, 56};
    lamiera::pannello(t, ps, 8);
    incisa_su(t, f.inc7, ps.x + 10, ps.y + 7, testo("SALA RADIO  141001Z MAR 89", "RADIORUBKA  020601Z APR 89"), BIANCO);
    struct Spia { const char* nome; ColoreLampada c; bool accesa; };
    const Spia antenne_usa[] = {{"ALBERO", ColoreLampada::verde, true}, {"FILO", ColoreLampada::verde, false},
                                {"VLF", ColoreLampada::verde, true},    {"HF", ColoreLampada::verde, true},
                                {"UHF", ColoreLampada::verde, true}};
    const Spia antenne_urss[] = {{"ALBERO", ColoreLampada::verde, false}, {"FILO", ColoreLampada::verde, false},
                                 {"ELF", ColoreLampada::verde, true},     {"VLF", ColoreLampada::verde, false},
                                 {"HF", ColoreLampada::verde, false},     {"UHF", ColoreLampada::verde, false}};
    const auto& antenne = ALFA ? std::vector<Spia>(std::begin(antenne_urss), std::end(antenne_urss))
                               : std::vector<Spia>(std::begin(antenne_usa), std::end(antenne_usa));
    int x = ps.x + 10;
    for (const auto& s : antenne) {
        lamiera::spia_voce(t, {x, ps.y + 24, 12, 9}, s.c, s.accesa);
        if (s.accesa) incisa_su(t, o, x + 16, ps.y + 25, s.nome, BIANCO);
        else seconda(t, o, x + 16, ps.y + 25, s.nome);
        x += 22 + largo(o, s.nome);
    }
    seconda(t, o, ps.x + 10, ps.y + 41, "ROE");
    const Spia roe[] = {{"SOTT.", ColoreLampada::rossa, true}, {"NAVI", ColoreLampada::verde, true},
                        {"MERC.", ColoreLampada::rossa, true}, {"NEUTR.", ColoreLampada::rossa, true},
                        {"AEREI", ColoreLampada::rossa, true}};
    x = ps.x + 40;
    for (const auto& s : roe) {
        lamiera::spia_voce(t, {x, ps.y + 40, 12, 9}, s.c, s.accesa);
        incisa_su(t, o, x + 16, ps.y + 41, s.nome, BIANCO);
        x += 22 + largo(o, s.nome);
    }
    // la finestra: il campo e la barra del tempo che manca
    const int xf = ps.x + 380;
    seconda(t, o, xf, ps.y + 9, "PROSSIMA FINESTRA");
    lamiera::vetro(t, {xf, ps.y + 20, 240, 14});
    scrivi(t, o, xf + 4, ps.y + 23, testo("1005Z +- 2 MIN", "0602Z +- 1 MIN"), fosforo(6));
    t.rettangolo_pieno(xf + 130, ps.y + 24, 104, 6, fosforo(1));
    t.rettangolo_pieno(xf + 130, ps.y + 24, 30, 6, fosforo(5));
    seconda(t, o, xf, ps.y + 41, testo("SESSIONE 1003Z, FATTE 0, MANCATE 0", "SESSIONE 0601Z, FATTE 0, MANCATE 0"));
    // il messaggio scelto, sulla telescrivente
    const Rettangolo pm{4, 64, 424, VISTA_H - 68};
    lamiera::pannello(t, pm, 8);
    const int passo = o.righe() + 3;
    const Rettangolo a = lamiera::modulo_continuo(t, {pm.x + 8, pm.y + 8, pm.w - 16, pm.h - 16}, passo);
    int y = a.y;
    scrivi(t, o, a.x, y, testo("O IMMEDIATE  141000Z MAR 89", "O IMMEDIATE  020600Z APR 89"), NERO);
    const std::string classifica = testo("CONFIDENTIAL", "SOV. SEKRETNO");
    lamiera::timbro(t, o, a.x + a.w - largo(o, classifica) - 12, y - 2, classifica, ROSSO);
    y += passo;
    scrivi(t, o, a.x, y, testo("DA: CINCLANTFLT   A: USS LOS ANGELES (SSN 688)", "DA: KP SF   A: PL LENINSKII KOMSOMOL"), NERO), y += passo;
    scrivi(t, o, a.x, y, testo("OGG.: TORPEX NLANT", "OGG.: SLEZHENIE"), NERO), y += 2 * passo;
    const char* corpo = ALFA ? "UN SOTTOMARINO NUCLEARE AMERICANO E' STATO SEGNALATO NEL MARE DI NORVEGIA, DIRETTO A NORD-EST. "
                               "PORTATEVI SULLA SUA ROTTA, STABILITE IL CONTATTO E SEGUITELO SENZA FARVI SCOPRIRE. NON APRITE "
                               "IL FUOCO SENZA UN ORDINE ESPLICITO. RIFERITE A OGNI SESSIONE LA POSIZIONE DEL CONTATTO."
                             : "BENVENUTO NELL'ATLANTICO DEL NORD, COMANDANTE. LEI DARA' INIZIO A UN'ESERCITAZIONE AL LARGO DELLA "
                        "COSTA DELL'ISOLA PIU' MERIDIONALE DELL'ARCIPELAGO DELLE FAER OER. TRE CACCIATORPEDINIERE RADIATI DELLA "
                        "CLASSE FORREST SHERMAN STANNO ATTRAVERSANDO L'ARCIPELAGO DIRETTI A NORD-OVEST. SONO STATI DOTATI DI "
                        "PILOTI AUTOMATICI TELECOMANDATI, CHE CONSENTONO LORO ALCUNE SEMPLICI MANOVRE DURANTE L'ESERCITAZIONE.";
    for (const auto& r : lamiera::a_capo(o, corpo, a.w)) {
        if (y + passo > a.y + a.h - passo) break;
        scrivi(t, o, a.x, y, r, NERO), y += passo;
    }
    scrivi(t, o, a.x, a.y + a.h - passo, "(SEGUE) 1/3   ORDINI: IL TESTO INTERO", BLU);
    // a destra: in onda, in programma, i messaggi ricevuti
    const int x0 = 432, w0 = 204;
    const Rettangolo pd{x0, 64, w0, VISTA_H - 68};
    lamiera::pannello(t, pd, 8);
    int yd = pd.y + 8;
    yd += lamiera::campo_vetro(t, o, pd.x + 8, yd, pd.w - 16, "IN ONDA",
                               {{testo("SERIE 1  P PRIORITY", "SERIE 1  O IMMEDIATE"), fosforo(6)},
                                {testo("1000Z-1002Z, 1A MESSA", "0601Z-0602Z, 1A MESSA"), fosforo(6)}}, scritte()) + 4;
    yd += lamiera::campo_vetro(t, o, pd.x + 8, yd, pd.w - 16, "IN PROGRAMMA",
                               {{testo("5 R 1002Z   2 O 1002Z", "5 R 0602Z   2 P 0606Z"), fosforo(5)},
                                {testo("3 R 1006Z   4 P 1010Z", "3 P 0611Z   4 O 0615Z"), fosforo(5)}, {"SERIE COMPLETA", fosforo(7)}}, scritte()) + 4;
    seconda(t, o, pd.x + 8, yd, "MESSAGGI RICEVUTI");
    yd += 12;
    lamiera::elenco_vetro(t, {pd.x + 8, yd, pd.w - 16, pd.y + pd.h - 8 - yd}, o,
                          ALFA ? std::vector<std::string>{"0 O 0600Z VLF SLEZHENIE"} : std::vector<std::string>{"0 O 1000Z VLF TORPEX", "1 P 1000Z (30%)"},
                          0, 14);
}

// la manovra: sopra, al posto della vista del periscopio abbassato, il ripetitore di manovra: il profilo della quota
// negli ultimi 10 minuti con lo strato e il fondo, e gli ordini contro lo stato. Il pannello di sotto resta
void manovra(Tela& t, const Fonti& f) {
    const Font& g = f.fos5;
    lamiera::lamiera(t, {0, 0, 640, VISTA_H}, 7, 6);
    std::printf("CARTA %s %d %d %d %d\n", carta_nome("manovra").c_str(), 0, VISTA_H, 640, 480 - VISTA_H);
    const Rettangolo a = q::schermo(t, {4, 4, 420, VISTA_H - 8}, g, "PROFILO DI QUOTA, ULTIMI 10 MIN", "PERISCOPIO GIU'");
    // i campioni d'esempio, uno ogni 10 s: la quota scende da 18 a 45 m, il fondo ondeggia attorno a 128 m
    q::ProfiloQuota pq;
    pq.periscopica_m = 18.0, pq.strato_m = 62.0, pq.scala_m = 150.0;
    for (int k = 0; k <= 60; ++k) {
        const double s = k / 60.0;
        pq.quota_m.push_back(s < 0.4 ? 18.0 : s < 0.7 ? 18.0 + (s - 0.4) / 0.3 * 27.0 : 45.0);
        pq.fondo_m.push_back(128.0 + 6.0 * std::sin(k * 0.6));
    }
    q::profilo_quota(t, a, g, pq);
    // a destra gli ordini contro lo stato
    const Rettangolo b = q::schermo(t, {428, 4, 208, VISTA_H - 8}, g, "ORDINATO / ATTUALE");
    int y = b.y + 4;
    const std::vector<int> col = {96, 152};
    y += q::intestazione(t, b, y, g, "", col, {"ORD.", "ORA"});
    const char* righe[][3] = {{"ROTTA", "000", "000"}, {"VELOCITA'", "5 KN", "4,2 KN"}, {"QUOTA", "45 M", "45 M"},
                              {"TIMONE", "0", "0"},    {"PIANI", "0", "+2"},       {"MACCHINA", testo("AH 1/3", "MV"), testo("AH 1/3", "MV")},
                              {"ZAVORRA", "-", "AUTO"}};
    int k = 0;
    for (const auto& r : righe) q::riga(t, b, y, g, r[0], col, {r[1], r[2]}, k++ % 2 == 1), y += 11;
    y += 8;
    y += q::intestazione(t, b, y, g, "SOTTO LA CHIGLIA");
    scrivi(t, g, b.x + 4, y, "83 M, IN SALITA", fosforo(6)), y += 11;
    scrivi(t, g, b.x + 4, y, testo("CAVITAZIONE A 7,5 KN", "CAVITAZIONE A 6 KN"), fosforo(5)), y += 18;
    scrivi(t, g, b.x + 4, b.y + b.h - 12, "PERISCOPIO SU: LA VISTA", fosforo(3));
}

// le armi col periscopio giu': sopra, al posto della vista sott'acqua, il ripetitore delle armi: a sinistra le armi in
// acqua (corsa, filo, modo, stato), a destra i tubi e il carico. Il pannello di sotto (fire control, weapon control
// panel) resta quello di oggi
void armi(Tela& t, const Fonti& f) {
    const Font& g = f.fos5;
    lamiera::lamiera(t, {0, 0, 640, VISTA_H}, 7, 6);
    std::printf("CARTA %s %d %d %d %d\n", carta_nome("armi").c_str(), 0, VISTA_H, 640, 480 - VISTA_H);
    const Rettangolo a = q::schermo(t, {4, 4, 420, VISTA_H - 8}, g, "ARMI IN ACQUA", testo("2 FILI SU 4", "SENZA FILO"));
    int y = a.y + 4;
    const std::vector<int> col = {20, 70, 130, 186, 244, 322};
    y += q::intestazione(t, a, y, g, "N.", col, {"ARMA", "CORSA", "FILO", "MODO", "STATO", "BERS."});
    const char* righe_usa[][7] = {{"1", "MK48", "4,2 KM", "9,8 KM", "ATTIVO", "AGGANCIO", "S4"},
                                  {"2", "MK48", "1,1 KM", "12,9 KM", "PASSIVO", "RICERCA", "S2"},
                                  {"3", "MK48", "7,5 KM", "TAGLIATO", "ATTIVO", "AUTONOMO", "-"}};
    // l'Alfa (carico.toml, antisom): SET-65 e 53-65K, senza filo; l'esca MG-74 dal suo lanciatore
    const char* righe_urss[][7] = {{"1", "SET-65", "3,6 KM", "-", "ATTIVO", "AGGANCIO", "S2"},
                                   {"2", "53-65K", "2,0 KM", "-", "SCIA", "RICERCA", "-"}};
    std::vector<const char* const*> righe;
    if (ALFA) for (const auto& r : righe_urss) righe.push_back(r);
    else for (const auto& r : righe_usa) righe.push_back(r);
    int k = 0;
    for (const auto* r : righe) q::riga(t, a, y, g, r[0], col, {r[1], r[2], r[3], r[4], r[5], r[6]}, k++ % 2 == 1), y += 11;
    y += 8;
    y += q::intestazione(t, a, y, g, testo("L'ARMA SCELTA: 1 MK48", "L'ARMA SCELTA: 1 SET-65"));
    const char* scelta_usa[] = {"RILEVAMENTO 012, DISTANZA STIMATA 3,1 KM", "QUOTA 60 M, VELOCITA' ALTA, RICERCA 100 M",
                                "AGGANCIO DA 40 S, IL BERSAGLIO ACCOSTA A DRITTA", "FILO: 9,8 KM SULLA BOBINA DEL BATTELLO"};
    const char* scelta_urss[] = {"RILEVAMENTO 347, DISTANZA STIMATA 4,2 KM", "QUOTA 80 M, 40 NODI, RICERCA ATTIVA",
                                 "AGGANCIO DA 25 S", "SENZA FILO: AUTONOMO DAL LANCIO"};
    const auto& scelta = ALFA ? scelta_urss : scelta_usa;
    for (const char* r : scelta) scrivi(t, g, a.x + 4, y, r, fosforo(6)), y += 11;
    scrivi(t, g, a.x + 4, a.y + a.h - 12, "PERISCOPIO SU: LA VISTA", fosforo(3));
    // a destra i tubi e il carico
    const Rettangolo b = q::schermo(t, {428, 4, 208, VISTA_H - 8}, g, "TUBI");
    y = b.y + 4;
    const std::vector<int> ct = {24, 92};
    y += q::intestazione(t, b, y, g, "T.", ct, {"ARMA", "STATO"});
    const char* tubi_usa[][3] = {{"1", "VUOTO", "-"}, {"2", "VUOTO", "-"}, {"3", "MK48", "PRONTO"}, {"4", "MK48", "ALLAGATO"}};
    const char* tubi_urss[][3] = {{"1", "VUOTO", "-"},       {"2", "VUOTO", "-"},       {"3", "SET-65", "PRONTO"},
                                  {"4", "53-65K", "ALLAGATO"}, {"5", "53-65K", "CARICO"}, {"E", "MG-74", "PRONTO"}};
    std::vector<const char* const*> tubi;
    if (ALFA) for (const auto& r : tubi_urss) tubi.push_back(r);
    else for (const auto& r : tubi_usa) tubi.push_back(r);
    k = 0;
    for (const auto* r : tubi) q::riga(t, b, y, g, r[0], ct, {r[1], r[2]}, k++ % 2 == 1), y += 11;
    y += 8;
    y += q::intestazione(t, b, y, g, "CARICO");
    const char* carico_usa[] = {"MK48       14", "HARPOON     4", "TOMAHAWK    4 + 12 VLS", "MK60 CAPTOR 0"};
    const char* carico_urss[] = {"SET-65      6", "53-65K      5", "MG-74       2", "ESCHE       4"};
    const auto& carico = ALFA ? carico_urss : carico_usa;
    for (const char* r : carico) scrivi(t, g, b.x + 4, y, r, fosforo(6)), y += 11;
    y += 8;
    y += q::intestazione(t, b, y, g, "RICARICA");
    scrivi(t, g, b.x + 4, y, testo("TUBO 1: MK48, 4 MIN", "TUBO 1: 53-65K, 3 MIN"), fosforo(5));
}

// la carta: la carta resta; il pannello di sotto con lo stato in spie e vetri e i comandi come tasti della tastiera
void carta_pannello(Tela& t, const Fonti& f) {
    const Font& o = f.orig;
    std::printf("CARTA %s %d %d %d %d\n", carta_nome("carta").c_str(), 0, 0, 640, VISTA_H);
    const Rettangolo p{0, VISTA_H, 640, 480 - VISTA_H};
    lamiera::lamiera(t, p, 7, 8);
    const std::string titolo = testo("NAVIGAZIONE  TORPEX '89  1,5X", "SOZH  SCROLLATEVELO DI DOSSO  1,5X");
    const std::string ora = testo("ORA 10:01:00Z  TEMPO X1", "ORA 06:01:00Z  TEMPO X1");
    incisa_su(t, o, 8, p.y + 6, titolo, CIANO_CHIARO);
    incisa_su(t, o, 640 - 8 - largo(o, ora), p.y + 6, ora, CIANO_CHIARO);
    // lo stato: spie per pilota, proiezione, scie; vetri per punti e ingrandimento
    const Rettangolo ps{6, p.y + 20, 300, 44};
    lamiera::pannello(t, ps, 8);
    struct Spia { const char* nome; bool accesa; };
    const Spia spie[] = {{"PILOTA", false}, {"PROIEZ. 10 MIN", false}, {"SCIE", false}};
    int x = ps.x + 8;
    for (const auto& s : spie) {
        lamiera::spia_voce(t, {x, ps.y + 8, 12, 9}, ColoreLampada::verde, s.accesa);
        seconda(t, o, x + 16, ps.y + 9, s.nome);
        x += 26 + largo(o, s.nome);
    }
    seconda(t, o, ps.x + 8, ps.y + 26, "PUNTI DI ROTTA");
    lamiera::vetro(t, {ps.x + 112, ps.y + 23, 44, 14});
    scrivi(t, o, ps.x + 118, ps.y + 26, "0/9", fosforo(6));
    seconda(t, o, ps.x + 166, ps.y + 26, "SCALA");
    lamiera::vetro(t, {ps.x + 214, ps.y + 23, 30, 14});
    scrivi(t, o, ps.x + 222, ps.y + 26, "3", fosforo(6));
    // il bersaglio
    const Rettangolo pb{310, p.y + 20, 324, 44};
    lamiera::pannello(t, pb, 8);
    seconda(t, o, pb.x + 8, pb.y + 7, "BERSAGLIO");
    lamiera::vetro(t, {pb.x + 8, pb.y + 19, pb.w - 16, 17});
    scrivi(t, o, pb.x + 14, pb.y + 24, "NESSUNO", fosforo(4));
    // i comandi: il tasto disegnato e la parola, su due file
    const struct { const char* k; const char* n; } cmd[] = {{"4", "RIGHELLO"}, {"5", "LINEE"}, {"6", "ANELLI"}, {"7", "SEGNI"},
                                                           {"8", "CPA"},      {"9", "T-RIL."}, {"Q", "EKELUND"}, {"E", "EVENTO"},
                                                           {"F", "AGGANCIA"}, {"B", "STILE"},  {"T", "MATE"}};
    int xc = 8, yc = p.y + 70;
    for (const auto& c : cmd) {
        const int w = 14 + 4 + largo(o, c.n) + 12;
        if (xc + w > 632) xc = 8, yc += 18;
        lamiera::tasto_tastiera(t, {xc, yc, 14, 12}, c.k, o);
        seconda(t, o, xc + 18, yc + 2, c.n);
        xc += w;
    }
    // la riga dell'equipaggio, come oggi
    lamiera::vetro(t, {4, 480 - 18, 632, 15});
    scrivi(t, o, 10, 480 - 14, testo("COMANDANTE, SONAR: NUOVO CONTATTO, RILEVAMENTO 293, DESIGNATO SIERRA 4.",
                                     "COMANDANTE, SONAR: NUOVO CONTATTO, RILEVAMENTO 283, DESIGNATO SIERRA 5."), fosforo(6));
}

}  // namespace

// --- la volta del ghiaccio (opzione A del sotto-ghiaccio; nucleo/volta_ghiaccio del disassemblatore), da approvare ---
// I dati d'esempio lungo la rotta, in metri dal battello (negativi a poppa): il pescaggio del ghiaccio con le chiglie,
// una polynya passata 7 minuti fa, il fondo; il battello con la chiglia a 60 m (la cima della vela a 44 m)
namespace volta {
constexpr double QUOTA = 60.0, CIMA_VELA = 44.0, MARGINE = 10.0;
struct Chiglia { double d, pescaggio, larga; };
const Chiglia CHIGLIE[] = {{-1500, 14, 60}, {-450, 9, 40}, {700, 12, 50}, {1250, 19, 70}, {2050, 38, 80}, {3300, 16, 60}, {5200, 24, 90}};
double pescaggio(double d) {
    if (d > -950 && d < -700) return 0.0;   // la polynya
    double p = 2.6 + 0.8 * std::sin(d / 137.0) + 0.5 * std::sin(d / 41.0);
    for (const auto& c : CHIGLIE) p = std::max(p, c.pescaggio * std::exp(-0.5 * std::pow((d - c.d) / c.larga, 2)));
    return p;
}
double fondo(double d) { return 285.0 + 25.0 * std::sin(d / 1300.0) + 8.0 * std::sin(d / 310.0); }
// la scala a radice: 30 m di ghiaccio e 300 di fondo nello stesso riquadro
double y_radice(double m, double scala, int y0, int h) { return y0 + std::sqrt(std::clamp(m / scala, 0.0, 1.0)) * (h - 1); }
}  // namespace volta

// la pagina del fondale con la volta sopra: a sinistra la volta a prua dal BQS-15 (pianta), a destra la carta termica
// col sondatore alto e il BQN-17, sotto la sezione di prua col pescaggio misurato in alto e il fondo in basso
void volta_fondale(Tela& t, const Fonti& f) {
    using namespace volta;
    const Font& g = f.fos5;
    lamiera::lamiera(t, {0, 0, 640, VISTA_H}, 7, 6);
    std::printf("CARTA %s %d %d %d %d\n", "volta_fondale", 0, VISTA_H, 640, 480 - VISTA_H);
    // 1) la volta a prua, in pianta: il settore del BQS-15 (portata effettiva 2,5 km), le chiglie viste, le celle del
    // quadro dietro (il sondatore alto lungo la scia), la polynya candidata con l'eta'
    const Rettangolo a = q::schermo(t, {4, 4, 400, 214}, g, "VOLTA A PRUA  BQS-15  2,5 KM", "QUADRO 200 M");
    // da 1 km a poppa a 2,6 km a prua; gli avvisi in alto a sinistra, dove il settore lascia posto
    const double m_px = 3700.0 / (a.h - 4);
    const int cx = a.x + a.w / 2, cy = a.y + a.h - 2 - static_cast<int>(1000.0 / m_px);
    const auto px = [&](double avanti, double lato) { return std::pair<int, int>{cx + static_cast<int>(lato / m_px), cy - static_cast<int>(avanti / m_px)}; };
    for (const double r : {1000.0, 2000.0, 2500.0})
        for (int gr = -45; gr <= 45; gr += 2) {
            const double a0 = gr * 3.14159265 / 180.0;
            const auto [x, y] = px(r * std::cos(a0), r * std::sin(a0));
            t.pixel(x, y, r == 2500.0 ? fosforo(4) : fosforo(2));
        }
    for (const int gr : {-45, 45})
        for (double r = 100; r < 2500; r += 60) {
            const double a0 = gr * 3.14159265 / 180.0;
            const auto [x, y] = px(r * std::cos(a0), r * std::sin(a0));
            t.pixel(x, y, fosforo(2));
        }
    for (const double r : {1000.0, 2000.0}) {   // i numeri degli anelli sul lato sinistro del settore, fuori dagli echi
        const auto [x, y] = px(r * std::cos(0.785), -r * std::sin(0.785));
        const std::string n = r == 1000.0 ? "1 KM" : "2 KM";
        scrivi(t, g, x - largo(g, n) - 3, y - 3, n, fosforo(3));
    }
    // le celle misurate dal sondatore alto lungo la scia (una fila larga una cella): piu' scure piu' spesse
    for (double d = -1000; d < 0; d += 200) {
        double pmax = 0;
        for (double k = d; k < d + 200; k += 10) pmax = std::max(pmax, pescaggio(k));
        const auto [x0, y0] = px(d + 200, -100);
        const auto [x1, y1] = px(d, 100);
        if (pmax <= 0.0) {   // vista libera: la polynya candidata
            for (int x = x0; x <= x1; ++x) t.pixel(x, y0, colori::CIANO_CHIARO), t.pixel(x, y1, colori::CIANO_CHIARO);
            for (int y = y0; y <= y1; ++y) t.pixel(x0, y, colori::CIANO_CHIARO), t.pixel(x1, y, colori::CIANO_CHIARO);
            scrivi(t, g, x1 + 4, y0 + 1, "LIBERA 7'  4/5", colori::CIANO_CHIARO);
            continue;
        }
        const int tono = std::clamp(1 + static_cast<int>(pmax / 5.0), 1, 6);
        for (int y = y0 + 1; y < y1; ++y)
            for (int x = x0 + 1; x < x1; ++x)
                if ((x + y) % 2 == 0 || tono > 4) t.pixel(x, y, fosforo(tono));
    }
    // i ritorni delle chiglie a prua: la tacca e, dai 15 m, il pescaggio; l'ostacolo (arriva alla vela meno il margine) in rosso
    struct Eco { double avanti, lato, pescaggio; };
    const Eco ECHI[] = {{700, -180, 12}, {1250, 240, 19}, {2050, -60, 38}, {1600, -620, 8}, {2300, 700, 17}, {900, 520, 6}, {1950, 420, 11}};
    for (const auto& e : ECHI) {
        const auto [x, y] = px(e.avanti, e.lato);
        const bool ostacolo = e.pescaggio >= CIMA_VELA - MARGINE;
        const std::uint8_t c = ostacolo ? std::uint8_t{12} : e.pescaggio >= 15 ? fosforo(7) : fosforo(5);
        t.linea_orizzontale(x - 3, x + 3, y, c);
        t.linea_verticale(x, y - 1, y + 1, c);
        if (e.pescaggio >= 15) scrivi(t, g, x + 5, y - 3, std::to_string(static_cast<int>(e.pescaggio)) + (ostacolo ? " M!" : ""), c);
    }
    // il battello e la rotta
    t.rettangolo_pieno(cx - 1, cy - 4, 3, 9, colori::VERDE_CHIARO);
    for (int y = cy - 6; y > a.y + 4; y -= 4) t.pixel(cx, y, fosforo(3));
    scrivi(t, g, a.x + 4, a.y + 2, "CHIGLIA A PRUA 2,1 KM", std::uint8_t{12});
    scrivi(t, g, a.x + 4, a.y + 13, "38 M: SOTTO LA VELA", std::uint8_t{12});
    scrivi(t, g, a.x + 4, a.y + 28, "VELOCITA' SICURA 9 KN", fosforo(5));
    scrivi(t, g, a.x + 4, a.y + a.h - 11, "A POPPA: IL SONDATORE ALTO", fosforo(3));
    // 2) la carta termica: il sondatore alto e il BQN-17 insieme, ultimi 30 minuti (il presente a destra)
    const std::uint8_t carta = smalto(11), quadretti = smalto(8), nero = colori::NERO;
    const Rettangolo b{412, 6, 222, 214};
    t.rettangolo_pieno(b.x, b.y, b.w, b.h, carta);
    scrivi(t, g, b.x + 4, b.y + 3, "VOLTA E FONDO  30 MIN", nero);
    const Rettangolo pb{b.x + 26, b.y + 14, b.w - 30, b.h - 28};
    for (const int m : {0, 10, 25, 50, 100, 200, 300}) {
        const int y = static_cast<int>(y_radice(m, 320, pb.y, pb.h));
        t.linea_orizzontale(pb.x, pb.x + pb.w - 1, y, quadretti);
        scrivi(t, g, pb.x - 3 - largo(g, std::to_string(m)), y - 3, std::to_string(m), nero);
    }
    for (int k = 0; k <= 6; ++k) t.linea_verticale(pb.x + k * (pb.w - 1) / 6, pb.y, pb.y + pb.h - 1, quadretti);
    // un campione ogni 10 s: 2 s di sondatore = ~10 m a 10 kn; la posizione lungo la rotta e' d = -velocita * dt
    int ya = -1, yf = -1, yq = -1;
    for (int x = 0; x < pb.w; ++x) {
        const double dt = (pb.w - 1 - x) / static_cast<double>(pb.w - 1) * 1800.0, d = -dt * 4.6;
        const double qd = dt > 1300 ? 45.0 : dt > 1000 ? 45.0 + (1300 - dt) / 300.0 * 15.0 : QUOTA;
        const int y1 = static_cast<int>(y_radice(pescaggio(d + 7000.0 * 0), 320, pb.y, pb.h));
        const int y2 = static_cast<int>(y_radice(fondo(d), 320, pb.y, pb.h));
        const int y3 = static_cast<int>(y_radice(qd, 320, pb.y, pb.h));
        const int xx = pb.x + x;
        if (pescaggio(d) <= 0.0) {   // acqua libera: il tratto resta in alto, con la tacca
            t.linea_verticale(xx, pb.y, pb.y + 3, colori::BLU);
        } else {
            t.linea_verticale(xx, pb.y, y1, smalto(6));   // il ghiaccio sopra la traccia, a mezzo tono
            if (ya >= 0) t.linea_verticale(xx, std::min(ya, y1), std::max(ya, y1), nero);
        }
        t.linea_verticale(xx, y2, pb.y + pb.h - 1, smalto(6));
        if (yf >= 0) t.linea_verticale(xx, std::min(yf, y2), std::max(yf, y2), nero);
        if (yq >= 0) t.linea_verticale(xx, std::min(yq, y3), std::max(yq, y3), colori::VERDE);
        ya = pescaggio(d) <= 0.0 ? -1 : y1, yf = y2, yq = y3;
    }
    scrivi(t, g, b.x + 4, b.y + b.h - 11, "SU: ALTO   GIU': BQN-17", nero);
    // 3) la sezione di prua: 2 km a poppa e 8 a prua; in alto il pescaggio (misurato pieno dal sondatore, a tacche
    // dal BQS-15, a puntini dove non si sa), la cima della vela e il margine, il battello; in basso il fondo
    const Rettangolo c = q::schermo(t, {4, 222, 632, VISTA_H - 224}, g, "SEZIONE DI PRUA  -2 / +8 KM  SCALA A RADICE", "VOLTA 38 M A 2,1 KM");
    const Rettangolo pc{c.x + 28, c.y + 2, c.w - 32, c.h - 4};
    const auto xs = [&](double d) { return pc.x + static_cast<int>((d + 2000.0) / 10000.0 * (pc.w - 1)); };
    const auto ys = [&](double m) { return static_cast<int>(y_radice(m, 320, pc.y, pc.h)); };
    for (const int m : {0, 10, 25, 50, 100, 200, 300}) {
        const int y = ys(m);
        for (int x = pc.x; x < pc.x + pc.w; x += 3) t.pixel(x, y, fosforo(1));
        scrivi(t, g, pc.x - 3 - largo(g, std::to_string(m)), std::clamp(y - 3, pc.y, pc.y + pc.h - 7), std::to_string(m), fosforo(4));
    }
    for (int x = pc.x; x < pc.x + pc.w; ++x) {
        const double d = -2000.0 + (x - pc.x) / static_cast<double>(pc.w - 1) * 10000.0;
        const double p = pescaggio(d);
        if (d <= 0.0) {   // misurato dal sondatore alto
            if (p <= 0.0) {
                t.pixel(x, pc.y, colori::CIANO_CHIARO), t.pixel(x, pc.y + 1, colori::CIANO_CHIARO);
            } else {
                t.linea_verticale(x, pc.y, ys(p), fosforo(3));
                t.pixel(x, ys(p), fosforo(7));
            }
        } else if (d <= 2500.0) {   // dal BQS-15: solo le chiglie, a tacche, il resto a puntini
            bool chiglia = false;
            for (const auto& k : CHIGLIE)
                if (k.d > 0 && std::fabs(d - k.d) < k.larga * 1.2) chiglia = true;
            if (chiglia) {
                const int y = ys(p);
                for (int yy = pc.y; yy <= y; yy += 2) t.pixel(x, yy, p >= CIMA_VELA - MARGINE ? colori::ROSSO : fosforo(3));
                t.pixel(x, y, p >= CIMA_VELA - MARGINE ? std::uint8_t{12} : fosforo(7));
            } else if (x % 3 == 0) {
                t.pixel(x, ys(3.0), fosforo(3));
            }
        } else if (x % 5 == 0) {   // oltre la portata: non si sa
            t.pixel(x, ys(3.0), fosforo(2));
        }
        // il fondo: misurato a poppa, la carta a tratti a prua
        const int yf2 = ys(fondo(d));
        if (d <= 0.0) t.linea_verticale(x, yf2, pc.y + pc.h - 1, fosforo(2)), t.pixel(x, yf2, fosforo(6));
        else if ((x / 3) % 2 == 0) t.pixel(x, ys(fondo(d) + 6.0), fosforo(4));
    }
    // la cima della vela col margine (giallo a tratti), la quota, il battello; la polynya con l'eta'
    for (int x = pc.x; x < pc.x + pc.w; x += 4) t.linea_orizzontale(x, x + 1, ys(CIMA_VELA - MARGINE), colori::GIALLO);
    t.linea_orizzontale(pc.x, pc.x + pc.w - 1, ys(QUOTA), colori::VERDE);
    const int xb = xs(0.0);
    t.rettangolo_pieno(xb - 8, ys(QUOTA) - 2, 17, 3, colori::VERDE_CHIARO);
    t.linea_verticale(xb + 1, ys(CIMA_VELA), ys(QUOTA) - 2, colori::VERDE_CHIARO);
    t.linea_verticale(xb + 2, ys(CIMA_VELA), ys(QUOTA) - 2, colori::VERDE_CHIARO);
    for (int y = pc.y; y < pc.y + pc.h; y += 3) t.pixel(xs(2500.0), y, fosforo(3));
    scrivi(t, g, xs(2500.0) + 3, pc.y + 2, "PORTATA BQS-15", fosforo(3));
    scrivi(t, g, xs(-830.0) - 20, ys(6.0), "LIBERA 7'", colori::CIANO_CHIARO);
    scrivi(t, g, xs(2050.0) + 8, ys(30.0), "38 M", std::uint8_t{12});
    scrivi(t, g, pc.x + pc.w - 4 - largo(g, "VELA - 10 M"), ys(CIMA_VELA - MARGINE) - 9, "VELA - 10 M", colori::GIALLO);
}

// la striscia della volta sulla carta, come le misure del fondale: lungo la scia il nastro del sondatore alto (bianco
// sottile, grigio spesso, nero le chiglie, ciano l'acqua libera), le chiglie viste a prua, le polynye candidate col
// cerchio e l'eta', la legenda. Disegnata su 255 (trasparente): lo script la mette sopra la carta di oggi
void volta_carta(Tela& t, const Fonti& f) {
    using namespace volta;
    const Font& g = f.fos5;
    t.rettangolo_pieno(0, 0, 640, 480, 255);
    // la scia: dal battello (320, 180) indietro verso SSO, 74 m per pixel come la carta di oggi a scala 3
    const double mpp = 74.0, rotta = 20.0 * 3.14159265 / 180.0;
    const double ax = std::sin(rotta), ay = -std::cos(rotta), lx = std::cos(rotta), ly = std::sin(rotta);
    const int bx = 320, by = 180;
    for (double d = -9000; d <= 0; d += 25) {
        const double p = pescaggio(d / 4.0 - 0.0);   // la scia lunga: la stessa volta compressa
        const std::uint8_t c = p <= 0.0 ? colori::CIANO_CHIARO : p < 3.5 ? colori::BIANCO : p < 9 ? colori::GRIGIO : colori::NERO;
        for (int k = -1; k <= 1; ++k) {
            const int x = bx + static_cast<int>(std::lround(d / mpp * ax + k * lx)), y = by + static_cast<int>(std::lround(d / mpp * ay + k * ly));
            t.pixel(x, y, c);
        }
    }
    // le polynye candidate: il cerchio ciano e l'eta'
    for (const auto& [d, eta] : {std::pair{-3300.0, "7'"}, std::pair{-7600.0, "41'"}}) {
        const int x = bx + static_cast<int>(d / mpp * ax), y = by + static_cast<int>(d / mpp * ay);
        for (const auto& [r, c] : {std::pair{8.0, colori::NERO}, std::pair{7.0, colori::CIANO_CHIARO}, std::pair{6.0, colori::CIANO_CHIARO}})
            for (int k = 0; k < 48; ++k)
                t.pixel(x + static_cast<int>(std::lround(r * std::cos(k * 0.1309))), y + static_cast<int>(std::lround(r * std::sin(k * 0.1309))), static_cast<std::uint8_t>(c));
        scrivi(t, g, x + 12, y - 2, std::string("LIBERA ") + eta, colori::NERO);   // l'ombra, sull'acqua della carta
        scrivi(t, g, x + 11, y - 3, std::string("LIBERA ") + eta, colori::CIANO_CHIARO);
    }
    // le chiglie a prua del BQS-15: tacche, l'ostacolo con la croce rossa e il pescaggio
    const struct { double avanti, lato, p; } K[] = {{700, -180, 12}, {1250, 240, 19}, {2050, -60, 38}, {2300, 700, 17}};
    for (const auto& k : K) {
        const int x = bx + static_cast<int>((k.avanti * ax + k.lato * lx) / mpp), y = by + static_cast<int>((k.avanti * ay + k.lato * ly) / mpp);
        if (k.p >= CIMA_VELA - MARGINE) {
            for (int i = -3; i <= 3; ++i) t.pixel(x + i, y + i, colori::ROSSO), t.pixel(x + i, y - i, colori::ROSSO);
            scrivi(t, g, x - 5 - largo(g, "38"), y - 3, "38", colori::ROSSO);   // a sinistra: a dritta c'e' la 17
        } else {
            t.linea_orizzontale(x - 2, x + 2, y, colori::NERO);
            if (k.p >= 15) scrivi(t, g, x + 4, y - 3, std::to_string(static_cast<int>(k.p)), colori::NERO);
        }
    }
    // la legenda, come il bollettino del meteo: un riquadro bianco col filo nero
    const Rettangolo l{22, 18, 196, 54};
    t.rettangolo_pieno(l.x, l.y, l.w, l.h, colori::BIANCO);
    t.linea_orizzontale(l.x, l.x + l.w - 1, l.y, colori::NERO), t.linea_orizzontale(l.x, l.x + l.w - 1, l.y + l.h - 1, colori::NERO);
    t.linea_verticale(l.x, l.y, l.y + l.h - 1, colori::NERO), t.linea_verticale(l.x + l.w - 1, l.y, l.y + l.h - 1, colori::NERO);
    scrivi(t, g, l.x + 6, l.y + 4, "VOLTA MISURATA", colori::NERO);
    const struct { std::uint8_t c; const char* testo; } voci[] = {{colori::GRIGIO, "SOTTILE / SPESSA / CHIGLIA"}, {colori::CIANO_CHIARO, "ACQUA LIBERA (ETA')"}, {colori::ROSSO, "CHIGLIA SOTTO LA VELA"}};
    int y = l.y + 16;
    for (const auto& v : voci) {
        if (v.c == colori::GRIGIO) {
            t.rettangolo_pieno(l.x + 6, y + 1, 4, 5, colori::BIANCO), t.linea_orizzontale(l.x + 6, l.x + 9, y + 1, colori::NERO);
            t.rettangolo_pieno(l.x + 11, y + 1, 4, 5, colori::GRIGIO), t.rettangolo_pieno(l.x + 16, y + 1, 4, 5, colori::NERO);
        } else {
            t.rettangolo_pieno(l.x + 6, y + 1, 14, 5, v.c);
        }
        scrivi(t, g, l.x + 24, y, v.testo, colori::NERO);
        y += 11;
    }
}

int main(int argc, char** argv) {
    if (argc < 3) {
        std::fprintf(stderr, "uso: %s <dati> <cartella d'uscita>\n", argv[0]);
        return 2;
    }
    const std::string dati = argv[1], fuori = argv[2];
    Palette p = Palette::carica(dati + "/palette/giorno.pal");
    cerimonia::imposta_palette(p, Marina::usn);
    applica_tema(p, TemaStazione{});
    const Fonti f{Font::carica(dati + "/font/originale.fnt"), Font::carica(dati + "/font/incisione688_7.fnt"),
                  Font::carica(dati + "/font/fosforo688_5x7.fnt"), Font::carica(dati + "/font/fosforo688_7x9.fnt")};
    // l'albo: le sole rampe di cerimonia, come nel gioco (i ritratti negli indici del tema)
    Palette pc = Palette::carica(dati + "/palette/giorno.pal");
    cerimonia::imposta_palette(pc, Marina::usn);
    PALETTE_TEMA = &p;
    const struct { const char* nome; void (*disegna)(Tela&, const Fonti&); bool cerimonia = false; } tavole[] = {
        {"statistiche_riepilogo_C", statistiche_riepilogo},
        {"statistiche_scoperte_C", statistiche_scoperte},
        {"replay_C", replay},
        {"vivo_C", vivo},
        {"briefing_A", briefing},
        {"debriefing_A", debriefing},
        {"serie_C", serie},
        {"museo_A", museo},
        {"statistiche_cumulate_C", statistiche_cumulate},
        {"albo_B", albo, true},
        {"fascicolo_piede_B", fascicolo_piede, true},
        {"bacheca_piede_B", bacheca_piede, true},
        {"esito_piede_B", esito_piede, true},
        {"editor_C", editor},
        {"editor_unita_C", editor_unita},
        {"editor_briefing_C", editor_briefing},
        {"radio_A", radio},
        {"manovra_C", manovra},
        {"armi_C", armi},
        {"carta_A", carta_pannello},
        {"volta_fondale_C", volta_fondale},
        {"volta_carta_A", volta_carta},
    };
    for (const auto& x : tavole) {
        Tela t(640, 480);
        t.riempi(0);
        x.disegna(t, f);
        t.salva_bmp(fuori + "/" + x.nome + ".bmp", x.cerimonia ? pc : p);
    }
    // le stazioni dell'Alfa: il suo tema e le scritte incise a stampino
    Palette pa = Palette::carica(dati + "/palette/giorno.pal");
    cerimonia::imposta_palette(pa, Marina::usn);
    TemaStazione tema_alfa;
    tema_alfa.battello = BattelloTema::alfa;
    applica_tema(pa, tema_alfa);
    const Fonti fa{Font::carica(dati + "/font/originale.fnt"), Font::carica(dati + "/font/stampino_urss_7.fnt"),
                   Font::carica(dati + "/font/fosforo688_5x7.fnt"), Font::carica(dati + "/font/fosforo688_7x9.fnt")};
    ALFA = true;
    const struct { const char* nome; void (*disegna)(Tela&, const Fonti&); } alfa[] = {
        {"radio_alfa_A", radio}, {"manovra_alfa_C", manovra}, {"armi_alfa_C", armi}, {"carta_alfa_A", carta_pannello}};
    for (const auto& x : alfa) {
        Tela t(640, 480);
        t.riempi(0);
        x.disegna(t, fa);
        t.salva_bmp(fuori + "/" + x.nome + ".bmp", pa);
    }
    ALFA = false;
    return 0;
}
